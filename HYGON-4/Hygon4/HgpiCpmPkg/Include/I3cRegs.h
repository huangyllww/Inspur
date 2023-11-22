/*****************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 *
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 ******************************************************************************
 */

#ifndef _I3C_REGS_H_
#define _I3C_REGS_H_

/*
 * Registers offset
 */
#define DEVICE_CTRL                  (0x0)
#define DEVICE_ADDR                  (0x4)
#define HW_CAPABILITY                (0x8)
#define COMMAND_QUEUE_PORT           (0xc)
#define RESPONSE_QUEUE_PORT          (0x10)
#define TX_DATA_PORT                 (0x14)
#define RX_DATA_PORT                 (0x14)
#define IBI_QUEUE_STATUS             (0x18)
#define IBI_QUEUE_DATA               (0x18)
#define QUEUE_THLD_CTRL              (0x1c)
#define DATA_BUFFER_THLD_CTRL        (0x20)
#define IBI_QUEUE_CTRL               (0x24)
#define IBI_MR_REQ_REJECT            (0x2c)
#define IBI_SIR_REQ_REJECT           (0x30)
#define RESET_CTRL                   (0x34)
#define SLV_EVENT_STATUS             (0x38)
#define INTR_STATUS                  (0x3c)
#define INTR_STATUS_EN               (0x40)
#define INTR_SIGNAL_EN               (0x44)
#define INTR_FORCE                   (0x48)
#define QUEUE_STATUS_LEVEL           (0x4c)
#define DATA_BUFFER_STATUS_LEVEL     (0x50)
#define PRESENT_STATE                (0x54)
#define CCC_DEVICE_STATUS            (0x58)
#define DEVICE_ADDR_TABLE_POINTER    (0x5c)
#define DEV_CHAR_TABLE_POINTER       (0x60)
#define VENDOR_SPECIFIC_REG_POINTER  (0x6c)
#define SLV_MIPI_ID_VALUE            (0x70)
#define SLV_PID_VALUE                (0x74)
#define SLV_CHAR_CTRL                (0x78)
#define SLV_MAX_LEN                  (0x7c)
#define MAX_READ_TURNAROUND          (0x80)
#define MAX_DATA_SPEED               (0x84)
#define SLV_INTR_REQ                 (0x8c)
#define SLV_TSX_SYMBL_TIMING         (0x90)
#define SLV_SIR_DATA                 (0x94)
#define SLV_IBI_RESP                 (0x98)
#define SLV_NACK_REQ                 (0x9c)
#define SLV_NACK_CONF                (0xa0)
#define SLV_INSTANT_STATUS           (0xa4)
#define DEVICE_CTRL_EXTENDED         (0xb0)
#define SCL_I3C_OD_TIMING            (0xb4)
#define SCL_I3C_PP_TIMING            (0xb8)
#define SCL_I2C_FM_TIMING            (0xbc)
#define SCL_I2C_FMP_TIMING           (0xc0)
#define SCL_EXT_LCNT_TIMING          (0xc8)
#define SCL_EXT_TERMN_LCNT_TIMING    (0xcc)
#define SDA_HOLD_SWITCH_DLY_TIMING   (0xd0)
#define BUS_FREE_AVAIL_TIMING        (0xd4)
#define BUS_IDLE_TIMING              (0xd8)
#define SCL_LOW_MST_EXT_TIMEOUT      (0xdc)
#define I3C_VER_ID                   (0xe0)
#define I3C_VER_TYPE                 (0xe4)
#define QUEUE_SIZE_CAPABILITY        (0xe8)
#define DEV_CHAR_TABLE1_LOC1         (0x200)
#define SEC_DEV_CHAR_TABLE1          (0x200)
#define DEV_CHAR_TABLE1_LOC2         (0x204)
#define DEV_CHAR_TABLE1_LOC3         (0x208)
#define DEV_CHAR_TABLE1_LOC4         (0x20c)
#define DEV_ADDR_TABLE_LOC1          (0x220)
#define DEV_ADDR_TABLE1_LOC1         (0x220)

// todo, I3c Timeout how to set ??
#define STATUS_WAIT_RETRY  (10)

#define HYGON_I3C_MS_DELAY  (1)

// INTR_STATUS
#define BUS_RESET_DONE_STS_MASK    (1 << 15)
#define BUSOWNER_UPDATED_STS_MASK  (1 << 13)
#define IBI_UPDATED_STS_MASK       (1 << 12)
#define READ_REQ_RECV_STS_MASK     (1 << 11)
#define DEFSLV_STS_MASK            (1 << 10)
#define TRANSFER_ERR_STS_MASK      (1 << 9)
#define DYN_ADDR_ASSGN_STS_MASK    (1 << 8)
#define CCC_UPDATED_STS_MASK       (1 << 6)
#define TRANSFER_ABORT_STS_MASK    (1 << 5)
#define INTR_STATUS_CLEAR          (TRANSFER_ABORT_STS_MASK | CCC_UPDATED_STS_MASK | DYN_ADDR_ASSGN_STS_MASK | TRANSFER_ERR_STS_MASK | \
                                    DEFSLV_STS_MASK | READ_REQ_RECV_STS_MASK | IBI_UPDATED_STS_MASK | BUSOWNER_UPDATED_STS_MASK | BUS_RESET_DONE_STS_MASK)

typedef union {
  struct {
    UINT32    TX_THLD_STS          : 1; // bit0
    UINT32    RX_THLD_STS          : 1; // bit1
    UINT32    IBI_THLD_STS         : 1; // bit2
    UINT32    CMD_QUEUE_READY_STS  : 1; // bit3
    UINT32    RESP_READY_STS       : 1; // bit4
    UINT32    TRANSFER_ABORT_STS   : 1; // bit5
    UINT32    CCC_UPDATED_STS      : 1; // bit6
    UINT32    Rsvd0                : 1; // bit7
    UINT32    DYN_ADDR_ASSGN_STS   : 1; // bit8
    UINT32    TRANSFER_ERR_STS     : 1; // bit9
    UINT32    DEFSLV_STS           : 1; // bit10
    UINT32    READ_REQ_RECV_STS    : 1; // bit11
    UINT32    IBI_UPDATED_STS      : 1; // bit12
    UINT32    BUSOWNER_UPDATED_STS : 1; // bit13
    UINT32    Rsvd1                : 1; // bit14
    UINT32    BUS_RESET_DONE_STS   : 1; // bit15
    UINT32    Rsvd2                : 16; // bit31:16
  } Field;
  UINT32    Value;
} I3C_INTR_STATUS;

// INTR_STATUS_EN
#define BUS_RESET_DONE_STS_EN    0x8000
#define BUSOWNER_UPDATED_STS_EN  0x2000
#define IBI_UPDATED_STS_EN       0x1000
#define READ_REQ_RECV_STS_EN     0x800
#define DEFSLV_STS_EN            0x400
#define TRANSFER_ERR_STS_EN      0x200
#define DYN_ADDR_ASSGN_STS_EN    0x100
#define CCC_UPDATED_STS_EN       0x40
#define TRANSFER_ABORT_STS_EN    0x20
#define RESP_READY_STS_EN        0x10
#define CMD_QUEUE_READY_STS_EN   0x8
#define IBI_THLD_STS_EN          0x4
#define RX_THLD_STS_EN           0x2
#define TX_THLD_STS_EN           0x1
#define INTR_STATUS_ALL_EN       (BUS_RESET_DONE_STS_EN | BUSOWNER_UPDATED_STS_EN | IBI_UPDATED_STS_EN | READ_REQ_RECV_STS_EN | DEFSLV_STS_EN | TRANSFER_ERR_STS_EN | DYN_ADDR_ASSGN_STS_EN | \
                                  CCC_UPDATED_STS_EN | TRANSFER_ABORT_STS_EN | RESP_READY_STS_EN | CMD_QUEUE_READY_STS_EN | IBI_THLD_STS_EN | RX_THLD_STS_EN | TX_THLD_STS_EN)

// INTR_SIGNAL_EN
#define BUS_RESET_DONE_SIGNAL_EN    0x8000
#define BUSOWNER_UPDATED_SIGNAL_EN  0x2000
#define IBI_UPDATED_SIGNAL_EN       0x1000
#define READ_REQ_RECV_SIGNAL_EN     0x800
#define DEFSLV_SIGNAL_EN            0x400
#define TRANSFER_ERR_SIGNAL_EN      0x200
#define DYN_ADDR_ASSGN_SIGNAL_EN    0x100
#define CCC_UPDATED_SIGNAL_EN       0x40
#define TRANSFER_ABORT_SIGNAL_EN    0x20
#define RESP_READY_SIGNAL_EN        0x10
#define CMD_QUEUE_READY_SIGNAL_EN   0x8
#define IBI_THLD_SIGNAL_EN          0x4
#define RX_THLD_SIGNAL_EN           0x2
#define TX_THLD_SIGNAL_EN           0x1
#define INTR_SIGNAL_ALL_EN          (BUS_RESET_DONE_SIGNAL_EN | BUSOWNER_UPDATED_SIGNAL_EN | IBI_UPDATED_SIGNAL_EN | READ_REQ_RECV_SIGNAL_EN | DEFSLV_SIGNAL_EN | TRANSFER_ERR_SIGNAL_EN | DYN_ADDR_ASSGN_SIGNAL_EN | \
                                     CCC_UPDATED_SIGNAL_EN | TRANSFER_ABORT_SIGNAL_EN | RESP_READY_SIGNAL_EN | CMD_QUEUE_READY_SIGNAL_EN | IBI_THLD_SIGNAL_EN | RX_THLD_SIGNAL_EN | TX_THLD_SIGNAL_EN)
// Common Command Code
#define SETAASA_CCC  0x29

// DEVICE_CTRL Reg
#define I3C_CONTROLLER_ENABLE   0x1
#define I3C_CONTROLLER_DISABLE  0x0

typedef union {
  struct {
    UINT32    IBA_INCLUDE        : 1; // bit0
    UINT32    Rsvd0              : 6; // bit6:1
    UINT32    I2C_SLAVE_PRESENT  : 1; // bit7
    UINT32    HOT_JOIN_CTRL      : 1; // bit8
    UINT32    Rsvd1              : 15; // bit23:9
    UINT32    IDLE_CNT_MULTPLIER : 2; // bit25:24
    UINT32    Rsvd2              : 1; // bit26
    UINT32    ADAPTIVE_I2C_I3C   : 1; // bit27
    UINT32    Rsvd3              : 1; // bit28
    UINT32    ABORT              : 1; // bit29
    UINT32    RESUME             : 1; // bit30
    UINT32    ENABLE             : 1; // bit31
  } Field;
  UINT32    Value;
} I3C_DEVICE_CTRL;

// QUEUE_SIZE_CAPABILITY Reg
#define TX_BUF_SIZE_MASK    0x0000000F
#define RX_BUF_SIZE_MASK    0x000000F0
#define TX_BUF_SIZE_OFFSET  0x0
#define RX_BUF_SIZE_OFFSET  0x4

typedef union {
  struct {
    UINT32    TX_BUF_SIZE   : 4;       // bit3:0
    UINT32    RX_BUF_SIZE   : 4;       // bit7:4
    UINT32    CMD_BUF_SIZE  : 4;       // bit11:8
    UINT32    RESP_BUF_SIZE : 4;       // bit15:12
    UINT32    IBI_BUF_SIZE  : 4;       // bit19:16
    UINT32    Rsvd          : 12;      // bit31:20
  } Field;
  UINT32    Value;
} I3C_QUEUE_SIZE_CAPABILITY;

// DEVICE_CTRL_EXTENDED Reg
#define MASTER_MODE              0x0
#define DEV_OPERATION_MODE_MASK  0xFFFFFFFC

typedef union {
  struct {
    UINT32    DEV_OPERATION_MODE : 2;  // bit1:0
    UINT32    Rsvd0              : 1;  // bit2
    UINT32    REQMST_ACK_CTRL    : 1;  // bit3
    UINT32    Rsvd1              : 28; // bit31:4
  } Field;
  UINT32    Value;
} I3C_DEVICE_CTRL_EXTENDED;

// QUEUE_THLD_CTRL Reg
typedef union {
  struct {
    UINT32    CMD_EMPTY_BUF_THLD : 8;  // bit7:0
    UINT32    RESP_BUF_THLD      : 8;  // bit15:8
    UINT32    IBI_DATA_THLD      : 8;  // bit23:16
    UINT32    IBI_STATUS_THLD    : 8;  // bit31:24
  } Field;
  UINT32    Value;
} I3C_QUEUE_THLD_CTRL;

// DATA_BUFFER_THLD_CTRL Reg
typedef union {
  struct {
    UINT32    TX_EMPTY_BUF_THLD : 3;   // bit2:0
    UINT32    Rsvd0             : 5;   // bit7:3
    UINT32    RX_BUF_THLD       : 3;   // bit10:8
    UINT32    Rsvd1             : 5;   // bit15:11
    UINT32    TX_START_THLD     : 3;   // bit18:16
    UINT32    Rsvd2             : 5;   // bit23:19
    UINT32    RX_START_THLD     : 3;   // bit26:24
    UINT32    Rsvd3             : 5;   // bit31:27
  } Field;
  UINT32    Value;
} I3C_DATA_BUFFER_THLD_CTRL;

// SCL_I3C_OD_TIMING Reg
#define  OD_LCNT  0x20
#define  OD_HCNT  0xE

typedef union {
  struct {
    UINT32    I3C_OD_LCNT : 8;         // bit7:0
    UINT32    Rsvd0       : 8;         // bit15:8
    UINT32    I3C_OD_HCNT : 8;         // bit23:16
    UINT32    Rsvd1       : 8;         // bit31:24
  } Field;
  UINT32    Value;
} I3C_SCL_I3C_OD_TIMING;

// SCL_I3C_PP_TIMING Reg
#define  PP_LCNT  0x7
#define  PP_HCNT  0x6

typedef union {
  struct {
    UINT32    I3C_PP_LCNT : 8;         // bit7:0
    UINT32    Rsvd0       : 8;         // bit15:8
    UINT32    I3C_PP_HCNT : 8;         // bit23:16
    UINT32    Rsvd1       : 8;         // bit31:24
  } Field;
  UINT32    Value;
} I3C_SCL_I3C_PP_TIMING;

// SCL_I2C_FM_TIMING Reg
#define  FM_LCNT  0x5
#define  FM_HCNT  0x5

typedef union {
  struct {
    UINT32    I2C_FM_LCNT : 16;         // bit15:0
    UINT32    I2C_FM_HCNT : 16;         // bit31:16
  } Field;
  UINT32    Value;
} I3C_SCL_I2C_FM_TIMING;

// SCL_I2C_FMP_TIMING Reg
#define  FMP_LCNT  0x3F
#define  FMP_HCNT  0x21

typedef union {
  struct {
    UINT32    I2C_FMP_LCNT : 16;        // bit15:0
    UINT32    I2C_FMP_HCNT : 8;         // bit23:16
    UINT32    Rsvd0        : 8;         // bit31:24
  } Field;
  UINT32    Value;
} I3C_SCL_I2C_FMP_TIMING;

// DEVICE_ADDR Reg
typedef union {
  struct {
    UINT32    STATIC_ADDR        : 7; // bit6:0
    UINT32    Rsvd0              : 8; // bit14:7
    UINT32    STATIC_ADDR_VALID  : 1; // bit15
    UINT32    DYNAMIC_ADDR       : 7; // bit22:16
    UINT32    Rsvd1              : 8; // bit30:23
    UINT32    DYNAMIC_ADDR_VALID : 1; // bit31
  } Field;
  UINT32    Value;
} I3C_DEVICE_ADDR;

// DEV_ADDR_TABLE1_LOC1
typedef union {
  struct {
    UINT32    STATIC_ADDRESS      : 7; // bit6:0
    UINT32    Rsvd0               : 4; // bit10:7
    UINT32    IBI_PEC_EN          : 1; // bit11
    UINT32    IBI_WITH_DATA       : 1; // bit12
    UINT32    SIR_REJECT          : 1; // bit13
    UINT32    MR_REJECT           : 1; // bit14
    UINT32    Rsvd1               : 1; // bit15
    UINT32    DEV_DYNAMIC_ADDR    : 7; // bit22:16
    UINT32    DYNAMIC_ADDR_PARITY : 1; // bit23
    UINT32    Rsvd2               : 5; // bit28:24
    UINT32    DEV_NACK_RETRY_CNT  : 2; // bit30:29
    UINT32    DEVICE              : 1; // bit31
  } Field;
  UINT32    Value;
} I3C_DEV_ADDR_TABLE1_LOC1;

// COMMAND_QUEUE_PORT
typedef enum {
  TRANSFER_CMD    = 0,
  TRANSFER_ARGU   = 1,
  SHORT_DATA_ARGU = 2,
  ADDR_ASSIGN_CMD = 3
} I3C_CMD_ATTR;

typedef enum {
  TRANS_SDR0    = 0,
  TRANS_SDR1    = 1,
  TRANS_SDR2    = 2,
  TRANS_SDR3    = 3,
  TRANS_SDR4    = 4,
  TRANS_HDR_TS  = 5,
  TRANS_HDR_DDR = 6,
  TRANS_I2C_FM  = 7
} I3C_TRANS_SPEED;

typedef enum {
  I2C_FM      = 0,
  I2C_FM_PLUS = 1
} I2C_TRANS_SPEED;

typedef enum {
  I3C_CMD_WRITE = 0,
  I3C_CMD_READ  = 1
} I3C_CMD_TYPE;

typedef union {
  struct {
    UINT32    CMD_ATTR : 3; // bit2:0 i3c_cmd_attr_t i3c_cmd_attr_t
    UINT32    TID      : 4; // bit6:3 transaction ID
    UINT32    CMD      : 8; // bit14:7 transfer command
    UINT32    CP       : 1; // bit15 command preset
    UINT32    DEV_INDX : 5; // bit20:16 device index
    UINT32    SPEED    : 3; // bit23:21
    UINT32    RESVD0   : 1; // bit24
    UINT32    DBP      : 1; // bit25 defining byte present
    UINT32    ROC      : 1; // bit26 respose on completion
    UINT32    SDAP     : 1; // bit27 short data argument present
    UINT32    RNW      : 1; // bit28 read and write
    UINT32    RESVD1   : 1; // bit29
    UINT32    TOC      : 1; // bit30 termination on completion
    UINT32    PEC      : 1; // bit31 parity error check enable
  } Trans_cmd;

  struct {
    UINT32    CMD_ATTR : 3; // bit2:0 i3c_cmd_attr_t
    UINT32    RESVD0   : 5; // bit7:3
    UINT32    DB       : 8; // bit15:7 defining byte value
    UINT32    DATA_LEN : 16; // bit31:16 data length
  } Trans_argu;

  struct {
    UINT32    CMD_ATTR      : 3; // bit2:0 i3c_cmd_attr_t
    UINT32    BYTE_STRB     : 3; // bit5:3 byte strobe
    UINT32    RESVD0        : 2; // bit7:6
    UINT32    DATA_BYTE0_DB : 8; // bit15:8 data byte0/define byte
    UINT32    DATA_BYTE1    : 8; // bit23:16 data byte1
    UINT32    DATA_BYTE2    : 8; // bit31:24 data byte2
  } Short_data_argu;

  struct {
    UINT32    CMD_ATTR  : 3; // bit2:0 i3c_cmd_attr_t
    UINT32    TID       : 4; // bit6:3 transaction ID
    UINT32    CMD       : 8; // bit14:7
    UINT32    RESVD0    : 1; // bit15
    UINT32    DEV_INDX  : 5; // bit20:16
    UINT32    DEV_COUNT : 5; // bit25:21
    UINT32    ROC       : 1; // bit26
    UINT32    RESVD1    : 3; // bit29:27
    UINT32    TOC       : 1; // bit30
    UINT32    RESVD2    : 1; // bit31
  } Addr_assign_cmd;
  UINT32    Value;
} I3C_COMMAND_DATA;

// Response Data Structure
typedef union {
  struct {
    UINT32    DL      : 16; // bit15:0, data length
    UINT32    CCCT    : 8;  // bit23:16, CCC/HDR Header Type
    UINT32    TID     : 4;  // bit27:24
    UINT32    ERR_STS : 4;  // bit31:28
  } Rsp;
  UINT32    Value;
} I3C_RESPONSE_DATA;

typedef enum {
  NO_ERR               = 0,
  CRC_ERR              = 1,
  PARITY_ERR           = 2,
  FRAME_ERR            = 3,
  BROADCAST_ADDR_NACK  = 4,
  ADDR_NACK            = 5,
  RECV_BUFF_OVERFLOW   = 6,
  TRANSFER_ABORT       = 8,
  I2C_SLAVE_WRITE_NACK = 9,
  PEC_ERR              = 12,
} I3C_ERROR_STATUS;

typedef union {
  struct {
    UINT32    SOFT_RST       : 1;      // bit0
    UINT32    CMD_QUEUE_RST  : 1;      // bit1
    UINT32    RESP_QUEUE_RST : 1;      // bit2
    UINT32    TX_FIFO_RST    : 1;      // bit3
    UINT32    RX_FIFO_RST    : 1;      // bit4
    UINT32    IBI_QUEUE_RST  : 1;      // bit5
    UINT32    Rsvd0          : 23;     // bit28:6
    UINT32    BUS_RESET_TYPE : 2;      // bit30:29
    UINT32    BUS_RESET      : 1;      // bit31
  } Field;
  UINT32    Value;
} I3C_RESET_CTRL;

// Present State Reg
typedef union {
  struct {
    UINT32    SCL_LINE_SIGNAL_LEVEL : 1;           // bit0
    UINT32    SDA_LINE_SIGNAL_LEVEL : 1;           // bit1
    UINT32    CURRENT_MASTER        : 1;           // bit2
    UINT32    Rsvd0                 : 5;           // bit7:3
    UINT32    CM_TFR_STS            : 6;           // bit13:8
    UINT32    Rsvd1                 : 2;           // bit15:14
    UINT32    CM_TFR_ST_STS         : 6;           // bit21:16
    UINT32    Rsvd2                 : 2;           // bit23:22
    UINT32    CMD_TID               : 4;           // bit27:24
    UINT32    MASTER_IDLE           : 1;           // bit28
    UINT32    Rsvd3                 : 3;           // bit31:29
  } Field;
  UINT32    Value;
} I3C_PRESENT_STATE;

#endif
