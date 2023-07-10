#ifndef _EAPI_GPIO_IMPL_H_
#define _EAPI_GPIO_IMPL_H_

#include <EApiLib.h>

#ifdef __cplusplus
extern "C" {
#endif

EApiStatus_t EApiLibInitializeGPIO(void);
EApiStatus_t EApiLibUnInitializeGPIO(void);
EApiStatus_t EApiGPIOGetLevelImpl(__IN EApiId_t Id, __IN uint32_t BitMask, __OUT uint32_t *pLevel);
EApiStatus_t EApiGPIOSetLevelImpl(__IN EApiId_t Id, __IN uint32_t BitMask, __IN uint32_t Level);
EApiStatus_t EApiGPIOGetDirectionImpl(__IN EApiId_t Id, __IN uint32_t BitMask, __OUT uint32_t *pDirection);
EApiStatus_t EApiGPIOSetDirectionImpl(__IN EApiId_t Id, __IN uint32_t BitMask, __IN uint32_t Direction);
EApiStatus_t EApiGPIOGetDirectionCapsImpl(__IN EApiId_t Id, __OUTOPT uint32_t *pInputs, __OUTOPT uint32_t *pOutputs);
EApiStatus_t EApiGPIOGetCountImpl(__OUT uint32_t *pCount);

struct eapi_gpio_func
{
// Called by EApiLibInitialize
    EApiStatus_t (*init)(void);
// Called by EApiLibUnInitialize
    EApiStatus_t (*deinit)(void);
// Called by EApiGPIOGetLevelImpl
    EApiStatus_t (*get_lv)(__IN EApiId_t Id, __IN uint32_t BitMask, __OUT uint32_t *pLevel);
// Called by EApiGPIOSetLevelImpl
    EApiStatus_t (*set_lv)(__IN EApiId_t Id, __IN uint32_t BitMask, __IN uint32_t Level);
// Called by EApiGPIOGetDirectionImpl
    EApiStatus_t (*get_dir)(__IN EApiId_t Id, __IN uint32_t BitMask, __OUT uint32_t *pDirection);
// Called by EApiGPIOSetDirectionImpl
    EApiStatus_t (*set_dir)(__IN EApiId_t Id, __IN uint32_t BitMask, __IN uint32_t Direction);
// Called by EApiGPIOGetDirectionCapsImpl
    EApiStatus_t (*get_dir_caps)(__IN EApiId_t Id, __OUTOPT uint32_t *pInputs, __OUTOPT uint32_t *pOutputs);
// Called by  EApiGPIOGetCountImpl
    EApiStatus_t (*get_count)(__OUT uint32_t *pCount);	
    
};

void register_eapi_gpio_func(struct eapi_gpio_func func);
#ifdef __cplusplus
}
#endif


#endif // _EAPI_GPIO_IMPL_H_
