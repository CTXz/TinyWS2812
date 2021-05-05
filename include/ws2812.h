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
 * @brief Exposes the Tiny-WS2812 library interface.
 * 
 * The Tiny-WS2812 library initially derives from the
 * driver code of an open source WS2812 LED controller that I had
 * worked on prior, and provides a nearly barebone interface to
 * communicate with WS2812 LED devices.
 * 
 * The following platforms and frameworks are currently supported:
 *      - Barebone AVR
 *      - The Arduino Framework (Currently only AVR based (eg. Uno, Leonardo, Micro...))
 * 
 * It has been developed out of the necessity to have an extremely light 
 * weight and flexible cross-platform library that can be further abstracted
 * and used troughout my WS2812 projects, particullary on MCUs with severe 
 * memory constraints (ex. ATTiny chips), where one cannot just define an RGB
 * array equivalent to the number of LEDs. This libraries purpose is **NOT**
 * to provide fancy abstractions and functions for color correction, brightness 
 * settings, animations etc.
 * 
 * To summerize, this library is inteded to:
 *      - be used on MCUs with limited computing resources.
 *      - act as a base for more abstract WS2812 libraries.
 *      - be easily portable to other platforms or programming frameworks (ex. Arduino).
 *
 * @note Because the motivation of this library is to be as barebone as possible,
 *      it relies on a superficial understanding of the WS2812 protocol and may
 *      demand an understanding of the host platforms platform (eg. registers etc.).
 *      For quick and simple programming of WS2812 devices, where memory and processing
 *      power are not a big issue, other libraries should probably be consulted.  
 */

#pragma once

#ifdef WS2812_TARGET_PLATFORM_AVR
#ifdef _WS2812_TARGET_PLATFORM_DEFINED
#error "Multiple target platforms defined!"
#endif
#define _WS2812_TARGET_PLATFORM_DEFINED
#include "ws2812_avr.h"
#endif

#ifdef WS2812_TARGET_PLATFORM_ARDUINO_AVR
#ifdef _WS2812_TARGET_PLATFORM_DEFINED
#error "Multiple target platforms defined!"
#endif
#define _WS2812_TARGET_PLATFORM_DEFINED
#include "ws2812_avr.h"
#endif

#ifndef _WS2812_TARGET_PLATFORM_DEFINED
#error "No target platform defined!"
#endif

#include <stddef.h>
#include <stdint.h>

#include "ws2812_common.h"

/**
 * @brief Initializes a rgb map for a given color order. 
 *
 * The following function is intended only to be used for internal library code, hence
 * the _ prefix. It fills a 3 element byte array with the necessary offsets to map/convert
 * RGB values to a different color order. For example, setting the order to `rgb` fills the
 * rgb map with `0, 1, 2` and `bgr` fill the rgb map with `2, 1, 0`, etc.
 *
 */
void _ws2812_get_rgbmap(uint8_t (*rgbmap)[3], ws2812_order order);

/**
 * @brief Configures a @ref ws2812 "WS2812 device struct". 
 *
 * The following function initializes/configures a @ref ws2812 "WS2812 device struct" 
 * using a @ref ws2812_cfg "ws2812_cfg configuration struct".
 *
 */
uint8_t ws2812_config(ws2812 *dev, ws2812_cfg config);

/**
 * @brief Prepares the host device for data transmission. 
 *
 * The following function prepares the host device for data transmission.
 * The exact preperation procedure is platform specific, but typically
 * will involve disabling interrupts, stashing registers that may be
 * modified when communicating to the WS2812 device and potentially 
 * preparing the passed @ref ws2812 "WS2812 device struct" for data transmission.
 * For a more detailed understanding, please refer to the platform
 * specific code of this function.
 *
 * @warning Always call this function before making any calls to #ws2812_tx()!
 *      Not doing so may result in undefined behaivour!
 */
void ws2812_prep_tx(ws2812 *dev);

/**
 * @brief Transmits RGB values to the provided @ref ws2812 "WS2812 device".
 *
 * The following function transmits RGB values to the provided @ref ws2812 "WS2812 device".
 * Calling this function consecutively for the same device will continue 
 * programming LEDs after the position where the last transmission has ended.
 * In other words, calling the function consecutively for the same device will
 * **NOT** program the device from the first LED, but rather take off from where
 * it last ended. If this is not desired, call #ws2812_wait_rst() after each
 * transmission.
 * 
 */
void ws2812_tx(ws2812 *dev, ws2812_rgb *pxls, size_t n_pxls);

/**
 * @brief Waits for the @ref ws2812 "WS2812 device" to reset.
 *
 * The following function waits for the @ref ws2812 "WS2812 device" to reset, thus allowing it to be
 * overwritten from the first LED again. The reset time is configuredin the ws2812_cfg 
 * object used to initialize/configure the passed @ref ws2812 "WS2812 device struct",
 * and is recommended to be 50us according to the WS2812 datasheet, but may be set significantly
 * lower for some WS2812 devices. Call this function if you wish to overwritte the WS2812 device after a
 * previous #ws2812_tx() call.
 *
 * @note This function does not have to be called prior calling #ws2812_end_tx() as it already
 *      contains a call to #ws2812_wait_rst().
 */
void ws2812_wait_rst(ws2812 *dev);

/**
 * @brief Closes a WS2812 transmission.
 *
 * The following function should be called after ending data transmission with
 * a WS2812 device. The exact closing procedure is platform specific but will
 * typically involve restoring stashed registers to their previous states, 
 * re-enable interrupts, wait for the WS2812 to reset by calling #ws2812_wait_rst(),
 * and potentially alter fields of the provided @ref ws2812 "WS2812 device struct".
 */
void ws2812_close_tx(ws2812 *dev);