/*****************************************************************************
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
 ******************************************************************************
 */

#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Protocol/I2cMaster.h>
#include <Protocol/Sff8472Protocol.h>
#include "Sff8472Dxe.h"

extern  EFI_BOOT_SERVICES       *gBS;

EFI_DXE_SFF8472_PROTOCOL mDxeSff8472Protocol = {
  SFF8472_DXE_REVISION,
  Sff8472SetPageSelectDxe,
  Sff8472GetPageSelectDxe,
  Sff8472GetMapDxe
};

//
// Driver Global Data
//

/*********************************************************************************
 * Name: Pca9545aDxeInit
 *
 * Description
 *   Entry point of the AMD PCA9545A DXE driver
 *   Perform the configuration init, resource reservation, early post init
 *   and install all the supported protocol
 *
 * Input
 *   ImageHandle : EFI Image Handle for the DXE driver
 *   SystemTable : pointer to the EFI system table
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
Sff8472DxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  EFI_HANDLE          Handle;

  Handle = ImageHandle;

  // Publish Error Log service Protocol For IP driver
  Status = gBS->InstallProtocolInterface (
                &Handle,
                &gSff8472ProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mDxeSff8472Protocol
                );

  return (Status);
}


EFI_STATUS
EFIAPI
Sff8472SetPageSelectDxe (
  IN       EFI_DXE_SFF8472_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  IN       UINT8             PageSelectByte
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_I2C_MASTER_PROTOCOL *I2cMaster;
  EFI_I2C_REQUEST_PACKET  RequestPacket;
  EFI_I2C_OPERATION       *Operation;
  UINT8 prependAddress[2];

  //Big Enddian Address
  prependAddress[0] = PAGE_SELECT_BYTE_OFFSET;
  prependAddress[1] = PageSelectByte;

  Status = gBS->LocateProtocol (&gEfiI2cMasterProtocolGuid, NULL, &I2cMaster);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //Write Byte Operation
  Operation   = RequestPacket.Operation;
  RequestPacket.OperationCount = 1;
  Operation[0].Flags           = 0;
  Operation[0].LengthInBytes   = 2;
  Operation[0].Buffer          = &prependAddress[0];

  Status = I2cMaster->StartRequest (I2cMaster, DIAGNOSTIC_MONITORING_MAP_SLV, &RequestPacket, NULL, NULL);

  return Status;
}

EFI_STATUS
EFIAPI
Sff8472GetPageSelectDxe (
  IN       EFI_DXE_SFF8472_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  OUT      UINT8             *PageSelectByte
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_I2C_MASTER_PROTOCOL       *I2cMaster;
  EFI_I2C_REQUEST_PACKET        *RequestPacket;
  EFI_I2C_OPERATION             *Operation;
  UINT8                         PacketBuffer[sizeof (EFI_I2C_REQUEST_PACKET) + sizeof (EFI_I2C_OPERATION)];
  UINT8                         PageSelectByteOffset;

  PageSelectByteOffset    = PAGE_SELECT_BYTE_OFFSET;

  Status = gBS->LocateProtocol (&gEfiI2cMasterProtocolGuid, NULL, &I2cMaster);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  RequestPacket = (EFI_I2C_REQUEST_PACKET*)PacketBuffer;
  Operation   = RequestPacket->Operation;
  RequestPacket->OperationCount = 2;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof (UINT8);
  Operation[0].Buffer           = &PageSelectByteOffset;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = sizeof (UINT8);
  Operation[1].Buffer           = PageSelectByte;

  Status = I2cMaster->StartRequest (I2cMaster, DIAGNOSTIC_MONITORING_MAP_SLV, RequestPacket, NULL, NULL);

  return Status;
}

EFI_STATUS
Sff8472GetMapDxe (
  IN       EFI_DXE_SFF8472_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  IN       UINT32            Length,
  OUT      UINT8             *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_I2C_MASTER_PROTOCOL       *I2cMaster;
  EFI_I2C_REQUEST_PACKET        *RequestPacket;
  EFI_I2C_OPERATION             *Operation;
  UINT8                         PacketBuffer[sizeof (EFI_I2C_REQUEST_PACKET) + sizeof (EFI_I2C_OPERATION)];

  Status = gBS->LocateProtocol (&gEfiI2cMasterProtocolGuid, NULL, &I2cMaster);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  RequestPacket = (EFI_I2C_REQUEST_PACKET*)PacketBuffer;
  Operation   = RequestPacket->Operation;
  RequestPacket->OperationCount = 2;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof (UINT8);
  Operation[0].Buffer           = &Offset;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = Length;
  Operation[1].Buffer           = Data;

  Status = I2cMaster->StartRequest (I2cMaster, SlaveAddress, RequestPacket, NULL, NULL);

  return Status;
}
