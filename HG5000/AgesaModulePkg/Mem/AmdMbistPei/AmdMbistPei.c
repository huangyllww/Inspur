/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory API, and related functions.
 *
 * Contains code that initializes memory
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision:  $   @e \$Date: $
 *
 */
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
 ******************************************************************************
 */
/*++
Module Name:

  AmdMbistPei.c
  Memory MBIST Pei Module

Abstract:
--*/

#include "PiPei.h"
#include <Agesa.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Guid/AmdMbistHob.h>
#include <Ppi/AmdMbistPpi.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/AmdPspApobLib.h>
#include <Library/HobLib.h>
#include "Filecode.h"

#define FILECODE MEM_AMDMBISTPEI_AMDMBISTPEI_FILECODE


VOID
UpdateMbistTestResults (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN APOB_MBIST_STATUS_TYPE_STRUCT  *ApobMBistData,
  IN OUT MBIST_TEST_STATUS        *MbistTestResults
)
/*++

Routine Description:

  This function constructs the results of MBIST Interface test from APOB and
  updates them in MBIST_TEST_STATUS structure.

Arguments:

  PeiServices     -     PeiServices
  ApobMBistData   -     Mbist interface test results.
  MbistTestResults -    Updated Mbist Test Status for each chipselect in a DIE.

Returns:
  None
--*/
{
  UINTN ChIndex;
  UINTN StIndex;
  for (StIndex = 0; StIndex < APOB_MBIST_SUBTESTS; StIndex++) {
    for (ChIndex = 0; ChIndex < 2; ChIndex++) {
      MbistTestResults->ResultValid [StIndex][ChIndex] =  ApobMBistData->ResultValid[StIndex][ChIndex];
      MbistTestResults->ErrorStatus [StIndex][ChIndex] =  ApobMBistData->ErrorStatus[StIndex][ChIndex];
    }
  }
}

VOID
UpdateDataEyeResults (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN APOB_MBIST_STATUS_TYPE_STRUCT  *ApobMbistData,
  IN OUT MBIST_DATA_EYE_MARGIN_RECORD  *MbistDataEye
)
/*++

Routine Description:

  This function constructs the results of MBIST DataEye Margin from APOB and
  updates them in MBIST_DATA_EYE  structure.

Arguments:

  PeiServices     -     PeiServices
  ApobMBistData   -     Mbist interface test results.
  MbistDataEye    -     Updated Mbist Data Eye Magin for Read and Write DQ for
                        each chipselect in a DIE along with Read and Write Vref

Returns:
  None
--*/
{
  UINTN ChIndex;
  UINTN CsIndex;
  IDS_HDT_CONSOLE (MAIN_FLOW, "\t Updating DataEye Results from APOB\n");
  IDS_HDT_CONSOLE (MAIN_FLOW, "\t Chnl  ChipSel  iRdDq  oRdDq  iRdVref  oRdVref, WrdDq  oWrDq  iWrVref  oWrVref\n");
  for (ChIndex = 0; ChIndex < MAX_CHANNELS_PER_DIE; ChIndex++) {
    for (CsIndex = 0; CsIndex < MAX_CHIPSELECT_PER_CHANNEL; CsIndex++) {
      MbistDataEye->IsDataEyeValid = ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeValid;
      MbistDataEye->DataEyeMargin.ReadDqDelay.PositiveEdge = ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.ReadDqDelay.PositiveEdge;
      MbistDataEye->DataEyeMargin.ReadDqDelay.NegativeEdge = ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.ReadDqDelay.NegativeEdge;
      MbistDataEye->DataEyeMargin.ReadVref.PositiveEdge = ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.ReadVref.PositiveEdge;
      MbistDataEye->DataEyeMargin.ReadVref.NegativeEdge = ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.ReadVref.NegativeEdge;
      MbistDataEye->DataEyeMargin.WriteDqDelay.PositiveEdge = ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.WriteDqDelay.PositiveEdge;
      MbistDataEye->DataEyeMargin.WriteDqDelay.NegativeEdge = ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.WriteDqDelay.NegativeEdge;
      MbistDataEye->DataEyeMargin.WriteVref.PositiveEdge = ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.WriteVref.PositiveEdge;
      MbistDataEye->DataEyeMargin.WriteVref.NegativeEdge = ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.WriteVref.NegativeEdge;

      IDS_HDT_CONSOLE (MAIN_FLOW, "\t %d    %d    +%02x   +%02x   +%02x  +%02x  +%02x   +%02x   +%02x  +%02x\n",
           ChIndex, CsIndex,
           ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.ReadDqDelay.PositiveEdge,
           MbistDataEye->DataEyeMargin.ReadDqDelay.PositiveEdge,
           ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.ReadVref.PositiveEdge,
           MbistDataEye->DataEyeMargin.ReadVref.PositiveEdge,
           ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.WriteDqDelay.PositiveEdge,
           MbistDataEye->DataEyeMargin.WriteDqDelay.PositiveEdge,
           ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.WriteVref.PositiveEdge,
           MbistDataEye->DataEyeMargin.WriteVref.PositiveEdge
           );

      IDS_HDT_CONSOLE (MAIN_FLOW, "\t %d    %d    -%02x   -%02x   -%02x  -%02x  -%02x   -%02x   -%02x  -%02x\n"  ,
           ChIndex, CsIndex,
           ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.ReadDqDelay.NegativeEdge,
           MbistDataEye->DataEyeMargin.ReadDqDelay.NegativeEdge,
           ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.ReadVref.NegativeEdge,
           MbistDataEye->DataEyeMargin.ReadVref.NegativeEdge,
           ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.WriteDqDelay.PositiveEdge,
           MbistDataEye->DataEyeMargin.WriteDqDelay.NegativeEdge,
           ApobMbistData->ApobDataEyeRecord[ChIndex][CsIndex].DataEyeRecord.WriteVref.NegativeEdge,
           MbistDataEye->DataEyeMargin.WriteVref.NegativeEdge
           );
      MbistDataEye++;
    }
  }
}

EFI_STATUS
GetMbistTestResults (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINT32  *BufferSize,
  IN OUT MBIST_TEST_STATUS  *MbistTestResults
  )
/*++

Routine Description:

  Extract DataEye  Margin results for given MBIST_DATA_EYE for all the supported
  socket and Die. Based off the field which determines, if the data is valid
  it can be utilized accordignly.

Arguments:
  PeiServices   - PeiServices
  BufferSize    - Total Buffer Size of the data
  MbistTestResults  - Pointer to MBIST_TEST_STATUS structure

Returns:

  EFI_STATUS - Status Code
--*/
{
  EFI_STATUS  Status;
  AMD_MEMORY_MBIST_TEST_RESULTS_PPI *MbistTestStatusPpi;
  UINTN SizeOfData;

  SizeOfData = (sizeof (MBIST_TEST_STATUS) * MAX_SOCKETS_SUPPORTED * MAX_DIES_PER_SOCKET);

  if (*BufferSize < SizeOfData) {
    *BufferSize = SizeOfData;
    return EFI_BUFFER_TOO_SMALL;
  }

  *BufferSize = SizeOfData;
  Status = (*PeiServices)->LocatePpi (
                PeiServices,
                &gAmdMbistPeiPpiGuid,
                0,
                NULL,
                (VOID**)&MbistTestStatusPpi
                );
  if (Status != EFI_SUCCESS) {
    return EFI_NOT_FOUND;
  }
  CopyMem (MbistTestResults, &MbistTestStatusPpi->MbistTestStatus[0][0], *BufferSize);
  return EFI_SUCCESS;
}

EFI_STATUS
GetDataEyeResults (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINT32  *BufferSize,
  IN OUT MBIST_DATA_EYE  *MbistDataEye
  )
/*++

Routine Description:

  Extract DataEye  Margin results for given MBIST_DATA_EYE for all the supported
  socket and Die. Based off the field which determines, if the data is valid
  it can be utilized accordignly.

Arguments:
  PeiServices   - PeiServices
  BufferSize    - Total Buffer Size of the data
  MbistDataEye  - Pointer to MBIST_DATA_EYE structure

Returns:

  EFI_STATUS - Status Code

--*/
{
  EFI_STATUS  Status;
  AMD_MEMORY_MBIST_TEST_RESULTS_PPI *MbistTestStatusPpi;
  UINTN SizeOfData;

  SizeOfData = (sizeof (MBIST_DATA_EYE) * MAX_SOCKETS_SUPPORTED * MAX_DIES_PER_SOCKET);

  if (*BufferSize < SizeOfData) {
    *BufferSize = SizeOfData;
    return EFI_BUFFER_TOO_SMALL;
  }

  *BufferSize = SizeOfData;
  Status = (*PeiServices)->LocatePpi (
                PeiServices,
                &gAmdMbistPeiPpiGuid,
                0,
                NULL,
                (VOID**)&MbistTestStatusPpi
                );
  if (Status != EFI_SUCCESS) {
    return EFI_NOT_FOUND;
  }
  CopyMem (MbistDataEye, &MbistTestStatusPpi->MbistDataEyeMargin[0][0], *BufferSize);
  return EFI_SUCCESS;
}

EFI_STATUS
BuildMbistHobInfo (
  IN CONST  EFI_PEI_SERVICES    **PeiServices,
  IN AMD_MEMORY_MBIST_TEST_RESULTS_PPI  *This,
  IN UINT8                      NumSocket,
  IN UINT8                      NumDie,
  IN UINT8                      NumSubTest,
  IN UINT8                      NumChannel
  )
/*++

Routine Description:

  This function build HOB for the Mbist Test Status and Mbist Data Eye Margin
  for Read and Write DQ as well as their Vref.

Arguments:

  PeiServices    -   PeiServices
  This           -   Pointer to AMD_MEMORY_MBIST_TEST_RESULTS_PPI to extract the
                     tables
  NumSocket      -   Number of Socket
  NumDie         -   Number of Dies
  NumSubTest     -   Number of SubTest
  NumChannel     -   Number of Channel

Returns:
  EFI_STATUS - Status code
--*/
{
  UINT8   Socket;
  UINT8   Channel;
  UINT8   Die;
  UINT8   SubTest;
  UINT8 Chipselect;
  AMD_MEMORY_MBIST_HOB    MbistResultsHob;

  MbistResultsHob.NumSockets = NumSocket;
  MbistResultsHob.NumDiePerSocket = NumDie;
  MbistResultsHob.NumSubTests = NumSubTest;
  MbistResultsHob.NumChannelPerDie = NumChannel;

  IDS_HDT_CONSOLE (MAIN_FLOW, "MBIST Test Results\n");
  for (Socket = 0; Socket < NumSocket; Socket++) {
    for (Die = 0; Die < MAX_DIES_PER_SOCKET; Die++) {
      for (SubTest = 0; SubTest < NumSubTest; SubTest++) {
        for (Channel = 0; Channel < NumChannel; Channel++) {
          MbistResultsHob.MbistTestStatus[Socket][Die][SubTest][Channel].ResultValid = This->MbistTestStatus[Socket][Die].ResultValid[SubTest][Channel];
          MbistResultsHob.MbistTestStatus[Socket][Die][SubTest][Channel].ErrorStatus = This->MbistTestStatus[Socket][Die].ErrorStatus[SubTest][Channel];
          IDS_HDT_CONSOLE (MAIN_FLOW, " Input Data: IsValid[%d][%d][%d][%d].Valid:        0x%x", Socket, Die, SubTest, Channel, This->MbistTestStatus[Socket][Die].ResultValid[SubTest][Channel]);
          IDS_HDT_CONSOLE (MAIN_FLOW, "             HOB Mbist IsValid[%d][%d][%d][%d].Valid: 0x%x\n", Socket, Die, SubTest, Channel, MbistResultsHob.MbistTestStatus[Socket][Die][SubTest][Channel].ResultValid);
          IDS_HDT_CONSOLE (MAIN_FLOW, " Input Data ResultStatus[%d][%d][%d][%d].MbistResults: 0x%x", Socket, Die, SubTest, Channel, This->MbistTestStatus[Socket][Die].ErrorStatus[SubTest][Channel]);
          IDS_HDT_CONSOLE (MAIN_FLOW, "      HOB Mbist Status[%d][%d][%d][%d].MbistResults: 0x%x\n", Socket, Die, SubTest, Channel, MbistResultsHob.MbistTestStatus[Socket][Die][SubTest][Channel].ErrorStatus);
        }
      }
    } // for Die loop
  } // for Socket loop

  IDS_HDT_CONSOLE (MAIN_FLOW, "MBIST Data Eye Worst Case Margin\n");
  IDS_HDT_CONSOLE (MAIN_FLOW, "\t Soc Die  Chnl  ChipSel  iRdDq  oRdDq  iRdVref  oRdVref, WrdDq  oWrDq  iWrVref  oWrVref\n");
  for (Socket = 0; Socket < NumSocket; Socket++) {
    for (Die = 0; Die < MAX_DIES_PER_SOCKET; Die++) {
      for (Channel = 0; Channel < NumChannel; Channel++) {
        for (Chipselect = 0; Chipselect < MAX_CHIPSELECT_PER_CHANNEL; Chipselect++) {
          MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].IsDataEyeValid = This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].IsDataEyeValid;
          MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].ReadDqDelay.PositiveEdge =  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.ReadDqDelay.PositiveEdge;
          MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].ReadDqDelay.NegativeEdge =  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.ReadDqDelay.NegativeEdge;
          MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].ReadVref.PositiveEdge =  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.ReadVref.PositiveEdge;
          MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].ReadVref.NegativeEdge =  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.ReadVref.NegativeEdge;

          MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].WriteDqDelay.PositiveEdge =  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.WriteDqDelay.PositiveEdge;
          MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].WriteDqDelay.NegativeEdge =  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.WriteDqDelay.NegativeEdge;
          MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].WriteVref.PositiveEdge =  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.WriteVref.PositiveEdge;
          MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].WriteVref.NegativeEdge =  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.WriteVref.NegativeEdge;
          IDS_HDT_CONSOLE (MAIN_FLOW, "\t %d   %d    %d    %d    +%02x   +%02x   +%02x  +%02x  +%02x   +%02x   +%02x  +%02x\n",
                  Socket, Die, Channel, Chipselect,
                  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.ReadDqDelay.PositiveEdge,
                  MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].ReadDqDelay.PositiveEdge,
                  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.ReadVref.PositiveEdge,
                  MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].ReadVref.PositiveEdge,
                  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.WriteDqDelay.PositiveEdge,
                  MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].WriteDqDelay.PositiveEdge,
                  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.WriteVref.PositiveEdge,
                  MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].WriteVref.PositiveEdge
           );
          IDS_HDT_CONSOLE (MAIN_FLOW, "\t %d   %d    %d    %d    -%02x   -%02x   -%02x  -%02x  -%02x   -%02x   -%02x  -%02x\n"  ,
                  Socket, Die, Channel, Chipselect,
                  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.ReadDqDelay.NegativeEdge,
                  MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].ReadDqDelay.NegativeEdge,
                  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.ReadVref.NegativeEdge,
                  MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].ReadVref.NegativeEdge,
                  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.WriteDqDelay.PositiveEdge,
                  MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].WriteDqDelay.NegativeEdge,
                  This->MbistDataEyeMargin[Socket][Die].MbistDataEyeRecord[Channel][Chipselect].DataEyeMargin.WriteVref.NegativeEdge,
                  MbistResultsHob.MbistDataEyeMargin[Socket][Die][Channel][Chipselect].WriteVref.NegativeEdge
           );

        } //for Chipselect
      } // for channel
    } // for Die loop
  } // for Socket loop
  if (BuildGuidDataHob (&gAmdMemoryMbistHobGuid, &MbistResultsHob, sizeof (AMD_MEMORY_MBIST_HOB)) == NULL) {
     return EFI_NOT_FOUND;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmdMbistPeimZp (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_PEI_PPI_DESCRIPTOR                *MbistInitPpiList;
  APOB_TYPE_HEADER                      *ApobEntry;
  APOB_MBIST_STATUS_TYPE_STRUCT         ApobMbistResult;
  AMD_MEMORY_MBIST_TEST_RESULTS_PPI     *AmdMBistResultsPpi;
  AMD_PEI_FABRIC_TOPOLOGY_SERVICES_PPI *FabricTopologyServicesPpi;
  EFI_STATUS                            Status;
  UINTN NumberOfProcessors;
  UINTN TotalNumberOfDie;
  UINTN SocketIndex;
  UINTN DieIndex;
  UINT32 InstanceId;

  ApobEntry = NULL;
  InstanceId = 0;

  IDS_HDT_CONSOLE (MAIN_FLOW, "  MBIST Driver Entry\n");
  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (AMD_MEMORY_MBIST_TEST_RESULTS_PPI), &AmdMBistResultsPpi);
  ZeroMem ( AmdMBistResultsPpi, sizeof (AMD_MEMORY_MBIST_TEST_RESULTS_PPI));

  Status = (**PeiServices).LocatePpi (
                PeiServices,
                &gAmdFabricTopologyServicesPpiGuid,
                0,
                NULL,
                &FabricTopologyServicesPpi
                );
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  FabricTopologyServicesPpi->GetSystemInfo (&NumberOfProcessors, &TotalNumberOfDie, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "  Total Socket: %d, TotalDie: %d\n", NumberOfProcessors, TotalNumberOfDie);
  for (SocketIndex = 0; SocketIndex < NumberOfProcessors; SocketIndex++) {
    for ( DieIndex = 0 ; DieIndex < ABL_APOB_MAX_DIES_PER_SOCKET; DieIndex++) {
      InstanceId = (((UINT32) SocketIndex << 8) | (UINT32) DieIndex);
      Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_MBIST_RESULT_INFO_TYPE, InstanceId, FALSE, &ApobEntry);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  SocketIndex: %d, DieIndex: %d, InstanceId: %d, Apob Status: %x\n", SocketIndex, DieIndex, InstanceId, Status );
      if (Status == EFI_SUCCESS) {
        //
        // Make sure all the data from various instance is collected here
        //
        CopyMem (&ApobMbistResult, ApobEntry, sizeof (APOB_MBIST_STATUS_TYPE_STRUCT));
        UpdateMbistTestResults (PeiServices, &ApobMbistResult, &AmdMBistResultsPpi->MbistTestStatus[SocketIndex][DieIndex]);
        UpdateDataEyeResults (PeiServices, &ApobMbistResult, &AmdMBistResultsPpi->MbistDataEyeMargin[SocketIndex][DieIndex].MbistDataEyeRecord[0][0]);
      }
    }
  }

  //
  //Initialize the PpiList and Functions in Ppi
  //
  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (EFI_PEI_PPI_DESCRIPTOR), &MbistInitPpiList);
  MbistInitPpiList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  MbistInitPpiList->Guid  = &gAmdMbistPeiPpiGuid;
  MbistInitPpiList->Ppi   = AmdMBistResultsPpi;
  AmdMBistResultsPpi->Revision = AMD_MBIST_PPI_REVISION;
  AmdMBistResultsPpi->GetMbistTestResults = GetMbistTestResults;
  AmdMBistResultsPpi->GetMbistDataEyeMargin = GetDataEyeResults;

  Status = (*PeiServices)->InstallPpi (PeiServices, MbistInitPpiList);
  BuildMbistHobInfo (PeiServices, AmdMBistResultsPpi, (UINT8) NumberOfProcessors, (UINT8)(TotalNumberOfDie/NumberOfProcessors), APOB_MBIST_SUBTESTS, ABL_APOB_MAX_CHANNELS_PER_DIE);
  return (Status);
}

