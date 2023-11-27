/*******************************************************************************
 * @file Viewport.hpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-28
 * @version v1.0
 * @brief Definitions of the Viewport mechanics of the game
 *
 * @copyright Copyright (c) 2022 nubix Software-Design GmbH, All rights
 *reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 *BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#pragma once

/**
 * @class Viewport
 * @brief Represents the viewport of the game
 */
class Viewport {
    public:
        /**
         * @brief Class constructor
         *
         * @param[in] width Width of the viewport
         * @param[in] height Height of the viewport
         */
        Viewport(int width, int height);

        /**
         * @brief Update the center of the viewport
         *
         * @param[in] x x-coordinate
         * @param[in] y y-coordinate
         */
        void UpdateCenter(int x, int y);

        /**
         * @brief Translates the given absolute x-coordinate into viewport's
         * coordinate
         *
         * @return Translated x-coordinate value
         */
        int TranslateX(int x) const;

        /**
         * @brief Translates the given absolute y-coordinate into viewport's
         * coordinate
         *
         * @return Translated y-coordinate value
         */
        int TranslateY(int y) const;

        /**
         * @brief Get the current x-coordinate of the viewport (top left corner)
         *
         * @return x-coordinate
         */
        int GetX(void) const;

        /**
         * @brief Get the current y-coordinate of the viewport (top left corner)
         *
         * @return Returns the y-coordinate
         */
        int GetY(void) const;

        /**
         * @brief Get the width of the viewport
         *
         * @return Width
         */
        int GetWidth(void) const;

        /**
         * @brief Get the height of the viewport
         *
         * @return Height
         */
        int GetHeight(void) const;

    protected:
        int centerX_;  ///< Indicates the current center-x coordinate of the screen
        int centerY_;  ///< Indicates the current center-y coordinate of the screen

        int width_;   ///< width of the viewport
        int height_;  ///< height of the viewport

        int XMin_;  ///< mininum-x coordinate (top left corner)
        int XMax_;  ///< maximum-x coordinate (top right corner)
        int YMin_;  ///< minimum-y coordinate (top left corner)
        int YMax_;  ///< maximum-y coordinate (top right corner)
};
