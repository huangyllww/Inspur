/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM PCIE Initialization
 *
 * Contains CPM code to perform PCIE initialization under DXE
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

#include <HygonCpmDxe.h>
#include <Library/PcdLib.h>
#include <HygonBoardId.h>
#include <TSFch.h>

#include <GnbRegisters.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Protocol/HygonNbioPcieServicesProtocol.h>
#include <protocol/PciIo.h>
#include <protocol/PciRootBridgeIo.h>
#include <IndustryStandard/PciExpress21.h>

EFI_GUID  EXPRESS_CARD_ACPI_DEVICE_GUID = {
  0x5f26ca84, 0x837d, 0x4c3e, 0x9d, 0x76, 0xb6, 0xa7, 0x88, 0xb7, 0x50, 0x57
};

VOID
EFIAPI
HygonCpmPcieInitLate (
  IN      EFI_EVENT           Event,
  IN      VOID                *Context
  );

EFI_STATUS
EFIAPI
HygonCpmPcieBoardInitDxe (
  );
  
/*----------------------------------------------------------------------------------------*/

/**
 * Entry point of the HYGON CPM PCIE Init DXE driver
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
HygonCpmPcieInitDxeEntryPoint (
  IN      EFI_HANDLE                      ImageHandle,
  IN      EFI_SYSTEM_TABLE                *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   ReadyToBootEvent;

  DEBUG ((DEBUG_INFO, "HygonCpmPcieInitDxeEntryPoint Entry\n"));
  //
  // Initialize Global Variable
  //
  CpmInitializeDriverLib (ImageHandle, SystemTable);

  Status = gBS->CreateEventEx (
                  CPM_EVENT_NOTIFY_SIGNAL,
                  CPM_TPL_NOTIFY,
                  HygonCpmPcieInitLate,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &ReadyToBootEvent
                  );

  // Check Slot Num 
  //HygonCpmPcieBoardInitDxe();
  
  DEBUG ((DEBUG_INFO, "HygonCpmPcieInitDxeEntryPoint Exit\n"));
  
  return Status;
}

EFI_STATUS
EFIAPI
HygonCpmPcieBoardInitDxe (
  )
{
  EFI_STATUS                           Status;
  PCIe_PLATFORM_CONFIG                 *Pcie;
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL  *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB        *PciePlatformConfigHobData;
  GNB_HANDLE                           *NbioHandle;
  UINT32                               Address;
  PCIe_ENGINE_CONFIG                   *EngineList;
  PCIe_WRAPPER_CONFIG                  *Wrapper;
  UINT32                               HeaderType;
  PCI_REG_PCIE_SLOT_CAPABILITY         SlotCap;
  UINT8                                Dev;
  UINT8                                Fun;
  UINT32                               HotPlugSlotNum = 1;
  UINT32                               SlotNum = 0x3f;
  BOOLEAN                              IsMutiFunction;

  DEBUG ((DEBUG_INFO, "HygonCpmPcieBoardInitDxe Entry\n"));
  Wrapper    = (PCIe_WRAPPER_CONFIG *)NULL;
  EngineList = (PCIe_ENGINE_CONFIG *)NULL;

  Status = gBS->LocateProtocol (
                  &gHygonNbioPcieServicesProtocolGuid,
                  NULL,
                  &PcieServicesProtocol
                  );
  if(Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "LocalProtocol fail\n"));
  }

  if(Status == EFI_SUCCESS) {
    Status = PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **)&PciePlatformConfigHobData);
    if(Status != EFI_SUCCESS) {
      return Status;
    }

    Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
    NbioHandle = NbioGetHandle (Pcie);
    while(NbioHandle != NULL) {
      for(Dev = 0; Dev < 32; Dev++) {
        IsMutiFunction = FALSE;
        for(Fun = 0; Fun < 7; Fun++) {
          Address = MAKE_SBDFO (0, 0, Dev, Fun, 0xc);
          NbioRegisterRead (NbioHandle, TYPE_PCI, Address, &HeaderType, 0);
          if(Fun == 0) {
            if((HeaderType != 0xFFFFFFFF) && (HeaderType & BIT23)) {
              IsMutiFunction = TRUE;
            }
          }

          if(Fun && IsMutiFunction == FALSE) {
            break;
          }

          if(HeaderType == 0xFFFFFFFF) {
            continue;
          }

          DEBUG ((DEBUG_INFO, "HeaderType %x Dev %x Fun %x\n", HeaderType, Dev, Fun));
          if(HeaderType & BIT16) {
            // P2P bridge
            Address = MAKE_SBDFO (0, 0, Dev, Fun, 0x6c);
            NbioRegisterRead (NbioHandle, TYPE_PCI, Address, &SlotCap, 0);
            if(SlotCap.Bits.HotPlugCapable == 1) {
              DEBUG ((DEBUG_INFO, "hotplug en slot num %x\n", HotPlugSlotNum));
              SlotCap.Bits.PhysicalSlotNumber = HotPlugSlotNum;
              NbioRegisterWrite (NbioHandle, TYPE_PCI, Address, &SlotCap, 0);
              HotPlugSlotNum++;
            } else {
              DEBUG ((DEBUG_INFO, "hotplug not en slot num %x\n", SlotNum));
              SlotCap.Bits.PhysicalSlotNumber = SlotNum;
              NbioRegisterWrite (NbioHandle, TYPE_PCI, Address, &SlotCap, 0);
              SlotNum++;
            }
          }
        }   // for fun
      }  // for Dev

      NbioHandle = GnbGetNextHandle (NbioHandle);
    }
  }

  DEBUG ((DEBUG_INFO, "HygonCpmPcieBoardInitDxe Exit\n"));
  return EFI_SUCCESS;  
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
HygonCpmExpressCardSsdtCallBack (
  IN      VOID                    *This,
  IN      VOID                    *AmlObjPtr,
  IN      VOID                    *Context
  )
{
  UINT32  *BufferPtr;

  BufferPtr = (UINT32 *)Context;

  switch ( *((UINT32 *)AmlObjPtr)) {
    case (CPM_SIGNATURE_32 ('M', '1', '2', '9')):   // CpmExpressCardEventMethod: M129
      if (*(BufferPtr)) {
        *(UINT32 *)AmlObjPtr = *(BufferPtr);
      }

      break;
    case (CPM_SIGNATURE_32 ('M', '1', '3', '0')):   // CpmExpressCardBridge: M130
      if (*(BufferPtr + 1)) {
        *(UINT32 *)AmlObjPtr = *(BufferPtr + 1);
      }

      break;
    case (CPM_SIGNATURE_32 ('M', '2', '2', '2')):   // CpmOtherHotplugCard0EventMethod: M222
      if (*(BufferPtr + 8)) {
        *(UINT32 *)AmlObjPtr = *(BufferPtr + 8);
      }

      break;
    case (CPM_SIGNATURE_32 ('M', '2', '2', '3')):   // CpmOtherHotplugCard0Bridge: M223
      if (*(BufferPtr + 9)) {
        *(UINT32 *)AmlObjPtr = *(BufferPtr + 9);
      }

      break;
    case (CPM_SIGNATURE_32 ('M', '2', '2', '4')):   // CpmOtherHotplugCard1EventMethod: M224
      if (*(BufferPtr + 10)) {
        *(UINT32 *)AmlObjPtr = *(BufferPtr + 10);
      }

      break;
    case (CPM_SIGNATURE_32 ('M', '2', '2', '5')):   // CpmOtherHotplugCard1Bridge: M225
      if (*(BufferPtr + 11)) {
        *(UINT32 *)AmlObjPtr = *(BufferPtr + 11);
      }

      break;
  }

  return FALSE;
}

/*----------------------------------------------------------------------------------------*/

/**
 * The function to load, override and install HYGON CPM Express Card SSDT table.
 *
 * This function gets called each time the EFI_EVENT_SIGNAL_READY_TO_BOOT gets signaled.
 *
 * @param[in]     Event          EFI_EVENT
 * @param[in]     Context        The Parameter Buffer
 *
 */
VOID
EFIAPI
HygonCpmPcieInitLate (
  IN      EFI_EVENT               Event,
  IN      VOID                    *Context
  )
{
  HYGON_CPM_NV_DATA_PROTOCOL          *CpmNvDataProtocolPtr;
  HYGON_CPM_TABLE_PROTOCOL            *CpmTableProtocolPtr;
  HYGON_CPM_EXPRESS_CARD_TABLE        *ExpressCardTablePtr;
  HYGON_CPM_OTHER_HOTPLUG_CARD_TABLE  *OtherHotplugTablePtr;
  HYGON_CPM_PCIE_TOPOLOGY_TABLE       *PcieTopologyTablePtr;
  HYGON_CPM_HSIO_TOPOLOGY_TABLE       *HsioTopologyTablePtr;
  STATIC BOOLEAN                      InitlateInvoked = FALSE;
  EFI_STATUS                          Status;
  UINT32                              Buffer[12];
  UINT8                               SciMap;
  UINT8                               Index;
  UINT32                              IsFchBridge;
  UINT32                              CpuModel;

  if (!InitlateInvoked) {
    Status = gBS->LocateProtocol (
                    &gHygonCpmTableProtocolGuid,
                    NULL,
                    (VOID **)&CpmTableProtocolPtr
                    );
    if (EFI_ERROR (Status)) {
      return;
    }

    CpmTableProtocolPtr->CommonFunction.PostCode (CpmTpPcieInitDxeDriverBegin);

    Status = gBS->LocateProtocol (
                    &gHygonCpmNvDataProtocolGuid,
                    NULL,
                    (VOID **)&CpmNvDataProtocolPtr
                    );
    if (EFI_ERROR (Status)) {
      return;
    }

    ExpressCardTablePtr  = NULL;
    OtherHotplugTablePtr = NULL;
    PcieTopologyTablePtr = NULL;
    HsioTopologyTablePtr = NULL;
    CpuModel             = GetHygonSocModel();
    
    if (CpuModel == HYGON_EX_CPU) {
      ExpressCardTablePtr  = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYEX_CPM_SIGNATURE_PCIE_EXPRESS_CARD);
      OtherHotplugTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYEX_CPM_SIGNATURE_PCIE_OTHER_HOTPLUG_CARD);
      PcieTopologyTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYEX_CPM_SIGNATURE_PCIE_TOPOLOGY);
      HsioTopologyTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY);
    } else if (CpuModel == HYGON_GX_CPU) {
      ExpressCardTablePtr  = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYGX_CPM_SIGNATURE_PCIE_EXPRESS_CARD);
      OtherHotplugTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYGX_CPM_SIGNATURE_PCIE_OTHER_HOTPLUG_CARD);
      PcieTopologyTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYGX_CPM_SIGNATURE_PCIE_TOPOLOGY);
      HsioTopologyTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY);
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

    Buffer[8]  = 0;
    Buffer[9]  = 0;
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
                                            HygonCpmExpressCardSsdtCallBack,
                                            &Buffer[0]
                                            );
    }

    CpmTableProtocolPtr->CommonFunction.PostCode (CpmTpPcieInitDxeDriverEnd);
  }

  InitlateInvoked = TRUE;

  return;
}
