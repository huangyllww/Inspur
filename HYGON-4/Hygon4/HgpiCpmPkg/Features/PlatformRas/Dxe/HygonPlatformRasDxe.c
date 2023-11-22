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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "PiDxe.h"
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/SmnAccessLib.h>
#include <Library/LocalApicLib.h>
#include <Protocol/MpService.h>
#include <Protocol/HygonRasApeiProtocol.h>
#include <Protocol/HygonCoreTopologyProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/HygonNbioPcieAerProtocol.h>
#include <Protocol/AcpiSystemDescriptionTable.h>          // byo231031 +
#include "HygonApeiErst.h"
#include "HygonApeiEinj.h"
#include "HygonApeiHest.h"
#include "HygonApeiBert.h"
#include "HygonPlatformRasDxe.h"
#include <TSFch.h>
#include <Fch.h>
#include <GnbRegisters.h>
#include <Library/RasMcaLib.h>
#include <Library/HygonSmnAddressLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/RasElogEventLib.h>              // byo231101 +
#include <Protocol/ReportRasUceProtocol.h>        // byo231101 +


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
PLATFORM_APEI_PRIVATE_BUFFER  *mPlatformApeiPrivate;
EFI_ACPI_TABLE_PROTOCOL       *AcpiTableProtocol;
HYGON_RAS_APEI_PROTOCOL       *HygonRasApeiProtocol;
BOOLEAN                       gAerEn;
BOOLEAN                       gDpcEn;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
PlatformConfigInit (
  VOID
  );

EFI_STATUS
McaErrorDetection (
  VOID
  );

EFI_STATUS
NbioHestUpdate (
  VOID
  );

EFI_STATUS
NbioErrorDetection (
  VOID
  );

EFI_STATUS
SmnErrorDetection (
  VOID
  );

EFI_STATUS
PciePortDetect (
  VOID
  );

EFI_STATUS
PcieAerHestUpdate (
  VOID
  );

EFI_STATUS
PcieCapErrorConfig ( 
  VOID 
  );

EFI_STATUS
PcieAerErrorConfig (
  VOID
  );

EFI_STATUS
NbifErrorConfig (
  VOID
  );

EFI_STATUS
FstActionCtrl (
  VOID
  );

EFI_STATUS
NbioErrorScan (
  RAS_NBIO_ERROR_INFO   *RasNbioErrorInfo
  );

EFI_STATUS
HyExNbifErrorScan (
  UINT32    NbioGlobalStatusHi,
  UINT8     RbBusNum,
  UINT8     RbNum
  );
  
EFI_STATUS
HyGxNbifErrorScan (
  UINT32    NbioGlobalStatusHi,
  UINT8     RbBusNum,
  UINT8     LogicalDieId,
  UINT8     RbNum
  );

EFI_STATUS
PcieErrorScan (
  UINT32    NbioGlobalStatusHi,
  UINT8     RbBusNum
  );

EFI_STATUS
FstErrorScan (
  UINT32 NbioGlobalStatusHi,
  UINT8  SocketId,
  UINT8  IohubPhysicalDieId,
  UINT8  RbBusNum
  );

EFI_STATUS
McaStatusClear (
  EFI_MP_SERVICES_PROTOCOL  *MpServices,
  RAS_MCA_ERROR_INFO        *RasMcaErrorInfo,
  UINT8                     BankIndex
  );

SCAN_STATUS
STATIC
RasDevStsClr (
  IN       PCI_ADDR             Device,
  IN OUT   RAS_PCI_SCAN_DATA    *ScanData
  );

VOID
FillPcieInfo (
  PCI_ADDR           PciPortAddr,
  PCIE_ERROR_SECTION *PcieErrorSection  
  );
  
VOID
STATIC
PcieDevErrScanOnDevice (
  PCI_ADDR       Device,
  VOID           *Buffer                 // byo231109 -
  );

VOID
STATIC
PcieDevErrScanOnFunction (
  PCI_ADDR       Function,
  VOID           *Buffer                 // byo231109 -
  );

SCAN_STATUS
STATIC
PcieDevErrScanCallback (
  PCI_ADDR             Device,
  RAS_PCI_SCAN_DATA    *ScanData
  );
  
VOID
RasLateBootInit (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  );

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern  EFI_BOOT_SERVICES  *gBS;

/*----------------------------------------------------------------------------------------
 *                          T A B L E    D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

EFI_GUID gPcieErrorSectGuid = PCIE_SECT_GUID;         // byo231109 +


// Generic Processor Error entries
GENERIC_PROC_ERR_ENTRY  gGenProcErrEntry = {
  {
    { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0 }, // UEFI 2.6 Section N 2.5 Processor Specific Error Section Type 16-byte GUID, update in runtime.
    ERROR_SEVERITY_FATAL,                                                         // UINT32 ACPI 6.1 Table 18-343 Generic Error Data Entry
    GENERIC_ERROR_REVISION,                                                       // UINT16 UEFI 2.6 Section N Revision Field
    FRU_STRING_VALID,                                                             // UINT8 UEFI 2.6 Section N Section Descriptor Validation Bits field
    0x1,                                                                          // UINT8 UEFI 2.6 Section N Section Descriptor Flags
    0,                                                                            // UINT32 Generic Error Data Block Length, update in runtime
    { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0 }, // UINT8[16] FRU ID - UEFI 2.6 Section N
    { 'P', 'r', 'o', 'c', 'e', 's', 's', 'o', 'r', 'E', 'r', 'r', 'o', 'r' }      // UINT8[20] FRU Text[20] - UEFI 2.6 Section N
  },
  {
    { 0 }, // UEFI 2.6 Section N Table 251 -Processor Specific Error Type Valid bit
    0,     // Processor APIC ID
    0      // CPUID Information
  }
};

// Generic Memory Error entries
GENERIC_MEM_ERR_ENTRY  gGenMemErrEntry = {
  {
    { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0 }, // UEFI 2.6 Section N 2.5 Memory Error Section Type 16-byte GUID, update in runtime.
    ERROR_SEVERITY_FATAL,                                                         // UINT32 ACPI 6.1 Table 18-343 Generic Error Data Entry
    GENERIC_ERROR_REVISION,                                                       // UINT16 UEFI 2.6 Section N Revision Field
    FRU_STRING_VALID,                                                             // UINT8 UEFI 2.6 Section N Section Descriptor Validation Bits field
    0x1,                                                                          // UINT8 UEFI 2.6 Section N Section Descriptor Flags
    sizeof (PLATFORM_MEM_ERR_SEC),                                                // UINT32 Generic Error Data Block Length
    { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0 }, // UINT8[16] FRU ID - UEFI 2.6 Section N
    { 'U', 'n', 'c', 'o', 'r', 'r', 'e', 'c', 't', 'e', 'd', 'E', 'r', 'r' }      // UINT8[20] FRU Text[20] - UEFI 2.6 Section N
  },
  {
    { 0 }, // 0x4009 UEFI 2.6 Section N Table 245 Bit[14]-Memory Error Type Valid, Bit[3]-Node Valid, Bit[0]-ErrSts Valid (0xC34D)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Card,Node, Module, Bank, etc.
    UNKNOWN                                   // Memory Error Type
  }
};

// Generic Memory CRC/Parity Error entries
GENERIC_MEM_ERR_ENTRY  gGenMemParityErrEntry = {
  {
    { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0 }, // UEFI 2.6 Section N 2.5 Memory Error Section Type 16-byte GUID, update in runtime.
    ERROR_SEVERITY_FATAL,                                                       // UINT32 ACPI 6.1 Table 18-343 Generic Error Data Entry
    GENERIC_ERROR_REVISION,                                                     // UINT16 UEFI 2.6 Section N Revision Field
    FRU_STRING_VALID,                                                           // UINT8 UEFI 2.6 Section N Section Descriptor Validation Bits field
    0x1,                                                                        // UINT8 UEFI 2.6 Section N Section Descriptor Flags
    sizeof (PLATFORM_MEM_ERR_SEC),                                              // UINT32 Generic Error Data Block Length
    { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0 }, // UINT8[16] FRU ID - UEFI 2.6 Section N
    { 'C', 'R', 'C', '/', 'P', 'a', 'r', 'i', 't', 'y', 'E', 'r', 'r' }         // UINT8[20] FRU Text[20] - UEFI 2.6 Section N
  },
  {
    { 0 }, // 0x4009 UEFI 2.6 Section N Table 245 Bit[14]-Memory Error Type Valid, Bit[3]-Node Valid, Bit[0]-ErrSts Valid (0xC34D)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Card,Node, Module, Bank, etc.
    PARITY_ERROR                              // Memory Error Type
  }
};

EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE  HestErrRegTbl = {
  { 0, 0, 0, 0, 0 },                                               // UINT32 BlockStatus - WHEA Platform Design Guide Table 3-2 Block Status
  sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE),               // UINT32 RawDataOffset - WHEA Platform Design Guide Table 3-2 Raw Data Offset
  0,                                                               // UINT32 RawDataLength - WHEA Platform Design Guide Table 3-2 Raw Data Offset
  0,                                                               // UINT32 DataLength - WHEA Platform Design Guide Table 3-2 Raw Data Offset
  EFI_ACPI_6_0_ERROR_SEVERITY_NONE                                 // UINT32 ErrorSeverity - WHEA Platform Design Guide Table 3-2 Raw Data Offset (Initialize Severity to [None] = 0x03)
};

GENERIC_NBIO_ERR_ENTRY  gGenNbioErrEntry = {
  {
    { 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0 }, // HYGON NBIO Error Section Type 16-byte GUID, fill the GUID in runtime.
    ERROR_SEVERITY_FATAL,                                               // UINT32 ACPI 6.1 Table 18-343 Generic Error Data Entry
    GENERIC_ERROR_REVISION,                                             // UINT16 UEFI 2.6 Section N Revision Field
    FRU_STRING_VALID,                                                   // UINT8 UEFI 2.6 Section N Section Descriptor Validation Bits field
    0x1,                                                                // UINT8 UEFI 2.6 Section N Section Descriptor Flags
    sizeof (HYGON_NBIO_ERROR_RECORD),                                   // UINT32 Generic Error Data Block Length
    { 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0 }, // UINT8[16] FRU ID - UEFI 2.6 Section N
    { 'N', 'b', 'i', 'o', 'E', 'r', 'r', 'o', 'r' }                     // UINT8 [20] FRU Text[20] - UEFI 2.6 Section N
  },
  {
    { 0 },
    0,
    { 0 },
    { 0 },
    0,
    0
  }
};

GENERIC_SMN_ERR_ENTRY  gGenSmnErrEntry = {
  {
    { 0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0 }, // HYGON SMN Error Section Type 16-byte GUID
    ERROR_SEVERITY_FATAL,                                             // UINT32 ACPI 6.1 Table 18-343 Generic Error Data Entry
    GENERIC_ERROR_REVISION,                                           // UINT16 UEFI 2.6 Section N Revision Field
    FRU_STRING_VALID,                                                 // UINT8 UEFI 2.6 Section N Section Descriptor Validation Bits field
    0x1,                                                              // UINT8 UEFI 2.6 Section N Section Descriptor Flags
    sizeof (HYGON_SMN_ERROR_RECORD),                                  // UINT32 Generic Error Data Block Length
    { 0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0 }, // UINT8[16] FRU ID - UEFI 2.6 Section N
    { 'S', 'm', 'n', 'E', 'r', 'r', 'o', 'r' }                        // UINT8 [20] FRU Text[20] - UEFI 2.6 Section N
  },
  {
    { 0 },
    0,
    { 0 },
    { 0 },
  }
};

EFI_ACPI_6_0_GENERIC_HARDWARE_ERROR_SOURCE_STRUCTURE  gNbioErrSrc = {
  0x0009,                                                         // Type
  NBIO_SOURCE_ID,                                                 // SourceId
  0xffff,                                                         // RelatedSourceId
  0x00,                                                           // Flags
  0x01,                                                           // Enabled
  0x00000001,                                                     // NumberofRecordsToPreAllocate
  0x00000001,                                                     // MaxSectionsPerRecord
  MAX_ERROR_BLOCK_SIZE,                                           // MaxRawDataLength
  { 0x00, 0x40, 0x00, 0x04, 0x00 },                               // ErrorStatusAddress. Address will be filled in runtime
  {
    HARDWARE_ERROR_NOTIFICATION_POLLED,                           // UINT8  NotifiyType = Polled
    sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE),  // UINT8  ErrNotifyLen
    { 0,    0,    0,    0,    0, 0, 0},                           // UINT16 ConfigWrite
    5000,                                                         // UINT32 PollInterval = 5000 ms
    0,                                                            // UINT32 Interrupt Vector
    0,                                                            // UINT32 SwitchPollingThreshVal
    0,                                                            // UINT32 SwitchPollingThreshWindow
    0,                                                            // UINT32 ErrThresholdVal;
    0                                                             // UINT32 ErrThresholdWindow
  },
  MAX_ERROR_BLOCK_SIZE                                            // ErrorStatusSize
};

GENERIC_PCIE_AER_ERR_ENTRY  gGenPcieErrEntry = {
  {
    { 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0 }, // HYGON NBIO Error Section Type 16-byte GUID, update in runtime.
    ERROR_SEVERITY_FATAL,                                               // UINT32 ACPI 6.1 Table 18-343 Generic Error Data Entry
    GENERIC_ERROR_REVISION,                                             // UINT16 UEFI 2.6 Section N Revision Field
    FRU_STRING_VALID,                                                   // UINT8 UEFI 2.6 Section N Section Descriptor Validation Bits field
    0x1,                                                                // UINT8 UEFI 2.6 Section N Section Descriptor Flags
    sizeof (PCIE_ERROR_SECTION),                                        // UINT32 Generic Error Data Block Length
    { 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0 }, // UINT8[16] FRU ID - UEFI 2.6 Section N
    { 'P', 'c', 'i', 'e', 'E', 'r', 'r', 'o', 'r' }                     // UINT8 [20] FRU Text[20] - UEFI 2.6 Section N
  }
};

EFI_ACPI_6_0_GENERIC_HARDWARE_ERROR_SOURCE_STRUCTURE  gPcieGenErrSrc = {
  0x0009,                                                         // Type
  PCIE_SOURCE_ID,                                                 // SourceId
  0xffff,                                                         // RelatedSourceId
  0x00,                                                           // Flags
  0x01,                                                           // Enabled
  0x00000001,                                                     // NumberofRecordsToPreAllocate
  0x00000001,                                                     // MaxSectionsPerRecord
  MAX_ERROR_BLOCK_SIZE,                                           // MaxRawDataLength
  { 0x00, 0x40, 0x00, 0x04, 0x00 },                               // ErrorStatusAddress. Address will be filled in runtime
  {
    HARDWARE_ERROR_NOTIFICATION_POLLED,                           // UINT8  NotifiyType = Polled
    sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE),  // UINT8  ErrNotifyLen
    { 0,    0,    0,    0,    0, 0, 0},                           // UINT16 ConfigWrite
    5000,                                                         // UINT32 PollInterval = 5000 ms
    0,                                                            // UINT32 Interrupt Vector
    0,                                                            // UINT32 SwitchPollingThreshVal
    0,                                                            // UINT32 SwitchPollingThreshWindow
    0,                                                            // UINT32 ErrThresholdVal;
    0                                                             // UINT32 ErrThresholdWindow
  },
  MAX_ERROR_BLOCK_SIZE                                            // ErrorStatusSize
};

// A PCI-E port device/function loopup table.
PCIE_PORT_LIST  gPciePortList[] = {
  { 1, 1 },         // PCI-E 0 Port A
  { 1, 2 },         // PCI-E 0 Port B
  { 1, 3 },         // PCI-E 0 Port C
  { 1, 4 },         // PCI-E 0 Port D
  { 1, 5 },         // PCI-E 0 Port E
  { 1, 6 },         // PCI-E 0 Port F
  { 1, 7 },         // PCI-E 0 Port G
  { 2, 1 },         // PCI-E 0 Port H
  { 3, 1 },         // PCI-E 1 Port A
  { 3, 2 },         // PCI-E 1 Port B
  { 3, 3 },         // PCI-E 1 Port C
  { 3, 4 },         // PCI-E 1 Port D
  { 3, 5 },         // PCI-E 1 Port E
  { 3, 6 },         // PCI-E 1 Port F
  { 3, 7 },         // PCI-E 1 Port G
  { 4, 1 },         // PCI-E 1 Port H
  { 5, 1 },         // PCI-E 2 Port A
  { 5, 2 },         // PCI-E 2 Port B
  { 5, 3 },         // PCI-E 2 Port C
  { 5, 4 },         // PCI-E 2 Port D
  { 5, 5 },         // PCI-E 2 Port E
  { 5, 6 },         // PCI-E 2 Port F
  { 5, 7 },         // PCI-E 2 Port G
  { 6, 1 },         // PCI-E 2 Port H
};

ERR_ACT_CTRL_REG  gPciePortActListAerDpc[] = {
  { 1, 0, 1, 1 },                        // SerrActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 1, 0, 1, 1 },                        // IntFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 0, 3, 0, 0 },                        // IntNonFatalActCtrl (IntrGenSel = SMI)
  { 0, 0, 0, 0 },                        // IntCorrActCtrl (IntrGenSel = SMI)
  { 0, 0, 0, 0 },                        // ExtFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 0, 0, 0, 0 },                        // ExtNonFatalActCtrl (IntrGenSel = SMI)
  { 0, 3, 0, 0 },                        // ExtCorrActCtrl (IntrGenSel = SMI)
  { 0, 3, 0, 0 }                         // ParityErrActCtrl (IntrGenSel = SMI)
};

ERR_ACT_CTRL_REG  gPciePortActListAerPfehDis[] = {
  { 1, 0, 1, 1 },                        // SerrActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 0, 0, 0, 0 },                        // IntFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 0, 0, 0, 0 },                        // IntNonFatalActCtrl
  { 0, 0, 0, 0 },                        // IntCorrActCtrl
  { 0, 0, 0, 0 },                        // ExtFatalActCtrl
  { 0, 0, 0, 0 },                        // ExtNonFatalActCtrl
  { 0, 0, 0, 0 },                        // ExtCorrActCtrl
  { 0, 3, 0, 0 }                         // ParityErrActCtrl (IntrGenSel = SMI)
};

ERR_ACT_CTRL_REG  gPciePortActListAerNoDpc[] = {
  { 1, 0, 1, 1 },                        // SerrActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 0, 0, 0, 0 },                        // IntFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 0, 0, 0, 0 },                        // IntNonFatalActCtrl (IntrGenSel = SMI)
  { 0, 0, 0, 0 },                        // IntCorrActCtrl (IntrGenSel = SMI)
  { 1, 0, 1, 1 },                        // ExtFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 0, 3, 0, 0 },                        // ExtNonFatalActCtrl (IntrGenSel = SMI)
  { 0, 3, 0, 0 },                        // ExtCorrActCtrl (IntrGenSel = SMI)
  { 0, 3, 0, 0 }                         // ParityErrActCtrl (IntrGenSel = SMI)
};

ERR_ACT_CTRL_REG  gPciePortActListNoAer[] = {
  { 1, 0, 1, 1 },                        // SerrActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 1, 0, 1, 1 },                        // IntFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 0, 3, 0, 0 },                        // IntNonFatalActCtrl (IntrGenSel = SMI)
  { 0, 3, 0, 0 },                        // IntCorrActCtrl (IntrGenSel = SMI)
  { 0, 0, 0, 0 },                        // ExtFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 0, 0, 0, 0 },                        // ExtNonFatalActCtrl (IntrGenSel = SMI)
  { 0, 0, 0, 0 },                        // ExtCorrActCtrl (IntrGenSel = SMI)
  { 0, 3, 0, 0 }                         // ParityErrActCtrl (IntrGenSel = SMI)
};

ERR_ACT_CTRL_REG  gNbifActConfigList[] = {
  { 1, 0, 1, 1 },                        // SerrActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 1, 0, 1, 1 },                        // IntFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 0, 3, 0, 0 },                        // IntNonFatalActCtrl (IntrGenSel = SMI)
  { 0, 3, 0, 0 },                        // IntCorrActCtrl (IntrGenSel = SMI)
  { 1, 0, 1, 1 },                        // ExtFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 0, 3, 0, 0 },                        // ExtNonFatalActCtrl (IntrGenSel = SMI)
  { 0, 3, 0, 0 },                        // ExtCorrActCtrl (IntrGenSel = SMI)
  { 0, 3, 0, 0 }                         // ParityErrActCtrl (IntrGenSel = SMI)
};

ERR_ACT_CTRL_REG  gFstActConfigList[] = {
  { 1, 0, 1, 1 },                        // FstusFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 0, 3, 0, 0 },                        // FstusCorrActCtrl (IntrGenSel = SMI)
  { 1, 0, 1, 1 },                        // FstdsFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  { 0, 3, 0, 0 }                         // FstdsCorrActCtrl (IntrGenSel = SMI)
};






// byo231101 + >>
VOID SaveMemUceAddr(UINT64 Address)
{
  EFI_STATUS                 Status;
  HYGON_REPORT_UCE_PROTOCOL  *ReportUce;

  Status = gBS->LocateProtocol (
                       &gHygonReportRasUceProtocolGuid,
                       NULL,
                       (VOID**)&ReportUce
                       );
  if (EFI_ERROR (Status)) {
    return;
  }

  ReportUce->ReportMemUceAddr(ReportUce, Address);
}

VOID SaveCoreUceApicId(UINTN ApicId)
{
  EFI_STATUS                 Status;
  HYGON_REPORT_UCE_PROTOCOL  *ReportUce;

  Status = gBS->LocateProtocol (
                       &gHygonReportRasUceProtocolGuid,
                       NULL,
                       (VOID**)&ReportUce
                       );
  if (EFI_ERROR (Status)) {
    return;
  }

  ReportUce->ReportCoreUce(ReportUce, ApicId);
}

VOID ReportUceEnd()
{
  EFI_STATUS                 Status;
  HYGON_REPORT_UCE_PROTOCOL  *ReportUce;

  Status = gBS->LocateProtocol (
                       &gHygonReportRasUceProtocolGuid,
                       NULL,
                       (VOID**)&ReportUce
                       );
  if (EFI_ERROR (Status)) {
    return;
  }

  ReportUce->End(ReportUce);
}
// byo231101 + <<




/*---------------------------------------------------------------------------------------*/

/*
 * Calculate an ACPI style checksum
 *
 * Computes the checksum and stores the value to the checksum
 * field of the passed in ACPI table's header.
 *
 * @param[in]  Table             ACPI table to checksum
 *
 */
VOID
ChecksumAcpiTable (
  IN OUT   EFI_ACPI_DESCRIPTION_HEADER *Table
  )
{
  UINT8   *BuffTempPtr;
  UINT8   Checksum;
  UINT32  BufferOffset;

  Table->Checksum = 0;
  Checksum    = 0;
  BuffTempPtr = (UINT8 *)Table;
  for (BufferOffset = 0; BufferOffset < Table->Length; BufferOffset++) {
    Checksum = Checksum - *(BuffTempPtr + BufferOffset);
  }

  Table->Checksum = Checksum;
}

// byo231031 + >>
VOID UpdatePciePortActList(ERR_ACT_CTRL_REG *Regs, ERR_ACT_CTRL_REG *Override, UINTN Count)
{
  UINTN  Index;

  for (Index = 0; Index < Count; Index++) {
    if(Override[Index].Value != 0xFFFFFFFF && Regs[Index].Value != 0){
      Regs[Index].Value = Override[Index].Value;
    }
  }
}

VOID DumpErrActCtrlRegList(ERR_ACT_CTRL_REG *p, UINTN Size)
{
  UINTN  Count = Size/sizeof(ERR_ACT_CTRL_REG);
  UINTN  Index;

  for(Index=0;Index<Count;Index++){
    DEBUG ((EFI_D_INFO, "{%d, %d, %d, %d}\n", \
      p[Index].Fields.ApmlErrEn, p[Index].Fields.IntrGenSel, p[Index].Fields.LinkDisEn, p[Index].Fields.SyncFloodEn));
  }

  DEBUG((EFI_D_INFO, "\n"));
}
// byo231031 + <<

/*********************************************************************************
 * Name: HygonPlatformRasDxeInit
 *
 * Description
 *   Entry point of the HYGON SOC Satori SP4 DXE driver
 *   Perform the configuration init, resource reservation, early post init
 *   and install all the supported protocol
 *
 * Input
 *   ImageHandle : EFI Image Handle for the DXE driver
 *   SystemTable : pointer to the EFI system table
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
HygonPlatformRasDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_HANDLE  Handle = NULL;
  EFI_EVENT   ExitBootServicesEvent;
  BYO_RAS_POLICY_DATA  *ByoRasPolicy;                  // byo231031 +

// byo231031 + >>
  DEBUG((EFI_D_INFO, "HygonPlatformRasDxeInit\n"));

  Status = gBS->LocateProtocol (
                       &gByoRasPolicyDataProtocolGuid,
                       NULL,
                       (VOID**)&ByoRasPolicy
                       );
  if(!EFI_ERROR(Status) && 
     ByoRasPolicy->Tag == BYO_RAS_POLICY_DATA_TAG && 
     ByoRasPolicy->Version == BYO_RAS_POLICY_DATA_VER &&
     ByoRasPolicy->Eacr.Valid){
    CopyMem(gNbifActConfigList, ByoRasPolicy->Eacr.NbifActConfigList, sizeof(gNbifActConfigList));
    UpdatePciePortActList(gPciePortActListAerDpc, ByoRasPolicy->Eacr.PciePortActList, ARRAY_SIZE(gPciePortActListAerDpc));
    UpdatePciePortActList(gPciePortActListAerPfehDis, ByoRasPolicy->Eacr.PciePortActList, ARRAY_SIZE(gPciePortActListAerPfehDis));
    UpdatePciePortActList(gPciePortActListAerNoDpc, ByoRasPolicy->Eacr.PciePortActList, ARRAY_SIZE(gPciePortActListAerNoDpc));
    UpdatePciePortActList(gPciePortActListNoAer, ByoRasPolicy->Eacr.PciePortActList, ARRAY_SIZE(gPciePortActListNoAer));
  } 
  DumpErrActCtrlRegList(gPciePortActListAerDpc, sizeof(gPciePortActListAerDpc));
  DumpErrActCtrlRegList(gPciePortActListAerPfehDis, sizeof(gPciePortActListAerPfehDis));
  DumpErrActCtrlRegList(gPciePortActListAerNoDpc, sizeof(gPciePortActListAerNoDpc));
  DumpErrActCtrlRegList(gPciePortActListNoAer, sizeof(gPciePortActListNoAer));
  DumpErrActCtrlRegList(gNbifActConfigList, sizeof(gNbifActConfigList));
// byo231031 + <<

  // Locate ACPI Support table.
  Status = gBS->LocateProtocol (
                  &gEfiAcpiTableProtocolGuid,
                  NULL,
                  &AcpiTableProtocol
                  );
  ASSERT (!EFI_ERROR (Status));                    // byo231031 +
  if (EFI_ERROR (Status)) {
//- ASSERT (!EFI_ERROR (Status));                  // byo231031 -
    return Status;
  }

  // Locate Ras APEI protocol
  Status = gBS->LocateProtocol (
                  &gHygonRasApeiProtocolGuid,
                  NULL,
                  &HygonRasApeiProtocol
                  );
  ASSERT (!EFI_ERROR (Status));                     // byo231031 +
  if (EFI_ERROR (Status)) {
//- ASSERT (!EFI_ERROR (Status));                   // byo231031 -
    return Status;
  }

  PlatformConfigInit ();
// byo231031 + >>  
  if(ByoRasPolicy->Tag == BYO_RAS_POLICY_DATA_TAG && ByoRasPolicy->Version == BYO_RAS_POLICY_DATA_VER){
    mPlatformApeiPrivate->PlatRasPolicy.ApeiDisable          = ByoRasPolicy->ApeiDisable;
    mPlatformApeiPrivate->PlatRasPolicy.ApeiEinjDisable      = ByoRasPolicy->ApeiEinjDisable;
    mPlatformApeiPrivate->PlatRasPolicy.ApeiEinjCpuCeDisable = ByoRasPolicy->ApeiEinjCpuCeDis;
    mPlatformApeiPrivate->PlatRasPolicy.PcieAerCeMask        = ByoRasPolicy->PcieAerCeMask;
    mPlatformApeiPrivate->PlatRasPolicy.PcieAerUceMask       = ByoRasPolicy->PcieAerUceMask;
    mPlatformApeiPrivate->PlatRasPolicy.PcieCeThreshold      = ByoRasPolicy->PcieCeThreshold;                    // byo231109 +
  }
// byo231109 + >>  
  DEBUG((EFI_D_INFO, "ApeiDisable         :%d\n", mPlatformApeiPrivate->PlatRasPolicy.ApeiDisable));
  DEBUG((EFI_D_INFO, "ApeiEinjDisable     :%d\n", mPlatformApeiPrivate->PlatRasPolicy.ApeiEinjDisable));
  DEBUG((EFI_D_INFO, "ApeiEinjCpuCeDisable:%d\n", mPlatformApeiPrivate->PlatRasPolicy.ApeiEinjCpuCeDisable));
  DEBUG((EFI_D_INFO, "PcieAerCeMask       :%x\n", mPlatformApeiPrivate->PlatRasPolicy.PcieAerCeMask));
  DEBUG((EFI_D_INFO, "PcieAerUceMask      :%x\n", mPlatformApeiPrivate->PlatRasPolicy.PcieAerUceMask));
  DEBUG((EFI_D_INFO, "PcieCeThreshold     :%d\n", mPlatformApeiPrivate->PlatRasPolicy.PcieCeThreshold));
// byo231109 + <<  
// byo231031 + <<

  // Initialize BERT
  ApeiBertInstall ();
  // Initialize HEST
  ApeiHestInstall ();
  // Initialize EINJ
  ApeiEinjInstall ();
  // Initialize ERST
  // ApeiErstInstall ();

  NbioHestUpdate ();

  PciePortDetect ();

  PcieAerHestUpdate ();

  mPlatformApeiPrivate->Valid = TRUE;

  Handle = ImageHandle;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonPlatformApeiDataProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  mPlatformApeiPrivate
                  );
  if (EFI_ERROR (Status)) {
    return (Status);
  }

  SmnErrorDetection ();
  McaErrorDetection ();
  NbioErrorDetection ();

  ReportUceEnd();                               // byo231101 +

  //
  // Register the event handling function
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  RasLateBootInit,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &ExitBootServicesEvent
                  );

  return (Status);
}



// byo231031 + >>
VOID RemoveApeiTableIfNeeded()
{
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  EFI_ACPI_SDT_PROTOCOL         *AcpiSdt;
  EFI_STATUS                    Status;
  INTN                          Index;
  EFI_ACPI_TABLE_VERSION        Version;
  UINTN                         TableKey;
  EFI_ACPI_SDT_HEADER           *AcpiSdtHdr;
  CHAR8                         *c;
  BOOLEAN                       ApeiEinjDisable = mPlatformApeiPrivate->PlatRasPolicy.ApeiEinjDisable;
  BOOLEAN                       ApeiDisable     = mPlatformApeiPrivate->PlatRasPolicy.ApeiDisable;
  BOOLEAN                       NeedRemove;
  UINT32                        Signature;


  DEBUG((EFI_D_INFO, "RemoveApeiTableIfNeeded\n"));

  if(ApeiEinjDisable == 0 && ApeiDisable == 0){
    return;
  }

  Status = gBS->LocateProtocol (
                  &gEfiAcpiTableProtocolGuid,
                  NULL,
                  (VOID**)&AcpiTable
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = gBS->LocateProtocol (
                  &gEfiAcpiSdtProtocolGuid,
                  NULL,
                  (VOID**)&AcpiSdt
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  Index = 0;
  while(1) {

    Status = AcpiSdt->GetAcpiTable (
                        Index,
                        &AcpiSdtHdr,
                        &Version,
                        &TableKey
                        );
    if (EFI_ERROR (Status)) {
      break;
    }

    NeedRemove = FALSE;
    if(ApeiDisable){
      if (AcpiSdtHdr->Signature == BERT_SIG ||
          AcpiSdtHdr->Signature == EFI_ACPI_6_0_HARDWARE_ERROR_SOURCE_TABLE_SIGNATURE ||
          AcpiSdtHdr->Signature == EFI_ACPI_6_0_ERROR_INJECTION_TABLE_SIGNATURE) {
        NeedRemove = TRUE;
      }
    } else if(ApeiEinjDisable){
      if(AcpiSdtHdr->Signature == EFI_ACPI_6_0_ERROR_INJECTION_TABLE_SIGNATURE){
        NeedRemove = TRUE;
      }
    }

    if(NeedRemove){
      Signature = AcpiSdtHdr->Signature;
      Status = AcpiTable->UninstallAcpiTable (
                            AcpiTable,
                            TableKey
                            );
      c = (CHAR8*)&Signature;
      DEBUG((EFI_D_INFO, "UninstallAcpiTable(%c%c%c%c):%r\n", c[0], c[1], c[2], c[3], Status));
      Index = 0;
      continue;
    }
    Index++;
  } 
}
// byo231031 + <<



VOID
RasLateBootInit (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  gBS->CloseEvent(Event);  
  
  //PCI-E AER platform-first enablement
  PcieAerErrorConfig ();

  PcieCapErrorConfig ();
  
  //NBIF platform-first enablement
  NbifErrorConfig ();

  // FST action config
  FstActionCtrl();

  RemoveApeiTableIfNeeded();                   // byo231031 +
}

EFI_STATUS
PlatformConfigInit (
  VOID
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT64      MmioMsr;
  HYGON_RAS_POLICY  *HygonRasPolicy;           // byo231031 +

// byo231031 + >>
  Status = gBS->LocateProtocol(&gHygonRasInitDataProtocolGuid, NULL, (VOID**)&HygonRasPolicy);
  ASSERT(!EFI_ERROR(Status));
// byo231031 + <<

  //
  // Allocate memory and Initialize for Data block
  //
  Status = gBS->AllocatePool (
                  EfiReservedMemoryType,
                  sizeof (PLATFORM_APEI_PRIVATE_BUFFER),
                  (VOID **)&mPlatformApeiPrivate
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (mPlatformApeiPrivate, sizeof (PLATFORM_APEI_PRIVATE_BUFFER));

  mPlatformApeiPrivate->Valid = FALSE;
  mPlatformApeiPrivate->PlatformHestValid = FALSE;
  mPlatformApeiPrivate->PlatformBertValid = FALSE;
  mPlatformApeiPrivate->PlatformEinjValid = FALSE;

  mPlatformApeiPrivate->PlatRasPolicy.PFEHEnable = HygonRasPolicy->PFEHEnable;                                      // byo231031 -
  mPlatformApeiPrivate->PlatRasPolicy.FchApuRasSmiSupport = PcdGetBool (PcdHygonFchApuRasSmiSupport);

  mPlatformApeiPrivate->PlatRasPolicy.MceSwSmiData      = HygonRasPolicy->MceSwSmiData;                             // byo231031 -
  mPlatformApeiPrivate->PlatRasPolicy.EinjSwSmiData     = PcdGet8 (PcdCpmEinjSwSmiData);
  mPlatformApeiPrivate->PlatRasPolicy.McaErrThreshEn    = HygonRasPolicy->McaErrThreshEn;                           // byo231031 -
  mPlatformApeiPrivate->PlatRasPolicy.McaErrThreshCount = HygonRasPolicy->McaErrThreshCount;                        // byo231031 -
  mPlatformApeiPrivate->PlatRasPolicy.McaPollInterval   = PcdGet32 (PcdCpmMcaPollInterval);
  mPlatformApeiPrivate->PlatRasPolicy.NbioPollInterval  = PcdGet32 (PcdCpmNbioPollInterval);
  mPlatformApeiPrivate->PlatRasPolicy.PciePollInterval  = PcdGet32 (PcdCpmPciePollInterval);
  mPlatformApeiPrivate->PlatRasPolicy.NbioCorrectedErrThreshEn    = HygonRasPolicy->NbioCorrectedErrThreshEn;       // byo231031 -
  mPlatformApeiPrivate->PlatRasPolicy.NbioCorrectedErrThreshCount = HygonRasPolicy->NbioCorrectedErrThreshCount;    // byo231031 -
  mPlatformApeiPrivate->PlatRasPolicy.NbioDeferredErrThreshEn     = HygonRasPolicy->NbioDeferredErrThreshEn;        // byo231031 -
  mPlatformApeiPrivate->PlatRasPolicy.NbioDeferredErrThreshCount  = HygonRasPolicy->NbioDeferredErrThreshCount;     // byo231031 -
  mPlatformApeiPrivate->PlatRasPolicy.FchSataRasSupport = PcdGetBool (PcdSataRasSupport);
  mPlatformApeiPrivate->PlatRasPolicy.RasSmiThreshold   = PcdGet32 (PcdCpmSmiThreshold);
  mPlatformApeiPrivate->PlatRasPolicy.RasSmiScale = PcdGet32 (PcdCpmSmiScale);
  mPlatformApeiPrivate->PlatRasPolicy.RasThresholdPeriodicSmiEn = PcdGetBool (PcdCpmThresholdPeriodicSmiEn);
  mPlatformApeiPrivate->PlatRasPolicy.RasRetryCnt = PcdGet32 (PcdCpmRasRetryCount);
  mPlatformApeiPrivate->PlatRasPolicy.EinjTrigErrSwSmiId = PcdGet8 (PcdCpmEinjTrigErrSwSmiId);
  mPlatformApeiPrivate->PlatRasPolicy.McaErrThreshSwCount = HygonRasPolicy->McaErrThreshSwCount;                    // byo231101 +
  mPlatformApeiPrivate->PlatRasPolicy.McaNonMemErrThresh  = HygonRasPolicy->McaNonMemErrThresh;                     // byo231109 +

  // FCH software SMI command port
  mPlatformApeiPrivate->PlatRasPolicy.SwSmiCmdPortAddr = HygonRasPolicy->SwSmiCmdPortAddr;                          // byo231031 -

  // Get PCI configuration MMIO base address.
  MmioMsr = AsmReadMsr64 (MSR_MMIO_CFG_BASE);
  mPlatformApeiPrivate->PcieBaseAddress = (MmioMsr & 0x0000FFFFFFF00000);

// byo231031 + >>
  DEBUG((EFI_D_INFO, "McaErrThreshEn     :%x\n", mPlatformApeiPrivate->PlatRasPolicy.McaErrThreshEn));
  DEBUG((EFI_D_INFO, "McaErrThreshSwCount:%x\n", mPlatformApeiPrivate->PlatRasPolicy.McaErrThreshSwCount));         // byo231109 +
  DEBUG((EFI_D_INFO, "McaErrThreshCount  :%x\n", mPlatformApeiPrivate->PlatRasPolicy.McaErrThreshCount));
  DEBUG((EFI_D_INFO, "McaNonMemErrThresh :%x\n", mPlatformApeiPrivate->PlatRasPolicy.McaNonMemErrThresh));          // byo231109 -
  DEBUG((EFI_D_INFO, "PcieBaseAddress    :%x\n", mPlatformApeiPrivate->PcieBaseAddress));
  DEBUG((EFI_D_INFO, "PFEHEnable         :%x\n", mPlatformApeiPrivate->PlatRasPolicy.PFEHEnable));
  DEBUG((EFI_D_INFO, "MceSwSmiData       :%x\n", mPlatformApeiPrivate->PlatRasPolicy.MceSwSmiData));
  DEBUG((EFI_D_INFO, "SwSmiCmdPortAddr   :%x\n", mPlatformApeiPrivate->PlatRasPolicy.SwSmiCmdPortAddr));
  DEBUG((EFI_D_INFO, "FchApuRasSmiSupport:%x\n", mPlatformApeiPrivate->PlatRasPolicy.FchApuRasSmiSupport));
  DEBUG((EFI_D_INFO, "RasSmiThreshold    :%x\n", mPlatformApeiPrivate->PlatRasPolicy.RasSmiThreshold));
// byo231031 + <<
  
  return Status;
}

EFI_STATUS
NbioHestUpdate (
  VOID
  )
{
  EFI_STATUS                                Status = EFI_SUCCESS;
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE  *NbioErrBlk;
  GENERIC_NBIO_ERR_ENTRY                    *NbioErrData;
  UINT64                                    *ErrBlkAddress;
  EFI_GUID                                  NbioErrorSectGuid = NBIO_ERROR_SECT_GUID;

  //
  // Allocate memory and Initialize for NBIO Error Data block
  //

  Status = gBS->AllocatePool (
                  EfiReservedMemoryType,
                  sizeof (UINT64),
                  &ErrBlkAddress
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->AllocatePool (
                  EfiReservedMemoryType,
                  MAX_ERROR_BLOCK_SIZE,
                  (VOID **)&NbioErrBlk
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (NbioErrBlk, MAX_ERROR_BLOCK_SIZE);

  // Init Hest NBIO Generic error status block.
  gBS->CopyMem (NbioErrBlk, &HestErrRegTbl, sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE));

  // Init Hest NBIO Generic error data entry
  NbioErrData = (GENERIC_NBIO_ERR_ENTRY *)(NbioErrBlk + 1);
  gBS->CopyMem (NbioErrData, &gGenNbioErrEntry, sizeof (GENERIC_NBIO_ERR_ENTRY));

  // Update Error section GUID
  gBS->CopyMem (&NbioErrData->GenErrorDataEntry.SectionType[0], &NbioErrorSectGuid, sizeof (EFI_GUID));

  // Update pointer and install NBIO error source record.
  *ErrBlkAddress = (UINT64)NbioErrBlk;
  gNbioErrSrc.ErrorStatusAddress.Address = (UINT64)ErrBlkAddress;

  // Update NBIO error notification polling interval time.
  gNbioErrSrc.NotificationStructure.PollInterval = mPlatformApeiPrivate->PlatRasPolicy.NbioPollInterval;

  HygonRasApeiProtocol->AddHestErrorSourceEntry ((UINT8 *)&gNbioErrSrc, sizeof (EFI_ACPI_6_0_GENERIC_HARDWARE_ERROR_SOURCE_STRUCTURE));

  mPlatformApeiPrivate->HygonNbioErrBlk = NbioErrBlk;

  return Status;
}

UINT8
GetPhysicalPortNumber (
  IN UINT8  RbBusNum,
  IN UINT8  IohubPhysicalDieNum,
  IN UINT8  RbNum,
  IN UINT32 Device,
  IN UINT32 Function
  )
{
  UINT32  Index;
  UINT32  Value;
  UINT32  Addr;
  UINT32  CpuModel;
  UINT32  DevRemapAddr;
  UINT32  DevRemapAddrN18;
  
  Addr = (Device << 3) + Function;
  CpuModel = GetHygonSocModel();
  
  DevRemapAddr = (CpuModel == HYGON_EX_CPU) ? NB_PROG_DEVICE_REMAP_HYEX : NB_PROG_DEVICE_REMAP_HYGX;
  DevRemapAddrN18 = (CpuModel == HYGON_EX_CPU) ? NB_PROG_DEVICE_REMAP_N18_HYEX : NB_PROG_DEVICE_REMAP_N18_HYGX;
  
  // Get PCIE physical port
  for (Index = 0; Index < 20; Index++) {
    if (Index < 18) {
      SmnRegisterRead (RbBusNum, NBIO_SPACE2 (IohubPhysicalDieNum, RbNum, DevRemapAddr + (Index << 2)), &Value);
    } else {
      SmnRegisterRead (RbBusNum, NBIO_SPACE2 (IohubPhysicalDieNum, RbNum, DevRemapAddrN18 + ((Index-18) << 2)), &Value);
    }

    if (Addr == (Value & 0xFF)) {
      return (UINT8)Index;
    }
  }

  // Error return, never run here
  return 0xFF;
}

EFI_STATUS
PciePortDetect (
  VOID
  )
{
  EFI_STATUS                               Status = EFI_SUCCESS;
  PCIE_ACTIVE_PORT_MAP                     *TempPciePortMap;
  PCIE_ACTIVE_PORT_MAP                     *mPciePortMap;
  UINT32                                   PciePortMapSize;
  UINT8                                    Port;
  UINT8                                    PhysicalPort;
  UINT8                                    MaxPciePorts;
  UINTN                                    RbBusNum;
  UINT16                                   VendorID;
  PCI_ADDR                                 PciPortAddr;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  UINTN                                    NumberOfSockets;
  UINTN                                    NumberOfPhysicalDies;
  UINTN                                    NumberOfLogicalDies;
  UINTN                                    RbNumberOfLogicalDie;
  UINTN                                    SocketIndex, LogicalDieIndex, RbIndex;
  UINT8                                    IohubPhysicalDieNum;
  UINT32                                   CpuModel;
  
  CpuModel = GetHygonSocModel();
  MaxPciePorts = (CpuModel == HYGON_EX_CPU) ? MAX_PCIE_PORT_SUPPORT_HYEX : MAX_PCIE_PORT_SUPPORT_HYGX;

  //
  // Allocate memory and Initialize a temporary PCI-E Port Map data block and NBIO Error Data block pointer
  //
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  MAX_PCIEMAP_BLOCK_SIZE,
                  (VOID **)&TempPciePortMap
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (TempPciePortMap, MAX_PCIEMAP_BLOCK_SIZE);

  Status = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **)&FabricTopology);
  ASSERT (!EFI_ERROR (Status));

  FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL);

  // Register Error handler per Rb.
  for (SocketIndex = 0; SocketIndex < NumberOfSockets; SocketIndex++) {
    FabricTopology->GetProcessorInfo (FabricTopology, SocketIndex, &NumberOfPhysicalDies, &NumberOfLogicalDies, NULL);

    for(LogicalDieIndex = 0; LogicalDieIndex < NumberOfLogicalDies; LogicalDieIndex++) {
      FabricTopology->GetDieInfo (FabricTopology, SocketIndex, LogicalDieIndex, &RbNumberOfLogicalDie, NULL, NULL);

      for (RbIndex = 0; RbIndex < RbNumberOfLogicalDie; RbIndex++) {
        FabricTopology->GetRootBridgeInfo (FabricTopology, SocketIndex, LogicalDieIndex, RbIndex, NULL, &RbBusNum, NULL);
        DEBUG ((EFI_D_ERROR, "[RAS-DXE]Socket %d, Logical Die %d, Rb %d, Bus 0x%x \n", SocketIndex, LogicalDieIndex, RbIndex, RbBusNum));

        PciPortAddr.AddressValue = 0;

        for (Port = 0; Port < MaxPciePorts; Port++) {
          PciPortAddr.Address.Bus      = (UINT32)RbBusNum;
          PciPortAddr.Address.Device   = gPciePortList[Port].Device;
          PciPortAddr.Address.Function = gPciePortList[Port].Function;

          VendorID = PciRead16 (PciPortAddr.AddressValue);
          if (VendorID != HYGON_VID) {
            continue;
          }

          DEBUG ((
            EFI_D_ERROR,
            "[RAS-DXE]Active PCI-E Root Port Bus:%d Dev:%d Fun:%d, Address: 0x%08x\n",
            PciPortAddr.Address.Bus,
            PciPortAddr.Address.Device,
            PciPortAddr.Address.Function,
            PciPortAddr.AddressValue
            ));

          // Found active PCI-E port
          TempPciePortMap->PciPortNumber[TempPciePortMap->PortCount].NbioSocketNum     = (UINT8)SocketIndex;
          TempPciePortMap->PciPortNumber[TempPciePortMap->PortCount].NbioLogicalDieNum = (UINT8)LogicalDieIndex;
          TempPciePortMap->PciPortNumber[TempPciePortMap->PortCount].NbioRbNum  = (UINT8)RbIndex;
          TempPciePortMap->PciPortNumber[TempPciePortMap->PortCount].NbioBusNum = (UINT8)RbBusNum;

          IohubPhysicalDieNum = (UINT8)FabricTopologyGetIohubPhysicalDieId (LogicalDieIndex);
          TempPciePortMap->PciPortNumber[TempPciePortMap->PortCount].NbioIohubPhysicalDieNum = IohubPhysicalDieNum;
          // Get PCIE logical port
          TempPciePortMap->PciPortNumber[TempPciePortMap->PortCount].PciPortNumber = Port;
          // Get PCIE physical port
          PhysicalPort = GetPhysicalPortNumber ((UINT8)RbBusNum, IohubPhysicalDieNum, (UINT8)RbIndex, PciPortAddr.Address.Device, PciPortAddr.Address.Function);
          TempPciePortMap->PciPortNumber[TempPciePortMap->PortCount].PciPhysicalPortNumber = PhysicalPort;

          DEBUG ((EFI_D_ERROR, "[RAS-DXE] Logical port %d, physical port %d\n", Port, PhysicalPort));

          TempPciePortMap->PortCount++;
        }
      }
    }
  }

  DEBUG ((EFI_D_ERROR, "[RAS-DXE]Total Active PCI-E port count %d\n", TempPciePortMap->PortCount));

  PciePortMapSize = (sizeof (PCIE_ACTIVE_PORT_MAP)) + ((sizeof (PCIE_PORT_PROFILE)) * (TempPciePortMap->PortCount - 1));

  Status = gBS->AllocatePool (
                  EfiReservedMemoryType,
                  PciePortMapSize,
                  (VOID **)&mPciePortMap
                  );
  if (EFI_ERROR (Status)) {
    // release temporary PCI-E port map space.
    gBS->FreePool (TempPciePortMap);
    return Status;
  }

  // Copy to reserve memory space
  gBS->CopyMem (mPciePortMap, TempPciePortMap, PciePortMapSize);

  mPlatformApeiPrivate->HygonPciePortMap = mPciePortMap;

  return Status;
}

EFI_STATUS
AddHestGhes ( 
  IN       UINT16   SourceId,
  IN       UINT16   RelatedSourceId,
  IN       UINT32   ErrDataEntrySize,
  IN       EFI_GUID *ErrorSectGuid,
  IN       EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE      *GenErrEntry,
  IN       EFI_ACPI_6_0_GENERIC_HARDWARE_ERROR_SOURCE_STRUCTURE *GenErrSrc,
  IN       EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE   *NotificationStructure,
  OUT      EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE          **BootErrBlk
 )
{
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE              *GenBootErrBlk;
  EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE       *ErrDataEntry;
  UINT64                                                *ErrBlkAddress;
  EFI_STATUS                                            Status = EFI_SUCCESS;
  UINT32                                                Index;
  UINT32                                                ErrorBlockSize;

  Index = 0;
  ErrorBlockSize = MAX_ERROR_BLOCK_SIZE;

  Status = gBS->AllocatePool (
                  EfiReservedMemoryType,
                  ErrorBlockSize,
                  (VOID **)&GenBootErrBlk
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (GenBootErrBlk,  ErrorBlockSize);

  //
  //  Allocate memory and Initialize memory blocks for Error Data block and Error Data block pointer
  //

  Status = gBS->AllocatePool (
                EfiReservedMemoryType,
                sizeof (UINT64),
                &ErrBlkAddress
                );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //Init Hest Generic error status block.
  gBS->CopyMem (GenBootErrBlk, &HestErrRegTbl, sizeof (EFI_ACPI_6_0_GENERIC_ERROR_STATUS_STRUCTURE));

  //Init Hest Generic error data entry
  ErrDataEntry = (EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE*) (GenBootErrBlk + 1);
  gBS->CopyMem (ErrDataEntry, GenErrEntry, ErrDataEntrySize);

  //Update Error section GUID
  gBS->CopyMem (&ErrDataEntry->SectionType[0], ErrorSectGuid, sizeof (EFI_GUID));

  //Update pointer and install error source record.
  *ErrBlkAddress = (UINT64) GenBootErrBlk;
  GenErrSrc->ErrorStatusAddress.Address =  (UINT64)ErrBlkAddress;

  //Update notification structure.
  gBS->CopyMem (&GenErrSrc->NotificationStructure, NotificationStructure, sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE));

  GenErrSrc->SourceId = SourceId;
  GenErrSrc->RelatedSourceId = RelatedSourceId;
  GenErrSrc->MaxRawDataLength = ErrorBlockSize;
  GenErrSrc->ErrorStatusBlockLength = ErrorBlockSize;

  HygonRasApeiProtocol->AddHestErrorSourceEntry ((UINT8*)GenErrSrc, sizeof (EFI_ACPI_6_0_GENERIC_HARDWARE_ERROR_SOURCE_STRUCTURE));
  
  *BootErrBlk = GenBootErrBlk;
  return Status;
}

EFI_STATUS
PcieAerHestUpdate (
  VOID
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_ACPI_6_0_PCI_EXPRESS_ROOT_PORT_AER_STRUCTURE  *PcieAerHestTableHead;
  EFI_ACPI_6_0_PCI_EXPRESS_ROOT_PORT_AER_STRUCTURE  *PcieRpAerHestTable;
  EFI_ACPI_6_0_PCI_EXPRESS_DEVICE_AER_STRUCTURE     *PcieDevAerHestTable;
  EFI_ACPI_6_0_PCI_EXPRESS_BRIDGE_AER_STRUCTURE     *PcieBridgeAerHestTable;
  UINT32                                            PcieAerHestTableSize;
  BOOLEAN                                           AerReportPcd;
  EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE* NotificationStructure;
//EFI_GUID                                          PcieErrorSectGuid = PCIE_SECT_GUID;                  // byo231109 -
  UINT32                                            ErrSrcCount;

  PcieAerHestTableSize = sizeof (EFI_ACPI_6_0_PCI_EXPRESS_ROOT_PORT_AER_STRUCTURE) + \
                         sizeof (EFI_ACPI_6_0_PCI_EXPRESS_DEVICE_AER_STRUCTURE) + \
                         sizeof (EFI_ACPI_6_0_PCI_EXPRESS_BRIDGE_AER_STRUCTURE);

  Status = gBS->AllocatePool (EfiBootServicesData,
                              PcieAerHestTableSize ,
                              &PcieAerHestTableHead);
  if (EFI_ERROR (Status)) {
    return Status;
  } else {
    //clear instances content
    gBS->SetMem (PcieAerHestTableHead, PcieAerHestTableSize, 0);
  }
  DEBUG ((EFI_D_ERROR, "[RAS]Install PCI Express AER HEST table\n"));

  AerReportPcd = mPlatformApeiPrivate->PlatRasPolicy.PFEHEnable;                                          // byo231031 -
  ErrSrcCount = 0;

  //Setup Root Port AER Structure
  PcieRpAerHestTable = PcieAerHestTableHead;

  PcieRpAerHestTable->Type = EFI_ACPI_6_0_PCI_EXPRESS_ROOT_PORT_AER;
  PcieRpAerHestTable->SourceId = PCIE_ROOT_PORT_SRC_ID;

  if (AerReportPcd) {
    PcieRpAerHestTable->Flags = EFI_ACPI_6_0_ERROR_SOURCE_FLAG_GLOBAL | EFI_ACPI_6_0_ERROR_SOURCE_FLAG_FIRMWARE_FIRST;
  } else {
    PcieRpAerHestTable->Flags = EFI_ACPI_6_0_ERROR_SOURCE_FLAG_GLOBAL;
  }

  PcieRpAerHestTable->Enabled = 1;
  PcieRpAerHestTable->NumberOfRecordsToPreAllocate = 0x1;
  PcieRpAerHestTable->MaxSectionsPerRecord = 0x10;
  PcieRpAerHestTable->DeviceControl = 0x7;
  PcieRpAerHestTable->UncorrectableErrorMask = mPlatformApeiPrivate->PlatRasPolicy.PcieRpUnCorrectedErrorMask;
  PcieRpAerHestTable->UncorrectableErrorSeverity = mPlatformApeiPrivate->PlatRasPolicy.PcieRpUnCorrectedErrorSeverity;
  PcieRpAerHestTable->CorrectableErrorMask = mPlatformApeiPrivate->PlatRasPolicy.PcieRpCorrectedErrorMask;
  PcieRpAerHestTable->AdvancedErrorCapabilitiesAndControl = 0;
  PcieRpAerHestTable->RootErrorCommand = 0;
  ErrSrcCount++;  //Create PcieRpAerHestTable - done

  //Setup Device AER Structure
  PcieRpAerHestTable++;
  PcieDevAerHestTable = (EFI_ACPI_6_0_PCI_EXPRESS_DEVICE_AER_STRUCTURE*)(PcieRpAerHestTable);
  PcieDevAerHestTable->Type = EFI_ACPI_6_0_PCI_EXPRESS_DEVICE_AER;
  PcieDevAerHestTable->SourceId = PCIE_DEVICE_SOURCE_ID;

  if (AerReportPcd) {
    PcieDevAerHestTable->Flags = EFI_ACPI_6_0_ERROR_SOURCE_FLAG_GLOBAL | EFI_ACPI_6_0_ERROR_SOURCE_FLAG_FIRMWARE_FIRST;
  } else {
    PcieDevAerHestTable->Flags = EFI_ACPI_6_0_ERROR_SOURCE_FLAG_GLOBAL;
  }
  PcieDevAerHestTable->Enabled = 1;
  PcieDevAerHestTable->NumberOfRecordsToPreAllocate = 0x1;
  PcieDevAerHestTable->MaxSectionsPerRecord = 0x10;
  PcieDevAerHestTable->DeviceControl = 0x7;
  PcieDevAerHestTable->UncorrectableErrorMask = mPlatformApeiPrivate->PlatRasPolicy.PcieDevUnCorrectedErrorMask;
  PcieDevAerHestTable->UncorrectableErrorSeverity = mPlatformApeiPrivate->PlatRasPolicy.PcieDevUnCorrectedErrorSeverity;
  PcieDevAerHestTable->CorrectableErrorMask = mPlatformApeiPrivate->PlatRasPolicy.PcieDevCorrectedErrorMask;
  PcieDevAerHestTable->AdvancedErrorCapabilitiesAndControl = 0;
  ErrSrcCount++;  //Create PcieDevAerHestTable - done.

  //Setup Bridge AER Structure
  PcieDevAerHestTable++;
  PcieBridgeAerHestTable = (EFI_ACPI_6_0_PCI_EXPRESS_BRIDGE_AER_STRUCTURE*)(PcieDevAerHestTable);
  PcieBridgeAerHestTable->Type = EFI_ACPI_6_0_PCI_EXPRESS_BRIDGE_AER;
  PcieBridgeAerHestTable->SourceId = PCIE_BRIDGE_SOURCE_ID;

   if (AerReportPcd) { 
    PcieBridgeAerHestTable->Flags = EFI_ACPI_6_0_ERROR_SOURCE_FLAG_GLOBAL | EFI_ACPI_6_0_ERROR_SOURCE_FLAG_FIRMWARE_FIRST;
  } else {
    PcieBridgeAerHestTable->Flags = EFI_ACPI_6_0_ERROR_SOURCE_FLAG_GLOBAL;
  }
  PcieBridgeAerHestTable->Enabled = 1;
  PcieBridgeAerHestTable->NumberOfRecordsToPreAllocate = 0x1;
  PcieBridgeAerHestTable->MaxSectionsPerRecord = 0x10;
  PcieBridgeAerHestTable->DeviceControl = 0x7;
  PcieBridgeAerHestTable->UncorrectableErrorMask = mPlatformApeiPrivate->PlatRasPolicy.PcieDevUnCorrectedErrorMask;
  PcieBridgeAerHestTable->UncorrectableErrorSeverity = mPlatformApeiPrivate->PlatRasPolicy.PcieDevUnCorrectedErrorSeverity;
  PcieBridgeAerHestTable->CorrectableErrorMask = mPlatformApeiPrivate->PlatRasPolicy.PcieDevCorrectedErrorMask;
  PcieBridgeAerHestTable->AdvancedErrorCapabilitiesAndControl = 0;
  PcieBridgeAerHestTable->SecondaryUncorrectableErrorMask = mPlatformApeiPrivate->PlatRasPolicy.PcieDevUnCorrectedErrorMask;
  PcieBridgeAerHestTable->SecondaryUncorrectableErrorSeverity = mPlatformApeiPrivate->PlatRasPolicy.PcieDevUnCorrectedErrorSeverity;
  PcieBridgeAerHestTable->SecondaryAdvancedErrorCapabilitiesAndControl = 0;
  ErrSrcCount++;  //Create PcieBridgeAerHestTable - done.

  //Add structure to HEST table
  HygonRasApeiProtocol->AddMultipleHestErrorSourceEntry ((UINT8*)PcieAerHestTableHead, PcieAerHestTableSize, ErrSrcCount);

  gBS->FreePool (PcieAerHestTableHead);

  //Set PCIE GHES and link to PCIE AER structure
  Status = gBS->AllocatePool (EfiBootServicesData,
                              sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE) ,
                              &NotificationStructure);
  if (EFI_ERROR (Status)) {
    return Status;
  } else {
    //clear instances content
    gBS->SetMem (NotificationStructure, sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE), 0);
  }  
  NotificationStructure->Type = HARDWARE_ERROR_NOTIFICATION_POLLED;
  NotificationStructure->Length = sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE);
  NotificationStructure->PollInterval = mPlatformApeiPrivate->PlatRasPolicy.PciePollInterval;

  //Add Root Port GHES Assist
  
  AddHestGhes (PCIE_RP_GHES_CORR_SRC_ID,
               PCIE_ROOT_PORT_SRC_ID,
               sizeof (GENERIC_PCIE_AER_ERR_ENTRY),
               &gPcieErrorSectGuid,                                                     // byo231109 -
               (EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE*)&gGenPcieErrEntry,
               &gPcieGenErrSrc,
               NotificationStructure,
               &mPlatformApeiPrivate->HygonPcieAerErrBlk
               );
 
  DEBUG ((EFI_D_ERROR, "Root Port Correctable Error Block Address: 0x%08x\n", (UINTN)mPlatformApeiPrivate->HygonPcieAerErrBlk));

  AddHestGhes (PCIE_RP_GHES_UNCORR_SRC_ID,
               PCIE_ROOT_PORT_SRC_ID,
               sizeof (GENERIC_PCIE_AER_ERR_ENTRY),
               &gPcieErrorSectGuid,                                                     // byo231109 -
               (EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE*)&gGenPcieErrEntry,
               &gPcieGenErrSrc,
               NotificationStructure,
               &mPlatformApeiPrivate->HygonPcieAerUnErrBlk
               );
  DEBUG ((EFI_D_ERROR, "Root Port Un-Correctable Error Block Address: 0x%08x\n", (UINTN)mPlatformApeiPrivate->HygonPcieAerUnErrBlk));
    
  //Add Device GHES Assist
    
  AddHestGhes (PCIE_DEV_GHES_CORR_SRC_ID,
               PCIE_DEVICE_SOURCE_ID,
               sizeof (GENERIC_PCIE_AER_ERR_ENTRY),
               &gPcieErrorSectGuid,                                                     // byo231109 -
               (EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE*)&gGenPcieErrEntry,
               &gPcieGenErrSrc,
               NotificationStructure,
               &mPlatformApeiPrivate->HygonPcieDevAerErrBlk
               );
  DEBUG ((EFI_D_ERROR, "Devices Correctable Error Block Address: 0x%08x\n", (UINTN)mPlatformApeiPrivate->HygonPcieDevAerErrBlk)); 
    
  AddHestGhes (PCIE_DEV_GHES_UNCORR_SRC_ID,
               PCIE_DEVICE_SOURCE_ID,
               sizeof (GENERIC_PCIE_AER_ERR_ENTRY),
               &gPcieErrorSectGuid,                                                     // byo231109 -
               (EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE*)&gGenPcieErrEntry,
               &gPcieGenErrSrc,
               NotificationStructure,
               &mPlatformApeiPrivate->HygonPcieDevAerUnErrBlk
               );
  DEBUG ((EFI_D_ERROR, "Devices Un-Correctable Error Block Address: 0x%08x\n", (UINTN)mPlatformApeiPrivate->HygonPcieDevAerUnErrBlk));
    
  //Add Bridge GHES Assist
  
  AddHestGhes (PCIE_BRIDGE_GHES_CORR_SRC_ID,
               PCIE_BRIDGE_SOURCE_ID,
               sizeof (GENERIC_PCIE_AER_ERR_ENTRY),
               &gPcieErrorSectGuid,                                                     // byo231109 -
               (EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE*)&gGenPcieErrEntry,
               &gPcieGenErrSrc,
               NotificationStructure,
               &mPlatformApeiPrivate->HygonPcieBridgeAerErrBlk
               );
    
  AddHestGhes (PCIE_BRIDGE_GHES_UNCORR_SRC_ID,
               PCIE_BRIDGE_SOURCE_ID,
               sizeof (GENERIC_PCIE_AER_ERR_ENTRY),
               &gPcieErrorSectGuid,                                                     // byo231109 -
               (EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE*)&gGenPcieErrEntry,
               &gPcieGenErrSrc,
               NotificationStructure,
               &mPlatformApeiPrivate->HygonPcieBridgeAerUnErrBlk
               );

  gBS->FreePool (NotificationStructure);

  return Status;
}

EFI_STATUS
NbioSetDpc (
  PCI_ADDR  PciCfgAddr
  )
{
  PCIE_DPC_CNTL_REG  DpcReg;
  PCI_ADDR           PciPortAddr;
  UINT16             DpcCapPtr;

  // Move to local variable
  PciPortAddr.AddressValue = PciCfgAddr.AddressValue;

  // Read DPC Extended Capability
  DpcCapPtr = RasFindPcieExtendedCapability (PciCfgAddr.AddressValue, DPC_EXT_CAP_ID);
  if (DpcCapPtr != 0) {
    PciPortAddr.Address.Register = (UINT32)(DpcCapPtr + 6);
    DEBUG ((EFI_D_ERROR, "   PCIE_DPC_CNTL Address: 0x%08x\n", PciPortAddr.AddressValue));
    DpcReg.Raw = PciRead16 (PciPortAddr.AddressValue);
    DEBUG ((EFI_D_ERROR, "   PCIE_DPC_CNTL Value: 0x%08x\n", DpcReg.Raw));

    // Platform BIOS needs to enable eDPC.
    // For each PCIe link, platform BIOS should program the following in the eDPC Control Register (PCIE_DPC_CNTL):
    // DPC Trigger Enable: 01b - FATAL only
    // DPC Completion Control - 1b (UR)
    // DPC Interrupt Enable - 0b
    // DPC ERR_COR Enable - 0b
    // Poisoned TLP Egress Blocking Enable - 0b
    // DPC Software Trigger - 0b
    // DL_Active ERR_COR Enable - 0b
    //
    DpcReg.DpcRegField.DpcTriggerEnable     = 0x01;
    DpcReg.DpcRegField.DpcCompletionControl = 1;
    DpcReg.DpcRegField.DpcInterruptEnable   = 0;
    DpcReg.DpcRegField.DpcErrCorEnable = 0;
    DpcReg.DpcRegField.PoisonedTLPEgressBlkEn = 0;
    DpcReg.DpcRegField.DpcSoftwareTrigger     = 0;
    DpcReg.DpcRegField.DlActiveErrCorEnable   = 0;

    // Set PCIE_DPC_CNTL: 0x386
    PciWrite16 (PciPortAddr.AddressValue, DpcReg.Raw);
    gDpcEn = TRUE;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PcieStsClr (
  PCI_ADDR          PciPortAddr
  )
{
  UINT16  AerCapPtr;
  UINT16  SecPcieExtCapPtr;
  UINT8   PcieCapPtr;
  UINT32  PcieUncorrStatus;
  UINT32  PcieCorrStatus;
  UINT32  PcieRootStatus;
  UINT16  PcieDevStatus;
  UINT32  PcieLnkStatus;
  UINT32  RasRetryCounter;

  AerCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, PCIE_EXT_AER_CAP_ID);
  SecPcieExtCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, SEC_PCIE_EXT_CAP_ID);
  PcieCapPtr = RasFindPciCapability (PciPortAddr.AddressValue, PCIE_CAP_ID);

  PcieUncorrStatus = 0xFFFFFFFF;
  PcieCorrStatus = 0xFFFFFFFF;
  PcieRootStatus = 0xFFFFFFFF;
  PcieDevStatus = 0xF;
  PcieLnkStatus = 0xFFFFFFFF;

  DEBUG ((EFI_D_ERROR, "[RAS]ClrSts @ Bus: 0x%x, Dev: 0x%x, Func: 0x%x\n", PciPortAddr.Address.Bus, PciPortAddr.Address.Device, PciPortAddr.Address.Function));
  //Clear Status register
  RasRetryCounter = mPlatformApeiPrivate->PlatRasPolicy.RasRetryCnt;
  if (AerCapPtr != 0) {
    PcieUncorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR); 
    PcieCorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR);
    PcieRootStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_ROOT_STATUS_PTR);
    while ((PcieUncorrStatus !=0)||(PcieCorrStatus !=0) || (PcieRootStatus !=0)) {
      if (RasRetryCounter == 0) {
        break;
      } else {
        RasRetryCounter--; 
      }
      DEBUG ((EFI_D_ERROR, "[RAS] PCIE UnCorr Error Status : 0x%08x\n", PcieUncorrStatus));
      PciWrite32(PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR, PcieUncorrStatus);
      PcieUncorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR);
      
      DEBUG ((EFI_D_ERROR, "[RAS] PCIE Corr Error Status : 0x%08x\n", PcieCorrStatus));
      PciWrite32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR, PcieCorrStatus);
      PcieCorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR);
         
      if (RasGetPcieDeviceType (PciPortAddr) == PcieDeviceRootComplex) {
        DEBUG ((EFI_D_ERROR, "[RAS] PCIE Root Error Status : 0x%08x\n", PcieRootStatus));
        PciWrite32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_ROOT_STATUS_PTR, PcieRootStatus);
        PcieRootStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_ROOT_STATUS_PTR);
      }
    }
  }
  RasRetryCounter = mPlatformApeiPrivate->PlatRasPolicy.RasRetryCnt;
  if (PcieCapPtr != 0) {
    PcieDevStatus = PciRead16 (PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_STATUS_PTR);
    while ((PcieDevStatus & 0xF) != 0) {
      if (RasRetryCounter == 0) {
        break;
      } else {
        RasRetryCounter--; 
      }
    DEBUG ((EFI_D_ERROR, "[RAS] PCIE Device Status : 0x%08x\n", PcieDevStatus));
    PciWrite16(PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_STATUS_PTR, PcieDevStatus);
    PcieDevStatus = PciRead16 (PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_STATUS_PTR);
    }
  }
  RasRetryCounter = mPlatformApeiPrivate->PlatRasPolicy.RasRetryCnt;
  if (SecPcieExtCapPtr !=0 ) {
    PcieLnkStatus = PciRead32 (PciPortAddr.AddressValue + SecPcieExtCapPtr + PCIE_LANE_ERR_STATUS_PTR);
    while (PcieLnkStatus != 0) {
      if (RasRetryCounter == 0) {
        break;
      } else {
        RasRetryCounter--; 
      }
      PciWrite32 (PciPortAddr.AddressValue + SecPcieExtCapPtr + PCIE_LANE_ERR_STATUS_PTR, PcieLnkStatus);
      PcieLnkStatus = PciRead32 (PciPortAddr.AddressValue + SecPcieExtCapPtr + PCIE_LANE_ERR_STATUS_PTR);
	  DEBUG((EFI_D_ERROR, "[RAS] PCIE Lane Error Status : 0x%08x\n", PcieLnkStatus));
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PcieActionCtrl (
  UINT16    NbioBusNum,
  UINT16    NbioIohubDieNum,
  UINT16    NbioRbNum,
  UINT16    PciePortNum
  )
{
  UINT32            PciePortActCtrlBase;
  UINT32            Index;
  ERR_ACT_CTRL_REG  *PciePortActConfigList;
  UINT32            CpuModel;
  
  CpuModel = GetHygonSocModel();
  
  PciePortActCtrlBase  = NBIO_SPACE2 (NbioIohubDieNum, NbioRbNum, (CpuModel==HYGON_EX_CPU)?PCIE_PORT_ACTION_CONTROL_BASE_HYEX:PCIE0_PORT_ACTION_CONTROL_BASE_HYGX);
  PciePortActCtrlBase += PciePortNum * PCIE_ACTION_CONTROL_OFFSET;

  if (gAerEn) {
    if (gDpcEn) {
      DEBUG ((EFI_D_ERROR, "[RAS-DXE]Platform PCI-E Action Control. AER = TRUE, DPC = TRUE\n"));
      if(mPlatformApeiPrivate->PlatRasPolicy.PFEHEnable) {
        PciePortActConfigList = &gPciePortActListAerDpc[0];
      } else {
        PciePortActConfigList = &gPciePortActListAerPfehDis[0];
      }
    } else {
      DEBUG ((EFI_D_ERROR, "[RAS-DXE]Platform PCI-E Action Control. AER = TRUE, DPC = FALSE\n"));
      PciePortActConfigList = &gPciePortActListAerNoDpc[0];
    }
  } else {
    DEBUG ((EFI_D_ERROR, "[RAS-DXE]Platform PCI-E Action Control. AER = FALSE\n"));
    PciePortActConfigList = &gPciePortActListNoAer[0];
  }

  for (Index = 0; Index < 8; Index++) {
    SmnRegisterWrite ((UINT32)NbioBusNum, PciePortActCtrlBase + (Index * 4), &PciePortActConfigList[Index].Value, 0);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EnAerErrorReport (
  PCI_ADDR  PciPortAddr
  )
{
  UINT16  AerCapPtr;
  UINT32  PcieRootErrCmd;

  AerCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, PCIE_EXT_AER_CAP_ID);

  if (AerCapPtr != 0) {
    PcieRootErrCmd  = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_ROOT_ERR_CMD_PTR);
    PcieRootErrCmd |= PCIE_ROOT_CORR_ERR + PCIE_ROOT_NON_FATAL_ERR + PCIE_ROOT_FATAL_ERR;
    PciWrite32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_ROOT_ERR_CMD_PTR, PcieRootErrCmd);
  }

  return EFI_SUCCESS;
}

VOID
STATIC
PcieDevCntlEnableOnFunction (
  IN       PCI_ADDR             Function
  )
{
  UINT8   PcieCapPtr;
  UINT16  PcieDevCtrl;

  PcieCapPtr = RasFindPciCapability (Function.AddressValue, PCIE_CAP_ID);
  if (PcieCapPtr != 0) {
    PcieDevCtrl  = PciRead16 (Function.AddressValue + PcieCapPtr + PCIE_DEVICE_CONTROL_PTR);
    PcieDevCtrl |= PCIE_DEV_CORR_ERR + PCIE_DEV_NON_FATAL_ERR + PCIE_DEV_FATAL_ERR;
    PciWrite16 (Function.AddressValue + PcieCapPtr + PCIE_DEVICE_CONTROL_PTR, PcieDevCtrl);
  }
}

VOID
STATIC
PcieDevCntlEnableOnDevice (
  IN       PCI_ADDR             Device
  )
{
  UINT8  MaxFunc;
  UINT8  CurrentFunc;

  MaxFunc = RasPciIsMultiFunctionDevice (Device.AddressValue) ? 7 : 0;
  for (CurrentFunc = 0; CurrentFunc <= MaxFunc; CurrentFunc++) {
    Device.Address.Function = CurrentFunc;
    DEBUG ((
      EFI_D_ERROR,
      "  Checking Device: %d:%d:%d\n",
      Device.Address.Bus,
      Device.Address.Device,
      Device.Address.Function
      ));
    if (RasPciIsDevicePresent (Device.AddressValue)) {
      DEBUG ((
        EFI_D_ERROR,
        "  Enable Device Error report for Device = %d:%d:%d\n",
        Device.Address.Bus,
        Device.Address.Device,
        Device.Address.Function
        ));
      PcieDevCntlEnableOnFunction (Device);
    }
  }
}

SCAN_STATUS
STATIC
PcieDevCntlCallback (
  IN       PCI_ADDR             Device,
  IN OUT   RAS_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS       ScanStatus;
  PCIE_DEVICE_TYPE  DeviceType;

  ScanStatus = SCAN_SUCCESS;
  DEBUG ((
    EFI_D_ERROR,
    "  PcieDevCntlCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    ));
  ScanStatus = SCAN_SUCCESS;
  DeviceType = RasGetPcieDeviceType (Device);
  DEBUG ((EFI_D_ERROR, "  PCI-E device type = 0x%x\n", DeviceType));
  switch (DeviceType) {
    case PcieDeviceRootComplex:
    case PcieDeviceDownstreamPort:
    case PcieDeviceUpstreamPort:
      DEBUG ((EFI_D_ERROR, "  PCI-E device root port found\n"));
      ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
      break;
    case PcieDevicePcieToPcix:
      DEBUG ((EFI_D_ERROR, "  PCI-E device PCIE to PCIx found\n"));
      ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
      break;
    case PcieDeviceEndPoint:
    case PcieDeviceLegacyEndPoint:
      DEBUG ((EFI_D_ERROR, "  PCI-E endpoint found\n"));
      PcieDevCntlEnableOnDevice (Device);
      ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
      break;
    default:
      break;
  }

  return ScanStatus;
}

VOID
STATIC
PcieDevCntlConfigure (
  IN       PCI_ADDR  DownstreamPort
  )
{
  RAS_PCI_SCAN_DATA  ScanData;

  ScanData.Buffer = NULL;
  ScanData.RasScanCallback = PcieDevCntlCallback;
  RasPciScanSecondaryBus (DownstreamPort, &ScanData);
}

EFI_STATUS
EnDevErrReport (
  PCI_ADDR  PciPortAddr
  )
{
  UINT16  PcieDevCtrl;
  UINT8   PcieCapPtr;

  // Get active port PCI-E cap pointer
  PcieCapPtr = RasFindPciCapability (PciPortAddr.AddressValue, PCIE_CAP_ID);

  if (PcieCapPtr != 0) {
    // Enable down link PCI-E end devices.
    PcieDevCntlConfigure (PciPortAddr);

    // Enable active port dev_cntl
    PcieDevCtrl  = PciRead16 (PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_CONTROL_PTR);
    PcieDevCtrl |= PCIE_DEV_CORR_ERR + PCIE_DEV_NON_FATAL_ERR + PCIE_DEV_FATAL_ERR;
    PciWrite16 (PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_CONTROL_PTR, PcieDevCtrl);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PcieCapErrorConfig ( 
  VOID
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINT16                        PciePortIndex;
  UINT16                        PcieRootCtrl;
  UINT8                         PcieCapPtr;
  PCIE_PORT_PROFILE             *PciePortProfileInstance;
  PCI_ADDR                      PciPortAddr;

  DEBUG ((EFI_D_ERROR, "[RAS-DXE]Platform PCI-E Cap Error Report Config entry\n"));

  //Programm device's ROOT_CNT_REG to generate #SERR.
  PciePortProfileInstance = mPlatformApeiPrivate->HygonPciePortMap->PciPortNumber;
  for (PciePortIndex = 0; PciePortIndex < mPlatformApeiPrivate->HygonPciePortMap->PortCount; PciePortIndex++, PciePortProfileInstance++) {
    PciPortAddr.AddressValue = 0;
    PciPortAddr.Address.Bus = PciePortProfileInstance->NbioBusNum;
    PciPortAddr.Address.Device = gPciePortList[PciePortProfileInstance->PciPortNumber].Device;
    PciPortAddr.Address.Function = gPciePortList[PciePortProfileInstance->PciPortNumber].Function;
    DEBUG ((EFI_D_ERROR, "[RAS]PciPortAddr Bus = %x Device = %x Function = %x\n", PciPortAddr.Address.Bus, PciPortAddr.Address.Device, PciPortAddr.Address.Function));
    
    PcieCapPtr = RasFindPciCapability (PciPortAddr.AddressValue, PCIE_CAP_ID);
    if (PcieCapPtr != 0) {
       PcieRootCtrl = PciRead16 (PciPortAddr.AddressValue + PcieCapPtr + PCIE_ROOT_CONTROL_PTR);
       PcieRootCtrl |= SERR_ON_ERR_COR_EN + SERR_ON_ERR_NONFATAL_EN + SERR_ON_ERR_FATAL_EN;
       PciWrite16(PciPortAddr.AddressValue + PcieCapPtr + PCIE_ROOT_CONTROL_PTR, PcieRootCtrl);
    }
  }

  return Status; 
}

EFI_STATUS
PcieAerErrorConfig (
  VOID
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  HYGON_NBIO_PCIE_AER_PROTOCOL  *HygonNbioPcieAerProtocol;
  UINT16                        PciePortIndex;
  PCIE_PORT_PROFILE             *PciePortProfileInstance;
  PCI_ADDR                      PciPortAddr;
  PCIe_PORT_AER_CONFIG          PcieAerSetting;

  DEBUG ((EFI_D_ERROR, "[RAS-DXE]Platform PCI-E Aer Enablement entry\n"));

  // Get NBIO PCI-E AER protocol
  Status = gBS->LocateProtocol (&gHygonNbioPcieAerProtocolGuid, NULL, &HygonNbioPcieAerProtocol);
  if (EFI_ERROR (Status)) {
    return Status;    // Error detected while trying to locate protocol
  }

  PciePortProfileInstance = mPlatformApeiPrivate->HygonPciePortMap->PciPortNumber;
  for (PciePortIndex = 0; PciePortIndex < mPlatformApeiPrivate->HygonPciePortMap->PortCount; PciePortIndex++, PciePortProfileInstance++) {
    gAerEn = FALSE;
    gDpcEn = FALSE;
    PciPortAddr.AddressValue     = 0;
    PciPortAddr.Address.Bus      = PciePortProfileInstance->NbioBusNum;
    PciPortAddr.Address.Device   = gPciePortList[PciePortProfileInstance->PciPortNumber].Device;
    PciPortAddr.Address.Function = gPciePortList[PciePortProfileInstance->PciPortNumber].Function;

    NbioSetDpc (PciPortAddr);

    PcieAerSetting.AerEnable = 1;
    PcieAerSetting.PciBus    = PciePortProfileInstance->NbioBusNum;
    PcieAerSetting.PciDev    = gPciePortList[PciePortProfileInstance->PciPortNumber].Device;
    PcieAerSetting.PciFunc   = gPciePortList[PciePortProfileInstance->PciPortNumber].Function;
    PcieAerSetting.CorrectableMask.Value   = mPlatformApeiPrivate->PlatRasPolicy.PcieAerCeMask;           // byo231031 -
    PcieAerSetting.UncorrectableMask.Value = mPlatformApeiPrivate->PlatRasPolicy.PcieAerUceMask;          // byo231031 -
    PcieAerSetting.UncorrectableMask.Field.UnsupportedRequestErrorMask = 1;
    PcieAerSetting.UncorrectableSeverity.Value = 0;

    HygonNbioPcieAerProtocol->SetPcieAerFeature (HygonNbioPcieAerProtocol, &PcieAerSetting);

    // PCI-E Aer firmware first setup
    PcieStsClr (PciPortAddr);
    gAerEn = TRUE;
    PcieActionCtrl (
      PciePortProfileInstance->NbioBusNum,
      PciePortProfileInstance->NbioIohubPhysicalDieNum,
      PciePortProfileInstance->NbioRbNum,
      PciePortProfileInstance->PciPhysicalPortNumber
      );
    EnAerErrorReport (PciPortAddr);

    EnDevErrReport (PciPortAddr);
  }

  return Status;
}

EFI_STATUS
NbifActionCtrl (
  UINT8    NbioBusNum,
  UINT8    IohubPhysicalDieId,
  UINT8    RbNum
  )
{
  UINT32    Index;
  PCI_ADDR  Nbif0Addr;
  PCI_ADDR  Nbif1Addr;
  PCI_ADDR  Nbif2Addr;
  PCI_ADDR  Nbif3Addr;
  UINT32    CpuModel;

  Nbif0Addr.AddressValue = MAKE_SBDFO (0, NbioBusNum, 7, 1, 0);
  Nbif1Addr.AddressValue = MAKE_SBDFO (0, NbioBusNum, 8, 1, 0);
  Nbif2Addr.AddressValue = MAKE_SBDFO (0, NbioBusNum, 8, 1, 0);
  Nbif3Addr.AddressValue = MAKE_SBDFO (0, NbioBusNum, 9, 1, 0);

  CpuModel = GetHygonSocModel();

  if (CpuModel == HYGON_EX_CPU) {
    if (RasPciIsDevicePresent (Nbif0Addr.AddressValue)) {
      for (Index = 0; Index < 8; Index++) {
        // NBIF0 Action Control setup
        SmnRegisterWrite ((UINT32)NbioBusNum, NBIO_SPACE2 (IohubPhysicalDieId, RbNum, NBIF0_ACTION_CONTROL_BASE_HYEX) + (Index * 4), &gNbifActConfigList[Index].Value, 0);
      }
    }
    if (RasPciIsDevicePresent (Nbif1Addr.AddressValue)) {
      for (Index = 0; Index < 8; Index++) {
        //NBIF1 Action Control setup
        SmnRegisterWrite ((UINT32)NbioBusNum, NBIO_SPACE2(IohubPhysicalDieId, RbNum, NBIF1_ACTION_CONTROL_BASE_HYEX) + (Index * 4), &gNbifActConfigList[Index].Value, 0);
      }
    }
  } else if (CpuModel == HYGON_GX_CPU) {
    if (RasPciIsDevicePresent (Nbif0Addr.AddressValue)) {
      for (Index = 0; Index < 8; Index++) {
        // NBIF0 Action Control setup
        SmnRegisterWrite ((UINT32)NbioBusNum, NBIO_SPACE2 (IohubPhysicalDieId, RbNum, NBIF0_ACTION_CONTROL_BASE_HYGX) + (Index * 4), &gNbifActConfigList[Index].Value, 0);
      }
    }
    if (RasPciIsDevicePresent (Nbif2Addr.AddressValue)) {
      for (Index = 0; Index < 8; Index++) {
        // NBIF2 Action Control setup
        SmnRegisterWrite ((UINT32)NbioBusNum, NBIO_SPACE2 (IohubPhysicalDieId, RbNum, NBIF2_ACTION_CONTROL_BASE_HYGX) + (Index * 4), &gNbifActConfigList[Index].Value, 0);
      }
    }

    if (RasPciIsDevicePresent (Nbif3Addr.AddressValue)) {
      for (Index = 0; Index < 8; Index++) {
        // NBIF3 Action Control setup
        SmnRegisterWrite ((UINT32)NbioBusNum, NBIO_SPACE2 (IohubPhysicalDieId, RbNum, NBIF3_ACTION_CONTROL_BASE_HYGX) + (Index * 4), &gNbifActConfigList[Index].Value, 0);
      }
    }
  }
  return EFI_SUCCESS;
}

EFI_STATUS
FstActionCtrl (
  VOID
  )
{
  EFI_STATUS Status;
  UINT32     Index;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  UINTN                                    NumberOfSockets;
  UINTN      NumberOfPhysicalDies;
  UINTN      NumberOfLogicalDies;
  UINTN      SocketId;
  UINTN      DieId;
  UINTN      LogicalDieId;
  UINTN      IohubPhysicalDieId;
  UINTN      DieType;
  UINTN      BusNumberBase;
 

  Status = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **)&FabricTopology);
  if (!EFI_ERROR (Status)) {
    FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL);
    for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
      FabricTopology->GetProcessorInfo (FabricTopology, SocketId, &NumberOfPhysicalDies, &NumberOfLogicalDies, NULL);
      for(DieId = 0; DieId < NumberOfPhysicalDies; DieId++) {
        FabricTopologyGetPhysicalIodDieInfo (DieId, &LogicalDieId, &DieType);

        if (DieType == IOD_EMEI) {
          IohubPhysicalDieId = FabricTopologyGetIohubPhysicalDieId (LogicalDieId);
          FabricTopology->GetRootBridgeInfo (FabricTopology, SocketId, LogicalDieId, 0, NULL, &BusNumberBase, NULL);
          for (Index = 0; Index < 4; Index++) {
            SmnRegisterWrite ((UINT32)BusNumberBase, NBIO_SPACE2 (IohubPhysicalDieId, 0, FST_ACTION_CONTROL_BASE_HYGX) + (Index * 4), &gFstActConfigList[Index].Value, 0);
          }
        }
      }
    }
  }
  return EFI_SUCCESS;
}

EFI_STATUS
NbifErrorConfig (
  VOID
  )
{
  EFI_STATUS                               Status;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  UINTN                                    NumberOfSockets;
  UINTN                                    NumberOfPhysicalDies;
  UINTN                                    NumberOfLogicalDies;
  UINTN                                    RootBridgesPerLogicalDie;
  UINTN                                    BusNumberBase;
  UINTN                                    SocketId;
  UINTN                                    LogicalDieId;
  UINTN                                    RbId;
  UINTN                                    IohubPhysicalDieId;

  DEBUG ((EFI_D_ERROR, "[RAS-DXE]Platform Nbif Enablement entry\n"));

  Status = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **)&FabricTopology);
  if (!EFI_ERROR (Status)) {
    FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL);

    for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
      FabricTopology->GetProcessorInfo (FabricTopology, SocketId, &NumberOfPhysicalDies, &NumberOfLogicalDies, NULL);

      for(LogicalDieId = 0; LogicalDieId < NumberOfLogicalDies; LogicalDieId++) {
        IohubPhysicalDieId = FabricTopologyGetIohubPhysicalDieId (LogicalDieId);

        FabricTopology->GetDieInfo (FabricTopology, SocketId, LogicalDieId, &RootBridgesPerLogicalDie, NULL, NULL);
        for (RbId = 0; RbId < RootBridgesPerLogicalDie; RbId++) {
          FabricTopology->GetRootBridgeInfo (FabricTopology, SocketId, LogicalDieId, RbId, NULL, &BusNumberBase, NULL);
          DEBUG ((EFI_D_ERROR, "[RAS-DXE]NBIF Action Control @ Socket %d, Logical Die %d, Rb %d, Bus 0x%x\n", SocketId, LogicalDieId, RbId, BusNumberBase));

          // NBIF Action Control setup
          NbifActionCtrl ((UINT8)BusNumberBase, (UINT8)IohubPhysicalDieId, (UINT8)RbId);
        }
      }
    }
  }

  return Status;
}

VOID
GetLocalApicId (
  OUT       UINT32 *LocalApicId
  )
{
  *LocalApicId = GetApicId ();
}

UINT32
RasGetApicId (
  EFI_MP_SERVICES_PROTOCOL  *MpServices,
  UINTN ProcessorNumber
  )
{
  UINT32  LocalApicId;

  if (ProcessorNumber == 0) {
    // This is BSP
    GetLocalApicId (&LocalApicId);
  } else {
    MpServices->StartupThisAP (
                  MpServices,
                  GetLocalApicId,
                  ProcessorNumber,
                  NULL,
                  0,
                  (VOID *)&LocalApicId,
                  NULL
                  );
  }

  return LocalApicId;
}

VOID
GetCpuId (
  RAS_CPUID_FN *RasCpuIdFn
  )
{
  AsmCpuid (
    RasCpuIdFn->FunctionId,
    &RasCpuIdFn->EAX_Reg,
    &RasCpuIdFn->EBX_Reg,
    &RasCpuIdFn->ECX_Reg,
    &RasCpuIdFn->EDX_Reg
    );
}

VOID
RasGetCpuIdInfo (
  EFI_MP_SERVICES_PROTOCOL  *MpServices,
  UINTN ProcessorNumber,
  RAS_CPUID_FN *RasCpuidFn
  )
{
  if (ProcessorNumber == 0) {
    // This is BSP
    GetCpuId (RasCpuidFn);
  } else {
    MpServices->StartupThisAP (
                  MpServices,
                  GetCpuId,
                  ProcessorNumber,
                  NULL,
                  0,
                  (VOID *)RasCpuidFn,
                  NULL
                  );
  }
}

VOID
GetSysEnterEip (
  UINT64 *SysEnterEip
  )
{
  UINT64  McgStatReg;

  McgStatReg = AsmReadMsr64 (MSR_MCG_STAT);

  if ((McgStatReg & MSR_MCG_EIPV) != 0) {
    *SysEnterEip = AsmReadMsr64 (MSR_SYSENTER_EIP);
  }
}

VOID
RasGetSysEnterEip (
  EFI_MP_SERVICES_PROTOCOL  *MpServices,
  UINTN ProcessorNumber,
  UINT64 *SysEnterEip
  )
{
  if (ProcessorNumber == 0) {
    // This is BSP
    GetSysEnterEip (SysEnterEip);
  } else {
    MpServices->StartupThisAP (
                  MpServices,
                  GetSysEnterEip,
                  ProcessorNumber,
                  NULL,
                  0,
                  (VOID *)SysEnterEip,
                  NULL
                  );
  }
}

EFI_STATUS
LogProcessorError (
  EFI_MP_SERVICES_PROTOCOL  *MpServices,
  RAS_MCA_ERROR_INFO        *RasMcaErrorInfo,
  UINT8                     BankIndex,
  UINTN                     ProcessorNumber
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  GENERIC_PROC_ERR_ENTRY  *GenericProcErrEntry;
  UINT32                  GenericProcErrEntrySize;
  EFI_GUID                ProcErrorSectGuid = PROCESSOR_SPECIFIC_X86_SECT_GUID;
  EFI_GUID                MsCheckGuid    = IA32_X64_MS_CHECK_FIELD_DESC_GUID;
  EFI_GUID                TlbCheckGuid   = IA32_X64_TLB_CHECK_STRUC_GUID;
  EFI_GUID                CacheCheckGuid = IA32_X64_CACHE_CHECK_STRUC_GUID;
  PROC_ERR_INFO_STRUC     *ProcErrInfoStruc;
  PROC_CONEXT_INFO_STRUC  *ProcContextInfoStruc;
  UINT32                  LocalApicId;
  RAS_CPUID_FN            RasCpuidFn;
  MS_CHECK_FIELD          MsCheckField;
  TLB_CHECK_STRUC         TlbCheckStruc;
  CACHE_CHECK_STRUC       CacheCheckStruc;
  UINT16                  McaErrorCode;
  MCA_TLB_ERROR           TlbError;
  MCA_MEM_ERROR           MemError;
  MCA_PP_ERROR            PpError;
  MCA_INT_ERROR           InternalError;
  UINT64                  SysEnterEip;

  // prepare structure
  GenericProcErrEntrySize = sizeof (GENERIC_PROC_ERR_ENTRY) + sizeof (PROC_ERR_INFO_STRUC) + sizeof (PROC_CONEXT_INFO_STRUC) + ((sizeof (UINT64)) * 10);
  DEBUG ((EFI_D_ERROR, "[RAS-DXE]Processor Error Section Size: 0x%x\n", GenericProcErrEntrySize));
  Status = gBS->AllocatePool (EfiBootServicesData, GenericProcErrEntrySize, &GenericProcErrEntry);
  ZeroMem (GenericProcErrEntry, GenericProcErrEntrySize);

  ProcErrInfoStruc     = (PROC_ERR_INFO_STRUC *)((UINTN)GenericProcErrEntry + sizeof (GENERIC_PROC_ERR_ENTRY));
  ProcContextInfoStruc = (PROC_CONEXT_INFO_STRUC *)((UINTN)ProcErrInfoStruc + sizeof (PROC_ERR_INFO_STRUC));

  DEBUG ((EFI_D_ERROR, "[RAS-DXE]GenericProcErrEntry Address: 0x%08x\n", GenericProcErrEntry));
  DEBUG ((EFI_D_ERROR, "[RAS-DXE]ProcErrInfoStruc Address: 0x%08x\n", ProcErrInfoStruc));
  DEBUG ((EFI_D_ERROR, "[RAS-DXE]ProcContextInfoStruc Address: 0x%08x\n", ProcContextInfoStruc));

  // Update generic data info
  gBS->CopyMem (GenericProcErrEntry, &gGenProcErrEntry, sizeof (GENERIC_PROC_ERR_ENTRY));
  GenericProcErrEntry->GenErrorDataEntry.ErrorDataLength = GenericProcErrEntrySize - sizeof (EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE);

  // Update Error section GUID
  gBS->CopyMem (&GenericProcErrEntry->GenErrorDataEntry.SectionType[0], &ProcErrorSectGuid, sizeof (EFI_GUID));

  // Checked Valid bits
  GenericProcErrEntry->ProcErrorSection.ValidBits.Field.CpuIdInfoValid          = 1;
  GenericProcErrEntry->ProcErrorSection.ValidBits.Field.LocalApicIDValid        = 1;
  GenericProcErrEntry->ProcErrorSection.ValidBits.Field.ProcErrInfoStrucNum     = 1;
  GenericProcErrEntry->ProcErrorSection.ValidBits.Field.ProcContextInfoStrucNum = 0;

  // Get LocalApicId
  LocalApicId = RasGetApicId (MpServices, ProcessorNumber);
  DEBUG ((EFI_D_ERROR, "[RAS-DXE]LocalApicId: 0x%x\n", LocalApicId));
  GenericProcErrEntry->ProcErrorSection.LocalApicID = (UINT64)LocalApicId;

  // Get CPUID Information
  RasCpuidFn.FunctionId = 0x00000001;  // Register In EAX
  RasGetCpuIdInfo (MpServices, ProcessorNumber, &RasCpuidFn);
  DEBUG ((EFI_D_ERROR, "[RAS-DXE]Logical Processor : 0x%x, EAX: 0x%08x, EBX: 0x%08x, ECX: 0x%08x, EDX: 0x%08x\n", ProcessorNumber, RasCpuidFn.EAX_Reg, RasCpuidFn.EBX_Reg, RasCpuidFn.ECX_Reg, RasCpuidFn.EDX_Reg));

  GenericProcErrEntry->ProcErrorSection.CpuIdInfo_EAX = RasCpuidFn.EAX_Reg;
  GenericProcErrEntry->ProcErrorSection.CpuIdInfo_EBX = RasCpuidFn.EBX_Reg;
  GenericProcErrEntry->ProcErrorSection.CpuIdInfo_ECX = RasCpuidFn.ECX_Reg;
  GenericProcErrEntry->ProcErrorSection.CpuIdInfo_EDX = RasCpuidFn.EDX_Reg;

  // Check Error code types
  McaErrorCode = (UINT16)RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCode;

  if (((McaErrorCode & TLB_ERROR_MASK) >> TLB_ERROR_CHK_SHIFT) == 1) {
    // TLB error
    TlbError.Value = McaErrorCode;
    TlbCheckStruc.Value = 0;

    TlbCheckStruc.Field.Level = TlbError.Field.CacheLevel;
    TlbCheckStruc.Field.TranscationType = TlbError.Field.TransactionType;

    TlbCheckStruc.Field.ValidationBits = TLB_CHECK_LEVEL | TLB_CHECK_TRANSTYPE;

    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC) {
      TlbCheckStruc.Field.Uncorrected     = 1;
      TlbCheckStruc.Field.ValidationBits |= TLB_CHECK_UNCORRECTED;
      if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.PCC == 0 && RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.TCC == 0) {
        TlbCheckStruc.Field.ValidationBits |= TLB_CHECK_PRECISE_IP | TLB_CHECK_RESTARTABLE;
        TlbCheckStruc.Field.PreciseIp     = 1;
        TlbCheckStruc.Field.RestartableIp = 1;
        ProcErrInfoStruc->ValidBits.Field.InstructionPointerValid = 1;
      } else if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.PCC == 1) {
        TlbCheckStruc.Field.ValidationBits |= TLB_CHECK_PROC_CONEXT_CORRUPT;
        TlbCheckStruc.Field.ProcessorContextCorrupt = 1;
      }
    }

    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Overflow) {
      TlbCheckStruc.Field.Overflow = 1;
      TlbCheckStruc.Field.ValidationBits |= TLB_CHECK_OVERFLOW;
    }

    // Report TLB check field description
    gBS->CopyMem (&ProcErrInfoStruc->ErrorStructureType[0], &TlbCheckGuid, sizeof (EFI_GUID));

    // Update Processor check information
    ProcErrInfoStruc->ValidBits.Field.CheckInfoValid = 1;
    ProcErrInfoStruc->CheckInformation = TlbCheckStruc.Value;
  } else if (((McaErrorCode & MEM_ERROR_MASK) >> MEM_ERROR_CHK_SHIFT) == 1) {
    // Memory error
    MemError.Value = McaErrorCode;
    CacheCheckStruc.Value = 0;

    CacheCheckStruc.Field.Level = MemError.Field.CacheLevel;
    CacheCheckStruc.Field.TranscationType = MemError.Field.TransactionType;
    CacheCheckStruc.Field.Operation = MemError.Field.MemTransactionType;

    CacheCheckStruc.Field.ValidationBits = CACHE_CHECK_LEVEL | CACHE_CHECK_TRANSTYPE | CACHE_CHECK_OPERATION;

    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC) {
      CacheCheckStruc.Field.Uncorrected     = 1;
      CacheCheckStruc.Field.ValidationBits |= CACHE_CHECK_UNCORRECTED;
      if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.PCC == 0 && RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.TCC == 0) {
        CacheCheckStruc.Field.ValidationBits |= CACHE_CHECK_PRECISE_IP | CACHE_CHECK_RESTARTABLE;
        CacheCheckStruc.Field.PreciseIp     = 1;
        CacheCheckStruc.Field.RestartableIp = 1;
        ProcErrInfoStruc->ValidBits.Field.InstructionPointerValid = 1;
      } else if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.PCC == 1) {
        CacheCheckStruc.Field.ValidationBits |= CACHE_CHECK_PROC_CONEXT_CORRUPT;
        CacheCheckStruc.Field.ProcessorContextCorrupt = 1;
      }
    }

    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Overflow) {
      CacheCheckStruc.Field.Overflow = 1;
      CacheCheckStruc.Field.ValidationBits |= CACHE_CHECK_OVERFLOW;
    }

    // Report Cache check field description
    gBS->CopyMem (&ProcErrInfoStruc->ErrorStructureType[0], &CacheCheckGuid, sizeof (EFI_GUID));

    // Update Processor check information
    ProcErrInfoStruc->ValidBits.Field.CheckInfoValid = 1;
    ProcErrInfoStruc->CheckInformation = CacheCheckStruc.Value;
  } else if (((McaErrorCode & INT_ERROR_MASK) >> INT_ERROR_CHK_SHIFT) == 1) {
    // Bus Error
    PpError.Value = McaErrorCode;
    MsCheckField.Value = 0;

    MsCheckField.Field.ErrorType = MSCHK_ERRTYPE_INTERNAL_UNCLASSIFIED;
    MsCheckField.Field.ValidationBits = MS_CHECK_ERRORTYPE;

    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC) {
      MsCheckField.Field.Uncorrected     = 1;
      MsCheckField.Field.ValidationBits |= MS_CHECK_UNCORRECTED;
      if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.PCC == 0 && RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.TCC == 0) {
        MsCheckField.Field.ValidationBits |= MS_CHECK_PRECISE_IP | MS_CHECK_RESTARTABLE;
        MsCheckField.Field.PreciseIp     = 1;
        MsCheckField.Field.RestartableIp = 1;
        ProcErrInfoStruc->ValidBits.Field.InstructionPointerValid = 1;
      } else if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.PCC == 1) {
        MsCheckField.Field.ValidationBits |= MS_CHECK_PROC_CONEXT_CORRUPT;
        MsCheckField.Field.ProcessorContextCorrupt = 1;
      }
    }

    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Overflow) {
      MsCheckField.Field.Overflow = 1;
      MsCheckField.Field.ValidationBits |= MS_CHECK_OVERFLOW;
    }

    // Report MS check field description
    gBS->CopyMem (&ProcErrInfoStruc->ErrorStructureType[0], &MsCheckGuid, sizeof (EFI_GUID));

    // Update Processor check information
    ProcErrInfoStruc->ValidBits.Field.CheckInfoValid = 1;
    ProcErrInfoStruc->CheckInformation = MsCheckField.Value;
  } else if (((McaErrorCode & PP_ERROR_MASK) >> PP_ERROR_CHK_SHIFT) == 1) {
    // Internal unclassified error
    InternalError.Value = McaErrorCode;

    MsCheckField.Value = 0;

    MsCheckField.Field.ErrorType = InternalError.Field.InternalErrorType;
    MsCheckField.Field.ValidationBits = MS_CHECK_ERRORTYPE;

    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC) {
      MsCheckField.Field.Uncorrected     = 1;
      MsCheckField.Field.ValidationBits |= MS_CHECK_UNCORRECTED;
      if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.PCC == 0 && RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.TCC == 0) {
        MsCheckField.Field.ValidationBits |= MS_CHECK_PRECISE_IP | MS_CHECK_RESTARTABLE;
        MsCheckField.Field.PreciseIp     = 1;
        MsCheckField.Field.RestartableIp = 1;
        ProcErrInfoStruc->ValidBits.Field.InstructionPointerValid = 1;
      } else if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.PCC == 1) {
        MsCheckField.Field.ValidationBits |= MS_CHECK_PROC_CONEXT_CORRUPT;
        MsCheckField.Field.ProcessorContextCorrupt = 1;
      }
    }

    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Overflow) {
      MsCheckField.Field.Overflow = 1;
      MsCheckField.Field.ValidationBits |= MS_CHECK_OVERFLOW;
    }

    // Report MS check field description
    gBS->CopyMem (&ProcErrInfoStruc->ErrorStructureType[0], &MsCheckGuid, sizeof (EFI_GUID));

    // Update Processor check information
    ProcErrInfoStruc->ValidBits.Field.CheckInfoValid = 1;
    ProcErrInfoStruc->CheckInformation = MsCheckField.Value;
  } else {
    // Unknown error
    gBS->FreePool (GenericProcErrEntry);
    return RETURN_ABORTED;
  }

  if (ProcErrInfoStruc->ValidBits.Field.InstructionPointerValid) {
    // Get SYSENTER_EIP
    RasGetSysEnterEip (MpServices, ProcessorNumber, &SysEnterEip);
    ProcErrInfoStruc->InstructionPointer = SysEnterEip;
  }

  // Update Processor Conext Information
  ProcContextInfoStruc->MSRAddress = MCA_LEGACY_BASE + (BankIndex * 4);  // Ude Legacy MCA address
  ProcContextInfoStruc->RegisterContextType = MSR_REGISTERS;
  ProcContextInfoStruc->RegisterArraySize   = (sizeof (UINT64)) * 10;
  ProcContextInfoStruc->RegisterArray[0]    = RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaBankNumber;
  gBS->CopyMem (&ProcContextInfoStruc->RegisterArray[1], &RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Value, ProcContextInfoStruc->RegisterArraySize - sizeof (UINT64));

  // Add a new record to the BERT table
  Status = HygonRasApeiProtocol->AddBootErrorRecordEntry (
                                   (UINT8 *)GenericProcErrEntry,                            // IN UINT8* pErrRecord
                                   GenericProcErrEntrySize,                                 // IN UINT nSize
                                   ERROR_TYPE_GENERIC,                                      // IN UINT8 ErrorType - GENERIC error type
                                   ERROR_SEVERITY_FATAL                                     // IN UINT8 SeverityType - NON-CORRECTABLE
                                   );

  gBS->FreePool (GenericProcErrEntry);

// byo231101 + >>
  if(RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC){
    SaveCoreUceApicId(LocalApicId);
  }
  SendElogEventMca((UINT8*)RasMcaErrorInfo, BankIndex, NULL);
// byo231101 + <<

  return Status;
}

EFI_STATUS
LogMemError (
  RAS_MCA_ERROR_INFO *RasMcaErrorInfo,
  UINT8 BankIndex
  )
{
  EFI_STATUS             Status = EFI_SUCCESS;
  UINT64                 SystemMemoryAddress;
  NORMALIZED_ADDRESS     NormalizedAddress;
  DIMM_INFO              DimmInfo;
  GENERIC_MEM_ERR_ENTRY  *GenericMemErrEntry;
  UINT8                  SeverityType;
  EFI_GUID               MemErrorSectGuid = PLATFORM_MEMORY_SECT_GUID;
  UINT32                 CpuModel;
  UINT8                  ChannelNumPerCdd;

  CpuModel = GetHygonSocModel();
  ChannelNumPerCdd = (CpuModel == HYGON_EX_CPU) ? MAX_CHANNELS_PER_CDD_HYEX : MAX_CHANNELS_PER_CDD_HYGX;

  NormalizedAddress.normalizedAddr         = RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaAddrMsr.Field.ErrorAddr;
  NormalizedAddress.normalizedSocketId     = RasMcaErrorInfo->CpuInfo.SocketId;
  NormalizedAddress.normalizedCddId        = RasMcaErrorInfo->CpuInfo.CddId;
  NormalizedAddress.normalizedChannelId    = UmcMcaInstanceIdSearch (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaIpidMsr) / 2;
  NormalizedAddress.normalizedSubChannelId = UmcMcaInstanceIdSearch (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaIpidMsr) % 2;
  HygonRasApeiProtocol->McaErrorAddrTranslate (&NormalizedAddress, &SystemMemoryAddress, &DimmInfo);

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (GENERIC_MEM_ERR_ENTRY), &GenericMemErrEntry);
  gBS->CopyMem (GenericMemErrEntry, &gGenMemErrEntry, sizeof (GENERIC_MEM_ERR_ENTRY));

  // Update Error section GUID
  gBS->CopyMem (&GenericMemErrEntry->GenErrorDataEntry.SectionType[0], &MemErrorSectGuid, sizeof (EFI_GUID));

  GenericMemErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;    // 0x01;
  GenericMemErrEntry->MemErrorSection.ValidBits.Value = MEM_VALID_BIT_MAP;
  GenericMemErrEntry->MemErrorSection.PhyAddr = SystemMemoryAddress;
  GenericMemErrEntry->MemErrorSection.Node    = (UINT16)((NormalizedAddress.normalizedSocketId * MAX_CDDS_PER_SOCKET) + NormalizedAddress.normalizedCddId);
  GenericMemErrEntry->MemErrorSection.Bank    = (UINT16)DimmInfo.Bank;
  GenericMemErrEntry->MemErrorSection.Row     = (UINT16)DimmInfo.Row;

  GenericMemErrEntry->MemErrorSection.Column = (UINT16)DimmInfo.Column;
  GenericMemErrEntry->MemErrorSection.Module = (UINT16)((NormalizedAddress.normalizedChannelId * ChannelNumPerCdd) + ((DimmInfo.ChipSelect & 0x03) >> 1));
  GenericMemErrEntry->MemErrorSection.Device = (UINT16)DimmInfo.ChipSelect;

  if (Status == EFI_NOT_FOUND) {
    AsciiStrCpyS (GenericMemErrEntry->GenErrorDataEntry.FruText, 20, "DIMM# Not Sourced");    // Error Fru Text String
  } else {
    AsciiStrCpyS (GenericMemErrEntry->GenErrorDataEntry.FruText, 20, "DIMM# Sourced");   // New Fru Text String
  }

  GenericMemErrEntry->MemErrorSection.ErrStatus = ((1 << 18) | (4 << 8));     // Error Detected on Data Transaction | Internal DRAM Error (0x40400)
  //
  GenericMemErrEntry->MemErrorSection.MemErrType = UNKNOWN;    // not a valid field.

  SeverityType = ERROR_SEVERITY_FATAL;
  if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.CECC) {
    SeverityType = ERROR_SEVERITY_CORRECTED;
  }

  GenericMemErrEntry->GenErrorDataEntry.ErrorSeverity = SeverityType;

// byo231101 + >>
  if(SeverityType == ERROR_SEVERITY_FATAL){
    SaveMemUceAddr(SystemMemoryAddress);
  }
  SendElogEventMca((UINT8*)RasMcaErrorInfo, BankIndex, &DimmInfo);
// byo231101 + <<

  // Add a new record to the BERT table
  Status = HygonRasApeiProtocol->AddBootErrorRecordEntry (
                                   (UINT8 *)GenericMemErrEntry,                             // IN UINT8* pErrRecord
                                   sizeof (GENERIC_MEM_ERR_ENTRY),                          // IN UINT nSize
                                   ERROR_TYPE_GENERIC,                                      // IN UINT8 ErrorType - GENERIC error type
                                   SeverityType                                             // IN UINT8 SeverityType
                                   );
  if (EFI_ERROR (Status)) {
    ASSERT (!EFI_ERROR (Status));
    gBS->FreePool (GenericMemErrEntry);
    return EFI_SUCCESS;
  }

  gBS->FreePool (GenericMemErrEntry);

  return Status;
}

EFI_STATUS
LogMemParityError (
  RAS_MCA_ERROR_INFO *RasMcaErrorInfo,
  UINT8 BankIndex
  )
{
  EFI_STATUS             Status = EFI_SUCCESS;
  GENERIC_MEM_ERR_ENTRY  *MemParityErrEntry;
  EFI_GUID               MemErrorSectGuid = PLATFORM_MEMORY_SECT_GUID;
  UMC_EXT_ERROR_CODE     UmcExtErrorCode;
  UINT16                 ChannelId;
  UINT16                 ChipSelect;
  UINT32                 CpuModel;
  UINT8                  ChannelNumPerCdd;

  CpuModel = GetHygonSocModel();
  ChannelNumPerCdd = (CpuModel == HYGON_EX_CPU) ? MAX_CHANNELS_PER_CDD_HYEX : MAX_CHANNELS_PER_CDD_HYGX;

  UmcExtErrorCode.Value = (UINT8)RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCodeExt;
  ChannelId = (UINT16)UmcMcaInstanceIdSearch (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaIpidMsr) / 2;

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (GENERIC_MEM_ERR_ENTRY), &MemParityErrEntry);
  gBS->CopyMem (MemParityErrEntry, &gGenMemParityErrEntry, sizeof (GENERIC_MEM_ERR_ENTRY));

  // Update Error section GUID
  gBS->CopyMem (&MemParityErrEntry->GenErrorDataEntry.SectionType[0], &MemErrorSectGuid, sizeof (EFI_GUID));

  MemParityErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;  // 0x01;
  MemParityErrEntry->MemErrorSection.ValidBits.Value = MEM_PARITY_VALID_BIT_MAP;
  MemParityErrEntry->MemErrorSection.Node = (UINT16)((RasMcaErrorInfo->CpuInfo.SocketId * 4) + RasMcaErrorInfo->CpuInfo.CddId);
  if (UmcExtErrorCode.Field.AddrCmdParityErr || UmcExtErrorCode.Field.WriteDataCrcErr) {
    ChipSelect = (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaAddrMsr.Field.ErrorAddr >> 32) & 0x03;
    MemParityErrEntry->MemErrorSection.ValidBits.Field.DeviceValid = 1;
    MemParityErrEntry->MemErrorSection.ValidBits.Field.ModuleValid = 1;

    MemParityErrEntry->MemErrorSection.Device = ChipSelect;
    MemParityErrEntry->MemErrorSection.Module = (ChannelId * ChannelNumPerCdd) + (ChipSelect >> 1);
    DEBUG ((EFI_D_ERROR, "[RAS-DXE]Memory CRC/Parity Error Found. Node: %d, UMC Channel: %d, Chip Select: %d\n", MemParityErrEntry->MemErrorSection.Node, ChannelId, ChipSelect));

    AsciiStrCpyS (MemParityErrEntry->GenErrorDataEntry.FruText, 20, "DIMM# Sourced"); // New Fru Text String
  } else {
    AsciiStrCpyS (MemParityErrEntry->GenErrorDataEntry.FruText, 20, "DIMM# Not Sourced");  // Error Fru Text String
  }

  MemParityErrEntry->MemErrorSection.ErrStatus = ((1 << 18) | (16 << 8));   // Error Detected on Data Transaction | ERR_BUS (0x41000)
  //
  MemParityErrEntry->MemErrorSection.MemErrType = PARITY_ERROR;

  SendElogEventMca((UINT8*)RasMcaErrorInfo, BankIndex, NULL);                               // byo231101 +

  // Add a new record to the BERT table
  Status = HygonRasApeiProtocol->AddBootErrorRecordEntry (
                                   (UINT8 *)MemParityErrEntry,                              // IN UINT8* pErrRecord
                                   sizeof (GENERIC_MEM_ERR_ENTRY),                          // IN UINT nSize
                                   ERROR_TYPE_GENERIC,                                      // IN UINT8 ErrorType - GENERIC error type
                                   ERROR_SEVERITY_FATAL                                     // IN UINT8 SeverityType - NON-CORRECTABLE
                                   );
  if (EFI_ERROR (Status)) {
    ASSERT (!EFI_ERROR (Status));
    gBS->FreePool (MemParityErrEntry);
    return EFI_SUCCESS;
  }

  gBS->FreePool (MemParityErrEntry);

  return Status;
}

EFI_STATUS
MemErrorDecode (
  EFI_MP_SERVICES_PROTOCOL  *MpServices,
  RAS_MCA_ERROR_INFO        *RasMcaErrorInfo,
  UINT8                     BankIndex,
  UINTN                     ProcessorNumber
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  UMC_EXT_ERROR_CODE  UmcExtErrorCode;

  UmcExtErrorCode.Value = (UINT8)RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCodeExt;
  // Add error record to BERT table.
  if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UECC || RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.CECC) {
    // Has Valid error address?
    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.AddrV) {
      LogMemError (RasMcaErrorInfo, BankIndex);
    } // End-- Error Address != 0?
  } else {
    // Decode
    if (UmcExtErrorCode.Field.WriteDataPoisonErr || UmcExtErrorCode.Field.ApbErr) {
      LogProcessorError (MpServices, RasMcaErrorInfo, BankIndex, ProcessorNumber);
    }

    if (UmcExtErrorCode.Field.AddrCmdParityErr || UmcExtErrorCode.Field.SdpParityErr || UmcExtErrorCode.Field.WriteDataCrcErr) {
      LogMemParityError (RasMcaErrorInfo, BankIndex);
    }
  }

  return Status;
}

EFI_STATUS
McaErrorDetection (
  VOID
  )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  RAS_MCA_ERROR_INFO        RasMcaErrorInfo;
  UINTN                     ProcessorNumber;
  EFI_MP_SERVICES_PROTOCOL  *MpServices = NULL;
  UINTN                     NumberOfLogicProcessors;
  UINTN                     NumberOfEnabledProcessors;
  UINT8                     BankNum;
  UINT8                     BankIndex;
  UINT64                    MsrData;
  MCA_STATUS_MSR            *McaSts;                    // byo231101 +
  MCA_MISC0_MSR             *McaMisc0;                  // byo231101 +

  // Get EFI MP service
  if (MpServices == NULL) {
    Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
    if (EFI_ERROR (Status)) {
      ASSERT (!EFI_ERROR (Status));
      return Status;    // Error detected while trying to locate MP Service protocol
    }
  }

  // Get StartThisApFrom, StartThisApTo, StartThisApSteps
  MpServices->GetNumberOfProcessors (MpServices, &NumberOfLogicProcessors, &NumberOfEnabledProcessors);

  MsrData = AsmReadMsr64 (MSR_MCG_CAP);            // MCG_CAP
  BankNum = (UINT8)(MsrData & 0xFF);
  for (ProcessorNumber = 0; ProcessorNumber < NumberOfLogicProcessors; ProcessorNumber++) {
    ZeroMem (&RasMcaErrorInfo, sizeof (RasMcaErrorInfo));
    RasMcaErrorInfo.CpuInfo.ProcessorNumber = ProcessorNumber;
    HygonRasApeiProtocol->SearchMcaError (&RasMcaErrorInfo);

    for (BankIndex = 0; BankIndex < BankNum; BankIndex++) {
      if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaIpidMsr.Field.HardwareID == 0) {
        continue;
      }

      // a Valid error return?
      if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val) {
        DEBUG ((EFI_D_ERROR, "[RAS-DXE]MCA Error Found in bank : 0x%x\n", RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaBankNumber));

// byo231101 + >>
        McaSts   = &RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr;
        McaMisc0 = &RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr;
        if(!McaSts->Field.UC && McaSts->Field.CECC && McaSts->Field.MiscV && !McaMisc0->Field.Ovrflw){
          DEBUG ((EFI_D_INFO, "Ignore CE without Threshold overflow.\n"));
          continue;
        }
// byo231101 + <<

        if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaIpidMsr.Field.HardwareID == MCA_UMC_ID) {
          MemErrorDecode (MpServices, &RasMcaErrorInfo, BankIndex, ProcessorNumber);
        } else {
          LogProcessorError (MpServices, &RasMcaErrorInfo, BankIndex, ProcessorNumber);
        }

        // Clear MCA Error status registers.
        McaStatusClear (MpServices, &RasMcaErrorInfo, BankIndex);
      }
    }
  }

  return Status;
}

VOID
McaStsClear (
  UINT32  *McaBank
  )
{
  UINT64         SaveHwcr;
  UINT64         MsrData;
  UINT32         McaBankAddrBase;
  MCA_MISC0_MSR  McaMisc0Msr;
  MCA_MISC1_MSR  McaMisc1Msr;
  MCA_IPID_MSR   McaIpidMsr;

  McaBankAddrBase = MCA_EXTENSION_BASE + (*McaBank * 0x10);

  // MSR_C001_0015[18][McStatusWrEn] = 1
  SaveHwcr = AsmReadMsr64 (MSR_HWCR);
  MsrData  = SaveHwcr | BIT18;
  AsmWriteMsr64 (MSR_HWCR, MsrData);

  AsmWriteMsr64 (McaBankAddrBase | MCA_ADDR_OFFSET, 0);
  AsmWriteMsr64 (McaBankAddrBase | MCA_SYND_OFFSET, 0);
  AsmWriteMsr64 (McaBankAddrBase | MCA_DEADDR_OFFSET, 0);
  AsmWriteMsr64 (McaBankAddrBase | MCA_DESTAT_OFFSET, 0);
  AsmWriteMsr64 (McaBankAddrBase | MCA_STATUS_OFFSET, 0);

  McaMisc0Msr.Value = AsmReadMsr64 (McaBankAddrBase | MCA_MISC0_OFFSET);
  McaMisc0Msr.Field.Ovrflw = 0;
  AsmWriteMsr64 (McaBankAddrBase | MCA_MISC0_OFFSET, McaMisc0Msr.Value);

  McaIpidMsr.Value = AsmReadMsr64 (McaBankAddrBase | MCA_IPID_OFFSET);
  if (McaIpidMsr.Field.HardwareID == MCA_UMC_ID) {
    McaMisc1Msr.Value = AsmReadMsr64 (McaBankAddrBase | MCA_MISC1_OFFSET);
    McaMisc1Msr.Field.Ovrflw = 0;
    AsmWriteMsr64 (McaBankAddrBase | MCA_MISC1_OFFSET, McaMisc1Msr.Value);
  }

  // Restore MSR_C001_0015[18][McStatusWrEn]
  AsmWriteMsr64 (MSR_HWCR, SaveHwcr);
}

EFI_STATUS
McaStatusClear (
  EFI_MP_SERVICES_PROTOCOL  *MpServices,
  RAS_MCA_ERROR_INFO        *RasMcaErrorInfo,
  UINT8                     BankIndex
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32      McaBank;
  UINTN       NumberOfEnabledProcessors;
  UINTN       NumberOfLogicProcessors;

  NumberOfLogicProcessors = 0;

  MpServices->GetNumberOfProcessors (MpServices, &NumberOfLogicProcessors, &NumberOfEnabledProcessors);

  if (RasMcaErrorInfo->CpuInfo.ProcessorNumber > NumberOfLogicProcessors) {
    return EFI_INVALID_PARAMETER;
  }

  McaBank = BankIndex;

  // Program BSP first
  if (RasMcaErrorInfo->CpuInfo.ProcessorNumber == 0) {
    McaStsClear (&McaBank);
  } else {
    MpServices->StartupThisAP (
                  MpServices,
                  McaStsClear,
                  RasMcaErrorInfo->CpuInfo.ProcessorNumber,
                  NULL,
                  0,
                  (VOID *)&McaBank,
                  NULL
                  );
  }

  return Status;
}

BOOLEAN
NbioSearchErr (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  )
{
  UINT32  Index;
  UINT32  NbioParityErrorSts;
  UINT32  BaseAddr;
  UINT32  MaxGroupId;
  UINT32  CpuModel;

  CpuModel   = GetHygonSocModel();
  MaxGroupId = 0;
  
  if (CpuModel == HYGON_EX_CPU) {
    switch (RasNbioErrorInfo->TypeId) {
    case NBIO_GRP_UNCORR:
      BaseAddr   = NBIO_SPACE2 (RasNbioErrorInfo->IohubPhysicalDie, RasNbioErrorInfo->Rb, PARITY_ERROR_STATUS_UNCORR_GRP_HYEX);
      MaxGroupId = MAX_UNCORR_GRP_NUM_HYEX;
      break;
    case NBIO_GRP_CORR:
      BaseAddr   = NBIO_SPACE2 (RasNbioErrorInfo->IohubPhysicalDie, RasNbioErrorInfo->Rb, PARITY_ERROR_STATUS_CORR_GRP_HYEX);
      MaxGroupId = MAX_CORR_GRP_NUM_HYEX;
      break;
    case NBIO_GRP_UCP:
      BaseAddr   = NBIO_SPACE2 (RasNbioErrorInfo->IohubPhysicalDie, RasNbioErrorInfo->Rb, PARITY_ERROR_STATUS_UCP_GRP_HYEX);
      MaxGroupId = MAX_UCP_GRP_NUM_HYEX;
      break;
    }
  } else if (CpuModel == HYGON_GX_CPU) {
    switch (RasNbioErrorInfo->TypeId) {
    case NBIO_GRP_UNCORR:
      BaseAddr   = NBIO_SPACE2 (RasNbioErrorInfo->IohubPhysicalDie, RasNbioErrorInfo->Rb, PARITY_ERROR_STATUS_UNCORR_GRP_HYGX);
      MaxGroupId = MAX_UNCORR_GRP_NUM_HYGX;
      break;
    case NBIO_GRP_CORR:
      BaseAddr   = NBIO_SPACE2 (RasNbioErrorInfo->IohubPhysicalDie, RasNbioErrorInfo->Rb, PARITY_ERROR_STATUS_CORR_GRP_HYGX);
      MaxGroupId = MAX_CORR_GRP_NUM_HYGX;
      break;
    case NBIO_GRP_UCP:
      BaseAddr   = NBIO_SPACE2 (RasNbioErrorInfo->IohubPhysicalDie, RasNbioErrorInfo->Rb, PARITY_ERROR_STATUS_UCP_GRP_HYGX);
      MaxGroupId = MAX_UCP_GRP_NUM_HYGX;
      break;
    }
  }

  for (Index = 0; Index < MaxGroupId; Index++) {
    SmnRegisterRead (
      (UINT32)RasNbioErrorInfo->BusNumber,
      BaseAddr + (Index * 4),
      &NbioParityErrorSts
      );
    if (NbioParityErrorSts != 0) {
      RasNbioErrorInfo->GroupId = (UINT8)Index;
      RasNbioErrorInfo->NbioParityErrorStsAddr = BaseAddr + (Index * 4);
      RasNbioErrorInfo->NbioParityErrorSts     = NbioParityErrorSts;
      return TRUE;
    }
  }

  return FALSE;
}

EFI_STATUS
NbioClrStatus (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  )
{
  SmnRegisterWrite (
    (UINT32)RasNbioErrorInfo->BusNumber,
    NBIO_SPACE2 (RasNbioErrorInfo->IohubPhysicalDie, RasNbioErrorInfo->Rb, RasNbioErrorInfo->NbioParityErrorStsAddr),
    &RasNbioErrorInfo->NbioParityErrorSts,
    0
    );

  return EFI_SUCCESS;
}

EFI_STATUS
NbioLogError (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  GENERIC_NBIO_ERR_ENTRY  *GenericNbioErrEntry;
  EFI_GUID                NbioErrorSectGuid = NBIO_ERROR_SECT_GUID;

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (GENERIC_NBIO_ERR_ENTRY), &GenericNbioErrEntry);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->CopyMem (GenericNbioErrEntry, &gGenNbioErrEntry, sizeof (GENERIC_NBIO_ERR_ENTRY));

  // Update Error section GUID
  gBS->CopyMem (&GenericNbioErrEntry->GenErrorDataEntry.SectionType[0], &NbioErrorSectGuid, sizeof (EFI_GUID));

  GenericNbioErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;
  GenericNbioErrEntry->NbioErrorSection.ValidationBits.Value = NBIO_VALID_BIT_MAP;
  GenericNbioErrEntry->NbioErrorSection.BusId = RasNbioErrorInfo->BusNumber;
  GenericNbioErrEntry->NbioErrorSection.ErrorSource.Field.Nbio = 1;
  GenericNbioErrEntry->NbioErrorSection.ErrorType.Value = (UINT8)(RasNbioErrorInfo->NbioGlobalStatusLo & 0x0000000F);
  GenericNbioErrEntry->NbioErrorSection.GroupId      = RasNbioErrorInfo->GroupId;
  GenericNbioErrEntry->NbioErrorSection.GroupType    = RasNbioErrorInfo->TypeId;
  GenericNbioErrEntry->NbioErrorSection.ParityErrSts = RasNbioErrorInfo->NbioParityErrorSts;

  // Add a new record to the BERT table
  Status = HygonRasApeiProtocol->AddBootErrorRecordEntry (
                                   (UINT8 *)GenericNbioErrEntry,                             // IN UINT8* pErrRecord
                                   sizeof (GENERIC_NBIO_ERR_ENTRY),                          // IN UINT nSize
                                   ERROR_TYPE_GENERIC,                                       // IN UINT8 ErrorType - GENERIC error type
                                   ERROR_SEVERITY_FATAL                                      // IN UINT8 SeverityType - NON-CORRECTABLE
                                   );

  gBS->FreePool (GenericNbioErrEntry);

  SendElogEventNbio((UINT8*)RasNbioErrorInfo);           // byo231101 +
  
  return Status;
}

EFI_STATUS
NbifLogError (
  UINT8         RbBusNum,
  UINT8         NbifMm,
  UINT8         Leaf,
  NBIO_ERR_SRC  ErrorSource,
  NBIO_ERR_TYPE ErrorType

  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  GENERIC_NBIO_ERR_ENTRY  *GenericNbioErrEntry;
  EFI_GUID                NbioErrorSectGuid = NBIO_ERROR_SECT_GUID;

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (GENERIC_NBIO_ERR_ENTRY), &GenericNbioErrEntry);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->CopyMem (GenericNbioErrEntry, &gGenNbioErrEntry, sizeof (GENERIC_NBIO_ERR_ENTRY));

  // Update Error section GUID
  gBS->CopyMem (&GenericNbioErrEntry->GenErrorDataEntry.SectionType[0], &NbioErrorSectGuid, sizeof (EFI_GUID));

  GenericNbioErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;
  GenericNbioErrEntry->NbioErrorSection.ValidationBits.Value = NBIF_VALID_BIT_MAP;
  GenericNbioErrEntry->NbioErrorSection.BusId = RbBusNum;
  GenericNbioErrEntry->NbioErrorSection.ErrorSource = ErrorSource;
  GenericNbioErrEntry->NbioErrorSection.ErrorType   = ErrorType;
  GenericNbioErrEntry->NbioErrorSection.NbifId = NbifMm;
  GenericNbioErrEntry->NbioErrorSection.LeafId = Leaf;

  // Add a new record to the NBIO BERT table
  Status = HygonRasApeiProtocol->AddBootErrorRecordEntry (
                                   (UINT8 *)GenericNbioErrEntry,                             // IN UINT8* pErrRecord
                                   sizeof (GENERIC_NBIO_ERR_ENTRY),                          // IN UINT nSize
                                   ERROR_TYPE_GENERIC,                                       // IN UINT8 ErrorType - GENERIC error type
                                   ERROR_SEVERITY_FATAL                                      // IN UINT8 SeverityType - NON-CORRECTABLE
                                   );

  gBS->FreePool (GenericNbioErrEntry);

  return Status;
}

EFI_STATUS
NbioErrorDetection (
  VOID
  )
{
  EFI_STATUS                               Status = EFI_SUCCESS;
  RAS_NBIO_ERROR_INFO                      RasNbioErrorInfo;
  UINT32                                   NbioGlobalStatusLo;
  UINT32                                   NbioGlobalStatusHi;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  UINTN                                    NumberOfSockets;
  UINTN                                    NumberOfPhysicalDies;
  UINTN                                    NumberOfLogicalDies;
  UINTN                                    RbNumberOfLogicalDie;
  UINTN                                    BusNumberBase;
  UINTN                                    SocketId;
  UINTN                                    LogicalDieId;
  UINTN                                    IohubPhysicalDieId;
  UINTN                                    RbId;
  UINT32                                   CpuModel;

  CpuModel = GetHygonSocModel();
  Status = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **)&FabricTopology);
  if (!EFI_ERROR (Status)) {
    FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL);

    for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
      FabricTopology->GetProcessorInfo (FabricTopology, SocketId, &NumberOfPhysicalDies, &NumberOfLogicalDies, NULL);
      for(LogicalDieId = 0; LogicalDieId < NumberOfLogicalDies; LogicalDieId++) {
        IohubPhysicalDieId = FabricTopologyGetIohubPhysicalDieId (LogicalDieId);

        FabricTopology->GetDieInfo (FabricTopology, SocketId, LogicalDieId, &RbNumberOfLogicalDie, NULL, NULL);
        for (RbId = 0; RbId < RbNumberOfLogicalDie; RbId++) {
          FabricTopology->GetRootBridgeInfo (FabricTopology, SocketId, LogicalDieId, RbId, NULL, &BusNumberBase, NULL);
          
          if (CpuModel == HYGON_EX_CPU) {
            // Check NBIO Global Error Status
            SmnRegisterRead ((UINT32)BusNumberBase, NBIO_SPACE2 (IohubPhysicalDieId, RbId, RAS_GLOBAL_STATUS_LO_HYEX), &NbioGlobalStatusLo);
            SmnRegisterRead ((UINT32)BusNumberBase, NBIO_SPACE2 (IohubPhysicalDieId, RbId, RAS_GLOBAL_STATUS_HI_HYEX), &NbioGlobalStatusHi);

            if (((NbioGlobalStatusLo & RAS_GLOBAL_STATUS_LO_MASK_HYEX) == 0) && ((NbioGlobalStatusHi & RAS_GLOBAL_STATUS_HI_MASK_HYEX) == 0)) {
              continue;
            }
          } else if(CpuModel == HYGON_GX_CPU) {
            // Check NBIO Global Error Status
            SmnRegisterRead ((UINT32)BusNumberBase, NBIO_SPACE2 (IohubPhysicalDieId, RbId, RAS_GLOBAL_STATUS_LO_HYGX), &NbioGlobalStatusLo);
            SmnRegisterRead ((UINT32)BusNumberBase, NBIO_SPACE2 (IohubPhysicalDieId, RbId, RAS_GLOBAL_STATUS_HI_HYGX), &NbioGlobalStatusHi);

            if (((NbioGlobalStatusLo & RAS_GLOBAL_STATUS_LO_MASK_HYGX) == 0) && ((NbioGlobalStatusHi & RAS_GLOBAL_STATUS_HI_MASK_HYGX) == 0)) {
              continue;
            }
          }
          // Error found!!
          RasNbioErrorInfo.NbioGlobalStatusLo = NbioGlobalStatusLo;
          RasNbioErrorInfo.NbioGlobalStatusHi = NbioGlobalStatusHi;
          RasNbioErrorInfo.Socket = (UINT8)SocketId;
          RasNbioErrorInfo.IohubPhysicalDie = (UINT8)IohubPhysicalDieId;
          RasNbioErrorInfo.Rb = (UINT8)RbId;
          RasNbioErrorInfo.BusNumber = (UINT8)BusNumberBase;

          NbioErrorScan (&RasNbioErrorInfo);
          if (CpuModel == HYGON_EX_CPU) {
            HyExNbifErrorScan (NbioGlobalStatusHi, (UINT8)BusNumberBase, (UINT8)RbId);
          } else if (CpuModel == HYGON_GX_CPU) {
            HyGxNbifErrorScan (NbioGlobalStatusHi, (UINT8)BusNumberBase, (UINT8)LogicalDieId, (UINT8)RbId);
			FstErrorScan (NbioGlobalStatusHi, (UINT8)SocketId, (UINT8)IohubPhysicalDieId, (UINT8)BusNumberBase);
          }
          //PcieErrorScan (NbioGlobalStatusHi, (UINT8)BusNumberBase);
        }
      }
    }
  }

  return Status;
}

EFI_STATUS
NbioErrorScan (
  RAS_NBIO_ERROR_INFO   *RasNbioErrorInfo
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32      CpuModel = GetHygonSocModel();

  // Scan Error Reporting Status by Type (UNCORR, CORR, UCP)
  // Right now only report first finding. make it loop for all errors.
  if ((RasNbioErrorInfo->NbioGlobalStatusLo & ((CpuModel==HYGON_EX_CPU)?RAS_GLOBAL_STATUS_LO_MASK_HYEX:RAS_GLOBAL_STATUS_LO_MASK_HYGX)) != 0) {
    RasNbioErrorInfo->TypeId = NBIO_GRP_UNCORR;
    if (NbioSearchErr (RasNbioErrorInfo)) {
      NbioLogError (RasNbioErrorInfo);
      NbioClrStatus (RasNbioErrorInfo);
      return Status;
    }

    RasNbioErrorInfo->TypeId = NBIO_GRP_CORR;
    if (NbioSearchErr (RasNbioErrorInfo)) {
      NbioLogError (RasNbioErrorInfo);
      NbioClrStatus (RasNbioErrorInfo);
      return Status;
    }

    RasNbioErrorInfo->TypeId = NBIO_GRP_UCP;
    if (NbioSearchErr (RasNbioErrorInfo)) {
      NbioLogError (RasNbioErrorInfo);
      NbioClrStatus (RasNbioErrorInfo);
      return Status;
    }
  }

  return Status;
}

EFI_STATUS
HyExNbifErrorScan (
  UINT32    NbioGlobalStatusHi,
  UINT8     RbBusNum,
  UINT8     RbNum
  )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  RAS_LEAF_CTRL_REG         RasLeafCtrl;
  RAS_GLOBAL_STS_HI_REG_HYEX RasGlobalSts;
  UINT8                     NbifMm;
  UINT8                     SysHubMm;
  UINT8                     Leaf;
  UINT32                    BaseAddr;
  NBIO_ERR_SRC              ErrorSource;
  NBIO_ERR_TYPE             ErrorType;

  RasGlobalSts.Value = NbioGlobalStatusHi;

  if (RasGlobalSts.Fields.Nbif0PortA || RasGlobalSts.Fields.Nbif1PortA) {
    DEBUG ((EFI_D_ERROR, "[RAS]NBIF error found!!!\n"));
    //Scan NBIFMM
    ErrorSource.Value = 0;
    ErrorType.Value = 0;
    for (NbifMm = 0; NbifMm < 2; NbifMm++) {
      BaseAddr = NBIO_SPACE2(0, RbNum, NBIF_BIF_RAS_LEAF0_CTRL_HYEX + (0x400000 * NbifMm));
      for (Leaf = 0; Leaf < 3; Leaf++) {
        SmnRegisterRead ((UINT32)RbBusNum,
                   BaseAddr + (Leaf * 4),
                   &RasLeafCtrl.Value);

        if (RasLeafCtrl.Fields.ErrEventSent && (RasLeafCtrl.Fields.ParityErrDet || RasLeafCtrl.Fields.PoisonErrDet)) {
          DEBUG ((EFI_D_ERROR, "[RAS]NBIF: %0d, LEAF: %0d\n", NbifMm, Leaf));
          //Log Error
          ErrorSource.Field.NbifMm = 1;
          if (RasLeafCtrl.Fields.ParityErrDet) {
            ErrorType.Field.ParityErrFatal = 1;
          } else {
            ErrorType.Field.PoisonError = 1;
          }

          NbifLogError (RbBusNum, NbifMm, Leaf, ErrorSource, ErrorType);
          //Clear Status
          SmnRegisterWrite ((UINT32)RbBusNum,
                     BaseAddr + (Leaf * 4),
                     &RasLeafCtrl.Value,
                     0);
        }
      }
    }

    ErrorSource.Value = 0;
    ErrorType.Value = 0;
    //Scan SYSHUBMM
    for (SysHubMm = 0; SysHubMm < 2; SysHubMm++) {
      BaseAddr = NBIO_SPACE2(0, RbNum, NBIF_GDC_RAS_LEAF0_CTRL_HYEX + (0x400000 * SysHubMm));
      for (Leaf = 0; Leaf < 6; Leaf++) {
        SmnRegisterRead ((UINT32)RbBusNum,
                   BaseAddr + (Leaf * 4),
                   &RasLeafCtrl.Value);
        if (RasLeafCtrl.Fields.ErrEventSent && (RasLeafCtrl.Fields.ParityErrDet || RasLeafCtrl.Fields.PoisonErrDet)) {
          //Log Error
          ErrorSource.Field.SyshubMm = 1;
          if (RasLeafCtrl.Fields.ParityErrDet) {
            ErrorType.Field.ParityErrFatal = 1;
          } else {
            ErrorType.Field.PoisonError = 1;
          }
          NbifLogError (RbBusNum, NbifMm, Leaf, ErrorSource, ErrorType);
          //Clear Status
          SmnRegisterWrite ((UINT32)RbBusNum,
                     BaseAddr + (Leaf * 4),
                     &RasLeafCtrl.Value,
                     0);
        }
      }
    }
  }

  return Status;
}

VOID
DetectAndLogNbifErr (
  UINT8     RbBusNum,
  UINT8     NbifMm,
  UINT32    RasLeafCtrlAddress,
  UINT32    GdcRasLeafCtrlAddress
  )
{
  RAS_LEAF_CTRL_REG  RasLeafCtrl;
  NBIO_ERR_SRC       ErrorSource;
  NBIO_ERR_TYPE      ErrorType;
  UINT8              Leaf;

  ErrorSource.Value = 0;
  ErrorType.Value   = 0;
  for (Leaf = 0; Leaf < 3; Leaf++) {
    SmnRegisterRead ((UINT32)RbBusNum, RasLeafCtrlAddress + (Leaf * 4), &RasLeafCtrl.Value);

    if (RasLeafCtrl.Fields.ErrEventSent && (RasLeafCtrl.Fields.ParityErrDet || RasLeafCtrl.Fields.PoisonErrDet)) {
      DEBUG ((EFI_D_ERROR, "[RAS-DXE]NBIF: %0d, LEAF: %0d\n", NbifMm, Leaf));
      // Log Error
      ErrorSource.Field.NbifMm = 1;
      if (RasLeafCtrl.Fields.ParityErrDet) {
        ErrorType.Field.ParityErrFatal = 1;
      } else {
        ErrorType.Field.PoisonError = 1;
      }

      NbifLogError (RbBusNum, NbifMm, Leaf, ErrorSource, ErrorType);
      // Clear Status
      SmnRegisterWrite ((UINT32)RbBusNum, RasLeafCtrlAddress + (Leaf * 4), &RasLeafCtrl.Value, 0);
    }
  }

  ErrorSource.Value = 0;
  ErrorType.Value   = 0;
  for (Leaf = 0; Leaf < 6; Leaf++) {
    SmnRegisterRead ((UINT32)RbBusNum, GdcRasLeafCtrlAddress + (Leaf * 4), &RasLeafCtrl.Value);
    if (RasLeafCtrl.Fields.ErrEventSent && (RasLeafCtrl.Fields.ParityErrDet || RasLeafCtrl.Fields.PoisonErrDet)) {
      // Log Error
      ErrorSource.Field.SyshubMm = 1;
      if (RasLeafCtrl.Fields.ParityErrDet) {
        ErrorType.Field.ParityErrFatal = 1;
      } else {
        ErrorType.Field.PoisonError = 1;
      }

      NbifLogError (RbBusNum, NbifMm, Leaf, ErrorSource, ErrorType);
      // Clear Status
      SmnRegisterWrite ((UINT32)RbBusNum, GdcRasLeafCtrlAddress + (Leaf * 4), &RasLeafCtrl.Value, 0);
    }
  }
}

EFI_STATUS
HyGxNbifErrorScan (
  UINT32    NbioGlobalStatusHi,
  UINT8     RbBusNum,
  UINT8     LogicalDieId,
  UINT8     RbNum
  )
{
  EFI_STATUS                 Status = EFI_SUCCESS;
  RAS_GLOBAL_STS_HI_REG_HYGX  RasGlobalSts;
  UINT8                      NbifMm;
  UINT8                      PhysicalDieId;
  UINT32                     RasLeafCtrlAddress;
  UINT32                     GdcRasLeafCtrlAddress;

  RasGlobalSts.Value = NbioGlobalStatusHi;

  if (RasGlobalSts.Fields.Nbif0PortA) {
    DEBUG ((EFI_D_ERROR, "[RAS-DXE]NBIF0 error found!!!\n"));

    NbifMm = 0;
    PhysicalDieId = (UINT8)FabricTopologyGetEmeiPhysicalDieId (LogicalDieId);
    RasLeafCtrlAddress    = NBIO_SPACE2 (PhysicalDieId, RbNum, NBIF0_BIF_RAS_LEAF0_CTRL_HYGX);
    GdcRasLeafCtrlAddress = NBIO_SPACE2 (PhysicalDieId, RbNum, NBIF0_GDC_RAS_LEAF0_CTRL_HYGX);

    DetectAndLogNbifErr (RbBusNum, NbifMm, RasLeafCtrlAddress, GdcRasLeafCtrlAddress);
  }

  if (RasGlobalSts.Fields.Nbif2PortA) {
    DEBUG ((EFI_D_ERROR, "[RAS-DXE]NBIF2 error found!!!\n"));

    NbifMm = 2;
    PhysicalDieId = (UINT8)FabricTopologyGetDjPhysicalDieId (LogicalDieId);
    RasLeafCtrlAddress    = NBIO_SPACE2 (PhysicalDieId, RbNum, NBIF2_BIF_RAS_LEAF0_CTRL_HYGX);
    GdcRasLeafCtrlAddress = NBIO_SPACE2 (PhysicalDieId, RbNum, NBIF2_GDC_RAS_LEAF0_CTRL_HYGX);

    DetectAndLogNbifErr (RbBusNum, NbifMm, RasLeafCtrlAddress, GdcRasLeafCtrlAddress);
  }

  if (RasGlobalSts.Fields.Nbif3PortA) {
    DEBUG ((EFI_D_ERROR, "[RAS-DXE]NBIF3 error found!!!\n"));

    NbifMm = 3;
    PhysicalDieId = (UINT8)FabricTopologyGetDjPhysicalDieId (LogicalDieId);
    RasLeafCtrlAddress    = NBIO_SPACE2 (PhysicalDieId, RbNum, NBIF3_BIF_RAS_LEAF0_CTRL_HYGX);
    GdcRasLeafCtrlAddress = NBIO_SPACE2 (PhysicalDieId, RbNum, NBIF3_GDC_RAS_LEAF0_CTRL_HYGX);

    DetectAndLogNbifErr (RbBusNum, NbifMm, RasLeafCtrlAddress, GdcRasLeafCtrlAddress);
  }

  return Status;
}

// byo231109 + >>
typedef struct {
  BOOLEAN           RootErrStatusSet;
  PCIE_PORT_PROFILE *PciePortProfile;
} PCIE_SCAN_DATA_BUFFER;
// byo231109 + <<

EFI_STATUS
PcieErrorLog (
  PCIE_PORT_PROFILE *PciePortProfile,
  IN UINT32         RbBusNum
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  PCI_ADDR      PciPortAddr;
  UINT16        AerCapPtr;
  UINT32        PcieUncorrStatus;
  UINT32        PcieUncorrMask;
  UINT32        PcieUncorrSeverity;
  UINT32        PcieCorrStatus;
  BOOLEAN       IsRootPortError = FALSE;
  BOOLEAN       RootErrStatusSet = FALSE;
  RAS_PCI_SCAN_DATA  ScanData;
  GENERIC_PCIE_AER_ERR_ENTRY  *GenPcieAerErrEntry;
  PCIE_ERROR_SECTION          *PcieErrorSection;
  UINT16                 PcieCeThreshold = mPlatformApeiPrivate->PlatRasPolicy.PcieCeThreshold;  // byo231109 +
  PCIE_SCAN_DATA_BUFFER  Buffer;                                                                 // byo231109 +
  BOOLEAN                NeedElog = TRUE;                                                        // byo231109 +

  PciPortAddr.AddressValue = 0;
  PciPortAddr.Address.Bus = PciePortProfile->NbioBusNum;
  PciPortAddr.Address.Device = gPciePortList[PciePortProfile->PciPortNumber].Device;
  PciPortAddr.Address.Function = gPciePortList[PciePortProfile->PciPortNumber].Function;

  AerCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, PCIE_EXT_AER_CAP_ID);

  if (RasPciErrCheck(PciPortAddr, &RootErrStatusSet)) {
    IsRootPortError = TRUE;
  }
  DEBUG ((EFI_D_ERROR, "[RAS] IsRootPortError = %d\n", IsRootPortError));
  if (!IsRootPortError && RootErrStatusSet) {
     PcieStsClr (PciPortAddr);
     //Scan and log device error
     Buffer.RootErrStatusSet = RootErrStatusSet;           // byo231109 +
     Buffer.PciePortProfile  = PciePortProfile;            // byo231109 +
     ScanData.Buffer = (void *)&Buffer;                    // byo231109 -
     ScanData.RasScanCallback = PcieDevErrScanCallback;
     RasPciScanSecondaryBus (PciPortAddr, &ScanData);
  }else{
     //Root Port Error
	 Status = gBS->AllocatePool (EfiBootServicesData, sizeof (GENERIC_PCIE_AER_ERR_ENTRY), &GenPcieAerErrEntry);
     if (EFI_ERROR (Status)) {
       return Status;
     }

     gBS->CopyMem (GenPcieAerErrEntry, &gGenPcieErrEntry, sizeof (GENERIC_PCIE_AER_ERR_ENTRY));

     //Update Error section GUID
     gBS->CopyMem (&GenPcieAerErrEntry->GenErrorDataEntry.SectionType[0], &gPcieErrorSectGuid, sizeof (EFI_GUID));   // byo231109 -
     PcieErrorSection = &GenPcieAerErrEntry->PcieAerErrorSection;
     
     if (AerCapPtr != 0) {
       //Un-Correctable error
       PcieUncorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR);
       PcieUncorrMask = PciRead32(PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_MASK_PTR);//hattie
       PcieUncorrSeverity = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_SEVERITY_PTR);//hattie-add
       if (0 != (PcieUncorrStatus & ~PcieUncorrMask)) {
         if (PcieUncorrStatus & PcieUncorrSeverity) {
           GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;
      	 }else {
           GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_RECOVERABLE;
      	 }
       } else {
         //Correctable error
      	 PcieCorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR);
      	 if (PcieCorrStatus != 0) {
           GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_CORRECTED;
// byo231109 + >>           
            if(PcieCeThreshold == 0){
              NeedElog = FALSE;
            } else {
              PciePortProfile->CeCount++;
              if(PciePortProfile->CeCount < PcieCeThreshold){
                NeedElog = FALSE;
              } else {
                PciePortProfile->CeCount = 0;
                NeedElog = TRUE;
              } 
            }
// byo231109 + <<			
      	 }
       }
	   FillPcieInfo(PciPortAddr, PcieErrorSection);
    }

    // Add a new record to the BERT table
    if(!NeedElog || mPlatformApeiPrivate->PlatRasPolicy.ApeiDisable){      // byo231109 +
    } else {                                                               // byo231109 +
      Status = HygonRasApeiProtocol->AddBootErrorRecordEntry ((UINT8*)GenPcieAerErrEntry,                       // IN UINT8* pErrRecord
                                                           sizeof (GENERIC_PCIE_AER_ERR_ENTRY),                 // IN UINT nSize
                                                           ERROR_TYPE_GENERIC,                                  // IN UINT8 ErrorType - GENERIC error type
                                                           GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity  // IN UINT8 SeverityType
                                                           );
      SendElogEventPcie((UINT8*)GenPcieAerErrEntry, PcieUncorrStatus, 0);  // byo231109 +
    }                                                                      // byo231109 +
    gBS->FreePool(GenPcieAerErrEntry);
  }
  if (IsRootPortError)
  {  
    PcieStsClr(PciPortAddr);
  } else {
    //Clear device error status
    ScanData.Buffer = NULL;
    ScanData.RasScanCallback = RasDevStsClr;
    RasPciScanSecondaryBus (PciPortAddr, &ScanData);
  }
  return Status;
}

EFI_STATUS
PcieErrorScan (
  UINT32 NbioGlobalStatusHi,
  UINT8  RbBusNum
  )
{
  EFI_STATUS         Status = EFI_SUCCESS;
  UINT32             PcieGlobalStatus;
  UINT16             PciePortIndex;
  PCIE_PORT_PROFILE  *PciePortProfileInstance;

  PcieGlobalStatus = NbioGlobalStatusHi & 0x07F8FFFF;

  PciePortProfileInstance = mPlatformApeiPrivate->HygonPciePortMap->PciPortNumber;

  // Search active PCI-E port for error only
  for (PciePortIndex = 0; PciePortIndex < mPlatformApeiPrivate->HygonPciePortMap->PortCount; PciePortIndex++, PciePortProfileInstance++) {
    if (PciePortProfileInstance->NbioBusNum != RbBusNum) {
      // find next
      continue;
    }

    if ((PcieGlobalStatus & (1 << PciePortProfileInstance->PciPhysicalPortNumber)) != 0) {
      DEBUG ((EFI_D_ERROR, "PCI-E Error detected at Bus: 0x%x, physical Port: %0d\n", RbBusNum, PciePortProfileInstance->PciPhysicalPortNumber));
      // PCI-E error detected
      PcieErrorLog (PciePortProfileInstance, RbBusNum);
    }
  }

  return Status;
}

EFI_STATUS
SmnErrorDetection (
  VOID
  )
{
  EFI_STATUS                               Status;
  GENERIC_SMN_ERR_ENTRY                    *GenericSmnErrEntry;
  UINT32                                   PmBreakEvent;
  UINT32                                   PmBreakEvent_Raw;
  UINTN                                    Index;
  EFI_GUID                                 SmnErrorSectGuid = SMN_ERROR_SECT_GUID;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  UINTN                                    NumberOfSockets;
  UINTN                                    BusNumberBase;
  UINTN                                    SocketId;
  UINT32                                   CpuModel;
  
  CpuModel = GetHygonSocModel();
  
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (GENERIC_SMN_ERR_ENTRY),
                  &GenericSmnErrEntry
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **)&FabricTopology);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL);

  for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
    FabricTopology->GetRootBridgeInfo (FabricTopology, SocketId, 0, 0, NULL, &BusNumberBase, NULL);

    // Check SMN PM BreakEvent Status
    SmnRegisterRead ((UINT32)BusNumberBase, IOD_SPACE (0, (CpuModel==HYGON_EX_CPU)?PMIO_SMN_BREAK_EVENT_HYEX:PMIO_SMN_BREAK_EVENT_HYGX), &PmBreakEvent);
    PmBreakEvent_Raw = PmBreakEvent;

    // Keep only SMN related bits
    PmBreakEvent &= SMN_CATEGORY_MASK;
    if (PmBreakEvent == 0) {
      continue;
    }

    // Error found!!
    for (Index = 0; Index <= (UINTN)HighBitSet32 (SMN_CATEGORY_MASK); Index++) {
      if ((PmBreakEvent & (1 << Index)) != 0) {
        gBS->CopyMem (GenericSmnErrEntry, &gGenSmnErrEntry, sizeof (GENERIC_SMN_ERR_ENTRY));

        // Update Error section GUID
        gBS->CopyMem (&GenericSmnErrEntry->GenErrorDataEntry.SectionType[0], &SmnErrorSectGuid, sizeof (EFI_GUID));

        GenericSmnErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;
        GenericSmnErrEntry->SmnErrorSection.ValidationBits.Value = SMN_VALID_BIT_MAP;
        GenericSmnErrEntry->SmnErrorSection.BusId    = (UINT8)BusNumberBase;
        GenericSmnErrEntry->SmnErrorSection.Category = SMN_CATEGORY_FATAL;
        GenericSmnErrEntry->SmnErrorSection.ErrorSource.Value = (UINT8)(PmBreakEvent & (1 << Index));

        // Add a new record to the SMN BERT table
        Status = HygonRasApeiProtocol->AddBootErrorRecordEntry (
                                         (UINT8 *)GenericSmnErrEntry,  // IN UINT8* pErrRecord
                                         sizeof (GENERIC_SMN_ERR_ENTRY), // IN UINT nSize
                                         ERROR_TYPE_GENERIC,           // IN UINT8 ErrorType - GENERIC error type
                                         ERROR_SEVERITY_FATAL          // IN UINT8 SeverityType - NON-CORRECTABLE
                                         );
      }
    }

    // Clear any SMN sources in PM BreakEvent register
    PmBreakEvent_Raw &= ~SMN_CATEGORY_MASK;
    SmnRegisterWrite ((UINT32)BusNumberBase, IOD_SPACE (0, (CpuModel==HYGON_EX_CPU)?PMIO_SMN_BREAK_EVENT_HYEX:PMIO_SMN_BREAK_EVENT_HYGX), &PmBreakEvent_Raw, 0);
  }

  gBS->FreePool (GenericSmnErrEntry);

  return Status;
}

EFI_STATUS
FstErrorScan (
  UINT32 NbioGlobalStatusHi,
  UINT8  SocketId,
  UINT8  IohubPhysicalDieId,
  UINT8  RbBusNum
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  RAS_GLOBAL_STS_HI_REG_HYGX  RasGlobalSts;
  UINT32      FstLinkStatusReg;
  UINT32      FstLinkStatusValue;


  RasGlobalSts.Value = NbioGlobalStatusHi;
  // Check Error Reporting Status by Type (UNCORR, CORR)
  if (RasGlobalSts.Fields.Fstus) {
    FstLinkStatusReg = NBIO_SPACE2 (IohubPhysicalDieId, RbBusNum, FST_LINK_STATUS_HYGX);
    SmnRegisterRead(RbBusNum, FstLinkStatusReg, &FstLinkStatusValue);

    if (FstLinkStatusValue & 0x1) {
      DEBUG ((EFI_D_ERROR, "[RAS-DXE]Socket %d IOD %d NBIO 0 FST US Correctable error found!!!\n", SocketId, IohubPhysicalDieId));
    } else if (FstLinkStatusValue & 0x2) {
      DEBUG ((EFI_D_ERROR, "[RAS-DXE]Socket %d IOD %d NBIO 0 FST US UnCorrectable error found!!!\n", SocketId, IohubPhysicalDieId));
    }

    // Clear FSTUS Link status
    SmnRegisterWrite(RbBusNum, FstLinkStatusReg, &FstLinkStatusValue, 0);
  }

  if (RasGlobalSts.Fields.Fstds) {
    FstLinkStatusReg = NBIO_SPACE2 (0, RbBusNum, FST_LINK_STATUS_HYGX);
    SmnRegisterRead(RbBusNum, FstLinkStatusReg, &FstLinkStatusValue);

    if (FstLinkStatusValue & 0x1) {
      DEBUG ((EFI_D_ERROR, "[RAS-DXE]Socket %d IOD 0 NBIO 0 FST DS Correctable error found!!!\n", SocketId));
    } else if (FstLinkStatusValue & 0x2) {
      DEBUG ((EFI_D_ERROR, "[RAS-DXE]Socket %d IOD 0 NBIO 0 FST DS UnCorrectable error found!!!\n", SocketId));
    }
    // Clear FSTUS Link status
    SmnRegisterWrite(RbBusNum, FstLinkStatusReg, &FstLinkStatusValue, 0);
  }
  
  return Status;
}

SCAN_STATUS
STATIC
RasDevStsClr (
  IN       PCI_ADDR             Device,
  IN OUT   RAS_PCI_SCAN_DATA    *ScanData
  )
{
    SCAN_STATUS             ScanStatus;
    PCIE_DEVICE_TYPE        DeviceType;
    ScanStatus = SCAN_SUCCESS;
    DEBUG ((EFI_D_ERROR, "[RAS] RasDevStsClr for Device = %d:%d:%d\n",
      Device.Address.Bus,
      Device.Address.Device,
      Device.Address.Function
      ));
    ScanStatus = SCAN_SUCCESS;
    DeviceType = RasGetPcieDeviceType (Device);
    DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device type = 0x%x\n", DeviceType));
    switch (DeviceType) {
    case  PcieDeviceRootComplex:
    case  PcieDeviceDownstreamPort:
    case  PcieDeviceUpstreamPort:
      DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device root port found\n"));
      ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
      break;
    case  PcieDevicePcieToPcix:
      DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device PCIE to PCIx found\n"));
      ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
      break;
    case  PcieDeviceEndPoint:
    case  PcieDeviceLegacyEndPoint:
      DEBUG ((EFI_D_ERROR, "[RAS] PCI-E endpoint found\n"));
      PcieStsClr (Device);
      ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
      break;
    default:
      break;
    }
    return ScanStatus;
}

VOID
FillPcieInfo (
  PCI_ADDR           PciPortAddr,
  PCIE_ERROR_SECTION *PcieErrorSection  
  )
{
    UINT32              Index;
    UINT8               PcieCapPtr;
    UINT16              AerCapPtr;
    PCIE_DEVICE_TYPE    PortType;
    
    AerCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, PCIE_EXT_AER_CAP_ID);
    PcieCapPtr = RasFindPciCapability (PciPortAddr.AddressValue, PCIE_CAP_ID);
    
    PortType = RasGetPcieDeviceType (PciPortAddr);
    PcieErrorSection->Validation.Value = 0xEF;
    PcieErrorSection->PortType = (UINT32)PortType;
    PcieErrorSection->Revision = 0x02;
    PcieErrorSection->CommandStatus = PciRead32 (PciPortAddr.AddressValue + 0x04);

    PcieErrorSection->DeviceId.VendorId = PciRead16 (PciPortAddr.AddressValue + 0x00);
    PcieErrorSection->DeviceId.DeviceId = PciRead16 (PciPortAddr.AddressValue + 0x02);
    PcieErrorSection->DeviceId.ClassCode[0] = PciRead8 (PciPortAddr.AddressValue + 0x09);
    PcieErrorSection->DeviceId.ClassCode[1] = PciRead8 (PciPortAddr.AddressValue + 0x0A);
    PcieErrorSection->DeviceId.ClassCode[2] = PciRead8 (PciPortAddr.AddressValue + 0x0B);
    PcieErrorSection->DeviceId.Function = (UINT8)PciPortAddr.Address.Function;
    PcieErrorSection->DeviceId.Device = (UINT8)PciPortAddr.Address.Device;
    PcieErrorSection->DeviceId.Segment = (UINT8)PciPortAddr.Address.Segment;
    PcieErrorSection->DeviceId.PrimaryBus = (UINT8)PciPortAddr.Address.Bus;
    PcieErrorSection->DeviceId.SecondaryBus =  \
            ((PortType <= PcieDeviceLegacyEndPoint))? 0x00:PciRead8 (PciPortAddr.AddressValue + 0x19);
    
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.Function : 0x%x\n", PcieErrorSection->DeviceId.Function));
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.Device : 0x%x\n", PcieErrorSection->DeviceId.Device));
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.Segment : 0x%x\n", PcieErrorSection->DeviceId.Segment));
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.PrimaryBus : 0x%x\n", PcieErrorSection->DeviceId.PrimaryBus));
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.SecondaryBus : 0x%x\n", PcieErrorSection->DeviceId.SecondaryBus));
    
    if (PcieCapPtr !=0) {
      PcieErrorSection->DeviceId.Slot = (UINT8)(PciRead32(PciPortAddr.AddressValue + PcieCapPtr + 0x14) >> 19);
      PcieErrorSection->BridgeCtrlStatus = (UINT32) (PciRead16 (PciPortAddr.AddressValue + 0x3E)) << 16 | (UINT32) (PciRead16 (PciPortAddr.AddressValue + 0x1E));

      for (Index = 0; Index < 15; Index++) {
        PcieErrorSection->CapabilityStructure.CapabilityData[Index] = \
                                            PciRead32 (PciPortAddr.AddressValue + (0x58 + (4 * Index)));
      }
    }
    
    if (AerCapPtr !=0) {
      for (Index = 0; Index < 24; Index++) {
        PcieErrorSection->AerInfo.AerInfoData[Index] = PciRead32 (PciPortAddr.AddressValue + (AerCapPtr + (4 * Index)));
      }
    }
}

VOID
STATIC
PcieDevErrScanOnFunction (
  PCI_ADDR       Function,
  VOID           *pBuffer                      // byo231109 -
  )
{
  EFI_STATUS  Status;
  UINT8       PcieCapPtr;
  UINT16      AerCapPtr;
  UINT32      PcieUncorrStatus;
  UINT32      PcieUncorrMask;
  UINT32      PcieUncorrSverity;
  BOOLEAN     PcieCorrErrorValid;

  GENERIC_PCIE_AER_ERR_ENTRY                *GenPcieAerErrEntry;
  PCIE_ERROR_SECTION                        *PcieErrorSection;
  PCIE_SCAN_DATA_BUFFER                     *Buffer = (PCIE_SCAN_DATA_BUFFER*)pBuffer;                              // byo231109 +
  UINT16                                    PcieCeThreshold = mPlatformApeiPrivate->PlatRasPolicy.PcieCeThreshold;  // byo231109 +
  PCIE_PORT_PROFILE                         *PciePortProfile = Buffer->PciePortProfile;                             // byo231109 +
  BOOLEAN                                   NeedElog = TRUE;                                                        // byo231109 +
  
//EFI_GUID    PcieErrSectionType = PCIE_SECT_GUID;                                                                  // byo231109 -
  
  PcieCapPtr = RasFindPciCapability (Function.AddressValue, PCIE_CAP_ID);
  AerCapPtr = RasFindPcieExtendedCapability (Function.AddressValue, PCIE_EXT_AER_CAP_ID);

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (GENERIC_PCIE_AER_ERR_ENTRY), &GenPcieAerErrEntry);
  if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "[RAS] AllocatePool failed!!!\n"));
  }

  gBS->CopyMem (GenPcieAerErrEntry, &gGenPcieErrEntry, sizeof (GENERIC_PCIE_AER_ERR_ENTRY));

  //Update Error section GUID
  gBS->CopyMem (&GenPcieAerErrEntry->GenErrorDataEntry.SectionType[0], &gPcieErrorSectGuid, sizeof (EFI_GUID));     // byo231109 -
  PcieErrorSection = &GenPcieAerErrEntry->PcieAerErrorSection;
  
  if (RasPciErrCheck(Function, &Buffer->RootErrStatusSet)) {                                                        // byo231109 -
    if (AerCapPtr != 0) {
      PcieUncorrStatus = PciRead32 (Function.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR);
      PcieUncorrMask = PciRead32(Function.AddressValue + AerCapPtr + PCIE_UNCORR_MASK_PTR);
      PcieUncorrSverity = PciRead32 (Function.AddressValue + AerCapPtr + PCIE_UNCORR_SEVERITY_PTR);
      if (0 != (PcieUncorrStatus & ~PcieUncorrMask)) {
        if (PcieUncorrStatus & PcieUncorrSverity) {
          GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;
        }else {
          GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_RECOVERABLE;
        }
      } else {
        PcieCorrErrorValid = PcieAerCorrErrCheck (Function, AerCapPtr);
        if (PcieCorrErrorValid) {
          GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_CORRECTED;
// byo231109 + >>
          if(PcieCeThreshold == 0){
            NeedElog = FALSE;
          } else {
            PciePortProfile->CeCount++;
            if(PciePortProfile->CeCount < PcieCeThreshold){
              NeedElog = FALSE;
            } else {
              PciePortProfile->CeCount = 0;
              NeedElog = TRUE;
            } 
          }
// byo231109 + <<		  
        }
      }
      FillPcieInfo(Function, PcieErrorSection);
    }
    // Add a new record to the BERT table
    if(!NeedElog || mPlatformApeiPrivate->PlatRasPolicy.ApeiDisable){     // byo231109 +
    } else {                                                              // byo231109 +
      Status = HygonRasApeiProtocol->AddBootErrorRecordEntry ((UINT8*)GenPcieAerErrEntry,          // IN UINT8* pErrRecord
                                                           sizeof (GENERIC_PCIE_AER_ERR_ENTRY),       // IN UINT nSize
                                                           ERROR_TYPE_GENERIC,                   // IN UINT8 ErrorType - GENERIC error type
                                                           GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity                  // IN UINT8 SeverityType
                                                           );
      SendElogEventPcie((UINT8*)GenPcieAerErrEntry, PcieUncorrStatus, 0); // byo231109 +
    }                                                                     // byo231109 +
    gBS->FreePool(GenPcieAerErrEntry);
  }
}

VOID
STATIC
PcieDevErrScanOnDevice (
  PCI_ADDR       Device,
  VOID           *Buffer                                                  // byo231109 -
  )
{
  UINT8          MaxFunc;
  UINT8          CurrentFunc;

  MaxFunc = RasPciIsMultiFunctionDevice (Device.AddressValue) ? 7 : 0;
  for (CurrentFunc = 0; CurrentFunc <= MaxFunc; CurrentFunc++) {
    Device.Address.Function = CurrentFunc;
    DEBUG ((EFI_D_ERROR, "[RAS] Checking Device: %d:%d:%d\n",
            Device.Address.Bus,
            Device.Address.Device,
            Device.Address.Function
            ));
    if (RasPciIsDevicePresent (Device.AddressValue)) {
      DEBUG ((EFI_D_ERROR, "[RAS] Scan Device Error report for Device = 0x%x:0x%x:0x%x\n",
      Device.Address.Bus,
      Device.Address.Device,
      Device.Address.Function
      ));
      PcieDevErrScanOnFunction (Device, Buffer);                          // byo231109 -
    }else
        //if find no function on the device, will stop scan
        break;
  }
}

SCAN_STATUS
STATIC
PcieDevErrScanCallback (
  PCI_ADDR             Device,
  RAS_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS             ScanStatus;
  PCIE_DEVICE_TYPE        DeviceType;
//BOOLEAN                 RootStatusSet;                                  // byo231109 -

  ScanStatus = SCAN_SUCCESS;
  DEBUG ((EFI_D_ERROR, "[RAS] PcieDevErrScanCallback for Device = %d:%d:%d\n",
          Device.Address.Bus,
          Device.Address.Device,
          Device.Address.Function
      ));
  ScanStatus = SCAN_SUCCESS;
  DeviceType = RasGetPcieDeviceType (Device);
  DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device type = 0x%x\n", DeviceType));
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device root port found\n"));
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  case  PcieDevicePcieToPcix:
    DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device PCIE to PCIx found\n"));
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    DEBUG ((EFI_D_ERROR, "[RAS] PCI-E endpoint found\n"));
//  RootStatusSet = *(BOOLEAN *)ScanData->Buffer;                          // byo231109 -
//  DEBUG ((EFI_D_ERROR, "[RAS] RootStatusSet = %x\n",RootStatusSet));     // byo231109 -
    PcieDevErrScanOnDevice (Device, ScanData->Buffer);                     // byo231109 -
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  default:
    break;
  }
  return ScanStatus;
}
