/* $NoKeywords:$ */

/**
 * @file
 *
 * HygonSmuServicesPpi Implementation
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioSmuPei
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
#include <HYGON.h>

#include <PiPei.h>
#include <Filecode.h>
#include <GnbRegisters.h>
#include <CddRegistersDm.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioSmuLib.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include <Guid/GnbPcieInfoHob.h>
#include <HygonNbioSmuPei.h>

#define FILECODE  NBIO_SMU_HYGX_HYGONNBIOSMUPEI_HYGONSMUSERVICESPPI_FILECODE

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

/*----------------------------------------------------------------------------------------
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
  EFI_PEI_HOB_POINTERS           Hob;
  GNB_PCIE_INFORMATION_DATA_HOB  *PciePlatformConfigHob;
  PCIe_PLATFORM_CONFIG           *Pcie;
  EFI_STATUS                     Status;
  GNB_HANDLE                     *GnbHandle;
  BOOLEAN                        InstanceFound;
  GNB_PCIE_INFORMATION_DATA_HOB  *GnbPcieInfoDataHob;

  GnbHandle = NULL;
  // Need topology structure
  GnbPcieInfoDataHob    = NULL;
  PciePlatformConfigHob = NULL;

  Status = PeiServicesGetHobList (&Hob.Raw);

  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION &&
        CompareGuid (&Hob.Guid->Name, &gGnbPcieHobInfoGuid)) {
      PciePlatformConfigHob = (GNB_PCIE_INFORMATION_DATA_HOB *)Hob.Raw;
      Status = EFI_SUCCESS;
      break;
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  if (PciePlatformConfigHob == NULL) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "%a() - Fail to get PciePlatformConfigHob\n", __FUNCTION__);
    return EFI_NOT_FOUND;
  }

  Pcie = &(PciePlatformConfigHob->PciePlatformConfigHob);
  GnbHandle = NbioGetHandle (Pcie);

  InstanceFound = FALSE;
  Status = EFI_INVALID_PARAMETER;
  while ((InstanceFound == FALSE) && (GnbHandle != NULL)) {
    if ((GnbHandle->SocketId == SocketId) && (GnbHandle->PhysicalDieId == PhysicalDieId)) {
      InstanceFound = TRUE;
      Status = EFI_SUCCESS;
    } else {
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
  }

  *MyHandle = GnbHandle;
  return Status;
}

/*----------------------------------------------------------------------------------------
/**
  Reads a fuse value based on an enumerated list of fuse "names"

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of this socket
  FuseName
    The "name" of the fuse from an enumerated list of fuses supported by this function
  FuseValue
    Pointer to the value of the fuse

  Status Codes Returned:
  EFI_SUCCESS           - The fuse was located and the FuseValue returned is valid
  EFI_INVALID_PARAMETER - One of the input parameters was invalid
                        - InstanceId did not reference a valid NBIO instance
                        - FuseName was not found in the list of supported fuse identifier values
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
SmuReadFuseByName (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         FuseName,
  OUT      UINT32                         *FuseValue
  )
{
  return EFI_UNSUPPORTED;
}

/*----------------------------------------------------------------------------------------
/**
  Reads a fuse value based on chain, start bit, and size

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of this socket
  FuseChain
    The chain number of the fuse to read
  FuseStartBit
    The starting bit within the specified fuse chain of the fuse to read
  FuseSize
    The bitfield width of the fuse to read
  FuseValue
    Pointer to the value of the fuse

  Status Codes Returned:
  EFI_SUCCESS           - The fuse was located and the FuseValue returned is valid
  EFI_INVALID_PARAMETER - One of the input parameters was invalid
                        - InstanceId did not reference a valid NBIO instance
                        - FuseChain was not a valid fuse chain value
                        - FuseStartBit was not valid within the specified fuse chain
                        - FuseSize exceeded the size available in the return field
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
SmuReadFuseByLocation (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         FuseChain,
  IN       UINT32                         FuseStartBit,
  IN       UINT32                         FuseSize,
  OUT      UINT32                         *FuseValue
  )
{
  return EFI_UNSUPPORTED;
}

/*----------------------------------------------------------------------------------------
/**
  Executes a specific SMU service and returns the results (Generally intended for NBIO internal requests)

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  SocketId
    The unique identifier of this socket
  PhysicalDieId
    The unique identifier of this Physical Die
  ServiceRequest
    The service request identifier
  InputParameters
    Pointer to an array of service request arguments (for CZ, UINT32[6]).  Input parameters are not validated by the driver prior to issuing the service request.
  ReturnValues
    Pointer to an array of service request response values (for CZ, UINT32[6])

  Status Codes Returned:
  EFI_SUCCESS           - The SMU service request was completed and the argument registers are returned in "ReturnValues"
  EFI_INVALID_PARAMETER - One of the input parameters was invalid
                        - InstanceId did not reference a valid NBIO instance
                        - ServiceRequest was not supported by this version of SMU
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
SmuServiceRequest (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         SocketId,
  IN       UINT32                         PhysicalDieId,
  IN       UINT32                         ServiceRequest,
  IN       UINT32                         *InputParameters,
  OUT      UINT32                         *ReturnValues
  )
{
  GNB_HANDLE  *GnbHandle;
  EFI_STATUS  Status;
  UINT32      SmuArg[6];
  UINT32      SmuResult;

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuServiceRequest Entry\n");
  Status = SmuGetGnbHandle (SocketId, PhysicalDieId, &GnbHandle);
  if (Status == EFI_SUCCESS) {
    LibHygonMemCopy (SmuArg, InputParameters, 24, (HYGON_CONFIG_PARAMS *)NULL);
    SmuResult = NbioSmuServiceRequest (NbioGetHostPciAddress (GnbHandle), PhysicalDieId, ServiceRequest, SmuArg, 0);
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

/*----------------------------------------------------------------------------------------
/**
  Executes a specific HSIO service and returns the results (Generally intended for NBIO internal requests)

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  SocketId
    The unique identifier of this socket
  PhysicalDieId
    The unique identifier of this Physical Die
  ServiceRequest
    The service request identifier
  InputParameters
    Pointer to an array of service request arguments (for CZ, UINT32[6]).  Input parameters are not validated by the driver prior to issuing the service request.
  ReturnValues
    Pointer to an array of service request response values (for CZ, UINT32[6])

  Status Codes Returned:
  EFI_SUCCESS           - The SMU service request was completed and the argument registers are returned in "ReturnValues"
  EFI_INVALID_PARAMETER - One of the input parameters was invalid
                        - InstanceId did not reference a valid NBIO instance
                        - ServiceRequest was not supported by this version of SMU
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
SmuHsioServiceRequest (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         SocketId,
  IN       UINT32                         PhysicalDieId,
  IN       UINT32                         ServiceRequest,
  IN       UINT32                         *InputParameters,
  OUT      UINT32                         *ReturnValues
  )
{
  GNB_HANDLE  *GnbHandle;
  EFI_STATUS  Status;
  UINT32      SmuArg[6];
  UINT32      SmuResult;

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuHsioServiceRequest Entry\n");
  Status = SmuGetGnbHandle (SocketId, PhysicalDieId, &GnbHandle);
  if (Status == EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "GnbHandle Found\n");
    LibHygonMemCopy (SmuArg, InputParameters, 24, (HYGON_CONFIG_PARAMS *)NULL);
    SmuResult = NbioHsioServiceRequest (GnbHandle, PhysicalDieId, ServiceRequest, SmuArg, 0);
    if (SmuResult == 1) {
      LibHygonMemCopy (ReturnValues, SmuArg, 24, (HYGON_CONFIG_PARAMS *)NULL);
    } else {
      Status = EFI_INVALID_PARAMETER;
      IDS_HDT_CONSOLE (MAIN_FLOW, "SocketId not found!!\n");
      ASSERT (FALSE);       // Unsupported SMU service request
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuHsioServiceRequest Exit Status = %d\n", Status);
  return Status;
}

/*----------------------------------------------------------------------------------------
/**
  Reads a 32-bit register from SMU register space (Generally intended for NBIO internal requests)

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of this socket
  RegisterIndex
    The index of the register to be read
  RegisterValue
    Pointer to a UINT32 to store the value read from the register

  Status Codes Returned:
  EFI_SUCCESS           - The register value is returned in RegisterValue
  EFI_INVALID_PARAMETER - One of the input parameters was invalid
                        - InstanceId did not reference a valid NBIO instance
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
SmuRegisterRead (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         RegisterIndex,
  OUT      UINT32                         *RegisterValue
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

/*----------------------------------------------------------------------------------------
/**
  Writes a 32-bit register in SMU register space (Generally intended for NBIO internal requests)

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of this socket
  RegisterIndex
    The index of the register to be read
  RegisterValue
    Pointer to a UINT32 holding the value to write to the register

  Status Codes Returned:
  EFI_SUCCESS           - The specified register has been updated with the requested value
  EFI_INVALID_PARAMETER - One of the input parameters was invalid
                        - InstanceId did not reference a valid NBIO instance
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
SmuRegisterWrite (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         RegisterIndex,
  IN       UINT32                         *RegisterValue
  )
{
  GNB_HANDLE  *GnbHandle;
  EFI_STATUS  Status;

  Status = SmuGetGnbHandle (InstanceId, 0, &GnbHandle);
  if (Status == EFI_SUCCESS) {
    Status = NbioRegisterWrite (GnbHandle, TYPE_SMN, RegisterIndex, RegisterValue, 0);
  }

  return Status;
}

/**
  Read/Modify/Write a 32-bit register from SMU register space (Generally intended for NBIO internal requests)

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of this socket
  RegisterIndex
    The index of the register to be read
  RegisterANDValue
    UINT32 holding the value to be "ANDed" with the data read from the register (i.e. mask of data that will not be modified)
  RegisterORValue
    UINT32 holding the value to be "ORed" with the data read from the register (i.e. data bits to be "set" by this function)

  Status Codes Returned:
  EFI_SUCCESS           - The specified register has been updated with the requested value
  EFI_INVALID_PARAMETER - One of the input parameters was invalid
                        - InstanceId did not reference a valid NBIO instance
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
SmuRegisterRMW (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         RegisterIndex,
  IN       UINT32                         RegisterANDValue,
  IN       UINT32                         RegisterORValue
  )
{
  GNB_HANDLE  *GnbHandle;
  EFI_STATUS  Status;
  UINT32      RegisterValue;

  Status = SmuGetGnbHandle (InstanceId, 0, &GnbHandle);
  if (Status == EFI_SUCCESS) {
    Status = NbioRegisterRead (GnbHandle, TYPE_SMN, RegisterIndex, &RegisterValue, 0);
    RegisterValue = (RegisterValue & RegisterANDValue) | RegisterORValue;
    Status = NbioRegisterWrite (GnbHandle, TYPE_SMN, RegisterIndex, &RegisterValue, 0);
  }

  return Status;
}

/*----------------------------------------------------------------------------------------
/**
  Enable/Disable HTC

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of this socket
  HtcStatus
    Pointer to a BOOLEAN that returns the state of HTC.  If HtcReg.HtcTmpLmt != 0 then HTC will be enabled and return TRUE.
      TRUE   - HTC is enabled
      FALSE  - HTC is disabled

  Status Codes Returned:
  EFI_SUCCESS           - HTC register has been updated based on the value of HtcTmpLmt (i.e enable/disable)
  EFI_INVALID_PARAMETER - One of the input parameters was invalid
                        - InstanceId did not reference a valid NBIO instance
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
SmuHtcControl (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  OUT      BOOLEAN                        *HtcStatus
  )
{
  return EFI_UNSUPPORTED;
}

/*----------------------------------------------------------------------------------------
/**
  Returns the CAC weights read from SMU

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of this socket
  MaxNumWeights
    The number of values to read from the SMU (defines the maximum size of ApmWeights return data buffer)
  ApmWeights
    Pointer to return data buffer
  NumWeightsRead
    The number of values read from SMU
  EnergyCountNormalize
    Pointer to return value for energy count normalize

  Status Codes Returned:
  EFI_SUCCESS           - ApmWeights contains the CaC weights read from SMU
                        - ApmWeights contains the number of values available from SMU
                        - EnergyCountNormalize contains the value returned by SMU
  EFI_INVALID_PARAMETER - One of the input parameters was invalid
                        - InstanceId did not reference a valid NBIO instance
  EFI_BUFFER_TOO_SMALL  - The number of weights available exceeds MaxNumWeights
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
SmuReadCacWeights (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         MaxNumWeights,
  OUT      UINT64                         *ApmWeights,
  OUT      UINT32                         *NumWeightsRead,
  OUT      UINT32                         *EnergyCountNormalize
  )
{
  return EFI_UNSUPPORTED;
}

/*----------------------------------------------------------------------------------------
/**
  Program PsiVid and PsiVidEn

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of this socket
  TargetVid
    Target Vid value (0 to skip enablement)
  TargetNbVid
    Target NbVid value (0 to skip enablement)
**/
EFI_STATUS
SmuPsiControl (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         TargetVid,
  IN       UINT32                         TargetNbVid
  )
{
  return EFI_UNSUPPORTED;
}

/*----------------------------------------------------------------------------------------
/**
  Returns the TDP value from cTDP or SMU-defined value from SystemConfig in milliwatts

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of this socket
  TdpValue
    Pointer to a UINT32 to store the TDP value in milliwatts

  Status Codes Returned:
  EFI_SUCCESS           - TdpValue is updated with the cTDP or SmuTDP value as appropriate
  EFI_INVALID_PARAMETER - One of the input parameters was invalid
                        - InstanceId did not reference a valid NBIO instance
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver OR TdpValue is not available
**/
EFI_STATUS
SmuGetTdpValue (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  OUT      UINT32                         *TdpValue
  )
{
  return EFI_UNSUPPORTED;
}

/*----------------------------------------------------------------------------------------
/**
  Returns the brand string read from SMN space.

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of this socket
  BrandStringLength
    Number of characters to be read.  This value does NOT have to be a multiple of 4.  NBIO driver code will be responsible for alignment, endianness, and string length.
  BrandString
    Pointer to a CHAR array to store the brand string.

  Status Codes Returned:
  EFI_SUCCESS           - BrandString has been updated with the value read from SMU
  EFI_INVALID_PARAMETER - One of the input parameters was invalid
                        - InstanceId did not reference a valid NBIO instance
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
SmuReadBrandString (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         BrandStringLength,
  OUT      UINT8                          *BrandString
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

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuServiceRequest Entry\n");
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

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuServiceRequest Exit Status = %d\n", Status);
  return Status;
}

/*----------------------------------------------------------------------------------------
/**
  Reads the Core PllLock Timer and returns the programmed value

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of this socket
  LockTimerValue
    Pointer to a UINT32 to store the value read from the Core PLL Lock Timer.  This field alters the timing from PLL lock enable to Pll frequency lock. Please consult the PLL specification for more detail.

  Status Codes Returned:
  EFI_SUCCESS           - LockTimerValue has been updated with the value read from the PLL Lock Timer register field
  EFI_INVALID_PARAMETER - One of the input parameters was invalid
                        - InstanceId did not reference a valid NBIO instance
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
SmuReadCorePllLockTimer (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  OUT      UINT32                         *LockTimerValue
  )
{
  return EFI_UNSUPPORTED;
}

/*----------------------------------------------------------------------------------------
/**
  set the downcore register to the specified value on all dies across the system.

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  CoreDisMask
    The value to set the downcore register to.

  Status Codes Returned:
  EFI_SUCCESS           - The downcore register was successfully updated.
  EFI_NOT_FOUND         - the PPI is not in the database.

**/
EFI_STATUS
SmuSetDownCoreRegister (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         CoreDisMask
  )
{
  UINT32                                  SmuArg[6];
  EFI_PEI_SERVICES                        **PeiServices;
  EFI_STATUS                              Status;
  GNB_HANDLE                              *GnbHandle;
  UINTN                                   SocketCount, CddCount, CddsPresent;
  UINTN                                   i, j;
  HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI  *FabricTopologyServices;

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuSetDownCoreRegister Entry\n");
  GnbHandle = NULL;

  PeiServices = (EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonFabricTopologyServicesPpiGuid,
                             0,
                             NULL,
                             &FabricTopologyServices
                             );
  if (Status == EFI_SUCCESS) {
    FabricTopologyServices->GetSystemInfo (&SocketCount, NULL, NULL);
    for (i = 0; i < SocketCount; i++) {
      FabricTopologyServices->GetCddInfo (i, &CddCount, &CddsPresent);
      Status = SmuGetGnbHandle (i, 0, &GnbHandle);
      for (j = 0; j < MAX_CDDS_PER_SOCKET; j++) {
        if (!IS_CDD_PRESENT (j, CddsPresent)) {
          continue;
        }

        if (GnbHandle != NULL) {
          IDS_HDT_CONSOLE (MAIN_FLOW, "  down core\n");
          Status = NbioRegisterWrite (GnbHandle, TYPE_SMN, CDD_SPACE (j, PWR_DOWNCORE_CNTL_ADDRESS), &CoreDisMask, 0);
        }
      }

      if (GnbHandle != NULL) {
        NbioSmuServiceCommonInitArguments (SmuArg);
        SmuArg[0] = 0xAA55AA55;
        Status    = NbioSmuServiceRequest (NbioGetHostPciAddress (GnbHandle), 0, SMC_MSG_TestMessage, SmuArg, 0);
      }
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuSetDownCoreRegister(%x) Exit\n", Status);
  return Status;
}

/*----------------------------------------------------------------------------------------
/**
  disable SMT on all dies across the system. This routine should only be called from the BSP.

  Parameters:
  This
    A pointer to the PEI_HYGON_NBIO_SMU_SERVICES_PPI instance.
  SmtMode
    SMT Mode

  Status Codes Returned:
  EFI_SUCCESS           - SMT was successfully disabled.
  EFI_NOT_FOUND         - the PPI is not in the database.

**/
EFI_STATUS
SmuSetSmt (
  IN       PEI_HYGON_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT8                          SmtMode
  )
{
  UINT32                                  SmuArg[6];
  EFI_PEI_SERVICES                        **PeiServices;
  EFI_STATUS                              Status;
  GNB_HANDLE                              *GnbHandle;
  THREAD_CONFIGURATION_STRUCT             ThreadConfiguration;
  UINTN                                   SocketCount, CddCount, CddsPresent;
  UINTN                                   i, j;
  HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI  *FabricTopologyServices;

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuSetSmt Entry\n");
  GnbHandle = NULL;

  PeiServices = (EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();

  Status = (*PeiServices)->LocatePpi (PeiServices, &gHygonFabricTopologyServicesPpiGuid, 0, NULL, &FabricTopologyServices);
  if (Status == EFI_SUCCESS) {
    FabricTopologyServices->GetSystemInfo (&SocketCount, NULL, NULL);
    for (i = 0; i < SocketCount; i++) {
      Status = SmuGetGnbHandle (i, 0, &GnbHandle);
      FabricTopologyServices->GetCddInfo (i, &CddCount, &CddsPresent);
      for (j = 0; j < MAX_CDDS_PER_SOCKET; j++) {
        if (!IS_CDD_PRESENT (j, CddsPresent)) {
          continue;
        }

        if (GnbHandle != NULL) {
          IDS_HDT_CONSOLE (MAIN_FLOW, "  Socket %d CDD %d Set SMTMode = %d\n", i, j, SmtMode);
          Status = NbioRegisterRead (GnbHandle, TYPE_SMN, CDD_SPACE (j, THREAD_CONFIGURATION_ADDRESS_HYGX), &ThreadConfiguration, 0);
          ThreadConfiguration.HyGxField.SMTDisable = SmtMode;
          Status = NbioRegisterWrite (GnbHandle, TYPE_SMN, CDD_SPACE (j, THREAD_CONFIGURATION_ADDRESS_HYGX), &ThreadConfiguration.Value, 0);
        }
      }

      if (GnbHandle != NULL) {
        NbioSmuServiceCommonInitArguments (SmuArg);
        SmuArg[0] = 0xAA55AA55;
        Status    = NbioSmuServiceRequest (NbioGetHostPciAddress (GnbHandle), 0, SMC_MSG_TestMessage, SmuArg, 0);
      }
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuSetSmt(%x) Exit\n", Status);
  return Status;
}

/*----------------------------------------------------------------------------------------
 *                          P P I   D E S C R I P T O R
 *----------------------------------------------------------------------------------------
 */

PEI_HYGON_NBIO_SMU_SERVICES_PPI  mNbioSmuServicePpi = {
  HYGON_NBIO_SMU_SERVICES_REVISION,  ///< revision
  SmuReadFuseByName,
  SmuReadFuseByLocation,
  SmuServiceRequest,
  SmuHsioServiceRequest,
  SmuRegisterRead,
  SmuRegisterWrite,
  SmuRegisterRMW,
  SmuHtcControl,
  SmuReadCacWeights,
  SmuPsiControl,
  SmuGetTdpValue,
  SmuReadBrandString,
  SmuReadCorePllLockTimer,
  SmuSetDownCoreRegister,
  SmuSetSmt
};
