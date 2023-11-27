/*******************************************************************************
 * @file Object.cpp
 * @author Cassiano Silva de Campes
 * @date 2022-05-09
 * @version v1.0
 * @brief Implementation of the Object base class of the game
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
#include "Object.hpp"

Object::Object() {}

Object &Object::GetInstance() {
  static Object instance;
  return instance;
}

void Object::SetType(unsigned int w, unsigned int h, DILIndex sprite) {
  this->width_ = w;
  this->height_ = h;
  this->sprite_ = sprite;
}

void Object::Draw(int x, int y, FrameView *fb) {
  int cutoffX = (x < 0) ? std::abs(x) : 0;
  int cutoffY = (y < 0) ? std::abs(y) : 0;

  fb->blit(x, y, cutoffX, cutoffY, width_ - cutoffX, height_ - cutoffY,
           DIL::GetInstance().GetSprite(sprite_));
}

bool Object::IsVisible(int x, int y, FrameView *fb) {
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

  if (!visible) DIL::GetInstance().ReleaseSprite(sprite_);

  return visible;
}

bool Object::CheckCollision(int x, int y, Particle &particle) {
  Particle p(x, y);
  float distance = p.DistanceTo(particle);

  return (distance < static_cast<float>(width_ >> 1));
}
