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
#include <Library/BaseFabricTopologyLib.h>
#include <Library/SmnAccessLib.h>
#include <Protocol/AmdApcbProtocol.h>
#include <AmdRas.h>
#include "AmdPlatformRasSmm.h"
#include "Protocol/FchSmmPeriodicalDispatch2.h"
#include <AmdCpmSmm.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
UINT32                                     mSmiCount = 0;
UINT32                                     mSmiCoolOff = 0;
UINT64                                     mTscLast = 0;
SMI_MODE                                   mSmiMode = INTERRUPT_MODE;
EFI_HANDLE                                 mPollingModeHandle = NULL;
FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL      *mAmdPeriodicalDispatch = NULL;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
LogMCAError (
  IN       RAS_MCA_ERROR_INFO   *RasMcaErrorInfo
  );

VOID
McaThresholdInit (
  IN       RAS_MCA_ERROR_INFO   *RasMcaErrorInfo,
  IN       UINT8                BankIndex
  );

VOID
ErrorThresholdInit (
  IN VOID *Buffer
  );

EFI_STATUS
DimmPostPackageRepair (
  IN       RAS_MCA_ERROR_INFO   *RasMcaErrorInfo,
  IN       UINT8                BankIndex
  );

BOOLEAN
ReCloakCheck(
  IN       UINTN    ProcessorNumber, 
  OUT      UINT8    *BankIndex
  );

VOID 
ProcessPfehSmiSource (
  BOOLEAN  *SmiSourceChecked
  );

VOID 
ProcessPeriodicSMI (
  VOID
  );

VOID
CpmSetMcaThreshold (
  IN       RAS_THRESHOLD_CONFIG* RasThresholdConfig
  );

VOID
CpmSetMpMcaThreshold (
  IN       RAS_THRESHOLD_CONFIG* RasThresholdConfig
  );

BOOLEAN
FindThresholdOrDeferredError (
  VOID
  );

EFI_STATUS
EFIAPI
AmdMcePeriodicSmiCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context,
  IN OUT VOID    *CommBuffer,
  IN OUT UINTN   *CommBufferSize
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


/*********************************************************************************
 * Name: AmdMcetoSmiCallback
 *
 * Description
 *   MCE software SMI call back function entry
 *   Perform MCE error check, uncloak/cloak MCE registers
 *   call out platform error handle
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
AmdMcetoSmiCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{

  LOCAL_SMI_STATUS *pLocalSmiStatusList;
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN ProcessorNumber;
  RAS_MCA_ERROR_INFO RasMcaErrorInfo;
  UINT8 BankNum;
  UINT8 BankIndex;
  UINT64 MsrData;
  BOOLEAN AlreadyPPRepaired;
  BOOLEAN SmiSourceChecked = FALSE;
  BOOLEAN RasThresholdPeriodicSmiEn;
                                                                                      
  DEBUG ((DEBUG_ERROR, "MCE to Software SMI error handler\n"));

  RasThresholdPeriodicSmiEn = mPlatformApeiData->PlatRasPolicy.RasThresholdPeriodicSmiEn;

  AlreadyPPRepaired = FALSE;
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    (sizeof (LOCAL_SMI_STATUS) * (gSmst->NumberOfCpus)),
                    &pLocalSmiStatusList
                    );
  ASSERT_EFI_ERROR (Status);


  //Get LocalSmiStatus through all CPUs.
  mAmdRasSmmProtocol->GetAllLocalSmiStatus (pLocalSmiStatusList);

  MsrData = AsmReadMsr64 (MSR_MCG_CAP);            // MCG_CAP
  BankNum = (UINT8) (MsrData & 0xFF);

  //Check LocalSmiStatus
  for (ProcessorNumber = 0; ProcessorNumber < gSmst->NumberOfCpus; ProcessorNumber++) {
    ZeroMem (&RasMcaErrorInfo, sizeof (RasMcaErrorInfo));
    RasMcaErrorInfo.CpuInfo.ProcessorNumber = ProcessorNumber;
    //SMI from MCE?
    if (pLocalSmiStatusList[ProcessorNumber].Field.MceRedirSts) {
      DEBUG ((EFI_D_ERROR, "[RAS]Local SMI Status: MceRedirSts\n"));
      mAmdRasSmmProtocol->SearchMcaError (&RasMcaErrorInfo);
      LogMCAError (&RasMcaErrorInfo);
      //A machine check execption is generated to local core upon SMM exit
      mAmdRasSmmProtocol->RasSmmExitType (ProcessorNumber, GENERATE_MCE_ON_EXIT);
      //Un-Cloak MCA register
      for (BankIndex = 0; BankIndex < BankNum; BankIndex++) {
        if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val) {
          mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
        }
      }
    }
    //SMI from Threshold or Deferred error?
    if (pLocalSmiStatusList[ProcessorNumber].Field.SmiSrcMca) {
      DEBUG ((EFI_D_ERROR, "[RAS]Local SMI Status: SmiSrcMca\n"));
      mAmdRasSmmProtocol->SearchMcaError (&RasMcaErrorInfo);
      LogMCAError (&RasMcaErrorInfo);

      //Here is default handle start
      for (BankIndex = 0; BankIndex < BankNum; BankIndex++) {
        if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val) {

          // DEFERRED ERROR
          //Check source is from Deferred error
          if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Deferred) {
            // Check for Dimm Post Package repair on any Deferred
            if (!AlreadyPPRepaired) {
              Status = DimmPostPackageRepair (&RasMcaErrorInfo, BankIndex);
              if (!EFI_ERROR (Status)) {
                AlreadyPPRepaired = TRUE;
              }
            }
            if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaConfigMsr.Field.DeferredIntType == MCA_APIC) {
              //A deferred error LVT interrupt is generated upon SMM exit
              mAmdRasSmmProtocol->RasSmmExitType (ProcessorNumber, GENERATE_DEFERREDLVT_ON_EXIT);
              //Un-Cloak MCA register
              mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
            } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaConfigMsr.Field.DeferredIntType == MCA_APIC)
            if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaConfigMsr.Field.DeferredIntType == MCA_SMI) {
              mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
            } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaConfigMsr.Field.DeferredIntType == MCA_SMI)
          } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Deferred)

          // MISC 0
          // Only check threshold status if there was an overflow
          if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.Ovrflw) {
            // Check for Dimm Post Package repair on any overflow
            if (!AlreadyPPRepaired) {
              Status = DimmPostPackageRepair (&RasMcaErrorInfo, BankIndex);
              if (!EFI_ERROR (Status)) {
                AlreadyPPRepaired = TRUE;
              }
            }
            //Check Error Threshold interrupt type.
            if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.ThresholdIntType == MCA_APIC) {
              //Set GenerateThresholdLvtOnExit
              mAmdRasSmmProtocol->RasSmmExitType (ProcessorNumber, GENERATE_THRESHOLDLVT_ON_EXIT);
              //Un-Cloak MCA register
              mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
            } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.ThresholdIntType == MCA_APIC)
            if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.ThresholdIntType == MCA_SMI) {
              if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC) {
                //A machine check execption is generated to local core upon SMM exit if a uncorrectable error.
                mAmdRasSmmProtocol->RasSmmExitType (ProcessorNumber, GENERATE_MCE_ON_EXIT);
              }
              //Un-Cloak MCA register
              mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
            } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.ThresholdIntType == MCA_SMI)
            //re-init Error Thresholding ErrCnt
            McaThresholdInit (&RasMcaErrorInfo, BankIndex);
          }

          // MISC 1
          // Only check threshold status if there was an overflow
          if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc1Msr.Field.Ovrflw) {
            // Check for Dimm Post Package repair on any overflow
            if (!AlreadyPPRepaired) {
              Status = DimmPostPackageRepair (&RasMcaErrorInfo, BankIndex);
              if (!EFI_ERROR (Status)) {
                AlreadyPPRepaired = TRUE;
              }
            }
            //Check Error Threshold interrupt type.
            if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc1Msr.Field.ThresholdIntType == MCA_APIC) {
              //Set GenerateThresholdLvtOnExit
              mAmdRasSmmProtocol->RasSmmExitType (ProcessorNumber, GENERATE_THRESHOLDLVT_ON_EXIT);
              //Un-Cloak MCA register
              mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
            } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc1Msr.Field.ThresholdIntType == MCA_APIC)
            if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc1Msr.Field.ThresholdIntType == MCA_SMI) {
              if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC) {
                //A machine check execption is generated to local core upon SMM exit if a uncorrectable error.
                mAmdRasSmmProtocol->RasSmmExitType (ProcessorNumber, GENERATE_MCE_ON_EXIT);
              }
              //Un-Cloak MCA register
              mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
            } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc1Msr.Field.ThresholdIntType == MCA_SMI)
            //re-init Error Thresholding ErrCnt
            McaThresholdInit (&RasMcaErrorInfo, BankIndex);
          }
        } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val)

        //Reset PPR flag for next bank
        AlreadyPPRepaired = FALSE;
      } //for (BankIndex = 0; BankIndex < BankNum; BankIndex++)
      if (RasThresholdPeriodicSmiEn) {
        ProcessPfehSmiSource (&SmiSourceChecked);
      }
    } //if (pLocalSmiStatusList[ProcessorNumber].Field.SmiSrcMca)

    //SMI from WRMSR
    if (pLocalSmiStatusList[ProcessorNumber].Field.WrMsr) {
      DEBUG ((EFI_D_ERROR, "[RAS]Local SMI Status: WrMsr\n"));
      //
      //About AmdRasSmmProtocol->ClrMcaStatus service:
      //  If the 3rd parameter: IsWrMsr is True, then the 2nd parameter: McaBankNumber can be ignored,
      //  because the real MCA bank number can be derived from MCA MSR address.
      //
      if (ReCloakCheck(ProcessorNumber, &BankIndex)) {
          //Cloak MCA register
          DEBUG ((EFI_D_ERROR, "Recloak Processor : %d Bank : 0x%x\n", ProcessorNumber, BankIndex));
          mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, LShiftU64 (1, BankIndex), 0);
      }
      Status = mAmdRasSmmProtocol->ClrMcaStatus (ProcessorNumber, 0, TRUE);
      if (Status == EFI_ABORTED) {
        //Un-cloak MCA register
        DEBUG ((EFI_D_ERROR, "Write MCA_STATUS aborted, uncloak processor: %d Bank : 0x%x\n", ProcessorNumber, BankIndex));
        mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
      }
    }
  }

  Status = gSmst->SmmFreePool (pLocalSmiStatusList);
  ASSERT_EFI_ERROR (Status);
  return EFI_SUCCESS;
}

EFI_STATUS
RasSmmRegisterMceSwSmi (
  VOID
  )
{
  EFI_STATUS                               Status;
  FCH_SMM_SW_DISPATCH2_PROTOCOL            *AmdSwDispatch;
  FCH_SMM_SW_REGISTER_CONTEXT              SwRegisterContext;
  EFI_HANDLE                               SwHandle;
  UINT16 SwSmiCmdPortAddr;
  UINT64 SmiTrigIoCycleData;
  UINT8 MceSwSmiData;

  //
  //  Locate SMM SW dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmSwDispatch2ProtocolGuid,
                  NULL,
                  &AmdSwDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_ERROR, "[RAS] Setup MCE software SMI redirection\n"));


  MceSwSmiData = mPlatformApeiData->PlatRasPolicy.MceSwSmiData;
  SwSmiCmdPortAddr = mPlatformApeiData->PlatRasPolicy.SwSmiCmdPortAddr;
  SmiTrigIoCycleData = MceSwSmiData;
  SmiTrigIoCycleData = ((SmiTrigIoCycleData << 16) | BIT25 | SwSmiCmdPortAddr);

  mAmdRasSmmProtocol->SetSmiTrigIoCycle (SmiTrigIoCycleData);

  SwRegisterContext.AmdSwValue  = MceSwSmiData; // use of PCD in place of MCE to Software SMI    0x80
  SwRegisterContext.Order  = 0x80;
  Status = AmdSwDispatch->Register (
                            AmdSwDispatch,
                            AmdMcetoSmiCallback,
                            &SwRegisterContext,
                            &SwHandle
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
TranslateToNormalizedAddress (
  IN       RAS_MCA_ERROR_INFO  *RasMcaErrorInfo,
  IN       UINT8               BankIndex,
  OUT      NORMALIZED_ADDRESS  *NormalizedAddress
  )
{
  if ((BankIndex == MCA_UMC0_BANK) | (BankIndex == MCA_UMC1_BANK)) {
    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val) {
      if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaConfigMsr.Field.LogDeferredInMcaStat == 1) {
        NormalizedAddress->normalizedAddr = RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaAddrMsr.Field.ErrorAddr >> (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaAddrMsr.Field.LSB);
        NormalizedAddress->normalizedAddr = NormalizedAddress->normalizedAddr << (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaAddrMsr.Field.LSB);
      } else {
        NormalizedAddress->normalizedAddr = RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaDeAddrMsr.Field.ErrorAddr >> (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaDeAddrMsr.Field.LSB);
        NormalizedAddress->normalizedAddr = NormalizedAddress->normalizedAddr << (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaDeAddrMsr.Field.LSB);
      }
      NormalizedAddress->normalizedSocketId = RasMcaErrorInfo->CpuInfo.SocketId;
      NormalizedAddress->normalizedDieId = RasMcaErrorInfo->CpuInfo.DieId;
      NormalizedAddress->normalizedChannelId = (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaBankNumber == 16) ? 1 : 0;
    } else {
      return EFI_UNSUPPORTED;
    }
  } else {
    return EFI_UNSUPPORTED;
  }
  return EFI_SUCCESS;
}

VOID
LogMCAError (
  IN       RAS_MCA_ERROR_INFO   *RasMcaErrorInfo
  )
{
  EFI_STATUS          Status;
  NORMALIZED_ADDRESS NormalizedAddress;
  UINT64             SystemMemoryAddress;
  DIMM_INFO          DimmInfo;
  UINT8              BankNum;
  UINT8              BankIndex;
  BOOLEAN            RasThresholdPeriodicSmiEn;
  UINT64             MsrData;
                                           
  RasThresholdPeriodicSmiEn = mPlatformApeiData->PlatRasPolicy.RasThresholdPeriodicSmiEn;

  MsrData = AsmReadMsr64 (MSR_MCG_CAP);            // MCG_CAP
  BankNum = (UINT8) (MsrData & 0xFF);

  if (!RasThresholdPeriodicSmiEn) {
    DEBUG ((EFI_D_ERROR, "Log MCA error entry\n"));
  }

  for (BankIndex = 0; BankIndex < BankNum; BankIndex++) {
    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val) {
      DEBUG ((EFI_D_ERROR, "Socket# %d, Die# %d, Ccx# %d, Core# %d, Thread# %d\n", RasMcaErrorInfo->CpuInfo.SocketId, RasMcaErrorInfo->CpuInfo.DieId, RasMcaErrorInfo->CpuInfo.CcxId, RasMcaErrorInfo->CpuInfo.CoreId, RasMcaErrorInfo->CpuInfo.ThreadID));
      DEBUG ((EFI_D_ERROR, "MCA Bank Number : %d\n", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaBankNumber));
      DEBUG ((EFI_D_ERROR, "MCA_STATUS : 0x%08x", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Value >> 32));
      DEBUG ((EFI_D_ERROR, "%08x\n", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Value & 0xFFFFFFFF));
      DEBUG ((EFI_D_ERROR, "MCA_ADDR : 0x%08x", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaAddrMsr.Value >> 32));
      DEBUG ((EFI_D_ERROR, "%08x\n", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaAddrMsr.Value & 0xFFFFFFFF));
      DEBUG ((EFI_D_ERROR, "MCA_SYND : 0x%08x", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaSyndMsr.Value >> 32));
      DEBUG ((EFI_D_ERROR, "%08x\n", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaSyndMsr.Value & 0xFFFFFFFF));
      DEBUG ((EFI_D_ERROR, "MCA_MISC0 : 0x%08x", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaMisc0Msr.Value >> 32));
      DEBUG ((EFI_D_ERROR, "%08x\n", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaMisc0Msr.Value & 0xFFFFFFFF));
      DEBUG ((EFI_D_ERROR, "MCA_MISC1 : 0x%08x", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaMisc1Msr.Value >> 32));
      DEBUG ((EFI_D_ERROR, "%08x\n", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaMisc1Msr.Value & 0xFFFFFFFF));
      Status = TranslateToNormalizedAddress (RasMcaErrorInfo, BankIndex, &NormalizedAddress);
      if (!EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "ERROR ADDRESS : 0x%08x", NormalizedAddress.normalizedAddr >> 32));
        DEBUG ((EFI_D_ERROR, "%08x\n", NormalizedAddress.normalizedAddr & 0xFFFFFFFF));
        DEBUG ((EFI_D_ERROR, "ERROR ADDRESS LSB : 0x%x\n", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaAddrMsr.Field.LSB));

        mAmdRasSmmProtocol->McaErrorAddrTranslate (&NormalizedAddress, &SystemMemoryAddress, &DimmInfo);
        DEBUG ((EFI_D_ERROR, "System Address Hi: 0x%08x\n", SystemMemoryAddress >> 32));
        DEBUG ((EFI_D_ERROR, "System Address Lo: 0x%08x\n", SystemMemoryAddress & 0xFFFFFFFF));

        DEBUG ((EFI_D_ERROR, "DIMM Info (Chip Select): 0x%x\n", DimmInfo.ChipSelect));
        DEBUG ((EFI_D_ERROR, "DIMM Info (Bank): 0x%x\n", DimmInfo.Bank));
        DEBUG ((EFI_D_ERROR, "DIMM Info (Row): 0x%x\n", DimmInfo.Row));
        DEBUG ((EFI_D_ERROR, "DIMM Info (Column): 0x%x\n", DimmInfo.Column));
        DEBUG ((EFI_D_ERROR, "DIMM Info (rankmul): 0x%x\n", DimmInfo.rankmul));
      } else {
        DEBUG ((EFI_D_ERROR, "MCA_ADDR : 0x%08x", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaAddrMsr.Value >> 32));
        DEBUG ((EFI_D_ERROR, "%08x\n", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaAddrMsr.Value & 0xFFFFFFFF));
        DEBUG ((EFI_D_ERROR, "MCA_SYND : 0x%08x", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaSyndMsr.Value >> 32));
        DEBUG ((EFI_D_ERROR, "%08x\n", RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaSyndMsr.Value & 0xFFFFFFFF));
      }
    }
  }
}

VOID
McaThresholdInit (
  IN       RAS_MCA_ERROR_INFO   *RasMcaErrorInfo,
  IN       UINT8                BankIndex
  )
{
  EFI_STATUS          Status;
  UINT8               *TmpBuffer;

  if (RasMcaErrorInfo->CpuInfo.ProcessorNumber > gSmst->NumberOfCpus) {
    return;
  }

  Status = gSmst->SmmAllocatePool (
                  EfiRuntimeServicesData,
                  (sizeof (RAS_MCA_ERROR_INFO) + 1),
                  (VOID *)&TmpBuffer
                  );

  ASSERT_EFI_ERROR (Status);
  CopyMem (TmpBuffer, RasMcaErrorInfo, sizeof (RAS_MCA_ERROR_INFO));
  (*(UINT8 *) (TmpBuffer + sizeof (RAS_MCA_ERROR_INFO))) = BankIndex;

  if (RasMcaErrorInfo->CpuInfo.ProcessorNumber == 0) {
    ErrorThresholdInit (TmpBuffer);
  } else {
    gSmst->SmmStartupThisAp (
            ErrorThresholdInit,
            RasMcaErrorInfo->CpuInfo.ProcessorNumber,
            TmpBuffer
            );
  }

  Status = gSmst->SmmFreePool (TmpBuffer);
  ASSERT_EFI_ERROR (Status);
}

VOID
ErrorThresholdInit (
  IN VOID *Buffer
  )
{
  UINT64 SaveHwcr;
  UINT64 MsrData;
  MCA_MISC0_MSR Misc0;
  MCA_MISC1_MSR Misc1;
  UINT32 MsrAddr;
  RAS_MCA_ERROR_INFO   *RasMcaErrorInfo;
  UINT8                BankIndex;
  UINT8                *TmpBuffer;

  TmpBuffer = (UINT8 *)Buffer;
  RasMcaErrorInfo = (RAS_MCA_ERROR_INFO *) (TmpBuffer);
  BankIndex = (*(UINT8 *) (TmpBuffer + sizeof (RAS_MCA_ERROR_INFO)));

  DEBUG ((EFI_D_ERROR, "Re-init MCA error thresholding counter entry\n"));

  SaveHwcr = AsmReadMsr64 (MSR_HWCR);
  MsrData = SaveHwcr | BIT18;
  AsmWriteMsr64 (MSR_HWCR, MsrData);

  //Check if ErrCnt was enabled.
  if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.CntEn &&
      RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.Ovrflw) {
    // MSR_C001_0015[18][McStatusWrEn] = 1

    MsrAddr = MCA_EXTENSION_BASE + (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaBankNumber << 4) | MCA_MISC0_OFFSET;
    Misc0.Value = AsmReadMsr64 (MsrAddr);
    Misc0.Field.CntEn = 0;
    Misc0.Field.ErrCnt = mPlatformApeiData->PlatRasPolicy.McaErrThreshCount;
    //Clear Ovrflw bit.
    Misc0.Field.Ovrflw = 0;

    AsmWriteMsr64 (MsrAddr, Misc0.Value);
    Misc0.Field.CntEn = 1;
    AsmWriteMsr64 (MsrAddr, Misc0.Value);
  }

  if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaMisc1Msr.Field.Valid &&
      RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaMisc1Msr.Field.CntEn &&
      RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaMisc1Msr.Field.Ovrflw) {
    MsrAddr = MCA_EXTENSION_BASE + (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaBankNumber << 4) | MCA_MISC1_OFFSET;
    Misc1.Value = AsmReadMsr64 (MsrAddr);
    Misc1.Field.CntEn = 0;
    Misc1.Field.ErrCnt = mPlatformApeiData->PlatRasPolicy.McaErrThreshCount;
    //Clear Ovrflw bit.
    Misc1.Field.Ovrflw = 0;

    AsmWriteMsr64 (MsrAddr, Misc1.Value);
    Misc1.Field.CntEn = 1;
    AsmWriteMsr64 (MsrAddr, Misc1.Value);
  }
  // Restore MSR_C001_0015[18][McStatusWrEn]
  AsmWriteMsr64 (MSR_HWCR, SaveHwcr);
}

VOID
MsrRegisterAccess (
  IN OUT   PLAT_RAS_MSR_ACCESS  *RasMsrAccess
)
{
    if (RasMsrAccess->IsWrite) {
      AsmWriteMsr64 (RasMsrAccess->RegisterAddress, RasMsrAccess->RegisterValue);
    } else {
      RasMsrAccess->RegisterValue = AsmReadMsr64 (RasMsrAccess->RegisterAddress);
    }
}

VOID
MpRegisterAccess (
  IN       UINTN                ProcessorNumber,
  IN OUT   PLAT_RAS_MSR_ACCESS  *RasMsrAccess
  )
{

  if (ProcessorNumber > gSmst->NumberOfCpus) {
    return;
  }

  DEBUG ((EFI_D_ERROR, "MSR Access @ Processor: %d, Address: 0x%08x, IsWrite (1:True): %d\n", ProcessorNumber, RasMsrAccess->RegisterAddress, RasMsrAccess->IsWrite));

  if (ProcessorNumber == 0) {
    MsrRegisterAccess(RasMsrAccess);
  } else {
    gSmst->SmmStartupThisAp (
             MsrRegisterAccess,
             ProcessorNumber,
             RasMsrAccess
             );
  }

}

BOOLEAN
ReCloakCheck(
  IN       UINTN    ProcessorNumber, 
  OUT      UINT8    *BankIndex
)
{
  BOOLEAN ReCloakFlag;
  BOOLEAN IsLegacyMcaAddr;
  UINT64 SmmSaveStateBase;
  SMM_SAVE_STATE *SmmSaveState;
  UINT32 ECX_Data;
  PLAT_RAS_MSR_ACCESS  RasMsrAccess;
  MCA_DESTAT_MSR McaDestatMsr;
  MCA_STATUS_MSR McaStatusMsr;
  UINT32 McaExtensionAddrBase;

  ReCloakFlag = FALSE;
  IsLegacyMcaAddr = FALSE;

  //Check RCX value if the address is in legacy MCA address range
  //if not then the address will be SMCA address.
  mAmdRasSmmProtocol->GetSmmSaveStateBase (ProcessorNumber, &SmmSaveStateBase);
  SmmSaveState = (SMM_SAVE_STATE*)SmmSaveStateBase;
  ECX_Data = (UINT32) (SmmSaveState->RCX & 0xFFFFFFFF);
  DEBUG ((EFI_D_ERROR, "ECX Data : 0x%08x\n", ECX_Data));
  if ((MCA_LEGACY_BASE <= ECX_Data) && (ECX_Data < MCA_LEGACY_TOP_ADDR)) {
    //Legacy MCA address
    *BankIndex = (UINT8) ((ECX_Data - MCA_LEGACY_BASE) >> 2);
    IsLegacyMcaAddr = TRUE;
  } else if ((ECX_Data == LMCA_STATUS_REG) || (ECX_Data == LMCA_ADDR_REG)){
    *BankIndex = (UINT8)(ECX_Data >> 2);
    IsLegacyMcaAddr = TRUE;
  } else {
    //Extension MCA Address
    *BankIndex = (UINT8) ((ECX_Data & 0x00000FF0) >> 4);
  }

  McaExtensionAddrBase = MCA_EXTENSION_BASE + (*BankIndex << 4);

  if (IsLegacyMcaAddr) {
    // If the OS is writing MCA_STATUS through the old address space, assume it is an SMCA-unaware OS that will never clear DESTAT
    if ((ECX_Data & MCA_REG_OFFSET_MASK) == MCA_STATUS_OFFSET) {
      DEBUG ((EFI_D_ERROR, "MCA_STATUS Write @ Processor: %d, Bank: 0x%x\n", ProcessorNumber, *BankIndex));
      //No-condition Clear MCA_DESTAT
      RasMsrAccess.RegisterAddress = McaExtensionAddrBase | MCA_DESTAT_OFFSET;
      RasMsrAccess.IsWrite = TRUE;
      RasMsrAccess.RegisterValue = 0;
      MpRegisterAccess (ProcessorNumber,&RasMsrAccess);

      ReCloakFlag = TRUE;
    }
  } else {
    RasMsrAccess.RegisterAddress = McaExtensionAddrBase | MCA_STATUS_OFFSET;
    RasMsrAccess.IsWrite = FALSE;
    MpRegisterAccess (ProcessorNumber,&RasMsrAccess);
    McaStatusMsr.Value = RasMsrAccess.RegisterValue;

    RasMsrAccess.RegisterAddress = McaExtensionAddrBase | MCA_DESTAT_OFFSET;
    RasMsrAccess.IsWrite = FALSE;
    MpRegisterAccess (ProcessorNumber,&RasMsrAccess);
    McaDestatMsr.Value = RasMsrAccess.RegisterValue;

    // If the OS is writing DESTAT
    if ((ECX_Data & SMCA_REG_OFFSET_MASK) == MCA_DESTAT_OFFSET) {
      DEBUG ((EFI_D_ERROR, "SMCA_DESTAT Write @ Processor: %d, Bank: 0x%x\n", ProcessorNumber, *BankIndex));
	  // If the MCA_STATUS does not contain an error
      if (McaStatusMsr.Field.Val == 0) {
        ReCloakFlag = TRUE;
      }
    }
    // If the OS is writing MCA_STATUS through the new address space, it is an SMCA-aware OS that will also clear DESTAT
    if ((ECX_Data & SMCA_REG_OFFSET_MASK) == MCA_STATUS_OFFSET) {
      DEBUG ((EFI_D_ERROR, "SMCA_STATUS Write @ Processor: %d, Bank: 0x%x\n", ProcessorNumber, *BankIndex));
     // If MCA_DESTAT does not contain an error, or it contains the same error as MCA_STATUS (STATUS[Deferred]==1)
      if ((McaDestatMsr.Field.Val== 0) || (McaStatusMsr.Field.Deferred && McaStatusMsr.Field.Val)) {
        //Clear MCA_DESTAT
        RasMsrAccess.RegisterAddress = McaExtensionAddrBase | MCA_DESTAT_OFFSET;
        RasMsrAccess.IsWrite = TRUE;
        RasMsrAccess.RegisterValue = 0;
        MpRegisterAccess (ProcessorNumber,&RasMsrAccess);
        ReCloakFlag = TRUE;
      }
    }
  }
  return ReCloakFlag;
}


/**
 * DimmPostPackageRepair
 *
 * Determines if a DDR Post Package Repair is needed and log it in the APCB.
 * Already know the MCA would be something like a threshold exceeded and would
 * want to log it.
 *
 * IN   RasMcaErrorInfo     Contains information about the MCi
 *
 */
EFI_STATUS
DimmPostPackageRepair (
  IN       RAS_MCA_ERROR_INFO   *RasMcaErrorInfo,
  IN       UINT8                BankIndex
  )
{
  AMD_APCB_SERVICE_PROTOCOL *AmdApcbService;
  DRRP_REPAIR_ENTRY         DpprEntry;
  EFI_STATUS                Status;
  NORMALIZED_ADDRESS        NormalizedAddress;
  UINT64                    SystemMemoryAddress;
  DIMM_INFO                 DimmInfo;
  UINT32                    DeviceStart;
  UINT32                    DeviceEnd;
  UINT8                     DeviceType;
  UINT32                    RepairDevice;
  BOOLEAN                   AtLeastOneRepair;

  AtLeastOneRepair = FALSE;

  // If it is not a DRAM error then we don't support it.
  if (!(RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaIpidMsr.Field.McaType == 0 &&
      RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaIpidMsr.Field.HardwareID == MCA_UMC_ID &&
      RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val == 1 &&
      RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCodeExt == 0)) {
    return EFI_UNSUPPORTED;
  }

  Status = gSmst->SmmLocateProtocol (
      &gAmdApcbSmmServiceProtocolGuid,
      NULL,
      (VOID**)&AmdApcbService
      );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "DimmPostPackageRepair: Could not locate gAmdApcbSmmServiceProtocolGuid.\n"));
    DEBUG ((DEBUG_ERROR, "DimmPostPackageRepair: Status=%r\n", Status));
    return Status;
  }

  // Translate MCA data into normalized address
  Status = TranslateToNormalizedAddress (
      RasMcaErrorInfo,
      BankIndex,
      &NormalizedAddress
      );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "DimmPostPackageRepair: Translate to normalized address failed.\n"));
    DEBUG ((DEBUG_ERROR, "DimmPostPackageRepair: Status=%r\n", Status));
    return Status;
  }

  // Translate Normalized Address into System address and DIMM Info
  Status = mAmdRasSmmProtocol->McaErrorAddrTranslate (
      &NormalizedAddress,
      &SystemMemoryAddress,
      &DimmInfo
      );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "DimmPostPackageRepair: Mca error address translation failed.\n"));
    DEBUG ((DEBUG_ERROR, "DimmPostPackageRepair: Status=%r\n", Status));
    return Status;
  }

  Status = mAmdRasSmmProtocol->MapSymbolToDramDevice (
    mAmdRasSmmProtocol,
    RasMcaErrorInfo,
    &NormalizedAddress,
    BankIndex,
    &DeviceStart,
    &DeviceEnd,
    &DeviceType
    );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "DimmPostPackageRepair: Map symbol to dram device failed.\n"));
    DEBUG ((DEBUG_ERROR, "DimmPostPackageRepair: Status=%r\n", Status));
    return Status;
  }

  // Generate repair entries for all indicated repairs
  for ( RepairDevice = DeviceStart;
      RepairDevice <= DeviceEnd;
      RepairDevice++) {
    // Build Post Package Repair entry
    DpprEntry.RepairType = DRAM_POST_PKG_SOFT_REPAIR;
    DpprEntry.Bank = DimmInfo.Bank;
    DpprEntry.Row = DimmInfo.Row;
    DpprEntry.Column = DimmInfo.Column;
    DpprEntry.RankMultiplier = DimmInfo.rankmul;
    DpprEntry.ChipSelect = DimmInfo.ChipSelect;
    DpprEntry.Socket = NormalizedAddress.normalizedSocketId;
    DpprEntry.Channel = NormalizedAddress.normalizedChannelId;
    // If it is a Deferred error, we only need to indicate to repair all
    // devices in one record. MapSymbolToDramDevice will return DeviceStart = 0
    // and DeviceEnd = last device for DeviceType.
    if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Deferred) {
      DpprEntry.Device = DeviceType;
      DpprEntry.TargetDevice = 0;
    } else {
      DpprEntry.Device = 0x1F;
      DpprEntry.TargetDevice = RepairDevice;
    }
#if 1
    DEBUG ((DEBUG_ERROR, "Adding DIMM Post Package Repair Entry to APCB:\n"));
    DEBUG ((DEBUG_ERROR, "Repair Type = Soft Repair\n"));
    DEBUG ((DEBUG_ERROR, "Bank = 0x%X\n", DpprEntry.Bank));
    DEBUG ((DEBUG_ERROR, "Row = 0x%X\n", DpprEntry.Row));
    DEBUG ((DEBUG_ERROR, "Column = 0x%X\n", DpprEntry.Column));
    DEBUG ((DEBUG_ERROR, "Rank Multiplier = 0x%X\n", DpprEntry.RankMultiplier));
    DEBUG ((DEBUG_ERROR, "Chipselect = 0x%X\n", DpprEntry.ChipSelect));
    DEBUG ((DEBUG_ERROR, "Socket = 0x%X\n", DpprEntry.Socket));
    DEBUG ((DEBUG_ERROR, "Channel = 0x%X\n", DpprEntry.Channel));
    DEBUG ((DEBUG_ERROR, "Device = 0x%X\n", DpprEntry.Device));
    DEBUG ((DEBUG_ERROR, "TargetDevice = 0x%X\n", DpprEntry.TargetDevice));
#endif

    // Insert Post Package repair entry
    Status = AmdApcbService->ApcbAddDramPostPkgRepairEntry (
        AmdApcbService,
        &DpprEntry
        );
    if (!EFI_ERROR (Status)) {
      AtLeastOneRepair = TRUE;
      if ( DpprEntry.Device != 0x1F ) {
        // Translated all device repair to one record, so break.
        break;
      }
    } else {
      DEBUG ((DEBUG_ERROR, "Failed Inserting DIMM Post Package Repair Entry:\n"));
      DEBUG ((DEBUG_ERROR, "DimmPostPackageRepair: Status=%r\n", Status));
      break;
    }
  }
  // If we successfully added Post Package Repair records, then flush back
  // to SPI
  if (AtLeastOneRepair) {
    Status = AmdApcbService->ApcbFlushData (AmdApcbService);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed Flushing DIMM Post Package Repair Entries:\n"));
      DEBUG ((DEBUG_ERROR, "DimmPostPackageRepair: Status=%r\n", Status));
    }
  }
  return Status;
}

UINT64
ConvertToMilliseconds (
  IN       UINT64 TscTicks
  )
{
  UINT32      CpuFid;
  UINT32      CpuDfsId;
  UINT64      TscRateInMhz;
  UINT64      NumOfTicksPerMilliSec;
  PSTATE_MSR  PstateMsr;

  TscRateInMhz = 0;

  // The TSC increments at the rate specified by the P0 Pstate.
  // Core current operating frequency in MHz (CoreCOF) =
  //   (Core::X86::Msr::PStateDef[CpuFid[7:0]]/Core::X86::Msr::PStateDef[CpuDfsId])*200.
  PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0);
  CpuFid   = (UINT32) PstateMsr.Field.CpuFid_7_0;
  CpuDfsId = (UINT32) PstateMsr.Field.CpuDfsId;
  if (CpuDfsId == 0) {
    TscRateInMhz = 0;
    ASSERT (FALSE);
  } else if ((CpuDfsId >= 8) && (CpuDfsId <= 0x30)) {
    TscRateInMhz = (UINTN) ((200 * CpuFid) / CpuDfsId);
  }
  NumOfTicksPerMilliSec = 1000 * TscRateInMhz;
  
  return (TscTicks/NumOfTicksPerMilliSec);
}

VOID
CpmSetMcaThreshold (
  IN       RAS_THRESHOLD_CONFIG* RasThresholdConfig
  )
{
  UINT8 BankNum;
  UINT64 McMisc0;
  UINT64 MsrData;
  UINT32 i;
  UINT64 SaveHwcr;


  MsrData = AsmReadMsr64 (MSR_MCG_CAP);            // MCG_CAP
  BankNum = (UINT8)(MsrData & 0xFF);

  McMisc0 = RasThresholdConfig->ThresholdIntType;
  McMisc0 = (UINT64)(((McMisc0 << 17 | (RasThresholdConfig->ThresholdCount & 0x0FFF)) << 32) | BIT51);

  if (RasThresholdConfig->ThresholdControl) {
    // MSR_C001_0015[18][McStatusWrEn] = 1
    SaveHwcr = AsmReadMsr64 (MSR_HWCR);
    MsrData = SaveHwcr | BIT18;
    AsmWriteMsr64 (MSR_HWCR, MsrData);

    for (i = 0; i < BankNum; i++) {
      AsmMsrAndThenOr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_MISC0_OFFSET), 0xFFF1F000FFFFFFFF, McMisc0);
      if ((MCA_UMC0_BANK == i) || (MCA_UMC1_BANK == i)) {
        AsmMsrAndThenOr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_MISC1_OFFSET), 0xFFF1F000FFFFFFFF, McMisc0);
      }
    }
    // Restore MSR_C001_0015[18][McStatusWrEn]
    AsmWriteMsr64 (MSR_HWCR, SaveHwcr);
  }
}

VOID
CpmSetMpMcaThreshold (
  IN       RAS_THRESHOLD_CONFIG* RasThresholdConfig
  )
{
  UINT16      i;

  //Program BSP first
  CpmSetMcaThreshold(RasThresholdConfig);

  //Program AP
  for (i = 1; i < gSmst->NumberOfCpus; i++) {
    gSmst->SmmStartupThisAp (
            CpmSetMcaThreshold,
            i,
            (VOID *) RasThresholdConfig
            );
  }
}

BOOLEAN
FindThresholdOrDeferredError (
  VOID
  )
{
  EFI_STATUS         Status = EFI_SUCCESS;
  UINTN              ProcessorNumber;
  RAS_MCA_ERROR_INFO RasMcaErrorInfo;
  UINT8              BankNum;
  UINT8              BankIndex;
  UINT64             MsrData;
  BOOLEAN            AlreadyPPRepaired;
  BOOLEAN            ErrorFound;

  MsrData = AsmReadMsr64 (MSR_MCG_CAP);            // MCG_CAP
  BankNum = (UINT8) (MsrData & 0xFF);
  ErrorFound = FALSE;

  for (ProcessorNumber = 0; ProcessorNumber < gSmst->NumberOfCpus; ProcessorNumber++) {
    ZeroMem (&RasMcaErrorInfo, sizeof (RasMcaErrorInfo));
    RasMcaErrorInfo.CpuInfo.ProcessorNumber = ProcessorNumber;
  
    //Collect Threshold and Deferred error
    mAmdRasSmmProtocol->SearchMcaError (&RasMcaErrorInfo);
    LogMCAError (&RasMcaErrorInfo);

    //Here is default handle start
    for (BankIndex = 0; BankIndex < BankNum; BankIndex++) {
      if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val) {
        // DEFERRED ERROR
        //Check source is from Deferred error
        if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Deferred) {
          // Check for Dimm Post Package repair on any Deferred
          if (!AlreadyPPRepaired) {
            Status = DimmPostPackageRepair (&RasMcaErrorInfo, BankIndex);
            if (!EFI_ERROR (Status)) {
              AlreadyPPRepaired = TRUE;
            }
          }
          if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaConfigMsr.Field.DeferredIntType == MCA_APIC) {
            //A deferred error LVT interrupt is generated upon SMM exit
            mAmdRasSmmProtocol->RasSmmExitType (ProcessorNumber, GENERATE_DEFERREDLVT_ON_EXIT);
            //Un-Cloak MCA register
            mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
          } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaConfigMsr.Field.DeferredIntType == MCA_APIC)
          if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaConfigMsr.Field.DeferredIntType == MCA_SMI) {
            mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
          } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaConfigMsr.Field.DeferredIntType == MCA_SMI)
          ErrorFound = TRUE;
        } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Deferred)

        // MISC 0
        // Only check threshold status if there was an overflow
        if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.Ovrflw) {
          // Check for Dimm Post Package repair on any overflow
          if (!AlreadyPPRepaired) {
            Status = DimmPostPackageRepair (&RasMcaErrorInfo, BankIndex);
            if (!EFI_ERROR (Status)) {
              AlreadyPPRepaired = TRUE;
            }
          }
          //Check Error Threshold interrupt type.
          if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.ThresholdIntType == MCA_APIC) {
            //Set GenerateThresholdLvtOnExit
            mAmdRasSmmProtocol->RasSmmExitType (ProcessorNumber, GENERATE_THRESHOLDLVT_ON_EXIT);
            //Un-Cloak MCA register
            mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
          } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.ThresholdIntType == MCA_APIC)

          // Since the ThresholdIntType is cleared in Polling mode, so we assume the ThresholdIntType is MCA_SMI in here.
          if ((RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.ThresholdIntType == MCA_SMI) ||
              (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.ThresholdIntType == MCA_NO_INTERRUPT)) {
            if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC) {
              //A machine check execption is generated to local core upon SMM exit if a uncorrectable error.
              mAmdRasSmmProtocol->RasSmmExitType (ProcessorNumber, GENERATE_MCE_ON_EXIT);
            }
            //Un-Cloak MCA register
            mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
          } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc0Msr.Field.ThresholdIntType == MCA_SMI)
          //re-init Error Thresholding ErrCnt
          McaThresholdInit (&RasMcaErrorInfo, BankIndex);
          ErrorFound = TRUE;
        }

        // MISC 1
        // Only check threshold status if there was an overflow
        if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc1Msr.Field.Ovrflw) {
          // Check for Dimm Post Package repair on any overflow
          if (!AlreadyPPRepaired) {
            Status = DimmPostPackageRepair (&RasMcaErrorInfo, BankIndex);
            if (!EFI_ERROR (Status)) {
              AlreadyPPRepaired = TRUE;
            }
          }
          //Check Error Threshold interrupt type.
          if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc1Msr.Field.ThresholdIntType == MCA_APIC) {
            //Set GenerateThresholdLvtOnExit
            mAmdRasSmmProtocol->RasSmmExitType (ProcessorNumber, GENERATE_THRESHOLDLVT_ON_EXIT);
            //Un-Cloak MCA register
            mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
          } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc1Msr.Field.ThresholdIntType == MCA_APIC)
          
          // Since the ThresholdIntType is cleared in Polling mode, so we assume the ThresholdIntType is MCA_SMI in here.
          if ((RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc1Msr.Field.ThresholdIntType == MCA_SMI) ||
              (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc1Msr.Field.ThresholdIntType == MCA_NO_INTERRUPT)) {
            if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC) {
              //A machine check execption is generated to local core upon SMM exit if a uncorrectable error.
              mAmdRasSmmProtocol->RasSmmExitType (ProcessorNumber, GENERATE_MCE_ON_EXIT);
            }
            //Un-Cloak MCA register
            mAmdRasSmmProtocol->SetMcaCloakCfg (ProcessorNumber, 0, LShiftU64 (1, BankIndex));
          } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaMisc1Msr.Field.ThresholdIntType == MCA_SMI)
          //re-init Error Thresholding ErrCnt
          McaThresholdInit (&RasMcaErrorInfo, BankIndex);
          ErrorFound = TRUE;
        }
      } //if (RasMcaErrorInfo.McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val)

      //Reset PPR flag for next bank
      AlreadyPPRepaired = FALSE;
    } //for (BankIndex = 0; BankIndex < BankNum; BankIndex++)      
  }

  return ErrorFound;
}

// Goal... limit the system on average to 5 interrupts per 1000ms or less
// If we exceed the limit, enter periodic polling mode
//
// If we are in periodic polling mode, and no errors occur during the 1000ms
// time scale, return to interrupt mode.
//
// mTscLast = <TSC>
// mSmiMode = Interrupt (default)
// mSmiCount = 0 (default)
// 
// SMI_threshold = PcdCpmSmiThreshold (default: 5 interrupts)
// SMI_scale = PcdCpmSmiScale (default: 1000 ms)
// 
// SmiRate (SmiPeriod) = SMI_scale / SMI_threshold
// mSmiCoolOff = 0;
// 
// TimeSinceLastSmi (Uninitilized)
VOID 
ProcessPfehSmiSource (
  BOOLEAN  *SmiSourceChecked
  )
{
  UINT64                  TimeSinceLastSmi;
  UINT32                  SmiRate;
  UINT64                  MsrData;
  RAS_THRESHOLD_CONFIG    RasThresholdConfig;
  UINT64                  TscCurrent;
  BOOLEAN                 McaErrThreshEn;
  UINT16                  McaErrThreshCount;
  UINT32                  SmiThreshold;
  UINT32                  SmiScale;
  EFI_STATUS              Status;

  if (mSmiMode != INTERRUPT_MODE)
    return;

  if (*SmiSourceChecked)
    return;

  *SmiSourceChecked = TRUE;

  mSmiCount++;
  DEBUG ((DEBUG_ERROR, "INTERRUPT_MODE - Entry: Before Leaky bucket process => mSmiCount = 0x%08x\n", mSmiCount));

  McaErrThreshEn = mPlatformApeiData->PlatRasPolicy.McaErrThreshEn;
  McaErrThreshCount = mPlatformApeiData->PlatRasPolicy.McaErrThreshCount;
  SmiThreshold = mPlatformApeiData->PlatRasPolicy.RasSmiThreshold;
  SmiScale = mPlatformApeiData->PlatRasPolicy.RasSmiScale;
  //Ex: SmiRate = (SmiScale: 1000, unit: ms)/(SmiThreshold: 5 interrupts) = 200ms 
  SmiRate = SmiScale/SmiThreshold;   

  // Leak events out of the leaky bucket based on the time since the last SMI
  TscCurrent = AsmReadMsr64(TSC);
  TimeSinceLastSmi = ConvertToMilliseconds((TscCurrent - mTscLast));
  while ((TimeSinceLastSmi > SmiRate) && (mSmiCount > 0)) {
    mSmiCount--;
    TimeSinceLastSmi-=SmiRate;
  }
  
  // Save the current TSC to the last TSC
  mTscLast = TscCurrent;
  DEBUG ((DEBUG_ERROR, "  INTERRUPT_MODE: After Leaky bucket process => mSmiCount = 0x%08x\n", mSmiCount));
  if (mSmiCount > SmiThreshold) {
    // This event caused the Leaky bucket to overflow, enable polled mode via periodic SMIs
    DEBUG ((DEBUG_ERROR, "  INTERRUPT_MODE: mSmiCount: 0x%04x > SmiThreshold: 0x%04x => Leaky bucket overflow, Start switching to POLLING_MODE\n", 
                         mSmiCount, SmiThreshold));
    
    //Try to turn on periodic SMI's at rate of SmiRate
    Status = RasSmmRegisterMcePeriodicSmi ();
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "INTERRUPT_MODE - Exit: Register AmdMcePeriodicSmiCallback failed: 0x%08x, cannot switch to POLLING_MODE.\n", Status));
      return;
    }

    mSmiMode = POLLING_MODE;
    mSmiCoolOff = SmiThreshold;
    
    //Disable SMI generation for Machine Check Events 
    //01.
    RasThresholdConfig.ThresholdControl = McaErrThreshEn;
    RasThresholdConfig.ThresholdCount = McaErrThreshCount;
    RasThresholdConfig.ThresholdIntType = MCA_NO_INTERRUPT; // No interrupt   
    CpmSetMpMcaThreshold(&RasThresholdConfig);
    //02.
    MsrData = 0xFFFFFFFFFFFFFFFF;
    AsmWriteMsr64 (MSR_PFEH_DEF_INT_MASK, MsrData);

    DEBUG ((DEBUG_ERROR, "INTERRUPT_MODE - Exit: Successfully switched to POLLING_MODE\n"));
  } else {
    DEBUG ((DEBUG_ERROR, "INTERRUPT_MODE - Exit: Leaky bucket has not overflowed\n"));
  }

  return;
}

VOID 
ProcessPeriodicSMI (
  VOID
  )
{
  UINT64                  MsrData;
  RAS_THRESHOLD_CONFIG    RasThresholdConfig;
  UINT32                  SmiThreshold;
  BOOLEAN                 McaErrThreshEn;
  UINT16                  McaErrThreshCount;
  EFI_STATUS              Status = EFI_SUCCESS;
  
  McaErrThreshEn = mPlatformApeiData->PlatRasPolicy.McaErrThreshEn;
  McaErrThreshCount = mPlatformApeiData->PlatRasPolicy.McaErrThreshCount;
  SmiThreshold = mPlatformApeiData->PlatRasPolicy.RasSmiThreshold;

  if (mSmiMode == POLLING_MODE) {
    DEBUG ((DEBUG_ERROR, "POLLING_MODE - Entry\n"));
    if (FindThresholdOrDeferredError()) {
      // An error was detected, reset the threshold to max
      mSmiCoolOff = SmiThreshold;
      DEBUG ((DEBUG_ERROR, "POLLING_MODE - Exit: An error was detected, reset the mSmiCoolOff to max: 0x%08x\n", mSmiCoolOff));
    } else {
      mSmiCoolOff--;
      DEBUG ((DEBUG_ERROR, "  POLLING_MODE: No Error, mSmiCoolOff-- = 0x%08x\n", mSmiCoolOff));
      if (mSmiCoolOff == 0) {
        // If we go one full leaky bucket time scale with no errors, return to interrupt mode
        DEBUG ((DEBUG_ERROR, "  POLLING_MODE: mSmiCoolOff == 0x%08x => Go one full leaky bucket time scale with no errors => Start returning to INTERRUPT_MODE.\n", 
                                mSmiCoolOff));

        //Try to turn off periodic SMI's for PFEH polling
        if (mPollingModeHandle == NULL) {
          DEBUG ((DEBUG_ERROR, "POLLING_MODE - Exit: cannot locate mPollingModeHandle, fail to return to INTERRUPT_MODE\n"));
          return;
        }

        Status = mAmdPeriodicalDispatch->UnRegister (
                                           mAmdPeriodicalDispatch, 
                                           mPollingModeHandle);
        if (EFI_ERROR(Status)) {
          DEBUG ((DEBUG_ERROR, "POLLING_MODE - Exit: cannot UnrRgister AmdMcePeriodicSmiCallback, fail to return to INTERRUPT_MODE\n"));
          return;
        }
    
        mPollingModeHandle = NULL;
        mSmiMode = INTERRUPT_MODE;
        mSmiCount  = 0;

        //Enable SMI generation for Machine Check Events
        //01.
        RasThresholdConfig.ThresholdControl = McaErrThreshEn;
        RasThresholdConfig.ThresholdCount = McaErrThreshCount;
        RasThresholdConfig.ThresholdIntType = MCA_SMI; // SMI trigger event  
        CpmSetMpMcaThreshold(&RasThresholdConfig);
        //02.
        MsrData = 0x0000000000000000;
        AsmWriteMsr64 (MSR_PFEH_DEF_INT_MASK, MsrData);
    
        DEBUG ((DEBUG_ERROR, "POLLING_MODE - Exit: Successfully return to INTERRUPT_MODE\n"));
      } else {
      DEBUG ((DEBUG_ERROR, "POLLING_MODE - Exit: No error duration time is less than SMI time scale\n"));
      }
    }
  }

  return;
}

EFI_STATUS
EFIAPI
AmdMcePeriodicSmiCallback (
  IN       EFI_HANDLE                                DispatchHandle,
  IN       CONST FCH_SMM_PERIODICAL_REGISTER_CONTEXT *RegisterContext,
  IN OUT   EFI_SMM_PERIODIC_TIMER_CONTEXT            *PeriodicTimerContext,
  IN OUT   UINTN                                     *SizeOfContext
  )
{

  ProcessPeriodicSMI ();

  return EFI_SUCCESS;
}

EFI_STATUS
RasSmmRegisterMcePeriodicSmi (
  VOID
  )
{
  EFI_STATUS                               Status;
  FCH_SMM_PERIODICAL_REGISTER_CONTEXT      PeriodicalRegisterContext; 
  UINT32                                   SmiRate;
  UINT32                                   SmiThreshold;
  UINT32                                   SmiScale;

  SmiThreshold = mPlatformApeiData->PlatRasPolicy.RasSmiThreshold;
  SmiScale = mPlatformApeiData->PlatRasPolicy.RasSmiScale;

  //
  // Periodic Timer SMI Registration
  //
  Status = gSmst->SmmLocateProtocol (
                    &gFchSmmPeriodicalDispatch2ProtocolGuid,
                    NULL,
                    &mAmdPeriodicalDispatch
                    );
  
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //SmiRate = (SmiScale: 1000, unit: ms)/(SmiThreshold: 5 interrupts) = 200ms
  SmiRate = SmiScale/SmiThreshold;

  //SmiTrig0, In FchSmmPeriodicalDispatcher.c:
  //  if (SmiTickInterval == LONG_TIMER_SMI_INTERVAL) then {FCH_SMI_REG98 |= BIT29}
  //  FCH_SMI_REG98[29] = 1 => SmiTimer to be SmiLongTimer register and long timer runs at 1 ms unit time.
  PeriodicalRegisterContext.SmiTickInterval  = LONG_TIMER_SMI_INTERVAL;
  
  //SmiTimer, FCH_SMI_REG96 = (UINT16) (Period / SmiTickInterval) & 0x7FFF;
  PeriodicalRegisterContext.Period           = SmiRate * LONG_TIMER_SMI_INTERVAL;
  
  //SmiTimer, FCH_SMI_REG96 |= SMI_TIMER_ENABLE
  PeriodicalRegisterContext.StartNow         = 1;

  mPollingModeHandle = NULL;
  Status = mAmdPeriodicalDispatch->Register (
                                     mAmdPeriodicalDispatch,
                                     AmdMcePeriodicSmiCallback,
                                     &PeriodicalRegisterContext,
                                     &mPollingModeHandle
                                     );

  return Status;
}

