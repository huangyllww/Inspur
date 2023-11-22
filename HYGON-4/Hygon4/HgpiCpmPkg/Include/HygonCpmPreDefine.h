/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM structures and definitions
 *
 * Contains HYGON CPM Platform Interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  Include
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

#ifndef _HYGON_CPM_PRE_DEFINE_H_
#define _HYGON_CPM_PRE_DEFINE_H_

#include <HygonCpmPlatform.h>
#include <HygonCpmDefine.h>

#if (PI_SPECIFICATION_VERSION < 0x00010000)
  #define gCpmFirmwareVolumeProtocolGuid  gEfiFirmwareVolume2ProtocolGuid
  #define CpmFirmwareVolume               FirmwareVolume2
  #define CPM_FIRMWARE_VOLUME_PROTOCOL    EFI_FIRMWARE_VOLUME2_PROTOCOL
#else
  #define gCpmFirmwareVolumeProtocolGuid  gEfiFirmwareVolume2ProtocolGuid
  #define CpmFirmwareVolume               FirmwareVolume2
  #define CPM_FIRMWARE_VOLUME_PROTOCOL    EFI_FIRMWARE_VOLUME2_PROTOCOL
#endif

#define CPM_STRINGIZE(a)                    # a
#define CPM_PROTOCOL_DEFINITION(a)          CPM_STRINGIZE (Protocol/a/a.h)
#define CPM_GUID_DEFINITION(a)              CPM_STRINGIZE (Guid/a/a.h)
#define CPM_PPI_DEFINITION(a)               CPM_STRINGIZE (Ppi/a/a.h)
#define CPM_LIBRARY_DEFINITION(a)           CPM_STRINGIZE (Library/a.h)
#define CPM_INDUSTRYSTANDARD_DEFINITION(a)  CPM_STRINGIZE (IndustryStandard/a.h)
#define CPM_CALLBACK_NAME
#define CPM_CALLBACK_VAR_TYPE
#define CPM_CALLBACK_VAR_NAME

#define CpmInitializeDriverLib(x, y)
#define CPM_PEI_FILE_HANDLE                  EFI_PEI_FILE_HANDLE
#define CPM_PEI_SERVICES                     CONST EFI_PEI_SERVICES
#define gCpmPeiMemoryDiscoveredPpiGuid       gEfiPeiMemoryDiscoveredPpiGuid
#define CPM_SMBUS_PPI                        EFI_PEI_SMBUS2_PPI
#define gCpmPeiSmbusPpiGuid                  gEfiPeiSmbus2PpiGuid
#define CpmSmbusPpi                          SmbusPpi
#define CPM_DEADLOOP                         CpuDeadLoop
#define CPM_EVENT_NOTIFY_SIGNAL              EVT_NOTIFY_SIGNAL
#define CPM_EVENT_SIGNAL_EXIT_BOOT_SERVICES  EVT_SIGNAL_EXIT_BOOT_SERVICES
#define CPM_TPL_NOTIFY                       TPL_NOTIFY
#define CPM_TPL_CALLBACK                     TPL_CALLBACK
#define CpmZeroMem                           ZeroMem
#define CPM_SIGNATURE_32                     SIGNATURE_32
#define gCpmSmbusProtocolGuid                gEfiSmbusHcProtocolGuid
#define CPM_ACPI_SUPPORT_PROTOCOL            EFI_ACPI_TABLE_PROTOCOL
#define gCpmAcpiSupportGuid                  gEfiAcpiTableProtocolGuid
#define CpmGetSystemConfigurationTable       EfiGetSystemConfigurationTable
#define CPM_PEI_READ_ONLY_VARIABLE_PPI       EFI_PEI_READ_ONLY_VARIABLE2_PPI
#define gCpmPeiReadOnlyVariablePpiGuid       gEfiPeiReadOnlyVariable2PpiGuid
#define CpmGetVariable(Ppi, Name, Guid, Attr, Size, Setup)  Ppi->GetVariable (Ppi, Name, &Guid, &Attr, &Size, &Setup)
#define CpmVariable           ReadOnlyVariable2
#define CpmVariableAttribute  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS

#define CPM_SMM_SX_DISPATCH_PROTOCOL   EFI_SMM_SX_DISPATCH2_PROTOCOL
#define CPM_SMM_SX_DISPATCH_CONTEXT    EFI_SMM_SX_REGISTER_CONTEXT
#define CPM_SMM_SW_DISPATCH_PROTOCOL   EFI_SMM_SW_DISPATCH2_PROTOCOL
#define CPM_SMM_SW_DISPATCH_CONTEXT    EFI_SMM_SW_REGISTER_CONTEXT
#define CpmSmmLocateProtocol           gSmst->SmmLocateProtocol
#define gCpmSmmSxDispatchProtocolGuid  gEfiSmmSxDispatch2ProtocolGuid
#define gCpmSmmSwDispatchProtocolGuid  gEfiSmmSwDispatch2ProtocolGuid
#define CpmCreateEventReadyToBoot      EfiCreateEventReadyToBootEx

#define CPM_DEFINE_gBS
#define CPM_DEFINE_gSmst
#define CPM_EXTERN_gBS

#define CFG_CPM_ASL_OEM_CALLBACK         PcdGet32 (OemCallBackAslName)
#define CFG_CPM_ASL_OEM_MLIB             PcdGet32 (OemMLibAslName)
#define CFG_HYGON_CPM_ASL_CPU0           PcdGet32 (Cpu0AslName)
#define CFG_HYGON_CPM_ASL_EC_NAME        PcdGet32 (EcAslName)
#define CFG_HYGON_CPM_ASL_EC_MUTEX       PcdGet32 (EcMutexAslName)
#define CFG_HYGON_CPM_ASL_LPC_NAME       PcdGet32 (LpcAslName)
#define CFG_HYGON_CPM_ASL_OKEC_NAME      PcdGet32 (OkEcAslName)
#define CFG_CPM_ACPI_RESTORE_SW_SMI      PcdGet8 (AcpiRestoreSwSmi)
#define CFG_CPM_ACPI_ENABLE_SW_SMI       PcdGet8 (AcpiEnableSwSmi)
#define CFG_CPM_PLATFORM_ID_OVERRIDE     PcdGet16 (PlatformIdOverride)
#define CFG_CPM_ECRAM_BASE_ADDR          PcdGet16 (EcRamBaseAddr)
#define CFG_CPM_ECRAM_GPIO_OFFSET        PcdGet8 (EcRamGpioOffset)
#define CFG_CPM_SWSMICMD_SETAUTOUMAMODE  PcdGet8 (SwSmiCmdSetAutoUmaMode)

#define CFG_CPM_X86_IO_REMAP_BASE  PcdGet64 (X86IoRemapBase)
#define CFG_CPM_PCI_MMIO_BASE      PcdGet64 (PciMmioBase)

#define CPM_SET_CURRENT_PLATFORM_ID(x)   PcdSet16S (CurrentPlatformId, x)
#define CPM_SET_ORIGINAL_PLATFORM_ID(x)  PcdSet16S (OriginalPlatformId, x)

#define If_CpmBuildGuidHob(Guid, Hob, Data, Size, Status)  Hob = BuildGuidHob (&Guid, Size); \
  if (Hob != NULL) { \
    if (Data) { \
      CpmCopyMem (Hob, Data, Size); \
    } \
  }  \
  if (Hob != NULL) \

#define CpmSetAcpiTable(AcpiSupport, CurrentTable, Flag, Mask, Size, TableList, Status) \
  Status = AcpiSupport->InstallAcpiTable ( \
                          AcpiSupport, \
                          CurrentTable, \
                          Size, \
                          &TableList \
                          ); \

#define If_CpmGetHobBuffer(HobList, Guid, Buffer, BufferSize, GuidHob) \
  { \
    EFI_HOB_GUID_TYPE  *GuidHob; \
    GuidHob = GetNextGuidHob (&Guid, HobList); \
    if (GuidHob != NULL ) { \
      Buffer     = GET_GUID_HOB_DATA (GuidHob); \
      BufferSize = GET_GUID_HOB_DATA_SIZE (GuidHob); \
    } \
  } \
  if (Buffer != NULL ) \

#define If_CpmInSmm(ImageHandle, SystemTable, gBS, gSmst, Status) \
  BOOLEAN                         InSmm; \
  EFI_SMM_BASE2_PROTOCOL  *SmmBase; \
  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID **)&SmmBase); \
  ASSERT (!EFI_ERROR (Status)); \
  if (EFI_ERROR (Status)) { \
    return Status; \
  } \
  SmmBase->InSmm (SmmBase, &InSmm); \
  if (InSmm) \

#define CPM_DEFINE_CALLBACK(function, type1, var1, type2, var2)  function (type1 var1, CONST VOID var2, VOID *CommBuffer, UINTN *CommBufferSize)

// Move the following defines to HgpiCpmPkg\Include\HygonCpmPlatform.h.
// Platform BIOS should redefine the following CPU SUPPORT defines in HgpiCpmPkg\Addendum\Oem\*Board*\HygonCpmPlatform.h.
// Then, copy HgpiCpmPkg\Addendum\Oem\*Board*\HygonCpmPlatform.h to HgpiCpmPkg\HygonCpmPlatform.h before building BIOS.
// #define HYGON_CPM_CPU_FAMILY_0x14_ON_SUPPORT
// #define HYGON_CPM_CPU_FAMILY_0x15_TN_SUPPORT
// #define HYGON_CPM_CPU_FAMILY_0x15_KV_SUPPORT
// #define HYGON_CPM_CPU_FAMILY_0x15_CZ_SUPPORT
// #define HYGON_CPM_CPU_FAMILY_0x16_KB_SUPPORT
// #define HYGON_CPM_CPU_FAMILY_0x16_ML_SUPPORT
// #define HYGON_CPM_CPU_FAMILY_0x16_NL_SUPPORT
// #define HYGON_CPM_CPU_FAMILY_0x21_AM_SUPPORT
// #define HYGON_CPM_CPU_FAMILY_0x15_ST_SUPPORT
// #define HYGON_CPM_CPU_FAMILY_0x15_BR_SUPPORT

#ifndef EFI_PPI_DEFINITION
  #define EFI_PPI_DEFINITION(a)  CPM_STRINGIZE (Ppi/a.h)
#endif
#ifndef EFI_PROTOCOL_DEFINITION
  #define EFI_PROTOCOL_DEFINITION(a)  CPM_STRINGIZE (Protocol/a.h)
#endif
#ifndef EFI_GUID_DEFINITION
  #define EFI_GUID_DEFINITION(a)  CPM_STRINGIZE (Guid/a.h)
#endif

#endif
