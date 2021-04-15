# Tiny-WS2812 Driver Documentation


<br>

> ## [Doxygen Reference](https://ctxz.github.io/TinyWS2812/https://ctxz.github.io/TinyWS2812/)

<br>

Welcome to the Tiny-WS2812 documentation.


# Introduction

The Tiny-WS2812 driver interface initially derives from the driver code of an open source WS2812 LED controller that I had worked on prior, and provides a nearly barebone interface to communicate with WS2812 LED strips.

The following platforms and frameworks are currently supported:

* Barebone AVR
* The Arduino Framework (Currently only AVR based (eg. Uno, Leonardo, Micro...))
It has been developed out of the necessity to have an extremely light weight and flexible cross-platform driver that can be further abstracted and used troughout my WS2812 projects, particullary on MCUs with severe memory constraints (ex. ATTiny chips), where one cannot just define an RGB array equivalent to the size of the LED strip. This drivers purpose is **NOT** to provide fancy abstractions and functions for color correction, brightness settings, animations etc.

To summerize, this driver is inteded to:

* be used on MCUs with limited computing resources.
* act as a base for more abstract WS2812 libraries.
* be easily portable to other platforms or programming frameworks (ex. Arduino).
Because the motivation of this driver is to be as barebone as possible, it relies on a superficial understanding of the WS2812 protocol and may demand an understanding of the host platforms platform (eg. registers etc.). For quick and simple programming of WS2812 strips, where memory and processing power are not a big issue, other drivers/libraries should probably be consulted.


# Overview

The WS2812 driver code is split into two parts. The first part is the abstract driver interface defined in ws2812.h. The second part is the platform specific code that implements the interface for different platforms (at the moment only barebone AVR and AVR based Arduino devices). The platform specific driver implementations can be found in the src directory. To use this driver throughout your project, only an understanding of the driver interface is required.

To make the use of this driver a breeze, all source files have been documented and referenced using doxygen.



* You can find a list of classes here: [Class list](https://ctxz.github.io/TinyWS2812/annotated.html)
* You can find a overview of all source files, including examples, here: [File list](https://ctxz.github.io/TinyWS2812/files.html)
 
In the next section we will go through a basic example to get familiar with the driver in no time.


# Basic usage/tutorial


## Importing the driver

The driver has been primarily written with platformio in mind, but can be also be imported into the Arduino IDE and frankly, any other programming environment.

To import the driver on platformio, simply look for its name in the extensions browser and download the extension.

Importing the driver into the Arduino IDE is a little nasty, due to its limitations of now allowing one to define compile macros, as well as its incompatibility with c files. To still be able to import this project into the Arduino IDE, I have written a bash script, called makearduinolib, which can be found in the tools/ folder of this project. To run it, simply execute:



```cpp
./tools/makearduinolib.sh
```

The script will as you to specify a target platform and produce a Arduio-IDE compatible library zip in the root folder of this project.


## Selecting the target platform

This section can be skipped if you use the Arduino IDE and have generated a library zip using the script mentioned above, as a target platform has already been selected in the library generating process.


To use the Tiny-WS2812 driver throughout your project, first ensure that you have specified the target platform for which the driver is being build for. To do so, define one of the following build flags in your project:



* Barebone AVR: `WS2812_TARGET_PLATFORM_AVR`
* Arduino Framework (AVR): `WS2812_TARGET_PLATFORM_ARDUINO_AVR`
Support for more platforms (ex. ESP and ARM) is planned in the future.

Perhaps you may be wondering what the difference it makes to build for the barebone AVR target and the Arduino AVR target. While both targets can be effectively used for any AVR MCU based device, the barebone AVR target limits itself to the code provided by the AVR C libraries. The Arduino AVR target makes use of the code provided by the Arduino framework. The only difference relevant to the driver user here is that the Arduino framework target will include the Arduino framework (which may not be desired or possible in some circumstances) and the differences in the driver configuration struct (ws2812_cfg, more on that later), which is platform specific and is used to configure various driver parameters (data output pin, strip reset time, strip color order etc.).


## Learning by example: Blinking one or more WS2812 strips

In the following section we will working our way through the examples/arduino_avr/blink_array.c example. We assume our target platform to be ARDUINO_AVR, however most of the driver specific code is the same across all platforms. Platform specific differences will be indicated.



```cpp
#include <Arduino.h>
#include <ws2812.h>

// Parameters - ALTER THESE TO CORRESPOND WITH YOUR OWN SETUP!
#define STRIP_SIZE 8     ///< Size of your WS2812 strip(s)
#define DATA_PINS {8, 9} ///< Arduino pin(s) used to program the WS2812 strip(s). Must share same port! (See https://www.arduino.cc/en/Reference/PortManipulation)
#define RESET_TIME 50    ///< Reset time in microseconds (50us recommended by datasheet)
#define COLOR_ORDER grb  ///< Color order of your LED strips (Typically grb or rgb)

uint8_t pins[] = DATA_PINS;
ws2812_rgb strip[STRIP_SIZE]; ///< RGB array which represents the LED strip
ws2812_cfg cfg;               ///< Driver configuration

void setup()
{
        // Configure the driver
        cfg.pins = pins;
        cfg.rst_time_us = RESET_TIME;
        cfg.order = COLOR_ORDER;
        cfg.n_strips = sizeof(pins); ///< Number of strips
        
        if (ws2812_config(cfg) != 0) {
                // HANDLE ERROR HERE
                void;
        }
}

void loop()
{
        // Program all LEDs to white
        for (unsigned int i = 0; i < STRIP_SIZE; i++) {
                strip[i].r = 255;
                strip[i].g = 255;
                strip[i].b = 255;              
        }

        ws2812_prep_tx();              // Prepare driver to transmit data
        ws2812_tx(strip, STRIP_SIZE);  // Transmit array of org values to strip
        ws2812_close_tx();             // Close transmission

        // Wait 500ms
        delay(500);

        // Program all LEDs to black (off)
        for (unsigned int i = 0; i < STRIP_SIZE; i++) {
                strip[i].r = 0;
                strip[i].g = 0;
                strip[i].b = 0;
        }

        ws2812_prep_tx();              // Prepare driver to transmit data
        ws2812_tx(strip, STRIP_SIZE);  // Transmit array of org values to strip
        ws2812_close_tx();             // Close transmission

        // Wait 500ms
        delay(500);
}
```

Let's begin with the most obvious parts first. At the top of the code we import the ws2812.h header. 

```cpp
#include <ws2812.h>
```

This header exposes the interface of the WS2812 driver, in other words, it provides you with all of the relevant functions to drive WS2812 strips.

Following the header inclusion, we define some fixed macro parameters that you must adjust to your own setup. 

```cpp
#define STRIP_SIZE 8     ///< Size of your WS2812 strip(s)
#define DATA_PINS {8, 9} ///< Arduino pin(s) used to program the WS2812 strip(s). Must share same port! (See https://www.arduino.cc/en/Reference/PortManipulation)
#define RESET_TIME 50    ///< Reset time in microseconds (50us recommended by datasheet)
#define COLOR_ORDER grb  ///< Color order of your LED strips (Typically grb or rgb)
```

The `DATA_PINS` parameter tells the driver which Arduino pins are used to program WS2812 strips. As you may have noticed by now, the driver has the option to drive multiple WS2812 strips in parallel. However, this feature may be **platform specific** and comes with some restrictions.

A note on driving multiple WS2812 strips on AVR devices: Because GPIO pins are accessed via 8 bit port registers on AVR devices (each bit representing one GPIO), we have the possibility to output the same data across a maximum of 8 pins at the same time. This, however, is not without restrictions, as all data pins must be assigned to the same port register (see the [Arduino Port Manipulation reference](https://ctxz.github.io/TinyWS2812/https://www.arduino.cc/en/Reference/PortManipulation) for Arduino builds, or consult your AVR MCUs datasheet for barebone AVR builds). Further, since all pins will output the same data simultaneously, it also means that the WS2812 strips **must be identical in size and color order**. For configurations where these requirements cannot be satisfied, it is recommended to create multiple configuration objects for each WS2812 strip, which can then be programmed in series. Noticeable delays for large strips can be reduced by quickly alternating transmission between multiple LED strips, as long as the MCU can toggle between transmissions faster than the reset time.


The `RESET_TIME` parameter tells the driver how many microseconds it must wait after programming WS2812 strips, before it can program the strips from the first LED again. Should none of this make sense, it is highly advised to take a look at [how WS2812 strips are driven](https://ctxz.github.io/TinyWS2812/https://www.arrow.com/en/research-and-events/articles/protocol-for-the-ws2812b-programmable-led).

The `COLOR_ORDER` parameters defines the color order in which the WS2812 strips must be written to. Unfortunately there is no fixed standard on the color order of WS2812 strips. Some are programmed in RGB order, however, from my own experience, GRB seems to be the most common order across readily available WS2812 strips.

The remaining parameters, along with their comments, should be rather self explanatory.

Next up we have the following three defintions: 

```cpp
uint8_t pins[] = DATA_PINS;
ws2812_rgb strip[STRIP_SIZE]; ///< RGB array which represents the LED strip
ws2812_cfg cfg;               ///< Driver configuration
```

In the top definition we create an array that will tell the driver which pins are used to drive WS2812 strips.

In the middle definition we create an array of RGB values with the size of the LED strip. This RGB array will be used to program the LED strip and acts virtual representation of it.

The latter definition creates a ws2812_cfg configuration object and is used to configure various, **often platform-specific**, parameters of the WS2812 driver. For an overview of platform-specific differences, please refer to the ws2812_cfg struct refrence.

Continuing further down we declare the setup function: 

```cpp
// Configure the driver
cfg.pins = pins;
cfg.rst_time_us = RESET_TIME;
cfg.order = COLOR_ORDER;
cfg.n_strips = sizeof(pins); ///< Number of strips

if (ws2812_config(cfg) != 0) {
        // HANDLE ERROR HERE
        void;
}
```

Within the setup() function we set the fields of our configuration object and use it to configure the WS2812 driver using the ws2812_config() function. The code should be rather self explanatory since it simply parses the parameter macros that we have already discussed above. It should once again be mentioned that the ws2812_cfg struct is **platform specific**! In this example we see how to configure the for the ARDUINO_AVR target platform, for other target platforms, please refer to the platform specific examples or the ws2812_cfg reference page. 

All fields of the configuration object must be defined. Leaving fields undefined will lead to undefined behaivor!


Next up we come to the loop() function, here we will make the strip blink.

We start off by setting all RGB values in our strip array to white: 

```cpp
// Program all LEDs to white
for (unsigned int i = 0; i < STRIP_SIZE; i++) {
        strip[i].r = 255;
        strip[i].g = 255;
        strip[i].b = 255;              
}
```

Now that the strip array has been filled with white, all that is left is to transmit its values to the WS2812 strip: 

```cpp
ws2812_prep_tx();              // Prepare driver to transmit data
ws2812_tx(strip, STRIP_SIZE);  // Transmit array of org values to strip
ws2812_close_tx();             // Close transmission
```

First, we call ws2812_prep_tx() to prepare the driver for data tranmission. Following the ws2812_prep_tx() call we then transmit the rgb values of the strip array to the WS2812 strip(s). Finally, once we are done transmitting, we proceed to close the tranmission. The WS2812 strip(s) should now be set to white.

Calling ws2812_tx() consecutively without closing and preparing a new transmission, or without waiting for the WS2812 strip to reset by calling ws2812_wait_rst(), will cause the strip to get programmed from position where the last transmission has ended. This can actually be used to to ones advantage, as seen in the blink_loop example, where we save memory by repeatedly tranmsitting the same RGB value to fill the strip instead of creating an RGB array the size of the strip.


Given that we have made it this far, the rest of the code should require no more explanation, as we simply wait for a total of 500ms, set our strip to off using the exact same procedure that we have used to set it white, and lastly wait another 500ms. After that, everything simply gets repeated again and we should be seeing one or more blinking WS2812 strips.


## A final word

In the example above we have used an RGB array to set the colors of the WS2812 strips. However, for cases like above, where we only wish to fill the entire strip with a single color, allocating an RGB array the size of the strip seems like a memory expensive solution. Instead of allocating an array for the entire strip, we can simply repeatedly transmit the same color value to the WS2812 strip(s) until the entire strip has been filled. This much more memory friendly method is showcased in the 

-------------------------------

Updated on 15 April 2021 at 19:59:13 CEST
