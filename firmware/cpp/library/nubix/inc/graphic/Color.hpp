/*******************************************************************************
 * @file Color.hpp
 * @author Andreas Petter
 * @date 2022-11-26
 * @version v1.0
 * @brief Definition of Color class holding a ARGB value and a bunch of
 *        predefined values according to the color palette of modern web browsers.
 *
 * @copyright Copyright =c) 2022 nubix Software-Design GmbH, All rights reserved.
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
 * =INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT =INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#pragma once

#include <stdint.h>
#include <pico/float.h>

enum Color : uint32_t
{
    Opaque = 0x00000000,

    // pink colors
    Pink = 0xFFFFC0CB,
    LightPink = 0xFFFFB6C1,
    HotPink = 0xFFFF69B4,
    DeepPink = 0xFFFF1493,
    PaleVioletRed = 0xFFDB7093,
    MediumVioletRed = 0xFFC71585,

    // red colors
    LightSalmon = 0xFFFFA07A,
    Salmon = 0xFFFA8072,
    DarkSalmon = 0xFFE9967A,
    LightCoral = 0xFFF08080,
    IndianRed = 0xFFCD5C5C,
    Crimson = 0xFFDC143C,
    FireBrick = 0xFFB22222,
    DarkRed = 0xFF8B0000,
    Red = 0xFFFF0000,

    // orange colors
    OrangeRed = 0xFFFF4500,
    Tomato = 0xFFFF6347,
    Coral = 0xFFFF7F50,
    DarkOrange = 0xFFFF8C00,
    Orange = 0xFFFFA500,

    // yellow colors
    Yellow = 0xFFFFFF00,
    LightYellow = 0xFFFFFFE0,
    LemonChiffonv = 0xFFFFFACD,
    LightGoldenrodYellow = 0xFFFAFAD2,
    PapayaWhip = 0xFFFFEFD5,
    Moccasin = 0xFFFFE4B5,
    PeachPuff = 0xFFFFDAB9,
    PaleGoldenrod = 0xFFEEE8AA,
    Khaki = 0xFFF0E68C,
    DarkKhaki = 0xFFBDB76B,
    Gold = 0xFFFFD700,

    // brown colors
    Cornsilk = 0xFFFFF8DC,
    BlanchedAlmond = 0xFFFFEBCD,
    Bisque = 0xFFFFE4C4,
    NavajoWhite = 0xFFFFDEAD,
    Wheat = 0xFFF5DEB3,
    BurlyWood = 0xFFDEB887,
    Tan = 0xFFD2B48C,
    RosyBrown = 0xFFBC8F8F,
    SandyBrown = 0xFFF4A460,
    Goldenrod = 0xFFDAA520,
    DarkGoldenrod = 0xFFB8860B,
    Peru = 0xFFCD853F,
    Chocolate = 0xFFD2691E,
    SaddleBrown = 0xFF8B4513,
    Sienna = 0xFFA0522D,
    Brown = 0xFFA52A2A,
    Maroon = 0xFF800000,

    // green colors
    DarkOliveGreen = 0xFF556B2F,
    Olive = 0xFF808000,
    OliveDrab = 0xFF6B8E23,
    YellowGreen = 0xFF9ACD32,
    LimeGreen = 0xFF32CD32,
    Lime = 0xFF00FF00,
    LawnGreen = 0xFF7CFC00,
    Chartreuse = 0xFF7FFF00,
    GreenYellow = 0xFFADFF2F,
    SpringGreen = 0xFF00FF7F,
    MediumSpringGreen = 0xFF00FA9A,
    LightGreen = 0xFF90EE90,
    PaleGreen = 0xFF98FB98,
    DarkSeaGreen = 0xFF8FBC8F,
    MediumSeaGreen = 0xFF3CB371,
    SeaGreen = 0xFF2E8B57,
    ForestGreen = 0xFF228B22,
    Green = 0xFF008000,
    DarkGreen = 0xFF006400,

    // cyan colors
    MediumAquamarine = 0xFF66CDAA,
    Aqua = 0xFF00FFFF,
    Cyan = 0xFF00FFFF,
    LightCyan = 0xFFE0FFFF,
    PaleTurquoise = 0xFFAFEEEE,
    Aquamarine = 0xFF7FFFD4,
    Turquoise = 0xFF40E0D0,
    MediumTurquoise = 0xFF48D1CC,
    DarkTurquoise = 0xFF00CED1,
    LightSeaGreen = 0xFF20B2AA,
    CadetBlue = 0xFF5F9EA0,
    DarkCyan = 0xFF008B8B,
    Teal = 0xFF008080,

    // blue colors
    LightSteelBlue = 0xFFB0C4DE,
    PowderBlue = 0xFFB0E0E6,
    LightBlue = 0xFFADD8E6,
    SkyBlue = 0xFF87CEEB,
    LightSkyBlue = 0xFF87CEFA,
    DeepSkyBlue = 0xFF00BFFF,
    DodgerBlue = 0xFF1E90FF,
    CornflowerBlue = 0xFF6495ED,
    SteelBlue = 0xFF4682B4,
    RoyalBlue = 0xFF4169E1,
    Blue = 0xFF0000FF,
    MediumBlue = 0xFF0000CD,
    DarkBlue = 0xFF00008B,
    Navy = 0xFF000080,
    MidnightBlue = 0xFF191970,

    // violet colors
    Lavender = 0xFFE6E6FA,
    Thistle = 0xFFD8BFD8,
    Plum = 0xFFDDA0DD,
    Violet = 0xFFEE82EE,
    Orchid = 0xFFDA70D6,
    Fuchsia = 0xFFFF00FF,
    Magenta = 0xFFFF00FF,
    MediumOrchid = 0xFFBA55D3,
    MediumPurple = 0xFF9370DB,
    BlueViolet = 0xFF8A2BE2,
    DarkViolet = 0xFF9400D3,
    DarkOrchid = 0xFF9932CC,
    DarkMagenta = 0xFF8B008B,
    Purple = 0xFF800080,
    Indigo = 0xFF4B0082,
    DarkSlateBlue = 0xFF483D8B,
    SlateBlue = 0xFF6A5ACD,
    MediumSlateBlue = 0xFF7B68EE,
    RebeccaPurple = 0xFF663399,

    // white colors
    White = 0xFFFFFFFF,
    Snow = 0xFFFFFAFA,
    Honeydew = 0xFFF0FFF0,
    MintCream = 0xFFF5FFFA,
    Azure = 0xFFF0FFFF,
    AliceBlue = 0xFFF0F8FF,
    GhostWhite = 0xFFF8F8FF,
    WhiteSmoke = 0xFFF5F5F5,
    Seashell = 0xFFFFF5EE,
    Beige = 0xFFF5F5DC,
    OldLace = 0xFFFDF5E6,
    FloralWhite = 0xFFFFFAF0,
    Ivory = 0xFFFFFFF0,
    AntiqueWhite = 0xFFFAEBD7,
    Linen = 0xFFFAF0E6,
    LavenderBlush = 0xFFFFF0F5,
    MistyRose = 0xFFFFE4E1,

    // grey / black colors
    Gainsboro = 0xFFDCDCDC,
    LightGray = 0xFFD3D3D3,
    Silver = 0xFFC0C0C0,
    DarkGray = 0xFFA9A9A9,
    Gray = 0xFF808080,
    DimGray = 0xFF696969,
    LightSlateGray = 0xFF778899,
    SlateGray = 0xFF708090,
    DarkSlateGray = 0xFF2F4F4F,
    Black = 0xFF000000,
};

typedef union argb_t
{
    struct
    {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    };
    uint32_t raw;

    argb_t &operator=(const Color other)
    {
        raw = other;
        return *this;
    }
} argb_t;

static uint8_t clamp(float v) // define a function to bound and round the input float value to 0-255
{
    if (v < 0)
        return 0;
    if (v > 255)
        return 255;
    return (uint8_t)v;
}

static Color colorTransformHue(const Color color, const float fHue)
{
    static argb_t in;
    static argb_t out;
    static float old_hue;

    if (in.raw != color || old_hue != fHue) // use cached out, when in is same as before
    {
        in.raw = color;
        old_hue = fHue;
        if (in.alpha)
        {
            const float cosA = cosf(fHue * 3.14159265f / 180); // convert degrees to radians
            const float sinA = sinf(fHue * 3.14159265f / 180); // convert degrees to radians
            // calculate the rotation matrix, only depends on Hue
            float matrix[3][3] = {{cosA + (1.0f - cosA) / 3.0f, 1.0f / 3.0f * (1.0f - cosA) - sqrtf(1.0f / 3.0f) * sinA, 1.0f / 3.0f * (1.0f - cosA) + sqrtf(1.0f / 3.0f) * sinA},
                                  {1.0f / 3.0f * (1.0f - cosA) + sqrtf(1.0f / 3.0f) * sinA, cosA + 1.0f / 3.0f * (1.0f - cosA), 1.0f / 3.0f * (1.0f - cosA) - sqrtf(1.0f / 3.0f) * sinA},
                                  {1.0f / 3.0f * (1.0f - cosA) - sqrtf(1.0f / 3.0f) * sinA, 1.0f / 3.0f * (1.0f - cosA) + sqrtf(1.0f / 3.0f) * sinA, cosA + 1.0f / 3.0f * (1.0f - cosA)}};
            // Use the rotation matrix to convert the RGB directly
            out.red = clamp(in.red * matrix[0][0] + in.green * matrix[0][1] + in.blue * matrix[0][2]);
            out.green = clamp(in.red * matrix[1][0] + in.green * matrix[1][1] + in.blue * matrix[1][2]);
            out.blue = clamp(in.red * matrix[2][0] + in.green * matrix[2][1] + in.blue * matrix[2][2]);
            out.alpha = in.alpha;
        }
        out.alpha = in.alpha;
    }
    return (Color)out.raw;
}

static Color colorTransformBrightness(const Color color)
{
    argb_t out;
    argb_t in;
    uint32_t scratch;

    in.raw = color;

    scratch = in.red;
    scratch *= in.alpha;
    out.red = scratch >> 8;
    scratch = in.green;
    scratch *= in.alpha;
    out.green = scratch >> 8;
    scratch = in.blue;
    scratch *= in.alpha;
    out.blue = scratch >> 8;
    return (Color)out.raw;
}

static Color colorSetAlphaF(const Color color, float alpha)
{
    argb_t out;

    out.raw = color;
    out.alpha = 255 * alpha;
    return (Color)out.raw;
}

static inline Color colorSetAlphaI(const Color color, uint8_t alpha)
{
    argb_t out;

    out.raw = color;
    out.alpha = alpha;
    return (Color)out.raw;
}

/**
 * @brief Combine alpha channels having the @p color.alpha channel as the base alpha
 *        So, the @p color.alpha ranges from 0-255, which will scale the intensity
 *        at @p alpha.
 *
 * @param color The color which will be applied (includes the alpha)
 * @param alpha The alpha to be merged with the @p color.alpha
 */
static inline Color colorCombineAlphaI(const Color color, uint8_t alpha)
{
    argb_t out;

    uint8_t achannel = color >> 24;
    float alpha1 = achannel ? (static_cast<float>(achannel) / 255) : 0.f;
    float alpha2 = static_cast<float>(static_cast<float>(alpha) / 255);

    out.raw = color;

    float result = alpha1 * alpha2 * 255;
    out.alpha = static_cast<uint8_t>(result);
    return (Color)out.raw;
}

static Color colorAdd(const Color a, const Color b)
{
    uint32_t x = a;
    uint32_t y = b;
    uint32_t signmask = 0x80808080;
    uint32_t t0 = (y ^ x) & signmask;
    uint32_t t1 = (y & x) & signmask;
    x &= ~signmask;
    y &= ~signmask;
    x += y;
    t1 |= t0 & x;
    t1 = (t1 << 1) - (t1 >> 7);
    return (Color)((x ^ t0) | t1);
}

static Color colorAlphaBlend(const Color over, const Color back)
{
    static const int AMASK = 0xFF000000;
    static const int RBMASK = 0x00FF00FF;
    static const int GMASK = 0x0000FF00;
    static const int AGMASK = AMASK | GMASK;
    static const int ONEALPHA = 0x01000000;
    unsigned int a = (over & AMASK) >> 24;
    unsigned int na = 255 - a;
    unsigned int rb = ((na * (back & RBMASK)) + (a * (over & RBMASK))) >> 8;
    unsigned int ag = (na * ((back & AGMASK) >> 8)) + (a * (ONEALPHA | ((over & GMASK) >> 8)));
    return (Color)((rb & RBMASK) | (ag & AGMASK));
}
