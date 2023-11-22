/* $NoKeywords:$ */

/**
 * @file
 *
 * PSP Base Library
 *
 * Contains interface to the PSP library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  PSP
 *
 */
/*
 ******************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Uefi.h>
#include "HGPI.h"
#include "Filecode.h"
#include <Library/BaseLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/HygonPspBaseLib.h>
#include <Library/HygonPspCommonLib.h>
#include <Library/CcxRolesLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/SmnAccessLib.h>
#include <Library/PciLib.h>
#include <GnbRegisters.h>
#include <Library/HygonSmnAddressLib.h>

#define FILECODE  LIBRARY_HYGONPSPBASELIB_HYGONPSPBASELIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define NBMSIC_SMN_BASE_HYEX             0x13B10000ul             ///< Define the SMN Base address for NB MISC register for 4A
#define NBMSIC_SMN_BASE_HYGX             0x08010000ul             ///< Define the SMN Base address for NB MISC register for 4B
#define NBMSIC_MP0_BASE_ADDR_LO_OFFSET  0x2E0                    ///< Define the offset of MP0_BASE_ADDR_LO in NB MISC block
#define NBMSIC_MP0_BASE_ADDR_HI_OFFSET  0x2E4                    ///< Define the offset of MP0_BASE_ADDR_HI in NB MISC block
#define NBMSIC_MP8_BASE_ADDR_LO_OFFSET  0x328                    ///< Define the offset of MP8_BASE_ADDR_LO in NB MISC block
#define NBMSIC_MP8_BASE_ADDR_HI_OFFSET  0x32C                    ///< Define the offset of MP8_BASE_ADDR_HI in NB MISC block
#define PSP_BAR_SIZE                    0x100000ul               ///< Size of PSP BAR
#define C2P_REG_OFFSET                  0x10500ul                ///< BIOS to PSP mailbox Register SMN offset
#define BIOS_MAILBOX_BASE               0x70                     ///< BIOS to PSP Mailbox base offset on PCIe BAR

#define PSP_MUTEX_REG0_OFFSET  (24 * 4)          ///< PSP Mutex0 register offset
#define PSP_MUTEX_REG1_OFFSET  (25 * 4)          ///< PSP Mutex1 register offset

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
UINT32
Fletcher32 (
  IN OUT   UINT16  *data,
  IN       UINTN   words
  )
{
  UINT32  sum1;
  UINT32  sum2;
  UINTN   tlen;

  sum1 = 0xffff;
  sum2 = 0xffff;

  while (words) {
    tlen   = words >= 359 ? 359 : words;
    words -= tlen;
    do {
      sum2 += sum1 += *data++;
    } while (--tlen);

    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
  }

  // Second reduction step to reduce sums to 16 bits
  sum1 = (sum1 & 0xffff) + (sum1 >> 16);
  sum2 = (sum2 & 0xffff) + (sum2 >> 16);
  return sum2 << 16 | sum1;
}

BOOLEAN
ValidatePspDir (
  IN       PSP_DIRECTORY     *PspDir,
  IN       UINT32     Signature
  )
{
  // Validate Signature
  if (PspDir->Header.PspCookie != Signature) {
    return FALSE;
  }

  // Do the checksum
  if (PspDir->Header.Checksum != Fletcher32 (
                                   (UINT16 *)&PspDir->Header.TotalEntries, \
                                   (sizeof (PSP_DIRECTORY_HEADER) - OFFSET_OF (PSP_DIRECTORY_HEADER, TotalEntries) + PspDir->Header.TotalEntries * sizeof (PSP_DIRECTORY_ENTRY)) / 2
                                   )) {
    return FALSE;
  }

  return TRUE;
}

BOOLEAN
ValidateBiosDir (
  IN       BIOS_DIRECTORY     *BiosDir,
  IN       UINT32     Signature
  )
{
  // Validate Signature
  if (BiosDir->Header.PspCookie != Signature) {
    return FALSE;
  }

  // Do the checksum
  if (BiosDir->Header.Checksum != Fletcher32 (
                                    (UINT16 *)&BiosDir->Header.TotalEntries, \
                                    (sizeof (PSP_DIRECTORY_HEADER) - OFFSET_OF (PSP_DIRECTORY_HEADER, TotalEntries) + BiosDir->Header.TotalEntries * sizeof (BIOS_DIRECTORY_ENTRY)) / 2
                                    )) {
    return FALSE;
  }

  return TRUE;
}

BOOLEAN
GetComboDir (
  IN       UINT32     PspComboDirBase,
  IN       UINT32     PspChipId,
  IN OUT   UINT32     *PspDirBase
  )
{
  PSP_COMBO_DIRECTORY        *PspComboDir;
  PSP_COMBO_DIRECTORY_ENTRY  *PspComboEntry;
  UINT32                     i;

  PspComboDir   = (PSP_COMBO_DIRECTORY *)(UINTN)PspComboDirBase;
  PspComboEntry = &PspComboDir->ComboEntry[0];
  // Check the signature
  if (PspComboDir->Header.PspCookie != PSP_COMBO_DIRECTORY_HEADER_SIGNATURE) {
    return FALSE;
  }

  // Loop to found matched entry
  for (i = 0; i < PspComboDir->Header.TotalEntries; i++) {
    if (PspComboEntry[i].Id == PspChipId) {
      *PspDirBase = (UINT32)PspComboEntry[i].PspDirTableAddr;
      return TRUE;
    }
  }

  return FALSE;
}

BOOLEAN
GetDirBase (
  IN OUT   UINT32     *PspDirBase,
  IN OUT   UINT32     *BiosDirBase
  )
{
  UINTN                   i;
  FIRMWARE_ENTRY_TABLEV2  *FirmwareTableBase;
  UINT32                  PspChipId;
  BOOLEAN                 PspDirSigValid;
  BOOLEAN                 BiosDirSigValid;
  CONST UINT32            RomSigAddrTable[] =
  {
    0xFFFA0000,         // --> 512KB base
    0xFFF20000,         // --> 1MB base
    0xFFE20000,         // --> 2MB base
    0xFFC20000,         // --> 4MB base
    0xFF820000,         // --> 8MB base
    0xFF020000          // --> 16MB base
  };

  PspChipId = 0;

  if (SocFamilyIdentificationCheck () == FALSE) {
    // Unsupported Family detected
    return (FALSE);
  }

  for (i = 0; i < sizeof (RomSigAddrTable) / sizeof (UINT32); i++) {
    FirmwareTableBase = (FIRMWARE_ENTRY_TABLEV2 *)(UINTN)RomSigAddrTable[i];
    // Search flash for unique signature 0x55AA55AA
    if (FirmwareTableBase->Signature  == FIRMWARE_TABLE_SIGNATURE) {
      *BiosDirBase = FirmwareTableBase->SatBiosDirBase;
      PspChipId    = SAT_PSP_CHIP_ID;

      // Check if combo structure
      if (GetComboDir (FirmwareTableBase->PspDirBase, PspChipId, PspDirBase) == FALSE) {
        // Using the address Get from ROMSIG directly if not combo DIR
        *PspDirBase = FirmwareTableBase->PspDirBase;
      }

      PspDirSigValid  = FALSE;
      BiosDirSigValid = FALSE;
      PspDirSigValid  = ValidatePspDir ((PSP_DIRECTORY *)(UINTN)*PspDirBase, PSP_DIRECTORY_HEADER_SIGNATURE);
      BiosDirSigValid = ValidateBiosDir ((BIOS_DIRECTORY *)(UINTN)*BiosDirBase, BIOS_DIRECTORY_HEADER_SIGNATURE);

      ASSERT (PspDirSigValid);
      ASSERT (BiosDirSigValid);
      if (PspDirSigValid && BiosDirSigValid) {
        return TRUE;
      } else {
        return FALSE;
      }
    }
  }

  return (FALSE);
}

BOOLEAN
PSPEntryInfoV2 (
  IN       PSP_DIRECTORY_ENTRY_TYPE    EntryType,
  IN OUT   UINT64                      *EntryAddress,
  IN       UINT32                      *EntrySize
  )
{
  PSP_DIRECTORY  *PspDir;
  PSP_DIRECTORY  *PspLv2Dir;
  UINT32         Ignored;
  UINTN          i;
  BOOLEAN        Lv2DirExist;
  BOOLEAN        Lv2DirValid;
  UINT64         Level2EntryAddress;
  UINT32         IgnoredEntrySize;
  BOOLEAN        RecoveryFlag;

  PspDir      = NULL;
  PspLv2Dir   = NULL;
  Lv2DirExist = FALSE;
  Lv2DirValid = FALSE;
  Level2EntryAddress = 0;
  RecoveryFlag = FALSE;

  if (GetDirBase ((UINT32 *)&PspDir, &Ignored) != TRUE) {
    return FALSE;
  }

  // Check the existence of Level 2 DIR
  if (EntryType != PSP_DIR_LV2) {
    Lv2DirExist = PSPEntryInfoV2 (PSP_DIR_LV2, &Level2EntryAddress, &IgnoredEntrySize);
  }

  // Try to load entry from level2 if exist
  if (Lv2DirExist) {
    PspLv2Dir    = (PSP_DIRECTORY *)(UINTN)Level2EntryAddress;
    Lv2DirValid  = ValidatePspDir (PspLv2Dir, PSP_LV2_DIRECTORY_HEADER_SIGNATURE);
    RecoveryFlag = CheckPspRecoveryFlag ();

    if ((Lv2DirValid) && (RecoveryFlag == FALSE)) {
      for (i = 0; i < PspLv2Dir->Header.TotalEntries; i++) {
        if (PspLv2Dir->PspEntry[i].Type == EntryType) {
          *EntryAddress = PspLv2Dir->PspEntry[i].Location;
          *EntrySize    = PspLv2Dir->PspEntry[i].Size;
          return (TRUE);
        }
      }
    }
  }

  // If level 2 not exist, or can't find according entry in level 2, try in Level 1
  for (i = 0; i < PspDir->Header.TotalEntries; i++) {
    if (PspDir->PspEntry[i].Type == EntryType) {
      *EntryAddress = PspDir->PspEntry[i].Location;
      *EntrySize    = PspDir->PspEntry[i].Size;
      return (TRUE);
    }
  }

  return (FALSE);
}

/**
 * Get PSP Entry SCS binary information
 *
 *
 *
 * @param[in,out] EntryAddress        Address of SCS binary
 * @param[in,out] EntrySize           Size of the SCS binary
 */
BOOLEAN
GetPspEntryScsBinaryV2 (
  IN OUT   UINT64                      *EntryAddress,
  IN       UINT32                      *EntrySize
  )
{
  return PSPEntryInfoV2 (HYGON_SCS_BINARY, EntryAddress, EntrySize);
}

/**
 * Get PSP Entry PSP NV data information
 *
 *
 *
 * @param[in,out] EntryAddress        Address of PSP NV data
 * @param[in,out] EntrySize           Size of the PSP NV data
 */
BOOLEAN
GetPspEntryPspNvDataV2 (
  IN OUT   UINT64                      *EntryAddress,
  IN       UINT32                      *EntrySize
  )
{
  return PSPEntryInfoV2 (PSP_NV_DATA, EntryAddress, EntrySize);
}

/**
 *
 *  Get BIOS Directory Entry 's properties
 *
 *  @param[in]     EntryType    BIOS Directory Entry type
 *  @param[in]     EntryInstance    If input with INSTANCE_IGNORED, will return 1st Entry type matched
 *                                  If input with Non INSTANCE_IGNORED, will return the entry which both Type & Instance matched
 *  @param[in,out] TypeAttrib   TypeAttrib of entry
 *  @param[in,out] EntryAddress Address of entry
 *  @param[in,out] EntrySize    Size of entry
 *  @param[in,out] EntryDest    Destination of entry
 *
 *  @retval TRUE   Success to get the Entry 's properties
 *  @retval FALSE  Fail to get the Entry 's properties
 *
 **/
BOOLEAN
BIOSEntryInfo (
  IN       UINT8                       EntryType,
  IN       UINT8                       EntryInstance,
  IN OUT   TYPE_ATTRIB                 *TypeAttrib,
  IN OUT   UINT64                      *EntryAddress,
  IN OUT   UINT32                      *EntrySize,
  IN OUT   UINT64                      *EntryDest
  )
{
  BIOS_DIRECTORY  *BiosDir;
  BIOS_DIRECTORY  *BiosLv2Dir;
  UINT32          Ignored;
  UINTN           i;
  BOOLEAN         Found;
  BOOLEAN         Lv2DirExist;
  BOOLEAN         Lv2DirValid;
  UINT64          Level2EntryAddress;
  TYPE_ATTRIB     IgnoredTypeAttrib;
  UINT32          IgnoredEntrySize;
  UINT64          IgnoredEntryDest;
  BOOLEAN         RecoveryFlag;

  BiosDir     = NULL;
  BiosLv2Dir  = NULL;
  Lv2DirExist = FALSE;
  Lv2DirValid = FALSE;
  Level2EntryAddress = 0;
  Found = FALSE;
  RecoveryFlag = FALSE;

  if (GetDirBase (&Ignored, (UINT32 *)&BiosDir) != TRUE) {
    return FALSE;
  }

  // Check the existence of Level 2 DIR
  if (EntryType != BIOS_DIR_LV2) {
    Lv2DirExist = BIOSEntryInfo (BIOS_DIR_LV2, INSTANCE_IGNORED, &IgnoredTypeAttrib, &Level2EntryAddress, &IgnoredEntrySize, &IgnoredEntryDest);
  }

  // Try to load entry from level2 if exist
  if (Lv2DirExist) {
    BiosLv2Dir   = (BIOS_DIRECTORY *)(UINTN)Level2EntryAddress;
    Lv2DirValid  = ValidateBiosDir (BiosLv2Dir, BIOS_LV2_DIRECTORY_HEADER_SIGNATURE);
    RecoveryFlag = CheckPspRecoveryFlag ();

    if ((Lv2DirValid) && (RecoveryFlag == FALSE)) {
      for (i = 0; i < BiosLv2Dir->Header.TotalEntries; i++) {
        if (BiosLv2Dir->BiosEntry[i].TypeAttrib.Type == EntryType) {
          if (EntryInstance == INSTANCE_IGNORED) {
            Found = TRUE;
          } else if (BiosLv2Dir->BiosEntry[i].TypeAttrib.Instance == EntryInstance) {
            Found = TRUE;
          }

          if (Found == TRUE) {
            *TypeAttrib   = BiosLv2Dir->BiosEntry[i].TypeAttrib;
            *EntryAddress = BiosLv2Dir->BiosEntry[i].Location;
            *EntrySize    = BiosLv2Dir->BiosEntry[i].Size;
            *EntryDest    = BiosLv2Dir->BiosEntry[i].Destination;
            return (TRUE);
          }
        }
      }
    }

    Found = FALSE;
  }

  // If level 2 not exist, or can't find according entry in level 2, try in Level 1
  Found = FALSE;
  for (i = 0; i < BiosDir->Header.TotalEntries; i++) {
    if (BiosDir->BiosEntry[i].TypeAttrib.Type == EntryType) {
      if (EntryInstance == INSTANCE_IGNORED) {
        Found = TRUE;
      } else if (BiosDir->BiosEntry[i].TypeAttrib.Instance == EntryInstance) {
        Found = TRUE;
      }

      if (Found == TRUE) {
        *TypeAttrib   = BiosDir->BiosEntry[i].TypeAttrib;
        *EntryAddress = BiosDir->BiosEntry[i].Location;
        *EntrySize    = BiosDir->BiosEntry[i].Size;
        *EntryDest    = BiosDir->BiosEntry[i].Destination;
        return (TRUE);
      }
    }
  }

  return (FALSE);
}

/**
  Check if PSP device is present

  @retval BOOLEAN  0: PSP Disabled, 1: PSP Enabled

**/
BOOLEAN
CheckPspDevicePresentV2 (
  VOID
  )
{
  /// @todo Add code to read SMU FUSE shadow register
  return (TRUE);
}

/**
  Check PSP Platform Seucre Enable State
  HVB & Secure S3 (Resume vector set to Dram, & core content will restore by uCode)
  will be applied if Psp Plaform Secure is enabled

  @retval BOOLEAN  0: PSP Platform Secure Disabled, 1: PSP Platform Secure  Enabled

**/
BOOLEAN
CheckPspPlatformSecureEnable (
  VOID
  )
{
  /// @todo Add code to read SMU FUSE shadow register
  return (TRUE);
}

/**
  Return the PspMbox MMIO location


  @retval EFI_STATUS  0: Success, NonZero Error

**/
BOOLEAN
GetPspMboxLocation (
  IN OUT   PSP_MBOX **PspMbox
  )
{
  UINT32  PspMmioBase;
  UINT32  *PspMboxU32;

  if (GetPspMmioBase (&PspMmioBase) == FALSE) {
    return FALSE;
  }

  *PspMbox = (PSP_MBOX *)(UINTN)(PspMmioBase + C2P_REG_OFFSET + BIOS_MAILBOX_BASE);

  // Ensure PSP open the decoder
  PspMboxU32 = (UINT32 *)*PspMbox;
  ASSERT (*((UINT32 *)PspMbox) != 0xFFFFFFFFul);
  return TRUE;
}

/**
  Check PSP Recovery Flag
  Target will set Recovery flag if some PSP entry point by PSP directory has been corrupted.

  @retval BOOLEAN  0: Recovery Flag is cleared, 1: Recovery Flag has been set

**/
BOOLEAN
CheckPspRecoveryFlag (
  VOID
  )
{
  PSP_MBOX  *PspMbox;

  PspMbox = NULL;

  if (PcdGetBool (PcdHygonPspRecoveryFlagDetectEnable) == FALSE) {
    return FALSE;
  }

  // Init PSP MMIO
  PspBarInitEarly ();
  // NOTE, GetPspMboxLocation is reading the address from index will have the potential multiple core racinng issue
  GetPspMboxLocation (&PspMbox);

  return (BOOLEAN)(PspMbox->Cmd.Field.Recovery);
}

VOID
PspBarInitEarly (
  VOID
  )
{
  UINT32                 Value32;
  UINT64                 PspMmioBase;
  UINT64                 Length;
  FABRIC_TARGET          MmioTarget;
  EFI_STATUS             Status;
  FABRIC_MMIO_ATTRIBUTE  Attributes;
  UINTN                  IohubPhysicalDieId;
  UINT32                 CpuModel;

  if ((CcxIsBsp (NULL) == FALSE)) {
    return;
  }

  // Check if PSP BAR has been assigned, if not do the PSP BAR initialation
  if (GetPspMmioBase (&Value32) == FALSE) {
    // Allocate MMIO Region from MMIO manager
    Length = PSP_BAR_SIZE;
    MmioTarget.TgtType     = TARGET_RB;
    MmioTarget.SocketNum   = 0;
    MmioTarget.DieNum      = 0;
    MmioTarget.RbNum       = 0;
    Attributes.ReadEnable  = 1;
    Attributes.WriteEnable = 1;
    Attributes.NonPosted   = 0;
    Attributes.MmioType    = NON_PCI_DEVICE_BELOW_4G;
    PspMmioBase = 0;
    Status = FabricAllocateMmio (&PspMmioBase, &Length, ALIGN_1M, MmioTarget, &Attributes);
    ASSERT (Status == EFI_SUCCESS);
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("PspBarInitEarly Allocate MMIO Fail\n");
    } else {
      IDS_HDT_CONSOLE_PSP_TRACE ("PspBarInitEarly Allocate MMIO @0x%lx\n", PspMmioBase);
    }

    IohubPhysicalDieId = FabricTopologyGetIohubPhysicalDieId (0);

    CpuModel = GetHygonSocModel();
    if (CpuModel == HYGON_EX_CPU) {
      //Set PSP BASE Address in NBMISC, and enable lock the MMIO
      Value32 = (UINT32) (PspMmioBase | (BIT0 + BIT8));
      SmnRegisterWrite(0, NBMSIC_SMN_BASE_HYEX + NBMSIC_MP0_BASE_ADDR_LO_OFFSET, &Value32, 0);

      Value32 = (UINT32) RShiftU64 (PspMmioBase, 32);
      SmnRegisterWrite(0, NBMSIC_SMN_BASE_HYEX + NBMSIC_MP0_BASE_ADDR_HI_OFFSET, &Value32, 0);
    } else if (CpuModel == HYGON_GX_CPU) {
      // Set PSP BASE Address in NBMISC, and enable lock the MMIO
      Value32 = (UINT32)(PspMmioBase | (BIT0 + BIT8));
      SmnRegisterWrite (0, IOD_SPACE (IohubPhysicalDieId, NBMSIC_SMN_BASE_HYGX + NBMSIC_MP8_BASE_ADDR_LO_OFFSET), &Value32, 0);

      Value32 = (UINT32)RShiftU64 (PspMmioBase, 32);
      SmnRegisterWrite (0, IOD_SPACE (IohubPhysicalDieId, NBMSIC_SMN_BASE_HYGX + NBMSIC_MP8_BASE_ADDR_HI_OFFSET), &Value32, 0);
    }
    
  }
}

/**
  Return the PspMMIO MMIO location

  @param[in] PspMmioBase Pointer to Psp MMIO address

  @retval BOOLEAN  0: Error, 1 Success
**/
BOOLEAN
GetPspMmioBase (
  IN OUT   UINT32 *PspMmioBase
  )
{
  UINT32  Value32;
  UINTN   IohubPhysicalDieId;
  UINT32  CpuModel;

  *PspMmioBase = 0;

  IohubPhysicalDieId = FabricTopologyGetIohubPhysicalDieId (0);
  Value32            = 0;

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    SmnRegisterRead(0, NBMSIC_SMN_BASE_HYEX + NBMSIC_MP0_BASE_ADDR_LO_OFFSET, &Value32);
  } else if (CpuModel == HYGON_GX_CPU) {
    SmnRegisterRead (0, IOD_SPACE (IohubPhysicalDieId, NBMSIC_SMN_BASE_HYGX + NBMSIC_MP8_BASE_ADDR_LO_OFFSET), &Value32);
  }

  // Mask out the lower bits
  Value32 &= 0xFFF00000;

  if (Value32 == 0) {
    return (FALSE);
  }

  *PspMmioBase = Value32;
  return (TRUE);
}

/// @todo remove below FCH definition when it has been declared by FCH module
#define ACPI_MMIO_BASE   0xFED80000ul
#define PMIO_BASE        0x300  // DWORD
#define FCH_PMIOA_REG64  0x64   // AcpiPmTmrBlk

/**
 * PSP Time out function with conditioner
 * The routine will exit in two conditions:
 * 1. Time out of input uSec
 * 2. Conditioner function return TRUE
 *
 * @param[in] uSec         Timer in microseconds
 * @param[in] Conditioner  Function for check condition of exit the timeout routine
 * @param[in] Context      Conditioner function context
 *
 * @retval BOOLEAN  FALSE: Timer exceed, TRUE: condition match
 */
BOOLEAN
PspLibTimeOutV2 (
  IN       UINT64              uSec,
  IN       FP_CONDITIONER      Conditioner,
  IN       VOID                *Context
  )
{
  UINT16  timerAddr;
  UINT32  startTime;
  UINT64  elapsedTime;

  // Return immediately if condition already match
  if (Conditioner (Context) == TRUE) {
    return TRUE;
  }

  // Check if infinitely flag set
  if (uSec == PSPLIB_WAIT_INFINITELY) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Wait infinitely ..\n");
    while (Conditioner (Context) == FALSE) {
      ;
    }

    return TRUE;
  }

  // Check availibitly of ACPI timer
  LibHygonMemRead (AccessWidth16, (UINT64)(ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG64), &timerAddr, NULL);
  // Using IO delay, if ACPI timer is not available, it is not accurate, since the conditioner function will take time
  if ( timerAddr == 0 ) {
    uSec = DivU64x32 (uSec, 2);
    while ( uSec != 0) {
      if (Conditioner (Context) == TRUE) {
        return TRUE;
      }

      LibHygonIoRead (AccessWidth8, 0xEB, (UINT8 *)&elapsedTime, NULL);
      uSec--;
    }
  } else {
    LibHygonIoRead (AccessWidth32, timerAddr, &startTime, NULL);
    elapsedTime = 0;
    for ( ; ; ) {
      LibHygonIoRead (AccessWidth32, timerAddr, &elapsedTime, NULL);
      if ( elapsedTime <= startTime ) {
        elapsedTime = elapsedTime + 0xFFFFFFFFul - startTime;
      } else {
        elapsedTime = elapsedTime - startTime;
      }

      if (Conditioner (Context) == TRUE) {
        return TRUE;
      }

      if (DivU64x32 (MultU64x32 (elapsedTime, 28), 100) > uSec ) {
        break;
      }
    }
  }

  return FALSE;
}

BOOLEAN
GetFtpmControlAreaV2 (
  IN OUT   VOID **FtpmControlArea
  )
{
  UINT32  PspMmioBase;

  if (GetPspMmioBase (&PspMmioBase) == FALSE) {
    return FALSE;
  }

  *FtpmControlArea = (VOID *)(UINTN)(PspMmioBase + C2P_REG_OFFSET);
  return TRUE;
}

VOID
SwitchPspMmioDecodeV2 (
  IN       BOOLEAN SwitchFlag,
  IN OUT   UINT32 *RegisterCopy
  )
{
}

/**
 * Acquire the Mutex for access PSP,X86 co-accessed register
 * Call this routine before access SMIx98 & SMIxA8
 *
 */
VOID
AcquirePspSmiRegMutexV2 (
  VOID
  )
{
  UINT32  PspMmioBase;
  UINT32  MutexReg0;
  UINT32  MutexReg1;

  PspMmioBase = 0;
  if (GetPspMmioBase (&PspMmioBase)) {
    MutexReg0 = PspMmioBase + PSP_MUTEX_REG0_OFFSET;
    MutexReg1 = PspMmioBase + PSP_MUTEX_REG1_OFFSET;
    *(volatile UINT32 *)(UINTN)(MutexReg0) |= BIT0;
    *(volatile UINT32 *)(UINTN)(MutexReg1) |= BIT0;
    // Wait till PSP FW release the mutex
    while ((*(volatile UINT32 *)(UINTN)(MutexReg0)& BIT1) && (*(volatile UINT32 *)(UINTN)(MutexReg1) & BIT0)) {
      ;
    }
  }
}

/**
 * Release the Mutex for access PSP,X86 co-accessed register
 * Call this routine after access SMIx98 & SMIxA8
 *
 */
VOID
ReleasePspSmiRegMutexV2 (
  VOID
  )
{
  UINT32  PspMmioBase;
  UINT32  MutexReg0;

  PspMmioBase = 0;
  if (GetPspMmioBase (&PspMmioBase)) {
    MutexReg0 = PspMmioBase + PSP_MUTEX_REG0_OFFSET;
    *(volatile UINT32 *)(UINTN)(MutexReg0) &= ~BIT0;
  }
}

#define MP0_PUBLIC0_SMN_BASE_HYEX  (0x03800000)
#define MP0_PUBLIC0_SMN_BASE_HYGX  (0x01A00000)

/*
 *  Routine to read a C2P register.
 *
 * @param[in] Socket             Processor socket to read from
 * @param[in] Die                Die number on Socket to read from
 * @param[in] C2pMsgOffset       SMN address offset, e.g. MP0_C2PMSG_37_SMN_OFFSET
 * @param[in] Instance           Instance ID of the target fabric device
 * @retval    HGPI_BOUNDS_CHK   Invalid Socket or Die
 * @retval    HGPI_SUCCESS      Value retrieved successfully
 */
HGPI_STATUS
ReadC2PMsgReg (
  IN       UINTN      Socket,
  IN       UINTN      Die,
  IN       UINT32      C2pMsgOffset,
  IN OUT   UINT32     *RegValue
  )
{
  UINTN  HostBridgeBusBase;
  UINTN  LogicalDie;
  UINTN  DieType;
  UINT32 CpuModel;

  // Check the validity of Socket and Die
  if (Socket >= FabricTopologyGetNumberOfSocketPresent ()) {
    return HGPI_BOUNDS_CHK;
  }

  if (Die >= (FabricTopologyGetNumberOfPhysicalDiesOnSocket (Socket))) {
    return HGPI_BOUNDS_CHK;
  }

  FabricTopologyGetPhysicalIodDieInfo (Die, &LogicalDie, &DieType);

  // Get the PCI host bridge bus base used to access SMN register
  HostBridgeBusBase = FabricTopologyGetHostBridgeBusBase (Socket, LogicalDie, 0);

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    SmnRegisterRead ((UINT32) HostBridgeBusBase, MP0_PUBLIC0_SMN_BASE_HYEX + C2pMsgOffset, RegValue);
  } else if (CpuModel == HYGON_GX_CPU) {
    SmnRegisterRead ((UINT32)HostBridgeBusBase, IOD_SPACE (0, MP0_PUBLIC0_SMN_BASE_HYGX + C2pMsgOffset), RegValue);
  }

  return HGPI_SUCCESS;
}
