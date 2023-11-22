/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  HddSanitizationDxe.h

Abstract:
  Hdd password DXE driver.

Revision History:

**/



#ifndef _HDD_SANITIZE_DXE_H_
#define _HDD_SANITIZE_DXE_H_
//-----------------------------------------------------------------------

#include <Uefi.h>
#include <Pi/PiBootMode.h>
#include <IndustryStandard/Atapi.h>
#include <Protocol/AtaPassThru.h>
#include <Protocol/OemHookProtocol2.h>
#include <Protocol/PciIo.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/SetupSaveNotify.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Guid/MdeModuleHii.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/HiiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Library/ByoCommLib.h>
#include <Library/SetupUiLib.h>
#include <Library/HobLib.h>
#include <Library/TimerLib.h>
#include "HddSanitizationCfg.h"
#include <Library/TcgStorageOpalLib.h>
#include <Guid/OpalSpStorage.h>
#include <Protocol/BlockIo.h>
#include <IndustryStandard/Nvme.h>
#include <Library/OpalPasswordSupportLib.h>
#include <Protocol/NvmeSscpHookProtocol.h>
#include <Protocol/StorageSecurityCommand.h>
#include <Protocol/ByoPcdAccess.h>
#include <Protocol/ByoNetworkIdentifierProtocol.h>
#include <Protocol/DisplayBackupRecovery.h>


extern BYO_PCD_ACCESS_PROTOCOL *mPcdAccessProtocol;

#define DEV_INFO_TYPE(devinfo)                (*(UINT32 *)(DevInfo))
#define ATA_DEVICE_INFO_SIGN                  SIGNATURE_32('_', 'A', 'T', 'A')
#define NVME_DEVICE_INFO_SIGN                 SIGNATURE_32('N', 'V', 'M', 'e')
#define HDD_PASSWORD_DXE_PRIVATE_SIGNATURE    SIGNATURE_32 ('_', 'H', '_', 'S')


#define HDD_STRING_LENGTH                     64

#define ERASE_BLOCKS_COUNT                    0x800

#define NVME_ADMIN_SANITIZE_CMD               0x84
#define NVME_CONTROLLER_NSID                  0x0
#define NVME_LOG_LID_SANITIZE					        0x81



//
// Time out value for ATA pass through protocol
// When unlocking the password,the 3-second timeout value of some hard disks is not enough,
// so it is changed to 5 seconds.
//
#define ATA_TIMEOUT                           EFI_TIMER_PERIOD_SECONDS (5)

#define BLT_BACKGROUND_WHITE    0x50

//
//Iterate through the doule linked list. NOT delete safe
//
#define EFI_LIST_FOR_EACH(Entry, ListHead)    \
  for(Entry = (ListHead)->ForwardLink; Entry != (ListHead); Entry = Entry->ForwardLink)

//
// Tcg_Opal feature information
//
typedef struct {
  // Copied from DeviceInfo datastructure
  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL  *Sscp;
  //
  //Opal device Session
  //
  OPAL_SESSION                           Session;
  //
  //Opal device base identify data
  //
  UINT32                                 MediaId;
  UINT16                                 OpalBaseComId;
  //
  //Opal device Security status and password authority(Admin1 / User1) used to lock/unlock/frozen/Erase device
  //
  OPAL_DISK_SUPPORT_ATTRIBUTE            SupportedAttributes;
  BOOLEAN                                SIDBlocked;
  TCG_LOCKING_FEATURE_DESCRIPTOR         LockingFeature; 
  TCG_UID                                OpalPasswordOwner;
} OPAL_FEATURE_INFO;

//
// Common information Entity for every storage device
//
typedef struct {
  //
  // Device Controller info
  //
  UINT8                                  Bus;
  UINT8                                  Dev;
  UINT8                                  Func;
  UINT8                                  PciScc;
  EFI_PCI_IO_PROTOCOL                    *PciIo;
  EFI_HANDLE                             Controller;
  EFI_DEVICE_PATH                        *ControllerDevPath;
  //
  // Device security State & Erase timeout
  //
  UINT8                                  Supported;
  BOOLEAN                                Enabled;
  BOOLEAN                                Locked;
  UINT8                                  Frozen;
  UINT8                                  CountExpired;
  //
  // Misc
  //
  UINT16                                 HddIndex;
} HDD_PASSWORD_INFO;

//
// Single NVMe device information
//
typedef struct {
  //
  // NVMe device base Information
  //
  UINT32                                 Sign;
  UINT8                                  SerialNo[21];
  UINT8                                  Mn[41];
  UINT32                                 NamespaceId;
  //
  // NVMe device Protocol & devcie info
  //
  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL  *Sscp;
  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL     *NvmePassThru;
  EFI_BLOCK_IO_PROTOCOL                  *BlockIo;
  // ATA_SEC device information
  //
  BOOLEAN                                IsAtaSec;
  //
  //for the Time required for a ATA_SEC device Normal Erase mode SECURITY ERASE UNIT command.
  //
  UINT32                                 EraseUnitTimeOut; 
  //
  //data buffer to transfer between the ATA controller and the ATA device for write/read or bidirectional commands.
  //
  VOID                                   *AtaSecPayload;
  UINTN                                  AtaSecPayloadSize;
  //
  //NVMe Sanitize Supported types OverWrite(0x4) | BLOCKERASE(0x2) | CryptoErase(0x1)
  //
  UINT8                                  SanitizeCap;
  //
  //NVMe Sanitize Supported types OverWrite(0x4) | BLOCKERASE(0x2) | CryptoErase(0x1)
  //
  UINT8                                  FormatAttributes;
  //
  // Device OPAL/Pyrite SSC information
  //
  OPAL_FEATURE_INFO                      *OpalInfo;
  //
  // Common Device Info 
  //
  HDD_PASSWORD_INFO                      HdpInfo;
} NVME_DEVICE_INFO;

//
// Single ATA device information
//
typedef struct {
  //
  // ATA device base information
  UINT32                                  Sign;
  //
  UINT8                                   SerialNo[20];
  //
  //ATA device protrocol
  //
  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL   *Sscp;
  EFI_ATA_PASS_THRU_PROTOCOL              *AtaPassThru;
  EFI_BLOCK_IO_PROTOCOL                   *BlockIo;
  EFI_DEVICE_PATH_PROTOCOL                *DevicePath;
  //
  //Port information
  //
  UINT16                                  Port;
  UINT16                                  PortMp;
  //
  //sense data that was generated by the execution of the ATA command.
  // Asb is aligned to IoAlign
  //
  EFI_ATA_STATUS_BLOCK                    *Asb;
  //
  //data buffer to transfer between the ATA controller and the ATA device for read and bidirectional commands.
  //IdentifyData is aligned to IoAlign
  //
  ATA_IDENTIFY_DATA                       *IdentifyData;
  //
  //data buffer to transfer between the ATA controller and the ATA device for write or bidirectional commands.
  //PayLoad is aligned to IoAlign
  //
  UINT16                                  *PayLoad;
  UINTN                                   PayLoadSize;
  //
  //ATA Sanitize Supported (Word 59 BIT 12)
  //
  UINT8                                  SanitizeSupported;
  //
  //ATA Sanitize SanitizeCap (Word 59 BIT13 | BIT14 |BIT15)
  //
  UINT8                                  SanitizeCap;
  //
  //for the Time required for a Normal Erase mode SECURITY ERASE UNIT command.
  //
  UINT32                                  EraseUnitTimeOut; 
  //
  //A 4-byte value containing the device ID and vendor ID of the Pci device,at offset 0 in the PCI configuration space for the PCI controller.
  //
  UINT32                                  PciId;
  //
  // Device OPAL/Pyrite SSC information
  //
  OPAL_FEATURE_INFO                       *OpalInfo;
  //
  // Common Device Info 
  //
  HDD_PASSWORD_INFO                        HdpInfo;
} ATA_DEVICE_INFO;

typedef struct {
  LIST_ENTRY                    Link;
  CHAR16                        HddString[HDD_STRING_LENGTH];
  EFI_STRING_ID                 TitleToken;
  EFI_STRING_ID                 TitleHelpToken;
  HDD_SECURITY_INFO             HddSecInfo;
  //
  // Device Command & Communication Protocol
  //
  VOID                          *DevInfo;
} HDD_SANITIZE_DEVICE_ENTRY;

#pragma pack(1)
typedef struct {
  UINT8     Reserved;
  UINT8     ParamLen;
  UINT16    EraseTime;
  UINT16    EhEraseTime;
  UINT16    MasterIdentify;
  UINT8     MaxSet;
  UINT8     Status;
  UINT8     Reserved2[6];
} NVME_ATA_SECU_STS;
#pragma pack()


typedef struct {
  UINTN                            Signature;
  EFI_HANDLE                       DriverHandle;
  EFI_HII_HANDLE                   HiiHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL   ConfigAccess;
  HDD_SANITIZE_DEVICE_ENTRY        *Current;
  SETUP_SAVE_NOTIFY_PROTOCOL       SetupSaveNotify;
  HDD_SANITIZE_DEVICE_ENTRY        Dummy;
} HDD_SANITIZE_DXE_PRIVATE_DATA;


#pragma pack(1)

///
/// HII specific Vendor Device Path definition.
///
typedef struct {
  VENDOR_DEVICE_PATH           VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL     End;
} HDP_HII_VENDOR_DEVICE_PATH;

#pragma pack()

//
//For support erase data progress box
//
typedef struct {
  UINT32  ElapsedTimeInSeconds;
  UINT32  EraseUnitTimeOutInSeconds;
} TIMER_CALLBACK_CTX;

typedef enum {
  HDD_ERASE_DRAW_TYPE_ENTRY = 1,
  HDD_ERASE_DRAW_TYPE_REFRESH,
  HDD_ERASE_DRAW_TYPE_EXIT,
} HDD_ERASE_DRAW_TYPE;

typedef enum {
  TIMEOUT_DRAW_TYPE_ENTRY = 1,
  TIMEOUT_DRAW_TYPE_REFRESH,
  TIMEOUT_DRAW_TYPE_EXIT,
} TIMEOUT_DRAW_TYPE;

typedef struct {
  BOOLEAN                         Init;
  EFI_QUESTION_ID                 CurQid;
  EFI_HII_HANDLE                  HiiHandle;
  HDD_SANITIZE_DEVICE_ENTRY       *Current;
} HDD_DLG_CTX;

//===============================================NVME Sanitize CMD Data Structure==========================================
/**
 * The following Struct about NVMe Sanitize Reference to NVMe-NVM-Express-2.0 Base Spec.
*/
typedef enum {
	NvmeSanitizeExitFailure = 1,
	NvmeSanitizeBlockErase,
	NvmeSanitizeOverwrite,
	NvmeSanitizeCryptoErase,
}NMVE_SANITIZE_ACTION;

typedef enum {
	NvmeCsiNVM	= 0,
	NvmeCsiKV		= 1,
	NvmeCsiZNS	= 2,
}NVME_CSI;

typedef enum {
  NoneSanitize       = 0,
  Sanitizecompletion = 1,
  SanitizeInProgress = 2,
  SanitizeFailed     = 3,
  SanitizeWithNodas  = 6,
}SANITIZE_STATUS;

typedef  struct  {
	UINT32         Sanact;         //Sanitize action: 1 = Exit failure mode, 2 = Start block erase, 3 = Start overwrite, 4 = Start crypto erase
	UINT32         Ovrpat;         //Overwrite pattern
	BOOLEAN        Ause;           //Allow unrestricted sanitize exit
	UINT8          Owpass;         //Overwrite pass count.
	BOOLEAN        Oipbp;          //Overwrite invert pattern between passes.
	BOOLEAN        Nodas;          //No deallocate after sanitize.
} NVME_SANITIZE_OPERATION_CMD;

typedef  struct  {
	UINT64         Lpo;            //Log page offset for partial log transfers
	UINT32         Lid;            //Log page identifier, see &enum nvme_cmd_get_log_lid for known values
	UINT32         Len;            //Length of provided user buffer to hold the log data in bytes
	UINT32         Nsid;           //Namespace identifier, if applicable
	UINT32         Csi;            //Command set identifier, see &enum nvme_csi for known values
	UINT16         Lsi;            //Log Specific Identifier
	UINT8          Lsp;            //Log specific field
	UINT8          Uuidx;          //UUID selection, if supported
	BOOLEAN        Rae;            //Retain asynchronous events
	BOOLEAN        Ot;             //Offset Type; if set @lpo specifies the index into the list
} NVME_GET_SINITIZE_LOG_CMD;

typedef  struct  {
  UINT32         Reserved;
	UINT32         LbaFu;          //This field specifies the most significant 2 bits of the Format Index of the User Data Format to apply to the NVM media
	UINT32         Ses;            //This field specifies whether a secure erase should be performed as part of the format and the type of the secure erase operation
	UINT32         Pil;            //Protection Information Location
	UINT32         Pi;             //Namespace identifier, if applicable
	UINT32         Mset;           //Metadata Settings
	UINT32         LbaFl;          //This field specifies the least significant 4 bits of the Format Index of the User Data Format to apply to the NVM media
} NVME_FORMAT_NVM_CMD;
//=============================================================================================================================

extern HDD_DLG_CTX                   gHddDlgCtx;
extern UINT8                         HddSanitizationCfgBin[];
//
//Declare the strings for the progress bar prompt
//
extern LIST_ENTRY                     mHddSanitizationCfgFormList;
extern HDP_HII_VENDOR_DEVICE_PATH     mHddSanitizationHiiVendorDevicePath;
extern HDD_SANITIZE_DXE_PRIVATE_DATA  *gPrivate;
extern UINTN                          mNumberOfHddDevices;
extern CHAR16                         *gSanitizeTitleStr;
extern CHAR16                         *gSanitizeWarnigStr;
extern CHAR16                         *gSanitizeNoPowerOffStr[2];
extern EFI_EVENT                      HddSanitizeProgressEvent;

VOID
NvmeSscpHookForHddSanitize (
  VOID  *Param
  );

VOID
SataPassThruHookForHddSanitize (
  VOID  *Param
  );


/**
 * @brief Get NVMe Controller Identify data with opcode NVME_ADMIN_IDENTIFY_CMD
 *        These operations refer to NVM-Express-2.0a Spec.
 * 
 * @param[in] NvmePassThru                A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance.
 * @param[in] Buffer                      A pointer to the Controller Data Buffer.
 * 
*/
EFI_STATUS
NvmeGetIdentifyControllerData (
  IN     EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL *NvmePassThru,
  IN OUT VOID                               *Buffer
  );


/**
 * @brief start a sanitize operation via Sanitize command.
 *        All sanitize operations are processed in the background (i.e., completion of the Sanitize
          command does not indicate completion of the sanitize operation).
 * 
 * @param[in] NvmePassThru                A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance.
 * @param[in] SanitizeCmd        A pointer to the Get Log Page command.
 * 
*/
EFI_STATUS
NvmeStartSanitize (
  IN  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL *NvmePassThru,
  IN  NVME_SANITIZE_OPERATION_CMD        *SanitizeCmd
);

/**
 * @brief Get the status associated with the most recent sanitize operation about NVMe Device.
 *        These operations refer to NVM-Express-2.0a Spec.
 * 
 * @param[in] NvmePassThru                A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance.
 * @param[in] GetSanitizeLogCmd           A pointer to the Get Log Page command.
 * @param[in] SanitizeSatus               A pointer to the Sanitize Satus.
 * 
*/
EFI_STATUS
NvmeGetSanitizeStatus(
  IN   EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL *NvmePassThru,
  IN   NVME_GET_SINITIZE_LOG_CMD          *GetSanitizeLogCmd,
  OUT  UINTN                              *SanitizeSatus
  );

/**
 * @brief start a sanitize operation via Sanitize command.
 *        All sanitize operations (i.e., Block Erase, Crypto Erase, and Overwrite) are performed in the background 
 *         (i.e., Sanitize command completion does not indicate sanitize operation completion)
 *        These operations refer to NVM-Express-2.0a Spec.
 * 
 * @param[in] NvmePassThru                A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance.
 * @param[in] SanitizeMethodType          A pointer to the Sanitize Operation.
 * 
*/
EFI_STATUS
NvmeDeviceSanitize (
  IN  NVME_DEVICE_INFO     *NvmeInfo,
  IN  UINT8                *SanitizeMethodType
  );

/**
  Get a ATA device Security Status with the device Identify Data,and report to Hdd password DXE driver
**/
EFI_STATUS
GetAtaDevicePasswordSecurityStatus (
  IN     ATA_IDENTIFY_DATA    *IdentifyData,
  IN OUT HDD_SECURITY_INFO    *HddSecInfo
  );

/**
 * @brief  Erase a Unencrypted ATA device with BlockIo->WriteBlocks()
 * 
 * @param  AtaInfo   A pointer to the SATA device 
*/

EFI_STATUS
SataDevBlockIoErase(
  ATA_DEVICE_INFO      *AtaInfo
);

EFI_STATUS
SataDeviceSanitize (
  IN  ATA_DEVICE_INFO       *AtaInfo,
  IN  UINT8                 *SanitizeMethodType
  );

/**
 * @brief  Erase a Unencrypted NVMe device with BlockIo->WriteBlocks()
 * 
 * @param  BlkIo   A pointer to the EFI_BLOCK_IO_PROTOCOL instance
*/
EFI_STATUS
NvmeBlockIoErase (
  EFI_BLOCK_IO_PROTOCOL   *BlkIo
  );

#endif
