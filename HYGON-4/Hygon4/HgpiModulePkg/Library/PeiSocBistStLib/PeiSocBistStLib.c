/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON SOC BIST Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Lib
 *
 */
/*
 ******************************************************************************
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
#include "Uefi.h"
#include <Library/BaseLib.h>
#include "HYGON.h"
#include "HygonFuse.h"
#include <CddRegistersDm.h>
#include "Filecode.h"
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <GnbRegisters.h>
#include <Library/HygonSocBaseLib.h>
#include  <Library/HygonSmnAddressLib.h>

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_PEISOCBISTSTLIB_PEISOCBISTSTLIB_FILECODE

/*---------------------------------------------------------------------------------------*/

/**
 *
 * Reads the status of CCX BIST
 * @param[in]      SocketNumber           Socket number to read BIST data from
 * @param[in]      CddNumber              Cdd number to read BIST data from
 * @param[in]      CcxNumber              Ccx number to read BIST data from
 * @param[in]      PeiServices            Pointer to PEI services
 *
 *
 * @retval         HGPI_ALERT            A BIST error has occurred
 * @retval         HGPI_SUCCESS          No BIST errors have occured
 */
HGPI_STATUS
ReadCcxBistData (
  IN       UINTN            SocketNumber,
  IN       UINTN            CddNumber,
  IN       UINTN            CcxNumber,
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  UINT8                            BistEn;
  UINT8                            ColdResetMBistEn;
  UINT32                           BistStatus;
  UINT32                           Chl3InitPkg1Value;
  UINT32                           PmregInitPkg1Value;
  EFI_STATUS                       CalledStatus;
  PEI_HYGON_NBIO_SMU_SERVICES_PPI  *NbioSmuServices;
  UINT32                           CpuModel;

  CalledStatus = (**PeiServices).LocatePpi (
                                   PeiServices,
                                   &gHygonNbioSmuServicesPpiGuid,
                                   0,
                                   NULL,
                                   &NbioSmuServices
                                   );

  ASSERT (CalledStatus == EFI_SUCCESS);

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    NbioSmuServices->SmuRegisterRead (NbioSmuServices, SocketNumber, CCX_L3_SPACE_HYEX(CddNumber, CcxNumber, CCX0_CHL3_INITPKG1_ADDRESS), &Chl3InitPkg1Value);
    BistEn = (Chl3InitPkg1Value >> BIST_EN_OFFSET) & 0x1;
    IDS_HDT_CONSOLE (MAIN_FLOW, "Chl3InitPkg1Value=0x%X \n",  Chl3InitPkg1Value);

    NbioSmuServices->SmuRegisterRead (NbioSmuServices, SocketNumber, CCX_SPACE_HYEX(CddNumber, CcxNumber, 0, CCX0_PMREG_INITPKG1_ADDRESS), &PmregInitPkg1Value);
    ColdResetMBistEn = (PmregInitPkg1Value >> COLD_RESET_MBIST_EN_OFFSET) & 0x1;
    IDS_HDT_CONSOLE (MAIN_FLOW, "PmregInitPkg1Value=0x%X \n", PmregInitPkg1Value);

    // Check BIST status only if BIST is fuse enabled
    if ((BistEn == 0x1) || (ColdResetMBistEn == 0x1)){
      //read from ChL3FreqStat[0x18080064], bit9=BistDone
      NbioSmuServices->SmuRegisterRead (NbioSmuServices, SocketNumber, CCX_L3_SPACE_HYEX(CddNumber, CcxNumber, CCX0_CHL3_FREQSTAT_ADDRESS), &BistStatus);
      IDS_HDT_CONSOLE (MAIN_FLOW, "ChL3FreqStat=0x%X \n",  BistStatus);
      if (BistStatus & BIST_DONE_HYEX) {
        if ((BistStatus & BIST_PF_HYEX) == 0) {
          return HGPI_ALERT;
        }
      }
    }
  } else if (CpuModel == HYGON_GX_CPU) {
    NbioSmuServices->SmuRegisterRead (NbioSmuServices, SocketNumber, CCX_L3_SPACE_HYGX (CddNumber, CcxNumber, CCX0_CHL3_INITPKG1_ADDRESS), &Chl3InitPkg1Value);
    BistEn = (Chl3InitPkg1Value >> BIST_EN_OFFSET) & 0x1;
    IDS_HDT_CONSOLE (MAIN_FLOW, "Chl3InitPkg1Value=0x%X \n", Chl3InitPkg1Value);

    NbioSmuServices->SmuRegisterRead (NbioSmuServices, SocketNumber, CCX_SPACE_HYGX (CddNumber, CcxNumber, 0, CCX0_PMREG_INITPKG1_ADDRESS), &PmregInitPkg1Value);
    ColdResetMBistEn = (PmregInitPkg1Value >> COLD_RESET_MBIST_EN_OFFSET) & 0x1;
    IDS_HDT_CONSOLE (MAIN_FLOW, "PmregInitPkg1Value=0x%X \n", PmregInitPkg1Value);

    // Check BIST status only if BIST is fuse enabled
    if ((BistEn == 0x1) || (ColdResetMBistEn == 0x1)) {
      // read from ChL3FreqStat[0x18080064], bit9=BistDone
      NbioSmuServices->SmuRegisterRead (NbioSmuServices, SocketNumber, CCX_L3_SPACE_HYGX (CddNumber, CcxNumber, CCX0_CHL3_FREQSTAT_ADDRESS), &BistStatus);
      IDS_HDT_CONSOLE (MAIN_FLOW, "ChL3FreqStat=0x%X \n", BistStatus);
      if (BistStatus & BIST_DONE_HYGX) {
        if ((BistStatus & BIST_PF_HYGX) == 0) {
          return HGPI_ALERT;
        }
      }
    }
  }

  return HGPI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/

/**
 *
 * Reads the status of non-CCX BIST
 *
 * @param[in]      DieNumber              Die number to read data from
 * @param[out]     BistData               Value of BIST status register
 * @param[in]      PeiServices            Pointer to PEI services
 *
 * @retval         HGPI_ALERT            A BIST error has occurred
 * @retval         HGPI_SUCCESS          No BIST errors have occured
 */
HGPI_STATUS
ReadNonCcxBistData (
  IN       UINTN            SocketNumber,
  OUT      UINTN            *BistData,
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  UINT8                            RunMBist;
  UINT32                           RunMBistFuse;
  UINT32                           CpuModel;
  EFI_STATUS                       CalledStatus;
  PEI_HYGON_NBIO_SMU_SERVICES_PPI  *NbioSmuServices;

  CpuModel = GetHygonSocModel();

  CalledStatus = (**PeiServices).LocatePpi (
                                   PeiServices,
                                   &gHygonNbioSmuServicesPpiGuid,
                                   0,
                                   NULL,
                                   &NbioSmuServices
                                   );

  ASSERT (CalledStatus == EFI_SUCCESS);

  RunMBist = 0;
  RunMBistFuse = 0;
  if (CpuModel == HYGON_EX_CPU) {
    NbioSmuServices->SmuRegisterRead (NbioSmuServices, SocketNumber, IOD_SPACE (0, IOD_MP0_RUN_MBIST_HYEX), &RunMBistFuse);
    RunMBist = (RunMBistFuse >> IOD_RUN_MBIST_FUSE_BITS_HYEX) & 0x1;
  }
  else if (CpuModel == HYGON_GX_CPU) {
    NbioSmuServices->SmuRegisterRead (NbioSmuServices, SocketNumber, IOD_SPACE (0, IOD_MP0_RUN_MBIST_HYGX), &RunMBistFuse);
    RunMBist = (RunMBistFuse >> IOD_RUN_MBIST_FUSE_BITS_HYGX) & 0x1;
  }

  if (RunMBist == 0x1) {
    NbioSmuServices->SmuRegisterRead (NbioSmuServices, SocketNumber, IOD_SPACE (0, 0x01A10038), BistData); // MP0 MMIO Public

    if (*BistData != 0) {
      return HGPI_ALERT;
    }
  }

  return HGPI_SUCCESS;
}
