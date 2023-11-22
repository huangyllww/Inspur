
#include <Uefi.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ByoHygonCommLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Fch.h>


VOID LibHygonEarlyDecode(UINT16 IpmiIoBase, UINT16 PmBase)
{
// D0BF3x090 [Wide IO 2] (WideIO2) 
//   [15:0] IOBaseAddress2: IO Base Address 2. Read-write. Reset: 0. 
//          16-bit PCI IO base address for wide generic IO port range. 
//          This function is enabled by FCH::ITF::LPC::IOMemPortDecodeEn[WideIO2Enable]. 
//          If FCH::ITF::LPC::AlternativeWideIORngEn[AlternativeWideIO2RangeEnable] == 1, 
//          the range is 16 bytes; else, the ranges is 512 bytes. 
// D0BF3x048 IOMemPortDecodeEn
//   [25] WideIO2Enable. Read-write. Reset: 0. 
//        1=Enable the IO range. Port enable for wide generic IO port 2 defined by 
//        FCH::ITF::LPC::WideIO2[IOBaseAddress2]. 
// D0BF3x074 [Alternative Wide IO Range Enable] (AlternativeWideIORngEn)
//   [3] AlternativeWideIO2RangeEnable: Alternative Wide IO 2 Range Enable. 
//       Read-write. Reset: 0. 
//       This bit is similar to bit [AlternativeWideIO0RangeEnable], but it applies to the 
//       IO range defined by FCH::ITF::LPC::WideIO2[IOBaseAddress2]
//       1=The range is 16 bytes
//       0=The range is 512 bytes

  if(IpmiIoBase){
    MmioAndThenOr32(LPC_PCI_REG(0x90), (UINT32)~0xFFFF, IpmiIoBase & 0xFFF0);
    MmioOr32(LPC_PCI_REG(0x48), BIT25);
    MmioOr32(LPC_PCI_REG(0x74), BIT3);
  }   

// PMx004 [IsaControl]
//   [1] MmioEn. Read-write. Reset: 1. Init: BIOS,1. 
//       1=Enable ACPI MMIO range (FED8_0000h-FED8_1FFFh).
  IoWrite8(PM_IO_INDEX, FCH_PMIOA_REG04);
  IoWrite8(PM_IO_DATA, IoRead8(PM_IO_DATA) | BIT1);

// PMx000 [DecodeEn]
//   [1] Cf9IoEn. Read-write. Reset: 0. 
//       1=Enable CF9h IO port decoding.
  IoWrite8(PM_IO_INDEX, FCH_PMIOA_REG00);
  IoWrite8(PM_IO_DATA, IoRead8(PM_IO_DATA) | BIT1); 

// PMx060 [AcpiPm1EvtBlk] (AcpiPm1EvtBlk) 
//   [15:2]  AcpiPm1EvtBlk. Read-write. Reset: Cold,0. 
//           Specifies the 16-bit IO range base address[15:2] of the ACPI power 
//           management event block. 
  MmioWrite16(ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG60, PmBase);  
}


// D0BF3x07C [TPM]
//   [0] Tpm12En. Read-write. Reset: 1. 
//       1=Enable decoding of TPM cycles defined in TPM 1.2 spec
//   [12] TpmPfetchEn. Read-write. Reset: 0. 
//        0=Disable TPM burst read. 
//        1=Enable TPM burst read. 
//   [13] TpmBufferEn. Read-write. Reset: 0. 
//        0=Disable TPM buffer. 
//        1=Enable TPM buffer.
//
// D0BF3x0A0 [SPI Base_Addr] (SPIBaseAddr) 
//   [0]  AltSpiCSEnable. Read-write. Reset: 0. 
//        Alternative SPI CS
//   [3]  RouteTpm2Spi
VOID LibTpmDecode(UINT8 TpmSelect)
{
  switch(TpmSelect){
    case 0:
    default:
      DEBUG((EFI_D_INFO, "Disable Tpm Decode.\n"));  
      MmioAnd32(LPC_PCI_REG(0x7C), (UINT32)~BIT0);        
      break;

    case 2:
      MmioOr32(LPC_PCI_REG(0x7C), BIT0 | BIT12 | BIT13);
      MmioOr32(LPC_PCI_REG(0xA0), BIT0 | BIT3);
      break;

    case 3:
      MmioOr32(LPC_PCI_REG(0x7C), BIT0 | BIT12 | BIT13);
      break;
  }    
}


UINT8 LibGetCpuPhyAddrBits() 
{
  UINT32  Eax;

  AsmCpuid(0x80000008, &Eax, NULL, NULL, NULL);
  return (UINT8)(Eax & 0xFF);  
}


UINT8 LibGetCpuModel()
{
  UINT32         CpuModel;
  
  CpuModel = GetHygonSocModel();
  if(CpuModel == HYGON_EX_CPU){
    return CPU_MODEL_749X;
  } else if(CpuModel == HYGON_GX_CPU){
    return CPU_MODEL_748X;
  } else {
    return CPU_MODEL_UNKNOWN;
  }
}



UINT8 LibGetAcpiWakeUpSrc(UINT16 PmBase)
{
  UINT8      WakeUpSrc;
  UINT16     En;
  UINT16     Sts;
  UINT32     PmResetSts;

  WakeUpSrc = WAK_TYPE_NONE;

  Sts = IoRead16(PmBase + PMIO_STS_REG);
  En  = IoRead16(PmBase + PMIO_PM_EN);
  PmResetSts = MmioRead32(FCH_PM_MMIO_BASE + PM_S5_RESET_STATUS);
  DEBUG((EFI_D_INFO, "Sts:%X En:%X Reset:%x\n", Sts, En, PmResetSts));	
  
  if(!(Sts & PMIO_STS_WAK)){
    if(PmResetSts & (RESET_STATUS_SYNC_FLOOD | RESET_STATUS_DO_RESET | RESET_STATUS_USER_RST)){
      WakeUpSrc = WAK_TYPE_SYS_RESET;
    }    
    goto ProcExit;		
  }		
  if(Sts & PMIO_STS_PWBTNOR){
    WakeUpSrc = WAK_TYPE_PBOR;
    goto ProcExit;
  }
  if(Sts & PMIO_STS_PWRBTN){
    WakeUpSrc = WAK_TYPE_POWERBUTTON;
    goto ProcExit;
  }
  if(Sts & En & PMIO_STS_RTC){
    WakeUpSrc = WAK_TYPE_RTC;
    goto ProcExit;
  }
  if((Sts & PMIO_STS_PCIEW) && !(En & PMIO_PM_DIS_PCIEW)){
    WakeUpSrc = WAK_TYPE_PCIE;
    goto ProcExit;    
  }	

  WakeUpSrc = WAK_TYPE_UNKNOWN;

ProcExit:
  DEBUG((EFI_D_INFO, "WakeUpSrc:%d\n", WakeUpSrc));	
  return WakeUpSrc;
}



UINT8 CmosRead(UINT8 Address)
{
  IoWrite8(0x70, (UINT8)(Address|(UINT8)(IoRead8(0x70) & 0x80)));
  return IoRead8(0x71);
}

VOID CmosWrite(UINT8 Address, UINT8 Data)
{
  IoWrite8(0x70, (UINT8)(Address|(UINT8)(IoRead8(0x70) & 0x80)));
  IoWrite8(0x71, Data);
}

// after CMOS battery lost, data in cmos will be changed to 00.
BOOLEAN
LibCheckCmosBad (
  VOID
  )
{
  UINT8    Data1, Data2;

  Data1 = CmosRead(0x2e);
  Data2 = CmosRead(0x2f);

  if(Data1 == 0 && Data2 == 0){
    return TRUE;
  }else{
    return FALSE;
  }
}


VOID
LibClearCmosBad (
  VOID
  )
{
  CmosWrite(0x2e, 0x55);
  CmosWrite(0x2f, 0xAA);
}




/*
[28]  UART0_1p843MCLK_EN. Read-write. Reset: 0. 
  0=use 48MHz as UART baud rate clock. 
  1=enable UART legacy mode to use 1.843MHz as baud rate clock. 

[29]  UART1_1p843MCLK_EN. Read-write. Reset: 0. 
  0=use 48MHz as UART baud rate clock. 
  1=enable UART legacy mode to use 1.843MHz as baud rate clock.

// [0] 2e8-2ef, 0 - disable, 1 - enable
// [1] 2f8-2ff, 0 - disable, 1 - enable
// [2] 3e8-3ff, 0 - disable, 1 - enable
// [3] 3f8-3ff, 0 - disable, 1 - enable
// [09:08] which uart uses 2E8-2EF,  0 - UART0, 1 - UART1, 2 - UART2, 3 - UART3
// [11:10]                 2F8-2FF
// [13:12]                 3E8-3EF
// [15:14]                 3F8-3FF
*/
EFI_STATUS
LibSocSerialPortInitialize (
    UINT8  UartIndex
  )
{
  switch(UartIndex){
    case 0:
      MmioOr32(SMBUS_PCI_REG(0xFC), BIT28);
      MmioWrite8 (FCH_IOMUXx89_UART0_RTS_L_EGPIO137, 0x00);
      MmioWrite8 (FCH_IOMUXx8A_UART0_TXD_EGPIO138, 0x00);
      MmioWrite16(FCH_AL2AHBx20_LEGACY_UART_IO_ENABLE, BIT3);
      return EFI_SUCCESS;

    case 1:
      MmioOr32(SMBUS_PCI_REG(0xFC), BIT29);
      MmioWrite8 (FCH_IOMUXx8E_UART1_RTS_L_EGPIO142, 0x00);
      MmioWrite8 (FCH_IOMUXx8F_UART1_TXD_EGPIO143, 0x00);
      MmioWrite16(FCH_AL2AHBx20_LEGACY_UART_IO_ENABLE, BIT1|BIT10);
      return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}


// SerialPortEnable0 (0x3f8)
// SuperIOConfigurationPortEnable(2e/2f)
VOID LibEnableLpcSioUart0Decode()
{
  MmioOr32(LPC_PCI_REG(0x44), BIT6);              // FCH::ITF::LPC::IOPortDecodeEn
  MmioOr32(LPC_PCI_REG(0x48), BIT0);              // SuperIOConfigurationPortEnable
}



