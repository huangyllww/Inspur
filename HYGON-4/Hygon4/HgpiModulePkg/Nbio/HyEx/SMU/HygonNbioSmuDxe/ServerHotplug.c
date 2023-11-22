/* $NoKeywords:$ */

/**
 * @file
 *
 * ServerHotplug Initialization
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioSmuDxe
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
#include <HygonNbioSmuDxe.h>
#include <CcxRegistersDm.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/HygonNbioPcieServicesProtocol.h>
#include <Protocol/HygonCcxProtocol.h>
#include <GnbHsio.h>
#include <Guid/GnbPcieInfoHob.h>
#include <GnbRegisters.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieConfigLib.h>
#include <IdsHookId.h>
#include <Library/HygonIdsHookLib.h>
#include <Guid/HobList.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioSmuLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/GnbLib.h>

#include <HygonServerHotplug.h>
#include <HygonPcieComplex.h>
#include <Protocol/NbioHotplugDesc.h>
#include <Library/TimerLib.h>

#define FILECODE  NBIO_SMU_HYEX_HYGONNBIOSMUDXE_PCIEHOTPLUG_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define HotplugGetNextDescriptor(Descriptor) (Descriptor != NULL ? ((((Descriptor->Flags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : (++Descriptor))) : NULL)

#define SLOT_CNTL_DEFAULT_SETTING  (UINT32) ((1 << SLOT_CNTL_ATTN_BUTTON_PRESSED_EN_OFFSET) |\
                                             (1 << SLOT_CNTL_PWR_FAULT_DETECTED_EN_OFFSET) |\
                                             (0 << SLOT_CNTL_PWR_CONTROLLER_CNTL_OFFSET) |\
                                             (1 << SLOT_CNTL_PRESENCE_DETECT_CHANGED_EN_OFFSET) |\
                                             (1 << SLOT_CNTL_COMMAND_COMPLETED_INTR_EN_OFFSET) |\
                                             (1 << SLOT_CNTL_ATTN_INDICATOR_CNTL_OFFSET) |\
                                             (1 << SLOT_CNTL_PWR_INDICATOR_CNTL_OFFSET) |\
                                             (1 << SLOT_CNTL_DL_STATE_CHANGED_EN_OFFSET) |\
                                             (1 << SLOT_CNTL_HOTPLUG_INTR_EN_OFFSET) )

#define SLOT_CAP_DEFAULT_CONFIGURETION  (UINT32) (0 << SLOT_CAP_NO_COMMAND_COMPLETED_SUPPORTED_OFFSET)
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  UINT32    Address;
  UINT32    SlotNum;
  UINT8     FunctionMask;
  UINT8     Format;
} HP_SLOT_INFO;

typedef struct {
  UINT8           numEntries;
  HP_SLOT_INFO    entries[65];
} HP_SLOT_INFO_LIST;

typedef struct {
  UINT8     HotplugFormat;          ///< Hotplug format selected by user
  UINT8     MaskBit;                ///< Mask bit set by user
  UINT32    CapMask;                ///< Capability bit to be cleared
} MASK_TO_CAP_TABLE;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

MASK_TO_CAP_TABLE  MaskToCapTable[] = {
  // { HotplugExpressModule,  PCIE_HOTPLUG_EXPRESSMODULE_ATNSW_, SLOT_CAP_ATTN_BUTTON_PRESENT_MASK },
  // { HotplugExpressModule,  PCIE_HOTPLUG_EXPRESSMODULE_PWREN_, SLOT_CAP_PWR_CONTROLLER_PRESENT_MASK },
  // { HotplugExpressModule,  PCIE_HOTPLUG_EXPRESSMODULE_ATNLED, SLOT_CAP_ATTN_INDICATOR_PRESENT_MASK},
  // { HotplugExpressModule,  PCIE_HOTPLUG_EXPRESSMODULE_PWRLED, SLOT_CAP_PWR_INDICATOR_PRESENT_MASK },

  { HotplugExpressModuleB, PCIE_HOTPLUG_EXPRESSMODULE_B_ATNSW_, SLOT_CAP_ATTN_BUTTON_PRESENT_MASK    },
  // { HotplugExpressModuleB, PCIE_HOTPLUG_EXPRESSMODULE_B_PWREN_, SLOT_CAP_PWR_CONTROLLER_PRESENT_MASK },
  { HotplugExpressModuleB, PCIE_HOTPLUG_EXPRESSMODULE_B_ATNLED, SLOT_CAP_ATTN_INDICATOR_PRESENT_MASK },
  // { HotplugExpressModuleB, PCIE_HOTPLUG_EXPRESSMODULE_B_PWRLED, SLOT_CAP_PWR_INDICATOR_PRESENT_MASK },
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
  UINT32                SlotNum = 0;
  UINT32                SmnAddress;
  UINT32                Value;
  UINT32                Index;

  if ((Engine->Type.Port.PortData.LinkHotplug == HsioHotplugServerExpress ) || (Engine->Type.Port.PortData.LinkHotplug == HsioHotplugServerEntSSD )) {
    
    IDS_HDT_CONSOLE (GNB_TRACE, "ServerHotplugFinalInitCallback: Hotplug Port%x\n", Engine->Type.Port.PortId);

    Wrapper = PcieConfigGetParentWrapper (Engine);
    NbioHandle = (GNB_HANDLE *) PcieConfigGetParentSilicon (Engine);
    SmnAddress = NBIO_SPACE (NbioHandle, NB_DEVINDCFG0_IOHC_Bridge_CNTL_ADDRESS_HYEX) + (Engine->Type.Port.LogicalBridgeId << 10);

    NbioRegisterRMW ( NbioHandle,
                      TYPE_SMN,
                      SmnAddress,
                      (UINT32)~(IOHC_BRIDGE_CNTL_BridgeDis_MASK),
                      (0 << IOHC_BRIDGE_CNTL_BridgeDis_OFFSET),
                      0
                      );

    for (Index = 0; Index < ((HP_SLOT_INFO_LIST *)Buffer)->numEntries; Index++) {
      if (Engine->Type.Port.Address.AddressValue ==
          ((HP_SLOT_INFO_LIST *)Buffer)->entries[Index].Address) {
        SlotNum = ((HP_SLOT_INFO_LIST *)Buffer)->entries[Index].SlotNum;
      }
    }

    if (SlotNum == 0) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Error: No Slot Num definded in the HotplugUserConfig for This Port\n");
      ASSERT (SlotNum != 0);
      return;
    }

    // if PRESENCE_DETECT is clear, then turn off slot power among things
    NbioRegisterRead ( NbioHandle,
                       TYPE_PCI,
                       MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_PCI_OFFSET),
                       &Value,
                       0);

    
    if ( Value & ((UINT32)(1 << SLOT_CNTL_PRESENCE_DETECT_STATE_OFFSET)) ) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Found endpoint -> power on slot\n");
      NbioRegisterRMW (NbioHandle,
                       TYPE_PCI,
                       MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_PCI_OFFSET),
                       (UINT32) ~(SLOT_CNTL_PWR_CONTROLLER_CNTL_MASK),
                       (UINT32) (0 << SLOT_CNTL_PWR_CONTROLLER_CNTL_OFFSET),
                       0
                       );
    } else {
      IDS_HDT_CONSOLE (GNB_TRACE, "No endpoint -> power off slot\n", Value);
      NbioRegisterRMW (NbioHandle,
                       TYPE_PCI,
                       MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_PCI_OFFSET),
                       (UINT32) ~(SLOT_CNTL_PWR_CONTROLLER_CNTL_MASK),
                       (UINT32) (1 << SLOT_CNTL_PWR_CONTROLLER_CNTL_OFFSET),
                       0
                       );
    }
    MicroSecondDelay (50000);
    // Clear SLOT_CNTL
    NbioRegisterRead ( NbioHandle,
                       TYPE_PCI,
                       MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_PCI_OFFSET),
                       &Value,
                       0);

    NbioRegisterWrite (NbioHandle,
                       TYPE_PCI,
                       MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_PCI_OFFSET),
                       &Value,
                       0);

  }
}

/**
* Callback to init hotplug features on all hotplug ports
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
  UINT32                SlotNum = 0;
  UINT32                Index;
  UINT32                Address;
  UINT32                Value;
  UINT8                 FunctionMask = 0;
  UINT8                 HotplugFormat = 0;

  if ((Engine->Type.Port.PortData.LinkHotplug == HsioHotplugServerExpress ) || (Engine->Type.Port.PortData.LinkHotplug == HsioHotplugServerEntSSD )) {

    IDS_HDT_CONSOLE (GNB_TRACE, "ServerHotplugPreInitCallback: Hotplug Port%x\n", Engine->Type.Port.PortId);

    Wrapper = PcieConfigGetParentWrapper (Engine);
    NbioHandle = (GNB_HANDLE *) PcieConfigGetParentSilicon (Engine);
    SmnAddress = ConvertPcieCoreAddress (PCIE0_CNTL_ADDRESS_HYEX, NbioHandle, Wrapper);

    NbioRegisterRMW (NbioHandle,
                      TYPE_SMN,
                      SmnAddress,
                      (UINT32) ~(PCIE_CNTL_HWINIT_WR_LOCK_MASK),
                      0 << PCIE_CNTL_HWINIT_WR_LOCK_OFFSET,
                      0
                      );

    for (Index = 0; Index < ((HP_SLOT_INFO_LIST *)Buffer)->numEntries; Index++) {
      if (Engine->Type.Port.Address.AddressValue ==
          ((HP_SLOT_INFO_LIST *)Buffer)->entries[Index].Address) {
        SlotNum = ((HP_SLOT_INFO_LIST *)Buffer)->entries[Index].SlotNum;
        FunctionMask = ((HP_SLOT_INFO_LIST *)Buffer)->entries[Index].FunctionMask;
        HotplugFormat = ((HP_SLOT_INFO_LIST *)Buffer)->entries[Index].Format;
        break;
      }
    }

    if (SlotNum == 0) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Error: No Slot Num definded in the HotplugUserConfig for This Port\n");
      ASSERT (SlotNum != 0);
      return;
    }

    // read the Slot Cap register
    Address = MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CAP_PCI_OFFSET);
    NbioRegisterRead ( NbioHandle,
                       TYPE_PCI,
                       Address,
                       &Value,
                       0);

    IDS_HDT_CONSOLE (GNB_TRACE, "SlotCap (in)  = %x\n", Value);
    
    // Set the Slot Cap register default value
    Value &= ~SLOT_CAP_DEFAULT_CONFIGURETION;
    Value |= SLOT_CAP_DEFAULT_CONFIGURETION;

    // Clear any needed bits depending on the hotplug format ...
    for (Index = 0; Index < sizeof (MaskToCapTable) / sizeof (MASK_TO_CAP_TABLE); Index++) {
      if (HotplugFormat == MaskToCapTable[Index].HotplugFormat) {
        if (FunctionMask & MaskToCapTable[Index].MaskBit) {
          Value &= ~(MaskToCapTable[Index].CapMask);
        }
      }
    }

    Value &= (UINT32) ~(SLOT_CAP_PHYSICAL_SLOT_NUM_MASK);
    Value |= (UINT32) (SlotNum << SLOT_CAP_PHYSICAL_SLOT_NUM_OFFSET);

    IDS_HDT_CONSOLE (GNB_TRACE, "SlotCap (out) = %x\n", Value);

    // Write the Slot Cap register
    NbioRegisterWrite ( NbioHandle,
                        TYPE_PCI,
                        Address,
                        &Value,
                        0);

    NbioRegisterRMW (NbioHandle,
                      TYPE_SMN,
                      SmnAddress,
                      (UINT32) ~(PCIE_CNTL_HWINIT_WR_LOCK_MASK),
                      1 << PCIE_CNTL_HWINIT_WR_LOCK_OFFSET,
                      0
                      );

    SmnAddress = ConvertPciePortAddress (0x1114036C, NbioHandle, Wrapper, (Engine->Type.Port.PortId % 8));
    NbioRegisterRMW ( NbioHandle,
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
  IDS_HDT_CONSOLE (GNB_TRACE, "  HotPlugDescriptor.EngineDescriptor:\n");
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
  IDS_HDT_CONSOLE (GNB_TRACE, "  AlternateSlotNumber = 0x%x\n", HotPlugConfig->MappingDescriptor[SlotNumber].AlternateSlotNumber);
  IDS_HDT_CONSOLE (GNB_TRACE, "  PrimarySecondaryLink = 0x%x\n", HotPlugConfig->MappingDescriptor[SlotNumber].PrimarySecondaryLink);
  IDS_HDT_CONSOLE (GNB_TRACE, "  DieID = 0x%x\n", HotPlugConfig->MappingDescriptor[SlotNumber].DieID);
  IDS_HDT_CONSOLE (GNB_TRACE, " FunctionDescriptor(Slot 0x%x):\n", SlotNumber);
  IDS_HDT_CONSOLE (GNB_TRACE, "  I2CGpioBitSelector = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].I2CGpioBitSelector);
  IDS_HDT_CONSOLE (GNB_TRACE, "  I2CGpioNibbleSelector = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].I2CGpioNibbleSelector);
  IDS_HDT_CONSOLE (GNB_TRACE, "  I2CGpioByteMapping = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].I2CGpioByteMapping);
  IDS_HDT_CONSOLE (GNB_TRACE, "  I2CGpioDeviceMapping = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].I2CGpioDeviceMapping);
  IDS_HDT_CONSOLE (GNB_TRACE, "  I2CDeviceType = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].I2CDeviceType);
  IDS_HDT_CONSOLE (GNB_TRACE, "  I2CBusSegment = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].I2CBusSegment);
  IDS_HDT_CONSOLE (GNB_TRACE, "  FunctionMask = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].FunctionMask);
  IDS_HDT_CONSOLE (GNB_TRACE, "  PortMapping = 0x%x\n", HotPlugConfig->FunctionDescriptor[SlotNumber].PortMapping);
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
  IDS_HDT_CONSOLE (GNB_TRACE, " HotPlugPatch(Slot 0x%x):\n", SlotNumber);
  IDS_HDT_CONSOLE (GNB_TRACE, "  PhyPortId = 0x%x\n", HotPlugConfig->HotPlugPatch[SlotNumber].PhyPortId);
  IDS_HDT_CONSOLE (GNB_TRACE, "  StartLane = 0x%x\n", HotPlugConfig->HotPlugPatch[SlotNumber].StartLane);
  IDS_HDT_CONSOLE (GNB_TRACE, "  EndLane = 0x%x\n", HotPlugConfig->HotPlugPatch[SlotNumber].EndLane);
  IDS_HDT_CONSOLE (GNB_TRACE, "  NbioId = 0x%x\n", HotPlugConfig->HotPlugPatch[SlotNumber].NbioId);
  IDS_HDT_CONSOLE (GNB_TRACE, "  reserved = 0x%x\n", HotPlugConfig->HotPlugPatch[SlotNumber].reserved);
}
 
/**
 *---------------------------------------------------------------------------------------
 *  ServerHotplugInitDxe
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
ServerHotplugInitDxe (
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

  IDS_HDT_CONSOLE (GNB_TRACE, "ServerHotplugInitDxe\n");

    Status = gBS->LocateProtocol (
                  &gHygonHotplugDescProtocolGuid,
                  NULL,
                  &HotplugDescriptorProtocol
                  );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE(GNB_TRACE, "ServerHotplugInitDxe Exit protocol is not published\n");
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
          if ((HotplugUserConfig->Engine.StartLane >= Wrapper->StartPhyLane + NbioHandle->SocketStartLane) &&
              (HotplugUserConfig->Engine.EndLane <= Wrapper->EndPhyLane + NbioHandle->SocketStartLane)) {
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
        IDS_HDT_CONSOLE(GNB_TRACE, "Found Hotplug engine lane %d - %d ! \n", HotplugUserConfig->Engine.StartLane, HotplugUserConfig->Engine.EndLane);
        IDS_HDT_CONSOLE(GNB_TRACE, "PCIE engine lane %d - %d ! \n", EngineList->EngineData.StartLane, EngineList->EngineData.EndLane);
        
        if (EngineList->Type.Port.PortData.LinkHotplug != HsioHotplugDisabled) {
          if ((HotplugUserConfig->Engine.StartLane == EngineList->EngineData.StartLane) && (HotplugUserConfig->Engine.EndLane == EngineList->EngineData.EndLane)) {
            SlotFound = TRUE;
            SlotNum = EngineList->Type.Port.PortData.SlotNum;
			      if(SlotNum == 0){
			        IDS_HDT_CONSOLE (GNB_TRACE, "  ERROR: SlotNum is 0x%X, make sure SlotNum is correct\n", SlotNum);
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
      DescriptorIndex = SlotNum;
      if (DescriptorIndex > 0x3F){
        IDS_HDT_CONSOLE (GNB_TRACE, "  ERROR: DescriptorIndex is 0x%X,Please make sure DescriptorIndex range in 0x00-0x3F\n", DescriptorIndex);
        ASSERT(FALSE);
      }

      LibHygonMemCopy ((VOID*) &HotPlugConfig->MappingDescriptor[DescriptorIndex], (VOID*) &HotplugUserConfig->Mapping, sizeof (PcieHotPlugMappingDescriptor_t), (HYGON_CONFIG_PARAMS *) NULL);
      LibHygonMemCopy ((VOID*) &HotPlugConfig->FunctionDescriptor[DescriptorIndex], (VOID*) &HotplugUserConfig->Function, sizeof (PcieHotPlugFunctionDescriptor_t), (HYGON_CONFIG_PARAMS *) NULL);
      LibHygonMemCopy ((VOID*) &HotPlugConfig->ResetDescriptor[DescriptorIndex], (VOID*) &HotplugUserConfig->Reset, sizeof (PcieHotPlugResetDescriptor_t), (HYGON_CONFIG_PARAMS *) NULL);
      LibHygonMemCopy ((VOID*) &HotPlugConfig->GpioDescriptor[DescriptorIndex], (VOID*) &HotplugUserConfig->Gpio, sizeof (SmuGenericGpioDescriptor_t), (HYGON_CONFIG_PARAMS *) NULL);
      HotPlugConfig->MappingDescriptor[DescriptorIndex].ApertureID = 0x111 + Wrapper->WrapId * 4 + NbioHandle->RbId;
      HotPlugConfig->MappingDescriptor[DescriptorIndex].PortActive = 1;
      HotPlugConfig->MappingDescriptor[DescriptorIndex].DieID = NbioHandle->SocketId;
      HotPlugConfig->FunctionDescriptor[DescriptorIndex].PortMapping = EngineList->Type.Port.LogicalBridgeId;
      HotPlugConfig->HotPlugPatch[DescriptorIndex].PhyPortId = EngineList->Type.Port.PortId;
      HotPlugConfig->HotPlugPatch[DescriptorIndex].StartLane = EngineList->EngineData.StartLane % 16;
      HotPlugConfig->HotPlugPatch[DescriptorIndex].EndLane = EngineList->EngineData.EndLane % 16;
      HotPlugConfig->HotPlugPatch[DescriptorIndex].NbioId = NbioHandle->RbId;
      DumpPcieHotPlugConfigTable(HotPlugConfig, HotplugUserConfig, DescriptorIndex);
    }
    HotplugUserConfig = HotplugGetNextDescriptor (HotplugUserConfig);
  }

  GnbLibDebugDumpBuffer ((VOID*) HotPlugConfig, sizeof (PcieHotPlugConfigTable_t), 3, 8);

  // Pass hotplug descriptor table address to SMU
  NbioSmuServiceCommonInitArguments (SmuArg);
  SmuArg[0] = (UINT32) ((EFI_PHYSICAL_ADDRESS) HotPlugConfig & 0xFFFFFFFF);
  SmuResult = NbioSmuServiceRequest (NbioGetHostPciAddress (MasterHandle), 0, SMC_MSG_SetBiosDramAddrLow, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
  if (SmuResult != SMC_Result_OK) {
    IDS_HDT_CONSOLE (GNB_TRACE, "SMC_MSG_SetBiosDramAddrLow Return Status = %d\n", SmuResult);
    return EFI_INVALID_PARAMETER;
  }

  NbioSmuServiceCommonInitArguments (SmuArg);
  SmuArg[0] = (UINT32) RShiftU64 ((UINT64)HotPlugConfig, 32);
  SmuResult = NbioSmuServiceRequest (NbioGetHostPciAddress (MasterHandle), 0, SMC_MSG_SetBiosDramAddrHigh, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
  if (SmuResult != SMC_Result_OK) {
    IDS_HDT_CONSOLE (GNB_TRACE, "SMC_MSG_SetBiosDramAddrHigh Return Status = %d\n", SmuResult);
    return EFI_INVALID_PARAMETER;
  }

  // Transfer hotplug descriptor table to SMU
  NbioSmuServiceCommonInitArguments (SmuArg);
  SmuArg[0] = TABLE_PCIE_HP_CONFIG;
  SmuResult = NbioSmuServiceRequest (NbioGetHostPciAddress (MasterHandle), 0, SMC_MSG_TransferTableDram2Smu, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
  if (SmuResult != SMC_Result_OK) {
    IDS_HDT_CONSOLE (GNB_TRACE, "SMC_MSG_TransferTableDram2Smu Return Status = %d\n", SmuResult);
    return EFI_INVALID_PARAMETER;
  }

  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    ServerHotplugPreInitCallback,
    &HplugSlotInfo,
    Pcie
    );

  NbioHandle = NbioGetHandle (Pcie);
  while (NbioHandle != NULL) {
    if(PcdGetBool(PcdCfgPcieHotplugSupport) == TRUE) {
        // Enable Hotplug Config
        NbioSmuServiceCommonInitArguments (SmuArg);
        HotplugMode = PcdGet8 (PcdCfgHotplugMode);
        HotplugMode <<= 4;
        HotplugSlotIndex = PcdGet8 (PcdHotplugSlotIndex) & 0xF;
        SmuArg[0] = HotplugSlotIndex | HotplugMode;
        SmuResult = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_EnableHotPlug, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
        if (SmuResult != SMC_Result_OK) {
          IDS_HDT_CONSOLE (GNB_TRACE, "SMC_MSG_EnableHotPlug Return Status = %d\n", SmuResult);
          return EFI_INVALID_PARAMETER;
        }
    }
    NbioHandle = GnbGetNextSocketHandle (NbioHandle);
  }

  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    ServerHotplugFinalInitCallback,
    &HplugSlotInfo,
    Pcie
    );

  IDS_HDT_CONSOLE (GNB_TRACE, "ServerHotplugInitDxe Exit Status = %d\n", Status);
  return Status;
}

