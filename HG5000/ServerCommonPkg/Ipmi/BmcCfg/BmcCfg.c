
/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  BmcCfg.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#include "BmcCfg.h"

#define BIOS_ID_SIGN_DATA  {'$', 'I', 'B', 'I', 'O', 'S', 'I', '$'}
#define SLAVE_BORD_ID_BASE _PCD_VALUE_PcdFlashFvMainBase+0xA0 //0xFDF800A0 

EFI_GUID  gCrcDataGuid = { 0x69729EA8, 0xD6FC, 0x4C02, { 0xAF, 0x96, 0x57, 0xD7, 0xC, 0x8D, 0x35, 0x62 } };

extern EFI_GUID gBdsAllDriversConnectedProtocolGuid;

STATIC CONST UINT8 gSlaveBiosidSign[] = BIOS_ID_SIGN_DATA;

UTC_INFO_2_BMC    *UTC;
UINT8             HddDeviceId = 0;

UINT8             *pBiosinfo = NULL;
UINTN             gBiosinfoLen = 0;

UINT8             *pPciinfo = NULL;
UINTN             gPciinfoLen = 0;

UINT8             *pCpuinfo = NULL;
UINTN             gCpuinfoLen = 0;

UINT8             *pDimminfo = NULL;
UINTN             gDimminfoLen = 0;

UINT8             *pHddinfo = NULL;
UINTN             gHddinfoLen = 0;

UINT8             *pNvmeinfo = NULL;
UINTN             gNvmeinfoLen = 0;

UINT8             *pNetcardinfo = NULL;
UINTN             gNetcardinfoLen = 0;

UINT8             *pBMCVersioninfo = NULL;
UINTN             gBMCVersioninfoLen = 0;

EFI_STATUS
FanSpeedCtrl (
  BMC_SETUP_DATA   *SetupData
  )
{
 // EFI_STATUS      Status;
 // UINT8           CmdDataBuff[10];
 // UINT8           CmdDataSize;
 // UINT8           ResponseData[10];
 // UINT8           ResponseSize;
 // EFI_GUID        SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
 // UINT8           FanSpeed;
 // UINT8           AmiFanCommand1[1];
 // UINT8           AmiFanCommand2[2];
/*
  ResponseSize = sizeof (ResponseData);
  CmdDataBuff[0] = 0x01;
  CmdDataBuff[1] = 0x02;
  CmdDataSize = 2; 
  Status = EfiSendCommandToBMC (
             SM_HYGON_NETFN_APP,
             HYGON_GET_FUN_SPEED,
             CmdDataBuff,
             CmdDataSize,
             ResponseData,
             &ResponseSize
             );
  DEBUG ((EFI_D_INFO, "Get FAN:%r\n", Status));

  if (!EFI_ERROR (Status) && (SetupData->FanProfile != ResponseData[1])) {


  switch (SetupData->FanProfile) {

     case 0:
       FanSpeed = SetupData->FanSpeedControl;  
       CmdDataBuff[0]=0x0F;  
       CmdDataBuff[1]=0x02;  
       CmdDataBuff[2]=0x00;  
       CmdDataSize = 3; 
       Status = EfiSendCommandToBMC (
                SM_HYGON_NETFN_APP,
                HYGON_GET_FUN_SPEED,
                CmdDataBuff,
                CmdDataSize,
                ResponseData,
                &ResponseSize
                );
   
      //save to NV
       CmdDataBuff[0]=0x0F;  
       CmdDataBuff[1]=0x05;  
       CmdDataBuff[2]=FanSpeed;//0x64;  
       CmdDataSize = 3; 
       Status = EfiSendCommandToBMC (
                SM_HYGON_NETFN_APP,
                HYGON_GET_FUN_SPEED,
                CmdDataBuff,
                CmdDataSize,
                ResponseData,
                &ResponseSize
                );

       //Effective immediately 
       CmdDataBuff[0]=0x00;  
       CmdDataBuff[1]=0x06;  
       CmdDataBuff[2]=FanSpeed; //0x64; 
       CmdDataBuff[3]=FanSpeed; //0x64;
       CmdDataBuff[4]=FanSpeed; //0x64; 
       CmdDataBuff[5]=FanSpeed; //0x64;  
       CmdDataBuff[6]=FanSpeed; //0x64; 
       CmdDataBuff[7]=FanSpeed; //0x64;  
       CmdDataBuff[8]=FanSpeed; //0x64; 
       CmdDataBuff[9]=FanSpeed; //0x64;  
       CmdDataSize = 10; 
       Status = EfiSendCommandToBMC (
                SM_HYGON_NETFN_APP,
                HYGON_GET_FUN_SPEED,
                CmdDataBuff,
                CmdDataSize,
                ResponseData,
                &ResponseSize
                );
      break;

    case 1:
       CmdDataBuff[0]=0x0F;  
       CmdDataBuff[1]=0x02;  
       CmdDataBuff[2]=0x01; 
       CmdDataSize = 3; 
       Status = EfiSendCommandToBMC (
                SM_HYGON_NETFN_APP,
                HYGON_GET_FUN_SPEED,
                CmdDataBuff,
                CmdDataSize,
                ResponseData,
                &ResponseSize
                );
      break;

    case 2:
       CmdDataBuff[0]=0x0F;  
       CmdDataBuff[1]=0x02;  
       CmdDataBuff[2]=0x02; 
       CmdDataSize = 3; 
       Status = EfiSendCommandToBMC (
                SM_HYGON_NETFN_APP,
                HYGON_GET_FUN_SPEED,
                CmdDataBuff,
                CmdDataSize,
                ResponseData,
                &ResponseSize
                );
      break;

    case 3:  
       CmdDataBuff[0]=0x0F;  
       CmdDataBuff[1]=0x02;  
       CmdDataBuff[2]=0x03; 
       CmdDataSize = 3; 
       Status = EfiSendCommandToBMC (
                SM_HYGON_NETFN_APP,
                HYGON_GET_FUN_SPEED,
                CmdDataBuff,
                CmdDataSize,
                ResponseData,
                &ResponseSize
                );
      break;
  }
  }

  if (SetupData->FanProfile == 0) {// Manual Mode
    FanSpeed = SetupData->FanSpeedControl;
    CmdDataBuff[0] = 0xCC;
    CmdDataBuff[1] = 0xCB;
    CmdDataBuff[2] = 0x00;
    CmdDataBuff[3] = SetupData->FanProfile;
    CmdDataBuff[4] = 0xFF;
    CmdDataBuff[5] = FanSpeed;
    CmdDataBuff[6] = FanSpeed;
    CmdDataBuff[7] = FanSpeed;
    CmdDataBuff[8] = FanSpeed;
    CmdDataBuff[9] = FanSpeed;
    CmdDataBuff[10] = FanSpeed;
    CmdDataBuff[11] = FanSpeed;
    CmdDataBuff[12] = FanSpeed;
    CmdDataSize = 13;
    ResponseSize = sizeof (ResponseData);
    Status = EfiSendCommandToBMC (
               0x3E,      //NetFn
               0x2B,      //CMD
               CmdDataBuff,
               CmdDataSize,
               ResponseData,
               &ResponseSize
               );
    DEBUG ((EFI_D_INFO, "Set FAN:%r,FanProfile:%x,FanSpeedControl:%d\n", Status,SetupData->FanProfile,FanSpeed));
  } else {//Auto Mode
     CmdDataBuff[0] = 0xCC;
     CmdDataBuff[1] = 0xCB;
     CmdDataBuff[2] = 0x00;
     CmdDataBuff[3] = 0x01;
     CmdDataBuff[4] = 0x00;
     CmdDataBuff[5] = 0x00;
     CmdDataBuff[6] = 0x00;
     CmdDataBuff[7] = 0x00;
     CmdDataBuff[8] = 0x00;
     CmdDataBuff[9] = 0x00;
     CmdDataBuff[10] = 0x00;
     CmdDataBuff[11] = 0x00;
     CmdDataBuff[12] = 0x00;
     CmdDataSize = 13;
     ResponseSize = sizeof (ResponseData);
     Status = EfiSendCommandToBMC (
                0x3E,      //NetFn
                0x2B,      //CMD
                CmdDataBuff,
                CmdDataSize,
                ResponseData,
                &ResponseSize
                );
     DEBUG ((EFI_D_INFO, "Set FAN:%r,FanProfile:%x\n", Status,SetupData->FanProfile));
  }  

  if(Status != EFI_SUCCESS){// support ami bmc
  	if(SetupData->FanProfile){//auto
  	  AmiFanCommand1[0] = 0;
      ResponseSize = sizeof (ResponseData);
      Status = EfiSendCommandToBMC (
                 0x3c,
                 0x2f,
                 AmiFanCommand1,
                 1,
                 ResponseData,
                 &ResponseSize
                 );
     DEBUG ((EFI_D_INFO, "Set FAN:%r,FanProfile:%x\n", Status,SetupData->FanProfile));
	}else{
  	  AmiFanCommand1[0] = 1;
      ResponseSize = sizeof (ResponseData);
      Status = EfiSendCommandToBMC (
                 0x3c,
                 0x2f,
                 AmiFanCommand1,
                 1,
                 ResponseData,
                 &ResponseSize
                 );
  	  AmiFanCommand2[0] = 0xff;
  	  AmiFanCommand2[1] = SetupData->FanSpeedControl;
      Status = EfiSendCommandToBMC (
                 0x3c,
                 0x2d,
                 AmiFanCommand2,
                 2,
                 ResponseData,
                 &ResponseSize
                 );
     DEBUG ((EFI_D_INFO, "Set FAN:%r,FanProfile:%x\n", Status,SetupData->FanProfile));
    }
  }*/ 
  return EFI_SUCCESS;
}


#ifndef MDEPKG_NDEBUG                                   // debug mode  

EFI_STATUS
ReportFrb2Status (
  )
/*++

  Routine Description:
    This routine is built only when DEBUG_MODE is enabled.  It is used
    to report the status of FRB2 when the FRB2 driver is installed.

  Arguments:
    none

  Returns:
    EFI_SUCCESS: All info was retrieved and reported
    EFI_ERROR:   There was an error during info retrieval

--*/
{
  UINT32       DataSize;
  EFI_STATUS   Status;
  UINT8        FrbType;
  EFI_FRB_TIMER FrbTimer[4];
  FrbType = 1;  // 1: frb  3:oswdt

  DataSize = sizeof (EFI_FRB_TIMER);
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_APP,                       // NetFunction
             EFI_APP_GET_WATCHDOG_TIMER,             // Command
             NULL,                                   // *CommandData
             0,                                      // CommandDataSize
             (UINT8*) &FrbTimer[FrbType],            // *ResponseData
             (UINT8*) &DataSize                      // ResponseDataSize
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Failed to get Watchdog Timer info from BMC.\n"));
    return Status;
  }

  //
  // Check if timer is running, report status to DEBUG_MODE output.
  //
  if (FrbTimer[FrbType].StartTimer == 1) {
    DEBUG ((EFI_D_INFO, "FRB2 Timer is running.\n"));
  } else {
    DEBUG ((EFI_D_INFO, "FRB2 Timer is not running.\n"));
  }

  return EFI_SUCCESS;
}

#endif

#if 1
STATIC
UINT16
SmbiosGetStrOffset(
   CONST SMBIOS_STRUCTURE *Hdr,
   UINT8 Index
   )
{
  CONST UINT8 *pData8;
  UINT8       i;

  if(Index == 0){return 0;}
  
  pData8  = (UINT8*)Hdr;
  pData8 += Hdr->Length;
  
  i = 1;
  while(i != Index){
    while(*pData8!=0){pData8++;}

    if(pData8[1] == 0){     // if next byte of a string end is NULL, type end.
      break;
    }
    
    pData8++;
    i++;
  }
  if(i == Index){
    return (UINT16)(pData8 - (UINT8*)Hdr);
  } else {
    return 0;
  }
}


STATIC
CHAR8 * 
SmbiosGetStringInTypeByIndex(
  SMBIOS_STRUCTURE_POINTER Hdr, 
  SMBIOS_TABLE_STRING      StrIndex
  )
{
  CHAR8  *Str8;
  if(StrIndex == 0){
    return "";
  }
  Str8 = (CHAR8*)(Hdr.Raw + SmbiosGetStrOffset(Hdr.Hdr, StrIndex));
  return Str8;
}

#if 0
EFI_STATUS
GetSlaveBiosId (
  OUT SLAVE_BIOS_ID_IMAGE  *SlaveBiosIdImage
  )
{
  EFI_STATUS   Status = EFI_NOT_FOUND;
  UINT8        *Buffer8;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  Buffer8= ( UINT8 *)(UINTN)SLAVE_BORD_ID_BASE;
    if(CompareMem(Buffer8, gSlaveBiosidSign, sizeof(gSlaveBiosidSign)) == 0){
      CopyMem(SlaveBiosIdImage, Buffer8, sizeof(SLAVE_BIOS_ID_IMAGE));
      Status = EFI_SUCCESS;
    }   
	DEBUG((EFI_D_ERROR, "Status=%r;\n",Status));

  return Status;
}

VOID
SendSlaveBiosInfo2BMC(
)
{
  CHAR16    SlaveBiosReleaseTime[17];
  SLAVE_BIOS_ID_IMAGE  SlaveBiosIdImage;
  CHAR16  SlaveRevision[14]; 
  EFI_STATUS   Status = EFI_NOT_FOUND;
  
  UINT8                        ResponseSize=1;
  UINT8                        ResponseData[50];
  UINT8                        BiosVer2Bmc[50];
  UINTN                        StrLength;

  CHAR8                        *BiosDate=0;
  CHAR8                        *BiosVer=0;

  UINT8 index;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  Status = GetSlaveBiosId(&SlaveBiosIdImage);
  if (EFI_ERROR (Status)) {
    return ;
  }
  

  SlaveBiosReleaseTime[0] = SlaveBiosIdImage.BiosIdString.TimeStamp[2];
   SlaveBiosReleaseTime[1] = SlaveBiosIdImage.BiosIdString.TimeStamp[3];
   SlaveBiosReleaseTime[2] = (CHAR16) ((UINT8) ('/'));  
   SlaveBiosReleaseTime[3] = SlaveBiosIdImage.BiosIdString.TimeStamp[4];
   SlaveBiosReleaseTime[4] = SlaveBiosIdImage.BiosIdString.TimeStamp[5];
   SlaveBiosReleaseTime[5] = (CHAR16) ((UINT8) ('/'));
   SlaveBiosReleaseTime[6] = '2';
   SlaveBiosReleaseTime[7] = '0';
   SlaveBiosReleaseTime[8] = SlaveBiosIdImage.BiosIdString.TimeStamp[0];
   SlaveBiosReleaseTime[9] = SlaveBiosIdImage.BiosIdString.TimeStamp[1];
  SlaveBiosReleaseTime[10] = (CHAR16) ((UINT8) ('\0'));
 
  SlaveRevision[0]=SlaveBiosIdImage.BiosIdString.BoardId[0];
  SlaveRevision[1]=SlaveBiosIdImage.BiosIdString.BoardId[1];
  SlaveRevision[2]=SlaveBiosIdImage.BiosIdString.BoardId[2];
  SlaveRevision[3]=SlaveBiosIdImage.BiosIdString.BoardId[3];
  SlaveRevision[4]=SlaveBiosIdImage.BiosIdString.BoardId[4];
  SlaveRevision[5]=SlaveBiosIdImage.BiosIdString.BoardId[5];
  SlaveRevision[6]=SlaveBiosIdImage.BiosIdString.BoardId[6];
  SlaveRevision[7]=(CHAR16) ((UINT8) ('-'));
  SlaveRevision[8]=SlaveBiosIdImage.BiosIdString.BuildType;
//  SlaveRevision[9]=SlaveBiosIdImage.BiosIdString.VersionMajor[0];
//  SlaveRevision[10]=SlaveBiosIdImage.BiosIdString.VersionMajor[1];
  SlaveRevision[9]=SlaveBiosIdImage.BiosIdString.VersionMajor[2];
  SlaveRevision[10]=SlaveBiosIdImage.BiosIdString.VersionMajor[3];
  SlaveRevision[11] = (CHAR16) ((UINT8) ('\0'));

  
  BiosVer = (CHAR8*)AllocatePool(sizeof(CHAR8)*14);

  BiosDate = (CHAR8*)AllocatePool(sizeof(CHAR8)*11);

  for(index=0;index<14;index++){
  	BiosVer[index] = (CHAR8)SlaveRevision[index];
  }

    for(index=0;index<11;index++){
  	BiosDate[index] = (CHAR8)SlaveBiosReleaseTime[index];
  }
	ZeroMem (&BiosVer2Bmc[0], 50);
	ZeroMem (ResponseData, 50);
	
	BiosVer2Bmc[0] = SET_SLAVE_BIOS_VER;
	BiosVer2Bmc[1] = 3;
	
    StrLength = AsciiStrSize(BiosVer);
	
  AsciiStrCpy ((CHAR8*)&BiosVer2Bmc[3], BiosVer);


  BiosVer2Bmc[2] = BiosVer2Bmc[1] + (UINT8)StrLength;
  AsciiStrCpy ((CHAR8*)&BiosVer2Bmc[3+StrLength], BiosDate);
	
	Status = EfiSendCommandToBMC (
			   SM_BYOSOFT_NETFN_APP,
			   SM_BYOSOFT_NETFN_SUB_FUN,
			   &BiosVer2Bmc[0],
			   50,
			   &ResponseData[0],
			   &ResponseSize
			   );
  DEBUG((EFI_D_INFO, "Send slave BiosInfo2BMC Status = %r\n",Status));
}
#endif
void
SendPciInfo2BMC_old (
)
{
  EFI_STATUS                  Status;
  UINT8                       Index;
  UINTN                       HandleCount;
  EFI_HANDLE                  *HandleBuffer;
  EFI_PCI_IO_PROTOCOL         *PciIo;
  UINTN                       Segment;
  UINTN                       Bus;
  UINTN                       Device;
  UINTN                       Function;
  UINT32                      VenderId;
  UINT8                       ClassCode[3];
  //PCI_INFO_2_BMC              *PciInfor2Bmc;
  UINT8                        PciInfor2Bmc[120];
  //PCI_INFO                    *PciInfor;
  UINT8                       ResponseSize=1;
  UINT8                        ResponseData[120];
  UINT8                        Index2 =0;
  //UINT8                        PciDeviceCount=0;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));

  ZeroMem (&PciInfor2Bmc[0], 120);
  ZeroMem (ResponseData, 120);

  PciInfor2Bmc[0] = SET_PCI_INFO;
  PciInfor2Bmc[1] = 0;
  
  Status = gBS->LocateHandleBuffer (
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
    ASSERT_EFI_ERROR (Status);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, 9, 3, &ClassCode);
    if (ClassCode[2] == 0x06 || ClassCode[2] == 0x08 || ClassCode[2] > 0x0F ) { // skip bridge + Base system peripherals + Encryption/Decryption controllers
      continue;
    }
    PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, 1, &VenderId);
  	//if (Bus == 0x00 || Bus == 0x03 || Bus == 0x08 || 
  	//	Bus == 0x80 || Bus == 0x86) { // skip onboard pci device
    //  continue;
    //  }
    if ((VenderId&0xFFFF) == 0x1D94) { // skip bridge + Base system peripherals + Encryption/Decryption controllers
      continue;
    }
  	DEBUG((EFI_D_INFO,"pci bus = %08x,ven = %08x",Bus,VenderId));
	  Index2 = PciInfor2Bmc[1];
	  CopyMem (&PciInfor2Bmc[Index2*10+2], (UINT8*)&VenderId, 4);
	  PciInfor2Bmc[Index2*10+6] = (UINT8)Bus;
	  PciInfor2Bmc[Index2*10+7] = (UINT8)Device;
	  PciInfor2Bmc[Index2*10+8] = (UINT8)Function;
    CopyMem (&PciInfor2Bmc[Index2*10+9], (UINT8*)ClassCode, 3);
	  PciInfor2Bmc[1] ++;
    DEBUG((EFI_D_INFO, "bus:dev:fun:(%02X,%02X,%02X) id:%08X cc:(%02X,%02X,%02X)\n", Bus, Device, Function, VenderId, ClassCode[2], ClassCode[1], ClassCode[0]));
    //if ((UINTN)PciInfor > (UINTN)(CommandData + BIOS_2_BMC_DATA_SIZE - sizeof (PCI_INFO_2_BMC))) {
   //   break;
    //}
  }
  DEBUG ((EFI_D_INFO,"PciDeviceCount:%x\n", PciInfor2Bmc[1]));

  Status = EfiSendCommandToBMC (
             SM_BYOSOFT_NETFN_APP,
             SM_BYOSOFT_NETFN_SUB_FUN,
             &PciInfor2Bmc[0],
             120,
             &ResponseData[0],
             &ResponseSize
             );
  DEBUG((EFI_D_INFO, "SendPciInfo2BMC Status = %r\n",Status));


  if (HandleBuffer != NULL){
    FreePool (HandleBuffer);
  }
}

BOOLEAN
IsMyDevicePathEnd (
  IN EFI_DEVICE_PATH_PROTOCOL    *Dp
  );

VOID*
LibGetPciIoFromDp (  
  IN EFI_BOOT_SERVICES         *BS,  
  IN EFI_DEVICE_PATH_PROTOCOL  *Dp  )
{
  EFI_STATUS                Status;  
  EFI_DEVICE_PATH_PROTOCOL  *TempDp = Dp;  
  EFI_HANDLE                Device;  
  EFI_PCI_IO_PROTOCOL       *PciIo = NULL;  
  Status = BS->LocateDevicePath(&gEfiPciIoProtocolGuid, &TempDp, &Device); 

  if(!EFI_ERROR(Status) && IsMyDevicePathEnd(TempDp)) {    
    Status = BS->HandleProtocol(Device, &gEfiPciIoProtocolGuid, (VOID**)&PciIo);  
  }  
  return PciIo;
}

EFI_DEVICE_PATH_PROTOCOL gEndDp = gEndEntire;



UINT32
GetCacheSize (
  EFI_SMBIOS_PROTOCOL       *Smbios,
  EFI_SMBIOS_HANDLE         CacheHandle
)
{
  EFI_SMBIOS_HANDLE         SmbiosHandle;
  EFI_SMBIOS_TYPE           SmbiosType;
  EFI_SMBIOS_TABLE_HEADER   *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER  p;
  EFI_STATUS                Status = EFI_SUCCESS;
  UINT32                    CacheSize = 0;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_CACHE_INFORMATION;
  while(1){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status) || SmbiosHandle == CacheHandle){
       p.Hdr = SmbiosHdr;
      break;
    }
  }
  if (EFI_ERROR(Status)) {
    goto ProcExit;
  }
    
  CacheSize = p.Type7->InstalledSize2;
  if(CacheSize & BIT31){
    CacheSize = CacheSize * 64;
  }

  if(CacheSize >= 1024 ){
    CacheSize = CacheSize / 1024;                     //MB
  } 
  
ProcExit:
  return CacheSize;
}

UINT32
GetL1CacheSize (
  EFI_SMBIOS_PROTOCOL       *Smbios,
  EFI_SMBIOS_HANDLE         CacheHandle
)
{
  EFI_SMBIOS_HANDLE         SmbiosHandle;
  EFI_SMBIOS_TYPE           SmbiosType;
  EFI_SMBIOS_TABLE_HEADER   *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER  p;
  EFI_STATUS                Status = EFI_SUCCESS;
  UINT32                    CacheSize = 0;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_CACHE_INFORMATION;
  while(1){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status) || SmbiosHandle == CacheHandle){
       p.Hdr = SmbiosHdr;
      break;
    }
  }
  if (EFI_ERROR(Status)) {
    goto ProcExit;
  }
    
  CacheSize = p.Type7->InstalledSize2;
  if(CacheSize & BIT31){
    CacheSize = CacheSize * 64;
  }

  
ProcExit:
  return CacheSize;
}

void 
HexToAscii(UINT8 Value, CHAR8 *s)
{
  UINT8 Data;

  Data = (Value >> 4) & 0xf;
  if(Data >= 0 && Data <= 9){
    s[0] = Data + '0';
  } else if(Data >= 0xa && Data <= 0xf){
    s[0] = Data - 0xa + 'A';
  }

  Data = Value & 0xf;
  if(Data >= 0 && Data <= 9){
    s[1] = Data + '0';
  } else if(Data >= 0xa && Data <= 0xf){
    s[1] = Data - 0xa + 'A';
  }
  s[2] = 0;
  
  return;
}

VOID
SendCpuInfo2BMC_old(
  IN EFI_SMBIOS_PROTOCOL  *Smbios
)
{
  EFI_STATUS                      Status;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  EFI_SMBIOS_TYPE                 SmbiosType;
  EFI_SMBIOS_TABLE_HEADER        *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER        p;
  UINT8                           Cpunum=0; //cpu number
  UINT8                           Cpucnum; //cpu core number
  UINT16                          CpuFreq;
  CHAR8                          *CpuName;
  UINT8                           CpuInfor2Bmc[60];
  UINT8                           ResponseData[60];
  UINT8                           ResponseDataSize;
  UINT8                           i;
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;

  while(1){
    ZeroMem(CpuInfor2Bmc, 60);
    ZeroMem(ResponseData, 60);
    Status     = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
    
    p.Hdr   = SmbiosHdr;
    CpuName = SmbiosGetStringInTypeByIndex(p, p.Type4->ProcessorVersion);
    CpuFreq = p.Type4->CurrentSpeed;
    Cpucnum = p.Type4->CoreCount;
    if(p.Type4->Status == 0 && p.Type4->MaxSpeed == 0){
      continue;   //cpu not present that we shouldn't report to bmc for saving boot time.
    }

    CpuInfor2Bmc[0] = SET_CPU_INFO;
    CpuInfor2Bmc[1] = Cpunum |BIT7;  //means CPU number 
    CpuInfor2Bmc[2] = Cpucnum;  //CPU core number
    CopyMem (&CpuInfor2Bmc[3], (UINT8*)&CpuFreq, 2);     // " MHz"
    for (i=0;i<AsciiStrSize(CpuName);i++) {
      CpuInfor2Bmc[5+i]=*(CpuName+i);
    }
    
    Status = EfiSendCommandToBMC (
                                 SM_BYOSOFT_NETFN_APP,              //OEM CMD to send CPU info to BMC
                                 SM_BYOSOFT_NETFN_SUB_FUN,
                                 (UINT8 *) &CpuInfor2Bmc,
                                 60,
                                 &ResponseData[0],
                                 &ResponseDataSize
                                 );
    Cpunum ++;
  }//loop for cpu number not 1
}
/**
  Eliminate the extra spaces in the Str to one space.
**/
VOID
EliminateExtraSpaces8 (
  IN CHAR8                    *Str
  )
{
  UINTN                        Index;
  UINTN                        ActualIndex;
  
  for (Index = 0, ActualIndex = 0; Str[Index] != '\0'; Index++) {
    if ((Str[Index] != ' ') || ((ActualIndex > 0) && (Str[ActualIndex - 1] != ' '))) {
      Str[ActualIndex++] = Str[Index];
    }
  }
  
  Str[ActualIndex] = '\0';

  while(ActualIndex--){
    if(Str[ActualIndex] == ' '){
      Str[ActualIndex] = 0;
    } else {
      break;
    }
  }
}

VOID
SendDIMMInfo2BMC_old( 
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  )
{
  EFI_STATUS                      Status;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  EFI_SMBIOS_TYPE                 SmbiosType;
  EFI_SMBIOS_TABLE_HEADER        *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER        p;
  UINT8                           DIMMnum= 0;  //dimm number
  UINT8                           Dimmtype= 4; //4-DDR4
  UINT16                          DimmFreq;
  UINT16                          SingleSize;
  CHAR8                          *Manufacturer;
  CHAR8                          *PN;
  CHAR8                          *SN;
  CHAR8                          *DeviceLocator;
  CHAR8                          *BankLocator;
  UINT8                           commanddata[90];
  UINT8                           ResponseData[90];
  UINT8                           ResponseDataSize;
  UINT8                           i;
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_MEMORY_DEVICE;  //type17
  while(1) {
    ZeroMem(commanddata, 90);
    ZeroMem(ResponseData, 90);
    
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
    p.Hdr = SmbiosHdr;
    DimmFreq = p.Type17->Speed;
    SingleSize = p.Type17->Size;
    if(!SingleSize){
       continue; // Dimm not installed 
    }
    Manufacturer = SmbiosGetStringInTypeByIndex(p, p.Type17->Manufacturer);
    PN = SmbiosGetStringInTypeByIndex(p, p.Type17->PartNumber);
    SN = SmbiosGetStringInTypeByIndex(p, p.Type17->SerialNumber);
    DeviceLocator = SmbiosGetStringInTypeByIndex(p, p.Type17->DeviceLocator); //DIMM 0/1    
    BankLocator = SmbiosGetStringInTypeByIndex(p, p.Type17->BankLocator); //P0/1 CHANNEL A/B/C/D/E/F/G/H

    commanddata[0] = SET_DIMM_INFO; 
    if (SingleSize != 0) {
      commanddata[1] = DIMMnum |BIT7;             //means dimm number
      commanddata[5] = Dimmtype;
    }
    else {
      commanddata[1] = DIMMnum;                    //Bit7=0 means not present
      commanddata[5] = 0;                          //no dimm
    }
    commanddata[2] = *(BankLocator + 1);         //Socket;
    commanddata[3] = *(BankLocator + 11);        //Channel;
    commanddata[4] = *(DeviceLocator + 9);       //Dimm;
    commanddata[6] = (UINT8)(DimmFreq & 0xFF);        // low 8 bit  
    commanddata[7] = (UINT8)((DimmFreq >> 8) & 0xFF); //high 8 bit
    commanddata[8] = SingleSize/0x400;                // /1024 -> GB 
    commanddata[9] = SingleSize%0x400;                // /1024 -> GB 

    for (i=0;i<AsciiStrSize(Manufacturer);i++) {
      commanddata[10+i]=*(Manufacturer+i);
    }

    for (i=0;i<AsciiStrSize(PN);i++) {
      commanddata[26+i]=*(PN+i);
    }

    for (i=0;i<AsciiStrSize(SN);i++) {
      commanddata[58+i]=*(SN+i);
    } 
    DIMMnum++;
    Status = EfiSendCommandToBMC (SM_BYOSOFT_NETFN_APP,                       //OEM CMD to send BIOS version and build date to BMC
                                SM_BYOSOFT_NETFN_SUB_FUN,
                                (UINT8 *) &commanddata,
                                sizeof(commanddata),
                                (UINT8 *) &ResponseData,
                                (UINT8 *) &ResponseDataSize);
  }
}

void
SwapEntries (
  IN CHAR8    *Data,
  IN UINT16   Size
  )
{
  UINT16  Index;
  CHAR8   Temp8;

  for (Index = 0; (Index + 1) < Size; Index += 2) {
    Temp8           = Data[Index];
    Data[Index]     = Data[Index + 1];
    Data[Index + 1] = Temp8;
  }
}

UINTN
CalculateNumOfDayPassedThisYear (
  IN    EFI_TIME    Time
  )
{
  UINTN Index;
  UINTN NumOfDays;
  INTN DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  
  NumOfDays = 0;
  for (Index = 1; Index < Time.Month; Index++) {
     NumOfDays += DaysOfMonth[Index - 1];
  }
  NumOfDays += Time.Day;
  return NumOfDays;
}
/*
BOOLEAN
IsLeapYear (
  IN UINT16    Year
  )
{
  if (Year % 4 == 0) {
    if (Year % 100 == 0) {
      if (Year % 400 == 0) {
         return TRUE;
      } else {
         return FALSE;
      }
    } else {
       return TRUE;
    }
  } else {
     return FALSE;
  }
}
*/
UINTN
CountNumOfLeapDays (
   IN EFI_TIME  *Time
   )
{
  UINT16    NumOfYear;
  UINT16    BaseYear;
  UINT16    Index;
  UINTN     Count;
  
  Count      = 0;
  BaseYear  = 1970;
  NumOfYear = Time->Year - 1970;
  
  for (Index = 0; Index <= NumOfYear; Index++) {
    if (IsLeapYear (BaseYear + Index)) {
          Count++;
    }
  }
    
  if ( IsLeapYear( Time->Year ) ) {
    if ( (Count > 0) && (Time->Month < 3) ) {
       Count--;
    }
   }
  return Count;
}

UINT32
EfiSmGetTimeStamp (VOID)
{
  UINT16                  NumOfYears;
  UINTN                   NumOfLeapDays;
  UINTN                   NumOfDays;
  EFI_TIME                Time;
  UINT32                  NumOfSeconds;
  UINT16                  TempUtc=0;
  
  gRT->GetTime (&Time, NULL);
  
  NumOfYears      = Time.Year - 1970;
  NumOfLeapDays   = CountNumOfLeapDays (&Time);
  NumOfDays       = CalculateNumOfDayPassedThisYear (Time);

  //
  // Add 365 days for all years. Add additional days for Leap Years. Subtract off current day.
  //
  NumOfDays += (NumOfLeapDays + (365 * NumOfYears) - 1);
  if((UTC->Utc[1] & BIT7) == 0){
    NumOfSeconds = (UINT32) (3600 * 24 * NumOfDays + (Time.Hour * 3600) + (60 * Time.Minute) + Time.Second - UTC->UctOffset * 60 );
  }else{
    TempUtc = (UINT16)(~UTC->UctOffset + 1);
    NumOfSeconds = (UINT32) (3600 * 24 * NumOfDays + (Time.Hour * 3600) + (60 * Time.Minute) + Time.Second + TempUtc * 60 );
  }

  return NumOfSeconds;
}

VOID
EliminateExtraSpaces (
  IN CHAR8                    *Str
  )
{
  UINTN                        Index;
  UINTN                        ActualIndex;

  for (Index = 0, ActualIndex = 0; Str[Index] != '\0'; Index++) {
    if ((Str[Index] != ' ') || ((ActualIndex > 0) && (Str[ActualIndex - 1] != ' '))) {
      Str[ActualIndex++] = Str[Index];
    }
  }

  if (Str[ActualIndex - 1] == ' ') {
    ActualIndex--;
  }
  Str[ActualIndex] = '\0';
}

BOOLEAN 
IsHygonAhci(EFI_HANDLE SataHandle)
{
  EFI_STATUS					  Status;
  EFI_DEVICE_PATH_PROTOCOL		  *Dp;
  EFI_HANDLE					  PciHandle;
  EFI_PCI_IO_PROTOCOL			  *PciIo;
  UINT32						  PciId;
	  
	  
  Status = gBS->HandleProtocol(SataHandle, &gEfiDevicePathProtocolGuid, &Dp);
  DEBUG((EFI_D_INFO,"gEfiDevicePathProtocolGuid Status = %r\n",Status));
  if(EFI_ERROR(Status)){
    return FALSE;
  }
	
  Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &PciHandle);
  DEBUG((EFI_D_INFO,"gEfiPciIoProtocolGuid Status = %r\n",Status));
  if(EFI_ERROR(Status)){
    return FALSE;
  }
	
  Status = gBS->HandleProtocol(PciHandle, &gEfiPciIoProtocolGuid, &PciIo);
  DEBUG((EFI_D_INFO,"gEfiPciIoProtocolGuid Status = %r\n",Status));
  if(EFI_ERROR(Status)){
    return FALSE;
  }  
	
  PciIo->Pci.Read (
                  PciIo,
				 EfiPciIoWidthUint32,
				 0,
				 1,
				 &PciId
				 );
    DEBUG((EFI_D_INFO, "PciId = %0x\n",PciId));
  if(PciId == 0x79011D94){
    DEBUG((EFI_D_INFO, "Higon AHCI\n"));
    return TRUE;
  }
	
  return FALSE;
}

UINT8
GetPciUid (EFI_HANDLE Handle)
{
  EFI_STATUS				  Status;
  EFI_DEVICE_PATH_PROTOCOL	  *DevicePath;
	
  Status = gBS->HandleProtocol(Handle,
								   &gEfiDevicePathProtocolGuid, 
								   &DevicePath);
  if(!EFI_ERROR(Status)){
    if(DevicePath->Type == 0x02 &&
      DevicePath->SubType == 0x01 &&
      ((ACPI_HID_DEVICE_PATH*)DevicePath)->HID == EISA_PNP_ID(0x0A03)){
        return ((ACPI_HID_DEVICE_PATH*)DevicePath)->UID;
    }
  }
  return 0xff;
}
	
	
#define FCH_SATA_BAR5_REG128                 0x0128  // Port Serial ATA Status
#define FCH_SMN_SATA_CONTROL_BAR5            0x03101000ul
  
STATIC 
UINT8
GetSataSpeed (
	EFI_HANDLE	SataHandle,
	UINTN		PortNum
  )
{
  EFI_STATUS                    Status;
  PLATFORM_COMM_INFO				*Info;
  UINT8							RootBridgeNum;
  UINT8							DieBusNum;
  UINT32							SATA_AHCI_P_SSTS;
  UINT8							P_SCTL_SPD;
  UINT8                         P_SCTL_SPD1;
  UINT32                          Offset;
  UINT8		                    Speed=0;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  UINT32                        SATA_AHCI_P_SSTS1;
  EFI_HANDLE                    PciHandle;
  EFI_DEVICE_PATH_PROTOCOL      *Dp;
	
  UINT8 SataSpeed[] = {
	  0, //NoLink
	  1, //1.5
	  3, 
	  6,
	  0
  };
  
	
  Info = GetPlatformCommInfo();
  
  if(IsHygonAhci(SataHandle)){
  	DEBUG((EFI_D_INFO,"GetSataSpeed True\n"));
    RootBridgeNum = GetPciUid(SataHandle);
    if(RootBridgeNum == 0xFF){
      goto ProcExit;
    }
	  
    DieBusNum = Info->BusBase[RootBridgeNum];
    PciWrite32 (PCI_LIB_ADDRESS (DieBusNum, 0, 0, 0xB8), (UINT32)(FCH_SMN_SATA_CONTROL_BAR5 + FCH_SATA_BAR5_REG128 + PortNum * 0x80));
    SATA_AHCI_P_SSTS = PciRead32 (PCI_LIB_ADDRESS (DieBusNum, 0, 0, 0xBC));
    P_SCTL_SPD = (UINT8)((SATA_AHCI_P_SSTS>>4)&0xF);
    Speed = SataSpeed[P_SCTL_SPD];
  	DEBUG((EFI_D_INFO,"GP_SCTL_SPD = %d\n",P_SCTL_SPD));
	  
  }else {
  	

      Status = gBS->HandleProtocol(SataHandle, &gEfiDevicePathProtocolGuid, &Dp);
      if(EFI_ERROR(Status)){
        return Speed;
      }

      Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &PciHandle);
      if(EFI_ERROR(Status)){
        return Speed;
      }

      Status = gBS->HandleProtocol(PciHandle, &gEfiPciIoProtocolGuid, &PciIo);
        if(EFI_ERROR(Status)){
        return Speed;
      }  

    Offset = 0x100 + (UINT8)PortNum * 0x80 + 0x28;

    PciIo->Mem.Read (
               PciIo,
               EfiPciIoWidthUint32,
               0x5,
               (UINT64) Offset,
               1,
               &SATA_AHCI_P_SSTS1
               );
    P_SCTL_SPD1 = (UINT8)((SATA_AHCI_P_SSTS1>>4)&0xF);
    Speed = SataSpeed[P_SCTL_SPD1];
  }
  ProcExit:
  return Speed;
}

VOID
SendHddInfo2BMC_old( )
{
  EFI_STATUS                      Status;
  EFI_DISK_INFO_PROTOCOL          *DiskInfo;
  UINTN                           HandleCount;
  EFI_HANDLE                      *HandleBuffer;
  UINTN                           Index;
  CHAR8                           *ModelName;
  CHAR8                           *FirmwareVer;
  CHAR8                           *SerialNo;
  CHAR8                           *Module;  
  UINT8                           commanddata[120];
  UINT8                           ResponseData[120];
  UINT32                          ResponseDataSize;  
  UINT64                          DriveSizeInBytes = 0;  
  UINT32                          DriveSizeInGB = 0;
  UINT32                          BufferSize;
  ATA_IDENTIFY_DATA               *IdentifyData = NULL;
  UINT64                          NumSectors = 0;
  UINT64                          RemainderInBytes = 0;
  UINT8                           i,HddNum=0;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  

  Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiDiskInfoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
  if(!EFI_ERROR(Status)){
    IdentifyData = AllocatePool(sizeof(ATA_IDENTIFY_DATA));
    if(IdentifyData == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ProcExit;
    }

    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol(
                    HandleBuffer[Index],
                    &gEfiDiskInfoProtocolGuid,
                    (VOID**)&DiskInfo
                    );
      ASSERT_EFI_ERROR(Status); 
 
      BufferSize = sizeof(ATA_IDENTIFY_DATA);
      Status = DiskInfo->Identify (
                       DiskInfo,
                       IdentifyData,
                       &BufferSize
                       );
      if (EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR,"Identify failed!\n"));   
        continue;
      }

      ModelName = IdentifyData->ModelName;
      FirmwareVer = IdentifyData->FirmwareVer;
      ConvertIdentifyDataFormat(FirmwareVer, AsciiStrSize(FirmwareVer));      
      SerialNo = IdentifyData->SerialNo;
      ConvertIdentifyDataFormat(SerialNo, AsciiStrSize(SerialNo));  
      Module = "SATA";

      if ((!(IdentifyData->config & ATAPI_DEVICE)) || (IdentifyData->config == 0x848A)) {
        if (IdentifyData->command_set_supported_83 & _48_BIT_ADDRESS_FEATURE_SET_SUPPORTED) { 
          NumSectors = *(UINT64 *)&IdentifyData->maximum_lba_for_48bit_addressing; 
        } else {
          NumSectors = IdentifyData->user_addressable_sectors_lo + (IdentifyData->user_addressable_sectors_hi << 16) ; 
        }

        DriveSizeInBytes = MultU64x32(NumSectors, 512); 
        //
        // DriveSizeInGB is DriveSizeInBytes / 1 GB (1 Binary GB = 2^30 bytes)
        // DriveSizeInGB = (UINT32) Div64(DriveSizeInBytes, (1 << 30), &RemainderInBytes); 
        // Convert the Remainder, which is in bytes, to number of tenths of a Binary GB.
        // NumTenthsOfGB = GetNumTenthsOfGB(RemainderInBytes); 
        // DriveSizeInGB is DriveSizeInBytes / 1 GB (1 Decimal GB = 10^9 bytes)
        //
        DriveSizeInGB = (UINT32) DivU64x64Remainder (DriveSizeInBytes, 1000000000, &RemainderInBytes);  

        commanddata[0] = SET_HDD_INFO;         //parameter 4 means hdd info 
        commanddata[1] = HddNum|BIT7;             //Bit7=1 means valid, bit6~0 means HDD number 
        commanddata[2] = (UINT8)(DriveSizeInGB & 0xFF);
        commanddata[3] = (UINT8)((DriveSizeInGB >> 8) & 0xFF); 

        for (i=0;i<AsciiStrSize(Module);i++) {
          commanddata[4+i]=*(Module+i);
        }

        for (i=0;i<AsciiStrSize(ModelName);i++) {
          commanddata[20+i]=*(ModelName+i);
        }

        for (i=0;i<AsciiStrSize(SerialNo);i++) {
          commanddata[52+i]=*(SerialNo+i);
        }

//       for (i=0;i<AsciiStrSize(FirmwareVer);i++) {
        for (i=0;i<8;i++) {
          commanddata[84+i]=*(FirmwareVer+i);
        }

        Status = EfiSendCommandToBMC (
                                     SM_BYOSOFT_NETFN_APP,
		                                 SM_BYOSOFT_NETFN_SUB_FUN,
                                    (UINT8 *) &commanddata,
                                     sizeof(commanddata),
                                    (UINT8 *) &ResponseData,
                                    (UINT8 *) &ResponseDataSize);
        HddNum++;
      }//ata dev
    } //for loop
  } //LocateHandleBuffer

ProcExit:  
  if(IdentifyData != NULL){
    FreePool(IdentifyData);}
  if (HandleBuffer!= NULL){
    FreePool(HandleBuffer);}
}

UINT8 GetAtaMajorVersion(UINT16 Major)
{
	UINT8   Ver = 0;
  UINT8   i;

	if(Major == 0x0000 || Major == 0xFFFF){
		return 0;
	}

	for(i = 14; i > 0; i--){
		if((Major >> i) & 1){
			Ver = i;
			break;
		}
	}

  return Ver;
}

UINTN GetAtaMediaRotationRate(ATA_IDENTIFY_DATA *IdentifyData)
{
 UINT16  Rate;

 if(GetAtaMajorVersion(IdentifyData->major_version_no) < 8){
 return 0;
 }

Rate = IdentifyData->nominal_media_rotation_rate;
if(Rate == 1){
 return 1;
} else if(Rate >= 0x401 && Rate <= 0xFFFE){
 return Rate;
} else {
 return 0;
}
}

CHAR8 *
GetNvmeDescription (
  IN EFI_HANDLE   Handle,
  IN UINT8        NvmeIndex
  )
{
  EFI_STATUS                               Status;
  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL       *NvmePassthru;
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET CommandPacket;
  EFI_NVM_EXPRESS_COMMAND                  Command;
  EFI_NVM_EXPRESS_COMPLETION               Completion;
  NVME_ADMIN_CONTROLLER_DATA               ControllerData;
  CHAR8                                    *Description;
  UINTN                                    Size;

  //
  // Send ADMIN_IDENTIFY command to NVME controller to get the model and serial number.
  //
  Status = gBS->HandleProtocol (Handle, &gEfiNvmExpressPassThruProtocolGuid, (VOID **) &NvmePassthru);
  ASSERT_EFI_ERROR (Status);

  ZeroMem (&CommandPacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Command, sizeof(EFI_NVM_EXPRESS_COMMAND));
  ZeroMem (&Completion, sizeof(EFI_NVM_EXPRESS_COMPLETION));

  Command.Cdw0.Opcode = NVME_ADMIN_IDENTIFY_CMD;
  //
  // According to Nvm Express 1.1 spec Figure 38, When not used, the field shall be cleared to 0h.
  // For the Identify command, the Namespace Identifier is only used for the Namespace data structure.
  //
  Command.Nsid        = 0;
  CommandPacket.NvmeCmd        = &Command;
  CommandPacket.NvmeCompletion = &Completion;
  CommandPacket.TransferBuffer = &ControllerData;
  CommandPacket.TransferLength = sizeof (ControllerData);
  CommandPacket.CommandTimeout = EFI_TIMER_PERIOD_SECONDS (5);
  CommandPacket.QueueType      = NVME_ADMIN_QUEUE;
  //
  // Set bit 0 (Cns bit) to 1 to identify a controller
  //
  Command.Cdw10                = 1;
  Command.Flags                = CDW10_VALID;

  Status = NvmePassthru->PassThru (
                               NvmePassthru,
                               0,
                               &CommandPacket,
                               NULL
                               );
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  
  Size = ARRAY_SIZE (ControllerData.Mn) + 1 + MAXIMUM_VALUE_CHARACTERS + 1 + 7;
  Description = AllocateZeroPool (Size);
  if (Description != NULL) {
    AsciiSPrint(Description, Size, "NVME%d: ", NvmeIndex);
    CopyMem(Description + 7, ControllerData.Mn, ARRAY_SIZE (ControllerData.Mn));
    EliminateExtraSpaces (Description);
  }

  return Description;
}



void
SendNvmeInfo2BMC_Old (
  IN UINT8                *CommandData,
  IN UINT8                *ResponseData
)
{
  UINT8                        ResponseSize;
  HDD_INFO_2_BMC               *NvmeInfo2Bmc;
  UINTN                        Index;
  EFI_STATUS                   Status;
  PLAT_HOST_INFO_PROTOCOL      *ptHostInfo;  
  EFI_DEVICE_PATH_PROTOCOL     *Dp = NULL;
  UINT8                        NvmeIndex = 0;
  EFI_HANDLE                   DeviceHandle;
  CHAR8                        *String;
  CHAR8                        *NvmeData;
  UINTN                        NvmeStringSize = 0;
  
  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (void**)&ptHostInfo);
  if(EFI_ERROR(Status)){
    return;
  }

  NvmeInfo2Bmc = (HDD_INFO_2_BMC*)CommandData;
  NvmeData     = NvmeInfo2Bmc->Data;
  for (Index = 0; Index < ptHostInfo->HostCount; Index ++){
    if (ptHostInfo->HostList[Index].HostType == PLATFORM_HOST_NVME) {
      NvmeIndex ++;

      Dp = ptHostInfo->HostList[Index].Dp;
      Status = gBS->LocateDevicePath (&gEfiNvmExpressPassThruProtocolGuid, &Dp, &DeviceHandle);
      DEBUG ((EFI_D_INFO,"DeviceHandle:%x,line:%d,Status:%r\n",DeviceHandle,__LINE__,Status));
      if (EFI_ERROR (Status)) {
        continue;
      }

      String = GetNvmeDescription (DeviceHandle, NvmeIndex - 1);
      DEBUG ((EFI_D_INFO, "%a\n", String));
      NvmeInfo2Bmc->SataDeviceCount ++;
      if (NvmeStringSize < 180 && String != NULL) {
        AsciiStrCpy (NvmeData + NvmeStringSize, String);
        NvmeStringSize += AsciiStrSize(String);
        FreePool(String);
      }
    }
  }
  DEBUG ((EFI_D_INFO, "NvmeCount:%x\n", NvmeInfo2Bmc->SataDeviceCount));
  NvmeInfo2Bmc->Function = SET_NVME_INFO;
  NvmeStringSize += 2;
  Status = EfiSendCommandToBMC (
             SM_BYOSOFT_NETFN_APP,
             SM_BYOSOFT_NETFN_SUB_FUN,
             CommandData,
             NvmeStringSize,
             ResponseData,
             &ResponseSize
             );
}

VOID
SendTimeInfo2BMC (VOID)
{
  TIME_INFO_2_BMC                       *TIME;
  UINT8                                 *ResponseData;
  UINT8                                 ResponseSize;
  EFI_STATUS                            Status;
  UINT8                                 *CommandData;

  
  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  CommandData = AllocatePool (4);
  TIME = (TIME_INFO_2_BMC*)CommandData;
  ResponseData = AllocatePool (4);
  TIME->time = EfiSmGetTimeStamp();
  DEBUG((EFI_D_INFO, "TIME:%x %x %x %x\n",TIME->ch[0],TIME->ch[1],TIME->ch[2],TIME->ch[3]));
  DEBUG((EFI_D_INFO, "TIME:%lx\n",TIME->time));
  
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_STORAGE,
             EFI_STORAGE_SET_SEL_TIME,
             CommandData,
             4,
             ResponseData,
             &ResponseSize
             );
  DEBUG((EFI_D_INFO, "time Status:%r\n", Status));
  FreePool (ResponseData);
  FreePool (CommandData);
}



VOID
GetUtcOffsetFromBMC (VOID)
{
  UINT8                                 *ResponseData;
  UINT8                                 ResponseSize;
  EFI_STATUS                            Status;

  
  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  ResponseData = AllocatePool (2);

  //Get UTC from BMC
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_STORAGE,
             EFI_STORAGE_GET_SEL_UTC_OFFSET,
             NULL,
             0,
             ResponseData,
             &ResponseSize
             );
  
  DEBUG((EFI_D_INFO, "GetUtc Status:%r\n", Status));
  CopyMem(UTC, ResponseData, 2);
  
  DEBUG((EFI_D_INFO, "GetUTC:%x %x\n", UTC->Utc[0], UTC->Utc[1]));
  DEBUG((EFI_D_INFO, "GetUctOffset:%lx\n",UTC->UctOffset));

  FreePool (ResponseData);
}



VOID
SetUtcOffset2BMC (VOID)
{
  UINT8                                 *ResponseData;
  UINT8                                 ResponseSize;
  EFI_STATUS                            Status;
  UINT8                                 *CommandData;

  
  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  CommandData = AllocatePool (2);
  ResponseData = AllocatePool (2);
  
  CopyMem(CommandData, UTC, 2);
  
  DEBUG((EFI_D_INFO, "SetUTCBefore:%x %x\n", UTC->Utc[0], UTC->Utc[1]));
  
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_STORAGE,
             EFI_STORAGE_SET_SEL_UTC_OFFSET,
             CommandData,
             2,
             ResponseData,
             &ResponseSize
             );
  DEBUG((EFI_D_INFO, "SetUtc Status:%r\n", Status));

  FreePool (ResponseData);
  FreePool (CommandData);
}

VOID
CollectBiosInfo (
  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_SMBIOS_HANDLE            SmbiosHandle;
  EFI_SMBIOS_TYPE              SmbiosType;
  EFI_SMBIOS_TABLE_HEADER      *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER     p;
  BIOS_VERSION_2_BMC           *BiosVer2Bmc;
  CHAR8                        BiosDate[20];
  CHAR8                        *BiosVer;
  UINT8                        StrLength;
  EFI_SMBIOS_PROTOCOL          *Smbios;
  UINT8                        *CommandData;
  BIOS_ID_IMAGE                BiosIdImage;
  CHAR16                       *q;
  CHAR8                        *s;
  
  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (Status != EFI_SUCCESS) {
    DEBUG((EFI_D_INFO, "LocateProtocol Smbios Status = %r\n", Status));
    return;
  }

  CommandData = AllocateZeroPool (BIOS_2_BMC_DATA_SIZE);
  pBiosinfo = CommandData;
  
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_BIOS_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr    = (SMBIOS_STRUCTURE*)SmbiosHdr;
  BiosVer  = SmbiosGetStringInTypeByIndex(p, p.Type0->BiosVersion);
  //BiosDate = SmbiosGetStringInTypeByIndex(p, p.Type0->BiosReleaseDate);
  
  Status = GetBiosId(&BiosIdImage);  
  q = BiosIdImage.BiosIdString.TimeStamp;     // YYMMDDHHMM -> "05/02/2018"
  s = BiosDate;
  s[0] = '2';
  s[1] = '0';
  s[2] = (CHAR8)q[0];
  s[3] = (CHAR8)q[1];
  s[4] = '-';
  s[5] = (CHAR8)q[2];
  s[6] = (CHAR8)q[3];
  s[7] = '-';
  s[8] = (CHAR8)q[4];
  s[9] = (CHAR8)q[5];
  s[10]= ' ';
  s[11]= (CHAR8)q[6];
  s[12]= (CHAR8)q[7];
  s[13]= ':';
  s[14]= (CHAR8)q[8];
  s[15]= (CHAR8)q[9];
  s[16]= ':';
  s[17]= '4';
  s[18]= '8';
  s[19]= 0;

  BiosVer2Bmc = (BIOS_VERSION_2_BMC*)CommandData;
  BiosVer2Bmc->Function = SET_BIOS_VER;
  BiosVer2Bmc->BiosVerOffset = OFFSET_OF (BIOS_VERSION_2_BMC, Data);
  StrLength = (UINT8)AsciiStrSize(BiosVer);
  AsciiStrCpy (BiosVer2Bmc->Data, BiosVer);

  BiosVer2Bmc->BiosDateOffset = BiosVer2Bmc->BiosVerOffset + StrLength;
  AsciiStrCpy (&(BiosVer2Bmc->Data[StrLength]), BiosDate);

  StrLength += sizeof (BIOS_VERSION_2_BMC) + (UINT8)AsciiStrSize(BiosDate);

  gBiosinfoLen = StrLength;

  DEBUG((EFI_D_INFO, "pBiosinfo = 0x%lx\n", pBiosinfo));
  DEBUG((EFI_D_INFO, "gBiosinfoLen = 0x%x\n", gBiosinfoLen));

}

VOID
CollectPciInfo (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINT8                                     Index;
  UINTN                                     HandleCount, Segment, Bus, Device, Function;
  EFI_HANDLE                                *HandleBuffer;
  EFI_PCI_IO_PROTOCOL                       *PciIo;
  UINTN                                     BridgeSegment, BridgeBus, BridgeDevice, BridgeFunction;
  UINT16                                    VenderId, DeviceId, SubsystemId, SubsystemVendorId;
  UINT32                                    VidDid;
  UINT8                                     ClassCode[3];
  UINT8                                     PciInfor2Bmc[32];
  UINT8                                     ResponseSize=1;
  UINT8                                     ResponseData[32];
  UINT8                                     Index2 =0;
  UINT8                                     RevisionId;  
  EFI_DEVICE_PATH_PROTOCOL                  *PciDp;
  PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH   SlotDp;
  EFI_PCI_IO_PROTOCOL                       *BridgePciIo;
  UINT32                                    LinkStatus, LinkCap, LinkCap2, DevCtrl, LinkCtrl;
  UINTN                                     CurLinkWidth, SupportLinkSpeedVector, MaxLinkSpeed;
  UINTN                                     CurLinkSpeed, MaxPayLoadSize, MaxLinkWidth, MaxReadReqSize;
  UINT8                                     Aspm, CapabilityPtr = 0, CapabilityID = 0, PcieCapabilityPtr = 0;
  UINT16                                    CapabilityEntry = 0;
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath;
  CHAR16                                   *Str;
  CHAR16                                   *Alloc = NULL;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL         *ptDP2Txt;
  EFI_HANDLE                                DiskDevHandle;
  BYO_DISKINFO_PROTOCOL                     *DiskInfo = NULL;
  UINT8                                     *pTemppPciinfo = NULL;
  
  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  
  pPciinfo = AllocateZeroPool(32 * HandleCount);
  pTemppPciinfo = pPciinfo;
  
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    ASSERT_EFI_ERROR (Status);
    
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, 9, 3, &ClassCode);
    PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
    if (ClassCode[2] == 0x06 || ClassCode[2] == 0x08 || ClassCode[2] > 0x0F || Bus == 0x04 || Function != 0) { // skip bridge + Base system peripherals + Encryption/Decryption controllers + vga control
      continue;
    }

    PcieCapabilityPtr = 0;
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CAPBILITY_POINTER_OFFSET, 1, &CapabilityPtr);
    while ((CapabilityPtr >= 0x40) && ((CapabilityPtr & 0x03) == 0x00)) {
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
        LinkStatus = LinkStatus >> 16;
        DEBUG((EFI_D_ERROR, __FUNCTION__"(), LinkStatus :0x%x.\n", LinkStatus));

        PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, PcieCapabilityPtr + 0x0c, 1, &LinkCap);
        DEBUG((EFI_D_ERROR, __FUNCTION__"(), LinkCap :0x%x.\n", LinkCap));

        CurLinkWidth = (LinkStatus >> 4) & 0x3F;
        MaxLinkWidth = (LinkCap >> 4) & 0x3F;

        DEBUG((EFI_D_ERROR, __FUNCTION__"(), CurLinkWidth:%d, MaxLinkWidth :%d.\n", CurLinkWidth, MaxLinkWidth));		
    }

    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID**)&PciDp);
    if (!EFI_ERROR (Status)) {
      CopyMem(&SlotDp, PciDp, sizeof(PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH));
      CopyMem(&SlotDp.End, &gEndDp, sizeof(EFI_DEVICE_PATH_PROTOCOL));
      ShowDevicePathDxe(gBS, (EFI_DEVICE_PATH_PROTOCOL*)&SlotDp);
      BridgePciIo = NULL;
      BridgePciIo = (EFI_PCI_IO_PROTOCOL*)LibGetPciIoFromDp(gBS, (EFI_DEVICE_PATH_PROTOCOL*)&SlotDp);
      if(BridgePciIo != NULL){
        BridgePciIo->GetLocation (BridgePciIo, &BridgeSegment, &BridgeBus, &BridgeDevice, &BridgeFunction);

        BridgePciIo->Pci.Read (BridgePciIo, EfiPciIoWidthUint32, 0x58 + 0x10, 1, &LinkStatus);
        LinkStatus = LinkStatus >> 16;

        BridgePciIo->Pci.Read (BridgePciIo, EfiPciIoWidthUint32, 0x58 + 0x0c, 1, &LinkCap);

        BridgePciIo->Pci.Read (BridgePciIo, EfiPciIoWidthUint32, 0x58 + 0x2c, 1, &LinkCap2);
        BridgePciIo->Pci.Read (BridgePciIo, EfiPciIoWidthUint32, 0x58 + 0x08, 1, &DevCtrl);
        BridgePciIo->Pci.Read (BridgePciIo, EfiPciIoWidthUint32, 0x58 + 0x10, 1, &LinkCtrl);

        // Supported Link Speeds Vector 
        //   Bit 0  2.5 GT/s 
        //   Bit 1  5.0 GT/s 
        //   Bit 2  8.0 GT/s 
        CurLinkSpeed = LinkStatus & 0xF;
        //CurLinkWidth = (LinkStatus >> 4) & 0x3F;
        SupportLinkSpeedVector = (LinkCap2 >> 1) & 0x7F;  
        MaxLinkSpeed = LinkCap & 0xF;
        //MaxLinkWidth = (LinkCap >> 4) & 0x3F;
        MaxPayLoadSize = 128 << ((DevCtrl >> 5) & 7);
        MaxReadReqSize = 128 << ((DevCtrl >> 12) & 7);
        Aspm = (UINT8)(LinkCtrl & 0x3);

        if(CurLinkSpeed >= 1 && CurLinkSpeed <= 7){
          CurLinkSpeed = ((UINTN)1 << (CurLinkSpeed - 1)) & SupportLinkSpeedVector;
          if (CurLinkSpeed == 4) {
            CurLinkSpeed = 3;
          }
        }
        if(MaxLinkSpeed >= 1 && MaxLinkSpeed <= 7){
          MaxLinkSpeed = ((UINTN)1 << (MaxLinkSpeed - 1)) & SupportLinkSpeedVector;
          if (MaxLinkSpeed == 4) {
            MaxLinkSpeed = 3;
          }
        }
        DEBUG((EFI_D_ERROR, __FUNCTION__"(), CurLinkSpeed:%d, MaxLinkSpeed :%d.\n", CurLinkSpeed, MaxLinkSpeed));		
      }
    }

    PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
    DEBUG((EFI_D_ERROR, __FUNCTION__"(), BDF :%d-%d-%d.\n", Bus, Device, Function));
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, 1, &VidDid);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0, 1, &VenderId);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0x02, 1, &DeviceId);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8,  0x08, 1, &RevisionId);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0x2C, 1, &SubsystemVendorId);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0x2E, 1, &SubsystemId);
  	//if (Bus == 0x00 || Bus == 0x03 || Bus == 0x08 || 
  	//	Bus == 0x80 || Bus == 0x86) { // skip onboard pci device
    //  continue;
    //  }
    if ((VidDid&0xFFFF) == 0x1D94) { // skip bridge + Base system peripherals + Encryption/Decryption controllers
      continue;
    }
    if (VidDid == 0x20001A03 || VidDid == 0x06221B21 ) { 
	  continue;
    }

    ZeroMem (&PciInfor2Bmc[0], 32);
    ZeroMem (ResponseData, 32);

    PciInfor2Bmc[0] = SET_PCI_INFO;
    PciInfor2Bmc[1] = Index2;
    CopyMem (&PciInfor2Bmc[2], (UINT8*)&VenderId, 2);
    CopyMem (&PciInfor2Bmc[4], (UINT8*)&DeviceId, 2);
    PciInfor2Bmc[6] = (UINT8)Bus;
    PciInfor2Bmc[7] = (UINT8)Device;
    PciInfor2Bmc[8] = (UINT8)Function;
    CopyMem (&PciInfor2Bmc[9], (UINT8*)ClassCode, 3);
    PciInfor2Bmc[12] = (UINT8)ClassCode[2];
    PciInfor2Bmc[13] = (UINT8)Function;
    PciInfor2Bmc[14] = 0;  //Physical
    PciInfor2Bmc[15] = (UINT8)RevisionId;
    CopyMem (&PciInfor2Bmc[16], (UINT8*)&SubsystemVendorId, 2);
    CopyMem (&PciInfor2Bmc[18], (UINT8*)&SubsystemId, 2);

    PciInfor2Bmc[20] = (UINT8) MaxLinkWidth;//MaxLinkWidth;
    PciInfor2Bmc[21] = (UINT8) MaxLinkSpeed;//MaxLinkSpeed;
    PciInfor2Bmc[22] = (UINT8) CurLinkWidth;//CurrentLinkWidth;
    PciInfor2Bmc[23] = (UINT8) CurLinkSpeed;//CurrentLinkSpeed;

    Status = gBS->HandleProtocol(
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid, 
                    &DevicePath
                    );
    Status = gBS->LocateProtocol(
                  &gEfiDevicePathToTextProtocolGuid,
                  NULL,
                  &ptDP2Txt
                  );

    if (DevicePath->Type == END_DEVICE_PATH_TYPE &&
        DevicePath->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE) {
      Str = L"(End)";
    }else{
      Str = ptDP2Txt->ConvertDevicePathToText (
                      DevicePath,
                      FALSE,
                      TRUE
                      );
      Alloc = Str;
    }

    Status = gBS->LocateDevicePath(&gByoDiskInfoProtocolGuid, &DevicePath, &DiskDevHandle);
    DEBUG((EFI_D_INFO,"Status = %r\n", Status));
    if (Status == EFI_SUCCESS) {
      Status = gBS->HandleProtocol(DiskDevHandle, &gByoDiskInfoProtocolGuid, &DiskInfo);
    }

	//if(DiskInfo->DevType ==BYO_DISK_INFO_DEV_NVME){
    //  continue;
	//}
	if(ClassCode[2]==1&&ClassCode[1]==8&&ClassCode[0]==2){
        continue;
	}
    DEBUG((EFI_D_INFO,"Bus = %0x,dev=%0x,dp:%s\n",Bus,Device,Str));
    if(0 == StrCmp(L"PciRoot(0x0)/Pci(0x1,0x3)/Pci(0x0,0x0)",Str)){ //OnBoard WX net card  (Ocp is NVME)
      PciInfor2Bmc[24] = 0x6;//Slot
      PciInfor2Bmc[25] = 0xff;
      PciInfor2Bmc[26] = 0xff;
    }else if(0 == StrCmp(L"PciRoot(0x0)/Pci(0x1,0x2)/Pci(0x0,0x0)",Str)){ //OnBoard WX net card  (Ocp is sata or OCP)
      PciInfor2Bmc[24] = 0x6;//Slot
      PciInfor2Bmc[25] = 0xff;
      PciInfor2Bmc[26] = 0xff;
    }else if(0 == StrCmp(L"PciRoot(0x1)/Pci(0x3,0x1)/Pci(0x0,0x0)",Str)){ //PCIE0
      PciInfor2Bmc[24] = 0;//Slot
      PciInfor2Bmc[25] = 0xff;
      PciInfor2Bmc[26] = 0xff;
      if (DiskInfo != NULL) {
        if(DiskInfo->DevType == BYO_DISK_INFO_DEV_NVME){
          PciInfor2Bmc[24] = 5;//Slot
          PciInfor2Bmc[25] = 3;
          PciInfor2Bmc[26] = 0xff;
        }
      }
    }else if(0 == StrCmp(L"PciRoot(0x3)/Pci(0x3,0x2)/Pci(0x0,0x0)",Str)){ // PCIE1
      PciInfor2Bmc[24] = 1;
      PciInfor2Bmc[25] = 0xff;
      PciInfor2Bmc[26] = 0xff;
      if (DiskInfo != NULL) {
        if(DiskInfo->DevType == BYO_DISK_INFO_DEV_NVME){
          PciInfor2Bmc[24] = 4;//Slot
          PciInfor2Bmc[25] = 4;
          PciInfor2Bmc[26] = 0xff;
        }
      }
    }else if(0 == StrCmp(L"PciRoot(0x3)/Pci(0x3,0x1)/Pci(0x0,0x0)",Str)){//PCIE2
      PciInfor2Bmc[24] = 2;//Slot
      PciInfor2Bmc[25] = 0xff;
      PciInfor2Bmc[26] = 0xff;
      if (DiskInfo != NULL) {
        if(DiskInfo->DevType == BYO_DISK_INFO_DEV_NVME){
          PciInfor2Bmc[24] = 4;//Slot
          PciInfor2Bmc[25] = 5;
          PciInfor2Bmc[26] = 0xff;
        }
      }
    }else if(0 == StrCmp(L"PciRoot(0x2)/Pci(0x1,0x1)/Pci(0x0,0x0)",Str)){//PCIE3
      PciInfor2Bmc[24] = 3;
      PciInfor2Bmc[25] = 0xff;
      PciInfor2Bmc[26] = 0xff;
      if (DiskInfo != NULL) {
        if(DiskInfo->DevType == BYO_DISK_INFO_DEV_NVME){
          PciInfor2Bmc[24] = 4;//Slot
          PciInfor2Bmc[25] = 5;
          PciInfor2Bmc[26] = 0xff;
        }
      }
    }else if(0 == StrCmp(L"PciRoot(0x1)/Pci(0x3,0x2)/Pci(0x0,0x0)",Str)){//Raid
      PciInfor2Bmc[24] = 4;
      PciInfor2Bmc[25] = 0xff;
      PciInfor2Bmc[26] = 0xff;
      if (DiskInfo != NULL) {
        if(DiskInfo->DevType == BYO_DISK_INFO_DEV_NVME){
          PciInfor2Bmc[24] = 4;//Slot
          PciInfor2Bmc[25] = 6;
          PciInfor2Bmc[26] = 0xff;
        }
      }
    } else if(0 == StrCmp(L"PciRoot(0x0)/Pci(0x1,0x1)/Pci(0x0,0x0)",Str)){ // Slimline Ocp
      PciInfor2Bmc[24] = 5;
      PciInfor2Bmc[25] = 0xff;
      PciInfor2Bmc[26] = 0xff;
    }else {
      PciInfor2Bmc[24] = 0xff;
      PciInfor2Bmc[25] = 0xff;
      PciInfor2Bmc[26] = 0xff;
    }

    PciInfor2Bmc[27] = (UINT8)BridgeBus;
    PciInfor2Bmc[28] = (UINT8)BridgeDevice;
    PciInfor2Bmc[29] = (UINT8)BridgeFunction;

    DEBUG((EFI_D_INFO, "PciInfor2Bmc[24] = %d,PciInfor2Bmc[25] = %d, PciInfor2Bmc[26] = %d,DiskInfo->DevType = %0x\n",PciInfor2Bmc[24],PciInfor2Bmc[25],PciInfor2Bmc[26],DiskInfo->DevType));

    CopyMem(pTemppPciinfo, PciInfor2Bmc, 32);
    pTemppPciinfo = pTemppPciinfo + 32;
    gPciinfoLen = gPciinfoLen + 32;
    Index2 ++;

  }

  DEBUG((EFI_D_INFO, "pPciinfo = 0x%lx\n", pPciinfo));
  DEBUG((EFI_D_INFO, "gPciinfoLen = 0x%lx\n", gPciinfoLen));

  if (HandleBuffer != NULL){
    FreePool (HandleBuffer);
  }
}

CHAR8 *
GetProcessName (
  IN      CHAR8                   *OptionalStrStart
  )
{
  UINTN          OptionalStrSize,StrSize1,StrSize2,StrSize3;
  UINT8 StrCount=1,Index=0,Index2=0;
  CHAR8 *Str;

  OptionalStrSize = 0;
  StrSize1 = 0;
  StrSize2 = 0;
  StrSize3 = 0;
  while (OptionalStrStart[OptionalStrSize] != 0){
  	DEBUG((EFI_D_INFO,"OptionalStrSize = %d,StrSize1 = %d,StrSize2 = %d,StrSize3 = %d\n",OptionalStrSize,StrSize1,StrSize2,StrSize3));
	if(OptionalStrStart[OptionalStrSize] != ' '){
  	  if(StrCount== 1){
  	    StrSize1 ++;
      }
  	  if(StrCount == 2){
  	    StrSize2 ++;
      }
  	  if(StrCount == 3 ){
        StrSize3 ++;
      }
	}else if(OptionalStrStart[OptionalStrSize] == ' '){
	  StrCount ++;
	}
	if(StrCount == 4){
	  break;
	}
	OptionalStrSize++;
  }
  Str= AllocateZeroPool ((StrSize1+StrSize3+1) * sizeof (CHAR16));
  for(Index=0;Index<StrSize1;Index++){
  	Str[Index] = OptionalStrStart[Index];
  }
  Str[Index] = '_';
  for(Index2=0;Index2<StrSize3;Index2++){
  	Str[Index+Index2+1] = OptionalStrStart[StrSize1+1+StrSize2+1+Index2];
  }
  Str[Index+Index2+1] =0;

  return Str;
}

VOID
CollectCpuInfo (
  VOID
  )
{
  UINT8                             DeviceId = 0, i, Index = -1, ProcessorType, Tmp, Count; 
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_TYPE                   SmbiosType;
  EFI_SMBIOS_TABLE_HEADER           *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER          p;
  EFI_STATUS                        Status;
  UINT8                             *CpuInfor2Bmc;
  
  CHAR8                             *InstructionSet, *ProcessorArchitecture, *Manufacture;
  CHAR8                             *Health, *DeviceLocator, *Position, *ProcessorVersion;
  CHAR8                             SerialNumber[9];
  UINT16                            MaxSpeed, CurSpeed;
  UINT8                             CoreCount, ThreadCount, socket;
  UINT32                            CpuId, ReturnValues=0;
  UINTN                             L1CacheSize, L2CacheSize, L3CacheSize;
  EFI_SMBIOS_HANDLE                 L1CacheHandle, L2CacheHandle, L3CacheHandle;
  CHAR8                             *CpuName[]={"CPU 0","CPU 1"};
  UINT8                             *pTempCpuinfo = NULL;
  EFI_SMBIOS_PROTOCOL               *Smbios;
  HYGON_PROCESSOR_SN_PROTOCOL       *ProcessorSNProtocol;
  DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB       *PciePlatformConfigHobData;
  PCIe_PLATFORM_CONFIG                *Pcie;
  GNB_HANDLE                          *GnbHandle;
  UINT32                               SmuArg[6];
  UINTN                                CpuTdp=0;
  CHAR8 *ProcessorName;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (Status != EFI_SUCCESS) {
    DEBUG((EFI_D_INFO, "LocateProtocol Smbios Status = %r\n", Status));
    return;
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;

    ProcessorSNProtocol = NULL;
    Status = gBS->LocateProtocol (
                    &gHygonProcessorServicesProtocolGuid,
                    NULL, 
                    &ProcessorSNProtocol
                    );
	
	Status = gBS->LocateProtocol (&gAmdNbioPcieServicesProtocolGuid,NULL, &PcieServicesProtocol); 
    if (Status == EFI_SUCCESS) {
      PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **) &PciePlatformConfigHobData);
      Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
      GnbHandle = NbioGetHandle (Pcie); 
      NbioSmuServiceCommonInitArgumentsV9 (SmuArg);
      Status = NbioSmuServiceRequestV9 (NbioGetHostPciAddress (GnbHandle), SMC_MSG_TDP_TJMAX, SmuArg, 0);
      DEBUG((EFI_D_INFO, "UpdateCpuInfo TDP/TJMAX = 0x%x\n", SmuArg[0]));
      CpuTdp = SmuArg[0] >> 16;
      DEBUG((EFI_D_INFO, "CpuTdp %d\n", CpuTdp));
	  
    }

  //
  //CPU max number is 2
  //
  pCpuinfo = AllocateZeroPool(160 * 2);
  pTempCpuinfo = pCpuinfo;

  
  while(DeviceId<2){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
	DEBUG((EFI_D_INFO,"Type4 status = %r,SmbiosHandle = %0x\n",Status,SmbiosHandle));
    if(EFI_ERROR(Status)){
      break;
    }
    p.Hdr   = SmbiosHdr;
    ProcessorType = p.Type4->ProcessorType;
    //CpuName = SmbiosGetStringInTypeByIndex(p, p.Type4->ProcessorVersion);
    //Manufacture =SmbiosGetStringInTypeByIndex(p, p.Type4->ProcessorManufacture);
	DEBUG((EFI_D_INFO,"Type4 MaxSpeed = %d\n",p.Type4->MaxSpeed ));
    if( p.Type4->MaxSpeed == 0){
      continue;   //cpu not present that we shouldn't report to bmc for saving boot time.
    }
    CpuInfor2Bmc = AllocateZeroPool(160);
    //ZeroMem(CpuInfor2Bmc, 160);
    Index++;
    MaxSpeed = p.Type4->MaxSpeed;
    CoreCount = p.Type4->CoreCount;
    ThreadCount = p.Type4->ThreadCount;
    socket = DeviceId+1;
    L1CacheHandle = p.Type4->L1CacheHandle;
    L1CacheSize = GetL1CacheSize(Smbios, L1CacheHandle); // send kb
    L2CacheHandle = p.Type4->L2CacheHandle;
    L2CacheSize = GetCacheSize(Smbios, L2CacheHandle);
    L3CacheHandle = p.Type4->L3CacheHandle;
    L3CacheSize = GetCacheSize(Smbios, L3CacheHandle);
    CurSpeed      = p.Type4->CurrentSpeed;

    CpuInfor2Bmc[0] = SET_CPU_INFO;
    CpuInfor2Bmc[1] = DeviceId;

    if(DeviceId == 0){
      for (i=0;i<AsciiStrSize(CpuName[0]);i++) {
        CpuInfor2Bmc[2+i]=*(CpuName[0]+i);
      }
    } else {
      for (i=0;i<AsciiStrSize(CpuName[1]);i++) {
        CpuInfor2Bmc[2+i]=*(CpuName[1]+i);
      }
    }

    CpuInfor2Bmc[18] = Index;
    CpuInfor2Bmc[19] = ProcessorType;

    ProcessorArchitecture = "x86";
    for (i=0;i<AsciiStrSize(ProcessorArchitecture);i++) {
      CpuInfor2Bmc[20+i]=*(ProcessorArchitecture+i);
    }

    InstructionSet =  "x86-64";
    for (i=0;i<AsciiStrSize(InstructionSet);i++) {
      CpuInfor2Bmc[28+i]=*(InstructionSet+i);
    }

    Manufacture = "Hygon";
    for (i=0;i<AsciiStrSize(Manufacture);i++) {
      CpuInfor2Bmc[36+i]=*(Manufacture+i);
    }

    ProcessorVersion = SmbiosGetStringInTypeByIndex(p, p.Type4->ProcessorVersion);
    EliminateExtraSpaces8(ProcessorVersion);
	ProcessorName = GetProcessName(ProcessorVersion); //Hygon_XXXX
    for (i=0;i<AsciiStrSize(ProcessorName);i++) {
      CpuInfor2Bmc[52+i]=*(ProcessorName+i);
    }

    CpuId = *(UINT32*)&(p.Type4->ProcessorId.Signature);
    CopyMem (&CpuInfor2Bmc[92], (UINT8*)&CpuId, 4);

    Tmp = CpuInfor2Bmc[92];
    CpuInfor2Bmc[92] = CpuInfor2Bmc[95];
    CpuInfor2Bmc[95] = Tmp;
    Tmp = CpuInfor2Bmc[93];
    CpuInfor2Bmc[93] = CpuInfor2Bmc[94];
    CpuInfor2Bmc[94] = Tmp;

    CopyMem (&CpuInfor2Bmc[96], (UINT8*)&MaxSpeed, 2); 
    CpuInfor2Bmc[98] = CoreCount;
    CpuInfor2Bmc[99] = ThreadCount;
    CpuInfor2Bmc[100] = socket;

    Health = "OK";
    for (i=0;i<AsciiStrSize(Health);i++) {
      CpuInfor2Bmc[101+i]=*(Health+i);
    }

    CopyMem (&CpuInfor2Bmc[109], (UINT8*)&L1CacheSize, 2); 
    CopyMem (&CpuInfor2Bmc[111], (UINT8*)&L2CacheSize, 2); 
    CopyMem (&CpuInfor2Bmc[113], (UINT8*)&L3CacheSize, 2); 

    if(socket == 1 ){
      DeviceLocator = "CPU0";
    }else {
      DeviceLocator = "CPU1";
    }
    for (i=0;i<AsciiStrSize(DeviceLocator);i++) {
      CpuInfor2Bmc[115+i]=*(DeviceLocator+i);
    }

    CopyMem (&CpuInfor2Bmc[123], (UINT8*)&CurSpeed, 2); 

    Position =  "mainboard";
    for (i=0;i<AsciiStrSize(Position);i++) {
      CpuInfor2Bmc[125+i]=*(Position+i);
    }

    //SerialNumber = SmbiosGetStringInTypeByIndex(p, p.Type4->SerialNumber);
    if (ProcessorSNProtocol == NULL) {
      ReturnValues = 0;
    } else {
      if (DeviceId ) {
        DeviceId= 1;
      }
      Status = ProcessorSNProtocol->GetProcessorSN(DeviceId,&ReturnValues);
    }
    DEBUG((EFI_D_INFO,"Status = %r,deviceid = %x,ReturnValues = 0x%x\n",Status,DeviceId,ReturnValues));

    if (Status == EFI_SUCCESS) {
      Count = sizeof(ReturnValues)/sizeof(UINT8);
      for(Tmp=0;Tmp<Count;Tmp++){
        DEBUG((EFI_D_INFO,"Tmp = %d,values = %x\n",Tmp,(UINT8)(ReturnValues >> (Count-1-Tmp)*8)));
        HexToAscii((UINT8)(ReturnValues >>((Count-1-Tmp)*8)), &SerialNumber[Tmp*2]);
      }
      for (i=0;i<AsciiStrSize(SerialNumber);i++) {
        CpuInfor2Bmc[141+i]=*(SerialNumber+i);
      }
      CpuInfor2Bmc[149]=0;
    } else {
      CpuInfor2Bmc[141]=' ';
      CpuInfor2Bmc[142]=0;
    }  

	CpuInfor2Bmc[157]=(UINT8)CpuTdp;


    CopyMem(pTempCpuinfo, CpuInfor2Bmc, 160);
    pTempCpuinfo = pTempCpuinfo + 160;
    gCpuinfoLen = gCpuinfoLen + 160;
    DeviceId ++;
	FreePool(CpuInfor2Bmc);

  }
  DEBUG((EFI_D_INFO,"pCpuinfo = 0x%lx\n", pCpuinfo));
  DEBUG((EFI_D_INFO,"gCpuinfoLen = 0x%x\n", gCpuinfoLen));
}


VOID
CollectDimmInfo (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  EFI_SMBIOS_TYPE                 SmbiosType;
  EFI_SMBIOS_TABLE_HEADER         *SmbiosHdr;
  EFI_SMBIOS_PROTOCOL             *Smbios;
  SMBIOS_STRUCTURE_POINTER        p;
  UINT8                           *commanddata = NULL;  
  UINT8                           DIMMnum= 0, Dimmtype= 4, Tmp=0;
  UINT16                          DimmFreq, MaxFreq;
  UINT32                          SingleSize;
  CHAR8                           DimmName[7] = {"DIMM00"};
  CHAR8                           *Manufacturer, *PN, *SN, *Health;
  UINT8                           i, Index = -1, DeviceId = 0, DataWidth, DevWidth;
  CHAR8                           DeviceType[5] = {"DDR4"};
  CHAR8                           *DeviceLocator, *Dw;
  UINT8                           *pTempDimminfo = NULL;
  CHAR8                           Locator[8];

  CHAR8 *BaseModuleType[] = {
    "RDIMM","UDIMM","LDIMM","SO_DIMM","other"
  };

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (Status != EFI_SUCCESS) {
    DEBUG((EFI_D_INFO, "LocateProtocol Smbios Status = %r\n", Status));
    return;
  }
  //
  //The max number is 32
  //
  pDimminfo = AllocateZeroPool(120 * 32);
  pTempDimminfo = pDimminfo;

  commanddata = AllocateZeroPool(120);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_MEMORY_DEVICE; 
  while(1) {
    ZeroMem(commanddata, 120);
    Index++;
    
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
    p.Hdr = SmbiosHdr;
    DimmFreq = p.Type17->ConfiguredMemoryClockSpeed;
    MaxFreq = p.Type17->Speed;
    SingleSize = p.Type17->Size;
    if(!SingleSize){
       continue; // Dimm not installed 
    }
    commanddata[0] = SET_DIMM_INFO;
    commanddata[1] = Index;

    DimmName[4]='0' + Index/10;
    DimmName[5]='0' + Index%10;
    for (i=0;i<AsciiStrSize(DimmName);i++) {
      commanddata[2+i]=*(DimmName+i);
    }

    for (i=0;i<AsciiStrSize(DimmName);i++) {
      commanddata[10+i]=*(DimmName+i);
    }

    if(SingleSize == 0 || SingleSize == 0xFFFF){
      SingleSize = 0;
    } else if(SingleSize == 0x7FFF){
      SingleSize = p.Type17->ExtendedSize & (~BIT31);	 // MB
    } else {
      if(SingleSize & BIT15){							 // unit in KB.
        SingleSize = (SingleSize&(~BIT15)) >> 10;		// to MB
      }else{											// unit in MB.
      }
    }

    commanddata[18] = SingleSize/0x400;                // /1024 -> GB 
    commanddata[19] = SingleSize%0x400;                // /1024 -> GB 

    Manufacturer = SmbiosGetStringInTypeByIndex(p, p.Type17->Manufacturer);
    for (i=0;i<AsciiStrSize(Manufacturer);i++) {
      commanddata[20+i]=*(Manufacturer+i);
    }

    commanddata[40] = (UINT8)(MaxFreq & 0xFF);        // low 8 bit  
    commanddata[41] = (UINT8)((MaxFreq >> 8) & 0xFF); //high 8 bit

    commanddata[42] = (UINT8)(DimmFreq & 0xFF);        // low 8 bit  
    commanddata[43] = (UINT8)((DimmFreq >> 8) & 0xFF); //high 8 bit
    
    SN = SmbiosGetStringInTypeByIndex(p, p.Type17->SerialNumber);
    for (i=0;i<AsciiStrSize(SN);i++) {
      commanddata[44+i]=*(SN+i);
    } 
    commanddata[55]='\0';

    for (i=0;i<AsciiStrSize(DeviceType);i++) {
      commanddata[56+i]=*(DeviceType+i);
    }

    if(p.Type17->DataWidth > 0xff){
      DataWidth = (UINT8)(p.Type17->DataWidth & 0xFF);
    }else {
      DataWidth = (UINT8)(p.Type17->DataWidth);
    }
	
    DevWidth = 0;
    Dw = LibSmbiosGetTypeEnd(p.Type17) - 4;
    if(Dw[0] == 'W' && Dw[1] >= '0' && Dw[1] <= '9' && Dw[2] >= '0' && Dw[2] <= '9'){
      DevWidth = (Dw[1] - '0') * 10 + (Dw[2] - '0');
    }
    commanddata[64] = DevWidth;
    commanddata[65] = (p.Type17->Attributes)&0x7; //rank count
	
    PN = SmbiosGetStringInTypeByIndex(p, p.Type17->PartNumber);
	
    for (i=0;i<AsciiStrSize(PN);i++) {
      if(*(PN+i) != ' '){
        commanddata[66+i]=*(PN+i);
      }else{
        *(PN+i) = '\0';
      }
    }
    ZeroMem(Locator, 8);
    Tmp = 0;
    DeviceLocator = SmbiosGetStringInTypeByIndex(p, p.Type17->DeviceLocator); //DIMM 0/1   
    for (i=0;i<AsciiStrSize(DeviceLocator);i++) {
      if (i<5 || i>8) {
        Locator[Tmp++]=*(DeviceLocator+i);
      }
    }
    for (i=0;i<AsciiStrSize(Locator);i++) {
      commanddata[86+i]=*(Locator+i);
    }
    commanddata[93]='\0';

    if(p.Type17->TypeDetail.Registered == 1 &&          //
      p.Type17->TypeDetail.LrDimm == 0  &&             // RDIMM
      p.Type17->FormFactor == 9){                      //
      for (i=0;i<AsciiStrSize(BaseModuleType[0]);i++) {
        commanddata[94+i]=*(BaseModuleType[0]+i);
      }
    } else if (p.Type17->TypeDetail.Unbuffered == 1 &&  //
             p.Type17->FormFactor == 9) {             // UDIMM 
      for (i=0;i<AsciiStrSize(BaseModuleType[1]);i++) { //
        commanddata[94+i]=*(BaseModuleType[1]+i);
      }
    } else if (p.Type17->TypeDetail.Registered == 1 &&  //
	          p.Type17->TypeDetail.LrDimm == 1 &&       // LDIMM
	          p.Type17->FormFactor == 9) {              //
      for (i=0;i<AsciiStrSize(BaseModuleType[2]);i++) {
        commanddata[94+i]=*(BaseModuleType[2]+i);
      }
    } else if (p.Type17->TypeDetail.Unbuffered == 1 &&  //
	           p.Type17->FormFactor == 13){             // SO_DIMM
      for (i=0;i<AsciiStrSize(BaseModuleType[3]);i++) { //
        commanddata[94+i]=*(BaseModuleType[3]+i);
      }
    } else {
      for (i=0;i<AsciiStrSize(BaseModuleType[4]);i++) {
        commanddata[94+i]=*(BaseModuleType[4]+i);
      }
    }
	
    Health = "OK";
    for (i=0;i<AsciiStrSize(Health);i++) {
      commanddata[114+i]=*(Health+i);
    }

    CopyMem(pTempDimminfo, commanddata, 120);
    pTempDimminfo = pTempDimminfo + 120;
    gDimminfoLen = gDimminfoLen + 120;
 }

  if (commanddata) {
    FreePool (commanddata);
  }
  DEBUG((EFI_D_INFO, "pDimminfo = 0x%lx\n", pDimminfo));
  DEBUG((EFI_D_INFO, "gDimminfoLen = 0x%x\n", gDimminfoLen));
}

VOID
CollectHddInfo (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_DISK_INFO_PROTOCOL          *DiskInfo;
  UINTN                           HandleCount, Index, RotationRate;
  EFI_HANDLE                      *HandleBuffer;
  CHAR8                           *ModelName, *FirmwareVer, *SerialNo, *Module, *Health, *IndicatorStatus;
  CHAR8                           *State, *Protocol, *MediaType, *IndicatorLED, *HotspareType;
  UINT8                           *commanddata = NULL;
  
  UINT64                          DriveSizeInBytes = 0, NumSectors = 0, RemainderInBytes = 0;
  UINT32                          DriveSizeInGB = 0, BufferSize;
  ATA_IDENTIFY_DATA               *IdentifyData = NULL;
  UINT8                           i, HddNum=0;
  UINT8                           *pTempHddinfo = NULL;
  UINT8						      Speed;
  UINTN						      ControllerNum;
  UINTN                           PortIndex;
  UINTN                           RealPortIndex;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  
  Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiDiskInfoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );

  if(!EFI_ERROR(Status)){
    IdentifyData = AllocatePool(sizeof(ATA_IDENTIFY_DATA));
    if(IdentifyData == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ProcExit;
    }
    pHddinfo = AllocateZeroPool(HandleCount * 190);
    pTempHddinfo = pHddinfo;

    commanddata = AllocateZeroPool (190);
    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol(
                    HandleBuffer[Index],
                    &gEfiDiskInfoProtocolGuid,
                    (VOID**)&DiskInfo
                    );
      ASSERT_EFI_ERROR(Status); 
 
      BufferSize = sizeof(ATA_IDENTIFY_DATA);
      Status = DiskInfo->Identify (
                       DiskInfo,
                       IdentifyData,
                       &BufferSize
                       );
      DEBUG((EFI_D_INFO,"Identify status = %r\n",Status));
      if (EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR,"Identify failed!\n"));   
        continue;
      }
      if ((!(IdentifyData->config & ATAPI_DEVICE)) || (IdentifyData->config == 0x848A)) {
        if (IdentifyData->command_set_supported_83 & _48_BIT_ADDRESS_FEATURE_SET_SUPPORTED) { 
          NumSectors = *(UINT64 *)&IdentifyData->maximum_lba_for_48bit_addressing; 
        } else {
          NumSectors = IdentifyData->user_addressable_sectors_lo + (IdentifyData->user_addressable_sectors_hi << 16) ; 
        }

        DriveSizeInBytes = MultU64x32(NumSectors, 512);    
        DriveSizeInGB = (UINT32) DivU64x64Remainder (DriveSizeInBytes, 1000000000, &RemainderInBytes);  
        DriveSizeInGB &=~1;

        RotationRate = GetAtaMediaRotationRate(IdentifyData);

        ZeroMem(commanddata, 190);

        commanddata[0] = SET_HDD_INFO;
        commanddata[1] = HddDeviceId++;

        ModelName = IdentifyData->ModelName;
        ConvertIdentifyDataFormat(ModelName,AsciiStrSize(ModelName));
        for (i=0;i<AsciiStrSize(ModelName);i++) {
          if(*(ModelName+i)!='\0'){
            commanddata[2+i]=*(ModelName+i);
          }else{
            commanddata[2+i]='\0';
          }
        }

        Module = "SATA";
        for (i=0;i<AsciiStrSize(Module);i++) {
          commanddata[42+i]=*(Module+i);
        }

        FirmwareVer = IdentifyData->FirmwareVer;
        ConvertIdentifyDataFormat(FirmwareVer, AsciiStrSize(FirmwareVer));    
        for (i=0;i<AsciiStrSize(FirmwareVer);i++) {
          commanddata[58+i]=*(FirmwareVer+i);
        }

        Health = "OK"; 
        for (i=0;i<AsciiStrSize(Health);i++) {
          commanddata[66+i]=*(Health+i);
        }

        State = "Enabled";
        for (i=0;i<AsciiStrSize(State);i++) {
          commanddata[74+i]=*(State+i);
        }

        CopyMem (&commanddata[82], (UINT8*)&DriveSizeInGB, 4);

        commanddata[86] = 1;

        Protocol = "SATA";
        for (i=0;i<AsciiStrSize(Protocol);i++) {
          commanddata[87+i]=*(Protocol+i);
        }

        if(RotationRate == 1){
          MediaType = "SSD";
        } else {
          MediaType = "HDD";
        }
        for (i=0;i<AsciiStrSize(MediaType);i++) {
          commanddata[103+i]=*(MediaType+i);
        }

        SerialNo = IdentifyData->SerialNo;
        ConvertIdentifyDataFormat(SerialNo, AsciiStrSize(SerialNo));  
        for (i=0;i<AsciiStrSize(SerialNo);i++) {
          if(*(SerialNo+i)!='\0'){
            commanddata[131+i]=*(SerialNo+i);
          }else{
            commanddata[131+i]='\0';
          }
        }
        commanddata[150] = '\0';
		

		LibGetSataPortInfo(gBS, HandleBuffer[Index], &ControllerNum, &PortIndex, &RealPortIndex);
		ControllerNum &= (UINTN)~BIT15;
		Speed = GetSataSpeed(HandleBuffer[Index], RealPortIndex);
		DEBUG((EFI_D_INFO,"Collect hdd speed = %d\n",Speed));
        commanddata[151] = Speed;//Capable Speed Gbs
        commanddata[152] = 3;//Negotiated Speed Gbs
      
        IndicatorLED = "Blinkling";
        for (i=0;i<AsciiStrSize(MediaType);i++) {
          commanddata[155+i]=*(MediaType+i);
        }

        HotspareType = "None";
        for (i=0;i<AsciiStrSize(HotspareType);i++) {
          commanddata[163+i]=*(HotspareType+i);
        }

        IndicatorStatus = "OK";
        for (i=0;i<AsciiStrSize(IndicatorStatus);i++) {
          commanddata[171+i]=*(IndicatorStatus+i);
        }

        CopyMem(pTempHddinfo, commanddata, 190);

        pTempHddinfo = pTempHddinfo + 190;
        gHddinfoLen = gHddinfoLen + 190;
      }
    }
  }
  DEBUG((EFI_D_INFO, "pHddinfo = 0x%lx\n", pHddinfo));
  DEBUG((EFI_D_INFO, "gHddinfoLen = 0x%x\n", gHddinfoLen));
ProcExit:  
  if(IdentifyData != NULL){
    FreePool(IdentifyData);
  }
  if (HandleBuffer!= NULL){
    FreePool(HandleBuffer);
  }
  if (commanddata!= NULL){
    FreePool(commanddata);
  }  
}

VOID
CollectNvmeInfo (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           HandleCount, Index, Size;
  EFI_HANDLE                      *Handles;
  BYO_DISKINFO_PROTOCOL           *ByoDiskInfo;
  CHAR8                           Mn[41];
  CHAR8                           Sn[21];
  UINT64                          DiskSize, RemainderInBytes;
  UINT32                          DriveSizeInGB;
  UINT8                           *commanddata = NULL;
  UINT8                           DeviceId = 0;
  CHAR8                           *Module, *Health, *State, *Protocol, *MediaType, *IndicatorLED;  
  CHAR8                           *HotspareType, *IndicatorStatus, *FirmwareVer;
  UINT8                           i;
  UINT8                           *pTempNvmeinfo = NULL;
  UINT8						      Speed;
  UINTN						      ControllerNum;
  UINTN                           PortIndex;
  UINTN                           RealPortIndex;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  
  gBS->LocateHandleBuffer (
                            ByProtocol,
                            &gByoDiskInfoProtocolGuid,
                            NULL,
                            &HandleCount,
                            &Handles
                            );

  pNvmeinfo = AllocateZeroPool(HandleCount * 190);
  pTempNvmeinfo = pNvmeinfo;

  commanddata = AllocateZeroPool(190);	
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                                  Handles[Index],
                                  &gByoDiskInfoProtocolGuid,
                                  (VOID **)&ByoDiskInfo
                                  );
    if (EFI_ERROR (Status)) {
      continue;
    }
    if(ByoDiskInfo->DevType != BYO_DISK_INFO_DEV_NVME){
      continue;
    }

    Size = sizeof(Mn);
    Status = ByoDiskInfo->GetMn(ByoDiskInfo, Mn, &Size);
    Size = sizeof(Sn);
    Status = ByoDiskInfo->GetSn(ByoDiskInfo, Sn, &Size);
    Status = ByoDiskInfo->GetDiskSize(ByoDiskInfo, &DiskSize);

    DriveSizeInGB = (UINT32) DivU64x64Remainder(DiskSize, 1000000000, &RemainderInBytes);  
    DriveSizeInGB &=~1;
    DEBUG((EFI_D_ERROR, __FUNCTION__"(), DriveSizeInGB :%d.\n", DriveSizeInGB));

    ZeroMem(commanddata, 190);

    commanddata[0] = SET_HDD_INFO;
    commanddata[1] = HddDeviceId++;

    for (i=0;i<AsciiStrSize(Mn);i++) {
      DEBUG((EFI_D_INFO,"Mn[%d] = %a\n",i,Mn[i]));
      if(*(Mn+i)!='\0'){
        commanddata[2+i]=*(Mn+i);
      }else{
        commanddata[2+i]='\0';
      }
    }

    Module = "NVMe";
    for (i=0;i<AsciiStrSize(Module);i++) {
      commanddata[42+i]=*(Module+i);
    }

    FirmwareVer = " ";
    ConvertIdentifyDataFormat(FirmwareVer, AsciiStrSize(FirmwareVer));    
    for (i=0;i<AsciiStrSize(FirmwareVer);i++) {
      commanddata[58+i]=*(FirmwareVer+i);
    }

    Health = "OK"; 
    for (i=0;i<AsciiStrSize(Health);i++) {
      commanddata[66+i]=*(Health+i);
    }

    State = "Enabled";
    for (i=0;i<AsciiStrSize(State);i++) {
      commanddata[74+i]=*(State+i);
    }

    CopyMem (&commanddata[82], (UINT8*)&DriveSizeInGB, 4);

    commanddata[86] = 1;

    Protocol = "NVMe";
    for (i=0;i<AsciiStrSize(Protocol);i++) {
      commanddata[87+i]=*(Protocol+i);
    }

    MediaType = "SSD";
    for (i=0;i<AsciiStrSize(MediaType);i++) {
      commanddata[103+i]=*(MediaType+i);
    }

    for (i=0;i<AsciiStrSize(Sn);i++) {
      if(*(Sn+i)!='\0'){
        commanddata[131+i]=*(Sn+i);
      }else{
        commanddata[131+i]='\0';
      }
    }
    commanddata[150] = '\0';
	LibGetSataPortInfo(gBS, Handles[Index], &ControllerNum, &PortIndex, &RealPortIndex);
	ControllerNum &= (UINTN)~BIT15;
	Speed = GetSataSpeed(Handles[Index], RealPortIndex);
	
	DEBUG((EFI_D_INFO,"Collect hdd speed = %d\n",Speed));

    commanddata[151] = Speed;//Capable Speed Gbs
    commanddata[152] = 3;//Negotiated Speed Gbs

    IndicatorLED = "Blinkling";
    for (i=0;i<AsciiStrSize(MediaType);i++) {
      commanddata[155+i]=*(MediaType+i);
    }

    HotspareType = "None";
    for (i=0;i<AsciiStrSize(HotspareType);i++) {
      commanddata[163+i]=*(HotspareType+i);
    }

    IndicatorStatus = "OK";
    for (i=0;i<AsciiStrSize(IndicatorStatus);i++) {
      commanddata[171+i]=*(IndicatorStatus+i);
    }

    CopyMem(pTempNvmeinfo, commanddata, 190);

    pTempNvmeinfo = pTempNvmeinfo + 190;
    gNvmeinfoLen = gNvmeinfoLen + 190;

  }

  if (commanddata) {
    FreePool (commanddata);
  }
  DEBUG((EFI_D_INFO, "pNvmeinfo = 0x%lx\n", pNvmeinfo));
  DEBUG((EFI_D_INFO, "gNvmeinfoLen = 0x%x\n", gNvmeinfoLen));
}

VOID
CollectNetCardInfo (
  VOID
  )
{
  EFI_STATUS                Status;
  UINTN                     HandleCount, Index, Index2;
  EFI_HANDLE                *HandleBuffer;
  UINT8                     MacAddr[6];
  UINT8                     MacAddr2[6];
  CHAR8                     MacAddress[18];
  UINT8                     LanIndex = 0;
  UINT8                     commanddata[32];
  EFI_PCI_IO_PROTOCOL       *PciIo, *PciIo2;
  UINT16                    VenderId, DeviceId, VenderId2, DeviceId2;
  UINTN                     Seg, Bus, Dev, Fun, Seg2,Bus2, Dev2, Fun2; 
  UINT32                    VidDid, VidDid2;
  UINT8                     ClassCode[3];
  UINT8                     ClassCode2[3];
  UINT8                     RevisionId;  
  UINT16                    SubsystemId, SubsystemVendorId;
  UINT32                    Data32 = 0;
  UINT8                     PciIndex=-1, PortNum, CapabilityID = 0, CapabilityPtr = 0;
  UINT16                    CapabilityEntry = 0;
  UINT16                    GetMac = 0xf, Init = 0x4;
  UINT8                     *pTempNetcardinfo = NULL;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  pNetcardinfo = AllocateZeroPool(HandleCount * 32);
  pTempNetcardinfo = pNetcardinfo;
  
  for (Index = 0; Index < HandleCount; Index++) { //PCIe
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    ASSERT_EFI_ERROR (Status);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, 9, 3, &ClassCode);
    PciIo->GetLocation (PciIo, &Seg, &Bus, &Dev, &Fun);
    if (ClassCode[2] == 0x06 || ClassCode[2] == 0x08 || ClassCode[2] > 0x0F || Bus == 0x04 || Fun != 0) { // skip bridge + Base system peripherals + Encryption/Decryption controllers + vga control
      continue;
    }
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, 1, &VidDid);
    if ((VidDid&0xFFFF) == 0x1D94) { // skip bridge + Base system peripherals + Encryption/Decryption controllers
      continue;
    }
    if (VidDid == 0x20001A03 || VidDid == 0x06221B21 ) { 
      continue;
    }

	if(ClassCode[2]==1&&ClassCode[1]==8&&ClassCode[0]==2){
        continue;
	}//skip nvme

    PciIndex ++;

    if ( ClassCode[2] != PCI_CLASS_NETWORK) {
      continue;
    }

    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0, 1, &VenderId);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0x02, 1, &DeviceId);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8,  0x08, 1, &RevisionId);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0x2C, 1, &SubsystemVendorId);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0x2E, 1, &SubsystemId);

    SetMem (MacAddress, sizeof (MacAddress), 0);

    ZeroMem(commanddata, 32);
    commanddata[0] = SET_LAN_INFO;
    commanddata[1] = LanIndex;
    if(Bus == 0x01 && Dev == 0){
      commanddata[2] = 0;
      commanddata[3] = 1;
    } else {
      commanddata[2] = 1;
      if(DeviceId == 0x1572){
        commanddata[3] = 2;
      }else{
        commanddata[3] = 0;
      }
    }

    commanddata[4] = PciIndex;
    PortNum = 1;

    DEBUG((EFI_D_INFO,"Netcard venderid = %0x\n",VenderId));
    if(VenderId == 0x10EC){
      Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_IO, NULL);
      Status = PciIo->Io.Read(PciIo, EfiPciIoWidthUint8, 0, 0, 6, MacAddr);
    } else if(VenderId == 0x8086){
      Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
      if(DeviceId == 0x1557 || DeviceId == 0x10fb || DeviceId == 0x1528 || DeviceId ==0x1563){
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x5400, 6, MacAddr);
      }else if(DeviceId == 0x1572){
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x001e4440, 4, MacAddr);
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x001e44c0, 2, &MacAddr[4]);
      }else {
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x40, 6, MacAddr);
      }
    } else if(VenderId == 0x14E4){ //Braodcom 
      DEBUG((EFI_D_INFO, "DevId:%X\n", DeviceId));  
      if(DeviceId == 0x168E){	//NetXtreme II	Gigabit Ethernet
        Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x8010, 4, &Data32);
        MacAddr[0] = (UINT8)(Data32>>8);
        MacAddr[1] = (UINT8)Data32;
        Data32 = 0;
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x8014, 4, &Data32);			
        MacAddr[2] = (UINT8)(Data32 >> 24);
        MacAddr[3] = (UINT8)(Data32>>16);
        MacAddr[4] = (UINT8)(Data32>>8);
        MacAddr[5] = (UINT8)Data32;
      }
    } else if(VenderId == 0x8088){
      Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
      Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x16200, 6, MacAddr);
      SwapLanMacAddress(MacAddr);
    } else if (VenderId == 0x15b3){
      PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CAPBILITY_POINTER_OFFSET, 1, &CapabilityPtr);
      while ((CapabilityPtr >= 0x40) && ((CapabilityPtr & 0x03) == 0x00)) {
        PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, CapabilityPtr, 1, &CapabilityEntry);
        CapabilityID = (UINT8) CapabilityEntry;
        DEBUG((EFI_D_INFO,"NETCARD CapabilityID = %0x\n",CapabilityID));
        if (0x9 == CapabilityID) {
          PciIo->Pci.Write(PciIo, EfiPciIoWidthUint16, CapabilityPtr + 0x04, 1, &GetMac);
          PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, CapabilityPtr + 0x10, 4, &Data32);
          MacAddr[0] = (UINT8)(Data32>>8);
          MacAddr[1] = (UINT8)Data32;
          Data32 = 0;
          PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, CapabilityPtr + 0x14, 4, &Data32);			
          MacAddr[2] = (UINT8)(Data32 >> 24);
          MacAddr[3] = (UINT8)(Data32>>16);
          MacAddr[4] = (UINT8)(Data32>>8);
          MacAddr[5] = (UINT8)Data32;
          PciIo->Pci.Write(PciIo, EfiPciIoWidthUint16, CapabilityPtr + 0x04, 1, &Init);
          break;
        }
        CapabilityPtr = (UINT8)(CapabilityEntry >> 8);
      }
    }else {
      Status = EFI_UNSUPPORTED;
      MacAddr[0] = 0;
      MacAddr[1] = 0;			
      MacAddr[2] = 0;
      MacAddr[3] = 0;
      MacAddr[4] = 0;
      MacAddr[5] = 0;
    }
    DEBUG((EFI_D_INFO,"set mem early\n"));
    commanddata[6]=MacAddr[0];
    commanddata[7]=MacAddr[1];
    commanddata[8]=MacAddr[2];
    commanddata[9]=MacAddr[3];
    commanddata[10]=MacAddr[4];
    commanddata[11]=MacAddr[5];

    for(Index2 = 0; Index2 < HandleCount; Index2++){
      Status = gBS->HandleProtocol (
                    HandleBuffer[Index2],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo2
                    );
      ASSERT_EFI_ERROR (Status);
      PciIo2->Pci.Read (PciIo2, EfiPciIoWidthUint16, 0, 1, &VenderId2);
      PciIo2->Pci.Read (PciIo2, EfiPciIoWidthUint8, 9, 3, &ClassCode2);
      PciIo2->GetLocation (PciIo2, &Seg2, &Bus2, &Dev2, &Fun2);
      PciIo2->Pci.Read (PciIo2, EfiPciIoWidthUint32, 0, 1, &VidDid2);
      PciIo2->Pci.Read (PciIo2, EfiPciIoWidthUint16, 0x02, 1, &DeviceId2);
      DEBUG((EFI_D_INFO,"netcard bus = %x,dev=%x\n",Bus2,Dev2));
      if(!(Bus2==Bus && Dev2==Dev && Fun2!=Fun)){
        continue;
      }
      DEBUG((EFI_D_INFO,"fun=%x,vendorid = %x\n",Fun2,VenderId2));
      PortNum++;
      if(VenderId2 == 0x10EC){
        Status = PciIo2->Attributes(PciIo2, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_IO, NULL);
        Status = PciIo2->Io.Read(PciIo2, EfiPciIoWidthUint8, 0, 0, 6, MacAddr2);
      } else if(VenderId2 == 0x8086){
        Status = PciIo2->Attributes(PciIo2, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
        if(DeviceId2 == 0x1557 || DeviceId2 == 0x10fb || DeviceId2 == 0x1528 || DeviceId2 ==0x1563){
          Status = PciIo2->Mem.Read(PciIo2, EfiPciIoWidthUint8, 0, 0x5400, 6, MacAddr2);
        }else if(DeviceId2 == 0x1572){
          Status = PciIo2->Mem.Read(PciIo2, EfiPciIoWidthUint8, 0, 0x001e4440, 4, MacAddr2);
          Status = PciIo2->Mem.Read(PciIo2, EfiPciIoWidthUint8, 0, 0x001e44c0, 2, &MacAddr2[4]);
        }else {
          Status = PciIo2->Mem.Read(PciIo2, EfiPciIoWidthUint8, 0, 0x40, 6, MacAddr2);
        }
      } else if(VenderId2 == 0x14E4){ //Braodcom  
        if(DeviceId2 == 0x168E){	//NetXtreme II	Gigabit Ethernet
          Status = PciIo2->Attributes(PciIo2, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
          Status = PciIo2->Mem.Read(PciIo2, EfiPciIoWidthUint8, 0, 0x8010, 4, &Data32);
          MacAddr2[0] = (UINT8)(Data32>>8);
          MacAddr2[1] = (UINT8)Data32;
          Data32 = 0;
          Status = PciIo2->Mem.Read(PciIo2, EfiPciIoWidthUint8, 0, 0x8014, 4, &Data32);			
          MacAddr2[2] = (UINT8)(Data32 >> 24);
          MacAddr2[3] = (UINT8)(Data32>>16);
          MacAddr2[4] = (UINT8)(Data32>>8);
          MacAddr2[5] = (UINT8)Data32;
        }
      } else if(VenderId2 == 0x8088){
        Status = PciIo2->Attributes(PciIo2, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
        Status = PciIo2->Mem.Read(PciIo2, EfiPciIoWidthUint8, 0, 0x16200, 6, MacAddr2);
        SwapLanMacAddress(MacAddr2);
      }else if (VenderId2 == 0x15b3){
        PciIo2->Pci.Read(PciIo2, EfiPciIoWidthUint8, PCI_CAPBILITY_POINTER_OFFSET, 1, &CapabilityPtr);
        while ((CapabilityPtr >= 0x40) && ((CapabilityPtr & 0x03) == 0x00)) {
          PciIo2->Pci.Read(PciIo2, EfiPciIoWidthUint16, CapabilityPtr, 1, &CapabilityEntry);
          CapabilityID = (UINT8) CapabilityEntry;
          if (0x9 == CapabilityID) {
            PciIo2->Pci.Write(PciIo2, EfiPciIoWidthUint16,CapabilityPtr + 0x04, 1, &GetMac);
            PciIo2->Pci.Read (PciIo2, EfiPciIoWidthUint8, CapabilityPtr + 0x10, 4, &Data32);
            MacAddr2[0] = (UINT8)(Data32>>8);
            MacAddr2[1] = (UINT8)Data32;
            Data32 = 0;
            PciIo2->Pci.Read(PciIo2, EfiPciIoWidthUint8, CapabilityPtr + 0x14, 4, &Data32);			
            MacAddr2[2] = (UINT8)(Data32 >> 24);
            MacAddr2[3] = (UINT8)(Data32>>16);
            MacAddr2[4] = (UINT8)(Data32>>8);
            MacAddr2[5] = (UINT8)Data32;
            PciIo2->Pci.Write(PciIo2, EfiPciIoWidthUint16, CapabilityPtr + 0x04, 1, &Init);
            break;
          }
          CapabilityPtr = (UINT8)(CapabilityEntry >> 8);
        }
      }else {
        Status = EFI_UNSUPPORTED;
        MacAddr2[0] = 0;
        MacAddr2[1] = 0;			
        MacAddr2[2] = 0;
        MacAddr2[3] = 0;
        MacAddr2[4] = 0;
        MacAddr2[5] = 0;
      }
      DEBUG((EFI_D_INFO,"MAC2 ADDRESS %02x-%02x-%02x-%02x-%02x-%02x\n",MacAddr2[0],MacAddr2[1],MacAddr2[2],MacAddr2[3],MacAddr2[4],MacAddr2[5]));
      commanddata[6+(PortNum-1)*6] =MacAddr2[0];
      commanddata[7+(PortNum-1)*6] =MacAddr2[1];
      commanddata[8+(PortNum-1)*6] =MacAddr2[2];
      commanddata[9+(PortNum-1)*6] =MacAddr2[3];
      commanddata[10+(PortNum-1)*6]=MacAddr2[4];
      commanddata[11+(PortNum-1)*6]=MacAddr2[5];

      commanddata[5] = PortNum;
    }

    CopyMem(pTempNetcardinfo, commanddata, 32);
    pTempNetcardinfo = pTempNetcardinfo + 32;
    gNetcardinfoLen = gNetcardinfoLen + 32;

    LanIndex++;
  }

  DEBUG((EFI_D_INFO, "pNetcardinfo = 0x%lx\n", pNetcardinfo));
  DEBUG((EFI_D_INFO, "gNetcardinfoLen = 0x%x\n", gNetcardinfoLen));

  if (HandleBuffer != NULL){
    FreePool (HandleBuffer);
  }
}
VOID
CollectBMCVersion(
  VOID
  )
{  
  EFI_STATUS                  Status;
  UINT16                                FwVersion;
  IPMI_INTERFACE_PROTOCOL               *IpmiInterface = NULL;

  Status = gBS->LocateProtocol (
              &gByoIpmiInterfaceProtocolGuid,
              NULL,
              (VOID **) &IpmiInterface
              );
  if (IpmiInterface == NULL) {
    return;
  }
  pBMCVersioninfo = AllocateZeroPool (20);

  FwVersion = IpmiInterface->GetBmcFirmwareVersion (IpmiInterface);  
  AsciiSPrint(pBMCVersioninfo, 20, "BMC version:%d.%x.%d", FwVersion >> 12, (FwVersion >> 4) & 0xff, FwVersion & 0xf);
  gBMCVersioninfoLen = (UINT8)AsciiStrSize(pBMCVersioninfo);
}
  

VOID
CollectAllData (
  VOID
  )
{  
  CollectBiosInfo ();
  CollectPciInfo ();
  CollectCpuInfo ();
  CollectDimmInfo ();
  CollectHddInfo ();
  CollectNvmeInfo ();
  CollectNetCardInfo ();
  CollectBMCVersion();
}

VOID
SendBiosInfo2BMC (
  VOID
)
{
  EFI_STATUS                   Status;
  UINT8                        ResponseSize;
  UINT8                        *ResponseData = NULL;
    
  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  if ((gBiosinfoLen == 0) || (pBiosinfo == NULL)) {
    return;
  }

  ResponseData = AllocateZeroPool(BIOS_2_BMC_DATA_SIZE);	
  DEBUG((EFI_D_INFO, "gBiosinfoLen = 0x%x \n", gBiosinfoLen));
  Status = EfiSendCommandToBMC (
             SM_BYOSOFT_NETFN_APP,
             SM_BYOSOFT_NETFN_SUB_FUN,
             pBiosinfo,
             gBiosinfoLen,
             ResponseData,
             &ResponseSize
             );
  if (Status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "EfiSendCommandToBMC Status = %r \n", Status));
  }

  if (ResponseData) {
    FreePool (ResponseData);
  }  
}

VOID
SendPciInfo2BMC(
)
{
  EFI_STATUS                  Status;
  UINT8                       Index;
  UINTN                       TotalCont;
  UINT8                       ResponseSize = 1;
  UINT8                       *ResponseData = NULL;


  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  if ((gPciinfoLen == 0) || (pPciinfo == NULL)) {
    return;
  }
  TotalCont = gPciinfoLen / 32;
  DEBUG((EFI_D_INFO, "gPciinfoLen = 0x%x \n", gPciinfoLen));

  ResponseData = AllocateZeroPool(32);	
  
  for (Index = 0; Index < TotalCont; Index++) {
    ZeroMem (ResponseData, 32);
    Status = EfiSendCommandToBMC (
                      SM_BYOSOFT_NETFN_APP,
                      SM_BYOSOFT_NETFN_SUB_FUN,
                      (pPciinfo + Index * 32), 
                      32,
                      &ResponseData[0],
                      &ResponseSize
                      );
    if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "EfiSendCommandToBMC Status = %r \n", Status));
    }
  }
  if (ResponseData) {
    FreePool (ResponseData);
  }
}

VOID
SendCpuInfo2BMC(
  VOID
)
{
  EFI_STATUS                      Status;
  UINT8                           Index;
  UINTN                           TotalCont;
  UINT8                           *ResponseData = NULL;
  UINT8                           ResponseDataSize;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  if ((gCpuinfoLen == 0) || (pCpuinfo == NULL)) {
    return;
  }
  TotalCont = gCpuinfoLen / 160;
  DEBUG((EFI_D_INFO, "gCpuinfoLen = 0x%x \n", gCpuinfoLen));
  ResponseData = AllocateZeroPool(160);	

  for (Index = 0; Index < TotalCont; Index++) {
    ZeroMem (ResponseData, 160);
    Status = EfiSendCommandToBMC (
                    SM_BYOSOFT_NETFN_APP,              //OEM CMD to send CPU info to BMC
                    SM_BYOSOFT_NETFN_SUB_FUN,
                    (pCpuinfo + Index * 160), 
                    160,
                    &ResponseData[0],
                    &ResponseDataSize
                    );
    if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "EfiSendCommandToBMC Status = %r \n", Status));
    }
  }
  if (ResponseData) {
    FreePool (ResponseData);
  }
}

VOID
SendDIMMInfo2BMC( 
  VOID
  )
{
  EFI_STATUS                      Status;
  UINT8                           Index;
  UINTN                           TotalCont;
  UINT8                           *ResponseData = NULL;
  UINT8                           ResponseDataSize;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  if ((gDimminfoLen == 0) || (pDimminfo == NULL)) {
    return;
  }
  TotalCont = gDimminfoLen / 120;
  DEBUG((EFI_D_INFO, "gDimminfoLen = 0x%x \n", gDimminfoLen));
  ResponseData = AllocateZeroPool(120);	

  for (Index = 0; Index < TotalCont; Index++) {
    ZeroMem (ResponseData, 120);
    Status = EfiSendCommandToBMC (
                                 SM_BYOSOFT_NETFN_APP,              
                                 SM_BYOSOFT_NETFN_SUB_FUN,
                                 (pDimminfo + Index * 120),
                                 120,
                                 &ResponseData[0],
                                 &ResponseDataSize
                                 );
    if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "EfiSendCommandToBMC Status = %r \n", Status));
    }
  }
  if (ResponseData) {
    FreePool (ResponseData);
  }
  
}  

VOID
SendHddInfo2BMC(
)
{
  EFI_STATUS                      Status;
  UINT8                           Index;
  UINTN                           TotalCont;
  UINT8                           *ResponseData = NULL;
  UINT8                           ResponseDataSize;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  if ((gHddinfoLen == 0) || (pHddinfo == NULL)) {
    return;
  }
  TotalCont = gHddinfoLen / 190;
  DEBUG((EFI_D_INFO, "gHddinfoLen = 0x%x \n", gHddinfoLen));

  ResponseData = AllocateZeroPool(190);	

  for (Index = 0; Index < TotalCont; Index++) {
    ZeroMem (ResponseData, 190);
    Status = EfiSendCommandToBMC (
                                 SM_BYOSOFT_NETFN_APP,              
                                 SM_BYOSOFT_NETFN_SUB_FUN,
                                 (pHddinfo + Index * 190),
                                 190,
                                 &ResponseData[0],
                                 &ResponseDataSize
                                 );
    if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "EfiSendCommandToBMC Status = %r \n", Status));
    }
  }
  if (ResponseData) {
    FreePool (ResponseData);
  }

}

VOID
SendNvmeInfo2BMC(
)
{
  EFI_STATUS                      Status;
  UINT8                           Index;
  UINTN                           TotalCont;
  UINT8                           *ResponseData = NULL;  
  UINT8                           ResponseDataSize;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  if ((gNvmeinfoLen == 0) || (pNvmeinfo == NULL)) {
    return;
  }
  TotalCont = gNvmeinfoLen / 190;
  DEBUG((EFI_D_INFO, "gNvmeinfoLen = 0x%x \n", gNvmeinfoLen));

  ResponseData = AllocateZeroPool(190);	
  
  for (Index = 0; Index < TotalCont; Index++) {
    ZeroMem (ResponseData, 190);
    Status = EfiSendCommandToBMC (
                                 SM_BYOSOFT_NETFN_APP,              
                                 SM_BYOSOFT_NETFN_SUB_FUN,
                                 (pNvmeinfo + Index * 190),
                                 190,
                                 &ResponseData[0],
                                 &ResponseDataSize
                                 );
    if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "EfiSendCommandToBMC Status = %r \n", Status));
    }
  }
  if (ResponseData) {
    FreePool (ResponseData);
  }

}

VOID
SendNetCardInfo2BMC( 
  )
{
  EFI_STATUS                      Status;
  UINT8                           Index;
  UINTN                           TotalCont;
  UINT8                           *ResponseData = NULL;
  UINT8                           ResponseDataSize;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  if ((gNetcardinfoLen == 0) || (pNetcardinfo == NULL)) {
    return;
  }
  TotalCont = gNetcardinfoLen / 32;
  DEBUG((EFI_D_INFO, "gNetcardinfoLen = 0x%x \n", gNetcardinfoLen));

  ResponseData = AllocateZeroPool(50);	
  
  for (Index = 0; Index < TotalCont; Index++) {
    ZeroMem (ResponseData, 50);
    Status = EfiSendCommandToBMC (
                                 SM_BYOSOFT_NETFN_APP,              
                                 SM_BYOSOFT_NETFN_SUB_FUN,
                                 (pNetcardinfo + Index * 32),
                                 32,
                                 &ResponseData[0],
                                 &ResponseDataSize
                                 );
    if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "EfiSendCommandToBMC Status = %r \n", Status));
    }
  }
  if (ResponseData) {
    FreePool (ResponseData);
  }

}
VOID
SendAllData2BMC (
  VOID
  )
{
  SendBiosInfo2BMC ();
  SendPciInfo2BMC ();
  SendCpuInfo2BMC ();
  SendDIMMInfo2BMC ();
  SendHddInfo2BMC ();
  SendNvmeInfo2BMC ();
  SendNetCardInfo2BMC ();
}

UINT32
CalculateAllDataCrc (
  VOID
  )
{
  UINT32        CrcVal = 0;

  if ((pBiosinfo != NULL) && (gBiosinfoLen > 0)) {
    CrcVal = CrcVal ^ CalculateCrc32(pBiosinfo, gBiosinfoLen);
  }

  if ((pPciinfo != NULL) && (gPciinfoLen > 0)) {
    CrcVal = CrcVal ^ CalculateCrc32(pPciinfo, gPciinfoLen);
  }
  
  if ((pCpuinfo != NULL) && (gCpuinfoLen > 0)) {
    CrcVal = CrcVal ^ CalculateCrc32(pCpuinfo, gCpuinfoLen);
  }
  
  if ((pDimminfo != NULL) && (gDimminfoLen > 0)) {
    CrcVal = CrcVal ^ CalculateCrc32(pDimminfo, gDimminfoLen);
  }
  
  if ((pHddinfo != NULL) && (gHddinfoLen > 0)) {
    CrcVal = CrcVal ^ CalculateCrc32(pHddinfo, gHddinfoLen);
  }
  
  if ((pNvmeinfo != NULL) && (gNvmeinfoLen > 0)) {
    CrcVal = CrcVal ^ CalculateCrc32(pNvmeinfo, gNvmeinfoLen);
  }
  
  if ((pNetcardinfo != NULL) && (gNetcardinfoLen > 0)) {
    CrcVal = CrcVal ^ CalculateCrc32(pNetcardinfo, gNetcardinfoLen);
  }
  
  if ((pBMCVersioninfo != NULL) && (gBMCVersioninfoLen > 0)) {
    CrcVal = CrcVal ^ CalculateCrc32(pBMCVersioninfo, gBMCVersioninfoLen);
  }

  return CrcVal;

}

BOOLEAN
IsCrcChanged (
  UINT32          NewCrcVal
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT32          CrcValVariable;
  UINTN           VarSize = sizeof(UINT32);

  Status = gRT->GetVariable (
                  L"DataCrc",
                  &gCrcDataGuid,
                  NULL,
                  &VarSize,
                  &CrcValVariable
                  );
  if ((Status == EFI_SUCCESS) && (CrcValVariable == NewCrcVal))  {
    DEBUG((EFI_D_INFO, "DataCrc not changed = 0x%x \n", NewCrcVal));
    return FALSE;
  }

  Status = gRT->SetVariable (
                    L"DataCrc",
                    &gCrcDataGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    VarSize,
                    &NewCrcVal
                    );
  DEBUG((EFI_D_INFO, "DataCrc has changed = 0x%x \n", NewCrcVal));
  return TRUE;
}

EFI_STATUS
GetSystemBootOptionParam5 (
  IN OUT UINT8  *Buffer,
  IN OUT UINTN  *BufferSize
  )
{
  EFI_STATUS  Status;
  UINT8       CmdDataBuff[10];
  UINT8       CmdDataSize;
  UINT8       ResponseBuff[0xCF];
  UINT8       ResponseSize;
  UINTN       Retry = 3;
  UINT8       TmpSize;

  if (NULL ==  Buffer) {
    return EFI_INVALID_PARAMETER;
  }
  CmdDataBuff[0] = 5;
  CmdDataBuff[1] = 0;
  CmdDataBuff[2] = 0;
  CmdDataSize = 3;
  ResponseSize = sizeof (ResponseBuff);
  SetMem (ResponseBuff, ResponseSize, 0);

  //
  //When setting lan parameters, we need check InProgress bit.
  //Since Lan setting need Linux kernel system invoking, it would take 5 seconds.
  //
  do {
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_CHASSIS,
              EFI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
              (UINT8 *) CmdDataBuff,
              CmdDataSize,
              (UINT8 *) &ResponseBuff,
              &ResponseSize
              );
    if (!EFI_ERROR (Status)) {
      break;
    }
    gBS->Stall (10000);
    Retry--;
  } while (0 != Retry);
  
  if (0 == Retry) {
    *BufferSize = 0;
    return EFI_DEVICE_ERROR;
  }
  TmpSize = sizeof (SYSTEM_BOOT_OPTION_RESPONDE_COMMON_HEAD);
  if (ResponseSize != (*BufferSize + TmpSize)) {
    *BufferSize = ResponseSize - TmpSize;
    return EFI_DEVICE_ERROR;
  }
  CopyMem (Buffer, (UINT8 *) (&(ResponseBuff[2])), *BufferSize);
  DEBUG ((EFI_D_INFO, "Get System Boot Option[%d] completed\n", 5));
  return EFI_SUCCESS;
}
  
EFI_STATUS  
IpmiSetBootGroup()
{
  EFI_STATUS                   Status;
  UINT8                        BiosBootType;
  SYSTEM_BOOT_OPTION_PARAMETER5         BootOptionParam5;
  UINTN                                 Size;
  UINT8                             UefiGroup;
  UINT8                             LegacyGroup;
  UINTN                             LegacyGroupOrderSize = 0;
  UINTN                             UefiGroupOrderSize = 0;
  UINT8                             UefiGroupOrder[5];
  UINT8                             LegacyGroupOrder[4];
  UINT8                             UefiGroupOrderNew[5];
  UINT8                             LegacyGroupOrderNew[4];
  UINT8                             Index1,Index2;
  UINT8                             Flag=FALSE;
  UINT8                             DefaultUefiBootGroupOrder[] = {
	BM_MENU_TYPE_UEFI_HDD, 
	//BM_MENU_TYPE_UEFI_ODD, 
	BM_MENU_TYPE_UEFI_USK_DISK,
	BM_MENU_TYPE_UEFI_PXE,
	BM_MENU_TYPE_UEFI_USB_ODD,
	BM_MENU_TYPE_UEFI_OTHERS
	};
  UINT8                             DefaultLegacyBootGroupOrder[] = {
    BM_MENU_TYPE_LEGACY_HDD, 
    //BM_MENU_TYPE_LEGACY_ODD, 
    BM_MENU_TYPE_LEGACY_USK_DISK,
    BM_MENU_TYPE_LEGACY_PXE,
    BM_MENU_TYPE_LEGACY_USB_ODD
  };

  if(PcdGetBool(PcdIpmiBootflag)){
    BiosBootType = PcdGet8(PcdIpmiBootMode);
  }else{
    Status = EFI_UNSUPPORTED;
    }


DEBUG((EFI_D_INFO,"IpmiSetBootGroup\n"));
  Size = sizeof (SYSTEM_BOOT_OPTION_PARAMETER5);
  SetMem (&BootOptionParam5, Size, 0);
  Status = GetSystemBootOptionParam5 ((UINT8 *)(&BootOptionParam5), &Size);
  DumpMem8(&BootOptionParam5, Size);
  if((BootOptionParam5.Data1.Bits.BootFlagValid != 1) || ( BootOptionParam5.Data1.Bits.Persistent != 1)){  
  	return EFI_UNSUPPORTED;
  }
  switch (BootOptionParam5.Data2.Bits.BootDeviceSelector) {
    case 0x01:  //Force Pxe
      UefiGroup = 4;
      LegacyGroup = 0x84;
    break;
    case 0x02:  //Local Hdd
    case 0x0b:  //Remote Hdd
      UefiGroup = 0;
      LegacyGroup = 0x80;
    break;
	case 0x05:  //Local DVD
    case 0x08:  //Remote Dvd
      UefiGroup = 3;
	  LegacyGroup = 0x83;
    break;
    case 0x07:  //Remote floppy
    case 0x0f: //Local floppy
      UefiGroup = 2;
      LegacyGroup = 0x82;
	break;
    default:
    return EFI_UNSUPPORTED;
  }
  if(BiosBootType == 1){//UEFI
    UefiGroupOrderSize = sizeof(UefiGroupOrder);
    Status = gRT->GetVariable(
                  BYO_UEFI_BOOT_GROUP_VAR_NAME, 
                  &gByoGlobalVariableGuid,
                  NULL,
                  &UefiGroupOrderSize,
                  UefiGroupOrder
                  );    
    for(Index1=0;Index1<4;Index1++){
     for(Index2=Index1+1;Index2<5;Index2++){
	    if(UefiGroupOrder[Index1] ==  UefiGroupOrder[Index2]){
	     Flag = TRUE;
	    }
	  }
    } 	 
    for(Index1=0;Index1<5;Index1++){
	  if(UefiGroupOrder[Index1] > 5 || UefiGroupOrder[Index1] == 1){
	    Flag = TRUE;
	  }
    }
	if(Flag){
		DEBUG((EFI_D_INFO,"Group order = null\n"));
	  for(Index1=0;Index1<5;Index1++){
        UefiGroupOrder[Index1] = DefaultUefiBootGroupOrder[Index1];
	  }
	}

    DEBUG((EFI_D_INFO,"UefiGroup = %0x\n",UefiGroup));
    DumpMem8(UefiGroupOrder,UefiGroupOrderSize);  
    for(Index1=0,Index2=1;Index1<5;Index1++){
      if(UefiGroup != UefiGroupOrder[Index1]){
        UefiGroupOrderNew[Index2++] = UefiGroupOrder[Index1];
      }else{
        UefiGroupOrderNew[0] = UefiGroup;
      }
    }
    Status = gRT->SetVariable (
                    BYO_UEFI_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    UefiGroupOrderSize,
                    UefiGroupOrderNew
                    ); 
    DumpMem8(UefiGroupOrderNew,UefiGroupOrderSize);  
	  
  }else if(BiosBootType == 2){//Legacy
    LegacyGroupOrderSize = sizeof(LegacyGroupOrder);
    Status = gRT->GetVariable(
                  BYO_LEGACY_BOOT_GROUP_VAR_NAME, 
                  &gByoGlobalVariableGuid,
                  NULL,
                  &LegacyGroupOrderSize,
                  LegacyGroupOrder
                  );    
	for(Index1=0;Index1<3;Index1++){
	  for(Index2=Index1+1;Index2<4;Index2++){
	    if(LegacyGroupOrder[Index1] == LegacyGroupOrder[Index2]){
	   	  Flag = TRUE;
		}
      }
	}	  
    for(Index1=0;Index1<4;Index1++){
	  if(((LegacyGroupOrder[Index1] < 0x80) && (LegacyGroupOrder[Index1] > 0x85)) || LegacyGroupOrder[Index1] == 1){
		Flag = TRUE;
      }
	}
    if(Flag){	 
	  for(Index1=0;Index1<sizeof(LegacyGroupOrder)/sizeof(UINT8);Index1++){
        LegacyGroupOrder[Index1] = DefaultLegacyBootGroupOrder[Index1];
	  }
	}

    
    DumpMem8(LegacyGroupOrder,LegacyGroupOrderSize);  
    for(Index1=0,Index2=1;Index1<5;Index1++){
      if(LegacyGroup != LegacyGroupOrder[Index1]){
        LegacyGroupOrderNew[Index2++] = LegacyGroupOrder[Index1];
      }else{
        LegacyGroupOrderNew[0] = LegacyGroup;
      }
    }
    Status = gRT->SetVariable (
                    BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    LegacyGroupOrderSize,
                    LegacyGroupOrderNew
                    ); 
    DumpMem8(LegacyGroupOrderNew,LegacyGroupOrderSize);  
	  
  }
  PcdSetBool(PcdIpmiSetBootOrder, TRUE);
  
return EFI_SUCCESS;
}



VOID 
BmcCfgConnectAllCallBack (
  IN EFI_EVENT Event, 
  IN VOID      *Context
  )
{
  EFI_STATUS                   Status;
  VOID                         *Interface;
  UINT32        CrcVal = 0;
  UINT8                       ResponseSize=116;
  UINT8                       ResponseData[116];
  UINT8                       GetCpuInfo[2];

  GetUtcOffsetFromBMC();
  SendTimeInfo2BMC();

  Status = gBS->LocateProtocol(&gBdsAllDriversConnectedProtocolGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  GetCpuInfo[0] = 0x00;
  GetCpuInfo[1] = 0x00;
  Status = EfiSendCommandToBMC (
  	         SM_BYOSOFT_NETFN_APP,
		     0x80,
	         &GetCpuInfo[0],
	         2,
	         &ResponseData[0],
	         &ResponseSize
			 );
  DEBUG((EFI_D_INFO,"Get cpu info from bmc status = %r,ResponseData[0]=%0x\n",Status,ResponseData[0]));

  CollectAllData ();
  CrcVal = CalculateAllDataCrc ();
  if (IsCrcChanged (CrcVal) || ((Status == EFI_SUCCESS) && (ResponseData[0]==0))) {
    SendAllData2BMC ();
  }
  SetUtcOffset2BMC();

  IpmiSetBootGroup();
}
#endif

EFI_BMC_WDT_CFG       mWdtCfg[4];
EFI_STATUS
BmcCfgDriverEntryPoint (
  IN EFI_HANDLE                           ImageHandle,
  IN EFI_SYSTEM_TABLE                     *SystemTable
)
{
  EFI_STATUS            Status;
  BMC_SETUP_DATA        SetupData = {0};
  UINTN                 VarSize;
  EFI_BMC_WDT_PROTOCOL  *WdtProt;
  EFI_GUID              SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_EVENT             Event;

  EfiInitializeIpmiBase ();
  DEBUG((EFI_D_INFO, "%a() end\n",__FUNCTION__));

  VarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
                  BMC_SETUP_VARIABLE_NAME,
                  &SystemConfigurationGuid,
                  NULL,
                  &VarSize,
                  &SetupData
                  );
  if (Status == EFI_NOT_FOUND) {
    SetupData.WdtEnable[1]  = 1;
    SetupData.WdtPolicy[1]  = 1;
    SetupData.WdtTimeout[1] = 150;
    SetupData.WdtEnable[3]  = 0;
    SetupData.WdtPolicy[3]  = 1;
    SetupData.WdtTimeout[3] = 300;
    Status = gRT->SetVariable (
                    BMC_SETUP_VARIABLE_NAME,
                    &SystemConfigurationGuid,
                    BMC_SETUP_VARIABLE_FLAG,
                    VarSize,
                    &SetupData
                    );
  }

  #ifndef MDEPKG_NDEBUG                                   // debug mode  
    Status = ReportFrb2Status();
  #endif

  //
  // Add FanSpeed control
  //
  //FanSpeedCtrl(&SetupData);

  Status = gBS->LocateProtocol (
               &gEfiBmcWdtProtocolGuid,
               NULL,
               &WdtProt
               );
  if (!EFI_ERROR (Status)) {
    mWdtCfg[0].Enable   = FALSE;
    mWdtCfg[0].Action   = 0;
    mWdtCfg[0].Timeout  = 0;
    mWdtCfg[1].Enable   = SetupData.WdtEnable[1];
    mWdtCfg[1].Action   = SetupData.WdtPolicy[1];
    mWdtCfg[1].Timeout  = SetupData.WdtTimeout[1];
    mWdtCfg[2].Enable   = FALSE;
    mWdtCfg[2].Action   = 0;
    mWdtCfg[2].Timeout  = 0;
    mWdtCfg[3].Enable   = SetupData.WdtEnable[3];
    mWdtCfg[3].Action   = SetupData.WdtPolicy[3];
    mWdtCfg[3].Timeout  = SetupData.WdtTimeout[3];
    WdtProt->SetWatchdog (WdtProt, mWdtCfg);
  }
  
  EfiCreateProtocolNotifyEvent (
    &gBdsAllDriversConnectedProtocolGuid,
    TPL_CALLBACK,
    BmcCfgConnectAllCallBack,
    NULL,
    &Event
    ); 
  return EFI_SUCCESS;
}
