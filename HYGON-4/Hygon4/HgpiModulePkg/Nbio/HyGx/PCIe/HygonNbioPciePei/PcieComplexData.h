/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe definitions
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
#ifndef _PCIECOMPLEXDATA_H_
#define _PCIECOMPLEXDATA_H_

//#define MAX_NUM_PHYs            2
#define MAX_NUM_LANE_PER_PHY      8

#define NUMBER_OF_GPP0_PORTS       8
#define NUMBER_OF_GPP1_PORTS       8
#define NUMBER_OF_GPP2_PORTS       2

#define NON_INITIALIZED_PCI_ADDRESS  0

#define GPP0_WRAP_ID               0
#define GPP0_START_PHY_LANE        0
#define GPP0_END_PHY_LANE          15
#define GPP0_CORE_ID               0
#define GPP0_NUMBER_OF_PIFs        1

#define GPP1_WRAP_ID               1
#define GPP1_START_PHY_LANE        16
#define GPP1_END_PHY_LANE          31
#define GPP1_CORE_ID               1
#define GPP1_NUMBER_OF_PIFs        1

#define GPP2_WRAP_ID               2
#define GPP2_START_PHY_LANE        128
#define GPP2_END_PHY_LANE          143
#define GPP2_CORE_ID               2
#define GPP2_NUMBER_OF_PIFs        1

//#define DDI_NUMBER_OF_PIFs        1

// PBR0
#define PBR0                      0
#define PBR0_NATIVE_PCI_DEV       1
#define PBR0_NATIVE_PCI_FUN       1
#define PBR0_CORE_ID              GPP0_CORE_ID
#define PBR0_PORT_ID              0
#define PBR0_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR0_UNIT_ID              0
#define PBR0_NUM_UNIT_IDs         0x1

// PBR1
#define PBR1                      1
#define PBR1_NATIVE_PCI_DEV       1
#define PBR1_NATIVE_PCI_FUN       2
#define PBR1_CORE_ID              GPP0_CORE_ID
#define PBR1_PORT_ID              1
#define PBR1_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR1_UNIT_ID              1
#define PBR1_NUM_UNIT_IDs         0x1

// PBR2
#define PBR2                      2
#define PBR2_NATIVE_PCI_DEV       1
#define PBR2_NATIVE_PCI_FUN       3
#define PBR2_CORE_ID              GPP0_CORE_ID
#define PBR2_PORT_ID              2
#define PBR2_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR2_UNIT_ID              2
#define PBR2_NUM_UNIT_IDs         0x1

// PBR3
#define PBR3                      3
#define PBR3_NATIVE_PCI_DEV       1
#define PBR3_NATIVE_PCI_FUN       4
#define PBR3_CORE_ID              GPP0_CORE_ID
#define PBR3_PORT_ID              3
#define PBR3_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR3_UNIT_ID              3
#define PBR3_NUM_UNIT_IDs         0x1

// PBR4
#define PBR4                      4
#define PBR4_NATIVE_PCI_DEV       1
#define PBR4_NATIVE_PCI_FUN       5
#define PBR4_CORE_ID              GPP0_CORE_ID
#define PBR4_PORT_ID              4
#define PBR4_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR4_UNIT_ID              4
#define PBR4_NUM_UNIT_IDs         0x1

// PBR5
#define PBR5                      5
#define PBR5_NATIVE_PCI_DEV       1
#define PBR5_NATIVE_PCI_FUN       6
#define PBR5_CORE_ID              GPP0_CORE_ID
#define PBR5_PORT_ID              5
#define PBR5_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR5_UNIT_ID              5
#define PBR5_NUM_UNIT_IDs         0x1

// PBR6
#define PBR6                      6
#define PBR6_NATIVE_PCI_DEV       1
#define PBR6_NATIVE_PCI_FUN       7
#define PBR6_CORE_ID              GPP0_CORE_ID
#define PBR6_PORT_ID              6
#define PBR6_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR6_UNIT_ID              6
#define PBR6_NUM_UNIT_IDs         0x1

// PBR7
#define PBR7                      7
#define PBR7_NATIVE_PCI_DEV       2
#define PBR7_NATIVE_PCI_FUN       1
#define PBR7_CORE_ID              GPP0_CORE_ID
#define PBR7_PORT_ID              7
#define PBR7_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR7_UNIT_ID              7
#define PBR7_NUM_UNIT_IDs         0x1

// PBR8
#define PBR8                      8
#define PBR8_NATIVE_PCI_DEV       3
#define PBR8_NATIVE_PCI_FUN       1
#define PBR8_CORE_ID              GPP1_CORE_ID
#define PBR8_PORT_ID              8
#define PBR8_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR8_UNIT_ID              8
#define PBR8_NUM_UNIT_IDs         0x1

// PBR9
#define PBR9                      9
#define PBR9_NATIVE_PCI_DEV       3
#define PBR9_NATIVE_PCI_FUN       2
#define PBR9_CORE_ID              GPP1_CORE_ID
#define PBR9_PORT_ID              9
#define PBR9_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR9_UNIT_ID              9
#define PBR9_NUM_UNIT_IDs         0x1

// PBR10
#define PBR10                      10
#define PBR10_NATIVE_PCI_DEV       3
#define PBR10_NATIVE_PCI_FUN       3
#define PBR10_CORE_ID              GPP1_CORE_ID
#define PBR10_PORT_ID              10
#define PBR10_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR10_UNIT_ID              10
#define PBR10_NUM_UNIT_IDs         0x1

// PBR11
#define PBR11                      11
#define PBR11_NATIVE_PCI_DEV       3
#define PBR11_NATIVE_PCI_FUN       4
#define PBR11_CORE_ID              GPP1_CORE_ID
#define PBR11_PORT_ID              11
#define PBR11_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR11_UNIT_ID              11
#define PBR11_NUM_UNIT_IDs         0x1

// PBR12
#define PBR12                      12
#define PBR12_NATIVE_PCI_DEV       3
#define PBR12_NATIVE_PCI_FUN       5
#define PBR12_CORE_ID              GPP1_CORE_ID
#define PBR12_PORT_ID              12
#define PBR12_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR12_UNIT_ID              12
#define PBR12_NUM_UNIT_IDs         0x1

// PBR13
#define PBR13                      13
#define PBR13_NATIVE_PCI_DEV       3
#define PBR13_NATIVE_PCI_FUN       6
#define PBR13_CORE_ID              GPP1_CORE_ID
#define PBR13_PORT_ID              13
#define PBR13_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR13_UNIT_ID              13
#define PBR13_NUM_UNIT_IDs         0x1

// PBR14
#define PBR14                      14
#define PBR14_NATIVE_PCI_DEV       3
#define PBR14_NATIVE_PCI_FUN       7
#define PBR14_CORE_ID              GPP1_CORE_ID
#define PBR14_PORT_ID              14
#define PBR14_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR14_UNIT_ID              14
#define PBR14_NUM_UNIT_IDs         0x1

// PBR15
#define PBR15                      15
#define PBR15_NATIVE_PCI_DEV       4
#define PBR15_NATIVE_PCI_FUN       1
#define PBR15_CORE_ID              GPP1_CORE_ID
#define PBR15_PORT_ID              15
#define PBR15_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR15_UNIT_ID              15
#define PBR15_NUM_UNIT_IDs         0x1

// PBR16
#define PBR16                      16
#define PBR16_NATIVE_PCI_DEV       5
#define PBR16_NATIVE_PCI_FUN       1
#define PBR16_CORE_ID              GPP2_CORE_ID
#define PBR16_PORT_ID              16
#define PBR16_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR16_UNIT_ID              16
#define PBR16_NUM_UNIT_IDs         0x1

// PBR17
#define PBR17                      17
#define PBR17_NATIVE_PCI_DEV       5
#define PBR17_NATIVE_PCI_FUN       2
#define PBR17_CORE_ID              GPP2_CORE_ID
#define PBR17_PORT_ID              17
#define PBR17_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR17_UNIT_ID              17
#define PBR17_NUM_UNIT_IDs         0x1

// PBR18
#define PBR18                 18
#define PBR18_NATIVE_PCI_DEV  5
#define PBR18_NATIVE_PCI_FUN  3
#define PBR18_CORE_ID         GPP2_CORE_ID
#define PBR18_PORT_ID         18
#define PBR18_PCI_ADDRESS     NON_INITIALIZED_PCI_ADDRESS
#define PBR18_UNIT_ID         18
#define PBR18_NUM_UNIT_IDs    0x1

// PBR19
#define PBR19                 19
#define PBR19_NATIVE_PCI_DEV  5
#define PBR19_NATIVE_PCI_FUN  4
#define PBR19_CORE_ID         GPP2_CORE_ID
#define PBR19_PORT_ID         19
#define PBR19_PCI_ADDRESS     NON_INITIALIZED_PCI_ADDRESS
#define PBR19_UNIT_ID         19
#define PBR19_NUM_UNIT_IDs    0x1

// PBR20
#define PBR20                 20
#define PBR20_NATIVE_PCI_DEV  5
#define PBR20_NATIVE_PCI_FUN  5
#define PBR20_CORE_ID         GPP2_CORE_ID
#define PBR20_PORT_ID         20
#define PBR20_PCI_ADDRESS     NON_INITIALIZED_PCI_ADDRESS
#define PBR20_UNIT_ID         20
#define PBR20_NUM_UNIT_IDs    0x1

// PBR21
#define PBR21                 21
#define PBR21_NATIVE_PCI_DEV  5
#define PBR21_NATIVE_PCI_FUN  6
#define PBR21_CORE_ID         GPP2_CORE_ID
#define PBR21_PORT_ID         21
#define PBR21_PCI_ADDRESS     NON_INITIALIZED_PCI_ADDRESS
#define PBR21_UNIT_ID         21
#define PBR21_NUM_UNIT_IDs    0x1

// PBR22
#define PBR22                 22
#define PBR22_NATIVE_PCI_DEV  5
#define PBR22_NATIVE_PCI_FUN  7
#define PBR22_CORE_ID         GPP2_CORE_ID
#define PBR22_PORT_ID         22
#define PBR22_PCI_ADDRESS     NON_INITIALIZED_PCI_ADDRESS
#define PBR22_UNIT_ID         22
#define PBR22_NUM_UNIT_IDs    0x1

// PBR23
#define PBR23                 23
#define PBR23_NATIVE_PCI_DEV  6
#define PBR23_NATIVE_PCI_FUN  1
#define PBR23_CORE_ID         GPP2_CORE_ID
#define PBR23_PORT_ID         23
#define PBR23_PCI_ADDRESS     NON_INITIALIZED_PCI_ADDRESS
#define PBR23_UNIT_ID         23
#define PBR23_NUM_UNIT_IDs    0x1

#define GPP_CORE_x4x4             ((4ull << 8) | (4ull << 0))
#define GPP_CORE_x4x2x2           ((2ull << 16) | (2ull << 8) | (4ull << 0))
#define GPP_CORE_x4x2x1x1         ((1ull << 24) | (1ull << 16) | (2ull << 8) | (4ull << 0))
#define GPP_CORE_x4x1x1x1x1       ((1ull << 32) | (1ull << 24) | (1ull << 16) | (1ull << 8) | (4ull << 0))

/// Complex Configuration for EMEI full NBIO
typedef struct {
  PCIe_SILICON_CONFIG     Silicon;                ///< Silicon

  PCIe_WRAPPER_CONFIG     Gpp0Wrapper;            ///< GPP0 Wrapper
  PCIe_WRAPPER_CONFIG     Gpp1Wrapper;            ///< GPP1 Wrapper
  // GPP0
  PCIe_ENGINE_CONFIG      PortPBR7;               ///< Port PBR7
  PCIe_ENGINE_CONFIG      PortPBR6;               ///< Port PBR6
  PCIe_ENGINE_CONFIG      PortPBR5;               ///< Port PBR5
  PCIe_ENGINE_CONFIG      PortPBR4;               ///< Port PBR4
  PCIe_ENGINE_CONFIG      PortPBR3;               ///< Port PBR3
  PCIe_ENGINE_CONFIG      PortPBR2;               ///< Port PBR2
  PCIe_ENGINE_CONFIG      PortPBR1;               ///< Port PBR1
  PCIe_ENGINE_CONFIG      PortPBR0;               ///< Port PBR0
  // GPP1
  PCIe_ENGINE_CONFIG     PortPBR8;                 ///< Port PBR8
} EMEI_COMPLEX_CONFIG_FULL_NBIO;

/// Complex Configuration for EMEI Lite NBIO
typedef struct {
  PCIe_SILICON_CONFIG    Silicon;                  ///< Silicon NBIO

  PCIe_WRAPPER_CONFIG    Gpp0Wrapper;              ///< GPP0 Wrapper

  // GPP0
  PCIe_ENGINE_CONFIG     PortPBR7;                 ///< Port PBR7
  PCIe_ENGINE_CONFIG     PortPBR6;                 ///< Port PBR6
  PCIe_ENGINE_CONFIG     PortPBR5;                 ///< Port PBR5
  PCIe_ENGINE_CONFIG     PortPBR4;                 ///< Port PBR4
  PCIe_ENGINE_CONFIG     PortPBR3;                 ///< Port PBR3
  PCIe_ENGINE_CONFIG     PortPBR2;                 ///< Port PBR2
  PCIe_ENGINE_CONFIG     PortPBR1;                 ///< Port PBR1
  PCIe_ENGINE_CONFIG     PortPBR0;                 ///< Port PBR0
} EMEI_COMPLEX_CONFIG_LITE_NBIO;

/// Complex Configuration for Dujiang NBIO
typedef struct {
  PCIe_SILICON_CONFIG    Silicon;                  ///< Silicon NBIO

  PCIe_WRAPPER_CONFIG    Gpp2Wrapper;              ///< GPP0 Wrapper

  // GPP2
  PCIe_ENGINE_CONFIG     PortPBR23;                 ///< Port PBR23
  PCIe_ENGINE_CONFIG     PortPBR22;                 ///< Port PBR22
  PCIe_ENGINE_CONFIG     PortPBR21;                 ///< Port PBR21
  PCIe_ENGINE_CONFIG     PortPBR20;                 ///< Port PBR20
  PCIe_ENGINE_CONFIG     PortPBR19;                 ///< Port PBR19
  PCIe_ENGINE_CONFIG     PortPBR18;                 ///< Port PBR18
  PCIe_ENGINE_CONFIG     PortPBR17;                 ///< Port PBR17
  PCIe_ENGINE_CONFIG     PortPBR16;                 ///< Port PBR16
} DJ_COMPLEX_CONFIG_NBIO;


#endif
