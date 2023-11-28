
#include "PlatformDxe.h"
#include <Protocol/LegacyRegion2.h>
#include <Protocol/Cpu.h>
#include <Library/BaseLib.h>

#include <AmdCpu.h>

#define PAM_GRANULARITY           0x4000



#define LEGACY_REGION_LOCK          0
#define LEGACY_REGION_BOOT_LOCK     1
#define LEGACY_REGION_UNLOCK        2
#define LEGACY_REGION_DECODE_ROM    3

// PAM-related declarations
#define REGION_LOCK                 0x10              // Read Only
#define REGION_UNLOCK               0x18              // Read/Write
#define REGION_DECODE_ROM           0x00		      // Read/Write ROM

EFI_CPU_ARCH_PROTOCOL           *gCpuArch;


#pragma pack (push, 1)
typedef struct
{
	UINT8 	Register;
	UINT8 	MaskShift;
	UINT32	StartAddress;
	UINT32	Length;
} PAM_STRUCT;

#pragma pack (pop)

// #include "CpuBoard.h"
//---------------------------------------------------------------------------

PAM_STRUCT gPamStruct[] =
{
    {0x68,0x00,0xc0000,0x1000},
    {0x68,0x08,0xc1000,0x1000},
    {0x68,0x10,0xc2000,0x1000},
    {0x68,0x18,0xc3000,0x1000},
    {0x68,0x20,0xc4000,0x1000},
    {0x68,0x28,0xc5000,0x1000},
    {0x68,0x30,0xc6000,0x1000},
    {0x68,0x38,0xc7000,0x1000},

    {0x69,0x00,0xc8000,0x1000},
    {0x69,0x08,0xc9000,0x1000},
    {0x69,0x10,0xca000,0x1000},
    {0x69,0x18,0xcb000,0x1000},
    {0x69,0x20,0xcc000,0x1000},
    {0x69,0x28,0xcd000,0x1000},
    {0x69,0x30,0xce000,0x1000},
    {0x69,0x38,0xcf000,0x1000},

    {0x6a,0x00,0xd0000,0x1000},
    {0x6a,0x08,0xd1000,0x1000},
    {0x6a,0x10,0xd2000,0x1000},
    {0x6a,0x18,0xd3000,0x1000},
    {0x6a,0x20,0xd4000,0x1000},
    {0x6a,0x28,0xd5000,0x1000},
    {0x6a,0x30,0xd6000,0x1000},
    {0x6a,0x38,0xd7000,0x1000},

    {0x6b,0x00,0xd8000,0x1000},
    {0x6b,0x08,0xd9000,0x1000},
    {0x6b,0x10,0xda000,0x1000},
    {0x6b,0x18,0xdb000,0x1000},
    {0x6b,0x20,0xdc000,0x1000},
    {0x6b,0x28,0xdd000,0x1000},
    {0x6b,0x30,0xde000,0x1000},
    {0x6b,0x38,0xdf000,0x1000},

    {0x6c,0x00,0xe0000,0x1000},
    {0x6c,0x08,0xe1000,0x1000},
    {0x6c,0x10,0xe2000,0x1000},
    {0x6c,0x18,0xe3000,0x1000},
    {0x6c,0x20,0xe4000,0x1000},
    {0x6c,0x28,0xe5000,0x1000},
    {0x6c,0x30,0xe6000,0x1000},
    {0x6c,0x38,0xe7000,0x1000},

    {0x6d,0x00,0xe8000,0x1000},
    {0x6d,0x08,0xe9000,0x1000},
    {0x6d,0x10,0xea000,0x1000},
    {0x6d,0x18,0xeb000,0x1000},
    {0x6d,0x20,0xec000,0x1000},
    {0x6d,0x28,0xed000,0x1000},
    {0x6d,0x30,0xee000,0x1000},
    {0x6d,0x38,0xef000,0x1000},

    {0x6e,0x00,0xf0000,0x1000},
    {0x6e,0x08,0xf1000,0x1000},
    {0x6e,0x10,0xf2000,0x1000},
    {0x6e,0x18,0xf3000,0x1000},
    {0x6e,0x20,0xf4000,0x1000},
    {0x6e,0x28,0xf5000,0x1000},
    {0x6e,0x30,0xf6000,0x1000},
    {0x6e,0x38,0xf7000,0x1000},

    {0x6f,0x00,0xf8000,0x1000},
    {0x6f,0x08,0xf9000,0x1000},
    {0x6f,0x10,0xfa000,0x1000},
    {0x6f,0x18,0xfb000,0x1000},
    {0x6f,0x20,0xfc000,0x1000},
    {0x6f,0x28,0xfd000,0x1000},
    {0x6f,0x30,0xfe000,0x1000},
    {0x6f,0x38,0xff000,0x1000}
};

#define NUM_PAM_ENTRIES (sizeof(gPamStruct) / sizeof(PAM_STRUCT))


typedef enum {
  CacheUncacheable    = 0,
  CacheWriteCombining = 1,
  CacheWriteThrough   = 4,
  CacheWriteProtected = 5,
  CacheWriteBack      = 6,
  CacheInvalid        = 7
} MTRR_MEMORY_CACHE_TYPE;


EFI_STATUS
CPUProgramPAMRegisters (
    IN UINT32                  StartAddress,
    IN UINT32                  Length,
    IN UINT8                   Setting,
    IN UINT64                  Attributes
    )
{
  UINT8       bCurrentMSR;
  UINT8       SetValue;
  UINT64      qMSRValue;
  UINT64      qMSRSave;
  UINTN       StartIndex;
  UINTN       EndIndex;
  UINTN       i;
  UINT32      TotalLength = 0;
  EFI_STATUS  Status = EFI_SUCCESS;
  MTRR_MEMORY_CACHE_TYPE    CacheType;


  DEBUG((EFI_D_INFO, "%a(%X,%X,%X)\n", __FUNCTION__, StartAddress, Length, Setting));

  for(StartIndex = 0; StartIndex < NUM_PAM_ENTRIES; ++StartIndex){
    if (gPamStruct[StartIndex].StartAddress >= StartAddress){ 
      break;
    }
  }
  for(EndIndex = StartIndex; EndIndex < NUM_PAM_ENTRIES; ++EndIndex){
    TotalLength += gPamStruct[EndIndex].Length;
    if (TotalLength >= Length){
      break;
    }
  }
  if(StartIndex >= NUM_PAM_ENTRIES || EndIndex >= NUM_PAM_ENTRIES){
    return EFI_INVALID_PARAMETER;
  }

  DEBUG((EFI_D_INFO, "Index [%d,%d]\n", StartIndex, EndIndex));  

  switch(Setting){
    case LEGACY_REGION_LOCK:
    case LEGACY_REGION_BOOT_LOCK:  
      SetValue = REGION_LOCK;
      break;

    case LEGACY_REGION_UNLOCK:
      SetValue = REGION_UNLOCK;
      break;

    default:
    case LEGACY_REGION_DECODE_ROM:
      SetValue = REGION_DECODE_ROM;
      break;      
  }

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

    default:
      CacheType = CacheUncacheable;
      break;
  }




  AsmDisableCache();
  AsmWriteMsr64(MTRR_DEF_TYPE, 0);

  qMSRSave = AsmReadMsr64(MSR_SYS_CFG);
  AsmWriteMsr64(MSR_SYS_CFG, qMSRSave | (3 << MTRRFixDRAMEnBit));

  for(i = StartIndex; i <= EndIndex; ++i){
    bCurrentMSR = gPamStruct[i].Register;
    qMSRValue = AsmReadMsr64(bCurrentMSR + 0x200);
    while ((gPamStruct[i].Register == bCurrentMSR) && (i <= EndIndex)){
      qMSRValue &= ~(LShiftU64(0x1F, gPamStruct[i].MaskShift));
      qMSRValue |= LShiftU64(SetValue | CacheType, gPamStruct[i].MaskShift);
      i++;
    }
    i--;
    AsmWriteMsr64(bCurrentMSR + 0x200, qMSRValue);
//- DEBUG((EFI_D_INFO, "Msr(%X):%lX\n", bCurrentMSR + 0x200, qMSRValue));
  }

  AsmWriteMsr64(MSR_SYS_CFG, qMSRSave);

  AsmWriteMsr64(MTRR_DEF_TYPE,3<<10);  //enable Fixed and Variable MTRRs, default UC
  AsmEnableCache();

  return Status;
}


            
           
EFI_STATUS
LegacyRegionManipulation (
  IN  EFI_LEGACY_REGION2_PROTOCOL *This,
  IN  UINT32                      StartAddress,
  IN  UINT32                      Length,
  IN  UINT32                      Setting,
  OUT UINT32                      *Granularity
  )
{
  UINT64                          Attributes;
  EFI_STATUS                      Status;


  if ((StartAddress < 0xC0000) || ((StartAddress + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  if(Granularity != NULL) {
    *Granularity = 0x1000;
  }

  StartAddress &= ~0xfff; 
  Length += StartAddress & 0xFFF;             //If start not align to 4K, add the offset to Length
  Length = (Length + 0xfff) & ~0xfff;

  switch(Setting) {
    case LEGACY_REGION_UNLOCK:
      Attributes = EFI_MEMORY_WB;
      break;
      
    case LEGACY_REGION_LOCK:
    case LEGACY_REGION_BOOT_LOCK:
      Attributes = EFI_MEMORY_WP;
      break;
      
    default:
    case LEGACY_REGION_DECODE_ROM:   
      Attributes = EFI_MEMORY_UC;
      break;
  }

  Status = CPUProgramPAMRegisters(StartAddress, Length, Setting, Attributes);
  if (Status != EFI_SUCCESS){
    return Status;
  }

//DEBUG((EFI_D_INFO, "SetMemoryAttributes(%X,%X,%X)\n", StartAddress, Length, Attributes));
  Status = gCpuArch->SetMemoryAttributes (
                      gCpuArch,
                      StartAddress,
                      Length,
                      Attributes
                      );
//DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status)); 
  return Status;
}


/**
  Modify the hardware to allow (decode) or disallow (not decode) memory reads in a region.

  If the On parameter evaluates to TRUE, this function enables memory reads in the address range 
  Start to (Start + Length - 1).
  If the On parameter evaluates to FALSE, this function disables memory reads in the address range 
  Start to (Start + Length - 1).

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose attributes
                                should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address
                                was not aligned to a region's starting address or if the length
                                was greater than the number of bytes in the first region.
  @param  On[in]                Decode / Non-Decode flag.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2Decode (
  IN  EFI_LEGACY_REGION2_PROTOCOL  *This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity,
  IN  BOOLEAN                      *On
  )
{
  DEBUG((EFI_D_ERROR, "%a(%X,%X) On:%d\n", __FUNCTION__, Start, Length, *On));
  if (*On) {
    return LegacyRegionManipulation (This, Start, Length, LEGACY_REGION_UNLOCK, Granularity);
  } else {
    return LegacyRegionManipulation (This, Start, Length, LEGACY_REGION_DECODE_ROM, Granularity);
  }
}

/**
  Modify the hardware to disallow memory writes in a region.

  This function changes the attributes of a memory range to not allow writes.

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose
                                attributes should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address was
                                not aligned to a region's starting address or if the length was
                                greater than the number of bytes in the first region.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2Lock (
  IN  EFI_LEGACY_REGION2_PROTOCOL *This,
  IN  UINT32                      Start,
  IN  UINT32                      Length,
  OUT UINT32                      *Granularity
  )
{
  DEBUG((EFI_D_INFO, "%a(%X,%X)\n", __FUNCTION__, Start, Length));
  return LegacyRegionManipulation (This, Start, Length, LEGACY_REGION_LOCK, Granularity);
}

/**
  Modify the hardware to disallow memory attribute changes in a region.

  This function makes the attributes of a region read only. Once a region is boot-locked with this 
  function, the read and write attributes of that region cannot be changed until a power cycle has
  reset the boot-lock attribute. Calls to Decode(), Lock() and Unlock() will have no effect.

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose
                                attributes should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address was
                                not aligned to a region's starting address or if the length was
                                greater than the number of bytes in the first region.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.
  @retval EFI_UNSUPPORTED       The chipset does not support locking the configuration registers in
                                a way that will not affect memory regions outside the legacy memory
                                region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2BootLock (
  IN  EFI_LEGACY_REGION2_PROTOCOL         *This,
  IN  UINT32                              Start,
  IN  UINT32                              Length,
  OUT UINT32                              *Granularity
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  return LegacyRegionManipulation(This, Start, Length, LEGACY_REGION_BOOT_LOCK, Granularity);
}

/**
  Modify the hardware to allow memory writes in a region.

  This function changes the attributes of a memory range to allow writes.  

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose
                                attributes should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address was
                                not aligned to a region's starting address or if the length was
                                greater than the number of bytes in the first region.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2Unlock (
  IN  EFI_LEGACY_REGION2_PROTOCOL  *This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity
  )
{
  DEBUG((EFI_D_INFO, "%a(%X,%X)\n", __FUNCTION__, Start, Length));
  return LegacyRegionManipulation (This, Start, Length, LEGACY_REGION_UNLOCK, Granularity);
}

/**
  Get region information for the attributes of the Legacy Region.

  This function is used to discover the granularity of the attributes for the memory in the legacy 
  region. Each attribute may have a different granularity and the granularity may not be the same
  for all memory ranges in the legacy region.  

  @param  This[in]              Indicates the EFI_LEGACY_REGION2_PROTOCOL instance.
  @param  DescriptorCount[out]  The number of region descriptor entries returned in the Descriptor
                                buffer.
  @param  Descriptor[out]       A pointer to a pointer used to return a buffer where the legacy
                                region information is deposited. This buffer will contain a list of
                                DescriptorCount number of region descriptors.  This function will
                                provide the memory for the buffer.

  @retval EFI_SUCCESS           The information structure was returned.
  @retval EFI_UNSUPPORTED       This function is not supported.

**/
EFI_STATUS
EFIAPI
LegacyRegionGetInfo (
  IN  EFI_LEGACY_REGION2_PROTOCOL   *This,
  OUT UINT32                        *DescriptorCount,
  OUT EFI_LEGACY_REGION_DESCRIPTOR  **Descriptor
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  return EFI_UNSUPPORTED;
}


STATIC EFI_LEGACY_REGION2_PROTOCOL  gLegacyRegion2 = {
  LegacyRegion2Decode,
  LegacyRegion2Lock,
  LegacyRegion2BootLock,
  LegacyRegion2Unlock,
  LegacyRegionGetInfo
};


EFI_STATUS
EFIAPI
LegacyRegion2Install (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  
  Status = gBS->LocateProtocol (
                  &gEfiCpuArchProtocolGuid,
                  NULL,
                  &gCpuArch
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiLegacyRegion2ProtocolGuid, &gLegacyRegion2,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
