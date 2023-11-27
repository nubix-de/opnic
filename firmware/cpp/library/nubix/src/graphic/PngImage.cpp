/*******************************************************************************
 * @file PngImage.cpp
 * @author Andreas Petter
 * @author Vinícius Gabriel Linden
 * @date 2022-12-09
 * @version v1.0
 * @brief Implementation of PNG image load and render using 3rd party 'pngle'
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

#include <pico/mutex.h>

#include "graphic/PngImage.hpp"
#include "common/endian.hpp"

auto_init_mutex(render_mutex);

static FrameBuffer *_buffer;

PngImage::PngImage(const uint32_t *pngData, uint32_t pngSize)
    : c_pData(pngData),
      c_dataSize(pngSize)
{
    m_pngHandle = pngle_new();
}

PngImage::~PngImage(void)
{
    pngle_destroy(m_pngHandle);
    m_pngHandle = nullptr;
}

void PngImage::render(FrameBuffer &buffer, uint32_t x, uint32_t y)
{
    int fed;
    size_t remain = c_dataSize;
    const uint8_t *pData = (uint8_t *)c_pData;
    static uint32_t x_offset, y_offset;

    if (x > buffer.get_width() || y > buffer.get_height())
        return;

    mutex_enter_blocking(&render_mutex); // only one instance can render an image
    _buffer = &buffer;
    x_offset = x;
    y_offset = y;
    pngle_reset(m_pngHandle);
    pngle_set_draw_callback(m_pngHandle, [](pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]) -> void
                            {
        argb_t temp = {{.blue = rgba[2], .green = rgba[1], .red = rgba[0], .alpha = rgba[3]}};

        _buffer->point(x + x_offset,y + y_offset,(Color)temp.raw); });
    while (remain)
    {
        fed = pngle_feed(m_pngHandle, pData, remain);
        if (fed <= 0)
            break;
        pData += fed;
        remain -= fed;
    }
    mutex_exit(&render_mutex);
}

FrameBuffer* PngImage::render(void)
{
    int fed;
    size_t remain = c_dataSize;
    const uint8_t *pData = (uint8_t *)c_pData;

    mutex_enter_blocking(&render_mutex); // only one instance can render an image
    pngle_reset(m_pngHandle);
    pngle_set_init_callback(m_pngHandle, [](pngle_t *pngle, uint32_t w, uint32_t h) -> void {
        _buffer = new FrameBuffer(w, h);
        _buffer->clear(Color::Opaque);
    });
    pngle_set_draw_callback(m_pngHandle, [](pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]) -> void
                            {
        argb_t temp = {{.blue = rgba[2], .green = rgba[1], .red = rgba[0], .alpha = rgba[3]}};

        _buffer->point(x,y,(Color)temp.raw); });
    while (remain)
    {
        fed = pngle_feed(m_pngHandle, pData, remain);
        if (fed <= 0) { //decode error :(
            delete _buffer;
            _buffer = NULL;
            break;
        }
        pData += fed;
        remain -= fed;
    }
    mutex_exit(&render_mutex);
    return _buffer;
}

uint32_t PngImage::widthGet(void) const {
    // NOTE pngle_get_width will not work before rendering.
    return SWAP_ENDIAN_32(c_pData[4]);
}

uint32_t PngImage::heightGet(void) const {
    // NOTE pngle_get_height will not work before rendering.
    return SWAP_ENDIAN_32(c_pData[5]);
}
