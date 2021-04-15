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
 * @file blink_array.c
 * @author Patrick Pedersen
 * @date 2021-04-09
 * @brief Blinks one or more WS2812 strips using a RGB array. 
 * 
 * See 
 * 
 * The following example showcases how the Tiny-WS2812 library can
 * be used on AVR platforms supporting the Arduino framework to blink
 * an entire WS2812 LED strip in white. In this rather memory expensive
 * example, we achieve this by simply creating a rgb array the size of 
 * the LED strip whose values we then transmit to the strip using the 
 * ws2812_tx() function.
 *
 * For a more memory efficient method, take a look at the blink_loop.c
 * example.
 * 
 * @note Please ensure that the WS2812_TARGET_PLATFORM_ARDUINO_AVR macro
 * is defined during compilation. This can either be done by specifying
 * `-DWS2812_TARGET_PLATFORM_ARDUINO_AVR` in the build flags, or by uncommenting
 * the `#define WS2812_TARGET_PLATFORM_ARDUINO_AVR` directive at the top of this file.
 *
 */

// #define WS2812_TARGET_PLATFORM_ARDUINO_AVR

#include <Arduino.h>
#include <ws2812.h>

// Parameters - ALTER THESE TO CORRESPOND WITH YOUR OWN SETUP!
#define STRIP_SIZE 8     ///< Size of your WS2812 strip(s)
#define DATA_PINS {8, 9} ///< Arduino pin(s) used to program the WS2812 strip(s). Must share same port! (See https://www.arduino.cc/en/Reference/PortManipulation)
#define RESET_TIME 50    ///< Reset time in microseconds (50us recommended by datasheet)
#define COLOR_ORDER grb  ///< Color order of your LED strips (Typically grb or rgb)

uint8_t pins[] = DATA_PINS;
ws2812_rgb strip[STRIP_SIZE]; ///< RGB array which represents the LED strip
ws2812_cfg cfg;               ///< Driver configuration

/**
 * Configures the WS2812 driver.
 */
void setup()
{
        // Configure the driver
        cfg.pins = pins;
        cfg.rst_time_us = RESET_TIME;
        cfg.order = COLOR_ORDER;
        cfg.n_strips = sizeof(pins); // Number of strips
        
        if (ws2812_config(cfg) != 0) {
                // HANDLE ERROR HERE
                void;
        }
}

/**
 * Continously blinks the entire WS2812 strip in white.
 */
void loop()
{
        // Program all LEDs to white
        for (unsigned int i = 0; i < STRIP_SIZE; i++) {
                strip[i].r = 255;
                strip[i].g = 255;
                strip[i].b = 255;              
        }

        ws2812_prep_tx();              // Prepare driver to transmit data
        ws2812_tx(strip, STRIP_SIZE);  // Transmit array of org values to strip
        ws2812_close_tx();             // Close transmission

        // Wait 500ms
        delay(500);

        // Program all LEDs to black (off)
        for (unsigned int i = 0; i < STRIP_SIZE; i++) {
                strip[i].r = 0;
                strip[i].g = 0;
                strip[i].b = 0;
        }

        ws2812_prep_tx();              // Prepare driver to transmit data
        ws2812_tx(strip, STRIP_SIZE);  // Transmit array of org values to strip
        ws2812_close_tx();             // Close transmission

        // Wait 500ms
        delay(500);
}