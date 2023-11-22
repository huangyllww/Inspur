#include <Protocol/HygonPspTpcmProtocol.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/DevicePathLib.h>
#include <Library/SecurityManagementLib.h>
#include <Library/HygonPspTpcmLib.h>
#include <Library/PcdLib.h>

static BOOLEAN  gIsTpcmEnabled = FALSE;

EFI_STATUS
TpcmVerifyRaw (
  IN  PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL  *This,
  IN  UINT32    MeasureStage,
  IN  VOID     *ImageInfo,
  IN  UINT32    ImageInfoSize,
  IN  UINT32    NumAddrRange,
  IN  IMAGE_ADDR_RANGE Ranges[],
  OUT UINT32   *MeasureResult,
  OUT UINT32   *ControlResult
  );

BOOLEAN
IsVerifyEnabled (
  IN  PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL  *This
  );

PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL  C2PImageVerify = {
  &TpcmVerifyRaw,
  &IsVerifyEnabled,
};

EFI_STATUS
TpcmVerifyRaw (
  IN  PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL  *This,
  IN  UINT32    MeasureStage,
  IN  VOID     *ImageInfo,
  IN  UINT32    ImageInfoSize,
  IN  UINT32    NumAddrRange,
  IN  IMAGE_ADDR_RANGE Ranges[],
  OUT UINT32   *MeasureResult,
  OUT UINT32   *ControlResult
  )
{
  return PspMboxImageVerify (
           MeasureStage,
           ImageInfo,
           ImageInfoSize,
           NumAddrRange,
           Ranges,
           MeasureResult,
           ControlResult
           );
}

BOOLEAN
IsVerifyEnabled (
  IN  PSP_C2P_MBOX_IMAGE_VERIFY_PROTOCOL  *This
  )
{
  return gIsTpcmEnabled;
}

EFI_STATUS
EFIAPI
HygonPspTpcmInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;

  IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspTpcmInitialize Enter\n");

  /// **************************************************
  /// * Customer porting start
  /// **************************************************

  gIsTpcmEnabled = PcdGetBool (PcdHygonPspTpcmEnable);

  /// **************************************************
  /// * Customer porting end
  /// **************************************************

  if (gIsTpcmEnabled == FALSE) {
    return EFI_SUCCESS;
  }

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gPspC2PmboxImageVerifyProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &C2PImageVerify
                  );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("InstallProtocolInterface fail\n");
    return Status;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspTpcmInitialize Exit\n");

  return EFI_SUCCESS;
}
