/*++

  Copyright (c) 2006 - 2019 Byosoft Corporation. All rights reserved.
  This program and associated documentation (if any) is furnished
  under a license. Except as permitted by such license,no part of this
  program or documentation may be reproduced, stored divulged or used
  in a public system, or transmitted in any form or by any means
  without the express written consent of Byosoft Corporation.

Module Name:
  PostError.c

Abstract:
  Implements the programming of Post Error.

Revision History:

--*/

#include <Uefi.h>
#include <Guid/SetupGuiCustom.h>
#include <Guid/MdeModuleHii.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/SetupUiLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/ByoBootManagerLib.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/PciIo.h>
#include <Protocol/HiiConfigAccess.h>
#include <IndustryStandard/Pci.h>
#include <Library/PlatformPciListFilterLib.h>
#include <Library/PciSegmentLib.h>
#include "PciListDxe.h"
#include "PciRootPort.h"


EFI_HII_HANDLE  gHiiHandle;
EFI_GUID gByoSetupPciListFormsetGuid = SETUP_PCI_LIST_FORMSET_GUID;
EFI_GUID gByoSetupPciRootPortFormsetGuid = SETUP_PCI_ROOT_PORT_FORMSET_GUID;
extern unsigned char PciListBin[];
extern unsigned char PciRootPortBin[];
PLATFORM_PCI_LIST_FILTER_HOOK_FUNC gPlatformPciListFilterFunc = NULL;
UINTN                              mRootPortIndex = 0;
PCI_IO_CTX                         *mPciIoCtx = NULL;
PCI_IO_DISPLAY_STR                 *mPciDisplayStr = NULL;

#define SUPPORTED_MAX_LENGTH            20
typedef struct {
  UINT16   VendorId;
  UINT16   DeviceId;
  CHAR16   *DeviceName;
} PCI_DEV_ID_NAME;

CHAR16 *GetClassCodeName(UINT8 ClassCode[3]);

extern PCI_DEV_ID_NAME gPciDevIdNameList[];
extern UINTN gPciDevIdNameListCount;

/*
CHAR16 *GetDevName(UINT32 PciId, UINT8 ClassCode[3])
{
  UINTN   Index;
  UINT16  *Id;
  CHAR16  *s;

  
  Id = (UINT16*)&PciId;
  for(Index=0;Index<gPciDevIdNameListCount;Index++){
    if(Id[0] == gPciDevIdNameList[Index].VendorId && Id[1] == gPciDevIdNameList[Index].DeviceId){
      return gPciDevIdNameList[Index].DeviceName;
    }
  }

  s = GetDevNameByClassCode(ClassCode);
  if(s == NULL){
    return L"Unknown";
  } else {
    return s;
  }
}
*/
///
/// HII specific Vendor Device Path definition.
///
typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       End;
} HII_VENDOR_DEVICE_PATH;

#define PCI_PRIVATE_SIGNATURE SIGNATURE_32 ('p', 'C', 'R', 'P')

typedef struct{
  UINTN                             Signature;
  EFI_HII_HANDLE                    HiiHandle;
  EFI_HANDLE                        DriverHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL    ConfigAccess;
} PCI_PRIVATE_DATA;


#define PCI_ROOT_PORT_FROM_THIS(a)  CR(a, PCI_PRIVATE_DATA, ConfigAccess, PCI_PRIVATE_SIGNATURE)

EFI_STATUS
EFIAPI
ExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  );

EFI_STATUS
EFIAPI
RouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  );

EFI_STATUS
EFIAPI
PciCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  ); 


PCI_PRIVATE_DATA gPrivateData = {
  PCI_PRIVATE_SIGNATURE,
  NULL,
  NULL,
  {                           // ConfigAccess
    ExtractConfig,
    RouteConfig,
    PciCallback
  }
};


HII_VENDOR_DEVICE_PATH  mPciHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    SETUP_PCI_ROOT_PORT_FORMSET_GUID
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
ExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;
  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
RouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  UINTN                  Index = 0;

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;
  for (Index = 0; Index < mRootPortIndex; Index ++) {
    FreePool (mPciDisplayStr[Index].MaxLinkSpeedStr);
    FreePool (mPciDisplayStr[Index].PciDeviceNameStr);
    FreePool (mPciDisplayStr[Index].ASPMSupportStr);
    FreePool (mPciDisplayStr[Index].LinkSpeedStr);
  }
  if (mPciDisplayStr != NULL) {
    FreePool (mPciDisplayStr);
  }
  if (mPciIoCtx  != NULL) {
    FreePool (mPciIoCtx );
  }

  return EFI_NOT_FOUND;
}

VOID 
CreateAndDisplyMsgForm (
  IN EFI_QUESTION_ID                      QuestionId
) 
{
  VOID                          *RpStartOpCodeHandle = NULL;
  VOID                          *RpEndOpCodeHandle = NULL;
  VOID                          *RpOptionsOpCodeHandle = NULL;
  EFI_IFR_GUID_LABEL            *RpStartLabel;
  EFI_IFR_GUID_LABEL            *RpEndLabel;
  CHAR16                        StrVendorID[20];
  CHAR16                        StrDeviceID[20];
  PCI_IO_DISPLAY_STR            *PciStrUpdate;
  PCI_IO_CTX                    *pciCtxUpdate;

  pciCtxUpdate = &mPciIoCtx[QuestionId-PCIE_RP_QUESTION_ID];
  PciStrUpdate = &mPciDisplayStr[QuestionId-PCIE_RP_QUESTION_ID];

  // Pci Root Port
  if(pciCtxUpdate->ClassCode[2] != 6 ||pciCtxUpdate->ClassCode[1] != 4 || pciCtxUpdate->ClassCode[0] != 0){
   return;
  }

  HiiSetString (gPrivateData.HiiHandle, STRING_TOKEN (STR_PCI_ROOT_FORMSET_SUBTITLE), (EFI_STRING)(pciCtxUpdate->RootPortName), NULL);
  UnicodeSPrint (StrVendorID, sizeof (StrVendorID), L"%04X", pciCtxUpdate->Id & 0xFFFF);
  UnicodeSPrint (StrDeviceID, sizeof (StrDeviceID), L"%04X", (pciCtxUpdate->Id >> 16) & 0xFFFF);

  RpStartOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (RpStartOpCodeHandle != NULL);

  RpEndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (RpEndOpCodeHandle != NULL);

  RpOptionsOpCodeHandle = HiiAllocateOpCodeHandle ();

  ASSERT (RpOptionsOpCodeHandle != NULL);
    
  RpStartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(RpStartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  RpStartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  RpStartLabel->Number       = LABEL_PCI_RP_SUBFORM_START;

  RpEndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(RpEndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  RpEndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  RpEndLabel->Number       = LABEL_PCI_RP_SUBFORM_END;


  HiiSetString (gPrivateData.HiiHandle,STRING_TOKEN(STR_PCI_ROOT_PORT_DEVICE_CLASS),PciStrUpdate->PciDeviceNameStr,NULL);
  HiiSetString (gPrivateData.HiiHandle,STRING_TOKEN(STR_PCI_ROOT_PORT_VENDOR_ID),StrVendorID,NULL);
  HiiSetString (gPrivateData.HiiHandle,STRING_TOKEN(STR_PCI_ROOT_PORT_DEVICE_ID),StrDeviceID,NULL);
  HiiSetString (gPrivateData.HiiHandle,STRING_TOKEN(STR_PCIE_ROOT_PORT_MAXLINK_WIDTH),PciStrUpdate->MaxLinkWidthStr,NULL);
  HiiSetString (gPrivateData.HiiHandle,STRING_TOKEN(STR_PCIE_ROOT_PORT_MAXLINK_SPEED),PciStrUpdate->MaxLinkSpeedStr,NULL);
  HiiSetString (gPrivateData.HiiHandle,STRING_TOKEN(STR_PCIE_ROOT_PORT_ASPM_STATE),PciStrUpdate->ASPMSupportStr,NULL);

  //ASPM
  HiiCreateTextOpCode (
    RpStartOpCodeHandle,
    STRING_TOKEN(STR_PCIE_ROOT_PORT_ASPM),
    STRING_TOKEN(STR_PCIE_ROOT_PORT_ASPM),
    STRING_TOKEN(STR_PCIE_ROOT_PORT_ASPM_STATE)
    );

  HiiCreateTextOpCode (
    RpStartOpCodeHandle,
    STRING_TOKEN(STR_PCI_LIST_VENDOR_ID),
    STRING_TOKEN(STR_PCI_LIST_VENDOR_ID),
    STRING_TOKEN(STR_PCI_ROOT_PORT_VENDOR_ID)
    );

  HiiCreateTextOpCode (
    RpStartOpCodeHandle,
    STRING_TOKEN(STR_PCI_LIST_DEVICE_ID),
    STRING_TOKEN(STR_PCI_LIST_DEVICE_ID),
    STRING_TOKEN(STR_PCI_ROOT_PORT_DEVICE_ID)
    );

  HiiCreateTextOpCode (
    RpStartOpCodeHandle,
    STRING_TOKEN(STR_PCI_LIST_DEVICE_CLASS),
    STRING_TOKEN(STR_PCI_LIST_DEVICE_CLASS),
    STRING_TOKEN(STR_PCI_ROOT_PORT_DEVICE_CLASS)
    );
  
  HiiCreateTextOpCode (
    RpStartOpCodeHandle,
    STRING_TOKEN(STR_PCIE_ROOT_PORT_MAXLINK_WIDTH_TITLE),
    STRING_TOKEN(STR_PCIE_ROOT_PORT_MAXLINK_WIDTH_TITLE),
    STRING_TOKEN(STR_PCIE_ROOT_PORT_MAXLINK_WIDTH)
    );

  HiiCreateTextOpCode (
    RpStartOpCodeHandle,
    STRING_TOKEN(STR_PCIE_ROOT_PORT_MAXLINK_SPEED_TITLE),
    STRING_TOKEN(STR_PCIE_ROOT_PORT_MAXLINK_SPEED_TITLE),
    STRING_TOKEN(STR_PCIE_ROOT_PORT_MAXLINK_SPEED)
    );

  HiiUpdateForm (
  gPrivateData.HiiHandle,
  &gByoSetupPciRootPortFormsetGuid,   // Formset GUID
  PCI_ROOT_PORT_SUBFORM_ID,               // Form ID
  RpStartOpCodeHandle,              // Label for where to insert opcodes
  RpEndOpCodeHandle                 // Replace data
  );

  if(RpStartOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(RpStartOpCodeHandle);
  }

  if(RpEndOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(RpEndOpCodeHandle);
  }

  if(RpOptionsOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(RpOptionsOpCodeHandle);
  }
}

EFI_STATUS
EFIAPI
PciCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      QuestionId,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
  )
{
  EFI_STATUS                  Status;

  Status = EFI_UNSUPPORTED;
  if ((This == NULL) || (Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Action == EFI_BROWSER_ACTION_FORM_OPEN || Action == EFI_BROWSER_ACTION_FORM_CLOSE) {
    return Status;
  }
  
  switch (Action) {
    case EFI_BROWSER_ACTION_CHANGING:
      if (QuestionId >= PCIE_RP_QUESTION_ID  && QuestionId <= PCIE_RP_QUESTION_ID + mRootPortIndex ) {
        CreateAndDisplyMsgForm (QuestionId);
      }
      Status = EFI_SUCCESS;
      break;

    default:
      break;
  }

  return Status;
}

//
//Create Pcie Root Port GotoOpcode dynamically
//
VOID
CreatePciRootPortForm (
  VOID
  )
{
  VOID                         *BridgeStartOpCodeHandle = NULL;
  VOID                         *BridgeEndOpCodeHandle   = NULL;
  EFI_IFR_GUID_LABEL           *StartLabelBridge;
  EFI_IFR_GUID_LABEL           *EndLabelBridge;
  CHAR16                       UpdateTitleStrBuffer[30];
  EFI_STRING_ID                StrId;
  UINTN                        Index;
  PCI_IO_CTX                   *pciCtxUpdate;

  BridgeStartOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (BridgeStartOpCodeHandle != NULL);

  BridgeEndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (BridgeEndOpCodeHandle != NULL);

  StartLabelBridge = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(BridgeStartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabelBridge->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabelBridge->Number       = LABEL_PCI_RP_MAIN_FORM_START;

  EndLabelBridge = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(BridgeEndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabelBridge->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabelBridge->Number       = LABEL_PCI_RP_MAIN_FORM_END;

  for(Index = 0; Index < mRootPortIndex; Index++) {
    pciCtxUpdate = &mPciIoCtx[Index];
    UnicodeSPrint (UpdateTitleStrBuffer, sizeof (UpdateTitleStrBuffer), L"%s", pciCtxUpdate->RootPortName);
    StrId = HiiSetString (
              gPrivateData.HiiHandle,
              0,
              UpdateTitleStrBuffer,
              NULL
              );
    HiiCreateGotoOpCode (
      BridgeStartOpCodeHandle,
      PCI_ROOT_PORT_SUBFORM_ID,
      StrId,
      STRING_TOKEN (STR_PCI_ROOT_FORMSET_HELP),
      EFI_IFR_FLAG_CALLBACK,
      (EFI_QUESTION_ID)(PCIE_RP_QUESTION_ID + Index)
      );
  }
  HiiUpdateForm (
    gPrivateData.HiiHandle,
    &gByoSetupPciRootPortFormsetGuid,
    PCI_ROOT_PORT_MAIN_FORM_ID,
    BridgeStartOpCodeHandle,
    BridgeEndOpCodeHandle
    );

  if(BridgeStartOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(BridgeStartOpCodeHandle);
  }
  if(BridgeEndOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(BridgeEndOpCodeHandle);
  }
}


VOID CreateItem(VOID *OpCodeHandle, ...) 
{
  VA_LIST               Marker;
  EFI_STRING_ID         StrId;

  VA_START (Marker, OpCodeHandle);
  StrId = VA_ARG (Marker, EFI_STRING_ID);

  HiiCreateSubTitleOpCode (
    OpCodeHandle,
    StrId,
    0,
    0,
    0
    );

  while ((StrId = VA_ARG (Marker, EFI_STRING_ID)) != MAX_UINT16) {
    HiiCreateTextOpCode (
      OpCodeHandle,
      StrId,
      0,
      0
      );
  }
  VA_END (Marker);
}

EFI_STATUS
GetPcieSlotName (
IN EFI_HANDLE            *Handles,
OUT CHAR16               **SlotString
 )
{
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath = NULL;
  PLAT_HOST_INFO_PROTOCOL       *PlatHostInfo = NULL;
  EFI_DEVICE_PATH_PROTOCOL      *Node = NULL;
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *NextNode;
  PLATFORM_HOST_INFO            *HostInfo = NULL;
  CHAR8                         *SlotName = NULL;
  UINTN                         Size;
  EFI_STATUS                    Status;
  UINTN                         Index = 0;

  DevicePath = DevicePathFromHandle (Handles);
  if (DevicePath == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto ProExit;
   }

  Status = gBS->LocateProtocol (&gPlatHostInfoProtocolGuid, NULL, (VOID **)&PlatHostInfo);
  if ((!EFI_ERROR (Status)) && ( PlatHostInfo != NULL ) && (PlatHostInfo->HostList != NULL)) {
    Node = DuplicateDevicePath (DevicePath);
    TempDevicePath = Node;
    while (!IsDevicePathEndType (TempDevicePath))
    {
      NextNode = NextDevicePathNode (TempDevicePath);
      if ((NextNode->Type == HARDWARE_DEVICE_PATH) && NextNode->SubType == HW_PCI_DP) {
        break;
      }
      TempDevicePath = NextDevicePathNode(TempDevicePath);
    }
    for (Index = 0; Index < PlatHostInfo->HostCount; Index ++) { // match HostInfo by DevicePath
      HostInfo = PlatHostInfo->HostList + Index;
      if (HostInfo->Dp != NULL && CompareMem (HostInfo->Dp, Node, GetDevicePathSize (Node) - END_DEVICE_PATH_LENGTH) == 0) {
        break;
      }
    }
    if (Node != NULL) {
      FreePool (Node);
    }
    if ((Index < PlatHostInfo->HostCount ) && (HostInfo != NULL) && (HostInfo->HostCtx != NULL)) {
      SlotName = ((PLATFORM_HOST_INFO_PCIE_CTX *)HostInfo->HostCtx)->SlotName;
      if (SlotName != NULL && (AsciiStrLen(SlotName))) {
        Size = (AsciiStrLen(SlotName) + 1) * sizeof(CHAR16);
        *SlotString = (CHAR16 *)AllocateZeroPool(Size);
        ASSERT(*SlotString != NULL);
        AsciiStrToUnicodeStrS (SlotName, *SlotString, Size/sizeof(CHAR16));
        Status = EFI_SUCCESS;
      }
    }
  }

ProExit:
  if (*SlotString == NULL) {
    *SlotString = (CHAR16 *)AllocateCopyPool(StrSize(L"N/A"), L"N/A");
    ASSERT(*SlotString != NULL);
  }
  return Status;
}

CHAR16 *
ConVertSpeedTextToString (
  IN  UINT8   LinkSpeed
  )
{
  CHAR16             *LinkSpeedStr = NULL;

  switch(LinkSpeed) {
    case 0:
      LinkSpeedStr = L"N/A";
      break;
    case 1:
      LinkSpeedStr = L"Gen1(2.5GT/s)";
      break;
    case 2:
      LinkSpeedStr = L"Gen2(5GT/s)";
      break;
    case 4:  
      LinkSpeedStr = L"Gen3(8GT/s)";
      break;
    case 8:
      LinkSpeedStr = L"Gen4(16GT/s)";
      break;
    case 16:
      LinkSpeedStr = L"Gen5(32GT/s)";
      break;
    default:
      LinkSpeedStr = L"N/A";
      break;
    }

  return  LinkSpeedStr;
}

VOID
CreateForm ()
{
  VOID                  *StartOpCodeHandle = NULL;
  VOID                  *EndOpCodeHandle   = NULL;
  EFI_IFR_GUID_LABEL    *StartLabel;
  EFI_IFR_GUID_LABEL    *EndLabel;
  EFI_STATUS            Status;
  UINTN                 HandleCount;
  EFI_HANDLE            *Handles = NULL;
  UINTN                 Index;
  EFI_STRING_ID         StrId;
  UINT8                 ClassCode[3];
  CHAR16                StrBuffer[256];
  EFI_PCI_IO_PROTOCOL   *PciIo;
  UINT32                PciId;
  UINTN                 s, b, d, f;
  PCI_IO_CTX            *PciIoCtx = NULL;
  UINTN                 PciIoCtxCount = 0;
  PCI_IO_CTX            *p;
  PCI_IO_CTX            PciIoCtxTemp;
  UINTN                 i, j;
  EFI_STRING_ID         HelpToken;
  EFI_STRING_ID         BusStrId;
  EFI_STRING_ID         DevStrId;
  EFI_STRING_ID         FunStrId;
  EFI_STRING_ID         VendorIDStrId;
  EFI_STRING_ID         DeviceIDStrId;
  EFI_STRING_ID         PciDeviceNameStrId;
  CHAR16                StrBuff[5];
  CHAR16                *PciDeviceNameStr;
  BYO_GUI_IFR_GUID_DESCRIPTION_FULL GridOpCode;
  BOOLEAN               NeedFilter;
  UINTN                 CurLinkWidth, MaxLinkWidth;
  UINTN                 CurLinkSpeed, MaxLinkSpeed;
  UINT8                 CapabilityPtr = 0, PcieCapabilityPtr = 0;
  UINT16                CapabilityEntry = 0, CapabilityID = 0;
  UINT32                LinkStatus, LinkCap;
  CHAR16                *LinkSpeedStr = NULL;
  CHAR16                *MaxLinkSpeedStr = NULL;
  CHAR16                CurLinkWidthStr[4];
  CHAR16                MaxLinkWidthStr[4];
  CHAR16                FrontPartPciDeviceNameStr [SUPPORTED_MAX_LENGTH + 1];
  CHAR16                RearPartPciDeviceNameStr [SUPPORTED_MAX_LENGTH + 1];
  CHAR16                *PciDeviceByName = NULL;
  CHAR16                *PciDeviceByNameStr = NULL;
  CHAR16                *DeviceNameDisplayStr = NULL;
  UINTN                 Size;
  UINTN                 ASPMSupport;
  CHAR16                *ASPMSupportStr = NULL;
  CHAR16                *SlotString = NULL;
  EFI_DEVICE_PATH_PROTOCOL           *DevicePath = NULL;
  EFI_HANDLE                         PciHandle;
  UINTN                              SplitIndex = 0;
  EFI_STRING_ID         SegStrId;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  PciIoCtx = AllocatePool(sizeof(PCI_IO_CTX) * HandleCount);
  if(PciIoCtx == NULL){
    goto ProcExit;
  }

  StartOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_PCI_LIST_START;

  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_PCI_LIST_END;

  for(Index=0;Index<HandleCount;Index++){

    Status = gBS->HandleProtocol(
                    Handles[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID**)&PciIo
                    );

    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, PCI_VENDOR_ID_OFFSET, 1, &PciId);
    if((UINT16)PciId == 0xFFFF || PciId==0){          // ignore non-existent
      continue;
    }
    
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CLASSCODE_OFFSET, 3, ClassCode);
    if(ClassCode[2] == 6 && ClassCode[1] != 4){          // ignore bridge except P2P bridge and SubDecode P2P bridge
      continue;
    }
    if(ClassCode[2] == 0x13 && ClassCode[1] == 0 && ClassCode[0] == 0){         // Non-Essential Instrumentation
      continue;
    }
    if(ClassCode[2] == 0x10 && ClassCode[1] == 0x80 && ClassCode[0] == 0){      // Other en/decryption
      continue;
    }    
 
    //
    // Get Pci Device Information
    //
    DevicePath = DevicePathFromHandle(Handles[Index]);
    Status = EfiGetControllerName (Handles[Index], &PciDeviceByName);
    if (EFI_ERROR(Status) && DevicePath != NULL) {
      Status = gBS->LocateDevicePath (
                  &gEfiPciIoProtocolGuid,
                  &DevicePath,
                  &PciHandle
                  );
      if (!EFI_ERROR(Status)) {
        Status = EfiGetControllerName(PciHandle, &PciDeviceByName);
      }
    }
    if (EFI_ERROR(Status)) {
      PciDeviceByName = GetPciVendorString(PciIo, GET_TYPE_STR);
    }
    if (PciDeviceByName == NULL) {
      PciDeviceByName = (CHAR16 *)AllocateCopyPool(StrSize(L"N/A"), L"N/A");
      ASSERT(PciDeviceByName != NULL);
    }
    GetPcieSlotName (Handles[Index], &SlotString);

    PcieCapabilityPtr = 0;
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CAPBILITY_POINTER_OFFSET, 1, &CapabilityPtr);

    while (CapabilityPtr >= 0x40) {
      PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, CapabilityPtr, 1, &CapabilityEntry);
      CapabilityID = (UINT8) CapabilityEntry;

      if ((UINT8)EFI_PCI_CAPABILITY_ID_PCIEXP == CapabilityID) {
        PcieCapabilityPtr = CapabilityPtr;
        break;
      }
      CapabilityPtr = (UINT8)(CapabilityEntry >> 8);
    }

    if (PcieCapabilityPtr) {
        PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, PcieCapabilityPtr + 0x10, 1, &LinkStatus);
        PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, PcieCapabilityPtr + 0x0c, 1, &LinkCap);
        LinkStatus = LinkStatus >> 16;

        CurLinkWidth= (LinkStatus >> 4) & 0x3F;
        CurLinkSpeed = LinkStatus & 0xF;
        MaxLinkWidth = (LinkCap >> 4) & 0x3F;
        MaxLinkSpeed = LinkCap & 0xF;
        ASPMSupport = (LinkCap >> 10) & 0x3;

        if (CurLinkSpeed >= 1 && CurLinkSpeed <= 7) {
          CurLinkSpeed = (UINTN)1 << (CurLinkSpeed - 1);
        }

         if (MaxLinkSpeed >= 1 && MaxLinkSpeed <= 7) {
          MaxLinkSpeed = (UINTN)1 << (MaxLinkSpeed - 1);
        }
    } else {
      CurLinkSpeed = 0;
      CurLinkWidth = 0;
      MaxLinkWidth = 0;
      MaxLinkSpeed = 0;
      ASPMSupport = 5;
    }

    PciIo->GetLocation(PciIo, &s, &b, &d, &f);

    p = &PciIoCtx[PciIoCtxCount];
    p->Seg  = (UINT16)s;
    p->Bus  = (UINT8)b;
    p->Dev  = (UINT8)d;
    p->Func = (UINT8)f;
    p->Id   = PciId;
    p->CurLinkWidth = (UINT8) CurLinkWidth;
    p->LinkSpeed = (UINT8) CurLinkSpeed;
    p->PciDeviceByName = AllocateCopyPool(StrSize (PciDeviceByName), PciDeviceByName);
    p->MaxLinkWidth = (UINT8) MaxLinkWidth;
    p->MaxLinkSpeed = (UINT8) MaxLinkSpeed;
    p->ASPMSupport = (UINT8) ASPMSupport;
    CopyMem(p->RootPortName, SlotString, StrSize(SlotString) < sizeof(p->RootPortName) - 2  ? StrSize(SlotString) : sizeof(p->RootPortName) - 2);
    p->RootPortName[ARRAY_SIZE(p->RootPortName)-1] = 0;
    CopyMem(p->ClassCode, ClassCode, sizeof(p->ClassCode));
    PciIoCtxCount++;
    if (PciDeviceByName != NULL) {
      FreePool (PciDeviceByName);
    }
    if (SlotString != NULL) {
      FreePool (SlotString);
      SlotString = NULL;
    }
  }

  mPciDisplayStr = AllocatePool(sizeof(PCI_IO_DISPLAY_STR) * PciIoCtxCount);
  if(mPciDisplayStr == NULL){
    goto ProcExit;
  }
  mPciIoCtx  = AllocatePool(sizeof(PCI_IO_CTX) * PciIoCtxCount);
  if(mPciIoCtx  == NULL){
    goto ProcExit;
  }
  if(PciIoCtxCount == 0){
    goto ProcExit;
  }

  for (i = 0; i < PciIoCtxCount; i++) {
    BOOLEAN     Changed;
    for (Changed = FALSE, j = 0; j+1 < PciIoCtxCount - i; j++) {
      if (PCI_SEGMENT_LIB_ADDRESS (PciIoCtx[j].Seg, PciIoCtx[j].Bus, PciIoCtx[j].Dev, PciIoCtx[j].Func, 0) >
          PCI_SEGMENT_LIB_ADDRESS (PciIoCtx[j+1].Seg, PciIoCtx[j+1].Bus, PciIoCtx[j+1].Dev, PciIoCtx[j+1].Func, 0)) {
        CopyMem (&PciIoCtxTemp,  &PciIoCtx[j],   sizeof (PCI_IO_CTX));
        CopyMem (&PciIoCtx[j],   &PciIoCtx[j+1], sizeof (PCI_IO_CTX));
        CopyMem (&PciIoCtx[j+1], &PciIoCtxTemp,  sizeof (PCI_IO_CTX));
        Changed = TRUE;
      }
    }
    if (!Changed) {
      break;
    }
  }

  HelpToken = HiiSetString(gHiiHandle, 0, L"", NULL);

  if (FixedPcdGetBool(PcdGuiEnable)) {
    GridOpCode.Data.Type      = BYO_IFR_TYPE_GRID_START;
    GridOpCode.Data.OpCode    = MAX_UINT16;
    GridOpCode.Data.Attribute = BYO_IFR_ALIGN_LEFT;
    HiiCreateGuidOpCode (
      StartOpCodeHandle,
      &gByoGuiIfrDescriptionGuid,
      &GridOpCode,
      sizeof(GridOpCode)
      );
  }

  //
  // New Title OpCode
  //
  if (FixedPcdGetBool(PcdGuiEnable)) {
    CreateItem(StartOpCodeHandle,
               STRING_TOKEN (STR_PCI_LIST_SEG), 
               STRING_TOKEN (STR_PCI_LIST_BUS),
               STRING_TOKEN (STR_PCI_LIST_DEV), 
               STRING_TOKEN (STR_PCI_LIST_FUN),
               STRING_TOKEN (STR_PCI_LIST_VENDOR_ID),
               STRING_TOKEN (STR_PCI_LIST_DEVICE_ID), 
               STRING_TOKEN (STR_PCI_LIST_DEVICE_CLASS), 
               MAX_UINT16
               );
  } else {
    HiiCreateSubTitleOpCode (
      StartOpCodeHandle,
      STRING_TOKEN(STR_PCI_LIST_HEADERS),
      0,
      0,
      0
      );
  }

//BUS DEV FUN   VendorID  DeviceID  LinkSpeed  Linkwidth  DeviceClass
//00  00  00    0000      0000      abcd       0000       abcd
  for(Index=0;Index<PciIoCtxCount;Index++){
    p = &PciIoCtx[Index];
    PciDeviceNameStr = GetClassCodeName(p->ClassCode);
    if (gPlatformPciListFilterFunc != NULL) {
      NeedFilter = gPlatformPciListFilterFunc (p, &PciDeviceNameStr);
      if (NeedFilter) {
        continue;
      }
    }

    LinkSpeedStr = ConVertSpeedTextToString (p->CurLinkWidth == 0 ? 0:p->LinkSpeed);
    MaxLinkSpeedStr = ConVertSpeedTextToString (p->MaxLinkSpeed);

    if (p->CurLinkWidth) {
      UnicodeSPrint (CurLinkWidthStr, sizeof (CurLinkWidthStr), L"x%d", p->CurLinkWidth);
    } else {
      UnicodeSPrint (CurLinkWidthStr, sizeof (CurLinkWidthStr), L"N/A");
    }
    if (p->MaxLinkWidth) {
      UnicodeSPrint (MaxLinkWidthStr, sizeof (MaxLinkWidthStr), L"x%d", p->MaxLinkWidth);
    } else {
      UnicodeSPrint (MaxLinkWidthStr, sizeof (MaxLinkWidthStr), L"N/A");
    }

    switch (p->ASPMSupport) {
      case 0:
        ASPMSupportStr = L"Disable";
        break;
      case 1:
        ASPMSupportStr = L"L0s";
        break;
      case 2:
        ASPMSupportStr = L"L1";
        break;
      case 3:
        ASPMSupportStr = L"L0s & L1";
        break;
      default:
        ASPMSupportStr = L"N/A";
        break;
    }

    if(p->ClassCode[2] == 6) {
      if ( p->ClassCode[1] == 4 && p->ClassCode[0] == 0 && (StrCmp (p->RootPortName, L"N/A"))) {
        CopyMem(&mPciIoCtx[mRootPortIndex], &PciIoCtx[Index], sizeof(PCI_IO_CTX));
        CopyMem(mPciDisplayStr[mRootPortIndex].CurLinkWidthStr , CurLinkWidthStr, sizeof(CurLinkWidthStr));
        CopyMem(mPciDisplayStr[mRootPortIndex].MaxLinkWidthStr , MaxLinkWidthStr, sizeof(MaxLinkWidthStr));
        mPciDisplayStr[mRootPortIndex].ASPMSupportStr = (CHAR16 *)AllocateCopyPool (StrSize(ASPMSupportStr), ASPMSupportStr);
        mPciDisplayStr[mRootPortIndex].LinkSpeedStr = (CHAR16 *)AllocateCopyPool (StrSize (LinkSpeedStr), LinkSpeedStr);
        mPciDisplayStr[mRootPortIndex].MaxLinkSpeedStr = (CHAR16 *)AllocateCopyPool (StrSize (MaxLinkSpeedStr), MaxLinkSpeedStr);
        mPciDisplayStr[mRootPortIndex].PciDeviceNameStr = (CHAR16 *)AllocateCopyPool (StrSize (PciDeviceNameStr), PciDeviceNameStr);
        mRootPortIndex++;
      }
      continue;
    }
    //
    //Display Pci device information in the help bar.  DeviceNameDisplayStr = Device information + : +PciDeviceByNameStr
    //
    HelpToken = 0;
    if (!StrCmp (p->PciDeviceByName, L"N/A")) {
      HelpToken = HiiSetString(gHiiHandle, HelpToken,  L" ", NULL);
    } else {
      PciDeviceByNameStr = HiiGetString(gHiiHandle, STRING_TOKEN(STR_PCI_LIST_DEVICE_INFO), "en-US");
      Size = StrSize(p->PciDeviceByName) + StrSize (PciDeviceByNameStr);
      DeviceNameDisplayStr = (CHAR16 *)AllocateZeroPool(Size);
      ASSERT(DeviceNameDisplayStr != NULL);
      UnicodeSPrint (DeviceNameDisplayStr ,Size,L"%s:%s", PciDeviceByNameStr,p->PciDeviceByName);
      HelpToken = HiiSetString(gHiiHandle, HelpToken,  DeviceNameDisplayStr, "en-US");
      FreePool (PciDeviceByNameStr);
      FreePool(DeviceNameDisplayStr);

      PciDeviceByNameStr = HiiGetString(gHiiHandle, STRING_TOKEN(STR_PCI_LIST_DEVICE_INFO), "zh-Hans");
      Size = StrSize(p->PciDeviceByName) + StrSize (PciDeviceByNameStr);
      DeviceNameDisplayStr = (CHAR16 *)AllocateZeroPool(Size);
      UnicodeSPrint (DeviceNameDisplayStr ,Size,L"%s:%s", PciDeviceByNameStr,p->PciDeviceByName);
      HelpToken = HiiSetString(gHiiHandle, HelpToken,  DeviceNameDisplayStr, "zh-Hans");
      FreePool (PciDeviceByNameStr);
      FreePool (DeviceNameDisplayStr);
    }
    FreePool (p->PciDeviceByName);

    if (FixedPcdGetBool(PcdGuiEnable)) {
      UnicodeSPrint(StrBuff, sizeof(StrBuff), L"%04X", p->Seg);
   	  SegStrId = HiiSetString (gHiiHandle, 0, StrBuff, NULL);
      UnicodeSPrint(StrBuff, sizeof(StrBuff), L"%02X", p->Bus);
      BusStrId = HiiSetString (gHiiHandle, 0, StrBuff, NULL);
      UnicodeSPrint(StrBuff, sizeof(StrBuff), L"%02X", p->Dev);
      DevStrId = HiiSetString (gHiiHandle, 0, StrBuff, NULL);
      UnicodeSPrint(StrBuff, sizeof(StrBuff), L"%02X", p->Func);
   	  FunStrId = HiiSetString (gHiiHandle, 0, StrBuff, NULL);
      UnicodeSPrint(StrBuff, sizeof(StrBuff), L"%04X", p->Id & 0xFFFF);
      VendorIDStrId = HiiSetString (gHiiHandle, 0, StrBuff, NULL);
      UnicodeSPrint(StrBuff, sizeof(StrBuff), L"%04X", (p->Id >> 16) & 0xFFFF);
      DeviceIDStrId = HiiSetString (gHiiHandle, 0, StrBuff, NULL);
      PciDeviceNameStrId = HiiSetString (gHiiHandle, 0, PciDeviceNameStr, NULL);
      CreateItem(StartOpCodeHandle, SegStrId, BusStrId, DevStrId, FunStrId, VendorIDStrId,  DeviceIDStrId, PciDeviceNameStrId, MAX_UINT16);
    } else {
      if (StrLen (PciDeviceNameStr) > SUPPORTED_MAX_LENGTH) {
        SplitIndex = SUPPORTED_MAX_LENGTH -1;
        while (SplitIndex > 0 && PciDeviceNameStr[SplitIndex] != L' ') {
          SplitIndex--;
        }
        if (SplitIndex == 0) {
          SplitIndex = SUPPORTED_MAX_LENGTH - 1;
        }
        ZeroMem (FrontPartPciDeviceNameStr, sizeof (FrontPartPciDeviceNameStr));
        ZeroMem (RearPartPciDeviceNameStr, sizeof (RearPartPciDeviceNameStr));
        UnicodeSPrint (FrontPartPciDeviceNameStr, (SplitIndex + 2)* sizeof (CHAR16), L"%s", PciDeviceNameStr);
        UnicodeSPrint (RearPartPciDeviceNameStr, sizeof (RearPartPciDeviceNameStr), L"%s", PciDeviceNameStr + SplitIndex + 1);
        UnicodeSPrint(
        StrBuffer,
        sizeof(StrBuffer),
        L"%04X %02X  %02X  %02X   %04X      %04X     %-13s  %-4s      %s\n                                                              %s",
        p->Seg, p->Bus, p->Dev, p->Func, 
        p->Id & 0xFFFF,
        (p->Id >> 16) & 0xFFFF,
        LinkSpeedStr,
        CurLinkWidthStr,
        FrontPartPciDeviceNameStr,
        RearPartPciDeviceNameStr
        );
      } else {
        UnicodeSPrint(
          StrBuffer,
          sizeof(StrBuffer),
          L"%04X %02X  %02X  %02X   %04X      %04X     %-13s  %-4s      %s",
          p->Seg, p->Bus, p->Dev, p->Func, 
          p->Id & 0xFFFF,
          (p->Id >> 16) & 0xFFFF,
          LinkSpeedStr,
          CurLinkWidthStr,
          PciDeviceNameStr
          );
      }
      StrId = HiiSetString (
                gHiiHandle,
                0,
                StrBuffer,
                NULL
                );
      HiiCreateActionOpCode (
        StartOpCodeHandle,
        (UINT16)(0xF000 + Index),
        StrId,
        HelpToken,
        EFI_IFR_FLAG_CALLBACK,
        0
        );
    }
  }
    
  if (FixedPcdGetBool(PcdGuiEnable)) {
    GridOpCode.Data.Type      = BYO_IFR_TYPE_GRID_END;
    GridOpCode.Data.OpCode    = MAX_UINT16;
    GridOpCode.Data.Attribute = 0;
    HiiCreateGuidOpCode (
      StartOpCodeHandle,
      &gByoGuiIfrDescriptionGuid,
      &GridOpCode,
      sizeof(GridOpCode)
      );
  }

  HiiUpdateForm (
    gHiiHandle,
    &gByoSetupPciListFormsetGuid,   // Formset GUID
    PCI_LIST_FORM_ID,               // Form ID
    StartOpCodeHandle,              // Label for where to insert opcodes
    EndOpCodeHandle                 // Replace data
    );

ProcExit:
  if(StartOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(StartOpCodeHandle);
  }
  if(EndOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(EndOpCodeHandle);
  }
  if(PciIoCtx != NULL){
    FreePool(PciIoCtx);
  }
}



VOID
EFIAPI
EnterSetupCallback (
  IN EFI_EVENT    Event,
  IN VOID*        Context
  )
{
  EFI_STATUS            Status;
  VOID                  *Interface;
  PLAT_HOST_INFO_PROTOCOL       *PlatHostInfo = NULL;

  Status = gBS->LocateProtocol(&gEfiSetupEnterGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);
  
  CreateForm();

  Status = gBS->LocateProtocol (&gPlatHostInfoProtocolGuid, NULL, (VOID **)&PlatHostInfo);
  //
  //If interfaces of PLAT_HOST_INFO_PROTOCOL is found, the PCIe Root Port Information page is displayed; otherwise, it is not.
  //
  if ((!EFI_ERROR (Status)) && ( PlatHostInfo != NULL ) && (PlatHostInfo->HostList != NULL)) {
    //
    // Install Device Path Protocol and Config Access protocol to driver handle
    //
    gPrivateData.DriverHandle = NULL;
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &gPrivateData.DriverHandle,
                    &gEfiDevicePathProtocolGuid,
                    &mPciHiiVendorDevicePath,
                    &gEfiHiiConfigAccessProtocolGuid,
                    &gPrivateData.ConfigAccess,
                    NULL
                    );
    ASSERT(!EFI_ERROR(Status));
    //
    // Publish our HII data
    //
    gPrivateData.HiiHandle = HiiAddPackages (
                                      &gByoSetupPciRootPortFormsetGuid,
                                      gPrivateData.DriverHandle,
                                      STRING_ARRAY_NAME,
                                      PciRootPortBin,
                                      NULL
                                      );
    ASSERT (gPrivateData.HiiHandle != NULL);
    CreatePciRootPortForm ();
  }
}







EFI_STATUS
EFIAPI
PciListDxeEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  VOID                  *Registration;

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  gHiiHandle = HiiAddPackages (
                 &gByoSetupPciListFormsetGuid,
                 NULL,
                 STRING_ARRAY_NAME,
                 PciListBin,
                 NULL
                 );
  ASSERT(gHiiHandle != NULL);

  EfiCreateProtocolNotifyEvent (
    &gEfiSetupEnterGuid,
    TPL_CALLBACK,
    EnterSetupCallback,
    NULL,
    &Registration
    );


  return EFI_SUCCESS;
}

