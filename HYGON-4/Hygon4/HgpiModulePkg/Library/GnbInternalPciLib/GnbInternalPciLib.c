/* $NoKeywords:$ */

/**
 * @file
 *
 * Various PCI service routines.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: GNB
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
* ***************************************************************************
*
*/
#include  <HYGON.h>
#include  <Gnb.h>
#include  <Filecode.h>
#include  <Library/HygonBaseLib.h>
#include  <Library/GnbPciAccLib.h>
#include  <Library/GnbPciLib.h>
#include  <Library/GnbLib.h>
#include  <GnbRegisters.h>
#include  <Library/NbioRegisterAccLib.h>
#include  <Library/IoLib.h>
#include  <Library/PcieConfigLib.h>
#include  <Library/TimerLib.h>
#include  <Library/BaseLib.h>
#include  <HYGON.h>
#include  <Library/HygonSmnAddressLib.h>

#define FILECODE  LIBRARY_GNBPCILIB_GNBPCILIB_FILECODE

#define PCI_CFG_SUB_BUS_NUMBER_LATENCY_OFT  0x18
#define PCIE_CFG_DEVICE_CNTL_AND_STATUS_OFT 0x60
#define PCIE_CFG_LINK_CAP_OFT               0x64
#define PCIE_CFG_LINK_CNTL_OFT              0x68
#define PCIE_CFG_LINK_STATUS_OFT            0x68
#define PCIE_CFG_LINK_CNTL2_OFT             0x88
#define PCIE_CFG_AER_UCE_STATUS_OFT         0x154
#define PCIE_CFG_AER_CE_STATUS_OFT          0x160
#define PCIE_CFG_ROOT_ERROR_STATUS_OFT      0x180
#define PCIE_CFG_LINK_CNTL3_OFT             0x274
#define PCIE_CFG_LANE_ERROR_STATUS_OFT      0x278

CHAR8* LtssmState[] = {
  "Detect_Quiet",               //0x00
  "Start_Common_Mode",          //0x01
  "Check_Common_Mode",          //0x02
  "Rcvr_Detect",                //0x03
  "No_Rcvr_Loop",               //0x04
  "Poll_Quiet",                 //0x05
  "Poll_Active",                //0x06
  "Poll_Compliance",            //0x07
  "Poll_Config",                //0x08
  "Config_Step1",               //0x09
  "Config_Step3",               //0x0A
  "Config_Step5",               //0x0B
  "Config_Step2",               //0x0C
  "Config_Step4",               //0x0D
  "Config_Step6",               //0x0E
  "Config_Idle",                //0x0F
  "Rcv_L0_and_Tx_L0",           //0x10
  "Rcv_L0_and_Tx_L0_Idle",      //0x11
  "Rcv_L0_and_Tx_L0s",          //0x12
  "Rcv_L0_and_Tx_L0s_FTS",      //0x13
  "Rcv_L0s_and_Tx_L0",          //0x14
  "Rcv_L0s_and_Tx_L0_Idle",     //0x15
  "Rcv_L0s_and_Tx_L0s",         //0x16
  "Rcv_L0s_and_Tx_L0s_FTS",     //0x17
  "L1_Entry",                   //0x18
  "L1_Idle",                    //0x19
  "L1_Wait",                    //0x1A
  "L1",                         //0x1B
  "L23_Stall",                  //0x1C
  "L23_Entry",                  //0x1D
  "L23_Idle",                   //0x1E
  "L23_Ready",                  //0x1F
  "Recovery_Lock",              //0x20
  "Recovery_Config",            //0x21
  "Recovery_Idle",              //0x22
  "Training_Bit",               //0x23
  "Rcvd_Loopback",              //0x24
  "Rcvd_Loopback_Idle",         //0x25
  "Rcvd_Reset_Idle",            //0x26
  "Rcvd_Disable_Entry",         //0x27
  "Rcvd_Disable_Idle",          //0x28
  "Rcvd_Disable",               //0x29
  "Detect_Idle",                //0x2A
  "L23_Wait",                   //0x2B
  "Rcv_L0s_Skp_and_Tx_L0",      //0x2C
  "Rcv_L0s_Skp_and_Tx_L0_Idle", //0x2D
  "Rcv_L0s_Skp_and_Tx_L0s",     //0x2E
  "Rcv_L0s_Skp_and_Tx_L0s_FTS", //0x2F
  "Config_Step2b",              //0x30
  "Recovery_Speed",             //0x31
  "Poll_Compliance_Idle",       //0x32
  "Rcvd_Loopback_Speed",        //0x33
  "Recovery_Eq0",               //0x34
  "Recovery_Eq1",               //0x35
  "Recovery_Eq2",               //0x36
  "Recovery_Eq3",               //0x37
  "L1_1",                       //0x38
  "L1_2_Entry",                 //0x39
  "L1_2_Idle",                  //0x3A
  "L1_2_Exit",                  //0x3B
  "Reserved_3C",                //0x3C
  "Reserved_3D",                //0x3D
  "Reserved_3E",                //0x3E
  "Illegal_State"               //0x3F
};

/**
 * Root bridge enum Down stream device
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[out] DsBus          Pointer to DsBus
 * 
 * @retval
 */
UINT32
RbEnumDownStreamDev (
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *Engine,
  OUT    UINT8                  *DsBus
  )
{
  UINT32                        OldValue, NewValue;
  UINT8                         PrimaryBus, SecondaryBus, SubBusNum;
  UINT32                        Address;

  Address = MAKE_SBDFO (
              0, 
              GnbHandle->Address.Address.Bus, 
              Engine->Type.Port.NativeDevNumber, 
              Engine->Type.Port.NativeFunNumber, \
              PCI_CFG_SUB_BUS_NUMBER_LATENCY_OFT
                 );
  
  //save old sub bus config
  GnbLibPciRead (Address, AccessWidth32, &OldValue, NULL);
  //allocate bus for DS
  PrimaryBus   = (UINT8)GnbHandle->Address.Address.Bus;
  SecondaryBus = (UINT8)(GnbHandle->Address.Address.Bus + 1);
  *DsBus       = SecondaryBus;
  SubBusNum    = (UINT8)(GnbHandle->Address.Address.Bus + 1);
  NewValue = 0;
  NewValue |= PrimaryBus;
  NewValue |= (SecondaryBus << 8);
  NewValue |= (SubBusNum << 16);
  GnbLibPciWrite (Address, AccessWidth32, &OldValue, NULL);
  
  return OldValue;
}

/**
 * Root bridge Unenum Down stream device
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[out] OldRegValue    Old Register Value
 * 
 * @retval
 */
VOID
RbUnEnumDownStreamDev (
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *Engine,
  IN     UINT32                 OldRegValue
  )
{
  UINT32                        Address;
  
  Address = MAKE_SBDFO (
              0, 
              GnbHandle->Address.Address.Bus, 
              Engine->Type.Port.NativeDevNumber, 
              Engine->Type.Port.NativeFunNumber, \
              PCI_CFG_SUB_BUS_NUMBER_LATENCY_OFT
                 );
  GnbLibPciWrite (Address, AccessWidth32, &OldRegValue, NULL);
  
}

/**
 * Get MMIO Configuration Base address
 * 
 * @retval MMIO Configuration Base address
 */
UINT32 GetMmCfg()
{
  return (UINT32)(AsmReadMsr64(MSR_MM_CFG) & 0xfff00000);
}

/**
 * PCIe Perform EqualizationSetting 
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  PerformEq      TRUE = Perform EqualizationSetting , FALSE = Don't Perform EqualizationSetting 
 * 
 * @retval    Old Perform EqualizationSetting status
 */
BOOLEAN
PciePerformEqualizationSetting (
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *Engine,
  IN     BOOLEAN                PerformEq
  )
{

  PCIE_LINK_CNTL3_STRUCT        LinkCtl3;
  UINT32                        DoEq;
  BOOLEAN                       OldPerformEqStatus;
  UINT32                        Address;

  Address = MAKE_SBDFO (
              0, 
              GnbHandle->Address.Address.Bus, 
              Engine->Type.Port.NativeDevNumber, 
              Engine->Type.Port.NativeFunNumber, \
              PCIE_CFG_LINK_CNTL3_OFT
                 );
  
  if (PerformEq) {
    DoEq = 1;
  } else {
    DoEq = 0;
  }
  
  GnbLibPciRead (Address, AccessWidth32, &LinkCtl3.Value, NULL);
  OldPerformEqStatus = (LinkCtl3.Field.PERFORM_EQUALIZATION == 1) ? TRUE : FALSE;
  LinkCtl3.Field.PERFORM_EQUALIZATION = DoEq;
  GnbLibPciWrite (Address, AccessWidth32, &LinkCtl3.Value, NULL);
  
  return OldPerformEqStatus;
}

/**
 * PCIe Retraining
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  ForceEq        TRUE = Retraining with EQ, FALSE = Retraining without EQ
 * 
 * @retval    void
 */
VOID
PcieRetraining (
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *Engine,
  IN     BOOLEAN                ForceEq
  )
{
  PCIE_LINK_CNTL_STRUCT         LinkCtl;
  BOOLEAN                       OldPerformEqStatus;
  UINT32                        Address;

  Address = MAKE_SBDFO (
              0, 
              GnbHandle->Address.Address.Bus, 
              Engine->Type.Port.NativeDevNumber, 
              Engine->Type.Port.NativeFunNumber, \
              PCIE_CFG_LINK_CNTL_OFT
                 );

    
  OldPerformEqStatus = PciePerformEqualizationSetting (GnbHandle, Engine, ForceEq);
  
  //force retrain link
  GnbLibPciRead (Address, AccessWidth32, &LinkCtl.Value, NULL);
  LinkCtl.Field.RETRAIN_LINK =1;
  GnbLibPciWrite (Address, AccessWidth32, &LinkCtl.Value, NULL);
  
  if (ForceEq) {
    //delay 50ms
    MicroSecondDelay (50*1000);
  }

  PciePerformEqualizationSetting (GnbHandle, Engine, OldPerformEqStatus);
}

/**
 * PCIe Link Speed change
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  NewSpeed       Link Speed change value
 * @param[in]  OldSpeed       Old Link Speed value
 * 
 * @retval    VOID
 */
VOID
PcieSpeedChange (
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *Engine,
  IN     UINT8                  NewSpeed,
  OUT    UINT8                  *OldSpeed
  )
{
  PCIE_LINK_CNTL2_STRUCT        LinkCtl2;
  PCIE_LINK_CNTL_STRUCT         LinkCtl;
  PCIE_LINK_STATUS_STRUCT       LinkStatus;
  UINT32                        Address;
  
  //read current link speed
  if (OldSpeed != NULL) {
    Address = MAKE_SBDFO (
                0, 
                GnbHandle->Address.Address.Bus, 
                Engine->Type.Port.NativeDevNumber, 
                Engine->Type.Port.NativeFunNumber, \
                PCIE_CFG_LINK_STATUS_OFT
                   ); 
    GnbLibPciRead (Address, AccessWidth32, &LinkStatus.Value, NULL);
    *OldSpeed = (UINT8)LinkStatus.Field.CURRENT_LINK_SPEED;
  }
  
  //speed change to NewSpeed
  Address = MAKE_SBDFO (
            0, 
            GnbHandle->Address.Address.Bus, 
            Engine->Type.Port.NativeDevNumber, 
            Engine->Type.Port.NativeFunNumber, \
            PCIE_CFG_LINK_CNTL2_OFT
                 );
  GnbLibPciRead (Address, AccessWidth32, &LinkCtl2.Value, NULL);
  LinkCtl2.Field.TARGET_LINK_SPEED = NewSpeed;
  GnbLibPciWrite (Address, AccessWidth32, &LinkCtl2.Value, NULL);
  
  //retraining link
  Address = MAKE_SBDFO (
            0, 
            GnbHandle->Address.Address.Bus, 
            Engine->Type.Port.NativeDevNumber, 
            Engine->Type.Port.NativeFunNumber, \
            PCIE_CFG_LINK_CNTL_OFT
                 );
  GnbLibPciRead (Address, AccessWidth32, &LinkCtl.Value, NULL);
  LinkCtl.Field.RETRAIN_LINK = 1;
  GnbLibPciWrite (Address, AccessWidth32, &LinkCtl.Value, NULL);
}

/**
 * PCIe Other Side Supported Speed
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe Other Side Supported Speed
 */
UINT8
PcieDsSupportedSpeed (    
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *PcieEngine
  )
{
  PCIE_LC_CNTL_10               LcCntl10;
  PCIE_LC_SPEED_CNTL_STRUCT     LcSpeed;
  UINT32                        SmnAddress;
  UINT32                        Data;

  SmnAddress = ConvertPciePortAddress2 (
                 NBIO_SPACE3(GnbHandle->PhysicalDieId, GnbHandle->RbId, PCIE0_REG_TYPE, PCIE0_LC_SPEED_STATUS_ADDRESS_HYGX & 0xFFF00000),
                 GnbHandle,
                 PcieEngine
                 );
  
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &Data, 0);
  if ((Data & BIT23) != 0) {
    return 5;
  } 

  SmnAddress = ConvertPciePortAddress2 (
                 NBIO_SPACE3(GnbHandle->PhysicalDieId, GnbHandle->RbId, PCIE0_REG_TYPE, PCIE0_LC_CNTL_10_ADDRESS_HYGX & 0xFFF00000),
                 GnbHandle,
                 PcieEngine
                 );
  
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LcCntl10.Value, 0);
  
  if (LcCntl10.Field.LC_OTHER_SIDE_SUPPORTS_GEN4 == 1)
    return 4;
  
  SmnAddress = ConvertPciePortAddress2 (
                 NBIO_SPACE3(GnbHandle->PhysicalDieId, GnbHandle->RbId, PCIE0_REG_TYPE, PCIE0_LC_SPEED_CNTL_ADDRESS_HYGX & 0xFFF00000),
                 GnbHandle,
                 PcieEngine
                 );
  
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LcSpeed.Value, 0);
  
  if (LcSpeed.Field.LC_OTHER_SIDE_SUPPORTS_GEN3 == 1)
    return 3;
  
  if (LcSpeed.Field.LC_OTHER_SIDE_SUPPORTS_GEN2 == 1)
    return 2;
  
  return 1;
  
}

/**
 * PCIe Current Link Speed
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe Current Link Speed
 */
UINT32
PcieCurrentLinkSpeed (
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *PcieEngine
  )
{
  PCIE_LINK_STATUS_STRUCT       LinkStatus;
  UINT32                        Address;

  Address = MAKE_SBDFO (
              0, 
              GnbHandle->Address.Address.Bus, 
              PcieEngine->Type.Port.NativeDevNumber, 
              PcieEngine->Type.Port.NativeFunNumber, \
              PCIE_CFG_LINK_STATUS_OFT
                 );

  GnbLibPciRead (Address, AccessWidth32, &LinkStatus.Value, NULL);
  
  return LinkStatus.Field.CURRENT_LINK_SPEED;
}

/**
 * PCIe Current Link States
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe Current Link Width
 */
UINT32
PcieCurrentLinkState (
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *PcieEngine       
)
{
  PCIE_LINK_STATUS_STRUCT       LinkStatus;
  UINT32                        Address;

  Address = MAKE_SBDFO (
              0, 
              GnbHandle->Address.Address.Bus, 
              PcieEngine->Type.Port.NativeDevNumber, 
              PcieEngine->Type.Port.NativeFunNumber, \
              PCIE_CFG_LINK_STATUS_OFT
                 );
  
  GnbLibPciRead (Address, AccessWidth32, &LinkStatus.Value, NULL);
  
  return LinkStatus.Value;
}

/**
 * PCIe Current Link Width
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe Current Link Width
 */
UINT32
PcieCurrentLinkWidth (
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *PcieEngine
  )
{
  PCIE_LINK_STATUS_STRUCT       LinkStatus;
  UINT32                        Address;

  Address = MAKE_SBDFO (
              0, 
              GnbHandle->Address.Address.Bus, 
              PcieEngine->Type.Port.NativeDevNumber, 
              PcieEngine->Type.Port.NativeFunNumber, \
              PCIE_CFG_LINK_STATUS_OFT
                 );
  
  GnbLibPciRead (Address, AccessWidth32, &LinkStatus.Value, NULL);
  
  return LinkStatus.Field.NEGOTIATED_LINK_WIDTH;
}

/**
 * PCIe Link Speed Cap
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe Link Speed Cap
 */
UINT32
PcieCapLinkSpeed (
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *PcieEngine
  )
{
  PCIE_LINK_CAP_STRUCT          LinkCap;
  UINT32                        Address;

  Address = MAKE_SBDFO (
              0, 
              GnbHandle->Address.Address.Bus, 
              PcieEngine->Type.Port.NativeDevNumber, 
              PcieEngine->Type.Port.NativeFunNumber, \
              PCIE_CFG_LINK_CAP_OFT
                 );

  GnbLibPciRead (Address, AccessWidth32, &LinkCap.Value, NULL);
  
  return LinkCap.Field.LINK_SPEED;
}

/**
 * PCIe Link Width Cap
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe Link Width Cap
 */
UINT32
PcieCapLinkWidth (
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *PcieEngine
  )
{
  PCIE_LINK_CAP_STRUCT          LinkCap;
  UINT32                        Address;

  Address = MAKE_SBDFO (
              0, 
              GnbHandle->Address.Address.Bus, 
              PcieEngine->Type.Port.NativeDevNumber, 
              PcieEngine->Type.Port.NativeFunNumber, \
              PCIE_CFG_LINK_CAP_OFT
                 );

  GnbLibPciRead (Address, AccessWidth32, &LinkCap.Value, NULL);
  
  return LinkCap.Field.LINK_WIDTH;
}

/**
 * PCIe DL Activate status
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe DL Activate status
 */
BOOLEAN
PcieDlActivate (
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *PcieEngine
  )
{
  PCIE_LINK_STATUS_STRUCT       LinkStatus;
  UINT32                        Address;

  Address = MAKE_SBDFO (
              0, 
              GnbHandle->Address.Address.Bus, 
              PcieEngine->Type.Port.NativeDevNumber, 
              PcieEngine->Type.Port.NativeFunNumber, \
              PCIE_CFG_LINK_STATUS_OFT
                 );

  GnbLibPciRead (Address, AccessWidth32, &LinkStatus.Value, NULL);
  
  return (LinkStatus.Field.DL_ACTIVE == 1) ? TRUE : FALSE;
}

/**
 * PCIe Wait to L0
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  MaxDelayMs     Max Delay Times to L0(ms)
 * 
 * @retval    TURE  PCIe wait L0 success
 *            FALSE PCIe wait L0 fail
 */
BOOLEAN
PcieWaitToL0 (
  IN     GNB_HANDLE              *GnbHandle,
  IN     PCIe_ENGINE_CONFIG      *Engine,
  IN     UINT32                  MaxDelayMs
  )
{
  UINT32                         i;
  BOOLEAN                        WaitStatus = FALSE;
  
  for (i = 0; i < MaxDelayMs; i++) {
    MicroSecondDelay (1000);
    if (PcieDlActivate (GnbHandle, Engine)) {
      WaitStatus = TRUE;
      break;
    }  
  }
  
  if (WaitStatus) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Wait to L0 Success, Wait %d ms\n", i);
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "Wait to L0 Fail, Time out\n", i);
  }
  
  return WaitStatus;
}

/**
 * PCIe Retry to L0
 *
 * @param[in]  GnbHandle            Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine               Pointer to engine config descriptor
 * @param[in]  RetryCount           Retry Count
 * @param[in]  MaxDelayMsPerTry     Max Delay Times to L0(ms) PreTry
 * 
 * @retval    TURE  PCIe Retry to L0 success
 *            FALSE PCIe Retry to L0 fail
 */
BOOLEAN
PcieRetryToL0 (
  IN     GNB_HANDLE              *GnbHandle,
  IN     PCIe_ENGINE_CONFIG      *Engine,
  IN     UINT32                  RetryCount,
  IN     UINT32                  MaxDelayMsPerTry
  )
{
  UINT32                         i;
  BOOLEAN                        RetryStatus = FALSE;
  
  for (i = 0; i < RetryCount; i++) {
    //Reqo EQ
    PciePerformEqualizationSetting (GnbHandle, Engine, TRUE);
    //wait to L0
    if (PcieWaitToL0 (GnbHandle, Engine, MaxDelayMsPerTry)) {
      RetryStatus = TRUE;
      break;
    }
  }
  
  if (RetryStatus) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Retry to L0 Success, Retry count = %d\n", i);
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "Retry to L0 Fail, Retry count = %d\n", i);
  }
  
  return RetryStatus;
}
/**
 * PCIe Link Speed Warning
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe Link Speed Warning status
 */
BOOLEAN
PcieLinkSpeedWarning(
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *PcieEngine
  )
{
  BOOLEAN                       SpeedWarning = FALSE;
  UINT32                        CurrentSpeed; 
  UINT32                        CapSpeed;
  UINT32                        TargetSupportedSpeed;

  CurrentSpeed = PcieCurrentLinkSpeed (GnbHandle, PcieEngine);
  //GET TARGET SUPPORTED SPEED
  TargetSupportedSpeed = PcieDsSupportedSpeed (GnbHandle, PcieEngine);
  //GET CAP SPEED
  CapSpeed = PcieCapLinkSpeed (GnbHandle, PcieEngine);
  
  if (CapSpeed < TargetSupportedSpeed) {
    if (CapSpeed > CurrentSpeed) {
      SpeedWarning = TRUE;
    } else {
      SpeedWarning = FALSE;
    }
  } else {
    if (TargetSupportedSpeed > CurrentSpeed) {
      SpeedWarning = TRUE;
    } else {
      SpeedWarning = FALSE;
    }  
  }
  
  return SpeedWarning;
}

/**
 * DUMP PCIe LTSSM States
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    VOID
 */
VOID
DumpLtssm (
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *PcieEngine
  )
{
  UINT32                        SmnAddress;
  UINT32                        Data32;
  UINT32                        i;
  UINT8                         LcCurrentState, LcPrevState1, LcPrevState2, LcPrevState3;

  IDS_HDT_CONSOLE (
    GNB_TRACE, 
    "PCIe Core%d Port%d LTSSM:\n", 
    (PcieEngine->Type.Port.PortId / 8), 
    (PcieEngine->Type.Port.PortId % 8)
    );

  for (i = 6; i != 0; i--) {
    SmnAddress = ConvertPciePortAddress2 (
                   NBIO_SPACE3(GnbHandle->PhysicalDieId, GnbHandle->RbId, PCIE0_REG_TYPE, ((PCIE0_LC_STATE0_ADDRESS_HYGX & 0xFFF00000) + (i - 1) * 4)),
                   GnbHandle,
                   PcieEngine
                   );
    NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &Data32, 0);
    LcCurrentState = (UINT8)(Data32 & 0xFF);
    LcPrevState1   = (UINT8)((Data32 >> 8) & 0xFF);
    LcPrevState2   = (UINT8)((Data32 >> 16) & 0xFF);
    LcPrevState3   = (UINT8)((Data32 >> 24) & 0xFF);
    IDS_HDT_CONSOLE (GNB_TRACE, "LC_STATE_%d: [%a(%x)] -> [%a(%x)] -> [%a(%x)] -> [%a(%x)]\n",   // byo231007 -
      (i - 1), 
      LcPrevState3 >= ARRAY_SIZE(LtssmState) ? "?" : LtssmState[LcPrevState3],        // byo231007 -
      LcPrevState3,                                                                   // byo231007 +
      LcPrevState2 >= ARRAY_SIZE(LtssmState) ? "?" : LtssmState[LcPrevState2],        // byo231007 -
      LcPrevState2,                                                                   // byo231007 +
      LcPrevState1 >= ARRAY_SIZE(LtssmState) ? "?" : LtssmState[LcPrevState1],        // byo231007 -
      LcPrevState1,                                                                   // byo231007 +
      LcCurrentState >= ARRAY_SIZE(LtssmState) ? "?" : LtssmState[LcCurrentState],    // byo231007 -
      LcCurrentState                                                                  // byo231007 +
      );
  }
}

/**
 * PCIe Clear All errors
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    VOID
 */
VOID
PcieClearAllErrors (
  IN     GNB_HANDLE             *GnbHandle,
  IN     PCIe_ENGINE_CONFIG     *Engine
  )
{
  PCIe_WRAPPER_CONFIG           *PcieWrapper;
  UINT8                         MappingPortID;
  UINT32                        Data32;
  UINT32                        Address;

  PcieWrapper   = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  IDS_HDT_CONSOLE (GNB_TRACE, "Pcie Clear All Errors for node%x - core%x - port%x\n", GnbHandle->InstanceId, PcieWrapper->WrapId , MappingPortID);
  // 1. clear CE/UCE/Root Errors
  Address = MAKE_SBDFO (
            0, 
            GnbHandle->Address.Address.Bus, 
            Engine->Type.Port.NativeDevNumber, 
            Engine->Type.Port.NativeFunNumber, \
            PCIE_CFG_AER_UCE_STATUS_OFT
            );

  GnbLibPciRead (Address, AccessWidth32, &Data32, NULL);
  IDS_HDT_CONSOLE (GNB_TRACE, "Before Clear: UCE status=%x\n", Data32);
  GnbLibPciRead (Address, AccessWidth32, &Data32, NULL);
  
  Address = MAKE_SBDFO (
            0, 
            GnbHandle->Address.Address.Bus, 
            Engine->Type.Port.NativeDevNumber, 
            Engine->Type.Port.NativeFunNumber, \
            PCIE_CFG_AER_CE_STATUS_OFT
            );

  GnbLibPciRead (Address, AccessWidth32, &Data32, NULL);
  IDS_HDT_CONSOLE (GNB_TRACE, "Before Clear: CE status=%x\n", Data32);
  GnbLibPciRead (Address, AccessWidth32, &Data32, NULL);

  Address = MAKE_SBDFO (
            0, 
            GnbHandle->Address.Address.Bus, 
            Engine->Type.Port.NativeDevNumber, 
            Engine->Type.Port.NativeFunNumber, \
            PCIE_CFG_ROOT_ERROR_STATUS_OFT
            );

  GnbLibPciRead (Address, AccessWidth32, &Data32, NULL);
  IDS_HDT_CONSOLE (GNB_TRACE, "Before Clear: Root Error s status=%x\n", Data32);
  GnbLibPciRead (Address, AccessWidth32, &Data32, NULL);
  
  // 2. clear device status errors
  Address = MAKE_SBDFO (
          0, 
          GnbHandle->Address.Address.Bus, 
          Engine->Type.Port.NativeDevNumber, 
          Engine->Type.Port.NativeFunNumber, \
          PCIE_CFG_DEVICE_CNTL_AND_STATUS_OFT
          );

  GnbLibPciRead (Address, AccessWidth32, &Data32, NULL);
  IDS_HDT_CONSOLE (GNB_TRACE, "Before Clear: device Error status=%x\n", Data32);
  GnbLibPciRead (Address, AccessWidth32, &Data32, NULL);
  
  // 3. clear lane errors
  Address = MAKE_SBDFO (
          0, 
          GnbHandle->Address.Address.Bus, 
          Engine->Type.Port.NativeDevNumber, 
          Engine->Type.Port.NativeFunNumber, \
          PCIE_CFG_LANE_ERROR_STATUS_OFT
          );

  GnbLibPciRead (Address, AccessWidth32, &Data32, NULL);
  IDS_HDT_CONSOLE (GNB_TRACE, "Before Clear: lane Error status=%x\n", Data32);
  GnbLibPciRead (Address, AccessWidth32, &Data32, NULL);
  
}
