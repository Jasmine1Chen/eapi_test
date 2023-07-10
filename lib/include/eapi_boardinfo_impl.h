#ifndef _EAPI_BOARDINFO_IMPL_H_
#define _EAPI_BOARDINFO_IMPL_H_

#include <EApiLib.h>

#ifdef __cplusplus
extern "C" {
#endif

EApiStatus_t EApiLibInitializeBoardInfo(void);
EApiStatus_t EApiLibUnInitializeBoardInfo(void);
EApiStatus_t EApiBoardGetValueImpl(__IN uint32_t Id , __OUT uint32_t *pValue);
EApiStatus_t EApiBoardGetStringAImpl(__IN uint32_t Id , __OUT char *pBuffer , __INOUT uint32_t *pBufLen);

struct eapi_boardinfo_func
{
// Called by EApiLibInitialize
    EApiStatus_t (*init)(void);
// Called by EApiLibUnInitialize
    EApiStatus_t (*deinit)(void);
// Called by EApiBoardGetValueImpl
    uint32_t (*get_value)(__IN uint32_t Id , __OUT uint32_t *pValue);
// Called by EApiBoardGetStringAImpl
    uint32_t (*get_string_a)(__IN uint32_t Id , __OUT char *pBuffer , __INOUT uint32_t *pBufLen);
};

void register_eapi_boardinfo_func(struct eapi_boardinfo_func func);

#ifdef __cplusplus
}
#endif

#endif // _EAPI_BOARDINFO_IMPL_H_
