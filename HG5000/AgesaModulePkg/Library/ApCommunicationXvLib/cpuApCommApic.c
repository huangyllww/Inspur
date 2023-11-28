/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU APIC related utility functions.
 *
 * Contains code that provides mechanism to invoke and control APIC communication.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ******************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "Filecode.h"
#include "cpuRegisters.h"
#include "cpuApComm.h"
#include "Library/CcxBaseX86Lib.h"
#include "Library/CcxHaltLib.h"

#define FILECODE LIBRARY_APCOMMUNICATIONXVLIB_CPUAPCOMMAPIC_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Reads the 'control byte' on the designated remote core.
 *
 * This function will read the current contents of the control byte
 * on the designated core using the APIC remote read inter-
 * processor interrupt sequence.
 *
 * @param[in]      Socket        Socket number of the target core
 * @param[in]      Die           Die number of the target core
 * @param[in]      Complex       Complex number of the target core
 * @param[in]      Core          Core number of the target core
 * @param[in]      Thread        Thread number of the target core
 * @param[in]      StdHeader     Configuration parameters pointer
 *
 * @return         The current value of the remote cores control byte
 *
 */
UINT8
ApReadRemoteCtrlByte (
  IN       UINT8 Socket,
  IN       UINT8 Die,
  IN       UINT8 Complex,
  IN       UINT8 Core,
  IN       UINT8 Thread,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  ControlByte;
  UINT32 ApicRegister;
  UINT32 MyApicId;

  GetLocalApicIdForCore (Socket, Core, &MyApicId, StdHeader);
  ApicRegister = ApUtilRemoteRead (MyApicId, APIC_CTRL_DWORD, StdHeader);
  ControlByte = (UINT8) ((ApicRegister & APIC_CTRL_MASK) >> APIC_CTRL_SHIFT);
  return (ControlByte);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Writes the 'control byte' on the executing core.
 *
 * This function writes data to a local APIC offset used in inter-
 * processor communication.
 *
 * @param[in]       Value
 * @param[in]       StdHeader
 *
 */
VOID
ApWriteCtrlByte (
  IN       UINT8 Value,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 ApicRegister;

  ApicRegister = ApUtilLocalRead (APIC_CTRL_REG, StdHeader);
  ApicRegister = ((ApicRegister & ~APIC_CTRL_MASK) | (UINT32) (Value << APIC_CTRL_SHIFT));
  ApUtilLocalWrite (APIC_CTRL_REG, ApicRegister, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Reads the 'data dword' on the designated remote core.
 *
 * This function will read the current contents of the data dword
 * on the designated core using the APIC remote read inter-
 * processor interrupt sequence.
 *
 * @param[in]      Socket        Socket number of the target core
 * @param[in]      Die           Die number of the target core
 * @param[in]      Complex       Complex number of the target core
 * @param[in]      Core          Core number of the target core
 * @param[in]      Thread        Thread number of the target core
 * @param[in]      StdHeader     Configuration parameters pointer
 *
 * @return         The current value of the remote core's data dword
 *
 */
UINT32
ApReadRemoteDataDword (
  IN       UINT8 Socket,
  IN       UINT8 Die,
  IN       UINT8 Complex,
  IN       UINT8 Core,
  IN       UINT8 Thread,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 MyApicId;

  GetLocalApicIdForCore (Socket, Core, &MyApicId, StdHeader);
  return (ApUtilRemoteRead (MyApicId, APIC_DATA_DWORD, StdHeader));
}


/*---------------------------------------------------------------------------------------*/
/**
 * Writes the 'data dword' on the executing core.
 *
 * This function writes data to a local APIC offset used in inter-
 * processor communication.
 *
 * @param[in]      Value        Value to write
 * @param[in]      StdHeader    Configuration parameters pointer
 *
 */
VOID
ApWriteDataDword (
  IN       UINT32 Value,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  ApUtilLocalWrite (APIC_DATA_REG, Value, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Runs the given task on the specified local core.
 *
 * This function is used to invoke an AP to run a specified AGESA
 * procedure.  It can only be called by cores that have subordinate
 * APs -- the BSC at POST, or any socket-relative core 0s at Early.
 *
 * @param[in]      Socket       Socket number of the target core
 * @param[in]      Die          Die number of the target core
 * @param[in]      Complex      Complex number of the target core
 * @param[in]      Core         Core number of the target core
 * @param[in]      Thread       Thread number of the target core
 * @param[in]      TaskPtr      Function descriptor
 * @param[in]      StdHeader    Configuration parameters pointer
 *
 * @return         Return value of the task that the AP core ran,
 *                 or zero if the task was VOID.
 *
 */
UINT32
ApRunCodeOnAp (
  IN       UINT8 Socket,
  IN       UINT8 Die,
  IN       UINT8 Complex,
  IN       UINT8 Core,
  IN       UINT8 Thread,
  IN       AP_TASK *TaskPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  CoreId;
  UINT8  CurrentStatus;
  UINT8  WaitStatus[3];
  UINT32 ApFlags;
  UINT32 ReturnCode;
  UINT32 TargetApicId;
  AP_WAIT_FOR_STATUS WaitForStatus;

  ApFlags = 0;
  ReturnCode = 0;

  CoreId = ApUtilCalculateUniqueId (Socket, Core, StdHeader);

  GetLocalApicIdForCore (Socket, Core, &TargetApicId, StdHeader);

  if (TaskPtr->DataTransfer.DataSizeInDwords != 0) {
    ApFlags |= AP_TASK_HAS_INPUT;
    if (((TaskPtr->ExeFlags & RETURN_PARAMS) != 0) &&
        ((TaskPtr->DataTransfer.DataTransferFlags & DATA_IN_MEMORY) == 0)) {
      ApFlags |= AP_RETURN_PARAMS;
    }
  }

  if ((TaskPtr->ExeFlags & TASK_HAS_OUTPUT) != 0) {
    ApFlags |= AP_TASK_HAS_OUTPUT;
  }

  if ((TaskPtr->ExeFlags & END_AT_HLT) != 0) {
    ApFlags |= AP_END_AT_HLT;
  }

  WaitStatus[0] = CORE_IDLE;
  WaitStatus[1] = CORE_IDLE_HLT;
  WaitStatus[2] = CORE_UNAVAILABLE;
  WaitForStatus.Status = WaitStatus;
  WaitForStatus.NumberOfElements = 3;
  WaitForStatus.RetryCount = WAIT_INFINITELY;
  WaitForStatus.WaitForStatusFlags = WAIT_STATUS_EQUALITY;
  CurrentStatus = ApUtilWaitForCoreStatus (Socket, 0, 0, Core, 0, &WaitForStatus, StdHeader);

  if (CurrentStatus != CORE_UNAVAILABLE) {
    ApWriteDataDword (ApFlags, StdHeader);
    ApWriteCtrlByte (CoreId, StdHeader);

    if (CurrentStatus == CORE_IDLE_HLT) {
      ApUtilFireDirectedNmi (TargetApicId, StdHeader);
    }

    ApUtilTransmitPointer (Socket, Core, (VOID **) &TaskPtr->FuncAddress, StdHeader);

    if ((ApFlags & AP_TASK_HAS_INPUT) != 0) {
      ApUtilTransmitBuffer (Socket, Core, &TaskPtr->DataTransfer, StdHeader);
    }

    if ((TaskPtr->ExeFlags & WAIT_FOR_CORE) != 0) {
      if (((ApFlags & AP_TASK_HAS_INPUT) != 0) &&
          ((ApFlags & AP_RETURN_PARAMS) != 0) &&
          ((TaskPtr->DataTransfer.DataTransferFlags & DATA_IN_MEMORY) == 0)) {
        if (ApUtilReceiveBuffer (Socket, Core, &TaskPtr->DataTransfer, StdHeader) == AGESA_ERROR) {
          // There is not enough space to put the return data.  This should never occur.  If it
          // does, this would point to strange heap corruption.
          IDS_ERROR_TRAP;
        }
      }

      ApUtilWaitForCoreStatus (Socket, 0, 0, Core, 0, &WaitForStatus, StdHeader);
      if ((ApFlags & AP_TASK_HAS_OUTPUT) != 0) {
        ReturnCode = ApReadRemoteDataDword (Socket, 0, 0, Core, 0, StdHeader);
      }
    }
  } else {
    ReturnCode = 0;
  }
  return (ReturnCode);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Main entry point for all APs in the system.
 *
 * This routine puts the AP cores in an infinite loop in which the cores
 * will poll their masters, waiting to be told to perform a task.  At early,
 * all socket-relative core zeros will receive their tasks from the BSC.
 * All others will receive their tasks from the core zero of their local
 * processor.  At the end of AmdInitEarly, all cores will switch to receiving
 * their tasks from the BSC.
 *
 * @param[in]     StdHeader       Handle to config for library and services.
 *
 */
VOID
ApEntry (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8   RemoteCmd;
  UINT8   SourceSocket;
  UINT8   CommandStart;
  UINT32  ApFlags;
  UINT32  FuncType;
  UINT32  ReturnCode;
  UINT32  CurrentCore;
  UINT32  *InputDataPtr;
  UINT32  BscSocket;
  AP_FUNCTION_PTR FuncAddress;
  IDT_DESCRIPTOR IdtDesc[32];
  AP_DATA_TRANSFER DataTransferInfo;

  // Initialize local variables
  ReturnCode = 0;
  DataTransferInfo.DataTransferFlags = 0;
  InputDataPtr = NULL;

  // Determine the executing core's socket and core numbers
  CurrentCore = CcxGetInitialApicID (StdHeader);

  // Determine the BSC's socket number
  BscSocket = 0;

  // Setup Interrupt Descriptor Table for sleep mode
  ApUtilSetupIdtForHlt (&IdtDesc[2], StdHeader);

  // Indicate to the BSC that we have reached the tasking engine
  ApWriteCtrlByte (CORE_IDLE, StdHeader);

  if (CurrentCore == 0) {
    // Core 0s receive their tasks from the BSC
    SourceSocket = (UINT8) BscSocket;
  } else {
    // All non-zero cores receive their tasks from the core 0 of their socket
    SourceSocket = 0;
  }

  // Determine the unique value that the master will write when it has a task
  // for this core to perform.
  CommandStart = ApUtilCalculateUniqueId (
                   0,
                   (UINT8)CurrentCore,
                   StdHeader
                   );
  for (;;) {
    RemoteCmd = ApReadRemoteCtrlByte (SourceSocket, 0, 0, 0, 0, StdHeader);
    if (RemoteCmd == CommandStart) {
      ApFlags = ApReadRemoteDataDword (SourceSocket, 0, 0, 0, 0, StdHeader);

      ApUtilReceivePointer (SourceSocket, 0, (VOID **) &FuncAddress, StdHeader);

      FuncType = ApFlags & (UINT32) (AP_TASK_HAS_INPUT | AP_TASK_HAS_OUTPUT);
      if ((ApFlags & AP_TASK_HAS_INPUT) != 0) {
        DataTransferInfo.DataSizeInDwords = 0;
        DataTransferInfo.DataPtr = NULL;
        DataTransferInfo.DataTransferFlags = 0;
        if (ApUtilReceiveBuffer (SourceSocket, 0, &DataTransferInfo, StdHeader) == AGESA_ERROR) {
          // There is not enough space to put the input data on the heap.  Undefined behavior is about
          // to result.
          IDS_ERROR_TRAP;
        }
        InputDataPtr = (UINT32 *) DataTransferInfo.DataPtr;
      }
      ApWriteCtrlByte (CORE_ACTIVE, StdHeader);
      switch (FuncType) {
      case 0:
        FuncAddress.PfApTask (StdHeader);
        break;
      case AP_TASK_HAS_INPUT:
        FuncAddress.PfApTaskI (InputDataPtr, StdHeader);
        break;
      case AP_TASK_HAS_OUTPUT:
        ReturnCode = FuncAddress.PfApTaskO (StdHeader);
        break;
      case (AP_TASK_HAS_INPUT | AP_TASK_HAS_OUTPUT):
        ReturnCode = FuncAddress.PfApTaskIO (InputDataPtr, StdHeader);
        break;
      default:
        ReturnCode = 0;
        break;
      }
      if (((ApFlags & AP_RETURN_PARAMS) != 0)) {
        ApUtilTransmitBuffer (SourceSocket, 0, &DataTransferInfo, StdHeader);
      }
      if ((ApFlags & AP_TASK_HAS_OUTPUT) != 0) {
        ApWriteDataDword (ReturnCode, StdHeader);
      }
      if ((ApFlags & AP_END_AT_HLT) != 0) {
        RemoteCmd = CORE_IDLE_HLT;
      } else {
        ApWriteCtrlByte (CORE_IDLE, StdHeader);
      }
    }
    if (RemoteCmd == CORE_IDLE_HLT) {
      SourceSocket = (UINT8) BscSocket;
      ApWriteCtrlByte (CORE_IDLE_HLT, StdHeader);
      ExecuteHltInstruction (StdHeader);
      ApWriteCtrlByte (CORE_IDLE, StdHeader);
    }
  }
}
