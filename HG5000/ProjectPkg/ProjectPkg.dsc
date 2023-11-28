# @file
#
# Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
# All rights reserved.This software and associated documentation (if any)
# is furnished under a license and may only be used or copied in
# accordance with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be reproduced,
# stored in a retrieval system, or transmitted in any form or by any
# means without the express written consent of Byosoft Corporation.
#
# File Name:
#   PlatformPkg.dsc
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
  PLATFORM_NAME                       = ProjectPkg
  PLATFORM_GUID                       = 0D28678E-EA14-46cd-9F43-01B48B7B48CD
  PLATFORM_VERSION                    = 0.1
  DSC_SPECIFICATION                   = 0x00010005
  OUTPUT_DIRECTORY                    = Build/ProjectPkg
  SUPPORTED_ARCHITECTURES             = IA32|X64
  BUILD_TARGETS                       = DEBUG|RELEASE
  SKUID_IDENTIFIER                    = DEFAULT
  FLASH_DEFINITION                    = ProjectPkg/ProjectPkg.fdf

  DEFINE      PLATFORM_PACKAGE        = PlatformPkg

  CBS_LIB_PATH   = Family/0x17/ZP/External
  AGESA_PKG_PATH = AgesaModulePkg
  AGESA_PKG_DEC  = AgesaCommonModulePkg
  FCH_PKG_PATH   = AgesaModulePkg/Fch/Taishan
  FCH_PKG_DEC    = FchTaishan
  

!include PlatformAutoGen.dsc
#-------------------------------------------------------------------------------
# Set CSM_ENABLE value:
# TRUE  - CSM is always enabled
# FALSE - CSM is always disabled
# 0x2 - User can choose CSM enabled or disabled in BIOS setup
DEFINE CSM_ENABLE = 0x2
DEFINE PERFORMANCE_ENABLE = FALSE
DEFINE S3_ENABLE = TRUE
DEFINE ACPI_ENABLE = TRUE
DEFINE ACPI50_ENABLE = TRUE
DEFINE BEEP_STATUS_CODE_ENABLE = TRUE
DEFINE POSTCODE_STATUS_CODE_ENABLE = TRUE
DEFINE TPM_ENABLE = TRUE
DEFINE TCM_ENABLE = FALSE
DEFINE HDD_PASSWORD_ENABLE = TRUE

# 1 - LPC
# 2 - SPI
DEFINE TPM_DECODE = 2

DEFINE SECURE_BOOT_ENABLE = TRUE
DEFINE CRYPTO_ENABLE = TRUE
DEFINE RECOVERY_ENABLE = TRUE
DEFINE RECOVERY_CDROM_ENABLE = TRUE
DEFINE UEFI_NATIVE_NETWORK_SUPPORT = TRUE
DEFINE NETWORK_IP6_ENABLE    = TRUE
DEFINE HTTP_BOOT_ENABLE      = FALSE
DEFINE SOURCE_DEBUG_ENABLE   = FALSE
DEFINE NVME_SUPPORT          = TRUE
DEFINE LEGACY_NVME_SUPPORT   = TRUE
DEFINE IPMI_SUPPORT          = TRUE

DEFINE SECURE_KEY_PATH       = ByoModulePkg/Security/SecureKey
DEFINE TARGET_SVID_SUPPORT   = FALSE
DEFINE TARGET_SVID           = 1D05
DEFINE TARGET_SSID           = 1E03
DEFINE BOARD_SVID            = 0x$(TARGET_SVID)
DEFINE BOARD_SSID            = 0x$(TARGET_SSID)
DEFINE BOARD_SSID_SVID       = 0x$(TARGET_SSID)$(TARGET_SVID)


#
# TARGET controls the compiler option to enable source level debug.
# DEBUG_BIOS_ENABLE flag enables DEBUG message and disable optimization.
#
# TARGET    DEBUG_BIOS_ENABLE    BiosImage
# DEBUG     TRUE                 Image supports easy source level debug, and have debug message.
# DEBUG     FALSE                Image supports source level debug, but no debug message.
# RELEASE   FALSE                Image without source level debug and debug message.
# RELEASE   TRUE                 Image without source level debug, but has debug message.
#
!if $(TARGET) == DEBUG
# Disable this flag when to debug image without debug message.
  DEFINE DEBUG_BIOS_ENABLE = TRUE
!else
  DEFINE DEBUG_BIOS_ENABLE = FALSE
!endif

!if $(DEBUG_BIOS_ENABLE) == TRUE
!if $(SOURCE_DEBUG_ENABLE) == TRUE
  DEFINE DEBUG_MESSAGE_ENABLE     = FALSE
!else
  DEFINE DEBUG_MESSAGE_ENABLE     = TRUE
!endif  
  DEFINE OPTIMIZE_COMPILER_ENABLE = FALSE
!else
  DEFINE DEBUG_MESSAGE_ENABLE     = FALSE
  DEFINE OPTIMIZE_COMPILER_ENABLE = TRUE
!endif







################################################################################
#
# SKU Identification section - list of all SKU IDs supported by this
#                              Platform.
#
################################################################################
[SkuIds]
  0|DEFAULT              # The entry: 0|DEFAULT is reserved and always required.

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

!if $(SOURCE_DEBUG_ENABLE) == TRUE
  PeCoffExtraActionLib|SourceLevelDebugPkg/Library/PeCoffExtraActionLibDebug/PeCoffExtraActionLibDebug.inf
  DebugCommunicationLib|SourceLevelDebugPkg/Library/DebugCommunicationLibSerialPort/DebugCommunicationLibSerialPort.inf
!else
  PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
!endif  
  
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

# UEFI & PI
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  HiiLib|ByoModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiDecompressLib|IntelFrameworkModulePkg/Library/BaseUefiTianoCustomDecompressLib/BaseUefiTianoCustomDecompressLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLibIdt/PeiServicesTablePointerLibIdt.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  UefiCpuLib|UefiCpuPkg/Library/BaseUefiCpuLib/BaseUefiCpuLib.inf
  S3BootScriptLib|MdeModulePkg/Library/PiDxeS3BootScriptLib/DxeS3BootScriptLib.inf
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  UefiScsiLib|MdePkg/Library/UefiScsiLib/UefiScsiLib.inf

  NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
  IpIoLib|MdeModulePkg/Library/DxeIpIoLib/DxeIpIoLib.inf
  UdpIoLib|MdeModulePkg/Library/DxeUdpIoLib/DxeUdpIoLib.inf
  TcpIoLib|MdeModulePkg/Library/DxeTcpIoLib/DxeTcpIoLib.inf
  DpcLib|MdeModulePkg/Library/DxeDpcLib/DxeDpcLib.inf

  RecoveryLib|IntelFrameworkModulePkg/Library/PeiRecoveryLib/PeiRecoveryLib.inf
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf

  LogoLib|ByoModulePkg/Library/LogoLib/LogoLib.inf
  UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  ByoUefiBootManagerLib|ByoModulePkg/Library/ByoUefiBootManagerLib/ByoUefiBootManagerLib.inf
  LegacyBootManagerLib|ByoModulePkg/Library/LegacyBootManagerLib/LegacyBootManagerLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
  SmmCorePlatformHookLib|MdeModulePkg/Library/SmmCorePlatformHookLibNull/SmmCorePlatformHookLibNull.inf
  IoApicLib|PcAtChipsetPkg/Library/BaseIoApicLib/BaseIoApicLib.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  LegacyBootOptionalDataLib|ByoModulePkg/Library/LegacyBootOptionalDataLib/LegacyBootOptionalDataLib.inf
  
# CPU
  MtrrLib|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
  LocalApicLib|UefiCpuPkg/Library/BaseXApicX2ApicLib/BaseXApicX2ApicLib.inf
  SmmCpuFeaturesLib|UefiCpuPkg/Library/SmmCpuFeaturesLib/SmmCpuFeaturesLib.inf
  SmmCpuPlatformHookLib|UefiCpuPkg/Library/SmmCpuPlatformHookLibNull/SmmCpuPlatformHookLibNull.inf

# Platform
# SerialPortLib|ProjectPkg/Library/SerialPortLib/SbSerialPortLib.inf
  SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
  
  TimerLib|$(PLATFORM_PACKAGE)/Library/CpuTimerLib/TscTimerLib.inf
  PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
  SmbusLib|MdePkg/Library/BaseSmbusLibNull/BaseSmbusLibNull.inf
  SmmLib|$(PLATFORM_PACKAGE)/Library/SmmLib/SmmLib.inf
  BeepLib|$(PLATFORM_PACKAGE)/Library/BeepLib/BeepLib.inf
  PlatformCommLib|$(PLATFORM_PACKAGE)/Library/PlatformCommLib/PlatformCommLib.inf
  ByoCommLib|ByoModulePkg/Library/ByoCommLib/ByoCommLib.inf  
  ByoAzaliaLib|ByoModulePkg/Library/ByoAzaliaLib/ByoAzaliaLib.inf
  ByoKbcLib|ByoModulePkg/Library/ByoKbcLib/ByoKbcLib.inf
  ByoAgesaSettingUpdateLib|ProjectPkg/Library/ByoAgesaSettingUpdateLib/ByoAgesaSettingUpdateLib.inf
  AmdPspFtpmLib|AgesaModulePkg/Library/AmdPspFtpmLib/AmdPspFtpmLib.inf 
  ResetSystemLib|PlatformPkg/Library/ResetSystemLib/ResetSystemLib.inf 
  
# Misc
!if $(DEBUG_MESSAGE_ENABLE) == FALSE
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
!else
  DebugLib|IntelFrameworkModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
!endif
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf 
  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
  OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf  
  OpalPasswordSupportLib|SecurityPkg/Library/OpalPasswordSupportLib/OpalPasswordSupportLib.inf
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
# EdkiiSystemCapsuleLib|SignedCapsulePkg/Library/EdkiiSystemCapsuleLib/EdkiiSystemCapsuleLib.inf
# IniParsingLib|SignedCapsulePkg/Library/IniParsingLib/IniParsingLib.inf
# PlatformFlashAccessLib|SignedCapsulePkg/Library/PlatformFlashAccessLibNull/PlatformFlashAccessLibNull.inf
  SystemPasswordLib|ByoModulePkg/Library/SystemPasswordLib/SystemPasswordLib.inf
  
  StatusCodeBeepCommLib|ByoModulePkg/Library/StatusCodeBeepCommLib/StatusCodeBeepCommLib.inf
  FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerZenZpLib/FabricResourceManagerZenZpLib.inf

  #
  # Agesa specific common libraries
  #
  AmdBaseLib|AgesaModulePkg/Library/AmdBaseLib/AmdBaseLib.inf
  AmdIdsDebugPrintLib|AgesaModulePkg/Library/AmdIdsDebugPrintLib/AmdIdsDebugPrintLib.inf
  AmdIdsHookLib|AgesaModulePkg/Library/AmdIdsHookLib/AmdIdsHookLib.inf
  AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
  IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  IdsMiscLib|AgesaModulePkg/Library/IdsMiscLib/IdsMiscLib.inf
#GR-Agesa-  AmdHeapLib|AgesaModulePkg/Library/AmdHeapLibNull/AmdHeapLibNull.inf
  AmdSocBaseLib|AgesaModulePkg/Library/AmdSocBaseLib/AmdSocBaseLib.inf
  BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
  BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
  AmdErrorLogLib|AgesaModulePkg/Library/AmdErrorLogLib/AmdErrorLogLib.inf
  AmdTableLib|AgesaModulePkg/Library/AmdTableLib/AmdTableLib.inf
  SocCmnIdsHookLibPei|AgesaModulePkg/Library/SocCmnIdsHookLib/Pei/SocCmnIdsHookLibPei.inf
  SocCmnIdsHookLibDxe|AgesaModulePkg/Library/SocCmnIdsHookLib/Dxe/SocCmnIdsHookLibDxe.inf
  BaseCoreLogicalIdLib|AgesaModulePkg/Library/BaseCoreLogicalIdX86Lib/BaseCoreLogicalIdX86Lib.inf
  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf

  ## PSP Libs
  AmdPspMboxLibV1|AgesaModulePkg/Library/AmdPspMboxLibV1/AmdPspMboxLibV1.inf
  AmdPspBaseLibV1|AgesaModulePkg/Library/AmdPspBaseLibV1/AmdPspBaseLibV1.inf
  AmdPspBaseLibV2|AgesaModulePkg/Library/AmdPspBaseLibV2/AmdPspBaseLibV2.inf
  AmdPspMboxLibV2|AgesaModulePkg/Library/AmdPspMboxLibV2/AmdPspMboxLibV2.inf
  AmdPspApobLib|AgesaModulePkg/Library/AmdPspApobLib/AmdPspApobLib.inf
  AmdPspFlashAccLib|AgesaModulePkg/Library/AmdPspFlashAccLibNull/AmdPspFlashAccLibNull.inf

  ## CCX Lib
  CcxBaseX86Lib|AgesaModulePkg/Library/CcxBaseX86Lib/CcxBaseX86Lib.inf
  CcxBistLib|AgesaModulePkg/Library/CcxBistLib/CcxBistLib.inf
  CcxPspLib|AgesaModulePkg/Library/CcxPspLib/CcxPspLib.inf
  CcxHaltLib|AgesaModulePkg/Library/CcxHaltLib/CcxHaltLib.inf
  CcxMicrocodePatchLib|AgesaModulePkg/Library/CcxMicrocodePatchLib/CcxMicrocodePatchLib.inf
  CcxRolesLib|AgesaModulePkg/Library/CcxRolesX86Lib/CcxRolesX86Lib.inf
  CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesZpLib/CcxResetTablesZpLib.inf
  CcxSetMcaLib|AgesaModulePkg/Library/CcxSetMcaZpLib/CcxSetMcaZpLib.inf
  CcxPstatesLib|AgesaModulePkg/Library/CcxPstatesZenZpLib/CcxPstatesZenZpLib.inf
  CcxSmbiosLib|AgesaModulePkg/Library/CcxSmbiosZenZpLib/CcxSmbiosZenZpLib.inf
  CcxStallLib|AgesaModulePkg/Library/CcxStallZenLib/CcxStallZenLib.inf
  DxeCcxBaseX86ServicesLib|AgesaModulePkg/Library/DxeCcxBaseX86ServicesLib/DxeCcxBaseX86ServicesLib.inf

  ## DF Lib
  FabricPstatesLib|AgesaModulePkg/Library/FabricPstatesZpLib/FabricPstatesZpLib.inf
  BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
  PeiFabricTopologyServicesLib|AgesaModulePkg/Library/PeiFabricTopologyServicesLib/PeiFabricTopologyServicesLib.inf
  DxeFabricTopologyServicesLib|AgesaModulePkg/Library/DxeFabricTopologyServicesLib/DxeFabricTopologyServicesLib.inf
  SmmFabricTopologyServicesLib|AgesaModulePkg/Library/SmmFabricTopologyServicesLib/SmmFabricTopologyServicesLib.inf
  FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf

  ## MEM Lib
  AmdMemBaseLib|AgesaModulePkg/Library/Mem/BaseLib/AmdMemBaseLib.inf

  ## Gnb Lib
  GnbCommonLib|AgesaModulePkg/Library/GnbCommonLib/GnbCommonLib.inf
  GnbMemAccLib|AgesaModulePkg/Library/GnbMemAccLib/GnbMemAccLib.inf
  GnbIoAccLib|AgesaModulePkg/Library/GnbIoAccLib/GnbIoAccLib.inf
  GnbPciAccLib|AgesaModulePkg/Library/GnbPciAccLib/GnbPciAccLib.inf
  GnbCpuAccLib|AgesaModulePkg/Library/GnbCpuAccLib/GnbCpuAccLib.inf
  GnbPciLib|AgesaModulePkg/Library/GnbPciLib/GnbPciLib.inf
  GnbLib|AgesaModulePkg/Library/GnbLib/GnbLib.inf
  NbioHandleLib|AgesaModulePkg/Library/NbioHandleLib/NbioHandleLib.inf
  NbioIommuIvrsLib|AgesaModulePkg/Library/NbioIommuIvrsLib/NbioIommuIvrsLib.inf
  PcieConfigLib|AgesaModulePkg/Library/PcieConfigLib/PcieConfigLib.inf
  GnbSSocketLib|AgesaModulePkg/Library/GnbSSocketLib/GnbSSocketLib.inf
  GnbRegisterAccZpLib|AgesaModulePkg/Library/GnbRegisterAccZpLib/GnbRegisterAccZpLib.inf
  NbioRegisterAccLib|AgesaModulePkg/Library/NbioRegisterAccLib/NbioRegisterAcc.inf
  NbioSmuV9Lib|AgesaModulePkg/Library/NbioSmuV9Lib/NbioSmuV9Lib.inf
  NbioIdsHookZPLib|AgesaModulePkg/Library/NbioIdsHookZPLib/NbioIdsHookZPLib.inf
  DxioLibV1|AgesaModulePkg/Library/DxioLibV1/DxioLibV1.inf
  PcieMiscCommLib|AgesaModulePkg/Library/PcieMiscCommLib/PcieMiscCommLib.inf
  NbioAzaliaLib|AgesaModulePkg/Library/NbioAzaliaLib/NbioAzaliaLib.inf
  SmnAccessLib|AgesaModulePkg/Library/SmnAccessLib/SmnAccessLib.inf

  ## Fch Lib
  FchBaseLib|AgesaModulePkg/Library/FchBaseLib/FchBaseLib.inf
  FchDxeLibV9|AgesaModulePkg/Library/FchDxeLib/FchDxeLib.inf
  FchSmmLibV9|AgesaModulePkg/Library/FchSmmLib/FchSmmLib.inf
  FchTaishanDxeLib|AgesaModulePkg/Fch/Taishan/FchTaishanCore/FchTaishanLibDxe.inf
  FchInitHookLib|AgesaModulePkg/Library/FchInitHookLib/FchInitHookLibDxe.inf #GR-Agesa-Modify
  FchIdsHookLib|AgesaModulePkg/Library/FchIdsHookLib/FchIdsHookLib.inf

  ## Ras Lib
  RasBaseLib|AgesaModulePkg/Library/RasBaseLib/RasBaseLib.inf
  OemAgesaCcxPlatformLib|AgesaPkg/Addendum/Ccx/OemAgesaCcxPlatformLibNull/OemAgesaCcxPlatformLibNull.inf
  OemGpioResetControlLib|AgesaPkg/Addendum/Nbio/OemGpioResetControlLibNull/OemGpioResetControlLibNull.inf

  AmdCpmBaseLib|AmdCpmPkg/Library/Proc/Base/AmdCpmBaseLib.inf
  AmdCpmCpuLib|AmdCpmPkg/Library/Proc/Cpu/AmdCpmCpu.inf
  AmdCpmFchLib|AmdCpmPkg/Library/Proc/Fch/AmdCpmFch.inf

  CbsSetAgesaPcdLib|AmdCbsPkg/Library/$(CBS_LIB_PATH)/Resource/CbsSetAgesaPcdLibZP.inf

  CbsSetupLib|AmdCbsPkg/Library/$(CBS_LIB_PATH)/CbsSetupLib.inf
!if $(IPMI_SUPPORT) == TRUE
  IpmiBaseLib|ServerCommonPkg/Library/IpmiBaseLib/IpmiBaseLib.inf
  EfiServerManagementLib|ServerCommonPkg/Library/ServerManagementLib/ServerManagementLib.inf
  PostErrorTableLib|ServerCommonPkg/Library/PostErrorTableLib/PostErrorTableLib.inf 
  IpmiSetupCommLib|ServerCommonPkg/Library/IpmiSetupCommLib/IpmiSetupCommLib.inf  
  SolSerialPortLib|ServerCommonPkg/Library/SolSerialPortLib/SolSerialPortLib.inf
!endif 
  DisplayTextModeLib|ProjectPkg/Library/DisplayTextModeLib/DisplayTextModeLib.inf
  ResetSystemLib|PcAtChipsetPkg/Library/ResetSystemLib/ResetSystemLib.inf

  BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf
  HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf

[LibraryClasses.IA32]
  FabricResourceSizeForEachDieLib|AgesaModulePkg/Library/PeiFabricResourceSizeForEachDieLib/PeiFabricResourceSizeForEachDieLib.inf
  
  
[LibraryClasses.common.PEIM]
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/PeiExtractGuidedSectionLib/PeiExtractGuidedSectionLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/PeiCryptLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxPeiLib.inf
  BiosIdLib|ByoModulePkg/Library/BiosIdLib/Pei/BiosIdPeiLib.inf
  MultiPlatSupportLib|ByoModulePkg/Library/PeiMultiPlatSupportLib/PeiMultiPlatSupportLib.inf  
# MpInitLib|UefiCpuPkg/Library/MpInitLib/PeiMpInitLib.inf
  Tcg2PhysicalPresenceLib|SecurityPkg/Library/PeiTcg2PhysicalPresenceLib/PeiTcg2PhysicalPresenceLib.inf
  Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2DeviceLibDTpm.inf
!if $(SOURCE_DEBUG_ENABLE) == TRUE
  DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/SecPeiDebugAgentLib.inf
!endif

  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapPeiLib/AmdHeapPeiLib.inf
  IdsLib|AgesaModulePkg/Library/IdsPeiLib/IdsPeiLib.inf
  FchPeiLibV9|AgesaModulePkg/Library/FchPeiLib/FchPeiLib.inf
  FchTaishanPeiLib|AgesaModulePkg/Fch/Taishan/FchTaishanCore/FchTaishanLibPei.inf
  FchInitHookLib|AgesaModulePkg/Library/FchInitHookLib/FchInitHookLibPei.inf
  GnbHeapLib|AgesaModulePkg/Library/GnbHeapPeiLib/GnbHeapPeiLib.inf
  AmdPspCommonLib|AgesaModulePkg/Library/AmdPspCommonLibPei/AmdPspCommonLibPei.inf
  PeiSocLogicalIdServicesLib|AgesaModulePkg/Library/PeiSocLogicalIdServicesLib/PeiSocLogicalIdServicesLib.inf
  AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Pei/AmdTableSmnPeiLib.inf
  AmdCapsuleLib|AgesaModulePkg/Library/AmdCapsuleLibPei/AmdCapsuleLibPei.inf
  PeiSocBistLoggingLib|AgesaModulePkg/Library/PeiSocBistLoggingLib/PeiSocBistLoggingLib.inf
  ## MEM Libs
  AmdMemSmbiosV2Lib|AgesaModulePkg/Library/MemSmbiosV2Lib/MemSmbiosV2Lib.inf

  ## IDS LIB
  CcxZenIdsHookLibPei|AgesaModulePkg/Library/CcxZenZpIdsHookLib/Pei/CcxZenZpIdsHookLibPei.inf
  FabricIdsHookZpLibPei|AgesaModulePkg/Library/FabricIdsHookZpLib/Pei/FabricIdsHookZpLibPei.inf

  CbsPeiFuncLib|AmdCbsPkg/Library/$(CBS_LIB_PATH)/CbsFuncLibPei.inf
  AmdIdsHookExtLib|AmdCbsPkg/Library/$(CBS_LIB_PATH)/CbsIdsLibPei.inf

  AmdCalloutLib|AgesaModulePkg/Library/AmdCalloutLib/AmdCalloutLib.inf
  CcxSetMmioCfgBaseLib|AgesaModulePkg/Library/CcxSetMmioCfgBaseLib/CcxSetMmioCfgBaseLib.inf
  
  
  
 
[LibraryClasses.common.SEC]
!if ($(DEBUG_MESSAGE_ENABLE) == TRUE) OR ($(SOURCE_DEBUG_ENABLE) == TRUE)
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  PlatformHookLib|ProjectPkg/Library/PlatformHookLib/PlatformHookLib.inf
!else
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
!endif

!if $(SOURCE_DEBUG_ENABLE) == TRUE
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/SecPeiDebugAgentLib.inf
!endif



[LibraryClasses.IA32.PEI_CORE]
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapPeiLib/AmdHeapPeiLib.inf
  AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Pei/AmdTableSmnPeiLib.inf

  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf  
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf  
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf  
!if $(PERFORMANCE_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/PeiPerformanceLib/PeiPerformanceLib.inf
  TimerLib|$(PLATFORM_PACKAGE)/Library/CpuLocalApicTimerLib/CpuLocalApicTimerLib.inf
!endif

[LibraryClasses.X64]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxDxeLib.inf
  SpiFlashLib|$(PLATFORM_PACKAGE)/Override/ByoNvMediaPkg/Library/SpiFlashLib/SpiFlashLib.inf
  BiosIdLib|ByoModulePkg/Library/BiosIdLib/Dxe/BiosIdDxeLib.inf
  VarCheckLib|MdeModulePkg/Library/VarCheckLib/VarCheckLib.inf
# MpInitLib|UefiCpuPkg/Library/MpInitLib/DxeMpInitLib.inf
  MpInitLib|ByoModulePkg/Library/MpInitLib/DxeMpInitLib.inf
  PciHostBridgeLib|MdeModulePkg/Library/PciHostBridgeLibNull/PciHostBridgeLibNull.inf
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/DxeCpuExceptionHandlerLib.inf

  AmdHeapLib|AgesaModulePkg/Library/AmdHeapDxeLib/AmdHeapDxeLib.inf
  AmdBaseLib|AgesaModulePkg/Library/AmdBaseLib/AmdBaseLib.inf
  FchInitHookLib|AgesaModulePkg/Library/FchInitHookLib/FchInitHookLibDxe.inf

!if $(SECURE_BOOT_ENABLE) == TRUE
  PlatformSecureLib|ByoModulePkg/Library/PlatformSecureLib/PlatformSecureLib.inf
  AuthVariableLib|SecurityPkg/Library/AuthVariableLib/AuthVariableLib.inf
!else
  AuthVariableLib|MdeModulePkg/Library/AuthVariableLibNull/AuthVariableLibNull.inf
!endif

  SetupUiLib|ByoModulePkg/Library/SetupUiLib/SetupUiLib.inf
  Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibTcg2/Tpm2DeviceLibTcg2.inf
  FmpAuthenticationLib|SecurityPkg/Library/FmpAuthenticationLibPkcs7/FmpAuthenticationLibPkcs7.inf

!if $(SOURCE_DEBUG_ENABLE) == TRUE
  DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/DxeDebugAgentLib.inf
!endif

  FabricResourceSizeForEachDieLib|AgesaModulePkg/Library/DxeFabricResourceSizeForEachDieLib/DxeFabricResourceSizeForEachDieLib.inf

# CcxIdsCustomPstatesLib|AgesaModulePkg/Library/CcxIdsCustomPstateNullLib/CcxIdsCustomPstatesNullLib.inf  
  CcxIdsCustomPstatesLib|AgesaModulePkg/Library/CcxIdsCustomPstateZenLib/CcxIdsCustomPstatesZenLib.inf



[LibraryClasses.common.DXE_DRIVER]
  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/S3Save/AmdS3SaveLib.inf
  IdsLib|AgesaModulePkg/Library/IdsDxeLib/IdsDxeLib.inf
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapDxeLib/AmdHeapDxeLib.inf
  GnbHeapLib|AgesaModulePkg/Library/GnbHeapDxeLib/GnbHeapDxeLib.inf
  AmdPspCommonLib|AgesaModulePkg/Library/AmdPspCommonLibDxe/AmdPspCommonLibDxe.inf
  AmdPspFlashAccLib|AgesaModulePkg/Library/AmdPspFlashAccLibDxe/AmdPspFlashAccLibDxe.inf
  AmdPspFlashUpdateLib|AgesaModulePkg/Library/AmdPspFlashUpdateLib/AmdPspFlashUpdateLib.inf
  CcxZenIdsHookLibDxe|AgesaModulePkg/Library/CcxZenZpIdsHookLib/Dxe/CcxZenZpIdsHookLibDxe.inf
  ApcbLib|AgesaModulePkg/Library/ApcbLib/ApcbLib.inf
  DxeSocLogicalIdServicesLib|AgesaModulePkg/Library/DxeSocLogicalIdServicesLib/DxeSocLogicalIdServicesLib.inf
  AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Dxe/AmdTableSmnDxeLib.inf
  AmdCapsuleLib|AgesaModulePkg/Library/AmdCapsuleLibDxe/AmdCapsuleLibDxe.inf
  CcxMpServicesLib|AgesaModulePkg/Library/CcxMpServicesDxeLib/CcxMpServicesDxeLib.inf

  ## IDS LIB
  FabricIdsHookZpLibDxe|AgesaModulePkg/Library/FabricIdsHookZpLib/Dxe/FabricIdsHookZpLibDxe.inf
  FchInitHookLib|AgesaModulePkg/Library/FchInitHookLib/FchInitHookLibDxe.inf


  CbsDxeFuncLib|AmdCbsPkg/Library/$(CBS_LIB_PATH)/CbsFuncLibDxe.inf
  AmdIdsHookExtLib|AmdCbsPkg/Library/$(CBS_LIB_PATH)/CbsIdsLibDxe.inf
  HstiLib|MdePkg/Library/DxeHstiLib/DxeHstiLib.inf
  RasElogEventLib|ServerCommonPkg/Library/RasElogEventLib/DxeRasElogEventLib.inf

[LibraryClasses.X64.PEIM]
!if $(DEBUG_MESSAGE_ENABLE) == FALSE
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!else
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
!endif
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/PeiCpuExceptionHandlerLib.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
!if $(SOURCE_DEBUG_ENABLE) == TRUE
  DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/SecPeiDebugAgentLib.inf
!endif
  

[LibraryClasses.X64.DXE_CORE]
  HobLib|MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
!if $(PERFORMANCE_ENABLE) == TRUE
  PerformanceLib|MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf
  TimerLib|$(PLATFORM_PACKAGE)/Library/CpuLocalApicTimerLib/CpuLocalApicTimerLib.inf
!endif
!if $(SOURCE_DEBUG_ENABLE) == TRUE
  DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/DxeDebugAgentLib.inf
!endif

[LibraryClasses.X64.DXE_SMM_DRIVER]
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
  MemoryAllocationLib|MdePkg/Library/SmmMemoryAllocationLib/SmmMemoryAllocationLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxSmmLib.inf
  SetMemAttributeSmmLib|$(PLATFORM_PACKAGE)/Library/SetMemAttributeSmmLib/SetMemAttributeSmmLib.inf
  SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/SmmCpuExceptionHandlerLib.inf
  Tcg2PhysicalPresenceLib|SecurityPkg/Library/SmmTcg2PhysicalPresenceLib/SmmTcg2PhysicalPresenceLib.inf

  AmdHeapLib|AgesaModulePkg/Library/AmdHeapDxeLib/AmdHeapDxeLib.inf
  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/S3Save/AmdS3SaveLib.inf
#@todo add specifiic SMM Lib instance, DXE Lib is depend on gBS service exisitance
  IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  AmdPspCommonLib|AgesaModulePkg/Library/AmdPspCommonLibDxe/AmdPspCommonLibDxe.inf
  AmdPspFlashUpdateLib|AgesaModulePkg/Library/AmdPspFlashUpdateLib/AmdPspFlashUpdateLib.inf
  CcxZenIdsHookLibSmm|AgesaModulePkg/Library/CcxZenZpIdsHookLib/Smm/CcxZenZpIdsHookLibSmm.inf
  ApcbLib|AgesaModulePkg/Library/ApcbLib/ApcbLib.inf
  AmdCapsuleLib|AgesaModulePkg/Library/AmdCapsuleLibDxe/AmdCapsuleLibDxe.inf
  AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Dxe/AmdTableSmnDxeLib.inf
  CcxMpServicesLib|AgesaModulePkg/Library/CcxMpServicesSmmLib/CcxMpServicesSmmLib.inf
  RasSmmLib|AgesaModulePkg/Library/RasSmmLib/RasSmmLib.inf
  
  AmdPspFlashAccLib|PlatformPkg/AmdPspFlashAccLibSmm/AmdPspFlashAccLibSmm.inf

  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/SmmCryptLib.inf

!if $(SOURCE_DEBUG_ENABLE) == TRUE
  DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/SmmDebugAgentLib.inf
!endif

  HstiLib|MdePkg/Library/DxeHstiLib/DxeHstiLib.inf
!if $(IPMI_SUPPORT) == TRUE
  IpmiBaseLib|ServerCommonPkg/Library/SmmIpmiBaseLib/SmmIpmiBaseLib.inf
!endif
  RasElogEventLib|ServerCommonPkg/Library/RasElogEventLib/SmmRasElogEventLib.inf


[LibraryClasses.X64.SMM_CORE]
  MemoryAllocationLib|MdeModulePkg/Library/PiSmmCoreMemoryAllocationLib/PiSmmCoreMemoryAllocationLib.inf
  SmmServicesTableLib|MdeModulePkg/Library/PiSmmCoreSmmServicesTableLib/PiSmmCoreSmmServicesTableLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
  SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/SmmCryptLib.inf
  HstiLib|MdePkg/Library/DxeHstiLib/DxeHstiLib.inf


[LibraryClasses.X64.DXE_RUNTIME_DRIVER]
  ReportStatusCodeLib|MdeModulePkg/Library/RuntimeDxeReportStatusCodeLib/RuntimeDxeReportStatusCodeLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/RuntimeCryptLib.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibFmp/DxeRuntimeCapsuleLib.inf
  HstiLib|MdePkg/Library/DxeHstiLib/DxeHstiLib.inf
  

[LibraryClasses.X64.UEFI_DRIVER]


[LibraryClasses.X64.UEFI_APPLICATION]
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  HstiLib|MdePkg/Library/DxeHstiLib/DxeHstiLib.inf

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag.common]
  gEfiByoModulePkgTokenSpaceGuid.PcdStatusCodeUseOem|FALSE
  gEfiByoModulePkgTokenSpaceGuid.PcdStatusCodePrintProgressCode|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdUgaConsumeSupport|FALSE

  gEfiMdeModulePkgTokenSpaceGuid.PcdFrameworkCompatibilitySupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreImageLoaderSearchTeSectionFirst|FALSE
!if $(DEBUG_MESSAGE_ENABLE) == FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|FALSE
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|TRUE
!endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseMemory|FALSE
!if $(POSTCODE_STATUS_CODE_ENABLE) == TRUE
  gEfiByoModulePkgTokenSpaceGuid.PcdStatusCodeUsePostCode|TRUE
!else
  gEfiByoModulePkgTokenSpaceGuid.PcdStatusCodeUsePostCode|FALSE
!endif
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

!if $(UART_SELECTION) == 1   
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdIsaSerialSetRTSAndDTRWhenReset|TRUE
!endif  

!if $(UART_SELECTION) == 2   
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdIsaSerialSetRTSAndDTRWhenReset|TRUE
!endif    


[PcdsFixedAtBuild.common]
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdFchOemBeforePciRestoreSwSmi|0xB3
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdFchOemAfterPciRestoreSwSmi|0xB4
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdFchOemEnableAcpiSwSmi|0xA0
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdFchOemDisableAcpiSwSmi|0xA1
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdFchOemSpiUnlockSwSmi|0xB7
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdFchOemSpiLockSwSmi|0xB8
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdFchOemStartTimerSmi|0xB5
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdFchOemStopTimerSmi|0xB6
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFchCfgAcpiPm1EvtBlkAddr|0x0800
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFchCfgAcpiPm1CntBlkAddr|0x804
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFchCfgAcpiPmTmrBlkAddr|0x808
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFchCfgCpuControlBlkAddr|0x810
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFchCfgAcpiGpe0BlkAddr|0x820
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosT16MaximumCapacity|0x80000000

  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdAcpiTableHeaderOemId|HIGON|VOID*|0x6
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdAcpiTableHeaderOemTableId|Higon |VOID*|0x9
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosT16ExtMaximumCapacity|0x0000040000000000
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdAcpiCpuSsdtProcessorScopeInSb|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdNumberOfPhysicalSocket|2
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintFilter|0x100401008A300408
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintHdtOutEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintHdtOutForceEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintRedirectIOEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPortEnable|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPortDetectCableConnection|FALSE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPspApcbRecoveryEnable|FALSE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPspRecoveryFlagDetectEnable|FALSE

!if $(UART_SELECTION) == 0 
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPort|0xfedc9000
!endif  
!if $(UART_SELECTION) == 2 
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPort|0xfedca000
!endif 

  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0xF0000000
# gEfiMdePkgTokenSpaceGuid.PcdPciExpressLength|0x8000000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVendor|L"Byosoft"
  
!if $(SECURE_BOOT_ENABLE) == TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x10000
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x4000
!endif

  gEfiMdeModulePkgTokenSpaceGuid.PcdHwErrStorageSize|0x00000800
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxHardwareErrorVariableSize|0x400
  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreMaxPeimPerFv|96
  gEfiMdeModulePkgTokenSpaceGuid.PcdAriSupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreMaxPpiSupported|160
  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|FALSE

!if $(DEBUG_MESSAGE_ENABLE) == FALSE
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x0
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x03
!else
!if $(SOURCE_DEBUG_ENABLE) == TRUE
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x17
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2F
!endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseHardwareFlowControl|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07
!endif

!if $(SOURCE_DEBUG_ENABLE) == TRUE
  gEfiSourceLevelDebugPkgTokenSpaceGuid.PcdDebugLoadImageMethod|0x2
!endif

!if $(PERFORMANCE_ENABLE) == TRUE
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0x1
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxPeiPerformanceLogEntries|30
!endif
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdEbdaReservedMemorySize|0x20000
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdHighPmmMemorySize|0x9600000
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdBiosVideoCheckVbeEnable|TRUE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdBiosVideoCheckVgaEnable|TRUE

  gPlatformModuleTokenSpaceGuid.PcdTemporaryRamBase|0x00030000
  gPlatformModuleTokenSpaceGuid.PcdTemporaryRamSize|0x00040000 
  gPlatformModuleTokenSpaceGuid.PcdPeiTemporaryRamStackSize|0                 # auto
  
  gEfiMdeModulePkgTokenSpaceGuid.PcdShadowPeimOnS3Boot|TRUE
  
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemId|"_BYO_ "
  
  gEfiByoModulePkgTokenSpaceGuid.AcpiIoPortBaseAddress|0x800

  gEfiMdeModulePkgTokenSpaceGuid.PcdDefer3rdPartyImageLoadBeforeEndOfDxe|FALSE
  gEfiByoModulePkgTokenSpaceGuid.PcdSetupMouseEnable|FALSE

  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFchCfgSmiCmdPortAddr|0xB2 
  gEfiByoModulePkgTokenSpaceGuid.PcdSwSmiCmdPort|0xB2  

  gEfiByoModulePkgTokenSpaceGuid.PcdLegacyUsbPciScanMaxBus|0x7F
  
  gEfiMdePkgTokenSpaceGuid.PcdUefiVariableDefaultPlatformLangCodes|"zh-Hans;en-US"
  gEfiMdePkgTokenSpaceGuid.PcdUefiVariableDefaultPlatformLang|"zh-Hans"
  
  ## Default OEM Table ID for ACPI table creation, it is "BYOSOFT ".
  # @Prompt Default OEM Table ID for ACPI table creation.
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemTableId|0x2054464F534F5942
  ## Default Creator ID for ACPI table creation.
  # @Prompt Default Creator ID for ACPI table creation.
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorId|0x204F5942  
  gEfiByoModulePkgTokenSpaceGuid.PcdBdsBootOrderUpdateMethod|2
  
  gEfiByoModulePkgTokenSpaceGuid.PcdOemCopyRightLineNumber|1

  gPlatformModuleTokenSpaceGuid.PcdUartSelection|$(UART_SELECTION)
  gPlatformModuleTokenSpaceGuid.PcdTestBiosVersion|$(TEST_BIOS_VERSION)

!if $(TCM_ENABLE) == TRUE OR $(TPM_ENABLE) == TRUE
  gPlatformModuleTokenSpaceGuid.PcdTpmDecode|$(TPM_DECODE)
!else
  gPlatformModuleTokenSpaceGuid.PcdTpmDecode|0
!endif  

# gUefiCpuPkgTokenSpaceGuid.PcdCpuMaxLogicalProcessorNumber|128
  
  gPlatformModuleTokenSpaceGuid.PcdSmbiosBiosVersion|"$(BIOS_VERSION_NAME)"
  gPlatformModuleTokenSpaceGuid.PcdSmbiosManufacturer|"Inspur"
  
#  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAgesaAssertEnable|FALSE

  gEfiByoModulePkgTokenSpaceGuid.PcdSetupDataVersionCheck|TRUE
  
 
[PcdsFixedAtBuild.X64]
  gEfiNvMediaDeviceTokenSpaceGuid.PcdNvMediaDeviceNumbers|0x1
  gPcAtChipsetPkgTokenSpaceGuid.Pcd8259LegacyModeMask|0x0eB8
  gPcAtChipsetPkgTokenSpaceGuid.PcdMaximalValidYear|2099
  gPcAtChipsetPkgTokenSpaceGuid.PcdMinimalValidYear|2020 






[PcdsPatchableInModule.common]
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80080046
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdLegacyBiosCacheLegacyRegion|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareRevision|0x00010000
  gEfiByoModulePkgTokenSpaceGuid.PcdBiosFileExt|L"bin"

[PcdsDynamicHii.common.DEFAULT]
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdBootState|L"BootState"|gEfiBootStateGuid|0x0|FALSE
  gEfiMdePkgTokenSpaceGuid.PcdHardwareErrorRecordLevel|L"HwErrRecSupport"|gEfiGlobalVariableGuid|0x0|1 # Variable: L"HwErrRecSupport"

[PcdsDynamicDefault.common.DEFAULT]
  gEfiMdeModulePkgTokenSpaceGuid.PcdS3BootScriptTablePrivateDataPtr|0x0
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|1024
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|768

  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInstanceGuid|{0xb6, 0xe5, 0x01, 0x8b, 0x19, 0x4f, 0xe8, 0x46, 0xab, 0x93, 0x1c, 0x53, 0x67, 0x1b, 0x90, 0xcc}
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2InitializationPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2SelfTestPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2ScrtmPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInitializationPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmScrtmPolicy|1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2HashMask|0x12
  gEfiSecurityPkgTokenSpaceGuid.PcdTcg2HashAlgorithmBitmap|0x12
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdCStateIoBaseAddress|0x813
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdIvrsControl|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFchApuRasSmiSupport|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataRasSupport|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdNbioPoisonConsumption|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdNbioRASControl|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgFchIoapicId|0x80
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgGnbIoapicId|0x81
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmtMode|0x1
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSmuFeatureControlDefines|0x0000
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdLegacyFree|1
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdNoneSioKbcSupport|1
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdResetMode|0x7
  
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataEspPort0|0x01
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataEspPort1|0x01
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataEspPort2|0x01
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataEspPort3|0x01
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataEspPort4|0x01
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataEspPort5|0x01
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataEspPort6|0x01
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataEspPort7|0x01
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataMultiDiePortESP|0xFFFFFFFFFFFFFFFF

  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoHorizontalResolution|1024
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoVerticalResolution|768
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutColumn|128
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutRow|40

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdFchOscout1ClkContinous|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdUse1GPageTable|TRUE

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXhciPlatformPortDisableMap|{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

  gEfiMdeModulePkgTokenSpaceGuid.PcdSrIovSupport|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdCfgPcieAriSupport|TRUE
  
  gEfiByoModulePkgTokenSpaceGuid.PcdSkipInputStringFlag|1
  gEfiMdeModulePkgTokenSpaceGuid.PcdUnilcRank|0

  
!if $(TARGET_SVID_SUPPORT) == TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdPcieSubsystemDeviceID|$(BOARD_SSID)
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdPcieSubsystemVendorID|$(BOARD_SVID)
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdCfgGnbPcieSSID|$(BOARD_SSID_SVID)
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdCfgGnbIGPUSSID|$(BOARD_SSID_SVID)
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdCfgGnbHDAudioSSID|$(BOARD_SSID_SVID)
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdCfgNbioSsid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdCfgIommuSsid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdCfgPspccpSsid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdCfgNtbccpSsid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdCfgXgbeSsid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdCfgNbifF0Ssid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdCfgNbifRCSsid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdCfgNtbSsid|$(BOARD_SSID_SVID)

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSmbusSsid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataAhciSsid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataRaidSsid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataRaid5Ssid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataIdeSsid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdEhciSsid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXhciSsid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdLpcSsid|$(BOARD_SSID_SVID)
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSdSsid|$(BOARD_SSID_SVID) 
!endif  


#HYGON_65N32 = 5  
  gAmdCpmPkgTokenSpaceGuid.PcdPlatformSelect|13
  
  
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataSgpio0|1
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataDisableGenericMode|1
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataAhciEnclosureManagement|1
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataSgpioMultiDieEnable|0xFF
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgPcieHotplugSupport|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgHotplugMode|1
  


  
#------------------------------------------------------------------------------
[Components.IA32]
  ByoModulePkg/Universal/Dummy/DummyPeiCore/DummyPeiMain.inf
  $(PLATFORM_PACKAGE)/SecCore/SecCore.inf
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
 
  $(PLATFORM_PACKAGE)/PlatformEarlyPei/PlatformEarlyPei.inf {
  <LibraryClasses>
!if $(DEBUG_MESSAGE_ENABLE) == TRUE
    DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
  }

!if $(IPMI_SUPPORT) == TRUE
    ServerCommonPkg/Ipmi/GenericPei/PeiGenericIpmi.inf
!endif
  
  ProjectPkg/ProjectEarlyPei/ProjectEarlyPei.inf {
  <LibraryClasses>
!if $(DEBUG_MESSAGE_ENABLE) == TRUE
    DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!else
    PlatformHookLib|ProjectPkg/Library/PlatformHookLib/PlatformHookLib.inf    
!endif
  }
  
  $(PLATFORM_PACKAGE)/BootModePei/BootModePei.inf
  
  MdeModulePkg/Universal/ReportStatusCodeRouter/Pei/ReportStatusCodeRouterPei.inf
  ByoModulePkg/StatusCodeHandler/Pei/StatusCodeHandlerPei.inf

  MdeModulePkg/Universal/FaultTolerantWritePei/FaultTolerantWritePei.inf
  MdeModulePkg/Universal/Variable/Pei/VariablePei.inf
  
  $(PLATFORM_PACKAGE)/PlatformPei/PlatformPei.inf

  UefiCpuPkg/PiSmmCommunication/PiSmmCommunicationPei.inf

  $(PLATFORM_PACKAGE)/Cpu/Pei/CpuPei.inf
  $(PLATFORM_PACKAGE)/MemoryInit/MemDetect.inf
  AgesaModulePkg/Mem/AgesaMemPei/AgesaMemPei.inf

  AgesaModulePkg/Debug/AmdIdsDebugPrintPei/AmdIdsDebugPrintPei.inf

  ##PSP Drivers
  AgesaModulePkg/Psp/AmdPspPeiV1/AmdPspPeiV1.inf
  AgesaModulePkg/Psp/AmdPspPeiV2/AmdPspPeiV2.inf
  AgesaModulePkg/Ccx/Zen/CcxZenZpPei/AmdCcxZenZpPei.inf {
  <LibraryClasses>
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesZpLib/CcxResetTablesZpLib.inf
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  }
  AgesaModulePkg/Fabric/ZP/FabricZpPei/AmdFabricZpPei.inf {
  <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  }
  AgesaModulePkg/Nbio/SMU/AmdNbioSmuV9Pei/AmdNbioSmuV9Pei.inf
  AgesaModulePkg/Nbio/PCIe/AmdNbioPcieZPPei/AmdNbioPcieZPPei.inf
  AgesaModulePkg/Nbio/NbioBase/AmdNbioBaseZPPei/AmdNbioBaseZPPei.inf
  AgesaModulePkg/Nbio/IOMMU/AmdNbioIOMMUZPPei/AmdNbioIOMMUZPPei.inf
  AgesaModulePkg/Nbio/IOAPIC/AmdNbioIOAPICZPPei/AmdNbioIOAPICZPPei.inf
  AgesaModulePkg/Fch/Taishan/FchTaishanPei/FchPei.inf
  AgesaModulePkg/Fch/Taishan/FchTaishanSmbusPei/Smbus.inf
  AgesaModulePkg/Fch/Taishan/FchTaishanMultiFchPei/FchMultiFchPei.inf
  AgesaModulePkg/Mem/AmdMemAm4Pei/AmdMemAm4Pei.inf
  AgesaModulePkg/Soc/AmdSocSp3ZpPei/AmdSocSp3ZpPei.inf {
  <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerZenZpLib/FabricResourceInitZenZpLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistZpLib/PeiSocBistZpLib.inf
  }
  AgesaModulePkg/Mem/AmdMemChanXLatZpPei/MemChanXLatZpPei.inf
  AgesaModulePkg/Mem/AmdMemSmbiosV2Pei/MemSmbiosV2Pei.inf
  AgesaModulePkg/Mem/AmdMbistPei/AmdMbistPei.inf
  AgesaModulePkg/ErrorLog/AmdErrorLogPei/AmdErrorLogPei.inf
  AgesaModulePkg/Universal/AmdMemoryHobInfoPeim/AmdMemoryHobInfoPeim.inf
  AgesaModulePkg/Universal/Version/AmdVersionPei/AmdVersionPei.inf

  ProjectPkg/AmdCpmOemInitPei/AmdCpmOemInitPeimHygon52D16.inf
# AmdCpmPkg/Addendum/Oem/AmdSataWorkaround/AmdSataWorkaround.inf

  AmdCpmPkg/Kernel/Pei/AmdCpmInitPeim.inf
# AmdCpmPkg/Features/BoardId/Pei/AmdBoardIdPei.inf
  AmdCpmPkg/Features/GpioInit/Pei/AmdCpmGpioInitPeim.inf
  AmdCpmPkg/Features/I2cMaster/Pei/AmdI2cMasterPei.inf
  AmdCpmPkg/Features/PcieInit/Pei/AmdCpmPcieInitPeim.inf
#  AmdCpmPkg/Devices/Ds125Br401a/Pei/Ds125Br401aPei.inf
   AmdCpmPkg/Devices/M24LC128/Pei/M24Lc128Pei.inf
#  AmdCpmPkg/Devices/Pca9535a/Pei/Pca9535aPei.inf
#  AmdCpmPkg/Devices/Pca9545a/Pei/Pca9545aPei.inf
#  AmdCpmPkg/Devices/Sff8472/Pei/Sff8472Pei.inf
  
  AmdCbsPkg/CbsBasePei/CbsBasePei.inf
	ProjectPkg/Feature/UsbEq/UsbTxEqPei.inf

  MdeModulePkg/Universal/PcatSingleSegmentPciCfg2Pei/PcatSingleSegmentPciCfg2Pei.inf
# UefiCpuPkg/Universal/Acpi/S3Resume2Pei/S3Resume2Pei.inf
# $(PLATFORM_PACKAGE)/Override/MdeModulePkg/Universal/CapsulePei/CapsulePei.inf
  MdeModulePkg/Core/DxeIplPeim/DxeIpl.inf
  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTablePei/FirmwarePerformancePei.inf

!if $(RECOVERY_ENABLE) == TRUE
  $(PLATFORM_PACKAGE)/PlatformPei/Usb/UsbController.inf
  ByoModulePkg/Universal/Disk/FatPei/FatPei.inf
  ByoModulePkg/Bus/Ata/AtaBusPei/AtaBusPei.inf
  ByoModulePkg/CrisisRecovery/ModuleRecoveryPei/ModuleRecoveryPei.inf
  MdeModulePkg/Bus/Usb/UsbBotPei/UsbBotPei.inf
  MdeModulePkg/Bus/Usb/UsbBusPei/UsbBusPei.inf
  MdeModulePkg/Bus/Pci/XhciPei/XhciPei.inf
# MdeModulePkg/Bus/Pci/EhciPei/EhciPei.inf
!if $(RECOVERY_CDROM_ENABLE) == TRUE
  MdeModulePkg/Bus/Pci/IdeBusPei/IdeBusPei.inf                    # for ATAPI
  ByoModulePkg/Universal/Disk/CDExpressPei/CdExpressPei.inf
!endif
!endif

!if $(TCM_ENABLE) == TRUE
  ByoModulePkg/Security/Tcm/Pei/TcmPei.inf
!endif

!if $(TPM_ENABLE) == TRUE
  SecurityPkg/Tcg/Tcg2Config/Tcg2ConfigPei.inf {
  <LibraryClasses>
      Tpm12CommandLib|SecurityPkg/Library/Tpm12CommandLib/Tpm12CommandLib.inf
      Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibDTpm/Tpm12DeviceLibDTpm.inf
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2DeviceLibDTpm.inf
  }

  SecurityPkg/Tcg/TcgPei/TcgPei.inf {
  <LibraryClasses>
      Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibDTpm/Tpm12DeviceLibDTpm.inf
      Tpm12CommandLib|SecurityPkg/Library/Tpm12CommandLib/Tpm12CommandLib.inf
  }
  
  SecurityPkg/Tcg/PhysicalPresencePei/PhysicalPresencePei.inf  
  
  SecurityPkg/Tcg/Tcg2Pei/Tcg2Pei.inf {
  <LibraryClasses>
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterPei.inf
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
      NULL|PlatformPkg/Library/Tpm2InstanceLibHgfTpm/Tpm2InstanceLibHgfTpm.inf  
      NULL|SecurityPkg/Library/HashInstanceLibSm3/HashInstanceLibSm3.inf        
     # NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf      
      HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterPei.inf
  }
!endif

  HygonWrapperPkg/CpmWrapper/Features/BoardId/Pei/HygonBoardIdPei.inf

#!if $(HDD_PASSWORD_ENABLE) == TRUE
#  ByoModulePkg/Security/HddPassword/Pei/HddPasswordPei.inf
#!endif

[Components.X64]
  MdeModulePkg/Universal/SmmCommunicationBufferDxe/SmmCommunicationBufferDxe.inf

  MdeModulePkg/Core/Dxe/DxeMain.inf {
  <LibraryClasses>
      NULL|IntelFrameworkModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
      NULL|ByoModulePkg/Library/GuidNameLib/GuidNameLib.inf
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
  }

!if $(SECURE_BOOT_ENABLE) == TRUE  
  ByoModulePkg/Security/SecureBootConfigDxe/SecureBootConfigDxe.inf  
!endif  

  ByoModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  ByoModulePkg/Universal/SmbiosSmm/SmbiosSmm.inf

  PcAtChipsetPkg/8259InterruptControllerDxe/8259.inf

  MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  MdeModulePkg/Universal/DriverHealthManagerDxe/DriverHealthManagerDxe.inf
  
  IntelFrameworkModulePkg/Universal/CpuIoDxe/CpuIoDxe.inf
  UefiCpuPkg/CpuIo2Dxe/CpuIo2Dxe.inf

  PlatformPkg/Cpu/Dxe/CpuDxe.inf
# UefiCpuPkg/CpuDxe/CpuDxe.inf

  MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  PcAtChipsetPkg/8254TimerDxe/8254Timer.inf
  MdeModulePkg/Universal/Metronome/Metronome.inf
  ByoModulePkg/Setup/SystemPassword/SystemPassword.inf
  ByoModulePkg/PostLogin/PostLogin.inf
  ByoModulePkg/Setup/HiiDatabaseDxe/HiiDatabaseDxe.inf  
  $(PLATFORM_PACKAGE)/Override/PcAtChipsetPkg/PcatRealTimeClockRuntimeDxe/PcatRealTimeClockRuntimeDxe.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  MdeModulePkg/Universal/EbcDxe/EbcDxe.inf
!if $(S3_ENABLE) == TRUE
  MdeModulePkg/Universal/Acpi/S3SaveStateDxe/S3SaveStateDxe.inf
  MdeModulePkg/Universal/LockBox/SmmLockBox/SmmLockBox.inf {
  <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }
  IntelFrameworkModulePkg/Universal/Acpi/AcpiS3SaveDxe/AcpiS3SaveDxe.inf
  MdeModulePkg/Universal/Acpi/BootScriptExecutorDxe/BootScriptExecutorDxe.inf {   # NVS
  <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  }
!endif

  ByoModulePkg/BdsDxe/BdsDxe.inf
  
  ByoModulePkg/Console/ConPlatformDxe/ConPlatformDxe.inf
  ByoModulePkg/Console/ConSplitterDxe/ConSplitterDxe.inf
  ByoModulePkg/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf

  $(PLATFORM_PACKAGE)/PlatformEarlyDxe/PlatformEarlyDxe.inf {
  <LibraryClasses>
!if $(DEBUG_MESSAGE_ENABLE) == TRUE
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
  }
  
  ProjectPkg/ProjectEarlyDxe/ProjectEarlyDxe.inf {
  <LibraryClasses>
!if $(DEBUG_MESSAGE_ENABLE) == TRUE
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
  } 
  
  $(PLATFORM_PACKAGE)/PlatformDxe/PlatformDxe.inf {
  <LibraryClasses>
!if $(DEBUG_MESSAGE_ENABLE) == TRUE
    DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
  }

# MdeModulePkg/Bus/Pci/PciHostBridgeDxe/PciHostBridgeDxe.inf
# MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
  $(PLATFORM_PACKAGE)/Override/PcAtChipsetPkg/PciHostBridgeDxe/PciHostBridgeDxe.inf
  $(PLATFORM_PACKAGE)/Override/MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf {
  <PcdsPatchableInModule>
#    gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }
# MdeModulePkg/Universal/MemoryTest/NullMemoryTestDxe/NullMemoryTestDxe.inf
  MdeModulePkg/Universal/MemoryTest/GenericMemoryTestDxe/GenericMemoryTestDxe.inf
  ByoModulePkg/Csm/LegacyBiosDxe/LegacyBiosDxe.inf
  ByoModulePkg/Csm/BlockIoDxe/BlockIoDxe.inf
  IntelFrameworkModulePkg/Csm/BiosThunk/VideoDxe/VideoDxe.inf {
  <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }
  ByoModulePkg/LegacyInterruptHookDxe/LegacyInterruptHook.inf

  UefiCpuPkg/CpuIo2Smm/CpuIo2Smm.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmIpl.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmCore.inf {
  <LibraryClasses>
      NULL|ByoModulePkg/Library/GuidNameLib/GuidNameLib.inf
!if $(DEBUG_MESSAGE_ENABLE) == TRUE
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
  <PcdsPatchableInModule>
#     gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }

  $(PLATFORM_PACKAGE)/SmmPlatform/SmmPlatform.inf
  UefiCpuPkg/PiSmmCommunication/PiSmmCommunicationSmm.inf
  
  ProjectPkg/ProjectSmm/ProjectSmm.inf

!if $(IPMI_SUPPORT) == TRUE  
  ServerCommonPkg/Ipmi/Generic/GenericIpmi.inf
  ServerCommonPkg/Ipmi/Generic/SmmGenericIpmi.inf
  ServerCommonPkg/Ipmi/IpmiInterfaceDxe/IpmiInterfaceDxe.inf
  
  ServerCommonPkg/Ipmi/BmcSol/SolStatus.inf
  ServerCommonPkg/Ipmi/GenericElog/Dxe/GenericElog.inf
  ServerCommonPkg/Ipmi/GenericElog/Smm/GenericElog.inf
  ServerCommonPkg/Ipmi/BmcElog/Dxe/BmcElog.inf
  ServerCommonPkg/Ipmi/BmcElog/Smm/BmcElog.inf
  ServerCommonPkg/Ipmi/BmcCfg/BmcCfg.inf
  ServerCommonPkg/Ipmi/BmcWdt/BmcWdt.inf

  ServerCommonPkg/Ipmi/PostErrorToSEL/SelError.inf  
  ServerCommonPkg/Ipmi/SetupBmcCfg/SetupBmcCfg.inf
  #ServerCommonPkg/Ipmi/SdrSensorDev/SdrSensorDev.inf

  ServerCommonPkg/Ipmi/IpmiBootOption/Dxe/IpmiBootOption.inf {
  <LibraryClasses>
    LegacyBootManagerLib|IntelFrameworkModulePkg/Library/LegacyBootManagerLib/LegacyBootManagerLib.inf	
  }
  ServerCommonPkg/Ipmi/IpmiRedirFru/IpmiRedirFru.inf 
!endif

#ATA & SCSI
  ByoModulePkg/Bus/Ata/AtaAtapiPassThru/AtaAtapiPassThru.inf
  MdeModulePkg/Bus/Ata/AtaBusDxe/AtaBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiBusDxe/ScsiBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiDiskDxe/ScsiDiskDxe.inf
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf

#  MdeModulePkg/Universal/CapsulePei/CapsuleX64.inf {
#    <LibraryClasses>
#!if $(DEBUG_MESSAGE_ENABLE) == TRUE
#      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
#!endif
#  }

  ByoModulePkg/SmiFlash/SmiFlash.inf
  ByoModulePkg/SmiFlashDxe/SmiFlashDxe.inf
 
#USB
#LEGACY_USB
  ByoModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
  ByoModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf
  ByoModulePkg/Bus/Usb/LegacyUsbSmm/LegacyUsbSmm.inf
  ByoModulePkg/Bus/Usb/LegacyFreeKbDxe/LegacyFreeKbDxe.inf

#NATIVE  
  MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf   
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
# MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf

 
#ACPI Support
  ProjectPkg/AcpiTables/AcpiTables.inf {
  <BuildOptions>
!if $(TCM_ENABLE) == TRUE    
      *_*_*_ASLPP_FLAGS = /DTCM_ENABLE=1
!endif      
  }

  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf
  $(PLATFORM_PACKAGE)/AcpiPlatformDxe/AcpiPlatformDxe.inf
  
  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf  

# SPI
  $(PLATFORM_PACKAGE)/Spi/SpiSmm.inf
  $(PLATFORM_PACKAGE)/Spi/SpiDxe.inf
  ByoNvMediaPkg/PlatformAccess/Smm/PlatformAccess.inf
  ByoNvMediaPkg/NvMediaAccess/Smm/NvMediaAccess.inf
  ByoNvMediaPkg/NvMediaAccess/Dxe/NvMediaAccess.inf
  ByoNvMediaPkg/FlashDevice/WINBOND25Q256/Smm/spiflashdevice.inf
  ByoNvMediaPkg/FlashDevice/WINBOND25Q256/Dxe/spiflashdevice.inf
  ByoNvMediaPkg/FlashDevice/WINBOND25Q128/Smm/spiflashdevice.inf
  ByoNvMediaPkg/FlashDevice/WINBOND25Q128/Dxe/spiflashdevice.inf  
  ByoNvMediaPkg\FlashDevice/MXIC25L256/Dxe/spiflashdevice.inf
  ByoNvMediaPkg\FlashDevice/MXIC25L256/Smm/spiflashdevice.inf  
  ByoNvMediaPkg\FlashDevice/GD25B256D/Dxe/spiflashdevice.inf
  ByoNvMediaPkg\FlashDevice/GD25B256D/Smm/spiflashdevice.inf 
!if 1
  ByoNvMediaPkg/FvbService/Smm/FvbService.inf {
  <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteSmm.inf  
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmmRuntimeDxe.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmm.inf
!else  
  MdeModulePkg\Universal\Variable\EmuRuntimeDxe\EmuVariableRuntimeDxe.inf 
!endif

  !if $(HDD_PASSWORD_ENABLE) == TRUE
    #ByoModulePkg/Security/HddPassword/Smm/HddPasswordSmm.inf
    ByoModulePkg/Security/HddPassword/Dxe/HddPasswordDxe.inf
  !endif

  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  ByoModulePkg/Setup/JpegDecoderDxe/JpegDecoder.inf
  ByoModulePkg/Setup/DisplayEngineDxe/DisplayEngineDxe.inf
  ByoModulePkg/Setup/SetupBrowserDxe/SetupBrowserDxe.inf
# ByoModulePkg/Setup/SetupMouse/SetupMouse.inf
  ByoModulePkg/Setup/UnicodeFontDxe/UnicodeFontDxe.inf
  ProjectPkg/PlatformSetupDxe/PlatformSetupDxe.inf
  ProjectPkg/SetupUiApp/SetupUiApp.inf
  ByoModulePkg/BootManagerMenuApp/BootManagerMenuApp.inf
  $(PLATFORM_PACKAGE)/PxeBootApp/PxeBootApp.inf
  
  ByoModulePkg/IsaAcpiDxe/IsaAcpi.inf
  IntelFrameworkModulePkg/Bus/Isa/IsaBusDxe/IsaBusDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/IsaSerialDxe/IsaSerialDxe.inf
  ByoModulePkg/Console/TerminalDxe/TerminalDxe.inf

  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTableDxe/FirmwarePerformanceDxe.inf
  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTableSmm/FirmwarePerformanceSmm.inf
  MdeModulePkg/Universal/Acpi/BootGraphicsResourceTableDxe/BootGraphicsResourceTableDxe.inf


!if $(TCM_ENABLE) == TRUE
  ByoModulePkg/Security/Tcm/Dxe/TcmDxe.inf
#!if $(CSM_ENABLE) != FALSE
# ByoModulePkg/Security/Tcm/Smm/TcmSmm16.inf
# ByoModulePkg/Security/Tcm/Smm/TcmSmmInstallInt1A.inf
# ByoModulePkg/Security/Tcm/Smm/TcmSmm.inf  
#!endif
# ByoModulePkg/Security/Tcm/Application/TcmApp/TcmApp.inf
!endif
# ByoModulePkg/CrisisRecovery/FlashUpdateDxe/FlashUpdateDxe.inf


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
!if $(DEBUG_MESSAGE_ENABLE) == TRUE
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif      
  }
  SecurityPkg/Tcg/TcgConfigDxe/TcgConfigDxe.inf {
  <LibraryClasses>
      TpmCommLib|SecurityPkg/Library/TpmCommLib/TpmCommLib.inf
      Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibDTpm/Tpm12DeviceLibDTpm.inf
  }
  SecurityPkg/Tcg/TcgSmm/TcgSmm.inf
  
!if $(CSM_ENABLE) != FALSE
#   ByoModulePkg/Security/Tpm/TcgServiceSmm/TcgSmm.inf {
#   <LibraryClasses>
#       TpmCommLib|SecurityPkg/Library/TpmCommLib/TpmCommLib.inf
#   }
#   ByoModulePkg/Security/Tpm/TcgServiceSmm/TcgSmm16.inf
#   ByoModulePkg/Security/Tpm/TcgServiceSmm/TcgSmmInstallInt1A.inf
!endif  

  SecurityPkg/Tcg/Tcg2Dxe/Tcg2Dxe.inf {
  <LibraryClasses>
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterDxe.inf
      HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterDxe.inf
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
      NULL|PlatformPkg/Library/Tpm2InstanceLibHgfTpm/Tpm2InstanceLibHgfTpmDxe.inf  
      NULL|SecurityPkg/Library/HashInstanceLibSm3/HashInstanceLibSm3.inf           
     #  NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
  }
  SecurityPkg/Tcg/Tcg2Config/Tcg2ConfigDxe.inf {
  <LibraryClasses>
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibTcg2/Tpm2DeviceLibTcg2.inf
  }
  
  SecurityPkg/Tcg/Tcg2Smm/Tcg2Smm.inf
  
!endif  



  ProjectPkg/BiosInfo/BiosInfo.inf
# NaplesSoCPkg/CpuAcpiTable/AcpiUpdateCpuInfo.inf
  AgesaModulePkg/Debug/AmdIdsDebugPrintDxe/AmdIdsDebugPrintDxe.inf
  AgesaModulePkg/Mem/AmdMemAm4Dxe/AmdMemAm4Dxe.inf
  AgesaModulePkg/Mem/AmdMemSmbiosV2Dxe/AmdMemSmbiosV2Dxe.inf
  AgesaModulePkg/Psp/ApcbDrv/ApcbZpDxe/ApcbZpDxe.inf{
  <LibraryClasses>
    ApcbLib|AgesaModulePkg/Library/ApcbLib/ApcbLib.inf
  }
  AgesaModulePkg/Psp/ApcbDrv/ApcbZpSmm/ApcbZpSmm.inf{
  <LibraryClasses>
    ApcbLib|AgesaModulePkg/Library/ApcbLib/ApcbLib.inf
  }
  AgesaModulePkg/Psp/AmdPspDxeV2/AmdPspDxeV2.inf
  AgesaModulePkg/Psp/AmdPspP2CmboxV2/AmdPspP2CmboxV2.inf
  AgesaModulePkg/Psp/AmdPspSmmV2/AmdPspSmmV2.inf

#GR-Agesa-  AgesaModulePkg/Psp/AmdHstiV2/AmdHstiV2.inf
  AgesaModulePkg/Psp/AmdHstiV2/AmdHstiV2Src.inf #GR-Agesa+
  AgesaModulePkg/Ccx/Zen/CcxZenZpDxe/AmdCcxZenZpDxe.inf {
  <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesZpLib/CcxResetTablesZpLib.inf
    FabricWdtLib|AgesaModulePkg/Library/FabricWdtZpLib/FabricWdtZpLib.inf
  }
  AgesaModulePkg/Ccx/Zen/CcxZenZpSmm/AmdCcxZenZpSmm.inf
  AgesaModulePkg/Fabric/ZP/FabricZpDxe/AmdFabricZpDxe.inf {
  <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  }
  AgesaModulePkg/Fabric/ZP/FabricZpSmm/AmdFabricZpSmm.inf {
  <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
  }
  AgesaModulePkg/Soc/AmdSocSp3ZpDxe/AmdSocSp3ZpDxe.inf {
  <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
  }
  AgesaModulePkg/ErrorLog/AmdErrorLogDxe/AmdErrorLogDxe.inf
  AgesaModulePkg/Mem/AmdMemPprSmmDriver/AmdMemPprSmmDriver.inf
  ## Gnb Dxe Drivers
  AgesaModulePkg/Nbio/NbioBase/AmdNbioBaseZPDxe/AmdNbioBaseZPDxe.inf
  AgesaModulePkg/Nbio/SMU/AmdNbioSmuV9Dxe/AmdNbioSmuV9Dxe.inf
  AgesaModulePkg/Nbio/PCIe/AmdNbioPcieZPDxe/AmdNbioPcieZPDxe.inf
  AgesaModulePkg/Nbio/IOMMU/AmdNbioIOMMUZPDxe/AmdNbioIOMMUZPDxe.inf
  AgesaModulePkg/Nbio/NbioALib/AmdNbioAlibZPDxe/AmdNbioAlibZPDxe.inf
  AgesaModulePkg/Fch/Taishan/FchTaishanDxe/FchDxe.inf
  AgesaModulePkg/Fch/Taishan/FchTaishanDxe/FchTaishanSsdt.inf #GR-Agesa+
  AgesaModulePkg/Fch/Taishan/FchTaishanSmm/FchSmm.inf
  AgesaModulePkg/Fch/Taishan/FchTaishanSmbusDxe/SmbusLight.inf
  AgesaModulePkg/Fch/Taishan/FchTaishanCf9ResetDxe/Cf9Reset.inf
  AgesaModulePkg/Fch/Taishan/FchTaishanLegacyInterruptDxe/LegacyInterrupt.inf
  AgesaModulePkg/Fch/Taishan/FchTaishanSmmControlDxe/SmmControl.inf
  AgesaModulePkg/Fch/Taishan/FchTaishanSmmDispatcher/FchSmmDispatcher.inf
  AgesaModulePkg/Fch/Taishan/FchTaishanMultiFchDxe/FchMultiFchDxe.inf
  AgesaModulePkg/Fch/Taishan/FchTaishanMultiFchSmm/FchMultiFchSmm.inf

  # Universal
  AgesaModulePkg/Universal/Smbios/AmdSmbiosDxe.inf
  AgesaModulePkg/Universal/Acpi/AmdAcpiDxe.inf
  AgesaModulePkg/Universal/AmdSmmCommunication/AmdSmmCommunication.inf
  AgesaModulePkg/Universal/Version/AmdVersionDxe/AmdVersionDxe.inf

  ## Ras Dxe Drivers
  AgesaModulePkg/Ras/AmdRasApeiDxe/AmdRasApeiDxe.inf
  AgesaModulePkg/Ras/AmdRasDxe/AmdRasDxe.inf
  AgesaModulePkg/Ras/AmdRasSmm/AmdRasSmm.inf

  ## Snp Dxe Drivers (Uncomment SnpDxePort0.inf for xGBE support)
  # AgesaModulePkg/SnpDxe/SnpDxePort0.inf                                                                              <SP3ZP>
  ## NVDIMM Dxe Driver
# AgesaModulePkg/JedecNvdimm/JedecNvdimm.inf
  #AgesaPkg/Addendum/Nbio/ServerHotplugDxe/ServerHotplugDxe.inf #GR-Agesa+
  ProjectPkg/ServerHotplugDxe/ServerHotplugDxe.inf
  AmdCbsPkg/CbsSetupDxe/CbsSetupDxe.inf

  AmdCbsPkg/CbsBaseDxe/CbsBaseDxe.inf

  AmdCpmPkg/Kernel/Asl/AmdCpmInitAsl.inf
  AmdCpmPkg/Kernel/Dxe/AmdCpmInitDxe.inf
  AmdCpmPkg/Kernel/Smm/AmdCpmInitSmm.inf
# AmdCpmPkg/Features/BoardId/Dxe/AmdBoardIdDxe.inf
  AmdCpmPkg/Features/PcieInit/Asl/AmdCpmPcieInitAsl.inf
  AmdCpmPkg/Features/PcieInit/Dxe/AmdCpmPcieInitDxe.inf
  AmdCpmPkg/Features/I2cMaster/Dxe/AmdI2cMasterDxe.inf
  AmdCpmPkg/Features/GpioInit/Dxe/AmdCpmGpioInitDxe.inf
  AmdCpmPkg/Features/GpioInit/Smm/AmdCpmGpioInitSmm.inf
 # AmdCpmPkg/Features/PlatformRas/Dxe/AmdPlatformRasDxe.inf
 # AmdCpmPkg/Features/PlatformRas/Smm/AmdPlatformRasSmm.inf
  $(PLATFORM_PACKAGE)/Override/AmdCpmPkg/Features/PlatformRas/Dxe/AmdPlatformRasDxe.inf
  $(PLATFORM_PACKAGE)/Override/AmdCpmPkg/Features/PlatformRas/Smm/AmdPlatformRasSmm.inf
  AmdCpmPkg/Features/xGbEI2cMaster/xGbEI2cMasterDxe.inf
#  AmdCpmPkg/Devices/Pca9535a/Dxe/Pca9535aDxe.inf
#  AmdCpmPkg/Devices/Pca9545a/Dxe/Pca9545aDxe.inf
#  AmdCpmPkg/Devices/Sff8472/Dxe/Sff8472Dxe.inf

  PlatformPkg/Cpu/Smm/CpuSmm.inf
  $(PLATFORM_PACKAGE)/PspPlatform/PspPlatform.inf

!if $(UEFI_NATIVE_NETWORK_SUPPORT) == TRUE
  MdeModulePkg/Universal/Network/SnpDxe/SnpDxe.inf
  MdeModulePkg/Universal/Network/DpcDxe/DpcDxe.inf
  MdeModulePkg/Universal/Network/MnpDxe/MnpDxe.inf
  MdeModulePkg/Universal/Network/VlanConfigDxe/VlanConfigDxe.inf
  MdeModulePkg/Universal/Network/ArpDxe/ArpDxe.inf
  MdeModulePkg/Universal/Network/Dhcp4Dxe/Dhcp4Dxe.inf
  MdeModulePkg/Universal/Network/Ip4Dxe/Ip4Dxe.inf
  MdeModulePkg/Universal/Network/Mtftp4Dxe/Mtftp4Dxe.inf
  MdeModulePkg/Universal/Network/Udp4Dxe/Udp4Dxe.inf
!if $(NETWORK_IP6_ENABLE) == TRUE
  NetworkPkg/Ip6Dxe/Ip6Dxe.inf
  NetworkPkg/TcpDxe/TcpDxe.inf
  NetworkPkg/Udp6Dxe/Udp6Dxe.inf
  NetworkPkg/Dhcp6Dxe/Dhcp6Dxe.inf
  NetworkPkg/Mtftp6Dxe/Mtftp6Dxe.inf
  NetworkPkg/UefiPxeBcDxe/UefiPxeBcDxe.inf
!if $(SECURE_BOOT_ENABLE) == TRUE
  NetworkPkg/IScsiDxe/IScsiDxe.inf
!else
  MdeModulePkg/Universal/Network/IScsiDxe/IScsiDxe.inf
!endif
!else
  MdeModulePkg/Universal/Network/Tcp4Dxe/Tcp4Dxe.inf
  MdeModulePkg/Universal/Network/UefiPxeBcDxe/UefiPxeBcDxe.inf
  MdeModulePkg/Universal/Network/IScsiDxe/IScsiDxe.inf
!endif
!if $(HTTP_BOOT_ENABLE) == TRUE
  NetworkPkg/DnsDxe/DnsDxe.inf
  NetworkPkg/HttpUtilitiesDxe/HttpUtilitiesDxe.inf
  NetworkPkg/HttpDxe/HttpDxe.inf
  NetworkPkg/HttpBootDxe/HttpBootDxe.inf
!endif
!endif

#ProjectPkg/UdkFlash/UdkFlash.inf {
#  <LibraryClasses>
#    ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
#}

  ProjectPkg/ProjectDxe/ProjectDxe.inf

  ByoModulePkg/Setup/PciListDxe/PciListDxe.inf
  ByoModulePkg/Setup/BiosUpdate/BiosUpdate.inf

  MdeModulePkg/Universal/FileExplorerDxe/FileExplorerDxe.inf  {
    <LibraryClasses>
      FileExplorerLib|ByoModulePkg/Library/FileExplorerLib/FileExplorerLib.inf
  }   

  FatPkg/EnhancedFatDxe/Fat.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }

# ByoToolPkg/FlashUpdateTool/EFI/FlashUpdate.inf
  ByoModulePkg/PostErrorDxe/PostErrorDxe.inf
  PlatformPkg/PcieInitDxe/PcieInitDxe.inf
  AmdCpmPkg/Features/SpiLock/Dxe/AmdSpiLockDxe.inf
  AmdCpmPkg/Features/SpiLock/Smm/AmdSpiLockSmm.inf

!if 0
  ProjectPkg/MyDebugDxe/MyDebugDxe.inf {
  <LibraryClasses>
!if $(DEBUG_MESSAGE_ENABLE) == TRUE
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
  }
!endif  

  ByoModulePkg/HddBindDxe/HddBindDxe.inf
  ProjectPkg/OemCpuP0Dxe/OemCpuP0Dxe.inf  
  
  ShellPkg/Application/Shell/Shell.inf {
    <LibraryClasses>
      NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
!ifndef $(NO_SHELL_PROFILES)
      NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork2CommandsLib/UefiShellNetwork2CommandsLib.inf
!ifdef $(INCLUDE_DP)
      NULL|ShellPkg/Library/UefiDpLib/UefiDpLib.inf
!endif #$(INCLUDE_DP)
!ifdef $(INCLUDE_TFTP_COMMAND)
      NULL|ShellPkg/Library/UefiShellTftpCommandLib/UefiShellTftpCommandLib.inf
!endif #$(INCLUDE_TFTP_COMMAND)
!endif #$(NO_SHELL_PROFILES)

    <PcdsFixedAtBuild>
      gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
  }

#-------------------------------------------------------------------------------
[BuildOptions.common.EDKII.DXE_RUNTIME_DRIVER, BuildOptions.common.EDKII.DXE_SMM_DRIVER, BuildOptions.common.EDKII.SMM_CORE]
  MSFT:*_*_*_DLINK_FLAGS         = /ALIGN:4096


[BuildOptions]


[BuildOptions.Common.EDKII]

!if $(DEBUG_MESSAGE_ENABLE) == TRUE
  DEFINE UDK_DEBUG_BUILD_OPTIONS =
!else
  DEFINE UDK_DEBUG_BUILD_OPTIONS = /DMDEPKG_NDEBUG
!endif


!if $(RECOVERY_ENABLE) == TRUE
  DEFINE RECOVERY_ENABLE_OPTIONS = /D RECOVERY_ENABLE
!else
  DEFINE RECOVERY_ENABLE_OPTIONS =
!endif

!if $(SECURE_BOOT_ENABLE) == TRUE
  DEFINE SECURE_BOOT_SUPPORT = /D SECURE_BOOT_ENABLE
!else
  DEFINE SECURE_BOOT_SUPPORT =
!endif

!if $(HDD_PASSWORD_ENABLE) == TRUE
  DEFINE HDD_PASSWORD_SUPPORT = /D HDD_PASSWORD_ENABLE
!else
  DEFINE HDD_PASSWORD_SUPPORT =
!endif

  DEFINE PLATFORM_FEATURE_SUPPORT = 

  DEFINE EDK_EDKII_DSC_FEATURE_BUILD_OPTIONS = $(UDK_DEBUG_BUILD_OPTIONS) $(PLATFORM_FEATURE_SUPPORT) $(RECOVERY_ENABLE_OPTIONS) $(SECURE_BOOT_SUPPORT) $(HDD_PASSWORD_SUPPORT)

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

  *_*_*_ASL_FLAGS = -oi


