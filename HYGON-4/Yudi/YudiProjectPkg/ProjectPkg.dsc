# @file
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
#   ProjectPkg.dsc
#
# Abstract:
#   Platform Configuration File
#
# Revision History:
#
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                       = $(PROJECT_PKG)
  PLATFORM_GUID                       = 0D28678E-EA14-46cd-9F43-01B48B7B48CD
  PLATFORM_VERSION                    = 0.1
  DSC_SPECIFICATION                   = 0x00010005
  OUTPUT_DIRECTORY                    = Build/$(PROJECT_PKG)
  SUPPORTED_ARCHITECTURES             = IA32|X64
  BUILD_TARGETS                       = DEBUG|RELEASE
  SKUID_IDENTIFIER                    = DEFAULT
  FLASH_DEFINITION                    = $(PROJECT_PKG)/ProjectPkg.fdf


!ifndef INSTANT_DEBUG_ENABLE
DEFINE INSTANT_DEBUG_ENABLE = FALSE
!endif

DEFINE PLATFORM_PACKAGE   = Platform4Pkg
DEFINE PERFORMANCE_ENABLE = TRUE
DEFINE BEEP_STATUS_CODE_ENABLE = TRUE
DEFINE TPM_ENABLE = TRUE
DEFINE LEGACY_TPM_SUPPORT = TRUE
DEFINE TCM_ENABLE = TRUE
DEFINE SECURE_BOOT_ENABLE    = TRUE
DEFINE RECOVERY_ENABLE       = FALSE
DEFINE RECOVERY_CDROM_ENABLE = TRUE
DEFINE HTTP_BOOT_ENABLE      = FALSE
DEFINE NVME_SUPPORT          = TRUE
DEFINE LEGACY_NVME_SUPPORT   = TRUE
DEFINE IPMI_SUPPORT          = TRUE

DEFINE SECURE_KEY_PATH       = ByoModulePkg/Security/SecureKey
DEFINE TARGET_SVID_SUPPORT   = FALSE
DEFINE TARGET_SVID           = 5678
DEFINE TARGET_SSID           = 1234
DEFINE BOARD_SVID            = 0x$(TARGET_SVID)
DEFINE BOARD_SSID            = 0x$(TARGET_SSID)
DEFINE BOARD_SSID_SVID       = 0x$(TARGET_SSID)$(TARGET_SVID)
DEFINE DEBUG_MESSAGE_ENABLE  = TRUE

DEFINE REDFISH_ENABLE                 = TRUE
DEFINE NETWORK_STACK_ENABLE           = TRUE
DEFINE NETWORK_TLS_ENABLE             = FALSE
DEFINE NETWORK_HTTP_BOOT_ENABLE       = TRUE
DEFINE NETWORK_ISCSI_ENABLE           = FALSE
DEFINE NETWORK_VLAN_ENABLE            = FALSE
DEFINE NETWORK_HIDE_IP4_HII           = TRUE
DEFINE NETWORK_HIDE_IP6_HII           = TRUE
DEFINE PLATFORMX64_ENABLE             = TRUE
DEFINE NETWORK_ALLOW_HTTP_CONNECTIONS = TRUE
DEFINE NO_SHELL_PROFILES              = FALSE
DEFINE OPENSSL_LIB_USE_BIN            = TRUE
DEFINE EARLY_GOP_SUPPORT              = TRUE

!if $(TKN_BOARD_ID_TYPE) == 0
DEFINE UART_SELECTION = 0
!elseif $(TKN_BOARD_ID_TYPE) == 1
DEFINE UART_SELECTION = 0
!elseif $(TKN_BOARD_ID_TYPE) == 2
DEFINE UART_SELECTION = 0
!else
DEFINE UART_SELECTION = 0
!endif


################################################################################
#
# SKU Identification section - list of all SKU IDs supported by this
#                              Platform.
#
################################################################################
[SkuIds]
  0|DEFAULT              # The entry: 0|DEFAULT is reserved and always required.


[PcdsFixedAtBuild]
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiS3Enable|FALSE
  gPlatformModuleTokenSpaceGuid.PcdVga07501BD4OptionRomSupport|TRUE

!include MdePkg/MdeLibs.dsc.inc
!include ByoModulePkg/ByoModule.dsc.inc
!include HgpiModulePkg/HgpiModulePkg.inc.dsc
!include HgpiCbsPkg/Library/HgpiCbs.inc.dsc
!include HgpiCpmPkg/Addendum/Oem/HygonCpmCommPkg.inc.dsc
!include ByoHgpiModulePkg/ByoHgpiModulePkg.inc.dsc
!include $(PLATFORM_PACKAGE)/PlatformPkg.inc.dsc


################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses.common]

# Entry point
  PeiCoreEntryPoint|MdePkg/Library/PeiCoreEntryPoint/PeiCoreEntryPoint.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  DxeCoreEntryPoint|MdePkg/Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf

!if $(INSTANT_DEBUG_ENABLE) == TRUE
  PeCoffExtraActionLib|ByoInstantDebugPkg/Library/PeCoffExtraActionLibDebug/PeCoffExtraActionLibDebug.inf
!else
  PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
!endif
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf

# Basic
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  PciLib|MdePkg/Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
  PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
  CacheMaintenanceLib|MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  PciSegmentLib|MdePkg/Library/BasePciSegmentLibPci/BasePciSegmentLibPci.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  BmpSupportLib|MdeModulePkg/Library/BaseBmpSupportLib/BaseBmpSupportLib.inf
  SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
  RngLib|MdePkg/Library/BaseRngLibTimerLib/BaseRngLibTimerLib.inf
  OrderedCollectionLib|MdePkg/Library/BaseOrderedCollectionRedBlackTreeLib/BaseOrderedCollectionRedBlackTreeLib.inf

# UEFI & PI
  MmServicesTableLib|MdePkg/Library/MmServicesTableLib/MmServicesTableLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLibIdt/PeiServicesTablePointerLibIdt.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  UefiCpuLib|UefiCpuPkg/Library/BaseUefiCpuLib/BaseUefiCpuLib.inf
  S3BootScriptLib|MdeModulePkg/Library/PiDxeS3BootScriptLib/DxeS3BootScriptLib.inf
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  UefiScsiLib|MdePkg/Library/UefiScsiLib/UefiScsiLib.inf

  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf

  BootLogoLib|MdeModulePkg/Library/BootLogoLib/BootLogoLib.inf
  UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  ByoUefiBootManagerLib|ByoModulePkg/Library/ByoUefiBootManagerLib/ByoUefiBootManagerLib.inf
  LegacyBootManagerLib|ByoLegacyPkg/Library/LegacyBootManagerLib/LegacyBootManagerLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
  SmmCorePlatformHookLib|MdeModulePkg/Library/SmmCorePlatformHookLibNull/SmmCorePlatformHookLibNull.inf
  IoApicLib|PcAtChipsetPkg/Library/BaseIoApicLib/BaseIoApicLib.inf

  LegacyBootOptionalDataLib|ByoLegacyPkg/Library/LegacyBootOptionalDataLib/LegacyBootOptionalDataLib.inf
  ByoRtcLib|ByoModulePkg/Library/ByoRtcLib/ByoRtcLib.inf
  BiosIdLib|ByoModulePkg/Library/BiosIdLib/BiosIdLib.inf
 
  ByoBootManagerLib|ByoModulePkg/Library/ByoBootManagerLib/ByoBootManagerLib.inf
  PlatformBootManagerLib|$(PROJECT_PKG)/Override/Library/PlatformBootManagerLib/PlatformBootManagerLib.inf

# CPU
  MtrrLib|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
  LocalApicLib|UefiCpuPkg/Library/BaseXApicX2ApicLib/BaseXApicX2ApicLib.inf
  SmmCpuFeaturesLib|UefiCpuPkg/Library/SmmCpuFeaturesLib/SmmCpuFeaturesLib.inf
  SmmCpuPlatformHookLib|UefiCpuPkg/Library/SmmCpuPlatformHookLibNull/SmmCpuPlatformHookLibNull.inf
  VmgExitLib|UefiCpuPkg/Library/VmgExitLibNull/VmgExitLibNull.inf


# Platform
  SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
  FileExplorerLib|ByoModulePkg/Library/FileExplorerLib/FileExplorerLib.inf
  SecureBootRestoreLib|ByoModulePkg/Library/SecureBootRestoreLib/SecureBootRestoreLib.inf
  PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
  SmbusLib|MdePkg/Library/BaseSmbusLibNull/BaseSmbusLibNull.inf
  ByoCommLib|ByoModulePkg/Library/ByoCommLib/ByoCommLib.inf
  ByoAzaliaLib|ByoModulePkg/Library/ByoAzaliaLib/ByoAzaliaLib.inf
  ByoKbcLib|ByoModulePkg/Library/ByoKbcLib/ByoKbcLib.inf
  SafeMemoryLib|ByoModulePkg/Library/ByoSafeMemoryLib/ByoSafeMemoryLib.inf
  SafePrintLib|ByoModulePkg/Library/ByoSafePrintLib/ByoSafePrintLib.inf

# Misc
!if $(DEBUG_MESSAGE_ENABLE) == FALSE
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
!else
  DebugLib|MdeModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
!endif
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf
  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf

!if $(OPENSSL_LIB_USE_BIN) == TRUE
  OpensslLib|Platform4Pkg/lib/OpensslLib.inf
!else  
  OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
!endif

  TcgStorageCoreLib|SecurityPkg/Library/TcgStorageCoreLib/TcgStorageCoreLib.inf
  TcgStorageOpalLib|SecurityPkg/Library/TcgStorageOpalLib/TcgStorageOpalLib.inf
  CpuExceptionHandlerLib|MdeModulePkg/Library/CpuExceptionHandlerLibNull/CpuExceptionHandlerLibNull.inf
  CustomizedDisplayLib|ByoModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf
  PlatformLanguageLib|ByoModulePkg/Library/PlatformLanguageLib/PlatformLanguageLib.inf
  Tcg2PhysicalPresenceLib|SecurityPkg/Library/DxeTcg2PhysicalPresenceLib/DxeTcg2PhysicalPresenceLib.inf
  TcgPhysicalPresenceLib|SecurityPkg/Library/DxeTcgPhysicalPresenceLib/DxeTcgPhysicalPresenceLib.inf
  Tpm2CommandLib|SecurityPkg/Library/Tpm2CommandLib/Tpm2CommandLib.inf
  Tcg2PpVendorLib|SecurityPkg/Library/Tcg2PpVendorLibNull/Tcg2PpVendorLibNull.inf
  TcgPpVendorLib|SecurityPkg/Library/TcgPpVendorLibNull/TcgPpVendorLibNull.inf
  TpmMeasurementLib|SecurityPkg/Library/DxeTpmMeasurementLib/DxeTpmMeasurementLib.inf
  SystemPasswordLib|ByoModulePkg/Library/SystemPasswordLib/SystemPasswordLib.inf

  StatusCodeBeepLib|ByoModulePkg/Library/StatusCodeBeepLib/StatusCodeBeepLib.inf
  PostCodeMapLib|ByoModulePkg/Library/PostCodeMapLib/PostCodeMapLib.inf
  IpmiBaseLib|ByoServerPkg/Library/IpmiBaseLib/IpmiBaseLib.inf

!if $(IPMI_SUPPORT) == TRUE
  EfiServerManagementLib|ByoServerPkg/Library/ServerManagementLib/ServerManagementLib.inf
  PostErrorTableLib|ByoServerPkg/Library/PostErrorTableLib/PostErrorTableLib.inf
!endif

  VariablePolicyLib|MdeModulePkg/Library/VariablePolicyLib/VariablePolicyLib.inf
  VariablePolicyHelperLib|MdeModulePkg/Library/VariablePolicyHelperLib/VariablePolicyHelperLib.inf

!if $(TCM_ENABLE) == TRUE
  TcmCommLib|ByoModulePkg/Security/Tcm/TcmCommLib/TcmCommLib.inf
!endif

  TlsLib|CryptoPkg/Library/TlsLib/TlsLib.inf

!if $(TPM_ENABLE) == TRUE
  TpmPlatformHierarchyLib|ByoModulePkg/Library/TpmPlatformHierarchyLib/TpmPlatformHierarchyLib.inf
!endif

  HstiLib|MdePkg/Library/DxeHstiLib/DxeHstiLib.inf
  NetLib|NetworkPkg/Library/DxeNetLib/DxeNetLib.inf

  ProjectCommonLib|$(PROJECT_PKG)/Library/ProjectCommonLib/ProjectCommonLib.inf 

[LibraryClasses.common.PEIM]
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/PeiExtractGuidedSectionLib/PeiExtractGuidedSectionLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/PeiCryptLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxPeiLib.inf
  MultiPlatSupportLib|ByoModulePkg/Library/PeiMultiPlatSupportLib/PeiMultiPlatSupportLib.inf
  MpInitLib|UefiCpuPkg/Library/MpInitLib/PeiMpInitLib.inf
  Tcg2PhysicalPresenceLib|SecurityPkg/Library/PeiTcg2PhysicalPresenceLib/PeiTcg2PhysicalPresenceLib.inf
  Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2DeviceLibDTpm.inf

!if $(PERFORMANCE_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/PeiPerformanceLib/PeiPerformanceLib.inf
!endif
  IpmiBaseLib|ByoServerPkg/Library/PeiIpmiBaseLib/PeiIpmiBaseLib.inf




[LibraryClasses.IA32.PEI_CORE]
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
!if $(PERFORMANCE_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/PeiPerformanceLib/PeiPerformanceLib.inf
!endif
!if $(INSTANT_DEBUG_ENABLE) == TRUE
  DebugAgentLib|ByoInstantDebugPkg/Library/InstantDebuggerLib/PeiInstantDebuggerLib.inf
!endif

[LibraryClasses.X64]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxDxeLib.inf
  VarCheckLib|MdeModulePkg/Library/VarCheckLib/VarCheckLib.inf
  MpInitLib|UefiCpuPkg/Library/MpInitLib/DxeMpInitLib.inf
  PciHostBridgeLib|MdeModulePkg/Library/PciHostBridgeLibNull/PciHostBridgeLibNull.inf
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/DxeCpuExceptionHandlerLib.inf

!if $(SECURE_BOOT_ENABLE) == TRUE
  PlatformSecureLib|ByoModulePkg/Library/PlatformSecureLib/PlatformSecureLib.inf
  AuthVariableLib|SecurityPkg/Library/AuthVariableLib/AuthVariableLib.inf
!else
  AuthVariableLib|MdeModulePkg/Library/AuthVariableLibNull/AuthVariableLibNull.inf
!endif

  SetupUiLib|ByoModulePkg/Library/SetupUiLib/SetupUiLib.inf
  Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibTcg2/Tpm2DeviceLibTcg2.inf
  FmpAuthenticationLib|SecurityPkg/Library/FmpAuthenticationLibPkcs7/FmpAuthenticationLibPkcs7.inf

!if $(INSTANT_DEBUG_ENABLE) == TRUE
  DebugAgentLib|ByoInstantDebugPkg/Library/InstantDebuggerLib/DxeInstantDebuggerLib.inf
!endif





[LibraryClasses.common.DXE_DRIVER]
!if $(PERFORMANCE_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
!endif


[LibraryClasses.X64.DXE_CORE]
  HobLib|MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
!if $(PERFORMANCE_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf
!endif


[LibraryClasses.X64.DXE_SMM_DRIVER]
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
  MemoryAllocationLib|MdePkg/Library/SmmMemoryAllocationLib/SmmMemoryAllocationLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxSmmLib.inf
  SetMemAttributeSmmLib|ByoModulePkg/Library/SetMemAttributeSmmLibNull/SetMemAttributeSmmLib.inf
  SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/SmmCpuExceptionHandlerLib.inf
  Tcg2PhysicalPresenceLib|SecurityPkg/Library/SmmTcg2PhysicalPresenceLib/SmmTcg2PhysicalPresenceLib.inf
  VariableServiceHookLib|$(PROJECT_PKG)/Library/VariableServiceHookLib/VariableServiceHookLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/SmmCryptLib.inf
  IpmiBaseLib|ByoServerPkg/Library/SmmIpmiBaseLib/SmmIpmiBaseLib.inf

!if $(PERFORMANCE_ENABLE) == TRUE  
  PerformanceLib|MdeModulePkg/Library/SmmPerformanceLib/SmmPerformanceLib.inf
!endif  







[LibraryClasses.X64.SMM_CORE]
  MemoryAllocationLib|MdeModulePkg/Library/PiSmmCoreMemoryAllocationLib/PiSmmCoreMemoryAllocationLib.inf
  SmmServicesTableLib|MdeModulePkg/Library/PiSmmCoreSmmServicesTableLib/PiSmmCoreSmmServicesTableLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
  SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/SmmCryptLib.inf
!if $(PERFORMANCE_ENABLE) == TRUE  
  PerformanceLib|MdeModulePkg/Library/SmmCorePerformanceLib/SmmCorePerformanceLib.inf
!endif  



[LibraryClasses.X64.DXE_RUNTIME_DRIVER]
  ReportStatusCodeLib|MdeModulePkg/Library/RuntimeDxeReportStatusCodeLib/RuntimeDxeReportStatusCodeLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/RuntimeCryptLib.inf
  VariablePolicyLib|MdeModulePkg/Library/VariablePolicyLib/VariablePolicyLibRuntimeDxe.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibFmp/DxeRuntimeCapsuleLib.inf
  ResetSystemLib|MdeModulePkg/Library/RuntimeResetSystemLib/RuntimeResetSystemLib.inf
!if $(PERFORMANCE_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
!endif

[LibraryClasses.X64.UEFI_DRIVER]


[LibraryClasses.X64.UEFI_APPLICATION]
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
!if $(PERFORMANCE_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
!endif


[LibraryClasses.X64.DXE_DRIVER, LibraryClasses.X64.UEFI_DRIVER, LibraryClasses.X64.UEFI_APPLICATION]
  ResetSystemLib|MdeModulePkg/Library/DxeResetSystemLib/DxeResetSystemLib.inf






################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag.common]
  gEfiByoModulePkgTokenSpaceGuid.PcdStatusCodeUseOem|FALSE
  gEfiByoModulePkgTokenSpaceGuid.PcdStatusCodePrintProgressCode|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdUgaConsumeSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreImageLoaderSearchTeSectionFirst|FALSE
  gEfiByoModulePkgTokenSpaceGuid.PcdStatusCodeUsePostCode|TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdShowPostCodeToScreen|TRUE

!if $(BEEP_STATUS_CODE_ENABLE) == TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdStatusCodeUseBeep|TRUE
!else
  gEfiByoModulePkgTokenSpaceGuid.PcdStatusCodeUseBeep|FALSE
!endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdVariableCollectStatistics|FALSE
  gEfiByoModulePkgTokenSpaceGuid.PcdPs2KeyboardRxAndDiscardPs2MouseData|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdUefiVariableDefaultLangDeprecate|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdInstallAcpiSdtProtocol|TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdKbcPresent|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdHiiOsRuntimeSupport|FALSE
  gEfiByoModulePkgTokenSpaceGuid.PcdStatusCodePrintProgressCode|FALSE
  gEfiByoModulePkgTokenSpaceGuid.PcdSortSmbiosByType|TRUE
!if gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiS3Enable
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwarePerformanceDataTableS3Support|FALSE
!endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserGrayOutTextStatement|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowerGrayOutReadOnlyMenu|TRUE




[PcdsFeatureFlag.X64]
  gByoLegacyPkgTokenSpaceGuid.PcdCsm32Legacy2UefiSupport|TRUE
  gByoLegacyPkgTokenSpaceGuid.PcdLegacyPciRestoreVideoModeSupport|FALSE




[PcdsFixedAtBuild.common]
!if $(DEBUG_MESSAGE_ENABLE) == FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|FALSE
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|TRUE
!endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseMemory|FALSE
!if $(UART_SELECTION) == 0
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonIdsDebugPrintSerialPort|0xfedc9000
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x3F8
!elseif $(UART_SELECTION) == 2
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonIdsDebugPrintSerialPort|0xfedca000
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x2F8
!endif
  gUefiCpuPkgTokenSpaceGuid.PcdCpuMaxLogicalProcessorNumber|256
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseSize|0x10000000
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0xE0000000
  gEfiByoModulePkgTokenSpaceGuid.PcdLegacyUsbPciScanMaxBus|0xFF  
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVendor|L"Byosoft"
!if $(SECURE_BOOT_ENABLE) == TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x10000
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x4000
!endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdHwErrStorageSize|0x00000800
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxHardwareErrorVariableSize|0x400
  gEfiMdeModulePkgTokenSpaceGuid.PcdAriSupport|TRUE

!if $(DEBUG_MESSAGE_ENABLE) == FALSE
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x0
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x03
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x7
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseHardwareFlowControl|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07
!endif
!if $(PERFORMANCE_ENABLE) == TRUE
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0x9
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxPeiPerformanceLogEntries|30
!endif
  gPlatformModuleTokenSpaceGuid.PcdTemporaryRamBase|0x000400000
  gPlatformModuleTokenSpaceGuid.PcdTemporaryRamSize|0x000100000
  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreMaxPeiStackSize|0x80000
  gPlatformModuleTokenSpaceGuid.PcdPeiTemporaryRamStackSize|0                 # auto
  gEfiMdeModulePkgTokenSpaceGuid.PcdShadowPeimOnS3Boot|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemId|"_BYO_ "
  gEfiByoModulePkgTokenSpaceGuid.PcdBiosRecoveryFileName|L""
  gEfiByoModulePkgTokenSpaceGuid.AcpiIoPortBaseAddress|0x400
  gEfiByoModulePkgTokenSpaceGuid.PcdTextSetupMouseEnable|TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdSwSmiCmdPort|0xB0
  gEfiMdePkgTokenSpaceGuid.PcdUefiVariableDefaultPlatformLangCodes|"zh-Hans;en-US"
  gEfiMdePkgTokenSpaceGuid.PcdUefiVariableDefaultPlatformLang|"zh-Hans"
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemTableId|0x2054464F534F5942
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorId|0x204F5942
  gPlatformModuleTokenSpaceGuid.PcdSmbiosBaseBoardVersion|" "
  gPlatformModuleTokenSpaceGuid.PcdSmbiosManufacturer|"Hygon"
  gPlatformModuleTokenSpaceGuid.PcdSmbiosSystemInfoFamily|" "
  gPlatformModuleTokenSpaceGuid.PcdSmbiosSystemInfoProductName|""
  gPlatformModuleTokenSpaceGuid.PcdSmbiosSystemInfoFamily|"Type1Family"
  gPlatformModuleTokenSpaceGuid.PcdSmbiosSystemInfoSkuNumber|"Type1Sku0"
  gPlatformModuleTokenSpaceGuid.PcdSmbiosChassisVersion|" "
  gPlatformModuleTokenSpaceGuid.PcdSmbiosChassisType|0x17           # RackMountChassis
  gPlatformModuleTokenSpaceGuid.PcdSmbiosChassisHeight|2
  gPlatformModuleTokenSpaceGuid.PcdSmbiosChassisPowerCordsCount|2
  gPlatformModuleTokenSpaceGuid.PcdSmbiosChassisSecurityStatus|3    # None
  gPlatformModuleTokenSpaceGuid.PcdSmbiosChassisBootupState|3
  gPlatformModuleTokenSpaceGuid.PcdSmbiosChassisPowerSupplyState|3
  gPlatformModuleTokenSpaceGuid.PcdSmbiosChassisThermalState|3
  gPlatformModuleTokenSpaceGuid.PcdPlatformSmbiosType11Support|TRUE
  gPlatformModuleTokenSpaceGuid.PcdPlatformSmbiosType12Support|FALSE
  gPlatformModuleTokenSpaceGuid.PcdPlatformSmbiosType23Support|FALSE
  gPlatformModuleTokenSpaceGuid.PcdPlatformSmbiosType11String|" "
  gEfiByoModulePkgTokenSpaceGuid.PcdBdsBootOrderUpdateMethod|2
  gEfiByoModulePkgTokenSpaceGuid.PcdOemCopyRightLineNumber|1
  gPlatformModuleTokenSpaceGuid.PcdUartSelection|$(UART_SELECTION)

  #j_t gEfiByoModulePkgTokenSpaceGuid.PcdSmbiosBiosVersion|""
  gEfiByoModulePkgTokenSpaceGuid.PcdSmbiosBiosVersion|"$(TKN_BIOS_VERSION)"
  gEfiByoModulePkgTokenSpaceGuid.PcdBiosFileBaseName|"$(TKN_BIOS_IMAGE_BASENAME)"
  #gEfiProjectPkgTokenSpaceGuid.PcdStaticCustomerId|$(TKN_CUSTOMER_ID)

  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserSubtitleTextColor|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserFieldTextColor|0x01
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserFieldTextHighlightColor|0x0F
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserFieldBackgroundHighlightColor|0x70

#
# cmos list
#
# 70 - 73     MTC. (disabled now)
# 74          bios update step.
# 76          debug level    
# 77          Nv variable sw update flag      
  gEfiByoModulePkgTokenSpaceGuid.PcdMtcCmosOffset|0x70
  gEfiByoModulePkgTokenSpaceGuid.PcdRecoveryStepCmosOffset|0x74
  gPlatformModuleTokenSpaceGuid.PcdDubugLevelCmosOffset|0x76
  gPlatformModuleTokenSpaceGuid.PcdNvVarSwSetCmosOffset|0x77
  gEfiByoModulePkgTokenSpaceGuid.PcdSetupDataVersion|0
  gEfiByoModulePkgTokenSpaceGuid.PcdByoSecureFlashSupport|TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdAhciOrPortNameIndexBase0|FALSE
  gEfiByoModulePkgTokenSpaceGuid.PcdInternalShellDefaultEnable|1
  gEfiByoModulePkgTokenSpaceGuid.PcdDefaultBootTimeout|5
  gEfiByoModulePkgTokenSpaceGuid.PcdIgdNotPhysicalPresent|0

  # set PCDs for ResetSystemLib ResetCold(), used in Tcg2Pei
  gPcAtChipsetPkgTokenSpaceGuid.PcdResetControlRegister|0xCF9
  gPcAtChipsetPkgTokenSpaceGuid.PcdResetControlValueColdReset|0x06

  gEfiProjectPkgTokenSpaceGuid.PcdBoardIdType|$(TKN_BOARD_ID_TYPE)
  gEfiByoModulePkgTokenSpaceGuid.PcdBiosSignMethod|$(TKN_SIGN_METHOD)
  gEfiByoModulePkgTokenSpaceGuid.PcdKeepSysPasswordWhenLoadFCE|TRUE

  gUefiCpuPkgTokenSpaceGuid.PcdCpuS3ApVectorAddress|0
  gUefiCpuPkgTokenSpaceGuid.PcdCpuS3ApVectorMaxSize|0

!if $(TARGET_SVID_SUPPORT) == TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdBoardSsid|$(BOARD_SSID_SVID)  
!endif

  gEfiByoModulePkgTokenSpaceGuid.PcdDebugLevelOem0|0x00000000                 # none
  gEfiByoModulePkgTokenSpaceGuid.PcdDebugLevelOem1|0x80040000                 # error, OEM
  gEfiByoModulePkgTokenSpaceGuid.PcdDebugLevelOem2|0x80040040                 # error, OEM, info.
  gEfiByoModulePkgTokenSpaceGuid.PcdDebugLevelOem3|0x80040046                 # error, OEM, info, load, warn.

  gPcAtChipsetPkgTokenSpaceGuid.PcdMaximalValidYear|2099
  gPcAtChipsetPkgTokenSpaceGuid.PcdMinimalValidYear|2021

!if $(IPMI_SUPPORT) == TRUE
  gServerCommonPkgModuleTokenSpaceGuid.PcdSolPriority|TRUE
  gServerCommonPkgModuleTokenSpaceGuid.PcdWaitBmcReadyMaxSeconds|90
!endif

  gEfiByoModulePkgTokenSpaceGuid.PcdSystemPasswordMinLength|6

  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdCfgIoApicMMIOAddressReservedEnable|FALSE
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdCfgIommuMMIOAddressReservedEnable|FALSE
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonAcpiCpuSsdtProcessorScopeInSb|TRUE
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonIdsDebugPrintSerialPortDetectCableConnection|FALSE

!if $(TKN_BOARD_ID_TYPE) == 0
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonNumberOfPhysicalSocket|2
!elseif $(TKN_BOARD_ID_TYPE) == 1
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonNumberOfPhysicalSocket|1
!elseif $(TKN_BOARD_ID_TYPE) == 2
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonNumberOfPhysicalSocket|2
!endif

  gPlatformModuleTokenSpaceGuid.PcdSetupSysDebugModeDefaultAll|FALSE
  
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonSmbiosSocketDesignationSocket0|"CPU0"
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonSmbiosSocketDesignationSocket1|"CPU1"

!if $(TKN_BOARD_ID_TYPE) == 1
  gEfiByoModulePkgTokenSpaceGuid.PcdAhciOrPortNameIndexBase0|TRUE  
!endif

  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonSmbiosT16MaximumCapacity|0x80000000        # >= 2TB
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonSmbiosT16ExtMaximumCapacity|0x60000000000  # 6TB


[PcdsFixedAtBuild.X64]
  gPcAtChipsetPkgTokenSpaceGuid.Pcd8259LegacyModeMask|0x0eB8
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetColorMagentaAsWhite|TRUE

  # Set UiApp File Guid
  gEfiMdeModulePkgTokenSpaceGuid.PcdBootManagerMenuFile|{ 0x4a, 0xc0, 0xc9, 0xd0, 0x96, 0xe7, 0x48, 0x4c, 0x9d, 0x52, 0x9e, 0x0c, 0xed, 0x39, 0x4f, 0x4b }

  gEfiByoModulePkgTokenSpaceGuid.PcdSmiFlashDxeDirectRead|TRUE
  
  gPlatformModuleTokenSpaceGuid.PcdSmbiosType24FrontPanelResetStatusValue|1
  gPlatformModuleTokenSpaceGuid.PcdSmbiosType24KeyboardPasswordStatusValue|2 

  gEfiByoModulePkgTokenSpaceGuid.PcdEnterSetupWhenNoBootDevice|TRUE

  gPlatformModuleTokenSpaceGuid.PcdHttpBootDefaultEnable|FALSE
  
  gEfiMdeModulePkgTokenSpaceGuid.PcdPciSerialSetRTSAndDTRWhenReset|TRUE
  
  gEfiByoModulePkgTokenSpaceGuid.PcdSmiFlashOa3AcpiTableSupport|TRUE
  
  gPlatformModuleTokenSpaceGuid.PcdSwapDimm0Dimm1InSendBmcInfo|TRUE

  
  


  

[PcdsPatchableInModule.common]
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80280046
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareRevision|0x00010000
  gEfiByoModulePkgTokenSpaceGuid.PcdBiosFileExt|L"bin"







[PcdsDynamicHii.common.DEFAULT]
  gEfiMdePkgTokenSpaceGuid.PcdHardwareErrorRecordLevel|L"HwErrRecSupport"|gEfiGlobalVariableGuid|0x0|1 # Variable: L"HwErrRecSupport"
!if $(TPM_ENABLE) == TRUE  
  gEfiSecurityPkgTokenSpaceGuid.PcdTcgPhysicalPresenceInterfaceVer|L"TCG2_VERSION"|gTcg2ConfigFormSetGuid|0x0|"1.3"|NV,BS
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2AcpiTableRev|L"TCG2_VERSION"|gTcg2ConfigFormSetGuid|0x8|4|NV,BS
!endif







[PcdsDynamicDefault.common.DEFAULT]
!if $(TPM_ENABLE) == TRUE
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInstanceGuid|{0xb6, 0xe5, 0x01, 0x8b, 0x19, 0x4f, 0xe8, 0x46, 0xab, 0x93, 0x1c, 0x53, 0x67, 0x1b, 0x90, 0xcc}
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2InitializationPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2SelfTestPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2ScrtmPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInitializationPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmScrtmPolicy|1

  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2HierarchyChangeAuthPlatform|TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2ChangeEps|FALSE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2ChangePps|FALSE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2Clear|FALSE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2HierarchyControlPlatform|TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2HierarchyControlEndorsement|TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2HierarchyControlOwner|TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdTpm2ChipPresent|TRUE
!endif

  gEfiMdeModulePkgTokenSpaceGuid.PcdS3BootScriptTablePrivateDataPtr|0x0
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|1024
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|768

  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoHorizontalResolution|1024
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoVerticalResolution|768
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutColumn|128
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutRow|40
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdFchOscout1ClkContinous
  gEfiMdeModulePkgTokenSpaceGuid.PcdUse1GPageTable|TRUE

  gEfiMdeModulePkgTokenSpaceGuid.PcdSrIovSupport|TRUE

!if $(TARGET_SVID_SUPPORT) == TRUE
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSmbusSsid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSataAhciSsid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSataRaidSsid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSataRaid5Ssid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSataIdeSsid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdXhciSsid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdLpcSsid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSdSsid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdCfgAzaliaSsid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonCfgGnbPcieSSID|$(BOARD_SSID_SVID)
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonCfgGnbIGPUSSID|$(BOARD_SSID_SVID)
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonCfgGnbHDAudioSSID|$(BOARD_SSID_SVID)
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdCfgNbioSsid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdCfgIommuSsid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdCfgPspccpSsid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdCfgNtbccpSsid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdCfgXgbeSsid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdCfgNbifF0Ssid|$(BOARD_SSID_SVID) 
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdCfgNbifRCSsid|$(BOARD_SSID_SVID)
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdCfgNtbSsid|$(BOARD_SSID_SVID)
!endif	
	
	
  # set Pxe retried times after booting failed
  # when set to 0xFF, it will keep retring until booting successfully
  gEfiByoModulePkgTokenSpaceGuid.PcdPxeRetrylimites|0xFF

  gPlatformModuleTokenSpaceGuid.PcdFtpmSupport|FALSE

  # configure sata device detect delay time(s)
  gEfiMdeModulePkgTokenSpaceGuid.PcdAtaPhyDetectDelay|16
  # configure USB wait port stable stall time(ms)
  gEfiMdeModulePkgTokenSpaceGuid.PcdUsbWaitPortStableStall|100

  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut|5
 
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdCfgFchIoapicId|0x80
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdCfgGnbIoapicId|0x81

  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|TRUE
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonIdsDebugPrintEnable|FALSE

  
!if $(TKN_BOARD_ID_TYPE) == 0  
  gHgpiCpmPkgTokenSpaceGuid.PcdPlatformSelect|31
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdDieNumUsbPortAndOCPinMapHyEx|{0x0,0x0,0x0,0x0, 0x0,0x0,0x4,0x0, 0x0,0x0,0x1,0x1, 0x0,0x0,0x5,0x1, 0x0,0x0,0x2,0x2, 0x0,0x0,0x6,0x2, 0x0,0x1,0x2,0x2, 0x1,0x0,0x0,0x0, 0x1,0x0,0x4,0x0, 0x1,0x0,0x1,0x0, 0x1,0x0,0x5,0x0, 0x1,0x1,0x0,0x0, 0x1,0x1,0x4,0x0, 0x1,0x1,0x1,0x0, 0x1,0x1,0x5,0x0, 0xFF,0xFF,0xFF,0xFF}  
!elseif $(TKN_BOARD_ID_TYPE) == 1
  gHgpiCpmPkgTokenSpaceGuid.PcdPlatformSelect|33
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdDieNumUsbPortAndOCPinMapHyEx|{0x0,0x0,0x0,0x0, 0x0,0x0,0x4,0x0, 0x0,0x0,0x1,0x1, 0x0,0x0,0x5,0x1, 0x0,0x0,0x2,0x2, 0x0,0x0,0x6,0x2, 0x0,0x0,0x3,0x3, 0x0,0x1,0x2,0x2, 0x0,0x1,0x3,0x3, 0xFF,0xFF,0xFF,0xFF}  
!elseif $(TKN_BOARD_ID_TYPE) == 2
  gHgpiCpmPkgTokenSpaceGuid.PcdPlatformSelect|31
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdDieNumUsbPortAndOCPinMapHyEx|{0x0,0x0,0x0,0x0, 0x0,0x0,0x4,0x0, 0x0,0x0,0x1,0x1, 0x0,0x0,0x5,0x1, 0x0,0x0,0x2,0x2, 0x0,0x0,0x6,0x2, 0x0,0x1,0x2,0x2, 0x1,0x0,0x0,0x0, 0x1,0x0,0x4,0x0, 0x1,0x0,0x1,0x0, 0x1,0x0,0x5,0x0, 0x1,0x1,0x0,0x0, 0x1,0x1,0x4,0x0, 0x1,0x1,0x1,0x0, 0x1,0x1,0x5,0x0, 0xFF,0xFF,0xFF,0xFF} 
!endif  
    
#j_t  gEfiProjectPkgTokenSpaceGuid.PcdCustomerId|$(TKN_CUSTOMER_ID)
  
  
#------------------------------------------------------------------------------
[Components.IA32]
  $(PLATFORM_PACKAGE)/SecCore/SecCore.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      PlatformHookLib|$(PROJECT_PKG)/Library/PlatformHookLib/PlatformHookLib.inf
      DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
      SecPlatformLib|$(PROJECT_PKG)/Library/SecPlatformLib/SecPlatformLib.inf
      NULL|HgpiModulePkg/Library/CcxSetMmioCfgBaseLib/CcxSetMmioCfgBaseLib.inf
  }
  MdeModulePkg/Core/Pei/PeiMain.inf {
  <LibraryClasses>
!if $(DEBUG_MESSAGE_ENABLE) == TRUE
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
  }

  MdeModulePkg/Universal/PCD/Pei/Pcd.inf {
  <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }

  UefiCpuPkg/CpuIoPei/CpuIoPei.inf
  $(PLATFORM_PACKAGE)/PlatformEarlyPei/PlatformEarlyPei.inf
!if $(IPMI_SUPPORT) == TRUE
  ByoServerPkg/Ipmi/GenericPei/PeiGenericIpmi.inf
  ByoServerPkg/Ipmi/IpmiBootOptionPei/IpmiBootOptionPei.inf
!endif
  $(PROJECT_PKG)/IpmiConfigSetup/IpmiConfigSetupPei.inf

  $(PROJECT_PKG)/ProjectEarlyPei/ProjectEarlyPei.inf
  $(PLATFORM_PACKAGE)/BootModePei/BootModePei.inf
  MdeModulePkg/Universal/ReportStatusCodeRouter/Pei/ReportStatusCodeRouterPei.inf
  ByoModulePkg/StatusCodeHandler/Pei/StatusCodeHandlerPei.inf
  MdeModulePkg/Universal/FaultTolerantWritePei/FaultTolerantWritePei.inf
  MdeModulePkg/Universal/Variable/Pei/VariablePei.inf
  $(PLATFORM_PACKAGE)/PlatformPei/PlatformPei.inf
  $(PLATFORM_PACKAGE)/ReportRasUce/ReportRasUcePei.inf
  $(PLATFORM_PACKAGE)/Cpu/Pei/CpuPei.inf
  $(PLATFORM_PACKAGE)/MemoryInit/MemDetect.inf {
    <LibraryClasses>
!if ($(TPM_ENABLE) == TRUE) OR ($(TCM_ENABLE) == TRUE)
    NULL|$(PLATFORM_PACKAGE)/Library/TpmInitDoneDepexLib/TpmInitDoneDepexLib.inf
!endif
  }

!if $(TKN_BOARD_ID_TYPE) == 0
  HgpiCpmPkg/Addendum/Oem/NanHaiVtb1/Pei/HygonCpmOemInitPei/HygonCpmOemInitPeimNanHaiVtb1.inf
!elseif $(TKN_BOARD_ID_TYPE) == 1
  HgpiCpmPkg/Addendum/Oem/NanHaiVtb3/Pei/HygonCpmOemInitPei/HygonCpmOemInitPeimNanHaiVtb3.inf
!elseif $(TKN_BOARD_ID_TYPE) == 2
  $(PROJECT_PKG)/HygonCpmOemInitPei/HygonCpmOemInitPei.inf
!endif

  MdeModulePkg/Universal/PcatSingleSegmentPciCfg2Pei/PcatSingleSegmentPciCfg2Pei.inf
!if gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiS3Enable
  UefiCpuPkg/PiSmmCommunication/PiSmmCommunicationPei.inf
  UefiCpuPkg/Universal/Acpi/S3Resume2Pei/S3Resume2Pei.inf
!endif  
  MdeModulePkg/Core/DxeIplPeim/DxeIpl.inf {  
    <LibraryClasses>      
      NULL|MdeModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  }
  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTablePei/FirmwarePerformancePei.inf

!if $(RECOVERY_ENABLE) == TRUE
  $(PLATFORM_PACKAGE)/PlatformPei/Usb/UsbController.inf
  ByoModulePkg/Universal/Disk/FatPei/FatPei.inf
  ByoModulePkg/CrisisRecovery/ModuleRecoveryPei/ModuleRecoveryPei.inf
  MdeModulePkg/Bus/Usb/UsbBotPei/UsbBotPei.inf
  MdeModulePkg/Bus/Usb/UsbBusPei/UsbBusPei.inf
  MdeModulePkg/Bus/Pci/XhciPei/XhciPei.inf
  MdeModulePkg/Bus/Ata/AhciPei/AhciPei.inf
  $(PLATFORM_PACKAGE)/AhciInitPei/AhciInitPei.inf
!if $(RECOVERY_CDROM_ENABLE) == TRUE
  MdeModulePkg/Bus/Pci/IdeBusPei/IdeBusPei.inf                    # for ATAPI
  ByoModulePkg/Universal/Disk/CDExpressPei/CdExpressPei.inf
!endif
!endif

!if $(TCM_ENABLE) == TRUE
  ByoModulePkg/Security/Tcm/Pei/TcmPei.inf {
    <LibraryClasses>
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/TcmDpx.inf
  }
!endif

!if $(TPM_ENABLE) == TRUE
  ByoModulePkg/Security/Tpm/Tpm2Setup/Tcg2ConfigPei.inf {
  <LibraryClasses>
      Tpm12CommandLib|SecurityPkg/Library/Tpm12CommandLib/Tpm12CommandLib.inf
      Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibDTpm/Tpm12DeviceLibDTpm.inf
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2DeviceLibDTpm.inf
      NULL|$(PLATFORM_PACKAGE)/Library/PcieTrainingDoneDepexLib/PcieTrainingDoneDepexLib.inf
  }

  SecurityPkg/Tcg/TcgPei/TcgPei.inf {
    <LibraryClasses>
      Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibDTpm/Tpm12DeviceLibDTpm.inf
      Tpm12CommandLib|SecurityPkg/Library/Tpm12CommandLib/Tpm12CommandLib.inf
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/Tpm12Dpx.inf
  }

  SecurityPkg/Tcg/PhysicalPresencePei/PhysicalPresencePei.inf

  SecurityPkg/Tcg/Tcg2Pei/Tcg2Pei.inf {
  <LibraryClasses>
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterPei.inf
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
      NULL|$(PLATFORM_PACKAGE)/Library/Tpm2InstanceLibHgfTpm/Tpm2InstanceLibHgfTpm.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha384/HashInstanceLibSha384.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha512/HashInstanceLibSha512.inf
      NULL|SecurityPkg/Library/HashInstanceLibSm3/HashInstanceLibSm3.inf
      HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterPei.inf
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/Tpm20Dpx.inf
  }
!endif

  ByoModulePkg/SmiFlashPei/SmiFlashPei.inf
!if $(EARLY_GOP_SUPPORT) == TRUE  
  $(PLATFORM_PACKAGE)/PlatEarlyGopPciInit/PlatEarlyGopPciInit.inf
!endif



[Components.X64]
  MdeModulePkg/Universal/SmmCommunicationBufferDxe/SmmCommunicationBufferDxe.inf

  MdeModulePkg/Core/Dxe/DxeMain.inf {
  <LibraryClasses>
      NULL|MdeModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
!if $(DEBUG_MESSAGE_ENABLE) == TRUE
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
  }

  MdeModulePkg/Universal/PCD/Dxe/Pcd.inf {
  <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }

  MdeModulePkg/Universal/ReportStatusCodeRouter/RuntimeDxe/ReportStatusCodeRouterRuntimeDxe.inf
  MdeModulePkg/Universal/ReportStatusCodeRouter/Smm/ReportStatusCodeRouterSmm.inf
  ByoModulePkg/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf {
  <LibraryClasses>
!if $(DEBUG_MESSAGE_ENABLE) == TRUE
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
  }
  ByoModulePkg/StatusCodeHandler/Smm/StatusCodeHandlerSmm.inf {
  <LibraryClasses>
!if $(DEBUG_MESSAGE_ENABLE) == TRUE
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
  }
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf {
  <LibraryClasses>
!if $(SECURE_BOOT_ENABLE) == TRUE
      NULL|SecurityPkg/Library/DxeImageVerificationLib/DxeImageVerificationLib.inf
!endif
!if $(TPM_ENABLE) == TRUE
      NULL|SecurityPkg/Library/DxeTpmMeasureBootLib/DxeTpmMeasureBootLib.inf
      NULL|SecurityPkg/Library/DxeTpm2MeasureBootLib/DxeTpm2MeasureBootLib.inf
!endif
      NULL|HgpiModulePkg/Library/HygonPspTpcmWrapperLib/HygonPspTpcmWrapperLib.inf
  }

!if $(SECURE_BOOT_ENABLE) == TRUE
  ByoModulePkg/Security/SecureBootConfigDxe/SecureBootConfigDxe.inf {
    <BuildOptions>
    # Security page class Guid gIfrByoSecurityUiPageGuid
    *_*_*_VFR_FLAGS = -g D7EDC7D9-5A9A-43DD-BE2B-C48FE840B4A7
  }
  ByoModulePkg/Security/SecureBoot/SecureBootDefult.inf
!endif

  ByoModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  ByoModulePkg/Universal/SmbiosSmm/SmbiosSmm.inf
  ByoModulePkg/Universal/ByoSmbiosTable/ByoSmbiosTable.inf

  ByoLegacyPkg/8259InterruptControllerDxe/8259.inf

  MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  MdeModulePkg/Universal/DriverHealthManagerDxe/DriverHealthManagerDxe.inf

  UefiCpuPkg/CpuIo2Dxe/CpuIo2Dxe.inf

  $(PLATFORM_PACKAGE)/Cpu/Dxe/CpuDxe.inf
  ByoLegacyPkg/VideoDxe/VideoDxe.inf

  MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  ByoLegacyPkg/8254TimerDxe/8254Timer.inf
  MdeModulePkg/Universal/Metronome/Metronome.inf
  ByoModulePkg/Setup/SystemPassword/SystemPassword.inf
  $(PROJECT_PKG)/Override/PostLogin/PostLogin.inf
  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  PcAtChipsetPkg/PcatRealTimeClockRuntimeDxe/PcatRealTimeClockRuntimeDxe.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
# MdeModulePkg/Universal/EbcDxe/EbcDxe.inf

  MdeModulePkg/Universal/LockBox/SmmLockBox/SmmLockBox.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }

!if gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiS3Enable
  MdeModulePkg/Universal/Acpi/S3SaveStateDxe/S3SaveStateDxe.inf
  MdeModulePkg/Universal/Acpi/SmmS3SaveState/SmmS3SaveState.inf
  MdeModulePkg/Universal/Acpi/BootScriptExecutorDxe/BootScriptExecutorDxe.inf {   # NVS
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  }
!endif

  $(PLATFORM_PACKAGE)/PcieInitDxe/PcieInitDxe.inf
  ByoModulePkg/Logo/LogoDxe.inf
  MdeModulePkg/Universal/BdsDxe/BdsDxe.inf

  ByoModulePkg/Setup/DisplayBackupRecoveryDxe/DisplayBackupRecoveryDxe.inf

  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf

  $(PLATFORM_PACKAGE)/PlatformEarlyDxe/PlatformEarlyDxe.inf
  $(PROJECT_PKG)/ProjectEarlyDxe/ProjectEarlyDxe.inf
  $(PLATFORM_PACKAGE)/ReportRasUce/ReportRasUceDxe.inf
  $(PROJECT_PKG)/Override/PlatformDxe/PlatformDxe.inf
  $(PROJECT_PKG)/Inspur/HamVga/InspurGop/UefiSmi.inf

  ByoModulePkg/HddBindDxe/HddBindDxe.inf {
    # Security Page Class Guid
    <BuildOptions>
       *_*_*_VFR_FLAGS = -g D7EDC7D9-5A9A-43DD-BE2B-C48FE840B4A7
  }

  ByoModulePkg/ByoBdsBootManagerDxe/ByoBdsBootManagerDxe.inf
  ByoModulePkg/StatusCodeHandler/VerifyErrorMsgDxe/VerifyErrorMsgDxe.inf
  ByoModulePkg/Universal/PlatformBootManagerDxe/PlatformBootManagerDxeEdk2.inf {
    <LibraryClasses>
    NULL|MdeModulePkg/Library/BootManagerUiLib/BootManagerUiLib.inf
  }
  ByoModulePkg/Universal/PlatformBootManagerDxe/PlatformBootManagerDxe.inf
  ByoLegacyPkg/Int18h/Int18h.inf
  ByoLegacyPkg/Int18h/InstallInt18.inf
!if $(IPMI_SUPPORT) == TRUE
#  $(PLATFORM_PACKAGE)/RasDxe/RasDxe.inf
!endif

  $(PLATFORM_PACKAGE)/Override/PcAtChipsetPkg/PciHostBridgeDxe/PciHostBridgeDxe.inf {
    <PcdsPatchableInModule>
#     gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
    <LibraryClasses>
      FabricResourceInitLib|HgpiModulePkg/Library/FabricResourceManagerStLib/FabricResourceInitStLib.inf
  }
  
  MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf {
  <PcdsPatchableInModule>
#   gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }
  MdeModulePkg/Universal/MemoryTest/GenericMemoryTestDxe/GenericMemoryTestDxe.inf
  ByoLegacyPkg/LegacyBiosDxe/LegacyBiosDxe.inf
# ByoLegacyPkg/BlockIoDxe/BlockIoDxe.inf
  ByoLegacyPkg/LegacyInterruptHookDxe/LegacyInterruptHook.inf

  UefiCpuPkg/CpuIo2Smm/CpuIo2Smm.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmIpl.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmCore.inf {
  <LibraryClasses>
!if $(DEBUG_MESSAGE_ENABLE) == TRUE
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
  }

  $(PLATFORM_PACKAGE)/SmmPlatform/SmmPlatform.inf
  UefiCpuPkg/PiSmmCommunication/PiSmmCommunicationSmm.inf

  $(PROJECT_PKG)/ProjectSmm/ProjectSmm.inf {
    <LibraryClasses>
      SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
  }

!if $(IPMI_SUPPORT) == TRUE
  ByoServerPkg/Ipmi/Generic/GenericIpmi.inf
  ByoServerPkg/Ipmi/Generic/SmmGenericIpmi.inf
  ByoServerPkg/Ipmi/BmcSol/SolStatus.inf
  $(PROJECT_PKG)/BmcCfg/BmcCfg.inf
  $(PROJECT_PKG)/Override/SendInfoToBmc/SendInfoToBmc.inf
  ByoServerPkg/Ipmi/BmcWdt/BmcWdt.inf
  $(PROJECT_PKG)/Override/SetupBmcCfg/SetupBmcCfg.inf {
    <BuildOptions>
    # Hide page class Guid gIfrByoHideUiPageGuid
    *_*_*_VFR_FLAGS = -g AA0B824F-D457-4277-8719-A6D7FD60ED38
  }
  ByoServerPkg/Ipmi/IpmiBootOrder/IpmiBootOrder.inf
  ByoServerPkg/Ipmi/IpmiRedirFru/IpmiRedirFru.inf
  $(PROJECT_PKG)/IpmiConfigSetup/IpmiConfigSetupDxe.inf
!endif

  $(PROJECT_PKG)/ByoPlatformPromptDxe/ByoPlatformPromptDxe.inf
#ATA & SCSI
  MdeModulePkg/Bus/Ata/AtaAtapiPassThru/AtaAtapiPassThru.inf
  MdeModulePkg/Bus/Ata/AtaBusDxe/AtaBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiBusDxe/ScsiBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiDiskDxe/ScsiDiskDxe.inf
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf

  ByoModulePkg/Security/HddPassword/Dxe/HddPasswordDxe.inf {
    <LibraryClasses>
      OpalPasswordSupportLib|ByoModulePkg/Library/OpalPasswordSupportLib/OpalPasswordSupportLib.inf
    # Security Page Class Guid
    <BuildOptions>
       *_*_*_VFR_FLAGS = -g D7EDC7D9-5A9A-43DD-BE2B-C48FE840B4A7
  }

  ByoModulePkg/SmiFlash/SmiFlash.inf {
    <LibraryClasses>
    SystemPasswordLib | ByoModulePkg/Library/SystemPasswordLib/SystemPasswordLibSmm.inf
  }
  ByoModulePkg/SmiFlashDxe/SmiFlashDxe.inf


  MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf   
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf
  MdeModulePkg/Bus/Usb/UsbMouseAbsolutePointerDxe/UsbMouseAbsolutePointerDxe.inf

  ByoModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
  ByoModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  ByoModulePkg/Bus/Usb/LegacyUsbSmm/LegacyUsbSmm.inf
  ByoModulePkg/Bus/Usb/LegacyFreeKbDxe/LegacyFreeKbDxe.inf
  ByoModulePkg/Bus/Usb/UsbMouseAbsolutePointerDxe/UsbMouseAbsolutePointerDxe.inf

  $(PROJECT_PKG)/AcpiTables/AcpiTables.inf

  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf
  $(PLATFORM_PACKAGE)/AcpiPlatformDxe/AcpiPlatformDxe.inf
  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf


  HgpiModulePkg/Fch/HyEx/FchTaishanSmm/FchSmm.inf {
    <LibraryClasses>
      NULL|ByoHgpiModulePkg/Library/SpiSmmProtocolLib/SpiSmmProtocolLib.inf
  }
  HgpiModulePkg/Fch/HyGx/FchTaishanSmm/FchSmm.inf {
    <LibraryClasses>
      NULL|ByoHgpiModulePkg/Library/SpiSmmProtocolLib/SpiSmmProtocolLib.inf 
  }
  ByoDevicePkg/PlatformAccess/Smm/PlatformAccess.inf
  ByoDevicePkg/NvMediaAccess/Smm/NvMediaAccess.inf
  ByoDevicePkg/FlashDevice/FlashDeviceSmm/FlashDeviceSmm.inf
  ByoDevicePkg/FvbService/Smm/FvbService.inf

  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteSmm.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmmRuntimeDxe.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmm.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckPolicyLib/VarCheckPolicyLib.inf  
  }

  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  ByoModulePkg/Setup/JpegDecoderDxe/JpegDecoder.inf
  ByoModulePkg/Setup/DisplayEngineDxe/DisplayEngineDxe.inf
!if $(REDFISH_ENABLE) == TRUE
  ByoModulePkg/Setup/SetupBrowserDxe/SetupBrowserRedfishDxe.inf
  ByoModulePkg/Application/ByoUiApp/ByoUiDxe.inf
!else
  ByoModulePkg/Setup/SetupBrowserDxe/SetupBrowserDxe.inf
!endif
  ByoModulePkg/Setup/SetupMouse/SetupMouse.inf
  ByoModulePkg/Setup/UnicodeFontDxe/UnicodeFontDxe.inf
  $(PROJECT_PKG)/PlatformSetupDxe/PlatformSetupDxe.inf {
    <BuildOptions>
      *_*_*_VFRPP_FLAGS = /DTKN_HYGON_PSP_TPCM_ENABLE
  }

  ByoModulePkg/Application/BootManagerMenuApp/BootManagerMenuApp.inf

  MdeModulePkg/Library/BootManagerUiLib/BootManagerUiLib.inf {
    # Boot Page Class Guid
    <BuildOptions>
    *_*_*_VFR_FLAGS = -g d6fc2e65-bd6c-4b79-9ffc-0d15364e0773
  }

  ByoModulePkg/Universal/BootMaintenanceManagerDxe/BootMaintenanceManagerDxe.inf

  ByoModulePkg/Application/ByoUiApp/ByoUiApp.inf

  $(PLATFORM_PACKAGE)/HygonSioDxe/HygonSioDxe.inf
  MdeModulePkg/Bus/Pci/PciSioSerialDxe/PciSioSerialDxe.inf
# MdeModulePkg/Bus/Isa/Ps2KeyboardDxe/Ps2KeyboardDxe.inf
# MdeModulePkg/Bus/Isa/Ps2MouseDxe/Ps2MouseDxe.inf
  
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf

  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTableDxe/FirmwarePerformanceDxe.inf
  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTableSmm/FirmwarePerformanceSmm.inf
  MdeModulePkg/Universal/Acpi/BootGraphicsResourceTableDxe/BootGraphicsResourceTableDxe.inf


!if $(TCM_ENABLE) == TRUE
  ByoModulePkg/Security/Tcm/Dxe/TcmDxe.inf {
    <LibraryClasses>
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/TcmDpx.inf
  }
  ByoModulePkg/Security/Tcm/Setup/TcmSetup.inf {
    <LibraryClasses>
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/TcmDpx.inf  
    <BuildOptions>
      # Security page class Guid gIfrByoSecurityUiPageGuid
      *_*_*_VFR_FLAGS = -g D7EDC7D9-5A9A-43DD-BE2B-C48FE840B4A7
  }
  ByoModulePkg/Security/Tcm/Dxe/TcmAcpi.inf {
    <LibraryClasses>
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/TcmDpx.inf
  }
  ByoModulePkg/Security/Tcm/Smm/TcmSmm.inf {
    <LibraryClasses>
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/TcmDpx.inf
  } 
!endif

!if $(NVME_SUPPORT) == TRUE
  ByoModulePkg/Nvme/NvmeDxe/NvmExpressDxe.inf
!if $(LEGACY_NVME_SUPPORT) == TRUE
  ByoModulePkg/Nvme/NvmeSmm/NvmeSmm.inf
!endif
!endif



!if $(TPM_ENABLE) == TRUE
  SecurityPkg/Tcg/MemoryOverwriteControl/TcgMor.inf
  SecurityPkg/Tcg/MemoryOverwriteRequestControlLock/TcgMorLockSmm.inf

  SecurityPkg/Tcg/TcgDxe/TcgDxe.inf{
  <LibraryClasses>
      Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibDTpm/Tpm12DeviceLibDTpm.inf
      Tpm12CommandLib|SecurityPkg/Library/Tpm12CommandLib/Tpm12CommandLib.inf
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/Tpm12Dpx.inf
  }

  ByoModulePkg/Security/Tpm/Tcg2PlatformDxe/Tcg2PlatformDxe.inf {
    <LibraryClasses>
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibTcg2/Tpm2DeviceLibTcg2.inf
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/Tpm20Dpx.inf
  }

  SecurityPkg/Tcg/TcgConfigDxe/TcgConfigDxe.inf {
  <BuildOptions>
    # Security page class Guid gIfrByoSecurityUiPageGuid
    *_*_*_VFR_FLAGS = -g D7EDC7D9-5A9A-43DD-BE2B-C48FE840B4A7
  <LibraryClasses>
      TpmCommLib|SecurityPkg/Library/TpmCommLib/TpmCommLib.inf
      Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibDTpm/Tpm12DeviceLibDTpm.inf
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/Tpm12Dpx.inf
  }
  SecurityPkg/Tcg/TcgSmm/TcgSmm.inf {
    <LibraryClasses>
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/Tpm12Dpx.inf
  }

!if $(LEGACY_TPM_SUPPORT) == TRUE
  ByoModulePkg/Security/Tpm/TcgServiceSmm/TcgSmm.inf {
  <LibraryClasses>
      TpmCommLib|SecurityPkg/Library/TpmCommLib/TpmCommLib.inf
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/Tpm12Dpx.inf
  }
  ByoModulePkg/Security/Tpm/TcgServiceSmm/TcgSmm16.inf{
    <LibraryClasses>
#     NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/Tpm12Dpx.inf
  }
  ByoModulePkg/Security/Tpm/TcgServiceSmm/TcgSmmInstallInt1A.inf{
    <LibraryClasses>
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/Tpm12Dpx.inf
  }
!endif

  SecurityPkg/Tcg/Tcg2Dxe/Tcg2Dxe.inf {
  <LibraryClasses>
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterDxe.inf
      HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterDxe.inf
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
      NULL|$(PLATFORM_PACKAGE)/Library/Tpm2InstanceLibHgfTpm/Tpm2InstanceLibHgfTpmDxe.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha384/HashInstanceLibSha384.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha512/HashInstanceLibSha512.inf
      NULL|SecurityPkg/Library/HashInstanceLibSm3/HashInstanceLibSm3.inf
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/Tpm20Dpx.inf
  }
  ByoModulePkg/Security/Tpm/Tpm2Setup/Tcg2ConfigDxe.inf {
  <BuildOptions>
    # Security page class Guid gIfrByoSecurityUiPageGuid
    *_*_*_VFR_FLAGS = -g D7EDC7D9-5A9A-43DD-BE2B-C48FE840B4A7
  <LibraryClasses>
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibTcg2/Tpm2DeviceLibTcg2.inf
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/Tpm20Dpx.inf
  }

  SecurityPkg/Tcg/Tcg2Smm/Tcg2Smm.inf{
    <LibraryClasses>
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/Tpm20Dpx.inf
  }
  SecurityPkg/Tcg/Tcg2Acpi/Tcg2Acpi.inf{
    <LibraryClasses>
      NULL|$(PLATFORM_PACKAGE)/Library/TpmTcmDpxLib/Tpm20Dpx.inf
  }

!endif

  $(PROJECT_PKG)/BiosInfo/BiosInfo.inf
  $(PLATFORM_PACKAGE)/Cpu/Smm/CpuSmm.inf
  $(PROJECT_PKG)/ProjectDxe/ProjectDxe.inf
  $(PROJECT_PKG)/NoBootDeviceDxe/NoBootDeviceDxe.inf
  $(PROJECT_PKG)/PciHotPlugInitDxe/PciHotPlugInitDxe.inf
  $(PROJECT_PKG)/OemPStateDxe/OemPStateDxe.inf

  ByoModulePkg/Setup/PciListDxe/PciListDxe.inf {
    <BuildOptions>
    # Hide page class Guid gIfrByoHideUiPageGuid
    *_*_*_VFR_FLAGS = -g AA0B824F-D457-4277-8719-A6D7FD60ED38
  }

  ByoModulePkg/Setup/BiosUpdate/BiosUpdate.inf

  ByoModulePkg/Library/FileExplorerLib/FileExplorerLib.inf {
    <BuildOptions>
    # Hide page class Guid gIfrByoHideUiPageGuid
    *_*_*_VFR_FLAGS = -g AA0B824F-D457-4277-8719-A6D7FD60ED38
  }

  MdeModulePkg/Universal/FileExplorerDxe/FileExplorerDxe.inf

  FatPkg/EnhancedFatDxe/Fat.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }

  $(PROJECT_PKG)/Override/PxeBootApp/PxeBootAppAll.inf
  $(PROJECT_PKG)/Override/TextScreenDxe/TextScreenDxe.inf
!if $(PERFORMANCE_ENABLE) == TRUE
  ShellPkg/DynamicCommand/DpDynamicCommand/DpDynamicCommand.inf {
    <LibraryClasses>
      ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
      FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
    <PcdsFixedAtBuild>
      gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
  }
!endif

ShellPkg/Application/Shell/Shell.inf {
    <LibraryClasses>
      ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
      NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf      
      NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
!if $(NO_SHELL_PROFILES) == FALSE      
      NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
!if $(NETWORK_IP6_ENABLE) == TRUE
      NULL|ShellPkg/Library/UefiShellNetwork2CommandsLib/UefiShellNetwork2CommandsLib.inf
!endif
!endif
      HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
      PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
      BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf

    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0xFF
      gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
      gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|8000
  }

!if $(NETWORK_STACK_ENABLE)
!include NetworkPkg/Network.dsc.inc
  NetworkPkg/TlsDxe/TlsDxe.inf
  NetworkPkg/TlsAuthConfigDxe/TlsAuthConfigDxe.inf

!if $(NETWORK_HIDE_IP4_HII) == TRUE
  NetworkPkg/Ip4Dxe/Ip4Dxe.inf {
    <BuildOptions>
    # Hide page class Guid gIfrByoHideUiPageGuid
    *_*_*_VFR_FLAGS = -g AA0B824F-D457-4277-8719-A6D7FD60ED38
  }
!endif  

!if $(NETWORK_HIDE_IP6_HII) == TRUE
  NetworkPkg/Ip6Dxe/Ip6Dxe.inf {
    <BuildOptions>
    # Hide page class Guid gIfrByoHideUiPageGuid
    *_*_*_VFR_FLAGS = -g AA0B824F-D457-4277-8719-A6D7FD60ED38
  }
!endif
!endif

!if $(REDFISH_ENABLE) == TRUE
!include RedfishClientPkg/RedfishClient.dsc.inc
!endif

  HgpiCbsPkg/Library/CbsSetupLib.inf {
    <BuildOptions>
    *_*_*_VFR_FLAGS = -g FDB62C83-BA16-482E-AE27-25E6A3B3A5EA        # gIfrByoRedfishUiPageGuid
#   *_*_*_VFRPP_FLAGS = /DTKN_CBS_HIDE_MEM_CE_THRESH
  }

!if $(TKN_BOARD_ID_TYPE) == 0
  HgpiCpmPkg/Addendum/Oem/NanHaiVtb1/Dxe/ServerHotplugDxe/ServerHotplugDxe.inf
!elseif $(TKN_BOARD_ID_TYPE) == 1
  HgpiCpmPkg/Addendum/Oem/NanHaiVtb3/Dxe/ServerHotplugDxe/ServerHotplugDxe.inf
!elseif $(TKN_BOARD_ID_TYPE) == 2
  $(PROJECT_PKG)/ServerHotplugDxe/ServerHotplugDxe.inf
!endif








#-------------------------------------------------------------------------------
[BuildOptions.common.EDKII.DXE_RUNTIME_DRIVER, BuildOptions.common.EDKII.DXE_SMM_DRIVER, BuildOptions.common.EDKII.SMM_CORE]
  MSFT:*_*_*_DLINK_FLAGS         = /ALIGN:4096


[BuildOptions]


[BuildOptions.Common.EDKII]

!if $(TARGET) == DEBUG
  DEFINE UDK_DEBUG_BUILD_OPTIONS = -D TKN_TARGET_DEBUG
!else
  DEFINE UDK_DEBUG_BUILD_OPTIONS =
!endif

  DEFINE EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS = $(UDK_DEBUG_BUILD_OPTIONS) -D DISABLE_NEW_DEPRECATED_INTERFACES

  *_*_IA32_ASM_FLAGS     = $(EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_IA32_CC_FLAGS      = $(EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_IA32_VFRPP_FLAGS   = $(EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_IA32_APP_FLAGS     = $(EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_IA32_PP_FLAGS      = $(EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_IA32_ASLPP_FLAGS   = $(EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_IA32_ASM16_FLAGS   = $(EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS)

  *_*_X64_ASM_FLAGS      = $(EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_X64_CC_FLAGS       = $(EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_X64_VFRPP_FLAGS    = $(EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_X64_APP_FLAGS      = $(EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_X64_PP_FLAGS       = $(EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_X64_ASLPP_FLAGS    = $(EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_X64_ASM16_FLAGS    = $(EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS)
  
!if $(INSTANT_DEBUG_ENABLE) == TRUE
  DEBUG_*_IA32_CC_FLAGS    =  /Oy-
  DEBUG_*_X64_GENFW_FLAGS  = --keepexceptiontable
!endif
  *_*_*_ASL_FLAGS = -oi
  *_*_*_ASMLINK_PATH     = $(WORKSPACE)/ByoTools/Bin/Link16.exe
  *_*_*_ASL_PATH         = $(WORKSPACE)/ByoTools/Bin/iasl.exe
  *_*_*_NASM_PATH        = $(WORKSPACE)/ByoTools/Bin/nasm.exe



