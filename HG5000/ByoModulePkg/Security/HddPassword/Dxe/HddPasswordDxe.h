/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR> 
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced, 
stored in a retrieval system, or transmitted in any form or by any 
means without the express written consent of Byosoft Corporation.

File Name:
  HddPasswordDxe.h

Abstract: 
  Hdd password DXE driver.

Revision History:

**/



#ifndef _HDD_PASSWORD_DXE_H_
#define _HDD_PASSWORD_DXE_H_
//-----------------------------------------------------------------------

#include <Uefi.h>
#include <IndustryStandard/Atapi.h>
#include <Protocol/AtaPassThru.h>
#include <Protocol/PciIo.h>
#include <Protocol/HiiConfigAccess.h>
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
#include "HddPasswordData.h"
#include <Protocol/SetupSaveNotify.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/CryptoLibDxeProtocol.h>
#include <Library/ByoCommLib.h>
#include <Library/SetupUiLib.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <Token.h>


extern UINT8  HddPasswordBin[];
extern UINT8  HddPasswordDxeStrings[];

#define HDD_PASSWORD_DXE_PRIVATE_SIGNATURE SIGNATURE_32 ('H', 'D', 'D', 'P')

#define ATA_CMD_RETRY_MAX_COUNT          3


typedef struct {
  EFI_ATA_PASS_THRU_PROTOCOL *AtaPassThru;
  EFI_HANDLE                 Controller;
  EFI_ATA_STATUS_BLOCK       *Asb;
  VOID                       *AsbAlloc;  
  ATA_IDENTIFY_DATA          *Identify; 
  VOID                       *IdentifyAlloc;
  UINT16                     *PayLoad;
  VOID                       *PayLoadAlloc;
  UINTN                      PayLoadSize;
  UINT32                     IoAlign;
  EFI_PCI_IO_PROTOCOL        *PciIo;
  UINT8                      Bus;
  UINT8                      Dev;
  UINT8                      Func;
  UINT8                      PciScc;
  UINT16                     Port;
  UINT16                     PortMp;
  UINT16                     HddIndex;
  BOOLEAN                    Present;
  BOOLEAN                    Enabled;
  BOOLEAN                    Locked;
  UINT8                      SerialNo[20];
  UINT8                      PasswordType;
} HDD_PASSWORD_INFO;

typedef struct {
  LIST_ENTRY                    Link;
  HDD_PASSWORD_INFO             HdpInfo;
  VOID                          *NvmeInfo;  
  CHAR16                        HddString[64];
  EFI_STRING_ID                 TitleToken;
  EFI_STRING_ID                 TitleHelpToken;
  HDD_PASSWORD_CONFIG           IfrData;
	UINT8                         InputUserPassword[32+1];
} HDD_PASSWORD_CONFIG_FORM_ENTRY;

typedef struct {
  UINTN                            Signature;
  EFI_HANDLE                       DriverHandle;
  EFI_HII_HANDLE                   HiiHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL   ConfigAccess;
  HDD_PASSWORD_CONFIG_FORM_ENTRY   *Current;
  SETUP_SAVE_NOTIFY_PROTOCOL       SetupSaveNotify;
  CRYPTO_LIB_PROTOCOL              *CryptoLib;
  HDD_PASSWORD_CONFIG_FORM_ENTRY   Dummy;
} HDD_PASSWORD_DXE_PRIVATE_DATA;

#define HDD_PASSWORD_DXE_PRIVATE_FROM_THIS_HII(a)  \
  CR(a, HDD_PASSWORD_DXE_PRIVATE_DATA, ConfigAccess, HDD_PASSWORD_DXE_PRIVATE_SIGNATURE)

#define HDD_PASSWORD_DXE_PRIVATE_FROM_THIS_SSN(a)  \
  CR(a, HDD_PASSWORD_DXE_PRIVATE_DATA, SetupSaveNotify, HDD_PASSWORD_DXE_PRIVATE_SIGNATURE)

//
//Iterate through the doule linked list. NOT delete safe
//
#define EFI_LIST_FOR_EACH(Entry, ListHead)    \
  for(Entry = (ListHead)->ForwardLink; Entry != (ListHead); Entry = Entry->ForwardLink)

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
// Time out value for ATA pass through protocol
//
#define ATA_TIMEOUT        EFI_TIMER_PERIOD_SECONDS (3)

#define MASTER_PASSWORD_DEFAULT_REVCODE       0xFFFE
#define MASTER_PASSWORD_USER_REVCODE          0x0001
#define MASTER_PASSWORD_USER_MASTER_REVCODE   0x0002


extern HDP_HII_VENDOR_DEVICE_PATH  mHddPasswordHiiVendorDevicePath;
extern LIST_ENTRY mHddPasswordConfigFormList;
extern HDD_PASSWORD_DXE_PRIVATE_DATA  *gPrivate;
extern UINTN      mNumberOfHddDevices;


EFI_STATUS 
NvSaveValue(  
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS 
NvDiscardValue(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS 
NvLoadDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS 
NvSaveUserDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS 
NvLoadUserDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS 
IsNvDataChanged(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This,
  BOOLEAN                       *IsDataChanged
  );

EFI_STATUS 
NvSetMfgDefault (
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );


EFI_STATUS
SecurityFrozenLock (
    IN  HDD_PASSWORD_INFO       *HdpInfo
  );

EFI_STATUS
SecurityDisableHddPassword (
  IN  HDD_PASSWORD_INFO             *HdpInfo,
  IN  UINT8                         Identifier,
  IN  UINT8                         *Password,
  IN  UINTN                         PasswordSize 
  );

EFI_STATUS
SecuritySetHddPassword (
  IN  HDD_PASSWORD_INFO             *HdpInfo,
  IN  UINT8                         Identifier,         // 0, 1
  IN  UINT8                         SecurityLevel,      // 0 = High
  IN  UINT16                        RevCode, 
  IN  UINT8                         *Password,
  IN  UINTN                         PasswordSize 
  );

EFI_STATUS
SecurityUnlockHdd (
  IN  HDD_PASSWORD_INFO             *HdpInfo,
  IN  BOOLEAN                       IsUserPassword,
  IN  VOID                          *Password,
  IN  UINTN                         PasswordSize
  );

EFI_STATUS
GetHddDeviceIdentifyData (
  IN HDD_PASSWORD_INFO  *HdpInfo
  );

EFI_STATUS
GetHddDeviceModelNumber (
  IN ATA_IDENTIFY_DATA             *IdentifyData,
  IN OUT CHAR16                    *String
  );

HDD_PASSWORD_CONFIG_FORM_ENTRY *
HddPasswordGetConfigFormEntryByIndex (
  IN UINTN Index
  );

EFI_STATUS
GetHddPasswordSecurityStatus (
  IN     ATA_IDENTIFY_DATA    *IdentifyData,
  IN OUT HDD_PASSWORD_CONFIG  *IfrData
  );

VOID *AllocAtaBuffer (
  EFI_ATA_PASS_THRU_PROTOCOL    *AtaPassThru,
  UINTN                         BufferSize,
  VOID                          **Alloc
  );


VOID
NvmeSscpHook (
  VOID  *Param   
  );

EFI_STATUS 
BuildHdpFromUserInput (
  CHAR16   *PasswordStr,
  UINT8    *Sn,
  UINT8    *Password,
  UINT8    *CmosData  OPTIONAL
  );

EFI_STATUS 
UpdateHddSecurityStatus (
  HDD_PASSWORD_INFO    *HdpInfo,
  HDD_PASSWORD_CONFIG  *IfrData
  );

BOOLEAN CheckAndHandleRetryCountOut(HDD_PASSWORD_CONFIG *IfrData);
VOID PromptNoPasswordError();

EFI_STATUS
NvmeSetPassword (
  VOID               *pNvmeInfo,
  BOOLEAN            IsAdmin,
  BOOLEAN            NeedVerify,  
  VOID               *OldPassword,
  UINT32             OldPassLength,
  VOID               *Password,
  UINT32             PassLength
  );

EFI_STATUS
NvmeUpdateStatus (
    VOID  *pNvmeInfo
  );

UINT32 GetNvmeNamespaceIdFromNvmeInfo(VOID *pNvmeInfo);

VOID 
SyncNvmeInfoToIfr(
  VOID                             *pNvmeInfo,
  HDD_PASSWORD_CONFIG              *IfrData
  );

EFI_STATUS
NvmeDisableAdmin (
  VOID               *pNvmeInfo,
  VOID               *Password,
  UINT32             PassLength  
  );

BOOLEAN
NvmePasswordVerify (
  VOID               *pNvmeInfo,
  BOOLEAN            IsAdmin,
  VOID               *Password,
  UINT32             PassLength
  );


VOID HotKeySaveAndRestore(BOOLEAN Save);

EFI_STATUS
NvmeOpalBlockSid (
  VOID               *pNvmeInfo
  );




typedef struct {
  BOOLEAN                         Init;
  EFI_QUESTION_ID                 CurQid;
  EFI_HII_HANDLE                  HiiHandle;
  HDD_PASSWORD_CONFIG_FORM_ENTRY  *Current;
  BOOLEAN                         IsUserQ;
  UINT8                           OldPassword[32];
  EFI_BDS_BOOT_MANAGER_PROTOCOL   *BdsBootMgr;
} HDP_DLG_CTX;

extern HDP_DLG_CTX gHdpDlgCtx;


//-----------------------------------------------------------------------
#endif


