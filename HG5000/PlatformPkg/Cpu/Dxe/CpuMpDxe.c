
#include <Uefi.h>
#include <Token.h>
#include <Amdcpu.h>
#include "CpuDxe.h"
#include <Library/MemoryAllocationLib.h>

VOID MpStart();
VOID MpRestart();
VOID SwitchStacks(UINT8, VOID *);
VOID JmpToMpStart();
// extern UINT32 JmpAddress;
extern UINTN JmpAddress;
extern UINT32 JmpToMpStartSize;
extern EFI_MP_SERVICES_PROTOCOL *gEfiMpServicesProtocol;

//This will be copied to RAM.
/**
    (DXE Only) This halt loop will be copied to Runtime. Halted
    CPUs will enter into the runtime version.

    @param VOID

    @retval VOID
**/
UINT8 HaltLoop[] =
{
//@@:
    0xfa,       //cli
    0xf4,       //hlt
    0xeb,0xfc   //jmp @b
};
VOID * CpyToRunTime(VOID *Src, UINTN Size);

typedef struct {
    VOID    (*Function)(UINT8 Cpu, VOID *Context);  //If not zero, CPU thread is executing this function.
    VOID    *Context;           //Cpu context.
    VOID    *Stack;
    UINT32  Id;
    UINT8   Halted;             //Signal to halt CPU.
    UINT8   Disabled;
} CPU_CONTROL;

typedef struct {
    UINT32  Id;
    UINT32  CpuNum;
} CPU_INFO_INIT;

typedef struct {
    UINT64  Gdt;
    UINT32  GdtPadding; // GDT64 takes 10 bytes
    UINT32  NumAps;
    UINT64  ApGlobalData;
    UINT32  PageTable;
    UINT32  TOM;
    volatile UINT32 EnteredHoldLoop;
    UINT32  EightByteAlign;     // padding for GDT, makes 8 byte align
} ZERO_DATA;

typedef struct {
    ZERO_DATA   Zdata;
    UINT64      Gdtr;
    UINT64      Sel_0;
    UINT64      Sel_8;
    UINT64      Sel_10;
    UINT64      Sel_18;
    UINT64      Sel_20;
    UINT64      Sel_28;
    UINT64      Sel_30;
    UINT64      Sel_38;
} ZERO_DATAx;

typedef struct {
    CPU_CONTROL *CpuControl;
    VOID        *HaltLoopEntry;
    VOID        *ReleaseCpu;
    UINT32      Idt;
    UINT8       CpuSync;
    UINT8       Bsp;
    UINT8       NumCpus;
    UINT8       RunningAps;
    UINT8       HaltedCpus;
} AP_GLOBAL_DATA;

#define WAIT_FOR_CPU_COUNT 400

typedef struct {
    EFI_MP_SERVICES_PROTOCOL    EfiMpServicesProtocol;
    AP_GLOBAL_DATA              *ApGlobalData;
} PRIVATE_MP_SERVICES;

EFI_STATUS EFIAPI EfiWhoAmI(
    IN EFI_MP_SERVICES_PROTOCOL *This,
    OUT UINTN   *ProcessorNumber
);


extern PRIVATE_MP_SERVICES gPrivateMpServices;
EFI_MP_SERVICES_PROTOCOL   *gEfiMpServicesProtocol = (EFI_MP_SERVICES_PROTOCOL*) &gPrivateMpServices;
extern IA32_DESCRIPTOR pIDT;

    #define ASSERT_ERROR_CPU(Status) ASSERT_EFI_ERROR(Status)
    #define ASSERT_CPU(Condition) ASSERT(Condition)
    #define STALL_CPU(time) gBS->Stall(time)
    #define ALLOCATE_MEMORY_CPU(ptr_of_Address, Size) \
        gBS->AllocatePool(EfiBootServicesData, Size, ptr_of_Address)
    #define FREE_MEMORY_CPU(ptr) \
        gBS->FreePool(ptr);
    #define ALLOCATE_PAGES_CPU(ptr_of_Address, Pages) \
        gBS->AllocatePages(AllocateAnyPages, EfiBootServicesData, Pages, ptr_of_Address)

#ifdef EFIx64
extern UINT32  CPULIB_GetPageTable(VOID);
#endif

typedef struct {
    EFI_EVENT   *WaitEvent;
    EFI_EVENT   ThisApTimerEvent;
    UINTN       TimeoutEnd;
    BOOLEAN     *Finished;
    UINT8       ApStatus;
    UINT8       CpuNum;
} THIS_AP_TRACK;

// ApStatus bits definition
#define AP_BUSY         0x01
#define AP_TMOUT_ACT    0x02
#define AP_TMOUT_EXP    0x04

//Create wrapper to interface MpServices calls with existing thread controllers.
typedef struct {
    EFI_AP_PROCEDURE    Function;
    VOID                *Parameters;
} EFI_MP_CONTEXT;

#if (NCPU > 128)
#error Max CPUs is 128 for current code
#endif

EFI_MP_CONTEXT  EfiMpContext[NCPU] = {NULL, NULL};
THIS_AP_TRACK   ThisApDb[NCPU] = {NULL, NULL,0, NULL, 0, 0};

EFI_EVENT AllApTimeoutEvent = NULL;
EFI_EVENT AllApTimerEvent = NULL;
EFI_EVENT AllApWaitEvent = NULL;
BOOLEAN   AllApTimeoutEnable = FALSE;
BOOLEAN   AllApTimeoutExpired = FALSE;
BOOLEAN   ApTerminatedTimeout = FALSE;
BOOLEAN   CancelAllApTimer = FALSE;
INTN      AllApTimeout = 0;
UINTN     **FailedCpuListPtr = NULL;
UINT64    TimeStart;
UINT64    TimeEnd;
EFI_CPU_ARCH_PROTOCOL *gCpuArchProtocol = NULL;

/**
  This function is a wrapper for CopyBspMTRRStoAllRunningApMTRRS
    for executing on APs.  This is needed because in PEI the
    EFI_MP_SERVICES_PROTOCOL is not available.

  @param Cpu - Current CPU number
  @param Context - Function Argument Structure

  @retval VOID
**/


EFI_STATUS
CopyBspMTRRStoAllRunningApMTRRS (
    IN VOID *pMTRRCopyContext );

VOID 
WrapCopyBspMTRRStoAllRunningApMTRRS(
  IN UINT8 Cpu, 
  IN VOID *Context )
{
    CopyBspMTRRStoAllRunningApMTRRS(Context);
}

/**
    Copies BSP's MTRRs to all runnint APs' MTRRs

    @param MpData (void*)AP_GLOBAL_DATA

    @retval VOID

    @note  This function is called implicitly with StartAllAps and
          RestartAp.
**/

EFI_STATUS 
MPSynchronizeMTRRs(
  IN VOID *MpData )
{
#if (0)
    MTRR_COPY_CONTEXT   MtrrCopyContext;
    EFI_STATUS          Status;
    InitMtrrCopyContext(&MtrrCopyContext);

    // Save MTRR values of the BSP to buffer
    DEBUG((EFI_D_INFO,
            "MPSynchronizeMTRRs: Copy BSP to(%X)\n",
            &MtrrCopyContext.ValueBuffer));
    Status = CopyBspMTRRStoAllRunningApMTRRS((void*)&MtrrCopyContext);

    // Write MTRR values from buffer to the APs' MTRRs
    DEBUG((EFI_D_INFO,
            "MPSynchronizeMTRRs: Write APs from(%X)\n",
            &MtrrCopyContext.ValueBuffer));

    ExecuteFunctionOnRunningCpus(MpData, FALSE, TRUE,
                                WrapCopyBspMTRRStoAllRunningApMTRRS,
                                (void*)&MtrrCopyContext);
#endif
    return EFI_SUCCESS;
}

/**
  Start one or more AP with INIT-SIPI-SIPI.

      (PEI Only) IN EFI_PEI_SERVICES      **PeiServices
      (PEI Only) IN EFI_PEI_STALL_PPI     *PeiStall
  @param ApGlobalData - pointer AP_GLOBAL_DATA structure
  @param AllCpus - Start one or all CPUs.
  @param ApicId - This Used if AllCpus == FALSE.

  @retval UINT8 Number of CPUs started.
**/

UINT8 
StartCpus(
  IN AP_GLOBAL_DATA *ApGlobalData,
  IN BOOLEAN        AllCpus,
  IN UINT8          ApicId 
  )
{
    ZERO_DATAx      ZeroDataBackUp;
    VOID            *Below1MBStartupBackup;
    UINT8           NumCpusStarted;
    EFI_STATUS      Status;
    EFI_TPL         OldTpl;
    UINT8           ExpectedAPCount = 0;
    UINT64          WaitCount;
    IA32_DESCRIPTOR MyGdtr;
    ZERO_DATAx      *ZeroDx;


    ZeroDx = (ZERO_DATAx*)(UINTN)MP_ZERO_DATA_ADDRESS;

    Status = ALLOCATE_MEMORY_CPU((VOID**)&Below1MBStartupBackup, JmpToMpStartSize);
    ASSERT_ERROR_CPU(Status);

    OldTpl = gBS->RaiseTPL(TPL_HIGH_LEVEL);

    // Back up the original contents at MP_ZERO_DATA_ADDRESS and copy ZERO_DATAx structure
    // to this address. This structure will accessed by APs.
    CopyMem(&ZeroDataBackUp, ZeroDx, sizeof(ZERO_DATAx));

    AsmReadGdtr(&MyGdtr);
    CopyMem(&ZeroDx->Zdata.Gdt, &MyGdtr, sizeof(MyGdtr));

    
    ZeroDx->Zdata.NumAps = 0; //Initialize Count.
    ZeroDx->Zdata.ApGlobalData = (UINT64)ApGlobalData;
    ZeroDx->Zdata.TOM = (UINT32)AsmReadMsr64(AMD_MSR_TOP_MEM);
#ifdef EFIx64
    ZeroDx->Zdata.PageTable = (UINT32)AsmReadCr3();
    ZeroDx->Sel_0  = 0;
    ZeroDx->Sel_8  = 0x00CF93000000FFFF; // sel 8 
    ZeroDx->Sel_10 = 0x00CF9B000000FFFF; // sel 10 32bit code
    ZeroDx->Sel_38 = 0x00AF9B000000FFFF; // sel 38 64bit code
    ZeroDx->Gdtr   = ((sizeof(ZERO_DATA) + 8 + MP_ZERO_DATA_ADDRESS)<< 16) + sizeof(UINT64)*8 - 1;	
#endif

#if ((MP_JUMP_FUNCTION_ADDRESS & ~0xFF000) != 0)
#error Token MP_JUMP_FUNCTION_ADDRESS must be 4k aligned and less than 1MB
#endif

    // Back up contents of MP_JUMP_FUNCTION_ADDRESS and copy JmpToMpStart()
    // to this address.  APs will begin execution at this address.
    CopyMem(Below1MBStartupBackup, (VOID*)MP_JUMP_FUNCTION_ADDRESS, JmpToMpStartSize);
    CopyMem((VOID*)MP_JUMP_FUNCTION_ADDRESS, JmpToMpStart, JmpToMpStartSize);

    ZeroDx->Zdata.EnteredHoldLoop = 0; //Initialize Synchronizer
                            
    *(UINTN*)((UINT32)&JmpAddress - (UINT32)JmpToMpStart + MP_JUMP_FUNCTION_ADDRESS) =
                            AllCpus ? (UINTN)MpStart : (UINTN)MpRestart;

//Send SIPI to start APs
    while((*((volatile UINT32*)(UINTN)(LOCAL_APIC_BASE | APIC_ICR_LOW_REGISTER))) & BIT12){STALL_CPU(10);} //kavi+
    *(UINT32*)(UINTN)(APIC_BASE + APIC_ICR_HIGH_REGISTER) = AllCpus ? 0 : ApicId << 24;
    *(UINT32*)(UINTN)(APIC_BASE + APIC_ICR_LOW_REGISTER)  = (AllCpus ? ALL_EXCLUDING_SELF : 0) + INIT + LEVEL_ASSERT;
    *(UINT32*)(UINTN)(APIC_BASE + APIC_ICR_LOW_REGISTER)  = (AllCpus ? ALL_EXCLUDING_SELF : 0) + LEVEL_ASSERT + SIPI + ((UINT32)MP_JUMP_FUNCTION_ADDRESS >> 12);
    STALL_CPU(200);
    *(UINT32*)(UINTN)(APIC_BASE + APIC_ICR_LOW_REGISTER)  = (AllCpus ? ALL_EXCLUDING_SELF : 0) + LEVEL_ASSERT + SIPI + ((UINT32)MP_JUMP_FUNCTION_ADDRESS >> 12);
    while((*((volatile UINT32*)(UINTN)(LOCAL_APIC_BASE | APIC_ICR_LOW_REGISTER))) & BIT12){STALL_CPU(10);} //kavi+

    if (AllCpus) {
    //        while ( ((ZERO_DATA*)MP_ZERO_DATA_ADDRESS)->NumAps < ExpectedAPCount);
        WaitCount = 0;
        while(WaitCount < 2000){
            if( ((ZERO_DATA*)MP_ZERO_DATA_ADDRESS)->EnteredHoldLoop == ((ZERO_DATA*)MP_ZERO_DATA_ADDRESS)->NumAps ) {
                break;
            }
            STALL_CPU(1);     // Wait 1 uS
            WaitCount++;
        }
    } else {
        WaitCount = 0;
        //kavi+sby
        while (((ZERO_DATA*)MP_ZERO_DATA_ADDRESS)->EnteredHoldLoop != 1) {
          CpuPause();
        }
        //kavi+e
        //kavi- while(WaitCount < 2000){
        //kavi-     if( ((ZERO_DATA*)MP_ZERO_DATA_ADDRESS)->EnteredHoldLoop == 1 ) {
        //kavi-         break;
        //kavi-     }
        //kavi-     STALL_CPU(1);     // Wait 1 uS
        //kavi-     WaitCount++;
        //kavi- }
    }


//Should not use TRACE After rising the trace level

    NumCpusStarted = AllCpus ? ((ZERO_DATA*)MP_ZERO_DATA_ADDRESS)->NumAps : 1;

    // Restore original memory contents.
    // This is important for S3 resume.
    CopyMem((VOID*)MP_ZERO_DATA_ADDRESS, &ZeroDataBackUp, sizeof(ZERO_DATAx));
    CopyMem((VOID*)MP_JUMP_FUNCTION_ADDRESS, Below1MBStartupBackup, JmpToMpStartSize);

    gBS->RestoreTPL(OldTpl);

    FREE_MEMORY_CPU(Below1MBStartupBackup);

    return NumCpusStarted;
}



VOID WaitUntilZero32 (volatile VOID*);

VOID WaitUntilZero8 (volatile VOID*);

VOID CPULib_LockByteInc (volatile UINT8* ptr );

VOID SwitchBspProgramReg(
    IN UINT8 Cpu, 
    IN volatile VOID *Context );

UINT8 
StartAllAps(
    OUT VOID    **MpData 
  )
{
    EFI_STATUS              Status;
    EFI_PHYSICAL_ADDRESS    ApStack;
    UINTN                   i;
    UINTN                   j;
    UINT8                   NumCpus;
    UINT32                  BspId;
    UINT32                  Id;
    volatile AP_GLOBAL_DATA *ApGlobalData;
    CPU_INFO_INIT           *CpuInfoInit;
    CPU_CONTROL             *CpuControl;


    Status = ALLOCATE_MEMORY_CPU((VOID**)&ApGlobalData, sizeof(AP_GLOBAL_DATA));
    ASSERT_ERROR_CPU(Status);

    ZeroMem((VOID*)ApGlobalData, sizeof(AP_GLOBAL_DATA));
    *MpData = (VOID*)ApGlobalData;

    ApGlobalData->HaltLoopEntry = CpyToRunTime(HaltLoop, sizeof(HaltLoop));
    ApGlobalData->Idt = (UINT32)&pIDT;   //Save Idt table pointer, this line moved here from below

//    DEBUG((EFI_D_INFO,"StartAllAps> Before StartCpus()\n"));
    NumCpus = StartCpus(
        (AP_GLOBAL_DATA*)ApGlobalData,
        TRUE,
        0       //Valid, if only 1 cpu
    );

    ++NumCpus;  //Count BSP
    ApGlobalData->NumCpus = NumCpus;
    DEBUG((EFI_D_INFO,"StartAllAps: NumCpus = %d.\n", NumCpus));

    //ApGlobalData->Idt = (UINT32)&pIDT;   //Save Idt table pointer
    //Ap gets control, switches to protected mode and is in a holding loop until
    //given a pointer to collect CPU info and give it a number.

    //
    //Get initial CPU Info to sort CPUs.
    //
    Status = ALLOCATE_MEMORY_CPU(&CpuInfoInit, NumCpus * sizeof(CPU_INFO_INIT));
    ASSERT_ERROR_CPU(Status);
    for (i = 0; i < NumCpus; ++i)
    {
        CpuInfoInit[i].CpuNum = 0xffffffff;
    }

    Status = ALLOCATE_MEMORY_CPU(&CpuControl, NumCpus * sizeof(CPU_CONTROL));
    ASSERT_ERROR_CPU(Status);
    ZeroMem(CpuControl, NumCpus * sizeof(CPU_CONTROL));   //Initialize

    // Allocate Stack
    // Each AP gets a page of stack. (Only allocate for APs)
    if (NumCpus > 1) {
        Status = ALLOCATE_PAGES_CPU(&ApStack, (NumCpus - 1));
        ASSERT_ERROR_CPU(Status);
    }

    ApGlobalData->CpuControl = CpuControl;

    // Release CPUs and Collect Ids.
    ApGlobalData->CpuSync = NumCpus - 1;

    BspId = MmioRead32(APIC_BASE + APIC_ID_REGISTER) >> 24;
    CpuInfoInit[0].Id = BspId;

    // Give CPU an address to collect ApicIDs.
    // This is used to sort CPUs by ApicId.
    for (i = 1; i < NumCpus; ++i)
    {
        ApGlobalData->ReleaseCpu = &CpuInfoInit[i];
        //Wait 'till AP clears number.
        WaitUntilZero32(&ApGlobalData->ReleaseCpu);
    }
    WaitUntilZero8(&ApGlobalData->CpuSync); //Wait 'till all APs are synced.

    // CPUs are held waiting for a Number and stack.

    // Sort CPUs by APIC ID. Fill in Cpu structure to give it a stack.
    // After a assigning a number, the CPU will be put in its main wait loop,
    // so it can execute functions.

    //Give lowest CPU ID, lowest CpuControl structure.
    Id = 0;
    for (i = 0; i < NumCpus; ++i)
    {
        UINTN k = 0;
        UINT32 NearestId = 0xffffffff;
        for (j = 0; j < NumCpus; ++j)
        {
            if (CpuInfoInit[j].Id == Id)
            {
                k = j;
                break;
            }
            if (CpuInfoInit[j].Id > Id && CpuInfoInit[j].Id < NearestId)
            {
                k = j;
                NearestId = CpuInfoInit[j].Id;
            }
        }
        if (j == NumCpus) Id = NearestId;

        if (k == 0)
        { //BSP
            ApGlobalData->Bsp = (UINT8)i;
            CpuControl[i].Id = BspId;
            CpuControl[i].Function = (VOID(*)(UINT8,VOID*)) - 1;    //BSP is never halted.
        } else {    //AP
            ApStack += 4096;    //Start at top of stack or stack for next AP.
            CpuControl[i].Stack = (VOID*)ApStack;   //Store stack addresses for AP.
        }
        CpuInfoInit[k].CpuNum = (UINT8)i;   //Release Cpu
        ++Id;
    }

    gPrivateMpServices.ApGlobalData = (AP_GLOBAL_DATA*)ApGlobalData;

    // Synchronize all APs' MTRRs with BSP's MTRRs
    DEBUG((EFI_D_INFO,"StartAllAps: Before MPSynchronizeMTRRs()\n"));

    MPSynchronizeMTRRs(
        *MpData);
    DEBUG((EFI_D_INFO,"StartAllAps: After MPSynchronizeMTRRs()\n"));

    //Aps are waiting in a loop, for an address to jump to, or halted.
    return NumCpus;
}

/**
  After CPU has been halted, this function restarts the CPU.
        
      (PEI Only) IN EFI_PEI_SERVICES      **PeiServices
      (PEI Only) IN EFI_PEI_STALL_PPI     *PeiStall
  @param MpData - pointer to AP_GLOBAL_DATA structure
  @param Cpu - Cpu to restart.

  @retval VOID
**/

VOID RestartAp(
  IN VOID     *MpData,
  IN UINT8    Cpu )
{
    volatile AP_GLOBAL_DATA *ApGlobalData = (volatile AP_GLOBAL_DATA *)MpData;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    UINT8   NumberCpus = ApGlobalData->NumCpus;

    if (Cpu == ApGlobalData->Bsp || Cpu >= NumberCpus) return;
    if (!CpuControl[Cpu].Halted) return;

    ApGlobalData->ReleaseCpu = (VOID*)(UINTN)Cpu;  //Assign CPU number.

    StartCpus(
        (AP_GLOBAL_DATA*)ApGlobalData,
        FALSE,
        CpuControl[Cpu].Id
    );

    // Synchronize all APs' MTRRs with BSP's MTRRs
//###    DEBUG((EFI_D_INFO,"RestartAp: Before MPSynchronizeMTRRs()"));
    MPSynchronizeMTRRs(MpData);
//###    DEBUG((EFI_D_INFO,"RestartAp: After MPSynchronizeMTRRs()"));

}

/**
  This function restarts the CPU when timeout expired
        
      (PEI Only) IN EFI_PEI_SERVICES      **PeiServices
      (PEI Only) IN EFI_PEI_STALL_PPI     *PeiStall
  @param MpData - pointer to AP_GLOBAL_DATA structure
  @param Cpu - Cpu to restart.

  @retval VOID
**/

VOID 
ResetAp(
  IN VOID     *MpData,
  IN UINT8    Cpu )
{
    volatile AP_GLOBAL_DATA *ApGlobalData = (volatile AP_GLOBAL_DATA *)MpData;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;

    if (Cpu == ApGlobalData->Bsp || Cpu >= ApGlobalData->NumCpus) return;

    ApGlobalData->ReleaseCpu = (VOID*)(UINTN)Cpu;  //Assign CPU number.
    ApGlobalData->Idt = (UINT32)&pIDT;   //Save Idt table pointer

    StartCpus(
        (AP_GLOBAL_DATA*)ApGlobalData,
        FALSE,
        CpuControl[Cpu].Id
    );

    // Synchronize all APs' MTRRs with BSP's MTRRs
//###    DEBUG((EFI_D_INFO,"ResetAp: Before MPSynchronizeMTRRs()"));
    MPSynchronizeMTRRs(MpData);
//###    DEBUG((EFI_D_INFO,"ResetAp: After MPSynchronizeMTRRs()"));
}

/**
  Get number of CPUs, that has been found.

  @param MpData - pointer to AP_GLOBAL_DATA structure

  @retval UINT8 - Number of CPUs found.
**/

UINT8 
GetNumCpus(
  IN VOID *MpData )
{
    return ((AP_GLOBAL_DATA*)MpData)->NumCpus;
}

/**
    Each CPU will execute this function.

    @param MpData 
    @param ExecuteOnBsp - TRUE if BSP executes this function.
    @param Block - TRUE if BSP should wait on APs.
    @param Function - Function to call.
    @param Context - This value is passed to the function.

    @retval VOID

**/

VOID 
ExecuteFunctionOnRunningCpus(
  IN VOID     *MpData,
  IN BOOLEAN  ExecuteOnBsp,
  IN BOOLEAN  Block,
  IN VOID     (*Function)(UINT8 Cpu, VOID *Context),
  IN VOID     *Context )
{
    UINT8   i;
    BOOLEAN IntState;

    volatile AP_GLOBAL_DATA *ApGlobalData = (volatile AP_GLOBAL_DATA *)MpData;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    UINT8   NumberCpus = ApGlobalData->NumCpus;

    WaitUntilZero8(&ApGlobalData->RunningAps);

    ApGlobalData->RunningAps = (NumberCpus - 1) - ApGlobalData->HaltedCpus;

    for (i = 0; i < NumberCpus; ++i)
    {
        if (i == ApGlobalData->Bsp) continue;
        if (CpuControl[i].Halted == TRUE) continue;
        IntState = GetInterruptState();
        DisableInterrupts();  

        CpuControl[i].Context = Context;        //Must set Context first.
        CpuControl[i].Function = Function;      //This releases the CPU.
#if USE_AP_HLT        
        // generate NMI for AP
        *((UINT32*)(LOCAL_APIC_BASE | APIC_ICR_HIGH_REGISTER) ) = (UINT32)((UINT8)CpuControl[i].Id << 24);
        *((UINT32*)(LOCAL_APIC_BASE | APIC_ICR_LOW_REGISTER) ) = 0x00000400;
#endif
    }

    if (ExecuteOnBsp) Function(ApGlobalData->Bsp, Context);

    if (Block)
    {
        WaitUntilZero8(&ApGlobalData->RunningAps);
    }
    if(IntState)    EnableInterrupts();   
}

/**
  This function provides a mechanism to execute a function
    on the specific CPU

  @param MpData MP data structure
  @param Cpu Cpu to execute function.
  @param Block TRUE if BSP should wait on APs.
  @param Function Function to call.
  @param Context This value is passed to the function.

  @retval TRUE on successful completion, FALSE otherwise
**/

BOOLEAN 
ExecuteFunctionOnCpu(
  IN VOID     *MpData,
  IN UINT8    Cpu,
  IN BOOLEAN  Block,
  IN VOID     (*Function)(UINT8 Cpu, VOID *Context),
  IN VOID     *Context )
{
    BOOLEAN     IntState;
    volatile AP_GLOBAL_DATA *ApGlobalData = (volatile AP_GLOBAL_DATA *)MpData;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    UINT8       NumberCpus = ApGlobalData->NumCpus;
       //kavi- UINT8       *pRunningAps = (UINT8*)&ApGlobalData->RunningAps;
    volatile    UINT8       *pRunningAps = (UINT8*)&ApGlobalData->RunningAps; //kavi+

    if (Cpu == ApGlobalData->Bsp) {
        Function(Cpu, Context);
        return TRUE;
    }

    if (Cpu >= NumberCpus) return FALSE;
    if (CpuControl[Cpu].Halted == TRUE) return FALSE;

    //Check if running, if not wait until finished.
    WaitUntilZero32(&CpuControl[Cpu].Function);

    CPULib_LockByteInc(pRunningAps);        //Increase number of CPUs running.
    IntState = GetInterruptState();
    //###    DisableInterrupts();  

    CpuControl[Cpu].Context = Context;      //Must set Context first.
    CpuControl[Cpu].Function = Function;    //This releases the CPU.
#if USE_AP_HLT
    // generate NMI for AP
    *((UINT32*)(LOCAL_APIC_BASE | APIC_ICR_HIGH_REGISTER) ) = (UINT32)((UINT8)CpuControl[Cpu].Id << 24);
    *((UINT32*)(LOCAL_APIC_BASE | APIC_ICR_LOW_REGISTER) ) = 0x00000400;
#endif    
    if (Block) {
        WaitUntilZero8(pRunningAps);
    }
    if(IntState)    EnableInterrupts();   

    return TRUE;
}

/**
  Halt all the APs.

  @param MpData 
  @param WaitUntilHalted - TRUE if BSP should wait on APs to halt.

  @retval VOID
**/

VOID 
HaltAllAps(
  IN VOID *MpData,
  IN BOOLEAN WaitUntilHalted )
{
    UINT8   i;
    volatile AP_GLOBAL_DATA *ApGlobalData = (volatile AP_GLOBAL_DATA *)MpData;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    UINT8   NumberCpus = ApGlobalData->NumCpus;

    for (i = 0; i < NumberCpus; ++i)
    {
        if (i == ApGlobalData->Bsp) continue;   //Can't halt BSP.
        CpuControl[i].Halted = TRUE;
#if USE_AP_HLT
        // generate NMI for AP
        *((UINT32*)(LOCAL_APIC_BASE | APIC_ICR_HIGH_REGISTER) ) = (UINT32)((UINT8)CpuControl[i].Id << 24);
        *((UINT32*)(LOCAL_APIC_BASE | APIC_ICR_LOW_REGISTER) ) = 0x00000400;
#endif         
    }

    if (WaitUntilHalted) {
        for(i = 0; i < NumberCpus; ++i) {
            if (i == ApGlobalData->Bsp) continue;   //BSP not halted.
            WaitUntilZero32(&CpuControl[i].Function);
        }
    }
}

/**
  Halt the CPU.

  @param MpData 
  @param Cpu - Cpu to halt.
  @param WaitUntilHalted - TRUE if BSP should wait on APs to halt.

  @retval VOID
**/

VOID 
HaltCpu(
  IN VOID *MpData,
  IN UINT8 Cpu,
  IN BOOLEAN WaitUntilHalted )
{
    volatile AP_GLOBAL_DATA *ApGlobalData = (volatile AP_GLOBAL_DATA *)MpData;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    UINT8   NumberCpus = ApGlobalData->NumCpus;

    if (Cpu == ApGlobalData->Bsp) return;   //Can't halt BSP
    if (Cpu >= NumberCpus) return;
    CpuControl[Cpu].Halted = TRUE;
#if USE_AP_HLT
    // generate NMI for AP
    *((UINT32*)(LOCAL_APIC_BASE | APIC_ICR_HIGH_REGISTER) ) = (UINT32)((UINT8)CpuControl[Cpu].Id << 24);
    *((UINT32*)(LOCAL_APIC_BASE | APIC_ICR_LOW_REGISTER) ) = 0x00000400;
#endif 
    if (WaitUntilHalted) WaitUntilZero32(&CpuControl[Cpu].Function);
}

/**
  Check if a Cpu is halted.

  @param MpData 
  @param Cpu 

  @retval TRUE CPU is halted
  @retval FALSE CPU is not halted
**/

BOOLEAN 
IsCpuHalted(
  IN VOID *MpData,
  IN UINT8 Cpu )
{
    AP_GLOBAL_DATA *ApGlobalData = (AP_GLOBAL_DATA *)MpData;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    UINT8   NumberCpus = ApGlobalData->NumCpus;

    if (Cpu >= NumberCpus) return TRUE;
    return CpuControl[Cpu].Halted && !CpuControl[Cpu].Function;
}

/**
  Check if a CPU is Idle.

  @param VOID *MpData pointer to AP_GLOBAL_DATA structure
  @param UINT8 Cpu AP number

  @retval TRUE AP is idle
  @retval FALSE AP is not idle
**/

BOOLEAN 
IsCpuIdle(
  IN VOID *MpData,
  IN UINT8 Cpu )
{
    AP_GLOBAL_DATA *ApGlobalData = (AP_GLOBAL_DATA *)MpData;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    UINT8   NumberCpus = ApGlobalData->NumCpus;

    if (Cpu >= NumberCpus) return TRUE;
    return !CpuControl[Cpu].Function;
}

/**
  Determine which CPU is the BSP.

  @param VOID* pointer to AP_GLOBAL DATA structure 

  @retval UINT8 the BSP value
**/

UINT8 
WhoIsBsp(
  IN VOID *MpData )
{
    return ((AP_GLOBAL_DATA*)MpData)->Bsp;
}

////////////////////////////////////////////////////////////
//////////////////CPU MP Services Protocol//////////////////
////////////////////////////////////////////////////////////

typedef struct {
    UINT64      CpusWaiting00;      // Bit map of CPUs waiting.
    UINT64      CpusWaiting64;      // Bit map of CPUs waiting.
    VOID        *MpData;            // Private Data
    EFI_EVENT   Event;              // Event to signal when all CPUs are finished.
    BOOLEAN     *Finished;          // For StartupThisAp()
} TRACK_CPUS_WAITING;


VOID
ProcessAllApTimeout(
  IN  AP_GLOBAL_DATA  *ApGlobalData )
{   
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    EFI_STATUS  Status;
    UINT8   i;
    UINT8   NumCpus = ApGlobalData->NumCpus;
    UINT8   Bsp = WhoIsBsp((VOID *)ApGlobalData);
    UINT8   NumRunningCpus;
    UINT8   CpuIdx;

    
    if(ApGlobalData->RunningAps != 0){
        // Save number of HaltedCpus
        NumRunningCpus = ApGlobalData->RunningAps;
        if(FailedCpuListPtr != NULL){
            Status = gBS->AllocatePool(EfiBootServicesData,
                                        (NumRunningCpus + 1) * sizeof(UINTN),
                                         FailedCpuListPtr);
            if(Status != EFI_SUCCESS){
                FailedCpuListPtr = NULL;
            }
        }
        
        ApTerminatedTimeout = TRUE;
        CpuIdx = 0;
        for (i = 0; i < NumCpus; ++i){
            if (i == Bsp) continue;
            // Skip disabled CPUs
            if (CpuControl[i].Halted) continue;
            if (!IsCpuIdle(ApGlobalData, i)) {
                ResetAp((VOID*)ApGlobalData, i);
                ApGlobalData->RunningAps--;
                
                // Fill out FailedCPUList
                if((FailedCpuListPtr != NULL)&&(*FailedCpuListPtr != NULL)){
                    (*FailedCpuListPtr)[CpuIdx++] = i;
                }
            }
        }
        if((FailedCpuListPtr != NULL)&&(*FailedCpuListPtr != NULL)){
            (*FailedCpuListPtr)[CpuIdx] = END_OF_CPU_LIST;
            FailedCpuListPtr = NULL;
        }
    }
}

/**
  Handler for periodic timer event used in StartupThisAP() function
  
  @param Event - Event handle
  @param Context - pointer to AP number
  
  @retval VOID
*/

VOID
ThisApTimerHandler(
  IN EFI_EVENT Event, 
  IN VOID *Context )
{
    UINT64  CurrentTime;
    EFI_STATUS  Status;
    AP_GLOBAL_DATA  *ApGlobalData;
    UINT8       Cpu;
    
    Cpu = *((UINT8*)Context);
    if(Cpu >= NCPU)  return;
    ApGlobalData = gPrivateMpServices.ApGlobalData;
    
    if((ThisApDb[Cpu].TimeoutEnd != 0)&&(gCpuArchProtocol != 0)){
        Status = gCpuArchProtocol->GetTimerValue(
                                          gCpuArchProtocol,
                                          0,            // TimerIndex
                                          &CurrentTime,   // TimerValue
                                          NULL          // period in femtoSec
                                        );
        if(CurrentTime > ThisApDb[Cpu].TimeoutEnd){
            ThisApDb[Cpu].ApStatus |= AP_TMOUT_EXP;
            ThisApDb[Cpu].ApStatus &= ~AP_TMOUT_ACT;
            ThisApDb[Cpu].TimeoutEnd = 0;
            // Stop periodic timer
            gBS->SetTimer(ThisApDb[Cpu].ThisApTimerEvent, TimerCancel, 0);
            if(ThisApDb[Cpu].ThisApTimerEvent != NULL){
                gBS->CloseEvent(ThisApDb[Cpu].ThisApTimerEvent);
            }
            if(!IsCpuIdle(ApGlobalData, Cpu)) {
                ResetAp((VOID*)ApGlobalData, Cpu);
                ApGlobalData->RunningAps--;
                ThisApDb[Cpu].ApStatus &= ~AP_BUSY;
            }
            if(ThisApDb[Cpu].WaitEvent != NULL){
                if(ThisApDb[Cpu].Finished)  *(ThisApDb[Cpu].Finished) = FALSE;
                // Signal event
                gBS->SignalEvent(ThisApDb[Cpu].WaitEvent);
                ThisApDb[Cpu].WaitEvent = NULL;
            }
        } else {
            // check if AP finished before timeout
            if(IsCpuIdle(ApGlobalData, Cpu) && (ThisApDb[Cpu].ApStatus & AP_TMOUT_ACT)){
                // Stop periodic timer
                gBS->SetTimer(ThisApDb[Cpu].ThisApTimerEvent, TimerCancel, 0);
                if(ThisApDb[Cpu].ThisApTimerEvent != NULL){
                    gBS->CloseEvent(ThisApDb[Cpu].ThisApTimerEvent);
                }
                if(ThisApDb[Cpu].WaitEvent){
                    if(ThisApDb[Cpu].Finished)  *(ThisApDb[Cpu].Finished) = TRUE;
                    // Signal event
                    gBS->SignalEvent(ThisApDb[Cpu].WaitEvent);
                    ThisApDb[Cpu].WaitEvent = NULL;
                }
            }
        }
    }
}

/**
  Handler for periodic timer event used in StartupAllAPs() function
  
  @param Event - Event handle
  @param Context - pointer to AP number
  
  @retval VOID
*/

VOID 
AllApTimerHandler(
  IN EFI_EVENT Event, 
  IN VOID *Context )
{
    AP_GLOBAL_DATA  *ApGlobalData = (AP_GLOBAL_DATA*)Context;
//  volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    UINT64  CurrentTime;
    EFI_STATUS  Status;
  
    if(AllApTimeoutEnable)  {
        AllApTimeout -= 500;
        if((TimeEnd != 0) && (gCpuArchProtocol != 0)){
            Status = gCpuArchProtocol->GetTimerValue(
                                          gCpuArchProtocol,
                                          0,            // TimerIndex
                                          &CurrentTime,   // TimerValue
                                          NULL          // period in femtoSec
                                        );
            if(CurrentTime >= TimeEnd){
                AllApTimeout = -1;
                TimeEnd = 0;
            }
        }
        if (AllApTimeout < 0){
            AllApTimeoutExpired = TRUE;
        }
    }
    
    // CPU can finish job before timeout check
    if(CancelAllApTimer || (AllApTimeoutExpired &&(AllApWaitEvent == NULL))){
        // Stop periodic timer
        gBS->SetTimer(AllApTimerEvent, TimerCancel, 0);
        if(AllApTimerEvent != NULL){
            gBS->CloseEvent(AllApTimerEvent);
        }
    }
    
    // Non-blocking case
    if(AllApWaitEvent != NULL){
        // Check if CPUs finished before timeout
        if(!AllApTimeoutExpired){
            if(ApGlobalData->RunningAps == 0){
                // Stop periodic timer
                gBS->SetTimer(AllApTimerEvent, TimerCancel, 0);
                if(AllApTimerEvent != NULL){
                    gBS->CloseEvent(AllApTimerEvent);
                }
                // Signal event
                gBS->SignalEvent(AllApWaitEvent);
            }
        } else {
            // Timeout Expired
            // Stop periodic timer
            gBS->SetTimer(AllApTimerEvent, TimerCancel, 0);
            if(AllApTimerEvent != NULL){
                gBS->CloseEvent(AllApTimerEvent);
            }
            // Process APs
            if(ApGlobalData->RunningAps == 0){
                // Stop timer 
            } else {
                // reset APs, fill failed CPU
                ProcessAllApTimeout(ApGlobalData);
            }
            // Signal event
            gBS->SignalEvent(AllApWaitEvent);
        }
    }
}

/**
  Install periodic timer used in StartupThisAP() function
  
  @param TimeOutInMicroseconds - Timeout in microseconds
  @param WaitEvent - Event to signal when timeout expired or AP finishes
  @param Cpu - AP number
  @param ApGlobalData - pointer to AP_GLOBAL_DATA structure
  
  @retval EFI_SUCCESS Timer installed
*/

EFI_STATUS
SetupThisApTimer(    
  IN  UINTN     TimeOutInMicroseconds,
  IN  EFI_EVENT WaitEvent,
  IN  UINT8     Cpu,
  IN  AP_GLOBAL_DATA  *ApGlobalData )
{
    EFI_STATUS  Status;
    UINT64      TimerPeriod;
    UINT64      TimerStartValue;

    if(Cpu >= NCPU){
        return EFI_INVALID_PARAMETER;
    }
    ThisApDb[Cpu].TimeoutEnd = 0;
    ThisApDb[Cpu].CpuNum = Cpu;
    ThisApDb[Cpu].ApStatus = AP_TMOUT_ACT;
    if(TimeOutInMicroseconds > 0){
        if(gCpuArchProtocol == NULL){
            Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **) &gCpuArchProtocol);
        }
        if(gCpuArchProtocol != NULL){
            Status = gCpuArchProtocol->GetTimerValue(
                                          gCpuArchProtocol,
                                          0,            // TimerIndex
                                          &TimerStartValue,    // TimerValue
                                          &TimerPeriod  // period in femtoSec
                                        );
            ThisApDb[Cpu].TimeoutEnd = TimerStartValue + DivU64x64Remainder(MultU64x64(TimeOutInMicroseconds, 1000000000), TimerPeriod, NULL);
        }
    }
    if(WaitEvent != NULL){
        ThisApDb[Cpu].WaitEvent = WaitEvent;
    }
    Status = gBS->CreateEvent(
          EVT_TIMER | EVT_NOTIFY_SIGNAL,
          TPL_NOTIFY,
          ThisApTimerHandler,
          (VOID*)&(ThisApDb[Cpu].CpuNum),
          &(ThisApDb[Cpu].ThisApTimerEvent)
        );
    if(Status != EFI_SUCCESS)   return  Status;
    Status = gBS->SetTimer(
          ThisApDb[Cpu].ThisApTimerEvent,
          TimerPeriodic,
          10*500                // 500 uS
        );
    return  Status;
}

/**
  Install periodic timer used in StartupAllAPs() function
  
  @param TimeOutInMicroseconds - Timeout in microseconds
  @param WaitEvent - Event to signal when timeout expired or AP finishes
  @param ApGlobalData - pointer to AP_GLOBAL_DATA structure
  
  @retval EFI_SUCCESS - Timer installed
*/

EFI_STATUS
SetupAllApTimer(
  IN  UINTN TimeOutInMicroseconds,
  EFI_EVENT   WaitEvent,
  IN  AP_GLOBAL_DATA  *ApGlobalData )
{
    EFI_STATUS  Status;
    UINT64      TimerPeriod;
  
    AllApTimeoutExpired = FALSE;
    if(TimeOutInMicroseconds > 0){
        AllApTimeout = TimeOutInMicroseconds;
        AllApTimeoutEnable = TRUE;
        CancelAllApTimer = FALSE;
    }
    if(WaitEvent != NULL){
        AllApWaitEvent = WaitEvent;
    }
    Status = gBS->CreateEvent(
          EVT_TIMER | EVT_NOTIFY_SIGNAL,
          TPL_NOTIFY,
          AllApTimerHandler,
          (VOID*)ApGlobalData,
          &AllApTimerEvent
        );
    if(gCpuArchProtocol == NULL){
        Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **) &gCpuArchProtocol);
    }
    if(gCpuArchProtocol != NULL){
        Status = gCpuArchProtocol->GetTimerValue(
                                          gCpuArchProtocol,
                                          0,            // TimerIndex
                                          &TimeStart,    // TimerValue
                                          &TimerPeriod  // period in femtoSec
                                        );
        TimeEnd = TimeStart + DivU64x64Remainder(MultU64x64(TimeOutInMicroseconds, 1000000000), TimerPeriod, NULL);
    }
    Status = gBS->SetTimer(
          AllApTimerEvent,
          TimerPeriodic,
          10*500                // 500 uS
        );
    return Status;
}

/**
  Check if CPUs being checked are idle by a periodic timer.
    If so, signal tracked  event and close this event.

  @param Event - Periodic timer event handler
  @param Context - Pointer to TRACK_CPUS_WAITING structure

  @retval VOID
**/

VOID 
CheckCpuWaiting(
  IN EFI_EVENT Event, 
  IN VOID *Context )
{
    TRACK_CPUS_WAITING *Track = (TRACK_CPUS_WAITING*) Context;
    UINT64 CpusWaiting00 = Track->CpusWaiting00;
    UINT64 CpusWaiting64 = Track->CpusWaiting64;
    UINTN  i;

    //For finished CPUs, the bit in the map is cleared.
    for (i = 0; CpusWaiting00 && i < 64; ++i)
    {
        if (CpusWaiting00 & 1)
        {
            if (IsCpuIdle(Track->MpData, (UINT8)i)) Track->CpusWaiting00 &= ~(LShiftU64(1, (UINT8)i));
        }
        CpusWaiting00 = RShiftU64(CpusWaiting00, 1);
    }
    for (i = 0; CpusWaiting64 && i < 64; ++i)
    {
        if (CpusWaiting64 & 1)
        {
            if (IsCpuIdle(Track->MpData, (UINT8)i + 64)) Track->CpusWaiting64 &= ~(LShiftU64(1, (UINT8)i));
        }
        CpusWaiting64 = RShiftU64(CpusWaiting64, 1);
    }

    //If all CPUs have finished, signal the event, and clean up.
    if ((!Track->CpusWaiting00) && (!Track->CpusWaiting64))
    {
        if(Track->Finished)    *(Track->Finished) = TRUE;
        gBS->SignalEvent(Track->Event);
        gBS->FreePool(Track);
        gBS->CloseEvent(Event);
    }
}

/**
  Setup periodic timer to check if CPUs are idle. This function
    works with function CheckCpuWaiting.

  @param MpData - pointer to AP_GLOBAL_DATA structure
  @param CpusWaiting00 - bitmap of running APs (1-63) to check 
  @param CpusWaiting64 - bitmap of running APs (64-127) to check 
  @param Event - event to signal when APs finishes
  @param Finished - pointer to BOOLEAN variable, reflecting if AP finished job

  @retval EFI_STATUS return the EFI status
**/

EFI_STATUS 
SetupCheckCpus(
  IN VOID       *MpData,
  IN UINT64     CpusWaiting00,
  IN UINT64     CpusWaiting64,
  IN EFI_EVENT  Event,
  IN BOOLEAN    *Finished )
{
    TRACK_CPUS_WAITING *Track;
    EFI_STATUS          Status;
    EFI_EVENT           CheckCpusEvent;

    Status = gBS->AllocatePool(EfiBootServicesData, sizeof(TRACK_CPUS_WAITING), &Track);
    if (EFI_ERROR(Status)) return Status;

    Track->CpusWaiting00 = CpusWaiting00;
    Track->CpusWaiting64 = CpusWaiting64;
    Track->MpData = MpData;
    Track->Event = Event;
    Track->Finished = Finished;

    //Create a timer event, to check when APs have finished.
    Status = gBS->CreateEvent(
        EVT_TIMER | EVT_NOTIFY_SIGNAL,
        TPL_NOTIFY,
        CheckCpuWaiting,
        Track,
        &CheckCpusEvent
    );
    if (EFI_ERROR(Status)) goto CREATE_EVENT_ERROR;

    Status = gBS->SetTimer(
        CheckCpusEvent,
        TimerPeriodic,
        10*100                  //100uS
    );
    if (EFI_ERROR(Status)) goto SET_TIMER_ERROR;

    return EFI_SUCCESS;

SET_TIMER_ERROR:
    gBS->CloseEvent(CheckCpusEvent);

CREATE_EVENT_ERROR:
    gBS->FreePool(Track);
    return Status;
}

/**
  A wrapper function to call EFI_MP_SERVICES_PROTOCOL function
    to execute on CPUs.

  @param Cpu - AP number
  @param Context - pointer to EFI_MP_CONTEXT structure

  @retval VOID
**/

VOID 
EfiMpFunction(
  IN UINT8 Cpu, 
  IN VOID *Context )
{
    EFI_MP_CONTEXT *Mp = (EFI_MP_CONTEXT*)Context;
    Mp->Function(Mp->Parameters);
}


/**
  Return CPU information.

  @param This 
  @param NumberOfProcessors 
  @param NumberOfEnabledProcessors 

  @retval EFI_STATUS return the EFI status
**/
EFI_STATUS 
EFIAPI 
EfiGetNumberOfProcessors (
  IN  EFI_MP_SERVICES_PROTOCOL *This,
  OUT UINTN *NumberOfProcessors,
  OUT UINTN *NumberOfEnabledProcessors )
{
  AP_GLOBAL_DATA       *ApGlobalData = ((PRIVATE_MP_SERVICES *)This)->ApGlobalData;
  volatile CPU_CONTROL *CpuControl   = ApGlobalData->CpuControl;    
  UINTN                ExecutingCpu;
  UINTN                Index;
  UINTN                EnCount = 0;


  if ((NumberOfProcessors == NULL) || (NumberOfEnabledProcessors == NULL)) {
      return EFI_INVALID_PARAMETER;
  }
  // Function can be called only by BSP
  EfiWhoAmI(This, &ExecutingCpu);
  if(ExecutingCpu != ApGlobalData->Bsp) {
      return EFI_DEVICE_ERROR;
  }

  for(Index=0;Index<ApGlobalData->NumCpus;Index++){
    if(!CpuControl[Index].Disabled){
      EnCount++;
    }
  }

  *NumberOfProcessors = ApGlobalData->NumCpus;
  *NumberOfEnabledProcessors = EnCount;

  return EFI_SUCCESS;
}


#if 0
/**
  Return processor information.
        
  @param This This object
  @param ProcessorNumber Number of processor
  @param BufferLength Length of Buffer
  @param ProcessorContextBuffer Buffer

  @retval EFI_SUCCESS if returned successful.
  @retval EFI_INVALID_PARAMETER if CPU not present.
  @retval EFI_BUFFER_TOO_SMALL if buffer is to small. Update *BufferLenght
        with buffer size.

  @note  Only ProcessorGlobalID field from EFI_MP_PROC_CONTEXT has useful
          information.
**/

EFI_STATUS 
EFIAPI 
EfiGetProcessorContext(
  IN EFI_MP_SERVICES_PROTOCOL *This,
  IN UINTN        ProcessorNumber,
  IN OUT UINTN    *BufferLength,
  IN OUT VOID     *ProcessorContextBuffer )
{
    AP_GLOBAL_DATA *ApGlobalData = ((PRIVATE_MP_SERVICES *)This)->ApGlobalData;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    EFI_MP_PROC_CONTEXT *ProcessorBuffer;
    UINT8   NumCpus = ApGlobalData->NumCpus;

    if (BufferLength == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    if (*BufferLength < sizeof (EFI_MP_PROC_CONTEXT)) {
        *BufferLength = sizeof (EFI_MP_PROC_CONTEXT);
        return EFI_BUFFER_TOO_SMALL;
    }

    if ((ProcessorNumber >= NumCpus) || (ProcessorContextBuffer == NULL)){
        return EFI_INVALID_PARAMETER;
    }

    ProcessorBuffer = ProcessorContextBuffer;
    ProcessorBuffer->ProcessorGlobalID = CpuControl[ProcessorNumber].Id;
    ProcessorBuffer->ProcessorEnabledFlag = 1;
    ProcessorBuffer->ProcessorDesignationFlags = ApGlobalData->Bsp ? 1 : 0;
    ProcessorBuffer->ProcessorHealthFlags = 0;
    ProcessorBuffer->ProcessorPALCompatibilityFlags = 0;
    ProcessorBuffer->ProcessorTestMask =0;
    ProcessorBuffer->ProcessorPackageNumber = 0;
    ProcessorBuffer->ProcessorCoreNumber = 0;
    ProcessorBuffer->ProcessorThreadNumber = 0;

    return EFI_SUCCESS;
}

#else
/**
  Return processor information.

  @param This - This object
  @param ProcessorNumber - Number of processor
  @param ProcessorInfoBuffer - Buffer

  @retval EFI_SUCCESS if returned successful.
  @retval EFI_INVALID_PARAMETER if CPU not present.
**/

EFI_STATUS 
EFIAPI 
EfiGetProcessorInfo(
    IN EFI_MP_SERVICES_PROTOCOL        *This,
    IN UINTN                           ProcessorNumber,
    IN OUT EFI_PROCESSOR_INFORMATION   *ProcessorInfoBuffer )
{
    AP_GLOBAL_DATA *ApGlobalData = ((PRIVATE_MP_SERVICES *)This)->ApGlobalData;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    UINT8   NumCpus = ApGlobalData->NumCpus;
    UINTN   ExecutingCpu;

    if (ProcessorNumber >= NumCpus) {
        return EFI_NOT_FOUND;
    }
    if(ProcessorInfoBuffer == NULL) {
        return EFI_INVALID_PARAMETER;
    }
    // Function can be called only by BSP
    EfiWhoAmI(This, &ExecutingCpu);
    if(ExecutingCpu != ApGlobalData->Bsp) {
        return EFI_DEVICE_ERROR;
    }
    
    ProcessorInfoBuffer->ProcessorId = CpuControl[ProcessorNumber].Id;

    if (ApGlobalData->Bsp == ProcessorNumber) {
        // it's BSP
        ProcessorInfoBuffer->StatusFlag = 0x7;  // Healthy Enabled BSP.
    }
    else {
        // it's AP
        ProcessorInfoBuffer->StatusFlag = 0x6;  // Healthy Enabled AP.
        if ( CpuControl[ProcessorNumber].Halted ){
          ProcessorInfoBuffer->StatusFlag &=~(UINT32)0x2; // clear PROCESSOR_ENABLED_BIT
        }
    }

    ProcessorInfoBuffer->Location.Package = 0;
    ProcessorInfoBuffer->Location.Core = 0;
    ProcessorInfoBuffer->Location.Thread = 0;

    return EFI_SUCCESS;
}
#endif


/**
  Execute function on all APs.

  @param This 
  @param Procedure 
  @param SingleThread 
        - if TRUE, execute one function on one CPU at a time.
        - if FALSE, execute function on all CPUs at the same time.
  @param WaitEvent OPTIONAL
        - Event to signal when CPUs have completed executing.
  @param TimeoutInMicroSecs Indicates the time limit in microseconds for APs to return from Procedure, either
                              for blocking or non-blocking mode. Zero means infinity.
  @param ProcArguments OPTIONAL
        - Paramter to pass to function.
  @param FailedCPUList OPTIONAL      (Unsupported)
  @param FailedCPUList OPTIONAL     (Unsupported)

  @retval EFI_STATUS return the EFI status
**/

EFI_STATUS 
EFIAPI 
EfiStartupAllAPs(
  IN EFI_MP_SERVICES_PROTOCOL *This,
  IN EFI_AP_PROCEDURE Procedure,
  IN BOOLEAN      SingleThread,
  IN EFI_EVENT    WaitEvent OPTIONAL,
  IN UINTN        TimeoutInMicroSecs,
  IN OUT VOID     *ProcArguments OPTIONAL,
  OUT UINTN       **FailedCPUList OPTIONAL 
  )
{
    AP_GLOBAL_DATA *ApGlobalData = ((PRIVATE_MP_SERVICES *)This)->ApGlobalData;
    BOOLEAN IsBlock = TRUE;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    UINT8   Bsp = WhoIsBsp((VOID *)ApGlobalData);
    UINT8   NumCpus = ApGlobalData->NumCpus;
    UINT8   i;
    EFI_STATUS Status;
    UINTN   ExecutingCpu;
    BOOLEAN IntState;
    //kavi-  UINT8   *pRunningAps = (UINT8*)&ApGlobalData->RunningAps;
    volatile    UINT8   *pRunningAps = (UINT8*)&ApGlobalData->RunningAps; //kavi+


    DEBUG((EFI_D_INFO, "StartAllAp(%X %d)\n", Procedure, SingleThread));

    // No enabled APs exist in the system.
    if((ApGlobalData->NumCpus - ApGlobalData->HaltedCpus) == 1){
        return EFI_NOT_STARTED;
    }
    if (Procedure == NULL){
        return EFI_INVALID_PARAMETER;
    }

    // Function can be called only by BSP
    EfiWhoAmI(This, &ExecutingCpu);
    if(ExecutingCpu != ApGlobalData->Bsp) {
        return EFI_DEVICE_ERROR;
    }

    // All Enabled CPUs must be idle.
    for (i = 0; i < NumCpus; ++i)
    {
        if (i == Bsp) continue;
        // Skip disabled CPUs
        if (CpuControl[i].Halted) continue;
        if (!IsCpuIdle(ApGlobalData, i)) return EFI_NOT_READY;
    }

    EfiMpContext[1].Function = Procedure;
    EfiMpContext[1].Parameters = ProcArguments;

    if(FailedCPUList != NULL){
      FailedCpuListPtr = FailedCPUList;
    }
    
    if((TimeoutInMicroSecs != 0) || (WaitEvent != 0)){
          SetupAllApTimer(TimeoutInMicroSecs, WaitEvent, ApGlobalData);
    }
    
    if (SingleThread) {
        // Execute one CPU at a time.
        for (i = 0; i < NumCpus; ++i)
        {
            if (i == Bsp) continue;
            // Skip disabled CPUs
            if (CpuControl[i].Halted) continue;

            CPULib_LockByteInc(pRunningAps);        // Increase number of CPUs running.
            CpuControl[i].Context = &EfiMpContext[1];      //Must set Context first.
            CpuControl[i].Function = EfiMpFunction;    //This releases the CPU.
#if USE_AP_HLT        
            // generate NMI for AP
            *((UINT32*)(LOCAL_APIC_BASE | APIC_ICR_HIGH_REGISTER) ) = (UINT32)((UINT8)CpuControl[i].Id << 24);
            *((UINT32*)(LOCAL_APIC_BASE | APIC_ICR_LOW_REGISTER) ) = 0x00000400;
#endif            
            if(WaitEvent == NULL){
                if(TimeoutInMicroSecs == 0){
                    WaitUntilZero8((UINT8*)&ApGlobalData->RunningAps);
                } else {
                    while((ApGlobalData->RunningAps != 0)|| !AllApTimeoutExpired){
                        CpuPause();
                    }
                }
            }
            if(TimeoutInMicroSecs != 0){
                if(ApTerminatedTimeout){
                    ApTerminatedTimeout = FALSE;
                    return EFI_TIMEOUT;
                }
            }
        }
//        if (WaitEvent &&(TimeoutInMicroSecs == 0)) gBS->SignalEvent(WaitEvent);
        return EFI_SUCCESS;
    }
    
    //Create periodic timer event for CPUs.
    if (WaitEvent &&(TimeoutInMicroSecs == 0)) {
        UINT64  CpusWaiting00 = 0;
        UINT64  CpusWaiting64 = 0;
        for (i = 0; i < NumCpus; ++i)
        {
            if (i == Bsp) continue;
            // Skip disabled CPUs
            if (CpuControl[i].Halted) continue;
            if(i < 64){
              CpusWaiting00 |= LShiftU64(1, i);
            }else{
              CpusWaiting64 |= LShiftU64(1, i-64);
            }
        }

        //If returns an error, execute, but BSP must wait for APs.
        //Before non-block works, both the Timer and the CPU architectural
        // protocols must be installed.
        Status = SetupCheckCpus(ApGlobalData, CpusWaiting00, CpusWaiting64, WaitEvent, NULL);
        if (!EFI_ERROR(Status)) IsBlock = FALSE;
    }

    ApGlobalData->RunningAps = (NumCpus - 1) - ApGlobalData->HaltedCpus;

    IntState = GetInterruptState();
    DisableInterrupts();  
    
    for (i = 0; i < NumCpus; ++i)
    {
        if (i == ApGlobalData->Bsp) continue;
        if (CpuControl[i].Halted == TRUE) continue;

        CpuControl[i].Context = &EfiMpContext[1];        //Must set Context first.
        CpuControl[i].Function = EfiMpFunction;      //This releases the CPU.
#if USE_AP_HLT        
        // generate NMI for AP
        *((UINT32*)(LOCAL_APIC_BASE | APIC_ICR_HIGH_REGISTER) ) = (UINT32)((UINT8)CpuControl[i].Id << 24);
        *((UINT32*)(LOCAL_APIC_BASE | APIC_ICR_LOW_REGISTER) ) = 0x00000400;
#endif        
    }
    if(IntState)    EnableInterrupts();   

    if (IsBlock && !AllApTimeoutEnable)
    {
        WaitUntilZero8(&ApGlobalData->RunningAps);
    } else if(IsBlock && AllApTimeoutEnable){
        while(TRUE){
            if((ApGlobalData->RunningAps == 0) || (AllApTimeoutExpired) ){
                AllApTimeoutEnable = FALSE;
                CancelAllApTimer = TRUE;
                AllApTimeout = 0;
                if((ApGlobalData->RunningAps != 0) && (AllApTimeoutExpired)){
                    ProcessAllApTimeout(ApGlobalData);
                }
                break;
            }
            STALL_CPU(100);
        }
    }
    //If an error occured on setting up non-blocking, signal event here.
    if (IsBlock && WaitEvent) gBS->SignalEvent(WaitEvent);
    if(ApTerminatedTimeout) {
        ApTerminatedTimeout = FALSE;
        if(WaitEvent == NULL){
            return  EFI_TIMEOUT;
        }
    }
    return EFI_SUCCESS;
}


/**
  Execute function on Cpu.

  @param This 
  @param Procedure 
  @param ProcessorNumber 
  @param WaitEvent OPTIONAL
         - Event to signal when CPUs have completed executing.
  @param TimeoutInMicroSecs OPTIONAL  (Unsupported)
  @param ProcArguments OPTIONAL
        - Paramter to pass to function.
  @param Finished (Unsupported)


         
  @retval EFI_STATUS return the EFI status
**/
EFI_STATUS 
EFIAPI 
EfiStartupThisAP(
    IN EFI_MP_SERVICES_PROTOCOL    *This,
    IN EFI_AP_PROCEDURE Procedure,
    IN UINTN            ProcessorNumber,
    IN EFI_EVENT        WaitEvent OPTIONAL,
    IN UINTN            TimeoutInMicroSecs OPTIONAL,
    IN OUT VOID         *ProcArguments OPTIONAL,
    OUT BOOLEAN         *Finished 
    )
{
    AP_GLOBAL_DATA  *ApGlobalData = ((PRIVATE_MP_SERVICES *)This)->ApGlobalData;
    BOOLEAN IsBlock = TRUE;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    UINT8   NumCpus = ApGlobalData->NumCpus;
    UINT8   Bsp = WhoIsBsp((VOID *)ApGlobalData);
    UINTN   ExecutingCpu;
   //kavi- UINT8   *pRunningAps = (UINT8*)&ApGlobalData->RunningAps;
    volatile    UINT8   *pRunningAps = (UINT8*)&ApGlobalData->RunningAps; //kavi+
    EFI_STATUS  Status;


    DEBUG((EFI_D_INFO, "StartAp(%X %d)\n", Procedure, ProcessorNumber));

    if ((ProcessorNumber == Bsp) || (Procedure == NULL)) return EFI_INVALID_PARAMETER;
    if (ProcessorNumber >= NumCpus) return EFI_NOT_FOUND;
    // Check if AP is disabled
    if (CpuControl[ProcessorNumber].Halted) return EFI_INVALID_PARAMETER;

    if (!IsCpuIdle(ApGlobalData, (UINT8)ProcessorNumber)) return EFI_NOT_READY;
    // Function can be called only by BSP
    EfiWhoAmI(This, &ExecutingCpu);
    if(ExecutingCpu != Bsp) {
        return EFI_DEVICE_ERROR;
    }

    EfiMpContext[ProcessorNumber].Function = Procedure;
    EfiMpContext[ProcessorNumber].Parameters = ProcArguments;

    //Create periodic timer event for CPUs.
    if ((WaitEvent)&&(TimeoutInMicroSecs == 0)) {
        UINT64      CpusWaiting00 = 0;
        UINT64      CpusWaiting64 = 0;
        EFI_STATUS  Status;
        if ( ProcessorNumber < 64 ) {
          CpusWaiting00 = LShiftU64(1, (UINT8)ProcessorNumber);
        } else {
          CpusWaiting64 = LShiftU64(1, (UINT8)ProcessorNumber - 64);
        }

        //If returns an error, execute, but BSP must wait for APs.
        //Before non-block works, both the Timer and the CPU architectural
        // protocols must be installed.
        Status = SetupCheckCpus(ApGlobalData, CpusWaiting00, CpusWaiting64, WaitEvent, Finished);
        if (!EFI_ERROR(Status)) IsBlock = FALSE;
    }

    if (TimeoutInMicroSecs != 0){
        Status = SetupThisApTimer(TimeoutInMicroSecs, WaitEvent, (UINT8)ProcessorNumber,
                                    ApGlobalData);
        if (!EFI_ERROR(Status)) IsBlock = FALSE;
        WaitUntilZero32(&CpuControl[ProcessorNumber].Function);
        
        if(WaitEvent && Finished){
            ThisApDb[ProcessorNumber].Finished = Finished;
        }
        CPULib_LockByteInc(pRunningAps);        //Increase number of CPUs running.

        CpuControl[ProcessorNumber].Context = &EfiMpContext[ProcessorNumber];      //Must set Context first.
        CpuControl[ProcessorNumber].Function = EfiMpFunction;    //This releases the CPU.
#if USE_AP_HLT        
        // generate NMI for AP
        *((UINT32*)(LOCAL_APIC_BASE | APIC_ICR_HIGH_REGISTER) ) = (UINT32)((UINT8)CpuControl[ProcessorNumber].Id << 24);
        *((UINT32*)(LOCAL_APIC_BASE | APIC_ICR_LOW_REGISTER) ) = 0x00000400;
#endif        

        if (IsBlock) {
            WaitUntilZero8(pRunningAps);
            if (WaitEvent) {
                if(Finished)    *Finished = TRUE;
                gBS->SignalEvent(WaitEvent);
            }
            return EFI_SUCCESS;
        } else {
            if (WaitEvent)  return EFI_SUCCESS;
            while((ApGlobalData->RunningAps != 0) && ((ThisApDb[ProcessorNumber].ApStatus & AP_TMOUT_EXP) != AP_TMOUT_EXP)){
                CpuPause();
            }
            if(ThisApDb[ProcessorNumber].ApStatus & AP_TMOUT_EXP) {
                ThisApDb[ProcessorNumber].ApStatus &= ~AP_TMOUT_EXP;
                return  EFI_TIMEOUT;
            }
        }
    }
    
    if (TimeoutInMicroSecs == 0){
        ExecuteFunctionOnCpu(
            ApGlobalData,
            (UINT8)ProcessorNumber,
            IsBlock,
            EfiMpFunction,
            &EfiMpContext[ProcessorNumber]
        );
    }

    //If an error occurred on setting up non-blocking, signal event here.
    if (IsBlock && WaitEvent) gBS->SignalEvent(WaitEvent);

    return EFI_SUCCESS;
}

/**
  Switch BSP to another CPU.
        
  @param This 
  @param ProcessorNumber 
  @param EnableOldBSP (FALSE - disable current BSP)
         
  @retval EFI_STATUS return the EFI status
**/

EFI_STATUS 
EFIAPI 
EfiSwitchBSP(
    IN EFI_MP_SERVICES_PROTOCOL *This,
    IN UINTN    ProcessorNumber,
    IN BOOLEAN  EnableOldBSP )
{
    AP_GLOBAL_DATA *ApGlobalData = ((PRIVATE_MP_SERVICES *)This)->ApGlobalData;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    UINT8 OldBsp;
    UINTN ExecutingCpu;

    if (ProcessorNumber >= ApGlobalData->NumCpus) return EFI_NOT_FOUND;
    if(ProcessorNumber == ApGlobalData->Bsp) return EFI_INVALID_PARAMETER;
    // Check if AP is disabled
    if (CpuControl[ProcessorNumber].Halted) return EFI_INVALID_PARAMETER;
    // Function can be called only by BSP
    EfiWhoAmI(This, &ExecutingCpu);
    if(ExecutingCpu != ApGlobalData->Bsp) {
        return EFI_DEVICE_ERROR;
    }

    OldBsp = WhoIsBsp(ApGlobalData);
    
    SwitchBsp(ApGlobalData,
        (UINT8)ProcessorNumber,
        SwitchBspProgramReg,
        (VOID*) ProcessorNumber
    );
    //Now, Old bsp is in wait loop, and New Bsp is executing here.

    if (!EnableOldBSP) HaltCpu(ApGlobalData, OldBsp, FALSE);

    return EFI_SUCCESS;
}

/**
  (UNSUPPORTED) Send IPI to CPU.

  @param This 
  @param ProcessorNumber 
  @param VectorNumber 
  @param DeliveryMode 

  @retval EFI_UNSUPPORTED always unsupported
**/

EFI_STATUS 
EFIAPI 
EfiSendIPI(
    IN EFI_MP_SERVICES_PROTOCOL    *This,
    IN UINTN    ProcessorNumber,
    IN UINTN    VectorNumber,
    IN UINTN    DeliveryMode )
{
    //Only the CPU module should Send IPI.
    return EFI_UNSUPPORTED;
}

/**
  Enable or disable a AP

  @param This 
  @param ProcessorNumber 
  @param Enable 
  @param HealthState OPTIONAL
      #if PI_SPECIFICATION_VERSION < 0x0001000A || BACKWARD_COMPATIBLE_MODE && defined(NO_PI_MP_SERVICES_SUPPORT)
  @param HealthState OPTIONAL
      #endif

  @retval EFI_STATUS return the EFI status
**/

EFI_STATUS 
EFIAPI 
EfiEnableDisableAP(
  IN EFI_MP_SERVICES_PROTOCOL    *This,
  IN UINTN    ProcessorNumber,
  IN BOOLEAN  Enable,
  UINT32      *HealthState OPTIONAL 
  )
{
    AP_GLOBAL_DATA *ApGlobalData = ((PRIVATE_MP_SERVICES *)This)->ApGlobalData;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    UINTN   ExecutingCpu;

    if (ProcessorNumber >= ApGlobalData->NumCpus) return EFI_NOT_FOUND;

    if (ProcessorNumber == ApGlobalData->Bsp) return EFI_INVALID_PARAMETER;

    // Function can be called only by BSP
    EfiWhoAmI(This, &ExecutingCpu);
    if(ExecutingCpu != ApGlobalData->Bsp) {
        return EFI_DEVICE_ERROR;
    }

    if (!Enable){
        if(!CpuControl[ProcessorNumber].Halted){
            UINT8   HaltedCpus = ApGlobalData->HaltedCpus;
            HaltCpu(ApGlobalData, (UINT8)ProcessorNumber, FALSE);
            while(HaltedCpus == ApGlobalData->HaltedCpus){
                CpuPause();
            }
        }
        // if AP disabled (halted) already - do nothing
    } else {
        if(CpuControl[ProcessorNumber].Halted){
            RestartAp(ApGlobalData, (UINT8)ProcessorNumber);
        }
        // if AP enabled already - do nothing
    }
    if (HealthState != NULL){
        *HealthState = (UINT32)PROCESSOR_HEALTH_STATUS_BIT;     // Healthy enabled/Disabled
    }
    return EFI_SUCCESS;
}

/**
  Return assigned number to CPU calling this.
        
  @param This 
  @param ProcessorNumber 
         
  @retval EFI_STATUS return the EFI status
**/

EFI_STATUS 
EFIAPI 
EfiWhoAmI(
    IN EFI_MP_SERVICES_PROTOCOL *This,
    OUT UINTN   *ProcessorNumber )
{
    AP_GLOBAL_DATA *ApGlobalData = ((PRIVATE_MP_SERVICES *)This)->ApGlobalData;
    volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
    UINT32  Id = MmioRead32(APIC_BASE + APIC_ID_REGISTER) >> 24;
    UINT8   NumCpu = ApGlobalData->NumCpus;
    UINTN   i;

    if(ProcessorNumber == NULL) return EFI_INVALID_PARAMETER;
    
    //Search CPU ID that matches this CPU.
    for (i = 0; i < NumCpu; ++i) {
        if (CpuControl[i].Id == Id) {
            *ProcessorNumber = i;
            return EFI_SUCCESS;
        }
    }

    return EFI_UNSUPPORTED;
}






EFI_STATUS 
EFIAPI 
MyEfiStartupAllAPs (
  IN EFI_MP_SERVICES_PROTOCOL *This,
  IN EFI_AP_PROCEDURE Procedure,
  IN BOOLEAN      SingleThread,
  IN EFI_EVENT    WaitEvent OPTIONAL,
  IN UINTN        TimeoutInMicroSecs,
  IN OUT VOID     *ProcArguments OPTIONAL,
  OUT UINTN       **FailedCPUList OPTIONAL 
  )
{
  AP_GLOBAL_DATA       *ApGlobalData = ((PRIVATE_MP_SERVICES*)This)->ApGlobalData;
  volatile CPU_CONTROL *CpuControl   = ApGlobalData->CpuControl;
  UINT8                NumCpus       = ApGlobalData->NumCpus;
  UINT8                i;
  EFI_STATUS           Status;
  

  for (i = 0; i < NumCpus; i++){
    if(!CpuControl[i].Disabled){
      EfiEnableDisableAP(This, i, TRUE, NULL);
    }
  }
  
  Status = EfiStartupAllAPs(
             This, 
             Procedure, 
             SingleThread, 
             WaitEvent, 
             TimeoutInMicroSecs, 
             ProcArguments,
             FailedCPUList
             );

  for (i = 0; i < NumCpus; i++){
    EfiEnableDisableAP(This, i, FALSE, NULL);
  }  

  return Status;
}





EFI_STATUS 
EFIAPI 
MyEfiStartupThisAP(
    IN EFI_MP_SERVICES_PROTOCOL    *This,
    IN EFI_AP_PROCEDURE Procedure,
    IN UINTN            ProcessorNumber,
    IN EFI_EVENT        WaitEvent OPTIONAL,
    IN UINTN            TimeoutInMicroSecs OPTIONAL,
    IN OUT VOID         *ProcArguments OPTIONAL,
    OUT BOOLEAN         *Finished 
    )
{
  EFI_STATUS           Status;
  AP_GLOBAL_DATA       *ApGlobalData = ((PRIVATE_MP_SERVICES*)This)->ApGlobalData;
  volatile CPU_CONTROL *CpuControl   = ApGlobalData->CpuControl;

  if(!CpuControl[ProcessorNumber].Disabled){
    EfiEnableDisableAP(This, ProcessorNumber, TRUE, NULL);
  }

  Status = EfiStartupThisAP (
             This, 
             Procedure, 
             ProcessorNumber, 
             WaitEvent, 
             TimeoutInMicroSecs, 
             ProcArguments,
             Finished
             );

  EfiEnableDisableAP(This, ProcessorNumber, FALSE, NULL);

  return Status;
}


EFI_STATUS 
EFIAPI 
MyEfiEnableDisableAP(
    IN EFI_MP_SERVICES_PROTOCOL    *This,
    IN UINTN    ProcessorNumber,
    IN BOOLEAN  Enable,
    UINT32      *HealthState OPTIONAL 
    )
{
  AP_GLOBAL_DATA       *ApGlobalData = ((PRIVATE_MP_SERVICES *)This)->ApGlobalData;
  volatile CPU_CONTROL *CpuControl = ApGlobalData->CpuControl;
  UINTN                ExecutingCpu;


  if (ProcessorNumber >= ApGlobalData->NumCpus) return EFI_NOT_FOUND;
  if (ProcessorNumber == ApGlobalData->Bsp) return EFI_INVALID_PARAMETER;
  EfiWhoAmI(This, &ExecutingCpu);
  if(ExecutingCpu != ApGlobalData->Bsp) {
      return EFI_DEVICE_ERROR;
  }

  CpuControl[ProcessorNumber].Disabled = !Enable;

  if (HealthState != NULL){
    *HealthState = (UINT32)PROCESSOR_HEALTH_STATUS_BIT;     // Healthy enabled/Disabled
  }
  
  return EFI_SUCCESS;

}


PRIVATE_MP_SERVICES gPrivateMpServices = {
  {
      EfiGetNumberOfProcessors,
      EfiGetProcessorInfo,
      EfiStartupAllAPs,
      EfiStartupThisAP,
      EfiSwitchBSP,
      EfiEnableDisableAP,
      EfiWhoAmI
  },
  0
};


void UpdateMyMpServiceProtocol()
{
  AP_GLOBAL_DATA       *ApGlobalData = gPrivateMpServices.ApGlobalData;
  volatile CPU_CONTROL *CpuControl   = ApGlobalData->CpuControl;
  UINT8                NumCpus       = ApGlobalData->NumCpus;
  UINT8                i;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  for (i = 0; i < NumCpus; i++){
    EfiEnableDisableAP(&gPrivateMpServices.EfiMpServicesProtocol, i, FALSE, NULL);
    if(!CpuControl[i].Halted){
      DEBUG((EFI_D_INFO, "CPU[%d] Not Halted\n"));
    }
  }  

  gPrivateMpServices.EfiMpServicesProtocol.EnableDisableAP = MyEfiEnableDisableAP;
  gPrivateMpServices.EfiMpServicesProtocol.StartupAllAPs   = MyEfiStartupAllAPs;  
  gPrivateMpServices.EfiMpServicesProtocol.StartupThisAP   = MyEfiStartupThisAP;
}



