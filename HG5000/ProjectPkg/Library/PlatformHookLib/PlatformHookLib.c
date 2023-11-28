
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <PlatformDefinition.h>
#include <Token.h>


EFI_STATUS
EFIAPI
SocSerialPortInitialize (
  VOID
  );


#define SIO_Index_Port 0x2E
#define SIO_Data_Port  (SIO_Index_Port + 1)


EFI_STATUS Bmc2500Uart1Init()
{
  // ;Enter config mode by writting 0xA5 to index port twice
  IoWrite8 (SIO_Index_Port, 0xA5);
  IoWrite8 (SIO_Index_Port, 0xA5);
  
  //;Select LDN=2,uart1
  IoWrite8 (SIO_Index_Port, 0x07);
  IoWrite8 (SIO_Data_Port, 0x02); 

  //;Set base address to 3F8h
  IoWrite8 (SIO_Index_Port, 0x60);
  IoWrite8 (SIO_Data_Port, 0x03);
  IoWrite8 (SIO_Index_Port, 0x61);
  IoWrite8 (SIO_Data_Port, 0xF8);

  //;Set IRQ=04h
  IoWrite8 (SIO_Index_Port, 0x70);
  IoWrite8 (SIO_Data_Port, 0x04);
  
  //;Enable UART1
  IoWrite8 (SIO_Index_Port, 0x30);
  IoWrite8 (SIO_Data_Port, 0x01);
  
  //;Exit config mode by writing 0xAA to Index Port.
  IoWrite8 (SIO_Index_Port, 0xAA);
	
//SetUartHostCtrlLegacyReg(0x3F8);
    
  return EFI_SUCCESS;
}




RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{
#if _PCD_VALUE_PcdUartSelection == TKN_UART_SELECTION_CPU_UART0
  return SocSerialPortInitialize();
#elif _PCD_VALUE_PcdUartSelection == TKN_UART_SELECTION_CPU_UART1
  return SocSerialPortInitialize();
#elif _PCD_VALUE_PcdUartSelection == TKN_UART_SELECTION_BMC_UART1
  MmioOr32(LPC_PCI_REG(0x44), BIT6);              // FCH::ITF::LPC::IOPortDecodeEn
  MmioOr32(LPC_PCI_REG(0x48), BIT0);              // SuperIOConfigurationPortEnable
  return Bmc2500Uart1Init();
#else
  return EFI_UNSUPPORTED;
#endif
}


