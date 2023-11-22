

#include "PlatformSetupDxe.h"
#include <Library/TimeStampLib.h>
#include <Protocol/HygonPspTpcmProtocol.h>



EFI_STRING_ID
SetStringWithString (
  EFI_HII_HANDLE    HiiHandle,
  EFI_STRING_ID     StrId,
  EFI_STRING_ID     RefStrId
  );

VOID
InitString (
  EFI_HII_HANDLE    HiiHandle,
  EFI_STRING_ID     StrRef,
  CHAR16            *sFormat, ...
  );

VOID UpdateTpcmInfo(EFI_HII_HANDLE HiiHandle)
{
  EFI_STATUS                          Status;
  PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL  *ImageVerify;
  TPCM_STATUS_DATA                    TpcmStatus;
  TPCM_LICENSE_INFO                   LicenseInfo;
  UINT32                              Seconds;
  DATE_TIME_INFO                      CmdTime;
  DATE_TIME_INFO                      AuthTime;
  EFI_STRING_ID                       StrId;
  UINT8                               *Data8;
  CHAR8                               Buffer[128];   
  

  DEBUG((EFI_D_INFO, "UpdateTpcmInfo\n"));

  Status = gBS->LocateProtocol(&gPspC2PmboxImageVerifyProtocolGuid, NULL, (VOID**)&ImageVerify);
  if(EFI_ERROR(Status)){
    return;
  }

  Seconds = 0;
  GetTimeStampLib(&Seconds);
  DEBUG((EFI_D_INFO, "Seconds:%d\n", Seconds));
  Status = ImageVerify->GetTpcmStatus(ImageVerify, Seconds, &TpcmStatus);
  if(EFI_ERROR(Status)){
    return;
  }
  TimeStampToDateTime(TpcmStatus.host_time, &CmdTime);

  Status = ImageVerify->GetTpcmLicenseInfo(ImageVerify, &LicenseInfo);
  if(EFI_ERROR(Status)){
    return;
  }  
  TimeStampToDateTime(LicenseInfo.AuthTime, &AuthTime);

  AsciiSPrint(Buffer, sizeof(Buffer), "%04d.%02d.%02d %02d:%02d:%02d", 
    CmdTime.year, CmdTime.mon, CmdTime.day, CmdTime.hour, CmdTime.min, CmdTime.sec);
  InitString (
    HiiHandle,
    STRING_TOKEN(STR_TPCM_INFO_GET_TIME_VALUE),
    L"%a",
    Buffer
    );

  AsciiSPrint(Buffer, sizeof(Buffer), "%04d.%02d.%02d %02d:%02d:%02d", 
    AuthTime.year, AuthTime.mon, AuthTime.day, AuthTime.hour, AuthTime.min, AuthTime.sec);
  InitString (
    HiiHandle,
    STRING_TOKEN(STR_TPCM_AUTH_START_TIME_VALUE),
    L"%a",
    Buffer
    );
  
  AsciiSPrint(Buffer, sizeof(Buffer), "%04d.%02d.%02d %02d:%02d:%02d", 
    AuthTime.year+3, AuthTime.mon, AuthTime.day, AuthTime.hour, AuthTime.min, AuthTime.sec);
  InitString (
    HiiHandle,
    STRING_TOKEN(STR_TPCM_AUTH_END_TIME_VALUE),
    L"%a",
    Buffer
    );

  switch(TpcmStatus.tpcm_type){
    case 1:
      StrId = STRING_TOKEN(STR_TPCM_TRUSTED_ROOT_FORM_MAIN);
      break;
    case 2:
      StrId = STRING_TOKEN(STR_TPCM_TRUSTED_ROOT_FORM_ADDITIONAL);
      break;
    case 3:
      StrId = STRING_TOKEN(STR_TPCM_TRUSTED_ROOT_FORM_SIM);
      break;      
    case 5:
      StrId = STRING_TOKEN(STR_TPCM_TRUSTED_ROOT_FORM_EXT);
      break;
    default:
      StrId = 0xFFFF;
      break;
  }
  if(StrId != 0xFFFF){
    SetStringWithString(HiiHandle, STRING_TOKEN(STR_TPCM_TRUSTED_ROOT_FORM_VALUE), StrId);
  }

  AsciiSPrint(Buffer, sizeof(Buffer), "%d", TpcmStatus.tpcm_total_flash/1024);
  InitString (
    HiiHandle,
    STRING_TOKEN(STR_TPCM_STORAGE_SIZE_VALUE),
    L"%a",
    Buffer
    );  

  AsciiSPrint(Buffer, sizeof(Buffer), "%d", TpcmStatus.tpcm_whiltelist_availble_flash/1024);
  InitString (
    HiiHandle,
    STRING_TOKEN(STR_TPCM_USABLE_STORAGE_SIZE_VALUE),
    L"%a",
    Buffer
    );    

  switch(TpcmStatus.TrustedBootState){
    case 0:
      StrId = STRING_TOKEN(STR_TPCM_TRUSTED);
      break;
    case 1:
      StrId = STRING_TOKEN(STR_TPCM_UNTRUSTED);
      break;
    case 2:
      StrId = STRING_TOKEN(STR_TPCM_UNKNOWN);
      break;      
    default:
      StrId = 0xFFFF;
      break;
  }
  if(StrId != 0xFFFF){
    SetStringWithString(HiiHandle, STRING_TOKEN(STR_TPCM_TRUSTED_STATE_VALUE), StrId);
  }

  Data8 = (UINT8*)&TpcmStatus.tpcm_firmware_version;
  AsciiSPrint(Buffer, sizeof(Buffer), "%d.%d.%d", Data8[3], Data8[2], Data8[1]);
  InitString (
    HiiHandle,
    STRING_TOKEN(STR_TPCM_VERSION_VALUE),
    L"%a",
    Buffer
    );    

  SetStringWithString(HiiHandle, STRING_TOKEN(STR_TPCM_HAS_ROOT_STORAGE_KEY_VALUE), 
    TpcmStatus.smk_generated ? STRING_TOKEN(STR_TPCM_GENERATED) : STRING_TOKEN(STR_TPCM_NOT_GENERATED));

}


