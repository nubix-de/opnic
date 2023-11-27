/*******************************************************************************
 * @file Snake.hpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-19
 * @version v1.0
 * @brief Definition of the Snake used by the game
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

#include <graphic/Color.hpp>

#include "FrameView.hpp"
#include "Physics.hpp"

#define kTailSize (8)                   ///< Must be 2^n
#define kSnakeBaseColor (Color::White)  ///< base color of the snake

/**
 * @brief Enumeration of the snake's motion
 */
enum SnakeTurn : uint8_t {
  Turn_LEFT = 0,  ///< Turn left
  Turn_RIGHT      ///< Turn right
};

/**
 * @class Snake
 * @brief Class that represents the Snake object
 */
class Snake {
 private:
  Particle particle_;  ///< Keeps the particle properties of the snake
  Color color_;        ///< Keeps the color of the snake

  Position_t
      tailTrace_[kTailSize];  ///< Stores the past tail positions of the snake
  void PushTailPosition();    ///< Push a new tail position on the vector
  void DrawTail(FrameView& fb);  ///< Draw the snake's tail
  uint8_t head_;                 ///< Stores the index of snake's head
  int8_t tail_;                  ///< Stores the index of snake's tail

 public:
  /**
   * @brief Snake constructor
   *
   * @param x x-coordinate
   * @param y y-coordinate
   * @param color Color of the snake
   */
  Snake(int x, int y, Color color);

  /**
   * @brief Run the update routine, that updates the snake position and related
   * physics
   */
  void Update();

  /**
   * @brief Draws the snake on the framebuffer
   *
   * @param fb Framebuffer to draw
   */
  void Draw(FrameView& fb);

  /**
   * @brief Execute the Turn action on the snake
   *
   * @param turn Turn left or right
   */
  void Turn(SnakeTurn turn);

  /**
   * @brief Get the snake's particle properties
   *
   * @return Returns the snake's particle reference
   */
  Particle& GetParticle();

  /**
   * @brief Get snake's vector position
   *
   * @return Returns the Vector position of the snake
   */
  Vector& GetPosition();

  /**
   * @brief Clears the tail buffer
   */
  void ClearTail();
};
