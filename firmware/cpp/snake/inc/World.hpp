/*******************************************************************************
 * @file World.hpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-28
 * @version v1.0
 * @brief Definitions of the World mechanics of the game
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

#include "DynamicImageLoader.hpp"
#include "FrameView.hpp"
#include "Physics.hpp"
#include "Snake.hpp"

extern FrameView *frameBuffer;  ///< Extern reference to the frameview

/**
 * @brief Indicates the possible actions/states of the game
 */
enum class Action : int {
  kKeepRunning,  ///< Action is to keep running the main screen
  /* below are the actions related to different screen behavior */
  kSensor,   ///< Action is to go to the Sensor screen
  kCredits,  ///< Action is to go to the Credits screen

  /* below are the actions related to full-screen images */
  kPickupFlash,         ///< Action is to go to the full-screen flash image
  kPickupManufacture1,  ///< Action is to go to the full-screen manufacture1
                        ///< image
  kPickupManufacture2,  ///< Action is to go to the full-screen manufacture2
                        ///< image
  kPickupSourceCode,  ///< Action is to go to the full-screen source-code image
  kPickupQrCodeCredits,  ///< Action is to go to the full-screen QR code credits
                         ///< image
};

/**
 * @brief Initializes the game's world
 */
void WorldInit();

/**
 * @brief Deinitialize the game's world
 */
void WorldDeinit();

/**
 * @brief Draws the world
 *
 * @param fb Framebuffer to be used to draw
 */
void WorldDraw(FrameView *fb);

/**
 * @brief Update the world
 *
 * @param fb Framebuffer to be used
 * @param particle Particle of the snake to be used to update the world
 *
 * @return Returns the next Action state
 */
Action WorldUpdate(FrameView *fb, Particle &particle);
