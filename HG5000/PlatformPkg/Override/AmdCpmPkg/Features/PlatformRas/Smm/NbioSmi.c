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


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include "RasSmmPciLib.h"
#include "AmdRas.h"
#include "AmdPlatformRasSmm.h"
#include "Protocol/FchSmmPeriodicalDispatch2.h"
#include <AmdCpmSmm.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
UINT32                                     mNbioSmiCount = 0;
UINT32                                     mNbioSmiCoolOff = 0;
UINT64                                     mNbioTscLast = 0;
SMI_MODE                                   mNbioSmiMode = INTERRUPT_MODE;
EFI_HANDLE                                 mNbioPollingModeHandle = NULL;
FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL      *mAmdNbioPeriodicalDispatch = NULL;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
NbioLogError (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  );

EFI_STATUS
NbioClrStatus (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  );

EFI_STATUS
NbioResetCounter (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  );

BOOLEAN
NbioSearchErr (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  );


EFI_STATUS
PcieErrorScan (
  UINT32    NbioGlobalStatusHi,
  UINT8     DieBusNum
  );

EFI_STATUS
PcieErrorLog (
  PCIE_PORT_PROFILE *PciePortProfile,
  IN UINT32         DieBusNum
  );

EFI_STATUS
SataErrorScan (
  UINT32 NbioGlobalStatusHi,
  UINT8 DieBusNum
  );

BOOLEAN
FindNbioError (
  VOID
  );

VOID 
ProcessNbioRasSmiSource (
  VOID
  );

EFI_STATUS
RasSmmRegisterNbioPeriodicSmi (
  VOID
  );

SCAN_STATUS
STATIC
RasDevSmmStsClr (
  IN       PCI_ADDR             Device,
  IN OUT   RAS_PCI_SCAN_DATA    *ScanData
  );

VOID
FillPcieInfo (
  PCI_ADDR      PciPortAddr,
  PCIE_ERROR_SECTION *PcieErrorSection  
  );
  
VOID
STATIC
PcieDevErrScanOnDevice (
  PCI_ADDR       Device,
  BOOLEAN        RootStatusSet
  );

VOID
STATIC
PcieDevErrScanOnFunction (
  PCI_ADDR       Function,
  BOOLEAN        RootStatusSet
  );

SCAN_STATUS
STATIC
PcieDevErrScanCallback (
  PCI_ADDR             Device,
  RAS_PCI_SCAN_DATA    *ScanData
  );

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          T A B L E    D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */
//A PCI-E port device/function loopup table.
PCIE_PORT_LIST gPciePortList[] = {
    {1, 1},
    {1, 2},
    {1, 3},
    {1, 4},
    {1, 5},
    {1, 6},
    {1, 7},
    {2, 1},
    {3, 1},
    {3, 2},
    {3, 3},
    {3, 4},
    {3, 5},
    {3, 6},
    {3, 7},
    {4, 1}
};


/*********************************************************************************
 * Name: AmdNbioRasSmiCallback
 *
 * Description
 *   NBIO RAS Error handle SMI call back function entry
 *   Perform NBIO Error handling
 *
 *
 * Arguments:
 *   DispatchHandle  : The handle of this callback, obtained when registering
 *   DispatchContext : Pointer to the FCH_SMM_APURAS_REGISTER_CONTEXT
 *
 * Returns:
 *   None
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
AmdNbioRasSmiCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       FCH_SMM_APURAS_REGISTER_CONTEXT   *MiscRegisterContext
  )
{

  RAS_NBIO_ERROR_INFO   RasNbioErrorInfo;
  UINT32                NbioGlobalStatusLo;
  UINT32                NbioGlobalStatusHi;
  BOOLEAN               RasThresholdPeriodicSmiEn;
  
  DEBUG ((EFI_D_ERROR, "AmdNbioRasSmiCallback	\n"));

  RasThresholdPeriodicSmiEn = mPlatformApeiData->PlatRasPolicy.RasThresholdPeriodicSmiEn;
// moved down to reduce debug messages
//  if (mNbioSmiMode == INTERRUPT_MODE) {
//    DEBUG ((EFI_D_ERROR, "NBIO SMI Error handler: Die %x, Bus %x\n", MiscRegisterContext->Die, MiscRegisterContext->Bus));
//  } else {
//    DEBUG ((EFI_D_ERROR, "POLLING_MODE NBIO RAS Error handler: Die %x, Bus %x\n", MiscRegisterContext->Die, MiscRegisterContext->Bus));
//  }
  
  //Check NBIO Global Error Status
  RasSmnRead ((UINT32)MiscRegisterContext->Bus, RAS_GLOBAL_STATUS_LO, &NbioGlobalStatusLo);
  RasSmnRead ((UINT32)MiscRegisterContext->Bus, RAS_GLOBAL_STATUS_HI, &NbioGlobalStatusHi);

  if (((NbioGlobalStatusLo & RAS_GLOBAL_STATUS_LO_MASK) == 0) && ((NbioGlobalStatusHi & RAS_GLOBAL_STATUS_HI_MASK) == 0)) {
    return EFI_SUCCESS;
  }

  //Error found!!
  RasNbioErrorInfo.NbioGlobalStatusLo = NbioGlobalStatusLo;
  RasNbioErrorInfo.NbioGlobalStatusHi = NbioGlobalStatusHi;
  RasNbioErrorInfo.Die = MiscRegisterContext->Die;
  RasNbioErrorInfo.DieBusNumber = MiscRegisterContext->Bus;

  if (mNbioSmiMode == INTERRUPT_MODE) {
    DEBUG ((EFI_D_ERROR, "NBIO SMI Error handler: Die %x, Bus %x\n", MiscRegisterContext->Die, MiscRegisterContext->Bus));
  } else {
    DEBUG ((EFI_D_ERROR, "POLLING_MODE NBIO RAS Error handler: Die %x, Bus %x\n", MiscRegisterContext->Die, MiscRegisterContext->Bus));
  }
  DEBUG ((EFI_D_ERROR, "NBIO Global Status Lo : 0x%08x\n", NbioGlobalStatusLo));
  DEBUG ((EFI_D_ERROR, "NBIO Global Status Hi : 0x%08x\n", NbioGlobalStatusHi));

  if (RasThresholdPeriodicSmiEn) {
    ProcessNbioRasSmiSource ();
  }

  //Scan Error Reporting Status by Type (UNCORR, CORR, UCP)
  //Right now only report first finding. make it loop for all errors.
  if ((NbioGlobalStatusLo & RAS_GLOBAL_STATUS_LO_MASK) != 0) {
    RasNbioErrorInfo.TypeId = NBIO_GRP_UNCORR;
    if (NbioSearchErr (&RasNbioErrorInfo)) {
      NbioLogError (&RasNbioErrorInfo);
      NbioClrStatus (&RasNbioErrorInfo);
    }

    RasNbioErrorInfo.TypeId = NBIO_GRP_CORR;
    if (NbioSearchErr (&RasNbioErrorInfo)) {
      NbioLogError (&RasNbioErrorInfo);
      NbioClrStatus (&RasNbioErrorInfo);
      NbioResetCounter (&RasNbioErrorInfo);
    }

    RasNbioErrorInfo.TypeId = NBIO_GRP_UCP;
    if (NbioSearchErr (&RasNbioErrorInfo)) {
      NbioLogError (&RasNbioErrorInfo);
      NbioClrStatus (&RasNbioErrorInfo);
      NbioResetCounter (&RasNbioErrorInfo);
    }
  }

  if ((NbioGlobalStatusHi & RAS_GLOBAL_STATUS_HI_MASK) != 0) {
    //PCI-E error detection
    //Scan active PCI-E port
    PcieErrorScan (NbioGlobalStatusHi, MiscRegisterContext->Bus);

    //SATA error detection
    SataErrorScan (NbioGlobalStatusHi, MiscRegisterContext->Bus);

    return EFI_SUCCESS;
  }

  return EFI_SUCCESS;
}

BOOLEAN
CheckSataEnabled (
  UINT8 DieBusNum
  )
{
  PCI_ADDR     Nbif1Addr;
  PCI_ADDR     SataAddr;
  UINT8        SecondaryBus;

  DEBUG ((EFI_D_ERROR, "NBIO Bus: %x\n", DieBusNum));
  Nbif1Addr.AddressValue = MAKE_SBDFO (0, DieBusNum, 8, 1, 0);
  if (RasPciIsDevicePresent (Nbif1Addr.AddressValue)) {
    SecondaryBus = PciRead8 (Nbif1Addr.AddressValue | 0x19);
    DEBUG ((EFI_D_ERROR, "NBIF1 secondary Bus: %x\n", SecondaryBus));
    if (SecondaryBus == 0) {
      //The secondary PCI bus has not assigned, it shouldn't be 0
      return FALSE;
    }
    SataAddr.AddressValue = MAKE_SBDFO (0, SecondaryBus, 0, 2, 0);
    if (RasPciIsDevicePresent (SataAddr.AddressValue)) {
      DEBUG ((EFI_D_ERROR, "SATA Controller Found !!!\n"));
      return TRUE;
    }
  }
  return FALSE;
}

EFI_STATUS
SataErrorScan (
  UINT32 NbioGlobalStatusHi,
  UINT8 DieBusNum
  )
{
  EFI_STATUS   Status = EFI_SUCCESS;
  UINT32       SataPortRasSts;
  UINT32       PortNum;

  //Check NBIF1 status
  if (0 != (NbioGlobalStatusHi & BIT17)) {
    //check if SataRasSupport and SATA enabled.
    if ( mPlatformApeiData->PlatRasPolicy.FchSataRasSupport && CheckSataEnabled (DieBusNum)) {
      DEBUG ((EFI_D_ERROR, "Scan SATA Error: Bus %x\n", DieBusNum));
      for ( PortNum = 0; PortNum < TAISHAN_SATA_PORT_NUM; PortNum++ ) {
        RasSmnRead (DieBusNum, FCH_SMN_SATA_CONTROL_SLOR + PortNum * 0x20 + 0x88, &SataPortRasSts);
        if ((SataPortRasSts & SATA_PAR_ERR_STS) == 0) {
          continue;
        }
        // Log Sata error

        //
        //Clear error status
        //
        RasSmnRW (DieBusNum, FCH_SMN_SATA_CONTROL_BAR5 + FCH_SATA_BAR5_REG130 + PortNum * 0x80, 0xFFFFFFFF, 0xFFFFFFFF);
        RasSmnRW (DieBusNum, FCH_SMN_SATA_CONTROL_BAR5 + FCH_SATA_BAR5_REG110 + PortNum * 0x80, 0xFFFFFFFF, 0x00);
        RasSmnRW (DieBusNum, FCH_SMN_SATA_CONTROL_SLOR + PortNum * 0x20 + 0x88, 0xFFFFFFFF, 0x00);
      }
    }
  }
  return Status;
}

EFI_STATUS
PcieErrorScan (
  UINT32 NbioGlobalStatusHi,
  UINT8 DieBusNum
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  UINT32                PcieGlobalStatus;
  UINT16                PciePortIndex;
  PCIE_PORT_PROFILE     *PciePortProfileInstance;

  PcieGlobalStatus = NbioGlobalStatusHi & 0xFFFF;

  PciePortProfileInstance = mPlatformApeiData->AmdPciePortMap->PciPortNumber;

  //Search active PCI-E port for error only
  for (PciePortIndex = 0; PciePortIndex < mPlatformApeiData->AmdPciePortMap->PortCount; PciePortIndex++, PciePortProfileInstance++) {
    if (PciePortProfileInstance->NbioBusNum != DieBusNum) {
      //find next
      continue;
    }
    if ((PcieGlobalStatus & (1 << PciePortProfileInstance->PciPhysicalPortNumber)) != 0) {
      DEBUG ((EFI_D_ERROR, "[RAS]PCI-E Error detected at Bus: 0x%x, Physical Port: %0d\n", DieBusNum, PciePortProfileInstance->PciPhysicalPortNumber));
      //PCI-E error detected
      Status = PcieErrorLog (PciePortProfileInstance,DieBusNum);
      if (Status == EFI_OUT_OF_RESOURCES) {
        DEBUG ((EFI_D_ERROR, "[RAS]PCI-E Error log Out of Resource!!!\n"));
      }
    }
  }

  return Status;
}


EFI_STATUS
RasSmmStsClr(
  PCI_ADDR          PciPortAddr
)
{
  UINT16        AerCapPtr;
  UINT16        SecPcieExtCapPtr;
  UINT8         PcieCapPtr;
  UINT32        PcieUncorrStatus;
  UINT32        PcieCorrStatus;
  UINT32        PcieRootStatus;
  UINT16        PcieDevStatus;
  UINT32        PcieLnkStatus;
  UINT32        RasRetryCounter;

  //Clear Status register
  DEBUG ((EFI_D_ERROR, "[RAS]Error Handle ClrSts @ Bus: 0x%x, Dev: 0x%x, Func: 0x%x, DLActive=%d\n", PciPortAddr.Address.Bus, PciPortAddr.Address.Device, PciPortAddr.Address.Function, (PciRead16(PciPortAddr.AddressValue + 0x6A) & BIT13 ? 1 : 0) ));

  
  AerCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, PCIE_EXT_AER_CAP_ID);
  SecPcieExtCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, SEC_PCIE_EXT_CAP_ID);
  PcieCapPtr = RasFindPciCapability (PciPortAddr.AddressValue, PCIE_CAP_ID);
  
  RasRetryCounter = mPlatformApeiData->PlatRasPolicy.RasRetryCnt;
  if (AerCapPtr != 0) {
    PcieUncorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR); 
    PcieCorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR);
    PcieRootStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_ROOT_STATUS_PTR);
    while ((PcieUncorrStatus !=0)||(PcieCorrStatus !=0) || (PcieRootStatus !=0)) {
      if (RasRetryCounter == 0) {
        break;
      }
      DEBUG ((EFI_D_ERROR, "[RAS] PCIE UnCorr Error Status : 0x%08x\n", PcieUncorrStatus));
      PciWrite32(PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR, PcieUncorrStatus);
      PcieUncorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR);
      
      DEBUG ((EFI_D_ERROR, "[RAS] PCIE Corr Error Status : 0x%08x\n", PcieCorrStatus));
      PciWrite32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR, PcieCorrStatus);
      PcieCorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR);
         
      if (RasGetPcieDeviceType (PciPortAddr) == PcieDeviceRootComplex) {
        DEBUG ((EFI_D_ERROR, "[RAS] PCIE Root Error Status : 0x%08x\n", PcieRootStatus));
        PciWrite32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_ROOT_STATUS_PTR, PcieRootStatus);
        PcieRootStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_ROOT_STATUS_PTR);
      }
      RasRetryCounter--;
    }
  }
  RasRetryCounter = mPlatformApeiData->PlatRasPolicy.RasRetryCnt;
  if (PcieCapPtr != 0) {
    PcieDevStatus = PciRead16 (PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_STATUS_PTR);
    while ((PcieDevStatus & 0xF) != 0) {
      if (RasRetryCounter == 0) {
        break;
      }
    DEBUG ((EFI_D_ERROR, "[RAS] PCIE Device Status : 0x%08x\n", PcieDevStatus));
    PciWrite16(PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_STATUS_PTR, PcieDevStatus);
    PcieDevStatus = PciRead16 (PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_STATUS_PTR);
    RasRetryCounter--;
    }
  }
  RasRetryCounter = mPlatformApeiData->PlatRasPolicy.RasRetryCnt;
  if (SecPcieExtCapPtr !=0 ) {
    PcieLnkStatus = PciRead32 (PciPortAddr.AddressValue + SecPcieExtCapPtr + PCIE_LANE_ERR_STATUS_PTR);
    while (PcieLnkStatus != 0) {
      if (RasRetryCounter == 0) {
        break;
      }
      DEBUG((EFI_D_ERROR, "[RAS] PCIE Lane Error Status : 0x%08x\n", PcieLnkStatus));
      PciWrite32 (PciPortAddr.AddressValue + SecPcieExtCapPtr + PCIE_LANE_ERR_STATUS_PTR, PcieLnkStatus);
      PcieLnkStatus = PciRead32 (PciPortAddr.AddressValue + SecPcieExtCapPtr + PCIE_LANE_ERR_STATUS_PTR);
      RasRetryCounter--;
    }
  }
  return EFI_SUCCESS;

}

SCAN_STATUS
STATIC
RasDevSmmStsClr (
  IN       PCI_ADDR             Device,
  IN OUT   RAS_PCI_SCAN_DATA    *ScanData
  )
{
    SCAN_STATUS             ScanStatus;
    PCIE_DEVICE_TYPE        DeviceType;
    ScanStatus = SCAN_SUCCESS;
    DEBUG ((EFI_D_ERROR, "[RAS] RasDevSmmStsClr for Device = %d:%d:%d\n",
      Device.Address.Bus,
      Device.Address.Device,
      Device.Address.Function
      ));
    ScanStatus = SCAN_SUCCESS;
    DeviceType = RasGetPcieDeviceType (Device);
    DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device type = 0x%x\n", DeviceType));
    switch (DeviceType) {
    case  PcieDeviceRootComplex:
    case  PcieDeviceDownstreamPort:
    case  PcieDeviceUpstreamPort:
      DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device root port found\n"));
      ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
      break;
    case  PcieDevicePcieToPcix:
      DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device PCIE to PCIx found\n"));
      ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
      break;
    case  PcieDeviceEndPoint:
    case  PcieDeviceLegacyEndPoint:
      DEBUG ((EFI_D_ERROR, "[RAS] PCI-E endpoint found\n"));
      RasSmmStsClr (Device);
      ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
      break;
    default:
      break;
    }
    return ScanStatus;
}


EFI_STATUS
PcieErrorLog (
  PCIE_PORT_PROFILE *PciePortProfile,
  IN UINT32         DieBusNum
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  PCI_ADDR      PciPortAddr;
  UINT16        AerCapPtr;
  UINT16        SecPcieExtCapPtr;
  UINT8         PcieCapPtr;
  UINT32        PcieUncorrStatus;
  UINT32        PcieUncorrMask;
  UINT32        PcieUncorrSeverity;
  UINT32        PcieCorrStatus;
  UINT32        PcieCorrMask;
  UINT32        ErrDataEntryCount;
  UINT32		SlotCapabilities;
  UINT32		SlotControl;
  UINT16		SlotStatus;
  BOOLEAN       IsRootPortError=FALSE;
  BOOLEAN       RootErrStatusSet = FALSE;
  UINT32        RootErrStatus;
  UINT8         PwrRsrCfg;
  RAS_PCI_SCAN_DATA  ScanData;
  
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE  *PcieErrStatusBlk;
  GENERIC_PCIE_AER_ERR_ENTRY                *GenPcieAerErrEntry;
  PCIE_ERROR_SECTION                        *PcieErrorSection;
  EFI_GUID      PcieErrSectionType = PCIE_SECT_GUID;

  PcieErrStatusBlk    = mPlatformApeiData->AmdPcieAerErrBlk;
  // Provide only one record per error report
  *((UINT32*)(&PcieErrStatusBlk->BlockStatus)) = 0;
  *((UINT32*)(&PcieErrStatusBlk->DataLength)) = 0;

  ErrDataEntryCount = PcieErrStatusBlk->BlockStatus.ErrorDataEntryCount;
  GenPcieAerErrEntry = (GENERIC_PCIE_AER_ERR_ENTRY *) ((UINTN ) PcieErrStatusBlk + sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE) + PcieErrStatusBlk->DataLength);
  PcieErrorSection = &GenPcieAerErrEntry->PcieAerErrorSection;

  PcieUncorrStatus = 0;
  PcieCorrStatus = 0;

  PciPortAddr.AddressValue = 0;
  PciPortAddr.Address.Bus = PciePortProfile->NbioBusNum;
  PciPortAddr.Address.Device = gPciePortList[PciePortProfile->PciPortNumber].Device;
  PciPortAddr.Address.Function = gPciePortList[PciePortProfile->PciPortNumber].Function;
  
  AerCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, PCIE_EXT_AER_CAP_ID);
  SecPcieExtCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, SEC_PCIE_EXT_CAP_ID);
  PcieCapPtr = RasFindPciCapability (PciPortAddr.AddressValue, PCIE_CAP_ID);

  DEBUG ((EFI_D_ERROR, "[RAS]Die %d Physical Port %d PCIE Cap Ptr : 0x%x\n", PciePortProfile->NbioDieNum, PciePortProfile->PciPhysicalPortNumber, PcieCapPtr));

  if ((sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE) + PcieErrStatusBlk->DataLength + sizeof (GENERIC_PCIE_AER_ERR_ENTRY)) > MAX_ERROR_BLOCK_SIZE) {
    RasSmmStsClr(PciPortAddr);
    return EFI_OUT_OF_RESOURCES;
  }

  CopyGuid ((EFI_GUID*)&GenPcieAerErrEntry->GenErrorDataEntry.SectionType, &PcieErrSectionType);
  GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_NONE;
  GenPcieAerErrEntry->GenErrorDataEntry.Revision = GENERIC_ERROR_REVISION;
  GenPcieAerErrEntry->GenErrorDataEntry.Flags = 0x01;
  GenPcieAerErrEntry->GenErrorDataEntry.ErrorDataLength = sizeof (PCIE_ERROR_SECTION);

  if (RasPciErrCheck(PciPortAddr, &RootErrStatusSet, &RootErrStatus)) {
    IsRootPortError = TRUE;
  }
  DEBUG ((EFI_D_ERROR, "[RAS] IsRootPortError = %d\n",IsRootPortError));
  if (!IsRootPortError && RootErrStatusSet) {
     RasSmmStsClr (PciPortAddr);
     //Scan and log device error
     ScanData.Buffer = (void *)&RootErrStatusSet;
     ScanData.RasScanCallback = PcieDevErrScanCallback;
     RasPciScanSecondaryBus (PciPortAddr, &ScanData);
  }else{
    //Root Port Error
    if (AerCapPtr != 0) {
      //Un-Correctable error
      PcieUncorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR);
      PcieUncorrMask = PciRead32(PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_MASK_PTR);
      PcieUncorrSeverity = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_SEVERITY_PTR);
      //PcieUncorrMask &= 0xffffbfff;
      if (0 != PcieUncorrStatus) {
        if (PcieUncorrStatus & PcieUncorrSeverity) {
          GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;
          PcieErrStatusBlk->ErrorSeverity = ERROR_SEVERITY_FATAL;
      	}else {
          GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_RECOVERABLE;
       	  PcieErrStatusBlk->ErrorSeverity = ERROR_RECOVERABLE;
      	}

      if(PcieUncorrStatus & BIT14)
      {    
          //Completion timeout err
          SlotCapabilities = PciRead32(PciPortAddr.AddressValue + PcieCapPtr + 0x14);
          if (SlotCapabilities & BIT6)
          {			        
            //Hotplug capable
            SlotControl = PciRead32(PciPortAddr.AddressValue + PcieCapPtr + 0x18);
            if (SlotControl & BIT10)
            {				
              //Power off
              GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_NONE;
              PcieErrStatusBlk->ErrorSeverity = ERROR_NONE;
            } else {
              if(PcieUncorrStatus==0x4000){
                GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_NONE;
                PcieErrStatusBlk->ErrorSeverity = ERROR_NONE;
              }
              SlotStatus = PciRead16(PciPortAddr.AddressValue + PcieCapPtr + 0x1A);
              if (!(SlotStatus & BIT6))
              {				
                //Presence Detect = Slot Empty
                GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_NONE;
                PcieErrStatusBlk->ErrorSeverity = ERROR_NONE;
              }
            }							  
          }			  
        }
	  //AMI PORTING END - check for hot removal
      } else {
        //Correctable error
      	PcieCorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR);
      	PcieCorrMask = PciRead32(PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_MASK_PTR);
      	if (0 != (PcieCorrStatus & ~PcieCorrMask)) {
          GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_CORRECTED;
          PcieErrStatusBlk->ErrorSeverity = ERROR_SEVERITY_CORRECTED;
      	}
      }

	  FillPcieInfo(PciPortAddr, PcieErrorSection);
    }

  switch (GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity) {
  case ERROR_SEVERITY_FATAL:
    if (PcieErrStatusBlk->BlockStatus.UncorrectableErrorValid) {
      PcieErrStatusBlk->BlockStatus.MultipleUncorrectableErrors = 1;
    } else {
      PcieErrStatusBlk->BlockStatus.UncorrectableErrorValid = 1;
    }
    break;
  case ERROR_SEVERITY_CORRECTED:
    if (PcieErrStatusBlk->BlockStatus.CorrectableErrorValid) {
      PcieErrStatusBlk->BlockStatus.MultipleCorrectableErrors = 1;
    } else {
      PcieErrStatusBlk->BlockStatus.CorrectableErrorValid = 1;
    }
    break;
  }

    if( PcieErrStatusBlk->ErrorSeverity != ERROR_NONE){
      //BYO Modify for RAS >>
      SendElogEventPcie((UINT8*)GenPcieAerErrEntry, PcieUncorrStatus, PcieCorrStatus);
      //BYO Modify for RAS <<
    }

    // Clear Root Port error status
    RasSmmStsClr(PciPortAddr);
    if (GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity == ERROR_SEVERITY_FATAL) {  
      // Trigger action - Warm reset
      IoWrite8 (0xCD6, 0x10);
      PwrRsrCfg = IoRead8 (0xCD7);
      PwrRsrCfg = PwrRsrCfg & 0xFD; //clear ToggleAllPwrGoodOnCf9
      IoWrite8 (0xCD7, PwrRsrCfg);
      IoWrite8(0xCF9, 0x2);
      IoWrite8(0xCF9, 0x6);
      CpuDeadLoop();
    }
    if( PcieErrStatusBlk->ErrorSeverity != ERROR_NONE){
	  PcieErrStatusBlk->BlockStatus.ErrorDataEntryCount++;
      PcieErrStatusBlk->DataLength += sizeof (GENERIC_PCIE_AER_ERR_ENTRY);
    }
  }
  
  return Status;
}

EFI_STATUS
RasSmmRegisterNbioSmi (
  VOID
  )
{
  EFI_STATUS                        Status;
  FCH_SMM_APURAS_DISPATCH_PROTOCOL  *AmdApuRasDispatch;
  FCH_SMM_APURAS_REGISTER_CONTEXT   ApuRasRegisterContext;
  EFI_HANDLE                        ApuRasHandle = NULL;
  UINT8                             Die;
  UINT8                             DieBusNum;
  UINT16                            VendorID;
  UINTN                             PciAddress;

  //
  //  Locate SMM SW dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmApuRasDispatchProtocolGuid,
                  NULL,
                  &AmdApuRasDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  //Register Error handler per Die.
  for (Die = 0; Die < MAX_DIE_SUPPORT; Die++) {
    PciAddress = ((Die + 24) << 15) + 0x000;
    VendorID = PciRead16 (PciAddress);
    if ((VendorID != AMD_VENDOR_ID) && (VendorID != HYGON_VID)) {
      continue;
    }

    PciAddress = ((Die + 24) << 15) + (DF_CFGADDRESSCNTL_FUNC << 12) + DF_CFGADDRESSCNTL_OFFSET;
    DieBusNum = PciRead8 (PciAddress);

    DEBUG ((EFI_D_ERROR, "[RAS] Install NBIO Error SMI callback handler : Die %x, Bus %x\n", Die, DieBusNum));

    //NBIO Error handle registry.
    ApuRasRegisterContext.Bus = DieBusNum;
    ApuRasRegisterContext.Die = Die;
    ApuRasRegisterContext.Order = 0x80;

    Status = AmdApuRasDispatch->Register (
                              AmdApuRasDispatch,
                              AmdNbioRasSmiCallback,
                              &ApuRasRegisterContext,
                              &ApuRasHandle
                              );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

BOOLEAN
NbioSearchErr (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  )
{
  UINT32 Index;
  UINT32 NbioParityErrorSts;
  UINT32 BaseAddr;
  UINT32 MaxGroupId;
  switch (RasNbioErrorInfo->TypeId) {
  case NBIO_GRP_UNCORR:
    BaseAddr = PARITY_ERROR_STATUS_UNCORR_GRP;
    MaxGroupId = MAX_UNCORR_GRP_NUM;
    break;
  case NBIO_GRP_CORR:
    BaseAddr = PARITY_ERROR_STATUS_CORR_GRP;
    MaxGroupId = MAX_CORR_GRP_NUM;
    break;
  case NBIO_GRP_UCP:
    BaseAddr = PARITY_ERROR_STATUS_UCP_GRP;
    MaxGroupId = MAX_UCP_GRP_NUM;
  }

  for (Index = 0; Index < MaxGroupId; Index++) {
    RasSmnRead ((UINT32)RasNbioErrorInfo->DieBusNumber,
                BaseAddr + (Index * 4),
                &NbioParityErrorSts);
    if (NbioParityErrorSts != 0) {
      RasNbioErrorInfo->GroupId = (UINT8)Index;
      RasNbioErrorInfo->NbioParityErrorStsAddr = BaseAddr + (Index * 4);
      RasNbioErrorInfo->NbioParityErrorSts = NbioParityErrorSts;
      return TRUE;
    }
  }
  return FALSE;
}


VOID
UpdateNbioErrDataEntry (
  IN       GENERIC_NBIO_ERR_ENTRY   *ErrDataEntry,
  IN       RAS_NBIO_ERROR_INFO      *RasNbioErrorInfo
  )
{
  EFI_GUID     NbioErrSectionType = NBIO_ERROR_SECT_GUID;

  CopyGuid ((EFI_GUID*)&ErrDataEntry->GenErrorDataEntry.SectionType, &NbioErrSectionType);
  if ((RasNbioErrorInfo->NbioGlobalStatusLo & BIT0) == 1) {
    ErrDataEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_CORRECTED;
  } else {
    ErrDataEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;
  }
  ErrDataEntry->GenErrorDataEntry.Revision = GENERIC_ERROR_REVISION;
  ErrDataEntry->GenErrorDataEntry.Flags = 0x01;
  ErrDataEntry->GenErrorDataEntry.ErrorDataLength = sizeof (AMD_NBIO_ERROR_RECORD);

  ErrDataEntry->NbioErrorSection.ValidationBits.Value = NBIO_VALID_BIT_MAP;
  ErrDataEntry->NbioErrorSection.BusId = RasNbioErrorInfo->DieBusNumber;
  ErrDataEntry->NbioErrorSection.ErrorSource.Field.Nbio = 1;
  ErrDataEntry->NbioErrorSection.ErrorType.Value = (UINT8) (RasNbioErrorInfo->NbioGlobalStatusLo & 0x0000000F);
  ErrDataEntry->NbioErrorSection.GroupId = RasNbioErrorInfo->GroupId;
  ErrDataEntry->NbioErrorSection.GroupType = RasNbioErrorInfo->TypeId;
  ErrDataEntry->NbioErrorSection.ParityErrSts = RasNbioErrorInfo->NbioParityErrorSts;
}

EFI_STATUS
NbioLogError (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  )
{
  EFI_STATUS                                    Status;
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE      *NbioErrStatusBlk;
  GENERIC_NBIO_ERR_ENTRY                        *ErrDataEntry;
  UINT32                                        ErrDataEntryCount;

  Status = EFI_SUCCESS;

  NbioErrStatusBlk    = mPlatformApeiData->AmdNbioErrBlk;
  ErrDataEntryCount = NbioErrStatusBlk->BlockStatus.ErrorDataEntryCount;

  // Provide only one record per error report
  *((UINT32*)(&NbioErrStatusBlk->BlockStatus)) = 0;
  *((UINT32*)(&NbioErrStatusBlk->DataLength)) = 0;


  ErrDataEntry      = (GENERIC_NBIO_ERR_ENTRY *) ((UINTN ) NbioErrStatusBlk + sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE) + NbioErrStatusBlk->DataLength);

  if ((sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE) + NbioErrStatusBlk->DataLength + sizeof (GENERIC_NBIO_ERR_ENTRY)) > MAX_ERROR_BLOCK_SIZE) {
    return EFI_OUT_OF_RESOURCES;
  }

  UpdateNbioErrDataEntry (ErrDataEntry, RasNbioErrorInfo);

  if ((RasNbioErrorInfo->NbioGlobalStatusLo & BIT0) == 1) {
    NbioErrStatusBlk->ErrorSeverity = ERROR_SEVERITY_CORRECTED;
    if (NbioErrStatusBlk->BlockStatus.CorrectableErrorValid) {
      NbioErrStatusBlk->BlockStatus.MultipleCorrectableErrors = 1;
    } else {
      NbioErrStatusBlk->BlockStatus.CorrectableErrorValid = 1;
    }
  } else {
    NbioErrStatusBlk->ErrorSeverity = ERROR_SEVERITY_FATAL;
    if (NbioErrStatusBlk->BlockStatus.UncorrectableErrorValid) {
      NbioErrStatusBlk->BlockStatus.MultipleUncorrectableErrors = 1;
    } else {
      NbioErrStatusBlk->BlockStatus.UncorrectableErrorValid = 1;
    }
  }

  NbioErrStatusBlk->BlockStatus.ErrorDataEntryCount++;
  NbioErrStatusBlk->DataLength += sizeof (GENERIC_NBIO_ERR_ENTRY);

  //BYO Modify for RAS >>
  SendElogEventNbio((UINT8*)RasNbioErrorInfo);
  //BYO Modify for RAS <<

  return EFI_SUCCESS;
}

EFI_STATUS
NbioClrStatus (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  )
{
  RasSmnWrite ((UINT32)RasNbioErrorInfo->DieBusNumber,
               RasNbioErrorInfo->NbioParityErrorStsAddr,
               &RasNbioErrorInfo->NbioParityErrorSts);

  return EFI_SUCCESS;
}

EFI_STATUS
NbioResetCounter (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  )
{
  UINT32        ParityCounterAddr;
  UINT32        MaxGroupId;
  UINT32        ParityCounterReset;

  switch (RasNbioErrorInfo->TypeId) {
  case NBIO_GRP_CORR:
    ParityCounterAddr = PARITY_COUNTER_CORR_GRP;
    MaxGroupId = MAX_CORR_GRP_NUM;
    break;
  case NBIO_GRP_UCP:
    ParityCounterAddr = PARITY_COUNTER_UCP_GRP;
    MaxGroupId = MAX_UCP_GRP_NUM;
  }

  if (RasNbioErrorInfo->GroupId > MaxGroupId) {
    return EFI_INVALID_PARAMETER;
  }

  ParityCounterAddr += (RasNbioErrorInfo->GroupId * 4);
  ParityCounterReset = BIT31;

  RasSmnWrite ((UINT32)RasNbioErrorInfo->DieBusNumber,
               ParityCounterAddr,
               &ParityCounterReset);

  return EFI_SUCCESS;
}

BOOLEAN
FindNbioError (
  VOID
  )
{
  EFI_STATUS                        Status;
  UINTN                             PciAddress;
  UINT8                             Die;
  UINT8                             DieBusNum;
  UINT16                            VendorID;
  UINT32                            NbioGlobalStatusLo;
  UINT32                            NbioGlobalStatusHi;
  FCH_SMM_APURAS_REGISTER_CONTEXT   ApuRasRegisterContext;
  BOOLEAN                           ErrorFound;

  ErrorFound = FALSE;
  DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - FindNbioError Entry\n"));
  for (Die = 0; Die < MAX_DIE_SUPPORT; Die++) {
    DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - Process Die: 0x%02x, ", Die));

    PciAddress = ((Die + 24) << 15) + 0x000;
    VendorID = PciRead16 (PciAddress);
    if (VendorID != AMD_VENDOR_ID && VendorID != HYGON_VID) {
      continue;
    }

    PciAddress = ((Die + 24) << 15) + (DF_CFGADDRESSCNTL_FUNC << 12) + DF_CFGADDRESSCNTL_OFFSET;
    DieBusNum = PciRead8 (PciAddress);
    DEBUG ((DEBUG_ERROR, "DieBusNum: 0x%02x.\n", DieBusNum));
    
    //Check NBIO Global Error Status
    RasSmnRead ((UINT32)DieBusNum, RAS_GLOBAL_STATUS_LO, &NbioGlobalStatusLo);
    RasSmnRead ((UINT32)DieBusNum, RAS_GLOBAL_STATUS_HI, &NbioGlobalStatusHi);

    if (((NbioGlobalStatusLo & RAS_GLOBAL_STATUS_LO_MASK) == 0) && ((NbioGlobalStatusHi & RAS_GLOBAL_STATUS_HI_MASK) == 0)) {
      continue;
    }
    
    ApuRasRegisterContext.Die = Die;
    ApuRasRegisterContext.Bus = DieBusNum;
    Status = AmdNbioRasSmiCallback (NULL, &ApuRasRegisterContext);
    DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - FindNbioError: Error Found in Die: 0x%02x\n", Die));
    ErrorFound = TRUE;
  }
  DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - FindNbioError Exit\n"));

  return ErrorFound;
}



// Goal... limit the system on average to 5 interrupts per 1000ms or less
// If we exceed the limit, enter periodic polling mode
//
// If we are in periodic polling mode, and no errors occur during the 1000ms
// time scale, return to interrupt mode.
//
// mNbioTscLast = <TSC>
// mNbioSmiMode = Interrupt (default)
// mNbioSmiCount = 0 (default)
// 
// SMI_threshold = PcdCpmSmiThreshold (default: 5 interrupts)
// SMI_scale = PcdCpmSmiScale (default: 1000 ms)
// 
// SmiRate (SmiPeriod) = SMI_scale / SMI_threshold
// mNbioSmiCoolOff = 0;
// 
// TimeSinceLastSmi (Uninitilized)
VOID 
ProcessNbioRasSmiSource (
  VOID
  )
{
  UINT64                  TimeSinceLastSmi;
  UINT32                  SmiRate;
  UINT64                  TscCurrent;
  UINT32                  Data32;
  BOOLEAN                 McaErrThreshEn;
  UINT16                  McaErrThreshCount;
  UINT32                  SmiThreshold;
  UINT32                  SmiScale;
  EFI_STATUS              Status;

  if (mNbioSmiMode != INTERRUPT_MODE)
    return;

  mNbioSmiCount++;
  DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] INTERRUPT_MODE - Entry: Before Leaky bucket process => mNbioSmiCount = 0x%08x\n", mNbioSmiCount));

  McaErrThreshEn = mPlatformApeiData->PlatRasPolicy.McaErrThreshEn;
  McaErrThreshCount = mPlatformApeiData->PlatRasPolicy.McaErrThreshCount;
  SmiThreshold = mPlatformApeiData->PlatRasPolicy.RasSmiThreshold;
  SmiScale = mPlatformApeiData->PlatRasPolicy.RasSmiScale;
  //Ex: SmiRate = (SmiScale: 1000, unit: ms)/(SmiThreshold: 5 interrupts) = 200ms 
  SmiRate = SmiScale/SmiThreshold;   

  // Leak events out of the leaky bucket based on the time since the last SMI
  TscCurrent = AsmReadMsr64(TSC);
  TimeSinceLastSmi = ConvertToMilliseconds((TscCurrent - mNbioTscLast));
  while ((TimeSinceLastSmi > SmiRate) && (mNbioSmiCount > 0)) {
    mNbioSmiCount--;
    TimeSinceLastSmi-=SmiRate;
  }
  
  // Save the current TSC to the last TSC
  mNbioTscLast = TscCurrent;
  DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI]   INTERRUPT_MODE: After Leaky bucket process => mNbioSmiCount = 0x%08x\n", mNbioSmiCount));
  if (mNbioSmiCount > SmiThreshold) {
    // This event caused the Leaky bucket to overflow, enable polled mode via periodic SMIs
    DEBUG ((DEBUG_ERROR, "  INTERRUPT_MODE: mNbioSmiCount: 0x%04x > SmiThreshold: 0x%04x => Leaky bucket overflow, Start switching to POLLING_MODE\n", 
                         mNbioSmiCount, SmiThreshold));

    // Try to turn on periodic SMI's at rate of SmiRate                  
    Status = RasSmmRegisterNbioPeriodicSmi ();
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] INTERRUPT_MODE - Exit: Register AmdNbioPeriodicSmiCallback failed: 0x%08x, cannot switch to POLLING_MODE.\n", Status));
      return;
    }

    mNbioSmiMode = POLLING_MODE;
    mNbioSmiCoolOff = SmiThreshold;
    
    //Disable SMI generation for NBIO SMI
    Data32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + 0xAC);  
    Data32 &= ~(BIT10 | BIT11);                             //Disable HW assertion SMI
    MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + 0xAC, Data32);

    DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] INTERRUPT_MODE - Exit: Successfully switched to POLLING_MODE\n"));
  } else {
    DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] INTERRUPT_MODE - Exit: Leaky bucket has not overflowed\n"));
  }

  return;
}

VOID 
ProcessNbioPeriodicSMI (
  VOID
  )
{
  UINT32                  Data32;
  UINT32                  SmiThreshold;
  BOOLEAN                 McaErrThreshEn;
  UINT16                  McaErrThreshCount;
  EFI_STATUS              Status = EFI_SUCCESS;
  UINTN                   PciAddress;
  UINT8                   Die;
  UINT8                   DieBusNum;
  UINT16                  VendorID;
  
  McaErrThreshEn = mPlatformApeiData->PlatRasPolicy.McaErrThreshEn;
  McaErrThreshCount = mPlatformApeiData->PlatRasPolicy.McaErrThreshCount;
  SmiThreshold = mPlatformApeiData->PlatRasPolicy.RasSmiThreshold;

  if (mNbioSmiMode == POLLING_MODE) {
    DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - Entry\n"));
    if (FindNbioError()) {
      // An error was detected, reset the threshold to max
      mNbioSmiCoolOff = SmiThreshold;
      DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI]  POLLING_MODE - Exit: An error was detected, reset the mNbioSmiCoolOff to max: 0x%08x\n", mNbioSmiCoolOff));
    } else {
      mNbioSmiCoolOff--;
      DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI]   POLLING_MODE: No Error, mNbioSmiCoolOff-- = 0x%08x\n", mNbioSmiCoolOff));
      if (mNbioSmiCoolOff == 0) {
        // If we go one full leaky bucket time scale with no errors, return to interrupt mode
        DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI]   POLLING_MODE: mNbioSmiCoolOff == 0x%08x => Go one full leaky bucket time scale with no errors => Start returning to INTERRUPT_MODE.\n", 
                                mNbioSmiCoolOff));
    
        // Try to turn off periodic SMI's for PFEH polling
        if (mNbioPollingModeHandle == NULL) {
          DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - Exit: cannot locate mNbioPollingModeHandle, fail to return to INTERRUPT_MODE\n"));
          return;
        }
        Status = mAmdNbioPeriodicalDispatch->UnRegister (
                                           mAmdNbioPeriodicalDispatch, 
                                           mNbioPollingModeHandle);
        if (EFI_ERROR(Status)) {
          DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - Exit: cannot UnrRgister AmdNbioPeriodicSmiCallback, fail to return to INTERRUPT_MODE\n"));
          return;
        }

        mNbioPollingModeHandle = NULL;
        mNbioSmiMode = INTERRUPT_MODE;
        mNbioSmiCount  = 0;
        
        //Enable SMI generation for NBIO SMI
        for (Die = 0; Die < MAX_DIE_SUPPORT; Die++) {
          PciAddress = ((Die + 24) << 15) + 0x000;
          VendorID = PciRead16 (PciAddress);
          if (VendorID != AMD_VENDOR_ID && VendorID != HYGON_VID) {
            continue;
          }
        
          PciAddress = ((Die + 24) << 15) + (DF_CFGADDRESSCNTL_FUNC << 12) + DF_CFGADDRESSCNTL_OFFSET;
          DieBusNum = PciRead8 (PciAddress);

          RasSmnRW (DieBusNum, RAS_GLOBAL_STATUS_LO, 0xfffffeff, BIT8);   //Clear HWSMI_STS
          RasSmnRW (DieBusNum, IOHC_INTERRUPT_EOI, 0xfffffffe, BIT0);   //Clear SMI_EOI
        }

        MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + 0x84, BIT21);  //Clear BIT21: ApuRasSmi
        Data32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + 0xAC);
        Data32 &= ~(BIT10 | BIT11);
        Data32 |= BIT10;
        MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + 0xAC, Data32); //Enable HW assertion SMI
        MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + 0x84, BIT21);  //Clear BIT21: ApuRasSmi

        DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - Exit: Successfully return to INTERRUPT_MODE\n"));
      } else {
      DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - Exit: No error duration time is less than SMI time scale\n"));
      }
    }
  }
  return;
}

EFI_STATUS
EFIAPI
AmdNbioPeriodicSmiCallback (
  IN       EFI_HANDLE                                DispatchHandle,
  IN       CONST FCH_SMM_PERIODICAL_REGISTER_CONTEXT *RegisterContext,
  IN OUT   EFI_SMM_PERIODIC_TIMER_CONTEXT            *PeriodicTimerContext,
  IN OUT   UINTN                                     *SizeOfContext
  )
{

  ProcessNbioPeriodicSMI ();

  return EFI_SUCCESS;
}

EFI_STATUS
RasSmmRegisterNbioPeriodicSmi (
  VOID
  )
{
  EFI_STATUS                               Status;
  FCH_SMM_PERIODICAL_REGISTER_CONTEXT      PeriodicalRegisterContext; 
  UINT32                                   SmiRate;
  UINT32                                   SmiThreshold;
  UINT32                                   SmiScale;

  SmiThreshold = mPlatformApeiData->PlatRasPolicy.RasSmiThreshold;
  SmiScale = mPlatformApeiData->PlatRasPolicy.RasSmiScale;

  //
  // Periodic Timer SMI Registration
  //
  Status = gSmst->SmmLocateProtocol (
                    &gFchSmmPeriodicalDispatch2ProtocolGuid,
                    NULL,
                    &mAmdNbioPeriodicalDispatch
                    );
  
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //SmiRate = (SmiScale: 1000, unit: ms)/(SmiThreshold: 5 interrupts) = 200ms
  SmiRate = SmiScale/SmiThreshold;

  //SmiTrig0, In FchSmmPeriodicalDispatcher.c:
  //  if (SmiTickInterval == LONG_TIMER_SMI_INTERVAL) then {FCH_SMI_REG98 |= BIT29}
  //  FCH_SMI_REG98[29] = 1 => SmiTimer to be SmiLongTimer register and long timer runs at 1 ms unit time.
  PeriodicalRegisterContext.SmiTickInterval  = LONG_TIMER_SMI_INTERVAL;
  
  //SmiTimer, FCH_SMI_REG96 = (UINT16) (Period / SmiTickInterval) & 0x7FFF;
  PeriodicalRegisterContext.Period           = SmiRate * LONG_TIMER_SMI_INTERVAL;
  
  //SmiTimer, FCH_SMI_REG96 |= SMI_TIMER_ENABLE
  PeriodicalRegisterContext.StartNow         = 1;

  mNbioPollingModeHandle = NULL;
  Status = mAmdNbioPeriodicalDispatch->Register (
                                     mAmdNbioPeriodicalDispatch,
                                     AmdNbioPeriodicSmiCallback,
                                     &PeriodicalRegisterContext,
                                     &mNbioPollingModeHandle
                                     );

  return Status;
}

VOID
FillPcieInfo (
  PCI_ADDR      PciPortAddr,
  PCIE_ERROR_SECTION *PcieErrorSection  
  )
{
    UINT32              Index;
    UINT8               PcieCapPtr;
    UINT16              AerCapPtr;
    PCIE_DEVICE_TYPE    PortType;
    
    AerCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, PCIE_EXT_AER_CAP_ID);
    PcieCapPtr = RasFindPciCapability (PciPortAddr.AddressValue, PCIE_CAP_ID);
    
    PortType = RasGetPcieDeviceType (PciPortAddr);
    PcieErrorSection->Validation.Value = 0xEF;
    PcieErrorSection->PortType = (UINT32)PortType;
    PcieErrorSection->Revision = 0x02;
    PcieErrorSection->CommandStatus = PciRead32 (PciPortAddr.AddressValue + 0x04);

    PcieErrorSection->DeviceId.VendorId = PciRead16 (PciPortAddr.AddressValue + 0x00);
    PcieErrorSection->DeviceId.DeviceId = PciRead16 (PciPortAddr.AddressValue + 0x02);
    PcieErrorSection->DeviceId.ClassCode[0] = PciRead8 (PciPortAddr.AddressValue + 0x09);
    PcieErrorSection->DeviceId.ClassCode[1] = PciRead8 (PciPortAddr.AddressValue + 0x0A);
    PcieErrorSection->DeviceId.ClassCode[2] = PciRead8 (PciPortAddr.AddressValue + 0x0B);
    PcieErrorSection->DeviceId.Function = (UINT8)PciPortAddr.Address.Function;
    PcieErrorSection->DeviceId.Device = (UINT8)PciPortAddr.Address.Device;
    PcieErrorSection->DeviceId.Segment = (UINT8)PciPortAddr.Address.Segment;
    PcieErrorSection->DeviceId.PrimaryBus = (UINT8)PciPortAddr.Address.Bus;
    PcieErrorSection->DeviceId.SecondaryBus =  \
            ((PortType <= PcieDeviceLegacyEndPoint))? 0x00:PciRead8 (PciPortAddr.AddressValue + 0x19);
    
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.Function : 0x%x\n", PcieErrorSection->DeviceId.Function));
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.Device : 0x%x\n", PcieErrorSection->DeviceId.Device));
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.Segment : 0x%x\n", PcieErrorSection->DeviceId.Segment));
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.PrimaryBus : 0x%x\n", PcieErrorSection->DeviceId.PrimaryBus));
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.SecondaryBus : 0x%x\n", PcieErrorSection->DeviceId.SecondaryBus));
    
    if (PcieCapPtr !=0) {
      PcieErrorSection->DeviceId.Slot = (UINT8)(PciRead32(PciPortAddr.AddressValue + PcieCapPtr + 0x14) >> 19);
      PcieErrorSection->BridgeCtrlStatus = (UINT32) (PciRead16 (PciPortAddr.AddressValue + 0x3E)) << 16 | (UINT32) (PciRead16 (PciPortAddr.AddressValue + 0x1E));

      for (Index = 0; Index < 15; Index++) {
        PcieErrorSection->CapabilityStructure.CapabilityData[Index] = \
                                            PciRead32 (PciPortAddr.AddressValue + (0x58 + (4 * Index)));
      }
    }
    
    if (AerCapPtr !=0) {
      for (Index = 0; Index < 24; Index++) {
        PcieErrorSection->AerInfo.AerInfoData[Index] = PciRead32 (PciPortAddr.AddressValue + (AerCapPtr + (4 * Index)));
      }
    }
}

VOID
STATIC
PcieDevErrScanOnFunction (
  PCI_ADDR       Function,
  BOOLEAN        RootStatusSet
  )
{
  UINT16      AerCapPtr;
  UINT32      PcieUncorrStatus;
  UINT32      PcieUncorrMask;
  UINT32      PcieUncorrSverity;
  BOOLEAN     PcieCorrErrorValid;
  UINT32      ErrDataEntryCount;
  UINT32      RootErrStatus;
  UINT8       PwrRsrCfg;
  
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE  *PcieErrStatusBlk;
  GENERIC_PCIE_AER_ERR_ENTRY                *GenPcieAerErrEntry;
  PCIE_ERROR_SECTION                        *PcieErrorSection;
  
  EFI_GUID    PcieErrSectionType = PCIE_SECT_GUID;
  
  PcieErrStatusBlk = mPlatformApeiData->AmdPcieAerErrBlk;
  ErrDataEntryCount = PcieErrStatusBlk->BlockStatus.ErrorDataEntryCount;
  GenPcieAerErrEntry = (GENERIC_PCIE_AER_ERR_ENTRY *) ((UINTN ) PcieErrStatusBlk + sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE) + PcieErrStatusBlk->DataLength);
  PcieErrorSection = &GenPcieAerErrEntry->PcieAerErrorSection;
  
  AerCapPtr = RasFindPcieExtendedCapability (Function.AddressValue, PCIE_EXT_AER_CAP_ID);

  CopyGuid ((EFI_GUID*)&GenPcieAerErrEntry->GenErrorDataEntry.SectionType, &PcieErrSectionType);
  GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_NONE;
  PcieErrStatusBlk->ErrorSeverity = ERROR_NONE;
  GenPcieAerErrEntry->GenErrorDataEntry.Revision = GENERIC_ERROR_REVISION;
  GenPcieAerErrEntry->GenErrorDataEntry.Flags = 0x01;
  GenPcieAerErrEntry->GenErrorDataEntry.ErrorDataLength = sizeof (PCIE_ERROR_SECTION);

  if (RasPciErrCheck(Function, &RootStatusSet, &RootErrStatus)) {
    if (AerCapPtr != 0) {
      PcieUncorrStatus = PciRead32 (Function.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR);
      PcieUncorrMask = PciRead32(Function.AddressValue + AerCapPtr + PCIE_UNCORR_MASK_PTR);
      PcieUncorrSverity = PciRead32 (Function.AddressValue + AerCapPtr + PCIE_UNCORR_SEVERITY_PTR);
      if (0 != (PcieUncorrStatus & ~PcieUncorrMask)) {
        if (PcieUncorrStatus & PcieUncorrSverity) {
          GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;
          PcieErrStatusBlk->ErrorSeverity = ERROR_SEVERITY_FATAL;
        }else {
          GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_RECOVERABLE;
          PcieErrStatusBlk->ErrorSeverity = ERROR_RECOVERABLE;
        }
      } else {
        PcieCorrErrorValid = PcieAerCorrErrCheck(Function, AerCapPtr);
        if (PcieCorrErrorValid) {
          GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_CORRECTED;
          PcieErrStatusBlk->ErrorSeverity = ERROR_SEVERITY_CORRECTED;
        }
      }
      FillPcieInfo(Function, PcieErrorSection);
      switch (GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity) {
      case ERROR_SEVERITY_FATAL:
        if (PcieErrStatusBlk->BlockStatus.UncorrectableErrorValid) {
          PcieErrStatusBlk->BlockStatus.MultipleUncorrectableErrors = 1;
        } else {
          PcieErrStatusBlk->BlockStatus.UncorrectableErrorValid = 1;
        }
        break;
      case ERROR_SEVERITY_CORRECTED:
        if (PcieErrStatusBlk->BlockStatus.CorrectableErrorValid) {
          PcieErrStatusBlk->BlockStatus.MultipleCorrectableErrors = 1;
        } else {
          PcieErrStatusBlk->BlockStatus.CorrectableErrorValid = 1;
        }
        break;
      }
    
      SendElogEventPcie((UINT8*)GenPcieAerErrEntry, PcieUncorrStatus, PcieCorrErrorValid);
      RasSmmStsClr(Function);
      
	  if (GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity == ERROR_SEVERITY_FATAL) {
	    // Trigger action - Warm reset 
	    IoWrite8 (0xCD6, 0x10);
	    PwrRsrCfg = IoRead8 (0xCD7);
	    PwrRsrCfg = PwrRsrCfg & 0xFD; //clear ToggleAllPwrGoodOnCf9
	    IoWrite8 (0xCD7, PwrRsrCfg);
	    IoWrite8(0xCF9, 0x2);
	    IoWrite8(0xCF9, 0x6);
	    CpuDeadLoop();
	  }
      PcieErrStatusBlk->BlockStatus.ErrorDataEntryCount++;
      PcieErrStatusBlk->DataLength += sizeof (GENERIC_PCIE_AER_ERR_ENTRY);
    }
  }
}

VOID
STATIC
PcieDevErrScanOnDevice (
  PCI_ADDR       Device,
  BOOLEAN        RootStatusSet
  )
{
  UINT8                   MaxFunc;
  UINT8                   CurrentFunc;

  MaxFunc = RasPciIsMultiFunctionDevice (Device.AddressValue) ? 7 : 0;
  for (CurrentFunc = 0; CurrentFunc <= MaxFunc; CurrentFunc++) {
    Device.Address.Function = CurrentFunc;
    DEBUG ((EFI_D_ERROR, "[RAS] Checking Device: %d:%d:%d\n",
            Device.Address.Bus,
            Device.Address.Device,
            Device.Address.Function
            ));
    if (RasPciIsDevicePresent (Device.AddressValue)) {
      DEBUG ((EFI_D_ERROR, "[RAS] Scan Device Error report for Device = 0x%x:0x%x:0x%x\n",
      Device.Address.Bus,
      Device.Address.Device,
      Device.Address.Function
      ));
      PcieDevErrScanOnFunction (Device,RootStatusSet);
    }
  }
}

SCAN_STATUS
STATIC
PcieDevErrScanCallback (
  PCI_ADDR             Device,
  RAS_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS             ScanStatus;
  PCIE_DEVICE_TYPE        DeviceType;
  BOOLEAN                 RootStatusSet;

  ScanStatus = SCAN_SUCCESS;
  DEBUG ((EFI_D_ERROR, "[RAS] PcieDevErrScanCallback for Device = %d:%d:%d\n",
          Device.Address.Bus,
          Device.Address.Device,
          Device.Address.Function
      ));
  ScanStatus = SCAN_SUCCESS;
  DeviceType = RasGetPcieDeviceType (Device);
  DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device type = 0x%x\n", DeviceType));
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device root port found\n"));
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  case  PcieDevicePcieToPcix:
    DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device PCIE to PCIx found\n"));
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    DEBUG ((EFI_D_ERROR, "[RAS] PCI-E endpoint found\n"));
    RootStatusSet = *(BOOLEAN *)ScanData->Buffer;
    DEBUG ((EFI_D_ERROR, "[RAS] RootStatusSet = %x\n",RootStatusSet));
    PcieDevErrScanOnDevice (Device,RootStatusSet);
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  default:
    break;
  }
  return ScanStatus;
}
