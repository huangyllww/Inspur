/*****************************************************************************
 *
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
 *
 ***************************************************************************/


//This file is auto generated, don't edit it manually

#include <Base.h>
#include <Library/DebugLib.h>
#include "AmdCbsVariable.h"
#include <Library/PcdLib.h>

VOID
CbsSetAgesaPcds (
  IN       VOID *CbsVariable
  )
{
  CBS_CONFIG *Setup_Config;

  if (CbsVariable == NULL) {
    ASSERT (FALSE);
    return;
  }
  Setup_Config = (CBS_CONFIG *) CbsVariable;
  if (Setup_Config->CbsCmnCpuPfeh != 3) {
    PcdSetBool (PcdAmdCcxCfgPFEHEnable, Setup_Config->CbsCmnCpuPfeh);
  }
  if (Setup_Config->CbsCmnCpuCpb != 1) {
    PcdSet8 (PcdAmdCpbMode, Setup_Config->CbsCmnCpuCpb);
  }
  if (Setup_Config->CbsCmnCpuGlobalCstateCtrl != 0) {
    PcdSet8 (PcdAmdCStateMode, Setup_Config->CbsCmnCpuGlobalCstateCtrl);
  }
  if (Setup_Config->CbsCmnCpuGenDowncoreCtrl != 0) {
    PcdSet8 (PcdAmdDownCoreMode, Setup_Config->CbsCmnCpuGenDowncoreCtrl);
  }
  if (Setup_Config->CbsCmnCpuL1StreamHwPrefetcher != 3) {
    PcdSetBool (PcdAmdL1StreamPrefetcher, Setup_Config->CbsCmnCpuL1StreamHwPrefetcher);
  }
  if (Setup_Config->CbsCmnCpuL2StreamHwPrefetcher != 3) {
    PcdSetBool (PcdAmdL2StreamPrefetcher, Setup_Config->CbsCmnCpuL2StreamHwPrefetcher);
  }
  if (Setup_Config->CbsDbgPoisonConsumption != 0xF) {
    PcdSetBool (PcdAmdNbioPoisonConsumption, Setup_Config->CbsDbgPoisonConsumption);
  }
  if (Setup_Config->CbsDbgRASControl != 0xF) {
    PcdSetBool (PcdAmdNbioRASControl, Setup_Config->CbsDbgRASControl);
  }
  if (Setup_Config->CbsCmnDeterminismSlider != 0xF) {
    PcdSetBool (PcdAmdDeterminismControl, Setup_Config->CbsCmnDeterminismSlider);
  }
  if (Setup_Config->CbsCmnNbioEfficiencyOptimizedMode != 0xF) {
    PcdSetBool (PcdEfficiencyOptimizedMode, Setup_Config->CbsCmnNbioEfficiencyOptimizedMode);
  }
  if (Setup_Config->CbsCmnNbioPSIDisable != 0xf) {
    PcdSetBool (PcdAmdPSIDisable, Setup_Config->CbsCmnNbioPSIDisable);
  }
  if (Setup_Config->CbsDbgGnbDbgACSEnable != 0xF) {
    PcdSetBool (PcdCfgACSEnable, Setup_Config->CbsDbgGnbDbgACSEnable);
  }
  if (Setup_Config->CbsGnbDbgPcieAriSupport != 0xf) {
    PcdSetBool (PcdCfgPcieAriSupport, Setup_Config->CbsGnbDbgPcieAriSupport);
  }
  if (Setup_Config->CbsCfgPcieLoopbackMode != 2) {
    PcdSetBool (PcdCfgPcieLoopbackMode, Setup_Config->CbsCfgPcieLoopbackMode);
  }
  PcdSet16 (PcdCfgPcieCrsDelay, Setup_Config->CbsCfgPcieCrsDelay);
  PcdSet16 (PcdCfgPcieCrsLimit, Setup_Config->CbsCfgPcieCrsLimit);
  if (PcdGet8(PcdAmdNbIOMMU) != 0xf) {
    PcdSetBool (PcdCfgIommuSupport, PcdGet8(PcdAmdNbIOMMU));
  }
  if (Setup_Config->CbsPcieSyncReset != 0xf) {
    PcdSetBool (PcdPcieSyncReset, Setup_Config->CbsPcieSyncReset);
  }
  if (Setup_Config->CbsCmnDisableSideband != 0xF) {
    PcdSetBool (PcdAmdDisableSideband, Setup_Config->CbsCmnDisableSideband);
  }
  if (Setup_Config->CbsCmnDisableL1wa != 0xF) {
    PcdSetBool (PcdAmdDisableL1wa, Setup_Config->CbsCmnDisableL1wa);
  }
  if (Setup_Config->CbsCmnDisableBridgeDis != 0xF) {
    PcdSetBool (PcdAmdDisableBridgeDis, Setup_Config->CbsCmnDisableBridgeDis);
  }
  if (Setup_Config->CbsCmnDisableIrqPoll != 0xF) {
    PcdSetBool (PcdAmdDisableIrqPoll, Setup_Config->CbsCmnDisableIrqPoll);
  }
  if (Setup_Config->CbsCmnIrqSetsBridgeDis != 0xF) {
    PcdSetBool (PcdAmdIrqSetBridgeDis, Setup_Config->CbsCmnIrqSetsBridgeDis);
  }
  if (Setup_Config->CbsPcieHotplugSupport != 0xF) {
    PcdSetBool (PcdCfgPcieHotplugSupport, Setup_Config->CbsPcieHotplugSupport);
  }
  if (Setup_Config->CbsCmnFchSataEnable != 0xf) {
    PcdSetBool (PcdSataEnable, Setup_Config->CbsCmnFchSataEnable);
  }
  if (Setup_Config->CbsCmnFchSataClass != 0xf) {
    PcdSet8 (PcdSataClass, Setup_Config->CbsCmnFchSataClass);
  }
  if (Setup_Config->CbsCmnFchSataRasSupport != 0xf) {
    PcdSetBool (PcdSataRasSupport, Setup_Config->CbsCmnFchSataRasSupport);
  }
  if (Setup_Config->CbsCmnFchSataAhciDisPrefetchFunction != 0xf) {
    PcdSetBool (PcdSataAhciDisPrefetchFunction, Setup_Config->CbsCmnFchSataAhciDisPrefetchFunction);
  }
  if (Setup_Config->CbsDbgFchSataAggresiveDevSlpP0 != 0xF) {
    PcdSetBool (PcdSataDevSlpPort0, Setup_Config->CbsDbgFchSataAggresiveDevSlpP0);
  }
  PcdSet8 (PcdSataDevSlpPort0Num, Setup_Config->CbsDbgFchSataDevSlpPort0Num);
  if (Setup_Config->CbsDbgFchSataAggresiveDevSlpP1 != 0xF) {
    PcdSetBool (PcdSataDevSlpPort1, Setup_Config->CbsDbgFchSataAggresiveDevSlpP1);
  }
  PcdSet8 (PcdSataDevSlpPort1Num, Setup_Config->CbsDbgFchSataDevSlpPort1Num);

// wz200322 - >>
/*
  if (Setup_Config->CbsCmnFchUsbXHCI0Enable != 0xf) {
    PcdSetBool (PcdXhci0Enable, Setup_Config->CbsCmnFchUsbXHCI0Enable);
  }
  if (Setup_Config->CbsCmnFchUsbXHCI1Enable != 0xf) {
    PcdSetBool (PcdXhci1Enable, Setup_Config->CbsCmnFchUsbXHCI1Enable);
  }
  if (Setup_Config->CbsCmnFchUsbXHCI2Enable != 0xf) {
    PcdSetBool (PcdXhci2Enable, Setup_Config->CbsCmnFchUsbXHCI2Enable);
  }
  if (Setup_Config->CbsCmnFchUsbXHCI3Enable != 0xf) {
    PcdSetBool (PcdXhci3Enable, Setup_Config->CbsCmnFchUsbXHCI3Enable);
  }
*/
// wz200322 - <<

  PcdSet8 (PcdSdConfig, Setup_Config->CbsCmnFchSdConfig);

// wz200227 - >>
// platform item will set it.  
//PcdSet8 (PcdPwrFailShadow, Setup_Config->CbsCmnFchSystemPwrFailShadow);
// wz200227 - <<

//PcdSet8 (FchUart0LegacyEnable, Setup_Config->CbsCmnFchUart0LegacyConfig);     // wz200313 -
  PcdSet8 (FchUart1LegacyEnable, Setup_Config->CbsCmnFchUart1LegacyConfig);
  PcdSet8 (FchUart2LegacyEnable, Setup_Config->CbsCmnFchUart2LegacyConfig);
  PcdSet8 (FchUart3LegacyEnable, Setup_Config->CbsCmnFchUart3LegacyConfig);
  if (Setup_Config->CbsCmnPTSataPortEnable != 0xf) {
    PcdSet8 (PcdPTSataPortEnable, Setup_Config->CbsCmnPTSataPortEnable);
  }
  if (Setup_Config->CbsCmnPTSataClass != 0xf) {
    PcdSet8 (PcdPTSataMode, Setup_Config->CbsCmnPTSataClass);
  }
  if (Setup_Config->CbsDbgPTSataAggresiveDevSlpP0 != 0xF) {
    PcdSet8 (PcdPTSataAggresiveDevSlpP0, Setup_Config->CbsDbgPTSataAggresiveDevSlpP0);
  }
  if (Setup_Config->CbsDbgPTSataAggresiveDevSlpP1 != 0xF) {
    PcdSet8 (PcdPTSataAggresiveDevSlpP1, Setup_Config->CbsDbgPTSataAggresiveDevSlpP1);
  }
  if (Setup_Config->CbsDbgPTXhciGen1 != 0xF) {
    PcdSet8 (PcdPTXhciGen1, Setup_Config->CbsDbgPTXhciGen1);
  }
  if (Setup_Config->CbsDbgPTXhciGen2 != 0xF) {
    PcdSet8 (PcdPTXhciGen2, Setup_Config->CbsDbgPTXhciGen2);
  }
  if (Setup_Config->CbsDbgPTUsbEqualization4 != 0xF) {
    PcdSet8 (PcdPTUsbEqualization4, Setup_Config->CbsDbgPTUsbEqualization4);
  }
  if (Setup_Config->CbsDbgPTUsbRedriver != 0xF) {
    PcdSet8 (PcdPTUsbRedriver, Setup_Config->CbsDbgPTUsbRedriver);
  }
  if (Setup_Config->CbsDbgPTUsb31P0 != 0xF) {
    PcdSet8 (PcdPTUsb31P0, Setup_Config->CbsDbgPTUsb31P0);
  }
  if (Setup_Config->CbsDbgPTUsb31P1 != 0xF) {
    PcdSet8 (PcdPTUsb31P1, Setup_Config->CbsDbgPTUsb31P1);
  }
  if (Setup_Config->CbsDbgPTUsb30P0 != 0xF) {
    PcdSet8 (PcdPTUsb30P0, Setup_Config->CbsDbgPTUsb30P0);
  }
  if (Setup_Config->CbsDbgPTUsb30P1 != 0xF) {
    PcdSet8 (PcdPTUsb30P1, Setup_Config->CbsDbgPTUsb30P1);
  }
  if (Setup_Config->CbsDbgPTUsb30P2 != 0xF) {
    PcdSet8 (PcdPTUsb30P2, Setup_Config->CbsDbgPTUsb30P2);
  }
  if (Setup_Config->CbsDbgPTUsb30P3 != 0xF) {
    PcdSet8 (PcdPTUsb30P3, Setup_Config->CbsDbgPTUsb30P3);
  }
  if (Setup_Config->CbsDbgPTUsb30P4 != 0xF) {
    PcdSet8 (PcdPTUsb30P4, Setup_Config->CbsDbgPTUsb30P4);
  }
  if (Setup_Config->CbsDbgPTUsb30P5 != 0xF) {
    PcdSet8 (PcdPTUsb30P5, Setup_Config->CbsDbgPTUsb30P5);
  }
  if (Setup_Config->CbsDbgPTUsb20P0 != 0xF) {
    PcdSet8 (PcdPTUsb20P0, Setup_Config->CbsDbgPTUsb20P0);
  }
  if (Setup_Config->CbsDbgPTUsb20P1 != 0xF) {
    PcdSet8 (PcdPTUsb20P1, Setup_Config->CbsDbgPTUsb20P1);
  }
  if (Setup_Config->CbsDbgPTUsb20P2 != 0xF) {
    PcdSet8 (PcdPTUsb20P2, Setup_Config->CbsDbgPTUsb20P2);
  }
  if (Setup_Config->CbsDbgPTUsb20P3 != 0xF) {
    PcdSet8 (PcdPTUsb20P3, Setup_Config->CbsDbgPTUsb20P3);
  }
  if (Setup_Config->CbsDbgPTUsb20P4 != 0xF) {
    PcdSet8 (PcdPTUsb20P4, Setup_Config->CbsDbgPTUsb20P4);
  }
  if (Setup_Config->CbsDbgPTUsb20P5 != 0xF) {
    PcdSet8 (PcdPTUsb20P5, Setup_Config->CbsDbgPTUsb20P5);
  }
  if (Setup_Config->CbsDbgPTProm2Usb31P0 != 0xF) {
    PcdSet8 (PcdPTProm2Usb31P0, Setup_Config->CbsDbgPTProm2Usb31P0);
  }
  if (Setup_Config->CbsDbgPTProm2Usb31P1 != 0xF) {
    PcdSet8 (PcdPTProm2Usb31P1, Setup_Config->CbsDbgPTProm2Usb31P1);
  }
  if (Setup_Config->CbsDbgPTProm2Usb30P0 != 0xF) {
    PcdSet8 (PcdPTProm2Usb30P0, Setup_Config->CbsDbgPTProm2Usb30P0);
  }
  if (Setup_Config->CbsDbgPTProm2Usb30P1 != 0xF) {
    PcdSet8 (PcdPTProm2Usb30P1, Setup_Config->CbsDbgPTProm2Usb30P1);
  }
  if (Setup_Config->CbsDbgPTProm2Usb20P0 != 0xF) {
    PcdSet8 (PcdPTProm2Usb20P0, Setup_Config->CbsDbgPTProm2Usb20P0);
  }
  if (Setup_Config->CbsDbgPTProm2Usb20P1 != 0xF) {
    PcdSet8 (PcdPTProm2Usb20P1, Setup_Config->CbsDbgPTProm2Usb20P1);
  }
  if (Setup_Config->CbsDbgPTProm2Usb20P2 != 0xF) {
    PcdSet8 (PcdPTProm2Usb20P2, Setup_Config->CbsDbgPTProm2Usb20P2);
  }
  if (Setup_Config->CbsDbgPTProm2Usb20P3 != 0xF) {
    PcdSet8 (PcdPTProm2Usb20P3, Setup_Config->CbsDbgPTProm2Usb20P3);
  }
  if (Setup_Config->CbsDbgPTProm2Usb20P4 != 0xF) {
    PcdSet8 (PcdPTProm2Usb20P4, Setup_Config->CbsDbgPTProm2Usb20P4);
  }
  if (Setup_Config->CbsDbgPTProm2Usb20P5 != 0xF) {
    PcdSet8 (PcdPTProm2Usb20P5, Setup_Config->CbsDbgPTProm2Usb20P5);
  }
  if (Setup_Config->CbsDbgPTProm1Usb31P0 != 0xF) {
    PcdSet8 (PcdPTProm1Usb31P0, Setup_Config->CbsDbgPTProm1Usb31P0);
  }
  if (Setup_Config->CbsDbgPTProm1Usb31P1 != 0xF) {
    PcdSet8 (PcdPTProm1Usb31P1, Setup_Config->CbsDbgPTProm1Usb31P1);
  }
  if (Setup_Config->CbsDbgPTProm1Usb30P0 != 0xF) {
    PcdSet8 (PcdPTProm1Usb30P0, Setup_Config->CbsDbgPTProm1Usb30P0);
  }
  if (Setup_Config->CbsDbgPTProm1Usb20P0 != 0xF) {
    PcdSet8 (PcdPTProm1Usb20P0, Setup_Config->CbsDbgPTProm1Usb20P0);
  }
  if (Setup_Config->CbsDbgPTProm1Usb20P1 != 0xF) {
    PcdSet8 (PcdPTProm1Usb20P1, Setup_Config->CbsDbgPTProm1Usb20P1);
  }
  if (Setup_Config->CbsDbgPTProm1Usb20P2 != 0xF) {
    PcdSet8 (PcdPTProm1Usb20P2, Setup_Config->CbsDbgPTProm1Usb20P2);
  }
  if (Setup_Config->CbsDbgPTProm1Usb20P3 != 0xF) {
    PcdSet8 (PcdPTProm1Usb20P3, Setup_Config->CbsDbgPTProm1Usb20P3);
  }
  if (Setup_Config->CbsDbgPTProm1Usb20P4 != 0xF) {
    PcdSet8 (PcdPTProm1Usb20P4, Setup_Config->CbsDbgPTProm1Usb20P4);
  }
  if (Setup_Config->CbsDbgPTProm1Usb20P5 != 0xF) {
    PcdSet8 (PcdPTProm1Usb20P5, Setup_Config->CbsDbgPTProm1Usb20P5);
  }
  if (Setup_Config->CbsDbgSP3NtbEnable != 0) {
    PcdSetBool (PcdCfgNTBEnable, Setup_Config->CbsDbgSP3NtbEnable);
  }
  if (Setup_Config->CbsDbgSP3NtbLocation != 0xFF) {
    PcdSet8 (PcdCfgNTBLocation, Setup_Config->CbsDbgSP3NtbLocation);
  }
  if (Setup_Config->CbsDbgSP3NtbPCIeCore != 0xf) {
    PcdSet8 (PcdCfgNTBPcieCoreSel, Setup_Config->CbsDbgSP3NtbPCIeCore);
  }
  if (Setup_Config->CbsDbgSP3NtbMode != 0xf) {
    PcdSet8 (PcdCfgNTBMode, Setup_Config->CbsDbgSP3NtbMode);
  }
  if (Setup_Config->CbsDbgSP3NtbLinkSpeed != 0xf) {
    PcdSet8 (PcdCfgNTBLinkSpeed, Setup_Config->CbsDbgSP3NtbLinkSpeed);
  }
  if (Setup_Config->CbsDbgSP3NtbBAR1Window != 0xff) {
    PcdSet8 (PcdCfgNTBBAR1Size, Setup_Config->CbsDbgSP3NtbBAR1Window);
  }
  if (Setup_Config->CbsDbgSP3NtbBAR23Window != 0xff) {
    PcdSet8 (PcdCfgNTBBAR23Size, Setup_Config->CbsDbgSP3NtbBAR23Window);
  } 
  if (Setup_Config->CbsDbgSP3NtbBAR45Window != 0xff) {
    PcdSet8 (PcdCfgNTBBAR45Size, Setup_Config->CbsDbgSP3NtbBAR45Window);
  } 
}
