#include <EApiLib.h>

#include <aaeon/hook_alert.h>
#include <eapi_alert_impl.h>

bool gAlertInitialized = false;
#define IS_ALERT_INITIALIZED      (gAlertInitialized == 1)

static struct eapi_alert_func eapi_alert_func_impl = {
    .init = NULL,
    .deinit = NULL,
    .get_cap = NULL,
    .get_value = NULL,
    .set_value = NULL,
    .check = NULL,
};

/*
 * This function should only be called on initialzing.
*/
void register_eapi_alert_func(struct eapi_alert_func func)
{
    if (IS_ALERT_INITIALIZED) {
        DBG("Warning: %s: function is already initialized.\n", __func__);
        return;
    }

    eapi_alert_func_impl.init = func.init;
    eapi_alert_func_impl.deinit = func.deinit;
    eapi_alert_func_impl.get_cap = func.get_cap;
    eapi_alert_func_impl.get_value = func.get_value;
    eapi_alert_func_impl.set_value = func.set_value;
    eapi_alert_func_impl.check = func.check;
    eapi_alert_func_impl.reset = func.reset;
}

/*
 *
 *
 *
 *  Alert Function
 *
 *
 *
 */

EApiStatus_t EApiLibInitializeAlert(void)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_alert_func_impl.init) {
        StatusCode = eapi_alert_func_impl.init();
        DBG("eapi_alert_func_impl.init() called\n");
    }

    return StatusCode;
}

EApiStatus_t EApiLibUnInitializeAlert(void)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_alert_func_impl.deinit) {
        StatusCode = eapi_alert_func_impl.deinit();
    }

    return StatusCode;
}

EApiStatus_t EApiAlertGetCapImpl(EApiId_t Id, uint32_t *pBoundarySupport, uint32_t *pMaxUpperBound, uint32_t *pMinLowerBound)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_alert_func_impl.get_cap) {
        StatusCode = eapi_alert_func_impl.get_cap(Id, pBoundarySupport, pMaxUpperBound, pMinLowerBound);
    }

    return StatusCode;
}

EApiStatus_t EApiAlertGetValueImpl(EApiId_t Id, uint32_t *pUpperBound, uint32_t *pLowerBound)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_alert_func_impl.get_value) {
        StatusCode = eapi_alert_func_impl.get_value(Id, pUpperBound, pLowerBound);
    }

    return StatusCode;
}

EApiStatus_t EApiAlertSetValueImpl(EApiId_t Id, uint32_t UpperBound, uint32_t LowerBound)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_alert_func_impl.set_value) {
        StatusCode = eapi_alert_func_impl.set_value(Id, UpperBound, LowerBound);
    }

    return StatusCode;
}

EApiStatus_t EApiAlertCheckImpl(EApiId_t Id, uint32_t *pAlertStatus)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_alert_func_impl.check) {
        StatusCode = eapi_alert_func_impl.check(Id, pAlertStatus);
    }

    return StatusCode;
}

EApiStatus_t EApiAlertResetImpl(EApiId_t Id)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_alert_func_impl.reset) {
        StatusCode = eapi_alert_func_impl.reset(Id);
    }

    return StatusCode;
}
