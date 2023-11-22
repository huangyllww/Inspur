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

#include <Filecode.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/HygonErrorLogLib.h>
#include <Protocol/HygonErrorLogProtocol.h>
#include <Protocol/HygonErrorLogServiceProtocol.h>
#include "HygonErrorLogDxe.h"

#define FILECODE  ERRORLOG_HYGONERRORLOGDXE_HYGONERRORLOGDXE_FILECODE

DXE_HYGON_ERROR_LOG_SERVICES_PROTOCOL  mDxeHygonErrorLogServicesProtocol = {
  HYGON_ERROR_LOG_SERVICE_PROTOCOL_REVISION,
  HygonAquireErrorLogDxe,
};

DXE_HYGON_ERROR_LOG_PROTOCOL  mDxeHygonErrorLogProtocol = {
  HYGON_ERROR_LOG_PROTOCOL_REVISION,
  HygonErrorLogDxe,
  HygonErrorLogIpCompleteDxe,
};

extern  EFI_BOOT_SERVICES  *gBS;

//
// Driver Global Data
//
STATIC HYGON_ERROR_BUFFER  *ErrorLogBuffer;

/*---------------------------------------------------------------------------------------*/

/**
 *
 * This function logs HGPI errors into the Error log.
 *
 * It will put the information in a circular buffer consisting of 128 such log
 * entries. If the buffer gets full, then the next Error log entry will be written
 * over the oldest Error log entry.
 *
 * @param[in]   This         Error log service instance.
 * @param[in]   ErrorClass   The severity of the Erorr, its associated HGPI_STATUS.
 * @param[in]   ErrorInfo    Uniquely identifies the error.
 * @param[in]   DataParam1   Error specific additional data
 * @param[in]   DataParam2   Error specific additional data
 * @param[in]   DataParam3   Error specific additional data
 * @param[in]   DataParam4   Error specific additional data
 *
 */
EFI_STATUS
EFIAPI
HygonErrorLogDxe (
  IN  DXE_HYGON_ERROR_LOG_PROTOCOL   *This,
  IN  HYGON_STATUS ErrorClass,
  IN  UINT32 ErrorInfo,
  IN  UINT32 DataParam1,
  IN  UINT32 DataParam2,
  IN  UINT32 DataParam3,
  IN  UINT32 DataParam4
  )
{
  EFI_STATUS         Status = EFI_SUCCESS;
  EFI_HANDLE         Handle;
  HYGON_ERROR_ENTRY  HygonErrorEntry;

  Handle = NULL;

  HygonErrorEntry.ErrorClass = ErrorClass;
  HygonErrorEntry.ErrorInfo  = ErrorInfo;
  HygonErrorEntry.DataParam1 = DataParam1;
  HygonErrorEntry.DataParam2 = DataParam2;
  HygonErrorEntry.DataParam3 = DataParam3;
  HygonErrorEntry.DataParam4 = DataParam4;

  AddErrorLog (ErrorLogBuffer, &HygonErrorEntry);

  if (ErrorLogBuffer->Count == TOTAL_ERROR_LOG_BUFFERS) {
    // Publish Error Log Full Protocol
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gHygonErrorLogFullProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
  }

  return Status;
}

EFI_STATUS
EFIAPI
HygonErrorLogIpCompleteDxe (
  IN       DXE_HYGON_ERROR_LOG_PROTOCOL   *This,
  IN CONST EFI_GUID                     *SiliconDriverId
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_HANDLE  Handle;

  Handle = NULL;

  // check here if all IP drivers complete before publish Error Log Available Protocol

  // Publish Error Log Available Protocol
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonErrorLogAvailableProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );

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
HygonAquireErrorLogDxe (
  IN       DXE_HYGON_ERROR_LOG_SERVICES_PROTOCOL   *This,
  OUT      ERROR_LOG_DATA_STRUCT *ErrorLogDataPtr
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  AquireErrorLog (ErrorLogBuffer, ErrorLogDataPtr);

  return Status;
}

/*********************************************************************************
 * Name: HygonErrorLogDxeInit
 *
 * Description
 *   Entry point of the HYGON Error Log DXE driver
 *   Perform the configuration init, resource reservation, early post init
 *   and install all the supported protocol
 *
 * Input
 *   ImageHandle : EFI Image Handle for the DXE driver
 *   SystemTable : pointer to the EFI system table
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
HygonErrorLogDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  EFI_HANDLE                Handle;
  HYGON_ERROR_LOG_INFO_HOB  *ErrorLogHob;

  DEBUG ((EFI_D_INFO, "*****************************DXE Error Log Driver Entry*********************\n"));       // byo230914 -

  //
  // Initialize the configuration structure and private data area
  //

  // Get Error Log buffer from HOB
  ErrorLogHob    = GetFirstGuidHob (&gErrorLogHobGuid);
  ErrorLogBuffer = &(ErrorLogHob->HygonErrorBuffer);

  Handle = ImageHandle;

  // Publish Error Log service Protocol For IP driver
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonErrorLogProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mDxeHygonErrorLogProtocol
                  );

  // Publish Error Log service Protocol for platform BIOS
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonErrorLogServiceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mDxeHygonErrorLogServicesProtocol
                  );

  // Publish Error Log Ready Protocol
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonErrorLogReadyProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );

  DEBUG ((EFI_D_INFO, "*****************************DXE Error Log Driver Exit*********************\n"));        // byo230914 -

  return (Status);
}
