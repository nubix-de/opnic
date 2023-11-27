/*******************************************************************************
 * @file FrameBuffer.cpp
 * @author Ted Rossin
 * @author Andreas Petter
 * @date 2022-11-20
 * @version v1.1
 * @brief Definition of some 2D graphic primitives
 *
 * @copyright Copyright (c) 2022 nubix Software-Design GmbH, All rights reserved.
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
#include <algorithm>
#include <cstring>

#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <hardware/dma.h>

#include "graphic/FrameBuffer.hpp"
#include "graphic/Font.hpp"

static int _dmaClear;
auto_init_mutex(clear_mutex);

__attribute__((constructor)) // called before main()
static void
_prepareCriticalSection()
{
    // initialize DMA for clear method
    _dmaClear = dma_claim_unused_channel(true);
    dma_channel_config conf = dma_channel_get_default_config(_dmaClear);
    channel_config_set_transfer_data_size(&conf, DMA_SIZE_32);
    channel_config_set_read_increment(&conf, false);
    channel_config_set_write_increment(&conf, true);
    dma_channel_set_config(_dmaClear, &conf, false);
}

FrameBuffer::FrameBuffer(Display &display)
    : c_pDisplay(&display),
      c_width(display.getWidth()),
      c_height(display.getHeight()),
      c_buffSize(c_width * c_height)
{
    m_buffer = new Color[c_buffSize];
    m_boundary = &m_buffer[c_buffSize + 1];
    _setColor(Color::White);
}

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height)
    : c_pDisplay(NULL),
      c_width(width),
      c_height(height),
      c_buffSize(c_width * c_height)
{
    m_buffer = new Color[c_buffSize];
    m_boundary = &m_buffer[c_buffSize + 1];
    _setColor(Color::White);
}

FrameBuffer::~FrameBuffer()
{
    delete m_buffer;
}

void FrameBuffer::show(bool vSync)
{
    if (c_pDisplay)
        c_pDisplay->update((uint32_t *)&m_buffer[0], c_buffSize, vSync);
}

void FrameBuffer::clear(Color color)
{
    mutex_enter_blocking(&clear_mutex);
    // this is the fastest way with 532Mbyte/s (155µs)
    dma_channel_set_read_addr(_dmaClear, &color, false);
    dma_channel_set_write_addr(_dmaClear, &m_buffer[0], false);
    dma_channel_set_trans_count(_dmaClear, c_buffSize, true);
    dma_channel_wait_for_finish_blocking(_dmaClear);
    mutex_exit(&clear_mutex);
}

void FrameBuffer::_setPos(unsigned int x, unsigned int y)
{
    static Color dummy_position; // invisible dot, when out of bounds

    if (x >= c_width || y >= c_height)
        m_position = &dummy_position;
    else
        m_position = x * c_height + y + &m_buffer[0]; // Display is rotated
}

// set m_curCol to m_position when it is below m_boundary (performed in 80ns)
void FrameBuffer::_dot()
{
    *m_position = m_col;
}

// color merge
void FrameBuffer::_alpha_dot()
{
    *m_position = colorAlphaBlend(m_col, *m_position); // apply m_col over backround
}

void FrameBuffer::_setColor(Color color)
{
    m_col = color;
    if (0xff000000 == (color & 0xff000000))
        m_dotFunc = &FrameBuffer::_dot;
    else
        m_dotFunc = &FrameBuffer::_alpha_dot;
}

void FrameBuffer::point(unsigned int x, unsigned int y, Color color)
{
    _setColor(color);
    _setPos(x, y);
    (this->*m_dotFunc)();
}

// performed in less than 45µs (max)
void FrameBuffer::line(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, Color color)
{
    int distX = std::abs((int)x1 - (int)x0), stepX = x0 < x1 ? c_height : c_height * -1;
    int distY = std::abs((int)y1 - (int)y0), stepY = y0 < y1 ? 1 : -1;

    if (0 == distY) // simple horizontal line
    {
        point(stepX > 0 ? x0 : x1, y0, color); // left start
        while (distX--)
        {
            m_position += c_height;
            (this->*m_dotFunc)();
        }
    }
    else if (0 == distX) // simple vertical line
    {
        point(x0, stepY > 0 ? y0 : y1, color); // top start
        while (distY--)
        {
            m_position++;
            (this->*m_dotFunc)();
        }
    }
    else // Bresenham algorithm
    {
        int err = (distX > distY ? distX : -distY) / 2, e2;
        Color *destPos = x1 * c_height + y1 + &m_buffer[0]; // Display is rotated

        point(x0, y0, color);
        while (m_position != destPos)
        {
            e2 = err;
            if (e2 > -distX)
            {
                err -= distY;
                m_position += stepX;
            }
            if (e2 < distY)
            {
                err += distX;
                m_position += stepY;
            }
            (this->*m_dotFunc)();
        }
    }
}

// with help of Xiaolin Wu's line algorithm
// https://www.geeksforgeeks.org/anti-aliased-line-xiaolin-wus-algorithm/
void FrameBuffer::line_soft(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, Color color)
{
    int distX = std::abs((int)x1 - (int)x0);
    int distY = std::abs((int)y1 - (int)y0);

    if (0 == distX || 0 == distY)
    {
        this->line(x0, y0, x1, y1, color);
        return;
    }
    bool steep = distY > distX;

    // swap the co-ordinates if slope > 1 or we
    // draw backwards
    if (steep)
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    // compute the slope
    float dx = (int)x1 - (int)x0;
    float dy = (int)y1 - (int)y0;
    float gradient = dy / dx;
    if (dx == 0.0f)
        gradient = 1;

    int xpxl1 = x0;
    int xpxl2 = x1;
    float intersectY = y0;

    if (steep)
    {
        for (int x = xpxl1; x <= xpxl2; x++)
        {
            // pixel coverage is determined by fractional part of y co-ordinate
            this->point((unsigned int)intersectY, x, colorSetAlphaF(color, modf(intersectY, NULL)));
            this->point(((unsigned int)intersectY) - 1, x, colorSetAlphaF(color, 1.0f - modf(intersectY, NULL)));
            intersectY += gradient;
        }
    }
    else
    {
        for (int x = xpxl1; x <= xpxl2; x++)
        {
            // pixel coverage is determined by fractional part of y co-ordinate
            this->point(x, (unsigned int)intersectY, colorSetAlphaF(color, modf(intersectY, NULL)));
            this->point(x, ((unsigned int)intersectY) - 1, colorSetAlphaF(color, 1.0f - modf(intersectY, NULL)));
            intersectY += gradient;
        }
    }
}


void FrameBuffer::line_soft2(int x1, int y1, int x2, int y2, int thickness, Color color)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int e2;
    int x = x1;
    int y = y1;

    for (int i = 0; i < thickness; i++) {
        point(x, y + i, color); // Draw the center line

        // Draw the parallel lines above and below the center line
        if (i > 0) {
            point(x, y - i, color);
        }
    }

    while (!(x == x2 && y == y2)) {
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }

        for (int i = 0; i < thickness; i++) {
            point(x, y + i, color); // Draw the center line
            if (i > 0) {
                point(x, y - i, color); // Draw the parallel lines above and below
            }
        }
    }
}

// performed in less than 100µs (max)
void FrameBuffer::rectangle(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, Color color)
{
    line(x0, y0, x1, y0, color);
    line(x0, y1, x1, y1, color);
    line(x0, y0, x0, y1, color);
    line(x1, y0, x1, y1, color);
}

// with help of Breseham circle algorithm
void FrameBuffer::round_rectangle(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int radius, Color color)
{
    int f, ddF_x, ddF_y, x, y, distX, distY, xc, yc;

    // make sure x0,y0 is top left corner
    if (x0 > x1)
        std::swap(x0, x1);
    if (y0 > y1)
        std::swap(y0, y1);

    distX = x1 - x0;
    distY = y1 - y0;

    // limit radius if rect is too small
    if (((int) radius) > (distX >> 1))
        radius = distX >> 1;
    if (((int) radius) > (distY >> 1))
        radius = distY >> 1;

    f = 1 - radius;
    ddF_x = 0;
    ddF_y = -2 * radius;
    x = 0;
    y = radius;
    distX += x0 - radius;
    distY += y0 - radius;
    xc = x0 + radius;
    yc = y0 + radius;

    line(x0 + radius, y0, x1 - radius, y0, color);
    line(x0 + radius, y1, x1 - radius, y1, color);
    line(x0, y0 + radius, x0, y1 - radius, color);
    line(x1, y0 + radius, x1, y1 - radius, color);
    _setPos(distX, distY + radius);
    (this->*m_dotFunc)();
    _setPos(xc, yc - radius);
    (this->*m_dotFunc)();
    _setPos(distX + radius, yc);
    (this->*m_dotFunc)();
    _setPos(xc - radius, yc);
    (this->*m_dotFunc)();

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;
        _setPos(x + distX, y + distY);
        (this->*m_dotFunc)();
        _setPos(xc - x, y + distY);
        (this->*m_dotFunc)();
        _setPos(x + distX, yc - y);
        (this->*m_dotFunc)();
        _setPos(xc - x, yc - y);
        (this->*m_dotFunc)();
        _setPos(y + distX, x + distY);
        (this->*m_dotFunc)();
        _setPos(xc - y, x + distY);
        (this->*m_dotFunc)();
        _setPos(y + distX, yc - x);
        (this->*m_dotFunc)();
        _setPos(xc - y, yc - x);
        (this->*m_dotFunc)();
    }
}

// performed in less than 3.26ms (max)
void FrameBuffer::rectangle_filled(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, Color color)
{
    if (x0 > x1)
        std::swap(x0, x1);

    for (unsigned int x = x0; x <= x1; x++)
        line(x, y0, x, y1, color);
}

void FrameBuffer::round_rectangle_filled(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int radius, Color color)
{
    int f, ddF_x, ddF_y, x, y, distX, distY, xc, yc;

    // make sure x0,y0 is top left corner
    if (x0 > x1)
        std::swap(x0, x1);
    if (y0 > y1)
        std::swap(y0, y1);

    distX = x1 - x0;
    distY = y1 - y0;

    // limit radius if rect is too small
    if (((int) radius) > (distX >> 1))
        radius = distX >> 1;
    if (((int) radius) > (distY >> 1))
        radius = distY >> 1;

    f = 1 - radius;
    ddF_x = 0;
    ddF_y = -2 * radius;
    x = 0;
    y = radius;
    distX += x0 - radius;
    distY += y0 - radius;
    xc = x0 + radius;
    yc = y0 + radius;

    rectangle_filled(xc, y0, distX, y1, color);
    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;
        line(x + distX, y + distY, x + distX, yc - y, color);
        line(xc - x, y + distY, xc - x, yc - y, color);
        line(y + distX, x + distY, y + distX, yc - x, color);
        line(xc - y, x + distY, xc - y, yc - x, color);
    }
}

// Breseham circle algorithm; performed in less than 76µs (max)
void FrameBuffer::circle(unsigned int xc, unsigned int yc, unsigned int radius, Color color)
{
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    _setColor(color);
    _setPos(xc, yc + radius);
    (this->*m_dotFunc)();
    _setPos(xc, yc - radius);
    (this->*m_dotFunc)();
    _setPos(xc + radius, yc);
    (this->*m_dotFunc)();
    _setPos(xc - radius, yc);
    (this->*m_dotFunc)();

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;
        _setPos(xc + x, yc + y);
        (this->*m_dotFunc)();
        _setPos(xc - x, yc + y);
        (this->*m_dotFunc)();
        _setPos(xc + x, yc - y);
        (this->*m_dotFunc)();
        _setPos(xc - x, yc - y);
        (this->*m_dotFunc)();
        _setPos(xc + y, yc + x);
        (this->*m_dotFunc)();
        _setPos(xc - y, yc + x);
        (this->*m_dotFunc)();
        _setPos(xc + y, yc - x);
        (this->*m_dotFunc)();
        _setPos(xc - y, yc - x);
        (this->*m_dotFunc)();
    }
}

// performed in less than 2.42ms (max)
void FrameBuffer::circle_filled(unsigned int xc, unsigned int yc, unsigned int radius, Color color)
{
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    // Check screen boundaries before drawing each line segment
    if (yc + radius < DISP_HEIGHT)
        line(xc, yc + radius, xc, yc - radius, color);

    // Check screen boundaries before drawing each line segment
    if (xc + radius < DISP_WIDTH)
        line(xc + radius, yc, xc - radius, yc, color);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;

        // Check screen boundaries before drawing each line segment
        if (((int) xc) + x >= 0 && xc + x < DISP_WIDTH && ((int) yc) + y >= 0 && yc + y < DISP_HEIGHT)
            line(xc + x, yc + y, xc + x, yc - y, color);
        if (((int) xc) - x >= 0 && xc - x < DISP_WIDTH && ((int) yc) + y >= 0 && yc + y < DISP_HEIGHT)
            line(xc - x, yc + y, xc - x, yc - y, color);
        if (((int) xc) + y >= 0 && xc + y < DISP_WIDTH && ((int) yc) + x >= 0 && yc + x < DISP_HEIGHT)
            line(xc + y, yc + x, xc + y, yc - x, color);
        if (((int) xc) - y >= 0 && xc - y < DISP_WIDTH && ((int) yc) + x >= 0 && yc + x < DISP_HEIGHT)
            line(xc - y, yc + x, xc - y, yc - x, color);
    }
}

void FrameBuffer::circle_filled2(int xc, int yc, unsigned int radius, Color color)
{
    int x, y;

    // Calculate the bounding box of the circle
    int xMin = std::max(0, static_cast<int>(xc) - static_cast<int>(radius));
    int xMax = std::min(static_cast<int>(DISP_WIDTH) - 1, static_cast<int>(xc) + static_cast<int>(radius));
    int yMin = std::max(0, static_cast<int>(yc) - static_cast<int>(radius));
    int yMax = std::min(static_cast<int>(DISP_HEIGHT) - 1, static_cast<int>(yc) + static_cast<int>(radius));

    // Iterate through each point within the bounding box
    for (x = xMin; x <= xMax; x++)
    {
        for (y = yMin; y <= yMax; y++)
        {
            // Check if the point is within the frame buffer bounds
            if (x >= 0 && x < DISP_WIDTH && y >= 0 && y < DISP_HEIGHT)
            {
                // Check if the point is within the circle's radius
                int dx = x - static_cast<int>(xc);
                int dy = y - static_cast<int>(yc);
                if ((dx * dx + dy * dy) <= (static_cast<int>(radius) * static_cast<int>(radius)))
                {
                    point(x, y, color);
                }
            }
        }
    }
}

void FrameBuffer::circle_filled3(int xc, int yc, unsigned int radius, Color color)
{
    int x, y;

    // Calculate the bounding box of the circle
    int xMin = std::max(0, static_cast<int>(xc) - static_cast<int>(radius));
    int xMax = std::min(static_cast<int>(DISP_WIDTH) - 1, static_cast<int>(xc) + static_cast<int>(radius));
    int yMin = std::max(0, static_cast<int>(yc) - static_cast<int>(radius));
    int yMax = std::min(static_cast<int>(DISP_HEIGHT) - 1, static_cast<int>(yc) + static_cast<int>(radius));

    // Iterate through each point within the bounding box
    for (y = yMin; y <= yMax; y++) {
        for (x = xMin; x <= xMax; x++) {
            int dx = x - static_cast<int>(xc);
            int dy = y - static_cast<int>(yc);
            float distance_squared = dx * dx + dy * dy;

            // Check if the pixel is within the circle
            if (distance_squared <= radius * radius) {
                if (distance_squared < (radius - 0.5f) * (radius - 0.5f)) {
                    // Pixel is fully inside the circle, no smoothing needed
                    if (x >= 0 && x < DISP_WIDTH && y >= 0 && y < DISP_HEIGHT) {
                        point(x, y, color);
                    }
                } else {
                    // Pixel is on the edge, apply smoothing based on distance from center
                    float distance = sqrt(distance_squared);
                    unsigned char alpha = (1.0f - (distance - (radius - 0.5f))) * 255;
                    if (x >= 0 && x < DISP_WIDTH && y >= 0 && y < DISP_HEIGHT) {
                        point(x, y, static_cast<Color>((color & 0xFFFFFF) | (alpha << 24)));
                    }
                }
            }
        }
    }
}

void FrameBuffer::circle_filled4(int xc, int yc, unsigned int radius, Color color)
{
    int x = radius;
    int y = 0;
    int radiusError = 1 - x;

    // Loop through the octants and draw pixels for all eight octants simultaneously
    while (x >= y) {
        int i;
        for (i = -4; i <= 4; i++) {
            // Calculate pixel positions for each octant with a 3-pixel wide edge
            int px = xc + x;
            int nx = xc - x;
            int py = yc + y + i;
            int ny = yc - y + i;

            // Calculate distances from the circle's boundary for smoothing
            float distance = sqrt(x * x + (y + i) * (y + i));
            unsigned char alpha = (1.0f - (distance - radius + 1)) * 255;

            // Draw pixels with smoothing
            point(px, py, static_cast<Color>((color & 0xFFFFFF) | (alpha << 24)));
            point(nx, py, static_cast<Color>((color & 0xFFFFFF) | (alpha << 24)));
            point(px, ny, static_cast<Color>((color & 0xFFFFFF) | (alpha << 24)));
            point(nx, ny, static_cast<Color>((color & 0xFFFFFF) | (alpha << 24)));
        }

        y++;
        if (radiusError < 0) {
            radiusError += 2 * y + 1;
        } else {
            x--;
            radiusError += 2 * (y - x + 1);
        }
    }
}


void FrameBuffer::circle_filled5(int xc, int yc, unsigned int radius, unsigned int thickness, Color color)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y) {
        int i;
        for (i = -thickness / 2; i <= ((int)(thickness) / 2); i++) {
            int px = xc + x;
            int nx = xc - x;
            int py = yc + y + i;
            int ny = yc - y + i;
            float distance = sqrt(x * x + (y + i) * (y + i));
            unsigned char alpha = (1.0f - fabs(distance - radius + thickness / 2)) * 255;
            alpha = 0xff;
            point(px, py, static_cast<Color>((color & 0xFFFFFF) | (alpha << 24)));
            point(nx, py, static_cast<Color>((color & 0xFFFFFF) | (alpha << 24)));
            point(px, ny, static_cast<Color>((color & 0xFFFFFF) | (alpha << 24)));
            point(nx, ny, static_cast<Color>((color & 0xFFFFFF) | (alpha << 24)));
        }

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void FrameBuffer::circle_filled6(int xc, int yc, unsigned int radius, Color color)
{
    int x, y;
    int outer_radius = radius + 1; // To consider the 3-pixel wide edge
    int inner_radius = radius - 2;

    for (y = yc - outer_radius; y <= yc + outer_radius; y++) {
        for (x = xc - outer_radius; x <= xc + outer_radius; x++) {
            int dx = x - xc;
            int dy = y - yc;
            int distance_squared = dx * dx + dy * dy;

            // Check if the pixel is within the outer and inner circles
            if (distance_squared <= outer_radius * outer_radius &&
                    distance_squared >= inner_radius * inner_radius) {
                float distance = sqrt((float)distance_squared);
                unsigned char alpha = 0;

                if (distance >= radius) {
                    // Pixel is on the outer boundary, apply smoothing based on distance from center
                    alpha = (1.0f - (distance - radius + 1.5f)) * 255;
                } else {
                    // Pixel is on the inner boundary, apply smoothing based on distance from center
                    alpha = (1.0f - (radius - distance + 0.5f)) * 255;
                }

                alpha = 0xff;
                point(x, y, static_cast<Color>((color & 0xFFFFFF) | (alpha << 24)));
            }
        }
    }
}

void FrameBuffer::blit(unsigned int x0, unsigned int y0, FrameBuffer *frame)
{
    if (NULL != frame)
        this->blit(x0, y0, 0, 0, frame->c_width, frame->c_height, frame);
}

void FrameBuffer::blit(unsigned int xc, unsigned int yc, FrameBuffer *frame, Effect &effect) // based on center of source image
{
    Pixel_t *tp;
    int xoff, yoff;

    xoff = frame->c_width >> 1;
    yoff = frame->c_height >> 1;
    for (int x = 0; x < (int) frame->c_width; x++)
    {
        for (int y = 0; y < (int) frame->c_height; y++)
        {
            frame->_setPos(x, y);
            tp = effect.transform((int)(x - xoff), (int)(y - yoff), *frame->m_position);
            this->point(tp->x + (int)xc, tp->y + (int)yc, tp->c);
        }
    }
}

void FrameBuffer::blit(int x0, int y0, unsigned int x1, unsigned int y1, unsigned int width, unsigned int height, FrameBuffer *frame)
{
    if (frame == NULL)
        return;

    uint32_t w;
    uint32_t h;

    if (x0 < 0)
    {
        w = MIN(width, this->c_width);
        x0 = 0;
    }
    else
        w = MIN(width, this->c_width - x0);

    if (y0 < 0)
    {
        h = MIN(height, this->c_height);
        y0 = 0;
    }
    else
        h = MIN(height, this->c_height - y0);

    //x1 and y1 theoretically will be always correct, because the caller should
    //have calculated the cutoff value.
    uint32_t xs = x1;

    while (w)
    {
        this->_setPos(x0, y0);
        frame->_setPos(xs, y1);
        for (uint32_t y = 0; y < h; y++)
        {
            this->_setColor((Color)*frame->m_position);
            (this->*m_dotFunc)();
            this->m_position++;
            frame->m_position++;
        }
        x0++;
        xs++;
        w--;
    }
}

void FrameBuffer::_char(unsigned int x0, unsigned int y0, const char c, const Font &font, Color foreG, Color backG)
{
    for (unsigned int line = 0; line < font.getHeight(); line++)
    {
        const uint8_t *pAlpha = font.getData(c, line);
        if (NULL == pAlpha)
        {
            if (Color::Opaque != backG)
                this->rectangle_filled(x0, y0, x0 + font.getWidth() - 1, y0 + font.getHeight() - 1, backG);
        }
        else
        {
            for (unsigned int col = 0; col < font.getWidth(); col++)
            {
                this->_setPos(x0 + col, y0 + line);
                if (Color::Opaque == backG)
                {
                    this->point(x0 + col, y0 + line, colorCombineAlphaI(foreG, *pAlpha));
                }
                else
                {
                    // merge with text background color
                    this->m_col = colorAlphaBlend(colorSetAlphaI(foreG, *pAlpha), backG);
                    this->_dot();
                }
                pAlpha++;
            }
        }
    }
}

unsigned int FrameBuffer::text(int x0, int y0, const char *string, const Font &font, Color foreG, Color backG)
{
    if (x0 + ((int) (font.getWidth() * strlen(string))) < 0)
        return 0;

    uint32_t oldx = x0;

    while (string && *string)
    {
        this->_char(x0, y0, *string++, font, foreG, backG);
        x0 += font.getWidth();
    }
    return x0 - oldx;
}

void FrameBuffer::line_thick(unsigned int x0, unsigned int y0,
                             unsigned int x1, unsigned int y1,
                             unsigned int thickness, Color color)
{
    if (x0 > DISP_WIDTH)  x0 = DISP_WIDTH;
    if (x1 > DISP_WIDTH)  x1 = DISP_WIDTH;
    if (y0 > DISP_HEIGHT) y0 = DISP_HEIGHT;
    if (y1 > DISP_HEIGHT) y0 = DISP_HEIGHT;

    int dx = std::abs(static_cast<int>(x1 - x0));
    int dy = std::abs(static_cast<int>(y1 - y0));
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    int x = x0;
    int y = y0;

    point(x, y, color);

    while ((x != (int) x1) || (y != (int) y1))
    {
        int e2 = err << 1;
        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
        for (int i = 0; i < (int) thickness; i++)
        {
            point(x, y + i, color);
            point(x, y - i, color);
            point(x + i, y, color);
            point(x - i, y, color);
        }
    }
}

