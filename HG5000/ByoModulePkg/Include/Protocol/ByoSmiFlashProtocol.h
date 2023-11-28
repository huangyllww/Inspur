
#ifndef __BYO_SMIFLASH_PROTOCOL_H__
#define __BYO_SMIFLASH_PROTOCOL_H__

#include <SmbiosPnp52.h>

extern EFI_GUID gByoSmiFlashProtocolGuid;
extern EFI_GUID gByoSmiFlashSmmProtocolGuid;

typedef struct _BYO_SMIFLASH_PROTOCOL  BYO_SMIFLASH_PROTOCOL;


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
(EFIAPI *BYO_SAVE_SETUP_PASSWORD)(
  IN  BYO_SMIFLASH_PROTOCOL          *This
  );  

typedef
VOID
(EFIAPI *BYO_FIRST_POWER_ON)(
  IN  BYO_SMIFLASH_PROTOCOL          *This
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
EFI_STATUS
(EFIAPI *BYO_VERIFY_SIGN_BIOS)(
  IN BYO_SMIFLASH_PROTOCOL		   *This,
  IN UINT8   					   *Buffer,
  IN UINTN   					   CapsuleSize
  );

struct _BYO_SMIFLASH_PROTOCOL {
  BYO_VERIFY_SIGN_BIOS    VerifySign;
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
  BYO_SAVE_SETUP_PASSWORD SaveSetupPassword;
  BYO_FIRST_POWER_ON	  FirstPowerOn;
};



#endif           // __BYO_SMIFLASH_PROTOCOL_H__

