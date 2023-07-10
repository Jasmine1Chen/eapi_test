#ifndef _HOOK_I2C_H_
#define _HOOK_I2C_H_

#include <stdio.h>
#include <stdlib.h>

#include <EApiLib.h>

EApiStatus_t i2c_probe_device(__IN uint32_t Id, __IN uint32_t Addr);

EApiStatus_t i2c_get_bus_cap(__IN uint32_t Id, __OUT uint32_t *pMaxBlkLen);

EApiStatus_t i2c_write_read(__IN uint32_t Id, __IN uint8_t Addr, __INOPT uint8_t *pWBuffer, __IN uint32_t WriteBCnt,
    __OUTOPT uint8_t *pRBuffer, __IN uint32_t RBufLen, __IN uint32_t ReadBCnt);

EApiStatus_t i2c_read_transfer(__IN uint32_t Id, __IN uint32_t Addr, __IN uint32_t Cmd, __OUT uint8_t *pBuffer, __IN uint32_t BufLen, __IN uint32_t ByteCnt); 	    
    
EApiStatus_t i2c_write_transfer(__IN uint32_t Id, __IN uint32_t Addr, __IN uint32_t Cmd, __IN uint8_t *pBuffer, __IN uint32_t ByteCnt);    
    
EApiStatus_t i2c_aaeon_deinit(void);    

#endif // _HOOK_I2C_H_
