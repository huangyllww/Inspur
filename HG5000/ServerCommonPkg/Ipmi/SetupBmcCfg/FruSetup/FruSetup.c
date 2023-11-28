

#include <SetupBmcCfg.h>
#include <Protocol/SdrSensorDevProtocol.h>
#include <IndustryStandard/IpmiNetFnStorage.h>
#include <Protocol/Smbios.h>


VOID InitString(EFI_HII_HANDLE HiiHandle, EFI_STRING_ID StrRef, CHAR16 *sFormat, ...);


/**
    Read the SDR version details from the BMC.
    Updates the SDR version in Dynamic PCD.

    @param VOID

    @retval EFI_STATUS Return Status

**/
CHAR8*
GetSdrVerNum ()
{
  EFI_STATUS                          Status;
  IPMI_GET_SDR_REPOSITORY_INFO        SdrInfo;
  UINT8                               SdrInfoSize = sizeof (SdrInfo);
  CHAR8                               *SdrVersionNo = NULL;

  DEBUG ((EFI_D_INFO, "%a Entry... \n", __FUNCTION__));
  //
  // Get SDR Version using Get SDR Repository Info command
  //
  Status = EfiSendCommandToBMC (
             IPMI_NETFN_STORAGE,
             IPMI_STORAGE_GET_SDR_REPOSITORY_INFO,
             NULL,
             0,
             (UINT8 *)&SdrInfo,
             (UINT8 *)&SdrInfoSize
             );
  DEBUG ((EFI_D_INFO, " IPMI_STORAGE_GET_SDR_REPOSITORY_INFO Status: %r\n", Status ));
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  //
  // Got the required SDR version Number in BCD format
  // Convert it to Unicode format and Update the same in PCD
  //
  SdrVersionNo = AllocateZeroPool ((UINTN)( sizeof (UINT8) * 4));  //Allocate size for two nibbles and a dot as a Unicode string with NULL terminator.
  if (SdrVersionNo == NULL) {
      return NULL;
  }
  DEBUG ((EFI_D_INFO, "SdrVersionNo Address : %x\n", SdrVersionNo));
  DEBUG ((EFI_D_INFO, "Version : %x\n", SdrInfo.Version));

  //
  // Version will be in format LSB.MSB example. 51h as 1.5
  //
  AsciiSPrint(SdrVersionNo, 4, "%02X", SdrInfo.Version);
  SdrVersionNo[2] = SdrVersionNo[1]; // MSB of Version
  SdrVersionNo[1] = '.'; // Dot Separates LSB and MSB

  DEBUG ((EFI_D_INFO, "SdrVersionNo: %a \n", SdrVersionNo));
  return SdrVersionNo;
}

STATIC UINT16 SmbiosGetStrOffset(CONST SMBIOS_STRUCTURE *Hdr, UINT8 Index)
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


VOID
InitFruStrings (
  IN EFI_HII_HANDLE     HiiHandle
  )
{
  EFI_GUID                            DefaultSystemUuid = {0x00};
  CHAR8                               *UpdateStr;
  EFI_GUID                            *Uuid;
  EFI_SMBIOS_PROTOCOL                 *Smbios;
  EFI_SMBIOS_HANDLE                   SmbiosHandle;
  EFI_SMBIOS_TYPE                     SmbiosType;
  EFI_SMBIOS_TABLE_HEADER             *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER            p;
  EFI_STATUS                          Status;
  //CHAR16                              *Str; 
  //UINT8                               Index;

  DEBUG ((EFI_D_INFO, "%a()\n", __FUNCTION__));
  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if(EFI_ERROR(Status)){
    return;
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_SYSTEM_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr = SmbiosHdr;
  UpdateStr = SmbiosGetStringInTypeByIndex(p, p.Type1->Manufacturer);
  InitString (
      HiiHandle,
      STRING_TOKEN(STR_PRODUCT_MANUFACTURER_VAL),
      L"%a",
      UpdateStr );

  UpdateStr = SmbiosGetStringInTypeByIndex(p, p.Type1->ProductName);
  InitString (
      HiiHandle,
      STRING_TOKEN(STR_PRODUCT_NAME_VAL),
      L"%a",
      UpdateStr );

  UpdateStr = SmbiosGetStringInTypeByIndex(p, p.Type1->Version);
  InitString (
      HiiHandle,
      STRING_TOKEN(STR_PRODUCT_VERSION_VAL),
      L"%a",
      UpdateStr );


  UpdateStr = SmbiosGetStringInTypeByIndex(p, p.Type1->SerialNumber);
  InitString (
      HiiHandle,
      STRING_TOKEN(STR_PRODUCT_SN_VAL),
      L"%a",
      UpdateStr );

  Uuid = (EFI_GUID *) &(p.Type1->Uuid);
  if (CompareGuid ((CONST EFI_GUID*)Uuid, (CONST EFI_GUID*)&DefaultSystemUuid) == FALSE) {
      DEBUG ((EFI_D_INFO, "UUID:%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\n", Uuid->Data1, Uuid->Data2, Uuid->Data3, Uuid->Data4[0], Uuid->Data4[1], Uuid->Data4[2], Uuid->Data4[3], Uuid->Data4[4], Uuid->Data4[5], Uuid->Data4[6], Uuid->Data4[7]));
      DEBUG ((EFI_D_INFO, "UUID:%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n", Uuid->Data1, Uuid->Data2, Uuid->Data3, Uuid->Data4[0], Uuid->Data4[1], Uuid->Data4[2], Uuid->Data4[3], Uuid->Data4[4], Uuid->Data4[5], Uuid->Data4[6], Uuid->Data4[7]));
      InitString (
          HiiHandle,
          STRING_TOKEN(STR_SYSTEM_UUID_VAL),
          L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
          Uuid->Data1, Uuid->Data2, Uuid->Data3, Uuid->Data4[0], Uuid->Data4[1], Uuid->Data4[2], Uuid->Data4[3], Uuid->Data4[4], Uuid->Data4[5], Uuid->Data4[6], Uuid->Data4[7]
          );
	 /* Str = HiiGetString(HiiHandle, STRING_TOKEN(STR_SYSTEM_UUID_VAL), NULL);
      for(Index=0;Index<StrLen(Str);Index++){
        if(Str[Index] >= 'A' && Str[Index] <= 'Z'){
		  Str[Index] += 32;
		 }
	  }
      InitString (
          HiiHandle,
          STRING_TOKEN(STR_SYSTEM_UUID_VAL),
          L"%s",
          Str
          );*/
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr = SmbiosHdr;
  UpdateStr = SmbiosGetStringInTypeByIndex(p, p.Type2->Manufacturer);
  InitString (
      HiiHandle,
      STRING_TOKEN(STR_BOARD_MANUFACTURER_VAL),
      L"%a",
      UpdateStr );

  UpdateStr = SmbiosGetStringInTypeByIndex(p, p.Type2->ProductName);
  InitString (
      HiiHandle,
      STRING_TOKEN(STR_BOARD_PRODUCT_NAME_VAL),
      L"%a",
      UpdateStr );

  UpdateStr = SmbiosGetStringInTypeByIndex(p, p.Type2->Version);
  InitString (
      HiiHandle,
      STRING_TOKEN(STR_BOARD_PN_VAL),
      L"%a",
      UpdateStr );

  UpdateStr = SmbiosGetStringInTypeByIndex(p, p.Type2->SerialNumber);
  InitString (
      HiiHandle,
      STRING_TOKEN(STR_BOARD_SN_VAL),
      L"%a",
      UpdateStr );
  

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr = SmbiosHdr;
  UpdateStr = SmbiosGetStringInTypeByIndex(p, p.Type3->Manufacturer);
  InitString (
      HiiHandle,
      STRING_TOKEN(STR_CHASSIC_MANUFACTURER_VAL),
      L"%a",
      UpdateStr );
  
  UpdateStr = SmbiosGetStringInTypeByIndex(p, p.Type3->Type);
  DEBUG ((EFI_D_INFO, "[Stephen] Type3->Type(CHASSIC_TYPE): %a\n",UpdateStr));
/*  InitString (
      HiiHandle,
      STRING_TOKEN(STR_CHASSIC_TYPE_VAL),
      L"%a",
      UpdateStr );
*/
  UpdateStr = SmbiosGetStringInTypeByIndex(p, p.Type3->Version);
  InitString (
      HiiHandle,
      STRING_TOKEN(STR_CHASSIC_PN_VAL),
      L"%a",
      UpdateStr );

  UpdateStr = SmbiosGetStringInTypeByIndex(p, p.Type3->SerialNumber);
  InitString (
      HiiHandle,
      STRING_TOKEN(STR_CHASSIC_SN_VAL),
      L"%a",
      UpdateStr );

  UpdateStr = SmbiosGetStringInTypeByIndex(p, p.Type3->AssetTag);  //+TangYan+
  InitString (
      HiiHandle,
      STRING_TOKEN(STR_PRODUCT_ASSET_TAG_VAL),
      L"%a",
      UpdateStr );
  //
  // SdrVersionNo
  //
  UpdateStr = GetSdrVerNum ();
  DEBUG ((EFI_D_INFO, "SdrVersionNo: UpdateStr: %a\n", UpdateStr));  //-TangYan-
  InitString (
      HiiHandle,
      STRING_TOKEN(STR_SDR_VERSION_VAL),
      L"%a",
      UpdateStr );

  if(UpdateStr != NULL){
    FreePool(UpdateStr);
  }

  DEBUG ((EFI_D_INFO, "%a Exiting...\n", __FUNCTION__));
  return ;

}
