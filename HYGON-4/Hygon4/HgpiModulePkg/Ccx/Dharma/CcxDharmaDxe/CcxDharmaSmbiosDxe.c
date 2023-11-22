/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPU SMBIOS functions.
 *
 * Contains code for collecting SMBIOS information
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Ccx
 *
 */
/*
 ****************************************************************************
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
#include "Porting.h"
#include "HYGON.h"
#include <HygonFuse.h>
#include <CddRegistersDm.h>
#include "Filecode.h"
#include "cpuRegisters.h"
#include <GnbRegisters.h>
#include <Library/BaseLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/CcxBaseX86Lib.h>
#include <Library/CcxSmbiosLib.h>
#include <Library/CcxPstatesLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Protocol/HygonNbioSmuServicesProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/HygonCoreTopologyProtocol.h>
#include <Protocol/HygonSmbiosServicesProtocol.h>
#include <Library/HygonSmnAddressLib.h>

#define FILECODE  CCX_DHARMA_CCXDHARMADXE_CCXDHARMASMBIOSDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
DharmaGetCoreDmiInfo (
  IN       HYGON_CCX_SMBIOS_SERVICES_PROTOCOL       *This,
  IN       UINTN                                   Socket,
  OUT      HYGON_CCX_CORE_DMI_INFO                  *CoreDmiInfo
  );

EFI_STATUS
EFIAPI
DharmaGetCacheDmiInfo (
  IN       HYGON_CCX_SMBIOS_SERVICES_PROTOCOL       *This,
  IN       UINTN                                   Socket,
  OUT      HYGON_CACHE_DMI_INFO                     *CacheDmiInfo
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
STATIC HYGON_CCX_SMBIOS_SERVICES_PROTOCOL  mDharmaDmiServicesProtocol = {
  SMBIOS_3_1_1,      // Support SMBIOS 3.1.1
  DharmaGetCoreDmiInfo,
  DharmaGetCacheDmiInfo
};

CHAR8 ROMDATA  str_ProcManufacturer[] = "Chengdu Hygon";

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
CcxDharmaSmbiosServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  // Install SMBIOS services protocol
  return gBS->InstallProtocolInterface (
                &ImageHandle,
                &gHygonCcxSmbiosServicesProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mDharmaDmiServicesProtocol
                );
}


// byo230920 + >>
VOID ByoTrimCpuNameStr(CHAR8 *Name)
{
  UINTN    StringLength;
  BOOLEAN  HasLeading;
  UINTN    SrcIndex;
  UINTN    TarIndex;
  CHAR8    Buffer[PROC_VERSION_LENGTH+1];
  
  StringLength = AsciiStrLen(Name);
  if(StringLength == 0){
    return; 
  }

  HasLeading = TRUE;
  TarIndex   = 0;
  for(SrcIndex=0;SrcIndex<StringLength;SrcIndex++){
    if(HasLeading){
      if(Name[SrcIndex] == ' '){
        continue;
      }else{
        HasLeading = FALSE;
      }
    }
    if(TarIndex>0){
      if((Name[SrcIndex-1] == Name[SrcIndex]) && (Name[SrcIndex] == ' ')){
        continue;
      }
    }
    Buffer[TarIndex++] = Name[SrcIndex];
  }

  if(TarIndex && Buffer[TarIndex-1] == ' '){
    TarIndex--;
  }
  Buffer[TarIndex] = 0;

  TarIndex++;
  for(SrcIndex=0; SrcIndex<TarIndex; SrcIndex++){
    Name[SrcIndex] = Buffer[SrcIndex];
  }
}
// byo230920 + <<



/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/**
 *  This service retrieves DMI information about the core.
 *
 * @param[in]  This                                 A pointer to the
 *                                                  HYGON_CCX_SMBIOS_SERVICES_PROTOCOL instance.
 * @param[in]  Socket                               Zero-based socket number to check.
 * @param[out] CoreDmiInfo                          Contains core DMI information
 *
 * @retval EFI_SUCCESS                              The core DMI information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER                    CoreDmiInfo is NULL.
 *
 **/
EFI_STATUS
EFIAPI
DharmaGetCoreDmiInfo (
  IN       HYGON_CCX_SMBIOS_SERVICES_PROTOCOL       *This,
  IN       UINTN                                   Socket,
  OUT      HYGON_CCX_CORE_DMI_INFO                  *CoreDmiInfo
  )
{
  UINT8                                    ByteIndexInUint64;
  UINT16                                   Index;
  UINT32                                   Fmax;
  UINT64                                   MsrData;
  UINTN                                    Frequency;
  UINTN                                    VoltageInuV;
  UINTN                                    PowerInmW;
  CPUID_DATA                               CpuId;
  HYGON_CONFIG_PARAMS                      StdHeader;
  EFI_STATUS                               Status;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL     *NbioSmuServices;
  HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL    *CoreTopology;
  UINTN                                    CddNumber, CddIndex, CddsPresent;
  UINTN                                    NumberOfComplexesPerCdd;
  UINTN                                    NumberOfCoresPerComplex;
  UINTN                                    NumberOfThreadsPerCore;
  UINTN                                    CoreCount, ThreadCount;
  UINTN                                    CpuModel;
  UINT8                                    PkgType;

  NbioSmuServices = NULL;
  FabricTopology  = NULL;
  Status = gBS->LocateProtocol (
                  &gHygonNbioSmuServicesProtocolGuid,
                  NULL,
                  &NbioSmuServices
                  );
  ASSERT (!EFI_ERROR (Status));

  Status = gBS->LocateProtocol (
                  &gHygonCoreTopologyServicesProtocolGuid,
                  NULL,
                  &CoreTopology
                  );
  ASSERT (!EFI_ERROR (Status));

  Status = gBS->LocateProtocol (
                  &gHygonFabricTopologyServicesProtocolGuid,
                  NULL,
                  &FabricTopology
                  );
  ASSERT (!EFI_ERROR (Status));

  // Type 4 Offset 0x05, Processor Type
  CoreDmiInfo->ProcessorType = CENTRAL_PROCESSOR;

  // Type 4 Offset 0x06, Processor Family
  CoreDmiInfo->ProcessorFamily = CcxGetProcessorFamilyForSmbios (0, &StdHeader);

  // Type4 Offset 0x08, Processor ID
  AsmCpuid (
    HYGON_CPUID_APICID_LPC_BID,
    &(CpuId.EAX_Reg),
    &(CpuId.EBX_Reg),
    &(CpuId.ECX_Reg),
    &(CpuId.EDX_Reg)
    );
  CoreDmiInfo->ProcessorID.EaxFamilyId  = CpuId.EAX_Reg;
  CoreDmiInfo->ProcessorID.EdxFeatureId = CpuId.EDX_Reg;

  // Type4 Offset 0x11/0x16, Voltage/Current Speed
  CcxGetPstateInfo (0, SwPstate0, &Frequency, &VoltageInuV, &PowerInmW, &StdHeader);
  CoreDmiInfo->Voltage = (UINT8)((VoltageInuV + 50000) / 100000 + 0x80);  // Voltage = 0x80 + Voltage * 10
  CoreDmiInfo->CurrentSpeed = (UINT16)Frequency;

  // Type4 Offset 0x14, Max Speed
  CoreCount   = 0;
  ThreadCount = 0;
  if (NbioSmuServices == NULL) {
    CoreDmiInfo->MaxSpeed = CoreDmiInfo->CurrentSpeed;
  } else {
    if (Socket > 0) {
      FabricTopology->GetCddInfo (FabricTopology, Socket, &CddNumber, &CddsPresent);
      for (CddIndex = 0; CddIndex < MAX_CDDS_PER_SOCKET; CddIndex++) {
        if (!IS_CDD_PRESENT (CddIndex, CddsPresent)) {
          continue;
        }

        CoreTopology->GetCoreTopologyOnCdd (
                        CoreTopology,
                        Socket,
                        CddIndex,
                        &NumberOfComplexesPerCdd,
                        &NumberOfCoresPerComplex,
                        &NumberOfThreadsPerCore
                        );
        CoreCount   += NumberOfComplexesPerCdd * NumberOfCoresPerComplex;
        ThreadCount += NumberOfComplexesPerCdd * NumberOfCoresPerComplex * NumberOfThreadsPerCore;
      }
    }

    CpuModel = GetHygonSocModel ();
    Fmax     = 0;
    if (CpuModel == HYGON_EX_CPU) {
      NbioSmuServices->HygonSmuRegisterRead (NbioSmuServices, (UINT32)Socket, IOD_MP1_FMAX_HYEX, &Fmax);
      Fmax = (Fmax >> IOD_MP1_FMAX_START_BITS_HYEX) & 0xFF;
    } else if (CpuModel == HYGON_GX_CPU) {
      PkgType  = GetSocPkgType();
      if (PkgType == DM1_102) {
        Fmax = (UINT32)(CoreDmiInfo->CurrentSpeed / 25);
      } else {
        NbioSmuServices->HygonSmuRegisterRead (NbioSmuServices, (UINT32)Socket, IOD_SPACE (0, IOD_MP1_FMAX_HYGX), &Fmax);
        IDS_HDT_CONSOLE (CPU_TRACE, "  Frequency Fmax = 0x%X \n", Fmax );
        Fmax = (Fmax >> IOD_MP1_FMAX_START_BITS_HYGX) & 0xFF;
      }
    }

    CoreDmiInfo->MaxSpeed = (UINT16)(Fmax * 25);
  }

  // Type4 Offset 0x12, External Clock
  CoreDmiInfo->ExternalClock = EXTERNAL_CLOCK_100MHZ;

  // Type4 Offset 0x18, Status
  CoreDmiInfo->Status = SOCKET_POPULATED | CPU_STATUS_ENABLED;

  // Type4 Offset 0x19, Processor Upgrade
  CoreDmiInfo->ProcessorUpgrade = CcxGetProcessorUpgradeForSmbios (0, &StdHeader);

  // Type4 Offset 0x23/0x25, Core Count/Thread Count
  AsmCpuid (
    HYGON_CPUID_ASIZE_PCCOUNT,
    &(CpuId.EAX_Reg),
    &(CpuId.EBX_Reg),
    &(CpuId.ECX_Reg),
    &(CpuId.EDX_Reg)
    );

  if (Socket > 0) {
    // Get Core Count/Thread Count from CoreTopology protocol
    CoreDmiInfo->ThreadCount = (UINT16)ThreadCount;
    CoreDmiInfo->CoreCount   = (UINT16)CoreCount;
  } else {
    // Get Core Count/Thread Count from registers
    CoreDmiInfo->ThreadCount = (UINT16)((CpuId.ECX_Reg & 0xFF) + 1);    // bit 7:0
    CoreDmiInfo->CoreCount   = CoreDmiInfo->ThreadCount / CcxGetThreadsPerCore ();
  }

  // Type4 Offset 0x24 Core Enabled
  CoreDmiInfo->CoreEnabled = CoreDmiInfo->CoreCount;

  // Type4 Offset 0x26, Processor Characteristics
  CoreDmiInfo->ProcessorCharacteristics = P_CHARACTERISTICS_DHARMA;

  // Type4 ProcessorVersion
  for (Index = 0; Index <= 5; Index++) {
    MsrData = AsmReadMsr64 ((MSR_CPUID_NAME_STRING0 + Index));
    for (ByteIndexInUint64 = 0; ByteIndexInUint64 <= 7; ByteIndexInUint64++) {
      CoreDmiInfo->ProcessorVersion[Index * 8 + ByteIndexInUint64] = (UINT8)RShiftU64 (MsrData, (8 * ByteIndexInUint64));
    }
  }
  CoreDmiInfo->ProcessorVersion[PROC_VERSION_LENGTH] = 0;         // byo230920 +
  ByoTrimCpuNameStr(CoreDmiInfo->ProcessorVersion);               // byo230920 +

  // Type4 ProcessorManufacturer
  ASSERT (PROC_MANU_LENGTH >= sizeof (str_ProcManufacturer));
  LibHygonMemCopy (CoreDmiInfo->ProcessorManufacturer, str_ProcManufacturer, sizeof (str_ProcManufacturer), &StdHeader);

  return EFI_SUCCESS;
}

/**
 * This service retrieves information about the cache.
 *
 * @param[in]  This                                 A pointer to the
 *                                                  HYGON_CCX_SMBIOS_SERVICES_PROTOCOL instance.
 * @param[in]  Socket                               Zero-based socket number to check.
 * @param[out] CacheDmiInfo                         Contains cache DMI information
 *
 * @retval EFI_SUCCESS                              The cache DMI information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER                    CacheDmiInfo is NULL.
 *
 **/
EFI_STATUS
EFIAPI
DharmaGetCacheDmiInfo (
  IN       HYGON_CCX_SMBIOS_SERVICES_PROTOCOL       *This,
  IN       UINTN                                   Socket,
  OUT      HYGON_CACHE_DMI_INFO                     *CacheDmiInfo
  )
{
  GET_CACHE_INFO       CacheInfo;
  HYGON_CONFIG_PARAMS  StdHeader;

  CcxGetCacheInfo (0, &CacheInfo, &StdHeader);

  // Type7 Offset 0x05, Cache Configuration
  CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].CacheCfg = CACHE_CFG_L1;
  CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].CacheCfg = CACHE_CFG_L2;
  CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].CacheCfg = CACHE_CFG_L3;

  // Type7 Offset 0x07 and 09, Maximum Cache Size and Installed Size

  // Maximum L1 cache size
  if (CacheInfo.CacheEachLevelInfo[CpuL1Cache].CacheSize > 0xFFFF) {
    CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].MaxCacheSize  = 0xFFFF;
    CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].MaxCacheSize2 = CacheInfo.CacheEachLevelInfo[CpuL1Cache].CacheSize;
  } else {
    CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].MaxCacheSize  = (UINT16)CacheInfo.CacheEachLevelInfo[CpuL1Cache].CacheSize;
    CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].MaxCacheSize2 = ((CacheInfo.CacheEachLevelInfo[CpuL1Cache].CacheSize & 0x7FFF) |
                                                                  ((CacheInfo.CacheEachLevelInfo[CpuL1Cache].CacheSize & 0x8000) << 16));
  }

  // Installed L1 cache size
  CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].InstallSize  = CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].MaxCacheSize;
  CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].InstallSize2 = CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].MaxCacheSize2;

  // Maximum L2 cache size
  if (CacheInfo.CacheEachLevelInfo[CpuL2Cache].CacheSize > 0xFFFF) {
    CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].MaxCacheSize  = 0xFFFF;
    CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].MaxCacheSize2 = CacheInfo.CacheEachLevelInfo[CpuL2Cache].CacheSize;
  } else {
    CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].MaxCacheSize  = (UINT16)CacheInfo.CacheEachLevelInfo[CpuL2Cache].CacheSize;
    CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].MaxCacheSize2 = ((CacheInfo.CacheEachLevelInfo[CpuL2Cache].CacheSize & 0x7FFF) |
                                                                  ((CacheInfo.CacheEachLevelInfo[CpuL2Cache].CacheSize & 0x8000) << 16));
  }

  // Installed L2 cache size
  CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].InstallSize  = CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].MaxCacheSize;
  CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].InstallSize2 = CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].MaxCacheSize2;

  // Maximum L3 cache size
  if (CacheInfo.CacheEachLevelInfo[CpuL3Cache].CacheSize > 0xFFFF) {
    CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].MaxCacheSize  = 0xFFFF;
    CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].MaxCacheSize2 = CacheInfo.CacheEachLevelInfo[CpuL3Cache].CacheSize;
  } else {
    CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].MaxCacheSize  = (UINT16)CacheInfo.CacheEachLevelInfo[CpuL3Cache].CacheSize;
    CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].MaxCacheSize2 = ((CacheInfo.CacheEachLevelInfo[CpuL3Cache].CacheSize & 0x7FFF) |
                                                                  ((CacheInfo.CacheEachLevelInfo[CpuL3Cache].CacheSize & 0x8000) << 16));
  }

  // Installed L3 cache size
  CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].InstallSize  = CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].MaxCacheSize;
  CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].InstallSize2 = CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].MaxCacheSize2;

  // Type7 Offset 0x0B and 0D, Supported SRAM Type and Current SRAM Type
  CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].SupportedSramType = SRAM_TYPE_PIPELINE_BURST;
  CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].CurrentSramType   = SRAM_TYPE_PIPELINE_BURST;
  CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].SupportedSramType = SRAM_TYPE_PIPELINE_BURST;
  CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].CurrentSramType   = SRAM_TYPE_PIPELINE_BURST;
  CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].SupportedSramType = SRAM_TYPE_PIPELINE_BURST;
  CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].CurrentSramType   = SRAM_TYPE_PIPELINE_BURST;

  // Type7 Offset 0x0F, Cache Speed
  CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].CacheSpeed = 1;
  CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].CacheSpeed = 1;
  CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].CacheSpeed = 1;

  // Type7 Offset 0x10, Error Correction Type
  CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].ErrorCorrectionType = ERR_CORRECT_TYPE_MULTI_BIT_ECC;
  CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].ErrorCorrectionType = ERR_CORRECT_TYPE_MULTI_BIT_ECC;
  CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].ErrorCorrectionType = ERR_CORRECT_TYPE_MULTI_BIT_ECC;

  // Type7 Offset 0x11, System Cache Type
  CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].SystemCacheType = CACHE_TYPE_UNIFIED;
  CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].SystemCacheType = CACHE_TYPE_UNIFIED;
  CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].SystemCacheType = CACHE_TYPE_UNIFIED;

  // Type7 Offset 0x12, Associativity
  CacheDmiInfo->CacheEachLevelInfo[CpuL1Cache].Associativity = CacheInfo.CacheEachLevelInfo[CpuL1Cache].Associativity;
  CacheDmiInfo->CacheEachLevelInfo[CpuL2Cache].Associativity = CacheInfo.CacheEachLevelInfo[CpuL2Cache].Associativity;
  CacheDmiInfo->CacheEachLevelInfo[CpuL3Cache].Associativity = CacheInfo.CacheEachLevelInfo[CpuL3Cache].Associativity;

  return EFI_SUCCESS;
}
