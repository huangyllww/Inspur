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
#include  "Porting.h"
#include  "HYGON.h"
#include  "Filecode.h"
#include  <Library/HygonBaseLib.h>
#include  <Library/IdsLib.h>
#include  <Library/GnbPciAccLib.h>
#include  <Library/HygonS3SaveLib.h>
#include  <Library/PciLib.h>

#define FILECODE  LIBRARY_GNBPCIACCLIB_GNBPCIACCLIB_FILECODE

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
 * Write PCI registers
 *
 *
 *
 * @param[in] Address         PCI address (as presented in PCI_ADDR.AddressValue)
 * @param[in] Width           Access width
 * @param[in] Value           Pointer to value
 * @param[in] StdHeader       Pointer to standard header
 */
VOID
GnbLibPciWrite (
  IN      UINT32              Address,
  IN      ACCESS_WIDTH        Width,
  IN      VOID                *Value,
  IN      HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  if ((Width >= AccessS3SaveWidth8) && (PcdGetBool (PcdHygonAcpiS3Support))) {
    HygonS3SaveScriptPciWrite (Width - AccessS3SaveWidth8 + 1, Address, Value);
    IDS_HDT_CONSOLE (NB_MISC, "  R S3 SAVE Script: Address 0x%08x, Width 0x%08x ", Address, Width);
    switch (Width) {
      case AccessS3SaveWidth8:
        IDS_HDT_CONSOLE (NB_MISC, "Value=%X\n", *((UINT8 *)Value));
        break;

      case AccessS3SaveWidth16:
        IDS_HDT_CONSOLE (NB_MISC, "Value=%X\n", *((UINT16 *)Value));
        break;

      case AccessS3SaveWidth32:
        IDS_HDT_CONSOLE (NB_MISC, "Value=%X\n", *((UINT32 *)Value));
        break;

      default:
        IDS_HDT_CONSOLE (NB_MISC, "Value=%lX\n", *((UINT64 *)Value));
        break;
    }
  }

  switch (Width) {
    case AccessWidth8:
    case AccessS3SaveWidth8:
      PciWrite8 (Address, *((UINT8 *)Value));
      break;
    case AccessWidth16:
    case AccessS3SaveWidth16:
      PciWrite16 (Address, *((UINT16 *)Value));
      break;
    case AccessWidth32:
    case AccessS3SaveWidth32:
      PciWrite32 (Address, *((UINT32 *)Value));
      break;
    default:
      IDS_ERROR_TRAP;
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Read PCI registers
 *
 *
 *
 * @param[in]   Address         PCI address (as presented in PCI_ADDR.AddressValue)
 * @param[in]   Width           Access width
 * @param[out]  Value           Pointer to value
 * @param[in]   StdHeader       Pointer to standard header
 */
VOID
GnbLibPciRead (
  IN       UINT32             Address,
  IN       ACCESS_WIDTH       Width,
  OUT      VOID               *Value,
  IN       HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  switch (Width) {
    case AccessWidth8:
    case AccessS3SaveWidth8:
      *((UINT8 *)Value) = PciRead8 (Address);
      break;
    case AccessWidth16:
    case AccessS3SaveWidth16:
      *((UINT16 *)Value) = PciRead16 (Address);
      break;
    case AccessWidth32:
    case AccessS3SaveWidth32:
      *((UINT32 *)Value) = PciRead32 (Address);
      break;
    default:
      IDS_ERROR_TRAP;
  }
}
