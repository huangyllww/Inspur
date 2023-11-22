#include <Uefi.h>
#include <PiPei.h>
#include "Pi/PiHob.h"
#include <HGPI.h>
#include "Library/HygonCalloutLib.h"
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IdsLib.h>
#include <Library/IoLib.h>
#include <Ppi/Smbus2.h>
#include <Ppi/HygonMemPpi.h>
#include <PlatformMemoryConfiguration.h>
#include "HgpiMemPei.h"
#include <Ppi/HygonMemPpi.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <HygonBoardId.h>
#include "Filecode.h"

#define FILECODE  MEM_HYGONMEMPEI_HGPIPEI_FILECODE

#define FCH_IOMUXx89_UART0_RTS_L_EGPIO137  0xFED80D89ul
#define FCH_GPIO_137_PIN                   (0xfed81500UL + 0x224)

HGPI_DIMM_INFO_SMBUS DimmSmbus[] =
{
  // DIMM_SPD_MAP
  { 0xFF, 0,    0,    0xA0 },
  { 0xFF, 0,    1,    0xA2 },
  { 0xFF, 1,    0,    0xA4 },
  { 0xFF, 1,    1,    0xA6 },
  { 0xFF, 2,    0,    0xA8 },
  { 0xFF, 2,    1,    0xAA },
  { 0xFF, 3,    0,    0xAC },
  { 0xFF, 3,    1,    0xAE },
  { 0xFF, 4,    0,    0xA0 },
  { 0xFF, 4,    1,    0xA2 },
  { 0xFF, 5,    0,    0xA4 },
  { 0xFF, 5,    1,    0xA6 },
  { 0xFF, 6,    0,    0xA8 },
  { 0xFF, 6,    1,    0xAA },
  { 0xFF, 7,    0,    0xAC },
  { 0xFF, 7,    1,    0xAE },
  { 0xFF, 0xFF, 0xFF, 0xFF }
};

EFI_STATUS
GetSocketAndDimmInfo (
  UINT8 *NumberOfSocket, UINT8 *NumberOfDimm
  )
{
  HYGON_BOARD_ID  PlatformSelect;

  if(NumberOfSocket == NULL || NumberOfDimm == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PlatformSelect = PcdGet8 (PcdPlatformSelect);
  IDS_HDT_CONSOLE (MAIN_FLOW, "    PlatformSelect=%d\n", PlatformSelect);
  switch (PlatformSelect) {
    case HYGON_NANHAIVTB1:
      *NumberOfDimm = 0x18;
      break;

    case HYGON_NANHAIVTB3:
      *NumberOfDimm = 0x18;
      break;

    default:
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
  }

  *NumberOfSocket = (UINT8)FabricTopologyGetNumberOfSocketPresent ();

  IDS_HDT_CONSOLE (MAIN_FLOW, "    NumberOfSocket=%d, NumberOfDimm=%d\n", *NumberOfSocket, *NumberOfDimm);
  return EFI_SUCCESS;
}

VOID
SmbusI2CMuxSelection (
  IN  CONST EFI_PEI_SERVICES     **PeiServices,
  IN  UINT8   I2CMuxAddress,
  IN  UINT8   I2CCommand,
  IN  UINT8   I2CChannelAddress
  )
{
  EFI_STATUS                Status;
  EFI_PEI_SMBUS2_PPI        *SmbusPpi = NULL;
  EFI_SMBUS_DEVICE_ADDRESS  SmbusSlaveAddress;
  UINTN                     SmbusLength = 0;
  UINTN                     SmbusOffset = 0;

  (**PeiServices).LocatePpi (PeiServices, &gEfiPeiSmbus2PpiGuid, 0, NULL, &SmbusPpi);
  SmbusLength = 1;
  SmbusOffset = 0;
  SmbusSlaveAddress.SmbusDeviceAddress = (I2CMuxAddress >> 1);
  Status = SmbusPpi->Execute (
                       SmbusPpi,
                       SmbusSlaveAddress,
                       I2CCommand,
                       EfiSmbusWriteByte,
                       FALSE,
                       &SmbusLength,
                       &I2CChannelAddress
                       );
  IDS_HDT_CONSOLE (MAIN_FLOW, "SmbusI2CMuxSelection Status = %r\n", Status);
}

/**
    This function changes the SMBUS page number.
    A dummy write to the slave address of 6E is made to switch the page address to 1, prior to the SPD reading commands.
    Upon completion, another dummy write to the slave address of 6C is made to restore the original page address

    @param PeiServices PEI Services table pointer
    @param Page        Page number for switch

    @retval EFI_STATUS return EFI status

 **/
VOID
SmbusPageChange (
  IN  CONST EFI_PEI_SERVICES     **PeiServices,
  IN  UINT8                      Page
  )
{
  EFI_STATUS                Status;
  EFI_PEI_SMBUS2_PPI        *SmbusPpi = NULL;
  EFI_SMBUS_DEVICE_ADDRESS  SmbusSlaveAddress;
  UINTN                     SmbusLength = 0;
  UINTN                     SmbusOffset = 0;
  UINT8                     Buffer = 0;

  (**PeiServices).LocatePpi (PeiServices, &gEfiPeiSmbus2PpiGuid, 0, NULL, &SmbusPpi);
  // A dummy write to the slave address of 6E is made to switch the page address to 1, prior to the SPD reading commands.
  // Upon completion, another dummy write to the slave address of 6C is made to restore the original page address

  SmbusLength = 1;
  SmbusOffset = 0;
  if (Page == 1) {
    SmbusSlaveAddress.SmbusDeviceAddress = (0x6E >> 1);
    Status = SmbusPpi->Execute (
                         SmbusPpi,
                         SmbusSlaveAddress,
                         SmbusOffset,
                         EfiSmbusWriteByte,
                         FALSE,
                         &SmbusLength,
                         &Buffer
                         );
    // IDS_HDT_CONSOLE (MAIN_FLOW, "\n\n Change SMBUS to page 1 \n");
  } else {
    SmbusSlaveAddress.SmbusDeviceAddress = (0x6C >> 1);
    Status = SmbusPpi->Execute (
                         SmbusPpi,
                         SmbusSlaveAddress,
                         SmbusOffset,
                         EfiSmbusWriteByte,
                         FALSE,
                         &SmbusLength,
                         &Buffer
                         );
    // IDS_HDT_CONSOLE (MAIN_FLOW, "\n\n Change SMBUS to page 0 \n");
  }
}

/**
    This function reads whole SPD to provided buffer.

    @param PeiServices PEI Services table pointer
    @param SaveAddr SMBus address of SPD
    @param SpdData Pointer to read SPD parameters structure

    @retval EFI_STATUS return EFI status

 **/
EFI_STATUS
MemInitReadSpd (
  IN  EFI_PEI_SERVICES   **PeiServices,
  IN  UINT8              SlaveAddr,
  IN OUT  HGPI_READ_SPD_PARAMS *SpdData
  )
{
  EFI_STATUS                Status;
  EFI_SMBUS_DEVICE_ADDRESS  SmbusSlaveAddress;
  UINTN                     SmbusLength = 0;
  UINTN                     SmbusOffset = 0;
  EFI_PEI_SMBUS2_PPI        *SmbusPpi   = NULL;
  UINTN                     Index    = 0;
  UINT8                     *Buffer2 = NULL;
  UINT8                     DramType = 0;             // B is DDR3, C is DDR4

  // Locate Smbus Device
  Status = (**PeiServices).LocatePpi (PeiServices, &gEfiPeiSmbus2PpiGuid, 0, NULL, &SmbusPpi);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SmbusSlaveAddress.SmbusDeviceAddress = (SlaveAddr >> 1);
  SmbusLength = 1;
  SmbusOffset = 2;    // Key Byte / DRAM Device Type
  Status = SmbusPpi->Execute (
                       SmbusPpi,
                       SmbusSlaveAddress,
                       SmbusOffset,
                       EfiSmbusReadByte,
                       FALSE,
                       &SmbusLength,
                       &DramType
                       );
  // if SPD data not available at location 2, then we return status
  if (EFI_DEVICE_ERROR == Status) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "MemInitReadSpd.Execute Status = %r\n", Status);
    return Status;
  }

  Status = EFI_SUCCESS;
  SmbusLength = 1;
  SmbusOffset = 0;
  Buffer2     = SpdData->Buffer;

  // It's DDR4
  // The 256 bytes in Page 0
  SmbusPageChange (PeiServices, 0);
  for ( Index = 0; Index < DIMM_SPD_DATA_256; Index++ ) {
    Status = SmbusPpi->Execute (
                         SmbusPpi,
                         SmbusSlaveAddress,
                         SmbusOffset,
                         EfiSmbusReadByte,
                         FALSE,
                         &SmbusLength,
                         Buffer2
                         );
    SmbusOffset += 1;
    Buffer2++;
  }

  // Change to page 1
  SmbusPageChange (PeiServices, 1);
  // The 256 bytes in Page 1
  for ( Index = 0; Index < DIMM_SPD_DATA_256; Index++ ) {
    Status = SmbusPpi->Execute (
                         SmbusPpi,
                         SmbusSlaveAddress,
                         SmbusOffset,
                         EfiSmbusReadByte,
                         FALSE,
                         &SmbusLength,
                         Buffer2
                         );
    SmbusOffset += 1;
    Buffer2++;
  }

  // Change to Page 0
  SmbusPageChange (PeiServices, 0);

  return Status;
}

/**
    This function reads provides SPD data if DIMM present.


    @param PeiServices PEI Services table pointer
    @param This Pointer to
    @param SpdData Pointer to read SPD parameters
        structure


    @retval EFI_STATUS return EFI status

 **/
EFI_STATUS
EFIAPI
PlatformDimmSpdRead (
  IN      EFI_PEI_SERVICES      **PeiServices,
  IN      PEI_HYGON_PLATFORM_DIMM_SPD_PPI      *This,
  IN OUT  HGPI_READ_SPD_PARAMS *SpdData
  )
{
  UINTN                 Index;
  UINT8                 SmbusAddress;
  EFI_STATUS            Status;
  HGPI_DIMM_INFO_SMBUS  *pDimmSmbus;
  HYGON_BOARD_ID        PlatformSelect;
  UINT8                 TotalNumberOfSocket;
  UINT8                 TotalNumberOfDimm;

  Status = GetSocketAndDimmInfo (&TotalNumberOfSocket, &TotalNumberOfDimm);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "ASSERT_EFI_ERROR (Status = %r)\n", Status);
    ASSERT (!EFI_ERROR (Status));
    return Status;
  }

  SmbusAddress = 0;
  Status = EFI_INVALID_PARAMETER;

  PlatformSelect = PcdGet8 (PcdPlatformSelect);

  pDimmSmbus = &DimmSmbus[0];

  for (Index = 0; pDimmSmbus[Index].SmbusAddress != 0xFF; Index++) {
    if ((pDimmSmbus[Index].SocketId == 0xFF) || (SpdData->SocketId == pDimmSmbus[Index].SocketId)) {
      if (  (SpdData->MemChannelId == pDimmSmbus[Index].ChannelId)
         && (SpdData->DimmId == pDimmSmbus[Index].DimmId)) {
        SmbusAddress = pDimmSmbus[Index].SmbusAddress;
        IDS_HDT_CONSOLE (MAIN_FLOW, "SMBUS address found = %x\n", SmbusAddress);
        Status = EFI_SUCCESS;
        break;
      }
    }
  }

  if (SpdData->SocketId > (TotalNumberOfSocket - 1)) {
    return Status;
  }

  if (Status == EFI_INVALID_PARAMETER) {
    return Status;
  }

  //
  // Switch the channel based off following information before initiating SPD read.
  //
  IDS_HDT_CONSOLE (MAIN_FLOW, "Hygon PlatformSelect = %x\n", PlatformSelect);

  Status = MemInitReadSpd (PeiServices, SmbusAddress, SpdData);

  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\n --Sckt-Ch-Dimm=[%x-%x-%x]--Dimm Addr %X  ---SPD Read Sts %r \n", \
    SpdData->SocketId,
    SpdData->MemChannelId,
    SpdData->DimmId,
    SmbusAddress,
    Status
    );
  return Status;
}

/**
    This function reads the serial number of SPD

    @param PeiServices PEI Services table pointer
    @param UINT8       SMBUS slave address to read
    @param Buffer      the data buffer to be sent out
    @param BufferIndex the buffer index

    @retval EFI_STATUS Status of result

 **/
EFI_STATUS
ReadSaveDimmSerial (
  IN   CONST EFI_PEI_SERVICES    **PeiServices,
  UINT8                          SlaveAddr,
  UINT8                          *Buffer,
  UINT8                          BufferIndex
  )
{
  UINT8                     Buffer2, Index;
  EFI_STATUS                Status;
  EFI_SMBUS_DEVICE_ADDRESS  SmbusSlaveAddress;
  EFI_PEI_SMBUS2_PPI        *SmbusPpi   = NULL;
  UINTN                     SmbusLength = 0;
  UINTN                     SmbusOffset = 0;
  UINT8                     DramType    = 0;

  // Check if device Present by reading offset 2
  // if not present return
  // if device present, read the device serial
  // Update Buffer
  // Returns error if cannot locate smbus ppi
  SmbusLength = 1;
  SmbusOffset = 2;    // Key Byte / DRAM Device Type

  IDS_HDT_CONSOLE (MAIN_FLOW, "\n ReadSaveDimmSerial entry \n");

  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gEfiPeiSmbus2PpiGuid,
                             0,
                             NULL,
                             &SmbusPpi
                             );

  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "\n SlaveAddr = %x \n", SlaveAddr);
  IDS_HDT_CONSOLE (MAIN_FLOW, "\n BufferIndex = %x \n", BufferIndex);

  SmbusSlaveAddress.SmbusDeviceAddress = (SlaveAddr >> 1);

  Status = SmbusPpi->Execute (
                       SmbusPpi,
                       SmbusSlaveAddress,
                       SmbusOffset,
                       EfiSmbusReadByte,
                       FALSE,
                       &SmbusLength,
                       &DramType
                       );
  // if SPD data is not available at offset 2, then we return status
  if (EFI_DEVICE_ERROR == Status) {
    return Status;
  }

  Status = EFI_SUCCESS;
  SmbusSlaveAddress.SmbusDeviceAddress = (SlaveAddr >> 1);
  SmbusLength = 1;

  // It's DDR4
  IDS_HDT_CONSOLE (MAIN_FLOW, "\n DDR4 Serial number = \n");

  // Change to page 1
  SmbusPageChange (PeiServices, 1);

  SmbusOffset = DIMM_SPD_SERIAL_NO_OFFSET_DDR4;
  for ( Index = 0; Index < DIMM_SPD_SERIAL_NO_LENGTH; Index++ ) {
    Status = SmbusPpi->Execute (
                         SmbusPpi,
                         SmbusSlaveAddress,
                         SmbusOffset,
                         EfiSmbusReadByte,
                         FALSE,
                         &SmbusLength,
                         &Buffer2
                         );
    IDS_HDT_CONSOLE (MAIN_FLOW, " %x ", Buffer2);
    SmbusOffset += 1;
    Buffer[BufferIndex * DIMM_SPD_SERIAL_NO_LENGTH + Index] = Buffer2;
  }

  // Change to Page 0
  SmbusPageChange (PeiServices, 0);

  return EFI_SUCCESS;
}

/**
 *  HgpiPeiEntry
 *
 *  Description:
 *    This function will prepare the platform configuarion for HGPI use.
 *
 *  Parameters:
 *    @param[in]        **PeiServices
 *                      FileHandle
 *
 *
 *    @retval         VOID
 *
 **/
EFI_STATUS
EFIAPI
HgpiMemPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  EFI_PEI_PPI_DESCRIPTOR           *mPpiDescriptorList;
  PEI_HYGON_PLATFORM_DIMM_SPD_PPI  *mPeiHygonPlatformDimmSpdPpi;
  UINT8                            TotalNumberOfSocket;
  UINT8                            TotalNumberOfDimm;

  IDS_HDT_CONSOLE (MAIN_FLOW, "    HgpiMemPeiEntry Entry\n");

  // Allocate memory for the PPI descriptor
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             (VOID **)&mPpiDescriptorList
                             );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "ASSERT_EFI_ERROR (Status = %r)\n", Status);
    ASSERT (!EFI_ERROR (Status));
    return Status;
  }

  // Allocate memory for the PPI descriptor
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (PEI_HYGON_PLATFORM_DIMM_SPD_PPI),
                             (VOID **)&mPeiHygonPlatformDimmSpdPpi
                             );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "ASSERT_EFI_ERROR (Status = %r)\n", Status);
    ASSERT (!EFI_ERROR (Status));
    return Status;
  }

  Status = GetSocketAndDimmInfo (&TotalNumberOfSocket, &TotalNumberOfDimm);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "ASSERT_EFI_ERROR (Status = %r)\n", Status);
    ASSERT (!EFI_ERROR (Status));
    return Status;
  }

  mPeiHygonPlatformDimmSpdPpi->TotalNumberOfSocket = TotalNumberOfSocket;
  mPeiHygonPlatformDimmSpdPpi->TotalNumberOfDimms  = TotalNumberOfDimm;
  mPeiHygonPlatformDimmSpdPpi->PlatformDimmSpdRead = PlatformDimmSpdRead;

  mPpiDescriptorList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  mPpiDescriptorList->Guid  = &gHygonPlatformDimmSpdPpiGuid;
  mPpiDescriptorList->Ppi   = mPeiHygonPlatformDimmSpdPpi;

  Status = (*PeiServices)->InstallPpi (PeiServices, mPpiDescriptorList);

  IDS_HDT_CONSOLE (MAIN_FLOW, "    HgpiMemPeiEntry Exit\n");

  return Status;
}
