#<KHeader>
#+=========================================================================
#I  Project Name: EApiDK Embedded Application Development Kit
#+=========================================================================
#I  $HeadURL$
#+=========================================================================
#I   Copyright: Copyright (c) 2002-2009, Kontron Embedded Modules GmbH
#I      Author: John Kearney,                  John.Kearney@kontron.com
#I
#I     License: All rights reserved. This program and the accompanying
#I              materials are licensed and made available under the
#I              terms and conditions of the BSD License which
#I              accompanies this distribution. The full text of the
#I              license may be found at
#I              http://opensource.org/licenses/bsd-license.php
#I
#I              THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "
#I              AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF
#I              ANY KIND, EITHER EXPRESS OR IMPLIED.
#I
#I Description: Auto Created for makefile
#I
#+-------------------------------------------------------------------------
#I
#I  File Name            : makefile
#I  File Location        : apps\EApiValidateAPI\linux
#I  Last committed       : $Revision$
#I  Last changed by      : $Author$
#I  Last changed date    : $Date$
#I  ID                   : $Id$
#I
#+=========================================================================
#</KHeader>
ARCH = $(shell uname -s)
ifndef MACH
MACH = $(shell uname -m)
endif
ifeq ($(ARCH), HP-UX)
ARCH=HPUX
endif
# Note: for cross compilation
# This script does allow for CROSS Compilation
# to achieve this specify the following
# variables
# CFLAGS
# MACH
.SUFFIXES:
.SUFFIXES: .c .s .o .exe .H

# static library
AR = ar
RANLIB = ranlib
ARFLAGS = -ruv

BinFolder=../../bin/$(ARCH)/$(MACH)/rel/products
DBinFolder=../../bin/$(ARCH)/$(MACH)/dbg/products
BuildDir=./build/$(MACH)/rel
DBuildDir=./build/$(MACH)/dbg
PrgFldr=$(join $(shell pwd)/, ..)
LibFldr=$(join $(shell pwd)/, ../../../lib)
LibcppFldr=$(join $(shell pwd)/, ../../../lib)
LiblinuxFldr=$(join $(shell pwd)/, ../../../lib/linux)
LibcoreFldr=$(join $(shell pwd)/, ../../../lib/core)
#LibwmiFldr=$(join $(shell pwd)/, ../../../lib/wmi)
#mnFldr=$(join $(shell pwd)/, ../../../lib/common)
#CmnFldr+=$(join $(shell pwd)/, ../../wmi)
EAPI_INC=$(join $(shell pwd)/,../../../include/EApi.h)
GET_VAR_FROM_CFILE=$(shell grep "$(2)" $(1) |sed -rn "s/^\#\s*define\s+$2\s+([0-9]+).*/\1/p")
EAPI_VERSION=$(call GET_VAR_FROM_CFILE,$(EAPI_INC),EAPI_VER)
EAPI_REVISION=$(call GET_VAR_FROM_CFILE,$(EAPI_INC),EAPI_REVISION)
EAPI_LIB_VERSION=$(call GET_VAR_FROM_CFILE,$(EAPI_INC),EAPI_LIB_VER)
EAPI_LIB_BUG=$(call GET_VAR_FROM_CFILE,$(EAPI_INC),EAPI_LIB_BUG)
EAPI_LIB_REVISION=$(call GET_VAR_FROM_CFILE,$(EAPI_INC),EAPI_LIB_REVISION)
GCCVERSION = $(shell gcc -dumpversion)

#LibName:=EApi
#VPNPID:=Aaeon
LibName:=Aaeon
VPNPID:=EAPI
LibFName:=$(LibName).$(EAPI_VERSION).$(EAPI_REVISION).o
SoName:=lib$(LibName).so.$(EAPI_VERSION)
#SoFName:=lib$(VPNPID)$(LibName).so.$(EAPI_LIB_VERSION).$(EAPI_LIB_BUG).$(EAPI_LIB_REVISION)
SoFName:=lib$(LibName)$(VPNPID).so.$(EAPI_LIB_VERSION).$(EAPI_LIB_BUG).$(EAPI_LIB_REVISION)
StaticLibFName:=$(LibName)$(VPNPID).a
ExeName=TestUtility

#DBGOPT:=-gxcoff3
#DBGOPT:=-gxcoff+
#DBGOPT:=-gcoff3
#DBGOPT:=-gdwarf-2
#DBGOPT:=-g3
#DBGOPT:=-gstabs
#DBGOPT:=-gstabs+
ifeq "$(GCCVERSION)" "4.8.5"
DBGOPT:=-g -O0 -Wno-unused-parameter -Wno-unused-label -Wno-unused-variable -std=gnu11
RELOPT:=-O3 -Wno-unused-parameter -Wno-unused-label -Wno-unused-variable -std=gnu11
else ifeq "$(GCCVERSION)" "5.4.0"
DBGOPT:=-g -O0 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-label -Wno-discarded-qualifiers
RELOPT:=-O3 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-label -Wno-discarded-qualifiers
else
#for DBG
DBGOPT:=-g -DUSE_DEBUG -Wno-unused-result -Wno-unused-parameter -Wno-pointer-arith -Wno-int-conversion -Wno-unused-variable -std=gnu11
#DBGOPT:=-g 
RELOPT:= -Wno-unused-result -Wno-unused-parameter -Wno-pointer-arith -Wno-int-conversion -Wno-unused-variable -std=gnu11
endif


INCLUDS=-I../../../include/ -I../../../lib/include/  


CFLAGS:= $(CFLAGS)-Wall -W -pedantic $(INCLUDS)
#CFLAGS += -DWMI 


ifneq ($(OS), Windows_NT)
ifndef INTERPRETER
INTERPRETER:=$(shell readelf -l $(shell which ld)|grep "\[.*:.*\]"| sed -nr "s/\s*\[.*:\s+(.*)\]/\1/p")
endif # INTERPRETER
InterpFName:=interp.o Message.o
CFLAGS:=$(CFLAGS) -fvisibility=hidden -fPIC -Wl,-e,EApiMain -D EAPI_INTERPRETER=\"$(INTERPRETER)\"
else # Check OS
InterpFName:=
endif # Check OS


define FLD_BUILD_RULE
@mkdir -p $(@)
@echo $(@)
endef

define PRG_BUILD_RULE
gcc -v $(CFLAGS) $(1) -L. -ldl -o $(@) -Wl,-Map,$(basename $(@)).map $^
endef

define DPRG_BUILD_RULE
gcc -v $(CFLAGS) $(1) -L. -ldl -o $(@) -Wl,-Map,$(basename $(@)).map $^
objcopy --only-keep-debug  "$(@)" "$(@).debug"
strip --strip-debug --strip-unneeded "$(@)"
objcopy --add-gnu-debuglink="$(@).debug" "$(@)"
endef

define C_BUILD_RULE
$(CC) $(CFLAGS) $(1) -x c -c -Wa,-a,-ad -o  $(@) $< > $(basename $(@)).lst
$(CC) $(CFLAGS) $(1) -x c -S -o  $(basename $(@)).s $<
endef

define CLEAN_BUILD_FOLDER
@rm -f $(1)/*.log
@rm -f $(1)/*.o
@rm -f $(1)/*
@rm -f $(1)/*.s
@rm -f $(1)/*.a
@rm -f $(1)/*.lst
@rm -f $(1)/*.map
@rm -f $(1)/*.exeendef
endef

define REMOVE_BIN_FILES
@rm -f $(1)
@rm -f $(basename $(1)).map
@rm -f $(basename $(1)).*
endef

# EAPI core
OBJECTS= \
      EApiAPI.o DbgChk.o \
      EApiInitLib.o \
      EApiOsNIX.o \
      
# AAEON EAPI implement:libinit
OBJECTS += \
      libinit.o \
          
# AAEON EAPI implement:boardinfo
OBJECTS += \
      eapi_boardinfo_impl.o \
      boardinfo.o \
                    
# AAEON EAPI implement:core
OBJECTS += \
      aaeon_common.o \

# AAEON EAPI implement:gpio
OBJECTS += \
      eapi_gpio_impl.o \
      gpio.o \
      
# AAEON EAPI implement:watchdog, dummy watchdog_aaeon.o
OBJECTS += \
      eapi_watchdog_impl.o \
      watchdog.o \
      
# AAEON EAPI implement:backlight
OBJECTS += \
      eapi_backlight_impl.o \
      backlight.o \
      
# AAEON EAPI implement:i2c
OBJECTS += \
      eapi_i2c_impl.o \
      i2c.o \
      

# APP
OBJECTS += \
	maintest.o \
	
define mark




# AAEON EAPI implement:smartfan
OBJECTS += \
      eapi_smartfan_impl.o \
      smartfan_ext2.o \
      smartfan.o

# AAEON EAPI implement:storage, TODO: storage is not implemented for wmi
OBJECTS += \
      eapi_storage_impl.o
      # storage.o


# AAEON EAPI implement:alert
OBJECTS += \
	  eapi_alert_impl.o \
	  alert_ext2.o \

endef #mark				

DefaultTgt: buildall

buildall: clean quick

quick:$(BinFolder) \
      $(BuildDir) \
      $(DBinFolder) \
      $(DBuildDir) \
      $(DBinFolder)/$(ExeName) \
      $(BinFolder)/$(ExeName)
	@echo SYSTEM ARCHITECTURE : $(ARCH)
	@echo CPU ARCHITECTURE .. : $(MACH)
	@echo Library Name ...... : $(LibName)
	@echo Shared Lib Name ... : $(SoName)
	@echo Static Lib Filename : $(StaticLibFName)
	@echo Shared Lib Filename : $(SoFName)
	@echo Program Name ...... : $(ExeName)
	@echo Interpreter ....... : $(INTERPRETER)
	

$(BinFolder):
	$(FLD_BUILD_RULE)

$(BuildDir):
	$(FLD_BUILD_RULE)
	
$(DBinFolder):
	$(FLD_BUILD_RULE)
		
$(DBuildDir):
	$(FLD_BUILD_RULE)
	
$(DBinFolder)/$(ExeName): $(addprefix $(DBuildDir)/,$(OBJECTS)) $(DBinFolder)/$(SoFName)
	$(call DPRG_BUILD_RULE,$(DBGOPT))

$(BinFolder)/$(ExeName):  $(addprefix $(BuildDir)/,$(OBJECTS))  $(BinFolder)/$(SoFName)
	$(call PRG_BUILD_RULE,$(RELOPT))

$(DBuildDir)/%.o:$(LibFldr)/%.c
	$(call C_BUILD_RULE,$(DBGOPT))

$(BuildDir)/%.o:$(LibFldr)/%.c
	$(call C_BUILD_RULE,$(RELOPT))

$(DBuildDir)/%.o:$(PrgFldr)/%.c
	$(call C_BUILD_RULE,$(DBGOPT))

$(BuildDir)/%.o:$(PrgFldr)/%.c
	$(call C_BUILD_RULE,$(RELOPT))
	
$(DBuildDir)/%.o:$(LiblinuxFldr)/%.c
	$(call C_BUILD_RULE,$(DBGOPT))

$(BuildDir)/%.o:$(LiblinuxFldr)/%.c
	$(call C_BUILD_RULE,$(RELOPT))

$(DBuildDir)/%.o:$(LibcoreFldr)/%.c
	$(call C_BUILD_RULE,$(DBGOPT))

$(BuildDir)/%.o:$(LibcoreFldr)/%.c
	$(call C_BUILD_RULE,$(RELOPT))
	
#$(DBuildDir)/%.o:$(LibwmiFldr)/%.c
#	$(call C_BUILD_RULE,$(DBGOPT))

#$(BuildDir)/%.o:$(LibwmiFldr)/%.c
#	$(call C_BUILD_RULE,$(RELOPT))
	
#$(DBuildDir)/%.o:$(CmnFldr)/%.c
#	$(call C_BUILD_RULE,$(DBGOPT))

#$(BuildDir)/%.o:$(CmnFldr)/%.c
#	$(call C_BUILD_RULE,$(RELOPT))	
		

$(DBuildDir)/%.o:$(LibcppFldr)/%.cpp
	$(call C_BUILD_RULE,$(DBGOPT))

$(BuildDir)/%.o:$(LibcppFldr)/%.cpp
	$(call C_BUILD_RULE,$(RELOPT))


.PHONY: clean
.PHONY: help
clean::
	
	$(call REMOVE_BIN_FILES,$(DBuildDir)/$(ExeName))
	$(call REMOVE_BIN_FILES,$(BuildDir)/$(ExeName))
	$(call REMOVE_BIN_FILES,$(DBinFolder)/$(ExeName))
	$(call REMOVE_BIN_FILES,$(BinFolder)/$(ExeName))
	$(call CLEAN_BUILD_FOLDER,$(DBuildDir))
	$(call CLEAN_BUILD_FOLDER,$(BuildDir))
	


help::
	@echo ###############################################################################
	@echo #        Build Mak Files
	@echo ###############################################################################
	@echo # help
	@echo # clean
	@echo # quick
	@echo # buildall
	@echo #

