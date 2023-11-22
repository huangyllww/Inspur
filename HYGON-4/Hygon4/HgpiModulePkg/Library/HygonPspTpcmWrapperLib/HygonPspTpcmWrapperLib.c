/* $NoKeywords:$ */

/**
 * @file
 *
 * Service procedure to access heap.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: LIB
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/DevicePathLib.h>
#include <Library/SecurityManagementLib.h>
#include <Protocol/HygonPspTpcmProtocol.h>

#define STAGE_OS_LOADER  1000
#define CTRL_CODE_MASK   0x1

// byo230831 + >>
BOOLEAN
IsFvImage (
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *File
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        DeviceHandle;
  EFI_DEVICE_PATH_PROTOCOL          *TempDevicePath;

  if (File == NULL) {
    return FALSE;
  }

  DeviceHandle   = NULL;
  TempDevicePath = (EFI_DEVICE_PATH_PROTOCOL*)File;
  Status = gBS->LocateDevicePath (
                  &gEfiFirmwareVolume2ProtocolGuid,
                  &TempDevicePath,
                  &DeviceHandle
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->OpenProtocol (
                    DeviceHandle,
                    &gEfiFirmwareVolume2ProtocolGuid,
                    NULL,
                    NULL,
                    NULL,
                    EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                    );
    if (!EFI_ERROR (Status)) {
      return TRUE;
    }
  }

  return FALSE;
}
// byo230831 + <<

EFI_STATUS EFIAPI
HygonPspTpcmHandler (
  IN  UINT32                         AuthenticationStatus,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL *FilePath,
  IN  VOID                           *FileBuffer,
  IN  UINTN                          FileSize,
  IN  BOOLEAN                        BootPolicy
  )
{
  PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL  *ImageVerify;
  UINT32                              MeasureResult;
  UINT32                              ControlResult;
  IMAGE_ADDR_RANGE                    Range;
  UINT32                              PathSize;
  CHAR16                              *PathName;
  EFI_STATUS                          Status;

  if (BootPolicy == FALSE) {
    return EFI_SUCCESS;
  }

// byo230831 + >>  
  if(IsFvImage(FilePath)) {
    return EFI_SUCCESS;
  }
// byo230831 + <<

  Status = gBS->LocateProtocol (&gPspC2PmboxImageVerifyProtocolGuid, NULL, &ImageVerify);

  if (!EFI_ERROR (Status) && ImageVerify->IsTpcmEnabled (ImageVerify)) {
//  DEBUG ((DEBUG_INFO, "HygonPspTpcmHandler Protocol Status=%r\n", Status));      // byo230831 -
    Range.Start = (UINT64)GetFileBufferByFilePath (
                            BootPolicy,
                            FilePath,
                            &Range.Length,
                            &MeasureResult
                            );

    PathName = ConvertDevicePathToText (FilePath, TRUE, FALSE);
    PathSize = 0;
    if (PathName) {
      PathSize = (UINT32)StrSize (PathName);
    }

    Status = ImageVerify->TpcmVerifyRaw (ImageVerify, STAGE_OS_LOADER, PathName, PathSize, 1, &Range, &MeasureResult, &ControlResult);
    DEBUG((DEBUG_INFO, "TpcmVerifyRaw:%r %s MR:%X CR:%X\n", Status, PathName==NULL?L"":PathName, MeasureResult, ControlResult));   // byo230831 +
    if (PathName != NULL) {
      gBS->FreePool (PathName);
    }

    if (EFI_ERROR (Status) || (ControlResult & CTRL_CODE_MASK) != 0) {
      DEBUG ((DEBUG_INFO, "ERROR: EFI Boot device failed verification!\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  return EFI_SUCCESS;
}

/**
  The constructor function used to register Security2Handler protocol

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor successfully gets Heap List Header.
  @retval Other value   The constructor can't get Heap List Header.

 **/
EFI_STATUS
EFIAPI
HygonPspTpcmWrapperLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "HygonPspTpcmWrapperLibConstructor\n"));

  Status = RegisterSecurity2Handler (
             HygonPspTpcmHandler,
             EFI_AUTH_OPERATION_VERIFY_IMAGE | EFI_AUTH_OPERATION_DEFER_IMAGE_LOAD | EFI_AUTH_OPERATION_MEASURE_IMAGE | EFI_AUTH_OPERATION_IMAGE_REQUIRED
             );
  ASSERT (!EFI_ERROR (Status));

  return EFI_SUCCESS;
}
