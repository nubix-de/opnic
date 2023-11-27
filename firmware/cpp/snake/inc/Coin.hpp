/*******************************************************************************
 * @file Coin.hpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-19
 * @version v1.0
 * @brief Definitions of the Coin mechanics of the game
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

#include <stdbool.h>

#include "FrameView.hpp"
#include "Object.hpp"
#include "Physics.hpp"

/**
 * @class Coin
 * @brief Class that represents the Coins/Collectables objects on the game
 */
class Coin : public Object {
 private:
  /**
   * @brief Class constructor
   */
  Coin();

  bool
      collected_;  ///< Stores the information if the coin was collected or not.
                   // This information is important for the Draw() method that
                   // uses this information to draw the coin at its normal color
                   // or at its collected color.

 public:
  /**
   * @brief Gets the static instance of the class
   *
   * @return Returns a reference to the Coin object
   */
  static Coin &GetInstance();

  /**
   * @brief Checks if the object and the particle collided or not
   *     Obs.: SetType must be called to set the width and height
   *
   * @param x x-position of the Coin
   * @param y y-position of the Coin
   * @param particle The particle that will be checked the collision against
   *
   * @return True, if they collided; False otherwise
   */
  bool CheckCollision(int x, int y, Particle &particle);

  /**
   * @brief Specialized drawing method for the coin
   *
   * @param x x-position of the Coin
   * @param y y-position of the Coin
   * @param fb Framebuffer where the Coin will be drawn
   */
  void Draw(int x, int y, FrameView *fb) override;

  /**
   * @brief Set if the coin was collected or not
   */
  void SetCollected(bool collected);
};
