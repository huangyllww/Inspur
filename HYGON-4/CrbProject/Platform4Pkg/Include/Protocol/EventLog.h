/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  EventLog.h
Abstract:
  EFI Event Log
Revision History:

$END----------------------------------------------------------------------------
*/

#ifndef _EFI_EVENT_LOG_H_
#define _EFI_EVENT_LOG_H_


//#include <EventLogSetup.h>

#define HEX2BCD8(n) ((((n) / 10) * 16) + (n) % 10)
#define BCD82HEX(n) ((((n) / 16) * 10) + (n) % 16)

typedef enum {
  ELOG_SPACE_VALID = 0x01,
  ELOG_SPACE_FULL  = 0x02,
} ELOG_SPACE_STATUS;

//
// System Management BIOS (SMBIOS) Reference Specification(Version 2.6.1)
// 3.3.16.5 Log Header Format
//
typedef enum {
  FORMAT_TYPE_NO_HEADER,
  FORMAT_TYPE_TYPE_1_LOG_HEADER
} LOG_HEADER_FORMAT_TYPE;

//
// 3.3.16.6.1 Event Log Types
//
typedef enum {
  EVENT_LOG_RESERVED,              // 0x00
  EVENT_LOG_S_ECC_ERROR,           // 0x01
  EVENT_LOG_M_ECC_ERROR,           // 0x02
  EVENT_LOG_PARITY_ERROR,          // 0x03
  EVENT_LOG_BUS_TIMEOUT,           // 0x04
  EVENT_LOG_IO_CHECK,              // 0x05
  EVENT_LOG_SOFTWARE_NMI,          // 0x06
  EVENT_LOG_POST_MEM_RESIZE,       // 0x07
  EVENT_LOG_POST_ERROR,            // 0x08
  EVENT_LOG_PCI_PARITY_ERROR,      // 0x09
  EVENT_LOG_PCI_SYSTEM_ERROR,      // 0x0a
  EVENT_LOG_CPU_FAILURE,           // 0x0b
  EVENT_LOG_EISA_TIMEOUT,          // 0x0c
  EVENT_LOG_MEMORY_LOG_DISABLED,   // 0x0d
  EVENT_LOG_LOGGING_DISABLED,      // 0x0e
  EVENT_LOG_RESERVED1,             // 0x0f
  EVENT_LOG_SYSTEM_LIMIT_EXCEEDED, // 0x10
  EVENT_LOG_HARDWARE_TIMER_EXPIRED,// 0x11
  EVENT_LOG_SYSTEM_CONFIG_INFO,    // 0x12
  EVENT_LOG_HDD_INFO,              // 0x13
  EVENT_LOG_SYSTEM_RECONFIG,       // 0x14
  EVENT_LOG_CPU_COMPLEX_ERROR,     // 0x15
  EVENT_LOG_LOG_CLEARED,           // 0x16
  EVENT_LOG_SYSTEM_BOOT            // 0x17
} ENUM_EVENT_LOG_TYPES;

//
// 3.3.16.6.2 Event Log Variable Data Format Types
//
typedef enum {
  EVENT_LOG_DATA_NONE,              // 0x00
  EVENT_LOG_DATA_HANDLE,
  EVENT_LOG_DATA_MULTIPLE_EVENT,
  EVENT_LOG_DATA_MULTIPLE_EVENT_HANDLE,
  EVENT_LOG_DATA_POST_RESULT_BITMAP,
  EVENT_LOG_DATA_SYS_MANAGEMENT_MAP,
  EVENT_LOG_DATA_ME_SYS_MANAGEMENT_MAP,
  EVENT_LOG_DATA_UNUSED
} ENUM_EVENT_LOG_DATA_FORMAT_TYPES;

//
// 3.3.16.6.3.1 POST Results Bitmap
//
// First DWORD
#define Channel_2_Timer_error                       0x00000001  // bit0
#define Master_PIC_8259_1_error                     0x00000002  // bit1
#define Slave_PIC_8259_2_error                      0x00000004  // bit2
#define CMOS_Battery_Failure                        0x00000008  // bit3
#define CMOS_System_Options_Not_Set                 0x00000010  // bit4
#define CMOS_Checksum_Error                         0x00000020  // bit5
#define CMOS_Configuration_Error                    0x00000040  // bit6
#define Mouse_and_Keyboard_Swapped                  0x00000080  // bit7
#define Keyboard_Locked                             0x00000100  // bit8
#define Keyboard_Not_Functional                     0x00000200  // bit9
#define Keyboard_Controller_Not_Functional          0x00000400  // bit10
#define CMOS_Memory_Size_Different                  0x00000800  // bit11
#define Memory_Decreased_in_Size                    0x00001000  // bit12
#define Cache_Memory_Error                          0x00002000  // bit13
#define Floppy_Drive_0_Error                        0x00004000  // bit14
#define Floppy_Drive_1_Error                        0x00008000  // bit15
#define Floppy_Controller_Failure                   0x00010000  // bit16
#define Number_of_ATA_Drives_Reduced_Error          0x00020000  // bit17
#define CMOS_Time_Not_Set                           0x00040000  // bit18
#define DDC_Monitor_Configuration_Change            0x00080000  // bit19
#define Reserved20                                  0x00100000  // bit20
#define Reserved21                                  0x00200000  // bit21
#define Reserved22                                  0x00400000  // bit22
#define Reserved23                                  0x00800000  // bit23
#define Second_DWORD_has_valid_data                 0x01000000  // bit24
#define Reserved25                                  0x02000000  // bit25
#define Reserved26                                  0x04000000  // bit26
#define Reserved27                                  0x08000000  // bit27
#define For_OEM_assignment_28                       0x10000000  // bit28
#define For_OEM_assignment_29                       0x20000000  // bit29
#define For_OEM_assignment_30                       0x40000000  // bit30
#define For_OEM_assignment_31                       0x80000000  // bit31

// Second DWORD
#define For_OEM_assignment_32                       0x00000001  // bit0 
#define For_OEM_assignment_33                       0x00000002  // bit1 
#define For_OEM_assignment_34                       0x00000004  // bit2 
#define For_OEM_assignment_35                       0x00000008  // bit3 
#define For_OEM_assignment_36                       0x00000010  // bit4 
#define For_OEM_assignment_37                       0x00000020  // bit5 
#define For_OEM_assignment_38                       0x00000040  // bit6 
#define PCI_Memory_Conflict                         0x00000080  // bit7 
#define PCI_IO_Conflict                             0x00000100  // bit8 
#define PCI_IRQ_Conflict                            0x00000200  // bit9 
#define PNP_Memory_Conflict                         0x00000400  // bit10
#define PNP_32_bit_Memory_Conflict                  0x00000800  // bit11
#define PNP_IO_Conflict                             0x00001000  // bit12
#define PNP_IRQ_Conflict                            0x00002000  // bit13
#define PNP_DMA_Conflict                            0x00004000  // bit14
#define Bad_PNP_Serial_ID_Checksum                  0x00008000  // bit15
#define Bad_PNP_Resource_Data_Checksum              0x00010000  // bit16
#define Static_Resource_Conflict                    0x00020000  // bit17
#define NVRAM_Checksum_Error_NVRAM_Cleared          0x00040000  // bit18
#define System_Board_Device_Resource_Conflict       0x00080000  // bit19
#define Primary_Output_Device_Not_Found             0x00100000  // bit20
#define Primary_Input_Device_Not_Found              0x00200000  // bit21
#define Primary_Boot_Device_Not_Found               0x00400000  // bit22
#define NVRAM_Cleared_By_Jumper                     0x00800000  // bit23
#define NVRAM_Data_Invalid_NVRAM_Cleared            0x01000000  // bit24
#define FDC_Resource_Conflict                       0x02000000  // bit25
#define Primary_ATA_Controller_Resource_Conflict    0x04000000  // bit26
#define Secondary_ATA_Controller_Resource_Conflict  0x08000000  // bit27
#define Parallel_Port_Resource_Conflict             0x10000000  // bit28
#define Serial_Port_1_Resource_Conflict             0x20000000  // bit29
#define Serial_Port_2_Resource_Conflict             0x40000000  // bit30
#define Audio_Resource_Conflict                     0x80000000  // bit31

#pragma pack(1)

//
// 3.3.16.1 Supported Event Log Type Descriptors
//
typedef struct {
  UINT8  Log_Type;
  UINT8  Data_Format;
} EventLog_TypeDescriptors;

//  
// 3.3.16.5.1 Log Header Type 1 Format
//
typedef struct {
  UINT8  OEMReserved[5];
  UINT8  METime;
  UINT8  MECount;
  UINT8  CMOSAddress;
  UINT8  CMOSBitIndex;
  UINT8  StartOffset;
  UINT8  ByteCount;
  UINT8  ChecksumOffset;
  UINT8  Reserved0[3];
  UINT8  Revision;
} LOG_HEADER_TYPE_1;

typedef struct {
  UINT16  year;
  UINT8   month;
  UINT8   day;
  UINT8   hour;
  UINT8   minute;
  UINT8   second;
} EVENT_LOG_DATE_TIME;

//
// 3.3.16.6 Log Record Format
//
typedef struct {
  UINT16              ErrorCode;
  EVENT_LOG_DATE_TIME DateTime;
} EVENT_LOG_RECORD;
#pragma pack()



typedef struct _EFI_EVENT_LOG_PROTOCOL EFI_EVENT_LOG_PROTOCOL;

typedef
void *
(EFIAPI *EFI_EVENT_LOG_PROTOCOL_GET) ();
  
typedef
EFI_STATUS
(EFIAPI *EFI_EVENT_LOG_PROTOCOL_INSERT) (
  IN     UINT16                         Type,
  IN     EFI_TIME                       *Time OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *EFI_EVENT_LOG_PROTOCOL_CLEAR) ();

struct _EFI_EVENT_LOG_PROTOCOL {
  EFI_EVENT_LOG_PROTOCOL_GET           Get;
  EFI_EVENT_LOG_PROTOCOL_INSERT        Insert;
  EFI_EVENT_LOG_PROTOCOL_CLEAR         Clear;
};

extern EFI_GUID gEfiEventLogProtocolGuid;

#endif
