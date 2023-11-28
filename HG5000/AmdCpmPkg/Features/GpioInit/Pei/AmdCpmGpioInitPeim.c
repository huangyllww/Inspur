/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM GPIO Initialization
 *
 * Contains code that initialized GPIO Init before memory init.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  GpioInit
 * @e \$Revision$   @e \$Date$
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

#include <AmdCpmPei.h>
#include EFI_PPI_DEFINITION (NbioPcieTrainingGen1CompleteFm15Ppi)

//extern EFI_GUID gEfiPeiPromotoryGpioPpiGuid;
EFI_GUID  gEfiPeiPromotoryGpioPpiGuid = { 0x31859c50, 0x2845, 0x42da, {0x9f, 0x37, 0xe7, 0x18, 0x67, 0xe3, 0xe0, 0x5e } };

VOID
AmdCpmGpioDeviceInit (
  IN      AMD_CPM_TABLE_PPI           *CpmTablePpiPtr,
  IN      UINT8                       InitFlag
  );

EFI_STATUS
AmdCpmGpioDeviceInitStage2 (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );

EFI_STATUS
EFIAPI
AmdCpmGpioInitPeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

EFI_STATUS
EFIAPI
AmdCpmPtGpioInitPeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

EFI_STATUS
EFIAPI
AmdCpmGpioInitPeimNotifyCallback (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );


STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mAmdCpmGpioInitPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCpmOverrideTablePpiGuid,
  AmdCpmGpioInitPeimNotifyCallback
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mAmdCpmGpioInitStage2PeimNotify =
{
  EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gAmdNbioPcieGen1TrainingCompleteFm15PpiGuid,
  AmdCpmGpioDeviceInitStage2
};

AMD_CPM_INIT_FLAG_TABLE gCpmInitFlagTable = {
  {CPM_SIGNATURE_INIT_FLAG, sizeof (gCpmInitFlagTable) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mCpmPtGpioInitPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiPromotoryGpioPpiGuid,
  AmdCpmPtGpioInitPeim
};

/*----------------------------------------------------------------------------------------*/
/**
 * Init Promontory GPIO pins
 *
 * This function initializes Promontory GPIO Device
 * after AGESA FCH Promotory Gpio PPI is installed.
 *
 * @param[in]     PeiServices       Pointer to Pei Services
 * @param[in]     NotifyDescriptor  The descriptor for the notification event
 * @param[in]     Ppi               Pointer to the PPI in question.
 *
 * @retval        EFI_SUCCESS       Module initialized successfully
 * @retval        EFI_ERROR         Initialization failed (see error for more details)
 *
 */
EFI_STATUS
AmdCpmPtGpioInitPeim (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN  VOID                      *Ppi
  )
{
  EFI_STATUS                    Status;
  AMD_CPM_TABLE_PPI             *CpmTablePtr;
  AMD_CPM_PT_GPIO_INIT_TABLE    *PtTablePtr;
  VOID                          *PtGpioTablePtr;
  EFI_PT_GPIO_PPI               *PtGpioPpi;

  CpmTablePtr = NULL;
  PtTablePtr = NULL;
  PtGpioTablePtr = NULL;
  PtGpioPpi = NULL;

  Status = (*PeiServices)->LocatePpi (
            (CPM_PEI_SERVICES**)PeiServices,
            &gAmdCpmTablePpiGuid,
            0,
            NULL,
            (VOID**)&CpmTablePtr
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (CpmTablePtr) {
    PtTablePtr = CpmTablePtr->CommonFunction.GetTablePtr (CpmTablePtr, CPM_SIGNATURE_PT_GPIO_INIT);

    PtGpioTablePtr = (VOID *)&(PtTablePtr->GpioCommon);

    if (PtGpioTablePtr) {
      Status = (*PeiServices)->LocatePpi (
                (CPM_PEI_SERVICES**)PeiServices,
                &gEfiPeiPromotoryGpioPpiGuid,
                0,
                NULL,
                (VOID**)&PtGpioPpi
                );
      if (EFI_ERROR (Status)) {
        return Status;
      }

      //
      // EFI_STATUS
      // PTGpioInitialPei (
      //   IN EFI_PEI_SERVICES  **PeiServices,
      //   IN VOID              *PTGpioPtr
      //   );
      //
      Status = PtGpioPpi->PTInitiialGpio (
                PeiServices,
                PtGpioTablePtr
                );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    } // End of if (PtGpioTablePtr)
  } // End of if (CpmTablePtr)

  ////
  //// This is a sample code to do output low in Promontory GPIO pin 1.
  //// EFI_STATUS
  //// PTGpioWritePei (
  ////   IN EFI_PEI_SERVICES  **PeiServices,
  ////   IN UINT8             Pin,
  ////   IN UINT8             Value
  ////   );
  ////
  // Status = PtGpioPpi->PTWriteGpio (
  //           PeiServices,
  //           1,
  //           0
  //           );
  // if (EFI_ERROR (Status)) {
  //   return Status;
  // }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * CPM GPIO Device Init Stage 2
 *
 * This function initializes GPIO Device in stage 2
 * after AGESA MEM PPI is installed.
 *
 * @param[in]     PeiServices    Pointer to Pei Services
 * @param[in]     NotifyDesc     The descriptor for the notification event
 * @param[in]     InvokePpi      Pointer to the PPI in question.
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 *
 */
EFI_STATUS
AmdCpmGpioDeviceInitStage2 (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  EFI_STATUS                  Status;
  AMD_CPM_TABLE_PPI           *CpmTablePtr;

  Status = (*PeiServices)->LocatePpi (
                              (CPM_PEI_SERVICES**)PeiServices,
                              &gAmdCpmTablePpiGuid,
                              0,
                              NULL,
                              (VOID**)&CpmTablePtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  AmdCpmGpioDeviceInit (CpmTablePtr, GPIO_DEVICE_INIT_STAGE_2);

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * The Function to init GPIO pins
 *
 * @param[in]     CpmTablePpiPtr  Pointer to AMD CPM Table Ppi
 *
 */

VOID
AmdCpmPeiGpioInit (
  IN      AMD_CPM_TABLE_PPI       *CpmTablePpiPtr
  )
{
  AMD_CPM_GPIO_INIT_TABLE         *GpioTablePtr;
  AMD_CPM_GPIO_ITEM               *TablePtr;

  if (CpmTablePpiPtr) {
    GpioTablePtr     = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_GPIO_INIT);

    if (GpioTablePtr) {
      TablePtr = &GpioTablePtr->GpioList[0];
      while (TablePtr->Pin != 0xFF ) {
        CpmTablePpiPtr->CommonFunction.SetGpio (CpmTablePpiPtr, TablePtr->Pin, TablePtr->Setting.Raw);
        TablePtr++;
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * The Function to pre-process GPIO Device tables
 *
 * @param[in]     CpmTablePpiPtr    Pointer to AMD CPM Table Ppi
 * @param[in]     ConfigTablePtr    Pointer to AMD CPM GPIO Device Config Table
 * @param[in]     PowerTablePtr     Pointer to AMD CPM GPIO Device Power Table
 * @param[in]     DetectionTablePtr Pointer to AMD CPM GPIO Device Detection Table
 * @param[in]     ResetTablePtr     Pointer to AMD CPM GPIO Device Reset Table
 * @param[in]     InitFlagTablePtr  Pointer to AMD CPM Init Flag Table
 * @param[in]     InitFlag          Init Flag
 *
 */

UINT32
CpmGpioDevicePreInit (
  IN      AMD_CPM_TABLE_PPI                   *CpmTablePpiPtr,
  IN      AMD_CPM_GPIO_DEVICE_CONFIG_TABLE    *ConfigTablePtr,
  IN      AMD_CPM_GPIO_DEVICE_POWER_TABLE     *PowerTablePtr,
  IN      AMD_CPM_GPIO_DEVICE_DETECTION_TABLE *DetectionTablePtr,
  IN      AMD_CPM_GPIO_DEVICE_RESET_TABLE     *ResetTablePtr,
  IN      AMD_CPM_INIT_FLAG_TABLE             *InitFlagTablePtr,
  IN      UINT8                               InitFlag
  )
{
  AMD_CPM_GPIO_DEVICE_CONFIG          *TablePtr;
  UINT8                               Index;
  UINT32                              ResetStall;
  UINT8                               GpioDeviceId;

  ResetStall = 0;
  if (ConfigTablePtr) {
    if (InitFlag <= GPIO_DEVICE_INIT_STAGE_1) {
      for (Index = 0; Index < AMD_GPIO_DEVICE_RESET_SIZE; Index ++) {
        if (ResetTablePtr && InitFlagTablePtr) {
          InitFlagTablePtr->ResetInitFlag[Index] = ResetTablePtr->DeviceResetList[Index].InitFlag;
        }
      }
      for (Index = 0; Index < AMD_GPIO_DEVICE_POWER_SIZE; Index ++) {
        if (PowerTablePtr && InitFlagTablePtr) {
          InitFlagTablePtr->PowerInitFlag[Index] = PowerTablePtr->DevicePowerList[Index].InitFlag;
        }
      }
      TablePtr = &ConfigTablePtr->DeviceList[0];
      while (TablePtr->DeviceId != 0xFF && TablePtr->DeviceId != 0x00) {
        if (CpmTablePpiPtr && TablePtr->Config.Setting.Enable == 2) {
          TablePtr->Config.Setting.Enable = CpmTablePpiPtr->CommonFunction.DetectDevice (CpmTablePpiPtr, TablePtr->DeviceId, NULL);
        }
        if (ResetTablePtr && TablePtr->Config.Setting.Enable < 2) {
          for (Index = 0; Index < AMD_GPIO_DEVICE_RESET_SIZE; Index ++) {
            GpioDeviceId = ResetTablePtr->DeviceResetList[Index].DeviceId;
            if (GpioDeviceId == 0xFF || GpioDeviceId == 0x00) {
              break;
            }
            if (InitFlagTablePtr && GpioDeviceId == TablePtr->DeviceId) {
              if (ResetTablePtr->DeviceResetList[Index].Mode == 0) {
                if ((InitFlagTablePtr->ResetInitFlag[Index] <= GPIO_DEVICE_INIT_STAGE_1) || !(TablePtr->Config.Setting.Enable & TablePtr->Config.Setting.ResetAssert)) {
                  InitFlagTablePtr->ResetInitFlag[Index] = TablePtr->Config.Setting.Enable & TablePtr->Config.Setting.ResetAssert;
                }
              } else if (ResetTablePtr->DeviceResetList[Index].Mode == 1) {
                if ((InitFlagTablePtr->ResetInitFlag[Index] <= GPIO_DEVICE_INIT_STAGE_1) || !(TablePtr->Config.Setting.Enable & TablePtr->Config.Setting.ResetDeassert)) {
                  InitFlagTablePtr->ResetInitFlag[Index] = TablePtr->Config.Setting.Enable & TablePtr->Config.Setting.ResetDeassert;
                }
              } else if (ResetTablePtr->DeviceResetList[Index].Mode == 2) {
                if ((InitFlagTablePtr->ResetInitFlag[Index] <= GPIO_DEVICE_INIT_STAGE_1) || !(TablePtr->Config.Setting.Enable & TablePtr->Config.Setting.ResetDeassert)) {
                  InitFlagTablePtr->ResetInitFlag[Index] = TablePtr->Config.Setting.Enable & TablePtr->Config.Setting.ResetDeassert;
                }
              }
            }
          }
        }
        if (PowerTablePtr && TablePtr->Config.Setting.Enable < 2) {
          for (Index = 0; Index < AMD_GPIO_DEVICE_POWER_SIZE; Index ++) {
            GpioDeviceId = PowerTablePtr->DevicePowerList[Index].DeviceId;
            if (GpioDeviceId == 0xFF || GpioDeviceId == 0x00) {
              break;
            }
            if (InitFlagTablePtr && GpioDeviceId == TablePtr->DeviceId) {
              if (PowerTablePtr->DevicePowerList[Index].Mode == TablePtr->Config.Setting.Enable) {
                if (InitFlagTablePtr->PowerInitFlag[Index] <= GPIO_DEVICE_INIT_STAGE_1) {
                  InitFlagTablePtr->PowerInitFlag[Index] = 1;
                }
              } else {
                InitFlagTablePtr->PowerInitFlag[Index] = 0;
              }
            }
          }
        }
        TablePtr++;
      }
    }
    if (ResetTablePtr) {
      for (Index = 0; Index < AMD_GPIO_DEVICE_RESET_SIZE; Index ++) {
        GpioDeviceId = ResetTablePtr->DeviceResetList[Index].DeviceId;
        if (GpioDeviceId == 0xFF || GpioDeviceId == 0x00) {
          break;
        }
        if (ResetTablePtr->DeviceResetList[Index].Mode == 2 && InitFlagTablePtr->ResetInitFlag[Index] == InitFlag) {
          ResetStall = ResetTablePtr->DeviceResetList[Index].Config.Stall;
        }
      }
    }
  }
  return ResetStall;
}

/*----------------------------------------------------------------------------------------*/
/**
 * The Function to toggle one GPIO pin to reset the device
 *
 * @param[in]     CpmTablePpiPtr  Pointer to AMD CPM Table Ppi
 * @param[in]     ResetItemPtr    Pointer to AMD CPM GPIO Device Reset item.
 *
 */

VOID
CpmGpioDeviceReset (
  IN      AMD_CPM_TABLE_PPI           *CpmTablePpiPtr,
  IN      AMD_CPM_GPIO_DEVICE_RESET   *ResetItemPtr
  )
{
  if (CpmTablePpiPtr && ResetItemPtr) {
    switch (ResetItemPtr->Type) {
    case 0:
      CpmTablePpiPtr->CommonFunction.SetGpio (
                                              CpmTablePpiPtr,
                                              ResetItemPtr->Config.Gpio.Pin,
                                              ResetItemPtr->Config.Gpio.Value
                                             );
      break;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * The Function to init on-board device.
 *
 * @param[in]     CpmTablePpiPtr  Pointer to AMD CPM Table Ppi
 * @param[in]     InitFlag        Init flag. 1: In stage1 2. In Stage2
 *
 */

VOID
AmdCpmGpioDeviceInit (
  IN      AMD_CPM_TABLE_PPI       *CpmTablePpiPtr,
  IN      UINT8                   InitFlag
  )
{
  AMD_CPM_GPIO_DEVICE_CONFIG_TABLE    *ConfigTablePtr;
  AMD_CPM_GPIO_DEVICE_POWER_TABLE     *PowerTablePtr;
  AMD_CPM_GPIO_DEVICE_DETECTION_TABLE *DetectionTablePtr;
  AMD_CPM_GPIO_DEVICE_RESET_TABLE     *ResetTablePtr;
  AMD_CPM_INIT_FLAG_TABLE             *InitFlagTablePtr;
  UINT8                               Index;
  UINT32                              ResetStall;
  UINT8                               GpioDeviceId;

  if (CpmTablePpiPtr) {
    ConfigTablePtr      = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_GPIO_DEVICE_CONFIG);
    PowerTablePtr       = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_GPIO_DEVICE_POWER);
    DetectionTablePtr   = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_GPIO_DEVICE_DETECTION);
    ResetTablePtr       = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_GPIO_DEVICE_RESET);
    if (InitFlag == GPIO_DEVICE_INIT_STAGE_1) {
      InitFlagTablePtr  = CpmTablePpiPtr->CommonFunction.AddTable (CpmTablePpiPtr, &gCpmInitFlagTable);
    } else {
      InitFlagTablePtr  = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_INIT_FLAG);
    }
    ResetStall = CpmGpioDevicePreInit (CpmTablePpiPtr, ConfigTablePtr, PowerTablePtr, DetectionTablePtr, ResetTablePtr, InitFlagTablePtr, InitFlag);

    if (ResetTablePtr) {
      for (Index = 0; Index < AMD_GPIO_DEVICE_RESET_SIZE; Index ++) {
        GpioDeviceId = ResetTablePtr->DeviceResetList[Index].DeviceId;
        if (GpioDeviceId == 0xFF || GpioDeviceId == 0x00) {
          break;
        }
        if (ResetTablePtr->DeviceResetList[Index].Mode == 0 && InitFlagTablePtr->ResetInitFlag[Index] == InitFlag) {
          CpmGpioDeviceReset (CpmTablePpiPtr, &ResetTablePtr->DeviceResetList[Index]);
        }
      }
    }
    if (PowerTablePtr) {
      for (Index = 0; Index < AMD_GPIO_DEVICE_POWER_SIZE; Index ++) {
        GpioDeviceId = PowerTablePtr->DevicePowerList[Index].DeviceId;
        if (GpioDeviceId == 0xFF || GpioDeviceId == 0x00) {
          break;
        }
        if (InitFlagTablePtr->PowerInitFlag[Index] == InitFlag) {
          switch (PowerTablePtr->DevicePowerList[Index].Type) {
          case 0:
            CpmTablePpiPtr->CommonFunction.SetGpio (
                                CpmTablePpiPtr,
                                PowerTablePtr->DevicePowerList[Index].Config.SetGpio.Pin,
                                PowerTablePtr->DevicePowerList[Index].Config.SetGpio.Value
                                );
            break;
          case 1:
            while (CpmTablePpiPtr->CommonFunction.GetGpio (CpmTablePpiPtr, PowerTablePtr->DevicePowerList[Index].Config.WaitGpio.Pin) !=
                   PowerTablePtr->DevicePowerList[Index].Config.WaitGpio.Value) {
            }
            break;
          case 2:
            CpmTablePpiPtr->CommonFunction.Stall (CpmTablePpiPtr, PowerTablePtr->DevicePowerList[Index].Config.Stall);
            break;
          }
        }
      }
    }
    if (ResetTablePtr) {
      CpmTablePpiPtr->CommonFunction.Stall (CpmTablePpiPtr, ResetStall);
      for (Index = 0; Index < AMD_GPIO_DEVICE_RESET_SIZE; Index ++) {
        GpioDeviceId = ResetTablePtr->DeviceResetList[Index].DeviceId;
        if (GpioDeviceId == 0xFF || GpioDeviceId == 0x00) {
          break;
        }
        if (ResetTablePtr->DeviceResetList[Index].Mode == 1 && InitFlagTablePtr->ResetInitFlag[Index] == InitFlag) {
          CpmGpioDeviceReset (CpmTablePpiPtr, &ResetTablePtr->DeviceResetList[Index]);
        }
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * The Function to reset on-board device.
 *
 * @param[in]     This          Pointer to AMD CPM Table Ppi
 * @param[in]     ResetId       Device Id of on-board device
 * @param[in]     ResetControl  Reset Control Flag. 0: Reset assert. 1: Reset deassert
 */

VOID
AmdCpmResetDevice (
  IN      VOID                        *This,
  IN      UINT8                       ResetId,
  IN      UINT8                       ResetControl
  )
{
  AMD_CPM_TABLE_PPI                   *CpmTablePpiPtr = This;
  AMD_CPM_GPIO_DEVICE_RESET_TABLE     *ResetTablePtr;
  UINT8                               Index;
  UINT8                               GpioDeviceId;

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpGpioInitPeimResetDeviceBegin);

  ResetTablePtr = 0;
  if (CpmTablePpiPtr->MainTablePtr->Header.TableSignature == CPM_SIGNATURE_MAIN_TABLE) {
    ResetTablePtr   = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_GPIO_DEVICE_RESET);
  } else {
    CpmTablePpiPtr->CommonFunction.PostCode (CpmTpErrorMainTablePointerInvalid);
    CPM_DEADLOOP ();
  }
  if (ResetTablePtr) {
    for (Index = 0; Index < AMD_GPIO_DEVICE_RESET_SIZE; Index ++) {
      GpioDeviceId = ResetTablePtr->DeviceResetList[Index].DeviceId;
      if (GpioDeviceId == 0xFF || GpioDeviceId == 0x00) {
        break;
      }
      if ((GpioDeviceId == ResetId) && (ResetTablePtr->DeviceResetList[Index].Mode == ResetControl)) {
        CpmGpioDeviceReset (CpmTablePpiPtr, &ResetTablePtr->DeviceResetList[Index]);
        break;
      }
    }
  }
  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpGpioInitPeimResetDeviceEnd);
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set Memory Voltage
 *
 * This function sets GPIO pins to control memory volatge according to memory modules to
 * be plugged in.
 *
 * @param[in]     This      Pointer to AMD CPM Table Ppi
 * @param[in]     Voltage   Memory voltage to be set. 1: 1.5 Volt. 2: 1.35 Volt. 3: 1.25 Volt
 *
 */

VOID
AmdCpmSetMemVoltage (
  IN      VOID                        *This,
  IN      UINT8                       Voltage
  )
{
  AMD_CPM_GPIO_MEM_VOLTAGE_TABLE      *TablePtr;
  UINT8                               Index;
  AMD_CPM_TABLE_PPI                   *CpmTablePpiPtr;

  CpmTablePpiPtr = This;
  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpGpioInitPeimSetMemVoltageBegin);

  TablePtr = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_SET_MEM_VOLTAGE);
  if (TablePtr) {
    for (Index = 0; Index < AMD_GPIO_MEM_VOLTAGE_SIZE; Index++) {
      if (TablePtr->Item[Index].Voltage == 0xFF) {
        break;
      } else if (TablePtr->Item[Index].Voltage == Voltage) {
        if (TablePtr->Item[Index].GpioPin1 != 0xFF) {
          CpmTablePpiPtr->CommonFunction.SetGpio (CpmTablePpiPtr, TablePtr->Item[Index].GpioPin1, TablePtr->Item[Index].Value1);
        }
        if (TablePtr->Item[Index].GpioPin2 != 0xFF) {
          CpmTablePpiPtr->CommonFunction.SetGpio (CpmTablePpiPtr, TablePtr->Item[Index].GpioPin2, TablePtr->Item[Index].Value2);
        }
      }
    }
  }

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpGpioInitPeimSetMemVoltageEnd);

  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set VDDP/VDDR Voltage
 *
 * This function sets GPIO pins to control VDDP/VDDR volatge according to the fuse value
 *
 * @param[in]     This      Pointer to AMD CPM Table Ppi
 * @param[in]     VddpVddrVoltage VDDP/VDDR voltage to be set. 0: 0.95 Volt. 1: 1.05 Volt.
 *
 */

VOID
AmdCpmSetVddpVddrVoltage (
  IN      VOID                        *This,
  IN      VDDP_VDDR_VOLTAGE           VddpVddrVoltage
  )
{
  AMD_CPM_GPIO_VDDP_VDDR_VOLTAGE_TABLE  *TablePtr;
  UINT8                                 Index;
  AMD_CPM_TABLE_PPI                     *CpmTablePpiPtr;

  CpmTablePpiPtr = This;
  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpGpioInitPeimSetVddpVddrVoltageBegin);

  TablePtr = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_SET_VDDP_VDDR_VOLTAGE);
  if (TablePtr && VddpVddrVoltage.IsValid) {
    for (Index = 0; Index < AMD_GPIO_VDDP_VDDR_VOLTAGE_SIZE; Index++) {
      if (TablePtr->Item[Index].Voltage == 0xFF) {
        break;
      } else if (TablePtr->Item[Index].Voltage == VddpVddrVoltage.Voltage) {
        if (TablePtr->Item[Index].GpioPin1 != 0xFF) {
          CpmTablePpiPtr->CommonFunction.SetGpio (CpmTablePpiPtr, TablePtr->Item[Index].GpioPin1, TablePtr->Item[Index].Value1);
        }
      }
    }
  }

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpGpioInitPeimSetVddpVddrVoltageEnd);

  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * CPM External ClkGen Init
 *
 * This function initializes external Clock Generator in early post
 *
 * @param[in]     CpmTablePpiPtr  Pointer to AMD CPM Table Ppi
 *
 */

VOID
AmdCpmPeiExtClkGenInit (
  IN      AMD_CPM_TABLE_PPI       *CpmTablePpiPtr
  )
{
  AMD_CPM_EXT_CLKGEN_TABLE        *ExtClkGenTablePtr;
  AMD_CPM_EXT_CLKGEN_ITEM         *TablePtr;
  UINT8                           Data;

  if (!(CpmTablePpiPtr->CommonFunction.GetStrap (CpmTablePpiPtr) & BIT1)) {
    ExtClkGenTablePtr   = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_EXT_CLKGEN);
    if (ExtClkGenTablePtr) {
      TablePtr = &ExtClkGenTablePtr->Item[0];
      while (TablePtr->Function != 0xFF ) {
        if (TablePtr->Function == CpmTablePpiPtr->MainTablePtr->ExtClkGen) {
          CpmTablePpiPtr->CommonFunction.ReadSmbusBlock (
                                                  CpmTablePpiPtr,
                                                  ExtClkGenTablePtr->SmbusSelect,
                                                  ExtClkGenTablePtr->SmbusAddress,
                                                  TablePtr->Offset,
                                                  1,
                                                  &Data
                                                  );
          Data = (Data & TablePtr->AndMask) | TablePtr->OrMask;
          CpmTablePpiPtr->CommonFunction.WriteSmbusBlock (
                                                  CpmTablePpiPtr,
                                                  ExtClkGenTablePtr->SmbusSelect,
                                                  ExtClkGenTablePtr->SmbusAddress,
                                                  TablePtr->Offset,
                                                  1,
                                                  &Data
                                                  );
        }
        TablePtr++;
      }
    }
  }
}

VOID
AmdCpmPeiAddOneItemInClkList (
  IN      AMD_CPM_EXT_CLKGEN_TABLE    *ExtClkGenTablePtr,
  IN      AMD_CPM_EXT_CLKGEN_ITEM     *ListPtr,
  IN      UINT8                       *ListNum,
  IN      UINT8                       ClkNum,
  IN      UINT8                       ClkReq
  )
{
  AMD_CPM_EXT_CLKGEN_ITEM     *CurrentItemPtr;
  UINT8   Index;
  UINT8   Index2;
  if (ClkNum == 0xFE) {
    return;
  }
  for (Index = 0; Index < AMD_EXT_CLKGEN_SIZE; Index++) {
    if (ExtClkGenTablePtr->Item[Index].Function == 0xFF) {
      break;
    }
    if (ExtClkGenTablePtr->Item[Index].Function == ClkNum + 0x80 && ClkReq == 0) {
      CurrentItemPtr = &ExtClkGenTablePtr->Item[Index];
    } else if (ExtClkGenTablePtr->Item[Index].Function == ClkReq + 0x8F && ClkReq != 0xFF && ClkReq != 0) {
      CurrentItemPtr = &ExtClkGenTablePtr->Item[Index];
    } else {
      continue;
    }
    for (Index2 = 0; Index2 < (*ListNum); Index2++) {
      if (ListPtr[Index2].Offset == CurrentItemPtr->Offset) {
        ListPtr[Index2].AndMask &= CurrentItemPtr->AndMask;
        ListPtr[Index2].OrMask |= CurrentItemPtr->OrMask;
        break;
      }
    }
    if (Index2 == (*ListNum)) {
      ListPtr[Index2].Function = 0;
      ListPtr[Index2].Offset = CurrentItemPtr->Offset;
      ListPtr[Index2].AndMask = CurrentItemPtr->AndMask;
      ListPtr[Index2].OrMask = CurrentItemPtr->OrMask;
      (*ListNum) ++;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * CPM External PCIe Clock Init
 *
 * This function initializes external Clock Generator for PCIe clock.
 *
 * @param[in]     CpmTablePpiPtr  Pointer to AMD CPM Table Ppi
 *
 */
VOID
AmdCpmPcieExtClockInit (
  IN      AMD_CPM_TABLE_PPI       *CpmTablePpiPtr
  )
{
  AMD_CPM_PCIE_CLOCK_TABLE        *CpmPcieClockTablePtr;
  AMD_CPM_PCIE_CLOCK_ITEM         *TablePtr;
  UINT8                           SlotStatus;
  BOOLEAN                         ClockType;
  AMD_CPM_EXT_CLKGEN_TABLE        *ExtClkGenTablePtr;
  AMD_CPM_EXT_CLKGEN_ITEM         ClkGen[AMD_PCIE_CLOCK_SIZE];
  UINT8                           Index;
  UINT8                           Data;
  UINT8                           i;
  UINT32                          PcieSlotStatus;

  ExtClkGenTablePtr = NULL;
  Index = 0;
  PcieSlotStatus = 0;

  if (CpmTablePpiPtr->CommonFunction.GetBootMode () != CPM_BOOT_MODE_S3) {
    return;
  }

  CpmPcieClockTablePtr  = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_PCIE_CLOCK);
  ClockType             = (BOOLEAN) CpmTablePpiPtr->CommonFunction.GetStrap (CpmTablePpiPtr) & BIT1;

  ExtClkGenTablePtr     = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_EXT_CLKGEN);
  if (!ExtClkGenTablePtr || ClockType) {
    return;
  }

  if (CpmPcieClockTablePtr && CpmTablePpiPtr->MainTablePtr->UnusedGppClkOffEn) {
    i = 0;
    TablePtr = &CpmPcieClockTablePtr->Item[0];
    while (TablePtr->ClkId != 0xFF) {
      SlotStatus = 1;
      if (TablePtr->SlotCheck & BIT1) {
        SlotStatus = CpmTablePpiPtr->CommonFunction.DetectDevice (CpmTablePpiPtr, TablePtr->DeviceId, NULL);
      }
      if (SlotStatus) {
        AmdCpmPeiAddOneItemInClkList (ExtClkGenTablePtr, &ClkGen[0], &Index, TablePtr->ClkIdExt, TablePtr->ClkReqExt);
      } else {
        AmdCpmPeiAddOneItemInClkList (ExtClkGenTablePtr, &ClkGen[0], &Index, TablePtr->ClkIdExt, 0);
      }
      TablePtr++;
      i++;
    }
    i = 0;
    while (Index > 0) {
      if (CpmTablePpiPtr->CommonFunction.ReadSmbusBlock) {
        CpmTablePpiPtr->CommonFunction.ReadSmbusBlock (
                                              CpmTablePpiPtr,
                                              ExtClkGenTablePtr->SmbusSelect,
                                              ExtClkGenTablePtr->SmbusAddress,
                                              ClkGen[i].Offset,
                                              1,
                                              &Data
                                              );
        Data = (Data & ClkGen[i].AndMask) | ClkGen[i].OrMask;
        CpmTablePpiPtr->CommonFunction.WriteSmbusBlock (
                                              CpmTablePpiPtr,
                                              ExtClkGenTablePtr->SmbusSelect,
                                              ExtClkGenTablePtr->SmbusAddress,
                                              ClkGen[i].Offset,
                                              1,
                                              &Data
                                              );
      }
      Index --;
      i ++;
    }
  }
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * CPM GPIO Device Init
 *
 * This function registers the public functions which will be used outside of CPM,
 * initializes GPIO Device stage 1 after AMD PEI INIT RESET PPI is installed.
 *
 * @param[in]     PeiServices       Pointer to Pei Services
 * @param[in]     NotifyDescriptor  The descriptor for the notification event
 * @param[in]     Ppi               Pointer to the PPI in question.
 *
 * @retval        EFI_SUCCESS       Module initialized successfully
 * @retval        EFI_ERROR         Initialization failed (see error for more details)
 *
 */
EFI_STATUS
EFIAPI
AmdCpmGpioInitPeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  EFI_STATUS                      Status;
  AMD_CPM_TABLE_PPI               *CpmTablePpiPtr;
  AMD_CPM_GPIO_INIT_FINISHED_PPI  *CpmGpioInitFinishedPpiPtr;
  EFI_PEI_PPI_DESCRIPTOR          *PpiListCpmGpioInitFinishedPtr;
  AMD_CPM_PCIE_CLOCK_TABLE        *CpmPcieClockTablePtr;
  AMD_CPM_PCIE_CLOCK_ITEM         *TablePtr;
  UINT8                           BootStatus;

  Status = (*PeiServices)->LocatePpi (
                              (CPM_PEI_SERVICES**)PeiServices,
                              &gAmdCpmTablePpiGuid,
                              0,
                              NULL,
                              (VOID**)&CpmTablePpiPtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpGpioInitPeimDriverBegin);

  CpmTablePpiPtr->CommonFunction.LoadPreInitTable (CpmTablePpiPtr, CPM_PRE_INIT_STAGE_1);

  AmdCpmPeiGpioInit (CpmTablePpiPtr);
  AmdCpmGpioDeviceInit (CpmTablePpiPtr, GPIO_DEVICE_INIT_STAGE_1);
  AmdCpmPeiExtClkGenInit (CpmTablePpiPtr);
  AmdCpmPcieExtClockInit (CpmTablePpiPtr);

  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmPtGpioInitPeimNotify);

  CpmTablePpiPtr->PeimPublicFunction.SetMemVoltage    = AmdCpmSetMemVoltage;
  CpmTablePpiPtr->PeimPublicFunction.SetVddpVddrVoltage = AmdCpmSetVddpVddrVoltage;
  CpmTablePpiPtr->CommonFunction.ResetDevice          = AmdCpmResetDevice;

  CpmPcieClockTablePtr    = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_PCIE_CLOCK);

  BootStatus = CpmTablePpiPtr->CommonFunction.GetBootMode ();
  if (CpmPcieClockTablePtr && (CpmTablePpiPtr->CommonFunction.GetStrap (CpmTablePpiPtr) & BIT1)) {
    TablePtr = &CpmPcieClockTablePtr->Item[0];
    while (TablePtr->ClkId != 0xFF) {
      if (TablePtr->ClkId != 0xFE) {
        CpmTablePpiPtr->CommonFunction.SetClkReq (CpmTablePpiPtr, TablePtr->ClkId, 0xFF);
      }
      TablePtr++;
    }
  }

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpGpioInitPeimDriverEnd);

  Status = (*PeiServices)->AllocatePool (
                              (CPM_PEI_SERVICES**)PeiServices,
                              sizeof (AMD_CPM_GPIO_INIT_FINISHED_PPI),
                              (VOID**)&CpmGpioInitFinishedPpiPtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = (*PeiServices)->AllocatePool (
                              (CPM_PEI_SERVICES**)PeiServices,
                              sizeof (EFI_PEI_PPI_DESCRIPTOR),
                              (VOID**)&PpiListCpmGpioInitFinishedPtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PpiListCpmGpioInitFinishedPtr->Flags   = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListCpmGpioInitFinishedPtr->Guid    = &gAmdCpmGpioInitFinishedPpiGuid;
  PpiListCpmGpioInitFinishedPtr->Ppi     = CpmGpioInitFinishedPpiPtr;

  Status = (*PeiServices)->InstallPpi (
                              (CPM_PEI_SERVICES**)PeiServices,
                              PpiListCpmGpioInitFinishedPtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM GPIO Init PEIM driver
 *
 * This function registers the functions to initialize GPIO pins, GEVENT pins, PCIe
 * reference clock and on-board devices.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */

EFI_STATUS
EFIAPI
AmdCpmGpioInitPeimEntryPoint (
  IN        CPM_PEI_FILE_HANDLE   FileHandle,
  IN        CPM_PEI_SERVICES      **PeiServices
  )
{
  EFI_STATUS                      Status;

  Status = (**PeiServices).NotifyPpi (PeiServices, &mAmdCpmGpioInitPeimNotify);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback Function of the AMD CPM GPIO Init PEIM driver
 *
 * This function registers the functions to initialize GPIO pins, GEVENT pins, PCIe
 * reference clock and on-board devices.
 *
 * @param[in]     PeiServices       The PEI core services table.
 * @param[in]     NotifyDescriptor  The descriptor for the notification event.
 * @param[in]     Ppi               Pointer to the PPI in question
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdCpmGpioInitPeimNotifyCallback (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_NOTIFY_DESCRIPTOR       NotifyDescriptorV9;
  VOID                            *PpiV9;

  PpiV9 = NULL;

  Status = AmdCpmGpioInitPeim ((EFI_PEI_SERVICES**)PeiServices,
                               &NotifyDescriptorV9,
                               PpiV9);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = (**PeiServices).NotifyPpi (PeiServices, &mAmdCpmGpioInitStage2PeimNotify);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}
