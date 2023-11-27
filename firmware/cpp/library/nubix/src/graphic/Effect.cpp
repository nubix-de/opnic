/*******************************************************************************
 * @file Effect.cpp
 * @author Andreas Petter
 * @date 2023-01-02
 * @version v1.0
 * @brief Implementation of few picture mapping effects
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

#include <cassert>

#include "graphic/Effect.hpp"

// assuming, that witdh and height of an immage can't be greater than 1024,
// we use a 2^SCALE_SHIFT multiplication and division on integer numbers,
// which improves performance alot.
#define SCALE_SHIFT 20

Rotation::Rotation(float angle)
{
    angle *= (float) M_PI / 180.0;
    m_sinS = (int)(sinf(angle) * (1 << SCALE_SHIFT));
    m_cosS = (int)(cosf(angle) * (1 << SCALE_SHIFT));
}

Pixel_t *Rotation::transform(int x, int y, Color c)
{
    m_result.x = ((x * m_cosS) >> SCALE_SHIFT) - ((y * m_sinS) >> SCALE_SHIFT);
    m_result.y = ((x * m_sinS) >> SCALE_SHIFT) + ((y * m_cosS) >> SCALE_SHIFT);
    m_result.c = c;
    if (m_nextEffect)
        return m_nextEffect->transform(m_result.x, m_result.y, m_result.c);
    else
        return &m_result;
}

Scale::Scale(float h, float v)
{
    m_hscale = (int)(h * (1 << SCALE_SHIFT));
    m_vscale = (int)(v * (1 << SCALE_SHIFT));
    // only downscaling and flipping is supported, so limit scale factors to this
    if (m_hscale < -(1 << SCALE_SHIFT))
        m_hscale = -(1 << SCALE_SHIFT);
    if (m_hscale > (1 << SCALE_SHIFT))
        m_hscale = (1 << SCALE_SHIFT);
    if (m_vscale < -(1 << SCALE_SHIFT))
        m_vscale = -(1 << SCALE_SHIFT);
    if (m_vscale > (1 << SCALE_SHIFT))
        m_vscale = (1 << SCALE_SHIFT);
}

Pixel_t *Scale::transform(int x, int y, Color c)
{
    // this is ultra fast because only single cycle multiplication and shifting is used
    m_result.x = (x * m_hscale) >> SCALE_SHIFT;
    m_result.y = (y * m_vscale) >> SCALE_SHIFT;
    m_result.c = c;
    if (m_nextEffect)
        return m_nextEffect->transform(m_result.x, m_result.y, m_result.c);
    else
        return &m_result;
}

ColorShift::ColorShift(float hue)
    : m_hue(hue)
{
}

Pixel_t *ColorShift::transform(int x, int y, Color c)
{
    m_result.x = x;
    m_result.y = y;
    m_result.c = colorTransformHue(c, m_hue);
    if (m_nextEffect)
        return m_nextEffect->transform(m_result.x, m_result.y, m_result.c);
    else
        return &m_result;
}
