## @file
# EFI/PI Reference Module Package for All Architectures
#
# Copyright (c) 2006 - 2022, Byosoft Corporation.<BR>
# All rights reserved.This software and associated documentation (if any)
# is furnished under a license and may only be used or copied in
# accordance with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be reproduced,
# stored in a retrieval system, or transmitted in any form or by any
# means without the express written consent of Byosoft Corporation.
#
#	File Name:
#  	ByoModule.fdf.inc
#
#	Abstract:
#  	ByoSoft Core Module Package include File
#
##


[Rule.Common.DXE_DRIVER.BINARY]
  FILE DRIVER = $(NAMED_GUID) {
    DXE_DEPEX DXE_DEPEX Optional      |.depex
    PE32      PE32                    |.efi
    UI        STRING="$(MODULE_NAME)" Optional
    VERSION   STRING="$(INF_VERSION)" Optional BUILD_NUM=$(BUILD_NUMBER)
  }


#
# Instant Debugger Feature Support 
#
   [Rule.Common.DXE_DRIVER.BINARY]
    FILE DRIVER = $(NAMED_GUID) {
    DXE_DEPEX DXE_DEPEX Optional      |.depex
    PE32      PE32                    |.efi
    UI        STRING="$(MODULE_NAME)" Optional
    VERSION   STRING="$(INF_VERSION)" Optional BUILD_NUM=$(BUILD_NUMBER)
    }

   [Rule.Common.PEIM.BINARY]
    FILE PEIM = $(NAMED_GUID) {
    PEI_DEPEX PEI_DEPEX Optional      |.depex
    PE32      PE32     Align = Auto   |.efi
    UI        STRING="$(MODULE_NAME)" Optional
    VERSION   STRING="$(INF_VERSION)" Optional BUILD_NUM=$(BUILD_NUMBER)
    }

   [Rule.Common.DXE_SMM_DRIVER.BINARY]
    FILE SMM = $(NAMED_GUID) {
    SMM_DEPEX SMM_DEPEX Optional      |.depex
    PE32      PE32                    |.efi
    UI        STRING="$(MODULE_NAME)" Optional
    VERSION   STRING="$(INF_VERSION)" Optional BUILD_NUM=$(BUILD_NUMBER)
    }


[Rule.Common.UEFI_APPLICATION.UI]
  FILE APPLICATION = $(NAMED_GUID) {
    PE32      PE32                     $(INF_OUTPUT)/$(MODULE_NAME).efi
    UI        STRING="Enter Setup"
    VERSION   STRING="$(INF_VERSION)" Optional BUILD_NUM=$(BUILD_NUMBER)
  }
