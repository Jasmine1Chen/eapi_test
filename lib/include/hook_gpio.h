#ifndef _HOOK_GPIO_H_
#define _HOOK_GPIO_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <EApiLib.h>

#if !defined(__ANDROID__) || !defined(ANDROID)
#include <limits.h>

typedef struct gpio_board {
    char name[NAME_MAX];
    uint32_t board_num;
}gpio_board;
#endif
// Pin 31 .. 0
//
typedef struct gpio_bank_io_caps_t {
    uint32_t gpio_bank_id;
    uint32_t gpio_input_caps;
    uint32_t gpio_output_caps;
}gpio_bank_io_caps_t;

#define GPIO_ID_DEV_2_EAPI(dev_id, eapi_id)   {dev_id, eapi_id}
typedef struct eapi_gpio_map_t {
    uint32_t dev_gpio_id;
    uint32_t eapi_gpio_id; // Mapping to EAPI GPIO BANK ID
}eapi_gpio_map_t;

EApiStatus_t gpio_init(void);
EApiStatus_t gpio_deinit(void);
EApiStatus_t gpio_get_lv(__IN EApiId_t Id, __IN uint32_t BitMask, __OUT uint32_t *pLevel);
EApiStatus_t gpio_set_lv(__IN EApiId_t Id, __IN uint32_t BitMask, __IN uint32_t Level);
EApiStatus_t gpio_get_dir(__IN EApiId_t Id, __IN uint32_t BitMask, __OUT uint32_t *pDirection);
EApiStatus_t gpio_set_dir(__IN EApiId_t Id, __IN uint32_t BitMask, __IN uint32_t Direction);
EApiStatus_t gpio_get_dir_caps(__IN EApiId_t Id, __OUTOPT uint32_t *pInputs, __OUTOPT uint32_t *pOutputs);
EApiStatus_t gpio_get_count(__OUT uint32_t *pCount);

#endif // _HOOK_GPIO_H_
