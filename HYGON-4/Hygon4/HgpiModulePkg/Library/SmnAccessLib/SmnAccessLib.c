/* $NoKeywords:$ */

/**
 * @file
 *
 * SMN Register Access Methods
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
#include  <Library/BaseLib.h>
#include  <HGPI.h>
#include  <Library/SmnAccessLib.h>
#include  <Library/HygonBaseLib.h>
#include  <Library/IdsLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/BaseFabricTopologyLib.h>
#include  <Filecode.h>
#define FILECODE  LIBRARY_SMNACCESSLIB_SMNACCESSLIB_FILECODE

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

/*
 *  Routine to read all register spaces.
 *
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[out] Value             Return value
 * @retval     VOID
 */
VOID
SmnRegisterRead (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  OUT      VOID                *Value
  )
{
  UINT64        TempData;
  ACCESS_WIDTH  Width;
  PCI_ADDR      GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus  = BusNumber;
  Width = AccessWidth32;

  GnbLibPciIndirectRead (
    GnbPciAddress.AddressValue | D0F0xB8_ADDRESS,
    (Address & (~0x3ull)),
    Width,
    &TempData,
    NULL
    );
  if ((Address & 0x3) != 0) {
    // Non aligned access allowed to fuse block
    GnbLibPciIndirectRead (
      GnbPciAddress.AddressValue | D0F0xB8_ADDRESS,
      (Address & (~0x3ull)) + 4,
      Width,
      ((UINT32 *)&TempData) + 1,
      NULL
      );
  }

  *((UINT32 *)Value) = (UINT32)RShiftU64 (TempData, ((Address & 0x3) * 8));
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to write all register spaces.
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[in]  Value             The value to write
 * @param[in]  Flags             Flags - BIT0 indicates S3 save/restore
 * @retval     VOID
 */
VOID
SmnRegisterWrite (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  )
{
  ACCESS_WIDTH  Width;
  PCI_ADDR      GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus  = BusNumber;
  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;

  GnbLibPciIndirectWrite (
    GnbPciAddress.AddressValue | D0F0xB8_ADDRESS,
    Address,
    Width,
    Value,
    NULL
    );
  return;
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to Read/Modify/Write all register spaces.
 *
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[in]  Mask              And Mask
 * @param[in]  Value             Or value
 * @retval     VOID
 */
VOID
SmnRegisterRMW (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       UINT32              AndMask,
  IN       UINT32              OrValue,
  IN       UINT32              Flags
  )
{
  UINT32  Data;

  SmnRegisterRead (BusNumber, Address, &Data);
  Data = (Data & AndMask) | OrValue;
  SmnRegisterWrite (BusNumber, Address, &Data, Flags);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to read all register spaces.
 *
 *
 *
 * @param[in]  Socket            Socket number of target
 * @param[in]  RbNumber          RootBridge number of target
 * @param[in]  SmnAddress        Register offset, but PortDevice
 * @param[in]  Width             AccessWidth8, AccessWidth16, AccessWidth32 etc...
 * @param[out] Value             Return value
 * @retval     VOID
 */
VOID
SmnRegisterReadBySocketRb (
  IN       UINT8               Socket,
  IN       UINT8               RbNumber,
  IN       UINT32              SmnAddress,
  IN       ACCESS_WIDTH        Width,
  OUT      VOID                *Value
  )
{
  UINT32    RegIndex;
  PCI_ADDR  GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus  = (UINT8)FabricTopologyGetHostBridgeBusBase (Socket, 0, RbNumber);

  RegIndex = SmnAddress & 0xFFFFFFFC;
  GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessWidth32, &RegIndex, NULL);

  RegIndex = SmnAddress & 0x3;
  GnbLibPciRead (GnbPciAddress.AddressValue | D0F0xBC_ADDRESS + RegIndex, Width, Value, NULL);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to write all register spaces.
 *
 *
 * @param[in]  Socket            Socket number of target
 * @param[in]  RbNumber          RootBridge number of target
 * @param[in]  SmnAddress        Register offset, but PortDevice
 * @param[in]  Width             AccessWidth8, AccessWidth16, AccessWidth32 etc...
 *                               If need S3 save/restore, should be AccessS3SaveWidthx
 * @param[in]  Value             The value to write
 * @retval     VOID
 */
VOID
SmnRegisterWriteBySocketRb (
  IN       UINT8               Socket,
  IN       UINT8               RbNumber,
  IN       UINT32              SmnAddress,
  IN       ACCESS_WIDTH        Width,
  IN       VOID                *Value
  )
{
  UINT32    RegIndex;
  PCI_ADDR  GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus  = (UINT8)FabricTopologyGetHostBridgeBusBase (Socket, 0, RbNumber);

  RegIndex = SmnAddress & 0xFFFFFFFC;
  if(Width >= AccessS3SaveWidth8) {
    GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessS3SaveWidth32, &RegIndex, NULL);
  } else {
    GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessWidth32, &RegIndex, NULL);
  }

  RegIndex = SmnAddress & 0x3;
  GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xBC_ADDRESS + RegIndex, Width, Value, NULL);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to Read/Modify/Write all register spaces.
 *
 *
 *
 * @param[in]  Socket            Socket number of target
 * @param[in]  RbNumber          RootBridge number of target
 * @param[in]  SmnAddress        Register offset, but PortDevice
 * @param[in]  Width             AccessWidth8, AccessWidth16, AccessWidth32 etc...
 *                               If need S3 save/restore, should be AccessS3SaveWidthx
 * @param[in]  Mask              And Mask
 * @param[in]  Value             Or value
 * @retval     VOID
 */
VOID
SmnRegisterRMWBySocketRb (
  IN       UINT8               Socket,
  IN       UINT8               RbNumber,
  IN       UINT32              SmnAddress,
  IN       ACCESS_WIDTH        Width,
  IN       UINT32              AndMask,
  IN       UINT32              OrValue
  )
{
  UINT32  Data;

  SmnRegisterReadBySocketRb (Socket, RbNumber, SmnAddress, Width, &Data);
  Data = (Data & AndMask) | OrValue;
  SmnRegisterWriteBySocketRb (Socket, RbNumber, SmnAddress, Width, &Data);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to read all register spaces.
 *
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[out] Value             Return value
 * @retval     VOID
 */
VOID
SmnRegisterRead8 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  OUT      VOID                *Value
  )
{
  ACCESS_WIDTH  Width;
  UINT32        RegIndex;
  PCI_ADDR      GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus  = BusNumber;
  Width = AccessWidth8;

  RegIndex = Address & 0xFFFFFFFC;
  GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessWidth32, &RegIndex, NULL);

  RegIndex = Address & 0x3;
  GnbLibPciRead (GnbPciAddress.AddressValue | D0F0xBC_ADDRESS + RegIndex, Width, Value, NULL);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to read all register spaces.
 *
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[out] Value             Return value
 * @retval     VOID
 */
VOID
SmnRegisterRead16 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  OUT      VOID                *Value
  )
{
  ACCESS_WIDTH  Width;
  UINT32        RegIndex;
  PCI_ADDR      GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus  = BusNumber;
  Width = AccessWidth16;

  RegIndex = Address & 0xFFFFFFFC;
  GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessWidth32, &RegIndex, NULL);

  RegIndex = Address & 0x3;
  GnbLibPciRead (GnbPciAddress.AddressValue | D0F0xBC_ADDRESS + RegIndex, Width, Value, NULL);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to read all register spaces.
 *
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[out] Value             Return value
 * @retval     VOID
 */
VOID
SmnRegisterRead32 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  OUT      VOID                *Value
  )
{
  ACCESS_WIDTH  Width;
  UINT32        RegIndex;
  PCI_ADDR      GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus  = BusNumber;
  Width = AccessWidth32;

  RegIndex = Address & 0xFFFFFFFC;
  GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessWidth32, &RegIndex, NULL);

  RegIndex = Address & 0x3;
  GnbLibPciRead (GnbPciAddress.AddressValue | D0F0xBC_ADDRESS + RegIndex, Width, Value, NULL);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine (byte width) to write all register spaces.
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[in]  Value             The value to write
 */
VOID
SmnRegisterWrite8 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  )
{
  ACCESS_WIDTH  Width;
  UINT32        RegIndex;
  PCI_ADDR      GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus  = BusNumber;
  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth8 : AccessWidth8;

  RegIndex = Address & 0xFFFFFFFC;
  if(Width >= AccessS3SaveWidth8) {
    GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessS3SaveWidth32, &RegIndex, NULL);
  } else {
    GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessWidth32, &RegIndex, NULL);
  }

  RegIndex = Address & 0x3;
  GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xBC_ADDRESS + RegIndex, Width, Value, NULL);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine (byte width) to write all register spaces.
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[in]  Value             The value to write
 */
VOID
SmnRegisterWrite16 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  )
{
  ACCESS_WIDTH  Width;
  UINT32        RegIndex;
  PCI_ADDR      GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus  = BusNumber;
  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth16 : AccessWidth16;

  RegIndex = Address & 0xFFFFFFFC;
  if(Width >= AccessS3SaveWidth8) {
    GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessS3SaveWidth32, &RegIndex, NULL);
  } else {
    GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessWidth32, &RegIndex, NULL);
  }

  RegIndex = Address & 0x3;
  GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xBC_ADDRESS + RegIndex, Width, Value, NULL);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine (byte width) to write all register spaces.
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[in]  Value             The value to write
 */
VOID
SmnRegisterWrite32 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  )
{
  ACCESS_WIDTH  Width;
  UINT32        RegIndex;
  PCI_ADDR      GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus  = BusNumber;
  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;

  RegIndex = Address & 0xFFFFFFFC;
  if(Width >= AccessS3SaveWidth8) {
    GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessS3SaveWidth32, &RegIndex, NULL);
  } else {
    GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessWidth32, &RegIndex, NULL);
  }

  RegIndex = Address & 0x3;
  GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xBC_ADDRESS + RegIndex, Width, Value, NULL);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to Read/Modify/Write all register spaces.
 *
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[in]  Mask              And Mask
 * @param[in]  Value             Or value
 * @retval     VOID
 */
VOID
SmnRegisterRMW8 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       UINT8               AndMask,
  IN       UINT8               OrValue,
  IN       UINT32              Flags
  )
{
  UINT8  Data;

  SmnRegisterRead8 (BusNumber, Address, &Data);
  Data = (Data & AndMask) | OrValue;
  SmnRegisterWrite8 (BusNumber, Address, &Data, Flags);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to Read/Modify/Write all register spaces.
 *
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[in]  Mask              And Mask
 * @param[in]  Value             Or value
 * @retval     VOID
 */
VOID
SmnRegisterRMW16 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       UINT16              AndMask,
  IN       UINT16              OrValue,
  IN       UINT32              Flags
  )
{
  UINT16  Data;

  SmnRegisterRead16 (BusNumber, Address, &Data);
  Data = (Data & AndMask) | OrValue;
  SmnRegisterWrite16 (BusNumber, Address, &Data, Flags);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to Read/Modify/Write all register spaces.
 *
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[in]  Mask              And Mask
 * @param[in]  Value             Or value
 * @retval     VOID
 */
VOID
SmnRegisterRMW32 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       UINT32              AndMask,
  IN       UINT32              OrValue,
  IN       UINT32              Flags
  )
{
  UINT32  Data;

  SmnRegisterRead32 (BusNumber, Address, &Data);
  Data = (Data & AndMask) | OrValue;
  SmnRegisterWrite32 (BusNumber, Address, &Data, Flags);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to Read/Modify/Write all register spaces.
 *
 *
 *
 * @param[in]  RbIndex           System of RootBrdige number of target
 * @param[in]  SmnAddress        Register offset, but PortDevice
 * @param[in]  Value             Return value
 * @retval     VOID
 */
VOID
SmnRegisterReadByRbIndex (
  IN       UINT32              SysRbIndex,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value
  )
{
  UINT8  Socket;
  UINT8  RbNumber;
  UINT8  RbNumberPerSocket;

  RbNumberPerSocket = (UINT8)FabricTopologyGetNumberOfRootBridgesOnSocket (0);

  Socket   = (UINT8)SysRbIndex / RbNumberPerSocket;
  RbNumber = (UINT8)SysRbIndex % RbNumberPerSocket;

  SmnRegisterReadBySocketRb (Socket, RbNumber, SmnAddress, AccessWidth32, Value);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to Read/Modify/Write all register spaces.
 *
 *
 *
 * @param[in]  RbIndex           System of RootBrdige number of target
 * @param[in]  SmnAddress        Register offset, but PortDevice
 * @param[in]  Value             Return value
 * @retval     VOID
 */
VOID
SmnRegisterWriteByRbIndex (
  IN       UINT32              SysRbIndex,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value,
  IN       UINT32              Flags
  )
{
  UINT8         Socket;
  UINT8         RbNumber;
  ACCESS_WIDTH  Width;
  UINT8         RbNumberPerSocket;

  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;

  RbNumberPerSocket = (UINT8)FabricTopologyGetNumberOfRootBridgesOnSocket (0);

  Socket   = (UINT8)SysRbIndex / RbNumberPerSocket;
  RbNumber = (UINT8)SysRbIndex % RbNumberPerSocket;

  SmnRegisterWriteBySocketRb (Socket, RbNumber, SmnAddress, Width, Value);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to Read/Modify/Write all register spaces.
 *
 *
 *
 * @param[in]  RbIndex           System of RootBrdige number of target
 * @param[in]  SmnAddress        Register offset, but PortDevice
 * @param[in]  Mask              And Mask
 * @param[in]  Value             Or value
 * @retval     VOID
 */
VOID
SmnRegisterRMWByRbIndex (
  IN       UINT32              SysRbIndex,
  IN       UINT32              SmnAddress,
  IN       UINT32              AndMask,
  IN       UINT32              OrValue,
  IN       UINT32              Flags
  )
{
  UINT32  Data;

  SmnRegisterReadByRbIndex (SysRbIndex, SmnAddress, &Data);
  Data = (Data & AndMask) | OrValue;
  SmnRegisterWriteByRbIndex (SysRbIndex, SmnAddress, &Data, Flags);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to read all register spaces.
 *
 *
 *
 * @param[in]  Socket            Socket number of target
 * @param[in]  SmnAddress        Register offset, but PortDevice
 * @param[in]  Width             AccessWidth8, AccessWidth16, AccessWidth32 etc...
 * @param[out] Value             Return value
 * @retval     VOID
 */
VOID
SmnRegisterReadBySocket (
  IN       UINT8               Socket,
  IN       UINT32              SmnAddress,
  IN       ACCESS_WIDTH        Width,
  OUT      VOID                *Value
  )
{
  UINT32    RegIndex;
  PCI_ADDR  GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus  = (UINT8)FabricTopologyGetHostBridgeBusBase (Socket, 0, 0);

  RegIndex = SmnAddress & 0xFFFFFFFC;
  GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessWidth32, &RegIndex, NULL);

  RegIndex = SmnAddress & 0x3;
  GnbLibPciRead (GnbPciAddress.AddressValue | D0F0xBC_ADDRESS + RegIndex, Width, Value, NULL);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to write all register spaces.
 *
 *
 * @param[in]  Socket            Socket number of target
 * @param[in]  SmnAddress        Register offset, but PortDevice
 * @param[in]  Width             AccessWidth8, AccessWidth16, AccessWidth32 etc...
 *                               If need S3 save/restore, should be AccessS3SaveWidthx
 * @param[in]  Value             The value to write
 * @retval     VOID
 */
VOID
SmnRegisterWriteBySocket (
  IN       UINT8               Socket,
  IN       UINT32              SmnAddress,
  IN       ACCESS_WIDTH        Width,
  IN       VOID                *Value
  )
{
  UINT32    RegIndex;
  PCI_ADDR  GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus  = (UINT8)FabricTopologyGetHostBridgeBusBase (Socket, 0, 0);

  RegIndex = SmnAddress & 0xFFFFFFFC;
  if(Width >= AccessS3SaveWidth8) {
    GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessS3SaveWidth32, &RegIndex, NULL);
  } else {
    GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xB8_ADDRESS, AccessWidth32, &RegIndex, NULL);
  }

  RegIndex = SmnAddress & 0x3;
  GnbLibPciWrite (GnbPciAddress.AddressValue | D0F0xBC_ADDRESS + RegIndex, Width, Value, NULL);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to Read/Modify/Write all register spaces.
 *
 *
 *
 * @param[in]  Socket            Socket number of target
 * @param[in]  SmnAddress        Register offset, but PortDevice
 * @param[in]  Width             AccessWidth8, AccessWidth16, AccessWidth32 etc...
 *                               If need S3 save/restore, should be AccessS3SaveWidthx
 * @param[in]  Mask              And Mask
 * @param[in]  Value             Or value
 * @retval     VOID
 */
VOID
SmnRegisterRMWBySocket (
  IN       UINT8               Socket,
  IN       UINT32              SmnAddress,
  IN       ACCESS_WIDTH        Width,
  IN       UINT32              AndMask,
  IN       UINT32              OrValue
  )
{
  UINT32  Data;

  SmnRegisterReadBySocket (Socket, SmnAddress, Width, &Data);
  Data = (Data & AndMask) | OrValue;
  SmnRegisterWriteBySocket (Socket, SmnAddress, Width, &Data);
}
