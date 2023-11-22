/** @file

Cxl 1.1 Device Handler.

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
#include <HygonCxlRegOffset.h>
#include <Library/SmnAccessLib.h>
#include <Library/HygonSmnAddressLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>

/**
 *---------------------------------------------------------------------------------------
 *  ProbeCxl11Device
 *
 *  Description:
 *     probe CXL 1.1 device information
 *  Parameters:
 *    @param[in]     *CxlDevice      CXL_DEVICE  pointer
 *
 *---------------------------------------------------------------------------------------
 **/
void ProbeCxl11Device (
  IN CXL_DEVICE                *CxlDevice
  )
{
  EFI_STATUS                      Status;
  
  CxlDevice->DeviceProbed = FALSE;
  
  DEBUG ((DEBUG_INFO, "  Probe CXL 1.1 device : \n"));
  CxlDevice->FlexBusDeviceCapPtr = FindPciCxlDvsecCapability (CxlDevice->PciAddr.AddressValue, FLEX_BUS_DEVICE_DVSEC_ID, NULL);
  DEBUG ((DEBUG_INFO, "  - FlexBusDeviceCapPtr is 0x%X \n", CxlDevice->FlexBusDeviceCapPtr));
  
  if (gCxl11ServicesProtocol != NULL) {
    Status = gCxl11ServicesProtocol->Cxl11GetRootPortInfoByBus (
                 gCxl11ServicesProtocol,
                 (UINT8) CxlDevice->PciAddr.Address.Bus,
                 &CxlDevice->Cxl11PortInfo);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Cxl11GetRootPortInfoByBus fail with status %r \n", Status));
      return;
    }
    CxlDevice->DeviceProbed = TRUE;
  }
}

/**
 *---------------------------------------------------------------------------------------
 *  ProgramCxl11MmioResource
 *
 *  Description:
 *     Config CXL 1.1 MMIO resource
 *  Parameters:
 *    @param[in]     *Device      CXL_DEVICE  pointer
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS 
ProgramCxl11MmioResource (
    IN  CXL_DEVICE               *CxlDevice
  )
{
  EFI_STATUS                      Status;
  PCI_ADDR                        EndPointAddress;
  UINT32                          RegOffset;
  EFI_PCI_IO_PROTOCOL            *PciIo;
  PCIE_BAR_STRUCT                 Bar32;
  UINT32                          Mmio32Base;
  UINT32                          Mmio32Size;
  UINT32                          Mmio32Limit;
  UINT64                          Mmio64Base;
  UINT64                          Mmio64Size; 
  UINT32                          Value;
  UINT64                          Value64;
  MEM_BASE_LIMIT_PCIERCCFG_UNION   MemBaseLimit;
  PREF_BASE_LIMIT_PCIERCCFG_UNION  PrefBaseLimit;
  UINT64                           MemP64Limit;
  UINT64                           UspRcrbMemBar0Addr;
  UINT32                           SmnAddr;
  FABRIC_TARGET                    MmioTarget;
  FABRIC_MMIO_ATTRIBUTE            MmioAttr;
  UINT64                           CxlMmioBase;
  UINT64                           CxlMmioSize;
  
  PciIo = CxlDevice->StartPciIo;
  EndPointAddress = CxlDevice->PciAddr;
      
  Mmio32Base = 0;
  Mmio32Size = 0;
  Mmio64Base = 0;
  Mmio64Size = 0;
  UspRcrbMemBar0Addr = 0;
  
  // Read RCiEP BARs and aggregate requests
  for (RegOffset = 0x10; RegOffset <= 0x30; RegOffset += 4) {
    if ((RegOffset > 0x24) && (RegOffset < 0x30)) {
      //Ignore offset 0x28, 0x2C 
      continue;
    }
    EndPointAddress.Address.Register = RegOffset;
    
    Status = PciIo->Pci.Read (
          PciIo,
          EfiPciIoWidthUint32,
          RegOffset,
          1,
          (VOID *)&Bar32.Value
          );
    
    if (Bar32.Field.MemIo == 0) {
      //MMIO resource
      if (Bar32.Field.MemType == MEM_32_BAR) {
        //Calc MMIO32 base
        Value = Bar32.Value & 0xFFFFFFF0;
        DEBUG ((DEBUG_INFO, "  MMIO32 BAR Reg 0x%x, base 0x%x, ", RegOffset, Value));
        if ((Mmio32Base == 0) || (Value < Mmio32Base)) {
          Mmio32Base = Value;
        }
        
        //Calc MMIO32 size
        Value = GetMmio32BarSize (EndPointAddress.AddressValue);
        DEBUG ((DEBUG_INFO, "  size 0x%x \n", Value));
        Mmio32Size += Value;
        
      } else if (Bar32.Field.MemType == MEM_64_BAR) {
        //Calc MMIO64 base
        Status = PciIo->Pci.Read (
              PciIo,
              EfiPciIoWidthUint32,
              RegOffset + 4,
              1,
              (VOID *)&Value
              );
        Value64 = LShiftU64 ((UINT64) Value, 32);
        Value64 += (UINT64) (Bar32.Value & 0xFFFFFFF0);
        DEBUG ((DEBUG_INFO, "  MMIO64 BAR Reg 0x%x, base 0x%lx, ", RegOffset, Value64));
        if ((Mmio64Base == 0) || (Value64 < Mmio64Base)) {
          Mmio64Base = Value64;
        }
        
        //Calc MMIO64 size
        Value64 = GetMmio64BarSize (EndPointAddress.AddressValue);
        DEBUG ((DEBUG_INFO, "  size 0x%lx \n", Value64));
        Mmio64Size += Value64;
        RegOffset += 4;
      }
    }
  }
  
  DEBUG ((DEBUG_INFO, "  RCiEP Total MMIO32 base 0x%x, size 0x%x\n", Mmio32Base, Mmio32Size));
  DEBUG ((DEBUG_INFO, "  RCiEP Total MMIO64 base 0x%lx, size 0x%lx\n", Mmio64Base, Mmio64Size));
  
  if (Mmio32Size != 0) {
    UspRcrbMemBar0Addr = (UINT64)(Mmio32Base + Mmio32Size);
    if ((UspRcrbMemBar0Addr & CXL11_MEMBAR0_ALIGN) != 0) {
      //CXL 1.1 USP membar0 Align
      UspRcrbMemBar0Addr = (UspRcrbMemBar0Addr + CXL11_MEMBAR0_ALIGN) & ~((UINT64)CXL11_MEMBAR0_ALIGN);
    }

    Mmio32Limit = (UINT32)UspRcrbMemBar0Addr + CXL11_MEMBAR0_SIZE - 1;

    if ((Mmio32Limit + 1 - Mmio32Base - Mmio32Size) > PcdGet32 (PcdCxl11ReservedMmioSize)) {
      DEBUG ((DEBUG_INFO, "  PcdCxl11ReservedMmioSize is too small, need add 0x%X \n", Mmio32Limit + 1 - Mmio32Base - Mmio32Size - PcdGet32 (PcdCxl11ReservedMmioSize)));
      UspRcrbMemBar0Addr = 0;
      Mmio32Limit = Mmio32Base + Mmio32Size - 1;
    }
  } else {
    //Allocate MMIO for USP component registers
    MmioTarget.TgtType = TARGET_PCI_BUS;
    MmioTarget.SocketNum = 0;
    MmioTarget.DieNum = 0;
    MmioTarget.RbNum = 0;
    MmioTarget.PciBusNum = (UINT16) CxlDevice->PciAddr.Address.Bus;
    
    MmioAttr.ReadEnable = 1;
    MmioAttr.WriteEnable = 1;
    MmioAttr.NonPosted = 0;
    MmioAttr.MmioType = MMIO_BELOW_4G;
      
    CxlMmioSize = CXL11_MEMBAR0_SIZE;
    Status = FabricAllocateMmio (&CxlMmioBase, &CxlMmioSize, ALIGN_64K, MmioTarget, &MmioAttr);
    if (!EFI_ERROR (Status)) {
      Mmio32Base = (UINT32) CxlMmioBase;
      Mmio32Limit = Mmio32Base + CXL11_MEMBAR0_SIZE - 1;
      UspRcrbMemBar0Addr = Mmio32Base;
    }
  }

  DEBUG ((DEBUG_INFO, "  USP MEMBAR0 = 0x%lx \n", UspRcrbMemBar0Addr));
  DEBUG ((DEBUG_INFO, "  DSP MMIO32 Range : 0x%x ~ 0x%x \n", Mmio32Base, Mmio32Limit));
  
  //Program MMIO32 range for DSP
  if (Mmio32Base != 0) {
    MemBaseLimit.Value = 0;
    MemBaseLimit.Field.MEM_BASE_31_20 = Mmio32Base >> 20;
    MemBaseLimit.Field.MEM_LIMIT_31_20 = Mmio32Limit >> 20;
    CXL_REG32_WRITE (CxlDevice->Cxl11PortInfo.DspRcrb, PCICFG_SPACE_MEMORY_BASE_OFFSET, MemBaseLimit.Value);
    
    SmnAddr = NBIO_SPACE2 (CxlDevice->Cxl11PortInfo.PhysicalDieId, CxlDevice->Cxl11PortInfo.RbIndex, NBIO0_IOHC_SHADOW_CXL_MEM_BASE_LIMIT);
    SmnRegisterWriteBySocket (CxlDevice->Cxl11PortInfo.SocketId, SmnAddr, AccessWidth32, &MemBaseLimit.Value);
  }
  
  //Program MMIO64 range for DSP
  if (Mmio64Size != 0) {
    //Program MMIO64 base/limit Reg
    MemP64Limit = Mmio64Base + Mmio64Size - 1;
    PrefBaseLimit.Value = 0;
    PrefBaseLimit.Field.PREF_MEM_BASE_TYPE = 1;
    PrefBaseLimit.Field.PREF_MEM_LIMIT_TYPE = 1;
    PrefBaseLimit.Field.PREF_MEM_BASE_31_20 = (UINT32) RShiftU64 (Mmio64Base, 20) & 0xFFF;
    PrefBaseLimit.Field.PREF_MEM_LIMIT_31_20 = (UINT32) RShiftU64 (MemP64Limit, 20) & 0xFFF;
    CXL_REG32_WRITE (CxlDevice->Cxl11PortInfo.DspRcrb, PCICFG_SPACE_PREFETCH_BASE_LOW_OFFSET, PrefBaseLimit.Value);

    SmnAddr = NBIO_SPACE2 (CxlDevice->Cxl11PortInfo.PhysicalDieId, CxlDevice->Cxl11PortInfo.RbIndex, NBIO0_IOHC_SHADOW_CXL_PREF_MEM_BASE_LIMIT);
    SmnRegisterWriteBySocket (CxlDevice->Cxl11PortInfo.SocketId, SmnAddr, AccessWidth32, &PrefBaseLimit.Value);

    //Program MMIO64 base upper Reg
    Value = (UINT32) RShiftU64 (Mmio64Base, 32);
    CXL_REG32_WRITE (CxlDevice->Cxl11PortInfo.DspRcrb, PCICFG_SPACE_PREFETCH_LOW_OFFSET, Value);

    SmnAddr = NBIO_SPACE2 (CxlDevice->Cxl11PortInfo.PhysicalDieId, CxlDevice->Cxl11PortInfo.RbIndex, NBIO0_IOHC_SHADOW_CXL_PREF_MEM_BASE_UPPER);
    SmnRegisterWriteBySocket (CxlDevice->Cxl11PortInfo.SocketId, SmnAddr, AccessWidth32, &Value);

    //Program MMIO64 limit upper Reg
    Value = (UINT32) RShiftU64 (MemP64Limit, 32);
    CXL_REG32_WRITE (CxlDevice->Cxl11PortInfo.DspRcrb, PCICFG_SPACE_PREFETCH_HIGH_OFFSET, Value);

    SmnAddr = NBIO_SPACE2 (CxlDevice->Cxl11PortInfo.PhysicalDieId, CxlDevice->Cxl11PortInfo.RbIndex, NBIO0_IOHC_SHADOW_CXL_PREF_MEM_LIMIT_UPPER);
    SmnRegisterWriteBySocket (CxlDevice->Cxl11PortInfo.SocketId, SmnAddr, AccessWidth32, &Value);
  }
  
  CxlDevice->Cxl11PortInfo.UspMemBar0 = (UINT32)UspRcrbMemBar0Addr;
  CXL_REG32_WRITE (CxlDevice->Cxl11PortInfo.UspRcrb, CXL_MEMBAR0_LOW_OFFSET, (UINT32)(UspRcrbMemBar0Addr & 0xFFFFFFFF));
  CXL_REG32_WRITE (CxlDevice->Cxl11PortInfo.UspRcrb, CXL_MEMBAR0_HIGH_OFFSET, 0);

  DEBUG ((DEBUG_INFO, "USP RCRB offset 0x10 = 0x%x \n", CXL_REG32 (CxlDevice->Cxl11PortInfo.UspRcrb, CXL_MEMBAR0_LOW_OFFSET)));

  return Status;
}
/**
 *---------------------------------------------------------------------------------------
 *  ProbeCxl11Device
 *
 *  Description:
 *     Init CXL 1.1 Device
 *  Parameters:
 *    @param[in]     *CxlDevice      CXL_DEVICE  pointer
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS 
InitCxl11Device (
  IN CXL_DEVICE                *CxlDevice
  )
{
  EFI_STATUS                      Status;
  EFI_PCI_IO_PROTOCOL            *PciIo;
  
  DEBUG ((DEBUG_INFO, "  InitCxl11Device Entry \n"));  
  if (CxlDevice->DeviceProbed == FALSE) {
    return EFI_DEVICE_ERROR;
  }
  
  ProgramCxl11MmioResource (CxlDevice);
  
  PciIo = CxlDevice->StartPciIo;
  CxlEnableMmioAccess (PciIo);
  
  DEBUG ((DEBUG_INFO, "DSP memory BAR0 addr 0x%x, offset 0x1000 value = 0x%x \n", CxlDevice->Cxl11PortInfo.DspMemBar0, CXL_REG32 (CxlDevice->Cxl11PortInfo.DspMemBar0, 0x1000)));
  DEBUG ((DEBUG_INFO, "USP memory BAR0 addr 0x%x, offset 0x1000 value = 0x%x \n", CxlDevice->Cxl11PortInfo.UspMemBar0, CXL_REG32 (CxlDevice->Cxl11PortInfo.UspMemBar0, 0x1000)));
  
  if (gCxl11ServicesProtocol != NULL) {
    Status = gCxl11ServicesProtocol->Cxl11ConfigureRootPort (
               gCxl11ServicesProtocol, 
               CxlDevice->PciAddr);
  }
  
  CxlGetMemPoolSize (CxlDevice);
  
  Status = CxlEnableMemPool (CxlDevice);
  if (!EFI_ERROR (Status)) {
    Status = CxlLockDvsec (CxlDevice);
  }
  
  DEBUG ((DEBUG_INFO, "  InitCxl11Device Exit \n"));
  return Status;
}
