/* $NoKeywords:$ */

/**
 * @file
 *
 * Platform PCIe Complex Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioSmuPei
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
#include <PiPei.h>
#include <HygonPcieComplex.h>
#include <Ppi/NbioPcieComplexPpi.h>

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

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

HSIO_PORT_DESCRIPTOR  PortList[] = {
  // GFX - x8 slot
  {
    0,
    HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 8, 15, 0, 0),
    HSIO_PORT_DATA_INITIALIZER_PCIE (
      HsioPortEnabled,                        // Port Present
      3,                                      // Requested Device
      1,                                      // Requested Function
      HsioHotplugDisabled,                    // Hotplug
      HsioGenMaxSupported,                    // Max Link Speed
      HsioGenMaxSupported,                    // Max Link Capability
      HsioAspmL0sL1,                          // ASPM
      0,                                      // ASPM L1.1 disabled
      0,                                      // ASPM L1.2 disabled
      0                                       // Clock PM
      )
  },
  // GPP[1:0] - M2 x2
  {
    DESCRIPTOR_TERMINATE_LIST,
    HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 4, 5, 0, 0),
    HSIO_PORT_DATA_INITIALIZER_PCIE (
      HsioPortEnabled,                        // Port Present
      2,                                      // Requested Device
      5,                                      // Requested Function
      HsioHotplugDisabled,                    // Hotplug
      HsioGenMaxSupported,                    // Max Link Speed
      HsioGenMaxSupported,                    // Max Link Capability
      HsioAspmL0sL1,                          // ASPM
      0,                                      // ASPM L1.1 disabled
      0,                                      // ASPM L1.2 disabled
      0                                       // Clock PM
      )
  }
};

HSIO_COMPLEX_DESCRIPTOR  PcieComplex = {
  DESCRIPTOR_TERMINATE_LIST,
  0,
  &PortList[0],
  NULL,
  NULL
};

/**
* This service will return a pointer to the HSIO_COMPLEX_DESCRIPTOR.
*
*  @param[in]  This        A pointer to the PEI_HYGON_NBIO_PCIE_COMPLEX_PPI instance
*  @param[in]  UserConfig  Pointer HSIO_COMPLEX_DESCRIPTOR pointer
*
*  @retval EFI_SUCCESS     The thread was successfully launched.
*
**/
EFI_STATUS
PcieGetComplex (
  IN     PEI_HYGON_NBIO_PCIE_COMPLEX_PPI  *This,
  OUT    HSIO_COMPLEX_DESCRIPTOR        **UserConfig
  )
{
  *UserConfig = &PcieComplex;
  return EFI_SUCCESS;
}

STATIC PEI_HYGON_NBIO_PCIE_COMPLEX_PPI  mPcieComplexPpi = {
  HYGON_NBIO_PCIE_COMPLEX_REVISION,  ///< revision
  PcieGetComplex
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mPcieComplexDescriptorPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonNbioPcieComplexPpiGuid,
  &mPcieComplexPpi
};

EFI_STATUS
EFIAPI
PcieComplexPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  // Fixup HSIO_COMPLEX_DESCRIPTOR here
  Status = (**PeiServices).InstallPpi (PeiServices, &mPcieComplexDescriptorPpiList);
  return Status;
}
