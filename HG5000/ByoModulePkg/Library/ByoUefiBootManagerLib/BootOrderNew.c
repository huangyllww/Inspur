
#include "InternalBdsLib.h"
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <ByoBootGroup.h>
#include <Library/ByoCommLib.h>
#include <Library/LegacyBootOptionalDataLib.h>
#include <SysMiscCfg2.h>
#include <Library/PlatformCommLib.h>
#include <SetupVariable.h>




UINT8
GetEfiNetWorkType (
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  OUT CHAR16                     **TypeStr
  );

CHAR16 *
GetEfiNetWorkString (
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  OUT BOOLEAN                     *IsPxeBootCard
  );

EFI_STATUS 
AddShellBootOption(
  EFI_BOOT_MANAGER_LOAD_OPTION **BootOptions,
  UINTN                        *BootOptionCount
  );

CHAR16 *
AppendOsDescription (
  IN EFI_HANDLE                Handle
  );

extern GET_BOOT_DESCRIPTION mGetBootDescription[];
extern UINTN                mGetBootDescriptionCount;
extern CHAR16               mUefiPrefix[];
extern UINT8                gBlockIoTypeArray[];
extern UINTN                gBlockIoTypeArraySize;

EFI_BOOT_MANAGER_LOAD_OPTION *
EFIAPI
EfiBootManagerEnumerateBootOptions2 (
  UINTN                                 *BootOptionCount,
  UINT8                                 *UefiGroupOrder, 
  UINTN                                 GroupOrderSize
  )
{
  EFI_STATUS                            Status;
  EFI_BOOT_MANAGER_LOAD_OPTION          *BootOptions;
  EFI_BOOT_MANAGER_LOAD_OPTION          *NewBootOptions;
  UINT16                                NonBlockNumber;
  UINTN                                 HandleCount;
  EFI_HANDLE                            *Handles;
  EFI_BLOCK_IO_PROTOCOL                 *BlkIo;
  UINTN                                 Index;
  UINTN                                 FunctionIndex;
  CHAR16                                *Temp;
  CHAR16                                *OsDescriptionPtr;  
  CHAR16                                *DescriptionPtr;
  CHAR16                                Description[180];
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  CHAR16                                *DevStr;
  CHAR16                                *BootTypeStr;
  CHAR16                                *IpTypeStr;
  UINT8                                 IpType;
  CONST VOID                            *OptData;
  UINTN                                 OptDataSize;
  UINT8                                 NetBootIp4Ip6;
  PLAT_HOST_INFO_PROTOCOL               *HostInfo;  
  UINTN                                 i,Size;
  UINTN                                 NewOptionIndex;
  BOOLEAN                               NoUsbBoot;
  EFI_DEVICE_PATH_PROTOCOL              *Dp;
  UINT8                                 *BlockIoRec = NULL;
  UINTN                                 TypeIndex;
  UINT8                                 DeviceType;
  PCI_DEVICE_PATH  						*PciM2Dp;
  BOOLEAN                   			HddM2 = FALSE;
  BOOLEAN                               bePxeBootEnable = TRUE;

  ASSERT (BootOptionCount != NULL);

  NetBootIp4Ip6 = PcdGet8(PcdNetBootIp4Ip6);
  if(PcdGet32(PcdSystemMiscConfig) & SYS_MISC_CFG_NO_USB_BOOT){
    NoUsbBoot = TRUE;
  } else {
    NoUsbBoot = FALSE;
  }

  *BootOptionCount = 0;
  BootOptions      = NULL;

  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiBlockIoProtocolGuid,
         NULL,
         &HandleCount,
         &Handles
         );

  if(HandleCount != 0){
    BlockIoRec = AllocateZeroPool(HandleCount);
    ASSERT(BlockIoRec);
  }

  for (TypeIndex = 0; TypeIndex < gBlockIoTypeArraySize; TypeIndex++){
    for (Index = 0; Index < HandleCount; Index++) {

      if(BlockIoRec[Index]){
        continue;
      }
      
      Status = gBS->HandleProtocol (
                      Handles[Index],
                      &gEfiBlockIoProtocolGuid,
                      (VOID **) &BlkIo
                      );
      ASSERT(!EFI_ERROR(Status));

      Dp = DevicePathFromHandle(Handles[Index]);

	  if(TypeIndex == 0){
  	  	while (!IsDevicePathEndType (Dp)) {
		  if ((Dp->Type == HARDWARE_DEVICE_PATH) && (Dp->SubType == HW_PCI_DP)) { 				   
			 PciM2Dp = (PCI_DEVICE_PATH *) Dp;
			 DEBUG((EFI_D_ERROR, "PciM2Dp->Device %x, PciM2Dp->Function %x\n",PciM2Dp->Device, PciM2Dp->Function)); 					
			if ((PciM2Dp->Device == 1)&&(PciM2Dp->Function == 7)){	
					DEBUG((EFI_D_ERROR, "GetUefiHddTypeIndex	M2.....\n"));	
					HddM2 = TRUE;
				}
			break;
		} 
		  
		Dp = NextDevicePathNode (Dp);
  	  }    
	  if(HddM2 == FALSE){
	  	continue;
	  	} 
      Dp = DevicePathFromHandle(Handles[Index]);
	  }
      DeviceType = GetDeviceTypeFromDp(Dp);

      if(gBlockIoTypeArray[TypeIndex] != 0xFF && gBlockIoTypeArray[TypeIndex] != DeviceType){
        continue;
      }
      
      if(DeviceType == MSG_USB_DP && NoUsbBoot){
        BlockIoRec[Index] = 1;
        continue;
      }

      //
      // Skip the logical partitions
      //
      if (BlkIo->Media->LogicalPartition) {
        BlockIoRec[Index] = 1;
        continue;
      }

      DescriptionPtr = NULL;
      for (FunctionIndex = 0; FunctionIndex < mGetBootDescriptionCount; FunctionIndex++) {
        DescriptionPtr = mGetBootDescription[FunctionIndex] (Handles[Index]);
        if (DescriptionPtr != NULL) {
          break;
        }
      }

      if (DescriptionPtr == NULL) {
        BlockIoRec[Index] = 1;
        continue;
      }

      //
      // Avoid description confusion between UEFI & Legacy boot option by adding "UEFI " prefix
      //
      OsDescriptionPtr = AppendOsDescription(Handles[Index]);
      if(OsDescriptionPtr != NULL){
        Size = StrSize (DescriptionPtr) + StrSize(OsDescriptionPtr)+4;
        Temp = AllocatePool(Size);
        UnicodeSPrint (Temp, Size, L"%s(%s)", OsDescriptionPtr, DescriptionPtr);
        DEBUG((EFI_D_INFO,"DescriptionPtr is %s\n",DescriptionPtr));
      }else{
        Temp = StrCat (
                 StrCpy (
                   AllocatePool (StrSize (DescriptionPtr) + StrSize(mUefiPrefix)),
                   mUefiPrefix
                   ),
                 DescriptionPtr
                 );
      }
      if(OsDescriptionPtr!=NULL){
        FreePool (OsDescriptionPtr);
      }
      FreePool (DescriptionPtr);
      DescriptionPtr = Temp;

      BootOptions = ReallocatePool (
                      sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*BootOptionCount),
                      sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*BootOptionCount + 1),
                      BootOptions
                      );
      ASSERT (BootOptions != NULL);

      Status = EfiBootManagerInitializeLoadOption (
                 &BootOptions[(*BootOptionCount)++],
                 LoadOptionNumberUnassigned,
                 LoadOptionTypeBoot,
                 LOAD_OPTION_ACTIVE,
                 DescriptionPtr,
                 Dp,
                 NULL,
                 0
                 );
      ASSERT_EFI_ERROR (Status);
      FreePool (DescriptionPtr);
      BlockIoRec[Index] = 1;
      
    }    // block io
  }
  if (HandleCount != 0) {
    FreePool(Handles);
    FreePool(BlockIoRec);
  }

  //
  // Parse simple file system not based on block io
  //
  NonBlockNumber = 0;
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiSimpleFileSystemProtocolGuid,
         NULL,
         &HandleCount,
         &Handles
         );
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **) &BlkIo
                    );
    if (!EFI_ERROR (Status)) {
      continue;
    }
    UnicodeSPrint (Description, sizeof (Description), NonBlockNumber > 0 ? L"%s %d" : L"%s", L"UEFI Non-Block Boot Device", NonBlockNumber);
    
    BootOptions = ReallocatePool (
                    sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*BootOptionCount),
                    sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*BootOptionCount + 1),
                    BootOptions
                    );
    ASSERT (BootOptions != NULL);

    Status = EfiBootManagerInitializeLoadOption (
               &BootOptions[(*BootOptionCount)++],
               LoadOptionNumberUnassigned,
               LoadOptionTypeBoot,
               LOAD_OPTION_ACTIVE,
               Description,
               DevicePathFromHandle (Handles[Index]),
               NULL,
               0
               );
    ASSERT_EFI_ERROR (Status);
  }

  if (HandleCount != 0) {
    FreePool (Handles);
  }

  //
  // Parse load file, assuming UEFI Network boot option
  //
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiLoadFileProtocolGuid,
         NULL,
         &HandleCount,
         &Handles
         );
  
  for (Index = 0; Index < HandleCount; Index++) {
    IpType      = 0xFF;
    DevStr      = L"";
    BootTypeStr = L"";
    bePxeBootEnable = TRUE;

    Status = gBS->HandleProtocol(
                    Handles[Index],
                    &gEfiDevicePathProtocolGuid,
                    &DevicePath
                    );
    if (!EFI_ERROR (Status)) {
      IpType = GetEfiNetWorkType(DevicePath, &BootTypeStr);
      DevStr = GetEfiNetWorkString(DevicePath, &bePxeBootEnable);
    }
    //DEBUG((EFI_D_ERROR, __FUNCTION__"(), bePxeBootEnable :%d.\n", bePxeBootEnable));
    if(!bePxeBootEnable){
      continue;
    }

    if ((NetBootIp4Ip6 == NET_BOOT_IP_V4 && IpType == MSG_IPv6_DP) || 
        (NetBootIp4Ip6 == NET_BOOT_IP_V6 && IpType == MSG_IPv4_DP) ||
        IpType == 0xFF) {
      continue;
    }

    switch(IpType){
      case MSG_IPv4_DP:
        IpTypeStr = L"IPv4";
        break;
      case MSG_IPv6_DP:
        IpTypeStr = L"IPv6";
        break;
      default:
        IpTypeStr = L"";
        break;
    }    

    UnicodeSPrint(
      Description, 
      sizeof(Description), 
      L"UEFI %s %s %s", 
      DevStr, 
      BootTypeStr,
      IpTypeStr
      );

    BootOptions = ReallocatePool (
                    sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*BootOptionCount),
                    sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*BootOptionCount + 1),
                    BootOptions
                    );
    ASSERT (BootOptions != NULL);

    Status = EfiBootManagerInitializeLoadOption (
               &BootOptions[(*BootOptionCount)++],
               LoadOptionNumberUnassigned,
               LoadOptionTypeBoot,
               LOAD_OPTION_ACTIVE,
               Description,
               DevicePathFromHandle (Handles[Index]),
               NULL,
               0
               );
    ASSERT_EFI_ERROR (Status);
  }

  if (HandleCount != 0) {
    FreePool (Handles);
  }


// Shell
  AddShellBootOption(&BootOptions, BootOptionCount);

  OptData = PcdGetPtr(PcdEfiBootOptionFlag);
  OptDataSize = PcdGetSize(PcdEfiBootOptionFlag);
  for (Index = 0; Index < *BootOptionCount; Index++) {
    BootOptions[Index].OptionalData     = AllocateCopyPool(OptDataSize, OptData);
    BootOptions[Index].OptionalDataSize = (UINT32)OptDataSize;
  }

// update position.
  if(*BootOptionCount >= 2){
    NewBootOptions = AllocatePool(*BootOptionCount * sizeof(EFI_BOOT_MANAGER_LOAD_OPTION));
    ASSERT(NewBootOptions != NULL);
    NewOptionIndex = 0;

    for(Index = 0; Index < *BootOptionCount; Index++){  
      BootOptions[Index].Deleted = 1;
    }
    for(i=0;i<GroupOrderSize;i++){
      for(Index = 0; Index < *BootOptionCount; Index++){
        if(BootOptions[Index].GroupType != UefiGroupOrder[i]){
          continue;
        }
        BootOptions[Index].Deleted = 0;
        CopyMem(&NewBootOptions[NewOptionIndex++], &BootOptions[Index], sizeof(EFI_BOOT_MANAGER_LOAD_OPTION));
      }
    }
    for(Index = 0; Index < *BootOptionCount; Index++){  
      if(BootOptions[Index].Deleted){
        EfiBootManagerFreeLoadOption(&BootOptions[Index]);
      }
    }

    FreePool(BootOptions);
    BootOptions = NewBootOptions;
    *BootOptionCount = NewOptionIndex;

    Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &HostInfo);
    if(!EFI_ERROR(Status) && HostInfo->UpdateBootOption != NULL){
      HostInfo->UpdateBootOption(&BootOptions, BootOptionCount);
    }
  }

  return BootOptions;
}

#if BC_TENCENT
UINT8 gDefaultUefiBootGroupOrder[] = {
  BM_MENU_TYPE_UEFI_HDD, 
  BM_MENU_TYPE_UEFI_PXE,
  BM_MENU_TYPE_UEFI_USB_ODD,
  BM_MENU_TYPE_UEFI_USK_DISK,
  BM_MENU_TYPE_UEFI_OTHERS
  };
#else
UINT8 gDefaultUefiBootGroupOrder[] = {
  BM_MENU_TYPE_UEFI_HDD, 
  //BM_MENU_TYPE_UEFI_ODD, 
  BM_MENU_TYPE_UEFI_USK_DISK,
  BM_MENU_TYPE_UEFI_PXE,
  BM_MENU_TYPE_UEFI_USB_ODD,
  BM_MENU_TYPE_UEFI_OTHERS
  };
#endif

// [FS]       PciRoot(0x0)/Pci(0x3,0x0)/Pci(0x0,0x0)/Pci(0x8,0x0)/Pci(0x0,0x0)/Pci(0xF,0x0)/Sata(0x2,0xFFFF,0x0)/
//            HD(2,GPT,5F343B3C-ADE7-4F05-8FF0-6A2A0AC55B86,0xFA000,0x32000)
// [WIN_NV]   HD(2,GPT,5F343B3C-ADE7-4F05-8FF0-6A2A0AC55B86,0xFA000,0x32000)/\EFI\Microsoft\Boot\bootmgfw.efi
// [BIOS_NV]  PciRoot(0x0)/Pci(0x3,0x0)/Pci(0x0,0x0)/Pci(0x8,0x0)/Pci(0x0,0x0)/Pci(0xF,0x0)/Sata(0x2,0xFFFF,0x0)
BOOLEAN
CheckOsShortDp (
  EFI_BOOT_MANAGER_LOAD_OPTION  *Option,
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption,
  UINTN                         BootOptionCount,
  EFI_BOOT_MANAGER_LOAD_OPTION  *NvBootOption,
  UINTN                         NvBootOptionCount,
  OUT  CHAR16                   **OsDesc
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *NvDp;
  EFI_DEVICE_PATH_PROTOCOL      *FsDp = NULL;
  EFI_DEVICE_PATH_PROTOCOL      *TempDp;
  EFI_STATUS                    Status;
  UINTN                         HandleCount;
  EFI_HANDLE                    *Handles;
  UINTN                         Index;
  CHAR16                        *PathName;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Fs;
  EFI_FILE_PROTOCOL                *RootFile;
  BOOLEAN                          DpValid = FALSE;
  UINTN                            DpSize;



  NvDp = Option->FilePath;
  if (DevicePathType(NvDp) == MEDIA_DEVICE_PATH && DevicePathSubType(NvDp) == MEDIA_HARDDRIVE_DP) {

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
      
      if(CompareMem(NvDp, TempDp, DevicePathNodeLength(NvDp)) != 0){        // node match ?
        continue;
      }

      DEBUG((EFI_D_INFO, "target HD found\n"));
      TempDp = NextDevicePathNode(NvDp);
      if(DevicePathType(TempDp) == MEDIA_DEVICE_PATH && DevicePathSubType(TempDp) == MEDIA_FILEPATH_DP){
        PathName = ((FILEPATH_DEVICE_PATH*)TempDp)->PathName;
      } else {
        PathName = EFI_REMOVABLE_MEDIA_FILE_NAME;
      }

      Status = gBS->HandleProtocol (
                      Handles[Index],
                      &gEfiSimpleFileSystemProtocolGuid,
                      (VOID**)&Fs
                      ); 
      Status = Fs->OpenVolume(Fs, &RootFile);
      if(!EFI_ERROR(Status)){
        if(IsFilePresent(gBS, RootFile, PathName)){
          DEBUG((EFI_D_INFO, "%s found\n", PathName));
          DpValid = TRUE;
        } else {
          DEBUG((EFI_D_INFO, "%s not found\n", PathName));
        }
        RootFile->Close(RootFile);
      }
      break;
    }

  }

  if(DpValid){
    for(Index=0;Index<BootOptionCount;Index++){
      DpSize = GetDevicePathSize(BootOption[Index].FilePath);
      if(DpSize < 4){
        continue;
      }
        if(CompareMem(BootOption[Index].FilePath, FsDp, DpSize-4) == 0){
          DEBUG((EFI_D_INFO, "delete BOOT[%d] %s\n", Index, BootOption[Index].Description));
          BootOption[Index].Deleted = TRUE;
          *OsDesc = BootOption[Index].Description;
          Option->DupDp = DuplicateDevicePath(BootOption[Index].FilePath);
          BootOption[Index].DupDp = DuplicateDevicePath(Option->FilePath);
          break;
        }
      }
      

    for(Index=0;Index<NvBootOptionCount;Index++){
      DpSize = GetDevicePathSize(NvBootOption[Index].FilePath);
      if(DpSize < 4){
        continue;
      }
        if(CompareMem(NvBootOption[Index].FilePath, FsDp, DpSize-4) == 0){
          DEBUG((EFI_D_INFO, "delete NvBOOT[%d] %s\n", Index, NvBootOption[Index].Description));
		      DpValid = TRUE;
          NvBootOption[Index].Deleted = TRUE;
          break;
        }    
  }
 }
  return DpValid;
}


UINT16
GetUefiHddTypeIndex (
  EFI_BOOT_MANAGER_LOAD_OPTION *Option
  )
{
  UINT16                    Index;
  EFI_DEVICE_PATH_PROTOCOL  *Dp;
  SATA_DEVICE_PATH          *SataAhciDp;
  ATAPI_DEVICE_PATH         *SataIdeDp;
  BOOLEAN                   Found = FALSE;
  BOOLEAN                   HddM2 = FALSE;
  PCI_DEVICE_PATH		    *PciDp;


  Dp = Option->FilePath;  

  while (!IsDevicePathEndType (Dp)) {
		  if ((Dp->Type == HARDWARE_DEVICE_PATH) && (Dp->SubType == HW_PCI_DP)) { 				   
			 PciDp = (PCI_DEVICE_PATH *) Dp;
			 DEBUG((EFI_D_ERROR, "PciDp->Device %x, PciDp->Function %x\n",PciDp->Device, PciDp->Function)); 					
			if ((PciDp->Device == 1)&&(PciDp->Function == 5)&&(PcdGet8(PcdSmileline) ==3)){	
					DEBUG((EFI_D_ERROR, "GetUefiHddTypeIndex	M2.....\n"));	
					HddM2 = TRUE;
				}
			if((PciDp->Device == 1)&&(PciDp->Function == 6)&&(PcdGet8(PcdSmileline) !=3)){
                    DEBUG((EFI_D_ERROR, "GetUefiHddTypeIndex	M2.....\n"));	
					HddM2 = TRUE;
			}
			break;
			  } 			   
		Dp = NextDevicePathNode (Dp);
  }    

  while(!(Dp->Type == END_DEVICE_PATH_TYPE &&
          Dp->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE)){
    if(Dp->Type == MESSAGING_DEVICE_PATH){
      Found = TRUE;
      break;
    }
    Dp = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)Dp + (Dp->Length[1]<<8) + Dp->Length[0]);
  }

  if(!Found && Option->DupDp != NULL){
    Dp = Option->DupDp;  
    while(!(Dp->Type == END_DEVICE_PATH_TYPE &&
            Dp->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE)){
      if(Dp->Type == MESSAGING_DEVICE_PATH){
        Found = TRUE;
        break;
      }
      Dp = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)Dp + (Dp->Length[1]<<8) + Dp->Length[0]);
    }
  }

  if(!Found){
    return 0xFFFF;
  }

  switch(Dp->SubType){
       
    case MSG_SATA_DP:
      SataAhciDp = (SATA_DEVICE_PATH*)Dp;
	  if(HddM2 == TRUE)
      	Index = SataAhciDp->HBAPortNumber + 0x20;
	  else
	  	Index = SataAhciDp->HBAPortNumber + 0x25;
      break;
	  
    case MSG_NVME_NAMESPACE_DP:
      Index = 0x40;
      break;

      
    case MSG_ATAPI_DP:
      SataIdeDp = (ATAPI_DEVICE_PATH*)Dp;
      Index = SataIdeDp->PrimarySecondary*2 + SataIdeDp->SlaveMaster + 0x60;
      break;
      
    default:
      Index = 0xFFFF;
      break;
  }
  return Index;
}
  



UINTN
GetBootXXXXIndex(  
  EFI_BOOT_MANAGER_LOAD_OPTION  *Option,
  UINTN                         OptionCount,
  UINT16                        XXXX
  )
{
  UINTN  Index;

  for(Index=0;Index<OptionCount;Index++){
    if(Option[Index].OptionNumber == XXXX){
      return Index;
    }
  }

  ASSERT(FALSE);
  return 0;
}


VOID
AdjustPXEBootOrderForSetup (
    EFI_BOOT_MANAGER_LOAD_OPTION  *NvBootOption,
    UINT16                        *PXENewOrder,
    UINTN                         NvBootOptionCount
  )
{
     UINTN                         Index,Index2,Indexout,PXEIndex,NewOrderIndex;
     SETUP_DATA                    SetupData;
     UINTN                         Size;
     UINT16                        PXEBoot[16][2];
     BOOLEAN                       OnboardPort0=TRUE;
     BOOLEAN                       OnboardPort=FALSE;
     UINTN                         MaxLanOrder = 0; 
     UINTN                         MinLanOrder = 0xFF;
     EFI_STATUS                    Status;
	 UINTN                         NumberofOnBoard;
	 UINTN                         OnBoardPXECount;
     Size = sizeof (SETUP_DATA);

     Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  );
     
     DEBUG((EFI_D_INFO, "SetupData.PXEBootPriority %d\n",SetupData.PXEBootPriority));

	 if(SetupData.NetBootIpVer == 0){
       NumberofOnBoard = 4;
	 }else{
       NumberofOnBoard = 2;
	 }

    for (Index = 0; Index < NvBootOptionCount; Index++) {
      DEBUG((EFI_D_INFO, "NV[%04X] %s\n", NvBootOption[Index].OptionNumber, NvBootOption[Index].Description));
    }
	
    if((SetupData.PXEBootPriority == 0) && !PcdGetBool(PcdFceLoadDefault) && NumberofOnBoard==2)
        return;
    for (Index = 0, PXEIndex = 0, NewOrderIndex = 0; Index < NvBootOptionCount; Index++) {
            if((UINTN)PXENewOrder[NewOrderIndex] == NvBootOption[Index].OptionNumber){
                
                if(NvBootOption[Index].GroupType == BM_MENU_TYPE_UEFI_PXE){
                PXEBoot[PXEIndex][0] = PXENewOrder[NewOrderIndex];
                if(MinLanOrder == 0xFF)
                    MinLanOrder = NewOrderIndex;
               
                if(MaxLanOrder < NewOrderIndex)
                    MaxLanOrder = NewOrderIndex;
                
                if(StrStr(NvBootOption[Index].Description, L"Onboard") != NULL){
                    PXEBoot[PXEIndex][1] = 1;
                    OnboardPort = TRUE;
                    }
                else
                    PXEBoot[PXEIndex][1] = 0;
                 PXEIndex++;          
                }
               NewOrderIndex++;
             }
        }

	DEBUG((EFI_D_INFO,"MaxLanOrder is %d\n  MinLanOrder is %d\n PXEIndex is %d\n",MaxLanOrder,MinLanOrder,PXEIndex));
      if(OnboardPort == FALSE)
            return;

      if(NumberofOnBoard == 4 && SetupData.PXEBootPriority == 0 && !CmosRead(0xD1)){
	  	DEBUG((EFI_D_INFO,"fist Open Ipv4 Ipv6\n"));
         for(Index = 0; Index < PXEIndex; Index++){
            for(Index2 = 0;  Index2 < 2; Index2++){
                DEBUG((EFI_D_INFO, "PXEBoot[%d][%d]:%d; ", Index,Index2,PXEBoot[Index][Index2]));        
            }
         }
		if(NumberofOnBoard == 4){
          if((PXEBoot[0][1] == 1) && (PXEBoot[1][1] == 1) && (PXEBoot[2][1] == 1) && (PXEBoot[3][1] == 1)){
             DEBUG((EFI_D_INFO, " onboard up!\n"));
             return;
           }
		}else{
         if((PXEBoot[0][1] == 1) && (PXEBoot[1][1] == 1)){
             DEBUG((EFI_D_INFO, " onboard up!\n"));
             return;
           }
	   }
		OnBoardPXECount = 1;
        for (Index = 0 ,Indexout = 0; Index < PXEIndex; Index++) {
            if(PXEBoot[Index][1] != 1){
                PXENewOrder[MinLanOrder+Indexout+NumberofOnBoard] = PXEBoot[Index][0];
                Indexout++;
                }
            else{
                if(OnboardPort0){
                    PXENewOrder[MinLanOrder] = PXEBoot[Index][0];
                    OnboardPort0 = FALSE;
                    }
                else{
                    PXENewOrder[MinLanOrder+OnBoardPXECount] = PXEBoot[Index][0];
					if(OnBoardPXECount==3){
                      break;
				    }
					OnBoardPXECount++;
                }
            }
        }
		CmosWrite(0xD1,1);
	  }
	  
      if(SetupData.PXEBootPriority == 1 || PcdGetBool(PcdFceLoadDefault)){
        for(Index = 0; Index < PXEIndex; Index++){
            for(Index2 = 0;  Index2 < 2; Index2++){
                DEBUG((EFI_D_INFO, "PXEBoot[%d][%d]:%d; ", Index,Index2,PXEBoot[Index][Index2]));        
            }
         }
		if(NumberofOnBoard == 4){
          if((PXEBoot[0][1] == 1) && (PXEBoot[1][1] == 1) && (PXEBoot[2][1] == 1) && (PXEBoot[3][1] == 1)){
             DEBUG((EFI_D_INFO, " onboard up!\n"));
             return;
           }
		}else{
         if((PXEBoot[0][1] == 1) && (PXEBoot[1][1] == 1)){
             DEBUG((EFI_D_INFO, " onboard up!\n"));
             return;
           }
	   }
		OnBoardPXECount = 1;
        for (Index = 0 ,Indexout = 0; Index < PXEIndex; Index++) {
            if(PXEBoot[Index][1] != 1){
                PXENewOrder[MinLanOrder+Indexout+NumberofOnBoard] = PXEBoot[Index][0];
                Indexout++;
                }
            else{
                if(OnboardPort0){
                    PXENewOrder[MinLanOrder] = PXEBoot[Index][0];
                    OnboardPort0 = FALSE;
                    }
                else{
                    PXENewOrder[MinLanOrder+OnBoardPXECount] = PXEBoot[Index][0];
					if(OnBoardPXECount==3 || NumberofOnBoard ==2){
                      break;
				    }
					OnBoardPXECount++;
                 }
            }

        }
        
    }
     if(SetupData.PXEBootPriority == 2 && !PcdGetBool(PcdFceLoadDefault)){

        for(Index = 0; Index < PXEIndex; Index++){
            for(Index2 = 0;  Index2 < 2; Index2++){
                DEBUG((EFI_D_INFO, "PXEBoot[%d][%d]:%d; \n", Index,Index2,PXEBoot[Index][Index2]));        
            }
         }
	   	if(NumberofOnBoard == 4){
		  if((PXEBoot[PXEIndex-1][1] == 1) && (PXEBoot[PXEIndex-2][1] == 1) && (PXEBoot[PXEIndex-3][1] == 1) && (PXEBoot[PXEIndex-4][1] == 1)){
		     DEBUG((EFI_D_INFO, " onboard bottom!\n"));
		     return;
		   }
		}else{
          if((PXEBoot[PXEIndex-1][1] == 1) && (PXEBoot[PXEIndex-2][1] == 1)){
             DEBUG((EFI_D_INFO, " onboard bottom!\n"));
             return;
           }
	   }
        OnBoardPXECount = 1;
		
        for (Index = PXEIndex-1, Indexout = 0 ; ; Index--)  {
            
            if(PXEBoot[Index][1] != 1){
                PXENewOrder[MaxLanOrder-Indexout-NumberofOnBoard] = PXEBoot[Index][0];  
                Indexout++;
                }
            else{
                if(OnboardPort0){
                    PXENewOrder[MaxLanOrder] = PXEBoot[Index][0];
                    OnboardPort0 = FALSE;
                    }
                else{
                    PXENewOrder[MaxLanOrder-OnBoardPXECount] = PXEBoot[Index][0];
					if(OnBoardPXECount==3 || NumberofOnBoard ==2){
                      break;
				    }
					OnBoardPXECount++;

                    }
                }
            if(Index == 0 )
                break;
        }

    }
     
  return;
}


VOID 
UpdateNewOrderData (
    UINT16 Data16,
    UINTN  Index,
    UINT16 *Array,
    UINTN  CurArrayCount
  )
{
  UINTN   i, j;

  DumpMem8(Array, CurArrayCount * sizeof(UINT16));

  i = CurArrayCount - 1;
  j = i - Index + 1;
  while(j--){
    Array[i+1] = Array[i];
    DEBUG((EFI_D_INFO, "%d -> %d\n", i, i+1));
    i--;
  }
  DEBUG((EFI_D_INFO, "i:%d\n", i));
  Array[Index] = Data16;

  DumpMem8(Array, (CurArrayCount+1) * sizeof(UINT16));  
}


VOID
ByoBmRefreshUefiBootOption2 (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_BOOT_MANAGER_LOAD_OPTION  *NvBootOption;
  UINTN                         NvBootOptionCount;
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption;
  UINTN                         BootOptionCount;
  UINTN                         Index,Index2;
  UINTN                         i, j;
  UINT8                         *UefiGroupOrder = NULL;
  UINT8                         GroupOrder[5];
  UINTN                         GroupOrderSize;
  UINTN                         GroupIndex;
  UINTN                         GroupStart;
  UINT16                        *NewOrder = NULL;
  UINT16                        NewIndex = 0;
  BOOLEAN                       Updated;
  BOOLEAN                       IsOsShort;
  CHAR16                        OptionName[sizeof("Boot####")];
  EFI_BDS_BOOT_MANAGER_PROTOCOL *BdsBootMgr;
  BOOLEAN                         Flag = TRUE;
  CHAR16                         *OsDesc=NULL;
  BMC_CONFIG_SETUP              *Value;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));



  UefiGroupOrder = EfiBootManagerGetVariableAndSize (
                       BYO_UEFI_BOOT_GROUP_VAR_NAME,
                       &gByoGlobalVariableGuid,
                       &GroupOrderSize
                       );
  
  if(UefiGroupOrder == NULL){
    GroupOrderSize = sizeof(gDefaultUefiBootGroupOrder);
    UefiGroupOrder = AllocateCopyPool(GroupOrderSize, gDefaultUefiBootGroupOrder);
    Status = gRT->SetVariable (
                    BYO_UEFI_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    GroupOrderSize,
                    UefiGroupOrder
                    );  
  DumpMem8(UefiGroupOrder,GroupOrderSize);  
    ASSERT(!EFI_ERROR(Status));    
  }
  DumpMem8(UefiGroupOrder,GroupOrderSize);  

  // Uefi
  if(PcdGet8(PcdBiosBootModeType) == BIOS_BOOT_LEGACY_OS){
    EfiBootManagerDeleteAllUefiBootOption();
    return;
  }

  if(PcdGet8(PcdBmcBiosConfigFlag) && (PcdGetBool(PcdIpmiSetBootOrder)==FALSE)){
    Value = GetBMCConfigValueHob();
    GroupOrder[0] = Value->FirstBoot;
    GroupOrder[1] = Value->SecondBoot;
    GroupOrder[2] = Value->ThirdBoot;
    GroupOrder[3]=  Value->FourthBoot;
    GroupOrder[4] = Value->FifthBoot;
	  
	for(Index=0;Index<4;Index++){
	 for(Index2=Index+1;Index2<5;Index2++){
		if(GroupOrder[Index] ==  GroupOrder[Index2]){
		 Flag = FALSE;
		}
      }
	}      
    for(Index=0;Index<5;Index++){
	  if(GroupOrder[Index] > 5 || GroupOrder[Index] == 1){
		Flag = FALSE;
      }
	}

    if(Flag){
	  for(Index=0;Index<5;Index++){
        UefiGroupOrder[Index] = GroupOrder[Index];
	  }
      Status = gRT->SetVariable (
                  BYO_UEFI_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  GroupOrderSize,
                  UefiGroupOrder
                  ); 
      DEBUG((EFI_D_INFO, "set variable\n"));
      DumpMem8(UefiGroupOrder,GroupOrderSize);  
	}
	
  }

  BootOption   = EfiBootManagerEnumerateBootOptions2(&BootOptionCount, UefiGroupOrder, GroupOrderSize);
  NvBootOption = EfiBootManagerGetLoadOptions(&NvBootOptionCount, LoadOptionTypeBoot);

  for (Index = 0; Index < BootOptionCount; Index++) {
    DEBUG((EFI_D_INFO, "[%04X] %s\n", BootOption[Index].OptionNumber, BootOption[Index].Description));
    BootOption[Index].ItemOrder = GetUefiHddTypeIndex(&BootOption[Index]);
  }  
  for (Index = 0; Index < NvBootOptionCount; Index++) {
    DEBUG((EFI_D_INFO, "NV[%04X] %s\n", NvBootOption[Index].OptionNumber, NvBootOption[Index].Description));
  }

// mask invalid
  for (Index = 0; Index < NvBootOptionCount; Index++) {
    if (NvBootOption[Index].Legacy) {
      continue;
    }

    DEBUG((EFI_D_INFO, "UEFIBOOT[%d] %s\n", Index, NvBootOption[Index].Description));
    ShowDevicePathDxe(gBS, NvBootOption[Index].FilePath);

    IsOsShort = FALSE;
    if (EfiBootManagerFindLoadOption (&NvBootOption[Index], BootOption, BootOptionCount) == (UINTN)-1) {
	  //OsDesc = (CHAR16 *) AllocateZeroPool (64 * sizeof (CHAR16));
      IsOsShort = CheckOsShortDp(&NvBootOption[Index], BootOption, BootOptionCount, NvBootOption, NvBootOptionCount,&OsDesc);
	  DEBUG((EFI_D_INFO,"OsDesc is %s\n",OsDesc));
      if(!IsOsShort){
        DEBUG((EFI_D_INFO, "delete %s\n", NvBootOption[Index].Description));
        NvBootOption[Index].Deleted = TRUE;
      }else{
        NvBootOption[Index].Description = OsDesc;
		Status = EfiBootManagerLoadOptionToVariable(&NvBootOption[Index]);
		DEBUG((EFI_D_INFO,"Change OsDesc is %r\n",Status));
	  }
    }

    if(IsOsShort){    
      NvBootOption[Index].ItemOrder = (UINT32)Index;
    } else {
      NvBootOption[Index].ItemOrder = GetUefiHddTypeIndex(&NvBootOption[Index]);
    }
  }
  
  if(NvBootOptionCount + BootOptionCount){
    NewOrder = AllocatePool((NvBootOptionCount + BootOptionCount) * sizeof(UINT16));
    ASSERT(NewOrder != NULL);
  }
  ZeroMem(NewOrder, (NvBootOptionCount + BootOptionCount) * sizeof(UINT16));

  NewIndex = 0; 
  for(GroupIndex=0;GroupIndex<GroupOrderSize;GroupIndex++){

    GroupStart = NewIndex;

// copy old    
    for (Index = 0; Index < NvBootOptionCount; Index++) {
      if(NvBootOption[Index].Legacy || NvBootOption[Index].Deleted || NvBootOption[Index].New) {
        continue;
      }
      if(NvBootOption[Index].GroupType != UefiGroupOrder[GroupIndex]){
        continue;
      }
      NewOrder[NewIndex] = (UINT16)NvBootOption[Index].OptionNumber;
      DEBUG((EFI_D_INFO, "UEFI [%d]:%X\n", NewIndex, NewOrder[NewIndex]));
      NewIndex++;
    }

// add new
    for (Index = 0; Index < BootOptionCount; Index++) {
      if(BootOption[Index].GroupType != UefiGroupOrder[GroupIndex]){
        continue;
      }

      if(BootOption[Index].Deleted){
        DEBUG((EFI_D_INFO, "ignore %s\n", BootOption[Index].Description));
        continue;
      }      
      if (EfiBootManagerFindLoadOption(&BootOption[Index], NvBootOption, NvBootOptionCount) == -1) {
        BmAddLoadOption(&BootOption[Index], &NvBootOption, &NvBootOptionCount);

        DEBUG((EFI_D_INFO, "GroupStart:%d, NewIndex:%d\n", GroupStart, NewIndex));
        if(UefiGroupOrder[GroupIndex] == BM_MENU_TYPE_UEFI_HDD){
          DEBUG((EFI_D_INFO, "NewItemOrder:%X\n", BootOption[Index].ItemOrder));

          Updated = FALSE;
          if(NewIndex - GroupStart){
            for(i=GroupStart;i<NewIndex;i++){
              j = GetBootXXXXIndex(NvBootOption, NvBootOptionCount, NewOrder[i]);
              DEBUG((EFI_D_INFO, "(L%d) %X %X %X %X\n", __LINE__, BootOption[Index].ItemOrder, NvBootOption[j].ItemOrder, NewOrder[i], j));
              if(BootOption[Index].ItemOrder <= NvBootOption[j].ItemOrder){
                UpdateNewOrderData(
                  (UINT16)BootOption[Index].OptionNumber,
                  i,
                  NewOrder,
                  NewIndex
                  );
                NewIndex++;
                Updated = TRUE;
                break;
              }
            }
          } 

          if(!Updated){
            NewOrder[NewIndex] = (UINT16)BootOption[Index].OptionNumber;
            DEBUG((EFI_D_INFO, "UEFI +[%d]:%X\n", NewIndex, NewOrder[NewIndex]));
            NewIndex++;
          }
          
        } else {
          NewOrder[NewIndex] = (UINT16)BootOption[Index].OptionNumber;
          DEBUG((EFI_D_INFO, "UEFI +[%d]:%X\n", NewIndex, NewOrder[NewIndex]));
          NewIndex++;
        } 
      }
    }
    
  }

  for (Index = 0; Index < NvBootOptionCount; Index++) {
    if(NvBootOption[Index].Legacy){
      NewOrder[NewIndex++] = (UINT16)NvBootOption[Index].OptionNumber;
    }
  }

  for (Index = 0; Index < NvBootOptionCount; Index++) {
    if(NvBootOption[Index].Deleted){
      UnicodeSPrint(OptionName, sizeof(OptionName), L"Boot%04X", NvBootOption[Index].OptionNumber);
      Status = gRT->SetVariable (
                      OptionName,
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      0,
                      NULL
                      );   
      DEBUG((EFI_D_INFO, "Del %X: %r\n", NvBootOption[Index].OptionNumber, Status));
    } else if(NvBootOption[Index].New){
      Status = EfiBootManagerLoadOptionToVariable(&NvBootOption[Index]);
      NvBootOption[Index].New = FALSE;
      DEBUG((EFI_D_INFO, "add %X: %r\n", NvBootOption[Index].OptionNumber, Status));
    }
  }

  AdjustPXEBootOrderForSetup(NvBootOption, NewOrder, NvBootOptionCount);
  DumpMem8(NewOrder, NewIndex * sizeof(UINT16));
  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  NewIndex * sizeof(UINT16),
                  NewOrder
                  );

  Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, &BdsBootMgr);
  if(!EFI_ERROR(Status)){
    BdsBootMgr->UefiOptions = BootOption;
    BdsBootMgr->UefiOptionsCount = BootOptionCount;
  }  

//ByoEfiBootManagerFreeLoadOptions (BootOption,   BootOptionCount);


  ByoEfiBootManagerFreeLoadOptions (NvBootOption, NvBootOptionCount);

  //FreePool(UefiGroupOrder);
  if(NewOrder != NULL){FreePool(NewOrder);}

// dump
#if !defined(MDEPKG_NDEBUG)  
  NvBootOption = EfiBootManagerGetLoadOptions(&NvBootOptionCount, LoadOptionTypeBoot);
  for (Index = 0; Index < NvBootOptionCount; Index++) {
    DEBUG((EFI_D_INFO, "[%04X] %s\n", NvBootOption[Index].OptionNumber, NvBootOption[Index].Description));
  }
  ByoEfiBootManagerFreeLoadOptions (NvBootOption, NvBootOptionCount);
#endif  
}
  
  
  
  
  
BM_MENU_TYPE GetLegacyBootGroupType(EFI_DEVICE_PATH_PROTOCOL *FilePath, UINT8 *OptionalData)
{
  UINT8   DevClass;
  UINT8   BbsType;

  DevClass = LegacyBootOptionalDataGetPciClass(OptionalData, NULL);
  BbsType  = (UINT8)((BBS_BBS_DEVICE_PATH*)FilePath)->DeviceType;
  return GetBbsGroupType(DevClass, BbsType); 
}

BM_MENU_TYPE GetEfiBootGroupType(EFI_DEVICE_PATH_PROTOCOL *FilePath)
{
  EFI_DEVICE_PATH_PROTOCOL      *Dp;
  EFI_HANDLE                    DevHandle;
  EFI_STATUS                    Status;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  UINT8                         ClassCode[3];
  BOOLEAN                       IsODD;
  EFI_BLOCK_IO_PROTOCOL         *BlockIo;
  BM_MENU_TYPE                  GroupType = BM_MENU_TYPE_MAX;
  
  
  IsODD = FALSE;
  Dp = FilePath;


  if (DevicePathType(Dp) == MEDIA_DEVICE_PATH && DevicePathSubType(Dp) == MEDIA_HARDDRIVE_DP) {
    return BM_MENU_TYPE_UEFI_HDD;
  }
  
  Status = gBS->LocateDevicePath(&gEfiBlockIoProtocolGuid, &Dp, &DevHandle);
  if(!EFI_ERROR(Status) && IsDevicePathEnd(Dp)){
    gBS->HandleProtocol(DevHandle, &gEfiBlockIoProtocolGuid, &BlockIo);
    if(BlockIo->Media->BlockSize == 2048){
      IsODD = TRUE;
    }
  }      

  Dp = FilePath;
  Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &DevHandle);
  if(EFI_ERROR(Status)){
    return BM_MENU_TYPE_UEFI_OTHERS;
  }
  gBS->HandleProtocol(DevHandle, &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
  PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 9, 3, ClassCode);

  switch (ClassCode[2]) {
    
    case 0x01:
      if(IsODD){
        GroupType = BM_MENU_TYPE_UEFI_ODD;
      } else {
        GroupType = BM_MENU_TYPE_UEFI_HDD;
      }
      break;
      
    case 0x0C:
      if(IsODD){
        GroupType = BM_MENU_TYPE_UEFI_USB_ODD;
      } else {
        GroupType = BM_MENU_TYPE_UEFI_USK_DISK;
      }
      break;

    case 0x02:
      GroupType = BM_MENU_TYPE_UEFI_PXE;
      break;
  }

  return GroupType;
}


  

