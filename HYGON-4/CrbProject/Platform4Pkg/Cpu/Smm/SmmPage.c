
#include "CpuSmm.h"
#include <Library/BaseLib.h>

extern  CPU_SMM_PRIVATE_DATA    gCpuSmmInt;


UINT32 GetPageTableNumPages(
    IN UINT8 NumberMemoryBits
)
{
    BOOLEAN GigPageSupport;
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    UINT32 NumPages;

    AsmCpuid(0x80000001, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    GigPageSupport = !!(RegEdx & BIT26);

    //Limit number of memory bits to 39 (512 GB) if no support for GB page-tables.
    if (!GigPageSupport && NumberMemoryBits > 39) NumberMemoryBits = 39;

    if (GigPageSupport) {
      //1 page for PML4E Table.
        //4 page for for Directory. First 4GB of 2MB pages.
        //1 Page for Page-Table Entries for 1st 2MB of 4k pages.
        NumPages = 6;

        //Allocate Pages for Directory Pointer.
        NumPages += 1 << (NumberMemoryBits > 39 ? NumberMemoryBits - 39 : 0);
    } else {
      //Allocate at least 7 pages to cover Identity Mapping for 32 bits.
      //1 page for PML4E Table
      //1 page for Page-Directory Pointer
        //Pages for Directory allocated later--one for each directory.
        //1 page for Page-Table Entries for 1st 2MB. This must be 4k blocks.
        NumPages = 3;

        //Allocate pages for page directories.
        NumPages += 1 << (NumberMemoryBits - 30);
    }
    return NumPages;
}


VOID FillPageTable(
    IN UINT8  NumberMemoryBits,
    IN VOID   *PageTable
)
{
    BOOLEAN GigPageSupport;
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    UINT64 *Pml4eTable = (UINT64*)PageTable;
  UINT64 *PdpTable   = (UINT64*)((UINT8*)PageTable + 0x1000);
  UINT64 *PDir;
    UINT64 *PTableEntry;
    UINT32 NumPml4Entries;
    UINT32 NumPgDirPtrEntries;
    UINT32 NumPgDirEntries;
    UINT32 i;

    AsmCpuid(0x80000001, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    GigPageSupport = !!(RegEdx & BIT26);

    //Limit number of memory bits to 39 (512 GB) if no support for GB page-tables.
    if (!GigPageSupport && NumberMemoryBits > 39) NumberMemoryBits = 39;

  // Update Page-Map Level-4 tables.
    NumPml4Entries =  1 << (NumberMemoryBits > 39 ? NumberMemoryBits - 39 : 0);
    for (i = 0; i < NumPml4Entries; ++i) Pml4eTable[i] = BIT0 + BIT1 + (i << 12) + (UINT64)(UINTN)PdpTable;
    ZeroMem(Pml4eTable + i, 4096 - 8 * i);  //Clear unused entries.

  //Update Page Pointer Directories.
    NumPgDirPtrEntries = 1 << (NumberMemoryBits - 30);
    PDir = PdpTable + (NumPgDirPtrEntries <= 512 ? 512 : NumPgDirPtrEntries);
    if (GigPageSupport) {
        //1st 4 pages must point to a Page directory
        PdpTable[0] = BIT0 + BIT1 + (0 << 12) + (UINT64)(UINTN)PDir;
        PdpTable[1] = BIT0 + BIT1 + (1 << 12) + (UINT64)(UINTN)PDir;
        PdpTable[2] = BIT0 + BIT1 + (2 << 12) + (UINT64)(UINTN)PDir;
        PdpTable[3] = BIT0 + BIT1 + (3 << 12) + (UINT64)(UINTN)PDir;
        for (i = 4; i <  NumPgDirPtrEntries; ++i)
            PdpTable[i] = BIT0 + BIT1 + BIT7 + LShiftU64(i, 30);
    } else {
        for (i = 0; i <  NumPgDirPtrEntries; ++i)
            PdpTable[i] = BIT0 + BIT1 + (i << 12) + (UINT64)(UINTN)PDir;
    }
    if (i < 512) ZeroMem(PdpTable + i, 4096 - 8 * i);  //Clear unused entries.

  //Initialize Page Directores.
    if (GigPageSupport) NumPgDirEntries = 2048;  //First 4 GB
    else NumPgDirEntries = 1 << (NumberMemoryBits - 21); //Number of 2MB pages.
    PTableEntry = PDir + NumPgDirEntries;
    PDir[0] = BIT0 + BIT1 + (UINT64)(UINTN)PTableEntry;  //4K Page Table for first 2MB.
  for(i = 1; i < NumPgDirEntries; ++i) PDir[i] = 0x83 + LShiftU64(i, 21);

    //Initialize 4k page entries for first 2MB.
    for(i = 0; i < 512; ++i) PTableEntry[i] = BIT0 + BIT1 + (i << 12);
}



EFI_STATUS
EFIAPI
SmmPageTableInit(
  VOID
)
{
    UINT32  NumPages;
    UINT8   NumAddrLines = (UINT8)gEfiCpuHob->SizeOfMemorySpace;
    VOID*   SmmPageTableStart;

    // !TODO - may not be enough SMM_PAGING_MAX_ADDRESS_LINES
    if (NumAddrLines > SMM_PAGING_MAX_ADDRESS_LINES) NumAddrLines = SMM_PAGING_MAX_ADDRESS_LINES;

    NumPages = GetPageTableNumPages(NumAddrLines);
    SmmPageTableStart = AllocatePages(NumPages);
    if(!SmmPageTableStart)  return EFI_OUT_OF_RESOURCES;

    FillPageTable(NumAddrLines, SmmPageTableStart);
    gCpuSmmInt.SmmPageTableStart = SmmPageTableStart;
    return  EFI_SUCCESS;
}


VOID
EFIAPI
GetPageDirEntry(
  IN LMODE_2MPAGE_TRANSL  *ReqPhysAddr,
  IN OUT PAGE_MAP_AND_DIRECTORY_POINTER  **PageDirEntry
)
{
    UINT64                          *TablePtr;
    PAGE_MAP_AND_DIRECTORY_POINTER  *Pml4Entry;
    PAGE_MAP_AND_DIRECTORY_POINTER  *PdpEntry;

    // Get PML4 address
    TablePtr = (UINT64*)(AsmReadCr3() & (UINT64)~0xfff);

    Pml4Entry = (PAGE_MAP_AND_DIRECTORY_POINTER*)(TablePtr + ReqPhysAddr->Bits.PML4Offset * 1);
    // Get PDP entry
    TablePtr = (UINT64*)(Pml4Entry->Bits.PageTableBaseAddress << 12);
    PdpEntry = (PAGE_MAP_AND_DIRECTORY_POINTER*)(TablePtr + ReqPhysAddr->Bits.PDPOffset * 1);
    // Get PD entry
    TablePtr = (UINT64*)(PdpEntry->Bits.PageTableBaseAddress << 12);
    *PageDirEntry = (PAGE_MAP_AND_DIRECTORY_POINTER*)(TablePtr + ReqPhysAddr->Bits.PageDirOffset * 1);

}


VOID
EFIAPI
GlobEnableMmioMap(
  IN    PAGE_STATE  PageState
)
{
    LMODE_2MPAGE_TRANSL             ReqPhysAddr;
    PAGE_MAP_AND_DIRECTORY_POINTER  *PageDirEntry;
    UINT64                          PageStart;
//#### EIP 143623
    UINT64                          PcieBase;

    PcieBase = AsmReadMsr64(0xC0010058);
    if((PcieBase & 1)== 1){
        PcieBase = PcieBase & (UINT64)0xFFF00000;
    }else{
        PcieBase = (UINT64)(PcdGet64 (PcdPciExpressBaseAddress));
    }
/*
    ReqPhysAddr.Uint64 = (UINT64)SB_MMIO_BASE;           // FCH MMIO

    GetPageDirEntry(&ReqPhysAddr, &PageDirEntry);
    // Set page state
    PageDirEntry->Bits.Present = PageState;

    // ROM region
    for(PageStart = ROM_START_ADDR; PageStart < (UINT64)ADDR_4G;){
*/
    for(PageStart = PcieBase; PageStart < (UINT64)ADDR_4G;){
        ReqPhysAddr.Uint64 = PageStart;
        GetPageDirEntry(&ReqPhysAddr, &PageDirEntry);
        // Set page state
        PageDirEntry->Bits.Present = PageState;
        PageStart += (UINT64)MMIO_PAGE_SIZE;
    }
}


VOID
EFIAPI
DisableMmioMap(
    VOID
)
{
    LMODE_2MPAGE_TRANSL             ReqPhysAddr;
    PAGE_MAP_AND_DIRECTORY_POINTER  *PageDirEntry;

    ReqPhysAddr.Uint64 = (UINT64)AsmReadCr2();

    GetPageDirEntry(&ReqPhysAddr, &PageDirEntry);
    // Disable page
    PageDirEntry->Bits.Present = DisablePage;
}



VOID
EFIAPI
EnableMmioMap(
    VOID
)
{
    LMODE_2MPAGE_TRANSL             ReqPhysAddr;
    PAGE_MAP_AND_DIRECTORY_POINTER  *PageDirEntry;

    ReqPhysAddr.Uint64 = (UINT64)AsmReadCr2();

    GetPageDirEntry(&ReqPhysAddr, &PageDirEntry);
    // Enable page
    PageDirEntry->Bits.Present = EnablePage;
}

