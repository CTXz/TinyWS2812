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
 * @file ws2812_avr.h
 * @author Patrick Pedersen
 * @date 2021-04-16
 * 
 * @brief Provides AVR platform specific definitions.
 * 
 */

#pragma once

#include <stdint.h>

#include "ws2812_common.h"
#include "ws2812.h"

/**
 * @brief Data structure to configure a @ref ws2812 "WS2812 device struct" on AVR platforms.
 * 
 * The following struct is used to initialize/configure a @ref ws2812 "WS2812 device struct"
 * on AVR based devices. It is passed to the ws2812_config() function along
 * with a reference to a @ref ws2812 "WS2812 device struct", and contains relevant
 * information such as pins used to drive WS2812 devices, the device's reset time etc., 
 * necessary for configuring the @ref ws2812 "WS2812 device struct" to drive one or multiple 
 * WS2812 devices on AVR based devices. The data structure varies for the Arduino AVR 
 * target and the barebone AVR target.
 * 
 * @warning All fields of the configuration object must be defined before passing it to #ws2812_config()!
 *      Leaving a field undefined will result in undefined behaivor!
 */
typedef struct ws2812_cfg {

#ifdef WS2812_TARGET_PLATFORM_AVR
        volatile uint8_t *port;   ///< PORT Register (ex. PORTB, PORTC, PORTD...) 
        volatile uint8_t *ddr;    ///< Data Direction Register (ex. DDRB, DDRC, DDRD...)
        uint8_t *pins;            ///< Array of pins used to program WS2812 devices (**Must share the same PORT!** (ex. PB0, PB1, PB2))
#endif

#ifdef WS2812_TARGET_PLATFORM_ARDUINO_AVR
        uint8_t *pins;             ///< Array of pins used to program WS2812 devices (**Must share the same PORT!** (ex. pin 0-7), see https://www.arduino.cc/en/Reference/PortManipulation) 
#endif

        uint8_t rst_time_us;      ///< Time required for WS2812 to reset in us
        ws2812_order order;       ///< Color order (ex. rgb, grb, bgr...)
        uint8_t n_dev;            ///< Number of WS2812 devices driven
        
} ws2812_cfg;

/**
 * @brief WS2812 device struct to drive one or more WS2812 devices on AVR based platforms.
 * 
 * The following struct is used to drive one or more WS2812 devices on AVR based devices.
 * It is initialized by the ws2812_config() function and is taken as an argument by practically
 * every function of the TinyWS2812 library relevant to driving WS2812 devices (ex. ws2812_tx(),
 * ws2812_prep_tx(), etc...).
 * 
 * @see ws2812_config()
 * 
 */
typedef struct ws2812 {
        volatile uint8_t *port; ///< PORT register of pins used to drive the WS2812 device(s).
        uint8_t rst_time_us;    ///< Time required for WS2812 to reset in us.
        uint8_t maskhi;         ///< PORT masks to toggle the data pins high.   
        uint8_t masklo;         ///< PORT masks to toggle the data pins low.
        uint8_t rgbmap[3];      ///< RGB map to map/convert RGB values to another color order.
} ws2812;