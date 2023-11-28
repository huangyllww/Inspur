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

#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Protocol/I2cMaster.h>
#include <DwI2cRegs.h>
#include "AmdI2cMasterDxe.h"

extern  EFI_BOOT_SERVICES       *gBS;

//
// Driver Global Data
//

/*********************************************************************************
 * Name: AmdI2cMasterDxeInit
 *
 * Description
 *   Entry point of the AMD I2cMaster DXE driver
 *   Perform the configuration init, resource reservation, early post init
 *   and install all the supported protocol
 *
 * Input
 *   ImageHandle : EFI Image Handle for the DXE driver
 *   SystemTable : pointer to the EFI system table
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
AmdI2cMasterDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  EFI_HANDLE          Handle;
  AMD_I2C_MASTER_PRIVATE *I2cPrivate;
  EFI_I2C_CONTROLLER_CAPABILITIES *I2cControllerCapabilities;
  UINT32 Index;

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (AMD_I2C_MASTER_PRIVATE), &I2cPrivate);
  if (EFI_ERROR (Status)) {
    return Status;
  } else {
    //clear instances content
    gBS->SetMem (I2cPrivate, sizeof (AMD_I2C_MASTER_PRIVATE), 0);
  }

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (EFI_I2C_CONTROLLER_CAPABILITIES), &I2cControllerCapabilities);
  if (EFI_ERROR (Status)) {
    return Status;
  } else {
    //clear instances content
    gBS->SetMem (I2cControllerCapabilities, sizeof (EFI_I2C_CONTROLLER_CAPABILITIES), 0);
  }

  //Init I2cCotroller capabilities data structure
  I2cControllerCapabilities->StructureSizeInBytes = sizeof (EFI_I2C_CONTROLLER_CAPABILITIES);
  I2cControllerCapabilities->MaximumReceiveBytes = 0xFFFFFFFF;
  I2cControllerCapabilities->MaximumTransmitBytes = 0xFFFFFFFF;
  I2cControllerCapabilities->MaximumTotalBytes = 0xFFFFFFFF;

  Handle = ImageHandle;

  for (Index = 0; Index < I2C_CONTROLLER_COUNT; Index++) {
//   I2cPrivate->I2cMaster[Index].Handle                           = Handle;
    I2cPrivate->I2cMaster[Index].I2cSdpAddress                           = I2cSdpAddressList[Index];
    I2cPrivate->I2cMaster[Index].I2cController.SetBusFrequency           = SetBusFrequency;
    I2cPrivate->I2cMaster[Index].I2cController.Reset                     = Reset;
    I2cPrivate->I2cMaster[Index].I2cController.StartRequest              = StartRequest;
    I2cPrivate->I2cMaster[Index].I2cController.I2cControllerCapabilities = I2cControllerCapabilities;
    I2cInit (&I2cPrivate->I2cMaster[Index]);
  }

  // Install Protocol
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &I2cPrivate->I2cMaster[0].Handle,
                  &gEfiI2cMasterProtocolGuid,
                  &I2cPrivate->I2cMaster[0],
                  NULL
                  );

  return (Status);
}

UINT32
STATIC
i2c_dw_scl_hcnt (
  UINT32 ic_clk,
  UINT32 tSYMBOL,
  UINT32 tf,
  UINT32 cond,
  UINT32 offset
  )
{
  if (cond) {
    //
    // Conditional expression:
    //
    //   IC_[FS]S_SCL_HCNT + (1+4+3) >= IC_CLK * tHIGH
    //
    // This is based on the DW manuals, and representing an
    // ideal configuration.  The resulting I2cBus speed will
    // be faster than any of the others.
    //
    // If your hardware is free from tHD;STA issue, try this one.
    //
    return (ic_clk * tSYMBOL + 5000) / 10000 - 8 + offset;
  } else {
    //
    // Conditional expression:
    //
    //   IC_[FS]S_SCL_HCNT + 3 >= IC_CLK * (tHD;STA + tf)
    //
    // This is just experimental rule; the tHD;STA period turned
    // out to be proportinal to (_HCNT + 3).  With this setting,
    // we could meet both tHIGH and tHD;STA timing specs.
    //
    // If unsure, you'd better to take this alternative.
    //
    // The reason why we need to take into account "tf" here,
    // is the same as described in i2c_dw_scl_lcnt ().
    //
    return (ic_clk * (tSYMBOL + tf) + 5000) / 10000 - 3 + offset;
  }
}

UINT32
STATIC
i2c_dw_scl_lcnt (
    UINT32 ic_clk,
    UINT32 tLOW,
    UINT32 tf,
    UINT32 offset
    )
{
  //
  // Conditional expression:
  //
  //   IC_[FS]S_SCL_LCNT + 1 >= IC_CLK * (tLOW + tf)
  //
  // DW I2cCore starts counting the SCL CNTs for the LOW period
  // of the SCL clock (tLOW) as soon as it pulls the SCL line.
  // We need to take into account tf to meet the tLOW timing spec.
  // Default tf value should be 0.3 us, for safety.
  //
  return ((ic_clk * (tLOW + tf) + 5000) / 10000) - 1 + offset;
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function use for change AGESA configuration parameter PCD for multiplatform support.
 *
 * It will prepare AGESA configuration group list and return to SOC driver.
 * then the SOC driver will update the PCD value based on the list provide by this function.
 *
 * @param[in]   PeiServices
 * @param[in]   SocId
 * @param[out]  PlalformGroupGuid
 *
 */
EFI_STATUS
EFIAPI
SetBusFrequency (
  IN CONST EFI_I2C_MASTER_PROTOCOL   *This,
  IN UINTN                    *BusClockHertz
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  I2CMASTER_PRIVATE *Private;
  UINT32 settings;
  UINT32 Base;
  UINT32 hcnt;
  UINT32 lcnt;

  settings = 0;

  Private = (I2CMASTER_PRIVATE*)This;
  Base = Private->I2cSdpAddress;

  // Disable the interface
  MmioWrite32 (Base + DW_IC_ENABLE, 0);
  if (I2cDwWaitI2cEnable (Base, CHECK_IC_EN_HIGH)) {
    return EFI_NOT_READY;
  }

  settings |= DW_I2C_CON_MASTER_MODE | DW_I2C_CON_IC_SLAVE_DISABLE;

  if (FS_SPEED < *BusClockHertz) {
    settings |= DW_I2C_CON_SPEED_HS;
    *BusClockHertz = FS_SPEED;        //Return actually clock setting
//    *BusClockHertz = HS_SPEED;
  }
  if (SS_SPEED < *BusClockHertz) {
    settings |= DW_I2C_CON_SPEED_FS;
    *BusClockHertz = FS_SPEED;        //Return actually clock setting
  } else {
    settings |= DW_I2C_CON_SPEED_SS;
    *BusClockHertz = SS_SPEED;        //Return actually clock setting
  }

  settings |= DW_I2C_CON_IC_RESTART_EN;

  MmioWrite32 (Base + DW_IC_CON, settings);

  // Setup spike suppression for SS and FS at 50ns
  MmioWrite32 (Base + DW_IC_FS_SPKLEN, configI2C_FS_GLITCH_FILTER);

  // Setup spike suppression for HS at 10ns
  MmioWrite32 (Base + DW_IC_FS_SPKLEN, configI2C_FS_GLITCH_FILTER);

  // Standard-mode 100Khz
  hcnt = i2c_dw_scl_hcnt (configI2C_CLOCK_HZ,
                          40,     // tHD;STA = tHIGH = 4.0 us
                          3,      // tf = 0.3 us
                          0,      // 0: DW default, 1: Ideal
                          0);     // No offset
  lcnt = i2c_dw_scl_lcnt (configI2C_CLOCK_HZ,
                          47,     // tLOW = 4.7 us
                          3,      // tf = 0.3 us
                          0);     // No offset

  MmioWrite32 (Base + DW_IC_SS_SCL_HCNT, hcnt); // std speed high, 4us
  MmioWrite32 (Base + DW_IC_SS_SCL_LCNT, lcnt); // std speed low, 4.7us

  DEBUG ((EFI_D_ERROR, "Standard-mode HCNT:LCNT = %d:%d\n", hcnt, lcnt));

  // Fast-mode 400Khz
  hcnt = i2c_dw_scl_hcnt (configI2C_CLOCK_HZ,
                          6,      // tHD;STA = tHIGH = 0.6 us
                          3,      // tf = 0.3 us
                          0,        // 0: DW default, 1: Ideal
                          0);       // No offset
  lcnt = i2c_dw_scl_lcnt (configI2C_CLOCK_HZ,
                          13,     // tLOW = 1.3 us
                          3,      // tf = 0.3 us
                          0);       // No offset

  MmioWrite32 (Base + DW_IC_FS_SCL_HCNT, hcnt);
  MmioWrite32 (Base + DW_IC_FS_SCL_LCNT, lcnt);
  MmioWrite32 (Base + DW_IC_HS_SCL_HCNT, hcnt);
  MmioWrite32 (Base + DW_IC_HS_SCL_LCNT, lcnt);

  DEBUG ((EFI_D_ERROR, "Fast-mode HCNT:LCNT = %d:%d\n", hcnt, lcnt));

  return Status;
}

EFI_STATUS
EFIAPI
Reset (
  IN CONST EFI_I2C_MASTER_PROTOCOL *This
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
StartRequest (
  IN       CONST EFI_I2C_MASTER_PROTOCOL    *This,
  IN       UINTN                            SlaveAddress,
  IN       EFI_I2C_REQUEST_PACKET           *RequestPacket,
  IN       EFI_EVENT                        Event      OPTIONAL,
  OUT      EFI_STATUS                       *I2cStatus OPTIONAL
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;

  I2cAccess ((I2CMASTER_PRIVATE*)This, SlaveAddress, RequestPacket);

  return Status;
}

/**
  This function checks for errors during SMBUS access

  @param Base - Base address of I2cController

  @retval EFI_STATUS
*/

UINT32
I2cDwCheckErrors (
  IN  UINT32   Base
  )
{
  UINT32 I2cStatusCnt;
  UINT32 TxAbrt;
  UINT32 Status;
  UINT32 I2cPollTime;

  I2cStatusCnt = DW_STATUS_WAIT_RETRY;
  TxAbrt = 0;
  Status = MmioRead32 (Base + DW_IC_RAW_INTR_STAT);
  I2cPollTime = (10 * 1000000) / configI2C_MAX_SPEED;

  if (Status & DW_IC_INTR_RX_UNDER) {
    MmioRead32 (Base + DW_IC_CLR_RX_UNDER);
  }
  if (Status & DW_IC_INTR_RX_OVER) {
    MmioRead32 (Base + DW_IC_CLR_RX_OVER);
  }
  if (Status & DW_IC_INTR_TX_ABRT) {
    Status = MmioRead32 (Base + DW_IC_TX_ABRT_SOURCE);
    MmioRead32 (Base + DW_IC_CLR_TX_ABRT);
    DEBUG ((EFI_D_ERROR, "TX_ABORT Error\n"));
  }

  if (Status & DW_IC_ERR_CONDITION) {
    if (Status || TxAbrt) {
      DEBUG ((EFI_D_ERROR, "Errors on I2cBus %08x %08x: \n", Status, TxAbrt));
    }
    MmioWrite32 (Base + DW_IC_ENABLE, 0); // Disable the adapter
    I2cDwWaitI2cEnable (Base, CHECK_IC_EN_HIGH);  //Wait controller status change
    do {
      MicroSecondDelay (I2cPollTime);
      if (I2cStatusCnt == 0) {
        DEBUG ((EFI_D_ERROR, "Timeout during disable \n"));
      }
    } while (((MmioRead32 (Base + DW_IC_ENABLE_STATUS) & 0x01)) && I2cStatusCnt--);
    MmioWrite32 (Base + DW_IC_TAR, 0); // Clear Target address
  }

  return (Status & DW_IC_ERR_CONDITION);
}


/**
  This function waits for bus to not be busy

  @param Base - Base address of I2cController

  @retval TRUE - Timeout while waiting for not busy
  @retval FALSE - Bus is not busy
*/

BOOLEAN
I2cDwWaitBusNotBusy (
  IN  UINT32   Base
  )
{
  INTN Timeout;
  Timeout = DW_BUS_WAIT_TIMEOUT;

  while (MmioRead32 (Base + DW_IC_STATUS) & 0x20) {
    if (Timeout <= 0) {
      DEBUG ((EFI_D_ERROR, "I2cTimeout waiting for bus ready\n"));
      return TRUE;
    }
    Timeout--;
    MicroSecondDelay (1000);
  }
  return FALSE;
}

/**
  This function waits for the TX FIFO to be available

  @param Base - Base address of I2cController

  @retval EFI_STATUS
*/

EFI_STATUS
I2cDwWaitTxData (
  IN  UINT32   Base
  )
{
  INTN Timeout;
  UINT32 I2cPollTime;

  Timeout = DW_TX_DATA_RETRY;
  I2cPollTime = (10 * 1000000) / configI2C_MAX_SPEED;

  while (0 == (MmioRead32 (Base + DW_IC_STATUS) & DW_I2C_STATUS_TFE)) {
    if (Timeout <= 0) {
      DEBUG ((EFI_D_ERROR, "I2cTimeout waiting for Tx Fifo full empty \n"));
      return EFI_TIMEOUT;
    }
    Timeout--;
    MicroSecondDelay (I2cPollTime);
  }
  DEBUG ((EFI_D_ERROR, "I2C_STATUS : %x, Tx timeout remaining : %d of %d\n", MmioRead32 (Base + DW_IC_STATUS), Timeout, DW_TX_DATA_RETRY));
  return EFI_SUCCESS;
}

EFI_STATUS
I2cDwWaitRxData (
  IN  UINT32   Base,
  IN  UINT32   RxFifoDepth
  )
{
  INTN Timeout;
  UINT32 I2cPollTime;

  Timeout = DW_TX_DATA_RETRY;
  I2cPollTime = (10 * 1000000) / configI2C_MAX_SPEED;

  while (MmioRead32 (Base + DW_IC_RXFLR) < RxFifoDepth) {
    if (Timeout <= 0) {
      DEBUG ((EFI_D_ERROR, "I2cTimeout waiting for Rx data ready\n"));
      return EFI_TIMEOUT;
    }
    Timeout--;
    MicroSecondDelay (I2cPollTime);
  }
  DEBUG ((EFI_D_ERROR, "I2C_STATUS : %x, Rx timeout remaining : %d of %d\n", MmioRead32 (Base + DW_IC_STATUS), Timeout, DW_TX_DATA_RETRY));
  return EFI_SUCCESS;
}

/**
  This function waits for the TX FIFO to be available

  @param Base - Base address of I2cController

  @retval EFI_STATUS
*/

EFI_STATUS
I2cDwWaitI2cEnable (
  IN  UINT32   Base,
  IN  UINT32   I2cEnBit
  )
{
  INTN Timeout;
  UINT32 I2cPollTime;

  Timeout = DW_STATUS_WAIT_RETRY;
  I2cPollTime = (10 * 1000000) / configI2C_MAX_SPEED;

  while (I2cEnBit == (MmioRead32 (Base + DW_IC_ENABLE_STATUS) & DW_I2C_EN_STATUS_IC_EN)) {
    if (Timeout <= 0) {
      DEBUG ((EFI_D_ERROR, "Timeout waiting for I2cEnable : %x\n", (MmioRead32 (Base + DW_IC_ENABLE_STATUS))));
      return EFI_TIMEOUT;
    }
    Timeout--;
    MicroSecondDelay (I2cPollTime);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
I2cInit (
  IN OUT   I2CMASTER_PRIVATE    *Private
  )
{
  EFI_STATUS Status;
  UINT32 Base;
  UINTN BusClockHertz;
  I2C_CONFIGURATION * configuration = &Private->I2cConfiguration;

  Base = Private->I2cSdpAddress;

  // Disable the interface
  MmioWrite32 (Base + DW_IC_ENABLE, 0);
  if (I2cDwWaitI2cEnable (Base, CHECK_IC_EN_HIGH)) {
    return EFI_NOT_READY;
  }

  //Mask Interrupt and Clear interrupt status
  MmioWrite32 (Base + DW_IC_INTR_MASK, 0);
  (VOID)MmioRead32 (Base + DW_IC_CLR_INTR);
  (VOID)MmioRead32 (Base + DW_IC_CLR_TX_ABRT);

  configuration->RxFifoDepth = ((MmioRead32 (Base + DW_IC_COMP_PARAM_1) & DW_I2C_RX_BUFFER_DEPTH_MASK) >> DW_I2C_RX_BUFFER_DEPTH_SHIFT) + 1;
  configuration->TxFifoDepth = ((MmioRead32 (Base + DW_IC_COMP_PARAM_1) & DW_I2C_TX_BUFFER_DEPTH_MASK) >> DW_I2C_TX_BUFFER_DEPTH_SHIFT) + 1;

  //Set default to Standard Speed
  BusClockHertz = SS_SPEED;
  Status = SetBusFrequency (&Private->I2cController, &BusClockHertz);
  return Status;
}

/**
  This function sets the target address for device on I2cBus

  @param Chip - Address of I2cDevice

  @retval EFI_STATUS
*/

EFI_STATUS
I2cSetTarget (
  IN  UINT32   Chip,
  IN  UINT32   Base
  )
{
  UINT32 I2cStatusCnt;
  UINT32 I2cPollTime;
  EFI_STATUS Status = EFI_SUCCESS;

  if (MmioRead32 (Base + DW_IC_TAR) != Chip) {
    MmioWrite32 (Base + DW_IC_ENABLE, 0); // Disable the Controller
    if (I2cDwWaitI2cEnable (Base, CHECK_IC_EN_HIGH)) {
      return EFI_NOT_READY;
    }
    I2cPollTime = (10 * 1000000) / configI2C_MAX_SPEED;
    I2cStatusCnt = DW_STATUS_WAIT_RETRY;

    do {
      MicroSecondDelay (I2cPollTime);
      if (I2cStatusCnt == 0) {
        DEBUG ((EFI_D_ERROR, "I2cSet Target: Timeout during disable\n"));
        MmioWrite32 (Base + DW_IC_TAR, 0); // Clear Target Address
        Status = EFI_DEVICE_ERROR;
        return Status;
      }
    } while (((MmioRead32 (Base + DW_IC_ENABLE_STATUS) & 0x01)) && I2cStatusCnt--);

    MmioWrite32 (Base + DW_IC_TAR, Chip); // Set Target Address
    DEBUG ((EFI_D_ERROR, "I2cTarget Set - Chip Address:%x\n", Chip));
    I2cStatusCnt = DW_STATUS_WAIT_RETRY;
    MmioWrite32 (Base + DW_IC_ENABLE, 1); // Enable the Controller
    I2cDwWaitI2cEnable (Base, CHECK_IC_EN_LOW);  //Check IC_EN bit
    do {
      MicroSecondDelay (I2cPollTime);
      if (I2cStatusCnt == 0) {
        DEBUG ((EFI_D_ERROR, "I2cSet Target: Timeout during enable\n"));
        MmioWrite32 (Base + DW_IC_TAR, 0); // Clear Target Address
        Status = EFI_DEVICE_ERROR;
        return Status;
      }
    } while ((!(MmioRead32 (Base + DW_IC_ENABLE_STATUS) & 0x01)) && I2cStatusCnt--);

    if (MmioRead32 (Base + DW_IC_TAR) != Chip) {
      DEBUG ((EFI_D_ERROR, "Cannot set the target on I2cBus to %x\n", Chip));
      Status = EFI_INVALID_PARAMETER;
    }
  }
  return Status;
}

EFI_STATUS
I2cWriteRead (
  IN OUT   I2CMASTER_PRIVATE    *Private
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32   Base;
  UINT32   TxFifoCount;
  UINT32   RxFifoCount;
  UINT32   Index;

  Base = Private->I2cSdpAddress;  // For convenience

  if (I2cDwWaitBusNotBusy (Base)) {
    return EFI_NOT_READY;
  }

  MmioWrite32 (Base + DW_IC_INTR_MASK, 0);
  (VOID)MmioRead32 (Base + DW_IC_CLR_INTR);

  DEBUG ((EFI_D_ERROR, "1 : Enable I2cInterface\n"));
  // Enable the interface
  MmioWrite32 (Base + DW_IC_ENABLE, DW_I2C_ENABLE);
  if (I2cDwWaitI2cEnable (Base, CHECK_IC_EN_LOW)) {
    return EFI_NOT_READY;
  }

#if 0
  // If both RX and TX counts are zero then just ping the device with a read
  if (0 == Private->TransferCount) {
    Private->TransferCount = 1;
  }
#endif

  // Load up the TX FIFO
  if (0 < Private->TransferCount) {
    while (0 < Private->WriteCount) {
      TxFifoCount = (Private->I2cConfiguration.TxFifoDepth < Private->WriteCount) ? Private->I2cConfiguration.TxFifoDepth : Private->WriteCount;
      DEBUG ((EFI_D_ERROR, "2 : Load up the TX FIFO, TxFifofDepth: %d\n", TxFifoCount));
      for (Index = 0; Index < TxFifoCount ; Index++) {
        if ((Private->TransferCount == 1) && (Private->ReadCount == 0)) {
          //DEBUG((EFI_D_ERROR, "Write Data: 0x%x\n", *(Private->WriteData)));
          MmioWrite32(Base + DW_IC_DATA_CMD, (*(Private->WriteData++) & DW_I2C_DATA_MASK) | DW_I2C_DATA_STOP);
        }
        else {
          //DEBUG((EFI_D_ERROR, "Write Data: 0x%x\n", *(Private->WriteData)));
          MmioWrite32(Base + DW_IC_DATA_CMD, *(Private->WriteData++) & DW_I2C_DATA_MASK);
        }
        Private->WriteCount--;
        Private->TransferCount--;
      }
      DEBUG ((EFI_D_ERROR, "3 : Wait for TxFifo empty\n"));
      //Wait for TxFifo empty
      Status = I2cDwWaitTxData (Base);
      if (Status) {
        //Transcation failed, send STOP command to free the bus
        MmioWrite32 (Base + DW_IC_DATA_CMD, DW_I2C_DATA_STOP);
        return Status;
      }
    }
  }
  DEBUG ((EFI_D_ERROR, "Write Count : %d, Transfer Count : %d\n", Private->WriteCount, Private->TransferCount));

  if (0 < Private->TransferCount) {
    DEBUG ((EFI_D_ERROR, "4 : Start Read trasaction\n"));

    while (0 < Private->ReadCount) {
      RxFifoCount = (Private->I2cConfiguration.RxFifoDepth < Private->ReadCount) ? Private->I2cConfiguration.RxFifoDepth : Private->ReadCount;
      DEBUG ((EFI_D_ERROR, "5 : fille up Rx Fifo, RxFifoDepth : %d\n", RxFifoCount));
      // Fill up Rx Fifo
      for (Index = 0; Index < RxFifoCount ; Index++) {
        if (Private->ReadCount == 1) {
          MmioWrite32 (Base + DW_IC_DATA_CMD, DW_I2C_DATA_CMD | DW_I2C_DATA_STOP);
        } else {
          MmioWrite32 (Base + DW_IC_DATA_CMD, DW_I2C_DATA_CMD);
        }
        Private->ReadCount--;
        Private->TransferCount--;
      }
      DEBUG ((EFI_D_ERROR, "6 : Wait Rx data ready \n"));
      // Waiting for Rx data ready
      Status = I2cDwWaitRxData (Base, RxFifoCount);
      if (Status) {
        //Transcation failed, send STOP command to free the bus
        MmioWrite32 (Base + DW_IC_DATA_CMD, DW_I2C_DATA_STOP);
        return Status;
      }
      DEBUG ((EFI_D_ERROR, "7 : Read data from Rxfifo\n"));
      for (Index = 0; Index < RxFifoCount; Index++) {
        if (I2cDwCheckErrors (Base)) {
          MmioWrite32 (Base + DW_IC_DATA_CMD, DW_I2C_DATA_STOP);
          return EFI_DEVICE_ERROR;
        }
        *(Private->ReadData++) = (UINT8) (MmioRead32 (Base + DW_IC_DATA_CMD) & DW_I2C_DATA_MASK); // Receive data unit from RxFifo
      }
      DEBUG ((EFI_D_ERROR, "Read Count : %d, Transfer Count : %d\n", Private->ReadCount, Private->TransferCount));
    }
  }

  // Transaction complete
  DEBUG ((EFI_D_ERROR, "8 : transaction complete\n"));

  return EFI_SUCCESS;
}

EFI_STATUS
I2cAccess (
  IN       I2CMASTER_PRIVATE         *Private,
  IN       UINTN                     SlaveAddress,
  IN OUT   EFI_I2C_REQUEST_PACKET    *RequestPacket
  )
{
  EFI_STATUS  Status;
  BOOLEAN     IsWriteOperation;
  UINT32      Base;

  Base = Private->I2cSdpAddress;

  //No PEC, ProcessCall and BlkProcessCall support
  if (RequestPacket->Operation[0].Flags & (I2C_FLAG_SMBUS_PEC | I2C_FLAG_SMBUS_PROCESS_CALL)) {
    return EFI_UNSUPPORTED;
  }

  //No QuickRead & QuickWrite
  if (0 == RequestPacket->Operation[0].LengthInBytes) {
    return EFI_UNSUPPORTED;
  }

  if (RequestPacket->OperationCount == 1) {
    if (RequestPacket->Operation[0].Flags & I2C_FLAG_READ) {
      //command READ
      Private->ReadCount = RequestPacket->Operation[0].LengthInBytes;
      Private->ReadData = RequestPacket->Operation[0].Buffer;
      Private->WriteCount = 0;
      Private->WriteData = NULL;
    } else {
      //command WRITE
      Private->ReadCount = 0;
      Private->ReadData = NULL;
      Private->WriteCount = RequestPacket->Operation[0].LengthInBytes;
      Private->WriteData = RequestPacket->Operation[0].Buffer;
    }
  } else if (RequestPacket->OperationCount == 2) {
    IsWriteOperation = (RequestPacket->Operation[1].Flags & I2C_FLAG_READ) ? FALSE : TRUE;
    if (RequestPacket->Operation[1].Flags & I2C_FLAG_READ) {
      //ReceviedByte
      Private->ReadCount = RequestPacket->Operation[1].LengthInBytes;
      Private->ReadData = RequestPacket->Operation[1].Buffer;
      Private->WriteCount = RequestPacket->Operation[0].LengthInBytes;
      Private->WriteData = RequestPacket->Operation[0].Buffer;
    } else {
      //only READ operation has two OperationCount
      return EFI_INVALID_PARAMETER;
    }
  }
  Private->TransferCount = Private->ReadCount + Private->WriteCount;

  //Set target device slave address
  if (I2cSetTarget ((UINT32)SlaveAddress, Private->I2cSdpAddress) != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  Status = I2cWriteRead (Private);

  // Disable the interface
  DEBUG ((EFI_D_ERROR, "9 : disable I2cInterface\n"));
  MmioWrite32 (Base + DW_IC_ENABLE, 0);
  I2cDwWaitI2cEnable (Base, CHECK_IC_EN_HIGH);  //Wait controller status change

  return Status;
}
