/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By: 
  winddy_zhang

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/


#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/ByoDiskInfoProtocol.h>
#include "HddBindDxe.h"
#include <Guid/MdeModuleHii.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiLib.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/ByoPlatformSetupProtocol.h>
#include <Library/UefiHiiServicesLib.h>
#include <Protocol/ByoBmBootHookProtocol.h>
#include <ByoBootGroup.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/PciIo.h>
#include <Library/ByoCommLib.h>
#include <Library/SetupUiLib.h>
#include <Protocol/DevicePathToText.h>
#include <Library/LegacyBootOptionalDataLib.h>
#include <Protocol/UefiBbsRelationInf.h>
#include <SetupVariable.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/LegacyBootOptionalDataLib.h>
#include <Protocol/UefiBbsRelationInf.h>
#include <SetupVariable.h>


#define   HDDBIND_DEBUG(a)        DEBUG(a)

extern unsigned char HddBindDxeVfrBin[];
extern HDD_BIND_CTX  gHddBindPrivate;


VOID GetHddBindNvData();

STATIC EFI_GUID gHddBindNvVarGuid = HDD_BIND_NV_GUID;


VOID 
InitString (
  EFI_HII_HANDLE    HiiHandle, 
  EFI_STRING_ID     StrRef, 
  CHAR16            *sFormat, ...
  )
{
  STATIC CHAR16 s[1024];
  VA_LIST  Marker;

  VA_START (Marker, sFormat);
  UnicodeVSPrint (s, sizeof (s),  sFormat, Marker);
  VA_END (Marker);
    
  HiiSetString (HiiHandle, StrRef, s, NULL);
}

VOID HddBindCreateForm()
{
  VOID                  *StartOpCodeHandle = NULL;
  VOID                  *EndOpCodeHandle   = NULL;
  EFI_IFR_GUID_LABEL    *StartLabel;
  EFI_IFR_GUID_LABEL    *EndLabel;
  UINTN                 Index;
  HDD_BIND_INFO         *p;
  VOID                  *OptionsOpCodeHandle = NULL;
  BOOLEAN               UnPlug = FALSE;
  UINT16                StringToken;
  CHAR16                TmpString [256];

  StartOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_HDD_BIND_START;

  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_HDD_BIND_END;

  OptionsOpCodeHandle = HiiAllocateOpCodeHandle();

  HiiCreateOneOfOptionOpCode (
    OptionsOpCodeHandle,
    STRING_TOKEN(STR_NOBIND),
    0,
    EFI_IFR_TYPE_NUM_SIZE_16,
    0
    );

  for(Index=0;Index<gHddBindPrivate.HddBindInfoCount;Index++){
    p = &gHddBindPrivate.HddBindInfo[Index];

    if(StrStr(p->Mn,L"ASMT106x")!=NULL){ //SKIP Asmedia Raid
      continue;
	}
    ZeroMem(TmpString, sizeof(TmpString));
    UnicodeSPrint(TmpString, sizeof(TmpString), L"B%dD%dF%d %s %a", p->Bus, p->Dev, p->Fun, p->Mn, p->Sn);
    DEBUG((EFI_D_ERROR, __FUNCTION__"(), TmpString :%s.\n", TmpString));

    StringToken = HiiSetString(gHddBindPrivate.HiiHandle, 0, TmpString, NULL); 
    HiiCreateOneOfOptionOpCode (
      OptionsOpCodeHandle,
      StringToken,
      0,
      EFI_IFR_TYPE_NUM_SIZE_16,
      Index+1
      );

    if(gHddBindPrivate.VarData.HddBind == 0 &&
       gHddBindPrivate.TargetSn != NULL && AsciiStrCmp(gHddBindPrivate.TargetSn, p->Sn) == 0){
      gHddBindPrivate.VarData.HddBind = (UINT16)(Index+1);
    }
    
  }

  if(gHddBindPrivate.TargetSn != NULL && gHddBindPrivate.VarData.HddBind == 0){
    gHddBindPrivate.VarData.HddBind = 1;
    UnPlug = TRUE;
  }

//-  if(gHddBindPrivate.TargetSn == NULL){
//-    HiiSetString(gHddBindPrivate.HiiHandle, STRING_TOKEN(STR_CUR_BOOT_HDD_VALUE), L"No Limit", NULL);
//-  } else if(UnPlug){
//-    UnicodeSPrint(Str, sizeof(Str), L"%s(%s)", gHddBindPrivate.TargetMn, L"UnPluged");
//-    HiiSetString(gHddBindPrivate.HiiHandle, STRING_TOKEN(STR_CUR_BOOT_HDD_VALUE), Str, NULL);
//-  } else {
//-    HiiSetString(gHddBindPrivate.HiiHandle, STRING_TOKEN(STR_CUR_BOOT_HDD_VALUE), gHddBindPrivate.TargetMn, NULL);
//-  }

  if(gHddBindPrivate.TargetSn == NULL){
    gHddBindPrivate.VarData.CurBinded = 0;
  } else {
    gHddBindPrivate.VarData.CurBinded = 1;
    InitString(gHddBindPrivate.HiiHandle, STRING_TOKEN(STR_CUR_BOOT_HDD_VALUE),L"%s SN:%a" , gHddBindPrivate.TargetMn,gHddBindPrivate.TargetSn);    
  }

  HiiCreateOneOfOpCode (
    StartOpCodeHandle,
    HDD_BIND_ONEOF_ITEM_ID,
    HDD_BIND_VARSTORE_ID,
    OFFSET_OF(HDD_BIND_VAR_DATA, HddBind),
    STRING_TOKEN(STR_HDD_BIND_ITEM_NAME),
    STRING_TOKEN(STR_HDD_BIND_ITEM_HELP),
    EFI_IFR_FLAG_CALLBACK,
    EFI_IFR_NUMERIC_SIZE_2,                        // <--- !!!
    OptionsOpCodeHandle,
    NULL
    );

  HiiUpdateForm (
    gHddBindPrivate.HiiHandle,
    &gByoHddBindFormsetGuid,        // Formset GUID
    HDD_BIND_FORM_ID,               // Form ID
    StartOpCodeHandle,              // Label for where to insert opcodes
    EndOpCodeHandle                 // Replace data
    );

  if(StartOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(StartOpCodeHandle);
  }
  if(EndOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(EndOpCodeHandle);
  }
  if(OptionsOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(OptionsOpCodeHandle);
  }  
}



VOID
EFIAPI
HddBindEnterSetupCallback (
  IN EFI_EVENT    Event,
  IN VOID*        Context
  )
{
  EFI_STATUS            Status;
  VOID                  *Interface;

  Status = gBS->LocateProtocol(&gEfiSetupEnterGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  GetHddBindNvData();  
  HddBindCreateForm();

}




HII_VENDOR_DEVICE_PATH  gHddBindHiiVendorDevicePath = {
  {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        {
          (UINT8) (sizeof (HII_VENDOR_DEVICE_PATH_NODE)),
          (UINT8) ((sizeof (HII_VENDOR_DEVICE_PATH_NODE)) >> 8)
        }
      },
      SETUP_HDD_BIND_FORMSET_GUID,
    },
    0,
    (UINTN)&gHddBindHiiVendorDevicePath
  },
  
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    { 
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};







EFI_STATUS
EFIAPI
HddBindExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS         Status;
  UINTN              BufferSize;
  HDD_BIND_CTX       *Private;
  EFI_STRING         ConfigRequestHdr;
  EFI_STRING         ConfigRequest;
  BOOLEAN            AllocatedRequest;
  UINTN              Size;


  HDDBIND_DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;

  if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, &gByoHddBindFormsetGuid, HDD_BIND_VAR_NAME)) {
    return EFI_NOT_FOUND;
  }

  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  AllocatedRequest = FALSE;
  Size             = 0;

  Private = HDD_BIND_DATA_FROM_THIS(This);
  
  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  BufferSize = sizeof(HDD_BIND_VAR_DATA);
  ConfigRequest = Request;
  if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request has no request element, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr (&gByoHddBindFormsetGuid, HDD_BIND_VAR_NAME, Private->DriverHandle);
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    FreePool (ConfigRequestHdr);
  }

  HDDBIND_DEBUG((EFI_D_INFO, "BufferSize:%d\n", BufferSize));

  Status = gHiiConfigRouting->BlockToConfig (
                                gHiiConfigRouting,
                                ConfigRequest,
                                (UINT8*)&Private->VarData,
                                BufferSize,
                                Results,
                                Progress
                                );
  //
  // Free the allocated config request string.
  //
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
    *Progress = Request + StrLen (Request);
  }

  return Status;
}





EFI_STATUS SaveHddBindVar(UINT16 HddBind)
{
  EFI_STATUS      Status;
  HDD_BIND_INFO   *Info;
  UINTN           Size;
  CHAR16          *p;
  UINTN           a, b;


  HDDBIND_DEBUG((EFI_D_INFO, "%a() %d\n", __FUNCTION__, HddBind));

  if(HddBind == 0){
    Status = gRT->SetVariable(  
                    HDD_BIND_NV_NAME, 
                    &gHddBindNvVarGuid, 
                    0,
                    0,
                    NULL
                    );
    HDDBIND_DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
    
  } else {
    ASSERT(HddBind <= gHddBindPrivate.HddBindInfoCount);
    Info = &gHddBindPrivate.HddBindInfo[HddBind-1];
    a    = StrSize(Info->Mn);
    b    = AsciiStrSize(Info->Sn);
    Size = a + b;
    p = (CHAR16*)AllocateZeroPool(Size);
    if(p == NULL){
      Status = EFI_OUT_OF_RESOURCES;
    } else {
      CopyMem(p, Info->Mn, a);
      CopyMem((VOID*)((UINTN)p + a), Info->Sn, b);
      Status = gRT->SetVariable(  
                      HDD_BIND_NV_NAME, 
                      &gHddBindNvVarGuid, 
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      Size,
                      p
                      );
      HDDBIND_DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));  
      FreePool(p);
    }
  }

  return Status;
}




EFI_STATUS
EFIAPI
HddBindRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  EFI_STATUS                       Status;
  HDD_BIND_CTX                     *Private;
  HDD_BIND_VAR_DATA                VarData;
  UINTN                            Size;
  

  HDDBIND_DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  

  Private = HDD_BIND_DATA_FROM_THIS(This);
  *Progress = Configuration;

  //
  // Check routing data in <ConfigHdr>.
  // Note: if only one Storage is used, then this checking could be skipped.
  //
  if (!HiiIsConfigHdrMatch (Configuration, &gByoHddBindFormsetGuid, HDD_BIND_VAR_NAME)) {
    return EFI_NOT_FOUND;
  }

//SerialPortWrite((UINT8*)Configuration, StrSize(Configuration));

  //
  // Convert <ConfigResp> to buffer data by helper function ConfigToBlock()
  //
  Size = sizeof(HDD_BIND_VAR_DATA);
  Status = gHiiConfigRouting->ConfigToBlock (
                                gHiiConfigRouting,
                                Configuration,
                                (UINT8*)&VarData,
                                &Size,
                                Progress
                                );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "%a() ConfigToBlock:%r\n", __FUNCTION__, Status));
    return Status;
  }

  //
  // Store Buffer Storage back to EFI variable
  //
  DEBUG((EFI_D_INFO, "HddBind:%d\n", VarData.HddBind));
  if(Private->UserChoice){
    SaveHddBindVar(VarData.HddBind);
  }else{
    SaveHddBindVar(0);
  }

  return Status;
}


EFI_STATUS
EFIAPI
HddBindCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL         *This,
  IN        EFI_BROWSER_ACTION                     Action,
  IN        EFI_QUESTION_ID                        QuestionId,
  IN        UINT8                                  Type,
  IN        EFI_IFR_TYPE_VALUE                     *Value,
  OUT       EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  DEBUG((EFI_D_INFO, "HddBindCallback A:%X Q:%X T:%d AR:%d\n", Action, QuestionId, Type, *ActionRequest));

  if (Action == EFI_BROWSER_ACTION_FORM_OPEN || Action == EFI_BROWSER_ACTION_FORM_CLOSE) {
    return EFI_SUCCESS;
  }

  if(Action == EFI_BROWSER_ACTION_CHANGED){
    if(QuestionId == HDD_BIND_ONEOF_ITEM_ID){
      DEBUG((EFI_D_INFO, "V:%d\n", Value->u16));
      gHddBindPrivate.UserChoice = TRUE;
      return EFI_SUCCESS;
    }
  } else if(Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD){
    if(QuestionId == HDD_BIND_ONEOF_ITEM_ID){
      gHddBindPrivate.UserChoice = FALSE;
    }
  }

  return EFI_UNSUPPORTED;
}





HDD_BIND_CTX gHddBindPrivate = {
  HDD_BIND_CTX_SIGNATURE,               // Signature
  NULL,                                 // HiiHandle
  NULL,                                 // DriverHandle
  {                                     // ConfigAccess
    HddBindExtractConfig,
    HddBindRouteConfig,
    HddBindCallback
  },
  {                                     // VarData
    0,
  },
  NULL,                                 // HddBindInfo
  0,                                    // HddBindInfoCount
  0xFFFF,                               // TagetHddIndex
  NULL,                                 // HddBindNv
  0,                                    // HddBindNvSize
  FALSE,                                // GetHddBindNv
  FALSE,                                // UserChoice
  NULL,                                 // TargetSn
  NULL,                                 // TargetMn
};



VOID GetHddBindNvData()
{
  EFI_STATUS   Status;

  HDDBIND_DEBUG((EFI_D_INFO, "%a() %d\n", __FUNCTION__, gHddBindPrivate.GetHddBindNv));

  if(!gHddBindPrivate.GetHddBindNv){
    Status = GetVariable2(
               HDD_BIND_NV_NAME, 
               &gHddBindNvVarGuid, 
               &gHddBindPrivate.HddBindNv,
               &gHddBindPrivate.HddBindNvSize
               );
    HDDBIND_DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));    
    if(EFI_ERROR(Status)){
      gHddBindPrivate.HddBindNv = NULL;
      gHddBindPrivate.HddBindNvSize = 0;
    } else {
      gHddBindPrivate.TargetMn = (CHAR16*)gHddBindPrivate.HddBindNv;
      gHddBindPrivate.TargetSn = (CHAR8*)((UINTN)gHddBindPrivate.HddBindNv + StrSize(gHddBindPrivate.TargetMn));
      HDDBIND_DEBUG((EFI_D_INFO, "(L%d) [%s][%a]\n", __LINE__, gHddBindPrivate.TargetMn, gHddBindPrivate.TargetSn)); 
    }
  }
  gHddBindPrivate.GetHddBindNv = TRUE;
}



void ShowBindHddBlockDlg(BOOLEAN IsUefi)
{
  CHAR16  *Title, *Content;
  //STATIC  BOOLEAN Flag[2] = {0, 0};

 /* if(IsUefi){
    if(Flag[1]){
      return;
    } else {
      Flag[1] = 1;
    }
  } else {
    if(Flag[0]){
      return;
    } else {
      Flag[0] = 1;
    }
  }*/

  BltSaveAndRetore(gBS, TRUE);
  gST->ConOut->ClearScreen(gST->ConOut);

  Title = HiiGetString(gHddBindPrivate.HiiHandle, STRING_TOKEN(STR_HDD_BIND_BLOCK_TITLE), NULL);
  Content = HiiGetString(gHddBindPrivate.HiiHandle, STRING_TOKEN(STR_HDD_BIND_BLOCK_INFO), NULL);
  UiConfirmDialog(DIALOG_WARNING, Title, NULL, TEXT_ALIGIN_CENTER, Content, NULL);
  FreePool(Title);
  FreePool(Content);

  BltSaveAndRetore(gBS, FALSE);
}






/**
  This function converts an input device structure to a Unicode string.

  @param DevPath                  A pointer to the device path structure.

  @return A new allocated Unicode string that represents the device path.

**/
CHAR16 *
DevicePathToString (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath
  )
{
  EFI_STATUS                       Status;
  CHAR16                           *ToText;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;

  if (DevPath == NULL) {
    return NULL;
  }

  Status = gBS->LocateProtocol (
                  &gEfiDevicePathToTextProtocolGuid,
                  NULL,
                  (VOID **) &DevPathToText
                  );
  ASSERT_EFI_ERROR (Status);
  ToText = DevPathToText->ConvertDevicePathToText (
                            DevPath,
                            FALSE,
                            TRUE
                            );
  ASSERT (ToText != NULL);
  return ToText;
}




// [FS]       PciRoot(0x0)/Pci(0x3,0x0)/Pci(0x0,0x0)/Pci(0x8,0x0)/Pci(0x0,0x0)/Pci(0xF,0x0)/Sata(0x2,0xFFFF,0x0)/
//            HD(2,GPT,5F343B3C-ADE7-4F05-8FF0-6A2A0AC55B86,0xFA000,0x32000)
// [WIN_NV]   HD(2,GPT,5F343B3C-ADE7-4F05-8FF0-6A2A0AC55B86,0xFA000,0x32000)/\EFI\Microsoft\Boot\bootmgfw.efi
// [BIOS_NV]  PciRoot(0x0)/Pci(0x3,0x0)/Pci(0x0,0x0)/Pci(0x8,0x0)/Pci(0x0,0x0)/Pci(0xF,0x0)/Sata(0x2,0xFFFF,0x0)
BOOLEAN
CheckOsDp (
	IN	EFI_DEVICE_PATH_PROTOCOL	   *Dp,
	OUT EFI_DEVICE_PATH_PROTOCOL       **Bios_NvDP
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *FsDp = NULL;
  EFI_DEVICE_PATH_PROTOCOL      *TempDp;
  EFI_STATUS                    Status;
  UINTN                         HandleCount;
  EFI_HANDLE                    *Handles;
  UINTN                         Index;
  UINT16						*DpString;
  UINT16						*TempString;
  UINT16						*FsString;
  BOOLEAN						OSPathMath = FALSE;

  if (DevicePathType(Dp) == MEDIA_DEVICE_PATH && DevicePathSubType(Dp) == MEDIA_HARDDRIVE_DP) {

    Status = gBS->LocateHandleBuffer (
                     ByProtocol,
                     &gEfiSimpleFileSystemProtocolGuid,
                     NULL,
                     &HandleCount,
                     &Handles
                     );
    if(EFI_ERROR(Status)){
      HandleCount = 0;
    }
    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol (
                      Handles[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID**)&FsDp
                      );
      if(EFI_ERROR(Status)){
        continue;
      }

	  FsString = DevicePathToString(FsDp);
	  DEBUG((EFI_D_INFO, "FsString1  %s\n", FsString)); 

      TempDp = FsDp;
      while(!IsDevicePathEnd(TempDp)){
        if(DevicePathType(TempDp) == MEDIA_DEVICE_PATH && DevicePathSubType(TempDp) == MEDIA_HARDDRIVE_DP){
          break;
        }
        TempDp = NextDevicePathNode(TempDp);
      }
      if(IsDevicePathEnd(TempDp)){
        continue;
      }
      
	  DpString = DevicePathToString(Dp);
	  DEBUG((EFI_D_INFO, "NvString  %s\n", DpString)); 
 
      
	  TempString = DevicePathToString(TempDp);
	  DEBUG((EFI_D_INFO, "TempString  %s\n", TempString)); 
	  
      if(CompareMem(Dp, TempDp, DevicePathNodeLength(Dp)) != 0){        // node match ?
        continue;
      }

      DEBUG((EFI_D_INFO, "target HD found\n"));
	  OSPathMath = TRUE;
      break;
    }

  }
  FsString = DevicePathToString(FsDp);
  *Bios_NvDP = FsDp;
  DEBUG((EFI_D_INFO, "FsString2	%s\n", FsString)); 
  
  return OSPathMath;
}




VOID
HddBindByoBmBootHook (
  VOID  *Param   
  )
{
  BYO_BM_BOOT_HOOK_PARAMETER     *p;
  EFI_STATUS                     Status;
  EFI_DEVICE_PATH_PROTOCOL       *Dp;
  EFI_DEVICE_PATH_PROTOCOL       *Bios_Dp=NULL;
  EFI_HANDLE                     DevHandle;
  BYO_DISKINFO_PROTOCOL          *ByoDiskInfo;
  UINTN                          Size;
  BOOLEAN                        Match = FALSE;
  UINT8                          Sn[21];
  UINT16							*SataString;
  BOOLEAN						OSHddMatch = FALSE;
  UINT16                        BbsIndex; 
  UINTN                         Index;
  UEFI_BBS_RELATION_INFO_PROTOCOL	*ptUefiBbsInfo;
  UefiBbsHddRL 						*StaticUefiSataHdd;
  SETUP_DATA                    SetupData;

  HDDBIND_DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  
  p = (BYO_BM_BOOT_HOOK_PARAMETER*)Param;
  ASSERT(p->Sign == BYO_BM_BOOT_HOOK_PARAMETER_SIGN);

  GetHddBindNvData();
  if(gHddBindPrivate.TargetSn == NULL){
    return;
  }
  if((p->BootOption->GroupType != BM_MENU_TYPE_UEFI_HDD) && (p->BootOption->GroupType != BM_MENU_TYPE_LEGACY_MIN)){
    return;
  }

  DEBUG((EFI_D_INFO, "Description is %s\n",p->BootOption->Description));

  if(StrStr(p->BootOption->Description,L"MegaRAID")!=NULL){
    Match = FALSE;
	goto MatchHdd;
  }
  Size = sizeof(SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  );
  if( SetupData.BootModeType == 1){
  		Dp = p->BootOption->FilePath;
  		SataString = DevicePathToString(Dp);
  		HDDBIND_DEBUG((EFI_D_INFO, "Uefi SataString %s\n", SataString)); 
		if((SataString[0] == L'H') && (SataString[1] == L'D')){
			OSHddMatch = CheckOsDp(Dp,&Bios_Dp);
			if(OSHddMatch){
				Match = TRUE;
				Dp = Bios_Dp;
				goto MatchHdd;
				}
			}
  	}else{
  		BbsIndex = LegacyBootOptionalDataGetBbsIndex(p->BootOption->OptionalData); 
  		Status = gBS->LocateProtocol(&gUefiBbsRelationProtocolGuid, NULL, &ptUefiBbsInfo);
  		StaticUefiSataHdd = ptUefiBbsInfo->UefiBbsHddTable;
  	for(Index = 0; Index < ptUefiBbsInfo->UefiSataHddCount; Index++)	{
  		if( BbsIndex == StaticUefiSataHdd[Index].BbsIndex){
				Dp = StaticUefiSataHdd[Index].HddDevicePath;	
				SataString = DevicePathToString(Dp);
				HDDBIND_DEBUG((EFI_D_INFO, "Legacy SataString %s\n", SataString)); 
  				}
  		}
  	}
  
  Dp = p->BootOption->FilePath;
  Size = sizeof(SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  );
  
  if( SetupData.BootModeType == 1){
  Dp = p->BootOption->FilePath;
  	}
  else{
  		BbsIndex = LegacyBootOptionalDataGetBbsIndex(p->BootOption->OptionalData); 
  		Status = gBS->LocateProtocol(&gUefiBbsRelationProtocolGuid, NULL, &ptUefiBbsInfo);
  		StaticUefiSataHdd = ptUefiBbsInfo->UefiBbsHddTable;
  	for(Index = 0; Index < ptUefiBbsInfo->UefiSataHddCount; Index++)	{
  		if( BbsIndex == StaticUefiSataHdd[Index].BbsIndex){
				Dp = StaticUefiSataHdd[Index].HddDevicePath;	
  				}
  		}
  	}

MatchHdd:
  Status = gBS->LocateDevicePath(
                  &gByoDiskInfoProtocolGuid,
                  &Dp,
                  &DevHandle
                  );
  if(!EFI_ERROR(Status)){
    Status = gBS->HandleProtocol(
                    DevHandle,
                    &gByoDiskInfoProtocolGuid,
                    (VOID**)&ByoDiskInfo
                    );
    Size = sizeof(Sn);
    ByoDiskInfo->GetSn(ByoDiskInfo, Sn, &Size);
    if(AsciiStrCmp(gHddBindPrivate.TargetSn, Sn) == 0){
      HDDBIND_DEBUG((EFI_D_INFO, "Taget Sn Match\n"));
      Match = TRUE;
    } else {
      Match = FALSE;
      DEBUG((EFI_D_INFO, "Taget Sn Not Match, Deny\n"));  
    }
  }


  if(!Match){
    ShowBindHddBlockDlg(1);
    p->Status = EFI_ACCESS_DENIED;
  }
}



void DumpBbs(BBS_TABLE *BbsTable, UINTN BbsCount)
{
  UINTN       Index;
  BBS_TABLE   *p;
  CHAR8       *DescStr;

  for(Index=0;Index<BbsCount;Index++){
    p = &BbsTable[Index];
  	if(p->BootPriority == BBS_IGNORE_ENTRY || p->BootPriority == BBS_DO_NOT_BOOT_FROM){
  	  continue;
  	}

  	DescStr = (CHAR8*)(UINTN)((p->DescStringSegment << 4) + p->DescStringOffset);
  	if(DescStr == (CHAR8*)(UINTN)0){
  		DescStr = " ";
  	}
    
    HDDBIND_DEBUG((EFI_D_INFO, "[%03d] %X (%02X,%02X,%02X) C(%02X,%02X) T:%02X [%a]\n", \
      Index, p->BootPriority, p->Bus, p->Device, p->Function, p->Class, p->SubClass, p->DeviceType, DescStr));
  }
}









VOID
HddBindAfterRefreshBbs (
  VOID
)
{
	EFI_STATUS                		Status;
  EFI_LEGACY_BIOS_PROTOCOL  		*ptLegacyBios;
  UINT16                    		HddCount;
  HDD_INFO                  		*HddInfo;
  UINT16                    		BbsCount;
  BBS_TABLE                 		*BbsTable;
  UINTN                     		Index,IndexHdd;
  UINT16                    		BootPriority;
  CHAR8                     		*DescStr;
  HDD_BIND_INFO             		*p;
  UEFI_BBS_RELATION_INFO_PROTOCOL	*ptUefiBbsInfo;
  UefiBbsHddRL 						*StaticUefiSataHdd;
  BOOLEAN							AsmediaHddBind = FALSE;


  HDDBIND_DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  GetHddBindNvData();
  if(gHddBindPrivate.TargetSn == NULL){
    return;
  }

  if(gHddBindPrivate.TagetHddIndex == 0xFFFF){
    p = NULL;
  } else {
    p = &gHddBindPrivate.HddBindInfo[gHddBindPrivate.TagetHddIndex];
  }
  
  Status = gBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, &ptLegacyBios);
  if(EFI_ERROR(Status)){
    return;
  }
  
  Status = ptLegacyBios->GetBbsInfo(ptLegacyBios, &HddCount, &HddInfo, &BbsCount, &BbsTable);
  if(EFI_ERROR(Status)){
    return;
  } 
  
//DumpBbs(BbsTable, BbsCount);
  
  for(Index=0;Index<BbsCount;Index++){
  	BootPriority = BbsTable[Index].BootPriority;
  	if(BootPriority == BBS_IGNORE_ENTRY || BootPriority == BBS_DO_NOT_BOOT_FROM){
  	  continue;
  	}
    if(BbsTable[Index].Class != 1 || BbsTable[Index].DeviceType == BBS_BEV_DEVICE){
      continue;
    }

  	DescStr = (CHAR8*)(UINTN)((BbsTable[Index].DescStringSegment << 4) + BbsTable[Index].DescStringOffset);
  	if(DescStr == (CHAR8*)(UINTN)0){
  		DescStr = " ";
  	}

    if(p == NULL){
      BbsTable[Index].BootPriority = BBS_IGNORE_ENTRY;
      DEBUG((EFI_D_INFO, "ignore %a (target unplug)\n", DescStr));
      ShowBindHddBlockDlg(0);
      continue;
    }

    if(BbsTable[Index].Bus != p->Bus || 
       BbsTable[Index].Device != p->Dev || 
       BbsTable[Index].Function != p->Fun){
      BbsTable[Index].BootPriority = BBS_IGNORE_ENTRY;
      DEBUG((EFI_D_INFO, "ignore %a(BDF not match)\n", DescStr));
      continue;      
    }

    if(BbsTable[Index].SubClass == 8){       // nvme do not need to check port index
      continue;
    }

// "SATA 2: "
// "SATA1-4"
    if(AsciiStrnCmp(DescStr, "SATA ", 5) == 0 && DescStr[5] >= '0' && DescStr[5] <= '9' && p->Port == (DescStr[5] - '0')){
      continue;
    }
    if(AsciiStrnCmp(DescStr, "SATA", 4) == 0 && DescStr[4] >= '0' && DescStr[4] <= '9' && DescStr[5] == '-' &&
    DescStr[6] >= '0' && DescStr[6] <= '9' && p->Port == (DescStr[6] - '0')){
      continue;
    }

	
	Status = gBS->LocateProtocol(&gUefiBbsRelationProtocolGuid, NULL, &ptUefiBbsInfo);
	
	if(!EFI_ERROR(Status)){
  		StaticUefiSataHdd = ptUefiBbsInfo->UefiBbsHddTable;
  		for(IndexHdd = 0; IndexHdd < ptUefiBbsInfo->UefiSataHddCount; IndexHdd++)	{
  		if(( Index == StaticUefiSataHdd[IndexHdd].BbsIndex) && (p->Port == StaticUefiSataHdd[IndexHdd].HddPortIndex)) {				
				HDDBIND_DEBUG((EFI_D_INFO, "asmedia hdd match! DescStr %a\n",DescStr));
				AsmediaHddBind = TRUE;
				break;
  			}
  		}
		if(AsmediaHddBind == TRUE)			
			continue;
	}
	Status = gBS->LocateProtocol(&gUefiBbsRelationProtocolGuid, NULL, &ptUefiBbsInfo);
	if(!EFI_ERROR(Status)){
  		StaticUefiSataHdd = ptUefiBbsInfo->UefiBbsHddTable;
  		for(IndexHdd = 0; IndexHdd < ptUefiBbsInfo->UefiSataHddCount; IndexHdd++)	{
  		if(( Index == StaticUefiSataHdd[IndexHdd].BbsIndex) && (p->Port == StaticUefiSataHdd[IndexHdd].HddPortIndex)) {				
				HDDBIND_DEBUG((EFI_D_INFO, "asmedia hdd match! DescStr %a\n",DescStr));
				AsmediaHddBind = TRUE;
				break;
  			}
  		}
		if(AsmediaHddBind == TRUE)			
			continue;
	}
    BbsTable[Index].BootPriority = BBS_IGNORE_ENTRY;
    DEBUG((EFI_D_INFO, "ignore %a(port not match p:%d)\n", DescStr, p->Port));
  }

  DEBUG_CODE_BEGIN();
  DumpBbs(BbsTable, BbsCount);
  DEBUG_CODE_END();

}



VOID HddBindAfterConsoleEnd()
{
  EFI_STATUS            Status;
  UINTN                 HandleCount;
  EFI_HANDLE            *Handles = NULL;
  UINTN                 Index;
  BYO_DISKINFO_PROTOCOL *ByoDiskInfo;
  HDD_BIND_INFO         *Info;
  HDD_BIND_INFO         *p;  
  UINTN                 Size;
  EFI_DEVICE_PATH_PROTOCOL   *MyDp; 
  EFI_HANDLE                 DevHandle;
  EFI_PCI_IO_PROTOCOL        *PciIo;
  UINTN                      Seg;
  UINT8                      Mn[41];
  UINTN 					PlatSataHostIndex = 0;
  UINTN 					AtaIndex, PhysicPortIndex;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  
  GetHddBindNvData();

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gByoDiskInfoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if(EFI_ERROR(Status) || HandleCount == 0){
    goto ProcExit;
  }

  Info = (HDD_BIND_INFO*)AllocateZeroPool(sizeof(HDD_BIND_INFO) * HandleCount);
  if(Info == NULL){
    goto ProcExit;
  }

  for(Index=0;Index<HandleCount;Index++){
    Status = gBS->HandleProtocol(
                    Handles[Index],
                    &gByoDiskInfoProtocolGuid,
                    (VOID**)&ByoDiskInfo
                    );
    p = &Info[Index];
    p->Type = ByoDiskInfo->DevType;
    ByoDiskInfo->GetDp(ByoDiskInfo, &p->Dp);
    Size = sizeof(Mn);
    ByoDiskInfo->GetMn(ByoDiskInfo, Mn, &Size);
    UnicodeSPrint(p->Mn, sizeof(p->Mn), L"%a", Mn);
    Size = sizeof(p->Sn);
    ByoDiskInfo->GetSn(ByoDiskInfo, p->Sn, &Size);

   		LibGetSataPortInfo(gBS, Handles[Index], &PlatSataHostIndex, &AtaIndex, &PhysicPortIndex);
      	p->Port = PhysicPortIndex;

    MyDp = p->Dp;
    Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &MyDp, &DevHandle);
    ASSERT(!EFI_ERROR(Status));
    Status = gBS->HandleProtocol(DevHandle, &gEfiPciIoProtocolGuid, &PciIo);
    ASSERT(!EFI_ERROR(Status));
    PciIo->GetLocation(PciIo, &Seg, &p->Bus, &p->Dev, &p->Fun);

    if(gHddBindPrivate.TagetHddIndex == 0xFFFF &&
       gHddBindPrivate.TargetSn != NULL && AsciiStrCmp(gHddBindPrivate.TargetSn, p->Sn) == 0){
      gHddBindPrivate.TagetHddIndex = Index;
    }

    HDDBIND_DEBUG((EFI_D_INFO, "[%d] p:%d %X %X %X [%s][%a] TT:%d\n", \
      Index, p->Port, p->Bus, p->Dev, p->Fun, p->Mn, p->Sn, gHddBindPrivate.TagetHddIndex));

  }

  gHddBindPrivate.HddBindInfo = Info;
  gHddBindPrivate.HddBindInfoCount = HandleCount;

ProcExit:
  if(Handles != NULL){
    FreePool(Handles);
  }  
}



EFI_STATUS
EFIAPI
HddBindDxeInit (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  VOID               *Registration;
  EFI_STATUS         Status;
  


  HDDBIND_DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  if(PcdGet8(PcdIsPlatformCmosBad) || PcdGetBool(PcdFceLoadDefault)){
     Status = gRT->SetVariable(  
                    HDD_BIND_NV_NAME, 
                    &gHddBindNvVarGuid, 
                    0,
                    0,
                    NULL
                    );
  }

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gHddBindPrivate.DriverHandle,
                  &gEfiDevicePathProtocolGuid,      &gHddBindHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid, &gHddBindPrivate.ConfigAccess,
                  &gByoBmBootHookProtocolGuid,      HddBindByoBmBootHook,
                  &gPlatAfterConsoleEndProtocolGuid, HddBindAfterConsoleEnd,
                  &gByoAfterRefreshBbsHookProtocolGuid, HddBindAfterRefreshBbs,
                  &gEfiCallerIdGuid, NULL,
                  NULL
                  );
  ASSERT(!EFI_ERROR(Status));

  gHddBindPrivate.HiiHandle = HiiAddPackages (
                         &gByoHddBindFormsetGuid,
                         gHddBindPrivate.DriverHandle,
                         STRING_ARRAY_NAME,
                         HddBindDxeVfrBin,
                         NULL
                         );
  ASSERT(gHddBindPrivate.HiiHandle != NULL);
  
  EfiCreateProtocolNotifyEvent (
    &gEfiSetupEnterGuid,
    TPL_CALLBACK,
    HddBindEnterSetupCallback,
    NULL,
    &Registration
    );

  return EFI_SUCCESS;

}


