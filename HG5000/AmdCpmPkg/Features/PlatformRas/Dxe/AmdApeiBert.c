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
#include "AmdPlatformRasDxe.h"
#include "AmdApeiBert.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


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
EFI_ACPI_6_0_BOOT_ERROR_RECORD_TABLE_HEADER BertAcpiTable = {
  //EFI_ACPI_DESCRIPTION_HEADER
  { BERT_SIG,
    sizeof (EFI_ACPI_6_0_BOOT_ERROR_RECORD_TABLE_HEADER),
    ACPI_REV1,
    0,
    {'H','Y','G','O','N',' '},
    AMD_BERT_TABLE_ID,
    1,
    AMD_CREATOR_ID,
    1
  },
  //  APEI_BERT_CONTEXT
  sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE),              // UINT32 nBootErrRegLen - Size of Boot Error Region Table
  0                                                               // UNIT64 BootErrRegPtr - Address of Boot Error Region Table Address
};

EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE ApeiBootErrRegTbl = {
  {0,0,0,0,0},                                                      // UINT32 BlockStatus - WHEA Platform Design Guide Table 3-2 Block Status
  sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE),                // UINT32 RawDataOffset - WHEA Platform Design Guide Table 3-2 Raw Data Offset
  0,                                                                // UINT32 RawDataLength - WHEA Platform Design Guide Table 3-2 Raw Data Offset
  0,                                                                // UINT32 DataLength - WHEA Platform Design Guide Table 3-2 Raw Data Offset
  0x03                                                              // UINT32 ErrorSeverity - WHEA Platform Design Guide Table 3-2 Raw Data Offset (Initialize Severity to [None] = 0x03)
};

/**
 *---------------------------------------------------------------------------------------
 *
 *  ApeiBertInstall
 *
 *  Description:
 *    Allocate ACPI NVS memory for Boot error region table and provides the pointer to BERT
 *    ACPI table interface.  Copy BERT ACPI table to Boot Services memory space. Updates the ACPI
 *    table address in APEI private data structure which is shared by a protocol interface.
 *
 *  Parameters:
 *    @retval     EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS
ApeiBertInstall ( VOID )
{
  EFI_STATUS                                    Status = EFI_SUCCESS;
  UINT8                                         *BootErrRgnTbl;
  EFI_ACPI_6_0_BOOT_ERROR_RECORD_TABLE_HEADER   *ApeiBertAcpiTable;
  UINTN                                         TableKey;

  //
  // Reserve EFI ACPI NVS Memory for Boot Error Region Table
  //
//Linux complains not accessible region  
//###  Status = gBS->AllocatePool (EfiACPIMemoryNVS, sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE), &BootErrRgnTbl);
  Status = gBS->AllocatePool (EfiReservedMemoryType, sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE), &BootErrRgnTbl);
  // Copy Boot Error Region Table into EFI ACPI NVS Memory
  gBS->CopyMem (BootErrRgnTbl, &ApeiBootErrRegTbl, sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE));
  // Assign  Boot Error Region pointer
  BertAcpiTable.BootErrorRegion = (UINT64) ((UINTN) BootErrRgnTbl);

  //
  // Reserve EFI Boot Services Memory for APEI BERT Table
  //
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (EFI_ACPI_6_0_BOOT_ERROR_RECORD_TABLE_HEADER), &ApeiBertAcpiTable);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  gBS->CopyMem (&BertAcpiTable.Header.OemTableId, (UINT64 *)PcdGetPtr (PcdAmdAcpiBertTableHeaderOemTableId), sizeof (UINT64));
  gBS->CopyMem (ApeiBertAcpiTable, &BertAcpiTable, sizeof (EFI_ACPI_6_0_BOOT_ERROR_RECORD_TABLE_HEADER));

  // Update BERT header Checksum
  ChecksumAcpiTable ((EFI_ACPI_DESCRIPTION_HEADER *) ApeiBertAcpiTable);

  TableKey = 0;
  AcpiTableProtocol->InstallAcpiTable (
                  AcpiTableProtocol,
                  ApeiBertAcpiTable,
                  sizeof (EFI_ACPI_6_0_BOOT_ERROR_RECORD_TABLE_HEADER),
                  &TableKey
                  );

  gBS->FreePool (ApeiBertAcpiTable);

  return EFI_SUCCESS;
}

