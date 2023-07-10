#ifndef _EAPI_WATCHDOG_IMPL_H_
#define _EAPI_WATCHDOG_IMPL_H_

#include <EApiLib.h>

#ifdef __cplusplus
extern "C" {
#endif

EApiStatus_t EApiLibInitializeWatchdog(void);
EApiStatus_t EApiLibUnInitializeWatchdog(void);
EApiStatus_t EAPI_CALLTYPE EApiWDogGetCapImpl(__OUTOPT uint32_t *pMaxDelay , __OUTOPT uint32_t *pMaxEventTimeout, __OUTOPT uint32_t *pMaxResetTimeout);
EApiStatus_t EApiWDogStartImpl(__IN uint32_t Delay , __IN uint32_t EventTimeout, __IN uint32_t ResetTimeout);
EApiStatus_t EApiWDogTriggerImpl(void);
EApiStatus_t EApiWDogStopImpl(void);

struct eapi_watchdog_func {
// Called by EApiLibInitialize
    EApiStatus_t (*init)(void);
// Called by EApiLibUnInitialize
    EApiStatus_t (*deinit)(void);
// Called by EApiWDogGetCapImpl
    EApiStatus_t (*get_cap)(__OUTOPT uint32_t *pMaxDelay , __OUTOPT uint32_t *pMaxEventTimeout, __OUTOPT uint32_t *pMaxResetTimeout);
// Called by EApiWDogStartImpl
    EApiStatus_t (*start)(__IN uint32_t Delay , __IN uint32_t EventTimeout, __IN uint32_t ResetTimeout);
// Called by EApiWDogTriggerImpl
    EApiStatus_t (*trigger)(void);
// Called by EApiWDogStopImpl
    EApiStatus_t (*stop)(void);
};

void register_eapi_watchdog_func(struct eapi_watchdog_func func);

#ifdef __cplusplus
}
#endif


#endif // _EAPI_WATCHDOG_IMPL_H_
