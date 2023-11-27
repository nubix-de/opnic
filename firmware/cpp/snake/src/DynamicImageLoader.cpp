/*******************************************************************************
 * @file DynamicImageLoader.cpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-19
 * @version v1.0
 * @brief Implementation of the Dynamic Image Loader mechanism of the game
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
#include "DynamicImageLoader.hpp"

#include <incbin.h>

#include <graphic/PngImage.hpp>

/**
 * @brief Include of the images needed by the snake game
 */
INCBIN(uint32_t, BackgroundNubixImg, "./background/Nubix.png");
INCBIN(uint32_t, ButtonCImg, "./button/C_s.png");
INCBIN(uint32_t, ButtonDImg, "./button/D_s.png");
INCBIN(uint32_t, FullscreenFlashImg, "./fullscreen/Flash_s.png");
INCBIN(uint32_t, FullscreenManufacture1Img, "./fullscreen/Manufacture1_s.png");
INCBIN(uint32_t, FullscreenManufacture2Img, "./fullscreen/Manufacture2_s.png");
INCBIN(uint32_t, FullscreenSourceCodeImg, "./fullscreen/SourceCode.png");
INCBIN(uint32_t, FullscreenQrCodeCreditsImg, "./fullscreen/QrCodeCredits.png");
INCBIN(uint32_t, HeadingTitleImg, "./background/Title_s.png");
INCBIN(uint32_t, PickupImg, "./pickup/About_s.png");
// INCBIN(uint32_t, NubixLogoImg, "./NubixLogo.png");
INCBIN(uint32_t, HexagonImg, "./background/Hexagon.png");

DIL::DIL()
    : sprites_{{.data = &gBackgroundNubixImgData[0],
                .size = gBackgroundNubixImgSize},
               {.data = &gButtonCImgData[0], .size = gButtonCImgSize},
               {.data = &gButtonDImgData[0], .size = gButtonDImgSize},
               {.data = &gFullscreenFlashImgData[0],
                .size = gFullscreenFlashImgSize},
               {.data = &gFullscreenManufacture1ImgData[0],
                .size = gFullscreenManufacture1ImgSize},
               {.data = &gFullscreenManufacture2ImgData[0],
                .size = gFullscreenManufacture2ImgSize},
               {.data = &gFullscreenSourceCodeImgData[0],
                .size = gFullscreenSourceCodeImgSize},
               {.data = &gFullscreenQrCodeCreditsImgData[0],
                .size = gFullscreenQrCodeCreditsImgSize},
               {.data = &gHeadingTitleImgData[0], .size = gHeadingTitleImgSize},
               {.data = &gPickupImgData[0], .size = gPickupImgSize},
               // {.data = &gNubixLogoImgData[0], .size = gNubixLogoImgSize},
               {.data = nullptr, .size = 0},
               {.data = &gHexagonImgData[0], .size = gHexagonImgSize}} {
  for (int i = static_cast<int>(DILIndex::kStartIndex);
       i < static_cast<int>(DILIndex::kNumDIL); i++) {
    frameBuffers_[i] = nullptr;
    current_[i] = 0;
    callers_[i] = 0;
  }
}

DIL& DIL::GetInstance() {
  static DIL dil;
  return dil;
}

/* so far, this method is used only by the full-screen image */
void DIL::GetSprite(DILIndex index, FrameView& fb) {
  fb.clear(Color::Opaque);

  const Sprites_t& sprite = sprites_[static_cast<int>(index)];
  PngImage img(sprite.data, sprite.size);
  // center image
  img.render(fb, (fb.get_width() - img.widthGet()) / 2,
             (fb.get_height() - img.heightGet()) / 2);
}

FrameView* DIL::GetSprite(DILIndex index) {
  current_[static_cast<int>(index)] = callers_[static_cast<int>(index)];

  if (frameBuffers_[static_cast<int>(index)] == nullptr) {
    /* here it is better to use stack rather than heap for allocatting,
     * otherwise we can increase the number of holes on the heap
     */
    PngImage img(sprites_[static_cast<int>(index)].data,
                 sprites_[static_cast<int>(index)].size);
    // dynamic_cast is disabled via -fno-rtti
    ///! @todo improve this cast
    frameBuffers_[static_cast<int>(index)] =
        static_cast<FrameView*>(img.render());
  }

  return frameBuffers_[static_cast<int>(index)];
}

void DIL::ReleaseSprite(DILIndex index) {
  if (frameBuffers_[static_cast<int>(index)] == nullptr) return;

  if (--current_[static_cast<int>(index)] <= 0) {
    current_[static_cast<int>(index)] = 0;
    delete frameBuffers_[static_cast<int>(index)];
    frameBuffers_[static_cast<int>(index)] = nullptr;
  }
}

void DIL::ReleaseSpriteForced(DILIndex index) {
  current_[static_cast<int>(index)] = 0;
  DIL::ReleaseSprite(index);
}

void DIL::ReleaseAll(void) {
  for (int i = static_cast<int>(DILIndex::kStartIndex);
       i < static_cast<int>(DILIndex::kNumDIL); i++) {
    ReleaseSpriteForced(static_cast<DILIndex>(i));
    current_[i] = 0;
  }
}

DILIndex DIL::GetFullScreenIndex(DILIndex index) {
  // bypassing the index
  return index;
}

void DIL::SetNumberOfCallers(DILIndex index, int callers) {
  if (index >= DILIndex::kNumDIL) return;

  callers_[static_cast<int>(index)] = callers;
}
