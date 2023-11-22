/** @file
  TPM Platform Hierarchy configuration library.
  This library provides functions for customizing the TPM's Platform Hierarchy
  Authorization Value (platformAuth) and Platform Hierarchy Authorization
  Policy (platformPolicy) can be defined through this function.

  @copyright
  BYOSOFT CONFIDENTIAL
  Copyright 2019 - 2022 Byosoft Corporation. <BR>

  This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.


    @par Specification Reference:
    https://trustedcomputinggroup.org/resource/tcg-tpm-v2-0-provisioning-guidance/
**/

#include <PiDxe.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/RngLib.h>
#include <Library/BaseCryptLib.h>
#include <Library/UefiLib.h>
#include <Protocol/DxeSmmReadyToLock.h>
#include <Protocol/Tcg2Protocol.h>


//
// The authorization value may be no larger than the digest produced by the hash
//   algorithm used for context integrity.
//
#define      MAX_NEW_AUTHORIZATION_SIZE SHA512_DIGEST_SIZE

UINT16       mAuthSize;
TPM2B_AUTH   mNewPlatformAuth;
TPM2B_AUTH   mNewLockoutAuth;
TPM2B_AUTH   mNewOwnerAuth;

//
// Requested feature:
//   1. Platform BIOS shall set PH's platformPolicy using TPM 2.0 command
//      TPM2_SetPrimaryPolicy ()
//
//   2. It shall use HashAlg == SHA256
//
//   3. authPolicy digest to be set must be the digest of the following
//      negotiated policy command: TPM2_PolicyOR (0, SHA256 ("IntelTXT"))
//      This digest is selected to match value used by server platform BIOS
//      which creates consistency of support between TXT tools and provisioning
//      scripts.
//
//   4. Important note - the fixed selected passphrase "IntelTXT" is used to
//      enable Intel reference BIOS and internal and external testing. It is
//      assumed that each of the OEMs will select own passphrase and / or own
//      set of TPM2.0 EA policy commands.
//
TPM2B_DIGEST mNewPlatformPolicy = {
  0x20,
  {
    0x22, 0x03, 0x0B, 0x7E, 0x0B, 0xB1, 0xf9, 0xD5, 0x06, 0x57, 0x57, 0x1E, 0xE2, 0xF7, 0xFC, 0xE1,
    0xEB, 0x91, 0x99, 0x0C, 0x8B, 0x8A, 0xE9, 0x77, 0xFC, 0xB3, 0xF1, 0x58, 0xB0, 0x3E, 0xBA, 0x96
  }
};

TPM2B_DIGEST mNewPlatformPolicySHA384 = {
  0x30,
  {
    0x6A, 0x4D, 0x09, 0xD2, 0x71, 0xD1, 0x51, 0x5F, 0x1A, 0xDA, 0x9C, 0x49, 0x9F, 0xA7, 0x45, 0xA4,
    0x13, 0xC0, 0x5F, 0x2C, 0x6F, 0x15, 0x48, 0xBA, 0x60, 0xF2, 0xFB, 0x2A, 0x60, 0x93, 0x0A, 0xDF,
    0x0B, 0xCD, 0x49, 0x93, 0x6C, 0x04, 0xA8, 0x51, 0x40, 0xDF, 0xDB, 0xCB, 0x2C, 0xC9, 0x1F, 0x7B
  }
};

/**
  This function returns the maximum size of TPM2B_AUTH; this structure is used for an authorization value
  and limits an authValue to being no larger than the largest digest produced by a TPM.

  @param[out] AuthSize                 Tpm2 Auth size

  @retval EFI_SUCCESS                  Auth size returned.
  @retval other error value

**/
EFI_STATUS
EFIAPI
GetAuthSize (
  OUT UINT16            *AuthSize
  )
{
  EFI_STATUS            Status;
  TPML_PCR_SELECTION    Pcrs;
  UINTN                 Index;
  UINT16                DigestSize;

  DEBUG ((DEBUG_INFO, "[TPM2] %a: Entry\n", __FUNCTION__));

  Status = EFI_SUCCESS;

  while (mAuthSize == 0) {

    mAuthSize = SHA1_DIGEST_SIZE;
    ZeroMem (&Pcrs, sizeof (TPML_PCR_SELECTION));
    Status = Tpm2GetCapabilityPcrs (&Pcrs);
    DEBUG ((DEBUG_INFO, "\tTpm2GetCapabilityPcrs - %r\n", Status));
    if (EFI_ERROR (Status)) {
      return Status;
    }

    for (Index = 0; Index < Pcrs.count; Index++) {
      switch (Pcrs.pcrSelections[Index].hash) {
      case TPM_ALG_SHA1:
        DigestSize = SHA1_DIGEST_SIZE;
        break;
      case TPM_ALG_SHA256:
        DigestSize = SHA256_DIGEST_SIZE;
        break;
      case TPM_ALG_SHA384:
        DigestSize = SHA384_DIGEST_SIZE;
        break;
      case TPM_ALG_SHA512:
        DigestSize = SHA512_DIGEST_SIZE;
        break;
      case TPM_ALG_SM3_256:
        DigestSize = SM3_256_DIGEST_SIZE;
        break;
      default:
        DigestSize = SHA1_DIGEST_SIZE;
        break;
      }

      if (DigestSize > mAuthSize) {
        mAuthSize = DigestSize;
      }
    }
    break;
  }

  *AuthSize = mAuthSize;

  DEBUG ((DEBUG_INFO, "[TPM2] %a: Exit\n", __FUNCTION__));

  return Status;
}

/**
  Set new platform auth.

  @param[in]  AuthHandle               TPM_RH_LOCKOUT, TPM_RH_ENDORSEMENT, TPM_RH_OWNER or TPM_RH_PLATFORM+{PP}

**/
VOID
SetNewAuthInfo (
  IN TPMI_RH_HIERARCHY_AUTH     AuthHandle
  )
{
  UINT16                        AuthSize;
  EFI_STATUS                    Status;

  DEBUG ((DEBUG_INFO, "[TPM2] %a: Entry\n", __FUNCTION__));

  Status = GetAuthSize (&AuthSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "\tGetAuthSize failed - %r\n", Status));
    return;
  }

  switch (AuthHandle) {
  case TPM_RH_PLATFORM:
    mNewPlatformAuth.size = AuthSize;
    RandomSeed (NULL, 0);
    RandomBytes (mNewPlatformAuth.buffer, mNewPlatformAuth.size);
    break;
  case TPM_RH_LOCKOUT:
    mNewLockoutAuth.size = AuthSize;
    RandomSeed (NULL, 0);
    RandomBytes (mNewLockoutAuth.buffer, mNewLockoutAuth.size);
    break;
  case TPM_RH_OWNER:
    mNewOwnerAuth.size = AuthSize;
    RandomSeed (NULL, 0);
    RandomBytes (mNewOwnerAuth.buffer, mNewOwnerAuth.size);
    break;
  default:
    ASSERT (FALSE);
    break;
  }

  DEBUG ((DEBUG_INFO, "[TPM2] %a: Exit\n", __FUNCTION__));

  return;
}

/**
  Send platform hierarchy change auth, with re-provision Endorsement/Platform/Storage Hierarchy, based on request.
**/
VOID
HierarchyChangeAuthPlatform (
  VOID
  )
{
  EFI_STATUS                Status;
  TPMS_AUTH_COMMAND         LocalAuthSession;

  DEBUG ((DEBUG_INFO, "[TPM2] %a: Entry\n", __FUNCTION__));

  if (PcdGetBool (PcdTpm2HierarchyChangeAuthPlatform)) {
    SetNewAuthInfo (TPM_RH_PLATFORM);

    ZeroMem (&LocalAuthSession, sizeof (LocalAuthSession));
    LocalAuthSession.sessionHandle = TPM_RS_PW;
    LocalAuthSession.hmac.size = mNewPlatformAuth.size;
    CopyMem (LocalAuthSession.hmac.buffer, mNewPlatformAuth.buffer, mNewPlatformAuth.size);

    //
    // Program Primary Policy, need localAuthSession to be NULL, response 9A2 if set
    //
    if (PcdGetBool (PcdTpm2TxtProvisionSupport)) {
      if (PcdGetBool (PcdTpm2TxtProvisionSHA384)) {
        Status = Tpm2SetPrimaryPolicy (TPM_RH_PLATFORM, NULL, &mNewPlatformPolicySHA384, TPM_ALG_SHA384);
      } else {
        Status = Tpm2SetPrimaryPolicy (TPM_RH_PLATFORM, NULL, &mNewPlatformPolicy, TPM_ALG_SHA256);
      }
      DEBUG ((DEBUG_INFO, "\tTpm2SetPrimaryPolicy - %r\n", Status));
      if (EFI_ERROR (Status)) {
        goto Done;
      }
    }

    Status = Tpm2HierarchyChangeAuth (TPM_RH_PLATFORM, NULL, &mNewPlatformAuth);
    DEBUG ((DEBUG_INFO, "\tTpm2HierarchyChangeAuth - %r\n", Status));
    if (EFI_ERROR (Status)) {
      ZeroMem (&mNewPlatformAuth, sizeof (mNewPlatformAuth));
      goto Done;
    }

    //
    // Re-provision Endorsement Hierarchy
    //
    if (PcdGetBool (PcdTpm2ChangeEps)) {
      Status = Tpm2ChangeEPS (TPM_RH_PLATFORM, &LocalAuthSession);
      DEBUG ((DEBUG_INFO, "\tTpm2ChangeEPS - %r\n", Status));
      if (!EFI_ERROR (Status)) {
        PcdSetBoolS (PcdTpm2ChangeEps, FALSE);
      }
    }

    //
    // Re-provision Platform Hierarchy
    //
    if (PcdGetBool (PcdTpm2ChangePps)) {
      Status = Tpm2ChangePPS (TPM_RH_PLATFORM, &LocalAuthSession);
      DEBUG ((DEBUG_INFO, "\tTpm2ChangePPS - %r\n", Status));
      if (!EFI_ERROR (Status)) {
        PcdSetBoolS (PcdTpm2ChangePps, FALSE);
      }
    }

    //
    // Re-provision Storage Hierarchy
    //
    if (PcdGetBool (PcdTpm2Clear)) {
      Status = Tpm2ClearControl (TPM_RH_PLATFORM, &LocalAuthSession, NO);
      DEBUG ((DEBUG_INFO, "\tTpm2ClearControl - %r\n", Status));
      if (!EFI_ERROR (Status)) {
        Status = Tpm2Clear (TPM_RH_PLATFORM, &LocalAuthSession);
        DEBUG ((DEBUG_INFO, "\tTpm2Clear - %r\n", Status));
        if (!EFI_ERROR (Status)) {
          PcdSetBoolS (PcdTpm2Clear, FALSE);
        }
      }
    }

    //
    // Enable or disable Endorsement Hierarchy
    //
    if (PcdGetBool (PcdTpm2HierarchyControlEndorsement)) {
      Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, &LocalAuthSession, TPM_RH_ENDORSEMENT, YES);
      DEBUG ((DEBUG_INFO, "\tTpm2HierarchyControl (TPM_RH_ENDORSEMENT: 0x%08x, %02x) - %r\n", TPM_RH_ENDORSEMENT, YES, Status));
    } else {
      Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, &LocalAuthSession, TPM_RH_ENDORSEMENT, NO);
      DEBUG ((DEBUG_INFO, "\tTpm2HierarchyControl (TPM_RH_ENDORSEMENT: 0x%08x, %02x) - %r\n", TPM_RH_ENDORSEMENT, NO, Status));
    }

    //
    // Enable or disable Storage Hierarchy
    //
    if (PcdGetBool (PcdTpm2HierarchyControlOwner)) {
      Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, &LocalAuthSession, TPM_RH_OWNER, YES);
      DEBUG ((DEBUG_INFO, "\tTpm2HierarchyControl (TPM_RH_OWNER: 0x%08x, %02x) - %r\n", TPM_RH_OWNER, YES, Status));
    } else {
      Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, &LocalAuthSession, TPM_RH_OWNER, NO);
      DEBUG ((DEBUG_INFO, "\tTpm2HierarchyControl (TPM_RH_OWNER: 0x%08x, %02x) - %r\n", TPM_RH_OWNER, NO, Status));
    }
  }

Done:
  ZeroMem (&LocalAuthSession.hmac, sizeof (LocalAuthSession.hmac));

  DEBUG ((DEBUG_INFO, "[TPM2] %a: Exit\n", __FUNCTION__));

  return;
}

/**
  According to TCG PC Client Platform Firmware Profile Specification, Section 10:

  Platform Firmware MUST protect access to the Platform Hierarchy and prevent access
  to the platform hierarchy by non-manufacturer-controlled components.
  Platform Firmware SHALL:
     a. Disable the platform Hierarchy, or
     b. If the Platform Hierarchy is enabled:
        i. If the TPM is hidden, Platform Firmware MUST change platformAuth to a
        random value prior to disabling the TPM as defined in Section 6 (TPM
        Discoverability).
        ii. If the TPM is visible, Platform Firmware MUST change platformAuth to a
        random value prior to executing 3rd party code, e.g. non-manufacturer
        controlled UEFI applications.

  This service will define the configuration of the Platform Hierarchy based on above.

  @retval EFI_SUCCESS                   Configure Tpm PlatformHierarchy successfully.
  @retval other error value

**/
EFI_STATUS
EFIAPI
ConfigureTpmPlatformHierarchy (
  VOID
  )
{
  EFI_STATUS                       Status;
  TPMS_AUTH_COMMAND                LocalAuthSession;
  EFI_TCG2_PROTOCOL                *Tcg2Protocol;
  EFI_TCG2_BOOT_SERVICE_CAPABILITY ProtocolCapability;
  BOOLEAN                          TPMPresentFlag;

  TPMPresentFlag = FALSE;

  DEBUG ((DEBUG_INFO, "[TPM2] %a: Entry\n", __FUNCTION__));

  //
  // Skip PH configuration if gEfiTcg2ProtocolGuid is not found
  //
  Status = gBS->LocateProtocol (&gEfiTcg2ProtocolGuid, NULL, (VOID **)&Tcg2Protocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "\tgEfiTcg2ProtocolGuid was not found\n"));
    return EFI_SUCCESS;
  }

  HierarchyChangeAuthPlatform ();

  ZeroMem (&LocalAuthSession, sizeof (LocalAuthSession));
  LocalAuthSession.sessionHandle = TPM_RS_PW;
  LocalAuthSession.hmac.size = mNewPlatformAuth.size;
  CopyMem (LocalAuthSession.hmac.buffer, mNewPlatformAuth.buffer, mNewPlatformAuth.size);

  //
  // Disable Endorsement and Owner hierarchy if TPM 2.0 is not present or is hidden.
  //
  ProtocolCapability.Size = (UINT8)sizeof (ProtocolCapability);
  Status = Tcg2Protocol->GetCapability (
                         Tcg2Protocol,
                         &ProtocolCapability
                         );
  if (!EFI_ERROR (Status)) {
    TPMPresentFlag = ProtocolCapability.TPMPresentFlag;
  }

  if (PcdGetBool (PcdTpm2HierarchyChangeAuthPlatform)) {
    //
    // Enable/Disable Platform Hierarchy
    //
    if (PcdGetBool (PcdTpm2HierarchyControlPlatform)) {
      Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, &LocalAuthSession, TPM_RH_PLATFORM, YES);
      DEBUG ((DEBUG_INFO, "\tTpm2HierarchyControl (TPM_RH_PLATFORM: 0x%08x, %02x) - %r\n", TPM_RH_PLATFORM, YES, Status));
      //
      // Disable Endorsement and Owner hierarchy if TPM 2.0 is not present or is hidden.
      //
      if (!TPMPresentFlag) {
        Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, &LocalAuthSession, TPM_RH_ENDORSEMENT, NO);
        DEBUG ((DEBUG_INFO, "\tTpm2HierarchyControl (TPM_RH_ENDORSEMENT: 0x%08x, %02x) - %r\n", TPM_RH_ENDORSEMENT, NO, Status));
        Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, &LocalAuthSession, TPM_RH_OWNER, NO);
        DEBUG ((DEBUG_INFO, "\tTpm2HierarchyControl (TPM_RH_OWNER: 0x%08x, %02x) - %r\n", TPM_RH_OWNER, NO, Status));
      }
    } else {
      Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, &LocalAuthSession, TPM_RH_PLATFORM, NO);
      DEBUG ((DEBUG_INFO, "\tTpm2HierarchyControl (TPM_RH_PLATFORM: 0x%08x, %02x) - %r\n", TPM_RH_PLATFORM, NO, Status));
    }
  }

  //
  // Clear TPM 2.0 Secrets
  //
  ZeroMem (&mNewPlatformAuth, sizeof (mNewPlatformAuth));
  ZeroMem (&mNewLockoutAuth, sizeof (mNewLockoutAuth));
  ZeroMem (&mNewOwnerAuth, sizeof (mNewOwnerAuth));
  ZeroMem (&LocalAuthSession.hmac, sizeof (LocalAuthSession.hmac));

  DEBUG ((DEBUG_INFO, "[TPM2] %a: Exit\n", __FUNCTION__));

  return EFI_SUCCESS;
}
