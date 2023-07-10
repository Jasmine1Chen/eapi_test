#include <EApiLib.h>

#include <hook_i2c.h>
#include <eapi_i2c_impl.h>

static struct eapi_i2c_func eapi_i2c_func_impl = {
	.i2c_probe_device = i2c_probe_device,
    	.i2c_get_bus_cap = i2c_get_bus_cap,
    	.i2c_w_r = i2c_write_read,
    	.i2c_read_transfer = i2c_read_transfer,
    	.i2c_write_transfer = i2c_write_transfer,
    	.i2c_aaeon_deinit = i2c_aaeon_deinit,
};

/*
 *
 *
 *
 *  i2c Function
 *
 *
 *
 */
 
 EApiStatus_t EApiI2CProbeDeviceImpl(__IN uint32_t Id, __IN uint32_t Addr) {
 
 	DBG("%s call\n", __func__);
 	
 	EApiStatus_t StatusCode = EAPI_STATUS_NOT_FOUND;
 	
 	if(eapi_i2c_func_impl.i2c_probe_device) {
 		StatusCode = eapi_i2c_func_impl.i2c_probe_device(Id, Addr);	
 	}
 	
 	return StatusCode;
 }
 
EApiStatus_t EApiI2CGetBusCapImpl(__IN uint32_t Id, __OUT uint32_t *pMaxBlkLen) {

	DBG("%s call\n", __func__);
	
	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    	if (eapi_i2c_func_impl.i2c_get_bus_cap) {
        	StatusCode = eapi_i2c_func_impl.i2c_get_bus_cap(Id, pMaxBlkLen);
   	}

	return StatusCode;
}

EApiStatus_t EApiI2CWriteReadImpl(__IN uint32_t Id, __IN uint8_t Addr, __INOPT uint8_t *pWBuffer, __IN uint32_t WriteBCnt,
    __OUTOPT uint8_t *pRBuffer, __IN uint32_t RBufLen, __IN uint32_t ReadBCnt) {
    
    	DBG("%s call\n", __func__);
    	
    	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    	if (eapi_i2c_func_impl.i2c_w_r) {
        	StatusCode = eapi_i2c_func_impl.i2c_w_r(Id, Addr, pWBuffer, WriteBCnt, pRBuffer, RBufLen, ReadBCnt);
    	}

    	return StatusCode;
}

EApiStatus_t EApiI2CReadTransferImpl(__IN uint32_t Id, __IN uint32_t Addr, __IN uint32_t Cmd, __OUT uint8_t *pBuffer, __IN uint32_t BufLen, __IN uint32_t ByteCnt) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    	if (eapi_i2c_func_impl.i2c_read_transfer) {
        	StatusCode = eapi_i2c_func_impl.i2c_read_transfer(Id, Addr, Cmd, pBuffer, BufLen, ByteCnt);
    	}

	DBG("%s pBuffer is 0x%02X\n", __func__, pBuffer);
    	return StatusCode;
}

EApiStatus_t EApiI2CWriteTransferImpl(__IN uint32_t Id, __IN uint32_t Addr, __IN uint32_t Cmd, __IN uint8_t *pBuffer, __IN uint32_t ByteCnt) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

    	if (eapi_i2c_func_impl.i2c_write_transfer) {
        	StatusCode = eapi_i2c_func_impl.i2c_write_transfer(Id, Addr, Cmd, pBuffer, ByteCnt);
    	}

    	return StatusCode;


}


EApiStatus_t EApiLibUnInitializeI2C(void) {

	EApiStatus_t StatusCode = EAPI_STATUS_UNSUPPORTED;

	if (eapi_i2c_func_impl.i2c_aaeon_deinit) {
        	StatusCode = eapi_i2c_func_impl.i2c_aaeon_deinit();
    	}

    	return StatusCode;

}


