/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Fch APU RAS SMI Dispatcher Driver
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

#include "FchSmmApuRasDispatcher.h"
#include "Filecode.h"
#include <Library/BaseFabricTopologyLib.h>
#include <Protocol/FabricTopologyServices.h>
#include <HygonRas.h>
#include <GnbRegisters.h>

#define FILECODE  UEFI_SMM_FCHSMMDISPATCHER_FCHSMMAPURASDISPATCHER_FCHSMMAPURASDISPATCHER_FILECODE

EFI_STATUS
EFIAPI
FchSmmApuRasDispatchRegister (
  IN       CONST FCH_SMM_APURAS_DISPATCH_PROTOCOL   *This,
  IN       FCH_SMM_APURAS_HANDLER_ENTRY_POINT       CallBackFunction,
  IN OUT   FCH_SMM_APURAS_REGISTER_CONTEXT          *MiscRegisterContext,
  OUT      EFI_HANDLE                               *DispatchHandle
  );

EFI_STATUS
EFIAPI
FchSmmApuRasDispatchUnRegister (
  IN       CONST FCH_SMM_APURAS_DISPATCH_PROTOCOL   *This,
  IN       EFI_HANDLE                               DispatchHandle
  );

VOID
FchSmmApuRasClearSmiSts (
  );

FCH_SMM_APURAS_DISPATCH_PROTOCOL  gFchSmmApuRasDispatchProtocol = {
  FchSmmApuRasDispatchRegister,
  FchSmmApuRasDispatchUnRegister
};

/*----------------------------------------------------------------------------------------*/

/**
 * FCH SMM APU RAS Smi dispatcher handler
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
FchSmmApuRasDispatchHandler (
  IN       EFI_HANDLE   SmmImageHandle,
  IN OUT   VOID         *CommunicationBuffer OPTIONAL,
  IN OUT   UINTN        *SourceSize OPTIONAL
  )
{
  EFI_STATUS           Status;
  FCH_SMM_APURAS_NODE  *CurrentFchSmmApuRasNodePtr;

  if (PcdGetBool(PcdHygonUsbSupport)) {
    //
    // 1, call USB dispatcher first as SAT design
    //
    Status = FchSmmUsbDispatchHandler (SmmImageHandle, CommunicationBuffer, SourceSize);
  }
  
  //
  // 2, loop for any registered RAS callback
  //
  if (HeadFchSmmApuRasNodePtr->FchApuRasNodePtr == NULL) {
    Status = EFI_NOT_FOUND;
  } else {
    CurrentFchSmmApuRasNodePtr = HeadFchSmmApuRasNodePtr;
    while (CurrentFchSmmApuRasNodePtr->FchApuRasNodePtr != NULL) {
      if (CurrentFchSmmApuRasNodePtr->CallBackFunction != NULL) {
        Status = CurrentFchSmmApuRasNodePtr->CallBackFunction (
                                               CurrentFchSmmApuRasNodePtr->DispatchHandle,
                                               &CurrentFchSmmApuRasNodePtr->Context
                                               );
        DEBUG ((
          DEBUG_VERBOSE,
          "[FchSmmDispatcher] APU RAS SMM handler dispatched: Order = 0x%x, \
               return - %r\n",
          CurrentFchSmmApuRasNodePtr->Context.Order,
          Status
          ));
      }

      CurrentFchSmmApuRasNodePtr = CurrentFchSmmApuRasNodePtr->FchApuRasNodePtr;
    }
  }

  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG84) = ApuRasSmi;
  FchSmmApuRasClearSmiSts ();

  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Register APU RAS child handler
 *
 *
 * @param[in]       This                  Pointer to protocol
 * @param[in]       CallBackFunction
 * @param[in, out]  ApuRasRegisterContext
 * @param[out]      DispatchHandle        Handle (see PI 1.1 for more details)
 *
 * @retval          EFI_SUCCESS           SMI handled by dispatcher
 * @retval          EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
FchSmmApuRasDispatchRegister (
  IN       CONST FCH_SMM_APURAS_DISPATCH_PROTOCOL   *This,
  IN       FCH_SMM_APURAS_HANDLER_ENTRY_POINT       CallBackFunction,
  IN OUT   FCH_SMM_APURAS_REGISTER_CONTEXT          *ApuRasRegisterContext,
  OUT      EFI_HANDLE                               *DispatchHandle
  )
{
  UINT8                RbBusNum;
  EFI_STATUS           Status;
  FCH_SMM_APURAS_NODE  *NewFchSmmApuRasNodePtr;
  FCH_SMM_APURAS_NODE  *CurrentFchSmmApuRasNodePtr;
  FCH_SMM_APURAS_NODE  *PreviousFchSmmApuRasNodePtr;

  Status = EFI_OUT_OF_RESOURCES;

  if (CallBackFunction == NULL || ApuRasRegisterContext == NULL || DispatchHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FCH_SMM_APURAS_NODE),
                    &NewFchSmmApuRasNodePtr
                    );
  ASSERT (!EFI_ERROR (Status));

  NewFchSmmApuRasNodePtr->CallBackFunction = CallBackFunction;
  NewFchSmmApuRasNodePtr->Context = *ApuRasRegisterContext;
  *DispatchHandle = &NewFchSmmApuRasNodePtr->DispatchHandle;
  NewFchSmmApuRasNodePtr->DispatchHandle = *DispatchHandle;

  DEBUG ((
    DEBUG_INFO,
    "[FchSmmDispatcher] Registering APU RAS SMM handler: Order = 0x%x\n", \
    ApuRasRegisterContext->Order
    ));
  if (HeadFchSmmApuRasNodePtr->FchApuRasNodePtr == NULL) {
    NewFchSmmApuRasNodePtr->FchApuRasNodePtr = HeadFchSmmApuRasNodePtr;
    HeadFchSmmApuRasNodePtr = NewFchSmmApuRasNodePtr;
  } else {
    PreviousFchSmmApuRasNodePtr = HeadFchSmmApuRasNodePtr;
    CurrentFchSmmApuRasNodePtr  = HeadFchSmmApuRasNodePtr;
    while (CurrentFchSmmApuRasNodePtr->FchApuRasNodePtr != NULL) {
      if (NewFchSmmApuRasNodePtr->Context.Order <= CurrentFchSmmApuRasNodePtr->Context.Order &&
          CurrentFchSmmApuRasNodePtr->Context.Socket == NewFchSmmApuRasNodePtr->Context.Socket &&
          CurrentFchSmmApuRasNodePtr->Context.Rb == NewFchSmmApuRasNodePtr->Context.Rb &&
          CurrentFchSmmApuRasNodePtr->Context.Bus == NewFchSmmApuRasNodePtr->Context.Bus) {
        if (PreviousFchSmmApuRasNodePtr == CurrentFchSmmApuRasNodePtr) {
          NewFchSmmApuRasNodePtr->FchApuRasNodePtr = HeadFchSmmApuRasNodePtr;
          HeadFchSmmApuRasNodePtr = NewFchSmmApuRasNodePtr;
          Status = EFI_SUCCESS;
          return Status;
        }

        NewFchSmmApuRasNodePtr->FchApuRasNodePtr = PreviousFchSmmApuRasNodePtr->FchApuRasNodePtr;
        PreviousFchSmmApuRasNodePtr->FchApuRasNodePtr = NewFchSmmApuRasNodePtr;

        Status = EFI_SUCCESS;
        return Status;
      }

      PreviousFchSmmApuRasNodePtr = CurrentFchSmmApuRasNodePtr;
      CurrentFchSmmApuRasNodePtr  = CurrentFchSmmApuRasNodePtr->FchApuRasNodePtr;
    }

    PreviousFchSmmApuRasNodePtr->FchApuRasNodePtr = NewFchSmmApuRasNodePtr;
    NewFchSmmApuRasNodePtr->FchApuRasNodePtr      = CurrentFchSmmApuRasNodePtr;
  }

  RbBusNum = (UINT8)FabricTopologyGetHostBridgeBusBase (ApuRasRegisterContext->Socket, 0, ApuRasRegisterContext->Rb);

  FchSmnRW ((UINT32)RbBusNum, NBIO_SPACE2(0, ApuRasRegisterContext->Rb, RAS_GLOBAL_STATUS_LO_HYEX), 0xfffffeff, BIT8, NULL); //HWSMI_STS
  FchSmnRW ((UINT32)RbBusNum, NBIO_SPACE2(0, ApuRasRegisterContext->Rb, MISC_RAS_CONTROL_HYEX), 0xffffbfff, BIT14, NULL);    //PCIE_SMI_EN
  FchSmnRW ((UINT32)RbBusNum, NBIO_SPACE2(0, ApuRasRegisterContext->Rb, IOHC_INTERRUPT_EOI_HYEX), 0xfffffffe, BIT0, NULL);   //SMI_EOI

  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG84)  = ApuRasSmi;
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAC) |= BIT10;
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG84)  = ApuRasSmi;

  Status = EFI_SUCCESS;
  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Unregister APU RAS child handler
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
FchSmmApuRasDispatchUnRegister (
  IN       CONST FCH_SMM_APURAS_DISPATCH_PROTOCOL   *This,
  IN       EFI_HANDLE                               DispatchHandle
  )
{
  EFI_STATUS           Status;
  FCH_SMM_APURAS_NODE  *CurrentFchSmmApuRasNodePtr;
  FCH_SMM_APURAS_NODE  *PreviousFchSmmApuRasNodePtr;

  if (DispatchHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (HeadFchSmmApuRasNodePtr->FchApuRasNodePtr == NULL) {
    Status = EFI_NOT_FOUND;
    return Status;
  } else {
    PreviousFchSmmApuRasNodePtr = HeadFchSmmApuRasNodePtr;
    CurrentFchSmmApuRasNodePtr  = HeadFchSmmApuRasNodePtr;
    if (CurrentFchSmmApuRasNodePtr->DispatchHandle == DispatchHandle) {
      HeadFchSmmApuRasNodePtr = CurrentFchSmmApuRasNodePtr->FchApuRasNodePtr;
    } else {
      while (CurrentFchSmmApuRasNodePtr->DispatchHandle != DispatchHandle) {
        PreviousFchSmmApuRasNodePtr = CurrentFchSmmApuRasNodePtr;
        CurrentFchSmmApuRasNodePtr  = CurrentFchSmmApuRasNodePtr->FchApuRasNodePtr;
        if (CurrentFchSmmApuRasNodePtr->DispatchHandle == NULL) {
          Status = EFI_NOT_FOUND;
          return Status;
        }
      }

      PreviousFchSmmApuRasNodePtr->FchApuRasNodePtr = CurrentFchSmmApuRasNodePtr->FchApuRasNodePtr;
    }

    Status = gSmst->SmmFreePool (
                      CurrentFchSmmApuRasNodePtr
                      );
    ASSERT (!EFI_ERROR (Status));
  }

  Status = EFI_SUCCESS;
  return Status;
}

VOID
FchSmmApuRasClearSmiSts (
  VOID
  )
{
  UINTN                                    RbBusNum;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  UINTN                                    NumberOfInstalledProcessors;
  UINTN                                    NumberOfRb;
  UINTN                                    RbNumberOfPerSocket;
  UINTN                                    SocketIndex, RbIndex;
  EFI_STATUS                               Status;

  Status = gSmst->SmmLocateProtocol (
                    &gHygonFabricTopologyServicesSmmProtocolGuid,
                    NULL,
                    &FabricTopology
                    );

  if (!EFI_ERROR (Status)) {
    FabricTopology->GetSystemInfo (FabricTopology, &NumberOfInstalledProcessors, NULL, &NumberOfRb);
    RbNumberOfPerSocket = NumberOfRb / NumberOfInstalledProcessors;

    //Register Error handler per Rb.
    for (SocketIndex = 0; SocketIndex < NumberOfInstalledProcessors; SocketIndex++) {
      for (RbIndex = 0; RbIndex < RbNumberOfPerSocket; RbIndex++) { 
        FabricTopology->GetRootBridgeInfo (FabricTopology, SocketIndex, 0, RbIndex, NULL, &RbBusNum, NULL);
        FchSmnRW ((UINT32)RbBusNum, NBIO_SPACE2(0, RbIndex, IOHC_INTERRUPT_EOI_HYEX), 0xfffffffe, BIT0, NULL);
        FchSmnRW ((UINT32)RbBusNum, NBIO_SPACE2(0, RbIndex, RAS_GLOBAL_STATUS_LO_HYEX), 0xfffffeff, BIT8, NULL);
      }
    }
  }
}

