/*******************************************************************************
 * @file FrameBuffer.hpp
 * @author Ted Rossin
 * @author Andreas Petter
 * @date 2022-11-25
 * @version v1.2
 * @brief Definition of some 2D graphic primitives
 *
 * @copyright Copyright (c) 2022 nubix Software-Design GmbH, All rights reserved.
 * @note Sources based on the work of Ted Rossin and used with his kind permission
 *       to publish this as open source.
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
#include "Font.hpp"
#include "PngImage.hpp"
#include "Display.hpp"
#include "Effect.hpp"

class FrameBuffer;

typedef void (FrameBuffer::*DotFuncPtr)(void);

class FrameBuffer
{
public:
    FrameBuffer(Display &display);
    FrameBuffer(unsigned int width, unsigned int height);
    ~FrameBuffer(void);

    void show(bool vSync);
    void clear(Color color);

    unsigned int get_width() { return c_width; };
    unsigned int get_height() { return c_height; };

    void point(unsigned int x, unsigned int y, Color color);
    void line(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, Color color);
    void line_soft(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, Color color);
    void line_soft2(int x1, int y1, int x2, int y2, int thickness, Color color);
    void rectangle(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, Color color);
    void round_rectangle(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int radius, Color color);
    void rectangle_filled(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, Color color);
    void round_rectangle_filled(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int radius, Color color);
    void circle(unsigned int xc, unsigned int yc, unsigned int radius, Color color);
    void circle_filled(unsigned int xc, unsigned int yc, unsigned int radius, Color color);
    void circle_filled2(int xc, int yc, unsigned int radius, Color color);
    void circle_filled3(int xc, int yc, unsigned int radius, Color color);
    void circle_filled4(int xc, int yc, unsigned int radius, Color color);
    void circle_filled5(int xc, int yc, unsigned int radius, unsigned int thickness, Color color);
    void circle_filled6(int xc, int yc, unsigned int radius, Color color);
    void blit(unsigned int x0, unsigned int y0, FrameBuffer *frame); //based on upper left corner
    void blit(unsigned int xc, unsigned int yc, FrameBuffer *frame, Effect& effect); //based on center of source image

    /**
     * @brief Blit the given image considering the position to draw, and slicing the image accordingly
     *
     * @param x0 X-coordinate where we should draw on the FrameBuffer
     * @param y0 Y-coordinate where we should draw on the FrameBuffer
     * @param x1 X-coordinate from where we should start drawing the origin image into the FrameBuffer
     * @param y1 Y-coordinate from where we should start drawing the origin image into the FrameBuffer
     * @param width From the X1 and Y1 coordinates, what is the width to draw on the FrameBuffer from the origin image
     * @param height From the X1 and Y1 coordinates, what is the height to draw the FrameBuffer from the origin image
     * @param frame FrameBuffer of the origin image
     */
    void blit(int x0, int y0, unsigned int x1, unsigned int y1, unsigned int width, unsigned int height, FrameBuffer *frame); //based on upper left corner

    /**
     * @brief Draw the given text on the framebuffer
     *
     * @param x0 X-coordinate of the top-left corner of the text
     * @param y0 Y-coordinate of the top-left corner of the text
     * @param string The string to be drawn
     * @param font The font type to be used
     * @param foreG Foreground color of the font
     * @param backG Background color of the font
     *
     * @return Returns the width of the drawn text
     */
    unsigned int text(int x0, int y0, const char *string, const Font &font, Color foreG, Color backG);

    void line_thick(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int thickness, Color color);

protected:
    const unsigned int c_width, c_height, c_buffSize;
    Display *c_pDisplay;
    Color *m_buffer;
    Color *m_position;
    Color *m_boundary;
    Color m_col;
    DotFuncPtr m_dotFunc;

    void _setColor(Color color);
    void _setPos(unsigned int x, unsigned int y);
    void _dot();       // set m_col to m_position when it is below m_boundary
    void _alpha_dot(); // color merge according to alpha channel of m_col
    void _char(unsigned int x0, unsigned int y0, const char c, const Font &font, Color foreG, Color backG);
};
