
#ifndef __CPU_BOARD_H__
#define __CPU_BOARD_H__

#include <Uefi.h>
#include <HygonCpu.h>
//#include <Framework/FrameworkInternalFormRepresentation.h>
//#include <Guid/DataHubRecords.h>
#include <Library/IoLib.h>

#ifdef __cplusplus
extern "C" {
#endif



//Size
#define EFI_CACHE_SIZE_RECORD_NUMBER      0x00000001

//Maximum Size
#define EFI_MAXIMUM_CACHE_SIZE_RECORD_NUMBER  0x00000002

//Speed
#define EFI_CACHE_SPEED_RECORD_NUMBER      0x00000003

//Socket
#define EFI_CACHE_SOCKET_RECORD_NUMBER      0x00000004

//SRAM Type Supported
#define EFI_CACHE_SRAM_SUPPORT_RECORD_NUMBER  0x00000005


//SRAM Type Installed
#define EFI_CACHE_SRAM_INSTALL_RECORD_NUMBER  0x00000006

//Error Correction Type Supported
#define EFI_CACHE_ERROR_SUPPORT_RECORD_NUMBER  0x00000007

//Type
#define EFI_CACHE_TYPE_RECORD_NUMBER      0x00000008

//Associativity
#define EFI_CACHE_ASSOCIATIVITY_RECORD_NUMBER  0x00000009

//Configuration
#define EFI_CACHE_CONFIGURATION_RECORD_NUMBER  0x0000000A





// These structures/constants are used to initialize CACHE_DESCRIPTOR_INFO
// structures to describe each CPU's cache structures in InitializeCpu()
typedef enum   {
    EAX  = 0,
    EBX  = 1,
    ECX  = 2,
    EDX  = 3,
} REGISTER_TYPE;


typedef UINT32 REGISTER_TABLE[4];

EFI_STATUS BoardCpuInit();

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

