
#ifndef __BYO_SMIFLASH_LOCK_PROTOCOL_H__
#define __BYO_SMIFLASH_LOCK_PROTOCOL_H__

typedef struct _BYO_SMIFLASH_LOCK_PROTOCOL  BYO_SMIFLASH_LOCK_PROTOCOL;


#define BYO_SMIFLASH_STS_SMBIOS_LOCK      BIT0
#define BYO_SMIFLASH_STS_MASK             (BYO_SMIFLASH_STS_SMBIOS_LOCK)

typedef
EFI_STATUS
(EFIAPI *BYO_SMIFLASH_GET_LOCK_STATUS)(
  IN  BYO_SMIFLASH_LOCK_PROTOCOL     *This,
  OUT UINTN                          *LockStatus
  );

typedef
EFI_STATUS
(EFIAPI *BYO_SMIFLASH_UNLOCK)(
  IN BYO_SMIFLASH_LOCK_PROTOCOL      *This,
  IN VOID                            *Buffer,
  IN UINTN                           BufferSize
  );

typedef
EFI_STATUS
(EFIAPI *BYO_SMIFLASH_LOCK)(
  IN BYO_SMIFLASH_LOCK_PROTOCOL      *This
  );

typedef
EFI_STATUS
(EFIAPI *BYO_SMIFLASH_SMBIOS_UPDATE_FILTER)(
  IN BYO_SMIFLASH_LOCK_PROTOCOL      *This,
  IN UINT8                           Type,
  IN UINT8                           Offset
  );



struct _BYO_SMIFLASH_LOCK_PROTOCOL {
  BYO_SMIFLASH_GET_LOCK_STATUS       GetLockStatus;
  BYO_SMIFLASH_UNLOCK                Unlock;
  BYO_SMIFLASH_LOCK                  Lock;
  BYO_SMIFLASH_SMBIOS_UPDATE_FILTER  SmbiosFilter;
};


extern EFI_GUID gByoSmiFlashLockProtocolGuid;

#endif

