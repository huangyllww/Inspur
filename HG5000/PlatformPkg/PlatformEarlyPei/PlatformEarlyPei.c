

#include <PiPei.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <PlatformDefinition.h>
#include <AmdCpu.h>
#include <Fch.h>
#include <Token.h>



EFI_STATUS
EFIAPI
EarlyPeiEntry (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS             Status = EFI_SUCCESS;
  CAR_TOP_DATA           *CarTopData;
  UINT8                  *TopofCar;
  UINT32                 RegEax;
  UINT32                 RegEbx;
  UINT32                 RegEcx;
  UINT32                 RegEdx;
  UINT8                  PhyAddrBits;
  BYO_SYS_CFG_STS_DATA   *SysCfg;
  

  CarTopData = BuildGuidHob(&gCarTopDataHobGuid, sizeof(CAR_TOP_DATA));
  ASSERT(CarTopData != NULL);
  TopofCar = (UINT8*)(UINTN)(PcdGet32(PcdTemporaryRamBase) + PcdGet32(PcdTemporaryRamSize));
  CopyMem(CarTopData, TopofCar - sizeof(CAR_TOP_DATA), sizeof(CAR_TOP_DATA));


  SysCfg = BuildGuidHob(&gByoSysCfgAndStsHobGuid, sizeof(BYO_SYS_CFG_STS_DATA));
  ASSERT(SysCfg != NULL);
  ZeroMem(SysCfg, sizeof(BYO_SYS_CFG_STS_DATA));
  SysCfg->Signature = BYO_SYS_CFG_STS_SIGNATURE;


// CPU HOB
	AsmCpuid(AMD_EXT_CPUID_8, &RegEax, &RegEbx, &RegEcx, &RegEdx);
  PhyAddrBits = (UINT8)(RegEax & 0xFF);
  BuildCpuHob(PhyAddrBits, 16);
  DEBUG((EFI_D_INFO, "PhyAddrBits:%d\n", PhyAddrBits));


// PMx004 [IsaControl] 
//   [1] MmioEn. Read-write. Reset: 1. Init: BIOS,1. 1=Enable ACPI MMIO range (FED8_0000h-FED8_1FFFh).
  IoWrite8(PM_IO_INDEX, FCH_PMIOA_REG04);
  IoWrite8(PM_IO_DATA, IoRead8(PM_IO_DATA) | BIT1);


// PMx000 [DecodeEn]
//   [1] Cf9IoEn. Read-write. Reset: 0. 1=Enable CF9h IO port decoding.
  IoWrite8(PM_IO_INDEX, FCH_PMIOA_REG00);
  IoWrite8(PM_IO_DATA, IoRead8(PM_IO_DATA) | BIT1);

  
//LPC[90] FCH::ITF::LPC::WideIO2
//        [15:0] IOBaseAddress2
//
//LPC[48] FCH::ITF::LPC::IOMemPortDecodeEn
//        [25] WideIO2Enable

#if defined TKN_IPMI_IO_BASE && TKN_IPMI_IO_BASE != 0
  MmioAndThenOr32(LPC_PCI_REG(0x90), ~0xFFFF, TKN_IPMI_IO_BASE);
  MmioOr32(LPC_PCI_REG(0x48), BIT25);
#endif

// For Port 0x61, program timer 1 as refresh timer
  IoWrite8(0x43, 0x54);
  IoWrite8(0x41, 0x12); 

  return Status;  
}  


