/**
 * @file
 *
 * Fabric Pstate Service definitions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
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
#ifndef _AMD_NBIO_SMU_V10_DXE_H_
#define _AMD_NBIO_SMU_V10_DXE_H_

#include <Protocol/AmdNbioSmuServicesProtocol.h>
#include <Protocol/AmdCoreTopologyProtocol.h>

EFI_STATUS
EFIAPI
AmdSmuReadFuseByName (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     FuseName,
  OUT UINT32                                     *FuseValue
  );

EFI_STATUS
EFIAPI
AmdSmuReadFuseByNumber (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     FuseChain,
  IN  UINT32                                     FuseStartBit,
  IN  UINT32                                     FuseSize,
  OUT UINT32                                     *FuseValue
  );

EFI_STATUS
EFIAPI
AmdSmuServiceRequest (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     InstanceId,
  IN  UINT32                                     ServiceRequest,
  IN  UINT32                                     *InputParameters,
  OUT UINT32                                     *ReturnValues
  );

EFI_STATUS
EFIAPI
AmdSmuRegisterRead (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     InstanceId,
  IN  UINT32                                     RegisterIndex,
  OUT UINT32                                     *RegisterValue
  );

EFI_STATUS
EFIAPI
AmdSmuRegisterWrite (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     InstanceId,
  IN  UINT32                                     RegisterIndex,
  IN  UINT32                                     *RegisterValue
  );

EFI_STATUS
EFIAPI
AmdSmuRegisterRMW (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     InstanceId,
  IN  UINT32                                     RegisterIndex,
  IN  UINT32                                     RegisterANDValue,
  IN  UINT32                                     RegisterORValue
  );

EFI_STATUS
EFIAPI
AmdSmuReadBrandString (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     InstanceId,
  IN  UINT32                                     BrandStringLength,
  OUT UINT8                                      *BrandString
  );

BOOLEAN
AmdSmuIsSmtEnable (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This
  );

EFI_STATUS
SmuV10GetCoreTopologyOnDie (
  IN       AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Die,
     OUT   UINTN                                 *NumberOfComplexes,
     OUT   UINTN                                 *NumberOfCores,
     OUT   UINTN                                 *NumberOfThreads
  );

EFI_STATUS
SmuV10LaunchThread (
  IN       AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Die,
  IN       UINTN                                 LogicalComplex,
  IN       UINTN                                 LogicalThread
  );

EFI_STATUS
AmdSmuReadCacWeights (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     MaxNumWeights,
  OUT UINT64                                     *ApmWeights
  );

EFI_STATUS
AmdSmuReadThrottleThresholds (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  OUT UINT32                                     *Thresholds
  );

EFI_STATUS
SmuGetGnbHandle (
  IN  UINT32                                     InstanceId,
  OUT GNB_HANDLE                                 **MyHandle
  );

EFI_STATUS
SmuDxeInitialize (
  OUT PCIe_PLATFORM_CONFIG                       **Pcie
  );
#endif
