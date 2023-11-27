/*******************************************************************************
 * @file Booster.cpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-19
 * @version v1.0
 * @brief Implementation of the Booster mechanics of the game
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
#include "Booster.hpp"

#include "DynamicImageLoader.hpp"

#define kBoosterAcceleration (0.5)  ///< Acceleration caused by the booster
#define kBoosterRadius (9)          ///< Radius of the buster
#define kBoosterWidth (19)          ///< Width of the booster
#define kBoosterHeight (19)         ///< Height of the booster

Booster::Booster() {
  SetType(kBoosterWidth, kBoosterHeight, DILIndex::kSpeedup);
}

Booster &Booster::GetInstance() {
  static Booster instance;
  return instance;
}

bool Booster::CheckCollision(int x, int y, Particle &particle) {
  Particle p(x + kBoosterRadius, y + kBoosterRadius);

  float distance = p.DistanceTo(particle);
  bool collided = distance < static_cast<float>(kBoosterRadius << 1);

  if (collided)
    particle.GetVelocity().SetLength(particle.GetVelocity().GetLength() +
                                     kBoosterAcceleration);

  return collided;
}

void Booster::Draw(int x, int y, FrameView *fb) {
  fb->circle_filled2(x, y, kBoosterRadius, static_cast<Color>(0x5f00e699));
  fb->circle_filled2(x, y, kBoosterRadius >> 2, Color::Cyan);
}
