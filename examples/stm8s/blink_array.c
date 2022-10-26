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
  * @file blink_array.cxx
  * @author Patrick Pedersen
  * @date 2022-10-26
  * @brief Blinks one or more WS2812 devices using a RGB array. 
  * 
  * The following example showcases how the Tiny-WS2812 library can
  * be used on STM8S platforms to blink an entire WS2812 device in red. 
  * In this rather memory expensive example, we achieve this by simply 
  * creating a rgb array equal to the number of LEDs on the WS2812 device,
  * whose values we then transmit to the device using the ws2812_tx() function.
  *
  * For a more memory efficient method, take a look at the blink_loop.cxx
  * example.
  * 
  * @note Please ensure that the WS2812_TARGET_PLATFORM_STM8S macro
  * is defined during compilation. This can either be done by specifying
  * -DWS2812_TARGET_PLATFORM_STM8S in the build flags, or by uncommenting
  * the define WS2812_TARGET_PLATFORM_STM8S directive below.
  */

#include <stm8s.h>

#include <ws2812.h>

#define N_LEDS 8				///< Number of LEDs in the strip
#define DATA_PORT_BASE GPIOD_BaseAddress	///< Port base address of the data pin
#define DATA_PINS {GPIO_PIN_4}			///< Data pin(s) of the strip
#define RESET_TIME 30				///< Reset time in µs
#define COLOR_ORDER grb				///< Color order of the strip

#define WAIT_LOOPS 1000000

uint8_t pins[] = DATA_PINS;
ws2812_rgb leds[N_LEDS]; 
ws2812 ws2812_dev;

void init_16mhz_clk()
{
	CLK_DeInit();
	CLK_HSICmd(ENABLE);
	CLK_HSECmd(DISABLE);
	CLK_LSICmd(DISABLE);
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
	CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
}

void main()
{
	// Initialize clock to run at 16Mhz
	init_16mhz_clk();

	// Initialize WS2812 device struct

        ws2812_cfg cfg;

        cfg.pins		 	= pins;
        cfg.rst_time_us 		= RESET_TIME;
        cfg.order 			= COLOR_ORDER;
        cfg.n_dev 			= sizeof(pins);
	cfg.port_baseaddr 		= DATA_PORT_BASE;

	ws2812_config(&ws2812_dev, &cfg);
	
	// Blink strip

	while(1) {
		// Fill strip array with red
		for (unsigned int i = 0; i < N_LEDS; i++) {
			leds[i].r = 255;
			leds[i].g = 0;
			leds[i].b = 0;              
		}

		// Write to strip
		ws2812_prep_tx(&ws2812_dev);
		ws2812_tx(&ws2812_dev, leds, N_LEDS);
		ws2812_close_tx(&ws2812_dev);

		// Wait for some time
		for (unsigned long i = 0; i < WAIT_LOOPS; i++) {
			__asm__("nop");
		}

		// Fill strip array with black
		for (unsigned int i = 0; i < N_LEDS; i++) {
			leds[i].r = 0;
			leds[i].g = 0;
			leds[i].b = 0;              
		}

		// Wait for some time
		for (unsigned long i = 0; i < WAIT_LOOPS; i++) {
			__asm__("nop");
		}
	}
}

// See: https://community.st.com/s/question/0D50X00009XkhigSAB/what-is-the-purpose-of-define-usefullassert
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
	while (TRUE)
	{
	}
}
#endif