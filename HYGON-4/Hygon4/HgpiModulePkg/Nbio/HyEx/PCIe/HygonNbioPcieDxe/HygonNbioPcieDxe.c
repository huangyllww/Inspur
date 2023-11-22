/* $NoKeywords:$ */
/**
 * @file
 *
 * HygonNbioPcieDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioPcieDxe
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
#include <PiDxe.h>
#include <Filecode.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/HygonNbioPcieServicesProtocol.h>
#include <Protocol/HygonNbioPcieAerProtocol.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Guid/HobList.h>
#include <Protocol/PciIo.h>
#include <Library/PcieConfigLib.h>
#include <Library/PcieMiscCommLib.h>
#include <Library/NbioHandleLib.h>
#include <GnbRegisters.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/NbioSmuLib.h>
#include "HygonNbioPcieAer.h"
#include "HygonNbioPcieSris.h"
#include "HygonNbioPcieClkReq.h"
#include <Library/BaseFabricTopologyLib.h>

#define FILECODE        NBIO_PCIE_HYGONNBIOPCIE_HYEX_DXE_HYGONNBIOPCIESTDXE_FILECODE

extern HYGON_NBIO_PCIE_AER_PROTOCOL mHygonNbioPcieAerProtocol;
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

VOID
NbioRASControl (
  IN GNB_HANDLE                 *GnbHandle
  );

EFI_STATUS
HygonGetGnbhandleBySystemLogicalRbId (
  IN  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL  *This,
  IN  UINTN                                SystemLogicalRbId,
  OUT GNB_HANDLE                           **MyHandle
  );

EFI_STATUS
HygonPcieGetTopology (
  IN       DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL  *This,
     OUT   UINT32                               **DebugOptions
  )
{
  EFI_STATUS                           Status;
  GNB_PCIE_INFORMATION_DATA_HOB        *GnbPcieInfoDataHob;
  //
  // Get information data from HOB.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &GnbPcieInfoDataHob);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  GnbPcieInfoDataHob = GetNextGuidHob (&gGnbPcieHobInfoGuid, GnbPcieInfoDataHob);
  if (GnbPcieInfoDataHob == NULL) {
    return EFI_NOT_FOUND;
  }

  *DebugOptions = (UINT32*) GnbPcieInfoDataHob;
  return Status;
}

DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL mDxeHygonNbioPcieServicesProtocol = {
  HygonPcieGetTopology,
  HygonGetGnbhandleBySystemLogicalRbId
};

/*----------------------------------------------------------------------------------------*/
/**
 * Clock gating
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 */

VOID
PciePwrClockGating (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_WRAPPER_CONFIG            *Wrapper;
  CPM_CONTROL_STRUCT             CpmCntl;
  PCIE_CONFIG_CNTL_STRUCT        ConfigCntl;
  PCIE_SDP_CTRL_STRUCT           SdpCntl;
  UINT32                         Address0;
  UINT32                         Address1;
  UINT32                         Address2;

  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrClockGating Enter\n");

  Wrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (Wrapper != NULL) {
    Address0 = ConvertPcieCoreAddress(CPM_CONTROL_PCIECORE0_ADDRESS_HYEX, GnbHandle, Wrapper);  
    Address1 = ConvertPcieCoreAddress(PCIE_CONFIG_CNTL_PCIECORE0_ADDRESS_HYEX, GnbHandle, Wrapper);
    Address2 = ConvertPcieCoreAddress(PCIE_SDP_CTRL_PCIECORE0_ADDRESS_HYEX, GnbHandle, Wrapper);

    NbioRegisterRead ( GnbHandle, CPM_CONTROL_TYPE, Address0, &CpmCntl.Value, 0);
    NbioRegisterRead ( GnbHandle, PCIE_CONFIG_CNTL_TYPE, Address1, &ConfigCntl.Value, 0);
    NbioRegisterRead ( GnbHandle, PCIE_SDP_CTRL_TYPE, Address2, &SdpCntl.Value, 0);

    if (Wrapper->Features.LclkGating == 1) {
      CpmCntl.Field.LCLK_DYN_GATE_ENABLE = 0x1;
      CpmCntl.Field.LCLK_GATE_TXCLK_FREE = 0x0;
      CpmCntl.Field.LCLK_DYN_GATE_LATENCY = 0x1;
    }
    // TXCLK+REFCLK
    if (Wrapper->Features.ClkGating == 1) {
      // TXCLK_PERM gating enable
      CpmCntl.Field.TXCLK_PERM_GATE_ENABLE = 0x1;
      CpmCntl.Field.TXCLK_PERM_GATE_LATENCY = 0x0;
      CpmCntl.Field.RCVR_DET_CLK_ENABLE = 0x1;
      ConfigCntl.Field.DYN_CLK_LATENCY = 0xf;

      // TXCLK DYN gating enable
      CpmCntl.Field.TXCLK_DYN_GATE_ENABLE = 0x1;
      CpmCntl.Field.TXCLK_DYN_GATE_LATENCY = 0x0;

      // TXCLK REGS gating enable
      CpmCntl.Field.TXCLK_REGS_GATE_ENABLE = 0x1;
      CpmCntl.Field.TXCLK_REGS_GATE_LATENCY = 0x0;

      // TXCLK PRBS gating enable
      CpmCntl.Field.TXCLK_PRBS_GATE_ENABLE = 0x1;

      // TXCLK LCNT gating enable
      CpmCntl.Field.TXCLK_LCNT_GATE_ENABLE = 0x1;

      // TXCLK PIF1X gating enable
      // PcieDir460.Field.TXCLK_PIF_GATE_ENABLE = 0x1;

      // REFCLK REGS gating enable
      CpmCntl.Field.REFCLK_REGS_GATE_ENABLE = 0x1;
      CpmCntl.Field.REFCLK_REGS_GATE_LATENCY = 0x0;

      SdpCntl.Field.CI_SLV_REQR_PART_DISCONNECT_EN = 0x1;
      SdpCntl.Field.CI_MSTSDP_CLKGATE_ONESIDED_ENABLE = 0x1;
    }
    CpmCntl.Field.CLKREQb_UNGATE_TXCLK_ENABLE = 0x1;
    NbioRegisterWrite (GnbHandle, CPM_CONTROL_TYPE, Address0, &CpmCntl.Value, GNB_REG_ACC_FLAG_S3SAVE);
    NbioRegisterWrite (GnbHandle, PCIE_CONFIG_CNTL_TYPE, Address1, &ConfigCntl.Value, GNB_REG_ACC_FLAG_S3SAVE);
    NbioRegisterWrite (GnbHandle, PCIE_SDP_CTRL_TYPE, Address2, &SdpCntl.Value, GNB_REG_ACC_FLAG_S3SAVE);

    Wrapper = PcieLibGetNextDescriptor (Wrapper);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrClockGating Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Lock registers
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 */

VOID
PcieLockRegisters (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  NbioRegisterRMW (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCIE0_CNTL_ADDRESS_HYEX), (UINT32) ~(BIT0), BIT0, GNB_REG_ACC_FLAG_S3SAVE);
  NbioRegisterRMW (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCIE1_CNTL_ADDRESS_HYEX), (UINT32) ~(BIT0), BIT0, GNB_REG_ACC_FLAG_S3SAVE);
  NbioRegisterRMW (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCIE2_CNTL_ADDRESS_HYEX), (UINT32) ~(BIT0), BIT0, GNB_REG_ACC_FLAG_S3SAVE);
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  HygonPcieMiscInit
 *
 *  Description:
 *     Calls HygonInitMid as part of Hgpi DXE Driver Initialization.
 *
 *  Parameters:
 *    @param[in]     Event
 *    @param[in]     *Context
 *
 *    @retval         VOID
 *
 *---------------------------------------------------------------------------------------
 **/

VOID
EFIAPI
HygonPcieMiscInit (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                          Status;
  PCIe_PLATFORM_CONFIG                *Pcie;
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB       *PciePlatformConfigHobData;
  GNB_HANDLE                          *GnbHandle;

  HGPI_TESTPOINT (TpHygonPcieMiscInitEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonPcieMiscInit Entry\n");
  Status = gBS->LocateProtocol (
                  &gHygonNbioPcieServicesProtocolGuid,
                  NULL,
                  &PcieServicesProtocol
                  );

  if (Status == EFI_SUCCESS) {
    PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **) &PciePlatformConfigHobData);
    Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      // Slot power limit
      PcieSlotPowerLimitInterface (GnbHandle);
      // Enabling PCIe Core Power Saving Features
      PciePwrClockGating (GnbHandle);
      // Initialize AER
      if (PcdGet8 (PcdRunEnvironment) != 2) {
          NbioPcieAerInitST (GnbHandle);
      }
      // configure MaxReadRequestSize on PCIE interface
      PcieMaxReadRequestInterface (GnbHandle);
      // configure MaxPayloadSize on PCIE interface
      PcieMaxPayloadInterface (GnbHandle);
      // configure CI_MAX_CPL_PAYLOAD_SIZE on PCIE core
      PcieMaxCplPayloadInterface (GnbHandle);
      // enable Clock Power Managment
      PcieClkPmInterface (GnbHandle);

      PcieSrisInit (GnbHandle);
      PcieAspmL1SSInterface (GnbHandle);
      PcieClkReqInterface (GnbHandle);
      PcieAspmInterface (GnbHandle);
      // program common clock configuration
      PcieCommClkCfgInterface (GnbHandle);
      // Enable ARI
      if (PcdGetBool (PcdHygonNbioRASControl)) {
        NbioRASControl (GnbHandle);
      }
      PcieSerrInterface (GnbHandle);
      PcieExtendedSync (GnbHandle);
      PcieLockRegisters (GnbHandle);
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
  }

  gBS->CloseEvent (Event);
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonPcieMiscInit Exit\n");
  HGPI_TESTPOINT (TpHygonPcieMiscInitExit, NULL);

  return;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  PcieRegCfgReadyToBootCallback
 *
 *  Description:
 *     PCIe Register configuration after BIOS boot.
 *
 *  Parameters:
 *    @param[in]     Event
 *    @param[in]     *Context
 *
 *    @retval         VOID
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
EFIAPI
PcieRegCfgReadyToBootCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                            Status;
  PCIe_PLATFORM_CONFIG                  *Pcie;
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB         *PciePlatformConfigHobData;
  GNB_HANDLE                            *GnbHandle;
  PCIe_ENGINE_CONFIG                    *PcieEngine;
  PCIe_WRAPPER_CONFIG                   *PcieWrapper;
  UINT32                                SmnAddress;
  
  Status = gBS->LocateProtocol (
                  &gHygonNbioPcieServicesProtocolGuid,
                  NULL,
                  &PcieServicesProtocol
                  );
				  
  if (Status == EFI_SUCCESS) {
    PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **) &PciePlatformConfigHobData);
    Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
      while (PcieWrapper != NULL) {
        PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
        while (PcieEngine != NULL) {
          SmnAddress = ConvertPcieCoreAddress ( PCIE0_PCIE_HW_DEBUG_ADDRESS_HYEX, GnbHandle, PcieWrapper);
          NbioRegisterRMW (GnbHandle, TYPE_SMN, SmnAddress, (UINT32)(~BIT13), BIT13, 0);
          PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
        }
        PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
      }
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
  }
  
  gBS->CloseEvent (Event);
  
  return;
}

EFI_STATUS
EFIAPI
HygonNbioPcieHyExDxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status;
  EFI_HANDLE          Handle;
  EFI_EVENT           PciIoEvent;
  EFI_EVENT           ReadyToBootEvent;
  VOID                *Registration;

  HGPI_TESTPOINT (TpNbioPCIeDxeEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioPcieHyExDxeEntry Entry\n");

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonNbioPcieServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mDxeHygonNbioPcieServicesProtocol
                  );

  //
  // produce HYGON_NBIO_PCIE_AER_PROTOCOL
  //
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonNbioPcieAerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mHygonNbioPcieAerProtocol
                  );

  //
  // Register the event handling function for HygonPcieMiscInit
  //
  Status = gBS->CreateEventEx (
             EVT_NOTIFY_SIGNAL,
             TPL_NOTIFY,
             HygonPcieMiscInit,
             NULL,
             NULL,
             &PciIoEvent
             );

  Status = gBS->RegisterProtocolNotify (
              &gEfiPciIoProtocolGuid,
              PciIoEvent,
              &Registration
              );

  //
  // Register the event handling function for Enable PCIe receiver error
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             PcieRegCfgReadyToBootCallback,
             NULL,
             &ReadyToBootEvent
             );
			   
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioPcieHyExDxeEntry Exit\n");
  HGPI_TESTPOINT (TpNbioPCIeDxeExit, NULL);
  return EFI_SUCCESS;
}

EFI_STATUS
HygonGetGnbhandleBySystemLogicalRbId (
  IN  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL  *This,
  IN  UINTN                                SystemLogicalRbId,
  OUT GNB_HANDLE                           **MyHandle
  )
{
  PCIe_PLATFORM_CONFIG           *Pcie;
  GNB_PCIE_INFORMATION_DATA_HOB  *PciePlatformConfigHobData;
  GNB_HANDLE                     *GnbHandle;
  UINTN                          SocketNumber;
  UINTN                          RbNumber;
  UINTN                          RbNumberPerIoDie;
  UINTN                          SocketId;
  UINTN                          SocketRbLogicalId;
  UINTN                          RbNumberOfPerSocket;

  SocketNumber     = FabricTopologyGetNumberOfSocketPresent ();
  RbNumber         = FabricTopologyGetNumberOfSystemRootBridges ();
  RbNumberPerIoDie = FabricTopologyGetNumberOfRootBridgesOnDie (0, 0);

  RbNumberOfPerSocket = RbNumber / SocketNumber;
  SocketId = SystemLogicalRbId / RbNumberOfPerSocket;
  SocketRbLogicalId = SystemLogicalRbId % RbNumberOfPerSocket;

  HygonPcieGetTopology (This, (UINT32 **)&PciePlatformConfigHobData);
  Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    if (GnbHandle->SocketId == SocketId) {
      if (SocketRbLogicalId == (GnbHandle->LogicalDieId * RbNumberPerIoDie + GnbHandle->RbId)) {
        IDS_HDT_CONSOLE (
          GNB_TRACE,
          "Get IohubGnbhandle: Socket %d Die PhysicalDie %d LogicalDie %d Rb %d\n",
          GnbHandle->SocketId,
          GnbHandle->PhysicalDieId,
          GnbHandle->LogicalDieId,
          GnbHandle->RbId
          );
        *MyHandle = GnbHandle;
        return EFI_SUCCESS;
      }
    }

    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  return EFI_NOT_FOUND;
}