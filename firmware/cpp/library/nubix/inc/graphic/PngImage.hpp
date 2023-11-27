/*******************************************************************************
 * @file PngImage.hpp
 * @author Andreas Petter
 * @author Vinícius Gabriel Linden
 * @date 2022-12-09
 * @version v1.1
 * @brief Definition of PNG image load and render
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

#include "FrameBuffer.hpp"
#include "pngle.h"

class FrameBuffer;

/**
 * @brief Class for interfacing pngimage with FrameBuffer.
 */
class PngImage
{
public:
    /**
     * Constructor.
     * @param[in] pngData Image data.
     * @param[in] pngSize Image size [bytes].
     */
    PngImage(const uint32_t *pngData, uint32_t pngSize);

    /**
     * Destructor.
     */
    ~PngImage(void);

    /**
     * Render PNG into an existing FrameBuffer at given position.
     * @param[in] buffer Frame buffer to apply image to.
     * @param[in] x Horizontal position [px].
     * @param[in] y Vertical position [px].
     */
    void render(FrameBuffer &buffer, uint32_t x, uint32_t y);

    /**
     * Create a new FrameBuffer with size and content of PNG image.
     * @return New frame buffer with image.
     */
    FrameBuffer *render(void);

    /**
     * Get image width.
     * @note This does not test if data is a PNG.
     * @return Image width.
     */
    uint32_t widthGet(void) const;

    /**
     * Get image height.
     * @note This does not test if data is a PNG.
     * @return Image height.
     */
    uint32_t heightGet(void) const;

private:
    //! Image data.
    const uint32_t *c_pData;
    //! Image size [bytes].
    const uint32_t c_dataSize;
    //! pngle handler.
    pngle_t *m_pngHandle;
};
