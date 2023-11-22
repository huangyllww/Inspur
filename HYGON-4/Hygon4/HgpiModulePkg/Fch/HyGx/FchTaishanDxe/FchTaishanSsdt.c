/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project   FCH DXE Driver
 *
 */
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
#include "FchTaishanSsdt.h"
#include "Filecode.h"

extern EFI_GUID  gFchSsdtInstalledProtocolGuid;
EFI_GUID         gHygonFchTaishanSsdtGuid = HYGON_FCH_TAISHAN_SSDT_GUID;

/*********************************************************************************
 * Name: FchTaishanSsdtInitEntry
 *
 * Description
 *   Entry point of the HYGON FCH FchTaishanSsdtInit support driver
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
FchTaishanSsdtInitHyGxEntry (
  IN      EFI_HANDLE                      ImageHandle,
  IN      EFI_SYSTEM_TABLE                *SystemTable
  )
{
  EFI_STATUS  Status;
  // FCH_INIT_PROTOCOL                         *pHygonFchInitProtocol;
  UINTN                        Index;
  INTN                         Instance;
  UINTN                        Size;
  UINTN                        NumberOfHandles;
  UINTN                        TableHandle;
  UINTN                        TableSize;
  UINT32                       FvStatus;
  EFI_HANDLE                   Handle;
  EFI_HANDLE                   *HandleBuffer;
  EFI_FV_FILETYPE              FileType;
  EFI_FV_FILE_ATTRIBUTES       Attributes;
  EFI_ACPI_COMMON_HEADER       *CurrentTable;
  EFI_ACPI_DESCRIPTION_HEADER  *FchTaishanAcpiBlockPtr = NULL;

  EFI_ACPI_TABLE_PROTOCOL        *AcpiTableProtocol;
  EFI_ACPI_SDT_PROTOCOL          *AcpiSdtProtocol;
  EFI_FIRMWARE_VOLUME2_PROTOCOL  *FwVol = NULL;

  Status = gBS->LocateProtocol (
                  &gEfiAcpiTableProtocolGuid,
                  NULL,
                  &AcpiTableProtocol
                  );

  Status = gBS->LocateProtocol (
                  &gEfiAcpiSdtProtocolGuid,
                  NULL,
                  &AcpiSdtProtocol
                  );

  FvStatus = 0;
  //
  // Locate protocol.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **)&FwVol
                    );
    ASSERT (!EFI_ERROR (Status));

    Size     = 0;
    FvStatus = 0;
    Status   = FwVol->ReadFile (
                        FwVol,
                        &gHygonFchTaishanSsdtGuid,
                        NULL,
                        &Size,
                        &FileType,
                        &Attributes,
                        &FvStatus
                        );

    if (Status == EFI_SUCCESS) {
      break;
    }
  }

  Instance     = 0;
  CurrentTable = NULL;
  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gHygonFchTaishanSsdtGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      &CurrentTable,
                      &Size,
                      &FvStatus
                      );

    if (!EFI_ERROR (Status)) {
      if (((EFI_ACPI_DESCRIPTION_HEADER *)CurrentTable)->OemTableId == SIGNATURE_64 ('F', 'C', 'H', 'H', 'G', 0, 0, 0)) {
        // Change ACPI Table id to Hygon
        FchTaishanAcpiBlockPtr = (EFI_ACPI_DESCRIPTION_HEADER *)CurrentTable;
        TableHandle = 0;
        TableSize   = FchTaishanAcpiBlockPtr->Length;

        //
        // Install ACPI table
        //
        Status = AcpiTableProtocol->InstallAcpiTable (
                                      AcpiTableProtocol,
                                      FchTaishanAcpiBlockPtr,
                                      TableSize,
                                      &TableHandle
                                      );

        //
        // Install gFchInitDonePolicyProtocolGuid to signal Platform
        //
        Handle = ImageHandle;
        Status = gBS->InstallProtocolInterface (
                        &Handle,
                        &gFchSsdtInstalledProtocolGuid,
                        EFI_NATIVE_INTERFACE,
                        NULL
                        );

        gBS->FreePool (CurrentTable);
      }

      Instance++;
      CurrentTable = NULL;
    }
  }

  return Status;
}
