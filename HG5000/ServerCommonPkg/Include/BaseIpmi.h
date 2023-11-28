/** @file
 Main header file for all IPMI drivers.
*/

#ifndef _IPMI_H_
#define _IPMI_H_

#include "IpmiIndustrialstandard.h"

#pragma pack(1)

#define IPMI_FUNCTION_ENABLE                  1
#define IPMI_FUNCTION_DISABLE                 0

//
// Define IPMI Version default 00h, in spec. 00h is reserved.
// If BMC doesn't work, the IPMI Version will be set as 00h
//
#define IPMI_VERSION_DEFAULT                   0x00

//
// System Interface Type
//
typedef enum {
  IPMI_UNKNOWN,
  IPMI_KCS,
  IPMI_SMIC,
  IPMI_BT,
  IPMI_SSIF,
  IPMI_MAX_INTERFACE_TYPE
} IPMI_INTERFACE_TYPE;

//
// System Interface Address Type
//
typedef enum {
  IPMI_MEMORY,
  IPMI_IO,
  IPMI_MAX_INTERFACE_ADDRESS_TYPE
} IPMI_INTERFACE_ADDRESS_TYPE;

//
// BMC Status
//
typedef enum {
  BMC_STATUS_UNKNOWN,
  BMC_STATUS_OK,
  BMC_STATUS_NOT_READY,
  BMC_STATUS_NOT_FOUND,
  BMC_STATUS_ERROR
} IPMI_BMC_STATUS;

//
// Address/Offset Sturcture
//
typedef struct {
  UINT16  Addr;
  UINT16  Offset;
} IPMI_INTERFACE_ADDRESS;

//
// Structure to store IPMI Network Function, LUN and command
//
typedef struct {
  UINT8 Lun   : 2;
  UINT8 NetFn : 6;
  UINT8 Cmd;
} IPMI_CMD_HEADER;

//
// Completion code
//
#define IPMI_COMPLETE_CODE_NO_ERROR               0x00
#define IPMI_RESPONSE_NETFN_BIT                   0x01

//
// LUN
//
#define IPMI_BMC_LUN               0x00

//
// Net Function Definition
//

#define IPMI_NETFN_APPLICATION     0x06
#define IPMI_NETFN_TRANSPORT       0x0C

//
// Enable Status Code
//
#define STATUS_CODE_ENABLE         0x8000

//
// Max number of base address
//
#define IPMI_MAX_BASE_ADDRESS_NUM     2
#define IPMI_POST_BASE_ADDRESS_INDEX  0
#define IPMI_OS_BASE_ADDRESS_INDEX    1

//
// KCS Interface Control Codes
//
#define KCS_CC_GET_STATUS_ABORT   0x60
#define KCS_CC_WRITE_START        0x61
#define KCS_CC_WRITE_END          0x62
#define KCS_CC_READ               0x68

//
// KCS Interface Status Codes
//
#define KCS_SC_NO_ERROR             0x00
#define KCS_SC_ABORTED_BY_COMMAND   0x01
#define KCS_SC_ILLEGAL_CONTROL_CODE 0x02
#define KCS_SC_LENGTH_ERROR         0x06
#define KCS_SC_UNSPECIFIED_ERROR    0xFF

//
// KCS Interface Status Bits
//
#define KCS_IDLE_STATE      0x00
#define KCS_READ_STATE      0x01
#define KCS_WRITE_STATE     0x02
#define KCS_ERROR_STATE     0x03

//
// RETRY_LIMIT in Error Exit
//
#define KCS_RETRY_LIMIT    1

//
// KCS Interface Status Register
//
typedef  struct {
  UINT8 Obf    : 1;
  UINT8 Ibf    : 1;
  UINT8 SmsAtn : 1;
  UINT8 CD     : 1;
  UINT8 Oem1   : 1;
  UINT8 Oem2   : 1;
  UINT8 S0     : 1;
  UINT8 S1     : 1;  
} KCS_STATUS_REGISTER;

//
// SMIC System Interface Register
//
#define SMIC_REG_FLAGS             0x02
#define SMIC_REG_CONTROL_STATUS    0x01
#define SMIC_REG_DATA              0x00

//
// Provides the arbitration mechanism for SMIC mailbox register access.
// This bit is only set (1) from the system side and only cleared (0) by the BMC.
//
#define SMIC_FLAGS_REG_BUSY_BIT    0x01

//
// SMIC Flags Register
//
typedef  struct {
  UINT8 Busy       : 1;
  UINT8 Reserved1  : 1;
  UINT8 SmsAtn     : 1;
  UINT8 EvtAtn     : 1;
  UINT8 Smi        : 1;
  UINT8 Reserved2  : 1;
  UINT8 TxDataRdy  : 1;
  UINT8 RxDataRdy  : 1;
} SMIC_FLAGS_REGISTER;

//
// SMIC Interface Control Code
//
#define SMIC_CC_SMS_GET_STATUS   0x40
#define SMIC_CC_SMS_WR_START     0x41
#define SMIC_CC_SMS_WR_NEXT      0x42
#define SMIC_CC_SMS_WR_END       0x43
#define SMIC_CC_SMS_RD_START     0x44
#define SMIC_CC_SMS_RD_NEXT      0x45
#define SMIC_CC_SMS_RD_END       0x46

//
// SMIC Interface Status Code
//
#define SMIC_SC_SMS_RDY         0xC0
#define SMIC_SC_SMS_WR_START    0xC1
#define SMIC_SC_SMS_WR_NEXT     0xC2
#define SMIC_SC_SMS_WR_END      0xC3
#define SMIC_SC_SMS_RD_START    0xC4
#define SMIC_SC_SMS_RD_NEXT     0xC5
#define SMIC_SC_SMS_RD_END      0xC6

//
// SMIC Interface Error Code
//
#define SMIC_EC_NO_ERROR        0x00
#define SMIC_EC_ABORTED         0x01
#define SMIC_EC_ILLEGAL_CC      0x02
#define SMIC_EC_NO_RESPONSE     0x03
#define SMIC_EC_ILLEGAL_CMD     0x04
#define SMIC_EC_BUFFER_FULL     0x05


//
// BT System Interface Register
//
#define BT_REG_CTRL             0x00
#define BT_REG_BMC2HOST         0x01
#define BT_REG_HOST2BMC         0x01
#define BT_REG_INTMASK          0x02

//
// BT Control Register
//
typedef  struct {
  UINT8 ClrWrPtr  : 1;
  UINT8 ClrRdPtr  : 1;
  UINT8 H2bAtn    : 1;
  UINT8 B2hAtn    : 1;
  UINT8 SmsAtn    : 1;
  UINT8 Oem0      : 1;
  UINT8 HBusy     : 1;
  UINT8 BBusy     : 1;
} BT_CONTROL_REGISTER;

//
// Command Length
//
#define BT_CMD_MIN_LEN     3

//
// BT Control Register Definition
//
#define BT_CTRL_CLR_WR_PTR    0x01
#define BT_CTRL_CLR_RD_PTR    0x02
#define BT_CTRL_H2B_ATN       0x04
#define BT_CTRL_B2H_ATN       0x08
#define BT_CTRL_H_BUSY        0x40
#define BT_CTRL_B_BUSY        0x80

#pragma pack()

#endif
