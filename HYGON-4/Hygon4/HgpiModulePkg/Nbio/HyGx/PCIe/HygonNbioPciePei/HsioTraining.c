/* $NoKeywords:$ */

/**
 * @file
 *
 * HSIO initialization services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: GNB
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
#include <Filecode.h>
#include <PiPei.h>
#include <HygonPcieComplex.h>
#include <GnbHsio.h>
#include <Library/BaseLib.h>
#include <Library/IdsLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/GnbLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/OemGpioResetControlLib.h>
#include <Library/SmnAccessLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseCoreLogicalIdLib.h>
#include <Library/IoLib.h>
#include <Library/GnbPciLib.h>
#include <Library/GnbInternalPciLib.h>
#include <Hsio.h>
#include "HsioTraining.h"
#include <TSFch.h>
#include <GnbRegisters.h>
#include <Library/HsioServiceLib.h>
#include "HsioPcieEq.h"
#include <Library/HygonSocBaseLib.h>
#include <Library/HygonSmnAddressLib.h>

#define FILECODE LIBRARY_HSIOLIB_HSIOLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
HsioConfigurationProgramAfterReconfig (
  IN  GNB_HANDLE            *GnbHandle
  );

VOID
HsioConfigurationProgramAfterReconfigHyGx (
  IN  GNB_HANDLE            *GnbHandle
  );

VOID
ReleasePcieReset (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  UINT32 GpioHigh = 0xC00000;
  UINT8  IoMuxSelect;

  IoMuxSelect = 1;
  IDS_HDT_CONSOLE (GNB_TRACE, "Release PCIe Reset For IOD:0x%d\n", GnbHandle->GlobalIodId);
  IDS_HDT_CONSOLE (GNB_TRACE, "Set GPIO 26 to High\n", GnbHandle->GlobalIodId);
  SmnRegisterWrite8 (GnbHandle->Address.Address.Bus, IOD_SPACE(GnbHandle->PhysicalDieId, (FCH_SMN_IOMUX_BASE_HYGX + FCH_TS_IOMUX_GPIO26)), &IoMuxSelect, 0);  //IoMux function1 
  SmnRegisterWrite32 (GnbHandle->Address.Address.Bus, IOD_SPACE(GnbHandle->PhysicalDieId, (FCH_SMN_GPIO_BASE_HYGX + FCH_TS_CONTROL_GPIO26)), &GpioHigh, 0);     //Set GPIO26 High
  IDS_HDT_CONSOLE (GNB_TRACE, "Set GPIO 27 to High\n", GnbHandle->GlobalIodId);
  SmnRegisterWrite8 (GnbHandle->Address.Address.Bus, IOD_SPACE(GnbHandle->PhysicalDieId, (FCH_SMN_IOMUX_BASE_HYGX + FCH_TS_IOMUX_GPIO27)), &IoMuxSelect, 0);  //IoMux function1 
  SmnRegisterWrite32 (GnbHandle->Address.Address.Bus, IOD_SPACE(GnbHandle->PhysicalDieId, (FCH_SMN_GPIO_BASE_HYGX + FCH_TS_CONTROL_GPIO27)), &GpioHigh, 0);     //Set GPIO27 High
  IDS_HDT_CONSOLE (GNB_TRACE, "Set GPIO 266 to High\n", GnbHandle->GlobalIodId);
  SmnRegisterWrite8 (GnbHandle->Address.Address.Bus, IOD_SPACE(GnbHandle->PhysicalDieId, (FCH_SMN_IOMUX_BASE_HYGX + FCH_TS_IOMUX_GPIO266)), &IoMuxSelect, 0);  //IoMux function1 
  SmnRegisterWrite32 (GnbHandle->Address.Address.Bus, IOD_SPACE(GnbHandle->PhysicalDieId, (FCH_SMN_REMOTE_GPIO_BASE_HYGX + FCH_TS_CONTROL_GPIO266)), &GpioHigh, 0);
  IDS_HDT_CONSOLE (GNB_TRACE, "Set GPIO 267 to High\n", GnbHandle->GlobalIodId);
  SmnRegisterWrite8 (GnbHandle->Address.Address.Bus, IOD_SPACE(GnbHandle->PhysicalDieId, (FCH_SMN_IOMUX_BASE_HYGX + FCH_TS_IOMUX_GPIO267)), &IoMuxSelect, 0);  //IoMux function1 
  SmnRegisterWrite32 (GnbHandle->Address.Address.Bus, IOD_SPACE(GnbHandle->PhysicalDieId, (FCH_SMN_REMOTE_GPIO_BASE_HYGX + FCH_TS_CONTROL_GPIO267)), &GpioHigh, 0);
}

/*----------------------------------------------------------------------------------------*/

/**
 * Helper function to dump input configuration to user engine descriptor
 *
 *
 * @param[in]  EngineDescriptor   Pointer to engine descriptor
 */
VOID
HsioEntryConfigDump (
  IN       HSIO_ENGINE_DESCRIPTOR    *EngineDescriptor
  )
{
  UINT8  *Buffer;

  Buffer = (UINT8 *)EngineDescriptor;
  // GnbLibDebugDumpBuffer ((VOID*) EngineDescriptor, sizeof(HSIO_ENGINE_DESCRIPTOR), 1, 16);
  IDS_HDT_CONSOLE (PCIE_MISC, "\n");

  IDS_HDT_CONSOLE (
    PCIE_MISC,
    "  Engine Type - %a\n",
    (EngineDescriptor->EngineType == HsioPcieEngine) ? "PCIe Port" : (
    (EngineDescriptor->EngineType == HsioSATAEngine) ? "SATA Port" : (
    (EngineDescriptor->EngineType == HsioEthernetEngine) ? "Ethernet Port" : (
    (EngineDescriptor->EngineType == HsioCxlEngine) ? "Cxl Port" : (
    (EngineDescriptor->EngineType == HsioUnusedEngine) ? "Unused" : "Invalid"))))
    );
  IDS_HDT_CONSOLE (
    PCIE_MISC,
    "    Start Phy Lane - %d\n    End   Phy Lane - %d\n",
    EngineDescriptor->StartLane,
    EngineDescriptor->EndLane
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "    Hotplug - %d\n", EngineDescriptor->HotPluggable);
  IDS_HDT_CONSOLE (PCIE_MISC, "    GpioGroupId - %d\n", EngineDescriptor->GpioGroupID);
  if (EngineDescriptor->EngineType == PciePortEngine) {
    IDS_HDT_CONSOLE (
      PCIE_MISC,
      "    PortPresent - %d\n    maxLinkSpeedCap - %d\n",
      EngineDescriptor->LinkDesc.Pcie.Capabilities.portPresent,
      EngineDescriptor->LinkDesc.Pcie.Capabilities.maxLinkSpeedCap
      );
    // IDS_HDT_CONSOLE (PCIE_MISC, "    TrainingState - %d\n", EngineDescriptor->TrainingState);
    IDS_HDT_CONSOLE (PCIE_MISC, "    MacPortID - %d\n", EngineDescriptor->LinkDesc.Pcie.MacPortID);
  }
  if (EngineDescriptor->EngineType == HsioCxlEngine) {
    IDS_HDT_CONSOLE (
      PCIE_MISC,
      "    PortPresent - %d\n    maxLinkSpeedCap - %d\n",
      EngineDescriptor->LinkDesc.Pcie.Capabilities.portPresent,
      EngineDescriptor->LinkDesc.Pcie.Capabilities.maxLinkSpeedCap
      );
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Helper function to dump the data being sent to HSIO
 *
 *
 * @param[in]  PlatformDescriptor   Pointer to platform descriptor
 * @param[in]  PlatformDescSize     Size of platform descriptor
 * @param[in]  AncillaryData        Pointer to platform descriptor
 * @param[in]  AncillaryDataSize    Size of platform descriptor
 */
VOID
HsioDebugDump (
  IN       HSIO_PLATFORM_DESCRIPTOR  *PlatformDescriptor,
  IN       UINT32                    PlatformDescSize,
  IN       UINT8                     *AncillaryData,
  IN       UINT32                    AncillaryDataSize
  )
{
  HSIO_PLATFORM_DESCRIPTOR  *PlatformPointer;
  HSIO_ENGINE_DESCRIPTOR    *EnginePointer;
  UINT32                    EngineCount;
  UINT32                    Index;

  PlatformPointer = (HSIO_PLATFORM_DESCRIPTOR *)PlatformDescriptor;
  EngineCount     = PlatformPointer->NumEngineDesc;
  IDS_HDT_CONSOLE (GNB_TRACE, "Number of Engines is %d\n", EngineCount);
  PlatformPointer++;
  EnginePointer = (HSIO_ENGINE_DESCRIPTOR *)PlatformPointer;
  for (Index = 0; Index < EngineCount; Index++) {
    HsioEntryConfigDump (EnginePointer);
    EnginePointer++;
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "Sending this Platform Descriptor to HSIO...\n");
  GnbLibDebugDumpBuffer ((VOID *)PlatformDescriptor, PlatformDescSize << 2, 1, 16);
  IDS_HDT_CONSOLE (GNB_TRACE, "\n");
  if (AncillaryDataSize != 0) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Sending this Ancillary Data to HSIO...\n");
    GnbLibDebugDumpBuffer ((VOID *)AncillaryData, AncillaryDataSize << 2, 3, 8);
    IDS_HDT_CONSOLE (GNB_TRACE, "\n");
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "No ancillary data to send\n");
  }

  return;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HSIO Update Port Training Status
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @param[in]  ThisNode       Pointer to the platform descriptor for this node
 * @retval     HGPI_STATUS
 */
VOID
HsioUpdatePortTrainingStatus (
  IN      GNB_HANDLE                      *GnbHandle,
  IN      HSIO_COMPLEX_DESCRIPTOR         *UserConfig,
  IN OUT  HSIO_PLATFORM_DESCRIPTOR        *ThisNode
  )
{
  // HSIO Firmware Structures
  HSIO_PLATFORM_DESCRIPTOR  *HsioNodePlatformDesc;
  HSIO_ENGINE_DESCRIPTOR    *HsioEnginePointer;
  UINT32                    HsioNodeCount;
  // User Config Pointers
  HSIO_COMPLEX_DESCRIPTOR  *UserCfg;
  HSIO_PORT_DESCRIPTOR     *UserCfgPortList;               // Pointer to start of User Config for this socket
  HSIO_PORT_DESCRIPTOR     *UserCfgPortPointer;            // Updatable pointer for UserCfgPortList processing
  // PCIe Structures
  PCIe_ENGINE_CONFIG       *PcieEngineList;
  PCIe_WRAPPER_CONFIG      *PcieWrapper;
  PCIE_LC_STATE0_STRUCT    LcState0;
  UINT8                    SocketId;
  BOOLEAN                  EndOfList;
  UINT32                   Index;
  BOOLEAN                  TrainingSuccess;
  UINT32                   SmnAddress;
  GNB_HANDLE               *LocalHandle;
  HGPI_STATUS              HgpiStatus;
  BOOLEAN                  TrainingState = FALSE;
  UINT32                   TimeOut;
  BOOLEAN                  RequestRedoEq = FALSE;
  BOOLEAN                  RedoEq = FALSE;

  IDS_HDT_CONSOLE (PCIE_MISC, "HsioUpdatePortTrainingStatus enter\n");

  // Get Socket Number
  SocketId = GnbHandle->SocketId;
  UserCfg  = UserConfig;
  UserCfgPortList = NULL;
  HgpiStatus = HGPI_SUCCESS;
  HgpiStatus = HsioGetUserCfgForSocket (SocketId, UserCfg, &UserCfgPortList);
  if (HgpiStatus != HGPI_SUCCESS) {
    IDS_HDT_CONSOLE (PCIE_MISC, "No user configure port list for socket %d \n", SocketId);
    return;
  }

  HsioNodePlatformDesc = ThisNode;
  HsioNodePlatformDesc++;                                               // Point to first engine descriptor
  HsioEnginePointer = (HSIO_ENGINE_DESCRIPTOR *)HsioNodePlatformDesc;   // Initialize engine pointer to first engine
  HsioNodePlatformDesc--;                                               // Restore HsioNodePlatformDesc address

  HsioNodeCount = HsioNodePlatformDesc->NumEngineDesc;
  for (Index = 0; Index < HsioNodeCount; Index++) {
    if (HsioEnginePointer->EngineType == HsioPcieEngine) {
      // HsioEntryConfigDump (HsioEnginePointer);
      // Find the UserConfig entry for this HSIO engine
      UserCfgPortPointer = UserCfgPortList;       // Start of the port list for this socket
      EndOfList = FALSE;
      while (EndOfList != TRUE) {
        // IDS_HDT_CONSOLE (PCIE_MISC, "This port starts at lane %d and ends at lane %d\n", UserCfgPortPointer->EngineData.StartLane,  UserCfgPortPointer->EngineData.EndLane);
        if ((UserCfgPortPointer->EngineData.StartLane >= (HsioEnginePointer->StartLane + GnbHandle->IodStartLane)) &&
            (UserCfgPortPointer->EngineData.EndLane <= (HsioEnginePointer->EndLane + GnbHandle->IodStartLane)) &&
            (UserCfgPortPointer->EngineData.EngineType == HsioPcieEngine)) {
          EndOfList = TRUE;
        } else {
          if ((UserCfgPortPointer->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
            UserCfgPortPointer++;
          } else {
            EndOfList = TRUE;
            ASSERT (FALSE);           // Something bad happened here @todo - handle error
          }
        }
      }

      // Find the PCIe wrapper for this HSIO engine
      LocalHandle = GnbHandle;
      while (LocalHandle != NULL) {
        if (LocalHandle->SocketId == SocketId) {
          PcieWrapper = PcieConfigGetChildWrapper (LocalHandle);
          EndOfList   = FALSE;
          HsioEnginePointer->LinkDesc.Pcie.MacPortID = HsioEnginePointer->LinkDesc.Pcie.MacPortID % 8;
          while (EndOfList != TRUE) {
            // IDS_HDT_CONSOLE (PCIE_MISC, "Wrapper StartLane = %d, EndLane = %d\n",PcieWrapper->StartPhyLane, PcieWrapper->EndPhyLane);
            if ((UserCfgPortPointer->EngineData.StartLane >= PcieWrapper->StartPhyLane) &&
                (UserCfgPortPointer->EngineData.EndLane <= PcieWrapper->EndPhyLane)) {
              EndOfList = TRUE;
              HsioEnginePointer->LinkDesc.Pcie.MacPortID = PcieWrapper->WrapId * 8 + HsioEnginePointer->LinkDesc.Pcie.MacPortID;
            } else {
              PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
              //HsioEnginePointer->LinkDesc.Pcie.MacPortID += 8;              /// @todo this should be port offset defined in the wrapper
              if (PcieWrapper == NULL) {
                // ASSERT (FALSE);       // Something bad happened here @todo - handle error
                break;
              }
            }
          }

          // Find the PCIe structure entry for this HSIO engine
          if (PcieWrapper != NULL) {
            PcieEngineList = PcieConfigGetChildEngine (PcieWrapper);
            EndOfList = FALSE;
            while (EndOfList != TRUE) {
              // IDS_HDT_CONSOLE (PCIE_MISC, "This PortId = %d\n", PcieEngineList->Type.Port.PortId);
              if (PcieEngineList->Type.Port.PortId == HsioEnginePointer->LinkDesc.Pcie.MacPortID) {
                EndOfList = TRUE;
              } else {
                PcieEngineList = PcieLibGetNextDescriptor (PcieEngineList);
                if (PcieEngineList == NULL) {
                  ASSERT (FALSE);                   // Something bad happened here @todo - handle error
                }
              }
            }

            // At this point we have aligned structures...
            // PcieEngineList points to the engine in the Pcie platform structure.
            // UserCfgPortPointer points to the port in UserConfig
            // HsioEnginePointer points to the engine structure in the HSIO structure
            // We can use this data to update the status of the engine in the Pcie platform
            if (PcieEngineList != NULL) {
              TrainingSuccess = FALSE;
              LcState0.Value = 0;
              IDS_HDT_CONSOLE (PCIE_MISC, "Update PCIe Training Status - <Socket%d Die%d NBIO%d Core%d Port%d>:\n",
                LocalHandle->SocketId,
                LocalHandle->PhysicalDieId,
                LocalHandle->RbId,
                PcieWrapper->WrapId,
                (PcieEngineList->Type.Port.PortId % 8)
                );

              //Get Training status
              SmnAddress = ConvertPciePortAddress (
                             PCIE0_LC_STATE0_ADDRESS_HYGX,
                             LocalHandle,
                             PcieWrapper,
                             (PcieEngineList->Type.Port.PortId % 8)
                             );

              TimeOut = 100;//100ms timeout
              do {
                NbioRegisterRead (LocalHandle, TYPE_SMN, SmnAddress, &LcState0.Value, 0);
                TimeOut--;
                MicroSecondDelay(1000);//delay 1ms
              } while ((LcState0.Field.LC_CURRENT_STATE < NO_RCVR_LOOP) && (TimeOut != 0));

              //Dump LTSSM
              DumpLtssm (LocalHandle, PcieEngineList);

              if ((LcState0.Field.LC_CURRENT_STATE < NO_RCVR_LOOP) || (HsioEnginePointer->LinkDesc.Pcie.Capabilities.portPresent == 0)) {
                PcieEngineList->InitStatus = INIT_STATUS_PCIE_EP_NOT_PRESENT;
                IDS_HDT_CONSOLE (
                  PCIE_MISC,
                  "[B%x|D%x|F%x]: Detect Failed\n",
                  LocalHandle->Address.Address.Bus,
                  PcieEngineList->Type.Port.PortData.DeviceNumber,
                  PcieEngineList->Type.Port.PortData.FunctionNumber
                  );
              } else {
                IDS_HDT_CONSOLE (
                  PCIE_MISC,
                  "[B%x|D%x|F%x]: Detect Success\n",
                  LocalHandle->Address.Address.Bus,
                  PcieEngineList->Type.Port.PortData.DeviceNumber,
                  PcieEngineList->Type.Port.PortData.FunctionNumber
                  );

                if (PcdGet8 (PcdPcieLinkTrainingType) == 1) {
                  //Training two step
                  HsioSetOperatingSpeed (
                    LocalHandle,
                    PcieEngineList,
                    HsioEnginePointer,
                    UserCfgPortPointer
                    );
                }
                
                //Wait for MAX 500ms to DL Activate
                TrainingState = PcieWaitToL0 (LocalHandle, PcieEngineList, 500);

                if (TrainingState == FALSE) {
                  PcieEngineList->InitStatus = INIT_STATUS_PCIE_PORT_TRAINING_FAIL;
                  IDS_HDT_CONSOLE (
                    PCIE_MISC,
                    "[B%x|D%x|F%x]: Training Failed\n",
                    LocalHandle->Address.Address.Bus,
                    PcieEngineList->Type.Port.PortData.DeviceNumber,
                    PcieEngineList->Type.Port.PortData.FunctionNumber
                    );
                } else {
                  PcieEngineList->InitStatus = INIT_STATUS_PCIE_TRAINING_SUCCESS;
                  IDS_HDT_CONSOLE (
                    PCIE_MISC,
                    "[B%x|D%x|F%x] - Training Success\n",
                    LocalHandle->Address.Address.Bus,
                    PcieEngineList->Type.Port.PortData.DeviceNumber,
                    PcieEngineList->Type.Port.PortData.FunctionNumber
                    );
                  IDS_HDT_CONSOLE (
                    PCIE_MISC,
                    "[B%x|D%x|F%x]: Current Link Speed:%d - Cap Link Speed:%d\n",
                    LocalHandle->Address.Address.Bus,
                    PcieEngineList->Type.Port.PortData.DeviceNumber,
                    PcieEngineList->Type.Port.PortData.FunctionNumber,
                    PcieCurrentLinkSpeed (LocalHandle, PcieEngineList),
                    PcieCapLinkSpeed (LocalHandle, PcieEngineList)
                    );
                  IDS_HDT_CONSOLE (
                    PCIE_MISC,
                    "[B%x|D%x|F%x]: Current Link Width:%d - Cap Link Width:%d\n",
                    LocalHandle->Address.Address.Bus,
                    PcieEngineList->Type.Port.PortData.DeviceNumber,
                    PcieEngineList->Type.Port.PortData.FunctionNumber,
                    PcieCurrentLinkWidth (LocalHandle, PcieEngineList),
                    PcieCapLinkWidth (LocalHandle, PcieEngineList)
                    );
                  if (PcieLinkSpeedWarning (LocalHandle, PcieEngineList)) {
                    IDS_HDT_CONSOLE (
                      PCIE_MISC,
                      "[B%x|D%x|F%x] - Link Speed warning\n",
                      LocalHandle->Address.Address.Bus,
                      PcieEngineList->Type.Port.PortData.DeviceNumber,
                      PcieEngineList->Type.Port.PortData.FunctionNumber
                      );                    
                  }
                }

                CommonPcieEqCfgAfterTraining (LocalHandle, PcieEngineList, &RequestRedoEq);
                RedoEq |= RequestRedoEq;

                CustomerPcieEqCfgAfterTraining (LocalHandle, PcieEngineList, &RequestRedoEq); 
                RedoEq |= RequestRedoEq;
                
                if (RedoEq) {
                  PcieRetraining (LocalHandle, PcieEngineList, TRUE);
                }
              }
            }
          }
        }

        LocalHandle = GnbGetNextHandle (LocalHandle);
      }
    }

    HsioEnginePointer++;
  }

  return;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HSIO Update Pcie Data Structure
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @param[in]  ThisNode       Pointer to the platform descriptor for this node
 * @retval     HGPI_STATUS
 */
VOID
HsioUpdatePcieStatus (
  IN      GNB_HANDLE                *GnbHandle,
  IN      HSIO_COMPLEX_DESCRIPTOR   *UserConfig,
  IN      HSIO_PLATFORM_DESCRIPTOR  *ThisNode,
  IN OUT  HSIO_ENGINE_DESCRIPTOR    **EarlyTrain,
  IN OUT  UINT32                    *EarlyTrainAddress,
  IN OUT  PCIe_ENGINE_CONFIG        **EarlyTrainEngine

  )
{
  // HSIO Firmware Structures
  HSIO_PLATFORM_DESCRIPTOR  *HsioNodePlatformDesc;
  HSIO_ENGINE_DESCRIPTOR    *HsioEnginePointer;
  UINT32                    HsioNodeCount;
  // User Config Pointers
  HSIO_COMPLEX_DESCRIPTOR  *UserCfg;
  HSIO_PORT_DESCRIPTOR     *UserCfgPortList;               // Pointer to start of User Config for this socket
  HSIO_PORT_DESCRIPTOR     *UserCfgPortPointer;            // Updatable pointer for UserCfgPortList processing
  // PCIe Structures
  PCIe_ENGINE_CONFIG   *PcieEngineList;
  PCIe_WRAPPER_CONFIG  *PcieWrapper;

  UINT8        SocketId;
  BOOLEAN      EndOfList;
  UINT32       Index;
  UINT8        EngineId;
  UINT8        PortDevMap[4][20];
  GNB_HANDLE   *LocalHandle;
  HGPI_STATUS  HgpiStatus;

  IDS_HDT_CONSOLE (PCIE_MISC, "HsioUpdatePcieStatus enter\n");

  LibHygonMemFill (PortDevMap, 0, sizeof (PortDevMap), (HYGON_CONFIG_PARAMS *)NULL);

  // Get Socket Number
  SocketId = GnbHandle->SocketId;
  UserCfg  = UserConfig;
  UserCfgPortList = NULL;
  HgpiStatus = HGPI_SUCCESS;
  HgpiStatus = HsioGetUserCfgForSocket (SocketId, UserCfg, &UserCfgPortList);
  if (HgpiStatus != HGPI_SUCCESS) {
    IDS_HDT_CONSOLE (PCIE_MISC, "No user configure port list for socket %d \n", SocketId);
    return;
  }

  HsioNodePlatformDesc = ThisNode;
  HsioNodePlatformDesc++;                                               // Point to first engine descriptor
  HsioEnginePointer = (HSIO_ENGINE_DESCRIPTOR *)HsioNodePlatformDesc;   // Initialize engine pointer to first engine
  HsioNodePlatformDesc--;                                               // Restore HsioNodePlatformDesc address

  HsioNodeCount = HsioNodePlatformDesc->NumEngineDesc;
  EngineId = 0;

  for (Index = 0; Index < HsioNodeCount; Index++) {
    if (HsioEnginePointer->EngineType == HsioPcieEngine) {
      // HsioEntryConfigDump (HsioEnginePointer);
      // Find the UserConfig entry for this HSIO engine
      UserCfgPortPointer = UserCfgPortList;           // Start of the port list for this socket
      EndOfList = FALSE;
      while (EndOfList != TRUE) {
        if ((UserCfgPortPointer->EngineData.StartLane >= (HsioEnginePointer->StartLane + GnbHandle->IodStartLane)) &&
            (UserCfgPortPointer->EngineData.EndLane <= (HsioEnginePointer->EndLane + GnbHandle->IodStartLane)) &&
            (UserCfgPortPointer->EngineData.EngineType == HsioPcieEngine)) {
          EndOfList = TRUE;
        } else {
          if ((UserCfgPortPointer->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
            UserCfgPortPointer++;
          } else {
            EndOfList = TRUE;
            ASSERT (FALSE);                     // Something bad happened here @todo - handle error
          }
        }
      }

      // Find the PCIe wrapper for this HSIO engine
      LocalHandle = GnbHandle;
      while (LocalHandle != NULL) {
        if (LocalHandle->SocketId == SocketId) {
          PcieWrapper = PcieConfigGetChildWrapper (LocalHandle);
          EndOfList   = FALSE;
          HsioEnginePointer->LinkDesc.Pcie.MacPortID = HsioEnginePointer->LinkDesc.Pcie.MacPortID % 8;
          while (EndOfList != TRUE) {
            if ((UserCfgPortPointer->EngineData.StartLane >= PcieWrapper->StartPhyLane) &&
                (UserCfgPortPointer->EngineData.EndLane <= PcieWrapper->EndPhyLane)) {
              EndOfList = TRUE;
			  HsioEnginePointer->LinkDesc.Pcie.MacPortID = PcieWrapper->WrapId * 8 + HsioEnginePointer->LinkDesc.Pcie.MacPortID;
            } else {
              PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
              //HsioEnginePointer->LinkDesc.Pcie.MacPortID += 8;                  /// @todo this should be port offset defined in the wrapper
              if (PcieWrapper == NULL) {
                // ASSERT (FALSE);       // Something bad happened here @todo - handle error
                break;
              }
            }
          }

          // Find the PCIe structure entry for this HSIO engine
          if (PcieWrapper != NULL) {
            PcieEngineList = PcieConfigGetChildEngine (PcieWrapper);
            EndOfList = FALSE;
            while (EndOfList != TRUE) {
              if (PcieEngineList->Type.Port.PortId == HsioEnginePointer->LinkDesc.Pcie.MacPortID) {
                EndOfList = TRUE;
              } else {
                PcieEngineList = PcieLibGetNextDescriptor (PcieEngineList);
                if (PcieEngineList == NULL) {
                  ASSERT (FALSE);                       // Something bad happened here @todo - handle error
                }
              }
            }

            // At this point we have aligned structures...
            // PcieEngineList points to the engine in the Pcie platform structure.
            // UserCfgPortPointer points to the port in UserConfig
            // HsioEnginePointer points to the engine structure in the HSIO structure
            // We can use this data to update the status of the engine in the Pcie platform
            if (PcieEngineList != NULL) {
              PcieEngineList->EngineData.EngineType = (UINT8)HsioEnginePointer->EngineType;
              PcieEngineList->EngineData.StartLane  = (UINT16)HsioEnginePointer->StartLane;
              PcieEngineList->EngineData.EndLane    = (UINT16)HsioEnginePointer->EndLane;
              if (HsioEnginePointer->LinkDesc.Pcie.Capabilities.earlyTrainLink == 1) {
                *EarlyTrain = HsioEnginePointer;
                *EarlyTrainAddress = HsioEnginePointer->LinkDesc.Pcie.DevFunNumber;
                *EarlyTrainEngine  = PcieEngineList;
              }

              PcieEngineList->Type.Port.PortData.PortPresent    = TRUE;
              PcieEngineList->Type.Port.PortData.DeviceNumber   = (UINT8)UserCfgPortPointer->Port.DeviceNumber;
              PcieEngineList->Type.Port.PortData.FunctionNumber = (UINT8)UserCfgPortPointer->Port.FunctionNumber;
              PcieEngineList->Type.Port.PortData.LinkSpeedCapability = (UINT8)UserCfgPortPointer->Port.LinkSpeedCapability;
              PcieEngineList->Type.Port.PortData.SlotNum = UserCfgPortPointer->Port.SlotNum;
              switch (PcdGet8 (PcdPcieAspmControl)) {
                case 0:
                  PcieEngineList->Type.Port.PortData.LinkAspm = HsioAspmDisabled;
                  PcieEngineList->Type.Port.LinkAspmL1_1 = HsioAspmDisabled;
                  PcieEngineList->Type.Port.LinkAspmL1_2 = HsioAspmDisabled;
                  break;
                case 1:
                  PcieEngineList->Type.Port.PortData.LinkAspm = HsioAspmL0s;
                  PcieEngineList->Type.Port.LinkAspmL1_1 = HsioAspmL0s;
                  PcieEngineList->Type.Port.LinkAspmL1_2 = HsioAspmL0s;
                  break;
                case 2:
                  PcieEngineList->Type.Port.PortData.LinkAspm = HsioAspmL1;
                  PcieEngineList->Type.Port.LinkAspmL1_1 = HsioAspmL1;
                  PcieEngineList->Type.Port.LinkAspmL1_2 = HsioAspmL1;
                  break;
                case 3:
                  PcieEngineList->Type.Port.PortData.LinkAspm = HsioAspmL0sL1;
                  PcieEngineList->Type.Port.LinkAspmL1_1 = HsioAspmL0sL1;
                  PcieEngineList->Type.Port.LinkAspmL1_2 = HsioAspmL0sL1;
                  break;
                default:
                  PcieEngineList->Type.Port.PortData.LinkAspm = UserCfgPortPointer->Port.LinkAspm;
                  PcieEngineList->Type.Port.LinkAspmL1_1 = UserCfgPortPointer->Port.LinkAspmL1_1;
                  PcieEngineList->Type.Port.LinkAspmL1_2 = UserCfgPortPointer->Port.LinkAspmL1_2;
              }
              if(PcdGetBool (PcdCfgPcieHotplugSupport) == FALSE) {
                UserCfgPortPointer->Port.LinkHotplug = HsioHotplugDisabled;
              }

              PcieEngineList->Type.Port.PortData.LinkHotplug = UserCfgPortPointer->Port.LinkHotplug;
              // IDS_HDT_CONSOLE (PCIE_MISC, "HsioEnginePointer->LinkHotplug = %d\n", PcieEngineList->Type.Port.PortData.LinkHotplug);
              PcieEngineList->Type.Port.PcieBridgeId = EngineId;                                /// @todo Workaround for speed set
              PcieEngineList->Type.Port.LinkAspmL1_1 = UserCfgPortPointer->Port.LinkAspmL1_1;
              PcieEngineList->Type.Port.LinkAspmL1_2 = UserCfgPortPointer->Port.LinkAspmL1_2;
              PcieEngineList->Type.Port.ClkReq   = UserCfgPortPointer->Port.ClkReq;
              PcieEngineList->Type.Port.EqPreset = (UINT8)UserCfgPortPointer->Port.EqPreset;
              PcieEngineList->Type.Port.PortData.MiscControls.ClkPmSupport = UserCfgPortPointer->Port.MiscControls.ClkPmSupport;
              // IDS_HDT_CONSOLE (PCIE_MISC, "Device 0:%d:%d ClkPmSupport = %d\n", PcieEngineList->Type.Port.PortData.DeviceNumber,
              // PcieEngineList->Type.Port.PortData.FunctionNumber,
              // PcieEngineList->Type.Port.PortData.MiscControls.ClkPmSupport);
              PcieConfigSetDescriptorFlags (PcieEngineList, DESCRIPTOR_ALLOCATED);
              PcieMapPortPciAddress (PcieEngineList, PortDevMap[LocalHandle->RbId]);
              PcieEngineList->Type.Port.Address.AddressValue = MAKE_SBDFO (
                                                                 0,
                                                                 LocalHandle->Address.Address.Bus,
                                                                 PcieEngineList->Type.Port.PortData.DeviceNumber,
                                                                 PcieEngineList->Type.Port.PortData.FunctionNumber,
                                                                 0
                                                                 );
              // PcieConfigEngineDebugDump (PcieEngineList);
            }
          }
        }

        LocalHandle = GnbGetNextHandle (LocalHandle);
      }
    }

    HsioEnginePointer++;
    EngineId++;
  }

  // Configure device map for all NBIOs on this socket
  LocalHandle = GnbHandle;
  while (LocalHandle != NULL) {
    if (LocalHandle->SocketId == SocketId) {
      PcieSetPortPciAddressMap (LocalHandle, PortDevMap[LocalHandle->RbId]);
    }

    LocalHandle = GnbGetNextHandle (LocalHandle);
  }

  return;
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Function to find the topology structure for a specific socket
 *
 *
 *
 * @param[in]     SocketNumber    Socket number to look for
 * @param[in]     UserComplex     Pointer to the top of the topology complex
 * @param[in]     PortList        Pointer to address of port list (for return)
 */
HGPI_STATUS
HsioGetUserCfgForSocket (
  IN      UINT8                     SocketNumber,
  IN      HSIO_COMPLEX_DESCRIPTOR   *UserComplex,
  IN OUT  HSIO_PORT_DESCRIPTOR      **PortList
  )
{
  HGPI_STATUS              HgpiStatus;
  HSIO_COMPLEX_DESCRIPTOR  *UserCfg;

  HgpiStatus = HGPI_ERROR;
  UserCfg    = UserComplex;

  ASSERT (UserCfg != NULL);

  while (UserCfg != NULL) {
    if (UserCfg->SocketId == SocketNumber) {
      *PortList  = UserCfg->PciePortList;
      HgpiStatus = HGPI_SUCCESS;
      break;
    }

    UserCfg = PcieConfigGetNextDataDescriptor (UserCfg);
  }

  ASSERT (HgpiStatus == HGPI_SUCCESS);
  return HgpiStatus;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HSIO User Config Parser
 *
 *
 *
 * @param[in]  PeiServices    Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @param[out] ThisNode       Pointer to a location to store the address of the platform descriptor for this node
 * @param[out] NodeSize       Pointer to a location to store the size (in dwords) of the platforrm desciptor for this node
 * @param[out] AncData        Pointer to a location to store the address of the ancillary port data
 * @param[out] AncDataSize    Pointer to a location to store the size (in dwords) of the ancillary port data
 * @retval     HGPI_STATUS
 */
HGPI_STATUS
HsioParseUserConfig (
  IN       CONST EFI_PEI_SERVICES    **PeiServices,
  IN       GNB_HANDLE                *GnbHandle,
  IN       HSIO_COMPLEX_DESCRIPTOR   *UserConfig,
  OUT      HSIO_PLATFORM_DESCRIPTOR  **ThisNode,
  OUT      UINT32                    *NodeSize,
  OUT      UINT8                     **AncData,
  OUT      UINT32                    *AncDataSize
  )
{
  HGPI_STATUS  HgpiStatus;
  UINT8        SocketId;
  UINT16       NodeMinLane;
  UINT16       NodeMaxLane;
  // HSIO Firmware Structures
  HSIO_PLATFORM_DESCRIPTOR  *NodePlatformDesc;
  HSIO_ENGINE_DESCRIPTOR    *EnginePointer;
  UINT32                    ThisNodeCount;
  UINT32                    ThisNodeSize;
  // Ancillary Data Structures
  UINT32                      DataSize;
  UINT8                       *AncillaryData;
  UINT32                      AncillaryDataSize;
  UINT32                      AncillaryDataOffset;
  ETHERNET_PORT_DATA          *EtherNetPortHeap;
  UINT32                      EtherNetPortOffset;
  HSIO_ANCILLARYDATA          *HsioAncData;
  HSIO_ANCILLARYDATA_SUBHEAP  *HsioSubHeapData;

  UINT32  EtherNetPortCount;
  // User Config Pointers
  HSIO_PORT_DESCRIPTOR  *PortList;                   // Pointer to start of User Config for this socket
  HSIO_PORT_DESCRIPTOR  *PortPointer;                // Updatable pointer for PortList processing
  BOOLEAN               EndOfList;
  BOOLEAN               SataEnabled;

  // Get Socket Number
  SocketId = GnbHandle->SocketId;

  // Get Min/Max lane number for this node
  NodeMinLane = GnbHandle->IodStartLane;
  NodeMaxLane = GnbHandle->IodEndLane;

  SataEnabled = FALSE;
  HgpiStatus  = HGPI_SUCCESS;
  PortList    = NULL;
  HgpiStatus  = HsioGetUserCfgForSocket (SocketId, UserConfig, &PortList);

  if (HGPI_SUCCESS == HgpiStatus) {
    PortPointer         = PortList;
    ThisNodeSize        = sizeof (HSIO_PLATFORM_DESCRIPTOR) + 8; // Include 2 reserved UINT32 at the end
    ThisNodeCount       = 0;
    EtherNetPortCount   = 0;
    EtherNetPortOffset  = 0;
    EtherNetPortHeap    = NULL;
    AncillaryDataSize   = 0;
    AncillaryDataOffset = NULL;
    EndOfList     = FALSE;
    EnginePointer = NULL;

    // Calculate size of Platform Structure and Ancillary Data for this node
    do {
      if (((PortPointer->EngineData.StartLane >= NodeMinLane) && (PortPointer->EngineData.StartLane <= NodeMaxLane)) &&
          (PortPointer->EngineData.EngineType != HsioUnusedEngine)) {
        ThisNodeSize += sizeof (HSIO_ENGINE_DESCRIPTOR);
        ThisNodeCount++;
        if (PortPointer->EngineData.EngineType == HsioEthernetEngine) {
          EtherNetPortCount++;
        } else {
          AncillaryDataSize += HsioAncDataCalcSize (PortPointer);
          // IDS_HDT_CONSOLE (PCIE_MISC, "This port is EtherNet, new count is %d and new AncillaryDataSize is 0x%x\n", ThisNodeCount, AncillaryDataSize);
        }
      }

      if ((PortPointer->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
        PortPointer++;
      } else {
        EndOfList = TRUE;
      }
    } while (EndOfList != TRUE);

    if (EtherNetPortCount != 0) {
      AncillaryDataSize += (4 * sizeof (ETHERNET_PORT_DATA)) + sizeof (HSIO_ANCILLARYDATA_SUBHEAP);
    }

    if (AncillaryDataSize != 0) {
      AncillaryDataSize += sizeof (HSIO_HEAPDATA_CONTAINER) + sizeof (HSIO_ANCILLARYDATA);
    }

    // Build data structures for this node if there is any configuration required
    if (ThisNodeCount != 0) {
      // Allocate memory for this node platform structure
      HgpiStatus = (*PeiServices)->AllocatePool (
                                     PeiServices,
                                     ThisNodeSize,
                                     (VOID **)&NodePlatformDesc
                                     );
      // ASSERT_EFI_ERROR (HgpiStatus);
      *ThisNode = NodePlatformDesc;             // Save pointer to allocated memory
      *NodeSize = (ThisNodeSize + 3) >> 2;      // Platfom Descriptor size in dwords

      // Allocate memory for this node ancillary data
      AncillaryData = NULL;
      if (AncillaryDataSize != 0) {
        HgpiStatus = (*PeiServices)->AllocatePool (
                                       PeiServices,
                                       AncillaryDataSize,
                                       (VOID **)&AncillaryData
                                       );
        if (HgpiStatus == HGPI_SUCCESS) {
          *AncData     = AncillaryData;                      // Save pointer to allocated memory
          *AncDataSize = AncillaryDataSize >> 2;             // Ancillary Data size in dwords

          LibHygonMemFill (AncillaryData, 0, AncillaryDataSize, (HYGON_CONFIG_PARAMS *)NULL);

          HsioAncData = (HSIO_ANCILLARYDATA *)AncillaryData;
          HsioAncData->descriptorType = 0;   /// @todo
          HsioAncData->version   = HSIO_ANCILLARYDATA_VERSION;
          HsioAncData->numDWords = (AncillaryDataSize >> 2) -1;
          AncillaryData += sizeof (HSIO_ANCILLARYDATA);

          AncillaryDataOffset = sizeof (HSIO_ANCILLARYDATA);

          if (EtherNetPortCount != 0) {
            HsioSubHeapData = (HSIO_ANCILLARYDATA_SUBHEAP *)AncillaryData;
            HsioSubHeapData->descriptorType = HSIO_ANCDATA_SUBHEAPTYPE_XGBE;
            HsioSubHeapData->version   = HSIO_ANCILLARYDATA_SUBHEAP_VERSION;
            HsioSubHeapData->numDWORDs = ((4 * sizeof (ETHERNET_PORT_DATA)) + sizeof (HSIO_ANCILLARYDATA_SUBHEAP)) >> 2;
            AncillaryData       += sizeof (HSIO_ANCILLARYDATA_SUBHEAP);
            EtherNetPortHeap     = (ETHERNET_PORT_DATA *)AncillaryData;
            EtherNetPortOffset   = AncillaryDataOffset;
            AncillaryData       += (4 * sizeof (ETHERNET_PORT_DATA));
            AncillaryDataOffset += (4 * sizeof (ETHERNET_PORT_DATA)) + sizeof (HSIO_ANCILLARYDATA_SUBHEAP);
          }
        } else {
          *AncData     = (UINT8 *)NULL;
          *AncDataSize = 0;
        }
      } else {
        *AncData     = (UINT8 *)NULL;
        *AncDataSize = 0;
      }

      // Initialize platform level entries for this structure
      LibHygonMemFill (NodePlatformDesc, 0, ThisNodeSize, (HYGON_CONFIG_PARAMS *)NULL);
      NodePlatformDesc->PlatformType  = ST_PLATFORM_TYPE;
      NodePlatformDesc->NumEngineDesc = ThisNodeCount;
      // Parse structure and copy entries for this node to this node platform structure
      PortPointer = PortList;
      NodePlatformDesc++;                                         // Point to first engine descriptor
      EnginePointer = (HSIO_ENGINE_DESCRIPTOR *)NodePlatformDesc; // Initialize engine pointer to first engine
      NodePlatformDesc--;                                         // Restore NodePlatformDesc address
      AncillaryDataSize = 0;
      EndOfList = FALSE;
      do {
        // IDS_HDT_CONSOLE (PCIE_MISC, "This port starts at lane %d and ends at lane %d\n", PortPointer->EngineData.StartLane,  PortPointer->EngineData.EndLane);
        if (((PortPointer->EngineData.StartLane >= NodeMinLane) && (PortPointer->EngineData.StartLane <= NodeMaxLane)) &&
            (PortPointer->EngineData.EngineType != HsioUnusedEngine)) {
          if ((PortPointer->EngineData.StartLane < NodeMinLane) || (PortPointer->EngineData.StartLane > NodeMaxLane)) {
            /// egh fix this
            // Invalid configuration
            ASSERT (FALSE);
          }

          if (PortPointer->EngineData.StartLane > PortPointer->EngineData.EndLane) {
            IDS_HDT_CONSOLE (GNB_TRACE, "ERROR:Start Lane(%d) > End Lane(%d)!!!\n", PortPointer->EngineData.StartLane, PortPointer->EngineData.EndLane);
            ASSERT (FALSE);
          }

          EnginePointer->StartLane     = PortPointer->EngineData.StartLane - NodeMinLane;
          EnginePointer->EndLane       = PortPointer->EngineData.EndLane - NodeMinLane;
          EnginePointer->GpioGroupID   = 1; // Unused
          EnginePointer->engineResetID = PortPointer->EngineData.GpioGroupId;
          EnginePointer->EngineType    = PortPointer->EngineData.EngineType;
          EnginePointer->HotPluggable  = PortPointer->EngineData.HotPluggable;
          switch (PortPointer->EngineData.EngineType) {
            case HsioPcieEngine:
            case HsioCxlEngine:
              EnginePointer->LinkDesc.Pcie.Capabilities.earlyTrainLink = PortPointer->Port.MiscControls.SbLink;

              if (PcdGet8 (PcdPcieLinkTrainingType) == 0) {
                IDS_HDT_CONSOLE (GNB_TRACE, "Train single step\n");
                if (EnginePointer->LinkDesc.Pcie.Capabilities.earlyTrainLink == 1) {
                  EnginePointer->LinkDesc.Pcie.Capabilities.maxLinkSpeedCap = HsioGen1;
                } else {
                  EnginePointer->LinkDesc.Pcie.Capabilities.maxLinkSpeedCap = (UINT8)PortPointer->Port.LinkSpeedCapability;
                }
              } else {
                IDS_HDT_CONSOLE (GNB_TRACE, "Train two step\n");
                EnginePointer->LinkDesc.Pcie.Capabilities.maxLinkSpeedCap = HsioGen1;
              }

              // mask ASPM L0S
              PortPointer->Port.LinkAspm = PortPointer->Port.LinkAspm & 0x02;

              EnginePointer->LinkDesc.Pcie.DevFunNumber = (PortPointer->Port.DeviceNumber << 3) | PortPointer->Port.FunctionNumber;
              EnginePointer->LinkDesc.Pcie.Capabilities.enableTurnOffSettings = 1;
              EnginePointer->LinkDesc.Pcie.Capabilities.turnOffUnusedLanes    = PortPointer->Port.MiscControls.TurnOffUnusedLanes;
              EnginePointer->LinkDesc.Pcie.Capabilities.portPresent = (UINT8)PortPointer->Port.PortPresent;
              if(PcdGetBool (PcdCfgPcieHotplugSupport) == FALSE) {
                PortPointer->Port.LinkHotplug = HsioHotplugDisabled;
              }

              EnginePointer->LinkDesc.Pcie.Capabilities.linkHotplug = PortPointer->Port.LinkHotplug;
              EnginePointer->LinkDesc.Pcie.Capabilities.autoSpeedChangeCntrl = (UINT8)PortPointer->Port.AutoSpdChng;

              if (PcdGet8 (PcdPcieEqSearchMode) != 1) {
                EnginePointer->LinkDesc.Pcie.Capabilities.eqSearchModeOride = 1;
                EnginePointer->LinkDesc.Pcie.Capabilities.eqSearchMode =
                  PcdGet8 (PcdPcieEqSearchMode);
                IDS_HDT_CONSOLE (
                  GNB_TRACE,
                  "EqSearchMode: %d\n",
                  EnginePointer->LinkDesc.Pcie.Capabilities.eqSearchMode
                  );
              }

              if (PortPointer->Port.LinkHotplug == HsioHotplugServerEntSSD) {
                EnginePointer->LinkDesc.Pcie.Capabilities.linkHotplug = HsioHotplugServerExpress;
              }

              if (PortPointer->EngineData.StartLane > PortPointer->EngineData.EndLane) {
                EnginePointer->LinkDesc.Pcie.Capabilities.linkReversal = 1;
              }

              if (PortPointer->Port.CxlMode == CXL_1_1) {
                //cxl 1.1
                EnginePointer->LinkDesc.Cxl.WorkMode = 1;
                EnginePointer->LinkDesc.Cxl.CxlMode = 0;
              } else if (PortPointer->Port.CxlMode == CXL_2_0) {
                //cxl 2.0
                EnginePointer->LinkDesc.Cxl.WorkMode = 1;
                EnginePointer->LinkDesc.Cxl.CxlMode = 1;
              } else {
                //cxl pcie
                EnginePointer->LinkDesc.Cxl.WorkMode = 0;
                EnginePointer->LinkDesc.Cxl.CxlMode = 0;
              }
              DataSize = HsioAncDataPopulate (EnginePointer, PortPointer, AncillaryData);
              if (0 != DataSize) {
                EnginePointer->LinkDesc.Pcie.AncillaryDataOffset = AncillaryDataOffset >> 2;
                AncillaryData += DataSize;
                AncillaryDataOffset += DataSize;
              }

              break;
            case HsioSATAEngine:
              EnginePointer->LinkDesc.Sata.ChannelType = PortPointer->Port.MiscControls.ChannelType;
              SataEnabled = TRUE;
              DataSize    = HsioAncDataPopulate (EnginePointer, PortPointer, AncillaryData);
              if (0 != DataSize) {
                EnginePointer->LinkDesc.Sata.AncillaryDataOffset = AncillaryDataOffset >> 2;
                AncillaryData += DataSize;
                AncillaryDataOffset += DataSize;
              }

              break;
            case HsioDPEngine:
              break;
            case HsioEthernetEngine:
              NbioEnableNbifDevice (GnbHandle, HsioEthernetEngine, (UINT8)EnginePointer->StartLane);
              EnginePointer->LinkDesc.Eth.AncillaryDataOffset = EtherNetPortOffset >> 2;
              EtherNetPortHeap += PortPointer->EtherNet.EthPortProp0.PortNum;
              LibHygonMemCopy (
                (VOID *)EtherNetPortHeap,
                (VOID *)&PortPointer->EtherNet,
                sizeof (ETHERNET_PORT_DATA),
                (HYGON_CONFIG_PARAMS *)NULL
                );
              EtherNetPortHeap -= PortPointer->EtherNet.EthPortProp0.PortNum;
              break;
            case HsioGOPEngine:
              break;
            default:
              break;
          }

          // HsioEntryConfigDump (EnginePointer);
          EnginePointer++;
        }

        if ((PortPointer->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
          PortPointer++;
        } else {
          EndOfList = TRUE;
        }
      } while (EndOfList != TRUE);
    } else {
      // Nothing found for this node, skip HSIO call
      *ThisNode = NULL;   // Save pointer to allocated memory
      *NodeSize = 0;      // Platfom Descriptor size in dwords
    }

    if ((EnginePointer != NULL) && (SataEnabled == TRUE)) {
      NbioEnableNbifDevice (GnbHandle, HsioSATAEngine, (UINT8)EnginePointer->StartLane);
    } else {
      if (!PcdGetBool (PcdHygonSataEnable)) {
        NbioDisableNbifDevice (GnbHandle, HsioSATAEngine, (UINT8)EnginePointer->StartLane);
      }
    }
  }

  // Return pointer to this node platform structure
  return (HgpiStatus);
}

/*----------------------------------------------------------------------------------------*/

/**
 * HSIO Process Results
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @param[in]  ThisNode       Pointer to the platform descriptor for this node
 * @param[in]  ThisNodeSize   Size of the platform descriptor for this node
 * @retval     HGPI_STATUS
 */
HGPI_STATUS
HsioProcessResultsV1 (
  IN       GNB_HANDLE                      *GnbHandle,
  IN       HSIO_COMPLEX_DESCRIPTOR         *UserConfig,
  IN       HSIO_PLATFORM_DESCRIPTOR        *ThisNode,
  IN       UINT32                          ThisNodeSize
  )
{
  HGPI_STATUS             HgpiStatus;
  UINT32                  HsioMsgArg[6];
  UINT8                   HsioRetval;
  BOOLEAN                 HsioComplete;
  HSIO_RETURN_PARAMS      *ReturnArgs;
  LISM_RETURN_PARAMS      *LismRetParams;
  PCI_ADDR                EarlyTrainAddress;
  HSIO_ENGINE_DESCRIPTOR  *EarlyTrainDesc;
  PCIe_ENGINE_CONFIG      *EarlyTrainEngine;
  BOOLEAN                 PcieStatusUpdated;

  // Process results
  HgpiStatus = HGPI_SUCCESS;
  ReturnArgs = (HSIO_RETURN_PARAMS*) &HsioMsgArg[0];
  PcieStatusUpdated = FALSE;
  HsioComplete = FALSE;
  EarlyTrainAddress.AddressValue = 0;
  EarlyTrainDesc   = NULL;
  EarlyTrainEngine = NULL;
  do {
    LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
    HsioServiceRequest (GnbHandle, HSIO_MSG_LISM_COLLECTRESULTS, HsioMsgArg, HsioMsgArg);
    HsioRetval = (UINT8) (HsioMsgArg[0] & 0xFF);
    switch (HsioRetval) {
      case HSIO_MBOX_RETVAL_OK:
        switch (ReturnArgs->RetParams_Type) {
          case HSIO_MBOX_RETPARAMS_LISM:
            // Process return parameters
            LismRetParams = (LISM_RETURN_PARAMS*) &HsioMsgArg[2];
            IDS_HDT_CONSOLE (GNB_TRACE, "Current State = 0x%x\n", LismRetParams->CurrentState);
            switch (LismRetParams->CurrentState) {
              case HSIO_LinkInitState_done:
                HsioComplete = TRUE;
                break;
              case HSIO_LinkInitState_mapped:
                // Ports are mapped.  Allow pre-configuration options to be programmed.
                IDS_HDT_CONSOLE (GNB_TRACE, "PORTS ARE MAPPED\n");
                LibHygonMemFill ((VOID *)ThisNode, 0x00, ThisNodeSize << 2, (HYGON_CONFIG_PARAMS *) NULL);
                LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
                HsioMsgArg[2] = (UINT32) ThisNode;
                HsioMsgArg[3] = ThisNodeSize;
                HsioServiceRequest (GnbHandle, HSIO_MSG_INIT_DMAENGINECONIFGTODRAM, HsioMsgArg, HsioMsgArg);
                EarlyTrainDesc = NULL;
                HsioUpdatePcieStatus (GnbHandle,
                                      UserConfig,
                                      ThisNode,
                                      (HSIO_ENGINE_DESCRIPTOR**)&EarlyTrainDesc,
                                      &EarlyTrainAddress.AddressValue,
                                      (PCIe_ENGINE_CONFIG **) &EarlyTrainEngine
                                      );
                break;
              case HSIO_LinkInitState_configured:
                // Configuration is done.  Allow post-configuration, pre-training options to be programmed.
                IDS_HDT_CONSOLE (GNB_TRACE, "RECONFIG IS COMPLETE\n");
                HsioConfigurationProgramAfterReconfig (GnbHandle);
                CustomerPcieEqCfgBeforeTraining (GnbHandle);
                //ReleasePcieReset (GnbHandle);
                break;
              case HSIO_LinkInitState_vetting:
                // Set HsioComplete for syncing all dies in multi-die config
                HsioComplete = TRUE;

              case HSIO_LinkInitState_earlyTrained:
                // Read back training results
                IDS_HDT_CONSOLE (GNB_TRACE, "Vetting or earlyTrained state detected...\n");
                
                break;
              default:
                break;
            }

            break;
          case HSIO_MBOX_RETPARAMS_REQ_RESET:
            // Issue reset request for specific PCIe Engine

            break;
          case HSIO_MBOX_RETPARAMS_NONE:
            HsioComplete = TRUE;
            break;
          case HSIO_MBOX_RETPARAMS_GENERIC:
            // Process return parameters
            break;
          default:
            IDS_HDT_CONSOLE (GNB_TRACE, "Invalid return parameter [0x%x]\n", HsioRetval);
            ASSERT (FALSE);
            break;
        }

        break;
      case HSIO_MBOX_RETVAL_ERROR:
        switch (ReturnArgs->RetParams_Type) {
          case HSIO_MBOX_RETPARAMS_LISM:
            LismRetParams = (LISM_RETURN_PARAMS*) &HsioMsgArg[2];
            IDS_HDT_CONSOLE (GNB_TRACE, "Current State = 0x%x", LismRetParams->CurrentState);
            HgpiStatus   = HGPI_CRITICAL;
            HsioComplete = TRUE;
            break;
          default:
            ASSERT (FALSE);
            break;
        }

        break;
      default:
        IDS_HDT_CONSOLE (GNB_TRACE, "HSIO Mailbox Request did not return OK [0x%x]\n", HsioRetval);
        ASSERT (FALSE);
        HsioComplete = TRUE;
        break;
    }

    if (FALSE == HsioComplete) {
      LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
      HsioServiceRequest (GnbHandle, HSIO_MSG_LISM_RESUME, HsioMsgArg, HsioMsgArg);
    }
  } while (FALSE == HsioComplete);

  if ((PcieStatusUpdated != TRUE) && (EarlyTrainDesc != NULL)) {
    GnbLibPciRMW (EarlyTrainAddress.AddressValue | 0x18, AccessWidth32, 0xFF000FF, 0x000000, (HYGON_CONFIG_PARAMS *)NULL);
  }

  return HgpiStatus;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Concurrent Training
 *
 *
 * @param[in]  PeiServices     Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  Pcie            Pointer to the HGPI platfom complex
 * @param[in]  UserConfig      Pointer to the platform BIOS supplied platform configuration
 * @retval     HGPI_STATUS
 */
HGPI_STATUS
HsioConcurrentTraining (
  IN       CONST EFI_PEI_SERVICES          **PeiServices,
  IN       PCIe_PLATFORM_CONFIG            *Pcie,
  IN       HSIO_COMPLEX_DESCRIPTOR         *UserConfig
  )
{
  HGPI_STATUS               HgpiStatus;
  HGPI_STATUS               Status;
  GNB_HANDLE                *GnbHandle;
  UINT32                    HsioMsgArg[6];
  UINT8                     InstanceId;
  UINT8                     HsioRetval;
  HSIO_PLATFORM_DESCRIPTOR  *PlatformDesc[MAX_INSTANCE_ID];
  UINT32                    PlatformDescSize[MAX_INSTANCE_ID];
  UINT8                     *AncillaryData[MAX_INSTANCE_ID];
  UINT32                    AncillaryDataSize[MAX_INSTANCE_ID];

  HgpiStatus = HGPI_SUCCESS;

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    InstanceId = GnbHandle->GlobalIodId;
    HsioParseUserConfig (
      PeiServices,
      GnbHandle,
      UserConfig,
      &PlatformDesc[InstanceId],
      &PlatformDescSize[InstanceId],
      &AncillaryData[InstanceId],
      &AncillaryDataSize[InstanceId]
      );

    if (PlatformDesc[InstanceId] != NULL) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Platform Descriptor for Global IOD %d\n", InstanceId);
      HsioDebugDump (
        PlatformDesc[InstanceId],
        PlatformDescSize[InstanceId],
        AncillaryData[InstanceId],
        AncillaryDataSize[InstanceId]
        );
    }

    if (InstanceId == UserConfig->BmcLinkLocation) {
      LibHygonMemFill(HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *)NULL);
      HsioServiceRequest (GnbHandle, HSIO_MSG_LISM_RELOAD, HsioMsgArg, HsioMsgArg);
    }

    LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
    HsioServiceRequest (GnbHandle, HSIO_MSG_INIT_INIT, HsioMsgArg, HsioMsgArg);
    HsioRetval = (UINT8) (HsioMsgArg[0] & 0xFF);
    if (HsioRetval != HSIO_MBOX_RETVAL_OK) {
      PlatformDesc[InstanceId] = NULL;
    }

    if (PlatformDesc[InstanceId] != NULL) {
      LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
      HsioServiceRequest (GnbHandle, HSIO_MSG_DBUG_GETVERSION, HsioMsgArg, HsioMsgArg);

      LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
      HsioMsgArg[1] = DBUG_SETSCRATCHFLAG_RETURNAFTERMAPPING;
      HsioMsgArg[2] = 1;
      HsioServiceRequest (GnbHandle, HSIO_MSG_DBUG_SETSCRATCH, HsioMsgArg, HsioMsgArg);

      LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
      HsioMsgArg[1] = DBUG_SETSCRATCHFLAG_RETURNAFTERCONFIG;
      HsioMsgArg[2] = 1;
      HsioServiceRequest (GnbHandle, HSIO_MSG_DBUG_SETSCRATCH, HsioMsgArg, HsioMsgArg);

      LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
      HsioMsgArg[1] = DBUG_SETSCRATCHFLAG_V1ANCILLARYDATA;
      HsioMsgArg[2] = 1;
      HsioServiceRequest (GnbHandle, HSIO_MSG_DBUG_SETSCRATCH, HsioMsgArg, HsioMsgArg);

      LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
      HsioServiceRequest (GnbHandle, HSIO_MSG_INIT_LOADCAPABILITIES, HsioMsgArg, HsioMsgArg);

      if (AncillaryDataSize[InstanceId] != 0) {
        LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
        HsioMsgArg[1] = 0;
        HsioMsgArg[2] = (UINT32) AncillaryData[InstanceId];
        HsioMsgArg[3] = AncillaryDataSize[InstanceId];
        HsioMsgArg[5] = HSIO_HEAPTYPE_ANCDATA;
        HsioServiceRequest (GnbHandle, HSIO_MSG_INIT_LOADHEAP, HsioMsgArg, HsioMsgArg);
      }

      LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
      HsioMsgArg[4] = 1;
      HsioMsgArg[5] = HSIO_HEAPTYPE_MACPCS;
      HsioServiceRequest (GnbHandle, HSIO_MSG_INIT_LOADHEAP, HsioMsgArg, HsioMsgArg);

      LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
      HsioMsgArg[4] = 1;
      HsioMsgArg[5] = HSIO_HEAPTYPE_GPIOPTR;
      HsioServiceRequest (GnbHandle, HSIO_MSG_INIT_LOADHEAP, HsioMsgArg, HsioMsgArg);

      LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
      HsioMsgArg[1] = 0;
      HsioMsgArg[2] = (UINT32) PlatformDesc[InstanceId];
      HsioMsgArg[3] = PlatformDescSize[InstanceId];
      HsioMsgArg[5] = HSIO_HEAPTYPE_CFGPTR;
      HsioServiceRequest (GnbHandle, HSIO_MSG_INIT_LOADHEAP, HsioMsgArg, HsioMsgArg);

      // Inititiate configuration
      LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
      HsioServiceRequest (GnbHandle, HSIO_MSG_LISM_GO, HsioMsgArg, HsioMsgArg);
    } 

    GnbHandle = GnbGetNextIodHandle (GnbHandle);
  }

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    InstanceId = GnbHandle->GlobalIodId;
    if (PlatformDesc[InstanceId] != NULL) {
      // Process results
      Status = HsioProcessResultsV1 (GnbHandle, UserConfig, PlatformDesc[InstanceId], PlatformDescSize[InstanceId]);
      HGPI_STATUS_UPDATE (Status, HgpiStatus);
    }
    GnbHandle = GnbGetNextIodHandle (GnbHandle);
  }

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    if (GnbHandle->DieType == IOD_DUJIANG) {
      ReleasePcieReset (GnbHandle);
    }
    GnbHandle = GnbGetNextIodHandle (GnbHandle);
  }

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    InstanceId = GnbHandle->GlobalIodId;
    if (PlatformDesc[InstanceId] != NULL) {
      LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
      HsioServiceRequest (GnbHandle, HSIO_MSG_LISM_RESUME, HsioMsgArg, HsioMsgArg);
      // Process results
      Status = HsioProcessResultsV1 (GnbHandle, UserConfig, PlatformDesc[InstanceId], PlatformDescSize[InstanceId]);
      HGPI_STATUS_UPDATE (Status, HgpiStatus);

      // Read back training results
      LibHygonMemFill ((VOID *)(PlatformDesc[InstanceId]), 0x00, PlatformDescSize[InstanceId] << 2, (HYGON_CONFIG_PARAMS *)NULL);
      LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
      HsioMsgArg[2] = (UINT32) PlatformDesc[InstanceId];
      HsioMsgArg[3] = PlatformDescSize[InstanceId];
      HsioServiceRequest (GnbHandle, HSIO_MSG_INIT_DMAENGINECONIFGTODRAM, HsioMsgArg, HsioMsgArg);

      HsioUpdatePortTrainingStatus (GnbHandle, UserConfig, PlatformDesc[InstanceId]);
    }
    GnbHandle = GnbGetNextIodHandle (GnbHandle);
  }

  return HgpiStatus;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HSIO Early Init
 *
 *
 *
 * @param[in]  PeiServices     Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  Pcie            Pointer to the HGPI platfom complex
 * @param[in]  UserConfig      Pointer to the platform BIOS supplied platform configuration
 * @retval     HGPI_STATUS
 */
HGPI_STATUS
HsioEarlyInit (
  IN       CONST EFI_PEI_SERVICES          **PeiServices,
  IN       PCIe_PLATFORM_CONFIG            *Pcie,
  IN       HSIO_COMPLEX_DESCRIPTOR         *UserConfig
  )
{
  HGPI_STATUS  HgpiStatus;

  IDS_HDT_CONSOLE (GNB_TRACE, "HsioEarlyInit Enter\n");

  HgpiStatus = HGPI_SUCCESS;
  HgpiStatus = HsioConcurrentTraining (PeiServices, Pcie, UserConfig);
  IDS_HDT_CONSOLE (GNB_TRACE, "HsioEarlyInit Exit [0x%x]\n", HgpiStatus);
  return HgpiStatus;
}

/*----------------------------------------------------------------------------------------*/

/**
 * PCIe Port Speed Update Request
 *
 *
 *
 * @param[in]  PeiServices     Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  InstanceId      The unique identifier of the NBIO instance associated with this socket/die
 * @param[in]  EngineId        PCIe bridge ID
 * @param[in]  TargetSpeed     Desired speed of the target port (Gen1, Gen2, Gen3)
 */
VOID
HsioSetLinkSpeed (
  IN       CONST EFI_PEI_SERVICES          **PeiServices,
  IN       GNB_HANDLE                      *GnbHandle,
  IN       UINT32                          EngineId,
  IN       UINT32                          TargetSpeed
  )
{
  UINT32                    HsioMsgArg[6];
  UINT32                    HsioMboxMessage;


  LibHygonMemFill (HsioMsgArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
  HsioMsgArg[1] = HsioPcieEngine;
  HsioMsgArg[2] = TargetSpeed;
  HsioMsgArg[3] = EngineId;

  HsioMboxMessage = HSIO_MSG_PHYS_TARGETFREQUPDATE | (HSIO_freqUpdType_single << 10);
  HsioServiceRequest (GnbHandle, HsioMboxMessage, HsioMsgArg, HsioMsgArg);
}
