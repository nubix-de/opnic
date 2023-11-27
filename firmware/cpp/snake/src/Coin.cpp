/*******************************************************************************
 * @file Coin.cpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-19
 * @version v1.0
 * @brief Implementation of the Coin mechanics of the game
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
#include "Coin.hpp"

#include <cstdint>

#define COIN_BG_COLOR (0xFFCC7800)  ///< Background color of the coin
#define COIN_FG_COLOR (0xFFFFFF64)  ///< Foreground color of the coin

Coin::Coin() : collected_(false) {}

Coin &Coin::GetInstance() {
  static Coin instance;
  return instance;
}

bool Coin::CheckCollision(int x, int y, Particle &particle) {
  Particle p(x, y);
  float distance = p.DistanceTo(particle);

  /* For the coins, we increase the radius to check the collision,
   * otherwise, it would be very hard for the Snake to collect these small coins
   */
  bool collided = distance < static_cast<float>(2 * width_);

  return collided;
}

void Coin::Draw(int x, int y, FrameView *fb) {
  if (!collected_) {
    fb->circle_filled2(x, y, width_, static_cast<Color>(COIN_BG_COLOR));
    fb->circle_filled2(x, y, 2, static_cast<Color>(COIN_FG_COLOR));
  } else
    fb->circle_filled2(x, y, width_, Color::DarkGray);
}

void Coin::SetCollected(bool collected) { collected_ = collected; }
