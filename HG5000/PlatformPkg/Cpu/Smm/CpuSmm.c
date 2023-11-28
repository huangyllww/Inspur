
#include <token.h>
#include "CpuSmm.h"
#include <Base.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/DebugAgentLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>


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
CPU_SMM_I               gCoreDataI[FixedPcdGet32(PcdCpuMaxCores)];
CPU_SMM_I               *pCoreDataI = &gCoreDataI[0];
SMM_AP_CPU_CONTROL      gApCpuControl[FixedPcdGet32(PcdCpuMaxCores)];
UINT32                  MaxCores = FixedPcdGet32(PcdCpuMaxCores);


EFI_SMM_CPU_PROTOCOL    SmmCpuProtocol = {
    SmmReadSaveState,
    SmmWriteSaveState
};

UINT64  OldTsegBase;
UINT64  OldTsegMask;


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


    gAcpiTimerIoBase = PcdGet16(PcdAmdFchCfgAcpiPmTmrBlkAddr);

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
    ASSERT(CpuInfoData->CpuCount <= MaxCores);

    // init Cores data, Smi entry code will begin from Buffer start
    gCpuSmmInt.NumCpus = CpuInfoData->CpuCount;
    for( i = 0; i < MaxCores; i++ ){
      pCoreDataI[i].ApicId = CpuInfoData->Cpuinfo[i].ApicId;
      pCoreDataI[i].SmmBase =(UINTN) (UINT8*)Buffer + i*SMM_ENTRY_CODE_OFFS;
      gCpuSmmInt.InSmm[i] = FALSE;
//      if(i < gCpuSmmInt.NumCpus){
//        DEBUG((EFI_D_INFO, "[%d] ApicId:%d SmmBase:%X\n", i, pCoreDataI[i].ApicId, pCoreDataI[i].SmmBase));
//      }
    }

    // Stack
    // AP Stacks start from Buffer +0x10000 -> APSTACK_AREA_START    
    for( i = 1; i < MaxCores; i++ ){
        pCoreDataI[i].StackTop =(UINTN) (UINT8*)Buffer + i*APSTACK_OFFS + APSTACK_AREA_START;
    }

    // BSP Stack
    pCoreDataI[0].StackTop =(UINTN) (UINT8*)Buffer + BSP_STACK_OFFS + APSTACK_AREA_START;
    

    // Init SMM Page Table
    Status = SmmPageTableInit();
    if (EFI_ERROR(Status)) return Status;
    
    
    // Init IDT and handlers
    FillIdt();
    
    
    // SMM Entry code
    
    for( i = 0; i < MaxCores; i++ ){
        SmmEntry = (UINT8*)Buffer + i*SMM_ENTRY_CODE_OFFS;
        CopyMem(SmmEntry, SmmEntryStart, gSmmEntrySize);
        // init SmmEntry variables
       *(UINT64*)(UINT8*)(SmmEntry + gSmmStackOffs) = pCoreDataI[i].StackTop;
       *(UINT32*)(UINT8*)(SmmEntry + gPageTableOffs) = (UINT32)gCpuSmmInt.SmmPageTableStart;

       *(UINT64*)(UINT8*)(SmmEntry + gSmmIdtBaseOffs) = (UINT64)&gIdtStart;
       // adjust offsets for far jumps
       *(UINT32*)(UINT8*)(SmmEntry + gProt32JmpOffs) = (UINT32)(UINT8*)(SmmEntry + gProt32JmpOffs + 6);
       *(UINT32*)(UINT8*)(SmmEntry + gLModeJmpOffs) = (UINT32)(UINT8*)(SmmEntry + gLModeJmpOffs + 6);
    }
    
    // Init global vars
    gCpuSmmInt.fNumInSmm = 0;
    gCpuSmmInt.BspIndex = (UINT32)-1;
    gCpuSmmInt.InsideSmm = FALSE;
    
    ZeroMem(&gApCpuControl, sizeof(SMM_AP_CPU_CONTROL) * MaxCores);
    
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

	return	Status;
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


