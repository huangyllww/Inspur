
/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/


#include "SioPei.h"
#include <Library/BaseLib.h>
#include <Uefi/UefiInternalFormRepresentation.h>
#include <Guid/SioSetupDataStruct.h>
#include <NctReg.h>
// For NCT6126
#ifdef NCT6126
EFI_SIO_TABLE mSioPeiTable[] = {
  //Global register
  { R_SIO_IRQ_CONTROL_BIT, 0xF7, 0x08 },   //irq type control 
  { R_SIO_IRQ_TYPE_SEL,    0xff, R_SIO_IRQ_EDGE }, //Device IRQ TYPE Selection
  { R_SIO_IRQ_TYPE_SEL2,   0xff, R_SIO_IRQ_EDGE }, //Device IRQ TYPE Selection

  { R_SIO_IRQ_POLARITY_SEL,    0xff, R_SIO_IRQ_POLARITY_HIGH }, //IRQ Polarity Selection  
  { R_SIO_IRQ_POLARITY_SEL2,   0xff, R_SIO_IRQ_POLARITY_HIGH }, //IRQ Polarity Selection  

  //{ 0x1B,   0xF7, 0x00 }, 
  //{ 0x24,   0xF9, 0x00 }, 
  { R_DEV_POWER_DOWN,   0xEF, 0xF8 }, //Power down

  //PRT
  { R_SIO_LOGICAL_DEVICE,    0xff, SIO_PARALLEL_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH, 0xff, 0x03 },
  { R_SIO_BASE_ADDRESS_LOW,  0xff, 0x78 },
  { R_SIO_ACTIVATE,          0xff, DEVICE_ENABLE },
  //UARTA
  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_UART_A_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH,        0xff, 0x03 },
  { R_SIO_BASE_ADDRESS_LOW,         0xff, 0xF8 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x04 },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_ENABLE},
  //UARTB
  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_UART_B_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH,        0xff, 0x02 },
  { R_SIO_BASE_ADDRESS_LOW,         0xff, 0xF8 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x03 },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_DISABLE },
  //UARTC
  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_UART_C_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH,        0xff, 0x03 },
  { R_SIO_BASE_ADDRESS_LOW,         0xff, 0xE0 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x04 },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_DISABLE},
  //UARTD
  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_UART_D_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH,        0xff, 0x02 },
  { R_SIO_BASE_ADDRESS_LOW,         0xff, 0xE0 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x03 },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_DISABLE },
  //UARTE
  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_UART_E_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH,        0xff, 0x03 },
  { R_SIO_BASE_ADDRESS_LOW,         0xff, 0xE8 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x04 },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_DISABLE},
  //UARTF
  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_UART_F_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH,        0xff, 0x02 },
  { R_SIO_BASE_ADDRESS_LOW,         0xff, 0xE8 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x03 },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_DISABLE },
  //KBC
  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_KBC_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH,        0xff, 0x00 },
  { R_SIO_BASE_ADDRESS_LOW,         0xff, 0x60 },
  { R_SIO_BASE_ADDRESS_HIGH2,       0xff, 0x00 },
  { R_SIO_BASE_ADDRESS_LOW2,        0xff, 0x64 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x01 },
  { R_KBC_KB_SPECIAL_CFG,           0xff, 0x83 },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_ENABLE },
  //CIR
  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_CIR_DEV_NUM },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_ENABLE },
  //ACPI
  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_ACPI_DEV_NUM },
  { R_POWER_LOSS_CONTROL,           0x60, 0x00 },

  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_EC_SPACE },
  { R_SIO_BASE_ADDRESS_HIGH,        0xFF, (UINT8)(_PCD_VALUE_PcdHwmIoBaseAddress >> 8)},
  { R_SIO_BASE_ADDRESS_LOW,         0xFF, (UINT8)(_PCD_VALUE_PcdHwmIoBaseAddress & 0xFF)},  
  //{ R_SIO_BASE_ADDRESS_HIGH2,       0xFF, (UINT8)(_PCD_VALUE_PcdHwmSmbusBaseAddress >> 8)},
  //{ R_SIO_BASE_ADDRESS_LOW2,        0xFF, (UINT8)(_PCD_VALUE_PcdHwmSmbusBaseAddress & 0xFF)},
  { R_SIO_ACTIVATE,                 0xff, DEVICE_ENABLE },


};
#endif
// For NCT6776
#ifdef NCT6776
EFI_SIO_TABLE mSioPeiTable[] = {
  //Global register
  { R_SIO_IRQ_CONTROL_BIT, 0xF7, 0x08 },   //irq type control 
  { R_SIO_IRQ_TYPE_SEL,    0xff, R_SIO_IRQ_EDGE }, //Device IRQ TYPE Selection
  { R_SIO_IRQ_TYPE_SEL2,   0xff, R_SIO_IRQ_EDGE }, //Device IRQ TYPE Selection

  { R_SIO_IRQ_POLARITY_SEL,    0xff, R_SIO_IRQ_POLARITY_HIGH }, //IRQ Polarity Selection  
  { R_SIO_IRQ_POLARITY_SEL2,   0xff, R_SIO_IRQ_POLARITY_HIGH }, //IRQ Polarity Selection  

  { 0x1B,   0xF7, 0x00 }, 
  { 0x24,   0xF9, 0x00 }, 
  { 0x22,   0xEF, 0x10 }, 

  //LPT
  { R_SIO_LOGICAL_DEVICE,    0xff, SIO_PARALLEL_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH, 0xff, 0x00 },
  { R_SIO_BASE_ADDRESS_LOW,  0xff, 0x00 },
  { R_SIO_ACTIVATE,          0xff, DEVICE_DISABLE },

  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_UART_A_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH,        0xff, 0x03 },
  { R_SIO_BASE_ADDRESS_LOW,         0xff, 0xF8 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x04 },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_ENABLE},

  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_UART_B_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH,        0xff, 0x00 },
  { R_SIO_BASE_ADDRESS_LOW,         0xff, 0x00 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x00 },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_DISABLE },

  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_KBC_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH,        0xff, 0x00 },
  { R_SIO_BASE_ADDRESS_LOW,         0xff, 0x60 },
  { R_SIO_BASE_ADDRESS_HIGH2,       0xff, 0x00 },
  { R_SIO_BASE_ADDRESS_LOW2,        0xff, 0x64 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x01 },
  { R_KBC_KB_SPECIAL_CFG,           0xff, 0x48 },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_DISABLE },

  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_CIR_DEV_NUM },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_DISABLE },

  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_ACPI_DEV_NUM },
  { R_POWER_LOSS_CONTROL,           0x60, 0x00 },

  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_EC_SPACE },
  { R_SIO_BASE_ADDRESS_HIGH,        0xFF, (UINT8)(_PCD_VALUE_PcdHwmIoBaseAddress >> 8)},
  { R_SIO_BASE_ADDRESS_LOW,         0xFF, (UINT8)(_PCD_VALUE_PcdHwmIoBaseAddress & 0xFF)},  
  //{ R_SIO_BASE_ADDRESS_HIGH2,       0xFF, (UINT8)(_PCD_VALUE_PcdHwmSmbusBaseAddress >> 8)},
  //{ R_SIO_BASE_ADDRESS_LOW2,        0xFF, (UINT8)(_PCD_VALUE_PcdHwmSmbusBaseAddress & 0xFF)},
  { R_SIO_ACTIVATE,                 0xff, DEVICE_ENABLE },


};

#endif
// For GP80~GP87 simple IO enable:
// a. Parallel Port Disable: LDN3\Index30h=00h
// b. SMBUS Isolation Disable: Global Index 2Ch<bit7>=1
//
// GP83 D_PSD
EFI_SIO_TABLE mSioGpioTable[] = {
  {0x00, 0x00, 0x00},       // clear
};



// TMPIN Source Selection 1 (TSS1)
// TMPIN Source Selection 2 (TSS2)
EFI_SIO_TABLE mHwmInitTable[] = {
  {R_SMI_MASK3,          0xFF, 0x00},  // bank 0

};






STATIC
EFI_STATUS
EFIAPI
SioEarlyInitialize (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDesc,
  IN       VOID                         *Ppi
  );
  
STATIC EFI_PEI_NOTIFY_DESCRIPTOR   mNotifyDesc = {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    SioEarlyInitialize
};
STATIC
EFI_STATUS
SioEarlyInitialize( 
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  )
{
  EFI_STATUS                        Status;
  UINT8                             Index;
  UINTN                             Size;
  SIO_SETUP_CONFIG                  SioConfig;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *Var2Ppi;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));
    
  EnterSioCfgMode();
  for (Index = 0; Index < ARRAY_SIZE(mSioPeiTable); Index++) {
    SioRegAndThenOr(
      mSioPeiTable[Index].Register, 
      (UINT8)~mSioPeiTable[Index].DataMask, 
      mSioPeiTable[Index].Value
      );
  }
  
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID**)&Var2Ppi
             );
  ASSERT_EFI_ERROR(Status);
  
  Size   = sizeof(SIO_SETUP_CONFIG);  
  Status = Var2Ppi->GetVariable (
                      Var2Ppi,
                      SIO_SETUP_VARIABLE_NAME,
                      &gSioSetupConfigGuid,
                      NULL,
                      &Size,
                      &SioConfig
                      );
  
  if(!EFI_ERROR(Status)){ 
     if(!SioConfig.UartAEn){
        WriteSioReg(R_SIO_LOGICAL_DEVICE, SIO_UART_A_DEV_NUM);
        WriteSioReg(R_SIO_ACTIVATE, DEVICE_DISABLE);
     }
     if(!SioConfig.UartBEn){
        WriteSioReg(R_SIO_LOGICAL_DEVICE, SIO_UART_B_DEV_NUM);
        WriteSioReg(R_SIO_ACTIVATE, DEVICE_DISABLE);
     }
  }

  ExitSioCfgMode();  

  
  return EFI_SUCCESS;
}




EFI_STATUS
SioPeiEntry (
  IN EFI_PEI_FILE_HANDLE           FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
  EFI_STATUS              Status;
  UINT16                  SioChipId;
  DEBUG((DEBUG_INFO, "%a\n", __FUNCTION__));
  SioChipId = GetSioChipId();
  if (SioChipId != NCT6776_CHIP_ID && SioChipId != NCT6126_CHIP_ID) {
    return EFI_UNSUPPORTED;
  }
  Status = (*PeiServices)->NotifyPpi (PeiServices, &mNotifyDesc);
 //SetGpioDefault();
 //SioEarlyInitialize();
 //HwmInit();  
  

  return Status;
}

