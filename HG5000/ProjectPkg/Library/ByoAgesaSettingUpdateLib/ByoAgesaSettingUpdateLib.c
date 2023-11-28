/** @file
==========================================================================================
      NOTICE: Copyright (c) 2006 - 2020 Byosoft Corporation. All rights reserved.
              This program and associated documentation (if any) is furnished
              under a license. Except as permitted by such license,no part of this
              program or documentation may be reproduced, stored divulged or used
              in a public system, or transmitted in any form or by any means
              without the express written consent of Byosoft Corporation.
==========================================================================================

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/PlatformCommLib.h>
#include <AMD.h>
#include <Library/GnbPciLib.h>
#include <GnbDxio.h>
#include <AmdPcieComplex.h>
#include <AmdCpmCommon.h>
#include <AmdCpmDefine.h>
#include <Ppi/AmdCpmTablePpi/AmdCpmTablePpi.h>
#include <SetupVariable.h>


#define GPIO_PIN(socket, die, gpio)     ((socket << 14) + (die << 10) + gpio)


VOID
EFIAPI
ByoUpdateMaxPayload (
  IN OUT UINT8                 *EngineMaxPayload,
  IN     PCIe_ENGINE_CONFIG    *Engine
  )
{
  CONST SETUP_DATA                    *SetupHob;
  UINT8                               MaxPay;

  MaxPay = *EngineMaxPayload;
  SetupHob = (SETUP_DATA*)GetSetupDataHobData();

  switch(SetupHob->PcieMaxPayloadSize){
    case PCIE_MAX_PAYLOAD_AUTO:
    default:
      break;

    case PCIE_MAX_PAYLOAD_128:
      *EngineMaxPayload = MAX_PAYLOAD_128;
      break;

    case PCIE_MAX_PAYLOAD_256:
      if(*EngineMaxPayload > MAX_PAYLOAD_256){
        *EngineMaxPayload = MAX_PAYLOAD_256;
      }
      break;

    case PCIE_MAX_PAYLOAD_512:
      if(*EngineMaxPayload > MAX_PAYLOAD_512){
        *EngineMaxPayload = MAX_PAYLOAD_512;
      }      
      break;
  }
 
}



STATIC DXIO_PORT_DESCRIPTOR  gDxioPcieSample = {
  0,
  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 40, 47, DxioHotplugDisabled, 1),    
  DXIO_PORT_DATA_INITIALIZER_PCIE (      
    DxioPortEnabled,                      // Port Present       
    0,                                    // Requested Device
    0,                                    // Requested Function
    DxioHotplugDisabled,                  // Hotplug
    DxioGenMaxSupported,                  // Max Link Speed
    DxioGenMaxSupported,                  // Max Link Capability
    DxioAspmDisabled,                        // ASPM
    DxioAspmDisabled,                     // ASPM L1.1 disabled
    DxioAspmDisabled,                     // ASPM L1.2 disabled
    DxioClkPmSupportDisabled              // Clock PM
  )
};

STATIC DXIO_PORT_DESCRIPTOR  gDxioSataSample = {
  0,
  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 4, 7, DxioHotplugDisabled, 1),
  DXIO_PORT_DATA_INITIALIZER_SATA (
    DxioPortEnabled                      
  )
};




STATIC
DXIO_PORT_DESCRIPTOR* 
AddDxioPcieLane (
    DXIO_PORT_DESCRIPTOR  *Port,
    UINT8                 StartLane,
    UINT8                 EndLane,
    UINT8                 HotPlug    
  )
{
  DEBUG((EFI_D_INFO, "%a(%d,%d), HotPlug :%d.\n", __FUNCTION__, StartLane, EndLane, HotPlug));

  CopyMem(Port, &gDxioPcieSample, sizeof(DXIO_PORT_DESCRIPTOR));
  Port->EngineData.StartLane = StartLane;
  Port->EngineData.EndLane   = EndLane;
  Port->Port.LinkHotplug = HotPlug;

  Port++;

  return Port;
}

STATIC
DXIO_PORT_DESCRIPTOR* 
AddDxioSataLane (
    DXIO_PORT_DESCRIPTOR  *Port,
    UINT8                 StartLane,
    UINT8                 EndLane
  )
{
  DEBUG((EFI_D_INFO, "%a(%d,%d)\n", __FUNCTION__, StartLane, EndLane));

  CopyMem(Port, &gDxioSataSample, sizeof(DXIO_PORT_DESCRIPTOR));
  Port->EngineData.StartLane = StartLane;
  Port->EngineData.EndLane   = EndLane;
  Port++;

  return Port;
}





VOID 
UpdatePcieSlotId (
  IN AMD_CPM_DXIO_TOPOLOGY_TABLE    *DxioTopologyTablePtr,
  IN UINT8                          *SlotId
  )
{
  UINT8                               Index;
  DXIO_PORT_DESCRIPTOR                *Port;
  UINT8                               Id = *SlotId;                              


  DEBUG((EFI_D_INFO, "UpdatePcieSlotId\n"));

  if (DxioTopologyTablePtr == NULL) {
    return;
  }
    
  for (Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index++) {
    Port = &DxioTopologyTablePtr->Port[Index];
    if(Port->EngineData.EngineType == DxioPcieEngine && Port->Port.SlotNum == 0){
      Port->Port.SlotNum = Id++;
      DEBUG((EFI_D_INFO, "(%d,%d) slot %d\n", Port->EngineData.StartLane, Port->EngineData.EndLane, Port->Port.SlotNum));
    }
    if(Port->Flags == DESCRIPTOR_TERMINATE_LIST){
      break;
    }    
  }

  *SlotId = Id;

}

VOID 
UpdatePcieAspm (
  IN AMD_CPM_DXIO_TOPOLOGY_TABLE    *DxioTopologyTablePtr,
  IN UINT8                          PcieAspm
  )
{
  UINT8                               Index;
  DXIO_PORT_DESCRIPTOR                *Port;


  DEBUG((EFI_D_INFO, "UpdatePcieAspm\n"));

  if (DxioTopologyTablePtr == NULL) {
    return;
  }
    
  for (Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index++) {
    Port = &DxioTopologyTablePtr->Port[Index];
    if(Port->EngineData.EngineType != DxioPcieEngine){
      continue;
    }

    switch(PcieAspm){
      case PCIE_ASPM_DISABLE:
      default:
        Port->Port.LinkAspm = DxioAspmDisabled;
        break;

      case PCIE_ASPM_L0S:
        Port->Port.LinkAspm = DxioAspmL0s;
        break;

      case PCIE_ASPM_L1:
        Port->Port.LinkAspm = DxioAspmL1;
        break;

      case PCIE_ASPM_L0SL1:
        Port->Port.LinkAspm = DxioAspmL0sL1;
        break;
    }

    if(Port->Flags == DESCRIPTOR_TERMINATE_LIST){
      break;
    }    
  }

}



VOID 
DisableDxioObLan (
  IN AMD_CPM_DXIO_TOPOLOGY_TABLE    *DxioTopologyTablePtr
  )
{
  UINT8                               Index;
  DXIO_PORT_DESCRIPTOR                *Port;

  if (DxioTopologyTablePtr == NULL) {
    return;
  }
    
  for (Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index++) {
    Port = &DxioTopologyTablePtr->Port[Index];
    if(Port->EngineData.StartLane == 12 && Port->EngineData.EndLane == 15){
      Port->Port.PortPresent = DxioPortDisabled;
      DEBUG((EFI_D_INFO, "DisableDxioObLan\n"));
      break;
    }
    if(Port->Flags == DESCRIPTOR_TERMINATE_LIST){
      break;
    }    
  }

}


VOID *MyGetByoSysAndStsHobData(VOID)
{
  EFI_PEI_HOB_POINTERS  GuidHob;
  VOID                  *Data;

  GuidHob.Raw = GetFirstGuidHob(&gByoSysCfgAndStsHobGuid);
  ASSERT(GuidHob.Raw != NULL);
  Data = (VOID *)(GuidHob.Guid + 1);

  return Data;
}



/*
[5000]
00 - 15   PCIE   // die0
16 - 31   xGMI   // die0
32 - 47   xGMI   // die2
48 - 63   PCIE   // die2

[7000]
00 - 15   PCIE           P0
16 - 31   xGMI
32 - 47   PCIE   <---    P1
48 - 63   xGMI   <---

64 - 79   xGMI
80 - 95   PCIE           P2
96 - 111  xGMI
112- 127  PCIE           P3
*/


EFI_STATUS
EFIAPI
ByoUpdateDxioData (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN AMD_CPM_DXIO_TOPOLOGY_TABLE    *DxioTopologyTableS0Ptr,
  IN AMD_CPM_DXIO_TOPOLOGY_TABLE    *DxioTopologyTableS1Ptr
  )
{
  EFI_STATUS                          Status;
  AMD_CPM_TABLE_PPI                   *AmdCpmTable;
  UINT8                               c0p1_1 = 0;
  //UINT8                               c0p1_2 = 0;
 // UINT8                               c0p2   = 0;
 // UINT8                               c1p0_1 = 0;
  //UINT8                               c1p0_2 = 0;  
  //UINT8                               c1p2   = 0;
  BYO_SYS_CFG_STS_DATA                *SysCfg;
  CONST SETUP_DATA                    *SetupHob;
  UINT8                               Index;
  DXIO_PORT_DESCRIPTOR                *Port;
  UINT8                               SlotId = 1;
 // UINT8                               SlimlineSata0En;
  //UINT8                               SlimlineSata1En;
  UINT8                               HotPlug = DxioHotplugDisabled;


  SetupHob = (SETUP_DATA*)GetSetupDataHobData();
  SysCfg = (BYO_SYS_CFG_STS_DATA*)MyGetByoSysAndStsHobData(); 


  Status = (*PeiServices)->LocatePpi(
                             PeiServices,
                             &gAmdCpmTablePpiGuid,
                             0,
                             NULL,
                             (VOID**)&AmdCpmTable
                             );
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "AmdCpmTable not found\n"));
    return Status;
  }

  //SlimlineSata0En = SetupHob->SlimlineSata0En;     // C0P1[7:0] ([39:32])
  //SlimlineSata1En = SetupHob->SlimlineSata1En;     // C1P0[7:0] ([07:00])
    
  c0p1_1 |= AmdCpmTable->CommonFunction.GetGpio(AmdCpmTable, GPIO_PIN(0, 0, 5))?BIT1:0;
  c0p1_1 |= AmdCpmTable->CommonFunction.GetGpio(AmdCpmTable, GPIO_PIN(0, 0, 6))?BIT0:0;  



// 11 SATA
// 10 OCP3.0
// 00 NVME

  DEBUG((EFI_D_INFO, "c0p1_1:%d \n", c0p1_1));
  PcdSet8(PcdSmileline,c0p1_1);

  if (DxioTopologyTableS0Ptr != NULL) {
    DEBUG((EFI_D_INFO, "DxioTopologyTableS0Ptr:%X\n", DxioTopologyTableS0Ptr));
    for (Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index++) {
      Port = &DxioTopologyTableS0Ptr->Port[Index];
      if(Port->Flags == DESCRIPTOR_TERMINATE_LIST){
        break;
      }
    }
    Port->Flags &= ~DESCRIPTOR_TERMINATE_LIST;
    Port++;

    //p1_1 = 0;
    //HotPlug = DxioHotplugDisabled;
    switch(c0p1_1){
      case 3:
        Port = AddDxioSataLane(Port, 0, 7);
        break;
        
      case 2:
        Port = AddDxioPcieLane(Port, 0, 7, HotPlug);
        break;
        
      case 0:
        Port = AddDxioPcieLane(Port, 0, 3, DxioHotplugServerExpress);
        Port = AddDxioPcieLane(Port, 4, 7, DxioHotplugServerExpress);
        break;
        
    }
    


    Port--;
    Port->Flags |= DESCRIPTOR_TERMINATE_LIST;

  }

  if (DxioTopologyTableS1Ptr != NULL) {
    DEBUG((EFI_D_INFO, "DxioTopologyTableS1Ptr:%X\n", DxioTopologyTableS1Ptr));
    for (Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index++) {
      Port = &DxioTopologyTableS1Ptr->Port[Index];
      if(Port->Flags == DESCRIPTOR_TERMINATE_LIST){
        break;
      }
    }
    Port->Flags &= ~DESCRIPTOR_TERMINATE_LIST;
    Port++;

    //p1_1 = 0;
    //HotPlug = DxioHotplugDisabled;
    if(SetupHob->RetimerCard){         //for retimer card at slot1
      Port = AddDxioPcieLane(Port, 56, 59, DxioHotplugServerExpress);
	  Port = AddDxioPcieLane(Port, 60, 63, DxioHotplugServerExpress);
	}else{
      Port = AddDxioPcieLane(Port, 56, 63, HotPlug);
	}


    Port--;
    Port->Flags |= DESCRIPTOR_TERMINATE_LIST;

  }


  UpdatePcieSlotId(DxioTopologyTableS0Ptr, &SlotId);
  UpdatePcieSlotId(DxioTopologyTableS1Ptr, &SlotId);

  if (!SetupHob->ObLanEn) {
  	PcdSet8(PcdObLanEn,0);
    DisableDxioObLan (DxioTopologyTableS0Ptr);
  }

  UpdatePcieAspm(DxioTopologyTableS0Ptr, SetupHob->PcieAspm);
  UpdatePcieAspm(DxioTopologyTableS1Ptr, SetupHob->PcieAspm);
  
  return EFI_SUCCESS;
}

