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
#I  File Location        : lib\linux
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
# INTERPRETER  (Not used for PE Format/Windows/Cygwin)
.SUFFIXES:
.SUFFIXES: .c .s .o .exe .H

# static library
AR = ar
RANLIB = ranlib
ARFLAGS = -ruv
InstallDir=/usr/lib

BinFolder=../../apps/bin/$(ARCH)/$(MACH)/rel/products
DBinFolder=../../apps/bin/$(ARCH)/$(MACH)/dbg/products
BuildDir=./build/$(MACH)/rel
DBuildDir=./build/$(MACH)/dbg
PrgFldr=$(join $(shell pwd)/, ..)
OalFldr=.
EAPI_INC=../../include/EApi.h
GET_VAR_FROM_CFILE=$(shell grep "$(2)" $(1) |sed -rn "s/^\#\s*define\s+$2\s+([0-9]+).*/\1/p")
EAPI_VERSION=$(call GET_VAR_FROM_CFILE,$(EAPI_INC),EAPI_VER)
EAPI_REVISION=$(call GET_VAR_FROM_CFILE,$(EAPI_INC),EAPI_REVISION)
EAPI_LIB_VERSION=$(call GET_VAR_FROM_CFILE,$(EAPI_INC),EAPI_LIB_VER)
EAPI_LIB_BUG=$(call GET_VAR_FROM_CFILE,$(EAPI_INC),EAPI_LIB_BUG)
EAPI_LIB_REVISION=$(call GET_VAR_FROM_CFILE,$(EAPI_INC),EAPI_LIB_REVISION)
GCCVERSION = $(shell gcc -dumpversion)

LibName:=Aaeon
VPNPID:=EAPI
#ARCH_IF:=wmi
LibFName:=$(LibName).$(EAPI_VERSION).$(EAPI_REVISION).o
SymLinkName:=lib$(LibName).so
SoName:=lib$(LibName).so
#SoFName:=lib$(VPNPID)$(LibName).so.$(EAPI_LIB_VERSION).$(EAPI_LIB_BUG).$(EAPI_LIB_REVISION)
SoFName:=lib$(LibName)$(VPNPID).so.$(EAPI_LIB_VERSION).$(EAPI_LIB_BUG).$(EAPI_LIB_REVISION)
StaticLibFName:=$(LibName)$(VPNPID).a
#DBGOPT:=-gxcoff3
#DBGOPT:=-gxcoff+
#DBGOPT:=-gcoff3
#DBGOPT:=-gdwarf-2 -O0
#DBGOPT:=-g3
#DBGOPT:=-gstabs
#DBGOPT:=-gstabs+
ifeq "$(GCCVERSION)" "4.8.5"
DBGOPT:=-g -Wno-unused-parameter -Wno-unused-label -Wno-unused-variable -std=gnu11 -DUSE_DEBUG
RELOPT:=-O3 -Wno-unused-parameter -Wno-unused-label -Wno-unused-variable -std=gnu11
else ifeq "$(GCCVERSION)" "5.4.0"
DBGOPT:=-g -Wno-unused-variable -Wno-unused-parameter -Wno-unused-label -Wno-discarded-qualifiers -DUSE_DEBUG
RELOPT:=-O3 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-label -Wno-discarded-qualifiers -Wno-unused-result
else # check GCCVERSION
#for USE_DEBUG
DBGOPT:=-g -DUSE_DEBUG -Wno-unused-result -Wno-unused-parameter -Wno-pointer-arith -Wno-int-conversion -Wno-unused-variable -std=gnu11
#DBGOPT:=-g  
RELOPT:=-O3 -Wno-unused-result -Wno-unused-parameter -Wno-pointer-arith -Wno-int-conversion -Wno-unused-variable -std=gnu11
endif # check GCCVERSION

INCLUDS=-I../ -I../include -I../../include 
#INCLUDS=-I../ -I../include -I../../include -I../../include/mraa/api -I../../include/mraa/include -I../../include/mraa/api/mraa
#CFLAGS:=$(CFLAGS) -Wall -W -Werror $(INCLUDS)
CFLAGS:=$(CFLAGS) -Wall -W $(INCLUDS)

ifneq ($(OS), Windows_NT)
ifndef INTERPRETER
INTERPRETER:=$(shell readelf -l $(shell which ld)|grep "\[.*:.*\]"| sed -nr "s/\s*\[.*:\s+(.*)\]/\1/p")
endif # INTERPRETER
InterpFName:=interp.o Message.o
CFLAGS:=$(CFLAGS) -fvisibility=hidden -fPIC -Wl,-e,EApiMain -D EAPI_INTERPRETER=\"$(INTERPRETER)\"
else # Check OS
InterpFName:=
endif # Check OS


ifeq ($(M32), ON)
CFLAGS+= $(CFLAGS) -m32
LDFLAGS:= -melf_i386
static_lib_path:= static_lib_32bit/
InstallDir=/usr/lib
else
static_lib_path:= static_lib_64bit/
ifneq ($(wildcard "/usr/lib64"),)
InstallDir=/usr/lib64
endif
endif

define FLD_BUILD_RULE
@mkdir -p $(@)
endef

define LIB_BUILD_RULE
$(LD) $(LDFLAGS) -o $(@) -r $^
endef

define STATIC_BUILD_RULE
$(AR) $(ARFLAGS) $(@) $^
$(RANLIB) $(@)
endef


define SHARED_BUILD_RULE
$(CC) $(CFLAGS) $(1) -v -shared -L. -o $(@) -Wl,-soname,$(SoName),-Map,$(basename $<).map  $^ -lpthread -lrt
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
@rm -f $(1)/*.a
@rm -f $(1)/*
@rm -f $(1)/*.lst
@rm -f $(1)/*.map
@rm -f $(1)/*.exeendef
endef

define REMOVE_BIN_FILES
@rm -f $(1)
@rm -f $(basename $(1)).map
endef
#CFLAGS += -DWMI

# EAPI core
OBJECTS= \
      EApiAPI.o DbgChk.o \
      EApiInitLib.o \
      EApiOsNIX.o
      
# AAEON EAPI implement:libinit
OBJECTS += \
      libinit.o    
      

# AAEON EAPI implement:boardinfo
OBJECTS += \
      eapi_boardinfo_impl.o \
      boardinfo.o
                    
# AAEON EAPI implement:gpio
OBJECTS += \
      eapi_gpio_impl.o \
      gpio.o
      
# AAEON EAPI implement:core
OBJECTS += \
      aaeon_common.o \
#      aaeon_wmi.o \
#      bfpi_ext2.o
      
# AAEON EAPI implement:watchdog
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
      


define mark
# AAEON EAPI implement:storage, TODO: storage is not implemented for wmi
OBJECTS += \
      eapi_storage_impl.o \
      storage.o
      

# AAEON EAPI implement:smartfan
OBJECTS += \
      eapi_smartfan_impl.o \
      smartfan_ext2.o \
      smartfan.o




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
      $(DBinFolder)/$(StaticLibFName) \
      $(BinFolder)/$(StaticLibFName) \
      $(DBuildDir)/$(StaticLibFName) \
      $(BuildDir)/$(StaticLibFName) \
      $(DBinFolder)/$(SoFName) \
      $(BinFolder)/$(SoFName)
	@echo SYSTEM ARCHITECTURE : $(ARCH)
	@echo CPU ARCHITECTURE .. : $(MACH)
	@echo Library Name ...... : $(LibName)
	@echo Shared Lib Name ... : $(SoName)
	@echo Static Lib Filename : $(StaticLibFName)
	@echo Shared Lib Filename : $(SoFName)
	@echo Interpreter ....... : $(INTERPRETER)
	@echo Interface: $(interface)


$(BinFolder):
	$(FLD_BUILD_RULE)

$(BuildDir):
	$(FLD_BUILD_RULE)

$(DBinFolder):
	$(FLD_BUILD_RULE)

$(DBuildDir):
	$(FLD_BUILD_RULE)

$(DBuildDir)/$(LibFName): $(addprefix $(DBuildDir)/,$(OBJECTS))
	$(call LIB_BUILD_RULE,$(DBGOPT))

$(BuildDir)/$(LibFName): $(addprefix $(BuildDir)/,$(OBJECTS))
	$(call LIB_BUILD_RULE,$(RELOPT))

$(DBinFolder)/$(StaticLibFName): $(addprefix $(DBuildDir)/,$(LibFName))
	$(call STATIC_BUILD_RULE,$(DBGOPT))

$(BinFolder)/$(StaticLibFName): $(addprefix $(BuildDir)/,$(LibFName))
	$(call STATIC_BUILD_RULE,$(RELOPT))

$(DBuildDir)/$(StaticLibFName): $(addprefix $(DBuildDir)/,$(LibFName))
	$(call STATIC_BUILD_RULE,$(DBGOPT))

$(BuildDir)/$(StaticLibFName): $(addprefix $(BuildDir)/,$(LibFName))
	$(call STATIC_BUILD_RULE,$(RELOPT))

$(DBinFolder)/$(SoFName): $(addprefix $(DBuildDir)/,$(LibFName) $(InterpFName))
	$(call SHARED_BUILD_RULE,$(DBGOPT))

$(BinFolder)/$(SoFName): $(addprefix $(BuildDir)/,$(LibFName) $(InterpFName))
	$(call SHARED_BUILD_RULE,$(RELOPT))

$(DBuildDir)/%.o:$(OalFldr)/%.c
	$(call C_BUILD_RULE,$(DBGOPT))

$(BuildDir)/%.o:$(OalFldr)/%.c
	$(call C_BUILD_RULE,$(RELOPT))

$(DBuildDir)/%.o:$(PrgFldr)/%.c
	$(call C_BUILD_RULE,$(DBGOPT))

$(DBuildDir)/%.o:$(PrgFldr)/core/%.c
	$(call C_BUILD_RULE,$(DBGOPT))

$(DBuildDir)/%.o:$(PrgFldr)/common/%.c
	$(call C_BUILD_RULE,$(DBGOPT))

#$(DBuildDir)/%.o:$(PrgFldr)/$(ARCH_IF)/%.c
#	$(call C_BUILD_RULE,$(DBGOPT))

$(BuildDir)/%.o:$(PrgFldr)/%.c
	$(call C_BUILD_RULE,$(RELOPT))

$(BuildDir)/%.o:$(PrgFldr)/core/%.c
	$(call C_BUILD_RULE,$(RELOPT))

$(BuildDir)/%.o:$(PrgFldr)/common/%.c
	$(call C_BUILD_RULE,$(RELOPT))

#$(BuildDir)/%.o:$(PrgFldr)/$(ARCH_IF)/%.c
#	$(call C_BUILD_RULE,$(RELOPT))

$(DBuildDir)/%.o:$(PrgFldr)/%.cpp
	$(call C_BUILD_RULE,$(DBGOPT))

$(BuildDir)/%.o:$(PrgFldr)/%.cpp
	$(call C_BUILD_RULE,$(RELOPT))

$(InstallDir)/$(SoFName): $(BinFolder)/$(SoFName)
	cp --force $< $(@)
	chmod 755 $(@)
	ln --force --symbolic $(SoFName) $(dir $(@))$(SoName)
	ln --force --symbolic $(SoFName) $(dir $(@))$(SymLinkName)
	ldconfig -n $(dir $(@))$(SoName)

install: $(InstallDir)/$(SoFName)

uninstall::
	rm -rf $(InstallDir)/$(SoFName)
	rm -rf $(InstallDir)/$(SymLinkName)
	ldconfig	
	
.PHONY: clean
.PHONY: help

clean::
	$(call REMOVE_BIN_FILES,$(DBuildDir)/$(SoFName))
	$(call REMOVE_BIN_FILES,$(BuildDir)/$(SoFName))
	$(call CLEAN_BUILD_FOLDER,$(DBuildDir))
	$(call CLEAN_BUILD_FOLDER,$(BuildDir))
	$(call CLEAN_BUILD_FOLDER,$(DBinFolder))
	$(call CLEAN_BUILD_FOLDER,$(BinFolder))
	

help::
	@echo "###############################################################################"
	@echo "#        Build Mak Files"
	@echo "###############################################################################"
	@echo "# help"
	@echo "# clean"
	@echo "# quick"
	@echo "# buildall"
	@echo "# interface=wmi"
