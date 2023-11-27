/*******************************************************************************
 * @file GyroAccel.hpp
 * @author Andreas Petter
 * @date 2022-12-23
 * @version v1.0
 * @brief abstract hardware driver interface to onboard MPU6050
 *
 * @copyright Copyright (c) 2022 nubix Software-Design GmbH, All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#pragma once

#define MPU6050_INCLUDE_DMP_MOTIONAPPS20
#define MPU_BUS_MAX_BLOCKING           (100) // ms
#define MPU_MIN_TEMPERATURE_SAMPLE     (1000) // ms
#define MPU_MIN_CALIBRATION_INTERVAL   (30 * 1000) //ms (30 second)

#include "I2Cdev.hpp"
#include "helper_3dmath.hpp"
#include "MPU6050.hpp"

typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
} IntVector_t;

typedef __attribute__((aligned(2))) struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} triple_short_t;

typedef __attribute__((aligned(4))) struct
{
    int32_t w;
    int32_t x;
    int32_t y;
    int32_t z;
} quarternation_t;

// Don't change this struchture! Alot of code depends on it.
typedef __attribute__((aligned(4))) struct
{
    quarternation_t quarternion;
    triple_short_t acceleration;
    triple_short_t gyroscope;
    uint8_t gesture[4];
} dmp_data_t; // size of 32 byte is important!

class GyroAccel
{
private:
    I2Cdev m_bus;
    MPU6050 m_sensor;
    bool m_isReady; // set true if DMP init was successful
    dmp_data_t m_dmpData;
    IntVector_t m_orientation, m_acceleration, m_gravity;
    int m_temperature;
    absolute_time_t m_nextTemperatureSample, m_nextCalibration;

    uint16_t m_packetSize; ///< Expected DMP packet size (default is 42 bytes)

public:
    GyroAccel(); // init instance and allocate all resources

    void initialize();
    void readFIFO();
    bool isReady();
    void calibrate();
    IntVector_t &getOrientation();  // [yaw, pitch, roll] angle in tenths of a degree
    IntVector_t &getAcceleration(); // world-frame acceleration, adjusted to remove gravity
                                    // and rotated based on known orientation
    int getTemperature();           // in thenth of °C
};
