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
 * @file ws2812_common.cxx
 * @author Patrick Pedersen
 * @date 2021-04-09
 * 
 * @brief Common code shared accross all supported platforms.
 * 
 * The following file holds the Tiny-WS2812 code used throughout all platforms.
 */

#include <stdio.h>
#include <string.h>

#include <ws2812.h>

// RGB maps
const static uint8_t ws2812_order_rgb[3] = { 0, 1, 2 };
const static uint8_t ws2812_order_rbg[3] = { 0, 2, 1 };
const static uint8_t ws2812_order_brg[3] = { 2, 0, 1 };
const static uint8_t ws2812_order_bgr[3] = { 2, 1, 0 };
const static uint8_t ws2812_order_grb[3] = { 1, 0, 2 };
const static uint8_t ws2812_order_gbr[3] = { 1, 2, 0 };

// Refer to header for documentation
void _ws2812_get_rgbmap(uint8_t (*rgbmap)[3], ws2812_order order)
{
        switch (order) {
                case rbg:
                        memcpy(rgbmap, ws2812_order_rbg, 3);
                        break;
                case brg:
                        memcpy(rgbmap, ws2812_order_brg, 3);
                        break;
                case bgr:
                        memcpy(rgbmap, ws2812_order_bgr, 3);
                        break;
                case grb:
                        memcpy(rgbmap, ws2812_order_grb, 3);
                        break;
                case gbr:
                        memcpy(rgbmap, ws2812_order_gbr, 3);
                        break;
                default: // rgb
                        memcpy(rgbmap, ws2812_order_rgb, 3);
                        break;
        }
}