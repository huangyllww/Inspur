/* $NoKeywords:$ */
/**
 * @file
 *
 * HygonCxlPei Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonCxlPei
 * @e \$Revision: 312065 $   @e \$Date: 2022-08-02 13:46:05 -0600 (Aug, 2 Tue 2022) $
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
#include <PiPei.h>
#include <Filecode.h>
#include <GnbHsio.h>
#include <Gnb.h>
#include <HygonCxlPei.h>
#include <GnbRegisters.h>
#include <HygonCxlRegOffset.h>
#include <HygonCxlRegStruct.h>
#include <FabricRegistersST.h>
#include <HygonPcieComplex.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/GnbPciLib.h>
#include <Library/GnbCxlLib.h>
#include <Library/GnbMemAccLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Ppi/NbioBaseServicesSTPpi.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Guid/GnbNbioBaseSTInfoHob.h>
#include <Guid/GnbPcieInfoHob.h>
#include <IndustryStandard/Cxl.h>
#include <IndustryStandard/Cxl20.h>        // byo230928 +

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define FILECODE        NBIO_CXL_HYGONCXLPEI_HYGONCXLPEI_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
  PEI_HYGON_NBIO_PCIE_SERVICES_PPI   *mPcieServicesPpi;
  GNB_PCIE_INFORMATION_DATA_HOB      *mPciePlatformConfigHob;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS NbioCxlInitCallbackAfterTraining (
    IN EFI_PEI_SERVICES           **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
    IN VOID                       *InvokePpi
);

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */
static EFI_PEI_NOTIFY_DESCRIPTOR mNotifyNbioCxlPpi = {
  EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gHygonNbioPcieTrainingDonePpiGuid, 
  NbioCxlInitCallbackAfterTraining,
};

/**----------------------------------------------------------------------------------------*/
/**
 * CXL interface to configure register setting
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this NBIO
 *
 * @retval    HGPI_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
CxlInterfaceInitSetting (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  IOHC_SHADOW_CXL_PORT_CNTL_AND_ALT_BUS_RANGE_REGISTER  IohcShadowCxlCtrl;
  
  if (GnbHandle->CxlPresent == FALSE) {
    return;
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "  CxlInterfaceInitSetting Enter \n");

  NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_IOHC_SHADOW_CXL_PORT_CNTL_AND_ALT_BUS_RANGE), &IohcShadowCxlCtrl.Value, 0);
  IohcShadowCxlCtrl.Field.CXL_ALT_BUS_BASE = 1;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_IOHC_SHADOW_CXL_PORT_CNTL_AND_ALT_BUS_RANGE), &IohcShadowCxlCtrl.Value, 0);
  
  //CXL Feature setting
  //EnableCxlRoRegWritable (GnbHandle, TRUE);
  
  //EnableCxlRoRegWritable (GnbHandle, FALSE);
  
  IDS_HDT_CONSOLE (GNB_TRACE, "  CxlInterfaceInitSetting Exit \n");
}

 /**----------------------------------------------------------------------------------------*/
 /**
  * Dump CXL LTSSM
  *
  * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this NBIO
  * @retval    
  */
  /*----------------------------------------------------------------------------------------*/
VOID
DumpCxlLtssm (
  IN      GNB_HANDLE  *GnbHandle
  )
{
  UINT32                          Index;
  UINT32                          Value32;

  IDS_HDT_CONSOLE (MAIN_FLOW, "  Dump Ltssm State: \n");
  for (Index = 0; Index < 7; Index++) {
    NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, (NBIO0_CXL_LC_STATUS0 + Index * 4)), &Value32, 0);
    IDS_HDT_CONSOLE (GNB_TRACE, "    Lcstate%d = 0x%08x\n", Index, Value32);
  }
}

 /**----------------------------------------------------------------------------------------*/
 /**
  * Dump CXL Registers
  *
  *
  *
  * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this NBIO
  *             DspPciAddr     CXL root port PCI address
  * @retval    
  */
  /*----------------------------------------------------------------------------------------*/
VOID
CxlRegistersTest (
  IN      GNB_HANDLE  *GnbHandle,
  IN      PCI_ADDR    DspPciAddr
  )
{
  UINT16                          CapPtr;
  UINT32                          Value;
   
  IDS_HDT_CONSOLE (GNB_TRACE, "CxlRegistersTest Enter \n");

  //DVSEC capability test
  CapPtr = FindPciCxlDvsecCapability (DspPciAddr.AddressValue, CXL_2_0_EXTENSIONS_DVSEC_ID, NULL);
  CapPtr = FindPciCxlDvsecCapability (DspPciAddr.AddressValue, CXL_2_0_REGISTER_LOCATOR_DVSEC_ID, NULL);
   
  //CXL Component Register Test
  NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, 0x07811000), &Value, 0);
  IDS_HDT_CONSOLE (GNB_TRACE, "  Component SMN 0x07811000 value = 0x%08X \n", Value);
   
  IDS_HDT_CONSOLE (GNB_TRACE, "CxlRegistersTest Exit \n");
}

 /**----------------------------------------------------------------------------------------*/
 /**
  * Retrain CXL link
  *
  *
  *
  * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this NBIO
  *
  * @retval    
  */
  /*----------------------------------------------------------------------------------------*/
VOID
CxlRetraining (
  IN      GNB_HANDLE  *GnbHandle
  )
{
  LINK_CONTROL_LINK_STATUS_REG_REGISTER      LinkStatus;
  LINK_CONTROL2_LINK_STATUS2_REG_REGISTER    LinkCntl2;

  IDS_HDT_CONSOLE (GNB_TRACE, "CxlRetraining Enter \n");
  NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_LINK_CONTROL2_LINK_STATUS2_REG), &LinkCntl2.Value, 0);
  LinkCntl2.Field.PCIE_CAP_TARGET_LINK_SPEED = HsioGen3;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_LINK_CONTROL2_LINK_STATUS2_REG), &LinkCntl2.Value, 0);

  NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_LINK_CONTROL_LINK_STATUS_REG), &LinkStatus.Value, 0);
  LinkStatus.Field.PCIE_CAP_RETRAIN_LINK = 1;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_LINK_CONTROL_LINK_STATUS_REG), &LinkStatus.Value, 0);  

  MicroSecondDelay (50000); //Delay 50ms
  IDS_HDT_CONSOLE (GNB_TRACE, "CxlRetraining Exit \n");
}

 /**----------------------------------------------------------------------------------------*/
 /**
  * Disable CXL root port
  *
  *
  *
  * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this NBIO
  *
  * @retval    HGPI_STATUS
  */
  /*----------------------------------------------------------------------------------------*/
VOID
DisableCxlRootPort (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  UINT32    Value;

  IDS_HDT_CONSOLE (GNB_TRACE, "Disable CXL root port \n");
    
  Value = (1 << IOHC_BRIDGE_CNTL_BridgeDis_OFFSET) | (1 << IOHC_BRIDGE_CNTL_CfgDis_OFFSET);
  Value |= (1 << IOHC_BRIDGE_CNTL_CrsEnable_OFFSET);
    
  NbioRegisterRMW (GnbHandle,
            TYPE_SMN,
            NBIO_SPACE (GnbHandle, NB_DEVINDCFG8_IOHC_Bridge_CNTL_ADDRESS_HYGX),
            (UINT32)~(IOHC_BRIDGE_CNTL_BridgeDis_MASK | IOHC_BRIDGE_CNTL_CfgDis_MASK | IOHC_BRIDGE_CNTL_CrsEnable_MASK),
            Value,
            0);
}

 /**----------------------------------------------------------------------------------------*/
 /**
  * Wait for CXL memory training down
  *
  *
  *
  * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this NBIO
  *
  * @retval    EFI_STATUS
  */
  /*----------------------------------------------------------------------------------------*/
EFI_STATUS
WaitforCxlMemTrainingDone (
  IN       GNB_HANDLE       *GnbHandle,
  OUT      BOOLEAN          *IsCxl11Mode
  )
{
  CXL_RCRB_FLEXBUS_CNTRL_STATUS_OFF_REGISTER FlexBusCtrlSts;
  UINT32                                     Timeout;

  Timeout = 0;
  do {
    NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_CXL_RCRB_FLEXBUS_CNTRL_STATUS_OFF), &FlexBusCtrlSts.Value, 0);
    if (FlexBusCtrlSts.Field.MEM_ENABLED == 1) {
      if (FlexBusCtrlSts.Field.CXL2P0_ENABLED == 0) {
        *IsCxl11Mode = TRUE;
      } else {
        *IsCxl11Mode = FALSE;
      }
      return EFI_SUCCESS;
    }
    MicroSecondDelay (1000); //Delay 1ms
    Timeout++;
  } while ((FlexBusCtrlSts.Field.MEM_ENABLED == 0) && (Timeout < CXL_MAX_DELAY_MS));

  return EFI_TIMEOUT;
}

EFI_STATUS
EFIAPI
HygonCxlPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                         Status; 
  PCIe_PLATFORM_CONFIG               *Pcie;
  GNB_HANDLE                         *GnbHandle;

  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonCxlPei Entry\n");
  
  if (IsEmeiPresent() == FALSE) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "No CXL, exit \n");
    PcdSetBoolS (PcdCfgCxlEnable, FALSE);
    return EFI_SUCCESS;
  }
      
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonNbioPcieServicesPpiGuid,
                             0,
                             NULL,
                             (VOID **)&mPcieServicesPpi
                             );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Found NbioPcieServicesPpi error status = %r \n", Status);
    return EFI_UNSUPPORTED;
  }

  mPcieServicesPpi->PcieGetTopology (mPcieServicesPpi, &mPciePlatformConfigHob);
  Pcie = &(mPciePlatformConfigHob->PciePlatformConfigHob);
  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    if (GnbHandle->CxlPresent) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "Init socket %d LogicalDie %d NBIO %d CXL\n", GnbHandle->SocketId, GnbHandle->LogicalDieId, GnbHandle->RbId);

      if (PcdGetBool (PcdCfgCxlEnable) == FALSE) {
        DisableCxlRootPort (GnbHandle);
      } else {
        CxlInterfaceInitSetting (GnbHandle);
        CommonCxlEqCfgBeforeTraining (GnbHandle);
      }
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
  
  if (PcdGetBool (PcdCfgCxlEnable)) {
    Status = (*PeiServices)->NotifyPpi (PeiServices, &mNotifyNbioCxlPpi);
    IDS_HDT_CONSOLE (MAIN_FLOW, "Notify CXL callback status %r \n", Status);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonCxlPei Exit\n");
  return Status;
}

 /*----------------------------------------------------------------------------------------*/
 /*
  *  Nbio Cxl init after HSIO training
  *
  * @param[in]     
  * @param[out]    
  */
EFI_STATUS NbioCxlInitCallbackAfterTraining (
    IN EFI_PEI_SERVICES           **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
    IN VOID                       *InvokePpi
)
{
  EFI_STATUS                                 Status; 
  PCIe_PLATFORM_CONFIG                       *Pcie;
  GNB_HANDLE                                 *GnbHandle;
  PCI_ADDR                                   CxlRc;
  UINT32                                     DeviceId;
  CXL_INFO_HOB_DATA                          CxlInfoHobData;
  UINT8                                      RcecIndex;
  UINT8                                      Cxl11Index;
  UINT8                                      NbioAvailCxl11Bus;
  UINT8                                      NbioMaxBus;
  UINT32                                     Value32;
  UINT8                                      ActiveCxlRootPortCount;
  BOOLEAN                                    IsCxl11Mode;
  LINK_CONTROL_LINK_STATUS_REG_REGISTER      LinkStatus;

  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioCxlInitCallbackAfterTraining Entry \n");

  RcecIndex = 0;
  Cxl11Index = 0;
  ActiveCxlRootPortCount = 0;
  ZeroMem (&CxlInfoHobData, sizeof (CXL_INFO_HOB_DATA));
  
  Pcie = &(mPciePlatformConfigHob->PciePlatformConfigHob);
  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    if (GnbHandle->CxlPresent) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "Init socket %d LogicalDie %d NBIO %d CXL \n", GnbHandle->SocketId, GnbHandle->LogicalDieId, GnbHandle->RbId);
      
      CommonCxlEqCfgAfterTraining (GnbHandle);

      //CXl 1.1 bus assign from NBIO highest bus
      NbioMaxBus = GnbHandle->BusLimit;
      NbioAvailCxl11Bus = NbioMaxBus;

      //Get CXL RC PCI address
      CxlRc.AddressValue = 0;
      CxlRc.Address.Bus = GnbHandle->Address.Address.Bus;
      CxlRc.Address.Device = HYGON_CXL_RC_DEV; 
      CxlRc.Address.Function = HYGON_CXL_RC_FUN;
      
      GnbLibPciRead (CxlRc.AddressValue, AccessWidth32, &DeviceId, NULL); 
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Found CXL RC [B%02X|D%02X|F%02X] VIDDID 0x%X \n", 
          CxlRc.Address.Bus, 
          CxlRc.Address.Device, 
          CxlRc.Address.Function, 
          DeviceId);
      if (DeviceId != HYGON_CXL_VIDDID) {  
        //CXL RC not present
        GnbHandle = GnbGetNextHandle (GnbHandle);
        continue;    
      }

      CxlRegistersTest (GnbHandle, CxlRc);

      DumpCxlLtssm (GnbHandle);
      //CxlRetraining (GnbHandle);   
      //DumpCxlLtssm (GnbHandle);

      //Check PCIE LINKSTATUS::DL_ACTIVE
      NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_LINK_CONTROL_LINK_STATUS_REG), &LinkStatus.Value, 0);
      if (LinkStatus.Field.PCIE_CAP_DLL_ACTIVE == 1) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "  CXL IO Link active \n");
        ActiveCxlRootPortCount++;
          
        Status = WaitforCxlMemTrainingDone (GnbHandle, &IsCxl11Mode);
        if (!EFI_ERROR(Status)) {
          IDS_HDT_CONSOLE (MAIN_FLOW, "  CXL memory Link training success \n");

          if (IsCxl11Mode) {
            IDS_HDT_CONSOLE (MAIN_FLOW, "  CXL memory trained 1.1 mode \n");
            CxlInfoHobData.Cxl11Info[Cxl11Index].Cxl11Index = Cxl11Index;
            CxlInfoHobData.Cxl11Info[Cxl11Index].SocketId = GnbHandle->SocketId;
            CxlInfoHobData.Cxl11Info[Cxl11Index].PhysicalDieId = GnbHandle->PhysicalDieId;
            CxlInfoHobData.Cxl11Info[Cxl11Index].LogicalDieId = GnbHandle->LogicalDieId;
            CxlInfoHobData.Cxl11Info[Cxl11Index].RbIndex = GnbHandle->RbId;
            CxlInfoHobData.Cxl11Info[Cxl11Index].InitState = CXL_NOT_CONFIGURED;
            ProbeAttachedCxl11Device(GnbHandle, CxlRc, &CxlInfoHobData.Cxl11Info[Cxl11Index], &NbioAvailCxl11Bus);
            Cxl11Index++;
            CxlInfoHobData.Cxl11Count++;
            
            NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, 0x780100C), &Value32, 0);
            IDS_HDT_CONSOLE (MAIN_FLOW, "  Test SMN Addr 0x780100C (DSP RCRB offset 0x0C) = 0x%08X \n", Value32);
            NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, 0x7801010), &Value32, 0);
            IDS_HDT_CONSOLE (MAIN_FLOW, "  Test SMN Addr 0x7801010 (DSP RCRB offset 0x10) = 0x%08X \n", Value32);
            NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, 0x7801018), &Value32, 0);
            IDS_HDT_CONSOLE (MAIN_FLOW, "  Test SMN Addr 0x7801018 (DSP RCRB offset 0x18) = 0x%08X \n", Value32);
            NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, 0x8002018), &Value32, 0);
            IDS_HDT_CONSOLE (MAIN_FLOW, "  Test SMN Addr 0x8002018 = 0x%08X \n", Value32);
          }
        }
      } else {
        IDS_HDT_CONSOLE (MAIN_FLOW, "  CXL IO Link not active \n");
        DisableCxlRootPort (GnbHandle);
      }
      
      //Set RCEC bus range for CXL 1.1 devices
      if (NbioAvailCxl11Bus < NbioMaxBus) {
        //Update NBIO avail bus limit
        GnbHandle->BusLimit = NbioAvailCxl11Bus;
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Update GnbHandle->BusLimit to 0x%02X \n", GnbHandle->BusLimit);
        
        CxlInfoHobData.Rcec[RcecIndex].SocketId = GnbHandle->SocketId;
        CxlInfoHobData.Rcec[RcecIndex].PhysicalDieId = GnbHandle->PhysicalDieId;
        CxlInfoHobData.Rcec[RcecIndex].LogicalDieId = GnbHandle->LogicalDieId;
        CxlInfoHobData.Rcec[RcecIndex].RbIndex = GnbHandle->RbId;
        CxlInfoHobData.Rcec[RcecIndex].RbBusNumber = (UINT8) GnbHandle->Address.Address.Bus;
        CxlInfoHobData.Rcec[RcecIndex].StartRciepBus = NbioAvailCxl11Bus + 1;
        CxlInfoHobData.Rcec[RcecIndex].EndRciepBus = NbioMaxBus;
        CxlInfoHobData.RcecCount++;
        
        InitRcecBusRange (GnbHandle, &CxlInfoHobData.Rcec[RcecIndex]);
        RcecIndex++;
      }  
    }  
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "  ActiveCxlRootPortCount = %d \n", ActiveCxlRootPortCount);
  if (ActiveCxlRootPortCount == 0) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Set PcdCfgCxlEnable = FALSE \n");
    PcdSetBoolS (PcdCfgCxlEnable, FALSE);
    return EFI_SUCCESS;
  }

  if ((CxlInfoHobData.Cxl11Count > 0) || (CxlInfoHobData.RcecCount > 0)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Create CXL info HOB, CXL 1.1 count %d, RCEC count %d \n", CxlInfoHobData.Cxl11Count, CxlInfoHobData.RcecCount);
    BuildGuidDataHob (&gHygonCxlInfoHobGuid, &CxlInfoHobData, sizeof (CXL_INFO_HOB_DATA));
  }
          
  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioCxlInitCallbackAfterTraining Exit\n");
  return EFI_SUCCESS;
}

