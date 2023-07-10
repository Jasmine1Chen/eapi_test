#include <EApiLib.h>
//#if defined(__ANDROID__) || defined(ANDROID)
//#include <android/log.h>
//#endif
#include <EApi.h>
#include <hook_gpio.h>
#include <eapi_gpio_impl.h>

bool gGpioInitialized = false;
#define IS_GPIO_INITIALIZED      (gGpioInitialized == 1)

static struct eapi_gpio_func eapi_gpio_func_impl = {
    .init = NULL,
    .deinit = NULL,
    .get_lv = NULL,
    .set_lv = NULL,
    .get_dir = NULL,
    .set_dir = NULL,
    .get_dir_caps = NULL,
    .get_count = NULL,
};

/*
 * This function should only be called on initialzing.
*/
void register_eapi_gpio_func(struct eapi_gpio_func func)
{
    if (IS_GPIO_INITIALIZED) {
        DBG("Warning: %s: function is already initialized.\n", __func__);
        return;
    }

    eapi_gpio_func_impl.init = func.init;
    eapi_gpio_func_impl.deinit = func.deinit;
    eapi_gpio_func_impl.get_lv = func.get_lv;
    eapi_gpio_func_impl.set_lv = func.set_lv;
    eapi_gpio_func_impl.get_dir = func.get_dir;
    eapi_gpio_func_impl.set_dir = func.set_dir;
    eapi_gpio_func_impl.get_dir_caps = func.get_dir_caps;
    eapi_gpio_func_impl.get_count = func.get_count;
}
/*
 *
 *
 *
 *  G P I O
 *
 *
 *
 */

EApiStatus_t EApiLibInitializeGPIO(void)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_gpio_func_impl.init) {
        StatusCode = eapi_gpio_func_impl.init();
        DBG("eapi_gpio_func_impl.init() called\n");
        
    }

    return StatusCode;
}

EApiStatus_t EApiLibUnInitializeGPIO(void)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_gpio_func_impl.init) {
        StatusCode = eapi_gpio_func_impl.deinit();
    }

    return StatusCode;
}

EApiStatus_t EApiGPIOGetLevelImpl(__IN EApiId_t Id, __IN uint32_t BitMask, __OUT uint32_t *pLevel)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_gpio_func_impl.get_lv) {
        StatusCode = eapi_gpio_func_impl.get_lv(Id, BitMask, pLevel);
    }

    return StatusCode;
}

EApiStatus_t EApiGPIOSetLevelImpl(__IN EApiId_t Id, __IN uint32_t BitMask, __IN uint32_t Level)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_gpio_func_impl.set_lv) {
        StatusCode = eapi_gpio_func_impl.set_lv(Id, BitMask, Level);
    }

    return StatusCode;
}

EApiStatus_t EApiGPIOGetDirectionImpl(__IN EApiId_t Id, __IN uint32_t BitMask, __OUT uint32_t *pDirection) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    	if (eapi_gpio_func_impl.get_dir) StatusCode = eapi_gpio_func_impl.get_dir(Id, BitMask, pDirection);
    	

    	return StatusCode;
}


EApiStatus_t EApiGPIOSetDirectionImpl(__IN EApiId_t Id, __IN uint32_t BitMask, __IN uint32_t Direction)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_gpio_func_impl.set_dir) {
        StatusCode = eapi_gpio_func_impl.set_dir(Id, BitMask, Direction);
    }

    return StatusCode;
}

EApiStatus_t EApiGPIOGetDirectionCapsImpl(__IN EApiId_t Id, __OUTOPT uint32_t *pInputs, __OUTOPT uint32_t *pOutputs)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_gpio_func_impl.get_dir_caps) {
        StatusCode = eapi_gpio_func_impl.get_dir_caps(Id, pInputs, pOutputs);
    }

    if (StatusCode == EAPI_STATUS_UNSUPPORTED) {
        DBGInfo("DIO is Unsupported\n");
        goto unsupported;
    }
    DBGInfo("DIO is Supported\n");
    DBGInfo("DIO Caps input=0x%x, output=0x%x\n", *pInputs, *pOutputs);

unsupported:
    return StatusCode;
}

EApiStatus_t EApiGPIOGetCountImpl(__OUT uint32_t *pCount) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
	
	if (eapi_gpio_func_impl.get_count) StatusCode = eapi_gpio_func_impl.get_count(pCount);
    	
    	if (StatusCode == EAPI_STATUS_UNSUPPORTED) {
        	DBGInfo("GetCount is Unsupported\n");
        	goto count_unsupported;
    	}
    

count_unsupported:
    	return StatusCode;

}



