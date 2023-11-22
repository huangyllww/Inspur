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

#define FILECODE NBIO_NBIOBASE_HYEX_HYGONNBIOBASEDXE_HYGONNBIOBASEDXE_FILECODE

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
  NbioRegisterRMW (GnbHandle,
                   DN_PCIE_CNTL_TYPE,
                   NBIO_SPACE(GnbHandle, NBIF0_DN_PCIE_CNTL_SMN_ADDRESS_HYEX),
                   (UINT32) ~(DN_PCIE_CNTL_HWINIT_WR_LOCK_MASK),
                   1 << DN_PCIE_CNTL_HWINIT_WR_LOCK_OFFSET,
                   GNB_REG_ACC_FLAG_S3SAVE
                   );
  NbioRegisterRMW (GnbHandle,
                   DN_PCIE_CNTL_TYPE,
                   NBIO_SPACE(GnbHandle, NBIF1_DN_PCIE_CNTL_SMN_ADDRESS_HYEX),
                   (UINT32) ~(DN_PCIE_CNTL_HWINIT_WR_LOCK_MASK),
                   1 << DN_PCIE_CNTL_HWINIT_WR_LOCK_OFFSET,
                   GNB_REG_ACC_FLAG_S3SAVE
                   );
  NbioRegisterRMW (GnbHandle,
                   DN_PCIE_CNTL_TYPE,
                   NBIO_SPACE(GnbHandle, NBIF2_DN_PCIE_CNTL_SMN_ADDRESS_HYEX),
                   (UINT32) ~(DN_PCIE_CNTL_HWINIT_WR_LOCK_MASK),
                   1 << DN_PCIE_CNTL_HWINIT_WR_LOCK_OFFSET,
                   GNB_REG_ACC_FLAG_S3SAVE
                   );
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
NbioClkGattingConfig (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{

  EFI_STATUS                          Status;
  PCIe_PLATFORM_CONFIG                *Pcie;
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB       *PciePlatformConfigHobData;
  GNB_HANDLE                          *GnbHandle;

  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioClkGattingConfig Entry\n");
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
  
      NbioRegisterRMW (GnbHandle,
                      TYPE_SMN,
                      NBIO_SPACE(GnbHandle, IOHC_GLUE_CG_LCLK_CTRL_1_ADDRESS_HYEX),
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
      NbioRegisterRMW (GnbHandle,
                      TYPE_SMN,
                      NBIO_SPACE(GnbHandle, IOHC_GLUE_CG_LCLK_CTRL_2_ADDRESS_HYEX),
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
      NbioRegisterRMW (GnbHandle,
                      TYPE_SMN,
                      SDPMUX_SPACE_HYEX(GnbHandle, SDPMUX_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYEX),
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
      NbioRegisterRMW (GnbHandle,
                      TYPE_SMN,
                      SDPMUX_SPACE_HYEX(GnbHandle, SDPMUX_GLUE_CG_LCLK_CTRL_1_ADDRESS_HYEX),
                      (UINT32) ~(NBMISC_0100_SOFT_OVERRIDE_CLK9_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK8_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK7_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK6_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK5_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK4_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK3_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK2_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK1_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK0_MASK), 
                      (0x1 << NBMISC_0100_SOFT_OVERRIDE_CLK9_OFFSET) |
                      (0x1 << NBMISC_0100_SOFT_OVERRIDE_CLK8_OFFSET) |
                      (0x1 << NBMISC_0100_SOFT_OVERRIDE_CLK7_OFFSET) |
                      (0x1 << NBMISC_0100_SOFT_OVERRIDE_CLK6_OFFSET) |
                      (0x1 << NBMISC_0100_SOFT_OVERRIDE_CLK5_OFFSET) | 
                      (0x0 << NBMISC_0100_SOFT_OVERRIDE_CLK4_OFFSET) |
                      (0x0 << NBMISC_0100_SOFT_OVERRIDE_CLK3_OFFSET) |
                      (0x0 << NBMISC_0100_SOFT_OVERRIDE_CLK2_OFFSET) |
                      (0x0 << NBMISC_0100_SOFT_OVERRIDE_CLK1_OFFSET) |
                      (0x0 << NBMISC_0100_SOFT_OVERRIDE_CLK0_OFFSET), 
                      GNB_REG_ACC_FLAG_S3SAVE 
                   );
      NbioRegisterRMW (GnbHandle,
                      TYPE_SMN,
                      NBIO_SPACE(GnbHandle, IOAGR_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYEX),
                      (UINT32) ~(NBMISC_0000_SOFT_OVERRIDE_CLK9_MASK |
                        NBMISC_0000_SOFT_OVERRIDE_CLK8_MASK |
                        NBMISC_0000_SOFT_OVERRIDE_CLK7_MASK |
                        NBMISC_0000_SOFT_OVERRIDE_CLK6_MASK |
                        NBMISC_0000_SOFT_OVERRIDE_CLK5_MASK |
                        NBMISC_0000_SOFT_OVERRIDE_CLK4_MASK |
                        NBMISC_0000_SOFT_OVERRIDE_CLK3_MASK |
                        NBMISC_0000_SOFT_OVERRIDE_CLK2_MASK |
                        NBMISC_0000_SOFT_OVERRIDE_CLK1_MASK |
                        NBMISC_0000_SOFT_OVERRIDE_CLK0_MASK |
                        NBMISC_0000_CG_OFF_HYSTERESIS_MASK),
                      (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK9_OFFSET) |
                      (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK8_OFFSET) |
                      (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK7_OFFSET) |
                      (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK6_OFFSET) |
                      (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK5_OFFSET) |
                      (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK4_OFFSET) |
                      (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK3_OFFSET) |
                      (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK2_OFFSET) |
                      (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK1_OFFSET) |
                      (0x0 << NBMISC_0000_SOFT_OVERRIDE_CLK0_OFFSET) |
                      (0x10 << NBMISC_0000_CG_OFF_HYSTERESIS_OFFSET), 
                      GNB_REG_ACC_FLAG_S3SAVE 
                   );
      NbioRegisterRMW (GnbHandle,
                      TYPE_SMN,
                      NBIO_SPACE(GnbHandle, IOAGR_GLUE_CG_LCLK_CTRL_1_ADDRESS_HYEX),
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
                      (0x1 << NBMISC_0004_SOFT_OVERRIDE_CLK6_OFFSET) |
                      (0x1 << NBMISC_0004_SOFT_OVERRIDE_CLK5_OFFSET) |
                      (0x1 << NBMISC_0004_SOFT_OVERRIDE_CLK4_OFFSET) |
                      (0x1 << NBMISC_0004_SOFT_OVERRIDE_CLK3_OFFSET) |
                      (0x0 << NBMISC_0004_SOFT_OVERRIDE_CLK2_OFFSET) |
                      (0x0 << NBMISC_0004_SOFT_OVERRIDE_CLK1_OFFSET) |
                      (0x0 << NBMISC_0004_SOFT_OVERRIDE_CLK0_OFFSET), 
                      GNB_REG_ACC_FLAG_S3SAVE 
                    );
      NbioRegisterRMW (GnbHandle,
                      TYPE_SMN,
                      NBIO_SPACE(GnbHandle, IOAPIC_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYEX),
                      (UINT32) ~(NBMISC_0100_SOFT_OVERRIDE_CLK9_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK8_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK7_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK6_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK5_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK4_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK3_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK2_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK1_MASK |
                        NBMISC_0100_SOFT_OVERRIDE_CLK0_MASK),
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
      NbioRegisterRMW (GnbHandle,
                      TYPE_SMN,
                      NBIO_SPACE(GnbHandle, SST_CLOCK_CTRL_SST0_ADDRESS_HYEX),
                      (UINT32) ~(SST_CLOCK_CTRL_TXCLKGATEEn_MASK |
                        SST_CLOCK_CTRL_PCTRL_IDLE_TIME_MASK |
                        SST_CLOCK_CTRL_RXCLKGATEEn_MASK),
                      (0x1 << SST_CLOCK_CTRL_TXCLKGATEEn_OFFSET) |
                      (0xF0  << SST_CLOCK_CTRL_PCTRL_IDLE_TIME_OFFSET) |
                      (0x1 << SST_CLOCK_CTRL_RXCLKGATEEn_OFFSET), 
                      GNB_REG_ACC_FLAG_S3SAVE 
                    );
      NbioRegisterRMW (GnbHandle,
                      TYPE_SMN,
                      NBIO_SPACE(GnbHandle, SST_CLOCK_CTRL_SST1_ADDRESS_HYEX),
                      (UINT32) ~(SST_CLOCK_CTRL_TXCLKGATEEn_MASK |
                        SST_CLOCK_CTRL_PCTRL_IDLE_TIME_MASK |
                        SST_CLOCK_CTRL_RXCLKGATEEn_MASK),
                      (0x1 << SST_CLOCK_CTRL_TXCLKGATEEn_OFFSET) |
                      (0xF0  << SST_CLOCK_CTRL_PCTRL_IDLE_TIME_OFFSET) |
                      (0x1 << SST_CLOCK_CTRL_RXCLKGATEEn_OFFSET), 
                      GNB_REG_ACC_FLAG_S3SAVE 
                    );

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
HygonNbioBaseHyExDxeEntry (
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
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioBaseHyExDxeEntry Entry\n");
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
                NbioClkGattingConfig,
                NULL,
                &gEfiEventReadyToBootGuid,
                &ReadyToBootEvent
                );

  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioBaseHyExDxeEntry Exit\n");
  HGPI_TESTPOINT (TpNbioBaseDxeExit, NULL);
  return EFI_SUCCESS;
}
