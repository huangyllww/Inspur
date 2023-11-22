/*****************************************************************************
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------*/
#include <HygonCpmPei.h>
#include "XgbeInitPei.h"
#include <Library/DebugLib.h>
#include <Ppi/Pca9545aPpi.h>
#include <Ppi/At24c08dPpi.h>
#include <Library\Ppi\HygonCpmTablePpi\HygonCpmTablePpi.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/HygonSocBaseLib.h>

#define SYS_CFG_BUS  0x0
#define SYS_CFG_DEV  0x18
#define SYS_CFG_FUN  0x1
#define SYS_CFG_OFT  0x200
#define GPIO_FUNCTION_0 0
#define GPIO_FUNCTION_1 1
#define GPIO_FUNCTION_2 2
#define GPIO_NA         3
#define GPIO_OUTPUT_LOW 0
#define GPIO_INPUT      2
#define GPIO_PD_EN      0
#define GPIO_PU_PD_DIS  2
// ----------------------------------------------------------------------------
enum {
  ON_BOARD = 0,
  CHANNEL_0,
  CHANNEL_1,
  CHANNEL_2,
  CHANNEL_3,
  MAX_INFO_DATA_COUNT
};

// ----------------------------------------------------------------------------
EFI_STATUS
XgbeInit (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN VOID                         *InvokePpi
  );

// -----------------------------------------------------------------------------
static EFI_PEI_NOTIFY_DESCRIPTOR  PeiNotifyList[] = {
  { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
    &gHygonNbioPcieTrainingStartPpiGuid, XgbeInit }
};

/**
 * Calculate crc16
 * @param[in]  Data    data point
 * @param[in]  Len     bytes of data
 * @param[in]  Crc     init CRC value
 * @return CRC for data
 */
UINT16
XgbeCrc16 (
  UINT8 *Data, UINTN Len, UINT16 Crc
  )
{
  UINTN  i;

  for( ; Len > 0; Len--) {
    Crc = Crc^(*Data<<8);
    Data++;
    for(i = 0; i < 8; i++) {
      if(Crc&0x8000) {
        Crc = (Crc<<1)^POLY;
      } else {
        Crc <<= 1;
      }
    }
  }

  return Crc;
}

/**
 * Check XGBE info header Valid
 * @param[in]   Head    Point to Head
 * @retval  TRUE   Head Valid
 *          FALSE  Head inValid
 */
BOOLEAN
IsXgbeHeaderValied (
  XGBE_INFO_HEADER *Head
  )
{
  UINT16  Crc;

  Crc = XgbeCrc16 ((UINT8 *)Head, sizeof (XGBE_INFO_HEADER)-2, CRC_INIT_VALUE);
  DEBUG ((EFI_D_ERROR, "Xgbe Header Crc=%x, Calculate Crc=%x\n", Head->Crc, Crc));
  return (Crc == (Head->Crc));
}

/**
 * Check Port Data Valid
 * @param[in]   PortData  Point to PortData
 * @retval  TRUE   Port Data Valid
 *          FALSE  Port Data inValid
 */
BOOLEAN
IsPortDataValied (
  XGBE_PORT_DATA *PortData
  )
{
  UINT16  Crc;

  Crc = XgbeCrc16 ((UINT8 *)PortData, sizeof (XGBE_PORT_DATA)-2, CRC_INIT_VALUE);
  DEBUG ((EFI_D_ERROR, "Port Data Crc=%x, Calculate Crc=%x\n", PortData->Crc, Crc));
  return (Crc == (PortData->Crc));
}

/**
 * Dump XGBE info struct
 * @param[in]   PeiServices  Point to PeiServices
 * @param[in]   XgbeInfo     Point to XGBE info dataPCA9545 I2C address
 * @retval  VOID
 */
void
XgbeDumpInfo (
  CPM_PEI_SERVICES      **PeiServices,
  XGBE_INFO *XgbeInfo
  )
{
  UINT8  i;

  DEBUG ((EFI_D_ERROR, "================================================================\n"));
  DEBUG ((EFI_D_ERROR, "DUMP XGBE INFO\n"));
  DEBUG ((EFI_D_ERROR, "XGBE_INFO->Header.Sign=%04x\n", XgbeInfo->Header.Sign));
  DEBUG ((EFI_D_ERROR, "XGBE_INFO->Header.MacNum=%04x\n", XgbeInfo->Header.MacNum));
  DEBUG ((EFI_D_ERROR, "XGBE_INFO->Header.Reserved=%4x\n", XgbeInfo->Header.Reserved));
  DEBUG ((EFI_D_ERROR, "XGBE_INFO->Header.Reserved=%4x\n", XgbeInfo->Header.Crc));
  for(i = 0; i < XgbeInfo->Header.MacNum; i++) {
    DEBUG ((EFI_D_ERROR, "+++++++++++++++++++++++++PORT %d+++++++++++++++++++++++++++++++\n", i));
    DEBUG ((EFI_D_ERROR, "XGBE_INFO->Header.PortData[%04x].Die=%d\n", i, XgbeInfo->PortData[i].Die));
    DEBUG ((EFI_D_ERROR, "XGBE_INFO->Header.PortData[%04x].Port=%d\n", i, XgbeInfo->PortData[i].Port));
    DEBUG ((EFI_D_ERROR, "XGBE_INFO->Header.PortData[%04x].Activate=%d\n", i, XgbeInfo->PortData[i].Activate));
    DEBUG ((EFI_D_ERROR, "XGBE_INFO->Header.PortData[%04x].MacAddressLo=0x%x\n", i, XgbeInfo->PortData[i].MacAddressLo));
    DEBUG ((EFI_D_ERROR, "XGBE_INFO->Header.PortData[%04x].MacAddressHi=0x%x\n", i, XgbeInfo->PortData[i].MacAddressHi));
    DEBUG ((EFI_D_ERROR, "XGBE_INFO->Header.PortData[%04x].Crc=0x%x\n", i, XgbeInfo->PortData[i].Crc));
  }

  DEBUG ((EFI_D_ERROR, "==================================================================\n"));
}

/**
 * Select Channel
 * @param[in]   PeiServices  Point to PeiServices
 * @param[in]   SwitchAdd    PCA9545 I2C address
 * @param[in]   Channel      PCA9545 channel select, if 0xff select, the PCA9545 not present
 * @retval  EFI_SUCCESS
 *          EFI_DEVICE_ERROR
 */
EFI_STATUS
SelectChannel (
  CPM_PEI_SERVICES **PeiServices, UINTN SwitchAdd, UINT8 ChannelSelect
  )
{
  EFI_STATUS            Status;
  EFI_PEI_PCA9545A_PPI  *Pca9545aPpi;

  if((ChannelSelect != 0xff) && (ChannelSelect > 3)) {
    return EFI_INVALID_PARAMETER;
  }

  // 1.9545A not present
  if(ChannelSelect == 0xFF) {
    return EFI_SUCCESS;
  }

  // 2.select channel
  Status = (*PeiServices)->LocatePpi (
                             (CPM_PEI_SERVICES **)PeiServices,
                             &gPca9545aPpiGuid,
                             0,
                             NULL,
                             (VOID **)&Pca9545aPpi
                             );
  if(EFI_ERROR (Status)) {
    return Status;
  }

  Status = Pca9545aPpi->Set (PeiServices, PCA9545_I2C_OWNER, SwitchAdd, BIT0<<ChannelSelect);
  return Status;
}

/**
 * Get XGBE info Data from At24c08d
 * @param[in]     PeiServices      Point to PeiServices
 * @param[in]     SwitchAdd        PCA9545 I2C address
 * @param[in]     ChannelSelect    PCA9545 channel select, if 0xff select, the PCA9545 not present
 * @param[in]     XgbeInfoRomAdd   At24c08d I2C address
 * @param[OUT]    XgbeInfo         Output XgbeInfo DATA
 * @retval  EFI_SUCCESS
 *          EFI_DEVICE_ERROR
 */
EFI_STATUS
ReadXgbeInfoRom (
  CPM_PEI_SERVICES      **PeiServices,
  UINTN                 SwitchAdd,
  UINT8                 ChannelSelect,
  UINTN                 XgbeInfoRomAdd,
  XGBE_INFO *XgbeInfo
  )
{
  EFI_STATUS            Status;
  UINT8                 *pXgbeInfo;
  XGBE_INFO_HEADER      *XgbeInfoHeader;
  UINT8                 ByteDate;
  UINT8                 i, j;
  UINT8                 PortDataCount;
  EFI_PEI_At24c08d_PPI  *At24c08dPpi;
	HYGON_CPM_TABLE_PPI       *CpmTablePpiPtr;
	//Reset I2C switch before reading eeprom MAC address if I2C switch error
	HYGON_CPM_GPIO_ITEM GpioList[6] = {
	  GPIO_DEF_V3 (0, 0,  147, GPIO_FUNCTION_2, GPIO_OUTPUT_LOW, GPIO_PD_EN), // GPIO147 output low
	  GPIO_DEF_V3 (0, 0,  147, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),     // I2C1_SCL For XGBE eeprom MAC 
	  GPIO_DEF_V3 (0, 0,  148, GPIO_FUNCTION_2, GPIO_INPUT, GPIO_PD_EN),      // GPIO148 input 
	  GPIO_DEF_V3 (0, 0,  148, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),     // I2C1_SDA For XGBE eeprom MAC
	  GPIO_DEF_V3 (0, 0,  147, GPIO_FUNCTION_1, GPIO_INPUT, GPIO_PD_EN),      // GPIO148 input 
	  GPIO_DEF_V3 (0, 0,  148, GPIO_FUNCTION_1, GPIO_NA, GPIO_PU_PD_DIS),     // I2C1_SDA For XGBE eeprom MAC
	}; 
    
	Status = (*PeiServices)->LocatePpi (
	             (CPM_PEI_SERVICES**)PeiServices,
	             &gHygonCpmTablePpiGuid,
	             0,
	             NULL,
	             (VOID**)&CpmTablePpiPtr
	             );
	if (EFI_ERROR (Status)) {
	  return Status;
	}
	CpmTablePpiPtr->CommonFunction.SetGpio (CpmTablePpiPtr, GpioList[2].Pin, GpioList[2].Setting.Raw);
	CpmTablePpiPtr->CommonFunction.Stall (CpmTablePpiPtr, 10*1000);  //delay 10ms
	if(0==(CpmTablePpiPtr->CommonFunction.GetGpio (CpmTablePpiPtr, 148)))
	{
	  DEBUG((EFI_D_ERROR,"XGBE At24c08d I2C switch error \n"));
	  CpmTablePpiPtr->CommonFunction.SetGpio (CpmTablePpiPtr, GpioList[0].Pin, GpioList[0].Setting.Raw);
	  CpmTablePpiPtr->CommonFunction.Stall (CpmTablePpiPtr, 50*1000);  //pull down 50ms
	  CpmTablePpiPtr->CommonFunction.SetGpio (CpmTablePpiPtr, GpioList[1].Pin, GpioList[1].Setting.Raw);
	}
    
	CpmTablePpiPtr->CommonFunction.SetGpio (CpmTablePpiPtr, GpioList[1].Pin, GpioList[1].Setting.Raw);
	CpmTablePpiPtr->CommonFunction.SetGpio (CpmTablePpiPtr, GpioList[3].Pin, GpioList[3].Setting.Raw);
	CpmTablePpiPtr->CommonFunction.Stall (CpmTablePpiPtr, 10*1000);
    
  if((ChannelSelect != 0xff) && (ChannelSelect > 3)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = (*PeiServices)->LocatePpi (
                             (CPM_PEI_SERVICES **)PeiServices,
                             &gAt24c08dPpiGuid,
                             0,
                             NULL,
                             (VOID **)&At24c08dPpi
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  pXgbeInfo = (UINT8 *)XgbeInfo;

  // select switch channel
  Status = SelectChannel (PeiServices, SwitchAdd, ChannelSelect);
  if(EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Select Channel Error,Status=%r\n", Status));
    return Status;
  }

  // read XGBE_INFO_HEADER from at24c08d
  DEBUG ((EFI_D_ERROR, "\n XGBE_INFO_HEADER:\n"));
  for(i = 0; i < sizeof (XGBE_INFO_HEADER); i++) {
    Status = At24c08dPpi->Get (PeiServices, AT24C08D_I2C_OWNER, XgbeInfoRomAdd, i, &ByteDate);
    DEBUG ((EFI_D_ERROR, "%02x ", ByteDate));
    if(EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "At24c08d read byte error\n"));
      return Status;
    }

    *pXgbeInfo = ByteDate;
    pXgbeInfo++;
  }

  DEBUG ((EFI_D_ERROR, "\n"));
  XgbeInfoHeader = (XGBE_INFO_HEADER *)XgbeInfo;
  // check header crc
  if (!IsXgbeHeaderValied (XgbeInfoHeader)) {
    return EFI_DEVICE_ERROR;
  }

  PortDataCount = XgbeInfoHeader->MacNum;

  // read XGBE_PORT_DATA from at24c08d
  DEBUG ((EFI_D_ERROR, "\n XGBE_PORT_DATA:\n"));
  for(j = 0; j < PortDataCount*sizeof (XGBE_PORT_DATA); i++, j++) {
    Status = At24c08dPpi->Get (PeiServices, AT24C08D_I2C_OWNER, XgbeInfoRomAdd, i, &ByteDate);
    DEBUG ((EFI_D_ERROR, "%02x ", ByteDate));
    if((j+1)%sizeof (XGBE_PORT_DATA) == 0) {
      DEBUG ((EFI_D_ERROR, "\n"));
    }

    if(EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "At24c08d read byte error\n"));
      return Status;
    }

    *pXgbeInfo = ByteDate;
    pXgbeInfo++;
  }

  DEBUG ((EFI_D_ERROR, "\n"));

  CpmTablePpiPtr->CommonFunction.SetGpio (CpmTablePpiPtr, GpioList[4].Pin, GpioList[4].Setting.Raw);
  CpmTablePpiPtr->CommonFunction.SetGpio (CpmTablePpiPtr, GpioList[5].Pin, GpioList[5].Setting.Raw);
  return EFI_SUCCESS;
}

/**
 * Get XGBE info Data
 * @param[in]     PeiServices      Point to PeiServices
 * @param[in]     SwitchAdd        PCA9545 I2C address
 * @param[in]     ChannelSelect    PCA9545 channel select, if 0xff select, the PCA9545 not present
 * @param[in]     XgbeInfoRomAdd   At24c08d I2C address
 * @param[OUT]    XgbeInfo         Output XgbeInfo DATA
 * @retval  EFI_SUCCESS
 *          EFI_DEVICE_ERROR
 */
EFI_STATUS
XgbeGetInfoData (
  CPM_PEI_SERVICES      **PeiServices,
  UINTN                 SwitchAdd,
  UINT8                 ChannelSelect,
  UINTN                 XgbeInfoRomAdd,
  XGBE_INFO_STORE       *XgbeInfoDataArray,
  XGBE_INFO **XgbeInfo
  )
{
  EFI_STATUS  Status;
  UINT8       XgbeInfoDataArrayIndex;

  if((ChannelSelect != 0xff) && (ChannelSelect > 3)) {
    return EFI_INVALID_PARAMETER;
  }

  if(ChannelSelect == 0xff) {
    XgbeInfoDataArrayIndex = 0;
  } else {
    XgbeInfoDataArrayIndex = ChannelSelect+1;
  }

  if(XgbeInfoDataArray[XgbeInfoDataArrayIndex].Valid != 0) {
    *XgbeInfo = XgbeInfoDataArray[XgbeInfoDataArrayIndex].InfoData;
    return EFI_SUCCESS;
  } else {
    if(XgbeInfoDataArray[XgbeInfoDataArrayIndex].InfoData == NULL) {
      Status = (*PeiServices)->AllocatePool (
                                 PeiServices,
                                 sizeof (XGBE_INFO),
                                 &XgbeInfoDataArray[XgbeInfoDataArrayIndex].InfoData
                                 );
      if(EFI_ERROR (Status)) {
        return Status;
      }
    }

    Status = ReadXgbeInfoRom (PeiServices, SwitchAdd, ChannelSelect, XgbeInfoRomAdd, XgbeInfoDataArray[XgbeInfoDataArrayIndex].InfoData);
    if(EFI_ERROR (Status)) {
      return Status;
    }

    XgbeInfoDataArray[XgbeInfoDataArrayIndex].Valid = 1;
    *XgbeInfo = XgbeInfoDataArray[XgbeInfoDataArrayIndex].InfoData;
    return EFI_SUCCESS;
  }
}

/**
 * Get MAC address from XGBE info data
 * @param[in]     PeiServices    Pointer to PEI Services
 * @param[in]     XgbeInfo       Pointer to XGBE info data
 * @param[in]     Die            Die number
 * @param[in]     XgbePort       Port number
 * @param[OUT]    MacAddressLo   MAC address low
 * @param[OUT]    MacAddressHi   MAC address high
 *
 * @retval        EFI_SUCCESS    get MAC address success
 * @retval        EFI_DEVICE_ERROR
 */
EFI_STATUS
OnBoardXgbeGetMacAddr (
  CPM_PEI_SERVICES      **PeiServices,
  XGBE_INFO *XgbeInfo,
  UINT8                 Die,
  UINT8                 XgbePort,
  UINT32 *MacAddressLo,
  UINT32 *MacAddressHi
  )
{
  UINT8    i = 0;
  BOOLEAN  FindFlag = FALSE;

  for(i = 0; i < MAX_XGBE_INFO_COUNT; i++) {
    if(XgbeInfo->PortData[i].Die == Die && XgbeInfo->PortData[i].Port == XgbePort) {
      FindFlag = TRUE;
      break;
    }
  }

  if(FindFlag) {
    if (IsPortDataValied (&XgbeInfo->PortData[i])) {
      *MacAddressLo = XgbeInfo->PortData[i].MacAddressLo;
      *MacAddressHi = XgbeInfo->PortData[i].MacAddressHi;
      return EFI_SUCCESS;
    }
  }

  return EFI_DEVICE_ERROR;
}

/**
 * Get MAC address from XGBE info data
 * @param[in]     PeiServices    Pointer to PEI Services
 * @param[in]     XgbeInfo       Pointer to XGBE info data
 * @param[in]     Die            Die number
 * @param[in]     XgbePort       Port number
 * @param[OUT]    MacAddressLo   MAC address low
 * @param[OUT]    MacAddressHi   MAC address high
 *
 * @retval        EFI_SUCCESS    get MAC address success
 * @retval        EFI_DEVICE_ERROR
 */
EFI_STATUS
FieldCardXgbeGetMacAddr (
  CPM_PEI_SERVICES      **PeiServices,
  XGBE_INFO *XgbeInfo,
  UINT8                 Die,
  UINT8                 XgbePort,
  UINT32 *MacAddressLo,
  UINT32 *MacAddressHi
  )
{
  UINT8    i = 0;
  BOOLEAN  FindFlag = FALSE;

  for(i = 0; i < MAX_XGBE_INFO_COUNT; i++) {
    if(XgbeInfo->PortData[i].Port == XgbePort) {
      FindFlag = TRUE;
      break;
    }
  }

  if(FindFlag) {
    if (IsPortDataValied (&XgbeInfo->PortData[i])) {
      *MacAddressLo = XgbeInfo->PortData[i].MacAddressLo;
      *MacAddressHi = XgbeInfo->PortData[i].MacAddressHi;
      return EFI_SUCCESS;
    }
  }

  return EFI_DEVICE_ERROR;
}

/**
 * XGBE initialization, Update MAC address to HSIO Table
 * @param[in] PeiServices   Pointer to PEI Services
 *
 * @retval EFI_SUCCESS
 *         EFI_DEVICE_ERROR
 */
EFI_STATUS
XgbeInitStart (
  IN        CPM_PEI_SERVICES      **PeiServices,
  IN        XGBE_INFO_STORE       *XgbeInfoDataArray
  )
{
  EFI_STATUS                     Status;
  UINT32                         Socket, SocketNumber, DiePreSocket, BaseDieNumber = 0;
  XGBE_INFO                      *XgbeInfo;
  HYGON_CPM_TABLE_PPI            *CpmTablePpiPtr;
  HYGON_CPM_HSIO_TOPOLOGY_TABLE  *HsioTopologyTablePtr;
  UINT8                          Index;
  UINT32                         XgbeDie, XgbePort;
  UINT32                         MacAddressLo, MacAddressHi;
  UINT32                         SlotMux, SlotBusSeg, SlotMuxUpAdd, XgbeInfoRomAdd, XgbeInfoRomAddUP, InfoRomLocation;
  UINT8                          ChannelSelect;
  UINTN                          Pca9545Addr, At24c08dAddr;
  UINT32                         CpuModel;

  // UINT32                        Date32;

  Status = (*PeiServices)->LocatePpi (
                             (CPM_PEI_SERVICES **)PeiServices,
                             &gHygonCpmTablePpiGuid,
                             0,
                             NULL,
                             (VOID **)&CpmTablePpiPtr
                             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  HsioTopologyTablePtr = NULL;
  CpuModel             = GetHygonSocModel();
  
  if (CpuModel == HYGON_EX_CPU) {
    HsioTopologyTablePtr = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY);
  } else if (CpuModel == HYGON_GX_CPU) {
    HsioTopologyTablePtr = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY);
  }

  ASSERT (HsioTopologyTablePtr != NULL);
  
  SocketNumber = (UINT32)FabricTopologyGetNumberOfSocketPresent ();
  DiePreSocket = (UINT32)FabricTopologyGetNumberOfPhysicalDiesOnSocket (0);

  if(SocketNumber > 2 || DiePreSocket > 4) {
    DEBUG ((EFI_D_ERROR, "ERROR:Socket Number=%d,Die Number Per socket=%d\n", SocketNumber, DiePreSocket));
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((EFI_D_ERROR, "Socket Number=%d,Die Number Per socket=%d\n", SocketNumber, DiePreSocket));

  // Update HSIO table
  for(Socket = 0; Socket < SocketNumber; Socket++) {
    HsioTopologyTablePtr += Socket;
    BaseDieNumber += (Socket*DiePreSocket);
    for ( Index = 0; Index < HYGON_HSIO_PORT_DESCRIPTOR_SIZE; Index++ ) {
      // EthernetEngine find?
      if ( HsioTopologyTablePtr->Port[Index].EngineData.EngineType == HsioEthernetEngine ) {
        DEBUG((EFI_D_ERROR,"xgbe HsioEthernetEngine  %x \n",HsioTopologyTablePtr->Port[Index].EngineData.EngineType));
        XgbeDie          = Socket;
        XgbePort         = HsioTopologyTablePtr->Port[Index].EtherNet.EthPortProp0.PortNum;
        SlotMux          = HsioTopologyTablePtr->Port[Index].EtherNet.EthPortProp5.SlotMux;
        SlotBusSeg       = HsioTopologyTablePtr->Port[Index].EtherNet.EthPortProp5.SlotBusSeg;
        SlotMuxUpAdd     = HsioTopologyTablePtr->Port[Index].EtherNet.EthPortProp5.SlotMuxUpAdd;
        XgbeInfoRomAdd   = HsioTopologyTablePtr->Port[Index].EtherNet.EthPortProp5.InfoRomAdd;
        XgbeInfoRomAddUP = HsioTopologyTablePtr->Port[Index].EtherNet.EthPortProp5.InfoRomAddUP;
        InfoRomLocation  = HsioTopologyTablePtr->Port[Index].EtherNet.EthPortProp5.InfoRomLocation;

        DEBUG ((EFI_D_ERROR, "[Socket%x Die%x Port%x] XGBE Update HSIO Table\n", Socket, XgbeDie, XgbePort));

        if(InfoRomLocation == 0) {
          Pca9545Addr   = 0xff;
          ChannelSelect = 0xff;
        } else {
          if(SlotMux == 0x7) {
            Pca9545Addr   = 0xff;
            ChannelSelect = 0xff;
          } else {
            Pca9545Addr   = (SlotMuxUpAdd<<2) + SlotMux;
            ChannelSelect = (UINT8)SlotBusSeg;
          }
        }

        At24c08dAddr = (XgbeInfoRomAddUP<<2) + XgbeInfoRomAdd;

        // read xgbe info
        Status = XgbeGetInfoData (PeiServices, Pca9545Addr, ChannelSelect, At24c08dAddr, XgbeInfoDataArray, &XgbeInfo);
        if(EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "XGBE Get Info Data error!!!\n"));
          continue;
        }

        // get mac address from xgbe info data
        if(InfoRomLocation == 0) {
          Status = OnBoardXgbeGetMacAddr (PeiServices, XgbeInfo, (UINT8)XgbeDie, (UINT8)XgbePort, &MacAddressLo, &MacAddressHi);
        } else if (InfoRomLocation == 1) {
          Status = FieldCardXgbeGetMacAddr (PeiServices, XgbeInfo, (UINT8)XgbeDie, (UINT8)XgbePort, &MacAddressLo, &MacAddressHi);
        } else {
          DEBUG ((EFI_D_ERROR, "InfoRomLocation invalid!\n"));
          continue;
        }

        // update mac address to hsio table
        if(!EFI_ERROR (Status)) {
          HsioTopologyTablePtr->Port[Index].EtherNet.MacAddressLo = MacAddressLo;
          HsioTopologyTablePtr->Port[Index].EtherNet.MacAddressHi = MacAddressHi;
          DEBUG ((EFI_D_ERROR, "MacAddressLo=0x%X\n", MacAddressLo));
          DEBUG ((EFI_D_ERROR, "MacAddressHi=0x%X\n", MacAddressHi));
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
 * XGBE initialization, Update MAC address to HSIO Table
 * @param[in] PeiServices   Pointer to PEI Services
 *
 * @retval EFI_SUCCESS
 *         EFI_DEVICE_ERROR
 */
EFI_STATUS
XgbeInit (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN VOID                         *InvokePpi
  )
{
  EFI_STATUS       Status;
  XGBE_INFO_STORE  XgbeInfoDataArray[5] = { { NULL, 0 }, { NULL, 0 }, { NULL, 0 }, { NULL, 0 }, { NULL, 0 } };

  DEBUG ((EFI_D_ERROR, "XGBE Initialization Start\n"));
  Status = XgbeInitStart (PeiServices, &XgbeInfoDataArray[0]);
  DEBUG ((EFI_D_ERROR, "XGBE Initialization Complete,Status=%r\n", Status));

  return Status;
}

/**
 * Entry point of the Field card init PEIM driver
 *
 * This function do field card init
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
XgbeInitPeiEntryPoint (
  IN        CPM_PEI_FILE_HANDLE   FileHandle,
  IN        CPM_PEI_SERVICES      **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = (*PeiServices)->NotifyPpi (PeiServices, PeiNotifyList);

  return EFI_SUCCESS;
}
