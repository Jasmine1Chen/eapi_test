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
 *I Description: Auto Created for EApiInitLib.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiInitLib.c
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
#include <sys/stat.h>
#include <libinit.h>

#include <common.h>
#include <EApiInitLib.h>

static struct eapi_libinit_func eapi_libinit_func_impl = {
    .init = libinit_init,
    .deinit = libinit_deinit,
}; 

FILE *OutputStream=NULL;

void get_debug_level() {

	char *value;

  	value = getenv("EAPI_debug_level");
  	if(value != NULL) debug_level = atoi(value);
  	else debug_level = 0;

  	DBG("debug_level: %d\n", debug_level);


	return;  	
}

void __cdecl DebugMsg(__IN const char *const fmt, ...) {

	va_list arg;
  	if(debug_level == 1) {
    		va_start(arg, fmt);
    		if(OutputStream!=NULL) vfprintf(OutputStream, fmt, arg);
    		else vfprintf(stdout, fmt, arg);

    		fflush(OutputStream);
    		va_end(arg);
  	}
}

void __cdecl DebugMsgInfo(__IN const char *const fmt, ...) {

	va_list arg;
  	va_start(arg, fmt);
  	if(OutputStream!=NULL) vfprintf(OutputStream, fmt, arg);
  	else vfprintf(stdout, fmt, arg);

  	fflush(OutputStream);
  	va_end(arg);
}

#if 0
EApiStatus_t OpenEepromFiles(void);
EApiStatus_t CloseEepromFiles(void);
EApiStatus_t OpenI2CEepromFiles(void);
EApiStatus_t CloseI2CEepromFiles(void);
#endif

EApiStatus_t EApiInitLib() {
	
	EApiStatus_t StatusCode = EAPI_STATUS_NOT_INITIALIZED;
	
#ifdef USE_DEBUG
  	if(OutputStream==NULL){
#if EAPI_DBG_USE_OUTPUT_FILE
    		OutputStream=fopen("EAPI.log", "w");
#else // EAPI_DBG_USE_OUTPUT_FILE
    		OutputStream=stdout;
#endif // EAPI_DBG_USE_OUTPUT_FILE
  	}
#endif // USE_DEBUG
 
  	get_debug_level();
  	
  	StatusCode = eapi_libinit_func_impl.init();
	
    	DBG("#\n"
              "# Embedded API EApi\n"
              "# Version %u.%u\n"
              "# Lib Version %u.%u.%u\n"
              "#\n",
              EAPI_VER_GET_VER(EAPI_VERSION), EAPI_VER_GET_REV(EAPI_VERSION),
              EAPI_LIB_VER, EAPI_LIB_BUG, EAPI_LIB_REVISION
            );

	
  	return StatusCode;
}



EApiStatus_t EApiUninitLib() {


	DBG("#\n"
              "# Embedded API EApi\n"
              "# Exit \n"
              "#\n"
            );
           
 
  	if(OutputStream!=NULL&&OutputStream!=stdout&&OutputStream!=stderr) fclose(OutputStream);
  
  	OutputStream=stdout;
 	
  	eapi_libinit_func_impl.deinit();
  	return EAPI_STATUS_SUCCESS;
}


