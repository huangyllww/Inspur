
#ifndef __CPU_DXE_H__
#define __CPU_DXE_H__

#ifndef VFRCOMPILE
  #include <Library\HobLib.h>
  #include <Protocol\Cpu.h>
  #include <HygonCpu.h>
  #include <Library/MtrrLib.h>
  #include <Library/UefiBootServicesTableLib.h>
  #include <Library/UefiRuntimeServicesTableLib.h>
  #include <Library/DebugLib.h>
  #include <Library/IoLib.h>
  #include <Library/PciLib.h>
  #include <Library/BaseMemoryLib.h>
  #include <Library/UefiLib.h>
  #include <Protocol/MpService.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


#define USE_AP_HLT 1

#ifndef VFRCOMPILE
#ifdef EFI_DEBUG


// return EFI_STATUS on error
#ifndef ABORT_CALL
#define ABORT_CALL(FunctionCall) { \
    EFI_STATUS Status = FunctionCall; \
    if ( EFI_ERROR( (Status) ) ) \
    { \
      DEBUG((EFI_D_ERROR, \
        "ABORT in %s on %i: %s\n",__FILE__, __LINE__, #FunctionCall)); \
      return Status; \
    } }
#endif

// override EFI_STATUS return value on error
#ifndef ABORT_CALL_STATUS
#define ABORT_CALL_STATUS(Status, FunctionCall) { \
    if ( EFI_ERROR( (FunctionCall) ) ) \
    { \
      DEBUG((EFI_D_ERROR, \
        "ABORT in %s on %i: %s\n",__FILE__, __LINE__, #FunctionCall)); \
      return Status; \
    } }
#endif

// force hang on error
#ifndef ASSERT_CALL
#define ASSERT_CALL(FunctionCall) ASSERT_EFI_ERROR(FunctionCall)
#endif

#else // EFI_DEBUG is not defined

// return EFI_STATUS on error
#ifndef ABORT_CALL
#define ABORT_CALL(FunctionCall) { \
    EFI_STATUS Status = FunctionCall; \
    if (EFI_ERROR(Status)) \
      return Status; \
    }
#endif

// override EFI_STATUS return value on error
#ifndef ABORT_CALL_STATUS
#define ABORT_CALL_STATUS(Status, FunctionCall) { \
  if (EFI_ERROR(FunctionCall)) \
    return Status; \
  }
#endif

// force hang on error
#ifndef ASSERT_CALL
#define ASSERT_CALL(FunctionCall) { \
  if (EFI_ERROR(FunctionCall)) \
    CpuDeadLoop(); \
  }
#endif

#endif // #ifdef EFI_DEBUG
#endif // #ifndef VFRCOMPILE

#define DELAY_CALCULATE_CPU_PERIOD  200    //uS

#pragma pack (push, 1)


typedef struct
{
    UINT16  LoOffset;
    UINT16  Segment;
    UINT16  DescBits;   //Will be set to 0x0f00 Present=1, DPL=0, D=1 (32bit)
    UINT16  HiOffset;
#ifdef EFIx64
    UINT32  Hi32Offset;
    UINT32  Rsv;
#endif
} INTR_GATE_DESC;

//This structure is used for setup.
typedef struct
{
  UINT8  XDBitAvailable;
  UINT8  HTAvailable;
  UINT8  VTAvailable;
  UINT8  LimitCpuidAvailable;
    UINT8   NodeCount;
    UINT8   DualNode;
    UINT8   PopScktCount;
  //UINT8  EnhancedDebugAvailable;
} SETUP_CPU_FEATURES;
#pragma pack (pop)

#ifndef VFRCOMPILE
// Define structures used and referenced in this file
#pragma pack (push, 1)
typedef struct{
  UINT32 MemAddress, MemLength;
} system_memory_struc;
#pragma pack (pop)

EFI_STATUS EfiCpuFlushDataCache (
    IN EFI_CPU_ARCH_PROTOCOL    *This,
    IN EFI_PHYSICAL_ADDRESS     Start,
    IN UINT64                   Length,
    IN EFI_CPU_FLUSH_TYPE       FlushType);

EFI_STATUS EfiCpuSetMemoryAttributes (
    IN EFI_CPU_ARCH_PROTOCOL    *This,
    IN EFI_PHYSICAL_ADDRESS     BaseAddress,
    IN UINT64                   Length,
    IN UINT64                   Attributes);

VOID EnableC1E();


#endif // #ifndef VFRCOMPILE

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

