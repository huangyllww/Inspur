/** @file
==========================================================================================
      NOTICE: Copyright (c) 2006 - 2019 Byosoft Corporation. All rights reserved.
              This program and associated documentation (if any) is furnished
              under a license. Except as permitted by such license,no part of this
              program or documentation may be reproduced, stored divulged or used
              in a public system, or transmitted in any form or by any means
              without the express written consent of Byosoft Corporation.
==========================================================================================

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/GuidNameLib.h>


typedef struct {
  EFI_GUID          *Guid;
  CHAR8             *Name;
} GUID_NAME;


STATIC EFI_GUID gScsiBusDxeGuid = 
  {0x0167CCC4, 0xD0F7, 0x4f21, {0xA3, 0xEF, 0x9E, 0x64, 0xB7, 0xCD, 0xCE, 0x8B}};

STATIC EFI_GUID gBootScriptExecutorDxeFileGuid = 
  {0xFA20568B, 0x548B, 0x4b2b, {0x81, 0xEF, 0x1B, 0xA0, 0x8D, 0x4A, 0x3C, 0xEC}};


STATIC GUID_NAME gGuidNameList[] = {
//                                         "5B1B31A1-9562-11D2-8E3F-00A0C969723B" 
 {&gEfiBlockIoProtocolGuid,                "BlockIo        "},
 {&gEfiDevicePathProtocolGuid,             "DevPath        "},
 {&gEfiUsbIoProtocolGuid,                  "UsbIo          "},
 {&gEfiDiskInfoProtocolGuid,               "DiskInfo       "},
 {&gEfiDiskIoProtocolGuid,                 "DiskIo         "},
 {&gEfiHiiConfigAccessProtocolGuid,        "HiiConfigAccess"},
 {&gEfiFirmwareManagementProtocolGuid,     "FMP            "},
 {&gEfiScsiPassThruProtocolGuid,           "ScsiPassThru   "},
 {&gEfiExtScsiPassThruProtocolGuid,        "ExtScsiPassThru"},
 {&gEfiAtaPassThruProtocolGuid,            "AtaPassThru    "},
 {&gEfiLoadedImageProtocolGuid,            "LoadedImage    "},
 {&gEfiLoadedImageDevicePathProtocolGuid,  "LoadedImageDP  "},
 {&gEfiScsiIoProtocolGuid,                 "ScsiIo         "},
 {&gScsiBusDxeGuid,                        "ScsiBusDxe     "},
 {&gEfiPciIoProtocolGuid,                  "PciIo          "},
 {&gEfiUsbHcProtocolGuid,                  "UsbHc          "},
 {&gEfiUsb2HcProtocolGuid,                 "Usb2Hc         "},
 {&gEfiBlockIo2ProtocolGuid,               "BlockIo2       "},
 {&gEfiLoadFileProtocolGuid,               "LoadFile       "},
 {&gEfiDecompressProtocolGuid,             "Decompress     "},
 {&gEfiFirmwareVolumeBlock2ProtocolGuid,   "FvBlk2         "},
 {&gEfiFirmwareVolume2ProtocolGuid,        "Fv2            "},
 {&gEfiIsaIoProtocolGuid,                  "IsaIo          "},
 {&gEfiSimpleTextInProtocolGuid,           "SimpleTextIn   "},
 {&gEfiSimpleTextInputExProtocolGuid,      "SimpleTextInEx "},
 {&gEfiConsoleInDeviceGuid,                "ConsoleInDev   "},
 {&gEfiIsaAcpiProtocolGuid,                "IsaAcpi        "},
 {&gEfiSimplePointerProtocolGuid,          "SimplePointer  "}, 
 {&gEfiSerialIoProtocolGuid,               "SerialIo       "},
 {&gEfiIp6ServiceBindingProtocolGuid,      "Ip6SvrBinding  "}, 
 {&gEfiIp6ProtocolGuid,                    "Ip6            "},
 {&gEfiSmmCpuProtocolGuid,                 "SmmCpu         "},
 {&gEfiSmmCpuServiceProtocolGuid,          "SmmCpuService  "},
 {&gEfiSmmSxDispatch2ProtocolGuid,         "SmmSxDispatch2 "},
 {&gEfiSmmSwDispatch2ProtocolGuid,         "SmmSwDispatch2 "},
 {&gEfiSmmPowerButtonDispatch2ProtocolGuid,   "SmmPBDispatch2 "},
 {&gEfiSmmPeriodicTimerDispatch2ProtocolGuid, "SmmPTDispatch2 "},
 {&gEfiSmmUsbDispatch2ProtocolGuid,           "SmmUsbDispatch2"},
 {&gEfiSmmFirmwareVolumeBlockProtocolGuid,    "SmmFVBlock     "},
 {&gEfiPnpSmbiosProtocolGuid,                 "PnpSmbios      "},
 {&gEfiSmmFaultTolerantWriteProtocolGuid,     "SmmFTW         "},
 {&gByoSmiFlashSmmProtocolGuid,               "ByoSmiFlashSmm "},
 {&gEfiSimpleTextOutProtocolGuid,             "SimpleTextOut"},
 {&gEfiConsoleOutDeviceGuid,                  "ConsoleOutDev"},
 {&gEfiSmmReadyToLockProtocolGuid,            "SmmReadyToLock"},
 {&gEfiPciRootBridgeIoProtocolGuid,           "PciRootBridgeIo"},
 {&gEfiComponentName2ProtocolGuid,            "ComponentName2"},
 {&gEfiComponentNameProtocolGuid,             "ComponentName"},
 {&gEfiDriverBindingProtocolGuid,             "DriverBinding"},
 {&gEfiAbsolutePointerProtocolGuid,           "AbsolutePointer"},
 {&gEfiDevicePathUtilitiesProtocolGuid,       "DPUtilities"},
 {&gEfiDevicePathToTextProtocolGuid,          "DPToText"},
 {&gEfiDevicePathFromTextProtocolGuid,        "DPFromText"},
 {&gEfiAcpiTableProtocolGuid,                 "AcpiTable"},
 {&gEfiAcpiSdtProtocolGuid,                   "AcpiSdt"},
 {&gEfiDxeSmmReadyToLockProtocolGuid,         "SmmReadyToLock"},
 {&gBootScriptExecutorDxeFileGuid,            "BootScriptExeFile"},
 {&gEfiEdidDiscoveredProtocolGuid,            "EdidDiscovered"},
};

  
  
  
  



#define GUID_NAME_LIST_COUNT         (sizeof(gGuidNameList)/sizeof(gGuidNameList[0]))

VOID
LibShowGuidName (
  IN CHAR8         *Str,
  IN EFI_GUID      *Guid,
  IN VOID          *Interface
  )
{
  UINTN  Index;
  
  for(Index=0;Index<GUID_NAME_LIST_COUNT;Index++){
    if(CompareGuid(Guid, gGuidNameList[Index].Guid)){
      DEBUG((DEBUG_INFO, "%a: %-36a %p\n", Str, gGuidNameList[Index].Name, Interface));
      return;
    }
  }
  
  DEBUG((DEBUG_INFO, "%a: %g %p\n", Str, Guid, Interface));
}


EFI_STATUS
EFIAPI
GuidNameLibConstructor (
    VOID
  )
{
  gFuncShowGuidName = LibShowGuidName;
  return EFI_SUCCESS;
}

