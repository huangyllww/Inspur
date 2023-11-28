/* $NoKeywords:$ */
/**
 * @file
 *
 * DXIO initialization services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Filecode.h>
#include <PiPei.h>
#include <AmdPcieComplex.h>
#include <GnbDxio.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Library/BaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/GnbLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/DxioLibV1.h>
#include <Library/OemGpioResetControlLib.h>
#include "DxioLibLocal.h"

#define FILECODE LIBRARY_DXIOLIBV1_DXIOLIBV1_FILECODE

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
DxioCfgBeforeReconfig (
  IN       GNB_HANDLE       *GnbHandle
  );

VOID
DxioCfgAfterReconfig (
  IN       GNB_HANDLE       *GnbHandle
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump input configuration to user engine descriptor
 *
 *
 * @param[in]  EngineDescriptor   Pointer to engine descriptor
 */
VOID
DxioEntryConfigDump (
  IN       DXIO_ENGINE_DESCRIPTOR    *EngineDescriptor
  )
{
  UINT8       *Buffer;

  Buffer = (UINT8*) EngineDescriptor;
  //GnbLibDebugDumpBuffer ((VOID*) EngineDescriptor, sizeof(DXIO_ENGINE_DESCRIPTOR), 1, 16);
  IDS_HDT_CONSOLE (PCIE_MISC, "\n");

  IDS_HDT_CONSOLE (PCIE_MISC, "  Engine Type - %a\n",
    (EngineDescriptor->EngineType == DxioPcieEngine) ? "PCIe Port" : (
    (EngineDescriptor->EngineType == DxioSATAEngine) ? "SATA Port" : (
    (EngineDescriptor->EngineType == DxioEthernetEngine) ? "Ethernet Port" : (
    (EngineDescriptor->EngineType == DxioUnusedEngine) ? "Unused" : "Invalid")))
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "    Start Phy Lane - %d\n    End   Phy Lane - %d\n",
    EngineDescriptor->StartLane,
    EngineDescriptor->EndLane
  );
  IDS_HDT_CONSOLE (PCIE_MISC, "    Hotplug - %d\n", EngineDescriptor->HotPluggable);
  IDS_HDT_CONSOLE (PCIE_MISC, "    GpioGroupId - %d\n", EngineDescriptor->GpioGroupID);
  if (EngineDescriptor->EngineType == PciePortEngine) {
    IDS_HDT_CONSOLE (PCIE_MISC, "    PortPresent - %d\n    maxLinkSpeedCap - %d\n" ,
      EngineDescriptor->LinkDesc.Pcie.Capabilities.portPresent,
      EngineDescriptor->LinkDesc.Pcie.Capabilities.maxLinkSpeedCap
    );
    //IDS_HDT_CONSOLE (PCIE_MISC, "    TrainingState - %d\n", EngineDescriptor->TrainingState);
    IDS_HDT_CONSOLE (PCIE_MISC, "    MacPortID - %d\n", EngineDescriptor->LinkDesc.Pcie.MacPortID);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump the data being sent to SMU
 *
 *
 * @param[in]  PlatformDescriptor   Pointer to platform descriptor
 * @param[in]  PlatformDescSize     Size of platform descriptor
 * @param[in]  AncillaryData        Pointer to platform descriptor
 * @param[in]  AncillaryDataSize    Size of platform descriptor
 */
VOID
DxioDebugDump (
  IN       DXIO_PLATFORM_DESCRIPTOR  *PlatformDescriptor,
  IN       UINT32                    PlatformDescSize,
  IN       UINT8                     *AncillaryData,
  IN       UINT32                    AncillaryDataSize
  )
{
  DXIO_PLATFORM_DESCRIPTOR  *PlatformPointer;
  DXIO_ENGINE_DESCRIPTOR    *EnginePointer;
  UINT32                    EngineCount;
  UINT32                    Index;


  PlatformPointer = (DXIO_PLATFORM_DESCRIPTOR *) PlatformDescriptor;
  EngineCount = PlatformPointer->NumEngineDesc;
  IDS_HDT_CONSOLE (GNB_TRACE, "Number of Engines is %d\n", EngineCount);
  PlatformPointer++;
  EnginePointer = (DXIO_ENGINE_DESCRIPTOR *)PlatformPointer;
  for (Index = 0;Index < EngineCount; Index++) {
    DxioEntryConfigDump (EnginePointer);
    EnginePointer++;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "Sending this Platform Descriptor to SMU...\n");
  GnbLibDebugDumpBuffer ((VOID*) PlatformDescriptor, PlatformDescSize << 2, 1, 16);
  IDS_HDT_CONSOLE (GNB_TRACE, "\n");
  if (AncillaryDataSize != 0) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Sending this Ancillary Data to SMU...\n");
    GnbLibDebugDumpBuffer ((VOID*) AncillaryData, AncillaryDataSize << 2, 3, 8);
    IDS_HDT_CONSOLE (GNB_TRACE, "\n");
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "No ancillary data to send\n");
  }
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * DXIO Update Port Training Status
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @param[in]  ThisNode       Pointer to the platform descriptor for this node
 * @retval     AGESA_STATUS
 */
VOID
DxioUpdatePortTrainingStatus (
  IN      GNB_HANDLE                      *GnbHandle,
  IN      DXIO_COMPLEX_DESCRIPTOR         *UserConfig,
  IN OUT  DXIO_PLATFORM_DESCRIPTOR        *ThisNode,
  IN      PEI_AMD_NBIO_SMU_SERVICES_PPI   *SmuServicesPpi
  )
{
  // DXIO Firmware Structures
  DXIO_PLATFORM_DESCRIPTOR  *DxioNodePlatformDesc;
  DXIO_ENGINE_DESCRIPTOR    *DxioEnginePointer;
  UINT32                    DxioNodeCount;
  //User Config Pointers
  DXIO_COMPLEX_DESCRIPTOR   *UserCfg;
  DXIO_PORT_DESCRIPTOR      *UserCfgPortList;              // Pointer to start of User Config for this socket
  DXIO_PORT_DESCRIPTOR      *UserCfgPortPointer;           // Updatable pointer for UserCfgPortList processing
  // PCIe Structures
  PCIe_ENGINE_CONFIG        *PcieEngineList;
  PCIe_WRAPPER_CONFIG       *PcieWrapper;
  PCIE_LC_STATE0_STRUCT     LcState0;
  UINT8                     SocketNumber;
  BOOLEAN                   EndOfList;
  UINT32                    Index;
  UINT8                     EngineId;
  UINT8                     PortDevMap[16];
  BOOLEAN                   TrainingSuccess;
  PCIE_LINK_CNTL2_STRUCT    LinkCntl2;
  PCIE_LINK_STATUS_STRUCT   LinkStatus;

  //IDS_HDT_CONSOLE (PCIE_MISC, "DxioUpdatePortTrainingStatus enter\n");

  LibAmdMemFill (PortDevMap, 0, sizeof (PortDevMap), (AMD_CONFIG_PARAMS *) NULL);

  // Get Socket Number
  SocketNumber = GnbHandle->SocketId;
  UserCfg = UserConfig;
  EndOfList = FALSE;
  UserCfgPortList = NULL;
  while (EndOfList != TRUE) {
    //IDS_HDT_CONSOLE (PCIE_MISC, " This list is for socket %d\n", UserCfg->SocketId);
    if (UserCfg->SocketId == SocketNumber) {
      UserCfgPortList = UserCfg->PciePortList;
    }
    if ((UserCfg->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
      UserCfg++;
    } else {
      EndOfList = TRUE;
    }
  }

  DxioNodePlatformDesc = ThisNode;
  DxioNodePlatformDesc ++;                                              // Point to first engine descriptor
  DxioEnginePointer = (DXIO_ENGINE_DESCRIPTOR *)DxioNodePlatformDesc;       // Initialize engine pointer to first engine
  DxioNodePlatformDesc --;                                              // Restore DxioNodePlatformDesc address

  DxioNodeCount = DxioNodePlatformDesc->NumEngineDesc;

  EngineId = 0;
//  for (Index = 0;Index < DxioNodeCount;Index++) {
//    GnbLibDebugDumpBuffer ((VOID*) DxioEnginePointer, sizeof(DXIO_ENGINE_DESCRIPTOR), 1, 16);
//    IDS_HDT_CONSOLE (PCIE_MISC, "\n");
//    DxioEnginePointer++;
//  }

  DxioNodePlatformDesc = ThisNode;
  DxioNodePlatformDesc ++;                                              // Point to first engine descriptor
  DxioEnginePointer = (DXIO_ENGINE_DESCRIPTOR *)DxioNodePlatformDesc;       // Initialize engine pointer to first engine
  DxioNodePlatformDesc --;                                              // Restore DxioNodePlatformDesc address

  DxioNodeCount = DxioNodePlatformDesc->NumEngineDesc;

  for (Index = 0;Index < DxioNodeCount;Index++) {
    if (DxioEnginePointer->EngineType == DxioPcieEngine) {
      if (DxioEnginePointer->StartLane >= 16) {
        DxioEnginePointer->StartLane -= 4;
        DxioEnginePointer->EndLane -= 4;     ///@todo this value should be an offset defined in the wrapper
      }
      DxioEnginePointer->StartLane += GnbHandle->StartLane;
      DxioEnginePointer->EndLane += GnbHandle->StartLane;
      //DxioEntryConfigDump (DxioEnginePointer);
      // Find the UserConfig entry for this DXIO engine
      UserCfgPortPointer = UserCfgPortList;       // Start of the port list for this socket
      EndOfList = FALSE;
      while (EndOfList != TRUE) {
        //IDS_HDT_CONSOLE (PCIE_MISC, "This port starts at lane %d and ends at lane %d\n", UserCfgPortPointer->EngineData.StartLane,  UserCfgPortPointer->EngineData.EndLane);
        if ((UserCfgPortPointer->EngineData.StartLane >= DxioEnginePointer->StartLane) &&
            (UserCfgPortPointer->EngineData.EndLane <= DxioEnginePointer->EndLane) &&
            (UserCfgPortPointer->EngineData.EngineType == DxioPcieEngine)) {
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
      // Find the PCIe wrapper for this DXIO engine
      PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
      EndOfList = FALSE;
      while (EndOfList != TRUE) {
        //IDS_HDT_CONSOLE (PCIE_MISC, "Wrapper StartLane = %d, EndLane = %d\n",PcieWrapper->StartPhyLane, PcieWrapper->EndPhyLane);
        if ((UserCfgPortPointer->EngineData.StartLane >= (PcieWrapper->StartPhyLane + GnbHandle->StartLane)) &&
            (UserCfgPortPointer->EngineData.EndLane <= (PcieWrapper->EndPhyLane + GnbHandle->StartLane))) {
          EndOfList = TRUE;
        } else {
          PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
          DxioEnginePointer->LinkDesc.Pcie.MacPortID += 8;      ///@todo this should be port offset defined in the wrapper
          if (PcieWrapper == NULL) {
            ASSERT (FALSE);       // Something bad happened here @todo - handle error
          }
        }
      }

      // Find the PCIe structure entry for this DXIO engine
      PcieEngineList = PcieConfigGetChildEngine (PcieWrapper);
      EndOfList = FALSE;
      while (EndOfList != TRUE) {
//        IDS_HDT_CONSOLE (PCIE_MISC, "This PortId = %d\n", PcieEngineList->Type.Port.PortId);
        if (PcieEngineList->Type.Port.PortId == DxioEnginePointer->LinkDesc.Pcie.MacPortID) {
            EndOfList = TRUE;
        } else {
          PcieEngineList = PcieLibGetNextDescriptor (PcieEngineList);
          if (PcieEngineList == NULL) {
            ASSERT (FALSE);       // Something bad happened here @todo - handle error
          }
        }
      }

      // At this point we have aligned structures...
      // PcieEngineList points to the engine in the Pcie platform structure.
      // UserCfgPortPointer points to the port in UserConfig
      // DxioEnginePointer points to the engine structure in the DXIO structure
      // We can use this data to update the status of the engine in the Pcie platform
      if (PcieEngineList != NULL) {
          /*JIRA PS-264      fixed PCIE card lose issue start*/
          TrainingSuccess = FALSE;
          LcState0.Value = 0;
          //MicroSecondDelay(1000);
          if (PcieEngineList->Type.Port.PortId > 7) {
              NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_STATE0_ADDRESS+((PcieEngineList->Type.Port.PortId - 8) << 12), &LcState0.Value, 0);
              IDS_HDT_CONSOLE (PCIE_MISC, "LcState0 for Port %d is 0x%x\n", PcieEngineList->Type.Port.PortId, LcState0.Value);
              if ((LcState0.Field.LC_CURRENT_STATE > 4)) {               
                TrainingSuccess = TRUE;
              }
          } else {
              NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_STATE0_ADDRESS + ((PcieEngineList->Type.Port.PortId) << 12), &LcState0.Value, 0);
              IDS_HDT_CONSOLE (PCIE_MISC, "LcState0 for Port %d is 0x%x\n", PcieEngineList->Type.Port.PortId, LcState0.Value);
              if ((LcState0.Field.LC_CURRENT_STATE > 4)) {
                TrainingSuccess = TRUE;
              }
          }
          /*JIRA PS-264     fixed PCIE card lose issue end*/
        if ((TrainingSuccess != TRUE) || (DxioEnginePointer->LinkDesc.Pcie.Capabilities.portPresent == 0)) {
          PcieEngineList->InitStatus = 0;
          IDS_HDT_CONSOLE (PCIE_MISC, "Training failed on Device %x Function %x\n",
                           PcieEngineList->Type.Port.PortData.DeviceNumber,
                           PcieEngineList->Type.Port.PortData.FunctionNumber
                           );
        } else {
          PcieEngineList->InitStatus = 1;
          IDS_HDT_CONSOLE (PCIE_MISC, "Training SUCCESS on Device %x Function %x\n",
                           PcieEngineList->Type.Port.PortData.DeviceNumber,
                           PcieEngineList->Type.Port.PortData.FunctionNumber
                           );
          PcieEngineList->InitStatus = INIT_STATUS_PCIE_TRAINING_SUCCESS;
          if (PcdGet8 (PcdPcieLinkTrainingType) == 1) {
            DxioSetOperatingSpeed (GnbHandle, PcieEngineList, DxioEnginePointer, UserCfgPortPointer, SmuServicesPpi);
          } else {
            if (PcieEngineList->Type.Port.PortId > 7) {
              NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LINK_STATUS_ADDRESS + ((PcieEngineList->Type.Port.PortId - 8) << 12), &LinkStatus.Value, 0);
              NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LINK_CNTL2_ADDRESS + ((PcieEngineList->Type.Port.PortId - 8) << 12), &LinkCntl2.Value, 0);
            } else {
              NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LINK_STATUS_ADDRESS + ((PcieEngineList->Type.Port.PortId) << 12), &LinkStatus.Value, 0);
              NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LINK_CNTL2_ADDRESS + ((PcieEngineList->Type.Port.PortId) << 12), &LinkCntl2.Value, 0);
            }
          }
        }
      }
    }
    DxioEnginePointer++;
    EngineId++;
  }
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * DXIO Update Pcie Data Structure
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @param[in]  ThisNode       Pointer to the platform descriptor for this node
 * @retval     AGESA_STATUS
 */
VOID
DxioUpdatePcieStatus (
  IN      GNB_HANDLE                *GnbHandle,
  IN      DXIO_COMPLEX_DESCRIPTOR   *UserConfig,
  IN      DXIO_PLATFORM_DESCRIPTOR  *ThisNode,
  IN OUT  DXIO_ENGINE_DESCRIPTOR    **EarlyTrain,
  IN OUT  UINT32                    *EarlyTrainAddress,
  IN OUT  PCIe_ENGINE_CONFIG        **EarlyTrainEngine

  )
{
  // DXIO Firmware Structures
  DXIO_PLATFORM_DESCRIPTOR  *DxioNodePlatformDesc;
  DXIO_ENGINE_DESCRIPTOR    *DxioEnginePointer;
  UINT32                    DxioNodeCount;
  //User Config Pointers
  DXIO_COMPLEX_DESCRIPTOR   *UserCfg;
  DXIO_PORT_DESCRIPTOR      *UserCfgPortList;              // Pointer to start of User Config for this socket
  DXIO_PORT_DESCRIPTOR      *UserCfgPortPointer;           // Updatable pointer for UserCfgPortList processing
  // PCIe Structures
  PCIe_ENGINE_CONFIG        *PcieEngineList;
  PCIe_WRAPPER_CONFIG       *PcieWrapper;

  UINT8                     SocketNumber;
  BOOLEAN                   EndOfList;
  UINT32                    Index;
  UINT8                     EngineId;
  UINT8                     PortDevMap[16];

  IDS_HDT_CONSOLE (PCIE_MISC, "DxioUpdatePcieStatus enter\n");

  LibAmdMemFill (PortDevMap, 0, sizeof (PortDevMap), (AMD_CONFIG_PARAMS *) NULL);

  // Get Socket Number
  SocketNumber = GnbHandle->SocketId;
  UserCfg = UserConfig;
  EndOfList = FALSE;
  UserCfgPortList = NULL;
  while (EndOfList != TRUE) {
    if (UserCfg->SocketId == SocketNumber) {
      UserCfgPortList = UserCfg->PciePortList;
    }
    if ((UserCfg->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
      UserCfg++;
    } else {
      EndOfList = TRUE;
    }
  }

  DxioNodePlatformDesc = ThisNode;
  DxioNodePlatformDesc ++;                                              // Point to first engine descriptor
  DxioEnginePointer = (DXIO_ENGINE_DESCRIPTOR *)DxioNodePlatformDesc;       // Initialize engine pointer to first engine
  DxioNodePlatformDesc --;                                              // Restore DxioNodePlatformDesc address

  DxioNodeCount = DxioNodePlatformDesc->NumEngineDesc;

  EngineId = 0;
  for (Index = 0;Index < DxioNodeCount;Index++) {
    //GnbLibDebugDumpBuffer ((VOID*) DxioEnginePointer, sizeof(DXIO_ENGINE_DESCRIPTOR), 1, 16);
    //IDS_HDT_CONSOLE (PCIE_MISC, "\n");
    DxioEnginePointer++;
  }

  DxioNodePlatformDesc = ThisNode;
  DxioNodePlatformDesc ++;                                              // Point to first engine descriptor
  DxioEnginePointer = (DXIO_ENGINE_DESCRIPTOR *)DxioNodePlatformDesc;       // Initialize engine pointer to first engine
  DxioNodePlatformDesc --;                                              // Restore DxioNodePlatformDesc address

  DxioNodeCount = DxioNodePlatformDesc->NumEngineDesc;

  for (Index = 0;Index < DxioNodeCount;Index++) {
    if (DxioEnginePointer->EngineType == DxioPcieEngine) {
      if (DxioEnginePointer->StartLane >= 16) {
        DxioEnginePointer->StartLane -= 4;
        DxioEnginePointer->EndLane -= 4;     ///@todo this value should be an offset defined in the wrapper
      }
      DxioEnginePointer->StartLane += GnbHandle->StartLane;
      DxioEnginePointer->EndLane += GnbHandle->StartLane;
      //DxioEntryConfigDump (DxioEnginePointer);
      // Find the UserConfig entry for this DXIO engine
      UserCfgPortPointer = UserCfgPortList;       // Start of the port list for this socket
      EndOfList = FALSE;
      while (EndOfList != TRUE) {
        if ((UserCfgPortPointer->EngineData.StartLane >= DxioEnginePointer->StartLane) &&
            (UserCfgPortPointer->EngineData.EndLane <= DxioEnginePointer->EndLane) &&
            (UserCfgPortPointer->EngineData.EngineType == DxioPcieEngine)) {
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
      // Find the PCIe wrapper for this DXIO engine
      PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
      EndOfList = FALSE;
      while (EndOfList != TRUE) {
        if ((UserCfgPortPointer->EngineData.StartLane >= (PcieWrapper->StartPhyLane + GnbHandle->StartLane)) &&
            (UserCfgPortPointer->EngineData.EndLane <= (PcieWrapper->EndPhyLane + GnbHandle->StartLane))) {
          EndOfList = TRUE;
        } else {
          PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
          DxioEnginePointer->LinkDesc.Pcie.MacPortID += 8;      ///@todo this should be port offset defined in the wrapper
          if (PcieWrapper == NULL) {
            ASSERT (FALSE);       // Something bad happened here @todo - handle error
          }
        }
      }

      // Find the PCIe structure entry for this DXIO engine
      PcieEngineList = PcieConfigGetChildEngine (PcieWrapper);
      EndOfList = FALSE;
      while (EndOfList != TRUE) {
        if (PcieEngineList->Type.Port.PortId == DxioEnginePointer->LinkDesc.Pcie.MacPortID) {
            EndOfList = TRUE;
        } else {
          PcieEngineList = PcieLibGetNextDescriptor (PcieEngineList);
          if (PcieEngineList == NULL) {
            ASSERT (FALSE);       // Something bad happened here @todo - handle error
          }
        }
      }

      // At this point we have aligned structures...
      // PcieEngineList points to the engine in the Pcie platform structure.
      // UserCfgPortPointer points to the port in UserConfig
      // DxioEnginePointer points to the engine structure in the DXIO structure
      // We can use this data to update the status of the engine in the Pcie platform
      if (PcieEngineList != NULL) {
        PcieEngineList->EngineData.EngineType = (UINT8) DxioEnginePointer->EngineType;
        PcieEngineList->EngineData.StartLane = (UINT16) DxioEnginePointer->StartLane;
        PcieEngineList->EngineData.EndLane = (UINT16) DxioEnginePointer->EndLane;
        if (DxioEnginePointer->LinkDesc.Pcie.Capabilities.earlyTrainLink == 1) {
          *EarlyTrain = DxioEnginePointer;
          *EarlyTrainAddress = DxioEnginePointer->LinkDesc.Pcie.DevFunNumber;
          *EarlyTrainEngine = PcieEngineList;
        }
        PcieEngineList->Type.Port.PortData.PortPresent = TRUE;
        // Byo Porting  >>  Add SlotNum
        PcieEngineList->Type.Port.PortData.SlotNum = UserCfgPortPointer->Port.SlotNum;
        // Byo Porting  <<
        PcieEngineList->Type.Port.PortData.DeviceNumber = UserCfgPortPointer->Port.DeviceNumber;
        PcieEngineList->Type.Port.PortData.FunctionNumber = UserCfgPortPointer->Port.FunctionNumber;
        PcieEngineList->Type.Port.PortData.LinkSpeedCapability = UserCfgPortPointer->Port.LinkSpeedCapability;
        PcieEngineList->Type.Port.PortData.LinkAspm = UserCfgPortPointer->Port.LinkAspm;
        if(PcdGetBool(PcdCfgPcieHotplugSupport) == FALSE) {
            UserCfgPortPointer->Port.LinkHotplug = DxioHotplugDisabled;
        }
        PcieEngineList->Type.Port.PortData.LinkHotplug = UserCfgPortPointer->Port.LinkHotplug;
        //IDS_HDT_CONSOLE (PCIE_MISC, "DxioEnginePointer->LinkHotplug = %d\n", PcieEngineList->Type.Port.PortData.LinkHotplug);
        PcieEngineList->Type.Port.PcieBridgeId = EngineId;                    ///@todo Workaround for speed set
        PcieEngineList->Type.Port.LinkAspmL1_1 = UserCfgPortPointer->Port.LinkAspmL1_1;
        PcieEngineList->Type.Port.LinkAspmL1_2 = UserCfgPortPointer->Port.LinkAspmL1_2;
        PcieEngineList->Type.Port.ClkReq = UserCfgPortPointer->Port.ClkReq;
        PcieEngineList->Type.Port.EqPreset = UserCfgPortPointer->Port.EqPreset;
        PcieEngineList->Type.Port.PortData.MiscControls.ClkPmSupport = UserCfgPortPointer->Port.MiscControls.ClkPmSupport;
        //IDS_HDT_CONSOLE (PCIE_MISC, "Device 0:%d:%d ClkPmSupport = %d\n", PcieEngineList->Type.Port.PortData.DeviceNumber,
        //                 PcieEngineList->Type.Port.PortData.FunctionNumber,
        //                 PcieEngineList->Type.Port.PortData.MiscControls.ClkPmSupport);
        PcieConfigSetDescriptorFlags (PcieEngineList, DESCRIPTOR_ALLOCATED);
        PcieMapPortPciAddress (PcieEngineList, PortDevMap);
        PcieEngineList->Type.Port.Address.AddressValue = MAKE_SBDFO (
                                             0,
                                             GnbHandle->Address.Address.Bus,
                                             PcieEngineList->Type.Port.PortData.DeviceNumber,
                                             PcieEngineList->Type.Port.PortData.FunctionNumber,
                                             0
                                             );
        //PcieConfigEngineDebugDump (PcieEngineList);
      }
    }
    DxioEnginePointer++;
    EngineId++;
  }
  PcieSetPortPciAddressMap (GnbHandle, PortDevMap);
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * DXIO User Config Parser
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
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
DxioParseUserConfig (
  IN       CONST EFI_PEI_SERVICES    **PeiServices,
  IN       GNB_HANDLE                *GnbHandle,
  IN       DXIO_COMPLEX_DESCRIPTOR   *UserConfig,
  OUT      DXIO_PLATFORM_DESCRIPTOR  **ThisNode,
  OUT      UINT32                    *NodeSize,
  OUT      UINT8                     **AncData,
  OUT      UINT32                    *AncDataSize
  )
{
  AGESA_STATUS               AgesaStatus;
  UINT8                      SocketNumber;
  UINT8                      InstanceId;
  UINT16                     NodeMinLane;
  UINT16                     NodeMaxLane;
  // DXIO Firmware Structures
  DXIO_PLATFORM_DESCRIPTOR   *NodePlatformDesc;
  DXIO_ENGINE_DESCRIPTOR     *EnginePointer;
  UINT32                     ThisNodeCount;
  UINT32                     ThisNodeSize;
  // Ancillary Data Structures
  UINT32                     DataSize;
  UINT8                      *AncillaryData;
  UINT32                     AncillaryDataSize;
  UINT32                     AncillaryDataOffset;
  ETHERNET_PORT_DATA         *EtherNetPortHeap;
  UINT32                     EtherNetPortOffset;
  DXIO_ANCILLARYDATA         *DxioAncData;
  DXIO_ANCILLARYDATA_SUBHEAP *DxioSubHeapData;

  UINT32                     EtherNetPortCount;
  //User Config Pointers
  DXIO_COMPLEX_DESCRIPTOR    *UserCfg;
  DXIO_PORT_DESCRIPTOR       *PortList;              // Pointer to start of User Config for this socket
  DXIO_PORT_DESCRIPTOR       *PortPointer;           // Updatable pointer for PortList processing
  BOOLEAN                    EndOfList;
  BOOLEAN                    SataEnabled;

  // Get Socket Number
  SocketNumber = GnbHandle->SocketId;
  InstanceId = GnbHandle->InstanceId;
  // Get Min/Max lane number for this node
  NodeMinLane = GnbHandle->StartLane;
  NodeMaxLane = GnbHandle->EndLane;

  SataEnabled = FALSE;
  AgesaStatus = AGESA_SUCCESS;
  //IDS_HDT_CONSOLE (PCIE_MISC, "Evaluating ports for Lane %d through %d\n", NodeMinLane, NodeMaxLane);
  // Locate socket-specific structure
  PortList = NULL;
  //IDS_HDT_CONSOLE (PCIE_MISC, "Looking for SocketNumber %d \n", SocketNumber);
  EndOfList = FALSE;
  UserCfg = UserConfig;
  while (EndOfList != TRUE) {
    //IDS_HDT_CONSOLE (PCIE_MISC, " This list is for socket %d\n", UserCfg->SocketId);
    if (UserCfg->SocketId == SocketNumber) {
      PortList = UserCfg->PciePortList;
    }
    if ((UserCfg->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
      UserCfg++;
    } else {
      EndOfList = TRUE;
    }
  }
  // Parse structure and count entries for this node
  if (PortList != NULL) {
    PortPointer = PortList;
    ThisNodeSize = sizeof (DXIO_PLATFORM_DESCRIPTOR) + 8;  // Include 2 reserved UINT32 at the end
    ThisNodeCount = 0;
    EtherNetPortCount = 0;
    EtherNetPortOffset = 0;
    EtherNetPortHeap = NULL;
    AncillaryDataSize = 0;
    AncillaryDataOffset = NULL;
    EndOfList = FALSE;
    EnginePointer = NULL;

    // Calculate size of Platform Structure and Ancillary Data for this node
    do {
      if (((PortPointer->EngineData.StartLane >= NodeMinLane) && (PortPointer->EngineData.StartLane <= NodeMaxLane)) &&
          (PortPointer->EngineData.EngineType != DxioUnusedEngine)) {
        ThisNodeSize += sizeof (DXIO_ENGINE_DESCRIPTOR);
        ThisNodeCount++;
        if (PortPointer->EngineData.EngineType == DxioEthernetEngine) {
          EtherNetPortCount++;
        } else {
          AncillaryDataSize += DxioAncDataCalcSize (PortPointer);
          //IDS_HDT_CONSOLE (PCIE_MISC, "This port is EtherNet, new count is %d and new AncillaryDataSize is 0x%x\n", ThisNodeCount, AncillaryDataSize);
        }
      }
      if ((PortPointer->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
        PortPointer++;
      } else {
        EndOfList = TRUE;
      }
    } while (EndOfList != TRUE);
    if (EtherNetPortCount != 0) {
      AncillaryDataSize += (4 * sizeof (ETHERNET_PORT_DATA)) + sizeof (DXIO_ANCILLARYDATA_SUBHEAP);
    }
    if (AncillaryDataSize != 0) {
      AncillaryDataSize += sizeof (DXIO_HEAPDATA_CONTAINER) + sizeof (DXIO_ANCILLARYDATA);
    }

    // Build data structures for this node if there is any configuration required
    if (ThisNodeCount != 0) {
      // Allocate memory for this node platform structure
      AgesaStatus = (*PeiServices)->AllocatePool (
                                      PeiServices,
                                      ThisNodeSize,
                                      (VOID **)&NodePlatformDesc
                               );
      //ASSERT_EFI_ERROR (AgesaStatus);
      *ThisNode = NodePlatformDesc;             // Save pointer to allocated memory
      *NodeSize = (ThisNodeSize + 3) >> 2;      // Platfom Descriptor size in dwords


      // Allocate memory for this node ancillary data
      AncillaryData = NULL;
      if (AncillaryDataSize != 0) {
        AgesaStatus = (*PeiServices)->AllocatePool (
                                        PeiServices,
                                        AncillaryDataSize,
                                        (VOID **)&AncillaryData
                                 );
        if (AgesaStatus == AGESA_SUCCESS) {
          *AncData = AncillaryData;                          // Save pointer to allocated memory
          *AncDataSize = AncillaryDataSize >> 2;             // Ancillary Data size in dwords

          LibAmdMemFill (AncillaryData, 0, AncillaryDataSize, (AMD_CONFIG_PARAMS *) NULL);

          DxioAncData = (DXIO_ANCILLARYDATA *) AncillaryData;
          DxioAncData->descriptorType = 0;   ///@todo
          DxioAncData->version = DXIO_ANCILLARYDATA_VERSION;
          DxioAncData->numDWords = (AncillaryDataSize >> 2) -1;
          AncillaryData += sizeof (DXIO_ANCILLARYDATA);

          AncillaryDataOffset = sizeof (DXIO_ANCILLARYDATA);

          if (EtherNetPortCount != 0) {
            DxioSubHeapData = (DXIO_ANCILLARYDATA_SUBHEAP *) AncillaryData;
            DxioSubHeapData->descriptorType = DXIO_ANCDATA_SUBHEAPTYPE_XGBE;
            DxioSubHeapData->version = DXIO_ANCILLARYDATA_SUBHEAP_VERSION;
            DxioSubHeapData->numDWORDs = ((4 * sizeof (ETHERNET_PORT_DATA)) + sizeof (DXIO_ANCILLARYDATA_SUBHEAP)) >> 2;
            AncillaryData += sizeof (DXIO_ANCILLARYDATA_SUBHEAP);
            EtherNetPortHeap = (ETHERNET_PORT_DATA *) AncillaryData;
            EtherNetPortOffset = AncillaryDataOffset;
            AncillaryData += (4 * sizeof (ETHERNET_PORT_DATA));
            AncillaryDataOffset += (4 * sizeof (ETHERNET_PORT_DATA)) + sizeof (DXIO_ANCILLARYDATA_SUBHEAP);
          }
        } else {
          *AncData = (UINT8 *) NULL;
          *AncDataSize = 0;
        }
      } else {
        *AncData = (UINT8 *) NULL;
        *AncDataSize = 0;
      }

      // Initialize platform level entries for this structure
      LibAmdMemFill (NodePlatformDesc, 0, ThisNodeSize, (AMD_CONFIG_PARAMS *) NULL);
      NodePlatformDesc->PlatformType = ZP_PLATFORM_TYPE;
      NodePlatformDesc->NumEngineDesc = ThisNodeCount;
      // Parse structure and copy entries for this node to this node platform structure
      PortPointer = PortList;
      NodePlatformDesc ++;                    // Point to first engine descriptor
      EnginePointer = (DXIO_ENGINE_DESCRIPTOR *)NodePlatformDesc;       // Initialize engine pointer to first engine
      NodePlatformDesc --;                    // Restore NodePlatformDesc address
      AncillaryDataSize = 0;
      EndOfList = FALSE;
      do {
        //IDS_HDT_CONSOLE (PCIE_MISC, "This port starts at lane %d and ends at lane %d\n", PortPointer->EngineData.StartLane,  PortPointer->EngineData.EndLane);
        if (((PortPointer->EngineData.StartLane >= NodeMinLane) && (PortPointer->EngineData.StartLane <= NodeMaxLane)) &&
          (PortPointer->EngineData.EngineType != DxioUnusedEngine)) {
          if ((PortPointer->EngineData.StartLane < NodeMinLane) || (PortPointer->EngineData.StartLane > NodeMaxLane)) {  ///egh fix this
            //Invalid configuration
            ASSERT (FALSE);
          }
          if (PortPointer->EngineData.StartLane <= PortPointer->EngineData.EndLane) {
            EnginePointer->StartLane = PortPointer->EngineData.StartLane - (NodeMinLane);
            EnginePointer->EndLane = PortPointer->EngineData.EndLane - (NodeMinLane);
          } else {
            EnginePointer->EndLane = PortPointer->EngineData.StartLane - (NodeMinLane);
            EnginePointer->StartLane = PortPointer->EngineData.EndLane - (NodeMinLane);
          }
          ///@todo DXIO expects lanes 20-35 for PCIe1, but we are using 16-31.  Fix offset here
          if (EnginePointer->StartLane >= 16) {
            EnginePointer->StartLane +=4;
            EnginePointer->EndLane +=4;
          }
          EnginePointer->GpioGroupID = 1; //Unused
          EnginePointer->engineResetID = PortPointer->EngineData.GpioGroupId;
          EnginePointer->EngineType = PortPointer->EngineData.EngineType;
          EnginePointer->HotPluggable = PortPointer->EngineData.HotPluggable;
          switch (PortPointer->EngineData.EngineType) {
          case DxioPcieEngine:
            EnginePointer->LinkDesc.Pcie.Capabilities.earlyTrainLink = PortPointer->Port.MiscControls.SbLink;

            if (PcdGet8 (PcdPcieLinkTrainingType) == 0) {
              IDS_HDT_CONSOLE (GNB_TRACE, "Train single step\n");
              if (EnginePointer->LinkDesc.Pcie.Capabilities.earlyTrainLink == 1) {
                EnginePointer->LinkDesc.Pcie.Capabilities.maxLinkSpeedCap = DxioGen1;
              } else {
                EnginePointer->LinkDesc.Pcie.Capabilities.maxLinkSpeedCap = PortPointer->Port.LinkSpeedCapability;
              }
            } else {
              IDS_HDT_CONSOLE (GNB_TRACE, "Train two step\n");
              EnginePointer->LinkDesc.Pcie.Capabilities.maxLinkSpeedCap = DxioGen1;
            }
            
            /* PS-252, mask ASPM L0S*/
            PortPointer->Port.LinkAspm = PortPointer->Port.LinkAspm & 0x02;
            
            EnginePointer->LinkDesc.Pcie.DevFunNumber = (PortPointer->Port.DeviceNumber << 3) | PortPointer->Port.FunctionNumber;
            EnginePointer->LinkDesc.Pcie.Capabilities.enableTurnOffSettings = 1;
            EnginePointer->LinkDesc.Pcie.Capabilities.turnOffUnusedLanes = PortPointer->Port.MiscControls.TurnOffUnusedLanes;
            EnginePointer->LinkDesc.Pcie.Capabilities.portPresent = PortPointer->Port.PortPresent;
            if(PcdGetBool(PcdCfgPcieHotplugSupport) == FALSE) {
                PortPointer->Port.LinkHotplug = DxioHotplugDisabled;
            }
            EnginePointer->LinkDesc.Pcie.Capabilities.linkHotplug = PortPointer->Port.LinkHotplug;
            EnginePointer->LinkDesc.Pcie.Capabilities.autoSpeedChangeCntrl = PortPointer->Port.AutoSpdChng;

            if (PcdGet8(PcdPcieEqSearchMode) != 1) {
              EnginePointer->LinkDesc.Pcie.Capabilities.eqSearchModeOride = 1;
              EnginePointer->LinkDesc.Pcie.Capabilities.eqSearchMode =
                  PcdGet8 (PcdPcieEqSearchMode);
              IDS_HDT_CONSOLE (GNB_TRACE, "EqSearchMode: %d\n",
                  EnginePointer->LinkDesc.Pcie.Capabilities.eqSearchMode);
            }

            if (PortPointer->Port.LinkHotplug == DxioHotplugServerEntSSD) {
              EnginePointer->LinkDesc.Pcie.Capabilities.linkHotplug = DxioHotplugServerExpress;
            }
            if (PortPointer->EngineData.StartLane > PortPointer->EngineData.EndLane) {
              EnginePointer->LinkDesc.Pcie.Capabilities.linkReversal = 1;
            }
            DataSize = DxioAncDataPopulate (EnginePointer, PortPointer, AncillaryData);
            if (0 != DataSize) {
              EnginePointer->LinkDesc.Pcie.AncillaryDataOffset = AncillaryDataOffset >> 2;
              AncillaryData += DataSize;
              AncillaryDataOffset += DataSize;
            }
            break;
          case DxioSATAEngine:
            EnginePointer->LinkDesc.Sata.ChannelType = PortPointer->Port.MiscControls.ChannelType;
            SataEnabled = TRUE;
            DataSize = DxioAncDataPopulate (EnginePointer, PortPointer, AncillaryData);
            if (0 != DataSize) {
              EnginePointer->LinkDesc.Sata.AncillaryDataOffset = AncillaryDataOffset >> 2;
              AncillaryData += DataSize;
              AncillaryDataOffset += DataSize;
            }
            break;
          case DxioDPEngine:
            break;
          case DxioEthernetEngine:
            NbioEnableNbifDevice (GnbHandle, DxioEthernetEngine, (UINT8) EnginePointer->StartLane);
            EnginePointer->LinkDesc.Eth.AncillaryDataOffset = EtherNetPortOffset >> 2;
            EtherNetPortHeap += PortPointer->EtherNet.EthPortProp0.PortNum;
            LibAmdMemCopy ((VOID *) EtherNetPortHeap,
                           (VOID *) &PortPointer->EtherNet,
                           sizeof (ETHERNET_PORT_DATA),
                           (AMD_CONFIG_PARAMS *)NULL
                           );
            EtherNetPortHeap -= PortPointer->EtherNet.EthPortProp0.PortNum;
            break;
          case DxioGOPEngine:
            break;
          default:
            break;
          }
          //DxioEntryConfigDump (EnginePointer);
          EnginePointer++;
        }
        if ((PortPointer->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
          PortPointer++;
        } else {
          EndOfList = TRUE;
        }
      } while (EndOfList != TRUE);
    } else {
      // Nothing found for this node, skip DXIO call
      *ThisNode = NULL;             // Save pointer to allocated memory
      *NodeSize = 0;      // Platfom Descriptor size in dwords
    }
    if ((EnginePointer != NULL) && (SataEnabled == TRUE)) {
      NbioEnableNbifDevice (GnbHandle, DxioSATAEngine, (UINT8) EnginePointer->StartLane);
    } else {
      if (!PcdGetBool (PcdAmdSataEnable)) {
        NbioDisableNbifDevice (GnbHandle, DxioSATAEngine, 0);
      }
    }
  }
  // Return pointer to this node platform structure
  return (AgesaStatus);
}
/*----------------------------------------------------------------------------------------*/
/**
 * SET HWDEBUG REG
 * @param[in]  PeiServices    Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @retval     AGESA_STATUS
 */
AGESA_STATUS
SetHwdbgReg(
    IN       CONST EFI_PEI_SERVICES    **PeiServices,
    IN       GNB_HANDLE                *GnbHandle,
    IN       DXIO_COMPLEX_DESCRIPTOR   *UserConfig
 )
{
    AGESA_STATUS               AgesaStatus;
    UINT32                     Data32;
    UINT8                      SocketNumber;
    UINT8                      InstanceId;
    UINT16                     NodeMinLane;
    UINT16                     NodeMaxLane;
    BOOLEAN                    EndOfList;
    UINT32                     AncillaryDataSize=0;
    DXIO_COMPLEX_DESCRIPTOR    *UserCfg;
    DXIO_PORT_DESCRIPTOR       *PortList;
    DXIO_PORT_DESCRIPTOR       *PortPointer; 
    UINT16                      EngineStartLan,EngineEndLan;
    UINT16                      i;
    UINT32                     HwDbgSmnAddress;                        
    Data32 = 0;
    
    IDS_HDT_CONSOLE (PCIE_MISC, "SetHwdbgReg start\n");
    
    NbioRegisterWrite (GnbHandle,TYPE_SMN,0x1220B7DC,&Data32,0);
    NbioRegisterWrite (GnbHandle,TYPE_SMN,0x1230B7DC,&Data32,0);
    
    SocketNumber = GnbHandle->SocketId;
    InstanceId = GnbHandle->InstanceId;
    // Get Min/Max lane number for this node
    NodeMinLane = GnbHandle->StartLane;
    NodeMaxLane = GnbHandle->EndLane;

    AgesaStatus = AGESA_SUCCESS;
    PortList = NULL;
    EndOfList = FALSE;
    UserCfg = UserConfig;
  
    while (EndOfList != TRUE) {
      if (UserCfg->SocketId == SocketNumber) {  
        PortList = UserCfg->PciePortList;
      }
      if ((UserCfg->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
        UserCfg++;
      } else {
        EndOfList = TRUE;
      }
     }
    
    EndOfList = FALSE;
     
    if (PortList != NULL) {
        PortPointer = PortList;
        do {
             if (((PortPointer->EngineData.StartLane >= NodeMinLane) && (PortPointer->EngineData.StartLane <= NodeMaxLane)) &&
                 (PortPointer->EngineData.EngineType != DxioUnusedEngine)) {
                 if (PortPointer->EngineData.EngineType == DxioPcieEngine) {
                        AncillaryDataSize = DxioAncDataCalcSize (PortPointer);
                        if(AncillaryDataSize!=0){
                            AncillaryDataSize = 0;
                            if(PortPointer->EngineData.StartLane <= PortPointer->EngineData.EndLane){
                                EngineStartLan = PortPointer->EngineData.StartLane - NodeMinLane;
                                EngineEndLan = PortPointer->EngineData.EndLane - NodeMinLane;
                            } else {
                                EngineStartLan = PortPointer->EngineData.EndLane - NodeMinLane;
                                EngineEndLan = PortPointer->EngineData.StartLane - NodeMinLane;
                            }
                            HwDbgSmnAddress = (EngineStartLan<16)? 0x1220B7DC : 0x1230B7DC;                           
                            for(i=EngineStartLan;i<=EngineEndLan;i++)
                            {
                                //write HwDbg Reg
                                NbioRegisterRMW(GnbHandle,TYPE_SMN,HwDbgSmnAddress,0xFFFFFFFF,1<<(i%16),0);  
                            }
                        }
                }
             }
            if ((PortPointer->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
                      PortPointer++;
                    } else {
                      EndOfList = TRUE;
                    }
         }while(EndOfList != TRUE);
     }
     IDS_HDT_CONSOLE (PCIE_MISC, "SetHwdbgReg end\n");
     return AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * DXIO Process Results
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @param[in]  ThisNode       Pointer to the platform descriptor for this node
 * @param[in]  ThisNodeSize   Size of the platform descriptor for this node
 * @param[in]  SmuServicesPpi Pointer to SMU Services PPI or similar entry point structure
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
DxioProcessResultsV1 (
  IN       GNB_HANDLE                      *GnbHandle,
  IN       DXIO_COMPLEX_DESCRIPTOR         *UserConfig,
  IN       DXIO_PLATFORM_DESCRIPTOR        *ThisNode,
  IN       UINT32                          ThisNodeSize,
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI   *SmuServicesPpi
  )
{
  AGESA_STATUS              AgesaStatus;
  UINT32                    SmuArg[6];
  UINT8                     DxioRetval;
  BOOLEAN                   DxioComplete;
  DXIO_RETURN_PARAMS        *ReturnArgs;
  LISM_RETURN_PARAMS        *LismRetParams;
  RESET_REQUEST_PARAMS      *ResetReqParams;
  GPIO_RESET_INFO           GpioResetInfo;
  PCI_ADDR                  EarlyTrainAddress;
  PCI_ADDR                  NbioAddress;
  DXIO_ENGINE_DESCRIPTOR    *EarlyTrainDesc;
  PCIe_ENGINE_CONFIG        *EarlyTrainEngine;
  BOOLEAN                   PcieStatusUpdated;
  PCIE_LC_STATE0_STRUCT     LcState0;
  GNB_HANDLE                *ResetHandle;

  // Process results

  AgesaStatus = AGESA_SUCCESS;
  ReturnArgs = (DXIO_RETURN_PARAMS*) &SmuArg[0];
  PcieStatusUpdated = FALSE;
  DxioComplete = FALSE;
  EarlyTrainAddress.AddressValue = 0;
  EarlyTrainDesc = NULL;
  EarlyTrainEngine = NULL;
  do {
    LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
    SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_LISM_COLLECTRESULTS, SmuArg, SmuArg);
    DxioRetval = (UINT8) (SmuArg[0] & 0xFF);
    switch (DxioRetval) {
    case DXIO_MBOX_RETVAL_OK:
      switch (ReturnArgs->RetParams_Type) {
      case DXIO_MBOX_RETPARAMS_LISM:
        // Process return parameters
        LismRetParams = (LISM_RETURN_PARAMS*) &SmuArg[2];
        IDS_HDT_CONSOLE (GNB_TRACE, "Current State = 0x%x\n", LismRetParams->CurrentState);
        switch (LismRetParams->CurrentState) {
        case DXIO_LinkInitState_done:
            DxioComplete = TRUE;
            break;
        case DXIO_LinkInitState_mapped:
          // Ports are mapped.  Allow pre-configuration options to be programmed.
          IDS_HDT_CONSOLE (GNB_TRACE, "PORTS ARE MAPPED\n");
          LibAmdMemFill ((VOID *)ThisNode, 0x00, ThisNodeSize << 2, (AMD_CONFIG_PARAMS *) NULL);
          LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
          SmuArg[2] = (UINT32) ThisNode;
          SmuArg[3] = ThisNodeSize;
          SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_INIT_DMAENGINECONIFGTODRAM, SmuArg, SmuArg);
//          DxioEngineMapping (GnbHandle, UserConfig, ThisNode);
          EarlyTrainDesc = NULL;
          DxioUpdatePcieStatus (GnbHandle,
                                UserConfig,
                                ThisNode,
                                (DXIO_ENGINE_DESCRIPTOR**)&EarlyTrainDesc,
                                &EarlyTrainAddress.AddressValue,
                                (PCIe_ENGINE_CONFIG **) &EarlyTrainEngine
                                );
          DxioCfgBeforeReconfig (GnbHandle);
          break;
        case DXIO_LinkInitState_configured:
          // Configuration is done.  Allow post-configuration, pre-training options to be programmed.
          IDS_HDT_CONSOLE (GNB_TRACE, "RECONFIG IS COMPLETE\n");
          DxioCfgAfterReconfig (GnbHandle);
          break;
        case DXIO_LinkInitState_vetting:
          if (PcdGetBool (PcdPcieSyncReset)) {
            // Set DxioComplete for syncing all dies in multi-die config
            DxioComplete = TRUE;
          }
        case DXIO_LinkInitState_earlyTrained:
          // Read back training results
          IDS_HDT_CONSOLE (GNB_TRACE, "Vetting or earlyTrained state detected...\n");
          if (PcieStatusUpdated != TRUE) {
            if (EarlyTrainDesc == NULL) {
              IDS_HDT_CONSOLE (GNB_TRACE, "EarlyTrainLink NOT FOUND\n");
            } else {
              IDS_HDT_CONSOLE (GNB_TRACE, "\nEarlyTrainLink found at 0x%x\n", EarlyTrainAddress);
              PcieStatusUpdated = TRUE;
              LibAmdMemFill ((VOID *)ThisNode, 0x00, ThisNodeSize << 2, (AMD_CONFIG_PARAMS *) NULL);
              LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
              SmuArg[2] = (UINT32) ThisNode;
              SmuArg[3] = ThisNodeSize;
              SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_INIT_DMAENGINECONIFGTODRAM, SmuArg, SmuArg);
              EarlyTrainAddress.AddressValue = EarlyTrainAddress.AddressValue << 12;
              NbioAddress = NbioGetHostPciAddress (GnbHandle);
              if (EarlyTrainEngine != NULL) {
                if (EarlyTrainEngine->Type.Port.PortId > 7) {
                  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_STATE0_ADDRESS + ((EarlyTrainEngine->Type.Port.PortId - 8) << 12), &LcState0.Value, 0);
                } else {
                  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_STATE0_ADDRESS + (EarlyTrainEngine->Type.Port.PortId << 12), &LcState0.Value, 0);
                }
                IDS_HDT_CONSOLE (PCIE_MISC, "LcState0 for earlyTrainLink is 0x%x\n", LcState0.Value);
                if ((LcState0.Field.LC_CURRENT_STATE >= 0x10) ||
                    (LcState0.Field.LC_PREV_STATE1 >= 0x10) ||
                    (LcState0.Field.LC_PREV_STATE2 >= 0x10) ||
                    (LcState0.Field.LC_PREV_STATE3 >= 0x10)) {
                  // earlyTrainLink is trained - report to PSP
                  EarlyTrainAddress.AddressValue |= NbioAddress.AddressValue;
                  GnbLibPciRMW (EarlyTrainAddress.AddressValue | 0x18, AccessWidth32, 0xFF000FF, 0x020200, (AMD_CONFIG_PARAMS *) NULL);

                  LibAmdMemFill ((VOID *)ThisNode, 0x00, ThisNodeSize << 2, (AMD_CONFIG_PARAMS *) NULL);
                  LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
                  SmuArg[2] = (UINT32) ThisNode;
                  SmuArg[3] = ThisNodeSize;
                  SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_INIT_DMAENGINECONIFGTODRAM, SmuArg, SmuArg);
                  EarlyTrainDesc->LinkDesc.Pcie.DevFunNumber = 0x200;
                  IDS_HDT_CONSOLE (GNB_TRACE, "Sending this Platform Descriptor back to SMU...\n");
                  GnbLibDebugDumpBuffer ((VOID*) ThisNode, ThisNodeSize << 2, 1, 16);
                  IDS_HDT_CONSOLE (GNB_TRACE, "\n");
                  LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
                  SmuArg[1] = 0;
                  SmuArg[2] = (UINT32) ThisNode;
                  SmuArg[3] = ThisNodeSize;
                  SmuArg[5] = DXIO_HEAPTYPE_CFGPTR;
                  SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);
                }
              }
            }
          }
          break;
        default:
          break;
        }
        break;
      case DXIO_MBOX_RETPARAMS_REQ_RESET:
        // Issue reset request for specific PCIe Engine
        ResetReqParams = (RESET_REQUEST_PARAMS*) &SmuArg[2];
        GpioResetInfo.ResetId = ResetReqParams->GpioId;
        GpioResetInfo.ResetControl = ResetReqParams->resetAssertDeassert;
        if (PcdGetBool (PcdPcieSyncReset)) {
	        if (ResetReqParams->resetAssertDeassert == 0) {
	          if (GnbHandle->Address.Address.Bus == 0) {
	            ResetHandle = GnbHandle;
	            while (ResetHandle != NULL) {
	              AgesaGpioSlotResetControl((UINTN)ResetHandle->Address.Address.Bus, &GpioResetInfo);
	              IDS_HDT_CONSOLE (GNB_TRACE, "Reset Assert Request for GpioId 0x%x on Bus 0x%x\n", ResetReqParams->GpioId, ResetHandle->Address.Address.Bus);
	              ResetHandle = GnbGetNextHandle (ResetHandle);
	            }
	          }
	        } else {
	          if (GnbHandle->Address.Address.Bus == 0) {
	            ResetHandle = GnbHandle;
	            while (ResetHandle != NULL) {
	              AgesaGpioSlotResetControl((UINTN)ResetHandle->Address.Address.Bus, &GpioResetInfo);
	              IDS_HDT_CONSOLE (GNB_TRACE, "Reset Deassert Request for GpioId 0x%x on Bus 0x%x\n", ResetReqParams->GpioId, ResetHandle->Address.Address.Bus);
	              ResetHandle = GnbGetNextHandle (ResetHandle);
	            }
	          }
	        }
        } else {
	        if (ResetReqParams->resetAssertDeassert == 0) {
	          AgesaGpioSlotResetControl ((UINTN) GnbHandle->Address.Address.Bus, &GpioResetInfo);
	          IDS_HDT_CONSOLE (GNB_TRACE, "Reset Assert Request for GpioId 0x%x\n", ResetReqParams->GpioId);
	        } else {
	          AgesaGpioSlotResetControl ((UINTN) GnbHandle->Address.Address.Bus, &GpioResetInfo);
	          IDS_HDT_CONSOLE (GNB_TRACE, "Reset Deassert Request for GpioId 0x%x\n", ResetReqParams->GpioId);
	        }
        }
		
        break;
      case DXIO_MBOX_RETPARAMS_NONE:
        DxioComplete = TRUE;
        break;
      case DXIO_MBOX_RETPARAMS_GENERIC:
        // Process return parameters
        break;
      default:
        IDS_HDT_CONSOLE (GNB_TRACE, "Invalid return parameter [0x%x]\n", DxioRetval);
        ASSERT (FALSE);
        break;
      }
      break;
    case DXIO_MBOX_RETVAL_ERROR:
      switch (ReturnArgs->RetParams_Type) {
      case DXIO_MBOX_RETPARAMS_LISM:
        LismRetParams = (LISM_RETURN_PARAMS*) &SmuArg[2];
        IDS_HDT_CONSOLE (GNB_TRACE, "Current State = 0x%x", LismRetParams->CurrentState);
        AgesaStatus = AGESA_CRITICAL;
        DxioComplete = TRUE;
        break;
      default:
        ASSERT (FALSE);
        break;
      }
      break;
    default:
      IDS_HDT_CONSOLE (GNB_TRACE, "DXIO Mailbox Request did not return OK [0x%x]\n", DxioRetval);
      ASSERT (FALSE);
      DxioComplete = TRUE;
      break;
    }
    if (FALSE == DxioComplete) {
      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_LISM_RESUME, SmuArg, SmuArg);
    }
  } while (FALSE == DxioComplete);
  if ((PcieStatusUpdated != TRUE) && (EarlyTrainDesc != NULL)) {
    GnbLibPciRMW (EarlyTrainAddress.AddressValue | 0x18, AccessWidth32, 0xFF000FF, 0x000000, (AMD_CONFIG_PARAMS *) NULL);
  }
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Concurrent Training
 *
 *
 * @param[in]  PeiServices     Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  SmuServicesPpi  Pointer to SMU Services PPI or similar entry point structure
 * @param[in]  Pcie            Pointer to the AGESA platfom complex
 * @param[in]  UserConfig      Pointer to the platform BIOS supplied platform configuration
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
DxioConcurrentTraining (
  IN       CONST EFI_PEI_SERVICES          **PeiServices,
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI   *SmuServicesPpi,
  IN       PCIe_PLATFORM_CONFIG            *Pcie,
  IN       DXIO_COMPLEX_DESCRIPTOR         *UserConfig
  )
{
  AGESA_STATUS              AgesaStatus;
  AGESA_STATUS              Status;
  GNB_HANDLE                *GnbHandle;
  UINT32                    SmuArg[6];
  UINT8                     InstanceId;
  UINT8                     DxioRetval;
  DXIO_PLATFORM_DESCRIPTOR  *PlatformDesc[MAX_INSTANCE_ID];
  UINT32                    PlatformDescSize[MAX_INSTANCE_ID];
  UINT8                     *AncillaryData[MAX_INSTANCE_ID];
  UINT32                    AncillaryDataSize[MAX_INSTANCE_ID];

  AgesaStatus = AGESA_SUCCESS;

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    InstanceId = GnbHandle->InstanceId;
    DxioParseUserConfig (PeiServices,
                         GnbHandle,
                         UserConfig,
                         &PlatformDesc[InstanceId],
                         &PlatformDescSize[InstanceId],
                         &AncillaryData[InstanceId],
                         &AncillaryDataSize[InstanceId]
                         );
    
    SetHwdbgReg(PeiServices,GnbHandle,UserConfig); 
    
    if (PlatformDesc[InstanceId] != NULL) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Platform Descriptor for Die %d\n", InstanceId);
      DxioDebugDump (PlatformDesc[InstanceId],
                     PlatformDescSize[InstanceId],
                     AncillaryData[InstanceId],
                     AncillaryDataSize[InstanceId]);
    }

    if (InstanceId == UserConfig->BmcLinkLocation) {
      LibAmdMemFill(SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *)NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_LISM_RELOAD, SmuArg, SmuArg);
    }

    LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
    SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_INIT, SmuArg, SmuArg);
    DxioRetval = (UINT8) (SmuArg[0] & 0xFF);
    if (DxioRetval != DXIO_MBOX_RETVAL_OK) {
      PlatformDesc[InstanceId] = NULL;
    }

    if (PlatformDesc[InstanceId] != NULL) {

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_GETVERSION, SmuArg, SmuArg);

        LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
        SmuArg[1] = DBUG_SETSCRATCHFLAG_RETURNAFTERMAPPING;
        SmuArg[2] = 1;
        SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_SETSCRATCH, SmuArg, SmuArg);

        LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
        SmuArg[1] = DBUG_SETSCRATCHFLAG_RETURNAFTERCONFIG;
        SmuArg[2] = 1;
        SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_SETSCRATCH, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[1] = DBUG_SETSCRATCHFLAG_V1ANCILLARYDATA;
      SmuArg[2] = 1;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_SETSCRATCH, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADCAPABILITIES, SmuArg, SmuArg);

      if (AncillaryDataSize[InstanceId] != 0) {
        LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
        SmuArg[1] = 0;
        SmuArg[2] = (UINT32) AncillaryData[InstanceId];
        SmuArg[3] = AncillaryDataSize[InstanceId];
        SmuArg[5] = DXIO_HEAPTYPE_ANCDATA;
        SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);
      }

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[4] = 1;
      SmuArg[5] = DXIO_HEAPTYPE_MACPCS;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[4] = 1;
      SmuArg[5] = DXIO_HEAPTYPE_GPIOPTR;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[1] = 0;
      SmuArg[2] = (UINT32) PlatformDesc[InstanceId];
      SmuArg[3] = PlatformDescSize[InstanceId];
      SmuArg[5] = DXIO_HEAPTYPE_CFGPTR;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);

      // Inititiate configuration
      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_LISM_GO, SmuArg, SmuArg);
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    InstanceId = GnbHandle->InstanceId;
    if (PlatformDesc[InstanceId] != NULL) {
      // Process results
      Status = DxioProcessResultsV1 (GnbHandle, UserConfig, PlatformDesc[InstanceId], PlatformDescSize[InstanceId], SmuServicesPpi);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    InstanceId = GnbHandle->InstanceId;
    if (PlatformDesc[InstanceId] != NULL) {
      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_LISM_RESUME, SmuArg, SmuArg);
      // Process results
      Status = DxioProcessResultsV1 (GnbHandle, UserConfig, PlatformDesc[InstanceId], PlatformDescSize[InstanceId], SmuServicesPpi);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);

      // Read back training results
      LibAmdMemFill ((VOID *)(PlatformDesc[InstanceId]), 0x00, PlatformDescSize[InstanceId] << 2, (AMD_CONFIG_PARAMS *) NULL);
      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[2] = (UINT32) PlatformDesc[InstanceId];
      SmuArg[3] = PlatformDescSize[InstanceId];
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_DMAENGINECONIFGTODRAM, SmuArg, SmuArg);

      DxioUpdatePortTrainingStatus (GnbHandle, UserConfig, PlatformDesc[InstanceId], SmuServicesPpi);
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Non Concurrent Training
 *
 *
 * @param[in]  PeiServices     Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  SmuServicesPpi  Pointer to SMU Services PPI or similar entry point structure
 * @param[in]  Pcie            Pointer to the AGESA platfom complex
 * @param[in]  UserConfig      Pointer to the platform BIOS supplied platform configuration
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
DxioNonConcurrentTraining (
  IN       CONST EFI_PEI_SERVICES          **PeiServices,
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI   *SmuServicesPpi,
  IN       PCIe_PLATFORM_CONFIG            *Pcie,
  IN       DXIO_COMPLEX_DESCRIPTOR         *UserConfig
  )
{
  AGESA_STATUS              AgesaStatus;
  AGESA_STATUS              Status;
  GNB_HANDLE                *GnbHandle;
  UINT32                    SmuArg[6];
  UINT8                     InstanceId;
  UINT8                     DxioRetval;
  DXIO_PLATFORM_DESCRIPTOR  *PlatformDesc[MAX_INSTANCE_ID];
  UINT32                    PlatformDescSize[MAX_INSTANCE_ID];
  UINT8                     *AncillaryData[MAX_INSTANCE_ID];
  UINT32                    AncillaryDataSize[MAX_INSTANCE_ID];

  AgesaStatus = AGESA_SUCCESS;

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    InstanceId = GnbHandle->InstanceId;
    DxioParseUserConfig (PeiServices,
                         GnbHandle,
                         UserConfig,
                         &PlatformDesc[InstanceId],
                         &PlatformDescSize[InstanceId],
                         &AncillaryData[InstanceId],
                         &AncillaryDataSize[InstanceId]
                         );
    
    SetHwdbgReg(PeiServices,GnbHandle,UserConfig); 
    
    if (PlatformDesc[InstanceId] != NULL) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Platform Descriptor for Die %d\n", InstanceId);
      DxioDebugDump (PlatformDesc[InstanceId],
                     PlatformDescSize[InstanceId],
                     AncillaryData[InstanceId],
                     AncillaryDataSize[InstanceId]);
    }

    if (InstanceId == UserConfig->BmcLinkLocation) {
      LibAmdMemFill(SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *)NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_LISM_RELOAD, SmuArg, SmuArg);
    }

    LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
    SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_INIT, SmuArg, SmuArg);
    DxioRetval = (UINT8) (SmuArg[0] & 0xFF);


    if ((PlatformDesc[InstanceId] != NULL) && (DxioRetval == DXIO_MBOX_RETVAL_OK)) {

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_GETVERSION, SmuArg, SmuArg);

        LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
        SmuArg[1] = DBUG_SETSCRATCHFLAG_RETURNAFTERMAPPING;
        SmuArg[2] = 1;
        SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_SETSCRATCH, SmuArg, SmuArg);

        LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
        SmuArg[1] = DBUG_SETSCRATCHFLAG_RETURNAFTERCONFIG;
        SmuArg[2] = 1;
        SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_SETSCRATCH, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[1] = DBUG_SETSCRATCHFLAG_V1ANCILLARYDATA;
      SmuArg[2] = 1;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_SETSCRATCH, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADCAPABILITIES, SmuArg, SmuArg);

      if (AncillaryDataSize[InstanceId] != 0) {
        LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
        SmuArg[1] = 0;
        SmuArg[2] = (UINT32) AncillaryData[InstanceId];
        SmuArg[3] = AncillaryDataSize[InstanceId];
        SmuArg[5] = DXIO_HEAPTYPE_ANCDATA;
        SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);
      }

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[4] = 1;
      SmuArg[5] = DXIO_HEAPTYPE_MACPCS;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[4] = 1;
      SmuArg[5] = DXIO_HEAPTYPE_GPIOPTR;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[1] = 0;
      SmuArg[2] = (UINT32) PlatformDesc[InstanceId];
      SmuArg[3] = PlatformDescSize[InstanceId];
      SmuArg[5] = DXIO_HEAPTYPE_CFGPTR;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);

      // Inititiate configuration
      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_LISM_GO, SmuArg, SmuArg);
      // Process results
      Status = DxioProcessResultsV1 (GnbHandle, UserConfig, PlatformDesc[InstanceId], PlatformDescSize[InstanceId], SmuServicesPpi);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);

      // Read back training results
      LibAmdMemFill ((VOID *)(PlatformDesc[InstanceId]), 0x00, PlatformDescSize[InstanceId] << 2, (AMD_CONFIG_PARAMS *) NULL);
      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[2] = (UINT32) PlatformDesc[InstanceId];
      SmuArg[3] = PlatformDescSize[InstanceId];
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_DMAENGINECONIFGTODRAM, SmuArg, SmuArg);

      DxioUpdatePortTrainingStatus (GnbHandle, UserConfig, PlatformDesc[InstanceId], SmuServicesPpi);
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * SMU Early Init
 *
 *
 *
 * @param[in]  PeiServices     Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  SmuServicesPpi  Pointer to SMU Services PPI or similar entry point structure
 * @param[in]  Pcie            Pointer to the AGESA platfom complex
 * @param[in]  UserConfig      Pointer to the platform BIOS supplied platform configuration
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
DxioEarlyInitV1 (
  IN       CONST EFI_PEI_SERVICES          **PeiServices,
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI   *SmuServicesPpi,
  IN       PCIe_PLATFORM_CONFIG            *Pcie,
  IN       DXIO_COMPLEX_DESCRIPTOR         *UserConfig
  )
{
  AGESA_STATUS              AgesaStatus;

  IDS_HDT_CONSOLE (GNB_TRACE, "DxioEarlyInitV1 Enter\n");

  AgesaStatus = AGESA_SUCCESS;

  if (PcdGetBool (PcdPcieSyncReset)) {
    AgesaStatus = DxioConcurrentTraining (PeiServices, SmuServicesPpi, Pcie, UserConfig);
  } else {
    AgesaStatus = DxioNonConcurrentTraining (PeiServices, SmuServicesPpi, Pcie, UserConfig);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "DxioEarlyInitV1 Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}


/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Port Speed Update Request
 *
 *
 *
 * @param[in]  PeiServices     Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  SmuServicesPpi  Pointer to SMU Services PPI or similar entry point structure
 * @param[in]  InstanceId      The unique identifier of the NBIO instance associated with this socket/die
 * @param[in]  EngineId        PCIe bridge ID
 * @param[in]  TargetSpeed     Desired speed of the target port (Gen1, Gen2, Gen3)
 */

VOID
DxioSetLinkSpeedV1 (
  IN       CONST EFI_PEI_SERVICES          **PeiServices,
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI   *SmuServicesPpi,
  IN       UINT32                          InstanceId,
  IN       UINT32                          EngineId,
  IN       UINT32                          TargetSpeed
  )
{
  UINT32                    SmuArg[6];
  UINT32                    DxioMboxMessage;


  LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
  SmuArg[1] = DxioPcieEngine;
  SmuArg[2] = TargetSpeed;
  SmuArg[3] = EngineId;

  DxioMboxMessage = DXIO_MSG_PHYS_TARGETFREQUPDATE | (DXIO_freqUpdType_single << 10);
  SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DxioMboxMessage, SmuArg, SmuArg);
}

