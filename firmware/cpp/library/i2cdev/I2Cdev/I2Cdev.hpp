/*******************************************************************************
 * @file I2Cdev.hpp
 * @author Jeff Rowberg <jeff@rowberg.net>
 * @author Andreas Petter
 * @date 2013-06-05
 * @brief Raspberry Pi Pico port for I2Cdev library collection.
 * Main I2C device class.
 * Abstracts bit and byte I2C R/W functions into a convenient class.
 *
 * Changelog:
 *      2021-09-29 - Initial port release by Gino Ipóliti.
 *      2022-12-22 - allow multiple i2c instances.
 *      2022-12-22 - initialize I²C instances by Andreas Petter.
 *
 * @copyright Copyright (c) 2013 Jeff Rowberg
 *
 * I2Cdev device library code is placed under the MIT license
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/
#pragma once

#include <hardware/i2c.h>

// 1000ms default read timeout (modify with "I2Cdev::readTimeout = [ms];")
#define I2CDEV_DEFAULT_READ_TIMEOUT ((uint32_t)1000000) // RP2040 I2C functions with timeout use microseconds so we have to multiply by 10^3

class I2Cdev
{
private:
    i2c_inst_t* const c_bus;

public:
    I2Cdev(i2c_inst_t *bus, uint32_t baudrate, uint32_t sda_pin, uint32_t scl_pin);

    int8_t readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint32_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);
    int8_t readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data, uint32_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);
    int8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint32_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);
    int8_t readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data, uint32_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);
    int8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint32_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);
    int8_t readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint32_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);
    int8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint32_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);
    int8_t readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint32_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);

    bool writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
    bool writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data);
    bool writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
    bool writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
    bool writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
    bool writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
    bool writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
    bool writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);
};
