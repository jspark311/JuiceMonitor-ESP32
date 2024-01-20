/*
File:   JuiceMonitor.h
Author: J. Ian Lindsay
Date:   2023.07.09

This header file is the top-level description of the project's hardware. Some
  projects will prefer to do things differently, but this should be simple
  enough for an intro project.
*/

/*******************************************************************************
* Inclusions
*******************************************************************************/
/* Compiler-provided */
#include <inttypes.h>
#include <stdint.h>
#include <math.h>

/*
* ManuvrPlatform is ESP32. But this project ought to work just as well on any
*   other platform that supports the needed features.
*/
#include <ESP32.h>

/*
* CppPotpourri contains our basic notions of types used throughout the project.
*   Notably, this includes AbstractPlatform, which forms the basis of our
*   interaction with things like GPIO pins, time and date, etc.
* Beyond that, we will use the ParsingConsole class to handle user interaction,
*   SensorFilter to aggregate sensor data, the I2C abstraction, and some fluff
*   classes for identifying and profiling this firmware.
*/
#include "CppPotpourri.h"
#include "AbstractPlatform.h"
#include "StringBuilder.h"
#include "SensorFilter.h"
#include "Console/C3PConsole.h"
#include "TimerTools/TimerTools.h"
#include "BusQueue/I2CAdapter.h"


/*
* Pull in ManuvrDrivers. This will include the headers for all known-not-broken
*   drivers. There are some perfectly reasonable drivers that are not included
*   by this single-header inclusion.
*/
//#include <ManuvrDrivers.h>

/*
* Better hygiene would be to only include the drivers you intend to use.
* Benefits:
*   1) Marginally faster builds.
*   2) Side-step any possible build failures in unused drivers.
*   3) Clearer documentation about hardware assumptions.
*   4) Including drivers that are not buildable on all platforms.
*/
#include "MCP23x17/MCP23x17.h"  // 16-bit GPIO expanders (I2C and SPI)
#include "PAC195x/PAC195x.h"    // A flexible multi-channel power monitor


#ifndef __JUICE_MONITOR_H__
#define __JUICE_MONITOR_H__


/*******************************************************************************
* Pin definitions and hardware constants.
*******************************************************************************/
/* Platform pins */
#define LED_PIN             16   // OUTPUT
#define MCP23017_IRQ_PIN    17   // INPUT_PULLUP
#define PAC195X_ALERT1_PIN  18   // INPUT_PULLUP
#define PAC195X_ALERT2_PIN  19   // INPUT_PULLUP
#define PAC195X_PWRDOWN_PIN 22   // OUTPUT
#define MCP23017_RESET_PIN  23   // OUTPUT
#define SDA0_PIN            25
#define SCL0_PIN            26

/* MCP23017 GPIO test pins */
#define TEST_PIN_0_PIN      0
#define TEST_PIN_1_PIN      1
#define TEST_PIN_2_PIN      2
#define TEST_PIN_3_PIN      3
#define TEST_PIN_4_PIN      4
#define TEST_PIN_5_PIN      5
#define TEST_PIN_6_PIN      6
#define TEST_PIN_7_PIN      7
#define TEST_PIN_8_PIN      8
#define TEST_PIN_9_PIN      9
#define TEST_PIN_A_PIN     10
#define TEST_PIN_B_PIN     11
#define TEST_PIN_C_PIN     12
#define TEST_PIN_D_PIN     13
#define TEST_PIN_E_PIN     14
#define TEST_PIN_F_PIN     15


/*******************************************************************************
* Invariant software parameters
*******************************************************************************/
#define MCP23017_I2C_ADDR  0x23
#define PAC195X_I2C_ADDR   0x17

/*******************************************************************************
* Externs to hardware resources
*******************************************************************************/
extern I2CAdapter i2c0;
extern PAC195x  pwr_sensor;
extern MCP23017 offboard_gpio;


/*******************************************************************************
* Externs to software singletons
*******************************************************************************/
extern ParsingConsole console;


/*******************************************************************************
* Function prototypes
*******************************************************************************/
void ledOn(uint8_t idx, uint32_t duration, uint16_t intensity = 3500);

#endif    // __JUICE_MONITOR_H__
