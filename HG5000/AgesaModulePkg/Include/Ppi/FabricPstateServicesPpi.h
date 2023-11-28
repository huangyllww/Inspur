/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Pstate Services PPI prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
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
 *
 ***************************************************************************/

#ifndef _FABRIC_PSTATE_SERVICES_PPI_H_
#define _FABRIC_PSTATE_SERVICES_PPI_H_

#define NB_HIGH 0x80
#define NB_LOW  0X81

///
/// Forward declaration for the AMD_PEI_FABRIC_PSTATE_SERVICES_PPI.
///
typedef struct _AMD_PEI_FABRIC_PSTATE_SERVICES_PPI AMD_PEI_FABRIC_PSTATE_SERVICES_PPI;

typedef
EFI_STATUS
(EFIAPI *AMD_PEI_FABRIC_PSTATE_SERVICES_GET_SYSTEM_INFO) (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
     OUT   BOOLEAN                              *AreAllSocketPstatesInTheSameDomain
  );

typedef
EFI_STATUS
(EFIAPI *AMD_PEI_FABRIC_PSTATE_SERVICES_GET_SOCKET_INFO) (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
     OUT   BOOLEAN                              *AreAllDiePstatesInTheSameDomain
  );

typedef
EFI_STATUS
(EFIAPI *AMD_PEI_FABRIC_PSTATE_SERVICES_GET_DIE_INFO) (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
     OUT   BOOLEAN                              *IsSwitchingEnabled,
     OUT   UINTN                                *TotalNumberOfStates
  );

typedef
EFI_STATUS
(EFIAPI *AMD_PEI_FABRIC_PSTATE_SERVICES_GET_PSTATE_INFO) (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
  IN       UINTN                                Pstate,
     OUT   UINTN                                *Frequency,
     OUT   UINTN                                *Voltage,
     OUT   UINTN                                *Power,
     OUT   UINTN                                *AssociatedMstate
  );

typedef
EFI_STATUS
(EFIAPI *AMD_PEI_FABRIC_PSTATE_SERVICES_FORCE_PSTATE) (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
  IN       UINTN                                Pstate
  );

typedef
EFI_STATUS
(EFIAPI *AMD_PEI_FABRIC_PSTATE_SERVICES_RELEASE_PSTATE_FORCE) (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die
  );

typedef
EFI_STATUS
(EFIAPI *AMD_PEI_FABRIC_PSTATE_SERVICES_GET_CURRENT_PSTATE) (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
     OUT   UINTN                                *Pstate
  );

///
/// When installed, the Fabric P-state Services PPI produces a collection of
/// services that are needed for Fabric P-state management.
///
/// When using the P-state force mechanism, the caller should take care to
/// release the forcing mechanism prior to continuing the boot process.  The
/// Fabric DXE driver will ensure that the force is disabled early in the DXE
/// phase in case the call to release was not made.
///
struct _AMD_PEI_FABRIC_PSTATE_SERVICES_PPI {
  AMD_PEI_FABRIC_PSTATE_SERVICES_GET_SYSTEM_INFO        GetSystemInfo;
  AMD_PEI_FABRIC_PSTATE_SERVICES_GET_SOCKET_INFO        GetSocketInfo;
  AMD_PEI_FABRIC_PSTATE_SERVICES_GET_DIE_INFO           GetDieInfo;
  AMD_PEI_FABRIC_PSTATE_SERVICES_GET_PSTATE_INFO        GetPstateInfo;
  AMD_PEI_FABRIC_PSTATE_SERVICES_FORCE_PSTATE           ForcePstate;
  AMD_PEI_FABRIC_PSTATE_SERVICES_RELEASE_PSTATE_FORCE   ReleasePstateForce;
  AMD_PEI_FABRIC_PSTATE_SERVICES_GET_CURRENT_PSTATE     GetCurrentPstate;
};

extern EFI_GUID gAmdFabricPstateServicesPpiGuid;

#endif
