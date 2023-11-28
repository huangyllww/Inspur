/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe link ASPM L1 SS
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  <GnbDxio.h>
#include  <OptionGnb.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/AmdBaseLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/PcieMiscCommLib.h>
#include  <GnbRegistersZP.h>
#include  <Library/NbioRegisterAccLib.h>

#define FILECODE NBIO_PCIE_AMDNBIOPCIEZPDXE_AMDNBIOPCIECLKREQZP_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
typedef struct {
  GNB_PCI_SCAN_DATA       ScanData;
  PCI_ADDR                DownstreamPort;
} PCIE_CLKREQ_PMM_DATA;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                     L O C A L   D A T A   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

UINT8 ROMDATA DefaultClkReqMapCntlZP [5][2] = {
  {0xFF, 0},    // CLKREQB_0
  {0xFF, 1},    // CLKREQB_1
  {0xFF, 2},    // CLKREQB_2
  {0xFF, 3},    // CLKREQB_3
  {0xFF, 4},    // CLKREQB_4
};

VOID
UpdateClkReqMapCntl ()
{
  UINT8                 Index;
  UINT8                 Index1;
  for (Index = 0; Index < 5; Index++) {
    IDS_HDT_CONSOLE (GNB_TRACE, "CLKREQB_%d: 0x%x - %d\n", Index, DefaultClkReqMapCntlZP[Index][0], DefaultClkReqMapCntlZP[Index][1]);
    if (DefaultClkReqMapCntlZP[Index][0] != 0xFF) {
      for (Index1 = 0; Index1 < 5; Index1++) {
        if (DefaultClkReqMapCntlZP[Index1][1] == DefaultClkReqMapCntlZP[Index][0]) {
          DefaultClkReqMapCntlZP[Index1][1] = DefaultClkReqMapCntlZP[Index][1];
        }
      }
      DefaultClkReqMapCntlZP[Index][1] = DefaultClkReqMapCntlZP[Index][0];
      DefaultClkReqMapCntlZP[Index][0] = 0xFF;
    }
  }
  for (Index = 0; Index < 5; Index++) {
    IDS_HDT_CONSOLE (GNB_TRACE, "CLKREQB_%d: 0x%x - %d\n", Index, DefaultClkReqMapCntlZP[Index][0], DefaultClkReqMapCntlZP[Index][1]);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set CLOCK_POWER_MANAGEMENT_EN on PCIe device function
 *
 *
 *
 * @param[in] AddressValue    PCI address of function.
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieClkReqPmmEnable (
  IN       UINT32            AddressValue
  )
{
  UINT8       PcieCapPtr;
  PcieCapPtr = GnbLibFindPciCapability (AddressValue, PCIE_CAP_ID, NULL);
  if (PcieCapPtr != 0) {
    GnbLibPciRMW (
      AddressValue | (PcieCapPtr + PCIE_LINK_CTRL_REGISTER) ,
      AccessS3SaveWidth32,
      ~(UINT32)BIT8,
      BIT8,
      NULL
      );
  }
}

SCAN_STATUS
SetPcieClkReqPmmEnableCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS             ScanStatus;
  PCIE_CLKREQ_PMM_DATA    *PcieClkReqPmmData;
  PCIE_DEVICE_TYPE        DeviceType;

  ScanStatus = SCAN_SUCCESS;
  PcieClkReqPmmData = (PCIE_CLKREQ_PMM_DATA *) ScanData;
  IDS_HDT_CONSOLE (GNB_TRACE, "  SetPcieClkReqPmmEnableCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    IDS_HDT_CONSOLE (GNB_TRACE, "  PcieDeviceRootComplex/PcieDeviceDownstreamPort/PcieDeviceUpstreamPort %d\n", DeviceType);
    GnbLibPciScanSecondaryBus (Device, &PcieClkReqPmmData->ScanData);
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    IDS_HDT_CONSOLE (GNB_TRACE, "  PcieDeviceEndPoint %d\n", DeviceType);
    PcieClkReqPmmEnable (Device.AddressValue);
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  default:
    IDS_HDT_CONSOLE (GNB_TRACE, "  Unknow DeviceType %d\n", DeviceType);
    break;
  }
  return ScanStatus;
}

VOID
PcieClkReqPmmEnableCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  PCIE_CLKREQ_PMM_DATA PcieClkReqPmmData;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieClkReqPmmEnableCallback Enter\n");
  PcieClkReqPmmData.ScanData.StdHeader = NULL;
  PcieClkReqPmmData.ScanData.GnbScanCallback = SetPcieClkReqPmmEnableCallback;
  GnbLibPciScan (Engine->Type.Port.Address, Engine->Type.Port.Address, &PcieClkReqPmmData.ScanData);

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieClkReqPmmEnableCallback Exit\n");
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to enable PM L1 SS
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieClkReqInterface (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG             *PcieEngine;
  PCIe_WRAPPER_CONFIG            *PcieWrapper;
  BOOLEAN                        Pcie0Cntl;
  BOOLEAN                        Pcie1Cntl;
  UINT32                         Pcie0MapCounter;
  UINT32                         Pcie1MapCounter;
  NBIO_CLKREQb_MAP_CNTL_STRUCT   NBIO_CLKREQb_MAP_CNTL0;
  NBIO_CLKREQb_MAP_CNTL_STRUCT   NBIO_CLKREQb_MAP_CNTL1;
  UINT32                         Address;
  UINT32                         Index;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieClkReqInterface Enter\n");
  Pcie0Cntl = FALSE;
  Pcie1Cntl = FALSE;
  Pcie0MapCounter = 0;
  Pcie1MapCounter = 0;
  Address = SMN_0x11140280_ADDRESS;
  Index = 0;

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if ((PcieLibIsEngineAllocated (PcieEngine)) &&
          (PcieConfigCheckPortStatus (PcieEngine, INIT_STATUS_PCIE_TRAINING_SUCCESS))) {
        if (PcieEngine->Type.Port.PortData.MiscControls.ClkPmSupport == 1) {
          PcieClkReqPmmEnableCallback (PcieEngine, NULL);
        }
        if ((PcieEngine->Type.Port.LinkAspmL1_1 == 1) || (PcieEngine->Type.Port.LinkAspmL1_2 == 1)) {
          if (PcieEngine->Type.Port.ClkReq != 0) {
            if (PcieEngine->Type.Port.CoreId == 1) {
              Pcie1Cntl = TRUE;
              if (Pcie1MapCounter < 5) {
                DefaultClkReqMapCntlZP[PcieEngine->Type.Port.ClkReq - 1][0] = PcieEngine->Type.Port.PortId - 8;
              }
              Pcie1MapCounter++;
            } else if (PcieEngine->Type.Port.CoreId == 0) {
              Pcie0Cntl = TRUE;
              if (Pcie0MapCounter < 5) {
                DefaultClkReqMapCntlZP[PcieEngine->Type.Port.ClkReq - 1][0] = PcieEngine->Type.Port.PortId;
              }
              Pcie0MapCounter++;
            }
          }
        }
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  UpdateClkReqMapCntl (DefaultClkReqMapCntlZP[0][0]);

  if (Pcie0Cntl != Pcie1Cntl) {
    NbioRegisterRead ( GnbHandle, TYPE_SMN, SMN_0x11180190, &NBIO_CLKREQb_MAP_CNTL0, 0);
    NbioRegisterRead ( GnbHandle, TYPE_SMN, SMN_0x11280190, &NBIO_CLKREQb_MAP_CNTL1, 0);
    if (Pcie0Cntl) {
      NBIO_CLKREQb_MAP_CNTL0.Field.PCIE_CLKREQB_CNTL_MASK = 0;
      NBIO_CLKREQb_MAP_CNTL0.Field.PCIE_CLKREQB_0_MAP = DefaultClkReqMapCntlZP[0][1];
      NBIO_CLKREQb_MAP_CNTL0.Field.PCIE_CLKREQB_1_MAP = DefaultClkReqMapCntlZP[1][1];
      NBIO_CLKREQb_MAP_CNTL0.Field.PCIE_CLKREQB_2_MAP = DefaultClkReqMapCntlZP[2][1];
      NBIO_CLKREQb_MAP_CNTL0.Field.PCIE_CLKREQB_3_MAP = DefaultClkReqMapCntlZP[3][1];
      NBIO_CLKREQb_MAP_CNTL0.Field.PCIE_CLKREQB_4_MAP = DefaultClkReqMapCntlZP[4][1];

      NBIO_CLKREQb_MAP_CNTL1.Field.PCIE_CLKREQB_CNTL_MASK = 1;
      NBIO_CLKREQb_MAP_CNTL1.Field.PCIE_CLKREQB_0_MAP = 0;
      NBIO_CLKREQb_MAP_CNTL1.Field.PCIE_CLKREQB_1_MAP = 0;
      NBIO_CLKREQb_MAP_CNTL1.Field.PCIE_CLKREQB_2_MAP = 0;
      NBIO_CLKREQb_MAP_CNTL1.Field.PCIE_CLKREQB_3_MAP = 0;
      NBIO_CLKREQb_MAP_CNTL1.Field.PCIE_CLKREQB_4_MAP = 0;
      Address = SMN_0x11140280_ADDRESS;
    }

    if (Pcie1Cntl) {
      NBIO_CLKREQb_MAP_CNTL0.Field.PCIE_CLKREQB_CNTL_MASK = 1;
      NBIO_CLKREQb_MAP_CNTL0.Field.PCIE_CLKREQB_0_MAP = 0;
      NBIO_CLKREQb_MAP_CNTL0.Field.PCIE_CLKREQB_1_MAP = 0;
      NBIO_CLKREQb_MAP_CNTL0.Field.PCIE_CLKREQB_2_MAP = 0;
      NBIO_CLKREQb_MAP_CNTL0.Field.PCIE_CLKREQB_3_MAP = 0;
      NBIO_CLKREQb_MAP_CNTL0.Field.PCIE_CLKREQB_4_MAP = 0;

      NBIO_CLKREQb_MAP_CNTL1.Field.PCIE_CLKREQB_CNTL_MASK = 0;
      NBIO_CLKREQb_MAP_CNTL1.Field.PCIE_CLKREQB_0_MAP = DefaultClkReqMapCntlZP[0][1] + 8;
      NBIO_CLKREQb_MAP_CNTL1.Field.PCIE_CLKREQB_1_MAP = DefaultClkReqMapCntlZP[1][1] + 8;
      NBIO_CLKREQb_MAP_CNTL1.Field.PCIE_CLKREQB_2_MAP = DefaultClkReqMapCntlZP[2][1] + 8;
      NBIO_CLKREQb_MAP_CNTL1.Field.PCIE_CLKREQB_3_MAP = DefaultClkReqMapCntlZP[3][1] + 8;
      NBIO_CLKREQb_MAP_CNTL1.Field.PCIE_CLKREQB_4_MAP = DefaultClkReqMapCntlZP[4][1] + 8;
      Address = SMN_0x11240280_ADDRESS;
    }
    NbioRegisterWrite (GnbHandle, TYPE_SMN, SMN_0x11180190, &NBIO_CLKREQb_MAP_CNTL0, GNB_REG_ACC_FLAG_S3SAVE);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, SMN_0x11280190, &NBIO_CLKREQb_MAP_CNTL1, GNB_REG_ACC_FLAG_S3SAVE);
    for (Index=0; Index < 7; Index++) {
      NbioRegisterRMW (GnbHandle, TYPE_SMN, (Address | (Index << 12)), (UINT32)~(PCIE_LC_CNTL_LC_DELAY_L1_EXIT_MASK), (0x1 << PCIE_LC_CNTL_LC_DELAY_L1_EXIT_OFFSET),0);
    }
  } else if (Pcie0Cntl) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Clk_req# signals can not cross both cores at the same time\n");
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieClkReqInterface Exit\n");
}
