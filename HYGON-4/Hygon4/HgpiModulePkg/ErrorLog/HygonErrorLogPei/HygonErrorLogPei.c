/*
****************************************************************************
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
/*++
Module Name:

  HygonErrorLogPei.c
  Init Soc interface

Abstract:
--*/

#include <Filecode.h>
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HygonErrorLogLib.h>
#include <Ppi/HygonErrorLogServicePpi.h>
#include <Ppi/HygonErrorLogPpi.h>
#include "HygonErrorLogPei.h"

#define FILECODE  ERRORLOG_HYGONERRORLOGPEI_HYGONERRORLOGPEI_FILECODE

STATIC PEI_HYGON_ERROR_LOG_READY_PPI  mHygonErrorLogReadyPpi = {
  HYGON_ERROR_LOG_PPI_REVISION,
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mErrorLogReadyPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonErrorLogReadyPpiGuid,
  &mHygonErrorLogReadyPpi
};

STATIC PEI_HYGON_ERROR_LOG_FULL_PPI  mHygonErrorLogFullPpi = {
  HYGON_ERROR_LOG_PPI_REVISION,
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mErrorLogFullPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonErrorLogFullPpiGuid,
  &mHygonErrorLogFullPpi
};

STATIC PEI_HYGON_ERROR_LOG_FULL_PPI  mHygonErrorLogAvailablePpi = {
  HYGON_ERROR_LOG_PPI_REVISION,
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mErrorLogAvailablePpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonErrorLogAvailablePpiGuid,
  &mHygonErrorLogAvailablePpi
};

/*---------------------------------------------------------------------------------------*/

/**
 *
 * This function logs HGPI Errors into the Error log.
 *
 * It will put the information in a circular buffer consisting of 16 such log
 * entries. If the buffer gets full, then the next Error log entry will be written
 * over the oldest Error log entry.
 *
 * @param[in]   PeiServices
 * @param[in]   ErrorClass   The severity of the Error, its associated HGPI_STATUS.
 * @param[in]   ErrorInfo    Uniquely identifies the Error.
 * @param[in]   DataParam1   Error specific additional data
 * @param[in]   DataParam2   Error specific additional data
 * @param[in]   DataParam3   Error specific additional data
 * @param[in]   DataParam4   Error specific additional data
 *
 */
EFI_STATUS
EFIAPI
HygonErrorLogPei (
  IN       PEI_HYGON_ERROR_LOG_PPI   *This,
  IN       HYGON_STATUS ErrorClass,
  IN       UINT32 ErrorInfo,
  IN       UINT32 DataParam1,
  IN       UINT32 DataParam2,
  IN       UINT32 DataParam3,
  IN       UINT32 DataParam4
  )
{
  EFI_STATUS             Status = EFI_SUCCESS;
  HYGON_ERROR_ENTRY      HygonErrorEntry;
  ERROR_LOG_PPI_PRIVATE  *Private;

  Private = (ERROR_LOG_PPI_PRIVATE *)This;

  HygonErrorEntry.ErrorClass = ErrorClass;
  HygonErrorEntry.ErrorInfo  = ErrorInfo;
  HygonErrorEntry.DataParam1 = DataParam1;
  HygonErrorEntry.DataParam2 = DataParam2;
  HygonErrorEntry.DataParam3 = DataParam3;
  HygonErrorEntry.DataParam4 = DataParam4;

  AddErrorLog (Private->ErrorLogBuffer, &HygonErrorEntry);

  if ((Private->ErrorLogBuffer)->Count == TOTAL_ERROR_LOG_BUFFERS) {
    // Publish Error Log Full Ppi
    Status = (*Private->PeiServices)->InstallPpi (
                                        Private->PeiServices,
                                        &mErrorLogFullPpiList
                                        );
    ASSERT (!EFI_ERROR (Status));
  }

  return Status;
}

/*---------------------------------------------------------------------------------------*/

/**
 *
 * This function logs HGPI Errors into the Error log.
 *
 * It will put the information in a circular buffer consisting of 16 such log
 * entries. If the buffer gets full, then the next Error log entry will be written
 * over the oldest Error log entry.
 *
 * @param[in]   PeiServices
 * @param[in]   SiliconDriverId
 *
 */
EFI_STATUS
EFIAPI
HygonErrorLogIpCompletePei (
  IN       PEI_HYGON_ERROR_LOG_PPI   *This,
  IN CONST EFI_GUID                *SiliconDriverId
  )
{
  EFI_STATUS             Status = EFI_SUCCESS;
  ERROR_LOG_PPI_PRIVATE  *Private;

  Private = (ERROR_LOG_PPI_PRIVATE *)This;

  // check here if all IP drivers complete before publish Error Log Available Ppi

  // Publish Error Log Available Ppi
  Status = (*Private->PeiServices)->InstallPpi (
                                      Private->PeiServices,
                                      &mErrorLogAvailablePpiList
                                      );
  ASSERT (!EFI_ERROR (Status));

  return Status;
}

/*---------------------------------------------------------------------------------------*/

/**
 *
 * This function logs HGPI Errors into the Error log.
 *
 * It will put the information in a circular buffer consisting of 128 such log
 * entries. If the buffer gets full, then the next Error log entry will be written
 * over the oldest Error log entry.
 *
 * @param[in]   PeiServices
 * @param[out]  ErrorLogDataPtr
 *
 */
EFI_STATUS
EFIAPI
HygonAquireErrorLogPei (
  IN       PEI_HYGON_ERROR_LOG_SERVICE_PPI   *This,
  OUT      ERROR_LOG_DATA_STRUCT *ErrorLogDataPtr
  )
{
  EFI_STATUS                     Status = EFI_SUCCESS;
  ERROR_LOG_SERVICE_PPI_PRIVATE  *Private;

  Private = (ERROR_LOG_SERVICE_PPI_PRIVATE *)This;

  AquireErrorLog (Private->ErrorLogBuffer, ErrorLogDataPtr);
  // Reset Error Log buffer
  ErrorLogBufferInit (Private->ErrorLogBuffer);

  return Status;
}

/*++

Routine Description:

  Error Log Driver Entry. Initialize and publish Error Log and PPI

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
PeiHygonErrorInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  HYGON_ERROR_LOG_INFO_HOB       *ErrorLogHob;
  EFI_STATUS                     Status = EFI_SUCCESS;
  ERROR_LOG_PPI_PRIVATE          *EventlogPrivate;
  ERROR_LOG_SERVICE_PPI_PRIVATE  *EventlogServicePrivate;
  HYGON_ERROR_BUFFER             *ErrorLogBuffer;
  EFI_PEI_PPI_DESCRIPTOR         *mErrorLogPpiList;
  EFI_PEI_PPI_DESCRIPTOR         *mErrorLogServicePpiList;

  DEBUG ((EFI_D_INFO, "*****************************PEI Error Log Driver Entry*********************\n"));     // byo230914 -

  // Create Error Log GUID HOB to save Error Log buffer data
  Status = (*PeiServices)->CreateHob (
                             PeiServices,
                             EFI_HOB_TYPE_GUID_EXTENSION,
                             sizeof (HYGON_ERROR_LOG_INFO_HOB),
                             &ErrorLogHob
                             );

  ASSERT (!EFI_ERROR (Status));

  CopyMem (&ErrorLogHob->EfiHobGuidType.Name, &gErrorLogHobGuid, sizeof (EFI_GUID));
  ErrorLogBuffer = &(ErrorLogHob->HygonErrorBuffer);

  ErrorLogBufferInit (ErrorLogBuffer);

  // Publish Error Log Ppi for IP driver
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (ERROR_LOG_PPI_PRIVATE),
                             &EventlogPrivate
                             );
  ASSERT (!EFI_ERROR (Status));

  EventlogPrivate->HygonErrorLogPpi.HygonErrorLogPei = HygonErrorLogPei;
  EventlogPrivate->HygonErrorLogPpi.HygonErrorLogIpCompletePei = HygonErrorLogIpCompletePei;
  EventlogPrivate->PeiServices    = (EFI_PEI_SERVICES **)PeiServices;
  EventlogPrivate->ErrorLogBuffer = ErrorLogBuffer;

  // Allocate memory for the PPI descriptor
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             (VOID **)&mErrorLogPpiList
                             );
  ASSERT (!EFI_ERROR (Status));

  mErrorLogPpiList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  mErrorLogPpiList->Guid  = &gHygonErrorLogPpiGuid;
  mErrorLogPpiList->Ppi   = &(EventlogPrivate->HygonErrorLogPpi);

  Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             mErrorLogPpiList
                             );
  ASSERT (!EFI_ERROR (Status));

  // Publish Error Log Service Ppi for Platform BIOS
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (ERROR_LOG_SERVICE_PPI_PRIVATE),
                             &EventlogServicePrivate
                             );
  ASSERT (!EFI_ERROR (Status));

  EventlogServicePrivate->HygonErrorLogServicePpi.HygonAquireErrorLogPei = HygonAquireErrorLogPei;
  EventlogServicePrivate->PeiServices    = (EFI_PEI_SERVICES **)PeiServices;
  EventlogServicePrivate->ErrorLogBuffer = ErrorLogBuffer;

  // Allocate memory for the PPI descriptor
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             (VOID **)&mErrorLogServicePpiList
                             );
  ASSERT (!EFI_ERROR (Status));

  mErrorLogServicePpiList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  mErrorLogServicePpiList->Guid  = &gHygonErrorLogServicePpiGuid;
  mErrorLogServicePpiList->Ppi   = &(EventlogServicePrivate->HygonErrorLogServicePpi);

  Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             mErrorLogServicePpiList
                             );
  ASSERT (!EFI_ERROR (Status));

  // Publish Error Log Ready Ppi
  Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             &mErrorLogReadyPpiList
                             );
  ASSERT (!EFI_ERROR (Status));

  DEBUG ((EFI_D_INFO, "*****************************PEI Error Driver Exit*********************\n"));          // byo230914 -
  return (Status);
}
