#include <EApiLib.h>
#include <eapi_watchdog_impl.h>
#include <hook_watchdog.h>

bool gWatchdogInitialized = false;
#define IS_WATCHDOG_INITIALIZED      (gWatchdogInitialized == 1)

static struct eapi_watchdog_func eapi_watchdog_func_impl = {

    	.init = watchdog_aaeon_init,
    	.deinit = watchdog_aaeon_deinit,
    	.get_cap = watchdog_aaeon_get_cap,
    	.start = watchdog_aaeon_start,
    	.trigger = watchdog_aaeon_trigger,
    	.stop = watchdog_stop,
    
};

/*
 * This function should only be called on initialzing.
*/
void register_eapi_watchdog_func(struct eapi_watchdog_func func) {
	
    	if (IS_WATCHDOG_INITIALIZED) {
        	DBG("Warning: %s: function is already initialized.\n", __func__);
        	return;
    	}

     	eapi_watchdog_func_impl.init = func.init;
    	eapi_watchdog_func_impl.deinit = func.deinit;
    	eapi_watchdog_func_impl.get_cap = func.get_cap;
    	eapi_watchdog_func_impl.start = func.start;
    	eapi_watchdog_func_impl.trigger = func.trigger;
    	eapi_watchdog_func_impl.stop = func.stop;
    
}

/*
 *
 *
 *
 *  Watchdog
 *
 *
 *
 */

EApiStatus_t EApiLibInitializeWatchdog(void) {

    	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    	if (eapi_watchdog_func_impl.init) {
        	StatusCode = eapi_watchdog_func_impl.init();
        	DBG("eapi_watchdog_func_impl.init() called\n");
    	}

    	return StatusCode;
}

EApiStatus_t EApiLibUnInitializeWatchdog(void) {

    	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    	if (eapi_watchdog_func_impl.deinit) {
        	StatusCode = eapi_watchdog_func_impl.deinit();
        	DBG("eapi_watchdog_func_impl.deinit() called\n");
    	}

    	return StatusCode;
}

EApiStatus_t EAPI_CALLTYPE EApiWDogGetCapImpl(__OUTOPT uint32_t *pMaxDelay , __OUTOPT uint32_t *pMaxEventTimeout, __OUTOPT uint32_t *pMaxResetTimeout) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;
	
	if (eapi_watchdog_func_impl.get_cap) StatusCode = eapi_watchdog_func_impl.get_cap(pMaxDelay, pMaxEventTimeout, pMaxResetTimeout);
    	
    	return StatusCode;
}

EApiStatus_t EApiWDogStartImpl(__IN uint32_t Delay , __IN uint32_t EventTimeout, __IN uint32_t ResetTimeout) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    	if (eapi_watchdog_func_impl.start) StatusCode = eapi_watchdog_func_impl.start(Delay, EventTimeout, ResetTimeout);
    
	return StatusCode;
}

EApiStatus_t EApiWDogTriggerImpl(void) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    	if (eapi_watchdog_func_impl.trigger) StatusCode = eapi_watchdog_func_impl.trigger();
    	
	return StatusCode;
}

EApiStatus_t EApiWDogStopImpl(void) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    	if (eapi_watchdog_func_impl.stop) StatusCode = eapi_watchdog_func_impl.stop();
    
	return StatusCode;
}


