/*******************************************************************************
 * @file Buttons.cpp
 * @author Andreas Petter
 * @date 2023-01-09
 * @version v1.0
 * @brief hardware driver to capacitive touch buttons
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

#include "sensor/Buttons.hpp"

const uint16_t cPioInstructions[] = {
            // new_event:
    0xa022, //  0: mov x, y                ;save new samples as reference in X
    0xa0c2, //  1: mov    isr, y
    0x8000, //  2: push noblock            ;push new samples into RX queue
    0xc000, //  3: irq    nowait 0
            //.wrap_target
    0xff1f, //  4: set pins, 0x1f [31]     ;raise all pins high and hold them a little bit
    0xbf47, //  5: mov y, osr [31]         ;init countdown from OSR and hold pins a little bit longer on high
    0xe080, //  6: set pindirs, 0          ;start measurement by switch all pins to input mode
            // count_down:
    0x0087, //  7: jmp y-- count_down      ;do the countdown using Y
    0xa040, //  8: mov y, pins             ;get the sample from all pins into Y
    0xe000, //  9: set pins, 0             ;drive all pins low
    0xff9f, // 10: set pindirs, 0x1f [31]  ;stop measurement by switch all pins to output mode (low)
    0x1fa0, // 11: jmp x!=y new_event [31] ;generate a new event, when new samples in Y differ from reference in X
            //.wrap
};

#define touch_wrap_target 4
#define touch_wrap 11

const struct pio_program cPioProgram = {
    .instructions = cPioInstructions,
    .length = count_of(cPioInstructions),
    .origin = -1,
};

static volatile uint32_t _buttons = 0;
static int _sm_instance = -1;

void pio_sm_interrupt_handler()
{
   pio_interrupt_clear(BUTTONS_PIO_INSTANCE, 0);              // we need to do this because there is no automatic clear inside the PIO
   _buttons = pio_sm_get(BUTTONS_PIO_INSTANCE, _sm_instance); // fetch data from rx queue
}

void Buttons::init()
{
   unsigned int offset;
   pio_sm_config conf;

   // configure pins to get controlled by pio's state machines
   for (unsigned int pin = BUTTON_BASE; pin <= BUTTON_BASE + BUTTON_COUNT; pin++)
      pio_gpio_init(BUTTONS_PIO_INSTANCE, pin); // mux GPIO pin to our PIO

   offset = pio_add_program(BUTTONS_PIO_INSTANCE, &cPioProgram);
   _sm_instance = pio_claim_unused_sm(BUTTONS_PIO_INSTANCE, true);
   conf = pio_get_default_sm_config();

   sm_config_set_wrap(&conf, offset + touch_wrap_target, offset + touch_wrap);
   sm_config_set_set_pins(&conf, BUTTON_BASE, BUTTON_COUNT);
   sm_config_set_in_pins(&conf, 0);            // basically default but to get sure
   sm_config_set_clkdiv_int_frac(&conf, 2, 0); // run at 16ns cycle time
   pio_sm_init(BUTTONS_PIO_INSTANCE, _sm_instance, offset /* + touch_wrap_target */, &conf);
   pio_sm_set_enabled(BUTTONS_PIO_INSTANCE, _sm_instance, true);

   setSensitivity(64); // empirical good value

   // enable interrupt driven events
   irq_add_shared_handler(BUTTONS_PIO_INSTANCE == pio0 ? PIO0_IRQ_0 : PIO1_IRQ_0,
                          pio_sm_interrupt_handler,
                          PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
   irq_set_enabled(BUTTONS_PIO_INSTANCE == pio0 ? PIO0_IRQ_0 : PIO1_IRQ_0, true);
   pio_set_irq0_source_enabled(BUTTONS_PIO_INSTANCE, pis_interrupt0, true);
}

void Buttons::setSensitivity(unsigned int value)
{
   if (_sm_instance >= 0)
   {
      // place value into tx queue and pull it into OSR
      pio_sm_put(BUTTONS_PIO_INSTANCE, _sm_instance, value);
      pio_sm_exec(BUTTONS_PIO_INSTANCE, _sm_instance, pio_encode_pull(false, false));
   }
}

bool Buttons::isPressed(Button b)
{
   return (_buttons & b); // simple test if bit is set or not
}
