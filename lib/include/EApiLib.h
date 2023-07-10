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
 *I Description: Auto Created for EApiLib.h
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiLib.h
 *I  File Location        : lib\include
 *I  Last committed       : $Revision$
 *I  Last changed by      : $Author$
 *I  Last changed date    : $Date$
 *I  ID                   : $Id$
 *I
 *+=========================================================================
 *</KHeader>
 */
#ifndef _EAPILIB_H_
#define _EAPILIB_H_

#ifdef __cplusplus
#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#define EAPI_BUILDING_LIB

#include <stdlib.h>
#include <EApiOs.h>
#include <EApi.h>
#include <EApiCommon.h>
#include <LibVer.h>

#define EAPI_PLATFORM_COM0 1
#define EAPI_PLATFORM_ETX  2
#define EAPI_PLATFORM_Q7   3
#define EAPI_PLATFORM_TCA  4
#if !defined EAPI_PLATFORM
#  define EAPI_PLATFORM EAPI_PLATFORM_COM0
#endif

#define SUPPORT_I2C			1
#define SUPPORT_WDT			1
#define	SUPPORT_SMART_FAN		0      
#define SUPPORT_BLACK_LIGHT		1
#define SUPPORT_GPIO			1
#define	SUPPORT_STORGE			0
#define SUPPORT_ALERT			0

//#include <EApiEmulI2C.h>
#include <DbgChk.h>
#include <eapi_boardinfo_impl.h>
#include <EApiInitLib.h>
#include <eapi_gpio_impl.h>
#include <eapi_watchdog_impl.h>
#include <eapi_backlight_impl.h>
#include <eapi_i2c_impl.h>

#ifdef BOOL
#undef BOOL
#endif

#if 0
#ifndef __cplusplus
typedef enum { false=0, true=1 } bool;
#endif
typedef bool BOOL;



#include <EApiEmulStorage.h>

#include <eapi_storage_impl.h>




#include <eapi_smartfan_impl.h>
#include <eapi_alert_impl.h>
#endif

extern char err[256];
//char boardname[256];


#ifndef EAPI_EMUL_DELAY_NS
#   define EAPI_EMUL_DELAY_NS(x)
#endif

#ifndef EAPI_LIB_MSG_OUT
#  include <stdio.h>
#  if 0
#    define EAPI_LIB_MSG_OUT(...) printf("E " ## __VA_ARGS__)
#  else
#    define EAPI_LIB_MSG_OUT(x)  printf(x)
#  endif
#endif

#ifndef EAPI_LIB_ERR_OUT
#  include <stdio.h>
#  if 0
#    define EAPI_LIB_ERR_OUT(...) printf("O " ## __VA_ARGS__)
#  else
#    define EAPI_LIB_ERR_OUT(x)   printf(x)
#  endif
#endif

/*
#if (STRICT_VALIDATION>1)
#define EAPI_FORMATED_MES1(type, func, err, desc) \
    siFormattedMessage_M2(type, __FILE__, #func, __LINE__, \
        err, "%s\n", desc );
        
#define EAPI_FORMATED_MES(type, func, err, desc) \
    siFormattedMessage_SC(type, __FILE__, #func, __LINE__, \
        (EApiStatus_t)(((uintptr_t)err)&UINT32_MAX), "%s\n", desc );        
        
#else
*/
#define EAPI_FORMATED_MES1(type, func, err, desc)
#define EAPI_FORMATED_MES(type, func, err, desc)
//#endif

#define EAPI_LIB_RETURN_SUCCESS(func, desc)  \
	do{\
  		EAPI_FORMATED_MES('L', func, EAPI_STATUS_SUCCESS, desc );\
  		StatusCode=EAPI_STATUS_SUCCESS; \
      goto ExitPoint; \
		}while(0)

#define EAPI_LIB_RETURN_ERROR(func, err, desc) \
	do{ \
  	  EAPI_FORMATED_MES('E', func, err, desc );\
  		StatusCode=err; \
      goto ExitPoint; \
	}while(0)

#define EAPI_LIB_RETURN_ERROR_IF(func, exp, err, desc)  \
  if(exp)\
  {\
    /* assert(!(exp)); */ \
    EAPI_LIB_RETURN_ERROR(func, err, desc );\
  }
#define EAPI_LIB_RETURN_ERROR_IF_S(func, exp, err)  \
  if(exp)\
  {\
    EAPI_LIB_RETURN_ERROR(func, err, TEXT(#exp) );\
  }
/*
#if (STRICT_VALIDATION>1)
#  define EAPI_LIB_ASSERT_PARAMATER_CHECK(func, exp, desc)  \
  EAPI_LIB_RETURN_ERROR_IF(func, exp, EAPI_STATUS_INVALID_PARAMETER, desc)
#else
*/
#  define EAPI_LIB_ASSERT_PARAMATER_CHECK(func, exp, desc)
//#endif

#define EAPI_LIB_ASSERT_PARAMATER_CHECK_S(func, exp)  \
  EAPI_LIB_ASSERT_PARAMATER_CHECK(func, exp, #exp)

#define EAPI_LIB_ASSERT_PARAMATER_ZERO(func, val)  \
  EAPI_LIB_ASSERT_PARAMATER_CHECK_S(func, (val==0))

#define EAPI_LIB_ASSERT_PARAMATER_NULL(func, val)  \
  EAPI_LIB_ASSERT_PARAMATER_CHECK_S(func, (val==NULL))


#define EAPI_LIB_PREVENT_BUF_OVERFLOW(func, x, y)  \
  if(x>y)\
  {\
    EAPI_FORMATED_MES('E', func, EAPI_STATUS_MORE_DATA, \
					" pBuffer Overflow Prevented" #x ">" #y );\
    x=y;\
    StatusCode=EAPI_STATUS_MORE_DATA;\
  }

#define EAPI_LIB_ASSERT_EXIT \
ExitPoint:

#endif /* _EAPILIB_H_ */

