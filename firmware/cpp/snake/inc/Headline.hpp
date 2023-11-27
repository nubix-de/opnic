/*******************************************************************************
 * @file Headline.hpp
 * @author Cassiano Silva de Campes
 * @date 2022-08-04
 * @version v1.0
 * @brief Definitions of the Headline of the game
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

#include "FrameView.hpp"
#include "Object.hpp"
#include "Physics.hpp"

#define TEXT_MAX_LENGTH (20)  ///< Maximum text length (with null terminator)
#define HEADLINE_LINES (2)    ///< Headline number of lines

/**
 * @class Headline
 * @brief Class that represents the Headline text objects on the game
 */
class Headline : public Object {
 private:
  /**
   * @brief Deleted constructor
   */
  Headline(const Headline &) = delete;

  /**
   * @brief Deleted constructor
   */
  Headline(Headline &&) = delete;

  /**
   * @brief Deleted operator
   */
  Headline &operator=(const Headline &) = delete;

  /**
   * @brief Deleted operator
   */
  Headline &operator=(Headline &&) = delete;

  /**
   * @brief Object constructor
   */
  Headline();

  /**
   * @brief Object destructor
   */
  ~Headline();

  char text_[HEADLINE_LINES]
            [TEXT_MAX_LENGTH];  ///< Stores the text that should be displayed
  Font font_;                   ///< Font to be used for drawing
  Color color_;                 ///< Color of the Headline
 public:
  /**
   * @brief Get the static instance of the class
   */
  static Headline &GetInstance();

  /**
   * @brief Set the headline text to be displayed
   *     This follows the same idea as the Unity game, which consists of
   *     the index text and the nromal text, example:
   *     1
   *     Headline bla bla bla
   *
   *     So, we split the index into one parameter, and the text in another.
   *
   * @param index Index value to be displayed
   * @param headline Text to be displayed
   */
  void SetHeadline(const char *index, const char *headline);

  /**
   * @brief Specialized drawing method for the headline
   */
  void Draw(int x, int y, FrameView *fb) override;

  /**
   * @brief Returns the font reference
   *
   * @return Returns the font reference
   */
  Font &GetFont(void);
};
