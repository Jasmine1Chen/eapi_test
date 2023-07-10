/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL$
 *+=========================================================================
 *I   Copyright: Copyright (c) 2002-2009, Kontron Embedded Modules GmbH
 *I      Author: John Kearney,                  John.Kearney@kontron.com
 *I
 *I     License: All rights reserved. This program and the accompanying
 *I              materials are licensed and made available under the
 *I              terms and conditions of the BSD License which
 *I              accompanies this distribution. The full text of the
 *I              license may be found at
 *I              http://opensource.org/licenses/bsd-license.php
 *I
 *I              THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "
 *I              AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF
 *I              ANY KIND, EITHER EXPRESS OR IMPLIED.
 *I
 *I Description: Auto Created for EApiAPI.cpp
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiAPI.cpp
 *I  File Location        : lib
 *I  Last committed       : $Revision$
 *I  Last changed by      : $Author$
 *I  Last changed date    : $Date$
 *I  ID                   : $Id$
 *I
 *+=========================================================================
 *</KHeader>
 */

#include <EApiLib.h>
#include <stdio.h>


int Initialized=0;

#define EAPI_CHECK_NOT_INITIALIZED(FuncName) \
  EAPI_LIB_RETURN_ERROR_IF(\
      FuncName, \
      Initialized==1, \
      EAPI_STATUS_INITIALIZED, \
      "Library Already Initialized" \
      )

#define EAPI_CHECK_INITIALIZED(FuncName) \
  EAPI_LIB_RETURN_ERROR_IF(FuncName, \
      Initialized==0, \
      EAPI_STATUS_NOT_INITIALIZED, \
      "Library Uninitialized" \
      )
      
      


/*
 *
 *
 *
 *      I2C
 *
 *
 *
 */
 
 #if SUPPORT_I2C
uint32_t
EAPI_CALLTYPE
EApiI2CGetBusCap(
    __IN  uint32_t  Id         , /* I2C Bus Id */
    __OUT uint32_t *pMaxBlkLen   /* Max Block Length
                                  * Supported on this
                                  * interface
                                  */
    )
{
  EApiStatus_t StatusCode;
  EAPI_CHECK_INITIALIZED(EApiI2CGetBusCap);
  EAPI_LIB_ASSERT_PARAMATER_NULL(EApiI2CGetBusCap, pMaxBlkLen);

  StatusCode=EApiI2CGetBusCapImpl(Id, pMaxBlkLen);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiI2CWriteReadRaw(
    __IN     uint32_t  Id       , /* I2C Bus Id */
    __IN     uint8_t   Addr     , /* Encoded 7Bit I2C
                                   * Device Address
                                   */
    __INOPT  void     *pWBuffer , /* Write Data pBuffer */
    __IN     uint32_t  WriteBCnt, /* Number of Bytes to
                                   * write
                                   */
    __OUTOPT void     *pRBuffer , /* Read Data pBuffer */
    __IN     uint32_t  RBufLen  , /* Data pBuffer Length */
    __IN     uint32_t  ReadBCnt   /* Number of Bytes to
                                   * Read
                                   */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiI2CWriteReadRaw);
  EAPI_LIB_ASSERT_PARAMATER_CHECK(
      EApiI2CWriteReadRaw,
      (WriteBCnt>1)&&(pWBuffer==NULL),
      "pWBuffer is NULL"
      );

  EAPI_LIB_ASSERT_PARAMATER_CHECK(
      EApiI2CWriteReadRaw,
      (ReadBCnt>1)&&(pRBuffer==NULL) ,
      "pRBuffer is NULL"
      );
  EAPI_LIB_ASSERT_PARAMATER_CHECK(
      EApiI2CWriteReadRaw,
      (ReadBCnt>1)&&(RBufLen==0) ,
      "RBufLen is ZERO"
      );
  EAPI_LIB_ASSERT_PARAMATER_CHECK(
      EApiI2CWriteReadRaw,
      ((WriteBCnt==0)&&(ReadBCnt==0)),
      "NO READ NO WRITE"
      );

  EAPI_LIB_PREVENT_BUF_OVERFLOW(
      EApiI2CWriteReadRaw,
      ReadBCnt,
      RBufLen+1
      );
  StatusCode=EApiI2CWriteReadImpl(
      Id,
      Addr,
      pWBuffer,
      WriteBCnt,
      pRBuffer,
      RBufLen,
      ReadBCnt
      );
  	
EAPI_LIB_ASSERT_EXIT
  return StatusCode;

}

EApiStatus_t
EAPI_CALLTYPE
EApiI2CReadTransfer(
    __IN  uint32_t  Id      , /* I2C Bus Id */
    __IN  uint32_t  Addr    , /* Encoded 7/10Bit I2C
                               * Device Address
                               */
    __IN  uint32_t  Cmd     , /* I2C Command/Offset */
    __OUT     void *pBuffer , /* Transfer Data pBuffer */
    __IN  uint32_t  BufLen  , /* Data pBuffer Length */
    __IN  uint32_t  ByteCnt   /* Byte Count to read */
    )
{
	EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  	EAPI_CHECK_INITIALIZED(EApiI2CReadTransfer);
	EAPI_LIB_ASSERT_PARAMATER_NULL(EApiI2CReadTransfer, pBuffer);
    	EAPI_LIB_ASSERT_PARAMATER_ZERO(EApiI2CReadTransfer, BufLen );
    	EAPI_LIB_ASSERT_PARAMATER_ZERO(EApiI2CReadTransfer, ByteCnt);
    	EAPI_LIB_ASSERT_PARAMATER_ZERO(EApiI2CReadTransfer, BufLen );
    	
    	StatusCode=EApiI2CReadTransferImpl(Id, Addr, Cmd, pBuffer, BufLen, ByteCnt);
    	DBG("%s pBuffer is 0x%02X\n", __func__,  pBuffer);
EAPI_LIB_ASSERT_EXIT
 	return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiI2CWriteTransfer(
        __IN  uint32_t  Id      , /* I2C Bus Id */
        __IN  uint32_t  Addr    , /* Encoded 7/10Bit I2C
                                               * Device Address
                                               */
        __IN  uint32_t  Cmd     , /* I2C Command/Offset */
        __IN  void *pBuffer , /* Transfer Data pBuffer */
        __IN  uint32_t  ByteCnt   /* Byte Count to write */
        )
{

    	EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
        EAPI_CHECK_INITIALIZED(EApiI2CWriteTransfer);
	EAPI_LIB_ASSERT_PARAMATER_NULL(EApiI2CWriteTransfer, pBuffer);
	EAPI_LIB_ASSERT_PARAMATER_ZERO(EApiI2CWriteTransfer, ByteCnt);
	
	StatusCode=EApiI2CWriteTransferImpl(Id, Addr, Cmd, pBuffer, ByteCnt);

EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiI2CProbeDevice(
    __IN  uint32_t  Id   , /* I2C Bus Id */
    __IN  uint32_t  Addr  /* Encoded 7/10Bit
                            * I2C Device Address*/
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiI2CProbeDevice);
  StatusCode = EApiI2CProbeDeviceImpl(Id, Addr);
  
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

#endif //SUPPORT_I2C
/*
 *
 *
 *
 *  Get String
 *
 *
 *
 */

EApiStatus_t
EAPI_CALLTYPE
EApiBoardGetStringA(
    __IN      EApiId_t  Id      , /* Name Id */
    __OUT     char     *pBuffer , /* Destination pBuffer */
    __INOUT   uint32_t *pBufLen   /* pBuffer Length */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiBoardGetStringA);
  EAPI_LIB_ASSERT_PARAMATER_NULL (EApiBoardGetStringA, pBufLen);
  EAPI_LIB_ASSERT_PARAMATER_CHECK(
      EApiBoardGetStringA,
      ((*pBufLen!=0)&&(pBuffer==NULL)) ,
      "pBuffer is NULL"
      );

  StatusCode=EApiBoardGetStringAImpl(Id, pBuffer, pBufLen);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

/*
 *
 *
 *
 *  Get Value
 *
 *
 *
 */
EApiStatus_t
EAPI_CALLTYPE
EApiBoardGetValue(
    __IN  EApiId_t  Id      , /* Value Id */
    __OUT uint32_t *pValue    /* Return Value */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiBoardGetValue);
  EAPI_LIB_ASSERT_PARAMATER_NULL(EApiBoardGetValue, pValue);

  if(Id==EAPI_ID_GET_EAPI_SPEC_VERSION){
    *pValue=EAPI_VERSION;
    EAPI_LIB_RETURN_SUCCESS(EApiBoardGetValue, "");
  }

  StatusCode=EApiBoardGetValueImpl(Id, pValue);
EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}
/*
 *
 *
 *
 *  SMART FAN
 *
 *
 *
 */

#if SUPPORT_SMART_FAN
EApiStatus_t
EAPI_CALLTYPE
EApiGetFanSpeed(
    __IN  EApiId_t  Id      , /* Fan Id */
    __OUT uint32_t *pSpeed    /* Fan Speed */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiGetFanSpeed);
  EAPI_LIB_ASSERT_PARAMATER_NULL(EApiGetFanSpeed, pSpeed);

  StatusCode=EApiGetFanSpeedImpl(Id, pSpeed);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiSetFanSpeed(
    __IN  EApiId_t  Id      , /* Fan Id */
    __IN  uint32_t  Speed     /* Fan Speed */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiSetFanSpeed);
  EAPI_LIB_ASSERT_PARAMATER_CHECK(
      EApiSetFanSpeed,
      (Speed>EAPI_FANSPEED_SET_FULLRUN),
      "Fan Invalid Value"
      );

  StatusCode=EApiSetFanSpeedImpl(Id, Speed);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiGetFanMode(
    __IN  EApiId_t  Id      , /* Fan Id */
    __OUT uint32_t *pMode     /* Fan Mode */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiGetFanMode);
  EAPI_LIB_ASSERT_PARAMATER_NULL(EApiGetFanMode, pMode);

  StatusCode=EApiGetFanModeImpl(Id, pMode);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiSetFanMode(
    __IN  EApiId_t  Id      , /* Fan Id */
    __IN  uint32_t  Mode      /* Fan Mode */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiSetFanMode);
  EAPI_LIB_ASSERT_PARAMATER_CHECK(
      EApiSetFanMode,
      (Mode>EAPI_FANMODE_AUTO),
      "Fan Invalid Value"
      );

  StatusCode=EApiSetFanModeImpl(Id, Mode);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

#endif	//SUPPORT_SMART_FAN

/*
 *
 *
 *
 *  B A C K L I G H T
 *
 *
 *
 */
 
#if SUPPORT_BLACK_LIGHT
EApiStatus_t
EAPI_CALLTYPE
EApiVgaGetBacklightEnable(
    __IN  EApiId_t  Id      , /* Backlight Id */
    __OUT uint32_t *pEnable   /* Backlight Enable */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiVgaGetBacklightEnable);
  EAPI_LIB_ASSERT_PARAMATER_NULL(EApiVgaGetBacklightEnable, pEnable);

  StatusCode=EApiVgaGetBacklightEnableImpl(Id, pEnable);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiVgaSetBacklightEnable(
    __IN  EApiId_t  Id      , /* Backlight Id */
    __IN  uint32_t  Enable    /* Backlight Enable */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

  EAPI_CHECK_INITIALIZED(EApiVgaSetBacklightEnable);
  EAPI_LIB_ASSERT_PARAMATER_CHECK(
      EApiVgaSetBacklightEnable,
      ((Enable!=EAPI_BACKLIGHT_SET_ON) && (Enable!=EAPI_BACKLIGHT_SET_OFF)),
      "Enable Invalid Value"
      );

  StatusCode=EApiVgaSetBacklightEnableImpl(Id, Enable);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}
EApiStatus_t
EAPI_CALLTYPE
EApiVgaGetBacklightBrightness(
    __IN  EApiId_t  Id      , /* Backlight Id */
    __OUT uint32_t *pBright   /* Backlight Brightness */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiVgaGetBacklightBrightness);
  EAPI_LIB_ASSERT_PARAMATER_NULL(EApiVgaGetBacklightBrightness, pBright);

  StatusCode=EApiVgaGetBacklightBrightnessImpl(Id, pBright);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiVgaSetBacklightBrightness(
    __IN  EApiId_t  Id      , /* Backlight Id */
    __IN  uint32_t  Bright    /* Backlight Brightness */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiVgaSetBacklightBrightness);
  EAPI_LIB_ASSERT_PARAMATER_CHECK(
      EApiVgaSetBacklightBrightness,
      (Bright>EAPI_BACKLIGHT_SET_BRIGHTEST),
      "Brightness Invalid Value"
      );

  StatusCode=EApiVgaSetBacklightBrightnessImpl(Id, Bright);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}
#endif	//SUPPORT_BLACK_LIGHT

/*
 *
 *
 *
 *  G P I O
 *
 *
 *
 */
#if SUPPORT_GPIO
EApiStatus_t
EAPI_CALLTYPE
EApiGPIOGetLevel(
    __IN  EApiId_t Id          , /* GPIO Id */
    __IN  uint32_t Bitmask     , /* Bit mask of Affected
                                  * Bits
                                  */
    __OUT uint32_t *pLevel       /* Current Level */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiGPIOGetLevel);
  EAPI_LIB_ASSERT_PARAMATER_ZERO(EApiGPIOGetLevel, Bitmask);
  EAPI_LIB_ASSERT_PARAMATER_NULL(EApiGPIOGetLevel, pLevel);

  StatusCode=EApiGPIOGetLevelImpl(Id, Bitmask, pLevel);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiGPIOSetLevel(
    __IN  EApiId_t Id          , /* GPIO Id */
    __IN  uint32_t Bitmask     , /* Bit mask of Affected
                                  * Bits
                                  */
    __IN  uint32_t Level         /* Level */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiGPIOSetLevel);
  EAPI_LIB_ASSERT_PARAMATER_ZERO(EApiGPIOSetLevel, Bitmask);

  StatusCode=EApiGPIOSetLevelImpl(Id, Bitmask, Level);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiGPIOGetDirection(
    __IN  EApiId_t Id          , /* GPIO Id */
    __IN  uint32_t Bitmask     , /* Bit mask of Affected
                                  * Bits
                                  */
    __OUT uint32_t *pDirection   /* Current Direction */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiGPIOGetDirection);
  EAPI_LIB_ASSERT_PARAMATER_ZERO(EApiGPIOGetDirection, Bitmask);
  EAPI_LIB_ASSERT_PARAMATER_NULL(EApiGPIOGetDirection, pDirection);
  StatusCode=EApiGPIOGetDirectionImpl(Id, Bitmask, pDirection);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiGPIOSetDirection(
    __IN  EApiId_t Id          , /* GPIO Id */
    __IN  uint32_t Bitmask     , /* Bit mask of Affected
                                  * Bits
                                  */
    __IN  uint32_t Direction     /* Direction */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiGPIOSetDirection);
  EAPI_LIB_ASSERT_PARAMATER_ZERO(EApiGPIOSetDirection, Bitmask);

  StatusCode=EApiGPIOSetDirectionImpl(Id, Bitmask, Direction);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiGPIOGetDirectionCaps(
    __IN     EApiId_t Id        , /* GPIO Id */
    __OUTOPT uint32_t *pInputs  , /* Supported GPIO Input
                                   * Bit Mask
                                   */
    __OUTOPT uint32_t *pOutputs   /* Supported GPIO Output
                                   * Bit Mask
                                   */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  uint32_t DpBuffer;

  EAPI_CHECK_INITIALIZED(EApiGPIOGetDirectionCaps);
  EAPI_LIB_ASSERT_PARAMATER_CHECK(
      EApiGPIOGetDirectionCaps,
      ((pOutputs==NULL)&&(pInputs==NULL)),
      "((pOutputs==NULL)&&(pInputs==NULL))"
      );

  if(pInputs==NULL)
    pInputs=&DpBuffer;
  if(pOutputs==NULL)
    pOutputs=&DpBuffer;

  StatusCode=EApiGPIOGetDirectionCapsImpl(Id, pInputs, pOutputs);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiGPIOGetCount(
    __OUT uint32_t *pCount        /* Pointer to a buffer that recevies th number fo GPIO pins */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiGPIOGetCount);
  EAPI_LIB_ASSERT_PARAMATER_NULL(EApiGPIOGetCount, pCount);

  StatusCode=EApiGPIOGetCountImpl(pCount);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

#endif	//SUPPORT_GPIO
/*
 *
 *
 *
 *  S T D
 *
 *
 *
 */
EApiStatus_t
EAPI_CALLTYPE
EApiLibInitialize(void)
{

	

  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

  EAPI_CHECK_NOT_INITIALIZED(EApiLibInitialize);
  
  EAPI_LIB_RETURN_ERROR_IF(EApiInitLib, (EAPI_STATUS_SUCCESS != EApiInitLib()), EAPI_STATUS_NOT_INITIALIZED, "Init fail.");

 
  StatusCode = EApiLibInitializeGPIO();
  EAPI_FORMATED_MES('L', EApiLibInitializeGPIO, StatusCode, "");
  
  StatusCode = EApiLibInitializeBoardInfo();
  EAPI_FORMATED_MES('L', EApiLibInitializeBoardInfo, StatusCode, "");
  
  StatusCode = EApiLibInitializeWatchdog();
  EAPI_FORMATED_MES('L', EApiLibInitializeWatchdog, StatusCode, "");
  
  StatusCode = EApiLibInitializeBacklight();
  EAPI_FORMATED_MES('L', EApiLibInitializeBacklight, StatusCode, "");
  
#if 0   
  StatusCode = EApiLibInitializeStorage();
  EAPI_FORMATED_MES('L', EApiLibInitializeStorage, StatusCode, "");
  
  StatusCode = EApiLibInitializeFan();
  EAPI_FORMATED_MES('L', EApiLibInitializeFan, StatusCode, "");
  StatusCode = EApiLibInitializeAlert();
  EAPI_FORMATED_MES('L', EApiLibInitializeAlert, StatusCode, "");
#endif

  Initialized=1;
  StatusCode=EAPI_STATUS_SUCCESS;

  EAPI_LIB_RETURN_SUCCESS(EApiLibInitialize, "");
  
 
  
EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}
EApiStatus_t
EAPI_CALLTYPE
EApiLibUnInitialize(void)
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

  EAPI_CHECK_INITIALIZED(EApiLibUnInitialize);


  EApiLibUnInitializeGPIO();
  
  EApiLibUnInitializeBoardInfo();
  
  EApiLibUnInitializeWatchdog();
  
  EApiLibUnInitializeBacklight();
  
  EApiLibUnInitializeI2C();
  
#if 0  
  EApiLibUnInitializeStorage();	
  
  EApiLibUnInitializeFan();
  EApiLibUnInitializeAlert();
  EApiUninitLib();
#endif

  Initialized=0;
  EAPI_LIB_RETURN_SUCCESS(EApiLibUnInitialize, "");

EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}



/*
 *
 *
 *
 *  W A T C H D O G
 *
 *
 *
 */
 
#if SUPPORT_WDT
EApiStatus_t
EAPI_CALLTYPE
EApiWDogGetCap(
    __OUTOPT uint32_t *pMaxDelay       ,/* Maximum Supported
                                         * Delay in milliseconds
                                         */
    __OUTOPT uint32_t *pMaxEventTimeout,/* Maximum Supported
                                         * Event Timeout in
                                         * milliseconds
                                         * 0 == Unsupported
                                         */
    __OUTOPT uint32_t *pMaxResetTimeout /* Maximum Supported
                                         * Reset Timeout in
                                         * milliseconds
                                         */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  uint32_t DummyData;
  EAPI_CHECK_INITIALIZED(EApiWDogGetCap);
  EAPI_LIB_ASSERT_PARAMATER_CHECK(
    EApiWDogGetCap,
    ((pMaxDelay==NULL)&&(pMaxEventTimeout==NULL)&&(pMaxResetTimeout==NULL)),
   "((pMaxDelay==NULL)&&(pMaxEventTimeout==NULL)&&(pMaxResetTimeout==NULL))"
  );

   if(pMaxDelay       ==NULL) pMaxDelay       =&DummyData;
   if(pMaxEventTimeout==NULL) pMaxEventTimeout=&DummyData;
   if(pMaxResetTimeout==NULL) pMaxResetTimeout=&DummyData;
  StatusCode=EApiWDogGetCapImpl(pMaxDelay, pMaxEventTimeout, pMaxResetTimeout);
EAPI_LIB_ASSERT_EXIT
	
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiWDogStart(
    __IN  uint32_t Delay       , /* Delay in milliseconds */
    __IN  uint32_t EventTimeout, /* Event Timeout in
                                  * milliseconds
                                  */
    __IN  uint32_t ResetTimeout  /* Reset Timeout in
                                  * milliseconds
                                  */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiWDogStart);
  StatusCode=EApiWDogStartImpl(Delay, EventTimeout, ResetTimeout);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}
EApiStatus_t
EAPI_CALLTYPE
EApiWDogTrigger(void)
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiWDogTrigger);
  StatusCode=EApiWDogTriggerImpl();
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}
EApiStatus_t
EAPI_CALLTYPE
EApiWDogStop(void)
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiWDogStop);
  StatusCode=EApiWDogStopImpl();
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

#endif	//SUPPORT_WDT

/*
 *
 *
 *
 *  S T O R A G E
 *
 *
 *
 */
 
#if SUPPORT_STORGE 
EApiStatus_t
EAPI_CALLTYPE
EApiStorageCap(
    __IN  EApiId_t  Id            , /* Storage Area Id */
    __OUT uint32_t  *pStorageSize , /* Total */
    __OUT uint32_t  *pBlockLength   /* Write Block Length & Alignment */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  uint32_t DummyValue;

  EAPI_CHECK_INITIALIZED(EApiStorageCap);
  EAPI_LIB_ASSERT_PARAMATER_CHECK(
      EApiStorageCap,
      ((pBlockLength==NULL)&&(pStorageSize==NULL)),
      "((pBlockLength==NULL)&&(pStorageSize==NULL))"
      );
  if(pStorageSize==NULL)
    pStorageSize=&DummyValue;         /* Better Example for a wrapper */
  if(pBlockLength==NULL)
    pBlockLength=&DummyValue;

  StatusCode=EApiStorageCapImpl(Id, pStorageSize, pBlockLength);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}
EApiStatus_t
EAPI_CALLTYPE
EApiStorageAreaRead(
    __IN  EApiId_t  Id      , /* Storage Area Id */
    __IN  uint32_t  Offset  , /* Byte Offset */
    __OUT     void *pBuffer , /* Pointer to Date pBuffer */
    __IN  uint32_t  BufLen  , /* Data pBuffer Size in bytes */
    __IN  uint32_t  ByteCnt   /* Number of bytes to read */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EApiStatus_t ErrorCode2;
  EAPI_CHECK_INITIALIZED(EApiStorageAreaRead);
/*  
#if (STRICT_VALIDATION>1)
  siFormattedMessage_M2(
          'L'                   ,
          __FILE__              ,
          "EApiI2CWriteTransfer",
          __LINE__              ,
          "Info"                ,
          "Id=%08"PRIX32" Offset=%04"PRIX32
          " BufLen=%04"PRIX32" ByteCnt=%04"PRIX32"\n",
          Id, Offset, BufLen, ByteCnt
        );
#endif
*/
  EAPI_LIB_ASSERT_PARAMATER_NULL(EApiStorageAreaRead, pBuffer);
  EAPI_LIB_ASSERT_PARAMATER_ZERO(EApiStorageAreaRead, ByteCnt);
  EAPI_LIB_ASSERT_PARAMATER_ZERO(EApiStorageAreaRead, BufLen);

  EAPI_LIB_PREVENT_BUF_OVERFLOW(EApiStorageAreaRead, ByteCnt, BufLen);

  ErrorCode2=EApiStorageAreaReadImpl(Id, Offset, pBuffer, ByteCnt);
  if(ErrorCode2!=EAPI_STATUS_SUCCESS)
    StatusCode=ErrorCode2;
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiStorageAreaWrite(
    __IN  EApiId_t  Id      , /* Storage Area Id */
    __IN  uint32_t  Offset  , /* Byte Offset */
    __IN      void *pBuffer , /* Pointer to Date pBuffer */
    __IN  uint32_t  ByteCnt   /* Number of bytes to write */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

  EAPI_CHECK_INITIALIZED(EApiStorageAreaWrite);
  /*
#if (STRICT_VALIDATION>1)
  siFormattedMessage_M2(
          'L'                   ,
          __FILE__              ,
          "EApiI2CWriteTransfer",
          __LINE__              ,
          "Info"                ,
          "Id=%08"PRIX32" Offset=%04"PRIX32
          " ByteCnt=%04"PRIX32"\n",
          Id, Offset, ByteCnt
        );
#endif
*/
  EAPI_LIB_ASSERT_PARAMATER_NULL(EApiStorageAreaWrite, pBuffer);
  EAPI_LIB_ASSERT_PARAMATER_ZERO(EApiStorageAreaWrite, ByteCnt);

  StatusCode=EApiStorageAreaWriteImpl(Id, Offset, pBuffer, ByteCnt);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

#endif	//SUPPORT_STORGE

/*
 *
 *
 *
 *  A L E R T
 *
 *
 *
 */
#if SUPPORT_ALERT
EApiStatus_t
EAPI_CALLTYPE
EApiAlertGetCap(
    __IN  EApiId_t  Id         , /* Hardware Monitor EAPI Id */
    __OUT uint32_t *pBoundarySupport, /* Boundary support */
    __OUT uint32_t *pMaxUpperBound, /* Upper Boundary Point*/
    __OUT uint32_t *pMinLowerBound /* Lower Boundary Point*/
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiAlertGetCap);
  StatusCode=EApiAlertGetCapImpl(Id, pBoundarySupport, pMaxUpperBound, pMinLowerBound);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiAlertGetValue(
    __IN  EApiId_t  Id         , /* Hardware Monitor EAPI Id */
    __OUT uint32_t *pUpperBound, /* Upper Boundary Point*/
    __OUT uint32_t *pLowerBound  /* Lower Boundary Point*/
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiAlertGetValue);
  StatusCode=EApiAlertGetValueImpl(Id, pUpperBound, pLowerBound);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiAlertSetValue(
    __IN EApiId_t Id        , /* Hardware Monitor EAPI Id */
    __IN uint32_t UpperBound, /* Upper Boundary Point*/
    __IN uint32_t LowerBound  /* Lower Boundary Point*/
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiAlertSetValue);
  StatusCode=EApiAlertSetValueImpl(Id, UpperBound, LowerBound);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiAlertCheck(
    __IN  EApiId_t  Id          , /* Hardware Monitor EAPI Id */
    __OUT uint32_t *pAlertStatus  /* Alert Status*/
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiAlertCheck);
  StatusCode=EApiAlertCheckImpl(Id, pAlertStatus);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiAlertReset(
    __IN  EApiId_t  Id /* Hardware Monitor EAPI Id */
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  EAPI_CHECK_INITIALIZED(EApiAlertReset);
  StatusCode=EApiAlertResetImpl(Id);
EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}

#endif	//SUPPORT_ALERT
