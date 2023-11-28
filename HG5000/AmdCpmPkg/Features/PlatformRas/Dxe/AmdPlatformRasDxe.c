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
#include <Protocol/MpService.h>
#include <Protocol/AmdRasApeiProtocol.h>
#include <Protocol/AmdCoreTopologyProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/AmdNbioPcieAerProtocol.h>
#include "AmdApeiErst.h"
#include "AmdApeiEinj.h"
#include "AmdApeiHest.h"
#include "AmdApeiBert.h"
#include "AmdPlatformRasDxe.h"

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
AMD_RAS_APEI_PROTOCOL         *AmdRasApeiProtocol;
BOOLEAN gAerEn;
BOOLEAN gDpcEn;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
PlatformConfigInit ( VOID );

EFI_STATUS
McaErrorDetection ( VOID );

EFI_STATUS
NbioHestUpdate ( VOID );

EFI_STATUS
NbioErrorDetection ( VOID );

EFI_STATUS
SmnErrorDetection ( VOID );

EFI_STATUS
PciePortDetect ( VOID );

EFI_STATUS
PcieAerHestUpdate ( VOID );

EFI_STATUS
PcieAerErrorConfig ( VOID );

EFI_STATUS
NbifErrorConfig ( VOID );

EFI_STATUS
NbioErrorScan (
  RAS_NBIO_ERROR_INFO   *RasNbioErrorInfo
  );

EFI_STATUS
NbifErrorScan (
  UINT32    NbioGlobalStatusHi,
  UINT8     DieBusNum
  );

EFI_STATUS
PcieErrorScan (
  UINT32    NbioGlobalStatusHi,
  UINT8     DieBusNum
  );

EFI_STATUS
McaStatusClear (
  EFI_MP_SERVICES_PROTOCOL  *MpServices,
  RAS_MCA_ERROR_INFO        *RasMcaErrorInfo,
  UINT8                     BankIndex
  );

VOID
RasLateBootInit(
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
extern  EFI_BOOT_SERVICES       *gBS;

/*----------------------------------------------------------------------------------------
 *                          T A B L E    D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */
// Generic Processor Error entries
GENERIC_PROC_ERR_ENTRY gGenProcErrEntry = {
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                            // UEFI 2.6 Section N 2.5 Processor Specific Error Section Type 16-byte GUID, update in runtime.
    ERROR_SEVERITY_FATAL,                                         // UINT32 ACPI 6.1 Table 18-343 Generic Error Data Entry
    GENERIC_ERROR_REVISION,                                       // UINT16 UEFI 2.6 Section N Revision Field
    FRU_STRING_VALID,                                             // UINT8 UEFI 2.6 Section N Section Descriptor Validation Bits field
    0x1,                                                          // UINT8 UEFI 2.6 Section N Section Descriptor Flags
    0,                                                            // UINT32 Generic Error Data Block Length, update in runtime
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                            // UINT8[16] FRU ID - UEFI 2.6 Section N
    {'P','r','o','c','e','s','s','o','r','E','r','r','o','r'}     // UINT8[20] FRU Text[20] - UEFI 2.6 Section N
  },
  {
    {0},                                                          // UEFI 2.6 Section N Table 251 -Processor Specific Error Type Valid bit
    0,                                                            // Processor APIC ID 
    0                                                             // CPUID Information 
  }
};

// Generic Memory Error entries
GENERIC_MEM_ERR_ENTRY gGenMemErrEntry = {
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                            // UEFI 2.6 Section N 2.5 Memory Error Section Type 16-byte GUID, update in runtime.
    ERROR_SEVERITY_FATAL,                                         // UINT32 ACPI 6.1 Table 18-343 Generic Error Data Entry
    GENERIC_ERROR_REVISION,                                       // UINT16 UEFI 2.6 Section N Revision Field
    FRU_STRING_VALID,                                             // UINT8 UEFI 2.6 Section N Section Descriptor Validation Bits field
    0x1,                                                          // UINT8 UEFI 2.6 Section N Section Descriptor Flags
    sizeof (PLATFORM_MEM_ERR_SEC),                                // UINT32 Generic Error Data Block Length
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                            // UINT8[16] FRU ID - UEFI 2.6 Section N
    {'U','n','c','o','r','r','e','c','t','e','d','E','r','r'}     // UINT8[20] FRU Text[20] - UEFI 2.6 Section N
  },
  {
    {0},                                                          // 0x4009 UEFI 2.6 Section N Table 245 Bit[14]-Memory Error Type Valid, Bit[3]-Node Valid, Bit[0]-ErrSts Valid (0xC34D)
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                  // Card,Node, Module, Bank, etc.
    UNKNOWN                                                       // Memory Error Type
  }
};

// Generic Memory CRC/Parity Error entries
GENERIC_MEM_ERR_ENTRY gGenMemParityErrEntry = {
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                            // UEFI 2.6 Section N 2.5 Memory Error Section Type 16-byte GUID, update in runtime.
    ERROR_SEVERITY_FATAL,                                         // UINT32 ACPI 6.1 Table 18-343 Generic Error Data Entry
    GENERIC_ERROR_REVISION,                                       // UINT16 UEFI 2.6 Section N Revision Field
    FRU_STRING_VALID,                                             // UINT8 UEFI 2.6 Section N Section Descriptor Validation Bits field
    0x1,                                                          // UINT8 UEFI 2.6 Section N Section Descriptor Flags
    sizeof (PLATFORM_MEM_ERR_SEC),                                // UINT32 Generic Error Data Block Length
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                            // UINT8[16] FRU ID - UEFI 2.6 Section N
    {'C','R','C','/','P','a','r','i','t','y','E','r','r'}         // UINT8[20] FRU Text[20] - UEFI 2.6 Section N
  },
  {
    {0},                                                          // 0x4009 UEFI 2.6 Section N Table 245 Bit[14]-Memory Error Type Valid, Bit[3]-Node Valid, Bit[0]-ErrSts Valid (0xC34D)
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                  // Card,Node, Module, Bank, etc.
    PARITY_ERROR                                                  // Memory Error Type
  }
};

EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE HestErrRegTbl = {
  {0,0,0,0,0},                                                     // UINT32 BlockStatus - WHEA Platform Design Guide Table 3-2 Block Status
  sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE),               // UINT32 RawDataOffset - WHEA Platform Design Guide Table 3-2 Raw Data Offset
  0,                                                               // UINT32 RawDataLength - WHEA Platform Design Guide Table 3-2 Raw Data Offset
  0,                                                               // UINT32 DataLength - WHEA Platform Design Guide Table 3-2 Raw Data Offset
  EFI_ACPI_6_0_ERROR_SEVERITY_NONE                                 // UINT32 ErrorSeverity - WHEA Platform Design Guide Table 3-2 Raw Data Offset (Initialize Severity to [None] = 0x03)
};

GENERIC_NBIO_ERR_ENTRY gGenNbioErrEntry = {
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                            // AMD NBIO Error Section Type 16-byte GUID, fill the GUID in runtime.
    ERROR_SEVERITY_FATAL,                                         // UINT32 ACPI 6.1 Table 18-343 Generic Error Data Entry
    GENERIC_ERROR_REVISION,                                       // UINT16 UEFI 2.6 Section N Revision Field
    FRU_STRING_VALID,                                             // UINT8 UEFI 2.6 Section N Section Descriptor Validation Bits field
    0x1,                                                          // UINT8 UEFI 2.6 Section N Section Descriptor Flags
    sizeof (AMD_NBIO_ERROR_RECORD),                               // UINT32 Generic Error Data Block Length
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                            // UINT8[16] FRU ID - UEFI 2.6 Section N
    {'N','b','i','o','E','r','r','o','r'}                         // UINT8 [20] FRU Text[20] - UEFI 2.6 Section N
  },
  {
    {0},
    0,
    {0},
    {0},
    0,
    0
  }
};

GENERIC_SMN_ERR_ENTRY gGenSmnErrEntry = {
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                                          // AMD SMN Error Section Type 16-byte GUID
    ERROR_SEVERITY_FATAL,                                         // UINT32 ACPI 6.1 Table 18-343 Generic Error Data Entry
    GENERIC_ERROR_REVISION,                                       // UINT16 UEFI 2.6 Section N Revision Field
    FRU_STRING_VALID,                                             // UINT8 UEFI 2.6 Section N Section Descriptor Validation Bits field
    0x1,                                                          // UINT8 UEFI 2.6 Section N Section Descriptor Flags
    sizeof (AMD_SMN_ERROR_RECORD),                                // UINT32 Generic Error Data Block Length
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                            // UINT8[16] FRU ID - UEFI 2.6 Section N
    {'S','m','n','E','r','r','o','r'}                             // UINT8 [20] FRU Text[20] - UEFI 2.6 Section N
  },
  {
    {0},
    0,
    {0},
    {0},
  }
};

EFI_ACPI_6_0_GENERIC_HARDWARE_ERROR_SOURCE_STRUCTURE gNbioErrSrc = {
  0x0009,                                                         // Type
  NBIO_SOURCE_ID,                                                 // SourceId
  0xffff,                                                         // RelatedSourceId
  0x00,                                                           // Flags
  0x01,                                                           // Enabled
  0x00000001,                                                     // NumberofRecordsToPreAllocate
  0x00000001,                                                     // MaxSectionsPerRecord
  MAX_ERROR_BLOCK_SIZE,                                           // MaxRawDataLength
  {0x00, 0x40, 0x00, 0x04, 0x00},                                 // ErrorStatusAddress. Address will be filled in runtime
  {
    HARDWARE_ERROR_NOTIFICATION_POLLED,                           // UINT8  NotifiyType = Polled
    sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE),  // UINT8  ErrNotifyLen
    {0,0,0,0,0,0,0},                                              // UINT16 ConfigWrite
    5000,                                                         // UINT32 PollInterval = 5000 ms
    0,                                                            // UINT32 Interrupt Vector
    0,                                                            // UINT32 SwitchPollingThreshVal
    0,                                                            // UINT32 SwitchPollingThreshWindow
    0,                                                            // UINT32 ErrThresholdVal;
    0                                                             // UINT32 ErrThresholdWindow
  },
  MAX_ERROR_BLOCK_SIZE                                            // ErrorStatusSize
};

GENERIC_PCIE_AER_ERR_ENTRY gGenPcieErrEntry = {
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                            // AMD NBIO Error Section Type 16-byte GUID, update in runtime.
    ERROR_SEVERITY_FATAL,                                         // UINT32 ACPI 6.1 Table 18-343 Generic Error Data Entry
    GENERIC_ERROR_REVISION,                                       // UINT16 UEFI 2.6 Section N Revision Field
    FRU_STRING_VALID,                                             // UINT8 UEFI 2.6 Section N Section Descriptor Validation Bits field
    0x1,                                                          // UINT8 UEFI 2.6 Section N Section Descriptor Flags
    sizeof (PCIE_ERROR_SECTION),                                  // UINT32 Generic Error Data Block Length
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                            // UINT8[16] FRU ID - UEFI 2.6 Section N
    {'P','c','i','e','E','r','r','o','r'}                         // UINT8 [20] FRU Text[20] - UEFI 2.6 Section N
  }
};

EFI_ACPI_6_0_GENERIC_HARDWARE_ERROR_SOURCE_STRUCTURE gPcieGenErrSrc = {
  0x0009,                                                         // Type
  PCIE_SOURCE_ID,                                                 // SourceId
  0xffff,                                                         // RelatedSourceId
  0x00,                                                           // Flags
  0x01,                                                           // Enabled
  0x00000001,                                                     // NumberofRecordsToPreAllocate
  0x00000001,                                                     // MaxSectionsPerRecord
  MAX_ERROR_BLOCK_SIZE,                                           // MaxRawDataLength
  {0x00, 0x40, 0x00, 0x04, 0x00},                                 // ErrorStatusAddress. Address will be filled in runtime
  {
    HARDWARE_ERROR_NOTIFICATION_POLLED,                           // UINT8  NotifiyType = Polled
    sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE),  // UINT8  ErrNotifyLen
    {0,0,0,0,0,0,0},                                              // UINT16 ConfigWrite
    5000,                                                         // UINT32 PollInterval = 5000 ms
    0,                                                            // UINT32 Interrupt Vector
    0,                                                            // UINT32 SwitchPollingThreshVal
    0,                                                            // UINT32 SwitchPollingThreshWindow
    0,                                                            // UINT32 ErrThresholdVal;
    0                                                             // UINT32 ErrThresholdWindow
  },
  MAX_ERROR_BLOCK_SIZE                                            // ErrorStatusSize
};

//A PCI-E port device/function loopup table.
PCIE_PORT_LIST gPciePortList[] = {
    {1, 1},         //PCI-E 0 Port A
    {1, 2},         //PCI-E 0 Port B
    {1, 3},         //PCI-E 0 Port C
    {1, 4},         //PCI-E 0 Port D
    {1, 5},         //PCI-E 0 Port E
    {1, 6},         //PCI-E 0 Port F
    {1, 7},         //PCI-E 0 Port G
    {2, 1},         //PCI-E 0 Port H
    {3, 1},         //PCI-E 1 Port A
    {3, 2},         //PCI-E 1 Port B
    {3, 3},         //PCI-E 1 Port C
    {3, 4},         //PCI-E 1 Port D
    {3, 5},         //PCI-E 1 Port E
    {3, 6},         //PCI-E 1 Port F
    {3, 7},         //PCI-E 1 Port G
    {4, 1}          //PCI-E 1 Port H
};


ERR_ACT_CTRL_REG gPciePortActListAerDpc[] = {
  {1, 0, 1, 1},                        // SerrActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  {1, 0, 1, 1},                        // IntFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  {0, 3, 0, 0},                        // IntNonFatalActCtrl (IntrGenSel = SMI)
  {0, 0, 0, 0},                        // IntCorrActCtrl (IntrGenSel = SMI)
  {0, 0, 0, 0},                        // ExtFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  {0, 0, 0, 0},                        // ExtNonFatalActCtrl (IntrGenSel = SMI)
  {0, 3, 0, 0},                        // ExtCorrActCtrl (IntrGenSel = SMI)
  {0, 3, 0, 0}                         // ParityErrActCtrl (IntrGenSel = SMI)
};

ERR_ACT_CTRL_REG gPciePortActListAerPfehDis[] = {
  {1, 0, 1, 1},                        // SerrActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  {0, 0, 0, 0},                        // IntFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  {0, 0, 0, 0},                        // IntNonFatalActCtrl
  {0, 0, 0, 0},                        // IntCorrActCtrl
  {0, 0, 0, 0},                        // ExtFatalActCtrl
  {0, 0, 0, 0},                        // ExtNonFatalActCtrl
  {0, 0, 0, 0},                        // ExtCorrActCtrl
  {0, 3, 0, 0}                         // ParityErrActCtrl (IntrGenSel = SMI)
};

ERR_ACT_CTRL_REG gPciePortActListAerNoDpc[] = {
  {1, 0, 1, 1},                        // SerrActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  {0, 0, 0, 0},                        // IntFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  {0, 0, 0, 0},                        // IntNonFatalActCtrl (IntrGenSel = SMI)
  {0, 0, 0, 0},                        // IntCorrActCtrl (IntrGenSel = SMI)
  {1, 0, 1, 1},                        // ExtFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  {0, 3, 0, 0},                        // ExtNonFatalActCtrl (IntrGenSel = SMI)
  {0, 3, 0, 0},                        // ExtCorrActCtrl (IntrGenSel = SMI)
  {0, 3, 0, 0}                         // ParityErrActCtrl (IntrGenSel = SMI)
};

ERR_ACT_CTRL_REG gPciePortActListNoAer[] = {
  {1, 0, 1, 1},                        // SerrActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  {1, 0, 1, 1},                        // IntFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  {0, 3, 0, 0},                        // IntNonFatalActCtrl (IntrGenSel = SMI)
  {0, 3, 0, 0},                        // IntCorrActCtrl (IntrGenSel = SMI)
  {0, 0, 0, 0},                        // ExtFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  {0, 0, 0, 0},                        // ExtNonFatalActCtrl (IntrGenSel = SMI)
  {0, 0, 0, 0},                        // ExtCorrActCtrl (IntrGenSel = SMI)
  {0, 3, 0, 0}                         // ParityErrActCtrl (IntrGenSel = SMI)
};


ERR_ACT_CTRL_REG gNbifActConfigList[] = {
  {1, 0, 1, 1},                        // SerrActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  {1, 0, 1, 1},                        // IntFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  {0, 3, 0, 0},                        // IntNonFatalActCtrl (IntrGenSel = SMI)
  {0, 3, 0, 0},                        // IntCorrActCtrl (IntrGenSel = SMI)
  {1, 0, 1, 1},                        // ExtFatalActCtrl (SyncFloodEn, LinkDisEn, APML_ERR_En)
  {0, 3, 0, 0},                        // ExtNonFatalActCtrl (IntrGenSel = SMI)
  {0, 3, 0, 0},                        // ExtCorrActCtrl (IntrGenSel = SMI)
  {0, 3, 0, 0}                         // ParityErrActCtrl (IntrGenSel = SMI)
};

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
  UINT8  *BuffTempPtr;
  UINT8  Checksum;
  UINT32 BufferOffset;

  Table->Checksum = 0;
  Checksum = 0;
  BuffTempPtr = (UINT8 *) Table;
  for (BufferOffset = 0; BufferOffset < Table->Length; BufferOffset++) {
    Checksum = Checksum - *(BuffTempPtr + BufferOffset);
  }

  Table->Checksum = Checksum;
}

/*********************************************************************************
 * Name: AmdPlatformRasDxeInit
 *
 * Description
 *   Entry point of the AMD SOC Zeppelin SP4 DXE driver
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
AmdPlatformRasDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_HANDLE    Handle = NULL;
  EFI_EVENT     ExitBootServicesEvent;


  // Locate ACPI Support table.
  Status = gBS->LocateProtocol (
                       &gEfiAcpiTableProtocolGuid,
                       NULL,
                       &AcpiTableProtocol
                       );
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  // Locate Ras APEI protocol
  Status = gBS->LocateProtocol (
                       &gAmdRasApeiProtocolGuid,
                       NULL,
                       &AmdRasApeiProtocol
                       );
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  PlatformConfigInit();

  // Initialize BERT
  ApeiBertInstall ();
  // Initialize HEST
  ApeiHestInstall ();
  // Initialize EINJ
  ApeiEinjInstall ();
  // Initialize ERST
//  ApeiErstInstall ();

  NbioHestUpdate ();

  PciePortDetect ();

  PcieAerHestUpdate ();

  mPlatformApeiPrivate->Valid = TRUE;

  Handle = ImageHandle;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdPlatformApeiDataProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  mPlatformApeiPrivate
                  );
  if (EFI_ERROR (Status)) {
    return (Status);
  }

  SmnErrorDetection ();
  McaErrorDetection ();
  NbioErrorDetection ();

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


VOID
RasLateBootInit(
  IN EFI_EVENT  Event,
  IN VOID       *Context
)
{
  gBS->CloseEvent(Event);  
  
  //PCI-E AER platform-first enablement
  PcieAerErrorConfig ();

  //NBIF platform-first enablement
  NbifErrorConfig ();
}


EFI_STATUS
PlatformConfigInit ( VOID )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINT64 MmioMsr;
  //
  //  Allocate memory and Initialize for Data block
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

  mPlatformApeiPrivate->PlatRasPolicy.PFEHEnable = PcdGetBool (PcdAmdCcxCfgPFEHEnable);
  mPlatformApeiPrivate->PlatRasPolicy.FchApuRasSmiSupport = PcdGetBool (PcdAmdFchApuRasSmiSupport);

  mPlatformApeiPrivate->PlatRasPolicy.MceSwSmiData = PcdGet8 (PcdMceSwSmiData);
  mPlatformApeiPrivate->PlatRasPolicy.EinjSwSmiData = PcdGet8 (PcdCpmEinjSwSmiData);
  mPlatformApeiPrivate->PlatRasPolicy.McaErrThreshEn = PcdGetBool (PcdMcaErrThreshEn);
  mPlatformApeiPrivate->PlatRasPolicy.McaErrThreshCount = PcdGet16 (PcdMcaErrThreshCount);
  mPlatformApeiPrivate->PlatRasPolicy.McaPollInterval = PcdGet32 (PcdCpmMcaPollInterval);
  mPlatformApeiPrivate->PlatRasPolicy.NbioPollInterval = PcdGet32 (PcdCpmNbioPollInterval);
  mPlatformApeiPrivate->PlatRasPolicy.PciePollInterval = PcdGet32 (PcdCpmPciePollInterval);
  mPlatformApeiPrivate->PlatRasPolicy.NbioCorrectedErrThreshEn = PcdGetBool (PcdNbioCorrectedErrThreshEn);
  mPlatformApeiPrivate->PlatRasPolicy.NbioCorrectedErrThreshCount = PcdGet16 (PcdNbioCorrectedErrThreshCount);
  mPlatformApeiPrivate->PlatRasPolicy.NbioDeferredErrThreshEn = PcdGetBool (PcdNbioDeferredErrThreshEn);
  mPlatformApeiPrivate->PlatRasPolicy.NbioDeferredErrThreshCount = PcdGet16 (PcdNbioDeferredErrThreshCount);
  mPlatformApeiPrivate->PlatRasPolicy.FchSataRasSupport = PcdGetBool (PcdSataRasSupport);
  mPlatformApeiPrivate->PlatRasPolicy.RasSmiThreshold = PcdGet32 (PcdCpmSmiThreshold);
  mPlatformApeiPrivate->PlatRasPolicy.RasSmiScale = PcdGet32 (PcdCpmSmiScale);
  mPlatformApeiPrivate->PlatRasPolicy.RasThresholdPeriodicSmiEn = PcdGetBool (PcdCpmThresholdPeriodicSmiEn);
  mPlatformApeiPrivate->PlatRasPolicy.RasRetryCnt = PcdGet32 (PcdCpmRasRetryCount);
  mPlatformApeiPrivate->PlatRasPolicy.EinjTrigErrSwSmiId = PcdGet8 (PcdCpmEinjTrigErrSwSmiId);

  //FCH software SMI command port
  mPlatformApeiPrivate->PlatRasPolicy.SwSmiCmdPortAddr = MmioRead16 ((ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A));

  //Get PCI configuration MMIO base address.
  MmioMsr = AsmReadMsr64 (MSR_MMIO_CFG_BASE);
  mPlatformApeiPrivate->PcieBaseAddress = (MmioMsr & 0x0000FFFFFFF00000);

  return Status;
}

EFI_STATUS
NbioHestUpdate ( VOID )
{
  EFI_STATUS                                 Status = EFI_SUCCESS;
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE   *NbioErrBlk;
  GENERIC_NBIO_ERR_ENTRY                     *NbioErrData;
  UINT64                                     *ErrBlkAddress;
  EFI_GUID                                   NbioErrorSectGuid = NBIO_ERROR_SECT_GUID;

  //
  //  Allocate memory and Initialize for NBIO Error Data block
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

  //Init Hest NBIO Generic error status block.
  gBS->CopyMem (NbioErrBlk, &HestErrRegTbl, sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE));

  //Init Hest NBIO Generic error data entry
  NbioErrData = (GENERIC_NBIO_ERR_ENTRY*) (NbioErrBlk + 1);
  gBS->CopyMem (NbioErrData, &gGenNbioErrEntry, sizeof (GENERIC_NBIO_ERR_ENTRY));

  //Update Error section GUID
  gBS->CopyMem (&NbioErrData->GenErrorDataEntry.SectionType[0], &NbioErrorSectGuid, sizeof (EFI_GUID));

  //Update pointer and install NBIO error source record.
  *ErrBlkAddress = (UINT64) NbioErrBlk;
  gNbioErrSrc.ErrorStatusAddress.Address = (UINT64)ErrBlkAddress;

  //Update NBIO error notification polling interval time.
  gNbioErrSrc.NotificationStructure.PollInterval = mPlatformApeiPrivate->PlatRasPolicy.NbioPollInterval;

  AmdRasApeiProtocol->AddHestErrorSourceEntry ((UINT8*)&gNbioErrSrc, sizeof (EFI_ACPI_6_0_GENERIC_HARDWARE_ERROR_SOURCE_STRUCTURE));

  mPlatformApeiPrivate->AmdNbioErrBlk = NbioErrBlk;

  return Status;
}

UINT8
GetPhysicalPortNumber(
   IN UINT8  DieBusNum,
   IN UINT32 Device,
   IN UINT32 Function
)
{
    UINT32   Index;
    UINT32   Value;
    UINT32   Addr;
    
    Addr = (Device << 3) + Function;

    //Get PCIE physical port
    for (Index = 0; Index < 18; Index++)
    {
        RasSmnRead(DieBusNum, NB_PROG_DEVICE_REMAP+(Index << 2), &Value);
        if (Addr == (Value & 0xFF))
        {
            return (UINT8)Index;
        }
    }
    
    //Error return, never run here
    return 0xFF;
}

EFI_STATUS
PciePortDetect ( VOID )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  PCIE_ACTIVE_PORT_MAP  *TempPciePortMap;
  PCIE_ACTIVE_PORT_MAP  *mPciePortMap;
  UINT32                PciePortMapSize;
  UINT8                 Die;
  UINT8                 Port;
  UINT8                 PhysicalPort;
  UINT8                 DieBusNum;
  UINT16                VendorID;
  UINTN                 NbioPciAddr;
  PCI_ADDR              PciPortAddr;

  //
  //  Allocate memory and Initialize a temporary PCI-E Port Map data block and NBIO Error Data block pointer
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

  for (Die = 0; Die < MAX_DIE_SUPPORT; Die++) {
    NbioPciAddr = ((Die + 24) << 15) + 0x000;
    VendorID = PciRead16 (NbioPciAddr);
    if ((VendorID != AMD_VENDOR_ID) && (VendorID != HYGON_VID)) {
      continue;
    }
    NbioPciAddr = ((Die + 24) << 15) + (DF_CFGADDRESSCNTL_FUNC << 12) + DF_CFGADDRESSCNTL_OFFSET;
    DieBusNum = PciRead8 (NbioPciAddr);

    DEBUG ((EFI_D_ERROR, "[RAS]Die %d, Bus 0x%x, NBIO PCI Address: 0x%08x\n", Die, DieBusNum, NbioPciAddr));

    PciPortAddr.AddressValue = 0;

    for (Port = 0; Port < MAX_PCIE_PORT_SUPPORT; Port++) {
      PciPortAddr.Address.Bus = DieBusNum;
      PciPortAddr.Address.Device = gPciePortList[Port].Device;
      PciPortAddr.Address.Function = gPciePortList[Port].Function;

      VendorID = PciRead16 (PciPortAddr.AddressValue);
      if ((VendorID != AMD_VENDOR_ID) && (VendorID != HYGON_VID)) {
        continue;
      }
      DEBUG ((EFI_D_ERROR, "[RAS]Active PCI-E Root Port Bus:%d Dev:%d Fun:%d, Address: 0x%08x\n",
              PciPortAddr.Address.Bus,
              PciPortAddr.Address.Device,
              PciPortAddr.Address.Function,
              PciPortAddr.AddressValue));

      //Found active PCI-E port
      TempPciePortMap->PciPortNumber[TempPciePortMap->PortCount].NbioDieNum = Die;
      TempPciePortMap->PciPortNumber[TempPciePortMap->PortCount].NbioBusNum = DieBusNum;
      //Get PCIE logical port
      TempPciePortMap->PciPortNumber[TempPciePortMap->PortCount].PciPortNumber = Port;
      //Get PCIE physical port
      PhysicalPort = GetPhysicalPortNumber(DieBusNum, PciPortAddr.Address.Device, PciPortAddr.Address.Function);
      TempPciePortMap->PciPortNumber[TempPciePortMap->PortCount].PciPhysicalPortNumber = PhysicalPort;
       
      DEBUG ((EFI_D_ERROR, "[RAS] Logical port %d, physical port %d\n", Port, PhysicalPort));
              
      TempPciePortMap->PortCount++;
    }
  }

  DEBUG ((EFI_D_ERROR, "[RAS]Total Active PCI-E port count %d\n", TempPciePortMap->PortCount));

  PciePortMapSize = (sizeof (PCIE_ACTIVE_PORT_MAP)) + ((sizeof (PCIE_PORT_PROFILE)) * (TempPciePortMap->PortCount - 1));

  Status = gBS->AllocatePool (
                  EfiReservedMemoryType,
                  PciePortMapSize,
                  (VOID **)&mPciePortMap
                  );
  if (EFI_ERROR (Status)) {
    //release temporary PCI-E port map space.
    gBS->FreePool (TempPciePortMap);
    return Status;
  }

  //Copy to reserve memory space
  gBS->CopyMem (mPciePortMap, TempPciePortMap, PciePortMapSize);

  mPlatformApeiPrivate->AmdPciePortMap = mPciePortMap;

  return Status;
}

EFI_STATUS
PcieAerHestUpdate ( VOID )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE   *PcieAerErrBlk;
  GENERIC_NBIO_ERR_ENTRY                     *PcieAerErrData;
  UINT64                                     *ErrBlkAddress;
  EFI_GUID                                   PcieErrorSectGuid = PCIE_SECT_GUID;

  //
  //  Allocate memory and Initialize for NBIO Error Data block and NBIO Error Data block pointer
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
                  (VOID **)&PcieAerErrBlk
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (PcieAerErrBlk, MAX_ERROR_BLOCK_SIZE);

  //Init Hest PCI-E Aer Generic error status block.
  gBS->CopyMem (PcieAerErrBlk, &HestErrRegTbl, sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE));

  //Init Hest NBIO Generic error data entry
  PcieAerErrData = (GENERIC_NBIO_ERR_ENTRY*) (PcieAerErrBlk + 1);
  gBS->CopyMem (PcieAerErrData, &gGenPcieErrEntry, sizeof (GENERIC_PCIE_AER_ERR_ENTRY));

  //Update Error section GUID
  gBS->CopyMem (&PcieAerErrData->GenErrorDataEntry.SectionType[0], &PcieErrorSectGuid, sizeof (EFI_GUID));

  //Update pointer and install NBIO error source record.
  *ErrBlkAddress = (UINT64) PcieAerErrBlk;
  gPcieGenErrSrc.ErrorStatusAddress.Address = (UINT64)ErrBlkAddress;

  //Update PCI-E error notification polling interval time.
  gPcieGenErrSrc.NotificationStructure.PollInterval = mPlatformApeiPrivate->PlatRasPolicy.PciePollInterval;

  AmdRasApeiProtocol->AddHestErrorSourceEntry ((UINT8*)&gPcieGenErrSrc, sizeof (EFI_ACPI_6_0_GENERIC_HARDWARE_ERROR_SOURCE_STRUCTURE));

  mPlatformApeiPrivate->AmdPcieAerErrBlk = PcieAerErrBlk;

  return Status;
}

EFI_STATUS
NbioSetDpc (
  PCI_ADDR  PciCfgAddr
  )
{
  PCIE_DPC_CNTL_REG                     DpcReg;
  PCI_ADDR                              PciPortAddr;
  UINT16                                DpcCapPtr;

  //Move to local variable
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
    DpcReg.DpcRegField.DpcTriggerEnable = 0x01;
    DpcReg.DpcRegField.DpcCompletionControl = 1;
    DpcReg.DpcRegField.DpcInterruptEnable = 0;
    DpcReg.DpcRegField.DpcErrCorEnable = 0;
    DpcReg.DpcRegField.PoisonedTLPEgressBlkEn = 0;
    DpcReg.DpcRegField.DpcSoftwareTrigger = 0;
    DpcReg.DpcRegField.DlActiveErrCorEnable = 0;
    // Set PCIE_DPC_CNTL: 0x386
    PciWrite16 (PciPortAddr.AddressValue, DpcReg.Raw);
    gDpcEn = TRUE;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
PcieStsClr (
  PCI_ADDR  PciCfgAddr
  )
{
  UINT16        AerCapPtr;
  UINT16        SecPcieExtCapPtr;
  UINT8         PcieCapPtr;
  UINT32        PcieUncorrStatus;
  UINT32        PcieCorrStatus;
  UINT32        PcieRootStatus;
  UINT16        PcieDevStatus;
  UINT32        PcieLnkStatus;

  AerCapPtr = RasFindPcieExtendedCapability (PciCfgAddr.AddressValue, PCIE_EXT_AER_CAP_ID);
  SecPcieExtCapPtr = RasFindPcieExtendedCapability (PciCfgAddr.AddressValue, SEC_PCIE_EXT_CAP_ID);
  PcieCapPtr = RasFindPciCapability (PciCfgAddr.AddressValue, PCIE_CAP_ID);

  PcieUncorrStatus = 0xFFFFFFFF;
  PcieCorrStatus = 0xFFFFFFFF;
  PcieRootStatus = 0xFFFFFFFF;
  PcieDevStatus = 0xF;
  PcieLnkStatus = 0xFFFFFFFF;

  DEBUG ((EFI_D_ERROR, "[RAS]ClrSts @ Bus: 0x%x, Dev: 0x%x, Func: 0x%x\n", PciCfgAddr.Address.Bus, PciCfgAddr.Address.Device, PciCfgAddr.Address.Function));
  //Clear Status register
  if (AerCapPtr != 0) {
    while (PcieUncorrStatus != 0) {
      PciWrite32(PciCfgAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR, PcieUncorrStatus);
      PcieUncorrStatus = PciRead32 (PciCfgAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR);
      DEBUG ((EFI_D_ERROR, "[RAS]  PCIE UnCorr Error Status : 0x%08x\n", PcieUncorrStatus));
    }
    while (PcieCorrStatus != 0) {
      PciWrite32 (PciCfgAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR, PcieCorrStatus);
      PcieCorrStatus = PciRead32 (PciCfgAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR);
      DEBUG ((EFI_D_ERROR, "[RAS]  PCIE Corr Error Status : 0x%08x\n", PcieCorrStatus));
    }
    while (PcieRootStatus != 0) {
      PciWrite32 (PciCfgAddr.AddressValue + AerCapPtr + PCIE_ROOT_STATUS_PTR, PcieRootStatus);
      PcieRootStatus = PciRead32 (PciCfgAddr.AddressValue + AerCapPtr + PCIE_ROOT_STATUS_PTR);
      DEBUG ((EFI_D_ERROR, "[RAS]  PCIE Root Error Status : 0x%08x\n", PcieRootStatus));
    }
  }

  if (PcieCapPtr != 0) {
    while ((PcieDevStatus & 0xF) != 0) {
      PciWrite16(PciCfgAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_STATUS_PTR, PcieDevStatus);
      PcieDevStatus = PciRead16 (PciCfgAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_STATUS_PTR);
      DEBUG ((EFI_D_ERROR, "[RAS]  PCIE Device Status : 0x%08x\n", PcieDevStatus));
    }
  }

  if (SecPcieExtCapPtr !=0 ) {
    while (PcieLnkStatus != 0) {
      PciWrite32 (PciCfgAddr.AddressValue + SecPcieExtCapPtr + PCIE_LANE_ERR_STATUS_PTR, PcieLnkStatus);
      PcieLnkStatus = PciRead32 (PciCfgAddr.AddressValue + SecPcieExtCapPtr + PCIE_LANE_ERR_STATUS_PTR);
      DEBUG ((EFI_D_ERROR, "[RAS]  PCIE Lane Error Status : 0x%08x\n", PcieLnkStatus));
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PcieActionCtrl (
  UINT16    NbioBusNum,
  UINT16    PciePortNum
  )
{
  UINT32 PciePortActCtrlBase;
  UINT32 Index;
  ERR_ACT_CTRL_REG *PciePortActConfigList;

  PciePortActCtrlBase = PCIE_PORT_ACTION_CONTROL_BASE;
  PciePortActCtrlBase += PciePortNum * PCIE_ACTION_CONTROL_OFFSET;

  if (gAerEn) {
    if (gDpcEn) {
      DEBUG ((EFI_D_ERROR, "[RAS]Platform PCI-E Action Control. AER = TRUE, DPC = TRUE\n"));
      if(mPlatformApeiPrivate->PlatRasPolicy.PFEHEnable) {
          PciePortActConfigList = &gPciePortActListAerDpc[0];
      } else {
          PciePortActConfigList = &gPciePortActListAerPfehDis[0];
      }
    } else {
      DEBUG ((EFI_D_ERROR, "[RAS]Platform PCI-E Action Control. AER = TRUE, DPC = FALSE\n"));
      PciePortActConfigList = &gPciePortActListAerNoDpc[0];
    }
  } else {
    DEBUG ((EFI_D_ERROR, "[RAS]Platform PCI-E Action Control. AER = FALSE\n"));
    PciePortActConfigList = &gPciePortActListNoAer[0];
  }

  for (Index = 0; Index < 8; Index++) {
    RasSmnWrite ((UINT32)NbioBusNum, PciePortActCtrlBase + (Index * 4), &PciePortActConfigList[Index].Value);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EnAerErrorReport(
  PCI_ADDR  PciPortAddr
  )
{
   UINT16      AerCapPtr;
   UINT32      PcieRootErrCmd;

   AerCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, PCIE_EXT_AER_CAP_ID);

   if (AerCapPtr != 0) {
     PcieRootErrCmd = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_ROOT_ERR_CMD_PTR);
     PcieRootErrCmd |= PCIE_ROOT_CORR_ERR + PCIE_ROOT_NON_FATAL_ERR + PCIE_ROOT_FATAL_ERR;
     PciWrite32(PciPortAddr.AddressValue + AerCapPtr + PCIE_ROOT_ERR_CMD_PTR, PcieRootErrCmd);
   }

   return EFI_SUCCESS;
}

VOID
STATIC
PcieDevCntlEnableOnFunction (
  IN       PCI_ADDR             Function
  )
{
  UINT8       PcieCapPtr;
  UINT16      PcieDevCtrl;

  PcieCapPtr = RasFindPciCapability (Function.AddressValue, PCIE_CAP_ID);
  if (PcieCapPtr != 0) {
     PcieDevCtrl = PciRead16 (Function.AddressValue + PcieCapPtr + PCIE_DEVICE_CONTROL_PTR);
     PcieDevCtrl |= PCIE_DEV_CORR_ERR + PCIE_DEV_NON_FATAL_ERR + PCIE_DEV_FATAL_ERR;
     PciWrite16(Function.AddressValue + PcieCapPtr + PCIE_DEVICE_CONTROL_PTR, PcieDevCtrl);
  }
}

VOID
STATIC
PcieDevCntlEnableOnDevice (
  IN       PCI_ADDR             Device
  )
{
  UINT8                   MaxFunc;
  UINT8                   CurrentFunc;

  MaxFunc = RasPciIsMultiFunctionDevice (Device.AddressValue) ? 7 : 0;
  for (CurrentFunc = 0; CurrentFunc <= MaxFunc; CurrentFunc++) {
    Device.Address.Function = CurrentFunc;
    DEBUG ((EFI_D_ERROR, "  Checking Device: %d:%d:%d\n",
            Device.Address.Bus,
            Device.Address.Device,
            Device.Address.Function
            ));
    if (RasPciIsDevicePresent (Device.AddressValue)) {
      DEBUG ((EFI_D_ERROR, "  Enable Device Error report for Device = %d:%d:%d\n",
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
  SCAN_STATUS             ScanStatus;
  PCIE_DEVICE_TYPE        DeviceType;
  ScanStatus = SCAN_SUCCESS;
  DEBUG ((EFI_D_ERROR, "  PcieDevCntlCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    ));
  ScanStatus = SCAN_SUCCESS;
  DeviceType = RasGetPcieDeviceType (Device);
  DEBUG ((EFI_D_ERROR, "  PCI-E device type = 0x%x\n", DeviceType));
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    DEBUG ((EFI_D_ERROR, "  PCI-E device root port found\n"));
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  case  PcieDevicePcieToPcix:
    DEBUG ((EFI_D_ERROR, "  PCI-E device PCIE to PCIx found\n"));
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
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
//  RasPciScan (DownstreamPort, DownstreamPort, &ScanData);
//  GnbLibPciRMW (Device.AddressValue | 0x18, AccessS3SaveWidth32, 0xffffffffull, 0x0, ScanData->StdHeader);
  RasPciScanSecondaryBus (DownstreamPort, &ScanData);
}

EFI_STATUS
EnDevErrReport(
  PCI_ADDR  PciPortAddr
  )
{
  UINT16      PcieDevCtrl;
  UINT8       PcieCapPtr;

  //Get active port PCI-E cap pointer
  PcieCapPtr = RasFindPciCapability (PciPortAddr.AddressValue, PCIE_CAP_ID);

  if (PcieCapPtr != 0) {
    //Enable down link PCI-E end devices.
    PcieDevCntlConfigure(PciPortAddr);

    //Enable active port dev_cntl
    PcieDevCtrl = PciRead16 (PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_CONTROL_PTR);
    PcieDevCtrl |= PCIE_DEV_CORR_ERR + PCIE_DEV_NON_FATAL_ERR + PCIE_DEV_FATAL_ERR;
    PciWrite16(PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_CONTROL_PTR, PcieDevCtrl);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PcieAerErrorConfig ( VOID )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  AMD_NBIO_PCIE_AER_PROTOCOL    *AmdNbioPcieAerProtocol;
  UINT16                        PciePortIndex;
  PCIE_PORT_PROFILE             *PciePortProfileInstance;
  PCI_ADDR                      PciPortAddr;
  PCIe_PORT_AER_CONFIG          PcieAerSetting;

  DEBUG ((EFI_D_ERROR, "[RAS]Platform PCI-E Aer Enablement entry\n"));

  // Get NBIO PCI-E AER protocol
  Status = gBS->LocateProtocol (&gAmdNbioPcieAerProtocolGuid, NULL, &AmdNbioPcieAerProtocol);
  if (EFI_ERROR (Status)) {
    return Status;    // Error detected while trying to locate protocol
  }

  PciePortProfileInstance = mPlatformApeiPrivate->AmdPciePortMap->PciPortNumber;
  for (PciePortIndex = 0; PciePortIndex < mPlatformApeiPrivate->AmdPciePortMap->PortCount; PciePortIndex++, PciePortProfileInstance++) {
    gAerEn = FALSE;
    gDpcEn = FALSE;
    PciPortAddr.AddressValue = 0;
    PciPortAddr.Address.Bus = PciePortProfileInstance->NbioBusNum;
    PciPortAddr.Address.Device = gPciePortList[PciePortProfileInstance->PciPortNumber].Device;
    PciPortAddr.Address.Function = gPciePortList[PciePortProfileInstance->PciPortNumber].Function;

    NbioSetDpc (PciPortAddr);

    PcieAerSetting.AerEnable = 1;
    PcieAerSetting.PciBus = PciePortProfileInstance->NbioBusNum;
    PcieAerSetting.PciDev = gPciePortList[PciePortProfileInstance->PciPortNumber].Device;
    PcieAerSetting.PciFunc = gPciePortList[PciePortProfileInstance->PciPortNumber].Function;
    PcieAerSetting.CorrectableMask.Value = 0;
    PcieAerSetting.UncorrectableMask.Value = 0;
    PcieAerSetting.UncorrectableMask.Field.UnsupportedRequestErrorMask = 1;
    PcieAerSetting.UncorrectableSeverity.Value = 0;

    AmdNbioPcieAerProtocol->SetPcieAerFeature (AmdNbioPcieAerProtocol, &PcieAerSetting);

    //PCI-E Aer firmware first setup
    PcieStsClr (PciPortAddr);
    gAerEn = TRUE;
    PcieActionCtrl (PciePortProfileInstance->NbioBusNum, PciePortProfileInstance->PciPhysicalPortNumber);
    EnAerErrorReport(PciPortAddr);

    EnDevErrReport(PciPortAddr);
  }

  return Status;
}


EFI_STATUS
NbifActionCtrl (
  UINT8    NbioBusNum
  )
{
  UINT32      Index;
  PCI_ADDR    Nbif0Addr;
  PCI_ADDR    Nbif1Addr;

  Nbif0Addr.AddressValue = MAKE_SBDFO (0, NbioBusNum, 7, 1, 0);
  Nbif1Addr.AddressValue = MAKE_SBDFO (0, NbioBusNum, 8, 1, 0);

  if (RasPciIsDevicePresent (Nbif0Addr.AddressValue)) {
    for (Index = 0; Index < 8; Index++) {
      //NBIF0 Action Control setup
      RasSmnWrite ((UINT32)NbioBusNum, NBIF0_ACTION_CONTROL_BASE + (Index * 4), &gNbifActConfigList[Index].Value);
    }
  }

  if (RasPciIsDevicePresent (Nbif1Addr.AddressValue)) {
    for (Index = 0; Index < 8; Index++) {
      //NBIF1 Action Control setup
      RasSmnWrite ((UINT32)NbioBusNum, NBIF1_ACTION_CONTROL_BASE + (Index * 4), &gNbifActConfigList[Index].Value);
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
NbifErrorConfig ( VOID )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT8       Die;
  UINT8       DieBusNum;
  UINT16      VendorID;
  UINTN       NbioPciAddr;

  DEBUG ((EFI_D_ERROR, "[RAS]Platform Nbif Enablement entry\n"));

  for (Die = 0; Die < MAX_DIE_SUPPORT; Die++) {
    NbioPciAddr = ((Die + 24) << 15) + 0x000;
    VendorID = PciRead16 (NbioPciAddr);
    if (VendorID != AMD_VENDOR_ID && VendorID != HYGON_VID) {
      continue;
    }
    NbioPciAddr = ((Die + 24) << 15) + (DF_CFGADDRESSCNTL_FUNC << 12) + DF_CFGADDRESSCNTL_OFFSET;
    DieBusNum = PciRead8 (NbioPciAddr);

    DEBUG ((EFI_D_ERROR, "[RAS]NBIF Action Control @ Die %d, Bus 0x%x\n", Die, DieBusNum));

    //NBIF Action Control setup
    NbifActionCtrl (DieBusNum);
  }

  return Status;
}

VOID
GetLocalApicBase (
  OUT   UINT64 *ApicBase
)
{
  *ApicBase = AsmReadMsr64 (MSR_APIC_BAR);
  *ApicBase &= LAPIC_BASE_ADDR_MASK;
}


VOID
GetLocalApicId(
  IN       UINT32 *LocalApicId
)
{
    UINT64 LocalApicBase;
    UINT64  Address;
    UINT32  TempVar_a;

    // Get local apic base Address
    GetLocalApicBase (&LocalApicBase);
    Address = LocalApicBase + APIC_ID_REG;

    TempVar_a = MmioRead32(Address);

    // ApicId [7:0]
    *LocalApicId = (TempVar_a >> APIC20_ApicId_Offset) & 0x000000FF;
}

UINT32
RasGetApicId(
    EFI_MP_SERVICES_PROTOCOL  *MpServices,
    UINTN ProcessorNumber
)
{
    UINT32 LocalApicId;
    if (ProcessorNumber == 0) {
    //This is BSP
      GetLocalApicId(&LocalApicId);
    }
    else {
      MpServices->StartupThisAP(
                  MpServices,
                  GetLocalApicId,
                  ProcessorNumber,
                  NULL,
                  0,
                  (VOID *) &LocalApicId,
                  NULL
                  );
    }
    return LocalApicId;
}

VOID
GetCpuId (RAS_CPUID_FN* RasCpuIdFn)
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
    //This is BSP
      GetCpuId(RasCpuidFn);
    }
    else {
      MpServices->StartupThisAP(
                  MpServices,
                  GetCpuId,
                  ProcessorNumber,
                  NULL,
                  0,
                  (VOID *) RasCpuidFn,
                  NULL
                  );
    }
}

VOID
GetSysEnterEip (UINT64* SysEnterEip)
{
  UINT64 McgStatReg;
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
    //This is BSP
      GetSysEnterEip(SysEnterEip);
    }
    else {
      MpServices->StartupThisAP(
                  MpServices,
                  GetSysEnterEip,
                  ProcessorNumber,
                  NULL,
                  0,
                  (VOID *) SysEnterEip,
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
  EFI_STATUS                    Status = EFI_SUCCESS;
  GENERIC_PROC_ERR_ENTRY        *GenericProcErrEntry;
  UINT32                        GenericProcErrEntrySize;
  EFI_GUID                      ProcErrorSectGuid = PROCESSOR_SPECIFIC_X86_SECT_GUID;
  EFI_GUID                      MsCheckGuid = IA32_X64_MS_CHECK_FIELD_DESC_GUID;
  EFI_GUID                      TlbCheckGuid = IA32_X64_TLB_CHECK_STRUC_GUID;
  EFI_GUID                      CacheCheckGuid = IA32_X64_CACHE_CHECK_STRUC_GUID;
  PROC_ERR_INFO_STRUC           *ProcErrInfoStruc;
  PROC_CONEXT_INFO_STRUC        *ProcContextInfoStruc;
  UINT32                        LocalApicId;
  RAS_CPUID_FN                  RasCpuidFn;
  MS_CHECK_FIELD                MsCheckField;
  TLB_CHECK_STRUC               TlbCheckStruc;
  CACHE_CHECK_STRUC             CacheCheckStruc;
  UINT16                        McaErrorCode;
  MCA_TLB_ERROR                 TlbError;
  MCA_MEM_ERROR                 MemError;
  MCA_PP_ERROR                  PpError;
  MCA_INT_ERROR                 InternalError;
  UINT64                        SysEnterEip;

  //prepare structure
  GenericProcErrEntrySize = sizeof (GENERIC_PROC_ERR_ENTRY) + sizeof (PROC_ERR_INFO_STRUC) + sizeof (PROC_CONEXT_INFO_STRUC) + ((sizeof (UINT64)) * 10);
  DEBUG ((EFI_D_ERROR, "[RAS]Processor Error Section Size: 0x%x\n", GenericProcErrEntrySize));
  Status = gBS->AllocatePool (EfiBootServicesData, GenericProcErrEntrySize, &GenericProcErrEntry);
  ZeroMem (GenericProcErrEntry, GenericProcErrEntrySize);

  ProcErrInfoStruc = (PROC_ERR_INFO_STRUC*)((UINTN )GenericProcErrEntry + sizeof(GENERIC_PROC_ERR_ENTRY));
  ProcContextInfoStruc = (PROC_CONEXT_INFO_STRUC*)((UINTN )ProcErrInfoStruc + sizeof(PROC_ERR_INFO_STRUC));

  DEBUG ((EFI_D_ERROR, "[RAS]GenericProcErrEntry Address: 0x%08x\n", GenericProcErrEntry));
  DEBUG ((EFI_D_ERROR, "[RAS]ProcErrInfoStruc Address: 0x%08x\n", ProcErrInfoStruc));
  DEBUG ((EFI_D_ERROR, "[RAS]ProcContextInfoStruc Address: 0x%08x\n", ProcContextInfoStruc));

  //Update generic data info
  gBS->CopyMem (GenericProcErrEntry, &gGenProcErrEntry, sizeof (GENERIC_PROC_ERR_ENTRY));
  GenericProcErrEntry->GenErrorDataEntry.ErrorDataLength = GenericProcErrEntrySize - sizeof (EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE);

  //Update Error section GUID
  gBS->CopyMem (&GenericProcErrEntry->GenErrorDataEntry.SectionType[0], &ProcErrorSectGuid, sizeof (EFI_GUID));

  //Checked Valid bits
  GenericProcErrEntry->ProcErrorSection.ValidBits.Field.CpuIdInfoValid = 1;
  GenericProcErrEntry->ProcErrorSection.ValidBits.Field.LocalApicIDValid = 1;
  GenericProcErrEntry->ProcErrorSection.ValidBits.Field.ProcErrInfoStrucNum = 1;
  GenericProcErrEntry->ProcErrorSection.ValidBits.Field.ProcContextInfoStrucNum = 0;

  //Get LocalApicId
  LocalApicId = RasGetApicId(MpServices, ProcessorNumber);
  DEBUG ((EFI_D_ERROR, "[RAS]LocalApicId: 0x%x\n", LocalApicId));
  GenericProcErrEntry->ProcErrorSection.LocalApicID = (UINT64)LocalApicId;

  //Get CPUID Information
  RasCpuidFn.FunctionId = 0x00000001;  //Register In EAX
  RasGetCpuIdInfo(MpServices, ProcessorNumber, &RasCpuidFn);
  DEBUG ((EFI_D_ERROR, "[RAS]Logical Processor : 0x%x, EAX: 0x%08x, EBX: 0x%08x, ECX: 0x%08x, EDX: 0x%08x\n", ProcessorNumber, RasCpuidFn.EAX_Reg, RasCpuidFn.EBX_Reg, RasCpuidFn.ECX_Reg, RasCpuidFn.EDX_Reg));

  GenericProcErrEntry->ProcErrorSection.CpuIdInfo_EAX = RasCpuidFn.EAX_Reg;
  GenericProcErrEntry->ProcErrorSection.CpuIdInfo_EBX = RasCpuidFn.EBX_Reg;
  GenericProcErrEntry->ProcErrorSection.CpuIdInfo_ECX = RasCpuidFn.ECX_Reg;
  GenericProcErrEntry->ProcErrorSection.CpuIdInfo_EDX = RasCpuidFn.EDX_Reg;

  //Check Error code types
  McaErrorCode = (UINT16)RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCode;

  if (((McaErrorCode & TLB_ERROR_MASK) >> TLB_ERROR_CHK_SHIFT) == 1) {
    //TLB error
    TlbError.Value = McaErrorCode;
    TlbCheckStruc.Value = 0;

    TlbCheckStruc.Field.Level = TlbError.Field.CacheLevel;
    TlbCheckStruc.Field.TranscationType = TlbError.Field.TransactionType;

    TlbCheckStruc.Field.ValidationBits = TLB_CHECK_LEVEL | TLB_CHECK_TRANSTYPE;

    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC) {
      TlbCheckStruc.Field.Uncorrected = 1;
      TlbCheckStruc.Field.ValidationBits |= TLB_CHECK_UNCORRECTED;
      if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.PCC == 0 && RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.TCC == 0) {
        TlbCheckStruc.Field.ValidationBits |= TLB_CHECK_PRECISE_IP | TLB_CHECK_RESTARTABLE;
        TlbCheckStruc.Field.PreciseIp = 1;
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

    //Report TLB check field description
    gBS->CopyMem (&ProcErrInfoStruc->ErrorStructureType[0], &TlbCheckGuid, sizeof (EFI_GUID));

    //Update Processor check information
    ProcErrInfoStruc->ValidBits.Field.CheckInfoValid = 1;
    ProcErrInfoStruc->CheckInformation = TlbCheckStruc.Value;

  } else if (((McaErrorCode & MEM_ERROR_MASK) >> MEM_ERROR_CHK_SHIFT) == 1) {
    //Memory error
    MemError.Value = McaErrorCode;
    CacheCheckStruc.Value = 0;

    CacheCheckStruc.Field.Level = MemError.Field.CacheLevel;
    CacheCheckStruc.Field.TranscationType = MemError.Field.TransactionType;
    CacheCheckStruc.Field.Operation = MemError.Field.MemTransactionType;

    CacheCheckStruc.Field.ValidationBits = CACHE_CHECK_LEVEL | CACHE_CHECK_TRANSTYPE | CACHE_CHECK_OPERATION;

    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC) {
      CacheCheckStruc.Field.Uncorrected = 1;
      CacheCheckStruc.Field.ValidationBits |= CACHE_CHECK_UNCORRECTED;
      if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.PCC == 0 && RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.TCC == 0) {
        CacheCheckStruc.Field.ValidationBits |= CACHE_CHECK_PRECISE_IP | CACHE_CHECK_RESTARTABLE;
        CacheCheckStruc.Field.PreciseIp = 1;
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

    //Report Cache check field description
    gBS->CopyMem (&ProcErrInfoStruc->ErrorStructureType[0], &CacheCheckGuid, sizeof (EFI_GUID));

    //Update Processor check information
    ProcErrInfoStruc->ValidBits.Field.CheckInfoValid = 1;
    ProcErrInfoStruc->CheckInformation = CacheCheckStruc.Value;

  } else if (((McaErrorCode & INT_ERROR_MASK) >> INT_ERROR_CHK_SHIFT) == 1) {
    //Bus Error
    PpError.Value = McaErrorCode;
    MsCheckField.Value = 0;

    MsCheckField.Field.ErrorType = MSCHK_ERRTYPE_INTERNAL_UNCLASSIFIED;
    MsCheckField.Field.ValidationBits = MS_CHECK_ERRORTYPE;

    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC) {
      MsCheckField.Field.Uncorrected = 1;
      MsCheckField.Field.ValidationBits |= MS_CHECK_UNCORRECTED;
      if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.PCC == 0 && RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.TCC == 0) {
        MsCheckField.Field.ValidationBits |= MS_CHECK_PRECISE_IP | MS_CHECK_RESTARTABLE;
        MsCheckField.Field.PreciseIp = 1;
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

    //Report MS check field description
    gBS->CopyMem (&ProcErrInfoStruc->ErrorStructureType[0], &MsCheckGuid, sizeof (EFI_GUID));

    //Update Processor check information
    ProcErrInfoStruc->ValidBits.Field.CheckInfoValid = 1;
    ProcErrInfoStruc->CheckInformation = MsCheckField.Value;

  } else if (((McaErrorCode & PP_ERROR_MASK) >> PP_ERROR_CHK_SHIFT) == 1) {
    //Internal unclassified error
    InternalError.Value = McaErrorCode;

    MsCheckField.Value = 0;

    MsCheckField.Field.ErrorType = InternalError.Field.InternalErrorType;
    MsCheckField.Field.ValidationBits = MS_CHECK_ERRORTYPE;

    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC) {
      MsCheckField.Field.Uncorrected = 1;
      MsCheckField.Field.ValidationBits |= MS_CHECK_UNCORRECTED;
      if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.PCC == 0 && RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.TCC == 0) {
        MsCheckField.Field.ValidationBits |= MS_CHECK_PRECISE_IP | MS_CHECK_RESTARTABLE;
        MsCheckField.Field.PreciseIp = 1;
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

    //Report MS check field description
    gBS->CopyMem (&ProcErrInfoStruc->ErrorStructureType[0], &MsCheckGuid, sizeof (EFI_GUID));

    //Update Processor check information
    ProcErrInfoStruc->ValidBits.Field.CheckInfoValid = 1;
    ProcErrInfoStruc->CheckInformation = MsCheckField.Value;

  } else {
    //Unknown error
    gBS->FreePool (GenericProcErrEntry);
    return RETURN_ABORTED;
  }

  if (ProcErrInfoStruc->ValidBits.Field.InstructionPointerValid) {
    //Get SYSENTER_EIP
    RasGetSysEnterEip(MpServices,ProcessorNumber, &SysEnterEip);
    ProcErrInfoStruc->InstructionPointer = SysEnterEip;
  }

  //Update Processor Conext Information
  ProcContextInfoStruc->MSRAddress = MCA_LEGACY_BASE + (BankIndex * 4);  //Ude Legacy MCA address
  ProcContextInfoStruc->RegisterContextType = MSR_REGISTERS;
  ProcContextInfoStruc->RegisterArraySize = (sizeof (UINT64)) * 10;
  ProcContextInfoStruc->RegisterArray[0] = RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaBankNumber;
  gBS->CopyMem (&ProcContextInfoStruc->RegisterArray[1], &RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Value, ProcContextInfoStruc->RegisterArraySize - sizeof (UINT64));

  // Add a new record to the BERT table
  Status = AmdRasApeiProtocol->AddBootErrorRecordEntry ((UINT8*)GenericProcErrEntry,        // IN UINT8* pErrRecord
                                                       GenericProcErrEntrySize,             // IN UINT nSize
                                                       ERROR_TYPE_GENERIC,                  // IN UINT8 ErrorType - GENERIC error type
                                                       ERROR_SEVERITY_FATAL                 // IN UINT8 SeverityType - NON-CORRECTABLE
                                                       );

  gBS->FreePool (GenericProcErrEntry);

  return Status;
}


EFI_STATUS
LogMemError (
  RAS_MCA_ERROR_INFO *RasMcaErrorInfo,
  UINT8 BankIndex
)
{
    EFI_STATUS                    Status = EFI_SUCCESS;
    UINT64                        SystemMemoryAddress;
    NORMALIZED_ADDRESS            NormalizedAddress;
    DIMM_INFO                     DimmInfo;
    GENERIC_MEM_ERR_ENTRY         *GenericMemErrEntry;
    UINT8                         SeverityType;
    EFI_GUID                      MemErrorSectGuid = PLATFORM_MEMORY_SECT_GUID;

    NormalizedAddress.normalizedAddr = RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaAddrMsr.Field.ErrorAddr;
    NormalizedAddress.normalizedSocketId = RasMcaErrorInfo->CpuInfo.SocketId;
    NormalizedAddress.normalizedDieId = RasMcaErrorInfo->CpuInfo.DieId;
    NormalizedAddress.normalizedChannelId = (UINT8)(RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaBankNumber - MCA_UMC0_BANK);
    AmdRasApeiProtocol->McaErrorAddrTranslate (&NormalizedAddress, &SystemMemoryAddress, &DimmInfo);

    Status = gBS->AllocatePool (EfiBootServicesData, sizeof (GENERIC_MEM_ERR_ENTRY), &GenericMemErrEntry);
    gBS->CopyMem (GenericMemErrEntry, &gGenMemErrEntry, sizeof (GENERIC_MEM_ERR_ENTRY));

    //Update Error section GUID
    gBS->CopyMem (&GenericMemErrEntry->GenErrorDataEntry.SectionType[0], &MemErrorSectGuid, sizeof (EFI_GUID));

    GenericMemErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;  // 0x01;
    GenericMemErrEntry->MemErrorSection.ValidBits.Value = MEM_VALID_BIT_MAP;
    GenericMemErrEntry->MemErrorSection.PhyAddr = SystemMemoryAddress;
    GenericMemErrEntry->MemErrorSection.Node = (UINT16) ((NormalizedAddress.normalizedSocketId * 4) + NormalizedAddress.normalizedDieId);
    GenericMemErrEntry->MemErrorSection.Bank = (UINT16) DimmInfo.Bank;
    GenericMemErrEntry->MemErrorSection.Row = (UINT16) DimmInfo.Row;

    GenericMemErrEntry->MemErrorSection.Column = (UINT16) DimmInfo.Column;
    GenericMemErrEntry->MemErrorSection.Module = (UINT16)((NormalizedAddress.normalizedChannelId * 2) + ((DimmInfo.ChipSelect & 0x03) >> 1));
    GenericMemErrEntry->MemErrorSection.Device = (UINT16) DimmInfo.ChipSelect;

    if (Status == EFI_NOT_FOUND) {
      AsciiStrCpy (GenericMemErrEntry->GenErrorDataEntry.FruText, "DIMM# Not Sourced");  // Error Fru Text String
    } else {
      AsciiStrCpy (GenericMemErrEntry->GenErrorDataEntry.FruText, "DIMM# Sourced"); // New Fru Text String
    }
    GenericMemErrEntry->MemErrorSection.ErrStatus = ((1 << 18) | (4 << 8));   // Error Detected on Data Transaction | Internal DRAM Error (0x40400)
    //
    GenericMemErrEntry->MemErrorSection.MemErrType = UNKNOWN;  // not a valid field.

    SeverityType = ERROR_SEVERITY_FATAL;
    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.CECC) {
      SeverityType = ERROR_SEVERITY_CORRECTED;
    }
    GenericMemErrEntry->GenErrorDataEntry.ErrorSeverity = SeverityType;

    // Add a new record to the BERT table
    Status = AmdRasApeiProtocol->AddBootErrorRecordEntry ((UINT8*)GenericMemErrEntry,       // IN UINT8* pErrRecord
                                                         sizeof (GENERIC_MEM_ERR_ENTRY),    // IN UINT nSize
                                                         ERROR_TYPE_GENERIC,                // IN UINT8 ErrorType - GENERIC error type
                                                         SeverityType                       // IN UINT8 SeverityType
                                                         );
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
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
  EFI_STATUS                Status = EFI_SUCCESS;
  GENERIC_MEM_ERR_ENTRY     *MemParityErrEntry;
  EFI_GUID                  MemErrorSectGuid = PLATFORM_MEMORY_SECT_GUID;
  UMC_EXT_ERROR_CODE        UmcExtErrorCode;
  UINT16                    ChannelId;
  UINT16                    ChipSelect;

  UmcExtErrorCode.Value = (UINT8)RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCodeExt;
  ChannelId = (UINT16)(RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaBankNumber - MCA_UMC0_BANK);

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (GENERIC_MEM_ERR_ENTRY), &MemParityErrEntry);
  gBS->CopyMem (MemParityErrEntry, &gGenMemParityErrEntry, sizeof (GENERIC_MEM_ERR_ENTRY));

  //Update Error section GUID
  gBS->CopyMem (&MemParityErrEntry->GenErrorDataEntry.SectionType[0], &MemErrorSectGuid, sizeof (EFI_GUID));

  MemParityErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;  // 0x01;
  MemParityErrEntry->MemErrorSection.ValidBits.Value = MEM_PARITY_VALID_BIT_MAP;
  MemParityErrEntry->MemErrorSection.Node = (UINT16) ((RasMcaErrorInfo->CpuInfo.SocketId * 4) + RasMcaErrorInfo->CpuInfo.DieId);
  if (UmcExtErrorCode.Field.AddrCmdParityErr || UmcExtErrorCode.Field.WriteDataCrcErr) {
    ChipSelect = (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaAddrMsr.Field.ErrorAddr >> 32) & 0x03;
    MemParityErrEntry->MemErrorSection.ValidBits.Field.DeviceValid = 1;
    MemParityErrEntry->MemErrorSection.ValidBits.Field.ModuleValid = 1;

    MemParityErrEntry->MemErrorSection.Device = ChipSelect;
    MemParityErrEntry->MemErrorSection.Module = (ChannelId * 2) + (ChipSelect >> 1);
    DEBUG ((EFI_D_ERROR, "[RAS]Memory CRC/Parity Error Found. Node: %d, UMC Channel: %d, Chip Select: %d\n", MemParityErrEntry->MemErrorSection.Node, ChannelId, ChipSelect));

    AsciiStrCpy (MemParityErrEntry->GenErrorDataEntry.FruText, "DIMM# Sourced"); // New Fru Text String
  } else {
    AsciiStrCpy (MemParityErrEntry->GenErrorDataEntry.FruText, "DIMM# Not Sourced");  // Error Fru Text String
  }

  MemParityErrEntry->MemErrorSection.ErrStatus = ((1 << 18) | (16 << 8));   // Error Detected on Data Transaction | ERR_BUS (0x41000)
  //
  MemParityErrEntry->MemErrorSection.MemErrType = PARITY_ERROR;
  
  // Add a new record to the BERT table
  Status = AmdRasApeiProtocol->AddBootErrorRecordEntry ((UINT8*)MemParityErrEntry,          // IN UINT8* pErrRecord
                                                       sizeof (GENERIC_MEM_ERR_ENTRY),      // IN UINT nSize
                                                       ERROR_TYPE_GENERIC,                  // IN UINT8 ErrorType - GENERIC error type
                                                       ERROR_SEVERITY_FATAL                 // IN UINT8 SeverityType - NON-CORRECTABLE
                                                       );
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
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
  EFI_STATUS                Status = EFI_SUCCESS;
  UMC_EXT_ERROR_CODE        UmcExtErrorCode;

  UmcExtErrorCode.Value = (UINT8)RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCodeExt;
  //Add error record to BERT table.
  if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UECC || RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.CECC) {
    //Has Valid error address?
    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.AddrV) {
      LogMemError (RasMcaErrorInfo, BankIndex);
    } // End-- Error Address != 0?
  }
  else {
    //Decode
    if (UmcExtErrorCode.Field.WriteDataPoisonErr || UmcExtErrorCode.Field.ApbErr){
      LogProcessorError (MpServices, RasMcaErrorInfo, BankIndex, ProcessorNumber);
    }
    if (UmcExtErrorCode.Field.AddrCmdParityErr || UmcExtErrorCode.Field.SdpParityErr || UmcExtErrorCode.Field.WriteDataCrcErr) {
      LogMemParityError (RasMcaErrorInfo, BankIndex);
    }

  }
  return Status;
}

EFI_STATUS
McaErrorDetection ( VOID )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  RAS_MCA_ERROR_INFO            RasMcaErrorInfo;
  UINTN                         ProcessorNumber;
  EFI_MP_SERVICES_PROTOCOL      *MpServices = NULL;
  UINTN                         NumberOfLogicProcessors;
  UINTN                         NumberOfEnabledProcessors;
  UINT8                         BankNum;
  UINT8                         BankIndex;
  UINT64                        MsrData;

  // Get EFI MP service
  if (MpServices == NULL) {
    Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;    // Error detected while trying to locate MP Service protocol
    }
  }

  // Get StartThisApFrom, StartThisApTo, StartThisApSteps
  MpServices->GetNumberOfProcessors (MpServices, &NumberOfLogicProcessors, &NumberOfEnabledProcessors);

  MsrData = AsmReadMsr64 (MSR_MCG_CAP);            // MCG_CAP
  BankNum = (UINT8) (MsrData & 0xFF);
  for (ProcessorNumber = 0; ProcessorNumber < NumberOfLogicProcessors; ProcessorNumber++) {
    ZeroMem (&RasMcaErrorInfo, sizeof (RasMcaErrorInfo));
    RasMcaErrorInfo.CpuInfo.ProcessorNumber = ProcessorNumber;
    AmdRasApeiProtocol->SearchMcaError (&RasMcaErrorInfo);

    for (BankIndex = 0; BankIndex < BankNum; BankIndex++) {
      if (BankIndex == MCA_EMPTY0_BANK) {
        continue;
      }
      //a Valid error return?
      if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val) {
        DEBUG ((EFI_D_ERROR, "[RAS]MCA Error Found in bank : 0x%x\n", RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaBankNumber));
        switch (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaBankNumber) {
        case MCA_LS_BANK:
        case MCA_IF_BANK:
        case MCA_L2_BANK:
        case MCA_DE_BANK:
        case MCA_EX_BANK:
        case MCA_FP_BANK:
          LogProcessorError(MpServices, &RasMcaErrorInfo, BankIndex, ProcessorNumber);
          break;
        case MCA_L3_C0_S0_BANK:
        case MCA_L3_C0_S1_BANK:
        case MCA_L3_C0_S2_BANK:
        case MCA_L3_C0_S3_BANK:
        case MCA_L3_C1_S0_BANK:
        case MCA_L3_C1_S1_BANK:
        case MCA_L3_C1_S2_BANK:
        case MCA_L3_C1_S3_BANK:
          LogProcessorError(MpServices, &RasMcaErrorInfo, BankIndex, ProcessorNumber);
          break;

        case MCA_UMC0_BANK:
        case MCA_UMC1_BANK:
          MemErrorDecode(MpServices, &RasMcaErrorInfo, BankIndex, ProcessorNumber);
          break;
        case MCA_SMU_BANK:
        case MCA_PSP_BANK:
        case MCA_FUSE_BANK:
        case MCA_CS0_BANK:
        case MCA_CS1_BANK:
        case MCA_PIE_BANK:
          LogProcessorError(MpServices, &RasMcaErrorInfo, BankIndex, ProcessorNumber);
          break;
        }
        //Clear MCA Error status registers.
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
  UINT64 SaveHwcr;
  UINT64 MsrData;
  UINT32 McaBankAddrBase;
  MCA_MISC0_MSR McaMisc0Msr;
  MCA_MISC1_MSR McaMisc1Msr;

  McaBankAddrBase = MCA_EXTENSION_BASE + (*McaBank * 0x10);

  // MSR_C001_0015[18][McStatusWrEn] = 1
  SaveHwcr = AsmReadMsr64 (MSR_HWCR);
  MsrData = SaveHwcr | BIT18;
  AsmWriteMsr64 (MSR_HWCR, MsrData);

  AsmWriteMsr64 (McaBankAddrBase | MCA_ADDR_OFFSET, 0);
  AsmWriteMsr64 (McaBankAddrBase | MCA_SYND_OFFSET, 0);
  AsmWriteMsr64 (McaBankAddrBase | MCA_DEADDR_OFFSET, 0);
  AsmWriteMsr64 (McaBankAddrBase | MCA_DESTAT_OFFSET, 0);
  AsmWriteMsr64 (McaBankAddrBase | MCA_STATUS_OFFSET, 0);

  McaMisc0Msr.Value = AsmReadMsr64 (McaBankAddrBase | MCA_MISC0_OFFSET);
  McaMisc0Msr.Field.Ovrflw = 0;
  AsmWriteMsr64 (McaBankAddrBase | MCA_MISC0_OFFSET, McaMisc0Msr.Value);

  if ((MCA_UMC0_BANK == *McaBank) || (MCA_UMC1_BANK == *McaBank)) {
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
  EFI_STATUS    Status = EFI_SUCCESS;
  UINT32        McaBank;
  UINTN         NumberOfEnabledProcessors;
  UINTN         NumberOfLogicProcessors;

  NumberOfLogicProcessors = 0;

  MpServices->GetNumberOfProcessors (MpServices, &NumberOfLogicProcessors, &NumberOfEnabledProcessors);

  if (RasMcaErrorInfo->CpuInfo.ProcessorNumber > NumberOfLogicProcessors) {
    return EFI_INVALID_PARAMETER;
  }

  McaBank = BankIndex;

  //Program BSP first
  if (RasMcaErrorInfo->CpuInfo.ProcessorNumber == 0) {
    McaStsClear (&McaBank);
  } else {
    MpServices->StartupThisAP (
                               MpServices,
                               McaStsClear,
                               RasMcaErrorInfo->CpuInfo.ProcessorNumber,
                               NULL,
                               0,
                               (VOID *) &McaBank ,
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
  UINT32 Index;
  UINT32 NbioParityErrorSts;
  UINT32 BaseAddr;
  UINT32 MaxGroupId;
  switch (RasNbioErrorInfo->TypeId) {
  case NBIO_GRP_UNCORR:
    BaseAddr = PARITY_ERROR_STATUS_UNCORR_GRP;
    MaxGroupId = MAX_UNCORR_GRP_NUM;
    break;
  case NBIO_GRP_CORR:
    BaseAddr = PARITY_ERROR_STATUS_CORR_GRP;
    MaxGroupId = MAX_CORR_GRP_NUM;
    break;
  case NBIO_GRP_UCP:
    BaseAddr = PARITY_ERROR_STATUS_UCP_GRP;
    MaxGroupId = MAX_UCP_GRP_NUM;
  }

  for (Index = 0; Index < MaxGroupId; Index++) {
    RasSmnRead ((UINT32)RasNbioErrorInfo->DieBusNumber,
                BaseAddr + (Index * 4),
                &NbioParityErrorSts);
    if (NbioParityErrorSts != 0) {
      RasNbioErrorInfo->GroupId = (UINT8)Index;
      RasNbioErrorInfo->NbioParityErrorStsAddr = BaseAddr + (Index * 4);
      RasNbioErrorInfo->NbioParityErrorSts = NbioParityErrorSts;
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
  RasSmnWrite ((UINT32)RasNbioErrorInfo->DieBusNumber,
               RasNbioErrorInfo->NbioParityErrorStsAddr,
               &RasNbioErrorInfo->NbioParityErrorSts);

  return EFI_SUCCESS;
}


EFI_STATUS
NbioLogError (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  )
{
  EFI_STATUS               Status = EFI_SUCCESS;
  GENERIC_NBIO_ERR_ENTRY   *GenericNbioErrEntry;
  EFI_GUID                 NbioErrorSectGuid = NBIO_ERROR_SECT_GUID;

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (GENERIC_NBIO_ERR_ENTRY), &GenericNbioErrEntry);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  gBS->CopyMem (GenericNbioErrEntry, &gGenNbioErrEntry, sizeof (GENERIC_NBIO_ERR_ENTRY));

  //Update Error section GUID
  gBS->CopyMem (&GenericNbioErrEntry->GenErrorDataEntry.SectionType[0], &NbioErrorSectGuid, sizeof (EFI_GUID));

  GenericNbioErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;
  GenericNbioErrEntry->NbioErrorSection.ValidationBits.Value = NBIO_VALID_BIT_MAP;
  GenericNbioErrEntry->NbioErrorSection.BusId = RasNbioErrorInfo->DieBusNumber;
  GenericNbioErrEntry->NbioErrorSection.ErrorSource.Field.Nbio = 1;
  GenericNbioErrEntry->NbioErrorSection.ErrorType.Value = (UINT8) (RasNbioErrorInfo->NbioGlobalStatusLo & 0x0000000F);
  GenericNbioErrEntry->NbioErrorSection.GroupId = RasNbioErrorInfo->GroupId;
  GenericNbioErrEntry->NbioErrorSection.GroupType = RasNbioErrorInfo->TypeId;
  GenericNbioErrEntry->NbioErrorSection.ParityErrSts = RasNbioErrorInfo->NbioParityErrorSts;

  // Add a new record to the BERT table
  Status = AmdRasApeiProtocol->AddBootErrorRecordEntry ((UINT8*)GenericNbioErrEntry,          // IN UINT8* pErrRecord
                                                       sizeof (GENERIC_NBIO_ERR_ENTRY),       // IN UINT nSize
                                                       ERROR_TYPE_GENERIC,                   // IN UINT8 ErrorType - GENERIC error type
                                                       ERROR_SEVERITY_FATAL                  // IN UINT8 SeverityType - NON-CORRECTABLE
                                                       );

  gBS->FreePool (GenericNbioErrEntry);
  return Status;
}

EFI_STATUS
NbifLogError (
  UINT8         DieBusNum,
  UINT8         NbifMm,
  UINT8         Leaf,
  NBIO_ERR_SRC  ErrorSource,
  NBIO_ERR_TYPE ErrorType

  )
{
  EFI_STATUS               Status = EFI_SUCCESS;
  GENERIC_NBIO_ERR_ENTRY   *GenericNbioErrEntry;
  EFI_GUID                 NbioErrorSectGuid = NBIO_ERROR_SECT_GUID;

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (GENERIC_NBIO_ERR_ENTRY), &GenericNbioErrEntry);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  gBS->CopyMem (GenericNbioErrEntry, &gGenNbioErrEntry, sizeof (GENERIC_NBIO_ERR_ENTRY));

  //Update Error section GUID
  gBS->CopyMem (&GenericNbioErrEntry->GenErrorDataEntry.SectionType[0], &NbioErrorSectGuid, sizeof (EFI_GUID));

  GenericNbioErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;
  GenericNbioErrEntry->NbioErrorSection.ValidationBits.Value = NBIF_VALID_BIT_MAP;
  GenericNbioErrEntry->NbioErrorSection.BusId = DieBusNum;
  GenericNbioErrEntry->NbioErrorSection.ErrorSource = ErrorSource;
  GenericNbioErrEntry->NbioErrorSection.ErrorType = ErrorType;
  GenericNbioErrEntry->NbioErrorSection.NbifId = NbifMm;
  GenericNbioErrEntry->NbioErrorSection.LeafId = Leaf;

  // Add a new record to the NBIO BERT table
  Status = AmdRasApeiProtocol->AddBootErrorRecordEntry ((UINT8*)GenericNbioErrEntry,          // IN UINT8* pErrRecord
                                                       sizeof (GENERIC_NBIO_ERR_ENTRY),       // IN UINT nSize
                                                       ERROR_TYPE_GENERIC,                   // IN UINT8 ErrorType - GENERIC error type
                                                       ERROR_SEVERITY_FATAL                  // IN UINT8 SeverityType - NON-CORRECTABLE
                                                       );

  gBS->FreePool (GenericNbioErrEntry);

  return Status;
}

EFI_STATUS
NbioErrorDetection ( VOID )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  RAS_NBIO_ERROR_INFO   RasNbioErrorInfo;
  UINT32                NbioGlobalStatusLo;
  UINT32                NbioGlobalStatusHi;
  UINT8                 Die;
  UINT8                 DieBusNum;
  UINT16                VendorID;
  UINTN                 PciAddress;

  //Register Error handler per Die.
  for (Die = 0; Die < MAX_DIE_SUPPORT; Die++) {
    PciAddress = ((Die + 24) << 15) + 0x000;
    VendorID = PciRead16 (PciAddress);
    if ((VendorID != AMD_VENDOR_ID) && (VendorID != HYGON_VID)) {
      continue;
    }

    PciAddress = ((Die + 24) << 15) + (DF_CFGADDRESSCNTL_FUNC << 12) + DF_CFGADDRESSCNTL_OFFSET;
    DieBusNum = PciRead8 (PciAddress);

    //Check NBIO Global Error Status
    RasSmnRead ((UINT32)DieBusNum, RAS_GLOBAL_STATUS_LO, &NbioGlobalStatusLo);
    RasSmnRead ((UINT32)DieBusNum, RAS_GLOBAL_STATUS_HI, &NbioGlobalStatusHi);

    if (((NbioGlobalStatusLo & RAS_GLOBAL_STATUS_LO_MASK) == 0) && ((NbioGlobalStatusHi & RAS_GLOBAL_STATUS_HI_MASK) == 0)) {
      continue;
    }

    //Error found!!
    RasNbioErrorInfo.NbioGlobalStatusLo = NbioGlobalStatusLo;
    RasNbioErrorInfo.NbioGlobalStatusHi = NbioGlobalStatusHi;
    RasNbioErrorInfo.Die = Die;
    RasNbioErrorInfo.DieBusNumber = DieBusNum;

    NbioErrorScan (&RasNbioErrorInfo);
    NbifErrorScan (NbioGlobalStatusHi, DieBusNum);
    PcieErrorScan (NbioGlobalStatusHi, DieBusNum);

  }
  return Status;
}


EFI_STATUS
NbioErrorScan (
  RAS_NBIO_ERROR_INFO   *RasNbioErrorInfo
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  //Scan Error Reporting Status by Type (UNCORR, CORR, UCP)
  //Right now only report first finding. make it loop for all errors.
  if ((RasNbioErrorInfo->NbioGlobalStatusLo & RAS_GLOBAL_STATUS_LO_MASK) != 0) {
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
NbifErrorScan (
  UINT32    NbioGlobalStatusHi,
  UINT8     DieBusNum
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  RAS_LEAF_CTRL_REG     RasLeafCtrl;
  RAS_GLOBAL_STS_HI_REG RasGlobalSts;
  UINT8                 NbifMm;
  UINT8                 SysHubMm;
  UINT8                 Leaf;
  UINT32                BaseAddr;
  NBIO_ERR_SRC          ErrorSource;
  NBIO_ERR_TYPE         ErrorType;

  RasGlobalSts.Value = NbioGlobalStatusHi;

  if (RasGlobalSts.Fields.Nbif0PortA || RasGlobalSts.Fields.Nbif1PortA) {
    DEBUG ((EFI_D_ERROR, "[RAS]NBIF error found!!!\n"));
    //Scan NBIFMM
    ErrorSource.Value = 0;
    ErrorType.Value = 0;
    for (NbifMm = 0; NbifMm < 2; NbifMm++) {
      BaseAddr = NBIF_BIF_RAS_LEAF0_CTRL + (0x100000 * NbifMm);
      for (Leaf = 0; Leaf < 3; Leaf++) {
        RasSmnRead ((UINT32)DieBusNum,
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

          NbifLogError (DieBusNum, NbifMm, Leaf, ErrorSource, ErrorType);
          //Clear Status
          RasSmnWrite ((UINT32)DieBusNum,
                     BaseAddr + (Leaf * 4),
                     &RasLeafCtrl.Value);
        }
      }
    }

    ErrorSource.Value = 0;
    ErrorType.Value = 0;
    //Scan SYSHUBMM
    for (SysHubMm = 0; SysHubMm < 2; SysHubMm++) {
      BaseAddr = NBIF_GDC_RAS_LEAF0_CTRL + (0x100000 * SysHubMm);
      for (Leaf = 0; Leaf < 6; Leaf++) {
        RasSmnRead ((UINT32)DieBusNum,
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
          NbifLogError (DieBusNum, NbifMm, Leaf, ErrorSource, ErrorType);
          //Clear Status
          RasSmnWrite ((UINT32)DieBusNum,
                     BaseAddr + (Leaf * 4),
                     &RasLeafCtrl.Value);
        }
      }
    }
  }

  return Status;
}

EFI_STATUS
PcieErrorLog (
  PCIE_PORT_PROFILE *PciePortProfile,
  IN UINT32         DieBusNum
  )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  PCI_ADDR                    PciPortAddr;
  UINT16                      AerCapPtr;
  UINT8                       PcieCapPtr;
  UINT32                      PcieUncorrStatus;
  UINT16                      PcieDevStatus;
  GENERIC_PCIE_AER_ERR_ENTRY  *GenPcieAerErrEntry;
  PCIE_ERROR_SECTION          *PcieErrorSection;
  UINT32                      Index;
  EFI_GUID                    PcieErrorSectGuid = PCIE_SECT_GUID;

  PciPortAddr.AddressValue = 0;
  PciPortAddr.Address.Bus = PciePortProfile->NbioBusNum;
  PciPortAddr.Address.Device = gPciePortList[PciePortProfile->PciPortNumber].Device;
  PciPortAddr.Address.Function = gPciePortList[PciePortProfile->PciPortNumber].Function;

  AerCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, PCIE_EXT_AER_CAP_ID);
  PcieCapPtr = RasFindPciCapability (PciPortAddr.AddressValue, PCIE_CAP_ID);

  if (AerCapPtr != 0) {
    //Un-Correctable error
    PcieUncorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR);
    if (PcieUncorrStatus != 0) {
      Status = gBS->AllocatePool (EfiBootServicesData, sizeof (GENERIC_PCIE_AER_ERR_ENTRY), &GenPcieAerErrEntry);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      gBS->CopyMem (GenPcieAerErrEntry, &gGenPcieErrEntry, sizeof (GENERIC_PCIE_AER_ERR_ENTRY));

      //Update Error section GUID
      gBS->CopyMem (&GenPcieAerErrEntry->GenErrorDataEntry.SectionType[0], &PcieErrorSectGuid, sizeof (EFI_GUID));

      PcieErrorSection = &GenPcieAerErrEntry->PcieAerErrorSection;

      GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;

      PcieErrorSection->Validation.Value = 0xEF;
      PcieErrorSection->PortType = (UINT32)RasGetPcieDeviceType (PciPortAddr);
      PcieErrorSection->Revision = 0x02;
      PcieErrorSection->CommandStatus = PciRead32 (PciPortAddr.AddressValue + 0x04);

      PcieErrorSection->DeviceId.VendorId = PciRead16 (PciPortAddr.AddressValue + 0x00);
      PcieErrorSection->DeviceId.DeviceId = PciRead16 (PciPortAddr.AddressValue + 0x02);
      PcieErrorSection->DeviceId.ClassCode[0] = PciRead8 (PciPortAddr.AddressValue + 0x09);
      PcieErrorSection->DeviceId.ClassCode[1] = PciRead8 (PciPortAddr.AddressValue + 0x0A);
      PcieErrorSection->DeviceId.ClassCode[2] = PciRead8 (PciPortAddr.AddressValue + 0x0B);
      PcieErrorSection->DeviceId.Function = gPciePortList[PciePortProfile->PciPortNumber].Function;
      PcieErrorSection->DeviceId.Device = gPciePortList[PciePortProfile->PciPortNumber].Device;
      PcieErrorSection->DeviceId.Segment = 0x00;
      PcieErrorSection->DeviceId.PrimaryBus = (UINT8)PciPortAddr.Address.Bus;
      PcieErrorSection->DeviceId.SecondaryBus = PciRead8 (PciPortAddr.AddressValue + 0x19);
      PcieErrorSection->DeviceId.Slot = PciePortProfile->PciPortNumber;

      PcieErrorSection->BridgeCtrlStatus = (UINT32) (PciRead16 (PciPortAddr.AddressValue + 0x3E)) << 16 | (UINT32) (PciRead16 (PciPortAddr.AddressValue + 0x1E));

      for (Index = 0; Index < 15; Index++) {
        PcieErrorSection->CapabilityStructure.CapabilityData[Index] = \
                                              PciRead32 (PciPortAddr.AddressValue + (0x58 + (4 * Index)));
      }

      for (Index = 0; Index < 24; Index++) {
        PcieErrorSection->AerInfo.AerInfoData[Index] = PciRead32 (PciPortAddr.AddressValue + (AerCapPtr + (4 * Index)));
      }

      // Add a new record to the BERT table
      Status = AmdRasApeiProtocol->AddBootErrorRecordEntry ((UINT8*)GenPcieAerErrEntry,          // IN UINT8* pErrRecord
                                                           sizeof (GENERIC_PCIE_AER_ERR_ENTRY),       // IN UINT nSize
                                                           ERROR_TYPE_GENERIC,                   // IN UINT8 ErrorType - GENERIC error type
                                                           ERROR_SEVERITY_FATAL                  // IN UINT8 SeverityType - NON-CORRECTABLE
                                                           );
      
      gBS->FreePool(GenPcieAerErrEntry);
    }
  }

  //Clear Status register
  if (PcieCapPtr != 0) {
    PcieDevStatus = PciRead16 (PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_STATUS_PTR);
    PciWrite16 (PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_STATUS_PTR, PcieDevStatus);
  }

  if (AerCapPtr != 0) {
    PciWrite32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR, PcieUncorrStatus);
  }

  return Status;
}

EFI_STATUS
PcieErrorScan (
  UINT32 NbioGlobalStatusHi,
  UINT8 DieBusNum
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  UINT32                PcieGlobalStatus;
  UINT16                PciePortIndex;
  PCIE_PORT_PROFILE     *PciePortProfileInstance;

  PcieGlobalStatus = NbioGlobalStatusHi & 0xFFFF;

  PciePortProfileInstance = mPlatformApeiPrivate->AmdPciePortMap->PciPortNumber;

  //Search active PCI-E port for error only
  for (PciePortIndex = 0; PciePortIndex < mPlatformApeiPrivate->AmdPciePortMap->PortCount; PciePortIndex++, PciePortProfileInstance++) {
    if (PciePortProfileInstance->NbioBusNum != DieBusNum) {
      //find next
      continue;
    }
    if ((PcieGlobalStatus & (1 << PciePortProfileInstance->PciPhysicalPortNumber)) != 0) {
      DEBUG ((EFI_D_ERROR, "PCI-E Error detected at Bus: 0x%x, physical Port: %0d\n", DieBusNum, PciePortProfileInstance->PciPhysicalPortNumber));
      //PCI-E error detected
      PcieErrorLog (PciePortProfileInstance,DieBusNum);
    }
  }

  return Status;
}

EFI_STATUS
SmnErrorDetection ( VOID )
{
  EFI_STATUS            Status;
  GENERIC_SMN_ERR_ENTRY *GenericSmnErrEntry;
  UINT32                PmBreakEvent;
  UINT32                PmBreakEvent_Raw;
  UINT8                 Die;
  UINT8                 DieBusNum;
  UINT16                VendorID;
  UINTN                 PciAddress;
  UINTN                 Index;
  EFI_GUID              SmnErrorSectGuid = SMN_ERROR_SECT_GUID;

  Status = gBS->AllocatePool (
      EfiBootServicesData,
      sizeof (GENERIC_SMN_ERR_ENTRY),
      &GenericSmnErrEntry
      );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //Check for SMN error per Die.
  for (Die = 0; Die < MAX_DIE_SUPPORT; Die++) {
    PciAddress = ((Die + 24) << 15) + 0x000;
    VendorID = PciRead16 (PciAddress);
    if ((VendorID != AMD_VENDOR_ID) && (VendorID != HYGON_VID)) {
      continue;
    }

    PciAddress = ((Die + 24) << 15) + (DF_CFGADDRESSCNTL_FUNC << 12) +
      DF_CFGADDRESSCNTL_OFFSET;
    DieBusNum = PciRead8 (PciAddress);

    //Check SMN PM BreakEvent Status
    RasSmnRead ((UINT32)DieBusNum, PMIO_SMN_BREAK_EVENT, &PmBreakEvent);
    PmBreakEvent_Raw = PmBreakEvent;

    // Keep only SMN related bits
    PmBreakEvent &= SMN_CATEGORY_MASK;
    if (PmBreakEvent == 0) {
      continue;
    }

    //Error found!!
    for (Index = 0; Index <= (UINTN)HighBitSet32 (SMN_CATEGORY_MASK); Index ++) {
      if ((PmBreakEvent & (1 << Index)) != 0) {
        gBS->CopyMem (GenericSmnErrEntry, &gGenSmnErrEntry, sizeof (GENERIC_SMN_ERR_ENTRY));

        //Update Error section GUID
        gBS->CopyMem (&GenericSmnErrEntry->GenErrorDataEntry.SectionType[0], &SmnErrorSectGuid, sizeof (EFI_GUID));

        GenericSmnErrEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;
        GenericSmnErrEntry->SmnErrorSection.ValidationBits.Value = SMN_VALID_BIT_MAP;
        GenericSmnErrEntry->SmnErrorSection.BusId = DieBusNum;
        GenericSmnErrEntry->SmnErrorSection.Category = SMN_CATEGORY_FATAL;
        GenericSmnErrEntry->SmnErrorSection.ErrorSource.Value = (UINT8) (PmBreakEvent & (1 << Index));

        // Add a new record to the SMN BERT table
        Status = AmdRasApeiProtocol->AddBootErrorRecordEntry (
            (UINT8*)GenericSmnErrEntry,           // IN UINT8* pErrRecord
            sizeof (GENERIC_SMN_ERR_ENTRY),        // IN UINT nSize
            ERROR_TYPE_GENERIC,                   // IN UINT8 ErrorType - GENERIC error type
            ERROR_SEVERITY_FATAL                  // IN UINT8 SeverityType - NON-CORRECTABLE
            );
      }
    }
    // Clear any SMN sources in PM BreakEvent register
    PmBreakEvent_Raw &= ~SMN_CATEGORY_MASK;
    RasSmnWrite ((UINT32)DieBusNum, PMIO_SMN_BREAK_EVENT, &PmBreakEvent_Raw);
  }
  gBS->FreePool (GenericSmnErrEntry);

  return Status;
}
