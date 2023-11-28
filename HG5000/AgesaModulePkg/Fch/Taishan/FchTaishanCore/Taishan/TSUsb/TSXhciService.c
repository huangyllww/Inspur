/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Xhci controller
 *
 * Init Xhci Controller features (PEI phase).
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
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
****************************************************************************
*/
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include "FchPlatform.h"
#include "Filecode.h"

#define FILECODE FCH_TAISHAN_FCHTAISHANCORE_TAISHAN_TSUSB_TSXHCISERVICE_FILECODE

#define MAX_RETRY_NUM   1000
/**
 * FchTSXhciProgramInternalRegStepOne - Program USB internal
 * registers Step One
 *
 *
 * @param[in] DieBusNum  IOCH bus number on current Die.
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciProgramInternalRegStepOne (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT8                        RegValue;
  FCH_RESET_DATA_BLOCK        *LocalCfgPtr;
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

//6. Program phy configuration, phy lane parameter and container registers
// Program USB3 PHY0 PHYCFG0(address=0x16D08200) to 0x00000039
// Program USB3 PHY1 PHYCFG0(address=0x16D08600) to 0x00000039
// Program USB3 PHY2 PHYCFG0(address=0x16D08A00) to 0x00000039
// Program USB3 PHY3 PHYCFG0(address=0x16D08E00) to 0x00000039
// Program USB3 PHY0 LANEPARACTL0(address=0x16D0C000) to 0x01B3C153
// Program USB3 PHY1 LANEPARACTL0(address=0x16D0C400) to 0x01B3C153
// Program USB3 PHY2 LANEPARACTL0(address=0x16D0C800) to 0x01B3C153
// Program USB3 PHY3 LANEPARACTL0(address=0x16D0CC00) to 0x01B3C153
// Program Interrupt control (address=0x16D8_0118) to 0x00001110 to enable SMI

  FchSmnRW (DieBusNum, FCH_TS_USB_PHY0_PHYCFG0, 0x00, 0x00000039, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY1_PHYCFG0, 0x00, 0x00000039, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY2_PHYCFG0, 0x00, 0x00000039, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY3_PHYCFG0, 0x00, 0x00000039, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY0_LANEPARACTL0, 0x00, LocalCfgPtr->XhciLaneParaCtl0, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY1_LANEPARACTL0, 0x00, LocalCfgPtr->XhciLaneParaCtl0, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY2_LANEPARACTL0, 0x00, LocalCfgPtr->XhciLaneParaCtl0, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY3_LANEPARACTL0, 0x00, LocalCfgPtr->XhciLaneParaCtl0, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY0PARACTL2, ~(UINT32) (0x07 << 7), (0x2 << 7), LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY1PARACTL2, ~(UINT32) (0x07 << 7), (0x2 << 7), LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY2PARACTL2, ~(UINT32) (0x07 << 7), (0x2 << 7), LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY3PARACTL2, ~(UINT32) (0x07 << 7), (0x2 << 7), LocalCfgPtr->StdHeader);

  RegValue = LocalCfgPtr->XhciECCDedErrRptEn;
  if (RegValue) {
    FchSmnRW (DieBusNum, FCH_TS_USB_INTERRUPT_CONTROL, 0xffffefff, BIT12, LocalCfgPtr->StdHeader);
  }else {
    FchSmnRW (DieBusNum, FCH_TS_USB_INTERRUPT_CONTROL, 0xffffefff, 0, LocalCfgPtr->StdHeader);
  }
}

/**
 * FchTSXhciProgramInternalRegStepTwo - Program USB internal
 * registers Step Two
 *
 *
 * @param[in] DieBusNum  IOCH bus number on current Die.
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciProgramInternalRegStepTwo (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK        *LocalCfgPtr;
  UINT32 Value32;
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

// 11. Program Controller internal registers
//        Program GUCTL1(address=0x16C0C11C) to 0x2004010A
//        Program GUCTL(address=0x16C0C12C) to 0x0A43487D  //PLAT-11336 clearing GUCTL[15].CMdevAddr to 0
//        Program GUSB3PIPECTL0(address=0x16C0C2C0) to 0x010A0002
//        Program GUSB3PIPECTL1(address=0x16C0C2C4) to 0x010A0002
//        Program GUSB3PIPECTL2(address=0x16C0C2C8) to 0x010A0002
//        Program GUSB3PIPECTL3(address=0x16C0C2CC) to 0x010A0002

  FchSmnRW (DieBusNum, FCH_TS_USB_GUCTL1, 0x00, 0x2004010A, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_GUCTL, 0x00, 0x0A43487D, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_GUSB3PIPECTL0, 0x00, 0x010A0002, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_GUSB3PIPECTL1, 0x00, 0x010A0002, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_GUSB3PIPECTL2, 0x00, 0x010A0002, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_GUSB3PIPECTL3, 0x00, 0x010A0002, LocalCfgPtr->StdHeader);
  
  FchSmnRead (DieBusNum, FCH_TS_USB_GSNPSID, &Value32, NULL);
  if (Value32 == GSNPSID_REVISION_320A) {
      FchSmnRW (DieBusNum, FCH_TS_USB_GUCTL2, 0xFFFFEFFF, GUCTL2_ENABLE_EP_CACHE_EVICT, LocalCfgPtr->StdHeader);
  }
  
//  FchSmnRW (DieBusNum, FCH_TS_USB_INTERRUPT_CONTROL, 0x00, 0x00001110, LocalCfgPtr->StdHeader);

  //PHYLANEx0018(RX_OVRD_IN_HI)
  //  RX_EQ_EN = 0
  //  RX_EQ_EN_OVRD = 1
  //  RX_EQ.= 3
  //  RX_EQ_OVRD = 1
  Value32 = 0x00000880 | (3 << 8);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY0_RX_OVRD_IN_HI, 0xFFFFF03F, Value32, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY1_RX_OVRD_IN_HI, 0xFFFFF03F, Value32, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY2_RX_OVRD_IN_HI, 0xFFFFF03F, Value32, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY3_RX_OVRD_IN_HI, 0xFFFFF03F, Value32, LocalCfgPtr->StdHeader);

  //Program USB3 PHY0 PHYINTERNALLANEREGISTER ( address = 0x16 D0_4098 ) to 0x0000_0001
  //Program USB3 PHY1 PHYINTERNALLANEREGISTER ( address = 0x16 D0_4498 ) to 0x0000_0001
  //Program USB3 PHY2 PHYINTERNALLANEREGISTER ( address = 0x16 D0_4898 ) to 0x0000_0001
  //Program USB3 PHY3 PHYINTERNALLANEREGISTER ( address = 0x16 D0_4C98 ) to 0x0000_0001
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY0_PHYINTERNALLANEREGISTER, 0x00, 0x00000001, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY1_PHYINTERNALLANEREGISTER, 0x00, 0x00000001, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY2_PHYINTERNALLANEREGISTER, 0x00, 0x00000001, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, FCH_TS_USB_PHY3_PHYINTERNALLANEREGISTER, 0x00, 0x00000001, LocalCfgPtr->StdHeader);
}

UINTN
BitWeight (
  IN    VOID                        *Buffer,
  IN    UINTN                       Size
  )
{
  UINT8                     *Bytes;
  UINTN                     Weight;
  UINTN                     ByteIndex;
  UINTN                     BitIndex;

  Bytes = (UINT8 *)Buffer;
  Weight = 0;
  for (ByteIndex = 0; ByteIndex < Size; ByteIndex++) {
      for (BitIndex = 0; BitIndex < 8; BitIndex++) {
          if (Bytes[ByteIndex] & (1 << BitIndex)) {
              Weight++;
          }
      }
  }
  return Weight;
}

/**
 * FchTSXhciPlatformPortDisable - Platform/Customer Based Host
 * configuration
 *
 *
 * @param[in] DieBusNum  IOCH bus number on current Die.
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciPlatformPortDisable (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT8                       PortNum;
  UINT8                       Usb3PortNum;
  UINT8                       Usb2PortNum;
  UINT32                      UsbPortDisable;
  FCH_RESET_DATA_BLOCK        *LocalCfgPtr;
  UINT8                       *UsbDisableMap;
  UINTN                       UsbDisableMapCount;
  UINT32                      Usb3PortDisable;
  UINT32                      Usb2PortDisable;
  UINT8                       NodeIndex;
  UINT8                       PkgType;
  UINT32                      RegEbx;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;
  
  UsbDisableMap = PcdGetPtr(PcdXhciPlatformPortDisableMap);
  UsbDisableMapCount = PcdGetSize(PcdXhciPlatformPortDisableMap);
  
  //Get SOC package type
  AsmCpuid (0x80000001, NULL, &RegEbx, NULL, NULL);
  PkgType = (UINT8)(RegEbx >> 28);

  for (NodeIndex = 0; NodeIndex < UsbDisableMapCount; NodeIndex++) {
    if (DieBusNum == ReadSocDieBusNum(NodeIndex)) {
      Usb2PortDisable = (UINT32)(UsbDisableMap[NodeIndex] & 0xF);
      Usb3PortDisable = (UINT32)((UsbDisableMap[NodeIndex] & 0xF0) >> 4);
      
      if (PkgType == PKG_SP3) {
          //BIOS must disable 2.0 Port 2/3 and 3.0 port 2/3 for SP3 Package
          //It only has two 2.0 ports and two 3.0 Ports
          Usb2PortDisable |= (BIT2 + BIT3);
          Usb3PortDisable |= (BIT2 + BIT3);
      }
      UsbPortDisable = (Usb3PortDisable << 16) | Usb2PortDisable;
      
      Usb2PortNum = 4 - (UINT8)BitWeight(&Usb2PortDisable, sizeof(Usb2PortDisable));
      Usb3PortNum = 4 - (UINT8)BitWeight(&Usb3PortDisable, sizeof(Usb3PortDisable));
      PortNum = (Usb3PortNum << 4) | Usb2PortNum;
      
      FchSmnRW (DieBusNum, FCH_TS_USB_PORT_CONTROL, 0x00, (UINT32)PortNum, LocalCfgPtr->StdHeader);
      FchSmnRW (DieBusNum, FCH_TS_USB_PORT_DISABLE0, 0x00, UsbPortDisable, LocalCfgPtr->StdHeader);
    }
  }
}

/**
 * FchTSXhciPlatformOverCurrentMap - Platform/Customer Based
 * Host configuration
 *
 *
 * @param[in] DieBusNum  IOCH bus number on current Die.
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciPlatformOverCurrentMap (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT8                       DieNum;
  UINT8                       PortNum;
  UINT8                       OCPin;
  UINT8                       *mDieNumUsbPortOCPinArrayPcd = NULL;
  
  FCH_RESET_DATA_BLOCK        *LocalCfgPtr;
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;
  
  //Set OC Pin is low when OC occurs.
  FchSmnRW (DieBusNum, FCH_TS_USB_PORT_CONTROL, (UINT32)~BIT8, (UINT32)BIT8, LocalCfgPtr->StdHeader);

  //Get DieNum, USB Port, OCPin Array by Pcd
  mDieNumUsbPortOCPinArrayPcd = PcdGetPtr (PcdDieNumUsbPortAndOCPinMap);
  
  while (*mDieNumUsbPortOCPinArrayPcd != 0xFF) {
      //Get Die Num
      DieNum = (*mDieNumUsbPortOCPinArrayPcd >> 4) & 0x0F;
      if (DieBusNum == ReadSocDieBusNum(DieNum)) {//Match Die and DieBusNum
          // Get USB Port
          PortNum = *mDieNumUsbPortOCPinArrayPcd & 0x0F; 
          // Get OC Pin
          OCPin = *(++mDieNumUsbPortOCPinArrayPcd);
          FchTSXhciOCmapping (DieBusNum, PortNum, OCPin);
          
          mDieNumUsbPortOCPinArrayPcd++;
      } else {
          mDieNumUsbPortOCPinArrayPcd = mDieNumUsbPortOCPinArrayPcd + 2;
      }
  }
}

/**
 * FchTSXhciPlatformConfiguration - Platform/Customer Based Host
 * configuration
 *
 *
 * @param[in] DieBusNum  IOCH bus number on current Die.
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciPlatformConfiguration (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK        *LocalCfgPtr;
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  FchTSXhciPlatformPortDisable (DieBusNum, FchDataPtr);

  FchTSXhciPlatformOverCurrentMap (DieBusNum, FchDataPtr);
}

/**
 * FchTSXhciInitBootProgram - Config Xhci controller during
 * Power-On
 *
 *
 * @param[in] DieBusNum  IOCH bus number on current Die.
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciInitBootProgram (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT32                      Retry;
  UINT32                      CurrentPowerState;
  FCH_RESET_DATA_BLOCK        *LocalCfgPtr;
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  //1.  Wait until USB rsmu_hard_resetb is cleared by SMU FW
  //FCH_DEADLOOP();
  //1. Program D0
  //1.1.  Write PMCSR.PowerState in USB PCIe header to D0 (2'b00)
  FchSmnRW (DieBusNum, FCH_TS_USB_SHADOWED_POWERSTATE, 0x00, 0x00, LocalCfgPtr->StdHeader);
  //1.2.  Wait until Current_power_state is read as D0 (read as 2'b00)
  for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
    FchSmnRead (DieBusNum, FCH_TS_USB_CURRENT_PWR_STS, &CurrentPowerState, LocalCfgPtr->StdHeader);
    if ((CurrentPowerState & (BIT8 + BIT9)) == 0) {
      break;
    }
    FchStall (1, LocalCfgPtr->StdHeader);
  }
  // ASSERT (Retry < MAX_RETRY_NUM);
  //2.  Program USB_S5_RESET = 1
  FchSmnRW (DieBusNum, FCH_TS_USB_RESET, ~ ((UINT32) FCH_TS_USB_S5_RESET), FCH_TS_USB_S5_RESET, LocalCfgPtr->StdHeader);
  //3.  Program USB_SOFT_CONTROLLER_RESET = 1, USB_SOFT_PHY_RESET = 1
  FchSmnRW (DieBusNum, FCH_TS_USB_RESET, ~ ((UINT32) (FCH_TS_USB_CONTROLLER_RESET + FCH_TS_USB_PHY_RESET)), (FCH_TS_USB_CONTROLLER_RESET + FCH_TS_USB_PHY_RESET), LocalCfgPtr->StdHeader);
  //4.  Wait at least 100us
  FchStall (100, LocalCfgPtr->StdHeader);
// 5.   Platform/Customer based host configuration: If there's needed to program USB container register to change USB features or mode, do in this step. Following are some examples:
// a.   To change USB port numbers, program Port Control (0x16D8_0130), and Port Disable Write Once (0x16D8_0128) and/or Port Disable Read Write (0x16D8_012C)
// b.   To program over current mapping, program Overcurrent Map register (0x16D8_0120 or 0x16D8_0120)
// c.   To enable SMI/NMI (0x16D8_0118)
  FchTSXhciPlatformConfiguration (DieBusNum, FchDataPtr);
  //6.  Program internal registers
  FchTSXhciProgramInternalRegStepOne (DieBusNum, FchDataPtr);
  //7.  Program USB_S5_RESET = 0
  FchSmnRW (DieBusNum, FCH_TS_USB_RESET, ~ ((UINT32) FCH_TS_USB_S5_RESET), 0, LocalCfgPtr->StdHeader);
  //8.  Program USB_SOFT_PHY_RESET = 0
  FchSmnRW (DieBusNum, FCH_TS_USB_RESET, ~ ((UINT32) (FCH_TS_USB_PHY_RESET)), 0, LocalCfgPtr->StdHeader);
  //9.  Wait at least 200us
  FchStall (200, LocalCfgPtr->StdHeader);
  //10. Program USB_SOFT_CONTROLLER_RESET = 0
  FchSmnRW (DieBusNum, FCH_TS_USB_RESET, ~ ((UINT32) (FCH_TS_USB_CONTROLLER_RESET)), 0, LocalCfgPtr->StdHeader);
  //11. Program internal registers
  FchTSXhciProgramInternalRegStepTwo (DieBusNum, FchDataPtr);
}

/**
 * FchTSXhciInitS3ExitProgram - Config Xhci controller during
 * S3 Exit
 *
 *
 * @param[in] DieBusNum  IOCH bus number on current Die.
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciInitS3ExitProgram (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT32                      Retry;
  UINT32                      Pgfsm_RegData;
  UINT32                      CurrentPowerState;
  FCH_RESET_DATA_BLOCK        *LocalCfgPtr;
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  DEBUG ((DEBUG_INFO, "FchTSXhciInitS3ExitProgram on Bus %x - Entry\n", DieBusNum));
  FchSmnRW (DieBusNum, FCH_TS_USB_RESET, ~ ((UINT32) FCH_TS_USB_S5_RESET), 0, LocalCfgPtr->StdHeader); //Temp

  //1.  To restores PGFSM state which is lost by not being in S5 power
  //a.  program USB PGFSM to 'Power Down'
  //b.  Poll USB PGFSM Status to wait for 'Power is Off'
  //FCH_DEADLOOP();
  FchSmnRW (DieBusNum, FCH_TS_USB_RSMU_PGFSM_CNTL, 0x00, PGFSM_SELECT_0 + PGFSM_CMD_POWER_DOWN + PGFSM_SOFTWARE_CONTROL, LocalCfgPtr->StdHeader);
  for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
    FchSmnRW (DieBusNum, FCH_TS_USB_RSMU_PGFSM_CNTL, 0x00, PGFSM_SELECT_0 + PGFSM_CMD_POWER_READ + PGFSM_SOFTWARE_CONTROL, LocalCfgPtr->StdHeader);  // read Status register 0x00
    FchSmnRead (DieBusNum, FCH_TS_USB_RSMU_PGFSM_RD_DATA, &Pgfsm_RegData, LocalCfgPtr->StdHeader);
    if ((Pgfsm_RegData & PGFSM_STATUS_PWR_STS) == PGFSM_STATUS_PWR_OFF) {
      break;
    }
    FchStall (1, LocalCfgPtr->StdHeader);
  }
  ASSERT (Retry < MAX_RETRY_NUM);
  //2.  Disables PGFSM Override Register in USB Container Registers by programming the values as below:
  //    PGFSM_OVERRIDE_COLDRESETB       = 1'b1
  //    PGFSM_OVERRIDE_HARDRESETB       = 1'b1
  //    PGFSM_OVERRIDE_DS               = 1'b0
  //    PGFSM_OVERRIDE_SD               = 1'b0
  //    PGFSM_OVERRIDE_ISOB             = 1'b1
  //    PGFSM_OVERRIDE_SCAN_ISOB        = 1'b1
  //    PGFSM_OVERRIDE_MOTHER_SLEEP     = 1'b0
  //    PGFSM_OVERRIDE_DAUGHTER_SLEEP   = 1'b0
  FchSmnRW (DieBusNum, FCH_TS_USB_PGFSM_OVERRIDE, 0x00, 0x33, LocalCfgPtr->StdHeader);
  //3.  Program USB PGFSM to 'Power Up'
  FchSmnRW (DieBusNum, FCH_TS_USB_RSMU_PGFSM_CNTL, 0x00, PGFSM_SELECT_0 + PGFSM_CMD_POWER_UP + PGFSM_SOFTWARE_CONTROL, LocalCfgPtr->StdHeader);
  //4.  Poll USB PGFSM Status to wait for 'Power is On'
  for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
    FchSmnRW (DieBusNum, FCH_TS_USB_RSMU_PGFSM_CNTL, 0x00, PGFSM_SELECT_0 + PGFSM_CMD_POWER_READ + PGFSM_SOFTWARE_CONTROL, LocalCfgPtr->StdHeader);  // read Status register 0x00
    FchSmnRead (DieBusNum, FCH_TS_USB_RSMU_PGFSM_RD_DATA, &Pgfsm_RegData, LocalCfgPtr->StdHeader);
    if ((Pgfsm_RegData & PGFSM_STATUS_PWR_STS) == PGFSM_STATUS_PWR_ON) {
      break;
    }
    FchStall (1, LocalCfgPtr->StdHeader);
  }
  ASSERT (Retry < MAX_RETRY_NUM);
  //5.  Write PMCSR.PowerState in USB PCIe header to D0 (2'b00)
  FchSmnRW (DieBusNum, FCH_TS_USB_SHADOWED_POWERSTATE, 0x00, 0x00, LocalCfgPtr->StdHeader);
  //6.  Wait until Current_power_state is read as D0 (read as 2'b00)
  for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
    FchSmnRead (DieBusNum, FCH_TS_USB_CURRENT_PWR_STS, &CurrentPowerState, LocalCfgPtr->StdHeader);
    if ((CurrentPowerState & (BIT8 + BIT9)) == 0) {
      break;
    }
    FchStall (1, LocalCfgPtr->StdHeader);
  }
  ASSERT (Retry < MAX_RETRY_NUM);
  //7.  Program internal registers
  FchTSXhciProgramInternalRegStepTwo (DieBusNum, FchDataPtr);

  //Open Usb OC
  FchTSXhciPlatformOverCurrentMap(DieBusNum, FchDataPtr);
  DEBUG ((DEBUG_INFO, "FchTSXhciInitS3ExitProgram on Bus %x - Exit\n", DieBusNum));
}

/**
 * FchTSXhciInitS3EntryProgram - Config Xhci controller before
 * entering S3
 *
 *
 * @param[in] DieBusNum  IOCH bus number on current Die.
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciInitS3EntryProgram (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT32                      Retry;
  UINT32                      Pgfsm_RegData;
  UINT32                      CurrentPowerState;
  FCH_RESET_DATA_BLOCK        *LocalCfgPtr;
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  DEBUG ((DEBUG_INFO, "FchTSXhciInitS3EntryProgram on Bus %x - Entry\n", DieBusNum));

  //1.  Check USB container register Current_power_state is read as 2'b11 (D3 state)
  for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
    FchSmnRead (DieBusNum, FCH_TS_USB_CURRENT_PWR_STS, &CurrentPowerState, LocalCfgPtr->StdHeader);
    if ((CurrentPowerState & (BIT8 + BIT9)) == (BIT8 + BIT9)) {
      break;
    }
    FchStall (1, LocalCfgPtr->StdHeader);
  }
  //ASSERT (Retry < MAX_RETRY_NUM);
  //2.  Program USB PGFSM to 'Power Down'
  FchSmnRW (DieBusNum, FCH_TS_USB_RSMU_PGFSM_CNTL, 0x00, PGFSM_SELECT_0 + PGFSM_CMD_POWER_DOWN + PGFSM_SOFTWARE_CONTROL, LocalCfgPtr->StdHeader);
  //3.  Poll USB PGFSM Status to wait for 'Power is Off'
  for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
    FchSmnRW (DieBusNum, FCH_TS_USB_RSMU_PGFSM_CNTL, 0x00, PGFSM_SELECT_0 + PGFSM_CMD_POWER_READ + PGFSM_SOFTWARE_CONTROL, LocalCfgPtr->StdHeader);  // read Status register 0x00
    FchSmnRead (DieBusNum, FCH_TS_USB_RSMU_PGFSM_RD_DATA, &Pgfsm_RegData, LocalCfgPtr->StdHeader);
    if ((Pgfsm_RegData & PGFSM_STATUS_PWR_STS) == PGFSM_STATUS_PWR_OFF) {
      break;
    }
    FchStall (1, LocalCfgPtr->StdHeader);
  }
  ASSERT (Retry < MAX_RETRY_NUM);
  //4.  To maintain the PGFSM outputs (Isolation, Reset, SD/DS, power gating controls) which will be lost when S0 power is lost in S3, 
  // BIOS needs to program PGFSM Override Register in USB Container Registers as below:

  //    PGFSM_OVERRIDE_COLDRESETB       = 1'b0
  //    PGFSM_OVERRIDE_HARDRESETB       = 1'b0
  //    PGFSM_OVERRIDE_DS               = 1'b0
  //    PGFSM_OVERRIDE_SD               = 1'b1
  //    PGFSM_OVERRIDE_ISOB             = 1'b0
  //    PGFSM_OVERRIDE_SCAN_ISOB        = 1'b0
  //    PGFSM_OVERRIDE_MOTHER_SLEEP     = 1'b1
  //    PGFSM_OVERRIDE_DAUGHTER_SLEEP   = 1'b1
  FchSmnRW (DieBusNum, FCH_TS_USB_PGFSM_OVERRIDE, 0x00, 0xC8, LocalCfgPtr->StdHeader);

  //Close Usb OC
  FchSmnRW (DieBusNum, FCH_TS_USB_PORT_OCMAPPING0, (UINT32)0x0, (UINT32)~0x0, LocalCfgPtr->StdHeader);
  DEBUG ((DEBUG_INFO, "FchTSXhciInitS3EntryProgram on Bus %x - Exit\n", DieBusNum));
}

/**
 * FchTSXhciInitSsid - Update Xhci SSID
 *
 *
 * @param[in] DieBusNum  IOCH bus number on current Die.
 * @param[in] Ssid       The SSID value to be updated
 *
 */
VOID
FchTSXhciInitSsid (
  IN  UINT32   DieBusNum,
  IN  UINT32   Ssid
  )
{
  FchSmnRW (DieBusNum, FCH_TS_USB_SMN_PCICFG + 0x4C, 0x00, Ssid, NULL);
}

/**
 * FchTSXhciDisablePort - Disable Xhci ports
 *
 *
 * @param[in] DieBusNum      IOCH bus number on current Die.
 * @param[in] USB3DisableMap Bitmap to disable USB3 ports.
 * @param[in] USB2DisableMap Bitmap to disable USB2 ports.
 *
 */
BOOLEAN
FchTSXhciDisablePort (
  IN  UINT32   DieBusNum,
  IN  UINT32   USB3DisableMap,
  IN  UINT32   USB2DisableMap
  )
{
  UINT32 PortDisableMap;
  UINT32 PortDisableReg;

  PortDisableMap = ((USB3DisableMap & 0xFFFF) << 16) + (USB2DisableMap & 0xFFFF);
  FchSmnRead (DieBusNum, FCH_TS_USB_PORT_DISABLE0, &PortDisableReg, NULL);

  PortDisableMap |= PortDisableReg;

  //
  // Return TRUE if port already disabled.
  //
  if (PortDisableMap == PortDisableReg) {
    return TRUE;
  }

  //
  // Return FALSE if trying to disable port0 while other port enabled.
  //
  if ((PortDisableMap & BIT0) || (PortDisableMap & BIT16)) {
    if ((PortDisableMap & 0x000E000E) != 0x000E000E) {
      return FALSE;
    }
  }
  FchSmnRW (DieBusNum, FCH_TS_USB_PORT_DISABLE0, 0x00, PortDisableMap, NULL);
  return TRUE;
}

/**
 * FchTSXhciDisablePortMCM - Disable USB ports on MCM
 *
 *
 * @param[in] Socket         Sock number.
 * @param[in] USB3DisableMap Bitmap to disable USB3 ports.
 * @param[in] USB2DisableMap Bitmap to disable USB2 ports.
 *
 */
BOOLEAN
FchTSXhciDisablePortMCM (
  IN  UINT8    Socket,
  IN  UINT32   USB3DisableMap,
  IN  UINT32   USB2DisableMap
  )
{
  BOOLEAN Status;
  UINT32  DieBusNum;
  UINT32  DieAUSB3DisableMap;
  UINT32  DieAUSB2DisableMap;
  UINT32  DieBUSB3DisableMap;
  UINT32  DieBUSB2DisableMap;

  Status = TRUE;
  DieAUSB3DisableMap = USB3DisableMap & 0x3;
  DieAUSB2DisableMap = USB2DisableMap & 0x3;
  DieBUSB3DisableMap = (USB3DisableMap >> 2) & 0x3;
  DieBUSB2DisableMap = (USB2DisableMap >> 2) & 0x3;

  if ((DieAUSB3DisableMap != 0) || (DieAUSB2DisableMap != 0)) {
    if (Socket == 0) {
      DieBusNum = 0;
    } else {
      DieBusNum = ReadSocDieBusNum (4);
    }
    if (DieBusNum == 0xFFFFFFFF) {
      return FALSE;
    }
    Status = FchTSXhciDisablePort (DieBusNum, DieAUSB3DisableMap, DieAUSB2DisableMap);
  }

  if (!Status) {
    return FALSE;
  }

  if ((DieBUSB3DisableMap != 0) || (DieBUSB2DisableMap != 0)) {
    if (Socket == 0) {
      DieBusNum = ReadSocDieBusNum (1);
    } else {
      DieBusNum = ReadSocDieBusNum (5);
    }
    if (DieBusNum == 0xFFFFFFFF) {
      return FALSE;
    }
    Status = FchTSXhciDisablePort (DieBusNum, DieBUSB3DisableMap, DieBUSB2DisableMap);
  }
  return Status;
}

/**
 * FchTSXhciDisablePortSp3R2 - Disable USB ports on Sp3R2
 *
 *
 * @param[in] Socket         Sock number.
 * @param[in] USB3DisableMap Bitmap to disable USB3 ports.
 * @param[in] USB2DisableMap Bitmap to disable USB2 ports.
 *
 */
BOOLEAN
FchTSXhciDisablePortSp3R2 (
  IN  UINT8    Socket,
  IN  UINT32   USB3DisableMap,
  IN  UINT32   USB2DisableMap
  )
{
  BOOLEAN Status;
  UINT32  DieBusNum;
  UINT32  DieAUSB3DisableMap;
  UINT32  DieAUSB2DisableMap;
  UINT32  DieBUSB3DisableMap;
  UINT32  DieBUSB2DisableMap;

  Status = TRUE;
  DieAUSB3DisableMap = USB3DisableMap & 0xF;
  DieAUSB2DisableMap = USB2DisableMap & 0xF;
  DieBUSB3DisableMap = (USB3DisableMap >> 4) & 0xF;
  DieBUSB2DisableMap = (USB2DisableMap >> 4) & 0xF;

  if ((DieAUSB3DisableMap != 0) || (DieAUSB2DisableMap != 0)) {
    DieBusNum = 0;
    Status = FchTSXhciDisablePort (DieBusNum, DieAUSB3DisableMap, DieAUSB2DisableMap);
  }

  if (!Status) {
    return FALSE;
  }

  if ((DieBUSB3DisableMap != 0) || (DieBUSB2DisableMap != 0)) {
    DieBusNum = ReadSocDieBusNum (1);
    if (DieBusNum == 0xFFFFFFFF) {
      return FALSE;
    }
    Status = FchTSXhciDisablePort (DieBusNum, DieBUSB3DisableMap, DieBUSB2DisableMap);
  }
  return Status;
}

/**
 * FchXhciDisablePortSoc - Disable USB ports on Soc
 *
 *
 * @param[in] Socket         Sock number.
 * @param[in] USB3DisableMap Bitmap to disable USB3 ports.
 * @param[in] USB2DisableMap Bitmap to disable USB2 ports.
 *
 */
BOOLEAN
FchXhciDisablePortSoc (
  IN  UINT8    Socket,
  IN  UINT32   USB3DisableMap,
  IN  UINT32   USB2DisableMap
  )
{
  UINT8    PkgType;
  UINT32   RegEbx;

  //
  // MCM checking
  //
  AsmCpuid (0x80000001, NULL, &RegEbx, NULL, NULL);
  PkgType = (UINT8) (RegEbx >> 28);
  if ((PkgType == PKG_SP3) || (PkgType == PKG_SP4)) {
    //
    // SP3 & SP4
    //
    return FchTSXhciDisablePortMCM (Socket, USB3DisableMap, USB2DisableMap);
  } else if (PkgType == PKG_SP3_R2) {
    return FchTSXhciDisablePortSp3R2 (Socket, USB3DisableMap, USB2DisableMap);
  } else {
    return FchTSXhciDisablePort (0, USB3DisableMap, USB2DisableMap);
  }
}

/**
 * FchTSXhciOCmapping -  Xhci ports OC mapping
 *
 *
 * @param[in] DieBusNum      IOCH bus number on current Die.
 * @param[in] Port           Usb Port.
 * @param[in] OCPin          Over Current Pin number.
 *
 */
BOOLEAN
FchTSXhciOCmapping (
  IN  UINT32   DieBusNum,
  IN  UINT8    Port,
  IN  UINT8    OCPin
  )
{
  UINT8        PortOffset;

  if ((Port > 7) || (OCPin > 3)) {
    return FALSE;
  }

  PortOffset = Port << 2;

  FchSmnRW (DieBusNum, FCH_TS_USB_PORT_OCMAPPING0, ~(UINT32) (0xF << PortOffset), (UINT32) (OCPin << PortOffset), NULL);
  return TRUE;
}

/**
 * FchTSXhciOCmappingMCM -  Xhci ports OC mapping on MCM
 *
 *
 * @param[in] Socket         CPU socket number.
 * @param[in] Port           Usb Port.
 * @param[in] OCPin          Over Current Pin number.
 *
 */
BOOLEAN
FchTSXhciOCmappingMCM (
  IN  UINT8    Socket,
  IN  UINT8    Port,
  IN  UINT8    OCPin
  )
{
  UINT32  DieBusNum;

  DieBusNum = 0;

  if ((Port > 3) || (OCPin > 3)) {
    return FALSE;
  }

  //
  // on MCM, Port 0/1 and OC 0/1 from Die0
  //         Port 2/3 and OC 2/3 from Die1
  //
  if (((Port > 1) && (OCPin < 2)) ||((Port < 2) && (OCPin > 1))) {
    return FALSE;
  }

  if (Port < 2) {
    if (Socket == 0) {
      DieBusNum = 0;
    } else {
      DieBusNum = ReadSocDieBusNum (4);
    }
    if (DieBusNum == 0xFFFFFFFF) {
      return FALSE;
    }
    return FchTSXhciOCmapping (DieBusNum, Port, OCPin);
  } else {
    if (Socket == 0) {
      DieBusNum = ReadSocDieBusNum (1);
    } else {
      DieBusNum = ReadSocDieBusNum (5);
    }
    if (DieBusNum == 0xFFFFFFFF) {
      return FALSE;
    }
    return FchTSXhciOCmapping (DieBusNum, (Port - 2), (OCPin - 2));
  }
}

/**
 * FchTSXhciOCmappingSp3R2 -  Xhci ports OC mapping on Sp3R2
 *
 *
 * @param[in] Socket         CPU socket number.
 * @param[in] Port           Usb Port.
 * @param[in] OCPin          Over Current Pin number.
 *
 */
BOOLEAN
FchTSXhciOCmappingSp3R2 (
  IN  UINT8    Socket,
  IN  UINT8    Port,
  IN  UINT8    OCPin
  )
{
  UINT32  DieBusNum;

  DieBusNum = 0;

  if ((Port > 7) || (OCPin > 7)) {
    return FALSE;
  }

  //
  // on Sp3R2, Port 0-3 and OC 0-3 from Die0
  //           Port 4-7 and OC 4-7 from Die1
  //
  if (((Port > 3) && (OCPin < 4)) ||((Port < 4) && (OCPin > 3))) {
    return FALSE;
  }

  if (Port < 4) {
    DieBusNum = 0;
    return FchTSXhciOCmapping (DieBusNum, Port, OCPin);
  } else {
    DieBusNum = ReadSocDieBusNum (1);
    if (DieBusNum == 0xFFFFFFFF) {
      return FALSE;
    }
    return FchTSXhciOCmapping (DieBusNum, (Port - 4), (OCPin - 4));
  }
}
/**
 * FchXhciOCmappingSoc -  Xhci ports OC mapping on Soc
 *
 *
 * @param[in] Socket         CPU socket number.DieBusNum.
 * @param[in] Port           Usb Port.
 * @param[in] OCPin          Over Current Pin number.
 *
 */
BOOLEAN
FchXhciOCmappingSoc (
  IN  UINT8    Socket,
  IN  UINT8    Port,
  IN  UINT8    OCPin
  )
{
  UINT8    PkgType;
  UINT32   RegEbx;

  //
  // MCM checking
  //
  AsmCpuid (0x80000001, NULL, &RegEbx, NULL, NULL);
  PkgType = (UINT8) (RegEbx >> 28);
  if ((PkgType == PKG_SP3) || (PkgType == PKG_SP4)) {
    //
    // SP3 & SP4
    //
    return FchTSXhciOCmappingMCM (Socket, Port, OCPin);
  } else if (PkgType == PKG_SP3_R2) {
    return FchTSXhciOCmappingSp3R2 (Socket, Port, OCPin);
  } else {
    return FchTSXhciOCmapping (0, Port, OCPin);
  }
}

// DM1-271 >>>
/**
 * TurnOffUsbPme - Disable USB PME
 *
 *
 * @param[in] DieBusNum  IOCH bus number on current Die.
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
TurnOffUsbPme (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
    FCH_RESET_DATA_BLOCK        *LocalCfgPtr;
    UINT32                      Data32;

    if(PcdGetBool(PcdFchUsbPmeOffSupport) == FALSE) return;

    LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;
    FchSmnRead(DieBusNum, FCH_TS_USB_SHADOWED_PMCSR, &Data32, NULL);
    DEBUG ((DEBUG_INFO, "TurnOffUsbPme write Bus %x SMN: %x\n", DieBusNum, Data32));

    if(Data32 & FCH_TS_USB_PMCSR_PME_EN)
    {
        Data32 &= ~FCH_TS_USB_PMCSR_PME_EN;
        FchSmnWrite(DieBusNum, FCH_TS_USB_SHADOWED_PMCSR, &Data32, NULL); 
    }
}
// DM1-271 <<<

