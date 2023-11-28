
#include "InternalBdsLib.h"
#include <Guid/BlockIoVendor.h> 
#include <Protocol/AtaPassThru.h>
#include <Protocol/ScsiPassThruExt.h>
#include <Protocol/ScsiPassThru.h>
#include <Library/ByoCommLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/UefiLib.h>
#include <Protocol/OpRomPnpName.h>
#include <Library/SystemPasswordLib.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <SysMiscCfg.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <SysMiscCfg2.h>
#include <Protocol/EfiPostLoginProtocol.h>
#include <Protocol/LegacyRegion2.h>
#include <Protocol/AtaPassThru.h>
#include <Protocol/ByoBmBootHookProtocol.h>
#include <IndustryStandard/Scsi.h>
#include <Protocol/smbios.h>
#include <Protocol/ByoDiskInfoProtocol.h>
#include "../PlatformPkg/Include/SetupVariable.h"
#include <Library/IpmiBaseLib.h>
#include <Protocol/IpmiInterfaceProtocol.h>
#include <Library/AmdCbsVariable.H>


CONST UINT16 USB_LANG_ID   = 0x0409; // English
CHAR16       mUefiPrefix[] = L"UEFI ";

#define      MAX_SAS_CTRL_NUM     30
EFI_HANDLE   mSasHandleList[MAX_SAS_CTRL_NUM] = {NULL};

EFI_BOOT_MANAGER_REFRESH_LEGACY_BOOT_OPTION  mEfiBootManagerRefreshLegacyBootOption  = NULL;
EFI_BOOT_MANAGER_LEGACY_BOOT                 mEfiBootManagerLegacyBoot               = NULL;

extern EFI_GUID gEfiNetworkInterfaceIdentifierProtocolGuid_31;


///
/// This GUID is used for an EFI Variable that stores the front device pathes
/// for a partial device path that starts with the HD node.
///
EFI_GUID mHdBootVariablePrivateGuid = { 0xfab7e9e1, 0x39dd, 0x4f2b, { 0x84, 0x08, 0xe2, 0x0e, 0x90, 0x6c, 0xb6, 0xde } };


STATIC CHAR16 *gLoaderPathNameList[] = {
  L"\\EFI\\nfs\\shim.efi",
  L"\\EFI\\centos\\shimx64.efi",
 // L"\\EFI\\uos\\shimx64.efi",
  L"\\EFI\\Ubuntu\\grubx64.efi",
  L"\\EFI\\neokylin\\shimx64.efi",
  L"\\EFI\\redhat\\shimx64.efi",
  L"\\EFI\\kylin\\shimx64.efi",
  L"\\EFI\\linx\\grubx64.efi",
  L"\\EFI\\tencent\\grubx64.efi", 
  L"\\EFI\\tencent\\grub.efi",  
  L"\\EFI\\redhat\\grub.efi",
  EFI_REMOVABLE_MEDIA_FILE_NAME,
};

typedef struct {
  CHAR16            *FilePathString;
  CHAR16            *Description;
}BDS_GENERAL_UEFI_BOOT_OS;

BDS_GENERAL_UEFI_BOOT_OS mBdsGeneralUefiBootOs[] = {
  L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi",      L"Windows Boot Manager",
  L"\\EFI\\centos\\shimx64-centos.efi",         L"CentOS Linux",
  L"\\EFI\\centos\\shim.efi",                   L"CentOS Linux",
  L"\\EFI\\redhat\\grub.efi",                   L"CentOS Linux",//cent os 6.5
  L"\\EFI\\uos\\shimx64.efi",                   L"UOS", 
  L"\\EFI\\ubuntu\\grubx64.efi",                L"Ubuntu Linux",
  L"\\EFI\\redhat\\shimx64-redhat.efi",         L"Red Hat Linux",
  L"\\EFI\\redhat\\shim.efi",                   L"Red Hat Linux",
  L"\\EFI\\tencent\\grubx64.efi",               L"Tencent OS tlinux",
  L"\\EFI\\tencent\\grub.efi",                  L"Tencent OS tlinux",
  L"\\EFI\\redhat\\shimx64.efi",                L"Red Hat Enterprise Linux",
  L"\\EFI\\neokylin\\shimx64.efi",              L"Kylin Linux Advanced Server",
  L"\\EFI\\kylin\\shimx64.efi",                 L"Kylin Linux Advanced Server",
  EFI_REMOVABLE_MEDIA_FILE_NAME,                NULL,  
  
//  L"\\EFI\\ubuntu\\grub.efi",                 L"Ubuntu",
//  L"\\EFI\\SuSE\\elilo.efi",                  L"SuSE Linux",
//  L"\\EFI\\android\\grub.efi",                L"Android",
  };

typedef enum {
  UnknowBoot,
  SataHddBoot,
  CdromBoot,
  UsbDiskBoot,
  LanBoot,
  NvmeBoot,
  TypeMaxBoot
} BOOT_DEVICE_TYPE;

/**
  Get the image file buffer data and buffer size by its device path. 

  @param FilePath  On input, a pointer to an allocated buffer containing the device
                   path of the file.
                   On output the pointer could be NULL when the function fails to
                   load the boot option, or could point to an allocated buffer containing
                   the device path of the file.
                   It could be updated by either short-form device path expanding,
                   or default boot file path appending.
                   Caller is responsible to free it when it's non-NULL.
  @param FileSize  A pointer to the size of the file buffer.

  @retval NULL   File is NULL, or FileSize is NULL. Or, the file can't be found.
  @retval other  The file buffer. The caller is responsible to free the memory.
**/
VOID *
LoadEfiBootOption (
  IN OUT EFI_DEVICE_PATH_PROTOCOL **FilePath,
  OUT    UINTN                    *FileSize
  );

UINT8
GetEfiNetWorkType (
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  OUT CHAR16                     **TypeStr
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *Node;
  EFI_DEVICE_PATH_PROTOCOL      *NextNode;
  UINT8                         Type;
  UINT8                         IpType;

  ASSERT (DevicePath != NULL);

  IpType = 0xFF;
  
  for (Node = DevicePath; !IsDevicePathEndType(Node); Node = NextDevicePathNode(Node)) {
    if(DevicePathType(Node) != MESSAGING_DEVICE_PATH){
      continue;
    }  

    Type = DevicePathSubType(Node);
    if(Type == MSG_IPv4_DP){
      IpType = MSG_IPv4_DP;
    } else if(Type == MSG_IPv6_DP){
      IpType = MSG_IPv6_DP;
    }
    
    NextNode = NextDevicePathNode(Node);    
    if (!IsDevicePathEndType(NextNode)) {
      continue;
    }

    Type = DevicePathSubType(Node);
    switch(Type){
     case MSG_IPv4_DP:
     case MSG_IPv6_DP:
       *TypeStr = L"PXE";
       return IpType;

     case MSG_URI_DP:
       *TypeStr = L"HTTP";
       return IpType;          
    }
  }

  *TypeStr = L"";
  return 0xFF;
}







typedef struct {
  UINT32                    DidVid;
  CHAR16                    *Str;
  EFI_PCI_IO_PROTOCOL       *PciIo[8];
  UINTN                     Index[8];
  UINTN                     Count;
} NetWorkTypeStruct;

NetWorkTypeStruct mNetWorkTypeList[] = {
  { 0x15028086, L"Intel 82579LM Eth",                {NULL,}, {0,}, 0 },
  { 0x15218086, L"Intel I350 Eth",                   {NULL,}, {0,}, 0 },
  { 0x15338086, L"Intel I210 Eth",                   {NULL,}, {0,}, 0 },
  { 0x10D38086, L"Intel 82574L Eth",                 {NULL,}, {0,}, 0 },
  { 0x15638086, L"Intel X550 Eth",                   {NULL,}, {0,}, 0 },
  { 0x816810EC, L"Realtek 8111G Eth",                {NULL,}, {0,}, 0 },
  { 0x16D814E4, L"Broadcom BCM57416 Eth",            {NULL,}, {0,}, 0 },  
  { 0x168E14E4, L"Broadcom BCM57810 Eth",            {NULL,}, {0,}, 0 },
  { 0x16C914E4, L"Broadcom BCM57302 Eth",            {NULL,}, {0,}, 0 },
  { 0x101515B3, L"Mellanox ConnectX-4 LX Eth",       {NULL,}, {0,}, 0 }, 
  { 0x101715B3, L"Mellanox ConnectX-5 LX Eth",       {NULL,}, {0,}, 0 },
  { 0x101915B3, L"Mellanox ConnectX-5 100G Eth",     {NULL,}, {0,}, 0 },
  { 0x01058088, L"WangXun(R)",       				 {NULL,}, {0,}, 0 },
  { 0x01018088, L"WangXun(R)",       				 {NULL,}, {0,}, 0 },
  { 0x01098088, L"WangXun(R)",       				 {NULL,}, {0,}, 0 },
  { 0x20018088, L"WangXun(R)",       				 {NULL,}, {0,}, 0 },
  { 0x10018088, L"WangXun(R) OCP",       		     {NULL,}, {0,}, 0 },
  { 0x15728086, L"Intel(R) X710 10GbE",       	     {NULL,}, {0,}, 0 },
  { 0x15848086, L"Intel(R) XL710 Q1 40GbE",       	 {NULL,}, {0,}, 0 },
  { 0x15838086, L"Intel(R) XL710 Q2 40GbE",       	 {NULL,}, {0,}, 0 },
  { 0x10FB8086, L"Intel(R) 82599 10GbE",       	     {NULL,}, {0,}, 0 },
//{ 0x91801D17, L"ZX-200 GNIC",   {NULL,}, {0,}, 0 },
};

NetWorkTypeStruct gUnknownNetcardType = {0xFFFFFFFF, L"Netcard", {NULL,}, {0,}, 0};


UINTN GetDevInstance(EFI_PCI_IO_PROTOCOL *PciIo, NetWorkTypeStruct *p)
{
  UINTN              Index;
  UINTN               Temp;


  if(PciIo == NULL){
    return 0;
  }

  for(Index=0;Index<p->Count;Index++){
    if(p->PciIo[Index] == PciIo){
      return p->Index[Index];
    }
  }

  p->PciIo[p->Count] = PciIo;
  p->Index[p->Count] = p->Count;
  Temp = p->Count;
  if(p->Count < ARRAY_SIZE(p->PciIo)){    
    p->Count++;
  }
  
  return Temp;

}

UINT8 GetPciPortNum (
  IN  EFI_PCI_IO_PROTOCOL   *PciIo,
  IN  UINT32                VIDDID
  )
{
  EFI_STATUS                Status;
  UINTN                     PciSegment;
  UINTN                     PciBus;
  UINTN                     PciDevice;
  UINTN                     PciFunction;
  UINT8                     PortNum = 0xFF;

  Status = PciIo->GetLocation(PciIo, &PciSegment, &PciBus, &PciDevice, &PciFunction);
  if(!EFI_ERROR (Status)) {
  //	if(VIDDID == 0x01098088){
  //    PortNum = (UINT8)PciFunction==0?1:0;// XCL only for on board net card change slot number
  //    return PortNum;
//	}
    PortNum = (UINT8)PciFunction ;
  }

  return PortNum;
}


CHAR16 * 
LibDriverGetDriverDeviceName (
  IN   EFI_BOOT_SERVICES   *BS,
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_STATUS                    Status;
  CHAR16                        *DevName = NULL;
  CHAR16                        *Str = NULL;
  EFI_HANDLE*                   Cn2Buffer = NULL;
  UINTN                         Cn2s;
  EFI_COMPONENT_NAME2_PROTOCOL* Cn2;
  UINTN                         Index1;
  EFI_HANDLE                    ControllerHandle,DeviceHandle = NULL;
  //EFI_DEVICE_PATH_PROTOCOL      *ControllerDP;
  //EFI_DEVICE_PATH_PROTOCOL      *DeviceDp;
  EFI_DEVICE_PATH_PROTOCOL      *TempDp;
  EFI_PCI_IO_PROTOCOL         *PciIo = NULL;
  UINT8                        ClassCode[3];


  if(!PcdGetBool(PcdGetBootDescriptionFromDrivers)){
    return NULL;
  }

  
  TempDp = DevicePath;
  Status = gBS->LocateDevicePath (
                  &gEfiPciIoProtocolGuid,
                  &TempDp,
                  &ControllerHandle
                  );
  if(EFI_ERROR(Status)){
     goto ProcExit;
  }
  //ControllerDP = DevicePathFromHandle(ControllerHandle);
  //ShowDevicePathDxe(gBS, ControllerDP);

  TempDp = DevicePath;
  Status = gBS->HandleProtocol(
                  ControllerHandle,
                  &gEfiPciIoProtocolGuid,
                  &PciIo
                  );
  PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CLASSCODE_OFFSET, 3, ClassCode);
  if(ClassCode[2] == PCI_CLASS_NETWORK){
    Status = gBS->LocateDevicePath (
                    &gEfiNetworkInterfaceIdentifierProtocolGuid_31,
                    &TempDp,
                    &DeviceHandle
                    );
    if(EFI_ERROR(Status)){
       goto ProcExit;
    }  
  }else if (ClassCode[2] == PCI_CLASS_MASS_STORAGE && (ClassCode[1] == PCI_CLASS_MASS_STORAGE_RAID || ClassCode[1] == PCI_CLASS_MASS_STORAGE_SAS)){
        Status = gBS->LocateDevicePath (
                    &gEfiBlockIoProtocolGuid,
                    &TempDp,
                    &DeviceHandle
                    );
    if(EFI_ERROR(Status)){
       goto ProcExit;
    } 
  }
 // DeviceDp = DevicePathFromHandle(DeviceHandle);
 // ShowDevicePathDxe(gBS, DeviceDp);

  Status = BS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiComponentName2ProtocolGuid,
                  NULL,
                  &Cn2s,
                  &Cn2Buffer
                  );
  if (EFI_ERROR(Status) || Cn2s == 0) {
    goto ProcExit;
  }

  for (Index1 = 0; Index1 < Cn2s; Index1++) {
    Status = BS->HandleProtocol(
                 Cn2Buffer[Index1],
                 &gEfiComponentName2ProtocolGuid,
                 (VOID**)&Cn2
                 );
    
      Status = Cn2->GetControllerName(
                      Cn2,
                      ControllerHandle,
                      DeviceHandle,
                      Cn2->SupportedLanguages,
                      &DevName
                      );
      if (!EFI_ERROR(Status) && DevName != NULL) {
        Str = AllocatePool(StrSize(DevName));
        StrCpy(Str, DevName);
        TrimStr16(Str); 
        //DEBUG((EFI_D_ERROR,"Controller name :%s Size= %x\n",Str,StrSize(DevName)));
        goto ProcExit;
      }
  }

ProcExit:
  if(Cn2Buffer != NULL){BS->FreePool(Cn2Buffer);}
  
  return Str;
}



CHAR16 *
GetNetworkMacByDp (
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  BOOLEAN                    Found =FALSE;
  EFI_DEVICE_PATH_PROTOCOL   *TmpDevicePath;
  MAC_ADDR_DEVICE_PATH       *MacDevPath;
  CHAR16                     *MacAddr;   
  TmpDevicePath = DevicePath;
  //
  // find the mac path node
  //
  while (!IsDevicePathEnd (TmpDevicePath)) {
    if ((DevicePathType (TmpDevicePath) == MESSAGING_DEVICE_PATH) &&
        (DevicePathSubType (TmpDevicePath) == MSG_MAC_ADDR_DP)
        ) { 
      MacDevPath = (MAC_ADDR_DEVICE_PATH *)TmpDevicePath;
      Found = TRUE;
      break;
    }
    TmpDevicePath = NextDevicePathNode (TmpDevicePath);
    ShowDevicePathDxe(gBS, TmpDevicePath);
  }
  if(Found){
    //DEBUG((EFI_D_INFO,"Mac[0] = %x \nMac[1] = %x \nMac[2] = %x\nMac[3] = %x\nMac[4] = %x\nMac[5] = %x\n",\
    //                  MacDevPath->MacAddress.Addr[0], MacDevPath->MacAddress.Addr[1], MacDevPath->MacAddress.Addr[2],\
    //                   MacDevPath->MacAddress.Addr[3], MacDevPath->MacAddress.Addr[4], MacDevPath->MacAddress.Addr[5]));
    
    
    MacAddr = AllocateZeroPool(StrSize(L"(00-00-00-00-00-00)"));
    UnicodeSPrint(MacAddr, StrSize(L"(00-00-00-00-00-00)"),  L"(%02x-%02x-%02x-%02x-%02x-%02x)", \
                  MacDevPath->MacAddress.Addr[0],MacDevPath->MacAddress.Addr[1], MacDevPath->MacAddress.Addr[2],\
                  MacDevPath->MacAddress.Addr[3], MacDevPath->MacAddress.Addr[4], MacDevPath->MacAddress.Addr[5]);
                  
    return MacAddr;
  }

  return NULL;
}

VOID
CheckPxeNicVar (
  VOID 
  ) 
{
  EFI_STATUS    Status;
  SETUP_DATA    SetupData;
  UINTN    Size, HandleCount, Index;
  EFI_HANDLE    *HandleBuffer;
  EFI_PCI_IO_PROTOCOL    *PciIo;
  UINT8    PciMacAddr[6];
  UINT32    MacCrc;
  BOOLEAN    beFound;
  
  ZeroMem (&SetupData, sizeof (SETUP_DATA));
  Size = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  );  
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), GetVariable :%r, SelectPxeNic :%d.\n", Status, SetupData.SelectPxeNic));
  if (EFI_ERROR (Status)) {
    PcdSet32(PcdPxeNicVidDid, 0); 
    PcdSet16(PcdPxeNicMacCrc, 0); 
    return;
  }  
  
  if (SetupData.SelectPxeNic == 0 ) {
    if (SetupData.PxeNicVid != 0 ||
  	SetupData.PxeNicDid != 0 ||
  	SetupData.PxeNicMacCrc != 0) {
      goto DEFAULT_PXE_NIC;	
    } else {
      return;
    }  	
  } else {
    if (SetupData.PxeNicVid == 0 ||
  	SetupData.PxeNicDid == 0 ||
  	SetupData.PxeNicMacCrc == 0) {
      goto DEFAULT_PXE_NIC;	
    } 
  }

  //
  // Check VID\DID\MacCrc.
  //
  HandleCount = 0;
  HandleBuffer = NULL;
  gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  beFound = FALSE;
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if (!EFI_ERROR (Status)) {
      MacCrc = 0;
      ZeroMem(PciMacAddr, sizeof(PciMacAddr));
      Status = GetPciNicMac (PciIo, PciMacAddr);
      if (!EFI_ERROR(Status)) {
        MacCrc = CalculateCrc32(PciMacAddr, 6);	
        if (SetupData.PxeNicMacCrc == (MacCrc & 0xFFFF)) {
          beFound = TRUE;
        }		
      }
    }
  }
  if (HandleBuffer) {
    FreePool (HandleBuffer);
  }
  
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), beFound :%d.\n", beFound));  
  if (beFound)  {
    return;
  }
  
DEFAULT_PXE_NIC:
	
  PcdSet32(PcdPxeNicVidDid, 0); 
  PcdSet16(PcdPxeNicMacCrc, 0); 

  SetupData.SelectPxeNic = 0;
  SetupData.PxeNicVid = 0;
  SetupData.PxeNicDid = 0;
  SetupData.PxeNicMacCrc = 0;
	
  Size = sizeof (SETUP_DATA);	
  Status = gRT->SetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  PLATFORM_SETUP_VARIABLE_FLAG,
                  Size,
                  &SetupData
                  );  
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), Set default Pxe Nic Variable :%r.\n", Status));
  
  return;
}



BOOLEAN 
BePxeNicenable (
    EFI_PCI_IO_PROTOCOL    *NicPciIo 
  ) 
{
  EFI_STATUS    Status;
  UINTN    HandleCount, Index;
  EFI_HANDLE    *HandleBuffer;
  EFI_PCI_IO_PROTOCOL    *PciIo;
  UINT8    PciMacAddr[6];
  UINT32    MacCrc;
  BOOLEAN    beMatched;

  UINTN    NicSegment, NicBus, NicDevice, NicFunction;
  UINTN    Segment, Bus, Device, Function;
  UINT32    DidVid;

  if (PcdGet32(PcdPxeNicVidDid) == 0 || PcdGet16(PcdPxeNicMacCrc) == 0){
    return TRUE;
  }

  if (NULL == NicPciIo) {
    return FALSE;
  }

  Status = NicPciIo->GetLocation (NicPciIo, &NicSegment, &NicBus, &NicDevice, &NicFunction);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  //DEBUG((EFI_D_ERROR, __FUNCTION__"(), NicPciIo NicBus:%d, NicDevice :%d.\n\n", NicBus, NicDevice));

  beMatched = FALSE;
  HandleCount = 0;
  HandleBuffer = NULL;
  gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if (!EFI_ERROR (Status)) {
      DidVid = 0;
      Status = PciIo->Pci.Read (
                   PciIo,
                   EfiPciIoWidthUint32,
                   0,
                   1,
                   &DidVid
                   );

      Bus = 0;
      Device = 0;
      Function = 0;
      Status = PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
      //DEBUG((EFI_D_ERROR, __FUNCTION__"(), DidVid :0x%x, PcdPxeNicVidDid :0x%x.\n", DidVid, PcdGet32(PcdPxeNicVidDid)));
      if ((Bus == NicBus) && (Device == NicDevice) && (DidVid ==PcdGet32(PcdPxeNicVidDid))) {
         //DEBUG((EFI_D_ERROR, __FUNCTION__"(), PciIo BDF :%d-%d-%d.\n", Bus, Device, Function));
         MacCrc = 0;
         ZeroMem(PciMacAddr, sizeof(PciMacAddr));
         Status = GetPciNicMac (PciIo, PciMacAddr);
         if (!EFI_ERROR(Status)) {
           MacCrc = CalculateCrc32(PciMacAddr, 6);	
         }
         if (MacCrc) {
           //DEBUG((EFI_D_ERROR, __FUNCTION__"(), MacCrc :0x%x, PcdPxeNicMacCrc :0x%x.\n", MacCrc, PcdGet16(PcdPxeNicMacCrc)));
           if (PcdGet32(PcdPxeNicVidDid) && (PcdGet32(PcdPxeNicVidDid) ==DidVid)){
             if (PcdGet16(PcdPxeNicMacCrc) ==  (MacCrc&0xFFFF)){
               beMatched = TRUE;
               break;
             }         	
           }	
         }
      }	
    }
  }
	
  if (HandleBuffer) {
    FreePool (HandleBuffer);
  }
  
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), beMatched :%d.\n", beMatched));  

  return beMatched;  
}


CHAR16 *
GetEfiNetWorkString (
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  OUT BOOLEAN                     *IsPxeBootCard
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                DriverHandle;
  EFI_PCI_IO_PROTOCOL       *PciIo = NULL;
  CHAR16                    *DevName  = NULL;
  CHAR16                    *Description  = NULL;
  UINT8                     SlotNum;
  VOID                      *Dp;
  UINT32                    DidVid;
  UINTN                     Index;  
  UINT8                     NetStrType;
  CHAR16                    *MacAddr;
  
  DEBUG((EFI_D_ERROR,"%a()\n\n",__FUNCTION__));

  NetStrType = PcdGet8(PcdNetworkBootDescriptionType);
  Dp = (VOID*)(UINTN)DevicePath; 
  SlotNum = GetPciSlotNum(gBS, Dp);
  MacAddr = GetNetworkMacByDp(Dp);

  Status = gBS->LocateDevicePath (
                  &gEfiPciIoProtocolGuid,
                  &(EFI_DEVICE_PATH_PROTOCOL*)Dp,
                  &DriverHandle
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol(
                    DriverHandle,
                    &gEfiPciIoProtocolGuid,
                    &PciIo
                    );
    if (!EFI_ERROR (Status)) {
      DevName = LibDriverGetDriverDeviceName(gBS,DevicePath);
      if(DevName != NULL){
        Description=AllocatePool(StrSize(DevName)+40);
        if(NetStrType == 1){
          UnicodeSPrint(Description, StrSize(DevName)+40, L"%s%s", DevName,MacAddr);
          FreePool(MacAddr);
        }else{
          UnicodeSPrint(Description, StrSize(DevName)+40, L"Slot %d: Port %d - %s", SlotNum, GetPciPortNum(PciIo,0xFF), DevName);
        }
        FreePool(DevName);
        return Description;
      }else{
         PciIo->Pci.Read (
                   PciIo,
                   EfiPciIoWidthUint32,
                   0,
                   1,
                   &DidVid
                   );
	    *IsPxeBootCard = BePxeNicenable (PciIo);
		 
          for (Index = 0; Index < ARRAY_SIZE(mNetWorkTypeList); Index ++) {
            if (DidVid == mNetWorkTypeList[Index].DidVid){
              Description=AllocatePool(StrSize(mNetWorkTypeList[Index].Str) + 40);
              if(NetStrType == 1){
                UnicodeSPrint(Description, StrSize(mNetWorkTypeList[Index].Str) + 40, L"%s%s", mNetWorkTypeList[Index].Str,MacAddr);
                FreePool(MacAddr);
              }else{
			  	if(DidVid == 0x01018088 || DidVid == 0x01098088){
                  UnicodeSPrint(Description, StrSize(mNetWorkTypeList[Index].Str) + 40, L"Onboard: Port %d - %s", GetPciPortNum(PciIo,DidVid), mNetWorkTypeList[Index].Str);
				}else{
                  UnicodeSPrint(Description, StrSize(mNetWorkTypeList[Index].Str) + 40, L"Slot %d: Port %d - %s", SlotNum, GetPciPortNum(PciIo,DidVid), mNetWorkTypeList[Index].Str);
				}
              }
              return Description;
            }
          }
      }
    }
  }

  Description=AllocatePool(StrSize(gUnknownNetcardType.Str) + 40);
  if(NetStrType == 1){
    UnicodeSPrint(Description, sizeof(Description), L"%s%s", gUnknownNetcardType.Str,MacAddr);
  }else{
    UnicodeSPrint(Description, sizeof(Description), L"Slot %d: Port %d - %s", SlotNum, GetPciPortNum(PciIo,0xFF), gUnknownNetcardType.Str);
  }

  return Description;
}


VOID
EFIAPI
ByoEfiBootManagerRegisterLegacyBootSupport (
  EFI_BOOT_MANAGER_REFRESH_LEGACY_BOOT_OPTION   RefreshLegacyBootOption,
  EFI_BOOT_MANAGER_LEGACY_BOOT                  LegacyBoot
  )
{
  mEfiBootManagerRefreshLegacyBootOption  = RefreshLegacyBootOption;
  mEfiBootManagerLegacyBoot               = LegacyBoot;
}

VOID
FreeAndSet (
  VOID   **Orig,
  VOID   *New
  )
{
  FreePool (*Orig);
  *Orig = New;
}

/**
  Internal function to check if the input boot option is a valid EFI NV Boot####.

  @param OptionToCheck  Boot option to be checked.

  @retval TRUE      This boot option matches a valid EFI NV Boot####.
  @retval FALSE     If not.

**/
BOOLEAN
BootOptionInVariable (
  IN  EFI_BOOT_MANAGER_LOAD_OPTION             *OptionToCheck
  )
{
  EFI_STATUS                   Status;
  EFI_BOOT_MANAGER_LOAD_OPTION BootOption;
  BOOLEAN                      Valid;
  CHAR16                       OptionName[sizeof ("Boot####")];

  UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", OptionToCheck->OptionNumber);
  Status = EfiBootManagerVariableToLoadOption (OptionName, &BootOption);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  //
  // If the Boot Option Number and Device Path matches, OptionToCheck matches a
  // valid EFI NV Boot####.
  //
  Valid = FALSE;
  if ((OptionToCheck->OptionNumber == BootOption.OptionNumber) &&
      (CompareMem (OptionToCheck->FilePath, BootOption.FilePath, GetDevicePathSize (OptionToCheck->FilePath)) == 0))
      {
    Valid = TRUE;
  }

  EfiBootManagerFreeLoadOption (&BootOption);
  return Valid;
}

/**
  According to a file guild, check a Fv file device path is valid. If it is invalid,
  try to return the valid device path.
  FV address maybe changes for memory layout adjust from time to time, use this function
  could promise the Fv file device path is right.

  @param  DevicePath   The Fv file device path to be fixed up.

  @retval DevicePath   The DevicePath ponits to the valid FV file.
  @retval !NULL        The fixed device path.
  @retval NULL         Failed to fix up the DevicePath.
**/
VOID
FixupMemmapFvFilePath (
  IN OUT EFI_DEVICE_PATH_PROTOCOL      **DevicePath
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  EFI_DEVICE_PATH_PROTOCOL      *Node;
  EFI_HANDLE                    FvHandle;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  EFI_LOADED_IMAGE_PROTOCOL     *LoadedImage;
  UINTN                         Size;
  EFI_FV_FILETYPE               Type;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        AuthenticationStatus;
  UINTN                         FvHandleCount;
  EFI_HANDLE                    *FvHandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL      *NewDevicePath;
  
  Node = *DevicePath;
  Status = gBS->LocateDevicePath (&gEfiFirmwareVolume2ProtocolGuid, &Node, &FvHandle);
  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (FvHandle, &gEfiFirmwareVolume2ProtocolGuid, (VOID **) &Fv);
    ASSERT_EFI_ERROR (Status);

    Status = Fv->ReadFile (
                   Fv,
                   EfiGetNameGuidFromFwVolDevicePathNode ((CONST MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) Node),
                   NULL,
                   &Size,
                   &Type,
                   &Attributes,
                   &AuthenticationStatus
                   );
    if (EFI_ERROR (Status)) {
      FreeAndSet (DevicePath, NULL);
    }
    return;
  }

    
  Node = NextDevicePathNode (DevicePath);

  //
  // Firstly find the FV file in current FV
  //
  gBS->HandleProtocol (
         gImageHandle,
         &gEfiLoadedImageProtocolGuid,
         (VOID **) &LoadedImage
         );
  NewDevicePath = AppendDevicePathNode (DevicePathFromHandle (LoadedImage->DeviceHandle), Node);
  FixupMemmapFvFilePath (&NewDevicePath);

  if (NewDevicePath != NULL) {
    FreeAndSet (DevicePath, NewDevicePath);
    return;
  }

  //
  // Secondly find the FV file in all other FVs
  //
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiFirmwareVolume2ProtocolGuid,
         NULL,
         &FvHandleCount,
         &FvHandleBuffer
         );
  for (Index = 0; Index < FvHandleCount; Index++) {
    if (FvHandleBuffer[Index] == LoadedImage->DeviceHandle) {
      //
      // Skip current FV
      //
      continue;
    }
    NewDevicePath = AppendDevicePathNode (DevicePathFromHandle (FvHandleBuffer[Index]), Node);
    FixupMemmapFvFilePath (&NewDevicePath);

    if (NewDevicePath != NULL) {
      FreeAndSet (DevicePath, NewDevicePath);
      return;
    }
  }
}

/**
  Check if it's of Fv file device path type.
  
  The function doesn't garentee the device path points to existing Fv file.

  @retval TRUE   The device path is of Fv file device path type.
  @retval FALSE  The device path isn't of Fv file device path type.
**/
BOOLEAN
IsMemmapFvFilePath (
  IN EFI_DEVICE_PATH_PROTOCOL    *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL   *FileNode;

  if ((DevicePathType (DevicePath) == HARDWARE_DEVICE_PATH) && (DevicePathSubType (DevicePath) == HW_MEMMAP_DP)) {
    FileNode = NextDevicePathNode (DevicePath);
    if ((DevicePathType (FileNode) == MEDIA_DEVICE_PATH) && (DevicePathSubType (FileNode) == MEDIA_PIWG_FW_FILE_DP)) {
      return IsDevicePathEnd (NextDevicePathNode (FileNode));
    }
  }

  return FALSE;
}

/**
  Check whether a USB device match the specified USB Class device path. This
  function follows "Load Option Processing" behavior in UEFI specification.

  @param UsbIo       USB I/O protocol associated with the USB device.
  @param UsbClass    The USB Class device path to match.

  @retval TRUE       The USB device match the USB Class device path.
  @retval FALSE      The USB device does not match the USB Class device path.

**/
BOOLEAN
MatchUsbClass (
  IN EFI_USB_IO_PROTOCOL        *UsbIo,
  IN USB_CLASS_DEVICE_PATH      *UsbClass
  )
{
  EFI_STATUS                    Status;
  EFI_USB_DEVICE_DESCRIPTOR     DevDesc;
  EFI_USB_INTERFACE_DESCRIPTOR  IfDesc;
  UINT8                         DeviceClass;
  UINT8                         DeviceSubClass;
  UINT8                         DeviceProtocol;

  if ((DevicePathType (UsbClass) != MESSAGING_DEVICE_PATH) ||
      (DevicePathSubType (UsbClass) != MSG_USB_CLASS_DP)){
    return FALSE;
  }

  //
  // Check Vendor Id and Product Id.
  //
  Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DevDesc);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  if ((UsbClass->VendorId != 0xffff) &&
      (UsbClass->VendorId != DevDesc.IdVendor)) {
    return FALSE;
  }

  if ((UsbClass->ProductId != 0xffff) &&
      (UsbClass->ProductId != DevDesc.IdProduct)) {
    return FALSE;
  }

  DeviceClass    = DevDesc.DeviceClass;
  DeviceSubClass = DevDesc.DeviceSubClass;
  DeviceProtocol = DevDesc.DeviceProtocol;
  if (DeviceClass == 0) {
    //
    // If Class in Device Descriptor is set to 0, use the Class, SubClass and
    // Protocol in Interface Descriptor instead.
    //
    Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &IfDesc);
    if (EFI_ERROR (Status)) {
      return FALSE;
    }

    DeviceClass    = IfDesc.InterfaceClass;
    DeviceSubClass = IfDesc.InterfaceSubClass;
    DeviceProtocol = IfDesc.InterfaceProtocol;
  }

  //
  // Check Class, SubClass and Protocol.
  //
  if ((UsbClass->DeviceClass != 0xff) &&
      (UsbClass->DeviceClass != DeviceClass)) {
    return FALSE;
  }

  if ((UsbClass->DeviceSubClass != 0xff) &&
      (UsbClass->DeviceSubClass != DeviceSubClass)) {
    return FALSE;
  }

  if ((UsbClass->DeviceProtocol != 0xff) &&
      (UsbClass->DeviceProtocol != DeviceProtocol)) {
    return FALSE;
  }

  return TRUE;
}

/**
  Eliminate the extra spaces in the Str to one space.
**/
VOID
EliminateExtraSpaces (
  IN CHAR16                    *Str
  )
{
  UINTN                        Index;
  UINTN                        ActualIndex;
  
  for (Index = 0, ActualIndex = 0; Str[Index] != L'\0'; Index++) {
    if ((Str[Index] != L' ') || ((ActualIndex > 0) && (Str[ActualIndex - 1] != L' '))) {
      Str[ActualIndex++] = Str[Index];
    }
  }
  
  Str[ActualIndex] = L'\0';

  while(ActualIndex--){
    if(Str[ActualIndex] == L' '){
      Str[ActualIndex] = 0;
    } else {
      break;
    }
  }
}



  






/**
  Try to get the controller's ATA/ATAPI description.

  @param Handle                Controller handle.

  @return  The description string.
**/
CHAR16 *
GetAtaAtapiDescription (
  IN EFI_HANDLE                Handle
  )
{
  UINTN                        Index;
  EFI_STATUS                   Status;
  EFI_DISK_INFO_PROTOCOL       *DiskInfo;
  UINT32                       BufferSize;
  EFI_ATAPI_IDENTIFY_DATA      *IdentifyData = NULL;
  CHAR16                       *ModelName = NULL;  
  CHAR16                       *NewString = NULL;
  CHAR16                       *SearNumber = NULL;
  UINTN                        AtaIndex;
  UINTN                        PlatSataHostIndex = 0;
  UINTN                        ModelNameSize = 64;
  UINTN                        PhysicsPortIndex = 0;

  if(!LibGetSataPortInfo(gBS, Handle, &PlatSataHostIndex, &AtaIndex, &PhysicsPortIndex)){
    Status = EFI_UNSUPPORTED;
    goto ProcExit;
  }

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDiskInfoProtocolGuid,
                  (VOID**)&DiskInfo
                  );
  if (EFI_ERROR (Status)) {
    goto ProcExit;
  }
  IdentifyData = AllocatePool(sizeof(EFI_ATAPI_IDENTIFY_DATA));
  if(IdentifyData == NULL){
    goto ProcExit;
  }  
  ModelName = AllocatePool((40+1)*sizeof(CHAR16));
  if(ModelName == NULL){
    goto ProcExit;
  } 

  SearNumber = AllocatePool((20+1)*sizeof(CHAR16));
  if(SearNumber == NULL){
    goto ProcExit;
  }  
  
  BufferSize = sizeof(EFI_ATAPI_IDENTIFY_DATA);
  Status = DiskInfo->Identify (
                       DiskInfo,
                       IdentifyData,
                       &BufferSize
                       );
  if (EFI_ERROR (Status)) {
    goto ProcExit;
  }
  for (Index = 0; Index < 40; Index += 2) {
    ModelName[Index]     = IdentifyData->ModelName[Index + 1];
    ModelName[Index + 1] = IdentifyData->ModelName[Index];
  }
  ModelName[Index] = L'\0';
  EliminateExtraSpaces(ModelName);

  for (Index = 0; Index + 1 < 20; Index += 2) {
     SearNumber[Index]     =  IdentifyData->SerialNo[Index + 1];
     SearNumber[Index + 1] =  IdentifyData->SerialNo[Index];
  }
  SearNumber[Index] = 0;
  EliminateExtraSpaces(SearNumber);

  NewString = AllocatePool(ModelNameSize*sizeof(CHAR16));
  if (NewString == NULL) {
    goto ProcExit;
  }  

if(PlatSataHostIndex & BIT15){
    UnicodeSPrint(NewString, ModelNameSize*sizeof(CHAR16), L"SATA %d: %s", PhysicsPortIndex, ModelName);
  } else {
    UnicodeSPrint(NewString, ModelNameSize*sizeof(CHAR16), L"SATA%d-%d: %s", PlatSataHostIndex, PhysicsPortIndex, ModelName);
  }

ProcExit:
  if(IdentifyData!=NULL){FreePool(IdentifyData);}
  if(ModelName!=NULL){FreePool(ModelName);}
  if(SearNumber!=NULL){FreePool(SearNumber);}
  return NewString;
}

CHAR16 *
AppendOsDescription (
  IN EFI_HANDLE                Handle
  )
{
  EFI_STATUS                          Status;
  EFI_HANDLE                          *SimpleFileSystemHandles;
  UINTN                               NumberSimpleFileSystemHandles;
  EFI_DEVICE_PATH_PROTOCOL            *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  UINTN                               Size,TempSize,Index;
  UINTN                               LoaderIndex;
  UINTN                               LoaderCount;
  EFI_IMAGE_DOS_HEADER                DosHeader;
  EFI_IMAGE_OPTIONAL_HEADER_UNION     HdrData;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION Hdr;
  CHAR16                              *Description;

  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Description= NULL;
  DevicePath = DevicePathFromHandle(Handle);
  ShowDevicePathDxe(gBS, DevicePath);
  //
  // Detect the the default boot file from removable Media
  //
  Size = GetDevicePathSize(DevicePath) - END_DEVICE_PATH_LENGTH;
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiSimpleFileSystemProtocolGuid,
         NULL,
         &NumberSimpleFileSystemHandles,
         &SimpleFileSystemHandles
         );
  for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
    //
    // Get the device path size of SimpleFileSystem handle
    //
    TempDevicePath = DevicePathFromHandle (SimpleFileSystemHandles[Index]);
    ShowDevicePathDxe(gBS, TempDevicePath);
    TempSize       = GetDevicePathSize (TempDevicePath)- END_DEVICE_PATH_LENGTH;
    //
    // Check whether the device path of boot option is part of the SimpleFileSystem handle's device path
    //
    if ((Size <= TempSize) && (CompareMem (TempDevicePath, DevicePath, Size) == 0)) {
      //
      // Load the default boot file \EFI\BOOT\boot{machinename}.EFI from removable Media
      //  machinename is ia32, ia64, x64, ...
      //
      Hdr.Union = &HdrData;
      LoaderCount = sizeof(mBdsGeneralUefiBootOs)/sizeof(mBdsGeneralUefiBootOs[0]);
      Status = EFI_NOT_FOUND;
      for(LoaderIndex=0;LoaderIndex<LoaderCount;LoaderIndex++){
        Status = GetImageHeader (
                   SimpleFileSystemHandles[Index],
                   mBdsGeneralUefiBootOs[LoaderIndex].FilePathString,
                   &DosHeader,
                   Hdr
                   );
        if(!EFI_ERROR(Status)){
          break;
        }
      }
      
      if (!EFI_ERROR (Status) && 
          EFI_IMAGE_MACHINE_TYPE_SUPPORTED (Hdr.Pe32->FileHeader.Machine) &&
          Hdr.Pe32->OptionalHeader.Subsystem == EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION) {
          if(mBdsGeneralUefiBootOs[LoaderIndex].Description !=NULL){
             Description = AllocateZeroPool (100);
             StrCpy(Description, mBdsGeneralUefiBootOs[LoaderIndex].Description);
          }
          break;
      }
    }
  }

  if (SimpleFileSystemHandles != NULL) {
    FreePool(SimpleFileSystemHandles);
  }
  DEBUG((EFI_D_INFO, "%a() %s\n", __FUNCTION__, Description));
  return Description;
}


/**
  Try to get the controller's USB description.

  @param Handle                Controller handle.

  @return  The description string.
**/
CHAR16 *
GetUsbDescription (
  IN EFI_HANDLE                Handle
  )
{
  EFI_STATUS                   Status;
  CHAR16                       *Description;
  UINT8                        ModelNumber[50];
  USB_BOOT_INQUIRY_DATA        UsbInquiryData;  
  UINT32                       BufferSize = 0;
  EFI_DISK_INFO_PROTOCOL       *DiskInfo;
  

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDiskInfoProtocolGuid,
                  &DiskInfo
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }             

  if(!CompareGuid(&DiskInfo->Interface, &gEfiDiskInfoUsbInterfaceGuid)){
    return NULL;
  }

  BufferSize = sizeof(USB_BOOT_INQUIRY_DATA);  
  Status = DiskInfo->Inquiry(
                     DiskInfo,
                     &UsbInquiryData,
                     &BufferSize
                     );
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  
  ZeroMem (ModelNumber, sizeof (ModelNumber));
  CopyMem (ModelNumber, UsbInquiryData.VendorID, 8);
  ModelNumber[8] = ' ';

  CopyMem (&ModelNumber[9], UsbInquiryData.ProductID, 16);
  ModelNumber[25] = '\0';

  Description = AllocateZeroPool (150);
  UnicodeSPrint (Description, 150, L"USB: %a", ModelNumber);
  EliminateExtraSpaces (Description);
  
  return Description;
}






STATIC CHAR16 *GetDescFromPci(EFI_HANDLE Handle, EFI_DEVICE_PATH_PROTOCOL *ptDevPath)
{
  EFI_PCI_IO_PROTOCOL            *ptPciIo;
  EFI_HANDLE                     PciHandle;
  UINT8                          ClassCode[3];
  EFI_DEVICE_PATH_PROTOCOL       *ptDP;
  CHAR16                         *DevName;
  EFI_STATUS                     Status;
  CHAR16                         *Name = NULL;  
  OPROM_PNP_DEV_NAME             *PnpName;
  UINTN                          MyStrSize;
  BYO_DISKINFO_PROTOCOL          *ByoDiskInfo;
  CHAR8                           Sn[21] = {0};
  UINTN                           Size;
  UINTN 						  Seg,Bus,Dev,Func;

  ptDP = ptDevPath;
  Status = gBS->LocateDevicePath (
                  &gEfiPciIoProtocolGuid,
                  &ptDP,
                  &PciHandle
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
      
  Status = gBS->HandleProtocol(
                  PciHandle,
                  &gEfiPciIoProtocolGuid,
                  &ptPciIo
                  );
  ASSERT(!EFI_ERROR(Status));
  ptPciIo->GetLocation(ptPciIo, &Seg, &Bus, &Dev, &Func);
  DEBUG((EFI_D_INFO, "(%X,%X,%X) %X\n", Bus, Dev, Func));
      
  Status = ptPciIo->Pci.Read(
                          ptPciIo, 
                          EfiPciIoWidthUint8,  
                          PCI_CLASSCODE_OFFSET, 
                          3, 
                          ClassCode
                          );
  if(ClassCode[2] == 8 && 
     ClassCode[1] == 5 && 
    (ClassCode[0] == 0 || ClassCode[0] == 1)){      // SD
    Name = L"SD";
    Name = AllocateCopyPool(StrSize(Name), Name);
  } else if(ClassCode[2] == 1 && ClassCode[1] == 8 && ClassCode[0] == 2){ // NVMe
    DevName = LibOpalDriverGetDriverDeviceName(gBS, Handle);
    if(DevName == NULL){
      Name = L"NVME";
      Name = AllocateCopyPool(StrSize(Name), Name);
    } else {

      Status = gBS->HandleProtocol (
                    Handle,
                    &gByoDiskInfoProtocolGuid,
                    (VOID **)&ByoDiskInfo
                    );
      if (EFI_ERROR (Status)) {
      DEBUG((EFI_D_INFO,"locate byodiskinfo protocol is Error\n"));
      Name = L"NVME";
      return Name;
      }

    Size = sizeof(Sn);
    Status = ByoDiskInfo->GetSn(ByoDiskInfo, Sn, &Size);

      MyStrSize = StrSize(DevName) + Size + 36;
      Name = AllocateZeroPool(MyStrSize);
      UnicodeSPrint(Name, MyStrSize, L"NVME(PCI%x-%x-%x): %s", Bus, Dev, Func, DevName);
      FreePool(DevName);
    }
  } else {
    Status = gBS->HandleProtocol(
                    Handle,
                    &gOpromPnpNameProtocolGuid,
                    &PnpName
                    );
    if(!EFI_ERROR(Status)){
      Name = AllocateCopyPool(StrSize(PnpName->Name), PnpName->Name);
    } else {
      Name = NULL;
    }
  }

ProcExit:
  return Name;
}




/**
  Return the boot description for the controller based on the type.

  @param Handle                Controller handle.

  @return  The description string.
**/
CHAR16 *
GetMiscDescription (
  IN EFI_HANDLE                  Handle
  )
{
  EFI_STATUS                     Status;
  CHAR16                         *Description = NULL;
  EFI_BLOCK_IO_PROTOCOL          *BlockIo;
  EFI_DEVICE_PATH_PROTOCOL       *ptDevPath;
  BOOT_TYPE                      BootType;
  CHAR16                         *StrOut;
  BOOLEAN                        NeedFree = 0;
  
  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  
  ptDevPath = DevicePathFromHandle(Handle);
  BootType  = BootTypeFromDevicePath(ptDevPath);
  
  switch (BootType) {
    case AcpiFloppyBoot:
      Description = L"Floppy";
      break;

    case MessageAtapiBoot:
    case MessageSataBoot:
      Status = gBS->HandleProtocol (Handle, &gEfiBlockIoProtocolGuid, (VOID **) &BlockIo);
      ASSERT_EFI_ERROR (Status);
      Description = BlockIo->Media->RemovableMedia ? L"ODD" : L"HDD";
      break;

    case MessageUsbBoot:
      Description = L"USB Device";
      break;

    case MessageScsiBoot:
      Description = L"SCSI Device";
      break;

    case UnsupportedBoot:
      Description = GetDescFromPci(Handle, ptDevPath);
      if(Description == NULL){
        Description = L"Misc Device";
      } else {
        NeedFree = 1;
      }
      break;

    default:
      Description = L"Misc Device";
      break;
  }

  StrOut = AllocateCopyPool(StrSize(Description), Description);
  if(NeedFree && Description!=NULL){
    FreePool(Description);
  }
  return StrOut;
}








EFI_STATUS
AtaReadIdentifyData (
  IN  EFI_ATA_PASS_THRU_PROTOCOL    *ptAtaPassThru,
  IN  UINT16                        Port,
  IN  UINT16                        PortMP,
  OUT ATA_IDENTIFY_DATA             *IdentifyData
  )
{
  EFI_STATUS                        Status;
  EFI_ATA_COMMAND_BLOCK             Acb;
  EFI_ATA_STATUS_BLOCK              Asb;
  EFI_ATA_PASS_THRU_COMMAND_PACKET  Packet;

  if((ptAtaPassThru == NULL) || (IdentifyData == NULL)){
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem(&Acb, sizeof(Acb));
  ZeroMem(&Asb, sizeof(Asb));
  Acb.AtaCommand    = ATA_CMD_IDENTIFY_DRIVE;
  Acb.AtaDeviceHead = (UINT8) (BIT7 | BIT6 | BIT5 | (PortMP == 0xFFFF ? 0 : (PortMP << 4)));

  ZeroMem(&Packet, sizeof(Packet));
  Packet.Protocol         = EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_IN;
  Packet.Length           = EFI_ATA_PASS_THRU_LENGTH_BYTES;
  Packet.Asb              = &Asb;
  Packet.Acb              = &Acb;
  Packet.InDataBuffer     = IdentifyData;
  Packet.InTransferLength = sizeof(ATA_IDENTIFY_DATA);
  Packet.Timeout          = EFI_TIMER_PERIOD_SECONDS (3);

  Status = ptAtaPassThru->PassThru (
                            ptAtaPassThru,
                            Port,
                            PortMP,
                            &Packet,
                            NULL
                            );
  
  return Status;
}





//
// DP:PciRoot(0x3)/Pci(0x3,0x2)/Pci(0x0,0x0)/SAS(0xC5FAEC803954B66,0x0,0x1,SAS,External,Expanded,0,0x0)
//
// DP:PciRoot(0x3)/Pci(0x3,0x2)/Pci(0x0,0x0)/SAS(0x500E004AAAAAAA03,0x0,0x1,SATA,External,Expanded,0,0x0)
//                                        DP:SAS(0x500E004AAAAAAA03,0x0,0x1,SATA,External,Expanded,0,0x0)
// DP:PciRoot(0x3)/Pci(0x3,0x2)/Pci(0x0,0x0)/SAS(0x500E004AAAAAAA02,0x0,0x1,SATA,External,Expanded,0,0x0)
//                                        DP:SAS(0x500E004AAAAAAA02,0x0,0x1,SATA,External,Expanded,0,0x0)
UINT32
GetSasCtrlNum (
  EFI_HANDLE                Handle
  )
{
  UINT32      Index = 0;

  for (Index = 0; Index < MAX_SAS_CTRL_NUM; Index++) {
    if (mSasHandleList[Index] == NULL) {
      mSasHandleList[Index] = Handle;
      return Index;
    }
    if (Handle == mSasHandleList[Index]) {
      return Index;
    }
  }
  return MAX_SAS_CTRL_NUM;
}

CHAR16 *
GetSasDescription (
  IN EFI_HANDLE                Handle
  )
{
  EFI_DEVICE_PATH_PROTOCOL       *Dp;
  EFI_DEVICE_PATH_PROTOCOL       *Node;
  EFI_DEVICE_PATH_PROTOCOL       *Temp;
  EFI_DEVICE_PATH_PROTOCOL       *PortDp;
  SAS_DEVICE_PATH                *SasDp;
  UINT16                         Info;
  CHAR16                         *p;
  EFI_STATUS                     Status;
  EFI_HANDLE                     SasHandle;
  EFI_ATA_PASS_THRU_PROTOCOL     *AtaPassThru;
  UINT16                         Port;
  UINT16                         PortMp; 
  UINT16                         SavedPort = 0xFFFF;
  UINT16                         SavedPortMp = 0xFFFF;   
  BOOLEAN                        PortFound;
  ATA_IDENTIFY_DATA              *IdentifyData;
  CHAR8                          ModelName[40 + 1];
  CHAR16                         Buffer[64];
  UINT32                         SasCtrlIndex = 0;

  Dp = DevicePathFromHandle(Handle);

  for (Node = Dp; !IsDevicePathEndType(Node); Node = NextDevicePathNode(Node)) {
    if(DevicePathType(Node) == MESSAGING_DEVICE_PATH && DevicePathSubType(Node) == MSG_VENDOR_DP) {  
      SasDp = (SAS_DEVICE_PATH*)Node;
      if(!CompareGuid(&SasDp->Guid, &gEfiSasDevicePathGuid)){
        continue;
      }
      Info = SasDp->DeviceTopology;
      if ((Info & 0x0f) <= 2 && (Info & BIT7) == 0 && (Info & BIT4)){
        Temp = Dp;
        Status = gBS->LocateDevicePath(&gEfiAtaPassThruProtocolGuid, &Temp, &SasHandle);
        if(!EFI_ERROR(Status)){
          Status = gBS->HandleProtocol (
                          SasHandle,
                          &gEfiAtaPassThruProtocolGuid,
                          (VOID*)&AtaPassThru
                          );
          
          PortFound = FALSE;
          Port = 0xFFFF;
          while (TRUE) {
            Status = AtaPassThru->GetNextPort (AtaPassThru, &Port);
            if (EFI_ERROR (Status)) {
              break;
            }

            PortMp = 0xFFFF;
            while (TRUE) {
              Status = AtaPassThru->GetNextDevice(AtaPassThru, Port, &PortMp);
              if (EFI_ERROR (Status)) {
                break;
              }
            }

            if(!PortFound){
              Status = AtaPassThru->BuildDevicePath(AtaPassThru, Port, PortMp, &PortDp);
              if(!EFI_ERROR (Status)){
                if(CompareMem(PortDp, Temp, GetDevicePathSize(PortDp)) == 0){
                  SavedPort = Port;
                  SavedPortMp = PortMp;
                  PortFound = TRUE;
                }
                FreePool(PortDp);
              }
            }
          }
          
          if(PortFound){

            Port   = SavedPort;
            PortMp = SavedPortMp;
            DEBUG((EFI_D_INFO, "SAS Found Port(%d,%d)\n", Port, PortMp));
            
            IdentifyData = AllocateBsZeroMemoryBelow4G(gBS, sizeof(ATA_IDENTIFY_DATA));
            ASSERT(IdentifyData != NULL);
            Status = AtaReadIdentifyData(AtaPassThru, Port, PortMp, IdentifyData);
            if(!EFI_ERROR(Status)){
              SasCtrlIndex = GetSasCtrlNum (SasHandle);
              DEBUG((EFI_D_INFO, "SAS SasCtrlIndex %d \n", SasCtrlIndex));
              CopyMem(ModelName, IdentifyData->ModelName, sizeof(IdentifyData->ModelName));
              SwapWordArray(ModelName, 40);
              ModelName[40] = 0;
              TrimStr8(ModelName);
              UnicodeSPrint(Buffer, sizeof(Buffer), L"SAS%d-%d: %a", SasCtrlIndex, Port, ModelName);
              p = AllocateCopyPool(StrSize(Buffer), Buffer);
              FreePages(IdentifyData, EFI_SIZE_TO_PAGES(sizeof(ATA_IDENTIFY_DATA)));              
              return p;              
            }
            FreePages(IdentifyData, EFI_SIZE_TO_PAGES(sizeof(ATA_IDENTIFY_DATA)));
          }
        }
      }
      
      UnicodeSPrint(Buffer, sizeof(Buffer), L"SAS(0x%lx,0x%lx,0x%x)", SasDp->SasAddress, SasDp->Lun, SasDp->RelativeTargetPort);
      p = AllocateCopyPool(StrSize(Buffer), Buffer);
      return p;
    }
  }

  return NULL;
}



CHAR16 *
GetRaidDescription (
  IN EFI_HANDLE                  Handle
  )
{
  EFI_PCI_IO_PROTOCOL            *ptPciIo;
  EFI_HANDLE                     PciHandle;
  EFI_HANDLE                     ScsiHandle;  
  UINT8                          ClassCode[3];
  EFI_DEVICE_PATH_PROTOCOL       *ptDP;
  CHAR16                         *DevName;
  EFI_STATUS                     Status;
  CHAR16                         *Name = NULL;  
  OPROM_PNP_DEV_NAME             *PnpName;
  UINTN                          MyStrSize;
  EFI_DEVICE_PATH_PROTOCOL       *ptDevPath;
  UINT16                          VendorId;
  EFI_SCSI_PASS_THRU_PROTOCOL     *ScsiPassThru;
  UINT16                          Pun;
  UINT32                         ControllerNum = 0;



  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  ptDevPath = DevicePathFromHandle(Handle);
  ptDP = ptDevPath;
  Status = gBS->LocateDevicePath (
                  &gEfiPciIoProtocolGuid,
                  &ptDP,
                  &PciHandle
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
      
  Status = gBS->HandleProtocol(
                  PciHandle,
                  &gEfiPciIoProtocolGuid,
                  &ptPciIo
                  );
  ASSERT(!EFI_ERROR(Status));
      
  Status = ptPciIo->Pci.Read(
                          ptPciIo, 
                          EfiPciIoWidthUint8,  
                          PCI_CLASSCODE_OFFSET, 
                          3, 
                          ClassCode
                          );
  if(ClassCode[2] == PCI_CLASS_MASS_STORAGE && (ClassCode[1] == PCI_CLASS_MASS_STORAGE_RAID || ClassCode[1] == PCI_CLASS_MASS_STORAGE_SAS)){
    DevName = LibDriverGetDriverDeviceName(gBS, ptDevPath);
    if(DevName != NULL){
      MyStrSize = StrSize(DevName) + 32;
      Name = AllocateZeroPool(MyStrSize);
      UnicodeSPrint(Name, MyStrSize, L"%s", DevName);
      FreePool(DevName);
    }else{
      ptPciIo->Pci.Read (
               ptPciIo,
               EfiPciIoWidthUint32,
               0,
               1,
               &VendorId
               );
      if(VendorId == 0x1000||VendorId == 0x9005){ //AVAGO LSI MEAGRAID
           ptDP = ptDevPath;
           ShowDevicePathDxe(gBS, ptDP);
           Status = gBS->LocateDevicePath (
                            &gEfiScsiPassThruProtocolGuid,
                            &ptDP,
                            &ScsiHandle
                            );
           if(EFI_ERROR(Status)){
              goto ProcExit;
           }
           Status = gBS->HandleProtocol(
                ScsiHandle,
                &gEfiScsiPassThruProtocolGuid,
                &ScsiPassThru
                );
           if(EFI_ERROR(Status)){
              goto ProcExit;
           }
           Pun = ((SCSI_DEVICE_PATH *)ptDP)->Pun;
           for (ptDP = ptDevPath; !IsDevicePathEndType (ptDP); ptDP = NextDevicePathNode (ptDP)) {
                if(DevicePathType (ptDP) == HARDWARE_DEVICE_PATH && DevicePathSubType(ptDP) == HW_CONTROLLER_DP){
                   ControllerNum = ((CONTROLLER_DEVICE_PATH *)ptDP)->ControllerNumber;
                   break;
                }
           }
           MyStrSize = StrSize(ScsiPassThru->Mode->ControllerName) + StrSize(ScsiPassThru->Mode->ChannelName) + 32;
           Name = AllocateZeroPool(MyStrSize);
           UnicodeSPrint(Name, MyStrSize, L"%s Drive %d %s Pun %d", ScsiPassThru->Mode->ControllerName,ControllerNum,ScsiPassThru->Mode->ChannelName,Pun);
      }
    }    
  } else {
    Status = gBS->HandleProtocol(
                    Handle,
                    &gOpromPnpNameProtocolGuid,
                    &PnpName
                    );
    if(!EFI_ERROR(Status)){
      Name = AllocateCopyPool(StrSize(PnpName->Name), PnpName->Name);
    } else {
      Name = NULL;
    }
  }

ProcExit:
  return Name;
}


GET_BOOT_DESCRIPTION mGetBootDescription[] = {
  GetUsbDescription,
  GetAtaAtapiDescription,
  GetSasDescription,
  GetRaidDescription,
  GetMiscDescription
};

UINTN mGetBootDescriptionCount = (sizeof(mGetBootDescription)/sizeof(mGetBootDescription[0]));

/**
  Check whether a USB device match the specified USB WWID device path. This
  function follows "Load Option Processing" behavior in UEFI specification.

  @param UsbIo       USB I/O protocol associated with the USB device.
  @param UsbWwid     The USB WWID device path to match.

  @retval TRUE       The USB device match the USB WWID device path.
  @retval FALSE      The USB device does not match the USB WWID device path.

**/
BOOLEAN
MatchUsbWwid (
  IN EFI_USB_IO_PROTOCOL        *UsbIo,
  IN USB_WWID_DEVICE_PATH       *UsbWwid
  )
{
  EFI_STATUS                   Status;
  EFI_USB_DEVICE_DESCRIPTOR    DevDesc;
  EFI_USB_INTERFACE_DESCRIPTOR IfDesc;
  UINT16                       *LangIdTable;
  UINT16                       TableSize;
  UINT16                       Index;
  CHAR16                       *CompareStr;
  UINTN                        CompareLen;
  CHAR16                       *SerialNumberStr;
  UINTN                        Length;

  if ((DevicePathType (UsbWwid) != MESSAGING_DEVICE_PATH) ||
      (DevicePathSubType (UsbWwid) != MSG_USB_WWID_DP)) {
    return FALSE;
  }

  //
  // Check Vendor Id and Product Id.
  //
  Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DevDesc);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  if ((DevDesc.IdVendor != UsbWwid->VendorId) ||
      (DevDesc.IdProduct != UsbWwid->ProductId)) {
    return FALSE;
  }

  //
  // Check Interface Number.
  //
  Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &IfDesc);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  if (IfDesc.InterfaceNumber != UsbWwid->InterfaceNumber) {
    return FALSE;
  }

  //
  // Check Serial Number.
  //
  if (DevDesc.StrSerialNumber == 0) {
    return FALSE;
  }

  //
  // Get all supported languages.
  //
  TableSize = 0;
  LangIdTable = NULL;
  Status = UsbIo->UsbGetSupportedLanguages (UsbIo, &LangIdTable, &TableSize);
  if (EFI_ERROR (Status) || (TableSize == 0) || (LangIdTable == NULL)) {
    return FALSE;
  }

  //
  // Serial number in USB WWID device path is the last 64-or-less UTF-16 characters.
  //
  CompareStr = (CHAR16 *) (UINTN) (UsbWwid + 1);
  CompareLen = (DevicePathNodeLength (UsbWwid) - sizeof (USB_WWID_DEVICE_PATH)) / sizeof (CHAR16);
  if (CompareStr[CompareLen - 1] == L'\0') {
    CompareLen--;
  }

  //
  // Compare serial number in each supported language.
  //
  for (Index = 0; Index < TableSize / sizeof (UINT16); Index++) {
    SerialNumberStr = NULL;
    Status = UsbIo->UsbGetStringDescriptor (
                      UsbIo,
                      LangIdTable[Index],
                      DevDesc.StrSerialNumber,
                      &SerialNumberStr
                      );
    if (EFI_ERROR (Status) || (SerialNumberStr == NULL)) {
      continue;
    }

    Length = StrLen (SerialNumberStr);
    if ((Length >= CompareLen) &&
        (CompareMem (SerialNumberStr + Length - CompareLen, CompareStr, CompareLen * sizeof (CHAR16)) == 0)) {
      FreePool (SerialNumberStr);
      return TRUE;
    }

    FreePool (SerialNumberStr);
  }

  return FALSE;
}

VOID
PrintDp (
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath
  )
{
  EFI_STATUS                          Status;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL    *ToText;
  CHAR16                              *DpStr;

  Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &ToText);
  ASSERT_EFI_ERROR (Status);

  DpStr = ToText->ConvertDevicePathToText (DevicePath, FALSE, FALSE);
  DEBUG ((EFI_D_INFO, "%s", DpStr));
  FreePool (DpStr);
}

/**
  Find a USB device which match the specified short-form device path start with 
  USB Class or USB WWID device path. If ParentDevicePath is NULL, this function
  will search in all USB devices of the platform. If ParentDevicePath is not NULL,
  this function will only search in its child devices.

  @param DevicePath           The device path that contains USB Class or USB WWID device path.
  @param ParentDevicePathSize The length of the device path before the USB Class or 
                              USB WWID device path.
  @param UsbIoHandleCount     A pointer to the count of the returned USB IO handles.

  @retval NULL       The matched USB IO handles cannot be found.
  @retval other      The matched USB IO handles.

**/
EFI_HANDLE *
FindUsbDevice (
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN  UINTN                     ParentDevicePathSize,
  OUT UINTN                     *UsbIoHandleCount
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                *UsbIoHandles;
  EFI_DEVICE_PATH_PROTOCOL  *UsbIoDevicePath;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  UINTN                     Index;
  UINTN                     UsbIoDevicePathSize;
  BOOLEAN                   Matched;

  ASSERT (UsbIoHandleCount != NULL);  

  //
  // Get all UsbIo Handles.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiUsbIoProtocolGuid,
                  NULL,
                  UsbIoHandleCount,
                  &UsbIoHandles
                  );
  if (EFI_ERROR (Status)) {
    *UsbIoHandleCount = 0;
    UsbIoHandles      = NULL;
  }

  for (Index = 0; Index < *UsbIoHandleCount; ) {
    //
    // Get the Usb IO interface.
    //
    Status = gBS->HandleProtocol(
                    UsbIoHandles[Index],
                    &gEfiUsbIoProtocolGuid,
                    (VOID **) &UsbIo
                    );
    UsbIoDevicePath = DevicePathFromHandle (UsbIoHandles[Index]);
    Matched         = FALSE;
    if (!EFI_ERROR (Status) && (UsbIoDevicePath != NULL)) {
      UsbIoDevicePathSize = GetDevicePathSize (UsbIoDevicePath) - END_DEVICE_PATH_LENGTH;

      //
      // Compare starting part of UsbIoHandle's device path with ParentDevicePath.
      //
      if (CompareMem (UsbIoDevicePath, DevicePath, ParentDevicePathSize) == 0) {
        if (MatchUsbClass (UsbIo, (USB_CLASS_DEVICE_PATH *) ((UINTN) DevicePath + ParentDevicePathSize)) ||
            MatchUsbWwid (UsbIo, (USB_WWID_DEVICE_PATH *) ((UINTN) DevicePath + ParentDevicePathSize))) {
          Matched = TRUE;
        }
      }
    }

    if (!Matched) {
      (*UsbIoHandleCount) --;
      CopyMem (&UsbIoHandles[Index], &UsbIoHandles[Index + 1], (*UsbIoHandleCount - Index) * sizeof (EFI_HANDLE));
    } else {
      Index++;
    }
  }

  return UsbIoHandles;
}

/**
  Expand USB Class or USB WWID device path node to be full device path of a USB
  device in platform.

  This function support following 4 cases:
  1) Boot Option device path starts with a USB Class or USB WWID device path,
     and there is no Media FilePath device path in the end.
     In this case, it will follow Removable Media Boot Behavior.
  2) Boot Option device path starts with a USB Class or USB WWID device path,
     and ended with Media FilePath device path.
  3) Boot Option device path starts with a full device path to a USB Host Controller,
     contains a USB Class or USB WWID device path node, while not ended with Media
     FilePath device path. In this case, it will follow Removable Media Boot Behavior.
  4) Boot Option device path starts with a full device path to a USB Host Controller,
     contains a USB Class or USB WWID device path node, and ended with Media
     FilePath device path.

  @param  DevicePath    On input, a pointer to an allocated buffer that contains the 
                        file device path.
                        On output, a pointer to an reallocated buffer that contains 
                        the expanded device path. It would point to NULL if the file
                        cannot be read.

  @param  FileSize      A pointer to the file size.

  @retval !NULL  The file buffer.
  @retval NULL   The input device path doesn't point to a valid file.
**/
VOID *
ExpandUsbShortFormDevicePath (
  IN OUT EFI_DEVICE_PATH_PROTOCOL  **DevicePath,
  OUT UINTN                        *FileSize
  )
{
  UINTN                             ParentDevicePathSize;
  EFI_DEVICE_PATH_PROTOCOL          *ShortformNode;
  EFI_DEVICE_PATH_PROTOCOL          *RemainingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL          *FullDevicePath;
  EFI_HANDLE                        *UsbIoHandles;
  UINTN                             UsbIoHandleCount;
  UINTN                             Index;
  VOID                              *FileBuffer;
  
  //
  // Search for USB Class or USB WWID device path node.
  //
  for ( ShortformNode = *DevicePath
      ; !IsDevicePathEnd (ShortformNode)
      ; ShortformNode = NextDevicePathNode (ShortformNode)
      ) {
    if ((DevicePathType (ShortformNode) == MESSAGING_DEVICE_PATH) &&
        ((DevicePathSubType (ShortformNode) == MSG_USB_CLASS_DP) ||
         (DevicePathSubType (ShortformNode) == MSG_USB_WWID_DP))) {
      break;
    }
  }
  ASSERT (!IsDevicePathEnd (ShortformNode));

  FullDevicePath       = NULL;
  ParentDevicePathSize = (UINTN) ShortformNode - (UINTN) *DevicePath;
  RemainingDevicePath  = NextDevicePathNode (ShortformNode);
  FileBuffer           = NULL;
  UsbIoHandles         = FindUsbDevice (*DevicePath, ParentDevicePathSize, &UsbIoHandleCount);

  for (Index = 0; Index < UsbIoHandleCount; Index++) {
    FullDevicePath = AppendDevicePath (DevicePathFromHandle (UsbIoHandles[Index]), RemainingDevicePath);
    DEBUG ((EFI_D_INFO, "[Bds] FullDp1[%d]:", Index)); DEBUG_CODE (PrintDp (FullDevicePath); ); DEBUG ((EFI_D_INFO, "\n"));
    FileBuffer = LoadEfiBootOption (&FullDevicePath, FileSize);
    if (FileBuffer != NULL) {
      DEBUG ((EFI_D_INFO, "-->")); DEBUG_CODE (PrintDp (FullDevicePath); ); DEBUG ((EFI_D_INFO, FileBuffer != NULL ? " - Found\n" : "\n"));
      break;
    }
  }

  if (UsbIoHandles != NULL) {
    FreePool (UsbIoHandles);
  }

  if (FileBuffer == NULL) {
    //
    // Boot Option device path starts with USB Class or USB WWID device path.
    // For Boot Option device path which doesn't begin with the USB Class or
    // USB WWID device path, it's not needed to connect again here.
    //
    if ((DevicePathType (*DevicePath) == MESSAGING_DEVICE_PATH) &&
        ((DevicePathSubType (*DevicePath) == MSG_USB_CLASS_DP) ||
         (DevicePathSubType (*DevicePath) == MSG_USB_WWID_DP))) {
      EfiBootManagerConnectUsbShortFormDevicePath (*DevicePath);

      UsbIoHandles = FindUsbDevice (*DevicePath, ParentDevicePathSize, &UsbIoHandleCount);
      for (Index = 0; Index < UsbIoHandleCount; Index++) {
        FullDevicePath = AppendDevicePath (DevicePathFromHandle (UsbIoHandles[Index]), RemainingDevicePath);
        DEBUG ((EFI_D_INFO, "[Bds] FullDp2[%d]:", Index)); DEBUG_CODE (PrintDp (FullDevicePath); ); DEBUG ((EFI_D_INFO, "\n"));
        FileBuffer = LoadEfiBootOption (&FullDevicePath, FileSize);
        if (FileBuffer != NULL) {
          DEBUG ((EFI_D_INFO, "-->")); DEBUG_CODE (PrintDp (FullDevicePath); ); DEBUG ((EFI_D_INFO, FileBuffer != NULL ? " - Found\n" : "\n"));
          break;
        }
      }

      if (UsbIoHandles != NULL) {
        FreePool (UsbIoHandles);
      }
    }
  }

  FreeAndSet (DevicePath, FullDevicePath);
  return FileBuffer;
}

/**
  Expand a device path that starts with a hard drive media device path node to be a
  full device path that includes the full hardware path to the device. We need
  to do this so it can be booted. As an optimization the front match (the part point
  to the partition node. E.g. ACPI() /PCI()/ATA()/Partition() ) is saved in a variable
  so a connect all is not required on every boot. All successful history device path
  which point to partition node (the front part) will be saved.

  @param  DevicePath    On input, a pointer to an allocated buffer that contains the 
                        file device path.
                        On output, a pointer to an reallocated buffer that contains 
                        the expanded device path. It would point to NULL if the file
                        cannot be read.

**/
VOID
ExpandPartitionShortFormDevicePath (
  IN OUT EFI_DEVICE_PATH_PROTOCOL      **DevicePath
  )
{
  EFI_STATUS                Status;
  UINTN                     BlockIoHandleCount;
  EFI_HANDLE                *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *FullDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath;
  UINTN                     Index;
  UINTN                     InstanceNum;
  EFI_DEVICE_PATH_PROTOCOL  *CachedDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempNewDevicePath;
  UINTN                     CachedDevicePathSize;
  BOOLEAN                   DeviceExist;
  BOOLEAN                   NeedAdjust;
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  UINTN                     Size;

  FullDevicePath      = NULL;
  //
  // Check if there is prestore 'HDDP' variable.
  // If exist, search the front path which point to partition node in the variable instants.
  // If fail to find or 'HDDP' not exist, reconnect all and search in all system
  //
  CachedDevicePath = EfiBootManagerGetVariableAndSize (
                      L"HDDP",
                      &mHdBootVariablePrivateGuid,
                      &CachedDevicePathSize
                      );

  if (CachedDevicePath != NULL) {
    TempNewDevicePath = CachedDevicePath;
    DeviceExist = FALSE;
    NeedAdjust = FALSE;
    do {
      //
      // Check every instance of the variable
      // First, check whether the instance contain the partition node, which is needed for distinguishing  multi
      // partial partition boot option. Second, check whether the instance could be connected.
      //
      Instance  = GetNextDevicePathInstance (&TempNewDevicePath, &Size);
      if (MatchPartitionDevicePathNode (Instance, (HARDDRIVE_DEVICE_PATH *) *DevicePath)) {
        //
        // Connect the device path instance, the device path point to hard drive media device path node
        // e.g. ACPI() /PCI()/ATA()/Partition()
        //
        Status = EfiBootManagerConnectDevicePath (Instance, NULL);
        if (!EFI_ERROR (Status)) {
          DeviceExist = TRUE;
          break;
        }
      }
      //
      // Come here means the first instance is not matched
      //
      NeedAdjust = TRUE;
      FreePool(Instance);
    } while (TempNewDevicePath != NULL);

    if (DeviceExist) {
      //
      // Find the matched device path.
      // Append the file path information from the boot option and return the fully expanded device path.
      //
      FullDevicePath = AppendDevicePath (Instance, NextDevicePathNode (*DevicePath));

      //
      // Adjust the 'HDDP' instances sequence if the matched one is not first one.
      //
      if (NeedAdjust) {
        //
        // First delete the matched instance.
        //
        TempNewDevicePath = CachedDevicePath;
        CachedDevicePath  = EfiBootManagerDelPartMatchInstance (CachedDevicePath, Instance);
        FreePool (TempNewDevicePath);

        //
        // Second, append the remaining path after the matched instance
        //
        TempNewDevicePath = CachedDevicePath;
        CachedDevicePath = AppendDevicePathInstance (Instance, CachedDevicePath );
        FreePool (TempNewDevicePath);
        //
        // Save the matching Device Path so we don't need to do a connect all next time
        //
        Status = gRT->SetVariable (
                        L"HDDP",
                        &mHdBootVariablePrivateGuid,
                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                        GetDevicePathSize (CachedDevicePath),
                        CachedDevicePath
                        );
      }

      FreePool (Instance);
      FreePool (CachedDevicePath);
      FreePool (*DevicePath);
      *DevicePath = FullDevicePath;
      return;
    }
  }

  //
  // If we get here we fail to find or 'HDDP' not exist, and now we need
  // to search all devices in the system for a matched partition
  //
  ByoEfiBootManagerConnectAll ();
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &BlockIoHandleCount, &BlockIoBuffer);
  if (EFI_ERROR (Status)) {
    BlockIoHandleCount = 0;
    BlockIoBuffer      = NULL;
  }
  //
  // Loop through all the device handles that support the BLOCK_IO Protocol
  //
  for (Index = 0; Index < BlockIoHandleCount; Index++) {

    Status = gBS->HandleProtocol (BlockIoBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID *) &BlockIoDevicePath);
    if (EFI_ERROR (Status) || BlockIoDevicePath == NULL) {
      continue;
    }

    if (MatchPartitionDevicePathNode (BlockIoDevicePath, (HARDDRIVE_DEVICE_PATH *) *DevicePath)) {
      //
      // Find the matched partition device path
      //
      FullDevicePath = AppendDevicePath (BlockIoDevicePath, NextDevicePathNode (*DevicePath));

      //
      // Save the matched partition device path in 'HDDP' variable
      //
      if (CachedDevicePath != NULL) {
        //
        // Save the matched partition device path as first instance of 'HDDP' variable
        //
        if (EfiBootManagerMatchDevicePaths (CachedDevicePath, BlockIoDevicePath)) {
          TempNewDevicePath = CachedDevicePath;
          CachedDevicePath = EfiBootManagerDelPartMatchInstance (CachedDevicePath, BlockIoDevicePath);
          FreePool(TempNewDevicePath);

          TempNewDevicePath = CachedDevicePath;
          CachedDevicePath = AppendDevicePathInstance (BlockIoDevicePath, CachedDevicePath);
          if (TempNewDevicePath != NULL) {
            FreePool(TempNewDevicePath);
          }
        } else {
          TempNewDevicePath = CachedDevicePath;
          CachedDevicePath = AppendDevicePathInstance (BlockIoDevicePath, CachedDevicePath);
          FreePool(TempNewDevicePath);
        }
        //
        // Here limit the device path instance number to 12, which is max number for a system support 3 IDE controller
        // If the user try to boot many OS in different HDs or partitions, in theory, the 'HDDP' variable maybe become larger and larger.
        //
        InstanceNum = 0;
        ASSERT (CachedDevicePath != NULL);
        TempNewDevicePath = CachedDevicePath;
        while (!IsDevicePathEnd (TempNewDevicePath)) {
          TempNewDevicePath = NextDevicePathNode (TempNewDevicePath);
          //
          // Parse one instance
          //
          while (!IsDevicePathEndType (TempNewDevicePath)) {
            TempNewDevicePath = NextDevicePathNode (TempNewDevicePath);
          }
          InstanceNum++;
          //
          // If the CachedDevicePath variable contain too much instance, only remain 12 instances.
          //
          if (InstanceNum >= 12) {
            SetDevicePathEndNode (TempNewDevicePath);
            break;
          }
        }
      } else {
        CachedDevicePath = DuplicateDevicePath (BlockIoDevicePath);
      }

      //
      // Save the matching Device Path so we don't need to do a connect all next time
      //
      Status = gRT->SetVariable (
                      L"HDDP",
                      &mHdBootVariablePrivateGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      GetDevicePathSize (CachedDevicePath),
                      CachedDevicePath
                      );

      break;
    }
  }

  if (CachedDevicePath != NULL) {
    FreePool (CachedDevicePath);
  }
  if (BlockIoBuffer != NULL) {
    FreePool (BlockIoBuffer);
  }
  FreeAndSet (DevicePath, FullDevicePath);
}

/**
  Algorithm follows the UEFI Spec chapter 3.4 Boot Mechanisms.

  @param  DevicePath  Device Path to a  bootable device

  @return  The bootable media handle. If the media on the DevicePath is not bootable, NULL will return.

**/
EFI_HANDLE
GetBootableDeviceHandle (
  IN  EFI_DEVICE_PATH_PROTOCOL        *DevicePath,
  OUT CHAR16                          **OsLoaderPathName
  )
{
  EFI_STATUS                          Status;
  EFI_DEVICE_PATH_PROTOCOL            *UpdatedDevicePath;
  EFI_HANDLE                          Handle;
  EFI_BLOCK_IO_PROTOCOL               *BlockIo;
  VOID                                *Buffer;
  EFI_DEVICE_PATH_PROTOCOL            *TempDevicePath;
  UINTN                               Size;
  UINTN                               TempSize;
  EFI_HANDLE                          ReturnHandle;
  EFI_HANDLE                          *SimpleFileSystemHandles;
  UINTN                               NumberSimpleFileSystemHandles;
  UINTN                               Index;
  UINTN                               LoaderIndex;
  UINTN                               LoaderCount;
  EFI_IMAGE_DOS_HEADER                DosHeader;
  EFI_IMAGE_OPTIONAL_HEADER_UNION     HdrData;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION Hdr;

  ReturnHandle      = NULL;
  UpdatedDevicePath = DevicePath;

  //
  // Check whether the device is connected
  //
  Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &UpdatedDevicePath, &Handle);
  if (EFI_ERROR (Status)) {
    //
    // Skip the case that the boot option point to a simple file protocol which does not consume block Io protocol,
    //
    Status = gBS->LocateDevicePath (&gEfiSimpleFileSystemProtocolGuid, &UpdatedDevicePath, &Handle);
    if (EFI_ERROR (Status)) {
      //
      // Fail to find the proper BlockIo and simple file protocol, maybe because device not present,  we need to connect it firstly
      //
      UpdatedDevicePath = DevicePath;
      Status            = gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &UpdatedDevicePath, &Handle);
      gBS->ConnectController (Handle, NULL, NULL, TRUE);
    }
  } else {
    //
    // For removable device boot option, its contained device path only point to the removable device handle, 
    // should make sure all its children handles (its child partion or media handles) are created and connected. 
    //
    gBS->ConnectController (Handle, NULL, NULL, TRUE); 
    //
    // Get BlockIo protocol and check removable attribute
    //
    Status = gBS->HandleProtocol (Handle, &gEfiBlockIoProtocolGuid, (VOID **)&BlockIo);
    //
    // Issue a dummy read to the device to check for media change.
    // When the removable media is changed, any Block IO read/write will
    // cause the BlockIo protocol be reinstalled and EFI_MEDIA_CHANGED is
    // returned. After the Block IO protocol is reinstalled, subsequent
    // Block IO read/write will success.
    //
    Buffer = AllocatePool (BlockIo->Media->BlockSize);
    if (Buffer != NULL) {
      BlockIo->ReadBlocks (
               BlockIo,
               BlockIo->Media->MediaId,
               0,
               BlockIo->Media->BlockSize,
               Buffer
               );
      FreePool(Buffer);
    }
  }

  //
  // Detect the the default boot file from removable Media
  //
  Size = GetDevicePathSize(DevicePath) - END_DEVICE_PATH_LENGTH;
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiSimpleFileSystemProtocolGuid,
         NULL,
         &NumberSimpleFileSystemHandles,
         &SimpleFileSystemHandles
         );
  for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
    //
    // Get the device path size of SimpleFileSystem handle
    //
    TempDevicePath = DevicePathFromHandle (SimpleFileSystemHandles[Index]);
    TempSize       = GetDevicePathSize (TempDevicePath)- END_DEVICE_PATH_LENGTH;
    //
    // Check whether the device path of boot option is part of the SimpleFileSystem handle's device path
    //
    if ((Size <= TempSize) && (CompareMem (TempDevicePath, DevicePath, Size) == 0)) {
      //
      // Load the default boot file \EFI\BOOT\boot{machinename}.EFI from removable Media
      //  machinename is ia32, ia64, x64, ...
      //
      Hdr.Union = &HdrData;

      LoaderCount = sizeof(gLoaderPathNameList)/sizeof(gLoaderPathNameList[0]);
      for(LoaderIndex=0;LoaderIndex<LoaderCount;LoaderIndex++){
        Status = GetImageHeader (
                   SimpleFileSystemHandles[Index],
                   gLoaderPathNameList[LoaderIndex],
                   &DosHeader,
                   Hdr
                   );
        if(!EFI_ERROR(Status)){
          break;
        }
      }
      
      if (!EFI_ERROR (Status) && 
          EFI_IMAGE_MACHINE_TYPE_SUPPORTED (Hdr.Pe32->FileHeader.Machine) &&
          Hdr.Pe32->OptionalHeader.Subsystem == EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION) {
        ReturnHandle = SimpleFileSystemHandles[Index];
        *OsLoaderPathName = gLoaderPathNameList[LoaderIndex];
        break;
      }
    }
  }

  if (SimpleFileSystemHandles != NULL) {
    FreePool(SimpleFileSystemHandles);
  }

  return ReturnHandle;
}

/**
  Get the image file buffer data and buffer size by its device path. 

  @param FilePath  On input, a pointer to an allocated buffer that contains the 
                   file device path.
                   On output the device path pointer could be modified to point to
                   a new allocated buffer that contains the full device path.
                   It could be caused by either short-form device path expanding,
                   or default boot file path appending.
  @param FileSize  A pointer to the size of the file buffer.

  @retval NULL   The file can't be found.
  @retval other  The file buffer. The caller is responsible to free memory.
**/
VOID *
LoadEfiBootOption (
  IN OUT EFI_DEVICE_PATH_PROTOCOL **FilePath,
  OUT    UINTN                    *FileSize
  )
{
  EFI_HANDLE                      Handle;
  VOID                            *FileBuffer;
  UINT32                          AuthenticationStatus;
  EFI_DEVICE_PATH_PROTOCOL        *Node;
  CHAR16                          *LoaderPathName;
  

  ASSERT ((FilePath != NULL) && (*FilePath != NULL) && (FileSize != NULL));

  EfiBootManagerConnectDevicePath (*FilePath, NULL);

  *FileSize  = 0;
  FileBuffer = NULL;
  //
  // Expand the short-form device path to full device path
  //
  if (1) {

    if ((DevicePathType (*FilePath) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (*FilePath) == MEDIA_HARDDRIVE_DP)) {
      //
      // Expand the Harddrive device path
      //
      ExpandPartitionShortFormDevicePath (FilePath);
      if (*FilePath == NULL) {
        return NULL;
      }

    } else {
      for (Node = *FilePath; !IsDevicePathEnd (Node); Node = NextDevicePathNode (Node)) {
        if ((DevicePathType (Node) == MESSAGING_DEVICE_PATH) &&
            ((DevicePathSubType (Node) == MSG_USB_CLASS_DP) ||
             (DevicePathSubType (Node) == MSG_USB_WWID_DP))) {
          break;
        }
      }

      if (!IsDevicePathEnd (Node)) {
        //
        // Expand the USB WWID/Class device path
        //
        FileBuffer = ExpandUsbShortFormDevicePath (FilePath, FileSize);
        if (FileBuffer == NULL) {
          return NULL;
        }
      }
    }
  }

  //
  // Fix up the boot option path if it points to a FV in memory map style of device path
  //
  if (IsMemmapFvFilePath (*FilePath)) {
    FixupMemmapFvFilePath (FilePath);
    if (*FilePath == NULL) {
      return NULL;
    }
  }

  if (FileBuffer == NULL) {
    FileBuffer = GetFileBufferByFilePath (TRUE, *FilePath, FileSize, &AuthenticationStatus);
  }

  //
  // If we didn't find an image directly, we need to try as if it is a removable device boot option
  // and load the image according to the default boot behavior.
  //
  if (FileBuffer == NULL) {
    //
    // check if there is a bootable media could be found in this device path,
    // and get the bootable media handle
    //
    Handle = GetBootableDeviceHandle (*FilePath, &LoaderPathName);
    if (Handle != NULL) {
      //
      // Load the default boot file \EFI\BOOT\boot{machinename}.EFI from the media
      //  machinename is ia32, ia64, x64, ...
      //
      FreeAndSet (FilePath, FileDevicePath (Handle, LoaderPathName));
      ASSERT (*FilePath != NULL);
      FileBuffer = GetFileBufferByFilePath (TRUE, *FilePath, FileSize, &AuthenticationStatus);
    }
  }

  if (FileBuffer == NULL) {
    FreeAndSet (FilePath, NULL);
  }

  return FileBuffer;
}





BOOLEAN
BmIsByoUiAppFilePath (
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
)
{
  EFI_HANDLE                      FvHandle;
  VOID                            *NameGuid;
  EFI_STATUS                      Status;

  Status = gBS->LocateDevicePath (&gEfiFirmwareVolume2ProtocolGuid, &DevicePath, &FvHandle);
  if (!EFI_ERROR (Status)) {
    NameGuid = EfiGetNameGuidFromFwVolDevicePathNode ((CONST MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) DevicePath);
    if (NameGuid != NULL) {
      return CompareGuid (NameGuid, &gSetupFileGuid);
    }
  }

  return FALSE;
}



/**
  Attempt to boot the EFI boot option. This routine sets L"BootCurent" and
  also signals the EFI ready to boot event. If the device path for the option
  starts with a BBS device path a legacy boot is attempted via the registered 
  gLegacyBoot function. Short form device paths are also supported via this 
  rountine. A device path starting with MEDIA_HARDDRIVE_DP, MSG_USB_WWID_DP,
  MSG_USB_CLASS_DP gets expaned out to find the first device that matches.
  If the BootOption Device Path fails the removable media boot algorithm 
  is attempted (\EFI\BOOTIA32.EFI, \EFI\BOOTX64.EFI,... only one file type 
  is tried per processor type)

  @param  BootOption    Boot Option to try and boot.
                        On return, BootOption->Status contains the boot status.
                        EFI_SUCCESS     BootOption was booted
                        EFI_UNSUPPORTED A BBS device path was found with no valid callback
                                        registered via EfiBootManagerInitialize().
                        EFI_NOT_FOUND   The BootOption was not found on the system
                        !EFI_SUCCESS    BootOption failed with this error status

**/
VOID
EFIAPI
ByoEfiBootManagerBoot (
  IN  EFI_BOOT_MANAGER_LOAD_OPTION             *BootOption
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                ImageHandle;
  EFI_LOADED_IMAGE_PROTOCOL *ImageInfo;
  EFI_DEVICE_PATH_PROTOCOL  *FilePath;
  VOID                      *FileBuffer;
  UINTN                     FileSize;
  BOOLEAN                   LoadOptionBoot;
  BOOLEAN                   BootSetup;
  EFI_BDS_BOOT_MANAGER_PROTOCOL  *BdsBootMgr;
  EFI_POST_LOGIN_PROTOCOL        *PostLogin;
  STATIC BOOLEAN                 ReadyToBootHook = FALSE;
  BYO_BM_BOOT_HOOK_PARAMETER     BootHookParam;
  
  
  if (BootOption == NULL) {
    return;
  }

  if (BootOption->FilePath == NULL || BootOption->OptionType != LoadOptionTypeBoot) {
    BootOption->Status = EFI_INVALID_PARAMETER;
    return;
  }

  LoadOptionBoot = (BOOLEAN)((BootOption->Attributes & LOAD_OPTION_CATEGORY) == LOAD_OPTION_CATEGORY_BOOT);
  if(LoadOptionBoot){
    BootHookParam.Sign = BYO_BM_BOOT_HOOK_PARAMETER_SIGN;
    BootHookParam.BootOption = BootOption;
    BootHookParam.Status = EFI_SUCCESS;
    InvokeHookProtocol2(gBS, &gByoBmBootHookProtocolGuid, &BootHookParam);
    if(BootHookParam.Status == EFI_ACCESS_DENIED){
      BootOption->Status = EFI_ACCESS_DENIED;
      DEBUG((EFI_D_INFO, "Boot Denied\n"));
      return;
    }
  }


  // 1. Create Boot#### for a temporary boot (i.e. a boot by selected a EFI Shell using "Boot From File")
  // 2. Update system variables: remove BootNext, set BootCurrent
  // 2.1. Remove BootNext
  // To prevent loops, the boot manager deletes BootNext before transferring control to the 
  // preselected boot option.
  //
  if (BootOption->BootNext) {
    Status = gRT->SetVariable (
                    L"BootNext",
                    &gEfiGlobalVariableGuid,
                    0,
                    0,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
  }
  //
  // 2.2. Set BootCurrent
  //
  Status = gRT->SetVariable (
                  L"BootCurrent",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (UINT16),
                  &BootOption->OptionNumber
                  );
  if(EFI_ERROR (Status)){
    DEBUG((EFI_D_ERROR, "Set BootCurrent:%r\n", Status));
  }

  BootSetup = FALSE;
  if (BmIsByoUiAppFilePath(BootOption->FilePath)) {
    BootSetup = TRUE;
  }

  Status = gBS->LocateProtocol(&gEfiPostLoginProtocolGuid, NULL, &PostLogin);
  if(!EFI_ERROR(Status)){
    PostLogin->CheckSysPassword(BootSetup);
  }
  
  if(BootSetup){
    Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, &BdsBootMgr);
    if(!EFI_ERROR(Status)){
      BdsBootMgr->BootSetup(BdsBootMgr);
    } 
    return;
  }

  //
  // 3. Signal the EVT_SIGNAL_READY_TO_BOOT event when we are about to load and execute
  //    the boot option.
  if (LoadOptionBoot) {
    REPORT_STATUS_CODE (
      EFI_PROGRESS_CODE, 
      (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_READY_TO_BOOT_EVENT)
      );
    DEBUG((EFI_D_INFO, "Signal ReadyToBoot\n"));
    if(!ReadyToBootHook){
      InvokeHookProtocol(gBS, &gEfiReadyToBootProtocolGuid);
      ReadyToBootHook = TRUE;
    }
    EfiSignalEventReadyToBoot();
    DEBUG((EFI_D_INFO, "ReadyToBootEnd\n"));    
  }

  if(BootOption->Legacy){
    if (BootOption->GroupType == BM_MENU_TYPE_LEGACY_PXE) {
      gST->ConOut->ClearScreen(gST->ConOut);
    }
  } else {
    if (BootOption->GroupType == BM_MENU_TYPE_UEFI_PXE) {
      gST->ConOut->ClearScreen(gST->ConOut);
    }
  }

  if(StrStr(BootOption->Description,L"Windows Boot Manager")!=NULL){
      gST->ConOut->ClearScreen(gST->ConOut);
  }
  //
  // 4. Load EFI boot option to ImageHandle
  //
  ImageHandle = NULL;
  if (DevicePathType (BootOption->FilePath) != BBS_DEVICE_PATH) {
    Status     = EFI_NOT_FOUND;
    FilePath   = DuplicateDevicePath (BootOption->FilePath);
    FileBuffer = LoadEfiBootOption (&FilePath, &FileSize);
    if (FileBuffer != NULL) {

      REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PcdGet32 (PcdProgressCodeOsLoaderLoad));

      Status = gBS->LoadImage (
                      TRUE,
                      gImageHandle,
                      FilePath,
                      FileBuffer,
                      FileSize,
                      &ImageHandle
                      );
      FreePool (FileBuffer);
      FreePool (FilePath);
    }

    if (EFI_ERROR (Status)) {
      REPORT_STATUS_CODE (
        EFI_ERROR_CODE, 
        (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_EC_BOOT_OPTION_LOAD_ERROR)
        );
      BootOption->Status = Status;

      if(Status == EFI_SECURITY_VIOLATION){
        InvokeHookProtocol(gBS, &gByoSecureBootViolationHookProtocolGuid);
      }
      
      return;
    }
  }

  //
  // 5. Adjust the different type memory page number just before booting
  //    and save the updated info into the variable for next boot to use
  //
  SetMemoryTypeInformationVariable(LoadOptionBoot);

  //
  // Record the performance data for End of BDS
  //
  PERF_END(NULL, "BDS", NULL, 0);

  PERF_CODE (
    WriteBootToOsPerformanceData ();
  );

  DEBUG_CODE_BEGIN();
    if (BootOption->Description == NULL) {
      DEBUG ((DEBUG_INFO | DEBUG_LOAD, "[Bds]Booting from unknown device path\n"));
    } else {
      DEBUG ((DEBUG_INFO | DEBUG_LOAD, "[Bds]Booting %s\n", BootOption->Description));
    }
  DEBUG_CODE_END();

  //
  // Check to see if we should legacy BOOT. If yes then do the legacy boot
  // Write boot to OS performance data for Legacy boot
  //
  if ((DevicePathType (BootOption->FilePath) == BBS_DEVICE_PATH) && (DevicePathSubType (BootOption->FilePath) == BBS_BBS_DP)) {
    if (mEfiBootManagerLegacyBoot != NULL) {
      mEfiBootManagerLegacyBoot (BootOption);
    } else {
      BootOption->Status = EFI_UNSUPPORTED;
    }
    return;
  }
 
  //
  // Provide the image with its load options
  //
  Status = gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **) &ImageInfo);
  ASSERT_EFI_ERROR (Status);

  ImageInfo->LoadOptionsSize  = BootOption->OptionalDataSize;
  ImageInfo->LoadOptions      = BootOption->OptionalData;
  //
  // Before calling the image, enable the Watchdog Timer for 5 minutes period
  //
  gBS->SetWatchdogTimer (5 * 60, 0x0000, 0x00, NULL);

  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PcdGet32 (PcdProgressCodeOsLoaderStart));

  Status = gBS->StartImage (ImageHandle, &BootOption->ExitDataSize, &BootOption->ExitData);
  DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Image Return Status = %r\n", Status));

  if (EFI_ERROR (Status)) {
    REPORT_STATUS_CODE (
      EFI_ERROR_CODE, 
      (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_EC_BOOT_OPTION_FAILED)
      );
  }

  //
  // Clear the Watchdog Timer after the image returns
  //
  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);

  //
  // Clear Boot Current
  //
  gRT->SetVariable (
        L"BootCurrent",
        &gEfiGlobalVariableGuid,
        0,
        0,
        NULL
        );

  if (BootOption->GroupType == BM_MENU_TYPE_UEFI_OTHERS ||
  	BootOption->GroupType == BM_MENU_TYPE_UEFI_USK_DISK ) {
    gST->ConOut->ClearScreen(gST->ConOut);
  }

  BootOption->Status = Status;
}


/**
  Check whether there is a instance in BlockIoDevicePath, which contain multi device path
  instances, has the same partition node with HardDriveDevicePath device path

  @param  BlockIoDevicePath      Multi device path instances which need to check
  @param  HardDriveDevicePath    A device path which starts with a hard drive media
                                 device path.

  @retval TRUE                   There is a matched device path instance.
  @retval FALSE                  There is no matched device path instance.

**/
BOOLEAN
MatchPartitionDevicePathNode (
  IN  EFI_DEVICE_PATH_PROTOCOL   *BlockIoDevicePath,
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  )
{
  HARDDRIVE_DEVICE_PATH     *TmpHdPath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BOOLEAN                   Match;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoHdDevicePathNode;

  if ((BlockIoDevicePath == NULL) || (HardDriveDevicePath == NULL)) {
    return FALSE;
  }

  //
  // Make PreviousDevicePath == the device path node before the end node
  //
  DevicePath              = BlockIoDevicePath;
  BlockIoHdDevicePathNode = NULL;

  //
  // find the partition device path node
  //
  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (DevicePath) == MEDIA_HARDDRIVE_DP)
        ) {
      BlockIoHdDevicePathNode = DevicePath;
      break;
    }

    DevicePath = NextDevicePathNode (DevicePath);
  }

  if (BlockIoHdDevicePathNode == NULL) {
    return FALSE;
  }
  //
  // See if the harddrive device path in blockio matches the orig Hard Drive Node
  //
  TmpHdPath = (HARDDRIVE_DEVICE_PATH *) BlockIoHdDevicePathNode;
  Match = FALSE;

  //
  // Check for the match
  //
  if ((TmpHdPath->MBRType == HardDriveDevicePath->MBRType) &&
      (TmpHdPath->SignatureType == HardDriveDevicePath->SignatureType)) {
    switch (TmpHdPath->SignatureType) {
    case SIGNATURE_TYPE_GUID:
      Match = CompareGuid ((EFI_GUID *)TmpHdPath->Signature, (EFI_GUID *)HardDriveDevicePath->Signature);
      break;
    case SIGNATURE_TYPE_MBR:
      Match = (BOOLEAN) (*((UINT32 *) (&(TmpHdPath->Signature[0]))) == ReadUnaligned32((UINT32 *)(&(HardDriveDevicePath->Signature[0]))));
      break;
    default:
      Match = FALSE;
      break;
    }
  }

  return Match;
}



EFI_STATUS 
GetBiosEfiFileFvHandleInAllFv(
  IN  EFI_GUID      *FfsGuid,
  OUT EFI_HANDLE    *FvHandle
  )
{
  UINTN                              FvHandleCount;
  EFI_HANDLE                         *FvHandleBuffer = NULL;  
  EFI_STATUS                         Status;
  UINTN                              Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL      *Fv2;
  UINT32                             AuthenticationStatus;
  VOID                               *Buffer;
  UINTN                              Size;


  ASSERT(FfsGuid != NULL && FvHandle != NULL);
  *FvHandle = NULL;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &FvHandleCount,
                  &FvHandleBuffer
                  );
  if(EFI_ERROR(Status) || FvHandleCount==0){
    goto ProcExit;
  }
  
  for (Index = 0; Index < FvHandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    FvHandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID**)&Fv2
                    );
    ASSERT(!EFI_ERROR(Status));

    Buffer  = NULL;
    Size    = 0;
    Status  = Fv2->ReadSection (
                    Fv2,
                    FfsGuid,
                    EFI_SECTION_PE32,
                    0,
                    &Buffer,
                    &Size,
                    &AuthenticationStatus
                    );
    if(!EFI_ERROR(Status)){
      if (Buffer != NULL) {
        FreePool(Buffer);
      }
      *FvHandle = FvHandleBuffer[Index];
      break;
    }

  }

ProcExit:

  if (FvHandleBuffer != NULL) {
    gBS->FreePool(FvHandleBuffer);
  }  
  if(*FvHandle == NULL){
    return EFI_NOT_FOUND;
  } else {
    return EFI_SUCCESS;
  }
}



EFI_STATUS
GetInternalShellDevPath (
    EFI_DEVICE_PATH_PROTOCOL **DevicePath
  )
{
  EFI_STATUS                         Status;
  EFI_DEVICE_PATH_PROTOCOL           *ShellDP;
  EFI_HANDLE                         FvHandle;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH  FileNode;
	EFI_GUID                           *ShellGuid;


  ASSERT(DevicePath != NULL);

  ShellGuid = (EFI_GUID*)PcdGetPtr(PcdShellFile);
  EfiInitializeFwVolDevicepathNode(&FileNode, ShellGuid);

  Status = GetBiosEfiFileFvHandleInAllFv(ShellGuid, &FvHandle);
  if(EFI_ERROR(Status)){
    return Status;
  }

  ShellDP = AppendDevicePathNode (
              DevicePathFromHandle(FvHandle),
              (EFI_DEVICE_PATH_PROTOCOL*)&FileNode
              );
  if(ShellDP == NULL){
    Status = EFI_OUT_OF_RESOURCES;
  } else {
    *DevicePath = ShellDP;
  }

  return Status;
}



EFI_STATUS 
AddShellBootOption(
  EFI_BOOT_MANAGER_LOAD_OPTION **BootOptions,
  UINTN                        *BootOptionCount
  )
{
  EFI_STATUS                 Status;
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;


  if(!(PcdGet32(PcdSystemMiscConfig) & SYS_MISC_CFG_SHELL_EN)){
    return EFI_UNSUPPORTED;
  }
    
  Status = GetInternalShellDevPath(&DevicePath);
  if(!EFI_ERROR(Status)){
    *BootOptions = ReallocatePool (
                    sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*BootOptionCount),
                    sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*BootOptionCount + 1),
                    *BootOptions
                    );
    ASSERT (*BootOptions != NULL);

    Status = EfiBootManagerInitializeLoadOption (
               &(*BootOptions)[(*BootOptionCount)++],
               LoadOptionNumberUnassigned,
               LoadOptionTypeBoot,
               LOAD_OPTION_ACTIVE,
               L"Internal EDK Shell",
               DevicePath,
               NULL,
               0
               );
    ASSERT_EFI_ERROR (Status);
		
    FreePool(DevicePath);
  }		

  return Status;
}



UINT8 gBlockIoTypeArray[] = { MSG_SATA_DP, MSG_NVME_NAMESPACE_DP, MSG_ATAPI_DP, 0xFF};
UINTN gBlockIoTypeArraySize = ARRAY_SIZE(gBlockIoTypeArray);



/**
  Emuerate all possible bootable medias in the following order:
  1. Removable BlockIo            - The boot option only points to the removable media
                                    device, like USB key, DVD, Floppy etc.
  2. Fixed BlockIo                - The boot option only points to a Fixed blockIo device,
                                    like HardDisk.
  3. Non-BlockIo SimpleFileSystem - The boot option points to a device supporting
                                    SimpleFileSystem Protocol, but not supporting BlockIo
                                    protocol.
  4. LoadFile                     - The boot option points to the media supporting 
                                    LoadFile protocol.
  Reference: UEFI Spec chapter 3.3 Boot Option Variables Default Boot Behavior

  TODO: Do we need to add EfiBootManagerConnectAll() in this function? Impact to SLE platform is big.
**/
EFI_BOOT_MANAGER_LOAD_OPTION *
EFIAPI
EfiBootManagerEnumerateBootOptions (
  UINTN                                 *BootOptionCount
  )
{
  EFI_STATUS                            Status;
  EFI_BOOT_MANAGER_LOAD_OPTION          *BootOptions;
  UINT16                                NonBlockNumber;
  UINTN                                 HandleCount;
  EFI_HANDLE                            *Handles;
  EFI_BLOCK_IO_PROTOCOL                 *BlkIo;
  UINTN                                 Index;
  UINTN                                 FunctionIndex;
  CHAR16                                *Temp;
  CHAR16                                *DescriptionPtr;
  CHAR16                                Description[60];
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  CHAR16                                *DevStr;
  CHAR16                                *BootTypeStr;
  CHAR16                                *IpTypeStr;
  UINT8                                 IpType;
  CONST VOID                            *OptData;
  UINTN                                 OptDataSize;
  UINT8                                 NetBootIp4Ip6;
  PLAT_HOST_INFO_PROTOCOL               *HostInfo;  
  EFI_DEVICE_PATH_PROTOCOL              *Dp;
  UINT8                                 *BlockIoRec = NULL;
  UINTN                                 TypeIndex;
  UINT8                                 DeviceType;
  BOOLEAN                               NoUsbBoot;  
  BOOLEAN                               bePxeBootEnable=TRUE;


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

  for (TypeIndex = 0; TypeIndex < ARRAY_SIZE(gBlockIoTypeArray); TypeIndex++){
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
      for (FunctionIndex = 0; FunctionIndex < sizeof (mGetBootDescription) / sizeof (mGetBootDescription[0]); FunctionIndex++) {
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
      Temp = StrCat (
               StrCpy (
                 AllocatePool (StrSize (DescriptionPtr) + sizeof (mUefiPrefix)),
                 mUefiPrefix
                 ),
               DescriptionPtr
               );
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
      DevStr = GetEfiNetWorkString(DevicePath,&bePxeBootEnable);
    }
    //DEBUG((EFI_D_ERROR, __FUNCTION__"(),bePxeBootEnable :%d.\n", bePxeBootEnable));
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

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &HostInfo);
  if(!EFI_ERROR(Status) && HostInfo->UpdateBootOption != NULL){
    HostInfo->UpdateBootOption(&BootOptions, BootOptionCount);
  }

  return BootOptions;
}





VOID RemoveReduplicateBootOption(VOID)
{
  UINT16                       *BootOrder = NULL;
  UINTN                        BootOrderSize;
  UINTN                        BootOrderCount = 0;
  UINTN                        Index;
  UINTN                        CmpIndex;
  CHAR16                       OptionName[8+1];
  VOID                         **BootOption = NULL;
  UINTN                        *BootOptionSize = NULL;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  BootOrder = EfiBootManagerGetVariableAndSize (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  &BootOrderSize
                  );
  if (BootOrder == NULL) {
    goto ProcExit;
  }

  BootOrderCount = BootOrderSize/sizeof(UINT16);
  BootOption = AllocateZeroPool(BootOrderCount * sizeof(VOID*));
  if(BootOption == NULL){
    goto ProcExit;
  }
  BootOptionSize = AllocateZeroPool(BootOrderCount * sizeof(UINTN));
  if(BootOption == NULL){
    goto ProcExit;
  }  

	for(Index=0; Index<BootOrderCount; Index++){
    UnicodeSPrint(OptionName, sizeof(OptionName), L"Boot%04X", BootOrder[Index]);
    BootOption[Index] = EfiBootManagerGetVariableAndSize (
                          OptionName,
                          &gEfiGlobalVariableGuid,
                          &BootOptionSize[Index]
                          );
    if(BootOption[Index] == NULL){
      goto ProcExit;
    }
  }

  for(Index=0;Index<BootOrderCount;Index++){
    for(CmpIndex=Index+1;CmpIndex<BootOrderCount;CmpIndex++){
      if(BootOptionSize[Index] != BootOptionSize[CmpIndex]){
        continue;
      }
      if(CompareMem(BootOption[Index], BootOption[CmpIndex], BootOptionSize[Index]) == 0){
        EfiBootManagerDeleteLoadOptionVariable(BootOrder[CmpIndex], LoadOptionTypeBoot);  
        DEBUG((EFI_D_INFO, "delete boot order [%d]:%d\n", CmpIndex, BootOrder[CmpIndex]));
      }
    }
  }



ProcExit:
  if(BootOrder != NULL){FreePool(BootOrder);}
  if(BootOption != NULL){
    for(Index=0; Index<BootOrderCount; Index++){
      if(BootOption[Index] != NULL){
        FreePool(BootOption[Index]);
      }
    }    
    FreePool(BootOption);
  }
  if(BootOptionSize != NULL){FreePool(BootOptionSize);} 
  return;
}





BOOLEAN
IsValidOsShortDp (
  EFI_BOOT_MANAGER_LOAD_OPTION  *NvBootOption
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *NvDp;
  EFI_DEVICE_PATH_PROTOCOL      *FsDp = NULL;
  EFI_DEVICE_PATH_PROTOCOL      *TempDp;
  EFI_STATUS                    Status;
  UINTN                         HandleCount;
  EFI_HANDLE                    *Handles = NULL;
  UINTN                         Index;
  CHAR16                        *PathName;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Fs;
  EFI_FILE_PROTOCOL                *RootFile;
  BOOLEAN                          DpValid = FALSE;
  

  NvDp = NvBootOption->FilePath;
  if (!(DevicePathType(NvDp) == MEDIA_DEVICE_PATH && DevicePathSubType(NvDp) == MEDIA_HARDDRIVE_DP)) {
    return DpValid;
  }

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
    
    if(CompareMem(NvDp, TempDp, DevicePathNodeLength(NvDp)) != 0){
      continue;
    }

    DEBUG((EFI_D_INFO, "target HD found\n"));
    TempDp = NextDevicePathNode(TempDp);
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

  if(Handles != NULL){
    FreePool(Handles);
  }
  
  return DpValid;
}





VOID
EFIAPI
ByoBmRefreshUefiBootOption1 (
  VOID
  )
{
  EFI_BOOT_MANAGER_LOAD_OPTION  *NvBootOptions;
  UINTN                         NvBootOptionCount;
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions;
  UINTN                         BootOptionCount;
  UINTN                         Index;
  CONST VOID                    *OptData;
  UINTN                         OptDataSize;


// Uefi
  if(PcdGet8(PcdBiosBootModeType) == BIOS_BOOT_LEGACY_OS){
    EfiBootManagerDeleteAllUefiBootOption();
    return;		
  }		

  BootOptions   = EfiBootManagerEnumerateBootOptions (&BootOptionCount);
  NvBootOptions = EfiBootManagerGetLoadOptions (&NvBootOptionCount, LoadOptionTypeBoot);

  OptData = PcdGetPtr(PcdEfiBootOptionFlag);
  OptDataSize = PcdGetSize(PcdEfiBootOptionFlag);

  //
  // Remove invalid EFI boot options from NV
  //
  for (Index = 0; Index < NvBootOptionCount; Index++) {
    if (DevicePathType(NvBootOptions[Index].FilePath) == BBS_DEVICE_PATH && 
        DevicePathSubType(NvBootOptions[Index].FilePath) == BBS_BBS_DP) {
      continue;
    }				

    if (EfiBootManagerFindLoadOption (&NvBootOptions[Index], BootOptions, BootOptionCount) == (UINTN) -1) {
      if(IsValidOsShortDp(&NvBootOptions[Index])){
        continue;
      }
      EfiBootManagerDeleteLoadOptionVariable (NvBootOptions[Index].OptionNumber, LoadOptionTypeBoot);
    }
  }

  //
  // Add new EFI boot options to NV
  //
  for (Index = 0; Index < BootOptionCount; Index++) {
  	DEBUG((EFI_D_INFO,"ByoBmRefreshUefiBootOption1 bootoption type = %0x\n",BootOptions[Index].GroupType));
    if (EfiBootManagerFindLoadOption (&BootOptions[Index], NvBootOptions, NvBootOptionCount) == (UINTN) -1) {
      EfiBootManagerAddLoadOptionVariable (&BootOptions[Index], (UINTN) -1);
    }
  }

  ByoEfiBootManagerFreeLoadOptions (BootOptions,   BootOptionCount);
  ByoEfiBootManagerFreeLoadOptions (NvBootOptions, NvBootOptionCount);
  
}


/**
  For a bootable Device path, return its boot type.

  @param  DevicePath                   The bootable device Path to check

  @retval AcpiFloppyBoot               If given device path contains ACPI_DEVICE_PATH type device path node
                                       which HID is floppy device.
  @retval MessageAtapiBoot             If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_ATAPI_DP.
  @retval MessageSataBoot              If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_SATA_DP.
  @retval MessageScsiBoot              If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_SCSI_DP.
  @retval MessageUsbBoot               If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_USB_DP.
  @retval MessageNetworkBoot           If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_MAC_ADDR_DP, MSG_VLAN_DP,
                                       MSG_IPv4_DP or MSG_IPv6_DP.
  @retval UnsupportedBoot              If tiven device path doesn't match the above condition, it's not supported.

**/
BOOT_TYPE
BootTypeFromDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *Node;
  EFI_DEVICE_PATH_PROTOCOL      *LastDeviceNode;

  ASSERT (DevicePath != NULL);

  for (Node = DevicePath; !IsDevicePathEndType (Node); Node = NextDevicePathNode (Node)) {
    switch (DevicePathType (Node)) {

      case ACPI_DEVICE_PATH:
        if (EISA_ID_TO_NUM (((ACPI_HID_DEVICE_PATH *) Node)->HID) == 0x0604) {
          return AcpiFloppyBoot;
        }
        break;

      case MESSAGING_DEVICE_PATH:
        //
        // Skip LUN device node
        //
        LastDeviceNode=Node;
        while (DevicePathSubType(LastDeviceNode) == MSG_DEVICE_LOGICAL_UNIT_DP){
                LastDeviceNode = NextDevicePathNode (LastDeviceNode); 
        }

        //
        // if the device path not only point to driver device, it is not a messaging device path,
        //
        if (!IsDevicePathEndType (LastDeviceNode)) {
          break;
        }

        switch (DevicePathSubType (Node)) {
        case MSG_ATAPI_DP:
          return MessageAtapiBoot;
          break;

        case MSG_SATA_DP:
          return MessageSataBoot;
          break;

        case MSG_USB_DP:
          return MessageUsbBoot;
          break;

        case MSG_SCSI_DP:
          return MessageScsiBoot;
          break;

        case MSG_MAC_ADDR_DP:
        case MSG_VLAN_DP:
        case MSG_IPv4_DP:
        case MSG_IPv6_DP:
          return MessageNetworkBoot;
          break;
        }
    }
  }

  return UnsupportedBoot;
}

BOOT_DEVICE_TYPE
GetDeviceTypeFromPath (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN EFI_GUID                     *Protocol,
  IN OUT EFI_HANDLE               *Handle
)
{
  BOOT_DEVICE_TYPE                DeviceType;
  EFI_HANDLE                      *HandleBuffer;
  UINTN                           NumberOfHandles;
  EFI_DEVICE_PATH_PROTOCOL        *Next;
  UINTN                           Size;
  UINTN                           Index;
  EFI_DEVICE_PATH_PROTOCOL        *TempDevicePath;
  UINTN                           TempSize;
  EFI_BLOCK_IO_PROTOCOL           *BlockIo;

  TempDevicePath = DevicePath;
  Next = GetNextDevicePathInstance (&TempDevicePath, &TempSize);
  while (!IsDevicePathEndType (Next)) {
    if ((Next->Type == MESSAGING_DEVICE_PATH) && (Next->SubType == MSG_MAC_ADDR_DP)) {
      return LanBoot;
    }
    Next = NextDevicePathNode (Next);
  }

  DeviceType = UnknowBoot;
  Size = GetDevicePathSize (DevicePath) - END_DEVICE_PATH_LENGTH;
  HandleBuffer = NULL;
  gBS->LocateHandleBuffer (
         ByProtocol,
         Protocol,
         NULL,
         &NumberOfHandles,
         &HandleBuffer
         );
  for (Index = 0; Index < NumberOfHandles; Index++) {
    //
    // Get the device path size of SimpleFileSystem handle
    //
    *Handle = HandleBuffer[Index];
    TempDevicePath = DevicePathFromHandle (HandleBuffer[Index]);
    // DEBUG ((DEBUG_INFO, "TempDevicePath:"));
    // BmPrintDp (TempDevicePath);
    // DEBUG ((DEBUG_INFO, "\n"));

    TempSize = GetDevicePathSize (TempDevicePath) - END_DEVICE_PATH_LENGTH;
    //
    // Check whether the device path of boot option is part of the SimpleFileSystem handle's device path
    //
    if ((Size <= TempSize) && (CompareMem (TempDevicePath, DevicePath, Size) == 0)) {
      gBS->HandleProtocol (HandleBuffer[Index], &gEfiBlockIoProtocolGuid, (VOID **) &BlockIo);
      Next = GetNextDevicePathInstance (&TempDevicePath, &TempSize);
      while (!IsDevicePathEndType (Next)) {
        if ((Next->Type == MESSAGING_DEVICE_PATH) && (Next->SubType == MSG_SATA_DP)) {
          if (BlockIo->Media->BlockSize == 0x800) {
            DeviceType = CdromBoot;
            goto Exit;
          } else {
            DeviceType = SataHddBoot;
            goto Exit;
          }
        }
        if ((Next->Type == MESSAGING_DEVICE_PATH) && (Next->SubType == MSG_USB_DP)) {
          if (BlockIo->Media->BlockSize == 0x200) {
            DeviceType = UsbDiskBoot;
          } else {
            DeviceType = CdromBoot;
          }
          goto Exit;
        }
        if ((Next->Type == MESSAGING_DEVICE_PATH) && (Next->SubType == MSG_NVME_NAMESPACE_DP)) {
          DeviceType = NvmeBoot;
          goto Exit;
        }
        
        Next = NextDevicePathNode (Next);
      }
    }
  }

Exit:
  if(HandleBuffer != NULL){
    FreePool(HandleBuffer);
  }

  return DeviceType;
}


VOID
ByoBmRefreshUefiBootOption2 (
  VOID
  );


VOID
EFIAPI
ByoEfiBootManagerRefreshAllBootOption (
  VOID
  )
{
  UINT32                        SystemMiscCfg;
  UINT8                         BootOrderMethod;


  SystemMiscCfg = PcdGet32(PcdSystemMiscConfig);

  if(SystemMiscCfg & SYS_MISC_CFG_UPD_BOOTORDER){
    return;
  }
  PcdSet32(PcdSystemMiscConfig, SystemMiscCfg | SYS_MISC_CFG_UPD_BOOTORDER);

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  
  CheckPxeNicVar ();

  if (mEfiBootManagerRefreshLegacyBootOption != NULL) {
    mEfiBootManagerRefreshLegacyBootOption();
  }

  BootOrderMethod = PcdGet8(PcdBdsBootOrderUpdateMethod);
  if(BootOrderMethod == 1){
    ByoBmRefreshUefiBootOption1();
  } else if(BootOrderMethod == 2){
    ByoBmRefreshUefiBootOption2();
  }

  RemoveReduplicateBootOption();
  
  InvokeHookProtocol(gBS, &gByoBmRefreshAllBootOptionEndHookProtocolGuid);

}


VOID
GetType11String (
  IN  CHAR8 *FirstHddStr,
  IN  CHAR8 *BootOrderStr
)
{
  EFI_BOOT_MANAGER_LOAD_OPTION    *LoadOptions;
  UINTN                           LoadOptionCount;
  UINTN                           Index;
  UINT8                           Index1 = 0;
  UINT8                           Data;
  BOOLEAN                         HddFound = FALSE;
  BOOT_TYPE                       DeviceType;
  EFI_HANDLE                      Handle;
  UINTN                           Index2;


  LoadOptions = EfiBootManagerGetLoadOptions (&LoadOptionCount, LoadOptionTypeBoot);
  for (Index = 0; Index < LoadOptionCount; Index++) {
    if (LoadOptions[Index].Attributes != LOAD_OPTION_ACTIVE) {
      continue;
    }
    DeviceType = GetDeviceTypeFromPath (LoadOptions[Index].FilePath, &gEfiBlockIoProtocolGuid, &Handle);
    switch (DeviceType) {
      case SataHddBoot:
	  case NvmeBoot:
        Data = 0x31; // "1"
        break;
      case UsbDiskBoot:
        Data = 0x34; // "4"
        break;
      case CdromBoot:
        Data = 0x33; // "3"
        break;
      case LanBoot:
        Data = 0x32; // "2"
        break;
      default:
        Data = 0;
        break;
    }

    for(Index2=0;Index2<(ARRAY_SIZE(mBdsGeneralUefiBootOs)-1);Index2++){
       if(StrStr(LoadOptions[Index].Description,mBdsGeneralUefiBootOs[Index2].Description)!=NULL){
         if(!HddFound){
           HddFound = TRUE;
           UnicodeStrToAsciiStr(LoadOptions[Index].Description, FirstHddStr);
         }
         Data = 0x31; // "1"
       }
    }
	if (Data == 0) {
      continue;
    }
    BootOrderStr[Index1++] = Data;
    if (!HddFound && (DeviceType == SataHddBoot || DeviceType == NvmeBoot)) {
      HddFound = TRUE;
      UnicodeStrToAsciiStr(LoadOptions[Index].Description, FirstHddStr);
    }
  }
  ByoEfiBootManagerFreeLoadOptions (LoadOptions, LoadOptionCount);
}


STATIC
EFI_STATUS
AddSmbiosRecord (
	IN EFI_SMBIOS_PROTOCOL		  *Smbios,
	OUT EFI_SMBIOS_HANDLE		  *SmbiosHandle,
	IN EFI_SMBIOS_TABLE_HEADER	  *Record
    )
{
		*SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
		return Smbios->Add (
						 Smbios,
						 NULL,
						 SmbiosHandle,
						 Record
						 );
}

VOID
UpdateSmbiosType11 ()
{
  EFI_STATUS                  Status;
  EFI_SMBIOS_PROTOCOL         *SmbiosProtocol;
  UINT16                      TypeNo;
  UINTN                       StringNo;
  EFI_SMBIOS_HANDLE           SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER     *Record;
  CHAR8                       BootOrderStr[SMBIOS_STRING_MAX_LENGTH];
  CHAR8                       FirstHddStr[SMBIOS_STRING_MAX_LENGTH];
  CHAR16              BmcFwVersion[20];
  UINT16                                FwVersion;
  IPMI_INTERFACE_PROTOCOL               *IpmiInterface = NULL;
    CHAR8                       *BmcVer = NULL;

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID**)&SmbiosProtocol);
  ASSERT_EFI_ERROR(Status);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  TypeNo       = EFI_SMBIOS_TYPE_OEM_STRINGS;
  Status = SmbiosProtocol->GetNext (
                             SmbiosProtocol, 
                             &SmbiosHandle, 
                             (EFI_SMBIOS_TYPE *)&TypeNo, 
                             &Record, 
                             NULL
                             );
  if (EFI_ERROR(Status)) {
    return;
  }

  ZeroMem(BootOrderStr, SMBIOS_STRING_MAX_LENGTH);
  ZeroMem(FirstHddStr, SMBIOS_STRING_MAX_LENGTH);
  GetType11String (FirstHddStr, BootOrderStr);  // boot order (HDD 1, PXE 2, CDROM 3)

  
  //
  //  Update smbios type 11 bmc version
  //
  Status = gBS->LocateProtocol (
              &gByoIpmiInterfaceProtocolGuid,
              NULL,
              (VOID **) &IpmiInterface
              );
  if (IpmiInterface == NULL) {
    return;
  }

  ZeroMem (BmcFwVersion, sizeof (BmcFwVersion));
  FwVersion = IpmiInterface->GetBmcFirmwareVersion (IpmiInterface);  

  BmcVer = AllocatePool (20);
  #if BC_TENCENT
  AsciiSPrint(BmcVer, 20, "BMC version:%d.%02x.%02d", FwVersion >> 12, (FwVersion >> 4) & 0xff, FwVersion & 0xf);
  #else
  AsciiSPrint(BmcVer, 20, "BMC version:%d.%x.%d", FwVersion >> 12, (FwVersion >> 4) & 0xff, FwVersion & 0xf);
  #endif 
  StringNo = 4;
  SmbiosProtocol->UpdateString (
                      SmbiosProtocol,
                      &SmbiosHandle,
                      &StringNo,
                      BmcVer
                      );

  if (AsciiStrLen (BootOrderStr)) {
    StringNo = 5;
    SmbiosProtocol->UpdateString (
                      SmbiosProtocol,
                      &SmbiosHandle,
                      &StringNo,
                      BootOrderStr
                      );
  }
  // First Hdd string
  if (AsciiStrLen (FirstHddStr)) {
    StringNo = 6;
    SmbiosProtocol->UpdateString (
                      SmbiosProtocol,
                      &SmbiosHandle,
                      &StringNo,
                      FirstHddStr
                      );
  }

}


EFI_STATUS AddSmbiosType24()
{
  EFI_SMBIOS_PROTOCOL    *Smbios;
  EFI_STATUS				 Status;
  EFI_SMBIOS_HANDLE		 SmbiosHandle;
  SMBIOS_TABLE_TYPE24 	 *SmbiosRecord;
  UINT32                 Attributes;	
  UINTN                  VarSize =0;
  SYSTEM_PASSWORD	     gTse;
  
  EFI_GUID                 gEfiSystemPasswordVariableGuid = SYSTEM_PASSWORD_GUID;

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  DEBUG((EFI_D_INFO,"locate protocol : status = %r\n",Status));

  VarSize = sizeof(SYSTEM_PASSWORD);
  Status = gRT->GetVariable (
	            L"SysPd", 
	            &gEfiSystemPasswordVariableGuid, 
	            &Attributes, 
	            &VarSize, 
	            &gTse
	            );

  DEBUG((EFI_D_INFO,"Get variable : status = %r\n",Status));
  DEBUG((EFI_D_INFO,"Get variable : status = %r, Guid = %g, Attributes = %d, varsize = %d\n",Status));
  DEBUG((EFI_D_INFO,"Get variable : gTse adminpassword present = %x\n",gTse.bHaveAdmin));
  SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE24));
  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_HARDWARE_SECURITY;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE24);
  SmbiosRecord->Hdr.Handle = 0;
  SmbiosRecord->HardwareSecuritySettings = 0x22;

  if((gTse.bHaveAdmin!= 0 )|| (gTse.bHavePowerOn!= 0)){
    SmbiosRecord->HardwareSecuritySettings |= (1 << 6);
  }
  if(gTse.bHaveAdmin != 0){
    SmbiosRecord->HardwareSecuritySettings |= (1 << 2);
  }
  Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

  FreePool(SmbiosRecord);
  return Status;

}


#define BMC_16BYTE                   16

EFI_STATUS AddSmbiosType39()
{
  UINT8                 Index;
  UINT8                 i = 0;
  EFI_STATUS            Status = 0;
  UINTN                 TableSize = 0;
  UINT8                 ResponseData[7*BMC_16BYTE];
  UINT8                 ResponseDataSize=0;
  UINTN                 CommandDatasize = 2;
  UINT8                 CommandData[2];
  SMBIOS_TABLE_TYPE39   *Type39Record = NULL;
  EFI_STRING            AssetTagNumStr = NULL;
  EFI_STRING            ModelPartNumStr = NULL;
  UINTN                 AssetTagNumStrLen = 0;
  UINTN                 ModelPartNumStrLen = 0;
  UINTN                 MaxPower = 0;
  UINT8                 *Response = NULL;
  CHAR8                 *OptionalStrStart = NULL;
  EFI_STRING            ManufacturerStr = NULL;
  UINTN                 ManufaturerStrLen = 0;
  EFI_STRING            SerialNumStr = NULL;
  UINTN                 SerialNumStrLen = 0;
  EFI_STRING            RevisionLevelStr = NULL;
  UINTN                 RevisionLevelStrLen = 0;
  EFI_STRING            LocationStr = NULL;
  UINTN                 LocationStrLen = 0;
  EFI_STRING            DeviceNameStr = NULL;
  UINTN                 DeviceNameStrLen = 0;
  EFI_STRING            MaxPowerCapacityStr = NULL;
  UINTN                 MaxPowerCapacityStrLen = 0;
  UINT16                PowerSupplyPresentFlag = 0;
  EFI_STRING            PowerSupplyPresentTemp = NULL;
  EFI_SMBIOS_PROTOCOL   *Smbios;
  //EFI_IPMI_TRANSPORT    *gIpmiTransport = NULL;
  EFI_SMBIOS_HANDLE     PowerSupplyHandle;

   EfiInitializeIpmiBase ();
  
  ResponseDataSize = sizeof (ResponseData);
  DEBUG((DEBUG_INFO,"entry type39 callback\n"));
 // Status = gBS->LocateProtocol (
 //             &gEfiIpmiTransportProtocolGuid,
 //             NULL,
 //             &gIpmiTransport
 //             );
  if (!EFI_ERROR (Status))
  {
   for(Index=0;Index<2;Index++)
   {
     CommandData[0]=0x08;
     CommandData[1]=Index;
     Status = EfiSendCommandToBMC(
                             0x3E,
                             0x80,
                             (UINT8 *)&CommandData, 
                             CommandDatasize,
                             (UINT8*)&ResponseData,
                             &ResponseDataSize
                            );
     if (EFI_ERROR (Status))
     {
       DEBUG((DEBUG_INFO,"Get PUS%d info failed: %r\n",Index,Status));
       continue;
     }else{
       DEBUG((DEBUG_INFO,"ResponseDataSize:0x%02x \n", ResponseDataSize));
       DEBUG((DEBUG_INFO,"ResponseData:\n"));     
      for(i=0;i<ResponseDataSize;i++)
      {
        DEBUG((DEBUG_INFO,"0x%02x ", ResponseData[i]));
      }
     DEBUG ((DEBUG_INFO,"\n")); 
     }


   //get Manufacturer String
    ManufacturerStr = AllocatePool(2*BMC_16BYTE);
    ZeroMem(ManufacturerStr,2*BMC_16BYTE);
    ASSERT (ManufacturerStr != NULL);
    for(i=0;(i<16)&&(ResponseData[i] != 0);i++)
    {
      ManufacturerStr[i] = ResponseData[i];
    }
     ManufaturerStrLen = StrLen(ManufacturerStr);
     DEBUG((DEBUG_INFO,"ManufacturerStr:%s \n", ManufacturerStr));


   //get ModelPart Num String
    ModelPartNumStr = AllocatePool(2*BMC_16BYTE);
    ZeroMem(ModelPartNumStr,2*BMC_16BYTE);
    ASSERT (ModelPartNumStr != NULL);
     for(i=16;(i<32)&&(ResponseData[i] != 0);i++)
     {
       ModelPartNumStr[i-16] = ResponseData[i];
     }
    ModelPartNumStrLen = StrLen(ModelPartNumStr);
    DEBUG((DEBUG_INFO,"ModelPartNumStr:%s \n", ModelPartNumStr));


   //get MaxPowerCapacity String
    MaxPowerCapacityStr = AllocatePool(2*BMC_16BYTE);
    ZeroMem(MaxPowerCapacityStr,2*BMC_16BYTE);
    ASSERT (MaxPowerCapacityStr != NULL);
     for(i=32;(i<48)&&(ResponseData[i] != 0);i++)
     {
       MaxPowerCapacityStr[i-32] = ResponseData[i];
     }
    MaxPowerCapacityStrLen = StrLen(MaxPowerCapacityStr);
    MaxPower = StrDecimalToUintn(MaxPowerCapacityStr);
    DEBUG((DEBUG_INFO,"MaxPowerCapacityStr:%s \n", MaxPowerCapacityStr));


   //get Serial Nummber String
    SerialNumStr = AllocatePool(2*BMC_16BYTE);
    ZeroMem(SerialNumStr,2*BMC_16BYTE);
    ASSERT (SerialNumStr != NULL);
    for(i=48;(i<64)&&(ResponseData[i] != 0);i++)
    {
      SerialNumStr[i-48] = ResponseData[i];
    }
    SerialNumStrLen = StrLen(SerialNumStr);
    DEBUG((DEBUG_INFO,"SerialNumStr:%s \n", SerialNumStr));


   //get Revision Level String
    RevisionLevelStr = AllocatePool(2*BMC_16BYTE);
    ZeroMem(RevisionLevelStr,2*BMC_16BYTE);
    ASSERT (RevisionLevelStr != NULL);
     for(i=64;(i<80)&&(ResponseData[i] != 0);i++)
     {
      RevisionLevelStr[i-64] = ResponseData[i];
     }
    RevisionLevelStrLen = StrLen(RevisionLevelStr);
    DEBUG((DEBUG_INFO,"RevisionLevelStr:%s \n", RevisionLevelStr));


   //get  PowerSupplyPresent
    PowerSupplyPresentTemp = AllocatePool(2*2*BMC_16BYTE);
    ZeroMem(PowerSupplyPresentTemp,2*2*BMC_16BYTE);
    ASSERT (PowerSupplyPresentTemp != NULL);
     for(i=80;(i<112)&&(ResponseData[i] != 0);i++)
     {
       PowerSupplyPresentTemp[i-80] = ResponseData[i];
     }
    PowerSupplyPresentFlag = (UINT16)StrDecimalToUintn(PowerSupplyPresentTemp);
     if(PowerSupplyPresentFlag ==  0)
     {
       DEBUG ((DEBUG_INFO,"PowerSupply not Present\n"));
     }else{
       DEBUG ((DEBUG_INFO,"PowerSupplyPresent Flag is %d\n",PowerSupplyPresentFlag));
       PowerSupplyPresentFlag = 1;
     }


   //Psu Location,DeviceName,AssetTag
     if(Index == 0)
     {
       LocationStr     = L"in the back, on the left-hand side";
       ASSERT (LocationStr != NULL);
       LocationStrLen     = StrLen(LocationStr);
       
       DeviceNameStr     = L"PSU0";
       ASSERT (DeviceNameStr != NULL);
       DeviceNameStrLen   = StrLen(DeviceNameStr);
     }
     else {
       LocationStr     = L"in the back, on the left-hand side";
       ASSERT (LocationStr != NULL);
       LocationStrLen     = StrLen(LocationStr);
       
       DeviceNameStr     = L"PSU1";
       ASSERT (DeviceNameStr != NULL);
       DeviceNameStrLen   = StrLen(DeviceNameStr);
     }

    AssetTagNumStr   = L"N/A";
    ASSERT (AssetTagNumStr != NULL);
    AssetTagNumStrLen     = StrLen(AssetTagNumStr);
    
    TableSize = sizeof(SMBIOS_TABLE_TYPE39) + 1 + LocationStrLen + 1 + DeviceNameStrLen + 1 + RevisionLevelStrLen + 1+ SerialNumStrLen + 1 + \
    AssetTagNumStrLen + 1 + ModelPartNumStrLen + 1 + ManufaturerStrLen  + 1 + 1;

    Type39Record = AllocatePool(TableSize);
    ZeroMem(Type39Record, TableSize);
    ASSERT (Type39Record != NULL);

    Type39Record->Hdr.Type           = EFI_SMBIOS_TYPE_SYSTEM_POWER_SUPPLY;
    Type39Record->Hdr.Handle         = SMBIOS_HANDLE_PI_RESERVED;
    Type39Record->Hdr.Length         = sizeof(SMBIOS_TABLE_TYPE39);
    Type39Record->PowerUnitGroup     = Index+1;
    Type39Record->Location           = 0x01;
    Type39Record->DeviceName         = 0x02;
    Type39Record->Manufacturer       = 0x03;
    Type39Record->SerialNumber       = 0x04;
    Type39Record->AssetTagNumber     = 0x05;
    Type39Record->ModelPartNumber    = 0x06;
    Type39Record->RevisionLevel      = 0x07;
    Type39Record->MaxPowerCapacity   = (UINT16)MaxPower;
    
    DEBUG ((DEBUG_INFO,"MaxPower is %d\n",MaxPower)); 
    
    Type39Record->PowerSupplyCharacteristics.PowerSupplyHotReplaceable = (UINT16)(0);
    Type39Record->PowerSupplyCharacteristics.PowerSupplyPresent        = PowerSupplyPresentFlag;
    Type39Record->PowerSupplyCharacteristics.PowerSupplyUnplugged      = (UINT16)(0);
    Type39Record->PowerSupplyCharacteristics.InputVoltageRangeSwitch   = (UINT16)(0x01);
    Type39Record->PowerSupplyCharacteristics.PowerSupplyStatus         = (UINT16)(0x03);
    Type39Record->PowerSupplyCharacteristics.PowerSupplyType           = (UINT16)(0x04);
    Type39Record->PowerSupplyCharacteristics.Reserved                  = (UINT16)(0);

    Type39Record->InputCurrentProbeHandle  = 0xFFFF;
    Type39Record->InputVoltageProbeHandle  = 0xFFFF;
    Type39Record->CoolingDeviceHandle      = 0xFFFF;

    OptionalStrStart = (CHAR8 *) (Type39Record + 1);
    UnicodeStrToAsciiStr (LocationStr,     OptionalStrStart);
    UnicodeStrToAsciiStr (DeviceNameStr,   OptionalStrStart + LocationStrLen + 1);
    UnicodeStrToAsciiStr (ManufacturerStr,   OptionalStrStart + LocationStrLen + 1 + DeviceNameStrLen + 1);
    UnicodeStrToAsciiStr (SerialNumStr,   OptionalStrStart + LocationStrLen + 1 + DeviceNameStrLen + 1 + ManufaturerStrLen + 1);
    UnicodeStrToAsciiStr (AssetTagNumStr,   OptionalStrStart + LocationStrLen + 1 + DeviceNameStrLen + 1 + \
                          ManufaturerStrLen + 1 + SerialNumStrLen+ 1);
    UnicodeStrToAsciiStr (ModelPartNumStr,   OptionalStrStart + LocationStrLen + 1 + DeviceNameStrLen + 1 + \
                          ManufaturerStrLen + 1 + SerialNumStrLen+ 1 + AssetTagNumStrLen + 1);
    UnicodeStrToAsciiStr (RevisionLevelStr, OptionalStrStart + LocationStrLen + 1 + DeviceNameStrLen + 1 + \
                          ManufaturerStrLen + 1 + SerialNumStrLen + 1 +AssetTagNumStrLen +1+ModelPartNumStrLen + 1);
     

    Status = gBS->LocateProtocol ( &gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
     if (EFI_ERROR(Status))
     {
       DEBUG((DEBUG_INFO,"Locate EfiSmbiosProtocol fail!\n"));
       continue;
     }
     
    PowerSupplyHandle = SMBIOS_HANDLE_PI_RESERVED;
    Status = Smbios->Add (Smbios, NULL, &PowerSupplyHandle, (EFI_SMBIOS_TABLE_HEADER*)Type39Record);
    DEBUG((DEBUG_INFO,"add PSU%d status is %r\n",Index+1,Status));
   
    
   // free pointer
    FreePool(SerialNumStr);
    FreePool(ModelPartNumStr);
    FreePool(ManufacturerStr);
    FreePool(RevisionLevelStr);
    FreePool(MaxPowerCapacityStr);
    FreePool(Type39Record);
    FreePool(PowerSupplyPresentTemp);
       }
  }

  return Status;
}



VOID
SendToBmcConfig()
{
  EFI_STATUS      Status;
  UINTN           Size;
  BMC_CONFIG_SETUP       commanddata,ResponseData;
  SETUP_DATA            SetupData;
  UINTN        GroupOrderSize = 0;
  UINT8                 ResponseDataSize;
  UINT8        LegacyGroupOrder[4];
  UINT8        UefiGroupOrder[5];
  UINTN        LegacyGroupOrderSize = 0;
  UINTN        UefiGroupOrderSize = 0;
  CHAR8                    *CurrentLang;
  UINT8        *CMD;
  UINTN        BufferSize;
  CBS_CONFIG   CBSData;
  CBS_CONFIG    CbsVariable;
  
  Size = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  );  
  TranslateBmcConfig (&commanddata, &SetupData, 0);
  Size = sizeof (CBS_CONFIG);
  Status = gRT->GetVariable(L"AmdSetup", &gCbsSystemConfigurationGuid, NULL, &Size, &CbsVariable);
  DEBUG((EFI_D_ERROR, __FUNCTION__"(L %d), Get CBS :%r, Size :%d.\n", __LINE__, Status, Size));
  if (!EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, __FUNCTION__"(L %d), CbsDfCmnMemIntlv :%d.\n", __LINE__, CbsVariable.CbsDfCmnMemIntlv));
    DEBUG((EFI_D_ERROR, __FUNCTION__"(L %d), CbsCmnCpuCpb :%d.\n", __LINE__, CbsVariable.CbsCmnCpuCpb));

    if (CbsVariable.CbsDfCmnMemIntlv == 7) {
      commanddata.CbsDfCmnMemIntlv = 4;
    } else {
      commanddata.CbsDfCmnMemIntlv = CbsVariable.CbsDfCmnMemIntlv;
    }
    commanddata.CbsCmnCpuCpb = CbsVariable.CbsCmnCpuCpb;
  }
  
  BufferSize = sizeof(CBS_CONFIG);
  Status = gRT->GetVariable (
              L"AmdSetup",
              &gCbsSystemConfigurationGuid,
              NULL,
              &BufferSize,
              &CBSData
              );
  UefiGroupOrderSize = sizeof(UefiGroupOrder);
  Status = gRT->GetVariable(
                  BYO_UEFI_BOOT_GROUP_VAR_NAME, 
                  &gByoGlobalVariableGuid,
                  NULL,
                  &UefiGroupOrderSize,
                  UefiGroupOrder
                  );    

  LegacyGroupOrderSize = sizeof(LegacyGroupOrder);
  Status = gRT->GetVariable(
                  BYO_LEGACY_BOOT_GROUP_VAR_NAME, 
                  &gByoGlobalVariableGuid,
                  NULL,
                  &LegacyGroupOrderSize,
                  LegacyGroupOrder
                  );  

  if(PcdGet8(PcdBiosBootModeType) == 1){
    commanddata.FirstBoot = UefiGroupOrder[0];
    commanddata.SecondBoot= UefiGroupOrder[1];
    commanddata.ThirdBoot = UefiGroupOrder[2];
    commanddata.FourthBoot= UefiGroupOrder[3];
    commanddata.FifthBoot = UefiGroupOrder[4]; 
    commanddata.SixthBoot = 0x01;
  }else{
    commanddata.FirstBoot = LegacyGroupOrder[0]-0x80;
    commanddata.SecondBoot= LegacyGroupOrder[1]-0x80;
    commanddata.ThirdBoot = LegacyGroupOrder[2]-0x80;
    commanddata.FourthBoot= LegacyGroupOrder[3]-0x80;
    commanddata.FifthBoot = 0x05; 
    commanddata.SixthBoot = 0x01;
  }
  DumpMem8(UefiGroupOrder,UefiGroupOrderSize);
  DumpMem8(LegacyGroupOrder,LegacyGroupOrderSize);

  CurrentLang = GetEfiGlobalVariable(L"PlatformLang");
  DEBUG((EFI_D_INFO, "CurrentLang:%a\n", CurrentLang));
  if (CurrentLang[0] == 'z') {
    commanddata.Language = 0;
  } else {
    commanddata.Language = 1;
  }
  if(CurrentLang != NULL){
    FreePool(CurrentLang);
  }
  CMD = (UINT8*)&commanddata;
  DumpMem8(CMD, sizeof(BMC_CONFIG_SETUP));
  
  Status = EfiSendCommandToBMC (
                  0x3e,
                  0xc3,
                  (UINT8 *) &commanddata,
                  sizeof(commanddata),
                  (UINT8 *)&ResponseData,
                  (UINT8 *)&ResponseDataSize
                  );
  DEBUG((EFI_D_INFO,"Send command Status = %r\n",Status));
}

VOID
BootBmcNextBoot()
{  
  UINT16    *BootNext;
  UINTN    NextSize;
  UINTN    BootOptionCount;
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions;
  UINTN                         Index, I;

  BootNext = EfiBootManagerGetVariableAndSize (
                  L"BmcBootNext",
                  &gBMCOrderGuid,
                  &NextSize
                  );

  if (BootNext == NULL || !NextSize) {
    return;
  }

  BootOptionCount = 0;

  BootOptions = EfiBootManagerGetLoadOptions(&BootOptionCount, LoadOptionTypeBoot);
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), BootOptionCount :%d.\n", BootOptionCount));
  if (!BootOptionCount) {
    FreePool (BootNext);
    return;
  }

  for(Index=0;Index<NextSize/sizeof(UINT16);Index++){
    for(I=0;I<BootOptionCount;I++){
	  if(BootNext[Index] == BootOptions[I].OptionNumber){
        ByoEfiBootManagerBoot (&BootOptions[I]);
	  }
	}
  }
  
  ByoEfiBootManagerFreeLoadOptions(BootOptions, BootOptionCount);
  FreePool (BootNext);
  return; 
}