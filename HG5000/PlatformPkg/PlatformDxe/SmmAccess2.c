

#include <Uefi.h>
#include <AmdCpu.h>
#include <Library/HobLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/MpService.h>
#include <Protocol/SmmAccess2.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/MtrrLib.h>


EFI_SMRAM_DESCRIPTOR gSmramMap[] = {
    {
    0,                                  //PhysicalStart <--To be filled in.
    0,                                  //CputStart
    1024 * 1024,                        //PhysicalSize < - Will be filled in.
    EFI_SMRAM_CLOSED                    //RegionState
    }
};

#define SMRAM_MAP_NUM_DESCRIPTORS (sizeof(gSmramMap)/sizeof(EFI_SMRAM_DESCRIPTOR))


STATIC PLATFORM_COMM_INFO *gPlatCommInfo = NULL;


// MSRC001_0113 [SMM TSeg Mask] (Core::X86::Msr::SMMMask)
//   [47:17] TSegMask[47:17]: TSeg address range mask.
//   [16:15] Reserved
//   [14:12] TMTypeDram: TSeg address range memory type. 
//   [10:8]  AMTypeDram: ASeg Range Memory Type
//   [5]     TMTypeIoWc: non-SMM TSeg address range memory type
//   [4]     AMTypeIoWc: non-SMM ASeg address range memory type
//   [3]     TClose: send TSeg address range data accesses to MMIO
//   [2]     AClose: send ASeg address range data accesses to MMIO
//   [1]     TValid: enable TSeg SMM address range
//   [0]     AValid: enable ASeg SMM address range.
//

// CPUAddr[47:17] & TSegMask[47:17] == TSegBase[47:17] & TSegMask[47:17]
//
STATIC
EFI_STATUS
NBSMM_ProgramTsegBase (
    VOID
  )
{
  UINT64    TsegAddress;
   
  AsmWriteMsr64 (AMD_MSR_SMM_ADDR_HL, gPlatCommInfo->TsegBase);

  TsegAddress = AsmReadMsr64 (AMD_MSR_SMM_MASK_HL);
  TsegAddress &= 0xFFFF000000018FFFULL;
  TsegAddress |= (~(UINT64)(gPlatCommInfo->TsegSize - 1)) & 0xFFFFFFFE0000;
  TsegAddress |= CacheWriteBack << 12;
  AsmWriteMsr64 (AMD_MSR_SMM_MASK_HL, TsegAddress);

  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
NBSMM_EnableSMMAddress(VOID)
{
    EFI_STATUS                  Status;
    EFI_MP_SERVICES_PROTOCOL    *MpServices;
    

    gPlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo(); 
    DEBUG((EFI_D_INFO, "TSEG(%X,%X)\n", gPlatCommInfo->TsegBase, gPlatCommInfo->TsegSize));

    gSmramMap[0].PhysicalStart = gPlatCommInfo->TsegBase;
    gSmramMap[0].CpuStart      = gPlatCommInfo->TsegBase;
    gSmramMap[0].PhysicalSize	 = gPlatCommInfo->TsegSize;

    NBSMM_ProgramTsegBase();

    // Execute on running APs
    Status = gBS->LocateProtocol(&gEfiMpServiceProtocolGuid, NULL, &MpServices);
    ASSERT_EFI_ERROR(Status);

    MpServices->StartupAllAPs(
        MpServices,                                 // EFI_MP_SERVICES_PROTOCOL*
        (EFI_AP_PROCEDURE)NBSMM_ProgramTsegBase,    // EFI_AP_PROCEDURE
        FALSE,                                      // BOOLEAN SingleThread
        NULL,                                       // EFI_EVENT WaitEvent
        (UINTN)NULL,                                // UINTN Timeout
        (VOID*)NULL,                                // VOID *ProcArguments
        NULL);   


    return EFI_SUCCESS;

}


STATIC
EFI_STATUS
ApSetSmram(
    IN  VOID*   pSmramState
)
{
    UINT64  MtrrVal;

    if (AsmReadMsr64(AMD_MSR_HWCR) & 0x01)
    return EFI_ACCESS_DENIED;   //If device is locked, return error.

        // Enable RDMem & WRMem bits visible in fixed MTRR
    MtrrVal = AsmReadMsr64 (MSR_SYS_CFG);
    MtrrVal |= ((UINT32)1 << MTRRFixDRAMModEnBit);
    AsmWriteMsr64 (MSR_SYS_CFG, MtrrVal);

    if(*(UINT64*)pSmramState == EFI_SMRAM_OPEN){
        // Enable 0xA0000 read/write
        AsmWriteMsr64 (MTRR_FIX_16K_A0000, 0x1C1C1C1C1C1C1C1C);
    }else if(*(UINT64*)pSmramState == EFI_SMRAM_CLOSED){
        // Disable 0xA0000 read/write
        AsmWriteMsr64 (MTRR_FIX_16K_A0000, 0x0404040404040404);
    }

    // Disable RDMem & WRMem bits visible in fixed MTRR
    MtrrVal = AsmReadMsr64 (MSR_SYS_CFG);
    MtrrVal &= ~((UINT64)1 << MTRRFixDRAMModEnBit);
    AsmWriteMsr64 (MSR_SYS_CFG, MtrrVal);

    // Disable/Enable ASEG & TSEG SMRAM for VGA access
    MtrrVal = AsmReadMsr64 (AMD_MSR_SMM_MASK_HL);
    if(*(UINT64*)pSmramState == EFI_SMRAM_OPEN){
        MtrrVal &= ~((UINT64)3);
    }else if(*(UINT64*)pSmramState == EFI_SMRAM_LOCKED){  // DEBUG - Changed from EFI_SMRAM_CLOSED <AAV> EIP157631 Implement caching improvments in SMM
        MtrrVal |= 3;
    }
    AsmWriteMsr64 (AMD_MSR_SMM_MASK_HL, MtrrVal);

    return	EFI_SUCCESS;
}


STATIC
VOID
AllApSetSmram(
    IN  UINT64  SmramState
)
{
    EFI_STATUS                  Status;
    EFI_MP_SERVICES_PROTOCOL    *MpServices;

    Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
    ASSERT_EFI_ERROR(Status);


      MpServices->StartupAllAPs(
          MpServices,                                 // EFI_MP_SERVICES_PROTOCOL*
          (EFI_AP_PROCEDURE)ApSetSmram,               // EFI_AP_PROCEDURE
          TRUE,                                       // BOOLEAN SingleThread
          NULL,                                       // EFI_EVENT WaitEvent
          (UINTN)NULL,                                // UINTN Timeout
          (VOID*)&SmramState,                         // VOID *ProcArguments
          NULL
          ); 

}

/**
    This function programs the NB chipset registers to open
    the SMRAM area.

    @param This Pointer to the SMM access protocol
    @param DescriptorIndex Index of SMM Descriptor

           
    @retval EFI_SUCCESS Opened the SMM area.
    @retval EFI_INVALID_PARAMETER Descriptor doesn't exist.
    @retval EFI_ACCESS_DENIED SMM area locked

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
                Here is the control flow of this function:
                    1. If invalid Descriptor return EFI_INVALID_PARAMETER.
                    2. Read NB SMM register.
                    3. If locked, return EFI_ACCESS_DENIED
                    4. Set Value for register to Open SMM area (0a0000 - bffff)
                    5. Write Register.
                    6. Set OpenState to TRUE.
                    7. Set the RegionState to EFI_SMRAM_OPEN.
                    8. Return EFI_SUCCESS.
**/
STATIC
EFI_STATUS NBSMM_OpenSmram (
    IN EFI_SMM_ACCESS2_PROTOCOL      *This 
    )
{
    // All regions must open and close at the same time.
    UINT64  MTRRValue;


    if (AsmReadMsr64(AMD_MSR_HWCR) & 0x01)
        return EFI_ACCESS_DENIED;   //If device is locked, return error.

    // Enable RDMem & WRMem bits visible in fixed MTRR
    MTRRValue = AsmReadMsr64 (MSR_SYS_CFG);
    MTRRValue |= ((UINT32)1 << MTRRFixDRAMModEnBit);
    AsmWriteMsr64 (MSR_SYS_CFG, MTRRValue);

    // Enable 0xA0000 read/write
    AsmWriteMsr64 (MTRR_FIX_16K_A0000, 0x1C1C1C1C1C1C1C1C);

    // Disable RDMem & WRMem bits visible in fixed MTRR
    MTRRValue = AsmReadMsr64 (MSR_SYS_CFG);
    MTRRValue &= ~((UINT64)1 << MTRRFixDRAMModEnBit);
    AsmWriteMsr64 (MSR_SYS_CFG, MTRRValue);

    // Disable ASEG & TSEG SMRAM for VGA access
    MTRRValue = AsmReadMsr64 (AMD_MSR_SMM_MASK_HL);
    MTRRValue &= ~((UINT64)3);
    AsmWriteMsr64 (AMD_MSR_SMM_MASK_HL, MTRRValue);

    AllApSetSmram(EFI_SMRAM_OPEN);

    // Update appropriate flags
    This->OpenState = TRUE;
    gSmramMap[0].RegionState = EFI_SMRAM_OPEN;

    return EFI_SUCCESS;
}

/**
    This function programs the NB chipset registers to close
    the SMRAM area.

    @param This Pointer to the SMM access protocol
    @param DescriptorIndex Index of SMM Descriptor

           
    @retval EFI_SUCCESS Closed the SMM area.
    @retval EFI_INVALID_PARAMETER Descriptor doesn't exist.
    @retval EFI_ACCESS_DENIED SMM area locked

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
                Here is the control flow of this function:
                    1. If invalid Descriptor return EFI_INVALID_PARAMETER.
                    2. Read NB SMM register.
                    3. If locked, return EFI_ACCESS_DENIED
                    4. Set Value for register to close SMM area (0a0000 - bffff)
                    5. Write Register.
                    6. Set OpenState to FALSE.
                    7. Set the RegionState to EFI_SMRAM_CLOSED & EFI_ALLOCATED
                    8. Return EFI_SUCCESS.
**/
STATIC
EFI_STATUS NBSMM_CloseSmram (
    IN EFI_SMM_ACCESS2_PROTOCOL      *This 
    )
{
    // All regions must open and close at the same time.
    UINT64  MTRRValue;

    DEBUG((EFI_D_ERROR,"NBSMM_CloseSmram\n"));
    if (AsmReadMsr64(AMD_MSR_HWCR) & 0x01)
        return EFI_ACCESS_DENIED;   //If device is locked, return error.

    // Enable RDMem & WRMem bits visible in fixed MTRR
    MTRRValue = AsmReadMsr64 (MSR_SYS_CFG);
    MTRRValue |= ((UINT32)1 << MTRRFixDRAMModEnBit);
    AsmWriteMsr64 (MSR_SYS_CFG, MTRRValue);

    // Disable 0xA0000 read/write
    AsmWriteMsr64 (MTRR_FIX_16K_A0000, 0x0404040404040404);

    // Disable RDMem & WRMem bits visible in fixed MTRR
    MTRRValue = AsmReadMsr64 (MSR_SYS_CFG);
    MTRRValue &= ~((UINT64)1 << MTRRFixDRAMModEnBit);
    AsmWriteMsr64 (MSR_SYS_CFG, MTRRValue);

    // Enable ASEG & TSEG SMRAM for VGA access
//  MTRRValue = AsmReadMsr64 (AMD_MSR_SMM_MASK_HL);
//  MTRRValue |= 3;
//  AsmWriteMsr64 (AMD_MSR_SMM_MASK_HL, MTRRValue);

    AllApSetSmram(EFI_SMRAM_CLOSED);

    // Update appropriate flags
    This->OpenState = FALSE;
    gSmramMap[0].RegionState = EFI_SMRAM_CLOSED | EFI_ALLOCATED;

    return EFI_SUCCESS;
}


/**
    This function programs the NB chipset registers to lock
    the SMRAM area from opening/closing.  Only system level reset
    can unlock the SMRAM lock.

    @param This Pointer to the SMM access protocol
    @param DescriptorIndex Index of SMM Descriptor

           
    @retval EFI_SUCCESS Locked the SMM area.
    @retval EFI_INVALID_PARAMETER Descriptor doesn't exist.
    @retval EFI_DEVICE_ERROR SMM area is opened, need to be
        closed first before locking

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
                Here is the control flow of this function:
                    1. If invalid Descriptor return EFI_INVALID_PARAMETER.
                    2. Read NB SMM register.
                    3. If opened, return EFI_ACCESS_DENIED
                    4. Set Value for register to lock SMM area (0a0000 - bffff)
                    5. Write Register.
                    6. Set the RegionState to EFI_SMRAM_CLOSED & EFI_ALLOCATED
                    7. Return EFI_SUCCESS.
**/
STATIC
EFI_STATUS NBSMM_LockSmram (
    IN EFI_SMM_ACCESS2_PROTOCOL      *This 
    )

{
    UINT64  MTRRValue;
    UINT64  AsegStat;


    DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

    AsmWbinvd();
    // Enable ASEG & TSEG SMRAM for VGA access
    MTRRValue = AsmReadMsr64 (AMD_MSR_SMM_MASK_HL);
    MTRRValue |= 3;
    AsmWriteMsr64 (AMD_MSR_SMM_MASK_HL, MTRRValue);

    AllApSetSmram(EFI_SMRAM_LOCKED);
    // Enable RDMem & WRMem bits visible in fixed MTRR
    MTRRValue = AsmReadMsr64 (MSR_SYS_CFG);
    MTRRValue |= ((UINT32)1 << MTRRFixDRAMModEnBit);
    AsmWriteMsr64 (MSR_SYS_CFG, MTRRValue);

    // Read 0xA0000 status
    AsegStat = AsmReadMsr64 (MTRR_FIX_16K_A0000);

    // Disable RDMem & WRMem bits visible in fixed MTRR
    MTRRValue = AsmReadMsr64 (MSR_SYS_CFG);
    MTRRValue &= ~((UINT64)1 << MTRRFixDRAMModEnBit);
    AsmWriteMsr64 (MSR_SYS_CFG, MTRRValue);

    if (!(AsegStat & 0x1818181818181818))
        return EFI_DEVICE_ERROR;        // Region must be closed before locking

    // Lock the region
    //Errata 342
    //AsmWriteMsr64(AMD_MSR_HWCR, (AsmReadMsr64(AMD_MSR_HWCR) | 0x01));

    // Update appropriate flags
    This->LockState = TRUE;
    gSmramMap[0].RegionState = EFI_SMRAM_LOCKED;

    return EFI_SUCCESS;
}

/**
    This function returns the current SMRAM area map information
    such as number of regions and its start address and size

    @param This Pointer to the SMM access protocol
    @param SmramMapSize Size of the SMRAM map buffer provided
    @param SmramMap Buffer to copy the SMRAM map information

           
    @retval EFI_SUCCESS Smram Map copied into buffer
    @retval EFI_BUFFER_TOO_SMALL Indicates that provided buffer is
        not enough SmramMapSize filled with required/updated size

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
                Here is the control flow of this function:
                    1. If Smram Map Size less than the actual map size, set 
                        the map size and return EFI_BUFFER_TOO_SMALL.
                    2. Copy the Smram Map descriptors into the supplied buffer.
                    3. Set the map size in *SmramMapSize, just in case is larger 
                        than the actual buffer.
                    4. Return EFI_SUCCESS.
**/
STATIC
EFI_STATUS NBSMM_GetCapabilities (
    IN CONST EFI_SMM_ACCESS2_PROTOCOL     *This,
    IN OUT UINTN                          *SmramMapSize,
    OUT EFI_SMRAM_DESCRIPTOR              *SmramMap 
    )
{
    UINTN   i;

    if (*SmramMapSize < sizeof(gSmramMap)) {    //If input map size to small report the correct map size
        *SmramMapSize = sizeof(gSmramMap);      //and return error.
        return EFI_BUFFER_TOO_SMALL;
    }

    for(i=0;i<SMRAM_MAP_NUM_DESCRIPTORS;++i) 
        CopyMem(&SmramMap[i], &gSmramMap[i], sizeof(EFI_SMRAM_DESCRIPTOR));

    *SmramMapSize = sizeof(gSmramMap);          //Set the correct map size in case too large.
    return EFI_SUCCESS;
}


STATIC
EFI_SMM_ACCESS2_PROTOCOL gSmmAccess = {
    NBSMM_OpenSmram,
    NBSMM_CloseSmram,
    NBSMM_LockSmram,
    NBSMM_GetCapabilities,
    FALSE,
    FALSE
};


EFI_STATUS
SmmAccessInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    )
{
  NBSMM_EnableSMMAddress();

  return gBS->InstallMultipleProtocolInterfaces(
                &ImageHandle,
                &gEfiSmmAccess2ProtocolGuid, &gSmmAccess,
                NULL
                );
}


