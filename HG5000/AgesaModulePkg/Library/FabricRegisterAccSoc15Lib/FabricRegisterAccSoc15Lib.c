/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Register Access Methods.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: Fabric
 * @e \$Revision$   @e \$Date$
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
#include <AGESA.h>
#include <Ids.h>
#include <Library/AmdBaseLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/PciLib.h>
#include <Library/AmdS3SaveLib.h>
#include "FabricRegisterAccSoc15Lib.h"
#include <Filecode.h>

#define FILECODE LIBRARY_FABRICREGISTERACCSOC15LIB_FABRICREGISTERACCSOC15LIB_FILECODE

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
UINT32
FabricRegisterAccGetPciDeviceNumberOfDie (
  IN       UINTN Socket,
  IN       UINTN Die
  );

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
  IN       UINTN  Die,
  IN       UINTN  Function,
  IN       UINTN  Offset,
  IN       UINTN  Instance
  )
{
  UINT32                               RegisterValue;
  PCI_ADDR                             PciAddr;
  FABRIC_IND_CFG_ACCESS_ADDR_REGISTER  FICAA3;

  ASSERT (Socket < 2);
  ASSERT (Die < 4);
  ASSERT (Function < 8);
  ASSERT (Offset < 0x400);
  ASSERT ((Offset & 3) == 0);
  ASSERT (Instance <= FABRIC_REG_ACC_BC);

  PciAddr.AddressValue = 0;
  PciAddr.Address.Device = FabricRegisterAccGetPciDeviceNumberOfDie (Socket, Die);
  if (Instance == FABRIC_REG_ACC_BC) {
    PciAddr.Address.Function = (UINT32) Function;
    PciAddr.Address.Register = (UINT32) Offset;
    RegisterValue = PciRead32 (PciAddr.AddressValue);
  } else {
    PciAddr.Address.Function = FICAA3_FUNC;
    PciAddr.Address.Register = FICAA3_REG;
    FICAA3.Value = 0;
    FICAA3.Field.CfgRegInstAccEn = 1;
    FICAA3.Field.IndCfgAccRegNum = ((UINT32) Offset) >> 2;
    FICAA3.Field.IndCfgAccFuncNum = (UINT32) Function;
    FICAA3.Field.CfgRegInstID = (UINT32) Instance;
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
  IN       UINTN    Die,
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

  ASSERT (Socket < 2);
  ASSERT (Die < 4);
  ASSERT (Function < 8);
  ASSERT (Offset < 0x400);
  ASSERT ((Offset & 3) == 0);
  ASSERT (Instance <= FABRIC_REG_ACC_BC);

  RegisterValue = Value;
  PciAddr.AddressValue = 0;
  PciAddr.Address.Device = FabricRegisterAccGetPciDeviceNumberOfDie (Socket, Die);
  if (Instance == FABRIC_REG_ACC_BC) {
    PciAddr.Address.Function = (UINT32) Function;
    PciAddr.Address.Register = (UINT32) Offset;
    PciWrite32 (PciAddr.AddressValue, RegisterValue);
    if (LogForS3) {
      AmdS3SaveScriptPciWrite (AccessWidth32, PciAddr.AddressValue, &RegisterValue);
    }
  } else {
    PciAddr.Address.Function = FICAA3_FUNC;
    PciAddr.Address.Register = FICAA3_REG;
    FICAA3.Value = 0;
    FICAA3.Field.CfgRegInstAccEn = 1;
    FICAA3.Field.IndCfgAccRegNum = ((UINT32) Offset) >> 2;
    FICAA3.Field.IndCfgAccFuncNum = (UINT32) Function;
    FICAA3.Field.CfgRegInstID = (UINT32) Instance;
    PciWrite32 (PciAddr.AddressValue, FICAA3.Value);
    if (LogForS3) {
      AmdS3SaveScriptPciWrite (AccessWidth32, PciAddr.AddressValue, &FICAA3.Value);
    }

    PciAddr.Address.Function = FICAD3_LO_FUNC;
    PciAddr.Address.Register = FICAD3_LO_REG;
    PciWrite32 (PciAddr.AddressValue, RegisterValue);
    if (LogForS3) {
      AmdS3SaveScriptPciWrite (AccessWidth32, PciAddr.AddressValue, &RegisterValue);
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
  IN       UINTN    Die,
  IN       UINTN    Function,
  IN       UINTN    Offset,
  IN       UINTN    Instance,
  IN       UINT32   NandValue,
  IN       UINT32   OrValue,
  IN       BOOLEAN  LogForS3
  )
{
  UINT32  RegisterValue;

  ASSERT (Socket < 2);
  ASSERT (Die < 4);
  ASSERT (Function < 8);
  ASSERT (Offset < 0x400);
  ASSERT ((Offset & 3) == 0);
  ASSERT (Instance <= FABRIC_REG_ACC_BC);

  RegisterValue = (FabricRegisterAccRead (Socket, Die, Function, Offset, Instance) & ~NandValue) | OrValue;
  FabricRegisterAccWrite (Socket, Die, Function, Offset, Instance, RegisterValue, LogForS3);
  return RegisterValue;
}

UINT32
FabricRegisterAccGetPciDeviceNumberOfDie (
  IN       UINTN Socket,
  IN       UINTN Die
  )
{
  return (UINT32) (((((UINT32) Socket) << 2) | (UINT32) Die) + 0x18);
}

