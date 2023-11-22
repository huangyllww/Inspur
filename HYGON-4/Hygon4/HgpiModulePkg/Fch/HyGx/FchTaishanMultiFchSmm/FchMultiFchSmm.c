/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH SMM Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project   FCH SMM Driver
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
#include "FchMultiFchSmm.h"

typedef struct _USB_DIE_BUS_NUM {
  UINT8     Socket;
  UINT8     PhysicalDie;
  UINT32    BusNum;
} USB_DIE_BUS_NUM;

//
// Driver Global Data
//
USB_DIE_BUS_NUM            UsbDieBusNum[MAX_SOCKET_SUPPORT][MAX_IOD_SUPPORT] = {
  { { 0, 0, 0xFF }, { 0, 1, 0xFF }, { 0, 2, 0xFF }, { 0, 3, 0xFF } },     // Socket0
  { { 1, 0, 0xFF }, { 1, 1, 0xFF }, { 1, 2, 0xFF }, { 1, 3, 0xFF } },     // Socket1
  { { 2, 0, 0xFF }, { 2, 1, 0xFF }, { 2, 2, 0xFF }, { 2, 3, 0xFF } },     // Socket2
  { { 3, 0, 0xFF }, { 3, 1, 0xFF }, { 3, 2, 0xFF }, { 3, 3, 0xFF } },     // Socket3
  { { 4, 0, 0xFF }, { 4, 1, 0xFF }, { 4, 2, 0xFF }, { 4, 3, 0xFF } },     // Socket4
  { { 5, 0, 0xFF }, { 5, 1, 0xFF }, { 5, 2, 0xFF }, { 5, 3, 0xFF } },     // Socket5
  { { 6, 0, 0xFF }, { 6, 1, 0xFF }, { 6, 2, 0xFF }, { 6, 3, 0xFF } },     // Socket6
  { { 7, 0, 0xFF }, { 7, 1, 0xFF }, { 7, 2, 0xFF }, { 7, 3, 0xFF } },     // Socket7
};
UINT32                     gSmmPcdSataEnable32;
UINT32                     gSmmPcdSataSgpio32;
UINT64                     gSmmSataMultiDiePortShutDown[MAX_SOCKET_SUPPORT];
UINT64                     gSmmSataMultiDiePortEsp[MAX_SOCKET_SUPPORT];
UINT64                     gSmmPcdSataMultiDieDevSlp;
FCH_SMM_INIT_PROTOCOL      *pFchSmmInitProtocol;
FCH_MULITI_FCH_DATA_BLOCK  FchSmmMfData;
UINTN                      gNumberOfSockets = 1;
UINTN                      gNumberOfPhysicalDiePerSocket = 1;

EFI_STATUS
InitGlobalVariable (
  FCH_MULTI_FCH_INIT_PROTOCOL *FchMfInit
  )
{
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  EFI_STATUS                               Status;

  Status = gBS->LocateProtocol (
                  &gHygonFabricTopologyServicesProtocolGuid,
                  NULL,
                  &FabricTopology
                  );
  if(!EFI_ERROR (Status)) {
    FabricTopology->GetSystemInfo (
                      FabricTopology,
                      &gNumberOfSockets,
                      NULL,
                      NULL
                      );
    FabricTopology->GetProcessorInfo (
                      FabricTopology,
                      0,
                      &gNumberOfPhysicalDiePerSocket,
                      NULL,
                      NULL
                      );
  }

  gBS->CopyMem (
         &FchSmmMfData,
         &FchMfInit->FchMfData,
         sizeof (FCH_MULITI_FCH_DATA_BLOCK)
         );

  gSmmPcdSataEnable32 = PcdGet32 (PcdSataEnableHyGx);
  gSmmPcdSataSgpio32  = PcdGet32 (PcdSataSgpioHyGx);
  gSmmPcdSataMultiDieDevSlp = PcdGet64 (PcdSataMultiDieDevSlpHyGx);

  gBS->CopyMem (
         &gSmmSataMultiDiePortShutDown,
         PcdGetPtr (PcdSataMultiDiePortShutDownHyGx),
         PcdGetSize (PcdSataMultiDiePortShutDownHyGx)
         );

  gBS->CopyMem (
         &gSmmSataMultiDiePortEsp,
         PcdGetPtr (PcdSataMultiDiePortESPHyGx),
         PcdGetSize (PcdSataMultiDiePortESPHyGx)
         );

  return Status;
}

/*********************************************************************************
 * Name: FchSmmHyGxEntryPoint
 *
 * Description
 *   Entry point of the HYGON FCH SMM driver
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
MultiFchSmmHyGxInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  FCH_MULTI_FCH_INIT_PROTOCOL  *FchMfInit;
  EFI_STATUS                   Status;

  Status = gBS->LocateProtocol (
                  &gFchMultiFchInitProtocolGuid,
                  NULL,
                  &FchMfInit
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "MultiFchSmmHyGxInit, no MultiFch!!!\n"));
    return EFI_SUCCESS;     // need reture
  }

  Status = gSmst->SmmLocateProtocol (
                    &gFchSmmInitProtocolGuid,
                    NULL,
                    &pFchSmmInitProtocol
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = InitGlobalVariable (FchMfInit);
  ASSERT (!EFI_ERROR (Status));

  Status = MultiFchSmmRegisterSxSmi ();

  Status = MultiFchSmmRegisterSwSmi ();

  return Status;
}

EFI_STATUS
MultiFchSmmRegisterSxSmi (
  VOID
  )
{
  EFI_STATUS                     Status;
  FCH_SMM_SX_DISPATCH2_PROTOCOL  *HygonSxDispatch;
  FCH_SMM_SX_REGISTER_CONTEXT    SxRegisterContext;
  EFI_HANDLE                     SxHandle;

  MultiFchSmmDieBusInfo ();

  //
  // Register HYGON SX SMM
  //
  Status = gSmst->SmmLocateProtocol (
                    &gFchSmmSxDispatch2ProtocolGuid,
                    NULL,
                    &HygonSxDispatch
                    );
  ASSERT (!EFI_ERROR (Status));

  SxRegisterContext.Type  = SxS3;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 1;

  Status = HygonSxDispatch->Register (
                              HygonSxDispatch,
                              MultiFchS3SleepEntryCallback,
                              &SxRegisterContext,
                              &SxHandle
                              );

  SxRegisterContext.Type  = SxS4;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 1;

  Status = HygonSxDispatch->Register (
                              HygonSxDispatch,
                              MultiFchS4SleepEntryCallback,
                              &SxRegisterContext,
                              &SxHandle
                              );
  return Status;
}

EFI_STATUS
MultiFchSmmRegisterSwSmi (
  VOID
  )
{
  EFI_STATUS                     Status;
  FCH_SMM_SW_DISPATCH2_PROTOCOL  *HygonSwDispatch;
  FCH_SMM_SW_REGISTER_CONTEXT    SwRegisterContext;
  EFI_HANDLE                     SwHandle;

  //
  // Locate SMM SW dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gFchSmmSwDispatch2ProtocolGuid,
                    NULL,
                    &HygonSwDispatch
                    );
  ASSERT (!EFI_ERROR (Status));

  SwRegisterContext.HygonSwValue = PcdGet8 (PcdFchOemBeforePciRestoreSwSmi); // use of PCD in place of FCHOEM_BEFORE_PCI_RESTORE_SWSMI    0xD3
  SwRegisterContext.Order = 0x90;
  Status = HygonSwDispatch->Register (
                              HygonSwDispatch,
                              MultiFchBeforePciS3RestoreCallback,
                              &SwRegisterContext,
                              &SwHandle
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SwRegisterContext.HygonSwValue = PcdGet8 (PcdFchOemAfterPciRestoreSwSmi); // use of PCD in place of FCHOEM_AFTER_PCI_RESTORE_SWSMI    0xD4
  SwRegisterContext.Order = 0x90;
  Status = HygonSwDispatch->Register (
                              HygonSwDispatch,
                              MultiFchAfterPciS3RestoreCallback,
                              &SwRegisterContext,
                              &SwHandle
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}

/*++

Routine Description:
    USB S3Entry programming for Slave die

--*/
EFI_STATUS
EFIAPI
MultiFchS3SleepEntryCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SX_REGISTER_CONTEXT *DispatchContext,
  IN OUT   VOID                              *CommBuffer OPTIONAL,
  IN OUT   UINTN                             *CommBufferSize  OPTIONAL
  )
{
  UINT8                 Index;
  UINT8                 UsbDieBusCount;
  USB_DIE_BUS_NUM       *pUsbDieBusNum;
  FCH_RESET_DATA_BLOCK  FchPolicy;
  UINT8                 UsbIndex;
  UINT32                UsbNbifStrap0;

  UsbDieBusCount = (UINT8)(sizeof (UsbDieBusNum) / sizeof (USB_DIE_BUS_NUM));
  pUsbDieBusNum  = &UsbDieBusNum[0][0];
  for (Index = 0; Index < UsbDieBusCount; Index++) {
    if (pUsbDieBusNum[Index].BusNum != 0xFF) {
      for (UsbIndex = 0; UsbIndex < USB_NUMBER_PER_DUJIANG; UsbIndex++) {
        FchSmnRead (
          pUsbDieBusNum[Index].BusNum,
          NBIO_SPACE2 (pUsbDieBusNum[Index].PhysicalDie, 0, (UsbIndex == 0 ? NBIF2_USB_STRAP0_ADDRESS_HYGX : NBIF3_USB_STRAP0_ADDRESS_HYGX)),
          &UsbNbifStrap0,
          NULL
          );
        if (UsbNbifStrap0 & BIT28) {
          FchTSXhciInitS3EntryProgram (
            pUsbDieBusNum[Index].Socket,
            pUsbDieBusNum[Index].PhysicalDie,
            UsbIndex,
            pUsbDieBusNum[Index].BusNum,
            &FchPolicy
            );
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/*++

Routine Description:
    USB S4Entry programming for Slave die

--*/
EFI_STATUS
EFIAPI
MultiFchS4SleepEntryCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SX_REGISTER_CONTEXT *DispatchContext,
  IN OUT   VOID                              *CommBuffer OPTIONAL,
  IN OUT   UINTN                             *CommBufferSize  OPTIONAL
  )
{
  UINT8                 Index;
  UINT8                 UsbDieBusCount;
  USB_DIE_BUS_NUM       *pUsbDieBusNum;
  FCH_RESET_DATA_BLOCK  FchPolicy;
  UINT8                 UsbIndex;
  UINT32                UsbNbifStrap0;

  UsbDieBusCount = (UINT8)(sizeof (UsbDieBusNum) / sizeof (USB_DIE_BUS_NUM));
  pUsbDieBusNum  = &UsbDieBusNum[0][0];
  for (Index = 0; Index < UsbDieBusCount; Index++) {
    if (pUsbDieBusNum[Index].BusNum != 0xFF) {
      for (UsbIndex = 0; UsbIndex < USB_NUMBER_PER_DUJIANG; UsbIndex++) {
        FchSmnRead (pUsbDieBusNum[Index].BusNum, NBIO_SPACE2 (pUsbDieBusNum[Index].PhysicalDie, 0, (UsbIndex == 0 ? NBIF2_USB_STRAP0_ADDRESS_HYGX : NBIF3_USB_STRAP0_ADDRESS_HYGX)), &UsbNbifStrap0, NULL);
        if (UsbNbifStrap0 & BIT28) {
          TurnOffUsbPme (pUsbDieBusNum[Index].PhysicalDie, UsbIndex, pUsbDieBusNum[Index].BusNum, &FchPolicy);
        }
      }
    }
  }

  return EFI_SUCCESS;
}

VOID
MultiFchSmmDieBusInfo (
  VOID
  )
{
  UINT32  DieBusNum;
  UINTN   SocketId;
  UINTN   PhysicalDieId;
  UINTN   LogicalDieId;
  UINTN   DieType;

  for (SocketId = 0; SocketId < gNumberOfSockets; SocketId++) {
    for (PhysicalDieId = 0; PhysicalDieId < gNumberOfPhysicalDiePerSocket; PhysicalDieId++) {
      if ((SocketId == 0) && (PhysicalDieId == 0)) {
        continue;           // bypass master DJ
      }

      FabricTopologyGetPhysicalIodDieInfo (PhysicalDieId, &LogicalDieId, &DieType);
      if (DieType == IOD_DUJIANG) {
        DieBusNum = ReadSocDieBusNum (SocketId, LogicalDieId, 0);

        //
        // Save Usb Die Bus info
        //
        UsbDieBusNum[SocketId][PhysicalDieId].Socket = (UINT8)SocketId;
        UsbDieBusNum[SocketId][PhysicalDieId].PhysicalDie = (UINT8)PhysicalDieId;
        UsbDieBusNum[SocketId][PhysicalDieId].BusNum = DieBusNum;
      }
    }
  }
}

/*++

Routine Description:
  SMI handler to restore Slave Fch

--*/
EFI_STATUS
EFIAPI
MultiFchBeforePciS3RestoreCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  FCH_DATA_BLOCK  *FchDataPtr;
  EFI_STATUS      Status;
  UINT8           DieBusNum;
  UINT32          SataEnable32;
  UINT32          SataSgpio32;
  UINT8           *SataMultiDiePortShutDown;
  UINT8           *SataMultiDiePortESP;
  UINT64          SataMultiDieDevSlp;
  UINTN           SocketId;
  UINTN           PhysicalDieId;
  UINTN           LogicalDieId;
  UINTN           DieType;
  UINT8           SataIndex;
  UINT8           SataEnDieOffset;
  UINT8           SataEnOffset;

  Status = EFI_SUCCESS;

  SataEnable32             = gSmmPcdSataEnable32;
  SataSgpio32              = gSmmPcdSataSgpio32;
  SataMultiDiePortShutDown = (UINT8 *)&gSmmSataMultiDiePortShutDown[0];
  SataMultiDiePortESP      = (UINT8 *)&gSmmSataMultiDiePortEsp[0];
  SataMultiDieDevSlp       = gSmmPcdSataMultiDieDevSlp;

  FchDataPtr = (FCH_DATA_BLOCK *)(pFchSmmInitProtocol->FchSmmPolicy);

  //
  // Check Die#
  //
  for (SocketId = 0; SocketId < gNumberOfSockets; SocketId++) {
    for (PhysicalDieId = 0; PhysicalDieId < gNumberOfPhysicalDiePerSocket; PhysicalDieId++) {
      if ((SocketId == 0) && (PhysicalDieId == 0)) {
        continue;         // bypass master DJ
      }

      FabricTopologyGetPhysicalIodDieInfo (PhysicalDieId, &LogicalDieId, &DieType);
      if (DieType == IOD_DUJIANG) {
        SataEnDieOffset = (UINT8)((SocketId * MAX_SATA_NUMBER_PER_SOCKET) + (PhysicalDieId * SATA_NUMBER_PER_DUJIANG));            // each socket has MAX 4 SATA, each DJ has 1 SATA
        //
        // Update local Data Structure for each Die
        //
        FchDataPtr->HwAcpi.FchAcpiMmioBase   = (UINT32)FchSmmMfData.FchAcpiMmioBase[SocketId][PhysicalDieId];
        FchDataPtr->Sata.SataMode.SataEnable = (UINT32)((SataEnable32 >> SataEnDieOffset) & 0x1);
        FchDataPtr->Sata.SataSgpio = (UINT32)((SataSgpio32 >> SataEnDieOffset) & 0x1);

        DieBusNum = (UINT8)ReadSocDieBusNum (SocketId, LogicalDieId, 0);
        for (SataIndex = 0; SataIndex < SATA_NUMBER_PER_DUJIANG; SataIndex++) {
          SataEnOffset = SataEnDieOffset + SataIndex;

          FchDataPtr->Sata.SataPortPower.SataPortReg = SataMultiDiePortShutDown[SataEnOffset];
          FchDataPtr->Sata.SataEspPort.SataPortReg   = SataMultiDiePortESP[SataEnOffset];

          FchDataPtr->Sata.SataDevSlpPort0    = SataMultiDieDevSlp & BIT0;
          FchDataPtr->Sata.SataDevSlpPort0Num = (UINT8)((SataMultiDieDevSlp >> 1) & 0x7);
          FchDataPtr->Sata.SataDevSlpPort1    = SataMultiDieDevSlp & BIT4;
          FchDataPtr->Sata.SataDevSlpPort1Num = (UINT8)((SataMultiDieDevSlp >> 5) & 0x7);

          // SATA
          MultiFchSataInitSmm ((UINT8)PhysicalDieId, SataIndex, DieBusNum, FchDataPtr);
        }

        // Ethernet
        MultiFchXgbeInitSmm ((UINT8)PhysicalDieId, DieBusNum, FchDataPtr);
      }
    }

    SataMultiDieDevSlp = SataMultiDieDevSlp >> 8;
  }

  return Status;
}

/*++

Routine Description:
  SMI handler to restore Slave Fch

--*/
EFI_STATUS
EFIAPI
MultiFchAfterPciS3RestoreCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  FCH_DATA_BLOCK  *FchDataPtr;
  EFI_STATUS      Status;
  UINT8           DieBusNum;
  UINT64          SataEnable32;
  UINT64          SataSgpio32;
  UINT8           *SataMultiDiePortShutDown;
  UINT8           *SataMultiDiePortESP;
  UINT64          SataMultiDieDevSlp;
  UINTN           SocketId;
  UINTN           PhysicalDieId;
  UINTN           LogicalDieId;
  UINTN           DieType;
  UINT8           SataIndex;
  UINT8           SataEnDieOffset;
  UINT8           SataEnOffset;

  Status                   = EFI_SUCCESS;
  SataEnable32             = gSmmPcdSataEnable32;
  SataSgpio32              = gSmmPcdSataSgpio32;
  SataMultiDiePortShutDown = (UINT8 *)&gSmmSataMultiDiePortShutDown[0];
  SataMultiDiePortESP      = (UINT8 *)&gSmmSataMultiDiePortEsp[0];
  SataMultiDieDevSlp       = gSmmPcdSataMultiDieDevSlp;
  FchDataPtr               = (FCH_DATA_BLOCK *)(pFchSmmInitProtocol->FchSmmPolicy);

  //
  // Check Die#
  //
  for (SocketId = 0; SocketId < gNumberOfSockets; SocketId++) {
    for (PhysicalDieId = 0; PhysicalDieId < gNumberOfPhysicalDiePerSocket; PhysicalDieId++) {
      if ((SocketId == 0) && (PhysicalDieId == 0)) {
        continue;         // bypass master DJ
      }

      FabricTopologyGetPhysicalIodDieInfo (PhysicalDieId, &LogicalDieId, &DieType);
      if (DieType == IOD_DUJIANG) {
        SataEnDieOffset = (UINT8)((SocketId * MAX_SATA_NUMBER_PER_SOCKET) + (PhysicalDieId * SATA_NUMBER_PER_DUJIANG));                // each socket has MAX 4 SATA, each DJ has 1 SATA
        //
        // Update local Data Structure for each Die
        //
        FchDataPtr->HwAcpi.FchAcpiMmioBase   = (UINT32)FchSmmMfData.FchAcpiMmioBase[SocketId][PhysicalDieId];
        FchDataPtr->Sata.SataMode.SataEnable = (UINT32)((SataEnable32 >> SataEnDieOffset) & 0x1);
        FchDataPtr->Sata.SataSgpio = (UINT32)((SataSgpio32 >> SataEnDieOffset) & 0x1);

        DieBusNum = (UINT8)ReadSocDieBusNum (SocketId, LogicalDieId, 0);
        for (SataIndex = 0; SataIndex < SATA_NUMBER_PER_DUJIANG; SataIndex++) {
          SataEnOffset = SataEnDieOffset + SataIndex;

          FchDataPtr->Sata.SataPortPower.SataPortReg = SataMultiDiePortShutDown[SataEnOffset];
          FchDataPtr->Sata.SataEspPort.SataPortReg   = SataMultiDiePortESP[SataEnOffset];

          FchDataPtr->Sata.SataDevSlpPort0    = SataMultiDieDevSlp & BIT0;
          FchDataPtr->Sata.SataDevSlpPort0Num = (UINT8)((SataMultiDieDevSlp >> 1) & 0x7);
          FchDataPtr->Sata.SataDevSlpPort1    = SataMultiDieDevSlp & BIT4;
          FchDataPtr->Sata.SataDevSlpPort1Num = (UINT8)((SataMultiDieDevSlp >> 5) & 0x7);

          // SATA
          MultiFchSataInit2Smm ((UINT8)PhysicalDieId, SataIndex, DieBusNum, FchDataPtr);
        }
      }
    }

    SataMultiDieDevSlp = SataMultiDieDevSlp >> 8;
  }

  return Status;
}

VOID
MultiFchSataInitSmm (
  IN  UINT8       PhysicalDieId,
  IN  UINT8       SataIndex,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT8           PortNum;
  UINT32          DieBusNum32;
  UINT32          SataNbifStrap0;
  FCH_DATA_BLOCK  *LocalCfgPtr;

  DieBusNum32 = (UINT32)DieBusNum;
  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;

  //
  // Check if Sata is enabled by NBIO
  //
  FchSmnRead (DieBusNum32, NBIO_SPACE2 (PhysicalDieId, 0, (SataIndex == 0 ? NBIF2_SATA_STRAP0_ADDRESS_HYGX : NBIF3_SATA_STRAP0_ADDRESS_HYGX)), &SataNbifStrap0, NULL);

  if ((SataNbifStrap0 & BIT28) == 0) {
    FchSmnRW (DieBusNum32, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_TS_SATA_MISC_CONTROL_HYGX), 0xfffffffe, BIT0, NULL);
    FchSmnRW (DieBusNum32, SATA_SPACE_HYGX (PhysicalDieId, SataIndex, FCH_TS_SATA_MISC_CONTROL_HYGX), ~(UINT32)BIT11, BIT11, NULL);
    return;
  }

  if ((LocalCfgPtr->Sata.SataMode.SataEnable & (UINT32)BIT (SataIndex)) == 0) {
    FchSmnRW (DieBusNum32, NBIO_SPACE2 (PhysicalDieId, 0, (SataIndex == 0 ? NBIF2_SATA_STRAP0_ADDRESS_HYGX : NBIF3_SATA_STRAP0_ADDRESS_HYGX)), ~(UINT32)BIT28, 0, NULL);
    return;                                                // return if SATA controller is disabled.
  }

  //
  // Init Sata
  //
  SataEnableWriteAccessTS (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);

  for (PortNum = 0; PortNum < TAISHAN_SATA_PORT_NUM; PortNum++) {
    if (((LocalCfgPtr->Sata.SataPortPower.SataPortReg >> PortNum) & BIT0) == 0) {
      FchTSSataInitPortActive (PhysicalDieId, SataIndex, DieBusNum32, PortNum, FchDataPtr);
    }
  }

  // Do Sata init
  FchTSSataInitRsmuCtrl (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);

  FchTSSataInitCtrlReg (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);

  FchTSSataInitEsata (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);

  // Sata Controller0 Init DevSlp
  if ((PhysicalDieId == 0) && (SataIndex == 0)) {
    FchTSSataInitDevSlp (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);
  }

  FchTSSataInitMpssMap (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);

  FchTSSataInitEnableErr (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);

  FchTSSataSetPortGenMode (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);

  if (LocalCfgPtr->Sata.SataSgpio & (UINT32)BIT (SataIndex)) {
    FchTSSataGpioInitial (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);
  }

  if ((LocalCfgPtr->Sata.SataClass == SataAhci7804) || (LocalCfgPtr->Sata.SataClass == SataAhci)) {
    FchInitEnvSataAhciTS (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);
  }

  if ( LocalCfgPtr->Sata.SataClass == SataRaid) {
    FchInitEnvSataRaidTS (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);
  }

  SataDisableWriteAccessTS (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);
}

VOID
MultiFchXgbeInitSmm (
  IN  UINT8       PhysicalDieId,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT8           PortNum;
  UINT32          DieBusNum32;
  UINT32          SystemPort;
  UINT32          XgbeNbifStrap0;
  FCH_XGBE_PORT   *FchXgbePort;
  FCH_XGBE_MAC    *FchXgbePortMac;
  FCH_DATA_BLOCK  *LocalCfgPtr;

  if (PcdGet8 (PcdRunEnvironment) > 0) {
    IDS_HDT_CONSOLE (FCH_TRACE, "  Not execute MultiFchXgbeInitSmm in presilicon env \n");
    return;
  }
  
  DieBusNum32    = (UINT32)DieBusNum;
  LocalCfgPtr    = (FCH_DATA_BLOCK *)FchDataPtr;
  FchXgbePort    = &LocalCfgPtr->Xgbe.Port[0];
  FchXgbePortMac = &LocalCfgPtr->Xgbe.Mac[0];

  for (PortNum = 0; PortNum < 4; PortNum++) {
    //
    // Check if GMAC is enabled
    //
    FchSmnRead (DieBusNum32, NBIO_SPACE2 (PhysicalDieId, 0, NBIF2_XGBE0_STRAP0_ADDRESS_HYGX+ PortNum * 0x200), &XgbeNbifStrap0, NULL);

    if (XgbeNbifStrap0 & BIT28) {
      SystemPort = LocalCfgPtr->Xgbe.MaxPortNum;
      LocalCfgPtr->Xgbe.MaxPortNum++;
      if (FchXgbePort->XgbePortConfig) {
        FchTsXgbePortPlatformConfig (PhysicalDieId, DieBusNum32, PortNum, SystemPort, FchXgbePort);
        FchTsXgbePortMacAddress (PhysicalDieId, DieBusNum32, PortNum, FchXgbePortMac);
        FchTsXgbePortPtpInit (PhysicalDieId, DieBusNum32, PortNum, FchXgbePort);
      }
    } else {
      FchTsXgbePortClockGate (PhysicalDieId, DieBusNum32, PortNum);
    }

    FchXgbePort++;
    FchXgbePortMac++;
  }

  FchTsXgbePadEnable (DieBusNum32, LocalCfgPtr);
}

VOID
MultiFchSataInit2Smm (
  IN  UINT8       PhysicalDieId,
  IN  UINT8       SataIndex,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT32          DieBusNum32;
  UINT32          SataNbifStrap0;
  FCH_DATA_BLOCK  *LocalCfgPtr;

  DieBusNum32 = (UINT32)DieBusNum;
  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;

  //
  // Check if Sata is enabled by NBIO
  //
  FchSmnRead (DieBusNum32, NBIO_SPACE2 (PhysicalDieId, 0, (SataIndex == 0 ? NBIF2_SATA_STRAP0_ADDRESS_HYGX : NBIF3_SATA_STRAP0_ADDRESS_HYGX)), &SataNbifStrap0, NULL);

  if ((SataNbifStrap0 & BIT28) == 0) {
    return;
  }

  //
  // Set Sata PCI Configuration Space Write enable
  //
  SataEnableWriteAccessTS (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);
  //
  // Call Sub-function for each Sata mode
  //
  if ((LocalCfgPtr->Sata.SataClass == SataAhci7804) || (LocalCfgPtr->Sata.SataClass == SataAhci)) {
    FchInitLateSataAhci (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);
  }

  if ( LocalCfgPtr->Sata.SataClass == SataRaid) {
    FchInitLateSataRaid (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);
  }

  FchTSSataShutdownUnconnectedSataPortClock (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);

  FchTSSataInitMMC (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);
  //
  // Set Sata PCI Configuration Space Write disable
  //
  SataDisableWriteAccessTS (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);
}
