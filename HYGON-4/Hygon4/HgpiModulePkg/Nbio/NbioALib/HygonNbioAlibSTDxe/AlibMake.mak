#/**
# * @file
# *
# * ALIB build make file
# *
# *
# *
# * @xrefitem bom "File Content Label" "Release Content"
# * @e project:     HGPI
# * @e sub-project: NBIO
# *
# */
#/*
#*****************************************************************************
#*
# * 
# * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
# * 
# * HYGON is granting you permission to use this software (the Materials)
# * pursuant to the terms and conditions of your Software License Agreement
# * with HYGON.  This header does *NOT* give you permission to use the Materials
# * or any rights under HYGON's intellectual property.  Your use of any portion
# * of these Materials shall constitute your acceptance of those terms and
# * conditions.  If you do not agree to the terms and conditions of the Software
# * License Agreement, please do not use any portion of these Materials.
# * 
# * CONFIDENTIALITY:  The Materials and all other information, identified as
# * confidential and provided to you by HYGON shall be kept confidential in
# * accordance with the terms and conditions of the Software License Agreement.
# * 
# * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
# * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
# * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
# * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
# * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
# * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
# * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
# * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
# * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
# * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
# * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
# * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
# * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
# * 
# * HYGON does not assume any responsibility for any errors which may appear in
# * the Materials or any other related information provided to you by HYGON, or
# * result from use of the Materials or any related information.
# * 
# * You agree that you will not reverse engineer or decompile the Materials.
# * 
# * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
# * further information, software, technical information, know-how, or show-how
# * available to you.  Additionally, HYGON retains the right to modify the
# * Materials at any time, without notice, and is not obligated to provide such
# * modified Materials to you.
# * 
# * 
#* ***************************************************************************
#*
#*/

# To build ALIB
# 1) Set HGPI_ROOT
# 2) Add path to MS asl compiler


.SUFFIXES : .esl .asl

#ALIB_DEBUG = 1
ALIB_TOOLS_DIR       = $(HGPI_ROOT)\HgpiModulePkg\HYGONTools\ALIB
ALIB_PROJECT_DIR     = $(HGPI_ROOT)\HgpiModulePkg\Nbio\NbioALib\HygonNbioAlibSTDxe
ALIB_PROJECT_ASL_DIR = $(ALIB_PROJECT_DIR)\ASL
ALIB_CORE_ASL_DIR    = $(ALIB_PROJECT_DIR)\ASL\Common
ALIB_BUILD_DIR       = $(ALIB_PROJECT_DIR)\BUILD
ALIB_C_FLAGS         = /FI$(ALIB_CORE_ASL_DIR)\AlibDefs.esl
ESL_TO_ASL_FLAGS     =
TARGET_ID=ST

!ifdef ALIB_DEBUG
ESL_TO_ASL_FLAGS     = $(ESL_TO_ASL_FLAGS) -debug
ALIB_C_FLAGS         = $(ALIB_C_FLAGS) /DDEBUG_OUT
!endif

ALIB_ASL_OBJ =\
$(ALIB_BUILD_DIR)\AlibST.asl \
$(ALIB_BUILD_DIR)\PcieTopologyST.asl \
$(ALIB_BUILD_DIR)\Alib.asl \
$(ALIB_BUILD_DIR)\DataBlock.asl \
$(ALIB_BUILD_DIR)\Pspp.asl \
$(ALIB_BUILD_DIR)\PciePort.asl \
$(ALIB_BUILD_DIR)\PciePortRegisters.asl \
$(ALIB_BUILD_DIR)\PcieRegistersST.asl \
$(ALIB_BUILD_DIR)\PciePortHotplug.asl \
$(ALIB_BUILD_DIR)\PcieEndpoint.asl \
$(ALIB_BUILD_DIR)\Hotplug.asl \
$(ALIB_BUILD_DIR)\AcDc.asl \
$(ALIB_BUILD_DIR)\NbioPcieST.asl \
$(ALIB_BUILD_DIR)\NbioST.asl \
$(ALIB_BUILD_DIR)\NbioDPTCiST.asl \
$(ALIB_BUILD_DIR)\SmnRegistersST.asl \

TARGET_ASL_OBJ = DUMMY

ALIB_STUB_ASL_OBJ =\
$(ALIB_BUILD_DIR)\AlibStub.asl \


all:  AlibSsdtAll

AlibSsdtAll:
        echo "Build AlibSsdt Image"
        $(MAKE) AlibSsdt$(TARGET_ID) -f $(ALIB_PROJECT_DIR)\AlibMake.mak \
                                TARGET_ESL_OBJ="$(ALIB_ASL_OBJ:asl=esl)" \
                                TARGET_ASL_OBJ="$(ALIB_ASL_OBJ)" \
                                TARGET_C_FLAGS="$(ALIB_C_FLAGS) /FI$(ALIB_PROJECT_ASL_DIR)\AlibConfigST.esl"
        $(MAKE) AlibSsdtStub -f $(ALIB_PROJECT_DIR)\AlibMake.mak \
                                TARGET_ESL_OBJ="$(ALIB_STUB_ASL_OBJ:asl=esl)" \
                                TARGET_ASL_OBJ="$(ALIB_STUB_ASL_OBJ)" \
                                TARGET_C_FLAGS="$(ALIB_C_FLAGS)"



AlibSsdt$(TARGET_ID): $(ALIB_PROJECT_DIR)\AlibSsdt$(TARGET_ID).h

AlibSsdtStub: $(ALIB_PROJECT_DIR)\AlibSsdtStub.h

#
# Build C include file
#
$(ALIB_PROJECT_DIR)\AlibSsdt$(TARGET_ID).h: $(ALIB_BUILD_DIR)\AlibSsdt$(TARGET_ID).aml
        perl $(ALIB_TOOLS_DIR)\Aml2C.pl --datatag AlibSsdt$(TARGET_ID) --filetag _ALIBSSDT$(TARGET_ID)_H_ --file $? > $@

$(ALIB_PROJECT_DIR)\AlibSsdtStub.h: $(ALIB_BUILD_DIR)\AlibSsdtStub.aml
        perl $(ALIB_TOOLS_DIR)\Aml2C.pl --datatag AlibSsdtStub --filetag _ALIBSSDTSTUB_H_ --file $? > $@

#
# Build aml file
#
$(ALIB_BUILD_DIR)\AlibSsdt$(TARGET_ID).aml: $(TARGET_ASL_OBJ)
        cd $(ALIB_BUILD_DIR)
        if EXIST PcieRegistersST.asl copy PcieRegistersST.asl PcieRegisters.asl
        asl.exe /Fo=$(ALIB_BUILD_DIR)\AlibSsdt$(TARGET_ID).aml $(ALIB_BUILD_DIR)\Alib$(TARGET_ID).asl

$(ALIB_BUILD_DIR)\AlibSsdtStub.aml: $(TARGET_ASL_OBJ)
        cd $(ALIB_BUILD_DIR)
        if EXIST PcieRegistersST.asl copy PcieRegistersST.asl PcieRegisters.asl
        asl.exe /Fo=$(ALIB_BUILD_DIR)\AlibSsdtStub.aml $(ALIB_BUILD_DIR)\AlibStub.asl

#
# Preprocess ESL files by ESL preprocessor
#
$(TARGET_ASL_OBJ):$(TARGET_ESL_OBJ)
        echo "Building asl"
        perl $(ALIB_TOOLS_DIR)\Esl2AslV2.pl $(ESL_TO_ASL_FLAGS) $(TARGET_ESL_OBJ)

#
# Preprocess core ESL files by C preprocessor
#
{$(ALIB_CORE_ASL_DIR)\}.esl{$(ALIB_BUILD_DIR)\}.esl:
        echo "Building core esl"
        @if not EXIST $(ALIB_BUILD_DIR) mkdir $(ALIB_BUILD_DIR)
        $(CC) $(TARGET_C_FLAGS) /C /EP $< > $*.esl
#
# Preprocess project ESL files by C preprocessor
#
{$(ALIB_PROJECT_ASL_DIR)\}.esl{$(ALIB_BUILD_DIR)\}.esl:
        echo "Building family esl"
        @if not EXIST $(ALIB_BUILD_DIR) mkdir $(ALIB_BUILD_DIR)
        $(CC) $(TARGET_C_FLAGS) /C /EP $< > $*.esl

clean:
        rmdir /S /Q $(ALIB_BUILD_DIR)
