/*******************************************************************************
 * @file World.cpp
 * @author Cassiano Silva de Campes
 * @date 2022-04-28
 * @version v1.0
 * @brief Implements the World mechanics of the game
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
#include "World.hpp"

#include <pico/stdlib.h>

#include <cstdint>
#include <cstring>

#include "BlackHole.hpp"
#include "Booster.hpp"
#include "Bumper.hpp"
#include "Coin.hpp"
#include "Coordinates.hpp"
#include "Game.hpp"
#include "Headline.hpp"
#include "Snake.hpp"
#include "Text.hpp"

/**
 * @brief Vector to store which actions are for wich pickup object
 */
constexpr Action kActions[static_cast<int>(WorldObjects::kPickups)] = {
    Action::kPickupManufacture1, Action::kPickupManufacture2, Action::kSensor,
    Action::kPickupFlash,        Action::kPickupSourceCode,   Action::kCredits,
};

static void drawWorldLimits(FrameView *fb) {
  Viewport *vp = fb->get_viewport();
  int x;
  int y;

  Bumper &bumper = Bumper::GetInstance();

  x = static_cast<int>(std::floor(vp->GetX() / Bumpers[0].width));
  y = static_cast<int>(std::floor(vp->GetY() / Bumpers[0].height));
  y *= Bumpers[0].height;

  /***************************************************************************
   * Here is the drawing to depict what side of the limits the algorithm
   * is going to draw.
   *
   * --------------------------------------------------> (x)
   * |  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
   * |  │                               │
   * |  │                               │
   * |  │                               │
   * |  │                               │
   * |  │                               │
   * |  │           ┌───────────────────┘
   * |  │           │
   * |  │           │
   * |  │           │
   * |  │           │
   * |  │           │
   * |  └───────────┘
   * |
   * \/
   * (y)
   **************************************************************************/
  bumper.SetType(MAX_X - MIN_X, Bumpers[0].height, DILIndex::kBumper);
  if (bumper.IsVisible(MIN_X, MIN_Y, fb)) {
    x *= Bumpers[0].width;
    y = MIN_Y;

    for (int i = 0; x < (int)(vp->GetX() + fb->get_width()) && x < MAX_X; i++) {
      if (x > MIN_X) bumper.Draw(vp->TranslateX(x), vp->TranslateY(y), fb);
      x += Bumpers[0].width;
    }
  }

  x = std::floor(vp->GetX() / Bumpers[0].width);
  y = std::floor(vp->GetY() / Bumpers[0].height);
  y *= Bumpers[0].height;

  /***************************************************************************
   * --------------------------------------------------> (x)
   * |  ┌───────────────────────────────┐
   * |  │                               │
   * |  │                               │
   * |  │                               │
   * |  │                               │
   * |  │                               │
   * |  │           ┌───────────────────┘
   * |  │           │
   * |  │           │
   * |  │           │
   * |  │           │
   * |  │           │
   * |  xxxxxxxxxxxxx
   * |
   * \/
   * (y)
   **************************************************************************/
  bumper.SetType(MAX_X - 1393, Bumpers[0].height, DILIndex::kBumper);
  if (bumper.IsVisible(MIN_X, MAX_Y, fb)) {
    x *= Bumpers[0].width;
    y = MAX_Y;

    for (int i = 0; x < (int)(vp->GetX() + fb->get_width()) && x < 1393; i++) {
      if (x > MIN_X) bumper.Draw(vp->TranslateX(x), vp->TranslateY(y), fb);
      x += Bumpers[0].width;
    }
  }

  x = std::floor(vp->GetX() / Bumpers[0].width);
  y = std::floor(vp->GetY() / Bumpers[0].height);
  y *= Bumpers[0].height;

  /***************************************************************************
   * --------------------------------------------------> (x)
   * |  ┌───────────────────────────────┐
   * |  │                               │
   * |  │                               │
   * |  │                               │
   * |  │                               │
   * |  │                               │
   * |  │           xxxxxxxxxxxxxxxxxxxxx
   * |  │           │
   * |  │           │
   * |  │           │
   * |  │           │
   * |  │           │
   * |  └───────────┘
   * |
   * \/
   * (y)
   **************************************************************************/
  bumper.SetType(MAX_X - 1382, Bumpers[0].height, DILIndex::kBumper);
  if (bumper.IsVisible(1382, 8663, fb)) {
    x *= Bumpers[0].width;
    y = 8663;

    for (int i = 0; x < (int)(vp->GetX() + fb->get_width()) && x < MAX_X; i++) {
      if (x > 1382) bumper.Draw(vp->TranslateX(x), vp->TranslateY(y), fb);
      x += Bumpers[0].width;
    }
  }

  x = std::floor(vp->GetX() / Bumpers[0].width);
  y = std::floor(vp->GetY() / Bumpers[0].height);
  x *= Bumpers[0].width;

  /***************************************************************************
   * --------------------------------------------------> (x)
   * |  x───────────────────────────────┐
   * |  x                               │
   * |  x                               │
   * |  x                               │
   * |  x                               │
   * |  x                               │
   * |  x           ┌───────────────────┘
   * |  x           │
   * |  x           │
   * |  x           │
   * |  x           │
   * |  x           │
   * |  x───────────┘
   * |
   * \/
   * (y)
   **************************************************************************/
  bumper.SetType(Bumpers[0].width, MAX_Y - MIN_Y, DILIndex::kBumper);
  if (bumper.IsVisible(MIN_X, MIN_Y, fb)) {
    x = MIN_X;
    y *= Bumpers[0].height;

    for (int i = 0; y < (int)(vp->GetY() + fb->get_height()) && y < MAX_Y;
         i++) {
      if (y > MIN_Y) bumper.Draw(vp->TranslateX(x), vp->TranslateY(y), fb);
      y += Bumpers[0].width;
    }
  }

  x = std::floor(vp->GetX() / Bumpers[0].width);
  y = std::floor(vp->GetY() / Bumpers[0].height);
  x *= Bumpers[0].width;

  /***************************************************************************
   * --------------------------------------------------> (x)
   * |  ┌───────────────────────────────┐
   * |  │                               │
   * |  │                               │
   * |  │                               │
   * |  │                               │
   * |  │                               │
   * |  │          x────────────────────┘
   * |  │          x
   * |  │          x
   * |  │          x
   * |  │          x
   * |  │          x
   * |  └──────────x
   * |
   * \/
   * (y)
   **************************************************************************/
  if (bumper.IsVisible(1382, 8680, fb)) {
    x = 1382;
    y *= Bumpers[0].height;

    for (int i = 0; y < (int)(vp->GetY() + fb->get_height()) && y < MAX_Y;
         i++) {
      if (y > 8663) bumper.Draw(vp->TranslateX(x), vp->TranslateY(y), fb);
      y += Bumpers[0].width;
    }
  }

  /***************************************************************************
   * --------------------------------------------------> (x)
   * |  ┌───────────────────────────────x
   * |  │                               x
   * |  │                               x
   * |  │                               x
   * |  │                               x
   * |  │                               x
   * |  │           ┌───────────────────x
   * |  │           │
   * |  │           │
   * |  │           │
   * |  │           │
   * |  │           │
   * |  └───────────┘
   * |
   * \/
   * (y)
   **************************************************************************/
  if (bumper.IsVisible(MAX_X, MIN_Y, fb)) {
    x = MAX_X;
    y *= Bumpers[0].height;

    for (int i = 0; y < (int)(vp->GetY() + fb->get_height()) && y < 8663; i++) {
      if (y > MIN_Y) bumper.Draw(vp->TranslateX(x), vp->TranslateY(y), fb);
      y += Bumpers[0].width;
    }
  }
}

static void drawNubixLogo(FrameView *fb) {
  Viewport *vp = fb->get_viewport();
  Object &object = Object::GetInstance();

  object.SetType(NubixLogos[0].width, NubixLogos[0].height,
                 DILIndex::kBackgroundNubix);
  if (object.IsVisible(NubixLogos[0].x, NubixLogos[0].y, fb))
    object.Draw(vp->TranslateX(NubixLogos[0].x),
                vp->TranslateY(NubixLogos[0].y), fb);
}

static void drawSixEuroLogo(FrameView *fb) {
  Viewport *vp = fb->get_viewport();
  Object &object = Object::GetInstance();

  object.SetType(_6EArcades[0].width, _6EArcades[0].height,
                 DILIndex::kHeadingTitle);
  if (object.IsVisible(_6EArcades[0].x, _6EArcades[0].y, fb))
    object.Draw(vp->TranslateX(_6EArcades[0].x),
                vp->TranslateY(_6EArcades[0].y), fb);
}

static void drawDecorativeHexagons(FrameView *fb) {
  Viewport *vp = fb->get_viewport();
  Object &object = Object::GetInstance();

  object.SetType(Decorative_Hexagons[0].width, Decorative_Hexagons[0].height,
                 DILIndex::kHexagon);

  for (int i = 0; i < static_cast<int>(WorldObjects::kDecorative_Hexagons);
       i++) {
    if (object.IsVisible(Decorative_Hexagons[i].x, Decorative_Hexagons[i].y,
                         fb))
      object.Draw(vp->TranslateX(Decorative_Hexagons[i].x),
                  vp->TranslateY(Decorative_Hexagons[i].y), fb);
  }
}

static void drawBlackHole(FrameView *fb) {
  Viewport *vp = fb->get_viewport();
  BlackHole &blackhole = BlackHole::GetInstance();
  // blackhole.SetType(BlackHoles[0].width, BlackHoles[0].height,
  // DILIndex::kBlackHole);
  blackhole.SetType(200, 200, DILIndex::kBlackHole);  // TODO::FIX THIS

  for (int i = 0; i < static_cast<int>(WorldObjects::kBlackHoles); i++) {
    /* x and y refers to the center of the object */
    if (blackhole.IsVisible(BlackHoles[i].x - BlackHoles[i].width,
                            BlackHoles[i].y - BlackHoles[i].height, fb)) {
      blackhole.Draw(vp->TranslateX(BlackHoles[i].x),
                     vp->TranslateY(BlackHoles[i].y), fb);
      BlackHoles[i].visible = true;
    } else
      BlackHoles[i].visible = false;
  }
}

static void drawHeadline(FrameView *fb) {
  char index[2] = "0";
  Viewport *vp = fb->get_viewport();
  Headline &headline = Headline::GetInstance();

  for (int i = 0; i < static_cast<int>(WorldObjects::kHeadlines); i++) {
    unsigned int width =
        strlen(Headlines[i].text) * headline.GetFont().getWidth();
    unsigned int height = HEADLINE_LINES * headline.GetFont().getHeight();
    headline.SetType(width, height, DILIndex::kHeadline);
    if (headline.IsVisible(Headlines[i].x, Headlines[i].y, fb)) {
      index[0] = '1' + i;
      headline.SetHeadline(index, Headlines[i].text);
      headline.Draw(vp->TranslateX(Headlines[i].x),
                    vp->TranslateY(Headlines[i].y), fb);
    }
  }
}

static void drawFadingText(FrameView *fb) {
  Viewport *vp = fb->get_viewport();
  Text &text = Text::GetInstance();
  char *ptr = NULL;

  text.SetType(10, 10, DILIndex::kFadingText);  // TODO change sizes here
  text.SetSize(TextSize::Small);

  for (int i = 0; i < static_cast<int>(WorldObjects::kFadingTextAreas); i++) {
    static Coordinate_t coordinate = {.x = 0,
                                      .y = 0,
                                      .visible = 0,
                                      .picked = 0,
                                      .width = 0,
                                      .height = 0,
                                      .decay = 0x00};
    static uint32_t decay = 0;

    if (FadingTextAreas[i].picked == true || FadingTextAreas[i].visible) {
      decay++;

#define FADING_TOGGLE UINT32_C(15)
#define FADING_IN_OUT UINT32_C(20)
#define FADING_STABLE UINT32_C(10)

      if (decay < FADING_TOGGLE) {
        FadingTextAreas[i].visible = true;
        coordinate.decay += 8;
      } else if (decay < (FADING_TOGGLE + FADING_IN_OUT))
        coordinate.decay = 0xff;
      else if (decay < (FADING_TOGGLE + FADING_IN_OUT + FADING_STABLE))
        coordinate.decay -= 8;
      else if (decay < (FADING_TOGGLE + 2 * FADING_IN_OUT + FADING_STABLE)) {
        decay = 0;
        coordinate.decay = 0;
        FadingTextAreas[i].visible = false;
      }
      Color color = static_cast<Color>((coordinate.decay << 24) | 0x00FFDEAD);
      ptr = strstr(FadingTextAreas[i].text, "\n");
      if (ptr) {
        int len = 0;
        const char *begin = FadingTextAreas[i].text;
        while (begin++ != ptr) len++;
        text.SetText(FadingTextAreas[i].text, len, ++ptr, strlen(ptr));
        text.Draw(fb, color, vp->TranslateX(FadingTextAreas[i].x),
                  vp->TranslateY(FadingTextAreas[i].y));
      } else {
        text.SetText(FadingTextAreas[i].text);
        text.Draw(fb, color, vp->TranslateX(FadingTextAreas[i].x),
                  vp->TranslateY(FadingTextAreas[i].y));
      }
    }
  }
}

static void drawPickups(FrameView *fb) {
  Viewport *vp = fb->get_viewport();
  Object &pickup = Object::GetInstance();

  for (int i = 0; i < static_cast<int>(WorldObjects::kPickups); i++) {
    pickup.SetType(64, 64, DILIndex::kPickup);
    if (pickup.IsVisible(Pickups[i].x, Pickups[i].y, fb)) {
      pickup.Draw(vp->TranslateX(Pickups[i].x), vp->TranslateY(Pickups[i].y),
                  fb);
      Pickups[i].visible = true;
    } else
      Pickups[i].visible = false;
  }
}

static void drawBumper(FrameView *fb) {
  Viewport *vp = fb->get_viewport();
  Bumper &bumper = Bumper::GetInstance();

  for (int i = 0; i < static_cast<int>(WorldObjects::kBumpers); i++) {
    if (bumper.IsVisible(Bumpers[i].x, Bumpers[i].y, fb)) {
      bumper.Draw(vp->TranslateX(Bumpers[i].x), vp->TranslateY(Bumpers[i].y),
                  fb);
      Bumpers[i].visible = true;
    } else
      Bumpers[i].visible = false;
  }
}

static void drawButtons(FrameView *fb) {
  Viewport *vp = fb->get_viewport();
  Object &buttons = Object::GetInstance();

  buttons.SetType(CButtons[0].width, CButtons[0].height, DILIndex::kButtonC);

  if (buttons.IsVisible(CButtons[0].x, CButtons[0].y, fb))
    buttons.Draw(vp->TranslateX(CButtons[0].x), vp->TranslateY(CButtons[0].y),
                 fb);

  buttons.SetType(DButtons[0].width, DButtons[0].height, DILIndex::kButtonD);
  if (buttons.IsVisible(DButtons[0].x, DButtons[0].y, fb))
    buttons.Draw(vp->TranslateX(DButtons[0].x), vp->TranslateY(DButtons[0].y),
                 fb);
}

static void drawBooster(FrameView *fb) {
  Viewport *vp = fb->get_viewport();
  Booster &booster = Booster::GetInstance();

  for (int i = 0; i < static_cast<int>(WorldObjects::kBoosters); i++) {
    if (booster.IsVisible(Boosters[i].x, Boosters[i].y, fb)) {
      booster.Draw(vp->TranslateX(Boosters[i].x), vp->TranslateY(Boosters[i].y),
                   fb);
      Boosters[i].visible = true;
    } else
      Boosters[i].visible = false;
  }
}

static void drawCoins(FrameView *fb) {
  Viewport *vp = fb->get_viewport();
  Coin &coin = Coin::GetInstance();

  coin.SetType(Collectables[0].width, Collectables[0].height, DILIndex::kCoin);

  for (int i = 0; i < static_cast<int>(WorldObjects::kCollectables); i++) {
    coin.SetCollected(Collectables[i].picked);

    if (coin.IsVisible(Collectables[i].x, Collectables[i].y, fb)) {
      coin.Draw(vp->TranslateX(Collectables[i].x),
                vp->TranslateY(Collectables[i].y), fb);
      Collectables[i].visible = true;
    } else
      Collectables[i].visible = true;
  }
}

static void updateBlackHole(FrameView *fb, Particle &particle) {
  (void)fb;

  BlackHole &blackhole = BlackHole::GetInstance();

  for (int i = 0; i < static_cast<int>(WorldObjects::kBlackHoles); i++) {
    if (BlackHoles[i].visible)
      blackhole.GravityPull(BlackHoles[i].x, BlackHoles[i].y, particle);
  }
}

static void updateFadingText(FrameView *fb, Particle &particle) {
  (void)fb;
  Text &text = Text::GetInstance();
  text.SetType(10, 10, DILIndex::kButtonC);  // TODO change sizes here
  text.SetSize(TextSize::Small);

  for (int i = 0; i < static_cast<int>(WorldObjects::kFadingTextAreas); i++) {
    if (particle.GetPosition().GetX() >= FadingTextAreas[i].x &&
        particle.GetPosition().GetX() < FadingTextAreas[i].x + 10)
      FadingTextAreas[i].picked = true;
    else
      FadingTextAreas[i].picked = false;
  }
}

static Action updatePickup(FrameView *fb, Particle &particle) {
  (void)fb;
  Action index = Action::kKeepRunning;
  Object &pickup = Object::GetInstance();

  for (int i = 0; i < static_cast<int>(WorldObjects::kPickups); i++) {
    pickup.SetType(Pickups[i].width / 2, Pickups[i].height / 2,
                   DILIndex::kPickup);

    /* if we collide with a pickup object, that will cause a state change
     * to show the fullscreen image. So, it is safe here to store the
     * Action for a single collision, because we will never collide with
     * more than one pickup object at the time.
     */
    if (!Pickups[i].visible) continue;

    if (Pickups[i].picked) {
      if (--Pickups[i].decay <= 0)
        Pickups[i].picked = false;
      else
        continue;
    }

    if (pickup.CheckCollision(Pickups[i].x + Pickups[i].width / 2,
                              Pickups[i].y + Pickups[i].height / 2, particle)) {
      if (!Pickups[i].picked) {
        Pickups[i].picked = true;
        Pickups[i].decay = INT32_C(2) * GAME_FPS;
      }
      index = kActions[i];
    }
  }
  return index;
}

static void updateWorldLimitsBumper(FrameView *fb, Particle &particle) {
  Viewport *vp = fb->get_viewport();
  int x;
  int y;

  Bumper &bumper = Bumper::GetInstance();

  x = static_cast<int>(std::floor(vp->GetX() / Bumpers[0].width));
  y = static_cast<int>(std::floor(vp->GetY() / Bumpers[0].height));
  y *= Bumpers[0].height;

  /***************************************************************************
   * Here is the drawing to depict what side of the limits the algorithm
   * is going to draw.
   *
   * --------------------------------------------------> (x)
     |  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
     |  │                               │
     |  │                               │
     |  │                               │
     |  │                               │
     |  │                               │
     |  │           ┌───────────────────┘
     |  │           │
     |  │           │
     |  │           │
     |  │           │
     |  │           │
     |  └───────────┘
     |
     \/
     (y)
  ***************************************************************************/
  bumper.SetType(MAX_X - MIN_X, Bumpers[0].height, DILIndex::kBumper);
  if (bumper.IsVisible(MIN_X, MIN_Y, fb)) {
    x *= Bumpers[0].width;
    y = MIN_Y;

    bumper.SetType(Bumpers[0].width, Bumpers[0].height, DILIndex::kBumper);

    for (int i = 0; x < (int)(vp->GetX() + fb->get_width()) && x < MAX_X; i++) {
      bumper.CheckCollision(x, y, particle);
      x += Bumpers[0].width;
    }
  }

  x = std::floor(vp->GetX() / Bumpers[0].width);
  y = std::floor(vp->GetY() / Bumpers[0].height);
  y *= Bumpers[0].height;

  /***************************************************************************
   * --------------------------------------------------> (x)
     |  ┌───────────────────────────────┐
     |  │                               │
     |  │                               │
     |  │                               │
     |  │                               │
     |  │                               │
     |  │           ┌───────────────────┘
     |  │           │
     |  │           │
     |  │           │
     |  │           │
     |  │           │
     |  xxxxxxxxxxxxx
     |
     \/
     (y)
  ***************************************************************************/
  bumper.SetType(MAX_X - 1393, Bumpers[0].height, DILIndex::kBumper);
  if (bumper.IsVisible(MIN_X, MAX_Y, fb)) {
    x *= Bumpers[0].width;
    y = MAX_Y;

    for (int i = 0; x < (int)(vp->GetX() + fb->get_width()) && x < 1393; i++) {
      bumper.CheckCollision(x, y, particle);
      x += Bumpers[0].width;
    }
  }

  x = std::floor(vp->GetX() / Bumpers[0].width);
  y = std::floor(vp->GetY() / Bumpers[0].height);
  y *= Bumpers[0].height;
  /***************************************************************************
   * --------------------------------------------------> (x)
     |  ┌───────────────────────────────┐
     |  │                               │
     |  │                               │
     |  │                               │
     |  │                               │
     |  │                               │
     |  │           xxxxxxxxxxxxxxxxxxxxx
     |  │           │
     |  │           │
     |  │           │
     |  │           │
     |  │           │
     |  └───────────┘
     |
     \/
     (y)
  ***************************************************************************/
  bumper.SetType(MAX_X - 1459, Bumpers[0].height, DILIndex::kBumper);
  if (bumper.IsVisible(1459, 8663, fb)) {
    x *= Bumpers[0].width;
    y = 8663;

    for (int i = 0; x < (int)(vp->GetX() + fb->get_width()) && x < MAX_X; i++) {
      bumper.CheckCollision(x, y, particle);
      x += Bumpers[0].width;
    }
  }

  x = std::floor(vp->GetX() / Bumpers[0].width);
  y = std::floor(vp->GetY() / Bumpers[0].height);
  x *= Bumpers[0].width;

  /***************************************************************************
   * --------------------------------------------------> (x)
     |  x───────────────────────────────┐
     |  x                               │
     |  x                               │
     |  x                               │
     |  x                               │
     |  x                               │
     |  x           ┌───────────────────┘
     |  x           │
     |  x           │
     |  x           │
     |  x           │
     |  x           │
     |  x───────────┘
     |
     \/
     (y)
  ***************************************************************************/
  bumper.SetType(Bumpers[0].width, MAX_Y - MIN_Y, DILIndex::kBumper);
  if (bumper.IsVisible(MIN_X, MIN_Y, fb)) {
    x = MIN_X;
    y *= Bumpers[0].height;

    bumper.SetType(Bumpers[0].width, Bumpers[0].height, DILIndex::kBumper);

    for (int i = 0; y < (int)(vp->GetY() + fb->get_height()) && y < MAX_Y;
         i++) {
      bumper.CheckCollision(x, y, particle);
      y += Bumpers[0].width;
    }
  }

  x = std::floor(vp->GetX() / Bumpers[0].width);
  y = std::floor(vp->GetY() / Bumpers[0].height);
  x *= Bumpers[0].width;

  /***************************************************************************
   * --------------------------------------------------> (x)
     |  ┌───────────────────────────────┐
     |  │                               │
     |  │                               │
     |  │                               │
     |  │                               │
     |  │                               │
     |  │          x────────────────────┘
     |  │          x
     |  │          x
     |  │          x
     |  │          x
     |  │          x
     |  └──────────x
     |
     \/
     (y)
  ***************************************************************************/
  if (bumper.IsVisible(1382, 8733 /*8695*/, fb)) {
    x = 1382;
    y *= Bumpers[0].height;

    bumper.SetType(Bumpers[0].width, Bumpers[0].height, DILIndex::kBumper);

    for (int i = 0; y < (int)(vp->GetY() + fb->get_height()) && y < 9500; i++) {
      bumper.CheckCollision(x, y, particle);
      y += Bumpers[0].width;
    }
  }

  x = std::floor(vp->GetX() / Bumpers[0].width);
  y = std::floor(vp->GetY() / Bumpers[0].height);
  x *= Bumpers[0].width;
  /***************************************************************************
   * --------------------------------------------------> (x)
     |  ┌───────────────────────────────x
     |  │                               x
     |  │                               x
     |  │                               x
     |  │                               x
     |  │                               x
     |  │           ┌───────────────────x
     |  │           │
     |  │           │
     |  │           │
     |  │           │
     |  │           │
     |  └───────────┘
     |
     \/
     (y)
  ***************************************************************************/
  bumper.SetType(Bumpers[0].width, MAX_Y - MIN_Y, DILIndex::kBumper);
  if (bumper.IsVisible(4297, MIN_Y, fb)) {
    // x = MAX_X;
    x = 4297;
    y *= Bumpers[0].height;

    for (int i = 0; y < (int)(vp->GetY() + fb->get_height()) && y < 8733; i++) {
      bumper.CheckCollision(x, y, particle);
      y += Bumpers[0].width;
    }
  }
}

static void updateBumper(FrameView *fb, Particle &particle) {
  (void)fb;
  Bumper &bumper = Bumper::GetInstance();

  for (int i = 0; i < static_cast<int>(WorldObjects::kBumpers); i++) {
    if (Bumpers[i].visible)
      bumper.CheckCollision(Bumpers[i].x, Bumpers[i].y, particle);
  }
}

static void updateBooster(FrameView *fb, Particle &particle) {
  (void)fb;
  Booster &booster = Booster::GetInstance();

  for (int i = 0; i < static_cast<int>(WorldObjects::kBoosters); i++) {
    if (Boosters[i].visible)
      booster.CheckCollision(Boosters[i].x, Boosters[i].y, particle);
  }
}

static void updateCoin(FrameView *fb, Particle &particle) {
  (void)fb;
  Coin &coin = Coin::GetInstance();

  for (int i = 0; i < static_cast<int>(WorldObjects::kCollectables); i++) {
    if (!Collectables[i].visible) continue;

    bool collided =
        coin.CheckCollision(Collectables[i].x, Collectables[i].y, particle);

    if (!Collectables[i].picked) Collectables[i].picked = collided;
  }
}

void WorldInit() {
  /* first set to default values for the number of callers */
  for (int i = 0; i < static_cast<int>(DILIndex::kNumDIL); i++)
    DIL::GetInstance().SetNumberOfCallers(static_cast<DILIndex>(i), 1);

  for (int i = static_cast<int>(DILIndex::kBlackHole);
       i < static_cast<int>(DILIndex::kNumDIL); i++) {
    DIL::GetInstance().SetNumberOfCallers(
        static_cast<DILIndex>(i), static_cast<int>(WorldObjects::kBlackHoles));
  }

  for (int i = 0; i < static_cast<int>(WorldObjects::kCollectables); i++)
    Collectables[i].visible = true;

  DIL::GetInstance().SetNumberOfCallers(
      DILIndex::kPickup, static_cast<int>(WorldObjects::kPickups));
}

void WorldDeinit() {
  for (int i = 0; i < static_cast<int>(WorldObjects::kCollectables); i++)
    Collectables[i].visible = true;
}

void WorldDraw(FrameView *fb) {
  drawNubixLogo(fb);
  drawSixEuroLogo(fb);
  drawBlackHole(fb);
  drawHeadline(fb);
  drawBumper(fb);
  drawButtons(fb);
  drawBooster(fb);
  drawPickups(fb);
  drawCoins(fb);
  drawWorldLimits(fb);
  drawFadingText(fb);
}

Action WorldUpdate(FrameView *fb, Particle &particle) {
  Action index = Action::kKeepRunning;

  updateBlackHole(fb, particle);
  updateFadingText(fb, particle);
  index = updatePickup(fb, particle);
  updateBumper(fb, particle);
  updateWorldLimitsBumper(fb, particle);
  updateBooster(fb, particle);
  updateCoin(fb, particle);

  return index;
}
