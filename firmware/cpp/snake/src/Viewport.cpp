/*******************************************************************************
 * @file Viewport.cpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-28
 * @version v1.0
 * @brief Implementation of the Viewport mechanism of the game
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
#include "Viewport.hpp"

Viewport::Viewport(int width, int height)
    : centerX_(0),
      centerY_(0),
      width_(width),
      height_(height),
      XMin_(0),
      XMax_(0),
      YMin_(0),
      YMax_(0) {}

void Viewport::UpdateCenter(int x, int y) {
  centerX_ = x;
  centerY_ = y;

  XMax_ = centerX_ + (width_ >> 1);
  YMax_ = centerY_ + (height_ >> 1);
  XMin_ = centerX_ - (width_ >> 1);
  YMin_ = centerY_ - (height_ >> 1);
}

int Viewport::TranslateX(int x) const { return x - XMin_; }

int Viewport::TranslateY(int y) const { return y - YMin_; }

int Viewport::GetX(void) const { return XMin_; }

int Viewport::GetY(void) const { return YMin_; }

int Viewport::GetWidth(void) const { return width_; }

int Viewport::GetHeight(void) const { return height_; }
