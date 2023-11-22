/* $NoKeywords:$ */

/**
 * @file
 *
 * GNB function to create/locate PCIe configuration data area
 *
 * Contain code that create/locate/manages GNB/PCIe configuration
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
#include  <GnbHsio.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/HygonBaseLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/NbioHandleLib.h>

#define FILECODE  LIBRARY_NBIOHANDLELIB_NBIOHANDLELIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/

/**
 * Get child descriptor of specific type
 *
 *
 * @param[in]       Type            Descriptor type
 * @param[in]       Descriptor      Pointer to buffer to pass information to callback
 */
STATIC
PCIe_DESCRIPTOR_HEADER *
PcieConfigGetChild (
  IN       UINT32                        Type,
  IN       PCIe_DESCRIPTOR_HEADER        *Descriptor
  )
{
  while ((Descriptor->DescriptorFlags & Type) == 0) {
    if (Descriptor->Child != 0) {
      Descriptor = (PCIe_DESCRIPTOR_HEADER *)((UINT8 *)Descriptor + Descriptor->Child);
    } else {
      return NULL;
    }
  }

  return Descriptor;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get GNB handle
 *
 *
 * @param[in]       Pcie           Pointer to global PCIe configuration
 */
GNB_HANDLE *
NbioGetHandle (
  IN PCIe_PLATFORM_CONFIG            *Pcie
  )
{
  GNB_HANDLE  *NbioHandle;

  NbioHandle = (GNB_HANDLE *)PcieConfigGetChild (DESCRIPTOR_SILICON, &Pcie->Header);
  return NbioHandle;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get next socket die 0 NBIO 0 GnbHandle
 *
 *
 * @param[in]       GnbHandle           Pointer to a GnbHandle
 */
GNB_HANDLE *
GnbGetNextSocketHandle (
  IN GNB_HANDLE         *GnbHandle
  )
{
  GNB_HANDLE  *NbioHandle;

  NbioHandle = GnbHandle;

  while (NbioHandle != NULL) {
    NbioHandle = GnbGetNextHandle (NbioHandle);
    if (NbioHandle == NULL) {
      break;
    } else {
      // Found out each socket's DJ0 NBIO Handle
      if ((NbioHandle->PhysicalDieId == 0) && (NbioHandle->RbId == 0)) {
        break;
      }
    }
  }

  return NbioHandle;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get next IOD NBIO 0 GnbHandle
 *
 *
 * @param[in]       GnbHandle           Pointer to a GnbHandle
 */
GNB_HANDLE *
GnbGetNextIodHandle (
  IN GNB_HANDLE         *GnbHandle
  )
{
  GNB_HANDLE  *NbioHandle;

  NbioHandle = GnbHandle;

  while (NbioHandle != NULL) {
    NbioHandle = GnbGetNextHandle (NbioHandle);
    if (NbioHandle == NULL) {
      break;
    } else {
      // Found out each IOD NBIO0 Handle
      if (NbioHandle->RbId == 0) {
        break;
      }
    }
  }

  return NbioHandle;
}

/*----------------------------------------------------------------------------------------*/

/*
 * Get PCI_ADDR of GNB
 *
 *
 * @param[in]  Handle           Pointer to GNB_HANDLE
 */
PCI_ADDR
NbioGetHostPciAddress (
  IN      GNB_HANDLE          *Handle
  )
{
  ASSERT (Handle != NULL);
  return Handle->Address;
}
