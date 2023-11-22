/* $NoKeywords:$ */

/**
 * @file
 *
 * Fabric Pstate Services Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Fabric
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
 *
 ***************************************************************************/

#ifndef _NBIO_SMU_SERVICES_PROTOCOL_H_
#define _NBIO_SMU_SERVICES_PROTOCOL_H_

// Current PROTOCOL revision
#define DXE_HYGON_NBIO_SMU_SERVICES_DXE_REVISION  0x00

///
/// Forward declaration for the NBIO_SMU_SERVICES_PROTOCOL
///
typedef struct _DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL;

//
// Protocol Definitions
//

/**
  Reads a fuse value based on an enumerated list of fuse "names"
  This
    A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
  FuseChain
    The "name" of the fuse from an enumerated list of fuses supported by this function
  FuseValue
    Pointer to the value of the fuse
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_SMU_SERVICES_READ_FUSE_BY_NAME)(
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN  UINT32  FuseName,
  OUT UINT32  *FuseValue
  );

/**
  Reads a fuse value based on chain, start bit, and size

  This
    A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
  FuseChain
    The chain number of the fuse to read
  FuseStartBit
    The starting bit within the specified fuse chain of the fuse to read
  FuseSize
    The bitfield width of the fuse to read
  FuseValue
    Pointer to the value of the fuse
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_SMU_SERVICES_READ_FUSE_BY_NUMBER)(
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN  UINT32  FuseChain,
  IN  UINT32  FuseStartBit,
  IN  UINT32  FuseSize,
  OUT UINT32  *FuseValue
  );

/**
  Executes a specific SMU service and returns the results

  This
    A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
  SocketId
    The unique identifier of  this socket
  PhysicalDieId
    The unique identifier of  this Physical Die
  ServiceRequest
    The service request identifier
  InputParameters
    Pointer to an array of service request arguments (for CZ, UINT32[6])
  ReturnValues
    Pointer to an array of service request response values (for CZ, UINT32[6])
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_SMU_SERVICES_SMU_SERVICE_REQUEST)(
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN  UINT32                              SocketId,
  IN  UINT32                              PhysicalDieId,
  IN  UINT32                              ServiceRequest,
  IN  UINT32                              *InputParameters,
  OUT UINT32                              *ReturnValues
  );

/**
  Reads a 32-bit register from SMU register space

  This
    A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
  InstanceId
    The unique identifier of this socket
  RegisterIndex
    The index of the register to be read
  RegisterValue
    Pointer to a UINT32 to store the value read from the register
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_SMU_SERVICES_SMU_REGISTER_READ)(
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN  UINT32  InstanceId,
  IN  UINT32  RegisterIndex,
  OUT UINT32  *RegisterValue
  );

/**
  Writes a 32-bit register in SMU register space

  This
    A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
  InstanceId
    The unique identifier of this socket
  RegisterIndex
    The index of the register to be read
  RegisterValue
    Pointer to a UINT32 holding the value to write to the register
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_SMU_SERVICES_SMU_REGISTER_WRITE)(
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN  UINT32  InstanceId,
  IN  UINT32  RegisterIndex,
  IN  UINT32  *RegisterValue
  );

/**
  Read/Modify/Write a 32-bit register from SMU register space

  This
    A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
  InstanceId
    The unique identifier of this socket
  RegisterIndex
    The index of the register to be read
  RegisterANDValue
    UINT32 holding the value to be "ANDed" with the data read from the register (i.e. mask of data that will not be modified)
  RegisterORValue
    UINT32 holding the value to be "ORed" with the data read from the register (i.e. data bits to be "set" by this function)
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_SMU_SERVICES_SMU_REGISTER_RMW)(
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN  UINT32  InstanceId,
  IN  UINT32  RegisterIndex,
  IN  UINT32  RegisterANDValue,
  IN  UINT32  RegisterORValue
  );

/**
  Returns the brand string read from SMN space.

  Parameters:
  This
    A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
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
typedef
EFI_STATUS
(EFIAPI *HYGON_SMU_SERVICES_READ_BRAND_STRING)(
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              BrandStringLength,
  OUT UINT8                               *BrandString
  );

/**
  Returns the CAC weights read from SMU

  Parameters:
  This
    A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
  MaxNumWeights
    The number of values to read from the SMU (defines the maximum size of ApmWeights return data buffer)
  DpmWeights
    Pointer to return data buffer

  Status Codes Returned:
  EFI_SUCCESS           - ApmWeights contains the CaC weights read from SMU
                        - ApmWeights contains the number of values available from SMU
                        - EnergyCountNormalize contains the value returned by SMU
  EFI_INVALID_PARAMETER - One of the input parameters was invalid
                        - InstanceId did not reference a valid NBIO instance
  EFI_BUFFER_TOO_SMALL  - The number of weights available exceeds MaxNumWeights
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_SMU_SERVICES_READ_CAC_WEIGHTS)(
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN  UINT32                              MaxNumWeights,
  OUT UINT64                              *DpmWeights
  );

/**
  Returns the throttle thresholds read from SMU

  Parameters:
  This
    A pointer to the DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL instance.
  Thresholds
    Pointer to return data buffer

  Status Codes Returned:
  EFI_SUCCESS           - ApmWeights contains the throttle thresholds read from SMU
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_SMU_SERVICES_Read_THROTTLE_THRESHOLDS)(
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *This,
  OUT UINT32                              *Thresholds
  );

///
/// The Dxe of SMU Services
///
struct _DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL {
  UINT32                                         Revision;                 ///< revision
  HYGON_SMU_SERVICES_READ_FUSE_BY_NAME           HygonSmuReadFuseByName;   ///<
  HYGON_SMU_SERVICES_READ_FUSE_BY_NUMBER         HygonSmuReadFuseByNumber; ///<
  HYGON_SMU_SERVICES_SMU_SERVICE_REQUEST         HygonSmuServiceRequest;   ///<
  HYGON_SMU_SERVICES_SMU_REGISTER_READ           HygonSmuRegisterRead;     ///<
  HYGON_SMU_SERVICES_SMU_REGISTER_WRITE          HygonSmuRegisterWrite;    ///<
  HYGON_SMU_SERVICES_SMU_REGISTER_RMW            HygonSmuRegisterRMW;      ///<
  HYGON_SMU_SERVICES_READ_BRAND_STRING           HygonSmuReadBrandString;  ///<
  HYGON_SMU_SERVICES_READ_CAC_WEIGHTS            HygonSmuReadCacWeights;   ///<
  HYGON_SMU_SERVICES_Read_THROTTLE_THRESHOLDS    HygonSmuReadThrottleThresholds; ///<
};

extern EFI_GUID  gHygonNbioSmuServicesProtocolGuid;

extern EFI_GUID  gHygonProcessorServicesProtocolGuid;

typedef EFI_STATUS (EFIAPI *DXE_HYGON_PROCESSOR_SN)(
  IN       UINT8                               SocketID,
  IN OUT   UINT32                              *ReturnValues
  );

typedef struct _HYGON_PROCESSOR_SN_PROTOCOL {
  UINTN                     Revision;                          ///< Protocol Revision
  DXE_HYGON_PROCESSOR_SN    GetProcessorSN;
} HYGON_PROCESSOR_SN_PROTOCOL;

#endif
