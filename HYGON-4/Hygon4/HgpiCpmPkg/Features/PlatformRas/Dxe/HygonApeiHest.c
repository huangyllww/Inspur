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
#include "HygonPlatformRasDxe.h"
#include "HygonApeiHest.h"
#include "Protocol/HygonAcpiRasServicesProtocol.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
extern PLATFORM_APEI_PRIVATE_BUFFER  *mPlatformApeiPrivate;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
HYGON_MC_BANK_INIT_DATA  McBankInitData[] = {
  { 0xFFFFFFFFFFFFFFFF, 0xC0002000, 0xC0002001, 0xC0002002, 0xC0002003 },      // Bank 0
  { 0xFFFFFFFFFFFFFFFF, 0xC0002010, 0xC0002011, 0xC0002012, 0xC0002013 },      // Bank 1
  { 0xFFFFFFFFFFFFFFFF, 0xC0002020, 0xC0002021, 0xC0002022, 0xC0002023 },      // Bank 2
  { 0xFFFFFFFFFFFFFFFF, 0xC0002030, 0xC0002031, 0xC0002032, 0xC0002033 },      // Bank 3
  { 0x0000000000000000, 0xC0002040, 0xC0002041, 0xC0002042, 0xC0002043 },      // Bank 4 unused
  { 0xFFFFFFFFFFFFFFFF, 0xC0002050, 0xC0002051, 0xC0002052, 0xC0002053 },      // Bank 5
  { 0xFFFFFFFFFFFFFFFF, 0xC0002060, 0xC0002061, 0xC0002062, 0xC0002063 },      // Bank 6
  { 0xFFFFFFFFFFFFFFFF, 0xC0002070, 0xC0002071, 0xC0002072, 0xC0002073 },      // Bank 7
  { 0xFFFFFFFFFFFFFFFF, 0xC0002080, 0xC0002081, 0xC0002082, 0xC0002083 },      // Bank 8
  { 0xFFFFFFFFFFFFFFFF, 0xC0002090, 0xC0002091, 0xC0002092, 0xC0002093 },      // Bank 9
  { 0xFFFFFFFFFFFFFFFF, 0xC00020A0, 0xC00020A1, 0xC00020A2, 0xC00020A3 },      // Bank 10
  { 0xFFFFFFFFFFFFFFFF, 0xC00020B0, 0xC00020B1, 0xC00020B2, 0xC00020B3 },      // Bank 11
  { 0xFFFFFFFFFFFFFFFF, 0xC00020C0, 0xC00020C1, 0xC00020C2, 0xC00020C3 },      // Bank 12
  { 0xFFFFFFFFFFFFFFFF, 0xC00020D0, 0xC00020D1, 0xC00020D2, 0xC00020D3 },      // Bank 13
  { 0xFFFFFFFFFFFFFFFF, 0xC00020E0, 0xC00020E1, 0xC00020E2, 0xC00020E3 },      // Bank 14
  { 0xFFFFFFFFFFFFFFFF, 0xC00020F0, 0xC00020F1, 0xC00020F2, 0xC00020F3 },      // Bank 15
  { 0xFFFFFFFFFFFFFFFF, 0xC0002100, 0xC0002101, 0xC0002102, 0xC0002103 },      // Bank 16
  { 0xFFFFFFFFFFFFFFFF, 0xC0002110, 0xC0002111, 0xC0002112, 0xC0002113 },      // Bank 17
  { 0xFFFFFFFFFFFFFFFF, 0xC0002120, 0xC0002121, 0xC0002122, 0xC0002123 },      // Bank 18
  { 0xFFFFFFFFFFFFFFFF, 0xC0002130, 0xC0002131, 0xC0002132, 0xC0002133 },      // Bank 19
  { 0xFFFFFFFFFFFFFFFF, 0xC0002140, 0xC0002141, 0xC0002142, 0xC0002143 },      // Bank 20
  { 0xFFFFFFFFFFFFFFFF, 0xC0002150, 0xC0002151, 0xC0002152, 0xC0002153 },      // Bank 21
  { 0xFFFFFFFFFFFFFFFF, 0xC0002160, 0xC0002161, 0xC0002162, 0xC0002163 },      // Bank 22
  { 0xFFFFFFFFFFFFFFFF, 0xC0002170, 0xC0002171, 0xC0002172, 0xC0002173 },      // Bank 23
  { 0xFFFFFFFFFFFFFFFF, 0xC0002180, 0xC0002181, 0xC0002182, 0xC0002183 },      // Bank 24
  { 0xFFFFFFFFFFFFFFFF, 0xC0002190, 0xC0002191, 0xC0002192, 0xC0002193 },      // Bank 25
  { 0xFFFFFFFFFFFFFFFF, 0xC00021A0, 0xC00021A1, 0xC00021A2, 0xC00021A3 },      // Bank 26
  { 0xFFFFFFFFFFFFFFFF, 0xC00021B0, 0xC00021B1, 0xC00021B2, 0xC00021B3 },      // Bank 27
  { 0xFFFFFFFFFFFFFFFF, 0xC00021C0, 0xC00021C1, 0xC00021C2, 0xC00021C3 },      // Bank 28
  { 0xFFFFFFFFFFFFFFFF, 0xC00021D0, 0xC00021D1, 0xC00021D2, 0xC00021D3 },      // Bank 29
  { 0xFFFFFFFFFFFFFFFF, 0xC00021E0, 0xC00021E1, 0xC00021E2, 0xC00021E3 },      // Bank 30
  { 0xFFFFFFFFFFFFFFFF, 0xC00021F0, 0xC00021F1, 0xC00021F2, 0xC00021F3 },      // Bank 31
  { 0xFFFFFFFFFFFFFFFF, 0xC0002200, 0xC0002201, 0xC0002202, 0xC0002203 },      // Bank 32
  { 0xFFFFFFFFFFFFFFFF, 0xC0002210, 0xC0002211, 0xC0002212, 0xC0002213 },      // Bank 33
  { 0xFFFFFFFFFFFFFFFF, 0xC0002220, 0xC0002221, 0xC0002222, 0xC0002223 },      // Bank 34
  { 0xFFFFFFFFFFFFFFFF, 0xC0002230, 0xC0002231, 0xC0002232, 0xC0002233 },      // Bank 35
  { 0xFFFFFFFFFFFFFFFF, 0xC0002240, 0xC0002241, 0xC0002242, 0xC0002243 },      // Bank 36
  { 0xFFFFFFFFFFFFFFFF, 0xC0002250, 0xC0002251, 0xC0002252, 0xC0002253 },      // Bank 37
  { 0xFFFFFFFFFFFFFFFF, 0xC0002260, 0xC0002261, 0xC0002262, 0xC0002263 },      // Bank 38
  { 0xFFFFFFFFFFFFFFFF, 0xC0002270, 0xC0002271, 0xC0002272, 0xC0002273 },      // Bank 39
  { 0xFFFFFFFFFFFFFFFF, 0xC0002280, 0xC0002281, 0xC0002282, 0xC0002283 },      // Bank 40
  { 0xFFFFFFFFFFFFFFFF, 0xC0002290, 0xC0002291, 0xC0002292, 0xC0002293 },      // Bank 41
  { 0xFFFFFFFFFFFFFFFF, 0xC00022A0, 0xC00022A1, 0xC00022A2, 0xC00022A3 },      // Bank 42
  { 0xFFFFFFFFFFFFFFFF, 0xC00022B0, 0xC00022B1, 0xC00022B2, 0xC00022B3 },      // Bank 43
  { 0xFFFFFFFFFFFFFFFF, 0xC00022C0, 0xC00022C1, 0xC00022C2, 0xC00022C3 },      // Bank 44
  { 0xFFFFFFFFFFFFFFFF, 0xC00022D0, 0xC00022D1, 0xC00022D2, 0xC00022D3 },      // Bank 45
};

HYGON_MC_INIT_DATA  McaInitData = {
  0x0000000000000000,       // HygonGlobCapInitData
  0x000000000001FFEF,       // HygonGlobCtrlInitData
  0x00,                     // HygonMcbClrStatusOnInit
  0x02,                     // HygonMcbStatusDataFormat
  0x00,                     // Reserved
  (sizeof (McBankInitData) / sizeof (McBankInitData[0])), // McBankNum
  &McBankInitData[0]   // Pointer to Initial data of MC Bank
};

STATIC EFI_ACPI_6_0_HARDWARE_ERROR_SOURCE_TABLE_HEADER  HestHeaderStruct =
{
  { EFI_ACPI_6_0_HARDWARE_ERROR_SOURCE_TABLE_SIGNATURE,
      0,
      ACPI_REV1,
      0,
      { 'H',                                               'Y','G', 'O', 'N', ' ' },
      HYGON_HEST_TABLE_ID,
      1,
      HYGON_CREATOR_ID,
   1
  },
  0
};

STATIC EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE  CmcErrorNofication =
{
  HARDWARE_ERROR_NOTIFICATION_POLLED,                            // UINT8  NotifiyType = Polled
  sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE),   // UINT8  ErrNotifyLen
  { 0, 0, 0, 0, 0, 0, 0 },                                       // UINT16 ConfigWrite
  5000,                                                          // UINT32 PollInterval = 5000 ms
  0,                                                             // UINT32 Interrupt Vector
  0,                                                             // UINT32 SwitchPollingThreshVal
  0,                                                             // UINT32 SwitchPollingThreshWindow
  1,                                                             // UINT32 ErrThresholdVal;
  0                                                              // UINT32 ErrThresholdWindow
};

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern  EFI_BOOT_SERVICES  *gBS;

/*----------------------------------------------------------------------------------------
 *                          T A B L E    D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 *---------------------------------------------------------------------------------------
 *
 *  ApeiHestInit
 *
 *  Description:
 *     Install ACPI APEI HEST table.
 *
 *  Parameters:
 *    @retval        EFI_SUCCESS
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS
ApeiHestInstall (
  VOID
  )
{
  EFI_STATUS                                                         Status;
  UINT8                                                              BankNum;
  UINT64                                                             MsrData;
  UINT8                                                              *HestTableEnd;
  UINT32                                                             HestTableSize;
  UINT8                                                              BankIndex;
  UINTN                                                              TableKey;
  EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_EXCEPTION_STRUCTURE   *MceStruc;
  EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_ERROR_BANK_STRUCTURE  *McBankStruc;
  HYGON_MC_BANK_INIT_DATA                                            *HygonMcBankInitData;
  EFI_ACPI_6_0_IA32_ARCHITECTURE_CORRECTED_MACHINE_CHECK_STRUCTURE   *CmcStruc;
  EFI_ACPI_6_0_HARDWARE_ERROR_SOURCE_TABLE_HEADER                    *HestTableHdr;

  HygonMcBankInitData = McaInitData.McBankInitData;
  MsrData = AsmReadMsr64 (MSR_MCG_CAP);            // MCG_CAP
  BankNum = (UINT8)(((MSR_MCG_CAP_STRUCT *)&MsrData)->Count);

  DEBUG ((DEBUG_INFO, "McaInitData.McBankNum: 0x%x, McaBankNum: 0x%x\n", McaInitData.McBankNum, BankNum));
  if (McaInitData.McBankNum < BankNum) {
    DEBUG ((DEBUG_ERROR, "Error, McaInitData.McBankNum < McaBankNum\n"));
  }
  // ASSERT (McaInitData.McBankNum <= BankNum);

  HestTableSize = sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_SOURCE_TABLE_HEADER) + \
                  sizeof (EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_EXCEPTION_STRUCTURE) + \
                  (sizeof (EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_ERROR_BANK_STRUCTURE) * BankNum) + \
                  sizeof (EFI_ACPI_6_0_IA32_ARCHITECTURE_CORRECTED_MACHINE_CHECK_STRUCTURE) + \
                  (sizeof (EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_ERROR_BANK_STRUCTURE) * BankNum);

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  HestTableSize,
                  &HestTableHdr
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  } else {
    // clear instances content
    gBS->SetMem (HestTableHdr, HestTableSize, 0);
  }

  gBS->CopyMem (&HestHeaderStruct.Header.OemTableId, (UINT64 *)PcdGetPtr (PcdHygonAcpiHestTableHeaderOemTableId), sizeof (UINT64));
  gBS->CopyMem (
         HestTableHdr,
         &HestHeaderStruct,
         sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_SOURCE_TABLE_HEADER)
         );

  ((EFI_ACPI_DESCRIPTION_HEADER *)(&HestTableHdr->Header))->Length = HestTableSize;
  HestTableEnd = (UINT8 *)HestTableHdr;

  HestTableEnd       = (UINT8 *)HestTableHdr + sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_SOURCE_TABLE_HEADER);
  MceStruc           = (EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_EXCEPTION_STRUCTURE *)HestTableEnd;
  MceStruc->Type     = EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_EXCEPTION;
  MceStruc->SourceId = MCE_SOURCE_ID0;
  MceStruc->Flags    = 0;
  MceStruc->Enabled  = 1;
  MceStruc->NumberOfRecordsToPreAllocate = RECORDS_PREALLOCATE;
  MceStruc->MaxSectionsPerRecord     = RECORDS_PREALLOCATE;
  MceStruc->GlobalCapabilityInitData = McaInitData.GlobCapInitData;
  MceStruc->GlobalControlInitData    = McaInitData.GlobCtrlInitData;
  MceStruc->NumberOfHardwareBanks    = BankNum;

  // Point to Machine Check Bank Structure
  HestTableEnd = HestTableEnd + sizeof (EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_EXCEPTION_STRUCTURE);

  McBankStruc = (EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_ERROR_BANK_STRUCTURE *)HestTableEnd;

  for (BankIndex = 0; BankIndex < BankNum; BankIndex++) {
    McBankStruc->BankNumber = BankIndex;
    McBankStruc->ClearStatusOnInitialization = McaInitData.ClrStatusOnInit;
    McBankStruc->StatusDataFormat = McaInitData.StatusDataFormat;
    McBankStruc->ControlRegisterMsrAddress = HygonMcBankInitData->CtrlRegMSRAddr;
    McBankStruc->ControlInitData = HygonMcBankInitData->CtrlInitData;
    McBankStruc->StatusRegisterMsrAddress  = HygonMcBankInitData->StatRegMSRAddr;
    McBankStruc->AddressRegisterMsrAddress = HygonMcBankInitData->AddrRegMSRAddr;
    McBankStruc->MiscRegisterMsrAddress    = HygonMcBankInitData->MiscRegMSRAddr;
    McBankStruc++;         // Point to next bank
    HygonMcBankInitData++; // Point to next init data
  }

  HestTableHdr->ErrorSourceCount++;

  // Update HestTable point;
  HestTableEnd = (UINT8 *)McBankStruc;

  // Init Corrected Machine Check Structure
  CmcStruc = (EFI_ACPI_6_0_IA32_ARCHITECTURE_CORRECTED_MACHINE_CHECK_STRUCTURE *)HestTableEnd;
  CmcStruc->Type     = EFI_ACPI_6_0_IA32_ARCHITECTURE_CORRECTED_MACHINE_CHECK;
  CmcStruc->SourceId = CMC_SOURCE_ID0;
  CmcStruc->Flags    = 0;
  CmcStruc->Enabled  = 1;
  CmcStruc->NumberOfRecordsToPreAllocate = RECORDS_PREALLOCATE;
  CmcStruc->MaxSectionsPerRecord = SECTIONS_PER_RECORD;
  gBS->CopyMem (
         &CmcStruc->NotificationStructure,
         &CmcErrorNofication,
         sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE)
         );

  CmcStruc->NotificationStructure.ErrorThresholdValue = mPlatformApeiPrivate->PlatRasPolicy.McaErrThreshSwCount;         // byo231101 -
  CmcStruc->NotificationStructure.PollInterval = mPlatformApeiPrivate->PlatRasPolicy.McaPollInterval;

  // reset McBankInitData point
  HygonMcBankInitData = McaInitData.McBankInitData;
  CmcStruc->NumberOfHardwareBanks = BankNum;

  HestTableEnd = HestTableEnd + sizeof (EFI_ACPI_6_0_IA32_ARCHITECTURE_CORRECTED_MACHINE_CHECK_STRUCTURE);

  // Reuse McBankStruc pointer and setup Corrected Machine Check Bank Structure.
  McBankStruc = (EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_ERROR_BANK_STRUCTURE *)HestTableEnd;

  for (BankIndex = 0; BankIndex < BankNum; BankIndex++) {
    McBankStruc->BankNumber = BankIndex;
    McBankStruc->ClearStatusOnInitialization = McaInitData.ClrStatusOnInit;
    McBankStruc->StatusDataFormat = McaInitData.StatusDataFormat;
    McBankStruc->ControlRegisterMsrAddress = HygonMcBankInitData->CtrlRegMSRAddr;
    McBankStruc->ControlInitData = HygonMcBankInitData->CtrlInitData;
    McBankStruc->StatusRegisterMsrAddress  = HygonMcBankInitData->StatRegMSRAddr;
    McBankStruc->AddressRegisterMsrAddress = HygonMcBankInitData->AddrRegMSRAddr;
    McBankStruc->MiscRegisterMsrAddress    = HygonMcBankInitData->MiscRegMSRAddr;
    McBankStruc++;         // Point to next bank
    HygonMcBankInitData++; // Point to next init data
  }

  HestTableHdr->ErrorSourceCount++;

  // Update HEST header Checksum
  ChecksumAcpiTable ((EFI_ACPI_DESCRIPTION_HEADER *)HestTableHdr);

  TableKey = 0;
  AcpiTableProtocol->InstallAcpiTable (
                       AcpiTableProtocol,
                       HestTableHdr,
                       HestTableSize,
                       &TableKey
                       );

  gBS->FreePool (HestTableHdr);

  return Status;
}
