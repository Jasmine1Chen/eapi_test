#ifndef _EAPI_BACKLIGHT_IMPL_H_
#define _EAPI_BACKLIGHT_IMPL_H_

#include <EApiLib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct eapi_backlight_func
{
// Called by EApiLibInitializeBacklight
    EApiStatus_t (*init)(void);
// Called by EApiLibUnInitializeBacklight
    EApiStatus_t (*deinit)(void);
// Called by EApiVgaGetBacklightEnableImpl
    EApiStatus_t (*get_enable)(__IN uint32_t Id, __OUT uint32_t *pEnable);
// Called by EApiVgaSetBacklightEnableImpl
    EApiStatus_t (*set_enable)(__IN uint32_t Id, __IN uint32_t Enable);
// Called by EApiVgaGetBacklightBrightnessImpl
    EApiStatus_t (*get_brightness)(__IN uint32_t Id, __OUT uint32_t *pBrightness);
// Called by EApiVgaSetBacklightBrightnessImpl
    EApiStatus_t (*set_brightness)(__IN uint32_t Id, __IN uint32_t Brightness);
};

EApiStatus_t EApiLibInitializeBacklight(void);
EApiStatus_t EApiLibUnInitializeBacklight(void);
EApiStatus_t EApiVgaGetBacklightEnableImpl(__IN uint32_t Id, __OUT uint32_t *pEnable);
EApiStatus_t EApiVgaSetBacklightEnableImpl(__IN uint32_t Id, __IN uint32_t Enable);
EApiStatus_t EApiVgaGetBacklightBrightnessImpl(__IN uint32_t Id, __OUT uint32_t *pBrightness);
EApiStatus_t EApiVgaSetBacklightBrightnessImpl(__IN uint32_t Id, __IN uint32_t Brightness);

void register_eapi_backlight_func(struct eapi_backlight_func func);

#ifdef __cplusplus
}
#endif

#endif // _EAPI_BACKLIGHT_IMPL_H_


