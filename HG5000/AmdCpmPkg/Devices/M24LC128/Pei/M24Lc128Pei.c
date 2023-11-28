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
#include <Ppi/I2cMaster.h>
#include <Ppi/M24Lc128Ppi.h>
#include <M24Lc128Pei.h>

STATIC EFI_PEI_M24LC128_PPI mM24Lc128Ppi = {
  M24LC128_PPI_REVISION,
  M24Lc128Write,
  M24Lc128Read
};

STATIC EFI_PEI_PPI_DESCRIPTOR mM24Lc128PpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gM24Lc128PpiGuid,
  &mM24Lc128Ppi
};

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM M24LC128 PEI driver
 *
 * This function registers the function to publish M24LC128 EEPROM access ppi.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
M24Lc128PeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  // Publish M24LC128 access Ppi
  Status = (*PeiServices)->InstallPpi (
                           PeiServices,
                           &mM24Lc128PpiList
                           );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
EFIAPI
M24Lc128Write (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINTN                   BusSelect,
  IN UINTN                   SlaveAddress,
  IN UINT16                  Offset,
  IN UINT32                  Length,
  IN UINT8                   *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PEI_I2C_MASTER_PPI        *I2cMaster;

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gEfiPeiI2cMasterPpiGuid,
     BusSelect,
     NULL,
     &I2cMaster
     );

  Status = PageWrite (PeiServices, I2cMaster, SlaveAddress, Offset, Length, Data);

  return Status;
}

EFI_STATUS
PageWrite (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  EFI_PEI_I2C_MASTER_PPI     *I2cMaster,
  IN UINTN                   SlaveAddress,
  IN UINT16                  Offset,
  IN UINT32                  Length,
  IN UINT8                   *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_I2C_REQUEST_PACKET  RequestPacket;
  EFI_I2C_OPERATION       *Operation;
  WRITE_PAGE              WritePage;
  UINT32                  partialCount;
  UINT32                  pageSize;
  UINT32                  writeCount;
  UINT16                  address;

  Operation   = RequestPacket.Operation;
  RequestPacket.OperationCount = 1;
  pageSize = M24LC128_PAGESIZE;
  address = Offset;

  // Adjust first write so that it starts within the page and ends at the page boundary
  partialCount = (Offset % pageSize);
  if (partialCount) {
    partialCount = pageSize - partialCount;
    partialCount = (partialCount < Length) ? partialCount : Length;
  }

  while (0 < Length) {
    writeCount = pageSize;

    if (partialCount) {
      writeCount = partialCount;
      partialCount = 0;

    // If there are fewer bytes to write than a single page then adjust down
    } else if (pageSize > Length) {
      writeCount = Length;
    }

    //Big Enddian Address
    WritePage.prependAddress[0] = address >> 8;
    WritePage.prependAddress[1] = address & 0x00FF;

    (*PeiServices)->CopyMem ((UINT8*)&WritePage.TxData[0], Data, writeCount);

    RequestPacket.OperationCount  = 1;
    Operation[0].Flags            = 0;
    Operation[0].LengthInBytes    = sizeof (WritePage.prependAddress) + writeCount;
    Operation[0].Buffer           = (UINT8*)&WritePage.prependAddress[0];

    Status = I2cMaster->StartRequest (I2cMaster, SlaveAddress, &RequestPacket);

    if (Status) {
      break;
    }

    Data = &Data[writeCount];
    Length -= writeCount;
    address += (UINT16)writeCount;
  }

  return Status;
}

EFI_STATUS
EFIAPI
M24Lc128Read (
  IN       CONST EFI_PEI_SERVICES  **PeiServices,
  IN       UINTN                   BusSelect,
  IN       UINTN                   SlaveAddress,
  IN       UINT16                  Offset,
  IN       UINT32                  Length,
  OUT      UINT8                   *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PEI_I2C_MASTER_PPI        *I2cMaster;
  EFI_I2C_REQUEST_PACKET        *RequestPacket;
  EFI_I2C_OPERATION             *Operation;
  UINT8                         PacketBuffer[sizeof (EFI_I2C_REQUEST_PACKET) + sizeof (EFI_I2C_OPERATION)];
  UINT8 prependAddress[2];

  //Big Enddian Address
  prependAddress[0] = Offset >> 8;
  prependAddress[1] = Offset & 0x00FF;

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gEfiPeiI2cMasterPpiGuid,
     BusSelect,
     NULL,
     &I2cMaster
     );

  RequestPacket = (EFI_I2C_REQUEST_PACKET*)PacketBuffer;
  Operation   = RequestPacket->Operation;
  RequestPacket->OperationCount = 2;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof (UINT16);
  Operation[0].Buffer           = (UINT8*)&prependAddress;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = Length;
  Operation[1].Buffer           = Data;

  Status = I2cMaster->StartRequest (I2cMaster, SlaveAddress, RequestPacket);

  return Status;
}
