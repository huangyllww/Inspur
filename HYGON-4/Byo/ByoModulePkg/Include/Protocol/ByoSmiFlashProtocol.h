/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_SMIFLASH_PROTOCOL_H__
#define __BYO_SMIFLASH_PROTOCOL_H__

#include <SmbiosPnp52.h>

extern EFI_GUID gByoSmiFlashProtocolGuid;

typedef struct _BYO_SMIFLASH_PROTOCOL  BYO_SMIFLASH_PROTOCOL;


#define SIGN_INIT_HASH_TYPE_SHA256       0
#define SIGN_INIT_HASH_TYPE_SHA512       1

#define SKIP_BIOS_ID   BIT0
#define SKIP_AC        BIT1
#define ALLOW_PRINT    BIT2

#define BIOS_UPDATE_OPTIONS_NAME L"BiosUpdateOptions"

#pragma pack(1)

typedef struct {
  UINT32  HashType;
  UINT32  SignSize;
  UINT32  SignFvOffset;
  UINT8   SignMethod;
  UINT8   SignData[1];
} SIGN_INIT_DATA;

typedef struct {
  UINT32        SkipFlag;
  UINT32        Reserved[3];
} BIOS_UPDATE_OPTIONS; 

#pragma pack()


typedef
EFI_STATUS
(EFIAPI *BYO_GET_BIOS_INFO2)(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT8                          *NewBios,
  IN  UINTN                          NewBiosSize,  
  OUT VOID                           **Info
  );

typedef
EFI_STATUS
(EFIAPI *BYO_UPDATE_STEP_STATUS)(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Step
  );

typedef
EFI_STATUS
(EFIAPI *BYO_UPDATE_PREPARE_ENV)(
  IN  BYO_SMIFLASH_PROTOCOL          *This
  );

typedef
EFI_STATUS
(EFIAPI *BYO_UPDATE_CLEAR_ENV)(
  IN  BYO_SMIFLASH_PROTOCOL          *This
  );
  
typedef
EFI_STATUS
(EFIAPI *BYO_PROGRAM_FLASH)(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  VOID                           *Data,
  IN  UINT32                         DataSize
  );

typedef
EFI_STATUS
(EFIAPI *BYO_DEFAULT_UPDATE)(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT8                          *NewBiosData,
  IN  UINTN                          NewBiosDataSize
  );  

typedef
EFI_STATUS
(EFIAPI *BYO_UPDATE_ALL)(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT8                          *NewBiosData,
  IN  UINTN                          NewBiosDataSize
  ); 

typedef
EFI_STATUS
(EFIAPI *BYO_RECOVERY_UPDATE)(
  IN  BYO_SMIFLASH_PROTOCOL          *This
  );  

typedef
EFI_STATUS
(EFIAPI *BYO_READ_FLASH)(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  VOID                           *Data,
  IN  UINT32                         DataSize
  );

typedef
EFI_STATUS
(EFIAPI *BYO_ERASE_FLASH)(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  UINT32                         Size
  );

typedef
EFI_STATUS
(EFIAPI *BYO_WRTIE_FLASH)(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  VOID                           *Data,
  IN  UINT32                         DataSize  
  );

typedef
EFI_STATUS
(EFIAPI *BYO_UPDATE_SMBIOS)(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UPDATE_SMBIOS_PARAMETER        *SmbiosPtr
  );

typedef
EFI_STATUS
(EFIAPI *BYO_BIOS_CHECK)(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT8                          *NewBiosData,
  IN  UINTN                          NewBiosDataSize,
  OUT VOID                           **pBiosInfo2          OPTIONAL 
  );

typedef
VOID
(EFIAPI *PROGRESS_CALLBACK)(
  IN UINTN     Persent
  );

typedef
EFI_STATUS
(EFIAPI *BYO_SET_SIGN_VERIFY_PROGRESS_CALLBACK)(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  PROGRESS_CALLBACK              CallBack
  );

typedef
EFI_STATUS
(EFIAPI *BYO_UPDATE_FLASH_VERIFY) (
  IN  BYO_SMIFLASH_PROTOCOL          *This
  );

typedef enum {
  BIOS_UPDATA_DRAW_TYPE_ENTRY = 1,
  BIOS_UPDATA_DRAW_TYPE_INFO,
  BIOS_UPDATA_DRAW_TYPE_ERROR,
  BIOS_UPDATA_DRAW_TYPE_REFRESH,
  BIOS_UPDATA_DRAW_TYPE_EXIT,

  BIOS_UPDATA_DRAW_TYPE_COPYRIGHT_INFO,
} BIOS_UPDATA_DRAW_TYPE;

/**
  Show Bios Update Progress

  @param[in] Type         Draw Type
                          BIOS_UPDATA_DRAW_TYPE_START   The first drawing
                          BIOS_UPDATA_DRAW_TYPE_RUN     The updating drawing
                          BIOS_UPDATA_DRAW_TYPE_END     The end drawing
  @param[in] Rate         0-10000 Two decimal places
  @param[in] Context      Bios Update Progress Bar Draw Function Context

  @retval EFI_SUCCESS     Draw Success
  @retval Other           Draw Error

**/
typedef
EFI_STATUS
(EFIAPI *SHOW_PROGRESS_CALLBACK)(
  IN UINTN     Type,
  IN UINT64    Rate,
  IN CHAR16    *Info,
  IN VOID      *Context
  );

/**
  Register Bios Update Draw Function

  @param[in] This         The pointer to the byo smiflash protocol instance.
  @param[in] CallBack     Bios Update Progress Bar Draw Function
  @param[in] Context      Bios Update Progress Bar Draw Function Context

  @retval EFI_SUCCESS     Register Success
  @retval Other           Register Error

**/
typedef
EFI_STATUS
(EFIAPI *BYO_SET_SHOW_PROGRESS_CALLBACK)(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  SHOW_PROGRESS_CALLBACK         CallBack,
  IN  VOID                           *Context       OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *BYO_BACKUP_READ_FLASH)(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  UINT8                          *Data,
  IN  UINT32                         DataSize
  );

typedef
EFI_STATUS
(EFIAPI *BYO_PROGRAM_BACKUP_FLASH)(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  UINT8                          *Data,
  IN  UINT32                         DataSize
  ); 

typedef
UINT32
(EFIAPI *BYO_GET_PROGRAM_FLASH_BLOCK_SIZE)(
  );

struct _BYO_SMIFLASH_PROTOCOL {
  BYO_GET_BIOS_INFO2      GetBiosInfo2;
  BYO_UPDATE_PREPARE_ENV  PrepareEnv;
  BYO_UPDATE_CLEAR_ENV    ClearEnv;
  BYO_UPDATE_STEP_STATUS  UpdateStep;
  BYO_PROGRAM_FLASH       ProgramFlash;
  BYO_DEFAULT_UPDATE      DefaultUpdate;
  BYO_RECOVERY_UPDATE     Recovery;
  BYO_READ_FLASH          Read;
  BYO_ERASE_FLASH         Erase;
  BYO_WRTIE_FLASH         Write;
  BYO_UPDATE_SMBIOS       UpdateSmbios;
  BYO_BIOS_CHECK          BiosCheck;
  BYO_SET_SIGN_VERIFY_PROGRESS_CALLBACK  SetVerifyProgressCallBack;
  BYO_SET_SHOW_PROGRESS_CALLBACK         SetShowProgressCallBack;
  BYO_PROGRAM_BACKUP_FLASH               ProgramBackupFlash;
  BYO_BACKUP_READ_FLASH                  BackupRead;
  BYO_GET_PROGRAM_FLASH_BLOCK_SIZE       GetFlashBlockSize;
  BYO_UPDATE_ALL          UpdateAll;
  BYO_UPDATE_FLASH_VERIFY FlashVerify;
};



#endif           // __BYO_SMIFLASH_PROTOCOL_H__

