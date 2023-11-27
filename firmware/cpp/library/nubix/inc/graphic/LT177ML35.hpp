/*******************************************************************************
 * @file LT177ML35.hpp
 * @author Andreas Petter
 * @date 2022-11-24
 * @version v1.0
 * @brief hardware driver to TFT display type LT177ML35 using GC9106 controller
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

#include <hardware/pio.h>
#include "Display.hpp"

#define LED_PWM_FREQ 220 // 220Hz should be flicker free

class LT177ML35 : public Display
{
public:
    ~LT177ML35();

    static LT177ML35 &getInstance();
    constexpr unsigned int getWidth() override
    {
        return DISP_WIDTH;
    }

    constexpr unsigned int getHeight() override
    {
        return DISP_HEIGHT;
    }

    void update(uint32_t *frameBuffer, uint32_t pixelCount, bool vSync) override;
    void setBrightness(unsigned int percent);
    void setStatusLED(Color color);

private:
    const PIO c_pio;
    unsigned int m_sm_cmd_dat, m_sm_dat3_bgr, m_pio_offset, m_dmaTX, m_brightness;

    LT177ML35();
    void init();
    void initPIO();
    void initPWM(unsigned int pin);
    void writeCmd(uint8_t cmd);
    void writeData(uint8_t cmd);

    // no copy constructor or assignment operator = to avoid multiple instances
    LT177ML35(const LT177ML35&) = delete;
    LT177ML35(LT177ML35&&) = delete;
    LT177ML35& operator=(const LT177ML35&) = delete;
    LT177ML35& operator=(LT177ML35&&) = delete;
};
