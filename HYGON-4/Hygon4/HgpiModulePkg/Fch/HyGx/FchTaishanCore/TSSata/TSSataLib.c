/* $NoKeywords:$ */

/**
 * @file
 *
 * Fch SATA controller Library
 *
 * SATA Library
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
#include "FchPlatform.h"
#include  "FchCommonCfg.h"
#include "Filecode.h"
#define FILECODE  PROC_FCH_TAISHAN_TSSATA_TSSATALIB_FILECODE

#define MAX_RETRY_NUM  100

VOID
SataEnableWriteAccessTS (
  IN  UINT8               PhysicalDieId,
  IN  UINT8               SataIndex,
  IN  UINT32              DieBusNum,
  IN  VOID                *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0x00, 0xFFFFFFFE, BIT0, LocalCfgPtr->StdHeader);
}

VOID
SataDisableWriteAccessTS (
  IN  UINT8               PhysicalDieId,
  IN  UINT8               SataIndex,
  IN  UINT32              DieBusNum,
  IN  VOID                *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0x00, 0xFFFFFFFE, 0, LocalCfgPtr->StdHeader);
}

/**
 * FchTSSataInitPortOffline - Put Sata Port in Offline mode
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] SataIndex      Sata index 0 or 1.
 * @param[in] DieBusNum      IOHC bus number on current Die.
 * @param[in] PortNum        Sata Port to put in offline.
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSSataInitPortOffline (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  UINT32   PortNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT32                Retry;
  UINT32                PortStatus;
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG12C + PortNum * 0x80, 
    0xFFFFFFF0,
    0x4,
    LocalCfgPtr->StdHeader
    );

  for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
    FchSmnRead (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG128 + PortNum * 0x80, 
      &PortStatus,
      LocalCfgPtr->StdHeader
      );
    if ((PortStatus & 0x0F) == 0x4) {
      break;
    }

    FchStall (10, LocalCfgPtr->StdHeader);
  }
}

/**
 * FchTSSataInitPortActive - Put Sata Port back to active from
 * Offline mode
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] SataIndex      Sata index 0 or 1.
 * @param[in] DieBusNum      IOHC bus number on current Die.
 * @param[in] PortNum        Sata Port to put in offline.
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSSataInitPortActive (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  UINT32   PortNum,
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  if (PcdGet8 (PcdRunEnvironment) == 2) {
    /// on EMU phy can bringup after offline
    FchSmnRW (
      DieBusNum, 
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_GENII_SETTING_HYGX), 
      ~(UINT32)(0xfc), 
      0, 
      LocalCfgPtr->StdHeader
      );
    FchSmnRW (
      DieBusNum, 
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_GENIII_SETTING_HYGX), 
      ~(UINT32)(0xfc), 
      0, 
      LocalCfgPtr->StdHeader
      );
  }

  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0x00, 
    ~(UINT32)(BIT16 << PortNum),
    0,
    LocalCfgPtr->StdHeader
    );
  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG12C + PortNum * 0x80, 
    0xFFFFFFF0,
    0x1,
    LocalCfgPtr->StdHeader
    );
  FchStall (1000, LocalCfgPtr->StdHeader);
  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG12C + PortNum * 0x80, 
    0xFFFFFFF0,
    0x0,
    LocalCfgPtr->StdHeader
    );
}

/**
 * FchTSSataInitEnableErr - Enable Error reporting
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] SataIndex      Sata index 0 or 1.
 * @param[in] DieBusNum      Bus Number of current Die
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSSataInitEnableErr (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT32               PortNum;
  BOOLEAN              FchD2HFifoParityErr;
  BOOLEAN              FchH2DFifoParityErr;
  BOOLEAN              FchContextMemoryParityErr;
  BOOLEAN              FchSataRasSupport;
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  FchSataRasSupport   = LocalCfgPtr->Sata.SataRasSupport;
  FchD2HFifoParityErr = TRUE;          // Optional
  FchH2DFifoParityErr = TRUE;          // Optional
  FchContextMemoryParityErr = TRUE;    // Optional

  if ( FchSataRasSupport ) {
    //
    // (Optional) Enable Report D2H FIFO Parity Error to System for each port.
    //
    if ( FchD2HFifoParityErr ) {
      for ( PortNum = 0; PortNum < TAISHAN_SATA_PORT_NUM; PortNum++ ) {
        FchSmnRW (
          DieBusNum,
          SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + PortNum * 0x20 + 0x88,
          ~(UINT32) BIT4,
          BIT4,
          StdHeader
          );
      }
    }

    //
    // (Optional) Enable Report H2D FIFO Parity Error to System for each port
    //
    if ( FchH2DFifoParityErr ) {
      for ( PortNum = 0; PortNum < TAISHAN_SATA_PORT_NUM; PortNum++ ) {
        FchSmnRW (
          DieBusNum,
          SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + PortNum * 0x20 + 0x88,
          ~(UINT32) BIT5,
          BIT5,
          StdHeader
          );
      }
    }

    //
    // (Optional) Enable Report Context Memory Parity Error to System for each port
    //
    if ( FchContextMemoryParityErr ) {
      for ( PortNum = 0; PortNum < TAISHAN_SATA_PORT_NUM; PortNum++ ) {
        FchSmnRW (
          DieBusNum,
          SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + PortNum * 0x20 + 0x88,
          ~(UINT32) BIT6,
          BIT6,
          StdHeader
          );
      }
    }
  }

  //
  // Clear error status
  //

  for ( PortNum = 0; PortNum < TAISHAN_SATA_PORT_NUM; PortNum++ ) {
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG130 + PortNum * 0x80,
      0xFFFFFFFF,
      0xFFFFFFFF,
      StdHeader
      );
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG110 + PortNum * 0x80,
      0xFFFFFFFF,
      0x00,
      StdHeader
      );
  }
}

/**
 * FchTSSataInitEsata - Enable eSATA port
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] SataIndex      Sata index 0 or 1.
 * @param[in] DieBusNum      Bus Number of current Die
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSSataInitEsata (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN VOID      *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  //
  // SATA ESP port setting
  // These config bits are set for SATA driver to identify which ports are external SATA ports and need to
  // support hotplug. If a port is set as an external SATA port and need to support hotplug, then driver will
  // not enable power management (HIPM & DIPM) for these ports.
  //
  if ( LocalCfgPtr->Sata.SataEspPort.SataPortReg != 0 ) {
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGF8,
      ~(LocalCfgPtr->Sata.SataEspPort.SataPortReg),
      0,
      StdHeader
      );
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGF8,
      0xFF00FFFF,
      (LocalCfgPtr->Sata.SataEspPort.SataPortReg << 16),
      StdHeader
      );
    //
    // External SATA Port Indication Registers
    // If any of the ports was programmed as an external port, HCAP.SXS should also be set
    //
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGFC,
      ~(UINT32) (BIT20),
      BIT20,
      StdHeader
      );

    //
    // Enable  eSATA PHY power-saving when no device presented.
    //
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0x10,
      ~(UINT32) (BIT29),
      BIT29,
      StdHeader
      );
  } else {
    //
    // External SATA Port Indication Registers
    // If any of the ports was programmed as an external port, HCAP.SXS should also be set (Clear for no ESP port)
    //
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGF8,
      0xFF00FF00,
      0x00,
      StdHeader
      );
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGFC,
      ~(UINT32) (BIT20),
      0x00,
      StdHeader
      );
  }
}

/**
 * FchTSSataInitDevSlp - init DevSlp configuration
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] SataIndex      Sata index 0 or 1.
 * @param[in] DieBusNum      Bus Number of current Die
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSSataInitDevSlp (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT8                PortNumByte;
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  //
  // 1, Enable the DEVLSP related GPIO pad and put the it into open-drain mode
  // 2, Enable the capability bit for DevSlp support
  // 3, Program CFG_PxCMD2.DSP bit for the port which will support DEVSLP.
  // 4, Program PortX_DEVSLP_MAP to route any two ports to external PAD.
  //
  PortNumByte = LocalCfgPtr->Sata.SataDevSlpPort0Num;
  if ( LocalCfgPtr->Sata.SataDevSlpPort0 ) {
    RwMem (LocalCfgPtr->HwAcpi.FchAcpiMmioBase + IOMUX_BASE + FCH_GPIO_REG05, AccessWidth8, 0x00, 0x1);
    RwMem (
      LocalCfgPtr->HwAcpi.FchAcpiMmioBase + GPIO_BANK0_BASE + FCH_GPIO_014_BP_AGPIO5,
      AccessWidth32,
      ~(UINT32)(BIT22 + BIT23),
      BIT23
      );
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGF4,
      0xFFFFFFEF,
      BIT4 + (BIT8 << PortNumByte),
      StdHeader
      );
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0x8C,
      0xFFFF8FFF,
      (UINT32)(PortNumByte << 12),
      StdHeader
      );
  } else {
    RwMem (
      LocalCfgPtr->HwAcpi.FchAcpiMmioBase + IOMUX_BASE + FCH_GPIO_REG05,
      AccessWidth8,
      0x00,
      0x00
      );
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGF4,
      ~(UINT32)(BIT8 << PortNumByte),
      0x00,
      StdHeader
      );
    if ( !LocalCfgPtr->Sata.SataDevSlpPort1 ) {
      FchSmnRW (
        DieBusNum,
        SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGF4,
        0xFFFFFFEF,
        0x00,
        StdHeader
        );
    }
  }

  PortNumByte = LocalCfgPtr->Sata.SataDevSlpPort1Num;
  if ( LocalCfgPtr->Sata.SataDevSlpPort1 ) {
    RwMem (
      LocalCfgPtr->HwAcpi.FchAcpiMmioBase + IOMUX_BASE + FCH_GPIO_REG06,
      AccessWidth8,
      0x00,
      0x1
      );
    RwMem (
      LocalCfgPtr->HwAcpi.FchAcpiMmioBase + GPIO_BANK0_BASE + FCH_GPIO_018_BP_AGPIO6,
      AccessWidth32,
      ~(UINT32)(BIT22 + BIT23),
      BIT23
      );
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGF4,
      0xFFFFFFEF,
      BIT4 + (BIT8 << PortNumByte),
      StdHeader
      );
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0xAC,
      0xFFFF8FFF,
      (UINT32)(PortNumByte << 12),
      StdHeader
      );
  } else {
    RwMem (
      LocalCfgPtr->HwAcpi.FchAcpiMmioBase + IOMUX_BASE + FCH_GPIO_REG06,
      AccessWidth8,
      0x00,
      0x00
      );
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGF4,
      ~(UINT32)(BIT8 << PortNumByte),
      0x00,
      StdHeader
      );
    if ( !LocalCfgPtr->Sata.SataDevSlpPort0 ) {
      FchSmnRW (
        DieBusNum,
        SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGF4,
        0xFFFFFFEF,
        0x00,
        StdHeader
        );
    }
  }
}

/**
 * FchTSSataInitMpssMap - Program Mechanical Presence Switch
 * State Mapping.
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] SataIndex      Sata index 0 or 1.
 * @param[in] DieBusNum      Bus Number of current Die
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSSataInitMpssMap (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT32               PortNum;
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;
  for (PortNum = 0; PortNum < TAISHAN_SATA_PORT_NUM; PortNum++) {
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + PortNum * 0x20 + 0x8C,
      0xFFFFF8F8,
      (UINT32)((PortNum << 8) | 7),
      StdHeader
      );
  }
}

/**
 * FchTSSataInitRsmuCtrl - Sata Rsmu Control register setting
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] SataIndex      Sata index 0 or 1.
 * @param[in] DieBusNum      Bus Number of current Die
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSSataInitRsmuCtrl (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN VOID      *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  //
  // SATA controller operates in maximum Gen2
  //
  if ( LocalCfgPtr->Sata.SataMode.SataSetMaxGen2 ) {
    FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_TS_SATA_MISC_CONTROL_HYGX), ~(UINT32)BIT2, BIT2, StdHeader);
  } else {
    FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_TS_SATA_MISC_CONTROL_HYGX), ~(UINT32)BIT2, 0x00, StdHeader);
  }

  //
  // Sata Target Support 8 devices function
  //
  if ( LocalCfgPtr->Sata.SataTargetSupport8Device ) {
    FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_TS_SATA_MISC_CONTROL_HYGX), ~(UINT32)BIT3, BIT3, StdHeader);
  } else {
    FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_TS_SATA_MISC_CONTROL_HYGX), ~(UINT32)BIT3, 0x00, StdHeader);
  }

  //
  // Sata Generic Mode setting
  //
  if ( LocalCfgPtr->Sata.SataDisableGenericMode ) {
    FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_TS_SATA_MISC_CONTROL_HYGX), ~(UINT32)BIT1, BIT1, StdHeader);
  } else {
    FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_TS_SATA_MISC_CONTROL_HYGX), ~(UINT32)BIT1, 0x00, StdHeader);
  }

  //
  // OOB Detection Enhancement
  //
  if ( LocalCfgPtr->Sata.SataOobDetectionEnh ) {
    FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_TS_SATA_MISC_CONTROL_HYGX), ~(UINT32)BIT24, BIT24, StdHeader);
  } else {
    FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_TS_SATA_MISC_CONTROL_HYGX), ~(UINT32)BIT24, 0x00, StdHeader);
  }
}

/**
 * FchTSSataInitCtrlReg - Sata Rsmu Control register setting
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] SataIndex      Sata index 0 or 1.
 * @param[in] DieBusNum      Bus Number of current Die
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSSataInitCtrlReg (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN VOID      *FchDataPtr
  )
{
  UINT32               AndMaskDword;
  UINT32               OrMaskDword;
  UINT8                FchSataAggrLinkPmCap;
  UINT8                FchSataPortMultCap;
  UINT8                FchSataPscCap;
  UINT8                FchSataSscCap;
  UINT8                FchSataFisBasedSwitching;
  UINT8                FchSataCccSupport;
  UINT8                FchSataAhciEnclosureManagement;
  BOOLEAN              FchSataMsiEnable;
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;
  UINT32               SataStrapAddr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  FchSataAggrLinkPmCap           = (UINT8) LocalCfgPtr->Sata.SataAggrLinkPmCap;
  FchSataPortMultCap             = (UINT8) LocalCfgPtr->Sata.SataPortMultCap;
  FchSataPscCap                  = (UINT8) LocalCfgPtr->Sata.SataPscCap;
  FchSataSscCap                  = (UINT8) LocalCfgPtr->Sata.SataSscCap;
  FchSataFisBasedSwitching       = (UINT8) LocalCfgPtr->Sata.SataFisBasedSwitching;
  FchSataCccSupport              = (UINT8) LocalCfgPtr->Sata.SataCccSupport;
  FchSataAhciEnclosureManagement = (UINT8) LocalCfgPtr->Sata.SataAhciEnclosureManagement;
  FchSataMsiEnable               = LocalCfgPtr->Sata.SataMsiEnable;

  AndMaskDword = 0;
  OrMaskDword  = 0;

  if ( !FchSataPortMultCap ) {
    AndMaskDword |= BIT12;
  }

  if ( FchSataFisBasedSwitching ) {
    OrMaskDword |= BIT10;
  } else {
    AndMaskDword |= BIT10;
  }

  if ( FchSataAggrLinkPmCap ) {
    OrMaskDword |= BIT11;
  } else {
    AndMaskDword |= BIT11;
  }

  if ( FchSataPscCap ) {
    OrMaskDword |= BIT1;
  } else {
    AndMaskDword |= BIT1;
  }

  if ( FchSataSscCap ) {
    OrMaskDword |= BIT26;
  } else {
    AndMaskDword |= BIT26;
  }

  //
  // Disabling CCC (Command Completion Coalescing) support.
  //
  if ( FchSataCccSupport ) {
    OrMaskDword |= BIT19;
  } else {
    AndMaskDword |= BIT19;
  }

  if ( FchSataAhciEnclosureManagement ) {
    OrMaskDword |= BIT27;
  } else {
    AndMaskDword |= BIT27;
  }

  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGFC,
    ~AndMaskDword,
    OrMaskDword,
    StdHeader
    );

  if ( FchSataFisBasedSwitching ) {
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGF8,
      0x00FFFFFF,
      0xFF000000,
      StdHeader
      );
  } else {
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGF8,
      0x00FFFFFF,
      0x00,
      StdHeader
      );
  }

  if ( LocalCfgPtr->Sata.BiosOsHandOff == 1 ) {
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG24,
      ~(UINT32)BIT0,
      BIT0,
      StdHeader
      );
  } else {
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG24,
      ~(UINT32)BIT0,
      0x00,
      StdHeader
      );
  }

  // UBTS460961
  // Program sata_slor :: 0x0C[31:28] = 0xF>
  // Program sata_slor :: 0x0C[6:5] = 0x3>
  // Program sata_slor :: 0x0C[3] = 0x1>
  // Program sata_slor :: 0x0C[1] = 0x1>
  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0x0C,
    ~(UINT32)BIT23,
    0xF000006A,
    StdHeader
    );
  //
  // BIT2 : Enable dynamic blink clock gating
  // BIT14: Enable BlcClkGated gating
  // BIT15: Enable PciClkGated gating
  // BIT8 & BIT9 Disable performance enhancement
  // BIT25
  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0x10,
    ~(UINT32)(BIT2 + BIT8 + BIT9 + BIT14 + BIT15 + BIT25),
    BIT2 + BIT8 + BIT9 + BIT14 + BIT15 + BIT25,
    StdHeader
    );

  // Program sata_slor :: 0x14[8:5] = 0xF
  // Program sata_slor :: 0x14[30] = 1
  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0x14,
    ~(UINT32)(BIT5 + BIT6 + BIT7 + BIT8 + BIT30),
    BIT5 + BIT6 + BIT7 + BIT8 + BIT30,
    StdHeader
    );
  //
  // Disable Prefetch In Ahci Mode
  //
  if ( LocalCfgPtr->Sata.SataAhciDisPrefetchFunction ) {
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0x00,
      ~(UINT32) BIT13,
      BIT13,
      StdHeader
      );
  } else {
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0x00,
      ~(UINT32) BIT13,
      0x00,
      StdHeader
      );
  }

  //
  // MSI Capability could be disabled by setting register nBIF1:RCC_DEV0_EPF2_STRAP3.STRAP_MSI_EN_DEV0_F2 to 0.
  //
  if (SataIndex == 0) {
    SataStrapAddr = NBIF2_SATA_STRAP3_ADDRESS_HYGX;
  } else {
    SataStrapAddr = NBIF3_SATA_STRAP3_ADDRESS_HYGX;
  }

  if ( FchSataMsiEnable ) {
    FchSmnRW (DieBusNum, NBIO_SPACE2 (PhysicalDieId, 0, SataStrapAddr), ~(UINT32)BIT18, BIT18, StdHeader);
  } else {
    FchSmnRW (DieBusNum, NBIO_SPACE2 (PhysicalDieId, 0, SataStrapAddr), ~(UINT32)BIT18, 0x00, StdHeader);
  }

  //
  // Shutdown ports
  //
  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0x00,
    0xFF00FFFF,
    (LocalCfgPtr->Sata.SataPortPower.SataPortReg << 16),
    StdHeader
    );
}

/**
 * FchTSSataInitMMC - Sata MSI MMC setting
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] SataIndex      Sata index 0 or 1.
 * @param[in] DieBusNum      Bus Number of current Die
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSSataInitMMC (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN VOID      *FchDataPtr
  )
{
  UINT32               SataBusDevFun;
  UINT32               SataPciMmc;
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  if ( LocalCfgPtr->Sata.SataMsiEnable ) {
    //
    // Get Sata PCI Bus number
    //

    if (SataIndex == 0) {
      SataBusDevFun = PciRead32 ((((DieBusNum << 8) + TAISHAN_BUSCPORT_BUS_DEV_FUN) << 12) + 0x18);
    } else {
      SataBusDevFun = PciRead32 ((((DieBusNum << 8) + TAISHAN_BUSDPORT_BUS_DEV_FUN) << 12) + 0x18);
    }

    SataBusDevFun &= 0xFF00;

    //
    // Get Sata PCI MSI.MMC setting
    //
    SataPciMmc = PciRead32 (((SataBusDevFun + TAISHAN_SATA_DEV_FUN) << 12) + FCH_SATA_REGA0);
    // may need debug <<<

    //
    // Sata MMC programming
    //
    // nBif MMC at bit[19:17], SATA_Rsmux0A MMC bits at [26:24]
    SataPciMmc = (SataPciMmc & 0x000E0000) << 7;
    FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_TS_SATA_EVENT_SELECT_HYGX), 0xF8FFFFFF, SataPciMmc, StdHeader);
  }
}

/**
 * FchTSSataSetPortGenMode - Set Sata port mode (each) for
 * Gen1/Gen2/Gen3
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] SataIndex      Sata index 0 or 1.
 * @param[in] DieBusNum      Bus Number of current Die
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSSataSetPortGenMode (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT8                PortNumByte;
  UINT8                PortModeByte;
  UINT16               SataPortMode;
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  SataPortMode = (UINT16)LocalCfgPtr->Sata.SataPortMd.SataPortMode;
  PortNumByte  = 0;

  while ( PortNumByte < TAISHAN_SATA_PORT_NUM ) {
    PortModeByte = (UINT8) (SataPortMode & 3);
    if ( (PortModeByte == BIT0) || (PortModeByte == BIT1) ) {
      if ( PortModeByte == BIT0 ) {
        //
        // set GEN 1
        //
        FchSmnRW (
          DieBusNum,
          SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG12C + PortNumByte * 0x80,
          0xFFFFFF0F,
          0x10,
          StdHeader
          );
      }

      if ( PortModeByte == BIT1 ) {
        //
        // set GEN2 (default is GEN3)
        //
        FchSmnRW (
          DieBusNum,
          SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG12C + PortNumByte * 0x80,
          0xFFFFFF0F,
          0x20,
          StdHeader
          );
      }

      FchSmnRW (
        DieBusNum,
        SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG12C + PortNumByte * 0x80,
        0xFFFFFFFF,
        0x01,
        StdHeader
        );
    }

    SataPortMode >>= 2;
    PortNumByte++;
  }

  FchStall (1000, StdHeader);
  SataPortMode = (UINT16)LocalCfgPtr->Sata.SataPortMd.SataPortMode;
  PortNumByte  = 0;

  while ( PortNumByte < TAISHAN_SATA_PORT_NUM ) {
    PortModeByte = (UINT8)(SataPortMode & 3);

    if ((PortModeByte == BIT0) || (PortModeByte == BIT1)) {
      FchSmnRW (
        DieBusNum,
        SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG12C + PortNumByte * 0x80,
        0xFFFFFFFE,
        0x00,
        StdHeader
        );
    }

    PortNumByte++;
    SataPortMode >>= 2;
  }
}

/**
 * FchTSSataShutdownUnconnectedSataPortClock - Shutdown
 * unconnected Sata port clock
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] SataIndex      Sata index 0 or 1.
 * @param[in] DieBusNum      Bus Number of current Die
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSSataShutdownUnconnectedSataPortClock (
  IN  UINT8      PhysicalDieId,
  IN  UINT8      SataIndex,
  IN  UINT32     DieBusNum,
  IN  VOID       *FchDataPtr
  )
{
  UINT8                PortNumByte;
  UINT8                PortSataStatusByte;
  UINT8                NumOfPorts;
  UINT8                FchSataClkAutoOff;
  UINT32               PortStatusDword;
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;
  UINT8                SaveLocation;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;
  FchSataClkAutoOff = (UINT8)LocalCfgPtr->Sata.SataClkAutoOff;

  NumOfPorts = 0;
  //
  // Enable SATA auto clock control by default
  //
  if ( FchSataClkAutoOff ) {
    if ((ReadFchSleepType (StdHeader) != ACPI_SLPTYP_S3)) {
      for ( PortNumByte = 0; PortNumByte < TAISHAN_SATA_PORT_NUM; PortNumByte++ ) {
        FchSmnRead (
          DieBusNum,
          SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG128 + PortNumByte * 0x80,
          &PortStatusDword,
          StdHeader
          );
        PortSataStatusByte = (UINT8)(PortStatusDword & 0xFF);
        //
        // Shutdown the clock for the port and do the necessary port reporting changes.
        // Error port status should be 1 not 3
        //
        if (((PortSataStatusByte & 0x0F) != 0x03) && 
            (!((LocalCfgPtr->Sata.SataEspPort.SataPortReg) & (1 << PortNumByte)))) {
          FchSmnRW (
            DieBusNum,
            SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0x00,
            ~(UINT32) (BIT16 << PortNumByte),
            (BIT16 << PortNumByte),
            StdHeader
            );
          FchSmnRW (
            DieBusNum,
            SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG0C,
            ~(UINT32)(1 << PortNumByte),
            00,
            StdHeader
            );
        }
      } /// end of for (PortNumByte=0;PortNumByte<6;PortNumByte++)

      SaveLocation = 0;
      LibHygonIoWrite (AccessWidth8, FCH_IOMAP_REG72, &SaveLocation, StdHeader);
      FchSmnRead (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0x00, &PortStatusDword, StdHeader);
      PortSataStatusByte = (UINT8)((PortStatusDword >> 0x10) & 0xFF);
      SaveLocation = 1;
      LibHygonIoWrite (AccessWidth8, FCH_IOMAP_REG73, &PortSataStatusByte, StdHeader);
      LibHygonIoWrite (AccessWidth8, FCH_IOMAP_REG72, &SaveLocation, StdHeader);
      FchSmnRead (
        DieBusNum,
        SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG0C,
        &PortStatusDword,
        StdHeader
        );
      PortSataStatusByte = (UINT8)(PortStatusDword & 0xFF);
      LibHygonIoWrite (AccessWidth8, FCH_IOMAP_REG73, &PortSataStatusByte, StdHeader);
    } else {
      SaveLocation = 0;
      LibHygonIoWrite (AccessWidth8, FCH_IOMAP_REG72, &SaveLocation, StdHeader);
      LibHygonIoRead (AccessWidth8, FCH_IOMAP_REG73, &PortSataStatusByte, StdHeader);
      PortStatusDword = (UINT32)PortSataStatusByte << 0x10;
      FchSmnRW (
        DieBusNum,
        SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_SLOR_HYGX) + 0x00,
        0xFF00FFFF,
        PortStatusDword,
        StdHeader
        );
      SaveLocation = 1;
      LibHygonIoWrite (AccessWidth8, FCH_IOMAP_REG72, &SaveLocation, StdHeader);
      LibHygonIoRead (AccessWidth8, FCH_IOMAP_REG73, &PortSataStatusByte, StdHeader);
      FchSmnRW (
        DieBusNum,
        SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG0C,
        0xFFFFFF00,
        (UINT32)PortSataStatusByte,
        StdHeader
        );
    }
  }

  FchSmnRead (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG0C,
    &PortStatusDword,
    StdHeader
    );
  PortSataStatusByte = (UINT8)(PortStatusDword & 0xFF);

  //
  // if all ports are in disabled state, report at least one port
  //
  if ((PortSataStatusByte & 0xFF) == 0) {
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG0C,
      0xFFFFFF00,
      0x01,
      StdHeader
      );
  }

  FchSmnRead (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG0C,
    &PortStatusDword,
    StdHeader
    );
  PortSataStatusByte = (UINT8)(PortStatusDword & 0xFF);

  for (PortNumByte = 0; PortNumByte < TAISHAN_SATA_PORT_NUM; PortNumByte++) {
    if (PortSataStatusByte & (1 << PortNumByte)) {
      NumOfPorts++;
    }
  }

  if ( NumOfPorts == 0) {
    NumOfPorts = 0x01;
  }

  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG00,
    0xFFFFFFE0,
    (UINT32)(NumOfPorts - 1),
    StdHeader
    );
}

/**
 * sataSetIrqIntResource - Config SATA IRQ/INT# resource
 *
 *
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 * @param[in] StdHeader
 *
 */
VOID
SataSetIrqIntResource (
  IN  VOID                 *FchDataPtr,
  IN  HYGON_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8           ValueByte;
  FCH_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  //
  // IRQ14/IRQ15 come from IDE or SATA
  //
  ValueByte = 0x08;
  LibHygonIoWrite (AccessWidth8, FCH_IOMAP_REGC00, &ValueByte, StdHeader);
  LibHygonIoRead (AccessWidth8, FCH_IOMAP_REGC01, &ValueByte, StdHeader);

  // Only Config SATA IRQ/INT# resource Once
  if ((ValueByte & 0xF0) != 0) {
    return;
  }

  ValueByte = ValueByte & 0x0F;

  if (LocalCfgPtr->Sata.SataClass == SataLegacyIde) {
    ValueByte = ValueByte | 0x50;
  } else {
    //
    // Both IDE & SATA set to Native mode
    //
    ValueByte = ValueByte | 0xF0;
  }

  LibHygonIoWrite (AccessWidth8, FCH_IOMAP_REGC01, &ValueByte, StdHeader);
}

/**
 * FchTSSataGpioInitial - Sata GPIO function Procedure
 *
 *   - Private function
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] SataIndex      Sata index 0 or 1.
 * @param[in] DieBusNum      Bus Number of current Die
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchTSSataGpioInitial (
  IN  UINT8      PhysicalDieId,
  IN  UINT8      SataIndex,
  IN  UINT32     DieBusNum,
  IN  VOID       *FchDataPtr
  )
{
  UINT32          FchSataBarRegDword;
  UINT32          FchAcpiMmioBase;
  FCH_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr     = (FCH_DATA_BLOCK *)FchDataPtr;
  FchAcpiMmioBase = LocalCfgPtr->HwAcpi.FchAcpiMmioBase;

  // 1.Set SGPIO PAD
  // GPIOx00024 0000_0000h 9 AGPIO9_SGPIO_DATAOUT_MDIO1_SCL
  // GPIOx00028 0000_0000h 10 S0A3_GPIO_AGPIO10_SGPIO_CLK_MDIO0_SCL
  // GPIOx0005C 0000_0000h 23 AGPIO23_SGPIO_LOAD_MDIO1_SDA
  // GPIOx000A0 0000_0000h 40 AGPIO40_SGPIO_DATAIN_MDIO0_SDA
  if (SataIndex == 0) {
    RwMem (FchAcpiMmioBase + IOMUX_BASE + FCH_GPIO_REG09, AccessWidth8, 0x00, 0x01);
    RwMem (FchAcpiMmioBase + IOMUX_BASE + FCH_GPIO_REG10, AccessWidth8, 0x00, 0x02);
    RwMem (FchAcpiMmioBase + IOMUX_BASE + FCH_GPIO_REG23, AccessWidth8, 0x00, 0x01);
    RwMem (FchAcpiMmioBase + IOMUX_BASE + FCH_GPIO_REG40, AccessWidth8, 0x00, 0x01);
    //Set GPIO9[20] = 1, make SPIO_DATA_OUT pull up enable
    RwMem (FchAcpiMmioBase + GPIO_BANK0_BASE + 0x24, AccessWidth32, ~(UINT32) BIT20, BIT20);
  } 

  // 2.Support 8 device on 1 SGPIO stream
  // set SPIO_TgtSpt8dev = 0x1, support 8 device on 1 SGPIO stream
  FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_TS_SATA_MISC_CONTROL_HYGX), ~(UINT32)BIT3, BIT3, NULL);

  // 3.Enable SGPIO in Enclosure Management set
  // Program sata_ahci :: FCh[27] =1
  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REGFC,
    ~(UINT32)BIT27,
    BIT27,
    NULL
    );

  // 4.Set SGPIO enable
  // 4.2.3 SGPIO Initialization for Initiator0
  // a. Program GPIO_HYGON_0.ISEL=0
  // Programe sata_ahci::506h = 0xC0
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x506, 0x00, 0xC0, NULL);
  // Programe sata_ahci::507h = 0x00
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x507, 0x00, 0x00, NULL);
  // Programe sata_ahci::508h = 0x01
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x508, 0x00, 0x01, NULL);
  // Programe ISEL=0, sata_ahci::50Ch = 0x00000020 (ISEL=0, GN_AT=1(default), BSY_BYPEN= 0)
  FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x50C, 0x00, 0x00000020, NULL);
  // Programe Sata_ahci::20h[8] = 1
  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG20,
    ~(UINT32)(BIT8),
    BIT8,
    NULL
    );
  do {
    FchSmnRead (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG20,
      &FchSataBarRegDword,
      NULL
      );
  } while (FchSataBarRegDword & BIT8);

  FchStall (5000, NULL);

  // b. Configure GPIO_TX[0]
  // Programe sata_ahci::506h = 0x03
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x506, 0x00, 0x03, NULL);
  // Programe sata_ahci::507h = 0x00
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x507, 0x00, 0x00, NULL);
  // Programe sata_ahci::508h = 0x01
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x508, 0x00, 0x01, NULL);
  // Programe sata_ahci::50Ch = 0xA0A0A0A0 (ACT=0x5 SOF, LOC=0x0, ERR=0x0)
  FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x50C, 0x00, 0xA0A0A0A0, NULL);
  // Programe Sata_ahci::20h[8] = 1
  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG20,
    ~(UINT32)(BIT8),
    BIT8,
    NULL
    );
  do {
    FchSmnRead (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG20,
      &FchSataBarRegDword,
      NULL
      );
  } while (FchSataBarRegDword & BIT8);

  FchStall (5000, NULL);

  // c. Configure GPIO_CFG[1:0] (enable SGPIO initiator0 and activity stretch settings)
  // Programe sata_ahci::506h = 0x00
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x506, 0x00, 0x00, NULL);
  // Programe sata_ahci::507h = 0x00
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x507, 0x00, 0x00, NULL);
  // Programe sata_ahci::508h = 0x02
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x508, 0x00, 0x02, NULL);
  // Programe sata_ahci::50Ch = 0x00800000
  FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x50C, 0x00000000, BIT23, NULL);
  // Programe sata_ahci::510h = 0x0F0F3700
  FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x510, 0x00000000, 0x0F0F3700, NULL);
  // Programe Sata_ahci::20h[8] = 1
  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG20,
    ~(UINT32)(BIT8),
    BIT8,
    NULL
    );
  do {
    FchSmnRead (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG20,
      &FchSataBarRegDword,
      NULL
      );
  } while (FchSataBarRegDword & BIT8);

  FchStall (5000, NULL);

  // 4.2.4 SGPIO Initialization for Initiator1
  // a. Program GPIO_HYGON_0.ISEL=1
  // Programe sata_ahci::506h = 0xC0
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x506, 0x00, 0xC0, NULL);
  // Programe sata_ahci::507h = 0x00
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x507, 0x00, 0x00, NULL);
  // Programe sata_ahci::508h = 0x01
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x508, 0x00, 0x01, NULL);
  // Programe ISEL=0, sata_ahci::50Ch = 0x00000021 (ISEL=1, GN_AT=1(default), BSY_BYPEN= 0)
  FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x50C, 0x00, 0x00000021, NULL);
  // Programe Sata_ahci::20h[8] = 1
  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG20,
    ~(UINT32)(BIT8),
    BIT8,
    NULL
    );
  do {
    FchSmnRead (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG20,
      &FchSataBarRegDword,
      NULL
      );
  } while (FchSataBarRegDword & BIT8);

  FchStall (5000, NULL);

  // b. Configure GPIO_TX[0]
  // Programe sata_ahci::506h = 0x03
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x506, 0x00, 0x03, NULL);
  // Programe sata_ahci::507h = 0x00
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x507, 0x00, 0x00, NULL);
  // Programe sata_ahci::508h = 0x01
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x508, 0x00, 0x01, NULL);
  // Programe sata_ahci::50Ch = 0xA0A0A0A0 (ACT=0x5 SOF, LOC=0x0, ERR=0x0)
  FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x50C, 0x00, 0xA0A0A0A0, NULL);
  // Programe Sata_ahci::20h[8] = 1
  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG20,
    ~(UINT32)(BIT8),
    BIT8,
    NULL
    );
  do {
    FchSmnRead (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG20,
      &FchSataBarRegDword,
      NULL
      );
  } while (FchSataBarRegDword & BIT8);

  FchStall (5000, NULL);

  // c. Configure GPIO_CFG[1:0] (enable SGPIO initiator1 and activity stretch settings)
  // Programe sata_ahci::506h = 0x00
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x506, 0x00, 0x00, NULL);
  // Programe sata_ahci::507h = 0x00
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x507, 0x00, 0x00, NULL);
  // Programe sata_ahci::508h = 0x02
  FchSmnRW8 (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x508, 0x00, 0x02, NULL);
  // Programe sata_ahci::50Ch = 0x00800000
  FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x50C, 0x00000000, BIT23, NULL);
  // Programe sata_ahci::510h = 0x0F0F3700
  FchSmnRW (DieBusNum, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + 0x510, 0x00000000, 0x0F0F3700, NULL);
  // Programe Sata_ahci::20h[8] = 1
  FchSmnRW (
    DieBusNum,
    SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG20,
    ~(UINT32)(BIT8),
    BIT8,
    NULL
    );
  do {
    FchSmnRead (
      DieBusNum,
      SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_SMN_SATA_CONTROL_BAR5_HYGX) + FCH_SATA_BAR5_REG20,
      &FchSataBarRegDword,
      NULL
      );
  } while (FchSataBarRegDword & BIT8);

  FchStall (5000, NULL);
}

