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
#include "AmdApeiEinj.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

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
APEI_EINJ_ACPI_TABLE mEinjTable = {

  {
    //AcpiHeader
    {
      EFI_ACPI_6_0_ERROR_INJECTION_TABLE_SIGNATURE,
      sizeof (APEI_EINJ_ACPI_TABLE),
      ACPI_REV1,
      0,
      {'H','Y','G','O','N',' '},
      AMD_EINJ_TABLE_ID,
      1,
      AMD_CREATOR_ID,
      1
    },
    //EINJ Serialization Header
    sizeof (EFI_ACPI_6_0_ERROR_INJECTION_TABLE_HEADER), // Injection Header Size
    EINJ_RESERVED,                                      // Injection Flags = RESERVED
    {
      EINJ_RESERVED,                                    // RESERVED
      EINJ_RESERVED,                                    // RESERVED
      EINJ_RESERVED                                     // RESERVED
    },
    EINJ_INSTR_ENTRIES                                  // EINJ_INSTR_ENTRIES

  },
  {
    //EinjBeginInject
    {
      EFI_ACPI_6_0_EINJ_BEGIN_INJECTION_OPERATION,      // Injection Action
      EFI_ACPI_6_0_EINJ_WRITE_REGISTER_VALUE,           // Instruction
      EINJ_CLEAR_FLAG,                                  // Flags
      EINJ_RESERVED,                                    // Reserved
      { // Generic Address Region
        EFI_ACPI_6_0_SYSTEM_MEMORY,                     // AddressSpaceID = System Memory
        REG_BIT_WIDTH_QWORD,                            // Register Bit Width
        REG_BIT_OFFSET0,                                // Register Bit Offset
        EFI_ACPI_6_0_QWORD,                             // Access Size
        0                                               // Address
      },
      0,                                                // Value
      EINJ_QWORD_MASK                                   // Mask
    },

    //EinjGetTriggerErrActionTable
    {
      EFI_ACPI_6_0_EINJ_GET_TRIGGER_ERROR_ACTION_TABLE, // Injection Action
      EFI_ACPI_6_0_EINJ_READ_REGISTER,                  // Instruction
      EINJ_CLEAR_FLAG,                                  // Flags
      EINJ_RESERVED,                                    // Reserved
      { // Generic Address Region
        EFI_ACPI_6_0_SYSTEM_MEMORY,                     // AddressSpaceID = System Memory
        REG_BIT_WIDTH_QWORD,                            // Register Bit Width
        REG_BIT_OFFSET0,                                // Register Bit Offset
        EFI_ACPI_6_0_QWORD,                             // Access Size
        0                                               // Address
      },
      0,                                                // Value
      EINJ_QWORD_MASK                                   // Mask
    },

    //EinjSetErrorType
    {
      EFI_ACPI_6_0_EINJ_SET_ERROR_TYPE,                 // Injection Action
      EFI_ACPI_6_0_EINJ_WRITE_REGISTER,                 // Instruction
      EINJ_CLEAR_FLAG,                                  // Flags
      EINJ_RESERVED,                                    // Reserved
      { // Generic Address Region
        EFI_ACPI_6_0_SYSTEM_MEMORY,                     // AddressSpaceID = System Memory
        REG_BIT_WIDTH_QWORD,                            // Register Bit Width
        REG_BIT_OFFSET0,                                // Register Bit Offset
        EFI_ACPI_6_0_QWORD,                             // Access Size
        0                                               // Address
      },
      0,                                                // Value
      EINJ_QWORD_MASK                                   // Mask
    },

    //EinjGetErrorType
    {
      EFI_ACPI_6_0_EINJ_GET_ERROR_TYPE,                 // Injection Action
      EFI_ACPI_6_0_EINJ_READ_REGISTER,                  // Instruction
      EINJ_CLEAR_FLAG,                                  // Flags
      EINJ_RESERVED,                                    // Reserved
      { // Generic Address Region
        EFI_ACPI_6_0_SYSTEM_MEMORY,                     // AddressSpaceID = System Memory
        REG_BIT_WIDTH_QWORD,                            // Register Bit Width
        REG_BIT_OFFSET0,                                // Register Bit Offset
        EFI_ACPI_6_0_QWORD,                             // Access Size
        0                                               // Address
      },
      0,                                                // Value
      EINJ_QWORD_MASK                                   // Mask
    },

    //EinjEndOperation
    {
      EFI_ACPI_6_0_EINJ_END_OPERATION,                  // Injection Action
      EFI_ACPI_6_0_EINJ_WRITE_REGISTER_VALUE,           // Instruction
      EINJ_CLEAR_FLAG,                                  // Flags
      EINJ_RESERVED,                                    // Reserved
      { // Generic Address Region
        EFI_ACPI_6_0_SYSTEM_MEMORY,                     // AddressSpaceID = System Memory
        REG_BIT_WIDTH_QWORD,                            // Register Bit Width
        REG_BIT_OFFSET0,                                // Register Bit Offset
        EFI_ACPI_6_0_QWORD,                             // Access Size
        0                                               // Address
      },
      0,                                                // Value
      EINJ_QWORD_MASK                                   // Mask
    },

    //EinjExecOperation
    {
      EFI_ACPI_6_0_EINJ_EXECUTE_OPERATION,              // Injection Action
      EFI_ACPI_6_0_EINJ_WRITE_REGISTER_VALUE,           // Instruction
      EINJ_CLEAR_FLAG,                                  // Flags
      EINJ_RESERVED,                                    // Reserved
      { // Generic Address Region
        EFI_ACPI_6_0_SYSTEM_IO,                         // AddressSpaceID = System IO
        REG_BIT_WIDTH_BYTE,                             // Register Bit Width
        REG_BIT_OFFSET0,                                // Register Bit Offset
        EFI_ACPI_6_0_BYTE,                              // Access Size
        0                                               // Address, SwSmiCmdPort address update in runtime
      },
      0,                                                // Value, SwSmiCmd data update in runtime
      EINJ_BYTE_MASK                                    // Mask
    },

    //EinjCheckBusyStatus
    {
      EFI_ACPI_6_0_EINJ_CHECK_BUSY_STATUS,              // Injection Action
      EFI_ACPI_6_0_EINJ_READ_REGISTER_VALUE,            // Instruction
      EINJ_CLEAR_FLAG,                                  // Flags
      EINJ_RESERVED,                                    // Reserved
      { // Generic Address Region
        EFI_ACPI_6_0_SYSTEM_MEMORY,                     // AddressSpaceID = System Memory
        REG_BIT_WIDTH_QWORD,                            // Register Bit Width
        REG_BIT_OFFSET0,                                // Register Bit Offset
        EFI_ACPI_6_0_QWORD,                             // Access Size
        0                                               // Address
      },
      0x01,                                             // Value
      EINJ_CHK_BUSY_STS_MASK                            // Mask
    },

    //EinjGetCommandStatus
    {
      EFI_ACPI_6_0_EINJ_GET_COMMAND_STATUS,             // UINT8 SerializationAction - Injection Action
      EFI_ACPI_6_0_EINJ_READ_REGISTER,                  // UINT8 Instruction
      EINJ_CLEAR_FLAG,                                  // UINT8 Flags
      EINJ_RESERVED,                                    // UINT8 Reserved
      { // Generic Address Region
        EFI_ACPI_6_0_SYSTEM_MEMORY,                     // AddressSpaceID = System Memory
        REG_BIT_WIDTH_QWORD,                            // Register Bit Width
        REG_BIT_OFFSET0,                                // Register Bit Offset
        EFI_ACPI_6_0_QWORD,                             // Access Size
        0                                               // UINT64 Address
      },
      0,                                                // UINT64 Value
      EINJ_GET_CMD_STS_MASK                             // UINT64 Mask
    },

    //EinjSetErrorTypeWithAddress
    {
      EINJ_SET_ERROR_TYPE_WITH_ADDRESS,                 // UINT8 SerializationAction - Injection Action
      EFI_ACPI_6_0_EINJ_WRITE_REGISTER,                 // UINT8 Instruction
      EINJ_CLEAR_FLAG,                                  // UINT8 Flags
      EINJ_RESERVED,                                    // UINT8 Reserved
      { // Generic Address Region
        EFI_ACPI_6_0_SYSTEM_MEMORY,                     // AddressSpaceID = System Memory
        REG_BIT_WIDTH_QWORD,                            // Register Bit Width
        REG_BIT_OFFSET0,                                // Register Bit Offset
        EFI_ACPI_6_0_QWORD,                             // Access Size
        0                                               // UINT64 Address
      },
      0,                                                // UINT64 Value
      EINJ_QWORD_MASK                                   // UINT64 Mask
    }
  }
};

// Trigger Error Action Table
TRIG_ERROR_ACTION  mTrigErrActTable = \
{
  {
    sizeof (EFI_ACPI_6_0_EINJ_TRIGGER_ACTION_TABLE),    // UINT32 HeaderSize
    0,                                                  // UINT32 Revsion
    sizeof (TRIG_ERROR_ACTION),                         // UINT32 TableSize
    EINJ_TRIG_INSTR_ENTRIES                             // UINT32 Instruction EntryCount
  },
  {
    { //APEI_SERIALIZATION_INST_ENTRY[0]
      EFI_ACPI_6_0_EINJ_TRIGGER_ERROR,                  // UINT8 SerializationAction - Injection Action
      EFI_ACPI_6_0_EINJ_WRITE_REGISTER_VALUE,           // UINT8 Instruction
      EINJ_SET_FLAG,                                    // UINT8 Flags, 1 = PRESERVE_REGISTER
      EINJ_RESERVED,                                    // UINT8 Reserved
      { // Generic Address Region
        EFI_ACPI_6_0_SYSTEM_MEMORY,                     // UINT8 AddressSpaceID = System Memory
        REG_BIT_WIDTH_QWORD,                            // UINT8 Register Bit Width
        REG_BIT_OFFSET0,                                // UINT8 Register Bit Offset
        EFI_ACPI_6_0_QWORD,                             // UINT8 Access Size
        0x100000000                                     // UINT64 Address
      },
      0,                                                // UINT64 Value
      0                                                 // UINT64 Mask
    }
  }
};



/**
 *---------------------------------------------------------------------------------------
 *
 *  ApeiEinjInit
 *
 *  Description:
 *     Initialize ACPI APEI EINJ table.
 *
 *  Parameters:
 *    @retval        EFI_SUCCESS
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS
ApeiEinjInstall ( VOID )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  APEI_EINJ_ACPI_TABLE  *ApeiEinjAcpiTable;
  UINTN                 TableKey;
  APEI_EINJ_REGISTERS   *EinjActMemRegAddr;
  UINT8                 *TrigErrorAct;
  SET_ERR_TYPE_WITH_ADDR *SetErrTypeWithAddr;

  //
  //Reserve EFI memory buffer for Einj serial instruction
  //
  Status = gBS->AllocatePool (EfiReservedMemoryType, sizeof (APEI_EINJ_REGISTERS), &EinjActMemRegAddr);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  //Reserve EFI memory buffer for Einj SET_ERROR_TYPE_WITH_ADDRESS action command structure
  //
  Status = gBS->AllocatePool (EfiReservedMemoryType, sizeof (SET_ERR_TYPE_WITH_ADDR), &SetErrTypeWithAddr);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (SetErrTypeWithAddr, sizeof (SET_ERR_TYPE_WITH_ADDR));
  EinjActMemRegAddr->EinjSetErrTypeWithAddrReg = (UINT64)(UINTN)SetErrTypeWithAddr;

  DEBUG ((EFI_D_ERROR, "[RAS]Set Error Type With Address structure locate: 0x%08x", RShiftU64(EinjActMemRegAddr->EinjSetErrTypeWithAddrReg, 32)));
  DEBUG ((EFI_D_ERROR, "%08x\n", EinjActMemRegAddr->EinjSetErrTypeWithAddrReg & 0xFFFFFFFF));

  mEinjTable.InjectionInstEntry[0].RegisterRegion.Address  = (UINT64) ((UINTN) &EinjActMemRegAddr->EinjBeginInjReg);
  mEinjTable.InjectionInstEntry[1].RegisterRegion.Address  = (UINT64) ((UINTN) &EinjActMemRegAddr->EinjGetTriActTblReg);
  mEinjTable.InjectionInstEntry[2].RegisterRegion.Address  = (UINT64) ((UINTN) &EinjActMemRegAddr->EinjSetErrTypeReg);
  mEinjTable.InjectionInstEntry[3].RegisterRegion.Address  = (UINT64) ((UINTN) &EinjActMemRegAddr->EinjGetErrTypeReg);
  mEinjTable.InjectionInstEntry[4].RegisterRegion.Address  = (UINT64) ((UINTN) &EinjActMemRegAddr->EinjEndOperReg);
  mEinjTable.InjectionInstEntry[6].RegisterRegion.Address  = (UINT64) ((UINTN) &EinjActMemRegAddr->EinjChkBusyStsReg);
  mEinjTable.InjectionInstEntry[7].RegisterRegion.Address  = (UINT64) ((UINTN) &EinjActMemRegAddr->EinjCmdStsReg);
  mEinjTable.InjectionInstEntry[8].RegisterRegion.Address  = (UINT64) ((UINTN) EinjActMemRegAddr->EinjSetErrTypeWithAddrReg);

  //Update EinjExecOperation Software SMIcommand port address and command data
  mEinjTable.InjectionInstEntry[5].RegisterRegion.Address = mPlatformApeiPrivate->PlatRasPolicy.SwSmiCmdPortAddr;
  mEinjTable.InjectionInstEntry[5].Value = (UINT64)mPlatformApeiPrivate->PlatRasPolicy.EinjSwSmiData;

  // Set error injection capabilities bitmask for this platform
  EinjActMemRegAddr->EinjGetErrTypeReg = EFI_ACPI_6_0_EINJ_ERROR_PROCESSOR_CORRECTABLE | EFI_ACPI_6_0_EINJ_ERROR_PROCESSOR_UNCORRECTABLE_NONFATAL| EFI_ACPI_6_0_EINJ_ERROR_PROCESSOR_UNCORRECTABLE_FATAL;
  //EinjActMemRegAddr->EinjGetErrTypeReg = EFI_ACPI_6_0_EINJ_ERROR_MEMORY_CORRECTABLE | EFI_ACPI_6_0_EINJ_ERROR_MEMORY_UNCORRECTABLE_NONFATAL| EFI_ACPI_6_0_EINJ_ERROR_MEMORY_UNCORRECTABLE_FATAL;
//  EinjActMemRegAddr->EinjGetErrTypeReg = EFI_ACPI_6_0_EINJ_ERROR_MEMORY_CORRECTABLE | EFI_ACPI_6_0_EINJ_ERROR_MEMORY_UNCORRECTABLE_FATAL | EFI_ACPI_6_0_EINJ_ERROR_PLATFORM_CORRECTABLE | EFI_ACPI_6_0_EINJ_ERROR_PLATFORM_UNCORRECTABLE_FATAL;

  //Allocate EFI Boot Services memory space for EINJ table
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (APEI_EINJ_ACPI_TABLE), &ApeiEinjAcpiTable);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->CopyMem (&mEinjTable.InjectionHeader.Header.OemTableId, (UINT64 *)PcdGetPtr (PcdAmdAcpiEinjTableHeaderOemTableId), sizeof (UINT64));
  // Copy local EINJ table to EFI Boot Services memory buffer
  gBS->CopyMem (ApeiEinjAcpiTable, &mEinjTable, sizeof (APEI_EINJ_ACPI_TABLE));

  // Allocate ACPI NVS memory for Trigger Error Action Table
  Status = gBS->AllocatePool (EfiReservedMemoryType, sizeof (TRIG_ERROR_ACTION), (VOID **) &TrigErrorAct);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Copy local Trigger Error Action table to ACPI NVS memory space
  gBS->CopyMem (TrigErrorAct, &mTrigErrActTable, sizeof (TRIG_ERROR_ACTION));

  // Update EINJ header Checksum
  ChecksumAcpiTable ((EFI_ACPI_DESCRIPTION_HEADER *) ApeiEinjAcpiTable);

  TableKey = 0;
  AcpiTableProtocol->InstallAcpiTable (
                  AcpiTableProtocol,
                  ApeiEinjAcpiTable,
                  sizeof (APEI_EINJ_ACPI_TABLE),
                  &TableKey
                  );

  gBS->FreePool (ApeiEinjAcpiTable);

  //Update EINJ action interustion register buffer pointer,
  //Trigger Error Injection Action table pointer to APEI private buffer.
  mPlatformApeiPrivate->EinjActMemRegAddr = EinjActMemRegAddr;
  mPlatformApeiPrivate->TrigErrorAct = TrigErrorAct;

  return Status;
}

