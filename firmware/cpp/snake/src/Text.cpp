/*******************************************************************************
 * @file Text.cpp
 * @author Cassiano Silva de Campes
 * @date 2022-07-19
 * @version v1.0
 * @brief Implementation of the Text of the game
 *
 * @copyright Copyright (c) 2023 nubix Software-Design GmbH, All rights
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
#include "Text.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <graphic/LT177ML35.hpp>

Text::Text() : size_(TextSize::Small), n_lines_(1) {}

Text::~Text() {}

Text &Text::GetInstance() {
  static Text instance;
  return instance;
}

void Text::_SetText(const char *a, int pos) {
  if (pos >= TEXT_MAX_NUM_LINES) pos = 0;

  memset(text_[pos], 0, TEXT_MAX_LENGTH);
  strncpy(text_[pos], a, TEXT_MAX_LENGTH - 1);
}

void Text::_SetText(const char *a, int lena, int pos) {
  if (pos >= TEXT_MAX_NUM_LINES) pos = 0;

  if (lena >= TEXT_MAX_LENGTH - 1) lena = TEXT_MAX_LENGTH - 1;

  memset(text_[pos], 0, TEXT_MAX_LENGTH);
  strncpy(text_[pos], a, lena);
}

void Text::SetText(const char *a) {
  int pos = 0;

  this->_SetText(a, pos++);

  this->n_lines_ = pos;
}

void Text::SetText(const char *a, int lena, const char *b, int lenb) {
  int pos = 0;

  this->_SetText(a, lena, pos++);
  this->_SetText(b, lenb, pos++);

  this->n_lines_ = pos;
}

void Text::SetText(const char *a, const char *b) {
  int pos = 0;

  this->_SetText(a, pos++);
  this->_SetText(b, pos++);

  this->n_lines_ = pos;
}

void Text::SetText(const char *a, const char *b, const char *c) {
  int pos = 0;

  this->_SetText(a, pos++);
  this->_SetText(b, pos++);
  this->_SetText(c, pos++);

  this->n_lines_ = pos;
}

void Text::Draw(FrameView *fb, Color color, int x, int y) {
  (void)x;
  (void)y;
  for (int i = 0; i < this->n_lines_; i++) {
    if (size_ == TextSize::Small) {
      Font font{Azaret_small_WIDTH, Azaret_small_HEIGHT, &Azaret_smallData[0]};
      fb->text(5, 15 + i * 13, text_[i], font, color, Color::Opaque);
    } else {
      Font font{Azaret_large_WIDTH, Azaret_large_HEIGHT, &Azaret_largeData[0]};
      fb->text(5, 15 + i * 13, text_[i], font, color, Color::Opaque);
    }
  }
}

void Text::SetSize(TextSize size) { size_ = size; }

bool Text::IsVisible(int x, int y, FrameView *fb) {
  Viewport *vp = fb->get_viewport();
  bool visible = true;

  int bRight = x + width_;    ///< right-most pixel of the object
  int bBottom = y + height_;  ///< top-most pixel of the object

  int vpX = vp->GetX();                ///< viewport left-most X position
  int vpY = vp->GetY();                ///< viewport left-most Y position
  int vpXMax = vpX + vp->GetWidth();   ///< viewport bottom-most X position
  int vpYMax = vpY + vp->GetHeight();  ///< viewport bottom-most Y position

  if (x >= vpXMax || bRight <= vpX) visible = false;
  if (y >= vpYMax || bBottom <= vpY) visible = false;

  return visible;
}
