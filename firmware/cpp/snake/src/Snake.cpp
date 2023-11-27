/*******************************************************************************
 * @file Snake.cpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-19
 * @version v1.0
 * @brief Implementation of the Snake used by the game
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
#include "Snake.hpp"

#define kDeltaAngle ((4 * M_PI) / 180)  ///< The step-angle that snakes rotate
#define kTailPush \
  (5)  ///< How many time to push a new entry into the snake's tail
#define kTailAlphaDecay \
  (40)  ///< Alpha decay factor to draw the tail of the snake
#define kSnakeHeadRadius (3)       ///< Radius of the snake's head
#define kSnakeTailRadius (2)       ///< Radius of the snake's tail
#define kSnakeDefaultSpeed (1.0f)  ///< Speed of the snake

Snake::Snake(int x, int y, Color color) : particle_(x, y), color_(color) {
  for (int i = 0; i < kTailSize; i++) {
    tailTrace_[i].x = x;
    tailTrace_[i].y = y;
  }
  tail_ = head_ = 0;
  particle_.GetVelocity().SetLength(kSnakeDefaultSpeed);

  /* set the starting direction of the velocity (going up) */
  particle_.GetVelocity().SetAngle(-(90 * M_PI / 180));
}

void Snake::ClearTail() {
  for (int i = 0; i < kTailSize; i++) {
    tailTrace_[i].x = particle_.GetPosition().GetX();
    tailTrace_[i].y = particle_.GetPosition().GetY();
  }
}

void Snake::PushTailPosition() {
  tailTrace_[head_].x = particle_.GetPosition().GetX();
  tailTrace_[head_].y = particle_.GetPosition().GetY();

  head_ = ((head_ + 1) & (kTailSize - 1));

  if (head_ == tail_) tail_ = ((tail_ + 1) & (kTailSize - 1));
}

void Snake::Update() {
  static uint8_t count = 0;

  if (count++ >= kTailPush) {
    count = 0;
    PushTailPosition();
  }

  float velocity = particle_.GetVelocity().GetLength();

  if (velocity > kSnakeDefaultSpeed)
    particle_.GetVelocity().SetLength(velocity - 0.1);
  if (velocity < kSnakeDefaultSpeed)
    particle_.GetVelocity().SetLength(velocity + 0.1);

  particle_.Update();
}

void Snake::DrawTail(FrameView &fb) {
  uint8_t index = ((head_ - 1) & (kTailSize - 1));

  Viewport *vp = fb.get_viewport();

  int vpX = vp->GetX();
  int vpY = vp->GetY();
  int vpXMax = vpX + vp->GetWidth();
  int vpYMax = vpY + vp->GetHeight();

  if (vp == nullptr) return;

  for (uint8_t i = 0, j = 0xff; i < kTailSize - 1;
       i++, index = ((index - 1) & (kTailSize - 1)), j -= kTailAlphaDecay) {
    int x = tailTrace_[index].x;
    int y = tailTrace_[index].y;
    int bRight = x + kSnakeTailRadius;
    int bBottom = y + kSnakeTailRadius;

    /* we must make sure that the tail out-of-bounds should not de drawn */
    if (x >= vpXMax || bRight <= vpX) continue; /* Not visible, continue */

    if (y >= vpYMax || bBottom <= vpY) continue; /* Not visible, continue */

    fb.circle_filled2(
        vp->TranslateX(tailTrace_[index].x +
                       (index % 2 ? 1 : 0)), /* add some tail effect */
        vp->TranslateY(tailTrace_[index].y +
                       (index % 2 ? -1 : 0)), /* add some tail effect */
        kSnakeTailRadius,
        static_cast<Color>((Color::White & (0x00FFFFFF)) |
                           (j << 24))); /* we play with the alpha here */
  }
}

void Snake::Draw(FrameView &fb) {
  Viewport *vp = fb.get_viewport();

  if (vp == nullptr) return;

  fb.circle_filled2(vp->TranslateX(particle_.GetPosition().GetX()),
                    vp->TranslateY(particle_.GetPosition().GetY()),
                    kSnakeHeadRadius, kSnakeBaseColor);

  DrawTail(fb);
}

void Snake::Turn(SnakeTurn turn) {
  float angle = particle_.GetVelocity().GetAngle();

  angle += (SnakeTurn::Turn_RIGHT == turn) ? kDeltaAngle : -kDeltaAngle;
  particle_.GetVelocity().SetAngle(angle);
}

Particle &Snake::GetParticle() { return particle_; }

Vector &Snake::GetPosition() { return particle_.GetPosition(); }
