
#include "CpuDxe.h"
#include <CpuBoard.h>
#include <token.h>
#include <Protocol/Variable.h>
#include <Protocol/MpService.h>
#include <Protocol/Datahub.h>
#include <SetupVariable.h>
#include <Protocol/LegacyBios.h>
#include <Library/ByoCommLib.h>


#define NUM_COMMON_MTRRS 36

typedef UINT32 MTRR_INDEX[NUM_COMMON_MTRRS]; 
typedef UINT64 MTRR_SAVED_VALUE[NUM_COMMON_MTRRS]; 


#pragma pack (1)
typedef struct {
    MTRR_INDEX          MsrIndex;
	  MTRR_SAVED_VALUE    ValueBuffer;    
    UINT8               Size;
    BOOLEAN             BSPisCopied;
} MTRR_COPY_CONTEXT;
#pragma pack ()

UINT32
WaitForSemaphore (
  IN OUT  volatile UINT8           *Sem
  );






extern MTRR_SETTINGS    *mMtrrTable;
CPUINFO_HOB_DATA        *gCpuInfoData;

volatile UINT8  gCpuInfoSemaphore = 0;
UINT8           gNumCpus = 0;
UINT64          gTimerPeriod = 0;
UINT32          gNumBoostStates = 0;
UINT8           gCoreCnt = 0;   // this value will be update at the end of DxeInitializeCPU
BOOLEAN         gC1ESupport = TRUE;
UINT32          gC1EStackSave;
#ifdef EFIx64
UINT16          *gIrqRuntimeHandler;
#else
UINT8           *gIrqRuntimeHandler;
#endif
INTR_GATE_DESC  *gIdt;

VOID            *gMpData;

#pragma pack(1)
typedef struct {
    UINT8   PushOp;
    UINT32  Interrupt;
    UINT8   PushEaxOp;
#ifdef EFIx64
    UINT8   Ext64Op;
#endif
    UINT8   MovEaxOp;
    VOID    *pJmpAddr;
    UINT16  JmpOp;          //Jmp ebx/rbx
} INT_ENTRY_POINT;
#pragma pack()

INT_ENTRY_POINT     *gIntEntryPoints;

INT_ENTRY_POINT IntEntryTemplate = 
{
    0x68,           //push Int
    0,              //Int
    0x50,           //push eax/rax
#ifdef EFIx64
    0x48,           //Extended Opcode
#endif
    0xb8,           //Mov eax,
    0,              //pJmpAddr
    0xe0ff          //Jmp ebx/rbx
};

//This extern is for a table of offset of interrupt routines in CpuSupport.asm.
extern UINT32       MachineCheckHandlerSize;
extern VOID         *InterruptHandlerStart;
extern UINT32       InterruptHandlerSize;
extern UINT32       InterruptHandlerTblFixup;


EFI_STATUS    InitInterrupts();
VOID 	        InterruptHandlerHalt(EFI_EXCEPTION_TYPE Exception,EFI_SYSTEM_CONTEXT Context);
#if USE_AP_HLT
VOID            InterruptHandlerRet(EFI_EXCEPTION_TYPE Exception,EFI_SYSTEM_CONTEXT Context);
VOID            AsmIret(VOID);
#endif
VOID            SetCPUS3ResumeData();

VOID MachineCheckHandler(
    IN EFI_EXCEPTION_TYPE   InterruptType,
    IN EFI_SYSTEM_CONTEXT   SystemContext
);

extern EFI_MP_SERVICES_PROTOCOL   *gEfiMpServicesProtocol;

VOID TempGPInterruptHandler(EFI_EXCEPTION_TYPE Exception,EFI_SYSTEM_CONTEXT Context);


BOOLEAN
IsItBsp (VOID)
{
    if ((AsmReadMsr32(MSR_XAPIC_BASE)) & XAPIC_BASE_BSP_BIT_MASK)
        return TRUE;       // BSP
    else
        return FALSE;       // AP
}






VOID
CPUReadMsrs (
    IN     MTRR_INDEX           MsrIndex,
    IN OUT MTRR_SAVED_VALUE     ValueBuffer,
    IN     UINT16               Size )
{
    UINT16  i;
    UINT64  qMSRSave;

    // Enable MTRRFix Rd/Wr Dram attributes bits and
    // Enable modification of MTRRFix Rd/Wr Dram attributes bits
    qMSRSave = AsmReadMsr64(MSR_SYS_CFG);
    AsmWriteMsr64(MSR_SYS_CFG, qMSRSave | (3 << MTRRFixDRAMEnBit));

    for (i = 0; i < Size; i ++)
    {
        ValueBuffer[i] = AsmReadMsr64(MsrIndex[i]);
    }

    // Disable modification of MTRRFix Rd/Wr Dram attributes bits
    AsmWriteMsr64(MSR_SYS_CFG, qMSRSave & (~(1 << MTRRFixDRAMModEnBit)));

    return;
}


VOID
CPUWriteMsrs (
    IN  MTRR_INDEX          MsrIndex,
    IN  MTRR_SAVED_VALUE    ValueBuffer,
    IN  UINT16              Size )
{
    UINT16  i;
    BOOLEAN InterruptState;
    UINT64  qMSRSave;

    // Prepare for MTRR programming by disabling MTRRS & cache
    InterruptState = GetInterruptState();
    DisableInterrupts();
    AsmDisableCache();
    AsmWriteMsr64(MTRR_DEF_TYPE, 0);

    // Enable MTRRFix Rd/Wr Dram attributes bits and
    // Enable modification of MTRRFix Rd/Wr Dram attributes bits
    qMSRSave = AsmReadMsr64(MSR_SYS_CFG);
    AsmWriteMsr64(MSR_SYS_CFG, qMSRSave | (3 << MTRRFixDRAMEnBit));

    for (i = 0; i < Size; i ++) {
        AsmWriteMsr64(MsrIndex[i], ValueBuffer[i]);
    }

    // Disable modification of MTRRFix Rd/Wr Dram attributes bits
    // (Read MSR_SYS_CFG again because it may have changed)
    qMSRSave = AsmReadMsr64(MSR_SYS_CFG);
    AsmWriteMsr64(MSR_SYS_CFG, qMSRSave & (~(1 << MTRRFixDRAMModEnBit)));

    // Activate changes by enabling MTRRs & cache
    AsmWriteMsr64(MTRR_DEF_TYPE,3<<10);
    AsmEnableCache ();
    if (InterruptState) EnableInterrupts();

    return;
}


EFI_STATUS
CopyBspMTRRStoAllRunningApMTRRS (
    IN VOID *pMTRRCopyContext )
{
    MTRR_COPY_CONTEXT *Ctx = (MTRR_COPY_CONTEXT*)pMTRRCopyContext;

    if (IsItBsp())
    {
        // Save MSR values of the BSP to buffer
        CPUReadMsrs(Ctx->MsrIndex, Ctx->ValueBuffer, Ctx->Size);
        Ctx->BSPisCopied = 1;
    }
    else
    {
        // It is an error for APs to call this function
        // before the BSP has saved its MSRs into the buffer
        if (!Ctx->BSPisCopied)
            return EFI_ACCESS_DENIED;

        // Write MSR values from buffer to AP's MTRRs
        CPUWriteMsrs(Ctx->MsrIndex, Ctx->ValueBuffer, Ctx->Size);
    }

    return EFI_SUCCESS;
}

VOID
InitMtrrCopyContext (
    IN OUT  MTRR_COPY_CONTEXT *Mcc )
{

    Mcc->Size = NUM_COMMON_MTRRS;
    Mcc->BSPisCopied = FALSE;

    Mcc->MsrIndex[0] = MTRR_PHYS_BASE_0;
    Mcc->MsrIndex[1] = MTRR_PHYS_MASK_0;
    Mcc->MsrIndex[2] = MTRR_PHYS_BASE_1;
    Mcc->MsrIndex[3] = MTRR_PHYS_MASK_1;
    Mcc->MsrIndex[4] = MTRR_PHYS_BASE_2;
    Mcc->MsrIndex[5] = MTRR_PHYS_MASK_2;
    Mcc->MsrIndex[6] = MTRR_PHYS_BASE_3;
    Mcc->MsrIndex[7] = MTRR_PHYS_MASK_3;
    Mcc->MsrIndex[8] = MTRR_PHYS_BASE_4;
    Mcc->MsrIndex[9] = MTRR_PHYS_MASK_4;
    Mcc->MsrIndex[10] = MTRR_PHYS_BASE_5;
    Mcc->MsrIndex[11] = MTRR_PHYS_MASK_5;
    Mcc->MsrIndex[12] = MTRR_PHYS_BASE_6;
    Mcc->MsrIndex[13] = MTRR_PHYS_MASK_6;
    Mcc->MsrIndex[14] = MTRR_PHYS_BASE_7;
    Mcc->MsrIndex[15] = MTRR_PHYS_MASK_7;
    Mcc->MsrIndex[16] = MTRR_FIX_64K_00000;
    Mcc->MsrIndex[17] = MTRR_FIX_16K_80000;
    Mcc->MsrIndex[18] = MTRR_FIX_16K_A0000;
    Mcc->MsrIndex[19] = MTRR_FIX_4K_C0000;
    Mcc->MsrIndex[20] = MTRR_FIX_4K_C8000;
    Mcc->MsrIndex[21] = MTRR_FIX_4K_D0000;
    Mcc->MsrIndex[22] = MTRR_FIX_4K_D8000;
    Mcc->MsrIndex[23] = MTRR_FIX_4K_E0000;
    Mcc->MsrIndex[24] = MTRR_FIX_4K_E8000;
    Mcc->MsrIndex[25] = MTRR_FIX_4K_F0000;
    Mcc->MsrIndex[26] = MTRR_FIX_4K_F8000;
    Mcc->MsrIndex[27] = AMD_MSR_TOP_MEM;
    Mcc->MsrIndex[28] = AMD_MSR_TOP_MEM2;
    Mcc->MsrIndex[29] = MSR_SYS_CFG;
    Mcc->MsrIndex[30] = AMD_MSR_IORR_BASE0;
    Mcc->MsrIndex[31] = 0xC001102A;
    Mcc->MsrIndex[32] = AMD_MSR_MMIO_CFG_BASE;
    Mcc->MsrIndex[33] = AMD_PP_MSR_IC_CFG;
    Mcc->MsrIndex[34] = AMD_PP_MSR_DC_CFG;
//-    Mcc->MsrIndex[35] = AMD_MSR_SMM_ADDR_HL;
//-    Mcc->MsrIndex[36] = AMD_MSR_SMM_MASK_HL;
    Mcc->MsrIndex[35] = AMD_MSR_HWCR;
}


EFI_STATUS
SynchronizeMTRRs (
    IN EFI_BOOT_SERVICES *gBS )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    MTRR_COPY_CONTEXT           MtrrCopyContext;
    EFI_MP_SERVICES_PROTOCOL    *MpServices;

    InitMtrrCopyContext(&MtrrCopyContext);

    Status = gBS->LocateProtocol (
                    &gEfiMpServiceProtocolGuid,
                    NULL,
                    &MpServices );
    if (Status != EFI_SUCCESS)
        return Status;

    // Execute on BSP
    Status = CopyBspMTRRStoAllRunningApMTRRS((void*)&MtrrCopyContext);
    if (Status != EFI_SUCCESS)
        return Status;

    // Execute on all running APs
    Status = MpServices->StartupAllAPs(
        MpServices,                              // EFI_MP_SERVICES_PROTOCOL*
        (EFI_AP_PROCEDURE)
        CopyBspMTRRStoAllRunningApMTRRS,         // EFI_AP_PROCEDURE
        FALSE,                                   // BOOLEAN SingleThread
        NULL,                                    // EFI_EVENT WaitEvent
        0,                                    // UINTN Timeout
        (void*)&MtrrCopyContext,                 // VOID *ProcArguments
        NULL);                                   // UINTN *FailedCPUList

    return Status;
}







/**
    Read the PCI Register

    @param Address :(UINT32) ( (((UINT32)bus) << 24) + (((UINT32)dev) << 19) \
        + (((UINT32)func) << 16) + ((UINT32)reg) )
    @param ExtAccess : TRUE for extended access, FALSE is for normal access

    @retval UINT32 return value from the PCI Register.

**/
UINT32 NbReadPCI32(
    IN UINT32 Address,
    IN BOOLEAN  ExtAccess
)
{
    UINT32  Value32;
    UINT32  AddrCf8 = (1 << 31) + (Address & 0xFFFF00) + (Address & 0xFC);

    if (ExtAccess) //If extended config Access requested then add the extended config address  bit (24 to 27)
        AddrCf8 = AddrCf8 | 0x1000000;

    IoWrite32(0xCF8, AddrCf8);
    Value32 = IoRead32(0xCFC);
    return Value32;
}

/**
    Read a CMOS byte

        
    @param Index 

         
    @retval UINT8 Value

**/
UINT8 EFI_ReadCMOSByte(IN UINT8 Index)
{
    if(Index & 0x80)
    {
        //Upper CMOS Read
        IoWrite8(0x72,Index);
        IoRead8(0x61); //IoDelay
        return IoRead8(0x73);
    }
    else
    {
        //Lower CMOS Read
        IoWrite8(0x70,Index);
        IoRead8(0x61); //IoDelay
        return IoRead8(0x71);
    }
}



IA32_DESCRIPTOR pIDT, pGdt;

//This is a table of pointers to installed interrupt handlers.
//It will be initialized as NULL, meaning no interrupt handler installed.
EFI_CPU_INTERRUPT_HANDLER *InterruptPtrTable;

//This will be copied to runtime memory.
UINT64 GDT[] = {
    0,                  //NULL_SEL
    0x00cf93000000ffff, //DATA_SEL 8, Data 0-ffffffff 32 bit
    0x00cf9b000000ffff, //CODE_SEL 10h, CODE 0-ffffffff 32 bit

// We only need this because Intel DebugSupport driver
// (RegisterPeriodicCallback funciton) assumes that selector 0x20 is valid
// The funciton sets 0x20 as a code selector in IDT
    0x00cf93000000ffff,	// YET_ANOTHER_DATA_SEL, 0x18, Data 0-ffffffff 32 bit	
    0x00cf9b000000ffff,	//YET_ANOTHER_CODE_SEL, 0x20, CODE 0-ffffffff 32 bit
#ifdef EFIx64
    0,                  //Spare
    0x00cf93000000FFFF, //64 Data.
    0x00af9b000000FFFF, //64 Code
    0                   //Spare
#endif
};

/**
    Allocate memory in runtime and copy data.

        
    @param Src 
    @param Size 

         
    @retval VOID return the runtime address

**/

VOID * CpyToRunTime(
    IN VOID *Src,
    IN UINTN Size)
{
    EFI_STATUS  Status;
    VOID        *Dest;

    //align to 16.
    Status = gBS->AllocatePool(EfiACPIMemoryNVS, Size + 16, &Dest);
    ASSERT_EFI_ERROR(Status);
    Dest = (VOID*)(((UINTN)Dest + 0xf) & ~(UINTN)0xf);

    CopyMem(Dest, Src, Size);
    return Dest;
}

/**
    Handler executed for ReadyToBoot

        
    @param Event 
    @param Context 

         
    @retval VOID

**/

VOID ExecuteAtReadyToBoot(IN EFI_EVENT Event, IN VOID *Context)
{
//    UINT8 Value8;

    SynchronizeMTRRs(gBS);
//- SetCPUS3ResumeData();

//    IoWrite8(0x72, PcdGet8(PcdCmosAgesaSpecified));
//    IoRead8(0x61);
//    Value8 = IoRead8(0x73); // Read from CMOS
//    Value8 |= BIT1; 
//    IoWrite8(0x73, Value8); // Save to CMOS

}

/**
    Handler executed before OS.

        
    @param Event 
    @param Context 

         
    @retval VOID

**/

VOID BootNotify(
    IN EFI_EVENT Event,
    IN VOID *Context)
{
	  UINT16 i;
    UINT64  HWCR_Value;

// SmmLock: SMM code lock    
    HWCR_Value = AsmReadMsr64(AMD_MSR_HWCR);
    HWCR_Value |= 0x1;                      // Set SMM lock
    AsmWriteMsr64(AMD_MSR_HWCR, HWCR_Value);

    SynchronizeMTRRs(gBS);
//  SetCPUS3ResumeData();

    DisableInterrupts();

    //The interrupts are programmed to a default runtime handler. This is because
    //earlier OSes that don't support multi-processors will not take control of APs.
    //If this happens, and the AP gets an exception, such as the machine check
    // exception, the system will hang or reset.
    for (i = 0; i < 256; ++i) {
        gIdt[i].LoOffset = (UINT16)gIrqRuntimeHandler;
        gIdt[i].HiOffset = (UINT16)((UINT32)gIrqRuntimeHandler >> 16);
    }

    HaltAllAps(gMpData, TRUE);  //If legacy, CSM halts them.
}

/**
    Calculate CPU time period.

        
    @param VOID

         
    @retval UINT64 Timer Period
**/

UINT64 CalculateTimerPeriod(VOID)
{
    UINT64  Timer1;
    UINT64  Timer2;

    EFI_TPL	OldTpl = gBS->RaiseTPL(TPL_HIGH_LEVEL);

    Timer1 = AsmReadTsc();

    gBS->Stall(DELAY_CALCULATE_CPU_PERIOD);

    Timer2 = AsmReadTsc();

    gBS->RestoreTPL(OldTpl);

    return DivU64x32(DELAY_CALCULATE_CPU_PERIOD * (UINT64)1000000000, (UINT32)(Timer2-Timer1));	//50us * 10E15  femtosec;
}

/**
    Enable CPU Interrupts.

        
    @param This 

         
    @retval EFI_STATUS EFI_SUCCESS (Always)
**/

EFI_STATUS EfiCpuEnableInterrupt (
    IN EFI_CPU_ARCH_PROTOCOL    *This)
{
	EnableInterrupts ();
    return EFI_SUCCESS;
}


/**
    Disable CPU Interrupts

        
    @param This 

         
    @retval EFI_STATUS EFI_SUCCESS (Always)

**/

EFI_STATUS EfiCpuDisableInterrupt (
    IN EFI_CPU_ARCH_PROTOCOL    *This)
{
    DisableInterrupts ();
    return EFI_SUCCESS;
}


/**
    Return State (Disable/Enable) CPU Interrupts.

        
    @param This 
    @param State *State=FALSE if Disable.

         
    @retval EFI_STATUS EFI_SUCCESS (Always)
**/

EFI_STATUS EfiCpuGetInterruptState (
    IN EFI_CPU_ARCH_PROTOCOL    *This,
    OUT BOOLEAN                 *State)
{
    *State = GetInterruptState ();
    return EFI_SUCCESS;
}

/**
    Send INIT to the CPU.

        
    @param This 
    @param InitType 

         
    @retval EFI_STATUS Never returns.

**/
EFI_STATUS EfiCpuINIT (
    IN EFI_CPU_ARCH_PROTOCOL    *This,
    IN EFI_CPU_INIT_TYPE        InitType)
{
	IoWrite8(0x64, 0xfe);	//reset system using keyboard controller.
    CpuDeadLoop();         	//wait until reset.
	return EFI_DEVICE_ERROR;
}

/**
    Install or Uninstall Interrupt Handler.

        
    @param This 
    @param InterruptType 
    @param InterruptHandler 

         
    @retval EFI_SUCCESS Interrupt handler was uninstalled/installed.
    @retval EFI_ALREADY_STARTED Attempt to install an Interrupt Handler
        when one is already installed.
    @retval EFI_INVALID_PARAMETER Attempt to uninstall an Interrupt Handler
        when one is not installed.
    @retval EFI_UNSUPPORTED Interrupt Type not supported.

**/
EFI_STATUS EfiCpuRegisterInterruptHandler (
    IN EFI_CPU_ARCH_PROTOCOL        *This,
    IN EFI_EXCEPTION_TYPE           InterruptType,
    IN EFI_CPU_INTERRUPT_HANDLER    InterruptHandler)
{
    if ((UINTN) InterruptType > 255) return EFI_UNSUPPORTED;
    if (InterruptHandler != NULL && InterruptPtrTable[InterruptType] != NULL)
        return EFI_ALREADY_STARTED;
    if (InterruptHandler == NULL && InterruptPtrTable[InterruptType] == NULL)
        return EFI_INVALID_PARAMETER;

    InterruptPtrTable[InterruptType] = InterruptHandler;

    return EFI_SUCCESS;
}

/**
    For a CPU timer, return its current value.

        
    @param This 
    @param TimerIndex 
    @param TimerValue 
    @param TimerPeriod OPTIONAL

         
    @retval EFI_SUCCESS The time value is returned.
    @retval EFI_INVALID_PARAMETER TimerIndex is greater than 0.
        Only timer exists.
    @retval EFI_INVALID_PARAMETER TimerValue is NULL.

**/

EFI_STATUS EfiCpuGetTimerValue (
    IN EFI_CPU_ARCH_PROTOCOL    *This,
    IN UINT32                   TimerIndex,
    OUT UINT64                  *TimerValue,
    OUT UINT64                  *TimerPeriod OPTIONAL)
{
    if (TimerIndex > 0) return EFI_INVALID_PARAMETER;
    if (TimerValue==NULL)  return EFI_INVALID_PARAMETER;
    if (TimerPeriod != NULL)
    {
        if (!gTimerPeriod) gTimerPeriod = CalculateTimerPeriod();
        *TimerPeriod = gTimerPeriod;
    }

    *TimerValue = AsmReadTsc();

    return EFI_SUCCESS;
}

/**
    Install Interrupt Handlers for 0 to 19, to the routine
    InterruptHalt. Initialize the interrupt descriptors.
    Execute assembly instruction LIDT.


    @param VOID

    @retval EFI_STATUS return the EFI status

**/

EFI_STATUS InitInterrupts(VOID)
{
    UINT32          i;
    UINT16          Segment;
    VOID            *InterruptHandler;
    EFI_STATUS      Status;


    //Allocate memory for addresses of interrupt Handlers.
    Status = gBS->AllocatePool(
        EfiACPIMemoryNVS,
        256 * sizeof(EFI_CPU_INTERRUPT_HANDLER), 
        (VOID*)&InterruptPtrTable
    );
    ASSERT_EFI_ERROR(Status);
    ZeroMem(InterruptPtrTable, 256 * sizeof(EFI_CPU_INTERRUPT_HANDLER)); //Clear addresses.

    //Allocate memory for addresses of interrupt Handlers.
    Status = gBS->AllocatePool(
        EfiACPIMemoryNVS,
        sizeof(*gIrqRuntimeHandler), 
        (VOID*)&gIrqRuntimeHandler
    );
    ASSERT_EFI_ERROR(Status);
    *gIrqRuntimeHandler = 0xcf48;	//IRET

    //InterruptHandler must be set to runtime address before copying interrupt handler.
    InterruptHandler = CpyToRunTime(InterruptHandlerStart, InterruptHandlerSize);
    *(VOID**)((UINT8*)InterruptHandler + InterruptHandlerTblFixup) = InterruptPtrTable;
    IntEntryTemplate.pJmpAddr = InterruptHandler;

    //Allocate interrupt entry points and copy.
    //Alocate space for Interrupt Descriptor Table. 256 entries/8 bytes.
    Status = gBS->AllocatePool(EfiACPIMemoryNVS, 256 * sizeof(INT_ENTRY_POINT), &gIntEntryPoints);
    ASSERT_EFI_ERROR(Status);

    for(i = 0; i < 256; ++i) {
        IntEntryTemplate.Interrupt = i;
        gIntEntryPoints[i] = IntEntryTemplate;
    }

    //Alocate space for Interrupt Descriptor Table. 256 entries/8 bytes.
    Status = gBS->AllocatePool(EfiACPIMemoryNVS, 256 * sizeof(*gIdt), &gIdt);
    ASSERT_EFI_ERROR(Status);

    for(i=0;i<=19;++i) EfiCpuRegisterInterruptHandler(0,i,InterruptHandlerHalt);
#if USE_AP_HLT    
    EfiCpuRegisterInterruptHandler(0, 2, NULL);                              // int 2
    Status = EfiCpuRegisterInterruptHandler(0, 2, InterruptHandlerRet);
    DEBUG((EFI_D_INFO, "%a L%d %r\n", __FUNCTION__, __LINE__, Status));
#endif    
	Segment = AsmReadCs();

    for (i = 0; i < 256; ++i)
    {
        UINTN IntHndlrAddr = (UINTN)&gIntEntryPoints[i];
        gIdt[i].LoOffset=*(UINT16*)&IntHndlrAddr;
        gIdt[i].HiOffset=*(((UINT16*)&IntHndlrAddr)+1);
#if USE_AP_HLT
        if(i == 2){
            UINTN AsmIretAddr = (UINTN)AsmIret;
            gIdt[i].LoOffset=*(UINT16*)&AsmIretAddr;
            gIdt[i].HiOffset=*(((UINT16*)&AsmIretAddr)+1);
        }
#endif        
        gIdt[i].Segment=Segment;
        gIdt[i].DescBits=0x8e00;  //Present=1, DPL = 0, D = 1 (32bit)
#ifdef EFIx64
        gIdt[i].Hi32Offset = 0;
        gIdt[i].Rsv = 0;
#endif

    }

    pIDT.Base=(UINT32)(UINTN)gIdt;

#ifdef EFIx64
    pIDT.Limit=16*256-1;
#else
    pIDT.Limit=8*256-1;
#endif

    return EFI_SUCCESS;
}


/**
    CPU exception

        
    @param Exception 
    @param Context 

         
    @retval VOID

**/
VOID InterruptHandlerHalt(
    IN EFI_EXCEPTION_TYPE Exception,
    IN EFI_SYSTEM_CONTEXT Context)
{
//    checkpoint((UINT8)Exception);
    DEBUG((EFI_D_ERROR,"ERROR: CPU Exception %X\n",Exception));
    CpuDeadLoop();
}

#if USE_AP_HLT
VOID InterruptHandlerRet(
    IN EFI_EXCEPTION_TYPE Exception,
    IN EFI_SYSTEM_CONTEXT Context)
{
    AsmIret();
}
#endif






/**
    This is an initialization routine for BSP and all APs.
    BSP has CPU number 0, and APs > 1. After init, BSP number may change.

        
    @param Cpu - number associated with CPU.
    @param Context 

         
    @retval VOID

**/
VOID InitializeCpu(
    IN UINT8          Cpu,
    IN volatile VOID  *Context
    )
{
  CPUINFO         *CpuInfo = &gCpuInfoData->Cpuinfo[Cpu];
  
  AsmWriteIdtr(&pIDT);      //load idt address for each cpu in memory

  // Need semaphore lock here for allocating memory
  WaitForSemaphore(&gCpuInfoSemaphore);
  
  CpuInfo->ApicId = *(UINT32*)(UINTN)(APIC_BASE + APIC_ID_REGISTER) >> 24;
  CpuInfo->Valid = TRUE;
  
  gCpuInfoSemaphore = 0;
}


EFI_CPU_ARCH_PROTOCOL gEfiCpuArchProtocol = {
    EfiCpuFlushDataCache,
    EfiCpuEnableInterrupt,
    EfiCpuDisableInterrupt,
    EfiCpuGetInterruptState,
    EfiCpuINIT,
    EfiCpuRegisterInterruptHandler,
    EfiCpuGetTimerValue,
    EfiCpuSetMemoryAttributes,
    1,
    32
};

void UpdateMyMpServiceProtocol();


/**
    Install Interrupt Handlers for 0 to 19, to the routine

        
    @param ImageHandle 
    @param SystemTable 

         
    @retval EFI_STATUS return the EFI status

**/

EFI_STATUS DxeInitializeCpu(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
    )
{
    EFI_STATUS          Status;
    EFI_HANDLE          Handle = 0;
    VOID                *RuntimeGdt;
    EFI_EVENT           BootEvent;
    VOID                *pRegistration = NULL;
    UINT32              Attributes = 0;
    UINT32              Address32 = 0;
    UINT32              Buffer32 = 0;
    UINTN               i;
    EFI_HOB_GUID_TYPE   *GuidHob;


    GuidHob = GetFirstGuidHob(&gEfiCpuInfoHobGuid);
    gCpuInfoData = (CPUINFO_HOB_DATA*)GET_GUID_HOB_DATA(GuidHob);
  
    Status = InitInterrupts();
    ASSERT_EFI_ERROR(Status);

    RuntimeGdt = CpyToRunTime(GDT, sizeof(GDT));
    pGdt.Limit = sizeof(GDT) - 1;
    pGdt.Base = (UINTN)RuntimeGdt;
    AsmWriteGdtr(&pGdt);

    Status =  EfiCpuRegisterInterruptHandler(NULL, 13, NULL);                    // # GP
    ASSERT_EFI_ERROR(Status);
    Status = EfiCpuRegisterInterruptHandler(0, 13, TempGPInterruptHandler);
    ASSERT_EFI_ERROR(Status);

    gNumCpus = StartAllAps(&gMpData);
    ASSERT(gNumCpus <= gCpuInfoData->CpuCount);

    gCpuInfoData->CpuCount = gNumCpus;
    
    for (i = 0; i < gNumCpus; i++) {
      gCpuInfoData->Cpuinfo[i].Valid = TRUE;
    }

    ExecuteFunctionOnRunningCpus(gMpData, TRUE, TRUE, InitializeCpu, NULL);

    Status = EfiCpuRegisterInterruptHandler(NULL, 13, NULL);
    ASSERT_EFI_ERROR(Status);
    Status = EfiCpuRegisterInterruptHandler(0, 13, InterruptHandlerHalt);	//Normal Handler.
    ASSERT_EFI_ERROR(Status);

    Status = gBS->CreateEvent(
                    EVT_SIGNAL_EXIT_BOOT_SERVICES,
                    TPL_CALLBACK,
                    BootNotify,
                    NULL,
                    &BootEvent
                    );
    ASSERT_EFI_ERROR(Status);
    
//In cases where core does not support a call back before option roms are executed.
//The calling outside of SMM will be disabled before shell or a OS is loaded.
    Status = EfiCreateEventReadyToBootEx (
               TPL_CALLBACK,
               ExecuteAtReadyToBoot,
               NULL,
               &BootEvent
               );
    ASSERT_EFI_ERROR(Status);

    Status = EfiCreateEventLegacyBootEx(
                TPL_CALLBACK,
                BootNotify,
                NULL, 
                &BootEvent
                );

    Status = gBS->AllocatePool(EfiACPIMemoryNVS, sizeof (MTRR_SETTINGS), &mMtrrTable);
    
    Status = gBS->InstallMultipleProtocolInterfaces(
                    &Handle,
                    &gEfiCpuArchProtocolGuid,	&gEfiCpuArchProtocol,
                    &gEfiMpServiceProtocolGuid,	gEfiMpServicesProtocol,
                    &gByoBdsEnterHookGuid, UpdateMyMpServiceProtocol,
                    NULL
                    );
    ASSERT_EFI_ERROR(Status);

    return EFI_SUCCESS;

}

