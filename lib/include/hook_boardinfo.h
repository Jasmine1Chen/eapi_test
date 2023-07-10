#ifndef _HOOK_BOARDINFO_H_
#define _HOOK_BOARDINFO_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BOARDINFO_STRING_LENTH  256
#define BOARD_CONFIG_PATH	"/etc/board_config"
#define GPIO_PATH		"/sys/class/gpio/"
#define BOARDINFO_PATH  	"/sys/class/dmi/id/"
#define HWM_PATH		"/sys/class/hwmon/"
#define WDT_PATH		"/sys/class/watchdog/"
#define BACKLIGHT_PATH		"/sys/class/backlight/"
#define AD5247_PATH		"/sys/bus/i2c/devices/"
#define BOARD_I2C_PATH		"/sys/module/"


typedef enum Category {
	OBJECT, TYPE, REF1, REF2
} Category;

typedef struct boardinfo_arg_t {
    uint32_t type;
    uint32_t data_len;
    union { // data
        char *string;
        uint32_t version;
        uint32_t temperature;
        uint32_t voltage;
        uint32_t fan_speed;
    };
} boardinfo_arg_t;

int eapi_boardinfo(uint32_t cmd, unsigned long arg);

EApiStatus_t boardinfo_aaeon_init(void);
EApiStatus_t boardinfo_aaeon_deinit(void);
EApiStatus_t boardinfo_aaeon_get_value(__IN uint32_t Id , __OUT uint32_t *pValue);
//EApiStatus_t boardinfo_aaeon_get_value_ext2(__IN uint32_t Id , __OUT uint32_t *pValue);
EApiStatus_t boardinfo_aaeon_get_string_a(__IN uint32_t Id , __OUT char *pBuffer , __INOUT uint32_t *pBufLen);


size_t next_buffer(const size_t buffer, const size_t count);
size_t read_line(FILE *file, char **line);
Category next_category(const Category category);

#endif // _HOOK_BOARDINFO_H_
