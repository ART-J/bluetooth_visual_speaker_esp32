/*
 * init.c
 *
 *  Created on: 2018-02-10 16:37
 *      Author: Jack Chen <redchenjs@live.com>
 */

#include "os/core.h"

#include "chip/bt.h"
#include "chip/nvs.h"
#include "chip/spi.h"
#include "chip/i2s.h"

#include "user/led.h"
#include "user/vfx.h"
#include "user/bt_app.h"
#include "user/ble_app.h"
#include "user/key_scan.h"
#include "user/audio_mp3.h"
#include "user/audio_input.h"

void os_init(void)
{
    os_core_init();
}

void chip_init(void)
{
    nvs_init();

    bt_init();

#if (CONFIG_AUDIO_OUTPUT_I2S_NUM == 0) || (CONFIG_AUDIO_INPUT_I2S_NUM == 0)
    i2s0_init();
#endif

#if (CONFIG_AUDIO_OUTPUT_I2S_NUM == 1) || (CONFIG_AUDIO_INPUT_I2S_NUM == 1)
    i2s1_init();
#endif

#ifdef CONFIG_ENABLE_VFX
    hspi_init();
#endif
}

void board_init(void) {}

void user_init(void)
{
    bt_app_init();

#ifdef CONFIG_ENABLE_BLE_CONTROL_IF
    ble_app_init();
#endif

#ifdef CONFIG_ENABLE_VFX
    vfx_init();
#endif

#ifdef CONFIG_ENABLE_LED
    led_init();
#endif

#ifdef CONFIG_ENABLE_SLEEP_KEY
    key_scan_init();
#endif

#ifdef CONFIG_ENABLE_AUDIO_PROMPT
    audio_mp3_init();
#endif

#ifndef CONFIG_AUDIO_INPUT_NONE
    audio_input_init();
#endif
}
