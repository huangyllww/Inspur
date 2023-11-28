/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#ifndef __BYO_COMMON_LIB_H__
#define __BYO_COMMON_LIB_H__

#include <Uefi.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PciIo.h>


typedef union {
  struct {
    UINT32  Lower32;
    UINT32  Upper32;
  } Uint32;
  UINT64    Uint64;
} MY_DATA_64;

typedef union {
  struct {
    UINT8  Data[4];
  } Uint8;
  UINT32  Uint32;
} DATA_32_4;

#define PCI_CLASS_MASS_STORAGE_NVM                0x08
#define PCI_IF_NVMHCI                             0x02


#define LEGACY_REGION_SEGMENT_E000                2
#define LEGACY_REGION_SEGMENT_F000                1
#define LEGACY_REGION_SEGMENT_E000_F000           0





UINT8 PcieRead8(UINTN PcieAddr);

UINT8 CaculateWeekDay(UINT16 y, UINT8 m, UINT8 d);

BOOLEAN IsLeapYear(UINT16 Year);

VOID DumpMem32(const VOID *Base, UINTN Size);

VOID DumpMem8(const VOID *Base, UINTN Size);

VOID DumpCmos();

VOID DumpPci(UINT8 Bus, UINT8 Dev, UINT8 Func);

VOID DumpIo(UINTN Base, UINT16 Size);

VOID DumpIo4(UINTN Base, UINT16 Size);

VOID DumpAllPciIntLinePin();

VOID DumpAllPci();

VOID DumpHob();

VOID 
ListPci (
    IN EFI_BOOT_SERVICES         *BS
  );

VOID 
WriteAsmedia (
    IN EFI_BOOT_SERVICES         *BS
  );


EFI_STATUS KbcWaitInputBufferFree();
EFI_STATUS KbcWaitOutputBufferFull();
EFI_STATUS KbcWaitOutputBufferFree();
EFI_STATUS KbcSendCmd(UINT8 Cmd);
BOOLEAN CheckKbcPresent(VOID);
VOID IsPs2DevicePresent(EFI_BOOT_SERVICES *BS, BOOLEAN *Ps2Kb, BOOLEAN *Ps2Ms);

EFI_STATUS
KbcCmdSendData (
  IN  UINT8  Cmd,
  OUT UINT8  Data
  );


EFI_STATUS
KbcCmdReadData (
  IN  UINT8  Cmd,
  OUT UINT8  *Data  OPTIONAL
  );


EFI_STATUS 
EFIAPI
LibCalcCrc32 (
  IN  VOID    *Data,
  IN  UINTN   DataSize,
  OUT UINT32  *CrcOut
  );

BOOLEAN LibVerifyDataCrc32(VOID *Data, UINTN DataSize, UINTN CrcOffset, UINT32 *CalcCrc32 OPTIONAL);
  
EFI_STATUS 
AzaliaLoadVerbTable (
  IN UINTN           HostPcieAddr,
  IN VOID            *VerbTable, 
  IN UINTN           VerbTableSize
  );
  
VOID *LibGetGOP(EFI_BOOT_SERVICES *pBS);

EFI_STATUS BltSaveAndRetore(VOID *BootServices, BOOLEAN Save);

VOID 
AcpiTableUpdateChksum (
  IN VOID *AcpiTable
  );

UINTN
MyGetDevicePathSize (
  IN CONST EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );
  
VOID
ShowDevicePathDxe (
  IN EFI_BOOT_SERVICES         *BS,
  IN EFI_DEVICE_PATH_PROTOCOL  *ptDevPath
  );

EFI_DEVICE_PATH_PROTOCOL *
GetPciParentDp (
    IN     EFI_BOOT_SERVICES         *BS,
    IN     EFI_DEVICE_PATH_PROTOCOL  *PciDp
  );

VOID
SwapWordArray (
  IN  UINT8   *Data,
  IN  UINTN   DataSize
  );

CHAR8 *TrimStr8(CHAR8 *Str);

CHAR16 *TrimStr16(CHAR16 *Str);
VOID SwapLanMacAddress(UINT8 *MacAddr);

EFI_STATUS
GetOnboardLanMacAddress (
  IN  EFI_BOOT_SERVICES         *pBS,
  IN  VOID                      *Dp,
  OUT UINT8                     MacAddr[6]
  );  
  
BOOLEAN IsSataDp(VOID *DevicePath);
BOOLEAN IsUsbDp(VOID *DevicePath);
UINT8 GetDeviceTypeFromDp(VOID *DevicePath);
UINT32 GetPartitionNumberFromDp(VOID *DevicePath);

BOOLEAN 
IsUefiAhciHddDp(
  IN  EFI_BOOT_SERVICES       *pBS,
  IN  VOID                    *DevicePath,
  OUT VOID                    **pDiskInfo  OPTIONAL
  );

VOID*
AllocateRunTimeZeroMemoryBelow4G (
  IN EFI_BOOT_SERVICES  *BS,
  IN UINTN              Size
  );

VOID*
AllocateAcpiNvsZeroMemoryBelow4G (
  IN EFI_BOOT_SERVICES  *BS,
  IN UINTN              Size
  );

VOID*
AllocateReservedZeroMemoryBelow4G (
  IN EFI_BOOT_SERVICES  *BS,
  IN UINTN              Size
  );

VOID*
AllocateReservedZeroMemoryBelow1M (
  IN EFI_BOOT_SERVICES  *BS,
  IN UINTN              Size
  );

VOID*
AllocateBsZeroMemoryBelow4G (
  IN EFI_BOOT_SERVICES  *BS,
  IN UINTN              Size
  );

EFI_STATUS InvokeHookProtocol(EFI_BOOT_SERVICES *BS, EFI_GUID *Protocol);  

typedef
VOID
(EFIAPI *EFI_MY_HOOK_PROTOCOL) (
  VOID
);

EFI_STATUS InvokeHookProtocol2(EFI_BOOT_SERVICES *BS, EFI_GUID *Protocol, VOID *Param);

typedef
VOID
(EFIAPI *EFI_MY_HOOK_PROTOCOL2) (
  VOID  *Param
);



VOID
SignalProtocolEvent (
  EFI_BOOT_SERVICES *BS,
  EFI_GUID          *ProtocolGuid,
  BOOLEAN           NeedUnInstall
  );  

EFI_HANDLE 
GetPciHandleFromBDF (
  EFI_BOOT_SERVICES         *BS,
  UINTN                     MyBus, 
  UINTN                     MyDev, 
  UINTN                     MyFunc
  );

BOOLEAN 
GetPciClassCodeFromBDF(
  EFI_BOOT_SERVICES         *BS,
  UINTN                     MyBus, 
  UINTN                     MyDev, 
  UINTN                     MyFunc,
  UINT8                     ClassCode[3]
  );

EFI_STATUS 
GetObSataHostDp(
  EFI_BOOT_SERVICES        *BS, 
  UINTN                    HostIndex, 
  EFI_DEVICE_PATH_PROTOCOL **HostDp
  );
UINT8 GetPciSlotNum(
  IN  EFI_BOOT_SERVICES *BS,
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  );
VOID *GetAcpiTableBySign(UINT32 Sign, EFI_SYSTEM_TABLE *SystemTable);

UINT8 Cmos70Read(UINT8 Address);
VOID Cmos70Write(UINT8 Address, UINT8 Data);
VOID WriteCheckedCmosByte(UINT8 Offset, UINT8 Data8);
BOOLEAN ReadCheckedCmosByte(UINT8 Offset, UINT8 *Data8);

UINT8 GetBbsGroupType(UINT8 PciClass, UINT8 BbsType);

VOID SetBootLogoInvalid(EFI_BOOT_SERVICES *BS);


#if !defined(MDEPKG_NDEBUG)
#define DUMPMEM8(x)  DumpMem8 x
#else
#define DUMPMEM8(x)
#endif



BOOLEAN 
IsFilePresent (
  EFI_BOOT_SERVICES  *BS,
  VOID               *pRootFile,
  CHAR16             *FilePathName
  );

BOOLEAN 
IsFilePresentSize (
  EFI_BOOT_SERVICES  *BS,
  VOID               *pRootFile,
  CHAR16             *FilePathName,
  UINTN              *FileSize          OPTIONAL
  );

EFI_STATUS 
GetFsLabel (
  EFI_BOOT_SERVICES  *BS,
  VOID               *pRootFile,
  CHAR16             **pLabel
  );


EFI_STATUS
GetPciNicMac (
  EFI_PCI_IO_PROTOCOL    *PciIo,
  UINT8    *MacAddr
  );


BOOLEAN 
IsFsFolderPresent (
  EFI_BOOT_SERVICES  *BS,
  VOID               *pRootFile,
  CHAR16             *FolderName
  );

EFI_STATUS 
RootFileReadFile(   
  IN   EFI_BOOT_SERVICES     *BS,
  IN   VOID                  *RootFile,
  IN   CHAR16                *FileName,
  OUT  VOID                  **FileData,
  OUT  UINTN                 *FileSize
  );

EFI_STATUS 
RootFileWriteFile(
  IN   VOID                  *RootFile,
  IN   CHAR16                *FileName,
  IN   VOID                  *FileData,
  IN   UINTN                 FileSize
  );

EFI_STATUS 
RootFileRmFile (
  IN   VOID                  *RootFile,
  IN   CHAR16                *FileName
  );

EFI_STATUS 
LibGetFileInfo (
  IN  EFI_BOOT_SERVICES     *BS,
  IN  VOID                  *ptFile,
  OUT VOID                  **Info
  );

CHAR8 *
LibSmbiosGetStringInTypeByIndex (
  VOID       *SmbiosHdr,
  UINT8      StrId
  );

CHAR8 *
LibSmbiosGetTypeEnd (
  VOID       *SmbiosHdr
  );

EFI_STATUS
LibReadFileFromFv(
  IN  EFI_BOOT_SERVICES *BS,
  IN  CONST EFI_GUID    *NameGuid,
  IN  UINT8             SectionType,
  IN  UINTN             SectionInstance,
  OUT VOID              **FileData,
  OUT UINTN             *FileDataSize
  );

BOOLEAN
IsFilePresentInFv (
  IN  EFI_BOOT_SERVICES *BS,
  IN  CONST EFI_GUID    *FileGuid
  );

BOOLEAN IsAllDataFF(VOID *DataIn, UINTN DataSize);

BOOLEAN LibIsHexChar(CHAR8 c);
BOOLEAN LibChar2Hex2(CHAR8 *s, UINT16 *d);
BOOLEAN LibChar2Hex4(CHAR8 *s, UINT32 *d);
BOOLEAN LibChar2Hex(CHAR8 *s, UINT8 *d);
BOOLEAN LibChar2Dec(CHAR8 *s, UINT8 *d);
BOOLEAN LibChar2Guid(CHAR8 *s, EFI_GUID *Guid);
BOOLEAN LibChar2Dec2(CHAR8 *s, UINT16 *d);
BOOLEAN LibChar2Dec4(CHAR8 *s, UINT32 *d);

CHAR16
LibUnicodeToUpper (
  IN      CHAR16                    Chr
  );

INTN
LibStrinCmp (
  IN CHAR16   *String,
  IN CHAR16   *String2,
  IN UINTN    Length
  );

INTN
LibStriCmp (
  IN CHAR16   *String,
  IN CHAR16   *String2
  );

VOID *GetByoSysAndStsHobData(VOID);

CHAR16 * 
LibOpalDriverGetDriverDeviceName (
    EFI_BOOT_SERVICES   *BS,
    EFI_HANDLE          DiskHandle
  );

BOOLEAN
LibContainEfiImage (
  IN VOID            *RomImage,
  IN UINT64          RomSize
  );

BOOLEAN
LibContainLegacyImage (
  IN VOID            *RomImage,
  IN UINT64          RomSize
  );

BOOLEAN
LibGetSataPortInfo (
  IN  EFI_BOOT_SERVICES         *BS,
  IN  EFI_HANDLE                Handle,
  OUT UINTN                     *HostIndex,
  OUT UINTN                     *PortIndex,
  OUT UINTN                     *RealPortIndex OPTIONAL
  );


typedef struct {
  UINT8             Pdt;            ///< Peripheral Device Type (low 5 bits)
  UINT8             Removable;      ///< Removable Media (highest bit)
  UINT8             Reserved0[2];
  UINT8             AddLen;         ///< Additional length
  UINT8             Reserved1[3];
  UINT8             VendorID[8];
  UINT8             ProductID[16];
  UINT8             ProductRevision[4];
} USB_BOOT_INQUIRY_DATA;



#endif

