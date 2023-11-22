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
 ******************************************************************************
 */

#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Protocol/I3cMasterProtocol.h>
#include <I3cRegs.h>
#include "HygonI3cMasterDxe.h"

extern  EFI_BOOT_SERVICES  *gBS;

UINT32  BaseProgrammed = 0;               ///< BaseProgrammed != 0 indicates that this controller has been initialized.
UINT8   IsDeviceAddrTableProgrammed = 0;  ///< IsDeviceAddrTableProgrammed = 1 indicates DEV_ADDR_TABLE1_LOC1 has been programmed, which means the Static Address
                                          ///< has been used to initialize the dynamic address in the DEV_ADDR_TABLE1_LOC1
UINT32  StaticAddressProgrammed = 0;      ///< If IsDeviceAddrTableProgrammed = 1,  StaticAddressProgrammed contains the dynamic address in DEV_ADDR_TABLE1_LOC1

/*++

Routine Description:

  I3cMaster Driver Entry. Publish I3cMaster PROTOCOL

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
HygonI3cMasterDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  HYGON_DXE_I3C_MASTER_PRIVATE     *I3cPrivate;
  EFI_I3C_CONTROLLER_CAPABILITIES  *I3cControllerCapabilities;
  UINT32                           Index;

  // Create the Protocols
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (HYGON_DXE_I3C_MASTER_PRIVATE), (VOID **)&I3cPrivate);
  if (EFI_ERROR (Status)) {
    return Status;
  } else {
    // clear instances content
    gBS->SetMem (I3cPrivate, sizeof (HYGON_DXE_I3C_MASTER_PRIVATE), 0);
  }

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (EFI_I3C_CONTROLLER_CAPABILITIES), (VOID **)&I3cControllerCapabilities);
  if (EFI_ERROR (Status)) {
    return Status;
  } else {
    // clear instances content
    gBS->SetMem (I3cControllerCapabilities, sizeof (EFI_I3C_CONTROLLER_CAPABILITIES), 0);
  }

  // Init I3cCotroller capabilities data structure
  I3cControllerCapabilities->StructureSizeInBytes = sizeof (EFI_I3C_CONTROLLER_CAPABILITIES);
  I3cControllerCapabilities->MaximumReceiveBytes  = 0xFFFFFFFF;
  I3cControllerCapabilities->MaximumTransmitBytes = 0xFFFFFFFF;
  I3cControllerCapabilities->MaximumTotalBytes    = 0xFFFFFFFF;

  for (Index = 0; Index < I3C_CONTROLLER_COUNT; Index++) {
    I3cPrivate->I3cMasterProtocol[Index].I3cSdpAddress = I3cSdpAddressList[Index];
    I3cPrivate->I3cMasterProtocol[Index].I3cProtocol.SetBusFrequency = SetBusFrequency;
    I3cPrivate->I3cMasterProtocol[Index].I3cProtocol.Reset = Reset;
    I3cPrivate->I3cMasterProtocol[Index].I3cProtocol.StartRequest = StartRequest;
    I3cPrivate->I3cMasterProtocol[Index].I3cProtocol.I3cControllerCapabilities = I3cControllerCapabilities;
    I3cPrivate->I3cMasterProtocol[Index].I3cI2cMode = I3C_MODE;
    CopyGuid (&I3cPrivate->I3cMasterProtocol[Index].I3cProtocol.Identifier, &HygonI3cMasterID);
    I3cInit (&I3cPrivate->I3cMasterProtocol[Index]);
    // Install Protocol
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &I3cPrivate->I3cMasterProtocol[Index].Handle,
                    &gEfiI3cMasterProtocolGuid,
                    &I3cPrivate->I3cMasterProtocol[Index],
                    NULL
                    );
  }

  return (Status);
}

/*---------------------------------------------------------------------------------------*/

/**
 *
 * This function is to initialize I3c Controller
 *
 *
 * @param[in]   Private
 * @param[out]  EFI_STATUS
 *
 */
EFI_STATUS
I3cInit (
  IN OUT   I3CMASTER_PROTOCOL_PRIVATE    *Private
  )
{
  EFI_STATUS                 Status;
  UINT32                     Base;
  I3C_DEVICE_CTRL            DeviceCtrl;
  I3C_QUEUE_SIZE_CAPABILITY  QueueSizeCap;
  I3C_DEVICE_CTRL_EXTENDED   DeviceCtrlExt;
  UINTN                      BusClockHertz;

  I3C_CONFIGURATION  *configuration = &Private->I3cConfiguration;

  /* I3c QUEUE_SIZE_CAPABILITY
  - 0x0: 2 DWORDS,  8Bytes
  - 0x1: 4 DWORDS,  16Bytes
  - 0x2: 8 DWORDS,  32Bytes
  - 0x3: 16 DWORDS, 64Bytes
  - 0x4: 32 DWORDS, 128Bytes
  - 0x5: 64 DWORDS, 256Bytes*/
  UINT32  QueueSizeCapability[] = { 8, 16, 32, 64, 128, 256 };

  Base = Private->I3cSdpAddress;

  // Disable controller
  DeviceCtrl.Value = MmioRead32 (Base + DEVICE_CTRL);
  if (Private->I3cI2cMode == I2C_MODE) {
    DeviceCtrl.Field.I2C_SLAVE_PRESENT = 1;
  } else {
    DeviceCtrl.Field.I2C_SLAVE_PRESENT = 0;
  }

  DeviceCtrl.Field.ENABLE = I3C_CONTROLLER_DISABLE;
  MmioWrite32 (Base + DEVICE_CTRL, DeviceCtrl.Value);

  // Device Mode Selection, Master Mode
  DeviceCtrlExt.Value = MmioRead32 (Base + DEVICE_CTRL_EXTENDED);
  DeviceCtrlExt.Field.DEV_OPERATION_MODE = MASTER_MODE;
  MmioWrite32 (Base + DEVICE_CTRL_EXTENDED, DeviceCtrlExt.Value);

  // Initializing Common Registers
  Status = InitCommonRegister (Base);

  // Get TX and RX buffer size
  QueueSizeCap.Field.RX_BUF_SIZE = ((MmioRead32 (Base + QUEUE_SIZE_CAPABILITY) & RX_BUF_SIZE_MASK) >> RX_BUF_SIZE_OFFSET);
  QueueSizeCap.Field.TX_BUF_SIZE = ((MmioRead32 (Base + QUEUE_SIZE_CAPABILITY) & TX_BUF_SIZE_MASK) >> TX_BUF_SIZE_OFFSET);
  configuration->RxFifoDepth     = QueueSizeCapability[QueueSizeCap.Field.RX_BUF_SIZE];
  configuration->TxFifoDepth     = QueueSizeCapability[QueueSizeCap.Field.TX_BUF_SIZE];

  // Set I3c Frequency
  Status = SetBusFrequency (&Private->I3cProtocol, &BusClockHertz);

  return Status;
}

/**
 *
 * @brief This function initializes I3C Common Registers.
 *
 *
 * @param Base - I3C controller base address
 *
 * @retval EFI_SUCCESS - success.
 *
 */
EFI_STATUS
InitCommonRegister (
  UINT32  Base
  )
{
  I3C_DATA_BUFFER_THLD_CTRL  DataBuffThld;
  I3C_QUEUE_THLD_CTRL        QueueThldCtrl;

  // Program Queue Threshold Control Registers
  QueueThldCtrl.Value = MmioRead32 (Base + QUEUE_THLD_CTRL);
  QueueThldCtrl.Field.CMD_EMPTY_BUF_THLD = 0;
  QueueThldCtrl.Field.RESP_BUF_THLD = 0;
  MmioWrite32 (Base + QUEUE_THLD_CTRL, QueueThldCtrl.Value);

  // Program Data Buffer Threshold Control Register
  DataBuffThld.Value = MmioRead32 (Base + DATA_BUFFER_THLD_CTRL);
  DataBuffThld.Field.TX_EMPTY_BUF_THLD = 0;
  DataBuffThld.Field.RX_BUF_THLD   = 0;
  DataBuffThld.Field.TX_START_THLD = 0;
  DataBuffThld.Field.RX_START_THLD = 0;
  MmioWrite32 (Base + DATA_BUFFER_THLD_CTRL, DataBuffThld.Value);

  // Enable all Interrupts in INTR_STATUS_EN register
  MmioWrite32 (Base + INTR_STATUS_EN, INTR_STATUS_ALL_EN);

  // Enable all Interrupts in INTR_SIGNAL_EN register
  MmioWrite32 (Base + INTR_SIGNAL_EN, INTR_SIGNAL_ALL_EN);

  return EFI_SUCCESS;
}

/**
 *
 * @brief This function Reset I3C Controller.
 *
 *
 * @param This Pointer to EFI_DXE_I3C_MASTER_PROTOCOL
 *
 * @retval EFI_UNSUPPORTED
 *
 */
EFI_STATUS
EFIAPI
Reset (
  IN CONST EFI_DXE_I3C_MASTER_PROTOCOL  *This
  )
{
  return EFI_UNSUPPORTED;
}

/**
 * @brief This function sets I3c bus frequency.
 *        Program the I3C Open Drain Timing Register and Push Pull Timing Register.
 *
 * @param This  Pointer to EFI_DXE_I3C_MASTER_PROTOCOL
 * @param BusClockHertz  Frequency
 *
 * @returns EFI_STATUS EFI_SUCCESS   success.
 */
EFI_STATUS
EFIAPI
SetBusFrequency (
  IN EFI_DXE_I3C_MASTER_PROTOCOL    *This,
  IN UINTN                          *BusClockHertz
  )
{
  I3CMASTER_PROTOCOL_PRIVATE  *Private;
  I3C_DEVICE_CTRL             DeviceCtrl;
  I3C_SCL_I3C_OD_TIMING       SclI3cOdTiming;
  I3C_SCL_I3C_PP_TIMING       SclI3cPpTiming;
  I3C_SCL_I2C_FM_TIMING       SclI2cFmTiming;
  I3C_SCL_I2C_FMP_TIMING      SclI3cFmpTiming;
  UINT32                      Base;

  DEBUG ((EFI_D_ERROR, "Set I3CBusFrequency\n"));

  // todo,how to set time register according to BusClockHertz
  Private = (I3CMASTER_PROTOCOL_PRIVATE *)This;
  Base    = Private->I3cSdpAddress;

  // Disable controller
  DeviceCtrl.Value = MmioRead32 (Base + DEVICE_CTRL);
  DeviceCtrl.Field.ENABLE = I3C_CONTROLLER_DISABLE;
  MmioWrite32 (Base + DEVICE_CTRL, DeviceCtrl.Value);

  // Set SCL clock high period and low period count for I3C Open Drain transfers.
  SclI3cOdTiming.Value = MmioRead32 (Base + SCL_I3C_OD_TIMING);
  SclI3cOdTiming.Field.I3C_OD_LCNT = OD_LCNT;
  SclI3cOdTiming.Field.I3C_OD_HCNT = OD_HCNT;
  MmioWrite32 (Base + SCL_I3C_OD_TIMING, SclI3cOdTiming.Value);

  // Set SCL clock high period and low period count for I3C Push Pull transfers.
  SclI3cPpTiming.Value = MmioRead32 (Base + SCL_I3C_PP_TIMING);
  SclI3cPpTiming.Field.I3C_PP_LCNT = PP_LCNT;
  SclI3cPpTiming.Field.I3C_PP_HCNT = PP_HCNT;
  MmioWrite32 (Base + SCL_I3C_PP_TIMING, SclI3cPpTiming.Value);

  // Set SCL clock high period and low period count for I2C FM.
  SclI2cFmTiming.Value = MmioRead32 (Base + SCL_I2C_FM_TIMING);
  SclI2cFmTiming.Field.I2C_FM_LCNT = FM_LCNT;
  SclI2cFmTiming.Field.I2C_FM_HCNT = FM_HCNT;
  MmioWrite32 (Base + SCL_I2C_FM_TIMING, SclI2cFmTiming.Value);

  // Set SCL clock high period and low period count for I2C FMP.
  SclI3cFmpTiming.Value = MmioRead32 (Base + SCL_I2C_FMP_TIMING);
  SclI3cFmpTiming.Field.I2C_FMP_LCNT = FMP_LCNT;
  SclI3cFmpTiming.Field.I2C_FMP_HCNT = FMP_HCNT;
  MmioWrite32 (Base + SCL_I2C_FMP_TIMING, SclI3cPpTiming.Value);

  return EFI_SUCCESS;
}

/**
 * @brief This function transmit/receive Data.
 *
 * @param This  Pointer to EFI_DXE_I3C_MASTER_PROTOCOL
 * @param SlaveAddress   Slave Device Address
 * @param RequestPacket  Pointer to EFI_I3C_REQUEST_PACKET
 *
 * @returns EFI_STATUS EFI_SUCCESS   success.
 */
EFI_STATUS
EFIAPI
StartRequest (
  IN CONST EFI_DXE_I3C_MASTER_PROTOCOL      *This,
  IN UINTN                                  SlaveAddress,
  IN EFI_I3C_REQUEST_PACKET                 *RequestPacket
  )
{
  EFI_STATUS  Status;

  Status = I3cAccess ((I3CMASTER_PROTOCOL_PRIVATE *)This, (UINT32)SlaveAddress, RequestPacket);

  return Status;
}

/**
 * @brief This function transmit/receive Data.
 *
 * @param This  Pointer to I3CMASTER_PROTOCOL_PRIVATE
 * @param SlaveAddress   Slave Device Address
 * @param RequestPacket  Pointer to EFI_I3C_REQUEST_PACKET
 *
 * @returns EFI_STATUS EFI_SUCCESS   success.
 */
EFI_STATUS
I3cAccess (
  IN I3CMASTER_PROTOCOL_PRIVATE         *Private,
  IN UINT32                             SlaveAddress,
  IN OUT EFI_I3C_REQUEST_PACKET         *RequestPacket
  )
{
  EFI_STATUS  Status;

  // No QuickRead & QuickWrite
  if (0 == RequestPacket->Operation[0].LengthInBytes) {
    return Status = EFI_UNSUPPORTED;
  }

  if (RequestPacket->OperationCount == 1) {
    if (RequestPacket->Operation[0].Flags & I3C_FLAG_READ) {
      // command READ
      Private->ReadCount  = RequestPacket->Operation[0].LengthInBytes;
      Private->ReadData   = RequestPacket->Operation[0].Buffer;
      Private->WriteCount = 0;
      Private->WriteData  = NULL;
    } else {
      // command WRITE
      Private->ReadCount  = 0;
      Private->ReadData   = NULL;
      Private->WriteCount = RequestPacket->Operation[0].LengthInBytes;
      Private->WriteData  = RequestPacket->Operation[0].Buffer;
    }
  } else if (RequestPacket->OperationCount == 2) {
    if (RequestPacket->Operation[1].Flags & I3C_FLAG_READ) {
      // ReceviedByte
      Private->ReadCount  = RequestPacket->Operation[1].LengthInBytes;
      Private->ReadData   = RequestPacket->Operation[1].Buffer;
      Private->WriteCount = RequestPacket->Operation[0].LengthInBytes;
      Private->WriteData  = RequestPacket->Operation[0].Buffer;
    } else {
      // only READ operation has two OperationCount
      return EFI_INVALID_PARAMETER;
    }
  }

  Private->TransferCount = Private->ReadCount + Private->WriteCount;

  // I3c Master register init
  Status = MasterRegistersInit (Private->I3cSdpAddress, SlaveAddress, Private->I3cI2cMode);

  // I3c write or read transaction
  Status = I3cWriteRead (Private);

  return Status;
}

/**
 * @brief This function initializes I3C Master Registers.
 *
 * @param   Base            I3C controller base address
 * @param   StaticAddress   Static Address of slave device
 * @param   I3cI2cMode      I3c or I2c mode
 *
 * @retval      EFI_SUCCESS        success.
 * @retval      EFI_DEVICE_ERROR   fail.
 *
 */
EFI_STATUS
MasterRegistersInit (
  UINT32    Base,
  UINT32    StaticAddress,
  IN UINT8  I3cI2cMode
  )
{
  I3C_DEVICE_ADDR           DeviceAdrr;
  I3C_DEV_ADDR_TABLE1_LOC1  DevAddTable;
  UINT32                    DynamicAddress = StaticAddress;
  EFI_STATUS                Status         = EFI_DEVICE_ERROR;

  DEBUG ((EFI_D_ERROR, "I3C MasterRegisters Init\n"));
  // set master dynamic address
  DeviceAdrr.Value = MmioRead32 (Base + DEVICE_ADDR);
  DeviceAdrr.Field.DYNAMIC_ADDR = I3C_MASTER_DYNAMIC_ADDR;
  DeviceAdrr.Field.DYNAMIC_ADDR_VALID = 1;
  MmioWrite32 (Base + DEVICE_ADDR, DeviceAdrr.Value);

  // clear interrupt
  MmioWrite32 (Base + INTR_STATUS, INTR_STATUS_CLEAR);

  // I2c mode on I3c controller!
  if (I3cI2cMode == I2C_MODE) {
    // program Device Address Table
    DevAddTable.Value = MmioRead32 (Base + DEV_ADDR_TABLE1_LOC1);
    DevAddTable.Field.DEVICE = 1;
    DevAddTable.Field.STATIC_ADDRESS = StaticAddress;
    MmioWrite32 (Base + DEV_ADDR_TABLE1_LOC1, DevAddTable.Value);
    return EFI_SUCCESS;
  }

  if (IsDeviceAddrTableProgrammed == 0 || BaseProgrammed != Base) {
    // Set Dynamic Address by SETAASA CCC
    Status = SendSETAASA (Base, StaticAddress);
    if (Status == EFI_SUCCESS) {
      IsDeviceAddrTableProgrammed++;
      StaticAddressProgrammed = StaticAddress;
      BaseProgrammed = Base;
    }
  } else if (IsDeviceAddrTableProgrammed > 0 && StaticAddressProgrammed != StaticAddress) {
    // program Device Address Table
    DevAddTable.Value = MmioRead32 (Base + DEV_ADDR_TABLE1_LOC1);
    DevAddTable.Field.STATIC_ADDRESS      = StaticAddress;
    DevAddTable.Field.DEV_DYNAMIC_ADDR    = DynamicAddress;
    DevAddTable.Field.DYNAMIC_ADDR_PARITY = Addr_Xor_Neg (DynamicAddress);
    MmioWrite32 (Base + DEV_ADDR_TABLE1_LOC1, DevAddTable.Value);
    // Update StaticAddressProgrammed
    StaticAddressProgrammed = StaticAddress;
    Status = EFI_SUCCESS;
  }

  return Status;
}

/**
 * @brief This function Send SETAASA CCC to Slave Device.
 *
 * @param   Base            I3C controller base address
 * @param   StaticAddress   Static Address of slave device
 *
 * @retval      EFI_SUCCESS        success.
 * @retval      EFI_DEVICE_ERROR   fail.
 *
 */
EFI_STATUS
SendSETAASA (
  UINT32 Base,
  UINT32 StaticAddress
  )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  I3C_DEVICE_CTRL           DeviceCtrl;
  I3C_DEV_ADDR_TABLE1_LOC1  DevAddTable;
  I3C_COMMAND_DATA          CommandData;
  UINT32                    DynamicAddress = StaticAddress;

  DEBUG ((EFI_D_ERROR, "Send SETAASA CCC\n"));

  // Enable controller
  DeviceCtrl.Value = MmioRead32 (Base + DEVICE_CTRL);
  DeviceCtrl.Field.ENABLE = I3C_CONTROLLER_ENABLE;
  MmioWrite32 (Base + DEVICE_CTRL, DeviceCtrl.Value);

  // Wait until Master Idle
  Status = I3cWaitMasterIdle (Base);

  // Soft reset if bus is not idle.
  if (Status) {
    Status = I3cSoftRst (Base);
    Status = I3cWaitMasterIdle (Base);
  }

  if (Status) {
    // Transcation failed, send STOP condition to free the bus
    I3cAbort (Base);
    return Status;
  }

  // Send Transfer Command
  CommandData.Value = 0;
  CommandData.Trans_cmd.CMD_ATTR = TRANSFER_CMD;
  CommandData.Trans_cmd.CMD = SETAASA_CCC;
  CommandData.Trans_cmd.CP  = 1;
  CommandData.Trans_cmd.DEV_INDX = 0;
  CommandData.Trans_cmd.SPEED    = TRANS_SDR0;
  CommandData.Trans_cmd.ROC  = 1;
  CommandData.Trans_cmd.SDAP = 1;
  CommandData.Trans_cmd.RNW  = I3C_CMD_WRITE;
  CommandData.Trans_cmd.TOC  = 1;
  MmioWrite32 (Base + COMMAND_QUEUE_PORT, CommandData.Value);

  // Wait for Transfer Complete
  Status = I3cWaitRespStat (Base);
  if (Status) {
    I3cAbort (Base);
    return Status;
  }

  // Read RESPONSE_QUEUE_PORT Reg
  Status = I3cReadRespQueue (Base);
  if (Status) {
    I3cAbort (Base);
    return Status;
  }

  // Program Device Address Table
  DevAddTable.Field.STATIC_ADDRESS      = StaticAddress;
  DevAddTable.Field.DEV_DYNAMIC_ADDR    = DynamicAddress;
  DevAddTable.Field.DYNAMIC_ADDR_PARITY = Addr_Xor_Neg (DynamicAddress);
  DevAddTable.Field.DEVICE = 0;
  MmioWrite32 (Base + DEV_ADDR_TABLE1_LOC1, DeviceCtrl.Value);

  return Status;
}

/**
 * @brief This function ~XOR(DynamicAddress)
 *
 * @param   Addr   DynamicAddress address
 */
UINT32
Addr_Xor_Neg (
  UINT32 Addr
  )
{
  UINT32  xor;
  UINT8   i;

  xor = Addr & 0x1;
  for (i = 1; i < 7; i++) {
    xor ^= (Addr >> i) & 0x1;
  }

  return (~xor) & 0x1;
}

/**
*  @brief This function checks the response status
*
*  @param Base - Base address of I3cController
*
*  @retval EFI_TIMEOUT - Timeout while waiting for RESP_READY_STS to be set
*  @retval EFI_SUCCESS - RESP_READY_STS is set, and the application can
*  read available response from RESPONSE_QUEUE_PORT register.
*/
EFI_STATUS
I3cWaitRespStat (
  IN  UINT32   Base
  )
{
  I3C_INTR_STATUS  IntrStatus;
  UINT32           Timeout;

  DEBUG ((EFI_D_ERROR, "I3c WaitRespStat\n"));

  Timeout = STATUS_WAIT_RETRY;

  IntrStatus.Value = MmioRead32 (Base + INTR_STATUS);
  while (IntrStatus.Field.RESP_READY_STS == 0) {
    if (IntrStatus.Value & TRANSFER_ERR_STS_MASK) {
      DEBUG ((EFI_D_ERROR, "TRANSFER_ERR_STS is set.\n"));
      return EFI_DEVICE_ERROR;
    }

    if (IntrStatus.Value & TRANSFER_ABORT_STS_MASK) {
      DEBUG ((EFI_D_ERROR, "TRANSFER_ABORT_STS is set.\n"));
      return EFI_DEVICE_ERROR;
    }

    if (Timeout <= 0) {
      DEBUG ((EFI_D_ERROR, "I3cTimeout waiting for RESP_READY_STS set.\n"));
      return EFI_TIMEOUT;
    }

    Timeout--;
    MicroSecondDelay (HYGON_I3C_MS_DELAY);
  }

  return EFI_SUCCESS;
}

/**
*  @brief This function reads the response queue port register
*
*  @param Base   Base address of I3cController
*
*  @returns EFI_STATUS
*  @retval EFI_DEVICE_ERROR   ERR_STS_MASK is set
*  @retval EFI_SUCCESS        ERR_STS_MASK is NOT set
*/
EFI_STATUS
I3cReadRespQueue (
  IN  UINT32   Base
  )
{
  I3C_RESPONSE_DATA  ResponseData;

  DEBUG ((EFI_D_ERROR, "I3c ReadRespQueue\n"));

  ResponseData.Value = MmioRead32 (Base + RESPONSE_QUEUE_PORT);
  if (ResponseData.Rsp.ERR_STS != 0) {
    switch (ResponseData.Rsp.ERR_STS) {
      case CRC_ERR:
        DEBUG ((EFI_D_ERROR, "CRC error!\n"));
        break;
      case PARITY_ERR:
        DEBUG ((EFI_D_ERROR, "Parity error!\n"));
        break;
      case FRAME_ERR:
        DEBUG ((EFI_D_ERROR, "Frame error!\n"));
        break;
      case BROADCAST_ADDR_NACK:
        DEBUG ((EFI_D_ERROR, "I3C Broadcast Address Nack Error!\n"));
        break;
      case ADDR_NACK:
        DEBUG ((EFI_D_ERROR, "Error: Address NACK'ed!\n"));
        break;
      case RECV_BUFF_OVERFLOW:
        DEBUG ((EFI_D_ERROR, "Rx buf Overflow or Tx buf underflow for HDR!\n"));
        break;
      case TRANSFER_ABORT:
        DEBUG ((EFI_D_ERROR, "Transfer Aborted!\n"));
        break;
      case I2C_SLAVE_WRITE_NACK:
        DEBUG ((EFI_D_ERROR, "I2C Slave Write Data NACK Error!\n"));
        break;
      case PEC_ERR:
        DEBUG ((EFI_D_ERROR, "PEC error\n"));
        break;
    }

    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
*  @brief This function  operate I3C aborts
*  @param Base - Base address of I3cController
*
*  @returns EFI_STATUS
*  @retval EFI_TIMEOUT - SOFT_RST is not clear
*  @retval EFI_SUCCESS - SOFT_RST is clear
*/
EFI_STATUS
I3cSoftRst (
  IN  UINT32   Base
  )
{
  I3C_RESET_CTRL  ResetCtrl;
  UINT32          Timeout;

  DEBUG ((EFI_D_ERROR, "I3c Soft Reset.\n"));

  Timeout = STATUS_WAIT_RETRY;

  // Set SOFT_RST Field to 1
  ResetCtrl.Value = MmioRead32 (Base + RESET_CTRL);
  ResetCtrl.Field.SOFT_RST = 1;
  MmioWrite32 (Base + RESET_CTRL, ResetCtrl.Value);

  // Wait SOFT_RST Field to 0
  ResetCtrl.Value = MmioRead32 (Base + RESET_CTRL);
  while (ResetCtrl.Field.SOFT_RST != 0) {
    if (Timeout <= 0) {
      DEBUG ((EFI_D_ERROR, "I3cTimeout waiting for SoftReset.\n"));
      return EFI_TIMEOUT;
    }

    Timeout--;
    MicroSecondDelay (HYGON_I3C_MS_DELAY);
    ResetCtrl.Value = MmioRead32 (Base + RESET_CTRL);
  }

  // todo, soft reset need to init commonregister??
  // Init Common Register
  // InitCommonRegister(Base);

  return EFI_SUCCESS;
}

/**
*  @brief This function wait master idle
*
*  @param Base Base address of I3cController
*
*  @returns EFI_STATUS
*  @retval EFI_TIMEOUT   Timeout while waiting for master idle
*  @retval EFI_SUCCESS   master is idle
*/
EFI_STATUS
I3cWaitMasterIdle (
  IN  UINT32   Base
  )
{
  I3C_PRESENT_STATE  PresentState;
  UINT32             Timeout;

  Timeout = STATUS_WAIT_RETRY;

  // wait MASTER_IDLE to 1
  PresentState.Value = MmioRead32 (Base + PRESENT_STATE);

  while (PresentState.Field.MASTER_IDLE != 1) {
    if (Timeout <= 0) {
      DEBUG ((EFI_D_ERROR, "I3cTimeout waiting for MasterIdle.\n"));
      return EFI_TIMEOUT;
    }

    Timeout--;
    MicroSecondDelay (HYGON_I3C_MS_DELAY);
    PresentState.Value = MmioRead32 (Base + PRESENT_STATE);
  }

  return EFI_SUCCESS;
}

/**
*  @brief This function aborts I3C operation
*  @param Base - Base address of I3cController
*
*  @returns EFI_STATUS
*  @retval EFI_TIMEOUT - TRANSFER_ABORT_STS is not set
*  @retval EFI_SUCCESS - TRANSFER_ABORT_STS is set
*/
EFI_STATUS
I3cAbort (
  IN  UINT32   Base
  )
{
  I3C_DEVICE_CTRL    DeviceCtrl;
  I3C_INTR_STATUS    IntrStatus;
  I3C_PRESENT_STATE  PresentState;
  UINT32             Timeout;

  DEBUG ((EFI_D_ERROR, "I3c Abort\n"));

  Timeout = STATUS_WAIT_RETRY;

  // Set ABORT Field to 1
  DeviceCtrl.Value = MmioRead32 (Base + DEVICE_CTRL);
  DeviceCtrl.Field.ABORT = 1;
  MmioWrite32 (Base + DEVICE_CTRL, DeviceCtrl.Value);

  // wait TRANSFER_ABORT_STAT to 1
  IntrStatus.Value = MmioRead32 (Base + INTR_STATUS);
  while (IntrStatus.Field.TRANSFER_ABORT_STS != 1) {
    if (Timeout <= 0) {
      DEBUG ((EFI_D_ERROR, "I3cTimeout waiting for Abort.\n"));
      return EFI_TIMEOUT;
    }

    Timeout--;
    MicroSecondDelay (HYGON_I3C_MS_DELAY);
    IntrStatus.Value = MmioRead32 (Base + INTR_STATUS);
  }

  // wait enter the halt state
  PresentState.Value = MmioRead32 (Base + INTR_STATUS);
  while (PresentState.Field.CM_TFR_STS != 0xf) {
    if (Timeout <= 0) {
      DEBUG ((EFI_D_ERROR, "I3cTimeout waiting for enter the halt state.\n"));
      return EFI_TIMEOUT;
    }

    Timeout--;
    MicroSecondDelay (HYGON_I3C_MS_DELAY);
    PresentState.Value = MmioRead32 (Base + INTR_STATUS);
  }

  // flush all the queues and fifos before program DEVICE_CTRL[RESUME]
  I3cSoftRst (Base);

  // Set RESUME Field to 1
  DeviceCtrl.Value = MmioRead32 (Base + DEVICE_CTRL);
  DeviceCtrl.Field.RESUME = 1;
  MmioWrite32 (Base + DEVICE_CTRL, DeviceCtrl.Value);

  // wait RESUME to 0
  DeviceCtrl.Value = MmioRead32 (Base + DEVICE_CTRL);
  while (DeviceCtrl.Field.RESUME != 0) {
    if (Timeout <= 0) {
      DEBUG ((EFI_D_ERROR, "I3cTimeout waiting for RESUME.\n"));
      return EFI_TIMEOUT;
    }

    Timeout--;
    MicroSecondDelay (HYGON_I3C_MS_DELAY);
    DeviceCtrl.Value = MmioRead32 (Base + DEVICE_CTRL);
  }

  return EFI_SUCCESS;
}

/**
  @brief This function sends I3c write or read transactions

  @param Private - pointer to the I3C master private PROTOCOL

  @returns EFI_STATUS
*/
EFI_STATUS
I3cWriteRead (
  IN OUT   I3CMASTER_PROTOCOL_PRIVATE    *Private
  )
{
  EFI_STATUS        Status;
  I3C_COMMAND_DATA  CommandData;
  UINT32            Index;
  UINT32            i;
  UINT32            Value;
  UINT32            Base;
  UINT32            TxFifoCount;
  UINT32            RxFifoCount;
  UINT32            *Data32 = NULL;

  DEBUG ((EFI_D_ERROR, "I3c WriteRead\n"));

  Base = Private->I3cSdpAddress;

  // Wait until Master Idle
  Status = I3cWaitMasterIdle (Base);

  // Soft reset if bus is not idle.
  if (Status) {
    Status = I3cSoftRst (Base);
    Status = I3cWaitMasterIdle (Base);
  }

  if (Status) {
    // Transcation failed, send STOP condition to free the bus
    I3cAbort (Base);
    return Status;
  }

  if (Private->TransferCount > 0) {
    while (Private->WriteCount > 0) {
      TxFifoCount = (Private->I3cConfiguration.TxFifoDepth < Private->WriteCount) ? Private->I3cConfiguration.TxFifoDepth : Private->WriteCount;
      DEBUG ((EFI_D_ERROR, "TxFifoCount = 0x%x\n", TxFifoCount));
      // Since TX data should be 4-byte aligned, we write 4 bytes each time. Thus, Index1 needs to add 4 in each loop.
      for (Index = 0; Index < TxFifoCount; Index += 4) {
        Status = I3cWaitTxData (Base);
        if (Status) {
          I3cAbort (Base);
          return Status;
        }

        Data32 = (UINT32 *)&Private->WriteData[Index];
        MmioWrite32 (Base + TX_DATA_PORT, *Data32);
      }

      Private->WriteCount    -= TxFifoCount;
      Private->TransferCount -= TxFifoCount;

      // write command
      CommandData.Value = 0;
      CommandData.Trans_argu.CMD_ATTR = TRANSFER_ARGU;
      CommandData.Trans_argu.DATA_LEN = TxFifoCount;
      MmioWrite32 (Base + COMMAND_QUEUE_PORT, CommandData.Value);

      CommandData.Value = 0;
      CommandData.Trans_cmd.CMD_ATTR = TRANSFER_CMD;
      CommandData.Trans_cmd.CMD = 0;
      CommandData.Trans_cmd.CP  = 0;
      CommandData.Trans_cmd.DEV_INDX = 0;
      // I3c mode - 0x2. I2c mode - 0x0 or 0x1.
      if (Private->I3cI2cMode == I2C_MODE) {
        CommandData.Trans_cmd.SPEED = I2C_FM;
      } else {
        CommandData.Trans_cmd.SPEED = TRANS_SDR0;
      }

      CommandData.Trans_cmd.DBP  = 0;
      CommandData.Trans_cmd.ROC  = 1;
      CommandData.Trans_cmd.SDAP = 0;
      CommandData.Trans_cmd.RNW  = I3C_CMD_WRITE;
      // If it's a Write command, and it's the last TX FIFO transfer, then need to set the STOP bit (Toc)
      if ((Private->TransferCount == 0) && (Private->ReadCount == 0)) {
        CommandData.Trans_cmd.TOC = 1;
      } else {
        CommandData.Trans_cmd.TOC = 0;
      }

      CommandData.Trans_cmd.PEC = 1;
      MmioWrite32 (Base + COMMAND_QUEUE_PORT, CommandData.Value);

      if (CommandData.Trans_cmd.TOC == 1) {
        // Wait for Transfer Complete
        Status = I3cWaitRespStat (Base);
        if (Status) {
          I3cAbort (Base);
          return Status;
        }

        // Read RESPONSE_QUEUE_PORT Reg
        Status = I3cReadRespQueue (Base);
        if (Status) {
          I3cAbort (Base);
          return Status;
        }
      }
    }
  }

  if (Private->TransferCount > 0) {
    while (Private->ReadCount > 0) {
      RxFifoCount = (Private->I3cConfiguration.RxFifoDepth < Private->ReadCount) ? Private->I3cConfiguration.RxFifoDepth : Private->ReadCount;
      DEBUG ((EFI_D_ERROR, "RxFifoCount = 0x%x\n", RxFifoCount));
      // read command
      CommandData.Value = 0;
      CommandData.Trans_argu.CMD_ATTR = TRANSFER_ARGU;
      CommandData.Trans_argu.DATA_LEN = RxFifoCount;
      MmioWrite32 (Base + COMMAND_QUEUE_PORT, CommandData.Value);

      CommandData.Value = 0;
      CommandData.Trans_cmd.CMD_ATTR = TRANSFER_CMD;
      CommandData.Trans_cmd.CMD = 0;
      CommandData.Trans_cmd.CP  = 0;
      CommandData.Trans_cmd.DEV_INDX = 0;
      CommandData.Trans_cmd.SPEED    = TRANS_SDR0;
      CommandData.Trans_cmd.DBP  = 0;
      CommandData.Trans_cmd.ROC  = 1;
      CommandData.Trans_cmd.SDAP = 0;
      CommandData.Trans_cmd.RNW  = I3C_CMD_READ;
      CommandData.Trans_cmd.TOC  = 1;
      CommandData.Trans_cmd.PEC  = 1;
      MmioWrite32 (Base + COMMAND_QUEUE_PORT, CommandData.Value);

      Private->ReadCount     -= RxFifoCount;
      Private->TransferCount -= RxFifoCount;

      // Wait for Transfer Complete
      Status = I3cWaitRespStat (Base);
      if (Status) {
        I3cAbort (Base);
        return Status;
      }

      // Read RESPONSE_QUEUE_PORT Reg
      Status = I3cReadRespQueue (Base);
      if (Status) {
        I3cAbort (Base);
        return Status;
      }

      for (Index = 0; Index < RxFifoCount; ) {
        Status = I3cWaitRxData (Base);
        if (Status) {
          I3cAbort (Base);
          return Status;
        }

        Value = MmioRead32 (Base + RX_DATA_PORT);
        for (i = 0; (i < sizeof (UINT32)) && (Index < RxFifoCount); i++, Index++) {
          *(Private->ReadData++) = (Value >> (i * 8)) & 0xff;
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
*  @brief This function waits for the RX FIFO to be available
*
*  @param Base Base address of I3cController
*
*  @returns EFI_STATUS
*  @retval EFI_TIMEOUT   Timeout while waiting for available number of entries in Rx Fifo
*  @retval EFI_SUCCESS   There are available number of entries in Rx Fifo
*/
EFI_STATUS
I3cWaitRxData (
  IN  UINT32   Base
  )
{
  I3C_INTR_STATUS  IntrStatus;
  UINT32           Timeout;

  Timeout = STATUS_WAIT_RETRY;

  // wait RX_THLD_STS to 1
  IntrStatus.Value = MmioRead32 (Base + INTR_STATUS);
  while (IntrStatus.Field.RX_THLD_STS == 0) {
    if (Timeout <= 0) {
      DEBUG ((EFI_D_ERROR, "I3cTimeout waiting for Receive Data.\n"));
      return EFI_TIMEOUT;
    }

    Timeout--;
    MicroSecondDelay (HYGON_I3C_MS_DELAY);
    IntrStatus.Value = MmioRead32 (Base + INTR_STATUS);
  }

  return EFI_SUCCESS;
}

/**
*  @brief This function waits for the TX FIFO to be available
*
*  @param Base Base address of I3cController
*
*  @returns EFI_STATUS
*  @retval EFI_TIMEOUT   Timeout while waiting for available number of entries in TX Fifo
*  @retval EFI_SUCCESS   There are available number of entries in TX Fifo
*/
EFI_STATUS
I3cWaitTxData (
  IN  UINT32   Base
  )
{
  I3C_INTR_STATUS  IntrStatus;
  UINT32           Timeout;

  Timeout = STATUS_WAIT_RETRY;

  // wait TX_THLD_STS to 1
  IntrStatus.Value = MmioRead32 (Base + INTR_STATUS);
  while (IntrStatus.Field.TX_THLD_STS == 0) {
    if (Timeout <= 0) {
      DEBUG ((EFI_D_ERROR, "I3cTimeout waiting for Transfer Data.\n"));
      return EFI_TIMEOUT;
    }

    Timeout--;
    MicroSecondDelay (HYGON_I3C_MS_DELAY);
  }

  return EFI_SUCCESS;
}
