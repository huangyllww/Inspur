
#include "SpiCommon.h"
#include <AMD.h>


#define SPI_PRIVATE_DATA_SIGNATURE  SIGNATURE_32('S', 'P', 'I', 'P')
#define SPI_INSTANCE_FROM_SPIPROTOCOL(a)  CR(a, SPI_INSTANCE, SpiProtocol, SPI_PRIVATE_DATA_SIGNATURE)


BOOLEAN
WaitForSpiDeviceComplete (
  );

UINT32
FchReadSpiId (
  IN       BOOLEAN    Flag
  );

AGESA_STATUS
FchSpiTransfer (
  IN       UINT8    PrefixCode,
  IN       UINT8    Opcode,
  IN OUT   UINT8    *DataPtr,
  IN       UINT8    *AddressPtr,
  IN       UINT8    Length,
  IN       BOOLEAN  WriteFlag,
  IN       BOOLEAN  AddressFlag,
  IN       BOOLEAN  DataFlag,
  IN       BOOLEAN  FinishedFlag
  );


EFI_STATUS
EFIAPI
SpiInit (
  IN EFI_SPI_PROTOCOL     *This,
  IN SPI_INIT_TABLE       *InitTable
  )
{
  SPI_INSTANCE  *SpiInstance;
  UINT32        FlashId;

  SpiInstance = SPI_INSTANCE_FROM_SPIPROTOCOL(This);

  if(SpiInstance->FlashId == 0){
    SpiInstance->FlashId = FchReadSpiId (TRUE);
    DEBUG((EFI_D_INFO, "Spi Device ID:%X\n", SpiInstance->FlashId));
  }

  if (InitTable == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FlashId = (InitTable->DeviceId1 << 16) | (InitTable->DeviceId0 << 8) | InitTable->VendorId;
  DEBUG((EFI_D_INFO, "FlashPartId:%X\n", FlashId));  
  if(FlashId != SpiInstance->FlashId){
    return EFI_INVALID_PARAMETER;
  }
  
  CopyMem(&SpiInstance->SpiInitTable, InitTable, sizeof(SPI_INIT_TABLE));
  DEBUG((EFI_D_INFO, "Match\n"));

  if(SpiInstance->SpiInitTable.FlashSize != SpiInstance->SpiInitTable.BiosSize){
    SpiInstance->SizeFix = (UINT32)(SpiInstance->SpiInitTable.FlashSize - SpiInstance->SpiInitTable.BiosSize);
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
SpiLock (
  IN EFI_SPI_PROTOCOL  *This
  )
{
#if 0
  SPI_INSTANCE  *SpiInstance;
  UINT32        SpiBar;

  SpiInstance = SPI_INSTANCE_FROM_SPIPROTOCOL(This);
  SpiBar      = SpiInstance->SpiBar;

  MmioOr16(SpiBar + SPI0_SPIS_REG, SPI0_SPIS_LOCKDOWN);

  if ((MmioRead16(SpiBar + SPI0_SPIS_REG) & SPI0_SPIS_LOCKDOWN) == 0) {
    return EFI_DEVICE_ERROR;
  }
#endif
  return EFI_SUCCESS;
}






BOOLEAN
WaitForSpiCycleDone (
  IN     EFI_SPI_PROTOCOL   *This
  )
{
#if 0
  UINT64        WaitTicks;
  UINT64        WaitCount;
  UINT16        Data16;
  SPI_INSTANCE  *SpiInstance;
  UINT32        SpiBar;
  SpiInstance = SPI_INSTANCE_FROM_SPIPROTOCOL(This);
  SpiBar      = SpiInstance->SpiBar;
  WaitCount   = WAIT_TIME / WAIT_PERIOD;
  for (WaitTicks = 0; WaitTicks < WaitCount; WaitTicks++) {
    Data16 = MmioRead16(SpiBar + SPI0_SPIS_REG);

    if(((Data16 & SPI0_SPIS_CYCLE_PROGRESS) == 0) &&((Data16 & SPI0_SPIS_CYCLE_DONE)== SPI0_SPIS_CYCLE_DONE)){
      MmioOr16(SpiBar + SPI0_SPIS_REG, SPI0_SPIS_CYCLE_DONE);
      return TRUE;
    }
    MicroSecondDelay(WAIT_PERIOD);
  }
#endif
  return FALSE;
}




EFI_STATUS
EFIAPI
SpiExec (
    IN     EFI_SPI_PROTOCOL   *This,
    IN     UINT8              OpcodeIndex,
    IN     UINT8              PrefixOpcodeIndex,
    IN     BOOLEAN            DataCycle,
    IN     BOOLEAN            Atomic,
    IN     BOOLEAN            ShiftOut,
    IN     UINTN              Address,
    IN     UINT32             DataByteCount,
    IN OUT UINT8              *Buffer,
    IN     SPI_REGION_TYPE    SpiRegionType			// Do not care SpiRegionType
  )
{
  EFI_STATUS     Status = EFI_SUCCESS;
  SPI_INSTANCE   *SpiInstance;
  UINTN          SpiBiosSize;
  UINT32         SpiDataCount;
  UINT8          OpCode;
  SPI_OPERATION  Operation;
  UINT8          PrefixOp;
  UINT8          AddrLow8;


//DEBUG((EFI_D_INFO, "%a(%X,%X,%X,%X,%X,%X,%X,%X)\n", __FUNCTION__, OpcodeIndex, \
//          PrefixOpcodeIndex, DataCycle, Atomic, ShiftOut, \
//          Address, DataByteCount, Buffer));

  SpiInstance = SPI_INSTANCE_FROM_SPIPROTOCOL(This);

  if ((OpcodeIndex >= SPI_NUM_OPCODE) || (PrefixOpcodeIndex >= SPI_NUM_PREFIX_OPCODE)) {
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

//if (!WaitForSpiDeviceComplete(This)) {
//  Status = EFI_DEVICE_ERROR;
//  goto ProcExit;
//}

  Address    -= SpiInstance->SizeFix;
  SpiBiosSize = SpiInstance->SpiInitTable.BiosSize;
  Operation   = SpiInstance->SpiInitTable.OpcodeMenu[OpcodeIndex].Operation;
  OpCode      = SpiInstance->SpiInitTable.OpcodeMenu[OpcodeIndex].Code;
  PrefixOp    = SpiInstance->SpiInitTable.PrefixOpcode[PrefixOpcodeIndex];
//DEBUG((EFI_D_INFO, "(L%d) A:%X %X %X\n", __LINE__, Address, Operation, OpCode));

  if (OpCode == 0 || SpiBiosSize == 0) {
    ASSERT (FALSE);
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  if (Operation == EnumSpiOperationReadData) {

     UINTN                i;
     UINTN                PageNumber = 0;
     UINT8                RemainSize = 0;
     UINTN                *DstPtrN;
     UINTN                *SrcPtrN;
     UINT8                *DstPtr8;
     UINT8                *SrcPtr8;
   
     PageNumber = (UINTN) (DataByteCount  / sizeof(UINTN));
     RemainSize = (UINT8) ((DataByteCount) % sizeof(UINTN));
   
     DstPtrN = (UINTN *)((UINTN)Buffer);
     SrcPtrN = (UINTN *)((UINTN)Address + _PCD_VALUE_PcdFlashAreaBaseAddress);

     DEBUG((EFI_D_INFO, "(L%d) %X -> %X\n", __LINE__, SrcPtrN, DstPtrN));
   
     DstPtr8 = (UINT8 *)(DstPtrN + PageNumber);
     SrcPtr8 = (UINT8 *)(SrcPtrN + PageNumber);
     for (i = 0; i < PageNumber; i ++) {
       *(DstPtrN + i) = *(SrcPtrN + i);
     }
   
     for (i = 0; i < RemainSize; i ++) {
       *(DstPtr8 + i) = *(SrcPtr8 + i);
     }

    Status = EFI_SUCCESS;
    
  }else if(Operation == EnumSpiOperationErase_4K_Byte){ 
    UINTN                NumBytes = DataByteCount;
    UINTN                DstAddress;
    UINTN                BlockEraseSize = SIZE_4KB;

    DstAddress = Address;
    if(NumBytes <= SIZE_4KB){
      NumBytes = SIZE_4KB;
    }
    while ((INTN)NumBytes > 0){
      if (DstAddress + BlockEraseSize <= _PCD_VALUE_PcdFlashAreaSize){
        Status = FchSpiTransfer(
                   PrefixOp,
                   OpCode,
                   NULL,
                   (UINT8*)DstAddress,
                   0,
                   FALSE,
                   TRUE,
                   FALSE,
                   TRUE
                   );
        if (Status != AGESA_SUCCESS) return Status;
      }
      DstAddress += BlockEraseSize;
      NumBytes   -= BlockEraseSize;
    }
  }  else if(Operation == EnumSpiOperationProgramData_1_Byte){
    if(DataByteCount == 0){
      Status = EFI_INVALID_PARAMETER;
      goto ProcExit;
    }

// [w25q256]        
// Page Program (02h)
//   XXXX00 -> 256
//
// The Page Program instruction allows from one byte to 256 bytes (a page) of data to be programmed at
// previously erased (FFh) memory locations. 
// If an entire 256 byte page is to be programmed, the last address byte (the 8 least significant address bits)
// should be set to 0. If the last address byte is not zero, and the number of clocks exceeds the remaining
// page length, the addressing will wrap to the beginning of the page. In some cases, less than 256 bytes (a
// partial page) can be programmed without having any effect on other bytes within the same page. One
// condition to perform a partial page program is that the number of clocks cannot exceed the remaining
// page length. If more than 256 bytes are sent to the device the addressing will wrap to the beginning of the
// page and overwrite previously sent data

// DumpMem8(Buffer, DataByteCount);

// try max 16 bytes one time.

    while(DataByteCount){
      AddrLow8 = (UINT8)(Address & 0xFF);
      SpiDataCount = DataByteCount;
      if(SpiDataCount > 16){
        SpiDataCount = 16;
      }
      if(AddrLow8 + SpiDataCount > 256){
        SpiDataCount = 256 - AddrLow8;
      }

      FchSpiTransfer (
        PrefixOp,
        OpCode,
        Buffer,
        (UINT8*)Address,
        SpiDataCount-1,
        TRUE,
        TRUE,
        TRUE,
        TRUE 
        );
      Address += SpiDataCount;
      Buffer  += SpiDataCount;
      DataByteCount -= SpiDataCount; 
      
    }

  }else if(Operation == EnumSpiOperationJedecId){
    if(DataByteCount == 3 && Buffer != NULL){
      CopyMem(Buffer, &SpiInstance->FlashId, 3);
    } else {
      Status = EFI_INVALID_PARAMETER;
    }
  } else {
    DEBUG((EFI_D_ERROR, "unhandled operation:%X\n", Operation));
  }


ProcExit:
  return Status;
}



SPI_INSTANCE  gSpiInstance = {
  SPI_PRIVATE_DATA_SIGNATURE,
  NULL,
  {
    SpiInit,
    SpiLock,
    SpiExec
  },
  {0},
  0,
  0
};


VOID
SpiVirtualddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  gRT->ConvertPointer (0, (VOID**)&(gSpiInstance.SpiProtocol.Init));
  gRT->ConvertPointer (0, (VOID**)&(gSpiInstance.SpiProtocol.Lock));
  gRT->ConvertPointer (0, (VOID**)&(gSpiInstance.SpiProtocol.Execute));
  gRT->ConvertPointer (0, (VOID**)&(gSpiInstance));
}



