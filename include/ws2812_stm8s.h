#pragma once

#ifdef WS2812_TARGET_PLATFORM_STM8S

#include <stm8s.h>
#include <stdint.h>

#include "ws2812_common.h"

typedef struct ws2812_cfg {
	uint16_t port_baseaddr;
        GPIO_Pin_TypeDef *pins;
        uint8_t rst_time_us;
        ws2812_order order;
        uint8_t n_dev;
} ws2812_cfg;

typedef struct ws2812 {
        uint16_t port_baseaddr;
        uint8_t rst_time_us; 
        uint8_t maskhi;    
        uint8_t masklo;         
        uint8_t rgbmap[3];    
} ws2812;

#endif