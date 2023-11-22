
#include "CpuSmm.h"
#include <Base.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/DebugAgentLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/AcpiS3Context.h>
#include <Library/ReportStatusCodeLib.h>


#define SMM_TIME_OUT_US     0x1388
#define AP_SMM_TIME_OUT_US  0x1388


EFI_HOB_CPU                     *gEfiCpuHob;
EFI_SMM_ENTRY_CONTEXT           gCpuSmmEntryContext;
EFI_SMM_CONFIGURATION_PROTOCOL  gSmmConfiguration;
EFI_SMM_RESERVED_SMRAM_REGION   CpuReservedSmramNull;
EFI_HANDLE                      CpuSmmProtocolHandle = NULL;
EFI_MP_SERVICES_PROTOCOL        *MpServices = NULL;
volatile UINTN                  RetFromSmmFlag;

UINT16                  gAcpiTimerIoBase;
CPU_SMM_PRIVATE_DATA    gCpuSmmInt;
CPU_SMM_I               gCoreDataI[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];
CPU_SMM_I               *pCoreDataI = &gCoreDataI[0];
SMM_AP_CPU_CONTROL      gApCpuControl[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];


EFI_SMM_CPU_PROTOCOL    SmmCpuProtocol = {
    SmmReadSaveState,
    SmmWriteSaveState
};

UINT64  OldTsegBase;
UINT64  OldTsegMask;
SMM_S3_RESUME_STATE          *mSmmS3ResumeState = NULL;
BOOLEAN                      gIsS3 = FALSE;

/**
    Structure to keep SMM timeout data.
 */
typedef struct {
    UINT32  OverFlow;
    UINT32  EndValue;
    UINT32  OldTimerValue;
} SMM_TIMEOUT;

/**
    Defines ACPI timer width in bits
 */
#define NUM_BITS_IN_ACPI_TIMER      32


/**
  Initialize timeout for a specified amount of time in uS.

   @param Timeout - Pointer to structure, data modified by function.
   @param Time - Timeout value in uS.
   @param Status - Status returned by function.

   @return EFI_STATUS
   @retval  EFI_SUCCESS - Always return success.
*/

EFI_STATUS
StartTimeout(
  IN SMM_TIMEOUT *Timeout,
  IN UINT32 Time )
{
    UINT64  TicksNeeded;
    UINT64  EndValue;

    // There are 3.58 ticks per uS.
    // TicksNeeded = Time * 358 / 100
    TicksNeeded = DivU64x32 (MultU64x32 (Time, 358), 100);

    // Read ACPI Timer
    Timeout->OldTimerValue = IoRead32(gAcpiTimerIoBase);
    EndValue = TicksNeeded + Timeout->OldTimerValue;

    // Calculate Overflow and EndValue from FullEndValue,
    // based on number of bits in ACPI Timer
    Timeout->OverFlow = (UINT32)RShiftU64(EndValue, NUM_BITS_IN_ACPI_TIMER);
    Timeout->EndValue = ((UINT32)EndValue) & (UINT32)(((UINT64)1 << NUM_BITS_IN_ACPI_TIMER) - 1);

    return EFI_SUCCESS;
}

/**
  Checks if timeout has expired.

   @param Timeout - Pointer to structure, data modified by function.
   @param Status - Status returned by function.

   @return EFI_STATUS
   @retval  EFI_SUCCESS - Timer not expired.
   @retval  EFI_TIMEOUT - Timer expired.
*/

EFI_STATUS
HasItTimedOut(
  IN SMM_TIMEOUT *Timeout)
{
    UINT32 TimerValue;

    // Read ACPI Timer
    TimerValue = IoRead32(gAcpiTimerIoBase);

    if (Timeout->OverFlow > 0) {
        // See if the current timer value is less than the previous value.
        // If it is, then the timer had wrapped around.
        if (TimerValue < Timeout->OldTimerValue) {
            --Timeout->OverFlow;
        }

        // Update OldTimerValue
        Timeout->OldTimerValue = TimerValue;
        return EFI_SUCCESS;
    }

    // See if the current timer value is less than the previous value.
    // If it is, then we are done.
    if (TimerValue < Timeout->OldTimerValue) return EFI_TIMEOUT;

    // If we passed the EndValue, we are done.
    if (TimerValue >= Timeout->EndValue) return EFI_TIMEOUT;

    // Update OldTimerValue
    Timeout->OldTimerValue = TimerValue;

    return EFI_SUCCESS;
}


void CloseAndLockSmram()
{
  UINT64   Data64;

  Data64 = AsmReadMsr64(HYGON_MSR_SMM_MASK_HL);
  Data64 |= 3;
  AsmWriteMsr64(HYGON_MSR_SMM_MASK_HL, Data64);

  Data64 = AsmReadMsr64(HYGON_MSR_HWCR);
  Data64 |= BIT0;
  AsmWriteMsr64(HYGON_MSR_HWCR, Data64); 

//DEBUG((EFI_D_INFO, "HYGON_MSR_SMM_ADDR_HL:%lX\n", AsmReadMsr64(HYGON_MSR_SMM_ADDR_HL)));
//DEBUG((EFI_D_INFO, "HYGON_MSR_SMM_MASK_HL:%lX\n", AsmReadMsr64(HYGON_MSR_SMM_MASK_HL)));
//DEBUG((EFI_D_INFO, "HYGON_MSR_HWCR       :%lX\n", AsmReadMsr64(HYGON_MSR_HWCR)));  
}


/**
  Perform SMM initialization for all processors in the S3 boot path.

  For a native platform, MP initialization in the S3 boot path is also performed in this function.
**/
VOID
EFIAPI
SmmRestoreCpu (
  VOID
  )
{
  SMM_S3_RESUME_STATE           *SmmS3ResumeState;


  DEBUG ((EFI_D_INFO, "SmmRestoreCpu()\n"));

  if (mSmmS3ResumeState == NULL) {
    DEBUG ((EFI_D_ERROR, "No context to return to PEI Phase\n"));
    CpuDeadLoop ();
  }

  SmmS3ResumeState = mSmmS3ResumeState;

  DEBUG (( EFI_D_INFO, "SMM S3 Return CS                = %x\n", SmmS3ResumeState->ReturnCs));
  DEBUG (( EFI_D_INFO, "SMM S3 Return Entry Point       = %x\n", SmmS3ResumeState->ReturnEntryPoint));
  DEBUG (( EFI_D_INFO, "SMM S3 Return Context1          = %x\n", SmmS3ResumeState->ReturnContext1));
  DEBUG (( EFI_D_INFO, "SMM S3 Return Context2          = %x\n", SmmS3ResumeState->ReturnContext2));
  DEBUG (( EFI_D_INFO, "SMM S3 Return Stack Pointer     = %x\n", SmmS3ResumeState->ReturnStackPointer));

  gIsS3 = TRUE;

  //
  // If SMM is in 64-bit mode, then use AsmDisablePaging64() to resume PEI Phase
  //
  if (SmmS3ResumeState->Signature == SMM_S3_RESUME_SMM_64) {
    DEBUG ((EFI_D_INFO, "Call AsmDisablePaging64() to return to S3 Resume in PEI Phase\n"));
    AsmDisablePaging64 (
      SmmS3ResumeState->ReturnCs,
      (UINT32)SmmS3ResumeState->ReturnEntryPoint,
      (UINT32)SmmS3ResumeState->ReturnContext1,
      (UINT32)SmmS3ResumeState->ReturnContext2,
      (UINT32)SmmS3ResumeState->ReturnStackPointer
      );
  }

  //
  // Can not resume PEI Phase
  //
  DEBUG ((EFI_D_ERROR, "No context to return to PEI Phase\n"));
  CpuDeadLoop ();
}




VOID
InitSmmS3ResumeState (
    VOID
  )
{
  VOID                       *GuidHob;
  EFI_SMRAM_DESCRIPTOR       *SmramDescriptor;
  SMM_S3_RESUME_STATE        *SmmS3ResumeState;


  if (!PcdGetBool(PcdAcpiS3Enable)) {
    return;
  }

  GuidHob = GetFirstGuidHob (&gEfiAcpiVariableGuid);
  if (GuidHob == NULL) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR:%a(): HOB(gEfiAcpiVariableGuid=%g) needed by S3 resume doesn't exist!\n",
      __FUNCTION__,
      &gEfiAcpiVariableGuid
    ));
    CpuDeadLoop ();
  } else {
    SmramDescriptor = (EFI_SMRAM_DESCRIPTOR *) GET_GUID_HOB_DATA (GuidHob);

    DEBUG ((EFI_D_INFO, "SMM S3 SMRAM Structure = %x\n", SmramDescriptor));
    DEBUG ((EFI_D_INFO, "SMM S3 Structure = %x\n", SmramDescriptor->CpuStart));

    SmmS3ResumeState = (SMM_S3_RESUME_STATE *)(UINTN)SmramDescriptor->CpuStart;
    ZeroMem (SmmS3ResumeState, sizeof (SMM_S3_RESUME_STATE));

    mSmmS3ResumeState = SmmS3ResumeState;
    SmmS3ResumeState->Smst = (EFI_PHYSICAL_ADDRESS)(UINTN)gSmst;

    SmmS3ResumeState->SmmS3ResumeEntryPoint = (EFI_PHYSICAL_ADDRESS)(UINTN)SmmRestoreCpu;

    SmmS3ResumeState->SmmS3StackSize = SIZE_32KB;
    SmmS3ResumeState->SmmS3StackBase = (EFI_PHYSICAL_ADDRESS)(UINTN)AllocatePages (EFI_SIZE_TO_PAGES ((UINTN)SmmS3ResumeState->SmmS3StackSize));
    if (SmmS3ResumeState->SmmS3StackBase == 0) {
      SmmS3ResumeState->SmmS3StackSize = 0;
    }

    SmmS3ResumeState->SmmS3Cr0 = (UINT32)AsmReadCr0();
    SmmS3ResumeState->SmmS3Cr3 = (UINT32)(UINTN)gCpuSmmInt.SmmPageTableStart;
    SmmS3ResumeState->SmmS3Cr4 = (UINT32)AsmReadCr4();

    if (sizeof (UINTN) == sizeof (UINT64)) {
      SmmS3ResumeState->Signature = SMM_S3_RESUME_SMM_64;
    }
    if (sizeof (UINTN) == sizeof (UINT32)) {
      SmmS3ResumeState->Signature = SMM_S3_RESUME_SMM_32;
    }
  }

}






/**
  Init CPU SMM entries, creates IDT and Page table, relocates SMM base,
   and installs protocols.

   @param ImageHandle - Image Handle.
   @param SystemTable - pointer to system table.
   @param Status - Status returned by function.

   @return EFI_STATUS
   @retval  SUCCESS - Init complete, protocol installed.
   @retval EFI_ERROR - Initialization failed
*/

EFI_STATUS
EFIAPI
CpuSmmInit(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS                  Status;
    VOID                        *Buffer;
    UINT32                      i;
    UINT8                       *SmmEntry;
    EFI_HOB_GUID_TYPE           *GuidHob;
    CPUINFO_HOB_DATA            *CpuInfoData;


  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_SMM_INIT
    );

    gAcpiTimerIoBase = PcdGet16(PcdHygonFchCfgAcpiPmTmrBlkAddr);

    Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, (VOID **)&MpServices);
    ASSERT_EFI_ERROR (Status);

    GuidHob = GetFirstGuidHob(&gEfiCpuInfoHobGuid);
    CpuInfoData = (CPUINFO_HOB_DATA*)GET_GUID_HOB_DATA(GuidHob);

    gEfiCpuHob = GetNextHob(EFI_HOB_TYPE_CPU, GetHobList());
    if (gEfiCpuHob == NULL) {
      DEBUG((EFI_D_ERROR, "CpuHob not found!\n"));
      return Status;
    }

    // Allocating 4 MB for buffer
    Buffer = AllocatePages (EFI_SIZE_TO_PAGES(0x400000));
    ASSERT(CpuInfoData->CpuCount <= FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber));

    DEBUG((EFI_D_VERBOSE, "Buffer:%x, gSmmEntrySize:%d\n", Buffer, gSmmEntrySize));
    ASSERT(gSmmEntrySize <= 512);

    // init Cores data, Smi entry code will begin from Buffer start
    gCpuSmmInt.NumCpus = CpuInfoData->CpuCount;
    for( i = 0; i < CpuInfoData->CpuCount; i++ ){
      pCoreDataI[i].ApicId   = CpuInfoData->Cpuinfo[i].ApicId;
      pCoreDataI[i].SmmBase  =(UINTN)Buffer + i*SMM_ENTRY_CODE_OFFS;
      pCoreDataI[i].StackTop =(UINTN)Buffer + i*APSTACK_OFFS + APSTACK_AREA_START;
      gCpuSmmInt.InSmm[i] = FALSE;
    }

    // Init SMM Page Table
    Status = SmmPageTableInit();
    if (EFI_ERROR(Status)) return Status;

    // Init IDT and handlers
    FillIdt();

    // SMM Entry code

    for( i = 0; i < gCpuSmmInt.NumCpus; i++ ){
        SmmEntry = (UINT8*)pCoreDataI[i].SmmBase + SMM_HANDLER_OFFSET;

        DEBUG((EFI_D_VERBOSE, "[%03d] SmmEntry:%x StackTop:%x\n", i, SmmEntry, pCoreDataI[i].StackTop));
        
        CopyMem(SmmEntry, SmmEntryStart, gSmmEntrySize);
        // init SmmEntry variables
       *(UINT64*)(UINTN)(SmmEntry + gSmmStackOffs) = pCoreDataI[i].StackTop;
       *(UINT32*)(UINTN)(SmmEntry + gPageTableOffs) = (UINT32)(UINTN)gCpuSmmInt.SmmPageTableStart;

       *(UINT64*)(UINTN)(SmmEntry + gSmmIdtBaseOffs) = (UINT64)(UINTN)&gIdtStart;
       // adjust offsets for far jumps
       *(UINT32*)(UINTN)(SmmEntry + gProt32JmpOffs) = (UINT32)(UINTN)(SmmEntry + gProt32JmpOffs + 6);
       *(UINT32*)(UINTN)(SmmEntry + gLModeJmpOffs)  = (UINT32)(UINTN)(SmmEntry + gLModeJmpOffs + 6);
    }

    //
    // Initialize Debug Agent to start source level debug in SMM code
    //
    InitializeDebugAgent (DEBUG_AGENT_INIT_SMM, NULL, NULL);
    

    // Init global vars
    gCpuSmmInt.fNumInSmm = 0;
    gCpuSmmInt.BspIndex = (UINT32)-1;
    gCpuSmmInt.InsideSmm = FALSE;

    ZeroMem(gApCpuControl, sizeof(gApCpuControl));

    // Init context for Foundation SMM
    gCpuSmmEntryContext.SmmStartupThisAp = SmmStartupThisAp;
    gCpuSmmEntryContext.NumberOfCpus = gCpuSmmInt.NumCpus;
    gCpuSmmEntryContext.CurrentlyExecutingCpu = 0; // Hardcoding to BSP only
    gCpuSmmEntryContext.CpuSaveStateSize = NULL; // !TODO
    gCpuSmmEntryContext.CpuSaveState = NULL; // !TODO


    // Relocate all cores  from default SMM base to new one
    SmmRelocate();


    // Install SMM Configuration protocol
    CpuReservedSmramNull.SmramReservedStart = 0;
    CpuReservedSmramNull.SmramReservedSize = 0;
    gSmmConfiguration.SmramReservedRegions = &CpuReservedSmramNull;
    gSmmConfiguration.RegisterSmmEntry = RegisterSmmEntry;

    gCpuSmmInt.CpuHandle = NULL;
    Status = SystemTable->BootServices->InstallMultipleProtocolInterfaces (
                                        &gCpuSmmInt.CpuHandle,
                                        &gEfiSmmConfigurationProtocolGuid, &gSmmConfiguration,
                                        NULL
                                        );
    ASSERT_EFI_ERROR (Status);

    // Install SMM CPU Protocol
    Status = gSmst->SmmInstallProtocolInterface (
                    &CpuSmmProtocolHandle,
                    &gEfiSmmCpuProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &SmmCpuProtocol
                    );
    ASSERT_EFI_ERROR (Status);

  InitSmmS3ResumeState();

  return  Status;
}


/**
  SMM handler, invoked from SmmEntry,

   @param VOID.

   @return VOID
*/

VOID
EFIAPI
SmmHandler(
    VOID
)
{
    UINT32  ApicId;
    UINT32  Index;

    gCpuSmmInt.fSmmDone = 0;
    InterlockedIncrement(&gCpuSmmInt.fNumInSmm);

    if(gIsS3){
      CloseAndLockSmram();
    }

    ApicId = GetApicId();
    for(Index = 0; Index < gCpuSmmInt.NumCpus; Index++){
        if(ApicId == pCoreDataI[Index].ApicId)
        {
            gCpuSmmInt.InSmm[Index] = TRUE;
            break;
        }
    }
    if(Index) {
        ApSmmHandler(Index);
    }else {
        //
        // Initialize Debug Agent to start source level debug in SMM code
        //
        InitializeDebugAgent (DEBUG_AGENT_INIT_ENTER_SMI, NULL, NULL);

        BspSmmHandler();

        //
        // Stop source level debug in SMM code, the code below will not be
        // debugged.
        //
        InitializeDebugAgent (DEBUG_AGENT_INIT_EXIT_SMI, NULL, NULL);
    }

    gCpuSmmInt.InSmm[Index] = FALSE;
    while(gCpuSmmInt.fNumInSmm > 0);

    if(Index == 0 && gIsS3){
      gIsS3 = FALSE;
    }
}

/**
  SMM handler for BSP,

   @param VOID.

   @return VOID
*/

VOID
EFIAPI
BspSmmHandler(
  VOID
)
{

    SMM_TIMEOUT     Timeout;
    UINT32          Index;

    // wait for APs
    // At S3 resume fNumInSmm keeps old data
    StartTimeout(&Timeout, SMM_TIME_OUT_US);

    gCpuSmmInt.InsideSmm = TRUE;

    while(gCpuSmmInt.NumCpus > gCpuSmmInt.fNumInSmm && !EFI_ERROR(HasItTimedOut(&Timeout)))
    {
        CpuPause ();
    }

    if (gCpuSmmInt.fNumInSmm < gCpuSmmInt.NumCpus)
    {
        //
        // Send SMI IPIs to bring outside processors in
        //
        for (Index = 0; Index < gCpuSmmInt.NumCpus; Index++)
        {
            if (!gCpuSmmInt.InSmm[Index])
            {
                SendSmiIpi ((UINT32)pCoreDataI[Index].ApicId);
            }
        }

        StartTimeout(&Timeout, SMM_TIME_OUT_US);
        while(gCpuSmmInt.NumCpus > gCpuSmmInt.fNumInSmm && !EFI_ERROR(HasItTimedOut(&Timeout)) )
        {
            CpuPause ();
        }
    }

    if(gCpuSmmInt.NumCpus < gCpuSmmInt.fNumInSmm) gCpuSmmInt.fNumInSmm = gCpuSmmInt.NumCpus;

    // Invoke SMM Foundation entry point
    if(gCpuSmmInt.CoreSmmEntryPoint)
    gCpuSmmInt.CoreSmmEntryPoint(&gCpuSmmEntryContext);

    // Clear SMI
    //debug_Ram ClearSmi();
    ClearSmi();

    // Release Aps and wait
    gCpuSmmInt.fSmmDone--;
    gCpuSmmInt.InsideSmm = FALSE;

    while(gCpuSmmInt.fNumInSmm != 1)
    {
        if (!gCpuSmmInt.fSmmDone)
        {
            gCpuSmmInt.fSmmDone--;
        }
    }

    InterlockedDecrement(&gCpuSmmInt.fNumInSmm);
}

/**
  SMM handler for AP,

   @param Index - AP number.

   @return VOID
*/

VOID
EFIAPI
ApSmmHandler(
  IN UINT32   Index
)
{

    SMM_TIMEOUT     Timeout;

    StartTimeout(&Timeout, AP_SMM_TIME_OUT_US);
    // Timeout BSP
    //
    while(!gCpuSmmInt.InsideSmm && !EFI_ERROR(HasItTimedOut(&Timeout)))
    {
        CpuPause ();
    }
    if (!gCpuSmmInt.InsideSmm)
    {
        //
        // BSP timeout in the first round
        //
        //
        // Send SMI IPI to bring BSP in
        //
        SendSmiIpi ((UINT32)pCoreDataI[0].ApicId);

        //
        // Now clock BSP for the 2nd time
        //
      StartTimeout(&Timeout, AP_SMM_TIME_OUT_US);
      // Timeout BSP
      //
      while(!gCpuSmmInt.InsideSmm && !EFI_ERROR(HasItTimedOut(&Timeout)))
      {
          CpuPause ();
      }
    }

    while(!gCpuSmmInt.fSmmDone){
        if(gCpuSmmInt.NumCpus < gCpuSmmInt.fNumInSmm) gCpuSmmInt.fNumInSmm = gCpuSmmInt.NumCpus;
        // check if there procedure to execute
        if(gApCpuControl[Index].Procedure != 0){
            gApCpuControl[Index].Procedure(gApCpuControl[Index].ProcArguments);
            gApCpuControl[Index].Procedure = 0;
        }
    }

    InterlockedDecrement(&gCpuSmmInt.fNumInSmm);
}


/**
  Starts procedure on required AP.

   @param Procedure - Function to execute.
   @param CpuNumber - Number of CPU to start.
   @param ProcArguments - optional pointer to arguments.
   @param Status - Status returned by function.

   @return EFI_STATUS
   @retval  EFI_INVALID_PARAMETER - function not implemented.
*/

EFI_STATUS
EFIAPI
SmmStartupThisAp (
  IN      EFI_AP_PROCEDURE          Procedure,
  IN      UINTN                     CpuNumber,
  IN OUT  VOID                      *ProcArguments OPTIONAL
)
{
    if(Procedure == NULL)   return  EFI_INVALID_PARAMETER;
    if((CpuNumber == 0)||(CpuNumber >= gCpuSmmInt.NumCpus)) return  EFI_INVALID_PARAMETER;

    gApCpuControl[CpuNumber].ProcArguments = ProcArguments;
    gApCpuControl[CpuNumber].Procedure = (SMM_AP_PROCEDURE)Procedure;
    // Blocking cycle below. Better have some timeout.[TODO]
    while(gApCpuControl[CpuNumber].Procedure);
    return  EFI_SUCCESS;
}

VOID
ApSetFlag(
        VOID )
{
    RetFromSmmFlag = 1;
}

VOID
WaitRetFromSmm(
  IN UINT32 Cpu )
{
    EFI_STATUS  Status;

    RetFromSmmFlag = 0;
    Status = MpServices->StartupThisAP(
            MpServices,
            (EFI_AP_PROCEDURE)ApSetFlag,
            (UINTN)Cpu,
            NULL,
            0,
            NULL,
            NULL
            );
    if(Status == EFI_SUCCESS){
        while(RetFromSmmFlag == 0){
            CpuPause();
        }
    }
}

/**
  Relocates SMM Base from default to new at TSeg,

   @param VOID.

   @return VOID
*/

VOID
EFIAPI
SmmRelocate(
    VOID
)
{
            UINT8   *SmmBackup;
            UINT8   *SmmEntry = (UINT8*)(UINTN)SMM_DEFAULT_ENTRY;
volatile    UINT32  *RelocFlag = (UINT32*)(UINT8*)(SmmEntry + gRelocDoneOffs);
            UINT32  i;


    DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

    // save 0x38000..0x3FFFF memory
    SmmBackup = AllocatePool (BACKUP_SMM_SIZE);
    CopyMem(SmmBackup, SmmEntry, BACKUP_SMM_SIZE);

    // copy relocation code
    CopyMem(SmmEntry, SmmRelocStart, gSmmRelocSize);

    // Do APs
    for(i=1; i < gCpuSmmInt.NumCpus; i++){
        // init variables for relocation code
        *RelocFlag = 0;
        *(UINT32*)(SmmEntry + gNewSmmBaseOffs) = (UINT32)pCoreDataI[i].SmmBase;
        SendSmiIpi(pCoreDataI[i].ApicId);
        while(!*RelocFlag){
            CpuPause();
        }
        WaitRetFromSmm(i);
    }
    // Do BSP
    // init variables for relocation code
    *(UINT32*)(SmmEntry + gNewSmmBaseOffs) = (UINT32)pCoreDataI[0].SmmBase;

    SendSmiIpi (pCoreDataI[0].ApicId);

    // restore memory
    CopyMem( SmmEntry, SmmBackup, BACKUP_SMM_SIZE);
    FreePool(SmmBackup);
}


/**
  Save SMM Foundation Entry point, to call it from SMI handler.

   @param This - Pointer to EFI_SMM_CONFIGURATION_PROTOCOL instance.
   @param SmmEntryPoint - SMM Foundation Entry point.
   @param Status - Status returned by function.

   @return EFI_STATUS
   @retval  SUCCESS - Always return success.
*/

EFI_STATUS
EFIAPI
RegisterSmmEntry (
  IN CONST EFI_SMM_CONFIGURATION_PROTOCOL  *This,
  IN EFI_SMM_ENTRY_POINT                   SmmEntryPoint
)
{

    // Save SMM Foundation Entry point, to call it from SMI handler
    gCpuSmmInt.CoreSmmEntryPoint = SmmEntryPoint;
    return EFI_SUCCESS;
}


