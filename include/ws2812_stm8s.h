/*
 * Copyright (C) 2022  Patrick Pedersen

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
 * @file ws2812_stm8s.h
 * @author Patrick Pedersen
 * @date 2022-10-26
 * 
 * @brief Provides STM8S platform specific definitions.
 * 
 */

#pragma once

#ifdef WS2812_TARGET_PLATFORM_STM8S

#include <stm8s.h>
#include <stdint.h>

#include "ws2812_common.h"

/**
 * @brief Data structure to configure a @ref ws2812 "WS2812 device struct" on STM8S platforms.
 * 
 * The following struct is used to initialize/configure a @ref ws2812 "WS2812 device struct"
 * on STM8S based devices. It is passed to the ws2812_config() function along
 * with a reference to a @ref ws2812 "WS2812 device struct", and contains relevant
 * information such as pins used to drive WS2812 devices, the device's reset time etc., 
 * necessary for configuring the @ref ws2812 "WS2812 device struct" to drive one or multiple 
 * WS2812 devices on STM8S based devices.
 * 
 * @note The library currently makes use of the STM8 Standard Peripheral Library meaning it is
 * a required dependency to use this library.
 * 
 * @warning All fields of the configuration object must be defined before passing it to #ws2812_config()!
 *      Leaving a field undefined will result in undefined behaivor!
 */
typedef struct ws2812_cfg {
	uint16_t port_baseaddr;
        GPIO_Pin_TypeDef *pins;
        uint8_t rst_time_us;
        ws2812_order order;
        uint8_t n_dev;
} ws2812_cfg;

/**
 * @brief WS2812 device struct to drive one or more WS2812 devices on STM8S based platforms.
 * 
 * The following struct is used to drive one or more WS2812 devices on STM8S based devices.
 * It is initialized by the ws2812_config() function and is taken as an argument by practically
 * every function of the TinyWS2812 library relevant to driving WS2812 devices (ex. ws2812_tx(),
 * ws2812_prep_tx(), etc...).
 * 
 * @see ws2812_config()
 * 
 */
typedef struct ws2812 {
        uint16_t port_baseaddr;
        uint8_t rst_time_us; 
        uint8_t maskhi;    
        uint8_t masklo;         
        uint8_t rgbmap[3];    
} ws2812;

#endif