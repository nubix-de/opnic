/*******************************************************************************
 * @file Font.hpp
 * @author Andreas Petter
 * @date 2022-11-28
 * @version v1.0
 * @brief Definition of a generic font class.
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

// Each font have to be encoded as 8bit grayscale uncompressed BMP file with white text on black ground.
// GraphicsConverter is a powerful tool to generate header files out of a bitmap.
// The image must contain all ASCII characters from 0x21 to 0x7f (count=95)
// !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~°
//
// usage example: Font font1 = {Azaret_large_WIDTH, Azaret_large_HEIGHT, &Azaret_largeData[0]};
#include "Font_Courier_small.h"
#include "Font_Courier_large.h"
#include "Font_Courier_bold.h"
#include "Font_Azaret_small.h"
#include "Font_Azaret_large.h"

typedef struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 1:Grayscale, 3:RGB, 4:RGBA */
  unsigned char* pixel_data;
} FontData_t;

class Font
{
    private:
        const unsigned int c_width, c_height;
        const uint8_t *c_pData;

    public:
        Font(unsigned int width, unsigned int height, const uint8_t* pData);

        unsigned int getHeight() const;
        unsigned int getWidth() const;
        const uint8_t* getData(char c, unsigned int scanline) const;
};

