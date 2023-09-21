/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_log.h"
#include "esp_check.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "driver/i2c.h"

#include "bsp/esp_eye.h"
#include "bsp_err_check.h"

static const char *TAG = "EYE";


static bool i2c_initialized = false;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
static adc_oneshot_unit_handle_t bsp_adc_handle = NULL;
#endif

// static const button_config_t bsp_button_config[BSP_BUTTON_NUM] = {
//     {
//         .type = BUTTON_TYPE_ADC,
// #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
//         .adc_button_config.adc_handle = &bsp_adc_handle,
// #endif
//         .adc_button_config.adc_channel = ADC_CHANNEL_0, // ADC1 channel 0 is GPIO1
//         .adc_button_config.button_index = BSP_BUTTON_MENU,
//         .adc_button_config.min = 2310, // middle is 2410mV
//         .adc_button_config.max = 2510
//     },
//     {
//         .type = BUTTON_TYPE_ADC,
// #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
//         .adc_button_config.adc_handle = &bsp_adc_handle,
// #endif
//         .adc_button_config.adc_channel = ADC_CHANNEL_0, // ADC1 channel 0 is GPIO1
//         .adc_button_config.button_index = BSP_BUTTON_PLAY,
//         .adc_button_config.min = 1880, // middle is 1980mV
//         .adc_button_config.max = 2080
//     },
//     {
//         .type = BUTTON_TYPE_ADC,
// #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
//         .adc_button_config.adc_handle = &bsp_adc_handle,
// #endif
//         .adc_button_config.adc_channel = ADC_CHANNEL_0, // ADC1 channel 0 is GPIO1
//         .adc_button_config.button_index = BSP_BUTTON_DOWN,
//         .adc_button_config.min = 720, // middle is 820mV
//         .adc_button_config.max = 920
//     },
//     {
//         .type = BUTTON_TYPE_ADC,
// #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
//         .adc_button_config.adc_handle = &bsp_adc_handle,
// #endif
//         .adc_button_config.adc_channel = ADC_CHANNEL_0, // ADC1 channel 0 is GPIO1
//         .adc_button_config.button_index = BSP_BUTTON_UP,
//         .adc_button_config.min = 280, // middle is 380mV
//         .adc_button_config.max = 480
//     }
// };

esp_err_t bsp_i2c_init(void)
{
    /* I2C was initialized before */
    if (i2c_initialized) {
        return ESP_OK;
    }

    const i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = BSP_I2C_SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = BSP_I2C_SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = CONFIG_BSP_I2C_CLK_SPEED_HZ
    };
    BSP_ERROR_CHECK_RETURN_ERR(i2c_param_config(BSP_I2C_NUM, &i2c_conf));
    BSP_ERROR_CHECK_RETURN_ERR(i2c_driver_install(BSP_I2C_NUM, i2c_conf.mode, 0, 0, 0));

    i2c_initialized = true;

    return ESP_OK;
}

esp_err_t bsp_i2c_deinit(void)
{
    BSP_ERROR_CHECK_RETURN_ERR(i2c_driver_delete(BSP_I2C_NUM));
    i2c_initialized = false;
    return ESP_OK;
}




esp_err_t bsp_leds_init(void)
{
    gpio_config_t led_io_config = {
        .pin_bit_mask = BIT64(BSP_LED_RED),
        .mode = GPIO_MODE_OUTPUT, // connected to the gate of an NMOS
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    BSP_ERROR_CHECK_RETURN_ERR(gpio_config(&led_io_config));

    led_io_config.pin_bit_mask = BIT64(BSP_LED_WHITE);
    BSP_ERROR_CHECK_RETURN_ERR(gpio_config(&led_io_config));

    return ESP_OK;
}

esp_err_t bsp_led_set(const bsp_led_t led_io, const bool on)
{
    BSP_ERROR_CHECK_RETURN_ERR(gpio_set_level((gpio_num_t) led_io, (uint32_t) on));
    return ESP_OK;
}

// esp_codec_dev_handle_t bsp_audio_codec_microphone_init(void)
// {
//     const audio_codec_data_if_t *i2s_data_if = bsp_audio_get_codec_itf();
//     if (i2s_data_if == NULL) {
//         /* Initilize I2C */
//         BSP_ERROR_CHECK_RETURN_NULL(bsp_i2c_init());
//         /* Configure I2S peripheral and Power Amplifier */
//         BSP_ERROR_CHECK_RETURN_NULL(bsp_audio_init(NULL));
//         i2s_data_if = bsp_audio_get_codec_itf();
//     }
//     assert(i2s_data_if);

//     esp_codec_dev_cfg_t codec_dev_cfg = {
//         .dev_type = ESP_CODEC_DEV_TYPE_IN,
//         .codec_if = NULL,
//         .data_if = i2s_data_if,
//     };
//     return esp_codec_dev_new(&codec_dev_cfg);
// }

// esp_err_t bsp_iot_button_create(button_handle_t btn_array[], int *btn_cnt, int btn_array_size)
// {
//     esp_err_t ret = ESP_OK;
//     if ((btn_array_size < BSP_BUTTON_NUM) ||
//             (btn_array == NULL)) {
//         return ESP_ERR_INVALID_ARG;
//     }
// #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
//     /* Initialize ADC and get ADC handle */
//     BSP_ERROR_CHECK_RETURN_NULL(bsp_adc_initialize());
//     bsp_adc_handle = bsp_adc_get_handle();
// #endif

//     if (btn_cnt) {
//         *btn_cnt = 0;
//     }
//     for (int i = 0; i < BSP_BUTTON_NUM; i++) {
//         btn_array[i] = iot_button_create(&bsp_button_config[i]);
//         if (btn_array[i] == NULL) {
//             ret = ESP_FAIL;
//             break;
//         }
//         if (btn_cnt) {
//             (*btn_cnt)++;
//         }
//     }
//     return ret;
// }
