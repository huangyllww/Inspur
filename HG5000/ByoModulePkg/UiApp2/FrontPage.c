/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  FrontPage.c
Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#include "FrontPage.h"
#include "../../PlatformPkg/Include/SetupVariable.h"
#include <Protocol/BootLogo.h>
#include <Guid/TcmSetupCfgGuid.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <Guid/IScsiConfigHii.h>
#include <ByoPlatformSetupConfig.h>
#include <Library/PlatformLanguageLib.h>
#include <Protocol/SystemPasswordProtocol.h>
#include <Library/SetupUiLib.h>

EFI_STATUS
AddBaseFormset (
  VOID 
  );

VOID BmValueInit();

extern EFI_GUID gSecureBootConfigFormSetGuid;
extern EFI_GUID gTcgConfigFormSetGuid;
extern EFI_GUID gTcg2ConfigFormSetGuid;
extern VOID     *gMyFontBin;
extern EFI_GUID gEfiNetworkInterfaceIdentifierProtocolGuid_31;

EFI_GUID  gHddPasswordVendorGuid = 
  {0xd5fd1546, 0x22c5, 0x4c2e, { 0x96, 0x9f, 0x27, 0x3c, 0x0, 0x77, 0x10, 0x80}};

EFI_GUID  gEfiFileExplorerFormsetGuid = 
  {0xfe561596, 0xe6bf, 0x41a6, {0x83, 0x76, 0xc7, 0x2b, 0x71, 0x98, 0x74, 0xd0}};

EFI_GUID  gByoSetupPciListFormsetGuid = SETUP_PCI_LIST_FORMSET_GUID;

EFI_GUID  gAmdCbsFormsetGuid = 
  {0xB04535E3, 0x3004, 0x4946, {0x9E, 0xB7, 0x14, 0x94, 0x28, 0x98, 0x30, 0x53}};

EFI_GUID  gByoBmcFormsetGuid = 
  {0x8236697e, 0xf0f6, 0x405f, {0x99, 0x13, 0xac, 0xbc, 0x50, 0xaa, 0x45, 0xd1}};
  
EFI_FORM_BROWSER2_PROTOCOL              *gFormBrowser2;
BOOLEAN                                 gAfterReadyToBoot = FALSE;

EFI_STATUS UiAppAddBaseFormset (VOID);
EFI_STATUS AddBootHiiPackages(EFI_HANDLE DeviceHandle);


#define _FREE_NON_NULL(POINTER) \
  do{ \
    if((POINTER) != NULL) { \
      FreePool((POINTER)); \
      (POINTER) = NULL; \
    } \
  } while(FALSE)


#define FORMSET_ADD_IGNORE       0
#define FORMSET_ADD_ADVANCE      1
#define FORMSET_ADD_SECURITY     2


typedef struct {
  EFI_GUID  *FormsetGuid;
  UINTN     Action;
} FORMSET_ADD_ACTION;

STATIC FORMSET_ADD_ACTION gFormsetAddActionList[] = {
  {&gIScsiConfigGuid,        FORMSET_ADD_IGNORE},          // do not move its position
  {&gEfiFormsetGuidMain,     FORMSET_ADD_IGNORE},
  {&gEfiFormsetGuidAdvance,  FORMSET_ADD_IGNORE},
  {&gEfiFormsetGuidDevices,  FORMSET_ADD_IGNORE},
  {&gEfiFormsetGuidPower,    FORMSET_ADD_IGNORE},
  {&gEfiFormsetGuidBoot,     FORMSET_ADD_IGNORE},
  {&gEfiFormsetGuidSecurity, FORMSET_ADD_IGNORE},
  {&gEfiFormsetGuidExit,     FORMSET_ADD_IGNORE},
  {&gEfiFileExplorerFormsetGuid, FORMSET_ADD_IGNORE},
  {&gByoSetupPciListFormsetGuid, FORMSET_ADD_IGNORE},
  {&gAmdCbsFormsetGuid,          FORMSET_ADD_IGNORE},
  {&gByoBmcFormsetGuid,          FORMSET_ADD_IGNORE},
  {&gTcgConfigFormSetGuid,        FORMSET_ADD_SECURITY},
  {&gTcg2ConfigFormSetGuid,       FORMSET_ADD_SECURITY},
  {&gTcmSetupConfigGuid,          FORMSET_ADD_SECURITY},
  {&gSecureBootConfigFormSetGuid, FORMSET_ADD_SECURITY},
  {&gHddPasswordVendorGuid,       FORMSET_ADD_SECURITY},

};  

UINTN GetFormsetAddAction(EFI_GUID *FormsetGuid)
{
  UINTN  Index;
  UINTN  Count;

  Count = sizeof(gFormsetAddActionList)/sizeof(gFormsetAddActionList[0]);
  for(Index=0;Index<Count;Index++){
    if(CompareGuid(FormsetGuid, gFormsetAddActionList[Index].FormsetGuid)){
      return gFormsetAddActionList[Index].Action;
    }  
  }

  return FORMSET_ADD_ADVANCE;
}




typedef struct {
  EFI_GUID          FormsetGuid;
  EFI_STRING_ID     FormSetTitle;
  EFI_STRING_ID     FormSetHelp;
  EFI_HII_HANDLE    HiiHandle;
} HII_FORMSET_GOTO_INFO;


STATIC 
EFI_STATUS 
GetAllHiiFormset (
  HII_FORMSET_GOTO_INFO  **Info,
  UINTN                  *InfoCount
  )
{
  EFI_STATUS                   Status;
  EFI_HII_HANDLE               HiiHandle;
  EFI_HII_HANDLE               *HiiHandleBuffer;
  UINTN                        Index; 
  EFI_HII_DATABASE_PROTOCOL    *HiiDB;
  UINTN                        BufferSize;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
  EFI_HII_PACKAGE_HEADER       PackageHeader;
  UINT32                       Offset;
  UINT32                       Offset2;
  UINT32                       PackageListLength;
  UINT8                        *Package;
  UINT8                        *OpCodeData;
  EFI_GUID                     *FormsetGuid;  
  UINT8                        OpCode;
  HII_FORMSET_GOTO_INFO        *GotoInfo = NULL;
  UINTN                        GotoIndex = 0;
  EFI_STRING_ID                FormSetTitle;
  EFI_STRING_ID                FormSetHelp; 

  
  HiiHandleBuffer = NULL;
  HiiPackageList  = NULL;
  
  Status = gBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid, NULL, &HiiDB);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }  
  
  HiiHandleBuffer = HiiGetHiiHandles(NULL);
  if(HiiHandleBuffer == NULL){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }

  for(Index=0; HiiHandleBuffer[Index]!=NULL; Index++){
    HiiHandle = HiiHandleBuffer[Index];
    
    _FREE_NON_NULL(HiiPackageList);
    BufferSize = 0;
    Status = HiiDB->ExportPackageLists(HiiDB, HiiHandle, &BufferSize, HiiPackageList);
    if(Status != EFI_BUFFER_TOO_SMALL){
      if(Status == EFI_SUCCESS){Status = EFI_ABORTED;}
      goto ProcExit;
    }
    HiiPackageList = AllocatePool(BufferSize);
    ASSERT(HiiPackageList != NULL);
    Status = HiiDB->ExportPackageLists(HiiDB, HiiHandle, &BufferSize, HiiPackageList);
    if(EFI_ERROR(Status)){
      goto ProcExit;
    } 

//- DEBUG((EFI_D_INFO, "Pkg[%d] %g\n", Index, &HiiPackageList->PackageListGuid));

    PackageListLength = ReadUnaligned32(&HiiPackageList->PackageLength);
    Offset  = sizeof(EFI_HII_PACKAGE_LIST_HEADER);  // total 
    Offset2 = 0;                                    // form

    while(Offset < PackageListLength){
      Package = (UINT8*)HiiPackageList + Offset;
      CopyMem(&PackageHeader, Package, sizeof(EFI_HII_PACKAGE_HEADER));
      
      if(PackageHeader.Type != EFI_HII_PACKAGE_FORMS){
        Offset += PackageHeader.Length;
        continue;
      }
      
      Offset2 = sizeof(EFI_HII_PACKAGE_HEADER);
      while(Offset2 < PackageHeader.Length){
        OpCodeData = Package + Offset2;
        OpCode     = ((EFI_IFR_OP_HEADER*)OpCodeData)->OpCode;
        if(OpCode != EFI_IFR_FORM_SET_OP){
          Offset2 += ((EFI_IFR_OP_HEADER*)OpCodeData)->Length;
          continue;
        }

        GotoInfo = ReallocatePool(
                     GotoIndex * sizeof(HII_FORMSET_GOTO_INFO), 
                     (GotoIndex+1) * sizeof(HII_FORMSET_GOTO_INFO),
                     GotoInfo
                     );
        ASSERT(GotoInfo != NULL);

        FormsetGuid = (EFI_GUID*)(OpCodeData + OFFSET_OF(EFI_IFR_FORM_SET, Guid));
        CopyMem(&FormSetTitle, &((EFI_IFR_FORM_SET*)OpCodeData)->FormSetTitle, sizeof(EFI_STRING_ID));
        CopyMem(&FormSetHelp, &((EFI_IFR_FORM_SET*)OpCodeData)->Help, sizeof(EFI_STRING_ID));
        CopyMem(&GotoInfo[GotoIndex].FormsetGuid, FormsetGuid, sizeof(EFI_GUID));          

        GotoInfo[GotoIndex].FormSetTitle = FormSetTitle;
        GotoInfo[GotoIndex].FormSetHelp  = FormSetHelp;            
        GotoInfo[GotoIndex].HiiHandle = HiiHandle;
        GotoIndex++;
            
        Offset2 += ((EFI_IFR_OP_HEADER*)OpCodeData)->Length;  
      }
 
      Offset += PackageHeader.Length;
    }
    _FREE_NON_NULL(HiiPackageList);
  }

  DEBUG((EFI_D_INFO, "InfoCount:%d\n", GotoIndex));

  if(GotoIndex){
    *Info      = GotoInfo;
    *InfoCount = GotoIndex;
  } else {
    *Info      = NULL;
    *InfoCount = 0;
  }

ProcExit:
  _FREE_NON_NULL(HiiHandleBuffer);
  _FREE_NON_NULL(HiiPackageList);
  return Status;  
}




VOID ShowAllHiiFormset(
  HII_FORMSET_GOTO_INFO  *Info,
  UINTN                  InfoCount
)
{
#if !defined(MDEPKG_NDEBUG) 
  UINTN       Index;
  EFI_STRING  Title;

  for(Index=0;Index<InfoCount;Index++){
    Title = HiiGetString(Info[Index].HiiHandle, Info[Index].FormSetTitle, "en-US");
    DEBUG((EFI_D_INFO, "[%d] %g %s\n", Index, &Info[Index].FormsetGuid, Title));
    FreePool(Title);
  }
#endif  
}


typedef struct {
  EFI_HII_HANDLE         HiiHandle;
  VOID                   *StartOpCodeHandle;
  VOID                   *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL     *StartLabel;
  EFI_IFR_GUID_LABEL     *EndLabel; 
  EFI_GUID               FormsetGuid;
} ADD_LABEL_CTX;


VOID FreeAddLabelCtx(ADD_LABEL_CTX *Ctx)
{
  if(Ctx->StartOpCodeHandle!=NULL){
    HiiFreeOpCodeHandle(Ctx->StartOpCodeHandle);
  }
  if(Ctx->EndOpCodeHandle!=NULL){
    HiiFreeOpCodeHandle(Ctx->EndOpCodeHandle);
  }
}

EFI_STATUS InitAddLabelCtx(ADD_LABEL_CTX *Ctx, EFI_GUID *FormsetGuid, UINT16 StartNo, UINT16 EndNo)
{
  EFI_HII_HANDLE    *DevHii = NULL;
  EFI_STATUS        Status;


  DevHii = HiiGetHiiHandles(FormsetGuid);
  if(DevHii == NULL){
    DEBUG((EFI_D_ERROR, "Formset %g Not Found\n", FormsetGuid));
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }  
  Ctx->HiiHandle = DevHii[0];
  FreePool(DevHii);

  Ctx->StartOpCodeHandle = HiiAllocateOpCodeHandle();
  if (Ctx->StartOpCodeHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;   
  }
  Ctx->EndOpCodeHandle = HiiAllocateOpCodeHandle();
  if (Ctx->EndOpCodeHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit; 
  }
  
  Ctx->StartLabel = (EFI_IFR_GUID_LABEL*)HiiCreateGuidOpCode(Ctx->StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof(EFI_IFR_GUID_LABEL));
  Ctx->StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  Ctx->StartLabel->Number       = StartNo;
  
  Ctx->EndLabel = (EFI_IFR_GUID_LABEL*)HiiCreateGuidOpCode(Ctx->EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof(EFI_IFR_GUID_LABEL));
  Ctx->EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  Ctx->EndLabel->Number       = EndNo;

  CopyMem(&Ctx->FormsetGuid, FormsetGuid, sizeof(EFI_GUID));

  Status = EFI_SUCCESS;

ProcExit:
  return Status;
}


EFI_QUESTION_ID
GenerateUniqueQuestionId (
  EFI_HII_HANDLE    HiiHandle,
  EFI_STRING_ID    FormSetTitle
  )
{
  UINT16   QuestionId;
  UINTN   HandleCount;

  DEBUG ((EFI_D_ERROR, "\n GenerateUniqueQuestionId(), HiiHandle :0x%x, FormSetTitle :0x%x.\n", HiiHandle, FormSetTitle));

  HandleCount = (UINT64)HiiHandle;
  	
  QuestionId =  (UINT16) (HandleCount & 0xFFFF);
  QuestionId +=  (UINT16) (HandleCount>>16);
  QuestionId |=  (UINT16) (FormSetTitle & 0xFFFF);

  DEBUG ((EFI_D_ERROR, "GenerateUniqueQuestionId(), QuestionId :0x%x.\n", QuestionId));
  return (EFI_QUESTION_ID) QuestionId;
}



EFI_STRING_ID
SetStringWithSysLanguages (
  EFI_HII_HANDLE    NewHiiHandle,
  EFI_HII_HANDLE    OldHiiHandle,
  EFI_STRING_ID     StrId
  )
{
  EFI_STRING    EnglishString;  
  EFI_STRING    String;  
  EFI_STRING_ID    NewStrId;
  CHAR8    *LanguageString;
  CHAR8    *LangCode;
  CHAR8    *Lang;


  if (NewHiiHandle == NULL || OldHiiHandle == NULL || StrId == 0) {
    DEBUG((EFI_D_ERROR, "%a(%X,%X,%X)\n", __FUNCTION__, NewHiiHandle, OldHiiHandle, StrId));
    return 0;
  }

  LanguageString = HiiGetSupportedLanguages(NewHiiHandle);
  if (LanguageString == NULL) {
    DEBUG((EFI_D_ERROR, "NoSupportLang\n"));
    return 0;
  }

  Lang = AllocatePool(AsciiStrSize(LanguageString));
  if (Lang == NULL) {
    DEBUG((EFI_D_ERROR, "Alloc Err %a\n", LanguageString));
    return 0;
  }

  EnglishString = HiiGetString(OldHiiHandle, StrId, "en-US");
  if (EnglishString == NULL) {
    DEBUG((EFI_D_ERROR, "NoEnglish\n"));
    return 0;
  }

  DEBUG((EFI_D_INFO, "%s %a\n", EnglishString, LanguageString));

  NewStrId = 0;
  LangCode = LanguageString;
  while (*LangCode != 0) {
    GetNextLanguage (&LangCode, Lang);	
    if (AsciiStriCmp (Lang, "uqi") == 0) {
      continue;
    }

    String = HiiGetString (OldHiiHandle, StrId, Lang);
    if (String != NULL) {
      NewStrId = HiiSetString (NewHiiHandle, NewStrId, String, Lang);
      FreePool (String);
    } else {
      NewStrId = HiiSetString (NewHiiHandle, NewStrId, EnglishString, Lang);
	  }
  }

  FreePool (LanguageString);
  FreePool (Lang);
  FreePool (EnglishString);

  return NewStrId;
}




EFI_STRING_ID
SetEnString (
  EFI_HII_HANDLE    NewHiiHandle,
  EFI_HII_HANDLE    OldHiiHandle,
  EFI_STRING_ID     StrId
  )
{
  EFI_STRING    EnglishString;  
//EFI_STRING    String;  
  EFI_STRING_ID    NewStrId;
  CHAR8    *LanguageString;
  CHAR8    *LangCode;
  CHAR8    *Lang;


  if (NewHiiHandle == NULL || OldHiiHandle == NULL || StrId == 0) {
    DEBUG((EFI_D_ERROR, "%a(%X,%X,%X)\n", __FUNCTION__, NewHiiHandle, OldHiiHandle, StrId));
    return 0;
  }

  LanguageString = HiiGetSupportedLanguages(NewHiiHandle);
  if (LanguageString == NULL) {
    DEBUG((EFI_D_ERROR, "NoSupportLang\n"));
    return 0;
  }

  Lang = AllocatePool(AsciiStrSize(LanguageString));
  if (Lang == NULL) {
    DEBUG((EFI_D_ERROR, "Alloc Err %a\n", LanguageString));
    return 0;
  }

  EnglishString = HiiGetString(OldHiiHandle, StrId, "en-US");
  if (EnglishString == NULL) {
    DEBUG((EFI_D_ERROR, "NoEnglish\n"));
    return 0;
  }

  DEBUG((EFI_D_INFO, "%s %a\n", EnglishString, LanguageString));

  NewStrId = 0;
  LangCode = LanguageString;
  while (*LangCode != 0) {
    GetNextLanguage (&LangCode, Lang);	
    if (AsciiStriCmp (Lang, "uqi") == 0) {
      continue;
    }
    NewStrId = HiiSetString (NewHiiHandle, NewStrId, EnglishString, Lang);
  }

  FreePool (LanguageString);
  FreePool (Lang);
  FreePool (EnglishString);

  return NewStrId;
}





EFI_STATUS AddDynamicFormset()
{
  EFI_STATUS             Status;
  HII_FORMSET_GOTO_INFO  *Info;
  UINTN                  InfoCount;
  UINTN                  Index;
  EFI_STRING_ID          Title;
  EFI_STRING_ID          Help;
//VOID                   *DummyIf;
  UINTN                  Action;
  ADD_LABEL_CTX          AdvCtx;
  ADD_LABEL_CTX          SecuCtx;
  ADD_LABEL_CTX          *CurCtx; 
  UINTN                  DevHiiCount = 0;
  EFI_HII_HANDLE         *DyHandles = NULL;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  ZeroMem(&AdvCtx, sizeof(AdvCtx));
  ZeroMem(&SecuCtx, sizeof(SecuCtx));  

//Status = gBS->LocateProtocol(
//                &gEfiNetworkInterfaceIdentifierProtocolGuid_31,
//                NULL,
//                (VOID**)&DummyIf
//                );
//if(EFI_ERROR(Status)){
//  gFormsetAddActionList[0].Action = FORMSET_ADD_IGNORE;
//}

  Status = GetAllHiiFormset(&Info, &InfoCount);
  if(EFI_ERROR(Status) || InfoCount == 0){
    goto ProcExit;
  } 

  ShowAllHiiFormset(Info, InfoCount);

  Status = InitAddLabelCtx(&SecuCtx, &gEfiFormsetGuidSecurity, SECURITY_DYNAMIC_LABEL, SECURITY_DYNAMIC_LABEL_END);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  } 
  Status = InitAddLabelCtx(&AdvCtx, &gEfiFormsetGuidAdvance, ADV_DYNAMIC_LABEL, ADV_DYNAMIC_LABEL_END);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  } 

  DyHandles = AllocateZeroPool((InfoCount+1) * sizeof(EFI_HII_HANDLE));
  ASSERT(DyHandles != NULL);  

  for(Index=0;Index<InfoCount;Index++){

    Action = GetFormsetAddAction(&Info[Index].FormsetGuid);
    if(Action == FORMSET_ADD_IGNORE){
      continue;        
    } else if(Action == FORMSET_ADD_ADVANCE){
      CurCtx = &AdvCtx;
      DyHandles[DevHiiCount++] = Info[Index].HiiHandle;
    } else if(Action == FORMSET_ADD_SECURITY){
      CurCtx = &SecuCtx;
    } else {
      continue;
    }

    if(Action == FORMSET_ADD_SECURITY){
      Title = SetStringWithSysLanguages(CurCtx->HiiHandle, Info[Index].HiiHandle, Info[Index].FormSetTitle);
      Help  = SetStringWithSysLanguages(CurCtx->HiiHandle, Info[Index].HiiHandle, Info[Index].FormSetHelp);
    } else {
      Title = SetEnString(CurCtx->HiiHandle, Info[Index].HiiHandle, Info[Index].FormSetTitle);
      Help  = SetEnString(CurCtx->HiiHandle, Info[Index].HiiHandle, Info[Index].FormSetHelp);
    }
    if(Title == 0 || Help == 0){
      continue;
    }
    HiiCreateGotoExOpCode (
      CurCtx->StartOpCodeHandle,
      0,
      Title,
      Help,
      0,
      (EFI_QUESTION_ID)(0xFA00 + Index),
      GenerateUniqueQuestionId (Info[Index].HiiHandle, Info[Index].FormSetTitle),
      &Info[Index].FormsetGuid,
      0
      );
  }

  CurCtx = &AdvCtx;
  Status = HiiUpdateForm (
             CurCtx->HiiHandle,
             &CurCtx->FormsetGuid,
             ADV_DYNAMIC_FORM_ID,
             CurCtx->StartOpCodeHandle,
             CurCtx->EndOpCodeHandle
             );
  DEBUG((EFI_D_INFO, "HiiUpdateForm:%r\n", Status));

  if(DevHiiCount){
    DyHandles[DevHiiCount] = NULL;
    Status = gBS->InstallProtocolInterface(&gImageHandle, &gByoDymamicFormsetHiiListProtocolGuid, EFI_NATIVE_INTERFACE, DyHandles);
    ASSERT(!EFI_ERROR(Status));
  } else {
    if(DyHandles != NULL){
      FreePool(DyHandles);
    }
  }

  CurCtx = &SecuCtx;
  Status = HiiUpdateForm (
             CurCtx->HiiHandle,
             &CurCtx->FormsetGuid,
             ROOT_FORM_ID,
             CurCtx->StartOpCodeHandle,
             CurCtx->EndOpCodeHandle
             );
  DEBUG((EFI_D_INFO, "HiiUpdateForm:%r\n", Status));

ProcExit:
  FreeAddLabelCtx(&AdvCtx);
  FreeAddLabelCtx(&SecuCtx);
  return Status;
}




EFI_BYO_PLATFORM_SETUP_PROTOCOL    *gByoSetup = NULL;
EFI_HII_HANDLE                     gHiiHandle = NULL;

VOID ClearPendingKeys()
{
	EFI_INPUT_KEY  Key;
  UINTN          Count = 0;

	while(1){
    Count = 0;
    if(gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) != EFI_SUCCESS){
      break;
    }
    Count++;
    if(Count >= 100){
      break;
    }
  };
}


EFI_STATUS 
EFIAPI 
EfiBootManagerBootSetup(
    EFI_BDS_BOOT_MANAGER_PROTOCOL      *This
  )
{
  EFI_STATUS                    Status;
  SETUP_VOLATILE_DATA           SetupVData;
  UINTN                         VariableSize;
  EFI_SYSTEM_PASSWORD_PROTOCOL  *SystemPassword;
  UINT8                         UserType;
  EFI_HII_HANDLE                HiiHandle;
  CHAR16                        *Str;


  DEBUG((EFI_D_INFO, "[Setup]\n"));

  if(gAfterReadyToBoot){

    HiiHandle = HiiAddPackages (
                  &gEfiCallerIdGuid,
                  NULL,
                  UiAppStrings,
                  NULL
                  );
    ASSERT(HiiHandle != NULL);

    Str = HiiGetString(HiiHandle, STRING_TOKEN(STR_SETUP_CANNOT_ENTER), NULL);
    
    UiConfirmDialog (
      DIALOG_INFO, 
      NULL, 
      NULL, 
      TEXT_ALIGIN_CENTER,
      L"",
      Str,
      L"",
      NULL
      );

    FreePool(Str);
    HiiRemovePackages(HiiHandle);
    return EFI_UNSUPPORTED;
  }

  This->ConnectAll();
  This->RefreshOptions();

  UserType = LOGIN_USER_ADMIN;
  Status = gBS->LocateProtocol (
                  &gEfiSystemPasswordProtocolGuid,
                  NULL,
                  (VOID**)&SystemPassword
                  );
  if(!EFI_ERROR(Status)){
    UserType = SystemPassword->GetEnteredType();
    DEBUG((EFI_D_INFO, "UserType:%d\n", UserType));
  }


  VariableSize = sizeof(SetupVData);
  Status = gRT->GetVariable (
                  SETUP_VOLATILE_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupVData
                  );
  if(EFI_ERROR(Status)){
    VariableSize = sizeof(SetupVData);
    ZeroMem(&SetupVData, VariableSize);
  }

  if(UserType == LOGIN_USER_POP){
    SetupVData.LoginUserType = 1;
  } else {
    SetupVData.LoginUserType = 0;
  }

  Status = gRT->SetVariable (
                  SETUP_VOLATILE_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  VariableSize,
                  &SetupVData
                  );
  DEBUG((EFI_D_INFO, "SetVar(%s):%r\n", SETUP_VOLATILE_VARIABLE_NAME, Status));

  Status = gBS->InstallProtocolInterface (
                  &gImageHandle,
                  &gEfiSetupEnterGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );  
  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_PC_USER_SETUP);  

  BmInit(This);

  Status = gBS->LocateProtocol (
                  &gEfiByoPlatformSetupGuid,
                  NULL,
                  (VOID**)&gByoSetup
                  );
  DEBUG((EFI_D_INFO, "ByoPlatformSetup:%r\n", Status));
  ASSERT(!EFI_ERROR(Status));

  UiAppAddBaseFormset();
  AddDynamicFormset();  
  gByoSetup->InitializeMainFormset(gByoSetup);
  ClearPendingKeys();  
  gByoSetup->Run(gByoSetup);
 
  ByoFreeBMPackage();

  gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  CpuDeadLoop();

  return EFI_SUCCESS;
}


VOID UiAppCollectHiiResourceHook()
{
  STATIC BOOLEAN                   RunBefore = FALSE;

  if (RunBefore) {
    return;
  }
  AddBootHiiPackages(NULL);      // for SCU tool
  RunBefore = TRUE;
}


VOID
EFIAPI
UiAppReadyToBoot (
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__)); 
  gBS->CloseEvent(Event);

  gAfterReadyToBoot = TRUE;
}



EFI_STATUS
EFIAPI
UiAppEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_BDS_BOOT_MANAGER_PROTOCOL *BdsBootMgr;
  EFI_EVENT                     Event;
  

  ASSERT(PcdGet8(PcdBdsBootOrderUpdateMethod) == 2);

  Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, &BdsBootMgr);
  ASSERT(!EFI_ERROR(Status));            // dpx
  if(EFI_ERROR(Status)){
    return Status;
  }  

  BdsBootMgr->BootSetup = EfiBootManagerBootSetup;

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gPlatCollectHiiResourceHookGuid, UiAppCollectHiiResourceHook,
                  NULL
                  );

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             UiAppReadyToBoot,
             NULL,
             &Event
             ); 
  ASSERT_EFI_ERROR(Status);  

  return EFI_SUCCESS;
}




