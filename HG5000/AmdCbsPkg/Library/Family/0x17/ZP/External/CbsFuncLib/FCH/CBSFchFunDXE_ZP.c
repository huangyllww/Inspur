/*****************************************************************************
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */
#include <Library/DebugLib.h>
#include "CBSFchFunDXE_ZP.h"

#ifdef AMD_CBS_FCH_ZP
DXE_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(FchDxeFunTable,_ZP)[] =
{
  {
    F17_ZP_RAW_ID,
    AMD_RESERVE_ALL,
    CBS_FUNC_DECLARE(FchInitEnvParams,_ZP),
    IDS_HOOK_FCH_INIT_ENV
  },
  
  {
    F18_DN_RAW_ID,
    AMD_RESERVE_ALL,
    CBS_FUNC_DECLARE(FchInitEnvParams,_ZP),
    IDS_HOOK_FCH_INIT_ENV
  },

  {
    0,        // F17_ZP_RAW_ID,
    0,        // AMD_RESERVE_ALL,
    NULL,     // CBS_FUNC_DECLARE(FchFunc_Init_DXE,_ZP),
    0         // CBS_CALLOUT_ID
  }
};

VOID
CBS_FUNC_DECLARE(FchInitEnvParams,_ZP) (
  IN EFI_BOOT_SERVICES        *BootServices,
  IN OUT  VOID                *DataParams,
  IN AMD_SYSTEM_CONFIGURATION *SystemConfiguration
  )
{
  UINT32                    PcdSata32;
  CBS_IDS_HOOK_PARAMS       *CbsIdsHookParams;
  FCH_DATA_BLOCK            *FchParams;

  CbsIdsHookParams = (CBS_IDS_HOOK_PARAMS *)DataParams;
  FchParams = (FCH_DATA_BLOCK *)(CbsIdsHookParams->Data);

  // I2C, Uart, ESPI
  PcdSata32 = PcdGet32 (FchRTDeviceEnableMap);

  if (SystemConfiguration->CbsCmnFchI2C0Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchI2C0Config) {
	  PcdSata32 |= BIT5;
	} else {
	  PcdSata32 &= ~ BIT5;
	}
  }
  if (SystemConfiguration->CbsCmnFchI2C1Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchI2C1Config) {
	  PcdSata32 |= BIT6;
	} else {
	  PcdSata32 &= ~ BIT6;
	}
  }
  if (SystemConfiguration->CbsCmnFchI2C2Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchI2C2Config) {
	  PcdSata32 |= BIT7;
	} else {
	  PcdSata32 &= ~ BIT7;
	}
  }
  if (SystemConfiguration->CbsCmnFchI2C3Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchI2C3Config) {
	  PcdSata32 |= BIT8;
	} else {
	  PcdSata32 &= ~ BIT8;
	}
  }
  if (SystemConfiguration->CbsCmnFchI2C4Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchI2C4Config) {
	  PcdSata32 |= BIT9;
	} else {
	  PcdSata32 &= ~ BIT9;
	}
  }
  if (SystemConfiguration->CbsCmnFchI2C5Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchI2C5Config) {
	  PcdSata32 |= BIT10;
	} else {
	  PcdSata32 &= ~ BIT10;
	}
  }
  if (SystemConfiguration->CbsCmnFchUart0Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchUart0Config) {
	  PcdSata32 |= BIT11;
	} else {
	  PcdSata32 &= ~ BIT11;
	}
  }
  if (SystemConfiguration->CbsCmnFchUart1Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchUart1Config) {
	  PcdSata32 |= BIT12;
	} else {
	  PcdSata32 &= ~ BIT12;
	}
  }
  if (SystemConfiguration->CbsCmnFchUart2Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchUart2Config) {
	  PcdSata32 |= BIT16;
	} else {
	  PcdSata32 &= ~ BIT16;
	}
  }
  if (SystemConfiguration->CbsCmnFchUart3Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchUart3Config) {
	  PcdSata32 |= BIT26;
	} else {
	  PcdSata32 &= ~ BIT26;
	}
  }

  if (SystemConfiguration->CbsCmnFchEspiConfig != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchEspiConfig) {
	  PcdSata32 |= BIT27;
	} else {
	  PcdSata32 &= ~ BIT27;
	}
  }
  PcdSet32 (FchRTDeviceEnableMap, PcdSata32);
}
#endif
