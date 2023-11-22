/* $NoKeywords:$ */
/**
 * @file
 *
 * HygonNbioBasePei Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioBasePei
 *
 */
/*****************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 *
 ******************************************************************************
 */
#include <HYGON.h>
#include <PiPei.h>
#include <CcxRegistersDm.h>
#include <Library/PeiServicesLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Filecode.h>
#include <Ppi/NbioBaseServicesSTPpi.h>
#include <GnbHsio.h>
#include <Guid/GnbNbioBaseSTInfoHob.h>
#include <Library/PcdLib.h>
#include <Library/HygonPspBaseLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/PcieConfigLib.h>
#include <Ppi/HygonMemoryInfoHobPpi.h>
#include <Guid/HygonMemoryInfoHob.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/NbioTable.h>
#include <Library/NbioHandleLib.h>
#include <IdsHookId.h>
#include <Library/HygonIdsHookLib.h>
#include <NbioTables.h>
#include <GnbRegisters.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Library/GnbPciLib.h>
#include <Library/NbioAzaliaLib.h>
#include <Library/HygonSmnAddressLib.h>
#include <Library/HygonSocBaseLib.h>

#define FILECODE  NBIO_NBIOBASE_HYGX_HYGONNBIOBASEPEI_HYGONNBIOBASEPEI_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

typedef UINT8 GNB_TABLE;

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
  IN  PEI_HYGON_NBIO_BASE_SERVICES_PPI  *This,
  OUT GNB_BUILD_OPTIONS_ST_DATA_HOB   **DebugOptions
  );

EFI_STATUS
GnbRegisterRead (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       UINT8                      RegisterSpaceType,
  IN       UINT32                     Address,
  OUT      VOID                       *Value,
  IN       UINT32                     Flags,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  );

EFI_STATUS
GnbRegisterWrite (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       UINT8                      RegisterSpaceType,
  IN       UINT32                     Address,
  IN       VOID                       *Value,
  IN       UINT32                     Flags,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
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

EFI_STATUS
EFIAPI
NbioBaseInitAudioCallbackPpi (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR        *NotifyDescriptor,
  IN VOID                             *Ppi
  );
  
VOID
NbioNBIFPoisonDataWorkaroundST (
  IN GNB_HANDLE                 *GnbHandle
  );

VOID
NbioNBIFParityErrorsWorkaroundST (
  IN GNB_HANDLE                 *GnbHandle
  );

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */

STATIC PEI_HYGON_NBIO_BASE_SERVICES_PPI  mNbioBaseServicePpi = {
  HYGON_NBIO_BASE_SERVICES_REVISION,  ///< revision
  DebugGetConfig,
  GnbRegisterRead,
  GnbRegisterWrite
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mNbioBaseServicesPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonNbioBaseServicesStPpiGuid,
  &mNbioBaseServicePpi
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mNotifyMemDonePpi = {
  EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gHygonMemoryInfoHobPpiGuid,
  MemoryConfigDoneCallbackPpi
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mNotifyNbioTopologyPpi = {
  EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gHygonNbioPcieServicesPpiGuid,
  NbioTopologyConfigureCallbackPpi
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mNotifyNbioAudioPpi = {
  EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gHygonCpmTablePpiGuid,
  NbioBaseInitAudioCallbackPpi
};
/*----------------------------------------------------------------------------------------*/

/**
 * DebugGetConfig PPI function
 *
 *
 *
 * @param[in]  This          PEI_HYGON_NBIO_BASE_SERVICES_PPI pointer
 * @param[out] DebugOptions  Pointer to GNB_BUILD_OPTIONS_ST_DATA_HOB pointer
 * @retval     EFI_STATUS
 */
EFI_STATUS
DebugGetConfig (
  IN       PEI_HYGON_NBIO_BASE_SERVICES_PPI      *This,
  OUT   GNB_BUILD_OPTIONS_ST_DATA_HOB       **DebugOptions
  )
{
  EFI_STATUS                     Status;
  EFI_PEI_HOB_POINTERS           Hob;
  GNB_BUILD_OPTIONS_ST_DATA_HOB  *GnbBuildOptionsSTDataHob;

  GnbBuildOptionsSTDataHob = NULL;

  Status = PeiServicesGetHobList (&Hob.Raw);

  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION &&
        CompareGuid (&Hob.Guid->Name, &gGnbNbioBaseSTHobInfoGuid)) {
      GnbBuildOptionsSTDataHob = (GNB_BUILD_OPTIONS_ST_DATA_HOB *)(Hob.Raw);
      Status = EFI_SUCCESS;
      break;
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  *DebugOptions = GnbBuildOptionsSTDataHob;
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
 * @retval    HGPI_STATUS
 */
EFI_STATUS
GnbRegisterRead (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  OUT      VOID                *Value,
  IN       UINT32              Flags,
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  EFI_STATUS  status;

  status = NbioRegisterRead (GnbHandle, RegisterSpaceType, Address, Value, Flags);
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
 * @retval    HGPI_STATUS
 */
EFI_STATUS
GnbRegisterWrite (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags,
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  EFI_STATUS  status;

  status = NbioRegisterWrite (GnbHandle, RegisterSpaceType, Address, Value, Flags);
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
GnbLoadBuildOptionDataST (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                     Status;
  GNB_BUILD_OPTIONS_ST_DATA_HOB  *GnbBuildOptionsSTDataHob;

  // 1. Create Build Options Structure (in HOB)
  Status = PeiServicesCreateHob (
             EFI_HOB_TYPE_GUID_EXTENSION,
             sizeof (GNB_BUILD_OPTIONS_ST_DATA_HOB),
             &GnbBuildOptionsSTDataHob
             );
  IDS_HOOK (IDS_HOOK_NBIO_LOAD_BUILD_OPTION, NULL, NULL);
  GnbBuildOptionsSTDataHob->EfiHobGuidType.Name = gGnbNbioBaseSTHobInfoGuid;
  GnbBuildOptionsSTDataHob->CfgNbioRASEnable    = PcdGetBool (PcdHygonNbioRASControl);
  GnbBuildOptionsSTDataHob->CfgNbioPoisonConsumption = PcdGetBool (PcdHygonNbioPoisonConsumption);
  GnbBuildOptionsSTDataHob->CfgIOHCClkGatinSgupport  = PcdGetBool (PcdIOHCClkGatingSupport);
  GnbBuildOptionsSTDataHob->CfgIommuSupport = PcdGetBool (PcdCfgIommuSupport);

  GnbBuildOptionsSTDataHob->GnbCommonOptions.CfgIommuL2ClockGatingEnable = PcdGetBool (PcdIommuL2ClockGatingEnable);
  GnbBuildOptionsSTDataHob->GnbCommonOptions.CfgIommuL1ClockGatingEnable = PcdGetBool (PcdIommuL1ClockGatingEnable);

  GnbBuildOptionsSTDataHob->CfgChubClockGating = PcdGetBool (PcdCfgChubClockGating);
  GnbBuildOptionsSTDataHob->CfgAcpClockGating  = PcdGetBool (PcdCfgAcpClockGating);
  GnbBuildOptionsSTDataHob->CfgAxgDisable = PcdGetBool (PcdCfgAxgDisable);
  GnbBuildOptionsSTDataHob->CfgPcieHwInitPwerGating = PcdGet8 (PcdCfgPcieHwInitPwerGating);
  GnbBuildOptionsSTDataHob->CfgAriSupport = PcdGetBool (PcdCfgPcieAriSupport);
  GnbBuildOptionsSTDataHob->CfgSpgClockGatingEnable   = PcdGetBool (PcdCfgSpgClockGatingEnable);
  GnbBuildOptionsSTDataHob->CfgAcgAzClockGatingEnable = PcdGetBool (PcdCfgAcgAzClockGatingEnable);
  GnbBuildOptionsSTDataHob->CfgIommuL1MemPowerGating  = PcdGetBool (PcdCfgIommuL1MemPowerGating);
  GnbBuildOptionsSTDataHob->CfgIommuL2MemPowerGating  = PcdGetBool (PcdCfgIommuL2MemPowerGating);
  GnbBuildOptionsSTDataHob->CfgOrbTxMemPowerGating    = PcdGet8 (PcdCfgOrbTxMemPowerGating);
  GnbBuildOptionsSTDataHob->CfgOrbRxMemPowerGating    = PcdGet8 (PcdCfgOrbRxMemPowerGating);
  GnbBuildOptionsSTDataHob->CfgSstunlClkGating = PcdGetBool (PcdCfgSstunlClkGating);
  GnbBuildOptionsSTDataHob->CfgParityErrorConfiguration  = PcdGetBool (PcdCfgParityErrorConfiguration);
  GnbBuildOptionsSTDataHob->CfgSpgMemPowerGatingEnable   = PcdGet8 (PcdCfgSpgMemPowerGatingEnable);
  GnbBuildOptionsSTDataHob->CfgAcgAzMemPowerGatingEnable = PcdGet8 (PcdCfgAcgAzMemPowerGatingEnable);
  GnbBuildOptionsSTDataHob->CfgBifMemPowerGatingEnable   = PcdGet8 (PcdCfgBifMemPowerGatingEnable);
  GnbBuildOptionsSTDataHob->CfgSDMAMemPowerGatingEnable  = PcdGetBool (PcdCfgSDMAMemPowerGatingEnable);
  GnbBuildOptionsSTDataHob->CfgPcieTxpwrInOff = PcdGet8 (PcdCfgPcieTxpwrInOff);
  GnbBuildOptionsSTDataHob->CfgPcieRxpwrInOff = PcdGet8 (PcdCfgPcieRxpwrInOff);
  GnbBuildOptionsSTDataHob->CfgSmuDeterminismAmbient = PcdGet32 (PcdCfgSmuDeterminismAmbient);
  GnbBuildOptionsSTDataHob->CfgBTCEnable      = PcdGetBool (PcdCfgBTCEnable);
  GnbBuildOptionsSTDataHob->CfgPSIEnable      = PcdGetBool (PcdCfgPSIEnable);
  GnbBuildOptionsSTDataHob->CfgACSEnable      = PcdGetBool (PcdCfgACSEnable);
  GnbBuildOptionsSTDataHob->CfgNTBEnable      = PcdGetBool (PcdCfgNTBEnable);
  GnbBuildOptionsSTDataHob->CfgNTBLocation    = PcdGet8 (PcdCfgNTBLocation) >> 4;
  GnbBuildOptionsSTDataHob->CfgNTBPcieCoreSel = PcdGet8 (PcdCfgNTBPcieCoreSel);
  GnbBuildOptionsSTDataHob->CfgNTBMode = PcdGet8 (PcdCfgNTBMode);
  GnbBuildOptionsSTDataHob->CfgNTBLinkSpeed  = PcdGet8 (PcdCfgNTBLinkSpeed);
  GnbBuildOptionsSTDataHob->CfgNTBBAR1Size   = PcdGet8 (PcdCfgNTBBAR1Size);
  GnbBuildOptionsSTDataHob->CfgNTBBAR23Size  = PcdGet8 (PcdCfgNTBBAR23Size);
  GnbBuildOptionsSTDataHob->CfgNTBBAR45Size  = PcdGet8 (PcdCfgNTBBAR45Size);
  GnbBuildOptionsSTDataHob->CfgX2APICSupport = PcdGetBool (PcdX2ApicMode);

  GnbBuildOptionsSTDataHob->GnbCommonOptions.CfgScsSupport               = PcdGetBool (PcdCfgScsSupport);
  GnbBuildOptionsSTDataHob->GnbCommonOptions.CfgUmaSteering              = PcdGet8 (PcdCfgUmaSteering);
  GnbBuildOptionsSTDataHob->GnbCommonOptions.GmcPowerGating              = PcdGet8 (PcdGmcPowerGating);
  GnbBuildOptionsSTDataHob->GnbCommonOptions.CfgGmcClockGating           = PcdGetBool (PcdCfgGmcClockGating);
  GnbBuildOptionsSTDataHob->GnbCommonOptions.CfgOrbDynWakeEnable         = PcdGetBool (PcdCfgOrbDynWakeEnable);
  GnbBuildOptionsSTDataHob->GnbCommonOptions.CfgOrbClockGatingEnable     = PcdGetBool (PcdCfgOrbClockGatingEnable);
  GnbBuildOptionsSTDataHob->GnbCommonOptions.CfgIocLclkClockGatingEnable = PcdGetBool (PcdCfgIocLclkClockGatingEnable);
  GnbBuildOptionsSTDataHob->GnbCommonOptions.CfgBapmSupport              = PcdGetBool (PcdCfgBapmSupport);
  GnbBuildOptionsSTDataHob->GnbCommonOptions.CfgDcTdpEnable              = PcdGetBool (PcdCfgDcTdpEnable);
  GnbBuildOptionsSTDataHob->GnbCommonOptions.CfgGnbNumDisplayStreamPipes = PcdGet8 (PcdCfgGnbNumDisplayStreamPipes);

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Callback to Enable IOAPIC on GNB
 *
 *
 *
 * @param[in] GnbHandle          GnbHandle
 * @retval    HGPI_STATUS
 */
HGPI_STATUS
STATIC
IoapicEnableCallback (
  IN       GNB_HANDLE          *GnbHandle
  )
{
  UINT32              Value;
  UINT64              GnbNbIoapicAddress;

  if (!GnbHandle->IohubPresent) {
    return HGPI_SUCCESS;
  }

  if ((GnbHandle->SocketId == 0) && (GnbHandle->LogicalDieId == 0) && (GnbHandle->RbId == 0)) {
    GnbNbIoapicAddress = PcdGet64 (PcdCfgGnbIoapicAddress);
    if (GnbNbIoapicAddress != 0x0ull) {
      Value = (UINT32)RShiftU64 (GnbNbIoapicAddress, 32);
      GnbRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_BASE_ADDR_HI_REG_HYGX), &Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
      Value = GnbNbIoapicAddress & 0xFFFFFF00;
      GnbRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_BASE_ADDR_LOW_REG_HYGX), &Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
      Value |= 1;
      GnbRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_BASE_ADDR_LOW_REG_HYGX), &Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
    }

    GnbRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_FEATURES_ENABLE_ADDRESS_HYGX), &Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
    Value |= 1 << 2;    // Ioapic_id_ext_en
    Value |= 1 << 4;    // Ioapic_sb_feature_en
    GnbRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_FEATURES_ENABLE_ADDRESS_HYGX), &Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
  } else {
    GnbRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_FEATURES_ENABLE_ADDRESS_HYGX), &Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
    Value |= 1 << 2;    // Ioapic_id_ext_en
    Value |= 1 << 4;    // Ioapic_sb_feature_en
    Value |= 1 << 5;    // Ioapic_secondary_en
    GnbRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_FEATURES_ENABLE_ADDRESS_HYGX), &Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
  }

  Value  = GnbHandle->Address.Address.Bus;
  Value |= 1 << 8;
  GnbRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NB_BUS_NUM_CNTL_ADDRESS_HYGX), &Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to initialize NBIO Base function
 *
 *
 *
 * @param[in] GnbHandle          GnbHandle
 * @retval    HGPI_STATUS
 */
HGPI_STATUS
STATIC
NbioBaseInitCallback (
  IN       GNB_HANDLE          *GnbHandle
  )
{
  UINT32                        Value;
  POISON_ACTION_CONTROL_STRUCT  PoisonActionCtrl;

  PoisonActionCtrl.Value = 0;

  if (GnbHandle->IohubPresent) {
    // initialize IOMMU
    GnbRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOMMU_MMIO_CONTROL0_W_HYGX), &Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
    IDS_HDT_CONSOLE (MAIN_FLOW, "BXXD00F2x080 = 0x%x\n", Value);
    GnbRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOMMUL2_MMIO_CONTROL0_W_HYGX), &Value, 0, (HYGON_CONFIG_PARAMS *)NULL);

    GnbRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOMMU_MMIO_CNTRL_1_HYGX), &Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
    IDS_HDT_CONSOLE (MAIN_FLOW, "0x0580001C = 0x%x\n", Value);
    GnbRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOMMUL2_MMIO_CNTRL_1_HYGX), &Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
  }

  if ((GnbHandle->DieType == IOD_EMEI) && ((GnbHandle->RbId % 2) == 0)) {
    if (PcdGetBool (PcdCfgNTBClockGatingEnable)) {
      GnbRegisterRead (GnbHandle, NTB_CTRL_TYPE, NBIO_SPACE (GnbHandle, NTB_CTRL_ADDRESS_HYGX), &Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
      Value |= (1 << NTB_CTRL_CLK_GATE_EN_OFFSET);
      GnbRegisterWrite (GnbHandle, NTB_CTRL_TYPE, NBIO_SPACE (GnbHandle, NTB_CTRL_ADDRESS_HYGX), &Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
    }
  }

  if (GnbHandle->IohubPresent) {
    // Enable/disable NBIO Poison Consumption actions
    GnbRegisterRead (GnbHandle, POISON_ACTION_CONTROL_TYPE, NBIO_SPACE (GnbHandle, POISON_ACTION_CONTROL_ADDRESS_HYGX), &PoisonActionCtrl.Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
    if (!PcdGetBool (PcdHygonNbioPoisonConsumption)) {
      PoisonActionCtrl.Field.IntPoisonAPMLErrEn        = 0x1;
      PoisonActionCtrl.Field.IntPoisonLinkDisEn        = 0x1;
      PoisonActionCtrl.Field.IntPoisonSyncFloodEn      = 0x1;
      PoisonActionCtrl.Field.EgressPoisonLSAPMLErrEn   = 0x0;
      PoisonActionCtrl.Field.EgressPoisonLSLinkDisEn   =  0x0;
      PoisonActionCtrl.Field.EgressPoisonLSSyncFloodEn =  0x0;
      PoisonActionCtrl.Field.EgressPoisonHSAPMLErrEn   = 0x1;
      PoisonActionCtrl.Field.EgressPoisonHSLinkDisEn   =  0x1;
      PoisonActionCtrl.Field.EgressPoisonHSSyncFloodEn =  0x1;
      GnbRegisterWrite (GnbHandle, POISON_ACTION_CONTROL_TYPE, NBIO_SPACE (GnbHandle, POISON_ACTION_CONTROL_ADDRESS_HYGX), &PoisonActionCtrl.Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
    }

    IDS_HDT_CONSOLE (MAIN_FLOW, "PoisonActionCtrl = 0x%x\n", PoisonActionCtrl.Value);
  }

  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**

 * configure HDAudio verb table
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    HGPI_STATUS
 */
/*----------------------------------------------------------------------------------------*/

VOID
HDAudioVerbTableSetting (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  EFI_STATUS                        Status;
  FABRIC_TARGET                     MmioTarget;
  FABRIC_MMIO_ATTRIBUTE             MmioAttr;
  UINT64                            HDAudioMmioBase;
  UINT64                            HDAudioMmioSize;
  VOID                              *VerbTableAddress;
  UINT32                            Value;
  UINT32                            MmioBase;
  UINT64                            Align;

  IDS_HDT_CONSOLE (MAIN_FLOW, "HDAudioVerbTableSetting Entry\n");

  NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3EPF3CFG_VENDOR_ID_ADDRESS_HYGX), &Value, 0);
  if (Value != 0xFFFFFFFF) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "HDAudio ID = 0x%x\n", Value);

    // Get Audio BAR0 size
    MmioBase = 0xFFFFFFFF;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3EPF3CFG_BAR0_ADDRESS_HYGX), &MmioBase, 0);
    NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3EPF3CFG_BAR0_ADDRESS_HYGX), &MmioBase, 0);
    IDS_HDT_CONSOLE (MAIN_FLOW, "   MmioBase read from BAR10 %x\n", MmioBase);

    // Allocate MMIO for Audio BAR0
    MmioTarget.TgtType   = TARGET_RB;
    MmioTarget.SocketNum = 0;
    MmioTarget.DieNum    = 0;
    MmioTarget.RbNum     = 0;
    MmioAttr.ReadEnable  = 1;
    MmioAttr.WriteEnable = 1;
    MmioAttr.NonPosted   = 0;
    MmioAttr.MmioType    = NON_PCI_DEVICE_BELOW_4G;
    HDAudioMmioSize = 0xFFFFFFFF - MmioBase + 1;
    Align = ~MmioBase;

    Status = FabricAllocateMmio (&HDAudioMmioBase, &HDAudioMmioSize, Align, MmioTarget, &MmioAttr);
    IDS_HDT_CONSOLE (MAIN_FLOW, "   HDAudioMmioBase = %x,HDAudioMmioSize = %x\n", (UINT32)HDAudioMmioBase, (UINT32)HDAudioMmioSize);
    if (Status == EFI_SUCCESS) {
      // Set temp MMIO base for audio
      MmioBase = (UINT32)HDAudioMmioBase;
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3EPF3CFG_BAR0_ADDRESS_HYGX), &MmioBase, 0);
      Value = (MmioBase & 0xFFF00000) | ((MmioBase & 0xFFF00000) >> 16);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3RCCFG_VENDOR_ID_ADDRESS_HYGX + PCICFG_SPACE_MEMORY_BASE_OFFSET), &Value, 0);

      // Set temp bus for audio
      NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3RCCFG_VENDOR_ID_ADDRESS_HYGX + PCICFG_SPACE_PRIMARY_BUS_OFFSET), &Value, 0);
      NbioRegisterRMW (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3RCCFG_VENDOR_ID_ADDRESS_HYGX + PCICFG_SPACE_PRIMARY_BUS_OFFSET), 0xFF0000FF, 0x0a0a00, 0);

      // Enable NBIF3 and audio bus and MMIO access
      NbioRegisterRMW (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3RCCFG_VENDOR_ID_ADDRESS_HYGX + PCICFG_SPACE_COMMAND_OFFSET), (UINT32) ~(BIT1 | BIT2), (BIT1 | BIT2), 0);
      NbioRegisterRMW (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3EPF3CFG_COMMAND_ADDRESS_HYGX), (UINT32) ~(BIT1 | BIT2), (BIT1 | BIT2), 0);

      // Config Audio codec verb table
      VerbTableAddress = (VOID*) PcdGet32 (PcdCfgAzaliaCodecVerbTable);
      if (VerbTableAddress != 0) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "   Configure Verb Table (%X)\n", VerbTableAddress);
        NbioConfigureVerbTable (MmioBase, VerbTableAddress);
      }

      // Clear temp bus and MMIO resource
      MmioBase = 0;
      NbioRegisterRMW (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3EPF3CFG_COMMAND_ADDRESS_HYGX), (UINT32) ~(BIT1 | BIT2), 0, 0);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3EPF3CFG_BAR0_ADDRESS_HYGX), &MmioBase, 0);
      NbioRegisterRMW (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3RCCFG_VENDOR_ID_ADDRESS_HYGX + PCICFG_SPACE_COMMAND_OFFSET), (UINT32) ~(BIT1 | BIT2), 0, 0);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3RCCFG_VENDOR_ID_ADDRESS_HYGX + PCICFG_SPACE_PRIMARY_BUS_OFFSET), &Value, 0);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3RCCFG_VENDOR_ID_ADDRESS_HYGX + PCICFG_SPACE_MEMORY_BASE_OFFSET), &MmioBase, 0);
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "HDAudioVerbTableSetting Exit\n");
}


/*----------------------------------------------------------------------------------------*/

/**
 * IsAudioEnable
 *
 *
 *
 * @param[in] VOID
 * @retval    BOOLEAN
 *              TRUE: Audio enable
 *              FALSE: Audio disable
 */
BOOLEAN
IsAudioEnable (
  VOID
  )
{
  BOOLEAN     Enable;
  UINT8       PkgType;

  Enable = FALSE;
  PkgType = GetSocPkgType ();

  if ((PkgType == DM1_102) && PcdGetBool (PcdCfgHdAudioEnable)) {
    Enable = TRUE;
  }
  
  return Enable;
}
/*----------------------------------------------------------------------------------------*/

/**
 * Initialize Dujiang Audio
 *
 *
 *
 * @param[in] GnbHandle          GnbHandle
 * @retval    HGPI_STATUS
 */
EFI_STATUS
EFIAPI
NbioBaseInitAudioCallbackPpi (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR        *NotifyDescriptor,
  IN VOID                             *Ppi
  )
{
  PEI_HYGON_NBIO_PCIE_SERVICES_PPI  *PcieServicesPpi;
  PCIe_PLATFORM_CONFIG              *Pcie;
  EFI_STATUS                        Status;
  GNB_HANDLE                        *GnbHandle;
  GNB_PCIE_INFORMATION_DATA_HOB     *PciePlatformConfigHob;

  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioBaseInitAudioCallbackPpi Entry\n");
  GnbHandle = NULL;

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonNbioPcieServicesPpiGuid,
                             0,
                             NULL,
                             (VOID **)&PcieServicesPpi
                             );
  PcieServicesPpi->PcieGetTopology (PcieServicesPpi, &PciePlatformConfigHob);
  Pcie = &(PciePlatformConfigHob->PciePlatformConfigHob);

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    if (GnbHandle->DieType == IOD_DUJIANG) {
      if ((GnbHandle->SocketId == 0) && (GnbHandle->PhysicalDieId == 0) && IsAudioEnable()) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Enable HsioHDaudioEngine for Socket %d Logical Die %d \n", GnbHandle->SocketId, GnbHandle->PhysicalDieId);
        NbioRegisterRMW (
          GnbHandle,
          TYPE_SMN,
          NBIO_SPACE (GnbHandle, NBIF3_HDAUDIO_STRAP0_ADDRESS_HYGX),
          (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
          (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
          0
          );

        IDS_HDT_CONSOLE (MAIN_FLOW, "  Enable audio interrupt \n");
        NbioRegisterRMW (
          GnbHandle,
          NBIFMISC_INTR_LINE_ENABLE_TYPE,
          NBIO_SPACE (GnbHandle, NBIFMISC3_INTR_LINE_ENABLE_ADDRESS_HYGX),
          (UINT32) ~(1 << 3),
          (1 << 3),
          0
          );
            
        HDAudioVerbTableSetting (GnbHandle);
      } else {
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Disable HsioHDaudioEngine\n");
        NbioRegisterRMW (
          GnbHandle,
          TYPE_SMN,
          NBIO_SPACE (GnbHandle, NBIF3_HDAUDIO_STRAP0_ADDRESS_HYGX),
          (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
          (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
          0
          );
        NbioRegisterRMW (
          GnbHandle,
          NBIFMISC_INTR_LINE_ENABLE_TYPE,
          NBIO_SPACE (GnbHandle, NBIFMISC3_INTR_LINE_ENABLE_ADDRESS_HYGX),
          (UINT32) ~(1 << 3),
          (0 << 3),
          0
          );
      }
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioBaseInitAudioCallbackPpi Exit\n");
  return HGPI_SUCCESS;
}

/**
 * Initialize Dujiang ASP
 *
 *
 *
 * @param[in] GnbHandle          GnbHandle
 * @retval    HGPI_STATUS
 */
HGPI_STATUS
STATIC
NbioBaseInitASP (
  IN       GNB_HANDLE          *GnbHandle
  )
{
  if (GnbHandle->DieType == IOD_DUJIANG) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  NbioBaseInitASP entry \n");
    
    IDS_HDT_CONSOLE (MAIN_FLOW, "  NbioBaseInitASP exit \n");
  }
  return HGPI_SUCCESS;
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
HygonNbioBaseInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  PEI_HYGON_NBIO_PCIE_SERVICES_PPI  *PcieServicesPpi;
  PCIe_PLATFORM_CONFIG              *Pcie;
  UINT32                            Property;
  EFI_STATUS                        Status;
  GNB_HANDLE                        *GnbHandle;
  HYGON_CONFIG_PARAMS               *StdHeader;
  GNB_PCIE_INFORMATION_DATA_HOB     *PciePlatformConfigHob;
  GNB_BUILD_OPTIONS_ST_DATA_HOB     *GnbBuildOptionData;
  PEI_HYGON_NBIO_BASE_SERVICES_PPI  *NbioBaseServices;
  RCC_BIF_STRAP1_STRUCT             BifStrap1;
  UINTN                             SocketNumber;
  NBMISC_NB_IOC_CNTL_STRUCT         NbCntl;

  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioBaseHyGxInit Entry\n");
  StdHeader = NULL;
  GnbHandle = NULL;

  // Need topology structure
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonNbioPcieServicesPpiGuid,
                             0,
                             NULL,
                             (VOID **)&PcieServicesPpi
                             );
  PcieServicesPpi->PcieGetTopology (PcieServicesPpi, &PciePlatformConfigHob);
  Pcie = &(PciePlatformConfigHob->PciePlatformConfigHob);

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonNbioBaseServicesStPpiGuid,
                             0,
                             NULL,
                             (VOID **)&NbioBaseServices
                             );

  NbioBaseServices->DebugOptions (NbioBaseServices, &GnbBuildOptionData);

  // Need debug options
  Property = TABLE_PROPERTY_DEFAULT;

  SocketNumber = FabricTopologyGetNumberOfSocketPresent ();
  if (SocketNumber >= 4) {
    Property |= TABLE_PROPERTY_DISABLE_NBIO23;
  }

  Property |= GnbBuildOptionData->CfgIOHCClkGatinSgupport ? TABLE_PROPERTY_IOHC_Clock_GATING : 0;
  Property |= GnbBuildOptionData->CfgIommuSupport ? 0 : TABLE_PROPERTY_IOMMU_DISABLED;
  Property |= GnbBuildOptionData->GnbCommonOptions.CfgIommuL2ClockGatingEnable ? TABLE_PROPERTY_IOMMU_L2_CLOCK_GATING : 0;
  Property |= GnbBuildOptionData->GnbCommonOptions.CfgIommuL1ClockGatingEnable ? TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING : 0;
  Property |= GnbBuildOptionData->CfgNbioPoisonConsumption ? TABLE_PROPERTY_POISON_ACTION_CONTROL : 0;
  Property |= GnbBuildOptionData->CfgSstunlClkGating ? 0 : TABLE_PROPERTY_SST_CLOCK_GATING_DISABLED;
  Property |= GnbBuildOptionData->CfgX2APICSupport ? 0 : TABLE_PROPERTY_IOMMU_X2APIC_SUPPORT;

  // Adjust Property based on Debug Info and PCD's
  // From Earlier

  // Step 1, Init IOHC and IOAPIC
  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "GnbHandle->SocketId %d PhysicalDieId %d IohubPresent %d \n", GnbHandle->SocketId, GnbHandle->PhysicalDieId, GnbHandle->IohubPresent);
    
    if (GnbHandle->IohubPresent) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "Init Socket %d Die P%d Rb %d : \n", GnbHandle->SocketId, GnbHandle->PhysicalDieId, GnbHandle->RbId);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Init GnbEarlyInitTableIohub start\n");
      Status = GnbProcessTable (
                 GnbHandle,
                 GnbEarlyInitTableIohub,
                 Property,
                 0,
                 StdHeader
                 );
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Init GnbEarlyInitTableIohub end\n");
    }

    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  // Step 2
  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Init Socket %d Die P%d Rb %d : \n", GnbHandle->SocketId, GnbHandle->PhysicalDieId, GnbHandle->RbId);

    if (GnbHandle->DieType == IOD_EMEI) {
      if ((GnbHandle->LogicalDieId == 0) & (GnbHandle->RbId == 0)) {
        //EMEI NBIO 0 with DJ NBIO 0
        //Set EMEI NBIO0 EMEI_AND_DJ_SIDEBAND_ENABLE = 1
        GnbRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (1, NB_IOC_CNTL_ADDRESS_HYGX), &NbCntl.Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
        NbCntl.Field.EMEI_AND_DJ_SIDEBAND_ENABLE = 1;
        GnbRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (1, NB_IOC_CNTL_ADDRESS_HYGX), &NbCntl.Value, 0, (HYGON_CONFIG_PARAMS *)NULL);

        //Set DJ EMEI_AND_DJ_SIDEBAND_ENABLE = 1
        GnbRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (0, NB_IOC_CNTL_ADDRESS_HYGX), &NbCntl.Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
        NbCntl.Field.EMEI_AND_DJ_SIDEBAND_ENABLE = 1;
        GnbRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (0, NB_IOC_CNTL_ADDRESS_HYGX), &NbCntl.Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
      }

      if ((GnbHandle->RbId % 2) == 0) {
        //Full NBIO
        GnbRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, RCC_NBIF0_STRAP1_ADDRESS_HYGX), &BifStrap1.Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
        BifStrap1.Field.STRAP_RP_BUSNUM = GnbHandle->Address.Address.Bus;
        GnbRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, RCC_NBIF0_STRAP1_ADDRESS_HYGX), &BifStrap1.Value, 0, (HYGON_CONFIG_PARAMS *)NULL);

        Property |= TABLE_PROPERTY_EMEI_FULL_NBIO;
      } else {
        //Lite NBIO
        Property &= ~TABLE_PROPERTY_EMEI_FULL_NBIO;
      }

      IDS_HDT_CONSOLE (MAIN_FLOW, "  Init GnbEarlyInitTableEmei start\n");
      Status = GnbProcessTable (
                 GnbHandle,
                 GnbEarlyInitTableEmei,
                 Property,
                 0,
                 StdHeader
                 );
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Init GnbEarlyInitTableEmei end\n");
    }

    if (GnbHandle->DieType == IOD_DUJIANG) {
      GnbRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, RCC_NBIF2_STRAP1_ADDRESS_HYGX), &BifStrap1.Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
      BifStrap1.Field.STRAP_RP_BUSNUM = GnbHandle->Address.Address.Bus;
      GnbRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, RCC_NBIF2_STRAP1_ADDRESS_HYGX), &BifStrap1.Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
      GnbRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, RCC_NBIF3_STRAP1_ADDRESS_HYGX), &BifStrap1.Value, 0, (HYGON_CONFIG_PARAMS *)NULL);
      BifStrap1.Field.STRAP_RP_BUSNUM = GnbHandle->Address.Address.Bus;
      GnbRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, RCC_NBIF3_STRAP1_ADDRESS_HYGX), &BifStrap1.Value, 0, (HYGON_CONFIG_PARAMS *)NULL);

      IDS_HDT_CONSOLE (MAIN_FLOW, "  Init GnbEarlyInitTableDj start\n");

      Status = GnbProcessTable (
                 GnbHandle,
                 GnbEarlyInitTableDj,
                 Property,
                 0,
                 StdHeader
                 );
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Init GnbEarlyInitTableDj end\n");
    }

    //IDS_HOOK (IDS_HOOK_NBIO_BASE_INIT, NULL, (void *)GnbHandle);

    IoapicEnableCallback (GnbHandle);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  IoapicEnableCallback end\n");

    if (PcdGetBool (PcdHygonNbioRASControl)) {
      NbioNBIFParityErrorsWorkaroundST (GnbHandle);
    }

    NbioNBIFPoisonDataWorkaroundST (GnbHandle);
    NbioBaseInitCallback (GnbHandle);
    NbioBaseInitASP (GnbHandle);

    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioBaseHyGxInit Exit\n");
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
  HGPI_TESTPOINT (TpNbioTopologyConfigureCallbackEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioTopologyConfigureCallbackPpi Entry\n");
  // At this point we know the NBIO topology, so we can initialize all NBIO base registers
  HygonNbioBaseInit (PeiServices);
  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioTopologyConfigureCallbackPpi Exit\n");
  HGPI_TESTPOINT (TpNbioTopologyConfigureCallbackExit, NULL);
  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HygonNbiosBase driver entry point for ST
 *
 *
 *
 * @param[in]  FileHandle  Standard configuration header
 * @param[in]  PeiServices Pointer to EFI_PEI_SERVICES pointer
 * @retval     EFI_STATUS
 */
EFI_STATUS
EFIAPI
HygonNbioBaseHyGxPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS    Status;

  HGPI_TESTPOINT (TpNbioBasePeiEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioBaseHyGxPeiEntry Entry\n");

  // 1. Set conditionals based on debug configuration and PCDs
  GnbLoadBuildOptionDataST (PeiServices);

  // 2. PCIe topology is dependent on NbioBaseServicesPpi, so Pcie driver will not publish PPI until after
  // this code is completed. We can simply request notification when gHygonNbioPcieServicesPpiGuid is published
  Status = (**PeiServices).NotifyPpi (PeiServices, &mNotifyNbioTopologyPpi);

  // 3. Publish callback for memory config done so that we can set top of memory
  Status = (**PeiServices).NotifyPpi (PeiServices, &mNotifyMemDonePpi);

  /// 4. Publish callback for initing Nbio Audio
  Status = (**PeiServices).NotifyPpi (PeiServices, &mNotifyNbioAudioPpi);

  // 5. Install PPI for NbioBaseServices
  Status = PeiServicesInstallPpi (&mNbioBaseServicesPpiList); // This driver is done.
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioBaseHyGxPeiEntry Exit\n");
  HGPI_TESTPOINT (TpNbioBasePeiExit, NULL);
  return EFI_SUCCESS;
}
