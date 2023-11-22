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
#include <PiPei.h>
#include <HygonPcieComplex.h>
#include <GnbHsio.h>
#include <Library/BaseLib.h>
#include <Library/IdsLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/GnbLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/SmnAccessLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>
#include <Ppi/EqTablePpi.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/GnbPciLib.h>
#include <GnbRegisters.h>
#include <Hsio.h>
#include "HsioPcieEq.h"
#include "HsioTraining.h"
#include <Library/GnbInternalPciLib.h>

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

/**
 * Calculate Phy Lane Reg Address
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  LaneNumber     lane number in dxio table 
 * @param[in]  RegOft         PHY register offset
 * @param[OUT] Address        Pointer to PHY smn address
 *
 * @retval    EFI_STATUS
 *            EFI_INVALID_PARAMETER
 */ 
EFI_STATUS
CalculatePhyLaneRegAddress (
  IN     GNB_HANDLE           *GnbHandle,
  IN     PCIe_ENGINE_CONFIG   *Engine,
  IN     UINT32               LaneNumber,
  IN     UINT32               RegOft,
  OUT    UINT32               *Address
)
{
  UINT32                    PortId;
  UINT32                    PhyRegBase;
  UINT32                    PhyRegAddr;
  UINT32                    LaneOft;
  UINT32                    PhyOft;
  UINT32                    PhyLaneOft;

  if ((LaneNumber < Engine->EngineData.StartLane) ||
     (LaneNumber > Engine->EngineData.EndLane))
     return EFI_INVALID_PARAMETER;

  PortId = Engine->Type.Port.PortId;
  LaneOft = LaneNumber % 16;
  PhyRegBase = (PortId <= 15) ? (0x13200000 + ((GnbHandle->RbId * 2 + PortId / 8) << 20)) : 0x13A00000;
  PhyOft = LaneOft / 4;
  PhyLaneOft = LaneOft % 4;
  PhyRegAddr = PhyRegBase + ((PhyOft * 4) << 16) + ((PhyLaneOft * 4) << 8) + RegOft * 2;
  *Address = PhyRegAddr;

  return EFI_SUCCESS;  
}

/**
 * Calculate Phy Reg Address
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  LaneNumber     lane number in dxio table 
 * @param[in]  RegOft         PHY register offset
 * @param[OUT] Address        Pointer to PHY smn address
 *
 * @retval    EFI_STATUS
 *            EFI_INVALID_PARAMETER
 */
EFI_STATUS
CalculatePhyRegAddress (
  IN     GNB_HANDLE           *GnbHandle,
  IN     PCIe_ENGINE_CONFIG   *Engine,
  IN     UINT32               LaneNumber,
  IN     UINT32               RegOft,
  OUT    UINT32               *Address
)
{
  UINT32                    PortId;
  UINT32                    PhyRegBase;
  UINT32                    PhyRegAddr;
  UINT32                    LaneOft;
  UINT32                    PhyOft;

  if ((LaneNumber < Engine->EngineData.StartLane) ||
     (LaneNumber > Engine->EngineData.EndLane))
     return EFI_INVALID_PARAMETER;

  PortId = Engine->Type.Port.PortId;
  LaneOft = LaneNumber % 16;
  PhyRegBase = (PortId <= 15) ? (0x13200000 + ((GnbHandle->RbId * 2 + PortId / 8) << 20)) : 0x13A00000;
  PhyOft = LaneOft / 4;
  PhyRegAddr = PhyRegBase + ((PhyOft * 4) << 16) + RegOft * 2;
  *Address = PhyRegAddr;

  return EFI_SUCCESS;  
}

/**
 * Read PHY Lane Register
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  LaneNumber     lane number in dxio table 
 * @param[in]  RegOft         PHY register offset
 * @param[OUT] Value          Pointer to Read Value
 *
 * @retval    EFI_STATUS
 *            EFI_INVALID_PARAMETER
 */
EFI_STATUS
ReadPhyLaneReg (
  IN     GNB_HANDLE           *GnbHandle,
  IN     PCIe_ENGINE_CONFIG   *Engine,
  IN     UINT32               LaneNumber,
  IN     UINT32               RegOft,
  OUT    UINT16               *Value
)
{
  EFI_STATUS                Status;
  UINT32                    PhyRegAddr;
  UINT32                    Base;
  UINT32                    Oft;
  UINT32                    Data32;
  
  Status = CalculatePhyLaneRegAddress (GnbHandle, Engine, LaneNumber, RegOft, &PhyRegAddr);
  if (EFI_ERROR(Status))
    return Status;
  Base = PhyRegAddr & 0xFFFFFFFC;
  Oft = PhyRegAddr & 0x3;

  NbioRegisterRead (GnbHandle, TYPE_SMN, Base, &Data32, 0);  
  *Value = (UINT16)(Data32 >> (Oft * 8));

  return EFI_SUCCESS;  
}

/**
 * Write PHY Lane Register
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  LaneNumber     lane number in dxio table 
 * @param[in]  RegOft         PHY register offset
 * @param[in]  Value          Pointer to Write Value
 *
 * @retval    EFI_STATUS
 *            EFI_INVALID_PARAMETER
 */
EFI_STATUS
WritePhyLaneReg (
  IN     GNB_HANDLE           *GnbHandle,
  IN     PCIe_ENGINE_CONFIG   *Engine,
  IN     UINT32               LaneNumber,
  IN     UINT32               RegOft,
  IN     UINT16               *Value
)
{
  EFI_STATUS                Status;
  UINT32                    PhyRegAddr;
  UINT32                    Base;
  UINT32                    Oft;
  UINT32                    Data32;

  Status = CalculatePhyLaneRegAddress (GnbHandle, Engine, LaneNumber, RegOft, &PhyRegAddr);
  if (EFI_ERROR(Status))
    return Status;
  Base = PhyRegAddr & 0xFFFFFFFC;
  Oft = PhyRegAddr & 0x3;
  NbioRegisterRead (GnbHandle, TYPE_SMN, Base, &Data32, 0); 
  Data32 &= ~(0xFFFF << (Oft * 8));
  Data32 |= (*Value << (Oft * 8));
  NbioRegisterWrite (GnbHandle, TYPE_SMN, Base, &Data32, 0);  

  return EFI_SUCCESS;  
}

/**
 * Read then write PHY Lane Register
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  LaneNumber     lane number in dxio table 
 * @param[in]  RegOft         PHY register offset
 * @param[in]  AndMask        And value
 * @param[in]  AndMask        Or value
 *
 * @retval    EFI_STATUS
 *            EFI_INVALID_PARAMETER
 */
EFI_STATUS
RwPhyLaneReg (
  IN     GNB_HANDLE           *GnbHandle,
  IN     PCIe_ENGINE_CONFIG   *Engine,
  IN     UINT32               LaneNumber,
  IN     UINT32               RegOft,
  IN     UINT16               AndMask,
  IN     UINT16               OrMask
)
{
  EFI_STATUS                Status;
  UINT16                    Value;
  UINT32                    Lane, StartLane, EndLane;

  if (LaneNumber == 0xFF) {
    StartLane = Engine->EngineData.StartLane;
    EndLane = Engine->EngineData.EndLane;
  } else {
    StartLane = LaneNumber;
    EndLane = LaneNumber;
  }

  for (Lane = StartLane; Lane <= EndLane; Lane++) {
    Status = ReadPhyLaneReg (GnbHandle, Engine, Lane, RegOft, &Value);
    if (EFI_ERROR(Status))
      return Status;
    Value &= AndMask;
    Value |= OrMask;
    Status = WritePhyLaneReg (GnbHandle, Engine, Lane, RegOft, &Value);
    if (EFI_ERROR(Status))
      return Status;
  }

  return EFI_SUCCESS;
}

/**
 * Read PHY Register
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  LaneNumber     lane number in dxio table 
 * @param[in]  RegOft         PHY register offset
 * @param[OUT] Value          Pointer to Read Value
 *
 * @retval    EFI_STATUS
 *            EFI_INVALID_PARAMETER
 */
EFI_STATUS
ReadPhyReg (
  IN     GNB_HANDLE           *GnbHandle,
  IN     PCIe_ENGINE_CONFIG   *Engine,
  IN     UINT32               LaneNumber,
  IN     UINT32               RegOft,
  OUT    UINT16               *Value
)
{
  EFI_STATUS                Status;
  UINT32                    PhyRegAddr;
  UINT32                    Base;
  UINT32                    Oft;
  UINT32                    Data32;
  
  Status = CalculatePhyLaneRegAddress (GnbHandle, Engine, LaneNumber, RegOft, &PhyRegAddr);
  if (EFI_ERROR(Status))
    return Status;
  Base = PhyRegAddr & 0xFFFFFFFC;
  Oft = PhyRegAddr & 0x3;

  NbioRegisterRead (GnbHandle, TYPE_SMN, Base, &Data32, 0);  
  *Value = (UINT16)(Data32 >> (Oft * 8));

  return EFI_SUCCESS;  
}

/**
 * Write PHY Register
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  LaneNumber     lane number in dxio table 
 * @param[in]  RegOft         PHY register offset
 * @param[in]  Value          Pointer to Write Value
 *
 * @retval    EFI_STATUS
 *            EFI_INVALID_PARAMETER
 */
EFI_STATUS
WritePhyReg (
  IN     GNB_HANDLE           *GnbHandle,
  IN     PCIe_ENGINE_CONFIG   *Engine,
  IN     UINT32               LaneNumber,
  IN     UINT32               RegOft,
  IN     UINT16               *Value
)
{
  EFI_STATUS                Status;
  UINT32                    PhyRegAddr;
  UINT32                    Base;
  UINT32                    Oft;
  UINT32                    Data32;

  Status = CalculatePhyLaneRegAddress (GnbHandle, Engine, LaneNumber, RegOft, &PhyRegAddr);
  if (EFI_ERROR(Status))
    return Status;
  Base = PhyRegAddr & 0xFFFFFFFC;
  Oft = PhyRegAddr & 0x3;
  NbioRegisterRead (GnbHandle, TYPE_SMN, Base, &Data32, 0); 
  Data32 &= ~(0xFFFF << (Oft * 8));
  Data32 |= (*Value << (Oft * 8));
  NbioRegisterWrite (GnbHandle, TYPE_SMN, Base, &Data32, 0);  

  return EFI_SUCCESS;  
}

/**
 * Read then write PHY Register
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  LaneNumber     lane number in dxio table 
 * @param[in]  RegOft         PHY register offset
 * @param[in]  AndMask        And value
 * @param[in]  AndMask        Or value
 *
 * @retval    EFI_STATUS
 *            EFI_INVALID_PARAMETER
 */
EFI_STATUS
RwPhyReg (
  IN     GNB_HANDLE           *GnbHandle,
  IN     PCIe_ENGINE_CONFIG   *Engine,
  IN     UINT32               LaneNumber,
  IN     UINT32               RegOft,
  IN     UINT16               AndMask,
  IN     UINT16               OrMask
)
{
  EFI_STATUS                Status;
  UINT16                    Value;
  UINT32                    Lane, StartLane, EndLane;

  if (LaneNumber == 0xFF) {
    StartLane = Engine->EngineData.StartLane;
    EndLane = Engine->EngineData.EndLane;
  } else {
    StartLane = LaneNumber;
    EndLane = LaneNumber;
  }

  for (Lane = StartLane; Lane <= EndLane; Lane += 4) {
    Status = ReadPhyReg (GnbHandle, Engine, Lane, RegOft, &Value);
    if (EFI_ERROR(Status))
      return Status;
    Value &= AndMask;
    Value |= OrMask;
    Status = WritePhyReg (GnbHandle, Engine, Lane, RegOft, &Value);
    if (EFI_ERROR(Status))
      return Status;
  }

  return EFI_SUCCESS;
}

/**
 * Pcie Gen 3 down stream TX Cursor
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  TxPreCursor    Pre-cursor value
 * @param[in]  TxMainCursor   main-cursor value
 * @param[in]  TxPostCursor   post-cursor value
 *
 * @retval    VOID
 */

VOID
PcieGen3DsTxCursor (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      TxPreCursor,
  IN     UINT8                      TxMainCursor,
  IN     UINT8                      TxPostCursor
  )
{
  PCIe_WRAPPER_CONFIG                 *PcieWrapper;
  UINT8                               MappingPortID;
  PCIE_LC_FORCE_COEFF_STRUCT          LcForceCoeff;
  UINT32                              SmnAddress;

  PcieWrapper   = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  SmnAddress = ConvertPciePortAddress(PCIE0_LC_FORCE_COEFF_ADDRESS_HYEX, GnbHandle, PcieWrapper, MappingPortID);
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LcForceCoeff.Value, 0);
  LcForceCoeff.Field.LC_FORCE_COEFF = 1;
  LcForceCoeff.Field.LC_FORCE_PRE_CURSOR = TxPreCursor;
  LcForceCoeff.Field.LC_FORCE_CURSOR = TxMainCursor;
  LcForceCoeff.Field.LC_FORCE_POST_CURSOR = TxPostCursor;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LcForceCoeff.Value, 0);
}

/**
 * Pcie Gen 4 down stream TX Cursor
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  TxPreCursor    Pre-cursor value
 * @param[in]  TxMainCursor   main-cursor value
 * @param[in]  TxPostCursor   post-cursor value
 *
 * @retval    VOID
 */

VOID
PcieGen4DsTxCursor (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      TxPreCursor,
  IN     UINT8                      TxMainCursor,
  IN     UINT8                      TxPostCursor
  )
{
  PCIe_WRAPPER_CONFIG                 *PcieWrapper;
  UINT8                               MappingPortID;
  PCIE_LC_FORCE_COEFF2_STRUCT         LcForceCoeff2;
  UINT32                              SmnAddress;

  PcieWrapper   = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  SmnAddress = ConvertPciePortAddress(PCIE0_LC_FORCE_COEFF2_ADDRESS_HYEX, GnbHandle, PcieWrapper, MappingPortID);
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LcForceCoeff2.Value, 0);
  LcForceCoeff2.Field.LC_FORCE_COEFF_16GT = 1;
  LcForceCoeff2.Field.LC_FORCE_PRE_CURSOR_16GT = TxPreCursor;
  LcForceCoeff2.Field.LC_FORCE_CURSOR_16GT = TxMainCursor;
  LcForceCoeff2.Field.LC_FORCE_POST_CURSOR_16GT = TxPostCursor;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LcForceCoeff2.Value, 0);
}

/**
 * Pcie Gen 5 down stream TX Cursor
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  TxPreCursor    Pre-cursor value
 * @param[in]  TxMainCursor   main-cursor value
 * @param[in]  TxPostCursor   post-cursor value
 *
 * @retval    VOID
 */

VOID
PcieGen5DsTxCursor (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      TxPreCursor,
  IN     UINT8                      TxMainCursor,
  IN     UINT8                      TxPostCursor
  )
{
  PCIe_WRAPPER_CONFIG                 *PcieWrapper;
  UINT8                               MappingPortID;
  PCIE_LC_FORCE_COEFF_32GT_STRUCT     LcForceCoeff32GT;
  UINT32                              SmnAddress;

  PcieWrapper   = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  SmnAddress = ConvertPciePortAddress(PCIE0_LC_FORCE_COEFF_32GT_ADDRESS_HYEX, GnbHandle, PcieWrapper, MappingPortID);
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LcForceCoeff32GT.Value, 0);
  LcForceCoeff32GT.Field.LC_FORCE_COEFF_32GT = 1;
  LcForceCoeff32GT.Field.LC_FORCE_PRE_CURSOR_32GT = TxPreCursor;
  LcForceCoeff32GT.Field.LC_FORCE_CURSOR_32GT = TxMainCursor;
  LcForceCoeff32GT.Field.LC_FORCE_POST_CURSOR_32GT = TxPostCursor;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LcForceCoeff32GT.Value, 0);
}

/**
 * Pcie Gen 3 up stream TX Cursor
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  TxPreCursor    Pre-cursor value
 * @param[in]  TxMainCursor   main-cursor value
 * @param[in]  TxPostCursor   post-cursor value
 *
 * @retval    VOID
 */

VOID
PcieGen3UsTxCursor (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      TxPreCursor,
  IN     UINT8                      TxMainCursor,
  IN     UINT8                      TxPostCursor
  )
{
  PCIe_WRAPPER_CONFIG                 *PcieWrapper;
  UINT8                               MappingPortID;
  PCIE_LC_FORCE_EQ_REQ_COEFF_STRUCT   LcForceReqCoeff;
  UINT32                              SmnAddress;

  PcieWrapper   = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  SmnAddress = ConvertPciePortAddress(PCIE0_LC_FORCE_EQ_REQ_COEFF_ADDRESS_HYEX, GnbHandle, PcieWrapper, MappingPortID);
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LcForceReqCoeff.Value, 0);
  LcForceReqCoeff.Field.LC_FORCE_COEFF_IN_EQ_REQ_PHASE = 1;
  LcForceReqCoeff.Field.LC_FORCE_PRE_CURSOR_REQ = TxPreCursor;
  LcForceReqCoeff.Field.LC_FORCE_CURSOR_REQ = TxMainCursor;
  LcForceReqCoeff.Field.LC_FORCE_POST_CURSOR_REQ = TxPostCursor;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LcForceReqCoeff.Value, 0);
}

/**
 * Pcie Gen 4 up stream TX Cursor
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  TxPreCursor    Pre-cursor value
 * @param[in]  TxMainCursor   main-cursor value
 * @param[in]  TxPostCursor   post-cursor value
 *
 * @retval    VOID
 */

VOID
PcieGen4UsTxCursor (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      TxPreCursor,
  IN     UINT8                      TxMainCursor,
  IN     UINT8                      TxPostCursor
  )
{
  PCIe_WRAPPER_CONFIG                 *PcieWrapper;
  UINT8                               MappingPortID;
  PCIE_LC_FORCE_EQ_REQ_COEFF2_STRUCT  LcForceReqCoeff2;
  UINT32                              SmnAddress;

  PcieWrapper   = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  SmnAddress = ConvertPciePortAddress(PCIE0_LC_FORCE_EQ_REQ_COEFF2_ADDRESS_HYEX, GnbHandle, PcieWrapper, MappingPortID);
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LcForceReqCoeff2.Value, 0);
  LcForceReqCoeff2.Field.LC_FORCE_COEFF_IN_EQ_REQ_PHASE_16GT = 1;
  LcForceReqCoeff2.Field.LC_FORCE_PRE_CURSOR_REQ_16GT = TxPreCursor;
  LcForceReqCoeff2.Field.LC_FORCE_CURSOR_REQ_16GT = TxMainCursor;
  LcForceReqCoeff2.Field.LC_FORCE_POST_CURSOR_REQ_16GT = TxPostCursor;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LcForceReqCoeff2.Value, 0);
}

/**
 * Pcie Gen 5 up stream TX Cursor
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  TxPreCursor    Pre-cursor value
 * @param[in]  TxMainCursor   main-cursor value
 * @param[in]  TxPostCursor   post-cursor value
 *
 * @retval    VOID
 */

VOID
PcieGen5UsTxCursor (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      TxPreCursor,
  IN     UINT8                      TxMainCursor,
  IN     UINT8                      TxPostCursor
  )
{
  PCIe_WRAPPER_CONFIG                      *PcieWrapper;
  UINT8                                    MappingPortID;
  PCIE_LC_FORCE_EQ_REQ_COEFF_32GT_STRUCT   LcForceReqCoeff32GT;
  UINT32                                   SmnAddress;

  PcieWrapper   = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  SmnAddress = ConvertPciePortAddress(PCIE0_LC_FORCE_EQ_REQ_COEFF_32GT_ADDRESS_HYEX, GnbHandle, PcieWrapper, MappingPortID);
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LcForceReqCoeff32GT.Value, 0);
  LcForceReqCoeff32GT.Field.LC_FORCE_COEFF_IN_EQ_REQ_PHASE_32GT = 1;
  LcForceReqCoeff32GT.Field.LC_FORCE_PRE_CURSOR_REQ_32GT = TxPreCursor;
  LcForceReqCoeff32GT.Field.LC_FORCE_CURSOR_REQ_32GT = TxMainCursor;
  LcForceReqCoeff32GT.Field.LC_FORCE_POST_CURSOR_REQ_32GT = TxPostCursor;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LcForceReqCoeff32GT.Value, 0);
}

/**
 * Pcie Gen 3 up stream TX Preset
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  Preset         Preset value
 *
 * @retval    VOID
 */

VOID
PcieGen3UsPreset (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      Preset
  )
{
  PCIe_WRAPPER_CONFIG                 *PcieWrapper;
  UINT8                               MappingPortID;
  PCIE_LC_CNTL4_STRUCT                LcCntl4;
  UINT32                              SmnAddress;

  PcieWrapper   = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  SmnAddress = ConvertPciePortAddress(PCIE0_GPP0_PCIE_LC_CNTL4_ADDRESS_HYEX, GnbHandle, PcieWrapper, MappingPortID);
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LcCntl4.Value, 0);
  LcCntl4.Field.LC_FORCE_PRESET_IN_EQ_REQ_PHASE = 1;
  LcCntl4.Field.LC_FORCE_PRESET_VALUE = Preset;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LcCntl4.Value, 0);
}

/**
 * Pcie Gen 4 up stream TX Preset
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  Preset         Preset value
 *
 * @retval    VOID
 */

VOID
PcieGen4UsPreset (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      Preset
  )
{
  PCIe_WRAPPER_CONFIG                 *PcieWrapper;
  UINT8                               MappingPortID;
  PCIE_LC_CNTL8_STRUCT                LcCntl8;
  UINT32                              SmnAddress;

  PcieWrapper   = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  SmnAddress = ConvertPciePortAddress(PCIE0_PCIE_LC_CNTL8_ADDRESS_HYEX, GnbHandle, PcieWrapper, MappingPortID);
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LcCntl8.Value, 0);
  LcCntl8.Field.LC_FORCE_PRESET_IN_EQ_REQ_PHASE_16GT = 1;
  LcCntl8.Field.LC_FORCE_PRESET_VALUE_16GT = Preset;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LcCntl8.Value, 0);
}

/**
 * Pcie Gen 5 up stream TX Preset
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  Preset         Preset value
 *
 * @retval    VOID
 */

VOID
PcieGen5UsPreset (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      Preset
  )
{
  PCIe_WRAPPER_CONFIG                    *PcieWrapper;
  UINT8                                  MappingPortID;
  PCIE_LC_32GT_EQUALIZATION_CNTL_STRUCT  LcCntl32GT;
  UINT32                                 SmnAddress;

  PcieWrapper   = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  SmnAddress = ConvertPciePortAddress(PCIE0_LC_32GT_EQUALIZATION_CNTL_ADDRESS_HYEX, GnbHandle, PcieWrapper, MappingPortID);
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LcCntl32GT.Value, 0);
  LcCntl32GT.Field.LC_FORCE_PRESET_IN_EQ_REQ_PHASE_32GT = 1;
  LcCntl32GT.Field.LC_FORCE_PRESET_VALUE_32GT = Preset;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LcCntl32GT.Value, 0);
}

/**
 * Pcie RX Term offset
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  Value          value
 *
 * @retval    VOID
 */

VOID
PcieRxTermOffset (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      Value
  )
{
  RwPhyReg (
    GnbHandle, 
    Engine, 
    0xFF, 
    SUP_DIG_SUP_OVRD_IN_1_ADDRESS_HYEX, 
    (UINT16)~(RX_TERM_OFFSET_OVRD_EN_MASK | TXUP_TERM_OFFSET_MASK),
    (UINT16)((1 << (TXUP_TERM_OFFSET_OVRD_EN_OFFSET)) | (Value << RX_TERM_OFFSET_OFFSET))
    );
}

/**
 * Pcie TX up Term offset
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  Value          value
 *
 * @retval    VOID
 */

VOID
PcieTxupTermOffset (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      Value
  )
{
  RwPhyReg (
    GnbHandle, 
    Engine, 
    0xFF, 
    SUP_DIG_SUP_OVRD_IN_1_ADDRESS_HYEX, 
    (UINT16)~(TXUP_TERM_OFFSET_OVRD_EN_MASK | RX_TERM_OFFSET_MASK),
    (UINT16)((1 << (RX_TERM_OFFSET_OVRD_EN_OFFSET)) | (Value << TXUP_TERM_OFFSET_OFFSET))
    );
}

/**
 * Pcie TX down Term offset
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  Value          value
 *
 * @retval    VOID
 */

VOID
PcieTxdnTermOffset (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      Value
  )
{
  RwPhyReg (
    GnbHandle, 
    Engine, 
    0xFF, 
    SUP_DIG_SUP_OVRD_IN_2_ADDRESS_HYEX, 
    (UINT16)~(TXDN_TERM_OFFSET_OVRD_EN_MASK | TXDN_TERM_OFFSET_MASK),
    (UINT16)((1 << (TXDN_TERM_OFFSET_OVRD_EN_OFFSET)) | (Value << TXDN_TERM_OFFSET_OFFSET))
    );
}

/**
 * Pcie RX SSC off phug1
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  Value          value
 *
 * @retval    VOID
 */

VOID
PcieRxSscOffPhug1 (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      Value
  )
{
  RwPhyLaneReg (
    GnbHandle,
    Engine,
    0xFF,
    LANE_DIG_RX_CDR_CNTL_3_HYEX,
    (UINT16)(~(BIT3 | BIT4 | BIT5)),
    (UINT16)(1 << 3)
    );
}

/**
 * Pcie RX SSC off frug1
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  Value          value
 *
 * @retval    VOID
 */

VOID
PcieRxSscOffFrug1 (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      Value
  )
{
  RwPhyLaneReg (
    GnbHandle,
    Engine,
    0xFF,
    LANE_DIG_RX_CDR_CNTL_4_HYEX,
    (UINT16)(~(BIT0 | BIT1 | BIT2)),
    (UINT16)1
    );
}

/**
 * Read Down stream device configuration
 *
 * @param[in]  DsBus      Down stream device bus number
 * @param[in]  Oft        configuration register offset
 * @param[in]  SizeOfByte size of byte
 * @param[out] Value      Pointer to read value
 * 
 * @retval
 */

VOID
ReadDsDevCfg (
  UINT8    DsBus,
  UINT32   Oft,
  UINT32   SizeOfByte,
  UINT32   *Value
  )
{
  UINT32   MmCfg;
  UINT32   PciCfgAddr;
  
  MmCfg = (UINT32)(AsmReadMsr64(MSR_MM_CFG) & 0xfff00000);
  PciCfgAddr = MmCfg + (DsBus << 20) + Oft;
  switch (SizeOfByte) {
    case 1:
      *Value = MmioRead8 (PciCfgAddr);
      break;
    case 2:
      *Value = MmioRead16 (PciCfgAddr);
      break;
    case 4:
      *Value = MmioRead32 (PciCfgAddr);
      break;
    default:
      break;
  }
}

/**
 * Write Down stream device configuration
 *
 * @param[in]  DsBus      Down stream device bus number
 * @param[in]  Oft        configuration register offset
 * @param[in]  SizeOfByte size of byte
 * @param[out] Value      Pointer to write value
 * 
 * @retval
 */

VOID
WriteDsDevCfg (
  UINT8    DsBus,
  UINT32   Oft,
  UINT32   SizeOfByte,
  UINT32   *Value
  )
{
  UINT32   MmCfg;
  UINT32   PciCfgAddr;
  
  MmCfg = (UINT32)(AsmReadMsr64(MSR_MM_CFG) & 0xfff00000);
  PciCfgAddr = MmCfg + (DsBus << 20) + Oft;
  switch (SizeOfByte) {
    case 1:
      MmioWrite8 (PciCfgAddr, (UINT8)(*Value));
      break;
    case 2:
      MmioWrite16 (PciCfgAddr, (UINT16)(*Value));
      break;
    case 4:
      MmioWrite32 (PciCfgAddr, *Value);
      break;
    default:
      break;
  }
}


/**
 * Root bridge enum Down stream device
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[out] DsBus          Pointer to DsBus
 * 
 * @retval
 */

UINT32
RbEnumDs (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  OUT    UINT8                      *DsBus
)
{
  UINT32                            OldValue, NewValue;
  UINT8                             PrimaryBus, SecondaryBus, SubBusNum;
  UINT32                            SmnAddress;
  
  SmnAddress = ConvertPciePortAddress2 (
                 SMN_PCIE0_SUB_BUS_NUMBER_LATENCY_HYGX,
                 GnbHandle,
                 Engine
                 );
  
  //save old sub bus config
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &OldValue, 0);
  //allocate bus for DS
  PrimaryBus   = (UINT8)GnbHandle->Address.Address.Bus;
  SecondaryBus = (UINT8)(GnbHandle->Address.Address.Bus + 1);
  *DsBus       = SecondaryBus;
  SubBusNum    = (UINT8)(GnbHandle->Address.Address.Bus + 1);
  NewValue = 0;
  NewValue |= PrimaryBus;
  NewValue |= (SecondaryBus << 8);
  NewValue |= (SubBusNum << 16);
  NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &NewValue, 0);

  return OldValue;
}

/**
 * Root bridge Unenum Down stream device
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[out] OldRegValue    Old Register Value
 * 
 * @retval
 */

VOID
RbUnEnumDs (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT32                     OldRegValue
)
{
  UINT32                            SmnAddress;
  
  SmnAddress = ConvertPciePortAddress2 (
                 SMN_PCIE0_SUB_BUS_NUMBER_LATENCY_HYGX,
                 GnbHandle,
                 Engine
                 );
  NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &OldRegValue, 0);
  
}

VOID
PcieRxIctrl (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      Value
)
{
  RwPhyLaneReg (
    GnbHandle,
    Engine,
    0xFF,
    LANE_ANA_RX_MISC_OVRD_4,
    (UINT16)(~(BIT4 | BIT5 | BIT6)),
    (UINT16)(Value << 4)
    );
  RwPhyLaneReg (
    GnbHandle,
    Engine,
    0xFF,
    LANE_ANA_RX_MISC_OVRD_7,
    (UINT16)(~BIT1),
    (UINT16)(BIT1)
    );
}
VOID
PcieRxFpk (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT8                      Value
)
{
  RwPhyLaneReg (
    GnbHandle,
    Engine,
    0xFF,
    LANE_ANA_RX_CAL_MUXA,
    (UINT16)(~(BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7)),
    (UINT16)(Value << 2)
    );
  RwPhyLaneReg (
    GnbHandle,
    Engine,
    0xFF,
    LANE_ANA_RX_MISC_OVRD_7,
    (UINT16)(~BIT0),
    (UINT16)(BIT0)
    );
}

EFI_STATUS
PcieMatchEqCfg (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     HYGON_CPM_EQ_CONFIG_TABLE  *EqCfgTable,
  IN     UINT32                     StartIndex,
  IN     UINT8                      Phase,                    
  OUT    UINT32                     *Index
  )
{
  UINT32                     VidDid, SubSystemVidDid;
  UINT32                     i;
  UINT32                     ForceSearchIndex = 0xffffffff;
  UINT32                     SpecifiedSearchIndex = 0xffffffff;
  UINT32                     OldRegValue;
  UINT8                      DsBus;

  if(Index == NULL){
    return EFI_INVALID_PARAMETER;
  }

  for (i = StartIndex; (EqCfgTable->EqCfgList[i].Flag & EQ_TABLE_END) != EQ_TABLE_END; i++) {
    if (((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) != PCIE_EQ_TYPE) ||
        //(EqCfgTable->EqCfgList[i].Flag & FLAG_THROW) ||
        (GnbHandle->SocketId != EqCfgTable->EqCfgList[i].Socket)) {          
      continue;
    }

    if ((Engine->EngineData.StartLane == EqCfgTable->EqCfgList[i].StartLane) && 
        (Engine->EngineData.EndLane == EqCfgTable->EqCfgList[i].EndLane)){         
      if (EqCfgTable->EqCfgList[i].Flag & FLAG_SPECIFIED){
        
        OldRegValue = RbEnumDs (GnbHandle, Engine, &DsBus);
        IDS_HDT_CONSOLE (GNB_TRACE, "RbEnumDs=%x\n", DsBus);
        ReadDsDevCfg (DsBus, PCIE_VIDDID_OFT, PCIE_VIDDID_SIZE, &VidDid);
        IDS_HDT_CONSOLE (GNB_TRACE, "Viddid=%x\n", VidDid);
        ReadDsDevCfg (DsBus, PCIE_SSVIDDID_OFT, PCIE_SSVIDDID_SIZE, &SubSystemVidDid);
        RbUnEnumDs (GnbHandle, Engine, OldRegValue);
        
        if ((VidDid != 0xffffffff) && (VidDid == EqCfgTable->EqCfgList[i].VidDid)) {
          if (EqCfgTable->EqCfgList[i].SubSysVidDid != 0xffffffff) {
            if (SubSystemVidDid == EqCfgTable->EqCfgList[i].SubSysVidDid) {
              SpecifiedSearchIndex = i;
              IDS_HDT_CONSOLE (GNB_TRACE, "Table Matched: SpecifiedSearchIndex=%x\n", SpecifiedSearchIndex);
              break;
            }                
          } else { 
            SpecifiedSearchIndex = i;
            IDS_HDT_CONSOLE (GNB_TRACE, "Table Matched: SpecifiedSearchIndex=%x\n", SpecifiedSearchIndex);
            break;
          }            
        }
        //EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW; 
      } else if (EqCfgTable->EqCfgList[i].Flag & FLAG_FORCE) {             
        ForceSearchIndex = i;
        IDS_HDT_CONSOLE (GNB_TRACE, "Table Matched: ForceSearchIndex=%x\n", ForceSearchIndex);
        break;
      }
    }
  }
  if (SpecifiedSearchIndex != 0xffffffff) {
    *Index = SpecifiedSearchIndex;
    return EFI_SUCCESS;
  }
  if (ForceSearchIndex != 0xffffffff) {
    *Index = ForceSearchIndex;
    return EFI_SUCCESS;
  }
  return EFI_NOT_FOUND;
}
/**
 * Get The Index of EQ Configuration Table witch match current PCIe Engine
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  EqCfgTable     Pointer to EQ Configuration Table
 * @param[in]  StartIndex     EQ Configuration Table Start Index for Search
 * @param[in]  Phase          Before PCIe Training = 0, After PCIe Training = 0
 * @param[out] Index          Index of EQ Configuration Table witch match current PCIe Engine
 * 
 * @retval    EFI_STATUS
 */

EFI_STATUS
PcieEqCfgInitPhase (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_WRAPPER_CONFIG        *Wrapper
)
{
  EFI_PEI_SERVICES            **PeiServices;
  EFI_STATUS                  Status;
  HYGON_CPM_EQ_CONFIG_TABLE*  EqCfgTable;
  EFI_PEI_EQ_TABLE_PPI        *EqTablePpi;
  UINT32                      i;

  PeiServices = (EFI_PEI_SERVICES**)GetPeiServicesTablePointer ();

  Status = (*PeiServices)->LocatePpi (
                              PeiServices,
                              &gEqTablePpiGuid,
                              0,
                              NULL,
                              (VOID**)&EqTablePpi
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = EqTablePpi->GetCfgTable (PeiServices, &EqCfgTable);
  if (EFI_ERROR (Status)) {
    return Status;
  }
   
  for (i = 0; (EqCfgTable->EqCfgList[i].Flag & EQ_TABLE_END) != EQ_TABLE_END; i++) { 
    if ((GnbHandle->SocketId == EqCfgTable->EqCfgList[i].Socket) &&
        (Wrapper->StartPhyLane <= EqCfgTable->EqCfgList[i].StartLane) &&
        (Wrapper->EndPhyLane >= EqCfgTable->EqCfgList[i].EndLane))
      {
      switch (EqCfgTable->EqCfgList[i].Type) {
        case PCIE_GEN3_TX_FORCE_DS_INITIAL_PRESET:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen3_tx_force_ds_initial_preset\n");
          SetPcieInitialPreset (GnbHandle, Wrapper, 3, 0, (UINT8)EqCfgTable->EqCfgList[i].CfgValue[0]);      
          break;            
        case PCIE_GEN3_TX_FORCE_US_INITIAL_PRESET:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen3_tx_force_us_initial_preset\n");
          SetPcieInitialPreset (GnbHandle, Wrapper, 3, 1, (UINT8)EqCfgTable->EqCfgList[i].CfgValue[0]);        
          break;           
        case PCIE_GEN4_TX_FORCE_DS_INITIAL_PRESET: 
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen4_tx_force_ds_initial_preset\n");
          SetPcieInitialPreset (GnbHandle, Wrapper, 4, 0, (UINT8)EqCfgTable->EqCfgList[i].CfgValue[0]);   
          break;           
        case PCIE_GEN4_TX_FORCE_US_INITIAL_PRESET:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen4_tx_force_us_initial_preset\n");
          SetPcieInitialPreset (GnbHandle, Wrapper, 4, 1, (UINT8)EqCfgTable->EqCfgList[i].CfgValue[0]);   
          break; 
        case PCIE_GEN5_TX_FORCE_DS_INITIAL_PRESET: 
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen5_tx_force_ds_initial_preset\n");
          SetPcieInitialPreset (GnbHandle, Wrapper, 5, 0, (UINT8)EqCfgTable->EqCfgList[i].CfgValue[0]);   
          break;           
        case PCIE_GEN5_TX_FORCE_US_INITIAL_PRESET:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen5_tx_force_us_initial_preset\n");
          SetPcieInitialPreset (GnbHandle, Wrapper, 5, 1, (UINT8)EqCfgTable->EqCfgList[i].CfgValue[0]);
          break;   
      }
    }
  }

  return EFI_SUCCESS;
}

/**
 * Configure EQ for current PCIe Engine 
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  EqCfgTable     Pointer to EQ Configuration Table
 * @param[in]  Index          Index of EQ Configuration Table witch match current PCIe Engine
 * @param[in]  Phase          Before PCIe Training = 0, After PCIe Training = 0
 * @param[out] RequestRedoEq  Request RedoEq status
 * 
 * @retval    EFI_STATUS
 */

EFI_STATUS
DoPcieEqCfg (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     HYGON_CPM_EQ_CONFIG_TABLE  *EqCfgTable,
  IN     UINT32                     Index,
  IN     PCIE_TRAINING_PHASE        Phase,
  OUT    BOOLEAN                    *RequestRedoEq    
  )
{
  PCIe_WRAPPER_CONFIG               *Wrapper;
  UINT8                             MappingPortID;
  UINT8                             DxioType;
  UINT8                             PreCursor, MainCursor, PostCursor;

    
  if (RequestRedoEq == NULL) {
    return EFI_INVALID_PARAMETER;
 }
    
  *RequestRedoEq = FALSE;
  Wrapper = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  DxioType  = (Engine->Type.Port.PortId < 8) ? 2 : 3;

  switch (Phase) {
    case AFTER_RECONFIG:
      switch (EqCfgTable->EqCfgList[Index].Type) {
        case PCIE_GEN3_TX_FORCE_DS_CURSOR_BEFORE_TRAINING:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen3_tx_force_ds_cursor_before_training\n");
          PreCursor = GET_PRE_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          MainCursor = GET_MAIN_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PostCursor = GET_POST_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PcieGen3DsTxCursor (GnbHandle, Engine, PreCursor, MainCursor, PostCursor);
          break;
        case PCIE_GEN4_TX_FORCE_DS_CURSOR_BEFORE_TRAINING:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen4_tx_force_ds_cursor_before_training\n");
          PreCursor = GET_PRE_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          MainCursor = GET_MAIN_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PostCursor = GET_POST_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PcieGen4DsTxCursor (GnbHandle, Engine, PreCursor, MainCursor, PostCursor);
          break;
        case PCIE_GEN5_TX_FORCE_DS_CURSOR_BEFORE_TRAINING:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen5_tx_force_ds_cursor_before_training\n");
          PreCursor = GET_PRE_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          MainCursor = GET_MAIN_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PostCursor = GET_POST_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PcieGen5DsTxCursor (GnbHandle, Engine, PreCursor, MainCursor, PostCursor);
          break;
        case PCIE_GEN3_TX_FORCE_US_PRESET_BEFORE_TRAINING:
          PcieGen3UsPreset (GnbHandle, Engine, (UINT8)EqCfgTable->EqCfgList[Index].CfgValue[0]);
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen3_tx_force_us_preset_before_training\n");
          break;
        case PCIE_GEN4_TX_FORCE_US_PRESET_BEFORE_TRAINING:
          PcieGen4UsPreset (GnbHandle, Engine, (UINT8)EqCfgTable->EqCfgList[Index].CfgValue[0]);
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen4_tx_force_us_preset_before_training\n");
          break;
        case PCIE_GEN5_TX_FORCE_US_PRESET_BEFORE_TRAINING:
          PcieGen5UsPreset (GnbHandle, Engine, (UINT8)EqCfgTable->EqCfgList[Index].CfgValue[0]);
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen5_tx_force_us_preset_before_training\n");
          break;
        case PCIE_RX_TERM_OFFSET:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_rx_term_offset\n");
          PcieRxTermOffset (GnbHandle, Engine, (UINT8)EqCfgTable->EqCfgList[Index].CfgValue[0]);
          break; 
        case PCIE_TXUP_TERM_OFFSET:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_txup_term_offset\n");
          PcieTxupTermOffset (GnbHandle, Engine, (UINT8)EqCfgTable->EqCfgList[Index].CfgValue[0]);
          break; 
        case PCIE_TXDN_TERM_OFFSET:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_txdn_term_offset\n");
          PcieTxdnTermOffset (GnbHandle, Engine, (UINT8)EqCfgTable->EqCfgList[Index].CfgValue[0]);
          break;
        default:
          return EFI_UNSUPPORTED;
      }
      break;
    case AFTER_TRAINING:
      switch (EqCfgTable->EqCfgList[Index].Type) {
        case PCIE_GEN3_TX_FORCE_DS_CURSOR:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen3_tx_force_ds_cursor\n");
          PreCursor = GET_PRE_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          MainCursor = GET_MAIN_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PostCursor = GET_POST_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PcieGen3DsTxCursor (GnbHandle, Engine, PreCursor, MainCursor, PostCursor);
          break;
        case PCIE_GEN4_TX_FORCE_DS_CURSOR:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen4_tx_force_ds_cursor\n");
          PreCursor = GET_PRE_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          MainCursor = GET_MAIN_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PostCursor = GET_POST_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PcieGen4DsTxCursor (GnbHandle, Engine, PreCursor, MainCursor, PostCursor);
          break;
        case PCIE_GEN5_TX_FORCE_DS_CURSOR:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen5_tx_force_ds_cursor\n");
          PreCursor = GET_PRE_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          MainCursor = GET_MAIN_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PostCursor = GET_POST_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PcieGen5DsTxCursor (GnbHandle, Engine, PreCursor, MainCursor, PostCursor);
          break;
        case PCIE_GEN3_TX_FORCE_US_CURSOR:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen3_tx_force_us_cursor\n");
          PreCursor = GET_PRE_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          MainCursor = GET_MAIN_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PostCursor = GET_POST_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PcieGen3UsTxCursor (GnbHandle, Engine, PreCursor, MainCursor, PostCursor);
          break;
        case PCIE_GEN4_TX_FORCE_US_CURSOR:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen4_tx_force_us_cursor\n");
          PreCursor = GET_PRE_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          MainCursor = GET_MAIN_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PostCursor = GET_POST_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PcieGen4UsTxCursor (GnbHandle, Engine, PreCursor, MainCursor, PostCursor);
          break;
        case PCIE_GEN5_TX_FORCE_US_CURSOR:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen5_tx_force_us_cursor\n");
          PreCursor = GET_PRE_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          MainCursor = GET_MAIN_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PostCursor = GET_POST_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]);
          PcieGen5UsTxCursor (GnbHandle, Engine, PreCursor, MainCursor, PostCursor);
          break;
        case PCIE_GEN3_TX_FORCE_US_PRESET:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen3_tx_force_us_preset\n");
          PcieGen3UsPreset (GnbHandle, Engine, (UINT8)EqCfgTable->EqCfgList[Index].CfgValue[0]);           
          break;
        case PCIE_GEN4_TX_FORCE_US_PRESET:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen4_tx_force_us_preset\n");
          PcieGen4UsPreset (GnbHandle, Engine, (UINT8)EqCfgTable->EqCfgList[Index].CfgValue[0]);
          break;
        case PCIE_GEN5_TX_FORCE_US_PRESET:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen5_tx_force_us_preset\n");
          PcieGen5UsPreset (GnbHandle, Engine, (UINT8)EqCfgTable->EqCfgList[Index].CfgValue[0]);
          break;
        case PCIE_RX_SSC_OFF_PHUG1:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_rx_ssc_off_phug1\n");
          PcieRxSscOffPhug1 (GnbHandle, Engine, (UINT8)EqCfgTable->EqCfgList[Index].CfgValue[0]); 
          break;
        case PCIE_RX_SSC_OFF_FRUG1:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_rx_ssc_off_frug1:\n");
          PcieRxSscOffFrug1 (GnbHandle, Engine, (UINT8)EqCfgTable->EqCfgList[Index].CfgValue[0]); 
          break;
        case PCIE_GEN5_RX_ICTRL:
          if (PcieCurrentLinkSpeed(GnbHandle, Engine) == 5) {
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen5_rx_ictrl\n");
            PcieRxIctrl (GnbHandle, Engine, (UINT8)EqCfgTable->EqCfgList[Index].CfgValue[0]);
          } 
          break;
        case PCIE_GEN5_RX_FPK:
          if (PcieCurrentLinkSpeed(GnbHandle, Engine) == 5) {
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen5_rx_fpk:\n");
            PcieRxFpk (GnbHandle, Engine, (UINT8)EqCfgTable->EqCfgList[Index].CfgValue[0]);
          } 
          break; 
        default:
          return EFI_UNSUPPORTED;
      }
      break;
    default:
      return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
 * Get EQ Configuration Table
 *
 * @param[out]  EqCfgTable     Pointer to EQ Configuration Table
 * 
 * @retval    EFI_STATUS
 */

EFI_STATUS
GetEqCfgTable (
    OUT HYGON_CPM_EQ_CONFIG_TABLE**  EqCfgTable
)
{
  EFI_PEI_SERVICES            **PeiServices;
  EFI_STATUS                  Status;
  HYGON_CPM_EQ_CONFIG_TABLE*  pEqCfgTable;
  EFI_PEI_EQ_TABLE_PPI        *EqTablePpi;

  PeiServices = (EFI_PEI_SERVICES**)GetPeiServicesTablePointer ();

  Status = (*PeiServices)->LocatePpi (
                              PeiServices,
                              &gEqTablePpiGuid,
                              0,
                              NULL,
                              (VOID**)&EqTablePpi
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = EqTablePpi->GetCfgTable (PeiServices, &pEqCfgTable);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  EqTablePpi->DumpEqCfgTable (PeiServices, pEqCfgTable, PCIE_EQ_TYPE);
  *EqCfgTable = pEqCfgTable;

  return EFI_SUCCESS;
}

/**
 * Set Pcie Strap
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Wrapper        Pointer to Wrapper config descriptor
 * @param[in]  Address        Strap Address 
 * @param[in]  Value          Set value
 *
 * @retval    EFI_STATUS
 *            EFI_INVALID_PARAMETER
 */

VOID
SetPcieStrap (
  IN     GNB_HANDLE           *GnbHandle,
  IN     PCIe_WRAPPER_CONFIG  *Wrapper,
  IN     UINT32               Address,
  IN     UINT32               Value
)
{
  UINT32                      StrapIndexPort;
  UINT32                      StrapDataPort;
                     
  StrapIndexPort = (Wrapper->WrapId == 2) ? NBIO0_PCIE2_STRAP_INDEX_ADDRESS : \
                   ((Wrapper->WrapId == 1) ? PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE1_STRAP_INDEX_ADDRESS) : \
                    PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE0_STRAP_INDEX_ADDRESS));
  StrapDataPort  = (Wrapper->WrapId == 2) ? NBIO0_PCIE2_STRAP_DATA_ADDRESS : \
                   ((Wrapper->WrapId == 1) ? PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE1_STRAP_DATA_ADDRESS) : \
                    PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE0_STRAP_DATA_ADDRESS));
  NbioRegisterWrite (GnbHandle, TYPE_SMN, StrapIndexPort, &Address, 0);
  NbioRegisterWrite (GnbHandle, TYPE_SMN, StrapDataPort, &Value, 0);
}

/**
 * Set Pcie Port Initial Preset for current PCIe Engine after PCIe training 
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Wrapper        Pointer to Wrapper config descriptor
 * @param[in]  Speed          PCIe Speed (1=GEN1, 2=GEN2, 3=GEN3...)
 * @param[in]  Stream         Down Stream(0) or Up Stream(1)
 * @param[in]  Preset         PCIe Preset Value
 *
 * @retval    EFI_STATUS
 *            EFI_INVALID_PARAMETER
 */
EFI_STATUS
SetPcieInitialPreset (
  IN     GNB_HANDLE           *GnbHandle,
  IN     PCIe_WRAPPER_CONFIG  *Wrapper,
  IN     UINT8                Speed,
  IN     UINT8                Stream,
  IN     UINT8                Preset
)
{
  UINT32                      StrapAddress;

  if (Speed == 3) {
    StrapAddress = (Stream == 0) ? STRAP_BIF_PCIE_LANE_EQ_DN_PORT_TX_PRESET_ADDR : \
                                   STRAP_BIF_PCIE_LANE_EQ_UP_PORT_TX_PRESET_ADDR;
  } else if (Speed == 4) {
    StrapAddress = (Stream == 0) ? STRAP_BIF_LANE_EQUALIZATION_CNTL_DSP_16GT_TX_PRESET_ADDR : \
                                   STRAP_BIF_LANE_EQUALIZATION_CNTL_USP_16GT_TX_PRESET_ADDR;
  } else if (Speed == 5) {
    StrapAddress = (Stream == 0) ? STRAP_BIF_LANE_EQUALIZATION_CNTL_DSP_32GT_TX_PRESET_ADDR : \
                                   STRAP_BIF_LANE_EQUALIZATION_CNTL_USP_32GT_TX_PRESET_ADDR;
  } else {
    return EFI_INVALID_PARAMETER;
  }
  if (Stream > 1)
    return EFI_INVALID_PARAMETER;
  SetPcieStrap (GnbHandle, Wrapper, StrapAddress, Preset);

  return EFI_SUCCESS;
}

/**
 * Common Configure EQ for current PCIe Engine Before PCIe training 
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    EFI_STATUS
 */

EFI_STATUS
CommonPcieEqConfigurationBeforeTraining (
    IN     GNB_HANDLE           *GnbHandle,
    IN     PCIe_ENGINE_CONFIG   *Engine
)
{
  return EFI_SUCCESS;
}

/**
 * Customer Configure EQ for current PCIe Engine
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    EFI_STATUS
 */
EFI_STATUS
CustomerPcieEqConfiguration (
  IN     GNB_HANDLE           *GnbHandle,
  IN     PCIe_ENGINE_CONFIG   *Engine,
  IN     PCIE_TRAINING_PHASE  Phase,
  OUT    BOOLEAN              *RequestRedoEq
  )
{
  EFI_STATUS                  Status;
  HYGON_CPM_EQ_CONFIG_TABLE*  EqCfgTable;
  UINT32                      StartIndex = 0;
  UINT32                      SearchIndex;
  BOOLEAN                     RedoEq = FALSE;
  BOOLEAN                     RedoEqRecord = FALSE;

  //get Eq Cfg Table
  Status = GetEqCfgTable (&EqCfgTable);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  do {
    //match cfg table?
    Status = PcieMatchEqCfg (GnbHandle, Engine, EqCfgTable, StartIndex, Phase, &SearchIndex);
    if (!EFI_ERROR(Status)) {
      StartIndex = SearchIndex + 1;
      //setting Eq
      DoPcieEqCfg (GnbHandle, Engine, EqCfgTable, SearchIndex, Phase, &RedoEq);
      RedoEqRecord |= RedoEq;
    } 
  } while (!EFI_ERROR(Status));
  
  if (RequestRedoEq != NULL) {
   *RequestRedoEq = RedoEqRecord;
  }

  return EFI_SUCCESS;
}


/**
 * Customer Configure EQ for current Gnbhandle before training 
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * 
 * @retval    VOID
 */
VOID
CustomerPcieEqCfgBeforeTraining (
    IN     GNB_HANDLE           *GnbHandle
)
{
  PCIe_ENGINE_CONFIG            *PcieEngine;
  PCIe_WRAPPER_CONFIG           *PcieWrapper;

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      CustomerPcieEqConfiguration (GnbHandle, PcieEngine, AFTER_RECONFIG, NULL);
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
}

/**
 * Common Configure EQ for current PCIe Engine after PCIe training 
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    EFI_STATUS
 */
EFI_STATUS
CommonPcieEqCfgAfterTraining (
    IN     GNB_HANDLE           *GnbHandle,
    IN     PCIe_ENGINE_CONFIG   *Engine,
    OUT    BOOLEAN              *RequestRedoEq
)
{
  UINT32                        SmnAddress;
  PCIe_WRAPPER_CONFIG           *PcieWrapper;

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  //set Gen5 CDR 1-1
  if ((PcieCurrentLinkSpeed (GnbHandle,Engine) == 5) || PcdGetBool (PcdPcieSITestMode)) {
    RwPhyLaneReg (GnbHandle, Engine, 0xFF, LANE_DIG_RX_CDR_CNTL_3_HYEX, (UINT16)(~(BIT3 | BIT4 | BIT5)), (UINT16)(1 << 3));
    RwPhyLaneReg (GnbHandle, Engine, 0xFF, LANE_DIG_RX_CDR_CNTL_4_HYEX, (UINT16)(~(BIT0 | BIT1 | BIT2)), (UINT16)1);
    *RequestRedoEq = TRUE;
  }
  SmnAddress = ConvertPcieCoreAddress (PCIE0_PCIE_HW_DEBUG_ADDRESS_HYEX, GnbHandle, PcieWrapper);
  NbioRegisterRMW (GnbHandle, TYPE_SMN, SmnAddress, (UINT32)(~BIT13), 0, 0);
  
  return EFI_SUCCESS;
}

/**
 * Common Configure EQ for current PCIe Engine after PCIe training 
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    EFI_STATUS
 */
EFI_STATUS
CustomerPcieEqCfgAfterTraining (
    IN     GNB_HANDLE           *GnbHandle,
    IN     PCIe_ENGINE_CONFIG   *Engine,
    OUT    BOOLEAN              *RequestRedoEq
)
{
  CustomerPcieEqConfiguration (GnbHandle, Engine, AFTER_TRAINING, RequestRedoEq);
  return EFI_SUCCESS;
}