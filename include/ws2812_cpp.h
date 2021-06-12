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
 * @file ws2812_cpp.h
 * @author Patrick Pedersen
 * @date 2021-05-12
 * 
 * @brief C++ Wrapper for the Tiny-WS2812 interface
 * 
 * The following file defines a C++ wrapper for the Tiny-WS2812 interface
 */

#include <ws2812.h>

class ws2812_cpp {
public:
        ws2812_cpp(ws2812_cfg config, uint8_t *ret);

private:
        ws2812 _ws2812;

public:
        void prep_tx();
        void tx(ws2812_rgb *leds, size_t n_leds);
        void wait_rst();
        void close_tx();
};