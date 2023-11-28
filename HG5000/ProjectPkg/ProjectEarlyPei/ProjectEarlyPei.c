

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
#include <AmdCpmDefine.h>
#include <Ppi/AmdCpmTablePpi/AmdCpmTablePpi.h>
#include <SetupVariable.h>
#include <Library/PlatformCommLib.h>
#include <Library/ByoCommLib.h>
#include <PlatformDefinition.h>
#include <Library/PlatformHookLib.h>
#include <Token.h>


EFI_STATUS
EFIAPI
AmdCpmTableCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  ) 
{
  return EFI_SUCCESS;
}



EFI_STATUS
EFIAPI
ByoSetupDataReadyCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  ) 
{
  UINT8              XhciPortDisMap[8];
  UINTN              SizeofBuffer = sizeof(XhciPortDisMap);  
  UINT8              *UsbDisableMap;
  UINTN              UsbDisableMapCount;
  CONST SETUP_DATA   *SetupHob;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  SetupHob = GetSetupDataHobData();

  UsbDisableMap      = PcdGetPtr(PcdXhciPlatformPortDisableMap);
  UsbDisableMapCount = PcdGetSize(PcdXhciPlatformPortDisableMap);
  ASSERT(sizeof(XhciPortDisMap) == UsbDisableMapCount);
  CopyMem(XhciPortDisMap, UsbDisableMap, UsbDisableMapCount);

//
//DieX Bit0 1 -- USB2.0 
//     Bit4 5 -- USB3.0 port
//

#if 0
// CPU0 P[0]
  if(SetupHob->UsbPortDisCtrl1){
    XhciPortDisMap[0] |= BIT0 | BIT4;
  } else {
    XhciPortDisMap[0] &= (UINT8)~(BIT0 | BIT4);
  }

// CPU1 P[0]  
  if(SetupHob->UsbPortDisCtrl2){
    XhciPortDisMap[4] |= BIT0 | BIT4;
  } else {
    XhciPortDisMap[4] &= (UINT8)~(BIT0 | BIT4);
  } 
#endif

//PORT  3.0  2.0
//   0    Y    Y     <X0>
//   1    Y    N
//
//   0    N    Y     <X1>
//   1    N    Y
//
//PORT  3.0  2.0
//   0   Y     Y     <X4>
//   1   N     N
//
//   0   N     N     <X5>
//   1   N     N

//XhciPortDisMap[0] |= BIT1;
//XhciPortDisMap[1] |= BIT4 | BIT5;
//XhciPortDisMap[4] |= BIT1 | BIT5;

  DumpMem8(XhciPortDisMap, sizeof(XhciPortDisMap));
  PcdSetPtr (PcdXhciPlatformPortDisableMap, &SizeofBuffer, (VOID*)XhciPortDisMap);   

  return EFI_SUCCESS;
}




STATIC EFI_PEI_NOTIFY_DESCRIPTOR  gPpiNotifyList[] = {
//  {
//    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
//    &gAmdCpmTablePpiGuid,
//    AmdCpmTableCallBack 
//  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gByoSetupDataReadyPpiGuid,
    ByoSetupDataReadyCallBack 
  },  
  
};




EFI_STATUS
EFIAPI
ProjectEarlyPeiEntry (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;

/*
C0P1  SATA x 8
C1P0  SATA x 8
C1P1  SATA x 8
*/

 // PcdSetBool(PcdSataEnable, FALSE);

// 00110010
 // PcdSet8(PcdSataMultiDieEnable, 0x32);


//  // 0, 1, 5 -> 00100011  
//  PcdSet8(PcdSataMultiDieEnable, 0x23);
//
//  PcdSetBool(PcdXhci0Enable, TRUE);
//  PcdSetBool(PcdXhci1Enable, TRUE);
//  PcdSetBool(PcdXhci2Enable, TRUE);
//  PcdSetBool(PcdXhci3Enable, TRUE);
//
//#if _PCD_VALUE_PcdUartSelection == TKN_UART_SELECTION_CPU_UART0
//  PcdSet8(FchUart0LegacyEnable, 4);          // 0x3F8
//  PcdSet32(FchRTDeviceEnableMap, 0x800);     // legacy Uart0 En
//#elif _PCD_VALUE_PcdUartSelection == TKN_UART_SELECTION_BMC_UART1
//  MmioOr32(LPC_PCI_REG(0x44), BIT6);              // FCH::ITF::LPC::IOPortDecodeEn
//  MmioOr32(LPC_PCI_REG(0x48), BIT0);              // SuperIOConfigurationPortEnable
//#endif
//
//  Status = PeiServicesNotifyPpi(&gPpiNotifyList[0]);
//  ASSERT_EFI_ERROR(Status);
//
  //Reconfig uarts for sol/console redirection in release bios.
  PlatformHookSerialPortInitialize();

  PcdSet32(FchRTDeviceEnableMap, PcdGet32(FchRTDeviceEnableMap) | BIT5);  // enable I2C0
  return Status;  
}  


