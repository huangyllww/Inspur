/* $NoKeywords:$ */

/**
 * @file
 *
 * HygonNbioSmuDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioSmuDxe
 *
 */
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
 *
 *
 ******************************************************************************
 */
#include <PiDxe.h>
#include <Filecode.h>
#include <HygonNbioSmuDxe.h>
#include <Library/HygonBaseLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/HygonNbioPcieServicesProtocol.h>
#include <Protocol/HygonCcxProtocol.h>
#include <Protocol/MpService.h>
#include <Protocol/SocLogicalIdProtocol.h>
#include <GnbHsio.h>
#include <Guid/GnbPcieInfoHob.h>
#include <GnbRegisters.h>
#include <CddRegistersDm.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieConfigLib.h>
#include <IdsHookId.h>
#include <Library/HygonIdsHookLib.h>
#include <Guid/HobList.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioSmuLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/BaseLib.h>
#include <Library/HygonCapsuleLib.h>
#include <SmuFeaturesEnable.h>
#include <Library/BaseMemoryLib.h>
#include <Library/GnbLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/HygonPspHpobLib.h>
#include <Protocol/FabricTopologyServices.h>
#include "AcBtc.h"
#include "FanPolicy.h"
#include <IdsNvIdSAT.h>

#define FILECODE  NBIO_SMU_HYEX_HYGONNBIOSMUDXE_HYGONNBIOSMUDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */
extern FAN_POLICY_TABLE  DefaultDHM1FanTable[];

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define GNB_MB_TO_PAGE(a) ((a) * (SIZE_1MB / EFI_PAGE_SIZE))
#define MAX_CAC_WEIGHT_NUM  21
#define DF_CFGADDRESSCNTL_OFFSET  0x84

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL  mHygonCoreTopologyServicesProtocol = {
  SmuGetCoreTopologyOnCdd,
  SmuLaunchThread
};

HYGON_PROCESSOR_SN_PROTOCOL   mProcessorSNProtocol = {
  0x01,
  SmuGetProcessorSN
};

DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  mDxeHygonNbioSmuServicesProtocol = {
  DXE_HYGON_NBIO_SMU_SERVICES_DXE_REVISION,
  HygonSmuReadFuseByName,
  HygonSmuReadFuseByNumber,
  HygonSmuServiceRequest,
  HygonSmuRegisterRead,
  HygonSmuRegisterWrite,
  HygonSmuRegisterRMW,
  HygonSmuReadBrandString,
  HygonSmuReadCacWeights,
  HygonSmuReadThrottleThresholds
};

/**
 *---------------------------------------------------------------------------------------
 *  CallbackAfterCCxDxeDone
 *
 *  Description:
 *     notification event handler after gHygonCcxDxeInitCompleteProtocolGuid ready
 *  Parameters:
 *    @param[in]     Event      Event whose notification function is being invoked.
 *    @param[in]     *Context   Pointer to the notification function's context.
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
EFIAPI
CallbackAfterCcxDxeDone (
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  )
{
  EFI_STATUS                            Status;
  EFI_STATUS                            CalledStatus;
  PCIe_PLATFORM_CONFIG                  *PcieFromHob;
  PCIe_PLATFORM_CONFIG                  *Pcie;
  GNB_HANDLE                            *NbioHandle;
  UINT32                                SmuArg[6];
  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *NbioSmuServiceProtocol;
  DXE_HYGON_CCX_INIT_COMPLETE_PROTOCOL  *CcxDxeInitCompleteProtocol;
  VOID                                  *Registration;

  IDS_HDT_CONSOLE (MAIN_FLOW, "CallbackAfterCcxDxeDone First notify\n");
  Status = EFI_SUCCESS;
  //
  // Add more check to locate protocol after got event, because
  // the library will signal this event immediately once it is register
  // just in case it is already installed.
  //
  CalledStatus = gBS->LocateProtocol (
                        &gHygonCcxDxeInitCompleteProtocolGuid,
                        NULL,
                        &CcxDxeInitCompleteProtocol
                        );
  Status = (CalledStatus > Status) ? CalledStatus : Status;
  if (EFI_ERROR (CalledStatus)) {
    return;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "CallbackAfterCcxDxeDone Start\n");

  SmuDxeInitialize (&PcieFromHob);
  Pcie = PcieFromHob;

  CalledStatus = gBS->LocateProtocol (
                        &gHygonNbioSmuServicesProtocolGuid,
                        NULL,
                        &NbioSmuServiceProtocol
                        );
  Status = (CalledStatus > Status) ? CalledStatus : Status;
  IDS_HDT_CONSOLE (MAIN_FLOW, "Status = 0x%x\n", CalledStatus);

  NbioHandle = NbioGetHandle (Pcie);
  while (NbioHandle != NULL) {
    //
    // Step24. BIOS call BIOSSMC_MSG_EnableAllSmuFeatures with a bitmask indicating which SMU features to enable.
    // Masks are allocated in export/smu.h. Each mask bit should be linked to a CBS option to enable/Disable it.
    //
    NbioSmuServiceCommonInitArguments (SmuArg);

    //
    // Update PcdSmuFeatureControlDefines value from CBS.
    //
    SmuArg[0] = PcdGet32 (PcdSmuFeatureControlDefines);
    if ((PcdGet8 (PcdAcBtc)) || (PcdGet8 (PcdDcBtc))) {
      // Do not enable CC6 if we are going to run Ac or Dc BTC
      if (SmuArg[0] & FEATURE_CC6_MASK) {
        SmuArg[0] &= ~((UINT32)FEATURE_CC6_MASK);
      }
    }

    CalledStatus = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_EnableSmuFeatures, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
    IDS_HDT_CONSOLE (MAIN_FLOW, "AllSmuFeatureControlDefines = 0x%x\n", SmuArg[0]);
    IDS_HDT_CONSOLE (MAIN_FLOW, "Status = 0x%x\n", CalledStatus);

    NbioHandle = GnbGetNextSocketHandle (NbioHandle);
  }

  //
  // Step25. If AC BTC is enabled, runs the AC BTC flow.
  //

  IDS_HDT_CONSOLE (MAIN_FLOW, "AcBtc \n");
  if ((PcdGet8 (PcdAcBtc)) || (PcdGet8 (PcdDcBtc))) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Notify AcBtc \n");
    EfiCreateProtocolNotifyEvent (&gEfiMpServiceProtocolGuid, TPL_CALLBACK, CallbackAcBtc, NULL, &Registration);
  }

  if (PcdGetBool (PcdCfgPcieHotplugSupport) == TRUE) {
    Status = ServerHotplugInitDxe (Pcie);
  }

  //
  // Close event, so it will not be invoked again.
  //
  gBS->CloseEvent (Event);

  IDS_HDT_CONSOLE (MAIN_FLOW, "CallbackAfterCcxDxeDone End\n");
}

/**
 *---------------------------------------------------------------------------------------
 *  ServerHotplugInitDxeCallback
 *---------------------------------------------------------------------------------------
 **/
VOID
EFIAPI
ServerHotplugInitDxeCallback (
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  )
{
  EFI_STATUS            Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  PCIe_PLATFORM_CONFIG  *PcieFromHob;
  VOID                  *Interface;

  Status = gBS->LocateProtocol (
                  &gEfiPciEnumerationCompleteProtocolGuid,
                  NULL,
                  &Interface
                  );
  if(EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "ServerHotplugInitDxeCallback locate fail\n");
    return;
  }

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  Pcie = (PCIe_PLATFORM_CONFIG *)Context;
  if(Context == NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Relocate *Pcie pointer\n");
    SmuDxeInitialize (&PcieFromHob);
    Pcie = PcieFromHob;
  }

  ASSERT (Pcie != NULL);

  Status = ServerHotplugInitDxe (Pcie);
  IDS_HDT_CONSOLE (GNB_TRACE, "ServerHotplugInitDxeCallback %r\n", Status);
}

/**
 * This service retrieves information about the layout of the cores on the given die.
 *
 * @param[in]  This                           A pointer to the
 *                                            HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL instance.
 * @param[in]  Socket                         Zero-based socket number to check.
 * @param[in]  Cdd                            The target cdd's identifier within Socket.
 * @param[out] NumberOfComplexes              Pointer to the number of enabled complexes on
 *                                            the given socket / die.
 * @param[out] NumberOfCores                  Pointer to the number of enabled cores per
 *                                            complex on the given socket / die.
 * @param[out] NumberOfThreads                Pointer to the number of enabled threads per
 *                                            core on the given socket / die.
 *
 * @retval EFI_SUCCESS                        The core topology information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER              Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER              Die is non-existent.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 *
 **/
EFI_STATUS
SmuGetCoreTopologyOnCdd (
  IN       HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Cdd,
  OUT      UINTN                                 *NumberOfComplexes,
  OUT      UINTN                                 *NumberOfCores,
  OUT      UINTN                                 *NumberOfThreads
  )
{
  EFI_STATUS                   Status;
  GNB_HANDLE                   *GnbHandle;
  THREAD_CONFIGURATION_STRUCT  ThreadConfiguration;

  // IDS_HDT_CONSOLE (MAIN_FLOW, "SmuGetCoreTopologyOnCdd Entry\n");
  GnbHandle = NULL;

  Status = SmuGetGnbHandle ((UINT32)Socket, 0, &GnbHandle);
  if (GnbHandle != NULL) {
    Status = NbioRegisterRead (
               GnbHandle,
               THREAD_CONFIGURATION_TYPE,
               (UINT32)CDD_SPACE (Cdd, THREAD_CONFIGURATION_ADDRESS_HYEX),
               &ThreadConfiguration,
               0
               );

      *NumberOfComplexes = ThreadConfiguration.HyExField.ComplexCount + 1;
      *NumberOfCores = ThreadConfiguration.HyExField.CoreCount + 1;
      *NumberOfThreads = (ThreadConfiguration.HyExField.SMTMode == 0) ? 2 : 1;
  } else {
      Status = EFI_INVALID_PARAMETER;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "Socket %d CDD %d has %d Ccx %d Cores %d threads \n", Socket, Cdd, *NumberOfComplexes, *NumberOfCores, *NumberOfThreads);
  //IDS_HDT_CONSOLE (MAIN_FLOW, "SmuGetCoreTopologyOnCdd Exit\n");
  return Status;
}

/**
 * This service will start a core to fetch its first instructions from the reset
 * vector.  This service may only be called from the BSP.
 *
 * @param[in]  This                           A pointer to the
 *                                            HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL instance.
 * @param[in]  Socket                         Zero-based socket number of the target thread.
 * @param[in]  Cdd                            Zero-based cdd number within Socket of the target thread.
 * @param[in]  LogicalComplex                 Zero-based logical complex number of the target thread.
 * @param[in]  LogicalThread                  Zero-based logical thread number of the target thread.
 *
 * @retval EFI_SUCCESS                        The thread was successfully launched.
 * @retval EFI_DEVICE_ERROR                   The thread has already been launched.
 * @retval EFI_INVALID_PARAMETER              Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER              Die is non-existent.
 * @retval EFI_INVALID_PARAMETER              LogicalComplex is non-existent.
 * @retval EFI_INVALID_PARAMETER              LogicalThread is non-existent.
 *
 **/
EFI_STATUS
SmuLaunchThread (
  IN       HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Cdd,
  IN       UINTN                                 LogicalComplex,
  IN       UINTN                                 LogicalThread
  )
{
  EFI_STATUS                   Status;
  GNB_HANDLE                   *GnbHandle;
  THREAD_ENABLE_STRUCT         ThreadEnable;
  THREAD_CONFIGURATION_STRUCT  ThreadConfiguration;
  UINTN                        NumberOfThreads;
  UINTN                        NumberOfCores;
  UINTN                        NumberOfLogicalThreads;
  UINT32                       SmuArg[6];

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuLaunchThread Entry\n");
  GnbHandle = NULL;
  Status    = SmuGetGnbHandle ((UINT32)Socket, 0, &GnbHandle);
  if (GnbHandle != NULL) {
    Status = NbioRegisterRead (
               GnbHandle,
               THREAD_ENABLE_TYPE,
               (UINT32)CDD_SPACE (Cdd, THREAD_ENABLE_ADDRESS_HYEX),
               &ThreadEnable,
               0
               );
    Status = NbioRegisterRead (
               GnbHandle,
               THREAD_CONFIGURATION_TYPE,
               (UINT32)CDD_SPACE (Cdd, THREAD_CONFIGURATION_ADDRESS_HYEX),
               &ThreadConfiguration,
               0
               );

      NumberOfThreads = (ThreadConfiguration.HyExField.SMTMode == 0) ? 2 : 1;
      NumberOfCores = ThreadConfiguration.HyExField.CoreCount + 1;
      NumberOfLogicalThreads = NumberOfCores * NumberOfThreads;
      if ((LogicalComplex <= ThreadConfiguration.HyExField.ComplexCount) && (LogicalThread < NumberOfLogicalThreads)) {
          if ((ThreadEnable.Field.ThreadEn & (1 << (LogicalThread + (8 * LogicalComplex)))) == 0) {
              ThreadEnable.Field.ThreadEn |= 1 << (LogicalThread + (8 * LogicalComplex));
              Status = NbioRegisterWrite (GnbHandle,
                      THREAD_ENABLE_TYPE,
                      (UINT32)CDD_SPACE(Cdd, THREAD_ENABLE_ADDRESS_HYEX),
                      &ThreadEnable,
                      0);
              NbioSmuServiceCommonInitArguments (SmuArg);
              SmuArg[0] = 0xAA55AA55;
              Status = NbioSmuServiceRequest (NbioGetHostPciAddress (GnbHandle), 0, SMC_MSG_TestMessage, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
          } else {
              Status = EFI_DEVICE_ERROR;
          }
      } else {
          Status = EFI_INVALID_PARAMETER;
      }
  } else {
    Status = EFI_INVALID_PARAMETER;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuLaunchThread Exit\n");
  return Status;
}

EFI_STATUS
SmuGetProcessorSN( 
   IN       UINT8                               SocketID,
   IN OUT   UINT32                              *ReturnValues        
)
{
    UINT32                              SmuArg[6];
    UINT32                              SmuResult;
    EFI_STATUS                          Status;
    GNB_HANDLE                          *GnbHandle;

    IDS_HDT_CONSOLE (MAIN_FLOW, "SmuGetProcessorSN Entry\n");

    Status = SmuGetGnbHandle (SocketID, 0, &GnbHandle);
    NbioSmuServiceCommonInitArguments (SmuArg);
    SmuArg[0] = 0;
    SmuResult = NbioSmuServiceRequest (NbioGetHostPciAddress (GnbHandle), 0, SMC_MSG_GetCpuSerialNumber, SmuArg, 0);
    if (SmuResult == SMC_Result_OK) {
        LibHygonMemCopy ((VOID *)ReturnValues, SmuArg, 24, (HYGON_CONFIG_PARAMS *) NULL); 
        IDS_HDT_CONSOLE (MAIN_FLOW, "SocketID %d SN:%x-%x\n", SocketID, SmuArg[1], SmuArg[0]);
        return EFI_SUCCESS;
    }
    return EFI_DEVICE_ERROR;  
}

/*----------------------------------------------------------------------------------------*/

/**
 *   Reads a fuse value based on an enumerated list of fuse "names"
 *
 *
 * @param[in]   This               A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
 * @param[in]   FuseName           The "name" of the fuse from an enumerated list of fuses supported by this function
 * @param[out]  FuseValue          Pointer to the value of the fuse
 *
 * @retval      EFI_SUCCESS
 *
 */
EFI_STATUS
EFIAPI
HygonSmuReadFuseByName (
  IN       DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              FuseName,
  OUT      UINT32                              *FuseValue
  )
{
  return EFI_UNSUPPORTED;
}

/*----------------------------------------------------------------------------------------*/
/**
 *   Reads a fuse value based on chain, start bit, and size
 *
 *
 * @param[in]   This               A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
 * @param[in]   FuseChain          The chain number of the fuse to read
 * @param[in]   FuseStartBit       The starting bit within the specified fuse chain of the fuse to read
 * @param[in]   FuseSize           The bitfield width of the fuse to read
 * @param[out]  FuseValue          Pointer to the value of the fuse
 *
 * @retval      EFI_SUCCESS
 *
 */
EFI_STATUS
EFIAPI
HygonSmuReadFuseByNumber (
  IN       DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              FuseChain,
  IN       UINT32                              FuseStartBit,
  IN       UINT32                              FuseSize,
  OUT      UINT32                              *FuseValue
  )
{
  return EFI_UNSUPPORTED;
}

/*----------------------------------------------------------------------------------------*/
/**
 *   Executes a specific SMU service and returns the results
 *
 *
 * @param[in]   This               A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
 * @param[in]   SocketId           The unique identifier of this socket
 * @param[in]   PhysicalDieId      The unique identifier of this Physical Die
 * @param[in]   ServiceRequest     The service request identifier
 * @param[in]   InputParameters    Pointer to an array of service request arguments (for CZ, UINT32[6])
 * @param[out]  ReturnValues       Pointer to an array of service request response values (UINT32[6])
 *
 * @retval      EFI_SUCCESS
 *
 */
EFI_STATUS
EFIAPI
HygonSmuServiceRequest (
  IN       DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              SocketId,
  IN       UINT32                              PhysicalDieId,
  IN       UINT32                              ServiceRequest,
  IN       UINT32                              *InputParameters,
  OUT      UINT32                              *ReturnValues
  )
{
  GNB_HANDLE  *GnbHandle;
  EFI_STATUS  Status;
  UINT32      SmuArg[6];
  UINT32      SmuResult;

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuServiceRequest Dxe Entry\n");
  Status = SmuGetGnbHandle (SocketId, PhysicalDieId, &GnbHandle);
  if (Status == EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "GnbHandle Found\n");
    LibHygonMemCopy (SmuArg, InputParameters, 24, (HYGON_CONFIG_PARAMS *)NULL);
    SmuResult = NbioSmuServiceRequest (NbioGetHostPciAddress (GnbHandle), PhysicalDieId, ServiceRequest, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
    if (SmuResult == 1) {
      LibHygonMemCopy (ReturnValues, SmuArg, 24, (HYGON_CONFIG_PARAMS *)NULL);
    } else {
      Status = EFI_INVALID_PARAMETER;
      IDS_HDT_CONSOLE (MAIN_FLOW, "SocketId not found!!\n");
      ASSERT (FALSE);       // Unsupported SMU service request
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuServiceRequest Exit Status = %d\n", Status);

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 *   Reads a 32-bit register from SMU register space
 *
 *
 * @param[in]  This               A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
 * @param[in]  InstanceId         The unique identifier of this socket
 * @param[in]  RegisterIndex      The index of the register to be read
 * @param[in]  RegisterValue      Pointer to a UINT32 holding the value to write to the register
 *
 */
EFI_STATUS
EFIAPI
HygonSmuRegisterRead (
  IN       DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              InstanceId,
  IN       UINT32                              RegisterIndex,
  OUT      UINT32                              *RegisterValue
  )
{
  GNB_HANDLE  *GnbHandle;
  EFI_STATUS  Status;

  Status = SmuGetGnbHandle (InstanceId, 0, &GnbHandle);
  if (Status == EFI_SUCCESS) {
    Status = NbioRegisterRead (GnbHandle, TYPE_SMN, RegisterIndex, RegisterValue, 0);
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Writes a 32-bit register in SMU register space
 *
 *
 *
 * @param[in]  This               A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
 * @param[in]  InstanceId         The unique identifier of this socket
 * @param[in]  RegisterIndex      The index of the register to be read
 * @param[in]  RegisterValue      Pointer to a UINT32 holding the value to write to the register
 *
 */
EFI_STATUS
EFIAPI
HygonSmuRegisterWrite (
  IN       DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              InstanceId,
  IN       UINT32                              RegisterIndex,
  IN       UINT32                              *RegisterValue
  )
{
  GNB_HANDLE  *GnbHandle;
  EFI_STATUS  Status;

  Status = SmuGetGnbHandle (InstanceId, 0, &GnbHandle);
  if (Status == EFI_SUCCESS) {
    Status = NbioRegisterWrite (GnbHandle, TYPE_SMN, RegisterIndex, RegisterValue, GNB_REG_ACC_FLAG_S3SAVE);
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read/Modify/Write a 32-bit register from SMU register space
 *
 *
 *
 * @param[in]  This               A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
 * @param[in]  InstanceId         The unique identifier of this socket
 * @param[in]  RegisterIndex      The index of the register to be read
 * @param[in]  RegisterANDValue   UINT32 holding the value to be "ANDed" with the data read from the register (i.e. mask of data that will not be modified)
 * @param[in]  RegisterORValue    UINT32 holding the value to be "ORed" with the data read from the register (i.e. data bits to be "set" by this function)
 *
 */
EFI_STATUS
EFIAPI
HygonSmuRegisterRMW (
  IN       DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              InstanceId,
  IN       UINT32                              RegisterIndex,
  IN       UINT32                              RegisterANDValue,
  IN       UINT32                              RegisterORValue
  )
{
  GNB_HANDLE  *GnbHandle;
  EFI_STATUS  Status;
  UINT32      RegisterValue;

  Status = SmuGetGnbHandle (InstanceId, 0, &GnbHandle);
  if (Status == EFI_SUCCESS) {
    Status = NbioRegisterRead (GnbHandle, TYPE_SMN, RegisterIndex, &RegisterValue, 0);
    RegisterValue = (RegisterValue & RegisterANDValue) | RegisterORValue;
    Status = NbioRegisterWrite (GnbHandle, TYPE_SMN, RegisterIndex, &RegisterValue, GNB_REG_ACC_FLAG_S3SAVE);
  }

  return Status;
}

/*----------------------------------------------------------------------------------------
/**
 * Returns the brand string read from SMN space.
 *
 * @param[in]  This               A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
 * @param[in]  InstanceId         The unique identifier of this socket
 * @param[in]  BrandStringLength  Number of characters to be read.  This value does NOT have to be a multiple of 4.
                                  NBIO driver code will be responsible for alignment, endianness, and string length.
 * @param[in]  BrandString        Pointer to a CHAR array to store the brand string.
 *
**/
EFI_STATUS
EFIAPI
HygonSmuReadBrandString (
  IN       DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              InstanceId,
  IN       UINT32                              BrandStringLength,
     OUT   UINT8                               *BrandString
  )
{
  GNB_HANDLE  *GnbHandle;
  EFI_STATUS  Status;
  UINT32      SmuArg[6];
  UINT32      SmuResult;
  UINT8       *StringPtr;
  UINT32      StringCount;
  UINT32      ByteCount;
  UINT32      Iteration;

  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonSmuReadBrandString Entry\n");
  Status = SmuGetGnbHandle (InstanceId, 0, &GnbHandle);
  if (Status == EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "GnbHandle Found\n");
    StringCount = BrandStringLength;
    StringPtr   = BrandString;
    Iteration   = 0;
    while (StringCount != 0) {
      NbioSmuServiceCommonInitArguments (SmuArg);
      SmuArg[0] = Iteration++;
      SmuResult = NbioSmuServiceRequest (NbioGetHostPciAddress (GnbHandle), 0, SMC_MSG_GetNameString, SmuArg, 0);
      if (SmuResult == 1) {
        ByteCount = (StringCount < 4) ? StringCount : 4;
        LibHygonMemCopy (StringPtr, SmuArg, StringCount, (HYGON_CONFIG_PARAMS *)NULL);
        StringPtr   += ByteCount;
        StringCount -= ByteCount;
      } else {
        Status = EFI_INVALID_PARAMETER;
        IDS_HDT_CONSOLE (MAIN_FLOW, "SmuRequestFail!!\n");
        break;
      }
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonSmuReadBrandString Exit Status = %d\n", Status);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Returns the CAC weights read from SMU
 *
 * @param[in]  This                    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
 * @param[in]  MaxNumWeights           The number of values to read from the SMU (defines the maximum size of ApmWeights return data buffer)
 * @param[Out] ApmWeights              Pointer to return data buffer
 *
 * @retval     EFI_SUCCESS             ApmWeights contains the CaC weights read from SMU
 * @retval     EFI_INVALID_PARAMETER   One of the input parameters was invalid
 *                                     InstanceId did not reference a valid NBIO instance
 * @retval     EFI_BUFFER_TOO_SMALL    The number of weights available exceeds MaxNumWeights
 * @retval     EFI_UNSUPPORTED         This function is not supported by this version of the driver
 */
EFI_STATUS
HygonSmuReadCacWeights (
  IN       DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              MaxNumWeights,
  OUT      UINT64                              *ApmWeights
  )
{
  EFI_STATUS                           Status;
  PCIe_PLATFORM_CONFIG                 *Pcie;
  PCIe_PLATFORM_CONFIG                 *PcieFromHob;
  GNB_HANDLE                           *NbioHandle;
  GNB_HANDLE                           *GnbHandle;
  UINT8                                WeightIndex;
  UINT32                               SmuArg[6];
  UINT32                               RxSmuMessageResponse[6];
  UINT32                               SmuResult;
  UINT64                               CacWeights[MAX_CAC_WEIGHT_NUM];

  IDS_HDT_CONSOLE (MAIN_FLOW, "Smu Cac Entry\n");

  LibHygonMemFill ((VOID *)CacWeights, 0x00, MAX_CAC_WEIGHT_NUM * (sizeof (UINT64)), NULL);
  SmuDxeInitialize (&PcieFromHob);
  Pcie = PcieFromHob;
  NbioSmuServiceCommonInitArguments (SmuArg);
  NbioHandle = NbioGetHandle (Pcie);
  Status     = SmuGetGnbHandle (0, 0, &GnbHandle);

  for (WeightIndex = 0; WeightIndex < MaxNumWeights; WeightIndex++) {

    NbioSmuServiceCommonInitArguments (SmuArg);
    SmuArg[0] = WeightIndex;

    SmuResult = NbioSmuServiceRequest (NbioGetHostPciAddress (GnbHandle), 0, SMC_MSG_ReadCoreCacWeightRegister, SmuArg, 0);

    if (SmuResult == 1) {
      LibHygonMemCopy (RxSmuMessageResponse, SmuArg, 24, (HYGON_CONFIG_PARAMS *)NULL);
      CacWeights[WeightIndex] = RxSmuMessageResponse[0];
      IDS_HDT_CONSOLE (MAIN_FLOW, "CacWeightsOrg[%d] = %x\n", WeightIndex, CacWeights[0]);
      IDS_HDT_CONSOLE (MAIN_FLOW, "RxSmuMessageResponse[0] = %x\n", RxSmuMessageResponse[0]);
      IDS_HDT_CONSOLE (MAIN_FLOW, "RxSmuMessageResponse[1] = %x\n", RxSmuMessageResponse[1]);
      CacWeights[WeightIndex] = (CacWeights[WeightIndex] << 32) | RxSmuMessageResponse[1];
      IDS_HDT_CONSOLE (MAIN_FLOW, "CacWeights[%d] = %llx\n", WeightIndex, CacWeights[WeightIndex]);
      LibHygonMemCopy (ApmWeights, CacWeights, MaxNumWeights * 8, (HYGON_CONFIG_PARAMS *) NULL);
      IDS_HDT_CONSOLE (MAIN_FLOW, "ApmWeights[0] = %llx\n", ApmWeights[0]);
    } else {
      Status = EFI_INVALID_PARAMETER;
      IDS_HDT_CONSOLE (MAIN_FLOW, "InstandId not found!!\n");
      ASSERT (FALSE);       // Unsupported SMU service request
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "Smu Cac Exit Status = %d\n", Status);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Returns the throttle thresholds read from SMU
 *
 * @param[in]  This              A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PPI instance.
 * @param[Out] Thresholds        Pointer to return data buffer
 *
 * @retval     EFI_SUCCESS       ApmWeights contains the throttle thresholds read from SMU
 * @retval     EFI_UNSUPPORTED   This function is not supported by this version of the driver
 */
EFI_STATUS
HygonSmuReadThrottleThresholds (
  IN       DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  OUT      UINT32                              *Thresholds
  )
{
  return EFI_UNSUPPORTED;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Reads a fuse value based on an enumerated list of fuse "names"
 *
 *
 *
 * @param[in]  SocketId                The unique identifier of the socket
 * @param[in]  PhysicalDieId           The unique identifier of the PhysicalDieId
 * @param[out] MyHandle                Pointer to GNB_HANDLE* to return GNB_HANDLE of this InstanceId
 *
 * @retval     EFI_SUCCESS             The fuse was located and the FuseValue returned is valid
 * @retval     EFI_INVALID_PARAMETER   InstanceId did not reference a valid NBIO instance
 */
EFI_STATUS
SmuGetGnbHandle (
  IN       UINT32                         SocketId,
  IN       UINT32                         PhysicalDieId,
  OUT      GNB_HANDLE                     **MyHandle
  )
{
  EFI_STATUS                     Status;
  GNB_PCIE_INFORMATION_DATA_HOB  *GnbPcieInformationDataHob;
  PCIe_PLATFORM_CONFIG           *Pcie;
  BOOLEAN                        InstanceFound;
  GNB_HANDLE                     *GnbHandle;

  //
  // Get information data from HOB.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &GnbPcieInformationDataHob);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  GnbPcieInformationDataHob = GetNextGuidHob (&gGnbPcieHobInfoGuid, GnbPcieInformationDataHob);
  if (GnbPcieInformationDataHob == NULL) {
    return EFI_NOT_FOUND;
  }

  Pcie = &(GnbPcieInformationDataHob->PciePlatformConfigHob);
  GnbHandle = NbioGetHandle (Pcie);

  InstanceFound = FALSE;
  Status = EFI_INVALID_PARAMETER;
  while ((InstanceFound == FALSE) && (GnbHandle != NULL)) {
    if ((GnbHandle->SocketId == SocketId) && (GnbHandle->PhysicalDieId == PhysicalDieId)) {
      InstanceFound = TRUE;
      Status = EFI_SUCCESS;
      IDS_HDT_CONSOLE (MAIN_FLOW, "InstanceId found!!\n");
    } else {
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
  }

  *MyHandle = GnbHandle;

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initilaize data for SMU Dxe phase.
 *
 *
 *
 * @param[out]  Pcie                    Get Pcie data from hob.
 *
 * @retval     EFI_SUCCESS
 */
EFI_STATUS
SmuDxeInitialize (
  OUT PCIe_PLATFORM_CONFIG           **Pcie
  )
{
  EFI_STATUS                     Status;
  GNB_PCIE_INFORMATION_DATA_HOB  *GnbPcieInformationDataHob;

  // Correcting Pcie information from Hob

  //
  // Get information data from HOB.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &GnbPcieInformationDataHob);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  GnbPcieInformationDataHob = GetNextGuidHob (&gGnbPcieHobInfoGuid, GnbPcieInformationDataHob);
  if (GnbPcieInformationDataHob == NULL) {
    return EFI_NOT_FOUND;
  }

  *Pcie = &(GnbPcieInformationDataHob->PciePlatformConfigHob);

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Dump Smu Feature Control.
 *
 *
 *
 * @param[in]  EnableFeatures
 *
 */
VOID
DumpSmuFeatureControl (
  IN       UINT32                         EnableFeatures
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "Smu Feature Control(0x%x):\n", EnableFeatures);
  IDS_HDT_CONSOLE (GNB_TRACE, " CCLK_CONTROLLER     %a\n", ((EnableFeatures & FEATURE_CCLK_CONTROLLER_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " FAN_CONTROLLER      %a\n", ((EnableFeatures & FEATURE_FAN_CONTROLLER_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " DATA_CALCULATION    %a\n", ((EnableFeatures & FEATURE_DATA_CALCULATION_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " PPT                 %a\n", ((EnableFeatures & FEATURE_PPT_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " TDC                 %a\n", ((EnableFeatures & FEATURE_TDC_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " THERMAL             %a\n", ((EnableFeatures & FEATURE_THERMAL_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " FIT                 %a\n", ((EnableFeatures & FEATURE_FIT_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " QOS                 %a\n", ((EnableFeatures & FEATURE_QOS_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " DF_CSTATES          %a\n", ((EnableFeatures & FEATURE_DF_CSTATES_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " PROCHOT             %a\n", ((EnableFeatures & FEATURE_PROCHOT_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " MCM_DATA_TRANSFER   %a\n", ((EnableFeatures & FEATURE_MCM_DATA_TRANSFER_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " DLWM                %a\n", ((EnableFeatures & FEATURE_DLWM_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " PC6                 %a\n", ((EnableFeatures & FEATURE_PC6_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " CSTATE_BOOST        %a\n", ((EnableFeatures & FEATURE_CSTATE_BOOST_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " VOLTAGE_CONTROLLER  %a\n", ((EnableFeatures & FEATURE_VOLTAGE_CONTROLLER_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " HOT_PLUG            %a\n", ((EnableFeatures & FEATURE_HOT_PLUG_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " CC1_CSTATE          %a\n", ((EnableFeatures & FEATURE_CC1_MASK) != 0) ? "Enable" : "Disable");
  IDS_HDT_CONSOLE (GNB_TRACE, " CC6_CSTATE          %a\n", ((EnableFeatures & FEATURE_CC6_MASK) != 0) ? "Enable" : "Disable");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Dump Fan Policy.
 *
 *
 *
 * @param[in]  PPTable
 *
 */
VOID
DumpFanPolicyTable (
  IN       PP_TABLE          *PPTable
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "Fan Policy:\n");
  IDS_HDT_CONSOLE (GNB_TRACE, "  ForceFanPwmEn = 0x%x\n", PPTable->ForceFanPwmEn);
  IDS_HDT_CONSOLE (GNB_TRACE, "  ForceFanPwm = 0x%x\n", PPTable->ForceFanPwm);
  IDS_HDT_CONSOLE (GNB_TRACE, "  FanTable_Override = 0x%x\n", PPTable->FanTable_Override);
  IDS_HDT_CONSOLE (GNB_TRACE, "  FanTable_Hysteresis = 0x%x\n", PPTable->FanTable_Hysteresis);
  IDS_HDT_CONSOLE (GNB_TRACE, "  FanTable_TempLow = 0x%x\n", PPTable->FanTable_TempLow);
  IDS_HDT_CONSOLE (GNB_TRACE, "  FanTable_TempMed = 0x%x\n", PPTable->FanTable_TempMed);
  IDS_HDT_CONSOLE (GNB_TRACE, "  FanTable_TempHigh = 0x%x\n", PPTable->FanTable_TempHigh);
  IDS_HDT_CONSOLE (GNB_TRACE, "  FanTable_TempCritical = 0x%x\n", PPTable->FanTable_TempCritical);
  IDS_HDT_CONSOLE (GNB_TRACE, "  FanTable_PwmLow = 0x%x\n", PPTable->FanTable_PwmLow);
  IDS_HDT_CONSOLE (GNB_TRACE, "  FanTable_PwmMed = 0x%x\n", PPTable->FanTable_PwmMed);
  IDS_HDT_CONSOLE (GNB_TRACE, "  FanTable_PwmHigh = 0x%x\n", PPTable->FanTable_PwmHigh);
  IDS_HDT_CONSOLE (GNB_TRACE, "  FanTable_PwmFreq = 0x%x\n", PPTable->FanTable_PwmFreq);
  IDS_HDT_CONSOLE (GNB_TRACE, "  FanTable_Polarity = 0x%x\n", PPTable->FanTable_Polarity);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  HygonNbioSmuHyExDxeEntry
 *
 *  @param[in]  ImageHandle     EFI Image Handle for the DXE driver
 *  @param[in]  SystemTable     Pointer to the EFI system table
 *
 *  Description:
 *    SMU Driver Entry.
 *
 *  @retval EFI_STATUS
 *
 */
EFI_STATUS
EFIAPI
HygonNbioSmuHyExDxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                               Status;
  EFI_HANDLE                               Handle;
  PP_TABLE                                 *PPTable;
  EFI_PHYSICAL_ADDRESS                     SetAgmLogDramAddress;
  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL     *NbioSmuServiceProtocol;
  UINT32                                   SmuArg[6];
  EFI_PHYSICAL_ADDRESS                     SmuDramBufferAddress;
  UINT32                                   DramBufferSize;
  GNB_HANDLE                               *NbioHandle;
  PCIe_PLATFORM_CONFIG                     *Pcie;
  PCIe_PLATFORM_CONFIG                     *PcieFromHob;
  VOID                                     *Registration;
  EFI_PHYSICAL_ADDRESS                     UnsecureDRAM;
  HPOB_HEADER                              *Hpob;
  HPOBLIB_INFO                             *HpobInfo;
  HPOB_MEM_NVDIMM_INFO_STRUCT_HYEX          *HpobNvDimmInfo;
  FAN_POLICY_TABLE                         *FanPolicyTable;
  UINT64                                   IdsNvValue;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  UINTN                                    CddPerSocket;

  HGPI_TESTPOINT (TpNbioSmuDxeEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioSmuHyExDxeEntry\n");
  Handle = NULL;
  FanPolicyTable = NULL;
  SetAgmLogDramAddress = 0xffffffff;

  IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_FORCE_VDDCR_CPU_VID_EN, &IdsNvValue);
  PcdSet8S (PcdForceVddcrCpuVidEn, (UINT8)IdsNvValue);
  IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_FORCE_VDDCR_CPU_VID, &IdsNvValue);
  PcdSet8S (PcdForceVddcrCpuVid, (UINT8)IdsNvValue);
  IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_VDDCR_CPU_VOLTAGE_MARGIN, &IdsNvValue);
  PcdSet32S (PcdVddcrCpuVoltageMargin, (UINT32)IdsNvValue);
  IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_FORCE_CCLK_FREQUENCY_EN, &IdsNvValue);
  PcdSet8S (PcdForceCclkFrequencyEn, (UINT8)IdsNvValue);
  IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_FORCE_CCLK_FREQUENCY, &IdsNvValue);
  PcdSet32S (PcdForceCclkFrequency, (UINT32)IdsNvValue);
  IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_DLDO_BYPASS, &IdsNvValue);
  PcdSet8S (PcdDldoBypass, (UINT8)IdsNvValue);

  // Enable default SMU Features based on package type
  Status = gBS->LocateProtocol (
                  &gHygonFabricTopologyServicesProtocolGuid,
                  NULL,
                  &FabricTopology
                  );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "LocateProtocol FabricTopologyServices %r\n", Status);
    return Status;
  }

  Status = FabricTopology->GetCddInfo (FabricTopology, 0, &CddPerSocket, NULL);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "GetCddInfo fail %r\n", Status);
    return Status;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "CddPerSocket = %d\n", CddPerSocket);
  if (4 == CddPerSocket) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Setting PcdSmuFeatureControlDefines to 0x%x for 4 CDD\n", (UINT32)SL2_SMU_FEATURES_ENABLE_DEFAULT);
    PcdSet32S (PcdSmuFeatureControlDefines, (UINT32)SL2_SMU_FEATURES_ENABLE_DEFAULT);
    PcdSet8S (PcdDcBtc, 1);    // Enabled DC BTC for SL2
    if (PcdGetBool (PcdEfficiencyOptimizedMode) == TRUE) {
      PcdSet32S (PcdSmuFeatureControlDefines, PcdGet32 (PcdSmuFeatureControlDefines) | ((UINT32)FEATURE_QOS_MASK));
    } else {
      PcdSet32S (PcdSmuFeatureControlDefines, PcdGet32 (PcdSmuFeatureControlDefines) & ~((UINT32)FEATURE_QOS_MASK));
    }
  } else if (2 == CddPerSocket) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Setting PcdSmuFeatureControlDefines to 0x%x for 2 CDD\n", (UINT32)SL2R2_SMU_FEATURES_ENABLE_DEFAULT);
    PcdSet32S (PcdSmuFeatureControlDefines, (UINT32)SL2R2_SMU_FEATURES_ENABLE_DEFAULT);
    // PcdSet8S (PcdDldoBypass, 1);
    if (PcdGetBool (PcdEfficiencyOptimizedMode) == TRUE) {
      PcdSet32S (PcdSmuFeatureControlDefines, PcdGet32 (PcdSmuFeatureControlDefines) | ((UINT32)FEATURE_QOS_MASK));
    } else {
      PcdSet32S (PcdSmuFeatureControlDefines, PcdGet32 (PcdSmuFeatureControlDefines) & ~((UINT32)FEATURE_QOS_MASK));
    }
  } else {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Setting PcdSmuFeatureControlDefines to 0x%x for 1 CDD\n", (UINT32)SMU_FEATURES_ENABLE_DEFAULT);
    PcdSet32S (PcdSmuFeatureControlDefines, (UINT32)SMU_FEATURES_ENABLE_DEFAULT);
    PcdSet8S (PcdDldoBypass, 1);   // Override DLDO bypass on DHM1
  }

  if (PcdGet8 (PcdRunEnvironment) > 0) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Setting PcdSmuFeatureControlDefines to 0 \n");
    PcdSet32S (PcdSmuFeatureControlDefines, (UINT32)0);
  }

  IDS_HOOK (IDS_HOOK_NBIO_SMU_INIT, NULL, NULL);
  IDS_HOOK (IDS_HOOK_NBIO_SMU_INIT_CMN, NULL, NULL);

  // Disable DLWM if NVDIMM installed
  Hpob = NULL;
  if (HygonPspGetHpobInfo (&HpobInfo) == EFI_SUCCESS) {
    if (HpobInfo->Supported) {
      Hpob = (HPOB_HEADER *)HpobInfo->HpobAddr;
    }
  }

  if (Hpob != NULL) {
    if (HygonPspGetHpobEntryInstance (HPOB_GROUP_MEM, HPOB_MEM_NVDIMM_INFO_TYPE, 0, FALSE, (HPOB_TYPE_HEADER **)&HpobNvDimmInfo) == EFI_SUCCESS) {
      if (HpobNvDimmInfo->NvdimmPresentInSystem == TRUE) {
        PcdSet32S (PcdSmuFeatureControlDefines, PcdGet32 (PcdSmuFeatureControlDefines) & ~((UINT32)FEATURE_DLWM_MASK));
      }
    }
  }

  DumpSmuFeatureControl (PcdGet32 (PcdSmuFeatureControlDefines));

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonNbioSmuServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mDxeHygonNbioSmuServicesProtocol
                  );

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonCoreTopologyServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mHygonCoreTopologyServicesProtocol
                  );

  // Collecting Pcie information from Hob
  SmuDxeInitialize (&PcieFromHob);
  Pcie = PcieFromHob;
  NbioHandle = NbioGetHandle (Pcie);

  //
  // Step14. Allocate DRAM to PPtable, System Topology
  //
  Status = gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  sizeof (PP_TABLE),
                  &PPTable
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Zero Pool
  gBS->SetMem (PPTable, sizeof (PP_TABLE), 0);

  //
  // Populate Pcd value to Pptable.
  //
  //BLDCFG
  PPTable->MaxSusPwr = PcdGet32 (PcdHygoncTDP);
  IDS_HDT_CONSOLE (MAIN_FLOW, "MaxSusPwr = 0x%x\n", PPTable->MaxSusPwr);
  PPTable->TDC = PcdGet32 (PcdCfgTDC);
  IDS_HDT_CONSOLE (MAIN_FLOW, "TDC = 0x%x\n", PPTable->TDC);
  PPTable->EDC = PcdGet32 (PcdCfgEDC);
  IDS_HDT_CONSOLE (MAIN_FLOW, "EDC = 0x%x\n", PPTable->EDC);
  PPTable->DeterminismPercentageEn = 1;
  IDS_HDT_CONSOLE (MAIN_FLOW, "DeterminismPercentageEn = 0x%x\n", PPTable->DeterminismPercentageEn);
  PPTable->TDPBoostEn = PcdGet8 (PcdHygonTDPBoost);
  IDS_HDT_CONSOLE (MAIN_FLOW, "TDPBoost = 0x%x\n", PPTable->TDPBoostEn);
  if (PcdGetBool (PcdHygonDeterminismControl) && ((4 == CddPerSocket) || (2 == CddPerSocket))) {
    PPTable->DeterminismPercentage = 100; // Performance
  } else {
    PPTable->DeterminismPercentage = 0; // Power
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "DeterminismPercentage = 0x%x\n", PPTable->DeterminismPercentage);

  // BLDCFG/CBS fan table
  PPTable->ForceFanPwmEn = PcdGet8 (PcdForceFanPwmEn);
  if (PPTable->ForceFanPwmEn != 0 ) {
    PPTable->ForceFanPwm = PcdGet8 (PcdForceFanPwm);
  }

  PPTable->FanTable_Override = PcdGet8 (PcdFanTableOverride);  // Fan Table Control Options
  if (PPTable->FanTable_Override != 0 ) {
    PPTable->FanTable_Hysteresis   = PcdGet8 (PcdFanTableHysteresis);
    PPTable->FanTable_TempLow      = PcdGet8 (PcdFanTableTempLow);
    PPTable->FanTable_TempMed      = PcdGet8 (PcdFanTableTempMed);
    PPTable->FanTable_TempHigh     = PcdGet8 (PcdFanTableTempHigh);
    PPTable->FanTable_TempCritical = PcdGet8 (PcdFanTableTempCritical);
    PPTable->FanTable_PwmLow   = PcdGet8 (PcdFanTablePwmLow);
    PPTable->FanTable_PwmMed   = PcdGet8 (PcdFanTablePwmMed);
    PPTable->FanTable_PwmHigh  = PcdGet8 (PcdFanTablePwmHigh);
    PPTable->FanTable_PwmFreq  = PcdGet8 (PcdFanTablePwmFreq);
    PPTable->FanTable_Polarity = PcdGet8 (PcdFanTablePolarity);
  } else {
    if (1 == CddPerSocket) {
      FanPolicyTable = (FAN_POLICY_TABLE *)&DefaultDHM1FanTable[0];
      // read the TDP and TJMAX
      NbioSmuServiceCommonInitArguments (SmuArg);
      Status = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_TdpTjmax, SmuArg, 0);
      IDS_HDT_CONSOLE (MAIN_FLOW, "TDP/TJMAX = 0x%x\n", SmuArg[0]);
      if (SmuArg[0] != 0) {
        while (FanPolicyTable->TdpTjMax != 0) {
          if (FanPolicyTable->TdpTjMax == SmuArg[0]) {
            if (PPTable->ForceFanPwmEn == 0) {
              PPTable->ForceFanPwmEn = FanPolicyTable->FanPolicy.ForceFanPwmEn;
              PPTable->ForceFanPwm   = FanPolicyTable->FanPolicy.ForceFanPwm;
            }

            PPTable->FanTable_Override   = FanPolicyTable->FanPolicy.FanTable_Override;
            PPTable->FanTable_Hysteresis = FanPolicyTable->FanPolicy.FanTable_Hysteresis;
            PPTable->FanTable_TempLow    = FanPolicyTable->FanPolicy.FanTable_TempLow;
            PPTable->FanTable_TempMed    = FanPolicyTable->FanPolicy.FanTable_TempMed;
            PPTable->FanTable_TempHigh   = FanPolicyTable->FanPolicy.FanTable_TempHigh;

            if(FanPolicyTable->FanPolicy.FanTable_TempCritical <=
               ((UINT8)(FanPolicyTable->TdpTjMax & 0x000000FF)  + FanPolicyTable->FanPolicy.FanTable_Hysteresis)) {
              PPTable->FanTable_TempCritical = (UINT8)(FanPolicyTable->TdpTjMax & 0x000000FF)  + FanPolicyTable->FanPolicy.FanTable_Hysteresis;
            } else {
              PPTable->FanTable_TempCritical = FanPolicyTable->FanPolicy.FanTable_TempCritical;
            }

            PPTable->FanTable_PwmLow   = FanPolicyTable->FanPolicy.FanTable_PwmLow;
            PPTable->FanTable_PwmMed   = FanPolicyTable->FanPolicy.FanTable_PwmMed;
            PPTable->FanTable_PwmHigh  = FanPolicyTable->FanPolicy.FanTable_PwmHigh;
            PPTable->FanTable_PwmFreq  = FanPolicyTable->FanPolicy.FanTable_PwmFreq;
            PPTable->FanTable_Polarity = FanPolicyTable->FanPolicy.FanTable_Polarity;
            break;
          }

          FanPolicyTable++;
        }
      }
    }
  }

  // dump Fan policy table
  DumpFanPolicyTable (PPTable);

  //CBS debug options
  PPTable->DldoPsmMargin = PcdGet8 (PcdDldoPsmMargin);
  IDS_HDT_CONSOLE(MAIN_FLOW, "DldoPsmMargin = 0x%x\n", PPTable->DldoPsmMargin);
  PPTable->CoreStretchThreshEn = PcdGet8 (PcdCoreStretchThreshEn);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CoreStretchThreshEn = 0x%x\n", PPTable->CoreStretchThreshEn);
  PPTable->CoreStretchThresh = PcdGet8 (PcdCoreStretchThresh);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CoreStretchThresh = 0x%x\n", PPTable->CoreStretchThresh);
  PPTable->L3StretchThreshEn = PcdGet8 (PcdL3StretchThreshEn);
  IDS_HDT_CONSOLE (MAIN_FLOW, "L3StretchThreshEn = 0x%x\n", PPTable->L3StretchThreshEn);
  PPTable->L3StretchThresh = PcdGet8 (PcdL3StretchThresh);
  IDS_HDT_CONSOLE (MAIN_FLOW, "L3StretchThresh = 0x%x\n", PPTable->L3StretchThresh);
  PPTable->DldoBypass = PcdGet8 (PcdDldoBypass);
  IDS_HDT_CONSOLE (MAIN_FLOW, "DldoBypass = 0x%x\n", PPTable->DldoBypass);
  PPTable->XiSeparationEn = PcdGet8 (PcdXiSeparationEn);
  IDS_HDT_CONSOLE (MAIN_FLOW, "XiSeparationEn = 0x%x\n", PPTable->XiSeparationEn);
  PPTable->XiSeparationHigh = PcdGet8 (PcdXiSeparationHigh);
  IDS_HDT_CONSOLE (MAIN_FLOW, "XiSeparationHigh = 0x%x\n", PPTable->XiSeparationHigh);
  PPTable->XiSeparationLow = PcdGet8 (PcdXiSeparationLow);
  IDS_HDT_CONSOLE (MAIN_FLOW, "XiSeparationLow = 0x%x\n", PPTable->XiSeparationLow);
  PPTable->AvfsCoeffTable_Override = PcdGet8 (PcdAvfsCoeffTableOverride);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AvfsCoeffTable_Override = 0x%x\n", PPTable->AvfsCoeffTable_Override);
  PPTable->ForceVddcrCpuVidEn = PcdGet8 (PcdForceVddcrCpuVidEn);
  IDS_HDT_CONSOLE (MAIN_FLOW, "ForceVddcrCpuVidEn = 0x%x\n", PPTable->ForceVddcrCpuVidEn);
  PPTable->ForceVddcrCpuVid = PcdGet8 (PcdForceVddcrCpuVid);
  IDS_HDT_CONSOLE (MAIN_FLOW, "ForceVddcrCpuVid = 0x%x\n", PPTable->ForceVddcrCpuVid);
  PPTable->VddcrCpuVoltageMargin = PcdGet32 (PcdVddcrCpuVoltageMargin);
  IDS_HDT_CONSOLE (MAIN_FLOW, "VddcrCpuVoltageMargin = 0x%x\n", PPTable->VddcrCpuVoltageMargin);
  PPTable->FcwSlewFrac_L3FidTotalSteps = PcdGet16 (PcdFcwSlewFracL3FidTotalSteps);
  IDS_HDT_CONSOLE (MAIN_FLOW, "FcwSlewFrac_L3FidTotalSteps = 0x%x\n", PPTable->FcwSlewFrac_L3FidTotalSteps);
  PPTable->FcwSlewFrac_L3FidTotalStepsEn = PcdGet8 (PcdFcwSlewFracL3FidTotalStepsEn);
  IDS_HDT_CONSOLE (MAIN_FLOW, "FcwSlewFrac_L3FidTotalStepsEn = 0x%x\n", PPTable->FcwSlewFrac_L3FidTotalStepsEn);
  PPTable->ForceCclkFrequencyEn = PcdGet8 (PcdForceCclkFrequencyEn);
  IDS_HDT_CONSOLE (MAIN_FLOW, "ForceCclkFrequencyEn = 0x%x\n", PPTable->ForceCclkFrequencyEn);
  PPTable->ForceCclkFrequency = PcdGet32 (PcdForceCclkFrequency);
  IDS_HDT_CONSOLE (MAIN_FLOW, "ForceCclkFrequency = 0x%x\n", PPTable->ForceCclkFrequency);
  PPTable->UseTelemetryData = PcdGet8 (PcdUseTelemetryData);
  IDS_HDT_CONSOLE (MAIN_FLOW, "UseTelemetryData = 0x%x\n", PPTable->UseTelemetryData);
  PPTable->WaitVidCompDis = PcdGet8 (PcdWaitVidCompDis);
  IDS_HDT_CONSOLE (MAIN_FLOW, "WaitVidCompDis = 0x%x\n", PPTable->WaitVidCompDis);

  PPTable->xHMIForceLinkWidthEn = PcdGet8 (PcdxHMIForceLinkWidthEn);
  IDS_HDT_CONSOLE (MAIN_FLOW, "xHMIForceLinkWidthEn = 0x%x\n", PPTable->xHMIForceLinkWidthEn);
  PPTable->xHMIForceLinkWidth = PcdGet8 (PcdxHMIForceLinkWidth);
  IDS_HDT_CONSOLE (MAIN_FLOW, "xHMIForceLinkWidth = 0x%x\n", PPTable->xHMIForceLinkWidth);
  PPTable->xHMIMaxLinkWidthEn = PcdGet8 (PcdxHMIMaxLinkWidthEn);
  IDS_HDT_CONSOLE (MAIN_FLOW, "xHMIMaxLinkWidthEn = 0x%x\n", PPTable->xHMIMaxLinkWidthEn);
  PPTable->xHMIMaxLinkWidth = PcdGet8 (PcdxHMIMaxLinkWidth);
  IDS_HDT_CONSOLE (MAIN_FLOW, "xHMIMaxLinkWidth = 0x%x\n", PPTable->xHMIMaxLinkWidth);

  // CBS debug options [AVFS]
  IDS_HDT_CONSOLE (MAIN_FLOW, "AVFS Control:\n");
  PPTable->CorePsfFreq = (INT32)PcdGet32 (PcdCorePsfFreq);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CorePsfFreq = %d\n", PPTable->CorePsfFreq);
  PPTable->CorePsfTemp = (INT32)PcdGet32 (PcdCorePsfTemp);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CorePsfTemp = %d\n", PPTable->CorePsfTemp);
  PPTable->CorePsfSidd = (INT32)PcdGet32 (PcdCorePsfSidd);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CorePsfSidd = %d\n", PPTable->CorePsfSidd);
  PPTable->CorePsfCac = (INT32)PcdGet32 (PcdCorePsfCac);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CorePsfCac = %d\n", PPTable->CorePsfCac);
  PPTable->CorePsfNumActiveCores = (INT32)PcdGet32 (PcdCorePsfNumActiveCores);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CorePsfNumActiveCores = %d\n", PPTable->CorePsfNumActiveCores);
  PPTable->CorePsfOtherDie = (INT32)PcdGet32 (PcdCorePsfOtherDie);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CorePsfOtherDie = %d\n", PPTable->CorePsfOtherDie);
  PPTable->CorePsfSigma = (INT32)PcdGet32 (PcdCorePsfSigma);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CorePsfSigma = %d\n", PPTable->CorePsfSigma);
  PPTable->L3PsfFreq = (INT32)PcdGet32 (PcdL3PsfFreq);
  IDS_HDT_CONSOLE (MAIN_FLOW, "L3PsfFreq = %d\n", PPTable->L3PsfFreq);
  PPTable->L3PsfTemp = (INT32)PcdGet32 (PcdL3PsfTemp);
  IDS_HDT_CONSOLE (MAIN_FLOW, "L3PsfTemp = %d\n", PPTable->L3PsfTemp);
  PPTable->L3PsfSidd = (INT32)PcdGet32 (PcdL3PsfSidd);
  IDS_HDT_CONSOLE (MAIN_FLOW, "L3PsfSidd = %d\n", PPTable->L3PsfSidd);
  PPTable->L3PsfCac = (INT32)PcdGet32 (PcdL3PsfCac);
  IDS_HDT_CONSOLE (MAIN_FLOW, "L3PsfCac = %d\n", PPTable->L3PsfCac);
  PPTable->L3PsfNumActiveCores = (INT32)PcdGet32 (PcdL3PsfNumActiveCores);
  IDS_HDT_CONSOLE (MAIN_FLOW, "L3PsfNumActiveCores = %d\n", PPTable->L3PsfNumActiveCores);
  PPTable->L3PsfOtherDie = (INT32)PcdGet32 (PcdL3PsfOtherDie);
  IDS_HDT_CONSOLE (MAIN_FLOW, "L3PsfOtherDie = %d\n", PPTable->L3PsfOtherDie);
  PPTable->L3PsfSigma = (INT32)PcdGet32 (PcdL3PsfSigma);
  IDS_HDT_CONSOLE (MAIN_FLOW, "L3PsfSigma = %d\n", PPTable->L3PsfSigma);

  IDS_HDT_CONSOLE (GNB_TRACE, "Sending this to the SMU...\n");
  GnbLibDebugDumpBuffer ((VOID *)PPTable, sizeof (PP_TABLE), 1, 16);
  IDS_HDT_CONSOLE (GNB_TRACE, "\n");

  Status = gBS->LocateProtocol (
                  &gHygonNbioSmuServicesProtocolGuid,
                  NULL,
                  &NbioSmuServiceProtocol
                  );
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }

  NbioSmuServiceCommonInitArguments (SmuArg);

  NbioHandle = NbioGetHandle (Pcie);
  while (NbioHandle != NULL) {
    //
    // Check SMU firmware revision.
    //
    IDS_HDT_CONSOLE (GNB_TRACE, "SMU Version Start\n");
    IDS_HDT_CONSOLE (GNB_TRACE, "BIOS2SmuSmc Version (%x)\n", BIOS_2_SMU_SMC_IF_VERSION);
    IDS_HDT_CONSOLE (GNB_TRACE, "SmuBiosIf Version (%x)\n", BIOS_IF_VERSION);
    IDS_HDT_CONSOLE (GNB_TRACE, "SmuFeature Version (%x)\n", SMU_FEATURE_VERSION);
    IDS_HDT_CONSOLE (GNB_TRACE, "SmuFwHearder Version (%x)\n", SMU_FW_HEADER_VERSION);

    NbioSmuServiceCommonInitArguments (SmuArg);
    SmuArg[0] = BIOS_IF_VERSION;
    Status    = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_GetBiosIfVersion, SmuArg, 0);
    IDS_HDT_CONSOLE (NB_MISC, "  SmuArg 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", SmuArg[0], SmuArg[1], SmuArg[2], SmuArg[3], SmuArg[4], SmuArg[5]);
    if (SmuArg[0] != BIOS_IF_VERSION) {
      IDS_HDT_CONSOLE (GNB_TRACE, "ERROR: SMU Version doesn't match (%x)\n", SmuArg[0]);
      ASSERT (FALSE);
    }

    IDS_HDT_CONSOLE (GNB_TRACE, "SMU Version End\n");

    //
    // Step15. Allocate DRAM space for the PPTable Structure.
    //
    SmuArg[0] = (UINT32)((UINTN)PPTable);
    Status    = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_SetBiosDramAddrLow, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
    if (EFI_ERROR (Status)) {
      return EFI_INVALID_PARAMETER;
    }

    SmuArg[0] = (UINT32)RShiftU64 ((UINT64)PPTable, 32);
    Status    = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_SetBiosDramAddrHigh, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
    if (EFI_ERROR (Status)) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // Step16. Call SMC_MSG_TransferTableDram2Smu to ask SMU to read in the PP Table, SMU reply when the DRAM read is complete.
    //
    NbioSmuServiceCommonInitArguments (SmuArg);
    SmuArg[0] = TABLE_PPTABLE; // Input Argument should be 0;
    Status    = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_TransferTableDram2Smu, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
    if (EFI_ERROR (Status)) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // Step18. Call BIOSSMC_MSG_CheckTdcCompatibility message.
    //
    if (PPTable->TDC != 0) {
      NbioSmuServiceCommonInitArguments (SmuArg);
      SmuArg[0] = PPTable->TDC;
      Status    = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_CheckTdcCompatibility, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
      if (EFI_ERROR (Status)) {
        // DEBUG ((EFI_D_ERROR | EFI_D_INFO, "fused TDC limit is failed !\n"));
      }

      IDS_HDT_CONSOLE (MAIN_FLOW, "SMC_MSG_CheckTdcCompatibility = 0x%x\n", SmuArg);
      IDS_HDT_CONSOLE (MAIN_FLOW, "Status = 0x%x\n", Status);
    }

    //
    // Step19. Call BIOSSMC_MSG_CheckEdcCompatibility message.
    //
    if (PPTable->EDC != 0) {
      NbioSmuServiceCommonInitArguments (SmuArg);
      SmuArg[0] = PPTable->EDC;
      Status    = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_CheckEdcCompatibility, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
      if (EFI_ERROR (Status)) {
        // DEBUG ((EFI_D_ERROR | EFI_D_INFO, "fused EDC limit is failed !\n"));
      }

      IDS_HDT_CONSOLE (MAIN_FLOW, "SMC_MSG_CheckEdcCompatibility = 0x%x\n", SmuArg);
      IDS_HDT_CONSOLE (MAIN_FLOW, "Status = 0x%x\n", Status);
    }

    //
    // Step20. Allocate 4KB DRAM, Call Message BIOSSMC_MSG_SetAgmLogDramAddress with 64 bit address
    //
    SetAgmLogDramAddress = 0xFFFFFFFF;
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiACPIMemoryNVS,
                    1,
                    &SetAgmLogDramAddress
                    );
    IDS_HDT_CONSOLE (MAIN_FLOW, "SetAgmLogDramAddress = 0x%x\n", SetAgmLogDramAddress);
    IDS_HDT_CONSOLE (MAIN_FLOW, "Status = 0x%x\n", Status);

    ZeroMem ((VOID *)(UINTN)SetAgmLogDramAddress, (1 * EFI_PAGE_SIZE));

    NbioSmuServiceCommonInitArguments (SmuArg);
    SmuArg[0] = (UINT32)SetAgmLogDramAddress;
    Status    = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_SetToolsDramAddrLow, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
    if (EFI_ERROR (Status)) {
      return EFI_INVALID_PARAMETER;
    }

    NbioSmuServiceCommonInitArguments (SmuArg);
    SmuArg[0] = (UINT32)RShiftU64 ((UINT64)SetAgmLogDramAddress, 32);
    Status    = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_SetToolsDramAddrHigh, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
    if (EFI_ERROR (Status)) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // Step21.Allocate 1kB unsecure DRAM for performance monitor logging.
    // Call message BIOSSMC_MSG_SetPerfmonDramAddressHigh (upper 32 bits)
    // and BIOSSMC_MSG_SetPerfmonDramAddressLog (lower 32 bits) with 64 bit address in the argument.
    //
    UnsecureDRAM = 0xFFFFFFFF;
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiACPIMemoryNVS,
                    1,
                    &UnsecureDRAM
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    IDS_HDT_CONSOLE (MAIN_FLOW, "UnsecureDRAM Status = 0x%x\n", Status);

    // Zero Pool
    ZeroMem ((VOID *)(UINTN)UnsecureDRAM, (1 * EFI_PAGE_SIZE));

    NbioSmuServiceCommonInitArguments (SmuArg);
    SmuArg[0] = (UINT32)UnsecureDRAM;
    IDS_HDT_CONSOLE (MAIN_FLOW, "UnsecureDRAM Low = 0x%x\n", SmuArg[0]);

    Status = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_SetPerfmonDramAddressLow, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);

    IDS_HDT_CONSOLE (MAIN_FLOW, "BIOSSMC_MSG_SetPerfmonDramAddressLow Status = 0x%x\n", Status);

    if (EFI_ERROR (Status)) {
      return EFI_INVALID_PARAMETER;
    }

    NbioSmuServiceCommonInitArguments (SmuArg);
    SmuArg[0] = (UINT32)RShiftU64 ((UINT64)UnsecureDRAM, 32);

    IDS_HDT_CONSOLE (MAIN_FLOW, "UnsecureDRAM High = 0x%x\n", SmuArg[0]);

    Status = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_SetPerfmonDramAddressHigh, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);

    IDS_HDT_CONSOLE (MAIN_FLOW, "BIOSSMC_MSG_SetPerfmonDramAddressHigh Status = 0x%x\n", Status);

    if (EFI_ERROR (Status)) {
      return EFI_INVALID_PARAMETER;
    }

    if (NbioHandle->InstanceId == 0) {
      //
      // Step22. if CBS options SmuAllocateDramBuffer is non-zero, allocate unsecure DRAM based on the size in Byte defined in the CBS options.
      // Call message BIOSSMC_MSG_SetDramBufferAddress to pass in the 64 bit address (use 2 message argument)
      // NOTE:  This memory is allocated for Die 0 of each socket
      DramBufferSize = PcdGet32 (PcdSmuAllocateDramBufferSize);

      if (DramBufferSize != 0) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "Allocating DramBufferSize = 0x%x\n", DramBufferSize);
        Status = gBS->AllocatePages (
                        AllocateAnyPages,
                        EfiACPIMemoryNVS,
                        GNB_MB_TO_PAGE (DramBufferSize),
                        &SmuDramBufferAddress
                        );
        IDS_HDT_CONSOLE (MAIN_FLOW, "Status = 0x%x\n", Status);
        if (!EFI_ERROR (Status)) {
          IDS_HDT_CONSOLE (MAIN_FLOW, "SmuDramBufferAddress = 0x%x\n", SmuDramBufferAddress);

          ZeroMem ((VOID *)SmuDramBufferAddress, DramBufferSize * SIZE_1MB);

          NbioSmuServiceCommonInitArguments (SmuArg);
          SmuArg[0] = (UINT32)((UINT64)SmuDramBufferAddress);
          IDS_HDT_CONSOLE (MAIN_FLOW, "SmuDramBufferAddressLow = 0x%x\n", SmuArg[0]);
          Status = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_DramLogSetDramAddrLow, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
          if (EFI_ERROR (Status)) {
            return EFI_INVALID_PARAMETER;
          }

          NbioSmuServiceCommonInitArguments (SmuArg);
          SmuArg[0] = (UINT32)RShiftU64 ((UINT64)SmuDramBufferAddress, 32);
          IDS_HDT_CONSOLE (MAIN_FLOW, "SmuDramBufferAddressHigh = 0x%x\n", SmuArg[0]);
          Status = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_DramLogSetDramAddrHigh, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
          if (EFI_ERROR (Status)) {
            return EFI_INVALID_PARAMETER;
          }

          NbioSmuServiceCommonInitArguments (SmuArg);
          SmuArg[0] = DramBufferSize * SIZE_1MB;
          Status    = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_DramLogSetDramSize, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
          if (EFI_ERROR (Status)) {
            return EFI_INVALID_PARAMETER;
          }
        }
      } else {
        IDS_HDT_CONSOLE (MAIN_FLOW, "Allocation FAILED!!\n");
      }
    }

    if (PcdGetBool (PcdHygonPSIDisable)) {
      NbioSmuServiceCommonInitArguments (SmuArg);
      Status = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_DisablePSI, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
    }

    NbioSmuServiceCommonInitArguments (SmuArg);
    SmuArg[0] = NbioHandle->Address.Address.Bus;
    Status    = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_PCIeInterruptRemap, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);

    NbioHandle = GnbGetNextSocketHandle (NbioHandle);
  }

  EfiCreateProtocolNotifyEvent (
    &gHygonCcxDxeInitCompleteProtocolGuid,
    TPL_NOTIFY,
    CallbackAfterCcxDxeDone,
    NULL,
    &Registration
    );
  
  Status = gBS->InstallProtocolInterface(
     &Handle,
     &gHygonProcessorServicesProtocolGuid,
     EFI_NATIVE_INTERFACE,
     &mProcessorSNProtocol
	 );
  
  HGPI_TESTPOINT (TpNbioSmuDxeExit, NULL);

  return EFI_SUCCESS;
}
