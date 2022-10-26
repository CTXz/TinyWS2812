/*
 * Copyright (C) 2022  Patrick Pedersen

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 */

/**
 * @file ws2812_stm8s.c
 * @author Patrick Pedersen
 * @date 2022-10-26
 * 
 * @brief Driver code for STM8S chips.
 * 
 * The following file holds the Tiny-WS2812 library code to drive 
 * WS2812 devices on STM8S chips.
 */

#ifdef WS2812_TARGET_PLATFORM_STM8S

#include <stm8s.h>
#include <stddef.h>
#include <stdbool.h>

#include <ws2812_common.h>
#include <ws2812_stm8s.h>

// Ensure that CPU clock runs at 16 MHz
#if F_CPU != 16000000UL
#error "F_CPU must be 16MHz!"
#endif

// Helper constants for time critical stuff
#define TICKS_PER_LOOP 2					///< Number of CPU ticks per loop in delay_us function
#define LOOPS_PER_US (F_CPU / TICKS_PER_LOOP / 1000000UL)	///< Number for loops required for 1 us to pass
#define LDW_OVERHEAD 2						///< Number of CPU ticks for the LDW instruction

// GPIO masks to quickly drive the WS2812 data line
volatile static uint16_t _port_odr_addr;	///< Address of the ODR register
volatile static uint8_t _mask_hi;		///< Mask for high state
volatile static uint8_t _mask_lo;		///< Mask for low state

static bool _prep = false;			///< Flag to indicate if the driver is prepared

// Decrementing coutner for the delay_us function
volatile static uint16_t _us_loops_remaining;

/**
 * @brief Halts the program for a given ammount of microseconds.
 * 
 * The following function temporarily disables interrupts and pauses the program
 * code for a provided ammount of microseconds (max 255).
 * 
 * @param us Number of microseconds to pause for.
 * 
 * @warning This function is blocking, meaning it reserves the CPU from performing any other tasks.
 */
static void delay_us(uint8_t us)
{
	disableInterrupts();

	_us_loops_remaining = (us * LOOPS_PER_US) - LDW_OVERHEAD;

	__asm
		ldw x, __us_loops_remaining	// 2 Cycles
	0000$:
		decw x				// 1 Cycle
		jrne 0000$			// 1-2 Cycles

	__endasm;

	enableInterrupts();
}

// Refer to header for documentation
uint8_t ws2812_config(ws2812 *dev, ws2812_cfg *cfg)
{
	dev->port_baseaddr = cfg->port_baseaddr;
	dev->rst_time_us = cfg->rst_time_us;

	GPIO_TypeDef *port = (GPIO_TypeDef *)dev->port_baseaddr;

	uint8_t pin_msk = 0;

	for (uint8_t i = 0; i < cfg->n_dev; i++) {
		pin_msk |= cfg->pins[i];
		GPIO_Init(port, cfg->pins[i], GPIO_MODE_OUT_PP_LOW_FAST);
	}

	dev->maskhi = pin_msk;
	dev->masklo = ~pin_msk;

	_ws2812_get_rgbmap(&dev->rgbmap, cfg->order);

	return 0;
}

// Refer to header for documentation
void ws2812_prep_tx(ws2812 *dev)
{
	if (_prep == true)
		return;

	_port_odr_addr = dev->port_baseaddr;
	_mask_hi = dev->maskhi;
	_mask_lo = dev->masklo;
	
	_prep = true;

	disableInterrupts();
}

// Refer to header for documentation
void ws2812_wait_rst(ws2812 *dev)
{
	delay_us(dev->rst_time_us);
}

/**
 * @brief Transmitts a 1 bit to the WS2812 device.
 * 
 * The following function transmits a 1 bit to the WS2812 device.
 * This is done in inline assembly to ensure that the timing is
 * kept up.
 * 
 * To transmitt a 1 bit, the data line is pulled high for 700ns,
 * then pulled low for 600ns, each allowing for a 150ns margin.
 * 
 * Since we are running at 16 MHz, that would approximately be
 * 11 - 12 CPU ticks for the high state and 9 - 10 CPU ticks for the low
 * state. The "correct" ammount has been brute forced through trail and error.
 * 
 * The instructions bellow toggle the data line and waits approximately
 * the required time for the high and low state as provided by the
 * datasheet.
 * 
 * To prevent timing inconsistencies due to pipelining, the function
 * must not be made inline, as the function call flushes the pipeline.
 * 
 * Due to the 150ns margin, the function can be looped thus thankfully
 * allowing us to abstract the rest in c code.
 * 
 * @warning This function is blocking, meaning it reserves the CPU from performing any other tasks.
 */
static void ws2812_tx_bit_1() // DO NOT INLINE TO PREVENT PIPELINING
{
	__asm
		ldw x, __port_odr_addr	// Load address of ODR register into X - 2 Cycles
		ld a, (x)		// Load content of ODR register into A - 1 Cycle
		or a, __mask_hi		// Set data line pin high using the pin mask - 1 Cycle
		ld (x), a		// Apply changes to ODR register - 1 Cycle
		nop			// Waste cycles until ~700ns have passed (nops are 1 cycle each)
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		and a, __mask_lo	// Set data line pin low using the pin mask - 1 Cycle
		ld (x), a		// Apply changes to ODR register - 1 Cycle
		nop			// Waste cycles until ~600ns have passed (nops are 1 cycle each)
		nop
		nop
		nop
		nop
		nop
		nop
	__endasm;
}

/**
 * @brief Transmitts a 1 bit to the WS2812 device.
 * 
 * The following function transmits a 0 bit to the WS2812 device.
 * This is done in inline assembly to ensure that the timing is
 * kept up.
 * 
 * To transmitt a 0 bit, the data line is pulled high for 350ns,
 * then pulled low for 800ns, each allowing for a 150ns margin.
 * 
 * Since we are running at 16 MHz, that would approximately be
 * 5 - 6 CPU ticks for the high state and 12 - 13 CPU ticks for the low
 * state. The "correct" ammount has been brute forced through trail and error.
 * 
 * The instructions bellow toggle the data line and waits approximately
 * the required time for the high and low state as provided by the
 * datasheet.
 * 
 * To prevent timing inconsistencies due to pipelining, the function
 * must not be made inline, as the function call flushes the pipeline.
 * 
 * Due to the 150ns margin, the function can be looped thus thankfully
 * allowing us to abstract the rest in c code.
 * 
 * @warning This function is blocking, meaning it reserves the CPU from performing any other tasks.
 */
static void ws2812_tx_bit_0() // DO NOT INLINE TO PREVENT PIPELINING
{
	__asm
		ldw x, __port_odr_addr
		ld a, (x)
		or a, __mask_hi
		ld (x), a
		nop
		nop
		and a, __mask_lo
		ld (x), a
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
	__endasm;
}

/**
 * @brief Transmits a byte of data to the WS2812 device.
 * 
 * The following function transmits a single byte of data to the WS2812 device.
 * This is done by looping through the bits of the byte and calling the
 * appropriate ws2812_tx_bit_* function.
 * 
 * @param byte The byte to be transmitted.
 */
static inline void ws2812_tx_byte(uint8_t byte)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if (byte & 0b10000000) // Tests every bit in the byte
			ws2812_tx_bit_1();
		else
			ws2812_tx_bit_0();

		byte <<= 1;
	}
}

// Refer to header for documentation
void ws2812_tx(ws2812 *dev, ws2812_rgb *leds, size_t n_leds)
{
        for (size_t i = 0; i < n_leds; i++) {
                for (uint8_t j = 0; j < sizeof(dev->rgbmap); j++) {
                        uint8_t *pxl = (uint8_t *) &(leds[i]);
                        ws2812_tx_byte(pxl[dev->rgbmap[j]]);
                }
        }
}

// Refer to header for documentation
void ws2812_close_tx(ws2812 *dev)
{
	if (_prep == false)
		return;

	enableInterrupts();
	_prep = false;
        ws2812_wait_rst(dev);
}

#endif