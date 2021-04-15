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
 * @file ws2812.h
 * @author Patrick Pedersen
 * @date 2021-04-09
 * 
 * @brief Exposes the Tiny-WS2812 driver interface.
 * 
 * The Tiny-WS2812 driver interface initially derives from the
 * driver code of an open source WS2812 LED controller that I had
 * worked on prior, and provides a nearly barebone interface to
 * communicate with WS2812 LED strips.
 * 
 * The following platforms and frameworks are currently supported:
 *      - Barebone AVR
 *      - The Arduino Framework (Currently only AVR based (eg. Uno, Leonardo, Micro...))
 * 
 * It has been developed out of the necessity to have an extremely light 
 * weight and flexible cross-platform driver that can be further abstracted
 * and used troughout my WS2812 projects, particullary on MCUs with severe 
 * memory constraints (ex. ATTiny chips), where one cannot just define an RGB
 * array equivalent to the size of the LED strip. This drivers purpose is **NOT**
 * to provide fancy abstractions and functions for color correction, brightness 
 * settings, animations etc.
 * 
 * To summerize, this driver is inteded to:
 *      - be used on MCUs with limited computing resources.
 *      - act as a base for more abstract WS2812 libraries.
 *      - be easily portable to other platforms or programming frameworks (ex. Arduino).
 *
 * @note Because the motivation of this driver is to be as barebone as possible,
 *      it relies on a superficial understanding of the WS2812 protocol and may
 *      demand an understanding of the host platforms platform (eg. registers etc.).
 *      For quick and simple programming of WS2812 strips, where memory and processing
 *      power are not a big issue, other drivers/libraries should probably be consulted.  
 */

#pragma once

#ifdef WS2812_TARGET_PLATFORM_AVR
#ifdef _WS2812_TARGET_PLATFORM_DEFINED
#error "Multiple target platforms defined!"
#endif
#define _WS2812_TARGET_PLATFORM_DEFINED
#endif

#ifdef WS2812_TARGET_PLATFORM_ARDUINO_AVR
#ifdef _WS2812_TARGET_PLATFORM_DEFINED
#error "Multiple target platforms defined!"
#endif
#define _WS2812_TARGET_PLATFORM_DEFINED
#endif

#ifndef _WS2812_TARGET_PLATFORM_DEFINED
#error "No target platform defined!"
#endif

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Enum to tell the driver about the WS2812's color order.
 *
 * Unfortunately there is no clear standard on sequential color
 * order of WS2812 chips, some are intuitively programmed in
 * RGB order, however, many, if not the majority, of available 
 * WS2812 chips are programmed in GRB oder. As there is automatic
 * way to identify the correct color order, it must be manually
 * provided in the driver configuration with this enum.
 */
typedef enum {
        rgb,
        rbg,
        brg,
        bgr,
        grb,
        gbr
} ws2812_order;

#pragma pack(1)
/**
 * @brief Data structure to hold RGB color values.
 *
 * The RGB struct holds the red, green and blue values
 * to define colors. It is used by the ws2812_tx() function 
 * to set the color of one or more WS2812 LEDs.
 * 
 * @note It should be noted that WS2812 LEDs typically suffer
 *      from poor color accuracy. As an example, the typical RGB
 *      value for orange (255,165,0) displays a color closer to 
 *      yellow on my WS2812 strip.  
 *
 */
typedef struct ws2812_rgb {
        uint8_t r,g,b;
} ws2812_rgb;

/**
 * @brief Data structure to configure the WS2812 driver.
 *
 * The following data structure is used to configure the WS2812 driver.
 * It is taken as an argument by the ws2812_config() function.
 * 
 * @warning All fields of the configuration object must be defined before passing it to #ws2812_config()!
 *      Leaving a field undefined will result in undefined behaivor!
 */
typedef struct ws2812_cfg {

#ifdef WS2812_TARGET_PLATFORM_AVR
        volatile uint8_t *port;   ///< PORT Register (ex. PORTB, PORTC, PORTD...) 
        volatile uint8_t *ddr;    ///< Data Direction Register (ex. DDRB, DDRC, DDRD...)
        uint8_t *pins;            ///< Array of pins used to program WS2812 strips (**Must share the same PORT!** (ex. PB0, PB1, PB2))
        uint8_t n_strips;         ///< Number of pins used used to program WS2812 strips
#endif

#ifdef WS2812_TARGET_PLATFORM_ARDUINO_AVR
        uint8_t *pins;             ///< Array of pins used to program WS2812 strips (**Must share the same PORT!** (ex. pin 0-7), see https://www.arduino.cc/en/Reference/PortManipulation) 
        uint8_t n_strips;          ///< Number of pins used used to program WS2812 strips
#endif

// COMMON
        uint8_t rst_time_us;      ///< Time required for WS2812 to reset in us
        ws2812_order order;       ///< Color order (ex. rgb, grb, bgr...)
        
} ws2812_cfg;

/**
 * @brief Initializes a rgb map for a given color order. 
 *
 * The following function is intended only to be used for internal driver code, hence
 * the _ prefix. It fills a 3 element byte array with the necessary offsets to map/convert
 * RGB values to a different color order. For example, setting the order to `rgb` fills the
 * rgb map with `0, 1, 2` and `bgr` fill the rgb map with `2, 1, 0`, etc.
 *
 */
void _ws2812_get_rgbmap(uint8_t (*rgbmap)[3], ws2812_order order);

/**
 * @brief Configures the WS2812 driver. 
 *
 * The following function configures various, often platform-specific, parameters of the WS2812
 * driver. The configured parameters are passed through a #ws2812_cfg object, and may define attributes
 * such as which pin is used for data transmission, the WS2812 reset time, the WS2812 color order and more... 
 *
 */
uint8_t ws2812_config(ws2812_cfg config);

/**
 * @brief Prepares the WS2812 driver for data transmission. 
 *
 * The following function prepares the WS2812 driver for data transmission.
 * The exact preperation procedure is platform specific, but typically
 * will involve disabling interrupts and stashing registers that may be
 * modified when communicating to the WS2812 strip. For a more detailed understanding,
 * please refer to the platform specific code of this function.
 *
 * @warning Always call this function before making any calls to #ws2812_tx()!
 *      Not doing so may result in undefined behaivour!
 */
void ws2812_prep_tx();

/**
 * @brief Transmits RGB values to the WS2812 strip.
 *
 * The following function transmits RGB values to the WS2812 strip.
 * Calling this function consecutively will continue programming LEDs
 * after the position where the last transmission has ended. In
 * other words, calling the function consecutively will **NOT** program
 * the strip from the first LED, but rather take off from where it last ended.
 * If this is not desired, call #ws2812_wait_rst() after each
 * transmission.
 * 
 * @warning Transmissions must take place consecutively.
 *      Should there be too great of a delay between transmission calls,
 *      the strip WS2812s reset time will expire, causing the strip to
 *      be programmed from the first LED again.
 */
void ws2812_tx(ws2812_rgb *pxls, size_t n_pxls);

/**
 * @brief Waits for the WS2812 strip to reset.
 *
 * The following function waits for the WS2812 strip to reset, thus allowing it to be
 * overwritten from the first LED again. The reset time is set in the ws2812_cfg object
 * and is recommended to be 50us according to the WS2812 datasheet, but may be set significantly
 * lower for some strips. Call this function if you wish to overwritte the LED strip after a
 * previous #ws2812_tx() call.
 *
 * @note This function does not have to be called prior calling #ws2812_end_tx() as it already
 *      contains a call to #ws2812_wait_rst().
 */
void ws2812_wait_rst();

/**
 * @brief Closes a WS2812 transmission.
 *
 * The following function should be called after ending data transmission with
 * the WS2812 strip. The exact closing procedure is platform specific but will
 * typically involve restoring stashed registers to their previous states, 
 * re-enable interrupts, and wait for the WS2812 to reset by calling #ws2812_wait_rst().
 */
void ws2812_close_tx();