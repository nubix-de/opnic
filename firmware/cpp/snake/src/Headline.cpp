/*******************************************************************************
 * @file Headline.cpp
 * @author Cassiano Silva de Campes
 * @date 2022-08-04
 * @version v1.0
 * @brief Implementation of the Headline of the game
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
#include "Headline.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <graphic/LT177ML35.hpp>

#define HEADLINE_COLOR (Color::White)  ///< Headline color

Headline::Headline()
    : font_{Azaret_large_WIDTH, Azaret_large_HEIGHT, &Azaret_largeData[0]},
      color_(HEADLINE_COLOR) {}

Headline::~Headline() {}

Headline &Headline::GetInstance() {
  static Headline instance;
  return instance;
}

void Headline::SetHeadline(const char *index, const char *headline) {
  uint8_t pos = 0;
  memset(text_[0], 0, TEXT_MAX_LENGTH);
  memset(text_[1], 0, TEXT_MAX_LENGTH);

  strncpy(text_[pos++], index, TEXT_MAX_LENGTH - 1);
  strncpy(text_[pos++], headline, TEXT_MAX_LENGTH - 1);
}

void Headline::Draw(int x, int y, FrameView *fb) {
  fb->text(x, y, text_[0], font_, color_, Color::Opaque);
  fb->text(x, y + 20, text_[1], font_, color_, Color::Opaque);
}

Font &Headline::GetFont(void) { return font_; }
