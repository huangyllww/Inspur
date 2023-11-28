
#include "SmiFlash.h"
#include <Protocol/PnpSmbios.h>


extern EFI_PNP_SMBIOS_PROTOCOL  *mPnpSmbiosProtocol;


EFI_STATUS
GetValueFromAsccii (
  IN OUT UINT8  *BinBuffer,
  IN OUT UINT32 *BinLength,
  IN     CHAR8  *HexStr
  )
{
  UINTN   Index;
  UINTN   Length;
  UINT8   Digit;
  CHAR8   TemStr[2];

  ZeroMem (TemStr, sizeof (TemStr));

  //
  // Find out how many hex characters the string has.
  //
  if ((HexStr[0] == '0') && ((HexStr[1] == 'x') || (HexStr[1] == 'X'))) {
    HexStr += 2;
  }

  Length = AsciiStrLen (HexStr);

  for (Index = 0; Index < Length; Index ++) {
    TemStr[0] = HexStr[Index];
    Digit = (UINT8) AsciiStrHexToUint64 (TemStr);
    if (Digit == 0 && TemStr[0] != '0') {
      //
      // Invalid Lun Char
      //
      break;
    }
    if ((Index & 1) == 0) {
      BinBuffer [Index/2] = Digit;
    } else {
      BinBuffer [Index/2] = (UINT8) ((BinBuffer [Index/2] << 4) + Digit);
    }
  }

  *BinLength = (UINT32) ((Index + 1)/2);

  return EFI_SUCCESS;
}





EFI_STATUS HandleSmbiosDataRequest(UPDATE_SMBIOS_PARAMETER *SmbiosPtr)
{
  UINT8                    Index;
  UINT32                   DataLength = 0;
  UINT32                   Data32[4];
  BOOLEAN                  DataIsValid = TRUE;
  PNP_52_DATA_BUFFER       *Parameter;
  EFI_STATUS               Status = EFI_SUCCESS;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Parameter = &SmbiosPtr->Parameter;
  DEBUG((EFI_D_INFO, "SubFun:%X, Data:%a, T:%X O:%X\n", SmbiosPtr->SubFun, \
    Parameter->StructureData, Parameter->StructureHeader.Type, Parameter->FieldOffset));
  
  if (SmbiosPtr->SubFun == UPDATE_LOCK_STATUS) {
    Status = EFI_ACCESS_DENIED;
    goto ProcExit;
  }
    
  Parameter->DataLength = (UINT16)AsciiStrLen (Parameter->StructureData) + 1;
  if (Parameter->DataLength == 1) {
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  if(Parameter->StructureHeader.Type > 3) {
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }


// (1,8) : UUID
  switch(Parameter->FieldOffset) {
    case 8:
      //
      // check string is valid
      // a~f:0x61~0x66, A~F:0x41~0x46, 0~9:0x30~0x39
      //
      if(Parameter->StructureHeader.Type == 1) {
        if (Parameter->StructureData[0] != ' ' && Parameter->StructureData[1] != ' '){
          for (Index = 0; Index < 0x20; Index++) {
            if (Parameter->StructureData[Index] > 'f') {
              DataIsValid = FALSE;
            } else if (Parameter->StructureData[Index] < 'a' && Parameter->StructureData[Index] > 'F') {
              DataIsValid = FALSE;
            } else if (Parameter->StructureData[Index] < 'A' && Parameter->StructureData[Index] > '9') {
              DataIsValid = FALSE;
            } else if (Parameter->StructureData[Index] < '0') {
              DataIsValid = FALSE;
            }
            if (DataIsValid == FALSE) {
              Status = EFI_INVALID_PARAMETER;
              goto ProcExit;
            }
          }

          if (DataIsValid) {
            GetValueFromAsccii((UINT8*)(UINTN)Data32, &DataLength, Parameter->StructureData);
            DEBUG((EFI_D_INFO, "uuid:%g L:%X\n", Data32, DataLength));
          } 
        }else {
          Status = EFI_INVALID_PARAMETER;
          goto ProcExit;
        }

        if (DataIsValid) {
          Parameter->Command     = DoubleWordChanged;
          Parameter->ChangeMask  = 0;
          Parameter->DataLength  = 0;
          Parameter->FieldOffset = 0x08; // UUID offset of Type1
          for (Index = 0; Index < (DataLength / 4); Index ++) {
            Parameter->ChangeValue = Data32[Index];
            Status = mPnpSmbiosProtocol->Pnp52SetSmbiosStructure(mPnpSmbiosProtocol, (VOID*)Parameter, TRUE);
            if(EFI_ERROR(Status)) {
              DEBUG((EFI_D_ERROR, "Can update Smbios Error: %r\n", Status));
              goto ProcExit;
            }
            Parameter->FieldOffset += 4;
          }
        }
      } else {
        Parameter->FieldOffset = 0x08;
        Parameter->Command = StringChanged;
      }
      break;

    case 4:
      if(Parameter->StructureHeader.Type == 3) {
        Status = EFI_INVALID_PARAMETER;
      } else {
        Parameter->FieldOffset = 0x04;
        Parameter->Command = StringChanged;
      }
      break;

    case 5:
      if(Parameter->StructureHeader.Type == 3) {
        Status = EFI_INVALID_PARAMETER;
      } else {
        Parameter->FieldOffset = 0x05;
        Parameter->Command = StringChanged;
      }
      break;

    case 6:
      if(Parameter->StructureHeader.Type == 0) {
        Status = EFI_INVALID_PARAMETER;
      } else {
        Parameter->FieldOffset = 0x06;
        Parameter->Command = StringChanged;
      }
      break;

    case 7:
      if(Parameter->StructureHeader.Type == 0) {
        Status = EFI_INVALID_PARAMETER;
      } else {
        Parameter->FieldOffset = 0x07;
        Parameter->Command = StringChanged;
      }
      break;

	case 25:
      if(Parameter->StructureHeader.Type != 1) {
        Status = EFI_INVALID_PARAMETER;
      } else {
        Parameter->FieldOffset = 0x19;
        Parameter->Command = StringChanged;
      }
      break;
	  
	case 26:
      if(Parameter->StructureHeader.Type != 1) {
        Status = EFI_INVALID_PARAMETER;
      } else {
        Parameter->FieldOffset = 0x1A;
        Parameter->Command = StringChanged;
      }
      break;

    default:
      Status = EFI_INVALID_PARAMETER;
      break;
  }

  DEBUG((EFI_D_ERROR, "(L%d) O:%X, C:%X\n", __LINE__, Parameter->FieldOffset, Parameter->Command));
  if(EFI_ERROR(Status)) {
    goto ProcExit;
  }

  if (Parameter->Command == StringChanged) {
    if (Parameter->DataLength > MAX_STRING_LENGTH) {
      Parameter->StructureData[MAX_STRING_LENGTH] = '\0';
      Parameter->DataLength = MAX_STRING_LENGTH + 1;
    }

    Status = mPnpSmbiosProtocol->Pnp52SetSmbiosStructure(mPnpSmbiosProtocol, (VOID*)Parameter, TRUE);
    if(EFI_ERROR(Status)) {
      goto ProcExit;
    }
  }

ProcExit:
  return Status;
}



