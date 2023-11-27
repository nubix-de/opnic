/*******************************************************************************
 * @file LT177ML35.cpp
 * @author Andreas Petter
 * @date 2022-11-24
 * @version v1.0
 * @brief hardware driver to TFT display type LT177ML35 using GC9106 controller
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
using namespace std;

#include <pico/stdlib.h>
#include <pico/sync.h>
#include <hardware/dma.h>
#include <hardware/clocks.h>
#include <hardware/pwm.h>

#include "graphic/Display.hpp"
#include "graphic/LT177ML35.hpp"

#define PWM_PRESCALE 255
#define PWM_WRAP_VAL (125 * MHZ / PWM_PRESCALE / LED_PWM_FREQ)

static critical_section_t _criticalLock;

__attribute__((constructor)) // called before main()
static void
_prepareCriticalSection()
{
    critical_section_init(&_criticalLock);
}

LT177ML35::LT177ML35()
    : c_pio(pio0)
{
    gpio_init(OPNIC_LCD_CSN);
    gpio_set_dir(OPNIC_LCD_CSN, GPIO_OUT);
    gpio_init(OPNIC_LCD_RSTN);
    gpio_set_dir(OPNIC_LCD_RSTN, GPIO_OUT);
    gpio_init(OPNIC_LCD_TE);
    gpio_set_dir(OPNIC_LCD_TE, GPIO_IN);

    init();
}

LT177ML35::~LT177ML35()
{
}

LT177ML35 &LT177ML35::getInstance()
{
    static LT177ML35 instance;
    return instance;
}

void LT177ML35::init()
{
    initPIO();

    gpio_put(OPNIC_LCD_CSN, 1);
    gpio_put(OPNIC_LCD_WRN, 1);
    gpio_put(OPNIC_LCD_RDN, 1);
    gpio_put(OPNIC_LCD_DC, 1);

    gpio_put(OPNIC_LCD_RSTN, 1);
    sleep_ms(5);
    gpio_put(OPNIC_LCD_RSTN, 0);
    sleep_ms(15);
    gpio_put(OPNIC_LCD_RSTN, 1);
    sleep_ms(120);

    gpio_put(OPNIC_LCD_CSN, 0);

    writeCmd(0xfe); // set Inter_command high with this sequence
    writeCmd(0xfe);
    writeCmd(0xef);

    writeCmd(0xb3);  // gamma register enable access
    writeData(0x03); // enable SET_GAMMA1 and SET_GAMMA2

    writeCmd(0xb6);  // frame rate enable access
    writeData(0x01); // Frame_Rate_Set enabled

    writeCmd(0xa3);  // Frame_Rate_Set
    writeData(0x42); // 0x11 = 59.9 Hz
                     // 0x1a = 50.6 Hz
                     // 0x42 = 29.9 Hz

    writeCmd(0x35);  // Tearing Effect Line ON
    writeData(0x00); // only V-Blanking (360µs@30Hz, high active)

    writeCmd(0x35);  // Scan line set
    writeData(0x00);

    writeCmd(0x21); // Display Inversion ON

    writeCmd(0x36);  // Memory Access Ctrl
    writeData(0x98); // 0x90 = upward, leftside, normal mode, reverse refresh, RGB color filter panel
                     // 0xd8 = upward, rightside, normal mode, reverse refresh, BGR color filter panel
                     // 0x78 = downward, leftside, reverse mode, reverse refresh, BGR color filter panel
                     // 0xb8 = upward, rightside, reverse mode, reverse refresh, BGR color filter panel

    writeCmd(0x3a);  // Pixel Format Set
    writeData(0x06); // 0x03 = 12-bit/pixel 4-4-4
                     // 0x05 = 16-bit/pixel 5-6-5
                     // 0x06 = 18-bit/pixel 6-6-6

    //  writeCmd(0xb4);     //Display Inversion Control
    //  writeData(0x21);    //1 dot inversion

    writeCmd(0xF0); // SET_GAMMA1
    writeData(0x2d);
    writeData(0x54);
    writeData(0x24);
    writeData(0x61);
    writeData(0xab);
    writeData(0x2e);
    writeData(0x2f);
    writeData(0x00);
    writeData(0x20);
    writeData(0x10);
    writeData(0x10);
    writeData(0x17);
    writeData(0x13);
    writeData(0x0f);

    writeCmd(0xF1); // SET_GAMMA2
    writeData(0x02);
    writeData(0x22);
    writeData(0x25);
    writeData(0x35);
    writeData(0xa8);
    writeData(0x08);
    writeData(0x08);
    writeData(0x00);
    writeData(0x00);
    writeData(0x09);
    writeData(0x09);
    writeData(0x17);
    writeData(0x18);
    writeData(0x0f);

    writeCmd(0xfe); // set Inter_command low with this sequence
    writeCmd(0xff);

    writeCmd(0x11); // Exit Sleep
    sleep_ms(120);

    writeCmd(0x29); // Display on
    initPWM(OPNIC_LCD_BACKLIGHT);
    setBrightness(60);
    initPWM(OPNIC_LED_RED);
    initPWM(OPNIC_LED_GREEN);
    initPWM(OPNIC_LED_BLUE);
    setStatusLED(Color::Black);

    // initialize DMA for frame transfer over PIO
    m_dmaTX = dma_claim_unused_channel(true);
    dma_channel_config conf = dma_channel_get_default_config(m_dmaTX);
    channel_config_set_transfer_data_size(&conf, DMA_SIZE_32);
    channel_config_set_read_increment(&conf, true);
    channel_config_set_write_increment(&conf, false);
    channel_config_set_dreq(&conf, pio_get_dreq(c_pio, m_sm_dat3_bgr, true));
    dma_channel_set_config(m_dmaTX, &conf, false);
}

void LT177ML35::initPIO(void)
{
    const uint16_t cPioInstructions[] = {
        // wrap_target, write command/data
        0x80a0, //  0: pull   block
        0x600b, //  1: out    pins, 11
        0xb842, //  2: nop                    side 1
        0xe001, //  3: set    pins, 1
                // wrap

        // wrap_target, write bgr data bytes
        0x80e0, //  4: pull   ifempty block
        0x7008, //  5: out    pins, 8         side 0
        0xba42, //  6: nop                    side 1 [2]
        // wrap
    };
    const uint cDatCmdStart = 0;
    const uint cDatCmdWrap = 3;
    const uint cBgrCmdStart = 4;
    const uint cBgrCmdWrap = 6;
    const struct pio_program cPioProgram = {
        .instructions = cPioInstructions,
        .length = count_of(cPioInstructions),
        .origin = -1,
    };

    // configure pins to get controlled by pio's state machines
    for (unsigned int pin = OPNIC_LCD_DB0; pin <= OPNIC_LCD_DC; pin++)
        pio_gpio_init(c_pio, pin); // mux GPIO pin to our PIO

    m_pio_offset = pio_add_program(c_pio, &cPioProgram);

    // configure 1st state machine for write cmd/data bytes
    m_sm_cmd_dat = pio_claim_unused_sm(c_pio, true);
    pio_sm_config conf = pio_get_default_sm_config();
    sm_config_set_wrap(&conf, m_pio_offset + cDatCmdStart, m_pio_offset + cDatCmdWrap);
    sm_config_set_out_pins(&conf, OPNIC_LCD_DB0, 11);      // DB0..7 + RD + WR + RS
    sm_config_set_out_shift(&conf, true, false, 11); // right shift, no autopull, 11 bit thresould
    sm_config_set_set_pins(&conf, OPNIC_LCD_DC, 1);       // RS overlaps with out, but this is intended
    sm_config_set_sideset(&conf, 2, true, false);    // one optional sideset pin (=2bit)
    sm_config_set_sideset_pins(&conf, OPNIC_LCD_WRN);      // WR is our data latch
    sm_config_set_fifo_join(&conf, PIO_FIFO_JOIN_TX);
    sm_config_set_clkdiv_int_frac(&conf, 2, 0);                                                  // run full speed
    pio_sm_set_consecutive_pindirs(c_pio, m_sm_cmd_dat, OPNIC_LCD_DB0, (OPNIC_LCD_DC - OPNIC_LCD_DB0 + 1), true); // all output
    pio_sm_init(c_pio, m_sm_cmd_dat, m_pio_offset + cDatCmdStart, &conf);
    pio_sm_set_enabled(c_pio, m_sm_cmd_dat, true);

    // configure 2nd state machine for write BGR values
    m_sm_dat3_bgr = pio_claim_unused_sm(c_pio, true);
    conf = pio_get_default_sm_config();
    sm_config_set_wrap(&conf, m_pio_offset + cBgrCmdStart, m_pio_offset + cBgrCmdWrap);
    sm_config_set_out_pins(&conf, OPNIC_LCD_DB0, 8);       // DB0..7
    sm_config_set_out_shift(&conf, true, false, 24); // right shift, np autopull, 24 bit thresould
    sm_config_set_sideset(&conf, 2, true, false);    // one optional sideset pin (=2bit)
    sm_config_set_sideset_pins(&conf, OPNIC_LCD_WRN);      // WR is our data latch
    sm_config_set_fifo_join(&conf, PIO_FIFO_JOIN_TX);
    sm_config_set_clkdiv_int_frac(&conf, 2, 0);                                                   // run full speed
    pio_sm_set_consecutive_pindirs(c_pio, m_sm_dat3_bgr, OPNIC_LCD_DB0, (OPNIC_LCD_DC - OPNIC_LCD_DB0 + 1), true); // all output
    pio_sm_init(c_pio, m_sm_dat3_bgr, m_pio_offset + cBgrCmdStart, &conf);
    pio_sm_set_enabled(c_pio, m_sm_dat3_bgr, true);
}

void inline LT177ML35::writeCmd(uint8_t cmd)
{
    // drive WR and RS low, leave RD high
    pio_sm_put_blocking(c_pio, m_sm_cmd_dat, (uint32_t)cmd | 0x00000100);
}

void inline LT177ML35::writeData(uint8_t data)
{
    // drive WR low and leave RS and RD high
    pio_sm_put_blocking(c_pio, m_sm_cmd_dat, (uint32_t)data | 0x00000500);
}

// performed in 2.45ms per frame
void LT177ML35::update(uint32_t *frameBuffer, uint32_t pixelCount, bool vSync)
{
    writeCmd(0x2a); // set column address
    writeData(0);   // column start
    writeData(0);
    writeData(0); // column end
    writeData(DISP_HEIGHT - 1);
    writeCmd(0x2b); // set row address
    writeData(0);   // row start
    writeData(0);
    writeData(0); // row end
    writeData(DISP_WIDTH - 1);
    writeCmd(0x2c); // write memory

    while (vSync && gpio_get(OPNIC_LCD_TE)) // wait when TE is already raised; we are too late for this frame
        tight_loop_contents();
    while (vSync && !gpio_get(OPNIC_LCD_TE)) // wait until TE is raised
        tight_loop_contents();
    dma_channel_set_read_addr(m_dmaTX, frameBuffer, false);
    dma_channel_set_write_addr(m_dmaTX, &c_pio->txf[m_sm_dat3_bgr], false);
    dma_channel_set_trans_count(m_dmaTX, pixelCount, true);
    dma_channel_wait_for_finish_blocking(m_dmaTX);
    // 25 MByte/s is the fastest TX we can achieve with this display without glitches
}

void LT177ML35::initPWM(unsigned int pin)
{
    unsigned int slice, channel;

    gpio_set_function(pin, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(pin);
    channel = pwm_gpio_to_channel(pin);
    pwm_set_clkdiv_int_frac(slice, PWM_PRESCALE, 0);
    pwm_set_wrap(slice, PWM_WRAP_VAL);
    pwm_set_chan_level(slice, channel, PWM_WRAP_VAL + 1); // 100% duty cycle mean alway high
    pwm_set_enabled(slice, true);
}

static void _setPwmLeve(unsigned int pin, int value)
{
    if (value < 0)
        value = 0;
    pwm_set_chan_level(pwm_gpio_to_slice_num(pin), pwm_gpio_to_channel(pin), value);
}

void LT177ML35::setBrightness(unsigned int percent)
{
    if (percent > 100)
        percent = 100;
    m_brightness = percent;
    _setPwmLeve(OPNIC_LCD_BACKLIGHT, (PWM_WRAP_VAL + 1) * m_brightness / 100);
}

void LT177ML35::setStatusLED(Color color)
{
    argb_t in;

    in.raw = color;
    _setPwmLeve(OPNIC_LED_RED, (PWM_WRAP_VAL + 1) - (((PWM_WRAP_VAL + 1) * in.red >> 8)) * m_brightness / 100);
    // we have to dim green more
    _setPwmLeve(OPNIC_LED_GREEN, (PWM_WRAP_VAL + 1) - (((PWM_WRAP_VAL + 1) * in.green >> 10)) * m_brightness / 100);
    // and we have to dim blue a bit to achieve smooth color mixture
    _setPwmLeve(OPNIC_LED_BLUE, (PWM_WRAP_VAL + 1) - (((PWM_WRAP_VAL + 1) * in.blue >> 9)) * m_brightness / 100);
}
