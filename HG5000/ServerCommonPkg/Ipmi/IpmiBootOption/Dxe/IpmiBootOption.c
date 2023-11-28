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
#  IpmiBootOption.c
#
#Abstract:
#  ServerMgmt features via IPMI commands (IPMI2.0 Spc rev1.1 2013)
#
#Revision History:
#
#TIME:       2017-7-5
#$AUTHOR:    Phinux Qin
#$REVIEWERS:
#$SCOPE:     All BMC Platforms
#$TECHNICAL:
#
#
#$END--------------------------------------------------------------------
#
--*/

#include "IpmiBootOption.h"

//
//The follow Guid is not a standard EDK definition, but it has been used for many GENs.
//So copy it here becaused it would be located in different *.dec.
//
STATIC EFI_GUID  gSignalBeforeEnterSetupGuid  = 
{ 0xd9f1669a, 0xf505, 0x48bd, { 0xa8, 0x92, 0x94, 0xb7, 0xca, 0x90, 0x30, 0x31 } };

IPMI_BOOT_OPTIONS_PROTOCOL  mIpmiBootOptionProtocol = {
  ProcessIpmiSystemBootOptionParameter5
};

SYSTEM_BOOT_OPTION_PARAMETER5         mBootOptionParam5;

/**

    GC_TODO: add routine description

    VOID     - GC_TODO: add arg description

**/
EFI_STATUS
WaitForInProgess (
  VOID
  )
{
  EFI_STATUS  Status;
  UINT8       CmdDataBuff[10];
  UINT8       CmdDataSize;
  UINT8       ResponseBuff[34];
  UINT8       ResponseSize;
  UINTN       Retry = 50;

  CmdDataBuff[0] = 0;
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
    if (!EFI_ERROR (Status) && (0 == ResponseBuff[2])) {
      DEBUG ((EFI_D_INFO, "WaitForInProgess completed\n"));
      break;
    }
    gBS->Stall (1000);
    Retry--;
  } while (0 != Retry);
  
  if (0 == Retry) {
    return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}

/**

    GC_TODO: add routine description

    @param Buffer     - GC_TODO: add arg description
    @param BufferSize     - GC_TODO: add arg description
    @param ParamX     - GC_TODO: add arg description

    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description
    @retval EFI_NOT_FOUND         - GC_TODO: add retval description
    @retval Status                - GC_TODO: add retval description

**/
EFI_STATUS
GetSystemBootOptionParamX (
  IN OUT UINT8  *Buffer,
  IN OUT UINTN  *BufferSize,
  IN     UINT8  ParamX
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
  CmdDataBuff[0] = ParamX;
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
  DEBUG ((EFI_D_INFO, "Get System Boot Option[%d] completed\n", ParamX));
  return EFI_SUCCESS;
}

/**

    GC_TODO: add routine description

    @param Buffer     - GC_TODO: add arg description
    @param BufferSize     - GC_TODO: add arg description
    @param ParamX     - GC_TODO: add arg description

    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description
    @retval EFI_NOT_FOUND         - GC_TODO: add retval description
    @retval Status                - GC_TODO: add retval description

**/
EFI_STATUS
SetSystemBootOptionParamX (
  IN UINT8  *Buffer,
  IN UINTN  BufferSize,
  IN UINT8  ParamX
  )
{
  EFI_STATUS  Status;
  UINT8       CmdDataBuff[0xCF];
  UINT8       CmdDataSize;
  UINT8       ResponseBuff[10];
  UINT8       ResponseSize;
  UINTN       Retry = 3;

  if (NULL ==  Buffer) {
    return EFI_INVALID_PARAMETER;
  }
  
  Status = WaitForInProgess();
  if (EFI_SUCCESS != Status) {
    return Status;
  }

  CmdDataBuff[0] = ParamX;
  CopyMem (&CmdDataBuff[1], Buffer, BufferSize);
  CmdDataSize = 1 + (UINT8)BufferSize;
  ResponseSize = sizeof (ResponseBuff);
  SetMem (ResponseBuff, ResponseSize, 0);

  //
  //When setting lan parameters, we need check InProgress bit.
  //Since Lan setting need Linux kernel system invoking, it would take 5 seconds.
  //
  do {
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_CHASSIS,
              EFI_CHASSIS_SET_SYSTEM_BOOT_OPTIONS,
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
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((EFI_D_INFO, "Set System Boot Option[%d] completed\n", ParamX));
  return EFI_SUCCESS;
}

/**

    GC_TODO: add routine description

    @param BootOptionParam5     - GC_TODO: add arg description

    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description
    @retval EFI_NOT_FOUND         - GC_TODO: add retval description
    @retval Status                - GC_TODO: add retval description

**/
EFI_STATUS
ProcessBootOptionParam5 (
  IN SYSTEM_BOOT_OPTION_PARAMETER5  *BootOptionParam5
  )
{
  UINT32                            BmcUsbPortInfo;
  EFI_STATUS                        Status;
  UINTN                             Size;
  SYSTEM_BOOT_OPTION_PARAMETER4     BootOptionParam4;

  //
  //Check Boot Flag is valid.
  //
  DEBUG((EFI_D_INFO,"boot option bootflag = %0x,DeviceInstanceSelector = %0x\n",BootOptionParam5->Data1.Bits.BootFlagValid,BootOptionParam5->Data2.Bits.BootDeviceSelector));
  if (0 == BootOptionParam5->Data1.Bits.BootFlagValid) {
    //
    //Check whether we need set some configuration on previous set.
    //
    //
    // Need more test to check wheter we need backup ForeceToSetup.
    // That would be a terriable user experience if this action must clear by cmd again.
    //
    //SetBootVarsFromLastPersistent ();
    DEBUG ((EFI_D_INFO, "Boot Flag Valid bit (0)\n"));
    return EFI_SUCCESS;
  }
  //
  //Change L"BootNext" L"BootOrder" if boot device is boot options device.
  //
  BmcUsbPortInfo = PcdGet32 (PcdBmcUsbPort);
  OverrideBootVars (BootOptionParam5, BmcUsbPortInfo);
  //
  //Invalid Boot Flag.
  //always clear valid bit to avoid bios setup cannot change bootorder
//  if (0 == BootOptionParam5->Data1.Bits.Persistent) {
	DEBUG ((EFI_D_INFO, "Chuang0429 BootFlagValid = 0\n"));
    mBootOptionParam5.Data1.Bits.BootFlagValid = 0;
	//mBootOptionParam5.Data1.Bits.Persistent=0;	//this bit and the boot flags valid bit
												//must be set simultaneously.
    //Status = SetSystemBootOptionParamX ((UINT8 *)(&mBootOptionParam5), sizeof (SYSTEM_BOOT_OPTION_PARAMETER5), 5);
  //}
  Status = WaitForInProgess();
  if (EFI_SUCCESS != Status) {
    goto ERROR_EXIT;
  }

  //
  //Set Bios Acknowledge
  //
  Size = sizeof (SYSTEM_BOOT_OPTION_PARAMETER4);
  SetMem (&BootOptionParam4, Size, 0);
  Status = GetSystemBootOptionParamX ((UINT8 *)(&BootOptionParam4), &Size, 4);
  if (EFI_SUCCESS != Status) {
    goto ERROR_EXIT;
  }

  if (BootOptionParam4.WriteMask &0x01) {
    BootOptionParam4.BootInitiatorAck = 0x01;
    Status = SetSystemBootOptionParamX ((UINT8 *)(&BootOptionParam4), sizeof (SYSTEM_BOOT_OPTION_PARAMETER4), 4);
  }

ERROR_EXIT:
  //
  //Leave for future extension.
  //
  FreeIpmiBootOptionStruct ();
  return Status;
}

/**
  Get Ipmi System Boot Option parameter5 and overrive local boot option

  @param This               Pointer to IPMI_BOOT_OPTIONS_PROTOCOL

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_INVALID_PARAMETERS

**/
EFI_STATUS
EFIAPI
ProcessIpmiSystemBootOptionParameter5 (
  IN IPMI_BOOT_OPTIONS_PROTOCOL         *This
  )
{
  EFI_STATUS    Status;

  if (NULL == This) {
    return EFI_INVALID_PARAMETER;
  }

  Status = ProcessBootOptionParam5 (&mBootOptionParam5);

  return Status;
}

/**

    GC_TODO: add routine description

    @retval EFI_SUCCESS
    @retval EFI_DEVICE_ERROR
    @retval EFI_INVALID_PARAMETERS
**/
EFI_STATUS
EFIAPI
InitIpmiBootOptions (
  VOID
  ) 
{
  EFI_STATUS                      Status;
  UINTN                           Size;
  EFI_HOB_GUID_TYPE              *GuidHob;
  SYSTEM_BOOT_OPTION_PARAMETER5  *HobBootOptionParam5 = NULL;
  UINT8                          BiosBootMode;

  //
  // T.O.D.O
  // IPMI PEI transport services in plan. This is because some settings from IPMI boot Options should not be done
  //  on DXE POST stage, like clear CMOS, BOOT mode etc, even debug mode.
  //
  GuidHob = GetFirstGuidHob (&gEfiIpmiBootOptionGuid);
  if (NULL != GuidHob) {
    HobBootOptionParam5 = GET_GUID_HOB_DATA (GuidHob);
  }
  if (NULL == HobBootOptionParam5) {
    //
    //Check System Boot Option Parameters ready.
    //
    Status = WaitForInProgess();
    if (EFI_SUCCESS != Status) {
      return Status;
    }
    //
    //Get System Boot Option Parameter 5.
    //
    Size = sizeof (SYSTEM_BOOT_OPTION_PARAMETER5);
    SetMem (&mBootOptionParam5, Size, 0);
    Status = GetSystemBootOptionParamX ((UINT8 *)(&mBootOptionParam5), &Size, 5);

    if (EFI_SUCCESS != Status || Size != sizeof (SYSTEM_BOOT_OPTION_PARAMETER5)) {
      return Status;
    }
  } else {
    CopyMem (&mBootOptionParam5, HobBootOptionParam5, sizeof (SYSTEM_BOOT_OPTION_PARAMETER5));
  }
  if((mBootOptionParam5.Data1.Bits.BootFlagValid== 1) && 
  (mBootOptionParam5.Data1.Bits.Persistent == 1) && 
  (mBootOptionParam5.Data2.Bits.BootDeviceSelector == 0x6)){
    CmosWrite(0x50, 0x01);
	DEBUG((EFI_D_INFO,"CMOS write 1 : %x\n",CmosRead(0x50)));
  }
  if((mBootOptionParam5.Data1.Bits.BootFlagValid== 1) &&
  ((mBootOptionParam5.Data1.Bits.Persistent != 1) || 
  (mBootOptionParam5.Data2.Bits.BootDeviceSelector != 0x6))){
    CmosWrite(0x50, 0x00);
	DEBUG((EFI_D_INFO,"CMOS write 0 : %x\n",CmosRead(0x50)));
  }
  if(CmosRead(0x50) == 0x01){
    PcdSetBoolS (PcdIpmiBootToSetup, TRUE);
  }else {
    PcdSetBoolS (PcdIpmiBootToSetup, FALSE);
  }

  if (mBootOptionParam5.Data1.Bits.BootFlagValid) {
    if (mBootOptionParam5.Data2.Bits.CmosClear) {
      PcdSetBoolS (PcdIpmiClearCmos, TRUE);
    } else {
      PcdSetBoolS (PcdIpmiClearCmos, FALSE);
    }
    if (0x6 == mBootOptionParam5.Data2.Bits.BootDeviceSelector) {
      PcdSetBoolS (PcdIpmiBootToSetup, TRUE);
    } else {
      PcdSetBoolS (PcdIpmiBootToSetup, FALSE);
    }
	if(mBootOptionParam5.Data1.Bits.Persistent == 0 && CmosRead(0x51) == 0x0){
	  BiosBootMode = PcdGet8(PcdBiosLastBootModeType);     //1.UEFI 2.LEGACY
      CmosWrite(0x51, 0x01);
      CmosWrite(0x52, BiosBootMode);
	}
  }
  return EFI_SUCCESS;
}

/**

    GC_TODO: add routine description

    @param Event - GC_TODO: add retval description
    @param Context - GC_TODO: add retval description

    @retval TRUE  - GC_TODO: add retval description
    @retval FALSE - GC_TODO: add retval description

**/
VOID
AfterConsoleEndHook (
    VOID
  )
{
  DEBUG ((EFI_D_INFO, "Ipmi Boot Option CallBack\n"));
  ProcessIpmiSystemBootOptionParameter5 (&mIpmiBootOptionProtocol);
}
  
VOID IpmiBeforeDefaultBootHook()
{

  EFI_STATUS          Status;
  EFI_BDS_BOOT_MANAGER_PROTOCOL  *BootManagerProtocol;
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if(!PcdGetBool (PcdIpmiBootToSetup)){
    return;
  }
  Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, &BootManagerProtocol);
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "[ERROR] EFI_BDS_BOOT_MANAGER_PROTOCOL not found\n"));
    return;
  }
  BootManagerProtocol->BootApp (&gSetupFileGuid); 
 
}
/**
  Entry point for the Core Handler initialization. 

  This function add services to change system boot order from IPMI spc2.0 rev1.1 "Get Boot Options cmd".
  

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval Status.

**/
EFI_STATUS
IpmiBootOptionEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              Handle;
  //EFI_EVENT               BeforSetupEvent;
  //VOID                    *BeforSetupRegistration;

  Handle = NULL;

  EfiInitializeIpmiBase ();

  Status = gBS->InstallProtocolInterface (
    &Handle,
    &gEfiIpmiBootOptionGuid,
    EFI_NATIVE_INTERFACE,
    &mIpmiBootOptionProtocol
    ); 

  InitIpmiBootOptions ();

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gPlatAfterConsoleEndProtocolGuid, AfterConsoleEndHook,
                  &gPlatBeforeDefaultBootProtocolGuid,IpmiBeforeDefaultBootHook,
                  NULL
                  );

  return EFI_SUCCESS;
}