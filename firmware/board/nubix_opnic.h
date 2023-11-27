/*******************************************************************************
 * @file nubix_opnic.h
 * @author Andreas Petter
 * @author Vinícius Linden
 * @date 2022-11-25
 * @version v1.0
 * @brief Definition of board defaults
 *
 * @copyright Copyright (c) 2022 nubix Software-Design GmbH, All rights
 *reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 *BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#ifndef NUBIX_OPNIC_H
#define NUBIX_OPNIC_H

#define OPNIC

// custom flash configuration
#define PICO_XOSC_STARTUP_DELAY_MULTIPLIER (64)
#define PICO_BOOT_STAGE2_CHOOSE_GENERIC_03H (1)
#define PICO_FLASH_SPI_CLKDIV (4)
#define PICO_FLASH_SIZE_BYTES (512 * 1024)
#define PICO_RP2040_B0_SUPPORTED (0)
#define PICO_RP2040_B1_SUPPORTED (0)
#define PICO_RP2040_B2_SUPPORTED (1)
// drive high to force power supply into PWM mode (lower ripple on 3V3 at light
// loads)
#define PICO_SMPS_MODE_PIN (23)

#define PICO_DEFAULT_LED_PIN (25)
#define PICO_DEFAULT_LED_PIN_INVERTED (1)

#define PICO_DEFAULT_UART_BAUD_RATE (115200)
#define PICO_DEFAULT_UART 1
#define PICO_DEFAULT_UART_TX_PIN (20)
#define PICO_DEFAULT_UART_RX_PIN (21)

#define PICO_DEFAULT_I2C 1
#define PICO_DEFAULT_I2C_SDA_PIN (18)
#define PICO_DEFAULT_I2C_SCL_PIN (19)

#define OPNIC_TOUCH_A (17)
#define OPNIC_TOUCH_B (14)
#define OPNIC_TOUCH_C (16)
#define OPNIC_TOUCH_D (15)

/*
 * NOTE the first 11 pins (DB0 .. DC) have to be in this exact
 * consecutive order to make the pio state machines work
 */
#define OPNIC_LCD_DB0 (0)
#define OPNIC_LCD_DB1 (1)
#define OPNIC_LCD_DB2 (2)
#define OPNIC_LCD_DB3 (3)
#define OPNIC_LCD_DB4 (4)
#define OPNIC_LCD_DB5 (5)
#define OPNIC_LCD_DB6 (6)
#define OPNIC_LCD_DB7 (7)
#define OPNIC_LCD_RDN (8)
#define OPNIC_LCD_WRN (9)
#define OPNIC_LCD_DC (10)
#define OPNIC_LCD_RSTN (11)
#define OPNIC_LCD_CSN (12)
#define OPNIC_LCD_TE (13)
#define OPNIC_LCD_BACKLIGHT (22)

#define OPNIC_LED_RED (23)
#define OPNIC_LED_GREEN (25)
#define OPNIC_LED_BLUE (24)

#define OPNIC_LCD_DISPLAY (22)
#define DISP_WIDTH (160)  //!< Display width [px]
#define DISP_HEIGHT (128) //!< Display height [px]

#define OPNIC_MCU_SDA PICO_DEFAULT_I2C_SDA_PIN
#define OPNIC_MCU_SCL PICO_DEFAULT_I2C_SCL_PIN
#define OPNIC_MPU_INT (26)

#endif /* NUBIX_OPNIC_H */
