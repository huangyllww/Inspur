/** @file
#
#Copyright (c) 2017, Byosoft Corporation.<BR>
#All rights reserved.This software and associated documentation (if any)
#is furnished under a license and may only be used or copied in
#accordance with the terms of the license. Except as permitted by such
#license, no part of this software or documentation may be reproduced,
#stored in a retrieval system, or transmitted in any form or by any
#means without the express written consent of Byosoft Corporation.
#
#File Name:
#  BootOption.c
#
#Abstract:
#  ServerMgmt features via IPMI commands (IPMI2.0 Spc rev1.1 2013)
#
#Revision History:
#
#TIME:       2017-7-7
#$AUTHOR:    Phinux Qin
#$REVIEWERS:
#$SCOPE:     All BMC Platforms
#$TECHNICAL:
#
# T.O.D.O
#   modify BBS USB type to hold USB root port info so that we can support USB remote media
#
#$END----------------------------------------------------------------------------------------------------------------
#
--*/

#include "IpmiBootOption.h"
#include <Protocol/LegacyBios.h>
#include <Guid/LegacyDevOrder.h>
#include <Library/ByoUefiBootManagerLib.h>
#include <Protocol/DevicePath.h>
#include <ByoBootGroup.h>
#include <Library/ByoCommLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/BlockIo.h>
#include <SysMiscCfg2.h>

#define  DEFAULT_IPMI_BOOT_OPTION     0x500

#define  IPMIBOOTDEBUG(x)

STATIC UINT32   mRemoteUsbPortNumber = 0xFFFFFFFF;

EFI_HANDLE     *mSimpleFileSystemHandles = NULL;
UINTN           NumberSimpleFileSystemHandles;

//
//The follow Guid is not a standard EDK definition, but it has been used for many GENs.
//So copy it here becaused it would be located in different *.dec.
//
STATIC EFI_GUID mUiAppFileGuid = {
  0x462CAA21, 0x7614, 0x4503, { 0x83, 0x6E, 0x8A, 0xB6, 0xF4, 0x66, 0x23, 0x31 }
};

//
//Notes: the follow two functions are not defined in extern head file in EfiBootManagerLib.
//
extern
EFI_STATUS
LegacyBmCreateLegacyBootOption (
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption,
  IN BBS_TABLE                         *BbsEntry,
  IN UINT16                            BbsIndex
  );

extern
VOID
BmPrintDp (
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath
  );
/**

    GC_TODO: add routine description

    @param Option     - GC_TODO: add arg description
    @param VariableAttributes     - GC_TODO: add arg description

    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description
    @retval EFI_NOT_FOUND         - GC_TODO: add retval description
    @retval Status                - GC_TODO: add retval description

**/
EFI_STATUS
BootOptionToVariable (
  IN EFI_BOOT_MANAGER_LOAD_OPTION  *Option,
  IN UINT32                        VariableAttributes
  )
{
  UINTN                            VariableSize;
  UINT8                            *Variable;
  UINT8                            *Ptr;
  CHAR16                           OptionName[sizeof (L"Boot####")];
  CHAR16                           *Description;
  CHAR16                           NullChar;

  if ((NULL == Option) || (NULL == Option->FilePath)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Convert NULL description to empty description
  //
  NullChar    = L'\0';
  Description = Option->Description;
  if (Description == NULL) {
    Description = &NullChar;
  }

  /*
  UINT32                      Attributes;
  UINT16                      FilePathListLength;
  CHAR16                      Description[];
  EFI_DEVICE_PATH_PROTOCOL    FilePathList[];
  UINT8                       OptionalData[];
  */
  VariableSize = sizeof (Option->Attributes)
               + sizeof (UINT16)
               + StrSize (Description)
               + GetDevicePathSize (Option->FilePath)
               + Option->OptionalDataSize;

  Variable     = AllocatePool (VariableSize);
  if (NULL == Variable) {
    return EFI_OUT_OF_RESOURCES;
  }

  Ptr             = Variable;
  WriteUnaligned32 ((UINT32 *) Ptr, Option->Attributes);
  Ptr            += sizeof (Option->Attributes);

  WriteUnaligned16 ((UINT16 *) Ptr, (UINT16) GetDevicePathSize (Option->FilePath));
  Ptr            += sizeof (UINT16);

  CopyMem (Ptr, Description, StrSize (Description));
  Ptr            += StrSize (Description);

  CopyMem (Ptr, Option->FilePath, GetDevicePathSize (Option->FilePath));
  Ptr            += GetDevicePathSize (Option->FilePath);

  CopyMem (Ptr, Option->OptionalData, Option->OptionalDataSize);

  UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", Option->OptionNumber);

  gRT->SetVariable (
         OptionName,
         &gEfiGlobalVariableGuid,
         VariableAttributes,
         VariableSize,
         Variable
         );

  if (NULL != Variable) {
    FreePool (Variable);
  }
  return EFI_SUCCESS;
}

/**

    GC_TODO: add routine description

    @param OptionNum     - GC_TODO: add arg description
    @param LegacyDeviceType     - GC_TODO: add arg description
    @param Instance     - GC_TODO: add arg description

    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description
    @retval EFI_NOT_FOUND         - GC_TODO: add retval description
    @retval Status                - GC_TODO: add retval description

**/
EFI_STATUS
CreateNewLegacyBootOption (
  IN UINT16                       OptionNum,
  IN UINT16                       CurrentBbsIndex,
  IN UINT8                        LegacyDeviceType,
  IN UINT8                        Instance
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         IsFound;
  UINT16                          HddCount;
  UINT16                          BbsCount;
  UINT16                          TargetBbsIndex;
  HDD_INFO                        *HddInfo;
  BBS_TABLE                       *BbsTable;
  EFI_LEGACY_BIOS_PROTOCOL        *LegacyBios;
  LEGACY_DEV_ORDER_ENTRY          *TmpPtr;
  LEGACY_DEV_ORDER_ENTRY          *LegacyDevOrder;
  UINTN                           LegacyDevOrderSize;
  EFI_BOOT_MANAGER_LOAD_OPTION    BootOption;

  if (0 == Instance || Instance > 0x0F) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Status = LegacyBios->GetBbsInfo (
                         LegacyBios,
                         &HddCount,
                         &HddInfo,
                         &BbsCount,
                         &BbsTable
                         );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  GetVariable2 (VAR_LEGACY_DEV_ORDER, &gEfiLegacyDevOrderVariableGuid, (VOID **) &LegacyDevOrder, &LegacyDevOrderSize);
  if (NULL == LegacyDevOrder || LegacyDevOrderSize < sizeof (LEGACY_DEV_ORDER_ENTRY)) {
    return EFI_DEVICE_ERROR;
  }

  IsFound = FALSE;
  TmpPtr = LegacyDevOrder;
  while ((UINT8 *)TmpPtr < ((UINT8 *)LegacyDevOrder + LegacyDevOrderSize)) {
    if (LegacyDeviceType == TmpPtr->BbsType) {
      if (0 == TmpPtr->Length) {
        DEBUG ((EFI_D_ERROR, "Critical progrom error in Legacy Device Order Type[%x]", TmpPtr->BbsType));
        Status = EFI_DEVICE_ERROR;
        goto ERR_EXIT;
      }
      if (TmpPtr->Length != sizeof (UINT16)) {
        if (TmpPtr->Length < Instance * sizeof (UINT16)) {
          DEBUG ((EFI_D_ERROR, "Ipmi Boot Order: This instance:%x is overflow in Type:%x", Instance, TmpPtr->BbsType));
          Status =  EFI_DEVICE_ERROR;
          goto ERR_EXIT;
        }
        IsFound = TRUE;
      }
      break;
    }
    TmpPtr = (LEGACY_DEV_ORDER_ENTRY *)((UINT8 *)TmpPtr + TmpPtr->Length + sizeof (UINT8));
  }

  if (TRUE == IsFound) {
    TargetBbsIndex = (TmpPtr->Data[Instance - 1]) & 0xFF00;
    if (CurrentBbsIndex == TargetBbsIndex) {
      //
      //T.O.D.O, One Legacy device order is in disabled by setup. re-enable it??
      //
      DEBUG ((EFI_D_ERROR, "The device instance is in using\n"));
      Status = EFI_SUCCESS;
      goto ERR_EXIT;
    }
    if ((BBS_IGNORE_ENTRY != BbsTable[TargetBbsIndex].BootPriority) &&
       (BBS_DO_NOT_BOOT_FROM != BbsTable[TargetBbsIndex].BootPriority) &&
       (BBS_LOWEST_PRIORITY != BbsTable[TargetBbsIndex].BootPriority)) {
      DEBUG ((EFI_D_ERROR, "The device instance is invalid by Bbs.BootPriority %x\n", BbsTable[TargetBbsIndex].BootPriority));
      Status = EFI_UNSUPPORTED;
      goto ERR_EXIT;
    }
    DEBUG ((EFI_D_INFO, "Find instance %x in type %x and prepare to create new boot option\n", Instance, TmpPtr->BbsType));
    Status = LegacyBmCreateLegacyBootOption (&BootOption, &BbsTable[TargetBbsIndex], TargetBbsIndex);
    if (EFI_ERROR (Status)) {
      goto ERR_EXIT;
    }
    BootOption.OptionNumber = OptionNum;
    if (DEFAULT_IPMI_BOOT_OPTION == OptionNum) {
      Status = BootOptionToVariable (&BootOption, (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS));
    } else {
      Status = BootOptionToVariable (&BootOption, (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE));
    }
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Fail to save Boot%04x\n"));
      EfiBootManagerFreeLoadOption (&BootOption);
      goto ERR_EXIT;
    }
    //
    //Important notes:
    //There are two policies to change L"BootXXXX" and L"LegacyDevOrder"
    //1. LegacyDevOrder type.data[0] will determin the BootXXXX.
    //2. BootXXXX will use the type.data[x] and nerver be impated by L"LegacyDevOrder"
    //
    //I am not sure what policy is being used in the current BKC., so update the L"LeagaDevOder" to advoid policy 1.
    //
    if (DEFAULT_IPMI_BOOT_OPTION != OptionNum) {
      TmpPtr->Data[Instance - 1] = TmpPtr->Data[0];
      TmpPtr->Data[0] = TargetBbsIndex;
      Status = gRT->SetVariable (
                 VAR_LEGACY_DEV_ORDER,
                 &gEfiLegacyDevOrderVariableGuid,
                 EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                 LegacyDevOrderSize,
                 LegacyDevOrder
                 );
      DEBUG ((EFI_D_INFO, "Save LegacyDevOrder %r\n", Status));
    }
    EfiBootManagerFreeLoadOption (&BootOption);
  }

ERR_EXIT:
  if (NULL != LegacyDevOrder) {
    FreePool (LegacyDevOrder);
  }
  return Status;
}

/**

    GC_TODO: add routine description

    @param BootOptionDP     - GC_TODO: add arg description

    @retval TRUE          - GC_TODO: add retval description
    @retval FALSE         - GC_TODO: add retval description

**/
BOOLEAN
IsPxe (
  IN EFI_DEVICE_PATH_PROTOCOL        *BootOptionDp
  )
{
  EFI_STATUS                         Status;
  EFI_HANDLE                         Handle;
  EFI_DEVICE_PATH_PROTOCOL           *Dp;

  if (NULL == BootOptionDp) {
    return FALSE;
  }
  Dp = BootOptionDp;
  Status = gBS->LocateDevicePath (&gEfiLoadFileProtocolGuid, &Dp, &Handle);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  return TRUE;
}

/**

    GC_TODO: add routine description

    @param BootOptionDP     - GC_TODO: add arg description

**/
EFI_DEVICE_PATH_PROTOCOL *
FindBestDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL        *BootOptionDp
  )
{
  EFI_STATUS                         Status;
  UINTN                              Index;
  EFI_DEVICE_PATH_PROTOCOL           *TempDevicePath;
  UINTN                              Size;
  UINTN                              TempSize;

  if (NULL == mSimpleFileSystemHandles) {
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiSimpleFileSystemProtocolGuid,
                    NULL,
                    &NumberSimpleFileSystemHandles,
                    &mSimpleFileSystemHandles
                    );
    if (EFI_ERROR(Status) || (NULL == mSimpleFileSystemHandles)) {
       if (mSimpleFileSystemHandles) {
         FreePool (mSimpleFileSystemHandles);
         mSimpleFileSystemHandles = NULL;
       }
      return NULL;
    }
  }
  DEBUG ((EFI_D_INFO, "Numbers of SimpleFileSytem instance are 0x%x\n", NumberSimpleFileSystemHandles));
  Size = GetDevicePathSize (BootOptionDp) - END_DEVICE_PATH_LENGTH;
  for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
    //
    // Get the device path size of SimpleFileSystem handle
    //
    TempDevicePath = DevicePathFromHandle (mSimpleFileSystemHandles[Index]);
    if (NULL == TempDevicePath) {
      continue;
    }
    TempSize = GetDevicePathSize (TempDevicePath) - END_DEVICE_PATH_LENGTH;
    //
    // Find the best device path.
    //
    if ((Size <= TempSize) && (CompareMem (TempDevicePath, BootOptionDp, Size) == 0)) {
      return DuplicateDevicePath (TempDevicePath);
    }
  }
  return NULL;
}
/**

    GC_TODO: add routine description

    @param BootOptionDP     - GC_TODO: add arg description
    @param IsCheckRemote     - GC_TODO: add arg description
    @param SubType     - GC_TODO: add arg description
    @param FileGuid     - GC_TODO: add arg description

    @retval TRUE          - GC_TODO: add retval description
    @retval FALSE         - GC_TODO: add retval description

**/
BOOLEAN
IsMediaMatched (
  IN EFI_DEVICE_PATH_PROTOCOL        *BootOptionDp,
  IN BOOLEAN                         IsCheckRemote,
  IN UINT8                           SubType,
  IN UINT8                           *FileGuid
  )
{
  UINT8                              Matches;
  EFI_DEVICE_PATH_PROTOCOL           *BootDevicePath;
  BOOLEAN                            Result;
  EFI_DEVICE_PATH_PROTOCOL      *Dp;
  EFI_HANDLE                    DevHandle;
  EFI_STATUS                    Status;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  UINT8                         ClassCode[3];
  BOOLEAN                       IsODD;
  EFI_BLOCK_IO_PROTOCOL         *BlockIo;
  BM_MENU_TYPE                  GroupType = BM_MENU_TYPE_MAX;
  EFI_DEVICE_PATH_PROTOCOL      *DevPath;

  Result = FALSE;
  BootDevicePath = NULL;
  if (NULL == BootOptionDp) {
    return Result;
  }

  //
  //Check File Media.
  //
  if ((MEDIA_PIWG_FW_FILE_DP == SubType) && ((NULL != FileGuid))) {
    IPMIBOOTDEBUG ((EFI_D_INFO, "Check the File Media "));
    for (Dp = BootOptionDp; !IsDevicePathEnd (Dp); Dp = NextDevicePathNode (Dp)) {
      if ((DevicePathType (Dp) == MEDIA_DEVICE_PATH) && (DevicePathSubType (Dp) == MEDIA_PIWG_FW_FILE_DP)) {
        //
        //Check File Guid
        //
        if (!CompareMem (FileGuid, &(((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *)Dp)->FvFileName), sizeof(EFI_GUID))) {
          IPMIBOOTDEBUG ((EFI_D_INFO, "Found \n"));
          Result = TRUE;
          goto EXIT;
        }
        IPMIBOOTDEBUG ((EFI_D_INFO, "Not Found \n"));
        goto EXIT;
      }
    }
    IPMIBOOTDEBUG ((EFI_D_INFO, "Not Found \n"));
    goto EXIT;
  }

  BootDevicePath = FindBestDevicePath (BootOptionDp);
  if (NULL == BootDevicePath && SubType != MEDIA_CDROM_DP) {
    DEBUG ((EFI_D_INFO, "Boot Device Path Fail\n"));
    goto EXIT;
  }

  if(SubType != MEDIA_CDROM_DP){
  BmPrintDp (BootDevicePath);
  DEBUG ((EFI_D_INFO, "\n"));
  //
  //Check Remote Media
  //
  Matches = 0;
  if (IsCheckRemote) {
    IPMIBOOTDEBUG ((EFI_D_INFO, "Check the remote Media "));
    //
    //Check iBMC remote media first
    //
    for (Dp = BootDevicePath; !IsDevicePathEnd (Dp); Dp = NextDevicePathNode (Dp)) {
      DEBUG ((EFI_D_INFO, "1:T[%d]ST[%d]\n", DevicePathType (Dp), DevicePathSubType (Dp)));
      if ((DevicePathType (Dp) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Dp) == MSG_USB_CLASS_DP) ||
	  	(DevicePathType (Dp) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Dp) ==MSG_USB_DP)) {
         if ((((USB_DEVICE_PATH *) Dp)->ParentPortNumber == (mRemoteUsbPortNumber &0xff)) || 
             (((USB_DEVICE_PATH *) Dp)->ParentPortNumber == ((mRemoteUsbPortNumber >> 8) &0xff))) {
            //
            //use Matches to contine to check whether other Dp matches the given Type
            //
            Matches = 1;
         }
      }
      if (1 == Matches) {
	  	if(SubType == 0xff){
		  SubType=1;
		}
        if ((DevicePathType (Dp) == MEDIA_DEVICE_PATH) && (DevicePathSubType (Dp) == SubType)) {
          IPMIBOOTDEBUG ((EFI_D_INFO, "(Find the iBMC remote Media Type)\n"));
          Result = TRUE;
          goto EXIT;
        }
      }
    }
    if (Matches) {
      IPMIBOOTDEBUG ((EFI_D_INFO, "(Not Found)\n"));
      goto EXIT;
    }
    //
    //Check Fiber channel and iSCSI
    //
    Matches = 0;
    for (Dp = BootDevicePath; !IsDevicePathEnd (Dp); Dp = NextDevicePathNode (Dp)) {
      DEBUG ((EFI_D_INFO, "2:T[%d]ST[%d]\n", DevicePathType (Dp), DevicePathSubType (Dp)));
      if ((DevicePathType (Dp) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Dp) == MSG_ISCSI_DP) ||
        (DevicePathType (Dp) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Dp) == MSG_FIBRECHANNEL_DP)||
        (DevicePathType (Dp) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Dp) == MSG_SATA_DP)||
        (DevicePathType (Dp) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Dp) == MSG_NVME_NAMESPACE_DP)) {
         Matches = 1;
      }
      if (1 == Matches) {
        //
        //Just for Hard Driver subtype
        //
        if ((DevicePathType (Dp) == MEDIA_DEVICE_PATH) && (DevicePathSubType (Dp) == SubType)) {
          IPMIBOOTDEBUG ((EFI_D_INFO, "(Find the Remote Hdd)\n"));
          Result = TRUE;
          goto EXIT;
        }
      }
    }
    IPMIBOOTDEBUG ((EFI_D_INFO, "(Not Found)\n"));
    goto EXIT;
  }

  //
  //Check Local Media
  //
  IPMIBOOTDEBUG ((EFI_D_INFO, "Check the local Media "));
  for (Dp = BootDevicePath; !IsDevicePathEnd (Dp); Dp = NextDevicePathNode (Dp)) {
    DEBUG ((EFI_D_INFO, "3:T[%d]ST[%d]\n ", DevicePathType (Dp), DevicePathSubType (Dp)));
    if ((DevicePathType (Dp) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Dp) == MSG_USB_CLASS_DP)) {
       if ((((USB_DEVICE_PATH *) Dp)->ParentPortNumber == (mRemoteUsbPortNumber &0xff)) || 
           (((USB_DEVICE_PATH *) Dp)->ParentPortNumber == ((mRemoteUsbPortNumber >> 8) &0xff))) {
          //
          //use Matches to contine to check whether this is a remote cd/hdd/floppy
          //
         IPMIBOOTDEBUG ((EFI_D_INFO, "(This is iBMC remote Media)\n"));
         goto EXIT;
       }
    }
    if ((DevicePathType (Dp) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Dp) == MSG_ISCSI_DP) ||
      (DevicePathType (Dp) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Dp) == MSG_FIBRECHANNEL_DP)) {
      IPMIBOOTDEBUG ((EFI_D_INFO, "(This is iSCSI/FiberChannel remote Media)\n"));
      goto EXIT;
    }

	if((DevicePathType (Dp) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Dp) == MSG_USB_DP) ) {
	  if(SubType == 0xff){
	  	SubType = 1;
	  }
    }
    if ((DevicePathType (Dp) == MEDIA_DEVICE_PATH) && (DevicePathSubType (Dp) == SubType)) {
      IPMIBOOTDEBUG ((EFI_D_INFO, "(Find the Local Media Type)\n"));
      Result = TRUE;
      goto EXIT;
    }
  } 
  }else if(SubType == MEDIA_CDROM_DP){  
  	DEBUG((EFI_D_INFO,"-----------SubType = MEDIA_CDROM_DP---------\n"));
  	IsODD = FALSE;
    DevPath = BootOptionDp;

	Status = gBS->LocateDevicePath(&gEfiBlockIoProtocolGuid, &DevPath, &DevHandle);
	if(Status != EFI_SUCCESS){
	  goto EXIT;
	}
    if(!EFI_ERROR(Status) && IsDevicePathEnd(DevPath)){
      Status = gBS->HandleProtocol(DevHandle, &gEfiBlockIoProtocolGuid, &BlockIo);
      if(BlockIo->Media->BlockSize == 2048){
        IsODD = TRUE;
      }
    }  
	DevPath = BootOptionDp;
    Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &DevPath, &DevHandle);
    if(EFI_ERROR(Status)){
      Result = FALSE;
      goto EXIT;
    }
    Status = gBS->HandleProtocol(DevHandle, &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 9, 3, ClassCode);
	DEBUG((EFI_D_INFO,"BlockSize = %d,ClassCode[2] =%d\n",BlockIo->Media->BlockSize,ClassCode[2]));
	if(ClassCode[2] != 0x0c || IsODD == FALSE){
      Result = FALSE;
      goto EXIT;
	}
    DEBUG ((EFI_D_INFO, "found \n"));
    Result = TRUE;
    goto EXIT;

  }
  DEBUG ((EFI_D_INFO, "(Not Found)"));
  DEBUG ((EFI_D_INFO, "\n"));
EXIT:
  if (BootDevicePath) {
    FreePool (BootDevicePath);
  }
  return Result;
}

/**

    GC_TODO: add routine description

    @param BootOptionDP     - GC_TODO: add arg description
    @param IpmiBootDeviceSelector     - GC_TODO: add arg description

    @retval TRUE          - GC_TODO: add retval description
    @retval FALSE         - GC_TODO: add retval description

**/
BOOLEAN
IsMeetBootDeviceSelector (
  IN EFI_DEVICE_PATH_PROTOCOL        *BootOptionDp,
  IN UINT8                           IpmiBootDeviceSelector
  )
{
  switch (IpmiBootDeviceSelector) {
    case FORCE_PXE:  //Force Pxe
      return IsPxe (BootOptionDp);
    case LOCAL_HDD:  //Local Hdd
      return IsMediaMatched (BootOptionDp, TRUE, MEDIA_HARDDRIVE_DP, NULL);
    case LOCAL_DVD:  //Local DVD
      return IsMediaMatched (BootOptionDp, FALSE, MEDIA_CDROM_DP, NULL);
    case FORCE_SETUP:  //Force to enter setup
      return IsMediaMatched (BootOptionDp, TRUE, MEDIA_PIWG_FW_FILE_DP, (UINT8 *) &mUiAppFileGuid);
    case REMOTE_FLOPPY:  //Remote floppy
      return IsMediaMatched (BootOptionDp, TRUE, 0xFF, NULL);
    case REMOTE_IMAGE_FILE:  //Remote Iso, remote image file?
      return IsMediaMatched (BootOptionDp, TRUE, MEDIA_PIWG_FW_FILE_DP, PcdGetPtr(PcdShellFile));
    case REMOTE_DVD:  //Remote Dvd
      return IsMediaMatched (BootOptionDp, TRUE, MEDIA_CDROM_DP, NULL);
    case REMOTE_HDD:  //Remote Hdd
      return IsMediaMatched (BootOptionDp, TRUE, MEDIA_HARDDRIVE_DP, NULL);
    case LOCAL_FLOPPY: //Local floppy
      return IsMediaMatched (BootOptionDp, FALSE, 0xFF, NULL);
    default:
    break;
  }

  return FALSE;
}

/**

    GC_TODO: add routine description

**/
VOID
FreeIpmiBootOptionStruct (
  VOID
  )
{
  if (mSimpleFileSystemHandles) {
    FreePool (mSimpleFileSystemHandles);
    mSimpleFileSystemHandles = NULL;
  }
}
/**

    GC_TODO: add routine description

**/
VOID
SetBootVarsFromLastPersistent (
  VOID
  )
{
  UINT32     *IpmiBootOption = NULL;
  UINTN      BootOptionSize;
  UINT16     BootIndex;

  GetVariable2 (L"IpmiBootOption", &gEfiIpmiBootOptionGuid, (VOID **) &IpmiBootOption, &BootOptionSize);

  if (NULL == IpmiBootOption|| BootOptionSize != sizeof (UINT32)) {
    return;
  }
  //
  //Bit15:0 for BootXXXX on previous "Force to Setup" Persistently
  //
  BootIndex = (UINT16)(*IpmiBootOption);
  //
  //Update the L"BootNext".
  //
  gRT->SetVariable (
         L"BootNext",
         &gEfiGlobalVariableGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
         sizeof (UINT16),
         &BootIndex
         );
  DEBUG ((EFI_D_INFO, "Update [%04x] to BootNext %r\n due to last persistent setting", BootIndex));

  PcdSetBoolS (PcdIpmiBootToSetup, TRUE);
  FreePool (IpmiBootOption);
}

EFI_STATUS 
BmcBootNextVariableUpdate (
  UINT16 OptionNumber
)
{
    EFI_STATUS       Status;
    UINT16          *BootNext;
    UINT16          *BootNextTemp;
    UINTN            BootNextSize;

    DEBUG((EFI_D_ERROR, __FUNCTION__"(), Next OptionNumber :%d.\n\n", OptionNumber));

  Status = EFI_SUCCESS;
  BootNext = NULL;
  BootNext = EfiBootManagerGetVariableAndSize (
                  L"BmcBootNext",
                  &gBMCOrderGuid,
                  &BootNextSize
                  );
  if (BootNext == NULL) {
    BootNextSize = sizeof (UINT16);
    Status = gRT->SetVariable (
                              L"BmcBootNext",
                              &gBMCOrderGuid,
                              EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                              BootNextSize,
                              &OptionNumber
                              );
  } else {
    BootNextSize += sizeof (UINT16);
    BootNextTemp = AllocateCopyPool(BootNextSize, BootNext);
    if (BootNextTemp) {
      BootNextTemp[BootNextSize/sizeof(UINT16) -1] = OptionNumber;
      DumpMem8(BootNextTemp,BootNextSize);

      Status = gRT->SetVariable (
                              L"BmcBootNext",
                              &gBMCOrderGuid,
                              EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                              BootNextSize,
                              BootNextTemp
                              );
      DEBUG((EFI_D_ERROR, __FUNCTION__"(), Set BmcBootNext :%r, BootNextSize :%d.\n", Status, BootNextSize));
    }
    FreePool (BootNext);
  }
    
  if (!EFI_ERROR(Status)) {
    PcdSetBool(PcdIpmiBootNext,TRUE);
  } else {
    PcdSetBool(PcdIpmiBootNext,FALSE);
  }
  
  return Status;
}

/**

    GC_TODO: add routine description

    @param BootOptionParam5     - GC_TODO: add arg description
    @param BmcUsbPortsInfo      - GC_TODO: add arg description

    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description
    @retval EFI_NOT_FOUND         - GC_TODO: add retval description
    @retval Status                - GC_TODO: add retval description

**/
EFI_STATUS
OverrideBootVars (
  IN SYSTEM_BOOT_OPTION_PARAMETER5  *BootOptionParam5,
  IN UINT32                         BmcUsbPortsInfo
  )
{
  UINTN                             BootOrderSize = 0;
  UINT16                            *BootOrder = NULL;
  UINT16                            Index;
  UINT16                            OptionName[sizeof (L"Boot####")];
  IPMI_BOOT_DEVICE                  IpmiBootDevice;
  UINT8                             TargetDeviceType = 0xFF;
  UINT8                             LegacyTargetDeviceType = 0xFF;
  EFI_STATUS                        Status;
  EFI_BOOT_MANAGER_LOAD_OPTION      BootOption;
  UINTN                             UefiBootDeviceFirstMeet;
  UINT16                             UefiBootDeviceMeets;
  UINT8                             BiosBootType;
 
  

  if (NULL == BootOptionParam5) {
    return EFI_INVALID_PARAMETER;
  }

  BiosBootType = PcdGet8(PcdIpmiBootMode);
  
  switch (BootOptionParam5->Data2.Bits.BootDeviceSelector) {
    case 0x01:  //Force Pxe
      IpmiBootDevice = FORCE_PXE;
    break;
    case 0x02:  //Local Hdd
      IpmiBootDevice = LOCAL_HDD;
    break;
    case 0x05:  //Local DVD
      IpmiBootDevice = LOCAL_DVD;
    break;
    case 0x06:  //Remote floppy
      IpmiBootDevice = FORCE_SETUP;	  
    break;
    case 0x07:  //Remote floppy
      IpmiBootDevice = REMOTE_FLOPPY;
    break;
    case 0x09:  //Remote Iso, remote image file?
      IpmiBootDevice = REMOTE_IMAGE_FILE;
    break;
    case 0x08:  //Remote Dvd
      IpmiBootDevice = REMOTE_DVD;
    break;
    case 0x0b:  //Remote Hdd
      IpmiBootDevice = REMOTE_HDD;
    break;
    case 0x0f: //Local floppy
      IpmiBootDevice = LOCAL_FLOPPY;
    break;
    default:
    return EFI_UNSUPPORTED;
  }

  //
  //T.O.D.O, for Byosoft CSM, we plan to support usb port identification.
  //@Important note: 
  // because remoley disk is iBMC remote media device and there is no definition for how to identify such like USB device from
  // legacy BBS table, so, on legacy boot, we merge remote device into local device.
  // It means we will not parse L"LegacyDev" to check whether a legacy device is Local or remote because BBS spc does not 
  // define how to identify a USB device is local or local. (iBMC embeded V.s others)
  //
  if (BIOS_BOOT_LEGACY_OS == BiosBootType) { //For legacy boot
    DEBUG ((EFI_D_INFO, "Legacy "));
    switch (IpmiBootDevice) {
    case FORCE_PXE:
      LegacyTargetDeviceType = BBS_EMBED_NETWORK;
    break;
    case LOCAL_HDD:
    case REMOTE_HDD:
      LegacyTargetDeviceType = BBS_HARDDISK;
    break;
    case LOCAL_FLOPPY:
    case REMOTE_FLOPPY:
      LegacyTargetDeviceType = BBS_FLOPPY;
    break;
    case LOCAL_DVD:
    case REMOTE_DVD:
      LegacyTargetDeviceType = BBS_CDROM;
    break;
    //case FORCE_SETUP:
    //case REMOTE_IMAGE_FILE:
    //  TargetDeviceType = IpmiBootDevice;
    //break;
    default:
      LegacyTargetDeviceType = 0xFF;
    break;
    }
  } else {
     DEBUG ((EFI_D_INFO, "Efi "));
     TargetDeviceType = IpmiBootDevice;
  }
  DEBUG ((EFI_D_INFO, "Target Boot Device Type efi 0x%x or legacy 0x%x\n", TargetDeviceType, LegacyTargetDeviceType));

  GetVariable2 (L"BootOrder", &gEfiGlobalVariableGuid, (VOID **) &BootOrder, &BootOrderSize);
  if (NULL == BootOrder || BootOrderSize < sizeof (UINT16)) {
    return EFI_UNSUPPORTED;
  }

  //
  //Walk through boot order list to find a match device type and instance, then  update L"BootXXXX" and L"BootNext"
  //
  UefiBootDeviceMeets = 0;

  for (Index = 0; Index < BootOrderSize / sizeof (UINT16); Index++) {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOrder[Index]);
    DEBUG ((EFI_D_INFO, "Boot%04x", BootOrder[Index]));
    
    Status = EfiBootManagerVariableToLoadOption (OptionName, &BootOption);

    if (EFI_ERROR (Status)) {
      continue;
    }
    DEBUG ((EFI_D_INFO, ".Attributes(0x%08x) ", BootOption.Attributes));
    if (BootOption.Description) {
      DEBUG ((EFI_D_INFO, "%s\n", BootOption.Description));
    }
    if (0 == (BootOption.Attributes & LOAD_OPTION_ACTIVE)){
      EfiBootManagerFreeLoadOption (&BootOption);
      continue;
    } else {
      
      //
      //Support Force to Internal EFI Shell/Setup both Legacy and EFI boot mode.
      //
      if ((REMOTE_IMAGE_FILE == TargetDeviceType) ||
        (FORCE_SETUP == TargetDeviceType)) {
        //
        //Force to Boot to Shell
        //
        if (TRUE == IsMeetBootDeviceSelector (BootOption.FilePath, TargetDeviceType)) {
            Status = BmcBootNextVariableUpdate((UINT16)BootOption.OptionNumber);

          UefiBootDeviceMeets++;
          UefiBootDeviceFirstMeet = Index;
          EfiBootManagerFreeLoadOption (&BootOption);
        }
        EfiBootManagerFreeLoadOption (&BootOption);
        continue; //Try Next Bootxxxx
      }
      //
      //@Important note: if  boot option bit is legacy, we get the target legacy boot option first, then break.
      //if  boot option bit is Efi, we get the first type, then break.
      //
      // Legacy boot (maybe we need support legacy+uefi simultaneously instead of one ..the other)
      //
      //
      if (BIOS_BOOT_LEGACY_OS == BiosBootType) {  // to be commented out
        if (NULL != BootOption.FilePath &&
           (BBS_DEVICE_PATH == DevicePathType (BootOption.FilePath)) &&
           (BBS_BBS_DP == (DevicePathSubType (BootOption.FilePath))) &&
           ((NULL != BootOption.OptionalData) && (BootOption.OptionalDataSize))) {
           //
           //Search the targe type
           //
           DEBUG((EFI_D_INFO,"LegacyTargetDeviceType = %0x,DeviceType = %0x\n",LegacyTargetDeviceType,((BBS_BBS_DEVICE_PATH *)(BootOption.FilePath))->DeviceType));
		   if((LegacyTargetDeviceType == 0x02 && (StrnCmp(L"USB",BootOption.Description,3) != 0) && ((BBS_BBS_DEVICE_PATH *)(BootOption.FilePath))->DeviceType == 0x02) ||
		   	  (LegacyTargetDeviceType == 0x01 && (StrnCmp(L"USB",BootOption.Description,3) == 0) && ((BBS_BBS_DEVICE_PATH *)(BootOption.FilePath))->DeviceType == 0x02) ||
              (LegacyTargetDeviceType != 0xFF && LegacyTargetDeviceType != 0x02 && LegacyTargetDeviceType != 0x01 &&  LegacyTargetDeviceType == ((BBS_BBS_DEVICE_PATH *)(BootOption.FilePath))->DeviceType)) {
              
             Status = BmcBootNextVariableUpdate((UINT16)BootOption.OptionNumber);
		     UefiBootDeviceFirstMeet = Index;
             UefiBootDeviceMeets++;
             EfiBootManagerFreeLoadOption (&BootOption);
           } 
         }
      }else {  //UEFI boot
         if (TRUE == IsMeetBootDeviceSelector (BootOption.FilePath, TargetDeviceType)) {
           Status = BmcBootNextVariableUpdate((UINT16)BootOption.OptionNumber);
           UefiBootDeviceFirstMeet = Index;
           UefiBootDeviceMeets++;
           EfiBootManagerFreeLoadOption (&BootOption);
         }
      }
    } //Try Next Bootxxxx
  }

  //
  //If Persistent is set, we need change&save L"BootOrder", maybe backup L"BootNext" because we will invald IPMI 
  // Boot Option flag after exiting.
  //
  if (BootOptionParam5->Data1.Bits.Persistent) {
  	DEBUG((EFI_D_INFO,"UefiBootDeviceFirstMeet = %d\n",UefiBootDeviceFirstMeet));
    if (UefiBootDeviceMeets && (((FORCE_SETUP != TargetDeviceType) && (0xFF != TargetDeviceType)) || (0xFF != LegacyTargetDeviceType))) {

      UefiBootDeviceMeets = BootOrder[0];
      BootOrder[0] = BootOrder[(UINT16)UefiBootDeviceFirstMeet];
      BootOrder[(UINT16)UefiBootDeviceFirstMeet] = (UINT16)UefiBootDeviceMeets;
      DEBUG ((EFI_D_INFO, "BootOrder[0000] <=> BootOrder[%04x] :new value 0x%04x 0x%4x", (UINT16)UefiBootDeviceFirstMeet, BootOrder[0], BootOrder[(UINT16)UefiBootDeviceFirstMeet]));

      Status = gRT->SetVariable (
                 L"BootOrder",
                 &gEfiGlobalVariableGuid,
                 EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                 BootOrderSize,
                 BootOrder
                 );
      DEBUG ((EFI_D_INFO, "Save BootOrder %r\n", Status));

      Status = gRT->SetVariable (
                 L"IpmiBootOption",
                 &gEfiIpmiBootOptionGuid,
                 EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                 0,
                 NULL
                 );

    } else if (UefiBootDeviceMeets && (FORCE_SETUP == TargetDeviceType)) {
      UINT32     IpmiBootOption;

      //
      //Backup "Fore To Setup" in only when Persistent(=0x01) and BootDeviceSelector(=0x06).
      //
      IpmiBootOption = BootOrder[(UINT16)UefiBootDeviceFirstMeet];
      Status = gRT->SetVariable (
                 L"IpmiBootOption",
                 &gEfiIpmiBootOptionGuid,
                 EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                 sizeof (UINT32),
                 &IpmiBootOption
                 );
      DEBUG ((EFI_D_INFO, "Backup Boot0x%4d [0x%4x] for Setup %r", (UINT16)UefiBootDeviceFirstMeet, IpmiBootOption, Status));
    }
  } else {
    if (UefiBootDeviceMeets) {
        /*
      //
      //Update the L"BootNext".
      //
      Status = gRT->SetVariable (
                 L"BootNext",
                 &gEfiGlobalVariableGuid,
                 EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                 sizeof (UINT16),
                 &BootOrder[(UINT16)UefiBootDeviceFirstMeet]
                 );
      DEBUG ((EFI_D_INFO, "Save [%04x] BootNext %r\n", BootOrder[(UINT16)UefiBootDeviceFirstMeet], Status));
      */
    }
     
    //
    //whatevert the result is, clear L"IpmiBootOption" to advoid to enter setup again.
    //
    Status = gRT->SetVariable (
               L"IpmiBootOption",
               &gEfiIpmiBootOptionGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               0,
               NULL
               );
  }
  if (0 == UefiBootDeviceMeets) {
    DEBUG ((EFI_D_INFO, "Ipmiboot option:Can not find the device\n"));
  }

  if (NULL != BootOrder) {
    FreePool (BootOrder);
  }  

  return Status;
}