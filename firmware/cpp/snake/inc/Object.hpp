/*******************************************************************************
 * @file Object.hpp
 * @author Cassiano Silva de Campes
 * @date 2022-05-09
 * @version v1.0
 * @brief Definitions of the Object base class of the game
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
#pragma once

#include "DynamicImageLoader.hpp"
#include "FrameView.hpp"
#include "Physics.hpp"

/**
 * @class Object
 * @brief This is the base Object class that describes a simple object.
 */
class Object {
 protected:
  int width_;        ///< Width of the object
  int height_;       ///< Height of the object
  DILIndex sprite_;  ///< Index of the sprite that represents this object

  /**
   * @brief Object constructor
   */
  Object();

  /**
   * @brief Constructor deleted
   */
  Object(const Object &) = delete;

  /**
   * @brief Operator deleted
   */
  Object &operator=(const Object &) = delete;

 public:
  /**
   * @brief Object is a static class that is mutable at runtime to represent
   * different objects that must be drawn and managed during the game
   */
  static Object &GetInstance();

  /**
   * @brief This is a generic object class, so the SetType is used
   *          to indicate what kind of object this represents in terms
   *          of width, height and sprite to be drawn
   *
   * @param w Width of the object
   * @param h Height of the object
   * @param sprite Sprite index that represents this object
   */
  void SetType(unsigned int w, unsigned int h, DILIndex sprite);

  /**
   * @brief Generic drawing method for objects
   *     Obs.: SetType must be called to set the width and height
   *
   * @param x x-position of the object
   * @param y y-position of the object
   * @param fb Framebuffer where the object will be drawn
   */
  virtual void Draw(int x, int y, FrameView *fb);

  /**
   * @brief Check if the object is visible or not with respect to the Viewport
   *     Obs.: SetType must be called to set the width and height
   *
   * @param x x-position of the object
   * @param y y-position of the object
   * @param fb Framebuffer representing the screen
   *
   * @return True, if the object is visible; False otherwise
   */
  bool IsVisible(int x, int y, FrameView *fb);

  /**
   * @brief Check if the object collides with the particle
   *     Obs.: SetType must be called to set the width and height
   *
   * @param x x-position of the object
   * @param y y-position of the object
   * @param particle Particle where we will check the collision against
   *
   * @return True, if they collide; False otherwise
   */
  virtual bool CheckCollision(int x, int y, Particle &particle);
};
