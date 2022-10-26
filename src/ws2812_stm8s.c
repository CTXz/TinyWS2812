#ifdef WS2812_TARGET_PLATFORM_STM8S

#include <stm8s.h>
#include <stddef.h>
#include <stdbool.h>

#include <ws2812_common.h>
#include <ws2812_stm8s.h>

#if F_CPU != 16000000UL
#error "F_CPU must be 16MHz!"
#endif

#define TICKS_PER_LOOP 2
#define LOOPS_PER_US (F_CPU / TICKS_PER_LOOP / 1000000UL)
#define LDW_OVERHEAD 2

volatile static uint16_t _port_odr_addr;
volatile static uint8_t _mask_hi;
volatile static uint8_t _mask_lo;

volatile static uint16_t _us_loops_remaining;

static bool _prep = false;

static void delay_us(uint16_t us)
{
	_us_loops_remaining = (us * LOOPS_PER_US) - LDW_OVERHEAD;

	__asm
		ldw x, __us_loops_remaining	// 2 Cycles
	0000$:
		decw x				// 1 Cycle
		jrne 0000$			// 1-2 Cycles

	__endasm;
}

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

void ws2812_wait_rst(ws2812 *dev)
{
	delay_us(dev->rst_time_us);
}

static void ws2812_tx_bit_1() // DO NOT INLINE TO PREVENT PIPELINING
{
	__asm
		ldw x, __port_odr_addr
		ld a, (x)
		or a, __mask_hi
		ld (x), a
		nop
		nop
		nop
		nop
		nop
		nop
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
	__endasm;
}

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

void ws2812_tx(ws2812 *dev, ws2812_rgb *leds, size_t n_leds)
{
        for (size_t i = 0; i < n_leds; i++) {
                for (uint8_t j = 0; j < sizeof(dev->rgbmap); j++) {
                        uint8_t *pxl = (uint8_t *) &(leds[i]);
                        ws2812_tx_byte(pxl[dev->rgbmap[j]]);
                }
        }
}

void ws2812_close_tx(ws2812 *dev)
{
	if (_prep == false)
		return;

	enableInterrupts();
	_prep = false;
        ws2812_wait_rst(dev);
}

#endif