/* $NoKeywords:$ */
/**
 * @file
 *
 * Contains the definition of the SMU9
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: Nbio/SMU
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
#ifndef SMU9_H
#define SMU9_H

//#include <stdint.h>
//#include <features/smu_features.h>

#pragma pack(push, 1)

#define ENABLE_DEBUG_FEATURES

// Feature Control Defines
#define FEATURE_CCLK_CONTROLLER_BIT    0
#define FEATURE_FAN_CONTROLLER_BIT     1
#define FEATURE_DATA_CALCULATION_BIT   2
#define FEATURE_PPT_BIT                3
#define FEATURE_TDC_BIT                4
#define FEATURE_THERMAL_BIT            5
#define FEATURE_FIT_BIT                6
#define FEATURE_QOS_BIT                7
#define FEATURE_CORE_CSTATES_BIT       8
#define FEATURE_PROCHOT_BIT            9
#define FEATURE_MCM_DATA_TRANSFER_BIT  10
#define FEATURE_DLWM_BIT               11
#define FEATURE_PC6_BIT                12
#define FEATURE_CSTATE_BOOST_BIT       13
#define FEATURE_VOLTAGE_CONTROLLER_BIT 14
#define FEATURE_HOT_PLUG_BIT           15
#define FEATURE_SPARE_16_BIT           16
#define FEATURE_FW_DEEPSLEEP_BIT       17
#define FEATURE_SPARE_18_BIT           18
#define FEATURE_SPARE_19_BIT           19
#define FEATURE_SPARE_20_BIT           20
#define FEATURE_SPARE_21_BIT           21
#define FEATURE_SPARE_22_BIT           22
#define FEATURE_SPARE_23_BIT           23
#define FEATURE_SPARE_24_BIT           24
#define FEATURE_SPARE_25_BIT           25
#define FEATURE_SPARE_26_BIT           26
#define FEATURE_SPARE_27_BIT           27
#define FEATURE_SPARE_28_BIT           28
#define FEATURE_SPARE_29_BIT           29
#define FEATURE_SPARE_30_BIT           30
#define FEATURE_SPARE_31_BIT           31

#define NUM_FEATURES                   32

#define FEATURE_CCLK_CONTROLLER_MASK    (1 << FEATURE_CCLK_CONTROLLER_BIT)
#define FEATURE_FAN_CONTROLLER_MASK     (1 << FEATURE_FAN_CONTROLLER_BIT)
#define FEATURE_DATA_CALCULATION_MASK   (1 << FEATURE_DATA_CALCULATION_BIT)
#define FEATURE_PPT_MASK                (1 << FEATURE_PPT_BIT)
#define FEATURE_TDC_MASK                (1 << FEATURE_TDC_BIT)
#define FEATURE_THERMAL_MASK            (1 << FEATURE_THERMAL_BIT)
#define FEATURE_FIT_MASK                (1 << FEATURE_FIT_BIT)
#define FEATURE_QOS_MASK                (1 << FEATURE_QOS_BIT)
#define FEATURE_CORE_CSTATES_MASK       (1 << FEATURE_CORE_CSTATES_BIT)
#define FEATURE_PROCHOT_MASK            (1 << FEATURE_PROCHOT_BIT)
#define FEATURE_MCM_DATA_TRANSFER_MASK  (1 << FEATURE_MCM_DATA_TRANSFER_BIT)
#define FEATURE_DLWM_MASK               (1 << FEATURE_DLWM_BIT)
#define FEATURE_PC6_MASK                (1 << FEATURE_PC6_BIT)
#define FEATURE_CSTATE_BOOST_MASK       (1 << FEATURE_CSTATE_BOOST_BIT)
#define FEATURE_VOLTAGE_CONTROLLER_MASK (1 << FEATURE_VOLTAGE_CONTROLLER_BIT)
#define FEATURE_HOT_PLUG_MASK           (1 << FEATURE_HOT_PLUG_BIT)
#define FEATURE_FW_DEEPSLEEP_MASK       (1 << FEATURE_FW_DEEPSLEEP_BIT)


// These defines are used with the following messages:
// SMC_MSG_TransferTableDram2Smu
// SMC_MSG_TransferTableSmu2Dram
#define TABLE_PPTABLE            0
#define TABLE_PCIE_HP_CONFIG     1
#define TABLE_PMSTATUSLOG        2
#define TABLE_COUNT              3


struct SMU9_Firmware_Footer
{
  UINT32 Signature;
};
typedef struct SMU9_Firmware_Footer SMU9_Firmware_Footer;



typedef struct 
{
  UINT8   Nonce[16];          // [0x00] Unique image id
  UINT32  HeaderVersion;      // [0x10] Version of the header
  UINT32  SizeFWSigned;       // [0x14] Size of the FW to be included in signature in bytes
  UINT32  EncOption;          // [0x18] 0 - Not encrypted, 1 - encrypted
  UINT32  EncAlgID;           // [0x1C] Encryption algorithm id
  UINT8   EncParameters[16];  // [0x20] Encryption Parameters
  UINT32  SigOption;          // [0x30] 0 - not signed 1 - signed
  UINT32  SigAlgID;           // [0x34] Signature algorithm ID
  UINT8   SigParameters[16];  // [0x38] Signature parameter
  UINT32  CompOption;         // [0x48] Compression option
  UINT32  CompAlgID;          // [0x4C] Compression Algorithm ID
  UINT32  UnCompImageSize;    // [0x50] Uncompressed Image Size
  UINT32  CompImageSize;      // [0x54] compressed Image Size
  UINT8   CompParameters[8];  // [0x58] Compression Parameters
  UINT32  ImageVersion;       // [0x60] Off Chip Firmware Version
  UINT32  APUFamilyID;        // [0x64] APU Family ID or SoC ID
  UINT32  FirmwareLoadAddr;   // [0x68] Firmware Load address (default 0)
  UINT32  SizeImage;          // [0x6C] Size of entire signed image including key tokens
  UINT32  SizeFWUnSigned;     // [0x70] Size of Un-signed portion of the FW
  UINT32  FirmwareSplitAddr;  // [0x74] Joining point of combined FWs (e.g. Nwd/Swd split address)
  UINT8   Reserved[8];        // [0x78] *** RESERVED ***
  UINT8   EncKey[16];         // [0x80] Encryption Key (Wrapped MEK)
  UINT8   SigningInfo[16];    // [0x90] Signing tool specific information
  UINT8   Padd[96];           // [0xA4] *** RESERVED ***
} SMU_Firmware_Header;



#define SMU9_MAX_CUS 2
#define SMU9_PSMS_PER_CU 10
#define SMU9_Num_tests 3

#define SMU9_FIRMWARE_HEADER_LOCATION 0x1FF80
#define SMU9_UNBCSR_START_ADDR 0xC0100000UL

#pragma pack(pop)


#endif

