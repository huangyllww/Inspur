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
  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf                                         <ALL>
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapPeiLib/AmdHeapPeiLib.inf                                                    <ALL>
  FabricResourceSizeForEachDieLib|AgesaModulePkg/Library/PeiFabricResourceSizeForEachDieLib/PeiFabricResourceSizeForEachDieLib.inf   <ALL>
  CcxSetMmioCfgBaseLib|AgesaModulePkg/Library/CcxSetMmioCfgBaseLib/CcxSetMmioCfgBaseLib.inf                            <ALL>
  IdsLib|AgesaModulePkg/Library/IdsPeiLib/IdsPeiLib.inf                                                                <ALL>
  FchPeiLibV9|AgesaModulePkg/Library/FchPeiLib/FchPeiLib.inf                                                           <ALL>
  FchTaishanPeiLib|AgesaModulePkg/Fch/Taishan/FchTaishanCore/FchTaishanLibPei.inf                                      <ZP>
  FchSandstonePeiLib|AgesaModulePkg/Fch/Sandstone/FchSandstoneCore/FchSandstoneLibPei.inf                              <RV>
  FchKernPeiLib|AgesaModulePkg/Fch/Kern/FchKernCore/FchKernLibPei.inf                                                  <FAMILY15H>
  FchInitHookLibPei|AgesaModulePkg/Library/FchInitHookLib/FchInitHookLibPei.inf                                           <ALL>
  GnbHeapLib|AgesaModulePkg/Library/GnbHeapPeiLib/GnbHeapPeiLib.inf                                                    <ALL>
  AgesaGnbPeiLib|AgesaModulePkg/Library/GnbCZLib/GnbPeiLibCZ.inf                                                       <FAMILY15H>
  AmdPspCommonLib|AgesaModulePkg/Library/AmdPspCommonLibPei/AmdPspCommonLibPei.inf                                     <ALL>
  PeiSocLogicalIdServicesLib|AgesaModulePkg/Library/PeiSocLogicalIdServicesLib/PeiSocLogicalIdServicesLib.inf          <ALL>
  AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Pei/AmdTableSmnPeiLib.inf                                          <ALL>
  AmdCapsuleLib|AgesaModulePkg/Library/AmdCapsuleLibPei/AmdCapsuleLibPei.inf                                           <ALL>
  PeiSocBistLoggingLib|AgesaModulePkg/Library/PeiSocBistLoggingLib/PeiSocBistLoggingLib.inf                            <FAMILY18H>
  ## MEM Libs
  AmdMemBaseLib|AgesaModulePkg/Library/Mem/BaseLib/AmdMemBaseLib.inf                                                   <FAMILY15H>
  AmdMemArdkLib|AgesaModulePkg/Library/Mem/ArdkLib/AmdMemArdkLib.inf                                                   <FAMILY15H>
  AmdMemMainCzLib|AgesaModulePkg/Library/Mem/MainCzLib/AmdMemMainCzLib.inf                                             <FAMILY15H>
  AmdMemTechLib|AgesaModulePkg/Library/Mem/TechLib/AmdMemTechLib.inf                                                   <FAMILY15H>
  AmdMemTechDdr3Lib|AgesaModulePkg/Library/Mem/TechDdr3Lib/AmdMemTechDdr3Lib.inf                                       <FAMILY15H>
  AmdMemTechDdr4Lib|AgesaModulePkg/Library/Mem/TechDdr4Lib/AmdMemTechDdr4Lib.inf                                       <FAMILY15H>
  AmdMemPsCzLib|AgesaModulePkg/Library/Mem/PsCzLib/AmdMemPsCzLib.inf                                                   <FAMILY15H>
  AmdMemPsLib|AgesaModulePkg/Library/Mem/PsLib/AmdMemPsLib.inf                                                         <FAMILY15H>
  AmdMemPsCzAm4Lib|AgesaModulePkg/Library/Mem/PsCzAm4Lib/AmdMemPsCzAm4Lib.inf                                          <FAMILY15H>
  AmdMemPsCzFp4Lib|AgesaModulePkg/Library/Mem/PsCzFp4Lib/AmdMemPsCzFp4Lib.inf                                          <FAMILY15H>
  AmdMemFeatIdentifyDimmLib|AgesaModulePkg/Library/Mem/FeatIdentifyDimmLib/AmdMemFeatIdentifyDimmLib.inf               <FAMILY15H>
  AmdMemFeatDmiCollectLib|AgesaModulePkg/Library/Mem/FeatDmiCollectLib/AmdMemFeatDmiCollectLib.inf                     <FAMILY15H>
  AmdMemFeatMemClrLib|AgesaModulePkg/Library/Mem/FeatMemClrLib/AmdMemFeatMemClrLib.inf                                 <FAMILY15H>
  AmdMemFeatCratLib|AgesaModulePkg/Library/Mem/FeatCratLib/AmdMemFeatCratLib.inf                                       <FAMILY15H>
  AmdMemFeatTableLib|AgesaModulePkg/Library/Mem/FeatTableLib/AmdMemFeatTableLib.inf                                    <FAMILY15H>
  AmdMemFeatEccLib|AgesaModulePkg/Library/Mem/FeatEccLib/AmdMemFeatEccLib.inf                                          <FAMILY15H>
  AmdMemServicesCzLib|AgesaModulePkg/Library/Mem/ServicesCzLib/AmdMemServicesCzLib.inf                                 <FAMILY15H>
  AmdMemFeatUmaLib|AgesaModulePkg/Library/Mem/FeatUmaLib/AmdMemFeatUmaLib.inf                                          <FAMILY15H>
  AmdMemFeatOLSpareLib|AgesaModulePkg/Library/Mem/Null/FeatOLSpareLibNull/AmdMemFeatOLSpareLibNull.inf                 <FAMILY15H>
  AmdMemFeatCsIntLvLib|AgesaModulePkg/Library/Mem/FeatCsIntLvLib/AmdMemFeatCsIntLvLib.inf                              <FAMILY15H>
  AmdMemFeatNodeIntLvLib|AgesaModulePkg/Library/Mem/Null/FeatNodeIntLvLibNull/AmdMemFeatNodeIntLvLibNull.inf           <FAMILY15H>
  AmdMemFeatChIntLvLib|AgesaModulePkg/Library/Mem/FeatChIntLvLib/AmdMemFeatChIntLvLib.inf                              <FAMILY15H>
  AmdMemFeatIntLvRgnLib|AgesaModulePkg/Library/Mem/Null/FeatIntLvRgnLibNull/AmdMemFeatIntLvRgnLibNull.inf              <FAMILY15H>
  AmdMemFeatLvDdr3Lib|AgesaModulePkg/Library/Mem/FeatLvDdr3Lib/AmdMemFeatLvDdr3Lib.inf                                 <FAMILY15H>
  AmdMemFeatOnDimmThermalLib|AgesaModulePkg/Library/Mem/FeatOnDimmThermalLib/AmdMemFeatOnDimmThermalLib.inf            <FAMILY15H>
  AmdMemFeatExcludeDimmLib|AgesaModulePkg/Library/Mem/FeatExcludeDimmLib/AmdMemFeatExcludeDimmLib.inf                  <FAMILY15H>
  AmdMemFeatAmpLib|AgesaModulePkg/Library/Mem/Null/FeatAmpLibNull/AmdMemFeatAmpLibNull.inf                             <FAMILY15H>
  AmdMemReportInfoLib|AgesaModulePkg/Library/Mem/ReportInfoLib/AmdMemReportInfoLib.inf                                 <FAMILY15H>
  FchPromontoryPeiLib|AgesaModulePkg/Fch/Promontory/PromontoryLibrary/FchPromontoryPeiLib/FchPromontoryPeiLib.inf      <AM4BR|AM4SM|SP3r2TR>
  AmdMemSmbiosV2Lib|AgesaModulePkg/Library/MemSmbiosV2Lib/MemSmbiosV2Lib.inf                                           <FAMILY18H>
  AmdCalloutLib|AgesaModulePkg/Library/AmdCalloutLib/AmdCalloutLib.inf                                                 <RV>

  ## IDS LIB
  CcxXvIdsHookLibPei|AgesaModulePkg/Library/CcxXvIdsHookLib/Pei/CcxXvIdsHookLibPei.inf                                 <FAMILY15H>
  FabricIdsHookUnbLibPei|AgesaModulePkg/Library/FabricIdsHookUnbLib/Pei/FabricIdsHookUnbLibPei.inf                     <FAMILY15H>
  MemIdsHookLib|AgesaModulePkg/Library/Mem/MemIdsHookCzLib/MemIdsHookCzLib.inf                                         <FAMILY15H>
  CcxZenIdsHookLibPei|AgesaModulePkg/Library/CcxZenZpIdsHookLib/Pei/CcxZenZpIdsHookLibPei.inf                          <ZP>
  CcxZenIdsHookLibPei|AgesaModulePkg/Library/CcxZenRvIdsHookLib/Pei/CcxZenRvIdsHookLibPei.inf                          <RV>
  FabricIdsHookZpLibPei|AgesaModulePkg/Library/FabricIdsHookZpLib/Pei/FabricIdsHookZpLibPei.inf                        <FAMILY18H>

[LibraryClasses.Common.DXE_DRIVER]
  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/S3Save/AmdS3SaveLib.inf                                             <ALL>
  IdsLib|AgesaModulePkg/Library/IdsDxeLib/IdsDxeLib.inf                                                                <ALL>
  FabricResourceSizeForEachDieLib|AgesaModulePkg/Library/DxeFabricResourceSizeForEachDieLib/DxeFabricResourceSizeForEachDieLib.inf   <ALL>
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapDxeLib/AmdHeapDxeLib.inf                                                    <ALL>
  GnbHeapLib|AgesaModulePkg/Library/GnbHeapDxeLib/GnbHeapDxeLib.inf                                                    <ALL>
  AgesaGnbDxeLib|AgesaModulePkg/Library/GnbCZLib/GnbDxeLibCZ.inf                                                       <FAMILY15H>
  AmdPspCommonLib|AgesaModulePkg/Library/AmdPspCommonLibDxe/AmdPspCommonLibDxe.inf                                     <ALL>
  AmdPspFlashAccLib|AgesaModulePkg/Library/AmdPspFlashAccLibDxe/AmdPspFlashAccLibDxe.inf                               <ALL>
  AmdPspFlashUpdateLib|AgesaModulePkg/Library/AmdPspFlashUpdateLib/AmdPspFlashUpdateLib.inf                            <ALL>
  FchPromontoryDxeLib|AgesaModulePkg/Fch/Promontory/PromontoryLibrary/FchPromontoryDxeLib/FchPromontoryDxeLib.inf      <AM4BR|AM4SM|SP3r2TR>
  CcxZenIdsHookLibDxe|AgesaModulePkg/Library/CcxZenZpIdsHookLib/Dxe/CcxZenZpIdsHookLibDxe.inf                          <ZP>
  CcxZenIdsHookLibDxe|AgesaModulePkg/Library/CcxZenRvIdsHookLib/Dxe/CcxZenRvIdsHookLibDxe.inf                          <RV>
  ApcbLib|AgesaModulePkg/Library/ApcbLib/ApcbLib.inf                                                                   <ALL>
  DxeSocLogicalIdServicesLib|AgesaModulePkg/Library/DxeSocLogicalIdServicesLib/DxeSocLogicalIdServicesLib.inf          <ALL>
  AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Dxe/AmdTableSmnDxeLib.inf                                          <ALL>
  AmdCapsuleLib|AgesaModulePkg/Library/AmdCapsuleLibDxe/AmdCapsuleLibDxe.inf                                           <ALL>
  CcxMpServicesLib|AgesaModulePkg/Library/CcxMpServicesDxeLib/CcxMpServicesDxeLib.inf                                  <ALL>
  CcxSmmAccess2Lib|AgesaModulePkg/Library/DxeCcxSmmAccess2Lib/DxeCcxSmmAccess2Lib.inf                                  <ALL>

  ## MEM Lib
  AmdMemS3InitLib|AgesaModulePkg/Library/Mem/S3InitLib/S3InitLib.inf                                                   <FAMILY15H>
  AmdMemArdkLib|AgesaModulePkg/Library/Mem/Null/ArdkLibNull/AmdMemArdkLibNull.inf                                      <FAMILY15H>
  AmdMemMainCzLib|AgesaModulePkg/Library/Mem/Null/MainCzLibNull/AmdMemMainCzLibNull.inf                                <FAMILY15H>
  AmdMemPsLib|AgesaModulePkg/Library/Mem/Null/PsLibNull/AmdMemPsLibNull.inf                                            <FAMILY15H>
  AmdMemPsCzLib|AgesaModulePkg/Library/Mem/Null/PsCzLibNull/AmdMemPsCzLibNull.inf                                      <FAMILY15H>
  AmdMemPsCzAm4Lib|AgesaModulePkg/Library/Mem/Null/PsCzAm4LibNull/AmdMemPsCzAm4LibNull.inf                             <FAMILY15H>
  AmdMemPsCzFp4Lib|AgesaModulePkg/Library/Mem/Null/PsCzFp4LibNull/AmdMemPsCzFp4LibNull.inf                             <FAMILY15H>
  AmdMemFeatDmiCollectLib|AgesaModulePkg/Library/Mem/Null/FeatDmiCollectLibNull/AmdMemFeatDmiCollectLibNull.inf        <FAMILY15H>
  AmdMemFeatCratLib|AgesaModulePkg/Library/Mem/Null/FeatCratLibNull/AmdMemFeatCratLibNull.inf                          <FAMILY15H>
  AmdMemTechLib|AgesaModulePkg/Library/Mem/Null/TechLibNull/AmdMemTechLibNull.inf                                      <FAMILY15H>
  AmdMemTechDdr3Lib|AgesaModulePkg/Library/Mem/Null/TechDdr3LibNull/AmdMemTechDdr3LibNull.inf                          <FAMILY15H>
  AmdMemTechDdr4Lib|AgesaModulePkg/Library/Mem/Null/TechDdr4LibNull/AmdMemTechDdr4LibNull.inf                          <FAMILY15H>
  AmdMemFeatTableLib|AgesaModulePkg/Library/Mem/Null/FeatTableLibNull/AmdMemFeatTableLibNull.inf                       <FAMILY15H>
  AmdMemFeatIdentifyDimmLib|AgesaModulePkg/Library/Mem/Null/FeatIdentifyDimmLibNull/AmdMemFeatIdentifyDimmLibNull.inf  <FAMILY15H>
  AmdMemFeatMemClrLib|AgesaModulePkg/Library/Mem/Null/FeatMemClrLibNull/AmdMemFeatMemClrLibNull.inf                    <FAMILY15H>
  AmdMemFeatDmiConstructLib|AgesaModulePkg/Library/Mem/FeatDmiConstructLib/MemFeatDmiConstructLib.inf                  <FAMILY15H>

  ## IDS LIB
  CcxXvIdsHookLibDxe|AgesaModulePkg/Library/CcxXvIdsHookLib/Dxe/CcxXvIdsHookLibDxe.inf                                 <FAMILY15H>
  FabricIdsHookUnbLibDxe|AgesaModulePkg/Library/FabricIdsHookUnbLib/Dxe/FabricIdsHookUnbLibDxe.inf                     <FAMILY15H>
  FabricIdsHookZpLibDxe|AgesaModulePkg/Library/FabricIdsHookZpLib/Dxe/FabricIdsHookZpLibDxe.inf                        <FAMILY18H>

[LibraryClasses.Common.DXE_SMM_DRIVER]
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapDxeLib/AmdHeapDxeLib.inf                                                    <ALL>
  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/S3Save/AmdS3SaveLib.inf                                             <ALL>
  FabricResourceSizeForEachDieLib|AgesaModulePkg/Library/DxeFabricResourceSizeForEachDieLib/DxeFabricResourceSizeForEachDieLib.inf   <ALL>
#@todo add specifiic SMM Lib instance, DXE Lib is depend on gBS service exisitance
  IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf                                                        <ALL>
  AmdPspCommonLib|AgesaModulePkg/Library/AmdPspCommonLibDxe/AmdPspCommonLibDxe.inf                                     <ALL>
  AmdPspFlashUpdateLib|AgesaModulePkg/Library/AmdPspFlashUpdateLib/AmdPspFlashUpdateLib.inf                            <ALL>
  CcxZenIdsHookLibSmm|AgesaModulePkg/Library/CcxZenZpIdsHookLib/Smm/CcxZenZpIdsHookLibSmm.inf                          <ZP>
  CcxZenIdsHookLibSmm|AgesaModulePkg/Library/CcxZenRvIdsHookLib/Smm/CcxZenRvIdsHookLibSmm.inf                          <RV>
  ApcbLib|AgesaModulePkg/Library/ApcbLib/ApcbLib.inf                                                                   <ALL>
  AmdCapsuleLib|AgesaModulePkg/Library/AmdCapsuleLibDxe/AmdCapsuleLibDxe.inf                                           <ALL>
  AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Dxe/AmdTableSmnDxeLib.inf                                          <ALL>
  CcxMpServicesLib|AgesaModulePkg/Library/CcxMpServicesSmmLib/CcxMpServicesSmmLib.inf                                  <ALL>
  RasSmmLib|AgesaModulePkg/Library/RasSmmLib/RasSmmLib.inf                                                             <SP3ZP>

[LibraryClasses.common.PEI_CORE]
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapPeiLib/AmdHeapPeiLib.inf                                                    <ALL>
  AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Pei/AmdTableSmnPeiLib.inf                                          <ALL>

[LibraryClasses]
  #
  # Agesa specific common libraries
  #
  AmdBaseLib|AgesaModulePkg/Library/AmdBaseLib/AmdBaseLib.inf                                                          <ALL>
  FabricResourceSizeForEachDieLib|AgesaModulePkg/Library/DxeFabricResourceSizeForEachDieLib/DxeFabricResourceSizeForEachDieLib.inf   <ALL>
  AmdIdsDebugPrintLib|AgesaModulePkg/Library/AmdIdsDebugPrintLib/AmdIdsDebugPrintLib.inf                               <ALL>
  AmdIdsHookLib|AgesaModulePkg/Library/AmdIdsHookLib/AmdIdsHookLib.inf                                                 <ALL>
  AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf                                    <ALL>
  IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf                                                        <ALL>
  IdsMiscLib|AgesaModulePkg/Library/IdsMiscLib/IdsMiscLib.inf                                                          <ALL>
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapLibNull/AmdHeapLibNull.inf                                                  <ALL>
  AmdMemReportInfoLib|AgesaModulePkg/Library/Mem/ReportInfoLib/AmdMemReportInfoLib.inf                                 <FAMILY15H>
  AmdSocBaseLib|AgesaModulePkg/Library/AmdSocBaseLib/AmdSocBaseLib.inf                                                 <ALL>
  AmdErrorLogLib|AgesaModulePkg/Library/AmdErrorLogLib/AmdErrorLogLib.inf                                              <ALL>
  AmdTableLib|AgesaModulePkg/Library/AmdTableLib/AmdTableLib.inf                                                       <ALL>
  SocCmnIdsHookLibPei|AgesaModulePkg/Library/SocCmnIdsHookLib/Pei/SocCmnIdsHookLibPei.inf                              <ALL>
  SocCmnIdsHookLibDxe|AgesaModulePkg/Library/SocCmnIdsHookLib/Dxe/SocCmnIdsHookLibDxe.inf                              <ALL>
  BaseCoreLogicalIdLib|AgesaModulePkg/Library/BaseCoreLogicalIdX86Lib/BaseCoreLogicalIdX86Lib.inf                      <ALL>
  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf                                         <ALL>

  ## PSP Libs
  AmdPspMboxLibV1|AgesaModulePkg/Library/AmdPspMboxLibV1/AmdPspMboxLibV1.inf                                           <ALL>
  AmdPspBaseLibV1|AgesaModulePkg/Library/AmdPspBaseLibV1/AmdPspBaseLibV1.inf                                           <ALL>
  AmdPspBaseLibV2|AgesaModulePkg/Library/AmdPspBaseLibV2/AmdPspBaseLibV2.inf                                           <ALL>
  AmdPspMboxLibV2|AgesaModulePkg/Library/AmdPspMboxLibV2/AmdPspMboxLibV2.inf                                           <ALL>
  AmdPspApobLib|AgesaModulePkg/Library/AmdPspApobLib/AmdPspApobLib.inf                                                 <ALL>
  AmdPspFtpmLib|AgesaModulePkg/Library/AmdPspFtpmLib/AmdPspFtpmLib.inf                                                 <AM4|RV|SP3r2TR>
  AmdPspFlashAccLib|AgesaModulePkg/Library/AmdPspFlashAccLibNull/AmdPspFlashAccLibNull.inf                             <ALL>

  ## CCX Lib
  CcxBaseX86Lib|AgesaModulePkg/Library/CcxBaseX86Lib/CcxBaseX86Lib.inf                                                 <ALL>
  CcxBistLib|AgesaModulePkg/Library/CcxBistLib/CcxBistLib.inf                                                          <ALL>
  CcxPspLib|AgesaModulePkg/Library/CcxPspLib/CcxPspLib.inf                                                             <ALL>
  CcxHaltLib|AgesaModulePkg/Library/CcxHaltLib/CcxHaltLib.inf                                                          <ALL>
  CcxMicrocodePatchLib|AgesaModulePkg/Library/CcxMicrocodePatchLib/CcxMicrocodePatchLib.inf                            <ALL>
  CcxRolesLib|AgesaModulePkg/Library/CcxRolesX86Lib/CcxRolesX86Lib.inf                                                 <ALL>
  CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesZpLib/CcxResetTablesZpLib.inf                                 <FAMILY18H>
  CcxSetMcaLib|AgesaModulePkg/Library/CcxSetMcaZpLib/CcxSetMcaZpLib.inf                                                <ZP>
  CcxPstatesLib|AgesaModulePkg/Library/CcxPstatesZenZpLib/CcxPstatesZenZpLib.inf                                       <FAMILY18H>
  CcxSmbiosLib|AgesaModulePkg/Library/CcxSmbiosZenZpLib/CcxSmbiosZenZpLib.inf                                          <FAMILY18H>
  CcxStallLib|AgesaModulePkg/Library/CcxStallZenLib/CcxStallZenLib.inf                                                 <FAMILY18H>
  DxeCcxBaseX86ServicesLib|AgesaModulePkg/Library/DxeCcxBaseX86ServicesLib/DxeCcxBaseX86ServicesLib.inf                <ALL>

  ## DF Lib
  FabricPstatesLib|AgesaModulePkg/Library/FabricPstatesZpLib/FabricPstatesZpLib.inf                                    <ALL>
  BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf                     <FAMILY18H>
  PeiFabricTopologyServicesLib|AgesaModulePkg/Library/PeiFabricTopologyServicesLib/PeiFabricTopologyServicesLib.inf    <ALL>
  DxeFabricTopologyServicesLib|AgesaModulePkg/Library/DxeFabricTopologyServicesLib/DxeFabricTopologyServicesLib.inf    <ALL>
  SmmFabricTopologyServicesLib|AgesaModulePkg/Library/SmmFabricTopologyServicesLib/SmmFabricTopologyServicesLib.inf    <ALL>
  FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf                  <ALL>
  FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerZenZpLib/FabricResourceManagerZenZpLib.inf         <FAMILY18H>

  ## MEM Lib
  AmdMemBaseLib|AgesaModulePkg/Library/Mem/BaseLib/AmdMemBaseLib.inf                                                   <FAMILY18H>
  AmdMemMainLib|AgesaModulePkg/Library/Mem/MainLib/AmdMemMainLib.inf                                                   <FAMILY15H>
  AmdMemNbLib|AgesaModulePkg/Library/Mem/NbLib/AmdMemNbLib.inf                                                         <FAMILY15H>
  AmdMemNbCzLib|AgesaModulePkg/Library/Mem/NbCzLib/AmdMemNbCzLib.inf                                                   <FAMILY15H>
  AmdMemFeatS3Lib|AgesaModulePkg/Library/Mem/FeatS3Lib/AmdMemFeatS3Lib.inf                                             <FAMILY15H>
  AmdMemErrorLogLib|AgesaModulePkg/Library/Mem/ErrorLogLib/AmdMemErrorLogLib.inf                                       <FAMILY15H>
  HardCodedMemCommonServiceLib|AgesaModulePkg/Library/HardCodedMemCommonServiceLib/HardCodedMemCommonServiceLib.inf    <FAMILY15H>
  OptionMemoryInstallCzLib|AgesaModulePkg/Library/OptionMemoryInstallCzLib/OptionMemoryInstallCzLib.inf                <FAMILY15H>

  ## Gnb Lib
  GnbCommonLib|AgesaModulePkg/Library/GnbCommonLib/GnbCommonLib.inf                                                    <ALL>
  GnbMemAccLib|AgesaModulePkg/Library/GnbMemAccLib/GnbMemAccLib.inf                                                    <ALL>
  GnbIoAccLib|AgesaModulePkg/Library/GnbIoAccLib/GnbIoAccLib.inf                                                       <ALL>
  GnbPciAccLib|AgesaModulePkg/Library/GnbPciAccLib/GnbPciAccLib.inf                                                    <ALL>
  GnbCpuAccLib|AgesaModulePkg/Library/GnbCpuAccLib/GnbCpuAccLib.inf                                                    <ALL>
  GnbPciLib|AgesaModulePkg/Library/GnbPciLib/GnbPciLib.inf                                                             <ALL>
  GnbLib|AgesaModulePkg/Library/GnbLib/GnbLib.inf                                                                      <ALL>
  NbioHandleLib|AgesaModulePkg/Library/NbioHandleLib/NbioHandleLib.inf                                                 <ALL>
  NbioIommuIvrsLib|AgesaModulePkg/Library/NbioIommuIvrsLib/NbioIommuIvrsLib.inf                                        <ALL>
  PcieConfigLib|AgesaModulePkg/Library/PcieConfigLib/PcieConfigLib.inf                                                 <ALL>
  GnbSSocketLib|AgesaModulePkg/Library/GnbSSocketLib/GnbSSocketLib.inf                                                 <ALL>
  GnbRegisterAccZpLib|AgesaModulePkg/Library/GnbRegisterAccZpLib/GnbRegisterAccZpLib.inf                               <ALL>
  NbioRegisterAccLib|AgesaModulePkg/Library/NbioRegisterAccLib/NbioRegisterAcc.inf                                     <ALL>
  NbioSmuV9Lib|AgesaModulePkg/Library/NbioSmuV9Lib/NbioSmuV9Lib.inf                                                    <ZP|RV|FAMILY15H>
  NbioSmuV10Lib|AgesaModulePkg/Library/NbioSmuV10Lib/NbioSmuV10Lib.inf                                                 <RV>
  NbioIdsHookCZLib|AgesaModulePkg/Library/NbioIdsHookCZLib/NbioIdsHookCZLib.inf                                        <FAMILY15H>
  NbioIdsHookZPLib|AgesaModulePkg/Library/NbioIdsHookZPLib/NbioIdsHookZPLib.inf                                        <FAMILY18H>
  NbioIdsHookRVLib|AgesaModulePkg/Library/NbioIdsHookRVLib/NbioIdsHookRVLib.inf                                        <RV>
  DxioLibV1|AgesaModulePkg/Library/DxioLibV1/DxioLibV1.inf                                                             <ALL>
  PcieMiscCommLib|AgesaModulePkg/Library/PcieMiscCommLib/PcieMiscCommLib.inf                                           <FAMILY18H>
  NbioAzaliaLib|AgesaModulePkg/Library/NbioAzaliaLib/NbioAzaliaLib.inf                                                 <FAMILY18H>
  SmnAccessLib|AgesaModulePkg/Library/SmnAccessLib/SmnAccessLib.inf                                                    <FAMILY18H>

  ## Fch Lib
  FchBaseLib|AgesaModulePkg/Library/FchBaseLib/FchBaseLib.inf                                                          <ALL>
  FchDxeLibV9|AgesaModulePkg/Library/FchDxeLib/FchDxeLib.inf                                                           <ALL>
  FchSmmLibV9|AgesaModulePkg/Library/FchSmmLib/FchSmmLib.inf                                                           <ALL>
  FchTaishanDxeLib|AgesaModulePkg/Fch/Taishan/FchTaishanCore/FchTaishanLibDxe.inf                                      <ZP>
  FchSandstoneDxeLib|AgesaModulePkg/Fch/Sandstone/FchSandstoneCore/FchSandstoneLibDxe.inf                              <RV>
  FchKernDxeLib|AgesaModulePkg/Fch/Kern/FchKernCore/FchKernLibDxe.inf                                                  <FAMILY15H>
  FchInitHookLibDxe|AgesaModulePkg/Library/FchInitHookLib/FchInitHookLibDxe.inf                                           <ALL>
  FchIdsHookLib|AgesaModulePkg/Library/FchIdsHookLib/FchIdsHookLib.inf                                                 <ALL>

  ## Ras Lib
  RasBaseLib|AgesaModulePkg/Library/RasBaseLib/RasBaseLib.inf                                                          <SP3ZP>

[Components.IA32]
  AgesaModulePkg/Debug/AmdIdsDebugPrintPei/AmdIdsDebugPrintPei.inf                                                     <ALL>

  ##PSP Drivers
  AgesaModulePkg/Psp/AmdPspPeiV1/AmdPspPeiV1.inf                                                                       <ALL>
  AgesaModulePkg/Psp/AmdPspPeiV2/AmdPspPeiV2.inf                                                                       <FAMILY18H>
  AgesaModulePkg/Psp/AmdPspFtpmPei/AmdPspFtpmPei.inf                                                                   <AM4|RV|SP3r2TR>
  AgesaModulePkg/Ccx/Zen/CcxZenZpPei/AmdCcxZenZpPei.inf {
    <LibraryClasses>
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesZpLib/CcxResetTablesZpLib.inf
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
  }                                                                                                                    <FAMILY18H>
  AgesaModulePkg/Ccx/Xv/CcxXvPei/AmdCcxXvPei.inf {
    <LibraryClasses>
    ApCommunicationLib|AgesaModulePkg/Library/ApCommunicationXvLib/ApCommunicationXvLib.inf
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    CcxDownCoreLib|AgesaModulePkg/Library/CcxDownCoreXvLib/CcxDownCoreXvLib.inf
    CcxPstatesLib|AgesaModulePkg/Library/CcxPstatesXvLib/CcxPstatesXvLib.inf
    CcxSmbiosLib|AgesaModulePkg/Library/CcxSmbiosXvLib/CcxSmbiosXvLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatBrLib/BaseSocLogicalIdXlatBrLib.inf
    AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Null/AmdTableSmnNullLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Fabric/ZP/FabricZpPei/AmdFabricZpPei.inf {
    <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  }                                                                                                                    <FAMILY18H>
  AgesaModulePkg/Fabric/XV/UnbXvPei/AmdUnbXvPei.inf {
    <LibraryClasses>
    FabricPstatesLib|AgesaModulePkg/Library/FabricPstatesUnbLib/FabricPstatesUnbLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatBrLib/BaseSocLogicalIdXlatBrLib.inf
    AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Null/AmdTableSmnNullLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Nbio/SMU/AmdNbioSmuV8Pei/AmdNbioSmuV8Pei.inf                                                          <FAMILY15H>
  AgesaModulePkg/Nbio/SMU/AmdNbioSmuV9Pei/AmdNbioSmuV9Pei.inf                                                          <ZP>
  AgesaModulePkg/Nbio/SMU/AmdNbioSmuV10Pei/AmdNbioSmuV10Pei.inf                                                        <RV>
  AgesaModulePkg/Nbio/PCIe/AmdNbioPcieZPPei/AmdNbioPcieZPPei.inf                                                       <ZP>
  AgesaModulePkg/Nbio/PCIe/AmdNbioPcieRVPei/AmdNbioPcieRVPei.inf                                                       <RV>
  AgesaModulePkg/Nbio/NbioBase/AmdNbioBaseZPPei/AmdNbioBaseZPPei.inf                                                   <ZP>
  AgesaModulePkg/Nbio/NbioBase/AmdNbioBaseCZPei/AmdNbioBaseCZPei.inf                                                   <FAMILY15H>
  AgesaModulePkg/Nbio/NbioBase/AmdNbioBaseRVPei/AmdNbioBaseRVPei.inf                                                   <RV>
  AgesaModulePkg/Nbio/GFX/AmdNbioGfxRVPei/AmdNbioGfxRVPei.inf                                                          <RV>
  AgesaModulePkg/Nbio/IOMMU/AmdNbioIOMMUZPPei/AmdNbioIOMMUZPPei.inf                                                    <FAMILY18H>
  AgesaModulePkg/Nbio/IOAPIC/AmdNbioIOAPICZPPei/AmdNbioIOAPICZPPei.inf                                                 <FAMILY18H>
  AgesaModulePkg/Fch/Taishan/FchTaishanPei/FchPei.inf                                                                  <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanSmbusPei/Smbus.inf                                                              <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanMultiFchPei/FchMultiFchPei.inf                                                  <ZP>
  AgesaModulePkg/Fch/Sandstone/FchSandstonePei/FchPei.inf                                                              <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneSmbusPei/Smbus.inf                                                          <RV>
  AgesaModulePkg/Fch/Kern/FchKernPei/FchPei.inf                                                                        <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernSmbusPei/Smbus.inf                                                                    <FAMILY15H>
  AgesaModulePkg/Mem/AmdMemAm4Pei/AmdMemAm4Pei.inf                                                                     <FAMILY18H>
  AgesaModulePkg/Soc/AmdSocAm4RvPei/AmdSocAm4RvPei.inf {
    <LibraryClasses>
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdRvDieLib/BaseSocketLogicalIdRvDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatRvDieLib/BaseSocLogicalIdXlatRvDieLib.inf
  }                                                                                                                    <AM4RV>
  AgesaModulePkg/Soc/AmdSocAm4SmPei/AmdSocAm4SmPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerZenZpLib/FabricResourceInitZenZpLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistZpLib/PeiSocBistZpLib.inf
  }                                                                                                                    <AM4SM>
  AgesaModulePkg/Soc/AmdSocAm4BrPei/AmdSocAm4BrPei.inf {
    <LibraryClasses>
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdUnbLib/BaseSocketLogicalIdUnbLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatBrLib/BaseSocLogicalIdXlatBrLib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyUnbLib/BaseFabricTopologyUnbLib.inf
  }                                                                                                                    <AM4BR>
  AgesaModulePkg/Soc/AmdSocFp5RvPei/AmdSocFp5RvPei.inf {
    <LibraryClasses>
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdRvDieLib/BaseSocketLogicalIdRvDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatRvDieLib/BaseSocLogicalIdXlatRvDieLib.inf
  }                                                                                                                    <FP5RV>
  AgesaModulePkg/Soc/AmdSocSp3ZpPei/AmdSocSp3ZpPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerZenZpLib/FabricResourceInitZenZpLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistZpLib/PeiSocBistZpLib.inf
  }                                                                                                                    <SP3ZP>
  AgesaModulePkg/Soc/AmdSocSp4ZpPei/AmdSocSp4ZpPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerZenZpLib/FabricResourceInitZenZpLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistZpLib/PeiSocBistZpLib.inf
  }                                                                                                                    <SP4ZP>
  AgesaModulePkg/Soc/AmdSocSp3r2TrPei/AmdSocSp3r2TrPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerZenZpLib/FabricResourceInitZenZpLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistZpLib/PeiSocBistZpLib.inf
  }                                                                                                                    <SP3r2TR>
  AgesaModulePkg/Mem/AmdMemFeatPei/AmdMemFeatPei.inf                                                                   <FAMILY15H>
  AgesaModulePkg/Mem/AmdMemCzPei/AmdMemCzPei.inf {
    <LibraryClasses>
    CcxPstatesLib|AgesaModulePkg/Library/CcxPstatesXvLib/CcxPstatesXvLib.inf
    FabricPstatesLib|AgesaModulePkg/Library/FabricPstatesUnbLib/FabricPstatesUnbLib.inf
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Mem/AmdMemChanXLatZpPei/MemChanXLatZpPei.inf                                                          <ZP>
  AgesaModulePkg/Mem/AmdMemSmbiosV2Pei/MemSmbiosV2Pei.inf                                                              <FAMILY18H>
  AgesaModulePkg/Mem/AmdMbistPei/AmdMbistPei.inf                                                                       <SP3ZP>
  AgesaModulePkg/ErrorLog/AmdErrorLogPei/AmdErrorLogPei.inf                                                            <ALL>
  AgesaModulePkg/Universal/AmdMemoryHobInfoPeim/AmdMemoryHobInfoPeim.inf                                               <ALL>
  AgesaModulePkg/Universal/Version/AmdVersionPei/AmdVersionPei.inf                                                     <ALL>

  ## PT Pei Drivers
  AgesaModulePkg/Fch/Promontory/PromontoryPei/FchPromontoryPei.inf                                                     <AM4BR|AM4SM|SP3r2TR>
  AgesaModulePkg/Fch/Promontory/PromontoryPei/FchPromontoryGpioPei.inf                                                 <AM4BR|AM4SM|SP3r2TR>
  AgesaModulePkg/Fch/Promontory/PromontoryPei/FchPromontoryCbsPei.inf                                                  <AM4BR|AM4SM|SP3r2TR>

[Components.X64]
  AgesaModulePkg/Debug/AmdIdsDebugPrintDxe/AmdIdsDebugPrintDxe.inf                                                     <ALL>
  AgesaModulePkg/Mem/AmdMemAm4Dxe/AmdMemAm4Dxe.inf                                                                     <FAMILY18H>
  AgesaModulePkg/Mem/AmdMemSmbiosDxe/AmdMemSmbiosDxe.inf                                                               <FAMILY15H>
  AgesaModulePkg/Mem/AmdMemSmbiosV2Dxe/AmdMemSmbiosV2Dxe.inf                                                           <FAMILY18H>
  AgesaModulePkg/Mem/AmdMemS3CzDxe/AmdMemS3CzDxe.inf{
    <LibraryClasses>
    FabricPstatesLib|AgesaModulePkg/Library/FabricPstatesUnbLib/FabricPstatesUnbLib.inf
    CcxPstatesLib|AgesaModulePkg/Library/CcxPstatesXvLib/CcxPstatesXvLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Psp/ApcbDrv/ApcbZpDxe/ApcbZpDxe.inf{
    <LibraryClasses>
    ApcbLib|AgesaModulePkg/Library/ApcbLib/ApcbLib.inf
  }                                                                                                                    <ZP>
  AgesaModulePkg/Psp/ApcbDrv/ApcbZpSmm/ApcbZpSmm.inf{
    <LibraryClasses>
    ApcbLib|AgesaModulePkg/Library/ApcbLib/ApcbLib.inf
  }                                                                                                                    <ZP>
  AgesaModulePkg/Psp/AmdPspDxeV1/AmdPspDxeV1.inf                                                                       <ALL>
  AgesaModulePkg/Psp/AmdPspSmmV1/AmdPspSmmV1.inf                                                                       <FAMILY15H>
  AgesaModulePkg/Psp/AmdPspP2CmboxV1/AmdPspP2CmboxV1.inf                                                               <FAMILY15H>
  AgesaModulePkg/Psp/AmdPspDxeV2/AmdPspDxeV2.inf                                                                       <FAMILY18H>
  AgesaModulePkg/Psp/AmdPspP2CmboxV2/AmdPspP2CmboxV2.inf                                                               <FAMILY18H>
  AgesaModulePkg/Psp/AmdPspSmmV2/AmdPspSmmV2.inf {
    <LibraryClasses>
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
  }                                                                                                                    <ZP>

  AgesaModulePkg/Psp/AmdPspFtpmDxe/AmdPspFtpmDxe.inf                                                                   <AM4|RV|SP3r2TR>
  AgesaModulePkg/Psp/AmdHstiV1/AmdHstiV1.inf                                                                           <FAMILY15H>
  AgesaModulePkg/Psp/AmdHstiV2/AmdHstiV2.inf                                                                           <FAMILY18H>
  AgesaModulePkg/Ccx/Zen/CcxZenZpDxe/AmdCcxZenZpDxe.inf {
    <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesZpLib/CcxResetTablesZpLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    FabricWdtLib|AgesaModulePkg/Library/FabricWdtZpLib/FabricWdtZpLib.inf
    CcxIdsCustomPstatesLib|AgesaModulePkg/Library/CcxIdsCustomPstateZenLib/CcxIdsCustomPstatesZenLib.inf
  }                                                                                                                    <AM4SM|SP3r2TR>
  #AgesaModulePkg/Ccx/Zen/CcxZenZpDxe/AmdCcxZenZpDxe.inf {
  #  <LibraryClasses>
  #  IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  #  CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesZpLib/CcxResetTablesZpLib.inf
  #  BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
  #  FabricWdtLib|AgesaModulePkg/Library/FabricWdtZpLib/FabricWdtZpLib.inf
  #  CcxIdsCustomPstatesLib|AgesaModulePkg/Library/CcxIdsCustomPstateNullLib/CcxIdsCustomPstatesNullLib.inf
  #}                                                                                                                    <SP3ZP|SP4ZP>
  AgesaModulePkg/Ccx/Xv/CcxXvDxe/AmdCcxXvDxe.inf {
    <LibraryClasses>
    CcxPstatesLib|AgesaModulePkg/Library/CcxPstatesXvLib/CcxPstatesXvLib.inf
    CcxSmbiosLib|AgesaModulePkg/Library/CcxSmbiosXvLib/CcxSmbiosXvLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatBrLib/BaseSocLogicalIdXlatBrLib.inf
    AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Null/AmdTableSmnNullLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Ccx/Xv/CcxXvSmm/AmdCcxXvSmm.inf  {
    <LibraryClasses>
    AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Null/AmdTableSmnNullLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Ccx/Zen/CcxZenZpSmm/AmdCcxZenZpSmm.inf {
    <LibraryClasses>
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
  }                                                                                                                    <FAMILY18H>
  AgesaModulePkg/Fabric/ZP/FabricZpDxe/AmdFabricZpDxe.inf {
    <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  }                                                                                                                    <FAMILY18H>
  AgesaModulePkg/Fabric/ZP/FabricZpSmm/AmdFabricZpSmm.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
  }                                                                                                                    <ZP>
  AgesaModulePkg/Fabric/XV/UnbXvDxe/AmdUnbXvDxe.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerUnbLib/FabricResourceManagerUnbLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatBrLib/BaseSocLogicalIdXlatBrLib.inf
    AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Null/AmdTableSmnNullLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Soc/AmdSocAm4SmDxe/AmdSocAm4SmDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
  }                                                                                                                    <AM4SM>
  AgesaModulePkg/Soc/AmdSocAm4BrDxe/AmdSocAm4BrDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdUnbLib/BaseSocketLogicalIdUnbLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatBrLib/BaseSocLogicalIdXlatBrLib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyUnbLib/BaseFabricTopologyUnbLib.inf
  }                                                                                                                    <AM4BR>
  AgesaModulePkg/Soc/AmdSocFp5RvDxe/AmdSocFp5RvDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdRvDieLib/BaseSocketLogicalIdRvDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatRvDieLib/BaseSocLogicalIdXlatRvDieLib.inf
  }                                                                                                                    <FP5RV>
  AgesaModulePkg/Soc/AmdSocSp3ZpDxe/AmdSocSp3ZpDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
  }                                                                                                                    <SP3ZP>
  AgesaModulePkg/Soc/AmdSocSp4ZpDxe/AmdSocSp4ZpDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
  }                                                                                                                    <SP4ZP>
  AgesaModulePkg/Soc/AmdSocAm4RvDxe/AmdSocAm4RvDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdRvDieLib/BaseSocketLogicalIdRvDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatRvDieLib/BaseSocLogicalIdXlatRvDieLib.inf
  }                                                                                                                    <AM4RV>
  AgesaModulePkg/Soc/AmdSocSp3r2TrDxe/AmdSocSp3r2TrDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
  }                                                                                                                    <SP3r2TR>
  AgesaModulePkg/ErrorLog/AmdErrorLogDxe/AmdErrorLogDxe.inf                                                            <ALL>
  AgesaModulePkg/Mem/AmdMemPprSmmDriver/AmdMemPprSmmDriver.inf                                                         <SP3ZP>
  ## Gnb Dxe Drivers
  AgesaModulePkg/Nbio/NbioBase/AmdNbioBaseCZDxe/AmdNbioBaseCZDxe.inf {
      <LibraryClasses>
    CcxBtcLib|AgesaModulePkg/Library/CcxBtcLib/CcxXvBtcLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Nbio/NbioBase/AmdNbioBaseRVDxe/AmdNbioBaseRVDxe.inf                                                   <RV>
  AgesaModulePkg/Nbio/NbioBase/AmdNbioBaseZPDxe/AmdNbioBaseZPDxe.inf                                                   <ZP>
  AgesaModulePkg/Nbio/SMU/AmdNbioSmuV9Dxe/AmdNbioSmuV9Dxe.inf                                                          <ZP>
  AgesaModulePkg/Nbio/SMU/AmdNbioSmuV10Dxe/AmdNbioSmuV10Dxe.inf                                                        <RV>
  AgesaModulePkg/Nbio/PCIe/AmdNbioPcieZPDxe/AmdNbioPcieZPDxe.inf                                                       <ZP>
  AgesaModulePkg/Nbio/PCIe/AmdNbioPcieRVDxe/AmdNbioPcieRVDxe.inf                                                       <RV>
  AgesaModulePkg/Nbio/IOMMU/AmdNbioIOMMUZPDxe/AmdNbioIOMMUZPDxe.inf                                                    <FAMILY18H>
  AgesaModulePkg/Nbio/GFX/AmdNbioGfxRVDxe/AmdNbioGfxRVDxe.inf                                                          <RV>
  AgesaModulePkg/Nbio/NbioALib/AmdNbioAlibZPDxe/AmdNbioAlibZPDxe.inf                                                   <FAMILY18H>
  AgesaModulePkg/Fch/Taishan/FchTaishanDxe/FchDxe.inf                                                                  <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanSmm/FchSmm.inf                                                                  <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanSmbusDxe/SmbusLight.inf                                                         <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanCf9ResetDxe/Cf9Reset.inf                                                        <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanLegacyInterruptDxe/LegacyInterrupt.inf                                          <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanSmmControlDxe/SmmControl.inf                                                    <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanSmmDispatcher/FchSmmDispatcher.inf                                              <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanMultiFchDxe/FchMultiFchDxe.inf                                                  <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanMultiFchSmm/FchMultiFchSmm.inf                                                  <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanDxe/FchTaishanSsdt.inf                                                          <AM4SM>
  AgesaModulePkg/Fch/Taishan/FchTaishanDxe/FchTaishanAml.inf                                                           <AM4SM>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneDxe/FchDxe.inf                                                              <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneSmm/FchSmm.inf                                                              <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneSmbusDxe/SmbusLight.inf                                                     <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneCf9ResetDxe/Cf9Reset.inf                                                    <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneLegacyInterruptDxe/LegacyInterrupt.inf                                      <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneSmmControlDxe/SmmControl.inf                                                <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneSmmDispatcher/FchSmmDispatcher.inf                                          <RV>
  AgesaModulePkg/Fch/Kern/FchKernDxe/FchDxe.inf                                                                        <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernSmm/FchSmm.inf                                                                        <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernSmbusDxe/SmbusLight.inf                                                               <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernCf9ResetDxe/Cf9Reset.inf                                                              <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernLegacyInterruptDxe/LegacyInterrupt.inf                                                <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernSmmControlDxe/SmmControl.inf                                                          <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernSmmDispatcher/FchSmmDispatcher.inf                                                    <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernComplement/ImcControl/ImcControl.inf                                                  <FAMILY15H>

  # Universal
  AgesaModulePkg/Universal/Smbios/AmdSmbiosDxe.inf                                                                     <ALL>
  AgesaModulePkg/Universal/Acpi/AmdAcpiDxe.inf                                                                         <ALL>
  AgesaModulePkg/Universal/AmdSmmCommunication/AmdSmmCommunication.inf                                                 <ALL>
  AgesaModulePkg/Universal/Version/AmdVersionDxe/AmdVersionDxe.inf                                                     <ALL>

  ## PT Dxe Drivers
  AgesaModulePkg/Fch/Promontory/PromontoryDxe/FchPromontoryGpioDxe.inf                                                 <AM4BR|AM4SM|SP3r2TR>
  AgesaModulePkg/Fch/Promontory/PromontoryDxe/FchPromontorySsdt.inf                                                    <AM4BR|AM4SM|SP3r2TR>
  AgesaModulePkg/Fch/Promontory/PromontorySmm/FchPromontorySmm.inf                                                     <AM4BR|AM4SM|SP3r2TR>
  AgesaModulePkg/Fch/Promontory/PromontorySmm/FchPromontoryGpioSmm.inf                                                 <AM4BR|AM4SM|SP3r2TR>
  AgesaModulePkg/Fch/Promontory/PromontoryDxe/FchPromontoryAml.inf                                                     <AM4BR|AM4SM|SP3r2TR>

  ## Ras Dxe Drivers
  AgesaModulePkg/Ras/AmdRasApeiDxe/AmdRasApeiDxe.inf                                                                   <SP3ZP>
  AgesaModulePkg/Ras/AmdRasDxe/AmdRasDxe.inf                                                                           <SP3ZP>
  AgesaModulePkg/Ras/AmdRasSmm/AmdRasSmm.inf                                                                           <SP3ZP>

  ## Snp Dxe Drivers (Uncomment SnpDxePort0.inf for xGBE support)
  # AgesaModulePkg/SnpDxe/SnpDxePort0.inf                                                                              <SP3ZP>

  ## NVDIMM Dxe Driver
  AgesaModulePkg/JedecNvdimm/JedecNvdimm.inf                                                                           <SP3ZP>

[PcdsFixedAtBuild]
  # AMD AGESA PI Name & Version string
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0BristolPI-AM4 1.0.0.5"                            <*Am4Br>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0RavenPI-FP5-AM4 0.0.6.0"                          <*Am4Rv>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0SummitPI-AM4 1.0.0.5"                             <*Am4SmBr>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0SummitPI-AM4 0.0.6.0"                             <*Am4RvSmBr>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0SummitPI-AM4 0.0.6.0"                             <*Am4RvSm>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0SummitPI-AM4 1.0.0.5"                             <*Am4Sm>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0RavenPI-FP5-AM4 0.0.6.0"                          <*Fp5Rv>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0HygonPI 1.0.1.0"                                 <*Sp3Zp>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0SnowOwlPI-SP4 0.0.6.0"                            <*Sp4Zp>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0SummitPI-SP3r2-1.0.0.0"                           <*Sp3r2Tr>
