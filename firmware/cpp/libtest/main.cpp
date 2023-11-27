/*******************************************************************************
 * @file main.cpp
 * @author Andreas Petter
 * @author Vinícius Gabriel Linden
 * @date 2022-10-28
 * @version v1.0
 * @brief Library tester
 *
 * @copyright Copyright (c) 2022 nubix Software-Design GmbH, All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#include <cstdio>

#include <hardware/i2c.h>
#include <pico/stdlib.h>

#include <common/debug.hpp>
#include <graphic/FrameBuffer.hpp>
#include <graphic/LT177ML35.hpp>
#include <incbin.h>
#include <sensor/Buttons.hpp>

INCBIN(uint32_t, NbxLogo, "./nbx_logo_transparent.png");

float hue, alpha;
Color c;

int main() {
  stdio_init_all();
  debug_clear_screen();
  printf("%s\n", __TIME__);

  LT177ML35 &tft = LT177ML35::getInstance();
  FrameBuffer frame1(tft);
  Font font1 = {Azaret_small_WIDTH, Azaret_small_HEIGHT, &Azaret_smallData[0]};
  frame1.clear(Color::White);
  PngImage *img = new PngImage(&gNbxLogoData[0], gNbxLogoSize);
  FrameBuffer *logo = img->render();
  frame1.blit(0, 30, logo);
  delete img;
  delete logo;
  frame1.text(3, 3, "touch button demo...", font1, Color::Blue, Color::Opaque);
  frame1.show(false);
  Buttons::init();
  sleep_ms(3000);

  alpha = 0.5;

  while (true) {
    c = colorTransformHue(Color::Blue, hue);
    c = colorSetAlphaF(c, alpha);
    c = colorTransformBrightness(c);
    frame1.clear(c);
    tft.setStatusLED(c);

    if (Buttons::isPressed(Button::Button_A)) {
      DEBUG_INFO("Button A");
      frame1.rectangle_filled(0, 0, 20, 20, Color::White);
      hue += 1.0;
      if (hue > 360.) hue = 0.;
    }
    if (Buttons::isPressed(Button::Button_B)) {
      DEBUG_INFO("Button B");
      frame1.rectangle_filled(139, 0, 159, 20, Color::White);
      alpha += .001;
      if (alpha > 1.) alpha = 1.;
    }
    if (Buttons::isPressed(Button::Button_C)) {
      DEBUG_INFO("Button C");
      frame1.rectangle_filled(0, 107, 20, 127, Color::White);
      hue -= 1.0;
      if (hue < 0.) hue = 360.;
    }
    if (Buttons::isPressed(Button::Button_D)) {
      DEBUG_INFO("Button D");
      frame1.rectangle_filled(139, 107, 159, 127, Color::White);
      alpha -= .001;
      if (alpha < 0.) alpha = 0.;
    }
    frame1.text(3, 3, "H+", font1, Color::White, Color::Black);
    frame1.text(142, 3, "B+", font1, Color::White, Color::Black);
    frame1.text(3, 110, "H-", font1, Color::White, Color::Black);
    frame1.text(142, 110, "B-", font1, Color::White, Color::Black);
    frame1.show(true);
  }
}
