/* $NoKeywords:$ */

/**
 * @file
 *
 * GNB function to create/locate PCIe configuration data area
 *
 * Contain code that create/locate and rebase configuration data area.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: GNB
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

#ifndef _PCIECONFIGLIB_LIB_H_
#define _PCIECONFIGLIB_LIB_H_

typedef VOID (*PCIe_RUN_ON_ENGINE_CALLBACK) (
  IN      PCIe_ENGINE_CONFIG                *Engine,
  IN OUT  VOID                              *Buffer,
  IN      PCIe_PLATFORM_CONFIG              *Pcie
  );

typedef HGPI_STATUS (*PCIe_RUN_ON_WRAPPER_CALLBACK) (
  IN      PCIe_WRAPPER_CONFIG               *Wrapper,
  IN OUT  VOID                              *Buffer,
  IN      PCIe_PLATFORM_CONFIG              *Pcie
  );

typedef VOID (*PCIe_RUN_ON_ENGINE_CALLBACK2) (
  IN      PCIe_ENGINE_CONFIG                *Engine,
  IN OUT  VOID                              *Buffer,
  IN      PCIe_WRAPPER_CONFIG               *Wrapper
  );

typedef VOID (*PCIe_RUN_ON_WRAPPER_CALLBACK2) (
  IN      PCIe_WRAPPER_CONFIG               *Wrapper,
  IN OUT  VOID                              *Buffer,
  IN      GNB_HANDLE                        *GnbHandle
  );

typedef HGPI_STATUS (*PCIe_RUN_ON_DESCRIPTOR_CALLBACK) (
  IN      PCIe_DESCRIPTOR_HEADER            *Descriptor,
  IN OUT  VOID                              *Buffer,
  IN      PCIe_PLATFORM_CONFIG              *Pcie
  );

UINT8
PcieConfigGetPcieEngineMasterLane (
  IN      PCIe_ENGINE_CONFIG                *Engine
  );

UINT8
PcieConfigGetNumberOfCoreLane (
  IN      PCIe_ENGINE_CONFIG                *Engine
  );

VOID
PcieConfigDisableAllEngines (
  IN      UINTN                             EngineTypeMask,
  IN      PCIe_WRAPPER_CONFIG               *Wrapper
  );

VOID
PcieConfigDisableEngine (
  IN      PCIe_ENGINE_CONFIG                *Engine
  );

UINT32
PcieConfigGetEnginePhyLaneBitMap (
  IN      PCIe_ENGINE_CONFIG                *Engine
  );

UINT8
PcieConfigGetNumberOfPhyLane (
  IN      PCIe_ENGINE_CONFIG                *Engine
  );

UINT64
PcieConfigGetConfigurationSignature (
  IN      PCIe_WRAPPER_CONFIG               *Wrapper,
  IN      UINT8                             CoreId
  );

BOOLEAN
PcieConfigCheckPortStatus (
  IN       PCIe_ENGINE_CONFIG               *Engine,
  IN       UINT32                           PortStatus
  );

UINT16
PcieConfigUpdatePortStatus (
  IN       PCIe_ENGINE_CONFIG               *Engine,
  IN       PCIe_ENGINE_INIT_STATUS          SetStatus,
  IN       PCIe_ENGINE_INIT_STATUS          ResetStatus
  );

VOID
PcieConfigRunProcForAllEngines (
  IN       UINT32                           DescriptorFlags,
  IN       PCIe_RUN_ON_ENGINE_CALLBACK      Callback,
  IN OUT   VOID                             *Buffer,
  IN       PCIe_PLATFORM_CONFIG             *Pcie
  );

VOID
PcieConfigRunProcForAllEnginesInWrapper (
  IN       UINT32                        DescriptorFlags,
  IN       PCIe_RUN_ON_ENGINE_CALLBACK2  Callback,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_WRAPPER_CONFIG           *Wrapper
  );

HGPI_STATUS
PcieConfigRunProcForAllWrappers (
  IN       UINT32                           DescriptorFlags,
  IN       PCIe_RUN_ON_WRAPPER_CALLBACK     Callback,
  IN OUT   VOID                             *Buffer,
  IN       PCIe_PLATFORM_CONFIG             *Pcie
  );

VOID
PcieConfigRunProcForAllWrappersInNbio (
  IN       UINT32                        DescriptorFlags,
  IN       PCIe_RUN_ON_WRAPPER_CALLBACK2 Callback,
  IN OUT   VOID                          *Buffer,
  IN       GNB_HANDLE                    *GnbHandle
  );

HGPI_STATUS
PcieConfigRunProcForAllDescriptors (
  IN       UINT32                           InDescriptorFlags,
  IN       UINT32                           OutDescriptorFlags,
  IN       UINT32                           TerminationFlags,
  IN       PCIe_RUN_ON_DESCRIPTOR_CALLBACK  Callback,
  IN OUT   VOID                             *Buffer,
  IN       PCIe_PLATFORM_CONFIG             *Pcie
  );

PCIe_DESCRIPTOR_HEADER *
PcieConfigGetParent (
  IN       UINT32                           Type,
  IN       PCIe_DESCRIPTOR_HEADER           *Descriptor
  );

PCIe_DESCRIPTOR_HEADER *
PcieConfigGetChild (
  IN       UINT32                           Type,
  IN       PCIe_DESCRIPTOR_HEADER           *Descriptor
  );

PCIe_DESCRIPTOR_HEADER *
PcieConfigGetPeer (
  IN       UINT32                           Type,
  IN       PCIe_DESCRIPTOR_HEADER           *Descriptor
  );

BOOLEAN
PcieConfigIsActivePcieEngine (
  IN      PCIe_ENGINE_CONFIG                *Engine
  );

PCIe_ENGINE_CONFIG *
PcieConfigLocateSbEngine (
  IN      PCIe_WRAPPER_CONFIG               *Wrapper
  );

VOID
PcieConfigDebugDump (
  IN      PCIe_PLATFORM_CONFIG              *Pcie
  );

VOID
PcieConfigWrapperDebugDump (
  IN      PCIe_WRAPPER_CONFIG               *WrapperList
  );

VOID
PcieConfigEngineDebugDump (
  IN      PCIe_ENGINE_CONFIG                *EngineList
  );

VOID
PcieUserConfigConfigDump (
  IN      PCIe_COMPLEX_DESCRIPTOR           *ComplexDescriptor
  );

VOID
PcieUserDescriptorConfigDump (
  IN      PCIe_ENGINE_DESCRIPTOR            *EngineDescriptor
  );

UINT32
GetIohcBridgeCntlAddress (
  IN      GNB_HANDLE            *GnbHandle,
  IN      PCIe_ENGINE_CONFIG    *Engine
  );

UINT32
ConvertPcieCoreAddress (
  IN      UINT32                Address,
  IN      GNB_HANDLE            *GnbHandle,
  IN      PCIe_WRAPPER_CONFIG   *PcieWrapper
  );

UINT32
ConvertPciePortAddress (
  IN      UINT32                Address,
  IN      GNB_HANDLE            *GnbHandle,
  IN      PCIe_WRAPPER_CONFIG   *PcieWrapper,
  IN      UINT8                 PortIndex
  );

UINT32
ConvertPciePortAddress2 (
  IN  UINT32                Address,
  IN  GNB_HANDLE            *GnbHandle,
  IN  PCIe_ENGINE_CONFIG    *Engine
  );

#define PcieConfigGetParentWrapper(Descriptor)                            ((PCIe_WRAPPER_CONFIG *)PcieConfigGetParent (DESCRIPTOR_ALL_WRAPPERS, &((Descriptor)->Header)))
#define PcieConfigGetParentSilicon(Descriptor)                            ((PCIe_SILICON_CONFIG *)PcieConfigGetParent (DESCRIPTOR_SILICON, &((Descriptor)->Header)))
#define PcieConfigGetParentComplex(Descriptor)                            ((PCIe_COMPLEX_CONFIG *)PcieConfigGetParent (DESCRIPTOR_COMPLEX, &((Descriptor)->Header)))
#define PcieConfigGetPlatform(Descriptor)                                 ((PCIe_PLATFORM_CONFIG *)PcieConfigGetParent (DESCRIPTOR_PLATFORM, &((Descriptor)->Header)))
#define PcieConfigGetChildWrapper(Descriptor)                             ((PCIe_WRAPPER_CONFIG *)PcieConfigGetChild (DESCRIPTOR_ALL_WRAPPERS, &((Descriptor)->Header)))
#define PcieConfigGetChildEngine(Descriptor)                              ((PCIe_ENGINE_CONFIG *)PcieConfigGetChild (DESCRIPTOR_ALL_ENGINES, &((Descriptor)->Header)))
#define PcieConfigGetChildSilicon(Descriptor)                             ((PCIe_SILICON_CONFIG *)PcieConfigGetChild (DESCRIPTOR_SILICON, &((Descriptor)->Header)))
#define PcieConfigGetNextDescriptor(Descriptor)                           ((((Descriptor->Header.DescriptorFlags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : (++Descriptor)))
#define PcieConfigIsPcieEngine(Descriptor)                                (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_PCIE_ENGINE) != 0) : FALSE)
#define PcieConfigIsDdiEngine(Descriptor)                                 (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_DDI_ENGINE) != 0) : FALSE)
#define PcieConfigIsPcieWrapper(Descriptor)                               (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_PCIE_WRAPPER) != 0) : FALSE)
#define PcieConfigIsSbPcieEngine(Engine)                                  (Engine != NULL ? ((BOOLEAN)(Engine->Type.Port.PortData.MiscControls.SbLink)) : FALSE)
#define PcieConfigIsDdiWrapper(Descriptor)                                (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_DDI_WRAPPER) != 0) : FALSE)
#define PcieConfigIsEngineAllocated(Descriptor)                           (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_ALLOCATED) != 0) : FALSE)
#define PcieConfigIsVirtualDesciptor(Descriptor)                          (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_VIRTUAL) != 0) : FALSE)
#define PcieConfigSetDescriptorFlags(Descriptor, SetDescriptorFlags)      if (Descriptor != NULL) (Descriptor)->Header.DescriptorFlags |= SetDescriptorFlags
#define PcieConfigResetDescriptorFlags(Descriptor, ResetDescriptorFlags)  if (Descriptor != NULL) ((PCIe_DESCRIPTOR_HEADER *)Descriptor)->DescriptorFlags &= (~(ResetDescriptorFlags))
#define PcieInputParsetGetNextDescriptor(Descriptor)                      (Descriptor != NULL ? ((((Descriptor->Flags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : ((Descriptor) + 1))) : NULL)
#ifndef PcieConfigGetNextTopologyDescriptor
  #define PcieConfigGetNextTopologyDescriptor(Descriptor, Termination)  (Descriptor != NULL ? (((((PCIe_DESCRIPTOR_HEADER *)Descriptor)->DescriptorFlags & Termination) != 0) ? NULL : ((UINT8 *)Descriptor + ((PCIe_DESCRIPTOR_HEADER *)Descriptor)->Peer)) : NULL)
#endif
#ifndef GnbGetNextHandle
  #define GnbGetNextHandle(Descriptor)  (GNB_HANDLE *)PcieConfigGetNextTopologyDescriptor (Descriptor, DESCRIPTOR_TERMINATE_TOPOLOGY)
#endif
#define PcieConfigGetNextDataDescriptor(Descriptor)  ((Descriptor->Flags & DESCRIPTOR_TERMINATE_LIST) != 0 ? NULL : ++Descriptor)

#define PcieConfigGetStdHeader(Descriptor)  ((HYGON_CONFIG_PARAMS *)((PCIe_PLATFORM_CONFIG *)PcieConfigGetParent (DESCRIPTOR_PLATFORM, &((Descriptor)->Header)))->StdHeader)

#endif
