/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM GPIO Initialization
 *
 * Contains CPM code to perform GPIO Init under DXE
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  GpioInit
 * @e \$Revision$   @e \$Date$
 *
 */
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

#include <AmdCpmDxe.h>

VOID
EFIAPI
AmdCpmGpioInitMid (
  IN      VOID             *Context
  );

VOID
EFIAPI
AmdCpmGpioInitLate (
  IN      EFI_EVENT        Event,
  IN      VOID             *Context
  );

VOID
AmdCpmDxePcieClockInit (
  IN      AMD_CPM_TABLE_PROTOCOL  *CpmTableProtocolPtr
  );

VOID
AmdCpmDxeGeventInit (
  IN      AMD_CPM_TABLE_PROTOCOL  *CpmTableProtocolPtr
  );

VOID
AmdCpmGeventSmiControlInit (
  IN      AMD_CPM_TABLE_PROTOCOL  *CpmTableProtocolPtr
  );


/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM GPIO Init DXE driver
 *
 * This function sets GEVENT pins and internal PCIe clock.
 *
 * @param[in]     ImageHandle    Pointer to the firmware file system header
 * @param[in]     SystemTable    Pointer to System table
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */

EFI_STATUS
EFIAPI
AmdCpmGpioInitDxeEntryPoint (
  IN      EFI_HANDLE                      ImageHandle,
  IN      EFI_SYSTEM_TABLE                *SystemTable
  )
{
  EFI_STATUS                              Status;
  EFI_EVENT                               ReadyToBootEvent;
  AMD_CPM_TABLE_PROTOCOL                  *CpmTableProtocolPtr;

  //
  // Initialize Global Variable
  //
  CpmInitializeDriverLib (ImageHandle, SystemTable);

  Status = gBS->LocateProtocol (  &gAmdCpmTableProtocolGuid,
                                  NULL,
                                  (VOID**)&CpmTableProtocolPtr
                                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpmTableProtocolPtr->CommonFunction.PostCode (CpmTpGpioInitDxeDriverBegin);

  AmdCpmGpioInitMid (CpmTableProtocolPtr);
  AmdCpmGpioInitMid (CpmTableProtocolPtr);

  Status = gBS->CreateEventEx (
                  CPM_EVENT_NOTIFY_SIGNAL,
                  CPM_TPL_NOTIFY,
                  AmdCpmGpioInitLate,
                  (VOID*)CpmTableProtocolPtr,
                  &gEfiEventReadyToBootGuid,
                  &ReadyToBootEvent
                  );

  CpmTableProtocolPtr->CommonFunction.PostCode (CpmTpGpioInitDxeDriverEnd);

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init internal PCIe clock.
 *
 * This function gets called each time the EFI_EVENT_SIGNAL_READY_TO_BOOT gets signaled.
 *
 * @param[in]     Event          EFI_EVENT
 * @param[in]     Context        The Parameter Buffer
 *
 */

VOID
EFIAPI
AmdCpmGpioInitLate (
  IN      EFI_EVENT        Event,
  IN      VOID             *Context
  )
{
  STATIC BOOLEAN  AmdCpmGpioInitLateInvoked = FALSE;

  if (!AmdCpmGpioInitLateInvoked) {
    AmdCpmGeventSmiControlInit (Context);
    AmdCpmDxePcieClockInit (Context);
  }

  AmdCpmGpioInitLateInvoked = TRUE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init GEVENT pins
 *
 * This function gets called each time the EFI_EVENT_SIGNAL_READY_TO_BOOT gets signaled.
 *
 * @param[in]     Context        The Parameter Buffer
 *
 */

VOID
EFIAPI
AmdCpmGpioInitMid (
  IN      VOID             *Context
  )
{
  STATIC BOOLEAN  AmdCpmGpioInitMidInvoked = FALSE;

  if (!AmdCpmGpioInitMidInvoked) {
    AmdCpmDxeGeventInit (Context);
  }

  AmdCpmGpioInitMidInvoked = TRUE;
}

/*----------------------------------------------------------------------------------------*/
VOID
AmdCpmDxeAddOneItemInClkList (
  IN      AMD_CPM_EXT_CLKGEN_TABLE    *ExtClkGenTablePtr,
  IN      AMD_CPM_EXT_CLKGEN_ITEM     *ListPtr,
  IN      UINT8                       *ListNum,
  IN      UINT8                       ClkNum,
  IN      UINT8                       ClkReq
  )
{
  AMD_CPM_EXT_CLKGEN_ITEM     *CurrentItemPtr;
  UINT8   Index;
  UINT8   Index2;
  if (ClkNum == 0xFE) {
    return;
  }
  for (Index = 0; Index < AMD_EXT_CLKGEN_SIZE; Index++) {
    if (ExtClkGenTablePtr->Item[Index].Function == 0xFF) {
      break;
    }
    if (ExtClkGenTablePtr->Item[Index].Function == ClkNum + 0x80 && ClkReq == 0) {
      CurrentItemPtr = &ExtClkGenTablePtr->Item[Index];
    } else if (ExtClkGenTablePtr->Item[Index].Function == ClkReq + 0x8F && ClkReq != 0xFF && ClkReq != 0) {
      CurrentItemPtr = &ExtClkGenTablePtr->Item[Index];
    } else {
      continue;
    }
    for (Index2 = 0; Index2 < (*ListNum); Index2++) {
      if (ListPtr[Index2].Offset == CurrentItemPtr->Offset) {
        ListPtr[Index2].AndMask &= CurrentItemPtr->AndMask;
        ListPtr[Index2].OrMask |= CurrentItemPtr->OrMask;
        break;
      }
    }
    if (Index2 == (*ListNum)) {
      ListPtr[Index2].Function = 0;
      ListPtr[Index2].Offset = CurrentItemPtr->Offset;
      ListPtr[Index2].AndMask = CurrentItemPtr->AndMask;
      ListPtr[Index2].OrMask = CurrentItemPtr->OrMask;
      (*ListNum) ++;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * The function to initialize internal PCIe clock.
 *
 * @param[in]     CpmTableProtocolPtr Pointer to AMD CPM Table Protocol
 *
 */
VOID
AmdCpmDxePcieClockInit (
  IN      AMD_CPM_TABLE_PROTOCOL      *CpmTableProtocolPtr
  )
{
  AMD_CPM_PCIE_CLOCK_TABLE            *CpmPcieClockTablePtr;
  AMD_CPM_PCIE_CLOCK_ITEM             *TablePtr;
  UINT8                               SlotStatus;
  BOOLEAN                             ClockType;
  AMD_CPM_EXT_CLKGEN_TABLE            *ExtClkGenTablePtr;
  AMD_CPM_EXT_CLKGEN_ITEM             ClkGen[AMD_PCIE_CLOCK_SIZE];
  UINT8                               Index;
  UINT8                               Data;
  UINT8                               i;
  UINT32                              PcieSlotStatus;

  ExtClkGenTablePtr = NULL;
  Index = 0;
  PcieSlotStatus = 0;
  CpmTableProtocolPtr->CommonFunction.PostCode (CpmTpGpioInitDxePcieClockInitBegin);
  CpmPcieClockTablePtr  = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_PCIE_CLOCK);
  ClockType             = (BOOLEAN) CpmTableProtocolPtr->CommonFunction.GetStrap (CpmTableProtocolPtr) & BIT1;
  if (!ClockType) {
    ExtClkGenTablePtr   = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_EXT_CLKGEN);
    if (!ExtClkGenTablePtr) {
      return;
    }
  }
  if (CpmPcieClockTablePtr && CpmTableProtocolPtr->MainTablePtr->UnusedGppClkOffEn) {
    TablePtr = &CpmPcieClockTablePtr->Item[0];
    i = 0;
    while (TablePtr->ClkId != 0xFF) {
      SlotStatus = 1;
      if (TablePtr->SlotCheck & BIT1) {
        SlotStatus = CpmTableProtocolPtr->CommonFunction.DetectDevice (CpmTableProtocolPtr, TablePtr->DeviceId, NULL) ? 1 : 0;
      }
      if ((SlotStatus == 1) && (TablePtr->SlotCheck & BIT0)) {
        SlotStatus = CpmTableProtocolPtr->CommonFunction.CheckPcieDevice (CpmTableProtocolPtr, TablePtr->Device, TablePtr->Function);
        if ((SlotStatus > 1) && (TablePtr->SlotCheck & BIT2)) {
          TablePtr->ClkReq = 0xFF;
          TablePtr->ClkReqExt = 0xFF;
        }
      }
      if (SlotStatus == 0) {
        TablePtr->ClkReq = 0;
        TablePtr->ClkReqExt = 0;
      }
      PcieSlotStatus |= SlotStatus << (2 * i);
      if (ClockType) {
        CpmTableProtocolPtr->CommonFunction.SetClkReq (CpmTableProtocolPtr, TablePtr->ClkId, TablePtr->ClkReq);
      } else {
        AmdCpmDxeAddOneItemInClkList (ExtClkGenTablePtr, &ClkGen[0], &Index, TablePtr->ClkIdExt, TablePtr->ClkReqExt);
      }
      TablePtr++;
      i++;
    }

    if (!ClockType) {
      i = 0;
      while (Index > 0) {
        if (CpmTableProtocolPtr->CommonFunction.ReadSmbusBlock) {
          CpmTableProtocolPtr->CommonFunction.ReadSmbusBlock (
                                                CpmTableProtocolPtr,
                                                ExtClkGenTablePtr->SmbusSelect,
                                                ExtClkGenTablePtr->SmbusAddress,
                                                ClkGen[i].Offset,
                                                1,
                                                &Data
                                                );
          Data = (Data & ClkGen[i].AndMask) | ClkGen[i].OrMask;
          CpmTableProtocolPtr->CommonFunction.WriteSmbusBlock (
                                                CpmTableProtocolPtr,
                                                ExtClkGenTablePtr->SmbusSelect,
                                                ExtClkGenTablePtr->SmbusAddress,
                                                ClkGen[i].Offset,
                                                1,
                                                &Data
                                                );
        }
        Index --;
        i ++;
      }
    }
  }

  CpmTableProtocolPtr->CommonFunction.PostCode (CpmTpGpioInitDxePcieClockInitEnd);

  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * The function to initialize GEVENT pins.
 *
 * @param[in]     CpmTableProtocolPtr Pointer to AMD CPM Table Protocol
 *
 */
VOID
AmdCpmDxeGeventInit (
  IN      AMD_CPM_TABLE_PROTOCOL  *CpmTableProtocolPtr
  )
{
  AMD_CPM_GEVENT_INIT_TABLE       *GeventTablePtr;
  AMD_CPM_GEVENT_ITEM             *TablePtr;
  AMD_CPM_EXPRESS_CARD_TABLE      *ExpressCardTablePtr;
  AMD_CPM_OTHER_HOTPLUG_CARD_TABLE  *OtherHotplugTablePtr;

  GeventTablePtr   = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_GEVENT_INIT);
  if (GeventTablePtr) {
    TablePtr = &GeventTablePtr->GeventList[0];
    while (TablePtr->Pin != 0xFF) {
      CpmTableProtocolPtr->CommonFunction.SetGevent (CpmTableProtocolPtr, TablePtr->Pin, TablePtr->Setting.Raw);
      TablePtr++;
    }
  }

  ExpressCardTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_PCIE_EXPRESS_CARD);
  if (ExpressCardTablePtr) {
    CpmTableProtocolPtr->CommonFunction.SetGeventSciTrig (
                                                CpmTableProtocolPtr,
                                                ExpressCardTablePtr->EventPin,
                                                CpmTableProtocolPtr->CommonFunction.GetGevent (CpmTableProtocolPtr, ExpressCardTablePtr->EventPin) ? 0 : 1
                                                );
  }

  OtherHotplugTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_PCIE_OTHER_HOTPLUG_CARD);
  if (OtherHotplugTablePtr) {
    if (OtherHotplugTablePtr->Number > 0) {
      CpmTableProtocolPtr->CommonFunction.SetGeventSciTrig (
                                                CpmTableProtocolPtr,
                                                OtherHotplugTablePtr->EventPin0,
                                                CpmTableProtocolPtr->CommonFunction.GetGevent (CpmTableProtocolPtr, OtherHotplugTablePtr->EventPin0) ? 0 : 1
                                                );
    }
    if (OtherHotplugTablePtr->Number > 1) {
      CpmTableProtocolPtr->CommonFunction.SetGeventSciTrig (
                                                CpmTableProtocolPtr,
                                                OtherHotplugTablePtr->EventPin1,
                                                CpmTableProtocolPtr->CommonFunction.GetGevent (CpmTableProtocolPtr, OtherHotplugTablePtr->EventPin1) ? 0 : 1
                                                );
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * The function to initialize SMI Control for GEVENT pins.
 *
 * @param[in]     CpmTableProtocolPtr Pointer to AMD CPM Table Protocol
 *
 */
VOID
AmdCpmGeventSmiControlInit (
  IN      AMD_CPM_TABLE_PROTOCOL  *CpmTableProtocolPtr
  )
{
  AMD_CPM_GEVENT_INIT_TABLE       *GeventTablePtr;
  AMD_CPM_GEVENT_ITEM             *TablePtr;

  GeventTablePtr   = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_GEVENT_INIT);
  if (GeventTablePtr) {
    TablePtr = &GeventTablePtr->GeventList[0];
    while (TablePtr->Pin != 0xFF) {
      if (TablePtr->Pin < 0x40) {
        CpmTableProtocolPtr->CommonFunction.SetSmiControl (CpmTableProtocolPtr, (UINT8) TablePtr->Pin, TablePtr->Setting.Raw);
      }
      TablePtr++;
    }
  }
}
