/* $NoKeywords:$ */
/**
 * @file
 *
 * Platform PCIe Complex Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioSmuV9Pei
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
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
 ******************************************************************************
 */
#include <PiPei.h>
#include <AmdPcieComplexFm15.h>
#include <Ppi/NbioPcieComplexFm15Ppi.h>

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

  PCIe_PORT_DESCRIPTOR  PortList[] = {
    // GFX - x8 slot
    {
      0,
      PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 8, 15),
      PCIE_PORT_DATA_INITIALIZER_V2 (
        PortEnabled,
        ChannelTypeExt6db,
        3,
        1,
        HotplugDisabled,
        PcieGenMaxSupported,
        PcieGenMaxSupported,
        AspmL0sL1,
        1,  // Bristol only uses x8
        0
      )
    },
    // GPP[1:0] - M2 x2
    {
      0,
      PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 4, 5),
      PCIE_PORT_DATA_INITIALIZER_V2 (
        PortEnabled,
        ChannelTypeExt6db,
        2,
        5,
        HotplugDisabled,
        PcieGenMaxSupported,
        PcieGenMaxSupported,
        AspmL0sL1,
        2,
        0
      )
    },

    // UMI - Promontroy x4
    {
      0,
      PCIE_ENGINE_DATA_INITIALIZER (PcieUnusedEngine, 0, 3),
      PCIE_PORT_DATA_INITIALIZER_V2 (
        PortDisabled,
        ChannelTypeExt6db,
        2,
        4,
        HotplugDisabled,
        PcieGenMaxSupported,
        PcieGenMaxSupported,
        AspmL0sL1,
        3,
        0
      )
    },
    // UMI - PCIe x4 slot
    {
      DESCRIPTOR_TERMINATE_LIST,
      PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 0, 3),
      PCIE_PORT_DATA_INITIALIZER_V2 (
        PortEnabled,
        ChannelTypeExt6db,
        2,
        3,
        HotplugDisabled,
        PcieGenMaxSupported,
        PcieGenMaxSupported,
        AspmL0sL1,
        4,
        0
      )
    },
  };

  PCIe_DDI_DESCRIPTOR DdiList[] = {
    // DDI0 - DP
    {
      0,
      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 16, 19),
      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDP, Aux1, Hdp1)
    },
    // DDI1 - HDMI
    {
      0,
      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 20, 23),
      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeHDMI, Aux2, Hdp2)
    },
    // DDI2 - HDMI
    {
      DESCRIPTOR_TERMINATE_LIST,
      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 24, 27),
      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeHDMI, Aux3, Hdp3)
    }
  };

  PCIe_COMPLEX_DESCRIPTOR PcieComplex = {
    DESCRIPTOR_TERMINATE_LIST,
    0,
    &PortList[0],
    &DdiList[0],
    NULL
  };

/**
  This service will start a core to fetch its first instructions from the reset
  vector.  This service may only be called from the BSP.

  @param[in]  This        A pointer to the PEI_AMD_NBIO_PCIE_COMPLEX_PPI instance
  @param[in]  UserConfig  Pointer PCIe_COMPLEX_DESCRIPTOR pointer

  @retval EFI_SUCCESS                        The thread was successfully launched.

**/
EFI_STATUS
PcieGetComplex (
  IN     PEI_AMD_NBIO_PCIE_COMPLEX_FM15_PPI *This,
     OUT PCIe_COMPLEX_DESCRIPTOR            **UserConfig
  )
{
  *UserConfig = &PcieComplex;
  return EFI_SUCCESS;
}

STATIC PEI_AMD_NBIO_PCIE_COMPLEX_FM15_PPI mPcieComplexPpi = {
  AMD_NBIO_PCIE_COMPLEX_FM15_REVISION,  ///< revision
  PcieGetComplex
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPcieComplexDescriptorPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdNbioPcieComplexFm15PpiGuid,
  &mPcieComplexPpi
};

EFI_STATUS
EFIAPI
PcieComplexFm15PeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                       Status;
  // Fixup PCIe_COMPLEX_DESCRIPTOR here
  Status = (**PeiServices).InstallPpi (PeiServices, &mPcieComplexDescriptorPpiList);
  return Status;
}
