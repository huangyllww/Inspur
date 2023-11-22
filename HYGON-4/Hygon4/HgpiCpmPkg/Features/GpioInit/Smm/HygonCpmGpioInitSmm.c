/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM GPIO Initialization
 *
 * Contains CPM code to perform GPIO Init in SMM
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

#include <HygonCpmSmm.h>
#include <Library/HygonSocBaseLib.h>

CPM_DEFINE_gBS;
CPM_DEFINE_gSmst;

HYGON_CPM_TABLE_PROTOCOL  *gCpmTableSmmProtocol;

EFI_STATUS
EFIAPI
CPM_DEFINE_CALLBACK (
  IN      CPM_CALLBACK_NAME     HygonCpmGpioInitS3RestoreCallback,
  IN      CPM_CALLBACK_VAR_TYPE EFI_HANDLE,
  IN      CPM_CALLBACK_VAR_NAME DispatchHandle,
  IN      CPM_CALLBACK_VAR_TYPE CPM_SMM_SW_DISPATCH_CONTEXT,
  IN      CPM_CALLBACK_VAR_NAME *DispatchContext
  );

/*----------------------------------------------------------------------------------------*/

/**
 * Entry point of the HYGON CPM GPIO Init SMM driver
 *
 * This function registers the SMI handlers to set internal PCIe clock when resume
 * from S3.
 *
 * @param[in]     ImageHandle    Pointer to the firmware file system header
 * @param[in]     SystemTable    Pointer to System table
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
HygonCpmGpioInitSmmEntryPoint (
  IN      EFI_HANDLE                  ImageHandle,
  IN      EFI_SYSTEM_TABLE            *SystemTable
  )
{
  EFI_STATUS                    Status;
  CPM_SMM_SW_DISPATCH_PROTOCOL  *SwDispatch;
  CPM_SMM_SW_DISPATCH_CONTEXT   SwContext;
  EFI_HANDLE                    SwHandle;

  If_CpmInSmm (ImageHandle, SystemTable, gBS, gSmst, Status) {
    //
    // Initialize global variables
    //
    Status = gBS->LocateProtocol (
                    &gHygonCpmTableSmmProtocolGuid,
                    NULL,
                    (VOID **)&gCpmTableSmmProtocol
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    gCpmTableSmmProtocol->CommonFunction.PostCode (CpmTpGpioInitSmmDriverBegin);
    //
    // Locate SMM SW dispatch protocol
    //
    Status = CpmSmmLocateProtocol (
               &gCpmSmmSwDispatchProtocolGuid,
               NULL,
               (VOID **)&SwDispatch
               );
    ASSERT (!EFI_ERROR (Status));

    SwContext.SwSmiInputValue = CFG_CPM_ACPI_RESTORE_SW_SMI;
    Status = SwDispatch->Register (
                           SwDispatch,
                           HygonCpmGpioInitS3RestoreCallback,
                           &SwContext,
                           &SwHandle
                           );

    gCpmTableSmmProtocol->CommonFunction.PostCode (CpmTpGpioInitSmmDriverEnd);
  }
  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * PCIE Clock Init
 *
 * This function sets internal PCIe clock.
 *
 * @param[in]     CpmTableProtocolPtr Pointer to HYGON CPM Table Protocol
 *
 */
VOID
HygonCpmSmmPcieClockInit (
  IN      HYGON_CPM_TABLE_PROTOCOL      *CpmTableProtocolPtr
  )
{
  HYGON_CPM_PCIE_CLOCK_TABLE          *CpmPcieClockTablePtr;
  BOOLEAN                             ClockType;
  HYGON_CPM_EXPRESS_CARD_TABLE        *ExpressCardTablePtr;
  HYGON_CPM_OTHER_HOTPLUG_CARD_TABLE  *OtherHotplugTablePtr;
  UINT32                              CpuModel;
  
  CpmPcieClockTablePtr = NULL;
  ExpressCardTablePtr  = NULL;
  OtherHotplugTablePtr = NULL;
  CpuModel             = GetHygonSocModel();
  
  if (CpuModel == HYGON_EX_CPU) {
    CpmPcieClockTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYEX_CPM_SIGNATURE_PCIE_CLOCK);
    ExpressCardTablePtr  = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYEX_CPM_SIGNATURE_PCIE_EXPRESS_CARD);
    OtherHotplugTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYEX_CPM_SIGNATURE_PCIE_OTHER_HOTPLUG_CARD);
  } else if (CpuModel == HYGON_GX_CPU) {
    CpmPcieClockTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYGX_CPM_SIGNATURE_PCIE_CLOCK);
    ExpressCardTablePtr  = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYGX_CPM_SIGNATURE_PCIE_EXPRESS_CARD);
    OtherHotplugTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYGX_CPM_SIGNATURE_PCIE_OTHER_HOTPLUG_CARD);
  }
  ClockType = (BOOLEAN)CpmTableProtocolPtr->CommonFunction.GetStrap (CpmTableProtocolPtr) & BIT1;

  if (ExpressCardTablePtr) {
    CpmTableProtocolPtr->CommonFunction.SetGeventSciTrig (
                                          CpmTableProtocolPtr,
                                          ExpressCardTablePtr->EventPin,
                                          CpmTableProtocolPtr->CommonFunction.GetGevent (CpmTableProtocolPtr, ExpressCardTablePtr->EventPin) ? 0 : 1
                                          );
    CpmTableProtocolPtr->CommonFunction.SetGeventSci (
                                          CpmTableProtocolPtr,
                                          ExpressCardTablePtr->EventPin
                                          );
  }

  if (OtherHotplugTablePtr) {
    if (OtherHotplugTablePtr->Number > 0) {
      CpmTableProtocolPtr->CommonFunction.SetGeventSciTrig (
                                            CpmTableProtocolPtr,
                                            OtherHotplugTablePtr->EventPin0,
                                            CpmTableProtocolPtr->CommonFunction.GetGevent (CpmTableProtocolPtr, OtherHotplugTablePtr->EventPin0) ? 0 : 1
                                            );
      CpmTableProtocolPtr->CommonFunction.SetGeventSci (
                                            CpmTableProtocolPtr,
                                            OtherHotplugTablePtr->EventPin0
                                            );
    }

    if (OtherHotplugTablePtr->Number > 1) {
      CpmTableProtocolPtr->CommonFunction.SetGeventSciTrig (
                                            CpmTableProtocolPtr,
                                            OtherHotplugTablePtr->EventPin1,
                                            CpmTableProtocolPtr->CommonFunction.GetGevent (CpmTableProtocolPtr, OtherHotplugTablePtr->EventPin1) ? 0 : 1
                                            );
      CpmTableProtocolPtr->CommonFunction.SetGeventSci (
                                            CpmTableProtocolPtr,
                                            OtherHotplugTablePtr->EventPin1
                                            );
    }
  }

  return;
}

/*----------------------------------------------------------------------------------------*/

/**
 * The Function to init GEVENT pins
 *
 * @param[in]     CpmTableProtocolPtr Pointer to HYGON CPM Table Protocol
 *
 */
VOID
HygonCpmSmmGeventInit (
  IN      HYGON_CPM_TABLE_PROTOCOL      *CpmTableProtocolPtr
  )
{
  HYGON_CPM_GEVENT_INIT_TABLE  *GeventTablePtr;
  HYGON_CPM_GEVENT_ITEM        *TablePtr;
  UINT32                       CpuModel;

  if (CpmTableProtocolPtr) {
    GeventTablePtr = NULL;
    CpuModel       = GetHygonSocModel();
    
    if (CpuModel == HYGON_EX_CPU) {
      GeventTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYEX_CPM_SIGNATURE_GEVENT_INIT);
    } else if (CpuModel == HYGON_GX_CPU) {
      GeventTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYGX_CPM_SIGNATURE_GEVENT_INIT);
    }

    if (GeventTablePtr) {
      TablePtr = &GeventTablePtr->GeventList[0];
      while (TablePtr->Pin != 0xFF) {
        CpmTableProtocolPtr->CommonFunction.SetGevent (CpmTableProtocolPtr, TablePtr->Pin, TablePtr->Setting.Raw);
        TablePtr++;
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 *
 * SMI Handler to set internal PCIe clock
 *
 * This function restores internal PCIe clock when resume from S3.
 *
 */
EFI_STATUS
EFIAPI
CPM_DEFINE_CALLBACK (
  IN      CPM_CALLBACK_NAME     HygonCpmGpioInitS3RestoreCallback,
  IN      CPM_CALLBACK_VAR_TYPE EFI_HANDLE,
  IN      CPM_CALLBACK_VAR_NAME DispatchHandle,
  IN      CPM_CALLBACK_VAR_TYPE CPM_SMM_SW_DISPATCH_CONTEXT,
  IN      CPM_CALLBACK_VAR_NAME *DispatchContext
  )
{
  HygonCpmSmmPcieClockInit (gCpmTableSmmProtocol);
  HygonCpmSmmGeventInit (gCpmTableSmmProtocol);
  return EFI_SUCCESS;
}
