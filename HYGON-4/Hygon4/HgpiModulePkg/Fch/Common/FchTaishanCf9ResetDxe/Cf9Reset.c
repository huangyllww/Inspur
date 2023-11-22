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

#include "Cf9Reset.h"
// byo230831 + >>
#include <Library/UefiLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/ResetNotification.h>
#include <Protocol/PlatformSpecificResetFilter.h>
#include <Protocol/PlatformSpecificResetHandler.h>
#include <Library/ByoSharedSmmData.h>
// byo230831 + <<

#define FILECODE  UEFI_DXE_CF9RESET_CF9RESET_FILECODE


// byo230831 + >>
#define MAX_RESET_NOTIFY_DEPTH 10

typedef struct {
  UINT32                   Signature;
  LIST_ENTRY               Link;
  EFI_RESET_SYSTEM         ResetNotify;
} RESET_NOTIFY_ENTRY;
#define RESET_NOTIFY_ENTRY_SIGNATURE    SIGNATURE_32('r', 's', 't', 'n')
#define RESET_NOTIFY_ENTRY_FROM_LINK(a) CR (a, RESET_NOTIFY_ENTRY, Link, RESET_NOTIFY_ENTRY_SIGNATURE)

typedef struct {
  UINT32                          Signature;
  EFI_RESET_NOTIFICATION_PROTOCOL ResetNotification;
  LIST_ENTRY                      ResetNotifies;
} RESET_NOTIFICATION_INSTANCE;
#define RESET_NOTIFICATION_INSTANCE_SIGNATURE    SIGNATURE_32('r', 's', 't', 'i')
#define RESET_NOTIFICATION_INSTANCE_FROM_THIS(a) \
  CR (a, RESET_NOTIFICATION_INSTANCE, ResetNotification, RESET_NOTIFICATION_INSTANCE_SIGNATURE)


EFI_STATUS
EFIAPI
RegisterResetNotify (
  IN EFI_RESET_NOTIFICATION_PROTOCOL *This,
  IN EFI_RESET_SYSTEM                ResetFunction
  );

EFI_STATUS
EFIAPI
UnregisterResetNotify (
  IN EFI_RESET_NOTIFICATION_PROTOCOL *This,
  IN EFI_RESET_SYSTEM                ResetFunction
  );

RESET_NOTIFICATION_INSTANCE mResetNotification = {
  RESET_NOTIFICATION_INSTANCE_SIGNATURE,
  {
    RegisterResetNotify,
    UnregisterResetNotify
  },
  INITIALIZE_LIST_HEAD_VARIABLE (mResetNotification.ResetNotifies)
};

RESET_NOTIFICATION_INSTANCE mPlatformSpecificResetFilter = {
  RESET_NOTIFICATION_INSTANCE_SIGNATURE,
  {
    RegisterResetNotify,
    UnregisterResetNotify
  },
  INITIALIZE_LIST_HEAD_VARIABLE (mPlatformSpecificResetFilter.ResetNotifies)
};

RESET_NOTIFICATION_INSTANCE mPlatformSpecificResetHandler = {
  RESET_NOTIFICATION_INSTANCE_SIGNATURE,
  {
    RegisterResetNotify,
    UnregisterResetNotify
  },
  INITIALIZE_LIST_HEAD_VARIABLE (mPlatformSpecificResetHandler.ResetNotifies)
};

GLOBAL_REMOVE_IF_UNREFERENCED CHAR16 *mResetTypeStr[] = {
  L"Cold", L"Warm", L"Shutdown", L"PlatformSpecific"
};

UINTN  mResetNotifyDepth = 0;
// byo230831 + <<

//
// The handle onto which the Reset Architectural Protocol is installed
//
EFI_HANDLE  mResetHandle = NULL;

// byo230831 + >>
VOID
Cf9SbSleepTrapControl (
    VOID
  )
{
  MmioAndThenOr32(ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0, (UINT32)~(BIT2 + BIT3), BIT2);  // 01: SMI
  MmioAnd8(ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGBE, (UINT8)~BIT5);
  MmioAndThenOr8(ACPI_MMIO_BASE + PMIO_BASE + 0xB, (UINT8)~(BIT0 + BIT1), BIT1);
}
// byo230831 + <<

UINT16
ReadPmio16 (
  IN       UINT8        Index
  )
{
  UINT8  bTemp;

  IoWrite8 (FCH_IOMAP_REGCD6, Index);
  bTemp = IoRead8 (FCH_IOMAP_REGCD7);
  IoWrite8 (FCH_IOMAP_REGCD6, Index + 1);
  return (UINT16)((IoRead8 (FCH_IOMAP_REGCD7) << 8) + bTemp);
}

VOID
EFIAPI
Cf9ResetSystem (
  IN EFI_RESET_TYPE   ResetType,
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN CHAR16           *ResetData OPTIONAL
  )

/*++

Routine Description:

  Reset the system.

Arguments:

    ResetType - warm or cold
    ResetStatus - possible cause of reset
    DataSize - Size of ResetData in bytes
    ResetData - Optional Unicode string
    For details, see efiapi.h

Returns:
  Does not return if the reset takes place.
  EFI_INVALID_PARAMETER   If ResetType is invalid.

--*/
{
  UINT8   InitialData;
  UINT8   OutputData;
  UINT8   PwrRsrCfg;
  UINT16  AcpiGpeBase;
  UINT16  AcpiPm1StsBase;
  UINT16  AcpiPm1CntBase;
  UINT32  Gpe0Enable;
  UINT16  PmCntl;
  UINT16  PwrSts;
  LIST_ENTRY          *Link;       // byo230831 +
  RESET_NOTIFY_ENTRY  *Entry;      // byo230831 +

  DEBUG ((DEBUG_INFO, "[FchCf9Reset] ResetSystem invoked:  ResetType = %d\n", ResetType));

// byo230831 + >>
  if(ResetType == EfiResetWarm && gByoSharedSmmData->ForceColdReset){
    DEBUG ((DEBUG_INFO, "Force ColdReset\n"));
    ResetType = EfiResetCold;
  }

  if (mResetNotifyDepth == 0) {
    REPORT_STATUS_CODE (EFI_PROGRESS_CODE, (EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_RS_PC_RESET_SYSTEM));
  }

  mResetNotifyDepth++;

  if(ResetType < ARRAY_SIZE (mResetTypeStr)){
    DEBUG ((
      DEBUG_INFO, "DXE ResetSystem2: ResetType %s, Call Depth = %d.\n",
      mResetTypeStr[ResetType], mResetNotifyDepth
      ));
  }

  if (mResetNotifyDepth <= MAX_RESET_NOTIFY_DEPTH) {
    if (!EfiAtRuntime ()) {
      //
      // Call reset notification functions registered through the
      // EDKII_PLATFORM_SPECIFIC_RESET_FILTER_PROTOCOL.
      //
      for ( Link = GetFirstNode (&mPlatformSpecificResetFilter.ResetNotifies)
          ; !IsNull (&mPlatformSpecificResetFilter.ResetNotifies, Link)
          ; Link = GetNextNode (&mPlatformSpecificResetFilter.ResetNotifies, Link)
          ) {
        Entry = RESET_NOTIFY_ENTRY_FROM_LINK (Link);
        Entry->ResetNotify (ResetType, ResetStatus, DataSize, ResetData);
      }
      //
      // Call reset notification functions registered through the
      // EFI_RESET_NOTIFICATION_PROTOCOL.
      //
      for ( Link = GetFirstNode (&mResetNotification.ResetNotifies)
          ; !IsNull (&mResetNotification.ResetNotifies, Link)
          ; Link = GetNextNode (&mResetNotification.ResetNotifies, Link)
          ) {
        Entry = RESET_NOTIFY_ENTRY_FROM_LINK (Link);
        Entry->ResetNotify (ResetType, ResetStatus, DataSize, ResetData);
      }
      //
      // call reset notification functions registered through the
      // EDKII_PLATFORM_SPECIFIC_RESET_HANDLER_PROTOCOL.
      //
      for ( Link = GetFirstNode (&mPlatformSpecificResetHandler.ResetNotifies)
          ; !IsNull (&mPlatformSpecificResetHandler.ResetNotifies, Link)
          ; Link = GetNextNode (&mPlatformSpecificResetHandler.ResetNotifies, Link)
          ) {
        Entry = RESET_NOTIFY_ENTRY_FROM_LINK (Link);
        Entry->ResetNotify (ResetType, ResetStatus, DataSize, ResetData);
      }
    }
  } else {
    ASSERT (ResetType < ARRAY_SIZE (mResetTypeStr));
    DEBUG ((DEBUG_ERROR, "DXE ResetSystem2: Maximum reset call depth is met. Use the current reset type: %s!\n", mResetTypeStr[ResetType]));
  }
// byo230831 + <<

  switch (ResetType) {
    //
    // For update resets, the reset data is a null-terminated string followed
    // by a VOID * to the capsule descriptors. Get the pointer and set the
    // capsule variable before we do a warm reset. Per the EFI 1.10 spec, the
    // reset data is only valid if ResetStatus != EFI_SUCCESS.
    //
    case EfiResetWarm:
      InitialData = HARDSTARTSTATE;
      OutputData  = HARDRESET;
      IoWrite8 (FCH_IOMAP_REGCD6, 0x10);
      PwrRsrCfg = IoRead8 (FCH_IOMAP_REGCD7);
      PwrRsrCfg = PwrRsrCfg & 0xFD; // clear ToggleAllPwrGoodOnCf9
      IoWrite8 (FCH_IOMAP_REGCD7, PwrRsrCfg);
      break;

    case EfiResetCold:
      InitialData = HARDSTARTSTATE;
      OutputData  = HARDRESET;
      IoWrite8 (FCH_IOMAP_REGCD6, 0x10);
      PwrRsrCfg = IoRead8 (FCH_IOMAP_REGCD7);
      PwrRsrCfg = PwrRsrCfg | BIT1; // set ToggleAllPwrGoodOnCf9
      IoWrite8 (FCH_IOMAP_REGCD7, PwrRsrCfg);
      break;

    case EfiResetShutdown:
      Cf9SbSleepTrapControl();                        // byo230831 +
      // Disable all GPE0 Event
      AcpiGpeBase = ReadPmio16 (FCH_PMIOA_REG68);
      Gpe0Enable  = 0;
      IoWrite32 (AcpiGpeBase, Gpe0Enable);

      // Clear Power Button status.
      AcpiPm1StsBase = ReadPmio16 (FCH_PMIOA_REG60);
      PwrSts = BIT8;
      IoWrite16 (AcpiPm1StsBase, PwrSts);

      // Transform system into S5 sleep state
      AcpiPm1CntBase = ReadPmio16 (FCH_PMIOA_REG62);
      PmCntl = IoRead16 (AcpiPm1CntBase);
      PmCntl = (PmCntl & ~SLP_TYPE) | SUS_S5 | SLP_EN;
      IoWrite16 (AcpiPm1CntBase, PmCntl);
      return;

    default:
      return;
  }

  IoWrite8 (FCH_IOMAP_REGCF9, InitialData);
  IoWrite8 (FCH_IOMAP_REGCF9, OutputData);

  //
  // Given we should have reset getting here would be bad
  //
  ASSERT (FALSE);
}



// byo230831 + >>
EFI_STATUS
EFIAPI
RegisterResetNotify (
  IN EFI_RESET_NOTIFICATION_PROTOCOL *This,
  IN EFI_RESET_SYSTEM                ResetFunction
  )
{
  RESET_NOTIFICATION_INSTANCE        *Instance;
  LIST_ENTRY                         *Link;
  RESET_NOTIFY_ENTRY                 *Entry;

  if (ResetFunction == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = RESET_NOTIFICATION_INSTANCE_FROM_THIS (This);

  for ( Link = GetFirstNode (&Instance->ResetNotifies)
      ; !IsNull (&Instance->ResetNotifies, Link)
      ; Link = GetNextNode (&Instance->ResetNotifies, Link)
      ) {
    Entry = RESET_NOTIFY_ENTRY_FROM_LINK (Link);
    if (Entry->ResetNotify == ResetFunction) {
      return EFI_ALREADY_STARTED;
    }
  }

  ASSERT (IsNull (&Instance->ResetNotifies, Link));
  Entry = AllocatePool (sizeof (*Entry));
  if (Entry == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Entry->Signature   = RESET_NOTIFY_ENTRY_SIGNATURE;
  Entry->ResetNotify = ResetFunction;
  InsertTailList (&Instance->ResetNotifies, &Entry->Link);
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
UnregisterResetNotify (
  IN EFI_RESET_NOTIFICATION_PROTOCOL *This,
  IN EFI_RESET_SYSTEM                ResetFunction
  )
{
  RESET_NOTIFICATION_INSTANCE        *Instance;
  LIST_ENTRY                         *Link;
  RESET_NOTIFY_ENTRY                 *Entry;

  if (ResetFunction == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = RESET_NOTIFICATION_INSTANCE_FROM_THIS (This);

  for ( Link = GetFirstNode (&Instance->ResetNotifies)
      ; !IsNull (&Instance->ResetNotifies, Link)
      ; Link = GetNextNode (&Instance->ResetNotifies, Link)
      ) {
    Entry = RESET_NOTIFY_ENTRY_FROM_LINK (Link);
    if (Entry->ResetNotify == ResetFunction) {
      RemoveEntryList (&Entry->Link);
      FreePool (Entry);
      return EFI_SUCCESS;
    }
  }

  return EFI_INVALID_PARAMETER;
}
// byo230831 + <<

/********************************************************************************
 * Name: InitializeCf9Reset
 *
 * Description
 *   Cf9ResetDxe Entrypoint
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
InitializeCf9Reset (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )

/*++

Routine Description:

  Initialize the state information for the Timer Architectural Protocol

Arguments:

  ImageHandle of the loaded driver
  Pointer to the System Table

Returns:

  Status

  EFI_SUCCESS           - thread can be successfully created
  EFI_OUT_OF_RESOURCES  - cannot allocate protocol data structure
  EFI_DEVICE_ERROR      - cannot create the timer service

--*/
{
  EFI_STATUS  Status;

  // Done by library constructor- EfiInitializeRuntimeDriverLib (ImageHandle, SystemTable, NULL);

  //
  // Make sure the Reset Architectural Protocol is not already installed in the system
  //
  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gEfiResetArchProtocolGuid);

  //
  // Hook the runtime service table
  //
  SystemTable->RuntimeServices->ResetSystem = Cf9ResetSystem;

  //
  // Now install the Reset RT AP on a new handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mResetHandle,
                  &gEfiResetArchProtocolGuid,         NULL,
                  &gEfiResetNotificationProtocolGuid, &mResetNotification.ResetNotification,                             // byo230831 +
                  &gEdkiiPlatformSpecificResetFilterProtocolGuid, &mPlatformSpecificResetFilter.ResetNotification,       // byo230831 +
                  &gEdkiiPlatformSpecificResetHandlerProtocolGuid, &mPlatformSpecificResetHandler.ResetNotification,     // byo230831 +
                  NULL
                  );
  ASSERT (!EFI_ERROR (Status));

  return Status;
}
