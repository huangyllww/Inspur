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
#include "AmdApeiHest.h"

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
AMD_MC_BANK_INIT_DATA McBankInitData[] = {
  {0xFFFFFFFFFFFFFFFF,0xC0002000,0xC0002001,0xC0002002,0xC0002003},      //Bank 0
  {0xFFFFFFFFFFFFFFFF,0xC0002010,0xC0002011,0xC0002012,0xC0002013},      //Bank 1
  {0xFFFFFFFFFFFFFFFF,0xC0002020,0xC0002021,0xC0002022,0xC0002023},      //Bank 2
  {0xFFFFFFFFFFFFFFFF,0xC0002030,0xC0002031,0xC0002032,0xC0002033},      //Bank 3
  {0x0000000000000000,0xC0002040,0xC0002041,0xC0002042,0xC0002043},      //Bank 4 unused
  {0xFFFFFFFFFFFFFFFF,0xC0002050,0xC0002051,0xC0002052,0xC0002053},      //Bank 5
  {0xFFFFFFFFFFFFFFFF,0xC0002060,0xC0002061,0xC0002062,0xC0002063},      //Bank 6
  {0xFFFFFFFFFFFFFFFF,0xC0002070,0xC0002071,0xC0002072,0xC0002073},      //Bank 7
  {0xFFFFFFFFFFFFFFFF,0xC0002080,0xC0002081,0xC0002082,0xC0002083},      //Bank 8
  {0xFFFFFFFFFFFFFFFF,0xC0002090,0xC0002091,0xC0002092,0xC0002093},      //Bank 9
  {0xFFFFFFFFFFFFFFFF,0xC00020A0,0xC00020A1,0xC00020A2,0xC00020A3},      //Bank 10
  {0xFFFFFFFFFFFFFFFF,0xC00020B0,0xC00020B1,0xC00020B2,0xC00020B3},      //Bank 11
  {0xFFFFFFFFFFFFFFFF,0xC00020C0,0xC00020C1,0xC00020C2,0xC00020C3},      //Bank 12
  {0xFFFFFFFFFFFFFFFF,0xC00020D0,0xC00020D1,0xC00020D2,0xC00020D3},      //Bank 13
  {0xFFFFFFFFFFFFFFFF,0xC00020E0,0xC00020E1,0xC00020E2,0xC00020E3},      //Bank 14
  {0xFFFFFFFFFFFFFFFF,0xC00020F0,0xC00020F1,0xC00020F2,0xC00020F3},      //Bank 15
  {0xFFFFFFFFFFFFFFFF,0xC0002100,0xC0002101,0xC0002102,0xC0002103},      //Bank 16
  {0xFFFFFFFFFFFFFFFF,0xC0002110,0xC0002111,0xC0002112,0xC0002113},      //Bank 17
  {0xFFFFFFFFFFFFFFFF,0xC0002120,0xC0002121,0xC0002122,0xC0002123},      //Bank 18
  {0xFFFFFFFFFFFFFFFF,0xC0002130,0xC0002131,0xC0002132,0xC0002133},      //Bank 19
  {0xFFFFFFFFFFFFFFFF,0xC0002140,0xC0002141,0xC0002142,0xC0002143},      //Bank 20
  {0xFFFFFFFFFFFFFFFF,0xC0002150,0xC0002151,0xC0002152,0xC0002153},      //Bank 21
  {0xFFFFFFFFFFFFFFFF,0xC0002160,0xC0002161,0xC0002162,0xC0002163},      //Bank 22
};

AMD_MC_INIT_DATA McaInitData = {
  0x0000000000000000,       // AmdGlobCapInitData
  0x000000000001FFEF,       // AmdGlobCtrlInitData
  0x00,                     // AmdMcbClrStatusOnInit
  0x02,                     // AmdMcbStatusDataFormat
  0x00,                     // Reserved
  (sizeof (McBankInitData) / sizeof (McBankInitData[0])), // McBankNum
  &McBankInitData[0]   // Pointer to Initial data of MC Bank
};

STATIC EFI_ACPI_6_0_HARDWARE_ERROR_SOURCE_TABLE_HEADER HestHeaderStruct =
{
  {EFI_ACPI_6_0_HARDWARE_ERROR_SOURCE_TABLE_SIGNATURE,
   0,
   ACPI_REV1,
   0,
   {'H','Y','G','O','N',' '},
   AMD_HEST_TABLE_ID,
   1,
   AMD_CREATOR_ID,
   1
  },
  0
};

STATIC EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE CmcErrorNofication =
{
  HARDWARE_ERROR_NOTIFICATION_POLLED,                            // UINT8  NotifiyType = Polled
  sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE),   // UINT8  ErrNotifyLen
  {0,0,0,0,0,0,0},                                               // UINT16 ConfigWrite
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
extern  EFI_BOOT_SERVICES       *gBS;



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
ApeiHestInstall ( VOID )
{
  EFI_STATUS Status;
  UINT8  BankNum;
  UINT64 MsrData;
  UINT8* HestTableEnd;
  UINT32 HestTableSize;
  UINT8 BankIndex;
  UINTN TableKey;
  EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_EXCEPTION_STRUCTURE* MceStruc;
  EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_ERROR_BANK_STRUCTURE* McBankStruc;
  AMD_MC_BANK_INIT_DATA* AmdMcBankInitData;
  EFI_ACPI_6_0_IA32_ARCHITECTURE_CORRECTED_MACHINE_CHECK_STRUCTURE* CmcStruc;
  EFI_ACPI_6_0_HARDWARE_ERROR_SOURCE_TABLE_HEADER* HestTableHdr;

  AmdMcBankInitData = McaInitData.McBankInitData;
  MsrData = AsmReadMsr64 (MSR_MCG_CAP);            // MCG_CAP
  BankNum = (UINT8) (((MSR_MCG_CAP_STRUCT*)&MsrData)->Count);

  DEBUG ((EFI_D_ERROR, "McBankNum: 0x%x, BankNum: 0x%x\n", McaInitData.McBankNum, BankNum));
  ASSERT (McaInitData.McBankNum <= BankNum);

  HestTableSize = sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_SOURCE_TABLE_HEADER) + \
                  sizeof (EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_EXCEPTION_STRUCTURE) + \
                  (sizeof (EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_ERROR_BANK_STRUCTURE) * McaInitData.McBankNum) + \
                  sizeof (EFI_ACPI_6_0_IA32_ARCHITECTURE_CORRECTED_MACHINE_CHECK_STRUCTURE) + \
                  (sizeof (EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_ERROR_BANK_STRUCTURE) * McaInitData.McBankNum);

  Status = gBS->AllocatePool (EfiBootServicesData,
                              HestTableSize ,
                              &HestTableHdr);
  if (EFI_ERROR (Status)) {
    return Status;
  } else {
    //clear instances content
    gBS->SetMem (HestTableHdr, HestTableSize, 0);
  }

  gBS->CopyMem (&HestHeaderStruct.Header.OemTableId, (UINT64 *)PcdGetPtr (PcdAmdAcpiHestTableHeaderOemTableId), sizeof (UINT64));
  gBS->CopyMem (HestTableHdr,
               &HestHeaderStruct,
               sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_SOURCE_TABLE_HEADER)
               );

  ((EFI_ACPI_DESCRIPTION_HEADER*) (&HestTableHdr->Header))->Length = HestTableSize;
  HestTableEnd = (UINT8*)HestTableHdr;

  HestTableEnd = (UINT8*)HestTableHdr + sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_SOURCE_TABLE_HEADER);
  MceStruc = (EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_EXCEPTION_STRUCTURE*)HestTableEnd;
  MceStruc->Type = EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_EXCEPTION;
  MceStruc->SourceId = MCE_SOURCE_ID0;
  MceStruc->Flags = 0;
  MceStruc->Enabled = 1;
  MceStruc->NumberOfRecordsToPreAllocate = RECORDS_PREALLOCATE;
  MceStruc->MaxSectionsPerRecord = RECORDS_PREALLOCATE;
  MceStruc->GlobalCapabilityInitData = McaInitData.GlobCapInitData;
  MceStruc->GlobalControlInitData = McaInitData.GlobCtrlInitData;
  MceStruc->NumberOfHardwareBanks = McaInitData.McBankNum;

  //Point to Machine Check Bank Structure
  HestTableEnd = HestTableEnd + sizeof (EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_EXCEPTION_STRUCTURE);

  McBankStruc = (EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_ERROR_BANK_STRUCTURE*)HestTableEnd;

  for (BankIndex = 0; BankIndex < McaInitData.McBankNum; BankIndex++) {
    McBankStruc->BankNumber = BankIndex;
    McBankStruc->ClearStatusOnInitialization = McaInitData.ClrStatusOnInit;
    McBankStruc->StatusDataFormat = McaInitData.StatusDataFormat;
    McBankStruc->ControlRegisterMsrAddress = AmdMcBankInitData->CtrlRegMSRAddr;
    McBankStruc->ControlInitData = AmdMcBankInitData->CtrlInitData;
    McBankStruc->StatusRegisterMsrAddress = AmdMcBankInitData->StatRegMSRAddr;
    McBankStruc->AddressRegisterMsrAddress = AmdMcBankInitData->AddrRegMSRAddr;
    McBankStruc->MiscRegisterMsrAddress = AmdMcBankInitData->MiscRegMSRAddr;
    McBankStruc++;        //Point to next bank
    AmdMcBankInitData++;  //Point to next init data
  }

  HestTableHdr->ErrorSourceCount++;

  //Update HestTable point;
  HestTableEnd = (UINT8*)McBankStruc;

  //Init Corrected Machine Check Structure
  CmcStruc = (EFI_ACPI_6_0_IA32_ARCHITECTURE_CORRECTED_MACHINE_CHECK_STRUCTURE*)HestTableEnd;
  CmcStruc->Type = EFI_ACPI_6_0_IA32_ARCHITECTURE_CORRECTED_MACHINE_CHECK;
  CmcStruc->SourceId = CMC_SOURCE_ID0;
  CmcStruc->Flags = 0;
  CmcStruc->Enabled = 1;
  CmcStruc->NumberOfRecordsToPreAllocate = RECORDS_PREALLOCATE;
  CmcStruc->MaxSectionsPerRecord = SECTIONS_PER_RECORD;
  gBS->CopyMem (&CmcStruc->NotificationStructure,
               &CmcErrorNofication,
               sizeof (EFI_ACPI_6_0_HARDWARE_ERROR_NOTIFICATION_STRUCTURE)
               );

  CmcStruc->NotificationStructure.ErrorThresholdValue = 0xFFF - mPlatformApeiPrivate->PlatRasPolicy.McaErrThreshCount;
  CmcStruc->NotificationStructure.PollInterval = mPlatformApeiPrivate->PlatRasPolicy.McaPollInterval;

  //reset McBankInitData point
  AmdMcBankInitData = McaInitData.McBankInitData;
  CmcStruc->NumberOfHardwareBanks = McaInitData.McBankNum;

  HestTableEnd = HestTableEnd + sizeof (EFI_ACPI_6_0_IA32_ARCHITECTURE_CORRECTED_MACHINE_CHECK_STRUCTURE);

  //Reuse McBankStruc pointer and setup Corrected Machine Check Bank Structure.
  McBankStruc = (EFI_ACPI_6_0_IA32_ARCHITECTURE_MACHINE_CHECK_ERROR_BANK_STRUCTURE*)HestTableEnd;

  for (BankIndex = 0; BankIndex < McaInitData.McBankNum; BankIndex++) {
    McBankStruc->BankNumber = BankIndex;
    McBankStruc->ClearStatusOnInitialization = McaInitData.ClrStatusOnInit;
    McBankStruc->StatusDataFormat = McaInitData.StatusDataFormat;
    McBankStruc->ControlRegisterMsrAddress = AmdMcBankInitData->CtrlRegMSRAddr;
    McBankStruc->ControlInitData = AmdMcBankInitData->CtrlInitData;
    McBankStruc->StatusRegisterMsrAddress = AmdMcBankInitData->StatRegMSRAddr;
    McBankStruc->AddressRegisterMsrAddress = AmdMcBankInitData->AddrRegMSRAddr;
    McBankStruc->MiscRegisterMsrAddress = AmdMcBankInitData->MiscRegMSRAddr;
    McBankStruc++;  //Point to next bank
    AmdMcBankInitData++;  //Point to next init data
  }

  HestTableHdr->ErrorSourceCount++;

  // Update HEST header Checksum
  ChecksumAcpiTable ((EFI_ACPI_DESCRIPTION_HEADER *) HestTableHdr);

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
