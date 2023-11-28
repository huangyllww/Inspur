#ifndef __RECOVERY_H__
#define __RECOVERY_H__


#define MAX_CAPUSLE_NUMBER 8
/*
typedef struct {
  EFI_PHYSICAL_ADDRESS    BaseAddress;
  UINT64                  Length;
  BIOS_ID_IMAGE           BiosIdImage;
  EFI_GUID                DeviceId;
  BOOLEAN                 Actived;
} CAPSULE_INFO;

typedef struct {
  CAPSULE_INFO        CapsuleInfo[MAX_CAPUSLE_NUMBER];
  UINT8               CapsuleCount;
} CAPSULE_RECORD;*/

typedef struct {
  EFI_GUID          CapsuleGuid;
  UINT32            HeaderSize;
  UINT32            Flags;
  UINT32            CapsuleImageSize;
} EFI_CAPSULE_HEADER;

/*
EFI_CAPSULE_HEADER
  CapsuleGuid         // 16
  HeaderSize          // 4
  Flags               // 4
  CapsuleImageSize    // 4
-------------------------------------------------------------
  (+) PubkeySize      // 4            +28
  (+) SignSize        // 4            +32
  (+) RangeArraySize  // 4            +36

  (+) Pubkey          //              +40
  (+) Sign            //              +40+PubkeySize            align 4
  (+) Range[]         //              +40+PubkeySize+SignSize   align 4
-------------------------------------------------------------
FD                    // 16 align
*/
typedef struct {
  EFI_CAPSULE_HEADER   Header;
  UINT32               PubkeySize;
  UINT32               SignSize;
  UINT32               RangeArraySize;
  UINT8                Data[1];
} BIOS_VERIFY_CAPSULE_BUFFER;


extern EFI_GUID gRecoveryCapsuleRecordGuid;
extern EFI_GUID gSignCapsuleHeaderGuild;

#endif
