/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef BYO_PLATFORM_BOOT_MANAGER_PROTOCOL_H__
#define BYO_PLATFORM_BOOT_MANAGER_PROTOCOL_H__

#include <Library/UefiBootManagerLib.h>

//
// Platform Boot Manager Protocol GUID value
//
#define BYO_PLATFORM_BOOT_MANAGER_PROTOCOL_GUID \
    { \
      0x524c119, 0x6fe7, 0x4b24, { 0x8d, 0x47, 0xbe, 0x37, 0xdc, 0xa7, 0x2a, 0x87} \
    }

//
// Revision The revision to which the protocol interface adheres.
//          All future revisions must be backwards compatible.
//          If a future version is not back wards compatible it is not the same GUID.
//
#define BYO_PLATFORM_BOOT_MANAGER_PROTOCOL_REVISION 0x00000005

// Boot Policy Status Value
#define BOOT_POLICY_IS_BOOT_MANAGER_MENU      0x00000001
#define BOOT_POLICY_IS_SETUP                  0x00000002
#define BOOT_POLICY_NEED_READY_TO_BOOT        0x00000004
#define BOOT_POLICY_NEED_POST_LOGIN           0x00000008
#define BOOT_POLICY_PXE_RETRY                 0x00000010
#define BOOT_POLICY_PXE_RETRY_TIMES_ENABLE    0x00000020
#define BOOT_POLICY_PXE_RETRY_TIMES           0x00FF0000
#define BOOT_POLICY_DEFAULT                   0x80000000
// Limit PXE retry times
#define GET_PXE_RETRY_TIMES(x)  (UINT8)((UINT32)((x) & BOOT_POLICY_PXE_RETRY_TIMES) >> 16)

//
// Function Prototypes
//

/*
  This function allows platform to refresh all boot options specific to the platform. Within
  this function, platform can make modifications to the auto enumerated platform boot options
  as well as NV boot options.

  @param[in const] BootOptions             An array of auto enumerated platform boot options.
                                           This array will be freed by caller upon successful
                                           exit of this function and output array would be used.

  @param[in const] BootOptionsCount        The number of elements in BootOptions.

  @param[out]      UpdatedBootOptions      An array of boot options that have been customized
                                           for the platform on top of input boot options. This
                                           array would be allocated by REFRESH_ALL_BOOT_OPTIONS
                                           and would be freed by caller after consuming it.

  @param[out]      UpdatedBootOptionsCount The number of elements in UpdatedBootOptions.


  @retval EFI_SUCCESS                      Platform refresh to input BootOptions and
                                           BootCount have been done.

  @retval EFI_OUT_OF_RESOURCES             Memory allocation failed.

  @retval EFI_INVALID_PARAMETER            Input is not correct.

  @retval EFI_UNSUPPORTED                  Platform specific overrides are not supported.
*/
typedef
EFI_STATUS
(EFIAPI *PLATFORM_BOOT_MANAGER_REFRESH_ALL_BOOT_OPTIONS) (
  IN  CONST EFI_BOOT_MANAGER_LOAD_OPTION *BootOptions,
  IN  CONST UINTN                        BootOptionsCount,
  OUT       EFI_BOOT_MANAGER_LOAD_OPTION **UpdatedBootOptions,
  OUT       UINTN                        *UpdatedBootOptionsCount
  );

//
// Function Prototypes
//

/*
  This function allows platform to add specific code at end of refresh boot option

  @retval                       Platform defined return code
*/
typedef
EFI_STATUS
(EFIAPI *END_OF_REFRESH_BOOT_OPTION) (
  VOID
  );

/*
  This function allows platform to execute the policy when booting.
*/
typedef 
EFI_STATUS
(EFIAPI *BOOT_POLICY) (
  IN CONST EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption,
  IN OUT UINT32                   *BootPolicyStatus
  );  

/*
  This function allows platform to execute the policy when booting failed.
*/
typedef 
BOOLEAN
(EFIAPI *BOOT_FAILED_POLICY) (
  IN  CONST EFI_BOOT_MANAGER_LOAD_OPTION *BootOption,
  IN EFI_STATUS                           Status
  );  

typedef struct {
  UINT64                                         Revision;
  PLATFORM_BOOT_MANAGER_REFRESH_ALL_BOOT_OPTIONS RefreshAllBootOptions;
  SORT_COMPARE                                   CompareBootOption;
  END_OF_REFRESH_BOOT_OPTION                     EndOfRefreshBootOption;
  BOOT_FAILED_POLICY                             BootFailedPolicy;
  BOOT_POLICY                                    BootPolicy;
} BYO_PLATFORM_BOOT_MANAGER_PROTOCOL;

extern EFI_GUID gByoPlatformBootManagerProtocolGuid;

#endif
