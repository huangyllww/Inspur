/* $NoKeywords:$ */
/**
 * @file
 *
 * NBIF Device Control Functions
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
#include <HYGON.h>
#include <Gnb.h>
#include <PiPei.h>
#include <Filecode.h>
#include <GnbRegisters.h>
#include <HygonPcieComplex.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/HygonSmnAddressLib.h>
#include <PcieComplexData.h>

#define FILECODE NBIO_PCIE_HYEX_HYGONNBIOPCIESTPEI_DEVICECONTROLST_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------*/

NBIO_WAPPER_PHYLANE_MAP NbioPhyLaneMap[] = {       
  //NBIO, Wrapper, startlane, endlane 
  {0, 0, 112, 127},   
  {0, 1, 0,   15},
  {0, 2, 128, 129},
  {1, 0, 16,  31},
  {1, 1, 96,  111},
  {2, 0, 32,  47},
  {2, 1, 80,  95},
  {3, 0, 64,  79},
  {3, 1, 48,  63}
};

/**
 * NBIO number PhyLane Convert
 *
 *
 */
EFI_STATUS
NbioPhyLaneConvert (
  IN        UINT8             StartLane,
  IN        UINT8             *NBIONumber,
  IN        UINT8             *WrapperNumber
  )
{
  UINT16 i;
  
  if(StartLane > 129) {
    IDS_HDT_CONSOLE (GNB_TRACE, "StartLane is %d ,Not supported \n", StartLane);
    return  EFI_UNSUPPORTED;
  }
  
  for (i = 0; i < (sizeof(NbioPhyLaneMap)/sizeof(NBIO_WAPPER_PHYLANE_MAP)); i++) {
    if((StartLane >= NbioPhyLaneMap[i].StarLane) && (StartLane <= NbioPhyLaneMap[i].EndLane)) {
      if (NBIONumber != NULL) {
        *NBIONumber    = NbioPhyLaneMap[i].NbioNumber;
      }
      
      if (WrapperNumber != NULL) {
        *WrapperNumber = NbioPhyLaneMap[i].WrapperNumber;
      }
      break;
    }
  }

  return EFI_SUCCESS;
}


/**
 * NBIF Device Enable
 *
 *
 *
 * @param[in]  PeiServices   Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  GnbHandle     GNB_HANDLE
 */

VOID
NbioEnableNbifDevice (
  IN        GNB_HANDLE         *GnbHandle,
  IN        UINT8              DeviceType,
  IN        UINT8              StartLane
  )
{
  UINT8      RbIndex;
  EFI_STATUS Status;
  
  IDS_HDT_CONSOLE (GNB_TRACE, "NbioEnableNbifDevice (DeviceType=%x, StartLane=%d)\n", DeviceType, StartLane);

  Status = NbioPhyLaneConvert (StartLane, &RbIndex, NULL);
  if (EFI_UNSUPPORTED == Status) {
    return ;
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "Root bridge index is %d \n", RbIndex);
  }

  switch (DeviceType) {
  case HsioSATAEngine:
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE2(0, RbIndex, NBIF1_SATA_STRAP0_ADDRESS_HYEX),
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE2(0, RbIndex, NBIFMISC1_INTR_LINE_ENABLE_ADDRESS_HYEX),
                     (UINT32) ~(1 << 2),
                     (1 << 2),
                     0
                     );
    break;

  case HsioEthernetEngine:
    switch (StartLane) {
      case GPP0_XGBE0_XGBE_LANE:
        NbioRegisterRMW (GnbHandle,
                         TYPE_SMN,
                         NBIO_SPACE2(0, RbIndex, NBIF1_XGBE0_STRAP0_ADDRESS_HYEX),
                         (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                         (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                         0
                         );
        NbioRegisterRMW (GnbHandle,
                         TYPE_SMN,
                         NBIO_SPACE2(0, RbIndex, NBIFMISC1_INTR_LINE_ENABLE_ADDRESS_HYEX),
                         (UINT32) ~(1 << 4),
                         (1 << 4),
                         0
                         );
        break;
      case GPP0_XGBE1_XGBE_LANE:
        NbioRegisterRMW (GnbHandle,
                         TYPE_SMN,
                         NBIO_SPACE2(0, RbIndex, NBIF1_XGBE1_STRAP0_ADDRESS_HYEX),
                         (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                         (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                         0
                         );
        NbioRegisterRMW (GnbHandle,
                         TYPE_SMN,
                         NBIO_SPACE2(0, RbIndex, NBIFMISC1_INTR_LINE_ENABLE_ADDRESS_HYEX),
                         (UINT32) ~(1 << 5),
                         (1 << 5),
                         0
                         );
      break;
      case GPP0_XGBE2_XGBE_LANE:
        NbioRegisterRMW (GnbHandle,
                         TYPE_SMN,
                         NBIO_SPACE2(0, RbIndex, NBIF1_XGBE2_STRAP0_ADDRESS_HYEX),
                         (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                         (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                         0
                         );
        NbioRegisterRMW (GnbHandle,
                         TYPE_SMN,
                         NBIO_SPACE2(0, RbIndex, NBIFMISC1_INTR_LINE_ENABLE_ADDRESS_HYEX),
                         (UINT32) ~(1 << 6),
                         (1 << 6),
                         0
                         );
        break;
      case GPP0_XGBE3_XGBE_LANE:
        NbioRegisterRMW (GnbHandle,
                         TYPE_SMN,
                         NBIO_SPACE2(0, RbIndex, NBIF1_XGBE3_STRAP0_ADDRESS_HYEX),
                         (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                         (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                         0
                         );
        NbioRegisterRMW (GnbHandle,
                         TYPE_SMN,
                         NBIO_SPACE2(0, RbIndex, NBIFMISC1_INTR_LINE_ENABLE_ADDRESS_HYEX),
                         (UINT32) ~(1 << 7),
                         (1 << 7),
                         0
                         );
        break;
      default:
        // XGBE is only supported on lanes 7:4, any other is invalid
        ASSERT (FALSE);
      }
    break;

  case HsioNTBDevice:
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE (GnbHandle, NBIF0_NTB_STRAP0_ADDRESS_HYEX),
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE (GnbHandle, NBIFMISC0_INTR_LINE_ENABLE_ADDRESS_HYEX),
                     (UINT32) ~(1 << 1),
                     (1 << 1),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE (GnbHandle, NBIF2_NTB_STRAP0_ADDRESS_HYEX),
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE (GnbHandle, NBIFMISC2_INTR_LINE_ENABLE_ADDRESS_HYEX),
                     (UINT32) ~(1 << 1),
                     (1 << 1),
                     0
                     );
    break;

  case HsioHDaudioEngine:
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE2(0, RbIndex, NBIF1_HDAUDIO_STRAP0_ADDRESS_HYEX),
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE2(0, RbIndex, NBIFMISC1_INTR_LINE_ENABLE_ADDRESS_HYEX),
                     (UINT32) ~(1 << 3),
                     (1 << 3),
                     0
                     );
    break;
  }
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * NBIF Device Enable
 *
 *
 *
 * @param[in]  PeiServices   Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  GnbHandle     GNB_HANDLE
 */

VOID
NbioDisableNbifDevice (
  IN        GNB_HANDLE         *GnbHandle,
  IN        UINT8              DeviceType,
  IN        UINT8              StartLane
  )
{
  UINT8      RbIndex;
  EFI_STATUS Status;

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioDisableNbifDevice (DeviceType=%x, StartLane=%d)\n", DeviceType, StartLane);

  Status = NbioPhyLaneConvert (StartLane, &RbIndex,NULL);
  if (EFI_UNSUPPORTED == Status) {
    return ;
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "Root bridge index is %d \n", RbIndex);
  }
  
  switch (DeviceType) {
  case HsioSATAEngine:

    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE2(0, RbIndex, NBIF1_SATA_STRAP0_ADDRESS_HYEX),
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE2(0, RbIndex, NBIFMISC1_INTR_LINE_ENABLE_ADDRESS_HYEX),
                     (UINT32) ~(1 << 2),
                     (0 << 2),
                     0
                     );
    break;

  case HsioEthernetEngine:
    switch (StartLane) {
    case 4:
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIO_SPACE2(0, RbIndex, NBIF1_XGBE0_STRAP0_ADDRESS_HYEX),
                       (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                       (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                       0
                       );
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIO_SPACE2(0, RbIndex, NBIFMISC1_INTR_LINE_ENABLE_ADDRESS_HYEX),
                       (UINT32) ~(1 << 4),
                       (0 << 4),
                       0
                       );
      break;
    case 5:
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIO_SPACE2(0, RbIndex, NBIF1_XGBE1_STRAP0_ADDRESS_HYEX),
                       (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                       (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                       0
                       );
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIO_SPACE2(0, RbIndex, NBIFMISC1_INTR_LINE_ENABLE_ADDRESS_HYEX),
                       (UINT32) ~(1 << 5),
                       (0 << 5),
                       0
                       );
      break;
    case 6:
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIO_SPACE2(0, RbIndex, NBIF1_XGBE2_STRAP0_ADDRESS_HYEX),
                       (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                       (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                       0
                       );
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIO_SPACE2(0, RbIndex, NBIFMISC1_INTR_LINE_ENABLE_ADDRESS_HYEX),
                       (UINT32) ~(1 << 6),
                       (0 << 6),
                       0
                       );
      break;
    case 7:
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIO_SPACE2(0, RbIndex, NBIF1_XGBE3_STRAP0_ADDRESS_HYEX),
                       (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                       (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                       0
                       );
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIO_SPACE2(0, RbIndex, NBIFMISC1_INTR_LINE_ENABLE_ADDRESS_HYEX),
                       (UINT32) ~(1 << 7),
                       (0 << 7),
                       0
                       );
      break;
    default:
      // XGBE is only supported on lanes 7:4, any other is invalid
      ASSERT (FALSE);
    }
    break;

  case HsioNTBDevice:
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE2(0, RbIndex, NBIF0_NTB_STRAP0_ADDRESS_HYEX),
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE2(0, RbIndex, NBIFMISC0_INTR_LINE_ENABLE_ADDRESS_HYEX),
                     (UINT32) ~(1 << 1),
                     (0 << 1),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE2(0, RbIndex, NBIF2_NTB_STRAP0_ADDRESS_HYEX),
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE2(0, RbIndex, NBIFMISC2_INTR_LINE_ENABLE_ADDRESS_HYEX),
                     (UINT32) ~(1 << 1),
                     (0 << 1),
                     0
                     );
    break;

  case HsioHDaudioEngine:
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE2(0, RbIndex, NBIF1_HDAUDIO_STRAP0_ADDRESS_HYEX),
                     (UINT32) ~(NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK),
                     (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE2(0, RbIndex, NBIFMISC1_INTR_LINE_ENABLE_ADDRESS_HYEX),
                     (UINT32) ~(1 << 3),
                     (0 << 3),
                     0
                     );
    break;
  }
  return;
}


