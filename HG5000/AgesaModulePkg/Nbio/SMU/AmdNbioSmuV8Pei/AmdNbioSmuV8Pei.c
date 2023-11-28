/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioSmuV8Pei Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioSmuV8Pei
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
#include <AGESA.h>
#include <PiPei.h>
#include <Filecode.h>
#include <GnbPcie.h>
#include <OptionGnb.h>
#include <GnbRegistersCZ.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/GnbHeapLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/AmdMemPpi.h>
#include <Ppi/NbioBaseEarlyBeforeCpuPpi.h>
#include <Library/AmdHeapLib.h>
#include <OptionGnb.h>
#include <mm.h>
#include <Library/BaseMemoryLib.h>
#include <Library/GnbCZLib/Modules/GnbInitCZ/GnbRegisterAccCZ.h>
#include <Library/GnbCZLib/Modules/GnbPcieConfig/GnbHandleLib.h>
#include <Library/GnbCZLib/Modules/GnbSmuLibV8/GnbSmuInitLibV8.h>

#define FILECODE        NBIO_SMU_AMDNBIOSMUV8PEI_AMDNBIOSMUV8PEI_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

#define MEMCLK_ID_TO_FREQ(Id)  ((((Id) + 6) * 33) + (((Id) + 6) / 3))
#define GnbFpLibToUint32(Value)            (*((UINT32*) &Value))

extern BUILD_OPT_CFG           UserOptions;
extern OPTION_GNB_FEATURE      GnbSmuEarlyInterfaceCZ;
extern OPTION_GNB_FEATURE      GnbScsInterfaceCZ;

typedef float FLOAT;
//extern EFI_GUID gAmdGnbOptionsPpiGuid;
EFI_GUID gAmdGnbOptionsPpiGuid     = { 0x34837265, 0x6f6e, 0x4f02, 0x8b, 0x27, 0x42, 0x7b, 0x01, 0x6f, 0xcd, 0xc5 };

/// GNB_HEAP_HANDLE
typedef struct {
  UINT32    HeapHandle; ///<
  UINT32    Size;       ///<
  VOID      *Buffer;    ///<
} GNB_HEAP_HANDLE;

/// The GNB Option parameter
typedef struct {
  AMD_EARLY_PARAMS      EarlyParams;       ///<
  AMD_POST_PARAMS       PostParams;        ///<
  GNB_HEAP_HANDLE       GnbHeapHandle[20]; ///<
} GNB_OPTION_PARAM;

EFI_STATUS
EFIAPI
AmdGnbSMUEarlyBeforeCPU (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

EFI_STATUS
EFIAPI
AmdGnbSmuInitAtPostAfterDram (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mGnbSMUEarlyBeforeCPUPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdNbioBaseEarlyBeforeCpuPpiGuid,
  AmdGnbSMUEarlyBeforeCPU
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mGnbPostAfterDramPeimNotify =
{
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdMemoryInitCompletePpiGuid,
  AmdGnbSmuInitAtPostAfterDram
};


/**
  Reads a fuse value based on an enumerated list of fuse "names"

  Parameters:
  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
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
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                          InstanceId,
  IN       UINT32                         FuseName,
     OUT   UINT32                         *FuseValue
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Reads a fuse value based on chain, start bit, and size

  Parameters:
  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
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
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         FuseChain,
  IN       UINT32                         FuseStartBit,
  IN       UINT32                         FuseSize,
     OUT   UINT32                         *FuseValue
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Executes a specific SMU service and returns the results (Generally intended for NBIO internal requests)

  Parameters:
  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
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
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         ServiceRequest,
  IN       UINT32                         *InputParameters,
     OUT   UINT32                         *ReturnValues
  )
{
  EFI_STATUS              Status;
  DEV_OBJECT              DevObject;
  UINT32                  SmuArg[6];
  GNB_OPTION_PARAM        *GnbOptionParamPtr;
  AMD_CONFIG_PARAMS       *StdHeader;
  GNB_HANDLE              *GnbHandle;

  Status = PeiServicesLocatePpi (
             &gAmdGnbOptionsPpiGuid,
             0,
             NULL,
             (VOID **)&GnbOptionParamPtr
             );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "AmdGnbInitAtPostAfterDram SMU Exit-1 Status=%r\n", Status);
    return Status;
  }

  StdHeader = &GnbOptionParamPtr->EarlyParams.StdHeader;
  GnbHandle = GnbGetHandle (StdHeader);

  GnbSmuServiceCommonInitArgumentsV8 (GnbHandle, StdHeader, &DevObject, SmuArg);

  SmuArg[0] = InputParameters[0];
  SmuArg[1] = InputParameters[1];
  SmuArg[2] = InputParameters[2];
  SmuArg[3] = InputParameters[3];
  SmuArg[4] = InputParameters[4];
  SmuArg[5] = InputParameters[5];

  GnbSmuServiceRequestV8 (
    &DevObject,
    ServiceRequest,
    SmuArg,
    0
    );

  ReturnValues[0] = SmuArg[0];
  ReturnValues[1] = SmuArg[1];
  ReturnValues[2] = SmuArg[2];
  ReturnValues[3] = SmuArg[3];
  ReturnValues[4] = SmuArg[4];
  ReturnValues[5] = SmuArg[5];

  return 0;
}
/**
  Executes a specific SMU service and returns the results (Generally intended for NBIO internal requests)

  Parameters:
  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
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
SmuDxioServiceRequest (
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         ServiceRequest,
  IN       UINT32                         *InputParameters,
     OUT   UINT32                         *ReturnValues
  )
{
  return  EFI_UNSUPPORTED;
}
/**
  Reads a 32-bit register from SMU register space (Generally intended for NBIO internal requests)

  Parameters:
  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
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
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         RegisterIndex,
     OUT   UINT32                         *RegisterValue
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Writes a 32-bit register in SMU register space (Generally intended for NBIO internal requests)

  Parameters:
  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
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
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         RegisterIndex,
  IN       UINT32                         *RegisterValue
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Read/Modify/Write a 32-bit register from SMU register space (Generally intended for NBIO internal requests)

  Parameters:
  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
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
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         RegisterIndex,
  IN       UINT32                         RegisterANDValue,
  IN       UINT32                         RegisterORValue
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Enable/Disable HTC

  Parameters:
  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
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
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
     OUT   BOOLEAN                        *HtcStatus
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Returns the CAC weights read from SMU

  Parameters:
  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
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
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         MaxNumWeights,
     OUT   UINT64                         *ApmWeights,
     OUT   UINT32                         *NumWeightsRead,
     OUT   UINT32                         *EnergyCountNormalize
  )
{
  return EFI_UNSUPPORTED;
}


/**
  Program PsiVid and PsiVidEn

  Parameters:
  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
  TargetVid
    Target Vid value (0 to skip enablement)
  TargetNbVid
    Target NbVid value (0 to skip enablement)
**/
EFI_STATUS
SmuPsiControl (
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         TargetVid,
  IN       UINT32                         TargetNbVid
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Returns the TDP value from cTDP or SMU-defined value from SystemConfig in milliwatts

  Parameters:
  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
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
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
     OUT   UINT32                         *TdpValue
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Returns the brand string read from SMN space.

  Parameters:
  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
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
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
  IN       UINT32                         BrandStringLength,
     OUT   UINT8                          *BrandString
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Reads the Core PllLock Timer and returns the programmed value

  Parameters:
  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
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
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI  *This,
  IN       UINT32                         InstanceId,
     OUT   UINT32                        *LockTimerValue
  )
{
  return EFI_UNSUPPORTED;
}

STATIC PEI_AMD_NBIO_SMU_SERVICES_PPI mNbioSmuServicePpi = {
  AMD_NBIO_SMU_SERVICES_REVISION,  ///< revision
  SmuReadFuseByName,
  SmuReadFuseByLocation,
  SmuServiceRequest,
  SmuDxioServiceRequest,
  SmuRegisterRead,
  SmuRegisterWrite,
  SmuRegisterRMW,
  SmuHtcControl,
  SmuReadCacWeights,
  SmuPsiControl,
  SmuGetTdpValue,
  SmuReadBrandString,
  SmuReadCorePllLockTimer
};

STATIC EFI_PEI_PPI_DESCRIPTOR mNbioSmuServicesPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdNbioSmuServicesPpiGuid,
  &mNbioSmuServicePpi
};

EFI_STATUS
EFIAPI
AmdGnbSMUEarlyBeforeCPU (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  EFI_STATUS                      Status;
  GNB_OPTION_PARAM                *GnbOptionParamPtr;

  Status = (*PeiServices)->LocatePpi (PeiServices, &gAmdGnbOptionsPpiGuid, 0, NULL, (VOID**)&GnbOptionParamPtr);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "AmdGnbInitAtPostAfterDram SMU Exit-1 Status=%r\n", Status);
    return Status;
  }

  Status = GnbSmuEarlyInterfaceCZ (&GnbOptionParamPtr->EarlyParams.StdHeader);
  Status = GnbScsInterfaceCZ (&GnbOptionParamPtr->EarlyParams.StdHeader);

  Status = (**PeiServices).InstallPpi (PeiServices, &mNbioSmuServicesPpiList);  //This driver is done.

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Load Memory Config Parameters to SMU
 *
 *
 *
 * @param[in]  GnbHandle           Pointer to GNB_HANDLE
 * @param[in]  GnbBuildOptionData  Pointer to Build Options
 * @param[in]  StdHeader           Standard configuration header
 */

VOID
STATIC
GnbSmuPrepareMemoryParams (
  IN       GNB_HANDLE            *GnbHandle,
  IN       GNB_BUILD_OPTIONS_CZ  *GnbBuildOptionData,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  EFI_STATUS                    Status;
  DIE_STRUCT                    *MCTPtr;
  FLOAT                         VddIo;
  FLOAT                         Vddr;
  UINT8                         offset;
  UINT16                        DimmValid;
  D18F2x2E0_dct0_STRUCT         D18F2x2E0_dct0;
  D18F2x2E0_dct1_STRUCT         D18F2x2E0_dct1;
  UINT32                        Dimms;
  UINT32                        MemorySpeedHigh;
  UINT32                        MemorySpeedLow;
  UINT32                        *SmuArg;
  AMD_MEMORY_INIT_COMPLETE_PPI  *MemInitCompleteData;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbSmuPrepareMemoryParamsCZ Enter\n");

  SmuArg = (UINT32 *) GnbAllocateHeapBufferAndClear (AMD_GNB_SMU_MEMORY_INFO_BUFFER_HANDLE, 24, StdHeader);
  ASSERT (SmuArg != NULL);

  MCTPtr = (DIE_STRUCT *) GnbLocateHeapBuffer (GENERATE_MEM_HANDLE (ALLOC_DIE_STRUCT_HANDLE, 0, 0, 0), StdHeader);
  ASSERT (MCTPtr != NULL);

  Status = PeiServicesLocatePpi (
             &gAmdMemoryInitCompletePpiGuid,
             0,
             NULL,
             (VOID **)&MemInitCompleteData
             );
  ASSERT (Status == EFI_SUCCESS);

  DimmValid = (UINT16) (MCTPtr->DimmValid & 0xFFFF);
  DimmValid |= (UINT16) (MCTPtr->DimmValid >> 16);

  IDS_HDT_CONSOLE (GNB_TRACE, "Valid Dimm Mask: %08x\n", MCTPtr->DimmValid);
  IDS_HDT_CONSOLE (GNB_TRACE, "Local Dimm Mask: %04x\n", DimmValid);
  Dimms = 0;

  // Get number of dimms
  for (offset = 0; offset < 16; offset ++) {
    if ((DimmValid & ((UINT32)1 << offset)) != 0) {
      Dimms ++;
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "Number of Dimms: %d\n", Dimms);
  SmuArg[4] = Dimms;

  // Get memory frequency
  GnbRegisterReadCZ (GnbHandle, D18F2x2E0_dct0_TYPE, D18F2x2E0_dct0_ADDRESS, &D18F2x2E0_dct0.Value, 0, StdHeader);
  GnbRegisterReadCZ (GnbHandle, D18F2x2E0_dct1_TYPE, D18F2x2E0_dct1_ADDRESS, &D18F2x2E0_dct1.Value, 0, StdHeader);
  MemorySpeedHigh = MCTPtr->DctData[0].Timings.Speed & 0xFFFF;

  if ((D18F2x2E0_dct0.Field.M1MemClkFreq != 0) || (D18F2x2E0_dct1.Field.M1MemClkFreq != 0)) {
    if (D18F2x2E0_dct0.Field.M1MemClkFreq > D18F2x2E0_dct1.Field.M1MemClkFreq) {
      IDS_HDT_CONSOLE (GNB_TRACE, "D18F2x2E0_dct0: %d\n", D18F2x2E0_dct0.Field.M1MemClkFreq);
      MemorySpeedLow = MEMCLK_ID_TO_FREQ (D18F2x2E0_dct0.Field.M1MemClkFreq);
    } else {
      IDS_HDT_CONSOLE (GNB_TRACE, "D18F2x2E0_dct1: %d\n", D18F2x2E0_dct1.Field.M1MemClkFreq);
      MemorySpeedLow = MEMCLK_ID_TO_FREQ (D18F2x2E0_dct1.Field.M1MemClkFreq);
    }
  } else {
    MemorySpeedLow = DDR667_FREQUENCY;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "Memory Speed Low: %dMHz\n", MemorySpeedLow);
  SmuArg[3] = MemorySpeedLow;
  IDS_HDT_CONSOLE (GNB_TRACE, "Memory Speed High: %dMHz\n", MemorySpeedHigh);
  SmuArg[2] = MemorySpeedHigh;

  // Get VDDIO
  // Take the voltage in volt
  VddIo = 1.5;
  switch (MemInitCompleteData->AmdMemoryVddIo) {
  case VOLT1_5:
    VddIo = 1.5;
    break;
  case VOLT1_35:
    VddIo = 1.35;
    break;
  case VOLT1_25:
    VddIo = 1.25;
    break;
  case VOLT1_2:
    VddIo = 1.2;
    break;
  default:
    ASSERT (FALSE);
  }

  SmuArg[0] = GnbFpLibToUint32 (VddIo);
  // Get VDDR
  // Format is the same as VDDIO
  Vddr = 1.05;
//todo User Options PhyVoltage
  if ((MemInitCompleteData->AmdMemoryVddpVddr.IsValid) && (MemInitCompleteData->AmdMemoryVddpVddr.Voltage == VOLT0_95)) {
    Vddr = 0.95;
  }

  ASSERT (UserOptions.CfgMemoryPhyVoltage < MAX_VDDR);
  SmuArg[1] = GnbFpLibToUint32 (Vddr);

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbSmuPrepareMemoryParamsCZ Exit\n");

}

/*----------------------------------------------------------------------------------------*/
/**
 * GNB initialize at Post after DRAM init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */
EFI_STATUS
EFIAPI
AmdGnbSmuInitAtPostAfterDram (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  EFI_STATUS                Status;
  AMD_CONFIG_PARAMS         *StdHeader;
  GNB_HANDLE                *GnbHandle;
  GNB_BUILD_OPTIONS_CZ      *GnbBuildOptionData;
  GNB_OPTION_PARAM          *GnbOptionParamPtr;
  EFI_BOOT_MODE             BootMode;

  Status = (*PeiServices)->LocatePpi (PeiServices, &gAmdGnbOptionsPpiGuid, 0, NULL, (VOID**)&GnbOptionParamPtr);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "AmdGnbInitAtPostAfterDram SMU Exit-1 Status=%r\n", Status);
    return Status;
  }

  StdHeader = &GnbOptionParamPtr->EarlyParams.StdHeader;

  GnbHandle = GnbGetHandle (StdHeader);
  GnbBuildOptionData = GnbLocateHeapBuffer (AMD_GNB_BUILD_OPTIONS_HANDLE, StdHeader);
  ASSERT (GnbBuildOptionData != NULL);

  (*PeiServices)->GetBootMode ((CONST EFI_PEI_SERVICES**)PeiServices, &BootMode);
  if (BootMode != BOOT_ON_S3_RESUME) {
    GnbSmuPrepareMemoryParams (GnbHandle, GnbBuildOptionData, StdHeader);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmdNbioSmuV8PeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS            Status;

  AGESA_TESTPOINT (TpNbioSmuV8PeiEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioSmuV8PeiEntry Entry\n");

  Status = (*PeiServices)->NotifyPpi (PeiServices, &mGnbSMUEarlyBeforeCPUPeimNotify);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = (*PeiServices)->NotifyPpi (PeiServices, &mGnbPostAfterDramPeimNotify);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioSmuV8PeiEntry Exit\n");
  AGESA_TESTPOINT (TpNbioSmuV8PeiExit, NULL);

  return EFI_SUCCESS;
}
