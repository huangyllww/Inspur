/*++
Copyright (c) 2011 Byosoft Corporation. All rights reserved.

Module Name:

  SmiFlash.h

Abstract:

 This file contains the Includes, Definitions, typedefs,
 Variable and External Declarations, Structure and
 function prototypes needed for the SmiFlash driver
--*/

#include <PiSmm.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/SetMemAttributeSmmLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/NvMediaAccess.h>
#include <Protocol/Smbios.h>
#include <Library/BiosIdLib.h>
#include <Guid/SmBios.h>
#include <IndustryStandard/SmBios.h>
#include <Library/UefiLib.h>
#include <Protocol/ByoSmiFlashProtocol.h>
#include <ByoSmiFlashInfo.h>

#include <openssl/ossl_typ.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <Library/BaseCryptLib.h>

#define FLASH_REGION_NVSTORAGE_SUBREGION_NV_SMBIOS_STORE_BASE        PcdGet32 (PcdFlashNvStorageSmbiosBase)
#define FLASH_REGION_NVSTORAGE_SUBREGION_NV_SMBIOS_STORE_SIZE        SIZE_4KB
#define FLASH_REGION_NVLOGO_STORE_BASE                               PcdGet32 (PcdFlashNvLogoBase)
#define FLASH_REGION_NVLOGO_STORE_SIZE                               PcdGet32 (PcdFlashNvLogoSize)
#define FLASH_REGION_NVVARIABLE_STORE_BASE                           PcdGet32 (PcdFlashNvStorageVariableBase)
#define FLASH_REGION_NVVARIABLE_STORE_SIZE                           PcdGet32 (PcdFlashNvStorageVariableSize)


#define SMBIOS_BUFFER_SIZE        4096
#define END_HANDLE                0xFFFF
#define SMBIOS_REC_SIGNATURE      0x55AA
#define MAX_STRING_LENGTH         30


#pragma pack(1)
typedef struct {
  UINT32        Type;                         // IN, Type of flash area
  UINT32        Offset;                       // OUT, Offset from FD base address or base address for FD_AREA_TYPE_FD
  UINT32        Size;                         // OUT, Byte size of flash area
} FD_AREA_INFO;


typedef enum {
  FD_AREA_TYPE_NULL = 0,
  FD_AREA_TYPE_FD,
  FD_AREA_TYPE_FV_LOGO,           
  FD_AREA_TYPE_DESCRIPTOR,
  FD_AREA_TYPE_BIOS_GBE,
  FD_AREA_TYPE_BIOS_ME,
  FD_AREA_TYPE_FV_SEC,
  FD_AREA_TYPE_FV_TOPSWAP,
  FD_AREA_TYPE_FV_RECOVERY2,
  FD_AREA_TYPE_FV_RECOVERY3,
  FD_AREA_TYPE_FV_FIRMWAREBIN,
  FD_AREA_TYPE_FV_FSPT,
  FD_AREA_TYPE_FV_FSPS,
  FD_AREA_TYPE_FV_FSPM,
  FD_AREA_TYPE_FV_FSPMT,
  FD_AREA_TYPE_NVM,     
  FD_AREA_TYPE_FV_SMBIOS,       
  FD_AREA_TYPE_MICROCODE_FV,          
  FD_AREA_TYPE_FVMAIN,
  FD_AREA_TYPE_FVCNV,
  FD_AREA_TYPE_FV_RECOVERY2_BACKUP,
  FD_AREA_TYPE_FV_RECOVERY3_BACKUP,
  FD_AREA_TYPE_FV_FIRMWAREBIN_BACKUP,
  FD_AREA_TYPE_FV_FSPT_BACKUP,
  FD_AREA_TYPE_FV_FSPS_BACKUP,
  FD_AREA_TYPE_FV_FSPM_BACKUP,
  FD_AREA_TYPE_FV_FSPMT_BACKUP,
  FD_AREA_TYPE_NVM_BACKUP,
  FD_AREA_TYPE_END
} FD_AREA_TYPE;



typedef struct {
    UINT32               BufferSize;
    EFI_PHYSICAL_ADDRESS Buffer;
} ROM_HOLE_PARAMETER;

typedef struct {
    UINT32               BufferSize;
	UINT32               LogoOffest;
    EFI_PHYSICAL_ADDRESS              Buffer;
    UINT8                 Flag;
} NVLOGO_PARAMETER;

typedef enum {
  VERIFY_INIT = 1,
  VERIFY_UPDATE,
  VERIFY_FINAL
} BIOS_VERIFY_SUBFUN;

typedef struct {
  UINT8                SubFun;
  EFI_PHYSICAL_ADDRESS Buffer;
  UINT32               Size;
} BIOS_VERIFY_PARAMETER;

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


#pragma pack()


EFI_STATUS HandleSmbiosDataRequest(UPDATE_SMBIOS_PARAMETER *SmbiosPtr);
EFI_STATUS AllocDataBuffer();

extern NV_MEDIA_ACCESS_PROTOCOL  *mMediaAccess;
extern EFI_GUID gSignCapsuleHeaderGuild;



