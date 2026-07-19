/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2022 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "env_validate.h"

#define BOARD_INFO_NAME "Pandaplacer MotionController V2.1"


// It is required to disable JTAG function because its pins are
// used as GPIO
// MUST DISABLE JTAG to use PA15 as GPIO
#define DISABLE_JTAG

//
// If you don't need the SWDIO functionality (any more), you may
// disable SWD here to regain PA13/PA14 pins for other use.
//
//#define DISABLE_JTAGSWD

// Ignore temp readings during development.
//#define BOGUS_TEMPERATURE_GRACE_PERIOD    2000

// Enable EEPROM Emulation for this board as it doesn't have EEPROM
#if EITHER(NO_EEPROM_SELECTED, FLASH_EEPROM_EMULATION)
// #if FLASH_EEPROM_EMULATION 
  #define FLASH_EEPROM_EMULATION
  #define EEPROM_PAGE_SIZE     (0x800U)           // 2K
  #define EEPROM_START_ADDRESS (0x8000000UL + (STM32_FLASH_SIZE) * 1024UL - (EEPROM_PAGE_SIZE) * 2UL)
  #define MARLIN_EEPROM_SIZE    EEPROM_PAGE_SIZE  // 2K
#endif

// #undef HSE_STARTUP_TIMEOUT
// #undef HSE_TIMEOUT_VALUE
// #define HSE_STARTUP_TIMEOUT    ((uint16_t)0x1000)   /*!< Time out for HSE start up */
// #define HSE_TIMEOUT_VALUE    HSE_STARTUP_TIMEOUT   /*!< Time out for HSE start up */

#undef LED_BUILTIN
#undef LED_GREEN
#undef USER_BTN


//F103RC TIMER
#define SERIAL_TIMER 1
#define TEMP_TIMER 6
#define STEP_TIMER 7
#define SERVO_TIMER 2
#define TONE_TIMER 8


#define NC_PIN  PD2     // RESERVED PIN 


//
// Limit Switches
//
#define X_MIN_PIN                           PC15
//#define X_MAX_PIN                           -1
//#define Y_MIN_PIN                           -1
#define Y_MAX_PIN                           PC13
//#define Z_MIN_PIN                           -1
#define Z_MAX_PIN                           PC14
#define I_MIN_PIN                           Y_MAX_PIN   // Y_MAX_PIN
#define J_MIN_PIN                           Y_MAX_PIN   // Y_MAX_PIN
// #define I_MIN_PIN                           NC_PIN   // NC_PIN
// #define J_MIN_PIN                           NC_PIN   // NC_PIN

//
// Steppers
//
#define X_STEP_PIN                          PC10
#define X_DIR_PIN                           PB15
#define X_ENABLE_PIN                        PA8

#define Y_STEP_PIN                          PB4
#define Y_DIR_PIN                           PB5
#define Y_ENABLE_PIN                        PC11

#define Z_STEP_PIN                          PB13
#define Z_DIR_PIN                           PB14
#define Z_ENABLE_PIN                        PB12

// A axis
#define I_STEP_PIN                          PC5
#define I_DIR_PIN                           PC4
#define I_ENABLE_PIN                        PB0      // AB ENABLE
// B axis
#define J_STEP_PIN                          PB1
#define J_DIR_PIN                           PB2
#define J_ENABLE_PIN                        PB0      // AB ENABLE

#define E0_STEP_PIN                         NC_PIN
#define E0_DIR_PIN                          NC_PIN
#define E0_ENABLE_PIN                       NC_PIN
#define E1_STEP_PIN                         NC_PIN
#define E1_DIR_PIN                          NC_PIN
#define E1_ENABLE_PIN                       NC_PIN


//
// Temperature Sensors
//
#define TEMP_0_PIN                          PC3   // Analog Input (A VAC SENSE)
#define TEMP_1_PIN                          PC2   // Analog Input (B VAC SENSE)
#define TEMP_BED_PIN                        PC1   // Analog Input (TANK VAC SENSE)

//
// Heaters / Fans
//
#define HEATER_0_PIN                        NC_PIN     // NC_PIN
#define HEATER_1_PIN                        NC_PIN     // NC_PIN

// #define VAC_TANK        // VACUUM TANK with TANK VAC SENSE
#ifdef VAC_TANK
#define HEATER_BED_PIN                      PC10   // EXMOS1
#else
#define HEATER_BED_PIN                      NC_PIN   // NC_PIN
#endif

#define FAN_PIN                             PC9    // P41 "FAN0" - A_VALVE
#define FAN1_PIN                            PC8    // P40 "FAN1" - B_VALVE
#define FAN2_PIN                            PC7    // P39 "FAN2" - A_PUMP
#define FAN3_PIN                            PC6    // P38 "FAN3" - B_PUMP
// #define FAN4_PIN                            PB8    // P24 "FAN4" - EXMOS1
// #define FAN5_PIN                            PB9   // P25 "FAN5" - EXMOS2
// #define FAN6_PIN                            PB6   // P22 "FAN6" - TOP_CAM_LIGHT
// #define FAN7_PIN                            PB7   // P23 "FAN7" - BOTTOM_CAM_LIGHT




//
// Servos
//
#define SERVO0_PIN                          PC12  // SERVO_0

//
// NEOPIXEL
//
#define NEOPIXEL_PIN                        PA0
#define NEOPIXEL2_PIN                       PA1

//
// BEEPER
//
#define BEEPER_PIN                          PC0


//
// I2C
//
#define I2C_SCL_PIN                         PB10
#define I2C_SDA_PIN                         PB3

//
// SPI
//
#define DEFAULT_SPI                         1


//
// RS485
//
#define RS485_DE_PIN                        PA15


//============================================