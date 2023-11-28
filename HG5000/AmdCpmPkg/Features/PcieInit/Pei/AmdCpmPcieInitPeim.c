/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM PCIE Initialization
 *
 * Contains code that initialized PCIE before memory init.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  PcieInit
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
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
 ******************************************************************************
 */

//#include <token.h>  //Remove this, need change to use OEM table to determine it is 1P or 2P platform.
#include <AmdCpmPei.h>
#include <Ppi/NbioPcieComplexPpi.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PcdLib.h>
#include <AmdBoardId.h>
#include <TSFch.h>
#include <Token.h>

//PS-67--start
#include <Ppi/M24Lc128Ppi.h>
#define DxioInputParserGetNextDescriptor(Descriptor) (Descriptor != NULL ? ((((Descriptor->Flags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : (++Descriptor))) : NULL)
//PS-67--end



// winddy + >>
EFI_STATUS
EFIAPI
ByoUpdateDxioData (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN AMD_CPM_DXIO_TOPOLOGY_TABLE    *DxioTopologyTableS0Ptr,
  IN AMD_CPM_DXIO_TOPOLOGY_TABLE    *DxioTopologyTableS1Ptr
  );
// winddy + <<





#define SATA_PCIE_SEL_SOCKET            0
#define SATA_PCIE_SEL_DIE               2
#define SATA_PCIE_SEL_PIN               23

#if 0  //DXIO Topology hardcode
typedef enum {
  DEVICE_ID_PCIE_X8_SLOT      =             0x01,                       ///< Device ID for PCIE X8 slot device. GPP_CLK0
  DEVICE_ID_PCIE_X16_SLOT     =             0x02,                       ///< Device ID for PCIE x16 slot device, GFX_CLK
  DEVICE_ID_PCIE_X4_SLOT      =             0x03,                       ///< Device ID for PCIE x4 slot device, GPP_CLK3
  DEVICE_ID_BT                =             0x04,                       ///< Device Id for BlueTooth
  DEVICE_ID_GBE               =             0x05,                       ///< Device Id for GBE
  DEVICE_ID_WLAN              =             0x06,                       ///< Device ID for WLAN
  DEVICE_ID_M2_CONNECT        =             0x07,                       ///< Device ID for M2 connect, GPP_CLK1
  DEVICE_ID_PROMONTORY        =             0x08,                       ///< Device ID for Promontroy, GPP_CLK2
  DEVICE_ID_SATA_EXPRESS      =             0x09,                       ///< Device ID for SATA Express
  DEVICE_ID_PCIE_X16_SWITCH   =             0x0A,                       ///< Device ID for PCIe x16/x8 switch
  DEVICE_ID_SATAE_M2_SWITCH   =             0x0B,                       ///< Device ID for SATAe/M.2 switch
} CPM_GPIO_DEVICE_ID;

//temporatory test
DXIO_PORT_DESCRIPTOR  PortListS0[] =  {
  { // P0 - SATA port
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_SATA (
      DxioPortEnabled                       // Port Present
    )
  },
  { // P0 - x8 slot
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 4, 7, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                      // Port Present
      1,                                    // Requested Device
      1,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGen1,                  // Max Link Speed
      DxioGen1,                  // Max Link Capability
      DxioAspmL0sL1,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },
  { // P0 - x8 slot
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                      // Port Present
      1,                                    // Requested Device
      2,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGen1,                  // Max Link Speed
      DxioGen1,                  // Max Link Capability
      DxioAspmL0sL1,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },
  { // P1 - x8 slot
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 32, 39, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                      // Port Present
      1,                                    // Requested Device
      1,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGen2,                  // Max Link Speed
      DxioGen2,                  // Max Link Capability
      DxioAspmL0sL1,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },
  { // P1 - x8 slot (BMC VGA)
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 40, 47, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                      // Port Present
      1,                                    // Requested Device
      2,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGen2,                  // Max Link Speed
      DxioGen2,                  // Max Link Capability
      DxioAspmL0sL1,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },
  { // P2 - x16 slot
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 80, 95, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                      // Port Present
      1,                                    // Requested Device
      1,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGenMaxSupported,                  // Max Link Speed
      DxioGenMaxSupported,                  // Max Link Capability
      DxioAspmL0sL1,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },
  { // P3 - x16 slot
    DESCRIPTOR_TERMINATE_LIST,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 112, 127, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                      // Port Present
      1,                                    // Requested Device
      1,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGenMaxSupported,                  // Max Link Speed
      DxioGenMaxSupported,                  // Max Link Capability
      DxioAspmL0sL1,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  }
};   // End of DXIO_PORT_DESCRIPTOR

//
// CPM DXIO Topology Table
//
DXIO_PORT_DESCRIPTOR  PortListS1[] =  {
  { // P0 - SATA port
    DESCRIPTOR_TERMINATE_LIST,
    DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_SATA (
      DxioPortEnabled                       // Port Present
    )
  }
};   // End of DXIO_PORT_DESCRIPTOR

// DXIO complex descriptor
DXIO_COMPLEX_DESCRIPTOR PcieComplex[] = {
  {
    0,
    0,
    &PortListS0[0],
    NULL,
    NULL
  },
  {
    DESCRIPTOR_TERMINATE_LIST,
    1,
    &PortListS1[0],
    NULL,
    NULL
  }
};
#endif


EFI_STATUS
EFIAPI
AmdCpmPcieInitPeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mCpmPcieInitPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCpmGpioInitFinishedPpiGuid,
  AmdCpmPcieInitPeim
};

EFI_STATUS
AmdCpmDxioGetComplex (
  IN       PEI_AMD_NBIO_PCIE_COMPLEX_PPI *This,
  OUT      DXIO_COMPLEX_DESCRIPTOR       **UserConfig
  )
{
#if 1 //DXIO Topology hardcode
  DXIO_COMPLEX_DESCRIPTOR             *DxioComplex;
  EFI_STATUS                          Status;
  EFI_PEI_SERVICES                    **PeiServices;
  AMD_CPM_TABLE_PPI                   *CpmTablePpiPtr;

  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();

  Status = (*PeiServices)->LocatePpi (
                                      (CPM_PEI_SERVICES**)PeiServices,
                                      &gAmdCpmTablePpiGuid,
                                      0,
                                      NULL,
                                      (VOID**)&CpmTablePpiPtr
                                      );
  if (EFI_ERROR (Status)) {
    return  Status;
  }
  DxioComplex = CpmTablePpiPtr->PeimPublicFunction.DxioComplexDescriptorPtr;

  *UserConfig = DxioComplex;
#else
  *UserConfig = PcieComplex;
#endif

  return EFI_SUCCESS;
}

STATIC PEI_AMD_NBIO_PCIE_COMPLEX_PPI mDxioComplexPpi = {
  AMD_NBIO_PCIE_COMPLEX_REVISION,  ///< revision
  AmdCpmDxioGetComplex
};

/*----------------------------------------------------------------------------------------*/
/**
 * The Function to reset PCIe device.
 *
 * @param[in]     This          Pointer to AMD CPM Table Ppi
 * @param[in]     ResetId       Pcie Device Id which is defined in Pcie Complex Descriptor table
 * @param[in]     ResetControl  Reset Control Flag. 0: Reset assert. 1: Reset deassert
 *
 * @retval        EFI_SUCCESS   Function initialized successfully
 * @retval        EFI_ERROR     Initialization failed (see error for more details)
 */

VOID
EFIAPI
CpmPcieReset (
  IN      VOID                        *This,
  IN      UINT8                       ResetId,
  IN      UINT8                       ResetControl
  )
{
  AMD_CPM_TABLE_PPI                   *CpmTablePpiPtr;
  CpmTablePpiPtr = This;
  CpmTablePpiPtr->CommonFunction.ResetDevice (CpmTablePpiPtr, ResetId, ResetControl);

  return;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Generate Pcie Complex Descriptor table for AGESA and set the trigger level of
 * GEVENT pin for Express Card.
 *
 * @param[in]     PeiServices       Pointer to Pei Services
 * @param[in]     NotifyDescriptor  The descriptor for the notification event
 * @param[in]     Ppi               Pointer to the PPI in question.
 *
 * @retval        EFI_SUCCESS       Module initialized successfully
 * @retval        EFI_ERROR         Initialization failed (see error for more details)
 */

EFI_STATUS
EFIAPI
AmdCpmPcieInitPeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  AMD_CPM_TABLE_PPI                   *CpmTablePpiPtr;
  AMD_CPM_MAIN_TABLE                  *MainTablePtr;
  AMD_CPM_DXIO_TOPOLOGY_TABLE         *DxioTopologyTableS0Ptr;
  AMD_CPM_DXIO_TOPOLOGY_TABLE         *DxioTopologyTableS1Ptr = NULL;       // winddy -
  AMD_CPM_EXPRESS_CARD_TABLE          *ExpressCardTablePtr;
  AMD_CPM_OTHER_HOTPLUG_CARD_TABLE    *OtherHotplugTablePtr;
  AMD_CPM_PCIE_CLOCK_TABLE            *PcieClockTablePtr;
  DXIO_COMPLEX_DESCRIPTOR             *DxioComplexDescriptorPtr;
  EFI_PEI_PPI_DESCRIPTOR              *PpiListNbioPcieComplexPtr;
  EFI_STATUS                          Status;
#if 0
  UINT8                               Index;
  UINT8                               i;
  BOOLEAN                             ClockType;
#endif
  UINT8                               CpuRevisionId;
  UINT8                               SataPcieSelect; 
  HYGON_BOARD_ID                      PlatformSelect;

  Status = (*PeiServices)->LocatePpi (
                                      (CPM_PEI_SERVICES**)PeiServices,
                                      &gAmdCpmTablePpiGuid,
                                      0,
                                      NULL,
                                      (VOID**)&CpmTablePpiPtr
                                      );
  if (EFI_ERROR (Status)) {
    return  Status;
  }

  PlatformSelect = PcdGet8(PcdPlatformSelect);
  
  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpPcieInitPeimDriverBegin);

  MainTablePtr                    = CpmTablePpiPtr->MainTablePtr;
  if(PlatformSelect == HYGON_65N16) {
      SataPcieSelect = CpmTablePpiPtr->CommonFunction.SmnRead8 (CpmTablePpiPtr, SATA_PCIE_SEL_SOCKET, SATA_PCIE_SEL_DIE, (UINT32) FCH_SMN_GPIO_BASE, (UINT32) ((SATA_PCIE_SEL_PIN << 2) + 2)) & 0x01;
      DEBUG((EFI_D_INFO, "[Hygon]: SataPcieSelect Pin Status: %x\n",SataPcieSelect));
      if (0 == SataPcieSelect){  // PCIe Interface
          DxioTopologyTableS0Ptr = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);
      } else if (1 == SataPcieSelect) { // SATA Interface
          DxioTopologyTableS0Ptr = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, EPRO_CPM_SIGNATURE_DXIO_TOPOLOGY);
      }
  }else{
      DxioTopologyTableS0Ptr = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);
  }
#if _PCD_VALUE_PcdAmdNumberOfPhysicalSocket == 2             // winddy -
  DxioTopologyTableS1Ptr          = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY_S1);
#endif
  ExpressCardTablePtr             = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_PCIE_EXPRESS_CARD);
  OtherHotplugTablePtr            = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_PCIE_OTHER_HOTPLUG_CARD);
  PcieClockTablePtr               = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_PCIE_CLOCK);
  CpuRevisionId                   = CpmTablePpiPtr->CommonFunction.GetCpuRevisionId (CpmTablePpiPtr);

  //PS-67 --start
  if(PlatformSelect == HYGON_DIESEL) {
      //AmdXgbeMacAddr (PeiServices); 
      if (DxioTopologyTableS0Ptr != NULL)
      {
          EFI_PEI_M24LC128_PPI       *M24Lc128Ppi;
          UINT8                      MacBuffer[48];
          UINT32                     Value;
          UINT8                      *MacPointer; 
          DXIO_PORT_DESCRIPTOR       *EngineDescriptor = NULL;
          EFI_GUID                   M24Lc128PpiGuid = { 0xca4f8926, 0xa5a4, 0x438d, {0xb6, 0xe7, 0x82, 0x9f, 0x10, 0xa5, 0x4c, 0xd3 }};

          UINT8 i = 0;

          Status = (*PeiServices)->LocatePpi (PeiServices, &M24Lc128PpiGuid, 0, NULL, &M24Lc128Ppi);
          Status = M24Lc128Ppi->Read (PeiServices, 5, 0x50, 0x4d, 24, MacBuffer);

          DEBUG((EFI_D_ERROR, "PS-67: Mac address read from M24Lc128:\n"));
          for(i=0; i<24; i++)
          {
              DEBUG((EFI_D_ERROR, "0x%02x ", MacBuffer[i]));
          }

          DEBUG((EFI_D_ERROR, "\n"));

          MacPointer = MacBuffer;
          EngineDescriptor  = &DxioTopologyTableS0Ptr->Port[0];

          while (EngineDescriptor != NULL) {
              if (EngineDescriptor->EngineData.EngineType == DxioEthernetEngine) {
                  switch (EngineDescriptor->EngineData.StartLane % 32) {
                  case 4:
                      // Initialize this descriptor for Port 0
                      Value = *((UINT32*) MacPointer);
                      EngineDescriptor->EtherNet.MacAddressLo = Value;  //Read the low value of the mac address here...;
                      MacPointer += 4;
                      Value = *((UINT32*) MacPointer);
                      //Value = *((UINT16*) MacPointer);
                      Value &= 0xFFFF;
                      Value |= 0x80000000;  // Set Valid
                      EngineDescriptor->EtherNet.MacAddressHi = Value;  //Read the high value of the mac address here...;
                      MacPointer += 2;
                      break;
                  case 5:
                      // Initialize this descriptor for Port 1
                      Value = *((UINT32*) MacPointer);
                      EngineDescriptor->EtherNet.MacAddressLo = Value;
                      MacPointer += 4;
                      Value = *((UINT32*) MacPointer);
                      //Value = *((UINT16*) MacPointer);
                      Value &= 0xFFFF;
                      Value |= 0x80000000;  // Set Valid
                      EngineDescriptor->EtherNet.MacAddressHi = Value;
                      MacPointer += 2;
                      break;
                  case 6:
                      // Initialize this descriptor for Port 2
                      Value = *((UINT32*) MacPointer); 
                      EngineDescriptor->EtherNet.MacAddressLo = Value;
                      MacPointer += 4;
                      Value = *((UINT32*) MacPointer);
                      //Value = *((UINT16*) MacPointer);
                      Value &= 0xFFFF;
                      Value |= 0x80000000;  // Set Valid
                      EngineDescriptor->EtherNet.MacAddressHi = Value;
                      MacPointer += 2;
                      break;
                  case 7:
                      // Initialize this descriptor for Port 3
                      Value = *((UINT32*) MacPointer);
                      EngineDescriptor->EtherNet.MacAddressLo = Value;
                      MacPointer += 4;
                      Value = *((UINT32*) MacPointer);
                      //Value = *((UINT16*) MacPointer);
                      Value &= 0xFFFF;
                      Value |= 0x80000000;  // Set Valid
                      EngineDescriptor->EtherNet.MacAddressHi = Value;
                      MacPointer += 2;
                      break;
                  default:
                      ASSERT (FALSE);  // Illegal lane address
                      break;
                  }
              }
              EngineDescriptor = DxioInputParserGetNextDescriptor (EngineDescriptor);
          } 
      }
  }
  //PS-67--end

  Status = ByoUpdateDxioData(PeiServices, DxioTopologyTableS0Ptr, DxioTopologyTableS1Ptr);        // winddy +
  
#if 0
  //Need rewrite routing for dual socket support
  if ((ExpressCardTablePtr || OtherHotplugTablePtr) && DxioTopologyTableS0Ptr) {
    for ( Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index ++ ) {
      if ((DxioTopologyTableS0Ptr->Port[Index].EngineData.EngineType == DxioPcieEngine) && (DxioTopologyTableS0Ptr->Port[Index].Port.PortPresent)) {
        if (ExpressCardTablePtr && (DxioTopologyTableS0Ptr->Port[Index].Port.DeviceNumber == ExpressCardTablePtr->Device)  &&
           (DxioTopologyTableS0Ptr->Port[Index].Port.FunctionNumber == ExpressCardTablePtr->Function)) {
          DxioTopologyTableS0Ptr->Port[Index].EngineData.HotPluggable = 1;
          DxioTopologyTableS0Ptr->Port[Index].Port.LinkHotplug = DxioHotplugBasic;
          if (CpmTablePpiPtr->CommonFunction.GetGevent (CpmTablePpiPtr, ExpressCardTablePtr->EventPin)) {
            DxioTopologyTableS0Ptr->Port[Index].Port.EndpointStatus = EndpointNotPresent;
          }
        }
        if (OtherHotplugTablePtr) {
          if ((OtherHotplugTablePtr->Number > 0) && (DxioTopologyTableS0Ptr->Port[Index].Port.DeviceNumber == OtherHotplugTablePtr->Device0)  &&
             (DxioTopologyTableS0Ptr->Port[Index].Port.FunctionNumber == OtherHotplugTablePtr->Function0)) {
            DxioTopologyTableS0Ptr->Port[Index].Port.LinkHotplug = HotplugBasic;
            if (CpmTablePpiPtr->CommonFunction.GetGevent (CpmTablePpiPtr, OtherHotplugTablePtr->EventPin0)) {
              DxioTopologyTableS0Ptr->Port[Index].Port.EndpointStatus = DxioEndpointNotPresent;
            }
          }
          if ((OtherHotplugTablePtr->Number > 1) && (DxioTopologyTableS0Ptr->Port[Index].Port.DeviceNumber == OtherHotplugTablePtr->Device1)  &&
             (DxioTopologyTableS0Ptr->Port[Index].Port.FunctionNumber == OtherHotplugTablePtr->Function1)) {
            DxioTopologyTableS0Ptr->Port[Index].EngineData.HotPluggable = 1;
            DxioTopologyTableS0Ptr->Port[Index].Port.LinkHotplug = DxioHotplugBasic;
            if (CpmTablePpiPtr->CommonFunction.GetGevent (CpmTablePpiPtr, OtherHotplugTablePtr->EventPin1)) {
              DxioTopologyTableS0Ptr->Port[Index].Port.EndpointStatus = DxioEndpointNotPresent;
            }
          }
        }
      }
      if (DxioTopologyTableS0Ptr->Port[Index].Flags == DESCRIPTOR_TERMINATE_LIST) {
        break;
      }
    }
  }

  //Need rewrite routing for dual socket support
  if (PcieClockTablePtr && DxioTopologyTableS0Ptr) {
    ClockType = (BOOLEAN) CpmTablePpiPtr->CommonFunction.GetStrap (CpmTablePpiPtr) & BIT1;
    for ( Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index ++ ) {
      if ((DxioTopologyTableS0Ptr->Port[Index].EngineData.EngineType == DxioPcieEngine) && (DxioTopologyTableS0Ptr->Port[Index].Port.PortPresent)) {
        for (i = 0; i < AMD_PCIE_CLOCK_SIZE; i++) {
          if ((DxioTopologyTableS0Ptr->Port[Index].Port.DeviceNumber == PcieClockTablePtr->Item[i].Device)  &&
              (DxioTopologyTableS0Ptr->Port[Index].Port.FunctionNumber == PcieClockTablePtr->Item[i].Function)) {
            if (ClockType) {
              if (PcieClockTablePtr->Item[i].ClkReq != 0x00 && PcieClockTablePtr->Item[i].ClkReq != 0xFF) {
                DxioTopologyTableS0Ptr->Port[Index].Port.MiscControls.ClkPmSupport = DxioClkPmSupportEnabled;
              }
            } else {
              if (PcieClockTablePtr->Item[i].ClkReqExt != 0x00 && PcieClockTablePtr->Item[i].ClkReqExt != 0xFF) {
                DxioTopologyTableS0Ptr->Port[Index].Port.MiscControls.ClkPmSupport = DxioClkPmSupportEnabled;
              }
            }
          }
        }
      }
      if (DxioTopologyTableS0Ptr->Port[Index].Flags == DESCRIPTOR_TERMINATE_LIST) {
        break;
      }
    }
  }
#endif
  CpmTablePpiPtr->PeimPublicFunction.PcieReset                = CpmPcieReset;

#if _PCD_VALUE_PcdAmdNumberOfPhysicalSocket == 2   //For 2P Multi-Die Multi-Core Topology  winddy -
  //Need create variable to indicate platform max socket support instead of use magic number here
  if (DxioTopologyTableS0Ptr && DxioTopologyTableS1Ptr) {
    Status = (**PeiServices).AllocatePool (
                                            (CPM_PEI_SERVICES**)PeiServices,
                                            (sizeof (DXIO_COMPLEX_DESCRIPTOR) * 2),
                                            (VOID**)&DxioComplexDescriptorPtr
                                            );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    (**PeiServices).SetMem (
                            DxioComplexDescriptorPtr,
                            (sizeof (DXIO_COMPLEX_DESCRIPTOR) * 2),
                            0
                           );
    DxioComplexDescriptorPtr[0].Flags         = 0;
    DxioComplexDescriptorPtr[0].SocketId      = 0;
    DxioComplexDescriptorPtr[0].PciePortList  = &DxioTopologyTableS0Ptr->Port[0];
    DxioComplexDescriptorPtr[1].Flags         = DESCRIPTOR_TERMINATE_LIST;
    DxioComplexDescriptorPtr[1].SocketId      = 1;
    DxioComplexDescriptorPtr[1].PciePortList  = &DxioTopologyTableS1Ptr->Port[0];
    CpmTablePpiPtr->PeimPublicFunction.DxioComplexDescriptorPtr  = DxioComplexDescriptorPtr;
  }
#else
  //Need create variable to indicate platform max socket support instead of use magic number here
  if (DxioTopologyTableS0Ptr) {
    Status = (**PeiServices).AllocatePool (
                                            (CPM_PEI_SERVICES**)PeiServices,
                                            sizeof (DXIO_COMPLEX_DESCRIPTOR),
                                            (VOID**)&DxioComplexDescriptorPtr
                                            );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    (**PeiServices).SetMem (
                            DxioComplexDescriptorPtr,
                            sizeof (DXIO_COMPLEX_DESCRIPTOR),
                            0
                           );
    DxioComplexDescriptorPtr->Flags         = DESCRIPTOR_TERMINATE_LIST;
    DxioComplexDescriptorPtr->SocketId      = 0;
    DxioComplexDescriptorPtr->PciePortList  = &DxioTopologyTableS0Ptr->Port[0];
    CpmTablePpiPtr->PeimPublicFunction.DxioComplexDescriptorPtr  = DxioComplexDescriptorPtr;
  }
#endif

  Status = (*PeiServices)->AllocatePool (
                              (CPM_PEI_SERVICES**)PeiServices,
                              sizeof (EFI_PEI_PPI_DESCRIPTOR),
                              (VOID**)&PpiListNbioPcieComplexPtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PpiListNbioPcieComplexPtr->Flags   = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListNbioPcieComplexPtr->Guid    = &gAmdNbioPcieComplexPpiGuid;
  PpiListNbioPcieComplexPtr->Ppi     = (VOID *) &mDxioComplexPpi;

  Status = (*PeiServices)->InstallPpi (
                              (CPM_PEI_SERVICES**)PeiServices,
                              PpiListNbioPcieComplexPtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpPcieInitPeimDriverEnd);

  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM PCIE Init PEIM driver
 *
 * This function registers the function to update PCIe topology table according
 * to AMD CPM PCIE TOPOLOGY OVERRIDE table and set GEVENT trigger level for Express
 * Card if AMD CPM Express Card Table exists.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */

EFI_STATUS
EFIAPI
AmdCpmPcieInitPeimEntryPoint (
  IN        CPM_PEI_FILE_HANDLE         FileHandle,
  IN        CPM_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                      Status;
//  EFI_PEI_PPI_DESCRIPTOR              *PpiListNbioPcieComplexPtr;

  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmPcieInitPeimNotify);

#if 0  //DXIO Topology hardcode
  Status = (*PeiServices)->AllocatePool (
                              (CPM_PEI_SERVICES**)PeiServices,
                              sizeof (EFI_PEI_PPI_DESCRIPTOR),
                              (VOID**)&PpiListNbioPcieComplexPtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PpiListNbioPcieComplexPtr->Flags   = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListNbioPcieComplexPtr->Guid    = &gAmdNbioPcieComplexPpiGuid;
  PpiListNbioPcieComplexPtr->Ppi     = (VOID *) &mDxioComplexPpi;

  Status = (*PeiServices)->InstallPpi (
                              (CPM_PEI_SERVICES**)PeiServices,
                              PpiListNbioPcieComplexPtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }
#endif


  return EFI_SUCCESS;
}
