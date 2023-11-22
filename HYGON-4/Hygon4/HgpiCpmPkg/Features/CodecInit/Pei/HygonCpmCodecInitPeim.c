/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM GPIO Initialization
 *
 * Contains code that initialized GPIO Init before memory init.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  GpioInit
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
#include <Library/DebugLib.h>

EFI_STATUS
EFIAPI
HygonCpmCodecInitPeimCallback (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mCpmPcieInitPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonCpmTablePpiGuid,
  HygonCpmCodecInitPeimCallback
};


/*----------------------------------------------------------------------------------------*/

/**
 * CPM Codec Verbtable Init
 *
 * This function registers the public functions which will be used outside of CPM,
 * initializes Verbtable struct for codec.
 *
 * @param[in]     PeiServices       Pointer to Pei Services
 * @param[in]     NotifyDescriptor  The descriptor for the notification event
 * @param[in]     Ppi               Pointer to the PPI in question.
 *
 * @retval        EFI_SUCCESS       Module initialized successfully
 * @retval        EFI_ERROR         Initialization failed (see error for more details)
 *
 */
EFI_STATUS
EFIAPI
HygonCpmCodecInitPeimCallback (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  EFI_STATUS                        Status;
  HYGON_CPM_TABLE_PPI               *CpmTablePpiPtr;
  HYGON_CPM_CODEC_VERBTABLE_TABLE   *TablePtr;

  DEBUG ((DEBUG_INFO, "HygonCpmCodecInitPeimCallback start\n"));
  
  PcdSet32S (PcdCfgAzaliaCodecVerbTable, 0);
  
  Status = (*PeiServices)->LocatePpi (
                             (CPM_PEI_SERVICES **)PeiServices,
                             &gHygonCpmTablePpiGuid,
                             0,
                             NULL,
                             (VOID **)&CpmTablePpiPtr
                             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Locate CpmTablePpi %r\n", Status));
    return Status;
  }
  
  TablePtr = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, HYGX_CPM_SIGNATURE_CODEC_VERBTABLE);
  if (TablePtr == NULL) {
    DEBUG ((DEBUG_INFO, "Get CpmPtr verbtable error\n"));
    return EFI_NOT_FOUND;
  }

  PcdSet32S (PcdCfgAzaliaCodecVerbTable, (UINT32)TablePtr->Verbtable);

  DEBUG ((DEBUG_INFO, "HygonCpmCodecInitPeimCallback end(%X-%X)\n", (UINT32)TablePtr->Verbtable, TablePtr->Verbtable[0]));
  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Entry point of the HYGON CPM GPIO Init PEIM driver
 *
 * This function registers the functions to initialize GPIO pins, GEVENT pins, PCIe
 * reference clock and on-board devices.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
HygonCpmCodecInitPeimEntryPoint (
  IN        CPM_PEI_FILE_HANDLE   FileHandle,
  IN        CPM_PEI_SERVICES      **PeiServices
  )
{
  EFI_STATUS  Status;

  // EFI_PEI_PPI_DESCRIPTOR              *PpiListNbioPcieComplexPtr;

  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmPcieInitPeimNotify);

  return EFI_SUCCESS;
}

