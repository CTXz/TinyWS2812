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
 * @file ws2812_common.h
 * @author Patrick Pedersen
 * @date 2021-04-16
 * 
 * @brief Definitions required by all platform specific headers.
 * 
 * The following header features platform independent definitions,
 * such as typedefs, enums and structs, that are used across
 * all platorm specific headers.
 * 
 */

#pragma once

/**
 * @brief Enum to specify the WS2812 device's color order.
 *
 * Unfortunately there is no clear standard on sequential color
 * order of WS2812 LEDs, some are intuitively programmed in
 * RGB order, however, many, if not the majority, are programmed
 * in GRB oder. As there is automatic way to identify the correct
 * color order, it must be manually provided in the device configuration
 * with this enum.
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