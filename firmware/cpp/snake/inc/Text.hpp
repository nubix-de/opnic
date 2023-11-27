/*******************************************************************************
 * @file Text.hpp
 * @author Cassiano Silva de Campes
 * @date 2022-07-19
 * @version v1.0
 * @brief Definitions of the Text of the game
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

#define TEXT_MAX_LENGTH (20)    ///< Maximum text length (with null terminator)
#define TEXT_MAX_NUM_LINES (3)  ///< Text maximum number of lines

enum class TextSize : uint8_t {
  Small,  ///< Small text
  Large,  ///< Large text
};

/**
 * @class Text
 * @brief Class that represents the Text on the screen
 */
class Text : public Object {
 private:
  Text(const Text &) = delete;
  Text(Text &&) = delete;
  Text &operator=(const Text &) = delete;
  Text &operator=(Text &&) = delete;

  Text();
  ~Text();

  uint8_t n_lines_;  ///< Number of lines that the text currently has
  TextSize size_;    ///< Size of the text font
  char text_[TEXT_MAX_NUM_LINES]
            [TEXT_MAX_LENGTH];  ///< Array to temporarilly store the texts

  /**
   * @brief Basic method to set the text on the buffer
   *
   * @param a String to set on the internal buffer
   * @param pos The position of the string
   */
  void _SetText(const char *a, int pos);

  /**
   * @brief Basic method to set the text on the buffer
   *
   * @param a String to set on the internal buffer
   * @param lena The length of the @p a string to be set
   * @param pos The position of the string
   */
  void _SetText(const char *a, int lena, int pos);

 public:
  /**
   * @brief Gets the static instance of the class
   *
   * @return Returns a reference to the Text object
   */
  static Text &GetInstance();

  /**
   * @brief Set the text font size
   */
  void SetSize(TextSize size);

  /**
   * @brief Set a single text string
   *
   * @param a The text string
   */
  void SetText(const char *a);

  /**
   * @brief Sets a two text strings
   *
   * @param a The first text string
   * @param b The second text string
   */
  void SetText(const char *a, const char *b);

  /**
   * @brief Sets a two text strings with lengths
   *
   * @param a The first text string
   * @param lena The length of the @p a string
   * @param b The second text string
   * @param lenb The length of the @p b string
   */
  void SetText(const char *a, int lena, const char *b, int lenb);

  /**
   * @brief Sets a three text strings
   *
   * @param a The first text string
   * @param b The second text string
   * @param c The third text string
   */
  void SetText(const char *a, const char *b, const char *c);

  /**
   * @brief Drawn method for the text
   *
   * @param fb Framebuffer to draw
   * @param color Text color (with the alpha channel)
   * @param x x-coordinate (top-left corner)
   * @param y y-coordinate (top-left corner)
   */
  void Draw(FrameView *fb, Color color, int x, int y);

  /**
   * @brief Returns if the text is visible on the screen
   *
   * @param x x-coordinate
   * @param y y-coordinate
   * @param fb Framebuffer to check
   *
   * @return Returns True if is visible; False otherwise
   */
  bool IsVisible(int x, int y, FrameView *fb);
};
