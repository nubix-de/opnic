/*******************************************************************************
 * @file BlackHole.hpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-19
 * @version v1.0
 * @brief Definitions of the BlackHole mechanics of the game
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

#include "FrameView.hpp"
#include "Object.hpp"
#include "Physics.hpp"

/**
 * @class BlackHole
 * @brief Class that represents the BlackHole object on the game
 */
class BlackHole : public Object {
 private:
  /**
   * @brief Class constructor
   */
  BlackHole();

 public:
  /**
   * @brief Gets the static instance of the class
   *
   * @return Returns a reference to the BlackHole object
   */
  static BlackHole &GetInstance();

  /**
   * @brief Do the gravity pull between the blackhole and the particle
   *
   * @param x x-position of the BlackHole
   * @param y y-position of the BlackHole
   * @param particle The particle that will suffer the gravity pull effect
   */
  void GravityPull(int x, int y, Particle &particle);

  /**
   * @brief Overide Draw method for drawing the BlackHole
   *
   * @param x x-position of the BlackHole
   * @param y y-position of the BlackHole
   * @param fb FrameView where the BlackHole will be drawn
   */
  void Draw(int x, int y, FrameView *fb) override;
};