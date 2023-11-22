/* $NoKeywords:$ */

/**
 * @file
 *
 * Fabric Topology Services -
 *
 * Contains code that provides fabric topology services
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
#include "HGPI.h"
#include <PiDxe.h>
#include <Library/SmmServicesTableLib.h>
#include "Library/IdsLib.h"
#include <Protocol/FabricTopologyServices.h>
#include <Filecode.h>
#include "HGPI.h"
#include "FabricTopologyServicesSmm.h"

#define FILECODE  LIBRARY_SMMFABRICTOPOLOGYSERVICESLIB_FABRICTOPOLOGYSERVICESSMM_FILECODE

STATIC HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  mFabricTopologyServicesSmmProtocol = {
  FabricTopologyGetSystemInfo,
  FabricTopologyGetProcessorInfo,
  FabricTopologyGetDieInfo,
  FabricTopologyGetRootBridgeInfo,
  FabricTopologyGetCddInfo
};

EFI_STATUS
EFIAPI
FabricTopologyServiceSmmProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;

  // Install Fabric Topology Services
  Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gHygonFabricTopologyServicesSmmProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mFabricTopologyServicesSmmProtocol
                    );

  return Status;
}

/**
 * This service retrieves information about the overall system with respect to data fabric.
 *
 * @param[in]  This                           A pointer to the
 *                                            HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL instance.
 * @param[out] NumberOfInstalledProcessors    Pointer to the total number of populated
 *                                            processor sockets in the system.
 * @param[out] TotalNumberOfIoDie             Pointer to the total number of IO die in the system.
 * @param[out] TotalNumberOfRootBridges       Pointer to the total number of root PCI bridges in
 *                                            the system.
 *
 * @retval EFI_SUCCESS                        The system topology information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 *
 **/
EFI_STATUS
EFIAPI
FabricTopologyGetSystemInfo (
  IN       HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *This,
  OUT   UINTN                                  *NumberOfInstalledProcessors,
  OUT   UINTN                                  *TotalNumberOfIoDie,
  OUT   UINTN                                  *TotalNumberOfRootBridges
  )
{
  if ((NumberOfInstalledProcessors == NULL) && (TotalNumberOfIoDie == NULL) && (TotalNumberOfRootBridges == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (NumberOfInstalledProcessors != NULL) {
    *NumberOfInstalledProcessors = FabricTopologyGetNumberOfSocketPresent ();
  }

  if (TotalNumberOfIoDie != NULL) {
    *TotalNumberOfIoDie = FabricTopologyGetNumberOfSocketPresent () * FabricTopologyGetNumberOfLogicalDiesOnSocket (0);
  }

  if (TotalNumberOfRootBridges != NULL) {
    *TotalNumberOfRootBridges = FabricTopologyGetNumberOfSystemRootBridges ();
  }

  return EFI_SUCCESS;
}

/**
 * This service retrieves information about the processor installed in the given socket.
 *
 * If no processor is installed in Socket, then EFI_INVALID_PARAMETER is returned.
 *
 * @param[in]  This                           A pointer to the
 *                                            HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL instance.
 * @param[in]  Socket                         Zero-based socket number to check.
 * @param[out] NumberOfDie                    Pointer to the number of die present on the
 *                                            given processor.
 * @param[out] NumberOfRootBridges            Pointer to the number of root PCI bridges on
 *                                            the given processor.
 *
 * @retval EFI_SUCCESS                        The processor information was retrieved successfully.
 * @retval EFI_INVALID_PARAMETER              Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 *
 **/
EFI_STATUS
EFIAPI
FabricTopologyGetProcessorInfo (
  IN       HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL           *This,
  IN       UINTN                                             Socket,
  OUT      UINTN                                             *NumberOfPhysicalDie,
  OUT      UINTN                                             *NumberOfLogicalDie,
  OUT      UINTN                                             *NumberOfRootBridges
  )
{
  if ((NumberOfPhysicalDie == NULL) && (NumberOfLogicalDie == NULL) && (NumberOfRootBridges == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Socket >= FabricTopologyGetNumberOfSocketPresent ()) {
    return EFI_INVALID_PARAMETER;
  }

  if (NumberOfPhysicalDie != NULL) {
    *NumberOfPhysicalDie = FabricTopologyGetNumberOfPhysicalDiesOnSocket (Socket);
  }

  if (NumberOfLogicalDie != NULL) {
    *NumberOfLogicalDie = FabricTopologyGetNumberOfLogicalDiesOnSocket (Socket);
  }

  if (NumberOfRootBridges != NULL) {
    *NumberOfRootBridges = FabricTopologyGetNumberOfRootBridgesOnSocket (Socket);
  }

  return EFI_SUCCESS;
}

/**
 * This service retrieves information about the given die.
 *
 * @param[in]  This                  A pointer to the
 *                                   HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL instance.
 * @param[in]  Socket                The target die's socket identifier.
 * @param[in]  LogicalDie            The target logical die's identifier within Socket.
 * @param[out] NumberOfRootBridges   Pointer to the number of root PCI bridges
 *                                   present on the given die.
 * @param[out] SystemIdOffset        Pointer to the die's offset for all of its
 *                                   devices.
 * @param[out] FabricIdMap           Pointer to an array describing the devices
 *                                   present within the given die's fabric.
 *
 * @retval EFI_SUCCESS               The die information was retrieved successfully.
 * @retval EFI_INVALID_PARAMETER     Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER     Die is non-existent.
 * @retval EFI_INVALID_PARAMETER     All output parameter pointers are NULL.
 *
 **/
EFI_STATUS
EFIAPI
FabricTopologyGetDieInfo (
  IN       HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL     *This,
  IN       UINTN                                       Socket,
  IN       UINTN                                       LogicalDie,
  OUT      UINTN                                       *NumberOfRootBridges,
  OUT      UINTN                                       *SystemIdOffset,
  OUT      HYGON_FABRIC_TOPOLOGY_DIE_DEVICE_MAP        **FabricIdMap
  )
{
  if ((NumberOfRootBridges == NULL) && (SystemIdOffset == NULL) && (FabricIdMap == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Socket >= FabricTopologyGetNumberOfSocketPresent ()) {
    return EFI_INVALID_PARAMETER;
  }

  if (LogicalDie >= FabricTopologyGetNumberOfLogicalDiesOnSocket (Socket)) {
    return EFI_INVALID_PARAMETER;
  }

  if (NumberOfRootBridges != NULL) {
    *NumberOfRootBridges = FabricTopologyGetNumberOfRootBridgesOnDie (Socket, LogicalDie);
  }

  if (SystemIdOffset != NULL) {
    *SystemIdOffset = FabricTopologyGetDieSystemOffset (Socket, LogicalDie);
  }

  if (FabricIdMap != NULL) {
    *FabricIdMap = (HYGON_FABRIC_TOPOLOGY_DIE_DEVICE_MAP *)FabricTopologyGetDeviceMapOnDie (Socket, LogicalDie);
  }

  return EFI_SUCCESS;
}

/**
 * This service retrieves information about the given root PCI bridge.
 *
 * @param[in]  This                  A pointer to the
 *                                   HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL instance.
 * @param[in]  Socket                The target root bridge's socket identifier.
 * @param[in]  LogicalDie            The target root bridge's logical die identifier within Socket.
 * @param[in]  Index                 The target root bridge's 0-based index on die
 * @param[out] SystemFabricID        Pointer to the root bridge's fabric identifier
 *                                   within the system.
 * @param[out] BusNumberBase         Pointer to the root bridge's base PCI bus
 *                                   number in the system.
 * @param[out] BusNumberLimit        Pointer to the root bridge's maximum decoded
 *                                   PCI bus number in the system.
 *
 * @retval EFI_SUCCESS               The root bridge's information was retrieved successfully.
 * @retval EFI_INVALID_PARAMETER     Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER     Die is non-existent.
 * @retval EFI_INVALID_PARAMETER     FabricId is non-existent.
 * @retval EFI_INVALID_PARAMETER     Socket / Die / FabricId does not have a PCI bus range.
 * @retval EFI_INVALID_PARAMETER     All output parameter pointers are NULL.
 *
 **/
EFI_STATUS
EFIAPI
FabricTopologyGetRootBridgeInfo (
  IN       HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *This,
  IN       UINTN                                   Socket,
  IN       UINTN                                   LogicalDie,
  IN       UINTN                                   Index,
  OUT      UINTN                                   *SystemFabricID,
  OUT      UINTN                                   *BusNumberBase,
  OUT      UINTN                                   *BusNumberLimit
  )
{
  if ((SystemFabricID == NULL) && (BusNumberBase == NULL) && (BusNumberLimit == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Socket >= FabricTopologyGetNumberOfSocketPresent ()) {
    return EFI_INVALID_PARAMETER;
  }

  if (LogicalDie >= FabricTopologyGetNumberOfLogicalDiesOnSocket (Socket)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Index >= FabricTopologyGetNumberOfRootBridgesOnDie (Socket, LogicalDie)) {
    return EFI_INVALID_PARAMETER;
  }

  if (SystemFabricID != NULL) {
    *SystemFabricID = FabricTopologyGetHostBridgeSystemFabricID (Socket, LogicalDie, Index);
  }

  if (BusNumberBase != NULL) {
    *BusNumberBase = FabricTopologyGetHostBridgeBusBase (Socket, LogicalDie, Index);
  }

  if (BusNumberLimit != NULL) {
    *BusNumberLimit = FabricTopologyGetHostBridgeBusLimit (Socket, LogicalDie, Index);
  }

  return EFI_SUCCESS;
}

/**
 * This service retrieves CDD information about the given socket.
 * @param[in]  This                  A pointer to the
 *                                   HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL instance.
 * @param[in]  Socket                The target root bridge's socket identifier.

 *                                   within the system.
 * @param[out] NumberOfCdd           Pointer to the CDD number of
 *                                   this socket in the system.
 * @param[out] CddsPresent           Pointer to the all CDD present on this socket
 *                                   BIT 0~7 indicate CDD 0~7 present
 * @retval EFI_SUCCESS               The root bridge's information was retrieved successfully.
 * @retval EFI_INVALID_PARAMETER     Socket is non-existent.
 *
 **/
EFI_STATUS
EFIAPI
FabricTopologyGetCddInfo (
  IN       HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *This,
  IN       UINTN                                   Socket,
  OUT      UINTN                                   *NumberOfCdd,
  OUT      UINTN                                   *CddsPresent
  )
{
  if (Socket >= FabricTopologyGetNumberOfSocketPresent ()) {
    return EFI_INVALID_PARAMETER;
  }

  if (NumberOfCdd == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (NumberOfCdd != NULL) {
    *NumberOfCdd = FabricTopologyGetNumberOfCddsOnSocket (Socket);
  }

  if (CddsPresent != NULL) {
    *CddsPresent = FabricTopologyGetCddsPresentOnSocket (Socket);
  }

  return EFI_SUCCESS;
}
