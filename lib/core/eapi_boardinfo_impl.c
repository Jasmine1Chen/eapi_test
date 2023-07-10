#include <EApiLib.h>
#include <eapi_boardinfo_impl.h>
#include <hook_boardinfo.h>

bool gBoardInfoInitialized = false;
#define IS_BOARDINFO_INITIALIZED      (gBoardInfoInitialized == 1)

struct eapi_boardinfo_func eapi_boardinfo_func_impl = {
    .init = NULL,
    .deinit = NULL,
    .get_value = NULL,
    .get_string_a = NULL,
};


/*
 * This function should only be called on initialzing.
*/
void register_eapi_boardinfo_func(struct eapi_boardinfo_func func) {

	
	if (IS_BOARDINFO_INITIALIZED) {
	
		DBG("Warning: %s: function is already initialized.\n", __func__);
		
        	return;
    	}

    	eapi_boardinfo_func_impl.init = func.init;
    	eapi_boardinfo_func_impl.deinit = func.deinit;
    	eapi_boardinfo_func_impl.get_value = func.get_value;
    	eapi_boardinfo_func_impl.get_string_a = func.get_string_a;
    	
    	
}

/*
 *
 *
 *
 *  Board Information
 *
 *
 *
 */

EApiStatus_t EApiLibInitializeBoardInfo(void)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_boardinfo_func_impl.init) {
        StatusCode = eapi_boardinfo_func_impl.init();
       
        DBG("eapi_boardinfo_func_impl.init() called\n");
        
    }

    gBoardInfoInitialized = true;

    return StatusCode;
}

EApiStatus_t EApiLibUnInitializeBoardInfo(void)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_boardinfo_func_impl.deinit) {
        StatusCode = eapi_boardinfo_func_impl.deinit();
    }

    gBoardInfoInitialized = false;

    return StatusCode;
}

EApiStatus_t EApiBoardGetValueImpl(__IN uint32_t Id , __OUT uint32_t *pValue)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_boardinfo_func_impl.get_value) {
        StatusCode = eapi_boardinfo_func_impl.get_value(Id, pValue);
    }

    return StatusCode;
}

EApiStatus_t EApiBoardGetStringAImpl(__IN uint32_t Id , __OUT char *pBuffer , __INOUT uint32_t *pBufLen)
{
    EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    if (eapi_boardinfo_func_impl.get_string_a) {
        StatusCode = eapi_boardinfo_func_impl.get_string_a(Id, pBuffer, pBufLen);
    }

    return StatusCode;
}
