/* $NoKeywords:$ */

/**
 * @file
 *
 * Config Fch Xhci controller
 *
 * Init Xhci Controller features (PEI phase).
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: FCH
 *
 */
/*
*****************************************************************************
*
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 *
****************************************************************************
*/
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include "FchPlatform.h"
#include "FchCommonCfg.h"
#include <Library/HygonSocBaseLib.h>
#include "Filecode.h"

#define FILECODE  FCH_TAISHAN_FCHTAISHANCORE_TAISHAN_TSUSB_TSXHCISERVICE_FILECODE

#define MAX_RETRY_NUM  1000

HYGON_USB_DISABLE_MAP gHygonUsbDisableMap[] = {
  // SocketNum, RootBridgeNum, UsbDisableMap
  // Socket0
  { 0, 0, 0x00 },
  { 0, 1, 0x00 },
  { 0, 2, 0xFF },     // may need debug
  { 0, 3, 0xFF },

  // Socket1
  { 1, 0, 0x00 },
  { 1, 1, 0x00 },
  { 1, 2, 0xFF },
  { 1, 3, 0xFF },

  // Socket2
  { 2, 0, 0x00 },
  { 2, 1, 0x00 },
  { 2, 2, 0xFF },
  { 2, 3, 0xFF },

  // Socket3
  { 3, 0, 0x00 },
  { 3, 1, 0x00 },
  { 3, 2, 0xFF },
  { 3, 3, 0xFF },

  // Socket4
  { 4, 0, 0x00 },
  { 4, 1, 0x00 },
  { 4, 2, 0xFF },
  { 4, 3, 0xFF },

  // Socket5
  { 5, 0, 0x00 },
  { 5, 1, 0x00 },
  { 5, 2, 0xFF },
  { 5, 3, 0xFF },

  // Socket6
  { 6, 0, 0x00 },
  { 6, 1, 0x00 },
  { 6, 2, 0xFF },
  { 6, 3, 0xFF },

  // Socket7
  { 7, 0, 0x00 },
  { 7, 1, 0x00 },
  { 7, 2, 0xFF },
  { 7, 3, 0xFF },
};

/**
 * FchTSXhciProgramInternalRegStepOne - Program USB internal
 * registers Step One
 *
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index 0 or 1.
 * @param[in] DieBusNum      IOHC bus number on current Die.
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciProgramInternalRegStepOne (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT8                       RegValue;
  FCH_RESET_DATA_BLOCK       *LocalCfgPtr;
  UINT32                      SmnAddress;
  UINT8                       PhyIndex;
  PHY_EXT_TX_EQ0              UsbTxEq0;
  PHY_EXT_TX_EQ1              UsbTxEq1;

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

  FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PHY0_PHYCFG0_HYEX), 0x00, 0x00000039, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PHY1_PHYCFG0_HYEX), 0x00, 0x00000039, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PHY2_PHYCFG0_HYEX), 0x00, 0x00000039, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PHY3_PHYCFG0_HYEX), 0x00, 0x00000039, LocalCfgPtr->StdHeader);
  
  if (0 == UsbIndex) {  //optmize the usb2.0 EQ of the nbio1 usb phy
    FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PHY0_LANEPARACTL0_HYEX), 0x00, 0x01B3C153, LocalCfgPtr->StdHeader);
    FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PHY1_LANEPARACTL0_HYEX), 0x00, 0x01B3C153, LocalCfgPtr->StdHeader);
    FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PHY2_LANEPARACTL0_HYEX), 0x00, 0x01B3C153, LocalCfgPtr->StdHeader);
    FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PHY3_LANEPARACTL0_HYEX), 0x00, 0x01B3C153, LocalCfgPtr->StdHeader);
  } else if (1 == UsbIndex) { 
    FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PHY0_LANEPARACTL0_HYEX), 0x00, 0x01B3C353, LocalCfgPtr->StdHeader);
    FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PHY1_LANEPARACTL0_HYEX), 0x00, 0x01B3C353, LocalCfgPtr->StdHeader);
    FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PHY2_LANEPARACTL0_HYEX), 0x00, 0x01B3C353, LocalCfgPtr->StdHeader);
    FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PHY3_LANEPARACTL0_HYEX), 0x00, 0x01B3C353, LocalCfgPtr->StdHeader);
  }

  // Program USB TX EQ
  for (PhyIndex = 0; PhyIndex < 4; PhyIndex++){
    //Program USB 3.1 Gen2 TX EQ
    SmnAddress = USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_PHY_EXT_TX_EQ0_HYEX + PhyIndex*0x400);
    FchSmnRead (DieBusNum, SmnAddress, &UsbTxEq0.Data32, LocalCfgPtr->StdHeader);
    UsbTxEq0.Reg.PROTOCOL_EXT_TX_EQ_OVRD_G1 = 1;
    UsbTxEq0.Reg.PROTOCOL_EXT_TX_EQ_MAIN_G1 = 0x39;
    UsbTxEq0.Reg.PROTOCOL_EXT_TX_EQ_OVRD_G2 = 1;
    UsbTxEq0.Reg.PROTOCOL_EXT_TX_EQ_MAIN_G2 = 0x37;
    FchSmnWrite (DieBusNum, SmnAddress, &UsbTxEq0.Data32, LocalCfgPtr->StdHeader);
    
    SmnAddress = USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_PHY_EXT_TX_EQ1_HYEX + PhyIndex*0x400);
    FchSmnRead (DieBusNum, SmnAddress, &UsbTxEq1.Data32, LocalCfgPtr->StdHeader);
    UsbTxEq1.Reg.PROTOCOL_EXT_TX_EQ_POST_G1 = 0x5;
    UsbTxEq1.Reg.PROTOCOL_EXT_TX_EQ_PRE_G1  = 0x0;
    UsbTxEq1.Reg.PROTOCOL_EXT_TX_EQ_POST_G2 = 0x5;
    UsbTxEq1.Reg.PROTOCOL_EXT_TX_EQ_PRE_G2  = 0x2;
    FchSmnWrite (DieBusNum, SmnAddress, &UsbTxEq1.Data32, LocalCfgPtr->StdHeader); 
  }
  SmnAddress = USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_PHY_CR_SELECT_HYEX);
  PhyIndex   = 0;
  FchSmnWrite (DieBusNum, SmnAddress, (UINT32*)&PhyIndex, LocalCfgPtr->StdHeader);

  // whether generate interrupt when ecc error
  RegValue = LocalCfgPtr->Xhci.XhciECCDedErrRptEn;
  if (RegValue) {
    FchSmnRW (
      DieBusNum,
      USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_INTERRUPT_CONTROL_HYEX),
      0xffffefff,
      BIT12,
      LocalCfgPtr->StdHeader
      );
  } else {
    FchSmnRW (
      DieBusNum,
      USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_INTERRUPT_CONTROL_HYEX),
      0xffffefff,
      0,
      LocalCfgPtr->StdHeader
      );
  }
}

/**
 * FchTSXhciGen31ProgramInternalRegStepTwo - Program USB internal
 * registers Step Two
 *
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index 0 or 1.
 * @param[in] DieBusNum      IOHC bus number on current Die.
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciGen31ProgramInternalRegStepTwo (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK        *LocalCfgPtr;
  UINT32                      Value32;
  UINT32                      SmnAddress;
  UINT8                       PhyIndex;
  SUP_DIG_LVL_OVER_IN         SupDigLvl;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  // 11. Program Controller internal registers
  //        Program GUCTL1(address=0x16C0C11C) to 0x20001908
  //        Program GUCTL(address=0x16C0C12C) to 0x0A434802  //PLAT-11336 clearing GUCTL[15].CMdevAddr to 0
  //        Program GUSB3PIPECTL0(address=0x16C0C2C0) to 0x01000202
  //        Program GUSB3PIPECTL1(address=0x16C0C2C4) to 0x01000202
  //        Program GUSB3PIPECTL2(address=0x16C0C2C8) to 0x01000202
  //        Program GUSB3PIPECTL3(address=0x16C0C2CC) to 0x01000202

  FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_GUCTL1_HYEX), 0x00, 0x20001908, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_GUCTL_HYEX), 0x00, 0x0A434802, LocalCfgPtr->StdHeader);

  // disable suspendenable bit as a workaroud. when parts of usb 3.1 ports are attached usb3.1 gen1 device, if reset xhci controller or reboot the system, 
  // then the pls of the remainder usb3.1 ports will enter disable status. 
  FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_GUSB3PIPECTL0_HYEX), 0x00, 0x01000202, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_GUSB3PIPECTL1_HYEX), 0x00, 0x01000202, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_GUSB3PIPECTL2_HYEX), 0x00, 0x01000202, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_GUSB3PIPECTL3_HYEX), 0x00, 0x01000202, LocalCfgPtr->StdHeader);
  //Disable USB2.0 suspendenable bit
  FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_GUSB2PHYCFG0_HYEX), 0xFFFFFFBF, 0, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_GUSB2PHYCFG1_HYEX), 0xFFFFFFBF, 0, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_GUSB2PHYCFG2_HYEX), 0xFFFFFFBF, 0, LocalCfgPtr->StdHeader);
  FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_GUSB2PHYCFG3_HYEX), 0xFFFFFFBF, 0, LocalCfgPtr->StdHeader);
    
  // workaroud
  FchSmnRead (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_GSNPSID_HYEX), &Value32, NULL);
  if (Value32 == GSNPSID_REVISION_320A) {
      FchSmnRW (
        DieBusNum,
        USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_GUCTL2_HYEX),
        0xFFFFEFFF,
        GUCTL2_ENABLE_EP_CACHE_EVICT,
        LocalCfgPtr->StdHeader
        );
  }
  
  // 12. Program USB TX vboost
  for (PhyIndex = 0; PhyIndex < 4; PhyIndex++){
    SmnAddress = USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_PHY_CR_SELECT_HYEX);
    FchSmnWrite (DieBusNum, SmnAddress, (UINT32*)&PhyIndex, LocalCfgPtr->StdHeader);
    
    SmnAddress = USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_SUP_DIG_LVL_OVER_IN_HYEX);
    FchSmnRead16 (DieBusNum, SmnAddress, &SupDigLvl.Data16, LocalCfgPtr->StdHeader);
    SupDigLvl.Reg.TX_VBOOST_LVL_EN = 1;
    SupDigLvl.Reg.TX_VBOOST_LVL  = 0x7;
    FchSmnWrite16 (DieBusNum, SmnAddress, &SupDigLvl.Data16, LocalCfgPtr->StdHeader);
  }
  SmnAddress = USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_PHY_CR_SELECT_HYEX);
  PhyIndex   = 0;
  FchSmnWrite (DieBusNum, SmnAddress, (UINT32*)&PhyIndex, LocalCfgPtr->StdHeader);
}

/**
 * FchTSXhciConfigPhyExternalSRAM - Config USB 3.1 PHY external SRAM
 *
 *
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index 0 or 1.
 * @param[in] DieBusNum  IOHC bus number on current Die.
 * @param[in] FchDataPtr Fch configuration structure pointer.
 * @param[in] PhyNum 0~3
 * @param[in] UpdateFlag 1:Update SRAM; 0:Not update SRAM
 *
 */
VOID
FchTSXhciGen31ConfigPhyExtSRAM (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr,
  IN  UINT32   PhyNum,
  IN  BOOLEAN  UpdateFlag,
  IN  VOID     *FwBuffer
  )
{
  UINT32  Retry;
  UINT32  Reg;
  UINT32  Data32;
  UINT32  Index;
  UINT16  *Buffer;
  FCH_RESET_DATA_BLOCK        *LocalCfgPtr;
  
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;
  
  //Step 1. polling sram_init_done, wait for it changes to 1
  DEBUG ((DEBUG_INFO, " Polling NBIO %d Bus (0x%X) PHY %d sram_init_done \n", UsbIndex, DieBusNum, PhyNum));
  Reg = USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_PHY0_SRAMINITDONE_HYEX + PhyNum * 0x400);
  for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
    FchSmnRead (DieBusNum, Reg, &Data32, LocalCfgPtr->StdHeader);
    if ((Data32 & BIT0) == 1) {
      break;
    }
    FchStall (1, LocalCfgPtr->StdHeader);
  }
  if (Retry == MAX_RETRY_NUM) {
    DEBUG ((DEBUG_INFO, "Wait failure \n"));
    return;
  }
  
  //Step 2. if USB3.1 SRAM needs update, override the SRAM using CREG address
  if (UpdateFlag) {
    if (FwBuffer == NULL) {
      DEBUG ((DEBUG_INFO, "Update bus (0x%X) FW , but FW buffer is NULL \n", DieBusNum));
    } else {
      DEBUG ((DEBUG_INFO, "Update bus (0x%X) PHY %d FW begin \n", DieBusNum, PhyNum));
      
      //Step 2.1 Select which USB3.1 PHY will be accessed
      FchSmnRW (
        DieBusNum,
        USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_PHY_CR_SELECT_HYEX),
        0xfffffffc,
        PhyNum,
        LocalCfgPtr->StdHeader
        );
            
      //Step 2.2 Write FW to PCS raw RAM 16D7_0000h ~ 16D7_5FFCh,
      //write data = {16'h0, 16-bit FW data}
      //Loop 6000h/4 = 1800h times
      Buffer = (UINT16 *)FwBuffer;
      for (Index = 0; Index < 0x1800; Index++) {
        Reg = USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_PHY_PCS_RAW_RAM_HYEX + Index * 4);
        Data32 = (UINT32)(*(Buffer++));
        FchSmnRW (DieBusNum, Reg, 0, Data32, LocalCfgPtr->StdHeader);
      }

      DEBUG ((DEBUG_INFO, "Update bus (0x%X) PHY %d FW end \n", DieBusNum, PhyNum));
    }
  }
    
  //Step 3. assert sram_ext_ld_done, write sram_ext_ld_done = 1
  DEBUG ((DEBUG_INFO, "Assert bus (0x%X) PHY %d sram_ext_ld_done \n", DieBusNum, PhyNum));
  Reg = USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_PHY0_EXTIDDONE_HYEX + PhyNum * 0x400);
  FchSmnRW (DieBusNum, Reg, 0xfffffffd, BIT1, LocalCfgPtr->StdHeader);
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
 * @param[in] SocketId       Socket Id of IOD.
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index 0 or 1.
 * @param[in] DieBusNum      IOHC bus number on current Die.
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciPlatformPortDisable (
  IN  UINT8    SocketId,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT8                       SocketNum;
  UINT8                       RbNum;
  UINT8                       PortNum;
  UINT8                       Usb3PortNum;
  UINT8                       Usb2PortNum;
  UINT32                      UsbPortDisable;
  FCH_RESET_DATA_BLOCK        *LocalCfgPtr;
  HYGON_USB_DISABLE_MAP       *UsbDisableMap;
  UINT8                       *TempUsbPortDisableMap;
  UINT8                       TempUsbPortDisableMapSize;
  UINTN                       UsbDisableMapCount;
  UINT32                      Usb3PortDisable;
  UINT32                      Usb2PortDisable;
  UINT8                       NodeIndex;
  UINT8                       SocketPresent;
  UINT8                       RbPresent;
  UINTN                       DieType;
  UINTN                       LogicalDieId;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  UsbDisableMap = (HYGON_USB_DISABLE_MAP *)AllocateZeroPool(sizeof(gHygonUsbDisableMap));

  if (UsbDisableMap != NULL){
    CopyMem(UsbDisableMap, gHygonUsbDisableMap, sizeof(gHygonUsbDisableMap));

    TempUsbPortDisableMap     = (UINT8 *)PcdGetPtr (PcdXhciPlatformPortDisableMapHyEx);
    TempUsbPortDisableMapSize = (UINT8)PcdGetSize(PcdXhciPlatformPortDisableMapHyEx);
    UsbDisableMapCount        = sizeof(gHygonUsbDisableMap) / sizeof(HYGON_USB_DISABLE_MAP);
    for (NodeIndex = 0; NodeIndex < UsbDisableMapCount; NodeIndex++) {
      if (NodeIndex == TempUsbPortDisableMapSize) {
        break;
      }
      UsbDisableMap[NodeIndex].UsbDisableMap = *TempUsbPortDisableMap;
      TempUsbPortDisableMap++;
    }
  } else {
    DEBUG ((DEBUG_INFO, "UsbDisableMap Use Default Value\n"));
    UsbDisableMap = &gHygonUsbDisableMap[0];
  }
  

  SocketPresent = (UINT8)FabricTopologyGetNumberOfSocketPresent ();
  RbPresent = (UINT8)FabricTopologyGetNumberOfRootBridgesOnSocket(0);
  for (NodeIndex = 0; NodeIndex < UsbDisableMapCount; NodeIndex++) {
    SocketNum = UsbDisableMap[NodeIndex].SocketNum;
    RbNum     = UsbDisableMap[NodeIndex].RootBridgeNum;

    if((SocketNum >= SocketPresent) || (RbNum >= RbPresent)) {
      continue;
    }

    Usb2PortDisable = (UINT32)(UsbDisableMap[NodeIndex].UsbDisableMap & 0xF);
    Usb3PortDisable = (UINT32)((UsbDisableMap[NodeIndex].UsbDisableMap & 0xF0) >> 4);

    FabricTopologyGetPhysicalIodDieInfo ((UINTN)PhysicalDieId, &LogicalDieId, &DieType);
    if (DieBusNum == ReadSocDieBusNum (SocketNum, LogicalDieId, RbNum)) {
      //Satori one XHCI controller has four 2.0 ports and three 3.0 Ports
      Usb3PortDisable |= BIT3;

      UsbPortDisable = (Usb3PortDisable << 16) | Usb2PortDisable;

      Usb2PortNum = 4 - (UINT8)BitWeight (&Usb2PortDisable, sizeof (Usb2PortDisable));
      Usb3PortNum = 4 - (UINT8)BitWeight (&Usb3PortDisable, sizeof (Usb3PortDisable));
      PortNum     = (Usb3PortNum << 4) | Usb2PortNum;

      DEBUG ((DEBUG_INFO, "FchTSXhciPlatformPortDisable on Bus 0x%x - PortNum 0x%x(Usb2PortNum=0x%x, Usb3PortNum=0x%x) - UsbPortDisable 0x%x\n", DieBusNum, PortNum, Usb2PortNum,Usb3PortNum, UsbPortDisable));
      FchSmnRW (DieBusNum, USB_SPACE_HYEX (RbNum, FCH_TS_USB_PORT_CONTROL_HYEX), 0x00, (UINT32)PortNum, LocalCfgPtr->StdHeader);
      FchSmnRW (DieBusNum, USB_SPACE_HYEX (RbNum, FCH_TS_USB_PORT_DISABLE0_HYEX), 0x00, UsbPortDisable, LocalCfgPtr->StdHeader);
    }
  }

  if (UsbDisableMap != &gHygonUsbDisableMap[0]){
    FreePool(UsbDisableMap);
  }
}

/**
 * FchTSXhciPlatformOverCurrentMap - Platform/Customer Based
 * Host configuration
 *
 * @param[in] SocketId       Socket Id of IOD.
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index 0 or 1.
 * @param[in] DieBusNum      IOHC bus number on current Die.
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciPlatformOverCurrentMap (
  IN  UINT8    SocketId,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT8                       SocNumPcd;
  UINT8                       RbNumPcd;
  UINT8                       PortNum;
  UINT8                       OCPin;
  UINT8                       *pUsbPortOCPinArray = NULL;
  UINT32                      UsbPortOCPinArray;

  FCH_RESET_DATA_BLOCK        *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  // Set OC Pin is low when OC occurs.
  FchSmnRW (
    DieBusNum,
    USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_PORT_CONTROL_HYEX),
    (UINT32)~BIT8,
    (UINT32)BIT8,
    LocalCfgPtr->StdHeader
    );

  //Get SocketNum/RbNum, USB Port, OCPin Array by Pcd
  //*pUsbPortOCPinArray = UINT32[OcPin:PortNum:RbNum:SocetNum]
  pUsbPortOCPinArray = (UINT8 *)PcdGetPtr (PcdDieNumUsbPortAndOCPinMapHyEx);

  while (*pUsbPortOCPinArray != 0xFF) {
    // Get SocketNum/Iohc/Port/OCPin Num
    UsbPortOCPinArray = *(UINT32 *)pUsbPortOCPinArray;

    SocNumPcd = (UINT8)((UsbPortOCPinArray >> 0) & 0xFF);
    RbNumPcd  = (UINT8)((UsbPortOCPinArray >> 8) & 0xFF);
    PortNum   = (UINT8)((UsbPortOCPinArray >> 16) & 0xFF);
    OCPin     = (UINT8)((UsbPortOCPinArray >> 24) & 0xFF);

    if ((SocketId == SocNumPcd) && (UsbIndex == RbNumPcd)) {
      //Set OC Pin is low when OC occurs.
      FchSmnRW (
        DieBusNum,
        USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_PORT_CONTROL_HYEX),
        (UINT32)~BIT8,
        (UINT32)BIT8,
        LocalCfgPtr->StdHeader
        );

      // Match the DieBusNum
      FchTSXhciOCmapping (SocketId, PhysicalDieId, UsbIndex, PortNum, OCPin);
    }

    pUsbPortOCPinArray += 4;
  }
}

/**
 * FchTSXhciPlatformConfiguration - Platform/Customer Based Host
 * configuration
 *
 * @param[in] SocketId       Socket Id of IOD.
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index 0 or 1.
 * @param[in] DieBusNum      IOHC bus number on current Die.
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciPlatformConfiguration (
  IN  UINT8    SocketId,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  FchTSXhciPlatformPortDisable (SocketId, PhysicalDieId, UsbIndex, DieBusNum, FchDataPtr);

  FchTSXhciPlatformOverCurrentMap (SocketId, PhysicalDieId, UsbIndex, DieBusNum, FchDataPtr);
}

/**
 * FchTSXhciInitBootProgram - Config Xhci controller during
 * Power-On
 *
 * @param[in] Socket         Socket Id of IOD.
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index
 * @param[in] DieBusNum      IOCH bus number on current Die.
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciInitBootProgram (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT32                Data = 0;
  UINT32                PortNum = 0; 
  UINT32                Retry;
  UINT32                CurrentPowerState;
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  if (PcdGet8 (PcdRunEnvironment) == 0) {
    DEBUG ((DEBUG_INFO, "FchTSXhciInitBootProgram ...Start.\n"));

    //enable smu FW to handle the d3 transition of the usb controller

    FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, RCC_NBIF0_STRAP1_ADDRESS_HYEX), 0xffffffef, 0x10, LocalCfgPtr->StdHeader);
    FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_SMU_ASSIST_POWER_STATE_CTL_HYEX), 0xffffffef, 0x10, LocalCfgPtr->StdHeader);
    FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_SMU_ASSIST_POWER_STATE_CTL_HYEX), 0xfffffeff, 0x100, LocalCfgPtr->StdHeader);

    // 1.  Wait until USB rsmu_hard_resetb is cleared by SMU FW
    // FCH_DEADLOOP();
    // 1. Program D0
    // 1.1.  Write PMCSR.PowerState in USB PCIe header to D0 (2'b00)
    FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_SHADOWED_POWERSTATE_HYEX), 0x00, 0x00, LocalCfgPtr->StdHeader);
    // 1.2.  Wait until Current_power_state is read as D0 (read as 2'b00)
    for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
      FchSmnRead (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_CURRENT_PWR_STS_HYEX), &CurrentPowerState, LocalCfgPtr->StdHeader);
      if ((CurrentPowerState & (BIT8 + BIT9)) == 0) {
        break;
      }

      FchStall (1, LocalCfgPtr->StdHeader);
    }

    // ASSERT (Retry < MAX_RETRY_NUM);
    // 2.  Program USB_S5_RESET = 1
    FchSmnRW (
      DieBusNum,
      USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_RESET_HYEX),
      ~((UINT32)FCH_TS_USB_S5_RESET),
      FCH_TS_USB_S5_RESET,
      LocalCfgPtr->StdHeader
      );
    // 3.  Program USB_SOFT_CONTROLLER_RESET = 1
    FchSmnRW (
      DieBusNum,
      USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_RESET_HYEX),
      ~((UINT32)(FCH_TS_USB_CONTROLLER_RESET)),
      (FCH_TS_USB_CONTROLLER_RESET),
      LocalCfgPtr->StdHeader
      );
    // 4.  Wait at least 100us
    FchStall (100, LocalCfgPtr->StdHeader);
    // 5.   Platform/Customer based host configuration: If there's needed to program USB container register to change USB features or mode, do in this step. Following are some examples:
    // a.   To change USB port numbers, program Port Control (0x16D8_0130), and Port Disable Write Once (0x16D8_0128) and/or Port Disable Read Write (0x16D8_012C)
    // b.   To program over current mapping, program Overcurrent Map register (0x16D8_0120 or 0x16D8_0120)
    // c.   To enable SMI/NMI (0x16D8_0118)
    FchTSXhciPlatformConfiguration (Socket, PhysicalDieId, UsbIndex, DieBusNum, FchDataPtr);
    
    //Set all USB port Force Gen1
    FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_FORCE_GEN1_HYEX), 0x0, 0xF, LocalCfgPtr->StdHeader); 

    // 6.  Program internal registers
    FchTSXhciProgramInternalRegStepOne (PhysicalDieId, UsbIndex, DieBusNum, FchDataPtr);
    // 7.  Program USB_S5_RESET = 0
    FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_RESET_HYEX), ~((UINT32)FCH_TS_USB_S5_RESET), 0, LocalCfgPtr->StdHeader);
    // 8.  Wait at least 200us
    FchStall (200, LocalCfgPtr->StdHeader);
    // 9. Update USB3.1 PHY FW
    FchTSXhciGen31ConfigPhyExtSRAM(PhysicalDieId, UsbIndex, DieBusNum, FchDataPtr, 0, FALSE, NULL);
    FchTSXhciGen31ConfigPhyExtSRAM(PhysicalDieId, UsbIndex, DieBusNum, FchDataPtr, 1, FALSE, NULL);
    FchTSXhciGen31ConfigPhyExtSRAM(PhysicalDieId, UsbIndex, DieBusNum, FchDataPtr, 2, FALSE, NULL);
    FchTSXhciGen31ConfigPhyExtSRAM(PhysicalDieId, UsbIndex, DieBusNum, FchDataPtr, 3, FALSE, NULL);
    // 10. Program USB_SOFT_CONTROLLER_RESET = 0
    FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_RESET_HYEX), ~((UINT32)(FCH_TS_USB_CONTROLLER_RESET)), 0, LocalCfgPtr->StdHeader);
    // 11. Program internal registers
    FchTSXhciGen31ProgramInternalRegStepTwo (PhysicalDieId, UsbIndex, DieBusNum, FchDataPtr);

    //reset usb PHY
    FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_RESET_HYEX), ~((UINT32)(FCH_TS_USB_PHY_RESET)),
                    (FCH_TS_USB_PHY_RESET), LocalCfgPtr->StdHeader);
    FchStall (200, LocalCfgPtr->StdHeader);
    FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_RESET_HYEX), ~((UINT32)(FCH_TS_USB_PHY_RESET)), 0, LocalCfgPtr->StdHeader); 
    
    //USB3.1 Port Link State will be Messed when XHC Controller is reset by setting USBCMD.HCRST=1, setting PORTSC.PLS=U3 can
    //make usb bus idle, so as to avoid this error case;
    FchStall (200, NULL);
    for(PortNum = 0; PortNum < 4; PortNum++){
      FchSmnRead (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB3_PORTSC_HYEX + 0x10 * PortNum), &Data, NULL);
      DEBUG ((DEBUG_INFO, "DieBusNum = 0x%x, xHC PORTSC%d = 0x%x before SetU3\n", DieBusNum, PortNum, Data));

      if(Data & BIT1){
        FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB3_PORTSC_HYEX + 0x10 * PortNum), 0xFFFFFFFD, 0x10060, NULL);
        FchStall (200, NULL);
        FchSmnRead (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB3_PORTSC_HYEX + 0x10 * PortNum), &Data, NULL);
        DEBUG ((DEBUG_INFO, "DieBusNum = 0x%x, xHC PORTSC%d = 0x%x after SetU3\n",DieBusNum, PortNum, Data));
      }
    }
    DEBUG ((DEBUG_INFO, "FchTSXhciInitBootProgram ...End.\n"));
  }
}

/**
 * FchTSXhciInitS3ExitProgram - Config Xhci controller during
 * S3 Exit
 *
 * @param[in] Socket         Socket Id of IOD.
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index 0 or 1.
 * @param[in] DieBusNum      IOCH bus number on current Die.
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciInitS3ExitProgram (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT32                Retry;
  UINT32                Pgfsm_RegData;
  UINT32                CurrentPowerState;
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  DEBUG ((DEBUG_INFO, "FchTSXhciInitS3ExitProgram on Bus %x - Entry\n", DieBusNum));
  FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_RESET_HYEX), ~ ((UINT32) FCH_TS_USB_S5_RESET), 0, LocalCfgPtr->StdHeader); //Temp

  // 1.  To restores PGFSM state which is lost by not being in S5 power
  // a.  program USB PGFSM to 'Power Down'
  // b.  Poll USB PGFSM Status to wait for 'Power is Off'
  // FCH_DEADLOOP();
  FchSmnRW (
    DieBusNum,
    USB_RSMU_SPACE_HYEX (UsbIndex, FCH_TS_USB_RSMU_PGFSM_CNTL),
    0x00,
    PGFSM_SELECT_0 + PGFSM_CMD_POWER_DOWN + PGFSM_SOFTWARE_CONTROL,
    LocalCfgPtr->StdHeader
    );

  for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
    FchSmnRW (
      DieBusNum,
      USB_RSMU_SPACE_HYEX (UsbIndex, FCH_TS_USB_RSMU_PGFSM_CNTL),
      0x00,
      PGFSM_SELECT_0 + PGFSM_CMD_POWER_READ + PGFSM_SOFTWARE_CONTROL,
      LocalCfgPtr->StdHeader
      );
    // read status
    FchSmnRead (
      DieBusNum,
      USB_RSMU_SPACE_HYEX (UsbIndex, FCH_TS_USB_RSMU_PGFSM_WR_DATA),
      &Pgfsm_RegData,
      LocalCfgPtr->StdHeader
      );

    if ((Pgfsm_RegData & PGFSM_STATUS_PWR_STS) == PGFSM_STATUS_PWR_OFF) {
      break;
    }

    FchStall (1, LocalCfgPtr->StdHeader);
  }

  ASSERT (Retry < MAX_RETRY_NUM);
  // 2.  Disables PGFSM Override Register in USB Container Registers by programming the values as below:
  // PGFSM_OVERRIDE_COLDRESETB       = 1'b1
  // PGFSM_OVERRIDE_HARDRESETB       = 1'b1
  // PGFSM_OVERRIDE_DS               = 1'b0
  // PGFSM_OVERRIDE_SD               = 1'b0
  // PGFSM_OVERRIDE_ISOB             = 1'b1
  // PGFSM_OVERRIDE_SCAN_ISOB        = 1'b1
  // PGFSM_OVERRIDE_MOTHER_SLEEP     = 1'b0
  // PGFSM_OVERRIDE_DAUGHTER_SLEEP   = 1'b0
  FchSmnRW (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_PGFSM_OVERRIDE_HYEX), 0x00, 0x33, LocalCfgPtr->StdHeader);
  // 3.  Program USB PGFSM to 'Power Up'
  FchSmnRW (
    DieBusNum,
    USB_RSMU_SPACE_HYEX (UsbIndex, FCH_TS_USB_RSMU_PGFSM_CNTL),
    0x00,
    PGFSM_SELECT_0 + PGFSM_CMD_POWER_UP + PGFSM_SOFTWARE_CONTROL,
    LocalCfgPtr->StdHeader
    );
  // 4.  Poll USB PGFSM Status to wait for 'Power is On'
  for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
    FchSmnRW (
      DieBusNum,
      USB_RSMU_SPACE_HYEX (UsbIndex, FCH_TS_USB_RSMU_PGFSM_CNTL),
      0x00,
      PGFSM_SELECT_0 + PGFSM_CMD_POWER_READ + PGFSM_SOFTWARE_CONTROL,
      LocalCfgPtr->StdHeader
      );
    // read Status register 0x00
    FchSmnRead (
      DieBusNum,
      USB_RSMU_SPACE_HYEX (UsbIndex, FCH_TS_USB_RSMU_PGFSM_WR_DATA),
      &Pgfsm_RegData,
      LocalCfgPtr->StdHeader
      );
    if ((Pgfsm_RegData & PGFSM_STATUS_PWR_STS) == PGFSM_STATUS_PWR_ON) {
      break;
    }

    FchStall (1, LocalCfgPtr->StdHeader);
  }

  ASSERT (Retry < MAX_RETRY_NUM);
  // 5.  Write PMCSR.PowerState in USB PCIe header to D0 (2'b00)
  FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_SHADOWED_POWERSTATE_HYEX), 0x00, 0x00, LocalCfgPtr->StdHeader);
  // 6.  Wait until Current_power_state is read as D0 (read as 2'b00)
  for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
    FchSmnRead (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_CURRENT_PWR_STS_HYEX), &CurrentPowerState, LocalCfgPtr->StdHeader);
    if ((CurrentPowerState & (BIT8 + BIT9)) == 0) {
      break;
    }

    FchStall (1, LocalCfgPtr->StdHeader);
  }

  ASSERT (Retry < MAX_RETRY_NUM);
  // 7.  Program internal registers
  FchTSXhciGen31ProgramInternalRegStepTwo (PhysicalDieId, UsbIndex, DieBusNum, FchDataPtr);

  // Open Usb OC
  FchTSXhciPlatformOverCurrentMap (Socket, PhysicalDieId, UsbIndex, DieBusNum, FchDataPtr);
  DEBUG ((DEBUG_INFO, "FchTSXhciInitS3ExitProgram on Bus %x - Exit\n", DieBusNum));
}

/**
 * FchTSXhciInitS3EntryProgram - Config Xhci controller before
 * entering S3
 *
 * @param[in] Socket         Socket Id of IOD.
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index 0 or 1.
 * @param[in] DieBusNum      IOHC bus number on current Die.
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSXhciInitS3EntryProgram (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT32                Retry;
  UINT32                Pgfsm_RegData;
  UINT32                CurrentPowerState;

  DEBUG ((DEBUG_INFO, "FchTSXhciInitS3EntryProgram on Bus %x - Entry\n", DieBusNum));

  //1.  Check USB container register Current_power_state is read as 2'b11 (D3 state)
  for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
    FchSmnRead (
      DieBusNum,
      USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_CURRENT_PWR_STS_HYEX),
      &CurrentPowerState,
      NULL
      );
    if ((CurrentPowerState & (BIT8 + BIT9)) == (BIT8 + BIT9)) {
      break;
    }

    FchStall (1, NULL);
  }

  // ASSERT (Retry < MAX_RETRY_NUM);
  // 2.  Program USB PGFSM to 'Power Down'
  FchSmnRW (
    DieBusNum,
    USB_RSMU_SPACE_HYEX (UsbIndex, FCH_TS_USB_RSMU_PGFSM_CNTL),
    0x00,
    PGFSM_SELECT_0 + PGFSM_CMD_POWER_DOWN + PGFSM_SOFTWARE_CONTROL,
    NULL
    );
  // 3.  Poll USB PGFSM Status to wait for 'Power is Off'
  for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
    FchSmnRW (
      DieBusNum,
      USB_RSMU_SPACE_HYEX (UsbIndex, FCH_TS_USB_RSMU_PGFSM_CNTL),
      0x00,
      PGFSM_SELECT_0 + PGFSM_CMD_POWER_READ + PGFSM_SOFTWARE_CONTROL,
      NULL
      );                                                                                                                                             // read Status register 0x00
    FchSmnRead (
      DieBusNum,
      USB_RSMU_SPACE_HYEX (UsbIndex, FCH_TS_USB_RSMU_PGFSM_WR_DATA),
      &Pgfsm_RegData,
      NULL
      );
      
    if ((Pgfsm_RegData & PGFSM_STATUS_PWR_STS) == PGFSM_STATUS_PWR_OFF) {
      break;
    }

    FchStall (1, NULL);
  }

  ASSERT (Retry < MAX_RETRY_NUM);
  // 4.To maintain the PGFSM outputs (Isolation, Reset, SD/DS, power gating controls) which will be lost when S0 power is lost in S3,
  // BIOS needs to program PGFSM Override Register in USB Container Registers as below:

  // PGFSM_OVERRIDE_COLDRESETB       = 1'b0
  // PGFSM_OVERRIDE_HARDRESETB       = 1'b0
  // PGFSM_OVERRIDE_DS               = 1'b0
  // PGFSM_OVERRIDE_SD               = 1'b1
  // PGFSM_OVERRIDE_ISOB             = 1'b0
  // PGFSM_OVERRIDE_SCAN_ISOB        = 1'b0
  // PGFSM_OVERRIDE_MOTHER_SLEEP     = 1'b1
  // PGFSM_OVERRIDE_DAUGHTER_SLEEP   = 1'b1
  FchSmnRW (
    DieBusNum,
    USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PGFSM_OVERRIDE_HYEX),
    0x00,
    0xC8,
    NULL
    );

  // Close Usb OC
  FchSmnRW (
    DieBusNum,
    USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_PORT_OCMAPPING0_HYEX),
    (UINT32)0x0,
    (UINT32) ~0x0,
    NULL
    );
  DEBUG ((DEBUG_INFO, "FchTSXhciInitS3EntryProgram on Bus %x - Exit\n", DieBusNum));
}

/**
 * FchTSXhciInitSsid - Update Xhci SSID
 *
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index 0 or 1.
 * @param[in] DieBusNum      IOHC bus number on current NBIO.
 * @param[in] Ssid           The SSID value to be updated
 *
 */
VOID
FchTSXhciInitSsid (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  UINT32   Ssid
  )
{
  // need confirm program USB0 & USB1 SSID
  FchSmnRW (
    DieBusNum,
    NBIO_SPACE2(PhysicalDieId, UsbIndex, PCICFG_NBIF0_EPFN_CFG_ADDRESS_HYEX) + 0x4C,
    0x00,
    Ssid,
    NULL
    );
}

/**
 * FchTSXhciDisablePort - Disable Xhci ports
 *
 *
 * @param[in] Socket         CPU Socket number.
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index 0 or 1.
 * @param[in] USB3DisableMap Bitmap to disable USB3 ports.
 * @param[in] USB2DisableMap Bitmap to disable USB2 ports.
 *
 */
BOOLEAN
FchTSXhciDisablePort (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   USB3DisableMap,
  IN  UINT32   USB2DisableMap
  )
{
  UINT32  PortDisableMap;
  UINT32  PortDisableReg;
  UINT32  DieBusNum;
  UINTN   LogicalDieId;
  UINTN   DieType;

  FabricTopologyGetPhysicalIodDieInfo (PhysicalDieId, &LogicalDieId, &DieType);
  DieBusNum = ReadSocDieBusNum (Socket, LogicalDieId, 0);
  PortDisableMap = ((USB3DisableMap & 0xFFFF) << 16) + (USB2DisableMap & 0xFFFF);
  FchSmnRead (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PORT_DISABLE0_HYEX), &PortDisableReg, NULL);

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

  FchSmnRW (DieBusNum, USB_SPACE_HYEX(UsbIndex, FCH_TS_USB_PORT_DISABLE0_HYEX), 0x00, PortDisableMap, NULL);
  return TRUE;
}

/**
 * FchTSXhciDisablePortMCM - Disable USB ports on MCM
 *
 *
 * @param[in] Socket         Sock number.
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index 0 or 1.
 * @param[in] USB3DisableMap Bitmap to disable USB3 ports.
 * @param[in] USB2DisableMap Bitmap to disable USB2 ports.
 *
 */
BOOLEAN
FchTSXhciDisablePortMCM (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   USB3DisableMap,
  IN  UINT32   USB2DisableMap
  )
{
  BOOLEAN  Status;
  UINT32   DieAUSB3DisableMap;
  UINT32   DieAUSB2DisableMap;
  UINT32   DieBUSB3DisableMap;
  UINT32   DieBUSB2DisableMap;

  Status = TRUE;
  DieAUSB3DisableMap = USB3DisableMap & 0x3;
  DieAUSB2DisableMap = USB2DisableMap & 0x3;
  DieBUSB3DisableMap = (USB3DisableMap >> 2) & 0x3;
  DieBUSB2DisableMap = (USB2DisableMap >> 2) & 0x3;

  if ((DieAUSB3DisableMap != 0) || (DieAUSB2DisableMap != 0)) {
    Status = FchTSXhciDisablePort (Socket, PhysicalDieId, UsbIndex, DieAUSB3DisableMap, DieAUSB2DisableMap);
  }

  if (!Status) {
    return FALSE;
  }

  if ((DieBUSB3DisableMap != 0) || (DieBUSB2DisableMap != 0)) {
    Status = FchTSXhciDisablePort (Socket, PhysicalDieId, UsbIndex, DieBUSB3DisableMap, DieBUSB2DisableMap);
  }

  return Status;
}

/**
 * FchXhciDisablePortSoc - Disable USB ports on Soc
 *
 *
 * @param[in] Socket         Sock number.
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index 0 or 1.
 * @param[in] USB3DisableMap Bitmap to disable USB3 ports.
 * @param[in] USB2DisableMap Bitmap to disable USB2 ports.
 *
 */
BOOLEAN
FchXhciDisablePortSoc (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   USB3DisableMap,
  IN  UINT32   USB2DisableMap
  )
{
  return FchTSXhciDisablePortMCM (Socket, PhysicalDieId, UsbIndex, USB3DisableMap, USB2DisableMap);
}

/**
 * FchTSXhciOCmapping -  Xhci ports OC mapping
 *
 *
 * @param[in] Socket         Socket number.
 * @param[in] PhysicalDie    Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index 0 or 1.
 * @param[in] Port           Usb Port.
 * @param[in] OCPin          Over Current Pin number.
 *
 */
BOOLEAN
FchTSXhciOCmapping (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT8    Port,
  IN  UINT8    OCPin
  )
{
  UINT8   PortNum;
  UINT8   PortOffset;
  UINT32  DieBusNum;
  UINTN   LogicalDieId;
  UINTN   DieType;

  PortNum = Port;
  if ((PortNum > 7) || (OCPin > 3)) {
    return FALSE;
  }

  if (PortNum > 3) {
    PortNum = PortNum - 4;
  }

  FabricTopologyGetPhysicalIodDieInfo (PhysicalDieId, &LogicalDieId, &DieType);
  DieBusNum  = ReadSocDieBusNum (Socket, LogicalDieId, 0);
  PortOffset = PortNum << 2;

  FchSmnRW (
    DieBusNum,
    USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_PORT_OCMAPPING0_HYEX),
    ~(UINT32)(0xF << PortOffset),
    (UINT32)(OCPin << PortOffset),
    NULL
    );

  return TRUE;
}

/**
 * FchXhciOCmappingSoc -  Xhci ports OC mapping on Soc
 *
 *
 * @param[in] Socket         CPU socket number.DieBusNum.
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       USB index 0 or 1.
 * @param[in] Port           Usb Port.
 * @param[in] OCPin          Over Current Pin number.
 *
 */
BOOLEAN
FchXhciOCmappingSoc (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT8    Port,
  IN  UINT8    OCPin
  )
{
  return FchTSXhciOCmapping (Socket, PhysicalDieId, UsbIndex, Port, OCPin);
}

/**
 * TurnOffUsbPme - Disable USB PME
 *
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] UsbIndex       Usb index 0 or 1.
 * @param[in] DieBusNum      IOHC bus number on current Die.
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
TurnOffUsbPme (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT32                Data32;

  if(PcdGetBool (PcdFchUsbPmeOffSupport) == FALSE || UsbIndex > 1) {
    return;
  }

  FchSmnRead (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_SHADOWED_PMCSR_HYEX), &Data32, NULL);
  DEBUG ((DEBUG_INFO, "TurnOffUsbPme write Bus %x SMN: %x\n", DieBusNum, Data32));

  if(Data32 & FCH_TS_USB_PMCSR_PME_EN) {
    Data32 &= ~FCH_TS_USB_PMCSR_PME_EN;
    FchSmnWrite (DieBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB_SHADOWED_PMCSR_HYEX), &Data32, NULL);
  }
}
