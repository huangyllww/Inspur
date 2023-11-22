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

#ifndef _FABRIC_PSTATE_SERVICES_PROTOCOL_H_
#define _FABRIC_PSTATE_SERVICES_PROTOCOL_H_

///
/// Forward declaration for the HYGON_FABRIC_PSTATE_SERVICES_PROTOCOL.
///
typedef struct _HYGON_FABRIC_PSTATE_SERVICES_PROTOCOL HYGON_FABRIC_PSTATE_SERVICES_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *HYGON_FABRIC_PSTATE_SERVICES_GET_SYSTEM_INFO)(
  IN       HYGON_FABRIC_PSTATE_SERVICES_PROTOCOL   *This,
  OUT   BOOLEAN                               *AreAllSocketPstatesInTheSameDomain
  );

typedef
EFI_STATUS
(EFIAPI *HYGON_FABRIC_PSTATE_SERVICES_GET_SOCKET_INFO)(
  IN       HYGON_FABRIC_PSTATE_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  OUT   BOOLEAN                               *AreAllDiePstatesInTheSameDomain
  );

typedef
EFI_STATUS
(EFIAPI *HYGON_FABRIC_PSTATE_SERVICES_GET_DIE_INFO)(
  IN       HYGON_FABRIC_PSTATE_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Die,
  OUT   BOOLEAN                               *IsSwitchingEnabled,
  OUT   UINTN                                 *TotalNumberOfStates
  );

typedef
EFI_STATUS
(EFIAPI *HYGON_FABRIC_PSTATE_SERVICES_GET_PSTATE_INFO)(
  IN       HYGON_FABRIC_PSTATE_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Die,
  IN       UINTN                                 Pstate,
  OUT   UINTN                                 *Frequency,
  OUT   UINTN                                 *Voltage,
  OUT   UINTN                                 *Power,
  OUT   UINTN                                 *AssociatedMstate
  );

typedef
EFI_STATUS
(EFIAPI *HYGON_FABRIC_PSTATE_SERVICES_GET_CURRENT_PSTATE)(
  IN       HYGON_FABRIC_PSTATE_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Die,
  OUT   UINTN                                 *Pstate
  );

///
/// When installed, the Fabric P-state Services Protocol produces a collection of
/// services that return various information about the fabric P-states.
///
struct _HYGON_FABRIC_PSTATE_SERVICES_PROTOCOL {
  HYGON_FABRIC_PSTATE_SERVICES_GET_SYSTEM_INFO       GetSystemInfo;
  HYGON_FABRIC_PSTATE_SERVICES_GET_SOCKET_INFO       GetSocketInfo;
  HYGON_FABRIC_PSTATE_SERVICES_GET_DIE_INFO          GetDieInfo;
  HYGON_FABRIC_PSTATE_SERVICES_GET_PSTATE_INFO       GetPstateInfo;
  HYGON_FABRIC_PSTATE_SERVICES_GET_CURRENT_PSTATE    GetCurrentPstate;
};

extern EFI_GUID  gHygonFabricPstateServicesProtocolGuid;

#endif
