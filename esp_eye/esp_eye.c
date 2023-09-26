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


static const button_config_t bsp_button_config[BSP_BUTTON_NUM] = {
    {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = BSP_BUTTON_FUN_IO,
            .active_level = 0,
        },
    }
};

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



esp_err_t bsp_camera_init()
{

    // camera_config_t config = BSP_CAMERA_DEFAULT_CONFIG;

    // // camera init
    // esp_err_t err = esp_camera_init(&config);
    // if (err != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
    //     return ESP_FAIL;
    // }

    // sensor_t *s = esp_camera_sensor_get();
    // if (s->id.PID == OV3660_PID || s->id.PID == OV2640_PID)
    //     s->set_vflip(s, 1); //flip it back
    // else if (s->id.PID == GC0308_PID){
    //     s->set_hmirror(s, 0);
    // }
    // else if (s->id.PID == GC032A_PID){
    //     s->set_vflip(s, 1);
    // }

    // if (s->id.PID == OV3660_PID)
    // {
    //     s->set_brightness(s, 2);
    //     s->set_contrast(s, 3);
    // }

    return ESP_OK;
}



esp_err_t bsp_iot_button_create(button_handle_t btn_array[], int *btn_cnt, int btn_array_size)
{
    esp_err_t ret = ESP_OK;
    if ((btn_array_size < BSP_BUTTON_NUM) ||
            (btn_array == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (btn_cnt) {
        *btn_cnt = 0;
    }
    for (int i = 0; i < BSP_BUTTON_NUM; i++) {
        btn_array[i] = iot_button_create(&bsp_button_config[i]);
        if (btn_array[i] == NULL) {
            ret = ESP_FAIL;
            break;
        }
        if (btn_cnt) {
            (*btn_cnt)++;
        }
    }
    return ret;
}
