/** @file

  Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SECURITY_MANAGEMENT_PROTOCOL_H__
#define __SECURITY_MANAGEMENT_PROTOCOL_H__

#include <Library/SecurityManagementLib.h>

//
// Security Management Protocol GUID value
//
#define SECURITY_MANAGEMENT_PROTOCOL_GUID \
    { \
      0xe9ae1a14, 0x3890, 0x4609, { 0xb9, 0x1e, 0x1a, 0xb8, 0x42, 0xb6, 0x2e, 0x40 } \
    }

//
// Protocol interface structure
//
typedef struct _SECURITY_MANAGEMENT_PROTOCOL SECURITY_MANAGEMENT_PROTOCOL;

//
// Revision The revision to which the protocol interface adheres.
//          All future revisions must be backwards compatible.
//          If a future version is not back wards compatible it is not the same GUID.
//
#define SECURITY_MANAGEMENT_PROTOCOL_REVISION  0x00000001

//
// Function Prototypes
//

/**
  Register security measurement handler with its operation type. Different
  handlers with the same operation can all be registered.

  If SecurityHandler is NULL, then ASSERT().
  If no enough resources available to register new handler, then ASSERT().
  If AuthenticationOperation is not recongnized, then ASSERT().
  If the previous register handler can't be executed before the later register handler, then ASSERT().

  @param[in]  SecurityHandler           The security measurement service handler to be registered.
  @param[in]  AuthenticationOperation   Theoperation type is specified for the registered handler.

  @retval EFI_SUCCESS              The handlers were registered successfully.
**/
typedef
EFI_STATUS
(EFIAPI *REGISTER_SECURITY_HANDLER)(
  IN  SECURITY_FILE_AUTHENTICATION_STATE_HANDLER  SecurityHandler,
  IN  UINT32                                      AuthenticationOperation
  );

/**
  Execute registered handlers until one returns an error and that error is returned.
  If none of the handlers return an error, then EFI_SUCCESS is returned.

  Before exectue handler, get the image buffer by file device path if a handler
  requires the image file. And return the image buffer to each handler when exectue handler.

  The handlers are executed in same order to their registered order.

  @param[in]  AuthenticationStatus
                           This is the authentication type returned from the Section
                           Extraction protocol. See the Section Extraction Protocol
                           Specification for details on this type.
  @param[in]  FilePath     This is a pointer to the device path of the file that is
                           being dispatched. This will optionally be used for logging.

  @retval EFI_SUCCESS            The file specified by File authenticated when more
                                 than one security handler services were registered,
                                 or the file did not authenticate when no security
                                 handler service was registered. And the platform policy
                                 dictates that the DXE Core may use File.
  @retval EFI_INVALID_PARAMETER  File is NULL.
  @retval EFI_SECURITY_VIOLATION The file specified by File did not authenticate, and
                                 the platform policy dictates that File should be placed
                                 in the untrusted state. A file may be promoted from
                                 the untrusted to the trusted state at a future time
                                 with a call to the Trust() DXE Service.
  @retval EFI_ACCESS_DENIED      The file specified by File did not authenticate, and
                                 the platform policy dictates that File should not be
                                 used for any purpose.
**/
typedef
EFI_STATUS
(EFIAPI *EXECUTE_SECURITY_HANDLERS)(
  IN  UINT32                            AuthenticationStatus,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL    *FilePath
  );

/**
  Register security measurement handler with its operation type. Different
  handlers with the same operation can all be registered.

  If SecurityHandler is NULL, then ASSERT().
  If no enough resources available to register new handler, then ASSERT().
  If AuthenticationOperation is not recongnized, then ASSERT().
  If AuthenticationOperation is EFI_AUTH_OPERATION_NONE, then ASSERT().
  If the previous register handler can't be executed before the later register handler, then ASSERT().

  @param[in]  Security2Handler          The security measurement service handler to be registered.
  @param[in]  AuthenticationOperation   The operation type is specified for the registered handler.

  @retval EFI_SUCCESS              The handlers were registered successfully.
**/
typedef
EFI_STATUS
(EFIAPI *REGISTER_SECURITY2_HANDLER)(
  IN  SECURITY2_FILE_AUTHENTICATION_HANDLER       Security2Handler,
  IN  UINT32                                      AuthenticationOperation
  );

/**
  Execute registered handlers based on input AuthenticationOperation until
  one returns an error and that error is returned.

  If none of the handlers return an error, then EFI_SUCCESS is returned.
  The handlers those satisfy AuthenticationOperation will only be executed.
  The handlers are executed in same order to their registered order.

  @param[in]  AuthenticationOperation
                           The operation type specifies which handlers will be executed.
  @param[in]  AuthenticationStatus
                           The authentication status for the input file.
  @param[in]  File         This is a pointer to the device path of the file that is
                           being dispatched. This will optionally be used for logging.
  @param[in]  FileBuffer   A pointer to the buffer with the UEFI file image
  @param[in]  FileSize     The size of File buffer.
  @param[in]  BootPolicy   A boot policy that was used to call LoadImage() UEFI service.

  @retval EFI_SUCCESS             The file specified by DevicePath and non-NULL
                                  FileBuffer did authenticate, and the platform policy dictates
                                  that the DXE Foundation may use the file.
  @retval EFI_SUCCESS             The device path specified by NULL device path DevicePath
                                  and non-NULL FileBuffer did authenticate, and the platform
                                  policy dictates that the DXE Foundation may execute the image in
                                  FileBuffer.
  @retval EFI_SUCCESS             FileBuffer is NULL and current user has permission to start
                                  UEFI device drivers on the device path specified by DevicePath.
  @retval EFI_SECURITY_VIOLATION  The file specified by DevicePath and FileBuffer did not
                                  authenticate, and the platform policy dictates that the file should be
                                  placed in the untrusted state. The image has been added to the file
                                  execution table.
  @retval EFI_ACCESS_DENIED       The file specified by File and FileBuffer did not
                                  authenticate, and the platform policy dictates that the DXE
                                  Foundation may not use File.
  @retval EFI_SECURITY_VIOLATION  FileBuffer is NULL and the user has no
                                  permission to start UEFI device drivers on the device path specified
                                  by DevicePath.
  @retval EFI_SECURITY_VIOLATION  FileBuffer is not NULL and the user has no permission to load
                                  drivers from the device path specified by DevicePath. The
                                  image has been added into the list of the deferred images.
  @retval EFI_INVALID_PARAMETER   File and FileBuffer are both NULL.
**/
typedef
EFI_STATUS
(EFIAPI *EXECUTE_SECURITY2_HANDLERS)(
  IN  UINT32                           AuthenticationOperation,
  IN  UINT32                           AuthenticationStatus,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *File, OPTIONAL
  IN  VOID                             *FileBuffer,
  IN  UINTN                            FileSize,
  IN  BOOLEAN                          BootPolicy
  );

struct _SECURITY_MANAGEMENT_PROTOCOL {
  UINT64                        Revision;
  REGISTER_SECURITY_HANDLER     RegisterSecurityHandler;
  EXECUTE_SECURITY_HANDLERS     ExecuteSecurityHandlers;
  REGISTER_SECURITY2_HANDLER    RegisterSecurity2Handler;
  EXECUTE_SECURITY2_HANDLERS    ExecuteSecurity2Handlers;
};

extern EFI_GUID  gSecurityManagementProtocolGuid;

#endif /* __SECURITY_MANAGEMENT_PROTOCOL_H__ */
