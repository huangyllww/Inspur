/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Hudson-2 SMI Dispatcher Driver
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
#include "FchSmmDispatcher.h"
#define FILECODE  UEFI_SMM_FCHSMMDISPATCHER_FCHSMMDISPATCHER_FILECODE

#include <Protocol/SmmBase2.h>
#include <Protocol/SmmConfiguration.h>
#include <Protocol/SmmCpuIo2.h>
#include <Library/BaseFabricTopologyLib.h>

extern FCH_SMM_DISPATCHER_TABLE FchSmmDispatcherTable[];
extern UINT8        NumOfDispatcherTableEntry;
extern SAVE_B2B_IO  B2bIoList[];
extern UINT8        NumOfB2bIoListEntry;
extern SAVE_PCI     SavePciList[];
extern UINT8        NumOfSavePciListEntry;
extern FCH_USB_SMI_SYSINFO  FchUsbSmiSysInfo[];

EFI_SMM_CPU_PROTOCOL  *mSmmCpuProtocol;

FCH_SMM_SW_NODE               *HeadFchSmmSwNodePtr;
FCH_SMM_SX_NODE               *HeadFchSmmSxNodePtr;
FCH_SMM_PWRBTN_NODE           *HeadFchSmmPwrBtnNodePtr;
FCH_SMM_PERIODICAL_NODE       *HeadFchSmmPeriodicalNodePtr;
FCH_SMM_PERIODICAL_NODE       *HeadFchSmmPeriodicalNodePtr2;
FCH_SMM_GPI_NODE              *HeadFchSmmGpiNodePtr;
FCH_SMM_USB_NODE              *HeadFchSmmUsbNodePtr;
FCH_SMM_MISC_NODE             *HeadFchSmmMiscNodePtr;
FCH_SMM_APURAS_NODE           *HeadFchSmmApuRasNodePtr;
FCH_SMM_COMMUNICATION_BUFFER  *CommunicationBufferPtr;
FCH_SMM_SW_CONTEXT            *EfiSmmSwContext;

EFI_SMM_PERIODIC_TIMER_CONTEXT  EfiSmmPeriodicTimerContext;
BOOLEAN                         gPcdHygonUsbSupport;

typedef struct {
  EFI_GUID    *Guid;
  VOID        *Interface;
} FCH_PROTOCOL_LIST;

FCH_PROTOCOL_LIST  FchProtocolList[] = {
  &gFchSmmSwDispatch2ProtocolGuid,            &gFchSmmSwDispatch2Protocol,
  &gEfiSmmSwDispatch2ProtocolGuid,            &gEfiSmmSwDispatch2Protocol,
  &gFchSmmSxDispatch2ProtocolGuid,            &gFchSmmSxDispatch2Protocol,
  &gEfiSmmSxDispatch2ProtocolGuid,            &gEfiSmmSxDispatch2Protocol,
  &gFchSmmPwrBtnDispatch2ProtocolGuid,        &gFchSmmPwrBtnDispatch2Protocol,
  &gEfiSmmPowerButtonDispatch2ProtocolGuid,   &gEfiSmmPwrBtnDispatch2Protocol,
  &gFchSmmPeriodicalDispatch2ProtocolGuid,    &gFchSmmPeriodicalDispatch2Protocol,
  &gEfiSmmPeriodicTimerDispatch2ProtocolGuid, &gEfiSmmPeriodicalDispatch2Protocol,
  &gFchSmmUsbDispatch2ProtocolGuid,           &gFchSmmUsbDispatch2Protocol,
  &gEfiSmmUsbDispatch2ProtocolGuid,           &gEfiSmmUsbDispatch2Protocol,
  &gFchSmmGpiDispatch2ProtocolGuid,           &gFchSmmGpiDispatch2Protocol,
  &gEfiSmmGpiDispatch2ProtocolGuid,           &gEfiSmmGpiDispatch2Protocol,
  &gFchSmmIoTrapDispatch2ProtocolGuid,        &gFchSmmIoTrapDispatch2Protocol,
  &gEfiSmmIoTrapDispatch2ProtocolGuid,        &gEfiSmmIoTrapDispatch2Protocol,
  &gFchSmmMiscDispatchProtocolGuid,           &gFchSmmMiscDispatchProtocol,
};

FCH_PROTOCOL_LIST  FchProtocolListRas[] = {
  &gFchSmmApuRasDispatchProtocolGuid, &gFchSmmApuRasDispatchProtocol,
};
  
EFI_STATUS
EFIAPI
FchSmmDispatchHandler (
  IN       EFI_HANDLE                   SmmImageHandle,
  IN       CONST EFI_SMM_ENTRY_CONTEXT  *SmmEntryContext,
  IN OUT   VOID                         *CommunicationBuffer OPTIONAL,
  IN OUT   UINTN                        *SourceSize OPTIONAL
  );

VOID
HyExFchSmmInitUsbSmiSysInfo (
  VOID
  );

/*----------------------------------------------------------------------------------------*/

/**
 * Entry point of the HYGON FCH SMM dispatcher driver
 * Example of dispatcher driver that handled IO TRAP requests only
 *
 * @param[in]     ImageHandle    Pointer to the firmware file system header
 * @param[in]     SystemTable    Pointer to System table
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
FchSmmDispatcherHyExEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  DispatchHandle;
  EFI_HANDLE  FchSmmDispatcherHandle;
  UINTN       i;

  HGPI_TESTPOINT (TpFchSmmDispatcherEntry, NULL);

  gPcdHygonUsbSupport = PcdGetBool (PcdHygonUsbSupport);

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmCpuProtocolGuid,
                    NULL,
                    &mSmmCpuProtocol
                    );
  ASSERT (!EFI_ERROR (Status));

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FCH_SMM_SW_NODE),
                    &HeadFchSmmSwNodePtr
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (HeadFchSmmSwNodePtr, sizeof (FCH_SMM_SW_NODE));

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FCH_SMM_SX_NODE),
                    &HeadFchSmmSxNodePtr
                    );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (HeadFchSmmSxNodePtr, sizeof (FCH_SMM_SX_NODE));

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FCH_SMM_PWRBTN_NODE),
                    &HeadFchSmmPwrBtnNodePtr
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (HeadFchSmmPwrBtnNodePtr, sizeof (FCH_SMM_PWRBTN_NODE));

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FCH_SMM_PERIODICAL_NODE),
                    &HeadFchSmmPeriodicalNodePtr
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (HeadFchSmmPeriodicalNodePtr, sizeof (FCH_SMM_PERIODICAL_NODE));

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FCH_SMM_PERIODICAL_NODE),
                    &HeadFchSmmPeriodicalNodePtr2
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (HeadFchSmmPeriodicalNodePtr2, sizeof (FCH_SMM_PERIODICAL_NODE));

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FCH_SMM_GPI_NODE),
                    &HeadFchSmmGpiNodePtr
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (HeadFchSmmGpiNodePtr, sizeof (FCH_SMM_GPI_NODE));
  HeadFchSmmGpiNodePtr->Context.GpiNum = 0xffff;

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FCH_SMM_USB_NODE),
                    &HeadFchSmmUsbNodePtr
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (HeadFchSmmUsbNodePtr, sizeof (FCH_SMM_USB_NODE));
  HeadFchSmmUsbNodePtr->Context.Order = 0xFF;

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FCH_SMM_MISC_NODE),
                    &HeadFchSmmMiscNodePtr
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (HeadFchSmmMiscNodePtr, sizeof (FCH_SMM_MISC_NODE));

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FCH_SMM_SW_CONTEXT),
                    &EfiSmmSwContext
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (EfiSmmSwContext, sizeof (FCH_SMM_SW_CONTEXT));

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FCH_SMM_COMMUNICATION_BUFFER),
                    &CommunicationBufferPtr
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (i = 0; i < sizeof (FchProtocolList) / sizeof (FCH_PROTOCOL_LIST); i++ ) {
    FchSmmDispatcherHandle =  NULL;
    Status = gSmst->SmmInstallProtocolInterface (
                      &FchSmmDispatcherHandle,
                      FchProtocolList[i].Guid,
                      EFI_NATIVE_INTERFACE,
                      FchProtocolList[i].Interface
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  if ( PcdGetBool (PcdHygonFchApuRasSmiSupport)) {
    Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      sizeof (FCH_SMM_APURAS_NODE),
                      &HeadFchSmmApuRasNodePtr
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    ZeroMem (HeadFchSmmApuRasNodePtr, sizeof (FCH_SMM_APURAS_NODE));

    FchSmmDispatcherHandle =  NULL;
    Status = gSmst->SmmInstallProtocolInterface (
                      &FchSmmDispatcherHandle,
                      FchProtocolListRas[0].Guid,
                      EFI_NATIVE_INTERFACE,
                      FchProtocolListRas[0].Interface
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    // Find handler for APU Hw Assertion bit
    for (i = 0; i < NumOfDispatcherTableEntry; i++ ) {
      if ((FchSmmDispatcherTable[i].StatusReg == FCH_SMI_REG84) && (FchSmmDispatcherTable[i].SmiStatusBit == ApuRasSmi)) {
        FchSmmDispatcherTable[i].SmiDispatcher = FchSmmApuRasDispatchHandler;
      }
    }
  }

  Status = gSmst->SmiHandlerRegister (
                    FchSmmDispatchHandler,
                    NULL,
                    &DispatchHandle
                    );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  {
    UINT32  SmmDispatcherData32;
    UINT32  SmmDispatcherIndex;

    //
    // Clear all handled SMI status bit
    //
    for (SmmDispatcherIndex = 0; SmmDispatcherIndex < NumOfDispatcherTableEntry; SmmDispatcherIndex++ ) {
      SmmDispatcherData32  = ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FchSmmDispatcherTable[SmmDispatcherIndex].StatusReg);
      SmmDispatcherData32 &= FchSmmDispatcherTable[SmmDispatcherIndex].SmiStatusBit;
      ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FchSmmDispatcherTable[SmmDispatcherIndex].StatusReg) = SmmDispatcherData32;
    }

    //
    // Clear SmiEnB and Set EOS
    //
    SmmDispatcherData32  = ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98);
    SmmDispatcherData32 &= ~(BIT31);
    SmmDispatcherData32 |= BIT28;
    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98) = SmmDispatcherData32;
  }

  HyExFchSmmInitUsbSmiSysInfo ();

  HGPI_TESTPOINT (TpFchSmmDispatcherExit, NULL);
  return Status;
}

VOID
SaveB2BRegisters (
  VOID
  )
{
  EFI_SMM_CPU_IO2_PROTOCOL  *SmmCpuIo;
  UINT8                     i;
  UINT32                    PciAddress;

  SmmCpuIo = &gSmst->SmmIo;

  for (i = 0; i < NumOfB2bIoListEntry; i++) {
    SmmCpuIo->Io.Read (SmmCpuIo, B2bIoList[i].ioWidth, B2bIoList[i].ioPort, 1, &B2bIoList[i].ioValue);
  }

  for (i = 0; i < NumOfSavePciListEntry; i++) {
    PciAddress  = 0x80000000;
    PciAddress |= 
      ((SavePciList[i].Bus << 16) | (SavePciList[i].Dev << 11) | (SavePciList[i].Func << 8) | (SavePciList[i].Offset));
    SmmCpuIo->Io.Write (SmmCpuIo, SMM_IO_UINT32, CFG_ADDR_PORT, 1, &PciAddress);
    SmmCpuIo->Io.Read (SmmCpuIo, SavePciList[i].DataWidth, CFG_DATA_PORT, 1, &SavePciList[i].DataValue);
  }
}

VOID
RestoreB2BRegisters (
  VOID
  )
{
  EFI_SMM_CPU_IO2_PROTOCOL  *SmmCpuIo;
  UINT8                     i;
  UINT32                    PciAddress;

  SmmCpuIo = &gSmst->SmmIo;

  for (i = 0; i < NumOfSavePciListEntry; i++) {
    PciAddress  = 0x80000000;
    PciAddress |= 
      ((SavePciList[i].Bus << 16) | (SavePciList[i].Dev << 11) | (SavePciList[i].Func << 8) | (SavePciList[i].Offset));
    SmmCpuIo->Io.Write (SmmCpuIo, SMM_IO_UINT32, CFG_ADDR_PORT, 1, &PciAddress);
    SmmCpuIo->Io.Write (SmmCpuIo, SavePciList[i].DataWidth, CFG_DATA_PORT, 1, &SavePciList[i].DataValue);
  }

  for (i = 0; i < NumOfB2bIoListEntry; i++) {
    SmmCpuIo->Io.Write (SmmCpuIo, B2bIoList[i].ioWidth, B2bIoList[i].ioPort, 1, &B2bIoList[i].ioValue);
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * FCH SMM dispatcher handler
 *
 *
 * @param[in]       SmmImageHandle        Image Handle
 * @param[in]       SmmEntryContext         (see PI 1.2 for more details)
 * @param[in, out]   OPTIONAL CommunicationBuffer   Communication Buffer (see PI 1.1 for more details)
 * @param[in, out]   OPTIONAL SourceSize            Buffer size (see PI 1.1 for more details)

 * @retval          EFI_SUCCESS           SMI handled by dispatcher
 * @retval          EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
FchSmmDispatchHandler (
  IN       EFI_HANDLE                   SmmImageHandle,
  IN       CONST EFI_SMM_ENTRY_CONTEXT  *SmmEntryContext,
  IN OUT   VOID                         *CommunicationBuffer OPTIONAL,
  IN OUT   UINTN                        *SourceSize OPTIONAL
  )
{
  UINT8       SmmDispatcherIndex;
  UINT32      SmiStatusData;
  UINT32      SmiReg88StatusData;
  UINT32      UsbSmiEnableStatus[MAX_XHCI_CONTROLLERS] = {0};
  UINT32      UsbSmiEnableRegister[MAX_XHCI_CONTROLLERS] = {0};
  UINT32      EosStatus;
  EFI_STATUS  Status;
  UINT32      SocketIndex;
  UINT32      XhciPerSocketIndex;
  UINTN       SocketPresent;
  UINT32      XhciIndex;

  Status = EFI_WARN_INTERRUPT_SOURCE_PENDING; // Updated to be compliant with UDK2010.SR1.UP1
  SaveB2BRegisters ();
  SmiReg88StatusData = ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG88) & SmiCmdPort;

  SocketPresent = FabricTopologyGetNumberOfSocketPresent ();
  if (SocketPresent > 4) {
    SocketPresent = 4; /*max support 4 way*/
  }
  if (gPcdHygonUsbSupport) {
    for (SocketIndex = 0 ; SocketIndex < SocketPresent; SocketIndex++) {
      for (XhciPerSocketIndex = 0 ; XhciPerSocketIndex < MAX_XHCI_CONTROLLERS_PER_SATORI; XhciPerSocketIndex++) {
        XhciIndex = (SocketIndex * MAX_XHCI_CONTROLLERS_PER_SATORI) + XhciPerSocketIndex;
        if ((FchUsbSmiSysInfo[XhciIndex].RbBusNum != 0xFF) && FchUsbSmiSysInfo[XhciIndex].XhciEnable) {
          FchSmnRead (FchUsbSmiSysInfo[XhciIndex].RbBusNum, USB_SPACE_HYEX (XhciPerSocketIndex, XHCI_INTERRUPT_CONTROL_HYEX), &UsbSmiEnableRegister[XhciIndex], NULL);
          UsbSmiEnableStatus[XhciIndex] = UsbSmiEnableRegister[XhciIndex] & BIT8;
        }
      }
    }
  }

  do {
    for (SocketIndex = 0 ; SocketIndex < SocketPresent; SocketIndex++) {
      for (XhciPerSocketIndex = 0 ; XhciPerSocketIndex < MAX_XHCI_CONTROLLERS_PER_SATORI; XhciPerSocketIndex++) {
        XhciIndex = (SocketIndex * MAX_XHCI_CONTROLLERS_PER_SATORI) + XhciPerSocketIndex;
        if (UsbSmiEnableStatus[XhciIndex]) {
          FchSmnRW (FchUsbSmiSysInfo[XhciIndex].RbBusNum, USB_SPACE_HYEX (XhciPerSocketIndex, XHCI_INTERRUPT_CONTROL_HYEX), 0xfffffeff, 0x000, NULL);
        }
      }
    }

    for (SmmDispatcherIndex = 0; SmmDispatcherIndex < NumOfDispatcherTableEntry; SmmDispatcherIndex++ ) {
      SmiStatusData = ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FchSmmDispatcherTable[SmmDispatcherIndex].StatusReg);
      if ((SmiStatusData &= FchSmmDispatcherTable[SmmDispatcherIndex].SmiStatusBit) != 0) {
        CommunicationBufferPtr->SmiStatusReg = FchSmmDispatcherTable[SmmDispatcherIndex].StatusReg;
        CommunicationBufferPtr->SmiStatusBit = SmiStatusData;
        CommunicationBuffer = (VOID *)CommunicationBufferPtr;
        Status = FchSmmDispatcherTable[SmmDispatcherIndex].SmiDispatcher (
                                                             SmmImageHandle,
                                                             CommunicationBuffer,
                                                             SourceSize
                                                             );
        if (Status != EFI_SUCCESS) {
          Status = EFI_WARN_INTERRUPT_SOURCE_PENDING;
        }

        SmmDispatcherIndex = 0;
      }
    }

    for (SocketIndex = 0 ; SocketIndex < SocketPresent; SocketIndex++) {
      for (XhciPerSocketIndex = 0 ; XhciPerSocketIndex < MAX_XHCI_CONTROLLERS_PER_SATORI; XhciPerSocketIndex++) {
        XhciIndex = (SocketIndex * MAX_XHCI_CONTROLLERS_PER_SATORI) + XhciPerSocketIndex;
        if (UsbSmiEnableStatus[XhciIndex]) {
          FchSmnRW (FchUsbSmiSysInfo[XhciIndex].RbBusNum, USB_SPACE_HYEX (XhciPerSocketIndex, XHCI_INTERRUPT_CONTROL_HYEX), 0xfffffeff, BIT8, NULL);
        }
      }
    }

    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98) |= Eos;
    EosStatus = ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98) & Eos;
  } while ((EosStatus != Eos) || (ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG84) & ApuRasSmi));

  RestoreB2BRegisters ();
  return Status;
}

// temp fix..
CONST VOID  *IdsDebugPrint[] =
{
  NULL
};

VOID
HyExFchSmmInitUsbSmiSysInfo (
  VOID
  )
{
  UINT32    Data32;
  UINTN     SocketIndex;
  UINTN     XhciPerSocketIndex;
  UINTN     SocketPresent;
  UINT32    XhciIndex;
  
  FchUsbSmiSysInfo[0].XhciEnable = PcdGetBool (PcdXhci0EnableHyEx);
  FchUsbSmiSysInfo[1].XhciEnable = PcdGetBool (PcdXhci1EnableHyEx);
  FchUsbSmiSysInfo[2].XhciEnable = PcdGetBool (PcdXhci2EnableHyEx);
  FchUsbSmiSysInfo[3].XhciEnable = PcdGetBool (PcdXhci3EnableHyEx);
  FchUsbSmiSysInfo[4].XhciEnable = PcdGetBool (PcdXhci4EnableHyEx);
  FchUsbSmiSysInfo[5].XhciEnable = PcdGetBool (PcdXhci5EnableHyEx);
  FchUsbSmiSysInfo[6].XhciEnable = PcdGetBool (PcdXhci6EnableHyEx);
  FchUsbSmiSysInfo[7].XhciEnable = PcdGetBool (PcdXhci7EnableHyEx);
  
  SocketPresent = FabricTopologyGetNumberOfSocketPresent ();
  if (SocketPresent > 4) {
    SocketPresent = 4; /*max support 4 way*/
  }
  for (SocketIndex = 0 ; SocketIndex < SocketPresent; SocketIndex++) {
    for (XhciPerSocketIndex = 0 ; XhciPerSocketIndex < MAX_XHCI_CONTROLLERS_PER_SATORI; XhciPerSocketIndex++) {
      XhciIndex = (UINT32)((SocketIndex * MAX_XHCI_CONTROLLERS_PER_SATORI) + XhciPerSocketIndex);
      FchUsbSmiSysInfo[XhciIndex].RbBusNum = (UINT8) FabricTopologyGetHostBridgeBusBase (SocketIndex, 0, XhciPerSocketIndex);
      Data32 = PciRead32 (PCI_LIB_ADDRESS (FchUsbSmiSysInfo[XhciIndex].RbBusNum, 0, 0, 0));
      if (Data32 == 0xFFFFFFFF) {
        FchUsbSmiSysInfo[XhciIndex].RbBusNum = 0xFF;
      }
      DEBUG ((DEBUG_INFO, "[FchSmmInitUsbSmiSysInfo] Xhci%d RbBusNum = 0x%x\n",XhciIndex, FchUsbSmiSysInfo[XhciIndex].RbBusNum));
    }
  }
}
