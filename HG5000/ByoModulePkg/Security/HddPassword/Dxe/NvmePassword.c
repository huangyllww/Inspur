/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  NvmePassword.c	

Abstract:
  
Revision History:

**/

#include "HddPasswordDxe.h"
#include <Library/TcgStorageOpalLib.h>
#include <Library/OpalPasswordSupportLib.h>
#include <Protocol/NvmeSscpHookProtocol.h>
#include <Library/TimerLib.h>
#include <Library/LogoLib.h>


#define NVME_PASSWORD_INFO_SIGN         SIGNATURE_32('_', 'N', 'P', 'I')
#define OPAL_MSID_LENGHT                128

typedef struct {
  UINT32                                 Sign;
  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL  *Sscp;
  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL     *NvmePT;
  UINT32                                 MediaId;
  UINT32                                 NamespaceId;
  UINT8                                  Sn[21];
  UINT8                                  Mn[41];

  UINT32                                          MsidLength;             // Byte length of MSID Pin for device
  UINT8                                           Msid[OPAL_MSID_LENGHT]; // MSID Pin for device
  UINT16                                          OpalBaseComId;          // Opal SSC 1 base com id.
//OPAL_OWNER_SHIP                                 Owner;
  OPAL_DISK_SUPPORT_ATTRIBUTE                     SupportedAttributes;
  TCG_LOCKING_FEATURE_DESCRIPTOR                  LockingFeature;         // Locking Feature Descriptor retrieved from performing a Level 0 Discovery
  OPAL_SESSION                                    Session;
  BOOLEAN                                         Enabled;
  BOOLEAN                                         Locked;
  BOOLEAN                                         SIDBlocked;
} NVME_PASSWD_INFO;


UINTN  gNvmeCount = 0;


UINT32 GetNvmeNamespaceIdFromNvmeInfo(VOID *pNvmeInfo)
{
  NVME_PASSWD_INFO    *NvmeInfo;
  
  NvmeInfo = (NVME_PASSWD_INFO*)pNvmeInfo;
  return NvmeInfo->NamespaceId;
}


EFI_STATUS
NvmeUpdateStatus (
    VOID  *pNvmeInfo
  )
{
  TCG_RESULT                  TcgResult;
  OPAL_SESSION                Session;
  NVME_PASSWD_INFO            *NvmeInfo;
  

  NvmeInfo = (NVME_PASSWD_INFO*)pNvmeInfo;

  ZeroMem(&Session, sizeof(Session));
  Session.Sscp = NvmeInfo->Sscp;
  Session.MediaId = NvmeInfo->MediaId;
  Session.OpalBaseComId = NvmeInfo->OpalBaseComId;

  TcgResult = OpalGetLockingInfo(&Session, &NvmeInfo->LockingFeature);
  if (TcgResult != TcgResultSuccess) {
    return EFI_DEVICE_ERROR;
  }

  if (NvmeInfo->MsidLength == 0) {
    return EFI_INVALID_PARAMETER;
  } else {
    //
    // Base on the Msid info to get the ownership, so Msid info must get first.
    //
//-    NvmeInfo->Owner = OpalUtilDetermineOwnership (
//-                        &Session, 
//-                        NvmeInfo->Msid, 
//-                        NvmeInfo->MsidLength
//-                        );
  }

  NvmeInfo->Enabled = OpalFeatureEnabled (
                       &NvmeInfo->SupportedAttributes, 
                       &NvmeInfo->LockingFeature
                       );
  if (NvmeInfo->Enabled) {
    NvmeInfo->Locked = OpalDeviceLocked (
                         &NvmeInfo->SupportedAttributes, 
                         &NvmeInfo->LockingFeature
                         );
  }

  return EFI_SUCCESS;
}





EFI_STATUS
NvmeUnlock (
  NVME_PASSWD_INFO   *NvmeInfo,
  VOID               *Password,
  UINT32             PassLength 
  )
{
  TCG_RESULT                   Ret;
  EFI_STATUS                   Status;


  ZeroMem(&NvmeInfo->Session, sizeof(NvmeInfo->Session));
  NvmeInfo->Session.Sscp = NvmeInfo->Sscp;
  NvmeInfo->Session.MediaId = NvmeInfo->MediaId;
  NvmeInfo->Session.OpalBaseComId = NvmeInfo->OpalBaseComId;

  Ret = OpalSupportUnlock(&NvmeInfo->Session, Password, PassLength, NULL);
  if (Ret == TcgResultSuccess) {
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}



EFI_STATUS
NvmeLock (
  NVME_PASSWD_INFO   *NvmeInfo,
  VOID               *Password,
  UINT32             PassLength 
  )
{
  TCG_RESULT                   Ret;
  EFI_STATUS                   Status;


  ZeroMem(&NvmeInfo->Session, sizeof(NvmeInfo->Session));
  NvmeInfo->Session.Sscp = NvmeInfo->Sscp;
  NvmeInfo->Session.MediaId = NvmeInfo->MediaId;
  NvmeInfo->Session.OpalBaseComId = NvmeInfo->OpalBaseComId;

  Ret = OpalSupportLock(&NvmeInfo->Session, Password, PassLength, NULL);
  if (Ret == TcgResultSuccess) {
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}




EFI_STATUS
NvmeDisableUser (
  NVME_PASSWD_INFO   *NvmeInfo,
  VOID               *Password,
  UINT32             PassLength 
  )
{
  BOOLEAN                      PasswordFailed;
  TCG_RESULT                   Ret;
  EFI_STATUS                   Status;


  ZeroMem(&NvmeInfo->Session, sizeof(NvmeInfo->Session));
  NvmeInfo->Session.Sscp = NvmeInfo->Sscp;
  NvmeInfo->Session.MediaId = NvmeInfo->MediaId;
  NvmeInfo->Session.OpalBaseComId = NvmeInfo->OpalBaseComId;

  Ret = OpalSupportDisableUser(
          &NvmeInfo->Session, 
          Password, 
          PassLength, 
          &PasswordFailed, 
          NULL
          );
  if (Ret == TcgResultSuccess && !PasswordFailed) {
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}



EFI_STATUS
NvmeOpalBlockSid (
  VOID               *pNvmeInfo
  )
{
  NVME_PASSWD_INFO             *NvmeInfo;
  TCG_RESULT                   Result;
  EFI_STATUS                   Status = EFI_UNSUPPORTED;
  

  NvmeInfo = (NVME_PASSWD_INFO*)pNvmeInfo;

  if(NvmeInfo->SupportedAttributes.BlockSid){
    ZeroMem(&NvmeInfo->Session, sizeof(NvmeInfo->Session));
    NvmeInfo->Session.Sscp = NvmeInfo->Sscp;
    NvmeInfo->Session.MediaId = NvmeInfo->MediaId;
    NvmeInfo->Session.OpalBaseComId = NvmeInfo->OpalBaseComId;

    Result = OpalBlockSid (&NvmeInfo->Session, FALSE);
    if (Result != TcgResultSuccess) {
      Status = EFI_ABORTED;
    } else {
      Status = EFI_SUCCESS;
    }
  }

  DEBUG((EFI_D_INFO, "%a %r\n", __FUNCTION__, Status));

  return Status;
}




EFI_STATUS
NvmeDisableAdmin (
  VOID               *pNvmeInfo,
  VOID               *Password,
  UINT32             PassLength  
  )
{
  EFI_STATUS                   Status;
  BOOLEAN                      KeepUserData = TRUE;
  BOOLEAN                      PasswordFailed;
  TCG_RESULT                   Ret;
  NVME_PASSWD_INFO             *NvmeInfo;


  NvmeInfo = (NVME_PASSWD_INFO*)pNvmeInfo;

  if (NvmeInfo->SupportedAttributes.PyriteSsc == 1 && 
      NvmeInfo->LockingFeature.MediaEncryption == 0) {
    //
    // For pyrite type device which also not supports media encryption, it not accept "Keep User Data" parameter.
    // So here hardcode a FALSE for this case.
    //
    KeepUserData = FALSE;
  }

  ZeroMem(&NvmeInfo->Session, sizeof(NvmeInfo->Session));
  NvmeInfo->Session.Sscp = NvmeInfo->Sscp;
  NvmeInfo->Session.MediaId = NvmeInfo->MediaId;
  NvmeInfo->Session.OpalBaseComId = NvmeInfo->OpalBaseComId;

  DEBUG((EFI_D_INFO, "%a KeepUserData:%d OpalBaseComId:%X\n", __FUNCTION__, KeepUserData, NvmeInfo->OpalBaseComId));

  Ret = OpalSupportRevert (
                      &NvmeInfo->Session,
                      KeepUserData,
                      Password,
                      PassLength,
                      NvmeInfo->Msid,
                      NvmeInfo->MsidLength,
                      &PasswordFailed,
                      NULL
                      );
  DEBUG((EFI_D_INFO, "OpalSupportRevert Ret:%d(PasswordFailed:%d)\n", Ret, PasswordFailed));  
  if (Ret == TcgResultSuccess && !PasswordFailed) {
    Status = EFI_SUCCESS;
  } else {
    Status =EFI_DEVICE_ERROR;
  }

  return Status;
}


BOOLEAN
NvmePasswordVerify (
  VOID               *pNvmeInfo,
  BOOLEAN            IsAdmin,
  VOID               *Password,
  UINT32             PassLength
)
{
  NVME_PASSWD_INFO   *NvmeInfo;
  TCG_RESULT         Ret;
  
  NvmeInfo = (NVME_PASSWD_INFO*)pNvmeInfo;

  Ret = OpalUtilVerifyPassword (
          &NvmeInfo->Session, 
          Password, 
          PassLength, 
          IsAdmin ? OPAL_LOCKING_SP_ADMIN1_AUTHORITY : OPAL_LOCKING_SP_USER1_AUTHORITY
          );
  DEBUG((EFI_D_INFO, "OpalUtilVerifyPassword(%d):%d\n", IsAdmin, Ret));
  if(Ret == TcgResultSuccess){
    return TRUE;
  } else {
    return FALSE;
  }  
}




EFI_STATUS
NvmeSetPassword (
  VOID               *pNvmeInfo,
  BOOLEAN            IsAdmin,
  BOOLEAN            NeedVerify,
  VOID               *OldPassword,
  UINT32             OldPassLength,
  VOID               *Password,
  UINT32             PassLength
  )
{
  TCG_RESULT         Ret;
  EFI_STATUS         Status;
  NVME_PASSWD_INFO   *NvmeInfo;


  DEBUG((EFI_D_INFO, "%a(%d %X %X %X %X)\n", __FUNCTION__, IsAdmin, OldPassword, OldPassLength, Password, PassLength));
  DumpMem8(Password, PassLength);

  NvmeInfo = (NVME_PASSWD_INFO*)pNvmeInfo;

  ZeroMem(&NvmeInfo->Session, sizeof(NvmeInfo->Session));
  NvmeInfo->Session.Sscp = NvmeInfo->Sscp;
  NvmeInfo->Session.MediaId = NvmeInfo->MediaId;
  NvmeInfo->Session.OpalBaseComId = NvmeInfo->OpalBaseComId;

  DumpMem8(NvmeInfo->Msid, NvmeInfo->MsidLength);

  if (!NvmeInfo->LockingFeature.LockingEnabled && IsAdmin) {

    if(Password == NULL || PassLength == 0){
      Status = EFI_INVALID_PARAMETER;
      goto ProcExit;
    }
    
    Ret = OpalSupportEnableOpalFeature (
            &NvmeInfo->Session, 
            NvmeInfo->Msid, 
            NvmeInfo->MsidLength,
            Password, 
            PassLength, 
            NULL
            );
    if(Ret != TcgResultSuccess){
      DEBUG((EFI_D_ERROR, "OpalSupportEnableOpalFeature:%d\n", Ret));
      Status = EFI_DEVICE_ERROR;
    } else {
      Status = EFI_SUCCESS;
    }    
    goto ProcExit;
  }

  if(OldPassword == NULL || OldPassLength == 0){
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  if(NeedVerify){
    Ret = OpalUtilVerifyPassword (
            &NvmeInfo->Session, 
            OldPassword, 
            OldPassLength, 
            OPAL_LOCKING_SP_ADMIN1_AUTHORITY
            );
    if(Ret != TcgResultSuccess){
      Ret = OpalUtilVerifyPassword (
              &NvmeInfo->Session, 
              OldPassword, 
              OldPassLength, 
              OPAL_LOCKING_SP_USER1_AUTHORITY
              );
    }
    if(Ret != TcgResultSuccess){
      DEBUG((EFI_D_ERROR, "OpalUtilVerifyPassword:%d\n", Ret));
      Status = EFI_INVALID_PARAMETER;
      goto ProcExit;
    }
  }

  if(Password == NULL || PassLength == 0){
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  Ret = OpalSupportSetPassword (
          &NvmeInfo->Session,
          OldPassword,
          OldPassLength,
          Password,
          PassLength,
          NULL,
          IsAdmin
          );
  if(Ret != TcgResultSuccess){
    DEBUG((EFI_D_ERROR, "OpalSupportSetPassword:%d\n", Ret));
    Status = EFI_DEVICE_ERROR;
  } else {
    Status = EFI_SUCCESS;
  }

ProcExit:
  return Status;
}


VOID 
SyncNvmeInfoToIfr (
  VOID                             *pNvmeInfo,
  HDD_PASSWORD_CONFIG              *IfrData
  )
{
  NVME_PASSWD_INFO   *NvmeInfo;

  NvmeInfo = (NVME_PASSWD_INFO*)pNvmeInfo;

  IfrData->Enabled = NvmeInfo->Enabled;
  IfrData->Locked  = NvmeInfo->Locked;
  IfrData->Frozen  = NvmeInfo->SIDBlocked;
}


VOID
NvmeSscpHook (
  VOID  *Param   
  )
{
  NVME_SSCP_HOOK_PARAMETER         *p;
  EFI_STATUS                       Status;
  LIST_ENTRY                       *Entry;
  HDD_PASSWORD_CONFIG_FORM_ENTRY   *ConfigFormEntry;
  BOOLEAN                          EntryExisted;
  HDD_PASSWORD_INFO                *HdpInfo;
  NVME_PASSWD_INFO                 *NvmeInfo;
  UINTN                            Index;
  UINTN                            SegNum;
  UINTN                            BusNum;
  UINTN                            DevNum;
  UINTN                            FuncNum;
  HDD_PASSWORD_CONFIG              *IfrData;  
  TCG_RESULT                       TcgResult;
  CHAR16                           Password[32 + 1];
  UINT8                            PasswordOut[32];
  CHAR16                           *StrTitle;
  CHAR16                           *StrTryAgain;
  CHAR16                           *StrCancel;
  CHAR16                           *EnterCurPwStr;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut;
  EFI_SIMPLE_TEXT_OUTPUT_MODE       SavedConsoleMode;
  BOOLEAN                           Rc;
  EFI_HANDLE                        Handle = NULL;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  p = (NVME_SSCP_HOOK_PARAMETER*)Param;
  ASSERT(p->Sign == NVME_SSCP_HOOK_PARAMETER_SIGN);

  EntryExisted = FALSE;
  EFI_LIST_FOR_EACH (Entry, &mHddPasswordConfigFormList) {
    ConfigFormEntry = BASE_CR (Entry, HDD_PASSWORD_CONFIG_FORM_ENTRY, Link);
    HdpInfo = &ConfigFormEntry->HdpInfo;
    if (HdpInfo->Controller == p->Controller) {
      EntryExisted = TRUE;
      break;
    }
  }
  if (EntryExisted) {
    DEBUG((EFI_D_INFO, "exist\n"));
    goto ProcExit;
  }

  if(gHdpDlgCtx.BdsBootMgr == NULL){
    Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, (VOID**)&gHdpDlgCtx.BdsBootMgr);
    if (EFI_ERROR (Status)) {
      DEBUG((EFI_D_ERROR, "%a (L%d) %r\n", __FUNCTION__, __LINE__, Status));
      goto ProcExit;
    }
  }

  NvmeInfo = AllocateZeroPool(sizeof(NVME_PASSWD_INFO));
  if(NvmeInfo == NULL){
    DEBUG((EFI_D_ERROR, "(L%d) malloc error\n", __LINE__));
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }
  NvmeInfo->Sign   = NVME_PASSWORD_INFO_SIGN;
  NvmeInfo->NvmePT = p->Passthru;
  NvmeInfo->Sscp   = p->Sscp; 
  NvmeInfo->MediaId = p->BlockIo->Media->MediaId;
  NvmeInfo->NamespaceId = p->NamespaceId;
  CopyMem(NvmeInfo->Sn, p->Sn, sizeof(p->Sn));
  CopyMem(NvmeInfo->Mn, p->Mn, sizeof(p->Mn));  

  ZeroMem(&NvmeInfo->Session, sizeof(NvmeInfo->Session));
  NvmeInfo->Session.Sscp    = NvmeInfo->Sscp;
  NvmeInfo->Session.MediaId = NvmeInfo->MediaId;

  TcgResult = OpalGetSupportedAttributesInfo (
                &NvmeInfo->Session, 
                &NvmeInfo->SupportedAttributes, 
                &NvmeInfo->OpalBaseComId
                );
  if (TcgResult != TcgResultSuccess) {
    DEBUG((EFI_D_ERROR, "%a (L%d) %d\n", __FUNCTION__, __LINE__, TcgResult));
    Status = EFI_DEVICE_ERROR;
    goto ProcExit;
  }
  NvmeInfo->Session.OpalBaseComId = NvmeInfo->OpalBaseComId;
  NvmeInfo->SIDBlocked = (BOOLEAN)NvmeInfo->SupportedAttributes.SIDBlockedState;

  TcgResult = OpalUtilGetMsid (
                &NvmeInfo->Session, 
                NvmeInfo->Msid, 
                OPAL_MSID_LENGHT, 
                &NvmeInfo->MsidLength
                );
  if (TcgResult != TcgResultSuccess) {
    DEBUG((EFI_D_ERROR, "%a (L%d) %d\n", __FUNCTION__, __LINE__, TcgResult));
    Status = EFI_DEVICE_ERROR;
    goto ProcExit;    
  }

  Status = NvmeUpdateStatus(NvmeInfo);
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "%a (L%d) %r\n", __FUNCTION__, __LINE__, Status));
    goto ProcExit;
  }

  ConfigFormEntry = AllocateZeroPool(sizeof(HDD_PASSWORD_CONFIG_FORM_ENTRY));
  ASSERT(ConfigFormEntry != NULL);
  if (ConfigFormEntry == NULL) {
    DEBUG((EFI_D_ERROR, "(L%d) malloc error\n", __LINE__));
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }

  ConfigFormEntry->NvmeInfo = (VOID*)NvmeInfo;

  HdpInfo = &ConfigFormEntry->HdpInfo;
  IfrData = &ConfigFormEntry->IfrData;  
  InitializeListHead(&ConfigFormEntry->Link);
  HdpInfo->Controller = p->Controller;

  Index = gNvmeCount++;

  HdpInfo->HddIndex    = 0x80 + (UINT8)Index;
  HdpInfo->Port        = HdpInfo->HddIndex;
  HdpInfo->PortMp      = 0xFFFF;
  HdpInfo->AtaPassThru = (EFI_ATA_PASS_THRU_PROTOCOL*)NvmeInfo;
  CopyMem(HdpInfo->SerialNo, NvmeInfo->Sn, sizeof(HdpInfo->SerialNo));

  SyncNvmeInfoToIfr(NvmeInfo, IfrData);

  Status = p->PciIo->GetLocation (
                       p->PciIo,
                       &SegNum,
                       &BusNum,
                       &DevNum,
                       &FuncNum
                       );
  ASSERT_EFI_ERROR(Status);  
  DEBUG((EFI_D_INFO, "(%X,%X,%X)\n", BusNum, DevNum, FuncNum));

  HdpInfo->Bus    = (UINT8)BusNum;
  HdpInfo->Dev    = (UINT8)DevNum;  
  HdpInfo->Func   = (UINT8)FuncNum;
  HdpInfo->PciIo  = p->PciIo;
  HdpInfo->PciScc = 0x08;
  TrimStr8(NvmeInfo->Mn);

  UnicodeSPrint(
    ConfigFormEntry->HddString, 
    sizeof(ConfigFormEntry->HddString), 
    L"NVME(PCI%d-%d-%d): %a",
    BusNum,
    DevNum,
    FuncNum,
    NvmeInfo->Mn
    ); 


  ConfigFormEntry->TitleToken = HiiSetString(gPrivate->HiiHandle, 0, ConfigFormEntry->HddString, NULL);
  InsertTailList (&mHddPasswordConfigFormList, &ConfigFormEntry->Link);
  mNumberOfHddDevices++;

  DEBUG((EFI_D_INFO, "Port:%X HddString:%s En:%d Lock:%d Exp:%d Frozen:%d Count:%d\n", \
    HdpInfo->HddIndex, ConfigFormEntry->HddString, IfrData->Enabled, IfrData->Locked, IfrData->CountExpired, \
    IfrData->Frozen, mNumberOfHddDevices));



  if(!IfrData->Enabled){
    goto ProcExit;
  }

//
// now hdp must be enabled.
//
    // install this to disable FRB2 watch dog
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiFrb2WatchDogNotifyGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  
  StopWarningPrompt((VOID*)PcdGet64(PcdWaringPrintEvent));
  StrTitle = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(HP_STR_OP_FAIL), NULL);
  StrTryAgain = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_WRONG_PWD_TRY_AGAIN), NULL); 
  StrCancel = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_HDP_CANCEL), NULL); 
  EnterCurPwStr = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_ENTER_HDP), NULL);

  ConOut = gST->ConOut;
  CopyMem(&SavedConsoleMode, ConOut->Mode, sizeof(SavedConsoleMode));
  BltSaveAndRetore(gBS, TRUE);
  ConOut->ClearScreen(ConOut);


  HotKeySaveAndRestore(TRUE);

	while(1){

    if(CheckAndHandleRetryCountOut(IfrData)){
      break;
    }
    
    Status = gHdpDlgCtx.BdsBootMgr->DrawPostPwdDlg(ConfigFormEntry->HddString, EnterCurPwStr, Password, ARRAY_SIZE(Password));
    ConOut->SetAttribute(ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
    ConOut->EnableCursor(ConOut, FALSE);
    ConOut->ClearScreen(ConOut);
    if(!EFI_ERROR(Status)){
      
      BuildHdpFromUserInput(Password, NvmeInfo->Sn, PasswordOut, NULL);

      if(IfrData->Locked){
        Status = NvmeUnlock(NvmeInfo, PasswordOut, sizeof(PasswordOut));
        DEBUG((EFI_D_INFO, "UnlockHdd:%r\n", Status));
      } else {
        Rc = NvmePasswordVerify(NvmeInfo, 1, PasswordOut, sizeof(PasswordOut));
        if(!Rc){
          Rc = NvmePasswordVerify(NvmeInfo, 0, PasswordOut, sizeof(PasswordOut));
        }
        if(Rc){
          Status = EFI_SUCCESS;
        } else {
          Status = EFI_ABORTED;
        }
      }
      if(!EFI_ERROR(Status)){
        Status = NvmeUpdateStatus(NvmeInfo);
        ASSERT(!EFI_ERROR (Status));
        SyncNvmeInfoToIfr(NvmeInfo, IfrData);
        DEBUG((EFI_D_INFO, "En:%d Lock:%d\n", IfrData->Enabled, IfrData->Locked));
        if (!IfrData->Locked) {
          IfrData->RetryCount = 0;
  				break;
        }
      }
      IfrData->RetryCount++;
      
    }else {     // ESC
      if(!IfrData->Locked){
        PromptNoPasswordError();
      }
      UiConfirmDialog(DIALOG_INFO, StrTitle, NULL, TEXT_ALIGIN_CENTER, StrCancel, NULL);
      break;
    }

    if(IfrData->RetryCount < 3){
      UiConfirmDialog(DIALOG_INFO, StrTitle, NULL, TEXT_ALIGIN_CENTER, StrTryAgain, NULL);
      continue;
    }
	}

  HotKeySaveAndRestore(FALSE);

  Status = ConOut->SetCursorPosition(ConOut, SavedConsoleMode.CursorColumn, SavedConsoleMode.CursorRow);
  Status = ConOut->SetAttribute(ConOut, SavedConsoleMode.Attribute);
  Status = ConOut->EnableCursor(ConOut, SavedConsoleMode.CursorVisible);
  BltSaveAndRetore(gBS, FALSE); 
  if(!PcdGet8(PcdLegacyBiosSupport)){
    if(IsEnglishLang()){
      StartWarningPrompt (L"Driver Loading");
    }else{
      StartWarningPrompt (L"Çý¶¯¼ÓÔØÖÐ");
    }
  }


  FreePool(StrTitle);
  FreePool(StrTryAgain);
  FreePool(StrCancel); 
  FreePool(EnterCurPwStr);

  DEBUG((EFI_D_INFO, "En:%d Lock:%d\n", IfrData->Enabled, IfrData->Locked));
  
	ZeroMem(Password, sizeof(Password));
	ZeroMem(PasswordOut, sizeof(PasswordOut));    

ProcExit:
  return;
}



