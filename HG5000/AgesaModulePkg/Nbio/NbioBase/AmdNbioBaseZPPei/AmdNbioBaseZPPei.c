/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioBaseZPPei Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioBaseZPPei
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */
#include <AMD.h>
#include <PiPei.h>
#include <CcxRegistersZp.h>
#include <Library/PeiServicesLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Filecode.h>
#include <Ppi/NbioBaseServicesZPPpi.h>
#include <GnbDxio.h>
#include <PcieComplexDataZP.h>
#include <Guid/GnbNbioBaseZPInfoHob.h>
#include <Library/PcdLib.h>
#include <Library/AmdPspBaseLibV1.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/PcieConfigLib.h>
#include <Ppi/AmdMemoryInfoHobPpi.h>
#include <Guid/AmdMemoryInfoHob.h>
#include <Library/GnbRegisterAccZpLib.h>
#include <Library/NbioTable.h>
//#include <Library/GnbHeapLib.h>
#include <Library/NbioHandleLib.h>
#include <IdsHookId.h>
#include <Library/AmdIdsHookLib.h>
#include <NbioTablesZP.h>
#include <GnbRegistersZP.h>
#include <Ppi/SocLogicalIdPpi.h>


#define FILECODE NBIO_NBIOBASE_AMDNBIOBASEZPPEI_AMDNBIOBASEZPPEI_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

typedef UINT8 GNB_TABLE;

extern ZP_COMPLEX_CONFIG      ComplexDataZP;
extern GNB_TABLE ROMDATA      GnbIommuEnvInitTableZP [];

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
DebugGetConfig (
  IN  PEI_AMD_NBIO_BASE_SERVICES_PPI  *This,
  OUT GNB_BUILD_OPTIONS_ZP_DATA_HOB   **DebugOptions
  );

EFI_STATUS
GnbRegisterRead (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       UINT8                      RegisterSpaceType,
  IN       UINT32                     Address,
     OUT   VOID                       *Value,
  IN       UINT32                     Flags,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

EFI_STATUS
GnbRegisterWrite (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       UINT8                      RegisterSpaceType,
  IN       UINT32                     Address,
  IN       VOID                       *Value,
  IN       UINT32                     Flags,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

EFI_STATUS
EFIAPI
MultiDieConfigureCallbackPpi (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR        *NotifyDescriptor,
  IN VOID                             *Ppi
  );

EFI_STATUS
EFIAPI
MemoryConfigDoneCallbackPpi (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR        *NotifyDescriptor,
  IN VOID                             *Ppi
  );

EFI_STATUS
EFIAPI
NbioTopologyConfigureCallbackPpi (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR        *NotifyDescriptor,
  IN VOID                             *Ppi
  );

VOID
NbioNBIFPoisonDataWorkaroundZP (
  IN GNB_HANDLE                 *GnbHandle
  );

VOID
NbioNBIFParityErrorsWorkaroundZP (
  IN GNB_HANDLE                 *GnbHandle
  );

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */

STATIC PEI_AMD_NBIO_BASE_SERVICES_PPI mNbioBaseServicePpi = {
  AMD_NBIO_BASE_SERVICES_REVISION,  ///< revision
  (VOID *) &ComplexDataZP,
  sizeof (ZP_COMPLEX_CONFIG),
  DebugGetConfig,
  GnbRegisterRead,
  GnbRegisterWrite
};

STATIC EFI_PEI_PPI_DESCRIPTOR mNbioBaseServicesPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdNbioBaseServicesZpPpiGuid,
  &mNbioBaseServicePpi
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR   mNotifyMemDonePpi = {
  EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gAmdMemoryInfoHobPpiGuid,
  MemoryConfigDoneCallbackPpi
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR   mNotifyNbioTopologyPpi = {
  EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gAmdNbioPcieServicesPpiGuid,
  NbioTopologyConfigureCallbackPpi
};
/*----------------------------------------------------------------------------------------*/
/**
 * DebugGetConfig PPI function
 *
 *
 *
 * @param[in]  This          PEI_AMD_NBIO_BASE_SERVICES_PPI pointer
 * @param[out] DebugOptions  Pointer to GNB_BUILD_OPTIONS_ZP_DATA_HOB pointer
 * @retval     EFI_STATUS
 */

EFI_STATUS
DebugGetConfig (
  IN       PEI_AMD_NBIO_BASE_SERVICES_PPI      *This,
     OUT   GNB_BUILD_OPTIONS_ZP_DATA_HOB       **DebugOptions
  )
{
  EFI_STATUS                     Status;
  EFI_PEI_HOB_POINTERS           Hob;
  GNB_BUILD_OPTIONS_ZP_DATA_HOB  *GnbBuildOptionsZPDataHob;

  GnbBuildOptionsZPDataHob = NULL;

  Status = PeiServicesGetHobList (&Hob.Raw);

  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION &&
        CompareGuid ( &Hob.Guid->Name, &gGnbNbioBaseZPHobInfoGuid)) {
      GnbBuildOptionsZPDataHob = (GNB_BUILD_OPTIONS_ZP_DATA_HOB *) (Hob.Raw);
      Status = EFI_SUCCESS;
      break;
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }
  *DebugOptions = GnbBuildOptionsZPDataHob;
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * GnbRegisterRead PPI function
 *
 *
 *
 * @param[in] GnbHandle          GnbHandle
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset, but PortDevice
 * @param[out] Value             Return value
 * @param[in] Flags              Flags - BIT0 indicates S3 save/restore
 * @param[in] StdHeader          Standard configuration header
 * @retval    AGESA_STATUS
 */

EFI_STATUS
GnbRegisterRead (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
     OUT   VOID                *Value,
  IN       UINT32              Flags,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  EFI_STATUS status;
  status = GnbRegisterReadZP (GnbHandle, RegisterSpaceType, Address, Value, Flags, StdHeader);
  return status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * GnbRegisterWrite PPI function
 *
 *
 *
 * @param[in] GnbHandle          GnbHandle
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset, but PortDevice
 * @param[out] Value             Return value
 * @param[in] Flags              Flags - BIT0 indicates S3 save/restore
 * @param[in] StdHeader          Standard configuration header
 * @retval    AGESA_STATUS
 */

EFI_STATUS
GnbRegisterWrite (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  EFI_STATUS status;
  status = GnbRegisterWriteZP (GnbHandle, RegisterSpaceType, Address, Value, Flags, StdHeader);
  return status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * GNB load build options data
 *
 *
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 * @retval     EFI_STATUS
 */

EFI_STATUS
STATIC
GnbLoadBuildOptionDataZP (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                    Status;
  GNB_BUILD_OPTIONS_ZP_DATA_HOB *GnbBuildOptionsZPDataHob;
// 1. Create Build Options Structure (in HOB)
  Status = PeiServicesCreateHob (
        EFI_HOB_TYPE_GUID_EXTENSION,
        sizeof (GNB_BUILD_OPTIONS_ZP_DATA_HOB),
        &GnbBuildOptionsZPDataHob
        );
  IDS_HOOK (IDS_HOOK_NBIO_LOAD_BUILD_OPTION, NULL, NULL);
  GnbBuildOptionsZPDataHob->EfiHobGuidType.Name = gGnbNbioBaseZPHobInfoGuid;
  GnbBuildOptionsZPDataHob->CfgNbioRASEnable = PcdGetBool (PcdAmdNbioRASControl);
  GnbBuildOptionsZPDataHob->CfgNbioPoisonConsumption = PcdGetBool (PcdAmdNbioPoisonConsumption);
  GnbBuildOptionsZPDataHob->CfgIOHCClkGatinSgupport = PcdGetBool (PcdIOHCClkGatingSupport);
  GnbBuildOptionsZPDataHob->CfgIommuSupport = PcdGetBool (PcdCfgIommuSupport);

  GnbBuildOptionsZPDataHob->GnbCommonOptions.CfgIommuL2ClockGatingEnable = PcdGetBool (PcdIommuL2ClockGatingEnable);
  GnbBuildOptionsZPDataHob->GnbCommonOptions.CfgIommuL1ClockGatingEnable = PcdGetBool (PcdIommuL1ClockGatingEnable);

  GnbBuildOptionsZPDataHob->CfgChubClockGating = PcdGetBool (PcdCfgChubClockGating);
  GnbBuildOptionsZPDataHob->CfgAcpClockGating = PcdGetBool (PcdCfgAcpClockGating);
  GnbBuildOptionsZPDataHob->CfgAxgDisable = PcdGetBool (PcdCfgAxgDisable);
  GnbBuildOptionsZPDataHob->CfgPcieHwInitPwerGating = PcdGet8 (PcdCfgPcieHwInitPwerGating);
  GnbBuildOptionsZPDataHob->CfgAriSupport = PcdGetBool (PcdCfgPcieAriSupport);
  GnbBuildOptionsZPDataHob->CfgSpgClockGatingEnable = PcdGetBool (PcdCfgSpgClockGatingEnable);
  GnbBuildOptionsZPDataHob->CfgAcgAzClockGatingEnable = PcdGetBool (PcdCfgAcgAzClockGatingEnable);
  GnbBuildOptionsZPDataHob->CfgIommuL1MemPowerGating = PcdGetBool (PcdCfgIommuL1MemPowerGating);
  GnbBuildOptionsZPDataHob->CfgIommuL2MemPowerGating = PcdGetBool (PcdCfgIommuL2MemPowerGating);
  GnbBuildOptionsZPDataHob->CfgOrbTxMemPowerGating = PcdGet8 (PcdCfgOrbTxMemPowerGating);
  GnbBuildOptionsZPDataHob->CfgOrbRxMemPowerGating = PcdGet8 (PcdCfgOrbRxMemPowerGating);
  GnbBuildOptionsZPDataHob->CfgSstunlClkGating = PcdGetBool (PcdCfgSstunlClkGating);
  GnbBuildOptionsZPDataHob->CfgParityErrorConfiguration = PcdGetBool (PcdCfgParityErrorConfiguration);
  GnbBuildOptionsZPDataHob->CfgSpgMemPowerGatingEnable = PcdGet8 (PcdCfgSpgMemPowerGatingEnable);
  GnbBuildOptionsZPDataHob->CfgAcgAzMemPowerGatingEnable = PcdGet8 (PcdCfgAcgAzMemPowerGatingEnable);
  GnbBuildOptionsZPDataHob->CfgBifMemPowerGatingEnable = PcdGet8 (PcdCfgBifMemPowerGatingEnable);
  GnbBuildOptionsZPDataHob->CfgSDMAMemPowerGatingEnable = PcdGetBool (PcdCfgSDMAMemPowerGatingEnable);
  GnbBuildOptionsZPDataHob->CfgPcieTxpwrInOff = PcdGet8 (PcdCfgPcieTxpwrInOff);
  GnbBuildOptionsZPDataHob->CfgPcieRxpwrInOff = PcdGet8 (PcdCfgPcieRxpwrInOff);
  GnbBuildOptionsZPDataHob->CfgSmuDeterminismAmbient = PcdGet32 (PcdCfgSmuDeterminismAmbient);
  GnbBuildOptionsZPDataHob->CfgBTCEnable = PcdGetBool (PcdCfgBTCEnable);
  GnbBuildOptionsZPDataHob->CfgPSIEnable = PcdGetBool (PcdCfgPSIEnable);
  GnbBuildOptionsZPDataHob->CfgACSEnable = PcdGetBool (PcdCfgACSEnable);
  GnbBuildOptionsZPDataHob->CfgNTBEnable = PcdGetBool (PcdCfgNTBEnable);
  GnbBuildOptionsZPDataHob->CfgNTBLocation = PcdGet8 (PcdCfgNTBLocation);
  GnbBuildOptionsZPDataHob->CfgNTBPcieCoreSel = PcdGet8 (PcdCfgNTBPcieCoreSel);
  GnbBuildOptionsZPDataHob->CfgNTBMode = PcdGet8 (PcdCfgNTBMode);
  GnbBuildOptionsZPDataHob->CfgNTBLinkSpeed = PcdGet8 (PcdCfgNTBLinkSpeed);
  GnbBuildOptionsZPDataHob->CfgNTBBAR1Size = PcdGet8 (PcdCfgNTBBAR1Size);
  GnbBuildOptionsZPDataHob->CfgNTBBAR23Size = PcdGet8 (PcdCfgNTBBAR23Size);
  GnbBuildOptionsZPDataHob->CfgNTBBAR45Size = PcdGet8 (PcdCfgNTBBAR45Size);

  GnbBuildOptionsZPDataHob->GnbCommonOptions.CfgScsSupport = PcdGetBool (PcdCfgScsSupport);
  GnbBuildOptionsZPDataHob->GnbCommonOptions.CfgUmaSteering = PcdGet8 (PcdCfgUmaSteering);
  GnbBuildOptionsZPDataHob->GnbCommonOptions.GmcPowerGating = PcdGet8 (PcdGmcPowerGating);
  GnbBuildOptionsZPDataHob->GnbCommonOptions.CfgGmcClockGating = PcdGetBool (PcdCfgGmcClockGating);
  GnbBuildOptionsZPDataHob->GnbCommonOptions.CfgOrbDynWakeEnable = PcdGetBool (PcdCfgOrbDynWakeEnable);
  GnbBuildOptionsZPDataHob->GnbCommonOptions.CfgOrbClockGatingEnable = PcdGetBool (PcdCfgOrbClockGatingEnable);
  GnbBuildOptionsZPDataHob->GnbCommonOptions.CfgIocLclkClockGatingEnable = PcdGetBool (PcdCfgIocLclkClockGatingEnable);
  GnbBuildOptionsZPDataHob->GnbCommonOptions.CfgBapmSupport = PcdGetBool (PcdCfgBapmSupport);
  GnbBuildOptionsZPDataHob->GnbCommonOptions.CfgDcTdpEnable = PcdGetBool (PcdCfgDcTdpEnable);
  GnbBuildOptionsZPDataHob->GnbCommonOptions.CfgGnbNumDisplayStreamPipes = PcdGet8 (PcdCfgGnbNumDisplayStreamPipes);

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to Enable IOAPIC on GNB
 *
 *
 *
 * @param[in] GnbHandle          GnbHandle
 * @retval    AGESA_STATUS
 */
AGESA_STATUS
STATIC
IoapicEnableCallback (
  IN       GNB_HANDLE          *GnbHandle
  )
{
  UINT32              Value;
  UINT64              GnbNbIoapicAddress;

  if (GnbHandle->InstanceId == 0) {
    GnbNbIoapicAddress = PcdGet64 (PcdCfgGnbIoapicAddress);
    if (GnbNbIoapicAddress != 0x0ull) {
      Value = (UINT32) RShiftU64 (GnbNbIoapicAddress, 32);
      GnbRegisterWrite (GnbHandle, TYPE_SMN, 0x13b102f4, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);
      Value = GnbNbIoapicAddress & 0xFFFFFF00;
      GnbRegisterWrite (GnbHandle, TYPE_SMN, 0x13b102f0, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);
      Value |= 1;
      GnbRegisterWrite (GnbHandle, TYPE_SMN, 0x13b102f0, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);
    }
    GnbRegisterRead (GnbHandle, TYPE_SMN, 0x14300000, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);
    Value |= 1 << 2;    //Ioapic_id_ext_en
    Value |= 1 << 4;    //Ioapic_sb_feature_en
    GnbRegisterWrite (GnbHandle, TYPE_SMN, 0x14300000, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);
  } else {
    GnbRegisterRead (GnbHandle, TYPE_SMN, 0x14300000, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);
    Value |= 1 << 2;    //Ioapic_id_ext_en
    Value |= 1 << 4;    //Ioapic_sb_feature_en
    Value |= 1 << 5;    //Ioapic_secondary_en
    GnbRegisterWrite (GnbHandle, TYPE_SMN, 0x14300000, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);
  }
  Value = GnbHandle->Address.Address.Bus;
  Value |= 1 << 8;
  GnbRegisterWrite (GnbHandle, TYPE_SMN, 0x13B10044, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to initialize NBIO Base function
 *
 *
 *
 * @param[in] GnbHandle          GnbHandle
 * @retval    AGESA_STATUS
 */
AGESA_STATUS
STATIC
NbioBaseInitCallback (
  IN       GNB_HANDLE          *GnbHandle
  )
{
  UINT32                Value;
  NBRASCFG_0400_STRUCT  NBRASCFG_0400;

  NBRASCFG_0400.Value = 0;
  // initialize IOMMU
  GnbRegisterRead (GnbHandle, TYPE_SMN, SMN_0x13f00080, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "BXXD00F2x080 = 0x%x\n", Value);
  GnbRegisterWrite (GnbHandle, TYPE_SMN, SMN_0x15704330, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);

  GnbRegisterRead (GnbHandle, TYPE_SMN, SMN_0x0240001C, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "0x0240001C = 0x%x\n", Value);
  GnbRegisterWrite (GnbHandle, TYPE_SMN, SMN_0x1570401C, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);

  if (PcdGetBool (PcdCfgNTBClockGatingEnable)) {
    GnbRegisterRead (GnbHandle, NTB_CTRL_TYPE, NTB_CTRL_ADDRESS, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);
    Value |= (1 << NTB_CTRL_CLK_GATE_EN_OFFSET);
    GnbRegisterWrite (GnbHandle, NTB_CTRL_TYPE, NTB_CTRL_ADDRESS, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);
  }

  // Enable/disable NBIO Poison Consumption actions
  GnbRegisterRead (GnbHandle, POISON_ACTION_CONTROL_TYPE, POISON_ACTION_CONTROL_ADDRESS, &NBRASCFG_0400.Value, 0, (AMD_CONFIG_PARAMS *) NULL);
  if (!PcdGetBool (PcdAmdNbioPoisonConsumption)) {
    NBRASCFG_0400.Field.IntPoisonAPMLErrEn = 0x1;
    //BYO Modify for RAS >>
    NBRASCFG_0400.Field.IntPoisonIntrGenSel = 0x3;
    //BYO Modify for RAS <<
    NBRASCFG_0400.Field.IntPoisonLinkDisEn = 0x1;
    NBRASCFG_0400.Field.IntPoisonSyncFloodEn = 0x1;
    NBRASCFG_0400.Field.EgressPoisonLSAPMLErrEn = 0x0;
    NBRASCFG_0400.Field.EgressPoisonLSLinkDisEn =  0x0;
    NBRASCFG_0400.Field.EgressPoisonLSSyncFloodEn =  0x0;
    NBRASCFG_0400.Field.EgressPoisonHSAPMLErrEn = 0x1;
    NBRASCFG_0400.Field.EgressPoisonHSLinkDisEn =  0x1;
    NBRASCFG_0400.Field.EgressPoisonHSSyncFloodEn =  0x1;
    GnbRegisterWrite (GnbHandle, POISON_ACTION_CONTROL_TYPE, POISON_ACTION_CONTROL_ADDRESS, &NBRASCFG_0400.Value, 0, (AMD_CONFIG_PARAMS *) NULL);
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "NBRASCFG_0400 = 0x%x\n", NBRASCFG_0400.Value);

  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * This is the main function for early NBIO initialization.
 *
 *
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 * @retval     EFI_STATUS
 */

EFI_STATUS
AmdNbioBaseInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  PEI_AMD_NBIO_PCIE_SERVICES_PPI  *PcieServicesPpi;
  PCIe_PLATFORM_CONFIG            *Pcie;
  UINT32                          Property;
  EFI_STATUS                      Status;
  GNB_HANDLE                      *GnbHandle;
  AMD_CONFIG_PARAMS               *StdHeader;
  GNB_PCIE_INFORMATION_DATA_HOB   *PciePlatformConfigHob;
  GNB_BUILD_OPTIONS_ZP_DATA_HOB   *GnbBuildOptionData;
  PEI_AMD_NBIO_BASE_SERVICES_PPI  *NbioBaseServices;
  UINT32                          PackageType;
  AMD_PEI_SOC_LOGICAL_ID_PPI      *SocLogicalIdPpi;
  SOC_LOGICAL_ID                  LogicalId;
  UINT32                          Value;
  RCC_BIF_STRAP1_STRUCT           BifStrap1;

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioBaseInit Entry\n");
  StdHeader = NULL;
  GnbHandle = NULL;
  LogicalId.Revision = AMD_REVISION_UNKNOWN;
  LogicalId.Family = AMD_FAMILY_UNKNOWN;

  // Need topology structure
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gAmdNbioPcieServicesPpiGuid,
                             0,
                             NULL,
                             (VOID **)&PcieServicesPpi
                             );
  PcieServicesPpi->PcieGetTopology (PcieServicesPpi, &PciePlatformConfigHob);
  Pcie = &(PciePlatformConfigHob->PciePlatformConfigHob);

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gAmdNbioBaseServicesZpPpiGuid,
                             0,
                             NULL,
                             (VOID **)&NbioBaseServices
                             );

  NbioBaseServices->DebugOptions (NbioBaseServices, &GnbBuildOptionData);

  // Need debug options
  Property = TABLE_PROPERTY_DEFAULT;

  Property |= GnbBuildOptionData->CfgIOHCClkGatinSgupport ? TABLE_PROPERTY_IOHC_Clock_GATING : 0;
  Property |= GnbBuildOptionData->CfgIommuSupport ? 0 : TABLE_PROPERTY_IOMMU_DISABLED;
  Property |= GnbBuildOptionData->GnbCommonOptions.CfgIommuL2ClockGatingEnable ? TABLE_PROPERTY_IOMMU_L2_CLOCK_GATING : 0;
  Property |= GnbBuildOptionData->GnbCommonOptions.CfgIommuL1ClockGatingEnable ? TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING : 0;
  Property |= GnbBuildOptionData->CfgNbioPoisonConsumption ? TABLE_PROPERTY_POISON_ACTION_CONTROL : 0;
  Property |= GnbBuildOptionData->CfgSstunlClkGating ? 0 : TABLE_PROPERTY_SST_CLOCK_GATING_DISABLED;

  PackageType = LibAmdGetPackageType ((AMD_CONFIG_PARAMS *) NULL);
  if ((UINT32) (1 << ZP_SOCKET_SP3) == PackageType || (UINT32) (1 << ZP_SOCKET_DM1) == PackageType || (UINT32) (1 << ZP_SOCKET_SP3r2) == PackageType) {
    Property |= TABLE_PROPERTY_PACKAGE_SP3;
  }
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gAmdSocLogicalIdPpiGuid,
                             0,
                             NULL,
                             &SocLogicalIdPpi
                             );
  if (!EFI_ERROR (Status)) {
    Status = SocLogicalIdPpi->GetLogicalIdOnCurrentCore (&LogicalId);
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Adjust Property based on Debug Info and PCD's
  // From Earlier
  GnbHandle = NbioGetHandle (Pcie);
  IDS_HDT_CONSOLE (MAIN_FLOW, "GnbHandle = 0x%x\n", GnbHandle);
  while (GnbHandle != NULL) {
    GnbRegisterRead (GnbHandle, RCC_BIF_STRAP1_TYPE, RCC_NBIF0_STRAP1_ADDRESS, &BifStrap1.Value, 0, (AMD_CONFIG_PARAMS *) NULL);
    BifStrap1.Field.STRAP_RP_BUSNUM = GnbHandle->Address.Address.Bus;
    GnbRegisterWrite (GnbHandle, RCC_BIF_STRAP1_TYPE, RCC_NBIF0_STRAP1_ADDRESS, &BifStrap1.Value, 0, (AMD_CONFIG_PARAMS *) NULL);
    GnbRegisterRead (GnbHandle, RCC_BIF_STRAP1_TYPE, RCC_NBIF1_STRAP1_ADDRESS, &BifStrap1.Value, 0, (AMD_CONFIG_PARAMS *) NULL);
    BifStrap1.Field.STRAP_RP_BUSNUM = GnbHandle->Address.Address.Bus;
    GnbRegisterWrite (GnbHandle, RCC_BIF_STRAP1_TYPE, RCC_NBIF1_STRAP1_ADDRESS, &BifStrap1.Value, 0, (AMD_CONFIG_PARAMS *) NULL);
    GnbRegisterRead (GnbHandle, RCC_BIF_STRAP1_TYPE, RCC_NBIF2_STRAP1_ADDRESS, &BifStrap1.Value, 0, (AMD_CONFIG_PARAMS *) NULL);
    BifStrap1.Field.STRAP_RP_BUSNUM = GnbHandle->Address.Address.Bus;
    GnbRegisterWrite (GnbHandle, RCC_BIF_STRAP1_TYPE, RCC_NBIF2_STRAP1_ADDRESS, &BifStrap1.Value, 0, (AMD_CONFIG_PARAMS *) NULL);

    Status = GnbProcessTable (
               GnbHandle,
               GnbEarlyInitTableZP,
               Property,
               0,
               StdHeader
               );
    // From Early (these can all be combined??)
    Status = GnbProcessTable (
               GnbHandle,
               GnbEnvInitTableZP,
               Property,
               0,
               StdHeader
               );

    Status = GnbProcessTable (
               GnbHandle,
               GnbIommuEnvInitTableZP,
               Property,
               0,
               StdHeader
               );

    // A0/B0-1 IOMMU MSI-X Workaround
    //For Dhyana family&model&stepping checking
    if (((LogicalId.Revision & (AMD_REV_F17_ZP_Ax | AMD_REV_F17_ZP_B0 | AMD_REV_F17_ZP_B1)) != 0) && (LogicalId.Family == AMD_FAMILY_17_ZP)) {
      GnbRegisterRead (GnbHandle, RCC_DEV0_EPF3_STRAP3_TYPE, RCC_DEV0_EPF3_STRAP3_ADDRESS, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);
      Value &= ~(RCC_DEV0_EPF3_STRAP3_STRAP_MSIX_EN_DEV0_F3_MASK);
      if (PcdGetBool (PcdUSBMsiXCapEnable)) {
        Value |= RCC_DEV0_EPF3_STRAP3_STRAP_MSIX_EN_DEV0_F3_MASK;
      }
      GnbRegisterWrite (GnbHandle, RCC_DEV0_EPF3_STRAP3_TYPE, RCC_DEV0_EPF3_STRAP3_ADDRESS, &Value, 0, (AMD_CONFIG_PARAMS *) NULL);
    }

    IDS_HOOK (IDS_HOOK_NBIO_BASE_INIT, NULL, (void *)GnbHandle);
    IoapicEnableCallback (GnbHandle);
    if (PcdGetBool (PcdAmdNbioRASControl)) {
      NbioNBIFParityErrorsWorkaroundZP (GnbHandle);
    }
    NbioNBIFPoisonDataWorkaroundZP (GnbHandle);
    NbioBaseInitCallback (GnbHandle);

    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioBaseInit Exit\n");
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback for NbioPcieServicesPpi installation when NbioTopology is completed
 *
 *
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  NotifyDescriptor  NotifyDescriptor pointer
 * @param[in]  Ppi               Ppi pointer
 * @retval     EFI_STATUS
 */

EFI_STATUS
EFIAPI
NbioTopologyConfigureCallbackPpi (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{

  AGESA_TESTPOINT (TpNbioTopologyConfigureCallbackEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioTopologyConfigureCallbackPpi Entry\n");
  // At this point we know the NBIO topology, so we can initialize all NBIO base registers
  AmdNbioBaseInit (PeiServices);
  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioTopologyConfigureCallbackPpi Exit\n");
  AGESA_TESTPOINT (TpNbioTopologyConfigureCallbackExit, NULL);
  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * AmdNbiosBase driver entry point for ZP
 *
 *
 *
 * @param[in]  FileHandle  Standard configuration header
 * @param[in]  PeiServices Pointer to EFI_PEI_SERVICES pointer
 * @retval     EFI_STATUS
 */

EFI_STATUS
EFIAPI
AmdNbioBaseZPPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS    Status;
  UINT32        PackageType;

  AGESA_TESTPOINT (TpNbioBasePeiEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioBaseZPPeiEntry Entry\n");

  PackageType = LibAmdGetPackageType ((AMD_CONFIG_PARAMS *) NULL);
  switch (PackageType) {
  case (UINT32) (1 << ZP_SOCKET_AM4):
    PcdSetBool (PcdAmdNbioPoisonConsumption, FALSE);
    PcdSetBool (PcdAmdSataEnable, TRUE);
    break;
  case (UINT32) (1 << ZP_SOCKET_SP3r2):
    PcdSetBool (PcdAmdNbioPoisonConsumption, TRUE);
    PcdSetBool (PcdAmdSataEnable, TRUE);
    break;
  case (UINT32)(1 << ZP_SOCKET_SP3):
//    PcdSetBool (PcdAmdNbioPoisonConsumption, TRUE);
//    PcdSetBool (PcdAmdSataEnable, FALSE);
//    PcdSetBool (PcdAmdNbioRASControl, TRUE);
    break;
  default:
    PcdSetBool (PcdAmdNbioPoisonConsumption, TRUE);
    PcdSetBool (PcdAmdSataEnable, FALSE);
    break;
  }

  // 1. Set conditionals based on debug configuration and PCDs
  GnbLoadBuildOptionDataZP (PeiServices);

  // 2. PCIe topology is dependent on NbioBaseServicesPpi, so Pcie driver will not publish PPI until after
  // this code is completed. We can simply request notification when gAmdNbioPcieServicesPpiGuid is published
  Status = (**PeiServices).NotifyPpi (PeiServices, &mNotifyNbioTopologyPpi);

  // 3. Publish callback for memory config done so that we can set top of memory
  Status = (**PeiServices).NotifyPpi (PeiServices, &mNotifyMemDonePpi);

  /// 4. @todo - do we need a callback for late PEI?  TBD

  // 5. Install PPI for NbioBaseServices
  Status = PeiServicesInstallPpi (&mNbioBaseServicesPpiList); //This driver is done.
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioBaseZPPeiEntry Exit\n");
  AGESA_TESTPOINT (TpNbioBasePeiExit, NULL);
  return EFI_SUCCESS;
}
