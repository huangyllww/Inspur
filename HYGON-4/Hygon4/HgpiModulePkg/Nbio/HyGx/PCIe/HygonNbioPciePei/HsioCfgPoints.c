/* $NoKeywords:$ */
/**
 * @file
 *
 * HsioCfgPoints - Configuration entry points for the HSIO subsystem
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioBasePei
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
#include <HYGON.h>
#include <Gnb.h>
#include <Filecode.h>
#include <GnbHsio.h>
#include <GnbRegisters.h>
#include <Library/IdsLib.h>
#include <Library/PcdLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/PcieConfigLib.h>


#define FILECODE  NBIO_PCIE_HYGX_HYGONNBIOPCIESTPEI_HSIOCFGPOINTSST_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

// Comment this line to enable extended debug output if tracing is enabled
#undef GNB_TRACE_ENABLE
#define HSIO_REG_CFG_TABLE_END 0xFFFFFFFF
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  UINT32                  SmnAddress;
  UINT32                  AndMask;
  UINT32                  OrMask;
} HSIO_REG_CFG_INFO;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */


HSIO_REG_CFG_INFO HsioCfgTableAfterReconfig[] = {
  {HSIO_REG_CFG_TABLE_END,  0, 0}
};

/*----------------------------------------------------------------------------------------*/
/**
 * Convert Hsio CfgReg Address
 *
 * @param[in]     GnbHandle  GNB Handle
 * @param[in]     Engine     Point to PCIe engine
 * @param[in]     OldAddress Old Address
 * @param[out]    NewAddress New Address
 
 */
EFI_STATUS
ConvertHsioCfgRegAddress (
  IN  GNB_HANDLE            *GnbHandle,
  IN  PCIe_ENGINE_CONFIG    *Engine,
  IN  UINT32                OldAddress,
  OUT UINT32                *NewAddress
  )
{
  UINT32                    ApertureId;
  PCIe_WRAPPER_CONFIG       *PcieWrapper;
  UINT8                     MappingPortID;
 
  ApertureId = (UINT32)((OldAddress & 0xFF00000) >> 20);
 
  if (ApertureId == NBIO0_PCIE0_APERTURE_ID_HYGX) {
    PcieWrapper = PcieConfigGetParentWrapper (Engine);
    MappingPortID = (Engine->Type.Port.PortId) % 8;
    *NewAddress = ConvertPciePortAddress (OldAddress, GnbHandle, PcieWrapper, MappingPortID);
    return EFI_SUCCESS;
  } else if (ApertureId == NBIO0_IOHC_APERTURE_ID_HYGX) {
    *NewAddress =  NBIO_SPACE (GnbHandle, OldAddress);
    return EFI_SUCCESS;
  }
  
  return EFI_UNSUPPORTED;
}
/*----------------------------------------------------------------------------------------*/

/**
 * Hsio Engine Register Configuration
 *
 * @param[in]     GnbHandle  GNB Handle
 */
VOID
HsioGnbHandleRegCfg (
  IN  GNB_HANDLE               *GnbHandle
  )
{
  UINT32        Data32;
  UINT32        Address;

  //PCIe CRS Delay
  Data32 = PcdGet16 (PcdCfgPcieCrsDelay);
  if (Data32 != 0xff) {
    Address = NBIO_SPACE (GnbHandle, IOHC_PCIE_CRS_Count_ADDRESS_HYGX);
    NbioRegisterRMW (
      GnbHandle, 
      TYPE_SMN, 
      Address, 
      (UINT32)~(IOHC_PCIE_CRS_Count_CrsDelayCount_MASK), 
      (UINT32)(Data32 << IOHC_PCIE_CRS_Count_CrsDelayCount_OFFSET), 
      0
      ); 
  }

  //PCIe CRS Limit
  Data32 = PcdGet16 (PcdCfgPcieCrsLimit);
  if (Data32 != 0xff) {
    Address = NBIO_SPACE (GnbHandle, IOHC_PCIE_CRS_Count_ADDRESS_HYGX);
    NbioRegisterRMW (
      GnbHandle, 
      TYPE_SMN, 
      Address, 
      (UINT32)~(IOHC_PCIE_CRS_Count_CrsLimitCount_MASK), 
      (UINT32)(Data32 << IOHC_PCIE_CRS_Count_CrsLimitCount_OFFSET), 
      0
      ); 
  }
}
/*----------------------------------------------------------------------------------------*/

/**
 * Hsio Engine Register Configuration
 *
 * @param[in]     GnbHandle  GNB Handle
 * @param[in]     Engine     Point to PCIe engine
 */
VOID
HsioEngineRegCfg (
  IN  GNB_HANDLE               *GnbHandle,
  IN  PCIe_ENGINE_CONFIG       *Engine  
  )
{
  UINT32        Data32;
  UINT32        Address;

  //SPC Mode 2.5GT
  Data32 = PcdGet8 (PcdHygonNbioSpcMode2P5GT);
  if (Data32 != 0xff) {
    Address = ConvertPciePortAddress2 (PCIE0_GPP0_LC_CNTL6_ADDRESS_HYGX, GnbHandle, Engine);
    NbioRegisterRMW (
      GnbHandle, 
      TYPE_SMN, 
      Address, 
      (UINT32)~(PCIE_LC_CNTL6_LC_SPC_MODE_2P5GT_MASK), 
      (UINT32)(Data32 << PCIE_LC_CNTL6_LC_SPC_MODE_2P5GT_OFFSET), 
      0
      ); 
  }

  //SPC Mode 5GT
  Data32 = PcdGet8 (PcdHygonNbioSpcMode5GT);
  if (Data32 != 0xff) {
    Address = ConvertPciePortAddress2 (PCIE0_GPP0_LC_CNTL6_ADDRESS_HYGX, GnbHandle, Engine);
    NbioRegisterRMW (
      GnbHandle, 
      TYPE_SMN, 
      Address, 
      (UINT32)~(PCIE_LC_CNTL6_LC_SPC_MODE_5GT_MASK), 
      (UINT32)(Data32 << PCIE_LC_CNTL6_LC_SPC_MODE_5GT_OFFSET), 
      0
      ); 
  }

  //SPC Mode 8GT
  Data32 = PcdGet8 (PcdHygonNbioSpcMode8GT);
  if (Data32 != 0xff) {
    Address = ConvertPciePortAddress2 (PCIE0_GPP0_LC_CNTL6_ADDRESS_HYGX, GnbHandle, Engine);
    NbioRegisterRMW (
      GnbHandle, 
      TYPE_SMN, 
      Address, 
      (UINT32)~(PCIE_LC_CNTL6_LC_SPC_MODE_8GT_MASK), 
      (UINT32)(Data32 << PCIE_LC_CNTL6_LC_SPC_MODE_8GT_OFFSET), 
      0
      ); 
  }

  //SPC Mode 16GT
  Data32 = PcdGet8 (PcdHygonNbioSpcMode16GT);
  if (Data32 != 0xff) {
    Address = ConvertPciePortAddress2 (PCIE0_PCIE_LC_CNTL8_ADDRESS_HYGX, GnbHandle, Engine);
    NbioRegisterRMW (
      GnbHandle, 
      TYPE_SMN, 
      Address, 
      (UINT32)~(LC_SPC_MODE_16GT_MASK), 
      (UINT32)(Data32 << LC_SPC_MODE_16GT_OFFSET), 
      0
      ); 
  }

  //SPC Mode 32GT
  Data32 = PcdGet8 (PcdHygonNbioSpcMode32GT);
  if (Data32 != 0xff) {
    Address = ConvertPciePortAddress2 (PCIE0_PCIE_LC_CNTL8_ADDRESS_HYGX, GnbHandle, Engine);
    NbioRegisterRMW (
      GnbHandle, 
      TYPE_SMN, 
      Address, 
      (UINT32)~(LC_SPC_MODE_32GT_MASK), 
      (UINT32)(Data32 << LC_SPC_MODE_32GT_OFFSET), 
      0
      ); 
  }

  //PCIe 32GT bypass EQ mode
  Data32 = PcdGet8 (PcdPcie32GTBypassEQMode);
  if (Data32 != 0xff) {
    Address = ConvertPciePortAddress2 (PCIE0_LINK_CNTL_32GT_ADDRESS_HYGX, GnbHandle, Engine);
    switch (Data32) {
      case 0:
        //Full EQ mode
        NbioRegisterRMW (
          GnbHandle, 
          TYPE_SMN, 
          Address, 
          (UINT32)~(EQUALIZATION_BYPASS_TO_HIGHEST_RATE_DISABLE_MASK | NO_EQUALIZATION_NEEDED_DISABLE_MASK), 
          (UINT32)((1 << EQUALIZATION_BYPASS_TO_HIGHEST_RATE_DISABLE_OFFSET) | (1 << NO_EQUALIZATION_NEEDED_DISABLE_OFFSET)), 
          0
          ); 
        break;
      case 1:
        //Bypass EQ mode
        NbioRegisterRMW (
          GnbHandle, 
          TYPE_SMN, 
          Address, 
          (UINT32)~(EQUALIZATION_BYPASS_TO_HIGHEST_RATE_DISABLE_MASK | NO_EQUALIZATION_NEEDED_DISABLE_MASK), 
          (UINT32)((0 << EQUALIZATION_BYPASS_TO_HIGHEST_RATE_DISABLE_OFFSET) | (1 << NO_EQUALIZATION_NEEDED_DISABLE_OFFSET)), 
          0
          ); 
        break;
      case 2:
        //No needed EQ mode
        NbioRegisterRMW (
          GnbHandle, 
          TYPE_SMN, 
          Address, 
          (UINT32)~(EQUALIZATION_BYPASS_TO_HIGHEST_RATE_DISABLE_MASK | NO_EQUALIZATION_NEEDED_DISABLE_MASK), 
          (UINT32)((0 << EQUALIZATION_BYPASS_TO_HIGHEST_RATE_DISABLE_OFFSET) | (0 << NO_EQUALIZATION_NEEDED_DISABLE_OFFSET)), 
          0
          ); 
        break;
    }
  }

  //PCIE 32GT pre-coding setting
  Data32 = PcdGet8 (PcdPcie32GTPrecoding);
  if (Data32 != 0xff) {
    Address = ConvertPciePortAddress2 (PCIE0_PCIE_LC_CNTL12_ADDRESS_HYGX, GnbHandle, Engine);
    NbioRegisterRMW (
      GnbHandle, 
      TYPE_SMN, 
      Address, 
      (UINT32)~(LC_TRANSMITTER_PRECODE_REQUEST_32GT_MASK), 
      (UINT32)(Data32 << LC_TRANSMITTER_PRECODE_REQUEST_32GT_OFFSET), 
      0
      ); 
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Hsio Configuration Program After Reconfig
 *
 * @param[in]     GnbHandle  Point to GnbHandle
 */
VOID
HsioConfigurationProgramAfterReconfig (
  IN  GNB_HANDLE            *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG   *PcieEngine;
  PCIe_WRAPPER_CONFIG  *PcieWrapper;
  UINT32               i;
  UINT32               ConvertedAddress;
  EFI_STATUS           Status;

  HsioGnbHandleRegCfg (GnbHandle);
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      HsioEngineRegCfg (GnbHandle, PcieEngine);
      for (i = 0; HsioCfgTableAfterReconfig[i].SmnAddress != HSIO_REG_CFG_TABLE_END; i++) {

        Status = ConvertHsioCfgRegAddress (
                  GnbHandle, 
                  PcieEngine,
                  HsioCfgTableAfterReconfig[i].SmnAddress,
                  &ConvertedAddress
                  );
        if (EFI_ERROR (Status)) {
          continue;
        }

        NbioRegisterRMW (
          GnbHandle,
          TYPE_SMN,
          ConvertedAddress,
          HsioCfgTableAfterReconfig[i].AndMask,
          HsioCfgTableAfterReconfig[i].OrMask,
          0
          ); 
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
}