
#include "CpuDxe.h"


//----------------------------------------------------------------------------
// External variables
//----------------------------------------------------------------------------
extern VOID     *gMpData;
MTRR_SETTINGS   *mMtrrTable;


//----------------------------------------------------------------------------
// Function definitions
//----------------------------------------------------------------------------
/**
    Flushes Data cache for IO DMA Operations. IO DMA operations are fully
    coherent, so EFI_SUCCESS returned. This routine is part of the
    EFI_CPU_ARCH_PROTOCOL.


    @param This Pointer to the CPU architecture protocol
    @param Start Start address of cache flush area
    @param Length Length of data to be flushed
    @param FlushType Flush type (writeback invalidate, writeback and invalidate)


    @retval EFI_SUCCESS always success

**/
EFI_STATUS EfiCpuFlushDataCache (
    IN  EFI_CPU_ARCH_PROTOCOL    *This,
    IN  EFI_PHYSICAL_ADDRESS     Start,
    IN  UINT64                   Length,
    IN  EFI_CPU_FLUSH_TYPE       FlushType)
{
    return EFI_SUCCESS;
}







/**
  Synchronzies up the MTRR values with BSP for calling processor.

  This function synchronzies up the MTRR values with BSP for calling processor.

  @param  Buffer         Mtrr table address.

**/
VOID
LoadMtrrData (
    IN  UINT32       Cpu,
    IN  VOID        *Context
)
{
//UINT64 qMSRSave;

//qMSRSave = AsmReadMsr64(MSR_SYS_CFG);
//AsmWriteMsr64(MSR_SYS_CFG, qMSRSave | (3 << MTRRFixDRAMEnBit));

  MtrrSetAllMtrrs(mMtrrTable);

//AsmWriteMsr64(MSR_SYS_CFG, qMSRSave);
}



VOID
MtrrDebugPrintAllMtrrsWorker (
  IN MTRR_SETTINGS    *MtrrSetting
  );


/**
  Implementation of SetMemoryAttributes() service of CPU Architecture Protocol.

  This function modifies the attributes for the memory region specified by BaseAddress and
  Length from their current attributes to the attributes specified by Attributes.

  @param  This             The EFI_CPU_ARCH_PROTOCOL instance.
  @param  BaseAddress      The physical address that is the start address of a memory region.
  @param  Length           The size in bytes of the memory region.
  @param  Attributes       The bit mask of attributes to set for the memory region.

  @retval EFI_SUCCESS           The attributes were set for the memory region.
  @retval EFI_ACCESS_DENIED     The attributes for the memory resource range specified by
                                BaseAddress and Length cannot be modified.
  @retval EFI_INVALID_PARAMETER Length is zero.
                                Attributes specified an illegal combination of attributes that
                                cannot be set together.
  @retval EFI_OUT_OF_RESOURCES  There are not enough system resources to modify the attributes of
                                the memory resource range.
  @retval EFI_UNSUPPORTED       The processor does not support one or more bytes of the memory
                                resource range specified by BaseAddress and Length.
                                The bit mask of attributes is not support for the memory resource
                                range specified by BaseAddress and Length.

**/
EFI_STATUS
EFIAPI
EfiCpuSetMemoryAttributes (
  IN EFI_CPU_ARCH_PROTOCOL     *This,
  IN EFI_PHYSICAL_ADDRESS      BaseAddress,
  IN UINT64                    Length,
  IN UINT64                    Attributes
  )
{
  EFI_STATUS                Status;
  MTRR_MEMORY_CACHE_TYPE    CacheType;
//UINT64                    qMSRSave;


  switch (Attributes) {
  case EFI_MEMORY_UC:
    CacheType = CacheUncacheable;
    break;

  case EFI_MEMORY_WC:
    CacheType = CacheWriteCombining;
    break;

  case EFI_MEMORY_WT:
    CacheType = CacheWriteThrough;
    break;

  case EFI_MEMORY_WP:
    CacheType = CacheWriteProtected;
    break;

  case EFI_MEMORY_WB:
    CacheType = CacheWriteBack;
    break;

  case EFI_MEMORY_UCE:
  case EFI_MEMORY_RP:
  case EFI_MEMORY_XP:
  case EFI_MEMORY_RUNTIME:
    return EFI_UNSUPPORTED;

  default:
    return EFI_INVALID_PARAMETER;
  }

  Status = MtrrSetMemoryAttribute(
             BaseAddress,
             Length,
             CacheType
             );

  if (!RETURN_ERROR (Status)) {
//-    qMSRSave = AsmReadMsr64(MSR_SYS_CFG);
//-    AsmWriteMsr64(MSR_SYS_CFG, qMSRSave | (3 << MTRRFixDRAMEnBit));
    MtrrGetAllMtrrs (mMtrrTable);
//-    AsmWriteMsr64(MSR_SYS_CFG, qMSRSave);
//-    MtrrDebugPrintAllMtrrsWorker(mMtrrTable);
    ExecuteFunctionOnRunningCpus(gMpData, TRUE, TRUE, LoadMtrrData, NULL);
  }
  return  Status;
}


