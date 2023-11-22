## @file
# EFI/PI Reference Module Package for All Architectures
#
# Copyright (c) 2006 - 2015, Byosoft Corporation.<BR>
# All rights reserved.This software and associated documentation (if any)
# is furnished under a license and may only be used or copied in
# accordance with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be reproduced,
# stored in a retrieval system, or transmitted in any form or by any
# means without the express written consent of Byosoft Corporation.
#
#	File Name:
#  	ByoModulePkg.dsc
#
#	Abstract:
#  	ByoSoft Core Module Package Configuration File
#
##

[Defines]
  PLATFORM_NAME                  = ByoModule
  PLATFORM_GUID                  = D9FBEF68-0995-47A4-9EA8-73FAD163715E
  PLATFORM_VERSION               = 0.01
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/ByoModule
  SUPPORTED_ARCHITECTURES        = IA32|X64|AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

!include MdePkg/MdeLibs.dsc.inc
!include ByoModulePkg/ByoModule.dsc.inc
[LibraryClasses]
  #
  # Entry point
  #
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  
  #
  # Basic
  #
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  CacheMaintenanceLib|MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLibNull/BaseCryptLibNull.inf
  TlsLib|CryptoPkg/Library/TlsLibNull/TlsLibNull.inf
  HashApiLib|CryptoPkg/Library/BaseHashApiLib/BaseHashApiLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  BmpSupportLib|MdeModulePkg/Library/BaseBmpSupportLib/BaseBmpSupportLib.inf
  SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf


  #
  # UEFI & PI
  #
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  
  #
  # Generic Modules
  #
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  BootLogoLib|MdeModulePkg/Library/BootLogoLib/BootLogoLib.inf
  
  #
  # Framework
  #
  S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
  
  #
  # TPM Library
  #
  TpmCommLib|SecurityPkg/Library/TpmCommLib/TpmCommLib.inf
  TcmCommLib|ByoModulePkg/Security/Tcm/TcmCommLib/TcmCommLib.inf

  #
  # Setup
  #
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  CustomizedDisplayLib|ByoModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf
  SetupUiLib|ByoModulePkg/Library/SetupUiLib/SetupUiLib.inf 
  PlatformLanguageLib|ByoModulePkg/Library/PlatformLanguageLib/PlatformLanguageLib.inf
  SystemPasswordLib|ByoModulePkg/Library/SystemPasswordLib/SystemPasswordLib.inf
  SafeMemoryLib|ByoModulePkg/Library/ByoSafeMemoryLib/ByoSafeMemoryLib.inf
  SafePrintLib|ByoModulePkg/Library/ByoSafePrintLib/ByoSafePrintLib.inf
  Tpm2CommandLib|SecurityPkg/Library/Tpm2CommandLib/Tpm2CommandLib.inf
  Tcg2PpVendorLib|SecurityPkg/Library/Tcg2PpVendorLibNull/Tcg2PpVendorLibNull.inf
  TcgPpVendorLib|SecurityPkg/Library/TcgPpVendorLibNull/TcgPpVendorLibNull.inf

  #
  # Misc
  #
  ResetSystemLib|MdeModulePkg/Library/BaseResetSystemLibNull/BaseResetSystemLibNull.inf
  PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
  PlatformSecureLib|ByoModulePkg/Library/PlatformSecureLib/PlatformSecureLib.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf  
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf  
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  SetupUiLib|ByoModulePkg/Library/SetupUiLib/SetupUiLib.inf
  ByoCommLib|ByoModulePkg/Library/ByoCommLib/ByoCommLib.inf
  FileExplorerLib|ByoModulePkg/Library/FileExplorerLib/FileExplorerLib.inf
  StatusCodeBeepLib|ByoModulePkg/Library/StatusCodeBeepLib/StatusCodeBeepLib.inf
  SetMemAttributeSmmLib|ByoModulePkg/Library/SetMemAttributeSmmLibNull/SetMemAttributeSmmLib.inf

  ByoUefiBootManagerLib|ByoModulePkg/Library/ByoUefiBootManagerLib/ByoUefiBootManagerLib.inf 
  ByoKbcLib|ByoModulePkg/Library/ByoKbcLib/ByoKbcLib.inf
  ## Consumed by HddPasswordDxe
  OpalPasswordSupportLib|ByoModulePkg/Library/OpalPasswordSupportLib/OpalPasswordSupportLib.inf

  PostCodeMapLib|ByoModulePkg/Library/PostCodeMapLib/PostCodeMapLib.inf

  BiosIdLib|ByoModulePkg/Library/BiosIdLib/BiosIdLib.inf

  RegisterFilterLib|MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf
  LegacyBootManagerLib|ByoLegacyPkg/Library/LegacyBootManagerLib/LegacyBootManagerLib.inf
  LegacyBootOptionalDataLib|ByoLegacyPkg/Library/LegacyBootOptionalDataLib/LegacyBootOptionalDataLib.inf
  RngLib|MdePkg/Library/BaseRngLib/BaseRngLib.inf
  TpmPlatformHierarchyLib|ByoModulePkg/Library/TpmPlatformHierarchyLib/TpmPlatformHierarchyLib.inf
  Tpm12CommandLib|SecurityPkg/Library/Tpm12CommandLib/Tpm12CommandLib.inf
  Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibDTpm/Tpm12DeviceLibDTpm.inf
  SecureBootRestoreLib|ByoModulePkg/Library/SecureBootRestoreLib/SecureBootRestoreLib.inf
  OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
  TcgStorageCoreLib|SecurityPkg/Library/TcgStorageCoreLib/TcgStorageCoreLib.inf
  TcgStorageOpalLib|SecurityPkg/Library/TcgStorageOpalLib/TcgStorageOpalLib.inf
  ##Default TCM IO Lib
  TMIoLib|ByoModulePkg/Library/MmioTMLib/MmioTMLib.inf


[PcdsDynamicDefault.common.DEFAULT]
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn|80
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow| 25
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution | 800
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution | 600
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInitializationPolicy
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInstanceGuid
[LibraryClasses.common.PEIM]
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/PeiExtractGuidedSectionLib/PeiExtractGuidedSectionLib.inf
  Tcg2PhysicalPresenceLib|SecurityPkg/Library/PeiTcg2PhysicalPresenceLib/PeiTcg2PhysicalPresenceLib.inf
  IpmiBaseLib|ByoServerPkg/Library/PeiIpmiBaseLib/PeiIpmiBaseLib.inf

[LibraryClasses.common.DXE_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  TcgPhysicalPresenceLib|SecurityPkg/Library/DxeTcgPhysicalPresenceLib/DxeTcgPhysicalPresenceLib.inf
  Tcg2PhysicalPresenceLib|SecurityPkg/Library/DxeTcg2PhysicalPresenceLib/DxeTcg2PhysicalPresenceLib.inf
  Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibTcg2/Tpm2DeviceLibTcg2.inf
  IpmiBaseLib|ByoServerPkg/Library/IpmiBaseLib/IpmiBaseLib.inf
  EfiServerManagementLib|ByoServerPkg/Library/ServerManagementLib/ServerManagementLib.inf
  NetLib|NetworkPkg/Library/DxeNetLib/DxeNetLib.inf
[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  IpmiBaseLib|ByoServerPkg/Library/IpmiBaseLib/IpmiBaseLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/SmmMemoryAllocationLib/SmmMemoryAllocationLib.inf
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  SystemPasswordLib|ByoModulePkg/Library/SystemPasswordLib/SystemPasswordLibSmm.inf

[LibraryClasses.common.UEFI_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf

[LibraryClasses.common.UEFI_APPLICATION]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  DebugLib|MdePkg/Library/UefiDebugLibStdErr/UefiDebugLibStdErr.inf
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
  
[LibraryClasses.AARCH64]
  #
  # It is not possible to prevent the ARM compiler for generic intrinsic functions.
  # This library provides the intrinsic functions generate by a given compiler.
  # [LibraryClasses.ARM] and NULL mean link this library into all ARM images.
  #
  NULL|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf

  # Add support for GCC stack protector
  NULL|MdePkg/Library/BaseStackCheckLib/BaseStackCheckLib.inf
###################################################################################################
#
# Components Section - list of the modules and components that will be processed by compilation
#                      tools and the EDK II tools to generate PE32/PE32+/Coff image files.
#
# Note: The EDK II DSC file is not used to specify how compiled binary images get placed
#       into firmware volume images. This section is just a list of modules to compile from
#       source into UEFI-compliant binaries.
#       It is the FDF file that contains information on combining binary files into firmware
#       volume images, whose concept is beyond UEFI and is described in PI specification.
#       Binary modules do not need to be listed in this section, as they should be
#       specified in the FDF file. For example: Shell binary (Shell_Full.efi), FAT binary (Fat.efi),
#       Logo (Logo.bmp), and etc.
#       There may also be modules listed in this section that are not required in the FDF file,
#       When a module listed here is excluded from FDF file, then UEFI-compliant binary will be
#       generated for it, but the binary will not be put into any firmware volume.
#
###################################################################################################

[Components.X64]
  ByoServerPkg/Library/SmmIpmiBaseLib/SmmIpmiBaseLib.inf
  ByoServerPkg/Library/RasElogEventLib/SmmRasElogEventLib.inf
  ByoServerPkg/Library/RasElogEventLib/DxeRasElogEventLib.inf

  ByoServerPkg/Ipmi/Generic/SmmGenericIpmi.inf
  ByoServerPkg/Ipmi/Generic/GenericIpmi.inf

  ByoLegacyPkg/8254TimerDxe/8254Timer.inf
  ByoLegacyPkg/8259InterruptControllerDxe/8259.inf
  ByoLegacyPkg/BlockIoDxe/BlockIoDxe.inf
  ByoLegacyPkg/Int18h/InstallInt18.inf
  ByoLegacyPkg/Int18h/Int18h.inf
  ByoLegacyPkg/LegacyBiosDxe/LegacyBiosDxe.inf
  ByoLegacyPkg/LegacyInterruptHookDxe/LegacyInterruptHook.inf
  ByoLegacyPkg/Library/LegacyBootManagerLib/LegacyBootManagerLib.inf
  ByoLegacyPkg/Library/LegacyBootOptionalDataLib/LegacyBootOptionalDataLib.inf
  ByoLegacyPkg/VideoDxe/VideoDxe.inf
  ByoModulePkg/Application/BootManagerMenuGuiApp/BootManagerMenuApp.inf{
    <Defines>
      FILE_GUID = 88D55B64-6DDF-4413-A3C6-5049E47DA433
  }
  ByoModulePkg/Application/PxeBootApp/PxeBootAppAll.inf {
    <Defines>
      FILE_GUID = 4CF13A69-3859-4D58-A2C1-8729A192FCAB
  }
  ByoModulePkg/HddBindDxe/HddBindDxe.inf
  ByoModulePkg/HddBindDxe/HddBindDxeAddBDF.inf {
    <Defines>
      FILE_GUID = 2046B692-7B12-4B69-BE3A-DC674C131D4F
  }
  ByoModulePkg/Library/ByoRtcLib/ByoRtcLib.inf
  ByoModulePkg/Library/SystemPasswordLib/SystemPasswordLibSmm.inf
  ByoModulePkg/Universal/SmbiosSmm/SmbiosSmm.inf
  ByoModulePkg/ByoBdsBootManagerDxe/ByoBdsBootManagerDxe.inf
  ByoModulePkg/Universal/PlatformBootManagerDxe/PlatformBootManagerDxe.inf
  ByoModulePkg/Nvme/NvmeSmm/NvmeSmm.inf
  ByoModulePkg/StatusCodeHandler/Smm/StatusCodeHandlerSmm.inf
  ByoModulePkg/Bus/Usb/UsbMouseAbsolutePointerDxe/UsbMouseAbsolutePointerDxe.inf
  ByoModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  ByoModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
  ByoModulePkg/Bus/Pci/UhciDxe/UhciDxe.inf
  ByoModulePkg/Bus/Usb/LegacyFreeKbDxe/LegacyFreeKbDxe.inf
  ByoModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  ByoModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
  ByoModulePkg/Bus/Usb/LegacyUsbSmm/LegacyUsbSmm.inf
  ByoModulePkg/Security/Tpm/TcgServiceSmm/TcgSmm.inf
  ByoModulePkg/Security/Tpm/TcgServiceSmm/TcgSmm16.inf
  ByoModulePkg/Security/Tpm/TcgServiceSmm/TcgSmmInstallInt1A.inf
  ByoModulePkg/Security/Tcm/Smm/TcmSmm16.inf
  ByoModulePkg/Security/Tcm/Smm/TcmSmm.inf
  ByoModulePkg/Security/Tcm/Smm/TcmSmmInstallInt1A.inf
  ByoModulePkg/SmiFlashDxe/SmiFlashDxe.inf
  ByoModulePkg/SmiFlash/SmiFlash.inf
  ByoModulePkg/Universal/PlatformSmiFlashExt/PlatformSmiFlashExt.inf
  ByoModulePkg/Universal/BootMaintenanceManagerDxe/BootMaintenanceManagerDxe.inf

[Components.IA32, Components.AARCH64]
  ByoModulePkg/Bus/Ata/AtaBusPei/AtaBusPei.inf
  ByoModulePkg/CrisisRecovery/ModuleRecoveryPei/ModuleRecoveryPei.inf
  ByoModulePkg/Library/BiosIdLib/PeiBiosIdLib.inf
  ByoModulePkg/Library/PeiMultiPlatSupportLib/PeiMultiPlatSupportLib.inf
  ByoModulePkg/Security/HddPassword/Pei/HddPasswordPei.inf
  ByoModulePkg/Security/OpalPassword/Pei/OpalPasswordPei.inf
  ByoModulePkg/Security/Tcm/Pei/TcmEnablePei.inf
  ByoModulePkg/Security/Tcm/Pei/TcmPei.inf
  ByoModulePkg/Security/Tpm/Tpm2Setup/Tcg2ConfigPei.inf
  ByoModulePkg/SmiFlashPei/SmiFlashPei.inf
  ByoModulePkg/StatusCodeHandler/Pei/StatusCodeHandlerPei.inf
  ByoModulePkg/Universal/Disk/CDExpressPei/CdExpressPei.inf
  ByoModulePkg/Universal/Disk/FatPei/FatPei.inf
  ByoModulePkg/Universal/RestoreDevicePciConfigPei/DevicePciConfigRestorePei.inf
  ByoModulePkg/Library/PeiUefiVariableDefaultHobLib/PeiUefiVariableDefaultHobLib.inf
  #ByoServerPkg/Ipmi/GenericPei/PeiGenericIpmi.inf
  ByoServerPkg/Ipmi/IpmiBootOptionPei/IpmiBootOptionPei.inf
  ByoServerPkg/Library/PeiIpmiBaseLib/PeiIpmiBaseLib.inf
  ByoServerPkg/Ipmi/IpmiSendStatusCodeHandler/IpmiStatusCodeHandlerPei.inf {
    <LibraryClasses>
    EfiServerManagementLib|ByoServerPkg/Library/PeiServerManagementLib/PeiServerManagementLib.inf
  }

[Components.X64, Components.AARCH64]
  ByoServerPkg/Library/IpmiBaseLib/IpmiBaseLib.inf
  ByoServerPkg/Library/SmmIpmiBaseLib/MmIpmiBaseLib.inf
  ByoServerPkg/Library/PostErrorTableLib/PostErrorTableLib.inf
  ByoServerPkg/Library/ServerManagementLib/ServerManagementLib.inf

  ByoServerPkg/Ipmi/BmcCfg/BmcCfg.inf
  ByoServerPkg/Ipmi/BmcSol/SolStatus.inf
  ByoServerPkg/Ipmi/BmcWdt/BmcWdt.inf
  ByoServerPkg/Ipmi/IpmiBootOrder/IpmiBootOrder.inf
  ByoServerPkg/Ipmi/IpmiBootOrder/IpmiBootOrderUefi.inf
  ByoServerPkg/Ipmi/IpmiRedirFru/IpmiRedirFru.inf
  ByoServerPkg/Ipmi/SetupBmcCfg/SetupBmcCfg.inf
  ByoServerPkg/Ipmi/SendInfoToBmc/SendInfoToBmc.inf
  ByoServerPkg/Ipmi/IpmiSendStatusCodeHandler/IpmiStatusCodeHandlerRuntimeDxe.inf{
    <LibraryClasses>
    EfiServerManagementLib|ByoServerPkg/Library/ServerManagementLib/ServerManagementLib.inf
  }

  ByoModulePkg/Application/BootManagerMenuApp/BootManagerMenuApp.inf
  ByoModulePkg/Application/ByoAuditModeTest/ByoAuditModeTest.inf
  ByoModulePkg/Application/ByoUiApp/ByoUiApp.inf {
    <LibraryClasses>
    NULL|MdeModulePkg/Library/BootManagerUiLib/BootManagerUiLib.inf
  }
  ByoModulePkg/Application/ByoUiApp/ByoUiDxe.inf
  ByoModulePkg/Application/PxeBootApp/PxeBootApp.inf
  ByoModulePkg/Bus/Pci/IncompatiblePciDeviceSupportDxe/IncompatiblePciDeviceSupportDxe.inf
  ByoModulePkg/HddBindDxe/HddBindDxeOnlyUefi.inf {
    <Defines>
      FILE_GUID = 6AFFB76B-7E81-4E69-B721-217DAE879DDA
  }
  ByoModulePkg/Library/BiosIdLib/BiosIdLib.inf
  ByoModulePkg/Library/ByoAzaliaLib/ByoAzaliaLib.inf
  ByoModulePkg/Library/ByoBiosSignInfoLib/ByoBiosSignInfoLib.inf
  ByoModulePkg/Library/ByoBootManagerLib/ByoBootManagerLib.inf
  ByoModulePkg/Library/ByoCommLib/ByoCommLib.inf
  ByoModulePkg/Library/ByoKbcLib/ByoKbcLib.inf
  ByoModulePkg/Library/ByoSafeMemoryLib/ByoSafeMemoryLib.inf
  ByoModulePkg/Library/ByoSafePrintLib/ByoSafePrintLib.inf
  ByoModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf
  ByoModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLibUserDefined.inf {
    <Defines>
      FILE_GUID = F843C8E8-5AA5-49DA-BC9C-934EE86F64BB
  }
  ByoModulePkg/Library/DxeMultiPlatSupportLib/DxeMultiPlatSupportLib.inf
  ByoModulePkg/Library/FileExplorerLib/FileExplorerLib.inf
  ByoModulePkg/Library/OpalPasswordSupportLib/OpalPasswordSupportLib.inf
  ByoModulePkg/Library/PlatformLanguageLib/PlatformLanguageLib.inf
  ByoModulePkg/Library/PlatformSecureLib/PlatformSecureLib.inf
  ByoModulePkg/Library/PostCodeMapLib/PostCodeMapLib.inf
  ByoModulePkg/Library/SecureBootRestoreLib/SecureBootRestoreLib.inf
  ByoModulePkg/Library/SetMemAttributeSmmLibNull/SetMemAttributeSmmLib.inf
  ByoModulePkg/Library/SetupUiLib/SetupUiLib.inf
  ByoModulePkg/Library/StatusCodeBeepLib/StatusCodeBeepLib.inf
  ByoModulePkg/Library/SystemPasswordLib/SystemPasswordLib.inf
  ByoModulePkg/Library/TpmPlatformHierarchyLib/TpmPlatformHierarchyLib.inf
  ByoModulePkg/Logo/LogoDxe.inf
  ByoModulePkg/Logo/LogoJpgDxe.inf
  ByoModulePkg/Nvme/NvmeDxe/NvmExpressDxe.inf
  ByoModulePkg/Security/HddPassword/Dxe/HddPasswordDxe.inf
  ByoModulePkg/Security/HddSanitizationDxe/HddSanitizationDxe.inf
  ByoModulePkg/Security/SecureBoot/SecureBootDefult.inf
  ByoModulePkg/Security/SecureBootConfigDxe/SecureBootConfigDxe.inf
  ByoModulePkg/Security/Tcm/Application/TcmApp/TcmApp.inf
  ByoModulePkg/Security/Tcm/Dxe/TcmAcpi.inf
  ByoModulePkg/Security/Tcm/Dxe/TcmDxe.inf
  ByoModulePkg/Security/Tcm/Setup/TcmSetup.inf
  ByoModulePkg/Security/Tcm/TcmCommLib/TcmCommLib.inf
  ByoModulePkg/Security/Tpm/Tcg2PlatformDxe/Tcg2PlatformDxe.inf
  ByoModulePkg/Security/Tpm/Tpm2Setup/Tcg2ConfigDxe.inf
  ByoModulePkg/Setup/BiosUpdate/BiosUpdate.inf
  ByoModulePkg/Setup/ChineseDxe/ChineseDxe.inf
  ByoModulePkg/Setup/DisplayBackupRecoveryDxe/DisplayBackupRecoveryDxe.inf
  ByoModulePkg/Setup/DisplayEngineDxe/DisplayEngineDxe.inf
  ByoModulePkg/Setup/JpegDecoderDxe/JpegDecoder.inf
  ByoModulePkg/Setup/PciListDxe/PciListDxe.inf
  ByoModulePkg/Setup/DisplayNVMESSDxe/DisplayNVMESSDxe.inf
  ByoModulePkg/Setup/PlatformSetupDxe/PlatformSetupDxe.inf
  ByoModulePkg/Setup/PostErrorDxe/PostErrorDxe.inf
  ByoModulePkg/Setup/PostLogin/PostLogin.inf
  ByoModulePkg/Setup/SetupBrowserDxe/SetupBrowserDxe.inf
  ByoModulePkg/Setup/SetupBrowserDxe/SetupBrowserRedfishDxe.inf
  ByoModulePkg/Setup/SetupMouse/SetupMouse.inf
  ByoModulePkg/Setup/SystemPassword/SystemPassword.inf
  ByoModulePkg/Setup/UnicodeFontDxe/UnicodeFontDxe.inf
  ByoModulePkg/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf
  ByoModulePkg/StatusCodeHandler/VerifyErrorMsgDxe/VerifyErrorMsgDxe.inf
  ByoModulePkg/StatusCodeHandler/EarlyVideoOutHandlerDxe/EarlyVideoOutHandlerDxe.inf
  ByoModulePkg/Universal/ByoSmbiosTable/ByoSmbiosTable.inf
  ByoModulePkg/Universal/MonotonicCounterRuntimeCmosDxe/MonotonicCounterRuntimeDxe.inf
  ByoModulePkg/Universal/PlatformBootManagerDxe/PlatformBootManagerDxeEdk2.inf
  ByoModulePkg/Universal/SaveDevicePciConfig/SaveDevicePciConfigDxe.inf
  ByoModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  ByoModulePkg/Library/BasePciLibPciSegment/BasePciLibPciSegment.inf
  ByoModulePkg/Library/UsbListLib/UsbListLib.inf
  ByoModulePkg/Library/BaseSpiDeviceLibNull/BaseSpiDeviceLibNull.inf
  ByoModulePkg/Library/BaseUefiVariableHelperLib/BaseUefiVariableHelperLib.inf
  ByoModulePkg/Library/ByoPlatformPcdListLibNull/ByoPlatformPcdListLibNull.inf
  ByoModulePkg/Library/MmioTMLib/MmioTMLib.inf
  ByoModulePkg/Library/PlatformLanguageLib/PlatformLanguageLibNull.inf
  ByoModulePkg/Library/SpiTMLib/SpiTMLib.inf
  ByoModulePkg/Logo/LogoPngDxe.inf
  ByoModulePkg/Security/DhcpGetNetworkId/DhcpGetNetworkId.inf
  ByoModulePkg/Security/HddPassword/Dxe/HddPasswordGuiDxe.inf
  ByoModulePkg/Setup/PngDecoderDxe/PngDecoder.inf
  ByoModulePkg/Setup/QrEncoderDxe/QrEncoderDxe.inf
  ByoModulePkg/Setup/ScreenShotDxe/ScreenShot.inf
  ByoModulePkg/Universal/BootMaintenanceManagerDxe/BootMaintenanceManagerDxeOnlyUefi.inf
  ByoModulePkg/ByoBdsBootManagerDxe/ByoBdsBootManagerDxeOnlyUefi.inf
  ByoModulePkg/Library/ByoUefiBootManagerLib/ByoUefiBootManagerLib.inf
  ByoModulePkg/Library/PlatformSecureLibStandalone/PlatformSecureLib.inf
  ByoModulePkg/Setup/NetworkDxe/NetworkDxe.inf

[BuildOptions]
  *_*_*_ASMLINK_PATH     = $(WORKSPACE)/ByoTools/Bin/Link16.exe
  *_*_*_CC_FLAGS         = -D DISABLE_NEW_DEPRECATED_INTERFACES
