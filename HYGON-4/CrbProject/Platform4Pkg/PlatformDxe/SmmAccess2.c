

#include <Uefi.h>
#include <HygonCpu.h>
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
#include <Library/MemoryAllocationLib.h>


STATIC EFI_SMRAM_DESCRIPTOR  *gSmramDesc;
STATIC UINTN                 gSmramDescSize;
STATIC PLATFORM_COMM_INFO    *gPlatCommInfo = NULL;



STATIC
EFI_STATUS NBSMM_OpenSmram (
    IN EFI_SMM_ACCESS2_PROTOCOL      *This
    );

STATIC
EFI_STATUS NBSMM_CloseSmram (
    IN EFI_SMM_ACCESS2_PROTOCOL      *This
    );

STATIC
EFI_STATUS NBSMM_LockSmram (
    IN EFI_SMM_ACCESS2_PROTOCOL      *This
    );

STATIC
EFI_STATUS NBSMM_GetCapabilities (
    IN CONST EFI_SMM_ACCESS2_PROTOCOL     *This,
    IN OUT UINTN                          *SmramMapSize,
    OUT EFI_SMRAM_DESCRIPTOR              *SmramMap
    );

STATIC
EFI_SMM_ACCESS2_PROTOCOL gSmmAccess2 = {
    NBSMM_OpenSmram,
    NBSMM_CloseSmram,
    NBSMM_LockSmram,
    NBSMM_GetCapabilities,
    FALSE,
    FALSE
};

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

  AsmWriteMsr64 (HYGON_MSR_SMM_ADDR_HL, gPlatCommInfo->TsegBase);

  TsegAddress = AsmReadMsr64 (HYGON_MSR_SMM_MASK_HL);
  TsegAddress &= 0xFFFF000000018FFFULL;
  TsegAddress |= (~(UINT64)(gPlatCommInfo->TsegSize - 1)) & 0xFFFFFFFE0000;
  TsegAddress |= CacheWriteBack << 12;
  AsmWriteMsr64 (HYGON_MSR_SMM_MASK_HL, TsegAddress);

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

    if (AsmReadMsr64(HYGON_MSR_HWCR) & 0x01)
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
    MtrrVal = AsmReadMsr64 (HYGON_MSR_SMM_MASK_HL);
    if(*(UINT64*)pSmramState == EFI_SMRAM_OPEN){
        MtrrVal &= ~((UINT64)3);
    }else if(*(UINT64*)pSmramState == EFI_SMRAM_LOCKED){  // DEBUG - Changed from EFI_SMRAM_CLOSED <AAV> EIP157631 Implement caching improvments in SMM
        MtrrVal |= 3;
    }
    AsmWriteMsr64 (HYGON_MSR_SMM_MASK_HL, MtrrVal);

    return  EFI_SUCCESS;
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





EFI_STATUS SmramHwOpen()
{
  // All regions must open and close at the same time.
  UINT64  MTRRValue;


  if (AsmReadMsr64(HYGON_MSR_HWCR) & 0x01)
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
  MTRRValue = AsmReadMsr64 (HYGON_MSR_SMM_MASK_HL);
  MTRRValue &= ~((UINT64)3);
  AsmWriteMsr64 (HYGON_MSR_SMM_MASK_HL, MTRRValue);

  AllApSetSmram(EFI_SMRAM_OPEN);

  return EFI_SUCCESS;
}


EFI_STATUS SmramHwClose()
{
  // All regions must open and close at the same time.
  UINT64  MTRRValue;

  DEBUG((EFI_D_INFO,"NBSMM_CloseSmram\n"));
  if (AsmReadMsr64(HYGON_MSR_HWCR) & 0x01)
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
//  MTRRValue = AsmReadMsr64 (HYGON_MSR_SMM_MASK_HL);
//  MTRRValue |= 3;
//  AsmWriteMsr64 (HYGON_MSR_SMM_MASK_HL, MTRRValue);

  AllApSetSmram(EFI_SMRAM_CLOSED);

  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
ApMission(
    IN  VOID    *Ctx
)
{
  AsmWriteMsr64(HYGON_MSR_HWCR, (AsmReadMsr64(HYGON_MSR_HWCR) | 0x01));
  return  EFI_SUCCESS;  
}


EFI_STATUS SmramHwLock()
{
  UINT64                      MTRRValue;
  UINT64                      AsegStat;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  AsmWbinvd();
  // Enable ASEG & TSEG SMRAM for VGA access
  MTRRValue = AsmReadMsr64 (HYGON_MSR_SMM_MASK_HL);
  MTRRValue |= 3;
  AsmWriteMsr64 (HYGON_MSR_SMM_MASK_HL, MTRRValue);

  AllApSetSmram(EFI_SMRAM_LOCKED);
  // Enable RDMem & WRMem bits visible in fixed MTRR
  MTRRValue = AsmReadMsr64 (MSR_SYS_CFG);
  MTRRValue |= ((UINT32)1 << MTRRFixDRAMModEnBit);
  AsmWriteMsr64 (MSR_SYS_CFG, MTRRValue);

  // Read 0xA0000 status
  AsegStat = AsmReadMsr64 (MTRR_FIX_16K_A0000);
  DEBUG((EFI_D_INFO, "AsegStat:%lx\n", AsegStat));  

  // Disable RDMem & WRMem bits visible in fixed MTRR
  MTRRValue = AsmReadMsr64 (MSR_SYS_CFG);
  MTRRValue &= ~((UINT64)1 << MTRRFixDRAMModEnBit);
  AsmWriteMsr64 (MSR_SYS_CFG, MTRRValue);

// Region must be closed before locking
  ASSERT((AsegStat & 0x1818181818181818) == 0);

  return EFI_SUCCESS;
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
  UINTN                  Index;
  UINTN                  Count;


  if(gSmmAccess2.LockState){
    return EFI_ACCESS_DENIED;
  }  

  Count = gSmramDescSize/sizeof(EFI_SMRAM_DESCRIPTOR);

  for (Index = 0; Index < Count; Index++){
    if (gSmramDesc[Index].RegionState & EFI_SMRAM_LOCKED){	    
      DEBUG ((EFI_D_INFO, "Cannot open a locked SMRAM region\n"));
      continue;
    }
    gSmramDesc[Index].RegionState &= (~(UINT64)(EFI_SMRAM_CLOSED |EFI_ALLOCATED));
    gSmramDesc[Index].RegionState |= EFI_SMRAM_OPEN;  	
  }	  	
  
  gSmmAccess2.OpenState = TRUE;

  SmramHwOpen();
  
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
  UINTN                  Index;
  UINTN                  Count;

  Count = gSmramDescSize/sizeof(EFI_SMRAM_DESCRIPTOR);
  
  for (Index = 0; Index < Count; Index++){

    if (gSmramDesc[Index].RegionState & EFI_SMRAM_LOCKED){	    
      DEBUG ((EFI_D_INFO, "Cannot close a locked SMRAM region\n"));
      continue;
    }
    if (gSmramDesc[Index].RegionState & EFI_SMRAM_CLOSED){
      continue;
    }

    gSmramDesc[Index].RegionState &= (~(UINT64)EFI_SMRAM_OPEN);
    gSmramDesc[Index].RegionState |= (UINT64)(EFI_SMRAM_CLOSED | EFI_ALLOCATED);
  }	

  gSmmAccess2.OpenState = FALSE;

  SmramHwClose();
  
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
  UINTN                  Index;
  UINTN                  Count;

  Count = gSmramDescSize/sizeof(EFI_SMRAM_DESCRIPTOR);

  for (Index = 0; Index < Count; Index++){
    if (gSmmAccess2.OpenState){
      DEBUG ((EFI_D_ERROR, "Cannot lock SMRAM when SMRAM regions are still open\n"));
      return EFI_DEVICE_ERROR;
    }

    gSmramDesc[Index].RegionState |= EFI_SMRAM_LOCKED; 	
  }	
   
  gSmmAccess2.LockState = TRUE; 

  SmramHwLock();
  
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
  UINTN       DescSize;
  EFI_STATUS  Status;
  
  DescSize = gSmramDescSize;
  if (*SmramMapSize < DescSize) {
    Status = EFI_BUFFER_TOO_SMALL;
  } else {
    CopyMem (SmramMap, gSmramDesc, DescSize);
    Status = EFI_SUCCESS;
  }

  *SmramMapSize = DescSize;
  return Status;
}



VOID
EFIAPI
SmmAccess2OnReadyToBoot (
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  )
{
  EFI_STATUS                  Status;
  EFI_MP_SERVICES_PROTOCOL    *MpServices;

  gBS->CloseEvent(Event);

  // Lock the region
  AsmWriteMsr64(HYGON_MSR_HWCR, (AsmReadMsr64(HYGON_MSR_HWCR) | 0x01));

  Status = gBS->LocateProtocol(&gEfiMpServiceProtocolGuid, NULL, &MpServices);
  if(!EFI_ERROR(Status)){
    Status = MpServices->StartupAllAPs(
                MpServices,                                 // EFI_MP_SERVICES_PROTOCOL*
                ApMission,                                  // EFI_AP_PROCEDURE
                TRUE,                                       // BOOLEAN SingleThread
                NULL,                                       // EFI_EVENT WaitEvent
                (UINTN)NULL,                                // UINTN Timeout
                NULL,                                       // VOID *ProcArguments
                NULL
                );
    DEBUG((EFI_D_INFO, "SmramLockAP:%r\n", Status));
  }  
}



EFI_STATUS
SmmAccessInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    )
{
  EFI_PEI_HOB_POINTERS  GuidHob;
  EFI_SMRAM_DESCRIPTOR  *SmramDesc;
  UINTN                 SmramDescSize;
  EFI_STATUS            Status;
  EFI_EVENT             Event;
  

  GuidHob.Raw = GetFirstGuidHob(&gSmramDescTableGuid);
  ASSERT(GuidHob.Raw != NULL);
  SmramDesc = (VOID*)(GuidHob.Guid+1);
  SmramDescSize = GET_GUID_HOB_DATA_SIZE(GuidHob.Guid);
  DEBUG((EFI_D_INFO, "SMRAM Desc(%X,%X)\n", SmramDesc, SmramDescSize));

  gSmramDescSize = SmramDescSize;
  gSmramDesc = (EFI_SMRAM_DESCRIPTOR*)AllocatePool(SmramDescSize);
  ASSERT(gSmramDesc != NULL);
  CopyMem(gSmramDesc, SmramDesc, SmramDescSize);

  gSmmAccess2.OpenState = !!(SmramDesc[0].RegionState & EFI_SMRAM_OPEN);
  gSmmAccess2.LockState = !!(SmramDesc[0].RegionState & EFI_SMRAM_LOCKED);
  DEBUG((EFI_D_INFO, "OpenState:%d, LockState:%d\n", gSmmAccess2.OpenState, gSmmAccess2.LockState));

  NBSMM_EnableSMMAddress();

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             SmmAccess2OnReadyToBoot,
             NULL,
             &Event
             );
  ASSERT_EFI_ERROR(Status);

  return gBS->InstallMultipleProtocolInterfaces(
                &ImageHandle,
                &gEfiSmmAccess2ProtocolGuid, &gSmmAccess2,
                NULL
                );
}


