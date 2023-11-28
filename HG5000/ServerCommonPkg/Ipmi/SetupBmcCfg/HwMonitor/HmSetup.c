/*++
Copyright (c) 2006-2019 Byosoft Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Byosoft Corporation.

Module Name:

  HmSetup.c

Abstract:

  This file does Setup related code for Heceta module
--*/

#include <Protocol/SdrSensorDevProtocol.h>
#include <Library/TimerLib.h>
#include <SetupBmcCfg.h>
#include <Hm.h>

SDR_SENOSR_DEV_PROTOCOL         *mSensorDev = NULL;
extern EFI_HII_HANDLE           HiiHandle;

VOID
InitString (
  EFI_HII_HANDLE    HiiHandle,
  EFI_STRING_ID     StrRef,
  CHAR16            *sFormat, ...
  )
{
  STATIC CHAR16 s[1024];
  VA_LIST  Marker;

  VA_START (Marker, sFormat);
  UnicodeVSPrint (s, sizeof (s),  sFormat, Marker);
  VA_END (Marker);

  HiiSetString (HiiHandle, StrRef, s, NULL);
}

void
UpdateSenorValue (
  UINT8    SenorNum,
  UINT16   StringToken,
  CHAR16   *String
)
{
  UINTN       Data = 0;
  UINT8       Div  = 0;
  
  mSensorDev->SensorReading (mSensorDev, SenorNum, 0, &Data, &Div);
  if(Data != 0x0) {
    DEBUG((EFI_D_ERROR,"%s, data:%d, Div:%x\n",String, Data, Div));

    switch (Div)
    {
    case 0x0C:
      InitString (HiiHandle, StringToken, L"%2d.%-4d  %s", Data/10000, Data%10000, String);
      break;

    case 0x0D:
      InitString (HiiHandle, StringToken, L"%2d.%-4d  %s", Data/1000, Data%1000, String);
      break;

    case 0x88:  //fanspeed watts
      InitString (HiiHandle, StringToken, L"%4d  %s", Data, String);
      break;

    default:  //temp
      InitString (HiiHandle, StringToken, L"%2d  %s", (UINT8)Data, String);
      break;
    }

  } else {
    InitString (HiiHandle, StringToken, L"No Reading");
  }
}


EFI_STATUS
EFIAPI
HhmCallBack (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      Key,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
  )
{
  EFI_STATUS                  Status;

  if(mSensorDev == NULL){
    Status = gBS->LocateProtocol (
                    &gSdrSensorDevProtocolGuid,
                    NULL,
                    &mSensorDev
                    );
     if (EFI_ERROR (Status)) {
       //DEBUG((EFI_D_ERROR,"HmCallBack: locate mSensorDev protocol fail...\n"));
       return EFI_INVALID_PARAMETER;
     }
  }
  Status = EFI_SUCCESS;
  DEBUG((EFI_D_ERROR,"%a() Action :%x. KeyValue = %x\n",__FUNCTION__, Action,Key));
  switch (Key) {
      case SENOR_TEMP_KEY:
          // degrees C
          UpdateSenorValue (CPU0_Temp,       STRING_TOKEN (STR_CPU0_TEMP_VALUE),       L"degrees °„C");
          UpdateSenorValue (CPU1_Temp,       STRING_TOKEN (STR_CPU1_TEMP_VALUE),       L"degrees °„C");
          UpdateSenorValue (PCIe_Amb_Temp,   STRING_TOKEN (STR_PCIE_AMB_TEMP_VALUE),   L"degrees °„C");
          UpdateSenorValue (OCP_Amb_Temp,    STRING_TOKEN (STR_OCP_AMB_TEMP_VALUE),    L"degrees °„C");
          UpdateSenorValue (SYS_Outlet_Temp, STRING_TOKEN (STR_SYS_OUTLET_TEMP_VALUE), L"degrees °„C");
          UpdateSenorValue (CPU0_Inlet_Temp, STRING_TOKEN (STR_CPU0_INLET_TEMP_VALUE), L"degrees °„C");
          UpdateSenorValue (CPU1_Inlet_Temp, STRING_TOKEN (STR_CPU1_INLET_TEMP_VALUE), L"degrees °„C");
          UpdateSenorValue (PSU1_Inlet_Temp, STRING_TOKEN (STR_PSU1_INLET_TEMP_VALUE), L"degrees °„C");
          UpdateSenorValue (PSU2_Inlet_Temp, STRING_TOKEN (STR_PSU2_INLET_TEMP_VALUE), L"degrees °„C");
          UpdateSenorValue (PSU1_Hs_Temp,    STRING_TOKEN (STR_PSU1_HS_TEMP_VALUE),    L"degrees °„C");
          UpdateSenorValue (PSU2_Hs_Temp,    STRING_TOKEN (STR_PSU2_HS_TEMP_VALUE),    L"degrees °„C");
          
          // Volts
          UpdateSenorValue (CPU0_VCORE,     STRING_TOKEN (STR_CPU0_VCORE_VALUE),     L"Volts");
          UpdateSenorValue (CPU1_VCORE,     STRING_TOKEN (STR_CPU1_VCORE_VALUE),     L"Volts");
          UpdateSenorValue (CPU0_VSOC,      STRING_TOKEN (STR_CPU0_VSOC_VALUE),      L"Volts");
          UpdateSenorValue (CPU1_VSOC,      STRING_TOKEN (STR_CPU1_VSOC_VALUE),      L"Volts");
          UpdateSenorValue (CPU0_ABCD_VDDQ, STRING_TOKEN (STR_CPU0_ABCD_VDDQ_VALUE), L"Volts");
          UpdateSenorValue (CPU0_EFGH_VDDQ, STRING_TOKEN (STR_CPU0_EFGH_VDDQ_VALUE), L"Volts");
          UpdateSenorValue (CPU1_IJKL_VDDQ, STRING_TOKEN (STR_CPU1_IJKL_VDDQ_VALUE), L"Volts");
          UpdateSenorValue (CPU1_MNOP_VDDQ, STRING_TOKEN (STR_CPU1_MNOP_VDDQ_VALUE), L"Volts");
          UpdateSenorValue (_5V_AUX,        STRING_TOKEN (STR_5V_AUX_VALUE),         L"Volts");
          UpdateSenorValue (_3V3_AUX,       STRING_TOKEN (STR_3V3_AUX_VALUE),        L"Volts");
          UpdateSenorValue (_1V8_AUX_CPU1,  STRING_TOKEN (STR_18V_AUX_CPU1_VALUE),   L"Volts");
          UpdateSenorValue (_12V,           STRING_TOKEN (STR_12V_VALUE),            L"Volts");
          UpdateSenorValue (_5V,            STRING_TOKEN (STR_5V_VALUE),             L"Volts");
          UpdateSenorValue (_3V3,           STRING_TOKEN (STR_3V3_VALUE),            L"Volts");
          UpdateSenorValue (VBAT,           STRING_TOKEN (STR_VBAT_VALUE),           L"Volts");

          // Watts
          UpdateSenorValue (PSU1_Power_in,  STRING_TOKEN (STR_PSU1_PIN_VALUE),       L"Watts");
          UpdateSenorValue (PSU2_Power_in,  STRING_TOKEN (STR_PSU2_PIN_VALUE),       L"Watts");

          //RPM
          UpdateSenorValue (FAN1_Speed,     STRING_TOKEN (STR_FAN1_SPEED_VALUE),     L"RPM");
          UpdateSenorValue (FAN2_Speed,     STRING_TOKEN (STR_FAN2_SPEED_VALUE),     L"RPM");
          UpdateSenorValue (FAN3_Speed,     STRING_TOKEN (STR_FAN3_SPEED_VALUE),     L"RPM");
          UpdateSenorValue (FAN4_Speed,     STRING_TOKEN (STR_FAN4_SPEED_VALUE),     L"RPM");          
        break;

      default:
        Status = EFI_INVALID_PARAMETER;
        break;
  }
  return Status;
}

