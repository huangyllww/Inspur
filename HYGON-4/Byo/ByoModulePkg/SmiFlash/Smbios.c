/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include "SmiFlash.h"
#include <Protocol/PnpSmbios.h>
#include <IndustryStandard/SmBios.h>


extern EFI_PNP_SMBIOS_PROTOCOL  *mPnpSmbiosProtocol;


EFI_STATUS HandleSmbiosDataRequest(UPDATE_SMBIOS_PARAMETER *SmbiosPtr)
{
  UINT8                    Index;
  UINT32                   Data32[4];
  PNP_52_DATA_BUFFER       *Parameter;
  EFI_STATUS               Status = EFI_INVALID_PARAMETER;
  UINT8                    SmbiosType;
  UINT8                    *p;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  if (mPnpSmbiosProtocol == NULL) {
    Status = EFI_UNSUPPORTED;
    goto ProcExit;
  }

  Parameter = &SmbiosPtr->Parameter;
  SmbiosType = Parameter->StructureHeader.Type;
  DEBUG((EFI_D_INFO, "SubFun:%X, Data:[%a], T:%X O:%X L:%d\n", SmbiosPtr->SubFun, \
    Parameter->StructureData, SmbiosType, Parameter->FieldOffset, Parameter->DataLength));

  if(Parameter->DataLength == 0){
    p = (UINT8*)Parameter->StructureData;
    while(1){
      if(p[Parameter->DataLength] == 0 || Parameter->DataLength >= SMBIOS_STRING_MAX_LENGTH){
        break;
      }
      Parameter->DataLength++;
    }
    if(p[Parameter->DataLength] != 0){
      DEBUG((EFI_D_ERROR, "string length too long\n"));
      goto ProcExit;
    }
    Parameter->DataLength++;
    
  } else {
    if(Parameter->DataLength > SMBIOS_STRING_MAX_LENGTH+1){
      DEBUG((EFI_D_ERROR, "string length too long\n"));
      goto ProcExit;
    }
  }

  if (Parameter->DataLength == 1) {
    DEBUG((EFI_D_ERROR, "empty string\n"));
    goto ProcExit;
  }

  if(SmbiosType > 3 && SmbiosType != 11) {
    Status = EFI_INVALID_PARAMETER;
    DEBUG((EFI_D_ERROR, "type is not 0, 1, 2, 3, 11\n"));
    goto ProcExit;
  }

  if(SmbiosType == 1 && Parameter->FieldOffset == 8){
    if(Parameter->DataLength == 16){
      CopyMem(Data32, Parameter->StructureData, 16);
    } else {
      if(Parameter->DataLength != 32 + 1){
        DEBUG((EFI_D_ERROR, "bad uuid string length\n"));
        goto ProcExit;
      }

      p = (UINT8*)Data32;
      for (Index = 0; Index < 16; Index++) {
        if(!LibChar2Hex((CHAR8 *)&Parameter->StructureData[Index*2], &p[Index])){
          Status = EFI_INVALID_PARAMETER;
          DEBUG((EFI_D_ERROR, "not hex\n"));
          goto ProcExit;
        }
      }
    }

    Parameter->Command     = DoubleWordChanged;
    Parameter->ChangeMask  = 0;
    Parameter->DataLength  = 0;
    Parameter->FieldOffset = 0x08;
    for (Index = 0; Index < 4; Index++) {
      Parameter->ChangeValue = Data32[Index];
      Status = mPnpSmbiosProtocol->Pnp52SetSmbiosStructure(mPnpSmbiosProtocol, (VOID*)Parameter, TRUE);
      if(EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR, "Can update Smbios Error: %r\n", Status));
        goto ProcExit;
      }
      Parameter->FieldOffset += 4;
    }
    Status = EFI_SUCCESS;
    
  }else {  // let pnp smbios to check parameter
    Parameter->Command = StringChanged;
    Status = mPnpSmbiosProtocol->Pnp52SetSmbiosStructure(mPnpSmbiosProtocol, (VOID*)Parameter, TRUE);
  }

ProcExit:
  return Status;
}



