/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Topology Services -
 *
 * Contains code that provides fabric topology services
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
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
#include "AGESA.h"
#include <PiDxe.h>
#include <Library/SmmServicesTableLib.h>
#include "Library/IdsLib.h"
#include <Protocol/FabricTopologyServices.h>
#include <Filecode.h>
#include "AGESA.h"
#include "FabricTopologyServicesSmm.h"

#define FILECODE LIBRARY_SMMFABRICTOPOLOGYSERVICESLIB_FABRICTOPOLOGYSERVICESSMM_FILECODE

STATIC AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  mFabricTopologyServicesSmmProtocol = {
  FabricTopologyGetSystemInfo,
  FabricTopologyGetProcessorInfo,
  FabricTopologyGetDieInfo,
  FabricTopologyGetRootBridgeInfo
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
                &gAmdFabricTopologyServicesSmmProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mFabricTopologyServicesSmmProtocol
                );

  return Status;
}

/**
 * This service retrieves information about the overall system with respect to data fabric.
 *
 * @param[in]  This                           A pointer to the
 *                                            AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL instance.
 * @param[out] NumberOfInstalledProcessors    Pointer to the total number of populated
 *                                            processor sockets in the system.
 * @param[out] TotalNumberOfDie               Pointer to the total number of die in the system.
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
  IN       AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *This,
     OUT   UINTN                                  *NumberOfInstalledProcessors,
     OUT   UINTN                                  *TotalNumberOfDie,
     OUT   UINTN                                  *TotalNumberOfRootBridges
  )
{
  if ((NumberOfInstalledProcessors == NULL) && (TotalNumberOfDie == NULL) && (TotalNumberOfRootBridges == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (NumberOfInstalledProcessors != NULL) {
    *NumberOfInstalledProcessors = FabricTopologyGetNumberOfProcessorsPresent ();
  }

  if (TotalNumberOfDie != NULL) {
    *TotalNumberOfDie = FabricTopologyGetNumberOfSystemDies ();
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
 *                                            AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL instance.
 * @param[in]  Socket                         Zero-based socket number to check.
 * @param[out] NumberOfDie                    Pointer to the number of die present on the
 *                                            given processor.
 * @param[out] NumberOfRootBridges            Pointer to the number of root PCI bridges on
 *                                            the given processor.
 * @param[out] NumberOfNeighbors              Pointer to the number of other processors that
 *                                            the given processor is physically connected to.
 * @param[out] NeighborInfo                   Pointer to information about the given
 *                                            processor's neighbors, if applicable.
 *
 * @retval EFI_SUCCESS                        The processor information was retrieved successfully.
 * @retval EFI_INVALID_PARAMETER              Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 *
 **/
EFI_STATUS
EFIAPI
FabricTopologyGetProcessorInfo (
  IN       AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL             *This,
  IN       UINTN                                             Socket,
     OUT   UINTN                                             *NumberOfDie,
     OUT   UINTN                                             *NumberOfRootBridges,
     OUT   UINTN                                             *NumberOfNeighbors,
     OUT   AMD_FABRIC_TOPOLOGY_PROCESSOR_NEIGHBOR_INFO       **NeighborInfo
  )
{
  if ((NumberOfDie == NULL) && (NumberOfRootBridges == NULL) && (NumberOfNeighbors == NULL) && (NeighborInfo == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Socket >= FabricTopologyGetNumberOfProcessorsPresent ()) {
    return EFI_INVALID_PARAMETER;
  }

  if (NumberOfDie != NULL) {
    *NumberOfDie = FabricTopologyGetNumberOfDiesOnSocket (Socket);
  }

  if (NumberOfRootBridges != NULL) {
    *NumberOfRootBridges = FabricTopologyGetNumberOfRootBridgesOnSocket (Socket);
  }

  if (NumberOfNeighbors != NULL) {
    *NumberOfNeighbors = FabricTopologyGetNumberOfSocketNeighbors (Socket);
  }

  if (NeighborInfo != NULL) {
    *NeighborInfo = FabricTopologyGetNeighborInfoForSocket (Socket);
  }

  return EFI_SUCCESS;
}

/**
 * This service retrieves information about the given die.
 *
 * @param[in]  This                  A pointer to the
 *                                   AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL instance.
 * @param[in]  Socket                The target die's socket identifier.
 * @param[in]  Die                   The target die's identifier within Socket.
 * @param[out] NumberOfRootBridges   Pointer to the number of root PCI bridges
 *                                   present on the given die.
 * @param[out] SystemIdOffset        Pointer to the die's offset for all of its
 *                                   devices.
 * @param[out] FabricIdMap           Pointer to an array describing the devices
 *                                   present within the given die's fabric.
 * @param[out] NumberOfNeighbors     Pointer to the number of other die that the
 *                                   given die is physically connected to.
 * @param[out] NeighborInfo          Pointer to information about the given die's
 *                                   neighbors, if applicable.
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
  IN       AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL       *This,
  IN       UINTN                                       Socket,
  IN       UINTN                                       Die,
     OUT   UINTN                                       *NumberOfRootBridges,
     OUT   UINTN                                       *SystemIdOffset,
     OUT   AMD_FABRIC_TOPOLOGY_DIE_DEVICE_MAP          **FabricIdMap,
     OUT   UINTN                                       *NumberOfNeighbors,
     OUT   AMD_FABRIC_TOPOLOGY_DIE_NEIGHBOR_INFO       **NeighborInfo
  )
{
  if ((NumberOfRootBridges == NULL) && (SystemIdOffset == NULL) && (FabricIdMap == NULL) && (NumberOfNeighbors == NULL) && (NeighborInfo == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Socket >= FabricTopologyGetNumberOfProcessorsPresent ()) {
    return EFI_INVALID_PARAMETER;
  }

  if (Die >= FabricTopologyGetNumberOfDiesOnSocket (Socket)) {
    return EFI_INVALID_PARAMETER;
  }

  if (NumberOfRootBridges != NULL) {
    *NumberOfRootBridges = FabricTopologyGetNumberOfRootBridgesOnDie (Socket, Die);
  }

  if (SystemIdOffset != NULL) {
    *SystemIdOffset = FabricTopologyGetDieSystemOffset (Socket, Die);
  }

  if (FabricIdMap != NULL) {
    *FabricIdMap = (AMD_FABRIC_TOPOLOGY_DIE_DEVICE_MAP *) FabricTopologyGetDeviceMapOnDie (Socket, Die);
  }

  if (NumberOfNeighbors != NULL) {
    *NumberOfNeighbors = FabricTopologyGetNumberOfDieNeighbors (Socket, Die);
  }

  if (NeighborInfo != NULL) {
    *NeighborInfo = FabricTopologyGetNeighborInfoForDie (Socket, Die);
  }

  return EFI_SUCCESS;
}

/**
 * This service retrieves information about the given root PCI bridge.
 *
 * @param[in]  This                  A pointer to the
 *                                   AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL instance.
 * @param[in]  Socket                The target root bridge's socket identifier.
 * @param[in]  Die                   The target root bridge's die identifier within Socket.
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
  IN       AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Die,
  IN       UINTN                                 Index,
     OUT   UINTN                                 *SystemFabricID,
     OUT   UINTN                                 *BusNumberBase,
     OUT   UINTN                                 *BusNumberLimit
  )
{
  if ((SystemFabricID == NULL) && (BusNumberBase == NULL) && (BusNumberLimit == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Socket >= FabricTopologyGetNumberOfProcessorsPresent ()) {
    return EFI_INVALID_PARAMETER;
  }

  if (Die >= FabricTopologyGetNumberOfDiesOnSocket (Socket)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Index >= FabricTopologyGetNumberOfRootBridgesOnDie (Socket, Die)) {
    return EFI_INVALID_PARAMETER;
  }

  if (SystemFabricID != NULL) {
    *SystemFabricID = FabricTopologyGetHostBridgeSystemFabricID (Socket, Die, Index);
  }

  if (BusNumberBase != NULL) {
    *BusNumberBase = FabricTopologyGetHostBridgeBusBase (Socket, Die, Index);
  }

  if (BusNumberLimit != NULL) {
    *BusNumberLimit = FabricTopologyGetHostBridgeBusLimit (Socket, Die, Index);
  }

  return EFI_SUCCESS;
}
