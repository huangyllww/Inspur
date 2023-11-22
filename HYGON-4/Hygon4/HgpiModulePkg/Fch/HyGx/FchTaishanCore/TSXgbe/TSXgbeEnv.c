/* $NoKeywords:$ */

/**
 * @file
 *
 * Config Fch xGbE controller
 *
 * Init USB features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: FCH
 *
 */
/*;********************************************************************************
;
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
;*********************************************************************************/
#include "FchPlatform.h"
#include "FchCommonCfg.h"
#include "Filecode.h"
#include <Library/FabricResourceManagerLib.h>
#define FILECODE  PROC_FCH_TAISHAN_TSXGBE_TSXGBEENV_FILECODE

/**
 * FchInitEnvXGbe - Config xGbE controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvXGbe (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;
  UINT8                Port;
  UINT32               SystemPort;
  UINT32               XgbeNbifStrap0;
  FCH_XGBE_PORT        *FchXgbePort;
  FCH_XGBE_MAC         *FchXgbePortMac;
  UINT32               Value;
  IOHC_FASTREG_APERTURE_STRUCT            FastRegAperture;
  IOHC_FASTREG_BASE_ADDR_LO_STRUCT        FastRegBaseLow;
  UINT64                                  FastRegMmioBase;
  UINT64                                  Length;
  FABRIC_TARGET                           MmioTarget;
  FABRIC_MMIO_ATTRIBUTE                   Attributes;
  EFI_STATUS                              Status;
  NBIF_FUN4_STRAP6_STRUCT                 XgbeStrap6;
  NBIF_FUN4_STRAP2_STRUCT                 XgbeStrap2;
  NBIF_FUN4_STRAP1_STRUCT                 XgbeStrap1;
  RCC_DEV0_EPFx_STRAP4_STRUCT             XgbeStrap4;

  if (PcdGet8 (PcdRunEnvironment) >= 2) {
    return;
  }
  IDS_HDT_CONSOLE (FCH_TRACE, "FchInitEnvXGbe start \n");
  
  FchSmnRead (0, NBIO_SPACE2 (0, 0, NBIF2_XGBE0_STRAP0_ADDRESS_HYGX), &XgbeNbifStrap0, NULL);
  if (XgbeNbifStrap0 & BIT28) {
    IDS_HDT_CONSOLE (FCH_TRACE, " DJ0 XGBE0 Enabled \n");
 
    IDS_HDT_CONSOLE (FCH_TRACE, "Enable XGBE0 prefetchable_en \n");
    FchSmnRead (0, NBIO_SPACE2(0, 0, NBIF2_FUNC4_STRAP6_ADDRESS_HYGX), &XgbeStrap6.Value, NULL);
    XgbeStrap6.Field.STRAP_APER0_PREFETCHABLE_EN_DEV0_F4 =1;
    XgbeStrap6.Field.STRAP_APER2_PREFETCHABLE_EN_DEV0_F4 =1;
    FchSmnWrite (0, NBIO_SPACE2(0, 0, NBIF2_FUNC4_STRAP6_ADDRESS_HYGX), &XgbeStrap6.Value, NULL);

    IDS_HDT_CONSOLE (FCH_TRACE, "Set XGBE0 VF device ID to 0x14AA \n");
    FchSmnRead (0, NBIO_SPACE2(0, 0, NBIF2_RCC_DEV0_EPF4_STRAP1_ADDRESS_HYGX), &XgbeStrap1.Value, NULL);
    XgbeStrap1.Field.STRAP_SRIOV_VF_DEVICE_ID_DEV0_F4 =0x14AA;
    FchSmnWrite (0, NBIO_SPACE2(0, 0, NBIF2_RCC_DEV0_EPF4_STRAP1_ADDRESS_HYGX), &XgbeStrap1.Value, NULL);

    IDS_HDT_CONSOLE (FCH_TRACE, "Enable XGBE0 ARI CAP \n");
    FchSmnRead (0, NBIO_SPACE2(0, 0, NBIF2_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYGX), &Value, NULL);
    Value |= BIT16;
    FchSmnWrite (0, NBIO_SPACE2(0, 0, NBIF2_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYGX), &Value, NULL);

    IDS_HDT_CONSOLE (FCH_TRACE, "Enable XGBE0 SRIOV CAP \n");
    FchSmnRead (0, NBIO_SPACE2(0, 0, NBIF2_FUNC4_STRAP2_ADDRESS_HYGX), &XgbeStrap2.Value, NULL);
    XgbeStrap2.Field.STRAP_SRIOV_EN_DEV0_F4 =1;
    FchSmnWrite (0, NBIO_SPACE2(0, 0, NBIF2_FUNC4_STRAP2_ADDRESS_HYGX), &XgbeStrap2.Value, NULL);

    IDS_HDT_CONSOLE (FCH_TRACE, "Enable XGBE0 FLR CAP \n");
    FchSmnRead (0, NBIO_SPACE2(0, 0, NBIF2_RCC_DEV0_EPF4_STRAP4_ADDRESS_HYGX), &XgbeStrap4.Value, NULL);
    XgbeStrap4.Field.STRAP_FLR_EN_DEV0_F4 =1;
    FchSmnWrite (0, NBIO_SPACE2(0, 0, NBIF2_RCC_DEV0_EPF4_STRAP4_ADDRESS_HYGX), &XgbeStrap4.Value, NULL);

    IDS_HDT_CONSOLE (FCH_TRACE, "Enable NBIF2 RSMU VF Enable \n");
    FchSmnRead (0, NBIO_SPACE2(0, 0, RSMU_VF_ENABLE_NBIF2_ADDRESS_HYGX), &Value, NULL);
    Value |= BIT0;
    FchSmnWrite (0, NBIO_SPACE2(0, 0, RSMU_VF_ENABLE_NBIF2_ADDRESS_HYGX), &Value, NULL);

    //Allocate 1MB MMIO For XGBE Bar
    Length = 0x100000;
    MmioTarget.TgtType = TARGET_RB;
    MmioTarget.SocketNum = 0;
    MmioTarget.DieNum = 0;
    MmioTarget.RbNum = 0;
    Attributes.ReadEnable = 1;
    Attributes.WriteEnable = 1;
    Attributes.NonPosted = 0;
    Attributes.MmioType = NON_PCI_DEVICE_BELOW_4G;
                                                                                    
    Status = FabricAllocateMmio (&FastRegMmioBase, &Length, ALIGN_1M, MmioTarget, &Attributes);
    if (Status == EFI_SUCCESS) {
      IDS_HDT_CONSOLE (FCH_TRACE, "Allocate FastRegMmioBase = 0x%X \n",FastRegMmioBase);
      FchSmnRead (0, NBIO_SPACE2(0, 0, IOHC_FASTREG_APERTURE_ADDRESS_HYGX), &FastRegAperture.Value, NULL);
      FastRegAperture.Field.FASTREG_NODE_ID = 0;
      FastRegAperture.Field.FASTREG_APERTURE_ID = 0x1E6;
      FchSmnWrite (0, NBIO_SPACE2(0, 0, IOHC_FASTREG_APERTURE_ADDRESS_HYGX), &FastRegAperture.Value, NULL);

      Value = 0;
      FchSmnWrite (0, NBIO_SPACE2(0, 0, IOHC_FASTREG_BASE_ADDR_HI_ADDRESS_HYGX), &Value, NULL);

      FchSmnRead (0, NBIO_SPACE2(0, 0, IOHC_FASTREG_BASE_ADDR_LO_ADDRESS_HYGX), &FastRegBaseLow.Value, NULL);
      FastRegBaseLow.Field.FASTREG_BASE_ADDR_LO = (UINT32) FastRegMmioBase >>20;
      FastRegBaseLow.Field.FASTREG_MMIO_EN =1;
      FchSmnWrite (0, NBIO_SPACE2(0, 0, IOHC_FASTREG_BASE_ADDR_LO_ADDRESS_HYGX), &FastRegBaseLow.Value, NULL);

      Value = (UINT32)FastRegMmioBase;
      FchSmnWrite (0, NBIO_SPACE2(0, 0, NBIF2_EPF4_PCIE_VENDOR_SPECIFIC1_HYGX), &Value, NULL);

      Value = *(UINT32 *)(FastRegMmioBase + 0x1E000);
      IDS_HDT_CONSOLE (FCH_TRACE, " FastReg Offset 0x1E000 = 0x%X \n",Value);
      
      FchSmnRead (0, NBIO_SPACE2(0, 0, NBIF2_EPF4_PCIE_VENDOR_SPECIFIC1_HYGX), &Value, NULL);
      IDS_HDT_CONSOLE (FCH_TRACE, " log FastReg MMIO Address = 0x%X \n", Value);
    }

    /*config DMAtoAXI register */
    Value = 0xbf3f0180;
    FchSmnWrite (0, NBIO_SPACE2(0, 0, FCH_SMN_MMDMA2AXI_0_HYGX), &Value, NULL);  
    Value = 0x222;
    FchSmnWrite (0, NBIO_SPACE2(0, 0, FCH_SMN_MMDMA2AXI_1_HYGX), &Value, NULL);  
    FchSmnWrite (0, NBIO_SPACE2(0, 0, FCH_SMN_MMDMA2AXI_2_HYGX), &Value, NULL);

  }
  LocalCfgPtr    = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader      = LocalCfgPtr->StdHeader;
  FchXgbePort    = &LocalCfgPtr->Xgbe.Port[0];
  FchXgbePortMac = &LocalCfgPtr->Xgbe.Mac[0];

  HGPI_TESTPOINT (TpFchInitResetXgbe, NULL);
  for (Port = 0; Port < 4; Port++) {
    //
    // Check if GMAC is enabled
    //
    FchSmnRead (0, NBIO_SPACE2 (0, 0, NBIF2_XGBE0_STRAP0_ADDRESS_HYGX+ Port * 0x200), &XgbeNbifStrap0, NULL);

    if (XgbeNbifStrap0 & BIT28) {
      SystemPort = LocalCfgPtr->Xgbe.MaxPortNum;
      LocalCfgPtr->Xgbe.MaxPortNum++;
      if (FchXgbePort->XgbePortConfig) {
        FchTsXgbePortPlatformConfig (0, 0, Port, SystemPort, FchXgbePort);
        FchTsXgbePortMacAddress (0, 0, Port, FchXgbePortMac);
        FchTsXgbePortPtpInit (0, 0, Port, FchXgbePort);
      }
      //Enable xgbe Pad for OS driver
      IDS_HDT_CONSOLE (FCH_TRACE, " Enable XGBE Pad for OS driver, Port:0x%X \n",Value , Port);
      LocalCfgPtr->Xgbe.XgbeMdio0Enable = TRUE;
      LocalCfgPtr->Xgbe.XgbeSfpEnable   = TRUE;  
    } else {
      FchTsXgbePortClockGate (0, 0, Port);
    }

    FchXgbePort++;
    FchXgbePortMac++;
  }

  FchTsXgbePadEnable (0, LocalCfgPtr);
}
