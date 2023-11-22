/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SystemPasswordLib.c

Abstract:
  System password library.

Revision History:

**/
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseCryptLib.h>
#include <Protocol/ByoFormSetManager.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SystemPasswordLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

SYSTEM_PASSWORD         mLibPasswordVariable;
UINT8                   mLibSaltBuffer [UNIQUE_SYSTEM_NUMBER_LENGTH + 1];
UINT8                                                 gRecoederTime = 0xFF;
#define RECORDER_TIME                                 gRecoederTime




EFI_STATUS
EFIAPI
PasswordGetVariable (
  IN     CHAR16                      *VariableName,
  IN     EFI_GUID                    *VendorGuid,
  OUT    UINT32                      *Attributes,    OPTIONAL
  IN OUT UINTN                       *DataSize,
  OUT    VOID                        *Data           OPTIONAL
  );
EFI_STATUS
EFIAPI
PasswordSetVariable (
  IN     CHAR16                      *VariableName,
  IN     EFI_GUID                    *VendorGuid,
  IN     UINT32                      Attributes,
  IN     UINTN                       DataSize,
  OUT    VOID                        *Data
  );

EFI_STATUS
EFIAPI
PasswordCreatEvent (
  VOID
  );

typedef struct {
  EFI_KEY EFIKey;
  CHAR16  Unicode;
  CHAR16  ShiftedUnicode;
  CHAR16  ScanCode;
} KEY_DESCRIPTOR;

STATIC CONST KEY_DESCRIPTOR  KeyDescriptor[] = {
  {EfiKeyC1,    'a',      'A',       0x1e },
  {EfiKeyB5,    'b',      'B',       0x30 },
  {EfiKeyB3,    'c',      'C',       0x2e },
  {EfiKeyC3,    'd',      'D',       0x20 },
  {EfiKeyD3,    'e',      'E',       0x12 },
  {EfiKeyC4,    'f',      'F',       0x21 },
  {EfiKeyC5,    'g',      'G',       0x22 },
  {EfiKeyC6,    'h',      'H',       0x23 },
  {EfiKeyD8,    'i',      'I',       0x17 },
  {EfiKeyC7,    'j',      'J',       0x24 },
  {EfiKeyC8,    'k',      'K',       0x25 },
  {EfiKeyC9,    'l',      'L',       0x26 },
  {EfiKeyB7,    'm',      'M',       0x32 },
  {EfiKeyB6,    'n',      'N',       0x31 },
  {EfiKeyD9,    'o',      'O',       0x18 },
  {EfiKeyD10,   'p',      'P',       0x19 },
  {EfiKeyD1,    'q',      'Q',       0x10 },
  {EfiKeyD4,    'r',      'R',       0x13 },
  {EfiKeyC2,    's',      'S',       0x1f },
  {EfiKeyD5,    't',      'T',       0x14 },
  {EfiKeyD7,    'u',      'U',       0x16 },
  {EfiKeyB4,    'v',      'V',       0x2f },
  {EfiKeyD2,    'w',      'W',       0x11 },
  {EfiKeyB2,    'x',      'X',       0x2d },
  {EfiKeyD6,    'y',      'Y',       0x15 },
  {EfiKeyB1,    'z',      'Z',       0x2c },
  {EfiKeyE1,    '1',      '!',       0x02 },
  {EfiKeyE2,    '2',      '@',       0x03 },
  {EfiKeyE3,    '3',      '#',       0x04 },
  {EfiKeyE4,    '4',      '$',       0x05 },
  {EfiKeyE5,    '5',      '%',       0x06 },
  {EfiKeyE6,    '6',      '^',       0x07 },
  {EfiKeyE7,    '7',      '&',       0x08 },
  {EfiKeyE8,    '8',      '*',       0x09 },
  {EfiKeyE9,    '9',      '(',       0x0a },
  {EfiKeyE10,   '0',      ')',       0x0b },
  {EfiKeyLShift,     0,     0,       0x2a },
  {EfiKeyCapsLock,  0,     0,       0x3a }
};


STATIC CONST CHAR16 gValidMarkChar16[] = {
  L'!', L'\"', L'#', L'$', L'%', L'&', L'\'', L'(', 
  L')', L'*',  L'+', L',', L'-', L'.', L'/',  L':', 
  L';', L'<',  L'=', L'>', L'?', L'@', L'[',  L'\\',
  L']', L'^',  L'_', L'{', L'|', L'}', L'~',  L' ',
  L'`'
};

BOOLEAN 
IsValidPdKey (
  EFI_INPUT_KEY *Key
  )
{
  CHAR16  UniChar;
  UINTN   Index;
  
  if(Key->ScanCode != SCAN_NULL){
    return FALSE;
  }
  
  UniChar = Key->UnicodeChar;
  if((UniChar >= L'0' && UniChar <= L'9') || 
     (UniChar >= L'A' && UniChar <= L'Z') ||
     (UniChar >= L'a' && UniChar <= L'z')){
    return TRUE;
  }    

  for(Index=0;Index<sizeof(gValidMarkChar16)/sizeof(gValidMarkChar16[0]);Index++){
    if(gValidMarkChar16[Index] == UniChar){
      return TRUE;
    }
  }
  
  return FALSE;
}


EFI_STATUS
EFIAPI
Sha256Hash (
  IN  CONST VOID        *Data,
  IN        UINTN       DataLen,
  OUT       UINT8       *Digest,
  IN        UINTN       DigestSize
  )
{
  ASSERT(Data != NULL && DataLen != 0 && Digest != NULL && DigestSize >= 32);

  Sha256HashAll (Data, DataLen, Digest);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Sm3Hash (
  IN  CONST VOID        *Data,
  IN        UINTN       DataLen,
  OUT       UINT8       *Digest,
  IN        UINTN       DigestSize
  )
{
  ASSERT(Data != NULL && DataLen != 0 && Digest != NULL && DigestSize >= 32);

  Sm3HashAll (Data, DataLen, Digest);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Sha512Hash (
  IN  CONST VOID        *Data,
  IN        UINTN       DataLen,
  OUT       UINT8       *Digest,
  IN        UINTN       DigestSize
  )
{
  ASSERT(Data != NULL && DataLen != 0 && Digest != NULL && DigestSize >= 32);

  Sha512HashAll (Data, DataLen, Digest);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Sha384Hash (
  IN  CONST VOID        *Data,
  IN        UINTN       DataLen,
  OUT       UINT8       *Digest,
  IN        UINTN       DigestSize
  )
{
  ASSERT(Data != NULL && DataLen != 0 && Digest != NULL && DigestSize >= 32);

  Sha384HashAll (Data, DataLen, Digest);

  return EFI_SUCCESS;
}

UINTN
Unicode2Ascii (
  IN  CHAR16    *String16,
  OUT  UINT8    **String8
  )
{
  UINTN    Length;
  UINTN    Index;
  UINT8    *Buffer;

  Length = StrLen(String16);

  if (0 < Length) {
    Buffer = NULL;
    Buffer = AllocateZeroPool(Length + 1);
    ASSERT (Buffer != NULL);

    for (Index = 0; Index < Length; Index++) {
      Buffer[Index] = (UINT8) (String16[Index]);
    }
  } else {
    Buffer = NULL;
  }

  *String8 = Buffer;
  return Length;
}

UINT8
GetUniqueNumber (
  VOID
  )
{
  EFI_STATUS  Status;
  UINTN       VarSize;
  UINTN       Index;

  VarSize = sizeof (mLibPasswordVariable);
  Status = PasswordGetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VarSize,
                  &mLibPasswordVariable
                  );
  if (EFI_ERROR(Status)) {
    CopyMem(mLibSaltBuffer, "87654321", UNIQUE_SYSTEM_NUMBER_LENGTH);
  } else {
    for (Index = 0; Index < UNIQUE_SYSTEM_NUMBER_LENGTH; Index++) {
      mLibSaltBuffer[Index] = (UINT8)(mLibPasswordVariable.Admin[Index] + mLibPasswordVariable.PowerOn[Index]);
    }
  }

  //
  // HashType is required to be retrieved on each time.
  //
  return mLibPasswordVariable.HashType;
}


UINTN
EncodePassword (
  IN  UINT8    *Password,
  IN  UINTN    Length,
  OUT  UINT8    **Hash,
  IN  UINTN    Type    //Type :Ascii or Scancode
  )
{
  UINTN    Len;
  UINTN    i,j;
  UINTN    KeyIndex;
  UINTN    Count;
  UINT8   *Buf = NULL;
  UINT8    PasswordArray[64];
  UINT8    HashArray[32];
  BOOLEAN  ShiftPress;
  BOOLEAN  CaplockPress;
  UINT8    HashType;
  BOOLEAN  HashIterateOk;
  BOOLEAN  SystemPasswordHashIteration;
  UINTN    DigestSize;
  EFI_STATUS                Status;
  UINTN                     VariableSize;
  SYSTEM_PASSWORD_REQUIREMENTS  SystemPasswordRequirement;

  VariableSize = sizeof(SYSTEM_PASSWORD_REQUIREMENTS);
  Status = PasswordGetVariable (
                  PASSWORD_REQUIREMENTS,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  &SystemPasswordRequirement
                  );
  if (!EFI_ERROR (Status)){
    SystemPasswordHashIteration = SystemPasswordRequirement.PcdSystemPasswordHashIteration;
  } else {
    SystemPasswordHashIteration = FixedPcdGetBool (PcdSystemPasswordHashIteration);
  }
  DigestSize   = 0;
  Count        = 0;
  ShiftPress   = FALSE;
  CaplockPress = FALSE;
  Len = UNIQUE_SYSTEM_NUMBER_LENGTH;
  HashType = GetUniqueNumber();
  ZeroMem(PasswordArray, sizeof(PasswordArray));
  CopyMem(PasswordArray, mLibSaltBuffer, Len);

  if(Type){
    //
    //convert Scancode to Ascii
    //
    for( i=0; i<Length; i++ ) 
    { 
      j=i*2;
      if((Password[j] == 0x00)&&(Password[j+1] == 0x00)) break; 
      if('0'<=Password[i]&&Password[i]<='9')Password[i]=(UINT16)(Password[j]-'0');
      else if('a'<=Password[j]&&Password[j]<='f') Password[i]=(UINT16)(Password[j]-'a')+10;       
      else if('A'<=Password[j]&&Password[j]<='F') Password[i]=(UINT16)(Password[j]-'A')+10;         
      else return 0;
      
      if('0'<=Password[j+1]&&Password[j+1]<='9') Password[i]=Password[i]*0x10+(UINT16)(Password[j+1]-'0');         
        else if('a'<=Password[j+1]&&Password[j+1]<='f')Password[i]=Password[i]*0x10+(UINT16)(Password[j+1]-'a')+10;    
        else if('A'<=Password[j+1]&&Password[j+1]<='F') Password[i]=Password[i]*0x10+(UINT16)(Password[j+1]-'A')+10;       
        else return 0;      
    }

    for(i=0;i<(Length/2);i++){
      if(Password[i] == 0x00) break;
        switch(Password[i])
        {
          case 0x2a: 
            //
            //Lshift pressed
            //
            ShiftPress=ShiftPress?FALSE:TRUE;
            break;
          case 0x3a:
            //
            //Capslock pressed
            //
            CaplockPress=CaplockPress?FALSE:TRUE;
            break;
          default:
            for(KeyIndex =0 ;KeyIndex < (sizeof(KeyDescriptor)/sizeof(KEY_DESCRIPTOR));KeyIndex++ )
            {
              if(Password[i] == KeyDescriptor[KeyIndex].ScanCode){
                if(ShiftPress){
                  Password[i-1] = (UINT8)KeyDescriptor[KeyIndex].ShiftedUnicode;
                  ShiftPress=ShiftPress?FALSE:TRUE;
                }else if(CaplockPress){
                       Password[i-1] = (UINT8)KeyDescriptor[KeyIndex].ShiftedUnicode;
                      }else{
                       Password[i] = (UINT8)KeyDescriptor[KeyIndex].Unicode;                    
                      }
                Count++;
                break;
               }
            }
            break;
        }
      }
       Length = Count;
    }
  // Judge HashType
  if (HashType == PASSWORD_HASH_SHA256) {
    DigestSize = SHA256_DIGEST_SIZE;
  } else if (HashType == PASSWORD_HASH_SM3) {
    DigestSize = SM3_DIGEST_SIZE;
  } else if (HashType == PASSWORD_HASH_SHA512) {
    DigestSize = SHA512_DIGEST_SIZE;
  } else if (HashType == PASSWORD_HASH_SHA384) {
    DigestSize = SHA384_DIGEST_SIZE;
  } else {
     ASSERT (FALSE);
  }
  
  ZeroMem(HashArray, sizeof(HashArray));
  if (SystemPasswordHashIteration) {
    HashIterateOk = Pkcs5HashPassword (
                      Length,
                      (CHAR8 *)Password,
                      Len,
                      mLibSaltBuffer,
                      DEFAULT_PASSWORD_ITERATION_COUNT,
                      DigestSize,
                      sizeof(HashArray),
                      HashArray
                    );
    if (!HashIterateOk) {
      return 0;
    }
  } else {
    if (HashType == PASSWORD_HASH_SHA256) {
      Sha256Hash(Password, Length, HashArray, sizeof(HashArray));
    } else if (HashType == PASSWORD_HASH_SM3) {
      Sm3Hash(Password, Length, HashArray, sizeof(HashArray));
    } else if (HashType == PASSWORD_HASH_SHA512) {
      Sha512Hash(Password, Length, HashArray, sizeof(HashArray));
    } else {
      Sha384Hash(Password, Length, HashArray, sizeof(HashArray));
    }
    CopyMem(PasswordArray + Len, HashArray, sizeof(HashArray));

    ZeroMem(HashArray, sizeof(HashArray));
    if (HashType == PASSWORD_HASH_SHA256) {
      Sha256Hash(PasswordArray, Len + sizeof(HashArray), HashArray, sizeof(HashArray));
    } else if (HashType == PASSWORD_HASH_SM3) {
      Sm3Hash(PasswordArray, Len + sizeof(HashArray), HashArray, sizeof(HashArray));
    } else if (HashType == PASSWORD_HASH_SHA512) {
      Sha512Hash(PasswordArray, Len + sizeof(HashArray), HashArray, sizeof(HashArray));
    } else {
      Sha384Hash(PasswordArray, Len + sizeof(HashArray), HashArray, sizeof(HashArray));
    }
  }

  Buf = NULL;
  Buf = AllocateZeroPool(SYSTEM_PASSWORD_HASH_LENGTH + 1);
  ASSERT (Buf != NULL);
  CopyMem(Buf, HashArray, SYSTEM_PASSWORD_HASH_LENGTH);

  *Hash = Buf;
  return SYSTEM_PASSWORD_HASH_LENGTH;
}

BOOLEAN
CompareInputPassword (
  IN  UINT8    *PasswordHash,
  IN  CHAR16    *InputPassword
  )
{
  UINTN    Index;
  UINTN    i;
  UINTN    Count;
  UINTN    Len;
  UINT8    *Str;
  UINT8    *InputHash;

  Len = Unicode2Ascii(InputPassword, &Str);
  if (0 < Len) {
    //
    // Uppercase of input.
    //
    for (i = 0; i < Len; i++) {
      if (Str[i] > 96 && Str[i] <123 ) {
        Str[i] = Str[i] - 32;
      }
    }
	
    Count = EncodePassword(Str, Len, &InputHash,0);
    if (Count == 0) {
      return FALSE;
    }
    FreePool (Str);

    for (Index = 0; Index < SYSTEM_PASSWORD_HASH_LENGTH; Index++) {
      if (InputHash[Index] != PasswordHash[Index] ) {
        FreePool (InputHash);
        return FALSE;
      }
    }

    FreePool (InputHash);
  } else {

    for (Index = 0; Index < SYSTEM_PASSWORD_HASH_LENGTH; Index++) {
      if (0 != PasswordHash[Index] ) {
        return FALSE;
      }
    }
  }
  return TRUE;
}

BOOLEAN 
SpLibCheckPasswordMatch(CHAR16 *Password, UINT8 CheckType)
{
  UINTN   i;
  UINTN   Length;

  Length = StrLen(Password);

  switch(CheckType) {
    case 0:
      for(i = 0; i < Length; i++) {
        if((Password[i] >= '0') && (Password[i] <= '9')) {
          //DEBUG((EFI_D_INFO, "CheckType0: Password[%d] = %c\n",i,Password[i]));
          return TRUE;
        }
      }
      break;
    case 1:
      for(i = 0; i < Length; i++) {
        if((Password[i] >= 'A') && (Password[i] <= 'Z')) {
          //DEBUG((EFI_D_INFO, "CheckType1: Password[%d] = %c\n",i,Password[i]));
          return TRUE;
        }
      }
      break;
    case 2:
      for(i = 0; i < Length; i++) {
        if(((Password[i] >= ' ') && (Password[i] <= '/')) ||
           ((Password[i] >= ':') && (Password[i] <= '@')) ||
           ((Password[i] >= '[') && (Password[i] <= '`')) ||
           ((Password[i] >= '{') && (Password[i] <= '~'))) {
          //DEBUG((EFI_D_INFO, "CheckType2: Password[%d] = %c\n",i,Password[i]));
          return TRUE;
        }
      }
      break;
    case 3:
      for(i = 0; i < Length; i++) {
        if((Password[i] >= 'a') && (Password[i] <= 'z')) {
          //DEBUG((EFI_D_INFO, "CheckType3: Password[%d] = %c\n",i,Password[i]));
          return TRUE;
        }
      }
      break;
    default:
      return FALSE;
      break;
  }
  return FALSE;
}

/**
  Check the complexity of the password (the password must contain characters).

  @param  Password  Input string

  @retval TRUE  The password meets the complexity requirement.
  @retval FALSE The password does not meet the complexity requirement.
**/
BOOLEAN 
ByoVerifyPasswordComplexity(CHAR16 *Password)
{
  UINT16 i;
  UINT16 j;
  UINT16 k;
  UINT16 n;

  i = SpLibCheckPasswordMatch(Password,0);
  j = SpLibCheckPasswordMatch(Password,1);
  k = SpLibCheckPasswordMatch(Password,2);
  n = SpLibCheckPasswordMatch(Password,3);

  if(i + j + k + n >= MIN_PASSWORD_COMPLEXITY) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Verify that the password matches the one you set before.

  @param  Type  password Type,admin and power on
  @param  Password  Input password string

  @return 1   Password is invalid,The password has been set before (less than five times).
  @retval 0   Password is valid, The password is not set before (less than five times).
**/
UINT8
ByoCheckPasswordRecord (
  PASSWORD_TYPE    Type,
  CHAR16           *Password
)
{
  EFI_STATUS                Status;
  UINTN                     VariableSize;
  BYO_SYSTEM_PASSWORD_RECORDER  PasswordRecorder;
  SYSTEM_PASSWORD_REQUIREMENTS  SystemPasswordRequirement;
  UINTN                     Len;
  UINT8                     *Str = NULL;
  UINT8                     *PasswordHash = NULL;
  UINTN                     Index;
  UINT8                     ReCount = 0;
 
  if (gRecoederTime == 0xFF) {
    VariableSize = sizeof(SYSTEM_PASSWORD_REQUIREMENTS);
    Status = PasswordGetVariable (
                    PASSWORD_REQUIREMENTS,
                    &gEfiSystemPasswordVariableGuid,
                    NULL,
                    &VariableSize,
                    &SystemPasswordRequirement
                    );
    if (!EFI_ERROR (Status)){
      gRecoederTime = SystemPasswordRequirement.RecoderTime;
    } else {
      gRecoederTime = PcdGet8(PcdRecorderPasswordTime);
    }
 }

  if (RECORDER_TIME == 0) {
  	return 0;
  }
  Len = Unicode2Ascii(Password, &Str);
  if (Len > 0) {
    EncodePassword(Str, Len, &PasswordHash, 0);
    if(Str != NULL){
      FreePool(Str);
    }
  } else {
    return 0;
  }

  VariableSize = sizeof(BYO_SYSTEM_PASSWORD_RECORDER);
  Status = PasswordGetVariable (
                  SYSTEM_PASSWORD_RECORDER_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  &PasswordRecorder
                  );
  if (EFI_ERROR (Status)) {
    ZeroMem(&PasswordRecorder, VariableSize);
  }

  for (Index = 0; Index < RECORDER_TIME; Index ++) {
    if (Type == PD_ADMIN) {
      Str = PasswordRecorder.Admin[Index];
    } else {
      Str = PasswordRecorder.PowerOn[Index];
    }
    if (CompareMem(Str, PasswordHash, SYSTEM_PASSWORD_HASH_LENGTH) == 0) {
      break;
    }
  }

  if (Index < RECORDER_TIME) {
    return 1;
  }

  if (Type == PD_ADMIN) {
    ReCount = PasswordRecorder.AdmCount;
    if (ReCount == RECORDER_TIME) {
      for (Index = 0; Index < (RECORDER_TIME - 1); Index ++) {
        CopyMem(PasswordRecorder.Admin[Index], PasswordRecorder.Admin[Index + 1], SYSTEM_PASSWORD_HASH_LENGTH);
      }
      Str = PasswordRecorder.Admin[ReCount - 1];
    } else {
      Str = PasswordRecorder.Admin[ReCount];
      PasswordRecorder.AdmCount ++;
    }
  } else {
    ReCount = PasswordRecorder.PopCount;
    if (ReCount == RECORDER_TIME) {
      for (Index = 0; Index < (RECORDER_TIME - 1); Index ++) {
        CopyMem(PasswordRecorder.PowerOn[Index], PasswordRecorder.PowerOn[Index + 1], SYSTEM_PASSWORD_HASH_LENGTH);
      }
      Str = PasswordRecorder.PowerOn[ReCount - 1];
    } else {
      Str = PasswordRecorder.PowerOn[ReCount];
      PasswordRecorder.PopCount ++;
    }
  }
  CopyMem(Str, PasswordHash, SYSTEM_PASSWORD_HASH_LENGTH);

  Status = PasswordSetVariable (
                  SYSTEM_PASSWORD_RECORDER_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  VariableSize,
                  &PasswordRecorder
                  );
  if(PasswordHash != NULL){
    FreePool (PasswordHash);
  }
  return 0;
}

EFI_STATUS
ByoReadSystemPassword(
  IN  PASSWORD_TYPE    Type,
  OUT UINT8            **Password
  )
{
  EFI_STATUS         Status;
  UINTN              VariableSize;
  EFI_GUID           PasswordGuid = SYSTEM_PASSWORD_GUID;
  SYSTEM_PASSWORD    SetupPassword;
  UINT8              *PasswordBuf = NULL;

  VariableSize = sizeof(SYSTEM_PASSWORD);
  Status = PasswordGetVariable (
                SYSTEM_PASSWORD_NAME,
                &PasswordGuid,
                NULL,
                &VariableSize,
                &SetupPassword
                );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  switch (Type) {
    case PD_ADMIN:
      PasswordBuf = AllocateCopyPool (SYSTEM_PASSWORD_HASH_LENGTH, &SetupPassword.AdminHash);
      break;
    case PD_POWER_ON:
      PasswordBuf = AllocateCopyPool (SYSTEM_PASSWORD_HASH_LENGTH, &SetupPassword.PowerOnHash);
      break;
    default:
      return EFI_INVALID_PARAMETER;
  }

  *Password = PasswordBuf;
  return EFI_SUCCESS;
}

/**
  Verify that passwords are the same. Checks whether the passed "String" matches the password for that "Type".

  @param  Type  password Type,admin and power on
  @param  String  Input password string

  @return TRUE   Same password.
  @retval FALSE  Different passwords.
**/
BOOLEAN
ByoCheckRepeatPsd(
  IN  PASSWORD_TYPE    Type,
  IN  CHAR16           *String
  )
{
  UINTN             Index;
  UINTN             Len;
  UINT8             *Str;
  UINT8             *InputHash;
  UINT8             *PasswordHash;
  EFI_STATUS        Status;

  Len = Unicode2Ascii(String, &Str);
  if (0 < Len) {
    EncodePassword(Str, Len, &InputHash,0);
    FreePool (Str);
    Status = ByoReadSystemPassword(Type, &PasswordHash);
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
    for (Index = 0; Index < SYSTEM_PASSWORD_HASH_LENGTH; Index++) {
      if (InputHash[Index] != PasswordHash[Index] ) {
        FreePool (PasswordHash);
        FreePool (InputHash);
        return FALSE;
      }
    }

    FreePool (InputHash);
    FreePool (PasswordHash);
  } else {
    Status = ByoReadSystemPassword(Type, &PasswordHash);
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
    for (Index = 0; Index < SYSTEM_PASSWORD_HASH_LENGTH; Index++) {
      if (0 != PasswordHash[Index] ) {
        FreePool (PasswordHash);
        return FALSE;
      }
    }
  }
  return TRUE;
}


EFI_STATUS
EFIAPI
SystemPasswordLibConstructor (
  IN      EFI_HANDLE                ImageHandle,
  IN      EFI_SYSTEM_TABLE          *SystemTable
  )
{
  EFI_STATUS                        Status;

  Status = PasswordCreatEvent ();
  return Status;
}