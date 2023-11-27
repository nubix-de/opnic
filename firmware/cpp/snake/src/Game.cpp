/*******************************************************************************
 * @file Game.cpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-24
 * @version v1.0
 * @brief Implementation of the Game state machine
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
#include "Game.hpp"

#include <pico/multicore.h>
#include <pico/stdlib.h>

#include <cstdio>
#include <cstring>
#include <graphic/LT177ML35.hpp>
#include <sensor/Buttons.hpp>
#include <sensor/GyroAccel.hpp>

#include "FrameView.hpp"
#include "World.hpp"

static FrameView fb_(LT177ML35::getInstance());
static Viewport vp_(DISP_WIDTH, DISP_HEIGHT);
static State state_ = State::Init;
static Snake snake_(1264, 9018, Color::Green);  ///< Correct starting point
// static Snake snake_(4200, 8280, Color::Green);  ///<  Test starting point
static Action action_ = Action::kKeepRunning;  ///< Global action of the game

volatile bool pause_thread1_ =
    false;  ///< Variable used to block/unblock the thread1

static bool isAnyButtonPressed(void) {
  return (Buttons::isPressed(Button::Button_A) ||
          Buttons::isPressed(Button::Button_B) ||
          Buttons::isPressed(Button::Button_C) ||
          Buttons::isPressed(Button::Button_D));
}

static void core1_thread(void) {
  while (1) {
    while (pause_thread1_)
      ; /* busy-wait for the popup */

    fb_.clear(kSnakeBackgroundColor);

    WorldDraw(&fb_);
    snake_.Draw(fb_);

    fb_.show(false);
  }
}

static void Init(void) {
  LT177ML35::getInstance().setBrightness(100);
  fb_.set_viewport(&vp_);
  fb_.clear(kSnakeBackgroundColor);
  fb_.show(false);

  Buttons::init();
  WorldInit();

  state_ = State::Startup;
}

static void Startup(void) {
  vp_.UpdateCenter(snake_.GetPosition().GetX(), snake_.GetPosition().GetY());

  fb_.clear(kSnakeBackgroundColor);
  WorldDraw(&fb_);
  snake_.Draw(fb_);
  fb_.show(false);

  /* wait user press any button */
  while (!isAnyButtonPressed()) {
    // do nothing
  }

  fb_.clear(kSnakeBackgroundColor);

  state_ = State::Running;

  multicore_launch_core1(core1_thread);
}

static void Running(void) {
  if (Buttons::isPressed(Button::Button_C)) snake_.Turn(SnakeTurn::Turn_LEFT);

  if (Buttons::isPressed(Button::Button_D)) snake_.Turn(SnakeTurn::Turn_RIGHT);

  vp_.UpdateCenter(snake_.GetPosition().GetX(), snake_.GetPosition().GetY());

  action_ = WorldUpdate(&fb_, snake_.GetParticle());

  switch (action_) {
    case Action::kKeepRunning:
      state_ = State::Running;
      break;
    case Action::kSensor:
      state_ = State::Sensor;
      break;
    case Action::kPickupSourceCode:
      state_ = State::Source;
      break;
    case Action::kCredits:
      state_ = State::Credits;
      break;
    default:
      state_ = State::Popup;
      break;
  }

  snake_.Update();
}

static DILIndex TranslateActionToDILIndex(Action action) {
  switch (action) {
    case Action::kPickupFlash:
      return DILIndex::kFullscreenFlash;
    case Action::kPickupManufacture1:
      return DILIndex::kFullscreenManufacture1;
    case Action::kPickupManufacture2:
      return DILIndex::kFullscreenManufacture2;
    case Action::kPickupQrCodeCredits:
      return DILIndex::kFullscreenQrCodeCredits;

    case Action::kPickupSourceCode:
    default:
      return DILIndex::kFullscreenSourceCode;
  }
}

static void Popup(bool run_forever = false) {
  pause_thread1_ = true;

  DIL::GetInstance().ReleaseAll();

  /* wait user press any button */
  while (!isAnyButtonPressed() || run_forever) {
    fb_.clear(Color::Black);

    // TODO::check if we need to keep the TranslateActionToDILIndex() func
    DIL::GetInstance().GetSprite(TranslateActionToDILIndex(action_), fb_);

    fb_.show(true);
  }

  state_ = State::Running;

  pause_thread1_ = false;
}

static void Source(void) {
  pause_thread1_ = true;
  sleep_ms(100);

  Font font{Azaret_small_WIDTH, Azaret_small_HEIGHT, &Azaret_smallData[0]};
  fb_.clear(Color::Black);

  size_t y = (fb_.get_height() >> 1) - font.getHeight();
  {
    char text[] = "Mehr Infos";
    fb_.text((fb_.get_width() - (strlen(text) * font.getWidth())) >> 1, y, text,
             font, Color::White, kSnakeBackgroundColor);
  }
  {
    char text[] = "zu OPNIC";
    fb_.text((fb_.get_width() - (strlen(text) * font.getWidth())) >> 1,
             y + (font.getHeight()), text, font, Color::White,
             kSnakeBackgroundColor);
  }

  fb_.show(false);
  sleep_ms(1700);

  Popup(false);

  state_ = State::Running;
  pause_thread1_ = false;
}

static void Sensor(void) {
  pause_thread1_ = true;
  DIL::GetInstance().ReleaseAll();

#define TEXT_SIZE (20)
  GyroAccel gyro;
  char text[TEXT_SIZE] = {0};
  Font font{Azaret_small_WIDTH, Azaret_small_HEIGHT, &Azaret_smallData[0]};

  gyro.initialize();
  gyro.calibrate();

  /* wait user press any button */
  while (!isAnyButtonPressed()) {
    fb_.clear(Color::DarkBlue);
    gyro.readFIFO();

    IntVector_t& orientation = gyro.getOrientation();
    IntVector_t& acceleration = gyro.getAcceleration();

    fb_.text(3, 3, "MPU6050:", font, Color::White, Color::Opaque);
    fb_.line(3, 15, 160 - 3, 15, Color::White);
    fb_.line(3, 16, 160 - 3, 16, Color::White);

    snprintf(text, sizeof(text), "ok");
    fb_.text(75, 3, text, font, Color::Lime, Color::Opaque);

    snprintf(text, sizeof(text), "yaw  : %d", orientation.x);
    fb_.text(3, 25, text, font, Color::Yellow, Color::Opaque);
    snprintf(text, sizeof(text), "pitch: %d", orientation.y);
    fb_.text(3, 40, text, font, Color::Yellow, Color::Opaque);
    snprintf(text, sizeof(text), "roll : %d", orientation.z);
    fb_.text(3, 55, text, font, Color::Yellow, Color::Opaque);

    snprintf(text, sizeof(text), "x : %d", acceleration.x);
    fb_.text(3, 70, text, font, Color::Yellow, Color::Opaque);
    snprintf(text, sizeof(text), "y : %d", acceleration.y);
    fb_.text(3, 85, text, font, Color::Yellow, Color::Opaque);
    snprintf(text, sizeof(text), "z : %d", acceleration.z);
    fb_.text(3, 100, text, font, Color::Yellow, Color::Opaque);

    fb_.show(false);
  }

  state_ = State::Running;
  pause_thread1_ = false;
}

static void Credits(void) {
  pause_thread1_ = true;
  sleep_ms(100);

  Font font{Azaret_small_WIDTH, Azaret_small_HEIGHT, &Azaret_smallData[0]};
  /* end of game screen */
  {
    fb_.clear(Color::Black);
    const char text[] = "SPIELENDE!";
    fb_.text((fb_.get_width() - (strlen(text) * font.getWidth())) >> 1,
             fb_.get_height() >> 1, text, font, Color::Yellow,
             kSnakeBackgroundColor);
    fb_.show(false);
    sleep_ms(1500);
  }

#define BASE_COORDINATE (1000)  ///< Base x & y coordinate

  /* credits */
  {
    const char* authors[] = {">> CREDITS << ",
                             "",
                             "Cassiano Campes",
                             "Sebastian Kranz",
                             "Vinicius Linden",
                             "Andreas Petter",
                             "Danilo Zimmermann",
                             "Christine Rothmaler",
                             "Christopher Miersch",
                             "Katharina Frank",
                             NULL};
    const size_t elements = sizeof(authors) / sizeof(authors[0]) - 1;
    for (size_t acc = 0; acc < 2 * elements * font.getHeight(); ++acc) {
      vp_.UpdateCenter(BASE_COORDINATE,
                       BASE_COORDINATE - (vp_.GetHeight() >> 1) + acc);
      /* accumulates to twice the credits length */

      fb_.clear(Color::Black);
      for (int i = 0; authors[i]; i++) {
        const char* text = authors[i];
        fb_.text((vp_.GetWidth() - (strlen(text) * font.getWidth())) >> 1,
                 vp_.TranslateY(BASE_COORDINATE + i * font.getHeight()), text,
                 font, Color::White, kSnakeBackgroundColor);
      }
      fb_.show(true);
    }
  }

  /* thank you */
  {
    fb_.clear(Color::Black);
    size_t y = (fb_.get_height() >> 1) - (2 * font.getHeight());
    {
      const char text[] = "Wir freuen uns";
      fb_.text((fb_.get_width() - (strlen(text) * font.getWidth())) >> 1, y,
               text, font, Color::White, kSnakeBackgroundColor);
    }
    y += font.getHeight();
    {
      const char text[] = "ueber Ihr Feedback";
      fb_.text((fb_.get_width() - (strlen(text) * font.getWidth())) >> 1, y,
               text, font, Color::White, kSnakeBackgroundColor);
    }
    y += 2 * font.getHeight();
    {
      const char text[] = "Vielen Dank!";
      fb_.text((fb_.get_width() - (strlen(text) * font.getWidth())) >> 1, y,
               text, font, Color::White, kSnakeBackgroundColor);
    }
    fb_.show(false);
    sleep_ms(4000);
  }

  state_ = State::QrCodeCredits;

  pause_thread1_ = false;
}

void GameRun(void) {
  uint32_t start = time_us_32();
  uint32_t end;
  int32_t delay = INT32_MAX;

  switch (state_) {
    case State::Init:
      Init();
      break;

    case State::Startup:
      Startup();
      break;

    case State::Sensor:
      Sensor();
      break;

    case State::Running:
      Running();
      break;

    case State::Source:
      Source();
      break;

    case State::Popup:
      Popup(false);
      break;

    case State::Credits:
      Credits();
      break;

    case State::QrCodeCredits:
      action_ = Action::kPickupQrCodeCredits;
      Popup(true);
      break;
  }

  do {
    end = time_us_32();
    delay = GAME_FPS_PERIOD_US - (end - start);
  } while (delay > 0);
}
