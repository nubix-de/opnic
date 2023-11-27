/*******************************************************************************
 * @file Bumper.cpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-19
 * @version v1.0
 * @brief Implements the Bumper mechanics of the game
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
#include "Bumper.hpp"

#include <cmath>

#include "DynamicImageLoader.hpp"

#define kBoosterWidth (18)  ///< Bumper width

Bumper::Bumper() { SetType(kBoosterWidth, kBoosterWidth, DILIndex::kBumper); }

Bumper &Bumper::GetInstance() {
  static Bumper instance;
  return instance;
}

#define THRESHOLD 14  ///< Threshold to detect if the collision happened

#define kBumperAcceleration (2.5)  ///< Acceleration when bumped
#define kBumperMaxAcceleration \
  (5)  ///< Define the maximum acceleration that the bumper can affect the snake

bool Bumper::CheckCollision(int x, int y, Particle &particle) {
  /* first check the bound-box of collision which is cheaper to process */
  bool collided = (particle.GetPosition().GetX() > x &&
                   particle.GetPosition().GetX() < x + kBoosterWidth &&
                   particle.GetPosition().GetY() > y &&
                   particle.GetPosition().GetY() < y + kBoosterWidth);

  if (!collided) return collided;

  float x1 = static_cast<float>(x) + static_cast<float>(kBoosterWidth >> 1);
  float x2 = static_cast<float>(x) + static_cast<float> kBoosterWidth;
  float x3 = static_cast<float>(x) + static_cast<float>(kBoosterWidth >> 1);
  float x4 = static_cast<float>(x);
  float y1 = static_cast<float>(y);
  float y2 = static_cast<float>(y) + static_cast<float>(kBoosterWidth >> 1);
  float y3 = static_cast<float>(y) + static_cast<float>(kBoosterWidth);
  float y4 = static_cast<float>(y) + static_cast<float>(kBoosterWidth >> 1);

  float y_expected_line1 = particle.GetPosition().GetX() + y1 - x1;
  float y_expected_line2 = -particle.GetPosition().GetX() + y2 + x2;
  float y_expected_line3 = particle.GetPosition().GetX() + y3 - x3;
  float y_expected_line4 = -particle.GetPosition().GetX() + y4 + x4;

  if (std::abs(y_expected_line1 - particle.GetPosition().GetY()) <= THRESHOLD ||
      std::abs(y_expected_line2 - particle.GetPosition().GetY()) <= THRESHOLD ||
      std::abs(y_expected_line3 - particle.GetPosition().GetY()) <= THRESHOLD ||
      std::abs(y_expected_line4 - particle.GetPosition().GetY()) <= THRESHOLD) {
    float velocity = particle.GetVelocity().GetLength();
    if (velocity < kBumperMaxAcceleration)
      particle.GetVelocity().SetLength(particle.GetVelocity().GetLength() +
                                       kBumperAcceleration);
    else
      particle.GetVelocity().SetLength(kBumperMaxAcceleration);
    particle.GetVelocity().ReverseX();
    particle.GetVelocity().ReverseY();
  } else
    collided = false;

  return collided;
}

#define BUMPER_COLOR \
  0xff70dbdb  // source: https://www.w3schools.com/colors/colors_picker.asp

void Bumper::Draw(int x, int y, FrameView *fb) {
  Color color = static_cast<Color>(BUMPER_COLOR);
  fb->line_soft2(x + (kBoosterWidth >> 1), y, x + kBoosterWidth,
                 y + (kBoosterWidth >> 1), 3, color);

  fb->line_soft2(x + (kBoosterWidth), y + (kBoosterWidth >> 1),
                 x + (kBoosterWidth >> 1), y + kBoosterWidth, 3, color);

  fb->line_soft2(x + (kBoosterWidth >> 1), y + kBoosterWidth, x,
                 y + (kBoosterWidth >> 1), 3, color);

  fb->line_soft2(x, y + (kBoosterWidth >> 1), x + (kBoosterWidth >> 1), y, 3,
                 color);

  fb->circle_filled2(x + (kBoosterWidth >> 1), y + (kBoosterWidth >> 1), 3,
                     color);
}
