/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Hudson-2 SMI Dispatcher Driver Example
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  UEFI
 *
 */
/******************************************************************************
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
****************************************************************************/

#include "FchSmmIoTrapDispatcher.h"
#include "Filecode.h"
#define FILECODE  UEFI_SMM_FCHSMMDISPATCHER_FCHSMMIOTRAPDISPATCHER_FCHSMMIOTRAPDISPATCHER_FILECODE

EFI_STATUS
EFIAPI
FchSmmIoTrapDispatch2Register (
  IN       CONST FCH_SMM_IO_TRAP_DISPATCH2_PROTOCOL  *This,
  IN       FCH_SMM_IO_TRAP_HANDLER_ENTRY_POINT2      DispatchFunction,
  IN OUT   FCH_SMM_IO_TRAP_REGISTER_CONTEXT          *RegisterContext,
  OUT      EFI_HANDLE                                *DispatchHandle
  );

EFI_STATUS
EFIAPI
FchSmmIoTrapDispatch2UnRegister (
  IN       CONST FCH_SMM_IO_TRAP_DISPATCH2_PROTOCOL *This,
  IN       EFI_HANDLE                               DispatchHandle
  );

FCH_SMM_IO_TRAP_DISPATCH2_PROTOCOL  gFchSmmIoTrapDispatch2Protocol = {
  FchSmmIoTrapDispatch2Register,
  FchSmmIoTrapDispatch2UnRegister
};

FCH_IO_TRAP_ENTRY  mFchIoTrapList[] = {
  { BIT20, 8, 0xC8, 0xF0, 0, 0, 0 },
};

/*----------------------------------------------------------------------------------------*/

/**
 * FCH SMM IO TRAP dispatcher handler
 *
 *
 * @param[in]       SmmImageHandle        Image Handle
 * @param[in, out]   OPTIONAL CommunicationBuffer   Communication Buffer (see PI 1.1 for more details)
 * @param[in, out]   OPTIONAL SourceSize            Buffer size (see PI 1.1 for more details)

 * @retval          EFI_SUCCESS           SMI handled by dispatcher
 * @retval          EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
FchSmmIoTrapDispatchHandler (
  IN       EFI_HANDLE   SmmImageHandle,
  IN OUT   VOID         *CommunicationBuffer OPTIONAL,
  IN OUT   UINTN        *SourceSize OPTIONAL
  )
{
  EFI_STATUS  Status;
  UINTN       Index;
  UINT32      SmiIoTrapStatusBitmap;

  Status = EFI_UNSUPPORTED;

  SmiIoTrapStatusBitmap  = ACPIMMIO32 (ACPI_MMIO_BASE + 0x290);
  SmiIoTrapStatusBitmap &= BIT20;
  if (SmiIoTrapStatusBitmap != 0) {
    for (Index = 0; Index < (sizeof (mFchIoTrapList) / sizeof (FCH_IO_TRAP_ENTRY)); Index++) {
      if ((mFchIoTrapList[Index].StatusMask & SmiIoTrapStatusBitmap) != 0) {
        ACPIMMIO32 (ACPI_MMIO_BASE + 0x290) = mFchIoTrapList[Index].StatusMask;
        if (mFchIoTrapList[Index].DispatchFunction) {
          Status = mFchIoTrapList[Index].DispatchFunction (
                                           mFchIoTrapList[Index].DispatchHandle,
                                           &mFchIoTrapList[Index].Context,
                                           NULL,
                                           NULL
                                           );
          DEBUG ((
            DEBUG_INFO,
            "[FchSmmDispatcher] IoTrap SMM handler dispatched: Address = 0x%04lx, return - %r\n", \
            mFchIoTrapList[Index].Context.Address,
            Status
            ));
        }

        break;
      }
    }
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Register IO TRAP child handler
 *
 *
 * @param[in]       This                  Pointer to protocol
 * @param[in]       DispatchFunction      Child dispcther function
 * @param[in, out]   RegisterContext            Register contecxt (see PI 1.1 for more details)
 * @param[out]      DispatchHandle        Handle (see PI 1.1 for more details)
 *
 * @retval          EFI_SUCCESS           SMI handled by dispatcher
 * @retval          EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
FchSmmIoTrapDispatch2Register (
  IN       CONST FCH_SMM_IO_TRAP_DISPATCH2_PROTOCOL  *This,
  IN       FCH_SMM_IO_TRAP_HANDLER_ENTRY_POINT2      DispatchFunction,
  IN OUT   FCH_SMM_IO_TRAP_REGISTER_CONTEXT          *RegisterContext,
  OUT      EFI_HANDLE                                *DispatchHandle
  )
{
  EFI_STATUS  Status;
  UINTN       Index;

  Status = EFI_OUT_OF_RESOURCES;

  ASSERT ((DispatchFunction != NULL) && (DispatchHandle != NULL));

  if (DispatchFunction == NULL || RegisterContext == NULL || DispatchHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < (sizeof (mFchIoTrapList) / sizeof (FCH_IO_TRAP_ENTRY)); Index++) {
    if (mFchIoTrapList[Index].DispatchFunction == NULL) {
      CopyMem (
        &mFchIoTrapList[Index].Context,
        RegisterContext,
        sizeof (FCH_SMM_IO_TRAP_REGISTER_CONTEXT)
        );
      mFchIoTrapList[Index].DispatchFunction = DispatchFunction;
      *DispatchHandle = &mFchIoTrapList[Index];
      // Clear Status
      ACPIMMIO32 (ACPI_MMIO_BASE + 0x290) = mFchIoTrapList[Index].StatusMask;
      // Set base address
      ACPIMMIO32 (ACPI_MMIO_BASE + 0x200 + mFchIoTrapList[Index].BaseAddressRegister) = RegisterContext->Address;
      // Set Read Write Attribute
      ACPIMMIO32 (ACPI_MMIO_BASE + 0x200 + mFchIoTrapList[Index].RwAddressRegister) &= 0x2;
      if (RegisterContext->Type == WriteTrap) {
        ACPIMMIO32 (ACPI_MMIO_BASE + 0x200 + mFchIoTrapList[Index].RwAddressRegister) |= 0x2;
      }

      // Enable SMI
      ACPIMMIO32 (ACPI_MMIO_BASE + 0x2C4) |= (1 << mFchIoTrapList[Index].EnableBitOffset);

      DEBUG ((
        DEBUG_INFO,
        "[FchSmmDispatcher] Registering IoTrap SMM handler: Address = 0x%04lx, Type = 0x%x\n", \
        RegisterContext->Address,
        RegisterContext->Type
        ));
      Status = EFI_SUCCESS;
      break;
    }
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Unregister IO TRAP child handler
 *
 *
 * @param[in]       This                  Pointer to protocol
 * @param[in]       DispatchHandle
 *
 * @retval          EFI_SUCCESS           SMI handled by dispatcher
 * @retval          EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
FchSmmIoTrapDispatch2UnRegister (
  IN       CONST FCH_SMM_IO_TRAP_DISPATCH2_PROTOCOL *This,
  IN       EFI_HANDLE                               DispatchHandle
  )
{
  EFI_STATUS  Status;
  UINTN       Index;

  Status = EFI_NOT_FOUND;
  for (Index = 0; Index < (sizeof (mFchIoTrapList) / sizeof (FCH_IO_TRAP_ENTRY)); Index++) {
    if ( &mFchIoTrapList[Index] == DispatchHandle) {
      mFchIoTrapList[Index].DispatchFunction = NULL;
      // Clear Status ACTION. Is it safe? wll EOS be taken care?
      ACPIMMIO32 (ACPI_MMIO_BASE + 0x290) = mFchIoTrapList[Index].StatusMask;
      // Disable SMI
      ACPIMMIO32 (ACPI_MMIO_BASE + 0x2C4) &= (~(3 << mFchIoTrapList[Index].EnableBitOffset));
      Status = EFI_SUCCESS;
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
EfiSmmIoTrapDispatch2Register (
  IN       CONST EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL *This,
  IN       EFI_SMM_HANDLER_ENTRY_POINT2             DispatchFunction,
  IN OUT   EFI_SMM_IO_TRAP_REGISTER_CONTEXT         *RegisterContext,
  OUT      EFI_HANDLE                               *DispatchHandle
  )
{
  FCH_SMM_IO_TRAP_REGISTER_CONTEXT  FchRegisterContext;

  FchRegisterContext.Address = RegisterContext->Address;
  FchRegisterContext.Length  = RegisterContext->Length;
  FchRegisterContext.Type    = RegisterContext->Type;
  return FchSmmIoTrapDispatch2Register (
           &gFchSmmIoTrapDispatch2Protocol,
           DispatchFunction,
           &FchRegisterContext,
           DispatchHandle
           );
}

EFI_STATUS
EFIAPI
EfiSmmIoTrapDispatch2UnRegister (
  IN       CONST EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL *This,
  IN       EFI_HANDLE                               DispatchHandle
  )
{
  return FchSmmIoTrapDispatch2UnRegister (
           &gFchSmmIoTrapDispatch2Protocol,
           DispatchHandle
           );
}

EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL  gEfiSmmIoTrapDispatch2Protocol = {
  EfiSmmIoTrapDispatch2Register,
  EfiSmmIoTrapDispatch2UnRegister
};
