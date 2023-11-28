/************************
*UFUDev.h
************************/
#ifndef _UFUDEV_H_
#define _UFUDEV_H_

#ifndef UFUDEV_NR_DEVS
#define UFUDEV_NR_DEVS 1 /*设备数*/
#endif

//
// Macro definition for defining IOCTL and FSCTL function control codes. 
//

#undef FILE_DEVICE_UFU
#undef UFU_IOCTL_INDEX
#undef METHOD_BUFFERED
#undef FILE_ANY_ACCESS

#define FILE_DEVICE_UFU                 0x00008010
#define UFU_IOCTL_INDEX                 0x810
#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define FILE_ANY_ACCESS                 0

#undef CTL_CODE
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define  SMIPort = 0x82f;//for VIA.

#ifdef __i386__
typedef unsigned long ADDRESS;
#elif __x86_64__
typedef unsigned long long ADDRESS;
#endif



// Define our own private IOCTL

#define IOCTL_UFU_MEM_ALLOCATE           CTL_CODE(FILE_DEVICE_UFU,  \
                                     UFU_IOCTL_INDEX,           \
                                     METHOD_BUFFERED,           \
                                     FILE_ANY_ACCESS)

#define IOCTL_UFU_MEM_FREE           CTL_CODE(FILE_DEVICE_UFU,  \
                                     UFU_IOCTL_INDEX + 1,       \
                                     METHOD_BUFFERED,           \
                                     FILE_ANY_ACCESS)

#define IOCTL_UFU_PHYSICAL_ADDRESS   CTL_CODE(FILE_DEVICE_UFU,  \
                                     UFU_IOCTL_INDEX + 2,       \
                                     METHOD_BUFFERED,           \
                                     FILE_ANY_ACCESS)

#define IOCTL_UFU_MEM_READ           CTL_CODE(FILE_DEVICE_UFU,  \
                                     UFU_IOCTL_INDEX + 3,       \
                                     METHOD_OUT_DIRECT,         \
                                     FILE_ANY_ACCESS)

#define IOCTL_UFU_MEM_WRITE          CTL_CODE(FILE_DEVICE_UFU,  \
                                     UFU_IOCTL_INDEX + 4,       \
                                     METHOD_IN_DIRECT,          \
                                     FILE_ANY_ACCESS)

#define IOCTL_UFU_PAGE_SIZE          CTL_CODE(FILE_DEVICE_UFU,  \
                                     UFU_IOCTL_INDEX + 5,       \
                                     METHOD_BUFFERED,           \
                                     FILE_ANY_ACCESS)

#define IOCTL_UFU_SMI_CALL           CTL_CODE(FILE_DEVICE_UFU,  \
                                     UFU_IOCTL_INDEX + 6,       \
                                     METHOD_BUFFERED,           \
                                     FILE_ANY_ACCESS)

#define IOCTL_UFU_RD_MSR64           CTL_CODE(FILE_DEVICE_UFU,  \
                                     UFU_IOCTL_INDEX + 7,       \
                                     METHOD_BUFFERED,           \
                                     FILE_ANY_ACCESS)

#define IOCTL_UFU_RD_PHYSICAL_MEM    CTL_CODE(FILE_DEVICE_UFU,  \
                                     UFU_IOCTL_INDEX + 8,       \
                                     METHOD_OUT_DIRECT,           \
                                     FILE_ANY_ACCESS)
//end of add.


/*mem设备描述结构体*/
struct mem_dev 
{ 
  char *data; 
  unsigned long size; 
};

typedef struct {
  unsigned long                Offset;              // IN
  unsigned long                BufferSize;          // IN, OUT
  loff_t  Buffer;              // OUT
} K_BLOCK_READ_PARAMETER;

typedef struct {
  unsigned long         Type;                       // IN, FD_AREA_TYPE, Type of flash area
  unsigned long         Offset;                     // OUT, Offset from FD base address or base address for FD_AREA_TYPE_FD
  unsigned long         Size;                       // OUT, Byte size of flash area
} K_FD_AREA_INFO;


//
// Data Structures
//
typedef struct{
    ADDRESS     dst;
    ADDRESS     src;
    unsigned long     size;
} UFU_MEM_OPERATION_BUFFER;

/**
typedef struct {
    unsigned long cr0;
    unsigned long cr1;
    unsigned long cr2;
} UFU_MSR_INFO;
**/

typedef struct{
    unsigned long       func;
    unsigned long       faddr;
    unsigned long       smiport;
} UFU_SMI_BUFFER;


#endif /* _UFUDEV_H_ */
