/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/


#include "IpmiBoot.h"
#include "Protocol/SimpleFileSystem.h"
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>

#include <Library/UefiBootManagerLib.h>
#include <Guid/ServerVariable.h>
#include <SetupVariable.h>
#include <Library/PlatformCommLib.h>

#define  UEFI_SHELL       0xffff

IPMI_BOOT_PROTOCOL        mIpmiBootProtocol;
IPMI_FORCE_BOOT_OPTIONS   mIpmiForceBootOptions;

extern BOOLEAN  gLoadDefault;

EFI_STATUS
IpmiBootEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++

  Routine Description:
    This is the standard EFI driver entry point. This function installs
    the IpmiBoot protocol. 
    
  Arguments:
    ImageHandle     - Handle for the image of this driver
    SystemTable     - Pointer to the EFI System Table

  Returns:
    EFI_SUCCESS     - Protocol successfully started and installed

--*/
{
  EFI_STATUS  Status;
  EFI_HANDLE  NewHandle;
  UINTN       TempBufferSize;
//  EFI_HOB_GUID_TYPE         *GuidHob;
//  UINT8                     HobData = 0;
 SETUP_DATA                *SetupData;

  
  // Try whether user load default in BIOS setup
  //
  TempBufferSize = sizeof (UINTN);
  Status = gRT->GetVariable (
                  LOAD_CUSTOMIZED_VARIABLE_NAME,
                  &gEfiServerVariableGuid,
                  NULL,
                  &TempBufferSize,
                  &gLoadDefault
                  );
  if (EFI_ERROR (Status)) {
    //
    // Try whether user load default with utility
    //
    gLoadDefault = FALSE;
  }

  if (gLoadDefault) {
    DEBUG ((EFI_D_INFO, "[IpmiBoot] Catch loaddefault flag, refresh IpmiBootOrder.\n"));
  }

  SetupData = (SETUP_DATA*)GetSetupDataHobData();
  
  NewHandle = NULL;
  EfiInitializeIpmiBase ();

  mIpmiBootProtocol.ProcessBootFlags      = ProcessIpmiBootFlags;
  mIpmiBootProtocol.ProcessBootOrderTable = ProcessIpmiBootOrderTable;
  mIpmiBootProtocol.ProcessF2BOT          = ProcessIpmiF2BootOrderTable;

  Status = gBS->InstallProtocolInterface (
                &NewHandle,
                &gEfiIpmiBootGuid,
                EFI_NATIVE_INTERFACE,
                &mIpmiBootProtocol
                );
  DEBUG ((EFI_D_INFO, "[IpmiBoot] Install gEfiIpmiBootGuid Protocol\n"));

  return Status;
}

EFI_STATUS
ProcessIpmiBootFlags (
  VOID
  )
/*++

Routine Description:
  
Check for IPMI Force Boot request and Set the BootNext variable with 
    boot index of the requested device. 

Arguments:
  
  None
  
Returns:

  EFI_NOT_FOUND - unsupported boot device selector

--*/
{
  UINTN                   BootIndex = 0; // c4701
  EFI_STATUS              Status;

  UINT8                   IpmiForcedPersistent;       // 1= OOB SW changed the order of Boot Devices - this has been detected 
                                                      //   so we need to communicate it thru this EFI variable
  UINTN                   IpmiForcedPersistentSize;
  EFI_STATUS              VarStatus;


  DEBUG((EFI_D_ERROR,"%a\n",__FUNCTION__));
  IpmiForcedPersistentSize = sizeof(IpmiForcedPersistent);

  //
  // Check if IPMI Force Boot.
  //
  GetForceBootOptions (&mIpmiForceBootOptions);
  DEBUG((EFI_D_ERROR,"GetForceBootOptions BootValid =%x  mIpmiForceBootOptions.BootDevice =%x\n",mIpmiForceBootOptions.BootValid,mIpmiForceBootOptions.BootDevice));
  
  if (mIpmiForceBootOptions.BootValid == 0) {
    return EFI_NOT_FOUND;
  }

  if (mIpmiForceBootOptions.BootDevice) 
  {
    //
    // Currently we support PXE, HD, CD/DVD, floppy
    // EFI shell (non-standard) and Setup. 
    //
    Status = EFI_NOT_FOUND;
    switch (mIpmiForceBootOptions.BootDevice) 
    {
      case ForceBootPxe:
        Status = FindEfiBootVariableByType (
                  BBS_TYPE_EMBEDDED_NETWORK,
                  &BootIndex
                  );
          DEBUG((EFI_D_ERROR,"[IPMIBoot] ForceBootPxe -%r\n",Status));
                  
        break;

      case ForceBootLegacyHardDrive:
        Status = FindEfiBootVariableByType (
                  BBS_TYPE_HARDDRIVE,
                  &BootIndex
                  );
          DEBUG((EFI_D_ERROR,"[IPMIBoot] ForceBootLegacyHardDrive -%r\n",Status));
                  
        break;

      case ForceBootLegacyCdDvd:
        Status = FindEfiBootVariableByType (
                  BBS_TYPE_CDROM,
                  &BootIndex
                  );
          DEBUG((EFI_D_ERROR,"[IPMIBoot] ForceBootLegacyCdDvd -%r\n",Status));
                  
        break;

      case ForceBootBiosSetup:
        //
        //Open: for EDKII setup engine
        //
        {
          BOOLEAN        IpmiForceSetupFlag;

          IpmiForceSetupFlag = TRUE;
          Status = gRT->SetVariable (
                        L"IpmiBootForceIntoSetup",
                        &gEfiGenericVariableGuid,
                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        sizeof (BOOLEAN),
                        &IpmiForceSetupFlag
                        );
          DEBUG((EFI_D_ERROR,"[IPMIBoot] ForceBootBiosSetup\n"));
          Status = EFI_NOT_FOUND;
        }
        break;

      case ForceBootEfiShell:
        Status = FindEfiBootVariableByType (
                  UEFI_SHELL,
                  &BootIndex
                  );
          DEBUG((EFI_D_ERROR,"[IPMIBoot] ForceBootEfiShell -%r\n",Status));
                  
        break;

      case ForceBootLegacyFloppy:
        Status = FindEfiBootVariableByType (
                  BBS_TYPE_FLOPPY,
                  &BootIndex
                  );
        break;

      default:
        Status = EFI_NOT_FOUND;
        break;
    }
      DEBUG((EFI_D_ERROR,"FindEfiBootVariableByType Boot Index= %x Status =%r\n",BootIndex,Status));

    if (!EFI_ERROR (Status)) 
    {
      //
      // Update BootNext to point to our desired boot
      //
      Status = gRT->SetVariable (L"BootNext", &gEfiGlobalVariableGuid, VAR_FLAG, sizeof(UINT16), &BootIndex);
      if( !EFI_ERROR(Status)) 
      {
        if ( mIpmiForceBootOptions.PersistentOptions) {
          DEBUG((EFI_D_ERROR,"UpdateBootOrderWithBootNext \n"));
          UpdateBootOrderWithBootNext();
          IpmiForcedPersistent = 1;

        } else {
          IpmiForcedPersistent = 0;
        }

        //
        // This flag is used by other platform override feature, like SBO since IpmiBoot management console has high priority
        //
        VarStatus = gRT->SetVariable (
                        L"IpmiForcedPersistent",
                        &gEfiGenericVariableGuid,
                        EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, 
                        IpmiForcedPersistentSize,
                        &IpmiForcedPersistent
                        );
      }
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ProcessIpmiBootOrderTable (
  VOID
  )
/*++

Routine Description:
  
  Read IPMI Boot Order Table from BMC, compare and merge changes, if any.  

Arguments:
  
  None
  
Returns:

  EFI_SUCCESS - 

--*/
{
  PlatformUpdateSmBootOrder();

  return EFI_SUCCESS;
}

#define BUFFER_SIZE 50

EFI_STATUS
IpmiSetBootOptions (
  IN  BOOT_OPTION_PARAMETER_TYPE       ParameterType,
  OUT BOOT_OPTIONS_PARAMETERS          *RequestParameters
  )
/*++

Routine Description:

Arguments:

  ParameterType
  RequestParameters

Returns:

  EFI_STATUS

--*/
{
  EFI_STATUS                Status;
  UINT8                     Buffer[BUFFER_SIZE];
  UINT8                     ResponseSize;
  SET_BOOT_OPTIONS_REQUEST  *SetRequest;
  SET_BOOT_OPTIONS_RESPONSE SetResponse;
  UINT8                     RequestSize;

  //
  // Wait for Set In Progress to clear
  //
  Status = IpmiWaitSetInProgressClear ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (Buffer, BUFFER_SIZE);
  SetRequest                        = (SET_BOOT_OPTIONS_REQUEST *) &Buffer[0];
  SetRequest->ParameterSelector     = (UINT8) ParameterType;
  SetRequest->MarkParameterInvalid  = 0;

  CopyMem (
    SetRequest->ParameterData,
    RequestParameters,
    sizeof (BOOT_OPTIONS_PARAMETERS)
    );

  ResponseSize = sizeof (SET_BOOT_OPTIONS_RESPONSE);
  if (ParameterType == ParmTypeBootFlags) {
    //
    // BMC FW expects all the 5 bytes of the Boot option param 5 to be passed. If only the Valid bit field
    // is passed, the command will return an Error
    //
    RequestSize = sizeof (SET_BOOT_OPTIONS_REQUEST) + 4;  
  } else {
    RequestSize = sizeof (SET_BOOT_OPTIONS_REQUEST);
  }

  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_CHASSIS,
            EFI_CHASSIS_SET_SYSTEM_BOOT_OPTIONS,
            (UINT8 *) SetRequest,
            RequestSize,
            (UINT8 *) &SetResponse,
            &ResponseSize
            );

  //
  // What error to check for
  //
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
IpmiGetBootOptions (
  IN  BOOT_OPTION_PARAMETER_TYPE      ParameterType,
  OUT BOOT_OPTIONS_PARAMETERS         *ResponseParameters
  )
/*++

Routine Description:

Arguments:

  ParameterType
  ResponseParameters

Returns:

  EFI_STATUS

--*/
{
  EFI_STATUS                Status;
  UINT8                     Buffer[BUFFER_SIZE];
  UINT8                     ResponseSize;
  GET_BOOT_OPTIONS_REQUEST  *GetRequest;
  GET_BOOT_OPTIONS_RESPONSE *GetResponse;

  //
  // Wait for Set In Progress to clear
  //
  Status = IpmiWaitSetInProgressClear ();
  DEBUG((EFI_D_ERROR,"IpmiWaitSetInProgressClear - %r\n ",Status));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (Buffer, BUFFER_SIZE);
  GetRequest                    = (GET_BOOT_OPTIONS_REQUEST *) &Buffer[0];
  GetRequest->ParameterSelector = (UINT8) ParameterType;
  GetRequest->SetSelector       = 0;
  GetRequest->BlockSelector     = 0;

  GetResponse = (GET_BOOT_OPTIONS_RESPONSE *) GetResponse = (GET_BOOT_OPTIONS_RESPONSE *) (Buffer + sizeof (GET_BOOT_OPTIONS_REQUEST));
  ResponseSize = sizeof (BOOT_OPTIONS_PARAMETERS) + sizeof (GET_BOOT_OPTIONS_RESPONSE);
  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_CHASSIS,
            EFI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
            (UINT8 *) GetRequest,
            sizeof (GET_BOOT_OPTIONS_REQUEST),
            (UINT8 *) GetResponse,
            &ResponseSize
            );

  //
  // What error to check for
  //
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (ResponseParameters, GetResponse->ParameterData, ResponseSize);

  return EFI_SUCCESS;
}

EFI_STATUS
IpmiWaitSetInProgressClear (
  VOID
  )
/*++

Routine Description:
  Wait for IPMI Get Ssytem Boot Option Set-in-Progress bit to clear

Arguments:
  None

Returns:
  EFI_SUCCESS if passed, 
  EFI_TIMEOUT if failed

--*/
{
  EFI_STATUS                        Status;
  UINT8                             Buffer[BUFFER_SIZE];
  UINT8                             ResponseSize;
  UINT32                            Retries;
  GET_BOOT_OPTIONS_REQUEST          *GetRequest;
  GET_BOOT_OPTIONS_RESPONSE         *GetResponse;
  BOOT_OPTIONS_RESPONSE_PARAMETER_0 *Parm0;

  Retries = 10;
  ZeroMem (Buffer, BUFFER_SIZE);
  GetRequest = (GET_BOOT_OPTIONS_REQUEST *) &Buffer[0];
  GetRequest->ParameterSelector = 0; // Set in progress parameter
  GetRequest->SetSelector   = 0;
  GetRequest->BlockSelector = 0;
  GetResponse = (GET_BOOT_OPTIONS_RESPONSE *) (Buffer + sizeof (GET_BOOT_OPTIONS_REQUEST));
  ResponseSize = sizeof (GET_BOOT_OPTIONS_RESPONSE);

  //
  // Wait for in-progress bit to clear
  //
  do {
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_CHASSIS,
              EFI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
              (UINT8 *) GetRequest,
              sizeof (GET_BOOT_OPTIONS_REQUEST),
              (UINT8 *) GetResponse,
              &ResponseSize
              );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // any other checks???
    //
    Parm0 = (BOOT_OPTIONS_RESPONSE_PARAMETER_0 *) GetResponse->ParameterData;
    if (Parm0->SetInProgress == 0) {
      break;
    }

    gBS->Stall (IPMI_STALL);
  } while (Retries-- > 0);

  if (Retries == 0) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}
BOOLEAN
CheckDevicePath(
  IN EFI_DEVICE_PATH_PROTOCOL  *DevPath,
  IN UINTN                      DPType,
  IN UINTN                      DPSubType
)
{
  EFI_DEVICE_PATH_PROTOCOL      *Node;

  ASSERT(DevPath != NULL);
  
  Node = DevPath;
  for(Node; !IsDevicePathEnd(Node); Node = NextDevicePathNode(Node)){
    if((DevicePathType (Node) == DPType) && (DevicePathSubType (Node) == DPSubType)){
      return TRUE;
    }
  }
  return FALSE;
}

BOOLEAN
CheckForceUefiDevice(
  IN UINT16                       DevType,
  IN EFI_DEVICE_PATH_PROTOCOL    *DevPath
)
{
  switch (DevType) 
  {
    //Force boot to UEFI PXE
    case BBS_TYPE_EMBEDDED_NETWORK:
      if(CheckDevicePath(DevPath, MESSAGING_DEVICE_PATH, MSG_MAC_ADDR_DP)) {
        return TRUE;
      }
      break;

    //Force boot to UEFI HDD  
    case BBS_TYPE_HARDDRIVE:
      if(CheckDevicePath(DevPath, MEDIA_DEVICE_PATH, MEDIA_HARDDRIVE_DP)) {
        return TRUE;
      }
      break;

    //Force boot to UEFI CD-ROM
    case BBS_TYPE_CDROM:
      if(CheckDevicePath(DevPath, MEDIA_DEVICE_PATH, MEDIA_CDROM_DP)) {
        return TRUE;
      }
      break;

    default:
      break;
  }
  return FALSE;
}

EFI_STATUS 
FindEfiBootVariableByType(
  IN    UINT16     DeviceType,
  OUT   UINTN      *BootIndex
  )
/**++

Routine Description:
  
  Look for boot#### variable information by DeviceType

Arguments:
  
  DeviceType

Returns:

  BootIndex - Boot#### of the device

  EFI_NOT_FOUND - Device path not available in the Boot list

--*/
{
  UINTN                         Index;
  UINT16                        OptionNumber[10];
  UINT16                        *BootOrder = NULL;
  UINTN                         BootOrderSize = 0;
  EFI_STATUS                    Status;
  EFI_BOOT_MANAGER_LOAD_OPTION  LoadOption;
 
  //
  // Get Boot Order 
  //
  Status = GetVariable2 (L"BootOrder",&gEfiGlobalVariableGuid, &BootOrder, &BootOrderSize);
  if(EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  //
  // Iterate Boot#### and and look for matching device path
  //
  for (Index = 0; Index < BootOrderSize / sizeof (UINT16); Index++) {
    UnicodeSPrint (OptionNumber, sizeof (OptionNumber), L"Boot%04x", BootOrder[Index]);
    Status = EfiBootManagerVariableToLoadOption(OptionNumber, &LoadOption);
    if (EFI_ERROR(Status)) {
      continue;
    }
    
    if((LoadOption.Attributes & LOAD_OPTION_ACTIVE) == 0){
      EfiBootManagerFreeLoadOption(&LoadOption);
      continue;
    }
/*
    if(PcdGetBool (PcdUefiOptimizedBootEx) && CheckForceUefiDevice(DeviceType, LoadOption.FilePath))
    {
      *BootIndex = BootOrder[Index];
      gBS->FreePool (BootOrder);
      EfiBootManagerFreeLoadOption(&LoadOption);
      return EFI_SUCCESS;
    }
*/
    if((DeviceType != UEFI_SHELL) &&
      (BBS_DEVICE_PATH == LoadOption.FilePath->Type) &&
       (BBS_BBS_DP     == LoadOption.FilePath->SubType)
     ){
      //
      //Find boot option for legacy devices
      //
      if(DeviceType ==((BBS_BBS_DEVICE_PATH *) LoadOption.FilePath)->DeviceType){
        *BootIndex = BootOrder[Index];
        gBS->FreePool (BootOrder);
        EfiBootManagerFreeLoadOption(&LoadOption);
        return EFI_SUCCESS;
      }      
    } else {
        //
        // Find boot option for EFI Shell
        //
        if(((BBS_DEVICE_PATH != LoadOption.FilePath->Type) ||
            (BBS_BBS_DP     != LoadOption.FilePath->SubType)) &&
            (DeviceType  == UEFI_SHELL)
        ){
          if(IsInternalShell(LoadOption.FilePath)){
            *BootIndex = BootOrder[Index];
            gBS->FreePool (BootOrder);
            EfiBootManagerFreeLoadOption(&LoadOption);
            return EFI_SUCCESS;
          }
        }
     }
     
     EfiBootManagerFreeLoadOption(&LoadOption);
  }

  gBS->FreePool (BootOrder);
  return EFI_NOT_FOUND;  
}

EFI_STATUS
GetForceBootOptions(
  OUT IPMI_FORCE_BOOT_OPTIONS *IpmiForceBootOptions
)
/*++

Routine Description:
  Find boot option from BMC

Arguments:

  out IpmiForceBootOptions: Boot Options from BMC

Returns:

  Return nozero value if there is a valid boot flag.
  
--*/
{
  static int                DoneOnce = FALSE;
  EFI_STATUS                Status = EFI_SUCCESS;
  BOOT_OPTIONS_PARAMETERS   BootOptParms;
  BOOLEAN                   LoadDefaults;
  
  DEBUG((EFI_D_ERROR,"DoneOnce =%x \n",DoneOnce));
  if (DoneOnce) {
    IpmiForceBootOptions = &mIpmiForceBootOptions;
    return EFI_SUCCESS;
  }

  //
  // Read Boot Flags and check if  "Boot flag valid" is set
  //
  ZeroMem (IpmiForceBootOptions, sizeof (IPMI_FORCE_BOOT_OPTIONS));
  Status = IpmiGetBootOptions (ParmTypeBootFlags, &BootOptParms);
  DEBUG((EFI_D_ERROR," IpmiGetBootOptions - %r \n",Status));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  {
    UINTN   Size,Index;
    UINT8*  Buffer;

    Size = sizeof(BOOT_OPTIONS_PARAMETERS);
    Buffer=AllocateZeroPool(Size);
    CopyMem(Buffer, &BootOptParms, Size);
    DEBUG((EFI_D_ERROR," \n"));
    for(Index=0;Index<Size;Index++){
      DEBUG((EFI_D_ERROR," 0x%x",*(Buffer+Index)));
    }
    DEBUG((EFI_D_ERROR," \n"));
  }
  
//tesst
//  BootOptParms.Parm5.BootFlagValid = 1;
//  BootOptParms.Parm5.BootDeviceSelector  = ForceBootLegacyHardDrive;
//test end 

  IpmiForceBootOptions->BootValid = BootOptParms.Parm5.BootFlagValid;
  DEBUG((EFI_D_ERROR," BootValid - %x \n",BootOptParms.Parm5.BootFlagValid));
  if (BootOptParms.Parm5.BootFlagValid) 
  {
    //
    // Fill out secure boot,
    //
    IpmiForceBootOptions->SecureMode.s.BlankVideo = BootOptParms.Parm5.ScreenBlank;
    IpmiForceBootOptions->SecureMode.s.WriteProtectDiskette = 0;
    IpmiForceBootOptions->SecureMode.s.LockNmiButton = 0;
    IpmiForceBootOptions->SecureMode.s.LockSleepButton = BootOptParms.Parm5.LockSleep;
    IpmiForceBootOptions->SecureMode.s.LockResetButton = BootOptParms.Parm5.LockReset;
    IpmiForceBootOptions->SecureMode.s.LockPowerButton = BootOptParms.Parm5.LockPower;
    IpmiForceBootOptions->SecureMode.s.LockFrontPanel = 0;

    //Secure Mode Boot Options are not supported for Blackford
    //if (IpmiForceBootOptions->SecureMode.AsUint8) {
      //
      // Handle Secure Mode Options here
      //
      //
      //HandleSecureModeBootOptions (IpmiForceBootOptions);
    //}

    //
    // CMOS clear, console redir flags ... here
    //
    IpmiForceBootOptions->CmosClear = BootOptParms.Parm5.CmosClear;
    DEBUG((EFI_D_ERROR," CmosClear - %x \n",BootOptParms.Parm5.CmosClear));
    
    if (IpmiForceBootOptions->CmosClear ) {
      LoadDefaults = TRUE;

      //
      // Set a NV variable. It will be checked next time through PEI
      // See: OEMPort.c::IsResetConfigMode()
      // NVRAMPEI.c::PeiInitNV() calls IsResetConfigMode().
      // If set due to CMOS bad, clear NV jumper or this variable, PEI sets
      // NvramMode |= NVRAM_MODE_RESET_CONFIGURATION;
      // That gets passed to DXE where it's picked up by NVRAMDXE.c::VarStoreDiscovery()
      //
      
      Status = gRT->SetVariable (
                      L"BMCSetClearCMOS",
                      &gEfiGenericVariableGuid,
                      VAR_FLAG,
                      sizeof (BOOLEAN),
                      &LoadDefaults
                      );
    }

    //
    // Redir PW Bypass BiosVerbosity done in ConsoleAvailableEntry()
    //
    IpmiForceBootOptions->UserPasswordBypass = BootOptParms.Parm5.UserPasswordBypass;
    IpmiForceBootOptions->BiosVerbosity = BootOptParms.Parm5.BiosVerbosity;

    //
    // TBD: how to do these?? 
    //
    IpmiForceBootOptions->ConsoleRedirection = BootOptParms.Parm5.ConsoleRedirection;
    IpmiForceBootOptions->LockKeyboard = BootOptParms.Parm5.LockKeyboard;

    //
    // Get the Boot device Selector
    //
    
    IpmiForceBootOptions->BootDevice = BootOptParms.Parm5.BootDeviceSelector;
    IpmiForceBootOptions->PersistentOptions = BootOptParms.Parm5.PersistentOptions;

    //
    // TBD: Get mail box data as OEM parameters
    //

    //
    // Get service partition Selector
    //
    if (IpmiForceBootOptions->BootDevice == ForceBootServicePartition ) {
      Status = IpmiGetBootOptions (ParmTypeServicePartitionSelector, &BootOptParms);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      IpmiForceBootOptions->ServicePartitionSelector = BootOptParms.Parm1.ServicePartitionSelector; 
    }

    //
    // TBD: Handle service partition scan request
    //

    //
    // Here we need to check if the option apply for next boot only, and if yes, clear the Valid bit
    //
    DEBUG((EFI_D_ERROR," PersistentOptions - %x \n",BootOptParms.Parm5.PersistentOptions));
    
    if (BootOptParms.Parm5.PersistentOptions == 0) {
      ZeroMem (&BootOptParms, sizeof (BootOptParms));
      BootOptParms.Parm5.BootFlagValid = 0;       
//test
//      Status = IpmiSetBootOptions(ParmTypeBootFlags, &BootOptParms);
//test end

      if (EFI_ERROR (Status)) {            
        return Status;
      }
    }
    
    //
    // Write to Boot Info Acknowlege "BIOS/POST has handled boot info"
    //
    if (BootOptParms.Parm4.WriteMask & BOOT_OPTION_HANDLED_BY_BIOS) {
      ZeroMem (&BootOptParms, sizeof (BootOptParms));
      BootOptParms.Parm4.BootInitiatorAcknowledgeData = BOOT_OPTION_HANDLED_BY_BIOS;
      Status = IpmiSetBootOptions(ParmTypeBootInfoAck, &BootOptParms);
    }
  }
  DoneOnce = TRUE;
  return Status;
}

VOID
UpdateBootOrderWithBootNext(
  VOID
)
/*++

Routine Description:

  Update the Boot Order based on a new Boot Next value

Arguments:

  None

Returns:

  None

--*/
{
  static BOOLEAN    DoneOnce = FALSE;
  UINT16           *BootOrder;
  UINTN             BootOrderSize;
  UINT16           *NewBootOrder;
  UINTN             NewBootOrderSize;
  EFI_STATUS        Status;
  UINT16            BootNextIndex;
  UINTN             BootNextSize = 2;
  UINTN             index;
  UINTN             i;

  if (DoneOnce) {
    //
    // Prevent from running mulitple times in one boot and losing configured Boot Order.
    //
    return;
  }

  Status = GetVariable2 (L"BootOrder", &gEfiGlobalVariableGuid, &BootOrder, &BootOrderSize);
  if (!EFI_ERROR(Status)) {

    Status = gRT->GetVariable (
                    L"BootNext",
                    &gEfiGlobalVariableGuid,
                    NULL,
                    &BootNextSize,
                    &BootNextIndex
                    );

    if (EFI_ERROR(Status)) {
      gBS->FreePool (BootOrder);
      return;
    }

    NewBootOrderSize = BootOrderSize;
    NewBootOrder = AllocatePool (NewBootOrderSize);
    if (NewBootOrder == NULL) {
      gBS->FreePool (BootOrder);
      return;
    }

    //
    // Create the new boot order and save to NVRAM
    //
    index = 0;
    NewBootOrder[index] = BootNextIndex;
    for (i=0; i<(BootOrderSize/sizeof(UINT16)); i++) {
      if (BootOrder[i] != BootNextIndex) {
        NewBootOrder[++index] = BootOrder[i];
      }
    }

    Status = gRT->SetVariable (
                  L"BootOrder", 
                  &gEfiGlobalVariableGuid,
                  VAR_FLAG,
                  NewBootOrderSize, 
                  NewBootOrder
                  );

    gBS->FreePool (NewBootOrder);
    gBS->FreePool (BootOrder);

  }

  DoneOnce = TRUE;
  return;
}

