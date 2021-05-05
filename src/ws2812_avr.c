/*
 * Copyright (C) 2021  Patrick Pedersen

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
 * @file ws2812_avr.c
 * @author Patrick Pedersen
 * @date 2021-04-09
 * 
 * @brief Driver code for AVR chips.
 * 
 * The following file holds the Tiny-WS2812 library code to drive 
 * WS2812 devices on AVR chips.
 */

#if defined(WS2812_TARGET_PLATFORM_AVR) || defined(WS2812_TARGET_PLATFORM_ARDUINO_AVR)

#include <stdint.h>
#include <stdbool.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include <ws2812.h>

#ifdef WS2812_TARGET_PLATFORM_ARDUINO_AVR
#include <Arduino.h>
#endif

// Timing in ns
#define w_zeropulse   350
#define w_onepulse    900
#define w_totalperiod 1250

// Fixed cycles used by the inner loop
#define w_fixedlow    3
#define w_fixedhigh   6
#define w_fixedtotal  10   

// Insert NOPs to match the timing, if possible
#define w_zerocycles    (((F_CPU/1000)*w_zeropulse          )/1000000)
#define w_onecycles     (((F_CPU/1000)*w_onepulse    +500000)/1000000)
#define w_totalcycles   (((F_CPU/1000)*w_totalperiod +500000)/1000000)

// w1 - nops between rising edge and falling edge - low
#define w1 (w_zerocycles-w_fixedlow)
// w2   nops between fe low and fe high
#define w2 (w_onecycles-w_fixedhigh-w1)
// w3   nops to complete loop
#define w3 (w_totalcycles-w_fixedtotal-w1-w2)

#if w1>0
  #define w1_nops w1
#else
  #define w1_nops  0
#endif

// The only critical timing parameter is the minimum pulse length of the "0"
// Warn or throw error if this timing can not be met with current F_CPU settings.
#define w_lowtime ((w1_nops+w_fixedlow)*1000000)/(F_CPU/1000)
#if w_lowtime>550
   #error "Light_ws2812: Sorry, the clock speed is too low. Did you set F_CPU correctly?"
#elif w_lowtime>450
   #warning "Light_ws2812: The timing is critical and may only work on WS2812B, not on WS2812(S)."
   #warning "Please consider a higher clockspeed, if possible"
#endif   

#if w2>0
#define w2_nops w2
#else
#define w2_nops  0
#endif

#if w3>0
#define w3_nops w3
#else
#define w3_nops  0
#endif

#define w_nop1  "nop      \n\t"
#define w_nop2  "rjmp .+0 \n\t"
#define w_nop4  w_nop2 w_nop2
#define w_nop8  w_nop4 w_nop4
#define w_nop16 w_nop8 w_nop8

static uint8_t _sreg_prev;
static bool _prep = false;

#ifdef WS2812_TARGET_PLATFORM_AVR
/**
 * @brief Halts the program for a given ammount of microseconds.
 * 
 * The following function temporarily disables interrupts and pauses the program
 * code for a provided ammount of microseconds (max 255).
 * 
 * @warning This function relies on the _delay_us() function, which reserves
 *      the CPU from performing any other tasks.
 * @warning Since the for loop of this function also requires time to be executed,
 *      the actual delay will always be slighly longer. 
 */
void delay_us(uint8_t us)
{
        cli();
        for (uint8_t i = 0; i < us; i++)
                _delay_us(1);
        sei();
}
#endif

// Refer to header for documentation
uint8_t ws2812_config(ws2812 *dev, ws2812_cfg config)
{
        if (config.n_dev == 0)
                return 1; // No devices to be driven!
        
        uint8_t pin_msk = 0;

#ifdef WS2812_TARGET_PLATFORM_ARDUINO_AVR
        for (uint8_t i = 0; i < config.n_dev; i++) {
                if (i+1 < config.n_dev &&
                    digitalPinToPort(config.pins[i]) != digitalPinToPort(config.pins[i+1]))
                        return 2; // Pins do not share same port!
                pinMode(config.pins[i], OUTPUT);
                pin_msk |= digitalPinToBitMask(config.pins[i]);
        }
        dev->port = portOutputRegister(digitalPinToPort(config.pins[0]));
#else
        for (uint8_t i = 0; i < config.n_dev; i++)
                pin_msk |= 1 << config.pins[i];

        *config.ddr = pin_msk;
        dev->port = config.port;
#endif
        dev->rst_time_us = config.rst_time_us;
        dev->masklo = ~pin_msk & *(dev->port);
        dev->maskhi = pin_msk | *(dev->port);
        
        _ws2812_get_rgbmap(&dev->rgbmap, config.order);
        
        return 0;
}

// Refer to header for documentation
void ws2812_prep_tx(ws2812 *dev)
{
        if (_prep == false) {
                _sreg_prev = SREG;
                cli();
                _prep = true;
        }
}

// Refer to header for documentation
void ws2812_wait_rst(ws2812 *dev)
{
#ifdef WS2812_TARGET_PLATFORM_ARDUINO_AVR
        delayMicroseconds(dev->rst_time_us);
#else
        delay_us(dev->rst_time_us);
#endif
}

// Disable optimizations as they may interfere with timing 
#pragma GCC push_options
#pragma GCC optimize ("O0")

/**
 * @brief Transmits a byte of data to the \ref ws2812 "WS2812 device".
 * 
 * The following function transmits a single byte of data to the provided \ref ws2812 "WS2812 device".
 * It is primarily based on the driver code of [cpldcpu's light_ws2812](https://github.com/cpldcpu/light_ws2812)
 * library and achieves precisely timed communication with the WS2812 device
 * through inline AVR assembly code.
 * 
 */
void ws2812_tx_byte(ws2812 *dev, uint8_t byte)
{
        uint8_t ctr;

        asm volatile(
                "       ldi   %0,8  \n\t"
                "loop%=:            \n\t"
                "       st    X,%3 \n\t"    //  '1' [02] '0' [02] - re
                #if (w1_nops&1)
                w_nop1
                #endif
                #if (w1_nops&2)
                w_nop2
                #endif
                #if (w1_nops&4)
                w_nop4
                #endif
                #if (w1_nops&8)
                w_nop8
                #endif
                #if (w1_nops&16)
                w_nop16
                #endif
                "       sbrs  %1,7  \n\t"    //  '1' [04] '0' [03]
                "       st    X,%4 \n\t"     //  '1' [--] '0' [05] - fe-low
                "       lsl   %1    \n\t"    //  '1' [05] '0' [06]
                #if (w2_nops&1)
                w_nop1
                #endif
                #if (w2_nops&2)
                w_nop2
                #endif
                #if (w2_nops&4)
                w_nop4
                #endif
                #if (w2_nops&8)
                w_nop8
                #endif
                #if (w2_nops&16)
                w_nop16 
                #endif
                "       brcc skipone%= \n\t"    //  '1' [+1] '0' [+2] - 
                "       st   X,%4      \n\t"    //  '1' [+3] '0' [--] - fe-high
                "skipone%=:               "     //  '1' [+3] '0' [+2] - 

                #if (w3_nops&1)
                w_nop1
                #endif
                #if (w3_nops&2)
                w_nop2
                #endif
                #if (w3_nops&4)
                w_nop4
                #endif
                #if (w3_nops&8)
                w_nop8
                #endif
                #if (w3_nops&16)
                w_nop16
                #endif
                "       dec   %0    \n\t"    //  '1' [+4] '0' [+3]
                "       brne  loop%=\n\t"    //  '1' [+5] '0' [+4]
                :	"=&d" (ctr)
                :	"r" (byte), "x" ((uint8_t *) dev->port), "r" (dev->maskhi), "r" (dev->masklo)
        );
}

#pragma GCC pop_options

// Refer to header for documentation
void ws2812_tx(ws2812 *dev, ws2812_rgb *pxls, size_t n_pxls)
{
        for (size_t i = 0; i < n_pxls; i++) {
                for (uint8_t j = 0; j < sizeof(dev->rgbmap); j++) {
                        uint8_t *pxl = (uint8_t *) &(pxls[i]);
                        ws2812_tx_byte(dev, pxl[dev->rgbmap[j]]);
                }
        }
}

// Refer to header for documentation
void ws2812_close_tx(ws2812 *dev)
{
        if (_prep == true) {
                SREG = _sreg_prev;
                sei();
                _prep = false;
                ws2812_wait_rst(dev);
        }
}

#endif