/* $NoKeywords:$ */

/**
 * @file
 *
 * HSIO initialization services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: GNB
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Filecode.h>
#include <PiPei.h>
#include <HygonPcieComplex.h>
#include <GnbHsio.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Library/BaseLib.h>
#include <Library/IdsLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/GnbLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/OemGpioResetControlLib.h>
#include "HsioTraining.h"
#include <Library/HsioServiceLib.h>
#include <GnbRegisters.h>
#include <Library/HygonSocBaseLib.h>
#include <Hsio.h>

#define FILECODE  LIBRARY_HSIOLIB_HSIOLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/

/**
 * Helper function to dump the data being sent to SMU
 *
 *
 * @param[in]  PlatformDescriptor   Pointer to platform descriptor
 * @param[in]  PlatformDescSize     Size of platform descriptor
 * @param[in]  AncillaryData        Pointer to platform descriptor
 * @param[in]  AncillaryDataSize    Size of platform descriptor
 */
VOID
HsioSetOperatingSpeed (
  IN        GNB_HANDLE                      *GnbHandle,
  IN        PCIe_ENGINE_CONFIG              *PcieEngineList,
  IN        HSIO_ENGINE_DESCRIPTOR          *HsioEnginePointer,
  IN        HSIO_PORT_DESCRIPTOR            *UserCfgPortPointer
  )
{
  UINT32                     HsioMsgArg[6];
  UINT32                     HsioMboxMessage;
  UINT32                     TargetSpeed;
  UINT32                     EngineId;
  PCIE_LC_SPEED_CNTL_STRUCT  LcSpeedCntl;
  PCIe_WRAPPER_CONFIG        *PcieWrapper;
  UINT32                     SmnAddress;
  UINTN                      CpuModel;
  
  TargetSpeed = UserCfgPortPointer->Port.LinkSpeedCapability;
  if (TargetSpeed == HsioGenMaxSupported) {
    TargetSpeed = HsioGen3;
  }

  if (HsioEnginePointer->LinkDesc.Pcie.Capabilities.earlyTrainLink == 1) {
    TargetSpeed = HsioGen1;
  }

  // @TODO - Check PSPP if supported.
  PcieWrapper = PcieConfigGetParentWrapper (PcieEngineList);
  
  CpuModel = GetHygonSocModel ();
  
  if (CpuModel == HYGON_EX_CPU) {
    SmnAddress  = ConvertPciePortAddress (PCIE0_LC_SPEED_CNTL_ADDRESS_HYEX, GnbHandle, PcieWrapper, (PcieEngineList->Type.Port.PortId % 8));
  } else if (CpuModel == HYGON_GX_CPU) {
    SmnAddress  = ConvertPciePortAddress (PCIE0_LC_SPEED_CNTL_ADDRESS_HYGX, GnbHandle, PcieWrapper, (PcieEngineList->Type.Port.PortId % 8));
  } else {
    return;
  }
  
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LcSpeedCntl.Value, 0);

  if (TargetSpeed == HsioGen3) {
    if (LcSpeedCntl.Field.LC_OTHER_SIDE_SUPPORTS_GEN3 == 0) {
      TargetSpeed = HsioGen2;
    }
  }

  if (TargetSpeed == HsioGen2) {
    if (LcSpeedCntl.Field.LC_OTHER_SIDE_SUPPORTS_GEN2 == 0) {
      TargetSpeed = HsioGen1;
    }
  }

  if (TargetSpeed != HsioGen1) {
    EngineId = PcieEngineList->Type.Port.PcieBridgeId;
    LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *)NULL);
    HsioMsgArg[1] = HsioPcieEngine;
    HsioMsgArg[2] = TargetSpeed;
    HsioMsgArg[3] = EngineId;
    HsioMboxMessage = HSIO_MSG_PHYS_TARGETFREQUPDATE | (HSIO_freqUpdType_single << 10);
    HsioServiceRequest (GnbHandle, HsioMboxMessage, HsioMsgArg, HsioMsgArg);
  }
}
