#ifndef _HYGON_PSP_TPCM_H_
#define _HYGON_PSP_TPCM_H_

typedef enum {
  MboxBiosCh2CmdImageVerify = 0x90,
} MBOX_CH2_COMMAND;

#pragma pack(push, 1)

// ADDR Range structure used in Image verification protocol API.
typedef struct {
  UINT64    Start;          ///< Physical start address of image loaded in memory.
  UINT64    Length;         ///< Image length in memory.
} IMAGE_ADDR_RANGE;

// ADDR Range structure used in the command buffer.
typedef struct {
  UINT64    StartAddr;          ///< Physical start address of image loaded in memory.
  UINT32    Length;             ///< Image length in memory.
} ADDR_RANGE;

typedef struct {
  UINT32    Length;             ///< Name string length.
  UINT64    StartAddr;          ///< Physical start address of name string in memory.
} RANGE_NAME;

typedef struct {
  UINT32        Tag;
  UINT32        MsgLen;
  UINT32        Code;
  UINT32        Stage;          ///< Measurement stage
  UINT32        NumAddrRange;   ///< Number of address ranges to measure, the target may be
                                /// loaded in physically discontinuous memory.
  ADDR_RANGE    Ranges[0];
} MBOX_IMAGE_VERIFY_CMD;

typedef struct {
  UINT32    Tag;
  UINT32    MsgLen;
  UINT16    CtrlCode;
  UINT16    RetCode;
} MBOX_IMAGE_VERIFY_RESPONSE;

typedef struct {
  UINT64    cmd_sequence; // increment, can be ignored for async command.
  UINT64    input_addr;
  UINT32    input_length;
  UINT64    output_addr;
  UINT32    output_maxlength; // For input, it's the length of output buffer;
                              // for output, it's the length of valid data in output buffer.
} MBOX_IMAGE_VERIFY_BUF;
#pragma pack(pop)

EFI_STATUS
PspMboxImageVerify (
  IN UINT32    MeasureStage,
  IN VOID     *ImageInfo,
  IN UINT32    ImageInfoSize,
  IN UINT32    NumAddrRange,
  IN IMAGE_ADDR_RANGE Ranges[],
  OUT UINT32  *MeasureResult,
  OUT UINT32  *ControlResult
  );

#endif
