/*******************************************************************************
 * @file Display.hpp
 * @author Andreas Petter
 * @date 2022-12-05
 * @version v1.0
 * @brief generic hardware driver interface different displays
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

#include <stdint.h>

#include "Color.hpp"

class Display {
public:
    Display() {}
    virtual ~Display() {}
    constexpr virtual unsigned int getWidth() = 0;
    constexpr virtual unsigned int getHeight() = 0;

    /**
     * @brief Transfer content of framebuffer to display using the fastest way.
     *
     * @param frameBuffer frame buffer to transfer, never NULL
     * @param pixelCount total number of pixels should match to display
     * @param vSync enable sync with frame refresh to avoid tearing effects with fast motions
     */
    virtual void update(uint32_t* frameBuffer, uint32_t pixelCount, bool vSync) = 0;

    /**
     * @brief Set the brightness of displays background LED
     *
     * @param percent 0..100
     */
    virtual void setBrightness(unsigned int percent) = 0;

    /**
     * @brief Set the color of the status LED next to the display
     *
     * @param color Color::Black is off; alpha channel is ignored
     */
    virtual void setStatusLED(Color color) = 0;
};
