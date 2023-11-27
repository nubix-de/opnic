/*******************************************************************************
 * @file GyroAccel.cpp
 * @author Andreas Petter
 * @date 2022-12-23
 * @version v1.0
 * @brief abstract hardware driver implementation to onboard MPU6050
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

#include <cmath>
#include <pico/stdlib.h>
#include <pico/sem.h>

#include "sensor/GyroAccel.hpp"
#include "MPU6050_6Axis_MotionApps_V6_12.hpp"

semaphore_t _bus_access;

__attribute__((constructor)) // called before main()
static void
_prepareGyroAccelSemaphore()
{
    sem_init(&_bus_access, 1, 1);
}

GyroAccel::GyroAccel()
    : m_bus(i2c1, 400 * 1000, OPNIC_MCU_SDA, OPNIC_MCU_SCL),
      m_sensor(m_bus),
      m_isReady(false)
{
    m_dmpData.gesture[2] = UINT8_MAX; // mark data as dirty
    m_dmpData.gesture[3] = UINT8_MAX;
}

void GyroAccel::initialize()
{
    if (!m_isReady && sem_acquire_timeout_ms(&_bus_access, MPU_BUS_MAX_BLOCKING))
    {
        m_sensor.initialize();
         if (0 == m_sensor.dmpInitialize())
         {
            m_sensor.dmpSetFIFORate(20);
            m_sensor.getIntStatus();
            m_sensor.setTempSensorEnabled(true);
            m_sensor.setDMPEnabled(true); // turn on the DMP, now that it's ready
            m_sensor.getIntStatus();
            m_isReady = true;
            m_packetSize = m_sensor.dmpGetFIFOPacketSize();
            m_sensor.dmpSetFIFORate(20);
            m_sensor.setTempSensorEnabled(true);
         }
        sem_release(&_bus_access);
    }
}

void GyroAccel::readFIFO()  //it needs 1.1 ms and intended to be called within a interrupt
{
    uint8_t mpuIntStatus; // holds actual interrupt status byte from MPU
    uint16_t fifoCount;   // count of all bytes currently in FIFO

    if (m_isReady && sem_try_acquire(&_bus_access))
    {
        mpuIntStatus = m_sensor.getIntStatus();
        fifoCount = m_sensor.getFIFOCount();
        if (mpuIntStatus & (1 << MPU6050_INTERRUPT_FIFO_OFLOW_BIT) || // check for overflow (this should never happen unless our code is too inefficient)
            sizeof(m_dmpData) != fifoCount)
        {
            m_sensor.resetFIFO(); // reset so we can continue cleanly
        }
        else if (mpuIntStatus & 0x01)
        {
            while (fifoCount < sizeof(m_dmpData))
                    fifoCount = m_sensor.getFIFOCount();

            m_sensor.getFIFOBytes((uint8_t *)&m_dmpData, sizeof(m_dmpData)); // read a packet from FIFO
            fifoCount -= sizeof(m_dmpData);
        }
        sem_release(&_bus_access);
    }
}

bool GyroAccel::isReady()
{
    return m_isReady;
}

void GyroAccel::calibrate()
{
    absolute_time_t now = get_absolute_time();

    if (m_isReady &&
        now._private_us_since_boot > m_nextCalibration._private_us_since_boot &&
        sem_acquire_timeout_ms(&_bus_access, MPU_BUS_MAX_BLOCKING))
    {
        m_sensor.CalibrateAccel(6);
        m_sensor.CalibrateGyro(6);
        sem_release(&_bus_access);
        m_nextCalibration = make_timeout_time_ms(MPU_MIN_CALIBRATION_INTERVAL);
    }
}

static bool _isQaternationValid(quarternation_t &q)
{
    int32_t temp;

    temp = q.w * q.w;
    temp += q.x * q.x;
    temp += q.y * q.y;
    temp += q.z * q.z;

    return (temp > 0xF000000 && temp < 0x11000000);
}

// get gravity vector (still in integer) from quartenation
// +1g corresponds to +8192, sensitivity is 2g.
static void _getGravity(IntVector_t &result, quarternation_t &q)
{
    result.x = (q.x * q.z - q.w * q.y) >> 13;
    result.y = (q.w * q.x + q.y * q.z) >> 13;
    result.z = (q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z) >> 14;
}

IntVector_t &GyroAccel::getOrientation()
{
    VectorFloat result;
    volatile int32_t tmp1, tmp2;

    if (UINT8_MAX != m_dmpData.gesture[3]) // new data to process?
    {
        this->getAcceleration(); // this gets quartenation and gravity vector
        // calculate yaw, pitch and roll
        // yaw: (about Z axis)
        tmp1 = m_dmpData.quarternion.x * m_dmpData.quarternion.y;
        tmp1 -= m_dmpData.quarternion.w * m_dmpData.quarternion.z;
        tmp2 = m_dmpData.quarternion.w * m_dmpData.quarternion.w;
        tmp2 -= 0x8000000;
        tmp2 += m_dmpData.quarternion.x * m_dmpData.quarternion.x;
        result.x = atan2f((float)(tmp1 >> 13) / 16384.0f, (float)(tmp2 >> 13) / 16384.0f); // first time using float
        // pitch: (nose up/down, about Y axis)
        tmp1 = m_gravity.y * m_gravity.y;
        tmp1 += m_gravity.z * m_gravity.z;
        result.y = atan2f((float)m_gravity.x / 16384.0f, (float)(tmp2 >> 13) / 16384.0f);
        // roll: (tilt left/right, about X axis)
        result.z = atan2f((float)m_gravity.y / 16384.0f, (float)m_gravity.z / 16384.0f);
        if (m_gravity.z < 0)
        {
            if (result.y > 0)
                result.y = M_PI - result.y;
            else
                result.y = -M_PI - result.y;
        }
        m_orientation.x = result.x * (1800. / M_PI); // yaw
        m_orientation.y = result.y * (1800. / M_PI); // pitch
        m_orientation.z = result.z * (1800. / M_PI); // roll

        // mark raw data as processed
        m_dmpData.gesture[3] = UINT8_MAX;
    }
    return m_orientation;
}

IntVector_t &GyroAccel::getAcceleration()
{
    volatile int32_t tmp1;

    if (UINT8_MAX != m_dmpData.gesture[2]) // new data to process?
    {
        // change byte order from big endian (sensor) to little endian (ARM)
        asm volatile("ldr %[tmp], [%[ptr], #0]  \n\t" // first revese 4 words
                     "rev %[tmp], %[tmp]        \n\t"
                     "asr %[tmp], #16           \n\t" // make it short
                     "str %[tmp], [%[ptr], #0]  \n\t"
                     "ldr %[tmp], [%[ptr], #4]  \n\t"
                     "rev %[tmp], %[tmp]        \n\t"
                     "asr %[tmp], #16           \n\t"
                     "str %[tmp], [%[ptr], #4]  \n\t"
                     "ldr %[tmp], [%[ptr], #8]  \n\t"
                     "rev %[tmp], %[tmp]        \n\t"
                     "asr %[tmp], #16           \n\t"
                     "str %[tmp], [%[ptr], #8]  \n\t"
                     "ldr %[tmp], [%[ptr], #12] \n\t"
                     "rev %[tmp], %[tmp]        \n\t"
                     "asr %[tmp], #16           \n\t"
                     "str %[tmp], [%[ptr], #12] \n\t"
                     "ldr %[tmp], [%[ptr], #16] \n\t" // then reverse 6 half words
                     "rev16 %[tmp], %[tmp]      \n\t"
                     "str %[tmp], [%[ptr], #16] \n\t"
                     "ldr %[tmp], [%[ptr], #20] \n\t"
                     "rev16 %[tmp], %[tmp]      \n\t"
                     "str %[tmp], [%[ptr], #20] \n\t"
                     "ldr %[tmp], [%[ptr], #24] \n\t"
                     "rev16 %[tmp], %[tmp]      \n\t"
                     "str %[tmp], [%[ptr], #24] \n\t"
                     : [tmp] "=&r"(tmp1)
                     : [ptr] "r"(&m_dmpData)
                     : "cc", "memory");
        if (!_isQaternationValid(m_dmpData.quarternion))
            return m_acceleration;
        _getGravity(m_gravity, m_dmpData.quarternion);
        m_acceleration.x = (m_dmpData.acceleration.x - m_gravity.x) >> 6;
        m_acceleration.y = (m_dmpData.acceleration.y - m_gravity.y) >> 6;
        m_acceleration.z = (m_dmpData.acceleration.z - m_gravity.z) >> 6;
        // mark raw data as processed
        m_dmpData.gesture[2] = UINT8_MAX;
    }
    return m_acceleration;
}

int GyroAccel::getTemperature()
{
    absolute_time_t now = get_absolute_time();

    if (m_isReady &&
        now._private_us_since_boot > m_nextTemperatureSample._private_us_since_boot &&
        sem_acquire_timeout_ms(&_bus_access, MPU_BUS_MAX_BLOCKING))
    {
        m_temperature = m_sensor.getTemperature();
        m_temperature = 350 + (m_temperature / 34);
        sem_release(&_bus_access);
        m_nextTemperatureSample = make_timeout_time_ms(MPU_MIN_TEMPERATURE_SAMPLE);
    }
    return m_temperature;
}
