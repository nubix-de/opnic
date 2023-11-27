/*******************************************************************************
 * @file Effect.hpp
 * @author Andreas Petter
 * @date 2023-01-02
 * @version v1.0
 * @brief Definition of few picture mapping effects
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

typedef struct
{
    int x;
    int y;
    Color c;
} Pixel_t;

class Effect
{
protected:
    Effect* m_nextEffect = NULL;
public:
    virtual Pixel_t *transform(int x, int y, Color c) = 0; // transform source pixel into destination pixel
    void setNextEffect(Effect& next) { m_nextEffect = &next; };
};

class Rotation : public Effect
{
private:
    Pixel_t m_result;
    int m_cosS, m_sinS;

public:
    Rotation(float angle);
    Pixel_t *transform(int x, int y, Color c);
};

class Scale : public Effect // only downscaling is supported
{
private:
    Pixel_t m_result;
    int m_hscale, m_vscale;

public:
    Scale(float h, float v); // h and v must be in range 0.0 .. 1.0
    Pixel_t *transform(int x, int y, Color c);
};

class ColorShift : public Effect
{
private:
    Pixel_t m_result;
    float m_hue;

public:
    ColorShift(float hue);
    Pixel_t *transform(int x, int y, Color c);
};
