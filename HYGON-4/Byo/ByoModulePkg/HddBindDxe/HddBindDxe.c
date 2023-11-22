/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  HddBindDxe.c

Abstract:
  Binding the harddisk(SATA or NVME) according to the configuration set by users.
When this function has been opened, it can't boot from the unbound harddisk.

Revision History:

**/

#include "HddBindDxe.h"
#include <Library/SetupUiLib.h>
#include <Protocol/BlockIo.h>
#include <Library/ByoCommLib.h>
#ifndef BYO_ONLY_UEFI
#include <Protocol/LegacyBios.h>
#endif

#define INDEX_OF_NO_EXIST_HDD 0xFFFF

#ifndef BYO_ONLY_UEFI
UINT16  mBbsIndex;
#endif

extern UINT8 HddBindDxeStrings[];
extern UINT8 HddBindDxeVfrBin[];
STATIC EFI_GUID gHddBindNvVarGuid = HDD_BIND_NV_GUID;
EFI_HII_HANDLE gStringHandle;

#ifndef BYO_ONLY_UEFI
BOOLEAN
IsValidBbsEntry (
  IN BBS_TABLE   *BbsEntry
  )
{
  switch (BbsEntry->BootPriority) {
    case BBS_IGNORE_ENTRY:
    case BBS_DO_NOT_BOOT_FROM:
    case BBS_LOWEST_PRIORITY:
      return FALSE;
    default:
      return TRUE;
  }
}
#endif

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
DriverCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );    

HDD_BIND_PRIVATE_DATA gPrivateData = {
  HDD_BIND_PRIVATE_SIGNATURE, // Signature
  NULL,                       // HiiHandle
  NULL,                       // DriverHandle
  {                           // ConfigAccess
    ExtractConfig,
    RouteConfig,
    DriverCallback
  },
  {                           // VarData
    0,
  },
  NULL,                       // HddBindInfo
  NULL,                       // AllHddInfo
  0,                          // HddCount
  0,                          // AllHddCount
  NULL,                       // BoundHddNvData
  0,                          // BoundHddNvSize
  FALSE,                      // UserChoice
  NULL,                       // TargetMn
  NULL                        // TargetSn
};

EFI_GUID gHddBindFormsetGuid = SETUP_HDD_BIND_FORMSET_GUID;

HII_VENDOR_DEVICE_PATH gHddBindHiiVendorDevicePath = {
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

VOID
EFIAPI
ShowWarningMsg (
  VOID
  )
{
  CHAR16                      *Str1 = NULL;
  CHAR16                      *Str2 = NULL;

  ASSERT (gStringHandle != NULL);
  Str1 = HiiGetString (gStringHandle, STRING_TOKEN(STR_HDD_MISTYPED), NULL);
  Str2 = HiiGetString (gStringHandle, STRING_TOKEN(STR_ENTER_STRING), NULL);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  while (TRUE) {
    if (SELECTION_YES == UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, Str1,Str2, NULL)) {
      FreePool (Str1);
      FreePool (Str2);
      break;
    }
  }
  
  gRT->ResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL);
}

EFI_STATUS
EFIAPI
CheckBindHdd (
  EFI_DEVICE_PATH_PROTOCOL                   *Node,
  EFI_DEVICE_PATH_PROTOCOL                   *FilePath
)
{
  EFI_STATUS                Status;
  CHAR8                     Sn[MAXSIZE_OF_SN];
  #ifndef BYO_ONLY_UEFI
    EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios;
    UINT16                        HddCount;
    UINT16                        BbsCount;
    HDD_INFO                      *HddInfo;
    BBS_TABLE                     *BbsTable;
    UINT16                        Index;
  #endif

  if (gPrivateData.TargetSn == NULL) {
    return EFI_SUCCESS;
  }

  Status = GetHddSnAndMn(Node, FilePath, NULL, Sn);
  if (!EFI_ERROR(Status)) {
    if (AsciiStrCmp(gPrivateData.TargetSn, Sn) == 0) {
      DEBUG ((DEBUG_ERROR, "Target Sn Match\n"));
      #ifndef BYO_ONLY_UEFI
        //
        // Disabled other legacy bbs entries
        //
        if (mBbsIndex != FIRST_USB_BBS_INDEX) {
          Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID**)&LegacyBios);
          if (EFI_ERROR (Status)) {
            return EFI_SUCCESS;
          }
          Status = LegacyBios->GetBbsInfo (
                         LegacyBios,
                         &HddCount,
                         &HddInfo,
                         &BbsCount,
                         &BbsTable
                         );
          if (EFI_ERROR(Status)) {
            return EFI_SUCCESS;
          }

          for (Index = 0; Index < FIRST_USB_BBS_INDEX; Index++) {
            if (!IsValidBbsEntry(&BbsTable[Index])) {
              continue;
            }
            if ((BbsTable[Index].DeviceType == BBS_HARDDISK) && (Index != mBbsIndex)) {
              BbsTable[Index].BootPriority = BBS_IGNORE_ENTRY;
            }
          }
        }
      #endif
      return EFI_SUCCESS;
    }
  }

  DEBUG ((DEBUG_ERROR, "Target Sn Not Match, Deny!\n"));
  ShowWarningMsg ();
  return EFI_ACCESS_DENIED;
}

VOID
EFIAPI
GetHddBindNvData (
  VOID
  )
{
  EFI_STATUS                Status;
  
  Status = GetVariable2(
               HDD_BIND_NV_NAME, 
               &gHddBindNvVarGuid, 
               &gPrivateData.BoundHddNvData,
               &gPrivateData.BoundHddNvSize
               );                
  if (EFI_ERROR(Status)) {
    gPrivateData.TargetSn = NULL;
    gPrivateData.TargetMn = NULL;
    gPrivateData.Description = NULL;
  } else {
    gPrivateData.TargetMn = (CHAR16 *)gPrivateData.BoundHddNvData;
    gPrivateData.TargetSn = (CHAR8 *)((UINTN)gPrivateData.BoundHddNvData + StrSize(gPrivateData.TargetMn));
    gPrivateData.Description = (CHAR16 *)((UINTN)gPrivateData.BoundHddNvData + StrSize(gPrivateData.TargetMn) + AsciiStrSize(gPrivateData.TargetSn));
  }
}

EFI_STATUS
SaveHddBindNvData (
  BOOLEAN                                    UserChoice,
  HDD_BIND_VAR_DATA                          *VarData
  )
{
  EFI_STATUS                Status;
  HDD_BIND_INFO             *HddInfo;
  CHAR16                    *NvData;
  UINTN                     Size;
  UINTN                     MnSize;
  UINTN                     SnSize;
  UINTN                     DesSize;

  if (((!UserChoice) )) { // user no choice or unbind
    Status = gRT->SetVariable(  
               HDD_BIND_NV_NAME, 
               &gHddBindNvVarGuid, 
               0,
               0,
               NULL
               );                                        
    if (EFI_ERROR(Status)) {
       if (Status == EFI_NOT_FOUND) {
         DEBUG((EFI_D_INFO, "[HddBindDxe] No create NvVar!"));
         return EFI_SUCCESS;
       }
      return Status;
    }
  } else {
    ASSERT (VarData->BindHddIndex <= gPrivateData.HddCount);
    HddInfo = &gPrivateData.HddBindInfo[VarData->BindHddIndex - 1];
    MnSize = StrSize(HddInfo->Mn);
    SnSize = AsciiStrSize(HddInfo->Sn);
	  DesSize = StrSize(HddInfo->Description);
    Size = MnSize + SnSize + DesSize;
    NvData = (CHAR16*)AllocateZeroPool(Size);
    ASSERT(NvData != NULL);
    CopyMem(NvData, HddInfo->Mn, MnSize);
    CopyMem((VOID *)((UINTN)NvData + MnSize), HddInfo->Sn, SnSize);
    CopyMem((VOID *)((UINTN)NvData + MnSize + SnSize), HddInfo->Description, DesSize);

    Status = gRT->SetVariable(  
                      HDD_BIND_NV_NAME, 
                      &gHddBindNvVarGuid, 
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      Size,
                      NvData
                      );                                         
    if (EFI_ERROR(Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    FreePool(NvData);
  }
  
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetHddSnAndMn (
  IN EFI_DEVICE_PATH_PROTOCOL                *Node,
  IN EFI_DEVICE_PATH_PROTOCOL                *DevicePath,
  IN OUT CHAR16                              *Mn,
  IN OUT CHAR8                               *Sn
  )
{
  EFI_STATUS                Status = EFI_NOT_FOUND;
  EFI_HANDLE                Handle;
  BYO_DISKINFO_PROTOCOL     *ByoDiskInfo;
  EFI_DISK_INFO_PROTOCOL    *DiskInfo;
  ATA_IDENTIFY_DATA         IdentifyData;
  UINT32                    BufferSize;
  UINTN                     Size;
  UINTN                     Index;
  CHAR8                     TempMn[MAXSIZE_OF_MN];

  if ((Node == NULL) || (DevicePath == NULL) ) {
    return EFI_NOT_READY;
  }
  
  if (Node != NULL) {
    if ((Node->SubType != MSG_SATA_DP) && (Node->SubType != MSG_NVME_NAMESPACE_DP) && (Node->SubType != MEDIA_HARDDRIVE_DP)) {
      return EFI_NOT_READY;
    }
  }

  if (Node->SubType == MSG_NVME_NAMESPACE_DP) {
    Status = gBS->LocateDevicePath(
              &gByoDiskInfoProtocolGuid,
              &DevicePath,
              &Handle
              );
    if (!EFI_ERROR(Status)) {
      Status = gBS->HandleProtocol(
                Handle,
                &gByoDiskInfoProtocolGuid,
                (VOID**)&ByoDiskInfo
                );
      if (Sn != NULL) {
        ZeroMem(Sn, MAXSIZE_OF_SN);
        Size = MAXSIZE_OF_SN * sizeof(CHAR8);
        ByoDiskInfo->GetSn(ByoDiskInfo, Sn, &Size);
        Sn[MAXSIZE_OF_SN - 1] = '\0';
        AsciiDeleteExtraSpaces(Sn);
      }
      
      if (Mn != NULL) {
        ZeroMem(Mn, MAXSIZE_OF_MN * sizeof(CHAR16));
        Size = sizeof(TempMn);
        ByoDiskInfo->GetMn(ByoDiskInfo, TempMn, &Size);
        UnicodeSPrint(Mn, MAXSIZE_OF_MN * sizeof(CHAR16), L"%a", TempMn);
      }
    } else {
      return Status;
    }
  } else {
    Status = gBS->LocateDevicePath(
          &gEfiDiskInfoProtocolGuid,
          &DevicePath,
          &Handle
          );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    Status = gBS->HandleProtocol(
                Handle,
                &gEfiDiskInfoProtocolGuid,
                (VOID**)&DiskInfo
                );                
    if (EFI_ERROR(Status)) {
      return Status;
    }                
    BufferSize = sizeof(ATA_IDENTIFY_DATA);
    Status = DiskInfo->Identify (
                     DiskInfo,
                     &IdentifyData,
                     &BufferSize
                     );      
    if (EFI_ERROR(Status)) {
      return Status;
    }

    if ((!(IdentifyData.config & ATAPI_DEVICE)) || (IdentifyData.config == 0x848A)) {
      if (Mn != NULL) {
        ZeroMem(Mn, MAXSIZE_OF_MN * sizeof(CHAR16));
        for (Index = 0; Index < (MAXSIZE_OF_MN - 1); Index += 2) {
          Mn[Index]     = IdentifyData.ModelName[Index + 1];
          Mn[Index + 1] = IdentifyData.ModelName[Index];
        }
        Mn[Index] = L'\0';
        DeleteExtraSpaces(Mn);
      }

      if (Sn != NULL) {
        ZeroMem(Sn, MAXSIZE_OF_SN);
        CopyMem(Sn, IdentifyData.SerialNo, sizeof(IdentifyData.SerialNo));  
        SwapWordArray((UINT8*)Sn, MAXSIZE_OF_SN - 1);
        Sn[MAXSIZE_OF_SN - 1] = '\0';
        AsciiDeleteExtraSpaces(Sn);
      }
    }
  }

  return Status;
}

/**
  Get informations of all harddisks(ATA&&NVME) on system.
  
  @param AllHddInfo return the informations of hard disks.And AllHddInfo need to free by caller
  when AllHddInfo is not NULL.
  @return  The number of harddisks enumerated.
**/
UINT16
EFIAPI
EnumerateAllHddInfo (
  IN OUT EXTEND_HDD_DATA          **AllHddInfo
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  UINTN                     HandleCount;
	EFI_HANDLE						    *HandleBuffer;
	EFI_HANDLE						    DeviceHandle;
  EFI_DEVICE_PATH_PROTOCOL	*DevicePath;
  EFI_DEVICE_PATH_PROTOCOL	*OriginDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  UINTN						  	      Seg,Bus,Dev,Func;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  UINT16                     HddCount;
  CHAR16                    Mn[MAXSIZE_OF_MN];
  CHAR8                     Sn[MAXSIZE_OF_SN];
  BOOLEAN							      FoundMessStorage = FALSE;	  
  EFI_BLOCK_IO_PROTOCOL     *BlockIo;
  Mn[0] = 0;
  
  if (AllHddInfo == NULL) {
    return 0;
  }

  Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiDiskInfoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );                
  if (EFI_ERROR(Status)) {
    HddCount = 0;
    AllHddInfo = NULL;
    return HddCount;
  }
  
  *AllHddInfo = (EXTEND_HDD_DATA *)AllocateZeroPool(sizeof(EXTEND_HDD_DATA) * HandleCount);
  ASSERT(*AllHddInfo != NULL);

  HddCount = 0;
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **) &BlockIo
                    );
    if (EFI_ERROR(Status)) {
      continue;
    }

    if (BlockIo->Media->RemovableMedia == 1 || BlockIo->Media->BlockSize == SIZE_2KB) { // filter out USB and SATA DVD
      continue;
    }

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &DevicePath
                    );
    if (EFI_ERROR(Status)) {
      continue;
    }
    OriginDevicePath = DevicePath;

    Status = gBS->LocateDevicePath (&gEfiPciIoProtocolGuid, &DevicePath, &DeviceHandle);
    if (EFI_ERROR(Status)) {
      continue;
    }
    Status = gBS->HandleProtocol (
                        DeviceHandle,
                        &gEfiPciIoProtocolGuid,
                        (VOID **)&PciIo
                        );                       
    ASSERT_EFI_ERROR(Status);

    PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);

    TempDevicePath = DevicePath;
    while (!IsDevicePathEndType (TempDevicePath)) { // check if it's nvme or sata
      if ((TempDevicePath->Type == MESSAGING_DEVICE_PATH) && ((TempDevicePath->SubType == MSG_SATA_DP) || (TempDevicePath->SubType == MSG_NVME_NAMESPACE_DP) || (TempDevicePath->SubType == MSG_ATAPI_DP))) {
				FoundMessStorage = TRUE;
				break;            
      }
			TempDevicePath = NextDevicePathNode (TempDevicePath);
    }
    
    Status = EFI_NOT_FOUND;
    if (FoundMessStorage) {
      Status = gBS->HandleProtocol (
                HandleBuffer[Index],
                &gEfiDevicePathProtocolGuid,
                (VOID**)&DevicePath
                );         
      if (!EFI_ERROR(Status)) {
        Status = GetHddSnAndMn(TempDevicePath, DevicePath, Mn, Sn);
      }
    }

    if (EFI_ERROR(Status)) {
      continue;
    }

    //
    // Filter out device whose Mn is empty
    //
    if (Mn[0] == 0) {
      continue;
    }

    (*AllHddInfo)[HddCount].Bus = Bus;
    (*AllHddInfo)[HddCount].Device = Dev;
    (*AllHddInfo)[HddCount].Function = Func;
    CopyMem((*AllHddInfo)[HddCount].Mn, Mn, MAXSIZE_OF_MN * sizeof(CHAR16));
    CopyMem((*AllHddInfo)[HddCount].Sn, Sn, MAXSIZE_OF_SN);
    (*AllHddInfo)[HddCount].DevicePath = DuplicateDevicePath (OriginDevicePath);
    (*AllHddInfo)[HddCount].Selected = FALSE;
    HddCount++;
    FoundMessStorage = FALSE;
  } // end of enumerating loop

  return HddCount;
}

EFI_DEVICE_PATH_PROTOCOL *
CheckHdShortDp (
  IN  EFI_DEVICE_PATH_PROTOCOL        *DevicePath
  )
{
  EFI_STATUS                          Status;
  EFI_DEVICE_PATH_PROTOCOL            *TempDp;
  UINTN                               HandleCount;
  EFI_HANDLE                          *Handles;
  UINTN                               Index;
  EFI_DEVICE_PATH_PROTOCOL            *FsDp = NULL;

  //
  // Check if it's HD short-form DevicePath
  //
  TempDp = DevicePath;
  if ((DevicePathType(TempDp) == MEDIA_DEVICE_PATH) && (DevicePathSubType(TempDp) == MEDIA_HARDDRIVE_DP)) {
    TempDp = NextDevicePathNode(TempDp);
    while (!IsDevicePathEnd(TempDp)) {
      if ((DevicePathType(TempDp) == MEDIA_DEVICE_PATH) && (DevicePathSubType(TempDp) == MEDIA_FILEPATH_DP)) {
        break;
      }
      TempDp = NextDevicePathNode(TempDp);
    }

    if (IsDevicePathEnd(TempDp)) {
      return NULL;
    }
  }

  //
  // Get full DevicePath of this HD short-form option
  //    
  Status = gBS->LocateHandleBuffer (
                   ByProtocol,
                   &gEfiSimpleFileSystemProtocolGuid,
                   NULL,
                   &HandleCount,
                   &Handles
                   );                  
  if (EFI_ERROR(Status)) {
    return NULL;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID**)&FsDp
                    );               
    if (EFI_ERROR(Status)) {
      continue;
    }
    
    TempDp = FsDp;
    while (!IsDevicePathEnd(TempDp)) {
      if (DevicePathType(TempDp) == MEDIA_DEVICE_PATH && DevicePathSubType(TempDp) == MEDIA_HARDDRIVE_DP) {
        break;
      }
      TempDp = NextDevicePathNode(TempDp);
    }
    if (IsDevicePathEnd(TempDp)) {
      continue;
    }

    if (CompareMem(DevicePath, TempDp, DevicePathNodeLength(DevicePath)) == 0) {        // node matched
      return DuplicateDevicePath(FsDp);
    }
  }

  return NULL;
}

/**
  Check if DevicePath is HD Short-Form,and get size of DevicePath:
  True:  return FullPath of DevicePath, and size is the size of the FullPath
minus the size of HD(xxxxx) node;
  False: return DevicePath,and size is size of DevicePath minus size of endnode.
**/
EFI_DEVICE_PATH_PROTOCOL *
ExpandHdShortForm (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *TempDp;

  TempDp = DevicePath;
  if ((DevicePathType(TempDp) == MEDIA_DEVICE_PATH) && (DevicePathSubType(TempDp) == MEDIA_HARDDRIVE_DP)) {
    TempDp = NextDevicePathNode(TempDp);
    while (!IsDevicePathEnd(TempDp)) {
      if ((DevicePathType(TempDp) == MEDIA_DEVICE_PATH) && (DevicePathSubType(TempDp) == MEDIA_FILEPATH_DP)) {
        break;
      }
      TempDp = NextDevicePathNode(TempDp);
    }
    if (!IsDevicePathEnd(TempDp)) {
      TempDp = CheckHdShortDp (DevicePath); // Try to get FullPath of HD Short-Form
      if (TempDp != NULL) {
        return TempDp;
      }
    }
  }

  return DevicePath;
}

/**
  Compare two device paths to check if they are exactly same.

  @param DevicePath1    A pointer to the first device path data structure.
  @param DevicePath2    A pointer to the second device path data structure.

  @retval TRUE    They are same.
  @retval FALSE   They are not same.

**/
BOOLEAN
CompareAllDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath1,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath2              
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Node1;
  EFI_DEVICE_PATH_PROTOCOL  *Node2;

  Node1 = ExpandHdShortForm(DevicePath1);
  Node2 = ExpandHdShortForm(DevicePath2);

  while (!IsDevicePathEnd(Node1) && !IsDevicePathEnd(Node2)) {
    if (CompareMem(Node1, Node2, DevicePathNodeLength(Node1)) != 0) {
      return FALSE;
    }
    Node1 = NextDevicePathNode(Node1);
    Node2 = NextDevicePathNode(Node2);
  }

  if (IsDevicePathEnd(Node1) || IsDevicePathEnd(Node2)) {
    return TRUE;
  }

  return FALSE;
}

/**
  Prepare informations of harddisks
  Build description of harddisk with BDF(Bus/Device/Function Number), MN and SN.
  Like:B7D0F0 INTEL SSDXXXXXXX BTYH01XXXXX
  @return the number of harddisks.
**/
UINT16
EFIAPI
PrepareHarddiskinfo (
  IN CONST EXTEND_HDD_DATA         *AllHddInfo,
  IN UINT16                        AllHddCount,
  IN OUT HDD_BIND_INFO             *CompleteHddInfo
  )
{
  UINTN                     Index;
  UINT16                    HddCount;
  EXTEND_HDD_DATA           *ExtendAllInfo;
  CHAR16                    BdfString[sizeof("BXXXXDXXXXFXXXX")];
  CHAR16                    *TempString;
  UINTN                     Size;

  if ((AllHddInfo == NULL) || (AllHddCount == 0) || (CompleteHddInfo == NULL)) {
    return 0;
  }

  ExtendAllInfo = (EXTEND_HDD_DATA *)AllHddInfo;
  HddCount = 0;
  for (Index = 0; Index < AllHddCount; Index++) {
    if ((ExtendAllInfo[Index].Mn != NULL) && 
        (ExtendAllInfo[Index].Sn != NULL) && 
        (StrStr (ExtendAllInfo[Index].Mn, L"ASMT106") == NULL)) {
      ZeroMem(BdfString, sizeof(BdfString));
      UnicodeSPrint(BdfString, sizeof(BdfString), L"B%xD%xF%x", ExtendAllInfo[Index].Bus,
        ExtendAllInfo[Index].Device, ExtendAllInfo[Index].Function);
      Size = StrLen(BdfString) * sizeof(CHAR16) + StrSize(ExtendAllInfo[Index].Mn)
        + AsciiStrSize(ExtendAllInfo[Index].Sn) * sizeof(CHAR16) + 1 * sizeof(CHAR16);
      TempString = AllocateZeroPool(Size);
      ASSERT(TempString != NULL);
      UnicodeSPrint(TempString, Size, L"%s %s %a", BdfString, ExtendAllInfo[Index].Mn, ExtendAllInfo[Index].Sn);

      DeleteExtraSpaces(TempString);

      //
      // Save informations of hdd which can be listed on HddBind Page
      //
      CompleteHddInfo[HddCount].Description = (CHAR16 *)AllocateCopyPool (StrSize (TempString), TempString);
      CopyMem(CompleteHddInfo[HddCount].Mn, ExtendAllInfo[Index].Mn, StrSize(ExtendAllInfo[Index].Mn));
      CopyMem(CompleteHddInfo[HddCount].Sn, ExtendAllInfo[Index].Sn, AsciiStrSize(ExtendAllInfo[Index].Sn));
      CompleteHddInfo[HddCount].ExtendData = AllocateCopyPool(sizeof(EXTEND_HDD_DATA), &ExtendAllInfo[Index]);
      CompleteHddInfo[HddCount].ExtendDataSize = sizeof(EXTEND_HDD_DATA);
      CompleteHddInfo[HddCount].HddIndex = HddCount + 1;
      HddCount++;
      if (TempString != NULL) {
        FreePool(TempString);
      }
    }
  }
  return HddCount;
}  

/**
  Filter out valid hard disks.
  Feature:
  Valid hard disks:Can match the boot options in hard disk Type.And they will be listed in
  configuration page for users to choose which hard disk to bind.The matching method here
  is to compare the MN(Manufacturer Number) of the hard disk with the MN contained in
  Description of the boot option.

  @param ValidHddInfo return informations of valid hard disks.This parameter is allocated 
  and freed by caller.
  @return  The number of valid hard disks enumerated.
**/
UINT16
EFIAPI
FilterOutValidHdd (
  IN CONST EXTEND_HDD_DATA         *AllHddInfo,
  IN UINT16                        AllHddCount,
  IN UINTN                         CountMax,
  IN OUT HDD_BIND_INFO             *ValidHddInfo
  )
{
  UINTN                     Index;
  EXTEND_HDD_DATA           *ExtendAllInfo;
  UINT16                    HddCount;
  UINTN                     Count;
  UINTN                     BootOptionCount;
  EFI_BOOT_MANAGER_LOAD_OPTION             *BootOptions;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_HANDLE						    DeviceHandle;
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL                 *FullPath = NULL;
  #ifndef BYO_ONLY_UEFI
  EFI_STATUS                LocateStatus;
  UEFI_BBS_RELATION_INFO_PROTOCOL          *UefiBbsInfo = NULL;
  UefiBbsHddRL              *UefiBbsHddTable;
  UINTN                     HBTableIndex;
  UINT16                    BbsIndex;
  #endif
  EFI_DEVICE_PATH_PROTOCOL  *TempDp;

  if ((ValidHddInfo == NULL) || (AllHddInfo == NULL) || (AllHddCount == 0)) {
    return 0;
  }

  BootOptions = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);
  ExtendAllInfo = (EXTEND_HDD_DATA *)AllHddInfo;
  HddCount = 0;

  #ifndef BYO_ONLY_UEFI
  LocateStatus = gBS->LocateProtocol(&gUefiBbsRelationProtocolGuid, NULL, &UefiBbsInfo);
  #endif

  for (Index = 0; Index < BootOptionCount; Index++) {
    //
    // Skip Asmedia RAID device and other RAID or SAS devices
    //
    if ((StrStr (BootOptions[Index].Description, L"ASMT106") != NULL)
      || (StrStr(BootOptions[Index].Description, L" RAID") != NULL)
      || (StrStr(BootOptions[Index].Description, L"SAS:") != NULL)) {
      continue;
    }

    TempDp = NULL;

    //
    // Check if it's HD short-form DevicePath and get fullpath
    //
    FullPath = CheckHdShortDp(BootOptions[Index].FilePath);

    //
    // Filter out valid harddisks from boot options
    //
    #ifndef BYO_ONLY_UEFI
    if (!EFI_ERROR(LocateStatus) && (UefiBbsInfo != NULL)) {
      if ((DevicePathType (BootOptions[Index].FilePath) == BBS_DEVICE_PATH) && (DevicePathSubType (BootOptions[Index].FilePath) == BBS_BBS_DP)) {
        //
        // Get real DevicePath of legacy option from gUefiBbsRelationProtocolGuid
        //
        BbsIndex = LegacyBootOptionalDataGetBbsIndex(BootOptions[Index].OptionalData);
        UefiBbsHddTable = UefiBbsInfo->UefiBbsHddTable;
        for (HBTableIndex = 0; HBTableIndex < UefiBbsInfo->UefiSataHddCount; HBTableIndex++) {
          if (UefiBbsHddTable[HBTableIndex].BbsIndex == BbsIndex) {
            TempDp = UefiBbsHddTable[HBTableIndex].HddDevicePath;
            break;
          }
        }
      }
    }
    #endif

    if ((DevicePathType (BootOptions[Index].FilePath) != BBS_DEVICE_PATH) || (DevicePathSubType (BootOptions[Index].FilePath) != BBS_BBS_DP)) {
      TempDp = BootOptions[Index].FilePath;
    }
    for (Count = 0; Count < AllHddCount; Count++) {
      if (ExtendAllInfo[Count].Selected == TRUE) {
        continue;
      }
      if (TempDp == NULL) {
        if ((DevicePathType (BootOptions[Index].FilePath) == BBS_DEVICE_PATH) && (DevicePathSubType (BootOptions[Index].FilePath) == BBS_BBS_DP)) {
          //
          // compare through Description in Legacy
          //
          if (StrStr(BootOptions[Index].Description, ExtendAllInfo[Count].Mn) != NULL) {
            break;
          }
        }
      } else {
        if (CompareAllDevicePath(TempDp, ExtendAllInfo[Count].DevicePath)) {
          break;
        }
      }
    }

    if (Count < AllHddCount) { // matched
      if (FullPath == NULL) { // Is not HD short-form DevicePath
        DevicePath = BootOptions[Index].FilePath;
        if ((DevicePathType (BootOptions[Index].FilePath) == BBS_DEVICE_PATH) && (DevicePathSubType (BootOptions[Index].FilePath) == BBS_BBS_DP)) {
          #ifndef BYO_ONLY_UEFI
            ExtendAllInfo[Count].BbsIndex = LegacyBootOptionalDataGetBbsIndex(BootOptions[Index].OptionalData);
            DevicePath = ExtendAllInfo[Count].DevicePath;
          #endif
        }
  
        //
        // Filter out device which not supporting BlockIo in UEFI mode
        //      
        Status = gBS->LocateDevicePath(&gEfiBlockIoProtocolGuid, &DevicePath, &DeviceHandle);
        if (EFI_ERROR(Status)) {
          continue;
        }
      }
      
      //
      // Save informations of hdd which can be listed on HddBind Page
      //
      if (HddCount < CountMax) {
        ValidHddInfo[HddCount].Description = (CHAR16 *)AllocateCopyPool (StrSize (BootOptions[Index].Description), BootOptions[Index].Description);
        CopyMem(ValidHddInfo[HddCount].Mn, ExtendAllInfo[Count].Mn, StrSize(ExtendAllInfo[Count].Mn));
        CopyMem(ValidHddInfo[HddCount].Sn, ExtendAllInfo[Count].Sn, AsciiStrSize(ExtendAllInfo[Count].Sn));
        ValidHddInfo[HddCount].ExtendData = AllocateCopyPool(sizeof(EXTEND_HDD_DATA), &ExtendAllInfo[Count]);
        ValidHddInfo[HddCount].ExtendDataSize = sizeof(EXTEND_HDD_DATA);
        ValidHddInfo[HddCount].HddIndex = HddCount + 1;
        ExtendAllInfo[Count].Selected = TRUE;
        HddCount++;
      }

      if (FullPath != NULL) {
        FreePool(FullPath);
      }
    }
  }
  EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);
  return HddCount;
}

VOID 
InitString (
  EFI_HII_HANDLE                             HiiHandle, 
  EFI_STRING_ID                              StrRef, 
  CHAR16                                     *sFormat, ...
  )
{
  STATIC CHAR16 s[1024];
  VA_LIST  Marker;

  VA_START (Marker, sFormat);
  UnicodeVSPrint (s, sizeof (s),  sFormat, Marker);
  VA_END (Marker);
    
  HiiSetString (HiiHandle, StrRef, s, NULL);
}

VOID
HddBindCreateForm (
  VOID
  )
{
  UINTN                     Index;
  VOID                      *StartOpCodeHandle;
  VOID                      *OptionsOpCodeHandle;
  EFI_IFR_GUID_LABEL        *StartLabel;
  VOID                      *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL        *EndLabel;
  UINT16                    StringToken;
  CHAR16                    TempString[DESCRIPTION_STRING_MAX_LENGTH];
  HDD_BIND_INFO             *TempHddInfo;
  BOOLEAN                   BoundHddExist = FALSE;

  TempHddInfo = gPrivateData.HddBindInfo;
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_HDD_BIND_START;

  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_HDD_BIND_END;
  
  OptionsOpCodeHandle = HiiAllocateOpCodeHandle();

  if (gPrivateData.TargetSn == NULL) { // no bind
    gPrivateData.VarData.HddBinded = 0;
    gPrivateData.VarData.BindHddIndex = 0;
  } else {
    gPrivateData.VarData.HddBinded = 1;
  }

  HiiCreateOneOfOptionOpCode (
    OptionsOpCodeHandle,
    STRING_TOKEN(STR_UNBIND),
    0,
    EFI_IFR_TYPE_NUM_SIZE_16,
    0
    );

  if ((gPrivateData.HddCount != 0) && (TempHddInfo != NULL)) {
    //
    // Update the value of HDD_BIND_VAR_DATA.BindHddIndex
    //
    if ((gPrivateData.VarData.HddBinded != 0) && (gPrivateData.Description != NULL)) {
      for (Index = 0; Index < gPrivateData.HddCount; Index++) {
        if (CompareMem(gPrivateData.TargetSn, TempHddInfo[Index].Sn, AsciiStrSize(gPrivateData.TargetSn)) == 0) {
          BoundHddExist = TRUE;
          if (TempHddInfo[Index].HddIndex != gPrivateData.VarData.BindHddIndex) {
            gPrivateData.VarData.BindHddIndex = TempHddInfo[Index].HddIndex;
          }
          break;
        }
      }

      if (!BoundHddExist) {
        gPrivateData.VarData.BindHddIndex = INDEX_OF_NO_EXIST_HDD;
        HiiCreateOneOfOptionOpCode (
          OptionsOpCodeHandle,
          STRING_TOKEN(STR_BOUND_HDD_NOT_EXIST),
          0,
          EFI_IFR_TYPE_NUM_SIZE_16,
          gPrivateData.VarData.BindHddIndex
          );
      }

      ZeroMem(TempString, sizeof(TempString));
      #ifndef DESCRIPTION_FROM_HDD_WITH_BDF
        if (!BoundHddExist) {
          UnicodeSPrint(TempString, sizeof(TempString), L"%s", gPrivateData.Description);
        } else {
          UnicodeSPrint(TempString, sizeof(TempString), L"%s", TempHddInfo[Index].Description);
        }
        HiiSetString(gPrivateData.HiiHandle, STRING_TOKEN(STR_CUR_BOOT_HDD_VALUE), TempString, NULL);
      #else
        UnicodeSPrint(TempString, sizeof(TempString), L"%s", gPrivateData.TargetMn);
        HiiSetString(gPrivateData.HiiHandle, STRING_TOKEN(STR_CUR_BOOT_HDD_VALUE), TempString, NULL);
        ZeroMem(TempString, sizeof(TempString));
        UnicodeSPrint(TempString, sizeof(TempString), L"SN:%a", gPrivateData.TargetSn);
        HiiSetString(gPrivateData.HiiHandle, STRING_TOKEN(STR_SN_TEXT), TempString, NULL);
      #endif
    }

    for (Index = 0; Index < gPrivateData.HddCount; Index++) {
      ZeroMem(TempString, sizeof(TempString));
      UnicodeSPrint(TempString, sizeof(TempString), L"%s", TempHddInfo[Index].Description);
      StringToken = HiiSetString(gPrivateData.HiiHandle, 0, TempString, NULL);
      HiiCreateOneOfOptionOpCode (
        OptionsOpCodeHandle,
        StringToken,
        0,
        EFI_IFR_TYPE_NUM_SIZE_16,
        TempHddInfo[Index].HddIndex
        );
    }
  }

  if ((gPrivateData.HddCount == 0) && (gPrivateData.TargetSn != NULL)) {
    gPrivateData.VarData.BindHddIndex = INDEX_OF_NO_EXIST_HDD;
    HiiCreateOneOfOptionOpCode (
      OptionsOpCodeHandle,
      STRING_TOKEN(STR_BOUND_HDD_NOT_EXIST),
      0,
      EFI_IFR_TYPE_NUM_SIZE_16,
      gPrivateData.VarData.BindHddIndex
      );
  }

  HiiCreateOneOfOpCode (
    StartOpCodeHandle,
    HDD_BIND_ONEOF_ITEM_ID,
    HDD_BIND_VARSTORE_ID,
    OFFSET_OF(HDD_BIND_VAR_DATA, BindHddIndex),
    STRING_TOKEN(STR_HDD_BIND_ITEM_NAME),
    STRING_TOKEN(STR_HDD_BIND_ITEM_HELP),
    EFI_IFR_FLAG_CALLBACK,
    EFI_IFR_TYPE_NUM_SIZE_16,                        // <--- !!!
    OptionsOpCodeHandle,
    NULL
    );

  HiiUpdateForm (
    gPrivateData.HiiHandle,
    &gHddBindFormsetGuid,        // Formset GUID
    HDD_BIND_FORM_ID,               // Form ID
    StartOpCodeHandle,              // Label for where to insert opcodes
    EndOpCodeHandle                 // Replace data
    );

  if (StartOpCodeHandle != NULL) {
    HiiFreeOpCodeHandle(StartOpCodeHandle);
  }
  if (EndOpCodeHandle != NULL) {
    HiiFreeOpCodeHandle(EndOpCodeHandle);
  }
  if (OptionsOpCodeHandle != NULL) {
    HiiFreeOpCodeHandle(OptionsOpCodeHandle);
  }

  gRT->SetVariable(  
          HDD_BIND_VAR_NAME, 
          &gHddBindFormsetGuid, 
          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
          sizeof (HDD_BIND_VAR_DATA),
          &gPrivateData.VarData
          );
}

VOID
EFIAPI
HddBindEnterSetupCallback (
  IN EFI_EVENT                               Event,
  IN VOID*                                   Context
  )
{
  EFI_STATUS                Status;
  VOID                      *Interface;
  UINTN                     HandleCount;
  UINTN                     Size;
  EFI_HANDLE                *Handles = NULL;

  Status = gBS->LocateProtocol(&gEfiSetupEnterGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  //
  // Install Device Path Protocol and Config Access protocol to driver handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gPrivateData.DriverHandle,
                  &gEfiDevicePathProtocolGuid,      &gHddBindHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid, &gPrivateData.ConfigAccess,
                  &gEfiCallerIdGuid, NULL,
                  NULL
                  );
  ASSERT(!EFI_ERROR(Status));

  gPrivateData.HiiHandle = HiiAddPackages (
                         &gHddBindFormsetGuid,
                         gPrivateData.DriverHandle,
                         HddBindDxeStrings,
                         HddBindDxeVfrBin,
                         NULL
                         );
  ASSERT(gPrivateData.HiiHandle != NULL);

  GetHddBindNvData();
  //
  // Get informations about hard disks that can be bound
  //
  Status = gBS->LocateHandleBuffer (
                ByProtocol,
                &gEfiDiskInfoProtocolGuid,
                NULL,
                &HandleCount,
                &Handles
                );
  if (EFI_ERROR(Status)) {
    gPrivateData.HddCount = 0;
    gPrivateData.HddBindInfo = NULL;
  } else {
    gPrivateData.AllHddCount = EnumerateAllHddInfo(&gPrivateData.AllHddInfo);
    if ((gPrivateData.AllHddCount != 0) && (gPrivateData.AllHddInfo != NULL)) {
      //
      // Filter out hard disks that can be bound
      //
      gPrivateData.HddBindInfo = (HDD_BIND_INFO*)AllocateZeroPool(sizeof(HDD_BIND_INFO) * HandleCount);
      ASSERT(gPrivateData.HddBindInfo != NULL);
      #ifndef DESCRIPTION_FROM_HDD_WITH_BDF
        gPrivateData.HddCount = FilterOutValidHdd(gPrivateData.AllHddInfo, gPrivateData.AllHddCount, HandleCount, gPrivateData.HddBindInfo);
      #else
        gPrivateData.HddCount = PrepareHarddiskinfo(gPrivateData.AllHddInfo, gPrivateData.AllHddCount, gPrivateData.HddBindInfo);
      #endif
    
      FreePool(gPrivateData.AllHddInfo);
    }
  }
  
  Size = sizeof(HDD_BIND_VAR_DATA);
  gRT->GetVariable (
          HDD_BIND_VAR_NAME,
          &gHddBindFormsetGuid,
          NULL,
          &Size,
          &gPrivateData.VarData
          );
  gPrivateData.VarData.HddCount = gPrivateData.HddCount;

  //
  // create form
  //
  HddBindCreateForm();
}

EFI_STATUS
EFIAPI
ExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                Status;
  UINTN                     BufferSize;
  HDD_BIND_PRIVATE_DATA     *Private;
  EFI_STRING                ConfigRequestHdr;
  EFI_STRING                ConfigRequest;
  BOOLEAN                   AllocatedRequest;
  UINTN                     Size;

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;

  if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, &gHddBindFormsetGuid, HDD_BIND_VAR_NAME)) {
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
    ConfigRequestHdr = HiiConstructConfigHdr (&gHddBindFormsetGuid, HDD_BIND_VAR_NAME, Private->DriverHandle);
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    FreePool (ConfigRequestHdr);
  }

  Status = gRT->GetVariable (
            HDD_BIND_VAR_NAME,
            &gHddBindFormsetGuid,
            NULL,
            &BufferSize,
            &Private->VarData
            );
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

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

EFI_STATUS
EFIAPI
RouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  EFI_STATUS                Status;
  HDD_BIND_VAR_DATA         VarData;
  UINTN                     Size;

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;

  //
  // Check routing data in <ConfigHdr>.
  // Note: if only one Storage is used, then this checking could be skipped.
  //
  if (!HiiIsConfigHdrMatch (Configuration, &gHddBindFormsetGuid, HDD_BIND_VAR_NAME)) {
    return EFI_NOT_FOUND;
  }

  //
  // Convert <ConfigResp> to buffer data by helper function ConfigToBlock()
  //
  Size = sizeof (HDD_BIND_VAR_DATA);
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
  // Save variables
  //
  Status = gRT->SetVariable(  
            HDD_BIND_VAR_NAME, 
            &gHddBindFormsetGuid, 
            EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
            sizeof (HDD_BIND_VAR_DATA),
            &VarData
            );
  ASSERT(!EFI_ERROR(Status));
  if (VarData.BindHddIndex != INDEX_OF_NO_EXIST_HDD) {
    Status = SaveHddBindNvData(gPrivateData.UserChoice, &VarData);
  }

  return Status;
}

EFI_STATUS
EFIAPI
DriverCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   GetBrowserDataResult;
  HDD_BIND_VAR_DATA         VarData;  

  if (Action == EFI_BROWSER_ACTION_FORM_OPEN || Action == EFI_BROWSER_ACTION_FORM_CLOSE) {
    return EFI_SUCCESS;
  }

  if (Action == EFI_BROWSER_ACTION_CHANGED) { 
    if (QuestionId == HDD_BIND_ONEOF_ITEM_ID) {
      GetBrowserDataResult = HiiGetBrowserData(&gHddBindFormsetGuid, HDD_BIND_VAR_NAME, sizeof(HDD_BIND_VAR_DATA), (UINT8 *)&VarData);
      if (GetBrowserDataResult) {
        if (VarData.BindHddIndex == 0) {
          gPrivateData.UserChoice = FALSE;
        } else {
          gPrivateData.UserChoice = TRUE;
        }
      }
    }
  } else if (Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
    if (QuestionId == HDD_BIND_ONEOF_ITEM_ID) {
      gPrivateData.UserChoice = FALSE;
      gPrivateData.TargetSn = NULL;
      GetBrowserDataResult = HiiGetBrowserData(&gHddBindFormsetGuid, HDD_BIND_VAR_NAME, sizeof(HDD_BIND_VAR_DATA), (UINT8 *)&VarData);
      if (GetBrowserDataResult) {
        VarData.BindHddIndex = 0;
        VarData.HddBinded = 0;
      }
      HiiSetBrowserData(&gHddBindFormsetGuid, HDD_BIND_VAR_NAME, sizeof(HDD_BIND_VAR_DATA), (UINT8 *)&VarData, NULL);
      HddBindCreateForm();
      return EFI_UNSUPPORTED;
    }
  }
  
  Status = EFI_SUCCESS;

  return Status;  
}

EFI_STATUS
EFIAPI
HddBindDxeInit (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                Status;
  VOID                      *Registration;
  EFI_RSC_HANDLER_PROTOCOL  *StatusCodeRouter;
  EFI_EVENT                 ExitBootEvent;

  if (PcdGet8(PcdIsPlatformCmosBad) || PcdGetBool(PcdFceLoadDefault)) {
    Status = gRT->SetVariable(  
               HDD_BIND_NV_NAME, 
               &gHddBindNvVarGuid, 
               0,
               0,
               NULL
               );
  }
  
  Status = gBS->LocateProtocol (&gEfiRscHandlerProtocolGuid,
                  NULL /* Registration */, (VOID **)&StatusCodeRouter);                                 
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Register the handler callback.
  //
  Status = StatusCodeRouter->Register (HandleStatusCode, TPL_CALLBACK);  
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a:%a: failed to register status code handler: %r\n",
      gEfiCallerBaseName, __FUNCTION__, Status));
    return Status;
  }

  //
  // Unregister the handler callback before exiting from the Boot Time
  //
  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES, // Type
                  TPL_CALLBACK,                  // NotifyTpl
                  UnregisterAtExitBootServices,  // NotifyFunction
                  StatusCodeRouter,              // NotifyContext
                  &ExitBootEvent                 // Event
                  );
  if (EFI_ERROR (Status)) {
    //
    // We have to unregister the callback right now, and fail the function.
    //
    DEBUG ((DEBUG_ERROR, "%a:%a: failed to create ExitBootServices() event: "
      "%r\n", gEfiCallerBaseName, __FUNCTION__, Status));
    StatusCodeRouter->Unregister (HandleStatusCode);
    return Status;
  }  

  //
  // Add error message strings
  //
  gStringHandle = HiiAddPackages (
                            &gEfiCallerIdGuid,
                            ImageHandle,
                            HddBindDxeStrings,
                            NULL
                            );
  ASSERT (gStringHandle!= NULL);  
  
  EfiCreateProtocolNotifyEvent (
    &gEfiSetupEnterGuid,
    TPL_CALLBACK,
    HddBindEnterSetupCallback,
    NULL,
    &Registration
    );
  
  return Status;
}