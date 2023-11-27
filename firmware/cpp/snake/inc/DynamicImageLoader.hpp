/*******************************************************************************
 * @file DynamicImageLoader.hpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-19
 * @version v1.0
 * @brief Definitions of the Dynamic Image Loader mechanism of the game
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

#include <cstdint>

#include "FrameView.hpp"

/**
 * @brief Enumeration of the sprites currently supported by the firmware
 */
enum class DILIndex : int {
  kStartIndex = 0,
  kBackgroundNubix = kStartIndex,  ///< Nubix background logo
  kButtonC,                        ///< C button
  kButtonD,                        ///< D button

  kFullscreenFlash,          ///< Fullscreen Flash image
  kFullscreenManufacture1,   ///< Fullscreen Manufacture1 image
  kFullscreenManufacture2,   ///< Fullscreen Manufacture2 image
  kFullscreenSourceCode,     ///< Fullscreen Source Code QR Code image
  kFullscreenQrCodeCredits,  ///< Fullscreen QR-Code Credits image

  kHeadingTitle,  ///< Heading title image

  kPickup,  ///< Pickup image

  kNubixLogo,   ///< Nubix logo sprite
  kHexagon,     ///< Hexagon background
  kBlackHole,   ///< Black-hole sprite
  kBumper,      ///< Bumper sprite
  kSpeedup,     ///< Speed-up sprite
  kCoin,        ///< Coin sprite
  kHeadline,    ///< Headline text
  kFadingText,  ///< Fading text
  kNumDIL,
};

/**
 * @brief Structure to store the Sprite information
 */
struct Sprites_t {
  const uint32_t* data;  ///< Data of the sprite used to draw
  uint32_t size;         ///< Size of the sprite's data
};

/**
 * @class DIL - Dynamic Image Loader
 */
class DIL {
 private:
  Sprites_t sprites_[static_cast<int>(
      DILIndex::kNumDIL)];  ///< Stores the sprites information
  FrameView* frameBuffers_[static_cast<int>(
      DILIndex::kNumDIL)];  ///< Temporary framebuffers used by loaded sprites
  int callers_[static_cast<int>(
      DILIndex::kNumDIL)];  ///< To store how many callers for a particular
                            ///< sprite will exist
  int current_[static_cast<int>(
      DILIndex::kNumDIL)];  ///< How many callers are currently using this
                            ///< sprite

  /**
   * @brief Class constructor
   */
  DIL();

 public:
  /**
   * @brief Gets the static instance of the class
   *
   * @return Returns a reference to the DIL object
   */
  static DIL& GetInstance();

  /**
   * @brief Get fullscreen sprite into the main framebuffer
   *     So, this method is used to draw fullscreen images on screen
   *
   * @param index Index of the sprite to be loaded
   * @param fb Reference to the main framebuffer
   */
  void GetSprite(DILIndex index, FrameView& fb);

  /**
   * @brief Loads the given sprite into the local framebuffer cache
   *     Obs.: After using it, must call the ReleaseSprite() method
   *     to release the memory
   *
   * @param index Index of the sprite to be loaded
   *
   * @return Returns a pointer to the loaded framebuffer
   */
  FrameView* GetSprite(DILIndex index);

  /**
   * @brief Release the given sprite framebuffer
   *
   * @param index Index of the sprite to be released
   */
  void ReleaseSprite(DILIndex index);

  /**
   * @brief Release the given sprite framebuffer forced
   *     That means, it will release the sprite irrespective of the
   *     number of callers set for this sprite
   *
   * @param index Index of the sprite to be released
   */
  void ReleaseSpriteForced(DILIndex index);

  /**
   * @brief Gets the full screen image index based on the provided @p index
   *     This is a mapping between DILIndex that can map to each other.
   *     This mapping is used to correlate pickup objects and its respective
   *     fullscreen image.
   *
   * @param index Index of the sprite to be mapped
   *
   * @return Returns the DILIndex of the fullscreen image
   */
  static DILIndex GetFullScreenIndex(DILIndex index);

  /**
   * @brief Set how many callers will be possibly using this sprite
   *        This information is neede to manage how we load the sprite
   *        and when we should release the sprite, to avoid keeping
   *        non-used sprites in SRAM memory.
   *        This function has relationship with the World.cpp class,
   *        which indicates how many objects of the same kind there will
   *        exist. So, based on that value the decision is taken.
   *
   * @param index Which sprite we are setting the number of callers
   * @param callers How many callers for this particular sprite
   */
  void SetNumberOfCallers(DILIndex index, int callers);

  /**
   * @brief Releases all the loaded sprites in the local framebuffer cache
   */
  void ReleaseAll(void);
};
