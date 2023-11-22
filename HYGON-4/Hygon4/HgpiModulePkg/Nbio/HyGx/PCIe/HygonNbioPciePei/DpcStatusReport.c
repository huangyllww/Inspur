/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe DPC Status PPI.
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
#include <PiPei.h>
#include <Filecode.h>
#include <GnbHsio.h>
#include <Ppi/NbioPcieDpcStatusPpi.h>

#define FILECODE  NBIO_PCIE_HYGX_HYGONNBIOPCIESTPEI_DPCSTATUSREPORT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
typedef struct {
  PEI_HYGON_NBIO_PCIE_DPCSTATUS_PPI    Ppi;
  EFI_PEI_PPI_DESCRIPTOR               PpiList;
  PCIe_DPC_STATUS_DATA                 DpcStatusData;
} PEI_DPCSTATUS_INTERNAL_DATA;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
PcieGetDpcStatusData (
  IN  PEI_HYGON_NBIO_PCIE_DPCSTATUS_PPI  *This,
  OUT PCIe_DPC_STATUS_DATA             **DpcStatus
  );

/*----------------------------------------------------------------------------------------
 *                     L O C A L   D A T A   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */
static PEI_HYGON_NBIO_PCIE_DPCSTATUS_PPI  PcieDpcStatusPpi = {
  PcieGetDpcStatusData
};

/*----------------------------------------------------------------------------------------*/

/**
 * Returns the DPC Status data structure
 *
 *
 * @param[in]  This         A pointer to the PEI_HYGON_NBIO_PCIE_DPCSTATUS_PPI instance.
 * @param[out] DpcStatus    A pointer to a pointer to store the address of DPC Status data array
 */
EFI_STATUS
PcieGetDpcStatusData (
  IN  PEI_HYGON_NBIO_PCIE_DPCSTATUS_PPI  *This,
  OUT PCIe_DPC_STATUS_DATA             **DpcStatus
  )
{
  PEI_DPCSTATUS_INTERNAL_DATA  *DpcThis;

  DpcThis    = (PEI_DPCSTATUS_INTERNAL_DATA *)This;
  *DpcStatus = (PCIe_DPC_STATUS_DATA *)&DpcThis->DpcStatusData;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieGetDpcStatusData(0x%x)\n", *DpcStatus);
  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Install DPC Status report PPI
 *
 *
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 * @retval     EFI_STATUS
 */
EFI_STATUS
PcieDpcStatusServiceInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                   Status;
  PEI_DPCSTATUS_INTERNAL_DATA  *pDpcStatus;

  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (PEI_DPCSTATUS_INTERNAL_DATA),
                             (VOID **)&pDpcStatus
                             );
  if (Status == EFI_SUCCESS) {
    (*PeiServices)->SetMem (
                      pDpcStatus,
                      sizeof (PEI_DPCSTATUS_INTERNAL_DATA),
                      0
                      );
    (*PeiServices)->CopyMem (
                      pDpcStatus,
                      &PcieDpcStatusPpi,
                      sizeof (PEI_HYGON_NBIO_PCIE_DPCSTATUS_PPI)
                      );
    pDpcStatus->PpiList.Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
    pDpcStatus->PpiList.Guid  = &gHygonNbioPcieDpcStatusPpiGuid;
    pDpcStatus->PpiList.Ppi   = pDpcStatus;
    Status = (*PeiServices)->InstallPpi (PeiServices, &pDpcStatus->PpiList);
  }

  return Status;
}

