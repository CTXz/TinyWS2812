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
 * @file blink_array.cxx
 * @author Patrick Pedersen
 * @date 2022-10-26
 * 
 */

#include <stm8s.h>

#include <ws2812.h>

// Parameters - ALTER THESE TO CORRESPOND WITH YOUR OWN SETUP!
#define N_LEDS 8
#define DATA_PORT_BASE GPIOD_BaseAddress
#define DATA_PINS {GPIO_PIN_3}
#define RESET_TIME 50
#define COLOR_ORDER grb

uint8_t pins[] = DATA_PINS; ///< Data pins
ws2812_rgb leds[N_LEDS];    ///< RGB array which represents the LEDs 
ws2812 ws2812_dev;          ///< Device struct

#define GPIO_LED_BUILTIN_PORT GPIOB
#define GPIO_LED_BUILTIN GPIO_PIN_5

void main()
{
	CLK_DeInit();
	CLK_HSICmd(ENABLE);
	CLK_HSECmd(DISABLE);
	CLK_LSICmd(DISABLE);
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
	CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
	
	GPIO_Init(GPIO_LED_BUILTIN_PORT, GPIO_LED_BUILTIN, GPIO_MODE_OUT_PP_HIGH_FAST);
	GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);

        ws2812_cfg cfg; // Device config

        // Configure the WS2812 device struct
        cfg.pins = pins;
        cfg.rst_time_us = RESET_TIME;
        cfg.order = COLOR_ORDER;
        cfg.n_dev = sizeof(pins); // Number of devices driven by this struct
	cfg.port_baseaddr = DATA_PORT_BASE;

        if (ws2812_config(&ws2812_dev, &cfg) != 0) {
                // HANDLE ERROR HERE
                void;
        }
	
	uint8_t j = 0;
	while(1) {
		// Program all LEDs to white
		for (unsigned int i = 0; i < N_LEDS; i++) {
			leds[i].r = 0;
			leds[i].g = 0;
			leds[i].b = j;              
		}

		ws2812_prep_tx(&ws2812_dev);           // Prepare to transmit data
		ws2812_tx(&ws2812_dev, leds, N_LEDS);  // Transmit array of rgb values to the device
		ws2812_close_tx(&ws2812_dev);          // Close transmission

		for (unsigned long i = 0; i < 100000; i++) {
			__asm__("nop");
		}

		j++;
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