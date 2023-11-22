/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON HSTI DXE Driver Header file
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  PSP
 *
 */
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
 *
 ******************************************************************************
 */
#define  HSTI_HYGON_FEATUERS_SIZE_IN_BYTES  (MAX_HSTI_HYGON_FEATURE_BYTE_INDEX + 1)
#define  HSTI_HYGON_ERROR_STRING_SIZE       0x100

#define MAX_HSTI_HYGON_FEATURE_BYTE_INDEX  HSTI_HYGON_FEATURE_BYTE_INDEX_5

// Security Feature bitmap
// Security Feature Byte Index 0
#define HSTI_HYGON_FEATURE_BYTE_INDEX_0  0                                  ///< Byte index 0
#define HSTI_HYGON_CRYPTO_STRENGTH       BIT0                               ///< Do you use RSA 2048 and SHA256 only (or similar crypto strength)

/// 2. Firmware Code must be present in protected storage
/// Security Feature Byte Index 1
#define HSTI_HYGON_FEATURE_BYTE_INDEX_1            1                        ///< Byte index 1
#define HSTI_HYGON_FWCODE_PROTECT_PROTECT_SPI      BIT0                     ///< Do you protect spiflash?
#define HSTI_HYGON_FWCODE_PROTECT_SIGNED_FW_CHECK  BIT2                     ///< Do you support Signed Firmware Check Firmware that is installed by OEM is either readonly or protected by secure firmware update process

/// 3. Secure firmware update process
/// Security Feature Byte Index 2
#define HSTI_HYGON_FEATURE_BYTE_INDEX_2                          2          ///< Byte index 2
#define HSTI_HYGON_SECURE_FW_UPDATE_DFT_TESTKEY                  BIT0       ///< Is secure firmware update process on by default with test keys?  (RECOMMENDED)
#define HSTI_HYGON_SECURE_FW_UPDATE_CHECK_TESTKEY_IN_PRODUCTION  BIT1       ///< Do you check if test keys are used in production?
#define HSTI_HYGON_SECURE_FW_UPDATE_ROLLBACK_CHECK               BIT2       ///< Do you allow rollback to insecure firmware  version? If yes then what is the protection mechanism? Do you clear TPM when rollback happens?

/// 4.Do you have backdoors to override SecureBoot
/// Security Feature Byte Index 3
#define HSTI_HYGON_FEATURE_BYTE_INDEX_3                     3               ///< Byte index 3
#define HSTI_HYGON_SECUREBOOT_BACKDOOR_INLINE_PROMPT_CHECK  BIT0            ///< a. Does your system support inline prompting to bypass Secureboot? If yes then is it disabled while SecureBoot is enabled
#define HSTI_HYGON_SECUREBOOT_BACKDOOR_MANUFACTURE_CHECK    BIT1            ///< b. Do  you  have  manufacturing  backdoors?  Do  you  check  for  them  to  be  disabled  while SecureBoot is enabled and always disabled in production system?

/// Security Feature Byte Index 4
#define HSTI_HYGON_FEATURE_BYTE_INDEX_4                 4                   ///< Byte index 4
#define HSTI_HYGON_FEATURE_PSP_SECURE_EN                BIT0                ///< Fuse SecureEnable to ensure silicon level secure
#define HSTI_HYGON_FEATURE_PSP_PLATFORM_SECURE_BOOT_EN  BIT1                ///< Boot Integrity Support
#define HSTI_HYGON_FEATURE_PSP_DEBUG_LOCK_ON            BIT2                ///< Protection against external hardware debugger
/// Compatibility Support Modules (CSM)
/// Security Feature Byte Index 5
#define HSTI_HYGON_FEATURE_BYTE_INDEX_5          5                          ///< Byte index 5
#define HSTI_HYGON_CSM_DISABLE_IF_SECUREBOOT_EN  BIT0                       ///< b.  Do you check blocking of loading CSM when SecureBoot is enabled
#define HSTI_HYGON_CSM_DISABLE_ON_CS_SYSTEM      BIT1                       ///< c.  [CS]Do you check if CSM is not present on CS systems permanently
