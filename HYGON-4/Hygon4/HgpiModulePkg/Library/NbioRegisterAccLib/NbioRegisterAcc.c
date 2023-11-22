/* $NoKeywords:$ */

/**
 * @file
 *
 * NBIO Register Access Methods.
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
#include  "Gnb.h"
#include  "NbioRegisterTypes.h"
#include  <Library/HygonBaseLib.h>
#include  <Library/IdsLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/NbioRegisterAccLib.h>
#include  <Library/NbioHandleLib.h>
#include  <Library/SmnAccessLib.h>
#include  <Filecode.h>
#define FILECODE  LIBRARY_NBIOREGISTERACCLIB_NBIOREGISTERACC_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#undef GNB_TRACE_ENABLE
#define GNB_IGNORED_PARAM      0xFF
#define ORB_WRITE_ENABLE       0       // 0x100
#define IOMMU_L1_WRITE_ENABLE  0x80000000ul
#define IOMMU_L2_WRITE_ENABLE  0x100

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
NbioRegisterWriteDump (
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       UINT32              Value
  );

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to read all register spaces.
 *
 *
 *
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset, but PortDevice
 * @param[out] Value             Return value
 * @param[in] Flags              Flags - BIT0 indicates S3 save/restore
 * @retval    HGPI_STATUS
 */
HGPI_STATUS
NbioRegisterRead (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  OUT   VOID                *Value,
  IN       UINT32              Flags
  )
{
  ACCESS_WIDTH  Width;
  PCI_ADDR      GnbPciAddress;

  // UINT64        GmmBase;

  GnbPciAddress = NbioGetHostPciAddress (GnbHandle);
  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;

  GNB_DEBUG_CODE (
    IDS_HDT_CONSOLE (
      NB_MISC,
      "  R READ Space 0x%x  Address 0x%04x\n",
      RegisterSpaceType,
      Address
      );
    )

  switch (RegisterSpaceType) {
    case TYPE_PCI:
      GnbLibPciRead (
        GnbPciAddress.AddressValue | Address,
        Width,
        Value,
        NULL
        );
      break;

    case TYPE_SMN:
      SmnRegisterRead (
        GnbPciAddress.Address.Bus,
        Address,
        Value
        );
      break;

    default:
      ASSERT (FALSE);
      break;
  }

  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to write all register spaces.
 *
 *
 * @param[in] GnbHandle          GnbHandle
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset, but PortDevice
 * @param[out] Value             The value to write
 * @param[in] Flags              Flags - BIT0 indicates S3 save/restore
 * @retval    HGPI_STATUS
 */
HGPI_STATUS
NbioRegisterWrite (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  )
{
  ACCESS_WIDTH  Width;
  PCI_ADDR      GnbPciAddress;

  // UINT64          GmmBase;

  GnbPciAddress = NbioGetHostPciAddress (GnbHandle);
  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;

  GNB_DEBUG_CODE (
    NbioRegisterWriteDump (RegisterSpaceType, Address, *((UINT32 *)Value));
    )

  switch (RegisterSpaceType) {
    case TYPE_PCI:
      GnbLibPciWrite (
        GnbPciAddress.AddressValue | Address,
        Width,
        Value,
        NULL
        );
      break;

    case TYPE_SMN:
      SmnRegisterWrite (
        GnbPciAddress.Address.Bus,
        Address,
        Value,
        Flags
        );
      break;

    default:
      ASSERT (FALSE);
      break;
  }

  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to Read/Modify/Write all register spaces.
 *
 *
 *
 * @param[in] GnbHandle          GnbHandle
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset, but PortDevice
 * @param[in] Mask               And Mask
 * @param[in] Value              Or value
 * @param[in] Flags              Flags - BIT0 indicates S3 save/restore
 * @retval    VOID
 */
VOID
NbioRegisterRMW (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       UINT32              Mask,
  IN       UINT32              Value,
  IN       UINT32              Flags
  )
{
  UINT32  Data;

  NbioRegisterRead (GnbHandle, RegisterSpaceType, Address, &Data, Flags);
  Data = (Data & Mask) | Value;
  NbioRegisterWrite (GnbHandle, RegisterSpaceType, Address, &Data, Flags);
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to dump all write register spaces.
 *
 *
 *
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset
 * @param[in] Value              The value to write
 */
VOID
NbioRegisterWriteDump (
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       UINT32              Value
  )
{
  IDS_HDT_CONSOLE (
    NB_MISC,
    "  R WRITE Space %a  Address 0x%04x, Value 0x%04x\n",
    (RegisterSpaceType == TYPE_PCI) ? "TYPE_PCI" : (
    (RegisterSpaceType == TYPE_SMN) ? "TYPE_SMN" : (
    (RegisterSpaceType == TYPE_D0F0xBC) ? "TYPE_D0F0xBC" : "Invalid")),
    Address,
    Value
    );
}
