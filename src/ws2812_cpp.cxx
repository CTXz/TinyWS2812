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
 * @file ws2812_cpp.cxx
 * @author Patrick Pedersen
 * @date 2021-05-12
 * 
 * @brief Defines functions for the Tiny-WS2812 C++ interface wrapper
 *
 * The folloing file defines all functions defined in the @ref ws2812_cpp.h "Tiny-WS2812 C++ interface header"
 * 
 */ 

#include <ws2812_cpp.h>

ws2812_cpp::ws2812_cpp(ws2812_cfg config, uint8_t *ret)
{
       *ret = ws2812_config(&_ws2812, config);
}

void ws2812_cpp::prep_tx()
{
        ws2812_prep_tx(&_ws2812);
}

void ws2812_cpp::tx(ws2812_rgb *leds, size_t n_leds)
{
        ws2812_tx(&_ws2812, leds, n_leds);
}

void ws2812_cpp::wait_rst()
{
        ws2812_wait_rst(&_ws2812);
}

void ws2812_cpp::close_tx()
{
        ws2812_close_tx(&_ws2812);
}