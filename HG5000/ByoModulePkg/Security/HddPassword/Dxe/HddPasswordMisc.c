/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  HddPasswordMisc.c	

Abstract:
  
Revision History:

**/
#include "HddPasswordDxe.h"


HDP_HII_VENDOR_DEVICE_PATH  mHddPasswordHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    HDD_PASSWORD_CONFIG_GUID
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
NvSaveValue(  
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  return EFI_UNSUPPORTED;
}



EFI_STATUS 
NvDiscardValue(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  return EFI_UNSUPPORTED; 
}



EFI_STATUS 
NvLoadDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  return EFI_UNSUPPORTED;
}


EFI_STATUS 
NvSaveUserDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  return EFI_UNSUPPORTED; 
}


EFI_STATUS 
NvLoadUserDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS 
IsNvDataChanged(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This,
  BOOLEAN                       *IsDataChanged
  )
{
  *IsDataChanged = FALSE;
  return EFI_SUCCESS;
}

EFI_STATUS 
NvSetMfgDefault (
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  return EFI_UNSUPPORTED;
}





EFI_STATUS
GetHddDeviceIdentifyData (
  IN HDD_PASSWORD_INFO  *HdpInfo
  )
{
  EFI_STATUS                        Status;
  EFI_ATA_COMMAND_BLOCK             Acb;
  EFI_ATA_PASS_THRU_COMMAND_PACKET  Packet;
  

  ZeroMem(&Acb, sizeof (Acb));
  ZeroMem(HdpInfo->Asb, sizeof(EFI_ATA_STATUS_BLOCK));
  ZeroMem(HdpInfo->Identify, sizeof(ATA_IDENTIFY_DATA));
  Acb.AtaCommand    = ATA_CMD_IDENTIFY_DRIVE;
  Acb.AtaDeviceHead = (UINT8) (BIT7 | BIT6 | BIT5 | (HdpInfo->PortMp == 0xFFFF ? 0 : (HdpInfo->PortMp << 4))); 

  ZeroMem(&Packet, sizeof (Packet));
  Packet.Protocol = EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_IN;
  Packet.Length   = EFI_ATA_PASS_THRU_LENGTH_BYTES;
  Packet.Asb      = HdpInfo->Asb;
  Packet.Acb      = &Acb;
  Packet.InDataBuffer     = (VOID*)HdpInfo->Identify;
  Packet.InTransferLength = sizeof(ATA_IDENTIFY_DATA);
  Packet.Timeout          = ATA_TIMEOUT;

  Status = HdpInfo->AtaPassThru->PassThru (
                                   HdpInfo->AtaPassThru,
                                   HdpInfo->Port,
                                   HdpInfo->PortMp,
                                   &Packet,
                                   NULL
                                   );
  
  return Status;
}



EFI_STATUS
SecurityUnlockHdd (
  IN  HDD_PASSWORD_INFO             *HdpInfo,
  IN  BOOLEAN                       IsUserPassword,
  IN  VOID                          *Password,
  IN  UINTN                         PasswordSize
  )
{
  EFI_STATUS                        Status;
  EFI_ATA_COMMAND_BLOCK             Acb;
  EFI_ATA_PASS_THRU_COMMAND_PACKET  Packet;
  

  if (HdpInfo == NULL || Password == NULL || PasswordSize > 32) {
    Status = EFI_INVALID_PARAMETER;
		goto ProcExit;
  }

  ZeroMem (&Acb, sizeof(Acb));
  ZeroMem (HdpInfo->Asb, sizeof(EFI_ATA_STATUS_BLOCK));  
  Acb.AtaCommand    = ATA_CMD_SECURITY_UNLOCK;
  Acb.AtaDeviceHead = (UINT8) (BIT7 | BIT6 | BIT5 | (HdpInfo->PortMp == 0xFFFF ? 0 : (HdpInfo->PortMp << 4))); 

  ZeroMem(HdpInfo->PayLoad, HdpInfo->PayLoadSize);
  HdpInfo->PayLoad[0] = IsUserPassword?0:1;
  CopyMem (&HdpInfo->PayLoad[1], Password, PasswordSize);

//DumpMem8(HdpInfo->PayLoad, 64);
  
  ZeroMem(&Packet, sizeof(Packet));
  Packet.Protocol = EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_OUT;
  Packet.Length   = EFI_ATA_PASS_THRU_LENGTH_BYTES;
  Packet.Asb      = HdpInfo->Asb;
  Packet.Acb      = &Acb;
  Packet.OutDataBuffer     = HdpInfo->PayLoad;
  Packet.OutTransferLength = (UINT32)HdpInfo->PayLoadSize;
  Packet.Timeout           = ATA_TIMEOUT;

  Status = HdpInfo->AtaPassThru->PassThru (
                          HdpInfo->AtaPassThru,
                          HdpInfo->Port,
                          HdpInfo->PortMp,
                          &Packet,
                          NULL
                          );
	
  ZeroMem(HdpInfo->PayLoad, HdpInfo->PayLoadSize);
	
  if(!EFI_ERROR(Status)){
    if(HdpInfo->Asb->AtaStatus & BIT0){							// Error
      Status = EFI_DEVICE_ERROR;
    }			
  }		

ProcExit:
  return Status;
}




EFI_STATUS
SecuritySetHddPassword (
  IN  HDD_PASSWORD_INFO             *HdpInfo,
  IN  UINT8                         Identifier,         // 0, 1
  IN  UINT8                         SecurityLevel,      // 0 = High
  IN  UINT16                        RevCode, 
  IN  UINT8                         *Password,
  IN  UINTN                         PasswordSize 
  )
{
  EFI_STATUS                        Status;
  EFI_ATA_COMMAND_BLOCK             Acb;
  EFI_ATA_PASS_THRU_COMMAND_PACKET  Packet;

  
  if(HdpInfo == NULL || Password == NULL || PasswordSize > 32 || PasswordSize == 0) {
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  DumpMem8(Password, PasswordSize);
  
  ZeroMem(&Acb, sizeof(Acb));
  ZeroMem (HdpInfo->Asb, sizeof(EFI_ATA_STATUS_BLOCK));  
  Acb.AtaCommand    = ATA_CMD_SECURITY_SET_PASSWORD;
  Acb.AtaDeviceHead = (UINT8) (BIT7 | BIT6 | BIT5 | (HdpInfo->PortMp == 0xFFFF ? 0 : (HdpInfo->PortMp << 4)));   

  ZeroMem(HdpInfo->PayLoad, HdpInfo->PayLoadSize);
  CopyMem(&HdpInfo->PayLoad[1], Password, PasswordSize);
  HdpInfo->PayLoad[0] = (UINT16)(Identifier | (SecurityLevel << 8));
  if (Identifier & BIT0) {
    HdpInfo->PayLoad[17] = RevCode;
  }

  DumpMem8(HdpInfo->PayLoad, 64);
  
  ZeroMem(&Packet, sizeof(Packet));
  Packet.Protocol          = EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_OUT;
  Packet.Length            = EFI_ATA_PASS_THRU_LENGTH_BYTES;
  Packet.Asb               = HdpInfo->Asb;
  Packet.Acb               = &Acb;
  Packet.OutDataBuffer     = HdpInfo->PayLoad;
  Packet.OutTransferLength = (UINT32)HdpInfo->PayLoadSize;  
  Packet.Timeout           = ATA_TIMEOUT;

  Status = HdpInfo->AtaPassThru->PassThru (
                            HdpInfo->AtaPassThru,
                            HdpInfo->Port,
                            HdpInfo->PortMp,
                            &Packet,
                            NULL
                            );
  ZeroMem(HdpInfo->PayLoad, HdpInfo->PayLoadSize);

ProcExit:
  return Status;
}



EFI_STATUS
SecurityDisableHddPassword (
  IN  HDD_PASSWORD_INFO             *HdpInfo,
  IN  UINT8                         Identifier,
  IN  UINT8                         *Password,
  IN  UINTN                         PasswordSize 
  )
{
  EFI_STATUS                        Status;
  EFI_ATA_COMMAND_BLOCK             Acb;
  EFI_ATA_PASS_THRU_COMMAND_PACKET  Packet;


  if (HdpInfo == NULL || Password == NULL || PasswordSize > 32 || PasswordSize == 0) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem(&Acb, sizeof(Acb));
  ZeroMem(HdpInfo->Asb, sizeof(EFI_ATA_STATUS_BLOCK));
  Acb.AtaCommand    = ATA_CMD_SECURITY_DISABLE_PASSWORD;
  Acb.AtaDeviceHead = (UINT8) (BIT7 | BIT6 | BIT5 | (HdpInfo->PortMp << 4)); 

  ZeroMem(HdpInfo->PayLoad, sizeof(HdpInfo->PayLoad));
  HdpInfo->PayLoad[0] = Identifier & BIT0;
  CopyMem (&HdpInfo->PayLoad[1], Password, PasswordSize);
  
  ZeroMem (&Packet, sizeof(Packet));
  Packet.Protocol = EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_OUT;
  Packet.Length   = EFI_ATA_PASS_THRU_LENGTH_BYTES;
  Packet.Asb      = HdpInfo->Asb;
  Packet.Acb      = &Acb;
  Packet.OutDataBuffer     = HdpInfo->PayLoad;
  Packet.OutTransferLength = (UINT32)HdpInfo->PayLoadSize;
  Packet.Timeout           = ATA_TIMEOUT;

  Status = HdpInfo->AtaPassThru->PassThru (
                          HdpInfo->AtaPassThru,
                          HdpInfo->Port,
                          HdpInfo->PortMp,
                          &Packet,
                          NULL
                          );

  ZeroMem(HdpInfo->PayLoad, sizeof(HdpInfo->PayLoad));
  return Status;

}




EFI_STATUS
SecurityFrozenLock (
    IN  HDD_PASSWORD_INFO       *HdpInfo
  )
{
  EFI_STATUS                        Status;
  EFI_ATA_COMMAND_BLOCK             Acb;
  EFI_ATA_PASS_THRU_COMMAND_PACKET  Packet;


  if (HdpInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&Acb, sizeof (Acb));
  ZeroMem (&HdpInfo->Asb, sizeof(EFI_ATA_STATUS_BLOCK));
  Acb.AtaCommand    = ATA_CMD_SECURITY_FREEZE_LOCK;
  Acb.AtaDeviceHead = (UINT8) (BIT7 | BIT6 | BIT5 | (HdpInfo->PortMp << 4)); 

  ZeroMem (&Packet, sizeof (Packet));
  Packet.Protocol = EFI_ATA_PASS_THRU_PROTOCOL_ATA_NON_DATA;
  Packet.Length   = EFI_ATA_PASS_THRU_LENGTH_BYTES | EFI_ATA_PASS_THRU_LENGTH_SECTOR_COUNT;
  Packet.Asb      = HdpInfo->Asb;
  Packet.Acb      = &Acb;
  Packet.Timeout  = ATA_TIMEOUT;

  Status = HdpInfo->AtaPassThru->PassThru (
                                  HdpInfo->AtaPassThru,
                                  HdpInfo->Port,
                                  HdpInfo->PortMp,
                                  &Packet,
                                  NULL
                                  );
  
  return Status;
}



EFI_STATUS
GetHddDeviceModelNumber (
  IN ATA_IDENTIFY_DATA             *IdentifyData,
  IN OUT CHAR16                    *String
  )
{
  UINTN             Index;

  for (Index = 0; Index < 40; Index += 2) {
    String[Index]      = IdentifyData->ModelName[Index + 1];
    String[Index + 1]  = IdentifyData->ModelName[Index];
  }

	Index--;		// now point to last wchar.
  while(Index){
		if(String[Index] == L' '){
			Index--;
		}else{
		  break;
		}
  }
	String[Index+1] = L'\0';
			
  return EFI_SUCCESS;
}



HDD_PASSWORD_CONFIG_FORM_ENTRY *
HddPasswordGetConfigFormEntryByIndex (
    IN UINTN Index
  )
{
  LIST_ENTRY                     *Entry;
  UINTN                          CurrentIndex;
  HDD_PASSWORD_CONFIG_FORM_ENTRY *ConfigFormEntry;

  CurrentIndex    = 0;
  ConfigFormEntry = NULL;

  EFI_LIST_FOR_EACH (Entry, &mHddPasswordConfigFormList) {
    if (CurrentIndex == Index) {
      ConfigFormEntry = BASE_CR (Entry, HDD_PASSWORD_CONFIG_FORM_ENTRY, Link);
      break;
    }

    CurrentIndex++;
  }

  return ConfigFormEntry;
}


EFI_STATUS
GetHddPasswordSecurityStatus (
  IN     ATA_IDENTIFY_DATA    *IdentifyData,
  IN OUT HDD_PASSWORD_CONFIG  *IfrData
  )
{
  UINT16  RevisionCode;
  
  IfrData->Supported           = (IdentifyData->command_set_supported_82 & BIT1) ? 1 : 0;
  IfrData->Enabled             = (IdentifyData->security_status & BIT1) ? 1 : 0;
  IfrData->Locked              = (IdentifyData->security_status & BIT2) ? 1 : 0;
  IfrData->Frozen              = (IdentifyData->security_status & BIT3) ? 1 : 0;
  IfrData->CountExpired        = (IdentifyData->security_status & BIT4) ? 1 : 0;
  IfrData->UserPasswordStatus  = IfrData->Enabled;
  
  RevisionCode = IdentifyData->master_password_identifier;
  DEBUG((EFI_D_INFO, "RevCode:%X\n", RevisionCode));
  switch(RevisionCode){
    default:
    case MASTER_PASSWORD_DEFAULT_REVCODE:
      IfrData->MasterPasswordStatus = 0;
      break;

    case MASTER_PASSWORD_USER_REVCODE:
      IfrData->MasterPasswordStatus = 1;
      break;      

    case MASTER_PASSWORD_USER_MASTER_REVCODE:
      IfrData->MasterPasswordStatus = 2;
      break;
  }
  return EFI_SUCCESS;
}



VOID *AllocAtaBuffer (
  EFI_ATA_PASS_THRU_PROTOCOL    *AtaPassThru,
  UINTN                         BufferSize,
  VOID                          **Alloc
  )
{
  VOID  *Buffer;
  VOID  *p;

  Buffer = (VOID*)AllocatePool(BufferSize + AtaPassThru->Mode->IoAlign);
  if(Buffer == NULL){
    return NULL;
  }
  p = ALIGN_POINTER(Buffer, AtaPassThru->Mode->IoAlign);
  *Alloc = Buffer;
  return p;
}


