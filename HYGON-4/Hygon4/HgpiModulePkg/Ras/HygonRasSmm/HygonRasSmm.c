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
#include <Filecode.h>
#include "HygonRasSmm.h"
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Protocol/HygonRasSmmProtocol.h>
#include <Protocol/HygonMemPprProtocol.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/SmnAccessLib.h>
#include "HYGON.h"
#include "Library/IdsLib.h"
#include <CddRegistersDm.h>
#include <Library/RasMcaLib.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  RAS_HYGONRASSMM_HYGONRASSMM_FILECODE

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
ADDR_DATA         *gAddrData;
HYGON_RAS_POLICY  *mHygonRasPolicy;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

EFI_STATUS
EFIAPI
HygonMcaErrorAddrTranslate (
  IN       NORMALIZED_ADDRESS *NormalizedAddress,
  OUT      UINT64             *SystemMemoryAddress,
  OUT      DIMM_INFO          *DimmInfo
  );

EFI_STATUS
EFIAPI
HygonTranslateSysAddrToCS (
  IN       UINT64             *SystemMemoryAddress,
  OUT      NORMALIZED_ADDRESS *NormalizedAddress,
  OUT      DIMM_INFO          *DimmInfo
  );

EFI_STATUS
EFIAPI
HygonSetSmiTrigIoCycle (
  IN       UINT64 SmiTrigIoCycleData
  );

EFI_STATUS
EFIAPI
HygonGetAllLocalSmiStatus (
  IN       LOCAL_SMI_STATUS *pLocalSmiStatusList
  );

EFI_STATUS
EFIAPI
HygonSearchMcaError (
  IN OUT   RAS_MCA_ERROR_INFO *RasMcaErrorInfo
  );

EFI_STATUS
EFIAPI
HygonRasSmmExitType (
  IN       UINTN ProcessorNumber,
  IN       UINTN SmiExitType
  );

EFI_STATUS
EFIAPI
HygonGetSmmSaveStateBase (
  IN       UINTN   ProcessorNumber,
  OUT      UINT64 *SmmSaveStateBase
  );

EFI_STATUS
EFIAPI
HygonSetMcaCloakCfg (
  IN       UINTN  ProcessorNumber,
  IN       UINT64 CloakValue,
  IN       UINT64 UnCloakValue
  );

EFI_STATUS
EFIAPI
HygonClrMcaStatus (
  IN       UINTN            ProcessorNumber,
  IN       UINTN            McaBankNumber,
  IN       BOOLEAN          IsWrMsr
  );

EFI_STATUS
EFIAPI
HygonMapSymbolToDramDevice (
  IN       HYGON_RAS_SMM_PROTOCOL *This,
  IN       RAS_MCA_ERROR_INFO   *RasMcaErrorInfo,
  IN       NORMALIZED_ADDRESS   *NormalizedAddress,
  IN       UINT8                BankIndex,
  OUT      UINT32               *DeviceStart,
  OUT      UINT32               *DeviceEnd,
  OUT      UINT8                *DeviceType
  );

STATIC
VOID
retrieve_regs (
  UINTN   pkgno,
  UINTN   mpuno,
  UINTN   umcno,
  UINTN   umcchno,
  UINTN   BusNumberBase
  );

EFI_STATUS
GetPostpackageRepairInfo (
  IN       HYGON_DIMM_INFO    *HygonDimmInfo,
  OUT      HYGON_PPR_INFO     *PprInfo
  );

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
STATIC HYGON_RAS_SMM_PROTOCOL  HygonRasSmmProtocol = {
  HygonMcaErrorAddrTranslate,
  HygonTranslateSysAddrToCS,
  HygonSetSmiTrigIoCycle,
  HygonGetAllLocalSmiStatus,
  HygonSearchMcaError,
  HygonRasSmmExitType,
  HygonGetSmmSaveStateBase,
  HygonSetMcaCloakCfg,
  HygonClrMcaStatus,
  HygonMapSymbolToDramDevice
};

/*********************************************************************************
 * Name: HygonRasSmmInit
 *
 * Description
 *   Entry point of the HYGON RAS SMM driver
 *   Register Ras Smm callbacks
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
HygonRasSmmInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )

{
  EFI_STATUS            Status;
  EFI_HANDLE            Handle = NULL;
  RAS_THRESHOLD_CONFIG  RasThresholdConfig;

  // Get HYGON Ras Policy
  Status = gBS->LocateProtocol (&gHygonRasInitDataProtocolGuid, NULL, &mHygonRasPolicy);
  if (EFI_ERROR (Status)) {
    ASSERT (!EFI_ERROR (Status));
    return Status;    // Error detected while trying to locate pool
  }

  // Init memory address data pointer
  gAddrData = mHygonRasPolicy->AddrData;

  // PFEH enabled, firmware should setup the Error Thresholding and set interrupt type to SMI.
  // PFEH Disabled, OS will enable error threshold.
  if (mHygonRasPolicy->PFEHEnable) {
    DEBUG ((EFI_D_ERROR, "[RAS] Platform-First Error Handle Enabled!!!\n"));
    // Set ECC error threshold through all banks.
    RasThresholdConfig.ThresholdControl = mHygonRasPolicy->McaErrThreshEn;
    RasThresholdConfig.ThresholdCount   = mHygonRasPolicy->McaErrThreshCount;
    RasThresholdConfig.NonMemThreshCount= mHygonRasPolicy->McaNonMemErrThresh;      // byo231109 +
    RasThresholdConfig.ThresholdIntType = 2;          // SMI

    if (mHygonRasPolicy->McaErrThreshEn) {
// byo231109 - >>	
      DEBUG ((EFI_D_INFO, "McaErrThreshCount:%d McaNonMemErrThresh:%d\n", \
        mHygonRasPolicy->McaErrThreshCount, mHygonRasPolicy->McaNonMemErrThresh));
// byo231109 - <<		
    }

    SetMpMcaThreshold (&RasThresholdConfig);
  }

  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gHygonRasSmmProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &HygonRasSmmProtocol
                    );

  return Status;
}

/*---------------------------------------------------------------------------------------*/

/**
 * MCA_ADDR Address Translate
 *
 * Translate UMC local address into specific memory DIMM information and system address
 *
 *
 * @param[in]   NormalizedAddress      UMC memory address Information
 * @param[out]  SystemMemoryAddress    System Address
 * @param[out]  DimmInfo               DIMM information
 *
 */
EFI_STATUS
EFIAPI
HygonMcaErrorAddrTranslate (
  IN       NORMALIZED_ADDRESS *NormalizedAddress,
  OUT      UINT64             *SystemMemoryAddress,
  OUT      DIMM_INFO          *DimmInfo
  )
{
  DEBUG ((EFI_D_ERROR, "[RAS]NormalizedAddr: 0x%lx\n", NormalizedAddress->normalizedAddr));
  DEBUG ((
    EFI_D_ERROR,
    "[RAS]NormalizedSocId: 0x%x, CddId: 0x%x, ChannelId: 0x%x, SubChannelId: 0x%x\n",
    NormalizedAddress->normalizedSocketId,
    NormalizedAddress->normalizedCddId,
    NormalizedAddress->normalizedChannelId,
    NormalizedAddress->normalizedSubChannelId
    ));

  translate_norm_to_dram_addr (
    NormalizedAddress->normalizedAddr,
    NormalizedAddress->normalizedSocketId,
    NormalizedAddress->normalizedCddId,
    NormalizedAddress->normalizedChannelId,
    NormalizedAddress->normalizedSubChannelId,
    &DimmInfo->ChipSelect,
    &DimmInfo->Bank,
    &DimmInfo->Row,
    &DimmInfo->Column,
    &DimmInfo->rankmul
    );

  *SystemMemoryAddress = calcSysAddr (
                           NormalizedAddress->normalizedAddr,
                           NormalizedAddress->normalizedSocketId,
                           NormalizedAddress->normalizedCddId,
                           NormalizedAddress->normalizedChannelId,
                           NormalizedAddress->normalizedSubChannelId
                           );

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/

/**
 * System Address Translate
 *
 * Translate system address into specific memory DIMM information and normalized address
 * information
 *
 * @param[in]   SystemMemoryAddress    System Address
 * @param[out]  NormalizedAddress      UMC memory address Information
 * @param[out]  DimmInfo               DIMM information
 *
 */
EFI_STATUS
EFIAPI
HygonTranslateSysAddrToCS (
  IN       UINT64             *SystemMemoryAddress,
  OUT      NORMALIZED_ADDRESS *NormalizedAddress,
  OUT      DIMM_INFO          *DimmInfo
  )
{
  *NormalizedAddress = calcNormAddr (*SystemMemoryAddress);

  translate_norm_to_dram_addr (
    NormalizedAddress->normalizedAddr,
    NormalizedAddress->normalizedSocketId,
    NormalizedAddress->normalizedCddId,
    NormalizedAddress->normalizedChannelId,
    NormalizedAddress->normalizedSubChannelId,
    &DimmInfo->ChipSelect,
    &DimmInfo->Bank,
    &DimmInfo->Row,
    &DimmInfo->Column,
    &DimmInfo->rankmul
    );

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/

/**
 * UMC Address Translate
 *
 * Translate physical address into specific memory channel information
 *
 *
 * @param[in/out]  RasDimmInfo      UMC memory address Information
 *
 */
EFI_STATUS
EFIAPI
HygonSetSmiTrigIoCycle (
  IN       UINT64 SmiTrigIoCycleData
  )
{
  RAS_BSP_AP_MSR_SYNC  ApMsrSync[3];
  UINT16               i;

  // RedirSmiEn = 1
  AsmMsrOr64 (MSR_MCEXCEPREDIR, BIT9);
  // Set SmiTrigIoCycle
  AsmMsrAndThenOr64 (MSR_SMITRIGIOCYCLE, 0xFFFFFFF800000000, SmiTrigIoCycleData);

  ZeroMem (ApMsrSync, sizeof (ApMsrSync));

  i = 0;

  ApMsrSync[i].RegisterAddress = MSR_MCEXCEPREDIR;
  ApMsrSync[i++].RegisterMask  = BIT9;

  ApMsrSync[i].RegisterAddress = MSR_SMITRIGIOCYCLE;
  ApMsrSync[i++].RegisterMask  = 0x0000000FFFFFFFFF;

  LibRasSmmSyncMsr (ApMsrSync);

  return EFI_SUCCESS;
}

VOID
GetApLocalSmiStatus (
  LOCAL_SMI_STATUS *LocalSmiStatus
  )
{
  SMM_SAVE_STATE  *ApSmmSaveState;

  ApSmmSaveState = (SMM_SAVE_STATE *)(AsmReadMsr64 (MSR_SMM_BASE)+ SMM_SAVE_STATE_OFFSET);
  LocalSmiStatus->Value = (UINT64)ApSmmSaveState->LocalSmiStatus;
}

EFI_STATUS
EFIAPI
HygonGetAllLocalSmiStatus (
  IN       LOCAL_SMI_STATUS *pLocalSmiStatusList
  )
{
  UINTN           ProcessorNumber;
  SMM_SAVE_STATE  *BspSmmSaveState;

  BspSmmSaveState = (SMM_SAVE_STATE *)(AsmReadMsr64 (MSR_SMM_BASE) + SMM_SAVE_STATE_OFFSET);

  pLocalSmiStatusList[0].Value = (UINT64)BspSmmSaveState->LocalSmiStatus;
  if (pLocalSmiStatusList[0].Value != 0) {
    if (!PcdGetBool(PcdEccLeakyBucketEnbale))
      DEBUG ((EFI_D_ERROR, "CPU0 Local SMI Status = 0x%08x\n", pLocalSmiStatusList[0].Value));
  }

  for (ProcessorNumber = 1; ProcessorNumber < gSmst->NumberOfCpus; ProcessorNumber++) {
    gSmst->SmmStartupThisAp (
             GetApLocalSmiStatus,
             ProcessorNumber,
             &pLocalSmiStatusList[ProcessorNumber]
             );
    if (pLocalSmiStatusList[ProcessorNumber].Value != 0) {
      if (!PcdGetBool(PcdEccLeakyBucketEnbale)) {
        DEBUG((EFI_D_ERROR, "CPU%d", ProcessorNumber));
        DEBUG((EFI_D_ERROR, " Local SMI Status = 0x%08x\n", pLocalSmiStatusList[ProcessorNumber].Value));
      }
    }
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
HygonSearchMcaError (
  IN OUT   RAS_MCA_ERROR_INFO *RasMcaErrorInfo
  )
{
  UINTN    Index;
  BOOLEAN  CpuMapFound = FALSE;

  if (RasMcaErrorInfo->CpuInfo.ProcessorNumber > mHygonRasPolicy->TotalNumberOfProcessors) {
    return EFI_INVALID_PARAMETER;
  }

  Index = 0;
  for (Index = 0; Index < mHygonRasPolicy->TotalNumberOfProcessors; Index++) {
    if (mHygonRasPolicy->RasCpuMap[Index].ProcessorNumber == RasMcaErrorInfo->CpuInfo.ProcessorNumber) {
      RasMcaErrorInfo->CpuInfo.SocketId = mHygonRasPolicy->RasCpuMap[Index].SocketId;
      RasMcaErrorInfo->CpuInfo.CcxId    = mHygonRasPolicy->RasCpuMap[Index].CcxId;
      RasMcaErrorInfo->CpuInfo.CddId    = mHygonRasPolicy->RasCpuMap[Index].CddId;
      RasMcaErrorInfo->CpuInfo.CoreId   = mHygonRasPolicy->RasCpuMap[Index].CoreId;
      RasMcaErrorInfo->CpuInfo.ThreadID = mHygonRasPolicy->RasCpuMap[Index].ThreadID;
      CpuMapFound = TRUE;
      break;
    }
  }

  if (!CpuMapFound) {
    return EFI_NOT_FOUND;
  }

  CollectMpMcaErrorInfo (RasMcaErrorInfo);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
HygonRasSmmExitType (
  IN       UINTN ProcessorNumber,
  IN       UINTN SmiExitType
  )
{
  RAS_MSR_ACCESS  RasMsrAccess;

  // Set SMI exit type
  RasMsrAccess.RegisterAddress = MSR_PFEH_CFG;
  RasMsrAccess.RegisterValue   = SmiExitType;
  RasMsrAccess.RegisterMask    = SMI_EXITTYPE_MASK;

  LibRasSmmMsrWrite (&RasMsrAccess, ProcessorNumber);
  return EFI_SUCCESS;
}

VOID
GetApSmmSaveStateBase (
  UINT64 *ApSaveStateBase
  )
{
  *ApSaveStateBase = AsmReadMsr64 (MSR_SMM_BASE)+ SMM_SAVE_STATE_OFFSET;
}

EFI_STATUS
EFIAPI
HygonGetSmmSaveStateBase (
  IN       UINTN ProcessorNumber,
  OUT      UINT64 *SmmSaveStateBase
  )
{
  if (ProcessorNumber == 0) {
    *SmmSaveStateBase = (AsmReadMsr64 (MSR_SMM_BASE) + SMM_SAVE_STATE_OFFSET);
    return EFI_SUCCESS;
  }

  gSmst->SmmStartupThisAp (
           GetApSmmSaveStateBase,
           ProcessorNumber,
           SmmSaveStateBase
           );
  return EFI_SUCCESS;
}

VOID
McaCloakControl (
  IN       RAS_MSR_ACCESS *RasMsrAccess
  )
{
  // MSR_PFEH_CLOAK_CFG data = (((MSR_PFEH_CLOAK_CFG data) & ~(UnCloakValue)) | CloakValue)
  AsmMsrAndThenOr64 (RasMsrAccess->RegisterAddress, ~(RasMsrAccess->RegisterMask), RasMsrAccess->RegisterValue);
  return;
}

EFI_STATUS
EFIAPI
HygonSetMcaCloakCfg (
  IN       UINTN ProcessorNumber,
  IN       UINT64 CloakValue,
  IN       UINT64 UnCloakValue
  )
{
  RAS_MSR_ACCESS  RasMsrAccess;

  // Cloak MC register (CloakValue)    : If BITx (i.e. Bank x) is set (= 1) then Bank x will be Cloaked
  // Uncloak MC register (UnCloakValue): If BITx (i.e. Bank x) is set (= 1) then Bank x will be UnCloaked
  // Note:
  // 01. CloakValue has a higher priority than UnCloakValue.
  // 02. Unspecified BITs (BITx = 0) in CloakValue and UnCloakValue paremeters will maintain its original value.
  RasMsrAccess.RegisterAddress = MSR_PFEH_CLOAK_CFG; // 0xC0010121
  RasMsrAccess.RegisterValue   = CloakValue;
  RasMsrAccess.RegisterMask    = UnCloakValue;

  LibRasSmmRunFunc (McaCloakControl, &RasMsrAccess, ProcessorNumber);

  return EFI_SUCCESS;
}

VOID
McaStatusClr (
  IN VOID  *Buffer
  )
{
  UINT8  *McaBankNumber;

  McaBankNumber = (UINT8 *)Buffer;
  // MSR_C001_0015[18][McStatusWrEn] = 1
  AsmMsrAndThenOr64 (MSR_HWCR, ~BIT18, BIT18);

  // Clear MCA_STATUS
  AsmMsrAndThenOr64 ((MCA_EXTENSION_BASE + (*McaBankNumber * 0x10) | MCA_STATUS_OFFSET), 0, 0);

  // MSR_C001_0015[18][McStatusWrEn] = 0
  AsmMsrAndThenOr64 (MSR_HWCR, ~BIT18, 0);
}

EFI_STATUS
EFIAPI
HygonClrMcaStatus (
  IN       UINTN            ProcessorNumber,
  IN       UINTN            McaBankNumber,
  IN       BOOLEAN          IsWrMsr
  )
{
  RAS_MSR_ACCESS  RasMsrAccess;
  UINT32          ECX_Data;
  UINT32          EAX_Data;
  UINT32          EDX_Data;
  UINT64          Msr_Data;
  UINT64          Hwcr_Data;
  UINT64          SmmSaveStateBase;
  SMM_SAVE_STATE  *SmmSaveState;
  BOOLEAN         IsMsrStatus;

  if (IsWrMsr) {
    // Check which register OS want to update ECX for Bank and Register, EAX:EDX for the Value
    HygonGetSmmSaveStateBase (ProcessorNumber, &SmmSaveStateBase);
    SmmSaveState = (SMM_SAVE_STATE *)SmmSaveStateBase;

    ECX_Data = (UINT32)(SmmSaveState->RCX & 0xFFFFFFFF);
    EAX_Data = (UINT32)(SmmSaveState->RAX & 0xFFFFFFFF);
    EDX_Data = (UINT32)(SmmSaveState->RDX & 0xFFFFFFFF);

    Msr_Data = (UINT64)EDX_Data;
    Msr_Data = (Msr_Data << 32) | EAX_Data;

    // Prepare MSR update table
    RasMsrAccess.RegisterAddress = MSR_HWCR;
    LibRasSmmMsrRead (&RasMsrAccess, ProcessorNumber);
    Hwcr_Data = RasMsrAccess.RegisterValue;

    IsMsrStatus = FALSE;
    // Check MCA_STATUS write
    if ((MCA_LEGACY_BASE <= ECX_Data) && (ECX_Data < MCA_LEGACY_TOP_ADDR)) {
      // Legacy MCA address
      if ((ECX_Data & MCA_REG_OFFSET_MASK) == MCA_STATUS_OFFSET) {
        IsMsrStatus = TRUE;
      }
    } else if (ECX_Data == LMCA_STATUS_REG) {
      IsMsrStatus = TRUE;
    } else {
      // Extension MCA Address
      if ((ECX_Data & SMCA_REG_OFFSET_MASK) == MCA_STATUS_OFFSET) {
        IsMsrStatus = TRUE;
      }
    }

    // BIOS should abort and leave MCA register uncloak if OS does not set
    // MSR_C001_0015[18][McStatusWrEn] = 1
    if (IsMsrStatus) {
      // HWCR[18] need set when write non 0 value to MCA_STATUS
      if ((Msr_Data != 0) && ((Hwcr_Data & BIT18) == 0)) {
        return EFI_ABORTED;
      }
    }

    RasMsrAccess.RegisterAddress = ECX_Data;
    RasMsrAccess.RegisterValue   = Msr_Data;
    RasMsrAccess.RegisterMask    = 0xFFFFFFFFFFFFFFFF;

    LibRasSmmMsrWrite (&RasMsrAccess, ProcessorNumber);

    return EFI_SUCCESS;
  } else {
    LibRasSmmRunFunc (McaStatusClr, &McaBankNumber, ProcessorNumber);
    return EFI_SUCCESS;
  }
}

EFI_STATUS
GetPostpackageRepairInfo (
  IN       HYGON_DIMM_INFO    *HygonDimmInfo,
  OUT      HYGON_PPR_INFO     *PprInfo
  )
{
  EFI_STATUS                               Status;
  HYGON_POST_PACKAGE_REPAIR_INFO_PROTOCOL  *PPRInterface;

  Status = gSmst->SmmLocateProtocol (&gHygonPostPackageRepairInfoProtocolGuid, NULL, &PPRInterface);
  if (EFI_ERROR (Status)) {
    return Status;    // Error detected
  }

  Status = PPRInterface->HygonGetPprInfo (PPRInterface, HygonDimmInfo, PprInfo);
  return Status;
}

/*********************************************************************************
 * Name: HygonMapSymbolToDramDevice
 *
 * Description
 *   Maps ECC Symbol to DRAM device based on DIMM device width, EccSymbolSize and
 *   EccBitInterleaving
 *
 * Arguments:
 *   RasMcaErrorInfo  : Structure containing error information
 *   NormalizedAddress : Structure containing DIMM location information
 *   BankIndex : Offset of data in McaBankErrorInfo struct
 *   DeviceStart : First device to repair
 *   DeviceEnd : Last device to repair
 *   DeviceWidth : DIMM Device Width from SPD
 *
 * Returns:
 *   EFI_STATUS
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
HygonMapSymbolToDramDevice (
  IN HYGON_RAS_SMM_PROTOCOL *This,
  IN RAS_MCA_ERROR_INFO   *RasMcaErrorInfo,
  IN NORMALIZED_ADDRESS   *NormalizedAddress,
  IN UINT8                BankIndex,
  OUT UINT32               *DeviceStart,
  OUT UINT32               *DeviceEnd,
  OUT UINT8                *DeviceWidth
  )
{
  EFI_STATUS       Status;
  UINT64           Symbol, Address;
  UMC_ECCCTRL_REG  RasEccCtrlReg;
  HYGON_DIMM_INFO  HygonDimmInfo;
  DIMM_INFO        DimmInfo;
  HYGON_PPR_INFO   PprInfo;
  UINT32           RasEccCtrlRegAddress;
  UINT32           BusNumber;
  UINT8            ChannelId;

  // Get device width from HygonPostPackageRepairInfoProtocol
  HygonMcaErrorAddrTranslate (NormalizedAddress, &Address, &DimmInfo);
  HygonDimmInfo.SocketId     = NormalizedAddress->normalizedSocketId;
  HygonDimmInfo.CddId        = NormalizedAddress->normalizedCddId;
  HygonDimmInfo.ChannelId    = NormalizedAddress->normalizedChannelId;
  HygonDimmInfo.SubChannelId = NormalizedAddress->normalizedSubChannelId;
  HygonDimmInfo.Chipselect   = DimmInfo.ChipSelect;
  Status = GetPostpackageRepairInfo (&HygonDimmInfo, &PprInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "HygonMapSymbolToDramDevice: Error calling GetPostpackageRepairInfo.\n"));
    return Status;    // Error detected
  }

  *DeviceWidth = (1<<(PprInfo.DeviceWidth & 0x7))*4;
  DEBUG ((DEBUG_VERBOSE, "HygonMapSymbolToDramDevice: Device Width: 0x%x\n", *DeviceWidth));

  // Check for multi-bit error, return all valid devices for a given DeviceType x4=0-17, x8=0-8
  DEBUG ((
    DEBUG_VERBOSE,
    "HygonMapSymbolToDramDevice: McaStatusMsr.Field.Deferred: 0x%x\n",
    RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Deferred
    ));
  DEBUG ((
    DEBUG_VERBOSE,
    "HygonMapSymbolToDramDevice: McaConfigMsr.Field.LogDeferredInMcaStat: 0x%x\n",
    RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaConfigMsr.Field.LogDeferredInMcaStat
    ));
  DEBUG ((
    DEBUG_VERBOSE,
    "HygonMapSymbolToDramDevice: McaDeStatMsr.Field.Val: 0x%x\n",
    RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaDeStatMsr.Field.Val
    ));
  if ((RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Deferred) ||
      ((RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaConfigMsr.Field.LogDeferredInMcaStat == 0) &&
       (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaDeStatMsr.Field.Val))) {
    *DeviceStart = 0;
    switch (*DeviceWidth) {
      case DEVICE_WIDTH_x4:
        *DeviceEnd = 17;
        break;
      case DEVICE_WIDTH_x8:
        *DeviceEnd = 8;
        break;
      default:
        *DeviceEnd = 0;
        DEBUG ((DEBUG_ERROR, "HygonMapSymbolToDramDevice: Invalid Device Width for multi-bit error.\n"));
        return EFI_INVALID_PARAMETER;
    }

    DEBUG ((
      DEBUG_VERBOSE,
      "HygonMapSymbolToDramDevice: Returning DeviceStart: 0x%x, "
      "DeviceEnd: 0x%x, DeviceWidth: 0x%x\n",
      *DeviceStart,
      *DeviceEnd,
      *DeviceWidth
      ));
    return EFI_SUCCESS;
  }

  // Read RAS ECC Control register to get EccSymbolSize and EccBitInterleaving values
  if (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaIpidMsr.Field.HardwareID == MCA_UMC_ID) {
    ChannelId = UmcMcaInstanceIdSearch (RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaIpidMsr) / 2;
    RasEccCtrlRegAddress = UMC_SPACE (HygonDimmInfo.CddId, ChannelId, UMC0_CH_REG_BASE | UMC_ECC_CTRL);
  } else {
    return EFI_UNSUPPORTED;
  }

  BusNumber = (UINT32)FabricTopologyGetHostBridgeBusBase (
                        NormalizedAddress->normalizedSocketId,
                        0,
                        0
                        );
  SmnRegisterRead (BusNumber, RasEccCtrlRegAddress, &RasEccCtrlReg.Value);

  DEBUG ((
    DEBUG_VERBOSE,
    "HygonMapSymbolToDramDevice: EccSymbolSize: 0x%x\n",
    RasEccCtrlReg.Field.EccSymbolSize
    ));
  DEBUG ((
    DEBUG_VERBOSE,
    "HygonMapSymbolToDramDevice: EccBitInterleaving: 0x%x\n",
    RasEccCtrlReg.Field.EccBitInterleaving
    ));

  // Get Symbol from ErrorInformation passed in.
  // For DramEccErr, Symbol = ErrorInformation[13:8]
  Symbol = RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaSyndMsr.Field.ErrorInformation;
  Symbol = ((Symbol >> 8) & 0x3F);
  // Check for valid symbol from HW then adjust at the end.
  DEBUG ((DEBUG_VERBOSE, "HygonMapSymbolToDramDevice: Raw Symbol: 0x%x\n", Symbol));
  if (Symbol == 0) {
    DEBUG ((DEBUG_ERROR, "HygonMapSymbolToDramDevice: Invalid Symbol: 0x%x\n", Symbol));
    return EFI_INVALID_PARAMETER;
  }

  // Adjust symbol after checking for valid case.
  Symbol--;

  if (*DeviceWidth == DEVICE_WIDTH_x4) {
    if (RasEccCtrlReg.Field.EccSymbolSize == ECC_SYMBOL_SIZE_x4) {
      if (RasEccCtrlReg.Field.EccBitInterleaving == ECC_BIT_INTERLEAVING_ENABLED) {
        *DeviceStart = (UINT32)Symbol / 2;
        *DeviceEnd   = *DeviceStart;
      } else if (RasEccCtrlReg.Field.EccBitInterleaving == ECC_BIT_INTERLEAVING_DISABLED) {
        if (Symbol < 0x20) {
          *DeviceStart = (UINT32)Symbol % 0x10;
          *DeviceEnd   = *DeviceStart;
        } else {
          *DeviceStart = (UINT32)(Symbol % 2) + 0x10;
          *DeviceEnd   = *DeviceStart;
        }
      } else {
        DEBUG ((
          DEBUG_ERROR,
          "HygonMapSymbolToDramDevice: Invalid Parameter Width: 0x%x, "
          "ECC Symbol Size: 0x%x, Bit Interleave: 0x%x\n",
          *DeviceWidth,
          RasEccCtrlReg.Field.EccSymbolSize,
          RasEccCtrlReg.Field.EccBitInterleaving
          ));
        return EFI_INVALID_PARAMETER;
      }
    } else if (RasEccCtrlReg.Field.EccSymbolSize == ECC_SYMBOL_SIZE_x8) {
      if (RasEccCtrlReg.Field.EccBitInterleaving == ECC_BIT_INTERLEAVING_ENABLED) {
        *DeviceStart = (UINT32)Symbol;
        *DeviceEnd   = *DeviceStart;
      } else if (RasEccCtrlReg.Field.EccBitInterleaving == ECC_BIT_INTERLEAVING_DISABLED) {
        if (Symbol < 0x10) {
          *DeviceStart = (UINT32)(Symbol*2) % 0x10;
          *DeviceEnd   = (UINT32)((Symbol*2) + 1) % 0x10;
        } else {
          *DeviceStart = (UINT32)0x10;
          *DeviceEnd   = (UINT32)0x11;
        }
      } else {
        DEBUG ((
          DEBUG_ERROR,
          "HygonMapSymbolToDramDevice: Invalid Parameter Width: 0x%x, "
          "ECC Symbol Size = 0x%x, Bit Interleave: 0x%x\n",
          *DeviceWidth,
          RasEccCtrlReg.Field.EccSymbolSize,
          RasEccCtrlReg.Field.EccBitInterleaving
          ));
        return EFI_INVALID_PARAMETER;
      }
    } else {
      DEBUG ((
        DEBUG_ERROR,
        "HygonMapSymbolToDramDevice: Invalid Parameter Width: 0x%x, "
        "ECC Symbol Size: 0x%x, Bit Interleave: 0x%x\n",
        *DeviceWidth,
        RasEccCtrlReg.Field.EccSymbolSize,
        RasEccCtrlReg.Field.EccBitInterleaving
        ));
      return EFI_INVALID_PARAMETER;
    }
  } else if (*DeviceWidth == DEVICE_WIDTH_x8) {
    if (RasEccCtrlReg.Field.EccSymbolSize == ECC_SYMBOL_SIZE_x4) {
      if (RasEccCtrlReg.Field.EccBitInterleaving == ECC_BIT_INTERLEAVING_ENABLED) {
        *DeviceStart = (UINT32)Symbol / 4;
        *DeviceEnd   = *DeviceStart;
      } else if (RasEccCtrlReg.Field.EccBitInterleaving == ECC_BIT_INTERLEAVING_DISABLED) {
        if (Symbol < 0x20) {
          *DeviceStart = (UINT32)(Symbol / 2) % 0x8;
          *DeviceEnd   = *DeviceStart;
        } else {
          *DeviceStart = 0x8;
          *DeviceEnd   = *DeviceStart;
        }
      } else {
        DEBUG ((
          DEBUG_ERROR,
          "HygonMapSymbolToDramDevice: Invalid Parameter Width: 0x%x, "
          "ECC Symbol Size: 0x%x, Bit Interleave: 0x%x\n",
          *DeviceWidth,
          RasEccCtrlReg.Field.EccSymbolSize,
          RasEccCtrlReg.Field.EccBitInterleaving
          ));
        return EFI_INVALID_PARAMETER;
      }
    } else if (RasEccCtrlReg.Field.EccSymbolSize == ECC_SYMBOL_SIZE_x8) {
      if (RasEccCtrlReg.Field.EccBitInterleaving == ECC_BIT_INTERLEAVING_ENABLED) {
        *DeviceStart = (UINT32)Symbol / 2;
        *DeviceEnd   = *DeviceStart;
      } else if (RasEccCtrlReg.Field.EccBitInterleaving == ECC_BIT_INTERLEAVING_DISABLED) {
        if (Symbol < 0x10) {
          *DeviceStart = (UINT32)Symbol % 8;
          *DeviceEnd   = *DeviceStart;
        } else {
          *DeviceStart = 0x8;
          *DeviceEnd   = *DeviceStart;
        }
      } else {
        DEBUG ((
          DEBUG_ERROR,
          "HygonMapSymbolToDramDevice: Invalid Parameter Width: 0x%x, "
          "ECC Symbol Size: 0x%x, Bit Interleave: 0x%x\n",
          *DeviceWidth,
          RasEccCtrlReg.Field.EccSymbolSize,
          RasEccCtrlReg.Field.EccBitInterleaving
          ));
        return EFI_INVALID_PARAMETER;
      }
    } else {
      DEBUG ((
        DEBUG_ERROR,
        "HygonMapSymbolToDramDevice: Invalid Parameter Width: 0x%x, "
        "ECC Symbol Size: 0x%x, Bit Interleave: 0x%x\n",
        *DeviceWidth,
        RasEccCtrlReg.Field.EccSymbolSize,
        RasEccCtrlReg.Field.EccBitInterleaving
        ));
      return EFI_INVALID_PARAMETER;
    }
  } else {
    DEBUG ((
      DEBUG_ERROR,
      "HygonMapSymbolToDramDevice: Invalid Parameter Width: 0x%x, "
      "ECC Symbol Size: 0x%x, Bit Interleave: 0x%x\n",
      *DeviceWidth,
      RasEccCtrlReg.Field.EccSymbolSize,
      RasEccCtrlReg.Field.EccBitInterleaving
      ));
    return EFI_INVALID_PARAMETER;
  }

  // Check data returned to ensure that it is valid
  if ((*DeviceWidth == DEVICE_WIDTH_x4) && (*DeviceEnd > 0x11) ||
      (*DeviceWidth == DEVICE_WIDTH_x8) && (*DeviceEnd > 0x8)) {
    DEBUG ((
      DEBUG_ERROR,
      "HygonMapSymbolToDramDevice: Invalid End Device generated: 0x%x",
      *DeviceEnd
      ));
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((
    DEBUG_VERBOSE,
    "HygonMapSymbolToDramDevice: Returning DeviceStart: 0x%x, "
    "DeviceEnd: 0x%x, DeviceWidth: 0x%x\n",
    *DeviceStart,
    *DeviceEnd,
    *DeviceWidth
    ));
  return EFI_SUCCESS;
}
