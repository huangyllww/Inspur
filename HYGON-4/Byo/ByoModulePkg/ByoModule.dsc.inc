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
#  	ByoModule.dsc.inc
#
#	Abstract:
#  	ByoSoft Core Module Package include File
#
##

[PcdsFixedAtBuild]
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemId|"_BYO_ "
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemTableId|0x2054464F534F5942  # "BYOSOFT "
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorId|0x204F5942           # "BYO "
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVendor|L"Byosoft"
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserSubtitleTextColor|0x0
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserFieldTextColor|0x01
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserFieldTextHighlightColor|0x0F
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserFieldBackgroundHighlightColor|0x70
  # For Chinese-English shift
  gEfiMdePkgTokenSpaceGuid.PcdUefiVariableDefaultPlatformLangCodes|"en-US;zh-Hans"

[PcdsFeatureFlag]
  gEfiMdePkgTokenSpaceGuid.PcdUefiVariableDefaultLangDeprecate|TRUE

[PcdsDynamicDefault.common.DEFAULT]
  gEfiByoModulePkgTokenSpaceGuid.PcdBiosIdVersionCheck | FALSE 

[LibraryClasses]
  ByoBiosSignInfoLib|ByoModulePkg/Library/ByoBiosSignInfoLib/ByoBiosSignInfoLib.inf
  ByoBootManagerLib|ByoModulePkg/Library/ByoBootManagerLib/ByoBootManagerLib.inf
  TMIoLib|ByoModulePkg/Library/MmioTMLib/MmioTMLib.inf
  SetupUiLib|ByoModulePkg/Library/SetupUiLib/SetupUiLib.inf
  PlatformLanguageLib|ByoModulePkg/Library/PlatformLanguageLib/PlatformLanguageLib.inf
  ByoCommLib|ByoModulePkg/Library/ByoCommLib/ByoCommLib.inf
  SystemPasswordLib|ByoModulePkg/Library/SystemPasswordLib/SystemPasswordLib.inf
  SetMemAttributeSmmLib|ByoModulePkg/Library/SetMemAttributeSmmLibNull/SetMemAttributeSmmLib.inf
  BiosIdLib|ByoModulePkg/Library/BiosIdLib/BiosIdLib.inf
  ByoRtcLib|ByoModulePkg/Library/ByoRtcLib/ByoRtcLib.inf
  UsbListLib|ByoModulePkg/Library/UsbListLib/UsbListLib.inf

[LibraryClasses.common.SEC, LibraryClasses.common.PEI_CORE, LibraryClasses.common.PEIM] 
!if $(TARGET) == DEBUG
  DebugCallStackLib|ByoInstantDebugPkg/Library/DebugCallStackLib/PeiDebugCallStackLib.inf
!endif

  BiosIdLib|ByoModulePkg/Library/BiosIdLib/PeiBiosIdLib.inf

[LibraryClasses.X64.PEIM] 
!if $(TARGET) == DEBUG
  DebugCallStackLib|MdePkg/Library/BaseDebugCallStackLibNull/BaseDebugCallStackLibNull.inf 
!endif

[LibraryClasses.common.DXE_CORE, LibraryClasses.common.DXE_DRIVER, LibraryClasses.common.DXE_RUNTIME_DRIVER, LibraryClasses.common.SMM_CORE, LibraryClasses.common.DXE_SMM_DRIVER, LibraryClasses.common.UEFI_DRIVER, LibraryClasses.common.UEFI_APPLICATION] 
!if $(TARGET) == DEBUG
  DebugCallStackLib|ByoInstantDebugPkg/Library/DebugCallStackLib/DxeDebugCallStackLib.inf
!endif

[PcdsDynamicHii.common.DEFAULT]
  gEfiByoModulePkgTokenSpaceGuid.PcdPlatformDefaultKey|L"SecureBootDefaultKey"|gEfiSecureBootDefaultKey|0x0|0 # Variable: L"SecureBootDefaultKey"

[BuildOptions]
  MSFT:*_*_*_NASM_PATH        = $(WORKSPACE)/ByoTools/Bin/nasm.exe
  GCC:RELEASE_*_*_GENFW_FLAGS = -z
