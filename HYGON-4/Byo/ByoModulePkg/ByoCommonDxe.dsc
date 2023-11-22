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
#   ByoCommonDxe.dsc
#

[PcdsPatchableInModule.common]

[PcdsFixedAtBuild]

[PcdsFixedAtBuild.X64]

[PcdsFeatureFlag]

[PcdsDynamicDefault]

#TPM2.0
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInstanceGuid|{0x5a, 0xf2, 0x6b, 0x28, 0xc3, 0xc2, 0x8c, 0x40, 0xb3, 0xb4, 0x25, 0xe6, 0x75, 0x8b, 0x73, 0x17}
!if $(TPM2_ENABLE) == TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2HierarchyChangeAuthPlatform|TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2ChangeEps|FALSE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2ChangePps|FALSE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2Clear|FALSE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2HierarchyControlPlatform|TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2HierarchyControlEndorsement|TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2HierarchyControlOwner|TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2ChipPresent|TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2TxtProvisionSupport|TRUE
!endif
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2InitializationPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2SelfTestPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2ScrtmPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInitializationPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmScrtmPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2HashMask|0x1f
  gEfiSecurityPkgTokenSpaceGuid.PcdTcg2HashAlgorithmBitmap|0x1f

[LibraryClasses]
  TcgStorageCoreLib|SecurityPkg/Library/TcgStorageCoreLib/TcgStorageCoreLib.inf
  TcgStorageOpalLib|SecurityPkg/Library/TcgStorageOpalLib/TcgStorageOpalLib.inf
  BootLogoLib|MdeModulePkg/Library/BootLogoLib/BootLogoLib.inf
  SetMemAttributeSmmLib|ByoModulePkg/Library/SetMemAttributeSmmLibNull/SetMemAttributeSmmLib.inf
  # Secure boot
  SecureBootRestoreLib|ByoModulePkg/Library/SecureBootRestoreLib/SecureBootRestoreLib.inf
  FileExplorerLib|ByoModulePkg/Library/FileExplorerLib/FileExplorerLib.inf

[LibraryClasses.IA32]

[LibraryClasses.X64]

  BmpSupportLib|MdeModulePkg/Library/BaseBmpSupportLib/BaseBmpSupportLib.inf

#TPM2 support
#
!if $(TPM2_ENABLE) == TRUE
  TpmPlatformHierarchyLib|ByoModulePkg/Library/TpmPlatformHierarchyLib/TpmPlatformHierarchyLib.inf
!endif
[LibraryClasses.X64.DXE_SMM_DRIVER]
SystemPasswordLib|ByoModulePkg/Library/SystemPasswordLib/SystemPasswordLibSmm.inf

[Components.IA32]

[Components.X64]

  ByoModulePkg/Logo/LogoDxe.inf
  ByoModulePkg/Universal/PlatformBootManagerDxe/PlatformBootManagerDxeEdk2.inf
  ByoModulePkg/Setup/BiosUpdate/BiosUpdate.inf
  ByoModulePkg/HddBindDxe/HddBindDxeOnlyUefi.inf {
     <BuildOptions>
      # Security page class Guid gIfrByoSecurityUiPageGuid
      *_*_*_VFR_FLAGS = -g D7EDC7D9-5A9A-43DD-BE2B-C48FE840B4A7
   }
  ByoModulePkg/Application/PxeBootApp/PxeBootApp.inf{
    <LibraryClasses>
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
  ByoModulePkg/Nvme/NvmeDxe/NvmExpressDxe.inf
  ByoModulePkg/Security/HddPassword/Dxe/HddPasswordDxe.inf
  ByoModulePkg/Setup/PlatformSetupDxe/PlatformSetupDxe.inf

!if $(SECURE_BOOT_ENABLE) == TRUE
  ByoModulePkg/Security/SecureBootConfigDxe/SecureBootConfigDxe.inf {
    <LibraryClasses>
      BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
      PlatformSecureLib|ByoModulePkg/Library/PlatformSecureLib/PlatformSecureLib.inf
    <BuildOptions>
      # security page class guid
      <BuildOptions>
      *_*_*_VFR_FLAGS = -g D7EDC7D9-5A9A-43DD-BE2B-C48FE840B4A7
  }
  ByoModulePkg/Security/SecureBoot/SecureBootDefult.inf {
    <LibraryClasses>
      BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
      PlatformSecureLib|ByoModulePkg/Library/PlatformSecureLib/PlatformSecureLib.inf
  }
!endif

  #
  #Platform BIOS Update Support
  #
  ByoModulePkg/Universal/ByoSmbiosTable/ByoSmbiosTable.inf
  ByoModulePkg/SmiFlash/SmiFlash.inf {
    <LibraryClasses>
    BiosIdLib|ByoModulePkg/Library/BiosIdLib/BiosIdLib.inf
  }
  ByoModulePkg/SmiFlashDxe/SmiFlashDxe.inf {
    <LibraryClasses>
    BiosIdLib|ByoModulePkg/Library/BiosIdLib/BiosIdLib.inf
  }


  # Support ByoDmi
  ByoModulePkg/Universal/SmbiosSmm/SmbiosSmm.inf
  ByoModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
#
#TPM2 support
#
!if $(TPM2_ENABLE) == TRUE
  ByoModulePkg/Security/Tpm/Tpm2Setup/Tcg2ConfigDxe.inf {
    <LibraryClasses>
      Tpm2CommandLib|SecurityPkg/Library/Tpm2CommandLib/Tpm2CommandLib.inf
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibTcg2/Tpm2DeviceLibTcg2.inf
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
    <BuildOptions>
      # Security page class Guid gIfrByoSecurityUiPageGuid
      *_*_*_VFR_FLAGS = -g D7EDC7D9-5A9A-43DD-BE2B-C48FE840B4A7
  }

  ByoModulePkg/Security/Tpm/Tcg2PlatformDxe/Tcg2PlatformDxe.inf {
    <LibraryClasses>
      Tpm2CommandLib|SecurityPkg/Library/Tpm2CommandLib/Tpm2CommandLib.inf
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibTcg2/Tpm2DeviceLibTcg2.inf
      TpmPlatformHierarchyLib|ByoModulePkg/Library/TpmPlatformHierarchyLib/TpmPlatformHierarchyLib.inf
  }
!endif
#For ipmi support
  !if $(IPMI_SUPPORT) == TRUE
    ByoServerPkg/Ipmi/IpmiBootOrder/IpmiBootOrderUefi.inf{
      <LibraryClasses>
        IpmiBaseLib|ByoServerPkg/Library/IpmiBaseLib/IpmiBaseLib.inf
    }
    ByoServerPkg/Ipmi/Generic/GenericIpmi.inf {
      <LibraryClasses>
        IpmiBaseLib|ByoServerPkg/Library/IpmiBaseLib/IpmiBaseLib.inf
    }
    ByoServerPkg/Ipmi/BmcSol/SolStatus.inf{
      <LibraryClasses>
        IpmiBaseLib|ByoServerPkg/Library/IpmiBaseLib/IpmiBaseLib.inf
    } 
    ByoServerPkg/Ipmi/IpmiRedirFru/IpmiRedirFru.inf{
      <LibraryClasses>
        IpmiBaseLib|ByoServerPkg/Library/IpmiBaseLib/IpmiBaseLib.inf
    } 
    ByoServerPkg/Ipmi/BmcWdt/BmcWdt.inf{
      <LibraryClasses>
        IpmiBaseLib|ByoServerPkg/Library/IpmiBaseLib/IpmiBaseLib.inf
    } 
    ByoServerPkg/Ipmi/BmcCfg/BmcCfg.inf{
      <LibraryClasses>
        IpmiBaseLib|ByoServerPkg/Library/IpmiBaseLib/IpmiBaseLib.inf
    } 
    ByoServerPkg/Ipmi/SetupBmcCfg/SetupBmcCfg.inf{
      <LibraryClasses>
        IpmiBaseLib|ByoServerPkg/Library/IpmiBaseLib/IpmiBaseLib.inf
    } 
  !endif
[BuildOptions]
