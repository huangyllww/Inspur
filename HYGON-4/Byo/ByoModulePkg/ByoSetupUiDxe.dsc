## @file
# EFI/PI Reference Module Package for All Architectures
#
# Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
# All rights reserved.This software and associated documentation (if any)
# is furnished under a license and may only be used or copied in
# accordance with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be reproduced,
# stored in a retrieval system, or transmitted in any form or by any
# means without the express written consent of Byosoft Corporation.
#
# File Name:
#   ByoSetupUiDxe.dsc
#
[Defines]
#
#Instant Debugger Feature Support 
#
  DEFINE  INSTANT_DEBUG_ENABLE        = TRUE

!if $(TARGET) == RELEASE
  DEFINE  INSTANT_DEBUG_ENABLE        = FALSE
!endif
[PcdsPatchableInModule.common]

# Change PcdBootManagerMenuFile to UiApp
gEfiMdeModulePkgTokenSpaceGuid.PcdBootManagerMenuFile|{ 0x4a, 0xc0, 0xc9, 0xd0, 0x96, 0xe7, 0x48, 0x4c, 0x9d, 0x52, 0x9e, 0x0c, 0xed, 0x39, 0x4f, 0x4b } 

[PcdsFixedAtBuild]
gEfiByoModulePkgTokenSpaceGuid.PcdTextSetupMouseEnable|TRUE

[PcdsFixedAtBuild.X64]

[PcdsFeatureFlag]

[PcdsDynamicDefault]

[LibraryClasses]

  CustomizedDisplayLib|ByoModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf
  FileExplorerLib|ByoModulePkg/Library/FileExplorerLib/FileExplorerLib.inf

[LibraryClasses.IA32]

[LibraryClasses.X64]

[LibraryClasses.X64.DXE_CORE]
#
#Instant Debugger Feature Support 
#    
!if $(INSTANT_DEBUG_ENABLE) == TRUE
  DebugAgentLib|ByoInstantDebugPkg/Library/InstantDebuggerLib/DxeInstantDebuggerLib.inf
  PeCoffExtraActionLib|ByoInstantDebugPkg/Library/PeCoffExtraActionLibDebug/PeCoffExtraActionLibDebug.inf
!endif
[Components.IA32]

[Components.X64, Components.AARCH64, Components.RISCV64]
  #
  # BYO UI
  #
  ByoModulePkg/Setup/DisplayEngineDxe/DisplayEngineDxe.inf 
  ByoModulePkg/Setup/SetupBrowserDxe/SetupBrowserDxe.inf  
  MdeModulePkg/Library/BootManagerUiLib/BootManagerUiLib.inf {
    <BuildOptions>
    *_*_*_VFR_FLAGS = -g d6fc2e65-bd6c-4b79-9ffc-0d15364e0773
  }
  ByoModulePkg/Application/ByoUiApp/ByoUiApp.inf {
    <LibraryClasses>
    PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf     
    NULL|MdeModulePkg/Library/BootManagerUiLib/BootManagerUiLib.inf
    NULL|MdeModulePkg/Library/BootMaintenanceManagerUiLib/BootMaintenanceManagerUiLib.inf
  }
  #ByoModulePkg/Setup/PlatformSetupDxe/PlatformSetupDxe.inf
  ByoModulePkg/Setup/PostLogin/PostLogin.inf
  ByoModulePkg/Setup/SystemPassword/SystemPassword.inf
  ByoModulePkg/Setup/UnicodeFontDxe/UnicodeFontDxe.inf
  ByoModulePkg/Setup/SetupMouse/SetupMouse.inf

  MdeModulePkg/Library/BootMaintenanceManagerUiLib/BootMaintenanceManagerUiLib.inf {
    # Boot Page Class Guid
    <BuildOptions>
    *_*_*_VFR_FLAGS = -g d6fc2e65-bd6c-4b79-9ffc-0d15364e0773
  }  
  MdeModulePkg/Universal/FileExplorerDxe/FileExplorerDxe.inf {
    <LibraryClasses>
      FileExplorerLib|ByoModulePkg/Library/FileExplorerLib/FileExplorerLib.inf
  }
  ByoModulePkg/Setup/DisplayBackupRecoveryDxe/DisplayBackupRecoveryDxe.inf
[BuildOptions]