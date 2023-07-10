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
 *I              THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN '
 *I              AS IS' BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF
 *I              ANY KIND, EITHER EXPRESS OR IMPLIED.
 *I
 *I Description: Auto Created for DbgPrt.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : DbgChk.h
 *I  File Location        : lib\include
 *I  Last committed       : $Revision$
 *I  Last changed by      : $Author$
 *I  Last changed date    : $Date$
 *I  ID                   : $Id$
 *I
 *+=========================================================================
 *</KHeader>
 */
#ifndef _DBGCHK_H_
#define _DBGCHK_H_

#include <EApiLib.h>
#if 1
#ifdef USE_DEBUG
#if defined(__ANDROID__) || defined(ANDROID)
// For Android Debug
#include <android/log.h>

#ifdef _ANDROID_LOG_H
#define Log(lv, format, ...)    __android_log_print(lv, __func__, format,##__VA_ARGS__)
#define DBG(format, ...)        Log(ANDROID_LOG_DEBUG, "%d:"format, __LINE__,##__VA_ARGS__)
#endif // _ANDROID_LOG_H
#else  // defined(__ANDROID__) || defined(ANDROID)
//void __cdecl DebugMsg(__IN const char *const fmt, ...);
//void __cdecl DebugMsgInfo(__IN const char *const fmt, ...);
#define DBG(format, ...) DebugMsg(format,##__VA_ARGS__)
#define DBGInfo(format, ...) DebugMsgInfo(format,##__VA_ARGS__)
#endif // defined(__ANDROID__) || defined(ANDROID)
#else // USE_DEBUG
#define DBG(format, ...) 
#define DBGInfo(format, ...) 
#endif // USE_DEBUG
#endif



#ifdef __cplusplus
extern "C" {
#endif



signed int
__cdecl
siFormattedMessage_M2(
    char          cType       ,
    const char *  cszFileName ,
    const char *  cszFuncName ,
    unsigned int  uiLineNum   ,
    const char *  csz2ndValue ,
    const char *  cszFormat   ,
    ...
    )
EAPI_PRINTF_ARG(6, 7);


signed int
__cdecl
siFormattedMessage_SC(
    char          cType       ,
    const char *  cszFileName ,
    const char *  cszFuncName ,
    unsigned int  uiLineNum   ,
    EApiStatus_t  StatusCode  ,
    const char *  cszFormat   ,
    ...
    )
EAPI_PRINTF_ARG (6, 7);


#ifdef __cplusplus
}
#endif



#endif /* _DBGCHK_H_ */


