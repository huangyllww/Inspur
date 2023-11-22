/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Ccx Microcode Patch Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Lib
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
#include <Library/BaseLib.h>
#include <Filecode.h>
#include <Library/PcdLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/CcxRolesLib.h>
#include <Library/CcxMicrocodePatchLib.h>
#include <Library/HygonPspBaseLib.h>
#include <Library/PciLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/HygonPspMboxLib.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_CCXMICROCODEPATCHLIB_CCXMICROCODEPATCHLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

BOOLEAN
STATIC
GetPatchEquivalentId (
  IN OUT   UINT16             *ProcessorEquivalentId,
  IN       UINT16             *MicrocodeEquivalenceTable,
  IN       UINT8              EquivalencyEntries,
  IN OUT   HYGON_CONFIG_PARAMS  *StdHeader
  );

BOOLEAN
STATIC
ValidateMicrocode (
  IN       MICROCODE_PATCH    *MicrocodePatchPtr,
  IN       UINT16             ProcessorEquivalentId,
  IN OUT   HYGON_CONFIG_PARAMS  *StdHeader
  );

BOOLEAN
STATIC
LoadMicrocode (
  IN       MICROCODE_PATCH    *MicrocodePatchPtr,
  IN OUT   HYGON_CONFIG_PARAMS  *StdHeader
  );

VOID
STATIC
GetMicrocodeVersion (
  OUT   UINT32             *pMicrocodeVersion,
  IN OUT   HYGON_CONFIG_PARAMS  *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/

/**
 *  Update microcode patch in current processor.
 *
 *  Then reads the patch id, and compare it to the expected, in the Microprocessor
 *  patch block.
 *
 *  @param[in] MicrocodeEquivalenceTable - Pointer to the Equivalence table.
 *  @param[in] EquivalencyEntries        - Number of valid entries in the Equivalence table.
 *  @param[in,out] UcodePatchAddr        - The selected UcodePatch address, return 0 if not found
 *  @param[in] StdHeader                 - Config handle for library and services.
 *
 *  @retval    TRUE   - Patch Loaded Successfully.
 *  @retval    FALSE  - Patch Did Not Get Loaded.
 *
 */
BOOLEAN
LoadMicrocodePatch (
  IN       UINT16            *MicrocodeEquivalenceTable,
  IN       UINT8             EquivalencyEntries,
  IN OUT   UINT64            *UcodePatchAddr,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT8        InstanceNumber;
  UINT16       ProcessorEquivalentId;
  TYPE_ATTRIB  TypeAttrib;
  UINT64       EntryAddress;
  UINT32       EntrySize;
  UINT64       EntryDest;
  BOOLEAN      Status;

  Status = FALSE;
  *UcodePatchAddr = 0;
  if (CcxIsBsp (StdHeader)) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  Start to load microcode patch\n");
  }

  // Get the processor microcode patch equivalent ID
  if (GetPatchEquivalentId (&ProcessorEquivalentId, MicrocodeEquivalenceTable, EquivalencyEntries, StdHeader)) {
    // parse the patch table to see if we have one for the current cpu
    InstanceNumber = 0;
    while (BIOSEntryInfo (UCODE_PATCH, InstanceNumber, &TypeAttrib, &EntryAddress, &EntrySize, &EntryDest)) {
      if (ValidateMicrocode ((MICROCODE_PATCH *)(UINTN)EntryAddress, ProcessorEquivalentId, StdHeader)) {
        *UcodePatchAddr = EntryAddress;
        IDS_HDT_CONSOLE (CPU_TRACE, "  UCODE PATCH 0x66 Entry Address 0x%X \n", EntryAddress);
        if (LoadMicrocode ((MICROCODE_PATCH *)(UINTN)EntryAddress, StdHeader)) {
          Status = TRUE;
        }

        break; // Once we find a microcode patch that matches the processor, exit the while loop
      }

      InstanceNumber++;
    }
  }

  return Status;
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/

/**
 *
 *  LoadMicrocode
 *
 *    Update microcode patch in current processor, then reads the
 *    patch id, and compare it to the expected, in the Microprocessor
 *    patch block.
 *
 *    @param[in]       MicrocodePatchPtr  - Pointer to Microcode Patch.
 *    @param[in,out]   StdHeader          - Pointer to HYGON_CONFIG_PARAMS struct.
 *
 *    @retval          TRUE  - Patch Loaded Successfully.
 *    @retval          FALSE - Patch Did Not Get Loaded.
 *
 */
BOOLEAN
STATIC
LoadMicrocode (
  IN       MICROCODE_PATCH    *MicrocodePatchPtr,
  IN OUT   HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32        MicrocodeVersion;
  PATCH_LOADER  PatchLoaderMsr;

  // Load microcode patch into CPU
  PatchLoaderMsr.RawData = (UINT64)(UINTN)MicrocodePatchPtr;
  PatchLoaderMsr.BitFields.SBZ = 0;
  AsmWriteMsr64 (MSR_PATCH_LOADER, PatchLoaderMsr.RawData);

  // Do ucode patch Authentication
  // Read microcode version back from CPU, determine if
  // it is the same patch level as contained in the source
  // microprocessor patch block passed in
  GetMicrocodeVersion (&MicrocodeVersion, StdHeader);
  if (MicrocodeVersion == MicrocodePatchPtr->PatchID) {
    if (CcxIsBsp (StdHeader)) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Microcode Patch %X is loaded\n", MicrocodeVersion);
    }

    return (TRUE);
  } else {
    IDS_HDT_CONSOLE (CPU_TRACE, "  Patch ID error, expect %x, actual %x\n", MicrocodePatchPtr->PatchID, MicrocodeVersion);
    return (FALSE);
  }
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  GetPatchEquivalentId
 *
 *    Return the equivalent ID for microcode patching
 *
 *    @param[in,out]   ProcessorEquivalentId     - Pointer to Processor Equivalent ID table.
 *    @param[in]       MicrocodeEquivalenceTable - Pointer to the Equivalence table.
 *    @param[in]       EquivalencyEntries        - Number of valid entries in the Equivalence table.
 *    @param[in,out]   StdHeader                 - Pointer to HYGON_CONFIG_PARAMS struct.
 *
 *    @retval          TRUE  - Equivalence Id found.
 *    @retval          FALSE - Equivalence Id not found.
 *
 */
BOOLEAN
STATIC
GetPatchEquivalentId (
  IN OUT   UINT16             *ProcessorEquivalentId,
  IN       UINT16             *MicrocodeEquivalenceTable,
  IN       UINT8              EquivalencyEntries,
  IN OUT   HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8   i;
  UINT16  ProcessorRevisionId;
  UINT32  SocRawId;

  // Read CPUID for family information
  SocRawId = BaseGetRawIdOnExecutingCore ();

  // High byte contains extended model and extended family
  ProcessorRevisionId = (UINT16)((SocRawId & (CPUID_EXT_MODEL_MASK | CPUID_EXT_FAMILY_MASK)) >> 8);

  // Low byte contains model and family
  ProcessorRevisionId |= (SocRawId & (CPUID_STEPPING_MASK | CPUID_BASE_MODEL_MASK));

  IDS_HDT_CONSOLE (CPU_TRACE, "  ProcessorRevisionId is 0x%X\n", ProcessorRevisionId);
  // Parse the equivalence table
  for (i = 0; i < (EquivalencyEntries * 2); i += 2) {
    // Check for equivalence
    if (ProcessorRevisionId == MicrocodeEquivalenceTable[i]) {
      *ProcessorEquivalentId = MicrocodeEquivalenceTable[i + 1];
      IDS_HDT_CONSOLE (CPU_TRACE, "  Found ProcessorEquivalentId is 0x%X\n", *ProcessorEquivalentId);
      return (TRUE);
    }
  }

  // Reached the end of equivalence table, this processor is not supported
  IDS_HDT_CONSOLE (CPU_TRACE, "  Not found ProcessorEquivalentId \n");
  *ProcessorEquivalentId = 0x0000;
  return (FALSE);
}

/*---------------------------------------------------------------------------------------*/

/**
 *
 *  ValidateMicrocode
 *
 *    Determine if the microcode patch block, currently pointed to
 *    is valid, and is appropriate for the current processor

 *    @param[in]       MicrocodePatchPtr      - Pointer to Microcode Patch.
 *    @param[in]       ProcessorEquivalentId  - Pointer to Processor Equivalent ID table.
 *    @param[in,out]   StdHeader              - Pointer to HYGON_CONFIG_PARAMS struct.
 *
 *    @retval          TRUE  - Patch Found.
 *    @retval          FALSE - Patch Not Found.
 *
 */
BOOLEAN
STATIC
ValidateMicrocode (
  IN       MICROCODE_PATCH    *MicrocodePatchPtr,
  IN       UINT16             ProcessorEquivalentId,
  IN OUT   HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  BOOLEAN   Chipset1Matched;
  BOOLEAN   Chipset2Matched;
  PCI_ADDR  PciAddress;
  UINT32    PciDeviceVidDid;
  UINT8     PciDeviceRevision;
  UINT8     DevCount;
  UINT8     FunCount;
  UINT32    Chipset1DeviceID;
  UINT32    Chipset2DeviceID;
  UINT8     MulitFunction;

  Chipset1Matched   = FALSE;
  Chipset2Matched   = FALSE;
  PciDeviceVidDid   = 0;
  PciDeviceRevision = 0;
  Chipset1DeviceID  = MicrocodePatchPtr->Chipset1DeviceID;
  Chipset2DeviceID  = MicrocodePatchPtr->Chipset2DeviceID;
  MulitFunction     = 0;

  if (MicrocodePatchPtr->ProcessorRevisionID !=  ProcessorEquivalentId) {
    return (FALSE);
  }

  //
  // parse the supplied microcode to see if it is compatible with the processor
  //
  IDS_HDT_CONSOLE (CPU_TRACE, "DateCode %X \n", MicrocodePatchPtr->DateCode);
  IDS_HDT_CONSOLE (CPU_TRACE, "PatchID %X \n", MicrocodePatchPtr->PatchID);
  IDS_HDT_CONSOLE (CPU_TRACE, "MicrocodePatchDataID %X \n", MicrocodePatchPtr->MicrocodePatchDataID);
  IDS_HDT_CONSOLE (CPU_TRACE, "MicrocodePatchDataLength %X \n", MicrocodePatchPtr->MicrocodePatchDataLength);
  IDS_HDT_CONSOLE (CPU_TRACE, "Ucode ProcessorRevisionID %X,  ProcessorEquivalentId %X\n", MicrocodePatchPtr->ProcessorRevisionID, ProcessorEquivalentId);

  if (Chipset1DeviceID == 0) {
    Chipset1Matched = TRUE;
  }

  if (Chipset2DeviceID == 0) {
    Chipset2Matched = TRUE;
  }

  if ((!Chipset1Matched) || (!Chipset2Matched)) {
    //
    // Scan all PCI devices in Bus 0, try to find out matched case.
    //
    for (DevCount = 0; DevCount < 32; DevCount++) {
      for (FunCount = 0; FunCount < 8; FunCount++) {
        PciAddress.AddressValue = MAKE_SBDFO (0, 0, DevCount, FunCount, 0);
        PciDeviceVidDid = PciRead32 (PciAddress.AddressValue);
        if (PciDeviceVidDid == 0xFFFFFFFF) {
          if (FunCount == 0) {
            break;
          } else {
            continue;
          }
        }

        PciAddress.Address.Register = 0x8;
        PciDeviceRevision = PciRead8 (PciAddress.AddressValue);
        if ((!Chipset1Matched) && (PciDeviceVidDid == Chipset1DeviceID)) {
          if (PciDeviceRevision == MicrocodePatchPtr->Chipset1RevisionID) {
            Chipset1Matched = TRUE;
          }
        }

        if ((!Chipset2Matched) && (PciDeviceVidDid == Chipset2DeviceID)) {
          if (PciDeviceRevision == MicrocodePatchPtr->Chipset2RevisionID) {
            Chipset2Matched = TRUE;
          }
        }

        if (Chipset1Matched && Chipset2Matched) {
          break;
        }

        //
        // Check multi-function. If it doesen't exist, we don't have to loop functions to 7.
        //
        if (FunCount == 0) {
          MulitFunction = 0;
          PciAddress.Address.Register = 0xE;
          MulitFunction = PciRead8 (PciAddress.AddressValue);
          if ((MulitFunction & 0x80) == 0) {
            break;
          }
        }
      } // end FunCount for loop.

      if (Chipset1Matched && Chipset2Matched) {
        break;
      }
    }  // end DevCount for loop.
  }

  return (Chipset1Matched && Chipset2Matched);
}

/*---------------------------------------------------------------------------------------*/

/**
 *
 *  GetMicrocodeVersion
 *
 *    Return the version of the currently loaded microcode patch, if any.
 *    Read from the patch level MSR, return the value in eax. If no patch
 *    has been loaded, 0 will be returned.
 *
 *    @param[out]      pMicrocodeVersion  - Pointer to Microcode Version.
 *    @param[in,out]   StdHeader          - Pointer to HYGON_CONFIG_PARAMS struct.
 *
 */
VOID
STATIC
GetMicrocodeVersion (
  OUT   UINT32             *pMicrocodeVersion,
  IN OUT   HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  UINT64  MsrData;

  MsrData = 0;
  MsrData = AsmReadMsr64 (MSR_PATCH_LEVEL);

  *pMicrocodeVersion = (UINT32)MsrData;
}
