#ifndef _HOOK_WATCHDOG_H_
#define _HOOK_WATCHDOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

EApiStatus_t watchdog_aaeon_init(void);
EApiStatus_t watchdog_aaeon_deinit(void);
EApiStatus_t watchdog_aaeon_get_cap(__OUTOPT uint32_t *pMaxDelay , __OUTOPT uint32_t *pMaxEventTimeout, __OUTOPT uint32_t *pMaxResetTimeout);
EApiStatus_t watchdog_aaeon_start(__IN uint32_t Delay , __IN uint32_t EventTimeout, __IN uint32_t ResetTimeout);
EApiStatus_t watchdog_aaeon_trigger(void);
EApiStatus_t watchdog_stop(void);

#endif // _HOOK_WATCHDOG_H_
