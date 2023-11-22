/* $NoKeywords:$ */

/**
 * @file
 *
 * Generate SMBIOS type 7
 *
 * Contains code that generate SMBIOS type 7
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Universal
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
#include "HygonSmbios.h"
#include "HYGON.h"
#include "Filecode.h"
#include "Library/IdsLib.h"
#include "Library/BaseLib.h"
#include "Library/PrintLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Protocol/Smbios.h"
#include "Protocol/HygonSmbiosServicesProtocol.h"
#include "HygonSmbiosDxe.h"
#include "PiDxe.h"

#define FILECODE  UNIVERSAL_SMBIOS_HYGONSMBIOSTYPE7_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define CACHE_LEVEL_STR_LENGTH  11

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate SMBIOS type 7
 *
 *    @param[in]       Socket             Socket number
 *    @param[out]      CacheHandle        Handle of L1 L2 & L3 cache
 *    @param[in]       Smbios             Pointer to EfiSmbiosProtocol
 *    @param[in]       StdHeader          Handle to config for library and services
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
HygonAddSmbiosType7 (
  IN       UINTN                 Socket,
  OUT   UINT16 (*CacheHandle)[CpuLmaxCache],
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8                               CacheLevel;
  UINTN                               StructureSize;
  UINTN                               TotalSize;
  EFI_STATUS                          Status;
  EFI_STATUS                          CalledStatus;
  EFI_SMBIOS_STRING                   CacheLevelStr[CACHE_LEVEL_STR_LENGTH];
  EFI_SMBIOS_STRING                   *StrPtr;
  HYGON_SMBIOS_TABLE_TYPE7            *SmbiosTableType7;
  HYGON_CACHE_DMI_INFO                HygonCacheDmiInfo;
  HYGON_CCX_SMBIOS_SERVICES_PROTOCOL  *HygonSmbiosServices;

  Status = EFI_SUCCESS;

  CalledStatus = gBS->LocateProtocol (&gHygonCcxSmbiosServicesProtocolGuid, NULL, (VOID **)&HygonSmbiosServices);
  if (EFI_ERROR (CalledStatus)) {
    return EFI_PROTOCOL_ERROR;
  }

  HygonSmbiosServices->GetCacheDmiInfo (HygonSmbiosServices, Socket, &HygonCacheDmiInfo);

  StructureSize = sizeof (HYGON_SMBIOS_TABLE_TYPE7);
  // Version Check
  if (!HygonSmbiosVersionCheck (Smbios, 3, 1)) {
    StructureSize -= sizeof (SmbiosTableType7->MaximumCacheSize2);
    StructureSize -= sizeof (SmbiosTableType7->InstalledSize2);
  }

  TotalSize = StructureSize + CACHE_LEVEL_STR_LENGTH + 1; // +1 is for additional null (00h), End of strings

  for (CacheLevel = CpuL1Cache; CacheLevel < CpuLmaxCache; CacheLevel++) {
    if (HygonCacheDmiInfo.CacheEachLevelInfo[CacheLevel].InstallSize == 0) {
      // break if cache size is 0
      break;
    }

    SmbiosTableType7 = NULL;
    SmbiosTableType7 = AllocateZeroPool (TotalSize);
    if (SmbiosTableType7 == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    // Generate DMI type 7 --- Start
    SmbiosTableType7->Hdr.Type   = HYGON_EFI_SMBIOS_TYPE_CACHE_INFORMATION;
    SmbiosTableType7->Hdr.Length = (UINT8)StructureSize;
    SmbiosTableType7->Hdr.Handle = HYGON_SMBIOS_HANDLE_PI_RESERVED;

    SmbiosTableType7->SocketDesignation  = 1;
    SmbiosTableType7->CacheConfiguration = HygonCacheDmiInfo.CacheEachLevelInfo[CacheLevel].CacheCfg;
    SmbiosTableType7->MaximumCacheSize   = HygonCacheDmiInfo.CacheEachLevelInfo[CacheLevel].MaxCacheSize;
    SmbiosTableType7->InstalledSize = HygonCacheDmiInfo.CacheEachLevelInfo[CacheLevel].InstallSize;
    *((UINT16 *)&SmbiosTableType7->SupportedSRAMType) = HygonCacheDmiInfo.CacheEachLevelInfo[CacheLevel].SupportedSramType;
    *((UINT16 *)&SmbiosTableType7->CurrentSRAMType)   = HygonCacheDmiInfo.CacheEachLevelInfo[CacheLevel].CurrentSramType;
    SmbiosTableType7->CacheSpeed = HygonCacheDmiInfo.CacheEachLevelInfo[CacheLevel].CacheSpeed;
    SmbiosTableType7->ErrorCorrectionType = HygonCacheDmiInfo.CacheEachLevelInfo[CacheLevel].ErrorCorrectionType;
    SmbiosTableType7->SystemCacheType     = HygonCacheDmiInfo.CacheEachLevelInfo[CacheLevel].SystemCacheType;
    SmbiosTableType7->Associativity = HygonCacheDmiInfo.CacheEachLevelInfo[CacheLevel].Associativity;
    if (HygonSmbiosVersionCheck (Smbios, 3, 1)) {
      SmbiosTableType7->MaximumCacheSize2 = HygonCacheDmiInfo.CacheEachLevelInfo[CacheLevel].MaxCacheSize2;
      SmbiosTableType7->InstalledSize2    = HygonCacheDmiInfo.CacheEachLevelInfo[CacheLevel].InstallSize2;
    }

    // Generate DMI type 7 --- End

    // Append all strings
    AsciiSPrint (CacheLevelStr, CACHE_LEVEL_STR_LENGTH, "L%d - Cache", (CacheLevel + 1));
    StrPtr = (EFI_SMBIOS_STRING *)(((UINT8 *)SmbiosTableType7) + StructureSize);
    Status = AsciiStrCatS (StrPtr, CACHE_LEVEL_STR_LENGTH, CacheLevelStr);

    // Add DMI type 7
    CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType7->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *)SmbiosTableType7);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
    if (!EFI_ERROR (CalledStatus)) {
      (*CacheHandle)[CacheLevel] = SmbiosTableType7->Hdr.Handle;
    }

    // Free pool
    FreePool (SmbiosTableType7);
  }

  return Status;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
