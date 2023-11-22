/* $NoKeywords:$ */
/**
 * @file
 *
 * CXL Base lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: CXL
 * @e \$Revision: 309090 $   @e \$Date: 2022-08-04 16:28:05 -0800 (Thurs, 04 Aug 2022) $
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
#ifndef _GNB_CXL_LIB_H_
#define _GNB_CXL_LIB_H_

#define DVSEC_CAP_ID                           0x23
#define CXL20_DVSEC_CAP_VID                    0x1E98
#define INTEL_DVSEC_CAP_VID                    0x8086

#define PRIMARY_BUS_REG_OFFSET                 0x18
#define SECONDARY_BUS_REG_OFFSET               0x19
#define SUBORDINATE_BUS_REG_OFFSET             0x1A

#define PROG_INTERFACE_OFFSET                  0x09
#define SUB_CLASSCODE_OFFSET                   0x0A
#define BASE_CLASSCODE_OFFSET                  0x0B

typedef union {
  struct {
    UINT32     PcieExtCapId:16;
    UINT32     PcieExtCapVersion:4;
    UINT32     PcieExtCapNextCapOffet:12;
  } Field; 
  UINT32 Value;
} PCIE_EXTENDED_CAPABILITY_HEADER;

#define CXLLIB_CM_CAP_HDR_CAP_ID                1
#define CXLLIB_CAPABILITY_HEADER_OFFSET         0

typedef union {
  struct {
    UINT32    CxlCapabilityId      : 16;                              // bit 0..15
    UINT32    CxlCapabilityVersion :  4;                              // bit 16..19
    UINT32    CxlCacheMemVersion   :  4;                              // bit 20..23
    UINT32    ArraySize            :  8;                              // bit 24..31
  } Bits;
  UINT32    Uint32;
} CXLLIB_CAPABILITY_HEADER;

typedef union {
  struct {
    UINT32    CxlCapabilityId          : 16;                          // bit 0..15
    UINT32    CxlCapabilityVersion     :  4;                          // bit 16..19
    UINT32    CxlCapabilityPointer     : 12;                          // bit 20..31
  } Bits;
  UINT32    Uint32;
} CXLLIB_COMMON_CAPABILITY_HEADER;

#ifndef CXL_2_0_REGISTER_LOCATOR_DVSEC_ID
#define CXL_2_0_REGISTER_LOCATOR_DVSEC_ID       8
#define REGISTER_BLOCK1_LOW_OFFSET              0x0C
#define REGISTER_BLOCK1_HIGH_OFFSET             0x10
#endif

#define  CXL_INVALID_DATA                       0xFFFFFFFF

typedef enum {
    EMPTY_REGISTER_BLOCK_ID   = 0x00,
    COMPONENT_REGISTER_BLOCK_ID,
    BAR_VIRT_ACL_REGISTER_BLOCK_ID,
    CXL_MEMORY_DEVICE_REGISTER_BLOCK_ID
} CXL_REGISTER_BLOCK_TYPE;

#pragma pack (push, 1)
typedef struct {
  UINT64                    BlockOffset;
  UINT32                    BarNo;
  UINT32                    BarOffset;
  UINT64                    BarAddress;
} CXL_REGISTER_BLOCK_INFO;
#pragma pack (pop)

#define  MmAddress( BaseAddr, Register ) \
            ((UINTN)(BaseAddr) + \
            (UINTN)(Register) \
             )

#define  Mm8Ptr( BaseAddr, Register ) \
            ((volatile UINT8 *)MmAddress (BaseAddr, Register ))

#define  Mm16Ptr( BaseAddr, Register ) \
            ((volatile UINT16 *)MmAddress (BaseAddr, Register ))

#define  Mm32Ptr( BaseAddr, Register ) \
            ((volatile UINT32 *)MmAddress (BaseAddr, Register ))

#define  CXL_REG8( BaseAddr, Register ) \
            (*Mm8Ptr ((BaseAddr), (Register)))

#define  CXL_REG16( BaseAddr, Register ) \
            (*Mm16Ptr ((BaseAddr), (Register)))

#define  CXL_REG32( BaseAddr, Register ) \
            (*Mm32Ptr ((BaseAddr), (Register)))

#define  CXL_REG8_WRITE( BaseAddr, Register, Data ) \
            (CXL_REG8 ((BaseAddr), (Register))) = ((UINT8) (Data))

#define  CXL_REG16_WRITE( BaseAddr, Register, Data ) \
            (CXL_REG16 ((BaseAddr), (Register))) = ((UINT16) (Data))

#define  CXL_REG32_WRITE( BaseAddr, Register, Data ) \
            (CXL_REG32 ((BaseAddr), (Register))) = ((UINT32) (Data))

UINT16
FindPciCxlDvsecCapability (
  IN      UINT32              PciAddress,
  IN      UINT16              CxlDvsecId,
  IN      HYGON_CONFIG_PARAMS   *StdHeader
  );

UINT8
FindRcrbPciCapability (
    IN      UINT64              RcrbAddress,
    IN      UINT16              CapabilityId,
    IN      HYGON_CONFIG_PARAMS   *StdHeader
  );

UINT16
FindRcrbExtendedCapability (
  IN      UINT64              RcrbAddress,
  IN      UINT16              ExtendedCapabilityId,
  IN      HYGON_CONFIG_PARAMS   *StdHeader
  );

UINT16
FindRcrbCxlDvsecCapability (
  IN      UINT64              RcrbAddress,
  IN      UINT16              CxlDvsecId,
  IN      HYGON_CONFIG_PARAMS   *StdHeader
  );

BOOLEAN IsCxlDevice(
  IN       PCI_ADDR               CxlDev
  );

VOID
FindCxl20RegBlock (
    IN     PCI_ADDR                          PciAddress,
    IN     CXL_REGISTER_BLOCK_TYPE           BlockType,
    OUT    CXL_REGISTER_BLOCK_INFO           *BlockInfo
);

VOID
EnumCxlCacheMemCapability (
    IN     UINT64                            CmRegBaseAddress
);

UINT32
FindCxlCacheMemCapability (
    IN     UINT64                            CmRegBaseAddress,
    IN     UINT16                            CmCapId
);

VOID
DumpCxlCmComponentRegisters (
    IN     UINT64                            CxlCmRegBaseAddr
);

UINT32
GetMmio32BarSize (
  IN  UINT32     EndpointBar
  );

UINT64
GetMmio64BarSize (
  IN  UINT32     EndpointBar
  );
#endif
