/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM PCIE Initialization
 *
 * Contains code that initialized PCIE before memory init.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  PcieInit
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

#include <HygonCpmPei.h>
#include <Ppi/NbioPcieComplexPpi.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <HygonBoardId.h>
#include <TSFch.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/HygonSocBaseLib.h>

#define HsioInputParserGetNextDescriptor(Descriptor)  (Descriptor != NULL ? ((((Descriptor->Flags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : (++Descriptor))) : NULL)

typedef struct _HYGON_CPM_HSIO_TABLE {
  UINT32                           TableId;
  HYGON_CPM_HSIO_TOPOLOGY_TABLE    *HsioTopologyTablePtr;
} HYGON_CPM_HSIO_TABLE;

EFI_STATUS
EFIAPI
HygonCpmPcieInitPeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mCpmPcieInitPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonCpmGpioInitFinishedPpiGuid,
  HygonCpmPcieInitPeim
};

EFI_STATUS
HygonCpmHsioGetComplex (
  IN       PEI_HYGON_NBIO_PCIE_COMPLEX_PPI *This,
  OUT      HSIO_COMPLEX_DESCRIPTOR       **UserConfig
  )
{
 #if 1 // HSIO Topology hardcode
    HSIO_COMPLEX_DESCRIPTOR  *HsioComplex;
    EFI_STATUS               Status;
    EFI_PEI_SERVICES         **PeiServices;
    HYGON_CPM_TABLE_PPI      *CpmTablePpiPtr;

    PeiServices = (EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();

    Status = (*PeiServices)->LocatePpi (
                               (CPM_PEI_SERVICES **)PeiServices,
                               &gHygonCpmTablePpiGuid,
                               0,
                               NULL,
                               (VOID **)&CpmTablePpiPtr
                               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    HsioComplex = CpmTablePpiPtr->PeimPublicFunction.HsioComplexDescriptorPtr;

    *UserConfig = HsioComplex;
 #else
    *UserConfig = PcieComplex;
 #endif

  return EFI_SUCCESS;
}

STATIC PEI_HYGON_NBIO_PCIE_COMPLEX_PPI  mHsioComplexPpi = {
  HYGON_NBIO_PCIE_COMPLEX_REVISION,  ///< revision
  HygonCpmHsioGetComplex
};

/*----------------------------------------------------------------------------------------*/

/**
 * The Function to reset PCIe device.
 *
 * @param[in]     This          Pointer to HYGON CPM Table Ppi
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
  HYGON_CPM_TABLE_PPI  *CpmTablePpiPtr;

  CpmTablePpiPtr = This;
  CpmTablePpiPtr->CommonFunction.ResetDevice (CpmTablePpiPtr, ResetId, ResetControl);

  return;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Generate Pcie Complex Descriptor table for HGPI and set the trigger level of
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
HygonCpmPcieInitPeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  HYGON_CPM_TABLE_PPI                 *CpmTablePpiPtr;
  HYGON_CPM_MAIN_TABLE                *MainTablePtr;
  HSIO_COMPLEX_DESCRIPTOR             *HsioComplexDescriptorPtr;
  EFI_PEI_PPI_DESCRIPTOR              *PpiListNbioPcieComplexPtr;
  EFI_STATUS                          Status;
  UINT16                              SocketIndex;
  UINT16                              SocketPresent;
  UINT16                              SocketConfiged;
  UINT32                              CpuModel;
  HYGON_CPM_HSIO_TABLE                *HsioTopologyTablePtrs;
  HYGON_CPM_HSIO_TABLE                HsioTopologyTablePtrsHyEx[] = {
    { HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY,    NULL },
    { HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S1, NULL },
    { HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S2, NULL },
    { HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S3, NULL },
    { HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S4, NULL },
    { HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S5, NULL },
    { HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S6, NULL },
    { HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S7, NULL },
    { 0xFFFFFFFF,                         NULL },
  };

  HYGON_CPM_HSIO_TABLE                HsioTopologyTablePtrsHyGx[] = {
    { HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY,    NULL },
    { HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S1, NULL },
    { HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S2, NULL },
    { HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S3, NULL },
    { HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S4, NULL },
    { HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S5, NULL },
    { HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S6, NULL },
    { HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S7, NULL },
    { 0xFFFFFFFF,                         NULL },
  };

  Status = (*PeiServices)->LocatePpi (
                             (CPM_PEI_SERVICES **)PeiServices,
                             &gHygonCpmTablePpiGuid,
                             0,
                             NULL,
                             (VOID **)&CpmTablePpiPtr
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpPcieInitPeimDriverBegin);

  MainTablePtr = CpmTablePpiPtr->MainTablePtr;
  
  CpuModel              = GetHygonSocModel();
  HsioTopologyTablePtrs = NULL;
  
  if (CpuModel == HYGON_EX_CPU) {
    HsioTopologyTablePtrs = &HsioTopologyTablePtrsHyEx[0];
  } else if (CpuModel == HYGON_GX_CPU) {
    HsioTopologyTablePtrs = &HsioTopologyTablePtrsHyGx[0];
  }

  ASSERT (HsioTopologyTablePtrs != NULL);

  SocketPresent = (UINT16)FabricTopologyGetNumberOfSocketPresent ();
  for (SocketIndex = 0; SocketIndex < SocketPresent; SocketIndex++) {
    HsioTopologyTablePtrs[SocketIndex].HsioTopologyTablePtr = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, HsioTopologyTablePtrs[SocketIndex].TableId);
    if (HsioTopologyTablePtrs[SocketIndex].HsioTopologyTablePtr == NULL) {
      break;
    }
  }

  HsioTopologyTablePtrs[SocketIndex].TableId = 0xFFFFFFFF;
  HsioTopologyTablePtrs[SocketIndex].HsioTopologyTablePtr = NULL;

  SocketConfiged = SocketIndex;
  DEBUG ((DEBUG_INFO, "Total Present Socket=%d, CPM HSIO configed socket=%d\n", SocketPresent, SocketConfiged));

  CpmTablePpiPtr->PeimPublicFunction.PcieReset = CpmPcieReset;

  // For Multi-Socket Topology
  // Need create variable to indicate platform max socket support instead of use magic number here

  Status = (**PeiServices).AllocatePool (
                             (CPM_PEI_SERVICES **)PeiServices,
                             (sizeof (HSIO_COMPLEX_DESCRIPTOR) * SocketConfiged),
                             (VOID **)&HsioComplexDescriptorPtr
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  (**PeiServices).SetMem (
                    HsioComplexDescriptorPtr,
                    (sizeof (HSIO_COMPLEX_DESCRIPTOR) * SocketConfiged),
                    0
                    );

  for (SocketIndex = 0; SocketIndex < SocketConfiged; SocketIndex++) {
    HsioComplexDescriptorPtr[SocketIndex].Flags        = 0;
    HsioComplexDescriptorPtr[SocketIndex].SocketId     = SocketIndex;
    HsioComplexDescriptorPtr[SocketIndex].PciePortList = &(HsioTopologyTablePtrs[SocketIndex].HsioTopologyTablePtr->Port[0]);
  }

  HsioComplexDescriptorPtr[SocketIndex - 1].Flags = DESCRIPTOR_TERMINATE_LIST;
  CpmTablePpiPtr->PeimPublicFunction.HsioComplexDescriptorPtr = HsioComplexDescriptorPtr;

  Status = (*PeiServices)->AllocatePool (
                             (CPM_PEI_SERVICES **)PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             (VOID **)&PpiListNbioPcieComplexPtr
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PpiListNbioPcieComplexPtr->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListNbioPcieComplexPtr->Guid  = &gHygonNbioPcieComplexPpiGuid;
  PpiListNbioPcieComplexPtr->Ppi   = (VOID *)&mHsioComplexPpi;

  Status = (*PeiServices)->InstallPpi (
                             (CPM_PEI_SERVICES **)PeiServices,
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
 * Entry point of the HYGON CPM PCIE Init PEIM driver
 *
 * This function registers the function to update PCIe topology table according
 * to HYGON CPM PCIE TOPOLOGY OVERRIDE table and set GEVENT trigger level for Express
 * Card if HYGON CPM Express Card Table exists.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
HygonCpmPcieInitPeimEntryPoint (
  IN        CPM_PEI_FILE_HANDLE         FileHandle,
  IN        CPM_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS  Status;

  // EFI_PEI_PPI_DESCRIPTOR              *PpiListNbioPcieComplexPtr;

  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmPcieInitPeimNotify);

  return EFI_SUCCESS;
}
