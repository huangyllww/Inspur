/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioSmuV10Dxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioSmuV10Dxe
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
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
#include <PiDxe.h>
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <GnbDxio.h>
#include <Guid/GnbPcieInfoHob.h>
#include <GnbRegistersZP.h>
#include <Library/GnbRegisterAccZpLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieConfigLib.h>
#include <IdsHookId.h>
#include <Library/AmdIdsHookLib.h>
#include <Guid/HobList.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioSmuV10Lib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <AmdNbioSmuV10Dxe.h>
#include <smu10_bios_if.h>
#include <smu10.h>
#include <Smu10FeaturesEnable.h>

#define FILECODE        NBIO_SMU_AMDNBIOSMUV10DXE_AMDNBIOSMUV10DXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL mAmdCoreTopologyServicesProtocol = {
  SmuV10GetCoreTopologyOnDie,
  SmuV10LaunchThread
};

DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL mDxeAmdNbioSmuServicesProtocol = {
  DXE_AMD_NBIO_SMU_SERVICES_DXE_REVISION,
  AmdSmuReadFuseByName,
  AmdSmuReadFuseByNumber,
  AmdSmuServiceRequest,
  AmdSmuRegisterRead,
  AmdSmuRegisterWrite,
  AmdSmuRegisterRMW,
  AmdSmuReadBrandString,
  AmdSmuReadCacWeights,
  AmdSmuReadThrottleThresholds
};

/**
 * This service retrieves information about the layout of the cores on the given die.
 *
 * @param[in]  This                           A pointer to the
 *                                            AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL instance.
 * @param[in]  Socket                         Zero-based socket number to check.
 * @param[in]  Die                            The target die's identifier within Socket.
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
SmuV10GetCoreTopologyOnDie (
  IN       AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Die,
     OUT   UINTN                                 *NumberOfComplexes,
     OUT   UINTN                                 *NumberOfCores,
     OUT   UINTN                                 *NumberOfThreads
  )
{
  DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol;
  PCIe_PLATFORM_CONFIG                *Pcie;
  EFI_STATUS                          Status;
  GNB_HANDLE                          *GnbHandle;
  AMD_CONFIG_PARAMS                   *StdHeader;
  GNB_PCIE_INFORMATION_DATA_HOB       *PciePlatformConfigHobData;
  PWR_01C_STRUCT                      ThreadConfiguration;

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuV10GetCoreTopologyOnDie Entry\n");
  GnbHandle = NULL;
  StdHeader = NULL;
  // Need topology structure
  Status = gBS->LocateProtocol (
                  &gAmdNbioPcieServicesProtocolGuid,
                  NULL,
                  &PcieServicesProtocol
                  );

//  ASSERT (Status == EFI_SUCCESS);
  if (Status == EFI_SUCCESS) {
    PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **) &PciePlatformConfigHobData);
    Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);

    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      //Find GnbHandle for this Socket/Die
      if ((Socket == GnbHandle->SocketId) && (Die == GnbHandle->DieNumber))  {
        break;
      }
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
//    ASSERT (GnbHandle != NULL);
    if (GnbHandle != NULL) {
      Status = GnbRegisterReadZP (GnbHandle,
                                  THREAD_CONFIGURATION_TYPE,
                                  THREAD_CONFIGURATION_ADDRESS,
                                  &ThreadConfiguration,
                                  0,
                                  StdHeader);
      *NumberOfComplexes = ThreadConfiguration.Field.ComplexCount + 1;
      *NumberOfCores = ThreadConfiguration.Field.CoreCount + 1;
      *NumberOfThreads = (ThreadConfiguration.Field.SMTMode == 0) ? 2 : 1;
    } else {
      Status = EFI_INVALID_PARAMETER;
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuV10GetCoreTopologyOnDie Exit\n");
  return Status;
}

/**
 * This service will start a core to fetch its first instructions from the reset
 * vector.  This service may only be called from the BSP.
 *
 * @param[in]  This                           A pointer to the
 *                                            AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL instance.
 * @param[in]  Socket                         Zero-based socket number of the target thread.
 * @param[in]  Die                            Zero-based die number within Socket of the target thread.
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
SmuV10LaunchThread (
  IN       AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Die,
  IN       UINTN                                 LogicalComplex,
  IN       UINTN                                 LogicalThread
  )
{
  DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol;
  PCIe_PLATFORM_CONFIG                *Pcie;
  EFI_STATUS                          Status;
  GNB_HANDLE                          *GnbHandle;
  AMD_CONFIG_PARAMS                   *StdHeader;
  GNB_PCIE_INFORMATION_DATA_HOB       *PciePlatformConfigHobData;
  PWR_018_STRUCT                      ThreadEnable;
  PWR_01C_STRUCT                      ThreadConfiguration;
  UINTN                               NumberOfThreads;
  UINTN                               NumberOfCores;
  UINTN                               NumberOfLogicalThreads;

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuV10GetCoreTopologyOnDie Entry\n");
  GnbHandle = NULL;
  StdHeader = NULL;
  // Need topology structure
  Status = gBS->LocateProtocol (
                  &gAmdNbioPcieServicesProtocolGuid,
                  NULL,
                  &PcieServicesProtocol
                  );

//  ASSERT (Status == EFI_SUCCESS);
  if (Status == EFI_SUCCESS) {
    PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **) &PciePlatformConfigHobData);
    Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      //Find GnbHandle for this Socket/Die
      if ((Socket == GnbHandle->SocketId) && (Die == GnbHandle->DieNumber)) {
        break;
      }
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
//    ASSERT (GnbHandle != NULL);
    if (GnbHandle != NULL) {
      Status = GnbRegisterReadZP (GnbHandle,
                                  THREAD_ENABLE_TYPE,
                                  THREAD_ENABLE_ADDRESS,
                                  &ThreadEnable,
                                  0,
                                  StdHeader);
      Status = GnbRegisterReadZP (GnbHandle,
                                  THREAD_CONFIGURATION_TYPE,
                                  THREAD_CONFIGURATION_ADDRESS,
                                  &ThreadConfiguration,
                                  0,
                                  StdHeader);
      NumberOfThreads = (ThreadConfiguration.Field.SMTMode == 0) ? 2 : 1;
      NumberOfCores = ThreadConfiguration.Field.CoreCount + 1;
      NumberOfLogicalThreads = NumberOfCores * NumberOfThreads;
      if ((LogicalComplex <= ThreadConfiguration.Field.ComplexCount) && (LogicalThread < NumberOfLogicalThreads)) {
        if ((ThreadEnable.Field.ThreadEn & (1 << (LogicalThread + (8 * LogicalComplex)))) == 0) {
          ThreadEnable.Field.ThreadEn |= 1 << (LogicalThread + (8 * LogicalComplex));
          Status = GnbRegisterWriteZP (GnbHandle,
                                       THREAD_ENABLE_TYPE,
                                       THREAD_ENABLE_ADDRESS,
                                       &ThreadEnable,
                                       0,
                                       StdHeader);
        } else {
          Status = EFI_DEVICE_ERROR;
        }
      } else {
        Status = EFI_INVALID_PARAMETER;
      }
    } else {
      Status = EFI_INVALID_PARAMETER;
    }
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuV10GetCoreTopologyOnDie Exit\n");
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 *   Reads a fuse value based on an enumerated list of fuse "names"
 *
 *
 * @param[in]   This               A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL instance.
 * @param[in]   FuseName           The "name" of the fuse from an enumerated list of fuses supported by this function
 * @param[out]  FuseValue          Pointer to the value of the fuse
 *
 * @retval      EFI_SUCCESS
 *
 */
EFI_STATUS
EFIAPI
AmdSmuReadFuseByName (
  IN       DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              FuseName,
     OUT   UINT32                              *FuseValue
  )
{
  return EFI_UNSUPPORTED;
}

/*----------------------------------------------------------------------------------------*/
/**
 *   Reads a fuse value based on chain, start bit, and size
 *
 *
 * @param[in]   This               A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL instance.
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
AmdSmuReadFuseByNumber (
  IN       DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              FuseChain,
  IN       UINT32                              FuseStartBit,
  IN       UINT32                              FuseSize,
     OUT   UINT32                              *FuseValue
  )
{
  return EFI_UNSUPPORTED;
}

/*----------------------------------------------------------------------------------------*/
/**
 *   Executes a specific SMU service and returns the results
 *
 *
 * @param[in]   This               A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL instance.
 * @param[in]   InstanceId         The unique identifier of the NBIO instance associated with this socket/die
 * @param[in]   ServiceRequest     The service request identifier
 * @param[in]   InputParameters    Pointer to an array of service request arguments (for CZ, UINT32[6])
 * @param[out]  ReturnValues       Pointer to an array of service request response values (UINT32[6])
 *
 * @retval      EFI_SUCCESS
 *
 */
EFI_STATUS
AmdSmuServiceRequest (
  IN       DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              InstanceId,
  IN       UINT32                              ServiceRequest,
  IN       UINT32                              *InputParameters,
     OUT   UINT32                              *ReturnValues
  )
{
  GNB_HANDLE                      *GnbHandle;
  EFI_STATUS                      Status;
  UINT32                          SmuArg[6];
  UINT32                          SmuResult;

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuServiceRequest Dxe Entry\n");
  Status = SmuGetGnbHandle (InstanceId, &GnbHandle);
  if (Status == EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "GnbHandle Found\n");
    LibAmdMemCopy (SmuArg, InputParameters, 24, (AMD_CONFIG_PARAMS *) NULL);
    SmuResult = NbioSmuServiceRequestV10 (NbioGetHostPciAddress (GnbHandle), ServiceRequest, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
    if (SmuResult == 1) {
      LibAmdMemCopy (ReturnValues, SmuArg, 24, (AMD_CONFIG_PARAMS *) NULL);
    } else {
      Status = EFI_INVALID_PARAMETER;
      IDS_HDT_CONSOLE (MAIN_FLOW, "InstandId not found!!\n");
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
 * @param[in]  This               A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL instance.
 * @param[in]  InstanceId         The unique identifier of the NBIO instance associated with this socket/die
 * @param[in]  RegisterIndex      The index of the register to be read
 * @param[in]  RegisterValue      Pointer to a UINT32 holding the value to write to the register
 *
 */
EFI_STATUS
AmdSmuRegisterRead (
  IN       DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              InstanceId,
  IN       UINT32                              RegisterIndex,
     OUT   UINT32                              *RegisterValue
  )
{
  GNB_HANDLE                      *GnbHandle;
  EFI_STATUS                      Status;

  Status = SmuGetGnbHandle (InstanceId, &GnbHandle);
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
 * @param[in]  This               A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL instance.
 * @param[in]  InstanceId         The unique identifier of the NBIO instance associated with this socket/die
 * @param[in]  RegisterIndex      The index of the register to be read
 * @param[in]  RegisterValue      Pointer to a UINT32 holding the value to write to the register
 *
 */
EFI_STATUS
AmdSmuRegisterWrite (
  IN       DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              InstanceId,
  IN       UINT32                              RegisterIndex,
  IN       UINT32                              *RegisterValue
  )
{
  GNB_HANDLE                      *GnbHandle;
  EFI_STATUS                      Status;

  Status = SmuGetGnbHandle (InstanceId, &GnbHandle);
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
 * @param[in]  This               A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL instance.
 * @param[in]  InstanceId         The unique identifier of the NBIO instance associated with this socket/die
 * @param[in]  RegisterIndex      The index of the register to be read
 * @param[in]  RegisterANDValue   UINT32 holding the value to be "ANDed" with the data read from the register (i.e. mask of data that will not be modified)
 * @param[in]  RegisterORValue    UINT32 holding the value to be "ORed" with the data read from the register (i.e. data bits to be "set" by this function)
 *
 */
EFI_STATUS
AmdSmuRegisterRMW (
  IN       DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              InstanceId,
  IN       UINT32                              RegisterIndex,
  IN       UINT32                              RegisterANDValue,
  IN       UINT32                              RegisterORValue
  )
{
  GNB_HANDLE                      *GnbHandle;
  EFI_STATUS                      Status;
  UINT32                          RegisterValue;

  Status = SmuGetGnbHandle (InstanceId, &GnbHandle);
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
 * @param[in]  This               A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL instance.
 * @param[in]  InstanceId         The unique identifier of the NBIO instance associated with this socket/die
 * @param[in]  BrandStringLength  Number of characters to be read.  This value does NOT have to be a multiple of 4.
                                  NBIO driver code will be responsible for alignment, endianness, and string length.
 * @param[in]  BrandString        Pointer to a CHAR array to store the brand string.
 *
**/
EFI_STATUS
AmdSmuReadBrandString (
  IN       DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              InstanceId,
  IN       UINT32                              BrandStringLength,
     OUT   UINT8                               *BrandString
  )
{
  GNB_HANDLE                      *GnbHandle;
  EFI_STATUS                      Status;
  UINT32                          SmuArg[6];
  UINT32                          SmuResult;
  UINT8                           *StringPtr;
  UINT32                          StringCount;
  UINT32                          ByteCount;
  UINT32                          Iteration;

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdSmuReadBrandString Entry\n");
  Status = SmuGetGnbHandle (InstanceId, &GnbHandle);
  if (Status == EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "GnbHandle Found\n");
    StringCount = BrandStringLength;
    StringPtr = BrandString;
    Iteration = 0;
    while (StringCount != 0) {
      NbioSmuServiceCommonInitArgumentsV10 (SmuArg);
      SmuArg[0] = Iteration++;
      SmuResult = NbioSmuServiceRequestV10 (NbioGetHostPciAddress (GnbHandle), BIOSSMC_MSG_GetNameString, SmuArg, 0);
      if (SmuResult == 1) {
        ByteCount = (StringCount < 4) ? StringCount : 4;
        LibAmdMemCopy (StringPtr, SmuArg, StringCount, (AMD_CONFIG_PARAMS *) NULL);
        StringPtr += ByteCount;
        StringCount -= ByteCount;
      } else {
        Status = EFI_INVALID_PARAMETER;
        IDS_HDT_CONSOLE (MAIN_FLOW, "SmuRequestFail!!\n");
        break;
      }
    }
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdSmuReadBrandString Exit Status = %d\n", Status);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Returns the CAC weights read from SMU
 *
 * @param[in]  This                    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
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
AmdSmuReadCacWeights (
  IN       DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              MaxNumWeights,
     OUT   UINT64                              *ApmWeights
  )
{
  return EFI_UNSUPPORTED;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Returns the throttle thresholds read from SMU
 *
 * @param[in]  This              A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PPI instance.
 * @param[Out] Thresholds        Pointer to return data buffer
 *
 * @retval     EFI_SUCCESS       ApmWeights contains the throttle thresholds read from SMU
 * @retval     EFI_UNSUPPORTED   This function is not supported by this version of the driver
 */
EFI_STATUS
AmdSmuReadThrottleThresholds (
  IN       DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL  *This,
     OUT   UINT32                              *Thresholds
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
 * @param[in]  InstanceId              The unique identifier of the NBIO instance associated with this socket/die
 * @param[out] MyHandle                Pointer to GNB_HANDLE* to return GNB_HANDLE of this InstanceId
 *
 * @retval     EFI_SUCCESS             The fuse was located and the FuseValue returned is valid
 * @retval     EFI_INVALID_PARAMETER   InstanceId did not reference a valid NBIO instance
 */
EFI_STATUS
SmuGetGnbHandle (
  IN       UINT32                         InstanceId,
     OUT   GNB_HANDLE                     **MyHandle
  )
{
  EFI_STATUS                           Status;
  GNB_PCIE_INFORMATION_DATA_HOB        *GnbPcieInformationDataHob;
  PCIe_PLATFORM_CONFIG                 *Pcie;
  BOOLEAN                              InstanceFound;
  GNB_HANDLE                           *GnbHandle;

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
    if (GnbHandle->InstanceId == InstanceId) {
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
  EFI_STATUS                           Status;
  GNB_PCIE_INFORMATION_DATA_HOB        *GnbPcieInformationDataHob;

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
 * @param[out]  Pcie                    Get Pcie data from hob.
 *
 */
VOID
DumpSmuFeatureControl (
  IN       UINT32                         EnableFeatures
  )
{
	IDS_HDT_CONSOLE (GNB_TRACE, "Smu Feature Control(0x%x):\n", EnableFeatures);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  AmdNbioSmuV10DxeEntry
 *
 *  @param[in]  ImageHandle     EFI Image Handle for the DXE driver
 *  @param[in]  SystemTable     Pointer to the EFI system table
 *
 *  Description:
 *    SMUV10 Driver Entry.
 *
 *  @retval EFI_STATUS
 *
 */
EFI_STATUS
EFIAPI
AmdNbioSmuV10DxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           Handle;
  UINT32                               SmuArg[6];
  UINT32                               RxSmuMessageResponse[6];

  AGESA_TESTPOINT (TpNbioSmuV10DxeEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioSmuV10DxeEntry Entry\n");
  Handle = NULL;

  // Enable default SMU Features
  PcdSet32 (PcdSmuFeatureControlDefines, (PcdGet32 (PcdSmuFeatureControlDefines) | (UINT32) SMU_FEATURES_ENABLE_DEFAULT));
  IDS_HOOK (IDS_HOOK_NBIO_SMUV10_INIT, NULL, NULL);
  DumpSmuFeatureControl (PcdGet32 (PcdSmuFeatureControlDefines));

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdNbioSmuServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mDxeAmdNbioSmuServicesProtocol
                  );

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdCoreTopologyServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mAmdCoreTopologyServicesProtocol
                  );

  //
  // Send SMC_MSG_EnableAllSmuFeatures
  //
  NbioSmuServiceCommonInitArgumentsV10 (SmuArg);
  SmuArg[0] = PcdGet32 (PcdSmuFeatureControlDefines);
  SmuArg[0] = 0;       // Todo: Temporarily to set SMU feature as 0
  SmuArg[0] |= BIT12;  // FEATURE_GFXOFF_BIT
  Status = mDxeAmdNbioSmuServicesProtocol.AmdSmuServiceRequest (
                                     &mDxeAmdNbioSmuServicesProtocol,
                                     0,
                                     SMC_MSG_EnableSmuFeatures,
                                     SmuArg,
                                     RxSmuMessageResponse
                                     );
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }

  NbioSmuServiceCommonInitArgumentsV10 (SmuArg);
  Status = mDxeAmdNbioSmuServicesProtocol.AmdSmuServiceRequest (
                                     &mDxeAmdNbioSmuServicesProtocol,
                                     0,
                                     SMC_MSG_GfxOffExit,
                                     SmuArg,
                                     RxSmuMessageResponse
                                     );
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioSmuV10DxeEntry Exit\n");
  AGESA_TESTPOINT (TpNbioSmuV10DxeExit, NULL);

  return EFI_SUCCESS;
}
