/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe ALIB
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */


#include  <GnbDxio.h>
#include  <GnbIommu.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/AmdBaseLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/NbioIommuIvrsLib.h>

#define FILECODE LIBRARY_NBIOIOMMUIVRSLIB_GNBIVRSLIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Create IVHDR entry for device range
 *
 *
 * @param[in]  StartRange      Address of start range
 * @param[in]  EndRange        Address of end range
 * @param[in]  DataSetting     Data setting
 * @param[in]  Ivhd            Pointer to IVHD entry
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
GnbIvhdAddDeviceRangeEntry (
  IN       PCI_ADDR             StartRange,
  IN       PCI_ADDR             EndRange,
  IN       UINT8                DataSetting,
  IN       IVRS_IVHD_ENTRY      *Ivhd,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  IVHD_GENERIC_ENTRY  *Entry;
  Entry = (IVHD_GENERIC_ENTRY *) ((UINT8 *) Ivhd + Ivhd->Length);
  Entry->Type = IvhdEntryStartRange;
  Entry->DeviceId = DEVICE_ID (StartRange);
  Entry->DataSetting = DataSetting;
  Ivhd->Length += sizeof (IVHD_GENERIC_ENTRY);
  Entry = (IVHD_GENERIC_ENTRY *) ((UINT8 *) Ivhd + Ivhd->Length);
  Entry->Type = IvhdEntryEndRange;
  Entry->DeviceId = DEVICE_ID (EndRange);
  Ivhd->Length += sizeof (IVHD_GENERIC_ENTRY);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Create IVHDR entry for aliased range
 *
 *
 * @param[in]  StartRange      Address of start range
 * @param[in]  EndRange        Address of end range
 * @param[in]  Alias           Address of alias requestor ID for range
 * @param[in]  DataSetting     Data setting
 * @param[in]  Ivhd            Pointer to IVHD entry
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
GnbIvhdAddDeviceAliasRangeEntry (
  IN       PCI_ADDR             StartRange,
  IN       PCI_ADDR             EndRange,
  IN       PCI_ADDR             Alias,
  IN       UINT8                DataSetting,
  IN       IVRS_IVHD_ENTRY      *Ivhd,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  IVHD_ALIAS_ENTRY    *RangeEntry;
  IVHD_GENERIC_ENTRY  *Entry;
  UINT16              Offset;
  Offset = (Ivhd->Length + 0x7) & (~ 0x7);
  RangeEntry = (IVHD_ALIAS_ENTRY *) ((UINT8 *) Ivhd + Offset);
  RangeEntry->Type = IvhdEntryAliasStartRange;
  RangeEntry->DeviceId = DEVICE_ID (StartRange);
  RangeEntry->AliasDeviceId = DEVICE_ID (Alias);
  RangeEntry->DataSetting = DataSetting;
  Ivhd->Length = sizeof (IVHD_ALIAS_ENTRY) + Offset;
  Entry = (IVHD_GENERIC_ENTRY *) ((UINT8 *) Ivhd + Ivhd->Length);
  Entry->Type = IvhdEntryEndRange;
  Entry->DeviceId = DEVICE_ID (EndRange);
  Ivhd->Length += sizeof (IVHD_GENERIC_ENTRY);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Create IVHDR entry for special device
 *
 *
 * @param[in]  SpecialDevice   Special device Type
 * @param[in]  Device          Address of requestor ID for special device
 * @param[in]  Id              Apic ID/ Hpet ID
 * @param[in]  DataSetting     Data setting
 * @param[in]  Ivhd            Pointer to IVHD entry
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
GnbIvhdAddSpecialDeviceEntry (
  IN       IVHD_SPECIAL_DEVICE  SpecialDevice,
  IN       PCI_ADDR             Device,
  IN       UINT8                Id,
  IN       UINT8                DataSetting,
  IN       IVRS_IVHD_ENTRY      *Ivhd,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  IVHD_SPECIAL_ENTRY  *SpecialEntry;
  UINT16               Offset;
  Offset = (Ivhd->Length + 0x7) & (~ 0x7);
  SpecialEntry = (IVHD_SPECIAL_ENTRY *) ((UINT8 *) Ivhd + Offset);
  SpecialEntry->Type = IvhdEntrySpecialDevice;
  SpecialEntry->AliasDeviceId = DEVICE_ID (Device);
  SpecialEntry->Variety = (UINT8) SpecialDevice;
  SpecialEntry->Handle = Id;
  SpecialEntry->DataSetting = DataSetting;
  Ivhd->Length = sizeof (IVHD_SPECIAL_ENTRY) + Offset;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Create IVMD entry
 *
 *
 * @param[in]  Type            Root type for IVMD (IvrsIvmdBlock or IvrsIvmdrBlock)
 * @param[in]  StartDevice     Device ID of start device range
 *                             Use 0x0000 for ALL
 * @param[in]  EndDevice       Device ID of end device range
 *                             Use 0xFFFF for ALL
 *                             Use == StartDevice for specific device
 * @param[in]  BlockAddress    Address of memory block to be excluded
 * @param[in]  BlockLength     Length of memory block go be excluded
 * @param[in]  Ivmd            Pointer to IVMD entry
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
GnbIvmdAddEntry (
  IN       IVRS_BLOCK_TYPE      Type,
  IN       UINT16               StartDevice,
  IN       UINT16               EndDevice,
  IN       UINT64               BlockAddress,
  IN       UINT64               BlockLength,
  IN       IVRS_IVMD_ENTRY      *Ivmd,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  Ivmd->Flags = IVMD_FLAG_EXCLUSION_RANGE;
  Ivmd->Length = sizeof (IVRS_IVMD_ENTRY);
  Ivmd->DeviceId = StartDevice;
  Ivmd->AuxiliaryData = 0x0;
  Ivmd->Reserved = 0x0000000000000000;
  Ivmd->BlockStart = BlockAddress;
  Ivmd->BlockLength = BlockLength;
  if (Type == IvrsIvmdBlock) {
    if (StartDevice == EndDevice) {
      Ivmd->Type = IvrsIvmdBlockSingle;
    } else if ((StartDevice == 0x0000) && (EndDevice == 0xFFFF)) {
      Ivmd->Type = IvrsIvmdBlock;
    } else {
      Ivmd->Type = IvrsIvmdBlockRange;
      Ivmd->AuxiliaryData = EndDevice;
    }
  } else {
    if (StartDevice == EndDevice) {
      Ivmd->Type = IvrsIvmdrBlockSingle;
    } else {
      Ivmd->Type = IvrsIvmdrBlock;
    }
  }
}

