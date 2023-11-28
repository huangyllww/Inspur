
#include <PiDxe.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Guid/LegacyDevOrder.h>
#include <Guid/GlobalVariable.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ByoUefiBootManagerLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/PerformanceLib.h>
#include <Library/HobLib.h>
#include <IndustryStandard/Atapi.h>
#include <Guid/MemoryTypeInformation.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Library/ByoCommLib.h>
#include <Library/LegacyBootOptionalDataLib.h>
#include <Protocol/EnumLegacyBootOptionsHook.h>
#include <ByoBootGroup.h>
#include <SysMiscCfg2.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <protocol/DiskInfo.h>
#include <IndustryStandard/Atapi.h>
#include <Library/DevicePathLib.h>
#include <Library/PciLib.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/ByoDiskInfoProtocol.h>
#include <Protocol/UefiBbsRelationInf.h>
#include <Library/PlatformCommLib.h>
#include <SetupVariable.h>

VOID
PrintBbsTable (
  IN BBS_TABLE  *LocalBbsTable,
  IN UINT16     BbsCount
  );

BOOLEAN
ValidBbsEntry (
  IN BBS_TABLE   *BbsEntry
  );

UINT16
GetDevcieTypeInBBS (
  IN CONST BBS_TABLE *BbsEntry
  );

EFI_STATUS
CreateLegacyBootOption (
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption,
  IN BBS_TABLE                         *BbsEntry, 
  IN UefiBbsHddRL 					   *UefiSataHdd,
  IN UINT16                            BbsIndex
  );

UINT8 gStorageTypeArray[] = {MSG_SATA_DP, MSG_SATA_DP, MSG_ATAPI_DP, MSG_NVME_NAMESPACE_DP, 0xFF};

/*
Nvme:
class:    PCI_CLASS_MASS_STORAGE:1 
subclass:  PCI_CLASS_MASS_STORAGE_NVM:8 
  
HDD:
class:    PCI_CLASS_MASS_STORAGE:1
subclass:  PCI_CLASS_MASS_STORAGE_IDE:0x01
               PCI_CLASS_MASS_STORAGE_SATADPA:0x06
*/

UINT8
GetBbsStorageType(UINT8 BbsClass, UINT8 BbsSubClass)
{
  UINT8 Type;
  
  if(BbsClass == 0x01){
    switch(BbsSubClass){
      case 0x08:
        Type = MSG_NVME_NAMESPACE_DP; 
        break;
        
      case 0x01:
        Type = MSG_ATAPI_DP;
        break;
        
      case 0x06:
        Type = MSG_SATA_DP;
        break;
        
      default:
        Type = 0xFF;
        break;
    }
  }else{
    Type = 0xFF;
  }
  
  return Type;
}


EFI_BOOT_MANAGER_LOAD_OPTION *
EnumerateLegacyBootOptions2 (
  UINTN                         *BootOptionCount,
  UINT8                         *LegacyGroupOrder, 
  UINTN                         GroupOrderSize, 
  UefiBbsHddRL 					*UefiSataHdd,  
  UINTN							UefiSataHddIndex,
  UINT8 		  				*AsmM2Hdd
  )
{
  EFI_STATUS                    Status;
  UINT16                        HddCount;
  UINT16                        BbsCount;
  HDD_INFO                      *HddInfo;
  BBS_TABLE                     *BbsTable;
  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios;
  UINT16                        Index,ConIndex;
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions = NULL;
  UINT16                        DevCount = 0;
  UINT16                        *IndexBuffer = NULL;
  ENUM_LEGACY_BOOT_OPTIONS_HOOK_PARAMETER   Param;
  UINTN                                     TypeIndex;
  UINTN                                     GroupIndex;
  UINT8                                     GroupType;
  BOOLEAN                                   *RecArray = NULL;
  UINT32                      	VidDid;
  UINTN							AsmIndex = 0;
  UINTN 						NewAsmIndex = 0;
  BOOLEAN						StorageTable = FALSE;

  ASSERT (BootOptionCount != NULL);
  *BootOptionCount = 0;
  DEBUG((EFI_D_INFO, "%a(G:%X,L:%X)\n", __FUNCTION__, LegacyGroupOrder, GroupOrderSize));
  
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID**)&LegacyBios);
  if (EFI_ERROR (Status)) {
    goto ProcExit;
  }

  Status = LegacyBios->GetBbsInfo (
                         LegacyBios,
                         &HddCount,
                         &HddInfo,
                         &BbsCount,
                         &BbsTable
                         );
  if (EFI_ERROR (Status)) {
    goto ProcExit;
  }

  DEBUG((EFI_D_INFO, "BbsCount:%d\n", BbsCount));

  IndexBuffer = AllocateZeroPool(BbsCount * sizeof(UINT16));
  if(IndexBuffer == NULL){
    goto ProcExit;
  }

  RecArray = AllocateZeroPool(BbsCount * sizeof(BOOLEAN));
  if(RecArray == NULL){
    goto ProcExit;
  }

  DumpMem8(LegacyGroupOrder, GroupOrderSize);

  DevCount = 0;
  for(GroupIndex=0; GroupIndex<GroupOrderSize; GroupIndex++){
    if(LegacyGroupOrder[GroupIndex] == BM_MENU_TYPE_LEGACY_HDD){
      for (TypeIndex = 0; TypeIndex < ARRAY_SIZE(gStorageTypeArray); TypeIndex++){
        for (Index = 0; Index < BbsCount; Index++) {

          if(RecArray[Index]){
            continue;
          }
          
          if (!ValidBbsEntry(&BbsTable[Index])) {
            RecArray[Index] = 1;
            continue;
          }
          GroupType = GetBbsGroupType(BbsTable[Index].Class, (UINT8)GetDevcieTypeInBBS(&BbsTable[Index]));
          if(LegacyGroupOrder[GroupIndex] != GroupType){
            continue;
          }
          if(gStorageTypeArray[TypeIndex] != GetBbsStorageType(BbsTable[Index].Class, BbsTable[Index].SubClass)){
            continue;
          }		  
		  if(TypeIndex == 0){
			VidDid = PciRead32(PCI_LIB_ADDRESS(BbsTable[Index].Bus,BbsTable[Index].Device,BbsTable[Index].Function, 0x0));
			if(VidDid == 0x6221B21){			
				if( AsmM2Hdd[AsmIndex]!=1 ){	
					AsmIndex ++;
					continue;
					}
				else{
					AsmM2Hdd[AsmIndex]=0;
					AsmM2Hdd[NewAsmIndex]=1;
					NewAsmIndex++;
					AsmIndex ++;
					}
		  	}
          }
          IndexBuffer[DevCount++] = Index;
          RecArray[Index] = 1;
        }
      }
    } else {
      for (Index = 0; Index < BbsCount; Index++) {

        if(RecArray[Index]){
          continue;
        }
        
        if (!ValidBbsEntry(&BbsTable[Index])) {
          RecArray[Index] = 1;
          continue;
        }
        GroupType = GetBbsGroupType(BbsTable[Index].Class, (UINT8)GetDevcieTypeInBBS(&BbsTable[Index]));
        if(LegacyGroupOrder[GroupIndex] != GroupType){
          continue;
        }
        IndexBuffer[DevCount++] = Index;
        RecArray[Index] = 1;
      }
    }
  }

  DumpMem8(IndexBuffer, sizeof(UINT16)*DevCount);

  Param.Sign = ENUM_LEGACY_BOOT_OPTIONS_HOOK_SIGN;
  Param.BbsTable = BbsTable;
  Param.BbsCount = BbsCount;
  Param.BbsIndexList  = IndexBuffer;
  Param.BbsIndexCount = &DevCount;
  InvokeHookProtocol2(gBS, &gEnumLegacyBootOptionsHookGuid, &Param);

  DEBUG((EFI_D_INFO, "find legacy dev count:%X\n", DevCount));

  if(DevCount != 0){
    BootOptions = AllocatePool(sizeof(EFI_BOOT_MANAGER_LOAD_OPTION) * DevCount);
    ASSERT (BootOptions != NULL);
    for(Index=0;Index<DevCount;Index++){
      DEBUG((EFI_D_INFO, "I:%X\n", IndexBuffer[Index]));
	  for(ConIndex=0; ConIndex<UefiSataHddIndex; ConIndex++){
	  	if( IndexBuffer[Index] == UefiSataHdd[ConIndex].BbsIndex ){ 			
			DEBUG((EFI_D_INFO, "bbs:%x con:%x\n", UefiSataHdd[ConIndex].BbsIndex, ConIndex));
      		Status = CreateLegacyBootOption(&BootOptions[Index], &BbsTable[IndexBuffer[Index]], &UefiSataHdd[ConIndex], IndexBuffer[Index]);
      		ASSERT_EFI_ERROR (Status);
			StorageTable = TRUE;			
			break;
				}	
	  		}
	  	if(StorageTable == TRUE){
			StorageTable = FALSE;
			continue;
	  		}
			DEBUG((EFI_D_INFO, "nondisk I:%X\n", IndexBuffer[Index]));
      		Status = CreateLegacyBootOption(&BootOptions[Index], &BbsTable[IndexBuffer[Index]], NULL, IndexBuffer[Index]);
	  	}
    }

ProcExit:
  if(IndexBuffer != NULL){FreePool(IndexBuffer);}
  if(RecArray != NULL){FreePool(RecArray);}
  *BootOptionCount = DevCount;
  DEBUG((EFI_D_INFO, "Exit P:%X,L:%d\n", BootOptions, DevCount));
  return BootOptions;
}



#if BC_TENCENT
UINT8 gDefaultLegacyBootGroupOrder[] = {
  BM_MENU_TYPE_LEGACY_HDD, 
  BM_MENU_TYPE_LEGACY_PXE,
  BM_MENU_TYPE_LEGACY_USB_ODD,
  BM_MENU_TYPE_LEGACY_USK_DISK
  };
#else
UINT8 gDefaultLegacyBootGroupOrder[] = {
  BM_MENU_TYPE_LEGACY_HDD, 
  //BM_MENU_TYPE_LEGACY_ODD, 
  BM_MENU_TYPE_LEGACY_USK_DISK,
  BM_MENU_TYPE_LEGACY_PXE,
  BM_MENU_TYPE_LEGACY_USB_ODD
  };
  #endif

UINT16
FuzzyMatch (
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption,
  BBS_TABLE                     *BbsTable,
  UINT16                         BbsCount,
  BOOLEAN                        *BbsIndexUsed
  );

INTN
LegacyBootManagerFindBootOption (
  IN CONST EFI_BOOT_MANAGER_LOAD_OPTION *Key,
  IN CONST EFI_BOOT_MANAGER_LOAD_OPTION *Array,
  IN UINTN                              Count
  );


UINT16
GetLegacyHddTypeIndex (
  EFI_BOOT_MANAGER_LOAD_OPTION *Option,
  BOOLEAN AsmediaMT
  )
{
  UINT8  DevType;
  UINT16 Index;
  UINT16 *Desc;
  UINT8  PciClass;
  UINT8  PciSubClass;

  	 
//DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));
  
  PciClass = LegacyBootOptionalDataGetPciClass(Option->OptionalData, &PciSubClass);
  DevType = GetBbsStorageType(PciClass, PciSubClass);
  
  switch(DevType){
      
    case MSG_SATA_DP:
    case MSG_ATAPI_DP:
      Desc = Option->Description;
	  if(AsmediaMT == 1){	  	
	  	Index = 0x0 + (Desc[6] - '0');   
      }else if(StrnCmp(Desc, L"SATA0", 0x05) == 0){   //Each SATA controller has 8 port.
        Index = 0xA + (Desc[6] - '0');         //But we give it 10 Index for compatible issue. cause sometime port number start from 1.
      }else if(StrnCmp(Desc, L"SATA1", 0x05) == 0){
        Index = 0x14 + (Desc[6] - '0');
      }else if(StrnCmp(Desc, L"SATA2", 0x05) == 0){
        Index = 0x1E + (Desc[6] - '0');
      }else if(StrnCmp(Desc, L"SATA3", 0x05) == 0){
        Index = 0x28 + (Desc[6] - '0');
      }else if(StrnCmp(Desc, L"SATA4", 0x05) == 0){
        Index = 0x32 + (Desc[6] - '0');        
      } else {
        Index = 0x3F;
      }
      break;

	case MSG_NVME_NAMESPACE_DP:
      Index = 0x50;
      break;
      
    default:
      Index = 0xFF;
      break;
  }
  return Index;
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

UINTN
GetBootXXXXIndex(  
  EFI_BOOT_MANAGER_LOAD_OPTION  *Option,
  UINTN                         OptionCount,
  UINT16                        XXXX
  );  

VOID 
UpdateNewOrderData (
    UINT16 Data16,
    UINTN  Index,
    UINT16 *Array,
    UINTN  CurArrayCount
  );

UEFI_BBS_RELATION_INFO_PROTOCOL *HBtable;

EFI_STATUS
UefiBbsRelationInfoInstall (
	IN UefiBbsHddRL *UefiBbsHddTable,
	IN UINTN LocalUefiSataHddIndex
  )
{
  EFI_STATUS     Status;
  EFI_HANDLE     Handle = NULL;
  
  UEFI_BBS_RELATION_INFO_PROTOCOL	*ptUefiBbsInfo;
  
 // CopyMem(HBtable->UefiBbsHddTable, UefiBbsHddTable, sizeof(UefiBbsHddRL));
  HBtable->UefiBbsHddTable = UefiBbsHddTable;
  HBtable->UefiSataHddCount = LocalUefiSataHddIndex;

  DEBUG((EFI_D_INFO, "HBtable->UefiSataHddCount %x\n", HBtable->UefiSataHddCount));

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gUefiBbsRelationProtocolGuid, 
                  EFI_NATIVE_INTERFACE,
                  HBtable
                  );
  
  ASSERT_EFI_ERROR(Status);
  
  Status = gBS->LocateProtocol(&gUefiBbsRelationProtocolGuid, NULL, &ptUefiBbsInfo);
	
  DEBUG((EFI_D_INFO, "jean LocateProtocol gUefiBbsRelationProtocolGuid %r\n", Status));
  DEBUG((EFI_D_INFO, "jean ptUefiBbsInfo->UefiSataHddCount %x\n", ptUefiBbsInfo->UefiSataHddCount));

  

  return Status;
}


VOID
PrintUefiBbsStorageTable(
	IN UefiBbsHddRL *LocalUefiSataHdd, 
	IN UINTN LocalUefiSataHddIndex
	)
{
	
	UINTN							  Index;	
	UINT16							  *SataString;

    DEBUG ((DEBUG_INFO, "\n"));
  	DEBUG((EFI_D_INFO, "UefiSataHddIndex %x\n", LocalUefiSataHddIndex));
  	DEBUG ((DEBUG_INFO, " NO bus/dev/func bbsno portno SerialNo     devicepath\n"));
  	DEBUG ((DEBUG_INFO, "=====================================================\n"));

	for (Index = 0; Index < LocalUefiSataHddIndex; Index++) {
  			SataString = DevicePathToString(LocalUefiSataHdd[Index].HddDevicePath);
    	DEBUG (
      		(DEBUG_INFO,
      		" %02x: %02x/%02x/%02x  %02x  %02x  %a  %s\n",
      		LocalUefiSataHdd[Index].UefiHddIndex,
      		LocalUefiSataHdd[Index].Bus,
      		LocalUefiSataHdd[Index].Device,
      		LocalUefiSataHdd[Index].Function,
      		LocalUefiSataHdd[Index].BbsIndex,
      		LocalUefiSataHdd[Index].HddPortIndex,
      		LocalUefiSataHdd[Index].SerialNo, 		     		
			SataString
      	));
  	}

}
VOID
UpdateBbsSataIndxe(
	IN OUT BBS_TABLE   *LocalBbsTable,
	IN UINT16	  		BbsCount,
	IN UefiBbsHddRL 	*BbsRelation
	)
{
	UINT16	Index;
	UINTN	BbsHddIndex = 0;
	UINT32	AsmBbsVidDid;
	UINT32	AsmUefiVidDid;	
	UINT32	NvmeBbsVidDid;
	UINT32	NvmeUefiVidDid;
	UINT32	NonAsmBbsVidDid;
	UINT32	NonAsmUefiVidDid;

	for ( Index = 0; Index < BbsCount; Index++ ) {
		if (( LocalBbsTable[Index].Class == 1) && ( LocalBbsTable[Index].SubClass == 6)){
			for( ;BbsHddIndex < BbsCount; BbsHddIndex++ ){				
					AsmBbsVidDid = PciRead32(PCI_LIB_ADDRESS(LocalBbsTable[Index].Bus,LocalBbsTable[Index].Device,LocalBbsTable[Index].Function, 0x0));					
					AsmUefiVidDid = PciRead32(PCI_LIB_ADDRESS(BbsRelation[BbsHddIndex].Bus,BbsRelation[BbsHddIndex].Device,BbsRelation[BbsHddIndex].Function, 0x0));					
				//	DEBUG((EFI_D_INFO, "AsmBbsVidDid %x AsmUefiVidDid %x\n", AsmBbsVidDid,AsmUefiVidDid));
					if((AsmBbsVidDid == 0x6221B21) && (AsmUefiVidDid == 0x6221B21)){						
						//	DEBUG((EFI_D_INFO, "Asm Index %x BbsHddIndex %x\n", Index,BbsHddIndex));
							LocalBbsTable[Index].Bus= (UINT32)BbsRelation[BbsHddIndex].Bus;						
							LocalBbsTable[Index].IBV1 = (UINT32)BbsRelation[BbsHddIndex].HddPortIndex;
							LocalBbsTable[Index].IBV2 = (UINT32)BbsRelation[BbsHddIndex].SataHostIndex;
							BbsRelation[BbsHddIndex].BbsIndex = Index;
							BbsHddIndex++;
							break;
						}
					
}
				}
			}
	
	DEBUG((EFI_D_INFO, "Nvme controller\n"));
	BbsHddIndex = 0;
	for ( Index = 0; Index < BbsCount; Index++ ) {
		if (( LocalBbsTable[Index].Class == 1) && (LocalBbsTable[Index].SubClass == 8)){
			for( ;BbsHddIndex < BbsCount; BbsHddIndex++ ){				
					NvmeBbsVidDid = PciRead32(PCI_LIB_ADDRESS(LocalBbsTable[Index].Bus,LocalBbsTable[Index].Device,LocalBbsTable[Index].Function, 0x0));					
					NvmeUefiVidDid = PciRead32(PCI_LIB_ADDRESS(BbsRelation[BbsHddIndex].Bus,BbsRelation[BbsHddIndex].Device,BbsRelation[BbsHddIndex].Function, 0x0));					
					DEBUG((EFI_D_INFO, "NvmeBbsVidDid %x NvmeUefiVidDid %x\n", NvmeBbsVidDid,NvmeUefiVidDid));
					if(((NvmeBbsVidDid == 0x0A548086) && (NvmeUefiVidDid == 0x0A548086)) && (LocalBbsTable[Index].Bus == (UINT32)BbsRelation[BbsHddIndex].Bus)){						
							DEBUG((EFI_D_INFO, "Nvme Index %x BbsHddIndex %x\n", Index,BbsHddIndex));
							LocalBbsTable[Index].IBV1 = (UINT32)BbsRelation[BbsHddIndex].HddPortIndex;
							LocalBbsTable[Index].IBV2 = (UINT32)BbsRelation[BbsHddIndex].SataHostIndex;
							BbsRelation[BbsHddIndex].BbsIndex = Index;
							BbsHddIndex++;
							break;
						}
					
}
				}
			}

	DEBUG((EFI_D_INFO, "Other hdd controller\n"));
	BbsHddIndex = 0;
	for ( Index = 0; Index < BbsCount; Index++ ) {
		if (( LocalBbsTable[Index].Class == 1) && ( LocalBbsTable[Index].SubClass == 6)){			
			NonAsmBbsVidDid = PciRead32(PCI_LIB_ADDRESS(LocalBbsTable[Index].Bus,LocalBbsTable[Index].Device,LocalBbsTable[Index].Function, 0x0));
			if((NonAsmBbsVidDid == 0x6221B21) || (NonAsmBbsVidDid == 0x0A548086))
				continue;
			for( ;BbsHddIndex < BbsCount; BbsHddIndex++ ){				
				NonAsmUefiVidDid = PciRead32(PCI_LIB_ADDRESS(BbsRelation[BbsHddIndex].Bus,BbsRelation[BbsHddIndex].Device,BbsRelation[BbsHddIndex].Function, 0x0));					
				DEBUG((EFI_D_INFO, "NonAsmBbsVidDid %x NonAsmUefiVidDid %x\n", NonAsmBbsVidDid,NonAsmUefiVidDid));
				if (( NonAsmBbsVidDid == NonAsmUefiVidDid)) {					
					DEBUG((EFI_D_INFO, "Index %x BbsHddIndex %x\n", Index,BbsHddIndex));
					LocalBbsTable[Index].IBV1 = (UINT32)BbsRelation[BbsHddIndex].HddPortIndex;
					LocalBbsTable[Index].IBV2 = (UINT32)BbsRelation[BbsHddIndex].SataHostIndex;
					BbsRelation[BbsHddIndex].BbsIndex = Index;
					BbsHddIndex++;
					break;
					}
				}
		}
	  }
	}
UINTN
GetUefiSataIndxe(
	OUT UefiBbsHddRL *HddRelation,	
	OUT UINT8		 *AsmediaUefiHdd
	)
{
	
	UINTN							  HandleCount;
	EFI_HANDLE						  *HandleBuffer;
	EFI_HANDLE						  DeviceHandle;
	UINTN						 	  AtaIndex, PhysicPortIndex;
	EFI_STATUS						  Status;
	UINTN						 	  PlatSataHostIndex = 0;
	UINTN							  Index;
	//UINT16 							  *SataString;
	//UINT16 							  *AsmString;
	EFI_DEVICE_PATH_PROTOCOL	  	  *DevicePath;		
	EFI_DEVICE_PATH_PROTOCOL	  	  *OrignDevicePath;	 	
	EFI_DEVICE_PATH_PROTOCOL	  	  *PciDevicePath;	 
	BOOLEAN							  HddConStorage = FALSE;	
	BOOLEAN							  NvmeConStorage = FALSE;	
	BOOLEAN							  MTConStorage = FALSE;	
	UINTN						  	  Seg,Bus,Dev,Func;
    EFI_PCI_IO_PROTOCOL               *PciIo;
	UINTN 							  HddIndex = 0;
	UINTN 							  AsmIndex = 0;
	UINT32							  VidDid;
	PCI_DEVICE_PATH	  				  *M2DevicePath;	
	BYO_DISKINFO_PROTOCOL 			  *ByoDiskInfo;
	CHAR8							  Sn[30];
	UINTN							  Size;

    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiDiskInfoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
 // if(EFI_ERROR(Status)){
//    return Status;
 // }  
	
  	DEBUG((EFI_D_INFO,"GetDiskInfo %r HandleCount %x\n",Status,HandleCount));
	
	for (Index = 0; Index < HandleCount; Index++) {

	Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &DevicePath
                    );
    ASSERT_EFI_ERROR(Status);
	
	OrignDevicePath = PciDevicePath = DevicePath;
	//SataString = DevicePathToString(DevicePath);
	//DEBUG((EFI_D_INFO, "\nDevicePath String %s\n", SataString));
		
    Status = gBS->LocateDevicePath (&gEfiPciIoProtocolGuid, &DevicePath, &DeviceHandle);

    Status = gBS->HandleProtocol (
                        DeviceHandle,
                        &gEfiPciIoProtocolGuid,
                        &PciIo
                        );
    if (!EFI_ERROR (Status)) {
      		DEBUG((EFI_D_ERROR," EfiPciIoProtocolGuid get bus dev"));
      		PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);
      		DEBUG((EFI_D_INFO, "(%X,%X,%X) %X\n", Bus, Dev, Func, HandleBuffer[Index]));
        }

	PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, 1, &VidDid);
	DEBUG((EFI_D_INFO, "DiskInfo  VidDid %X\n", VidDid));
	if(VidDid == 0x6221B21){
			AsmediaUefiHdd[AsmIndex] = 0;
						
			//AsmString = DevicePathToString(PciDevicePath);
			//DEBUG((EFI_D_INFO, "PciDevicePath String %s\n", AsmString));
			
			while (!IsDevicePathEndType (PciDevicePath)) {
					if ((PciDevicePath->Type == HARDWARE_DEVICE_PATH) && (PciDevicePath->SubType == HW_PCI_DP)) { 				 
					   M2DevicePath = (PCI_DEVICE_PATH *) PciDevicePath;
					   DEBUG((EFI_D_ERROR, "PciDp->Device %x, PciDp->Function %x\n",M2DevicePath->Device, M2DevicePath->Function));					  
					  if ((M2DevicePath->Device == 1)&&(M2DevicePath->Function == 7)){  
							  DEBUG((EFI_D_ERROR, "GetUefiHddTypeIndex	  M2.....\n"));   							  
							  AsmediaUefiHdd[AsmIndex] = 1;							  
							  DEBUG((EFI_D_ERROR, "UefiHdd[AsmIndex] = %x\n",AsmediaUefiHdd[AsmIndex]));
							  MTConStorage = TRUE;
						  }
					  break;
					}				 
				  PciDevicePath = NextDevicePathNode (PciDevicePath);
				}  
				AsmIndex++;
			}
	
    while (!IsDevicePathEndType (DevicePath)) {				
			DEBUG((EFI_D_ERROR," DevicePath->Type %x DevicePath->SubType %x\n",DevicePath->Type,DevicePath->SubType));
            if ((DevicePath->Type == MESSAGING_DEVICE_PATH) && ((DevicePath->SubType == MSG_SATA_DP)||(DevicePath->SubType == MSG_NVME_NAMESPACE_DP))) {                    
                     HddConStorage = TRUE;
					if(DevicePath->SubType == MSG_NVME_NAMESPACE_DP)
					 NvmeConStorage = TRUE;	
                  	}			
			  DevicePath = NextDevicePathNode (DevicePath);
            }
	//DEBUG((EFI_D_INFO,"AcpiStorage %x\n",AcpiStorage));

	if ( HddConStorage == FALSE ){
			continue;
		}
	if (( NvmeConStorage == TRUE ) || ( MTConStorage == TRUE ) || (HddConStorage == TRUE)) {
		
		DEBUG((EFI_D_INFO,"This are Nvme or M2 or HDD devices!\n"));
		Status = gBS->HandleProtocol(
				HandleBuffer[Index],
				&gByoDiskInfoProtocolGuid,
				(VOID**)&ByoDiskInfo
				);
		if(!EFI_ERROR(Status)){
			
			Size = sizeof(Sn);
			Status = ByoDiskInfo->GetSn(ByoDiskInfo, Sn, &Size);

			CopyMem(HddRelation[HddIndex].SerialNo, Sn, 30);
		DEBUG((EFI_D_INFO,"IdentifyData %r Sn %a\n",Status,Sn));	
		}
	}
	if(!LibGetSataPortInfo(gBS, HandleBuffer[Index], &PlatSataHostIndex, &AtaIndex, &PhysicPortIndex)){		
			DEBUG((EFI_D_INFO,"LibGetSataPortInfo flase!\n"));
	  		PhysicPortIndex = 0;	  		
	  }
	
  	//DEBUG((EFI_D_INFO,"PlatSataHostIndex %x\n",PlatSataHostIndex));	
		
	if(PlatSataHostIndex & BIT15){	
			HddRelation[HddIndex].HddPortIndex = 0;
	  		DEBUG((EFI_D_INFO,"Only one Port.\n"));
		} else {
			HddRelation[HddIndex].HddPortIndex = PhysicPortIndex;
	  		DEBUG((EFI_D_INFO,"This is %x PhysicPortIndex.\n",PhysicPortIndex));
		}
		
		HddRelation[HddIndex].UefiHddIndex = HddIndex;		
		HddRelation[HddIndex].SataHostIndex = PlatSataHostIndex;		
		HddRelation[HddIndex].Bus = Bus;
		HddRelation[HddIndex].Device = Dev;
		HddRelation[HddIndex].Function = Func;
		HddRelation[HddIndex].HddDevicePath = OrignDevicePath;
		HddIndex++;
	}
	
//	for(Index = 0; Index < AsmIndex; Index++)	
//		DEBUG((EFI_D_ERROR, "UefiHdd[%x] = %x\n",Index,AsmediaUefiHdd[Index]));
	
 	return HddIndex;
	
}


VOID
AdjustLegacyPXEBootOrderForSetup (
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
     Size = sizeof (SETUP_DATA);

     Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  );
     
     DEBUG((EFI_D_INFO, "SetupData.PXEBootPriority %d\n",SetupData.PXEBootPriority));

  
    if((SetupData.PXEBootPriority == 0) && !PcdGetBool(PcdFceLoadDefault))
        return;
        
    for (Index = 0, PXEIndex = 0, NewOrderIndex = 0; Index < NvBootOptionCount; Index++) {

            if((UINTN)PXENewOrder[NewOrderIndex] == NvBootOption[Index].OptionNumber){
                
                if(NvBootOption[Index].GroupType == BM_MENU_TYPE_LEGACY_PXE){
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

      if(OnboardPort == FALSE)
            return;
        
      if(SetupData.PXEBootPriority == 1 || PcdGetBool(PcdFceLoadDefault)){
        for(Index = 0; Index < PXEIndex; Index++){
            for(Index2 = 0;  Index2 < 2; Index2++){
                DEBUG((EFI_D_INFO, "PXEBoot[%d][%d]:%d; ", Index,Index2,PXEBoot[Index][Index2]));        
            }
         }

         if((PXEBoot[0][1] == 1) && (PXEBoot[1][1] == 1)){
             DEBUG((EFI_D_INFO, " onboard up!\n"));
             return;
           }
                
        for (Index = 0 ,Indexout = 0; Index < PXEIndex; Index++) {
            if(PXEBoot[Index][1] != 1){
                PXENewOrder[MinLanOrder+Indexout+2] = PXEBoot[Index][0];
                Indexout++;
                }
            else{
                if(OnboardPort0){
                    PXENewOrder[MinLanOrder] = PXEBoot[Index][0];
                    OnboardPort0 = FALSE;
                    }
                else{
                    PXENewOrder[MinLanOrder+1] = PXEBoot[Index][0];
                    break;
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
              
        if((PXEBoot[PXEIndex-1][1] == 1) && (PXEBoot[PXEIndex-2][1] == 1)){
             DEBUG((EFI_D_INFO, " onboard bottom!\n"));
             return;
           }
        for (Index = PXEIndex-1, Indexout = 0 ; ; Index--)  {
            
            if(PXEBoot[Index][1] != 1){
                PXENewOrder[MaxLanOrder-Indexout-2] = PXEBoot[Index][0];  
                Indexout++;
                }
            else{
                if(OnboardPort0){
                    PXENewOrder[MaxLanOrder] = PXEBoot[Index][0];
                    OnboardPort0 = FALSE;
                    }
                else{
                    PXENewOrder[MaxLanOrder-1] = PXEBoot[Index][0];
                    break;
                    }
                }
            if(Index == 0 )
                break;
        }

    }
     
  return;
}


VOID
EFIAPI
LegacyBmRefresh2 (
    VOID
  )
{
  EFI_STATUS                        Status;
  EFI_LEGACY_BIOS_PROTOCOL          *LegacyBios;
  EFI_BOOT_MANAGER_LOAD_OPTION      *BootOption;
  UINTN                             BootOptionCount;
  EFI_BOOT_MANAGER_LOAD_OPTION      *NvBootOption;
  UINTN                             NvBootOptionCount;  
  UINT16                            HddCount;
  UINT16                            BbsCount;
  HDD_INFO                          *HddInfo;
  BBS_TABLE                         *BbsTable;
  UINT16                            BbsIndex;
  UINT16                            OldBbsIndex;
  BOOLEAN                           *BbsIndexUsed = NULL;
  UINTN                             Index,Index2;
  UINT8                             *LegacyGroupOrder = NULL;
  UINT8                             GroupOrder[5];
  UINTN                             GroupOrderSize;
  UINTN                             GroupIndex;
  UINTN                             GroupStart;
  UINT16                            *NewOrder = NULL;
  UINT16                            NewIndex;
  BOOLEAN                           Updated;
  UINTN                             i, j;
  CHAR16                            OptionName[sizeof("Boot####")];  
  EFI_BDS_BOOT_MANAGER_PROTOCOL     *BdsBootMgr;
  UefiBbsHddRL 						*UefiSataHdd = NULL;
  UINTN								UefiSataHddIndex = 0;
  UINT16                            BbsHddIndex;
  UINT8 		  					AsmedUefiHdd[4] = {0};
  BOOLEAN		  					AsmediaM2 = FALSE;
  UINTN 							Bus,Dev,Func;
  UINT32                      		VidDid;
  BMC_CONFIG_SETUP              *Value;

  
  BOOLEAN                         Flag=TRUE;
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  
  UefiSataHdd = AllocatePool(sizeof(UefiBbsHddRL) * 20);

  LegacyGroupOrder = EfiBootManagerGetVariableAndSize (
                       BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                       &gByoGlobalVariableGuid,
                       &GroupOrderSize
                       );
  if(LegacyGroupOrder == NULL){
    GroupOrderSize = sizeof(gDefaultLegacyBootGroupOrder);
    LegacyGroupOrder = AllocateCopyPool(GroupOrderSize, gDefaultLegacyBootGroupOrder);
    Status = gRT->SetVariable (
                    BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    GroupOrderSize,
                    LegacyGroupOrder
                    );  
  DumpMem8(LegacyGroupOrder,GroupOrderSize);  
    ASSERT(!EFI_ERROR(Status));
  }
  
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID**)&LegacyBios);
  if (EFI_ERROR (Status) || PcdGet8(PcdBiosBootModeType) == BIOS_BOOT_UEFI_OS) {
    BootOption = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);
    for (Index = 0; Index < BootOptionCount; Index++) {
      if ((DevicePathType (BootOption[Index].FilePath) == BBS_DEVICE_PATH) &&
          (DevicePathSubType (BootOption[Index].FilePath) == BBS_BBS_DP)) {
        EfiBootManagerDeleteLoadOptionVariable (BootOption[Index].OptionNumber, BootOption[Index].OptionType);
      }
    }
    return;
  }
  PERF_START (NULL, "LegacyBootOptionEnum", "BDS", 0);


  Status = LegacyBios->GetBbsInfo (
                         LegacyBios,
                         &HddCount,
                         &HddInfo,
                         &BbsCount,
                         &BbsTable
                         );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "(%d)GetBbsInfo:%r\n", __LINE__, Status));
    return;
  }

  PrintBbsTable(BbsTable, BbsCount);
  UefiSataHddIndex = GetUefiSataIndxe(UefiSataHdd,AsmedUefiHdd);
  UpdateBbsSataIndxe(BbsTable,BbsCount,UefiSataHdd);
  UefiBbsRelationInfoInstall (UefiSataHdd, UefiSataHddIndex);
  PrintUefiBbsStorageTable(UefiSataHdd, UefiSataHddIndex);
  PrintBbsTable(BbsTable, BbsCount);
  

  if(PcdGet8(PcdBmcBiosConfigFlag) && (PcdGetBool(PcdIpmiSetBootOrder)==FALSE)){
    Value = GetBMCConfigValueHob();
    GroupOrder[0] = Value->FirstBoot;
    GroupOrder[1] = Value->SecondBoot;
    GroupOrder[2] = Value->ThirdBoot;
    GroupOrder[3]=  Value->FourthBoot;
    GroupOrder[4] = Value->FifthBoot;
	  
	for(Index=0;Index<4;Index++){
	  for(Index2=Index+1;Index2<5;Index2++){
	    if(GroupOrder[Index] == GroupOrder[Index2]){
	   	  Flag = FALSE;
		}
      }
	}	  
    for(Index=0;Index<6;Index++){
	  if(GroupOrder[Index] > 5 || GroupOrder[Index] == 1){
		Flag = FALSE;
      }
	}

    if(Flag){
      for(Index=0,Index2=0;Index<5;Index++){
        if(GroupOrder[Index]!=5){
          LegacyGroupOrder[Index2++] = GroupOrder[Index] + 0x80;
        }
    }
    Status = gRT->SetVariable (
                  BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  GroupOrderSize,
                  LegacyGroupOrder
                  ); 
    }
  }


  NvBootOption = EfiBootManagerGetLoadOptions(&NvBootOptionCount, LoadOptionTypeBoot);
  BootOption   = EnumerateLegacyBootOptions2(&BootOptionCount, LegacyGroupOrder, GroupOrderSize, UefiSataHdd, UefiSataHddIndex, AsmedUefiHdd);

  for (Index = 0; Index < BootOptionCount; Index++) {
    DEBUG((EFI_D_INFO, "[%04X] %s\n", BootOption[Index].OptionNumber, BootOption[Index].Description));
	BbsHddIndex = LegacyBootOptionalDataGetBbsIndex(BootOption[Index].OptionalData);	
	Bus = BbsTable[BbsHddIndex].Bus;
	Dev = BbsTable[BbsHddIndex].Device; 
	Func = BbsTable[BbsHddIndex].Function;
	VidDid = PciRead32(PCI_LIB_ADDRESS(Bus,Dev,Func, 0x0));
	if(VidDid == 0x6221B21){
		if(AsmedUefiHdd[Index] == 1)
			AsmediaM2 = TRUE;
		}	
    BootOption[Index].ItemOrder = GetLegacyHddTypeIndex(&BootOption[Index],AsmediaM2);
	AsmediaM2 = FALSE;
  }  
  for (Index = 0; Index < NvBootOptionCount; Index++) {
    DEBUG((EFI_D_INFO, "NV[%04X] %s\n", NvBootOption[Index].OptionNumber, NvBootOption[Index].Description));
  }

  BbsIndexUsed = AllocateZeroPool(BbsCount * sizeof(BOOLEAN));
  ASSERT (BbsIndexUsed != NULL);

// mark invalid
  for (Index = 0; Index < NvBootOptionCount; Index++) {
    if(NvBootOption[Index].Ignore || !NvBootOption[Index].Legacy) {
      DEBUG((EFI_D_INFO, "!NV[%d]:%X\n", Index, NvBootOption[Index].OptionNumber));
      continue;
    }

    DEBUG((EFI_D_INFO, "NV[%d]:%X\n", Index, NvBootOption[Index].OptionNumber)); 

    BbsIndex    = FuzzyMatch(&NvBootOption[Index], BbsTable, BbsCount, BbsIndexUsed);
    OldBbsIndex = LegacyBootOptionalDataGetBbsIndex(NvBootOption[Index].OptionalData);
    if (BbsIndex == BbsCount) {
      DEBUG((EFI_D_INFO, "[LegacyBds] Delete BOOT%04X: %s\n", NvBootOption[Index].OptionNumber, NvBootOption[Index].Description));
      NvBootOption[Index].Deleted = TRUE;
    } else {
      DEBUG((EFI_D_INFO, "[BBS] 0x%X -> 0x%X\n", OldBbsIndex, BbsIndex));
      if (OldBbsIndex != BbsIndex) {
        NvBootOption[Index].Deleted = TRUE;
      }
    }

    NvBootOption[Index].ItemOrder = GetLegacyHddTypeIndex(&NvBootOption[Index],0);
  }

  if(NvBootOptionCount + BootOptionCount){
    NewOrder = AllocatePool((NvBootOptionCount + BootOptionCount) * sizeof(UINT16));
    ASSERT(NewOrder != NULL);
  }
  DEBUG((EFI_D_INFO, "NewOrder P:%X\n", NewOrder));

// copy UEFI
  NewIndex = 0;
  for (Index = 0; Index < NvBootOptionCount; Index++) {
    if(!NvBootOption[Index].Legacy){
      DEBUG((EFI_D_INFO, "+UEFI [%d]:%d\n", NewIndex, NvBootOption[Index].OptionNumber));
      NewOrder[NewIndex++] = (UINT16)NvBootOption[Index].OptionNumber;
    }
  }

// copy LEGACY
  for(GroupIndex=0;GroupIndex<GroupOrderSize;GroupIndex++){

    GroupStart = NewIndex;

// copy old
    for (Index = 0; Index < NvBootOptionCount; Index++) {
      if(!NvBootOption[Index].Legacy || NvBootOption[Index].Deleted) {
        continue;
      }
      if(NvBootOption[Index].GroupType != LegacyGroupOrder[GroupIndex]){
        continue;
      }
      DEBUG((EFI_D_INFO, "+OLD LEGACY [%d]:%d\n", NewIndex, NvBootOption[Index].OptionNumber));
      NewOrder[NewIndex++] = (UINT16)NvBootOption[Index].OptionNumber;
    }

// add new
    for (Index = 0; Index < BootOptionCount; Index++) {

      if(BootOption[Index].GroupType != LegacyGroupOrder[GroupIndex]){
        continue;
      }
      
      if (LegacyBootManagerFindBootOption(&BootOption[Index], NvBootOption, NvBootOptionCount) == -1) {
        BmAddLoadOption(&BootOption[Index], &NvBootOption, &NvBootOptionCount);

        DEBUG((EFI_D_INFO, "GroupStart:%d, NewIndex:%d\n", GroupStart, NewIndex));

        if(LegacyGroupOrder[GroupIndex] == BM_MENU_TYPE_LEGACY_HDD){
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
            DEBUG((EFI_D_INFO, "LEGACY +[%d]:%X\n", NewIndex, NewOrder[NewIndex]));
            NewIndex++;
          }
          
        } else {
          NewOrder[NewIndex] = (UINT16)BootOption[Index].OptionNumber;
          DEBUG((EFI_D_INFO, "LEGACY +[%d]:%X\n", NewIndex, NewOrder[NewIndex]));
          NewIndex++;
        } 
      }
    }
  }

  DEBUG((EFI_D_INFO, "NewIndex:%d\n", NewIndex));

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

  AdjustLegacyPXEBootOrderForSetup(NvBootOption, NewOrder, NvBootOptionCount);
  DumpMem8(NewOrder, NewIndex * sizeof(UINT16));
  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  NewIndex * sizeof(UINT16),
                  NewOrder
                  );

  ByoEfiBootManagerFreeLoadOptions (NvBootOption, NvBootOptionCount);
//ByoEfiBootManagerFreeLoadOptions (BootOption, BootOptionCount);

  Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, &BdsBootMgr);
  if(!EFI_ERROR(Status)){
    BdsBootMgr->LegacyOptions = BootOption;
    BdsBootMgr->LegacyOptionsCount = BootOptionCount;
  } 

  //FreePool(LegacyGroupOrder);
  FreePool(BbsIndexUsed);
  if(NewOrder != NULL){FreePool(NewOrder);}

  PERF_END(NULL, "LegacyBootOptionEnum", "BDS", 0);
  DEBUG((EFI_D_INFO, "%a() Exit\n", __FUNCTION__));
}



