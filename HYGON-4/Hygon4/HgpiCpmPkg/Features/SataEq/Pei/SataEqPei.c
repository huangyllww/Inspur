/* $NoKeywords:$ */
/**
 * @file
 *
 * HYGON Sata TX EQ
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  SataTxEq
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
#include "SataEqPei.h"
#include <Library/HygonSocBaseLib.h>
#include <TsFch.h>

SATA_EQ_INIT_DATE SataEqInitData = {
  FALSE, {0x0,0x16,0xA}, //SATA GEN1 Tx Cursor
  FALSE, {0x0,0x16,0xA}, //SATA GEN2 Tx Cursor
  TRUE,  {0x0,0x16,0xA}  //SATA GEN3 Tx Cursor
};

/*----------------------------------------------------------------------------------------*/
/**
 * This function to set sata eq 
 *
 * @param[in]     CpmTablePpi    CPM Table
 * @param[in]     Socket         socket number
 * @param[in]     IodId          IodId number
 * @param[in]     SataIndex      SataIndex number
 * @param[in]     Port           Port number
 * @param[in]     Speed          Speed number
 * @param[out]    TxCursor       Eq param
 *
 */ 
VOID
CpmSetSataTxCursor (
  HYGON_CPM_TABLE_PPI  *CpmTablePpi,
  UINT8                Socket,
  UINT8                IodId,
  UINT8                SataIndex,
  UINT8                Port,
  SATA_SPEED           Speed,
  SATA_TX_CURSOR       *TxCursor
  )
{
  UINT32             SmnBase;
  UINT32             SmnOffset;
  PCS_LANE_COEFF1    LanCoeff1;
  PCS_LANE_COEFF2    LanCoeff2;
  PCS_LANE_COEFF3    LanCoeff3;
  UINT32             CpuModel;
 
  //Calculate lan coff address by speed and port
  CpuModel = GetHygonSocModel();
  SmnBase  = 0;
  if (CpuModel == HYGON_EX_CPU) {
    SmnBase = SATA_SPACE_HYEX (SataIndex, PCS_LANE_COEFF_SMN_BASE_HYEX);
  } else if (CpuModel == HYGON_GX_CPU) {
    SmnBase = SATA_SPACE_HYGX (IodId, SataIndex, PCS_LANE_COEFF_SMN_BASE_HYGX);
  }
  
  SmnOffset = ((Speed * 4) << 4) + Port * 4;

  //Dump setting info
  DEBUG ((EFI_D_INFO, "[Socket%x IodId%x SataIndex%x Port%x Gen%x] - CpmSetSataTxCursor:\n", Socket, IodId, SataIndex, Port, Speed));   // byo230914 -
  DEBUG ((EFI_D_INFO, "  Smn(0x%08x) - TxPreCursor(0x%08x)", SmnBase + SmnOffset, TxCursor->Pre));                                      // byo230914 -
  DEBUG ((EFI_D_INFO, " -TxMainCursor(0x%08x) - TxPostCursor(0x%08x)\n", TxCursor->Main, TxCursor->Post));                              // byo230914 -

  switch (Speed)
  {
    //Gen 1 select
    case SATA_GEN1:
      LanCoeff1.Reg32 = CpmTablePpi->CommonFunction.SmnRead32 (CpmTablePpi, Socket, SmnBase, SmnOffset);
      LanCoeff1.Bits.TxCoefficientPreCursorGen1  = TxCursor->Pre;
      LanCoeff1.Bits.TxCoefficientMainCursorGen1 = TxCursor->Main;
      LanCoeff1.Bits.TxCoefficientPostCursorGen1 = TxCursor->Post;
      CpmTablePpi->CommonFunction.SmnWrite32 (CpmTablePpi, Socket, SmnBase, SmnOffset, LanCoeff1.Reg32);
      break;
    //Gen 2 select
    case SATA_GEN2:
      LanCoeff2.Reg32 = CpmTablePpi->CommonFunction.SmnRead32 (CpmTablePpi, Socket, SmnBase, SmnOffset);
      LanCoeff2.Bits.TxCoefficientPreCursorGen2  = TxCursor->Pre;
      LanCoeff2.Bits.TxCoefficientMainCursorGen2 = TxCursor->Main;
      LanCoeff2.Bits.TxCoefficientPostCursorGen2 = TxCursor->Post;
      CpmTablePpi->CommonFunction.SmnWrite32 (CpmTablePpi, Socket, SmnBase, SmnOffset, LanCoeff2.Reg32);
      break;
    //Gen 3 select
    case SATA_GEN3:
      LanCoeff3.Reg32 = CpmTablePpi->CommonFunction.SmnRead32 (CpmTablePpi, Socket, SmnBase, SmnOffset);
      LanCoeff3.Bits.TxCoefficientPreCursorGen3  = TxCursor->Pre;
      LanCoeff3.Bits.TxCoefficientMainCursorGen3 = TxCursor->Main;
      LanCoeff3.Bits.TxCoefficientPostCursorGen3 = TxCursor->Post;
      CpmTablePpi->CommonFunction.SmnWrite32 (CpmTablePpi, Socket, SmnBase, SmnOffset, LanCoeff3.Reg32);
      break;
    default:
      break;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the Sata Eq
 *
 * This function to tune sata Eq
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
SataEqPeiEntry (
  IN CPM_PEI_FILE_HANDLE         FileHandle,
  IN CPM_PEI_SERVICES            **PeiServices
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
  SATA_TX_CURSOR                          TxCursor;
  HYGON_CPM_TABLE_PPI                     *CpmTablePpi;
  HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI  *FabricTopologyPpi;
  UINTN                                   NumberOfSockets;
  UINTN                                   TotalNumberOfIoDie;
  UINTN                                   TotalNumberRootBridges;
  UINT32                                  CpuModel;
  UINT32                                  IodCount;
  UINT8                                   AhciCount;
  UINT8                                   SataPortCount;

  DEBUG ((EFI_D_INFO, "Hygon Cpm Sata Eq Entry\n"));             // byo230914 -
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

  //1.init Sata EQ setting by Sata Eq init table
  Status = (*PeiServices)->LocatePpi (
                             (CPM_PEI_SERVICES**)PeiServices,
                             &gHygonFabricTopologyServicesPpiGuid,
                             0,
                             NULL,
                             (VOID**)&FabricTopologyPpi
                             );
  if (!EFI_ERROR(Status)) {
    DEBUG ((EFI_D_INFO, "CpmSetSataEq INIT Setting...\n"));      // byo230914 -
    FabricTopologyPpi->GetSystemInfo (&NumberOfSockets, &TotalNumberOfIoDie, &TotalNumberRootBridges);
    
    CpuModel = GetHygonSocModel();
    if (CpuModel == HYGON_EX_CPU) {
      IodCount      = (UINT8)FabricTopologyGetNumberOfPhysicalDiesOnSocket(0);
      AhciCount     = SATA_NUMBER_PER_SATORI;
      SataPortCount = SATA_PORT_NUMBER_PER_AHCI_ON_SATORI;
    } else if (CpuModel == HYGON_GX_CPU) {
      IodCount      = PcdGet8(PcdHygonDujiangDieCountPerProcessor);
      AhciCount     = SATA_NUMBER_PER_DUJIANG;
      SataPortCount = SATA_PORT_NUMBER_PER_DJ;
    }
    
    for (i = 0; i < NumberOfSockets; i++) {      
      for (j = 0; j < IodCount; j++) {
        for (k =0; k < AhciCount; k++) {
          for (m = 0; m < SataPortCount; m++) {
            if (SataEqInitData.InitGen1Cursor) {
              CpmSetSataTxCursor (CpmTablePpi, i, j, k, m, SATA_GEN1, &SataEqInitData.SataGen1Cursor);
            }
            if (SataEqInitData.InitGen2Cursor) {
              CpmSetSataTxCursor (CpmTablePpi, i, j, k, m, SATA_GEN2, &SataEqInitData.SataGen2Cursor);
            }
            if (SataEqInitData.InitGen3Cursor) {
              CpmSetSataTxCursor (CpmTablePpi, i, j, k, m, SATA_GEN3, &SataEqInitData.SataGen3Cursor);
            }   
          }//port loop
        }//Ahci loop
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

  //3.Do Sata EQ Configuration
  DEBUG ((EFI_D_INFO, "CpmSetSataEq User Setting...\n"));        // byo230914 -
  for (i = 0; (EqCfgTable->EqCfgList[i].Flag & EQ_TABLE_END) != EQ_TABLE_END; i++) {
    if (((EqCfgTable->EqCfgList[i].Flag & FLAG_USED) != FLAG_USED) &&
        ((EqCfgTable->EqCfgList[i].Flag & FLAG_THROW) != FLAG_THROW) &&
        ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) == SATA_EQ_TYPE))  
    {
      Socket        = EqCfgTable->EqCfgList[i].Socket;
      IodId         = EqCfgTable->EqCfgList[i].PhysicalIodId;
      Nbio          = EqCfgTable->EqCfgList[i].Nbio;
      Port          = EqCfgTable->EqCfgList[i].SataPort;
      TxCursor.Pre  = EqCfgTable->EqCfgList[i].SataCfgValue[0];
      TxCursor.Main = EqCfgTable->EqCfgList[i].SataCfgValue[1];
      TxCursor.Post = EqCfgTable->EqCfgList[i].SataCfgValue[2];
      switch (EqCfgTable->EqCfgList[i].Type) {
        case SATA_EQ_GEN1_CURSOR:
          CpmSetSataTxCursor (CpmTablePpi, Socket, IodId, Nbio, Port, SATA_GEN1, &TxCursor);
          break;
        case SATA_EQ_GEN2_CURSOR:
          CpmSetSataTxCursor (CpmTablePpi, Socket, IodId, Nbio, Port, SATA_GEN2, &TxCursor);
          break;
        case SATA_EQ_GEN3_CURSOR:
          CpmSetSataTxCursor (CpmTablePpi, Socket, IodId, Nbio, Port, SATA_GEN3, &TxCursor);
          break;
        default:
          break;
      }
    }
  }

  DEBUG ((EFI_D_INFO, "Hygon Cpm Sata Eq Exit\n"));              // byo230914 -
  return EFI_SUCCESS;
}