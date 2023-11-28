

#ifndef __AMD_CPU_H__
#define __AMD_CPU_H__
//------------------------------------------------------------------------


#include <Library/HobLib.h>
#include <Protocol/Cpu.h>
#include <Base.h>
#include <Pi/PiMultiPhase.h>
#include <Pi/PiStatusCode.h>
#include <Protocol/StatusCode.h>
#include <Guid/Acpi.h>

#ifdef PEI_COMPILE
#include <Pi/PiPeiCis.h>
#include <Ppi/CpuIo.h>
#include <Ppi/PciCfg2.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/BootInRecoveryMode.h>
#include <Ppi/DxeIpl.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/PciCfg.h>
#include <Ppi/Stall.h>
#endif


#pragma pack(push,1)

typedef struct {
    UINT64 SmrrSupport:1;
    UINT64 Reserved:63;
} ADDITIONAL_FEATURE_FLAGS;

typedef struct {
    UINT32 FeatureEcx;
    UINT32 FeatureEdx;
    UINT32 ExtFeatureEax;
    UINT32 ExtFeatureEbx;
    UINT32 ExtFeatureEcx;
    UINT32 ExtFeatureEdx;
    ADDITIONAL_FEATURE_FLAGS Flags;
} CPU_FEATURES;

typedef struct {
	BOOLEAN     Valid;
	UINT8       ApicId;
} CPUINFO;

// SmramCap field values
#define	SMRAM_ASEG_SUPPORTED			0x01			// Bit 0
#define	SMRAM_TSEG_SUPPORTED			0x02			// BIT 1
#define	SMRAM_HSEG_SUPPORTED			0x04			// BIT 2

typedef struct {
	UINT8					CpuCount;
	UINT8					BspNo;
	UINT32					SmramCap;
	EFI_PHYSICAL_ADDRESS	TsegAddress;
	UINT32					TsegSize;
	UINT16					CacheLineSize;			// (Bytes)
  CPU_FEATURES    CpuFeatures;
	CPUINFO					Cpuinfo[1];				//This must be last in structure.
} CPUINFO_HOB_DATA;
////////////////////////////////////////////////////////////////////////



typedef struct {
    EFI_HOB_GUID_TYPE   EfiHobGuidType;
    UINT32  Tseg;
    UINT32  TsegLength;
    UINT32  ManagedMemStart;
    UINT32  ManagedMemSize;
    UINT32  IedStart;
    UINT32  IedSize;
    UINT16  NumCpus;
    UINT16  Bsp;
    UINT32  SmmBase[1];
} SMM_HOB;

#pragma pack(pop)







//----OR masks
#define INIT				    (5 << 8)    // Then INIT message
#define SIPI				    (6 << 8)    // The startup message
#define DELIVERY_INCOMPLETE     (1 << 12)   // 1=incomplete
#define LEVEL_DEASSERT		    (0 << 14)   // Assertion level is low
#define LEVEL_ASSERT		    (1 << 14)   // Assertion level is high
#define TM_EDGE	            	(0 << 15)   // trigger mode = edge
#define TM_LEVEL            	(1 << 15)   // trigger mode = level
#define USE_DEST_FIELD	    	(0 << 18)   // dest = dest field
#define SELF                    (1 << 18)   // Destination for message
#define ALL_EXCLUDING_SELF	    (3 << 18)   // Destination for message

#define	MSR_XAPIC_BASE				0x1B
#define XAPIC_BASE_ENABLE_BIT		11
#define	XAPIC_BASE_BSP_BIT			8
#define	XAPIC_BASE_BSP_BIT_MASK		(1 << XAPIC_BASE_BSP_BIT)

#define	XAPIC_ENABLE_BIT			8d	// SVR SW APIC Enable/Disable Bit
#define	APIC_PRESENT_BIT			9	// APIC Present bit in Feature Flags

#define	MASK_ICR_CLEAR				0xFFF33000	// AND mask for reserved bits
#define	OR_MASK_INIT_IPI			0x00004500	// OR mask to send INIT IPI
#define	OR_MASK_USE_DEST_FIELD		0x00000000	// OR mask to set dest field 
                                                // = "Dest Field"

#define APIC_BASE	0xfee00000
#define LOCAL_APIC_BASE	0xfee00000
#define APIC_VERSION_REGISTER  0x30
#define APIC_SPURIOUS_VECTOR_REGISTER	0xf0
#define APIC_LVT_LINT0_REGISTER	0x350
#define APIC_LVT_LINT1_REGISTER	0x360
#define APIC_ICR_LOW_REGISTER	0x300
#define APIC_ICR_HIGH_REGISTER	0x310

#define MP_JUMP_FUNCTION_ADDRESS	0x1000
#define MP_ZERO_DATA_ADDRESS	    0x0
#define APIC_ID_REGISTER	        0x20


#define TSEG_SIZE	0x4000000


//-----------------------------------------------------------------------------
//              Local APIC Register Equates
//-----------------------------------------------------------------------------
#define	LOCAL_APIC_ID_REG        0x020
#define	LOCAL_APIC_VERSION       0x030
#define	LOCAL_APIC_TASK_PRI      0x080
#define	LOCAL_APIC_ARB_PRI       0x090
#define	LOCAL_APIC_PROC_PRI      0x0A0
#define	LOCAL_APIC_EOI           0x0B0
#define	LOCAL_APIC_LDR           0x0D0
#define	LOCAL_APIC_DEST_FORMAT   0x0E0
#define	LOCAL_APIC_SVR           0x0F0
#define	LOCAL_APIC_SVR_ASM       00F0h
#define	LOCAL_APIC_ISR0          0x100
#define	LOCAL_APIC_TMR0          0x180
#define	LOCAL_APIC_IRR0          0x200
#define	LOCAL_APIC_ERR_STAT      0x280
#define	LOCAL_APIC_ICR_LO        0x300
#define	LOCAL_APIC_ICR_HI        0x310
#define	LOCAL_APIC_LVT           0x320
#define LOCAL_APIC_TLVT	    	 0x330 //Thermal Local vector table entry
#define	LOCAL_APIC_PERF          0x340
#define	LOCAL_APIC_LVT_LINT0     0x350
#define	LOCAL_APIC_LVT_LINT1     0x360
#define	LOCAL_APIC_LVT_ERR       0x370
#define	LOCAL_APIC_ITC           0x380
#define	LOCAL_APIC_TIMER         0x390
#define	LOCAL_APIC_TMR_DIV       0x3E0

// APIC Base MSR Bit Equates
#define APICBase_Enable_Bit      0x0011d
#define APICBase_BSP_Bit         0x0008d


// Generic MTRR equates
#define	MTRR_PHYS_BASE_0		0x200
#define	MTRR_PHYS_MASK_0		0x201
#define	MTRR_PHYS_BASE_1		0x202
#define	MTRR_PHYS_MASK_1		0x203
#define	MTRR_PHYS_BASE_2		0x204
#define	MTRR_PHYS_MASK_2		0x205
#define	MTRR_PHYS_BASE_3		0x206
#define	MTRR_PHYS_MASK_3		0x207
#define	MTRR_PHYS_BASE_4		0x208
#define	MTRR_PHYS_MASK_4		0x209
#define	MTRR_PHYS_BASE_5		0x20A
#define	MTRR_PHYS_MASK_5		0x20B
#define	MTRR_PHYS_BASE_6		0x20C
#define	MTRR_PHYS_MASK_6		0x20D
#define	MTRR_PHYS_BASE_7		0x20E
#define	MTRR_PHYS_MASK_7		0x20F
#define	MTRR_FIX_64K_00000		0x250
#define	MTRR_FIX_16K_80000		0x258
#define	MTRR_FIX_16K_A0000		0x259
#define	MTRR_FIX_4K_C0000		0x268
#define	MTRR_FIX_4K_C8000		0x269
#define	MTRR_FIX_4K_D0000		0x26A
#define	MTRR_FIX_4K_D8000		0x26B
#define	MTRR_FIX_4K_E0000		0x26C
#define	MTRR_FIX_4K_E8000		0x26D
#define	MTRR_FIX_4K_F0000		0x26E
#define	MTRR_FIX_4K_F8000		0x26F
#define	MTRR_DEF_TYPE			0x2FF

//----------------------------------------------------------------------------
// MTRR_PHYS_BASE_7 is used for TSEG caching, which reduces the total number 
// of usable variable-range registers.  See InitializeSmm() (CpuPei.c) for
// programming of TSEG cache register.
//----------------------------------------------------------------------------
#if SMM_CACHE_SUPPORT == 1
  #define NUM_OF_MTRR 7
#else
  #define NUM_OF_MTRR 8
#endif

// AMD model specific MSR
#define	MSR_SYS_CFG				0xC0010010
#define MSR_TOP_MEM2    0x0C001001D
#define FORCE_TOM2_WB_BIT			BIT22
#define		SetDirtyEnE			8
		// Set Dirty Bit on Cache Block Transition to E
#define		SetDirtyEnS			9
		// Set Dirty Bit on Cache Block Transition to S
#define		SetDirtyEnO			10
		// Set Dirty Bit on Cache Block Transition to O
#define		SysECCEnable		15
		// Controls S2K Bus (FSB) ECC
#define		MTRRFixDRAMEnBit	18
		// Enables AMD Fixed MTRR extensions
#define		MTRRFixDRAMModEnBit	19
		// Controls visibilty of Fixed MTRR Mode bits
#define		MTRRVarDRAMEnBit	20
		// Enables Top of Mem registers and IORRs
#define	AMD_MSR_HWCR			0xC0010015
#define	AMD_MSR_IORR_BASE0		0xC0010016
#define	AMD_MSR_IORR_MASK0		0xC0010017
#define	AMD_MSR_IORR_BASE1		0xC0010018
#define	AMD_MSR_IORR_MASK1		0xC0010019
#define	AMD_MSR_TOP_MEM			0xC001001A
#define	AMD_MSR_TOP_MEM2		0xC001001D
#define	AMD_MSR_MC4_CTL			0xC0010048
#define	AMD_MSR_NB_CFG			0xC001001F

#define	AMD_MSR_SMI_TRIGGER_IO		0xC0010056
#define AMD_MSR_PSTATE_CONTROL          	0xC0010062
#define	AMD_MSR_UNLOCK			0x9C5A203A		// MSR unlock code
#define	AMD_PP_MSR_NB_CFG		0xC001101F
#define	AMD_PP_MSR_LS_CFG		0xC0011020
#define	AMD_PP_MSR_IC_CFG		0xC0011021
#define	AMD_PP_MSR_DC_CFG		0xC0011022
#define	AMD_PP_MSR_BU_CFG		0xC0011023

#define	AMD_MSR_EFER				0xC0000080
#define	AMD_MSR_CLK_CTRL			0xC001001B
#define	AMD_MSR_PATCH_LOADER		0xC0010020
#define	AMD_MSR_NAME0				0xC0010030
#define	AMD_MSR_NAME1				0xC0010031
#define	AMD_MSR_NAME2				0xC0010032
#define	AMD_MSR_NAME3				0xC0010033
#define	AMD_MSR_NAME4				0xC0010034
#define	AMD_MSR_NAME5				0xC0010035
#define	AMD_MSR_FIDVID_CTL			0xC0010041
#define	AMD_MSR_FIDVID_STS			0xC0010042
#define	AMD_MSR_C1E_INIT			0xC0010055
#define AMD_MSR_COFVID_STAT			0xC0010071
#define	AMD_MSR_SYSBUS_IOBASE		0xC0010100
#define	AMD_MSR_SYSBUS_CONFIGBASE	0xC0010101
#define	AMD_MSR_SYSBUS_IACKBASE		0xC0010102
#define	AMD_MSR_SYSBUS_SPEC_CYCBASE	0xC0010103
#define	AMD_MSR_SYSBUS_UPPER_ADD	0xC0010104
#define	AMD_MSR_SMM_ENTER_SCYC		0xC0010105
#define	AMD_MSR_SMM_EXIT_SCYC		0xC0010106
#define	AMD_MSR_SMM_BASE			  0xC0010111
#define	AMD_MSR_SMM_ADDR_HL			0xC0010112
#define	AMD_MSR_SMM_MASK_HL			0xC0010113
#define	BIT_SMM_AValid		        0		        // Enable SMRAM access 

#define AMD_MSR_MMIO_CFG_BASE		0xC0010058
#define AMD_MSR_PSP_ADDR            0xC00110A2

#define AMD_MSR_CPUID_NAME_STRING0  0xC0010030      // First CPUID namestring register
#define AMD_MSR_CPUID_NAME_STRING1  0xC0010031
#define AMD_MSR_CPUID_NAME_STRING2  0XC0010032
#define AMD_MSR_CPUID_NAME_STRING3  0xC0010033
#define AMD_MSR_CPUID_NAME_STRING4  0xC0010034
#define AMD_MSR_CPUID_NAME_STRING5  0xC0010035      // Last CPUID namestring register
//-----------------------------------------------------------------------------
//		Extended CPUID Equates
//-----------------------------------------------------------------------------
#define AMD_EXT_CPUID_0             0x80000000
#define AMD_EXT_CPUID_1             0x80000001
#define AMD_EXT_CPUID_2             0x80000002
#define AMD_EXT_CPUID_3             0x80000003
#define AMD_EXT_CPUID_4             0x80000004
#define AMD_EXT_CPUID_5             0x80000005
#define AMD_EXT_CPUID_6             0x80000006
#define AMD_EXT_CPUID_7             0x80000007
#define AMD_EXT_CPUID_8             0x80000008
#define AMD_CPU_NAME_CPUID_1        AMD_EXT_CPUID_2
#define AMD_CPU_NAME_CPUID_2        AMD_EXT_CPUID_3
#define AMD_CPU_NAME_CPUID_3        AMD_EXT_CPUID_4
#define AMD_L1_CACHE_CPUID          AMD_EXT_CPUID_5
#define AMD_L2_CACHE_CPUID          AMD_EXT_CPUID_6
#define AMD_MSR_MCODE_SIGNATURE     0x8b

static UINT32  MtrrRegs[] = {
    MTRR_FIX_64K_00000,
    MTRR_FIX_16K_80000, MTRR_FIX_16K_A0000,
    MTRR_FIX_4K_C0000,  MTRR_FIX_4K_C8000,
    MTRR_FIX_4K_D0000,  MTRR_FIX_4K_D8000,
    MTRR_FIX_4K_E0000,  MTRR_FIX_4K_E8000,
    MTRR_FIX_4K_F0000,  MTRR_FIX_4K_F8000,
    MTRR_PHYS_BASE_0,   MTRR_PHYS_MASK_0,
    MTRR_PHYS_BASE_1,   MTRR_PHYS_MASK_1,
    MTRR_PHYS_BASE_2,   MTRR_PHYS_MASK_2, 
    MTRR_PHYS_BASE_3,   MTRR_PHYS_MASK_3,
    MTRR_PHYS_BASE_4,   MTRR_PHYS_MASK_4,
    MTRR_PHYS_BASE_5,   MTRR_PHYS_MASK_5,
    MTRR_PHYS_BASE_6,   MTRR_PHYS_MASK_6, 
    MTRR_PHYS_BASE_7,   MTRR_PHYS_MASK_7,
    AMD_MSR_SMM_ADDR_HL, AMD_MSR_SMM_MASK_HL,
    AMD_MSR_IORR_BASE0, AMD_MSR_IORR_MASK0,
    AMD_MSR_IORR_BASE1, AMD_MSR_IORR_MASK1,
    MSR_SYS_CFG,
    AMD_MSR_MMIO_CFG_BASE,
    AMD_MSR_SMI_TRIGGER_IO
};

#define NUM_OF_MTRR_SAVED	(sizeof(MtrrRegs)/sizeof(UINT32))


// Define MSR
EFI_STATUS EfiCpuFlushDataCache (
    IN EFI_CPU_ARCH_PROTOCOL    *This,
    IN EFI_PHYSICAL_ADDRESS     Start,
    IN UINT64                   Length,
    IN EFI_CPU_FLUSH_TYPE       FlushType );

EFI_STATUS EfiCpuSetMemoryAttributes (
    IN EFI_CPU_ARCH_PROTOCOL    *This,
    IN EFI_PHYSICAL_ADDRESS     BaseAddress,
    IN UINT64                   Length,
    IN UINT64                   Attributes );

typedef	struct 
{
    UINT16  wInstalled_Processor_Revision_ID;
    UINT16  wEquivalent_Processor_Revision_ID;
} EQUIVALENT_PROCESSOR_TABLE_STRUC;

EFI_STATUS MPSynchronizeMTRRs (
#ifdef PEI_COMPILE
	IN EFI_PEI_SERVICES		**PeiServices,
#endif
    IN void *MpData );

UINT8 StartAllAps (
#ifdef PEI_COMPILE
	IN EFI_PEI_SERVICES	**PeiServices,
	IN EFI_PEI_STALL_PPI *PeiStall,
#endif
	OUT VOID **MpData );

EFI_STATUS PreAPStartup (
#ifdef PEI_COMPILE
	IN EFI_PEI_SERVICES	**PeiServices,
	IN EFI_PEI_STALL_PPI *PeiStall,
#endif
	OUT VOID **MpData );

EFI_STATUS PostAPCleanup (
#ifdef PEI_COMPILE
	IN EFI_PEI_SERVICES	**PeiServices,
	IN EFI_PEI_STALL_PPI *PeiStall,
#endif
	IN VOID **MpData );

VOID RestartAp (
#ifdef PEI_COMPILE
	IN EFI_PEI_SERVICES		**PeiServices,
	IN EFI_PEI_STALL_PPI	*PeiStall,
#endif
	IN VOID		*MpData,
	IN UINT8	Cpu );

UINT8 GetNumCpus (
    VOID *MpData );

VOID ExecuteFunctionOnRunningCpus(
	IN VOID 	*MpData,
	IN BOOLEAN	ExecuteOnBsp,
	IN BOOLEAN	Block,
	IN VOID 	(*Function)(UINT8 Cpu, VOID *Context),
	IN VOID 	*Context
);

BOOLEAN ExecuteFunctionOnCpu(
	IN VOID 	*MpData,
	IN UINT8 	Cpu,
	IN BOOLEAN	Block,
	IN VOID 	(*Function)(UINT8 Cpu, VOID *Context),
	IN VOID 	*Context
);

VOID HaltAllAps(
	IN VOID *MpData,
	IN BOOLEAN WaitUntilHalted
);
VOID HaltCpu(
	IN VOID *MpData,
	IN UINT8 Cpu,
	IN BOOLEAN WaitUntilHalted
);

BOOLEAN IsCpuHalted(
	IN VOID *MpData,
	IN UINT8 Cpu
);

BOOLEAN IsCpuIdle(
	IN VOID *MpData,
	IN UINT8 Cpu
);

UINT8 WhoIsBsp(VOID *MpData);

VOID SwitchBsp(
	IN VOID *MpData,
	IN UINT8 NewBsp,
	IN VOID (*Function)(UINT8 Cpu, VOID *Context),  OPTIONAL
	IN VOID *Context OPTIONAL
);

typedef struct {
	EFI_HOB_GUID_TYPE		EfiHobGuidType;
	UINT64					VarMTRRBase[7];
	UINT64					VarMTRRMask[7];
} MTRR_HOB;


//------------------------------------------------------------------------
#endif


