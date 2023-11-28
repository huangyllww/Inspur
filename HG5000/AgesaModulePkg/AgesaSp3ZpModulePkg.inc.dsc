#;*****************************************************************************
#;
#; 
#; Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
#; 
#; HYGON is granting you permission to use this software (the Materials)
#; pursuant to the terms and conditions of your Software License Agreement
#; with HYGON.  This header does *NOT* give you permission to use the Materials
#; or any rights under HYGON's intellectual property.  Your use of any portion
#; of these Materials shall constitute your acceptance of those terms and
#; conditions.  If you do not agree to the terms and conditions of the Software
#; License Agreement, please do not use any portion of these Materials.
#; 
#; CONFIDENTIALITY:  The Materials and all other information, identified as
#; confidential and provided to you by HYGON shall be kept confidential in
#; accordance with the terms and conditions of the Software License Agreement.
#; 
#; LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
#; PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
#; WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
#; MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
#; OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
#; IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
#; (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
#; INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
#; GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
#; RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
#; THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
#; EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
#; THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
#; 
#; HYGON does not assume any responsibility for any errors which may appear in
#; the Materials or any other related information provided to you by HYGON, or
#; result from use of the Materials or any related information.
#; 
#; You agree that you will not reverse engineer or decompile the Materials.
#; 
#; NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
#; further information, software, technical information, know-how, or show-how
#; available to you.  Additionally, HYGON retains the right to modify the
#; Materials at any time, without notice, and is not obligated to provide such
#; modified Materials to you.
#; 
#; AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
#; the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
#; the right to make the modified version available for use with HYGON's PRODUCT.
#;*****************************************************************************


[LibraryClasses.Common.PEIM]
  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapPeiLib/AmdHeapPeiLib.inf
  FabricResourceSizeForEachDieLib|AgesaModulePkg/Library/PeiFabricResourceSizeForEachDieLib/PeiFabricResourceSizeForEachDieLib.inf
  CcxSetMmioCfgBaseLib|AgesaModulePkg/Library/CcxSetMmioCfgBaseLib/CcxSetMmioCfgBaseLib.inf
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
  AmdCalloutLib|AgesaModulePkg/Library/AmdCalloutLib/AmdCalloutLib.inf

  ## IDS LIB
  CcxZenIdsHookLibPei|AgesaModulePkg/Library/CcxZenZpIdsHookLib/Pei/CcxZenZpIdsHookLibPei.inf
  FabricIdsHookZpLibPei|AgesaModulePkg/Library/FabricIdsHookZpLib/Pei/FabricIdsHookZpLibPei.inf

[LibraryClasses.Common.DXE_DRIVER]
  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/S3Save/AmdS3SaveLib.inf
  IdsLib|AgesaModulePkg/Library/IdsDxeLib/IdsDxeLib.inf
  FabricResourceSizeForEachDieLib|AgesaModulePkg/Library/DxeFabricResourceSizeForEachDieLib/DxeFabricResourceSizeForEachDieLib.inf
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

[LibraryClasses.Common.DXE_SMM_DRIVER]
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapDxeLib/AmdHeapDxeLib.inf
  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/S3Save/AmdS3SaveLib.inf
  FabricResourceSizeForEachDieLib|AgesaModulePkg/Library/DxeFabricResourceSizeForEachDieLib/DxeFabricResourceSizeForEachDieLib.inf
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

[LibraryClasses.common.PEI_CORE]
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapPeiLib/AmdHeapPeiLib.inf
  AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Pei/AmdTableSmnPeiLib.inf

[LibraryClasses]
  #
  # Agesa specific common libraries
  #
  AmdBaseLib|AgesaModulePkg/Library/AmdBaseLib/AmdBaseLib.inf
  FabricResourceSizeForEachDieLib|AgesaModulePkg/Library/DxeFabricResourceSizeForEachDieLib/DxeFabricResourceSizeForEachDieLib.inf
  AmdIdsDebugPrintLib|AgesaModulePkg/Library/AmdIdsDebugPrintLib/AmdIdsDebugPrintLib.inf
  AmdIdsHookLib|AgesaModulePkg/Library/AmdIdsHookLib/AmdIdsHookLib.inf
  AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
  IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  IdsMiscLib|AgesaModulePkg/Library/IdsMiscLib/IdsMiscLib.inf
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
  FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerZenZpLib/FabricResourceManagerZenZpLib.inf

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
  FchInitHookLib|AgesaModulePkg/Library/FchInitHookLib/FchInitHookLibDxe.inf
  FchIdsHookLib|AgesaModulePkg/Library/FchIdsHookLib/FchIdsHookLib.inf

  ## Ras Lib
  RasBaseLib|AgesaModulePkg/Library/RasBaseLib/RasBaseLib.inf
  OemAgesaCcxPlatformLib|AgesaPkg/Addendum/Ccx/OemAgesaCcxPlatformLibNull/OemAgesaCcxPlatformLibNull.inf
  OemGpioResetControlLib|AgesaPkg/Addendum/Nbio/OemGpioResetControlLibNull/OemGpioResetControlLibNull.inf

[Components.IA32]
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
  AgesaModulePkg/Mem/AgesaMemPei/AgesaMemPei.inf
  AgesaModulePkg/Mem/AmdMemChanXLatZpPei/MemChanXLatZpPei.inf
  AgesaModulePkg/Mem/AmdMemSmbiosV2Pei/MemSmbiosV2Pei.inf
  AgesaModulePkg/Mem/AmdMbistPei/AmdMbistPei.inf
  AgesaModulePkg/ErrorLog/AmdErrorLogPei/AmdErrorLogPei.inf
  AgesaModulePkg/Universal/AmdMemoryHobInfoPeim/AmdMemoryHobInfoPeim.inf
  AgesaModulePkg/Universal/Version/AmdVersionPei/AmdVersionPei.inf

[Components.X64]
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
  AgesaModulePkg/Psp/AmdPspDxeV1/AmdPspDxeV1.inf
  AgesaModulePkg/Psp/AmdPspDxeV2/AmdPspDxeV2.inf
  AgesaModulePkg/Psp/AmdPspP2CmboxV2/AmdPspP2CmboxV2.inf
  AgesaModulePkg/Psp/AmdPspSmmV2/AmdPspSmmV2.inf
  AgesaModulePkg/Psp/AmdHstiV2/AmdHstiV2Src.inf
  AgesaModulePkg/Ccx/Zen/CcxZenZpDxe/AmdCcxZenZpDxe.inf {
    <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesZpLib/CcxResetTablesZpLib.inf
    FabricWdtLib|AgesaModulePkg/Library/FabricWdtZpLib/FabricWdtZpLib.inf
    CcxIdsCustomPstatesLib|AgesaModulePkg/Library/CcxIdsCustomPstateZenLib/CcxIdsCustomPstatesZenLib.inf
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
  AgesaModulePkg/Fch/Taishan/FchTaishanDxe/FchTaishanSsdt.inf
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
  AgesaModulePkg/JedecNvdimm/JedecNvdimm.inf
  #AgesaPkg/Addendum/Nbio/ServerHotplugDxe/ServerHotplugDxe.inf

