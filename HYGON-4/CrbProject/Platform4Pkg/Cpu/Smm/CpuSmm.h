
#ifndef __CPUSMM__H__
#define __CPUSMM__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Library/BaseMemoryLib.h>
#include <Library/LocalApicLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/SmmLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/MpService.h>
#include <HygonCpu.h>
#include <Protocol/SmmConfiguration.h>
#include <Library/PlatformCommLib.h>


/*
+8000  +10000

127 * 1K = 127K = 1FC00
                  27C00  2FC00

255 * 1K = 255K = 3FC00
                  47C00  4FC00

255 * 8K = 1FE000
           +0x104000 = 302000
*/

//  Backup memory 0x38000.. 0x3FFFF used for SMM base relocation
#define BACKUP_SMM_SIZE     0x8000
#define SMM_DEFAULT_BASE    0x30000
#define SMM_HANDLER_OFFSET  0x8000
#define SMM_DEFAULT_ENTRY   (SMM_DEFAULT_BASE + SMM_HANDLER_OFFSET)
// 0x400 offset between  each SMM entry
#define SMM_ENTRY_CODE_OFFS 0x400
#define APSTACK_AREA_START  0x104000  //( > SMM_HANDLER_OFFSET + SMM_ENTRY_CODE_OFFS * MaxCores + BSP_STACK_SIZE(4000))
#define APSTACK_OFFS        0x2000    //(AP SMM Stack size)
#define SMM_PAGING_MAX_ADDRESS_LINES  39    // !TODO - limiting to 512GB for the moment

#ifndef  AMD_MSR_SMM_ADDR
    #define  AMD_MSR_SMM_ADDR    0xC0010112
#endif
#ifndef  AMD_MSR_SMM_MASK
    #define  AMD_MSR_SMM_MASK    0xC0010113
#endif
#ifndef  SB_MMIO_BASE
  #define SB_MMIO_BASE    0xFED80000
#endif
#define ROM_START_ADDR          0xFF000000  // 16M ROM at top of 4G
#define MMIO_PAGE_SIZE          0x200000    // 2M pages used for mapping MMIO
#define ADDR_4G                 0x100000000


extern  EFI_HOB_CPU *gEfiCpuHob;


extern  UINT16      gNewSmmBaseOffs;    // offset from SMM_DEFAULT_ENTRY to DD NewSmmBase
extern  UINT16      gRelocDoneOffs;     // offset from SMM_DEFAULT_ENTRY to DD RelocDone flag
extern  UINT16      gSmmRelocSize;
extern  UINT8       SmmRelocStart[];

extern  UINT16      gSmmEntrySize;
extern  UINT8       SmmEntryStart[];
extern  UINT16      gSmmStackOffs;
extern  UINT16      gProt32JmpOffs;
extern  UINT16      gLModeJmpOffs;
extern  UINT16      gPageTableOffs;
extern  UINT16      gSmmIdtBaseOffs;
extern  UINT64      gIdtStart;

#define CPU_S3_RESUME2_VARIABLE L"CpuS3Resume2"
typedef struct {
    UINT64          SmmBaseVal ;
} CPU_S3_RESUME2_Int;

typedef union {
    struct {
        UINT64  PageOffset:21;          // Offset in 2M physical page
        UINT64  PageDirOffset:9;        // 21..29 Offset in page directory table
        UINT64  PDPOffset:9;            // 30..38 offset in Page Directory Pointer table
        UINT64  PML4Offset:9;
        UINT64  SignExtend:16;
    } Bits;
    UINT64  Uint64;
} LMODE_2MPAGE_TRANSL;

//
// Page-Map Level-4 Offset (PML4) and
// Page-Directory-Pointer Offset (PDPE) entries 4K & 2MB
//

typedef union {
  struct {
    UINT64  Present:1;                // 0 = Not present in memory, 1 = Present in memory
    UINT64  ReadWrite:1;              // 0 = Read-Only, 1= Read/Write
    UINT64  UserSupervisor:1;         // 0 = Supervisor, 1=User
    UINT64  WriteThrough:1;           // 0 = Write-Back caching, 1=Write-Through caching
    UINT64  CacheDisabled:1;          // 0 = Cached, 1=Non-Cached
    UINT64  Accessed:1;               // 0 = Not accessed, 1 = Accessed (set by CPU)
    UINT64  Reserved:1;               // Reserved
    UINT64  MustBeZero:2;             // Must Be Zero
    UINT64  Available:3;              // Available for use by system software
    UINT64  PageTableBaseAddress:40;  // Page Table Base Address
    UINT64  AvabilableHigh:11;        // Available for use by system software
    UINT64  Nx:1;                     // No Execute bit
  } Bits;
  UINT64    Uint64;
} PAGE_MAP_AND_DIRECTORY_POINTER;

typedef enum{
    DisablePage = 0,
    EnablePage
} PAGE_STATE;

EFI_STATUS
EFIAPI
CpuSmmInit(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
);

EFI_STATUS
EFIAPI
SmmPageTableInit(
  VOID
);

VOID
EFIAPI
SmmHandler(
  VOID
);

VOID
EFIAPI
BspSmmHandler(
  VOID
);

VOID
EFIAPI
ApSmmHandler(
  IN UINT32   Index
);

VOID
EFIAPI
SmmRelocate(
  VOID
);

EFI_STATUS
EFIAPI
SmmStartupThisAp (
  IN      EFI_AP_PROCEDURE          Procedure,
  IN      UINTN                     CpuNumber,
  IN OUT  VOID                      *ProcArguments OPTIONAL
);

EFI_STATUS
EFIAPI
RegisterSmmEntry (
  IN CONST EFI_SMM_CONFIGURATION_PROTOCOL  *This,
  IN EFI_SMM_ENTRY_POINT                   SmmEntryPoint
);

EFI_STATUS
EFIAPI
SmmReadSaveState (
  IN CONST EFI_SMM_CPU_PROTOCOL         *This,
  IN UINTN                              Width,
  IN EFI_SMM_SAVE_STATE_REGISTER        Register,
  IN UINTN                              CpuIndex,
  OUT VOID                              *Buffer
);

EFI_STATUS
EFIAPI
SmmWriteSaveState (
  IN CONST EFI_SMM_CPU_PROTOCOL         *This,
  IN UINTN                              Width,
  IN EFI_SMM_SAVE_STATE_REGISTER        Register,
  IN UINTN                              CpuIndex,
  IN CONST VOID                         *Buffer
);

VOID
EFIAPI
FillIdt (
    VOID
);

VOID
EFIAPI
GlobEnableMmioMap(
  IN    PAGE_STATE  PageState
);

VOID
EFIAPI
DisableMmioMap(
    VOID
);

VOID
EFIAPI
EnableMmioMap(
    VOID
);


typedef struct {
    UINT32  ApicId;
    UINTN   SmmBase;
    UINTN   StackTop;
}CPU_SMM_I;

typedef struct {
    UINT32              NumCpus;
    volatile UINT32     fSmmDone;
    volatile UINT32     fNumInSmm;
    volatile UINT32     BspIndex;
    volatile BOOLEAN    InSmm[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];
    volatile BOOLEAN    InsideSmm;
    EFI_SMM_ENTRY_POINT CoreSmmEntryPoint; // SMM Foundation Entry Point
    EFI_HANDLE          CpuHandle;
    VOID                *SmmPageTableStart;
}CPU_SMM_PRIVATE_DATA;

typedef VOID (*SMM_AP_PROCEDURE) (
    IN volatile VOID *ProcArguments
);

typedef struct {
    volatile SMM_AP_PROCEDURE   Procedure;
    volatile VOID               *ProcArguments;
} SMM_AP_CPU_CONTROL;





/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
