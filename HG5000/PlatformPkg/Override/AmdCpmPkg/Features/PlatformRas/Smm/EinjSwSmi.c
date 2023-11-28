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
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include "AmdPlatformRasSmm.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define PARITY_UNCORRECTABLE_GROUP  0
#define PARITY_CORRECTABLE_GROUP    1
#define PARITY_ECC_GROUP            2

#define PARITY_READ_Error_Injection 2
#define DDP_Error_Injection         5
#define UCP_Error_Injection         0xA
#define DIE                         0

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
UINT64  gSysMemAddr;
UINT8   mBusNumForType4Einj = 0xFF;


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
BspMcaThresholdInit (
  IN       UINT8 McaBankNumber
  );

UINT16
MemEccErrInject (
  IN       UINT16 EccInjVector,
  IN       UINT8  DieNum,
  IN       UINT8  ChannelId,
  IN       UINT8  ChipSelect,
  IN       UINT64 SysMemAddr
  );

VOID
LogMCAError (
  IN       RAS_MCA_ERROR_INFO   *RasMcaErrorInfo
  );

VOID
McaThresholdInit (
  IN       RAS_MCA_ERROR_INFO   *RasMcaErrorInfo,
  IN       UINT8                BankIndex
  );

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          T A B L E    D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */
NBIO_PARITY_ERROR_TABLE gNbioParityErrorTable [] = {
  {PARITY_CORRECTABLE_GROUP, 12, 0, PARITY_READ_Error_Injection}
//  Add errors as needed. Below are the values from PPR
//  {PARITY_CORRECTABLE_GROUP, 12, 1, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 13, 0, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 13, 1, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 14, 0, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 14, 1, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 15, 0, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 15, 1, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 0, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 1, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 2, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 3, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 4, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 5, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 6, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 7, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 8, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 9, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 10, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 11, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 12, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 13, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 14, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 15, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 16, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 17, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 18, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 16, 19, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 17, 0, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 17, 1, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 17, 2, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 17, 3, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 17, 4, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 17, 5, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 17, 6, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 17, 7, PARITY_READ_Error_Injection},
//  UNCORRECTABLE
//  {PARITY_UNCORRECTABLE_GROUP, 12, 0, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 12, 1, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 12, 2, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 13, 0, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 13, 1, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 13, 2, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 13, 3, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 13, 4, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 13, 5, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 13, 6, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 13, 7, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 13, 8, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 13, 9, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 13, 10, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 13, 11, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 13, 12, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 13, 13, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 14, 0, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 14, 1, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 14, 2, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 15, 0, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 15, 1, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 15, 2, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 15, 3, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 15, 4, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 15, 5, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 15, 6, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 15, 7, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 15, 8, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 15, 9, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 15, 10, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 15, 11, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 15, 12, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 15, 13, PARITY_READ_Error_Injection},
//ECC Structures
//  {PARITY_CORRECTABLE_GROUP, 0, 0, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 0, 1, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 0, 2, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 0, 3, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 0, 4, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 0, 5, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 1, 0, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 1, 1, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 1, 2, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 1, 3, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 1, 4, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 1, 5, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 2, 0, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 3, 0, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 4, 0, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 5, 0, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 6, 0, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 6, 1, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 7, 0, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 7, 1, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 8, 0, PARITY_READ_Error_Injection},
// ECC + UCP
//  {PARITY_CORRECTABLE_GROUP, 2, 1, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 3, 1, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 4, 1, PARITY_READ_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 5, 1, PARITY_READ_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 2, 1, UCP_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 3, 1, UCP_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 4, 1, UCP_Error_Injection},
//  {PARITY_UNCORRECTABLE_GROUP, 5, 1, UCP_Error_Injection},
// DDP + UCP
//  {PARITY_CORRECTABLE_GROUP, 2, 2, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 3, 2, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 4, 2, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 5, 2, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 6, 2, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 7, 2, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 7, 3, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 7, 4, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 8, 2, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 8, 3, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 8, 4, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 9, 0, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 9, 1, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 10, 0, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 10, 1, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 11, 0, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 11, 1, DDP_Error_Injection},
//  {PARITY_CORRECTABLE_GROUP, 11, 2, DDP_Error_Injection}
};

UINT8
GetNBIOBusNum(UINT8 DieNum)
{
  UINT8                   DieBusNum;
  UINTN                   PciAddress;
  // Get IOHC Bus number based on Die.
  PciAddress = ((DieNum + 24) << 15) + (DF_CFGADDRESSCNTL_FUNC << 12) + DF_CFGADDRESSCNTL_OFFSET;
  DieBusNum = PciRead8 (PciAddress);
  DEBUG ((EFI_D_ERROR, "[RAS] Error inject to IOHC bus number : 0x%x\n", DieBusNum));

  return DieBusNum;
}

typedef struct {
    BOOLEAN Done;
    BOOLEAN Overflow;
    UINT32 ApicId;
    UINT8 Bank;
    BOOLEAN Pfeh;
    MCA_STATUS_MSR McaStatus;
    MCA_SYND_MSR McaSynd;
} MC_BANK_STATE_T;

UINT32
GetLocalApicId(VOID)
{
  UINT32 Eax, Ebx, Ecx, Edx;
  
  AsmCpuid (0x00000001, &Eax, &Ebx, &Ecx, &Edx);
  return ((Ebx & 0xFF000000) >> 24);
}

VOID
SetMcaBankAp (
  IN       VOID *Buffer
  )
{
  MC_BANK_STATE_T *McBankState;
  UINT64 SaveHwcr;
  UINT32 Msr;
  MCA_MISC0_MSR Misc0;
  UINT64 Count;
  
  McBankState = (MC_BANK_STATE_T *)Buffer;
  if (McBankState->ApicId != GetLocalApicId()) {
      return;
  }
  // MSR_C001_0015[18][McStatusWrEn] = 1
  SaveHwcr = AsmReadMsr64 (MSR_HWCR);
  AsmWriteMsr64 (MSR_HWCR, SaveHwcr | BIT18);
  
  Msr = MCA_EXTENSION_BASE + MCA_EXTENSION_REG_PER_BANK * McBankState->Bank + MCA_STATUS_OFFSET;
  AsmWriteMsr64 (Msr, McBankState->McaStatus.Value);
  Msr = MCA_EXTENSION_BASE + MCA_EXTENSION_REG_PER_BANK * McBankState->Bank + MCA_SYND_OFFSET;
  AsmWriteMsr64 (Msr, McBankState->McaSynd.Value);
  
  if (McBankState->McaStatus.Field.UC == 0) {
      AsmMsrOr64 (MSR_MCG_STAT, MSR_MCG_RIPV);
  }
  
  Msr = MCA_EXTENSION_BASE + MCA_EXTENSION_REG_PER_BANK * McBankState->Bank + MCA_MISC0_OFFSET;
  Misc0.Value = AsmReadMsr64 (Msr);
  if (Misc0.Field.CntEn) {
      Count = Misc0.Field.ErrCnt;
      Count++;
      Misc0.Field.ErrCnt = Count;
      if (Count == 0xFFF) {
          Misc0.Field.Ovrflw = 1;
          McBankState->Overflow = TRUE;
      }
      AsmWriteMsr64 (Msr, Misc0.Value);
  }
  
  // Restore MSR_C001_0015[18][McStatusWrEn]
  AsmWriteMsr64 (MSR_HWCR, SaveHwcr);
  
  McBankState->Done = TRUE;
}

VOID
SetMcaBank (
  IN MC_BANK_STATE_T *McBankState
  )
{
  UINT16          i;

  McBankState->Done = FALSE;
  McBankState->Overflow = FALSE;
  SetMcaBankAp (McBankState);
  if (McBankState->Done) {
      return;
  }

  for (i = 1; i < gSmst->NumberOfCpus; i++) {
    gSmst->SmmStartupThisAp (
            SetMcaBankAp,
            i,
            McBankState
            );
    if (McBankState->Done) {
        return;
    }
  }
}

UINT16
EFIAPI
AmdSmiEinjExecuteActionProcessor (VOID)
{
  APEI_EINJ_REGISTERS *EinjActMemRegAddr;
  TRIG_ERROR_ACTION   *TrigErrorAct;
  UINT64  ErrorType;
  UINT16  OperationStatus;
  SET_ERR_TYPE_WITH_ADDR  *SetErrTypeWithAddr;
  UINT32 ApicId;
  UINT8 Bank;
  MC_BANK_STATE_T McBankState;
  UINT64 PfehCfg;
  UINT8 PwrRsrCfg;

  DEBUG ((EFI_D_ERROR, "EINJ SMI Execute Action for Processor Error\n"));

  EinjActMemRegAddr = (APEI_EINJ_REGISTERS*)mPlatformApeiData->EinjActMemRegAddr;
  TrigErrorAct = (TRIG_ERROR_ACTION*)mPlatformApeiData->TrigErrorAct;

  // Initialize Local Variables
  OperationStatus = EFI_ACPI_6_0_EINJ_STATUS_UNKNOWN_FAILURE;

  // Get Injection error type

  //Init default value.
  ApicId = 0;
  Bank = 22;

  SetErrTypeWithAddr = (SET_ERR_TYPE_WITH_ADDR*)(UINTN)EinjActMemRegAddr->EinjSetErrTypeWithAddrReg;
  ErrorType = SetErrTypeWithAddr->ErrorType;

  if (ErrorType != 0) {
    DEBUG ((EFI_D_ERROR, "EINJ Set Error Type With Address command accepted!!\n"));
    if ((SetErrTypeWithAddr->Flags & SETWA_FLAGS_APICID) != 0) {
      ApicId = SetErrTypeWithAddr->Apicid;
    }
  }
  else {
    ErrorType = EinjActMemRegAddr->EinjSetErrTypeReg;
  }
  
  //
  // Execute error injection based upon requested Error Type
  //
  switch (ErrorType)
  {
  case EFI_ACPI_6_0_EINJ_ERROR_PROCESSOR_CORRECTABLE:
    DEBUG ((EFI_D_ERROR, "Inject processor correctable error to CPU(APIC ID: 0x%x), Bank %d\n", ApicId, Bank));

    PfehCfg = AsmReadMsr64 (MSR_PFEH_CFG);
    
    //Inject correctable error
    McBankState.McaStatus.Value = 0;
    McBankState.McaStatus.Field.Val = 1;
    McBankState.McaStatus.Field.MiscV = 1;
    McBankState.McaStatus.Field.SyndV = 1;
    McBankState.McaStatus.Field.ErrorCode = 1 << INT_ERROR_CHK_SHIFT;
    McBankState.McaStatus.Field.ErrorCodeExt = MCA_SYND_TYPE_INTERNAL_ERR;
    McBankState.McaSynd.Value = 0;
    McBankState.McaSynd.Field.ErrorInformation = (1 << 2) | 0x1;
    
    McBankState.ApicId = ApicId;
    McBankState.Bank = Bank;
    McBankState.Pfeh = (PfehCfg & BIT0) ? TRUE : FALSE;
    SetMcaBank(&McBankState);
    
    //9. Public action table
    if ((McBankState.Overflow) && ((PfehCfg & BIT0) != 0)) {
        TrigErrorAct->TrigErrorInstruction[0].Instruction = EFI_ACPI_6_0_EINJ_WRITE_REGISTER_VALUE;
        TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.AddressSpaceId = EFI_ACPI_6_0_SYSTEM_IO;
        TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.RegisterBitWidth = REG_BIT_WIDTH_BYTE;
        TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.AccessSize = EFI_ACPI_6_0_BYTE;
        TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.Address = 0xB2;
        TrigErrorAct->TrigErrorInstruction[0].Value = mPlatformApeiData->PlatRasPolicy.EinjTrigErrSwSmiId;
        TrigErrorAct->TrigErrorInstruction[0].Mask = EINJ_BYTE_MASK;
    } else {
        TrigErrorAct->TrigErrorInstruction[0].Instruction = EFI_ACPI_6_0_EINJ_NOOP;
    }
    OperationStatus = EFI_ACPI_6_0_EINJ_STATUS_SUCCESS;
    EinjActMemRegAddr->EinjGetTriActTblReg = (UINT64) ((UINTN) TrigErrorAct);
    
    break;
  case EFI_ACPI_6_0_EINJ_ERROR_PROCESSOR_UNCORRECTABLE_NONFATAL:
    DEBUG ((EFI_D_ERROR, "Inject processor non-fatal error to CPU(APIC ID: 0x%x), Bank %d\n", ApicId, Bank));

    AsmMsrOr64 (MSR_PFEH_CFG, GENERATE_MCE_ON_EXIT);
    PfehCfg = AsmReadMsr64 (MSR_PFEH_CFG);
    
    //Inject correctable error
    McBankState.McaStatus.Value = 0;
    McBankState.McaStatus.Field.Val = 1;
    McBankState.McaStatus.Field.MiscV = 1;
    McBankState.McaStatus.Field.SyndV = 1;
    McBankState.McaStatus.Field.ErrorCode = 1 << INT_ERROR_CHK_SHIFT;
    McBankState.McaStatus.Field.ErrorCodeExt = MCA_SYND_TYPE_INTERNAL_ERR;
    McBankState.McaStatus.Field.UC = 1;
    McBankState.McaSynd.Value = 0;
    McBankState.McaSynd.Field.ErrorInformation = (1 << 2) | 0x2;

    
    McBankState.ApicId = ApicId;
    McBankState.Bank = Bank;
    McBankState.Pfeh = (PfehCfg & BIT0) ? TRUE : FALSE;
    SetMcaBank(&McBankState);
    
    // Trigger action - Warm reset
    IoWrite8 (0xCD6, 0x10);
    PwrRsrCfg = IoRead8 (0xCD7);
    PwrRsrCfg = PwrRsrCfg & 0xFD; //clear ToggleAllPwrGoodOnCf9
    IoWrite8 (0xCD7, PwrRsrCfg);
    IoWrite8(0xCF9, 0x2);
    IoWrite8(0xCF9, 0x6);
    CpuDeadLoop();
    
    //9. Public action table : Update Read memory location 0x00000001_00000000 (uncachable)
    TrigErrorAct->TrigErrorInstruction[0].Instruction = EFI_ACPI_6_0_EINJ_NOOP;
    OperationStatus = EFI_ACPI_6_0_EINJ_STATUS_SUCCESS;
    EinjActMemRegAddr->EinjGetTriActTblReg = (UINT64) ((UINTN) TrigErrorAct);
    
    break;
  case EFI_ACPI_6_0_EINJ_ERROR_PROCESSOR_UNCORRECTABLE_FATAL:
    DEBUG ((EFI_D_ERROR, "Inject processor fatal error to CPU(APIC ID: 0x%x), Bank %d\n", ApicId, Bank));

    AsmMsrOr64 (MSR_PFEH_CFG, GENERATE_MCE_ON_EXIT);
    PfehCfg = AsmReadMsr64 (MSR_PFEH_CFG);
    
    //Inject correctable error
    McBankState.McaStatus.Value = 0;
    McBankState.McaStatus.Field.Val = 1;
    McBankState.McaStatus.Field.MiscV = 1;
    McBankState.McaStatus.Field.SyndV = 1;
    McBankState.McaStatus.Field.ErrorCode = 1 << INT_ERROR_CHK_SHIFT;
    McBankState.McaStatus.Field.ErrorCodeExt = MCA_SYND_TYPE_INTERNAL_ERR;
    McBankState.McaStatus.Field.UC = 1;
    McBankState.McaStatus.Field.PCC = 1;
    McBankState.McaSynd.Value = 0;
    McBankState.McaSynd.Field.ErrorInformation = (1 << 2) | 0x2;
    
    McBankState.ApicId = ApicId;
    McBankState.Bank = Bank;
    McBankState.Pfeh = (PfehCfg & BIT0) ? TRUE : FALSE;
    SetMcaBank(&McBankState);
    
    // Trigger action - Warm reset
    IoWrite8 (0xCD6, 0x10);
    PwrRsrCfg = IoRead8 (0xCD7);
    PwrRsrCfg = PwrRsrCfg & 0xFD; //clear ToggleAllPwrGoodOnCf9
    IoWrite8 (0xCD7, PwrRsrCfg);
    IoWrite8(0xCF9, 0x2);
    IoWrite8(0xCF9, 0x6);
    CpuDeadLoop();
    
    //9. Public action table : Update Read memory location 0x00000001_00000000 (uncachable)
    TrigErrorAct->TrigErrorInstruction[0].Instruction = EFI_ACPI_6_0_EINJ_NOOP;
    OperationStatus = EFI_ACPI_6_0_EINJ_STATUS_SUCCESS;
    EinjActMemRegAddr->EinjGetTriActTblReg = (UINT64) ((UINTN) TrigErrorAct);
    
    break;

  default:
    OperationStatus = EFI_ACPI_6_0_EINJ_STATUS_INVALID_ACCESS;
  }

  return OperationStatus;
}
UINT16
EFIAPI
AmdSmiEinjExecuteActionMemory (VOID)
{

  APEI_EINJ_REGISTERS *EinjActMemRegAddr;
  TRIG_ERROR_ACTION   *TrigErrorAct;
  UINT64  ErrorType;
  UINT16  OperationStatus;
  UINT32  RegValue;
  UINT8                   DieBusNum;
  UINT8                   DieNum;
  UINT8                   Channel;
  UINT8                   ChipSelect;
  SET_ERR_TYPE_WITH_ADDR  *SetErrTypeWithAddr;
  NORMALIZED_ADDRESS      NormalizedAddress;
  DIMM_INFO               DimmInfo;
  UINT32                  UmcRegBase;
  DRAM_SCRUB_ADDR_LO_REG  DramScrubAddrLo;
  DRAM_SCRUB_ADDR_HI_REG  DramScrubAddrHi;

  DEBUG ((EFI_D_ERROR, "EINJ SMI Execute Action for Memory\n"));

  EinjActMemRegAddr = (APEI_EINJ_REGISTERS*)mPlatformApeiData->EinjActMemRegAddr;
  TrigErrorAct = (TRIG_ERROR_ACTION*)mPlatformApeiData->TrigErrorAct;

  // Initialize Local Variables
  OperationStatus = EFI_ACPI_6_0_EINJ_STATUS_UNKNOWN_FAILURE;

  //Init a default address for memory error injection.
  gSysMemAddr = EINJ_DFLT_INJECT_ADDRESS;

  // Get Injection error type
  SetErrTypeWithAddr = (SET_ERR_TYPE_WITH_ADDR*)(UINTN)EinjActMemRegAddr->EinjSetErrTypeWithAddrReg;
  ErrorType = SetErrTypeWithAddr->ErrorType;

  if (ErrorType != 0) {
    if (SetErrTypeWithAddr->MemoryAddress != 0) {
      gSysMemAddr = SetErrTypeWithAddr->MemoryAddress;
      //DRAM Scrubber Error Address Low Mask 0xFFFFFFC0
      gSysMemAddr = gSysMemAddr & DRAM_SCRUBBER_ERROR_ADDRESS_LOW_MASK;
      DEBUG ((EFI_D_ERROR, "EINJ Set Error Type With Address command accepted!!\n"));
    }
  }
  else {
    ErrorType = EinjActMemRegAddr->EinjSetErrTypeReg;
  }

  DEBUG ((EFI_D_ERROR, "Memory Error inject to System Address: 0x%08x", RShiftU64(gSysMemAddr, 32)));
  DEBUG ((EFI_D_ERROR, "%08x\n", gSysMemAddr & 0xFFFFFFFF));

  //Find Dies and Channel ID use TranslateSysAddrToCS function.
  mAmdRasSmmProtocol->TranslateSysAddrToCS(&gSysMemAddr, &NormalizedAddress, &DimmInfo);
  DieNum = (NormalizedAddress.normalizedSocketId + 1) * NormalizedAddress.normalizedDieId;
  Channel = NormalizedAddress.normalizedChannelId;
  ChipSelect = DimmInfo.ChipSelect;

  //
  // Execute error injection based upon requested Error Type
  //
  switch (ErrorType)
  {
  case EFI_ACPI_6_0_EINJ_ERROR_MEMORY_CORRECTABLE:  // Correctable Memory Error
    DEBUG ((EFI_D_ERROR, "Memory ECC Error Injection to Die %x, UMC %x, CS %x\n", DieNum, Channel, ChipSelect));
    //Inject correctable error
    OperationStatus = MemEccErrInject (EINJ_ERROR_VECTOR_CORR, DieNum, Channel, ChipSelect, gSysMemAddr);
    if (OperationStatus == EFI_ACPI_6_0_EINJ_STATUS_INVALID_ACCESS) {
      DEBUG ((EFI_D_ERROR, "EINJ Invalid Access\n"));
      break;
    }
    //9. Public action table : Update Read memory location 0x00000001_00000000 (uncachable)
    TrigErrorAct->TrigErrorInstruction[0].Instruction = EFI_ACPI_6_0_EINJ_WRITE_REGISTER_VALUE;
    TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.AddressSpaceId = EFI_ACPI_6_0_SYSTEM_MEMORY;
    TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.RegisterBitWidth = REG_BIT_WIDTH_QWORD;
    TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.AccessSize = EFI_ACPI_6_0_QWORD;
    TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.Address = gSysMemAddr;
    TrigErrorAct->TrigErrorInstruction[0].Value = 0;
    TrigErrorAct->TrigErrorInstruction[0].Mask = 0;

    OperationStatus = EFI_ACPI_6_0_EINJ_STATUS_SUCCESS;
    EinjActMemRegAddr->EinjGetTriActTblReg = (UINT64) ((UINTN) TrigErrorAct);

    break;
  case EFI_ACPI_6_0_EINJ_ERROR_MEMORY_UNCORRECTABLE_NONFATAL:
    DEBUG ((EFI_D_ERROR, "[Type 4] Memory ECC Error Injection to Die %x, UMC %x, CS %x\n", DieNum, Channel, ChipSelect));
    //Inject un-correctable error
    OperationStatus = MemEccErrInject (EINJ_ERROR_VECTOR_UNCORR, DieNum, Channel, ChipSelect, gSysMemAddr);
    if (OperationStatus == EFI_ACPI_6_0_EINJ_STATUS_INVALID_ACCESS) {
      DEBUG ((EFI_D_ERROR, "EINJ Invalid Access\n"));
      break;
    }

    //9. Public action table :
    //FillIntheErrorActionTable();
    //Get IOHC bus number
    DieBusNum = GetNBIOBusNum(DieNum);
    //Get UMC registers base address
    UmcRegBase = (Channel == 0) ? UMC0_CH_REG_BASE : UMC1_CH_REG_BASE;
    
    //DF::DramScrubBaseAddr ==> 0x0  (disable the DRAM Scrubber)
    RegValue = 0;
    RasSmnRead (DieBusNum, DF_DFF6_REG_BASE + DRAM_SCRUBBER_BASEADDR_REG, &RegValue);
    RegValue &= ~BIT0;
    RasSmnWrite (DieBusNum, DF_DFF6_REG_BASE + DRAM_SCRUBBER_BASEADDR_REG, &RegValue);
    //Program DF::DramScrubAddrHi to the desired location (user-provided address. i.e. gSysMemAddr)
    DramScrubAddrHi.Value = 0;
    DramScrubAddrHi.Fields.SeqScrubAddrHi = (UINT32)RShiftU64(gSysMemAddr, 32);
    RasSmnWrite (DieBusNum, DF_DFF6_REG_BASE + DRAM_SCRUBBER_ADDR_HI, &DramScrubAddrHi.Value);
    //Program DF::DramScrubAddrLo to the desired location (user-provided address. i.e. gSysMemAddr)
    DramScrubAddrLo.Value = 0;
    DramScrubAddrLo.Fields.SeqScrubAddrLo = ((UINT32)RShiftU64((gSysMemAddr & 0xFFFFFFFF), 6) - 0x100); //Todo: Need to find a reasonable range
    RasSmnWrite (DieBusNum, DF_DFF6_REG_BASE + DRAM_SCRUBBER_ADDR_LO, &DramScrubAddrLo.Value);
    
    //Returns DF::DramScrubBaseAddr by "Trigger error action table" to Re-enable the periodic scrubber via DF::DramScrubBaseAddr
    TrigErrorAct->TrigErrorInstruction[0].Instruction = EFI_ACPI_6_0_EINJ_WRITE_REGISTER_VALUE;
    TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.AddressSpaceId = EFI_ACPI_6_0_SYSTEM_IO;
    TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.RegisterBitWidth = REG_BIT_WIDTH_BYTE;
    TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.AccessSize = EFI_ACPI_6_0_BYTE;
    TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.Address = 0xB2;
    TrigErrorAct->TrigErrorInstruction[0].Value = mPlatformApeiData->PlatRasPolicy.EinjTrigErrSwSmiId;
    TrigErrorAct->TrigErrorInstruction[0].Mask = EINJ_BYTE_MASK;
    
    mBusNumForType4Einj = DieBusNum;

    OperationStatus = EFI_ACPI_6_0_EINJ_STATUS_SUCCESS;
    EinjActMemRegAddr->EinjGetTriActTblReg = (UINT64) ((UINTN) TrigErrorAct);
    
    break; 
  case EFI_ACPI_6_0_EINJ_ERROR_MEMORY_UNCORRECTABLE_FATAL:  // Uncorrectable Memory Error
      DEBUG ((EFI_D_ERROR, "Memory ECC Error Injection to Die %x, UMC %x, CS %x\n", DieNum, Channel, ChipSelect));
    //Inject un-correctable error
    OperationStatus = MemEccErrInject (EINJ_ERROR_VECTOR_UNCORR, DieNum, Channel, ChipSelect, gSysMemAddr);
    if (OperationStatus == EFI_ACPI_6_0_EINJ_STATUS_INVALID_ACCESS) {
      DEBUG ((EFI_D_ERROR, "EINJ Invalid Access\n"));
      break;
    }

    //9. Public action table : Update Read memory location 0x00000001_00000000 (uncachable)
    TrigErrorAct->TrigErrorInstruction[0].Instruction = EFI_ACPI_6_0_EINJ_WRITE_REGISTER_VALUE;
    TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.AddressSpaceId = EFI_ACPI_6_0_SYSTEM_MEMORY;
    TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.RegisterBitWidth = REG_BIT_WIDTH_QWORD;
    TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.AccessSize = EFI_ACPI_6_0_QWORD;
    TrigErrorAct->TrigErrorInstruction[0].RegisterRegion.Address = gSysMemAddr;
    TrigErrorAct->TrigErrorInstruction[0].Value = 0;
    TrigErrorAct->TrigErrorInstruction[0].Mask = 0;

    OperationStatus = EFI_ACPI_6_0_EINJ_STATUS_SUCCESS;
    EinjActMemRegAddr->EinjGetTriActTblReg = (UINT64) ((UINTN) TrigErrorAct);

    break;
  default:
    OperationStatus = EFI_ACPI_6_0_EINJ_STATUS_INVALID_ACCESS;
  }

  return OperationStatus;
}
UINT16
EFIAPI
AmdSmiEinjExecuteActionPlatform (VOID)
{

  APEI_EINJ_REGISTERS *EinjActMemRegAddr;
  TRIG_ERROR_ACTION   *TrigErrorAct;
  UINT64  ErrorType;
  UINT16  OperationStatus;
  UINT32  RegValue;
  UINT8                   DieBusNum;
  UINT8                   i;
  PARITY_CONTROL_1_REG    ParityControl1;
  SET_ERR_TYPE_WITH_ADDR  *SetErrTypeWithAddr;

  DEBUG ((EFI_D_ERROR, "EINJ SMI Execute Action for Platform\n"));

  EinjActMemRegAddr = (APEI_EINJ_REGISTERS*)mPlatformApeiData->EinjActMemRegAddr;
  TrigErrorAct = (TRIG_ERROR_ACTION*)mPlatformApeiData->TrigErrorAct;

  // Initialize Local Variables
  OperationStatus = EFI_ACPI_6_0_EINJ_STATUS_UNKNOWN_FAILURE;

  // Get Injection error type
  SetErrTypeWithAddr = (SET_ERR_TYPE_WITH_ADDR*)(UINTN)EinjActMemRegAddr->EinjSetErrTypeWithAddrReg;
  ErrorType = SetErrTypeWithAddr->ErrorType;
  if (ErrorType == 0) {
    ErrorType = EinjActMemRegAddr->EinjSetErrTypeReg;
  }

  //
  // Execute error injection based upon requested Error Type
  //
  switch (ErrorType)
  {
  case EFI_ACPI_6_0_EINJ_ERROR_PLATFORM_CORRECTABLE:
    DEBUG ((EFI_D_ERROR, "NBIO EINJ \n"));
    DieBusNum = GetNBIOBusNum(DIE);

    for (i = 0; i < sizeof (gNbioParityErrorTable) / sizeof (NBIO_PARITY_ERROR_TABLE); i++) {
      if (gNbioParityErrorTable[i].GroupType == PARITY_CORRECTABLE_GROUP) {
        RegValue = 0;
        RasSmnWrite (DieBusNum, RAS_PARITY_CONTROL_1, &RegValue);
        do {
          RasSmnRead (DieBusNum, RAS_PARITY_CONTROL_1, &ParityControl1.Value);
        } while (ParityControl1.Fields.ParityErrGenInjectAllow == 0);

        ParityControl1.Fields.ParityErrGenGroupTypeSel = gNbioParityErrorTable[i].GroupType;
        ParityControl1.Fields.ParityErrGenGroupSel = gNbioParityErrorTable[i].GroupId;
        ParityControl1.Fields.ParityErrGenIdSel = gNbioParityErrorTable[i].StructId;
        ParityControl1.Fields.ParityErrGenCmd = gNbioParityErrorTable[i].ErrGenCmd;
        ParityControl1.Fields.ParityErrGenTrigger = 0x1;
        RasSmnWrite (DieBusNum, RAS_PARITY_CONTROL_1, &ParityControl1.Value);
      }
    }
  break;

  case EFI_ACPI_6_0_EINJ_ERROR_PLATFORM_UNCORRECTABLE_FATAL:
    DEBUG ((EFI_D_ERROR, "NBIO EINJ \n"));
    DieBusNum = GetNBIOBusNum(DIE);

    for (i = 0; i < sizeof (gNbioParityErrorTable) / sizeof (NBIO_PARITY_ERROR_TABLE); i++) {
      if (gNbioParityErrorTable[i].GroupType == PARITY_UNCORRECTABLE_GROUP) {
        RegValue = 0;
        RasSmnWrite (DieBusNum, RAS_PARITY_CONTROL_1, &RegValue);
        do {
          RasSmnRead (DieBusNum, RAS_PARITY_CONTROL_1, &ParityControl1.Value);
        } while (ParityControl1.Fields.ParityErrGenInjectAllow == 0);

        ParityControl1.Fields.ParityErrGenGroupTypeSel = gNbioParityErrorTable[i].GroupType;
        ParityControl1.Fields.ParityErrGenGroupSel = gNbioParityErrorTable[i].GroupId;
        ParityControl1.Fields.ParityErrGenIdSel = gNbioParityErrorTable[i].StructId;
        ParityControl1.Fields.ParityErrGenCmd = gNbioParityErrorTable[i].ErrGenCmd;
        ParityControl1.Fields.ParityErrGenTrigger = 0x1;
        RasSmnWrite (DieBusNum, RAS_PARITY_CONTROL_1, &ParityControl1.Value);
      }
    }
  break;

  default:
    OperationStatus = EFI_ACPI_6_0_EINJ_STATUS_INVALID_ACCESS;
  }

  return OperationStatus;
}


/*********************************************************************************
 * Name: AmdSmiEinjExecuteActionCallback
 *
 * Description
 *   EINJ Trigger Action SMI call back function entry
 *   Perform EINJ Error instruction request
 *
 *
 * Arguments:
 *   DispatchHandle  : The handle of this callback, obtained when registering
 *   DispatchContext : Pointer to the FCH_SMM_SW_DISPATCH_CONTEXT
 *
 * Returns:
 *   None
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
AmdSmiEinjExecuteActionCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  APEI_EINJ_REGISTERS *EinjActMemRegAddr;
  TRIG_ERROR_ACTION   *TrigErrorAct;
  UINT64  ErrorType;
  UINT16  OperationStatus;
  SET_ERR_TYPE_WITH_ADDR  *SetErrTypeWithAddr;

  DEBUG ((EFI_D_ERROR, "EINJ SMI Execute Action handler\n"));

  EinjActMemRegAddr = (APEI_EINJ_REGISTERS*)mPlatformApeiData->EinjActMemRegAddr;
  TrigErrorAct = (TRIG_ERROR_ACTION*)mPlatformApeiData->TrigErrorAct;

  // Initialize Local Variables
  OperationStatus = EFI_ACPI_6_0_EINJ_STATUS_UNKNOWN_FAILURE;

  // Set Busy Status bit
  EinjActMemRegAddr->EinjChkBusyStsReg = APEI_SET_BUSY_BIT;     // APEI_SET_BUSY_BIT = 0x01

  // Get Injection error type
  SetErrTypeWithAddr = (SET_ERR_TYPE_WITH_ADDR*)(UINTN)EinjActMemRegAddr->EinjSetErrTypeWithAddrReg;
  ErrorType = SetErrTypeWithAddr->ErrorType;

  if (ErrorType == 0) {
    ErrorType = EinjActMemRegAddr->EinjSetErrTypeReg;
  }

  //
  // Execute error injection based upon requested Error Type
  //
  DEBUG ((EFI_D_ERROR, "ErrorType: 0x%016llx\n", ErrorType));
  switch (ErrorType)
  {
  case EFI_ACPI_6_0_EINJ_ERROR_PROCESSOR_CORRECTABLE:
  case EFI_ACPI_6_0_EINJ_ERROR_PROCESSOR_UNCORRECTABLE_NONFATAL:
  case EFI_ACPI_6_0_EINJ_ERROR_PROCESSOR_UNCORRECTABLE_FATAL:
    OperationStatus = AmdSmiEinjExecuteActionProcessor();
    break;
  case EFI_ACPI_6_0_EINJ_ERROR_MEMORY_CORRECTABLE:  // Correctable Memory Error
  case EFI_ACPI_6_0_EINJ_ERROR_MEMORY_UNCORRECTABLE_NONFATAL:
  case EFI_ACPI_6_0_EINJ_ERROR_MEMORY_UNCORRECTABLE_FATAL:  // Uncorrectable Memory Error
    OperationStatus = AmdSmiEinjExecuteActionMemory();
    break;
  case EFI_ACPI_6_0_EINJ_ERROR_PLATFORM_CORRECTABLE:
  case EFI_ACPI_6_0_EINJ_ERROR_PLATFORM_UNCORRECTABLE_FATAL:
    OperationStatus = AmdSmiEinjExecuteActionPlatform();
    break;

  default:
    OperationStatus = EFI_ACPI_6_0_EINJ_STATUS_INVALID_ACCESS;
  }

  //Set action command status
  EinjActMemRegAddr->EinjCmdStsReg = OperationStatus;

  // Clear busy status bit
  EinjActMemRegAddr->EinjChkBusyStsReg = APEI_RESET_BUSY_BIT;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmdSmiEinjTriggProcessorError (VOID)
{
  UINTN ProcessorNumber;
  UINT8              BankNum;
  UINT8              BankIndex;
  UINT64             MsrData;
  RAS_MCA_ERROR_INFO RasMcaErrorInfo;
                                                                                      
  DEBUG ((EFI_D_ERROR, "Trigger Processor Error\n"));

  MsrData = AsmReadMsr64 (MSR_MCG_CAP);            // MCG_CAP
  BankNum = (UINT8) (MsrData & 0xFF);

  //Check Injected Errors
  for (ProcessorNumber = 0; ProcessorNumber < gSmst->NumberOfCpus; ProcessorNumber++) {
    ZeroMem (&RasMcaErrorInfo, sizeof (RasMcaErrorInfo));
    RasMcaErrorInfo.CpuInfo.ProcessorNumber = ProcessorNumber;
    mAmdRasSmmProtocol->SearchMcaError (&RasMcaErrorInfo);
    LogMCAError (&RasMcaErrorInfo);
    for (BankIndex = 0; BankIndex < BankNum; BankIndex++) {
      if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.Ovrflw) {
        //Check Error Threshold interrupt type.
        if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.ThresholdIntType == MCA_APIC) {
          //Set GenerateThresholdLvtOnExit
          DEBUG ((EFI_D_ERROR, "ljf-einj: %a[%d]\n", __FUNCTION__, __LINE__));
          mAmdRasSmmProtocol->RasSmmExitType (ProcessorNumber, GENERATE_THRESHOLDLVT_ON_EXIT);
        } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.ThresholdIntType == MCA_APIC)

        // Since the ThresholdIntType is cleared in Polling mode, so we assume the ThresholdIntType is MCA_SMI in here.
        if ((RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.ThresholdIntType == MCA_SMI) ||
          (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.ThresholdIntType == MCA_NO_INTERRUPT)) {
          DEBUG ((EFI_D_ERROR, "ljf-einj: %a[%d]\n", __FUNCTION__, __LINE__));
          mAmdRasSmmProtocol->RasSmmExitType (ProcessorNumber, GENERATE_MCE_ON_EXIT);
        }
        mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
        McaThresholdInit (&RasMcaErrorInfo, BankIndex);
      }
    }
  }

  return EFI_SUCCESS;
}
EFI_STATUS
EFIAPI
AmdSmiEinjTriggMemoryError (VOID)
{
  UINT32            RegValue;

  DEBUG ((EFI_D_ERROR, "Trigger Memory Error at address: 0x%08x", RShiftU64(gSysMemAddr, 32)));
  DEBUG ((EFI_D_ERROR, "%08x\n", gSysMemAddr & 0xFFFFFFFF));
  DEBUG ((EFI_D_ERROR, ", mBusNumForType4Einj: 0x%08x\n", mBusNumForType4Einj));

  if (mBusNumForType4Einj == 0xFF)
    return EFI_NOT_FOUND;

  //DF::DramScrubBaseAddr[DramScrubEn] ==> 1  (Eisable the DRAM Scrubber)
  RasSmnRead (mBusNumForType4Einj, DF_DFF6_REG_BASE + DRAM_SCRUBBER_BASEADDR_REG, &RegValue);
  RegValue |= BIT0;
  RasSmnWrite (mBusNumForType4Einj, DF_DFF6_REG_BASE + DRAM_SCRUBBER_BASEADDR_REG, &RegValue);
  mBusNumForType4Einj = 0xFF;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmdSmiEinjTriggErrCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
    EFI_STATUS Status = EFI_SUCCESS;
    APEI_EINJ_REGISTERS *EinjActMemRegAddr;
    UINT64  ErrorType;
    SET_ERR_TYPE_WITH_ADDR  *SetErrTypeWithAddr;

    DEBUG ((EFI_D_ERROR, "EINJ SMI Trigger Error handler\n"));

    EinjActMemRegAddr = (APEI_EINJ_REGISTERS*)mPlatformApeiData->EinjActMemRegAddr;

    // Get Injection error type
    SetErrTypeWithAddr = (SET_ERR_TYPE_WITH_ADDR*)(UINTN)EinjActMemRegAddr->EinjSetErrTypeWithAddrReg;
    ErrorType = SetErrTypeWithAddr->ErrorType;

    if (ErrorType == 0) {
      ErrorType = EinjActMemRegAddr->EinjSetErrTypeReg;
    }

    //
    // Execute error injection based upon requested Error Type
    //
    DEBUG ((EFI_D_ERROR, "ErrorType: 0x%016llx\n", ErrorType));
    switch (ErrorType)
    {
    case EFI_ACPI_6_0_EINJ_ERROR_PROCESSOR_CORRECTABLE:
    case EFI_ACPI_6_0_EINJ_ERROR_PROCESSOR_UNCORRECTABLE_NONFATAL:
    case EFI_ACPI_6_0_EINJ_ERROR_PROCESSOR_UNCORRECTABLE_FATAL:
      Status = AmdSmiEinjTriggProcessorError();
      break;

    case EFI_ACPI_6_0_EINJ_ERROR_MEMORY_CORRECTABLE:  // Correctable Memory Error
    case EFI_ACPI_6_0_EINJ_ERROR_MEMORY_UNCORRECTABLE_NONFATAL:
    case EFI_ACPI_6_0_EINJ_ERROR_MEMORY_UNCORRECTABLE_FATAL:  // Uncorrectable Memory Error
      Status = AmdSmiEinjTriggMemoryError();
      break;

    default:
      DEBUG ((EFI_D_ERROR, "EINJ Unhandled Error Trigger Action\n"));
    }

    return EFI_SUCCESS;
}

EFI_STATUS
RasSmmRegisterEinjSwSmi (
  VOID
  )
{
  EFI_STATUS                               Status;
  FCH_SMM_SW_DISPATCH2_PROTOCOL            *AmdSwDispatch;
  FCH_SMM_SW_REGISTER_CONTEXT              SwRegisterContext;
  EFI_HANDLE                               SwHandle;
  UINT8 EinjSwSmiData;
  UINT8 EinjTrigErrSwSmiId;

  //
  //  Locate SMM SW dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmSwDispatch2ProtocolGuid,
                  NULL,
                  &AmdSwDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_ERROR, "[RAS] Install EINJ Software SMI callback handler\n"));

  //EinjSwSmiData = 0x83;
  EinjSwSmiData = mPlatformApeiData->PlatRasPolicy.EinjSwSmiData;

  SwRegisterContext.AmdSwValue  = EinjSwSmiData; // EINJ software SMI ID
  SwRegisterContext.Order  = 0x80;
  Status = AmdSwDispatch->Register (
                            AmdSwDispatch,
                            AmdSmiEinjExecuteActionCallback,
                            &SwRegisterContext,
                            &SwHandle
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }


  DEBUG ((EFI_D_ERROR, "[RAS] Install EINJ Error Trigger Software SMI callback handler\n"));
  //EinjTrigErrSwSmiId = 0x84;
  EinjTrigErrSwSmiId = mPlatformApeiData->PlatRasPolicy.EinjTrigErrSwSmiId;

  SwRegisterContext.AmdSwValue  = EinjTrigErrSwSmiId; // EINJ software SMI ID
  SwRegisterContext.Order  = 0x80;
  Status = AmdSwDispatch->Register (
                            AmdSwDispatch,
                            AmdSmiEinjTriggErrCallback,
                            &SwRegisterContext,
                            &SwHandle
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }


  return EFI_SUCCESS;
}

VOID
BspMcaThresholdInit (
  IN       UINT8 McaBankNumber
  )
{
  UINT64 SaveHwcr;
  UINT64 MsrData;
  UINT32 MsrAddr;
  MCA_MISC0_MSR Misc0;

  DEBUG ((EFI_D_ERROR, "MCA error thresholding counter init\n"));

  // MSR_C001_0015[18][McStatusWrEn] = 1
  SaveHwcr = AsmReadMsr64 (MSR_HWCR);
  MsrData = SaveHwcr | BIT18;
  AsmWriteMsr64 (MSR_HWCR, MsrData);


  MsrAddr = MCA_EXTENSION_BASE + (McaBankNumber * 0x10) | MCA_MISC0_OFFSET;
  Misc0.Value = AsmReadMsr64 (MsrAddr);
  //Check if ErrCnt was enabled.
  if (Misc0.Field.CntEn) {
    Misc0.Field.CntEn = 0;
    Misc0.Field.ErrCnt = 0xFFE;
    //Clear Ovrflw bit.
    Misc0.Field.Ovrflw = 0;

    AsmWriteMsr64 (MsrAddr, Misc0.Value);
    Misc0.Field.CntEn = 1;
    AsmWriteMsr64 (MsrAddr, Misc0.Value);
  }
  // Restore MSR_C001_0015[18][McStatusWrEn]
  AsmWriteMsr64 (MSR_HWCR, SaveHwcr);

  Misc0.Value = AsmReadMsr64 (MsrAddr);
  DEBUG ((EFI_D_ERROR, "MCA error thresholding counter : 0x%04x\n", Misc0.Field.ErrCnt));

}

UINT16
MemEccErrInject (
  IN       UINT16 EccInjVector,
  IN       UINT8  DieNum,
  IN       UINT8  ChannelId,
  IN       UINT8  ChipSelect,
  IN       UINT64 SysMemAddr
  )
{
  UMC_MISCCFG_REG   UmcMiscCfg;
  UMC_ECCCTRL_REG   UmcEccCtrl;
  UMC_ECCERRINJ_REG UmcEccErrInj;
  UINT32            RegValue;
  UINT32            UmcRegBase;
  UINT8             BusNum;
  DRAM_SCRUB_ERRORADDR_LO_REG DramScrubErrorAddrLo;
  DRAM_SCRUB_ERRORADDR_HI_REG DramScrubErrorAddrHi;

  //Get IOHC bus number
  BusNum = GetNBIOBusNum(DieNum);

  //Get UMC registers base address
  UmcRegBase = (ChannelId == 0) ? UMC0_CH_REG_BASE : UMC1_CH_REG_BASE;

  //Init thresholding count to 0xFFE
//debug  BspMcaThresholdInit (MCA_UMC0_BANK);
  //Put memory error injection code here
  //Injection single bit error to Socket 0 Die 0 UmcChannel 0
  //1. UmcMiscCfg_ch0[DisErrInj] ==> `0b

  RasSmnRead (BusNum, UmcRegBase + UMC_MISC_CFG, &UmcMiscCfg.Value);
  if (UmcMiscCfg.Field.DisErrInj) {
    return EFI_ACPI_6_0_EINJ_STATUS_INVALID_ACCESS;
  }

  //2. check if ECC enabled
  RasSmnRead (BusNum, UmcRegBase + UMC_ECC_CTRL, &UmcEccCtrl.Value);
  if (0 == UmcEccCtrl.Field.RdEccEn) {
    return EFI_ACPI_6_0_EINJ_STATUS_INVALID_ACCESS;
  }
  DEBUG ((EFI_D_ERROR, "Memory Based ECC Error Injection : Vector : 0x%04x\n", EccInjVector));


  //7. UmcCH::EccErrInjCtrl ==> 0x00  (for Address based error injection)
  RegValue = 0;
  RasSmnWrite (BusNum, UmcRegBase + UMC_ECC_ERR_INJ_CTRL, &RegValue);

  //3. DF::DramScrubBaseAddr ==> 0x0  (disable the DRAM Scrubber)
  RegValue = 0;
  RasSmnWrite (BusNum, DF_DFF6_REG_BASE + DRAM_SCRUBBER_BASEADDR_REG, &RegValue);

  //4. DF::DramScrubErrAddrLo ==> Error Injection LowAddress
  DramScrubErrorAddrLo.Value = 0;
  DramScrubErrorAddrLo.Fields.ErrInjAddrLo = (UINT32)RShiftU64((SysMemAddr & 0xFFFFFFFF), 6);
  RasSmnWrite (BusNum, DF_DFF6_REG_BASE + DRAM_SCRUBBER_ERRORADDR_LO, &DramScrubErrorAddrLo.Value);

  //5. DF::DramScrubErrAddrHi ==> Error Injection High Address
  DramScrubErrorAddrHi.Value = 1;
  DramScrubErrorAddrHi.Fields.ErrInjAddrHi = (UINT32)RShiftU64(SysMemAddr, 32);
  RasSmnWrite (BusNum, DF_DFF6_REG_BASE + DRAM_SCRUBBER_ERRORADDR_HI, &DramScrubErrorAddrHi.Value);

  //6. UmcCH::EccErrInj
  //   EccInjEn ==> `1b (data word for error injection)
  //   EccInjVector ==> `1b (pattern XOR'ed into data word)
  UmcEccErrInj.Value = 0;
  UmcEccErrInj.Field.EccInjEn = 1;
  UmcEccErrInj.Field.EccInjVector = EccInjVector;
  RasSmnWrite (BusNum, UmcRegBase + (ChipSelect * 4) + UMC_ECC_ERR_INJ, &UmcEccErrInj.Value);

  //7. UmcCH::EccErrInjCtrl ==> 0x04  (for Address based error injection)
  RasSmnRW (BusNum, UmcRegBase + UMC_ECC_ERR_INJ_CTRL, ~ECC_ERR_ADDR_EN, ECC_ERR_ADDR_EN);

  //8. Inject the Error:
  //DF::DramScrubErrAddrLo [ErrInjEn] ==> 0x01   (perform the Address based error injection)
  //DF::DramScrubErrAddrLo[ErrInjDone]  == `1b When injection is completed
  RasSmnRW (BusNum, DF_DFF6_REG_BASE + DRAM_SCRUBBER_ERRORADDR_LO, ~ERR_INJ_EN, ERR_INJ_EN);

  do {
    RasSmnRead (BusNum, DF_DFF6_REG_BASE + DRAM_SCRUBBER_ERRORADDR_LO, &DramScrubErrorAddrLo.Value);
  } while (DramScrubErrorAddrLo.Fields.ErrInjDone == 0);

  return EFI_ACPI_6_0_EINJ_STATUS_SUCCESS;
}
