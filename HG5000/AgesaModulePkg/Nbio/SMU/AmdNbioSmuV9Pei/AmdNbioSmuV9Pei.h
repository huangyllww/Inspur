/**
 * @file
 *
 * SMU V9 Service definitions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision:
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
#ifndef _AMD_NBIO_SMU_V9_PEI_H_
#define _AMD_NBIO_SMU_V9_PEI_H_

#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/AmdCoreTopologyServicesPpi.h>

EFI_STATUS
EFIAPI
SmuReadFuseByName (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              FuseName,
  OUT UINT32                              *FuseValue
  );

EFI_STATUS
EFIAPI
SmuReadFuseByLocation (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              FuseChain,
  IN  UINT32                              FuseStartBit,
  IN  UINT32                              FuseSize,
  OUT UINT32                              *FuseValue
  );

EFI_STATUS
EFIAPI
SmuServiceRequest (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              ServiceRequest,
  IN  UINT32                              *InputParameters,
  OUT UINT32                              *ReturnValues
  );

EFI_STATUS
EFIAPI
SmuRegisterRead (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              RegisterIndex,
  OUT UINT32                              *RegisterValue
  );

EFI_STATUS
EFIAPI
SmuRegisterWrite (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              RegisterIndex,
  IN  UINT32                              *RegisterValue
  );

EFI_STATUS
EFIAPI
SmuRegisterRMW (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              RegisterIndex,
  IN  UINT32                              RegisterANDValue,
  IN  UINT32                              RegisterORValue
  );


EFI_STATUS
EFIAPI
SmuHtcControl (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  OUT BOOLEAN                             *HtcStatus
  );

EFI_STATUS
EFIAPI
SmuReadCacWeights (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              MaxNumWeights,
  OUT UINT64                              *ApmWeights,
  OUT UINT32                              *NumWeightsRead,
  OUT UINT32                              *EnergyCountNormalize
  );

EFI_STATUS
EFIAPI
SmuPsiControl (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              TargetVid,
  IN  UINT32                              TargetNbVid
  );

EFI_STATUS
EFIAPI
SmuGetTdpValue (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  OUT UINT32                              *TdpValue
  );

EFI_STATUS
EFIAPI
SmuReadBrandString (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              BrandStringLength,
  OUT UINT8                               *BrandString
  );

EFI_STATUS
EFIAPI
SmuReadCorePllLockTimer (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI        *This,
  IN  UINT32                               InstanceId,
  OUT UINT32                               *LockTimerValue
  );

EFI_STATUS
AmdNbioSmuEarlyInit (
  IN       PCIe_PLATFORM_CONFIG            *Pcie
  );

#endif
