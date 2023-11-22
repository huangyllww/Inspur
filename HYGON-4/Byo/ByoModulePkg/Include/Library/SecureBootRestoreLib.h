/** @file

  Copyright (c) 2020, Byosoft Corporation.<BR>
  All rights reserved.This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

  File Name:
    SecureBootRestoreLib.h

  Abstract:
    SecureBoot Policy Restoration Library Header File

  Revision History:

  TIME:
  $AUTHOR:
  $REVIEWERS:
  $SCOPE:
  $TECHNICAL:


  T.O.D.O

  $END

**/

#ifndef __SECUREBOOT_RESTORELIB_H__
#define __SECUREBOOT_RESTORELIB_H__

#define EFI_CERT_REVOCATION_ERROR   0x88888888
#define EFI_SECURE_BOOT_DEFAULT_KEY      L"SecureBootDefaultKey"

EFI_STATUS
RemoveAllCertKey (
  VOID
  );

EFI_STATUS
AuthVarMfgReset (
  VOID
  );

#endif
