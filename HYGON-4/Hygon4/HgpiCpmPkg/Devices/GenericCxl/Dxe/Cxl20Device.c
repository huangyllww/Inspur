/** @file

Cxl 2.0 Device Handler.

**/
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

#include "CxlDeviceDriver.h"
#include <Library/BaseLib.h>
#include <Protocol/HygonCxl20DeviceProtocol.h>

EFI_STATUS 
Cxl20DeviceMailboxSend (
    IN  HYGON_CXL20_DEVICE_PROTOCOL         *This,
    IN  CXL_DEVICE                          *CxlDevice,
    IN  UINT16                               OpCode,                                 
    IN  void                                *InBuffer,
    IN  UINT32                               InSize,
    IN  void                                *OutBuffer,
    IN  UINT32                               OutSize
  );

HYGON_CXL20_DEVICE_PROTOCOL mHygonCxl20DeviceProtocol = {
  HYGON_CXL20_DEVICE_PROTOCOL_REVISION,
  Cxl20DeviceMailboxSend
};

/**
 *---------------------------------------------------------------------------------------
 *  ProbeCxl20Device
 *
 *  Description:
 *     probe CXL 2.0 device information
 *  Parameters:
 *    @param[in]     *CxlDevice      CXL_DEVICE  pointer
 *
 *---------------------------------------------------------------------------------------
 **/
void ProbeCxl20Device (
  IN CXL_DEVICE                *CxlDevice
  )
{
  EFI_PCI_IO_PROTOCOL               *PciIo;
  CXL_REGISTER_BLOCK_INFO            ComponentBlockInfo;
  CXL_REGISTER_BLOCK_INFO            DeviceBlockInfo;

  DEBUG ((DEBUG_INFO, "  Probe CXL 2.0 device : \n"));
  
  PciIo = CxlDevice->StartPciIo;

  CxlDevice->FlexBusDeviceCapPtr = FindPciCxlDvsecCapability (CxlDevice->PciAddr.AddressValue, FLEX_BUS_DEVICE_DVSEC_ID, NULL);
  DEBUG ((DEBUG_INFO, "  - FlexBusDeviceCapPtr is 0x%X \n", CxlDevice->FlexBusDeviceCapPtr));
  
  FindCxl20RegBlock (CxlDevice->PciAddr, COMPONENT_REG_BLOCK, &ComponentBlockInfo);
  CxlDevice->ComponentRegBaseAddr = ComponentBlockInfo.BarAddress;
  DEBUG ((DEBUG_INFO, "  - ComponentRegBaseAddr is 0x%lX \n", CxlDevice->ComponentRegBaseAddr));  
    
  FindCxl20RegBlock (CxlDevice->PciAddr, MEMORY_DEVICE_BLOCK, &DeviceBlockInfo);
  CxlDevice->DeviceRegBaseAddr = DeviceBlockInfo.BarAddress;
  DEBUG ((DEBUG_INFO, "  - DeviceRegBaseAddr is 0x%lX \n", CxlDevice->DeviceRegBaseAddr));  
  
  CxlDevice->DeviceProbed = TRUE;
}

/**
 *---------------------------------------------------------------------------------------
 *  ConfigCxl20DeviceHDMDecoder
 *
 *  Description:
 *     Config CXL 2.0 device HDM decoder
 *  Parameters:
 *    @param[in]     *CxlDevice      CXL_DEVICE  pointer
 *
 *---------------------------------------------------------------------------------------
 **/
void ConfigCxl20DeviceHDMDecoder (
  IN CXL_DEVICE                *CxlDevice
  )
{
  UINT32                          HdmDecoderRegOffset;
  UINT32                          HdmDecoderRegBase;
  UINT32                          Data32;
  CXL_HDM_DECODER_CONTROL         HdmDecoderCtrl;
  CXL_HDM_DECODER_BASE_LOW        HdmDecoder0MemBaseLow;
  CXL_HDM_DECODER_SIZE_LOW        HdmDecoder0MemSizeLow;
  CXL_HDM_DECODER_GLOBAL_CONTROL  HdmGlobalCtrl;
  CXL_HDM_DECODER_CAPABILITY      HdmDecoderCap;
  UINT32                          HdmCounter;
  UINT32                          HdmIndex;
  
  DEBUG ((DEBUG_INFO, "  ConfigHDMDecoder Entry \n"));
  
  HdmDecoderRegOffset = FindCxlCacheMemCapability (CxlDevice->ComponentRegBaseAddr + CXL_CM_OFFSET, CXL_CM_CAP_CAP_ID_HDM);
  if (HdmDecoderRegOffset != CXL_INVALID_DATA) {
    HdmDecoderRegBase = (UINT32) CxlDevice->ComponentRegBaseAddr + CXL_CM_OFFSET + HdmDecoderRegOffset;
    
    HdmDecoderCap.Uint32 = CXL_REG32 (HdmDecoderRegBase, 0x00);
    if (HdmDecoderCap.Bits.DecoderCount == 0) {
      HdmCounter = 1;
    } else {
      HdmCounter = HdmDecoderCap.Bits.DecoderCount * 2;
    }
    DEBUG ((DEBUG_INFO, "  HDM Counter %d \n", HdmCounter));
    
    for (HdmIndex = 0; HdmIndex < HdmCounter; HdmIndex++) {
      if (HdmIndex >= (UINT16)CxlDevice->MemPoolCount) {
        DEBUG ((DEBUG_INFO, "  HDM Index %d is out of DVSEC memory range count %d \n", HdmIndex, CxlDevice->MemPoolCount));
        break;
      }
      
      //HDM Memory Base
      HdmDecoder0MemBaseLow.Uint32 = CXL_REG32 (HdmDecoderRegBase, (0x10 + HdmIndex * 0x20));
      HdmDecoder0MemBaseLow.Bits.MemoryBaseLow = (UINT32) RShiftU64(CxlDevice->MemPool[HdmIndex].Base, 28);
      CXL_REG32_WRITE (HdmDecoderRegBase, (0x10 + HdmIndex * 0x20), HdmDecoder0MemBaseLow.Uint32);
      
      Data32 = (UINT32) RShiftU64(CxlDevice->MemPool[HdmIndex].Base, 32);
      CXL_REG32_WRITE (HdmDecoderRegBase, (0x14 + HdmIndex * 0x20), Data32);
      
      DEBUG ((DEBUG_INFO, "    Config HDM Decoder %d Mem base low = 0x%X \n", HdmIndex, HdmDecoder0MemBaseLow.Uint32));
      DEBUG ((DEBUG_INFO, "    Config HDM Decoder %d Mem base High = 0x%X \n", HdmIndex, Data32));
      
      //HDM Size
      HdmDecoder0MemSizeLow.Uint32 = CXL_REG32 (HdmDecoderRegBase, (0x18 + HdmIndex * 0x20));
      HdmDecoder0MemSizeLow.Bits.MemorySizeLow = (UINT32) RShiftU64(CxlDevice->MemPool[HdmIndex].Size, 28);
      CXL_REG32_WRITE (HdmDecoderRegBase, (0x18 + HdmIndex * 0x20), HdmDecoder0MemSizeLow.Uint32);
      
      Data32 = (UINT32) RShiftU64(CxlDevice->MemPool[HdmIndex].Size, 32);
      CXL_REG32_WRITE (HdmDecoderRegBase, (0x1C + HdmIndex * 0x20), Data32);
      
      DEBUG ((DEBUG_INFO, "    Config HDM Decoder %d Mem size low = 0x%X \n", HdmIndex, HdmDecoder0MemSizeLow.Uint32));
      DEBUG ((DEBUG_INFO, "    Config HDM Decoder %d Mem size High = 0x%X \n", HdmIndex, Data32));
      
      //HDM control
      HdmDecoderCtrl.Uint32 = CXL_REG32 (HdmDecoderRegBase, (0x20 + HdmIndex * 0x20));
      HdmDecoderCtrl.Bits.Commit = 1;
      HdmDecoderCtrl.Bits.TargetDeviceType = 1; //Target is a CXL Type 3 Device
      CXL_REG32_WRITE (HdmDecoderRegBase, (0x20 + HdmIndex * 0x20), HdmDecoderCtrl.Uint32);
      
      DEBUG ((DEBUG_INFO, "    Config HDM Decoder %d control = 0x%X \n", HdmIndex, HdmDecoderCtrl.Uint32));
    }
    
    //Set memory enable = 1
    HdmGlobalCtrl.Uint32 = CXL_REG32 (HdmDecoderRegBase, 0x04);
    HdmGlobalCtrl.Bits.HdmDecoderEnable = 1;
    CXL_REG32_WRITE (HdmDecoderRegBase, 0x04, HdmGlobalCtrl.Uint32);
    
    DEBUG ((DEBUG_INFO, "    Config HDM global control = 0x%X \n", HdmGlobalCtrl.Uint32));
  }
  
  DEBUG ((DEBUG_INFO, "  ConfigHDMDecoder Exit \n"));
}
/**
 *---------------------------------------------------------------------------------------
 *  InitCxl20Device
 *
 *  Description:
 *     Init CXL 2.0 Device
 *  Parameters:
 *    @param[in]     *Device      CXL_DEVICE  pointer
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS 
InitCxl20Device (
  IN   CXL_DEVICE                *CxlDevice
  )
{
  EFI_PCI_IO_PROTOCOL            *PciIo;
  EFI_STATUS                      Status;
  
  DEBUG ((DEBUG_INFO, "  InitCxl20Device Entry \n"));
  
  PciIo = CxlDevice->StartPciIo;
  
  CxlEnableMmioAccess (PciIo);
  
  if (CxlDevice->ComponentRegBaseAddr != CXL_INVALID_DATA) {
    DumpCxlCmComponentRegisters (CxlDevice->ComponentRegBaseAddr + CXL_CM_OFFSET);
  }
  
  CxlGetMemPoolSize (CxlDevice);
  
  Status = CxlEnableMemPool (CxlDevice);
  if (!EFI_ERROR (Status)) {
    //ConfigCxl20DeviceHDMDecoder (CxlDevice);
    Status = CxlLockDvsec (CxlDevice);
  }

  Status = gBS->InstallProtocolInterface(
                      &CxlDevice->Controller,
                      &gHygonCxl20DeviceProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &mHygonCxl20DeviceProtocol);
  
  DEBUG ((DEBUG_INFO, "  InitCxl20Device Exit (status = %r)\n", Status));
  return Status;
}

/**
 *---------------------------------------------------------------------------------------
 *  Cxl20DeviceMailboxSend
 *
 *  Description:
 *     Send mailbox command to CXL 2.0 device
 *  Parameters:
 *    @param[in]     *Device      CXL_DEVICE  pointer
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS 
Cxl20DeviceMailboxSendCmd (
    IN  CXL_DEVICE                          *CxlDevice,
    IN  UINT16                               OpCode,                                 
    IN  void                                *InBuffer,
    IN  UINT32                               InSize,
    IN  void                                *OutBuffer,
    IN  UINT32                               OutSize
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS 
Cxl20DeviceMailboxSend (
    IN  HYGON_CXL20_DEVICE_PROTOCOL         *This,
    IN  CXL_DEVICE                          *CxlDevice,
    IN  UINT16                               OpCode,                                 
    IN  void                                *InBuffer,
    IN  UINT32                               InSize,
    IN  void                                *OutBuffer,
    IN  UINT32                               OutSize
  )
{
  EFI_STATUS  Status;
  
  Status = Cxl20DeviceMailboxSendCmd (CxlDevice, OpCode, InBuffer, InSize, OutBuffer, OutSize);
  
  return Status;
}
