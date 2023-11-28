/* $NoKeywords:$ */
/**
 * @file
 *
 * ServerHotplugV9 Initialization
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioSmuV9Dxe
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
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
#include <PiDxe.h>
#include <Filecode.h>
#include <AmdNbioSmuV9Dxe.h>
#include <CcxRegistersZp.h>
#include "smu9_bios_if.h"
#include "smu9.h"

#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <Protocol/AmdCcxProtocol.h>
#include <GnbDxio.h>
#include <Guid/GnbPcieInfoHob.h>
#include <GnbRegistersZP.h>
#include <Library/GnbRegisterAccZpLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieConfigLib.h>
#include <IdsHookId.h>
#include <Library/AmdIdsHookLib.h>
#include <Guid/HobList.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioSmuV9Lib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/GnbLib.h>

#include <AmdServerHotplug.h>
#include <AmdPcieComplex.h>
#include <Protocol/NbioHotplugDesc.h>


#define FILECODE        NBIO_SMU_AMDNBIOSMUV9DXE_PCIEHOTPLUGV9_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define HotplugGetNextDescriptor(Descriptor) (Descriptor != NULL ? ((((Descriptor->Flags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : (++Descriptor))) : NULL)

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  UINT32       Address;
  UINT32       SlotNum;
  UINT8        FunctionMask;
  UINT8        Format;
} HP_SLOT_INFO;


typedef struct {
  UINT8 numEntries;
  HP_SLOT_INFO entries[65];
} HP_SLOT_INFO_LIST;


typedef struct {
  UINT8        HotplugFormat;       ///< Hotplug format selected by user
  UINT8        MaskBit;             ///< Mask bit set by user
  UINT32       CapMask;             ///< Capability bit to be cleared
} MASK_TO_CAP_TABLE;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

MASK_TO_CAP_TABLE MaskToCapTable[] = {
    //{ HotplugExpressModule,  PCIE_HOTPLUG_EXPRESSMODULE_ATNSW_, SLOT_CAP_ATTN_BUTTON_PRESENT_MASK },
    //{ HotplugExpressModule,  PCIE_HOTPLUG_EXPRESSMODULE_PWREN_, SLOT_CAP_PWR_CONTROLLER_PRESENT_MASK },
    //{ HotplugExpressModule,  PCIE_HOTPLUG_EXPRESSMODULE_ATNLED, SLOT_CAP_ATTN_INDICATOR_PRESENT_MASK},
    //{ HotplugExpressModule,  PCIE_HOTPLUG_EXPRESSMODULE_PWRLED, SLOT_CAP_PWR_INDICATOR_PRESENT_MASK },
    
    { HotplugExpressModuleB, PCIE_HOTPLUG_EXPRESSMODULE_B_ATNSW_, SLOT_CAP_ATTN_BUTTON_PRESENT_MASK },
    //{ HotplugExpressModuleB, PCIE_HOTPLUG_EXPRESSMODULE_B_PWREN_, SLOT_CAP_PWR_CONTROLLER_PRESENT_MASK },
    { HotplugExpressModuleB, PCIE_HOTPLUG_EXPRESSMODULE_B_ATNLED, SLOT_CAP_ATTN_INDICATOR_PRESENT_MASK},
    //{ HotplugExpressModuleB, PCIE_HOTPLUG_EXPRESSMODULE_B_PWRLED, SLOT_CAP_PWR_INDICATOR_PRESENT_MASK },
};

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */



/**
 * Callback to init hotplug features on all hotplug ports
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */

VOID
STATIC
ServerHotplugFinalInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIe_WRAPPER_CONFIG   *Wrapper;
  GNB_HANDLE            *NbioHandle;
  UINT32                SlotNum;
  UINT32                Address;
  UINT32                Value;
  UINT32                Index;
  UINT8                 FunctionMask;
  UINT8                 HotplugFormat;

  IDS_HDT_CONSOLE (GNB_TRACE, "ServerHotplugFinalInitCallback Enter\n");
  if ((Engine->Type.Port.PortData.LinkHotplug == DxioHotplugServerExpress ) || (Engine->Type.Port.PortData.LinkHotplug == DxioHotplugServerEntSSD )) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Found Server Hotplug Engine at:\n");
    IDS_HDT_CONSOLE (GNB_TRACE, "  Port.PortId = %d\n", Engine->Type.Port.PortId);
    IDS_HDT_CONSOLE (GNB_TRACE, "  Port.PcieBridgeId = %d\n", Engine->Type.Port.PcieBridgeId);
    IDS_HDT_CONSOLE (GNB_TRACE, "  Port.Address = %x\n", Engine->Type.Port.Address.AddressValue);
    IDS_HDT_CONSOLE (GNB_TRACE, "  Device = %x, Function = %x, LinkHotplug=%x\n", 
            Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, Engine->Type.Port.PortData.LinkHotplug);
    IDS_HDT_CONSOLE (GNB_TRACE, "  Type = ");

    Wrapper = PcieConfigGetParentWrapper (Engine);
    NbioHandle = (GNB_HANDLE *) PcieConfigGetParentSilicon (Engine);

    NbioRegisterRMW (NbioHandle,
                     TYPE_SMN,
                     (Wrapper->WrapId == 0) ? PCIE0_CNTL_ADDRESS : PCIE1_CNTL_ADDRESS,
                     (UINT32) ~(PCIE_CNTL_HWINIT_WR_LOCK_MASK),
                     0 << PCIE_CNTL_HWINIT_WR_LOCK_OFFSET,
                     0
                     );

    IDS_HDT_CONSOLE (GNB_TRACE, "  PROG DevNum = %d\n", Engine->Type.Port.PortData.DeviceNumber);
    IDS_HDT_CONSOLE (GNB_TRACE, "  PROG FunNum = %d\n", Engine->Type.Port.PortData.FunctionNumber);
    IDS_HDT_CONSOLE (GNB_TRACE, "  PROG Addr = %x\n", Engine->Type.Port.Address.AddressValue);

    SlotNum = 0;
    FunctionMask = 0;
    HotplugFormat = 0;
    for (Index = 0; Index < ((HP_SLOT_INFO_LIST *)Buffer)->numEntries; Index++) {
      if (Engine->Type.Port.Address.AddressValue ==
          ((HP_SLOT_INFO_LIST *)Buffer)->entries[Index].Address) {
        SlotNum = ((HP_SLOT_INFO_LIST *)Buffer)->entries[Index].SlotNum;
        FunctionMask = ((HP_SLOT_INFO_LIST *)Buffer)->entries[Index].FunctionMask;
        HotplugFormat = ((HP_SLOT_INFO_LIST *)Buffer)->entries[Index].Format;
      }
    }
    IDS_HDT_CONSOLE (GNB_TRACE, "  Found SlotNum     = %d\n", SlotNum);
    IDS_HDT_CONSOLE (GNB_TRACE, "        Mask        = 0x%02x, HotplugFormat=%d\n", FunctionMask, HotplugFormat);

    // Some of the "hotplug function mask" bits require a Slot Cap bit to be cleared,
    // so read the Slot Cap register ...
    Address = MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CAP_ADDRESS);
    NbioRegisterRead ( NbioHandle,
                       TYPE_PCI,
                       Address,
                       &Value,
                       0);

    IDS_HDT_CONSOLE (GNB_TRACE, "  SlotCap (in)      = %x\n", Value);

    // ... and clear any needed bits depending on the hotplug format ...
    for (Index = 0; Index < sizeof (MaskToCapTable) / sizeof (MASK_TO_CAP_TABLE); Index++) {
      if (HotplugFormat == MaskToCapTable[Index].HotplugFormat) {
        if (FunctionMask & MaskToCapTable[Index].MaskBit) {
          Value &= ~(MaskToCapTable[Index].CapMask);
        }
      }
    }

    IDS_HDT_CONSOLE (GNB_TRACE, "  SlotCap (out)     = %x\n", Value);

    // ... and then write tne new value back to the Slot Cap register
    NbioRegisterWrite ( NbioHandle,
                        TYPE_PCI,
                        Address,
                        &Value,
                        0);

    // update cap reg with slot number
    NbioRegisterRMW (NbioHandle,
                     TYPE_PCI,
                     Address,
                     (UINT32) ~(SLOT_CAP_PHYSICAL_SLOT_NUM_MASK),
                     (UINT32) (SlotNum << SLOT_CAP_PHYSICAL_SLOT_NUM_OFFSET),
                     0
                     );


    // if PRESENCE_DETECT is clear, then turn off slot power among things
    NbioRegisterRead ( NbioHandle,
                       TYPE_PCI,
                       MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_ADDRESS),
                       &Value,
                       0);

    IDS_HDT_CONSOLE (GNB_TRACE, "  SlotCtl           = %x\n", Value);

    if ( Value & ((UINT32)(1 << 22)) ) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Found endpoint -> power on slot\n");

      NbioRegisterRMW (NbioHandle,
                       TYPE_PCI,
                       MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_ADDRESS),
                       (UINT32) ~(SLOT_CNTL_ATTN_BUTTON_PRESSED_EN_MASK |
                           SLOT_CNTL_PWR_FAULT_DETECTED_EN_MASK |
                           SLOT_CNTL_PWR_CONTROLLER_CNTL_MASK |
                           SLOT_CNTL_PRESENCE_DETECT_CHANGED_EN_MASK |
						   SLOT_CNTL_COMMAND_COMPLETED_INTR_EN_MASK |
						   SLOT_CNTL_ATTN_INDICATOR_CNTL_MASK |
                           SLOT_CNTL_PWR_INDICATOR_CNTL_MASK |
						   SLOT_CNTL_DL_STATE_CHANGED_EN_MASK |
                           SLOT_CNTL_HOTPLUG_INTR_EN_MASK),
                       (UINT32) ((1 << SLOT_CNTL_ATTN_BUTTON_PRESSED_EN_OFFSET) |
                           (1 << SLOT_CNTL_PWR_FAULT_DETECTED_EN_OFFSET) |
                           (0 << SLOT_CNTL_PWR_CONTROLLER_CNTL_OFFSET) |
                           (1 << SLOT_CNTL_PRESENCE_DETECT_CHANGED_EN_OFFSET) |
                           (1 << SLOT_CNTL_COMMAND_COMPLETED_INTR_EN_OFFSET) |
                           (1 << SLOT_CNTL_ATTN_INDICATOR_CNTL_OFFSET) |
                           //j_t (1 << SLOT_CNTL_PWR_INDICATOR_CNTL_OFFSET) |
                           (0 << SLOT_CNTL_PWR_INDICATOR_CNTL_OFFSET) |
                           (1 << SLOT_CNTL_DL_STATE_CHANGED_EN_OFFSET) |
                           (1 << SLOT_CNTL_HOTPLUG_INTR_EN_OFFSET) ),
                       0
                       );

    } else {
      IDS_HDT_CONSOLE (GNB_TRACE, "No endpoint -> power off slot\n", Value);

      NbioRegisterRMW (NbioHandle,
                       TYPE_PCI,
                       MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_ADDRESS),
                       (UINT32) ~(SLOT_CNTL_ATTN_BUTTON_PRESSED_EN_MASK |
					              SLOT_CNTL_PWR_FAULT_DETECTED_EN_MASK |
                                  SLOT_CNTL_PWR_CONTROLLER_CNTL_MASK |
                                  SLOT_CNTL_PRESENCE_DETECT_CHANGED_EN_MASK |
								  SLOT_CNTL_COMMAND_COMPLETED_INTR_EN_MASK |
								  SLOT_CNTL_ATTN_INDICATOR_CNTL_MASK |
								  SLOT_CNTL_PWR_INDICATOR_CNTL_MASK |
								  SLOT_CNTL_DL_STATE_CHANGED_EN_MASK |
                                  SLOT_CNTL_HOTPLUG_INTR_EN_MASK),
                       (UINT32) ((1 << SLOT_CNTL_ATTN_BUTTON_PRESSED_EN_OFFSET) |
//                           (1 << SLOT_CNTL_PWR_FAULT_DETECTED_EN_OFFSET) |
                           (1 << SLOT_CNTL_PWR_CONTROLLER_CNTL_OFFSET) |
                           (1 << SLOT_CNTL_PRESENCE_DETECT_CHANGED_EN_OFFSET) |
                           (1 << SLOT_CNTL_COMMAND_COMPLETED_INTR_EN_OFFSET) |
                           (3 << SLOT_CNTL_ATTN_INDICATOR_CNTL_OFFSET) |
                           //j_t (3 << SLOT_CNTL_PWR_INDICATOR_CNTL_OFFSET) |
                           (0 << SLOT_CNTL_PWR_INDICATOR_CNTL_OFFSET) |
                           (1 << SLOT_CNTL_DL_STATE_CHANGED_EN_OFFSET) |
                           (1 << SLOT_CNTL_HOTPLUG_INTR_EN_OFFSET) ),
                       0
                       );
    }

    // Clear SLOT_CNTL
    NbioRegisterRead ( NbioHandle,
                       TYPE_PCI,
                       MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_ADDRESS),
                       &Value,
                       0);

    IDS_HDT_CONSOLE (GNB_TRACE, "  SlotCtl (out)     = %x\n", Value);

    NbioRegisterWrite (NbioHandle,
                       TYPE_PCI,
                       MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_ADDRESS),
                       &Value,
                       0);

  }
}


/**
 * Delay 1ms
 *
 *
 *
 * @param[in]  Handle           Pointer to GNB_HANDLE
 *
 */

VOID
STATIC
Delay1ms (
  IN       GNB_HANDLE    *NbioHandle
  )
{
   UINT32 t1;
   UINT32 t2;
   NbioRegisterRead (NbioHandle, TYPE_SMN, 0x11180488, &t1, 0);
   t2 = t1;
   while ((t2 - t1) < 1000L) {
     NbioRegisterRead (NbioHandle, TYPE_SMN, 0x11180488, &t2, 0);
   }
}

/**
 * Verify active link for present hot plug end points
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */

VOID
STATIC
ServerHotplugLinkTestCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIe_WRAPPER_CONFIG   *Wrapper;
  GNB_HANDLE            *NbioHandle;
  UINT32                SmnAddress;
  UINT32                Value;
  BOOLEAN               DevicePresent;
  BOOLEAN               LinkActive;

  IDS_HDT_CONSOLE (GNB_TRACE, "ServerHotplugLinkTestCallback Enter\n");

  DevicePresent = FALSE;
  LinkActive = FALSE;

  if (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled ) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Found Hotplug Engine at:\n");

    Wrapper = PcieConfigGetParentWrapper (Engine);
    NbioHandle = (GNB_HANDLE *) PcieConfigGetParentSilicon (Engine);

    // Preload the DevicePresent and DL_Active status bits
    SmnAddress = (Wrapper->WrapId == 0) ?  GPP0_SLOT_CNTL_ADDRESS : GFX0_SLOT_CNTL_ADDRESS;
    SmnAddress |= (Engine->Type.Port.PortId % 8) << 12;
    NbioRegisterRead (NbioHandle, TYPE_SMN, SmnAddress, &Value, 0);
    if (Value & (UINT32)(1 << SLOT_CNTL_PRESENCE_DETECT_STATE_OFFSET)) {
      DevicePresent = TRUE;
    }

    SmnAddress = (Wrapper->WrapId == 0) ?  GPP0_LINK_CTL_STS_ADDRESS : GPP1_LINK_CTL_STS_ADDRESS;
    SmnAddress |= (Engine->Type.Port.PortId % 8) << 12;
    NbioRegisterRead (NbioHandle, TYPE_SMN, SmnAddress, &Value, 0);
    SmnAddress = SMN_0x13B31004_ADDRESS + (Engine->Type.Port.LogicalBridgeId << 10);
    if (Value & (UINT32)(1 << LINK_CTL_STS_DL_ACTIVE_OFFSET)) {
      LinkActive = TRUE;
    }

    // first check for Presence - if that is clear then set BridgeDis and move on
    if (!DevicePresent) {
      SmnAddress = SMN_0x13B31004_ADDRESS + (Engine->Type.Port.LogicalBridgeId << 10);
      NbioRegisterRMW (NbioHandle,
                       TYPE_SMN,
                       SmnAddress,
                       (UINT32)~(SMN_0x13B31004_BridgeDis_MASK),
                       (1 << SMN_0x13B31004_BridgeDis_OFFSET),
                       0
                       );
      IDS_HDT_CONSOLE (GNB_TRACE, "EP not present: %08x\n", SmnAddress);
    } else {
      if (LinkActive) {
        SmnAddress = SMN_0x13B31004_ADDRESS + (Engine->Type.Port.LogicalBridgeId << 10);
        NbioRegisterRMW (NbioHandle,
                         TYPE_SMN,
                         SmnAddress,
                         (UINT32)~(SMN_0x13B31004_BridgeDis_MASK),
                         (0 << SMN_0x13B31004_BridgeDis_OFFSET),
                         0
                         );
        IDS_HDT_CONSOLE (GNB_TRACE, "EP is present: %08x\n", SmnAddress);
      } else {
        // This is the problematic case: EP present but link not active
        UINT16 Count;
        Count = 1000;
        IDS_HDT_CONSOLE (GNB_TRACE, "Link down but Device is present: %08x\n",
            SmnAddress);
        IDS_HDT_CONSOLE (GNB_TRACE, "... wait for a good link\n");

        while (Count--) {
          Delay1ms ( NbioHandle );
          SmnAddress = (Wrapper->WrapId == 0) ?
                        GPP0_LINK_CTL_STS_ADDRESS : GPP1_LINK_CTL_STS_ADDRESS;
          SmnAddress |= (Engine->Type.Port.PortId % 8) << 12;
          NbioRegisterRead (NbioHandle, TYPE_SMN, SmnAddress, &Value, 0);
          if (Value & (UINT32)(1 << LINK_CTL_STS_DL_ACTIVE_OFFSET)) {
            LinkActive = TRUE;
            break;
          }
        }

        if (LinkActive) {
          IDS_HDT_CONSOLE (GNB_TRACE, "Link active\n");
          SmnAddress = SMN_0x13B31004_ADDRESS +
                       (Engine->Type.Port.LogicalBridgeId << 10);
          NbioRegisterRMW (NbioHandle,
                           TYPE_SMN,
                           SmnAddress,
                           (UINT32)~(SMN_0x13B31004_BridgeDis_MASK),
                           (0 << SMN_0x13B31004_BridgeDis_OFFSET),
                           0
                           );
        } else {
          IDS_HDT_CONSOLE (GNB_TRACE, "No link\n");
          SmnAddress = SMN_0x13B31004_ADDRESS +
                       (Engine->Type.Port.LogicalBridgeId << 10);
          NbioRegisterRMW (NbioHandle,
                           TYPE_SMN,
                           SmnAddress,
                           (UINT32)~(SMN_0x13B31004_BridgeDis_MASK),
                           (1 << SMN_0x13B31004_BridgeDis_OFFSET),
                           0
                           );
        }
      }
    }

  }

  IDS_HDT_CONSOLE (GNB_TRACE, "ServerHotplugLinkTestCallback Exit\n");

}

 /**
  * Callback to init hotplug features on all hotplug ports
  *
  *
  *
  *
  * @param[in]       Engine          Pointer to engine config descriptor
  * @param[in, out]  Buffer          Not used
  * @param[in]       Pcie            Pointer to global PCIe configuration
  *
  */

 VOID
 STATIC
 ServerHotplugPreInitCallback (
   IN       PCIe_ENGINE_CONFIG    *Engine,
   IN OUT   VOID                  *Buffer,
   IN       PCIe_PLATFORM_CONFIG  *Pcie
   )
 {
   PCIe_WRAPPER_CONFIG   *Wrapper;
   GNB_HANDLE            *NbioHandle;
   UINT32                SmnAddress;
   UINT32                SlotNum;
   UINT32                Index;
   UINT32                Address;
   UINT32                Value;

   IDS_HDT_CONSOLE (GNB_TRACE, "ServerHotplugPreInitCallback Enter\n");

   if ((Engine->Type.Port.PortData.LinkHotplug == DxioHotplugServerExpress ) || (Engine->Type.Port.PortData.LinkHotplug == DxioHotplugServerEntSSD )) {

     IDS_HDT_CONSOLE (GNB_TRACE, "Found Server Hotplug Engine at:\n");
     IDS_HDT_CONSOLE (GNB_TRACE, "  Port.PortId = %d\n", Engine->Type.Port.PortId);
     IDS_HDT_CONSOLE (GNB_TRACE, "  Port.PcieBridgeId = %d\n", Engine->Type.Port.PcieBridgeId);
     IDS_HDT_CONSOLE (GNB_TRACE, "  Port.Address = %x\n", Engine->Type.Port.Address);

     Wrapper = PcieConfigGetParentWrapper (Engine);
     NbioHandle = (GNB_HANDLE *) PcieConfigGetParentSilicon (Engine);

     NbioRegisterRMW (NbioHandle,
                      TYPE_SMN,
                      (Wrapper->WrapId == 0) ? PCIE0_CNTL_ADDRESS : PCIE1_CNTL_ADDRESS,
                      (UINT32) ~(PCIE_CNTL_HWINIT_WR_LOCK_MASK),
                      0 << PCIE_CNTL_HWINIT_WR_LOCK_OFFSET,
                      0
                      );

     IDS_HDT_CONSOLE (GNB_TRACE, "  PROG DevNum = %d\n", Engine->Type.Port.PortData.DeviceNumber);
     IDS_HDT_CONSOLE (GNB_TRACE, "  PROG FunNum = %d\n", Engine->Type.Port.PortData.FunctionNumber);
     IDS_HDT_CONSOLE (GNB_TRACE, "  PROG Addr = %x\n", Engine->Type.Port.Address.AddressValue);

     SlotNum = 0;
     for (Index = 0; Index < ((HP_SLOT_INFO_LIST *)Buffer)->numEntries; Index++) {
       if (Engine->Type.Port.Address.AddressValue ==
           ((HP_SLOT_INFO_LIST *)Buffer)->entries[Index].Address) {
         SlotNum = ((HP_SLOT_INFO_LIST *)Buffer)->entries[Index].SlotNum;
       }
     }
     IDS_HDT_CONSOLE (GNB_TRACE, "  Found SlotNum     = %d\n", SlotNum);

     Address = MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CAP_ADDRESS);
     NbioRegisterRead ( NbioHandle,
                        TYPE_PCI,
                        Address,
                        &Value,
                        0);
     IDS_HDT_CONSOLE (GNB_TRACE, "  Addr/Val= %x/%x\n", Address, Value);

     NbioRegisterRMW (NbioHandle,
                      TYPE_PCI,
                      Address,
                      (UINT32) ~(SLOT_CAP_PHYSICAL_SLOT_NUM_MASK),
                      (UINT32) (SlotNum << SLOT_CAP_PHYSICAL_SLOT_NUM_OFFSET),
                      GNB_REG_ACC_FLAG_S3SAVE
                      );


     SmnAddress = (Wrapper->WrapId == 0) ? 0x1114036C : 0x1124036C;
     SmnAddress |= (Engine->Type.Port.PortId % 8) << 12;
     NbioRegisterRMW (NbioHandle,
                      TYPE_SMN,
                      SmnAddress,
                      (UINT32) ~(0xFFFFFFFF),
                      (UINT32) (0x80000000 | SlotNum),
                      GNB_REG_ACC_FLAG_S3SAVE
                      );

   }
 }

 VOID
 DumpPcieHotPlugConfigTable (
   IN       PcieHotPlugConfigTable_t            *HotPlugConfig,
   IN       HOTPLUG_DESCRIPTOR                  *HotPlugDescriptor,
   IN       UINT32                              SlotNumber
   )
 {
     if(HotPlugConfig == NULL || HotPlugDescriptor == NULL) return;
     
     IDS_HDT_CONSOLE (GNB_TRACE, "HotPlugConfig Table:\n");
     
     IDS_HDT_CONSOLE (GNB_TRACE, "  Flags = 0x%x\n", HotPlugDescriptor->Flags);
     
     IDS_HDT_CONSOLE (GNB_TRACE, " HotPlugDescriptor.EngineDescriptor:\n");
     IDS_HDT_CONSOLE (GNB_TRACE, "  StartLane = 0x%x\n", HotPlugDescriptor->Engine.StartLane);
     IDS_HDT_CONSOLE (GNB_TRACE, "  EndLane = 0x%x\n", HotPlugDescriptor->Engine.EndLane);
     IDS_HDT_CONSOLE (GNB_TRACE, "  SocketNumber = 0x%x\n", HotPlugDescriptor->Engine.SocketNumber);

     IDS_HDT_CONSOLE (GNB_TRACE, " MappingDescriptor(Slot 0x%x):\n", SlotNumber);
     IDS_HDT_CONSOLE (GNB_TRACE, "  HotPlugFormat = 0x%x\n", HotPlugConfig->MappingDescriptor[SlotNumber].HotPlugFormat);
     IDS_HDT_CONSOLE (GNB_TRACE, "  GpioDescriptorValid = 0x%x\n", HotPlugConfig->MappingDescriptor[SlotNumber].GpioDescriptorValid);
     IDS_HDT_CONSOLE (GNB_TRACE, "  ResetDescriptorValid = 0x%x\n", HotPlugConfig->MappingDescriptor[SlotNumber].ResetDescriptorValid);
     IDS_HDT_CONSOLE (GNB_TRACE, "  PortActive = 0x%x\n", HotPlugConfig->MappingDescriptor[SlotNumber].PortActive);
     IDS_HDT_CONSOLE (GNB_TRACE, "  MasterSlaveAPU = 0x%x\n", HotPlugConfig->MappingDescriptor[SlotNumber].MasterSlaveAPU);
     IDS_HDT_CONSOLE (GNB_TRACE, "  DieID = 0x%x\n", HotPlugConfig->MappingDescriptor[SlotNumber].DieID);
     IDS_HDT_CONSOLE (GNB_TRACE, "  ApertureID = 0x%x\n", HotPlugConfig->MappingDescriptor[SlotNumber].ApertureID);
     IDS_HDT_CONSOLE (GNB_TRACE, "  AltSlotNum = 0x%x\n", HotPlugConfig->MappingDescriptor[SlotNumber].AltSlotNum);
     IDS_HDT_CONSOLE (GNB_TRACE, "  SecondaryLink = 0x%x\n", HotPlugConfig->MappingDescriptor[SlotNumber].SecondaryLink);
     IDS_HDT_CONSOLE (GNB_TRACE, "  DieID = 0x%x\n", HotPlugConfig->MappingDescriptor[SlotNumber].DieID);

     IDS_HDT_CONSOLE (GNB_TRACE, " FunctionDescriptor(Slot 0x%x):\n", SlotNumber);
     IDS_HDT_CONSOLE (GNB_TRACE, "  i2c_bit_select = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].i2c_bit_select);
     IDS_HDT_CONSOLE (GNB_TRACE, "  i2c_nibble_select = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].i2c_nibble_select);
     IDS_HDT_CONSOLE (GNB_TRACE, "  i2c_byte_select = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].i2c_byte_select);
     IDS_HDT_CONSOLE (GNB_TRACE, "  i2c_device_address = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].i2c_device_address);
     IDS_HDT_CONSOLE (GNB_TRACE, "  i2c_device_type = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].i2c_device_type);
     IDS_HDT_CONSOLE (GNB_TRACE, "  i2c_bus_segment = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].i2c_bus_segment);
     IDS_HDT_CONSOLE (GNB_TRACE, "  function_mask = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].function_mask);
     IDS_HDT_CONSOLE (GNB_TRACE, "  port_mapping = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].port_mapping);
#if 0
     IDS_HDT_CONSOLE (GNB_TRACE, " ResetDescriptor(Slot 0x%x):\n", SlotNumber);
     IDS_HDT_CONSOLE (GNB_TRACE, "  i2c_byte_select = 0x%x\n", HotPlugConfig->ResetDescriptor[SlotNumber].i2c_byte_select);
     IDS_HDT_CONSOLE (GNB_TRACE, "  i2c_device_address = 0x%x\n", HotPlugConfig->ResetDescriptor[SlotNumber].i2c_device_address);
     IDS_HDT_CONSOLE (GNB_TRACE, "  i2c_device_type = 0x%x\n", HotPlugConfig->ResetDescriptor[SlotNumber].i2c_device_type);
     IDS_HDT_CONSOLE (GNB_TRACE, "  i2c_bus_segment = 0x%x\n", HotPlugConfig->ResetDescriptor[SlotNumber].i2c_bus_segment);
     IDS_HDT_CONSOLE (GNB_TRACE, "  reset_select = 0x%x\n", HotPlugConfig->ResetDescriptor[SlotNumber].reset_select);

     IDS_HDT_CONSOLE (GNB_TRACE, " GpioDescriptor(Slot 0x%x):\n", SlotNumber);
     IDS_HDT_CONSOLE (GNB_TRACE, "  i2c_byte_select = 0x%x\n", HotPlugConfig->GpioDescriptor[SlotNumber].i2c_byte_select);
     IDS_HDT_CONSOLE (GNB_TRACE, "  i2c_device_address = 0x%x\n", HotPlugConfig->GpioDescriptor[SlotNumber].i2c_device_address);
     IDS_HDT_CONSOLE (GNB_TRACE, "  i2c_device_type = 0x%x\n", HotPlugConfig->GpioDescriptor[SlotNumber].i2c_device_type);
     IDS_HDT_CONSOLE (GNB_TRACE, "  i2c_bus_segment = 0x%x\n", HotPlugConfig->GpioDescriptor[SlotNumber].i2c_bus_segment);
     IDS_HDT_CONSOLE (GNB_TRACE, "  gpio_select = 0x%x\n", HotPlugConfig->GpioDescriptor[SlotNumber].gpio_select);
     IDS_HDT_CONSOLE (GNB_TRACE, "  gpio_interrupt_en = 0x%x\n", HotPlugConfig->GpioDescriptor[SlotNumber].gpio_interrupt_en);
#endif
 }

UINT8
GetPhysicalPort(
   IN GNB_HANDLE    *NbioHandle,
   IN UINT32        Device,
   IN UINT32        Function
)
{
    UINT32   Index;
    UINT32   Value;
    UINT32   Addr;
    
    Addr = (Device << 3) + Function;

    //Get PCIE physical port
    for (Index = 0; Index < 18; Index++)
    {
		NbioRegisterRead (NbioHandle, 
			               TYPE_SMN, 
			               NB_PROG_DEVICE_REMAP+(Index << 2), 
			               &Value, 
			               0);
        if (Addr == (Value & 0xFF))
        {
          return (UINT8)Index;
        }
    }
    
    //Error return, never run here
    return 0xFF;
}

 
/**
 *---------------------------------------------------------------------------------------
 *  ServerHotplugInitV9Dxe
 *
 *  Description:
 *     Server Hotplug SMU Initialization
 *  Parameters:
 *    @param[in]     Pcie       PCIe_PLATFORM_CONFIG pointer
 *    @param[in]     *Context   Pointer to the notification function's context.
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS
EFIAPI
ServerHotplugInitV9Dxe (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  EFI_STATUS                    Status;
  GNB_HANDLE                    *NbioHandle;
  GNB_HANDLE                    *MasterHandle;
  UINT32                        SmuArg[6];
  PcieHotPlugConfigTable_t      *HotPlugConfig;
  HOTPLUG_DESCRIPTOR            *HotplugUserConfig;
  PCIe_ENGINE_CONFIG            *EngineList;
  PCIe_WRAPPER_CONFIG           *Wrapper;
  UINT32                        DescriptorIndex;
  BOOLEAN                       SlotFound;
  UINT32                        SlotNum;
  BOOLEAN                       WrapperFound;
  UINT32                        SmuResult;
  UINT32                        HotplugSlotIndex;
  UINT32                        HotplugMode;
  NBIO_HOTPLUG_DESC_PROTOCOL    *HotplugDescriptorProtocol;
  HP_SLOT_INFO_LIST             HplugSlotInfo;
  UINT32                        Address;

  IDS_HDT_CONSOLE (GNB_TRACE, "ServerHotplugInitV9Dxe\n");

    Status = gBS->LocateProtocol (
                  &gAmdHotplugDescProtocolGuid,
                  NULL,
                  &HotplugDescriptorProtocol
                  );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE(GNB_TRACE, "ServerHotplugInitV9Dxe Exit protocol is not published\n");
    return EFI_SUCCESS;
  }
  HotplugUserConfig = HotplugDescriptorProtocol->NbioHotplugDesc;


  Status = gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  sizeof (PcieHotPlugConfigTable_t),
                  &HotPlugConfig
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //Zero Pool
  gBS->SetMem (HotPlugConfig, sizeof (PcieHotPlugConfigTable_t), 0);

  MasterHandle = NbioGetHandle (Pcie);
  Wrapper = (PCIe_WRAPPER_CONFIG*) NULL;
  EngineList = (PCIe_ENGINE_CONFIG*) NULL;

  HplugSlotInfo.numEntries = 0;
  SlotNum = 0;

  while (HotplugUserConfig != NULL) {
    SlotFound = FALSE;
    WrapperFound = FALSE;
    NbioHandle = NbioGetHandle (Pcie);
    MasterHandle = NbioHandle;
    while (NbioHandle != NULL) {
      if (HotplugUserConfig->Engine.SocketNumber == NbioHandle->SocketId) {
        Wrapper = PcieConfigGetChildWrapper (NbioHandle);
        while (Wrapper != NULL) {
          if ((HotplugUserConfig->Engine.StartLane >= Wrapper->StartPhyLane + NbioHandle->StartLane) &&
              (HotplugUserConfig->Engine.EndLane <= Wrapper->EndPhyLane + NbioHandle->StartLane)) {
            WrapperFound = TRUE;
            break;
          }
          Wrapper = PcieLibGetNextDescriptor (Wrapper);
        }
      }
      if (WrapperFound == TRUE) {
        break;
      }
      NbioHandle = GnbGetNextHandle (NbioHandle);
    }
    if (WrapperFound == TRUE) {
      EngineList = PcieConfigGetChildEngine (Wrapper);
      while (EngineList != NULL) {
        IDS_HDT_CONSOLE(GNB_TRACE, "Found Hotplug engine lane %d - %d ! \n", HotplugUserConfig->Engine.StartLane, HotplugUserConfig->Engine.EndLane);  //DM1-216+
        IDS_HDT_CONSOLE(GNB_TRACE, "PCIE engine lane %d - %d ! \n", EngineList->EngineData.StartLane, EngineList->EngineData.EndLane);  //DM1-216+
        
        if (EngineList->Type.Port.PortData.LinkHotplug != DxioHotplugDisabled) {
          if ((HotplugUserConfig->Engine.StartLane == EngineList->EngineData.StartLane) && (HotplugUserConfig->Engine.EndLane == EngineList->EngineData.EndLane)) {
            SlotFound = TRUE;
			
			Address = MAKE_SBDFO (0, 0, EngineList->Type.Port.PortData.DeviceNumber, EngineList->Type.Port.PortData.FunctionNumber, SLOT_CAP_ADDRESS);
			NbioRegisterRead ( NbioHandle,
							   TYPE_PCI,
							   Address,
							   &SlotNum,
							   0);
			SlotNum = (SlotNum >> SLOT_CAP_PHYSICAL_SLOT_NUM_OFFSET);
			if(SlotNum == 0){
			    IDS_HDT_CONSOLE (GNB_TRACE, "  WARNING: SlotNum is 0, make sure SlotNum has been filled\n", SlotNum);
			}
            HplugSlotInfo.entries[HplugSlotInfo.numEntries].FunctionMask = (UINT8) HotplugUserConfig->Function.FunctionMask;
            HplugSlotInfo.entries[HplugSlotInfo.numEntries].Format = (UINT8) HotplugUserConfig->Mapping.HotPlugFormat;
            HplugSlotInfo.entries[HplugSlotInfo.numEntries].SlotNum = SlotNum;
            HplugSlotInfo.entries[HplugSlotInfo.numEntries].Address = EngineList->Type.Port.Address.AddressValue;
            HplugSlotInfo.numEntries++;
            IDS_HDT_CONSOLE (GNB_TRACE, "  SlotNum: %x\n", SlotNum);
            IDS_HDT_CONSOLE (GNB_TRACE, "  Addr: %x\n", EngineList->Type.Port.Address.AddressValue);
            IDS_HDT_CONSOLE (GNB_TRACE, "  Entries: %x\n", HplugSlotInfo.numEntries);
            break;
          }
        }
        EngineList = PcieLibGetNextDescriptor (EngineList);
      }
    }
    if (SlotFound == TRUE) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  Found this slot [%d] at StartLane %d and EndLane %d\n", SlotNum, EngineList->EngineData.StartLane, EngineList->EngineData.EndLane);

      NbioHandle = (GNB_HANDLE *) PcieConfigGetParentSilicon (EngineList);
      Wrapper = PcieConfigGetParentWrapper (EngineList);
      DescriptorIndex = SlotNum - PcdGet8(PcdHotplugSlotIndex);
      LibAmdMemCopy ((VOID*) &HotPlugConfig->MappingDescriptor[DescriptorIndex], (VOID*) &HotplugUserConfig->Mapping, sizeof (PcieHotPlugMappingDescriptor_t), (AMD_CONFIG_PARAMS *) NULL);
      LibAmdMemCopy ((VOID*) &HotPlugConfig->FunctionDescriptor[DescriptorIndex], (VOID*) &HotplugUserConfig->Function, sizeof (PcieHotPlugFunctionDescriptor_t), (AMD_CONFIG_PARAMS *) NULL);
      LibAmdMemCopy ((VOID*) &HotPlugConfig->ResetDescriptor[DescriptorIndex], (VOID*) &HotplugUserConfig->Reset, sizeof (PcieHotPlugResetDescriptor_t), (AMD_CONFIG_PARAMS *) NULL);
      LibAmdMemCopy ((VOID*) &HotPlugConfig->GpioDescriptor[DescriptorIndex], (VOID*) &HotplugUserConfig->Gpio, sizeof (SmuGenericGpioDescriptor_t), (AMD_CONFIG_PARAMS *) NULL);
      HotPlugConfig->MappingDescriptor[DescriptorIndex].ApertureID = 0x111 + Wrapper->WrapId;
      HotPlugConfig->MappingDescriptor[DescriptorIndex].DieID = NbioHandle->InstanceId;
      HotPlugConfig->FunctionDescriptor[DescriptorIndex].port_mapping = EngineList->Type.Port.LogicalBridgeId;
      HotPlugConfig->HotPlugPatch[DescriptorIndex].PhyPortId = GetPhysicalPort(NbioHandle,EngineList->Type.Port.PortData.DeviceNumber,EngineList->Type.Port.PortData.FunctionNumber);
      DumpPcieHotPlugConfigTable(HotPlugConfig, HotplugUserConfig, DescriptorIndex);
    }
    HotplugUserConfig = HotplugGetNextDescriptor (HotplugUserConfig);
  }

  GnbLibDebugDumpBuffer ((VOID*) HotPlugConfig, sizeof (PcieHotPlugConfigTable_t), 3, 8);

  // Pass hotplug descriptor table address to SMU
  NbioSmuServiceCommonInitArgumentsV9 (SmuArg);
  SmuArg[0] = (UINT32) ((EFI_PHYSICAL_ADDRESS) HotPlugConfig & 0xFFFFFFFF);
  SmuResult = NbioSmuServiceRequestV9 (NbioGetHostPciAddress (MasterHandle), SMC_MSG_SetBiosDramAddrLow, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
  if (SmuResult != BIOSSMC_Result_OK) {
    IDS_HDT_CONSOLE (GNB_TRACE, "SMC_MSG_SetBiosDramAddrLow Return Status = %d\n", SmuResult);
    return EFI_INVALID_PARAMETER;
  }

  NbioSmuServiceCommonInitArgumentsV9 (SmuArg);
  SmuArg[0] = (UINT32) RShiftU64 ((UINT64)HotPlugConfig, 32);
  SmuResult = NbioSmuServiceRequestV9 (NbioGetHostPciAddress (MasterHandle), SMC_MSG_SetBiosDramAddrHigh, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
  if (SmuResult != BIOSSMC_Result_OK) {
    IDS_HDT_CONSOLE (GNB_TRACE, "SMC_MSG_SetBiosDramAddrHigh Return Status = %d\n", SmuResult);
    return EFI_INVALID_PARAMETER;
  }

  // Transfer hotplug descriptor table to SMU
  NbioSmuServiceCommonInitArgumentsV9 (SmuArg);
  SmuArg[0] = TABLE_PCIE_HP_CONFIG;
  SmuResult = NbioSmuServiceRequestV9 (NbioGetHostPciAddress (MasterHandle), SMC_MSG_TransferTableDram2Smu, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
  if (SmuResult != BIOSSMC_Result_OK) {
    IDS_HDT_CONSOLE (GNB_TRACE, "SMC_MSG_TransferTableDram2Smu Return Status = %d\n", SmuResult);
    return EFI_INVALID_PARAMETER;
  }

  ///hang on simnow, 0x11180488 always zero.
#if(SimNow_SUPPORT != 1)
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    ServerHotplugPreInitCallback,
    &HplugSlotInfo,
    Pcie
    );

  {
    UINT32 t1;
    UINT32 t2;
    NbioHandle = NbioGetHandle (Pcie);
    NbioRegisterRead (NbioHandle, TYPE_SMN, 0x11180488, &t1, 0);
    t2 = t1;
    while ((t2 - t1) < 100000L) {
      NbioRegisterRead (NbioHandle, TYPE_SMN, 0x11180488, &t2, 0);
    }
  }
#endif
  
  if(PcdGetBool(PcdCfgPcieHotplugSupport) == TRUE) {
      // Enable Hotplug Config
      NbioSmuServiceCommonInitArgumentsV9 (SmuArg);
      HotplugMode = PcdGet8 (PcdCfgHotplugMode);
      HotplugMode <<= 4;
      HotplugSlotIndex = PcdGet8 (PcdHotplugSlotIndex) & 0xF;
      SmuArg[0] = HotplugSlotIndex | HotplugMode;
      SmuResult = NbioSmuServiceRequestV9 (NbioGetHostPciAddress (MasterHandle), SMC_MSG_EnableHotPlug, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
      if (SmuResult != BIOSSMC_Result_OK) {
        IDS_HDT_CONSOLE (GNB_TRACE, "SMC_MSG_EnableHotPlug Return Status = %d\n", SmuResult);
        return EFI_INVALID_PARAMETER;
      }
  }

  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    ServerHotplugFinalInitCallback,
    &HplugSlotInfo,
    Pcie
    );
  
  PcieConfigRunProcForAllEngines (
	DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
	ServerHotplugLinkTestCallback,
	NULL,
	Pcie
	);

  IDS_HDT_CONSOLE (GNB_TRACE, "ServerHotplugInitV9Dxe Exit Status = %d\n", Status);
  return Status;
}



