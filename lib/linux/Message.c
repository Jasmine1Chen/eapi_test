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
 *I Description: Auto Created for Message.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : Message.c
 *I  File Location        : lib\linux
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
#define MPNPID "PMG"
#define MANUF_NAME "PICMG"
#define PROGRAM_DESC "Embedded Application Programming Interface"
char Header[]="\n"
"+----------------------------------------------------------------------------+\n"
"I              Copyright (C) Kontron Embedded Modules GmbH 2009              I\n"
"+----------------------------------------------------------------------------+\n"
"SVersion=%u.%u\n"
"LVersion=%u.%u.%u\n"
"Manufacturer="MANUF_NAME"\n"
"MPNPID="MPNPID"\n"
"OFilename=libEApi"MPNPID".so.%u.%u\n"
"Description="PROGRAM_DESC"\n";


void EApiMain (void)
{ 
  printf(
      Header, 
      EAPI_VER, EAPI_REVISION,
      LIB_VERSION, LIB_REVISION, LIB_BUILD,
      EAPI_VER, EAPI_REVISION
      );
  exit(0);
}


