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
 * @file blink_cpp.cpp
 * @author Patrick Pedersen
 * @date 2021-04-09
 * 
 * @brief C++ Wrapper implementation of blink_array.c 
 * 
 * This example showcases the @ref blink_array.c "blink array example" on 
 * AVR platforms using the C++ wrapper of the Tiny-WS2812 driver.
 * The purpose of this example is to merely showcase the differences between the
 * C++ wrapper and the C interface. For a more detailed description of the
 * functionality, consult the @ref blink_array.c "blink array example written in the C interface".
 * 
 * @note Please ensure that the WS2812_TARGET_PLATFORM_AVR macro
 * is defined during compilation. This can either be done by specifying
 * -DWS2812_TARGET_PLATFORM_AVR in the build flags, or by uncommenting
 * the define WS2812_TARGET_PLATFORM_AVR directive below.
 */

// #define WS2812_TARGET_PLATFORM_AVR

#include <avr/io.h>
#include <util/delay.h>

#include <ws2812_cpp.h>

// Parameters - ALTER THESE TO CORRESPOND WITH YOUR OWN SETUP!
#define N_LEDS 8                ///< Number of LEDs on your WS2812 device(s)
#define DATA_PINS_PORT PORTB    ///< Port register used to communicate with the WS2812 device(s)
#define DATA_PINS_DDR DDRB      ///< Data direction register of the pin(s) used to communicate with the WS2812 device(s)
#define DATA_PINS {PB0, PB1}    ///< Pin(s) used to communicate with the WS2812 device(s)
#define RESET_TIME 50           ///< Reset time in microseconds (50us recommended by datasheet)
#define COLOR_ORDER grb         ///< Color order of your WS2812 LEDs (Typically grb or rgb)

/**
 * Blinks one or more WS2812 device(s)
 */
int main()
{
        uint8_t pins[] = DATA_PINS;       // Data pins
        ws2812_rgb leds[N_LEDS];          // RGB array which represents the LEDs
        ws2812_cfg cfg;                   // Device configurationn

        // Configure the WS2812 device struct
        cfg.port = &DATA_PINS_PORT;
        cfg.ddr = &DATA_PINS_DDR;
        cfg.pins = pins;
        cfg.n_dev = sizeof(pins);         // Number of devices driven by this struct
        cfg.rst_time_us = RESET_TIME;
        cfg.order = grb;
        
        uint8_t ret;
        ws2812_cpp ws2812_dev(&cfg, &ret); // Initialize device struct

        if (ret != 0) {
                // HANDLE ERROR...
                void;
        };

        // Blink device
        while (1) {
                // Program all LEDs to white
                for (unsigned int i = 0; i < N_LEDS; i++) {
                        leds[i].r = 255;
                        leds[i].g = 255;
                        leds[i].b = 255;              
                }

                ws2812_dev.prep_tx();         // Prepare to transmit data
                ws2812_dev.tx(leds, N_LEDS);  // Transmit array of rgb values to the device
                ws2812_dev.close_tx();        // Close transmission

                // Wait 500ms
                _delay_ms(500);

                // Program all LEDs to black (off)
                for (unsigned int i = 0; i < N_LEDS; i++) {
                        leds[i].r = 0;
                        leds[i].g = 0;
                        leds[i].b = 0;
                }

                ws2812_dev.prep_tx();        // Prepare to transmit data
                ws2812_dev.tx(leds, N_LEDS); // Transmit array of rgb values to the device
                ws2812_dev.close_tx();       // Close transmission

                // Wait 500ms
                _delay_ms(500);
        }

        return 0;
}