#;*****************************************************************************
#;
#; 
#; Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
#; 
#;*****************************************************************************

[LibraryClasses.common.SEC]
  HygonHeapLib|HgpiModulePkg/Library/HygonHeapLibNull/HygonHeapLibNull.inf

[LibraryClasses.Common.PEIM]
  HygonS3SaveLib|HgpiModulePkg/Library/HygonS3SaveLib/WOS3Save/HygonWOS3SaveLib.inf
  HygonHeapLib|HgpiModulePkg/Library/HygonHeapPeiLib/HygonHeapPeiLib.inf
  FabricResourceSizeForEachRbLib|HgpiModulePkg/Library/PeiFabricResourceSizeForEachRbLib/PeiFabricResourceSizeForEachRbLib.inf
  CcxSetMmioCfgBaseLib|HgpiModulePkg/Library/CcxSetMmioCfgBaseLib/CcxSetMmioCfgBaseLib.inf
  IdsLib|HgpiModulePkg/Library/IdsPeiLib/IdsPeiLib.inf
  FchPeiLib|HgpiModulePkg/Library/FchPeiLib/FchPeiLib.inf
  FchTaishanPeiHyExLib|HgpiModulePkg/Fch/HyEx/FchTaishanCore/FchTaishanLibPei.inf
  FchTaishanPeiHyGxLib|HgpiModulePkg/Fch/HyGx/FchTaishanCore/FchTaishanLibPei.inf
  FchInitHookLib|HgpiModulePkg/Library/FchInitHookLib/FchInitHookLibPei.inf
  GnbHeapLib|HgpiModulePkg/Library/GnbHeapPeiLib/GnbHeapPeiLib.inf
  HpcbLib|HgpiModulePkg/Library/HpcbPeiLib/HpcbPeiLib.inf
  HygonPspCommonLib|HgpiModulePkg/Library/HygonPspCommonLibPei/HygonPspCommonLibPei.inf
  PeiSocLogicalIdServicesLib|HgpiModulePkg/Library/PeiSocLogicalIdServicesLib/PeiSocLogicalIdServicesLib.inf
  HygonTableSmnLib|HgpiModulePkg/Library/HygonTableLib/Pei/HygonTableSmnPeiLib.inf
  HygonCapsuleLib|HgpiModulePkg/Library/HygonCapsuleLibPei/HygonCapsuleLibPei.inf
  PeiSocBistLoggingLib|HgpiModulePkg/Library/PeiSocBistLoggingLib/PeiSocBistLoggingLib.inf
  ## MEM Libs
  HygonMemSmbiosLib|HgpiModulePkg/Library/MemSmbiosLib/MemSmbiosLib.inf
  HygonCalloutLib|HgpiModulePkg/Library/HygonCalloutLib/HygonCalloutLib.inf

  ## IDS LIB
  CcxDharmaIdsHookLibPei|HgpiModulePkg/Library/CcxDharmaSatIdsHookLib/Pei/CcxDharmaSatIdsHookLibPei.inf
  FabricIdsHookStLibPei|HgpiModulePkg/Library/FabricIdsHookStLib/Pei/FabricIdsHookStLibPei.inf

[LibraryClasses.Common.DXE_DRIVER]
!if gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiS3Enable
  HygonS3SaveLib|HgpiModulePkg/Library/HygonS3SaveLib/S3Save/HygonS3SaveLib.inf
!else
  HygonS3SaveLib|HgpiModulePkg/Library/HygonS3SaveLib/S3SaveNull/HygonS3SaveLibNull.inf
!endif
  IdsLib|HgpiModulePkg/Library/IdsDxeLib/IdsDxeLib.inf
  FabricResourceSizeForEachRbLib|HgpiModulePkg/Library/DxeFabricResourceSizeForEachRbLib/DxeFabricResourceSizeForEachRbLib.inf
  HygonHeapLib|HgpiModulePkg/Library/HygonHeapDxeLib/HygonHeapDxeLib.inf
  GnbHeapLib|HgpiModulePkg/Library/GnbHeapDxeLib/GnbHeapDxeLib.inf
  HygonPspCommonLib|HgpiModulePkg/Library/HygonPspCommonLibDxe/HygonPspCommonLibDxe.inf
  HygonPspFlashAccLib|HgpiModulePkg/Library/HygonPspFlashAccLibDxe/HygonPspFlashAccLibDxe.inf
  HygonPspFlashUpdateLib|HgpiModulePkg/Library/HygonPspFlashUpdateLib/HygonPspFlashUpdateLib.inf
  CcxDharmaIdsHookLibDxe|HgpiModulePkg/Library/CcxDharmaSatIdsHookLib/Dxe/CcxDharmaSatIdsHookLibDxe.inf
  HpcbLib|HgpiModulePkg/Library/HpcbDxeLib/HpcbDxeLib.inf
  DxeSocLogicalIdServicesLib|HgpiModulePkg/Library/DxeSocLogicalIdServicesLib/DxeSocLogicalIdServicesLib.inf
  HygonTableSmnLib|HgpiModulePkg/Library/HygonTableLib/Dxe/HygonTableSmnDxeLib.inf
  HygonCapsuleLib|HgpiModulePkg/Library/HygonCapsuleLibDxe/HygonCapsuleLibDxe.inf
  CcxMpServicesLib|HgpiModulePkg/Library/CcxMpServicesDxeLib/CcxMpServicesDxeLib.inf

  ## IDS LIB
  FabricIdsHookStLibDxe|HgpiModulePkg/Library/FabricIdsHookStLib/Dxe/FabricIdsHookStLibDxe.inf
  FchInitHookLib|HgpiModulePkg/Library/FchInitHookLib/FchInitHookLibDxe.inf

[LibraryClasses.Common.DXE_SMM_DRIVER]
  HygonHeapLib|HgpiModulePkg/Library/HygonHeapDxeLib/HygonHeapDxeLib.inf
!if gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiS3Enable
  HygonS3SaveLib|HgpiModulePkg/Library/HygonS3SaveLib/S3Save/HygonS3SaveLib.inf
!else
  HygonS3SaveLib|HgpiModulePkg/Library/HygonS3SaveLib/S3SaveNull/HygonS3SaveLibNull.inf
!endif
  FabricResourceSizeForEachRbLib|HgpiModulePkg/Library/DxeFabricResourceSizeForEachRbLib/DxeFabricResourceSizeForEachRbLib.inf
#@todo add specifiic SMM Lib instance, DXE Lib is depend on gBS service exisitance
  IdsLib|HgpiModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  HygonPspCommonLib|HgpiModulePkg/Library/HygonPspCommonLibDxe/HygonPspCommonLibDxe.inf
  HygonPspFlashUpdateLib|HgpiModulePkg/Library/HygonPspFlashUpdateLib/HygonPspFlashUpdateLib.inf
  CcxDharmaIdsHookLibSmm|HgpiModulePkg/Library/CcxDharmaSatIdsHookLib/Smm/CcxDharmaSatIdsHookLibSmm.inf
  HpcbLib|HgpiModulePkg/Library/HpcbDxeLib/HpcbDxeLib.inf
  HygonCapsuleLib|HgpiModulePkg/Library/HygonCapsuleLibDxe/HygonCapsuleLibDxe.inf
  HygonTableSmnLib|HgpiModulePkg/Library/HygonTableLib/Dxe/HygonTableSmnDxeLib.inf
  CcxMpServicesLib|HgpiModulePkg/Library/CcxMpServicesSmmLib/CcxMpServicesSmmLib.inf
  RasSmmLib|HgpiModulePkg/Library/RasSmmLib/RasSmmLib.inf

[LibraryClasses.common.PEI_CORE]
  HygonHeapLib|HgpiModulePkg/Library/HygonHeapPeiLib/HygonHeapPeiLib.inf
  HygonTableSmnLib|HgpiModulePkg/Library/HygonTableLib/Pei/HygonTableSmnPeiLib.inf

[LibraryClasses]
  #
  # Hgpi specific common libraries
  #
  HygonBaseLib|HgpiModulePkg/Library/HygonBaseLib/HygonBaseLib.inf
  FabricResourceSizeForEachRbLib|HgpiModulePkg/Library/DxeFabricResourceSizeForEachRbLib/DxeFabricResourceSizeForEachRbLib.inf
  HygonIdsDebugPrintLib|HgpiModulePkg/Library/HygonIdsDebugPrintLib/HygonIdsDebugPrintLib.inf
  HygonIdsHookLib|HgpiModulePkg/Library/HygonIdsHookLib/HygonIdsHookLib.inf
  HygonIdsHookExtLib|HgpiModulePkg/Library/HygonIdsExtLibNull/HygonIdsHookExtLibNull.inf
  IdsLib|HgpiModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  IdsMiscLib|HgpiModulePkg/Library/IdsMiscLib/IdsMiscLib.inf
  HygonSocBaseLib|HgpiModulePkg/Library/HygonSocBaseLib/HygonSocBaseLib.inf
  HygonSmnAddressLib|HgpiModulePkg/Library/HygonSmnAddressLib/HygonSmnAddressLib.inf
  BaseSocLogicalIdXlatLib|HgpiModulePkg/Library/BaseSocLogicalIdXlatDmLib/BaseSocLogicalIdXlatDmLib.inf
  BaseSocketLogicalIdLib|HgpiModulePkg/Library/BaseSocketLogicalIdDmLib/BaseSocketLogicalIdDmLib.inf
  HygonErrorLogLib|HgpiModulePkg/Library/HygonErrorLogLib/HygonErrorLogLib.inf
  HygonTableLib|HgpiModulePkg/Library/HygonTableLib/HygonTableLib.inf
  SocCmnIdsHookLibPei|HgpiModulePkg/Library/SocCmnIdsHookLib/Pei/SocCmnIdsHookLibPei.inf
  SocCmnIdsHookLibDxe|HgpiModulePkg/Library/SocCmnIdsHookLib/Dxe/SocCmnIdsHookLibDxe.inf
  BaseCoreLogicalIdLib|HgpiModulePkg/Library/BaseCoreLogicalIdX86Lib/BaseCoreLogicalIdX86Lib.inf
  HygonS3SaveLib|HgpiModulePkg/Library/HygonS3SaveLib/WOS3Save/HygonWOS3SaveLib.inf

  ## PSP Libs
  HygonPspBaseLib|HgpiModulePkg/Library/HygonPspBaseLib/HygonPspBaseLib.inf
  HygonPspMboxLib|HgpiModulePkg/Library/HygonPspMboxLib/HygonPspMboxLib.inf
  HygonPspHpobLib|HgpiModulePkg/Library/HygonPspHpobLib/HygonPspHpobLib.inf
  HygonPspFlashAccLib|HgpiModulePkg/Library/HygonPspFlashAccLibNull/HygonPspFlashAccLibNull.inf

  ## CCX Lib
  CcxBaseX86Lib|HgpiModulePkg/Library/CcxBaseX86Lib/CcxBaseX86Lib.inf
  CcxBistLib|HgpiModulePkg/Library/CcxBistLib/CcxBistLib.inf
  CcxPspLib|HgpiModulePkg/Library/CcxPspLib/CcxPspLib.inf
  CcxHaltLib|HgpiModulePkg/Library/CcxHaltLib/CcxHaltLib.inf
  CcxMicrocodePatchLib|HgpiModulePkg/Library/CcxMicrocodePatchLib/CcxMicrocodePatchLib.inf
  CcxRolesLib|HgpiModulePkg/Library/CcxRolesX86Lib/CcxRolesX86Lib.inf
  CcxResetTablesLib|HgpiModulePkg/Library/CcxResetTablesDmLib/CcxResetTablesDmLib.inf
  CcxSetMcaLib|HgpiModulePkg/Library/CcxSetMcaSatLib/CcxSetMcaSatLib.inf
  CcxPstatesLib|HgpiModulePkg/Library/CcxPstatesDharmaSatLib/CcxPstatesDharmaSatLib.inf
  CcxSmbiosLib|HgpiModulePkg/Library/CcxSmbiosDharmaSatLib/CcxSmbiosDharmaSatLib.inf
  CcxStallLib|HgpiModulePkg/Library/CcxStallDharmaLib/CcxStallDharmaLib.inf
  DxeCcxBaseX86ServicesLib|HgpiModulePkg/Library/DxeCcxBaseX86ServicesLib/DxeCcxBaseX86ServicesLib.inf

  ## DF Lib
  FabricPstatesLib|HgpiModulePkg/Library/FabricPstatesStLib/FabricPstatesStLib.inf
  BaseFabricTopologyLib|HgpiModulePkg/Library/BaseFabricTopologyStLib/BaseFabricTopologyStLib.inf
  PeiFabricTopologyServicesLib|HgpiModulePkg/Library/PeiFabricTopologyServicesLib/PeiFabricTopologyServicesLib.inf
  DxeFabricTopologyServicesLib|HgpiModulePkg/Library/DxeFabricTopologyServicesLib/DxeFabricTopologyServicesLib.inf
  SmmFabricTopologyServicesLib|HgpiModulePkg/Library/SmmFabricTopologyServicesLib/SmmFabricTopologyServicesLib.inf
  FabricRegisterAccLib|HgpiModulePkg/Library/FabricRegisterAccStLib/FabricRegisterAccStLib.inf
  FabricResourceManagerLib|HgpiModulePkg/Library/FabricResourceManagerStLib/FabricResourceManagerStLib.inf

  ## MEM Lib
  HygonMemBaseLib|HgpiModulePkg/Library/MemBaseLib/HygonMemBaseLib.inf

  ## Gnb Lib
  GnbCommonLib|HgpiModulePkg/Library/GnbCommonLib/GnbCommonLib.inf
  GnbMemAccLib|HgpiModulePkg/Library/GnbMemAccLib/GnbMemAccLib.inf
  GnbIoAccLib|HgpiModulePkg/Library/GnbIoAccLib/GnbIoAccLib.inf
  GnbPciAccLib|HgpiModulePkg/Library/GnbPciAccLib/GnbPciAccLib.inf
  GnbCpuAccLib|HgpiModulePkg/Library/GnbCpuAccLib/GnbCpuAccLib.inf
  GnbPciLib|HgpiModulePkg/Library/GnbPciLib/GnbPciLib.inf
  GnbCxlLib|HgpiModulePkg/Library/GnbCxlLib/GnbCxlLib.inf
  GnbInternalPciLib|HgpiModulePkg/Library/GnbInternalPciLib/GnbInternalPciLib.inf
  GnbLib|HgpiModulePkg/Library/GnbLib/GnbLib.inf
  NbioHandleLib|HgpiModulePkg/Library/NbioHandleLib/NbioHandleLib.inf
  NbioIommuIvrsLib|HgpiModulePkg/Library/NbioIommuIvrsLib/NbioIommuIvrsLib.inf
  PcieConfigLib|HgpiModulePkg/Library/PcieConfigLib/PcieConfigLib.inf
  GnbSSocketLib|HgpiModulePkg/Library/GnbSSocketLib/GnbSSocketLib.inf
  NbioRegisterAccLib|HgpiModulePkg/Library/NbioRegisterAccLib/NbioRegisterAcc.inf
  NbioSmuLib|HgpiModulePkg/Library/NbioSmuLib/NbioSmuLib.inf
  NbioIdsHookSTLib|HgpiModulePkg/Library/NbioIdsHookSTLib/NbioIdsHookSTLib.inf
  HsioServiceLib|HgpiModulePkg/Library/HsioServiceLib/HsioServiceLib.inf
  PcieMiscCommLib|HgpiModulePkg/Library/PcieMiscCommLib/PcieMiscCommLib.inf
  NbioAzaliaLib|HgpiModulePkg/Library/NbioAzaliaLib/NbioAzaliaLib.inf
  SmnAccessLib|HgpiModulePkg/Library/SmnAccessLib/SmnAccessLib.inf

  ## Fch Lib
  FchBaseLib|HgpiModulePkg/Library/FchBaseLib/FchBaseLib.inf
  FchDxeLib|HgpiModulePkg/Library/FchDxeLib/FchDxeLib.inf
  FchSmmLib|HgpiModulePkg/Library/FchSmmLib/FchSmmLib.inf
  FchTaishanDxeHyExLib|HgpiModulePkg/Fch/HyEx/FchTaishanCore/FchTaishanLibDxe.inf
  FchTaishanDxeHyGxLib|HgpiModulePkg/Fch/HyGx/FchTaishanCore/FchTaishanLibDxe.inf
  FchInitHookLib|HgpiModulePkg/Library/FchInitHookLib/FchInitHookLibDxe.inf
  FchIdsHookLib|HgpiModulePkg/Library/FchIdsHookLib/FchIdsHookLib.inf
  FchCommonLib|HgpiModulePkg/Fch/Common/Library/FchCommonLib/FchCommonLib.inf

  ## Ras Lib
  RasBaseLib|HgpiModulePkg/Library/RasBaseLib/RasBaseLib.inf
  RasMcaLib|HgpiModulePkg/Library/RasMcaLib/RasMcaLib.inf
  OemHgpiCcxPlatformLib|HgpiPkg/Addendum/Ccx/OemHgpiCcxPlatformLib/OemHgpiCcxPlatformLib.inf
  OemGpioResetControlLib|HgpiPkg/Addendum/Nbio/OemGpioResetControlLib/OemGpioResetControlLib.inf

[Components.IA32]
  HgpiModulePkg/Debug/HygonIdsDebugPrintPei/HygonIdsDebugPrintPei.inf

  ##PSP Drivers
  HgpiModulePkg/Psp/HygonPspPei/HygonPspPei.inf
  HgpiModulePkg/Ccx/Dharma/CcxDharmaPei/HygonCcxDharmaPei.inf {
    <LibraryClasses>
    CcxResetTablesLib|HgpiModulePkg/Library/CcxResetTablesDmLib/CcxResetTablesDmLib.inf
    IdsLib|HgpiModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  }
  HgpiModulePkg/Fabric/HyEx/FabricPei/HygonFabricPei.inf {
    <LibraryClasses>
    IdsLib|HgpiModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  }
  HgpiModulePkg/Fabric/HyGx/FabricPei/HygonFabricPei.inf {
    <LibraryClasses>
    IdsLib|HgpiModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  }

  HgpiModulePkg/Nbio/HyEx/SMU/HygonNbioSmuPei/HygonNbioSmuPei.inf
  HgpiModulePkg/Nbio/HyGx/SMU/HygonNbioSmuPei/HygonNbioSmuPei.inf
  HgpiModulePkg/Nbio/HyEx/PCIe/HygonNbioPciePei/HygonNbioPciePei.inf
  HgpiModulePkg/Nbio/HyGx/PCIe/HygonNbioPciePei/HygonNbioPciePei.inf
  HgpiModulePkg/Nbio/HyEx/NbioBase/HygonNbioBasePei/HygonNbioBasePei.inf
  HgpiModulePkg/Nbio/HyGx/NbioBase/HygonNbioBasePei/HygonNbioBasePei.inf
  HgpiModulePkg/Nbio/IOMMU/HygonNbioIOMMUPei/HygonNbioIOMMUPei.inf
  HgpiModulePkg/Nbio/IOAPIC/HygonNbioIOAPICSTPei/HygonNbioIOAPICSTPei.inf
  HgpiModulePkg/Nbio/HyGx/CXL/HygonCxlPei/HygonCxlPei.inf
  HgpiModulePkg/Fch/HyEx/FchTaishanPei/FchPei.inf
  HgpiModulePkg/Fch/HyGx/FchTaishanPei/FchPei.inf
  HgpiModulePkg/Fch/Common/FchTaishanSmbusPei/Smbus.inf
  HgpiModulePkg/Fch/HyEx/FchTaishanMultiFchPei/FchMultiFchPei.inf
  HgpiModulePkg/Fch/HyGx/FchTaishanMultiFchPei/FchMultiFchPei.inf
  HgpiModulePkg/Mem/HyEx/HygonMemPei/HygonMemPei.inf
  HgpiModulePkg/Mem/HyGx/HygonMemPei/HygonMemPei.inf
  HgpiModulePkg/Soc/HygonSocPei/HygonSocPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|HgpiModulePkg/Library/FabricResourceManagerStLib/FabricResourceInitStLib.inf
    HygonIdsHookExtLib|HgpiModulePkg/Library/HygonIdsExtLibNull/HygonIdsHookExtLibNull.inf
    PeiSocBistLib|HgpiModulePkg/Library/PeiSocBistStLib/PeiSocBistStLib.inf
  }
  HgpiModulePkg/Mem/HgpiMemPei/HgpiMemPei.inf
  HgpiModulePkg/Mem/HyEx/HygonMemChanXLatPei/MemChanXLatPei.inf
  HgpiModulePkg/Mem/HyGx/HygonMemChanXLatPei/MemChanXLatPei.inf
  HgpiModulePkg/Mem/HygonMemSmbiosPei/MemSmbiosPei.inf
  HgpiModulePkg/ErrorLog/HygonErrorLogPei/HygonErrorLogPei.inf
  HgpiModulePkg/Universal/HygonMemoryHobInfoPeim/HygonMemoryHobInfoPeim.inf
  HgpiModulePkg/Universal/Version/HygonVersionPei/HygonVersionPei.inf

[Components.X64]
  HgpiModulePkg/Debug/HygonIdsDebugPrintDxe/HygonIdsDebugPrintDxe.inf
  HgpiModulePkg/Mem/HygonMemSmbiosDxe/HygonMemSmbiosDxe.inf
  HgpiModulePkg/Psp/HpcbDrv/HpcbSatDxe/HpcbSatDxe.inf{
    <LibraryClasses>
    HpcbLib|HgpiModulePkg/Library/HpcbDxeLib/HpcbDxeLib.inf
  }
  HgpiModulePkg/Psp/HpcbDrv/HpcbSatSmm/HpcbSatSmm.inf{
    <LibraryClasses>
    HpcbLib|HgpiModulePkg/Library/HpcbDxeLib/HpcbDxeLib.inf
  }
  HgpiModulePkg/Psp/HygonPspDxe/HygonPspDxe.inf
  HgpiModulePkg/Psp/HygonPspP2Cmbox/HygonPspP2Cmbox.inf
  HgpiModulePkg/Psp/HygonPspSmm/HygonPspSmm.inf
  HgpiModulePkg/Psp/HygonHsti/HygonHstiSrc.inf
  HgpiModulePkg/Ccx/Dharma/CcxDharmaDxe/HygonCcxDharmaDxe.inf {
    <LibraryClasses>
    IdsLib|HgpiModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    CcxResetTablesLib|HgpiModulePkg/Library/CcxResetTablesDmLib/CcxResetTablesDmLib.inf
    FabricWdtLib|HgpiModulePkg/Library/FabricWdtStLib/FabricWdtStLib.inf
    CcxIdsCustomPstatesLib|HgpiModulePkg/Library/CcxIdsCustomPstateDmLib/CcxIdsCustomPstatesDmLib.inf
  }
  HgpiModulePkg/Ccx/Dharma/CcxDharmaSmm/HygonCcxDharmaSmm.inf
  HgpiModulePkg/Fabric/HyEx/FabricDxe/HygonFabricDxe.inf {
    <LibraryClasses>
    IdsLib|HgpiModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  }
  HgpiModulePkg/Fabric/HyGx/FabricDxe/HygonFabricDxe.inf {
    <LibraryClasses>
    IdsLib|HgpiModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  }
  HgpiModulePkg/Fabric/HyEx/FabricSmm/HygonFabricSmm.inf {
    <LibraryClasses>
    HygonS3SaveLib|HgpiModulePkg/Library/HygonS3SaveLib/WOS3Save/HygonWOS3SaveLib.inf
  }
  HgpiModulePkg/Fabric/HyGx/FabricSmm/HygonFabricSmm.inf {
    <LibraryClasses>
    HygonS3SaveLib|HgpiModulePkg/Library/HygonS3SaveLib/WOS3Save/HygonWOS3SaveLib.inf
  }
  HgpiModulePkg/Soc/HygonSocDxe/HygonSocDxe.inf {
    <LibraryClasses>
    HygonS3SaveLib|HgpiModulePkg/Library/HygonS3SaveLib/WOS3Save/HygonWOS3SaveLib.inf
  }
  HgpiModulePkg/ErrorLog/HygonErrorLogDxe/HygonErrorLogDxe.inf
  ## Gnb Dxe Drivers
  HgpiModulePkg/Nbio/HyEx/NbioBase/HygonNbioBaseDxe/HygonNbioBaseDxe.inf
  HgpiModulePkg/Nbio/HyGx/NbioBase/HygonNbioBaseDxe/HygonNbioBaseDxe.inf
  HgpiModulePkg/Nbio/HyEx/SMU/HygonNbioSmuDxe/HygonNbioSmuDxe.inf
  HgpiModulePkg/Nbio/HyGx/SMU/HygonNbioSmuDxe/HygonNbioSmuDxe.inf
  HgpiModulePkg/Nbio/HyEx/PCIe/HygonNbioPcieDxe/HygonNbioPcieDxe.inf
  HgpiModulePkg/Nbio/HyGx/PCIe/HygonNbioPcieDxe/HygonNbioPcieDxe.inf
  HgpiModulePkg/Nbio/IOMMU/HygonNbioIOMMUDxe/HygonNbioIOMMUDxe.inf
  HgpiModulePkg/Nbio/NbioALib/HygonNbioAlibSTDxe/HygonNbioAlibSTDxe.inf
  HgpiModulePkg/Nbio/HyGx/IOHC/IohcSmmSwSmiDispatcher/IohcSmmSwSmiDispatcher.inf
  HgpiModulePkg/Nbio/HyGx/CXL/HygonCxlDxe/HygonCxlDxe.inf
  HgpiModulePkg/Fch/HyEx/FchTaishanDxe/FchDxe.inf
# HgpiModulePkg/Fch/HyEx/FchTaishanDxe/FchTaishanSsdt.inf                           # byo230823 -
  HgpiModulePkg/Fch/HyEx/FchTaishanSmm/FchSmm.inf
  HgpiModulePkg/Fch/HyGx/FchTaishanDxe/FchDxe.inf
# HgpiModulePkg/Fch/HyGx/FchTaishanDxe/FchTaishanSsdt.inf                           # byo230823 -
  HgpiModulePkg/Fch/HyGx/FchTaishanSmm/FchSmm.inf
  HgpiModulePkg/Fch/Common/FchTaishanSmbusDxe/SmbusLight.inf
  HgpiModulePkg/Fch/Common/FchTaishanCf9ResetDxe/Cf9Reset.inf
  HgpiModulePkg/Fch/Common/FchTaishanLegacyInterruptDxe/LegacyInterrupt.inf
  HgpiModulePkg/Fch/Common/FchTaishanSmmControlDxe/SmmControl.inf
  HgpiModulePkg/Fch/HyEx/FchTaishanSmmDispatcher/FchSmmDispatcher.inf
  HgpiModulePkg/Fch/HyEx/FchTaishanMultiFchDxe/FchMultiFchDxe.inf
  HgpiModulePkg/Fch/HyEx/FchTaishanMultiFchSmm/FchMultiFchSmm.inf
  HgpiModulePkg/Fch/HyGx/FchTaishanSmmDispatcher/FchSmmDispatcher.inf
  HgpiModulePkg/Fch/HyGx/FchTaishanMultiFchDxe/FchMultiFchDxe.inf
  HgpiModulePkg/Fch/HyGx/FchTaishanMultiFchSmm/FchMultiFchSmm.inf

  # Universal
  HgpiModulePkg/Universal/Smbios/HygonSmbiosDxe.inf
  HgpiModulePkg/Universal/Acpi/HygonAcpiDxe.inf
  HgpiModulePkg/Universal/Acpi/HygonAcpiHmatService.inf
  HgpiModulePkg/Universal/HygonSmmCommunication/HygonSmmCommunication.inf
  HgpiModulePkg/Universal/Version/HygonVersionDxe/HygonVersionDxe.inf

  ## Ras Dxe Drivers
!if $(gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdRunEnvironment) != 2
  HgpiModulePkg/Ras/HygonRasApeiDxe/HygonRasApeiDxe.inf
  HgpiModulePkg/Ras/HygonRasDxe/HygonRasDxe.inf
  HgpiModulePkg/Ras/HygonRasSmm/HygonRasSmm.inf
!endif
  ## Snp Dxe Drivers (Uncomment SnpDxePort0.inf for xGBE support)
  # HgpiModulePkg/SnpDxe/SnpDxePort.inf
  ## NVDIMM Dxe Driver
  HgpiModulePkg/JedecNvdimm/JedecNvdimm.inf
  #HgpiPkg/Addendum/Nbio/ServerHotplugDxe/ServerHotplugDxe.inf
