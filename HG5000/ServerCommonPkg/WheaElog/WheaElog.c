/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  WheaElog.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/


#include "WheaElog.h"


#define MAX_BOOT_ERROR_LOG_SIZE     0x8000
#define WHEA_ERROR_RECORD_SWSMI     0xF3

WHEA_PARAMETER_REGION               *mParamBlock  = NULL; // Params
UINT8                               *mRecordBlock = NULL; // ELAR

EFI_WHEA_ELOG_STATISTICS  mElog = {
  NULL, // NvAcc
  (EFI_PHYSICAL_ADDRESS)(UINTN)-1,     // BaseAddress of ELOG
  0,    // FvBase
  0,    // FvSize
  0,    // Length
  0,    // TotalRecordBytes
  0,    // FreeRecordBytes
  (EFI_PHYSICAL_ADDRESS)(UINTN)-1,     // NextRecordAddress
  0
};

//
// WHEA Serialization Table
//
EFI_ACPI_WHEA_SERIALIZATION_ACTION mERST[16] = {
    {
      ACTION_BEGIN_WRITE_OPERATION,                 // Serialization Action - 0
      INSTRUCTION_WRITE_REGISTER_VALUE,             // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      WHEA_ERROR_SET_RECORD,                             // Value for SetErrorRecord()
      0x0000FFFF                                    // Mask is only valid for 32-bits 
    },
    {
      ACTION_BEGIN_READ_OPERATION,                  // Serialization Action - 1
      INSTRUCTION_WRITE_REGISTER_VALUE,             // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      WHEA_ERROR_GET_RECORD,                             // Value for GetErrorRecord()
      0x0000FFFF                                    // Mask is only valid for 32-bits 
    },
    {
      ACTION_BEGIN_CLEAR_OPERATION,                 // Serialization Action - 2
      INSTRUCTION_WRITE_REGISTER_VALUE,             // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      WHEA_ERROR_CLEAR_RECORD,                           // Value for SetErrorRecord()
      0x0000FFFF                                    // Mask is only valid for 32-bits 
    },
    {
      ACTION_END_OPERATION,                         // Serialization Action - 3
      INSTRUCTION_NO_OPERATION,                     // Instruction
      FLAG_PRESERVE_REGISTER,                       // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      WHEA_ERROR_END_OP,                            // Value for SetErrorRecord()
      0x0000FFFF                                    // Mask is only valid for 32-bits 
    },
    {
      ACTION_SET_RECORD_OFFSET,                     // Serialization Action - 4
      INSTRUCTION_WRITE_REGISTER,                   // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      0,                                            // Value for SetErrorRecord()
      0xFFFFFFFF                                    // Mask is only valid for 32-bits 
    },
    {
      ACTION_EXECUTE_OPEARTION,                     // Serialization Action - 5
      INSTRUCTION_WRITE_REGISTER_VALUE,             // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterExecute,                          // GAS (QWORD Memory) Address will be filled during boot.
      WHEA_ERROR_RECORD_SWSMI,                      // Value field provides the data to write to SMI port 0xb2
      0x0000FFFF                                    // Mask  16-bit
    },
    {
      ACTION_CHECK_BUSY_STATUS,                     // Serialization Action - 6
      INSTRUCTION_READ_REGISTER_VALUE,                // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      0x00000001,                                            // Value for SetErrorRecord()
      0x00000001                                    // Mask is only valid for 32-bits 
    },
    {
      ACTION_GET_COMMAND_STATUS,                    // Serialization Action - 7
      INSTRUCTION_READ_REGISTER,                    // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      0,                                            // Value for SetErrorRecord()
      0xffffffff                                    // Mask is only valid for 32-bits 
    },
    {
      ACTION_GET_RECORD_IDENTIFIER,                 // Serialization Action - 8
      INSTRUCTION_READ_REGISTER,                    // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      0,                                            // Value for SetErrorRecord()
      0xFFFFFFFFFFFFFFFF                            // Mask is only valid for 32-bits 
    },
    {
      ACTION_SET_RECORD_IDENTIFIER,                 // Serialization Action - 9
      INSTRUCTION_WRITE_REGISTER,                   // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      0,                                            // Value for SetErrorRecord()
      0xFFFFFFFFFFFFFFFF                            // Mask is only valid for 32-bits 
    },
    {
      ACTION_GET_RECORD_COUNT,                      // Serialization Action - 10
      INSTRUCTION_READ_REGISTER,                    // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      0,                                            // Value for SetErrorRecord()
      0xFFFFFFFF                                    // Mask is only valid for 32-bits 
    },
    {
      ACTION_BEGIN_DUMMY_WRITE_OPERATION,           // Serialization Action - 11
      INSTRUCTION_WRITE_REGISTER_VALUE,             // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      WHEA_ERROR_DUMMY_WRITE,                       // Value for Dummy write()
      0xFFFF                                        // Mask is only valid for 32-bits 
    },
    {
      ACTION_GET_NEXT_RECORD_IDENTIFIER,            // Serialization Action - 12
      INSTRUCTION_READ_REGISTER,                    // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      0,                                            // Value for SetErrorRecord()
      0xFFFFFFFF                                    // Mask is only valid for 32-bits 
    },
    {
      ACTION_GET_ERROR_LOG_ADDRESS_RANGE,           // Serialization Action - 13
      INSTRUCTION_READ_REGISTER,                    // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      0,                                            // Value for SetErrorRecord()
      0xFFFFFFFFFFFFFFFF                            // Mask is only valid for 32-bits 
    },
    {
      ACTION_GET_ERROR_LOG_ADDRESS_RANGE_LENGTH,    // Serialization Action - 14
      INSTRUCTION_READ_REGISTER,                    // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      0,                                            // Value for SetErrorRecord()
      0xFFFFFFFFFFFFFFFF                            // Mask is only valid for 32-bits 
    },
    {
      ACTION_GET_ERROR_LOG_ADDRESS_RANGE_ATTRIBUTES,// Serialization Action - 15
      INSTRUCTION_READ_REGISTER,                    // Instruction
      FLAG_NOTHING,                                 // Flags that qualify the instruction
      0x00,                                         // Reserved
      ErstRegisterFiller,                           // GAS (QWORD Memory) Address will be filled during boot.
      0,                                            // Value for SetErrorRecord()
      0xFFFFFFFFFFFFFFFF                            // Mask is only valid for 32-bits 
    }
};






EFI_STATUS
UpdateERST (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINTN                                 ElarSize;

  ElarSize = MAX_BOOT_ERROR_LOG_SIZE;
  Status   = gBS->AllocatePool(EfiReservedMemoryType, ElarSize, &mParamBlock);  
  ASSERT_EFI_ERROR(Status);
  if(EFI_ERROR(Status)){
    return Status;
  }  
  ZeroMem(mParamBlock, ElarSize);

  mRecordBlock = (UINT8 *) mParamBlock + sizeof(WHEA_PARAMETER_REGION);
  ElarSize -= sizeof(WHEA_PARAMETER_REGION);

  mParamBlock->ErrorLogAddressRange       = (UINT64) (UINTN)mRecordBlock;
  mParamBlock->ErrorLogAddressLength      = ElarSize;
  mParamBlock->ErrorLogAddressAttributes  = 0;
  mParamBlock->CurRecordID = (UINT64)(UINTN)-1;
  mParamBlock->RecordID    = 0;
  mParamBlock->RecordCount = 0;

  mERST[0].Register.Address   = (UINT64) (UINTN)&mParamBlock->Command;        // BEGIN_WRITE_OPERATION
  mERST[1].Register.Address   = (UINT64) (UINTN)&mParamBlock->Command;        // BEGIN_READ_OPERATION
  mERST[2].Register.Address   = (UINT64) (UINTN)&mParamBlock->Command;        // BEGIN_CLEAR_OPERATION
  mERST[3].Register.Address   = (UINT64) (UINTN)&mParamBlock->Command;        // END_OPERATION
  mERST[4].Register.Address   = (UINT64) (UINTN)&mParamBlock->LogOffset;      // SET_RECORD_OFFSET
  mERST[5].Register.Address   = PcdGet16(PcdSwSmiCmdPort);                    // ACTION_EXECUTE_OPEARTION

  mERST[6].Register.Address   = (UINT64) (UINTN)&mParamBlock->BusyFlag;        // CHECK_BUSY_STATUS
  mERST[7].Register.Address   = (UINT64) (UINTN)&mParamBlock->Status;          // GET_OPERATION_STATUS
  mERST[8].Register.Address   = (UINT64) (UINTN)&mParamBlock->CurRecordID;    // GET_RECORD_IDENTIFIER
  mERST[9].Register.Address   = (UINT64) (UINTN)&mParamBlock->RecordID;        // SET_RECORD_IDENTIFIER
  mERST[10].Register.Address  = (UINT64) (UINTN)&mParamBlock->RecordCount;    // GET_RECORD_COUNT
  mERST[11].Register.Address  = (UINT64) (UINTN)&mParamBlock->Command;        // BEGIN_DUMMY_WRITE_OPERATION
  mERST[12].Register.Address  = (UINT64) (UINTN)&mParamBlock->NextRecordID;    // GET_NEXT_RECORDID / RESERVED

  mERST[13].Register.Address  = (UINT64) (UINTN)&mParamBlock->ErrorLogAddressRange;       // GET_ERROR_LOG_ADDRESS_RANGE
  mERST[14].Register.Address  = (UINT64) (UINTN)&mParamBlock->ErrorLogAddressLength;     // GET_ERROR_LOG_ADDRESS_RANGE_LENGTH
  mERST[15].Register.Address  = (UINT64) (UINTN)&mParamBlock->ErrorLogAddressAttributes; // GET_ERROR_LOG_ADDRESS_RANGE_ATTRIBUTES

  return Status;
}

EFI_STATUS
WheaElogSwSmiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  )
{
  mParamBlock->BusyFlag = 1;

  switch (mParamBlock->Command) {
    case WHEA_ERROR_SET_RECORD:
      PrstWrite(
        mParamBlock->ErrorLogAddressRange + mParamBlock->LogOffset,
        &mParamBlock->Status
        );
      break;
      
    case WHEA_ERROR_GET_RECORD:
      PrstRead(
        mParamBlock->ErrorLogAddressRange + mParamBlock->LogOffset,
        mParamBlock->RecordID,
        &mParamBlock->Status
        );
      break;
      
    case WHEA_ERROR_CLEAR_RECORD:
      PrstClear(
        mParamBlock->RecordID,
        &mParamBlock->Status
        );
      break;
      
    default: // Unknown Serialization Command 
      break;
  }


  mParamBlock->BusyFlag = 0;  // inform OS that Serialization API has completed

  return EFI_SUCCESS;
}



EFI_STATUS
PrstInit (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_FIRMWARE_VOLUME_HEADER            *FvHeader;
  EFI_FIRMWARE_VOLUME_EXT_HEADER        *ExtHeader;
  UINTN                                 FvSize;
  
  
  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER*)(UINTN)PcdGet32(PcdFlashNvStorageApeiBase);
  FvSize   = PcdGet32(PcdFlashNvStorageApeiSize);
  if(FvHeader == NULL || FvHeader->ExtHeaderOffset == 0 || FvSize == 0){
    DEBUG((EFI_D_ERROR, "Fv(%X,%X)\n", FvHeader, FvSize));
    return EFI_NOT_FOUND; 
  }  

  ExtHeader = (EFI_FIRMWARE_VOLUME_EXT_HEADER*)((UINTN)FvHeader + FvHeader->ExtHeaderOffset);
  if (!CompareGuid(&ExtHeader->FvName, &gEfiWheaElogFvGuid)){
    DEBUG((EFI_D_ERROR, "ExtHeader:%X %g\n", ExtHeader, &ExtHeader->FvName));    
    return EFI_NOT_FOUND;    
  }  

  Status = gBS->LocateProtocol(&gEfiSmmNvMediaAccessProtocolGuid, NULL, &mElog.NvAcc);
  ASSERT(!EFI_ERROR(Status));  

  mElog.FvBase       = (UINT32)(UINTN)FvHeader;
  mElog.FvSize       = (UINT32)FvSize;
  mElog.HeaderLength = FvHeader->ExtHeaderOffset + (UINT16)ExtHeader->ExtHeaderSize;
  mElog.BaseAddress  = (UINTN)FvHeader + mElog.HeaderLength;
  mElog.Length       = (UINT32)(FvSize - mElog.HeaderLength);
  mElog.NextRecordAddress   = (UINTN)-1;
  mElog.FreeRecordBytes     = 0;
  mElog.TotalRecordBytes    = (UINT32)(FvSize - mElog.HeaderLength);

  return EFI_SUCCESS;
}


EFI_STATUS
PrstDefragment (
  VOID
  )
{
  EFI_STATUS                          Status;
  EFI_PHYSICAL_ADDRESS                SrcAddress;
  EFI_PHYSICAL_ADDRESS                LastUsedAddress;
  EFI_PHYSICAL_ADDRESS                DestAddress;
  UINTN                               DestLen;
  VOID                                *Buffer = NULL;
  INTN                                ElogLength;
  EFI_ERROR_RECORD_SERIALIZATION_INFO *SrcRecordHdr;
  UINTN                               SrcRecordLen;
  UINTN                               TotalBlockSize;
  ERROR_RECORD_HEADER                 *OsRecordHdr  = NULL;
  BOOLEAN                             ForceDefrag = FALSE;

  
  ASSERT (mParamBlock != NULL);
  mParamBlock->CurRecordID  = (UINT64)(UINTN)-1;
  mParamBlock->RecordCount  = 0;
  mElog.NextRecordAddress   = (EFI_PHYSICAL_ADDRESS)(UINTN)-1;
  mElog.FreeRecordBytes     = 0;

  if (mElog.NvAcc == NULL) return EFI_NOT_FOUND;

  TotalBlockSize = mElog.FvSize;

  Status = gBS->AllocatePages (
                  AllocateAnyPages,
                  EfiBootServicesData,
                  EFI_SIZE_TO_PAGES(TotalBlockSize),
                  &SrcAddress
                  );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "[PrstDefragment] malloc(%x):%r\n", TotalBlockSize, Status));
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }

  Buffer = (VOID*)(UINTN)SrcAddress;
  ZeroMem (Buffer, TotalBlockSize);

  MmioReadBuffer8(mElog.FvBase, mElog.HeaderLength, (UINT8*)Buffer);
  
  DestAddress  = (EFI_PHYSICAL_ADDRESS)(UINTN)Buffer;
  DestAddress += mElog.HeaderLength;
  DestLen      = mElog.HeaderLength;

  SrcAddress      = mElog.BaseAddress;
  LastUsedAddress = mElog.BaseAddress;
  ElogLength      = mElog.Length;
  while (ElogLength > 0) {
    if ((*(UINT16*)(UINTN)SrcAddress) == 0xFFFF) {
      DEBUG((DEBUG_INFO, "PrstDefragment meet 0xffff at %lX, exit\n", SrcAddress));
      break; // Reached the end of used space
    }

    OsRecordHdr = (ERROR_RECORD_HEADER*)(UINTN)SrcAddress;
    SrcRecordHdr = (EFI_ERROR_RECORD_SERIALIZATION_INFO*)&OsRecordHdr->OSReserved;
    if (SrcRecordHdr->Signature != SIGNATURE_16('E','R') ||
        OsRecordHdr->Signature != SIGNATURE_32('C','P','E','R')) {
      // Look like corrupted, so erase remaining and compact valid records
      ForceDefrag = TRUE;
      break;
    }

    // Found valid signature
    SrcRecordLen = OsRecordHdr->RecordLength;

    if ((SrcRecordHdr->Attributes & M_ATTRIBUTE_MASK) == B_RECORD_INUSE) {
      if (mParamBlock->RecordCount == 0) {
        mParamBlock->CurRecordID = OsRecordHdr->RecordID;
      }
      mParamBlock->RecordCount++;

      MmioReadBuffer8(SrcAddress, SrcRecordLen, (UINT8*)(UINTN)DestAddress);

      DestAddress += SrcRecordLen;
      DestLen += SrcRecordLen;
    } else {
    // Skip all the other records - cleared or corrupted attr
    }
    SrcAddress += SrcRecordLen;
    ElogLength -= SrcRecordLen;
    LastUsedAddress = SrcAddress;
  }

// Free offset for new record from last used space and remaining length
  mElog.NextRecordAddress = LastUsedAddress;
  mElog.FreeRecordBytes   = (UINT32)ElogLength;

// If last used record space beyond 3/4 of the WHEA elog space then defrag, otherwise skip. 
  if (mElog.FreeRecordBytes < TotalBlockSize/4) {
    ForceDefrag = TRUE;
  }

  if (!ForceDefrag) {
    Status = EFI_SUCCESS;
    goto ProcExit;
  }

// Free offset for new record is starting of the compacted used space and remaining length
  SrcAddress = mElog.FvBase;
  mElog.NextRecordAddress = SrcAddress + DestLen;
  mElog.FreeRecordBytes   = (UINT32)(TotalBlockSize - DestLen);

  Status = mElog.NvAcc->Erase(mElog.NvAcc, mElog.FvBase, mElog.FvSize, SPI_MEDIA_TYPE);
  DEBUG((DEBUG_INFO, "[PrstDefragment] Erase:%r\n",Status));
  Status = mElog.NvAcc->Write(mElog.NvAcc, mElog.FvBase, (UINT8*)Buffer, DestLen, SPI_MEDIA_TYPE);
  DEBUG((DEBUG_INFO, "[PrstDefragment] Write:%r\n",Status));
  

ProcExit:
  if(Buffer != NULL){
    gBS->FreePages((EFI_PHYSICAL_ADDRESS)(UINTN)Buffer, EFI_SIZE_TO_PAGES(TotalBlockSize));
  }  
  return Status;
}




VOID
PrstSearch (
  UINT64                RecordID,
  EFI_PHYSICAL_ADDRESS  *PhyAddress,
  UINT64                *Status,
  BOOLEAN               FindNextRecordID
  )
{
  EFI_PHYSICAL_ADDRESS                Record;
  EFI_ERROR_RECORD_SERIALIZATION_INFO *RecordHdr;  
  UINT32                              RecordLen;
  INTN                                ElogLen;
  ERROR_RECORD_HEADER                 *OsRecordHdr;

  *PhyAddress = 0;  // not found, null address
  *Status = 5;      // Record not found 
  if (mElog.NvAcc == NULL) return;

  Record = mElog.BaseAddress;
  OsRecordHdr = (ERROR_RECORD_HEADER*)mElog.BaseAddress;
  RecordHdr   = (EFI_ERROR_RECORD_SERIALIZATION_INFO*)&OsRecordHdr->OSReserved;
  if (RecordHdr->Signature != SIGNATURE_16('E', 'R')) {
    *Status = 4; // ELOG empty
    return ;
  }

  ElogLen = mElog.Length;
  while ((ElogLen > 0) 
    && ((RecordHdr->Signature == SIGNATURE_16('E', 'R'))
      &&(OsRecordHdr->Signature == SIGNATURE_32('C', 'P', 'E', 'R')))) {
    if ((RecordHdr->Attributes & M_ATTRIBUTE_MASK) == B_RECORD_INUSE) {
      if (*PhyAddress != 0) {
      // Looking for next record only
        mParamBlock->CurRecordID = OsRecordHdr->RecordID;
        return;
      }
      if ((OsRecordHdr->RecordID == RecordID)) {
        *PhyAddress = Record;
        *Status = 0;  // Success, Record found 
        if (!FindNextRecordID) {
          return ;
        }      
      }
    }

    //
    // Jump to next record
    //
    RecordLen = OsRecordHdr->RecordLength;
    Record += RecordLen;
    ElogLen -= RecordLen;
    OsRecordHdr = (ERROR_RECORD_HEADER *)Record;
    RecordHdr = (EFI_ERROR_RECORD_SERIALIZATION_INFO *)&OsRecordHdr->OSReserved;
  }

  if (*PhyAddress != 0) {
  // Try to find next record ID, but none found
    mParamBlock->CurRecordID = (UINT64) (UINTN)-1;
    return;
  }

  *Status = 5;  // Record not found 
  return ;
}



VOID
PrstRead (
  EFI_PHYSICAL_ADDRESS  RecordBuffer,
  UINT64                RecordID,
  UINT64                *Status
  )
{
  EFI_PHYSICAL_ADDRESS                  PhyAddress;
  ERROR_RECORD_HEADER                   *OsRecordHdr;
  EFI_ERROR_RECORD_SERIALIZATION_INFO   *RecordHdr;

  if (mElog.NvAcc == NULL) {
    *Status = 3; //failed
    return;
  }

  if (RecordID == 0) {
    if (mParamBlock->CurRecordID == (UINTN)-1) {
      *Status = 5; //not found
      return;
    }
    RecordID = mParamBlock->CurRecordID; // read the first good record
  }

  PrstSearch(RecordID, &PhyAddress, Status, TRUE);
  if (*Status == 0) {
    OsRecordHdr = (ERROR_RECORD_HEADER *)PhyAddress;

    MmioReadBuffer8(PhyAddress,OsRecordHdr->RecordLength,(UINT8*)RecordBuffer);

    OsRecordHdr = (ERROR_RECORD_HEADER *)RecordBuffer;
    RecordHdr = (EFI_ERROR_RECORD_SERIALIZATION_INFO *)&OsRecordHdr->OSReserved;
    RecordHdr->Attributes = 0;    // Clear out internal attribute mask used in Psd.
  }

}

VOID
PrstWrite(
  EFI_PHYSICAL_ADDRESS  NewRecord,
  UINT64                *Status
  )
{
  EFI_STATUS                          EfiStatus;
  EFI_ERROR_RECORD_SERIALIZATION_INFO *RecordHdr;  
  ERROR_RECORD_HEADER                 *OsRecordHdr;
  UINT32                              NumBytes;
  EFI_PHYSICAL_ADDRESS                PhyAddress;
  UINT8                               Attributes;

  *Status = 3;  // Failed

  if (mElog.NvAcc == NULL) {
    return;
  }

// Clear record if it exist
  OsRecordHdr = (ERROR_RECORD_HEADER*)NewRecord;
  if (OsRecordHdr->RecordID == 0) {
    return;
  }
  if (OsRecordHdr->Signature != SIGNATURE_32('C','P','E','R')) {
    return;
  }
  PrstSearch(OsRecordHdr->RecordID, &PhyAddress, Status, FALSE);
                    //0x00  Success
                    //0x04  Record Store Empty
                    //0x05  Record Not Found
  if (*Status == 0) {
    // mark the record deleted
    OsRecordHdr = (ERROR_RECORD_HEADER *) PhyAddress;
    RecordHdr = (EFI_ERROR_RECORD_SERIALIZATION_INFO *)&OsRecordHdr->OSReserved;
    PhyAddress = (EFI_PHYSICAL_ADDRESS)(UINTN) &RecordHdr->Attributes;

    // clear BIT[1:0] to mark it deleted
    Attributes = RecordHdr->Attributes & (~M_ATTRIBUTE_MASK);

    EfiStatus = mElog.NvAcc->Write(mElog.NvAcc, (UINTN)PhyAddress, (UINT8*)&Attributes, 1, SPI_MEDIA_TYPE);
    if (EFI_ERROR(EfiStatus)) {
      *Status = 3;  // Failed
      return ;
    }
    mParamBlock->RecordCount--;
  }

  //
  // Append a new copy of the new record
  //
  OsRecordHdr = (ERROR_RECORD_HEADER*)NewRecord;
  // Check if we have enough space
  NumBytes = OsRecordHdr->RecordLength;
  if (NumBytes > mElog.FreeRecordBytes) {
    *Status = 1; // not enough space
    return ;
  }

  RecordHdr = (EFI_ERROR_RECORD_SERIALIZATION_INFO*)&OsRecordHdr->OSReserved;

// If the Serialization Header is missing in OS Record, better make one
  if (RecordHdr->Signature != SIGNATURE_16('E','R')) {
    RecordHdr->Signature = SIGNATURE_16('E','R');
  }
  RecordHdr->Attributes = (UINT8)(~M_ATTRIBUTE_MASK) | B_RECORD_INUSE; // BUT[1:0] = 11 means in use

  EfiStatus = mElog.NvAcc->Write(mElog.NvAcc, (UINTN)mElog.NextRecordAddress, (UINT8*)OsRecordHdr, NumBytes, SPI_MEDIA_TYPE);

  mElog.NextRecordAddress += NumBytes;
  mElog.FreeRecordBytes -= NumBytes;
  if (EFI_ERROR (EfiStatus) || (NumBytes != OsRecordHdr->RecordLength)) {
    *Status = 3;  // Failed
    return ;
  }

  if (mParamBlock->RecordCount == 0){
    mParamBlock->CurRecordID = OsRecordHdr->RecordID;
  }
  mParamBlock->RecordCount++;
  *Status = 0;  // Success, Record written
  return ;
}



VOID
PrstClear(
  UINT64                RecordID,
  UINT64                *Status
  )
{
  EFI_STATUS                          EfiStatus;
  EFI_ERROR_RECORD_SERIALIZATION_INFO *RecordHdr;
  EFI_PHYSICAL_ADDRESS                PhyAddress;
  ERROR_RECORD_HEADER                 *OsRecordHdr;
  UINT8                               Attributes;


  *Status = 3;  // Failed

  if (RecordID == 0) {
    return;
  }
  if (mElog.NvAcc == NULL) {
    return;
  }

  PrstSearch(RecordID, &PhyAddress, Status, FALSE);
                    //0x00  Success
                    //0x04  Record Store Empty
                    //0x05  Record Not Found
  if (*Status == 0) {
// mark the record deleted
    OsRecordHdr = (ERROR_RECORD_HEADER *) PhyAddress;
    RecordHdr   = (EFI_ERROR_RECORD_SERIALIZATION_INFO *)&OsRecordHdr->OSReserved;
    PhyAddress  = (EFI_PHYSICAL_ADDRESS)(UINTN) &RecordHdr->Attributes;
    Attributes  = RecordHdr->Attributes & (~M_ATTRIBUTE_MASK);  // clear BIT[1:0]

    EfiStatus = mElog.NvAcc->Write(mElog.NvAcc, (UINTN)PhyAddress, &Attributes, 1, SPI_MEDIA_TYPE);
    if (EFI_ERROR(EfiStatus)) {
      *Status = 3;  // Failed
      return ;
    }
    if (mParamBlock->RecordCount != 0) mParamBlock->RecordCount--;
  }

  if (mParamBlock->CurRecordID == RecordID) {
    mParamBlock->CurRecordID = (UINT64) (UINTN)-1;
  }
  return ;
}






EFI_STATUS
WheaElogInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_HANDLE                               SwHandle = NULL;
  EFI_SMM_SW_DISPATCH2_PROTOCOL            *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT              SwContext;
  EFI_STATUS                               Status;
  EFI_ACPI_WHEA_ERROR_SERIALIZATION_TABLE  *Erst; 
  EFI_ACPI_TABLE_PROTOCOL                  *AcpiTable;
  UINTN                                    TableKey;
  UINT64                                   OemTableId;
  

  DEBUG((EFI_D_INFO, __FUNCTION__"()\n"));

  Status = UpdateERST();
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "UpdateERST():%r\n",Status));
    return Status;
  }

  Status = PrstInit();
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "PrstInit():%r\n",Status));
    return EFI_NO_MEDIA;
  }

  PrstDefragment();
    
  Status = gSmst->SmmLocateProtocol(&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);
  ASSERT_EFI_ERROR (Status);

  SwContext.SwSmiInputValue = WHEA_ERROR_RECORD_SWSMI;
  Status = SwDispatch->Register (
                         SwDispatch,
                         WheaElogSwSmiCallback,
                         &SwContext,
                         &SwHandle
                         );
  ASSERT_EFI_ERROR(Status);  
//DEBUG((EFI_D_INFO, "SwVal:%X\n", SwContext.SwSmiInputValue));  

  Status = gBS->AllocatePool(
                  EfiBootServicesData, 
                  sizeof(EFI_ACPI_WHEA_ERROR_SERIALIZATION_TABLE),
                  &Erst
                  );
  ASSERT_EFI_ERROR(Status);

  ZeroMem(Erst, sizeof(EFI_ACPI_WHEA_ERROR_SERIALIZATION_TABLE));
  Erst->Header.Signature = EFI_ACPI_WHEA_ERST_TABLE_SIGNATURE;
  Erst->Header.Length    = sizeof(EFI_ACPI_WHEA_ERROR_SERIALIZATION_TABLE);
  Erst->Header.Revision  = EFI_WHEA_TABLE_REVISION;
  Erst->Header.Checksum  = 0;
  CopyMem(Erst->Header.OemId, PcdGetPtr(PcdAcpiDefaultOemId), sizeof(Erst->Header.OemId));
  OemTableId = PcdGet64(PcdAcpiDefaultOemTableId);
  CopyMem(&Erst->Header.OemTableId, &OemTableId, sizeof(UINT64));
  Erst->Header.OemRevision     = PcdGet32(PcdAcpiDefaultOemRevision);
  Erst->Header.CreatorId       = PcdGet32(PcdAcpiDefaultCreatorId);
  Erst->Header.CreatorRevision = PcdGet32(PcdAcpiDefaultCreatorRevision);

  Erst->SerializationHeaderSize = sizeof(EFI_ACPI_WHEA_ERROR_SERIALIZATION_HEADER);  
  Erst->InstructionEntryCount   = 16;
  CopyMem(Erst->InstructionEntry, mERST, sizeof(mERST));

  Status = gBS->LocateProtocol(&gEfiAcpiTableProtocolGuid, NULL, (VOID**)&AcpiTable);
  ASSERT_EFI_ERROR(Status); 

  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        Erst,
                        Erst->Header.Length,
                        &TableKey
                        );
  ASSERT_EFI_ERROR(Status);   

  Status = gBS->FreePool(Erst);
  ASSERT_EFI_ERROR(Status);   

  return EFI_SUCCESS;
}



