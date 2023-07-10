#include <EApiLib.h>

#include <hook_backlight.h>
#include <eapi_backlight_impl.h>

bool gBacklightInitialized = false;
#define IS_BACKLIGHT_INITIALIZED      (gBacklightInitialized == 1)

static struct eapi_backlight_func eapi_backlight_func_impl = {
    .init = NULL,
    .deinit = NULL,
    .get_enable = NULL,
    .set_enable = NULL,
    .get_brightness = NULL,
    .set_brightness = NULL,
};

/*
 * This function should only be called on initialzing.
*/
void register_eapi_backlight_func(struct eapi_backlight_func func)
{
    if (IS_BACKLIGHT_INITIALIZED) {
        DBG("Warning: %s: function is already initialized.\n", __func__);
        return;
    }

    eapi_backlight_func_impl.init = func.init;
    eapi_backlight_func_impl.deinit = func.deinit;
    eapi_backlight_func_impl.get_enable = func.get_enable;
    eapi_backlight_func_impl.set_enable = func.set_enable;
    eapi_backlight_func_impl.get_brightness = func.get_brightness;
    eapi_backlight_func_impl.set_brightness = func.set_brightness;
}

/*
 *
 *
 *
 *  Backlight Control Function
 *
 *
 *
 */

EApiStatus_t EApiLibInitializeBacklight(void)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_backlight_func_impl.init) {
        StatusCode = eapi_backlight_func_impl.init();
        DBG("eapi_backlight_func_impl.init() called\n");
    }

    return StatusCode;
}

EApiStatus_t EApiLibUnInitializeBacklight(void)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_backlight_func_impl.deinit) {
        StatusCode = eapi_backlight_func_impl.deinit();
    }

    return StatusCode;
}

EApiStatus_t EApiVgaGetBacklightEnableImpl(__IN uint32_t Id, __OUT uint32_t *pEnable)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_backlight_func_impl.get_enable) {
        StatusCode = eapi_backlight_func_impl.get_enable(Id, pEnable);
    }

    return StatusCode;
}

EApiStatus_t EApiVgaSetBacklightEnableImpl(__IN uint32_t Id, __IN uint32_t Enable)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_backlight_func_impl.set_enable) {
        StatusCode = eapi_backlight_func_impl.set_enable(Id, Enable);
    }

    return StatusCode;
}

EApiStatus_t EApiVgaGetBacklightBrightnessImpl(__IN uint32_t Id, __OUT uint32_t *pBrightness)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_backlight_func_impl.get_brightness) {
        StatusCode = eapi_backlight_func_impl.get_brightness(Id, pBrightness);
    }

    return StatusCode;
}

EApiStatus_t EApiVgaSetBacklightBrightnessImpl(__IN uint32_t Id, __IN uint32_t Brightness)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_backlight_func_impl.set_brightness) {
        StatusCode = eapi_backlight_func_impl.set_brightness(Id, Brightness);
    }

    return StatusCode;
}
