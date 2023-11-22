/* $NoKeywords:$ */

/**
 * @file
 *
 * IOHC SW SMI Dispatcher Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      Hgpi
 * @e sub-project:  IOHC
 *
 */
/******************************************************************************
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
****************************************************************************/
#include <HYGON.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/IdsLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmConfiguration.h>
#include <Protocol/SmmCpuIo2.h>
#include <Library/SmnAccessLib.h>
#include <Library/PciLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Protocol/FabricTopologyServices.h>
#include <GnbHsio.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioSmuLib.h>
#include "IohcSmmSwSmiDispatcher.h"
#include <Library/FchBaseLib.h>

NBIO_INFO                      mSysNbioInfo[16];
UINT8                          mSysNbioCount = 0;

IOHC_SMM_COMMUNICATION_BUFFER  *mIohcCommunicationBufferPtr;

IOHC_SMM_DISPATCHER_TABLE  mIohcDispatcherTableHyGx[] = {
  {
    SMU_HOTPLUG_SMI_FLAG,
    IohcSmmNVMeEqDispatchHandler
  },
  {
    SMU_GET_RTC_RAM_FLAG,
    SmuGetRtcRamDispatchHandler
  },
};

UINT8  mNumOfDispatcherTableEntry = sizeof (mIohcDispatcherTableHyGx) / sizeof (IOHC_SMM_DISPATCHER_TABLE);

/*----------------------------------------------------------------------------------------*/

/**
 * Clear All IOHC SW SMI status and enable IOHC SW SMI
 *
 *
 * @param[in]         void
 * @retval            void
 */
/*----------------------------------------------------------------------------------------*/
VOID ClearAndEnableIohcSwSmi (
    void
  ) 
{
  UINTN       SysNbioIndex;
  UINT32      Value;
  UINT32      NbRasCntlSmnAddr;
  UINT32      SwSmiCntlSmnAddr;
  UINT32      BusBase;

  for (SysNbioIndex = 0; SysNbioIndex < mSysNbioCount; SysNbioIndex++) {
    BusBase = mSysNbioInfo[SysNbioIndex].BusBase;
    NbRasCntlSmnAddr = LOCAL_IOD_NBIO_SPACE (mSysNbioInfo[SysNbioIndex].RbId, NB_RAS_CONTROL_HYGX);
    SwSmiCntlSmnAddr = LOCAL_IOD_NBIO_SPACE (mSysNbioInfo[SysNbioIndex].RbId, SW_SMI_CNTL_HYGX);
        
    // Disable IOHC SW SMI
    SmnRegisterRMW (BusBase, NbRasCntlSmnAddr, ~(UINT32)SW_SMI_EN, 0, 0);
    
    // Clear SW_SMI_CNTL_HYGX. Writing 1 to bit[x] inverts the value of bit[x].
    SmnRegisterRead (BusBase, SwSmiCntlSmnAddr, &Value);
    SmnRegisterWrite (BusBase, SwSmiCntlSmnAddr, &Value, 0);
    
    // Enable IOHC SW SMI
    SmnRegisterRMW (BusBase, NbRasCntlSmnAddr, ~(UINT32)SW_SMI_EN, SW_SMI_EN, 0);
  }
}
/*----------------------------------------------------------------------------------------*/

/**
 * Entry point of the IOHC SW SMI SMM dispatcher driver
 *
 * @param[in]     ImageHandle    Pointer to the firmware file system header
 * @param[in]     SystemTable    Pointer to System table
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
IohcSmmSwSmiDispatcherEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                             Status;
  EFI_HANDLE                             ApuRasHandle = NULL;
  FCH_SMM_APURAS_REGISTER_CONTEXT        ApuRasRegisterContext;
  FCH_SMM_APURAS_DISPATCH_PROTOCOL       *HygonApuRasDispatch;
  UINT8                                  SysNbioIndex;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;
  UINTN                                  SocketNumber;
  UINTN                                  PhysicalDieNumber;
  UINTN                                  LogicalDieNumber;
  UINTN                                  NbioNumber;
  UINTN                                  SocketId;
  UINTN                                  LogicalDieId;
  UINTN                                  RbId;
  UINTN                                  RbBusNum;
  EFI_HANDLE                             Handle;
  
  Status = gBS->LocateProtocol (
                  &gHygonFabricTopologyServicesProtocolGuid,
                  NULL,
                  &FabricTopology
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Locate gHygonFabricTopologyServicesProtocolGuid fail status (%r) \n", Status));
    return Status;
  }

  SysNbioIndex = 0;
  FabricTopology->GetSystemInfo (FabricTopology, &SocketNumber, NULL, NULL);
  for (SocketId = 0; SocketId < SocketNumber; SocketId++) {
    FabricTopology->GetProcessorInfo (FabricTopology, SocketId, &PhysicalDieNumber, &LogicalDieNumber, NULL);

    for (LogicalDieId = 0; LogicalDieId < LogicalDieNumber; LogicalDieId++) {
      FabricTopology->GetDieInfo(FabricTopology, SocketId, LogicalDieId, &NbioNumber, NULL, NULL);

      for (RbId = 0; RbId < NbioNumber; RbId++) {         
        FabricTopology->GetRootBridgeInfo (FabricTopology, SocketId, LogicalDieId, RbId, NULL, &RbBusNum, NULL);
        mSysNbioInfo[SysNbioIndex].SocketId = SocketId;
        mSysNbioInfo[SysNbioIndex].LogicalDieId = LogicalDieId;
        mSysNbioInfo[SysNbioIndex].RbId = RbId;
        mSysNbioInfo[SysNbioIndex].BusBase = (UINT32) RbBusNum;
        
        DEBUG ((EFI_D_INFO, "mSysNbioInfo[%d] Socket %d Logical Die %d Rb %d Bus 0x%X \n", SysNbioIndex, SocketId, LogicalDieId, RbId, RbBusNum));
        
        SysNbioIndex++;
        mSysNbioCount++;
      }
    }
  }
  DEBUG ((EFI_D_INFO, "mSysNbioCount = %d \n", mSysNbioCount));
  
  ClearAndEnableIohcSwSmi ();
  
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (IOHC_SMM_COMMUNICATION_BUFFER),
                    &mIohcCommunicationBufferPtr
                    );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Allocate pool for IOHC_SMM_COMMUNICATION_BUFFER fail status (%r) \n", Status));
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gFchSmmApuRasDispatchProtocolGuid,
                    NULL,
                    &HygonApuRasDispatch
                    );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Locate gFchSmmApuRasDispatchProtocolGuid fail status (%r) \n", Status));
    return Status;
  }

  // Only register handler for master Logical die
  ApuRasRegisterContext.Bus   = 0;
  ApuRasRegisterContext.Socket   = 0;
  ApuRasRegisterContext.LogicalDie   = 0;
  ApuRasRegisterContext.IohubPhysicalDie = (UINT8)FabricTopologyGetIohubPhysicalDieId (ApuRasRegisterContext.LogicalDie);
  ApuRasRegisterContext.Rb   = 0;
  ApuRasRegisterContext.Order = 0x1;

  Status = HygonApuRasDispatch->Register (
                                HygonApuRasDispatch,
                                IohcSwSmiDispatchHandler,
                                &ApuRasRegisterContext,
                                &ApuRasHandle
                                );
  
  DEBUG ((EFI_D_INFO, "Register IohcSwSmiDispatchHandler status (%r) \n", Status));
  
  NotifySmuSmiHandlerReady ();
  
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                                &Handle,
                                &gHygonIohcSwSmiReadyProtocolGuid,
                                EFI_NATIVE_INTERFACE,
                                NULL
                                );
  
  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * IOHC SMM dispatcher handler
 *
 *
 * @param[in]         DispatchHandle        Dispatcher Handle
 * @param[in]         RegisterContext       FCH APU RAS Register Context
 * @retval            EFI_SUCCESS           SMI handled by dispatcher
 * @retval            EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
IohcSwSmiDispatchHandler (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       FCH_SMM_APURAS_REGISTER_CONTEXT   *MiscRegisterContext
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT8       DispatcherIndex;
  UINT32      IohcSwSmiRegData;
  VOID        *CommBuffer;
  UINT8       SysNbioIndex;
  
  for (SysNbioIndex = 0; SysNbioIndex < mSysNbioCount; SysNbioIndex++) {
    SmnRegisterRead (mSysNbioInfo[SysNbioIndex].BusBase, 
                     LOCAL_IOD_NBIO_SPACE (mSysNbioInfo[SysNbioIndex].RbId, SW_SMI_CNTL_HYGX), 
                     &IohcSwSmiRegData);

    for (DispatcherIndex = 0; DispatcherIndex < mNumOfDispatcherTableEntry; DispatcherIndex++ ) {
      if (((IohcSwSmiRegData & mIohcDispatcherTableHyGx[DispatcherIndex].SmiStatusBit) != 0) &&
          (mIohcDispatcherTableHyGx[DispatcherIndex].SmiDispatcher != NULL)) {
        
        mIohcCommunicationBufferPtr->SmiStatusBit = mIohcDispatcherTableHyGx[DispatcherIndex].SmiStatusBit;
        mIohcCommunicationBufferPtr->SocketId = mSysNbioInfo[SysNbioIndex].SocketId;
        mIohcCommunicationBufferPtr->LogicalDieId = mSysNbioInfo[SysNbioIndex].LogicalDieId;
        mIohcCommunicationBufferPtr->RbId = mSysNbioInfo[SysNbioIndex].RbId;
        mIohcCommunicationBufferPtr->BusBase = mSysNbioInfo[SysNbioIndex].BusBase;
        CommBuffer = (VOID *)mIohcCommunicationBufferPtr;
        
        // 1. callback function
        Status = mIohcDispatcherTableHyGx[DispatcherIndex].SmiDispatcher (DispatchHandle, CommBuffer, NULL);

        // 2. Clear Smi Status bit. Writing 1 to bit[x] inverts the value of bit[x].
        SmnRegisterWrite (mSysNbioInfo[SysNbioIndex].BusBase, 
                         LOCAL_IOD_NBIO_SPACE (mSysNbioInfo[SysNbioIndex].RbId, SW_SMI_CNTL_HYGX), 
                         &(mIohcCommunicationBufferPtr->SmiStatusBit), 
                         0);
      }
    }
  }

  return Status;
}


/*----------------------------------------------------------------------------------------*/

/**
 * NVMe EQ Dispatch Handler
 *
 *
 * @param[in]         DispatchHandle      Dispatcher Handle
 * @param[in, out]    CommBuffer          Communication Buffer
 * @param[in, out]    CommBufferSize      Buffer size
 *
 * @retval            EFI_SUCCESS         Always Return Success
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
IohcSmmNVMeEqDispatchHandler (
  IN       EFI_HANDLE   DispatchHandle,
  IN OUT   VOID         *CommBuffer OPTIONAL,
  IN OUT   UINTN        *CommBufferSize OPTIONAL
  )
{  
  DEBUG ((EFI_D_INFO, "IohcSmmNVMeEqDispatchHandler Entry \n"));
  
  // Todo
  
  DEBUG ((EFI_D_INFO, "IohcSmmNVMeEqDispatchHandler Exit \n"));
  
  return EFI_SUCCESS;
}

/**
 * SMU Get RTC RAM value SMI handler
 *
 *
 * @param[in]         DispatchHandle      Dispatcher Handle
 * @param[in, out]    CommBuffer          Communication Buffer
 * @param[in, out]    CommBufferSize      Buffer size
 *
 * @retval            EFI_SUCCESS         Always Return Success
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
SmuGetRtcRamDispatchHandler (
  IN       EFI_HANDLE   DispatchHandle,
  IN OUT   VOID         *CommBuffer OPTIONAL,
  IN OUT   UINTN        *CommBufferSize OPTIONAL
  )
{
  PCI_ADDR                  PciAddr;
  UINT32                    SmuArg[6];
  EFI_STATUS                Status;
  RTCRAM_DATA               DefaultData = {{0,0,0,0,0,0}};
  RTCRAM_DATA               *RtcRamData = &DefaultData;
 
      
  DEBUG ((EFI_D_INFO, "SmuGetRtcRamDispatchHandler Entry \n"));
  
  ReadRtcRam24((UINT8*)RtcRamData);
  
  DumpRtcRam24(RtcRamData);
  
  NbioSmuServiceCommonInitArguments (SmuArg);
  CopyMem (SmuArg, RtcRamData, 24);
  
  PciAddr.AddressValue = 0;
  Status = NbioSmuServiceRequest(PciAddr, 0, SMC_MSG_RtcRecordDataReady, SmuArg, 0);
  
  DEBUG ((EFI_D_INFO, "SmuGetRtcRamDispatchHandler Exit \n"));
  
  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Notify SMU BIOS SMI handler ready
 *
 * @param[in]         void
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
VOID
NotifySmuSmiHandlerReady (
  void
  )
{
  PCI_ADDR                             PciAddr;
  UINT32                               SmuArg[6];
  EFI_STATUS                           Status;
  
  DEBUG ((EFI_D_INFO, "NotifySmuSmiHandlerReady Entry \n"));
  
  PciAddr.AddressValue = 0;
  NbioSmuServiceCommonInitArguments (SmuArg);
  SmuArg[0] = 0xAA55AA55;
  Status = NbioSmuServiceRequest(PciAddr, 0, SMC_MSG_SmiHandlerReady, SmuArg, 0);
  
  DEBUG ((EFI_D_INFO, "NotifySmuSmiHandlerReady Exit \n"));
}
