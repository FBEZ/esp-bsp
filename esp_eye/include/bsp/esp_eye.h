/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief ESP BSP: EYE
 */

#pragma once

#include "sdkconfig.h"
#include "driver/gpio.h"
//#include "esp_codec_dev.h"
#include "iot_button.h"

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
#include "driver/i2s.h"
#else
#include "driver/i2s_std.h"
#endif

/**************************************************************************************************
 * ESP-EYE pinout
 **************************************************************************************************/

/* I2C */
#define BSP_I2C_SCL           (GPIO_NUM_23)
#define BSP_I2C_SDA           (GPIO_NUM_18)

/* Audio */
#define BSP_I2S_SCLK          (GPIO_NUM_41)
#define BSP_I2S_LCLK          (GPIO_NUM_42)
#define BSP_I2S_DIN           (GPIO_NUM_2)

/* Camera */
#define BSP_CAMERA_XCLK      (GPIO_NUM_4)
#define BSP_CAMERA_PCLK      (GPIO_NUM_25)
#define BSP_CAMERA_VSYNC     (GPIO_NUM_5)
#define BSP_CAMERA_HSYNC     (GPIO_NUM_27)
#define BSP_CAMERA_D0        (GPIO_NUM_34)
#define BSP_CAMERA_D1        (GPIO_NUM_13)
#define BSP_CAMERA_D2        (GPIO_NUM_14)
#define BSP_CAMERA_D3        (GPIO_NUM_35)
#define BSP_CAMERA_D4        (GPIO_NUM_39)
#define BSP_CAMERA_D5        (GPIO_NUM_38)
#define BSP_CAMERA_D6        (GPIO_NUM_37)
#define BSP_CAMERA_D7        (GPIO_NUM_36)


/* Others */
#define BSP_BUTTONS_IO       (GPIO_NUM_15) // All 4 buttons mapped to this GPIO

typedef enum bsp_led_t {
    BSP_LED_RED = (GPIO_NUM_21),
    BSP_LED_WHITE = (GPIO_NUM_22)
} bsp_led_t;

#ifdef __cplusplus
extern "C" {
#endif


/**************************************************************************************************
 *
 * Buttons interface
 *
 * Example configuration:
 * \code{.c}
 * button_handle_t btns[BSP_BUTTON_NUM];
 * bsp_iot_button_create(btns, NULL, BSP_BUTTON_NUM);
 * iot_button_register_cb(btns[0], ...
 * \endcode
 **************************************************************************************************/
// typedef enum {
//     BSP_BUTTON_MENU = 0,
//     BSP_BUTTON_DOWN,
//     BSP_BUTTON_UP,
//     BSP_BUTTON_PLAY,
//     BSP_BUTTON_NUM
// } bsp_button_t;

/**
 * @brief Initialize all buttons
 *
 * Returned button handlers must be used with espressif/button component API
 *
 * @note For LCD panel button which is defined as BSP_BUTTON_MAIN, bsp_display_start should
 *       be called before call this function.
 *
 * @param[out] btn_array      Output button array
 * @param[out] btn_cnt        Number of button handlers saved to btn_array, can be NULL
 * @param[in]  btn_array_size Size of output button array. Must be at least BSP_BUTTON_NUM
 * @return
 *     - ESP_OK               All buttons initialized
 *     - ESP_ERR_INVALID_ARG  btn_array is too small or NULL
 *     - ESP_FAIL             Underlaying iot_button_create failed
 */
esp_err_t bsp_iot_button_create(button_handle_t btn_array[], int *btn_cnt, int btn_array_size);

/**************************************************************************************************
 *
 * I2C interface
 *
 * There are two devices connected to I2C peripheral:
 *  - QMA7981 Inertial measurement unit
 *  - OV2640 Camera module
 **************************************************************************************************/
#define BSP_I2C_NUM     CONFIG_BSP_I2C_NUM

/**
 * @brief Init I2C driver
 *
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   I2C parameter error
 *      - ESP_FAIL              I2C driver installation error
 *
 */
esp_err_t bsp_i2c_init(void);

/**
 * @brief Deinit I2C driver and free its resources
 *
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   I2C parameter error
 *
 */
esp_err_t bsp_i2c_deinit(void);

/**************************************************************************************************
 *
 * Camera interface
 *
 * ESP32-S3-EYE is shipped with OV2640 camera module.
 * As a camera driver, esp32-camera component is used.
 *
 * Example configuration:
 * \code{.c}
 * const camera_config_t camera_config = BSP_CAMERA_DEFAULT_CONFIG;
 * esp_err_t err = esp_camera_init(&camera_config);
 * \endcode
 **************************************************************************************************/
/**
 * @brief ESP32-S3-EYE camera default configuration
 *
 * In this configuration we select RGB565 color format and 240x240 image size - matching the display.
 * We use double-buffering for the best performance.
 * Since we don't want to waste internal SRAM, we allocate the framebuffers in external PSRAM.
 * By setting XCLK to 16MHz, we configure the esp32-camera driver to use EDMA when accessing the PSRAM.
 *
 * @attention I2C must be enabled by bsp_i2c_init(), before camera is initialized
 */
#define BSP_CAMERA_DEFAULT_CONFIG         \
    {                                     \
        .pin_pwdn = GPIO_NUM_NC,          \
        .pin_reset = GPIO_NUM_NC,         \
        .pin_xclk = BSP_CAMERA_XCLK,      \
        .pin_sccb_sda = GPIO_NUM_NC,      \
        .pin_sccb_scl = GPIO_NUM_NC,      \
        .pin_d7 = BSP_CAMERA_D7,          \
        .pin_d6 = BSP_CAMERA_D6,          \
        .pin_d5 = BSP_CAMERA_D5,          \
        .pin_d4 = BSP_CAMERA_D4,          \
        .pin_d3 = BSP_CAMERA_D3,          \
        .pin_d2 = BSP_CAMERA_D2,          \
        .pin_d1 = BSP_CAMERA_D1,          \
        .pin_d0 = BSP_CAMERA_D0,          \
        .pin_vsync = BSP_CAMERA_VSYNC,    \
        .pin_href = BSP_CAMERA_HSYNC,     \
        .pin_pclk = BSP_CAMERA_PCLK,      \
        .xclk_freq_hz = 20000000,         \
        .ledc_timer = LEDC_TIMER_0,       \
        .ledc_channel = LEDC_CHANNEL_0,   \
        .pixel_format = PIXFORMAT_RGB565, \
        .frame_size = FRAMESIZE_240X240,  \
        .jpeg_quality = 12,               \
        .fb_count = 2,                    \
        .fb_location = CAMERA_FB_IN_PSRAM,\
        .sccb_i2c_port = BSP_I2C_NUM,     \
    }


/**************************************************************************************************
 *
 * LEDs
 *
 **************************************************************************************************/

/**
 * @brief Set LED's GPIOs as output push-pull
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t bsp_leds_init(void);

/**
 * @brief Turn LED on/off
 *
 * @param led_io LED io
 * @param on Switch LED on/off
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t bsp_led_set(const bsp_led_t led_io, const bool on);


// TODO

/**************************************************************************************************
 *
 * I2S audio interface
 *
 * There is only one device connected to the I2S peripheral
 *  - MEMSensing Microsystems MSM261S4030H0: 48kHz, 24bit mono digital microphone
 *
 * For microphone initialization use bsp_audio_codec_microphone_init() which is inside initialize I2S with bsp_audio_init().
 * After microphone initialization, use functions from esp_codec_dev for record audio.
 * Example audio play:
 * \code{.c}
 * esp_codec_dev_set_out_vol(spk_codec_dev, DEFAULT_VOLUME);
 * esp_codec_dev_open(spk_codec_dev, &fs);
 * esp_codec_dev_write(spk_codec_dev, wav_bytes, bytes_read_from_spiffs);
 * esp_codec_dev_close(spk_codec_dev);
 * \endcode
 **************************************************************************************************/

/**
 * @brief Init audio
 *
 * @note There is no deinit audio function. Users can free audio resources by calling i2s_del_channel()
 * @warning The type of i2s_config param is depending on IDF version.
 * @param[in]  i2s_config I2S configuration. Pass NULL to use default values (Mono, duplex, 16bit, 22050 Hz)
 * @param[out] tx_channel I2S TX channel
 * @param[out] rx_channel I2S RX channel
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_NOT_SUPPORTED The communication mode is not supported on the current chip
 *      - ESP_ERR_INVALID_ARG   NULL pointer or invalid configuration
 *      - ESP_ERR_NOT_FOUND     No available I2S channel found
 *      - ESP_ERR_NO_MEM        No memory for storing the channel information
 *      - ESP_ERR_INVALID_STATE This channel has not initialized or already started
 */
// #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
// esp_err_t bsp_audio_init(const i2s_config_t *i2s_config);
// #else
// esp_err_t bsp_audio_init(const i2s_std_config_t *i2s_config);
// #endif

/**
 * @brief Get codec I2S interface (initialized in bsp_audio_init)
 *
 * @return
 *      - Pointer to codec I2S interface handle or NULL when error occured
 */
//const audio_codec_data_if_t *bsp_audio_get_codec_itf(void);

/**
 * @brief Initialize microphone codec device
 *
 * @return Pointer to codec device handle or NULL when error occured
 */
//esp_codec_dev_handle_t bsp_audio_codec_microphone_init(void);

/**************************************************************************************************
 *
 * ADC interface
 *
 * There are multiple devices connected to ADC peripheral:
 *  - Buttons
 *
 * After initialization of ADC, use adc_handle when using ADC driver.
 **************************************************************************************************/

//#define BSP_ADC_UNIT     ADC_UNIT_1

/**
 * @brief Initialize ADC
 *
 * The ADC can be initialized inside BSP, when needed.
 *
 * @param[out] adc_handle Returned ADC handle
 */
//esp_err_t bsp_adc_initialize(void);


// #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
// /**
//  * @brief Get ADC handle
//  *
//  * @note This function is available only in IDF5 and higher
//  *
//  * @return ADC handle
//  */
// adc_oneshot_unit_handle_t bsp_adc_get_handle(void);
// #endif

#ifdef __cplusplus
}
#endif
