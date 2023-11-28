
#include <Uefi.h>
#include <PiPei.h>
#include "Pi/PiHob.h"
#include <AGESA.h>
#include "Library/AmdCalloutLib.h"
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Ppi/Smbus2.h>
#include <Ppi/AmdMemPpi.h>
#include <PlatformMemoryConfiguration.h>
#include "AgesaMemPei.h"
#include <Ppi/AmdMemPpi.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/PcdLib.h>
#include <AmdBoardId.h>
#include "Filecode.h"
#include <Token.h>                                                 // winddy +

// winddy + >>
#ifndef SPD_MUX_GROUP0_CTRL_ADDR
#define SPD_MUX_GROUP0_CTRL_ADDR    0xFF  
#endif

#ifndef SPD_MUX_GROUP0_CHANNEL
#define SPD_MUX_GROUP0_CHANNEL      1
#endif

#ifndef SPD_MUX_GROUP1_CTRL_ADDR
#define SPD_MUX_GROUP1_CTRL_ADDR    0xFF      
#endif

#ifndef SPD_MUX_GROUP1_CHANNEL
#define SPD_MUX_GROUP1_CHANNEL      2
#endif

#ifndef SPD_MUX_GROUP2_CTRL_ADDR
#define SPD_MUX_GROUP2_CTRL_ADDR    0xFF      
#endif

#ifndef SPD_MUX_GROUP2_CHANNEL
#define SPD_MUX_GROUP2_CHANNEL      4
#endif

#ifndef SPD_MUX_GROUP3_CTRL_ADDR
#define SPD_MUX_GROUP3_CTRL_ADDR    0xFF      
#endif

#ifndef SPD_MUX_GROUP3_CHANNEL
#define SPD_MUX_GROUP3_CHANNEL      8
#endif

#ifndef BLDCFG_I2C_MUX_ADRRESS
#define BLDCFG_I2C_MUX_ADRRESS      0xE0
#endif

#ifndef BLDCF_SPD_CH_A_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_A_DIMM0_ADDRESS      0xA0
#endif
#ifndef BLDCF_SPD_CH_A_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_A_DIMM1_ADDRESS      0xA2
#endif

#ifndef BLDCF_SPD_CH_B_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_B_DIMM0_ADDRESS      0xA4
#endif
#ifndef BLDCF_SPD_CH_B_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_B_DIMM1_ADDRESS      0xA6
#endif

#ifndef BLDCF_SPD_CH_C_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_C_DIMM0_ADDRESS      0xA8
#endif
#ifndef BLDCF_SPD_CH_C_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_C_DIMM1_ADDRESS      0xAA
#endif

#ifndef BLDCF_SPD_CH_D_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_D_DIMM0_ADDRESS      0xAC
#endif
#ifndef BLDCF_SPD_CH_D_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_D_DIMM1_ADDRESS      0xAE
#endif

#ifndef BLDCF_SPD_CH_E_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_E_DIMM0_ADDRESS      0xA0
#endif
#ifndef BLDCF_SPD_CH_E_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_E_DIMM1_ADDRESS      0xA2
#endif

#ifndef BLDCF_SPD_CH_F_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_F_DIMM0_ADDRESS      0xA4
#endif
#ifndef BLDCF_SPD_CH_F_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_F_DIMM1_ADDRESS      0xA6
#endif

#ifndef BLDCF_SPD_CH_G_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_G_DIMM0_ADDRESS      0xA8
#endif
#ifndef BLDCF_SPD_CH_G_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_G_DIMM1_ADDRESS      0xAA
#endif

#ifndef BLDCF_SPD_CH_H_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_H_DIMM0_ADDRESS      0xAC
#endif
#ifndef BLDCF_SPD_CH_H_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_H_DIMM1_ADDRESS      0xAE
#endif
// winddy + <<



#define FILECODE MEM_AMDMEMPEI_AGESAPEI_FILECODE

//#define FCH_IOMUXx89_UART0_RTS_L_EGPIO137   0xFED80D89ul         // winddy -
//#define FCH_GPIO_137_PIN (0xfed81500UL + 0x224)                  // winddy -


// winddy - >>
AGESA_DIMM_INFO_SMBUS DimmSmbus[] =
{
  //  DIMM_SPD_MAP
  {2, 0, 0xA0},
  {2, 1, 0xA2},
  {3, 0, 0xA4},
  {3, 1, 0xA6},
  {6, 0, 0xA0},
  {6, 1, 0xA2},
  {7, 0, 0xA4},
  {7, 1, 0xA6},  
  {0xFF, 0xFF, 0xFF}
};
// winddy - <<


EFI_STATUS GetSocketAndDimmInfo(UINT8 *NumberOfSocket, UINT8 *NumberOfDimm)
{
    HYGON_BOARD_ID       PlatformSelect;
    
	if(NumberOfSocket == NULL || NumberOfDimm == NULL) return EFI_INVALID_PARAMETER;
	
    PlatformSelect = PcdGet8(PcdPlatformSelect);
    DEBUG((EFI_D_INFO, "PlatformSelect=%d\n", PlatformSelect));
    
    switch (PlatformSelect)
    {
    //Hygon35N16
    case 4:
        *NumberOfSocket = 1;
        *NumberOfDimm   = 0x10;
        break;
        
    //Hygon65N32
    case 5:
        *NumberOfSocket = 2;
        *NumberOfDimm   = 0x10;
        break;
        
    //HygonDM1SLT
    case 11:
        *NumberOfSocket = 1;
        *NumberOfDimm   = 0x4;
        break;
        
    //Hygon52D16
    case 13:
        *NumberOfSocket = 2;
        *NumberOfDimm   = 0x8;
        break;
        
    //Hygon35N16SL1r2
    case 16:
        *NumberOfSocket = 1;
        *NumberOfDimm   = 0x8;
        break;
        
    //Hygon65N32SL1r2
    case 17:
    case 20: // w550-H220
        *NumberOfSocket = 2;
        *NumberOfDimm   = 0x8;
        break;
        
    default:
        ASSERT(FALSE);
		return EFI_INVALID_PARAMETER;
    }
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
    EFI_STATUS                          Status;
    EFI_PEI_SMBUS2_PPI                  *SmbusPpi = NULL;
    EFI_SMBUS_DEVICE_ADDRESS            SmbusSlaveAddress;
    UINTN                               SmbusLength = 0;
    UINTN                               SmbusOffset = 0;

    (**PeiServices).LocatePpi(PeiServices, &gEfiPeiSmbus2PpiGuid, 0, NULL, &SmbusPpi);
    SmbusLength = 1;
    SmbusOffset = 0;
    SmbusSlaveAddress.SmbusDeviceAddress = (I2CMuxAddress >> 1);
    Status = SmbusPpi->Execute(SmbusPpi, SmbusSlaveAddress, 
            I2CCommand,
            EfiSmbusWriteByte, 
            FALSE, 
            &SmbusLength,
            &I2CChannelAddress);
    DEBUG((EFI_D_INFO, "SmbusI2CMuxSelection Status = %r (%X %X %X)\n",Status, \
      I2CMuxAddress, I2CCommand, I2CChannelAddress));
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
    EFI_STATUS                          Status;
    EFI_PEI_SMBUS2_PPI                  *SmbusPpi = NULL;
    EFI_SMBUS_DEVICE_ADDRESS            SmbusSlaveAddress;
    UINTN                               SmbusLength = 0;
    UINTN                               SmbusOffset = 0;
    UINT8                               Buffer = 0;


    (**PeiServices).LocatePpi(PeiServices, &gEfiPeiSmbus2PpiGuid, 0, NULL, &SmbusPpi);
    //A dummy write to the slave address of 6E is made to switch the page address to 1, prior to the SPD reading commands.
    //Upon completion, another dummy write to the slave address of 6C is made to restore the original page address

    SmbusLength = 1;
    SmbusOffset = 0;
    if (Page == 1)
    {
        SmbusSlaveAddress.SmbusDeviceAddress = (0x6E >> 1);
        Status = SmbusPpi->Execute(SmbusPpi, SmbusSlaveAddress, SmbusOffset,
                EfiSmbusWriteByte, FALSE, &SmbusLength,
                &Buffer );
        //IDS_HDT_CONSOLE (MAIN_FLOW, "\n\n Change SMBUS to page 1 \n");
    }
    else
    {
        SmbusSlaveAddress.SmbusDeviceAddress = (0x6C >> 1);
        Status = SmbusPpi->Execute(SmbusPpi, SmbusSlaveAddress, SmbusOffset,
                EfiSmbusWriteByte, FALSE, &SmbusLength,
                &Buffer );
        //IDS_HDT_CONSOLE (MAIN_FLOW, "\n\n Change SMBUS to page 0 \n");
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
        IN OUT  AGESA_READ_SPD_PARAMS *SpdData
)
{
    EFI_STATUS                          Status;
    EFI_SMBUS_DEVICE_ADDRESS            SmbusSlaveAddress;
    UINTN                               SmbusLength = 0;
    UINTN                               SmbusOffset = 0;
    EFI_PEI_SMBUS2_PPI                  *SmbusPpi = NULL;
    UINTN                               Index = 0;
    UINT8                               *Buffer2 = NULL;
    UINT8                               DramType = 0;   // B is DDR3, C is DDR4

    // Locate Smbus Device
    Status = (**PeiServices).LocatePpi( PeiServices, &gEfiPeiSmbus2PpiGuid, 0, NULL, &SmbusPpi );
    if (EFI_ERROR(Status))  return Status;

    SmbusSlaveAddress.SmbusDeviceAddress = (SlaveAddr >> 1);
    SmbusLength = 1;
    SmbusOffset = 2;    // Key Byte / DRAM Device Type    
    Status = SmbusPpi->Execute( SmbusPpi, SmbusSlaveAddress, SmbusOffset,
            EfiSmbusReadByte, FALSE, &SmbusLength,
            &DramType );
    //if SPD data not available at location 2, then we return status
    if (EFI_DEVICE_ERROR == Status)
    {
        DEBUG((EFI_D_ERROR, "MemInitReadSpd.Execute Status = %r\n", Status));
        return Status;
    }


    Status = EFI_SUCCESS;
    SmbusLength = 1;
    SmbusOffset = 0;
    Buffer2 = SpdData->Buffer;

    //It's DDR4
    //The 256 bytes in Page 0
    for ( Index = 0; Index < DIMM_SPD_DATA_256; Index++ )
    {
        Status = SmbusPpi->Execute( SmbusPpi, SmbusSlaveAddress, SmbusOffset,
                EfiSmbusReadByte, FALSE, &SmbusLength,
                Buffer2 );
        SmbusOffset += 1;
        Buffer2++;
    }

    //Change to page 1
    SmbusPageChange(PeiServices, 1);
    //The 256 bytes in Page 1
    for ( Index = 0; Index < DIMM_SPD_DATA_256; Index++ )
    {
        Status = SmbusPpi->Execute( SmbusPpi, SmbusSlaveAddress, SmbusOffset,
                EfiSmbusReadByte, FALSE, &SmbusLength,
                Buffer2 );
        SmbusOffset += 1;
        Buffer2++;
    }

    //Change to Page 0
    SmbusPageChange(PeiServices, 0);


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
        IN      PEI_AMD_PLATFORM_DIMM_SPD_PPI      *This,
        IN OUT  AGESA_READ_SPD_PARAMS *SpdData
)
{
    UINTN                 Index;
    UINT8                 SmbusAddress;
    EFI_STATUS            Status;
    AGESA_DIMM_INFO_SMBUS *pDimmSmbus;
    HYGON_BOARD_ID        PlatformSelect;
	UINT8                 TotalNumberOfSocket;
	UINT8                 TotalNumberOfDimm;
    
	Status = GetSocketAndDimmInfo(&TotalNumberOfSocket, &TotalNumberOfDimm);
	if (EFI_ERROR (Status)) {
	    DEBUG((EFI_D_ERROR, "ASSERT_EFI_ERROR (Status = %r)\n", Status));
	    ASSERT (!EFI_ERROR (Status));
	    return Status;
	}
    SmbusAddress = 0;
    Status = EFI_INVALID_PARAMETER;

    PlatformSelect = PcdGet8(PcdPlatformSelect);

// winddy - >>    
//    if(PlatformSelect == HYGON_52D16) {
//        pDimmSmbus = &DimmSmbus52D16[0];
//    } else {
        pDimmSmbus = &DimmSmbus[0];
//    }
// winddy - <<

// winddy - >>
//	  if ((PlatformSelect == HYGON_65N32) || (PlatformSelect == HYGON_52D16) || (PlatformSelect == HYGON_65N32SL1R2)) {
//        // add for Hygon 65N32 C01 board
//        MmioWrite8(FCH_IOMUXx89_UART0_RTS_L_EGPIO137, 02);
//        MmioWrite32(FCH_GPIO_137_PIN, BIT23|BIT22);
//    }
// winddy - <<
    
    for (Index = 0; pDimmSmbus[Index].SmbusAddress != 0xFF; Index++)
    {
        if ((SpdData->MemChannelId == pDimmSmbus[Index].ChannelId)
                && (SpdData->DimmId == pDimmSmbus[Index].DimmId))
        {
            SmbusAddress = pDimmSmbus[Index].SmbusAddress;
            DEBUG((EFI_D_INFO, "SMBUS address found = %x\n",SmbusAddress));
            Status = EFI_SUCCESS;
            break;
        }
    }

    if (SpdData->SocketId > (TotalNumberOfSocket - 1)) return Status;

    if (Status == EFI_INVALID_PARAMETER)
    {
        return Status;
    }  

    //
    // Switch the channel based off following information before initiating SPD read.
    //
    DEBUG((EFI_D_INFO, "Hygon PlatformSelect = %x\n", PlatformSelect));

// winddy - >>
    if(PlatformSelect != HYGON_DM1SLT){
      if (SpdData->SocketId == 0) {
          if ((SpdData->MemChannelId >= 0) && (SpdData->MemChannelId < 4)) {
              SmbusI2CMuxSelection (PeiServices, BLDCFG_I2C_MUX_ADRRESS, SPD_MUX_GROUP0_CTRL_ADDR, SPD_MUX_GROUP0_CHANNEL);
          }
          if ((SpdData->MemChannelId >= 4) && (SpdData->MemChannelId < 8)) {
              SmbusI2CMuxSelection (PeiServices, BLDCFG_I2C_MUX_ADRRESS, SPD_MUX_GROUP1_CTRL_ADDR, SPD_MUX_GROUP1_CHANNEL);
          }
      } else if (SpdData->SocketId == 1) {
          if ((SpdData->MemChannelId >= 0) && (SpdData->MemChannelId < 4)) {
              SmbusI2CMuxSelection (PeiServices, BLDCFG_I2C_MUX_ADRRESS, SPD_MUX_GROUP2_CTRL_ADDR, SPD_MUX_GROUP2_CHANNEL);
          }
          if ((SpdData->MemChannelId >= 4) && (SpdData->MemChannelId < 8)) {
              SmbusI2CMuxSelection (PeiServices, BLDCFG_I2C_MUX_ADRRESS, SPD_MUX_GROUP3_CTRL_ADDR, SPD_MUX_GROUP3_CHANNEL);
          }
      }
    }
// winddy - <<

    Status = MemInitReadSpd(PeiServices, SmbusAddress, SpdData);

    DEBUG((EFI_D_INFO, "\n --Sckt-Ch-Dimm=[%x-%x-%x]--Dimm Addr %X  ---SPD Read Sts %r \n", \
            SpdData->SocketId,SpdData->MemChannelId,SpdData->DimmId,SmbusAddress, Status));
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
EFI_STATUS ReadSaveDimmSerial(
        IN   CONST EFI_PEI_SERVICES    **PeiServices,
        UINT8                          SlaveAddr,
        UINT8                          *Buffer,
        UINT8                          BufferIndex)
{

    UINT8                       Buffer2, Index;
    EFI_STATUS                  Status;
    EFI_SMBUS_DEVICE_ADDRESS    SmbusSlaveAddress;
    EFI_PEI_SMBUS2_PPI          *SmbusPpi = NULL;
    UINTN                       SmbusLength = 0;
    UINTN                       SmbusOffset = 0;
    UINT8                       DramType = 0;

    //Check if device Present by reading offset 2
    //if not present return
    //if device present, read the device serial
    //Update Buffer 
    //Returns error if cannot locate smbus ppi
    SmbusLength = 1;
    SmbusOffset = 2;    // Key Byte / DRAM Device Type

    DEBUG((EFI_D_INFO, "\n ReadSaveDimmSerial entry \n"));

    Status = (**PeiServices).LocatePpi( PeiServices,
            &gEfiPeiSmbus2PpiGuid,
            0, NULL, &SmbusPpi );

    if (EFI_ERROR(Status))  return  EFI_NOT_FOUND;

    DEBUG((EFI_D_INFO, "\n SlaveAddr = %x \n", SlaveAddr));
    DEBUG((EFI_D_INFO, "\n BufferIndex = %x \n", BufferIndex));

    SmbusSlaveAddress.SmbusDeviceAddress = (SlaveAddr >> 1);

    Status = SmbusPpi->Execute( SmbusPpi, SmbusSlaveAddress, SmbusOffset,
            EfiSmbusReadByte, FALSE, &SmbusLength,
            &DramType );
    //if SPD data is not available at offset 2, then we return status
    if (EFI_DEVICE_ERROR == Status)
        return Status;

    Status = EFI_SUCCESS;
    SmbusSlaveAddress.SmbusDeviceAddress = (SlaveAddr >> 1);
    SmbusLength = 1;

    //It's DDR4
    DEBUG((EFI_D_INFO, "\n DDR4 Serial number = \n"));

    //Change to page 1
    SmbusPageChange(PeiServices, 1);

    SmbusOffset = DIMM_SPD_SERIAL_NO_OFFSET_DDR4;
    for ( Index = 0; Index < DIMM_SPD_SERIAL_NO_LENGTH; Index++ )
    {
        Status = SmbusPpi->Execute( SmbusPpi, SmbusSlaveAddress, SmbusOffset,
                EfiSmbusReadByte, FALSE, &SmbusLength,
                &Buffer2 );
        DEBUG((EFI_D_INFO, " %x ", Buffer2));
        SmbusOffset += 1;
        Buffer[BufferIndex * DIMM_SPD_SERIAL_NO_LENGTH + Index] = Buffer2;
    }

    //Change to Page 0
    SmbusPageChange(PeiServices, 0);

    return EFI_SUCCESS;
}


/**
 *  AgesaPeiEntry
 *
 *  Description:
 *    This function will prepare the platform configuarion for AGESA use.
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
AgesaMemPeiEntry (
        IN       EFI_PEI_FILE_HANDLE  FileHandle,
        IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_STATUS                                  Status = EFI_SUCCESS;
    EFI_PEI_PPI_DESCRIPTOR                      *mPpiDescriptorList;
    PEI_AMD_PLATFORM_DIMM_SPD_PPI               *mPeiAmdPlatformDimmSpdPpi;
    UINT8                                       TotalNumberOfSocket;
    UINT8                                       TotalNumberOfDimm;

    DEBUG((EFI_D_INFO, "AgesaMemPeiEntry Entry\n"));

    // Allocate memory for the PPI descriptor
    Status = (*PeiServices)->AllocatePool (
            PeiServices,
            sizeof (EFI_PEI_PPI_DESCRIPTOR),
            (VOID **)&mPpiDescriptorList
            );
    if (EFI_ERROR (Status)) {
        DEBUG((EFI_D_ERROR, "ASSERT_EFI_ERROR (Status = %r)\n", Status));
        ASSERT (!EFI_ERROR (Status));
        return Status;
    }
    
    // Allocate memory for the PPI descriptor
    Status = (*PeiServices)->AllocatePool (
            PeiServices,
            sizeof (PEI_AMD_PLATFORM_DIMM_SPD_PPI),
            (VOID **)&mPeiAmdPlatformDimmSpdPpi
            );
    if (EFI_ERROR (Status)) {
        DEBUG((EFI_D_ERROR, "ASSERT_EFI_ERROR (Status = %r)\n", Status));
        ASSERT (!EFI_ERROR (Status));
        return Status;
    }
    
	Status = GetSocketAndDimmInfo(&TotalNumberOfSocket, &TotalNumberOfDimm);
	if (EFI_ERROR (Status)) {
	    DEBUG((EFI_D_ERROR, "ASSERT_EFI_ERROR (Status = %r)\n", Status));
	    ASSERT (!EFI_ERROR(Status));
	    return Status;
	}
	
    mPeiAmdPlatformDimmSpdPpi->TotalNumberOfSocket = TotalNumberOfSocket;
    mPeiAmdPlatformDimmSpdPpi->TotalNumberOfDimms  = TotalNumberOfDimm;
    mPeiAmdPlatformDimmSpdPpi->PlatformDimmSpdRead = PlatformDimmSpdRead;

    mPpiDescriptorList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
    mPpiDescriptorList->Guid  = &gAmdPlatformDimmSpdPpiGuid;
    mPpiDescriptorList->Ppi   = mPeiAmdPlatformDimmSpdPpi;

    Status = (*PeiServices)->InstallPpi(PeiServices, mPpiDescriptorList);

    DEBUG((EFI_D_INFO, "AgesaMemPeiEntry Exit\n"));

    return  Status;
}
