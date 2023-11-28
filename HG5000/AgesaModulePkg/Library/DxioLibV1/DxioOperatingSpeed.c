/* $NoKeywords:$ */
/**
 * @file
 *
 * DXIO initialization services
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
#include <Filecode.h>
#include <PiPei.h>
#include <AmdPcieComplex.h>
#include <GnbDxio.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Library/BaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/GnbLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/DxioLibV1.h>
#include <Library/OemGpioResetControlLib.h>
#include "DxioLibLocal.h"

#define FILECODE LIBRARY_DXIOLIBV1_DXIOLIBV1_FILECODE
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
DxioSetOperatingSpeed (
  IN        GNB_HANDLE                      *GnbHandle,
  IN        PCIe_ENGINE_CONFIG              *PcieEngineList,
  IN        DXIO_ENGINE_DESCRIPTOR          *DxioEnginePointer,
  IN        DXIO_PORT_DESCRIPTOR            *UserCfgPortPointer,
  IN        PEI_AMD_NBIO_SMU_SERVICES_PPI   *SmuServicesPpi
  )
{
  UINT32                    SmuArg[6];
  UINT32                    DxioMboxMessage;
  UINT32                    TargetSpeed;
  UINT32                    EngineId;
  PCIE_LC_SPEED_CNTL_STRUCT LcSpeedCntl;

  TargetSpeed = UserCfgPortPointer->Port.LinkSpeedCapability;
  if (TargetSpeed == DxioGenMaxSupported) {
    TargetSpeed = DxioGen3;
  }
  if (DxioEnginePointer->LinkDesc.Pcie.Capabilities.earlyTrainLink == 1) {
    TargetSpeed = DxioGen1;
  }

  //@TODO - Check PSPP if supported.

  if (PcieEngineList->Type.Port.PortId > 7) {
    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_SPEED_CNTL_ADDRESS + ((PcieEngineList->Type.Port.PortId - 8) << 12), &LcSpeedCntl.Value, 0);
  } else {
    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_SPEED_CNTL_ADDRESS + (PcieEngineList->Type.Port.PortId << 12), &LcSpeedCntl.Value, 0);
  }

  if (TargetSpeed == DxioGen3) {
    if (LcSpeedCntl.Field.LC_OTHER_SIDE_SUPPORTS_GEN3 == 0) {
      TargetSpeed = DxioGen2;
    }
  }

  if (TargetSpeed == DxioGen2) {
    if (LcSpeedCntl.Field.LC_OTHER_SIDE_SUPPORTS_GEN2 == 0) {
      TargetSpeed = DxioGen1;
    }
  }

  if (TargetSpeed != DxioGen1) {
    EngineId = PcieEngineList->Type.Port.PcieBridgeId;
    LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
    SmuArg[1] = DxioPcieEngine;
    SmuArg[2] = TargetSpeed;
    SmuArg[3] = EngineId;
    DxioMboxMessage = DXIO_MSG_PHYS_TARGETFREQUPDATE | (DXIO_freqUpdType_single << 10);
    SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DxioMboxMessage, SmuArg, SmuArg);
  }
}


