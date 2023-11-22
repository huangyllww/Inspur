/* $NoKeywords:$ */
/**
 * @file
 *
 * HygonCxl11Pei Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: HGPI     
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
#include <HygonCxlRegOffset.h>
#include <HygonCxlRegStruct.h>
#include <GnbRegisters.h>
#include <FabricRegistersST.h>
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
#include <Library/FabricRegisterAccLib.h>
#include <Ppi/NbioBaseServicesSTPpi.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Guid/GnbNbioBaseSTInfoHob.h>
#include <IndustryStandard/Cxl.h>
#include <IndustryStandard/Cxl20.h>         // byo230928 +

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define FILECODE        NBIO_CXL_HYGONCXLPEI_INITCXL11PEI_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */

 /*----------------------------------------------------------------------------------------*/
 /*
  *  Routine to force CXL 2.0 work in CXL 1.1 mode
  *
  *
  *
  * @param[in]     DspPciAddr    CXL RootComplex or DSP PCI address
  * @param[out]    *Cxl11Dev     Cxl 1.1 device pointer
  */
EFI_STATUS
ConvertCxl20ToCxl11Mode (
  IN      PCI_ADDR                DspPciAddr,
  IN      CXL11_INFO             *Cxl11Dev
  )
{
  UINT16                          ExtensionCapPtr;
  CXL_2_0_DVSEC_CXL_RCRB_BASE_LOW Cxl11RcrbLowAddr;
  UINT32                          Cxl11RcrbHighAddr;
  FABRIC_TARGET                   MmioTarget;
  FABRIC_MMIO_ATTRIBUTE           MmioAttr;
  UINT64                          RcrbBase;
  UINT64                          RcrbSize;
  EFI_STATUS                      Status;
  UINT32                          Value32;
     
  ExtensionCapPtr = FindPciCxlDvsecCapability (DspPciAddr.AddressValue, CXL_2_0_EXTENSIONS_DVSEC_ID, NULL);
  if (ExtensionCapPtr == 0) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Not found CXL 2.0 Extension capability \n");
    return EFI_UNSUPPORTED;
  }
     
  MmioTarget.TgtType = TARGET_PCI_BUS;
  MmioTarget.SocketNum = 0;
  MmioTarget.DieNum = 0;
  MmioTarget.RbNum = 0;
  MmioTarget.PciBusNum = (UINT16) DspPciAddr.Address.Bus;
     
  MmioAttr.ReadEnable = 1;
  MmioAttr.WriteEnable = 1;
  MmioAttr.NonPosted = 0;
  MmioAttr.MmioType = NON_PCI_DEVICE_BELOW_4G;
     
  RcrbSize = CXL_RCRB_SIZE;     
  Status = FabricAllocateMmio (&RcrbBase, &RcrbSize, ALIGN_8K, MmioTarget, &MmioAttr);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Allocate RCRB Space fail (%r)\n", Status);
    Cxl11Dev->DspRcrb = 0;
    Cxl11Dev->UspRcrb = 0;
    return Status;
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "Allocate RCRB Base = 0x%lx \n", RcrbBase);
     
  Cxl11RcrbHighAddr = (UINT32) RShiftU64 (RcrbBase, 32);
  GnbLibPciWrite ( DspPciAddr.AddressValue | (ExtensionCapPtr + CXL_RCRB_BASE_HIGH_OFFSET), 
             AccessWidth32, 
             &Cxl11RcrbHighAddr, 
             NULL);
     
  Cxl11RcrbLowAddr.Uint32 = (UINT32) (RcrbBase & 0xFFFFE000);
  Cxl11RcrbLowAddr.Bits.RcrbEnable = 1;
  GnbLibPciWrite ( DspPciAddr.AddressValue | (ExtensionCapPtr + CXL_RCRB_BASE_LOW_OFFSET), 
             AccessWidth32, 
             &Cxl11RcrbLowAddr.Uint32, 
             NULL);
     
  IDS_HDT_CONSOLE (MAIN_FLOW, "Switch to CXL 1.1 mode success \n");

  Cxl11Dev->DspRcrb = (UINT32)RcrbBase + CXL_DSP_RCRB_OFFSET;
  Cxl11Dev->UspRcrb = (UINT32)RcrbBase + CXL_USP_RCRB_OFFSET;

  Value32 = CXL_REG32 (Cxl11Dev->DspRcrb, 0);
  IDS_HDT_CONSOLE (MAIN_FLOW, "RCRB DSP First DW = 0x%X \n", Value32);

  Value32 = CXL_REG32 (Cxl11Dev->DspRcrb, 0x18);
  IDS_HDT_CONSOLE (MAIN_FLOW, "RCRB DSP offset 0x18 = 0x%X \n", Value32);
     
  Value32 = CXL_REG32 (Cxl11Dev->UspRcrb, 0);
  IDS_HDT_CONSOLE (MAIN_FLOW, "RCRB USP First DW = 0x%X \n", Value32);
  
  return EFI_SUCCESS;
}
 
/**
 * Allocate MMIO for CXL RC and 1.1 device
@param[in] Cxl11Dev    CXL 1.1 device structure pointer

@retval
**/
EFI_STATUS 
AllocateMmioForCxl11 (
  IN     CXL11_INFO              *Cxl11Dev
  )
{
  FABRIC_TARGET                   MmioTarget;
  FABRIC_MMIO_ATTRIBUTE           MmioAttr;
  UINT64                          CxlMmioBase;
  UINT64                          CxlMmioSize;
  EFI_STATUS                      Status;
  UINT32                          Value;

  Cxl11Dev->DspMemBar0 = 0;
  Cxl11Dev->UspMemBar0 = 0;
  
  //Allocate MMIO for DSP component registers
  MmioTarget.TgtType = TARGET_PCI_BUS;
  MmioTarget.SocketNum = 0;
  MmioTarget.DieNum = 0;
  MmioTarget.RbNum = 0;
  MmioTarget.PciBusNum = (UINT16) Cxl11Dev->ParentPciAddr.Address.Bus;
  
  MmioAttr.ReadEnable = 1;
  MmioAttr.WriteEnable = 1;
  MmioAttr.NonPosted = 0;
  MmioAttr.MmioType = NON_PCI_DEVICE_BELOW_4G;
    
  CxlMmioSize = CXL_COMPONENT_REG_BLOCK_SIZE;  //DSP COMPONENT register size   
  
  Status = FabricAllocateMmio (&CxlMmioBase, &CxlMmioSize, ALIGN_64K, MmioTarget, &MmioAttr);
  if (!EFI_ERROR (Status)) {
    Cxl11Dev->DspMemBar0 = (UINT32)CxlMmioBase;
    GnbLibPciWrite ( Cxl11Dev->ParentPciAddr.AddressValue | CXL_MEMBAR0_LOW_OFFSET, 
             AccessWidth32, 
             &Cxl11Dev->DspMemBar0, 
             NULL);

    Value = 0;
    GnbLibPciWrite ( Cxl11Dev->ParentPciAddr.AddressValue | CXL_MEMBAR0_HIGH_OFFSET, 
             AccessWidth32, 
             &Value, 
             NULL);

    IDS_HDT_CONSOLE (MAIN_FLOW, "Set DSP MEMBAR0 = 0x%x \n", Cxl11Dev->DspMemBar0);
    IDS_HDT_CONSOLE (MAIN_FLOW, "DSP MEMBAR0 offset 0x1000 = 0x%X \n", CXL_REG32 (Cxl11Dev->DspMemBar0, 0x1000));
  }
  
  return Status;
}
 
/**
 * CXL 1.1 or 2.0 ARB MUX Init
@param[in] ComponentRegBase   CXL component register MMIO base
 
@retval
**/
void 
CxlArbMuxInit (
  IN     CXL11_INFO                        *Cxl11Dev  
  )
{
  CXL_IO_ARBITRATION_CONTROL                IoArbitration;
  CXL_CACHE_MEMORY_ARBITRATION_CONTROL      CmArbitration;
  UINT64                                    ArbMuxRegBase;
     
  if (Cxl11Dev->DspMemBar0 == 0) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Error : Cxl11Dev->DspMemBar0 is NULL \n");
    return;
  }
  ArbMuxRegBase = Cxl11Dev->DspMemBar0 + CXL_ARB_MUX_OFFSET;
  
  IoArbitration.Uint32 = CXL_REG32 (ArbMuxRegBase, CXL_IO_ARBITRATION_CONTROL_OFFSET);
  IDS_HDT_CONSOLE (MAIN_FLOW, "Io Arbitration is 0x%X \n", IoArbitration.Bits.WeightedRoundRobinArbitrationWeight);
     
  CmArbitration.Uint32 = CXL_REG32 (ArbMuxRegBase, CXL_CACHE_MEMORY_ARBITRATION_CONTROL_OFFSET);
  IDS_HDT_CONSOLE (MAIN_FLOW, "Cache and memory Arbitration is 0x%X \n", CmArbitration.Bits.WeightedRoundRobinArbitrationWeight);
}
 
/*
 * Allocate bus for CXL 1.1 device
@param[in] DspPciAddr         DSP PCI Address
           *AvailBus          Pointer to available bus number

@retval    CXL 1.1 device bus number
*/
UINT8 AllocateBusForCxl11Device (
  IN      PCI_ADDR            DspPciAddr,
  IN      UINT8               *AvailBus
  )
{
  UINT8    BusNumber;
  UINT16   Value16;
    
  BusNumber = *AvailBus;

  GnbLibPciWrite ( DspPciAddr.AddressValue | SECONDARY_BUS_REG_OFFSET, 
             AccessWidth8, 
             &BusNumber, 
             NULL);

  GnbLibPciWrite ( DspPciAddr.AddressValue | SUBORDINATE_BUS_REG_OFFSET, 
             AccessWidth8, 
             &BusNumber, 
             NULL);
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "Allocate bus 0x%X for Cxl11 device\n", BusNumber);
  
  //Enable DSP Bus master and memory access
  GnbLibPciRead ( DspPciAddr.AddressValue | CXL_COMMAND_OFFSET, 
             AccessWidth16, 
             &Value16, 
             NULL);
  Value16 |= (BIT1 + BIT2);
  GnbLibPciWrite ( DspPciAddr.AddressValue | CXL_COMMAND_OFFSET, 
             AccessWidth16, 
             &Value16, 
             NULL);

  *AvailBus = *AvailBus - 1;

  return BusNumber;
}
 
/*----------------------------------------------------------------------------------------*/
/*
  *   Routine to probe CXL 1.1 Device Connected to CXL 2.0 Capable Downstream Port
  *
  *
  *
  * @param[in]     DspPciAddr    CXL Root Complex or DSP PCI address
  *                *Cxl11Dev     Pointer to CXL1.1 device info
  *                *AvailBus     Pointer to available bus number
 */
EFI_STATUS
ProbeAttachedCxl11Device (
  IN      GNB_HANDLE *GnbHandle,
  IN      PCI_ADDR    DspPciAddr,
  IN OUT  CXL11_INFO *Cxl11Dev,
  IN OUT  UINT8      *AvailBus
  )
{
  EFI_STATUS                      Status;

  IDS_HDT_CONSOLE (MAIN_FLOW, "ProbeAttachedCxl11Device Entry\n");
     
  Cxl11Dev->ParentPciAddr.AddressValue = DspPciAddr.AddressValue;
          
  //Allocate bus for CXL 1.1 device
  Cxl11Dev->PciAddr.AddressValue = 0;
  Cxl11Dev->PciAddr.Address.Bus = AllocateBusForCxl11Device (DspPciAddr, AvailBus);
  Cxl11Dev->PciAddr.Address.Device = CXL_DEV_DEV; 
  Cxl11Dev->PciAddr.Address.Function = CXL_DEV_FUNC;

  IsCxlDevice (Cxl11Dev->PciAddr);
  
  //Allocate MMIO for CXL RC and device
  AllocateMmioForCxl11 (Cxl11Dev);
  
  //Force to CXL 1.1 mode
  Status = ConvertCxl20ToCxl11Mode (DspPciAddr, Cxl11Dev);
  if (Status != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  if (!IsCxlDevice (Cxl11Dev->PciAddr)) {
    return EFI_DEVICE_ERROR;
  }

  //Programs ARB/MUX arbitration control registers 
  CxlArbMuxInit (Cxl11Dev);

  Cxl11Dev->InitState = CXL_CONFIGURED;
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "ProbeAttachedCxl11Device Exit \n");  
  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/*
  *   Routine to init RCEC bus range
  *
  *
  *
  * @param[in]     DspPciAddr    CXL RootComplex or DSP PCI address
 */
void
InitRcecBusRange (
  IN      GNB_HANDLE *GnbHandle,
  IN      RCEC_INFO  *RcecInfo
  )
{
  PCIE_RCECEPA_ASSOCI_REGISTER  NextLastBus;
  RCEC_CFG_REGISTER             RcecCfg;
    
  NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_RCEC_CFG), &RcecCfg, 0);
  RcecCfg.Field.HWINIT_WR_LOCK = 0;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_RCEC_CFG), &RcecCfg, 0);
      
  NextLastBus.Value = 0;
  NextLastBus.Field.NEXT_BUS = (UINT32) RcecInfo->StartRciepBus;
  NextLastBus.Field.LAST_BUS = (UINT32) RcecInfo->EndRciepBus;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_PCIE_RCECEPA_ASSOCI_ADDRESS), &NextLastBus, 0);
      
  RcecCfg.Field.HWINIT_WR_LOCK = 1;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_RCEC_CFG), &RcecCfg, 0);
      
  IDS_HDT_CONSOLE (MAIN_FLOW, "  Set RCEC bus range 0x%X ~ 0x%X \n", NextLastBus.Field.NEXT_BUS, NextLastBus.Field.LAST_BUS);
}
