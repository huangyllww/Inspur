/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioSmuV8Dxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioSmuV8Dxe
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
#include <AmdNbioSmuV8Dxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/GnbHeapLib.h>
#include <Library/GnbHandleLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/GnbSmuInitV8Lib.h>

#define FILECODE        NBIO_SMU_AMDNBIOSMUV8DXE_AMDNBIOSMUV8DXE_FILECODE

DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL mDxeAmdNbioSmuServicesProtocol = {
  DXE_AMD_NBIO_SMU_SERVICES_DXE_REVISION,
  AmdSmuReadFuseByName,
  AmdSmuReadFuseByNumber,
  AmdSmuServiceRequest,
  AmdSmuRegisterRead,
  AmdSmuRegisterWrite,
  AmdSmuRegisterRMW
};

/**
  Reads a fuse value based on an enumerated list of fuse "names"
  This
    A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL instance.
  FuseName
    The "name" of the fuse from an enumerated list of fuses supported by this function
  FuseValue
    Pointer to the value of the fuse
**/
EFI_STATUS
EFIAPI
AmdSmuReadFuseByName (
  IN       DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              FuseName,
     OUT   UINT32                              *FuseValue
  )
{
  return 0;
}


/**
  Reads a fuse value based on chain, start bit, and size

  This
    A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL instance.
  FuseChain
    The chain number of the fuse to read
  FuseStartBit
    The starting bit within the specified fuse chain of the fuse to read
  FuseSize
    The bitfield width of the fuse to read
  FuseValue
    Pointer to the value of the fuse
**/
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
  return 0;
}

/**
  Executes a specific SMU service and returns the results

  This
    A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL instance.
  ServiceRequest
    The service request identifier
  InputParameters
    Pointer to an array of service request arguments (for CZ, UINT32[6])
  ReturnValues
    Pointer to an array of service request response values (for CZ, UINT32[6])
**/
EFI_STATUS
AmdSmuServiceRequest (
  IN       DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              ServiceRequest,
  IN       UINT32                              *InputParameters,
     OUT   UINT32                              *ReturnValues
  )
{
  return 0;
}

/**
  Reads a 32-bit register from SMU register space

  This
    A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
  RegisterIndex
    The index of the register to be read
  RegisterValue
    Pointer to a UINT32 to store the value read from the register
**/
EFI_STATUS
AmdSmuRegisterRead (
  IN       DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              InstanceId,
  IN       UINT32                              RegisterIndex,
     OUT   UINT32                              *RegisterValue
  )
{
  return 0;
}

/**
  Writes a 32-bit register in SMU register space

  This
    A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
  RegisterIndex
    The index of the register to be read
  RegisterValue
    Pointer to a UINT32 holding the value to write to the register
**/
EFI_STATUS
AmdSmuRegisterWrite (
  IN       DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              InstanceId,
  IN       UINT32                              RegisterIndex,
  IN       UINT32                              *RegisterValue
  )
{
  return 0;
}

/**
  Read/Modify/Write a 32-bit register from SMU register space

  This
    A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL instance.
  InstanceId
    The unique identifier of the NBIO instance associated with this socket/die
  RegisterIndex
    The index of the register to be read
  RegisterANDValue
    UINT32 holding the value to be "ANDed" with the data read from the register (i.e. mask of data that will not be modified)
  RegisterORValue
    UINT32 holding the value to be "ORed" with the data read from the register (i.e. data bits to be "set" by this function)
**/
EFI_STATUS
AmdSmuRegisterRMW (
  IN       DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL  *This,
  IN       UINT32                              InstanceId,
  IN       UINT32                              RegisterIndex,
  IN       UINT32                              RegisterANDValue,
  IN       UINT32                              RegisterORValue
  )
{
  return 0;
}

EFI_STATUS
EFIAPI
AmdNbioSmuV8DxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           Handle;
#if 0
  AMD_CONFIG_PARAMS                *StdHeader;
  GNB_HANDLE                       *GnbHandle;
#endif

  AGESA_TESTPOINT (TpNbioSmuV8DxeEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioSmuV8DxeEntry\n");
  Handle = NULL;

  Status = gBS->InstallProtocolInterface (
                &Handle,
                &gAmdNbioSmuServicesProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mDxeAmdNbioSmuServicesProtocol
                );
#if 0
//
// Code flow of Arch 2008
//
  StdHeader = AllocatePages (sizeof (AMD_CONFIG_PARAMS));
  GnbHandle = GnbGetHandle (StdHeader);
  //
  //OPTION_GNBEARLYINTERFACECZ_ENTRY
  //
  GnbEarlyInterfaceCZ (StdHeader);

  //
  //  OPTION_GNBPOSTINTERFACECZ_ENTRY
  //
  GnbSmuRequestGpuPowerV8 (GnbHandle, SMC_MSG_PowerUpGpu, StdHeader);

  //
  //  OPTION_GNBSMUPOSTINTERFACECZ_ENTRY
  //
  GnbSmuPostInterfaceCZ (StdHeader);

  //
  //OPTIONBOOTTIMECALLATECZ_ENTRY
  //
  GnbBootTimeCalInterfaceCZ (StdHeader);

  //
  //OPTION_GNBSMUMIDINTERFACECZ_ENTRY
  //
  GnbSmuMidInterfaceCZ (StdHeader);

  //END OF MID

  //
  //OPTION_GNBSMUMIDLATEINTERFACECZ_ENTRY
  //

  GnbSmuMidLateInterfaceCZ (StdHeader);
  //END OF MID LATE
  //END OF LATE
#endif

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioSmuV8DxeExit\n");
  AGESA_TESTPOINT (TpNbioSmuV8DxeExit, NULL);

  return EFI_SUCCESS;
}
