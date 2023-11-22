/* $NoKeywords:$ */

/**
 * @file
 *
 * Fabric Pstate Service functions
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
#include "FabricPstateServicesPei.h"
#include <PiPei.h>
#include <Filecode.h>

#define FILECODE  FABRIC_HYGX_FABRICPEI_FABRICPSTATESERVICESPEI_FILECODE

STATIC HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI  mFabricPstateServicesPpi = {
  FabricPstateGetSystemInfo,
  FabricPstateGetSocketInfo,
  FabricPstateGetDieInfo,
  FabricPstateGetPstateInfo,
  FabricPstateForcePstate,
  FabricPstateReleasePstateForce,
  FabricPstateGetCurrentPstate
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mFabricStatesPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonFabricPstateServicesPpiGuid,
  &mFabricPstateServicesPpi
};

EFI_STATUS
EFIAPI
FabricPstateServicePpiInstall (
  IN       CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  // Install Fabric P-state Services
  Status = (**PeiServices).InstallPpi (PeiServices, &mFabricStatesPpiList);

  return Status;
}

/**
 * This service retrieves information about the system's fabric P-states.
 *
 * This function returns whether or not fabric P-states are system-wide.
 *
 * If AreAllSocketPstatesInTheSameDomain is NULL, then EFI_INVALID_PARAMETER is
 * returned. Otherwise, TRUE or FALSE is returned.  If FALSE, the individual
 * sockets are able to transition independently of one another.  If TRUE, all
 * sockets are tied in lock-step fashion to one another.  This enables a call
 * to ForcePstate to transition the entire system.
 *
 * @param[in]  This                                 A pointer to the
 *                                                  HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI instance.
 * @param[out] AreAllSocketPstatesInTheSameDomain   Pointer to whether or not fabric states are
 *                                                  globally defined.  When TRUE, all die in the
 *                                                  system are running at the same speed at all times.
 *
 * @retval EFI_SUCCESS                              The system P-state information was successfully
 *                                                  retrieved.
 * @retval EFI_INVALID_PARAMETER                    AreAllSocketPstatesInTheSameDomain is NULL.
 *
 **/
EFI_STATUS
EFIAPI
FabricPstateGetSystemInfo (
  IN       HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  OUT      BOOLEAN                              *AreAllSocketPstatesInTheSameDomain
  )
{
  if (AreAllSocketPstatesInTheSameDomain == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *AreAllSocketPstatesInTheSameDomain = FabricPstatesAreAllSocketsInAUnifiedDomain ();
  return EFI_SUCCESS;
}

/**
 * This service retrieves information about a particular socket's fabric P-states.
 *
 * This function returns whether or not a particular socket's fabric P-states are
 * globally defined within the socket.
 *
 * If no processor is installed in Socket, then EFI_INVALID_PARAMETER is returned.
 * If AreAllDiePstatesInTheSameDomain is NULL, then EFI_INVALID_PARAMETER is
 * returned. Otherwise, TRUE or FALSE is returned.  If FALSE, the individual
 * die within Socket are able to transition independently of one another.  If
 * TRUE, all die in Socket are tied in lock-step fashion to one another.
 * This enables a call to ForcePstate on any die within Socket to transition
 * all die on socket.
 *
 * @param[in]  This                              A pointer to the
 *                                               HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI instance.
 * @param[in]  Socket                            Zero-based socket number to check.
 * @param[out] AreAllDiePstatesInTheSameDomain   Pointer to whether or not fabric states are
 *                                               globally defined.  When TRUE, all die in the
 *                                               socket are running at the same speed at all times.
 *
 * @retval EFI_SUCCESS                           The socket P-state information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER                 Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER                 AreAllDiePstatesInTheSameDomain is NULL.
 *
 **/
EFI_STATUS
EFIAPI
FabricPstateGetSocketInfo (
  IN       HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  OUT      BOOLEAN                              *AreAllDiePstatesInTheSameDomain
  )
{
  if (AreAllDiePstatesInTheSameDomain == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // Add a check to topology services.  Should probably consolidate all published services
  // to a private structure, similar to HGPI DXE driver so whole module can access.
  if (Socket != 0) {
    return EFI_INVALID_PARAMETER;
  }

  *AreAllDiePstatesInTheSameDomain = FabricPstatesAreAllDieOnSocketInAUnifiedDomain (Socket);
  return EFI_SUCCESS;
}

/**
 * This service retrieves information about a particular die's fabric P-states.
 *
 * This function returns:
 *   1.  whether or not P-state switching is enabled on a particular die
 *   2.  how many unique P-states have been defined on a particular die
 *
 * If no processor is installed in Socket, then EFI_INVALID_PARAMETER is returned.
 * If no die is present at Die within Socket, then EFI_INVALID_PARAMETER is
 * returned.  If both IsSwitchingEnabled and TotalNumberOfStates are NULL, then
 * EFI_INVALID_PARAMETER is returned.  IsSwitchingEnabled will return TRUE or
 * FALSE.  TotalNumberOfStates will return the number of P-states have been
 * defined by hardware.
 *
 * @param[in]  This                  A pointer to the HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI instance.
 * @param[in]  Socket                Zero-based socket number to check.
 * @param[in]  Die                   Zero-based die number on Socket to check.
 * @param[out] IsSwitchingEnabled    Pointer to whether or not transitioning between fabric
 *                                   P-states is enabled on Socket / Die.
 * @param[out] TotalNumberOfStates   Pointer to the number of enabled unique P-states on the
 *                                   given socket / die.
 *
 * @retval EFI_SUCCESS               The die P-state information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER     Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER     Die is non-existent.
 * @retval EFI_INVALID_PARAMETER     Both IsSwitchingEnabled and TotalNumberOfStates are NULL.
 *
 **/
EFI_STATUS
EFIAPI
FabricPstateGetDieInfo (
  IN       HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
  OUT      BOOLEAN                              *IsSwitchingEnabled,
  OUT      UINTN                                *TotalNumberOfStates
  )
{
  if ((IsSwitchingEnabled == NULL) && (TotalNumberOfStates == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  // Add a check to topology services.  Should probably consolidate all published services
  // to a private structure, similar to HGPI DXE driver so whole module can access.
  if (Socket != 0) {
    return EFI_INVALID_PARAMETER;
  }

  // Add a check to topology services.  Should probably consolidate all published services
  // to a private structure, similar to HGPI DXE driver so whole module can access.
  if (Die != 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsSwitchingEnabled != NULL) {
    *IsSwitchingEnabled = FabricPstatesIsSwitchingEnabledOnDie (Socket, Die);
  }

  if (TotalNumberOfStates != NULL) {
    *TotalNumberOfStates = FabricPstatesGetNumberOfPstatesOnDie (Socket, Die);
  }

  return EFI_SUCCESS;
}

/**
 * This service retrieves detailed information about the given fabric P-state.
 *
 * This function returns:
 *   1.  frequency
 *   2.  voltage
 *   3.  consumed power
 *   4.  associated memory state
 *
 * If no processor is installed in Socket, then EFI_INVALID_PARAMETER is returned.
 * If no die is present at Die within Socket, then EFI_INVALID_PARAMETER is
 * returned.  If Pstate does not exist on Socket / Die, then EFI_INVALID_PARAMETER
 * is returned.  If all output parameters are NULL, then EFI_INVALID_PARAMETER is
 * returned.  Otherwise, the desired information is returned.
 *
 * @param[in]  This                  A pointer to the HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI instance.
 * @param[in]  Socket                Zero-based socket number to check.
 * @param[in]  Die                   Zero-based die number on Socket to check.
 * @param[in]  Pstate                Zero-based P-state number on Socket / Die to check.
 * @param[out] Frequency             Pointer to Pstate's frequency in MHz.
 * @param[out] Voltage               Pointer to Pstate's voltage in mV.
 * @param[out] Power                 Pointer to Pstate's consumed power in mW.
 * @param[out] AssociatedMstate      Pointer to the M-state index associated with Pstate.
 *
 * @retval EFI_SUCCESS               The P-state information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER     Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER     Die is non-existent.
 * @retval EFI_INVALID_PARAMETER     Pstate is non-existent.
 * @retval EFI_INVALID_PARAMETER     Frequency, Voltage, Power, and AssociatedMstate are
 *                                   all NULL.
 **/
EFI_STATUS
EFIAPI
FabricPstateGetPstateInfo (
  IN       HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
  IN       UINTN                                Pstate,
     OUT   UINTN                                *Frequency,
     OUT   UINTN                                *Voltage,
     OUT   UINTN                                *Power,
     OUT   UINTN                                *AssociatedMstate
  )
{
  FABRIC_PSTATE_INFO  PstateInfo;

  if ((Frequency == NULL) && (Voltage == NULL) && (Power == NULL) && (AssociatedMstate == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  // Add a check to topology services.  Should probably consolidate all published services
  // to a private structure, similar to HGPI DXE driver so whole module can access.
  if (Socket != 0) {
    return EFI_INVALID_PARAMETER;
  }

  // Add a check to topology services.  Should probably consolidate all published services
  // to a private structure, similar to HGPI DXE driver so whole module can access.
  if (Die != 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (Pstate >= FabricPstatesGetNumberOfPstatesOnDie (Socket, Die)) {
    return EFI_INVALID_PARAMETER;
  }

  FabricPstatesGetPstateInfo (Socket, Die, Pstate, &PstateInfo);

  if (Frequency != NULL) {
    *Frequency = PstateInfo.Frequency;
  }

  if (Voltage != NULL) {
    *Voltage = PstateInfo.Voltage;
  }

  if (Power != NULL) {
    *Power = PstateInfo.Power;
  }

  if (AssociatedMstate != NULL) {
    *AssociatedMstate = PstateInfo.AssociatedMstate;
  }

  return EFI_SUCCESS;
}

/**
 * This service transitions to the given hardware P-state and disables further transitions.
 *
 * If no processor is installed in Socket, then EFI_INVALID_PARAMETER is returned.
 * If no die is present at Die within Socket, then EFI_INVALID_PARAMETER is
 * returned.  If Pstate does not exist on Socket / Die, then EFI_INVALID_PARAMETER
 * is returned.  If the transition fails for any reason, EFI_DEVICE_ERROR is returned.
 *
 * @param[in]  This                  A pointer to the HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI instance.
 * @param[in]  Socket                Zero-based socket number on which to force.
 * @param[in]  Die                   Zero-based die number on Socket on which to force.
 * @param[in]  Pstate                Zero-based P-state number to switch to.
 *
 * @retval EFI_SUCCESS               The socket P-state information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER     Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER     Die is non-existent.
 * @retval EFI_INVALID_PARAMETER     Pstate is non-existent.
 * @retval EFI_DEVICE_ERROR          The P-state force was not successful.
 *
 **/
EFI_STATUS
EFIAPI
FabricPstateForcePstate (
  IN       HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
  IN       UINTN                                Pstate
  )
{
  // Add a check to topology services.  Should probably consolidate all published services
  // to a private structure, similar to HGPI DXE driver so whole module can access.
  if (Socket != 0) {
    return EFI_INVALID_PARAMETER;
  }

  // Add a check to topology services.  Should probably consolidate all published services
  // to a private structure, similar to HGPI DXE driver so whole module can access.
  if (Die != 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (Pstate >= FabricPstatesGetNumberOfPstatesOnDie (Socket, Die)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!FabricPstatesForcePstateOnDie (Socket, Die, Pstate)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
 * This service allows the hardware to transition between P-states freely.  This
 * is intended to run after the period of forcing one particular P-state or
 * another is over.
 *
 * If no processor is installed in Socket, then EFI_INVALID_PARAMETER is returned.
 * If no die is present at Die within Socket, then EFI_INVALID_PARAMETER is
 * returned.
 *
 * @param[in]  This                  A pointer to the HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI instance.
 * @param[in]  Socket                Zero-based socket number on which to release the force.
 * @param[in]  Die                   Zero-based die number on Socket on which to release
 *                                   the force.
 *
 * @retval EFI_SUCCESS               The hardware is able to transition freely.
 * @retval EFI_INVALID_PARAMETER     Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER     Die is non-existent.
 *
 **/
EFI_STATUS
EFIAPI
FabricPstateReleasePstateForce (
  IN       HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die
  )
{
  // Add a check to topology services.  Should probably consolidate all published services
  // to a private structure, similar to HGPI DXE driver so whole module can access.
  if (Socket != 0) {
    return EFI_INVALID_PARAMETER;
  }

  // Add a check to topology services.  Should probably consolidate all published services
  // to a private structure, similar to HGPI DXE driver so whole module can access.
  if (Die != 0) {
    return EFI_INVALID_PARAMETER;
  }

  FabricPstatesReleaseForceOnDie (Socket, Die);

  return EFI_SUCCESS;
}

/**
 *  This service returns the hardware P-state index at which the hardware is
 * currently running.
 *
 * If no processor is installed in Socket, then EFI_INVALID_PARAMETER is returned.
 * If no die is present at Die within Socket, then EFI_INVALID_PARAMETER is
 * returned.  If Pstate is NULL, then EFI_INVALID_PARAMETER is returned.  Otherwise,
 * Pstate returns the zero-based P-state index at which the hardware is currently operating.
 *
 * @param[in]  This                  A pointer to the HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI instance.
 * @param[in]  Socket                Zero-based socket number to check.
 * @param[in]  Die                   Zero-based die number on Socket to check.
 * @param[out] Pstate                Pointer to zero-based P-state number at which
 *                                   Socket / Die is currently operating.
 *
 * @retval EFI_SUCCESS               The socket P-state information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER     Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER     Die is non-existent.
 * @retval EFI_DEVICE_ERROR          Pstate is NULL.
 *
 **/
EFI_STATUS
EFIAPI
FabricPstateGetCurrentPstate (
  IN       HYGON_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
  OUT      UINTN                                *Pstate
  )
{
  if (Pstate == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // Add a check to topology services.  Should probably consolidate all published services
  // to a private structure, similar to HGPI DXE driver so whole module can access.
  if (Socket != 0) {
    return EFI_INVALID_PARAMETER;
  }

  // Add a check to topology services.  Should probably consolidate all published services
  // to a private structure, similar to HGPI DXE driver so whole module can access.
  if (Die != 0) {
    return EFI_INVALID_PARAMETER;
  }

  *Pstate = FabricPstatesGetCurrentPstateOnDie (Socket, Die);

  return EFI_SUCCESS;
}
