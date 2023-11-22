/* $NoKeywords:$ */

/**
 * @file
 *
 * Generate SMBIOS type 4 7 16 17 18 19 20
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

#ifndef _UNIVERSAL_SMBIOS_HYGON_SMBIOS_DXE_H_
#define _UNIVERSAL_SMBIOS_HYGON_SMBIOS_DXE_H_
#pragma pack (push, 1)

BOOLEAN
HygonSmbiosVersionCheck (
  IN       EFI_SMBIOS_PROTOCOL *Smbios,
  IN       UINT8                MajorVersion,
  IN       UINT8                MinorVersion
  );

EFI_STATUS
EFIAPI
HygonAddSmbiosType4 (
  IN       UINTN                 Socket,
  IN       UINT16                CacheHandle[CpuLmaxCache],
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  );

EFI_STATUS
EFIAPI
HygonAddSmbiosType4ForNoProcInstalled (
  IN       UINTN                 Socket,
  IN       UINT16                CacheHandle[CpuLmaxCache],
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  );

EFI_STATUS
EFIAPI
HygonAddSmbiosType7 (
  IN       UINTN                 Socket,
  OUT   UINT16 (*CacheHandle)[CpuLmaxCache],
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  );

EFI_STATUS
EFIAPI
HygonAddSmbiosType16 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       EFI_SMBIOS_HANDLE     MemoryErrorInfoHandle,
  IN       EFI_SMBIOS_HANDLE    *MemoryArrayHandle,
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  );

EFI_STATUS
EFIAPI
HygonAddSmbiosType17 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       UINT8                 Socket,
  IN       UINT8                 Channel,
  IN       UINT8                 Dimm,
  IN       EFI_SMBIOS_HANDLE     MemoryArrayHandle,
  IN       EFI_SMBIOS_HANDLE     MemoryErrorInfoHandle,
  OUT   EFI_SMBIOS_HANDLE    *MemoryDeviceHandle,
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  );

EFI_STATUS
EFIAPI
HygonAddSmbiosType18 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  OUT   EFI_SMBIOS_HANDLE    *MemoryErrorInfoHandle,
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  );

EFI_STATUS
EFIAPI
HygonAddSmbiosType19 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       EFI_SMBIOS_HANDLE     MemoryArrayHandle,
  IN       EFI_SMBIOS_HANDLE    *ArrayMappedHandle,
  IN       HYGON_CONFIG_PARAMS    *StdHeader,
  IN       UINT8                RegionIndex
  );

// byo231030 - >>
EFI_STATUS
EFIAPI
HygonAddSmbiosType20 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       EFI_SMBIOS_HANDLE     *MemoryArrayHandle
  );
// byo231030 - <<

#pragma pack (pop)
#endif // _UNIVERSAL_SMBIOS_HYGON_SMBIOS_DXE_H_
