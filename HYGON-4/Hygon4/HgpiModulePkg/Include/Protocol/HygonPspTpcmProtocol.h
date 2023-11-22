/* $NoKeywords:$ */

/**
 * @file
 * Protocol of image verification by PSP via mailbox
 *
 */

#ifndef _HYGON_PSP_TPCM_PROTOCOL_H_
#define _HYGON_PSP_TPCM_PROTOCOL_H_
#include <Library/HygonPspTpcmLib.h>

typedef struct _PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL;

/**
  Send the image to PSP for measurement. The Image can be loaded in physically-discontinuous
  memory, and thus mutiple address ranges should be specified.

  @param[in]    This           The PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL instance
  @param[in]    MeasureStage   The current measurement stage
  @param[in]    ImageInfo      Image description info in string
  @param[in]    NumAddrRange   Number of address ranges provided in the range array
  @param[in]    Ranges         Array of address ranges
  @param[out]   MeasureResult  Result of the measurement itself, 0 on success, otherwise error code.
  @param[out]   ControlResult  The execution control result based on the measurement result and
                               possible internal measurement policy in PSP. 0 - execution allowed,
                               1 - not allowed.

  @retval EFI_SUCCESS  The function return without any program error.
  @retval Others       Error code indicate program error, and normally the measurement result and
                       control result should be deemed as invalid.
**/
typedef EFI_STATUS
(EFIAPI *C2P_MBOX_IMAGE_VERIFY_RAW)(
  IN  PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL  *This,
  IN  UINT32    MeasureStage,
  IN  VOID     *ImageInfo,
  IN  UINT32    ImageInfoSize,
  IN  UINT32    NumAddrRange,
  IN  IMAGE_ADDR_RANGE Ranges[],
  OUT UINT32   *MeasureResult,
  OUT UINT32   *ControlResult
  );

/**
  Tell if the verify function is enabled. The function can be disabled in the UEFI Setup.
  When disabled, the C2P_MBOX_IMAGE_VERIFY_RAW api will already return with EFI_SUCCESS, and control
  result being execution allowed, and measurement result indicating measurement not performed indeed.

  @param[in]    This           The PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL instance

  @retval   0  Image verify function disabled
            1  Image verify function enabled
**/
typedef BOOLEAN
(EFIAPI *C2P_MBOX_IMAGE_IS_VERIFY_ENABLED)(
  IN  PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL  *This
  );

// Structure for PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL protocol
struct  _PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL {
  C2P_MBOX_IMAGE_VERIFY_RAW           TpcmVerifyRaw; // give raw content of the image to verify.
  // Future expansion may use other ways such as by hash.
  C2P_MBOX_IMAGE_IS_VERIFY_ENABLED    IsTpcmEnabled;
};

extern EFI_GUID  gPspC2PmboxImageVerifyProtocolGuid;
#endif //_HYGON_PSP_TPCM_PROTOCOL_H_
