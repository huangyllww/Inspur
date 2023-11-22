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
#include <Library/GnbPciAccLib.h>
#include <Library/GnbCxlLib.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/HygonCxl11ServicesProtocol.h>
#include <Protocol/HygonCxl20ServicesProtocol.h>
#include <IndustryStandard/Cxl.h>
#include <IndustryStandard/Cxl20.h>            // byo230928 +

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE        NBIO_CXL_HYGONCXLDXE_HYGONCXLDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

CXL_INFO_HOB_DATA       *mCxl11Info = NULL;

EFI_STATUS
EFIAPI
Cxl11GetRootPortInfoByIndex (
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,
  IN  UINTN                                PortIndex,
  OUT HYGON_CXL_PORT_INFO_STRUCT          *PortInformation
  );

EFI_STATUS
EFIAPI
Cxl11GetRootPortInfoByBus (
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,
  IN  UINT8                                EndPointBus,
  OUT HYGON_CXL_PORT_INFO_STRUCT          *PortInformation
  );

EFI_STATUS
EFIAPI
Cxl11ConfigureRootPort (
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,
  IN  PCI_ADDR                             EndpointBDF
  );

EFI_STATUS
EFIAPI
GetCxl11PortRBLocation (
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,
  IN  UINT8                                EndPointBus,
  OUT UINT8                               *SocketId,
  OUT UINT8                               *PhysicalDieId,
  OUT UINT8                               *LogicalDieId,
  OUT UINT8                               *RbIndex
  );

EFI_STATUS
EFIAPI
Cxl20GetRootPortInfo (
  IN  HYGON_NBIO_CXL20_SERVICES_PROTOCOL  *This,
  IN  PCI_ADDR                             PortBDF,
  OUT HYGON_CXL20_PORT_INFO_STRUCT        *PortInformation
  );

EFI_STATUS
EFIAPI
GetCxl20PortRBLocation (
  IN  HYGON_NBIO_CXL20_SERVICES_PROTOCOL  *This,
  IN  UINT8                                CxlRcBus,
  OUT UINT8                               *SocketId,
  OUT UINT8                               *LogicalDieId,
  OUT UINT8                               *RbIndex
  );

EFI_STATUS
EFIAPI
GetRcecMapInfoByIndex (
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,
  IN  UINT8                                RcecIndex,
  OUT HYGON_RCEC_MAP_INFO_STRUCT          *RcecMapInformation
  );
/*----------------------------------------------------------------------------------------
 *           P R O T O C O L   I N S T A N T I A T I O N
 *----------------------------------------------------------------------------------------
 */

HYGON_NBIO_CXL11_SERVICES_PROTOCOL mHygonNbioCxl11ServicesProtocol = {
  HYGON_NBIO_CXL11_SERVICES_REVISION,
  0,
  Cxl11GetRootPortInfoByIndex,
  Cxl11GetRootPortInfoByBus,
  Cxl11ConfigureRootPort,
  GetCxl11PortRBLocation,
  0,
  GetRcecMapInfoByIndex
};

HYGON_NBIO_CXL20_SERVICES_PROTOCOL mHygonNbioCxl20ServicesProtocol = {
  HYGON_NBIO_CXL20_SERVICES_REVISION,
  Cxl20GetRootPortInfo,
  GetCxl20PortRBLocation,
  CxlGetCdat,
  CxlParseCdat
};

/**
 *---------------------------------------------------------------------------------------
 *  DumpCxlHobInfo
 *
 *  Description:
 *     Print CXL Hob information
 *  Parameters:
 *    @param[in]     *CxlInfo      CXL HOB data pointer
 *
 *---------------------------------------------------------------------------------------
 **/
void
DumpCxlHobInfo (
  IN       CXL_INFO_HOB_DATA   *CxlInfo
  )
{
    UINT8  Index;
    
    IDS_HDT_CONSOLE (MAIN_FLOW, "----------------DumpCxlHobInfo Start----------------\n");
    
    for (Index = 0; Index < CxlInfo->Cxl11Count; Index++) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Cxl11 Index       : %d \n", CxlInfo->Cxl11Info[Index].Cxl11Index);
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Cxl11 DspRcrb     : 0x%X \n", CxlInfo->Cxl11Info[Index].DspRcrb);
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Cxl11 UspRcrb     : 0x%X \n", CxlInfo->Cxl11Info[Index].UspRcrb);
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Cxl11 DspMemBar0  : 0x%X \n", CxlInfo->Cxl11Info[Index].DspMemBar0);
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Cxl11 UspMemBar0  : 0x%X \n", CxlInfo->Cxl11Info[Index].UspMemBar0);
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Cxl11 PCI Address : 0x%X \n", CxlInfo->Cxl11Info[Index].PciAddr.AddressValue);
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Cxl11 Parent PCI Address : 0x%X \n", CxlInfo->Cxl11Info[Index].ParentPciAddr.AddressValue);
    }
    
    for (Index = 0; Index < CxlInfo->RcecCount; Index++) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "  RCEC Socket Id       : %d \n", CxlInfo->Rcec[Index].SocketId);
        IDS_HDT_CONSOLE (MAIN_FLOW, "  RCEC Logical Die Id  : %d \n", CxlInfo->Rcec[Index].LogicalDieId);
        IDS_HDT_CONSOLE (MAIN_FLOW, "  RCEC Physical Die Id : %d \n", CxlInfo->Rcec[Index].PhysicalDieId);
        IDS_HDT_CONSOLE (MAIN_FLOW, "  RCEC Nbio Index      : %d \n", CxlInfo->Rcec[Index].RbIndex);
        IDS_HDT_CONSOLE (MAIN_FLOW, "  RCEC Nbio Bus Base   : %d \n", CxlInfo->Rcec[Index].RbBusNumber);
        IDS_HDT_CONSOLE (MAIN_FLOW, "  RCEC Start RCiEP Bus : 0x%X \n", CxlInfo->Rcec[Index].StartRciepBus);
        IDS_HDT_CONSOLE (MAIN_FLOW, "  RCEC End RCiEP Bus   : 0x%X \n", CxlInfo->Rcec[Index].EndRciepBus);
    }
    
    IDS_HDT_CONSOLE (MAIN_FLOW, "----------------DumpCxlHobInfo End----------------\n");
}

/**
 *---------------------------------------------------------------------------------------
 *  FindCxl11PortIndexByBus
 *
 *  Description:
 *     Find CXL port index by bus
 *  Parameters:
 *    @param[in]     EndPointBus      CXL 1.1 endpoint bus
 *                  *PortIndex        CXL 1.1 port index in system
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS
EFIAPI
FindCxl11PortIndexByBus (
  IN   UINT8                 EndPointBus,
  OUT  UINT8                *PortIndex
  )
{
  UINT8                    Index;
  
  for (Index = 0; Index < mCxl11Info->Cxl11Count; Index++) {
    if (EndPointBus == mCxl11Info->Cxl11Info[Index].PciAddr.Address.Bus) {
      *PortIndex = Index;
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
}

/**
 *---------------------------------------------------------------------------------------
 *  CxlCallbackAfterPciEnum
 *
 *  Description:
 *     notification event handler after gEfiPciEnumerationCompleteProtocolGuid ready
 *  Parameters:
 *    @param[in]     Event      Event whose notification function is being invoked.
 *    @param[in]     *Context   Pointer to the notification function's context.
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
EFIAPI
CxlCallbackAfterPciEnum (
   IN      EFI_EVENT  Event,
   IN      VOID      *Context
   )
{
  IDS_HDT_CONSOLE (MAIN_FLOW, "CxlCallbackAfterPciEnum Entry\n");
   
  CxlUpdateHpcb ();
  CxlMiscInit ();
   
  IDS_HDT_CONSOLE (MAIN_FLOW, "CxlCallbackAfterPciEnum Exit\n");
  gBS->CloseEvent (Event);
}

/**
 *---------------------------------------------------------------------------------------
 *  CxlReadyToBoot
 *
 *  Description:
 *     notification event handler when on ReadyToBoot
 *  Parameters:
 *    @param[in]     Event      Event whose notification function is being invoked.
 *    @param[in]     *Context   Pointer to the notification function's context.
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
EFIAPI
CxlReadyToBoot (
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  )
{
  IDS_HDT_CONSOLE (MAIN_FLOW, "CxlReadyToBoot Entry\n");
  
  CxlSetCedt ();
  
  if (PcdGetBool (PcdCxlMemValidation)) {
    CxlMemoryTest ();
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "CxlReadyToBoot Exit\n");
  gBS->CloseEvent (Event);
}
/**
 *
 *  HygonCxlDxeEntry
 *
 *  @param[in]  ImageHandle     EFI Image Handle for the DXE driver
 *  @param[in]  SystemTable     Pointer to the EFI system table
 *
 *  Description:
 *    CXL Dxe Driver Entry.
 *
 *  @retval EFI_STATUS
 *
 */
EFI_STATUS
EFIAPI
HygonCxlDxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS               Status;
  VOID                    *HobAddr;
  CXL_INFO_HOB_DATA       *Cxl11Info;
  VOID                    *Registration;
  EFI_EVENT                PciEnumEvent;
  EFI_EVENT                ReadyToBootEvent;
  EFI_HANDLE               Handle;
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonCxlDxe Entry\n");
  
  Handle = NULL;
  Status = EFI_SUCCESS;
  
  if (PcdGetBool (PcdCfgCxlEnable) == FALSE) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "No CXL, Exit\n");
    return Status;
  }
  
  //Publish CXL 1.1 service protocol
  HobAddr = GetFirstGuidHob (&gHygonCxlInfoHobGuid);
  if (HobAddr != NULL) {
    Cxl11Info = (CXL_INFO_HOB_DATA *) GET_GUID_HOB_DATA (HobAddr);
    //if (Cxl11Info->Cxl11Count > 0) {
      Status = gBS->AllocatePool (EfiRuntimeServicesData, sizeof (CXL_INFO_HOB_DATA), &mCxl11Info);
      if (EFI_ERROR (Status)) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Allocate pool for CXL 1.1 info fail (%r) \n", Status);
        return Status;
      }
      gBS->CopyMem (mCxl11Info, Cxl11Info, sizeof (CXL_INFO_HOB_DATA));
      DumpCxlHobInfo (mCxl11Info);
      
      mHygonNbioCxl11ServicesProtocol.Cxl11Count = mCxl11Info->Cxl11Count;
      mHygonNbioCxl11ServicesProtocol.RcecCount = mCxl11Info->RcecCount;
      gBS->InstallProtocolInterface (
        &Handle,
        &gHygonNbioCxl11ServicesProtocolGuid,
        EFI_NATIVE_INTERFACE,
        &mHygonNbioCxl11ServicesProtocol
      );
    //}
  }
  
  //Publish CXL 2.0 service protocol
  gBS->InstallProtocolInterface (
    &Handle,
    &gHygonNbioCxl20ServicesProtocolGuid,
    EFI_NATIVE_INTERFACE,
    &mHygonNbioCxl20ServicesProtocol
  );
  
  // Publish CXL Memory Manager services
  Status = FabricCxlManagerProtocolInstall (ImageHandle, SystemTable);
  
  //Create PCI IO protocol callback
  Status = gBS->CreateEventEx (
             EVT_NOTIFY_SIGNAL,
             TPL_NOTIFY,
             CxlCallbackAfterPciEnum,
             NULL,
             NULL,
             &PciEnumEvent
             );

  Status = gBS->RegisterProtocolNotify (
              &gEfiPciEnumerationCompleteProtocolGuid,
              PciEnumEvent,
              &Registration
              );
  
  //Create ready to boot callback
  Status = EfiCreateEventReadyToBootEx (TPL_CALLBACK, CxlReadyToBoot, NULL, &ReadyToBootEvent);
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonCxlDxe Exit\n");
  
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
  This function build CXL 1.1 Port Information by CXL 1.1 HOB data

  PortInformation
    A pointer to an information structure to be populated by this function to
    identify the location of the CXL port.
  Cxl11Info
    A pointer to CXL 1.1 device information
**/
void
BuildCxl11PortInfo (
 IN  HYGON_CXL_PORT_INFO_STRUCT    *PortInformation,
 IN  CXL11_INFO                    *Cxl11Info
 )
{
  PortInformation->EndPointBDF = Cxl11Info->PciAddr;
  PortInformation->SocketId = Cxl11Info->SocketId;
  PortInformation->LogicalDieId = Cxl11Info->LogicalDieId;
  PortInformation->PhysicalDieId = Cxl11Info->PhysicalDieId;
  PortInformation->RbIndex = Cxl11Info->RbIndex;
  PortInformation->DspRcrb = Cxl11Info->DspRcrb;
  PortInformation->UspRcrb = Cxl11Info->UspRcrb;
  PortInformation->DspMemBar0 = Cxl11Info->DspMemBar0;
  PortInformation->UspMemBar0 = Cxl11Info->UspMemBar0;
  PortInformation->ParentPciAddr = Cxl11Info->ParentPciAddr;
  PortInformation->PortWidth = Cxl11Info->PortWidth; 
  
  PortInformation->PhysicalPortId = 8;
}

/*----------------------------------------------------------------------------------------*/
/**
  This function gets information about a specific CXL root port.

  This
    A pointer to the HYGON_NBIO_CXL11_SERVICES_PROTOCOL instance.
  PortIndex
    Cxl 1.1 port index in system.
  PortInformation
    A pointer to an information structure to be populated by this function to
    identify the location of the CXL port.
**/

EFI_STATUS
EFIAPI
Cxl11GetRootPortInfoByIndex (
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,
  IN  UINTN                                PortIndex,
  OUT HYGON_CXL_PORT_INFO_STRUCT          *PortInformation
  )
{
  if (PortIndex >= mCxl11Info->Cxl11Count) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Invalid CXL 1.1 port index %d \n", PortIndex);
    return EFI_INVALID_PARAMETER;
  }
  
  if (mCxl11Info->Cxl11Info[PortIndex].InitState != CXL_CONFIGURED) {
    return EFI_DEVICE_ERROR;
  }

  if (PortInformation != NULL) {
    BuildCxl11PortInfo (PortInformation, &mCxl11Info->Cxl11Info[PortIndex]);
  }
  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
  This function gets information about a specific CXL root port.

  This
    A pointer to the HYGON_NBIO_CXL11_SERVICES_PROTOCOL instance.
  EndPointBus
    Cxl 1.1 endpoint PCI bus number.
  PortInformation
    A pointer to an information structure to be populated by this function to
    identify the location of the CXL port.
**/
EFI_STATUS
EFIAPI
Cxl11GetRootPortInfoByBus (
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,
  IN  UINT8                                EndPointBus,
  OUT HYGON_CXL_PORT_INFO_STRUCT          *PortInformation
  )
{
  EFI_STATUS               Status;
  UINT8                    PortIndex;
  
  Status = FindCxl11PortIndexByBus (EndPointBus, &PortIndex);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Not found CXL 1.1 (Bus 0x%X) info \n", EndPointBus);
    return Status;
  }
  
  if (mCxl11Info->Cxl11Info[PortIndex].InitState != CXL_CONFIGURED) {
    return EFI_DEVICE_ERROR;
  }

  if (PortInformation != NULL) {
    BuildCxl11PortInfo (PortInformation, &mCxl11Info->Cxl11Info[PortIndex]);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetCxl11PortRBLocation (
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,
  IN  UINT8                                EndPointBus,
  OUT UINT8                               *SocketId,
  OUT UINT8                               *PhysicalDieId,
  OUT UINT8                               *LogicalDieId,
  OUT UINT8                               *RbIndex
  )
{
  EFI_STATUS               Status;
  UINT8                    PortIndex;
  
  Status = FindCxl11PortIndexByBus (EndPointBus, &PortIndex);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Not found CXL 1.1 (Bus 0x%X) location \n", EndPointBus);
    return Status;
  }
  
  if (SocketId != NULL)
    *SocketId = mCxl11Info->Cxl11Info[PortIndex].SocketId;
  
  if (PhysicalDieId != NULL)
    *PhysicalDieId = mCxl11Info->Cxl11Info[PortIndex].PhysicalDieId;
  
  if (LogicalDieId != NULL)
    *LogicalDieId = mCxl11Info->Cxl11Info[PortIndex].LogicalDieId;
  
  if (RbIndex != NULL)
    *RbIndex = mCxl11Info->Cxl11Info[PortIndex].RbIndex;
  
  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
  This function configures a CXL1.1 port

  This
    A pointer to the HYGON_NBIO_CXL11_SERVICES_PROTOCOL instance.
  EndpointBDF
    Bus/Device/Function of Endpoint in PCI_ADDR format.
**/
EFI_STATUS
EFIAPI
Cxl11ConfigureRootPort (
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,
  IN  PCI_ADDR                             EndpointBDF
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
GetRcecMapInfoByIndex (
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,
  IN  UINT8                                RcecIndex,
  OUT HYGON_RCEC_MAP_INFO_STRUCT          *RcecMapInformation
  )
{
  EFI_STATUS               Status;
  UINT8                    PortIndex;
  UINT8                    RciepBus;
  UINT8                    i;
  
  if (RcecIndex >= mCxl11Info->RcecCount) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Invalid RCEC index %d \n", RcecIndex);
    return EFI_INVALID_PARAMETER;
  }
  
  RcecMapInformation->RcecId = RcecIndex;
  RcecMapInformation->SocketId = mCxl11Info->Rcec[RcecIndex].SocketId;
  RcecMapInformation->LogicalDieId = mCxl11Info->Rcec[RcecIndex].LogicalDieId;
  RcecMapInformation->PhysicalDieId = mCxl11Info->Rcec[RcecIndex].PhysicalDieId;
  RcecMapInformation->RbIndex = mCxl11Info->Rcec[RcecIndex].RbIndex;
  RcecMapInformation->RbBusNumber = mCxl11Info->Rcec[RcecIndex].RbBusNumber;
  RcecMapInformation->StartRciepBus = mCxl11Info->Rcec[RcecIndex].StartRciepBus;
  RcecMapInformation->EndRciepBus = mCxl11Info->Rcec[RcecIndex].EndRciepBus;
  
  i = 0;
  RciepBus = RcecMapInformation->StartRciepBus;
  do {
    Status = FindCxl11PortIndexByBus (RciepBus, &PortIndex);
    if (EFI_ERROR (Status)) {
      break;
    }
    RcecMapInformation->DspRcrb[i] = mCxl11Info->Cxl11Info[PortIndex].DspRcrb;
    RcecMapInformation->DspCount++;
    RciepBus++;
  } while (RciepBus <= RcecMapInformation->EndRciepBus);
  
  return Status;
}

EFI_STATUS
EFIAPI
Cxl20GetRootPortInfo (
  IN  HYGON_NBIO_CXL20_SERVICES_PROTOCOL  *This,
  IN  PCI_ADDR                             PortBDF,
  OUT HYGON_CXL20_PORT_INFO_STRUCT        *PortInformation
  )
{
  CXL_REGISTER_BLOCK_INFO            CxlBlockInfo;
  UINT32                             VidDid;
  
  GnbLibPciRead (PortBDF.AddressValue, AccessWidth32, &VidDid, NULL);
  if (VidDid == 0xFFFFFFFF) {
    //CXL 2.0 Root port not present
    return EFI_DEVICE_ERROR;
  }
  
  PortInformation->PortBDF = PortBDF;
  
  FindCxl20RegBlock (PortBDF, COMPONENT_REG_BLOCK, &CxlBlockInfo);
  PortInformation->ComponentRegBaseAddr = CxlBlockInfo.BarAddress;
  IDS_HDT_CONSOLE (MAIN_FLOW, "Get CXL 2.0 port (0x%X) component register base 0x%lX \n", PortBDF.AddressValue, PortInformation->ComponentRegBaseAddr);
 
  if (PortInformation->ComponentRegBaseAddr != CXL_INVALID_DATA) {
    DumpCxlCmComponentRegisters (PortInformation->ComponentRegBaseAddr + CXL_CM_OFFSET);
    return EFI_SUCCESS;
  }
  
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetCxl20PortRBLocation (
  IN  HYGON_NBIO_CXL20_SERVICES_PROTOCOL  *This,
  IN  UINT8                                CxlRcBus,
  OUT UINT8                               *SocketId,
  OUT UINT8                               *LogicalDieId,
  OUT UINT8                               *RbIndex
  )
{
  EFI_STATUS                              Status;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;
  UINTN                                   NumberOfSockets;
  UINTN                                   SocketLoop;
  UINTN                                   NumberOfPhysicalDies;
  UINTN                                   NumberOfLogicalDies;
  UINTN                                   DieLoop;
  UINTN                                   NumberOfRootBridges;
  UINTN                                   RbLoop;
  UINTN                                   BusNumberBase;
  UINTN                                   BusNumberLimit;
  
  Status = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **) &FabricTopology);
  Status = FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL);
  
  for (SocketLoop = 0; SocketLoop < NumberOfSockets; SocketLoop++) { //Loop sockets
    Status = FabricTopology->GetProcessorInfo (FabricTopology, SocketLoop, &NumberOfPhysicalDies, &NumberOfLogicalDies, NULL);
    if(EFI_ERROR (Status)) return Status;
    
    for (DieLoop = 0; DieLoop < NumberOfLogicalDies; DieLoop++) { //Loop dies
      FabricTopology->GetDieInfo(FabricTopology, SocketLoop, DieLoop, &NumberOfRootBridges, NULL, NULL);
      if(EFI_ERROR (Status)) {
        return Status;
      }
      for(RbLoop = 0; RbLoop < NumberOfRootBridges; RbLoop++){
        FabricTopology->GetRootBridgeInfo(
                        FabricTopology,
                        SocketLoop,
                        DieLoop,
                        RbLoop,
                        NULL,
                        &BusNumberBase,
                        &BusNumberLimit
                        );
        if ((CxlRcBus >= (UINT8)BusNumberBase) && (CxlRcBus <= (UINT8)BusNumberLimit)) {
          *SocketId = (UINT8) SocketLoop;
          *LogicalDieId = (UINT8) DieLoop;
          *RbIndex = (UINT8) RbLoop;
          return EFI_SUCCESS;
        }
      }
    }
  }
  return EFI_NOT_FOUND;
}
