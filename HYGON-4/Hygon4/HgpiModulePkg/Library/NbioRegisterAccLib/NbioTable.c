/* $NoKeywords:$ */

/**
 * @file
 *
 * Service procedure to access PCI config space registers
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: GNB
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  <Library/BaseLib.h>
#include  <HGPI.h>
#include  <Library/IdsLib.h>
#include  <Library/HygonBaseLib.h>
#include  <Library/HygonSocBaseLib.h>
// #include  "cpuFamilyTranslation.h"
#include  <Gnb.h>
#include  <GnbRegisters.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/GnbLib.h>
#include  <Library/NbioTable.h>
#include  <Library/NbioRegisterAccLib.h>
#include  <Filecode.h>

#define FILECODE  LIBRARY_NBIOREGISTERACCLIB_NBIOTABLE_FILECODE

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

VOID
STATIC
GnbProcessTableRegisterRmw (
  IN      GNB_HANDLE                      *GnbHandle,
  IN      GNB_RMW_BLOCK                   *Data,
  IN      UINT32                          Flags
  );

/*----------------------------------------------------------------------------------------*/

/**
 * Convert a SMN Register space to other NBIO's SMN register
 *
 * @param[in] GnbHandle       Gnb handle
 * @param[in]  Address        SMN Register Address
 * @retval                    Address after converted
 *
 */
UINT32
ConvertNbioSmnAddress (
  IN      GNB_HANDLE    *GnbHandle,
  IN      UINT32        Address
  )
{
  UINT32  AppertureId;
  UINT32  DstAddress;
  UINT32  HygonCpuModel;
  
  AppertureId = (Address & 0xFFF00000) >> 20;
  HygonCpuModel = GetHygonSocModel();

  if ((Address == NBIO0_PCIE0_STRAP_INDEX_ADDRESS) || (Address == NBIO0_PCIE0_STRAP_DATA_ADDRESS) ||
      (Address == NBIO0_PCIE1_STRAP_INDEX_ADDRESS) || (Address == NBIO0_PCIE1_STRAP_DATA_ADDRESS) ||
      (Address == NBIO0_PCIE2_STRAP_INDEX_ADDRESS) || (Address == NBIO0_PCIE2_STRAP_DATA_ADDRESS)) {
    DstAddress = PCIE_STRAP_SPACE (GnbHandle, Address);
  } else {
    if ((HygonCpuModel == HYGON_EX_CPU) &&
        ((AppertureId == NBIO0_L1IMU_PCIE0_APERTURE_ID_HYEX) || 
         (AppertureId == NBIO0_L1IMU_PCIE1_APERTURE_ID_HYEX) || 
         (AppertureId == NBIO0_L1IMU_NBIF0_APERTURE_ID_HYEX) || 
         (AppertureId == NBIO0_L1IMU_IOAGR_APERTURE_ID_HYEX))) {
      //HyEx L1 IOMMU Space
      DstAddress = L1IMU_SPACE_HYEX(GnbHandle, Address);   
    } else {
      DstAddress = NBIO_SPACE (GnbHandle, Address);
    }
  }

  return DstAddress;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Process table
 *
 * @param[in] GnbHandle       Gnb handle
 * @param[in] Table           Table pointer
 * @param[in] Property        Property
 * @param[in] Flags           Flags
 * @param[in] StdHeader       Standard configuration header
 */
HGPI_STATUS
GnbProcessTable (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GNB_TABLE             *Table,
  IN      UINT32                Property,
  IN      UINT32                Flags,
  IN      HYGON_CONFIG_PARAMS     *StdHeader
  )
{
  UINT8   *EntryPointer;
  UINT64  Data;
  UINT64  Temp;
  UINT64  Mask;
  UINT32  WriteAccFlags;
  UINT32  Address, DstAddress;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbProcessTableExt Enter\n");
  IDS_HDT_CONSOLE (GNB_TRACE, " Property - 0x%08x\n", Property);

  EntryPointer  = (UINT8 *)Table;
  WriteAccFlags = 0;
  if ((Flags & NBIO_TABLE_FLAGS_FORCE_S3_SAVE) != 0) {
    WriteAccFlags |= GNB_REG_ACC_FLAG_S3SAVE;
  }

  while (*EntryPointer != GnbEntryTerminate) {
    Data = 0;
    Temp = 0;
    switch (*EntryPointer) {
      case GnbEntryWr:
        Address = ConvertNbioSmnAddress (GnbHandle, ((GNB_TABLE_ENTRY_WR *)EntryPointer)->Address);
        NbioRegisterWrite (
          GnbHandle,
          ((GNB_TABLE_ENTRY_WR *)EntryPointer)->RegisterSpaceType,
          Address,
          &((GNB_TABLE_ENTRY_WR *)EntryPointer)->Value,
          WriteAccFlags
          );
        EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_WR);
        break;
      case GnbEntryPropertyWr:
        if ((Property & ((GNB_TABLE_ENTRY_PROPERTY_WR *)EntryPointer)->Property) != 0) {
          Address = ConvertNbioSmnAddress (GnbHandle, ((GNB_TABLE_ENTRY_PROPERTY_WR *)EntryPointer)->Address);
          NbioRegisterWrite (
            GnbHandle,
            ((GNB_TABLE_ENTRY_PROPERTY_WR *)EntryPointer)->RegisterSpaceType,
            Address,
            &((GNB_TABLE_ENTRY_PROPERTY_WR *)EntryPointer)->Value,
            WriteAccFlags
            );
        }

        EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_PROPERTY_WR);
        break;
      case GnbEntryFullWr:
        if ((Property & ((GNB_TABLE_ENTRY_FULL_WR *)EntryPointer)->Property) != 0) {
          Address = ConvertNbioSmnAddress (GnbHandle, ((GNB_TABLE_ENTRY_FULL_WR *)EntryPointer)->Address);
          NbioRegisterWrite (
            GnbHandle,
            ((GNB_TABLE_ENTRY_FULL_WR *)EntryPointer)->RegisterSpaceType,
            Address,
            &((GNB_TABLE_ENTRY_FULL_WR *)EntryPointer)->Value,
            WriteAccFlags
            );
        }

        EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_FULL_WR);
        break;
      case GnbEntryRmw:
        GnbProcessTableRegisterRmw (
          GnbHandle,
          &((GNB_TABLE_ENTRY_RMW *)EntryPointer)->Data,
          WriteAccFlags
          );
        EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_RMW);
        break;
      case GnbEntryPropertyRmw:
        if ((Property & ((GNB_TABLE_ENTRY_PROPERTY_RMW *)EntryPointer)->Property) != 0) {
          GnbProcessTableRegisterRmw (
            GnbHandle,
            &((GNB_TABLE_ENTRY_PROPERTY_RMW *)EntryPointer)->Data,
            WriteAccFlags
            );
        }

        EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_PROPERTY_RMW);
        break;
      case GnbEntryRevRmw:
        GnbProcessTableRegisterRmw (
          GnbHandle,
          &((GNB_TABLE_ENTRY_REV_RMW *)EntryPointer)->Data,
          WriteAccFlags
          );

        EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_REV_RMW);
        break;
      case GnbEntryFullRmw:
        if ((Property & ((GNB_TABLE_ENTRY_FULL_RMW *)EntryPointer)->Property) != 0) {
          GnbProcessTableRegisterRmw (
            GnbHandle,
            &((GNB_TABLE_ENTRY_FULL_RMW *)EntryPointer)->Data,
            WriteAccFlags
            );
        }

        EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_FULL_RMW);
        break;
      case GnbEntryPoll:
        Address = ConvertNbioSmnAddress (GnbHandle, ((GNB_TABLE_ENTRY_POLL *)EntryPointer)->Address);
        do {
          NbioRegisterRead (
            GnbHandle,
            ((GNB_TABLE_ENTRY_POLL *)EntryPointer)->RegisterSpaceType,
            Address,
            &Data,
            0
            );
        } while ((Data & ((GNB_TABLE_ENTRY_POLL *)EntryPointer)->AndMask) != ((GNB_TABLE_ENTRY_POLL *)EntryPointer)->CompareValue);

        EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_POLL);
        break;
      case GnbEntryPropertyPoll:
        if ((Property & ((GNB_TABLE_ENTRY_PROPERTY_POLL *)EntryPointer)->Property) != 0) {
          Address = ConvertNbioSmnAddress (GnbHandle, ((GNB_TABLE_ENTRY_PROPERTY_POLL *)EntryPointer)->Address);
          do {
            NbioRegisterRead (
              GnbHandle,
              ((GNB_TABLE_ENTRY_PROPERTY_POLL *)EntryPointer)->RegisterSpaceType,
              Address,
              &Data,
              0
              );
          } while ((Data & ((GNB_TABLE_ENTRY_PROPERTY_POLL *)EntryPointer)->AndMask) != ((GNB_TABLE_ENTRY_PROPERTY_POLL *)EntryPointer)->CompareValue);
        }

        EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_PROPERTY_POLL);
        break;
      case GnbEntryFullPoll:
        if ((Property & ((GNB_TABLE_ENTRY_FULL_POLL *)EntryPointer)->Property) != 0) {
          Address = ConvertNbioSmnAddress (GnbHandle, ((GNB_TABLE_ENTRY_FULL_POLL *)EntryPointer)->Address);
          do {
            NbioRegisterRead (
              GnbHandle,
              ((GNB_TABLE_ENTRY_FULL_POLL *)EntryPointer)->RegisterSpaceType,
              Address,
              &Data,
              0
              );
          } while ((Data & ((GNB_TABLE_ENTRY_FULL_POLL *)EntryPointer)->AndMask) != ((GNB_TABLE_ENTRY_FULL_POLL *)EntryPointer)->CompareValue);
        }

        EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_FULL_POLL);
        break;
      case GnbEntryCopy:
        Address    = ConvertNbioSmnAddress (GnbHandle, ((GNB_TABLE_ENTRY_COPY *)EntryPointer)->SrcAddress);
        DstAddress = ConvertNbioSmnAddress (GnbHandle, ((GNB_TABLE_ENTRY_COPY *)EntryPointer)->DestAddress);
        NbioRegisterRead (
          GnbHandle,
          ((GNB_TABLE_ENTRY_COPY *)EntryPointer)->SrcRegisterSpaceType,
          Address,
          &Data,
          0
          );
        Mask = LShiftU64 (1ull, ((GNB_TABLE_ENTRY_COPY *)EntryPointer)->SrcFieldWidth) - 1;
        Data = RShiftU64 (Data, ((GNB_TABLE_ENTRY_COPY *)EntryPointer)->SrcFieldOffset) & Mask;
        NbioRegisterRead (
          GnbHandle,
          ((GNB_TABLE_ENTRY_COPY *)EntryPointer)->DestRegisterSpaceType,
          DstAddress,
          &Temp,
          0
          );
        Mask = LShiftU64 (1ull, ((GNB_TABLE_ENTRY_COPY *)EntryPointer)->DestFieldWidth) - 1;
        Temp = Temp & (~LShiftU64 (Mask, ((GNB_TABLE_ENTRY_COPY *)EntryPointer)->DestFieldOffset));
        Temp = Temp | LShiftU64 ((Data & Mask), ((GNB_TABLE_ENTRY_COPY *)EntryPointer)->DestFieldOffset);
        NbioRegisterWrite (
          GnbHandle,
          ((GNB_TABLE_ENTRY_COPY *)EntryPointer)->DestRegisterSpaceType,
          DstAddress,
          &Temp,
          WriteAccFlags
          );
        EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_COPY);
        break;
      default:
        ASSERT (FALSE);
        IDS_HDT_CONSOLE (NB_MISC, "  ERROR!!! Register table parse\n");
        return HGPI_ERROR;
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbProcessTableExt Exit\n");
  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Supporting function for register read modify write
 *
 * @param[in] GnbHandle                 Gnb handle
 * @param[in] Data                      Data pointer
 * @param[in] Flags                     Flags
 */
VOID
STATIC
GnbProcessTableRegisterRmw (
  IN      GNB_HANDLE                     *GnbHandle,
  IN      GNB_RMW_BLOCK                  *Data,
  IN      UINT32                         Flags
  )
{
  UINT64  Value;
  UINT32  Address;

  Value = 0;

  Address = ConvertNbioSmnAddress (GnbHandle, Data->Address);

  NbioRegisterRead (
    GnbHandle,
    Data->RegisterSpaceType,
    Address,
    &Value,
    0
    );
  Value = (Value & (~(UINT64)Data->AndMask)) | Data->OrMask;
  NbioRegisterWrite (
    GnbHandle,
    Data->RegisterSpaceType,
    Address,
    &Value,
    Flags
    );
}
