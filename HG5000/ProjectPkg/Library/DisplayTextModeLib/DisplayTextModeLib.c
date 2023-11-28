#include <Library/HiiLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
//#include <Library/PlatformBootManagerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <Library/IpmiBaseLib.h>
#include <protocol/DiskInfo.h>
#include <IndustryStandard/Atapi.h>
#include <Library/PlatformCommLib.h>
#include <Library/DisplayTextModeLib.h>
#include <Library/PlatformLanguageLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/IpmiInterfaceProtocol.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Library/ByoCommLib.h>
#include <Protocol/UsbIo.h>
#include <SetupVariable.h>
#include <Protocol/ByoDiskInfoProtocol.h>
#include <BmcConfig.h>


#define ME_SPS_INFO_MAX_STR_SIZE    0x200
#define EFI_SM_NETFN_APP  		    0x06
#define EFI_SM_NETFN_LAN            0x0C
#define GET_LAN_CONFIG_CMD          0x02
#define GET_DEVICE_ID_CMD           0x01
#define CLASS_HID                   3
#define SUBCLASS_BOOT               1
#define PROTOCOL_KEYBOARD           1
#define PROTOCOL_MOUSE              2
#define MANUFACTURER_STRING_LENGTH  0x30

#define _48_BIT_ADDRESS_FEATURE_SET_SUPPORTED   0x0400
#define ATAPI_DEVICE                            0x8000
#define CHARACTER_NUMBER_FOR_VALUE  30

EFI_HII_HANDLE    mHiiHandle = NULL;

EFI_GUID mTextModeStringPackGuid = { 0xDCEFF379, 0xD6E5, 0x41C9, {0xBD, 0x83, 0xB1, 0xC4, 0x64, 0x51, 0xC1, 0x77} };

CONST SETUP_DATA    *gSetupHob;

static CHAR16 mHexStr[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' };

STATIC CHAR16 *gMemoryManufacturer[] = {
    L"Samsung",
    L"SK Hynix",
    L"Micron Technology",
    L"UniIC",
    L"AgigA Tech",
    L"三星",
    L"海力士",
    L"镁光",
    L"紫光",
    L"AgigA Tech"
};

STATIC UINTN gMemMafcPrintTimes[] = {
       0,   // Samsung
       0,   // SK Hynix
       0,   // Micron Technology
       0,   // UniIC
       0   // AgigA Tech
};

typedef struct {
  CHAR16            *FilePathString;
  CHAR16            *Description;
}BDS_GENERAL_UEFI_BOOT_OS;

BDS_GENERAL_UEFI_BOOT_OS mBdsGeneralUefiBootOs1[] = {
  L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi",      L"Windows Boot Manager",
  L"\\EFI\\centos\\shimx64-centos.efi",         L"CentOS Linux",
  //L"\\EFI\\centos\\shim.efi",                   L"CentOS Linux",  
  L"\\EFI\\uos\\shimx64.efi",                   L"UOS", 
  L"\\EFI\\ubuntu\\grubx64.efi",                L"Ubuntu Linux",
  L"\\EFI\\redhat\\shimx64-redhat.efi",         L"Red Hat Linux",
  L"\\EFI\\tencent\\grubx64.efi",               L"Tencent OS tlinux",
  L"\\EFI\\tencent\\grub.efi",                  L"Tencent OS tlinux",
  //L"\\EFI\\redhat\\shim.efi",                   L"Red Hat Linux",
  L"\\EFI\\redhat\\shimx64.efi",                L"Red Hat Enterprise Linux",
  L"\\EFI\\neokylin\\shimx64.efi",              L"Kylin Linux Advanced Server",
  //L"\\EFI\\kylin\\shimx64.efi",                 L"Kylin Linux Advanced Server",
//  EFI_REMOVABLE_MEDIA_FILE_NAME,                NULL,   
//  L"\\EFI\\ubuntu\\grub.efi",                 L"Ubuntu",
//  L"\\EFI\\SuSE\\elilo.efi",                  L"SuSE Linux",
//  L"\\EFI\\android\\grub.efi",                L"Android",
  };

BDS_GENERAL_UEFI_BOOT_OS mBdsGeneralUefiBootOs2[] = {
  L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi",      L"Windows Boot Manager",
  L"\\EFI\\centos\\shimx64-centos.efi",         L"CentOS Linux",
  L"\\EFI\\centos\\shim.efi",                   L"CentOS Linux",
  L"\\EFI\\redhat\\grub.efi",                   L"CentOS Linux", //CentOS 6.5
  L"\\EFI\\tencent\\grubx64.efi",               L"Tencent OS tlinux",
  L"\\EFI\\tencent\\grub.efi",                  L"Tencent OS tlinux",
  L"\\EFI\\uos\\shimx64.efi",                   L"UOS", 
  L"\\EFI\\ubuntu\\grubx64.efi",                L"Ubuntu Linux",
  L"\\EFI\\redhat\\shimx64-redhat.efi",         L"Red Hat Linux",
  L"\\EFI\\redhat\\shim.efi",                   L"Red Hat Linux",
  L"\\EFI\\redhat\\shimx64.efi",                L"Red Hat Enterprise Linux",
  L"\\EFI\\neokylin\\shimx64.efi",              L"Kylin Linux Advanced Server",
  L"\\EFI\\kylin\\shimx64.efi",                 L"Kylin Linux Advanced Server",
//  EFI_REMOVABLE_MEDIA_FILE_NAME,                NULL,   
//  L"\\EFI\\ubuntu\\grub.efi",                 L"Ubuntu",
//  L"\\EFI\\SuSE\\elilo.efi",                  L"SuSE Linux",
//  L"\\EFI\\android\\grub.efi",                L"Android",
  };


/**
  Eliminate the extra spaces in the Str to one space.
**/
/*VOID
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
}*/


UINTN
EfiValueToHexStr (
  IN  OUT CHAR16  *Buffer, 
  IN  UINT64      Value, 
  IN  UINTN       Flags, 
  IN  UINTN       Width
  )
/*++

Routine Description:

  VSPrint worker function that prints a Value as a hex number in Buffer

Arguments:

  Buffer - Location to place ascii hex string of Value.

  Value  - Hex value to convert to a string in Buffer.

  Flags  - Flags to use in printing Hex string, see file header for details.

  Width  - Width of hex value.

Returns: 

  Number of characters printed.  

--*/
{
  CHAR16  TempBuffer[CHARACTER_NUMBER_FOR_VALUE];
  CHAR16  *TempStr;
  CHAR16  Prefix;
  CHAR16  *BufferPtr;
  UINTN   Count;
  UINTN   Index;

  TempStr   = TempBuffer;
  BufferPtr = Buffer;

  //
  // Count starts at one since we will null terminate. Each iteration of the
  // loop picks off one nibble. Oh yea TempStr ends up backwards
  //
  Count = 0;
  
  if (Width > CHARACTER_NUMBER_FOR_VALUE - 1) {
    Width = CHARACTER_NUMBER_FOR_VALUE - 1;
  }

  do {
    Index = ((UINTN)Value & 0xf);
    *(TempStr++) = mHexStr[Index];
    Value = RShiftU64 (Value, 4);
    Count++;
  } while (Value != 0);

  if (Flags & PREFIX_ZERO) {
    Prefix = '0';
  } else { 
    Prefix = ' ';
  }

  Index = Count;
  if (!(Flags & LEFT_JUSTIFY)) {
    for (; Index < Width; Index++) {
      *(TempStr++) = Prefix;
    }
  }

  //
  // Reverse temp string into Buffer.
  //
  if (Width > 0 && (UINTN) (TempStr - TempBuffer) > Width) {
    TempStr = TempBuffer + Width;
  }
  Index = 0;
  while (TempStr != TempBuffer) {
    *(BufferPtr++) = *(--TempStr);
    Index++;
  }
    
  *BufferPtr = 0;
  return Index;
}



EFI_STATUS
Ipv6AddtoStr (
  IN OUT CHAR16        *Buffer,
  IN     UINT8         *Ipadd
  )
/*++

Routine Description:

  Converts IPV6 Address Buffer to String

Arguments:

  Buffer  - GC_TODO: add argument description
  Ipadd   - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  UINTN Length;
  UINTN Ptr;

  Ptr     = 0;

 //
//In flag value is 0x20 to append charecter zero and 2 for specifing 2 digit value.
//
  Length  = EfiValueToHexStr (&Buffer[0], Ipadd[0], 0x20, 2);
  Ptr     = Ptr + Length;
 

 
  Length  = EfiValueToHexStr (&Buffer[Ptr], Ipadd[1], 0x20, 2);
  Ptr     = Ptr + Length;
  StrCpy (&Buffer[Ptr++], L":");

  
   Length  = EfiValueToHexStr (&Buffer[Ptr], Ipadd[2], 0x20, 2);
  Ptr = Ptr + Length;
  
  
  
  Length  = EfiValueToHexStr (&Buffer[Ptr], Ipadd[3], 0x20, 2);
  Ptr = Ptr + Length;
  StrCpy (&Buffer[Ptr++], L":");

 
 Length  =  EfiValueToHexStr (&Buffer[Ptr], Ipadd[4], 0x20, 2);
  Ptr = Ptr + Length;
 

 
  Length  =  EfiValueToHexStr (&Buffer[Ptr], Ipadd[5], 0x20, 2);
  Ptr = Ptr + Length;
  StrCpy (&Buffer[Ptr++], L":");
  
 
  Length  =  EfiValueToHexStr (&Buffer[Ptr], Ipadd[6], 0x20, 2);
  Ptr = Ptr + Length;
  
  
 
  Length  =   EfiValueToHexStr (&Buffer[Ptr], Ipadd[7], 0x20, 2);
  Ptr = Ptr + Length;
  StrCpy (&Buffer[Ptr++], L":");
  
  Length  = EfiValueToHexStr (&Buffer[Ptr], Ipadd[8], 0x20, 2);
  Ptr     = Ptr + Length;
 

 
  Length  = EfiValueToHexStr (&Buffer[Ptr], Ipadd[9], 0x20, 2);
  Ptr     = Ptr + Length;
  StrCpy (&Buffer[Ptr++], L":");

  
   Length  = EfiValueToHexStr (&Buffer[Ptr], Ipadd[10], 0x20, 2);
  Ptr = Ptr + Length;
  
  
  
  Length  = EfiValueToHexStr (&Buffer[Ptr], Ipadd[11], 0x20, 2);
  Ptr = Ptr + Length;
  StrCpy (&Buffer[Ptr++], L":");

 
 Length  =  EfiValueToHexStr (&Buffer[Ptr], Ipadd[12], 0x20, 2);
  Ptr = Ptr + Length;
 

 
  Length  =  EfiValueToHexStr (&Buffer[Ptr], Ipadd[13], 0x20, 2);
  Ptr = Ptr + Length;
  StrCpy (&Buffer[Ptr++], L":");
  
 
  Length  =  EfiValueToHexStr (&Buffer[Ptr], Ipadd[14], 0x20, 2);
  Ptr = Ptr + Length;
  

 
  Length  =   EfiValueToHexStr (&Buffer[Ptr], Ipadd[15], 0x20, 2);

  return EFI_SUCCESS;
}

EFI_STATUS
GetOptionalStringByIndex (
  IN      CHAR8                   *OptionalStrStart,
  IN      UINT8                   Index,
  OUT     CHAR16                  **String
  )
{
  UINTN          StrSize;

  if (Index == 0) {
    *String = AllocateZeroPool (sizeof (CHAR16));
    return EFI_SUCCESS;
  }

  StrSize = 0;
  do {
    Index--;
    OptionalStrStart += StrSize;
    StrSize           = AsciiStrSize (OptionalStrStart);
  } while (OptionalStrStart[StrSize] != 0 && Index != 0);

  if ((Index != 0) || (StrSize == 1)) {
    //
    // Meet the end of strings set but Index is non-zero, or
    // Find an empty string
    //
    *String = L"Missing String";
  } else {
    *String = AllocateZeroPool (StrSize * sizeof (CHAR16));
    AsciiStrToUnicodeStr (OptionalStrStart, *String);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
DisplayScreenText (
  IN  UINT32      ScLine,
  IN  CHAR8       *Format,
  ...
  )
{
  EFI_STATUS                       Status;
  CHAR16                           Buffer[0x100];
  VA_LIST                          Marker;
  UINTN                            LeftColumn;
  UINTN                            TopRow;
  static UINT32                    Row = 0;
  //UINTN                            PrintHeight;
  UINT16                           PrintX, PrintY;

  //
  // Convert the DEBUG() message to a Unicode String
  //
  SetMem (&Buffer[0], sizeof (Buffer), 0x0);
  VA_START (Marker, Format);
  UnicodeVSPrintAsciiFormat (Buffer, sizeof(Buffer),  Format, Marker);
  VA_END (Marker);

  //
  // Print on left and top of screen.
  //
  LeftColumn = 0;
  TopRow = 0;

  Status= gST->ConOut->QueryMode(gST->ConOut, gST->ConOut->Mode->Mode, &LeftColumn, &TopRow);

  if (EFI_ERROR(Status)) {
    Row = 0;
    return Status;
  }

  if (LeftColumn == 0) {
    LeftColumn = 24;
  }
  if (TopRow == 0) {
    TopRow = 3;
  }
  /*PrintHeight = 13;
  if (Row >= PrintHeight) {
    Row = 0;
    gBS->Stall(300 * 1000);
  }*/

  if (ScLine == 0) {
    Row = 1;
  } else if (ScLine == 0xFF) {
    Row++;
  } else {
    Row = ScLine;
  }

  if(Row>14&&Row<22){
    Row=22;
  }
  
  PrintX = 2;
  PrintY = 1;  

  Status = gST->ConOut->SetCursorPosition(gST->ConOut, PrintX, (PrintY + Row));
  DEBUG((EFI_D_INFO, "[Stephen] SetCursorPosition-%r PrintX-%d PrintY-%d\n", Status, PrintX, (PrintY + Row)));

  Status = gST->ConOut->EnableCursor(gST->ConOut, FALSE);

  Status = gST->ConOut->OutputString(gST->ConOut, Buffer);  //Output to all display devices.

  DEBUG((EFI_D_ERROR, __FUNCTION__"(), ScLine %d -%s\n", Row, Buffer)); 
  return EFI_SUCCESS;
}//tangyan+


VOID
CleanScreenText (
  VOID
  )
{
  EFI_STATUS                       Status;
  UINTN                            LeftColumn;
  UINTN                            TopRow;
  UINTN                            Index, IndexRow;
  UINTN                            PrintHeight;
  UINTN                            PrintX, PrintY;
  CHAR16                           Space[0x100];

  DEBUG((EFI_D_ERROR, __FUNCTION__"() \n"));

  LeftColumn = 0;
  TopRow = 0;
  Status= gST->ConOut->QueryMode (
                 gST->ConOut,
                 gST->ConOut->Mode->Mode,
                 &LeftColumn,
                 &TopRow
                 );
  if (EFI_ERROR(Status)) {
    return;
  }

  PrintHeight = 16;
  PrintX = 0;
  PrintY = 0;
  

  //
  //Clean all background.
  //
  for(Index = 0; Index < LeftColumn -PrintX; Index++ ) {
    Space[Index] = L' ';
  }
  Space[Index] = CHAR_NULL;
  

  gST->ConOut->EnableCursor(gST->ConOut, FALSE);
  gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_WHITE);
  for(IndexRow = 0; IndexRow < PrintHeight; IndexRow++ ) {
    gST->ConOut->SetCursorPosition(gST->ConOut, PrintX, (PrintY + IndexRow));
    gST->ConOut->OutputString(gST->ConOut, Space);
  }

  for(IndexRow = 22; (21 < IndexRow) && (IndexRow < 27); IndexRow++ ) {
    gST->ConOut->SetCursorPosition(gST->ConOut, PrintX, (PrintY + IndexRow));
    gST->ConOut->OutputString(gST->ConOut, Space);
  }

  return;

}


VOID
CleanScreenTextEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{

  CleanScreenText ();
  gBS->CloseEvent (Event);

  return;
}

  
UINT16
GetCacheSize (
  EFI_SMBIOS_PROTOCOL       *Smbios,
  EFI_SMBIOS_HANDLE         CacheHandle
  )
{
  EFI_SMBIOS_HANDLE         SmbiosHandle;
  EFI_SMBIOS_TYPE           SmbiosType;
  EFI_SMBIOS_TABLE_HEADER   *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER  p;
  EFI_STATUS                Status;
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
    if (!EFI_ERROR(Status)) {
        CacheSize = p.Type7->InstalledSize2;
        if(CacheSize & BIT31){
            CacheSize = CacheSize * 64;
        }
    }

    if(CacheSize >= 1024 && (CacheSize % 1024) == 0){
    CacheSize = CacheSize / 1024;                     //MB
  } 

    return CacheSize;
}//tangyan+

VOID
GetMemoryInfo (
  EFI_SMBIOS_PROTOCOL       *Smbios,
  UINTN                      CPUCount
  )
{
  SMBIOS_TABLE_TYPE17       *Type17Record;
  EFI_SMBIOS_HANDLE         SmbiosHandle;
  EFI_SMBIOS_TYPE           SmbiosType;
  EFI_SMBIOS_TABLE_HEADER   *SmbiosHdr;
  EFI_STATUS                Status;
  UINT32                    SingleSize;
  UINTN                     TotalMemSize = 0;
  UINTN                     Index;
  UINT16                    OemMemSpeed;
  CHAR16                    *OemMemManustring;
  CHAR16                    *OemMemTypestring;
  CHAR16                    *OemMemUniManustring;
  UINT8                     OemMemType;
  UINT8                     MemoryCount=0;

  
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
  while (1) {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
    Type17Record = (SMBIOS_TABLE_TYPE17 *) SmbiosHdr;	

    //
    // Calculate Total Size.
    //
    SingleSize = Type17Record->Size;
    if(SingleSize == 0 || SingleSize == 0xFFFF){
      SingleSize = 0;
    } else if(SingleSize == 0x7FFF){
      SingleSize = Type17Record->ExtendedSize & (~BIT31);   // MB
    } else {
      if(SingleSize & BIT15){ // unit in KB to MB.
  	SingleSize = (SingleSize&(~BIT15)) >> 10;      
      }
    }
    if (SingleSize == 0) {
      continue;
    }
    TotalMemSize += SingleSize;
    if(SingleSize!=0){
      MemoryCount++;
	}
    //
    // Show Different Manufacturer Info.
    //
    OemMemSpeed = Type17Record->ConfiguredMemoryClockSpeed;
    OemMemManustring = AllocateZeroPool(2*MANUFACTURER_STRING_LENGTH);
    ASSERT(OemMemManustring !=NULL);
    GetOptionalStringByIndex ((CHAR8*)((UINT8*)Type17Record + Type17Record->Hdr.Length), Type17Record->Manufacturer, &OemMemManustring);
    DEBUG((EFI_D_ERROR, __FUNCTION__"(), OemMemManustring :%s, OemMemSpeed :%d.\n", OemMemManustring, OemMemSpeed));
    
    if (StrStr(OemMemManustring,L"UniIC")!=NULL) {
      StrCpy(OemMemManustring,L"UniIC");
    } 

    OemMemType = Type17Record->MemoryType;
    switch (OemMemType) {
      case 0x03:
          OemMemTypestring = L"DRAM";
          break;
      case 0x11:
          OemMemTypestring = L"RDRAM";
          break;
      case 0x12:
          OemMemTypestring = L"DDR";
          break;
      case 0x13:
           OemMemTypestring = L"DDR2";
          break;
      case 0x18:
          OemMemTypestring = L"DDR3";
          break;
      case 0x1A:
          OemMemTypestring = L"DDR4";
          break;
    }       

    OemMemUniManustring = AllocatePool(2*MANUFACTURER_STRING_LENGTH+1);
    ASSERT(OemMemUniManustring !=NULL);
     for (Index = 0; Index < 5; Index++) {
       if(StrStr(OemMemManustring,gMemoryManufacturer[Index]) !=NULL) {
         StrCpy(OemMemUniManustring, gMemoryManufacturer[Index + 5]);
         if (gMemMafcPrintTimes[Index] == 0){
          if(BeEnglish()){
            DisplayScreenText (0xFF, "%s: %s, %s: %s, %s: %dMHz", L"Memory Manufacturer", OemMemManustring, L"Type", OemMemTypestring, L"Frequency", OemMemSpeed);
          }else{
            DisplayScreenText (0xFF, "%s: %s, %s: %s, %s: %dMHz ", L"内存厂商", OemMemUniManustring, L"类型", OemMemTypestring, L"频率", OemMemSpeed);
          }
           }
         gMemMafcPrintTimes[Index]++;  
       }
    } 
    
   if (OemMemManustring != NULL) {
     FreePool(OemMemManustring);
    }

   if (OemMemUniManustring != NULL) {
     FreePool(OemMemUniManustring);
    }
  }

  
  if(BeEnglish()){
    DisplayScreenText (0xFF, "%s: %dGB, %s: %d", L"Memory Total Size", (TotalMemSize/1024), L"Memory Total Count",MemoryCount);
  }else{
    DisplayScreenText (0xFF, "%s: %dGB, %s: %d", L"内存总大小", (TotalMemSize/1024), L"内存总数量",MemoryCount);
  }

  return;
}
 
VOID
GetBMCInfo (
    VOID
  )
{
    EFI_STATUS          Status;
    CHAR16              BmcFwVersion[20];
    CHAR16              BMCIPstring[46];
    CHAR16              BMCShareIPstring[46];
    UINT8               CmdDataBuff[4];
    UINT8               CmdDataSize;
    UINT8               ResponseBuff[16];
    UINT8               ResponseSize;
    UINT8               ResponseBuff2[16];
    UINT8               ResponseSize2;
    //UINT8               ResponseBuff3[22];
    //UINT8               ResponseSize3;
    //UINT8               ResponseBuff4[22];
   // UINT8               ResponseSize4;
   // UINT16              Ipv6Address[46];
    UINT16                                FwVersion;
    UINTN                       VarSize;
    BMC_SETUP_DATA              BmcSetupData = {0};
	
    IPMI_INTERFACE_PROTOCOL               *IpmiInterface = NULL;
	EFI_SMBIOS_HANDLE           SmbiosHandle;
    EFI_SMBIOS_PROTOCOL         *SmbiosProtocol;
    UINT16                      TypeNo;
    EFI_SMBIOS_TABLE_HEADER     *Record;
    UINTN                       StringNo;
    CHAR8                       *BmcVer = NULL;
    EFI_GUID                    SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;

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

    EfiInitializeIpmiBase ();
        SetMem (BMCIPstring, sizeof (BMCIPstring), 0);
        CmdDataBuff[0] = 1; //LanChannelNo
        CmdDataBuff[1] = 3; //LanParam(IP Address)
        CmdDataBuff[2] = 0;
        CmdDataBuff[3] = 0;
        CmdDataSize = 4;
        ResponseSize = sizeof (ResponseBuff);
        SetMem (ResponseBuff, ResponseSize, 0);
        Status = EfiSendCommandToBMC (
					  EFI_SM_NETFN_LAN,
					  GET_LAN_CONFIG_CMD,
					  (UINT8 *)&CmdDataBuff,
					  CmdDataSize,
					  (UINT8 *)&ResponseBuff,
					  (UINT8 *)&ResponseSize
					  );
        UnicodeSPrint(BMCIPstring, 2 * 16, L"%d.%d.%d.%d", ResponseBuff[1], ResponseBuff[2], ResponseBuff[3], ResponseBuff[4]);

     SetMem (BMCShareIPstring, sizeof (BMCShareIPstring), 0);
     CmdDataBuff[0] = 8; //LanChannelNo
     CmdDataBuff[1] = 3; //LanParam(IP Address)
     CmdDataBuff[2] = 0;
     CmdDataBuff[3] = 0;
     CmdDataSize = 4;
     ResponseSize2 = sizeof (ResponseBuff2);
     SetMem (ResponseBuff2, ResponseSize2, 0);
     Status = EfiSendCommandToBMC (
                   EFI_SM_NETFN_LAN,
                   GET_LAN_CONFIG_CMD,
                   (UINT8 *)&CmdDataBuff,
                   CmdDataSize,
                   (UINT8 *)&ResponseBuff2,
                   (UINT8 *)&ResponseSize2
                   );
     UnicodeSPrint(BMCShareIPstring, 2 * 16, L"%d.%d.%d.%d", ResponseBuff2[1], ResponseBuff2[2], ResponseBuff2[3], ResponseBuff2[4]);

     //IPV6 STATIC    
/*		if(0==ResponseBuff[1] && 0==ResponseBuff[2] && 0==ResponseBuff[3] && 0==ResponseBuff[4]){
      SetMem (CmdDataBuff, sizeof (CmdDataBuff), 0);
      CmdDataBuff[0] = 1;    //LanChannelNo
      CmdDataBuff[1] = 0x38; //LanParam(IP Address)
      CmdDataBuff[2] = 0;
      CmdDataBuff[3] = 0;
      CmdDataSize = 4;
      ResponseSize3 = sizeof (ResponseBuff3);
      SetMem (ResponseBuff3, ResponseSize3, 0);
      Status = EfiSendCommandToBMC (
					  EFI_SM_NETFN_LAN,
					  GET_LAN_CONFIG_CMD,
					  (UINT8 *)&CmdDataBuff,
					  CmdDataSize,
					  (UINT8 *)&ResponseBuff3,
					  (UINT8 *)&ResponseSize3
					  );
      Ipv6AddtoStr (&Ipv6Address[0], &ResponseBuff3[3]);
      
      // ipv6 dynamic address
      if(ResponseBuff3[2] != 0x80){
        SetMem (CmdDataBuff, sizeof (CmdDataBuff), 0);
        CmdDataBuff[0] = 3;    //LanChannelNo
        CmdDataBuff[1] = 0x3b; //LanParam(IP Address)
        CmdDataBuff[2] = 0;
        CmdDataBuff[3] = 0;
        CmdDataSize = 4;
        ResponseSize4 = sizeof (ResponseBuff4);
        SetMem (ResponseBuff4, ResponseSize4, 0);
        Status = EfiSendCommandToBMC (
					  EFI_SM_NETFN_LAN,
					  GET_LAN_CONFIG_CMD,
					  (UINT8 *)&CmdDataBuff,
					  CmdDataSize,
					  (UINT8 *)&ResponseBuff4,
					  (UINT8 *)&ResponseSize4
					  );
        SetMem(Ipv6Address,sizeof(Ipv6Address),0);
        Ipv6AddtoStr (&Ipv6Address[0], &ResponseBuff4[3]);
      }
      DEBUG((EFI_D_INFO,"ipv6 address: %s\n",Ipv6Address));
      SetMem(BMCIPstring,sizeof(BMCIPstring),0);
      UnicodeSPrint(BMCIPstring, 2 * 46, L"%s",Ipv6Address);

    }*/
		
		ResponseSize2 = sizeof (ResponseBuff2);
		SetMem (ResponseBuff2, ResponseSize2, 0);
		SetMem (BmcFwVersion, sizeof (BmcFwVersion), 0);
		Status = EfiSendCommandToBMC (
					  EFI_SM_NETFN_APP,
					  GET_DEVICE_ID_CMD,
					  (UINT8 *)NULL,
					  0,
					  (UINT8 *)&ResponseBuff2,
					  (UINT8 *)&ResponseSize2
					  );
	    UnicodeSPrint(BmcFwVersion, 2 * 8, L"%02x.%02x", ResponseBuff2[2], ResponseBuff2[3]);
        DEBUG((EFI_D_INFO,"BMC Version: %s\n",BmcFwVersion));
        
		
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
  #if BC_TENCENT
  UnicodeSPrint (
    BmcFwVersion,
    sizeof (BmcFwVersion),
    L"%d.%02x.%02d",
    FwVersion >> 12, 
    (FwVersion >> 4) & 0xff, 
    FwVersion & 0xf
    );
  #else
  UnicodeSPrint (
    BmcFwVersion,
    sizeof (BmcFwVersion),
    L"%d.%x.%d",
    FwVersion >> 12, 
    (FwVersion >> 4) & 0xff, 
    FwVersion & 0xf
    );
  #endif
  DEBUG((EFI_D_INFO,"Get bmc version %s\n",BmcFwVersion));

  
  //
  //  Update smbios type 11 bmc version
  //
  BmcVer = AllocatePool (20);
  AsciiSPrint(BmcVer, 20, "BMC version:%s", BmcFwVersion);
  
  StringNo = 4;
  SmbiosProtocol->UpdateString (
                      SmbiosProtocol,
                      &SmbiosHandle,
                      &StringNo,
                      BmcVer
                      );
  
  VarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
                  BMC_SETUP_VARIABLE_NAME,
                  &SystemConfigurationGuid,
                  NULL,
                  &VarSize,
                  &BmcSetupData
                  );
  if(BeEnglish()){
    DisplayScreenText (0xFF, "%s: %s", L"BMC Version", BmcFwVersion);
    if(BmcSetupData.NCSI){
      DisplayScreenText (0xFF, "%s: %s, %s: %s", L"Dedicated IP Address", BMCIPstring,L"Shared IP Address",BMCShareIPstring);
	}else{
      DisplayScreenText (0xFF, "%s: %s", L"Dedicated IP Address", BMCIPstring);
	}
  }else{
    DisplayScreenText (0xFF, "%s: %s", L"BMC版本", BmcFwVersion);
    if(BmcSetupData.NCSI){
      DisplayScreenText (0xFF, "%s: %s, %s: %s", L"独立口IP地址", BMCIPstring,L"共享口IP地址",BMCShareIPstring);
	}else{
      DisplayScreenText (0xFF, "%s: %s", L"独立口IP地址", BMCIPstring);
	}
  }

}


//
  UINT8 GetAtaMajorVersion(UINT16 Major)
  {
	  UINT8   Ver = 0;
	UINT8	i;
  
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
//

EFI_STATUS
GetImageHeader (
  IN  EFI_HANDLE                  Device,
  IN  CHAR16                      *FileName,
  OUT EFI_IMAGE_DOS_HEADER        *DosHeader,
  OUT EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr
  );

BOOLEAN
ISOsDescription (
  IN EFI_HANDLE                Handle,  
  OUT	 CHAR16                *OsDescription
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
      LoaderCount = sizeof(mBdsGeneralUefiBootOs2)/sizeof(mBdsGeneralUefiBootOs2[0]);
      Status = EFI_NOT_FOUND;
      for(LoaderIndex=0;LoaderIndex<LoaderCount;LoaderIndex++){
        Status = GetImageHeader (
                   SimpleFileSystemHandles[Index],
                   mBdsGeneralUefiBootOs2[LoaderIndex].FilePathString,
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
          if(mBdsGeneralUefiBootOs2[LoaderIndex].Description !=NULL){		  	
				StrCpy(OsDescription, mBdsGeneralUefiBootOs2[LoaderIndex].Description);				
				DEBUG((EFI_D_INFO, "%a() OsDescription %s\n", __FUNCTION__,OsDescription));
           		return TRUE;
          }
          break;
      }
    }
  }

  if (SimpleFileSystemHandles != NULL) {
    FreePool(SimpleFileSystemHandles);
  }
  return FALSE;
}


VOID
GetDiskInfo (
    VOID
  )
{
    EFI_STATUS          		   Status;

	EFI_DISK_INFO_PROTOCOL        *DiskInfo1;
	ATA_IDENTIFY_DATA             *IdentifyData = NULL;
	UINT32                        BufferSize;
	UINTN                         RotationRate;
	
    BYO_DISKINFO_PROTOCOL          *DiskInfo;
    EFI_HANDLE                     *HandleBuffer;
    UINTN                          HandleCount;
    UINT64                         NumSectors = 0; 
    UINT64                         DriveSizeInBytes = 0;
    UINT64                         RemainderInBytes = 0;
	UINT32						   DriveSizeInGB;
    UINT8                          Index;	
	CHAR8							Mn[41];
	CHAR8							Sn[21];
	UINTN							Size;
	UINT64							DiskSize;
	EFI_HANDLE				   		DiskDevHandle;
	EFI_PCI_IO_PROTOCOL 	   		*PciIo;
	EFI_DEVICE_PATH_PROTOCOL   		*DiskDp; 
  	UINT8               			HddCount=0;
  	CHAR16							*OsDesc;

  if (PcdGet8(PcdLegacyBiosSupport)) {
    return;
  }
	
	OsDesc= NULL;
	OsDesc = AllocateZeroPool (100);

    Status = gBS->LocateHandleBuffer (
         ByProtocol,
         &gByoDiskInfoProtocolGuid,
         NULL,
         &HandleCount,
         &HandleBuffer
         );
  	DEBUG((EFI_D_INFO,"GetDiskInfo %r HandleCount %x\n",Status,HandleCount));

    if(EFI_ERROR(Status)){
        return ;
      }
	

	  
   for (Index = 0; Index < HandleCount; Index++) {
    	Status = gBS->HandleProtocol(
                    HandleBuffer[Index],
                    &gByoDiskInfoProtocolGuid,
                    (VOID**)&DiskInfo
                    );
    ASSERT_EFI_ERROR(Status);

	Status = gBS->HandleProtocol(
                  HandleBuffer[Index],
                  &gEfiDiskInfoProtocolGuid,
                  (VOID**)&DiskInfo1
                  );

    IdentifyData = AllocatePool(sizeof(ATA_IDENTIFY_DATA));
	BufferSize = sizeof(ATA_IDENTIFY_DATA);
    Status = DiskInfo1->Identify (
                       DiskInfo1,
                       IdentifyData,
                       &BufferSize
                       );
    RotationRate = GetAtaMediaRotationRate(IdentifyData);
	DEBUG((EFI_D_INFO,"RotationRatevalue:%d\n",RotationRate));
	
    DiskInfo->GetDp(DiskInfo, &DiskDp);
  	Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &DiskDp, &DiskDevHandle);
    ASSERT(!EFI_ERROR(Status));
    Status = gBS->HandleProtocol(DiskDevHandle, &gEfiPciIoProtocolGuid, &PciIo);
    ASSERT(!EFI_ERROR(Status));



  HddCount++;
  if(!ISOsDescription(HandleBuffer[Index],OsDesc)){ //Only show system Hdd 
    continue;
  }
  DEBUG((EFI_D_INFO, "%a() OsDesc %s\n", __FUNCTION__, OsDesc));

    Size = sizeof(Mn);
    ZeroMem (&Mn[0], Size);
    Status = DiskInfo->GetMn(DiskInfo, Mn, &Size);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, __FUNCTION__"(), GetMn :%r.\n", Status));		
      continue;
    }

    Size = sizeof(Sn);
    ZeroMem (&Sn[0], Size);
    Status = DiskInfo->GetSn(DiskInfo, Sn, &Size);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, __FUNCTION__"(), GetSn :%r.\n", Status));		
      continue;
    }

    DiskSize = 0;
    Status = DiskInfo->GetDiskSize(DiskInfo, &DiskSize);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, __FUNCTION__"(), GetDiskSize :%r.\n", Status));		
      continue;
    }

    if (Mn[0] == 0 || Sn[0] == 0 ||DiskSize == 0) {
      continue;
    }

    DriveSizeInGB = (UINT32) DivU64x64Remainder(DiskSize, 1000000000, &RemainderInBytes);  
    DriveSizeInGB &=~1;
    DEBUG((EFI_D_ERROR, __FUNCTION__"(), DriveSizeInGB :%a %a %d.\n", Mn,Sn,DriveSizeInGB));

	if(DiskInfo->DevType == BYO_DISK_INFO_DEV_NVME){
		if(BeEnglish()){
			DisplayScreenText (0xFF, "%s %s: %a, Size: %dGB", L"NVME", OsDesc, Mn, DriveSizeInGB);
		}else{
			DisplayScreenText (0xFF, "%s %s: %a, %s: %dGB", L"NVME", OsDesc,  Mn, L"大小", DriveSizeInGB);
			}
  		}
	else{
		if(BeEnglish()){
			if(RotationRate == 1)
		    {
			 DisplayScreenText (0xFF, "%s %s: %a, Size: %dGB", L"SSD", OsDesc,  Mn, DriveSizeInGB);
			} else {
			DisplayScreenText (0xFF, "%s %s: %a, Size: %dGB", L"HDD", OsDesc,  Mn, DriveSizeInGB);
			}
		}else{
		if(RotationRate == 1)
			{
			DisplayScreenText (0xFF, "%s %s: %a, %s: %dGB", L"SSD", OsDesc,  Mn,   L"大小", DriveSizeInGB);
			}else{
			DisplayScreenText (0xFF, "%s %s: %a, %s: %dGB", L"HDD", OsDesc,  Mn,  L"大小", DriveSizeInGB);
				}
			}
		}
  	}


 if (HandleBuffer != NULL){
    FreePool(HandleBuffer);
  }

}

EFI_STATUS
EFIAPI
ByoEfiBootManagerFreeLoadOptions (
  IN  EFI_BOOT_MANAGER_LOAD_OPTION  *LoadOptions,
  IN  UINTN                         LoadOptionCount
  );

EFI_BOOT_MANAGER_LOAD_OPTION *
EFIAPI
EfiBootManagerGetLoadOptions (
  OUT UINTN                            *LoadOptionCount,
  IN EFI_BOOT_MANAGER_LOAD_OPTION_TYPE LoadOptionType
  );

VOID
GetRaidInfo()
{
  EFI_BOOT_MANAGER_LOAD_OPTION    *Option;
  UINTN                           BootOptionCount;
  UINTN                           i;
  UINTN                           Index;
  UINTN                           LoaderCount;
  UINTN                           RaidIndex;
  CHAR16                          Temp[64];
  CHAR16                          *TempString=NULL;
  //BOOLEAN                          SkipDesc;

  Option = EfiBootManagerGetLoadOptions(&BootOptionCount, LoadOptionTypeBoot);
  DEBUG((EFI_D_INFO,"BootOptionCount is %d\n",BootOptionCount));


      LoaderCount = sizeof(mBdsGeneralUefiBootOs1)/sizeof(mBdsGeneralUefiBootOs1[0]);

      for (i = 0; i < BootOptionCount; i++) {
        if(Option[i].Description==NULL){
          continue;
        }
        if(StrStr(Option[i].Description,L"HDD")!=NULL){
          continue;
        }
		if(StrStr(Option[i].Description,L"SSD")!=NULL && StrStr(Option[i].Description,L"SAS")==NULL){
          continue;
		}
        if(StrStr(Option[i].Description,L"NVME")!=NULL){
          continue;
        }
        if(StrStr(Option[i].Description,L"USB")!=NULL){
          continue;
        }
        if(StrStr(Option[i].Description,L"SATA")!=NULL||StrStr(Option[i].Description,L"ODD")!=NULL){
          continue;
        }
        for(Index = 0; Index < LoaderCount;Index++){

      /*    if(StrCmp(mBdsGeneralUefiBootOs1[Index].Description,Option[i].Description)==0){
            SkipDesc=PcdGetBool(IsByoDesc);
			if(!SkipDesc){
              DisplayScreenText (0xFF, "%s",Option[i].Description);
		    }
            continue;
          }*/
          if(StrStr(Option[i].Description,mBdsGeneralUefiBootOs1[Index].Description)!=NULL){
           TempString=Option[i].Description;
           while(*TempString!=L'('){
            *TempString++;
           }
           *TempString++;
           for(RaidIndex=0;*TempString!=L')';RaidIndex++){
              Temp[RaidIndex]=*TempString++;
           }
            
           Temp[RaidIndex]=L'\0'; 
          if(StrStr(Temp,L"(")){
            StrCat(Temp, L")");
          }
           DisplayScreenText (0xFF, "%s: %s",mBdsGeneralUefiBootOs1[Index].Description,Temp);
           TempString=NULL;
           break;
          }
        } 
    }  

  ByoEfiBootManagerFreeLoadOptions (Option, BootOptionCount);
}


EFI_STATUS
UpdateOnboardLanMac (void)
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  UINT8                     MacAddr[6];
  PLAT_HOST_INFO_PROTOCOL   *ptHostInfo;  
  UINTN                     Index;
  UINTN                     LanIndex = 0;
  UINTN                     LanCount;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &ptHostInfo);
  ASSERT(!EFI_ERROR(Status));

  LanCount = 2; //sizeof(gObLanStrList)/sizeof(gObLanStrList[0]);

  for(Index=0;Index<ptHostInfo->HostCount;Index++){
    if(ptHostInfo->HostList[Index].HostType == PLATFORM_HOST_LAN && LanIndex < LanCount){
      DevPath = ptHostInfo->HostList[Index].Dp;
      Status = GetOnboardLanMacAddress(gBS, DevPath, MacAddr);
      if(!EFI_ERROR(Status)){
        if(BeEnglish()){
          if (LanIndex) { 			
            DisplayScreenText (0xFF, "%s: %02X-%02X-%02X-%02X-%02X-%02X", L"Onboard NIC 1 MAC Address", MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]);
          } else {
            DisplayScreenText (0xFF, "%s: %02X-%02X-%02X-%02X-%02X-%02X", L"Onboard NIC 0 MAC Address", MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]);
          }
        }else{
          if (LanIndex) { 			
            DisplayScreenText (0xFF, "%s: %02X-%02X-%02X-%02X-%02X-%02X", L"板载网卡1 MAC地址", MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]);
          } else {
            DisplayScreenText (0xFF, "%s: %02X-%02X-%02X-%02X-%02X-%02X", L"板载网卡0 MAC地址", MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]);
          }

        }
        DEBUG((EFI_D_INFO,"MAC address: %02X-%02X-%02X-%02X-%02X-%02X\n", MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]));
      }
      LanIndex++;
    }
  }

  return Status;  
}

VOID
GetUSBInfo (
    VOID
  )
{
  EFI_STATUS                     Status;
  UINTN                          HandleCount;
  EFI_HANDLE                     *Handles;
  EFI_USB_IO_PROTOCOL            *UsbIo;
  UINTN	                         Index;
  EFI_USB_INTERFACE_DESCRIPTOR   Interface;
  EFI_USB_DEVICE_DESCRIPTOR      DevDesc;
  CHAR16                         *Manufacturer = NULL;
  CHAR16                         *Product = NULL;
  UINT16                         UsbLangId = 0x0409; // English
  BOOLEAN                        NeedFree1 = FALSE;
  BOOLEAN                        NeedFree2 = FALSE;

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiUsbIoProtocolGuid, NULL, &HandleCount, &Handles);
  for (Index = 0; Index < HandleCount; Index++) {
     Status = gBS->HandleProtocol (Handles[Index], &gEfiUsbIoProtocolGuid, (VOID **) &UsbIo);
      if (EFI_ERROR (Status)) {
       continue;
      }

     Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &Interface);
      if (EFI_ERROR (Status)) {
       continue;
     }

     Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DevDesc);
       if (EFI_ERROR (Status)) {
         return;
       }

      Status = UsbIo->UsbGetStringDescriptor (UsbIo, UsbLangId, DevDesc.StrManufacturer, &Manufacturer);
        if (EFI_ERROR (Status)) {
          Manufacturer = L"";
        } else {
          NeedFree1 = TRUE;
        }

  Status = UsbIo->UsbGetStringDescriptor (UsbIo, UsbLangId, DevDesc.StrProduct, &Product);
        if (EFI_ERROR (Status)) {
        Product = L"";
        } else {
          NeedFree2 = TRUE;
         }

        if (!NeedFree1 && !NeedFree2) {
          return ;
        }

        if(StrStr(Manufacturer,L"Linux")){
        DEBUG((EFI_D_INFO, "skip bmc usb kb&ms\n"));
         if ((Manufacturer != NULL)||(Product != NULL)) {
         FreePool (Manufacturer);
         FreePool (Product);
         }
         continue;
        }
    
        if (Interface.InterfaceClass == CLASS_HID && Interface.InterfaceSubClass == SUBCLASS_BOOT && Interface.InterfaceProtocol == PROTOCOL_KEYBOARD){
            if(BeEnglish()){
                DisplayScreenText (0xFF, "%s: %s %s", L"Keyboard", Manufacturer, Product);
                }else{
                DisplayScreenText (0xFF, "%s: %s %s", L"键盘", Manufacturer, Product);
                }
        }   
        else if (Interface.InterfaceClass == CLASS_HID && Interface.InterfaceSubClass == SUBCLASS_BOOT && Interface.InterfaceProtocol == PROTOCOL_MOUSE){
            if(BeEnglish()){
                DisplayScreenText (0xFF, "%s: %s %s", L"Mouse", Manufacturer, Product);
                }else{
                DisplayScreenText (0xFF, "%s: %s %s", L"鼠标", Manufacturer, Product);
                }
        }

//if(NeedFree1)
//FreePool(Manufacturer);
//if(NeedFree2)
// FreePool(Product);
 
   }

  return ;
  
}
 
VOID
Displaytextmodemessage (
  )
{
      UINT8                             Oembiosver,Oembiosdata,OemCpuInfo;
      CHAR16                            *Oembiosverstring;
      CHAR16                            *Oembiosdatestring;
      CHAR16                            *OemCpuInfostring = NULL;
      UINT16                            OemCpuSpeed;
      BOOLEAN                           Find[4];
      BOOLEAN                           Type4Find[2] = { FALSE , FALSE};
      EFI_STATUS                        Status;
      EFI_SMBIOS_HANDLE                 SmbiosHandle;
      EFI_SMBIOS_PROTOCOL               *Smbios;
      SMBIOS_TABLE_TYPE0                *Type0Record;
      SMBIOS_TABLE_TYPE2                *Type2Record;
      SMBIOS_TABLE_TYPE3                *Type3Record;
      SMBIOS_TABLE_TYPE4                *Type4Record;
      EFI_SMBIOS_TABLE_HEADER           *Record;
      UINTN                             CPUCount = 0;

      UINT32                            Cpuid;	

      DEBUG((EFI_D_ERROR, __FUNCTION__"(),\n\n"));

	  CleanScreenText ();
      ZeroMem (Find, sizeof (Find));

      Status = gBS->LocateProtocol (
                      &gEfiSmbiosProtocolGuid,
                      NULL,
                      (VOID **) &Smbios
                      );
      if (EFI_ERROR (Status)) {
        return ;
      }
      SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
      do {
        Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
        if (EFI_ERROR(Status)) {
          break;
        }

        if (Record->Type == EFI_SMBIOS_TYPE_BIOS_INFORMATION) {
          Type0Record = (SMBIOS_TABLE_TYPE0 *) Record;
          Oembiosver = Type0Record->BiosVersion;
          GetOptionalStringByIndex ((CHAR8*)((UINT8*)Type0Record + Type0Record->Hdr.Length), Oembiosver, &Oembiosverstring);
          Oembiosdata = Type0Record->BiosReleaseDate;
          GetOptionalStringByIndex ((CHAR8*)((UINT8*)Type0Record + Type0Record->Hdr.Length), Oembiosdata, &Oembiosdatestring);
          Find[0] = TRUE;
        }

        if ((Record->Type == EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION) && !Find[2]) {
          Type2Record = (SMBIOS_TABLE_TYPE2 *) Record;
            Find[2] = TRUE;
          }

        if ((Record->Type == EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE) && !Find[3]) {
          Type3Record = (SMBIOS_TABLE_TYPE3 *) Record;
            Find[3] = TRUE;
        }
        if ((Record->Type == EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION) && (!Type4Find[0] || !Type4Find[1])) {
          if (Type4Find[0] == FALSE) {
            Type4Find[0] = TRUE;
          } else {
            Type4Find[1] = TRUE;
          }
          
          Type4Record    = (SMBIOS_TABLE_TYPE4 *) Record;
          if (*(UINT32*)&(Type4Record->ProcessorId.Signature) == 0) {
          continue;
          }
          
          OemCpuInfo     = Type4Record->ProcessorVersion;
          OemCpuSpeed    = Type4Record->CurrentSpeed;

          if (OemCpuInfostring) {
            FreePool (OemCpuInfostring);
            OemCpuInfostring = NULL;
          }
          GetOptionalStringByIndex ((CHAR8*)((UINT8*)Type4Record + Type4Record->Hdr.Length), OemCpuInfo, &OemCpuInfostring);

        }
		
        if(Record->Type == EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION){
            Type4Record = (SMBIOS_TABLE_TYPE4 *) Record;
            Cpuid = *(UINT32*)&(Type4Record->ProcessorId.Signature);
            if(Cpuid != 0){
            CPUCount++;
            }  
        }  
      }while ( !(Find[0] && Find[2] && Find[3] && Type4Find[0] && Type4Find[1]));   

    if (Oembiosverstring != NULL && Oembiosdatestring != NULL && OemCpuInfostring != NULL)
    {
    if(BeEnglish()){
        DisplayScreenText (0xFF, "%s: %s, %s: %s", L"BIOS Version", Oembiosverstring, L"Release Date", Oembiosdatestring);		
        DisplayScreenText (0xFF, "%s: %s, %s: %dMHz", L"CPU 0", OemCpuInfostring, L"Frequency", OemCpuSpeed);
	    if(CPUCount==2){
           DisplayScreenText (0xFF, "%s: %s, %s: %dMHz", L"CPU 1", OemCpuInfostring, L"Frequency", OemCpuSpeed);
		}
    }else{
        DisplayScreenText (0xFF, "%s: %s, %s: %s", L"BIOS版本", Oembiosverstring, L"发布日期", Oembiosdatestring);
        DisplayScreenText (0xFF, "%s: %s, %s: %dMHz", L"处理器 0", OemCpuInfostring, L"频率", OemCpuSpeed);
		if(CPUCount==2){
           DisplayScreenText (0xFF, "%s: %s, %s: %dMHz", L"处理器 1", OemCpuInfostring, L"频率", OemCpuSpeed);
		}
    }
	
    if (OemCpuInfostring) {
      FreePool (OemCpuInfostring);
      OemCpuInfostring = NULL;
    }


    GetBMCInfo();
    GetMemoryInfo(Smbios,CPUCount);
    UpdateOnboardLanMac();
    GetUSBInfo();
    GetDiskInfo();
    GetRaidInfo();

   	}

}


EFI_STATUS
EFIAPI
DisplayTextModeConstructor (
  IN      EFI_HANDLE                ImageHandle,
  IN      EFI_SYSTEM_TABLE          *SystemTable
  )
{
  EFI_EVENT     Event;
  EFI_STATUS    Status;
  UINT8         QuiteBoot;
  
  gSetupHob = GetSetupDataHobData();
  QuiteBoot = gSetupHob->QuiteBoot;
  
  if (QuiteBoot == 0) {
  	gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gByoBdsWaitHookProtocolGuid, Displaytextmodemessage,
                  NULL
                  );
 
  DEBUG((EFI_D_INFO,"ByoBdsWaitHookProtocol\n"));

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             CleanScreenTextEvent,
             NULL,
             &Event
             );
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), Craete CleanScreenText :%r.\n", Status));

  	}
  return EFI_SUCCESS;
}

