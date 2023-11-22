/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Filecode.h>
#include <Library/DebugLib.h>
#include <Library/RasSmmLib.h>
#include <Library/PciLib.h>
#include "HYGON.h"
#include <Library/IdsLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <FabricRegistersST.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define CFG_ADDR_PORT     0xcf8
#define CFG_DATA_PORT     0xcfc
#define FCH_IOMAP_REGCD6  0xcd6
#define FCH_IOMAP_REGCD7  0xcd7

#define FILECODE  UNIVERSAL_VERISION_HYGONVERSIONDXE_HYGONVERSIONDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

extern  ADDR_DATA  *gAddrData;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
RasSmmMsrWrite (
  IN       RAS_MSR_ACCESS *RasMsrAccess
  );

VOID
RasSmmMsrRead (
  IN OUT   RAS_MSR_ACCESS *RasMsrAccess
  );

VOID
RasSmmMsrTblWrite (
  IN       VOID *MsrTable
  );

VOID
SetMcaThreshold (
  IN       RAS_THRESHOLD_CONFIG *RasThresholdConfig
  );

VOID
CollectMcaErrorInfo (
  IN OUT   RAS_MCA_ERROR_INFO *RasMcaErrorInfo
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

/*----------------------------------------------------------------------------------------*/

/**
 *  LibRasSmmMemRead
 *  Description
 *      SMM Memory Read Access
 *
 * @param[in]       SmmIo           Pointer to SMM CPU IO interface
 * @param[in]       AccessWidth     Access width
 * @param[in]       Address         Memory address
 * @param[out]      Value           Pointer to data buffer
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibRasSmmMemRead (
  IN       EFI_SMM_CPU_IO2_PROTOCOL    *SmmIo,
  IN       EFI_SMM_IO_WIDTH            AccessWidth,
  IN       UINT64                      Address,
     OUT   VOID                        *Value
  )
{
  SmmIo->Mem.Read (
               SmmIo,
               AccessWidth,
               Address,
               1,
               Value
               );
}

/*----------------------------------------------------------------------------------------*/

/**
 *  LibRasSmmIoRead
 *  Description
 *      SMM I/O Read Access
 *
 * @param[in]       SmmIo           Pointer to SMM CPU IO interface
 * @param[in]       AccessWidth     Access width
 * @param[in]       Address         IO address
 * @param[out]      Value           Pointer to data buffer
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibRasSmmIoRead (
  IN       EFI_SMM_CPU_IO2_PROTOCOL    *SmmIo,
  IN       EFI_SMM_IO_WIDTH            AccessWidth,
  IN       UINT16                      Address,
     OUT   VOID                        *Value
  )
{
  SmmIo->Io.Read (
              SmmIo,
              AccessWidth,
              Address,
              1,
              Value
              );
}

/*----------------------------------------------------------------------------------------*/

/**
 *  LibRasSmmIoWrite
 *  Description
 *      SMM I/O Write Access
 *
 * @param[in]       SmmIo           Pointer to SMM CPU IO interface
 * @param[in]       AccessWidth     Access width
 * @param[in]       Address         IO address
 * @param[out]      Value           Pointer to data buffer
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibRasSmmIoWrite (
  IN       EFI_SMM_CPU_IO2_PROTOCOL    *SmmIo,
  IN       EFI_SMM_IO_WIDTH            AccessWidth,
  IN       UINT64                      Address,
  IN       VOID                        *Value
  )
{
  SmmIo->Io.Write (
              SmmIo,
              AccessWidth,
              Address,
              1,
              Value
              );
}

VOID
LibRasSmmPciRead (
  IN       EFI_SMM_CPU_IO2_PROTOCOL    *SmmIo,
  IN       EFI_SMM_IO_WIDTH            AccessWidth,
  IN       UINT32                      PciAddress,
     OUT   VOID                        *Value
  )
{
  UINT32  Address32;

  Address32 = BIT31 + (UINT32)((PciAddress >> 8) & 0xFFFFFF00) + (PciAddress & 0xFF);
  LibRasSmmIoWrite (SmmIo, SMM_IO_UINT32, CFG_ADDR_PORT, &Address32);
  LibRasSmmIoRead (SmmIo, AccessWidth, CFG_DATA_PORT, Value);
}

VOID
LibRasSmmPmioRead8 (
  IN       EFI_SMM_CPU_IO2_PROTOCOL    *SmmIo,
  IN       UINT8                       Offset,
     OUT   VOID                        *Value
  )
{
  LibRasSmmIoWrite (SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGCD6, &Offset);
  LibRasSmmIoRead (SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGCD7, Value);
}

VOID
LibRasSmmPmioWrite8 (
  IN       EFI_SMM_CPU_IO2_PROTOCOL    *SmmIo,
  IN       UINT8                       Offset,
     OUT   VOID                        *Value
  )
{
  LibRasSmmIoWrite (SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGCD6, &Offset);
  LibRasSmmIoWrite (SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGCD7, Value);
}

/*---------------------------------------------------------------------------------------*/

/**
 * LibRasSmmMsrWrite
 *
 * Sync Givan MSR list to all APs
 *
 * @param[in]  ApMsrSync         MSRs table
 *
 */
VOID
LibRasSmmMsrWrite (
  IN       RAS_MSR_ACCESS *RasMsrAccess,
  IN       UINTN          ProcessorNumber
  )
{
  if (ProcessorNumber > gSmst->NumberOfCpus) {
    return;
  }

  if (ProcessorNumber == 0) {
    RasSmmMsrWrite (RasMsrAccess);
  } else {
    gSmst->SmmStartupThisAp (
             RasSmmMsrWrite,
             ProcessorNumber,
             RasMsrAccess
             );
  }
}

/*---------------------------------------------------------------------------------------*/

/**
 * LibRasSmmMsrRead
 *
 * Sync Givan MSR list to all APs
 *
 * @param[in]  ApMsrSync         MSRs table
 *
 */
VOID
LibRasSmmMsrRead (
  IN       RAS_MSR_ACCESS *RasMsrAccess,
  IN       UINTN          ProcessorNumber
  )
{
  if (ProcessorNumber > gSmst->NumberOfCpus) {
    return;
  }

  if (ProcessorNumber == 0) {
    RasSmmMsrRead (RasMsrAccess);
  } else {
    gSmst->SmmStartupThisAp (
             RasSmmMsrRead,
             ProcessorNumber,
             RasMsrAccess
             );
  }
}

/*---------------------------------------------------------------------------------------*/

/**
 * LibRasSmmMsrTblWrite
 *
 * Write Givan MSR list to all specified core.
 *
 * @param[in]  ApMsrSync         MSRs table
 * @param[in]  ProcessorNumber          Cpu Logic ID
 *
 */
VOID
LibRasSmmMsrTblWrite (
  IN OUT   RAS_BSP_AP_MSR_SYNC *ApMsrSync,
  IN       UINTN               ProcessorNumber
  )
{
  if (ProcessorNumber > gSmst->NumberOfCpus) {
    return;
  }

  if (ProcessorNumber == 0) {
    RasSmmMsrTblWrite (ApMsrSync);
  } else {
    gSmst->SmmStartupThisAp (
             RasSmmMsrTblWrite,
             ProcessorNumber,
             ApMsrSync
             );
  }
}

/*---------------------------------------------------------------------------------------*/

/**
 * LibRasSmmSyncMsr
 *
 * Sync Givan MSR list to all APs
 *
 * @param[in]  ApMsrSync         MSRs table
 *
 */
VOID
LibRasSmmSyncMsr (
  IN OUT   RAS_BSP_AP_MSR_SYNC *ApMsrSync
  )
{
  UINT16  i;

  //
  // Sync all MSR settings with BSP
  //

  for (i = 0; ApMsrSync[i].RegisterAddress != 0; i++) {
    if (ApMsrSync[i].ForceSetting == FALSE) {
      ApMsrSync[i].RegisterValue = AsmReadMsr64 (ApMsrSync[i].RegisterAddress);
    }
  }

  for (i = 1; i < gSmst->NumberOfCpus; i++) {
    gSmst->SmmStartupThisAp (
             RasSmmMsrTblWrite,
             i,
             (VOID *)ApMsrSync
             );
  }
}

/*---------------------------------------------------------------------------------------*/

/**
 * LibRasSmmRunFunc
 *
 * execute given MP procedure to BSP/AP
 *
 * @param[in]  RasMpProcedure Ras MP procedure
 * @param[in]  Buffer         Procedure input buffer
 * @param[in]  ProcessorNumber       Logic CPU ID
 *
 */
VOID
LibRasSmmRunFunc (
  IN        EFI_AP_PROCEDURE RasMpProcedure,
  IN        VOID             *Buffer,
  IN        UINTN            ProcessorNumber
  )
{
  if (ProcessorNumber > gSmst->NumberOfCpus) {
    return;
  }

  if (ProcessorNumber == 0) {
    RasMpProcedure (Buffer);
  } else {
    gSmst->SmmStartupThisAp (
             RasMpProcedure,
             ProcessorNumber,
             Buffer
             );
  }
}

VOID
SetMpMcaThreshold (
  IN       RAS_THRESHOLD_CONFIG *RasThresholdConfig
  )
{
  UINT16  i;

  // Program BSP first
  SetMcaThreshold (RasThresholdConfig);

  // Program AP
  for (i = 1; i < gSmst->NumberOfCpus; i++) {
    gSmst->SmmStartupThisAp (
             SetMcaThreshold,
             i,
             (VOID *)RasThresholdConfig
             );
  }
}

VOID
CollectMpMcaErrorInfo (
  IN OUT   RAS_MCA_ERROR_INFO *RasMcaErrorInfo
  )
{
  if (RasMcaErrorInfo->CpuInfo.ProcessorNumber > gSmst->NumberOfCpus) {
    return;
  }

  // Program BSP first
  if (RasMcaErrorInfo->CpuInfo.ProcessorNumber == 0) {
    CollectMcaErrorInfo (RasMcaErrorInfo);
  } else {
    gSmst->SmmStartupThisAp (
             CollectMcaErrorInfo,
             RasMcaErrorInfo->CpuInfo.ProcessorNumber,
             (VOID *)RasMcaErrorInfo
             );
  }
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/

/**
 * MSRs Write
 *
 *
 * @param[in]  RasMsrAccess          MSR data write
 *
 */
VOID
RasSmmMsrWrite (
  IN       RAS_MSR_ACCESS *RasMsrAccess
  )
{
  AsmMsrAndThenOr64 (
    RasMsrAccess->RegisterAddress,
    ~RasMsrAccess->RegisterMask,
    (RasMsrAccess->RegisterValue & RasMsrAccess->RegisterMask)
    );
}

/*---------------------------------------------------------------------------------------*/

/**
 * MSRs Read
 *
 *
 * @param[in]  RasMsrAccess          MSR data read
 *
 */
VOID
RasSmmMsrRead (
  IN       RAS_MSR_ACCESS *RasMsrAccess
  )
{
  RasMsrAccess->RegisterValue = AsmReadMsr64 (RasMsrAccess->RegisterAddress);
}

/*---------------------------------------------------------------------------------------*/

/**
 * AP task to sync MSRs with the BSC
 *
 *
 * @param[in]  MsrTable          MSRs table
 *
 */
VOID
RasSmmMsrTblWrite (
  IN       VOID *MsrTable
  )
{
  UINT8  i;

  for (i = 0; ((RAS_BSP_AP_MSR_SYNC *)MsrTable)[i].RegisterAddress != 0; i++) {
    AsmMsrAndThenOr64 (
      ((RAS_BSP_AP_MSR_SYNC *)MsrTable)[i].RegisterAddress,
      ~(((RAS_BSP_AP_MSR_SYNC *)MsrTable)[i].RegisterMask),
      (((RAS_BSP_AP_MSR_SYNC *)MsrTable)[i].RegisterValue &
       ((RAS_BSP_AP_MSR_SYNC *)MsrTable)[i].RegisterMask)
      );
  }
}

// byo231109 - >>
VOID
SetMcaThreshold (
  IN       RAS_THRESHOLD_CONFIG *RasThresholdConfig
  )
{
  UINT8         BankNum;
  UINT64        McMisc0;
  UINT64        NonMemMcMisc0;
  UINT64        Data64;
  UINT64        MsrData;
  UINT32        i;
  UINT64        SaveHwcr;
  MCA_IPID_MSR  McaIpid;
  BOOLEAN       IsUmc;
  BOOLEAN       NonMemThdCtrl;

  MsrData = AsmReadMsr64 (MSR_MCG_CAP);            // MCG_CAP
  BankNum = (UINT8)(MsrData & 0xFF);

  McMisc0 = RasThresholdConfig->ThresholdIntType;
  McMisc0 = (UINT64)(((McMisc0 << 17 | ((RasThresholdConfig->ThresholdCount) & 0x0FFF)) << 32) | BIT51);     // byo231101 -

  NonMemMcMisc0 = RasThresholdConfig->ThresholdIntType;
  NonMemMcMisc0 = (UINT64)(((NonMemMcMisc0 << 17 | ((RasThresholdConfig->NonMemThreshCount) & 0x0FFF)) << 32) | BIT51);
  NonMemThdCtrl = (RasThresholdConfig->NonMemThreshCount < 0xFFF);

  if (RasThresholdConfig->ThresholdControl || NonMemThdCtrl) {
    // MSR_C001_0015[18][McStatusWrEn] = 1
    SaveHwcr = AsmReadMsr64 (MSR_HWCR);
    MsrData  = SaveHwcr | BIT18;
    AsmWriteMsr64 (MSR_HWCR, MsrData);

    for (i = 0; i < BankNum; i++) {

      McaIpid.Value = AsmReadMsr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_IPID_OFFSET));
      if (McaIpid.Field.HardwareID == MCA_UMC_ID) {
        if(!RasThresholdConfig->ThresholdControl){
          continue;
        }
        IsUmc  = TRUE;
        Data64 = McMisc0;
      } else {
        if(!NonMemThdCtrl){
          continue;
        }      
        IsUmc = FALSE;
        Data64 = NonMemMcMisc0;
      }
      
      AsmMsrAndThenOr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_MISC0_OFFSET), 0xFFF1F000FFFFFFFF, Data64);
      if (IsUmc) {
        AsmMsrAndThenOr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_MISC1_OFFSET), 0xFFF1F000FFFFFFFF, Data64);
      }
    }

    // Restore MSR_C001_0015[18][McStatusWrEn]
    AsmWriteMsr64 (MSR_HWCR, SaveHwcr);
  }
}
// byo231109 - <<

VOID
CollectMcaErrorInfo (
  IN OUT   RAS_MCA_ERROR_INFO *RasMcaErrorInfo
  )
{
  UINT32          i;
  UINT8           BankNum;
  UINT64          MsrData;
  MCA_STATUS_MSR  McaStatusMsr;
  MCA_IPID_MSR    McaIpid;

  MsrData = AsmReadMsr64 (MSR_MCG_CAP);            // MCG_CAP
  BankNum = (UINT8)(MsrData & 0xFF);

  for (i = 0; i < BankNum; i++) {
    McaIpid.Value = AsmReadMsr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_IPID_OFFSET));
    if (McaIpid.Field.HardwareID == 0) {
      continue;
    }

    // Find error log
    McaStatusMsr.Value = AsmReadMsr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_STATUS_OFFSET));
    if (McaStatusMsr.Field.Val) {
      // Collect MSR value
      RasMcaErrorInfo->McaBankErrorInfo[i].McaBankNumber = i;
      RasMcaErrorInfo->McaBankErrorInfo[i].McaStatusMsr.Value = McaStatusMsr.Value;
      RasMcaErrorInfo->McaBankErrorInfo[i].McaAddrMsr.Value   = AsmReadMsr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_ADDR_OFFSET));
      RasMcaErrorInfo->McaBankErrorInfo[i].McaConfigMsr.Value = AsmReadMsr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_CONFIG_OFFSET));
      RasMcaErrorInfo->McaBankErrorInfo[i].McaIpidMsr.Value   = AsmReadMsr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_IPID_OFFSET));
      RasMcaErrorInfo->McaBankErrorInfo[i].McaSyndMsr.Value   = AsmReadMsr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_SYND_OFFSET));
      RasMcaErrorInfo->McaBankErrorInfo[i].McaMisc0Msr.Value  = AsmReadMsr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_MISC0_OFFSET));
      if (McaIpid.Field.HardwareID == MCA_UMC_ID) {
        RasMcaErrorInfo->McaBankErrorInfo[i].McaMisc1Msr.Value = AsmReadMsr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_MISC1_OFFSET));
      }

      if (RasMcaErrorInfo->McaBankErrorInfo[i].McaStatusMsr.Field.Deferred) {
        RasMcaErrorInfo->McaBankErrorInfo[i].McaDeStatMsr.Value = AsmReadMsr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_DESTAT_OFFSET));
        RasMcaErrorInfo->McaBankErrorInfo[i].McaDeAddrMsr.Value = AsmReadMsr64 ((MCA_EXTENSION_BASE + (i * 0x10) | MCA_DEADDR_OFFSET));
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 *
 *
 */
UINT64
pow_ras (
  UINTN   input,
  UINTN   exp
  )
{
  UINT64  value;
  UINTN   i;

  value = 1;

  if (0 == exp) {
  } else {
    for (i = 0; i < exp; i++) {
      value *= input;
    }
  }

  return value;
}

/*------------------------------------------------------------------
 Function: getBits64
 Purpose: A helper function to get abit range from a uint64
 Inputs:
   low bit number
   high bit number
   the data
 Outputs:
   the requested bits, right justified
 *------------------------------------------------------------------*/
UINT64
getBits64 (
  UINT32 lowBit,
  UINT32 highBit,
  UINT64 data
  )
{
  UINT64  mask;

  mask = (((UINT64)1) << (highBit - lowBit + ((UINT64)1))) - ((UINT64)1);
  return (UINT32)((data >> lowBit) & mask);
}

/*------------------------------------------------------------------
 Function: getBits
 Purpose: A helper function to get abit range from a uint32
 Inputs:
   low bit number
   high bit number
   the data
 Outputs:
   the requested bits, right justified
 *------------------------------------------------------------------*/
UINT32
getBits (
  UINT32 lowBit,
  UINT32 highBit,
  UINT32 data
  )
{
  UINT32  mask;

  mask = (1 << (highBit - lowBit + 1)) - 1;
  return ((data >> lowBit) & mask);
}

/*------------------------------------------------------------------
 Function: getBit
 Purpose: A helper function to get a specific bit from a uint32
 Inputs:
   bit number
   the data
 Outputs:
   the requested bit, right justified
 *------------------------------------------------------------------*/
UINT32
getBit (
  UINT32 bit,
  UINT32 data
  )
{
  return ((data >> bit) & 0x1);
}

/*------------------------------------------------------------------
 Function: getCddDfRegSystemFabricIdMask
 Purpose: Get the DF::SystemFabricIdMask register
 Inputs: 
   An socketId,
   a  cddId
 Outputs: The requested register
 *------------------------------------------------------------------*/
UINT32
getCddDfRegSystemFabricIdMask (
  UINT32 socketId,
  UINT32 cddId
  )
{
  UINT32  Function;
  UINT32  Register;

  Function = DF_SYSFABIDMASK_ADDR >> 10;
  Register = DF_SYSFABIDMASK_ADDR & 0x3FF;
  return CddFabricRegisterAccRead (socketId, cddId, (UINTN)Function, (UINTN)Register, FABRIC_REG_ACC_BC);
}

/*------------------------------------------------------------------
 Function: getCddDfRegFabricBlkInstanceCnt
 Purpose: Get the DF::FabricBlockInstanceCount register
 Inputs:  A socketId and a cddId
 Outputs: The requested register
 *------------------------------------------------------------------*/
UINT32
getCddDfRegFabricBlkInstanceCnt (
  UINT32  socketId,
  UINT32  cddId
  )
{
  UINT32  Function;
  UINT32  Register;

  Function = DF_FABBLKINSTCNT_ADDR >> 10;
  Register = DF_FABBLKINSTCNT_ADDR & 0x3FF;
  return CddFabricRegisterAccRead (socketId, cddId, (UINTN)Function, (UINTN)Register, FABRIC_REG_ACC_BC);
}

/*------------------------------------------------------------------
 Function: getCddDfRegFabricBlkInstInfo3
 Purpose: Get the DF::FabricBlockInstanceInformation3 register
 Inputs: An instanceID (CS), a socketId and a cddId
 Outputs: The requested register
 *------------------------------------------------------------------*/
UINT32
getCddDfRegFabricBlkInstInfo3 (
  UINT32 socketId,
  UINT32 cddId,
  UINT32 instanceId
  )
{
  UINT32  Function;
  UINT32  Register;

  Function = DF_FABBLKINFO3_ADDR >> 10;
  Register = DF_FABBLKINFO3_ADDR & 0x3FF;
  return CddFabricRegisterAccRead (socketId, cddId, (UINTN)Function, (UINTN)Register, (UINTN)instanceId);
}

/*------------------------------------------------------------------
 Function: getDfRegDramOffset
 Purpose: Get the DF::DramOffset[n] register
 Inputs:
   An socketId,
   a  cddId,
   a  register number,
   a  instanceID (CS)
 Outputs: The requested register
 *------------------------------------------------------------------*/
UINT32
getDfRegDramOffset (
  UINT32 socketId,
  UINT32 cddId,
  UINT32 regNum,
  UINT32 instanceId
  )
{
  UINT32  Function;
  UINT32  Register;

  Function = DF_DRAMOFFSET1_ADDR >> 10;
  Register = DF_DRAMOFFSET1_ADDR & 0x3FF + (4*(regNum-1));
  return CddFabricRegisterAccRead (socketId, cddId, (UINTN)Function, (UINTN)Register, (UINTN)instanceId);
}

/*------------------------------------------------------------------
 Function: getDfRegDramBase
 Purpose: Get the DF::DramBaseAddress register
 Inputs:
   An socketId,
   a  cddId,
   a  register number (0 through 31),
   a  instanceID (CS) 
 Outputs: The requested register
 *------------------------------------------------------------------*/
UINT32
getDfRegDramBase (
  UINT32 socketId,
  UINT32 cddId,
  UINT32 regNum,
  UINT32 instanceId
  )
{
  UINT32  Function;
  UINT32  Register;

  Function = DF_DRAMBASE0_ADDR >> 10;
  Register = DF_DRAMBASE0_ADDR & 0x3FF + (8*regNum);
  return CddFabricRegisterAccRead (socketId, cddId, (UINTN)Function, (UINTN)Register, (UINTN)instanceId);
}

/*------------------------------------------------------------------
 Function: getDfRegDramLimit
 Purpose: Get the DF::DramLimitAddress register
 Inputs:
   An socketId,
   a  cddId,
   a  register number (0 through 15),
   a  instanceID (CS)
 Outputs: The requested register
 *------------------------------------------------------------------*/
UINT32
getDfRegDramLimit (
  UINT32 socketId,
  UINT32 cddId,
  UINT32 regNum,
  UINT32 instanceId
  )
{
  UINT32  Function;
  UINT32  Register;

  Function = DF_DRAMLIMIT0_ADDR >> 10;
  Register = DF_DRAMLIMIT0_ADDR & 0x3FF + (8*regNum);
  return CddFabricRegisterAccRead (socketId, cddId, (UINTN)Function, (UINTN)Register, (UINTN)instanceId);
}

/*------------------------------------------------------------------
 Function: getDfRegDramHoleCtrl
 Purpose: Get the DF::DramHoleControl register
 Inputs:
   An socketId,
   a  cddId,
   a  instanceID (CS)
 Outputs: The requested register
 *------------------------------------------------------------------*/
UINT32
getDfRegDramHoleCtrl (
  UINT32 socketId,
  UINT32 cddId,
  UINT32 instanceId
  )
{
  UINT32  Function;
  UINT32  Register;

  Function = DF_DRAMHOLECTRL_ADDR >> 10;
  Register = DF_DRAMHOLECTRL_ADDR & 0x3FF;
  return CddFabricRegisterAccRead (socketId, cddId, (UINTN)Function, (UINTN)Register, (UINTN)instanceId);
}

/*----------------------------------------------------------------------------------------*/

/**
 * Convert socket/cdd/UMC number to the Nth UMC in the system (the same as Nth channel in the system,
 * since one channel per UMC on Dharma)
 * The input parameter umc_chan_num is fixed to 0 on Dharma
 *
 * @param[in] pkg_no            Socket ID (0..1)
 * @param[in] mpu_no            Cdd ID (0..3)
 * @param[in] umc_inst_num      UMC ID (0..1)
 * @param[in] umc_chan_num      always = 0 in Dharma
 * @retval                      Nth channel in the system
 *----------------------------------------------------------------------------------------*/
UINTN
convert_to_addr_trans_index (
  UINTN  pkg_no,
  UINTN  mpu_no,
  UINTN  umc_inst_num,
  UINTN  umc_chan_num
  )
{
  UINTN  U_CH;
  UINTN  M_U_CH;

  U_CH   = ((CHANNEL_PER_UMC * umc_inst_num) + umc_chan_num);
  M_U_CH = ((CHANNEL_PER_UMC * UMC_PER_CDD) * mpu_no) + U_CH;

  return (((CHANNEL_PER_UMC * UMC_PER_CDD * CDD_PER_SOCKET) * pkg_no) + M_U_CH);
}

BOOLEAN
internal_bit_wise_xor (
  UINT32  inp
  )
{
  BOOLEAN  t;
  UINT32   i;

  t = 0;
  for (i = 0; i < 32; i++) {
    t = t ^ ((inp >> i) & 0x1);
  }

  return t;
}

/*----------------------------------------------------------------------------------------*/

/**
 *
 *----------------------------------------------------------------------------------------*/
VOID
NormalizedToBankAddrMap (
  UINT64  ChannelAddr,
  UINT32 CSMask,
  UINT8 *Bank,
  UINT32 *Row,
  UINT16 *Col,
  UINT8 *Rankmul,
  UINT8 numbankbits,
  UINT8 bank4,
  UINT8 bank3,
  UINT8 bank2,
  UINT8 bank1,
  UINT8 bank0,
  UINT8 numrowlobits,
  UINT8 numrowhibits,
  UINT8 numcolbits,
  UINT8 row_lo0,
  UINT8 row_hi0,
  UINT32 COL0REG,
  UINT32 COL1REG,
  UINT8 numcsbits,
  UINT8 rm0,
  UINT8 rm1,
  UINT8 rm2,
  UINT8 chan,
  UINT8 vcm_en,
  UINT8 numbgbits,
  UINT8 bankgroupen,
  UINT8 invertmsbse,
  UINT8 invertmsbso,
  UINT8 rm0sec,
  UINT8 rm1sec,
  UINT8 rm2sec,
  UINT8 chansec,
  UINT8 invertmsbsesec,
  UINT8 invertmsbsosec,
  UINT64 CSMasksec,
  UINT8 SEC,
  UINT8 cs,
  UINT32 addrhashbank0,
  UINT32 addrhashbank1,
  UINT32 addrhashbank2,
  UINT32 addrhashbank3,
  UINT32 addrhashbank4,
  UINT32 addrhashbankpc,
  UINT32 addrhashbankpc2,
  UINT32 addrhashnormaddr0,
  UINT32 addrhashnormaddr1
  )
{
  if(SEC == 3) {
    // IDS_HDT_CONSOLE (MAIN_FLOW, "ERROR: SEC value cannot be 3\n");
    DEBUG ((EFI_D_ERROR, "ERROR: SEC value cannot be 3\n"));

    ASSERT (FALSE);
  }

  // if addrhash is enabled, we can get bank after gettting row and col
  // if(((addrhashbank0 & 0x1) == 0) && ((addrhashbank1 & 0x1) == 0) && ((addrhashbank2 & 0x1) == 0) &&
  // ((addrhashbank3 & 0x1) == 0) && ((addrhashbank4 & 0x1) == 0)) {
  // if (numbankbits == 3) {
  // *Bank = ((ChannelAddr >> (bank0 + 5)) & 1) | (((ChannelAddr >> (bank1 + 5)) & 1) << 1) |
  // (((ChannelAddr >> (bank2 + 5)) & 1) << 2);
  // } else if (numbankbits == 4) {
  // *Bank = ((ChannelAddr >> (bank0 + 5)) & 1) | (((ChannelAddr >> (bank1 + 5)) & 1) << 1) |
  // (((ChannelAddr >> (bank2 + 5)) & 1) << 2) | (((ChannelAddr >> (bank3 + 5)) & 1) << 3);
  // } else {
  // *Bank = ((ChannelAddr >> (bank0 + 5)) & 1) | (((ChannelAddr >> (bank1 + 5)) & 1) << 1) |
  // (((ChannelAddr >> (bank2 + 5)) & 1) << 2) | (((ChannelAddr >> (bank3 + 5)) & 1) << 3) |
  // (((ChannelAddr >> (bank4 + 5)) & 1) << 4);
  // }
  // }   // if addr hash is disabled
  if (numbankbits == 3) {
    *Bank = ((ChannelAddr >> (bank0 + 6)) & 1) | (((ChannelAddr >> (bank1 + 6)) & 1) << 1) |
      (((ChannelAddr >> (bank2 + 6)) & 1) << 2);

  } else if (numbankbits == 4) {
    *Bank = ((ChannelAddr >> (bank0 + 6)) & 1) | (((ChannelAddr >> (bank1 + 6)) & 1) << 1) |
      (((ChannelAddr >> (bank2 + 6)) & 1) << 2) | (((ChannelAddr >> (bank3 + 6)) & 1) << 3);

  } else {
    *Bank = ((ChannelAddr >> (bank0 + 6)) & 1) | (((ChannelAddr >> (bank1 + 6)) & 1) << 1) |
      (((ChannelAddr >> (bank2 + 6)) & 1) << 2) | (((ChannelAddr >> (bank3 + 6)) & 1) << 3) |
      (((ChannelAddr >> (bank4 + 6)) & 1) << 4);
  }

  *Col = ((ChannelAddr >> (((COL0REG >> 0) & 0xf) + 2)) & 0x1) |
         (((ChannelAddr >> (((COL0REG >> 4) & 0xf) + 2)) & 0x1) << 1) |
         (((ChannelAddr >> (((COL0REG >> 8) & 0xf) + 2)) & 0x1) << 2) |
         (((ChannelAddr >> (((COL0REG >> 12) & 0xf) + 2)) & 0x1) << 3) |
         (((ChannelAddr >> (((COL0REG >> 16) & 0xf) + 2)) & 0x1) << 4);

  if(numcolbits >= 6) {
    *Col |= (((ChannelAddr >> (((COL0REG >> 20) & 0xf) + 2)) & 0x1) << 5);
  }

  if(numcolbits >= 7) {
    *Col |= (((ChannelAddr >> (((COL0REG >> 24) & 0xf) + 8)) & 0x1) << 6);
  }

  if(numcolbits >= 8) {
    *Col |= (((ChannelAddr >> (((COL0REG >> 28) & 0xf) + 8)) & 0x1) << 7);
  }

  if(numcolbits >= 9) {
    *Col |= (((ChannelAddr >> (((COL1REG >> 0) & 0xf) + 8)) & 0x1) << 8);
  }

  if(numcolbits >= 10) {
    *Col |= (((ChannelAddr >> (((COL1REG >> 4) & 0xf) + 8)) & 0x1) << 9);
  }

  if(numcolbits >= 11) {
    *Col |= (((ChannelAddr >> (((COL1REG >> 8) & 0xf) + 8)) & 0x1) << 10);
  }

  if(numcolbits >= 12) {
    *Col |= (((ChannelAddr >> (((COL1REG >> 12) & 0xf) + 8)) & 0x1) << 11);
  }

  if(numcolbits >= 13) {
    *Col |= (((ChannelAddr >> (((COL1REG >> 16) & 0xf) + 14)) & 0x1) << 12);
  }

  if(numcolbits >= 14) {
    *Col |= (((ChannelAddr >> (((COL1REG >> 20) & 0xf) + 14)) & 0x1) << 13);
  }

  if(numcolbits >= 15) {
    *Col |= (((ChannelAddr >> (((COL1REG >> 24) & 0xf) + 14)) & 0x1) << 14);
  }

  *Row = (UINT32)(((ChannelAddr >> (row_lo0 + 12)) & (pow_ras (2, numrowlobits) - 1)) |
                  (((ChannelAddr >> (row_hi0 + 24)) & (pow_ras (2, numrowhibits) - 1)) << numrowlobits));

  // We will need to adjust row taking into account the InvertMSBsE/O for that Pr/Sec rank
  // Row =  Row[Msb] ^ InvertMsbE/O of Pri/SEC[1]<<Msb| Row[Msb-1] ^ InvertMsbE/O of Pri/SEC[0]<<Msb-1| Row & (2^(total number of row bits-2)-1)

  if(numrowhibits == 0) {
    *Row = ((((*Row >> (numrowlobits - 1)) & 1) ^ ((cs % 2) ? (SEC ? ((invertmsbsosec >> 1) & 1) : ((invertmsbso >> 1) & 1)) :
                                                   (SEC ? ((invertmsbsesec >> 1) & 1) : ((invertmsbse >> 1) & 1)))) << (numrowlobits - 1)) |
           ((((*Row >> (numrowlobits - 2)) & 1) ^ ((cs % 2) ? (SEC ? ((invertmsbsosec >> 0) & 1) : ((invertmsbso >> 0) & 1)) :
                                                   (SEC ? ((invertmsbsesec >> 0) & 1) : ((invertmsbse >> 0) & 1)))) << (numrowlobits - 2)) |
           (*Row & ((pow_ras (2, numrowlobits - 2)) - 1));
  } else {
    *Row = ((((*Row >> (numrowlobits + numrowhibits - 1)) & 1) ^ ((cs % 2) ? (SEC ? ((invertmsbsosec >> 1) & 1) : ((invertmsbso >> 1) & 1)) :
                                                                  (SEC ? ((invertmsbsesec >> 1) & 1) : ((invertmsbse >> 1) & 1)))) << (numrowlobits + numrowhibits - 1)) |
           ((((*Row >> (numrowlobits + numrowhibits - 2)) & 1) ^ ((cs % 2) ? (SEC ? ((invertmsbsosec >> 0) & 1) : ((invertmsbso >> 0) & 1)) :
                                                                  (SEC ? ((invertmsbsesec >> 0) & 1) : ((invertmsbse >> 0) & 1)))) << (numrowlobits + numrowhibits - 2)) |
           (*Row & ((pow_ras (2, numrowlobits + numrowhibits - 2)) - 1));
  }

  if ((addrhashbank0 & 0x1) || (addrhashbank1 & 0x1) || (addrhashbank2 & 0x1) || (addrhashbank3 & 0x1) || (addrhashbank4 & 0x1)) {
    //if (numbankbits == 3) {
    //  *Bank = (ChannelAddr >> (bank0 + 6) & 1) | ((ChannelAddr >> (bank1 + 6) & 1) << 1) | ((ChannelAddr >> (bank2 + 6) & 1) << 2);
    //
    //} else if (numbankbits == 4) {
    //  *Bank = (ChannelAddr >> (bank0 + 6) & 1) | ((ChannelAddr >> (bank1 + 6) & 1) << 1) |
    //    ((ChannelAddr >> (bank2 + 6) & 1) << 2) | ((ChannelAddr >> (bank3 + 6) & 1) << 3);
    //} else {
    //  *Bank = (ChannelAddr >> (bank0 + 6) & 1) | ((ChannelAddr >> (bank1 + 6) & 1) << 1) |
    //    ((ChannelAddr >> (bank2 + 6) & 1) << 2) | ((ChannelAddr >> (bank3 + 6) & 1) << 3) |
    //    ((ChannelAddr >> (bank4 + 6) & 1) << 4);
    //}

    gAddrData->addrhash[0] = ((internal_bit_wise_xor (*Col & ((addrhashbank0 >> 1) & 0x1fff)))
                              ^ (internal_bit_wise_xor (*Row & ((addrhashbank0 >> 14) & 0x3ffff)))) & (addrhashbank0 & 1);

    gAddrData->addrhash[1] = ((internal_bit_wise_xor (*Col & ((addrhashbank1 >> 1) & 0x1fff)))
                              ^ (internal_bit_wise_xor (*Row & ((addrhashbank1 >> 14) & 0x3ffff)))) & (addrhashbank1 & 1);

    gAddrData->addrhash[2] = ((internal_bit_wise_xor (*Col & ((addrhashbank2 >> 1) & 0x1fff)))
                              ^ (internal_bit_wise_xor (*Row & ((addrhashbank2 >> 14) & 0x3ffff)))) & (addrhashbank2 & 1);

    gAddrData->addrhash[3] = ((internal_bit_wise_xor (*Col & ((addrhashbank3 >> 1) & 0x1fff)))
                              ^ (internal_bit_wise_xor (*Row & ((addrhashbank3 >> 14) & 0x3ffff)))) & (addrhashbank3 & 1);

    gAddrData->addrhash[4] = ((internal_bit_wise_xor (*Col & ((addrhashbank4 >> 1) & 0x1fff)))
                              ^ (internal_bit_wise_xor (*Row & ((addrhashbank4 >> 14) & 0x3ffff)))) & (addrhashbank4 & 1);

    if (numbankbits == 3) {
      *Bank = (((addrhashbank0 & 0x1) == 1) ? (gAddrData->addrhash[0] ^ (*Bank & 1)) : (*Bank & 1)) |
              ((((addrhashbank1 & 1) == 1) ? (gAddrData->addrhash[1] ^ ((*Bank >> 1) & 1)) : ((*Bank >> 1) & 1)) << 1) |
              ((((addrhashbank2 & 1) == 1) ? (gAddrData->addrhash[2] ^ ((*Bank >> 2) & 1)) : ((*Bank >> 2) & 1)) << 2);
    } else if (numbankbits == 4) {
      *Bank = (((addrhashbank0 & 1) == 1) ? (gAddrData->addrhash[0] ^ (*Bank & 1)) : (*Bank & 1)) |
              ((((addrhashbank1 & 1) == 1) ? (gAddrData->addrhash[1] ^ ((*Bank >> 1) & 1)) : ((*Bank >> 1) & 1)) << 1) |
              ((((addrhashbank2 & 1) == 1) ? (gAddrData->addrhash[2] ^ ((*Bank >> 2) & 1)) : ((*Bank >> 2) & 1)) << 2) |
              ((((addrhashbank3 & 1) == 1) ? (gAddrData->addrhash[3] ^ ((*Bank >> 3) & 1)) : ((*Bank >> 3) & 1)) << 3);
    } else {
      *Bank = (((addrhashbank0 & 1) == 1) ? (gAddrData->addrhash[0] ^ (*Bank & 1)) : (*Bank & 1)) |
              ((((addrhashbank1 & 1) == 1) ? (gAddrData->addrhash[1] ^ ((*Bank >> 1) & 1)) : ((*Bank >> 1) & 1)) << 1) |
              ((((addrhashbank2 & 1) == 1) ? (gAddrData->addrhash[2] ^ ((*Bank >> 2) & 1)) : ((*Bank >> 2) & 1)) << 2) |
              ((((addrhashbank3 & 1) == 1) ? (gAddrData->addrhash[3] ^ ((*Bank >> 3) & 1)) : ((*Bank >> 3) & 1)) << 3) |
              ((((addrhashbank4 & 1) == 1) ? (gAddrData->addrhash[4] ^ ((*Bank >> 4) & 1)) : ((*Bank >> 4) & 1)) << 4);
    }
  }

  if(!SEC) {
    if (numcsbits == 0) {
      *Rankmul = 0;
    } else if (numcsbits == 1) {
      *Rankmul = (ChannelAddr >> (rm0 + 12)) & 1;
    } else if (numcsbits == 2) {
      *Rankmul = ((ChannelAddr >> (rm0 + 12)) & 1) | (((ChannelAddr >> (rm1 + 12)) & 1) << 1);
    } else {
      *Rankmul = ((ChannelAddr >> (rm0 + 12)) & 1) | (((ChannelAddr >> (rm1 + 12)) & 1) << 1) | (((ChannelAddr >> (rm2 + 12)) & 1) << 2);
    }
  } else {
    if (numcsbits == 0) {
      *Rankmul = 0;
    } else if (numcsbits == 1) {
      *Rankmul = (ChannelAddr>> (rm0sec + 12)) & 1;
    } else if (numcsbits == 2) {
      *Rankmul = ((ChannelAddr >> (rm0sec + 12)) & 1) | (((ChannelAddr >> (rm1sec + 12)) & 1) << 1);
    } else {
      *Rankmul = ((ChannelAddr >> (rm0sec + 12)) & 1) | (((ChannelAddr >> (rm1sec + 12)) & 1) << 1) | (((ChannelAddr >> (rm2sec + 12)) & 1) << 2);
    }
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Convert normalized address to chip select, row, column, bank, rankmul
 *
 * ChannelAddr expected to be passed from the caller should be till lsb=0 and only msb=39 is considered
 * no fancy 39:4 version
 * pkg_no: socket number
 * mpu_no: cdd number
 *
 *----------------------------------------------------------------------------------------*/
VOID
translate_norm_to_dram_addr (
  UINT64  ChannelAddr,
  UINT8   pkg_no,
  UINT8   mpu_no,
  UINT8   umc_inst_num,
  UINT8   umc_chan_num,
  UINT8   *cs_num,
  UINT8   *bank,
  UINT32  *row,
  UINT16  *col,
  UINT8   *rankmul
  )
{
  // Need to check the validity of the NA vs dct number vs node number
  UINT32  CSBase = 0, CSMask = 0, CSBasesec = 0, CSMasksec = 0;
  UINT8   Bank = 0, Rankmul = 0;
  UINT32  Row = 0;
  UINT16  Col = 0;
  UINT8   SEC = 0;

  UINT64  temp = 0;
  UINT8   cs  = 0;

  BOOLEAN  CSEn = 0, CSEnsec = 0;
  UINT8    Chipselect = 0;
  UINT8    noofbank = 0, noofrm = 0, noofrowlo = 0, noofrowhi = 0, noofcol = 0, noofbg = 0, bank0 = 0, bank1 = 0, bank2 = 0, bank3 = 0, bank4 = 0;
  UINT8    numrowlobits = 0, numrowhibits = 0, numcolbits = 0, numcsbits = 0, numbankbits = 0, numbgbits = 0;
  UINT8    row_lo0 = 0, row_hi0 = 0, rm0 = 0, rm1 = 0, rm2 = 0, chan = 0, invertmsbse = 0, invertmsbso = 0;
  UINT8    rm0sec = 0, rm1sec = 0, rm2sec = 0, chansec = 0, invertmsbsesec = 0, invertmsbsosec = 0;
  UINT32   COL0REG = 0, COL1REG = 0;
  UINT32   addrhashbank0 = 0, addrhashbank1 = 0, addrhashbank2 = 0, addrhashbank3 = 0, addrhashbank4 = 0;
  UINT32   addrhashbankpc = 0, addrhashbankpc2 = 0, addrhashnormaddr0 = 0, addrhashnormaddr1 = 0;
  UINT8    bankgroupen = 0, vcm_en = 0;
  UINTN    channelId;
  UINT32   InputAddr;

  // channelId: channel ID of system
  umc_chan_num = 0;   // umc_chan_num = 0 in SAT
  channelId    = convert_to_addr_trans_index (pkg_no, mpu_no, umc_inst_num, umc_chan_num);

  // get_highest_possible_addr_bit(pkg_no, mpu_no, umc_inst_num, umc_chan_num);

  // read out the addrhash* registers here
  addrhashbank0     = gAddrData->ADDRHASHBANK0[channelId];
  addrhashbank1     = gAddrData->ADDRHASHBANK1[channelId];
  addrhashbank2     = gAddrData->ADDRHASHBANK2[channelId];
  addrhashbank3     = gAddrData->ADDRHASHBANK3[channelId];
  addrhashbank4     = gAddrData->ADDRHASHBANK4[channelId];
  addrhashbankpc    = gAddrData->ADDRHASHBANKPC[channelId]; // nhu, useless, 01/31
  addrhashbankpc2   = gAddrData->ADDRHASHBANKPC2[channelId]; // nhu, useless, 01/31
  addrhashnormaddr0 = gAddrData->ADDRHASHNORMADDR[channelId][0];
  addrhashnormaddr1 = gAddrData->ADDRHASHNORMADDR[channelId][1];

  for (cs = 0; cs < 4; ++cs) {
    // nhu CSBASESEC[][y], retrieve_regs just save register data to [3:0], need to change to cs < 4
    CSBase = gAddrData->CSBASE[channelId][cs];
    CSMask = gAddrData->CSMASK[channelId][cs >> 1];

    // Secondary decoder stuff for each primary
    CSBasesec = gAddrData->CSBASESEC[channelId][cs];
    CSMasksec = gAddrData->CSMASKSEC[channelId][cs >> 1];

    // UMC0CHx00000000 [DRAM CS Base Address][31:1]: Base Address[40:10], shift error address right 9 bit to match BaseAddr
    InputAddr = (ChannelAddr >> 9) & 0xffffffff;

    // Read out *RAMCFG*
    temp = gAddrData->CONFIGDIMM[channelId][cs >> 1];
    CSEn = (temp >> (cs & 0x1)) & 1;

    // Secodnary decoder stuff
    // [25:24]  of CONFIGDIMM is the cssec [1:0]
    CSEnsec = (cs & 0x1) ? ((temp >> 25) & 0x1) : ((temp >> 24) & 0x1);

    if ((CSEn && ((InputAddr & ~CSMask) == (CSBase & ~CSMask))) ||
        (CSEnsec && ((InputAddr & ~CSMasksec) == (CSBasesec & ~CSMasksec)))) {
      // Hashing
      // Dealing with cshash..
      Chipselect = 0;
      if (gAddrData->ADDRHASHNORMADDR[channelId][0] & 0x1) {
        Chipselect = ((internal_bit_wise_xor ((gAddrData->ADDRHASHNORMADDR[channelId][0] >> 1) & (ChannelAddr >> 10))) ^ (cs & 0x1));
      } else {
        Chipselect = (cs & 0x1);
      }

      if (gAddrData->ADDRHASHNORMADDR[channelId][1] & 0x1) {
        Chipselect |= (((internal_bit_wise_xor((gAddrData->ADDRHASHNORMADDR[channelId][1] >> 1) & (ChannelAddr >> 10))) ^ ((cs >> 1) & 0x1)) << 1);
      } else {
        Chipselect |= ((cs >> 1) & 0x1) << 1;
      }

      // Hashing end

      SEC = (CSEn && ((InputAddr & ~CSMask) == (CSBase & ~CSMask))) ? 0 :
            (CSEnsec && ((InputAddr & ~CSMasksec) == (CSBasesec & ~CSMasksec))) ? 1 : 3;
      InputAddr = 0;

      noofbank  = (temp >> 4) & 0x3;
      noofrm    = (temp >> 6) & 0x3;
      noofrowlo = (temp >> 8) & 0xf;
      noofrowhi = (temp >> 12) & 0xf;
      noofcol   = (temp >> 16) & 0xf;
      noofbg    = (temp >> 20) & 0x3;

      // Read out *BANK_SEL*
      temp  = gAddrData->BANKSELDIMM[channelId][cs >> 1];
      bank0 = temp & 0xf;
      bank1 = (temp >> 4) & 0xf;
      bank2 = (temp >> 8) & 0xf;
      bank3 = (temp >> 12) & 0xf;
      bank4 = (temp >> 16) & 0xf;

      break;
    }   // csen inputaddr loop
  }   // cs loop

  DEBUG((EFI_D_ERROR, "noofbank = %d, noofrm = %d, noofrwolo = %d, noofrowhi = %d, noofcol = %d, noofbg = %d\n", 
         noofbank, noofrm, noofrowlo, noofrowhi, noofcol, noofbg));

  if (((gAddrData->RANK_ENABLE_PER_UMCCH_ADDR_TRANS[channelId] >> cs) & 0x1) == 0) {
    DEBUG ((EFI_D_ERROR, "ERROR: Rank is out of bounds\n", noofbank, noofrm, noofrowlo, noofrowhi, noofcol, noofbg));
  }

  // Read out *ROW_SEL*
  temp    = gAddrData->ROWSELDIMM[channelId][cs >> 1];
  row_lo0 = (temp >> 0) & 0xf;
  row_hi0 = (temp >> 4) & 0xf;

  // Read out *COL0_SEL*
  COL0REG = gAddrData->COL0SELDIMM[channelId][cs >> 1];

  // Read out *COL1_SEL*
  COL1REG = gAddrData->COL1SELDIMM[channelId][cs >> 1];

  // Read out *RM_SEL*
  temp = gAddrData->RMSELDIMM[channelId][cs >> 1];
  rm0  = (temp >> 0) & 0xf;
  rm1  = (temp >> 4) & 0xf;
  rm2  = (temp >> 8) & 0xf;
  // [15:12] reserved in SAT.chan = (temp >> 12) & 0xf;
  invertmsbse = (temp >> 16) & 0x3;
  invertmsbso = (temp >> 18) & 0x3;

  temp   = gAddrData->RMSELDIMMSEC[channelId][cs >> 1];
  rm0sec = (temp >> 0) & 0xf;
  rm1sec = (temp >> 4) & 0xf;
  rm2sec = (temp >> 8) & 0xf;
  // chansec = (temp>> 12) & 0xf;
  invertmsbsesec = (temp >> 16) & 0x3;
  invertmsbsosec = (temp >> 18) & 0x3;

  numrowlobits = noofrowlo + 10;
  numrowhibits = noofrowhi;
  numcolbits   = noofcol + 5;
  numcsbits    = noofrm;
  numbankbits  = (noofbank == 2) ? 5 : (noofbank == 1) ? 4 : 3;   // UMC0CHx00000030 [DRAM Address Configuration][21:20]
  // nhu numbgbits = (UINT32) pow_ras (2, noofbg);
  numbgbits = noofbg;

  // Let us see if we need to swizzle or not
  temp   = gAddrData->CTRLREG[channelId][cs >> 1];
  vcm_en = (temp >> 13) & 0x1;    // nhu vcm_en = DDR4eEn?
  bankgroupen = (temp >> 5) & 0x1;
  DEBUG((EFI_D_ERROR, "RowLoBits = %d, RowHiBits = %d, ColBits = %d, CsBits = %d, BankBits = %d, BangGpBits = %d\n", 
         numrowlobits, numrowhibits, numcolbits, numcsbits, numbankbits, numbgbits));


  NormalizedToBankAddrMap (
    ChannelAddr,
    CSMask,
    &Bank,
    &Row,
    &Col,
    &Rankmul,
    numbankbits,
    bank4,
    bank3,
    bank2,
    bank1,
    bank0,
    numrowlobits,
    numrowhibits,
    numcolbits,
    row_lo0,
    row_hi0,
    COL0REG,
    COL1REG,
    numcsbits,
    rm0,
    rm1,
    rm2,
    chan,
    vcm_en,
    numbgbits,
    bankgroupen,
    invertmsbse,
    invertmsbso,
    rm0sec,
    rm1sec,
    rm2sec,
    chansec,
    invertmsbsesec,
    invertmsbsosec,
    CSMasksec,
    SEC,
    cs,
    addrhashbank0,
    addrhashbank1,
    addrhashbank2,
    addrhashbank3,
    addrhashbank4,
    addrhashbankpc,
    addrhashbankpc2,
    addrhashnormaddr0,
    addrhashnormaddr1
    );

  // if((DRAMTYPE == 0) || (DRAMTYPE == 5)) {
  // if ((NormAddr & ((pow_ras (2, VALIDHI[channelId]) - 1) >> 3)) != (ChannelAddr & ((pow_ras (2, VALIDHI[channelId]) - 1) >> 3))) {
  ////nhuif(ASSERT_UMC_ADDR_TRANS){
  ////nhu  printf ("File of coredump is %s and line of coredump is %d\n",__FILE__,__LINE__); exit(1);}
  ////nhuelse
  ////nhu  printf ("ERROR: Given NormAddr_39_3=%jx, Backward NormAddr_39_3=%jx\n\n",
  ////nhu      (ChannelAddr & ((uint64_t)pow_ras(2,VALIDHI[channelId])-1) )>>3,(NormAddr & ((uint64_t)pow_ras(2,VALIDHI[channelId])-1))>>3);
  // }
  // }

  *cs_num  = Chipselect;
  *bank    = Bank;
  *row     = Row;
  *col     = Col;
  *rankmul = Rankmul;

  // Check if the outputs respect the config
  if (Bank >= ((noofbank == 2) ? 32 : ((noofbank == 1) ? 16 : 8))) {
    DEBUG ((EFI_D_ERROR, "ERROR: Bank of the Given NormAddr is out of bounds\n"));
  }

  if (Row >= (UINT32)(pow_ras (2, (noofrowlo + 10 + noofrowhi)))) {
    DEBUG ((EFI_D_ERROR, "ERROR: Row of the Given NormAddr is out of bounds\n"));
  }

  if (Col >= (UINT16)(pow_ras (2, (noofcol + 5)))) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "ERROR: Col of the Given NormAddr is out of bounds\n");
  }
}

/*------------------------------------------------------------------
 Function: checkDramHit

 Purpose: Lookup a system address in the DRAM address map.

 Parameters (all are input only)
   sysAddr (ulong)
     The address to be converted.
     The user must remove all VMGuard key indices from the system address.
   ccmInstanceId (uint)
     An instance ID of a CCM that we will look up the map.
 Returns:
   The destination (CS) fabric ID

 Side Effects:
   None:

 Limitations:
   - Does not support non-power-of-2 channels (not used on SAT)

 *------------------------------------------------------------------*/
UINT32
checkDramHit (
  UINT64  sysAddr,
  UINT32  ccmInstanceId
  )
{
  UINT64                                 dramBaseAddr;
  UINT64                                 dramLimitAddr;
  UINT64                                 dramHoleBase;
  UINT64                                 postHoleSysAddr;
  UINT64                                 preIntlvSysAddr;
  UINT32                                 dramBaseReg;
  UINT32                                 dramLimitReg;
  UINT32                                 systemFabricIdReg;
  UINT32                                 addrRngVal;
  UINT32                                 dstFabricId;
  UINT32                                 intLvAddrSel;
  UINT32                                 intLvAddrBit;
  UINT32                                 intLvNumSockets;
  UINT32                                 intLvNumCdds;
  UINT32                                 intLvNumChan;
  UINT32                                 firstDieIdBit;
  UINT32                                 firstSocketIdBit;
  UINT32                                 csId;
  UINT32                                 numSocketsInterleaved;
  UINT32                                 numCddInterleaved;
  UINT32                                 numChanInterleaved;
  UINT32                                 numInterleaveBits;
  UINT32                                 hashEnabled;
  UINT32                                 holeEn;
  UINT32                                 socketIdShift;
  UINT32                                 socketIdMask;
  UINT32                                 dieIdMask;
  UINT32                                 dieIdShift;
  UINT32                                 addrMapNum;
  UINT64                                 modBits;
  UINT32                                 CpuModel;
  FABRIC_DRAM_ADDR_CONFIG0_REGISTER_HYGX  DramAddrConfig0;
  FABRIC_DRAM_ADDR_CONFIG1_REGISTER_HYGX  DramAddrConfig1;

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_GX_CPU) {
    DramAddrConfig0.Value = FabricRegisterAccRead (0, 0, DRAMADDRCONFIG0_FUNC, DRAMADDRCONFIG0_REG_HYGX, IOMS0_INSTANCE_ID);
    DramAddrConfig1.Value = FabricRegisterAccRead (0, 0, DRAMADDRCONFIG1_FUNC, DRAMADDRCONFIG1_REG_HYGX, IOMS0_INSTANCE_ID);
  }
  for (addrMapNum = 0; addrMapNum < NUM_DRAM_MAPS; addrMapNum++) {
    dramBaseReg = getDfRegDramBase (0, 0, addrMapNum, ccmInstanceId);
    addrRngVal  = getBit (ADDRRNGVAL, dramBaseReg);
    if (addrRngVal == 0) {
      continue;
    }

    dramLimitReg = getDfRegDramLimit (0, 0, addrMapNum, ccmInstanceId);
    dramBaseAddr = getBits (DRAMBASEADDRLO, DRAMBASEADDRHI, dramBaseReg);
    dramBaseAddr = dramBaseAddr << 28;

    dramLimitAddr  = getBits (DRAMLIMITADDRLO, DRAMLIMITADDRHI, dramLimitReg);
    dramLimitAddr  = dramLimitAddr << 28;
    dramLimitAddr |= ((UINT64)0x0FFFFFFF);

    if ((sysAddr >= dramBaseAddr) && (sysAddr <= dramLimitAddr)) {
      systemFabricIdReg = getCddDfRegSystemFabricIdMask (0, 0);
      socketIdShift     = getBits (SOCKETIDSHIFTLO, SOCKETIDSHIFTHI, systemFabricIdReg);

      socketIdMask = getBits (SOCKETIDMASKLO, (CpuModel == HYGON_EX_CPU)?SOCKETIDMASKHI_HYEX:SOCKETIDMASKHI_HYGX, systemFabricIdReg);
      dieIdShift   = getBits (DIEIDSHIFTLO, DIEIDSHIFTHI, systemFabricIdReg);
      dieIdMask    = getBits (DIEIDMASKLO, (CpuModel == HYGON_EX_CPU)?DIEIDMASKHI_HYEX:DIEIDMASKHI_HYGX, systemFabricIdReg);

      // hit, now figure out the DstFabricId.
      intLvAddrSel = getBits (INTLVADDRSELLO, INTLVADDRSELHI, dramBaseReg);
      // assert (getBit(2, intLvAddrSel) == 0);
      intLvAddrBit = (getBit (1, (UINT32)intLvAddrSel) ? (getBit (0, (UINT32)intLvAddrSel) ? 11 : 10) : (getBit (0, (UINT32)intLvAddrSel) ? 9 : 8));

      intLvNumChan = getBits (INTLVNUMCHANLO, INTLVNUMCHANHI, dramBaseReg);
      // assert (intLvNumChan != 4);
      // assert (intLvNumChan != 6);
      // assert (intLvNumChan <= 8);
      // If we are using address hashing, the interleave address bit must be 8 or 9.
      // assert ((intLvNumChan != 8) || (intLvAddrSel == 0) || (intLvAddrSel == 1));
      if (intLvNumChan == 8) {
        hashEnabled = TRUE;
      } else {
        hashEnabled = FALSE;
      }

      numChanInterleaved = 1;
      numCddInterleaved  = 1;
      switch (intLvNumChan) {
        case 0:
          numChanInterleaved = 1;
          numInterleaveBits  = 0;
          break;
        case 1:
        case 8:
          numChanInterleaved = 2;
          numInterleaveBits  = 1;
          break;
        case 2:
          numChanInterleaved = 3;
          numInterleaveBits = 2;
          break;
        case 3:
          numChanInterleaved = 4;
          numInterleaveBits  = 2;
          break;
        case 5:
          numChanInterleaved = 8;
          numInterleaveBits  = 3;
          break;
        case 7:
          numChanInterleaved = 16;
          numInterleaveBits  = 4;
          break;
          // default:
          // assert(0);
      }

      if (CpuModel == HYGON_EX_CPU) {
	    intLvNumCdds = getBits (INTLVNUMDIESLO, INTLVNUMDIESHI, dramLimitReg);
	  } else if (CpuModel == HYGON_GX_CPU) {
		if (addrMapNum < 16) {
	      intLvNumCdds = (DramAddrConfig0.Value >> (addrMapNum * 2)) & 0x3;
	    } else {
	      intLvNumCdds = (DramAddrConfig1.Value >> ((addrMapNum-16) * 2)) & 0x3;
	    }
	  }
      
      // assert (intLvNumCdds != 3);
      switch (intLvNumCdds) {
        case 0:
          numCddInterleaved = 1;
          break;
        case 1:
          numCddInterleaved  = 2;
          numInterleaveBits += 1;
          break;
        case 2:
          numCddInterleaved  = 4;
          numInterleaveBits += 2;
          break;
        case 3:
          numCddInterleaved  = 8;
          numInterleaveBits += 3;
          break;
        default:
          numCddInterleaved = 1;
		  break;
      }

      // numSocketsInterleaved = getBit(INTLVNUMSKTS, dramLimitReg) + 1;
      // numInterleaveBits += (numSocketsInterleaved - 1);
      // assert (numInterleaveBits <= 4);
      intLvNumSockets = getBits (INTLVNUMSKTSLO, INTLVNUMSKTSHI, dramBaseReg);
      switch (intLvNumSockets) {
        case 0:
          numSocketsInterleaved = 1;
          break;
        case 1:
          numSocketsInterleaved = 2;
          numInterleaveBits    += 1;
          break;
        case 2:
          numSocketsInterleaved = 4;
          numInterleaveBits    += 2;
          break;
        case 3:
          numSocketsInterleaved = 8;
          numInterleaveBits    += 3;
          break;
          // default:
          // assert(0);
      }

      dstFabricId = getBits (DSTFABRICIDLO, (CpuModel == HYGON_EX_CPU)?DSTFABRICIDHI_HYEX:DSTFABRICIDHI_HYGX, dramLimitReg);

      // Calculate the CSID that we take from the address.
      firstDieIdBit = intLvAddrBit;
      if (numChanInterleaved > 1) {
        if (hashEnabled) {
          // [hashAddrBit] = XOR of sysAddr{intLvAddrBit, 12, 18, 21, 30}
          dstFabricId += (getBit (intLvAddrBit, (UINT32)sysAddr) ^
                          getBit (12, (UINT32)sysAddr) ^
                          getBit (18, (UINT32)sysAddr) ^
                          getBit (21, (UINT32)sysAddr) ^
                          getBit (30, (UINT32)sysAddr));
          firstDieIdBit++;
        } else {
          // firstDieIdBit += log2 (numChanInterleaved);
          switch (numChanInterleaved) {
          case 2:
            firstDieIdBit += 1;
            break;
          case 3:
            // Calculate the CSID for 3 channel interleaving
            holeEn = getBit(LGCYMMIOHOLEEN, dramBaseReg);
            dramHoleBase = getDfRegDramHoleCtrl(0, 0, ccmInstanceId);
            dramHoleBase = dramHoleBase << 24;

            // Account for the DRAM hole
            if (holeEn && (sysAddr >= dramHoleBase)) {
              postHoleSysAddr = sysAddr - ((((UINT64) 1) << 32) - dramHoleBase);
            } else {
              postHoleSysAddr = sysAddr;
            }

            // Subtract the base.
            preIntlvSysAddr = postHoleSysAddr - dramBaseAddr;

            modBits = (preIntlvSysAddr >> (numInterleaveBits + intLvAddrBit)) << 2 | ((preIntlvSysAddr >> intLvAddrBit) & 0x3);
            csId = modBits % 3;
            dstFabricId += csId;

            firstDieIdBit += 2;
            break;
          case 4:
            firstDieIdBit += 2;
            break;
          case 8:
            firstDieIdBit += 3;
            break;
          case 16:
            firstDieIdBit += 4;
            break;
          }
          if (numChanInterleaved != 3) {
            dstFabricId += (UINT32) getBits64 (intLvAddrBit, firstDieIdBit - 1, sysAddr);
          }
        }
      }

      firstSocketIdBit = firstDieIdBit;
      if (numCddInterleaved > 1) {
        // firstSocketIdBit += log2(numCddInterleaved);
        switch (numCddInterleaved) {
          case 2:
            firstSocketIdBit += 1;
            break;
          case 4:
            firstSocketIdBit += 2;
            break;
          case 8:
            firstSocketIdBit += 3;
            break;
        }

        csId = (UINT32)getBits64 (firstDieIdBit, firstSocketIdBit - 1, sysAddr);
        dstFabricId |= (csId << dieIdShift);
      }

      if (numSocketsInterleaved > 1) {
        csId = (UINT32)getBits64 (firstSocketIdBit, firstSocketIdBit + numSocketsInterleaved - 1, sysAddr);
        dstFabricId |= (csId << socketIdShift);
      }

      return (dstFabricId);
    }
  }

  // assert(0); // missed in address maps.
  return (0);
}

/*------------------------------------------------------------------
 Function: calcSysAddr

 Purpose: Denormalize an address from a memory controller into a
   system address.

 Parameters (all are input only)
   normAddr (ulong)
     The address to be converted
   mySocketNum (uint):
     This socket number within the system where this normalized
     address was found.
     Valid range (on Dharma): 0-1
   myCddNum (uint):
     This cdd number within the socket where this normalized
     address was found.
     Valid range (on Dharma): 0-3
   myChannelNum (uint):
     This channel number within the cdd where this normalized
     address was found.
     Valid range (on Dharma): 0-2
   mySubChannelNum (uint):
     This sub channel number within the UMC where this normalized
     address was found.
     Valid range (on Dharma): 0-1
 Returns:
   A system address (ulong)

 Side Effects:
   None:

 Limitations:
   - Does not support non-power-of-2 channels (not used on SAT)
   - The system address returned will not have any VMGuard key information
   - When the UMC address is being used for system functions
     e.g. PSP private area, CC6 save address); the algorithm will
    give you the system address - which is actually a E820 hole in
    the memory. The components are accessing this through a
    different address (the 'magic' or 'privileged' region).
    We could expand this algorithm to detect these ranges and
    change it to a 'magic' address; but it wasn't clear what
    the expectations were).

 *------------------------------------------------------------------*/
UINT64
calcSysAddr (
  UINT64  normAddr,
  UINT32  mySocketNum,
  UINT32  myCddNum,
  UINT32  myChannelNum,
  UINT32  mySubChannelNum
  )
{
  UINT64                                 hiAddrOffset;
  UINT64                                 dramBaseAddr;
  UINT64                                 dramLimitAddr;
  UINT64                                 dramHoleBase;
  UINT64                                 preBaseSysAddr;
  UINT64                                 sysAddr;
  UINT64                                 preNormAddr;
  UINT64                                 deIntLvAddr;
  UINT32                                 systemFabricIdReg;
  UINT32                                 dramBaseReg;
  UINT32                                 dramLimitReg;
  UINT32                                 socketIdShift;
  UINT32                                 socketIdMask;
  UINT32                                 dieIdMask;
  UINT32                                 dieIdShift;
  UINT32                                 numDFInstances;
  UINT32                                 csInstanceId;
  UINT32                                 csFabricId;
  UINT32                                 base;
  UINT32                                 intLvAddrSel;
  UINT32                                 intLvAddrBit;
  UINT32                                 intLvNumSockets;
  UINT32                                 intLvNumCdds;
  UINT32                                 intLvNumChan;
  UINT32                                 numSocketsInterleaved;
  UINT32                                 numCddInterleaved       = 0;
  UINT32                                 numChanInterleaved      = 0;
  UINT32                                 numInterleaveBits       = 0;
  UINT32                                 numChanInterleaveBits   = 0;
  UINT32                                 numCddInterleaveBits    = 0;
  UINT32                                 numSocketInterleaveBits = 0;
  UINT32                                 csId = 0;
  UINT32                                 holeEn;
  UINT32                                 hiAddrOffsetEn;
  UINT32                                 addrRngVal;
  UINT32                                 hashEnabled;
  UINT32                                 hashedBit;
  UINT32                                 CsConfigA2Val;
  BOOLEAN                                DramChanAddrSel;
  BOOLEAN                                EnDramChanHash;
  BOOLEAN                                EnDDR5;
  UINT32                                 Ddr5StartBit;
  UINT64                                 AddrMul3Offset;
  UINT32                                 CpuModel;
  FABRIC_DRAM_ADDR_CONFIG0_REGISTER_HYGX  DramAddrConfig0;
  FABRIC_DRAM_ADDR_CONFIG1_REGISTER_HYGX  DramAddrConfig1;

  ASSERT (mySocketNum < MAX_SOCKETS_SUPPORTED);
  ASSERT (myCddNum < MAX_CDDS_PER_SOCKET);
  ASSERT (myChannelNum < MAX_CHANNELS_PER_CDD);

  // IDS_HDT_CONSOLE (MAIN_FLOW, "calcSysAddr In Smm Mode\n");
  
  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_GX_CPU) {
    DramAddrConfig0.Value = FabricRegisterAccRead (mySocketNum, myCddNum, DRAMADDRCONFIG0_FUNC, DRAMADDRCONFIG0_REG_HYGX, IOMS0_INSTANCE_ID);
    DramAddrConfig1.Value = FabricRegisterAccRead (mySocketNum, myCddNum, DRAMADDRCONFIG1_FUNC, DRAMADDRCONFIG1_REG_HYGX, IOMS0_INSTANCE_ID);
  }
  CsConfigA2Val = CddFabricRegisterAccRead (mySocketNum, myCddNum, (UINTN)CS_CONFIG_A2_FUNC, (UINTN)CS_CONFIG_A2_REG, (UINTN)FABRIC_REG_ACC_BC);
  EnDDR5 = (BOOLEAN)((CsConfigA2Val & BIT19) >> 19);
  EnDramChanHash  = (BOOLEAN)((CsConfigA2Val & BIT23) >> 23);
  DramChanAddrSel = (BOOLEAN)((CsConfigA2Val & BIT24) >> 24);

  // Detect some information about the system
  systemFabricIdReg = getCddDfRegSystemFabricIdMask (mySocketNum, myCddNum);
  socketIdShift     = getBits (SOCKETIDSHIFTLO, SOCKETIDSHIFTHI, systemFabricIdReg);
  socketIdMask   = getBits (SOCKETIDMASKLO, (CpuModel == HYGON_EX_CPU)?SOCKETIDMASKHI_HYEX:SOCKETIDMASKHI_HYGX, systemFabricIdReg);
  dieIdShift     = getBits (DIEIDSHIFTLO, DIEIDSHIFTHI, systemFabricIdReg);
  dieIdMask      = getBits (DIEIDMASKLO, (CpuModel == HYGON_EX_CPU)?DIEIDMASKHI_HYEX:DIEIDMASKHI_HYGX, systemFabricIdReg);
  numDFInstances = getBits (BLKINSTCOUNTLO, BLKINSTCOUNTHI, getCddDfRegFabricBlkInstanceCnt (mySocketNum, myCddNum));

  // Find the matching CS
  // This is a bit of "magic" (requires one to "know" the methodology
  // for attaching CS->UMC rather than programmatically determining it).
  // It is safe to assume that the UMC number matches the CS sequence
  // (the second UMC will be attached to the second CS that you find
  // in the search) and will be on the same socket/die/offset.
  // CS instance IDs can be detected by scanning (from instance number 0
  // to numDFInstances) and pulling out those that have
  // DF::FabricBlockInstanceInformation0[InstanceType] == CS.
  // The number you find should equal DF::DieComponentMapB[myDieType][CSCount]
  // (do a broadcast read for this). Note: This is the last broadcast read of the
  // algorithm. All future accesses are instance-only reads to the targeted CS.

  // BOZO: finish this, for now, we're just using CsInstanceID = channel number
  csInstanceId = myChannelNum;

  // Once you have the CS instance ID, now get the FabricID:
  csFabricId = getBits (BLOCKFABRICIDLO, (CpuModel == HYGON_EX_CPU)?BLOCKFABRICIDHI_HYEX:BLOCKFABRICIDHI_HYGX, getCddDfRegFabricBlkInstInfo3 (mySocketNum, myCddNum, csInstanceId));
  // IDS_HDT_CONSOLE (MAIN_FLOW, "csInstanceId = %d, csFabricId = %x\n", csInstanceId, csFabricId);

  // Read the CS offset registers and determine whether this
  // address was part of base/limit register 0, 1, or 2.
  // Since base 2 is only used in tri-channel mappings, which the
  // algorithm doesn't implement, this skips that.
  hiAddrOffsetEn = getBit (HIADDROFFSETEN, getDfRegDramOffset (mySocketNum, myCddNum, 1, csInstanceId));
  hiAddrOffset   = getBits (HIADDROFFSETLO, HIADDROFFSETHI, getDfRegDramOffset (mySocketNum, myCddNum, 1, csInstanceId));
  hiAddrOffset   = hiAddrOffset << 28;
  // IDS_HDT_CONSOLE (MAIN_FLOW, "hiAddrOffsetEn[1] = %d, hiAddrOffset[1] = 0x%016lX\n", hiAddrOffsetEn, hiAddrOffset);
  if (hiAddrOffsetEn && (normAddr >= hiAddrOffset)) {
    base = 1;
  } else {
    base = 0;
  }

  // Now set up variables from the CS base/limit registers
  dramBaseReg  = getDfRegDramBase (mySocketNum, myCddNum, base, csInstanceId);
  dramLimitReg = getDfRegDramLimit (mySocketNum, myCddNum, base, csInstanceId);
  // IDS_HDT_CONSOLE (MAIN_FLOW, "dramBaseReg = 0x%08X, dramLimitReg = 0x%08X\n", dramBaseReg, dramLimitReg);

  addrRngVal = getBit (ADDRRNGVAL, dramBaseReg);
  ASSERT (addrRngVal == 1);

  if (base == 0) {
    hiAddrOffset = 0;
  } else {
    hiAddrOffset = getBits (HIADDROFFSETLO, HIADDROFFSETHI, getDfRegDramOffset (mySocketNum, myCddNum, base, csInstanceId));
    hiAddrOffset = hiAddrOffset << 28;
  }

  // IDS_HDT_CONSOLE (MAIN_FLOW, "hiAddrOffset = 0x%016lX\n", hiAddrOffset);

  dramBaseAddr = getBits (DRAMBASEADDRLO, DRAMBASEADDRHI, dramBaseReg);
  dramBaseAddr = dramBaseAddr << 28;
  // IDS_HDT_CONSOLE (MAIN_FLOW, "dramBaseAddr = 0x%016lX\n", dramBaseAddr);

  dramLimitAddr  = getBits (DRAMLIMITADDRLO, DRAMLIMITADDRHI, dramLimitReg);
  dramLimitAddr  = dramLimitAddr << 28;
  dramLimitAddr |= ((UINT64)0x0FFFFFFF);
  // IDS_HDT_CONSOLE (MAIN_FLOW, "dramLimitAddr = 0x%016lX\n", dramLimitAddr);

  holeEn = getBit (LGCYMMIOHOLEEN, dramBaseReg);
  dramHoleBase = getBits (DRAMHOLEBASELO, DRAMHOLEBASEHI, getDfRegDramHoleCtrl (mySocketNum, myCddNum, csInstanceId));
  dramHoleBase = dramHoleBase << 24;
  // IDS_HDT_CONSOLE (MAIN_FLOW, "holeEn = %d, dramHoleBase = 0x%016lX\n", holeEn, dramHoleBase);

  intLvAddrSel = getBits (INTLVADDRSELLO, INTLVADDRSELHI, dramBaseReg);
  intLvAddrBit = (getBit (1, intLvAddrSel) ? (getBit (0, intLvAddrSel) ? 11 : 10) : (getBit (0, intLvAddrSel) ? 9 : 8));

  intLvNumChan = getBits (INTLVNUMCHANLO, INTLVNUMCHANHI, dramBaseReg);
  // If we are using address hashing, the interleave address bit must be 8 or 9.
  // ASSERT ((intLvNumChan != 8) || (intLvAddrSel == 0) || (intLvAddrSel == 1));
  if (intLvNumChan == 8) {
    hashEnabled = TRUE;
  } else {
    hashEnabled = FALSE;
  }

  // Calc channel Interleave bits width
  switch (intLvNumChan) {
    case 0:
      numChanInterleaved    = 1;
      numChanInterleaveBits = 0;
      break;
    case 1:
    case 8:
      numChanInterleaved    = 2;
      numChanInterleaveBits = 1;
      break;
    case 2:
      numChanInterleaved    = 3;
      numChanInterleaveBits = 2;
      break;
    case 3:
      numChanInterleaved    = 4;
      numChanInterleaveBits = 2;
      break;
    case 5:
      numChanInterleaved    = 8;
      numChanInterleaveBits = 3;
      break;
    case 7:
      numChanInterleaved    = 16;
      numChanInterleaveBits = 4;
      break;
    default:
      numChanInterleaved    = 1;
      numChanInterleaveBits = 0;
      break;
  }

  // Calc cdd Interleave bits width
  if (CpuModel == HYGON_EX_CPU) {
    intLvNumCdds = getBits (INTLVNUMDIESLO, INTLVNUMDIESHI, dramLimitReg);
  } else if (CpuModel == HYGON_GX_CPU) {
    intLvNumCdds = DramAddrConfig0.Value & 0x3; // Need check
  }
  
  switch (intLvNumCdds) {
    case 0:
      numCddInterleaved = 1;
      break;
    case 1:
      numCddInterleaved    = 2;
      numCddInterleaveBits = 1;
      break;
    case 2:
      numCddInterleaved    = 4;
      numCddInterleaveBits = 2;
      break;
    case 3:
      numCddInterleaved    = 8;
      numCddInterleaveBits = 3;
	  break;
    default:
      numCddInterleaved = 1;
      break;
  }

  // Calc socket Interleave bits width
  intLvNumSockets = getBits (INTLVNUMSKTSLO, INTLVNUMSKTSHI, dramBaseReg);
  switch (intLvNumSockets) {
    case 0:
      numSocketsInterleaved = 1;
      break;
    case 1:
      numSocketsInterleaved   = 2;
      numSocketInterleaveBits = 1;
      break;
    case 2:
      numSocketsInterleaved   = 4;
      numSocketInterleaveBits = 2;
      break;
    case 3:
      numSocketsInterleaved   = 8;
      numSocketInterleaveBits = 3;
      break;
    default:
      numSocketsInterleaved = 1;
      break;
  }

  // Calc total Interleave bits width
  numInterleaveBits = numChanInterleaveBits + numCddInterleaveBits + numSocketInterleaveBits;
  //IDS_HDT_CONSOLE (MAIN_FLOW, "intLvAddrBit = %d, numSocketsInterleaved = %d, numCddInterleaved = %d, numChanInterleaved = %d, numInterleaveBits = %d, hashEnabled = %d\n",
  //                 intLvAddrBit, numSocketsInterleaved, numCddInterleaved, numChanInterleaved, numInterleaveBits, hashEnabled);

  // Subtract the offset
  if (EnDDR5) {
    // Insert DDR5 sub channel bit
    if (DramChanAddrSel) {
      Ddr5StartBit = 8;
      preNormAddr  = InsertBits64 (normAddr, (UINT64)mySubChannelNum, 8, 1);
    } else {
      Ddr5StartBit = 7;
      preNormAddr  = InsertBits64 (normAddr, (UINT64)mySubChannelNum, 7, 1);
    }

    preNormAddr = preNormAddr - hiAddrOffset;
  } else {
    preNormAddr = normAddr - hiAddrOffset;
  }

  // IDS_HDT_CONSOLE (MAIN_FLOW, "preNormAddr = 0x%016lX\n", preNormAddr);

  // Account for interleaving bits
  deIntLvAddr = preNormAddr;
  if (numInterleaveBits > 0) {
    // Calculate the CSID that is part of the de-normalized address.
    csId = 0;

    if (numChanInterleaved == 3) {
      // AddrMul3Offset = preNormAddr high * 3 + channel ID
      AddrMul3Offset = getBits64 ((UINT32)intLvAddrBit, (UINT32)63, preNormAddr) * 3 + (csFabricId & 0x03);
      csId = AddrMul3Offset & 0x03;

      if (numCddInterleaveBits > 0) {
        csId |= (((csFabricId & dieIdMask) >> dieIdShift) << numChanInterleaveBits);
      }

      if (numSocketInterleaveBits > 0) {
        csId |= (((csFabricId & socketIdMask) >> socketIdShift) << (numChanInterleaveBits + numCddInterleaveBits));
      }

      // IDS_HDT_CONSOLE (MAIN_FLOW, "csId = 0x%x\n", csId);
      deIntLvAddr = ((AddrMul3Offset & (~0x3)) << (intLvAddrBit + numCddInterleaveBits + numSocketInterleaveBits)) |
                    (csId << intLvAddrBit) |
                    getBits64 ((UINT32)0, (UINT32)(intLvAddrBit -1), preNormAddr);
    } else {
      // Insert csId to preNormAddr, start from intLvAddrBit, width is numInterleaveBits
      if (numChanInterleaveBits > 0) {
        csId = csFabricId & ((1 << numChanInterleaveBits) - 1);
      }

      if (numCddInterleaveBits > 0) {
        csId |= (((csFabricId & dieIdMask) >> dieIdShift) << numChanInterleaveBits);
      }

      if (numSocketInterleaveBits > 0) {
        csId |= (((csFabricId & socketIdMask) >> socketIdShift) << (numChanInterleaveBits + numCddInterleaveBits));
      }

      // IDS_HDT_CONSOLE (MAIN_FLOW, "csId = 0x%x\n", csId);
      deIntLvAddr = InsertBits64 (preNormAddr, csId, intLvAddrBit, numInterleaveBits);
    }
  }

  // IDS_HDT_CONSOLE (MAIN_FLOW, "deIntLvAddr = 0x%016lX\n", deIntLvAddr);

  // Add Physical memory base.
  preBaseSysAddr = deIntLvAddr + dramBaseAddr;
  // IDS_HDT_CONSOLE (MAIN_FLOW, "preBaseSysAddr = 0x%016lX\n", preBaseSysAddr);

  // Account for the DRAM hole
  if (holeEn && (preBaseSysAddr >= dramHoleBase)) {
    // IDS_HDT_CONSOLE (MAIN_FLOW, "dramHoleBase = 0x%016lX, adjust = 0x%016lX\n", dramHoleBase, ((((UINT64) 1) << 32) - dramHoleBase));
    sysAddr = preBaseSysAddr + ((((UINT64)1) << 32) - dramHoleBase);
  } else {
    sysAddr = preBaseSysAddr;
  }

  // IDS_HDT_CONSOLE (MAIN_FLOW, "sysAddr = 0x%016lX\n", sysAddr);

  // Adjust for the hashing (if enabled)
  if (hashEnabled) {
    // deIntLvAddr[hashAddrBit] = XOR of deIntLvAddr{12, 18, 21, 30} and csId[0]
    hashedBit = getBit (12, (UINT32)sysAddr) ^
                getBit (18, (UINT32)sysAddr) ^
                getBit (21, (UINT32)sysAddr) ^
                getBit (30, (UINT32)sysAddr) ^
                getBit (0, csId);
    if (hashedBit != getBit (intLvAddrBit, (UINT32)sysAddr)) {
      sysAddr ^= (((UINT64)1) << intLvAddrBit);
    }

    // IDS_HDT_CONSOLE (MAIN_FLOW, "hashedBit= %d, sysAddr = 0x%016lX\n", hashedBit, sysAddr);
  }

  if (EnDDR5 && EnDramChanHash) {
    // deIntLvAddr[hashAddrBit] = XOR of deIntLvAddr{12, 21, 30} and SubChannelId
    hashedBit = getBit (12, (UINT32)sysAddr) ^
                getBit (21, (UINT32)sysAddr) ^
                getBit (30, (UINT32)sysAddr) ^
                getBit (0, mySubChannelNum);
    if (hashedBit != getBit (Ddr5StartBit, (UINT32)sysAddr)) {
      sysAddr ^= (((UINT64)1) << Ddr5StartBit);
    }

    // IDS_HDT_CONSOLE (MAIN_FLOW, "DDR5 hashedBit= %d, sysAddr = 0x%016lX\n", hashedBit, sysAddr);
  }

  // Check that you didn't go over the pair limit
  ASSERT (sysAddr <= dramLimitAddr);
  return (sysAddr);
}

/*------------------------------------------------------------------
 Function: calcNormAddr

 Purpose: Normalize a system address into a memory controller
   (normalized) system address.

 Parameters (all are input only)
   sysAddr (ulong)
     The address to be converted.
     The user must remove all VMGuard key indices from the system address.
 Returns:
   A normalized address (ulong)

 Side Effects:
   None:

 Limitations:
   - Does not support non-power-of-2 channels (not used on SAT)

 *------------------------------------------------------------------*/
NORMALIZED_ADDRESS
calcNormAddr (
  UINT64 sysAddr
  )
{
  UINT64                                 hiAddrOffset;
  UINT64                                 dramBaseAddr;
  UINT64                                 dramLimitAddr;
  UINT64                                 dramHoleBase;
  UINT64                                 postHoleSysAddr;
  UINT64                                 preIntlvSysAddr;
  UINT64                                 intLvAddr;
  UINT32                                 systemFabricIdReg;
  UINT32                                 dramBaseReg;
  UINT32                                 dramLimitReg;
  UINT32                                 socketIdShift;
  UINT32                                 socketIdMask;
  UINT32                                 dieIdMask;
  UINT32                                 dieIdShift;
  UINT32                                 socketId, cddId;
  UINT32                                 ccmInstanceId;
  UINT32                                 csInstanceId;
  UINT32                                 csFabricId;
  UINT32                                 base;
  UINT32                                 intLvAddrSel;
  UINT32                                 intLvAddrBit;
  UINT32                                 intLvNumSockets;
  UINT32                                 intLvNumCdds;
  UINT32                                 intLvNumChan;
  UINT32                                 numInterleaveBits;
  UINT32                                 holeEn;
  UINT32                                 hiAddrOffsetEn;
  UINT32                                 addrRngVal;
  NORMALIZED_ADDRESS                     normalizedAddress;
  UINT32                                 CsConfigA2Val;
  BOOLEAN                                DramChanAddrSel;
  BOOLEAN                                EnDramChanHash;
  BOOLEAN                                EnDDR5;
  UINT32                                 Ddr5StartBit;
  UINT64                                 modBits;
  UINT64                                 modBitsDiv3;
  UINT32                                 CpuModel;
  FABRIC_DRAM_ADDR_CONFIG0_REGISTER_HYGX  DramAddrConfig0;
  FABRIC_DRAM_ADDR_CONFIG1_REGISTER_HYGX  DramAddrConfig1;

  numInterleaveBits = 0;
  socketId = 0;
  cddId    = 0;
  CpuModel = GetHygonSocModel();

  // Detect some information about the system
  systemFabricIdReg = getCddDfRegSystemFabricIdMask (0, 0);
  socketIdShift     = getBits (SOCKETIDSHIFTLO, SOCKETIDSHIFTHI, systemFabricIdReg);
  socketIdMask = getBits (SOCKETIDMASKLO, (CpuModel == HYGON_EX_CPU)?SOCKETIDMASKHI_HYEX:SOCKETIDMASKHI_HYGX, systemFabricIdReg);
  dieIdShift   = getBits (DIEIDSHIFTLO, DIEIDSHIFTHI, systemFabricIdReg);
  dieIdMask    = getBits (DIEIDMASKLO, (CpuModel == HYGON_EX_CPU)?DIEIDMASKHI_HYEX:DIEIDMASKHI_HYGX, systemFabricIdReg);

  // BOZO: Find a CCM instance ID;
  ccmInstanceId = CCM0_INSTANCE_ID;

  // Find the channel...
  csFabricId = checkDramHit (sysAddr, ccmInstanceId);
  // IDS_HDT_CONSOLE (MAIN_FLOW, "csFabricId = %08x\n", csFabricId);

  // convert the dstFabricId to a CS instance ID ,socketId and cddId.
  socketId     = (csFabricId & socketIdMask) >> socketIdShift;
  cddId        = ((csFabricId & dieIdMask) >> dieIdShift) - FABRIC_ID_CDD0_DIE_NUM;
  csInstanceId = csFabricId & ~(socketIdMask | dieIdMask);
  
  if (CpuModel == HYGON_GX_CPU) {
    DramAddrConfig0.Value = FabricRegisterAccRead (socketId, cddId, DRAMADDRCONFIG0_FUNC, DRAMADDRCONFIG0_REG_HYGX, IOMS0_INSTANCE_ID);
    DramAddrConfig1.Value = FabricRegisterAccRead (socketId, cddId, DRAMADDRCONFIG1_FUNC, DRAMADDRCONFIG1_REG_HYGX, IOMS0_INSTANCE_ID);
  }

  // Check if it hits on DRAM mapping register 0 or 1 in the CS.
  dramBaseReg  = getDfRegDramBase (socketId, cddId, 1, csInstanceId);  // Mapping register 1
  dramBaseAddr = getBits (DRAMBASEADDRLO, DRAMBASEADDRHI, dramBaseReg);
  dramBaseAddr = dramBaseAddr << 28;
  addrRngVal   = getBit (ADDRRNGVAL, dramBaseReg);
  if (addrRngVal && (sysAddr >= dramBaseAddr)) {
    base = 1;
  } else {
    base = 0;
  }

  // Now set up variables from the CS base/limit registers
  dramBaseReg  = getDfRegDramBase (socketId, cddId, base, csInstanceId);
  dramLimitReg = getDfRegDramLimit (socketId, cddId, base, csInstanceId);

  addrRngVal = getBit (ADDRRNGVAL, dramBaseReg);
  ASSERT (addrRngVal == 1);
  dramBaseAddr = getBits (DRAMBASEADDRLO, DRAMBASEADDRHI, dramBaseReg);
  dramBaseAddr = dramBaseAddr << 28;

  dramLimitAddr  = getBits (DRAMLIMITADDRLO, DRAMLIMITADDRHI, dramLimitReg);
  dramLimitAddr  = dramLimitAddr << 28;
  dramLimitAddr |= ((UINT64)0x0FFFFFFF);

  holeEn = getBit (LGCYMMIOHOLEEN, dramBaseReg);
  dramHoleBase = getBits (DRAMHOLEBASELO, DRAMHOLEBASEHI, getDfRegDramHoleCtrl (socketId, cddId, csInstanceId));
  dramHoleBase = dramHoleBase << 24;

  intLvAddrSel = getBits (INTLVADDRSELLO, INTLVADDRSELHI, dramBaseReg);
  intLvAddrBit = (getBit (1, intLvAddrSel) ? (getBit (0, intLvAddrSel) ? 11 : 10) : (getBit (0, intLvAddrSel) ? 9 : 8));

  intLvNumChan = getBits (INTLVNUMCHANLO, INTLVNUMCHANHI, dramBaseReg);
  switch (intLvNumChan) {
    case 0:
      numInterleaveBits = 0;
      break;
    case 1:
    case 8:
      numInterleaveBits = 1;
      break;
    case 2:
    case 3:
      numInterleaveBits = 2;
      break;
    case 5:
      numInterleaveBits = 3;
      break;
    case 7:
      numInterleaveBits = 4;
      break;
      // default:
      // assert(0);
  }

  if (CpuModel == HYGON_EX_CPU) {
    intLvNumCdds = getBits(INTLVNUMDIESLO, INTLVNUMDIESHI, dramLimitReg);
  } else if (CpuModel == HYGON_GX_CPU) {
    intLvNumCdds = DramAddrConfig0.Value & 0x3; // Need check
  }
  switch (intLvNumCdds) {
    case 0:
      break;
    case 1:
      numInterleaveBits += 1;
      break;
    case 2:
      numInterleaveBits += 2;
      break;
      // default:
      // assert(0);
  }

  // numInterleaveBits += getBit(INTLVNUMSKTS, dramLimitReg);
  intLvNumSockets = getBits (INTLVNUMSKTSLO, INTLVNUMSKTSHI, dramBaseReg);
  switch (intLvNumSockets) {
    case 0:
      break;
    case 1:
      numInterleaveBits += 1;
      break;
    case 2:
      numInterleaveBits += 2;
      break;
    case 3:
      numInterleaveBits += 3;
      break;
      // default:
      // assert(0);
  }

  // Account for the DRAM hole
  if (holeEn && (sysAddr >= dramHoleBase)) {
    postHoleSysAddr = sysAddr - ((((UINT64)1) << 32) - dramHoleBase);
  } else {
    postHoleSysAddr = sysAddr;
  }

  // Subtract the base.
  preIntlvSysAddr = postHoleSysAddr - dramBaseAddr;

  // Account for intereleaving bits
  intLvAddr = preIntlvSysAddr;
  if (intLvNumChan == 2) {
    modBits = (preIntlvSysAddr >> (numInterleaveBits + intLvAddrBit)) << 2 | ((preIntlvSysAddr >> intLvAddrBit) & 0x3);
    modBitsDiv3 = modBits / 3;
    intLvAddr = getBits64(0, intLvAddrBit - 1, sysAddr) | (modBitsDiv3 << intLvAddrBit);
  } else {
    if (numInterleaveBits > 0) {
      intLvAddr = RemoveBits64 (preIntlvSysAddr, intLvAddrBit, numInterleaveBits);
    }
  }

  if (base == 0) {
    hiAddrOffset = 0;
  } else {
    hiAddrOffsetEn = getBit (HIADDROFFSETEN, getDfRegDramOffset (socketId, cddId, 1, csInstanceId));
    hiAddrOffset   = getBits (HIADDROFFSETLO, HIADDROFFSETHI, getDfRegDramOffset (socketId, cddId, 1, csInstanceId));
    hiAddrOffset   = hiAddrOffset << 28;
  }

  normalizedAddress.normalizedSocketId  = (UINT8)((csFabricId & socketIdMask) >> socketIdShift);
  normalizedAddress.normalizedCddId     = (UINT8)((csFabricId & dieIdMask) >> dieIdShift) - FABRIC_ID_CDD0_DIE_NUM;
  normalizedAddress.normalizedChannelId = (UINT8)(csFabricId & ~(socketIdMask | dieIdMask));

  CsConfigA2Val = CddFabricRegisterAccRead (normalizedAddress.normalizedSocketId, normalizedAddress.normalizedCddId, (UINTN)CS_CONFIG_A2_FUNC, (UINTN)CS_CONFIG_A2_REG, (UINTN)FABRIC_REG_ACC_BC);
  EnDDR5 = (BOOLEAN)((CsConfigA2Val & BIT19) >> 19);
  EnDramChanHash  = (BOOLEAN)((CsConfigA2Val & BIT23) >> 23);
  DramChanAddrSel = (BOOLEAN)((CsConfigA2Val & BIT24) >> 24);
  if (EnDDR5) {
    if (DramChanAddrSel) {
      Ddr5StartBit = 8;
    } else {
      Ddr5StartBit = 7;
    }

    if (EnDramChanHash) {
      // SubChannelId = XOR of deIntLvAddr{12, 21, 30} and SubChannelId
      normalizedAddress.normalizedSubChannelId = getBit (12, (UINT32)sysAddr) ^
                                                 getBit (21, (UINT32)sysAddr) ^
                                                 getBit (30, (UINT32)sysAddr) ^
                                                 getBit (Ddr5StartBit, (UINT32)sysAddr);
    } else {
      normalizedAddress.normalizedSubChannelId = getBit (Ddr5StartBit, (UINT32)sysAddr);
    }

    // IDS_HDT_CONSOLE (MAIN_FLOW, "Remove DDR5 sub channel bit\n");
    normalizedAddress.normalizedAddr = RemoveBits64 (intLvAddr, Ddr5StartBit, 1) + hiAddrOffset;
  } else {
    normalizedAddress.normalizedAddr = intLvAddr + hiAddrOffset;
    normalizedAddress.normalizedSubChannelId = 0;
  }

  // IDS_HDT_CONSOLE (MAIN_FLOW, "normalizedAddress.normalizedSocketId = %08x\n", normalizedAddress.normalizedSocketId);
  // IDS_HDT_CONSOLE (MAIN_FLOW, "normalizedAddress.normalizedCddId = %08x\n", normalizedAddress.normalizedCddId);
  // IDS_HDT_CONSOLE (MAIN_FLOW, "normalizedAddress.normalizedChannelId = %08x\n", normalizedAddress.normalizedChannelId);
  // IDS_HDT_CONSOLE (MAIN_FLOW, "normalizedAddress.normalizedSubChannelId = %08x\n", normalizedAddress.normalizedSubChannelId);
  // IDS_HDT_CONSOLE (MAIN_FLOW, "normalizedAddress.normalizedAddr = %08x\n", normalizedAddress.normalizedAddr);

  return (normalizedAddress);
}

/*------------------------------------------------------------------
 Function: InsertBits64

 Purpose: Insert some bits into SrcData

 Parameters (all are input only)
   SrcData          SourceData
   InsertData       The data to insert into
   InsertStartBit   Insert to which bit of SourceData
   InsertBitWidth   Insert Bit widths
 Returns:
   Dest data

 Side Effects:
   None:

 Limitations:
   None
 *------------------------------------------------------------------*/
UINT64
InsertBits64 (
  IN UINT64 SrcData,
  IN UINT64 InsertData,
  IN UINT64 InsertStartBit,
  IN UINT64 InsertBitWidth
  )
{
  UINT64  LowData;
  UINT64  HighData;
  UINT64  DstData;

  // Get SrcData BIT[InsertStartBit-1:0]
  LowData = getBits64 ((UINT32)0, (UINT32)(InsertStartBit -1), SrcData);

  // Get SrcData BIT[63:InsertStartBit], left shift StartBit + InsertBitWidth
  HighData = getBits64 ((UINT32)InsertStartBit, (UINT32)63, SrcData);
  HighData = HighData << (InsertStartBit + InsertBitWidth);

  // Calc DstData
  DstData = 0;
  DstData = HighData | (InsertData << InsertStartBit) | LowData;
  return DstData;
}

/*------------------------------------------------------------------
 Function: RemoveBits

 Purpose: Remove some bits from SrcData

 Parameters (all are input only)
   SrcData          SourceData
   RemoveStartBit   Remove from which bit of SourceData
   RemoveBitWidth   Remove Bit widths
 Returns:
   Dest data

 Side Effects:
   None:

 Limitations:
   None
 *------------------------------------------------------------------*/
UINT64
RemoveBits64 (
  IN UINT64 SrcData,
  IN UINT64 RemoveStartBit,
  IN UINT64 RemoveBitWidth
  )
{
  UINT64  LowData;
  UINT64  HighData;
  UINT64  DstData;

  // Get SrcData BIT[RemoveStartBit-1:0]
  LowData = getBits64 ((UINT32)0, (UINT32)(RemoveStartBit -1), SrcData);

  HighData = getBits64 ((UINT32)(RemoveStartBit + RemoveBitWidth), (UINT32)63, SrcData);
  HighData = HighData << RemoveStartBit;

  // Calc DstData
  DstData = 0;
  DstData = HighData | LowData;
  return DstData;
}
