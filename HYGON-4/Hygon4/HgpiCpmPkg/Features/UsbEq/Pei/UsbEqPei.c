/* $NoKeywords:$ */
/**
 * @file
 *
 * HYGON USB TX EQ
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  UsbTxEq
 *
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2022 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 ******************************************************************************
 */
 
#include <HygonCpmPei.h>
#include <HygonCpmBase.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/EqTablePpi.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include "UsbEqPei.h"
#include <TSFch.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/BaseFabricTopologyLib.h>

USB_EQ_INIT_DATE UsbEqInitData = {
  FALSE, 0,                   //USB2 Tx Swing
  FALSE, 0,                   //USB2 Tx Preemphasis
  FALSE, 0,                   //USB3 Tx boost
  FALSE, {0, 0, 0},           //USB3 Gen1 Cursor
  FALSE, {0x2, 0x34, 0x7}     //USB3 Gen2 Cursor
};


/**
 * set usb 2.0 swing
 * @param CpmTablePpi CPM Table PPI
 * @param Socket Socket number
 * @param IodId IodId number
 * @param XhciIndex XhciId number
 * @param Port port number
 * @param Value set value
 * @return VOID
 */
VOID
SetUsb2TxSwing (
  HYGON_CPM_TABLE_PPI  *CpmTablePpi,
  UINT8                Socket,
  UINT8                IodId,
  UINT8                XhciIndex,
  UINT8                Port,
  UINT8                Value
  )
{
  UINT32          LaneCtlSmnAddr=0;
  LANEPARACTL0    LaneCtl;
  UINT32          CpuModel;
  
  CpuModel = GetHygonSocModel();
  
  if (CpuModel == HYGON_EX_CPU) {
    LaneCtlSmnAddr = USB_SPACE_HYEX(
                       XhciIndex, 
                       (FCH_TS_USB_PHY0_LANEPARACTL0_HYEX + Port * 0x400)
                       );
  } else if (CpuModel == HYGON_GX_CPU) {
    LaneCtlSmnAddr = USB_SPACE_HYGX (
                       IodId,
                       XhciIndex,
                       (FCH_TS_USB_PHY0_LANEPARACTL0_HYGX + Port * 0x400)
                       );
  }

  //Dump setting info
  DEBUG ((EFI_D_ERROR, "[Socket%x IodId%x XhciIndex%x Port%x] - SetUsb2TxSwing:", Socket, IodId, XhciIndex, Port));
  DEBUG ((EFI_D_ERROR, " SMN(0x%08x) - Value(0x%08x)\n", LaneCtlSmnAddr, Value));

  LaneCtl.Data32 = CpmTablePpi->CommonFunction.SmnRead32 (CpmTablePpi, Socket, LaneCtlSmnAddr, 0);
  LaneCtl.Reg.TXVREFTUNE = Value;
  CpmTablePpi->CommonFunction.SmnWrite32 (CpmTablePpi, Socket, LaneCtlSmnAddr, 0, LaneCtl.Data32);
  
	return;
}

/**
 * set usb 2.0 preemphasis
 * @param CpmTablePpi CPM Table PPI
 * @param Socket Socket number
 * @param IodId IodId number
 * @param XhciIndex XhciId number
 * @param Port port number
 * @param Value set value
 * @return VOID
 */
VOID
SetUsb2TxPreemphasis (
  HYGON_CPM_TABLE_PPI  *CpmTablePpi,
  UINT8                Socket,
  UINT8                IodId,
  UINT8                XhciIndex,
  UINT8                Port,
  UINT8                Value
  )
{
  UINT32          LaneCtlSmnAddr=0;
  LANEPARACTL0    LaneCtl;
  UINT32          CpuModel;
  
  CpuModel = GetHygonSocModel();
  
  if(CpuModel == HYGON_EX_CPU) {
    LaneCtlSmnAddr = USB_SPACE_HYEX(
                       XhciIndex, 
                       (FCH_TS_USB_PHY0_LANEPARACTL0_HYEX + Port * 0x400)
                       );
  } else if(CpuModel == HYGON_GX_CPU) {
    LaneCtlSmnAddr = USB_SPACE_HYGX (
                       IodId,
                       XhciIndex,
                       (FCH_TS_USB_PHY0_LANEPARACTL0_HYGX + Port * 0x400)
                       );
  
  }
  
  //Dump setting info
  DEBUG ((EFI_D_ERROR, "[Socket%x IodId%x XhciIndex%x Port%x] - SetUsb2TxPreemphasis:", Socket, IodId, XhciIndex, Port));
  DEBUG ((EFI_D_ERROR, " SMN(0x%08x) - Value(0x%08x)\n", LaneCtlSmnAddr, Value));

  LaneCtl.Data32 = CpmTablePpi->CommonFunction.SmnRead32 (CpmTablePpi, Socket, LaneCtlSmnAddr, 0);
  LaneCtl.Reg.TXPREEMPAMPTUNE = Value;
  CpmTablePpi->CommonFunction.SmnWrite32 (CpmTablePpi, Socket, LaneCtlSmnAddr, 0, LaneCtl.Data32);

  return;
}

/**
 * Set USB3 Gen1 TXEQ
 * @param CpmTablePpi CPM Table PPI
 * @param Socket Socket number
 * @param IodId IodId number
 * @param XhciIndex XhciId number
 * @param Port port number
 * @param TxMainCursor TXEQ Main-cursor
 * @param TxPreCursor TXEQ Pre-cursor
 * @param TxPostCursor TXEQ Post-cursor
 * @return VOID
 */
VOID 
SetUsb3Gen1Txeq (
  HYGON_CPM_TABLE_PPI  *CpmTablePpi,
  UINT8                Socket,
  UINT8                IodId,
  UINT8                XhciIndex,
  UINT8                Port,
  UINT8                TxPreCursor,
  UINT8                TxMainCursor,
  UINT8                TxPostCursor
  )
{
  UINT32            UsbTxEq0Addr;
  UINT32            UsbTxEq1Addr;
  PHY_EXT_TX_EQ0    TxEq0;
  PHY_EXT_TX_EQ1    TxEq1;
  UINT32            CpuModel;
  
  CpuModel = GetHygonSocModel();
  
  if (CpuModel == HYGON_EX_CPU) {
    UsbTxEq0Addr = USB_SPACE_HYEX(
                     XhciIndex,
                     FCH_TS_USB_MAP_PHY_BASE_HYEX + Port * 0x400 + PHY_EXT_TX_EQ0_OFFSET
                     );
    UsbTxEq1Addr = USB_SPACE_HYEX(
                     XhciIndex,
                     FCH_TS_USB_MAP_PHY_BASE_HYEX + Port * 0x400 + PHY_EXT_TX_EQ1_OFFSET
                     );
  } else if (CpuModel == HYGON_GX_CPU) {
    UsbTxEq0Addr = USB_SPACE_HYGX (
                     IodId,
                     XhciIndex,
                     FCH_TS_USB_MAP_PHY_BASE_HYGX + Port  * 0x400 + PHY_EXT_TX_EQ0_OFFSET
                     );
    UsbTxEq1Addr = USB_SPACE_HYGX (
                     IodId,
                     XhciIndex,
                     FCH_TS_USB_MAP_PHY_BASE_HYGX + Port  * 0x400 + PHY_EXT_TX_EQ1_OFFSET
                     );
  }
	
  //Dump setting info
  DEBUG ((EFI_D_ERROR, "[Socket%x IodId%x XhciIndex%x Port%x] - SetUsb3Gen1Txeq:\n", Socket, IodId, XhciIndex, Port));
  DEBUG ((EFI_D_ERROR, "  UsbTxEq0Addr(0x%08x) - TxMainCursor(0x%08x)\n", UsbTxEq0Addr, TxMainCursor));
  DEBUG ((EFI_D_ERROR, "  UsbTxEq1Addr(0x%08x) - TxPreCursor(0x%08x) - TxPostCursor(0x%08x)\n", UsbTxEq1Addr, TxPreCursor, TxPostCursor));

  TxEq0.Data32 = CpmTablePpi->CommonFunction.SmnRead32 (CpmTablePpi, Socket, UsbTxEq0Addr, 0);
  TxEq1.Data32 = CpmTablePpi->CommonFunction.SmnRead32 (CpmTablePpi, Socket, UsbTxEq1Addr, 0);

  TxEq0.Reg.PROTOCOL_EXT_TX_EQ_OVRD_G1 = 1;
  TxEq0.Reg.PROTOCOL_EXT_TX_EQ_MAIN_G1 = TxMainCursor;
  TxEq1.Reg.PROTOCOL_EXT_TX_EQ_PRE_G1  = TxPreCursor;
  TxEq1.Reg.PROTOCOL_EXT_TX_EQ_POST_G1 = TxPostCursor;

  CpmTablePpi->CommonFunction.SmnWrite32 (CpmTablePpi, Socket, UsbTxEq0Addr, 0, TxEq0.Data32);
  CpmTablePpi->CommonFunction.SmnWrite32 (CpmTablePpi, Socket, UsbTxEq1Addr, 0, TxEq1.Data32);

  return;
}

/**
 * Set USB3 Gen2 TXEQ
 * @param CpmTablePpi CPM Table PPI
 * @param Socket Socket number
 * @param IodId IodId number
 * @param XhciIndex XhciId number
 * @param Port port number
 * @param TxMainCursor TXEQ Main-cursor
 * @param TxPreCursor TXEQ Pre-cursor
 * @param TxPostCursor TXEQ Post-cursor
 * @return VOID
 */
VOID 
SetUsb3Gen2Txeq (
  HYGON_CPM_TABLE_PPI  *CpmTablePpi,
  UINT8                Socket,
  UINT8                IodId,
  UINT8                XhciIndex,
  UINT8                Port,
  UINT8                TxPreCursor,
  UINT8                TxMainCursor,
  UINT8                TxPostCursor
  )
{
  UINT32            UsbTxEq0Addr;
  UINT32            UsbTxEq1Addr;
  PHY_EXT_TX_EQ0    TxEq0;
  PHY_EXT_TX_EQ1    TxEq1;
  UINT32            CpuModel;
  
  CpuModel     = GetHygonSocModel();
  UsbTxEq0Addr = 0;
  UsbTxEq1Addr = 0;
  
  if (CpuModel == HYGON_EX_CPU) {
    UsbTxEq0Addr = USB_SPACE_HYEX(
                     XhciIndex,
                     FCH_TS_USB_MAP_PHY_BASE_HYEX + Port * 0x400 + PHY_EXT_TX_EQ0_OFFSET
                     );
    UsbTxEq1Addr = USB_SPACE_HYEX(
                     XhciIndex,
                     FCH_TS_USB_MAP_PHY_BASE_HYEX + Port * 0x400 + PHY_EXT_TX_EQ1_OFFSET
                     );
  } else if (CpuModel == HYGON_GX_CPU) {
    UsbTxEq0Addr = USB_SPACE_HYGX (
                     IodId,
                     XhciIndex,
                     FCH_TS_USB_MAP_PHY_BASE_HYGX + Port  * 0x400 + PHY_EXT_TX_EQ0_OFFSET
                     );
    UsbTxEq1Addr = USB_SPACE_HYGX (
                     IodId,
                     XhciIndex,
                     FCH_TS_USB_MAP_PHY_BASE_HYGX + Port  * 0x400 + PHY_EXT_TX_EQ1_OFFSET
                     );
  }
	
  //Dump setting info
  DEBUG ((EFI_D_ERROR, "[Socket%x IodId%x XhciIndex%x Port%x] - SetUsb3Gen2Txeq:\n", Socket, IodId, XhciIndex, Port));
  DEBUG ((EFI_D_ERROR, "  UsbTxEq0Addr(0x%08x) - TxMainCursor(0x%08x)\n", UsbTxEq0Addr + 0, TxMainCursor));
  DEBUG ((EFI_D_ERROR, "  UsbTxEq1Addr(0x%08x) - TxPreCursor(0x%08x) - TxPostCursor(0x%08x)\n", UsbTxEq1Addr, TxPreCursor, TxPostCursor));

  TxEq0.Data32 = CpmTablePpi->CommonFunction.SmnRead32 (CpmTablePpi, Socket, UsbTxEq0Addr, 0);
  TxEq1.Data32 = CpmTablePpi->CommonFunction.SmnRead32 (CpmTablePpi, Socket, UsbTxEq1Addr, 0);

  TxEq0.Reg.PROTOCOL_EXT_TX_EQ_OVRD_G2 = 1;
  TxEq0.Reg.PROTOCOL_EXT_TX_EQ_MAIN_G2 = TxMainCursor;
  TxEq1.Reg.PROTOCOL_EXT_TX_EQ_PRE_G2  = TxPreCursor;
  TxEq1.Reg.PROTOCOL_EXT_TX_EQ_POST_G2 = TxPostCursor;

  CpmTablePpi->CommonFunction.SmnWrite32 (CpmTablePpi, Socket, UsbTxEq0Addr, 0, TxEq0.Data32);
  CpmTablePpi->CommonFunction.SmnWrite32 (CpmTablePpi, Socket, UsbTxEq1Addr, 0, TxEq1.Data32);

  return;
}

/**
 * Set USB3 TX vboost
 * @param CpmTablePpi CPM Table PPI
 * @param Socket Socket number
 * @param IodId IodId number
 * @param XhciIndex XhciId number
 * @param Port port number
 * @param BoostLvl boost level
 * @return VOID
 */
VOID 
SetUsb3TxVboost (
  HYGON_CPM_TABLE_PPI   *CpmTablePpi,
  UINT8                 Socket,
  UINT8                 IodId,
  UINT8                 XhciIndex,
  UINT8                 Port,
  UINT8                 BoostLvl
  )
{
  UINT32                PhySelectAddr;
  UINT32                SupDigLvlOverInAddr;
  SUP_DIG_LVL_OVER_IN   SupDigLvlOverIn;
  UINT32                CpuModel;
  
  CpuModel = GetHygonSocModel();
  
  if (CpuModel == HYGON_EX_CPU) {
    PhySelectAddr       = USB_SPACE_HYEX(
                            XhciIndex,
                            PHY_CR_SELECT_HYEX
                            );
    SupDigLvlOverInAddr = USB_SPACE_HYEX(
                            XhciIndex,
                            FCH_TS_USB_PHY_BASE_HYEX + SUP_DIG_LVL_OVER_IN_OFFSET * 4
                            );
  } else if (CpuModel == HYGON_GX_CPU) {
    PhySelectAddr       = USB_SPACE_HYGX (IodId, XhciIndex, PHY_CR_SELECT_HYGX);
    SupDigLvlOverInAddr = USB_SPACE_HYGX (IodId, XhciIndex, (FCH_TS_USB_PHY_BASE_HYGX + SUP_DIG_LVL_OVER_IN_OFFSET * 4));
  }

  //Dump setting info
  DEBUG ((EFI_D_ERROR, "[Socket%x IodId%x XhciIndex%x Port%x] - SetUsb3TxVboost:", Socket, IodId, XhciIndex, Port));
  DEBUG ((EFI_D_ERROR, " SMN(0x%08x) - BoostLvl(0x%08x)\n", SupDigLvlOverInAddr, BoostLvl));

  //select port
  CpmTablePpi->CommonFunction.SmnWrite32 (CpmTablePpi, Socket, PhySelectAddr, 0, Port);
	
  //set boost level
  SupDigLvlOverIn.Data16 = CpmTablePpi->CommonFunction.SmnRead16 (CpmTablePpi, Socket, SupDigLvlOverInAddr, 0);

  SupDigLvlOverIn.Reg.TX_VBOOST_LVL_EN = 1;
  SupDigLvlOverIn.Reg.TX_VBOOST_LVL    = BoostLvl;

  CpmTablePpi->CommonFunction.SmnWrite16 (CpmTablePpi, Socket, SupDigLvlOverInAddr, 0, SupDigLvlOverIn.Data16);

  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the Usb Eq
 *
 * This function to tune Usb Eq
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
UsbEqPeiEntry (
  IN CPM_PEI_FILE_HANDLE           FileHandle,
  IN CPM_PEI_SERVICES              **PeiServices
  )
{
  EFI_STATUS                              Status;
  UINT8                                   i, j, k, m;
  HYGON_CPM_EQ_CONFIG_TABLE               *EqCfgTable;
  EFI_PEI_EQ_TABLE_PPI                    *EqTablePpi;
  UINT8                                   Socket;
  UINT8                                   IodId;
  UINT8                                   Nbio;
  UINT8                                   Port;	
  UINT8                                   UsbEqCfgData0;
  UINT8                                   UsbEqCfgData1;
  UINT8                                   UsbEqCfgData2;
  HYGON_CPM_TABLE_PPI                     *CpmTablePpi;
  HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI  *FabricTopologyPpi;
  UINTN                                   NumberOfSockets;
  UINTN                                   TotalNumberOfIoNbio;
  UINTN                                   TotalNumberRootBridges;
  UINT32                                  CpuModel;
  UINT8                                   IodCount;
  UINT8                                   XhciCount;
  UINT8                                   UsbPortCount;
  UINT8                                   XhciIndex;

  Status = (*PeiServices)->LocatePpi (
                             (CPM_PEI_SERVICES**)PeiServices,
                             &gHygonCpmTablePpiGuid,
                             0,
                             NULL,
                             (VOID**)&CpmTablePpi
                             );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //1.init Usb EQ setting by Usb Eq init table
  Status = (*PeiServices)->LocatePpi (
                             (CPM_PEI_SERVICES**)PeiServices,
                             &gHygonFabricTopologyServicesPpiGuid,
                             0,
                             NULL,
                             (VOID**)&FabricTopologyPpi
                             );
  if (!EFI_ERROR(Status)) {
    DEBUG ((EFI_D_INFO, "CpmSetUsbEq INIT Setting...\n"));         // byo230914 -
    FabricTopologyPpi->GetSystemInfo (&NumberOfSockets, &TotalNumberOfIoNbio, &TotalNumberRootBridges);
    
    CpuModel = GetHygonSocModel();
    if (CpuModel == HYGON_EX_CPU) {
      IodCount     = (UINT8)FabricTopologyGetNumberOfPhysicalDiesOnSocket(0);
      XhciCount    = USB_NUMBER_PER_SATORI;
      UsbPortCount = USB_PORT_NUMBER_PER_NBIO_OF_SATORI;
    } else if (CpuModel == HYGON_GX_CPU) {
      IodCount     = PcdGet8(PcdHygonDujiangDieCountPerProcessor);
      XhciCount    = USB_NUMBER_PER_DUJIANG;
      UsbPortCount = USB_PORT_NUMBER_PER_DJ;
    }
    
    for (i = 0; i < NumberOfSockets; i++) {      
      for (j = 0; j < IodCount; j++) {
        for (k = 0; k < XhciCount; k++) {
          for (m = 0; m < UsbPortCount; m++) {
            if (UsbEqInitData.InitTxSwing) {
              SetUsb2TxSwing (CpmTablePpi, i, j, k, m, UsbEqInitData.Usb2TxSwing);
            }
            if (UsbEqInitData.InitTxPreemphasis) {
              SetUsb2TxPreemphasis (CpmTablePpi, i, j, k, m, UsbEqInitData.Usb2TxPreemphasis);
            }
            if (UsbEqInitData.InitTxVboost) {
              SetUsb3TxVboost (CpmTablePpi, i, j, k, m, UsbEqInitData.Usb3TxVboost);
            }
            if (UsbEqInitData.InitGen1Cursor) {
              SetUsb3Gen1Txeq (CpmTablePpi, i, j, k, m, UsbEqInitData.Usb3Gen1Cursor.Main, UsbEqInitData.Usb3Gen1Cursor.Pre, UsbEqInitData.Usb3Gen1Cursor.Post);
            }
            if (UsbEqInitData.InitGen2Cursor) {
              SetUsb3Gen2Txeq (CpmTablePpi, i, j, k, m, UsbEqInitData.Usb3Gen2Cursor.Main, UsbEqInitData.Usb3Gen2Cursor.Pre, UsbEqInitData.Usb3Gen2Cursor.Post);
            }          
          }//port loop
        }//Xhci loop
      }//IodId loop
    }//socket loop
  }
  
  //2.Get HSIO Eq Configuration table
  Status = (*PeiServices)->LocatePpi (
                              PeiServices,
                              &gEqTablePpiGuid,
                              0,
                              NULL,
                              (VOID**)&EqTablePpi
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = EqTablePpi->GetCfgTable (PeiServices, &EqCfgTable);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //3.Do Usr EQ Configuration
  DEBUG ((EFI_D_INFO, "CpmSetUsbEq User Setting...\n"));           // byo230914 -
  for (i = 0; (EqCfgTable->EqCfgList[i].Flag & EQ_TABLE_END) != EQ_TABLE_END; i++) {
    if (((EqCfgTable->EqCfgList[i].Flag & FLAG_USED) != FLAG_USED) &&
        ((EqCfgTable->EqCfgList[i].Flag & FLAG_THROW) != FLAG_THROW) &&
        ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) == USB_EQ_TYPE))  
    {
      Socket            = EqCfgTable->EqCfgList[i].Socket;
      IodId             = EqCfgTable->EqCfgList[i].PhysicalIodId;
      Nbio              = EqCfgTable->EqCfgList[i].Nbio;
      Port              = EqCfgTable->EqCfgList[i].UsbPort;
      UsbEqCfgData0     = EqCfgTable->EqCfgList[i].UsbCfgValue[0];
      UsbEqCfgData1     = EqCfgTable->EqCfgList[i].UsbCfgValue[1];
      UsbEqCfgData2     = EqCfgTable->EqCfgList[i].UsbCfgValue[2];
      XhciIndex         = Nbio / 2;
      switch (EqCfgTable->EqCfgList[i].Type) {
        case USB2_TX_SWING:
          SetUsb2TxSwing (CpmTablePpi, Socket, IodId, XhciIndex, Port, UsbEqCfgData0);
          break;
        case USB2_TX_PREEMPHASIS:
          SetUsb2TxPreemphasis (CpmTablePpi, Socket, IodId, XhciIndex, Port, UsbEqCfgData0);
          break;
        case USB3_TX_VBOOST:
          SetUsb3TxVboost (CpmTablePpi, Socket, IodId, XhciIndex, Port, UsbEqCfgData0);
          break;
        case USB3_GEN1_TX_EQ_LEVEL:
          SetUsb3Gen1Txeq (CpmTablePpi, Socket, IodId, XhciIndex, Port, UsbEqCfgData0, UsbEqCfgData1, UsbEqCfgData2);
          break;
        case USB3_GEN2_TX_EQ_LEVEL:
          SetUsb3Gen2Txeq (CpmTablePpi, Socket, IodId, XhciIndex, Port, UsbEqCfgData0, UsbEqCfgData1, UsbEqCfgData2);
          break;
        default:
          break;    
      }
    }
  }

  DEBUG ((EFI_D_INFO, "Hygon Cpm Usb Tx Eq Exit\n"));              // byo230914 -
  return EFI_SUCCESS;
}