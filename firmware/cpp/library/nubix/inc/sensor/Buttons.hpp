/*******************************************************************************
 * @file Buttons.hpp
 * @author Andreas Petter
 * @date 2023-01-09
 * @version v1.0
 * @brief Hardware driver to capacitive touch buttons
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
#pragma once

#include <stdint.h>
#include <hardware/pio.h>

#define BUTTONS_PIO_INSTANCE pio0
#define BUTTON_BASE OPNIC_TOUCH_B // lowest pin number
#define BUTTON_COUNT 4           // number of consecutive pins; maximum 5

// buttons are encoded as bitmap
enum Button : uint32_t
{
   Button_A = (1 << OPNIC_TOUCH_A),
   Button_B = (1 << OPNIC_TOUCH_B),
   Button_C = (1 << OPNIC_TOUCH_C),
   Button_D = (1 << OPNIC_TOUCH_D),
};

class Buttons
{
private:
   Buttons(){};

public:
   static void init();
   static void setSensitivity(unsigned int value);
   static bool isPressed(Button b);
};
