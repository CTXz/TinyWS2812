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

#ifdef __cplusplus

#include <ws2812.h>

/**
 * @brief A C++ wrapper for the Tiny-WS2812 interface
 *
 * The following class defines a C++ wrapper for the Tiny-WS2812 interface.
 * For an usage example of the C++ wrapper, refer to the @ref examples/arduino_avr/blink_cpp.cxx
 * "blink_cpp example for the Arduino Framework" or the 
 * @ref examples/avr/blink_cpp.cxx "blink_cpp example for barebone AVR programming"
 */
class ws2812_cpp {
public:

        /**
         * @brief Constructs a @ref ws2812_cpp object from a ws2812_cfg struct and returns 0 on success. 
         *
         * The following constructor wraps around the ws2812_config() function and takes a configured
         * ws2812_cfg struct instance and initializes the ws2812_cpp object. On success, ret is set to
         * 0. For a overview of possible return values, refer to the @ref ws2812_config() reference. 
         *
         */
        ws2812_cpp(ws2812_cfg config, uint8_t *ret);

private:
        ws2812 _ws2812; ///< Wrapped ws2812 device struct

public:
        /**
         * @brief Wraps around the ws2812_prep_tx() function
         *
         */
        void prep_tx();

        /**
         * @brief Wraps around the ws2812_tx() function
         *
         */
        void tx(ws2812_rgb *leds, size_t n_leds);
        
        
        /**
         * @brief Wraps around the ws2812_rst() function
         *
         */
        void wait_rst();


        /**
         * @brief Wraps around the ws2812_close_tx() function
         *
         */
        void close_tx();
};

#endif // __cplusplus