
#include <Uefi.h>
#include <CpuBoard.h>


extern   UINT32 CpuReadPCI32(IN UINT32);

#define  PCI_ADDRESS(bus, dev, func, reg) \
            ((UINT64) ( (((UINTN)bus) << 16) + (((UINTN)dev) << 11) + \
            (((UINTN)func) << 8) + ((UINTN)reg) )) & 0x00000000ffffffff

// L1 instruction and data cache associativity encoding from the
// return value of CPUID to the SMBios specification's byte value.
ASSOC_MAP   L1AMap[] = 
    { 
     /* CPUID value, SMBios associativity encoding     */
      { 1,           EfiCacheAssociativityDirectMapped },
      { 2,           EfiCacheAssociativity2Way,        },
      { 4,           EfiCacheAssociativity4Way,        },
      { 8,           EfiCacheAssociativity8Way,        },
      { 16,          EfiCacheAssociativity16Way,       },
      { 0xff,        EfiCacheAssociativityFully,       }
    };
#define L1ASIZE sizeof(L1AMap) / sizeof(ASSOC_MAP)

// L2 data cache associativity encoding from the
// return value of CPUID to the SMBios specification's byte value.
ASSOC_MAP   L2AMap[] = 
    { 
     /* CPUID value, SMBios associativity encoding     */
      { 0,           EfiCacheAssociativityUnknown      },
      { 1,           EfiCacheAssociativityDirectMapped },
      { 2,           EfiCacheAssociativity2Way         },
      { 4,           EfiCacheAssociativity4Way         },
      { 6,           EfiCacheAssociativity8Way         },
      { 8,           EfiCacheAssociativity16Way        },
      { 0xf,         EfiCacheAssociativityFully        }
    };
#define L2ASIZE sizeof(L2AMap) / sizeof(ASSOC_MAP)

// L3 data cache associativity encoding from the
// return value of CPUID to the SMBios specification's byte value.
ASSOC_MAP   L3AMap[] = 
    { 
     /* CPUID value, SMBios associativity encoding     */
        { 0,        EfiCacheAssociativityUnknown        },
        { 1,        EfiCacheAssociativityDirectMapped   },
        { 2,        EfiCacheAssociativity2Way           },
        { 4,        EfiCacheAssociativity4Way           },
        { 6,        EfiCacheAssociativity8Way           },
        { 8,        EfiCacheAssociativity16Way          },
        { 0xa,      EfiCacheAssociativity32Way          },
        { 0xb,      EfiCacheAssociativity48Way          },
        { 0xc,      EfiCacheAssociativity64Way          },
        { 0xd,      EfiCacheAssociativityUnknown        }, // 96 way
        { 0xe,      EfiCacheAssociativityUnknown        }, // 128 way
        { 0xf,      EfiCacheAssociativityFully          }
    };
#define L3ASIZE sizeof(L3AMap) / sizeof(ASSOC_MAP)

// Describe how to retrieve cache information for each 
// instruction and data cache for the CPU.
CPU_CACHE_TABLE         tCache[] =
    {{ EFI_CACHE_L1, EfiCacheTypeInstruction, AMD_EXT_CPUID_5,
       EDX, 24, 0xFF, 16, 0xFF, {L1AMap, L1ASIZE} },
     { EFI_CACHE_L1, EfiCacheTypeData, AMD_EXT_CPUID_5,
       ECX, 24, 0xFF, 16, 0xFF, {L1AMap, L1ASIZE} },
     { EFI_CACHE_L2, EfiCacheTypeData, AMD_EXT_CPUID_6,
       ECX, 16, 0xFFFF, 12, 0xF, {L2AMap, L2ASIZE} },
     { EFI_CACHE_L3, EfiCacheTypeData, AMD_EXT_CPUID_6,
	   EDX, 18, 0x3FFF, 12, 0xF, {L3AMap, L3ASIZE}}};
UINT8 tCacheSize = sizeof(tCache) / sizeof(CPU_CACHE_TABLE);

/**
    Read the PCI Register

    @param Address (UINT32) ( (((UINT32)bus) << 24) + (((UINT32)dev) << 19) + (((UINT32)func) << 16) + ((UINT32)reg) )

    @retval UINT32 return value from the PCI Register.

**/
UINT32 CpuReadPCI32(
    IN UINT32 Address
)
{
    UINT32 Value32;
    UINT32 AddrCf8 = (1 << 31) + (Address & 0x0FFFF00) + (Address & 0xFC);

    IoWrite32(0xCF8, AddrCf8);
    Value32 = IoRead32(0xCFC);
    return Value32;
}

/**
    Board specfic CPU changes. This function will only execute
    on the BSP.

    @param VOID

    @retval EFI_STATUS return the EFI status

**/

EFI_STATUS BoardCpuInit(VOID)
{
    EFI_STATUS  Status = EFI_SUCCESS;

    return Status;
}


