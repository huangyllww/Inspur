/* $NoKeywords:$ */
/**
 * @file
 *
 * Platform PCIe Complex Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioSmuV9Pei
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
#include <PiPei.h>
#include <AmdPcieComplex.h>
#include <AmdServerHotplug.h>
#include <Protocol/NbioHotplugDesc.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <AmdCpmDxe.h>
#include <AmdCpmDefine.h>

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

HOTPLUG_DESCRIPTOR    HotplugDescriptor[] = {
// 4x J49 NVME
  {
      DESCRIPTOR_TERMINATE_LIST,
      HOTPLUG_ENGINE_DATA_INITIALIZER (32,      // Start Lane
                                       35,      // End Lane
                                       0)       // Socket Number
                                       
      PCIE_HOTPLUG_INITIALIZER_MAPPING (HotplugExpressModuleB,   //Hotplug Type
                                        0,      // 0 = No Gpio Descriptor
                                        0,      // 0 = No Reset Descriptor
                                        1,      // 1 = Port Active - this is a valid descriptor
                                        0,      // 1 = Master/Slave APU
                                        0,      // 0 = Die number this slot is connected to
                                        0,      // Alternate Slot number
                                        0)      // Primary/secondary for SSD only
      PCIE_HOTPLUG_INITIALIZER_FUNCTION (0,     // Gpio Bit Select : 3
                                         0,     // GPIO Byte Mapping : 3
                                         0,     // GPIO Device Mapping Ext, PCA9555 I2C Slave address 0x23 BIT[4:3]=00
                                         0,     // GPIO Device Mapping, PCA9555 I2C Slave address 0x23 BIT[2:0]=111
                                         1,     // Device Type 1 = 9535
                                         7,     // Bus Segment (No 9545 Present)
                                         0x08)  // Function Mask //AMD fix for NVMe
      PCIE_HOTPLUG_INITIALIZER_NO_RESET()
      PCIE_HOTPLUG_INITIALIZER_NO_GPIO()
  }
};

STATIC NBIO_HOTPLUG_DESC_PROTOCOL mHotplugDescriptorProtocol = {
  AMD_NBIO_HOTPLUG_DESC_VERSION,  ///< revision
  HotplugDescriptor
};

EFI_STATUS
EFIAPI
HotplugDescEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_HANDLE                       Handle;
  EFI_STATUS                      Status;
  AMD_CPM_TABLE_PROTOCOL          *CpmTableProtocolPtr;
  AMD_CPM_DXIO_TOPOLOGY_TABLE     *DxioTopologyTablePtr;
  UINTN                           Index, CpuCount;
  HOTPLUG_DESCRIPTOR              *NbioHotplugDesc;
  DXIO_PORT_DESCRIPTOR            *Port;
  UINT8                           I2cAddress;
  UINT8                           PortNumb;
  UINT8                           PciePort;
  HOTPLUG_DESCRIPTOR    SampleHotplugDescriptor = {
      0,
      HOTPLUG_ENGINE_DATA_INITIALIZER (0,      // Start Lane
                                       3,      // End Lane
                                       0)       // Socket Number
      PCIE_HOTPLUG_INITIALIZER_MAPPING (HotplugExpressModuleB,   //Hotplug Type
                                        0,      // 0 = No Gpio Descriptor
                                        0,      // 0 = No Reset Descriptor
                                        1,      // 1 = Port Active - this is a valid descriptor
                                        0,      // 1 = Master/Slave APU
                                        0,      // 0 = Die number this slot is connected to
                                        0,      // Alternate Slot number
                                        0)      // Primary/secondary for SSD only
      PCIE_HOTPLUG_INITIALIZER_FUNCTION (0,     // Gpio Bit Select : 3
                                         0,     // GPIO Byte Mapping : 3
                                         0,     // GPIO Device Mapping Ext, PCA9555 I2C Slave address 0x23 BIT[4:3]=00
                                         0,     // GPIO Device Mapping, PCA9555 I2C Slave address 0x23 BIT[2:0]=111
                                         1,     // Device Type 1 = 9535
                                         0,     // Bus Segment
                                         0xFD)  // Function Mask //AMD fix for NVMe
      PCIE_HOTPLUG_INITIALIZER_NO_RESET()
      PCIE_HOTPLUG_INITIALIZER_NO_GPIO()
  };

  DEBUG ((EFI_D_INFO,"%a()\n",__FUNCTION__));

  Status = gBS->LocateProtocol (
                  &gAmdCpmTableProtocolGuid,
                  NULL,
                  (VOID**)&CpmTableProtocolPtr
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpuCount = PcdGet8 (PcdAmdNumberOfPhysicalSocket);
  NbioHotplugDesc = AllocateZeroPool(AMD_DXIO_PORT_DESCRIPTOR_SIZE * CpuCount * sizeof (HOTPLUG_DESCRIPTOR));
  mHotplugDescriptorProtocol.NbioHotplugDesc = NbioHotplugDesc;
  CopyMem(NbioHotplugDesc, &SampleHotplugDescriptor, sizeof(HOTPLUG_DESCRIPTOR));
DxioTopologyTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr2 (CpmTableProtocolPtr, CPM_SIGNATURE_DXIO_TOPOLOGY); //kavi+
  for (Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index ++) {
    Port = &DxioTopologyTablePtr->Port[Index];
    DEBUG((EFI_D_ERROR, __FUNCTION__"(), CPU0 LinkHotplug :%d, Lane :%d-%d.\n", Port->Port.LinkHotplug, Port->EngineData.StartLane, Port->EngineData.EndLane));
    if (Port->Port.LinkHotplug == DxioHotplugServerExpress) {
      CopyMem(NbioHotplugDesc, &SampleHotplugDescriptor, sizeof(HOTPLUG_DESCRIPTOR));
      NbioHotplugDesc->Engine.StartLane    = Port->EngineData.StartLane;
      NbioHotplugDesc->Engine.EndLane      = Port->EngineData.EndLane;
      NbioHotplugDesc->Engine.SocketNumber = DxioTopologyTablePtr->SocketId;
      NbioHotplugDesc->Mapping.DieNumber   = DxioTopologyTablePtr->SocketId * 4 + Port->EngineData.StartLane / 32;
      PortNumb = 1;
	  if(PcdGet8(PcdSmileline) == 0){
        switch (Port->EngineData.StartLane) {
          case 0:
            PortNumb = 0;
            I2cAddress = 0x40;
            break;
          case 4:
            PortNumb = 1;
            I2cAddress = 0x40;
            break;
        }
	  }
      NbioHotplugDesc->Function.I2CGpioByteMapping   = PortNumb;
      NbioHotplugDesc->Function.I2CGpioDeviceMapping = (I2cAddress & 0xF) >> 1;
      NbioHotplugDesc->Function.I2CBusSegment = 0;
      DEBUG((EFI_D_ERROR, "CPU:%x,Segment:%x, Lane:(%d,%d), DieNumber:%d, ByteMapping:%x, DeviceMapping:%x\n", 
        DxioTopologyTablePtr->SocketId, NbioHotplugDesc->Function.I2CBusSegment,
        NbioHotplugDesc->Engine.StartLane, NbioHotplugDesc->Engine.EndLane, NbioHotplugDesc->Mapping.DieNumber, 
        NbioHotplugDesc->Function.I2CGpioByteMapping, NbioHotplugDesc->Function.I2CGpioDeviceMapping));
      NbioHotplugDesc ++;
    }
    if(Port->Flags == DESCRIPTOR_TERMINATE_LIST){
      break;
    }
  }
   if (CpuCount == 2 && PcdGet8(PcdSmileline) != 0) {
    DxioTopologyTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr2 (CpmTableProtocolPtr, CPM_SIGNATURE_DXIO_TOPOLOGY_S1);
    for (Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index ++) {
      Port = &DxioTopologyTablePtr->Port[Index];
      DEBUG((EFI_D_ERROR, __FUNCTION__"(), CPU1 LinkHotplug :%d, Lane :%d-%d.\n", Port->Port.LinkHotplug, Port->EngineData.StartLane, Port->EngineData.EndLane));

      if (Port->Port.LinkHotplug == DxioHotplugServerExpress) {
        CopyMem(NbioHotplugDesc, &SampleHotplugDescriptor, sizeof(HOTPLUG_DESCRIPTOR));
        NbioHotplugDesc->Engine.StartLane    = Port->EngineData.StartLane;
        NbioHotplugDesc->Engine.EndLane      = Port->EngineData.EndLane;
		
        NbioHotplugDesc->Engine.SocketNumber = DxioTopologyTablePtr->SocketId;
        PciePort                             = Port->EngineData.StartLane / 32;
        NbioHotplugDesc->Mapping.DieNumber   = DxioTopologyTablePtr->SocketId * 4 + PciePort;

        PortNumb = 1;
        switch (Port->EngineData.StartLane) {
          case 56:
            PortNumb = 0;
            I2cAddress = 0x40;
            NbioHotplugDesc->Function.I2CBusSegment = 1;
            break;
          case 60:
            PortNumb = 1;
            I2cAddress = 0x40;
            NbioHotplugDesc->Function.I2CBusSegment = 1;
            break;
        }
        NbioHotplugDesc->Function.I2CGpioByteMapping   = PortNumb;
        NbioHotplugDesc->Function.I2CGpioDeviceMapping = (I2cAddress & 0xF) >> 1;
        DEBUG((EFI_D_ERROR, "CPU:%x,Segment:%x, Lane:(%d,%d), DieNumber:%d, ByteMapping:%x, DeviceMapping:%x\n", 
          DxioTopologyTablePtr->SocketId, NbioHotplugDesc->Function.I2CBusSegment,
          NbioHotplugDesc->Engine.StartLane, NbioHotplugDesc->Engine.EndLane, NbioHotplugDesc->Mapping.DieNumber, 
          NbioHotplugDesc->Function.I2CGpioByteMapping, NbioHotplugDesc->Function.I2CGpioDeviceMapping));
        NbioHotplugDesc ++;
      }
      if(Port->Flags == DESCRIPTOR_TERMINATE_LIST){
        break;
      }
    }
  }

  if (mHotplugDescriptorProtocol.NbioHotplugDesc != NbioHotplugDesc) {
    NbioHotplugDesc --;
    NbioHotplugDesc->Flags = DESCRIPTOR_TERMINATE_LIST;
  } else {
    mHotplugDescriptorProtocol.NbioHotplugDesc = HotplugDescriptor;
    FreePool(NbioHotplugDesc);
  }

  Handle = NULL;

  // Fixup HOTPLUG_DESCRIPTOR here
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdHotplugDescProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mHotplugDescriptorProtocol
                  );

  return Status;
}
