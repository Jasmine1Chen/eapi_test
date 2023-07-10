#ifndef _HOOK_BACKLIGHT_H_
#define _HOOK_BACKLIGHT_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <EApiLib.h>

EApiStatus_t blctrl_init(void);
EApiStatus_t blctrl_deinit(void);
EApiStatus_t blctrl_get_enable(__IN uint32_t Id, __OUT uint32_t *pEnable);
EApiStatus_t blctrl_set_enable(__IN uint32_t Id, __IN uint32_t Enable);
EApiStatus_t blctrl_get_brightness(__IN uint32_t Id, __OUT uint32_t *pBrightness);
EApiStatus_t blctrl_set_brightness(__IN uint32_t Id, __IN uint32_t Brightness);

#endif // _HOOK_BACKLIGHT_H_
