/* $NoKeywords:$ */

/**
 * @file
 *
 * HygonNbioPcieDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioPcieDxe
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
#include <PiDxe.h>
#include <Filecode.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/HygonNbioBaseServicesProtocol.h>
#include <Protocol/HygonNbioPcieServicesProtocol.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Guid/GnbNbioBaseSTInfoHob.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Guid/HobList.h>
#include <Protocol/PciIo.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <GnbRegisters.h>
#include <Library/NbioRegisterAccLib.h>

#define FILECODE  NBIO_NBIOBASE_HYGX_HYGONNBIOBASEDXE_HYGONNBIOBASEDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

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

/*----------------------------------------------------------------------------------------*/

/**
 * debug config
 *
 *
 *
 * @param[in]     This
 * @param[in]     **DebugOptions
 */
EFI_STATUS
HygonDebugGetConfig (
  IN       DXE_HYGON_NBIO_DEBUG_SERVICES_PROTOCOL     *This,
  OUT   UINT32                                   **DebugOptions
  )
{
  EFI_STATUS                     Status;
  GNB_BUILD_OPTIONS_ST_DATA_HOB  *GnbBuildOptionsSTDataHob;

  //
  // Get information data from HOB.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &GnbBuildOptionsSTDataHob);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  GnbBuildOptionsSTDataHob = GetNextGuidHob (&gGnbNbioBaseSTHobInfoGuid, GnbBuildOptionsSTDataHob);
  if (GnbBuildOptionsSTDataHob == NULL) {
    return EFI_NOT_FOUND;
  }

  *DebugOptions = (UINT32 *)GnbBuildOptionsSTDataHob;
  return Status;
}

DXE_HYGON_NBIO_DEBUG_SERVICES_PROTOCOL  mDxeHygonNbioBaseServicesProtocol = {
  HYGON_NBIO_DEBUG_OPTION_SERVICES_DXE_REVISION,
  HygonDebugGetConfig
};

/*----------------------------------------------------------------------------------------*/

/**
 * set HWINIT_WR_LOCK
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 */
VOID
NbioBaseSetHwInitLock (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "NbioBaseSetHwInitLock Enter\n");

  if ((GnbHandle->DieType == IOD_EMEI) && ((GnbHandle->RbId % 2) == 0)) {
    NbioRegisterRMW (
      GnbHandle,
      DN_PCIE_CNTL_TYPE,
      NBIO_SPACE (GnbHandle, NBIF0_DN_PCIE_CNTL_SMN_ADDRESS_HYGX),
      (UINT32) ~(DN_PCIE_CNTL_HWINIT_WR_LOCK_MASK),
      1 << DN_PCIE_CNTL_HWINIT_WR_LOCK_OFFSET,
      GNB_REG_ACC_FLAG_S3SAVE
      );
  }

  if (GnbHandle->DieType == IOD_DUJIANG) {
    NbioRegisterRMW (
      GnbHandle,
      DN_PCIE_CNTL_TYPE,
      NBIO_SPACE (GnbHandle, NBIF2_DN_PCIE_CNTL_SMN_ADDRESS_HYGX),
      (UINT32) ~(DN_PCIE_CNTL_HWINIT_WR_LOCK_MASK),
      1 << DN_PCIE_CNTL_HWINIT_WR_LOCK_OFFSET,
      GNB_REG_ACC_FLAG_S3SAVE
      );
    NbioRegisterRMW (
      GnbHandle,
      DN_PCIE_CNTL_TYPE,
      NBIO_SPACE (GnbHandle, NBIF3_DN_PCIE_CNTL_SMN_ADDRESS_HYGX),
      (UINT32) ~(DN_PCIE_CNTL_HWINIT_WR_LOCK_MASK),
      1 << DN_PCIE_CNTL_HWINIT_WR_LOCK_OFFSET,
      GNB_REG_ACC_FLAG_S3SAVE
      );
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioBaseSetHwInitLock Exit\n");
}

/**
 *---------------------------------------------------------------------------------------
 *
 * create event for set HWINIT_WR_LOCK.
 *
 *
 *
 * @param[in]     Event
 * @param[in]     *Context
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
EFIAPI
NbioBaseHookPciIO (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                             Status;
  PCIe_PLATFORM_CONFIG                   *Pcie;
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL  *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB          *PciePlatformConfigHobData;
  GNB_HANDLE                             *GnbHandle;

  HGPI_TESTPOINT (TpNbioBaseHookPciIOEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioBaseHookPciIO Entry\n");
  Status = gBS->LocateProtocol (
                  &gHygonNbioPcieServicesProtocolGuid,
                  NULL,
                  &PcieServicesProtocol
                  );

  if (Status == EFI_SUCCESS) {
    PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **)&PciePlatformConfigHobData);
    Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      NbioBaseSetHwInitLock (GnbHandle);
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
  }

  gBS->CloseEvent (Event);
  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioBaseHookPciIO Exit\n");
  HGPI_TESTPOINT (TpNbioBaseHookPciIOExit, NULL);

  return;
}

VOID
EFIAPI
NbioClkGatingConfig (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{

  EFI_STATUS                             Status;
  PCIe_PLATFORM_CONFIG                   *Pcie;
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL  *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB          *PciePlatformConfigHobData;
  GNB_HANDLE                             *GnbHandle;

  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioClkGatingConfig Entry\n");
  Status = gBS->LocateProtocol (
                  &gHygonNbioPcieServicesProtocolGuid,
                  NULL,
                  &PcieServicesProtocol
                  );
  if (Status == EFI_SUCCESS) {
    PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **) &PciePlatformConfigHobData);
    Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      // IOHC_GLUE_CG_LCLK_CTRL_0
      NbioRegisterRMW (GnbHandle,
                      IOHC_GLUE_CG_LCLK_CTRL_0_TYPE,
                      NBIO_SPACE(GnbHandle, IOHC_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYGX),
                      (UINT32) ~(NBMISC_0088_CG_OFF_HYSTERESIS_MASK |
                        NBMISC_0088_SOFT_OVERRIDE_CLK9_MASK |
                        NBMISC_0088_SOFT_OVERRIDE_CLK8_MASK |
                        NBMISC_0088_SOFT_OVERRIDE_CLK7_MASK |
                        NBMISC_0088_SOFT_OVERRIDE_CLK6_MASK |
                        NBMISC_0088_SOFT_OVERRIDE_CLK5_MASK |
                        NBMISC_0088_SOFT_OVERRIDE_CLK4_MASK |
                        NBMISC_0088_SOFT_OVERRIDE_CLK3_MASK |
                        NBMISC_0088_SOFT_OVERRIDE_CLK2_MASK |
                        NBMISC_0088_SOFT_OVERRIDE_CLK1_MASK |
                        NBMISC_0088_SOFT_OVERRIDE_CLK0_MASK),
                      (0x10 << NBMISC_0088_CG_OFF_HYSTERESIS_OFFSET) |
                      (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK9_OFFSET) |
                      (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK8_OFFSET) |
                      (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK7_OFFSET) |
                      (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK6_OFFSET) |
                      (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK6_OFFSET) |
                      (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK4_OFFSET) |
                      (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK3_OFFSET) |
                      (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK2_OFFSET) |
                      (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK1_OFFSET) |
                      (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK0_OFFSET),
                      GNB_REG_ACC_FLAG_S3SAVE
                    );
      // IOHC_GLUE_CG_LCLK_CTRL_1
      NbioRegisterRMW (GnbHandle,
                      IOHC_GLUE_CG_LCLK_CTRL_1_TYPE,
                      NBIO_SPACE(GnbHandle, IOHC_GLUE_CG_LCLK_CTRL_1_ADDRESS_HYGX),
                      (UINT32) ~(NBMISC_008C_SOFT_OVERRIDE_CLK9_MASK |
                        NBMISC_008C_SOFT_OVERRIDE_CLK8_MASK |
                        NBMISC_008C_SOFT_OVERRIDE_CLK7_MASK |
                        NBMISC_008C_SOFT_OVERRIDE_CLK6_MASK |
                        NBMISC_008C_SOFT_OVERRIDE_CLK5_MASK |
                        NBMISC_008C_SOFT_OVERRIDE_CLK4_MASK |
                        NBMISC_008C_SOFT_OVERRIDE_CLK3_MASK |
                        NBMISC_008C_SOFT_OVERRIDE_CLK2_MASK |
                        NBMISC_008C_SOFT_OVERRIDE_CLK1_MASK |
                        NBMISC_008C_SOFT_OVERRIDE_CLK0_MASK),
                      (0x1 << NBMISC_008C_SOFT_OVERRIDE_CLK9_OFFSET) |
                      (0x1 << NBMISC_008C_SOFT_OVERRIDE_CLK8_OFFSET) |
                      (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK7_OFFSET) |
                      (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK6_OFFSET) |
                      (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK6_OFFSET) |
                      (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK4_OFFSET) |
                      (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK3_OFFSET) |
                      (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK2_OFFSET) |
                      (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK1_OFFSET) |
                      (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK0_OFFSET),
                      GNB_REG_ACC_FLAG_S3SAVE
                    );
      // IOHC_GLUE_CG_LCLK_CTRL_2
      NbioRegisterRMW (GnbHandle,
                      IOHC_GLUE_CG_LCLK_CTRL_2_TYPE,
                      NBIO_SPACE(GnbHandle, IOHC_GLUE_CG_LCLK_CTRL_2_ADDRESS_HYGX),
                      (UINT32) ~(NBMISC_0090_SOFT_OVERRIDE_CLK9_MASK |
                        NBMISC_0090_SOFT_OVERRIDE_CLK8_MASK |
                        NBMISC_0090_SOFT_OVERRIDE_CLK7_MASK |
                        NBMISC_0090_SOFT_OVERRIDE_CLK6_MASK |
                        NBMISC_0090_SOFT_OVERRIDE_CLK5_MASK |
                        NBMISC_0090_SOFT_OVERRIDE_CLK4_MASK |
                        NBMISC_0090_SOFT_OVERRIDE_CLK3_MASK |
                        NBMISC_0090_SOFT_OVERRIDE_CLK2_MASK |
                        NBMISC_0090_SOFT_OVERRIDE_CLK1_MASK |
                        NBMISC_0090_SOFT_OVERRIDE_CLK0_MASK),
                      (0x1 << NBMISC_0090_SOFT_OVERRIDE_CLK9_OFFSET) |
                      (0x1 << NBMISC_0090_SOFT_OVERRIDE_CLK8_OFFSET) |
                      (0x1 << NBMISC_0090_SOFT_OVERRIDE_CLK7_OFFSET) |
                      (0x1 << NBMISC_0090_SOFT_OVERRIDE_CLK6_OFFSET) |
                      (0x1 << NBMISC_0090_SOFT_OVERRIDE_CLK5_OFFSET) |
                      (0x1 << NBMISC_0090_SOFT_OVERRIDE_CLK4_OFFSET) |
                      (0x1 << NBMISC_0090_SOFT_OVERRIDE_CLK3_OFFSET) |
                      (0x1 << NBMISC_0090_SOFT_OVERRIDE_CLK2_OFFSET) |
                      (0x1 << NBMISC_0090_SOFT_OVERRIDE_CLK1_OFFSET) |
                      (0x0 << NBMISC_0090_SOFT_OVERRIDE_CLK0_OFFSET), 
                      GNB_REG_ACC_FLAG_S3SAVE
                  );
      // IOAPIC_GLUE_CG_LCLK_CTRL_0
      NbioRegisterRMW (GnbHandle,
                      IOAPIC_GLUE_CG_LCLK_CTRL_0_TYPE,
                      NBIO_SPACE(GnbHandle, IOAPIC_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYGX),
                      (UINT32) ~(NBMISC_0100_CG_OFF_HYSTERESIS_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK9_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK8_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK7_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK6_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK5_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK4_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK3_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK2_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK1_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK0_MASK),
                      (0x10 << NBMISC_0100_CG_OFF_HYSTERESIS_OFFSET) |
                      (0x1 << NBMISC_0100_SOFT_OVERRIDE_CLK9_OFFSET) |
                      (0x1 << NBMISC_0100_SOFT_OVERRIDE_CLK8_OFFSET) |
                      (0x1 << NBMISC_0100_SOFT_OVERRIDE_CLK7_OFFSET) |
                      (0x1 << NBMISC_0100_SOFT_OVERRIDE_CLK6_OFFSET) |
                      (0x1 << NBMISC_0100_SOFT_OVERRIDE_CLK5_OFFSET) |
                      (0x1 << NBMISC_0100_SOFT_OVERRIDE_CLK4_OFFSET) |
                      (0x1 << NBMISC_0100_SOFT_OVERRIDE_CLK3_OFFSET) |
                      (0x1 << NBMISC_0100_SOFT_OVERRIDE_CLK2_OFFSET) |
                      (0x0 << NBMISC_0100_SOFT_OVERRIDE_CLK1_OFFSET) |
                      (0x0 << NBMISC_0100_SOFT_OVERRIDE_CLK0_OFFSET), 
                      GNB_REG_ACC_FLAG_S3SAVE 
                    ); 
      // L2_L2A_CK_GATE_CONTROL
      NbioRegisterRMW (GnbHandle,
                      L2_L2A_CK_GATE_CONTROL_TYPE,
                      NBIO_SPACE(GnbHandle, L2_L2A_CK_GATE_CONTROL_ADDRESS_HYGX),
                      (UINT32) ~(IOMMUL2A_0CC_CKGateL2ARegsDisable_MASK |
                        IOMMUL2A_0CC_CKGateL2ADynamicDisable_MASK |
                        IOMMUL2A_0CC_CKGateL2ACacheDisable_MASK |
                        IOMMUL2A_0CC_CKGateL2ASpare_MASK |
                        IOMMUL2A_0CC_CKGateL2ALength_MASK |
                        IOMMUL2A_0CC_CKGateL2AStop_MASK),
                      (0x0 << IOMMUL2A_0CC_CKGateL2ARegsDisable_OFFSET) |
                      (0x0 << IOMMUL2A_0CC_CKGateL2ADynamicDisable_OFFSET) |
                      (0x0 << IOMMUL2A_0CC_CKGateL2ACacheDisable_OFFSET) |
                      (0x0 << IOMMUL2A_0CC_CKGateL2ASpare_OFFSET) |
                      (0x1 << IOMMUL2A_0CC_CKGateL2ALength_OFFSET) |
                      (0x1 << IOMMUL2A_0CC_CKGateL2AStop_OFFSET), 
                      GNB_REG_ACC_FLAG_S3SAVE 
                    ); 
      // L2_L2B_CK_GATE_CONTROL
      NbioRegisterRMW (GnbHandle,
                      L2_L2B_CK_GATE_CONTROL_TYPE,
                      NBIO_SPACE(GnbHandle, L2_L2B_CK_GATE_CONTROL_ADDRESS_HYGX),
                      (UINT32) ~(L2_L2B_CK_GATE_CONTROL_CKGateL2BRegsDisable_MASK |
                        L2_L2B_CK_GATE_CONTROL_CKGateL2BDynamicDisable_MASK |
                        L2_L2B_CK_GATE_CONTROL_CKGateL2BMiscDisable_MASK |
                        L2_L2B_CK_GATE_CONTROL_CKGateL2BCacheDisable_MASK |
                        L2_L2B_CK_GATE_CONTROL_CKGateL2BLength_MASK |
                        L2_L2B_CK_GATE_CONTROL_CKGateL2BStop_MASK),
                      (0x0 << L2_L2B_CK_GATE_CONTROL_CKGateL2BRegsDisable_OFFSET) |
                      (0x0 << L2_L2B_CK_GATE_CONTROL_CKGateL2BDynamicDisable_OFFSET) |
                      (0x0 << L2_L2B_CK_GATE_CONTROL_CKGateL2BMiscDisable_OFFSET) |
                      (0x0 << L2_L2B_CK_GATE_CONTROL_CKGateL2BCacheDisable_OFFSET) |
                      (0x1 << L2_L2B_CK_GATE_CONTROL_CKGateL2BLength_OFFSET) |
                      (0x1 << L2_L2B_CK_GATE_CONTROL_CKGateL2BStop_OFFSET), 
                      GNB_REG_ACC_FLAG_S3SAVE 
                    );                                       
      // Emei Only
      if (GnbHandle->DieType == IOD_EMEI) {
        // SDPMUX_GLUE_CG_LCLK_CTRL_0
        NbioRegisterRMW (GnbHandle,
                        SDPMUX_GLUE_CG_LCLK_CTRL_0_TYPE,
                        NBIO_SPACE(GnbHandle, SDPMUX_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYGX),
                        (UINT32) ~(NBMISC_4400_CG_OFF_HYSTERESIS_MASK |
                          NBMISC_4400_SOFT_OVERRIDE_CLK9_MASK |
                          NBMISC_4400_SOFT_OVERRIDE_CLK8_MASK |
                          NBMISC_4400_SOFT_OVERRIDE_CLK7_MASK |
                          NBMISC_4400_SOFT_OVERRIDE_CLK6_MASK |
                          NBMISC_4400_SOFT_OVERRIDE_CLK5_MASK |
                          NBMISC_4400_SOFT_OVERRIDE_CLK4_MASK |
                          NBMISC_4400_SOFT_OVERRIDE_CLK3_MASK |
                          NBMISC_4400_SOFT_OVERRIDE_CLK2_MASK |
                          NBMISC_4400_SOFT_OVERRIDE_CLK1_MASK |
                          NBMISC_4400_SOFT_OVERRIDE_CLK0_MASK),
                        (0x10 << NBMISC_4400_CG_OFF_HYSTERESIS_OFFSET) |
                        (0x0 << NBMISC_4400_SOFT_OVERRIDE_CLK9_OFFSET) |
                        (0x0 << NBMISC_4400_SOFT_OVERRIDE_CLK8_OFFSET) |
                        (0x0 << NBMISC_4400_SOFT_OVERRIDE_CLK7_OFFSET) |
                        (0x0 << NBMISC_4400_SOFT_OVERRIDE_CLK6_OFFSET) |
                        (0x0 << NBMISC_4400_SOFT_OVERRIDE_CLK5_OFFSET) |
                        (0x0 << NBMISC_4400_SOFT_OVERRIDE_CLK4_OFFSET) |
                        (0x0 << NBMISC_4400_SOFT_OVERRIDE_CLK3_OFFSET) |
                        (0x0 << NBMISC_4400_SOFT_OVERRIDE_CLK2_OFFSET) |
                        (0x0 << NBMISC_4400_SOFT_OVERRIDE_CLK1_OFFSET) |
                        (0x0 << NBMISC_4400_SOFT_OVERRIDE_CLK0_OFFSET), 
                        GNB_REG_ACC_FLAG_S3SAVE
                    );
        // SDPMUX_GLUE_CG_LCLK_CTRL_1
        NbioRegisterRMW (GnbHandle,
                        SDPMUX_GLUE_CG_LCLK_CTRL_1_TYPE,
                        NBIO_SPACE(GnbHandle, SDPMUX_GLUE_CG_LCLK_CTRL_1_ADDRESS_HYGX),
                        (UINT32) ~(NBMISC_4404_SOFT_OVERRIDE_CLK9_MASK |
                          NBMISC_4404_SOFT_OVERRIDE_CLK8_MASK |
                          NBMISC_4404_SOFT_OVERRIDE_CLK7_MASK |
                          NBMISC_4404_SOFT_OVERRIDE_CLK6_MASK |
                          NBMISC_4404_SOFT_OVERRIDE_CLK5_MASK |
                          NBMISC_4404_SOFT_OVERRIDE_CLK4_MASK |
                          NBMISC_4404_SOFT_OVERRIDE_CLK3_MASK |
                          NBMISC_4404_SOFT_OVERRIDE_CLK2_MASK |
                          NBMISC_4404_SOFT_OVERRIDE_CLK1_MASK |
                          NBMISC_4404_SOFT_OVERRIDE_CLK0_MASK),
                        (0x1 << NBMISC_4404_SOFT_OVERRIDE_CLK9_OFFSET) |
                        (0x0 << NBMISC_4404_SOFT_OVERRIDE_CLK8_OFFSET) |
                        (0x0 << NBMISC_4404_SOFT_OVERRIDE_CLK7_OFFSET) |
                        (0x0 << NBMISC_4404_SOFT_OVERRIDE_CLK6_OFFSET) |
                        (0x0 << NBMISC_4404_SOFT_OVERRIDE_CLK5_OFFSET) |
                        (0x0 << NBMISC_4404_SOFT_OVERRIDE_CLK4_OFFSET) |
                        (0x0 << NBMISC_4404_SOFT_OVERRIDE_CLK3_OFFSET) |
                        (0x0 << NBMISC_4404_SOFT_OVERRIDE_CLK2_OFFSET) |
                        (0x0 << NBMISC_4404_SOFT_OVERRIDE_CLK1_OFFSET) |
                        (0x0 << NBMISC_4404_SOFT_OVERRIDE_CLK0_OFFSET), 
                        GNB_REG_ACC_FLAG_S3SAVE
                    );                      
        // L1_CLKCNTRL_0_PCIE0              
        NbioRegisterRMW (GnbHandle,
                        L1_CLKCNTRL_0_TYPE,
                        NBIO_SPACE(GnbHandle, L1_CLKCNTRL_0_PCIE0_ADDRESS_HYGX),
                        (UINT32) ~(L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_MASK |
                          L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_MASK),
                        (0x1 << L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_OFFSET) |
                        (0x1 << L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_OFFSET) |
                        (0x0 << L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_OFFSET) |
                        (0x1 << L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_OFFSET) |
                        (0x1 << L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_OFFSET) |
                        (0x1 << L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_OFFSET) |
                        (0x1 << L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_OFFSET) |
                        (0x1 << L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_OFFSET) |
                        (0x1 << L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_OFFSET) |
                        (0x20 << L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_OFFSET) |
                        (UINT32)(0x1 << L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );
        // CPM_CONTROL_PCIECORE0              
        NbioRegisterRMW (GnbHandle,
                        CPM_CONTROL_TYPE,
                        NBIO_SPACE(GnbHandle, CPM_CONTROL_PCIECORE0_ADDRESS_HYGX),
                        (UINT32) ~(CPM_CONTROL_LCLK_DYN_GATE_ENABLE_MASK |
                          CPM_CONTROL_TXCLK_DYN_GATE_ENABLE_MASK |
                          CPM_CONTROL_TXCLK_PERM_GATE_ENABLE_MASK |
                          CPM_CONTROL_TXCLK_LCNT_GATE_ENABLE_MASK |
                          CPM_CONTROL_TXCLK_REGS_GATE_ENABLE_MASK |
                          CPM_CONTROL_TXCLK_PRBS_GATE_ENABLE_MASK |
                          CPM_CONTROL_REFCLK_REGS_GATE_ENABLE_MASK |
                          CPM_CONTROL_LCLK_DYN_GATE_LATENCY_MASK |
                          CPM_CONTROL_TXCLK_DYN_GATE_LATENCY_MASK |
                          CPM_CONTROL_TXCLK_PERM_GATE_LATENCY_MASK |
                          CPM_CONTROL_TXCLK_REGS_GATE_LATENCY_MASK |
                          CPM_CONTROL_REFCLK_REGS_GATE_LATENCY_MASK |
                          CPM_CONTROL_LCLK_GATE_TXCLK_FREE_MASK |
                          CPM_CONTROL_RCVR_DET_CLK_ENABLE_MASK |
                          CPM_CONTROL_FAST_TXCLK_LATENCY_MASK |
                          CPM_CONTROL_REFCLK_XSTCLK_ENABLE_MASK |
                          CPM_CONTROL_REFCLK_XSTCLK_LATENCY_MASK |
                          CPM_CONTROL_CLKREQb_UNGATE_TXCLK_ENABLE_MASK |
                          CPM_CONTROL_TXCLK_RXP_GATE_ENABLE_MASK |
                          CPM_CONTROL_TXCLK_PI_GATE_ENABLE_MASK |
                          CPM_CONTROL_SPARE_REGS_MASK),
                        (0x0 << CPM_CONTROL_LCLK_DYN_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_TXCLK_DYN_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_TXCLK_PERM_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_TXCLK_LCNT_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_TXCLK_REGS_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_TXCLK_PRBS_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_REFCLK_REGS_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_LCLK_DYN_GATE_LATENCY_OFFSET) |
                        (0x1 << CPM_CONTROL_TXCLK_DYN_GATE_LATENCY_OFFSET) |
                        (0x1 << CPM_CONTROL_TXCLK_PERM_GATE_LATENCY_OFFSET) | 
                        (0x1 << CPM_CONTROL_TXCLK_REGS_GATE_LATENCY_OFFSET) | 
                        (0x1 << CPM_CONTROL_REFCLK_REGS_GATE_LATENCY_OFFSET) |
                        (0x1 << CPM_CONTROL_LCLK_GATE_TXCLK_FREE_OFFSET) |
                        (0x1 << CPM_CONTROL_RCVR_DET_CLK_ENABLE_OFFSET) |
                        (0x0 << CPM_CONTROL_FAST_TXCLK_LATENCY_OFFSET) |
                        (0x0 << CPM_CONTROL_REFCLK_XSTCLK_ENABLE_OFFSET) |
                        (0x0 << CPM_CONTROL_REFCLK_XSTCLK_LATENCY_OFFSET) | 
                        (0x0 << CPM_CONTROL_CLKREQb_UNGATE_TXCLK_ENABLE_OFFSET) |
                        (0x3 << CPM_CONTROL_TXCLK_RXP_GATE_ENABLE_OFFSET) |
                        (0x3 << CPM_CONTROL_TXCLK_PI_GATE_ENABLE_OFFSET) |
                        (0x0 << CPM_CONTROL_SPARE_REGS_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );                    
        // CPM_CONTROL_2_PCIECORE0
        NbioRegisterRMW (GnbHandle,
                        CPM_CONTROL_2_TYPE,
                        NBIO_SPACE(GnbHandle, CPM_CONTROL_2_PCIECORE0_ADDRESS_HYGX),
                        (UINT32) ~(CPM_CONTROL_2_L1_PWR_GATE_ENABLE_MASK |
                          CPM_CONTROL_2_L1_1_PWR_GATE_ENABLE_MASK |
                          CPM_CONTROL_2_L1_2_PWR_GATE_ENABLE_MASK |
                          CPM_CONTROL_2_REFCLKREQ_REFCLKACK_LOOPBACK_ENABLE_MASK |
                          CPM_CONTROL_2_IGNORE_REGS_IDLE_IN_PG_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ALL_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_A_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_B_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_C_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_D_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_E_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_F_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_G_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_H_MASK |
                          CPM_CONTROL_2_LCLK_GATE_ALLOW_IN_L1_MASK |
                          CPM_CONTROL_2_PG_EARLY_WAKE_ENABLE_MASK |
                          CPM_CONTROL_2_PCIE_CORE_IDLE_MASK |
                          CPM_CONTROL_2_PCIE_LINK_IDLE_MASK |
                          CPM_CONTROL_2_PCIE_BUFFER_EMPTY_MASK |
                          CPM_CONTROL_2_REGS_IDLE_TO_PG_LATENCY_MASK),
                        (0x0 << CPM_CONTROL_2_L1_PWR_GATE_ENABLE_OFFSET) |
                        (0x0 << CPM_CONTROL_2_L1_1_PWR_GATE_ENABLE_OFFSET) |
                        (0x0 << CPM_CONTROL_2_L1_2_PWR_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_2_REFCLKREQ_REFCLKACK_LOOPBACK_ENABLE_OFFSET) |
                        (0x0 << CPM_CONTROL_2_IGNORE_REGS_IDLE_IN_PG_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ALL_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_A_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_B_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_C_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_D_OFFSET) | 
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_E_OFFSET) | 
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_F_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_G_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_H_OFFSET) |
                        (0x0 << CPM_CONTROL_2_LCLK_GATE_ALLOW_IN_L1_OFFSET) |
                        (0x0 << CPM_CONTROL_2_PG_EARLY_WAKE_ENABLE_OFFSET) |
                        (0x0 << CPM_CONTROL_2_PCIE_CORE_IDLE_OFFSET) | 
                        (0x0 << CPM_CONTROL_2_PCIE_LINK_IDLE_OFFSET) |
                        (0x0 << CPM_CONTROL_2_PCIE_BUFFER_EMPTY_OFFSET) |
                        (0x0 << CPM_CONTROL_2_REGS_IDLE_TO_PG_LATENCY_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      ); 
        // LC_CPM_CONTROL_0_PCIECORE0
        NbioRegisterRMW (GnbHandle,
                        LC_CPM_CONTROL_0_TYPE,
                        NBIO_SPACE(GnbHandle, LC_CPM_CONTROL_0_PCIECORE0_ADDRESS_HYGX),
                        (UINT32) ~(LC_CPM_CONTROL_0_TXCLK_DYN_PORT_A_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_PORT_B_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_PORT_C_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_PORT_D_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_PORT_E_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_PORT_F_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_PORT_G_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_PORT_H_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_A_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_B_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_C_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_D_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_E_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_F_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_G_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_H_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_A_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_B_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_C_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_D_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_E_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_F_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_G_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_H_GATE_ENABLE_MASK),
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_A_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_B_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_C_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_D_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_E_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_F_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_G_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_H_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_A_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_B_GATE_ENABLE_OFFSET) | 
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_C_GATE_ENABLE_OFFSET) | 
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_D_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_E_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_F_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_G_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_H_GATE_ENABLE_OFFSET) |
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_A_GATE_ENABLE_OFFSET) |
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_B_GATE_ENABLE_OFFSET) |
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_C_GATE_ENABLE_OFFSET) |
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_D_GATE_ENABLE_OFFSET) |                          
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_E_GATE_ENABLE_OFFSET) | 
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_F_GATE_ENABLE_OFFSET) |
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_G_GATE_ENABLE_OFFSET) |
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_H_GATE_ENABLE_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );
        // PCIE_CONFIG_CNTL_PCIECORE0
        NbioRegisterRMW (GnbHandle,
                        PCIE_CONFIG_CNTL_TYPE,
                        NBIO_SPACE(GnbHandle, PCIE_CONFIG_CNTL_PCIECORE0_ADDRESS_HYGX),
                        (UINT32) ~(PCIE_CONFIG_CNTL_DYN_CLK_LATENCY_MASK |
                          PCIE_CONFIG_CNTL_CI_SWUS_MAX_PAYLOAD_SIZE_MODE_MASK |
                          PCIE_CONFIG_CNTL_CI_SWUS_PRIV_MAX_PAYLOAD_SIZE_MASK |
                          PCIE_CONFIG_CNTL_CI_10BIT_TAG_EN_OVERRIDE_MASK |
                          PCIE_CONFIG_CNTL_CI_SWUS_10BIT_TAG_EN_OVERRIDE_MASK |
                          PCIE_CONFIG_CNTL_CI_MAX_PAYLOAD_SIZE_MODE_MASK |
                          PCIE_CONFIG_CNTL_CI_PRIV_MAX_PAYLOAD_SIZE_MASK |
                          PCIE_CONFIG_CNTL_CI_MAX_READ_REQUEST_SIZE_MODE_MASK |
                          PCIE_CONFIG_CNTL_CI_PRIV_MAX_READ_REQUEST_SIZE_MASK |
                          PCIE_CONFIG_CNTL_CI_MAX_READ_SAFE_MODE_MASK |
                          PCIE_CONFIG_CNTL_CI_EXTENDED_TAG_EN_OVERRIDE_MASK |
                          PCIE_CONFIG_CNTL_CI_SWUS_MAX_READ_REQUEST_SIZE_MODE_MASK |
                          PCIE_CONFIG_CNTL_CI_SWUS_MAX_READ_REQUEST_SIZE_PRIV_MASK |
                          PCIE_CONFIG_CNTL_CI_SWUS_EXTENDED_TAG_EN_OVERRIDE_MASK),
                        (0xF << PCIE_CONFIG_CNTL_DYN_CLK_LATENCY_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_SWUS_MAX_PAYLOAD_SIZE_MODE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_SWUS_PRIV_MAX_PAYLOAD_SIZE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_10BIT_TAG_EN_OVERRIDE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_SWUS_10BIT_TAG_EN_OVERRIDE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_MAX_PAYLOAD_SIZE_MODE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_PRIV_MAX_PAYLOAD_SIZE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_MAX_READ_REQUEST_SIZE_MODE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_PRIV_MAX_READ_REQUEST_SIZE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_MAX_READ_SAFE_MODE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_EXTENDED_TAG_EN_OVERRIDE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_SWUS_MAX_READ_REQUEST_SIZE_MODE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_SWUS_MAX_READ_REQUEST_SIZE_PRIV_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_SWUS_EXTENDED_TAG_EN_OVERRIDE_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );
        // PCIE_DEBUG_CNTL_PCIECORE0
        NbioRegisterRMW (GnbHandle,
                        PCIE_DEBUG_CNTL_TYPE,
                        NBIO_SPACE(GnbHandle, PCIE_DEBUG_CNTL_PCIECORE0_ADDRESS_HYGX),
                        (UINT32) ~(PCIE_DEBUG_CNTL_DEBUG_PORT_EN_MASK |
                          PCIE_DEBUG_CNTL_DEBUG_SELECT_MASK |
                          PCIE_DEBUG_CNTL_PCIE_DEBUG_LOGIC_DISABLE_MASK |
                          PCIE_DEBUG_CNTL_DEBUG_LANE_EN_MASK),
                        (0x1 << PCIE_DEBUG_CNTL_DEBUG_PORT_EN_OFFSET) |
                        (0x0 << PCIE_DEBUG_CNTL_DEBUG_SELECT_OFFSET) |
                        (0x1 << PCIE_DEBUG_CNTL_PCIE_DEBUG_LOGIC_DISABLE_OFFSET) |
                        (0x1 << PCIE_DEBUG_CNTL_DEBUG_LANE_EN_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );                                                                       

        // Full NBIO Only
        if (GnbHandle->RbId == 0 || GnbHandle->RbId == 2) {
          // L1_CLKCNTRL_0_PCIE1              
          NbioRegisterRMW (GnbHandle,
                          L1_CLKCNTRL_0_TYPE,
                          NBIO_SPACE(GnbHandle, L1_CLKCNTRL_0_PCIE1_ADDRESS_HYGX),
                          (UINT32) ~(L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_MASK |
                            L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_MASK),
                          (0x1 << L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_OFFSET) |
                          (0x0 << L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_OFFSET) |
                          (0x20 << L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_OFFSET) |
                          (UINT32)(0x1 << L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_OFFSET),
                          GNB_REG_ACC_FLAG_S3SAVE 
                        );
          // L1_CLKCNTRL_0_NBIF0              
          NbioRegisterRMW (GnbHandle,
                          L1_CLKCNTRL_0_TYPE,
                          NBIO_SPACE(GnbHandle, L1_CLKCNTRL_0_NBIF0_ADDRESS_HYGX),
                          (UINT32) ~(L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_MASK |
                            L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_MASK),
                          (0x1 << L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_OFFSET) |
                          (0x0 << L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_OFFSET) |
                          (0x20 << L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_OFFSET) |
                          (UINT32)(0x1 << L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_OFFSET), 
                          GNB_REG_ACC_FLAG_S3SAVE 
                        ); 
          // L1_CLKCNTRL_0_IOAGR              
          NbioRegisterRMW (GnbHandle,
                          L1_CLKCNTRL_0_TYPE,
                          NBIO_SPACE(GnbHandle, L1_CLKCNTRL_0_IOAGR_ADDRESS_HYGX),
                          (UINT32) ~(L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_MASK |
                            L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_MASK |
                            L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_MASK),
                          (0x1 << L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_OFFSET) |
                          (0x0 << L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_OFFSET) |
                          (0x1 << L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_OFFSET) |
                          (0x20 << L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_OFFSET) |
                          (UINT32)(0x1 << L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_OFFSET), 
                          GNB_REG_ACC_FLAG_S3SAVE 
                        );
          // CXL_CLK_CNTL_PCIECORE1
          NbioRegisterRMW (GnbHandle,
                          CXL_CLK_CNTL_PCIECORE1_TYPE,
                          NBIO_SPACE(GnbHandle, CXL_CLK_CNTL_PCIECORE1_ADDRESS),
                          (UINT32) ~(CXL_CLK_CNTL_LCLK_GATE_ENABLE_MASK |
                            CXL_CLK_CNTL_PCLK_GATE_ENABLE_MASK |
                            CXL_CLK_CNTL_FCLK_GATE_ENABLE_MASK |
                            CXL_CLK_CNTL_RADM_CLK_GATE_ENABLE_MASK |
                            CXL_CLK_CNTL_CXL_LINKDOWN_PCS_RST_EN_MASK |                            
                            CXL_CLK_CNTL_MAXPCLK_SEL_EN_MASK),
                          (0x0 << CXL_CLK_CNTL_LCLK_GATE_ENABLE_OFFSET) |
                          (0x1 << CXL_CLK_CNTL_PCLK_GATE_ENABLE_OFFSET) |
                          (0x1 << CXL_CLK_CNTL_FCLK_GATE_ENABLE_OFFSET) |
                          (0x1 << CXL_CLK_CNTL_RADM_CLK_GATE_ENABLE_OFFSET) |
                          (0x0 << CXL_CLK_CNTL_CXL_LINKDOWN_PCS_RST_EN_OFFSET) |
                          (0x0 << CXL_CLK_CNTL_MAXPCLK_SEL_EN_OFFSET),
                          GNB_REG_ACC_FLAG_S3SAVE 
                        ); 
          // NBIF0_MGCG_CTRL nBIF0
          NbioRegisterRMW (GnbHandle,
                          MGCG_CTRL_TYPE,
                          NBIO_SPACE(GnbHandle, NBIF0_MGCG_CTRL_ADDRESS_HYGX),
                          (UINT32) ~(MGCG_EN_MASK | MGCG_MODE_MASK | MGCG_HYSTERESIS_MASK),
                          (0x1 << MGCG_EN_OFFSET) |
                          (0x0 << MGCG_MODE_OFFSET) |
                          (0x20 << MGCG_HYSTERESIS_OFFSET),
                          GNB_REG_ACC_FLAG_S3SAVE 
                        );    
          // NBIF0_MGCG_CTRL nBIF1
          NbioRegisterRMW (GnbHandle,
                          MGCG_CTRL_TYPE,
                          NBIO_SPACE(GnbHandle, NBIF1_MGCG_CTRL_ADDRESS_HYGX),
                          (UINT32) ~(MGCG_EN_MASK | MGCG_MODE_MASK | MGCG_HYSTERESIS_MASK),
                          (0x1 << MGCG_EN_OFFSET) |
                          (0x0 << MGCG_MODE_OFFSET) |
                          (0x20 << MGCG_HYSTERESIS_OFFSET),
                          GNB_REG_ACC_FLAG_S3SAVE 
                        );    
          // NGDC_MGCG_CTRL SYSHUBMM0
          NbioRegisterRMW (GnbHandle,
                          MGCG_CTRL_TYPE,
                          NBIO_SPACE(GnbHandle, SYSHUBMM0_NGDC_MGCG_CTRL_ADDRESS_HYGX),
                          (UINT32) ~(MGCG_EN_MASK | MGCG_MODE_MASK | MGCG_HYSTERESIS_MASK),
                          (0x1 << MGCG_EN_OFFSET) |
                          (0x0 << MGCG_MODE_OFFSET) |
                          (0x20 << MGCG_HYSTERESIS_OFFSET),
                          GNB_REG_ACC_FLAG_S3SAVE 
                        );
          // NTB_CTRL
          NbioRegisterRMW (GnbHandle,
                          NTB_CTRL_TYPE,
                          NBIO_SPACE(GnbHandle, NTB_CTRL_ADDRESS_HYGX),
                          (UINT32) ~(NTB_CTRL_CLKOffHyst_MASK | 
                            NTB_CTRL_CLK_GATE_EN_MASK | 
                            NTB_CTRL_PMMReg_AccessPath_MASK | 
                            NTB_CTRL_SMMReg_AccessPath_MASK | 
                            NTB_CTRL_SMN_ERR_TYPE_MASK | 
                            NTB_CTRL_SMM_Reg_Ctrl_MASK | 
                            NTB_CTRL_PMM_Reg_Ctrl_MASK),
                          (0x0 << NTB_CTRL_CLKOffHyst_OFFSET) |
                          (0x1 << NTB_CTRL_CLK_GATE_EN_OFFSET) |
                          (0x0 << NTB_CTRL_PMMReg_AccessPath_OFFSET) |
                          (0x0 << NTB_CTRL_SMMReg_AccessPath_OFFSET) |
                          (0x0 << NTB_CTRL_SMN_ERR_TYPE_OFFSET) |
                          (0x0 << NTB_CTRL_SMM_Reg_Ctrl_OFFSET) |                                                    
                          (0x0 << NTB_CTRL_PMM_Reg_Ctrl_OFFSET),
                          GNB_REG_ACC_FLAG_S3SAVE 
                        );
          // NBIO0 Only
          if (GnbHandle->RbId == 0) {
            // FST_CLOCK_CTRL_FST0
            NbioRegisterRMW (GnbHandle,
                            FST_CLOCK_CTRL_FST0_TYPE,
                            NBIO_SPACE(GnbHandle, FST_CLOCK_CTRL_FST0_ADDRESS_HYGX),
                            (UINT32) ~(FST_CLOCK_CTRL_TXCLKGATEEn_MASK |
                              FST_CLOCK_CTRL_PCTRL_IDLE_TIME_MASK |
                                FST_CLOCK_CTRL_RXCLKGATEEn_MASK |
                                FST_CLOCK_CTRL_PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE_MASK |
                                FST_CLOCK_CTRL_PCTRL_USE_RX_IDLE_TIME_MASK |
                                FST_CLOCK_CTRL_PCTRL_RX_IDLE_TIME_MASK),
                              (0x1 << FST_CLOCK_CTRL_TXCLKGATEEn_OFFSET) |
                              (0x40 << FST_CLOCK_CTRL_PCTRL_IDLE_TIME_OFFSET) |
                              (0x1 << FST_CLOCK_CTRL_RXCLKGATEEn_OFFSET) |
                              (0x0 << FST_CLOCK_CTRL_PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE_OFFSET) |
                              (0x0 << FST_CLOCK_CTRL_PCTRL_USE_RX_IDLE_TIME_OFFSET) |
                              (0x0 << FST_CLOCK_CTRL_PCTRL_RX_IDLE_TIME_OFFSET), 
                            GNB_REG_ACC_FLAG_S3SAVE 
                          );            
          }                                                                                                                                                                                        
        }
      }

      // Dujiang Only
      if (GnbHandle->DieType == IOD_DUJIANG) {
        // IOAGR_GLUE_CG_LCLK_CTRL_0
        NbioRegisterRMW (GnbHandle,
                        IOAGR_GLUE_CG_LCLK_CTRL_0_TYPE,
                        NBIO_SPACE(GnbHandle, IOAGR_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYGX),
                        (UINT32) ~(NBMISC_0000_CG_OFF_HYSTERESIS_MASK |
                          NBMISC_0000_SOFT_OVERRIDE_CLK9_MASK |
                          NBMISC_0000_SOFT_OVERRIDE_CLK8_MASK |
                          NBMISC_0000_SOFT_OVERRIDE_CLK7_MASK |
                          NBMISC_0000_SOFT_OVERRIDE_CLK6_MASK |
                          NBMISC_0000_SOFT_OVERRIDE_CLK5_MASK |
                          NBMISC_0000_SOFT_OVERRIDE_CLK4_MASK |
                          NBMISC_0000_SOFT_OVERRIDE_CLK3_MASK |
                          NBMISC_0000_SOFT_OVERRIDE_CLK2_MASK |
                          NBMISC_0000_SOFT_OVERRIDE_CLK1_MASK |
                          NBMISC_0000_SOFT_OVERRIDE_CLK0_MASK),
                        (0x10 << NBMISC_0000_CG_OFF_HYSTERESIS_OFFSET) |
                        (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK9_OFFSET) |
                        (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK8_OFFSET) |
                        (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK7_OFFSET) |
                        (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK6_OFFSET) |
                        (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK5_OFFSET) |
                        (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK4_OFFSET) |
                        (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK3_OFFSET) |
                        (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK2_OFFSET) |
                        (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK1_OFFSET) |
                        (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK0_OFFSET), 
                        GNB_REG_ACC_FLAG_S3SAVE
                    );  
        // IOAGR_GLUE_CG_LCLK_CTRL_1
        NbioRegisterRMW (GnbHandle,
                        IOAGR_GLUE_CG_LCLK_CTRL_1_TYPE,
                        NBIO_SPACE(GnbHandle, IOAGR_GLUE_CG_LCLK_CTRL_1_ADDRESS_HYGX),
                        (UINT32) ~(NBMISC_0004_SOFT_OVERRIDE_CLK9_MASK |
                          NBMISC_0004_SOFT_OVERRIDE_CLK8_MASK |
                          NBMISC_0004_SOFT_OVERRIDE_CLK7_MASK |
                          NBMISC_0004_SOFT_OVERRIDE_CLK6_MASK |
                          NBMISC_0004_SOFT_OVERRIDE_CLK5_MASK |
                          NBMISC_0004_SOFT_OVERRIDE_CLK4_MASK |
                          NBMISC_0004_SOFT_OVERRIDE_CLK3_MASK |
                          NBMISC_0004_SOFT_OVERRIDE_CLK2_MASK |
                          NBMISC_0004_SOFT_OVERRIDE_CLK1_MASK |
                          NBMISC_0004_SOFT_OVERRIDE_CLK0_MASK),
                        (0x1 << NBMISC_0004_SOFT_OVERRIDE_CLK9_OFFSET) |
                        (0x1 << NBMISC_0004_SOFT_OVERRIDE_CLK8_OFFSET) |
                        (0x1 << NBMISC_0004_SOFT_OVERRIDE_CLK7_OFFSET) |
                        (0x0 << NBMISC_0004_SOFT_OVERRIDE_CLK6_OFFSET) |
                        (0x0 << NBMISC_0004_SOFT_OVERRIDE_CLK5_OFFSET) |
                        (0x0 << NBMISC_0004_SOFT_OVERRIDE_CLK4_OFFSET) |
                        (0x0 << NBMISC_0004_SOFT_OVERRIDE_CLK3_OFFSET) |
                        (0x0 << NBMISC_0004_SOFT_OVERRIDE_CLK2_OFFSET) |
                        (0x0 << NBMISC_0004_SOFT_OVERRIDE_CLK1_OFFSET) |
                        (0x0 << NBMISC_0004_SOFT_OVERRIDE_CLK0_OFFSET), 
                        GNB_REG_ACC_FLAG_S3SAVE
                    );
        // SST_CLOCK_CTRL_SST0
        NbioRegisterRMW (GnbHandle,
                        TYPE_SST0,
                        NBIO_SPACE(GnbHandle, SST_CLOCK_CTRL_SST0_ADDRESS_HYGX),
                        (UINT32) ~(SST_CLOCK_CTRL_TXCLKGATEEn_MASK |
                          SST_CLOCK_CTRL_PCTRL_IDLE_TIME_MASK |
                          SST_CLOCK_CTRL_RXCLKGATEEn_MASK |
                          SST_CLOCK_CTRL_RX_IDLE_IGONRE_RX_FIFO_EMPTY_MASK |
                          SST_CLOCK_CTRL_PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE_MASK |
                          SST_CLOCK_CTRL_PCTRL_USE_RX_IDLE_TIME_MASK |
                          SST_CLOCK_CTRL_PCTRL_RX_IDLE_TIME_MASK),
                        (0x1 << SST_CLOCK_CTRL_TXCLKGATEEn_OFFSET) |
                        (0xF0 << SST_CLOCK_CTRL_PCTRL_IDLE_TIME_OFFSET) |
                        (0x1 << SST_CLOCK_CTRL_RXCLKGATEEn_OFFSET) |
                        (0x0 << SST_CLOCK_CTRL_RX_IDLE_IGONRE_RX_FIFO_EMPTY_OFFSET) |
                        (0x0 << SST_CLOCK_CTRL_PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE_OFFSET) |
                        (0x0 << SST_CLOCK_CTRL_PCTRL_USE_RX_IDLE_TIME_OFFSET) |                        
                        (0x0 << SST_CLOCK_CTRL_PCTRL_RX_IDLE_TIME_OFFSET), 
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );
        // SST_CLOCK_CTRL_SST1
        NbioRegisterRMW (GnbHandle,
                        TYPE_SST1,
                        NBIO_SPACE(GnbHandle, SST_CLOCK_CTRL_SST1_ADDRESS_HYGX),
                        (UINT32) ~(SST_CLOCK_CTRL_TXCLKGATEEn_MASK |
                          SST_CLOCK_CTRL_PCTRL_IDLE_TIME_MASK |
                          SST_CLOCK_CTRL_RXCLKGATEEn_MASK |
                          SST_CLOCK_CTRL_RX_IDLE_IGONRE_RX_FIFO_EMPTY_MASK |
                          SST_CLOCK_CTRL_PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE_MASK |
                          SST_CLOCK_CTRL_PCTRL_USE_RX_IDLE_TIME_MASK |
                          SST_CLOCK_CTRL_PCTRL_RX_IDLE_TIME_MASK),
                        (0x1 << SST_CLOCK_CTRL_TXCLKGATEEn_OFFSET) |
                        (0xF0 << SST_CLOCK_CTRL_PCTRL_IDLE_TIME_OFFSET) |
                        (0x1 << SST_CLOCK_CTRL_RXCLKGATEEn_OFFSET) |
                        (0x0 << SST_CLOCK_CTRL_RX_IDLE_IGONRE_RX_FIFO_EMPTY_OFFSET) |
                        (0x0 << SST_CLOCK_CTRL_PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE_OFFSET) |
                        (0x0 << SST_CLOCK_CTRL_PCTRL_USE_RX_IDLE_TIME_OFFSET) |                        
                        (0x0 << SST_CLOCK_CTRL_PCTRL_RX_IDLE_TIME_OFFSET), 
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );
        // FST_CLOCK_CTRL_FST0
        NbioRegisterRMW (GnbHandle,
                        FST_CLOCK_CTRL_FST0_TYPE,
                        NBIO_SPACE(GnbHandle, FST_CLOCK_CTRL_FST0_ADDRESS_HYGX),
                        (UINT32) ~(FST_CLOCK_CTRL_TXCLKGATEEn_MASK |
                          FST_CLOCK_CTRL_PCTRL_IDLE_TIME_MASK |
                            FST_CLOCK_CTRL_RXCLKGATEEn_MASK |
                            FST_CLOCK_CTRL_PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE_MASK |
                            FST_CLOCK_CTRL_PCTRL_USE_RX_IDLE_TIME_MASK |
                            FST_CLOCK_CTRL_PCTRL_RX_IDLE_TIME_MASK),
                          (0x1 << FST_CLOCK_CTRL_TXCLKGATEEn_OFFSET) |
                          (0x40 << FST_CLOCK_CTRL_PCTRL_IDLE_TIME_OFFSET) |
                          (0x1 << FST_CLOCK_CTRL_RXCLKGATEEn_OFFSET) |
                          (0x0 << FST_CLOCK_CTRL_PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE_OFFSET) |
                          (0x0 << FST_CLOCK_CTRL_PCTRL_USE_RX_IDLE_TIME_OFFSET) |
                          (0x0 << FST_CLOCK_CTRL_PCTRL_RX_IDLE_TIME_OFFSET), 
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );                      
        // L1_CLKCNTRL_0_IOAGR              
        NbioRegisterRMW (GnbHandle,
                        L1_CLKCNTRL_0_TYPE,
                        NBIO_SPACE(GnbHandle, L1_CLKCNTRL_0_IOAGR_ADDRESS_HYGX),
                        (UINT32) ~(L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_MASK |
                          L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_MASK |
                          L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_MASK),
                        (0x1 << L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_OFFSET) |
                        (0x1 << L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_OFFSET) |
                        (0x0 << L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_OFFSET) |
                        (0x1 << L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_OFFSET) |
                        (0x1 << L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_OFFSET) |
                        (0x1 << L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_OFFSET) |
                        (0x1 << L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_OFFSET) |
                        (0x1 << L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_OFFSET) |
                        (0x1 << L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_OFFSET) |
                        (0x20 << L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_OFFSET) |
                        (UINT32)(0x1 << L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_OFFSET), 
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );   
        // CPM_CONTROL_PCIECORE2              
        NbioRegisterRMW (GnbHandle,
                        CPM_CONTROL_TYPE,
                        NBIO_SPACE(GnbHandle, CPM_CONTROL_PCIECORE2_ADDRESS_HYGX),
                        (UINT32) ~(CPM_CONTROL_LCLK_DYN_GATE_ENABLE_MASK |
                          CPM_CONTROL_TXCLK_DYN_GATE_ENABLE_MASK |
                          CPM_CONTROL_TXCLK_PERM_GATE_ENABLE_MASK |
                          CPM_CONTROL_TXCLK_LCNT_GATE_ENABLE_MASK |
                          CPM_CONTROL_TXCLK_REGS_GATE_ENABLE_MASK |
                          CPM_CONTROL_TXCLK_PRBS_GATE_ENABLE_MASK |
                          CPM_CONTROL_REFCLK_REGS_GATE_ENABLE_MASK |
                          CPM_CONTROL_LCLK_DYN_GATE_LATENCY_MASK |
                          CPM_CONTROL_TXCLK_DYN_GATE_LATENCY_MASK |
                          CPM_CONTROL_TXCLK_PERM_GATE_LATENCY_MASK |
                          CPM_CONTROL_TXCLK_REGS_GATE_LATENCY_MASK |
                          CPM_CONTROL_REFCLK_REGS_GATE_LATENCY_MASK |
                          CPM_CONTROL_LCLK_GATE_TXCLK_FREE_MASK |
                          CPM_CONTROL_RCVR_DET_CLK_ENABLE_MASK |
                          CPM_CONTROL_FAST_TXCLK_LATENCY_MASK |
                          CPM_CONTROL_REFCLK_XSTCLK_ENABLE_MASK |
                          CPM_CONTROL_REFCLK_XSTCLK_LATENCY_MASK |
                          CPM_CONTROL_CLKREQb_UNGATE_TXCLK_ENABLE_MASK |
                          CPM_CONTROL_TXCLK_RXP_GATE_ENABLE_MASK |
                          CPM_CONTROL_TXCLK_PI_GATE_ENABLE_MASK |
                          CPM_CONTROL_SPARE_REGS_MASK),
                        (0x0 << CPM_CONTROL_LCLK_DYN_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_TXCLK_DYN_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_TXCLK_PERM_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_TXCLK_LCNT_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_TXCLK_REGS_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_TXCLK_PRBS_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_REFCLK_REGS_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_LCLK_DYN_GATE_LATENCY_OFFSET) |
                        (0x1 << CPM_CONTROL_TXCLK_DYN_GATE_LATENCY_OFFSET) |
                        (0x1 << CPM_CONTROL_TXCLK_PERM_GATE_LATENCY_OFFSET) | 
                        (0x1 << CPM_CONTROL_TXCLK_REGS_GATE_LATENCY_OFFSET) |
                        (0x1 << CPM_CONTROL_REFCLK_REGS_GATE_LATENCY_OFFSET) |
                        (0x1 << CPM_CONTROL_LCLK_GATE_TXCLK_FREE_OFFSET) |
                        (0x1 << CPM_CONTROL_RCVR_DET_CLK_ENABLE_OFFSET) |
                        (0x0 << CPM_CONTROL_FAST_TXCLK_LATENCY_OFFSET) |
                        (0x0 << CPM_CONTROL_REFCLK_XSTCLK_ENABLE_OFFSET) |
                        (0x0 << CPM_CONTROL_REFCLK_XSTCLK_LATENCY_OFFSET) | 
                        (0x0 << CPM_CONTROL_CLKREQb_UNGATE_TXCLK_ENABLE_OFFSET) |
                        (0x3 << CPM_CONTROL_TXCLK_RXP_GATE_ENABLE_OFFSET) |
                        (0x3 << CPM_CONTROL_TXCLK_PI_GATE_ENABLE_OFFSET) |
                        (0x0 << CPM_CONTROL_SPARE_REGS_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );  
        // CPM_CONTROL_2_PCIECORE2
        NbioRegisterRMW (GnbHandle,
                        CPM_CONTROL_2_TYPE,
                        NBIO_SPACE(GnbHandle, CPM_CONTROL_2_PCIECORE2_ADDRESS_HYGX),
                        (UINT32) ~(CPM_CONTROL_2_L1_PWR_GATE_ENABLE_MASK |
                          CPM_CONTROL_2_L1_1_PWR_GATE_ENABLE_MASK |
                          CPM_CONTROL_2_L1_2_PWR_GATE_ENABLE_MASK |
                          CPM_CONTROL_2_REFCLKREQ_REFCLKACK_LOOPBACK_ENABLE_MASK |
                          CPM_CONTROL_2_IGNORE_REGS_IDLE_IN_PG_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ALL_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_A_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_B_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_C_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_D_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_E_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_F_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_G_MASK |
                          CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_H_MASK |
                          CPM_CONTROL_2_LCLK_GATE_ALLOW_IN_L1_MASK |
                          CPM_CONTROL_2_PG_EARLY_WAKE_ENABLE_MASK |
                          CPM_CONTROL_2_PCIE_CORE_IDLE_MASK |
                          CPM_CONTROL_2_PCIE_LINK_IDLE_MASK |
                          CPM_CONTROL_2_PCIE_BUFFER_EMPTY_MASK |
                          CPM_CONTROL_2_REGS_IDLE_TO_PG_LATENCY_MASK),
                        (0x0 << CPM_CONTROL_2_L1_PWR_GATE_ENABLE_OFFSET) |
                        (0x0 << CPM_CONTROL_2_L1_1_PWR_GATE_ENABLE_OFFSET) |
                        (0x0 << CPM_CONTROL_2_L1_2_PWR_GATE_ENABLE_OFFSET) |
                        (0x1 << CPM_CONTROL_2_REFCLKREQ_REFCLKACK_LOOPBACK_ENABLE_OFFSET) |
                        (0x0 << CPM_CONTROL_2_IGNORE_REGS_IDLE_IN_PG_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ALL_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_A_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_B_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_C_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_D_OFFSET) | 
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_E_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_F_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_G_OFFSET) |
                        (0x1 << CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_H_OFFSET) |
                        (0x0 << CPM_CONTROL_2_LCLK_GATE_ALLOW_IN_L1_OFFSET) |
                        (0x0 << CPM_CONTROL_2_PG_EARLY_WAKE_ENABLE_OFFSET) |
                        (0x0 << CPM_CONTROL_2_PCIE_CORE_IDLE_OFFSET) | 
                        (0x0 << CPM_CONTROL_2_PCIE_LINK_IDLE_OFFSET) |
                        (0x0 << CPM_CONTROL_2_PCIE_BUFFER_EMPTY_OFFSET) |
                        (0x0 << CPM_CONTROL_2_REGS_IDLE_TO_PG_LATENCY_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );
        // LC_CPM_CONTROL_0_PCIECORE2
        NbioRegisterRMW (GnbHandle,
                        LC_CPM_CONTROL_0_TYPE,
                        NBIO_SPACE(GnbHandle, LC_CPM_CONTROL_0_PCIECORE2_ADDRESS_HYGX),
                        (UINT32) ~(LC_CPM_CONTROL_0_TXCLK_DYN_PORT_A_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_PORT_B_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_PORT_C_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_PORT_D_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_PORT_E_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_PORT_F_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_PORT_G_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_PORT_H_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_A_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_B_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_C_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_D_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_E_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_F_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_G_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_H_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_A_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_B_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_C_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_D_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_E_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_F_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_G_GATE_ENABLE_MASK |
                          LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_H_GATE_ENABLE_MASK),
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_A_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_B_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_C_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_D_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_E_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_F_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_G_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_PORT_H_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_A_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_B_GATE_ENABLE_OFFSET) | 
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_C_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_D_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_E_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_F_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_G_GATE_ENABLE_OFFSET) |
                        (0x1 << LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_H_GATE_ENABLE_OFFSET) |
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_A_GATE_ENABLE_OFFSET) |
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_B_GATE_ENABLE_OFFSET) |
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_C_GATE_ENABLE_OFFSET) |
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_D_GATE_ENABLE_OFFSET) |                          
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_E_GATE_ENABLE_OFFSET) | 
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_F_GATE_ENABLE_OFFSET) |
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_G_GATE_ENABLE_OFFSET) |
                        (0x0 << LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_H_GATE_ENABLE_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );  
        // PCIE_CONFIG_CNTL_PCIECORE2
        NbioRegisterRMW (GnbHandle,
                        PCIE_CONFIG_CNTL_TYPE,
                        NBIO_SPACE(GnbHandle, PCIE_CONFIG_CNTL_PCIECORE2_ADDRESS_HYGX),
                        (UINT32) ~(PCIE_CONFIG_CNTL_DYN_CLK_LATENCY_MASK |
                          PCIE_CONFIG_CNTL_CI_SWUS_MAX_PAYLOAD_SIZE_MODE_MASK |
                          PCIE_CONFIG_CNTL_CI_SWUS_PRIV_MAX_PAYLOAD_SIZE_MASK |
                          PCIE_CONFIG_CNTL_CI_10BIT_TAG_EN_OVERRIDE_MASK |
                          PCIE_CONFIG_CNTL_CI_SWUS_10BIT_TAG_EN_OVERRIDE_MASK |
                          PCIE_CONFIG_CNTL_CI_MAX_PAYLOAD_SIZE_MODE_MASK |
                          PCIE_CONFIG_CNTL_CI_PRIV_MAX_PAYLOAD_SIZE_MASK |
                          PCIE_CONFIG_CNTL_CI_MAX_READ_REQUEST_SIZE_MODE_MASK |
                          PCIE_CONFIG_CNTL_CI_PRIV_MAX_READ_REQUEST_SIZE_MASK |
                          PCIE_CONFIG_CNTL_CI_MAX_READ_SAFE_MODE_MASK |
                          PCIE_CONFIG_CNTL_CI_EXTENDED_TAG_EN_OVERRIDE_MASK |
                          PCIE_CONFIG_CNTL_CI_SWUS_MAX_READ_REQUEST_SIZE_MODE_MASK |
                          PCIE_CONFIG_CNTL_CI_SWUS_MAX_READ_REQUEST_SIZE_PRIV_MASK |
                          PCIE_CONFIG_CNTL_CI_SWUS_EXTENDED_TAG_EN_OVERRIDE_MASK),
                        (0xF << PCIE_CONFIG_CNTL_DYN_CLK_LATENCY_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_SWUS_MAX_PAYLOAD_SIZE_MODE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_SWUS_PRIV_MAX_PAYLOAD_SIZE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_10BIT_TAG_EN_OVERRIDE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_SWUS_10BIT_TAG_EN_OVERRIDE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_MAX_PAYLOAD_SIZE_MODE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_PRIV_MAX_PAYLOAD_SIZE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_MAX_READ_REQUEST_SIZE_MODE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_PRIV_MAX_READ_REQUEST_SIZE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_MAX_READ_SAFE_MODE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_EXTENDED_TAG_EN_OVERRIDE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_SWUS_MAX_READ_REQUEST_SIZE_MODE_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_SWUS_MAX_READ_REQUEST_SIZE_PRIV_OFFSET) |
                        (0x0 << PCIE_CONFIG_CNTL_CI_SWUS_EXTENDED_TAG_EN_OVERRIDE_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );
        // PCIE_DEBUG_CNTL_PCIECORE2
        NbioRegisterRMW (GnbHandle,
                        PCIE_DEBUG_CNTL_TYPE,
                        NBIO_SPACE(GnbHandle, PCIE_DEBUG_CNTL_PCIECORE2_ADDRESS_HYGX),
                        (UINT32) ~(PCIE_DEBUG_CNTL_DEBUG_PORT_EN_MASK |
                          PCIE_DEBUG_CNTL_DEBUG_SELECT_MASK |
                          PCIE_DEBUG_CNTL_PCIE_DEBUG_LOGIC_DISABLE_MASK |
                          PCIE_DEBUG_CNTL_DEBUG_LANE_EN_MASK),
                        (0x1 << PCIE_DEBUG_CNTL_DEBUG_PORT_EN_OFFSET) |
                        (0x0 << PCIE_DEBUG_CNTL_DEBUG_SELECT_OFFSET) |
                        (0x1 << PCIE_DEBUG_CNTL_PCIE_DEBUG_LOGIC_DISABLE_OFFSET) |
                        (0x1 << PCIE_DEBUG_CNTL_DEBUG_LANE_EN_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );
        // NBIF0_MGCG_CTRL nBIF2
        NbioRegisterRMW (GnbHandle,
                        MGCG_CTRL_TYPE,
                        NBIO_SPACE(GnbHandle, NBIF2_MGCG_CTRL_ADDRESS_HYGX),
                        (UINT32) ~(MGCG_EN_MASK | MGCG_MODE_MASK | MGCG_HYSTERESIS_MASK),
                        (0x1 << MGCG_EN_OFFSET) |
                        (0x0 << MGCG_MODE_OFFSET) |
                        (0x20 << MGCG_HYSTERESIS_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );    
        // NBIF0_MGCG_CTRL nBIF3
        NbioRegisterRMW (GnbHandle,
                        MGCG_CTRL_TYPE,
                        NBIO_SPACE(GnbHandle, NBIF3_MGCG_CTRL_ADDRESS_HYGX),
                        (UINT32) ~(MGCG_EN_MASK | MGCG_MODE_MASK | MGCG_HYSTERESIS_MASK),
                        (0x1 << MGCG_EN_OFFSET) |
                        (0x0 << MGCG_MODE_OFFSET) |
                        (0x20 << MGCG_HYSTERESIS_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );    
        // NGDC_MGCG_CTRL SYSHUBMM2
        NbioRegisterRMW (GnbHandle,
                        MGCG_CTRL_TYPE,
                        NBIO_SPACE(GnbHandle, SYSHUBMM2_NGDC_MGCG_CTRL_ADDRESS_HYGX),
                        (UINT32) ~(MGCG_EN_MASK | MGCG_MODE_MASK | MGCG_HYSTERESIS_MASK),
                        (0x1 << MGCG_EN_OFFSET) |
                        (0x0 << MGCG_MODE_OFFSET) |
                        (0x20 << MGCG_HYSTERESIS_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );      
        // NGDC_MGCG_CTRL SYSHUBMM3
        NbioRegisterRMW (GnbHandle,
                        MGCG_CTRL_TYPE,
                        NBIO_SPACE(GnbHandle, SYSHUBMM3_NGDC_MGCG_CTRL_ADDRESS_HYGX),
                        (UINT32) ~(MGCG_EN_MASK | MGCG_MODE_MASK | MGCG_HYSTERESIS_MASK),
                        (0x1 << MGCG_EN_OFFSET) |
                        (0x0 << MGCG_MODE_OFFSET) |
                        (0x20 << MGCG_HYSTERESIS_OFFSET),
                        GNB_REG_ACC_FLAG_S3SAVE 
                      );                                                                                                                                                                                                                                                                                    
      }

      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioClkGattingConfig Exit\n");

  return;
}

/**
 *---------------------------------------------------------------------------------------
 *
 * NBIO Base driver entry.
 *
 *
 *
 * @param[in]     ImageHandle
 * @param[in]     *SystemTable
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS
EFIAPI
HygonNbioBaseHyGxDxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status;
  EFI_HANDLE          Handle;
  EFI_EVENT           PciIoEvent;
  VOID                *Registration;
  EFI_EVENT           ReadyToBootEvent;

  HGPI_TESTPOINT (TpNbioBaseDxeEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioBaseHyGxDxeEntry\n");
  Handle = NULL;

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonNbioDebugServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mDxeHygonNbioBaseServicesProtocol
                  );

  //
  // Register the event handling function
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  NbioBaseHookPciIO,
                  NULL,
                  NULL,
                  &PciIoEvent
                  );

  Status = gBS->RegisterProtocolNotify (
                  &gEfiPciIoProtocolGuid,
                  PciIoEvent,
                  &Registration
                  );

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  NbioClkGatingConfig,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &ReadyToBootEvent
                  );
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioBaseHyGxDxe Exit\n");
  HGPI_TESTPOINT (TpNbioBaseDxeExit, NULL);
  return EFI_SUCCESS;
}
