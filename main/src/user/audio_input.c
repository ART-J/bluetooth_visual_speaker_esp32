/*
 * audio_input.c
 *
 *  Created on: 2019-07-05 21:22
 *      Author: Jack Chen <redchenjs@live.com>
 */

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"

#include "os/core.h"
#include "chip/i2s.h"
#include "user/vfx_core.h"

#define BUFF_SIZE 128

static uint8_t audio_input_mode = 0;

static void audio_input_task_handle(void *pvParameters)
{
#ifndef CONFIG_AUDIO_INPUT_NONE
    size_t bytes_read = 0, bytes_written = 0;
    char *read_buff = (char *)calloc(BUFF_SIZE * 4, sizeof(char));

    while (1) {
        xEventGroupWaitBits(
            user_event_group,
            AUDIO_INPUT_RUN_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY
        );

        i2s_read(CONFIG_AUDIO_INPUT_I2S_NUM, read_buff, BUFF_SIZE, &bytes_read, portMAX_DELAY);

        EventBits_t uxBits = xEventGroupGetBits(user_event_group);
        if (uxBits & AUDIO_INPUT_LOOP_BIT) {
            i2s_write(CONFIG_AUDIO_OUTPUT_I2S_NUM, read_buff, BUFF_SIZE, &bytes_written, portMAX_DELAY);
        }

#ifdef CONFIG_ENABLE_VFX
        // Copy data to FIFO
        uint32_t idx = 0;
        int32_t size = bytes_read;
        const uint8_t *data = (const uint8_t *)read_buff;
        while (size > 0) {
            int16_t data_l = data[idx+1] << 8 | data[idx];
            int16_t data_r = data[idx+3] << 8 | data[idx+2];
            vfx_fifo_write((data_l + data_r) / 2);
            idx  += 4;
            size -= 4;
        }
#endif // CONFIG_ENABLE_VFX
    }
#endif // CONFIG_AUDIO_INPUT_NONE
}

void audio_input_set_mode(uint8_t mode)
{
    audio_input_mode = mode;
    if (audio_input_mode == 1) {
        xEventGroupSetBits(user_event_group, AUDIO_INPUT_RUN_BIT);
        xEventGroupClearBits(user_event_group, AUDIO_INPUT_LOOP_BIT);
    } else if (audio_input_mode == 2) {
        xEventGroupSetBits(user_event_group, AUDIO_INPUT_RUN_BIT);
        xEventGroupSetBits(user_event_group, AUDIO_INPUT_LOOP_BIT);
    } else {
        xEventGroupClearBits(user_event_group, AUDIO_INPUT_RUN_BIT);
        xEventGroupClearBits(user_event_group, AUDIO_INPUT_LOOP_BIT);
    }
}

uint8_t audio_input_get_mode(void)
{
    return audio_input_mode;
}

void audio_input_init(void)
{
    xTaskCreate(audio_input_task_handle, "audioInputT", 2048, NULL, 5, NULL);
}