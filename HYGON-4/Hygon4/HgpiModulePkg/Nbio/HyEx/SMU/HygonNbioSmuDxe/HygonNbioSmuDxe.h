/**
 * @file
 *
 * Fabric Pstate Service definitions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Fabric
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

#ifndef _HYGON_NBIO_SMU__DXE_H_
#define _HYGON_NBIO_SMU__DXE_H_

#include <GnbHsio.h>

#include <Protocol/HygonNbioSmuServicesProtocol.h>
#include <Protocol/HygonCoreTopologyProtocol.h>

EFI_STATUS
EFIAPI
HygonSmuReadFuseByName (
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     FuseName,
  OUT UINT32                                     *FuseValue
  );

EFI_STATUS
EFIAPI
HygonSmuReadFuseByNumber (
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     FuseChain,
  IN  UINT32                                     FuseStartBit,
  IN  UINT32                                     FuseSize,
  OUT UINT32                                     *FuseValue
  );

EFI_STATUS
EFIAPI
HygonSmuServiceRequest (
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     SocketId,
  IN  UINT32                                     PhysicalDieId,
  IN  UINT32                                     ServiceRequest,
  IN  UINT32                                     *InputParameters,
  OUT UINT32                                     *ReturnValues
  );

EFI_STATUS
EFIAPI
HygonSmuRegisterRead (
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     InstanceId,
  IN  UINT32                                     RegisterIndex,
  OUT UINT32                                     *RegisterValue
  );

EFI_STATUS
EFIAPI
HygonSmuRegisterWrite (
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     InstanceId,
  IN  UINT32                                     RegisterIndex,
  IN  UINT32                                     *RegisterValue
  );

EFI_STATUS
EFIAPI
HygonSmuRegisterRMW (
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     InstanceId,
  IN  UINT32                                     RegisterIndex,
  IN  UINT32                                     RegisterANDValue,
  IN  UINT32                                     RegisterORValue
  );

EFI_STATUS
EFIAPI
HygonSmuReadBrandString (
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     InstanceId,
  IN  UINT32                                     BrandStringLength,
  OUT UINT8                                      *BrandString
  );

EFI_STATUS
SmuGetCoreTopologyOnCdd (
  IN       HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Cdd,
  OUT      UINTN                                 *NumberOfComplexes,
  OUT      UINTN                                 *NumberOfCores,
  OUT      UINTN                                 *NumberOfThreads
  );

EFI_STATUS
SmuLaunchThread (
  IN       HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Die,
  IN       UINTN                                 LogicalComplex,
  IN       UINTN                                 LogicalThread
  );

EFI_STATUS
HygonSmuReadCacWeights (
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     MaxNumWeights,
  OUT UINT64                                     *ApmWeights
  );

EFI_STATUS
HygonSmuReadThrottleThresholds (
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL         *This,
  OUT UINT32                                     *Thresholds
  );

EFI_STATUS
SmuGetGnbHandle (
  IN  UINT32                                     SocketId,
  IN  UINT32                                     PhysicalDieId,
  OUT GNB_HANDLE                                 **MyHandle
  );

EFI_STATUS
SmuDxeInitialize (
  OUT PCIe_PLATFORM_CONFIG                       **Pcie
  );

EFI_STATUS
EFIAPI
ServerHotplugInitDxe (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

EFI_STATUS
SmuGetProcessorSN (
  IN       UINT8                               SocketID,
  IN OUT   UINT32                              *ReturnValues
  );

#endif
