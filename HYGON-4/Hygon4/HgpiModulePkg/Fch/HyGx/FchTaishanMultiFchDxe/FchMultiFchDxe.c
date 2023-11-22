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
#include "FchMultiFchDxe.h"

extern EFI_GUID  gFchMultiFchResetDataHobGuid;

//
// Driver Global Data
//

/*********************************************************************************
 * Name: MultiFchDxeHyGxInit
 *
 * Description
 *   Entry point of the HYGON FCH MCM DXE driver
 *   Perform the configuration init, resource reservation, early post init
 *   and install all the supported protocol for FCH MCM support
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
MultiFchDxeHyGxInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  EFI_HOB_GUID_TYPE                        *FchHob;
  FCH_MULTI_FCH_DXE_PRIVATE                *FchMfPrivate;
  FCH_MULITI_FCH_DATA_BLOCK                *FchMfData;
  FCH_INIT_PROTOCOL                        *FchInit;
  FCH_DATA_BLOCK                           *FchDataPtr;
  EFI_STATUS                               Status;
  EFI_HANDLE                               Handle;
  UINT8                                    NbioBusNum;
  UINT32                                   SataEnable32;
  UINT32                                   SataSgpio32;
  UINT8                                    *SataMultiDiePortShutDown;
  UINT8                                    *SataMultiDiePortESP;
  UINT64                                   SataMultiDieDevSlp;
  UINTN                                    NumberOfSockets;
  UINTN                                    NumberOfPhysicalDie;
  UINTN                                    SocketId;
  UINTN                                    PhysicalDieId;
  UINTN                                    LogicalDieId;
  UINTN                                    DieType;
  UINTN                                    NumberOfFch;
  UINT8                                    SataIndex;
  UINT8                                    SataEnDieOffset;
  UINT8                                    SataEnOffset;

  HGPI_TESTPOINT (TpFchMultiFchDxeEntry, NULL);

  Status = gBS->LocateProtocol (
                  &gHygonFabricTopologyServicesProtocolGuid,
                  NULL,
                  &FabricTopology
                  );
  FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL);
  FabricTopology->GetProcessorInfo (FabricTopology, 0, &NumberOfPhysicalDie, NULL, NULL);

  NumberOfFch = 0;
  for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
    if (NumberOfPhysicalDie == 1) {
        NumberOfFch++;  // Count Satori FCH
    } else {
      for (PhysicalDieId = 0; PhysicalDieId < NumberOfPhysicalDie; PhysicalDieId++) {
        FabricTopologyGetPhysicalIodDieInfo (PhysicalDieId, &LogicalDieId, &DieType);
        if (DieType == IOD_DUJIANG) {
          NumberOfFch++;  // Count Dujiang FCH
        }
      } 
    }
  }

  if (NumberOfFch < 2) {
    return EFI_SUCCESS;
  }

  //
  // Initialize EFI library
  //

  //
  // Initialize the configuration structure and private data area
  //
  // find HOB and update with reset data block
  FchHob = GetFirstGuidHob (&gFchMultiFchResetDataHobGuid);
  FchHob++;

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (FCH_MULTI_FCH_DXE_PRIVATE),
                  &FchMfPrivate
                  );

  ASSERT (!EFI_ERROR (Status));

  // Update Private Data
  ZeroMem (FchMfPrivate, sizeof (FCH_MULTI_FCH_DXE_PRIVATE));
  FchMfPrivate->Signature                = FCH_MULTI_FCH_DXE_PRIVATE_DATA_SIGNATURE;
  FchMfPrivate->FchMultiFchInit.Revision = FCH_MULTI_FCH_INIT_REV;
  FchMfData                              = &FchMfPrivate->FchMultiFchInit.FchMfData;
  gBS->CopyMem (
         FchMfData,
         FchHob,
         sizeof (FCH_MULITI_FCH_DATA_BLOCK)
         );

  //
  // Locate Fch INIT Protocol
  //
  Status = gBS->LocateProtocol (
                  &gFchInitProtocolGuid,
                  NULL,
                  &FchInit
                  );
  ASSERT (!EFI_ERROR (Status));

  // Allocate memory for the local data
  Status = gBS->AllocatePool (
                  EfiRuntimeServicesData, // review for memory type
                  sizeof (FCH_DATA_BLOCK),
                  &FchDataPtr
                  );
  ASSERT (!EFI_ERROR (Status));

  gBS->CopyMem (
         FchDataPtr,
         FchInit->FchPolicy,
         sizeof (FCH_DATA_BLOCK)
         );

  //
  // Update local Data Structure
  //
  SataEnable32             = PcdGet32 (PcdSataEnableHyGx);
  SataSgpio32              = PcdGet32 (PcdSataSgpioHyGx);
  SataMultiDiePortShutDown = (UINT8 *)PcdGetPtr (PcdSataMultiDiePortShutDownHyGx);
  SataMultiDiePortESP      = (UINT8 *)PcdGetPtr (PcdSataMultiDiePortESPHyGx);
  SataMultiDieDevSlp       = PcdGet64 (PcdSataMultiDieDevSlpHyGx);

  //
  // Check Die#
  //
  for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
    for (PhysicalDieId = 0; PhysicalDieId < NumberOfPhysicalDie; PhysicalDieId++) {
      if ((SocketId == 0) && (PhysicalDieId == 0)) {
        continue;           // Bypass master dujiang or Bypass master Satori
      }

      FabricTopologyGetPhysicalIodDieInfo (PhysicalDieId, &LogicalDieId, &DieType);
      if (DieType == IOD_DUJIANG) {
        SataEnDieOffset = (UINT8)((SocketId * MAX_SATA_NUMBER_PER_SOCKET) + (PhysicalDieId * SATA_NUMBER_PER_DUJIANG));
        //
        // Update local Data Structure for each Socket
        //
        FchDataPtr->HwAcpi.FchAcpiMmioBase   = (UINT32)FchMfData->FchAcpiMmioBase[SocketId][PhysicalDieId];
        FchDataPtr->Sata.SataMode.SataEnable = (UINT32)((SataEnable32 >> SataEnDieOffset) & 0x1);
        FchDataPtr->Sata.SataSgpio = (UINT32)((SataSgpio32 >> SataEnDieOffset) & 0x1);

        NbioBusNum = (UINT8)ReadSocDieBusNum (SocketId, LogicalDieId, 0);
        for (SataIndex = 0; SataIndex < SATA_NUMBER_PER_DUJIANG; SataIndex++) {
          SataEnOffset = SataEnDieOffset + SataIndex;

          FchDataPtr->Sata.SataPortPower.SataPortReg = SataMultiDiePortShutDown[SataEnOffset];
          FchDataPtr->Sata.SataEspPort.SataPortReg   = SataMultiDiePortESP[SataEnOffset];

          FchDataPtr->Sata.SataDevSlpPort0    = SataMultiDieDevSlp & BIT0;
          FchDataPtr->Sata.SataDevSlpPort0Num = (UINT8)((SataMultiDieDevSlp >> 1) & 0x7);
          FchDataPtr->Sata.SataDevSlpPort1    = SataMultiDieDevSlp & BIT4;
          FchDataPtr->Sata.SataDevSlpPort1Num = (UINT8)((SataMultiDieDevSlp >> 5) & 0x7);

          // SATA
          FchTSSecondaryFchInitSataDxe ((UINT8)PhysicalDieId, (UINT8)SataIndex, NbioBusNum, FchDataPtr);
        }

        // Ethernet
        FchTSSecondaryFchInitXgbeDxe ((UINT8)PhysicalDieId, NbioBusNum, FchDataPtr);
      }
    }

    SataMultiDieDevSlp = SataMultiDieDevSlp >> 8;
  }

  //
  // Install gFchMultiFchInitProtocolGuid to signal Platform
  //
  Handle = ImageHandle;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gFchMultiFchInitProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &FchMfPrivate->FchMultiFchInit
                  );
  ASSERT (!EFI_ERROR (Status));
  //
  // Register the event handling function for FchInitLate to be launched after
  // Ready to Boot
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             MultiFchInitRtb,
             NULL,
             &FchMfPrivate->EventReadyToBoot
             );

  Status = gBS->FreePool (FchDataPtr);
  ASSERT (!EFI_ERROR (Status));

  HGPI_TESTPOINT (TpFchMultiFchDxeExit, NULL);
  return (Status);
}

VOID
FchTSSecondaryFchInitSataDxe (
  IN  UINT8       PhysicalDieId,
  IN  UINT8       SataIndex,
  IN  UINT8       NbioBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT8           PortNum;
  UINT32          DieBusNum32;
  UINT32          SataNbifStrap0;
  FCH_DATA_BLOCK  *LocalCfgPtr;

  DieBusNum32 = (UINT32)NbioBusNum;
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
FchTSSecondaryFchInitXgbeDxe (
  IN  UINT8       PhysicalDieId,
  IN  UINT8       NbioBusNum,
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
    IDS_HDT_CONSOLE (FCH_TRACE, "  Not execute FchTSSecondaryFchInitXgbeDxe in presilicon env \n");
    return;
  }
  
  DieBusNum32    = (UINT32)NbioBusNum;
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
MultiFchInitRtb (
  IN       EFI_EVENT        Event,
  IN       VOID             *Context
  )
{
  FCH_MULTI_FCH_INIT_PROTOCOL           *FchMfInit;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;
  FCH_INIT_PROTOCOL                     *FchInit;
  FCH_DATA_BLOCK                        *FchDataPtr;
  EFI_STATUS                            Status;
  UINT8                                 NbioBusNum;
  UINT32                                SataEnable32;
  UINT32                                SataSgpio32;
  UINT8                                 *SataMultiDiePortShutDown;
  UINT8                                 *SataMultiDiePortESP;
  UINT64                                SataMultiDieDevSlp;
  UINTN                                 NumberOfSockets;
  UINTN                                 NumberOfPhysicalDie;
  UINTN                                 SocketId;
  UINTN                                 PhysicalDieId;
  UINTN                                 LogicalDieId;
  UINTN                                 DieType;
  UINTN                                 NumberOfFch;
  UINT8                                 SataIndex;
  UINT8                                 SataEnDieOffset;
  UINT8                                 SataEnOffset;

  Status = gBS->LocateProtocol (
      &gHygonFabricTopologyServicesProtocolGuid,
      NULL,
      &FabricTopology
  );
  if(EFI_ERROR (Status)) {
    return;
  }

  FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL);
  NumberOfPhysicalDie = FabricTopologyGetNumberOfPhysicalDiesOnSocket (0);

  NumberOfFch = 0;
  for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
    if (NumberOfPhysicalDie == 1) {
        NumberOfFch++;  // Count Satori FCH
    } else {
      for (PhysicalDieId = 0; PhysicalDieId < NumberOfPhysicalDie; PhysicalDieId++) {
        FabricTopologyGetPhysicalIodDieInfo (PhysicalDieId, &LogicalDieId, &DieType);
        if (DieType == IOD_DUJIANG) {
          NumberOfFch++;  // Count Dujiang FCH
        }
      } 
    }
  }

  if (NumberOfFch < 2) {
    return;
  }

  //
  // Locate Fch INIT Protocol
  //
  Status = gBS->LocateProtocol (
                  &gFchInitProtocolGuid,
                  NULL,
                  &FchInit
                  );
  ASSERT (!EFI_ERROR (Status));

  Status = gBS->LocateProtocol (
                  &gFchMultiFchInitProtocolGuid,
                  NULL,
                  &FchMfInit
                  );
  ASSERT (!EFI_ERROR (Status));

  // Allocate memory for the local data
  Status = gBS->AllocatePool (
                  EfiRuntimeServicesData, // review for memory type
                  sizeof (FCH_DATA_BLOCK),
                  &FchDataPtr
                  );
  ASSERT (!EFI_ERROR (Status));

  gBS->CopyMem (
         FchDataPtr,
         FchInit->FchPolicy,
         sizeof (FCH_DATA_BLOCK)
         );

  //
  // Update local Data Structure
  //
  SataEnable32             = PcdGet32 (PcdSataEnableHyGx);
  SataSgpio32              = PcdGet32 (PcdSataSgpioHyGx);
  SataMultiDiePortShutDown = (UINT8 *)PcdGetPtr (PcdSataMultiDiePortShutDownHyGx);
  SataMultiDiePortESP      = (UINT8 *)PcdGetPtr (PcdSataMultiDiePortESPHyGx);
  SataMultiDieDevSlp       = PcdGet64 (PcdSataMultiDieDevSlpHyGx);

  for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
    for (PhysicalDieId = 0; PhysicalDieId < NumberOfPhysicalDie; PhysicalDieId++) {
      if ((SocketId == 0) && (PhysicalDieId == 0)) {
        continue;         // Bypass master dujiang
      }

      FabricTopologyGetPhysicalIodDieInfo (PhysicalDieId, &LogicalDieId, &DieType);
      if (DieType == IOD_DUJIANG) {
        SataEnDieOffset = (UINT8)((SocketId * MAX_SATA_NUMBER_PER_SOCKET) + (PhysicalDieId * SATA_NUMBER_PER_DUJIANG));
        // Update local Data Structure for each Die
        FchDataPtr->HwAcpi.FchAcpiMmioBase   = (UINT32)FchMfInit->FchMfData.FchAcpiMmioBase[SocketId][PhysicalDieId];
        FchDataPtr->Sata.SataMode.SataEnable = (UINT32)((SataEnable32 >> SataEnDieOffset) & 0x1);
        FchDataPtr->Sata.SataSgpio = (UINT32)((SataSgpio32 >> SataEnDieOffset) & 0x1);

        NbioBusNum = (UINT8)ReadSocDieBusNum (SocketId, LogicalDieId, 0);
        for (SataIndex = 0; SataIndex < SATA_NUMBER_PER_DUJIANG; SataIndex++) {
          SataEnOffset = SataEnDieOffset + SataIndex;

          FchDataPtr->Sata.SataPortPower.SataPortReg = SataMultiDiePortShutDown[SataEnOffset];
          FchDataPtr->Sata.SataEspPort.SataPortReg   = SataMultiDiePortESP[SataEnOffset];

          FchDataPtr->Sata.SataDevSlpPort0    = SataMultiDieDevSlp & BIT0;
          FchDataPtr->Sata.SataDevSlpPort0Num = (UINT8)((SataMultiDieDevSlp >> 1) & 0x7);
          FchDataPtr->Sata.SataDevSlpPort1    = SataMultiDieDevSlp & BIT4;
          FchDataPtr->Sata.SataDevSlpPort1Num = (UINT8)((SataMultiDieDevSlp >> 5) & 0x7);

          FchTSSecondaryFchInitSataRtb ((UINT8)PhysicalDieId, SataIndex, NbioBusNum, FchDataPtr);
        }
      }
    }

    SataMultiDieDevSlp = SataMultiDieDevSlp >> 8;
  }

  gBS->CloseEvent (Event);
}

VOID
FchTSSecondaryFchInitSataRtb (
  IN  UINT8       PhysicalDieId,
  IN  UINT8       SataIndex,
  IN  UINT8       NbioBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT32          DieBusNum32;
  UINT32          SataNbifStrap0;
  FCH_DATA_BLOCK  *LocalCfgPtr;

  DieBusNum32 = (UINT32)NbioBusNum;
  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;

  //
  // Check if Sata is enabled
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

  FchTSSataInitMMC (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);
  //
  // Set Sata PCI Configuration Space Write disable
  //
  SataDisableWriteAccessTS (PhysicalDieId, SataIndex, DieBusNum32, LocalCfgPtr);
}
