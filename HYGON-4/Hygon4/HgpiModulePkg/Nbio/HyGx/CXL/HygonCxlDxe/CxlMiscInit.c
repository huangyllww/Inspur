/* $NoKeywords:$ */
/**
 * @file
 *
 * HygonCxlDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonCxlDxe
 * @e \$Revision: 312065 $   @e \$Date: 2022-11-08 16:17:05 -0600 (Nov, 8 Tue 2022) $
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
#include <HygonCxlDxe.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/HygonNbioPcieServicesProtocol.h>
#include <GnbHsio.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Guid/GnbCxlInfoHob.h>
#include <GnbRegisters.h>
#include <GnbPcieAer.h>
#include <HygonCxlRegOffset.h>
#include <HygonCxlRegStruct.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/UefiLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/GnbLib.h>
#include <Library/HygonPspHpobLib.h>
#include <Library/HobLib.h>
#include <Library/GnbPciLib.h>
#include <Library/PcieMiscCommLib.h>
#include <Protocol/FabricTopologyServices.h>

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define FILECODE        NBIO_CXL_HYGONCXLDXE_CXLMISCINIT_FILECODE

NBIO_PCIe_PORT_AER_CONFIG mCxlAerConfig = { 
      0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      3,          ///< PciDev
      1,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
};

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------------------*/
/**
 * Check CXL root Port link Status
 *
 *
 *
 * @param[in]  GnbHandle       Pointer to the Silicon Descriptor for this NBIO
 * @retval                     TRUE if link active
 */
BOOLEAN
CheckCxlRootPortLinkStatus (
  IN       GNB_HANDLE  *GnbHandle
  )
{
   LINK_CONTROL_LINK_STATUS_REG_REGISTER      LinkStatus;
   
   //Check PCIE LINKSTATUS::DL_ACTIVE
   NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_LINK_CONTROL_LINK_STATUS_REG), &LinkStatus.Value, 0);
   if (LinkStatus.Field.PCIE_CAP_DLL_ACTIVE == 1) {
     return TRUE;
   } else {
     return FALSE;
   }
}

 /*----------------------------------------------------------------------------------------*/
 /**
  * Get CXL root Port link Status
  *
  *
  *
  * @param[in]  GnbHandle       Pointer to the Silicon Descriptor for this NBIO
  * @retval                     Cxl Root Port Link Width 4:X4, 8:X8, 16:X16
  *                            
  */                           
  
 UINT8
 GetCxlRootPortLinkWidth (
   IN       GNB_HANDLE  *GnbHandle
   )
 {
    LINK_CONTROL_LINK_STATUS_REG_REGISTER      LinkStatus;
    UINT8                                      LinkWidth;
    
    //Check PCIE LINKSTATUS::DL_ACTIVE
    NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_LINK_CONTROL_LINK_STATUS_REG), &LinkStatus.Value, 0);
    LinkWidth = (UINT8) LinkStatus.Field.PCIE_CAP_NEGO_LINK_WIDTH;
    
    return LinkWidth;
 }

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to configure MaxPayloadSize on CXL interface
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this NBIO
 *
 */
 /*----------------------------------------------------------------------------------------*/

VOID
CxlMaxPayloadInterface (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  UINT8                     MaxPayload;
  UINT8                     LinkWidth;
  PCI_ADDR                  CxlRc;
  
  IDS_HDT_CONSOLE (GNB_TRACE, "CxlMaxPayloadInterface Enter\n");
  
  //Get CXL RC PCI address
  CxlRc.AddressValue = 0;
  CxlRc.Address.Bus = GnbHandle->Address.Address.Bus;
  CxlRc.Address.Device = HYGON_CXL_RC_DEV; 
  CxlRc.Address.Function = HYGON_CXL_RC_FUN;
  
  if (CheckCxlRootPortLinkStatus (GnbHandle)) {
    LinkWidth = GetCxlRootPortLinkWidth (GnbHandle);
    if (LinkWidth >= 8) {
      MaxPayload =  MAX_PAYLOAD_256;
    } else {
      MaxPayload =  MAX_PAYLOAD_128;
    }

    IDS_HDT_CONSOLE (GNB_TRACE, "  Link width %d Device = %d:%d:%d MaxPayload = %x\n", LinkWidth,
        CxlRc.Address.Bus, 
        CxlRc.Address.Device, 
        CxlRc.Address.Function, 
        MaxPayload);
    
    PcieSetMaxPayload (CxlRc, MaxPayload, NULL);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "CxlMaxPayloadInterface Exit\n");
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to enable Clock Power Managment
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this NBIO
 *
 */
 /*----------------------------------------------------------------------------------------*/

VOID
CxlAspmInterface (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCI_ADDR                  CxlRc;
  UINT8                     AspmType;
  
  IDS_HDT_CONSOLE (GNB_TRACE, "CxlAspmInterface Enter\n");
  if (CheckCxlRootPortLinkStatus (GnbHandle)) {
    //Get CXL RC PCI address
    CxlRc.AddressValue = 0;
    CxlRc.Address.Bus = GnbHandle->Address.Address.Bus;
    CxlRc.Address.Device = HYGON_CXL_RC_DEV; 
    CxlRc.Address.Function = HYGON_CXL_RC_FUN;
    
    AspmType = PcdGet8 (PcdCxlAspm);
    PcieLinkAspmEnable (CxlRc, AspmType, NULL);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "CxlAspmInterface Exit\n");
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to init CXL AER 
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this NBIO
 *
 */
 /*----------------------------------------------------------------------------------------*/

VOID
CxlAerInit (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  NBIO_PCIe_PORT_AER_CONFIG  *PortAerConfig;
  ROOT_CONTROL_ROOT_CAPABILITIES_REG_REGISTER RootControl;
  CORR_ERR_MASK_OFF_REGISTER                  CorrErrMask;
  UNCORR_ERR_MASK_OFF_REGISTER                UncorrErrMask;
  UNCORR_ERR_SEV_OFF_REGISTER                 UncorrErrSeverity;
  
  IDS_HDT_CONSOLE (GNB_TRACE, "CxlAerInit Enter\n");
  
  PortAerConfig = &mCxlAerConfig;
  if (PortAerConfig->PortAerEnable == 0) {
    //Disable AER error reporting
    NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_ROOT_CONTROL_ROOT_CAPABILITIES_REG), &RootControl.Value, 0);
    RootControl.Field.PCIE_CAP_SYS_ERR_ON_CORR_ERR_EN = 0;
    RootControl.Field.PCIE_CAP_SYS_ERR_ON_NON_FATAL_ERR_EN = 0;
    RootControl.Field.PCIE_CAP_SYS_ERR_ON_FATAL_ERR_EN = 0;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_ROOT_CONTROL_ROOT_CAPABILITIES_REG), &RootControl.Value, GNB_REG_ACC_FLAG_S3SAVE);
    
  } else if (PortAerConfig->PortAerEnable == 1) {
    //Set Correctable Error Mask
    NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_CORR_ERR_MASK_OFF), &CorrErrMask.Value, 0);
    CorrErrMask.Field.BAD_TLP_MASK = PortAerConfig->CorrectableMask.Field.BadTLPMask;
    CorrErrMask.Field.BAD_DLLP_MASK = PortAerConfig->CorrectableMask.Field.BadDLLPMask;
    CorrErrMask.Field.REPLAY_NO_ROLEOVER_MASK = PortAerConfig->CorrectableMask.Field.ReplayNumberRolloverMask;
    CorrErrMask.Field.RPL_TIMER_TIMEOUT_MASK = PortAerConfig->CorrectableMask.Field.ReplayTimerTimeoutMask;
    CorrErrMask.Field.ADVISORY_NON_FATAL_ERR_MASK = PortAerConfig->CorrectableMask.Field.AdvisoryNonFatalErrorMask;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_CORR_ERR_MASK_OFF), &CorrErrMask.Value, GNB_REG_ACC_FLAG_S3SAVE);
    
    //Set Unorrectable Error Mask
    NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_UNCORR_ERR_MASK_OFF), &UncorrErrMask.Value, 0);
    UncorrErrMask.Field.DL_PROTOCOL_ERR_MASK = PortAerConfig->UncorrectableSeverity.Field.DataLinkProtocolErrorSeverity;
    UncorrErrMask.Field.POIS_TLP_ERR_MASK = PortAerConfig->UncorrectableSeverity.Field.PoisonedTLPSeverity;
    UncorrErrMask.Field.CMPLT_TIMEOUT_ERR_MASK = PortAerConfig->UncorrectableSeverity.Field.CompletionTimeoutSeverity;
    UncorrErrMask.Field.CMPLT_ABORT_ERR_MASK = PortAerConfig->UncorrectableSeverity.Field.CompleterAbortSeverity;
    UncorrErrMask.Field.UNEXP_CMPLT_ERR_MASK = PortAerConfig->UncorrectableSeverity.Field.UnexpectedCompletionSeverity;
    UncorrErrMask.Field.MALF_TLP_ERR_MASK = PortAerConfig->UncorrectableSeverity.Field.MalTlpSeverity;
    UncorrErrMask.Field.ECRC_ERR_MASK = PortAerConfig->UncorrectableSeverity.Field.ECRCErrorSeverity;
    UncorrErrMask.Field.UNSUPPORTED_REQ_ERR_MASK = PortAerConfig->UncorrectableSeverity.Field.UnsupportedRequestErrorSeverity;
    UncorrErrMask.Field.ACS_VIOLATION_MASK = PortAerConfig->UncorrectableSeverity.Field.AcsViolationSeverity;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_UNCORR_ERR_MASK_OFF), &UncorrErrMask.Value, GNB_REG_ACC_FLAG_S3SAVE);
    
    //Set Unorrectable Error Severity
    NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_UNCORR_ERR_SEV_OFF), &UncorrErrSeverity.Value, 0);
    UncorrErrSeverity.Field.DL_PROTOCOL_ERR_SEVERITY = PortAerConfig->UncorrectableSeverity.Field.DataLinkProtocolErrorSeverity;
    UncorrErrSeverity.Field.POIS_TLP_ERR_SEVERITY = PortAerConfig->UncorrectableSeverity.Field.PoisonedTLPSeverity;
    UncorrErrSeverity.Field.CMPLT_TIMEOUT_ERR_SEVERITY = PortAerConfig->UncorrectableSeverity.Field.CompletionTimeoutSeverity;
    UncorrErrSeverity.Field.CMPLT_ABORT_ERR_SEVERITY = PortAerConfig->UncorrectableSeverity.Field.CompleterAbortSeverity;
    UncorrErrSeverity.Field.UNEXP_CMPLT_ERR_SEVERITY = PortAerConfig->UncorrectableSeverity.Field.UnexpectedCompletionSeverity;
    UncorrErrSeverity.Field.MALF_TLP_ERR_SEVERITY = PortAerConfig->UncorrectableSeverity.Field.MalTlpSeverity;
    UncorrErrSeverity.Field.ECRC_ERR_SEVERITY = PortAerConfig->UncorrectableSeverity.Field.ECRCErrorSeverity;
    UncorrErrSeverity.Field.UNSUPPORTED_REQ_ERR_SEVERITY = PortAerConfig->UncorrectableSeverity.Field.UnsupportedRequestErrorSeverity;
    UncorrErrSeverity.Field.ACS_VIOLATION_SEVERITY = PortAerConfig->UncorrectableSeverity.Field.AcsViolationSeverity;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_UNCORR_ERR_SEV_OFF), &UncorrErrSeverity.Value, GNB_REG_ACC_FLAG_S3SAVE);
  }
  
  IDS_HDT_CONSOLE (GNB_TRACE, "CxlAerInit Exit\n");
}
 /**
  *---------------------------------------------------------------------------------------
  *  CxlMiscInit
  *
  *  Description:
  *     Cxl Misc Init
  *  Parameters:
  *    
  *
  *---------------------------------------------------------------------------------------
  **/
void
CxlMiscInit (
  void
  )
{
  EFI_STATUS                           Status;
  PCIe_PLATFORM_CONFIG                *Pcie;
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB       *PciePlatformConfigHobData;
  GNB_HANDLE                          *GnbHandle;

  IDS_HDT_CONSOLE (MAIN_FLOW, "%a Entry\n", __FUNCTION__);
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
      if (GnbHandle->CxlPresent) {
        CxlAerInit (GnbHandle);
        CxlMaxPayloadInterface (GnbHandle);
        CxlAspmInterface (GnbHandle);
      }
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "%a Exit\n", __FUNCTION__);
}

