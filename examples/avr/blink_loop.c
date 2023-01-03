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
 * @file blink_loop.c
 * @author Patrick Pedersen
 * @date 2021-04-09
 * 
 * @brief Blinks one or more WS2812 devices using a more memory efficient method than the blink_array.c example.
 * 
 * The following example showcases how the Tiny-WS2812 library can
 * be used on AVR platforms to blink an entire WS2812 device in white.
 * Unlike the other blink example, we do not allocate a memory expensive
 * array for every single LED here, but instead loop the transmission of
 * a single RGB value until all WS2812 LEDs have been set.
 * 
 * The advantage of this method is that we can save allot of memory, the
 * disadvantage is that this method is more prone to programming mistakes.
 * 
 * @note Please ensure that the WS2812_TARGET_PLATFORM_AVR macro
 * is defined during compilation. This can either be done by specifying
 * -DWS2812_TARGET_PLATFORM_AVR in the build flags, or by uncommenting
 * the define WS2812_TARGET_PLATFORM_AVR directive below.
 */

// #define WS2812_TARGET_PLATFORM_AVR

#include <avr/io.h>
#include <util/delay.h>

#include <ws2812.h>

// Parameters - ALTER THESE TO CORRESPOND WITH YOUR OWN SETUP!
#define N_LEDS 8                ///< Number of LEDs on your WS2812 device(s)
#define DATA_PINS_PORT PORTB    ///< Port register used to communicate with the WS2812 device(s)
#define DATA_PINS_DDR DDRB      ///< Data direction register of the pin(s) used to communicate with the WS2812 device(s)
#define DATA_PINS {PB0, PB1}    ///< Pin(s) used to communicate with the WS2812 device(s)
#define RESET_TIME 50           ///< Reset time in microseconds (50us recommended by datasheet)
#define COLOR_ORDER grb         ///< Color order of your WS2812 LEDs (Typically grb or rgb)

// Colors
ws2812_rgb white = {255,255,255};
ws2812_rgb black = {0, 0 ,0};

/**
 * Blinks one or more WS2812 device(s)
 */
int main()
{
        uint8_t pins[] = DATA_PINS; // Data pins
        ws2812_cfg cfg;             // Device configurationn
        ws2812 ws2812_dev;          // Device struct

        // Configure the WS2812 device struct
        cfg.port = &DATA_PINS_PORT;
        cfg.ddr = &DATA_PINS_DDR;
        cfg.pins = pins;
        cfg.n_dev = sizeof(pins);     // Number of devices driven by this struct
        cfg.rst_time_us = RESET_TIME;
        cfg.order = grb;
        
        if (ws2812_config(&ws2812_dev, &cfg) != 0) {
                // HANDLE ERROR...
                void;
        };

        // Blink device
        while (1) {
                // Prepare to transmit data
                ws2812_prep_tx(&ws2812_dev);

                // Program all LEDs to white
                for (unsigned int i = 0; i < N_LEDS; i++) {
                        ws2812_tx(&ws2812_dev, &white, sizeof(white)/sizeof(ws2812_rgb));
                        // THIS LOOP NEEDS TO RUN UNINTERRUPTED!
                }
                
                // Close transmission
                ws2812_close_tx(&ws2812_dev);

                // Wait 500ms
                _delay_ms(500);

                // Prepare to transmit data
                ws2812_prep_tx(&ws2812_dev);

                // Program all LEDs to black (off)
                for (unsigned int i = 0; i < N_LEDS; i++) {
                        ws2812_tx(&ws2812_dev, &black, sizeof(black)/sizeof(ws2812_rgb));
                        // THIS LOOP NEEDS TO RUN UNINTERRUPTED!
                }
                
                // Close transmission
                ws2812_close_tx(&ws2812_dev);

                // Wait 500ms
                _delay_ms(500);
        }

        return 0;
}