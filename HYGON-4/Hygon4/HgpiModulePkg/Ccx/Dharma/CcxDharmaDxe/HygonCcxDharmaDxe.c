/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CCX Dharma API, and related functions.
 *
 * Contains code that initializes the core complex
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  CCX
 *
 */
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

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Protocol/MpService.h>
#include <Guid/HobList.h>
#include <Guid/EventGroup.h>
#include <Library/CcxRolesLib.h>
#include <Library/CcxBaseX86Lib.h>
#include <Library/HygonBaseLib.h>
#include <Library/CcxHaltLib.h>
#include <Library/HygonPspBaseLib.h>
#include <Library/CcxResetTablesLib.h>
#include <Library/CcxSetMcaLib.h>
#include <Library/HygonIdsHookLib.h>
#include <Library/HygonPspHpobLib.h>
#include <Library/HygonCapsuleLib.h>
#include <Library/HygonPspMboxLib.h>
#include <Library/FchBaseLib.h>
#include <Library/BaseCoreLogicalIdLib.h>
#include <Library/HygonHeapLib.h>
#include <Library/FabricWdtLib.h>
#include <Library/DxeCcxBaseX86ServicesLib.h>
#include <Library/HygonTableLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>
#include <Protocol/HygonCoreTopologyProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/HygonMpServicesPreReqProtocol.h>
#include <Protocol/HygonCcxProtocol.h>
#include <Protocol/HygonNbioSmuServicesProtocol.h>
#include <Protocol/SmmControl2.h>
#include <Protocol/HygonAcpiRasServicesProtocol.h>
#include <Protocol/SocLogicalIdProtocol.h>
#include <Library/HygonCbsVariable.h>
#include <SocLogicalId.h>
#include <Guid/HygonCbsConfig.h>
#include <CcxRegistersDm.h>
#include "HygonCcxDharmaDxe.h"
#include "CcxDharmaMicrocodePatch.h"
#include "CcxDharmaAcpiServicesDxe.h"
#include "CcxDharmaSmbiosDxe.h"
#include "CcxDharmaC6.h"
#include "CcxDharmaCpb.h"
#include "CcxDharmaPrefetch.h"
#include "cpuRegisters.h"
#include "Filecode.h"
#include <IdsNvIdSAT.h>
#include <HygonFuse.h>
#include <CddRegistersDm.h>

#define FILECODE  CCX_DHARMA_CCXDHARMADXE_HYGONCCXDHARMADXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                               D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

#define  MAX_LOGICAL_PROCESSOR_SUPPORT  (PcdGet32 (PcdCpuMaxLogicalProcessorNumber))
#define  AP_ALLOCATION_SIZE             (MAX_LOGICAL_PROCESSOR_SUPPORT * 1024)

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
EFI_EVENT         CcxDharmaInitWithMpServicesEvent;
EFI_EVENT         CcxDharmaOcCallbackEvent;
AP_MSR_REG_SETTINGS     mLateMsrSyncTable[] =
{
  { 0xC0010010, 0x0000000000000000, HYGON_REV_F18_DN_ALL },
  { 0xC0010030, 0x0000000000000000, HYGON_REV_F18_DN_ALL },
  { 0xC0010031, 0x0000000000000000, HYGON_REV_F18_DN_ALL },
  { 0xC0010032, 0x0000000000000000, HYGON_REV_F18_DN_ALL },
  { 0xC0010033, 0x0000000000000000, HYGON_REV_F18_DN_ALL },
  { 0xC0010034, 0x0000000000000000, HYGON_REV_F18_DN_ALL },
  { 0xC0010035, 0x0000000000000000, HYGON_REV_F18_DN_ALL },
  { 0xC0011004, 0x0000000000000000, HYGON_REV_F18_DN_ALL },  // byo230906 +
  { 0xC0011020, 0x0000000000000000, HYGON_REV_F18_DN_ALL },
  { 0xC0011022, 0x0000000000000000, HYGON_REV_F18_DN_ALL },
  { 0xC0011023, 0x0000000000000000, HYGON_REV_F18_DN_E0  },
  { 0xC001102B, 0x0000000000000000, HYGON_REV_F18_DN_ALL },
  { 0xC0010401, 0x0000000000000000, HYGON_REV_F18_DN_ALL }
};
VOID                  *mRegistrationForCcxDharmaInitWithMpServicesEvent;
VOID                  *mRegistrationForCcxDharmaOcCallbackEvent;
UINT64                mUcodePatchAddr = 0;
UINT64                BspPatchLevel;
UINT32                BspMsrLocation;
UINT32                ApSyncLocation;
UINT32                AllowToLaunchNextThreadLocation;
UINT8                 SleepType;
VOID                  *ApStackBasePtr;
UINTN                 ApTempBufferSize;
EFI_PHYSICAL_ADDRESS  ApStartupVector;
CCX_GDT_DESCRIPTOR    ApGdtDescriptor;
VOID                  *MemoryContentCopy;
EFI_PHYSICAL_ADDRESS  ApPageAllocation = 0;
SOC_LOGICAL_ID        mLogicalId;
BOOLEAN               mCpuWdtEn;
UINT16                mCpuWdtTimeOut;
UINT8                 IsX2ApicMode = FALSE;
AP_MTRR_SETTINGS      ApMsrSettingsList[] =
{
  { 0xC0010058,                 0x0000000000000000  },
  { HYGON_AP_MTRR_FIX64k_00000, 0x0000000000000000  },
  { HYGON_AP_MTRR_FIX16k_80000, 0x0000000000000000  },
  { HYGON_AP_MTRR_FIX16k_A0000, 0x0000000000000000  },
  { HYGON_AP_MTRR_FIX4k_C0000,  0x0000000000000000  },
  { HYGON_AP_MTRR_FIX4k_C8000,  0x0000000000000000  },
  { HYGON_AP_MTRR_FIX4k_D0000,  0x0000000000000000  },
  { HYGON_AP_MTRR_FIX4k_D8000,  0x0000000000000000  },
  { HYGON_AP_MTRR_FIX4k_E0000,  0x0000000000000000  },
  { HYGON_AP_MTRR_FIX4k_E8000,  0x0000000000000000  },
  { HYGON_AP_MTRR_FIX4k_F0000,  0x0000000000000000  },
  { HYGON_AP_MTRR_FIX4k_F8000,  0x0000000000000000  },
  { 0x00000200,                 0x0000000000000000  },
  { 0x00000201,                 0x0000000000000000  },
  { 0x00000202,                 0x0000000000000000  },
  { 0x00000203,                 0x0000000000000000  },
  { 0x00000204,                 0x0000000000000000  },
  { 0x00000205,                 0x0000000000000000  },
  { 0x00000206,                 0x0000000000000000  },
  { 0x00000207,                 0x0000000000000000  },
  { 0x00000208,                 0x0000000000000000  },
  { 0x00000209,                 0x0000000000000000  },
  { 0x0000020A,                 0x0000000000000000  },
  { 0x0000020B,                 0x0000000000000000  },
  { 0x0000020C,                 0x0000000000000000  },
  { 0x0000020D,                 0x0000000000000000  },
  { 0x0000020E,                 0x0000000000000000  },
  { 0x0000020F,                 0x0000000000000000  },
  { 0x000002FF,                 0x0000000000000000  },
  { CPU_LIST_TERMINAL                             }
};

UINT64  GdtEntries[] =
{
  0x0000000000000000, // [00h] Null descriptor
  0x00CF92000000FFFF, // [08h] Linear data segment descriptor
  0x00CF9A000000FFFF, // [10h] Linear code segment descriptor
  0x00CF92000000FFFF, // [18h] System data segment descriptor
  0x00CF9A000000FFFF, // [20h] System code segment descriptor
  0x0000000000000000, // [28h] Spare segment descriptor
  0x00CF93000000FFFF, // [30h] System data segment descriptor
  0x00AF9B000000FFFF, // [38h] System code segment descriptor
  0x0000000000000000  // [40h] Spare segment descriptor
};

UINT8  AsmNearJump[] =
{
  0x90,         // nop
  0xE9,         // near jmp
  0xFC, 0xFB
};

UINT8  ApStartupCode[] =
{
  // [000] [001] [002]
  0xBE, 0xF4, 0xFF, // mov si, 0FFF4h
  // [003]
  0x66,
  // [004] [005] [006] [007]
  0x2E, 0x0F, 0x01, 0x14, // lgdt       fword ptr cs:[si]
  // [008] [009] [010]
  0x0F, 0x20, 0xC0, // mov        eax, cr0                    ; Get control register 0
  // [011] [012] [013] [014]
  0x66, 0x83, 0xC8, 0x03, // or         eax, 000000003h             ; Set PE bit (bit #0)
  // [015] [016] [017]
  0x0F, 0x22, 0xC0, // mov        cr0, eax
  // [018] [019] [020]
  0x0F, 0x20, 0xE0, // mov        eax, cr4
  // [021] [022] [023] [024] [025] [026]
  0x66, 0x0D, 0x00, 0x06, 0x00, 0x00,   // or         eax, 00000600h
  // [027] [028] [029]
  0x0F, 0x22, 0xE0, // mov        cr4, eax
  // Protected Mode Start
  // [030] [031] [032]
  0xB8, 0x18, 0x00, // mov        ax,  18h
  // [033] [034]
  0x8E, 0xD8, // mov        ds,  ax
  // [035] [036]
  0x8E, 0xC0, // mov        es,  ax
  // [037] [038]
  0x8E, 0xE0, // mov        fs,  ax
  // [039] [040]
  0x8E, 0xE8, // mov        gs,  ax
  // [041] [042]
  0x8E, 0xD0, // mov        ss,  ax
  // [043] [044] [045]
  0x66, 0x67, 0xEA, // Far jump
  // [046] [047] [048] [049]
  0x24, 0xFC, 0x00, 0x00, // Byte [48], [49] will be replaced with segment from BIOS Directory
  // [050] [051]
  0x10, 0x00,

  // Long Mode setup
  // [052] [053] [054]
  0x0F, 0x20, 0xE0, // mov eax, cr4
  // [055] [056] [057] [058]
  0x0F, 0xBA, 0xE8, 0x05, // bts eax, 5                             ; Set PAE (bit #5)
  // [059] [060] [061]
  0x0F, 0x22, 0xE0, // mov cr4, eax
  // [062] [063] [064] [065] [066]
  0xB9, 0x80, 0x00, 0x00, 0xC0,         // mov ecx, 0C0000080h                    ; Read EFER MSR
  // [067] [068]
  0x0F, 0x32, // rdmsr
  // [069] [070] [071] [072]
  0x0F, 0xBA, 0xE8, 0x08, // bts eax, 8                             ; Set LME (bit #8)
  // [073] [074]
  0x0F, 0x30, // wrmsr

  // [075] [076] [077] [078] [079]
  0xBE, 0xE8, 0xFF, 0x00, 0x00,         // mov esi, 0000ffE8
                                        // Byte [78] [79] will be replaced with segment from BIOS Directory
  // [080] [081]
  0x8B, 0x0E, // mov ecx, [esi]
  // [082] [083] [084]
  0x0F, 0x22, 0xD9, // mov cr3, ecx                           ; Load CR3 with value from BSP

  // [085] [086] [087]
  0x0F, 0x20, 0xC0, // mov eax, cr0
  // [088] [089] [090] [091]
  0x0F, 0xBA, 0xE8, 0x1F, // bts eax, 31                            ; Set PG bit (bit #31)
  // [092] [093] [094]
  0x0F, 0x22, 0xC0, // mov cr0, eax

  // [095] [096]
  0x67, 0xEA, // Far jump to long mode
  // [097] [098] [099] [100]
  0x57, 0xFC, 0x00, 0x00, // Bytes [99] [100] will be replaced with segment from BIOS Directory
  // [101] [102]
  0x38, 0x00,
  // [103] [104]
  0x48, 0xB8, // mov rax, offset ApEntryInCOffset
  // [105] [106] [107] [108]               // Bytes [105] - [112] will be replaced with offset to APEntryInCOffset
  0xFF, 0xFF, 0xFF, 0xFF,
  // [109] [110] [111] [112]
  0xFF, 0xFF, 0xFF, 0xFF,
  // [113] [114]
  0xFF, 0xE0                            // jmp rax
};
UINT8  gPStateSettingValue = 0xFF;
#define DHARMA_CAC_WEIGHT_NUM  23
UINT64  mCacWeights[DHARMA_CAC_WEIGHT_NUM];

typedef struct {
  UINT8    *AsmNearJumpAddress;
  UINTN    ApStartupOffset;
  UINTN    Size;
} AP_ADDRESS_MAP;

VOID
ApGetStartupCodeInfo (
  OUT   AP_ADDRESS_MAP *AddressMap
  );

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
EFIAPI
CcxDharmaInitWithMpServices (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

VOID
EFIAPI
CcxDharmaOcCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

VOID
ApAsmCode (
  VOID
  );

VOID
RegSettingBeforeLaunchingNextThread (
  VOID
  );

VOID
ApEntryPointInC (
  VOID
  );

VOID
SetupApStartupRegion (
  );

VOID
RestoreResetVector (
  IN       UINT16 TotalCoresLaunched
  );

VOID
CcxDharmaSetMiscMsrs (
  IN       VOID  *Void
  );

VOID
CcxDharmaSyncMiscMsrs (
  IN       VOID  *Void
  );

VOID
CcxDharmaGetCacWeights (
  IN       DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *NbioSmuServices
  );

VOID
CcxDharmaSetCacWeights (
  );

VOID
CcxSvmDharmaLockSvm (
    CBS_CONFIG  *CbsConfig                                   // byo230831 +
  );

VOID
CcxDharmaEnableSmee (
    CBS_CONFIG  *CbsConfig                                   // byo230831 +
  );

VOID
CcxDharmaEnableWdt (
  );

VOID
CcxDharmaPrefetcher (
  );

// byo230906 + >> 
VOID
CcxDharmaAesSet (
  );
// byo230906 + <<  

VOID
CcxDharmaEnableRdseedRdrand (
  );

VOID
EFIAPI
CcxReadyToBoot (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

/**
 *
 *  Check Hygon GX SOC one core BIST result 
 *
 *  @param[in]  NbioSmuServices
 *  @param[in]  SocketId
 *              CddId
 *              CcxId 
 *              LogicalCoreId
 *  Description:
 *
 *  @retval TRUE   BIST fail
 *          FALSE  BIST pass
 *
 */
BOOLEAN
IsHyGxCoreBistFail (
  IN  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL     *NbioSmuServices,
  IN  UINT32                                    SocketId,
  IN  UINT32                                    CddId,
  IN  UINT32                                    CcxId,
  IN  UINT32                                    LogicalCoreId
)
{
  UINT32   CoreBistStatus = 0;

  ASSERT (CcxId < MAX_CCX_PER_CDD_HYGX);
  ASSERT (LogicalCoreId < MAX_CORE_PER_CCX_HYGX);

  NbioSmuServices->HygonSmuRegisterRead (NbioSmuServices, SocketId, CDD_SPACE (CddId, 0x03B105AC), &CoreBistStatus);
  if ((CoreBistStatus & (1 << (CcxId * MAX_CORE_PER_CCX_HYGX + LogicalCoreId))) != 0) {
    IDS_HDT_CONSOLE (CPU_TRACE, "Socket %d CDD %d CoreBistStatus 0x%08X, CCX %d logical core %d BIST fail \n",
                      SocketId, 
                      CddId, 
                      CoreBistStatus, 
                      CcxId, 
                      LogicalCoreId);
    return TRUE;  //BIST fail
  } else {
    return FALSE; //BIST pass
  }
}
/* -----------------------------------------------------------------------------*/

/**
 *
 *  HygonCcxDharmaDxeInit
 *
 *  @param[in]  ImageHandle     EFI Image Handle for the DXE driver
 *  @param[in]  SystemTable     Pointer to the EFI system table
 *
 *  Description:
 *    Dharma - Satori Driver Entry.  Initialize the core complex.
 *
 *  @retval EFI_STATUS
 *
 */
EFI_STATUS
EFIAPI
HygonCcxDharmaDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINT16                                   CoreNumber;
  UINTN                                    i;
  UINTN                                    j;
  UINTN                                    k;
  UINTN                                    m;
  UINTN                                    n;
  UINTN                                    LogicalThread;
  UINTN                                    NumberOfSockets;
  UINTN                                    NumberOfCdds;
  UINTN                                    CddsPresent;
  UINTN                                    NumberOfComplexes;
  UINTN                                    NumberOfCores;
  UINTN                                    NumberOfThreads;
  UINTN                                    TotalCoresLaunched;
  EFI_STATUS                               Status;
  EFI_STATUS                               CalledStatus;
  HYGON_CONFIG_PARAMS                      StdHeader;
  HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL    *CoreTopology;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  EFI_HANDLE                               Handle;
  DXE_HYGON_CCX_INIT_COMPLETE_PROTOCOL     CcxDxeInitCompleteProtocol;
  DXE_HYGON_MP_SERVICES_PREREQ_PROTOCOL    HygonMpServicesPreReqProtocol;
  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL     *NbioSmuServices;
  EFI_EVENT                                ReadyToBootEvent;
  ALLOCATE_HEAP_PARAMS                     AllocParams;
  UINT8                                    McaCount;
  PLATFORM_FEATS                           PlatformFeatures;
  UINT32                                   CpuModel;

  HGPI_TESTPOINT (TpCcxDxeEntry, NULL);

  IDS_HDT_CONSOLE (CPU_TRACE, "  HygonCcxDharmaDxeInit Entry\n");

  Status = EFI_SUCCESS;
  CpuModel = GetHygonSocModel();

  if (CcxIsBsp (&StdHeader)) {
    // Publish CCX services protocol
    CcxBaseServicesProtocolInstall (ImageHandle, SystemTable);

    gBS->LocateProtocol (&gHygonNbioSmuServicesProtocolGuid, NULL, &NbioSmuServices);

    // Get LogicalId and Package
    BaseGetLogicalIdOnExecutingCore (&mLogicalId);

    // Get PCD setting for CPU WDT
    mCpuWdtEn = PcdGetBool (PcdHygonCpuWdtEn);
    mCpuWdtTimeOut = PcdGet16 (PcdHygonCpuWdtTimeout);

    // Is X2APIC mode ?
    IsX2ApicMode = (UINT8)PcdGetBool (PcdX2ApicMode);

    // Load microcode on the BSP
    IDS_SKIP_HOOK (IDS_HOOK_CCX_SKIP_UCODE_PATCH, NULL, NULL) {
      if ( CcxDharmaMicrocodeInit (&mUcodePatchAddr, &StdHeader) == FALSE) {
        IDS_HDT_CONSOLE (CPU_TRACE, " CcxDharmaMicrocodeInit fail \n");
      }
    }

    RegSettingBeforeLaunchingNextThread ();

    // Enable watchdog timer
    CcxDharmaEnableWdt ();

    // L1, L2 HW Stream Prefetcher
    CcxDharmaPrefetcher ();

    CcxDharmaAesSet();                                       // byo230906 +

    // Get CacWeights from SMU
    CcxDharmaGetCacWeights (NbioSmuServices);

    if (HygonCapsuleGetStatus () == FALSE) {
      GetPlatformFeatures (&PlatformFeatures, &StdHeader);
      // Setup reset vector with AP Start up code
      HGPI_TESTPOINT (TpCcxDxeStartLaunchAp, NULL);
      SetupApStartupRegion ();

      // Launch APs to get the APs to a known state before we publish
      // Ccx MP Services Protocol
      Status = gBS->LocateProtocol (
                      &gHygonCoreTopologyServicesProtocolGuid,
                      NULL,
                      &CoreTopology
                      );
      Status = gBS->LocateProtocol (
                      &gHygonFabricTopologyServicesProtocolGuid,
                      NULL,
                      &FabricTopology
                      );

      if (FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL) != EFI_SUCCESS) {
        HGPI_TESTPOINT (TpCcxDxeLaunchApAbort, NULL);
        return EFI_DEVICE_ERROR;
      }

      IDS_HDT_CONSOLE (CPU_TRACE, "GetSystemInfo.NumberOfSockets=%llX\n", NumberOfSockets);
	  
      TotalCoresLaunched = 0;
      CoreNumber = 0xFFFF;
      for (i = 0; i < NumberOfSockets; i++) {
        Status = FabricTopology->GetCddInfo (FabricTopology, i, &NumberOfCdds, &CddsPresent);
        IDS_HDT_CONSOLE (CPU_TRACE, "NumberOfCdds=%llX ,CddsPresent=%llX\n", NumberOfCdds, CddsPresent);
        if (Status != EFI_SUCCESS) {
          HGPI_TESTPOINT (TpCcxDxeLaunchApAbort, NULL);
          return EFI_DEVICE_ERROR;
        }

        for (j = 0; j < MAX_CDDS_PER_SOCKET; j++) {
          if (!IS_CDD_PRESENT (j, CddsPresent)) {
            continue;
          }

          if (CoreTopology->GetCoreTopologyOnCdd (
                              CoreTopology,
                              i,
                              j,
                              &NumberOfComplexes,
                              &NumberOfCores,
                              &NumberOfThreads
                              ) != EFI_SUCCESS) {
            HGPI_TESTPOINT (TpCcxDxeLaunchApAbort, NULL);
            return EFI_DEVICE_ERROR;
          }

          for (k = 0; k < NumberOfComplexes; k++) {
            LogicalThread = 0;
            for (m = 0; m < NumberOfCores; m++) {
              if (CpuModel == HYGON_GX_CPU) {
                if (IsHyGxCoreBistFail (NbioSmuServices, (UINT32)i, (UINT32)j, (UINT32)k, (UINT32)m)) {
                  LogicalThread = LogicalThread + NumberOfThreads;
                  continue;
                }
              }
              for (n = 0; n < NumberOfThreads; n++) {
                if (!((i == 0) && (j == 0) && (k == 0) && (LogicalThread == 0))) {
                  IDS_HDT_CONSOLE (
                    CPU_TRACE,
                    "    Launch socket %X cdd %X complex %X core %X thread %X\n",
                    i,
                    j,
                    k,
                    m,
                    n
                    );
                  CoreTopology->LaunchThread (CoreTopology, i, j, k, LogicalThread);
                  TotalCoresLaunched++;
                  // Check whether the last core has completed necessory initialization before launching next thread
                  do {
                    gBS->CopyMem (
                           (VOID *)&CoreNumber,
                           (VOID *)(EFI_PHYSICAL_ADDRESS)(AllowToLaunchNextThreadLocation),
                           sizeof (CoreNumber)
                           );
                  } while (CoreNumber != TotalCoresLaunched);
                }

                LogicalThread++;
              }
            }
          }
        }
      }

      // Restore the data located at the reset vector
      RestoreResetVector ((UINT16)TotalCoresLaunched);
      HGPI_TESTPOINT (TpCcxDxeEndLaunchAp, NULL);
    }

    // Mca initialization
    // MSR_0000_0179[7:0][Count]
    McaCount = (UINT8)(AsmReadMsr64 (MSR_MCG_CAP) & 0xFF);
    IDS_HDT_CONSOLE (CPU_TRACE, "    MCA Count = %d  \n", McaCount);

    CcxSetMca ();
    IDS_HDT_CONSOLE (CPU_TRACE, "    CcxSetMca Done \n");

    // Cac Weights initialization
    HGPI_TESTPOINT (TpCcxDxeCacWeights, NULL);
    CcxDharmaSetCacWeights ();
    IDS_HDT_CONSOLE (CPU_TRACE, "    CcxDharmaSetCacWeights Done \n");

    // Install gHygonMpServicesPreReqProtocolGuid protocol
    HygonMpServicesPreReqProtocol.Revision = HYGON_MP_SERVICES_PREREQ_PROTOCOL_REVISION;
    Handle = NULL;
    CalledStatus = gBS->InstallProtocolInterface (
                          &Handle,
                          &gHygonMpServicesPreReqProtocolGuid,
                          EFI_NATIVE_INTERFACE,
                          &HygonMpServicesPreReqProtocol
                          );
    Status = (CalledStatus > Status) ? CalledStatus : Status;
    //
    // Set up call back after MP services are available.
    //
    CalledStatus = gBS->CreateEventEx (
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          CcxDharmaInitWithMpServices,
                          NULL,
                          NULL,
                          &CcxDharmaInitWithMpServicesEvent
                          );
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    CalledStatus = gBS->RegisterProtocolNotify (
                          &gEfiMpServiceProtocolGuid,
                          CcxDharmaInitWithMpServicesEvent,
                          &(mRegistrationForCcxDharmaInitWithMpServicesEvent)
                          );
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    //
    // Set up call back for OverClock.
    //
    CalledStatus = gBS->CreateEventEx (
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          CcxDharmaOcCallback,
                          NULL,
                          NULL,
                          &CcxDharmaOcCallbackEvent
                          );
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    CalledStatus = gBS->RegisterProtocolNotify (
                          &gEfiPciHostBridgeResourceAllocationProtocolGuid,
                          CcxDharmaOcCallbackEvent,
                          &(mRegistrationForCcxDharmaOcCallbackEvent)
                          );
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    CalledStatus = gBS->CreateEventEx (
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          CcxReadyToBoot,
                          NULL,
                          &gEfiEventReadyToBootGuid,
                          &ReadyToBootEvent
                          );
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    // Publish ACPI CPU SSDT services protocol
    CalledStatus = CcxDharmaAcpiCpuSsdtServicesProtocolInstall (ImageHandle, SystemTable);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    // Publish ACPI Ccx CRAT services protocol
    CalledStatus = CcxDharmaCratServicesProtocolInstall (ImageHandle, SystemTable);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    // Publish ACPI Ccx SRAT services protocol
    CalledStatus = CcxDharmaSratServicesProtocolInstall (ImageHandle, SystemTable);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    // Publish ACPI Ccx RAS services protocol
    CalledStatus = CcxDharmaRasServicesProtocolInstall (ImageHandle, SystemTable);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    // Publish SMBIOS services protocol
    CalledStatus = CcxDharmaSmbiosServicesProtocolInstall (ImageHandle, SystemTable);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    // Install gHygonCcxDxeInitCompleteProtocolGuid protocol
    CcxDxeInitCompleteProtocol.Revision = HYGON_CCX_PROTOCOL_REVISION;
    Handle = NULL;
    CalledStatus = gBS->InstallProtocolInterface (
                          &Handle,
                          &gHygonCcxDxeInitCompleteProtocolGuid,
                          EFI_NATIVE_INTERFACE,
                          &CcxDxeInitCompleteProtocol
                          );
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    // Save PcdHygonCcxCfgPFEHEnable to heap so it could be gotten in SMI handler
    AllocParams.RequestedBufferSize = sizeof (BOOLEAN);
    AllocParams.BufferHandle = HYGON_PFEH_HANDLE;
    AllocParams.Persist = HEAP_SYSTEM_MEM;

    if (HeapAllocateBuffer (&AllocParams, NULL) == HGPI_SUCCESS) {
      *((BOOLEAN *)AllocParams.BufferPtr) = PcdGetBool (PcdHygonCcxCfgPFEHEnable);
    }

    HGPI_TESTPOINT (TpCcxIdsAfterApLaunch, NULL);
    IDS_HOOK (IDS_HOOK_CCX_AFTER_AP_LAUNCH, NULL, NULL);
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "  HygonCcxDharmaDxeInit End\n");

  HGPI_TESTPOINT (TpCcxDxeExit, NULL);

  return (Status);
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  SetupApStartupRegion
 *
 *
 *  Description:
 *    This routine sets up the necessary code and data to launch APs.
 *
 */
VOID
SetupApStartupRegion (
  VOID
  )
{
  UINT8            i;
  EFI_STATUS       Status;
  IA32_DESCRIPTOR  BspGdtr;
  TYPE_ATTRIB      TypeAttrib;
  UINT64           EntryAddress;
  UINT32           EntrySize;
  UINT64           EntryDest;
  UINT32           Segment;
  UINT32           C3Value;
  UINT64           ApEntryInCOffset;

  IDS_HDT_CONSOLE (MAIN_FLOW, "SetupApStartupRegion entry \n");

  C3Value = (UINT32)AsmReadCr3 ();

  BIOSEntryInfo (BIOS_FIRMWARE, INSTANCE_IGNORED, &TypeAttrib, &EntryAddress, &EntrySize, &EntryDest);
  IDS_HDT_CONSOLE (MAIN_FLOW, "BIOS_FIRMWARE EntryAddress = 0x%x, EntrySize = 0x%x \n", EntryAddress, EntrySize);

  if (TypeAttrib.Copy == 0) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  First fetch is pointing to SPI\n");

    // AP first fetch must be below 4GB boundary
    ApPageAllocation = 0xFFFFFFFF;
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiBootServicesData,
                    EFI_SIZE_TO_PAGES (AP_ALLOCATION_SIZE),
                    &ApPageAllocation
                    );

    if (Status == EFI_SUCCESS) {
      ASSERT ((ApPageAllocation & 0xFFFFFFFF00000000) == 0);

      // Align new EntryDest on a 64KB boundary
      EntryDest  = ApPageAllocation;
      EntryDest += 0xFFFF;
      EntryDest &= ~0xFFFF;
      EntrySize  = 0x10000;

      IDS_HDT_CONSOLE (CPU_TRACE, "  AP CS Base = 0x%x\n", (UINT32)EntryDest);

      // Send EntryDest to PSP
      Status = PspMboxBiosSendApCsBase ((UINT32)EntryDest);
      ASSERT (Status == EFI_SUCCESS);
    } else {
      ApPageAllocation = 0;
      ASSERT (FALSE);
    }
  }

  Segment = ((UINT32)EntryDest + EntrySize - 0x10000);
  ApStartupVector = (EFI_PHYSICAL_ADDRESS)(((UINT32)EntryDest + EntrySize - 0x10000) + 0xFFF0);

  ApStartupCode[48] = (UINT8)((Segment >> 16) & 0xFF);
  ApStartupCode[78] = (UINT8)((Segment >> 16) & 0xFF);
  ApStartupCode[99] = (UINT8)((Segment >> 16) & 0xFF);

  ApStartupCode[49]  = (UINT8)((Segment >> 24) & 0xFF);
  ApStartupCode[79]  = (UINT8)((Segment >> 24) & 0xFF);
  ApStartupCode[100] = (UINT8)((Segment >> 24) & 0xFF);

  ApEntryInCOffset = (UINT64)&ApAsmCode;

  IDS_HDT_CONSOLE (MAIN_FLOW, "Segment =  0x%x\n", Segment);
  IDS_HDT_CONSOLE (MAIN_FLOW, "ApEntryInCOffset =  0x%x\n", ApEntryInCOffset);
  ApStartupCode[105] = (UINT8)(ApEntryInCOffset & 0xFF);
  ApStartupCode[106] = (UINT8)((ApEntryInCOffset >> 8) & 0xFF);
  ApStartupCode[107] = (UINT8)((ApEntryInCOffset >> 16) & 0xFF);
  ApStartupCode[108] = (UINT8)((ApEntryInCOffset >> 24) & 0xFF);
  ApStartupCode[109] = (UINT8)((ApEntryInCOffset >> 32) & 0xFF);
  ApStartupCode[110] = (UINT8)((ApEntryInCOffset >> 40) & 0xFF);
  ApStartupCode[111] = (UINT8)((ApEntryInCOffset >> 48) & 0xFF);
  ApStartupCode[112] = (UINT8)((ApEntryInCOffset >> 56) & 0xFF);

  // Allocate some space for APs to use as stack space
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  ((MAX_LOGICAL_PROCESSOR_SUPPORT - 1) * 0x400),             // Allocate 1K for all possible cores
                  &ApStackBasePtr
                  );
  ASSERT (!EFI_ERROR (Status));
  IDS_HDT_CONSOLE (MAIN_FLOW, "ApStackBasePtr =  0x%x\n", ApStackBasePtr);

  // Allocate space to store data at reset vector
  ApTempBufferSize = 0x410;
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  ApTempBufferSize,
                  &MemoryContentCopy
                  );
  ASSERT (!EFI_ERROR (Status));
  IDS_HDT_CONSOLE (MAIN_FLOW, "ApTempBuffer =  0x%x\n", MemoryContentCopy);

  gBS->SetMem (
         MemoryContentCopy,
         ApTempBufferSize,
         0
         );

  // Copy data at reset vector to temporary buffer so we
  // can temporarily replace it with AP start up code.
  gBS->CopyMem (
         MemoryContentCopy,
         (VOID *)(ApStartupVector - 0x400),
         ApTempBufferSize
         );

  gBS->SetMem (
         (VOID *)(ApStartupVector - 0x400),
         ApTempBufferSize,
         0
         );

  // Copy AP start up code to Segment + 0xFBF0
  gBS->CopyMem (
         (VOID *)(ApStartupVector - 0x400),
         (VOID *)&ApStartupCode,
         sizeof (ApStartupCode)
         );

  // Save BSP's patch level so that AP can use it to determine whether microcode patch
  // loading should be skipped
  BspPatchLevel = AsmReadMsr64 (0x0000008B);

  // Save sleep type so that AP needn't to get it by running FchReadSleepType (),
  // otherwise, it would lead into a race condition.
  SleepType = FchReadSleepType ();

  // Sync Fixed-MTRRs with BSP
  AsmMsrOr64 (0xC0010010, BIT19);

  for (i = 0; ApMsrSettingsList[i].MsrAddr != CPU_LIST_TERMINAL; i++) {
    ApMsrSettingsList[i].MsrData = AsmReadMsr64 (ApMsrSettingsList[i].MsrAddr);
  }

  // Some Fixed-MTRRs should be set according to PCDs
  UpdateApMtrrSettings (ApMsrSettingsList);

  AsmMsrAnd64 (0xC0010010, ~((UINT64)BIT19));

  // Copy BSP MSR values to Segment + 0xFF00
  ApSyncLocation = (UINT32)(ApStartupVector + 0xE);
  AllowToLaunchNextThreadLocation = (UINT32)(ApStartupVector + 0xC);
  BspMsrLocation = (UINT32)(ApStartupVector - 0x1C0);

  ASSERT (sizeof (ApMsrSettingsList) <= 0x1C0);
  gBS->CopyMem (
         (VOID *)((ApStartupVector - 0x1C0)),
         (VOID *)&ApMsrSettingsList,
         sizeof (ApMsrSettingsList)
         );

  // Copy GDT Entries to Segment + 0xFFA0
  gBS->CopyMem (
         (VOID *)(ApStartupVector - 0x50),
         (VOID *)&GdtEntries,
         sizeof (GdtEntries)
         );

  BspGdtr.Limit = sizeof (GdtEntries) - 1;
  BspGdtr.Base  = (UINTN)ApStartupVector - 0x50;

  // Copy pointer to GDT entries to Segment + 0xFFF4
  gBS->CopyMem (
         (VOID *)(ApStartupVector + sizeof (AsmNearJump)),
         (VOID *)&BspGdtr,
         sizeof (BspGdtr)
         );

  // Copy the near jump to AP startup code to reset vector. The near jump
  // forces execution to start from CS:FBF0
  gBS->CopyMem (
         (VOID *)ApStartupVector,
         (VOID *)AsmNearJump,
         sizeof (AsmNearJump)
         );

  // Copy the value of C3 to Segment + 0xFFE8
  gBS->CopyMem (
         (VOID *)(ApStartupVector - 0x08),
         (VOID *)&C3Value,
         sizeof (C3Value)
         );

  ApGdtDescriptor.Size    = (sizeof (GdtEntries)) - 1;
  ApGdtDescriptor.Pointer = (UINT64)&GdtEntries[0];

  AsmWbinvd ();
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  RestoreResetVector
 *
 *  @param[in] TotalCoresLaunched      The number of cores that were launched by the BSC
 *
 *  Description:
 *    This routine restores the code in the AP reset vector once all the APs that
 *    were launched are done running AP code
 *
 */
VOID
RestoreResetVector (
  IN       UINT16 TotalCoresLaunched
  )
{
  UINT16  CoreNumber;

  CoreNumber = 0xFFFF;

  // Check whether the last core has completed running the AP Startup code
  do {
    gBS->CopyMem (
           (VOID *)&CoreNumber,
           (VOID *)(EFI_PHYSICAL_ADDRESS)(ApSyncLocation),
           sizeof (CoreNumber)
           );
  } while (CoreNumber != TotalCoresLaunched);

  gBS->CopyMem (
         (VOID *)(ApStartupVector - 0x400),
         MemoryContentCopy,
         ApTempBufferSize
         );

  // Clean up memory allocations
  gBS->FreePool (MemoryContentCopy);

  if (ApPageAllocation != 0) {
    gBS->FreePages (ApPageAllocation, EFI_SIZE_TO_PAGES (AP_ALLOCATION_SIZE));
  }
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  RegSettingBeforeLaunchingNextThread
 *
 *  Description:
 *    Necessory register setting before launching next thread
 *
 */
VOID
RegSettingBeforeLaunchingNextThread (
  VOID
  )
{
  ASSERT (mLogicalId.Family != 0);
  ASSERT (mLogicalId.Revision != 0);
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  ApEntryPointInC
 *
 *  Description:
 *    This routine is the C entry point for APs and is called from ApAsmCode
 *
 */
VOID
ApEntryPointInC (
  VOID
  )
{
  HYGON_CONFIG_PARAMS  StdHeader;

  // Enable watchdog timer
  if (CcxIsComputeUnitPrimary (&StdHeader)) {
    CcxDharmaEnableWdt ();
  }

  CcxProgramTablesAtReset (SleepType, &StdHeader);

  // Skip loading microcode patch on AP if BSP's patch level is 0.
  if (BspPatchLevel != 0) {
    // Using the address saved by BSP previously
    if (mUcodePatchAddr != 0) {
      if (CcxIsComputeUnitPrimary (&StdHeader)) {
        AsmWriteMsr64 (MSR_PATCH_LOADER, mUcodePatchAddr);
      }
    }
  }

  // Mca initialization
  CcxSetMca ();

  // Cac Weights initialization
  CcxDharmaSetCacWeights ();
}

VOID
DisableRdseedRdrand (
  VOID                              *Context
  )
{
  // disable rdseed and rdrand
  AsmMsrAnd64 (MSR_CPUID_7_FEATS, ~BIT18);
  AsmMsrAnd64 (MSR_CPUID_FEATS, ~BIT62);
}

VOID
EnableRdseedRdrand (
  VOID                              *Context
  )
{
  // enable rdseed and rdrand
  AsmMsrOr64 (MSR_CPUID_7_FEATS, BIT18);
  AsmMsrOr64 (MSR_CPUID_FEATS, BIT62);
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxDharmaInitWithMpServices
 *
 *  @param[in] Event        The event that invoked this routine
 *  @param[in] Context      Unused
 *
 *  Description:
 *    This routine runs necessary routines across all APs
 *
 */
VOID
EFIAPI
CcxDharmaInitWithMpServices (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  UINTN                     i;
  EFI_STATUS                Status;
  HYGON_CONFIG_PARAMS       StdHeader;
  EFI_MP_SERVICES_PROTOCOL  *MpServices;
  EFI_PEI_HOB_POINTERS      GuidHob;                         // byo230831 +
  CBS_CONFIG                *CbsConfig;                      // byo230831 +
  

  HGPI_TESTPOINT (TpCcxDxeMpCallbackEntry, NULL);

  IDS_HDT_CONSOLE (CPU_TRACE, "  CcxDharmaInitWithMpServices Entry\n");

  gPStateSettingValue = PcdGet8(PcdHygonCpuPstateMode);                                // byo230905 +
  IDS_HDT_CONSOLE (CPU_TRACE, "gPStateSettingValue:%x\n", gPStateSettingValue);        // byo230905 +

  GuidHob.Raw = GetFirstGuidHob(&gHygonCbsVariableHobGuid);  // byo230831 +
  ASSERT(GuidHob.Raw != NULL);                               // byo230831 +
  CbsConfig = (CBS_CONFIG*)(GuidHob.Guid + 1);               // byo230831 +

  CcxDharmaInitializeC6 (&StdHeader);

  CcxDharmaInitializeCpb (&StdHeader);

  CcxDharmaInitializePrefetchMode (&StdHeader);

  // Set SVM and SVM Lock
  CcxSvmDharmaLockSvm (CbsConfig);                           // byo230831 -

  // Enable SMEE
  CcxDharmaEnableSmee (CbsConfig);                           // byo230831 -

  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    HGPI_TESTPOINT (TpCcxDxeMpCallbackAbort, NULL);
    return;
  }

  MpServices->StartupAllAPs (
                MpServices,
                CcxDharmaSetMiscMsrs,
                FALSE,
                NULL,
                0,
                NULL,
                NULL
                );
  CcxDharmaSetMiscMsrs (NULL);
  IDS_HDT_CONSOLE (CPU_TRACE, "  CcxDharmaSetMiscMsrs Done \n");

  CcxDharmaEnableRdseedRdrand ();
  
  for (i = 0; i < (sizeof (mLateMsrSyncTable) / sizeof (mLateMsrSyncTable[0])); i++) {
    //ECO chips have no WA
    if(mLogicalId.Revision & mLateMsrSyncTable[i].RevisionId){
      mLateMsrSyncTable[i].MsrData = AsmReadMsr64 (mLateMsrSyncTable[i].MsrAddr);
    }
  }

  MpServices->StartupAllAPs (
                MpServices,
                CcxDharmaSyncMiscMsrs,
                FALSE,
                NULL,
                0,
                NULL,
                NULL
                );
  IDS_HDT_CONSOLE (CPU_TRACE, "  CcxDharmaSyncMiscMsrs Done \n");

  IDS_HOOK (IDS_HOOK_CCX_AFTER_PWR_MNG, NULL, NULL);

  IDS_HDT_CONSOLE (CPU_TRACE, "  CcxDharmaInitWithMpServices Exit\n");

  HGPI_TESTPOINT (TpCcxDxeMpCallbackExit, NULL);
}

VOID
EFIAPI
CcxDharmaOcCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  IDS_HDT_CONSOLE (CPU_TRACE, "  CcxDharmaOcCallback Entry\n");

  IDS_HOOK (IDS_HOOK_CCX_CUSTOM_PSTATES, NULL, NULL);

  IDS_HDT_CONSOLE (CPU_TRACE, "  CcxDharmaOcCallback Exit\n");
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxDharmaSetMiscMsrs
 *
 *  Description:
 *    This routine sets miscellaneous MSRs:
 *    - Forces TSC recalculation
 *    - MSRC001_1023[49, TwCfgCombineCr0Cd]
 *
 */
VOID
CcxDharmaSetMiscMsrs (
  IN       VOID  *Void
  )
{
  UINT64   LocalMsrRegister;
  UINT32   EBX_Reg;
  BOOLEAN  SmtEnableStatus;
  UINT32   CpuModel;

  CpuModel = GetHygonSocModel();

// byo230905 - >>  
//  if(gPStateSettingValue == 1) {
//    AsmMsrAnd64 (0xC0010065, ~BIT63);
//    AsmMsrAnd64 (0xC0010066, ~BIT63);
//  }
// byo230905 - <<

// byo230905 + >>  
  //# 0x80/0x01 - P0 P1 P2
  //# 0x81      - P0 P1
  //# 0x82/0x00 - P0
  switch(gPStateSettingValue){
    case 1:
    case 0x80:
    default:
      break;

    case 0x81:
      AsmMsrAnd64(0xC0010066, ~BIT63);       //                 P2
      break;

    case 0:
    case 0x82:                               // PState disable
      AsmMsrAnd64(0xC0010065, ~BIT63);       // [63] PstateEn   P1
      AsmMsrAnd64(0xC0010066, ~BIT63);       //                 P2
      break;
  }  
// byo230905 + <<  

  // Force recalc of TSC on all threads after loading patch
  LocalMsrRegister = AsmReadMsr64 (0xC0010064);
  AsmWriteMsr64 (0xC0010064, LocalMsrRegister);

  if (CcxIsComputeUnitPrimary (NULL)) {
    // MSRC001_1023[49, TwCfgCombineCr0Cd] = 1
    AsmMsrOr64 (MSR_TW_CFG, BIT49);

    if(mLogicalId.Revision & HYGON_REV_F18_DN_E0){
      //WA for stream prefetch 32-KB page before ECO chips
      AsmMsrOr64 (MSR_TW_CFG, BIT24);
    }
  }

  //L1 BTB multi-hit WA
  AsmWriteMsr64 (MSR_MCA_CTL_MASK_IF, 0x2400);
  
  // DLT #4: Set MSRC001_1029 [Decode Configuration] (DE_CFG)  [Bit17]: DecfgSlowVmaskmov to 1
  LocalMsrRegister  = AsmReadMsr64 (MSR_DE_CFG);
  LocalMsrRegister |= BIT17;
  AsmWriteMsr64 (MSR_DE_CFG, LocalMsrRegister);

  // DLT #5: Set MSRC001_1021 [Instruction Cache Configuration] (IC_CFG)  [Bit12]: IcCfgDisCgNorm to 1
  LocalMsrRegister  = AsmReadMsr64 (MSR_IC_CFG);
  LocalMsrRegister |= BIT12;
  AsmWriteMsr64 (MSR_IC_CFG, LocalMsrRegister);

  // DLT #6: Set MSRC001_1029 [Decode Configuration] (DE_CFG)  [Bit1]: DeCfgSerializeLfence to 1
  LocalMsrRegister  = AsmReadMsr64 (MSR_DE_CFG);
  LocalMsrRegister |= BIT1;
  AsmWriteMsr64 (MSR_DE_CFG, LocalMsrRegister);

  // DLT #7: Set MSRC001_102D [Load-Store Configuration 2] (LS_CFG2)  [Bit34]: DisWayBlockingOnFillByp to 1
  // And [Bit61]:DisStIgnMabIdxBlk to 1
  LocalMsrRegister = AsmReadMsr64 (MSR_LS_CFG2);
  LocalMsrRegister |= (BIT34|BIT61);
  AsmWriteMsr64 (MSR_LS_CFG2, LocalMsrRegister);

  // DLT #8: Set MSRC001_1020 [Load-Store Configuration] (LS_CFG)  [Bit57]: LsCfgDisShortCommitPipe to 1
  LocalMsrRegister  = AsmReadMsr64 (MSR_LS_CFG);
  LocalMsrRegister |= BIT57;
  AsmWriteMsr64 (MSR_LS_CFG, LocalMsrRegister);

  // Set MSRC001_102C [Execution Unit Configuration] (EX_CFG)  [Bit51]: DISABLE_RESYNC_OCOK to 1
  LocalMsrRegister  = AsmReadMsr64 (MSR_EX_CFG);
  LocalMsrRegister |= BIT51;
  AsmWriteMsr64 (MSR_EX_CFG, LocalMsrRegister);

  // Set MSRC001_1000 [Microcode Control] (MCODE_CTL)  [Bit 29:26] to 6
  LocalMsrRegister  = AsmReadMsr64 (MSR_MCODE_CTL);
  LocalMsrRegister &= ~(BIT26 | BIT27 | BIT28 | BIT29);
  LocalMsrRegister |= 6 << 26;
  LocalMsrRegister |= BIT21;
  AsmWriteMsr64 (MSR_MCODE_CTL, LocalMsrRegister);

  // FP_CFG[43]:RET_DISACCF. Read-write. Reset:0. 1=Disable AccFlush
  AsmCpuid (0x8000001E, NULL, &EBX_Reg, NULL, NULL);
  SmtEnableStatus = FALSE;
  if (CpuModel == HYGON_EX_CPU) {
    SmtEnableStatus = (((EBX_Reg >> 8) & 0xFF) == 0x1) ? TRUE : FALSE;
  } else if (CpuModel == HYGON_GX_CPU) {
    SmtEnableStatus = (((EBX_Reg >> 8) & 0xFF) == 0x0) ? FALSE : TRUE;
  }
  LocalMsrRegister = AsmReadMsr64 (MSR_FP_CFG);

  // SMT enable : RET_DISACCF=1
  // SMT disable : RET_DISACCF=0
  if (SmtEnableStatus == TRUE) {
    // If SMT is enabled, set FP_CFG[43] to 1 to disable AccFlush
    LocalMsrRegister |= BIT43;
  } else {
    LocalMsrRegister &= ~BIT43;
  }

  AsmWriteMsr64 (MSR_FP_CFG, LocalMsrRegister);
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxDharmaSyncMiscMsrs
 *
 *  Description:
 *    This routine synchronizes the MSRs in mLateMsrSyncTable across all APs
 *
 */
VOID
CcxDharmaSyncMiscMsrs (
  IN       VOID  *Void
  )
{
  UINTN  i;

  for (i = 0; i < (sizeof (mLateMsrSyncTable) / sizeof (mLateMsrSyncTable[0])); i++) {
    //ECO chips have no WA
    if(mLogicalId.Revision & mLateMsrSyncTable[i].RevisionId){
       AsmWriteMsr64 (mLateMsrSyncTable[i].MsrAddr, mLateMsrSyncTable[i].MsrData);
    }
  }
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxDharmaGetCacWeights
 *
 *  @param[in] NbioSmuServices        SMU services
 *
 *  Description:
 *    This routine gets CAC weights from SMU
 *
 */
VOID
CcxDharmaGetCacWeights (
  IN       DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *NbioSmuServices
  )
{
  NbioSmuServices->HygonSmuReadCacWeights (NbioSmuServices, DHARMA_CAC_WEIGHT_NUM, mCacWeights);
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxDharmaSetCacWeights
 *
 *  Description:
 *    This routine sets all CAC weights
 *
 */
VOID
CcxDharmaSetCacWeights (
  )
{
  UINT8   WeightIndex;
  UINT64  LocalMsr;

  if (CcxIsComputeUnitPrimary (NULL)) {
    LocalMsr = AsmReadMsr64 (0xC0011074);
    AsmWriteMsr64 (0xC0011074, 0);

    for (WeightIndex = 0; WeightIndex < DHARMA_CAC_WEIGHT_NUM; WeightIndex++) {
      AsmWriteMsr64 (0xC0011076, WeightIndex);
      AsmWriteMsr64 (0xC0011077, mCacWeights[WeightIndex]);
    }

    AsmWriteMsr64 (0xC0011074, (LocalMsr | BIT63));
  }
}

UINT32  CpuWdtCountSelDecode[] =
{
  4095,
  2047,
  1023,
  511,
  255,
  127,
  63,
  31,
  8191,
  16383
};
#define NumberOfCpuWdtCountSel  (sizeof (CpuWdtCountSelDecode) / sizeof (CpuWdtCountSelDecode[0]))
#define MinCpuWdtCountSel       7
#define MaxCpuWdtCountSel       9

UINT8  CpuWdtCountSelBumpUp[] =
{
  7,
  6,
  5,
  4,
  3,
  2,
  1,
  0,
  8,
  9
};

UINT64  CpuWdtTimeBaseDecode[] =
{
  1310000,
  1280
};
#define NumberOfCpuWdtTimeBase  (sizeof (CpuWdtTimeBaseDecode) / sizeof (CpuWdtTimeBaseDecode[0]))
#define MaxCpuWdtTimeBase       0

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxDharmaEnableWdt
 *
 *  Description:
 *    This routine enables watchdog
 *
 */
VOID
CcxDharmaEnableWdt (
  )
{
  UINT8            BumpUpIndex;
  UINT16           CpuWdtTimeBase;
  UINT16           CpuWdtCountSel;
  UINT64           CpuWdtTime;
  UINT64           DfCcmWdtTime;
  CPU_WDT_CFG_MSR  CpuWdtCfg;

  if ((CcxIsBsp (NULL)) && (FabricGetCcmWdtInfo (&DfCcmWdtTime)) && (mCpuWdtEn)) {
    CpuWdtTimeBase = mCpuWdtTimeOut & 0xFF;
    CpuWdtCountSel = (mCpuWdtTimeOut & 0xFF00) >> 8;
    ASSERT (CpuWdtTimeBase < NumberOfCpuWdtTimeBase);
    ASSERT (CpuWdtCountSel < NumberOfCpuWdtCountSel);

    // RESTRICTION: When both CPU WDT & DF WDT are enable, the CPU WDT timeout must be greater than or equal to the DF CCM WDT timeout limit
    CpuWdtTime = (UINT64)(CpuWdtTimeBaseDecode[CpuWdtTimeBase] * CpuWdtCountSelDecode[CpuWdtCountSel]);
    if (DfCcmWdtTime > CpuWdtTime) {
      IDS_HDT_CONSOLE (
        CPU_TRACE,
        "  WARNING: CPU WDT (%ld.%ld S) is less than the DF CCM WDT (%ld.%ld S)\n",
        (CpuWdtTime / 1000000000),
        (CpuWdtTime % 1000000000),
        (DfCcmWdtTime / 1000000000),
        (DfCcmWdtTime % 1000000000)
        );
      IDS_HDT_CONSOLE (
        CPU_TRACE,
        "  Current CPU WDT setting: CpuWdtCountSel %X, CpuWdtTimeBase %X\n",
        CpuWdtCountSel,
        CpuWdtTimeBase
        );
      // bump the CPU WDT up to at least the value of the DF CCM WDT
      IDS_HDT_CONSOLE (CPU_TRACE, "  Try to bump up CPU WDT\n");
      mCpuWdtEn = FALSE; // Disable CPU WDT
      while ((CpuWdtCountSel != MaxCpuWdtCountSel) || (CpuWdtTimeBase != MaxCpuWdtTimeBase)) {
        if ((CpuWdtCountSel == MaxCpuWdtCountSel) && (CpuWdtTimeBase != MaxCpuWdtTimeBase)) {
          // Try to change TimeBase
          CpuWdtCountSel = MinCpuWdtCountSel;
          CpuWdtTimeBase--;
        } else {
          // Try to change CountSel
          for (BumpUpIndex = 0; BumpUpIndex < (NumberOfCpuWdtCountSel - 1); BumpUpIndex++) {
            if (CpuWdtCountSelBumpUp[BumpUpIndex] == CpuWdtCountSel) {
              CpuWdtCountSel = CpuWdtCountSelBumpUp[BumpUpIndex + 1];
              break;
            }
          }
        }

        CpuWdtTime = (UINT64)(CpuWdtTimeBaseDecode[CpuWdtTimeBase] * CpuWdtCountSelDecode[CpuWdtCountSel]);
        if (DfCcmWdtTime <= CpuWdtTime) {
          // Bump up succeed, enable CPU WDT
          mCpuWdtEn = TRUE;
          mCpuWdtTimeOut = CpuWdtTimeBase | (CpuWdtCountSel << 8);
          IDS_HDT_CONSOLE (
            CPU_TRACE,
            "  Succeed! New CPU WDT %ld.%ld S\n",
            (CpuWdtTime / 1000000000),
            (CpuWdtTime % 1000000000)
            );
          IDS_HDT_CONSOLE (
            CPU_TRACE,
            "  New CPU WDT setting: CpuWdtCountSel %X, CpuWdtTimeBase %X\n",
            CpuWdtCountSel,
            CpuWdtTimeBase
            );
          break;
        }
      }

      if (!mCpuWdtEn) {
        IDS_HDT_CONSOLE (CPU_TRACE, "  Failed, we would not enable CPU WDT\n");
      }
    }
  }

  CpuWdtCfg.Value = 0;
  if (mCpuWdtEn) {
    CpuWdtCfg.Field.CpuWdtEn = 1;
    CpuWdtCfg.Field.CpuWdtTimeBase = mCpuWdtTimeOut & 0xFF;
    CpuWdtCfg.Field.CpuWdtCountSel = (mCpuWdtTimeOut & 0xFF00) >> 8;
  }

  AsmWriteMsr64 (0xC0010074, CpuWdtCfg.Value);
}

VOID
EanbleLockSvmFeature (
  IN       VOID  *Void
  )
{
  UINT64  Msr64Data;

  Msr64Data  = AsmReadMsr64 (0xC0010114); // MSR SVM
  Msr64Data &= ~0x0010;                   // enabled
  Msr64Data |= 0x08;                      // Set SVM lock
  AsmWriteMsr64 (0xC0010114, Msr64Data);
}

VOID
DisablelockSvmFeature (
  IN       VOID  *Void
  )
{
  UINT64  Msr64Data;

  Msr64Data  = AsmReadMsr64 (0xC0010114); // MSR SVM
  Msr64Data |= 0x0010;                    // disabled
  Msr64Data |= 0x08;                      // Set SVM lock
  AsmWriteMsr64 (0xC0010114, Msr64Data);
}

/* -----------------------------------------------------------------------------*/
VOID
CcxSvmDharmaLockSvm (
    CBS_CONFIG    *CbsConfig                                 // byo230831 -
  )
{
  EFI_STATUS                Status;
  EFI_MP_SERVICES_PROTOCOL  *MpServices;
  UINT32                    FeatureFlagsEcx;

  // Check SVM (Secure Virtual Mode) feature is support ?
  AsmCpuid (0x80000001, NULL, NULL, &FeatureFlagsEcx, NULL);
  if ((FeatureFlagsEcx & BIT2) != 0) {
    IDS_HDT_CONSOLE (CPU_TRACE, "SVM feature support ! \n");

    Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
    ASSERT (!EFI_ERROR (Status));
    if (EFI_ERROR (Status)) {
      HGPI_TESTPOINT (TpCcxDxeSVMLaunchAPAbort, NULL);
      return;
    }

    if (CbsConfig->CbsCmnSVMCtrl == 0) {              // byo230831 -
      IDS_HDT_CONSOLE (CPU_TRACE, "Disable lock Svm Feature \n");
      DisablelockSvmFeature (NULL);      // disable BSP
      MpServices->StartupAllAPs (
                    MpServices,
                    DisablelockSvmFeature,
                    FALSE,
                    NULL,
                    0,
                    NULL,
                    NULL
                    );
    } else {
      IDS_HDT_CONSOLE (CPU_TRACE, "Enable lock Svm Feature \n");
      EanbleLockSvmFeature (NULL);      // Enable BSP
      MpServices->StartupAllAPs (
                    MpServices,
                    EanbleLockSvmFeature,
                    FALSE,
                    NULL,
                    0,
                    NULL,
                    NULL
                    );
    }
  } else {
    IDS_HDT_CONSOLE (CPU_TRACE, "SVM feature not support ! \n");
  }
}

VOID
EnableSmeeFeature (
  IN       VOID  *Void
  )
{
  AsmMsrOr64 (0xC0010010, BIT23);
}

VOID
DisableSmeeFeature (
  IN       VOID  *Void
  )
{
  AsmMsrAnd64 (0xC0010010, (~BIT23));
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxDharmaEnableSmee
 *
 *  Description:
 *    This routine enables secure memory encryption
 *
 */
VOID
CcxDharmaEnableSmee (
    CBS_CONFIG     *CbsConfig                         // byo230831 +
  )
{
  UINT32                    FeatureFlagsEax;
  EFI_STATUS                Status;
  EFI_MP_SERVICES_PROTOCOL  *MpServices;


  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    HGPI_TESTPOINT (TpCcxDxeSmeeLaunchAPAbort, NULL);
    return;
  }

  // Check SMEE feature is support?
  AsmCpuid (0x8000001F, &FeatureFlagsEax, NULL, NULL, NULL);
  if ((FeatureFlagsEax & BIT0) != 0) {
    IDS_HDT_CONSOLE (CPU_TRACE, "SMEE feature support ! \n");
    // MSRC001_0010[23, SMEE] = 1, dependent on CBS setting
    if ((CbsConfig->CbsCmnSmeeCtrl == 1) && (PcdGet8 (PcdRunEnvironment) == 0)) {       // byo230831 -
      IDS_HDT_CONSOLE (CPU_TRACE, "Enable SMEE feature \n");
      EnableSmeeFeature (NULL);  // Enable BSP
      MpServices->StartupAllAPs (
                    MpServices,
                    EnableSmeeFeature,
                    FALSE,
                    NULL,
                    0,
                    NULL,
                    NULL
                    );
    } else {
      IDS_HDT_CONSOLE (CPU_TRACE, "Disable SMEE feature \n");
      DisableSmeeFeature (NULL); // Disable BSP
      MpServices->StartupAllAPs (
                    MpServices,
                    DisableSmeeFeature,
                    FALSE,
                    NULL,
                    0,
                    NULL,
                    NULL
                    );
    }

  } else {
    IDS_HDT_CONSOLE (CPU_TRACE, "SMEE feature not support ! \n");
  }
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxDharmaEnableRdseedRdrand
 *
 *  Description:
 *    This routine enables Rdseed and Rdrand feature
 *
 */
VOID
CcxDharmaEnableRdseedRdrand (
  )
{
  UINT64                    IdsNvValue;
  EFI_STATUS                Status;
  EFI_MP_SERVICES_PROTOCOL  *MpServices;

  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (CPU_TRACE, "Not found gEfiMpServiceProtocolGuid, exit \n");
    return;
  }

  IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_RDSEED_RDRAND_CTRL, &IdsNvValue);
  if ((IdsNvValue == 0) || (PcdGet8 (PcdRunEnvironment) > 0)) {
    IDS_HDT_CONSOLE (CPU_TRACE, "Disable rdseed rdrand feature \n");
    MpServices->StartupAllAPs (
                  MpServices,
                  DisableRdseedRdrand,
                  FALSE,
                  NULL,
                  0,
                  NULL,
                  NULL
                  );
    DisableRdseedRdrand (NULL);
  } else {
    IDS_HDT_CONSOLE (CPU_TRACE, "Enable rdseed rdrand feature \n");
    MpServices->StartupAllAPs (
                  MpServices,
                  EnableRdseedRdrand,
                  FALSE,
                  NULL,
                  0,
                  NULL,
                  NULL
                  );
    EnableRdseedRdrand (NULL);
  }
}

VOID
CcxDharmaPrefetcher (
  )
{
  // L1 Stream HW Prefetcher
  if (!PcdGetBool (PcdHygonL1StreamPrefetcher)) {
    // MSR C001_1022[16] = 1
    AsmMsrOr64 (0xC0011022, BIT16);
  }

  // L2 Stream Prefetcher
  if (!PcdGetBool (PcdHygonL2StreamPrefetcher)) {
    // MSR C001_102B[0] = 0
    AsmMsrAnd64 (0xC001102B, ~((UINT64)BIT0));
  }
}

// byo230906 + >>
VOID
CcxDharmaAesSet (
  )
{
  UINT8           AesSupport;

  AesSupport = PcdGetBool(PcdHygonCpuAesInsSupport);
  IDS_HDT_CONSOLE (CPU_TRACE, "AesSupport %d\n", AesSupport);

  if (AesSupport) {
    AsmMsrOr64 (0xC0011004, BIT57);
  }else{
    AsmMsrAnd64 (0xC0011004, ~BIT57);
  }
}
// byo230906 + <<

/*---------------------------------------------------------------------------------------*/

/**
 * CcxReadyToBoot
 *
 * Calls CcxReadyToBoot
 *
 *  Parameters:
 *    @param[in]     Event
 *    @param[in]     *Context
 *
 *    @retval        VOID
 */
VOID
EFIAPI
CcxReadyToBoot (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                 Status;
  EFI_SMM_CONTROL2_PROTOCOL  *SmmControl;
  UINT8                      SmiDataValue;

  HGPI_TESTPOINT (TpCcxDxeRtbCallBackEntry, NULL);

  IDS_HDT_CONSOLE (CPU_TRACE, "CcxReadyToBoot Entry \n");

  Status = gBS->LocateProtocol (
                  &gEfiSmmControl2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmControl
                  );

  if (EFI_ERROR (Status)) {
    return;
  }

  SmiDataValue = PcdGet8 (PcdHygonCcxS3SaveSmi);

  SmmControl->Trigger (
                SmmControl,
                &SmiDataValue,
                NULL,
                0,
                0
                );

  gBS->CloseEvent (Event);

  IDS_HDT_CONSOLE (CPU_TRACE, "CcxReadyToBoot Exit \n");

  HGPI_TESTPOINT (TpCcxDxeRtbCallBackExit, NULL);
}
