/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR> 
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced, 
stored in a retrieval system, or transmitted in any form or by any 
means without the express written consent of Byosoft Corporation.

File Name:
  HddPasswordDxe.c

Abstract: 
  Hdd password DXE driver.

Revision History:

**/




#include "HddPasswordDxe.h"
#include <Protocol/AtaPassThruHookParamter.h>
#include <Pi/PiBootMode.h>
#include <Library/HobLib.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <Library/LogoLib.h>
#include <Protocol/PlatHostInfoProtocol.h>



EFI_GUID   mHddPasswordVendorGuid          = HDD_PASSWORD_CONFIG_GUID;
CHAR16     mHddPasswordVendorStorageName[] = HDD_PASSWORD_VAR_NAME;
LIST_ENTRY mHddPasswordConfigFormList;
UINTN      mNumberOfHddDevices = 0;
HDD_PASSWORD_DXE_PRIVATE_DATA  *gPrivate;



//-----------------------------------------------------------------
EFI_STATUS
EFIAPI
HdpFormExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                       Status;
  EFI_STRING                       ConfigRequestHdr;
  EFI_STRING                       ConfigRequest;
  BOOLEAN                          AllocatedRequest;
  UINTN                            Size;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;
  if ((Request != NULL) && !HiiIsConfigHdrMatch(Request, &mHddPasswordVendorGuid, mHddPasswordVendorStorageName)){
    DEBUG((EFI_D_ERROR, "HiiIsConfigHdrMatch error\n"));
    return EFI_NOT_FOUND;
  }

  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  AllocatedRequest = FALSE;

  ConfigRequest = Request;
  if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request has no request element, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr(&mHddPasswordVendorGuid, mHddPasswordVendorStorageName, gPrivate->DriverHandle);
    Size = (StrLen(ConfigRequestHdr) + 32 + 1) * sizeof(CHAR16);
    ConfigRequest = AllocateZeroPool(Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    UnicodeSPrint(ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, sizeof(HDD_PASSWORD_CONFIG));
    FreePool (ConfigRequestHdr);
  }
  Status = gHiiConfigRouting->BlockToConfig (
                                gHiiConfigRouting,
                                ConfigRequest,
                                (UINT8*)&gPrivate->Current->IfrData,
                                sizeof(HDD_PASSWORD_CONFIG),
                                Results,
                                Progress
                                );
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "BlockToConfig:%r\n", Status));
  }
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }

  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen(Request);
  }
  
  return Status;
}




EFI_STATUS
EFIAPI
HdpFormRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check routing data in <ConfigHdr>.
  // Note: if only one Storage is used, then this checking could be skipped.
  //
  if(HiiIsConfigHdrMatch(Configuration, &mHddPasswordVendorGuid, mHddPasswordVendorStorageName)){
    *Progress = Configuration + StrLen(Configuration);
    return EFI_SUCCESS;
  }
  
  *Progress = Configuration;
  return EFI_NOT_FOUND;
}




HDP_DLG_CTX gHdpDlgCtx = {0, 0, NULL};




VOID HotKeySaveAndRestore(BOOLEAN Save)
{
  EFI_STATUS                     Status;
  EFI_BDS_BOOT_MANAGER_PROTOCOL  *BdsBootMgr;
  STATIC UINTN                   HotKey = 0;
  STATIC BOOLEAN                 KeySaved = FALSE;


  Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, &BdsBootMgr);
  if(EFI_ERROR(Status)){
    return;
  }

  if(Save){
    HotKey = BdsBootMgr->GetCurHotKey();
    KeySaved = TRUE;
  } else {
    if(KeySaved){
      BdsBootMgr->SetCurHotKey(HotKey);
      KeySaved = FALSE;
    }
  }
}



EFI_STATUS 
BuildHdpFromUserInput (
  CHAR16   *PasswordStr,
  UINT8    *Sn,
  UINT8    *Password,
  UINT8    *CmosData  OPTIONAL
  )
{
  UINT8   Hash[32];
  UINT8   Buffer[HDP_FIRST_HASH_SIZE + 20];
  UINT8   *p;
  UINTN   PassSize;
  CHAR8   *Salt;
  UINTN   SaltSize;
  UINTN   Len;

  
  ASSERT(PasswordStr != NULL && Sn != NULL);
  Len = StrSize(PasswordStr);
  ASSERT(Len <= 32*2+2 && Len != 2);

  Salt = HDP_SALT;
  SaltSize = AsciiStrLen(Salt);
  ASSERT(SaltSize != 0);

  PassSize = SaltSize + Len;
  p = AllocatePool(PassSize);
  ASSERT(p != NULL);

  CopyMem(p, Salt, SaltSize);
  CopyMem(p + SaltSize, PasswordStr, Len);

  gPrivate->CryptoLib->Sha256(p, PassSize, Hash);
  CopyMem(Buffer, Hash, HDP_FIRST_HASH_SIZE);
  
  CopyMem(Buffer+HDP_FIRST_HASH_SIZE, Sn, 20);
  gPrivate->CryptoLib->Sha256(Buffer, sizeof(Buffer), Hash);
  CopyMem(Password, Hash, sizeof(Hash));

  return EFI_SUCCESS;
}



VOID HdpHandleSuccess()
{
  CHAR16  *Str1;
  CHAR16  *Str2;

  Str1 = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_PW_NOTICE), NULL);
  Str2 = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_CHANGES_SAVED), NULL);  
  UiConfirmDialog(DIALOG_INFO, Str1, NULL, TEXT_ALIGIN_CENTER, Str2, NULL);
  FreePool(Str1);
  FreePool(Str2);
}

VOID HdpHandleErrorReason(UINTN Reason)
{
  CHAR16  *Str1;
  CHAR16  *Str2;
  CHAR16  *Str3;  

  Str1 = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_PW_WARNING), NULL);
  switch(Reason){
    case PASSWD_REASON_NOT_EQUAL:
      Str2 = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_PW_NOT_MATCH), NULL);
      Str3 = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_PW_ENTER_TO_CONTINUE), NULL);   
      UiConfirmDialog(DIALOG_INFO, Str1, NULL, TEXT_ALIGIN_CENTER, Str2, Str3, NULL);
      FreePool(Str2);
      FreePool(Str3);      
      break;

    case PASSWD_REASON_WRONG_OLD:
    case PASSWD_REASON_EMPTY:      
      Str2 = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_PW_INVALID_PW), NULL);
      UiConfirmDialog(DIALOG_INFO, Str1, NULL, TEXT_ALIGIN_CENTER, Str2, NULL);
      FreePool(Str2);      
      break;    
  }

  FreePool(Str1);
}


VOID HandleFrozen()
{
  CHAR16  *Str1;
  CHAR16  *Str2;

  Str1 = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_OP_FAIL), NULL);
  Str2 = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_HDD_FROZEN), NULL);  
  UiConfirmDialog(DIALOG_WARNING, Str1, NULL, TEXT_ALIGIN_CENTER, Str2, NULL);
  FreePool(Str1);
  FreePool(Str2);
}


BOOLEAN CheckAndHandleRetryCountOut(HDD_PASSWORD_CONFIG *IfrData)
{
  CHAR16                           *Title;
  CHAR16                           *Str1;
  

  if(IfrData->RetryCount > 3){
    if(!IfrData->Locked){
      PromptNoPasswordError();
    }
  }

  if(IfrData->CountExpired || IfrData->RetryCount >= 3){
    Title = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(HP_STR_OP_FAIL), NULL);
    Str1  = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_NEED_HDP), NULL);

    UiConfirmDialog(DIALOG_WARNING, Title, NULL, TEXT_ALIGIN_CENTER, Str1, NULL);

    FreePool(Title);
    FreePool(Str1);

    return TRUE;
  }

  return FALSE;
}


VOID PromptNoPasswordError()
{
  CHAR16                           *Title;
  CHAR16                           *Str1;
 

  Title = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(HP_STR_OP_FAIL), NULL);
  Str1 = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_NEED_HDP), NULL);

  UiConfirmDialog(DIALOG_INFO, Title, NULL, TEXT_ALIGIN_CENTER, Str1, NULL);
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);

  FreePool(Title);
  FreePool(Str1);

}






EFI_STATUS 
UpdateHddSecurityStatus (
  HDD_PASSWORD_INFO    *HdpInfo,
  HDD_PASSWORD_CONFIG  *IfrData
  )
{
  EFI_STATUS  Status;

  Status = GetHddDeviceIdentifyData(HdpInfo);
  if(!EFI_ERROR(Status)){
    GetHddPasswordSecurityStatus(HdpInfo->Identify, IfrData);
  }
  return Status;
}


EFI_STATUS
EFIAPI
HdpDlgCallBack (
    IN CHAR16  *Password
  )
{
  return EFI_SUCCESS;
}


  EFI_STATUS
  EFIAPI
  CheckPasswordForHdd(
	  IN CHAR16  *Password
  )
  {
  
	EFI_STATUS				 Status;
	CHAR16					 *LengthWarnStr; 
	CHAR16					 *SignWarnStr;
	UINTN					 Index;
	BOOLEAN 				 PasNum = FALSE;
	BOOLEAN 				 PasAbc = FALSE;
	CHAR16					 *Title;
	Status = EFI_SUCCESS;
   if((StrLen(Password)<8)||(StrLen(Password)>20)){
			Title = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_PW_WARNING), NULL);
			LengthWarnStr = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_PW_INVALID_LEN), NULL);
			UiConfirmDialog(DIALOG_INFO, Title, NULL, TEXT_ALIGIN_CENTER, LengthWarnStr, NULL);
			goto ProcExit;
  
		  }
  
	  for( Index=0;Index<StrLen(Password); Index++){
		  if((Password[Index]>=0x30)&&(Password[Index]<=0x39)){ 
			  PasNum = TRUE;
			  break;
			  }
		 }
  
	  if(PasNum == TRUE){
		  for( Index=0;Index<StrLen(Password); Index++){
			  if(((Password[Index]>=0x41)&&(Password[Index]<=0x5A)) || ((Password[Index]>=0x61)&&(Password[Index]<=0x7A))){ 
				  PasAbc = TRUE;
				  break;
			  }
		 }
	   }
	  else{
			Title = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_PW_WARNING), NULL);
			SignWarnStr = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_PW_INVALID_CHARA), NULL);
			UiConfirmDialog(DIALOG_INFO, Title, NULL, TEXT_ALIGIN_CENTER, SignWarnStr, NULL);
			goto ProcExit;
  
		  }
  
	  if(!((PasNum == TRUE)&&(PasAbc == TRUE))){
			Title = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_PW_WARNING), NULL);
			SignWarnStr = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_PW_INVALID_CHARA), NULL);
			UiConfirmDialog(DIALOG_INFO, Title, NULL, TEXT_ALIGIN_CENTER, SignWarnStr, NULL);
			goto ProcExit;
		  }
	  else
		  return EFI_SUCCESS;
	  
	  ProcExit:
		  return EFI_ABORTED;
		  
  }



EFI_STATUS HandlePasswordSet(BOOLEAN IsUser)
{
  EFI_STATUS               Status;
  UINTN                    Reason;
  HDD_PASSWORD_INFO        *HdpInfo;
  CHAR16                   Password[32+1];
  UINT8                    PasswordData[32];
  HDD_PASSWORD_CONFIG      *IfrData;
  BOOLEAN                  PasswdIsUser;
  CHAR16                   *Title;


  DEBUG((EFI_D_INFO, "%a(%d)\n", __FUNCTION__, IsUser));

  IfrData = &gHdpDlgCtx.Current->IfrData;
  HdpInfo = &gHdpDlgCtx.Current->HdpInfo;  
  gHdpDlgCtx.IsUserQ = IsUser;
  
  if(IfrData->Frozen){
    HandleFrozen();
    return EFI_ABORTED;
  }

  if(gHdpDlgCtx.CurQid == KEY_HDD_DISABLE_USER_PASSWORD){
    Title = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_SATA_INPUT_USER_PSW), NULL);
    Status = gHdpDlgCtx.BdsBootMgr->DrawPostPwdDlg(gHdpDlgCtx.Current->HddString, Title, Password, ARRAY_SIZE(Password));
    FreePool(Title);
    if(EFI_ERROR(Status)){                        // cancel
      goto ProcExit;
    }
    
    BuildHdpFromUserInput (
      Password, 
      gHdpDlgCtx.Current->HdpInfo.SerialNo,
      PasswordData,
      NULL
      );

    PasswdIsUser = TRUE;
    Status = SecurityUnlockHdd (
               &gHdpDlgCtx.Current->HdpInfo, 
               PasswdIsUser, 
               PasswordData,
               sizeof(PasswordData)
               );
    IfrData->RetryCount++;
    if(EFI_ERROR(Status)){
      Status = EFI_ABORTED;
      HdpHandleErrorReason(PASSWD_REASON_WRONG_OLD);
      goto ProcExit;
    }      

    IfrData->RetryCount = 0;

    Status = SecurityDisableHddPassword (
               &gHdpDlgCtx.Current->HdpInfo, 
               !PasswdIsUser,                           // user
               PasswordData,
               sizeof(PasswordData)
               );
    DEBUG((EFI_D_INFO, "DisHddPassword:%r\n", Status));
    UpdateHddSecurityStatus(HdpInfo, IfrData);
    if(!IfrData->Enabled){
      IfrData->ChangeMasterSupport = 0;
    }
    
  }else {               // set

    if(gHdpDlgCtx.CurQid == KEY_HDD_ENABLE_USER_PASSWORD ||
       gHdpDlgCtx.CurQid == KEY_HDD_CHANGE_USER_PASSWORD){
      Title = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_SATA_INPUT_USER_PSW), NULL);
    } else if(gHdpDlgCtx.CurQid == KEY_HDD_SET_MASTER_PASSWORD ||
              gHdpDlgCtx.CurQid == KEY_HDD_CHANGE_MASTER_PASSWORD){
      Title = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_SATA_INPUT_MASTER_PSW), NULL);
    } else {
      Title = NULL;
    }
   
    Status = gHdpDlgCtx.BdsBootMgr->DrawPwdDlg(Password, ARRAY_SIZE(Password), HdpDlgCallBack, Title, &Reason);

    if(Title != NULL){
      FreePool(Title);
    }
    
    if(EFI_ERROR(Status)){
      HdpHandleErrorReason(Reason);
      goto ProcExit;
    }

	Status = CheckPasswordForHdd(Password);
    
    if(EFI_ERROR(Status)){
      goto ProcExit;
    }
    


    if(Password[0] == 0){
      HdpHandleErrorReason(PASSWD_REASON_EMPTY);
      goto ProcExit;      
    }

    BuildHdpFromUserInput(
      Password, 
      HdpInfo->SerialNo,
      PasswordData,
      NULL
      );
    Status = SecuritySetHddPassword (
               HdpInfo,
               !gHdpDlgCtx.IsUserQ,                                           // user
               0,                                                             // high
               gHdpDlgCtx.IsUserQ ? 0 : MASTER_PASSWORD_USER_MASTER_REVCODE,  // RevCode
               PasswordData,
               sizeof(PasswordData)
               );
    DEBUG((EFI_D_INFO, "SetHddPassword:%r\n", Status));
    if(!EFI_ERROR(Status)){
      UpdateHddSecurityStatus(HdpInfo, IfrData);
    }
    
  }
  
  HdpHandleSuccess();

ProcExit:    
  ZeroMem(Password, sizeof(Password));
  ZeroMem(PasswordData, sizeof(PasswordData));
  return Status;
}



EFI_STATUS
GetInputPassword (
  UINT8                           *Sn,
  UINT8                           *PasswordData
)
{
  CHAR16       *SubTitle;
  EFI_STATUS   Status;
  CHAR16       Password[32+1];


  SubTitle = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_INPUT_CUR_ADMIN_PSW), NULL);
  Status = gHdpDlgCtx.BdsBootMgr->DrawPostPwdDlg(gHdpDlgCtx.Current->HddString, SubTitle, Password, ARRAY_SIZE(Password));
  FreePool(SubTitle);

  if(!EFI_ERROR(Status)){
    BuildHdpFromUserInput (
      Password, 
      Sn,
      PasswordData,
      NULL
      );
  }

  return Status;
}



EFI_STATUS
EFIAPI
NvmeHdpDlgCallBack (
    IN CHAR16  *Password
  )
{
  UINT8    PasswordData[32];
  BOOLEAN  Rc;
  

  if(Password == NULL){
    return EFI_UNSUPPORTED;
  }

  BuildHdpFromUserInput(
    Password, 
    gHdpDlgCtx.Current->HdpInfo.SerialNo,
    PasswordData,
    NULL
    );

  Rc = NvmePasswordVerify(gHdpDlgCtx.Current->NvmeInfo, 1, PasswordData, sizeof(PasswordData));   // admin
  if(!Rc && gHdpDlgCtx.CurQid == KEY_NVME_CHANGE_USER_PASSWORD){
    Rc = NvmePasswordVerify(gHdpDlgCtx.Current->NvmeInfo, 0, PasswordData, sizeof(PasswordData)); // user
  }
  if(Rc){
    CopyMem(gHdpDlgCtx.OldPassword, PasswordData, sizeof(PasswordData));
    return EFI_SUCCESS;
  } else {
    return EFI_ABORTED;
  }
}



EFI_STATUS
GetInputPassword2 (
  UINT8                           *Sn,
  UINT8                           *PasswordData,
  UINTN                           *Reason
)
{
  EFI_STATUS   Status;
  CHAR16       *Title;
  CHAR16       Password[32+1];

  if(gHdpDlgCtx.CurQid == KEY_NVME_CHANGE_ADMIN_PASSWORD){
    Title = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_INPUT_ADMIN_PSW), NULL);
  } else {
    Title = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_INPUT_PSW), NULL);
  }
  
  Status = gHdpDlgCtx.BdsBootMgr->DrawPwdDlg(
                                    Password, 
                                    ARRAY_SIZE(Password), 
                                    NvmeHdpDlgCallBack, 
                                    Title, 
                                    Reason
                                    );
  if(Title != NULL){
    FreePool(Title);
  }
  
  if(!EFI_ERROR(Status)){
    if(Password[0] == 0){
      *Reason = PASSWD_REASON_EMPTY;
      Status = EFI_ABORTED;
    } else {
      BuildHdpFromUserInput(
        Password, 
        Sn,
        PasswordData,
        NULL
        );
    }
  }

  return Status;
}



EFI_STATUS
EFIAPI
NvmeHdpDlgCallBack3 (
    IN CHAR16  *Password
  )
{
  return EFI_SUCCESS;
}


EFI_STATUS
GetInputPassword3 (
  UINT8                           *Sn,
  UINT8                           *PasswordData,
  UINTN                           *Reason
)
{
  EFI_STATUS   Status;
  CHAR16       *Title;
  CHAR16       Password[32+1];

  Title = HiiGetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(HP_STR_INPUT_NEW_ADMIN_PSW), NULL);
  Status = gHdpDlgCtx.BdsBootMgr->DrawPwdDlg(
                                    Password, 
                                    ARRAY_SIZE(Password), 
                                    NvmeHdpDlgCallBack3, 
                                    Title, 
                                    Reason
                                    );
  FreePool(Title);
  if(!EFI_ERROR(Status)){
  	Status =  CheckPasswordForHdd(Password);
  }
  if(!EFI_ERROR(Status)){
    if(Password[0] == 0){
      *Reason = PASSWD_REASON_EMPTY;
      Status = EFI_ABORTED;
    } else {    
      BuildHdpFromUserInput(
        Password, 
        Sn,
        PasswordData,
        NULL
        );
    }
  }

  return Status;
}






EFI_STATUS
EFIAPI
HdpFormCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  HDD_PASSWORD_CONFIG_FORM_ENTRY   *ConfigFormEntry;
  BOOLEAN                          rc;
  BOOLEAN                          NeedUpdateData = FALSE;
  EFI_STRING_ID                    StrId;
  UINTN                            Reason;
  UINT8                            PasswordData[32];


  DEBUG((EFI_D_INFO, "%a() A:%d Q:%d T:%d\n", __FUNCTION__, Action, QuestionId, Type));

  if(!gHdpDlgCtx.Init){
    Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, (VOID**)&gHdpDlgCtx.BdsBootMgr);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    gHdpDlgCtx.HiiHandle = gPrivate->HiiHandle;
    gHdpDlgCtx.Init = 1; 
  }

  if (Action == EFI_BROWSER_ACTION_CHANGING && QuestionId >= KEY_HDD_DEVICE_ENTRY_BASE && 
      QuestionId <  KEY_HDD_DEVICE_ENTRY_BASE + mNumberOfHddDevices) {
    ConfigFormEntry = HddPasswordGetConfigFormEntryByIndex(QuestionId - KEY_HDD_DEVICE_ENTRY_BASE);
    ASSERT (ConfigFormEntry != NULL);
    if(gHdpDlgCtx.Current != ConfigFormEntry){
      gHdpDlgCtx.Current = ConfigFormEntry;
      NeedUpdateData = TRUE;
      DEBUG((EFI_D_INFO, "Current:%X\n", ConfigFormEntry));
    }
  }

  if(Action == EFI_BROWSER_ACTION_RETRIEVE){
    if(QuestionId == KEY_HDD_NAME || QuestionId == KEY_HDD_NAME2 || QuestionId == KEY_HDD_NAME3){
      StrId = HiiSetString(gHdpDlgCtx.HiiHandle, STRING_TOKEN(STR_HDD_NAME), gHdpDlgCtx.Current->HddString, NULL);
      DEBUG((EFI_D_INFO, "StrId:%X %s\n", StrId, gHdpDlgCtx.Current->HddString));
    }
    return EFI_SUCCESS;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGING) {
    return EFI_SUCCESS;
  }

  gHdpDlgCtx.CurQid = QuestionId;

  switch (QuestionId) {
      
    case KEY_HDD_ENABLE_USER_PASSWORD:
    case KEY_HDD_DISABLE_USER_PASSWORD:
    case KEY_HDD_CHANGE_USER_PASSWORD:
      Status = HandlePasswordSet(TRUE);
      NeedUpdateData = TRUE;
      break;

    case KEY_HDD_SET_MASTER_PASSWORD:
    case KEY_HDD_CHANGE_MASTER_PASSWORD:
      Status = HandlePasswordSet(FALSE); 
      NeedUpdateData = TRUE;      
      break;


    case KEY_NVME_ENABLE_ADMIN_PASSWORD:
      Status = GetInputPassword3(gHdpDlgCtx.Current->HdpInfo.SerialNo, PasswordData, &Reason);
      if(!EFI_ERROR(Status)){
        Status = NvmeSetPassword(
                   gHdpDlgCtx.Current->NvmeInfo, 
                   1, 
                   1,
                   NULL, 
                   0, 
                   PasswordData, 
                   sizeof(PasswordData)
                   ); 
        if(!EFI_ERROR(Status)){
          Status = NvmeUpdateStatus(gHdpDlgCtx.Current->NvmeInfo);
          if(!EFI_ERROR(Status)){
            SyncNvmeInfoToIfr(gHdpDlgCtx.Current->NvmeInfo, &gHdpDlgCtx.Current->IfrData);
          }
          HdpHandleSuccess();
          NeedUpdateData = TRUE; 
        }        
      } else {
        HdpHandleErrorReason(Reason);
      }
      ZeroMem(PasswordData, sizeof(PasswordData));
      break;
      
    case KEY_NVME_DISABLE_ADMIN_PASSWORD:
      Status = GetInputPassword(gHdpDlgCtx.Current->HdpInfo.SerialNo, PasswordData);
      if(!EFI_ERROR(Status)){
        Status = NvmeDisableAdmin(
                   gHdpDlgCtx.Current->NvmeInfo, 
                   PasswordData,
                   sizeof(PasswordData)
                   );
        if(!EFI_ERROR(Status)){
          Status = NvmeUpdateStatus(gHdpDlgCtx.Current->NvmeInfo);
          if(!EFI_ERROR(Status)){
            SyncNvmeInfoToIfr(gHdpDlgCtx.Current->NvmeInfo, &gHdpDlgCtx.Current->IfrData);
          }
          HdpHandleSuccess();
          NeedUpdateData = TRUE; 
        } else {
          HdpHandleErrorReason(PASSWD_REASON_WRONG_OLD);
        }
      }
      ZeroMem(PasswordData, sizeof(PasswordData));
      break;
      
    case KEY_NVME_CHANGE_ADMIN_PASSWORD:      
    case KEY_NVME_CHANGE_USER_PASSWORD:
      Status = GetInputPassword2(gHdpDlgCtx.Current->HdpInfo.SerialNo, PasswordData, &Reason);
      if(!EFI_ERROR(Status)){
        Status = NvmeSetPassword (
                   gHdpDlgCtx.Current->NvmeInfo, 
                   QuestionId == KEY_NVME_CHANGE_ADMIN_PASSWORD, 
                   0,
                   gHdpDlgCtx.OldPassword, 
                   sizeof(gHdpDlgCtx.OldPassword), 
                   PasswordData, 
                   sizeof(PasswordData)
                   );
        if(!EFI_ERROR(Status)){
          Status = NvmeUpdateStatus(gHdpDlgCtx.Current->NvmeInfo);
          if(!EFI_ERROR(Status)){
            SyncNvmeInfoToIfr(gHdpDlgCtx.Current->NvmeInfo, &gHdpDlgCtx.Current->IfrData);
          }
          HdpHandleSuccess();
          NeedUpdateData = TRUE; 
        }        
      } else {
        HdpHandleErrorReason(Reason);
      }
      ZeroMem(PasswordData, sizeof(PasswordData));
      ZeroMem(gHdpDlgCtx.OldPassword, sizeof(gHdpDlgCtx.OldPassword));
      break;
      
  }

  if (NeedUpdateData) {
    rc = HiiSetBrowserData(
           &mHddPasswordVendorGuid, 
           mHddPasswordVendorStorageName, 
           sizeof(HDD_PASSWORD_CONFIG), 
           (UINT8*)&gHdpDlgCtx.Current->IfrData,
           NULL
           );
    if(!rc){
      DEBUG((EFI_D_INFO, "HiiSetBrowserData error\n"));
    }
  }

  return Status;
}









EFI_STATUS
HandleHddPassword (
    ATA_PASSTHRU_HOOK_PARAMETER   *p,
    UINT16                        Port,
    UINT16                        PortMp
  )
{
  LIST_ENTRY                       *Entry;
  HDD_PASSWORD_CONFIG_FORM_ENTRY   *ConfigFormEntry;
  BOOLEAN                          EntryExisted;
  EFI_STATUS                       Status;
  CHAR8                            ModelName[40+1];
  CHAR16                           Password[32 + 1];
  HDD_PASSWORD_INFO                *HdpInfo;
  UINTN                            Index;
  EFI_PCI_IO_PROTOCOL              *PciIo;
  UINT8                            PciScc;
  HDD_PASSWORD_CONFIG              *IfrData;
  UINT8                            PasswordOut[32];
  CHAR16                           *StrTitle;
  CHAR16                           *StrTryAgain;
  CHAR16                           *StrCancel;
  CHAR16                           *EnterCurPwStr;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut;
  EFI_SIMPLE_TEXT_OUTPUT_MODE       SavedConsoleMode;
  UINTN                             SegNum;
  UINTN                             BusNum;
  UINTN                             DevNum;
  UINTN                             FuncNum;
  BOOLEAN                           IsUser;
  EFI_BDS_BOOT_MANAGER_PROTOCOL     *BdsBootMgr;
  EFI_HANDLE                        Handle = NULL;

  UINTN                             AtaIndex;
  UINTN                             PlatSataHostIndex = 0;
  UINTN                             PlatSataPortIndex = 0;
  UINTN                             PhysicsPortIndex = 0;
  PLAT_HOST_INFO_PROTOCOL           *PlatHostInfo;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  EntryExisted = FALSE;
  EFI_LIST_FOR_EACH (Entry, &mHddPasswordConfigFormList) {
    ConfigFormEntry = BASE_CR (Entry, HDD_PASSWORD_CONFIG_FORM_ENTRY, Link);
    HdpInfo = &ConfigFormEntry->HdpInfo;
    if (HdpInfo->Controller == p->Controller &&
        HdpInfo->Port == Port && 
        HdpInfo->PortMp == PortMp) {
      EntryExisted = TRUE;
      break;
    }
  }
  if (EntryExisted) {
    DEBUG((EFI_D_INFO, "exist\n"));
    return EFI_ALREADY_STARTED;
  } 
 
  ConfigFormEntry = AllocateZeroPool(sizeof(HDD_PASSWORD_CONFIG_FORM_ENTRY));
  ASSERT(ConfigFormEntry != NULL);
  if (ConfigFormEntry == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  HdpInfo = &ConfigFormEntry->HdpInfo;
  InitializeListHead (&ConfigFormEntry->Link);
  HdpInfo->AtaPassThru = p->AtaPassThru;
  HdpInfo->Controller  = p->Controller;
  HdpInfo->Port        = Port;
  HdpInfo->PortMp      = PortMp;

  HdpInfo->AsbAlloc = (VOID*)AllocatePool(sizeof(EFI_ATA_STATUS_BLOCK) + p->AtaPassThru->Mode->IoAlign);
  ASSERT(HdpInfo->AsbAlloc != NULL);
  HdpInfo->Asb = (EFI_ATA_STATUS_BLOCK*)ALIGN_POINTER(HdpInfo->AsbAlloc, p->AtaPassThru->Mode->IoAlign);

  HdpInfo->IdentifyAlloc = (VOID*)AllocatePool(sizeof(ATA_IDENTIFY_DATA) + p->AtaPassThru->Mode->IoAlign);
  ASSERT(HdpInfo->IdentifyAlloc != NULL);  
  HdpInfo->Identify = (ATA_IDENTIFY_DATA*)ALIGN_POINTER(HdpInfo->IdentifyAlloc, p->AtaPassThru->Mode->IoAlign);

  HdpInfo->PayLoadSize = 512;
  HdpInfo->PayLoadAlloc = (VOID*)AllocatePool(HdpInfo->PayLoadSize + p->AtaPassThru->Mode->IoAlign);
  ASSERT(HdpInfo->PayLoadAlloc != NULL);  
  HdpInfo->PayLoad = (UINT16*)ALIGN_POINTER(HdpInfo->PayLoadAlloc, p->AtaPassThru->Mode->IoAlign);

  for(Index=0;Index<ATA_CMD_RETRY_MAX_COUNT;Index++){
    Status = GetHddDeviceIdentifyData(HdpInfo);
    if(!EFI_ERROR(Status)){
      break;
    }
    DEBUG((EFI_D_ERROR, "Identify:%r\n", Status)); 
  }
  if(Index >= ATA_CMD_RETRY_MAX_COUNT){
    goto ProcExit;
  }

  Status = gBS->HandleProtocol (
                p->Controller,
                &gEfiPciIoProtocolGuid,
                (VOID**)&PciIo
                );
  ASSERT_EFI_ERROR (Status);
  Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0x0A, 1, &PciScc);
  ASSERT_EFI_ERROR (Status);

  if(PciScc == 0x01){         // IDE
    HdpInfo->HddIndex = Port * 2 + PortMp;
  } else {
    HdpInfo->HddIndex = Port;
  }

  Status = PciIo->GetLocation (
                    PciIo,
                    &SegNum,
                    &BusNum,
                    &DevNum,
                    &FuncNum
                    );
  ASSERT_EFI_ERROR(Status);  

  HdpInfo->Bus    = (UINT8)BusNum;
  HdpInfo->Dev    = (UINT8)DevNum;  
  HdpInfo->Func   = (UINT8)FuncNum;
  HdpInfo->PciIo  = PciIo;
  HdpInfo->PciScc = PciScc;

  IfrData = &ConfigFormEntry->IfrData;
  GetHddPasswordSecurityStatus(HdpInfo->Identify, IfrData);

  CopyMem(ModelName, HdpInfo->Identify->ModelName, 40);
  SwapWordArray(ModelName, 40);
  ModelName[40] = 0;
  TrimStr8(ModelName);

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &PlatHostInfo);
  if(!EFI_ERROR(Status)){
     PlatSataHostIndex = PlatHostInfo->GetSataHostIndex(p->Controller) ;
     AtaIndex = PlatHostInfo->GetSataPortIndex(p->Controller,&PlatSataPortIndex) ;
     if(PlatSataHostIndex == 0xFFFF || PlatSataHostIndex == 0x8000){
       PlatSataHostIndex = 0xFFFF;
     } else {
       PlatSataHostIndex +=1;
     }
  }
  DEBUG((EFI_D_ERROR, "PlatSataHostIndex:%d\n", PlatSataHostIndex)); 
  
  if(PlatSataHostIndex == 0xFFFF){
    UnicodeSPrint(
      ConfigFormEntry->HddString, 
      sizeof(ConfigFormEntry->HddString), 
      L"SATA %d: %a", 
      HdpInfo->HddIndex, 
      ModelName
      ); 
  }else{
    UnicodeSPrint(
      ConfigFormEntry->HddString, 
      sizeof(ConfigFormEntry->HddString), 
      L"SATA%d-%d: %a", 
      PlatSataHostIndex-1,
      HdpInfo->HddIndex, 
      ModelName
      ); 
  } 

  CopyMem(HdpInfo->SerialNo, HdpInfo->Identify->SerialNo, sizeof(HdpInfo->SerialNo));

  ConfigFormEntry->TitleToken = HiiSetString(gPrivate->HiiHandle, 0, ConfigFormEntry->HddString, NULL);

  if(StrStr(ConfigFormEntry->HddString,L"ASMT106")!=NULL){ //Skip Asmedia Raid Device
      return EFI_ABORTED;
  }

  InsertTailList (&mHddPasswordConfigFormList, &ConfigFormEntry->Link);

  mNumberOfHddDevices++;

  DEBUG((EFI_D_INFO, "Port:%d ModelName:%a En:%d Lock:%d Exp:%d Frozen:%d Count:%d\n", \
    HdpInfo->HddIndex, ModelName, IfrData->Enabled, IfrData->Locked, IfrData->CountExpired, \
    IfrData->Frozen, mNumberOfHddDevices));

  if(!IfrData->Enabled || IfrData->Frozen){
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

  Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, &BdsBootMgr);
  ASSERT(!EFI_ERROR(Status));  

  HotKeySaveAndRestore(TRUE);

	while(1){
    
    if(CheckAndHandleRetryCountOut(IfrData)){
      break;
    }
    
    Status = BdsBootMgr->DrawPostPwdDlg(ConfigFormEntry->HddString, EnterCurPwStr, Password, ARRAY_SIZE(Password));
    ConOut->SetAttribute(ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
    ConOut->EnableCursor(ConOut, FALSE);
    ConOut->ClearScreen(ConOut);
    if(!EFI_ERROR(Status)){
      BuildHdpFromUserInput(Password, HdpInfo->SerialNo, PasswordOut, NULL);

      IsUser = TRUE;
      Status = SecurityUnlockHdd(HdpInfo, IsUser, PasswordOut, sizeof(PasswordOut));
      DEBUG((EFI_D_INFO, "UnlockHdd:%r\n", Status));
      /*
      if(EFI_ERROR(Status)){
        IsUser = FALSE;
        Status = SecurityUnlockHdd(HdpInfo, IsUser, PasswordOut, sizeof(PasswordOut));
        DEBUG((EFI_D_INFO, "UnlockHdd:%r\n", Status));
      }
      */
      if(!EFI_ERROR(Status)){
        Status = UpdateHddSecurityStatus(HdpInfo, IfrData);
        ASSERT(!EFI_ERROR (Status));
        DEBUG((EFI_D_INFO, "En:%d Lock:%d Exp:%d Count:%d\n", IfrData->Enabled, IfrData->Locked, IfrData->CountExpired,IfrData->RetryCount));
        if (!IfrData->Locked) {
          if(IsUser){
            HdpInfo->PasswordType = 0x80;
          } else {
            HdpInfo->PasswordType = 0x81;
            IfrData->ChangeMasterSupport = 1;
          }
          IfrData->RetryCount = 0;
          break;
        }
      }
      IfrData->RetryCount++;
      
    }else {     // ESC, cancel
      if(!IfrData->Locked){
        PromptNoPasswordError();
      }
      UiConfirmDialog(DIALOG_WARNING, StrTitle, NULL, TEXT_ALIGIN_CENTER, StrCancel, NULL);
      break;
    }

    if(IfrData->RetryCount < 3){
      UiConfirmDialog(DIALOG_WARNING, StrTitle, NULL, TEXT_ALIGIN_CENTER, StrTryAgain, NULL);
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

  DEBUG((EFI_D_INFO, "En:%d Lock:%d Exp:%d\n", \
    IfrData->Enabled, IfrData->Locked, IfrData->CountExpired));
  
	ZeroMem(Password, sizeof(Password));
	ZeroMem(PasswordOut, sizeof(PasswordOut));    

ProcExit:
  return EFI_SUCCESS;
}




VOID
AtaPassThruHook (
  VOID  *Param   
  )
{
  ATA_PASSTHRU_HOOK_PARAMETER   *p;
  UINT16                        Port;
  UINT16                        PortMp;
  EFI_STATUS                    Status;


  p = (ATA_PASSTHRU_HOOK_PARAMETER*)Param;
  ASSERT(p->Sign == ATA_PASSTHRU_HOOK_PARAMETER_SIGN);

  DEBUG((EFI_D_INFO, "SataMode:%X\n", p->SataMode));

  Port = 0xFFFF;
  while (TRUE) {
    Status = p->AtaPassThru->GetNextPort(p->AtaPassThru, &Port);
    if (EFI_ERROR(Status)) {
      break;
    } 
    PortMp = 0xFFFF;
    while (TRUE) {
      Status = p->AtaPassThru->GetNextDevice(p->AtaPassThru, Port, &PortMp);
      if (EFI_ERROR (Status)) {
        break;
      }
      
      DEBUG((EFI_D_INFO, "AtaDev(%X,%X)\n", Port, PortMp));
      HandleHddPassword(p, Port, PortMp);
    }
  }  
  
}






VOID
HdpOnReadyToBoot (
  )
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  LIST_ENTRY                        *Entry;
  HDD_PASSWORD_CONFIG_FORM_ENTRY    *ConfigFormEntry;
  HDD_PASSWORD_INFO                 *HdpInfo;
  HDD_PASSWORD_CONFIG               *IfrData;
  STATIC BOOLEAN                    RunOnce = FALSE;
  

  if(RunOnce){
    return;
  }
  RunOnce = TRUE;

  EFI_LIST_FOR_EACH (Entry, &mHddPasswordConfigFormList) {
    ConfigFormEntry = BASE_CR (Entry, HDD_PASSWORD_CONFIG_FORM_ENTRY, Link);
    HdpInfo = &ConfigFormEntry->HdpInfo;
    IfrData = &ConfigFormEntry->IfrData;
    if(!IfrData->Frozen && !IfrData->Locked){
      if(HdpInfo->HddIndex < 0x80){
        Status = SecurityFrozenLock(HdpInfo);
      } else {
        Status = NvmeOpalBlockSid(ConfigFormEntry->NvmeInfo);
      }
      DEBUG((EFI_D_INFO, "[%d] Frozen:%r\n", HdpInfo->HddIndex, Status));
    }
  }

}




VOID
EFIAPI
HdpEnterSetupCallBack (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  VOID                             *Interface;
  EFI_STATUS                       Status;
  VOID                             *StartOpCodeHandle;
  VOID                             *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL               *StartLabel;
  EFI_IFR_GUID_LABEL               *EndLabel; 
  UINTN                            FormIndex;
  HDD_PASSWORD_CONFIG_FORM_ENTRY   *ConfigFormEntry;
  LIST_ENTRY                       *Entry;
  BOOLEAN                          IsLocked;
  EFI_FORM_ID                      FormId;
  

  Status = gBS->LocateProtocol(&gEfiSetupEnterGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  StartOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof(EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = HDD_DEVICE_ENTRY_LABEL;

  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof(EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = HDD_DEVICE_LABEL_END;

  FormIndex = 0;
  EFI_LIST_FOR_EACH (Entry, &mHddPasswordConfigFormList) {
    ConfigFormEntry = BASE_CR (Entry, HDD_PASSWORD_CONFIG_FORM_ENTRY, Link);

	if(StrStr(ConfigFormEntry->HddString,L"ASMT106")!=NULL){ //Skip Asmedia Raid Device
      continue;
	}

    IsLocked = ConfigFormEntry->IfrData.Enabled && ConfigFormEntry->IfrData.Locked;
    if(ConfigFormEntry->HdpInfo.HddIndex >= 0x80){
      if(ConfigFormEntry->IfrData.Frozen){
        FormId = FORMIN_HDD_LOCK_FORM;
      } else {
        FormId = FORMID_NVME_DEVICE_FORM;
      }
    } else {
      if(ConfigFormEntry->IfrData.Frozen || IsLocked){
        FormId = FORMIN_HDD_LOCK_FORM;
      } else {
        FormId = FORMID_HDD_DEVICE_FORM;
      }
    }

    HiiCreateGotoOpCode (
      StartOpCodeHandle,                                // Container for dynamic created opcodes
      FormId,                                           // Target Form ID
      ConfigFormEntry->TitleToken,                      // Prompt text
      ConfigFormEntry->TitleHelpToken,                  // Help text
      EFI_IFR_FLAG_CALLBACK,                            // Question flag
      (UINT16)(KEY_HDD_DEVICE_ENTRY_BASE + FormIndex)   // Question ID
      );

    FormIndex++;

    DEBUG((EFI_D_INFO, "FormIndex:%X %X %X\n", FormIndex, ConfigFormEntry->TitleToken, ConfigFormEntry->TitleHelpToken));
    
  }

  Status = HiiUpdateForm (
            gPrivate->HiiHandle,
            &mHddPasswordVendorGuid,
            FORMID_HDD_MAIN_FORM,
            StartOpCodeHandle,
            EndOpCodeHandle
            );
  DEBUG((EFI_D_INFO, "HiiUpdateForm:%r\n", Status));

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);
}



EFI_STATUS
EFIAPI
HddPasswordDxeInit (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                     Status;
  HDD_PASSWORD_DXE_PRIVATE_DATA  *Private;
  VOID                           *Registration;
  EFI_BOOT_MODE                  BootMode;  


  BootMode = GetBootModeHob();
  if(BootMode == BOOT_IN_RECOVERY_MODE){
    return EFI_UNSUPPORTED;
  }

  InitializeListHead(&mHddPasswordConfigFormList);

  Private = AllocateZeroPool(sizeof(HDD_PASSWORD_DXE_PRIVATE_DATA));
  if (Private == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Private->Current = &Private->Dummy;

  Status = gBS->LocateProtocol(&gCryptoLibDxeProtocolGuid, NULL, (VOID**)&Private->CryptoLib);
  ASSERT(!EFI_ERROR(Status));

  Private->Signature   = HDD_PASSWORD_DXE_PRIVATE_SIGNATURE;
  Private->ConfigAccess.ExtractConfig = HdpFormExtractConfig;
  Private->ConfigAccess.RouteConfig   = HdpFormRouteConfig;
  Private->ConfigAccess.Callback      = HdpFormCallback;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Private->DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mHddPasswordHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &Private->ConfigAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);

  Private->HiiHandle = HiiAddPackages (
                         &mHddPasswordVendorGuid,
                         Private->DriverHandle,
                         HddPasswordDxeStrings,
                         HddPasswordBin,
                         NULL
                         );
  ASSERT(Private->HiiHandle != NULL);

  gPrivate = Private;

  EfiCreateProtocolNotifyEvent (
    &gEfiSetupEnterGuid,
    TPL_CALLBACK,
    HdpEnterSetupCallBack,
    (VOID*)Private,
    &Registration
    ); 

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gAtaPassThruHookProtocolGuid, AtaPassThruHook,
                 // &gEfiReadyToBootProtocolGuid, HdpOnReadyToBoot,
                  &gNvmeSscpHookProtocolGuid, NvmeSscpHook,
                  NULL
                  );
           
  return EFI_SUCCESS;
}




