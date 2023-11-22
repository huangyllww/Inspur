/* $NoKeywords:$ */

/**
 * @file
 *
 * Fabric Register Access Methods.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: Fabric
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
#include <HGPI.h>
#include <Ids.h>
#include <Library/HygonBaseLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/PciLib.h>
#include <Library/HygonS3SaveLib.h>
#include <FabricRegistersST.h>
#include "FabricRegisterAccStLib.h"
#include <Filecode.h>

#define FILECODE  LIBRARY_FABRICREGISTERACCSTLIB_FABRICREGISTERACCSTLIB_FILECODE

#define USE_SMN_ACCESS_CDD_DF  0

#if USE_SMN_ACCESS_CDD_DF == 1
  #define  FABRIC_SMN_BASE_ADDR  0x1C000
  #define  CDD_DF_SMN_SPACE(CDDINDEX)  (0x80000000 + (CDDINDEX << 28) + FABRIC_SMN_BASE_ADDR)
#endif

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

/*
 *  Routine to check Group mode.
 */
BOOLEAN
IsGroupMode (
  )
{
  PCI_ADDR           PciAddr;
  PIE_CTRL_REGISTER  PIECTRL;

  PciAddr.AddressValue     = 0;
  PciAddr.Address.Device   = SOCKET0_IODIE_DEVNUM_IN_GROUP_MODE;
  PciAddr.Address.Function = (UINT32)PIE_CTRL_FUNC;
  PciAddr.Address.Register = (UINT32)PIE_CTRL_REG;
  PIECTRL.Value = PciRead32 (PciAddr.AddressValue);
  if (PIECTRL.Value != 0xFFFFFFFF) {
    return (BOOLEAN)PIECTRL.Field.GroupConfigEnable;
  }

  PciAddr.AddressValue     = 0;
  PciAddr.Address.Device   = SOCKET0_IODIE_DEVNUM_IN_FLAT_MODE;
  PciAddr.Address.Function = (UINT32)PIE_CTRL_FUNC;
  PciAddr.Address.Register = (UINT32)PIE_CTRL_REG;
  PIECTRL.Value = PciRead32 (PciAddr.AddressValue);
  ASSERT (PIECTRL.Value != 0xFFFFFFFF);
  return (BOOLEAN)PIECTRL.Field.GroupConfigEnable;
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to set FCAC die id for group broadcast access
 *
 * @param[in] PciAddr            DF Pci Addr
 * @param[in] Die                Die number to set
 */
VOID
SetFcacCfgDieID (
  IN       PCI_ADDR   PciAddr,
  IN       UINTN      Die
  )
{
  FABRIC_CONFIG_ACC_CTRL_REGISTER  FCAC;
  PCI_ADDR                         LocalPciAddr;

  LocalPciAddr.AddressValue     = PciAddr.AddressValue;
  LocalPciAddr.Address.Function = (UINT32)FCAC_FUNC;
  LocalPciAddr.Address.Register = (UINT32)FCAC_REG;
  FCAC.Value = PciRead32 (LocalPciAddr.AddressValue);
  FCAC.Field.CfgRegDieID = (UINT32)Die;
  PciWrite32 (LocalPciAddr.AddressValue, FCAC.Value);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to read a register.
 *
 * @param[in] Socket             Processor socket to read from
 * @param[in] Die                Die number on Socket to read from
 * @param[in] Function           Function number to read from
 * @param[in] Offset             Register to read
 * @param[in] Instance           Instance ID of the target fabric device
 * @retval    Current value of the target register
 */
UINT32
FabricRegisterAccRead (
  IN       UINTN  Socket,
  IN       UINTN  LogicalDie,
  IN       UINTN  Function,
  IN       UINTN  Offset,
  IN       UINTN  Instance
  )
{
  UINT32                               RegisterValue;
  PCI_ADDR                             PciAddr;
  FABRIC_IND_CFG_ACCESS_ADDR_REGISTER  FICAA3;
  BOOLEAN                              GroupMode;

  ASSERT (Socket < MAX_SOCKETS_SUPPORTED);
  ASSERT (LogicalDie < MAX_IODIE_PER_SOCKET);
  ASSERT (Function < 8);
  ASSERT (Offset < 0x400);
  ASSERT ((Offset & 3) == 0);
  ASSERT (Instance <= FABRIC_REG_ACC_BC);

  GroupMode = IsGroupMode ();

  PciAddr.AddressValue   = 0;
  PciAddr.Address.Device = FabricRegisterAccGetPciDeviceNumberOfDie (Socket, LogicalDie, GroupMode);

  if (Instance == FABRIC_REG_ACC_BC) {
    if (GroupMode) {
      SetFcacCfgDieID (PciAddr, LogicalDie);
    }

    PciAddr.Address.Function = (UINT32)Function;
    PciAddr.Address.Register = (UINT32)Offset;
    RegisterValue = PciRead32 (PciAddr.AddressValue);
    if (GroupMode) {
      SetFcacCfgDieID(PciAddr, GROUPMODE_DIE);
    }
  } else {
    PciAddr.Address.Function = FICAA3_FUNC;
    PciAddr.Address.Register = FICAA3_REG;
    FICAA3.Value = 0;
    FICAA3.Field.CfgRegInstAccEn  = 1;
    FICAA3.Field.IndCfgAccRegNum  = ((UINT32)Offset) >> 2;
    FICAA3.Field.IndCfgAccFuncNum = (UINT32)Function;
    FICAA3.Field.CfgRegInstID     = (UINT32)Instance;
    if (GroupMode) {
      FICAA3.Field.CfgRegDieID = (UINT32)LogicalDie;
    }

    PciWrite32 (PciAddr.AddressValue, FICAA3.Value);

    PciAddr.Address.Function = FICAD3_LO_FUNC;
    PciAddr.Address.Register = FICAD3_LO_REG;
    RegisterValue = PciRead32 (PciAddr.AddressValue);
  }

  return RegisterValue;
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to write a register.
 *
 * @param[in] Socket             Processor socket to read from
 * @param[in] Die                Die number on Socket to read from
 * @param[in] Function           Function number to read from
 * @param[in] Offset             Register to read
 * @param[in] Instance           Instance ID of the target fabric device
 * @param[in] Value              Value to write
 * @param[in] LogForS3           Whether or not this write should be logged for playback on S3
 */
VOID
FabricRegisterAccWrite (
  IN       UINTN    Socket,
  IN       UINTN    LogicalDie,
  IN       UINTN    Function,
  IN       UINTN    Offset,
  IN       UINTN    Instance,
  IN       UINT32   Value,
  IN       BOOLEAN  LogForS3
  )
{
  UINT32                               RegisterValue;
  PCI_ADDR                             PciAddr;
  FABRIC_IND_CFG_ACCESS_ADDR_REGISTER  FICAA3;
  BOOLEAN                              GroupMode;

  ASSERT (Socket < MAX_SOCKETS_SUPPORTED);
  ASSERT (LogicalDie < MAX_IODIE_PER_SOCKET);
  ASSERT (Function < 8);
  ASSERT (Offset < 0x400);
  ASSERT ((Offset & 3) == 0);
  ASSERT (Instance <= FABRIC_REG_ACC_BC);

  GroupMode = IsGroupMode ();

  RegisterValue = Value;
  PciAddr.AddressValue   = 0;
  PciAddr.Address.Device = FabricRegisterAccGetPciDeviceNumberOfDie (Socket, LogicalDie, GroupMode);

  if (Instance == FABRIC_REG_ACC_BC) {
    if (GroupMode) {
      SetFcacCfgDieID (PciAddr, LogicalDie);
    }

    PciAddr.Address.Function = (UINT32)Function;
    PciAddr.Address.Register = (UINT32)Offset;
    PciWrite32 (PciAddr.AddressValue, RegisterValue);
    if (LogForS3) {
      HygonS3SaveScriptPciWrite (AccessWidth32, PciAddr.AddressValue, &RegisterValue);
    }
    if (GroupMode) {
      SetFcacCfgDieID(PciAddr, GROUPMODE_DIE);
    }
  } else {
    PciAddr.Address.Function = FICAA3_FUNC;
    PciAddr.Address.Register = FICAA3_REG;
    FICAA3.Value = 0;
    FICAA3.Field.CfgRegInstAccEn  = 1;
    FICAA3.Field.IndCfgAccRegNum  = ((UINT32)Offset) >> 2;
    FICAA3.Field.IndCfgAccFuncNum = (UINT32)Function;
    FICAA3.Field.CfgRegInstID     = (UINT32)Instance;
    if (GroupMode) {
      FICAA3.Field.CfgRegDieID = (UINT32)LogicalDie;
    }

    PciWrite32 (PciAddr.AddressValue, FICAA3.Value);
    if (LogForS3) {
      HygonS3SaveScriptPciWrite (AccessWidth32, PciAddr.AddressValue, &FICAA3.Value);
    }

    PciAddr.Address.Function = FICAD3_LO_FUNC;
    PciAddr.Address.Register = FICAD3_LO_REG;
    PciWrite32 (PciAddr.AddressValue, RegisterValue);
    if (LogForS3) {
      HygonS3SaveScriptPciWrite (AccessWidth32, PciAddr.AddressValue, &RegisterValue);
    }
  }
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to write a register.
 *
 * @param[in] Socket             Processor socket to read from
 * @param[in] Die                Die number on Socket to read from
 * @param[in] Function           Function number to read from
 * @param[in] Offset             Register to read
 * @param[in] Instance           Instance ID of the target fabric device
 * @param[in] NandValue          Value to NAND with the current register value
 * @param[in] OrValue            Value to OR with the current register value
 * @param[in] LogForS3           Whether or not this write should be logged for playback on S3
 * @retval    Value written to target register if interested
 */
UINT32
FabricRegisterAccRMW (
  IN       UINTN    Socket,
  IN       UINTN    LogicalDie,
  IN       UINTN    Function,
  IN       UINTN    Offset,
  IN       UINTN    Instance,
  IN       UINT32   NandValue,
  IN       UINT32   OrValue,
  IN       BOOLEAN  LogForS3
  )
{
  UINT32  RegisterValue;

  ASSERT (Socket < MAX_SOCKETS_SUPPORTED);
  ASSERT (LogicalDie < MAX_IODIE_PER_SOCKET);
  ASSERT (Function < 8);
  ASSERT (Offset < 0x400);
  ASSERT ((Offset & 3) == 0);
  ASSERT (Instance <= FABRIC_REG_ACC_BC);

  RegisterValue = (FabricRegisterAccRead (Socket, LogicalDie, Function, Offset, Instance) & ~NandValue) | OrValue;
  FabricRegisterAccWrite (Socket, LogicalDie, Function, Offset, Instance, RegisterValue, LogForS3);
  return RegisterValue;
}

UINT32
FabricRegisterAccGetPciDeviceNumberOfDie (
  IN       UINTN   Socket,
  IN       UINTN   LogicalDie,
  IN       BOOLEAN GroupMode
  )
{
  UINT32  DeviceNumber;
  UINT32  CddNum;
  UINT32  CpuModel;

  CpuModel = GetHygonSocModel();

  if(GroupMode) {
    // Group mode
    DeviceNumber = (UINT32)Socket + SOCKET0_IODIE_DEVNUM_IN_GROUP_MODE;
  } else {
    //Flatting mode
    if (CpuModel == HYGON_EX_CPU) {
      if (LogicalDie == FABRIC_ID_IO_DIE_NUM) {
          DeviceNumber = (UINT32)Socket * 5 + SOCKET0_IODIE_DEVNUM_IN_FLAT_MODE;
      } else {
          CddNum = (UINT32)LogicalDie - FABRIC_ID_CDD0_DIE_NUM;
          DeviceNumber = (UINT32)Socket * 5 + CddNum + SOCKET0_CDD0_DEVNUM_IN_FLAT_MODE_HYEX;
      }
    }
    
    if (CpuModel == HYGON_GX_CPU) {
      if (LogicalDie < FABRIC_ID_CDD0_DIE_NUM) {
        DeviceNumber = (UINT32)Socket * 8 + SOCKET0_IODIE_DEVNUM_IN_FLAT_MODE + (UINT32)LogicalDie;
      } else {
        CddNum = (UINT32)LogicalDie - FABRIC_ID_CDD0_DIE_NUM;
        DeviceNumber = (UINT32)Socket * 8 + SOCKET0_CDD0_DEVNUM_IN_FLAT_MODE_HYGX + CddNum;
      }
    }
  }

  return DeviceNumber;
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to get CDD physical die ID
 */
UINTN
GetCddPhysicalDieId (
  IN       UINTN      LogicalDieId
  )
{
  UINTN  PhysicalDieId;
  UINT32 CpuModel;

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    return LogicalDieId;
  }

  switch (LogicalDieId) {
    case 4:
      PhysicalDieId = 4;
      break;
    case 5:
      PhysicalDieId = 9;
      break;
    case 6:
      PhysicalDieId = 8;
      break;
    case 7:
      PhysicalDieId = 5;
      break;
    default:
      PhysicalDieId = 0xFF;
  }

  return PhysicalDieId;
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to get CDD Logical die ID
 */
UINTN
GetCddLogicalDieId (
  IN       UINTN      PhysicalDieId
  )
{
  UINTN  LogicalDieId;
  UINT32 CpuModel;

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    return PhysicalDieId;
  }

  switch (PhysicalDieId) {
    case 4:
      LogicalDieId = 4;
      break;
    case 5:
      LogicalDieId = 7;
      break;
    case 8:
      LogicalDieId = 6;
      break;
    case 9:
      LogicalDieId = 5;
      break;
    default:
      LogicalDieId = 0xFF;
  }

  return LogicalDieId;
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to read a register.
 *
 * @param[in] Socket             Processor socket to read from
 * @param[in] Cdd                Cdd Index on Socket to read from
 * @param[in] Function           Function number to read from
 * @param[in] Offset             Register to read
 * @param[in] Instance           Instance ID of the target fabric device
 * @retval    Current value of the target register
 */
UINT32
CddFabricRegisterAccRead (
  IN       UINTN  Socket,
  IN       UINTN  Cdd,
  IN       UINTN  Function,
  IN       UINTN  Offset,
  IN       UINTN  Instance
  )
{
  UINT32                               RegisterValue;
  PCI_ADDR                             PciAddr;
  FABRIC_IND_CFG_ACCESS_ADDR_REGISTER  FICAA3;
  BOOLEAN                              GroupMode;
  UINTN                                CddDieId;

  ASSERT (Socket < MAX_SOCKETS_SUPPORTED);
  ASSERT (Cdd < MAX_CDDS_PER_SOCKET);
  ASSERT (Function < 8);
  ASSERT (Offset < 0x400);
  ASSERT ((Offset & 3) == 0);
  ASSERT (Instance <= FABRIC_REG_ACC_BC);

  GroupMode = IsGroupMode ();
  CddDieId  = FABRIC_ID_CDD0_DIE_NUM + Cdd;

  PciAddr.AddressValue   = 0;
  PciAddr.Address.Device = FabricRegisterAccGetPciDeviceNumberOfDie (Socket, GetCddLogicalDieId (CddDieId), GroupMode);

  if (Instance == FABRIC_REG_ACC_BC) {
    if (GroupMode) {
      SetFcacCfgDieID (PciAddr, CddDieId);
    }

    PciAddr.Address.Function = (UINT32)Function;
    PciAddr.Address.Register = (UINT32)Offset;
    RegisterValue = PciRead32 (PciAddr.AddressValue);
    if (GroupMode) {
      SetFcacCfgDieID(PciAddr, GROUPMODE_DIE);
    }
  } else {
    PciAddr.Address.Function = FICAA3_FUNC;
    PciAddr.Address.Register = FICAA3_REG;
    FICAA3.Value = 0;
    FICAA3.Field.CfgRegInstAccEn  = 1;
    FICAA3.Field.IndCfgAccRegNum  = ((UINT32)Offset) >> 2;
    FICAA3.Field.IndCfgAccFuncNum = (UINT32)Function;
    FICAA3.Field.CfgRegInstID     = (UINT32)Instance;
    if (GroupMode) {
      FICAA3.Field.CfgRegDieID = (UINT32)CddDieId;
    }

    PciWrite32 (PciAddr.AddressValue, FICAA3.Value);

    PciAddr.Address.Function = FICAD3_LO_FUNC;
    PciAddr.Address.Register = FICAD3_LO_REG;
    RegisterValue = PciRead32 (PciAddr.AddressValue);
  }

  return RegisterValue;
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to write a register.
 *
 * @param[in] Socket             Processor socket to read from
 * @param[in] Cdd                Cdd Index on Socket to read from
 * @param[in] Function           Function number to read from
 * @param[in] Offset             Register to read
 * @param[in] Instance           Instance ID of the target fabric device
 * @param[in] Value              Value to write
 * @param[in] LogForS3           Whether or not this write should be logged for playback on S3
 */
VOID
CddFabricRegisterAccWrite (
  IN       UINTN    Socket,
  IN       UINTN    Cdd,
  IN       UINTN    Function,
  IN       UINTN    Offset,
  IN       UINTN    Instance,
  IN       UINT32   Value,
  IN       BOOLEAN  LogForS3
  )
{
  UINT32                               RegisterValue;
  PCI_ADDR                             PciAddr;
  FABRIC_IND_CFG_ACCESS_ADDR_REGISTER  FICAA3;
  BOOLEAN                              GroupMode;
  UINTN                                CddDieId;

  ASSERT (Socket < MAX_SOCKETS_SUPPORTED);
  ASSERT (Cdd < MAX_CDDS_PER_SOCKET);
  ASSERT (Function < 8);
  ASSERT (Offset < 0x400);
  ASSERT ((Offset & 3) == 0);
  ASSERT (Instance <= FABRIC_REG_ACC_BC);

  GroupMode = IsGroupMode ();
  CddDieId  = FABRIC_ID_CDD0_DIE_NUM + Cdd;

  RegisterValue = Value;
  PciAddr.AddressValue   = 0;
  PciAddr.Address.Device = FabricRegisterAccGetPciDeviceNumberOfDie (Socket, GetCddLogicalDieId (CddDieId), GroupMode);

  if (Instance == FABRIC_REG_ACC_BC) {
    if (GroupMode) {
      SetFcacCfgDieID (PciAddr, CddDieId);
    }

    PciAddr.Address.Function = (UINT32)Function;
    PciAddr.Address.Register = (UINT32)Offset;
    PciWrite32 (PciAddr.AddressValue, RegisterValue);
    if (LogForS3) {
      HygonS3SaveScriptPciWrite (AccessWidth32, PciAddr.AddressValue, &RegisterValue);
    }
    if (GroupMode) {
      SetFcacCfgDieID(PciAddr, GROUPMODE_DIE);
    }
  } else {
    PciAddr.Address.Function = FICAA3_FUNC;
    PciAddr.Address.Register = FICAA3_REG;
    FICAA3.Value = 0;
    FICAA3.Field.CfgRegInstAccEn  = 1;
    FICAA3.Field.IndCfgAccRegNum  = ((UINT32)Offset) >> 2;
    FICAA3.Field.IndCfgAccFuncNum = (UINT32)Function;
    FICAA3.Field.CfgRegInstID     = (UINT32)Instance;
    if (GroupMode) {
      FICAA3.Field.CfgRegDieID = (UINT32)CddDieId;
    }

    PciWrite32 (PciAddr.AddressValue, FICAA3.Value);
    if (LogForS3) {
      HygonS3SaveScriptPciWrite (AccessWidth32, PciAddr.AddressValue, &FICAA3.Value);
    }

    PciAddr.Address.Function = FICAD3_LO_FUNC;
    PciAddr.Address.Register = FICAD3_LO_REG;
    PciWrite32 (PciAddr.AddressValue, RegisterValue);
    if (LogForS3) {
      HygonS3SaveScriptPciWrite (AccessWidth32, PciAddr.AddressValue, &RegisterValue);
    }
  }
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to write a register.
 *
 * @param[in] Socket             Processor socket to read from
 * @param[in] Cdd                Cdd Index on Socket to read from
 * @param[in] Function           Function number to read from
 * @param[in] Offset             Register to read
 * @param[in] Instance           Instance ID of the target fabric device
 * @param[in] NandValue          Value to NAND with the current register value
 * @param[in] OrValue            Value to OR with the current register value
 * @param[in] LogForS3           Whether or not this write should be logged for playback on S3
 * @retval    Value written to target register if interested
 */
UINT32
CddFabricRegisterAccRMW (
  IN       UINTN    Socket,
  IN       UINTN    Cdd,
  IN       UINTN    Function,
  IN       UINTN    Offset,
  IN       UINTN    Instance,
  IN       UINT32   NandValue,
  IN       UINT32   OrValue,
  IN       BOOLEAN  LogForS3
  )
{
  UINT32  RegisterValue;

  ASSERT (Socket < MAX_SOCKETS_SUPPORTED);
  ASSERT (Cdd < MAX_CDDS_PER_SOCKET);
  ASSERT (Function < 8);
  ASSERT (Offset < 0x400);
  ASSERT ((Offset & 3) == 0);
  ASSERT (Instance <= FABRIC_REG_ACC_BC);

  RegisterValue = (CddFabricRegisterAccRead (Socket, Cdd, Function, Offset, Instance) & ~NandValue) | OrValue;
  CddFabricRegisterAccWrite (Socket, Cdd, Function, Offset, Instance, RegisterValue, LogForS3);
  return RegisterValue;
}

