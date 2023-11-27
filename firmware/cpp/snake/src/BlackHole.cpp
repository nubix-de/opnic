/*******************************************************************************
 * @file BlackHole.cpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-19
 * @version v1.0
 * @brief Implementation of the BlackHole mechanics of the game
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
#include "BlackHole.hpp"

#include "DynamicImageLoader.hpp"

#define kBlackHoleWidth (104)  ///< The outer-most width of the black hole
#define kGravityPull \
  (0.00005)  ///< Gravity force when reached inner side of black hole

BlackHole::BlackHole() {
  Object::SetType(kBlackHoleWidth, kBlackHoleWidth, DILIndex::kBlackHole);
}

BlackHole &BlackHole::GetInstance() {
  static BlackHole instance;
  return instance;
}

void BlackHole::GravityPull(int x, int y, Particle &particle) {
  Vector &vector = particle.GetPosition();
  int radius = (kBlackHoleWidth >> 1);

  Particle p(x, y);

  float distance = p.DistanceTo(particle);
  bool collided = (distance <= static_cast<float>(radius));

  if (!collided) return;

  if (distance < static_cast<float>(radius >> 1))
    p.SetMass(kGravityPull);
  else
    p.SetMass(distance * 2 + 50);  // TODO: empiric magic number for gravity :)

  particle.GravitateTo(p);
}

void BlackHole::Draw(int x, int y, FrameView *fb) {
  fb->circle_filled6(x, y, 38, Color::Gray);
  fb->circle_filled6(x, y, 35, kSnakeBackgroundColor);

  fb->circle_filled6(x, y, 28, Color::Gray);
  fb->circle_filled6(x, y, 25, kSnakeBackgroundColor);

  fb->circle_filled6(x, y, 18, Color::Gray);
  fb->circle_filled6(x, y, 15, kSnakeBackgroundColor);

  fb->circle_filled3(x, y, 8, Color::Cyan);
  fb->circle_filled3(x, y, 5, kSnakeBackgroundColor);
  fb->circle_filled3(x, y, 2, Color::Cyan);
}
