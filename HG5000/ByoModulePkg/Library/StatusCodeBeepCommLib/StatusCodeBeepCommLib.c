/** @file
==========================================================================================
      NOTICE: Copyright (c) 2006 - 2018 Byosoft Corporation. All rights reserved.
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
#include <ByoStatusCode.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>



STATIC STATUS_CODE_TO_DATA_MAP gProgressBeepMap[] = {
  { PEI_RECOVERY_AUTO,    2 },
  { DXE_CON_OUT_CONNECT,  1 },

  {0,0}
};

STATIC STATUS_CODE_TO_DATA_MAP gErrorBeepMap[] = {
  { PEI_MEMORY_NOT_DETECTED, 3 }, 
  { DXE_NO_CON_OUT,          4 },
//{ DXE_NO_CON_IN,           2 },

  {0,0}
};


typedef struct {
  STATUS_CODE_TO_DATA_MAP  *Map;
  BEEP_VOICE_CFG           Voice;
} BEEP_MAP_VOICE;


STATIC BEEP_MAP_VOICE gBeepMapList[] = {
  {gProgressBeepMap, {5, 2, 100000}},
  {gErrorBeepMap,    {1, 2, 400000}},
};


#define BEEP_MAP_LIST_COUNT   (sizeof(gBeepMapList) / sizeof(gBeepMapList[0]))


#define NOTE(x) ((119318200 + (x) / 2) / (x))

/**
  Switch on Beep.

  @param  Note         The note of beep.
  @param  Octave       The octave of beep.

  @retval None

**/
STATIC
VOID
BeepOn (
  UINT8 Note,
  UINT8 Octave
  )
{
  UINT16  Frequency;

  //
  // beep tones
  //
  UINT16  tones[8] = {
    NOTE (26163),
    NOTE (29366),
    NOTE (32963),
    NOTE (34923),
    NOTE (39200),
    NOTE (44000),
    NOTE (49388),
    NOTE (26163 * 2)
  };

  Frequency = tones[(Note % 8)];

  if (Octave - 1 >= 0) {
    Frequency >>= Octave - 1;
  } else {
    Frequency <<= 1 - Octave;
  }
  //
  // set up channel 1 (used for delays)
  //
  IoWrite8 (0x43, 0x54);
  IoWrite8 (0x41, 0x12);
  //
  // set up channel 2 (used by speaker)
  //
  IoWrite8 (0x43, 0xb6);
  IoWrite8 (0x42, (UINT8) Frequency);
  IoWrite8 (0x42, (UINT8) (Frequency >> 8));
  //
  // turn the speaker on
  //
  IoWrite8 (0x61, IoRead8 (0x61) | 3);
}

/**
  Switch off Beep.

  @retval None

**/
STATIC
VOID
BeepOff (
  VOID
  )
{
  IoWrite8 (0x61, IoRead8 (0x61) & 0xfc);
}

/**
  Produces Beep.

  @param  Note         The note of beep.
  @param  Octave       The octave of beep.
  @param  Duration     The duration of beep.

  @retval None

**/
STATIC
VOID
Beep (
  UINT8            Note,
  UINT8            Octave,
  UINT32           Duration
  )
{
  BeepOn (Note, Octave);
  MicroSecondDelay (Duration);
  BeepOff ();
}




STATIC
UINT32
FindBeepByteCode (
  STATUS_CODE_TO_DATA_MAP *Map, 
  EFI_STATUS_CODE_VALUE   Value
  )
{
  while ( Map->Value != 0 ) {
    if ( Map->Value == Value ) {
      return Map->Data;
    }
    Map++;
  }
  return 0;
}


EFI_STATUS
EFIAPI
LibDoStatusCodeBeep (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value
  )
{
  UINT32          CodeTypeIndex;
  UINT8           Index;
  UINT32          BeepTimes;
  BEEP_VOICE_CFG  *BeepCfg;


//EFI_PROGRESS_CODE   0x00000001
//EFI_ERROR_CODE      0x00000002
  CodeTypeIndex = STATUS_CODE_TYPE (CodeType) - 1;

  if (CodeTypeIndex >= BEEP_MAP_LIST_COUNT) {
    return EFI_SUCCESS;
  }

  BeepTimes = FindBeepByteCode(gBeepMapList[CodeTypeIndex].Map, Value);
  if (BeepTimes > 0) {
    BeepCfg = &gBeepMapList[CodeTypeIndex].Voice;
    for (Index = 0; Index < BeepTimes; Index++) {
      Beep (BeepCfg->Note, BeepCfg->Octave, BeepCfg->Duration);
      MicroSecondDelay (100000);
    }

// patch.    
    if (Value == PEI_MEMORY_NOT_DETECTED) {
      Beep (BeepCfg->Note, BeepCfg->Octave, BeepCfg->Duration * 2);
      MicroSecondDelay (100000);
    }
  }

  return EFI_SUCCESS;
}


