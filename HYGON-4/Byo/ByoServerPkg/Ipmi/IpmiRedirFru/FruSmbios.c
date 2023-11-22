/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  FruSmbios.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/


#ifndef FRUMAXSTRING
#define FRUMAXSTRING 128
#endif

#include "IpmiRedirFru.h"
#include "Protocol/PlatHostInfoProtocol.h"
#include "Library/DevicePathLib.h" 
#include "Library/BaseMemoryLib.h"
#include <Library/ByoCommLib.h>
#include "Protocol/DiskInfo.h"
#include "Include/IndustryStandard/Atapi.h"


EFI_SM_FRU_REDIR_PROTOCOL   *mFruRedirProtocol;
EFI_SMBIOS_PROTOCOL         *mSmbiosProtocol;
extern UINT16               mDeviceID;


typedef struct {
  UINT8 BaseBoardManufacturer[FRUMAXSTRING];
  UINT8 BaseBoardProductName[FRUMAXSTRING];
  UINT8 BaseBoardSerialNumber[FRUMAXSTRING];
  UINT8 BaseBoardVersion[FRUMAXSTRING];
} BOARD_INFO;

typedef struct {
  UINT8 SystemManufacturer[FRUMAXSTRING];
  UINT8 SystemProductName[FRUMAXSTRING];
  UINT8 SystemPartNum[FRUMAXSTRING];
  UINT8 SystemVersion[FRUMAXSTRING];
  UINT8 SystemSerialNumber[FRUMAXSTRING];
  UINT8 AssetTag[FRUMAXSTRING];
} PRODUCT_INFO;

typedef struct {
  UINT8 ChassisVersion[FRUMAXSTRING];
  UINT8 ChassisSerialNumber[FRUMAXSTRING];
  UINT8 ChassisManufacturer[FRUMAXSTRING];
} CHASSIS_INFO;


UINT8
FruStrLen(
  IN CHAR8 *Str
  )
{
  UINT8  Length = 0;
  CHAR8  *Ptr;

  if (Str != NULL){
    Ptr = Str;
    while (*Ptr != '\0'){
      Length++;
      Ptr++;
    }
  }
  return Length;
}

void
GetStringFromFru (
  IN OUT  UINTN           *Offset,
  IN      UINT8           *TempPtr,
  IN OUT  UINT8           *StrPtr
  )
/*++

Routine Description:
  This routine attempts to get a string out of the FRU at the designated offset in the 
  buffer pointed to by TempPtr.  String type is ASCII

Arguments:
  Offset    - Offset of string in buffer pointed to by TempPtr, this is updated to the next
              offset.
  TempPtr   - Pointer to a buffer containing the FRU
  StrPtr    - the pointer to a buffer for retrieve the string get from FRU.

Returns:
  STRING_REF with reference to string retrieved from the FRU or NULL if none is found.

--*/
{
  UINTN       Type;
  UINTN       Length;
  UINT8       *SrcStrPtr;
  
  UINTN   i;
  UINTN   j;
  UINT32  Str;

  Type       = (0xC0 & TempPtr[*Offset]) >> 0X06;
  Length     = 0x3F & TempPtr[*Offset];
  DEBUG((EFI_D_INFO,"GetStringFromFru,Type = %x,Length = %d.\n",Type,Length));
  SrcStrPtr  = &TempPtr[*Offset + 1];

  ASSERT(Length < FRUMAXSTRING);

  if (Length > 0) {
  	if(Type == 1){
      for (i = 0; i < Length; i++) {
        if(SrcStrPtr[i] < 0xa){
          StrPtr[i] = SrcStrPtr[i] + '0';
		}else if (SrcStrPtr[i] == 0xa){
          StrPtr[i] = ' ';
		}else if (SrcStrPtr[i] == 0xb){
          StrPtr[i] = '-';
		}else if (SrcStrPtr[i] == 0xc){
          StrPtr[i] = '.';
		}else {
          StrPtr[i] = '\0';
		}
	  }
      StrPtr[Length] = '\0';
	}else if(Type == 2){
      //
  	  //  Unpack 3 6bit value (24 bits) to 4 8bit value at a time (3 bytes to 4 bytes)
  	  //
      for (i = 0; i < Length / 3; i++) {
        Str = *((UINT32 *) (SrcStrPtr + i * 3));
        for (j = 0; j < 4; j++) {
            StrPtr[i * 4 + j] = (CHAR8) (Str & 0x3F) + 0x20;
            Str >>= 6;
        }
      }
      //
      // Convert rest of AsciiStr entries
      //
      Str = *((UINT32 *) (SrcStrPtr + i * 3));
      for (j = 0; j < (Length - ((Length / 3) * 3)); j++) {
        StrPtr[i * 4 + j] = (CHAR8) (Str & 0x3F) + 0x20;
        Str >>= 6;
      }
      StrPtr[i * 4 + j] = '\0';
	} else {
      CopyMem(StrPtr, SrcStrPtr, Length);
      DEBUG((EFI_D_INFO,"Type = 3 ,StrPtr[0] = %x\n",StrPtr[0]));
      StrPtr[Length] = '\0';
	}
  } else {
    StrPtr[0] = '\0';
  }
  *Offset = *Offset + Length + 1;
  return;
}

UINT8 *
GetFruInfoArea (
  IN  EFI_SM_FRU_REDIR_PROTOCOL *This,
  IN  UINTN                     Offset
  )
/*++

Routine Description:
  This routine gets the FRU info area specified by the offset and returns it in
  an allocated buffer.  It is the caller's responsibility to free the buffer.

Arguments:
  This      - SM Fru Redir protocol
  Offset    - Info Area starting offset in multiples of 8 bytes.

Returns:
  Buffer with FruInfo data or NULL if not found.

--*/
{
  EFI_STATUS  Status;
  UINT8       *TempPtr;
  UINTN       Length;

  TempPtr = NULL;

  Offset  = Offset * 8;
  if (Offset > 0) {
    //
    // Get Info area length, which is in multiples of 8 bytes
    //
    Length = 0;
    Status = EfiGetFruRedirData (This, 0, (Offset + 1), 1, (UINT8 *) &Length);
    if (Status == EFI_NOT_FOUND) {
      return NULL;
    }
    Length = Length * 8;

    if (Length > 0) {
      TempPtr = AllocateRuntimePool (Length);
      ASSERT (TempPtr != NULL);
      Status = EfiGetFruRedirData (This, 0, Offset, Length, TempPtr);
      if (Status == EFI_NOT_FOUND) {
        return NULL;
      }
    }
  }
  return TempPtr;
}

//
//
//
UINT8*
GetStructureByTypeNo(
  IN  UINT16    TypeNo
)
/*++

Routine Description:
  Type1,2,3 only has one instance in SMBIOS tables for each.

Arguments:
  TypeNo    - The number of SMBIOS TYPE

Returns:
  the pointer of SMBIOS TYPE structure.

--*/
{
  EFI_STATUS                   Status;
  EFI_SMBIOS_HANDLE            SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER      *Record;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = mSmbiosProtocol->GetNext (
                              mSmbiosProtocol, 
                              &SmbiosHandle, 
                              (EFI_SMBIOS_TYPE *)&TypeNo, 
                              &Record, 
                              NULL
                              );
  if (EFI_ERROR(Status)) {
    return NULL;
  }

  return (UINT8*) Record;
}

UINT8
SmbiosCheckSum(
  IN UINT8    *ChecksumSrc,
  IN UINT8    length
  )
{
  UINT8     Checksum = 0;
  UINT8     i;

  for (i = 0; i < length; i++) {
    Checksum = Checksum + *ChecksumSrc++;
  }
  return (0 - Checksum);
}

VOID 
DynamicUpdateType(
  IN  UINT16      TypeNo,
  IN  UINTN       StringNo,
  IN  UINT8       *Data
  )
{
  EFI_STATUS                   Status;
  EFI_SMBIOS_HANDLE            SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER      *Record;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = mSmbiosProtocol->GetNext (
                              mSmbiosProtocol, 
                              &SmbiosHandle, 
                              (EFI_SMBIOS_TYPE *)&TypeNo, 
                              &Record, 
                              NULL
                              );
  if (EFI_ERROR(Status)) {
    return;
  }

 mSmbiosProtocol->UpdateString (
                    mSmbiosProtocol,
                    &SmbiosHandle,
                    &StringNo,
                    (CHAR8*)Data
                    );
  
}

void
UpdateType1UUID ()
{
  EFI_STATUS                        Status;
  SMBIOS_TABLE_TYPE1                *TablePtr;
  EFI_IPMI_MSG_GET_DEVICE_GUID_RSP  ResponseData;  
  UINT8                             ResponseDataSize;

  ResponseDataSize = sizeof (ResponseData);
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_APP,
             EFI_APP_GET_DEVICE_GUID,
             (UINT8 *) NULL,
             0,
             (UINT8 *) &ResponseData,
             (UINT8 *) &ResponseDataSize
             );
  if (EFI_ERROR (Status)) {
    return;
  }

  TablePtr = (SMBIOS_TABLE_TYPE1*)GetStructureByTypeNo (SMBIOSTYPE1);
  CopyMem(&TablePtr->Uuid, &ResponseData, ResponseDataSize);
}

EFI_STATUS
GetAndCheckFruString (
  IN  UINT8  *TempPtr, 
  IN  UINTN  StartOffset, 
  IN  UINT8  *Data, 
  IN  UINT8  TotalStringNum)
{
  UINT8                           Index;
  UINTN                           IndexJ;
  UINT8                           TempStr[FRUMAXSTRING];
  BOOLEAN                         NeedRetry = FALSE;
  
  for (Index = 0; Index < TotalStringNum; Index++) {
    ZeroMem(TempStr, FRUMAXSTRING);
    GetStringFromFru (&StartOffset, TempPtr, TempStr);
    IndexJ = 0;
    do {
      if (TempStr[IndexJ] == '\0') {
        break;
      }
      if (TempStr[IndexJ] < ' ' || TempStr[IndexJ] > '~')  {
        NeedRetry = TRUE;
      }
      IndexJ++;
    } while (TRUE);
    CopyMem(Data + Index * FRUMAXSTRING , TempStr, IndexJ + 1);
  }
  
  if (NeedRetry) {
    DEBUG((EFI_D_ERROR, "FRU string contains illegal characters.\n"));
    return EFI_UNSUPPORTED;
  }
  
  return EFI_SUCCESS;
}

VOID 
EFIAPI 
GenerateFruSmbiosType123DataNotified ()
{
  EFI_STATUS                      Status;
  UINT8                           *FruHdrPtr;
  UINT8                           FruHdrChksum;
  IPMI_FRU_COMMON_HEADER          FruCommonHeader; 
  UINT8                           i;
  UINT8                           *TempPtr;
  UINT8                           *TablePtr;
  UINT8                           RetryTime;
  UINT8                           NumofString;
  PRODUCT_INFO                    *ProductInfo = NULL;
  BOARD_INFO                      *BoardInfo = NULL;
  CHASSIS_INFO                    *ChassisInfo = NULL;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  

  SetMem((UINT8*)(&FruCommonHeader), sizeof(IPMI_FRU_COMMON_HEADER), 0);

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **)&mSmbiosProtocol);
  ASSERT_EFI_ERROR(Status);

  Status = EfiGetFruRedirData (mFruRedirProtocol, 0, 0, sizeof (IPMI_FRU_COMMON_HEADER), (UINT8 *) &FruCommonHeader);
  if (Status == EFI_NOT_FOUND) {
    DEBUG ((EFI_D_ERROR, "FRU header not found.\n"));
    //
    //  The FRU information was not found, nothing need to do.
    //
    return;
  } else {
    //
    // Do a validity check on the FRU header, since it may be all 0xFF(s) if
    // there is no FRU programmed on the system.
    //
    FruHdrPtr = (UINT8 *) &FruCommonHeader;
    for (i = 0, FruHdrChksum = 0; i < sizeof (FruCommonHeader); i++) {
      FruHdrChksum = (UINT8) (FruHdrChksum +*FruHdrPtr++);
    }
    if (FruHdrChksum != 0) {
      DEBUG ((EFI_D_ERROR, "FRU header invalid.\n"));
      //
      //  The FRU information is bad so nothing need to do.
      //
      return;
    }
  }
  //
  // SMBIOS Type 1, Product data
  //
  RetryTime = 3;
  NumofString = 6;
  Status = EFI_SUCCESS;
  do {
    TempPtr = GetFruInfoArea (mFruRedirProtocol, FruCommonHeader.ProductInfoStartingOffset);
    if (TempPtr != NULL) {
      //
      // Get the following fields in the specified order.  DO NOT change this order unless the FRU file definition
      // changes.  Offset is initialized and then is incremented to the next field offset in GetStringFromFru.
      //
      // Product Manufacturer
      // Product Name
      // Product Model Number / System Part Number
      // Product Version
      // Product Serial Number
      // Product Asset Tag
      //
      ProductInfo = NULL;
      ProductInfo = AllocateZeroPool(NumofString*FRUMAXSTRING);
      if (ProductInfo != NULL) {
        Status = GetAndCheckFruString(TempPtr, PRODUCT_MFG_OFFSET, (UINT8*)ProductInfo, NumofString);
        if (EFI_ERROR(Status)) {
          DumpMem8(ProductInfo, NumofString*FRUMAXSTRING);
          FreePool (TempPtr);
          FreePool (ProductInfo);
        }
      }
    }
    RetryTime--;  
  } while ((Status != EFI_SUCCESS || TempPtr == NULL || ProductInfo == NULL) && RetryTime > 0);

  if (TempPtr != NULL && ProductInfo != NULL) {
    //MiscSystemManufacturer.SystemManufacturer
    if (FruStrLen((CHAR8*)ProductInfo->SystemManufacturer) != 0) {
      DynamicUpdateType(SMBIOSTYPE1, STRING1, ProductInfo->SystemManufacturer);
      DynamicUpdateType(SMBIOSTYPE3, STRING1, ProductInfo->SystemManufacturer);
    }
    
    //MiscSystemManufacturer.SystemProductName
    if (FruStrLen((CHAR8*)ProductInfo->SystemProductName) != 0) {
      DynamicUpdateType(SMBIOSTYPE1, STRING2, ProductInfo->SystemProductName);
    }
    
    //
    // dmidecode -s system-version == ipmitool fru | grep 'Product Part Number'
    //
    //MiscSystemManufacturer.SystemPartNum
    if (FruStrLen((CHAR8*)ProductInfo->SystemPartNum) != 0) {
      DynamicUpdateType(SMBIOSTYPE1, STRING5, ProductInfo->SystemPartNum);
    }
    
    //MiscSystemManufacturer.SystemVersion
    if (FruStrLen((CHAR8*)ProductInfo->SystemVersion) != 0) {
      DynamicUpdateType(SMBIOSTYPE1, STRING3, ProductInfo->SystemVersion);
    }
    
    //MiscSystemManufacturer.SystemSerialNumber
    if (FruStrLen((CHAR8*)ProductInfo->SystemSerialNumber) != 0) {
      DynamicUpdateType(SMBIOSTYPE1, STRING4, ProductInfo->SystemSerialNumber);
    }
    
    //MiscSystemManufacturer.AssetTag
    if (FruStrLen((CHAR8*)ProductInfo->AssetTag) != 0) {
      DynamicUpdateType(SMBIOSTYPE2, STRING5, ProductInfo->AssetTag); // type2.AssetTag
      DynamicUpdateType(SMBIOSTYPE3, STRING4, ProductInfo->AssetTag); //NOTICE: this Asset Tag can be used by type 3 table
    }
    
    FreePool (TempPtr);
    FreePool (ProductInfo);
  }

  //
  // SMBIOS Type 2, Base Board data
  //
  RetryTime = 3;
  NumofString = 4;
  Status = EFI_SUCCESS;
  do {
    TempPtr = GetFruInfoArea (mFruRedirProtocol, FruCommonHeader.BoardAreaStartingOffset);
    if (TempPtr != NULL) {
      //
      // Get the following fields in the specified order.  DO NOT change this order unless the FRU file definition
      // changes.  Offset is initialized and then is incremented to the next field offset in GetStringFromFru.
      //
      // Board Manufacturer
      // Board Product Name
      // Board Serial Number
      // Board Part Number
      // FRU Version Number
      //
      BoardInfo = NULL;
      BoardInfo = AllocateZeroPool(NumofString*FRUMAXSTRING);
      if (BoardInfo != NULL) {
        Status = GetAndCheckFruString(TempPtr, BOARD_MFG_OFFSET, (UINT8*)BoardInfo, NumofString);
        if (EFI_ERROR(Status)) {
          DumpMem8(BoardInfo, NumofString*FRUMAXSTRING);
          FreePool (TempPtr);
          FreePool (BoardInfo);
        }
      }
    }
    RetryTime--;
  } while ((Status != EFI_SUCCESS || TempPtr == NULL || BoardInfo == NULL) && RetryTime > 0);
  
  if (TempPtr != NULL && BoardInfo != NULL) {
    //MiscBaseBoardManufacturer.BaseBoardManufacturer
    if (FruStrLen((CHAR8*)BoardInfo->BaseBoardManufacturer) != 0) {
      DynamicUpdateType(SMBIOSTYPE2, STRING1, BoardInfo->BaseBoardManufacturer);
    }
    
    //MiscBaseBoardManufacturer.BaseBoardProductName
    if (FruStrLen((CHAR8*)BoardInfo->BaseBoardProductName) != 0) {
      DynamicUpdateType(SMBIOSTYPE2, STRING2, BoardInfo->BaseBoardProductName);
    }
  
    //MiscBaseBoardManufacturer.BaseBoardSerialNumber
    if (FruStrLen((CHAR8*)BoardInfo->BaseBoardSerialNumber) != 0) {
      DynamicUpdateType(SMBIOSTYPE2, STRING4, BoardInfo->BaseBoardSerialNumber);
    }
    
    //MiscBaseBoardManufacturer.BaseBoardVersion
    if (FruStrLen((CHAR8*)BoardInfo->BaseBoardVersion) != 0) {
      DynamicUpdateType(SMBIOSTYPE2, STRING3, BoardInfo->BaseBoardVersion);
    }
    
    // Not currently needed in SMBIOS, uncomment and copy to SMBIOS if needed.
    //GetStringFromFru (&Offset, TempPtr, TempStr);  //**************************FRU Version Number
    
    FreePool (TempPtr);
    FreePool (BoardInfo);
  }

  //
  // SMBIOS Type 3, Chassis data
  //
  RetryTime = 3;
  NumofString = 3;
  Status = EFI_SUCCESS;
  do {
    TempPtr = GetFruInfoArea (mFruRedirProtocol, FruCommonHeader.ChassisInfoStartingOffset);
    if (TempPtr != NULL) {
      //
      // Get the following fields in the specified order.  DO NOT change this order unless the FRU file definition
      // changes.  Offset is initialized and then is incremented to the next field offset in GetStringFromFru.
      //    
      // ChassisVersion
      // ChassisSerialNumber
      // ChassisSerialNumber
      // ChassisManufacturer
      //
      ChassisInfo = NULL;
      ChassisInfo = AllocateZeroPool(NumofString*FRUMAXSTRING);
      if (ChassisInfo != NULL) {
        Status = GetAndCheckFruString(TempPtr, CHASSIS_PART_NUMBER, (UINT8*)ChassisInfo, NumofString);
        if (EFI_ERROR(Status)) {
          DumpMem8(ChassisInfo, NumofString*FRUMAXSTRING);
          FreePool (TempPtr);
          FreePool (ChassisInfo);
        }
      }
    }
    RetryTime--;
  } while ((Status != EFI_SUCCESS || TempPtr == NULL || ChassisInfo == NULL) && RetryTime > 0);

  if (TempPtr != NULL && ChassisInfo != NULL) {
    //special process:
    TablePtr = GetStructureByTypeNo(SMBIOSTYPE3);
    ASSERT (TablePtr != NULL);
    ((SMBIOS_TABLE_TYPE3*)TablePtr)->Type = TempPtr[CHASSIS_TYPE_OFFSET];
  
    //MiscChassisManufacturer.ChassisVersion
    if (FruStrLen((CHAR8*)ChassisInfo->ChassisVersion) != 0) {
      DynamicUpdateType(SMBIOSTYPE3, STRING2, ChassisInfo->ChassisVersion);
    }
    
    //MiscChassisManufacturer.ChassisSerialNumber
    if (FruStrLen((CHAR8*)ChassisInfo->ChassisSerialNumber) != 0) {
      DynamicUpdateType(SMBIOSTYPE3, STRING3, ChassisInfo->ChassisSerialNumber);
    }

    //GetStringFromFru (&Offset, TempPtr, TempStr);  //MiscChassisManufacturer.ChassisManufacturer  
    //if (FruStrLen(TempStr) != 0) {
    //  DynamicUpdateType(SMBIOSTYPE3, STRING1, TempStr);
    //}

    FreePool (TempPtr);
    FreePool (ChassisInfo);
  }
  UpdateType1UUID ();
  return;
}

VOID
GenerateFruSmbiosData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This
  )
/*++

Routine Description:
  This routine install a notify function listen to EfiSmbiosProtocol

Arguments:
  This                        - SM Fru Redir protocol

Returns:
  VOID

--*/
{
  EFI_HANDLE        ImageHandle = NULL;
  
  mFruRedirProtocol = This;

  gBS->InstallMultipleProtocolInterfaces (
         &ImageHandle,
         &gBdsAfterConnectAllDefConsoleProtocolGuid, GenerateFruSmbiosType123DataNotified,
         NULL
         );
}
