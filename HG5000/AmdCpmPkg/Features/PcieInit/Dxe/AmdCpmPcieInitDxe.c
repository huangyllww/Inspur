/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM PCIE Initialization
 *
 * Contains CPM code to perform PCIE initialization under DXE
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  PcieInit
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
#include <Library/PcdLib.h>
#include <AmdBoardId.h>
#include <TSFch.h>

#define SATA_PCIE_SEL_SOCKET            0
#define SATA_PCIE_SEL_DIE               2
#define SATA_PCIE_SEL_PIN               23


EFI_GUID EXPRESS_CARD_ACPI_DEVICE_GUID = {
  0x5f26ca84, 0x837d, 0x4c3e, 0x9d, 0x76, 0xb6, 0xa7, 0x88, 0xb7, 0x50, 0x57
};

VOID
EFIAPI
AmdCpmPcieInitLate (
  IN      EFI_EVENT           Event,
  IN      VOID                *Context
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM PCIE Init DXE driver
 *
 * This function loads, overrides and installs Express Card SSDT table.
 *
 * @param[in]     ImageHandle    Pointer to the firmware file system header
 * @param[in]     SystemTable    Pointer to System table
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */

EFI_STATUS
EFIAPI
AmdCpmPcieInitDxeEntryPoint (
  IN      EFI_HANDLE                      ImageHandle,
  IN      EFI_SYSTEM_TABLE                *SystemTable
  )
{
  EFI_STATUS                              Status;
  EFI_EVENT                               ReadyToBootEvent;

  //
  // Initialize Global Variable
  //
  CpmInitializeDriverLib (ImageHandle, SystemTable);

  Status = gBS->CreateEventEx (
                              CPM_EVENT_NOTIFY_SIGNAL,
                              CPM_TPL_NOTIFY,
                              AmdCpmPcieInitLate,
                              NULL,
                              &gEfiEventReadyToBootGuid,
                              &ReadyToBootEvent
                              );

  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Callback function to update Express Card SSDT table
 *
 * This function is used to update GPE number and PCIE Bridge Name
 *
 * @param[in]     This           Pointer to Protocol
 * @param[in]     AmlObjPtr      The AML Object Buffer
 * @param[in]     Context        The Parameter Buffer
 *
 * @retval        TRUE           SSDT Table has been updated completely
 * @retval        FALSE          SSDT Table has not been updated completely
 */

BOOLEAN
EFIAPI
AmdCpmExpressCardSsdtCallBack (
  IN      VOID                    *This,
  IN      VOID                    *AmlObjPtr,
  IN      VOID                    *Context
  )
{
  UINT32  *BufferPtr;

  BufferPtr = (UINT32*)Context;

  switch ( *((UINT32*)AmlObjPtr)) {
  case (CPM_SIGNATURE_32 ('M', '1', '2', '9')):     // CpmExpressCardEventMethod: M129
    if (*(BufferPtr )) {
      *(UINT32*) AmlObjPtr = *(BufferPtr);
    }
    break;
  case (CPM_SIGNATURE_32 ('M', '1', '3', '0')):     // CpmExpressCardBridge: M130
    if (*(BufferPtr + 1)) {
      *(UINT32*) AmlObjPtr = *(BufferPtr + 1);
    }
    break;
  case (CPM_SIGNATURE_32 ('M', '2', '2', '2')):     // CpmOtherHotplugCard0EventMethod: M222
    if (*(BufferPtr + 8)) {
      *(UINT32*) AmlObjPtr = *(BufferPtr + 8);
    }
    break;
  case (CPM_SIGNATURE_32 ('M', '2', '2', '3')):     // CpmOtherHotplugCard0Bridge: M223
    if (*(BufferPtr + 9)) {
      *(UINT32*) AmlObjPtr = *(BufferPtr + 9);
    }
    break;
  case (CPM_SIGNATURE_32 ('M', '2', '2', '4')):     // CpmOtherHotplugCard1EventMethod: M224
    if (*(BufferPtr + 10)) {
      *(UINT32*) AmlObjPtr = *(BufferPtr + 10);
    }
    break;
  case (CPM_SIGNATURE_32 ('M', '2', '2', '5')):     // CpmOtherHotplugCard1Bridge: M225
    if (*(BufferPtr + 11)) {
      *(UINT32*) AmlObjPtr = *(BufferPtr + 11);
    }
    break;
  }
  return FALSE;
}


/*----------------------------------------------------------------------------------------*/
/**
 * The function to load, override and install AMD CPM Express Card SSDT table.
 *
 * This function gets called each time the EFI_EVENT_SIGNAL_READY_TO_BOOT gets signaled.
 *
 * @param[in]     Event          EFI_EVENT
 * @param[in]     Context        The Parameter Buffer
 *
 */

VOID
EFIAPI
AmdCpmPcieInitLate (
  IN      EFI_EVENT               Event,
  IN      VOID                    *Context
  )
{
  AMD_CPM_NV_DATA_PROTOCOL        *CpmNvDataProtocolPtr;
  AMD_CPM_TABLE_PROTOCOL          *CpmTableProtocolPtr;
  AMD_CPM_EXPRESS_CARD_TABLE      *ExpressCardTablePtr;
  AMD_CPM_OTHER_HOTPLUG_CARD_TABLE  *OtherHotplugTablePtr;
  AMD_CPM_PCIE_TOPOLOGY_TABLE     *PcieTopologyTablePtr;
  AMD_CPM_DXIO_TOPOLOGY_TABLE     *DxioTopologyTablePtr;
  STATIC BOOLEAN                  InitlateInvoked = FALSE;
  EFI_STATUS                      Status;
  UINT32                          Buffer[12];
  UINT8                           SciMap;
  UINT8                           Index;
  UINT32                          IsFchBridge;
  UINT8                           SataPcieSelect;
  HYGON_BOARD_ID                  PlatformSelect;
  
  PlatformSelect = PcdGet8(PcdPlatformSelect);
  if (!InitlateInvoked) {

    Status = gBS->LocateProtocol (  &gAmdCpmTableProtocolGuid,
                                    NULL,
                                    (VOID**)&CpmTableProtocolPtr
                                    );
    if (EFI_ERROR (Status)) {
      return;
    }

    CpmTableProtocolPtr->CommonFunction.PostCode (CpmTpPcieInitDxeDriverBegin);

    Status = gBS->LocateProtocol (  &gAmdCpmNvDataProtocolGuid,
                                    NULL,
                                    (VOID**)&CpmNvDataProtocolPtr
                                    );
    if (EFI_ERROR (Status)) {
      return;
    }

    ExpressCardTablePtr   = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_PCIE_EXPRESS_CARD);
    OtherHotplugTablePtr  = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_PCIE_OTHER_HOTPLUG_CARD);
    PcieTopologyTablePtr  = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_PCIE_TOPOLOGY);
    if(PlatformSelect == HYGON_65N16) {
        SataPcieSelect = CpmTableProtocolPtr->CommonFunction.SmnRead8(CpmTableProtocolPtr, SATA_PCIE_SEL_SOCKET, SATA_PCIE_SEL_DIE, (UINT32) FCH_SMN_GPIO_BASE, (UINT32) ((SATA_PCIE_SEL_PIN << 2) + 2)) & 0x01;
        if (0 == SataPcieSelect){  // PCIe Interface 
            DxioTopologyTablePtr  = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);
        } else if (1 == SataPcieSelect) { // SATA Interface
            DxioTopologyTablePtr  = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, EPRO_CPM_SIGNATURE_DXIO_TOPOLOGY);
        }
    } else {
        DxioTopologyTablePtr  = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);
    }
    if (ExpressCardTablePtr) {
      SciMap = CpmTableProtocolPtr->CommonFunction.GetSciMap (CpmTableProtocolPtr, ExpressCardTablePtr->EventPin);

      Buffer[0] = 'L_';
      if ((SciMap & 0xF) < 0xA) {
        Buffer[0] |= (0x30 + ((SciMap) & 0xF)) << 24;
      } else {
        Buffer[0] |= (0x40 + (((SciMap) & 0xF) - 0x9)) << 24;
      }
      if (((SciMap >> 4) & 0xF) < 0xA) {
        Buffer[0] |= (0x30 + ((SciMap >> 4) & 0xF)) << 16;
      } else {
        Buffer[0] |= (0x40 + (((SciMap >> 4) & 0xF) - 0x9)) << 16;
      }
      IsFchBridge = CpmTableProtocolPtr->CommonFunction.IsFchDevice (
                                                          CpmTableProtocolPtr,
                                                          ExpressCardTablePtr->Device,
                                                          ExpressCardTablePtr->Function
                                                          );
      if (IsFchBridge) {
        Buffer[1] = CpmTableProtocolPtr->CommonFunction.GetFchPcieAslName (
                                                          CpmTableProtocolPtr,
                                                          ExpressCardTablePtr->Device,
                                                          ExpressCardTablePtr->Function
                                                          );
      } else {
        Buffer[1] = CpmTableProtocolPtr->CommonFunction.GetPcieAslName (
                                                          CpmTableProtocolPtr,
                                                          ExpressCardTablePtr->Device,
                                                          ExpressCardTablePtr->Function
                                                          );
      }
    } else {
      Buffer[0] = 0;
      Buffer[1] = 0;
    }

    for (Index = 0; Index < 6; Index++) {
      Buffer[2 + Index] = 0;
    }

    Buffer[8] = 0;
    Buffer[9] = 0;
    Buffer[10] = 0;
    Buffer[11] = 0;

    if (OtherHotplugTablePtr) {
      if (OtherHotplugTablePtr->Number > 0) {
        SciMap = CpmTableProtocolPtr->CommonFunction.GetSciMap (CpmTableProtocolPtr, OtherHotplugTablePtr->EventPin0);

        Buffer[8] = 'L_';
        if ((SciMap & 0xF) < 0xA) {
          Buffer[8] |= (0x30 + ((SciMap) & 0xF)) << 24;
        } else {
          Buffer[8] |= (0x40 + (((SciMap) & 0xF) - 0x9)) << 24;
        }
        if (((SciMap >> 4) & 0xF) < 0xA) {
          Buffer[8] |= (0x30 + ((SciMap >> 4) & 0xF)) << 16;
        } else {
          Buffer[8] |= (0x40 + (((SciMap >> 4) & 0xF) - 0x9)) << 16;
        }
        IsFchBridge = CpmTableProtocolPtr->CommonFunction.IsFchDevice (
                                                            CpmTableProtocolPtr,
                                                            OtherHotplugTablePtr->Device0,
                                                            OtherHotplugTablePtr->Function0
                                                            );
        if (IsFchBridge) {
          Buffer[9] = CpmTableProtocolPtr->CommonFunction.GetFchPcieAslName (
                                                            CpmTableProtocolPtr,
                                                            OtherHotplugTablePtr->Device0,
                                                            OtherHotplugTablePtr->Function0
                                                            );
        } else {
          Buffer[9] = CpmTableProtocolPtr->CommonFunction.GetPcieAslName (
                                                            CpmTableProtocolPtr,
                                                            OtherHotplugTablePtr->Device0,
                                                            OtherHotplugTablePtr->Function0
                                                            );
        }
      }
      if (OtherHotplugTablePtr->Number > 1) {
        SciMap = CpmTableProtocolPtr->CommonFunction.GetSciMap (CpmTableProtocolPtr, OtherHotplugTablePtr->EventPin1);

        Buffer[10] = 'L_';
        if ((SciMap & 0xF) < 0xA) {
          Buffer[10] |= (0x30 + ((SciMap) & 0xF)) << 24;
        } else {
          Buffer[10] |= (0x40 + (((SciMap) & 0xF) - 0x9)) << 24;
        }
        if (((SciMap >> 4) & 0xF) < 0xA) {
          Buffer[10] |= (0x30 + ((SciMap >> 4) & 0xF)) << 16;
        } else {
          Buffer[10] |= (0x40 + (((SciMap >> 4) & 0xF) - 0x9)) << 16;
        }
        IsFchBridge = CpmTableProtocolPtr->CommonFunction.IsFchDevice (
                                                            CpmTableProtocolPtr,
                                                            OtherHotplugTablePtr->Device1,
                                                            OtherHotplugTablePtr->Function1
                                                            );
        if (IsFchBridge) {
          Buffer[11] = CpmTableProtocolPtr->CommonFunction.GetFchPcieAslName (
                                                            CpmTableProtocolPtr,
                                                            OtherHotplugTablePtr->Device1,
                                                            OtherHotplugTablePtr->Function1
                                                            );
        } else {
          Buffer[11] = CpmTableProtocolPtr->CommonFunction.GetPcieAslName (
                                                            CpmTableProtocolPtr,
                                                            OtherHotplugTablePtr->Device1,
                                                            OtherHotplugTablePtr->Function1
                                                            );
        }
      }
    }

    if (ExpressCardTablePtr || OtherHotplugTablePtr) {
      CpmTableProtocolPtr->CommonFunction.AddSsdtTable (
                                                          CpmTableProtocolPtr,
                                                          &EXPRESS_CARD_ACPI_DEVICE_GUID,
                                                          NULL,
                                                          AmdCpmExpressCardSsdtCallBack,
                                                          &Buffer[0]
                                                          );
    }
    CpmTableProtocolPtr->CommonFunction.PostCode (CpmTpPcieInitDxeDriverEnd);
  }
  InitlateInvoked = TRUE;

  return;
}
