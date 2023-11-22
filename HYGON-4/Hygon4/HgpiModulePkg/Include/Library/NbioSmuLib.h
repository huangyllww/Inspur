/* $NoKeywords:$ */

/**
 * @file
 *
 * SMU services
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

#ifndef _NBIOSMU_LIB_H_
#define _NBIOSMU_LIB_H_

#include <NbioRegisterTypes.h>
#include <HygonSmuBiosIf.h>

#define NBIO0_SMU_APERTURE_ID_HYEX                 0x03B
#define NBIO0_SMU_APERTURE_ID_HYGX                 0x01D

// Address
#define MP1_C2PMSG_MESSAGE_ADDRESS_HYEX            (UINT32)((NBIO0_SMU_APERTURE_ID_HYEX << 20) + 0x10528)
#define MP1_C2PMSG_MESSAGE_ADDRESS_HYGX            (UINT32)((NBIO0_SMU_APERTURE_ID_HYGX << 20) + 0x10528)

// Type
#define MP1_C2PMSG_MESSAGE_TYPE                   TYPE_SMN

#define MP1_C2PMSG_MESSAGE_CONTENT_OFFSET         0
#define MP1_C2PMSG_MESSAGE_CONTENT_WIDTH          32
#define MP1_C2PMSG_MESSAGE_CONTENT_MASK           0xffffffffUL

/// MP1_C2PMSG_MESSAGE
typedef union {
  struct {
    ///<
    UINT32    CONTENT : 32;                                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} MP1_C2PMSG_MESSAGE_STRUCT;

// Address
#define MP1_C2PMSG_RESPONSE_ADDRESS_HYEX           (UINT32)((NBIO0_SMU_APERTURE_ID_HYEX << 20) + 0x10564)
#define MP1_C2PMSG_RESPONSE_ADDRESS_HYGX           (UINT32)((NBIO0_SMU_APERTURE_ID_HYGX << 20) + 0x10564)

// Type
#define MP1_C2PMSG_RESPONSE_TYPE                  TYPE_SMN

#define MP1_C2PMSG_RESPONSE_CONTENT_OFFSET        0
#define MP1_C2PMSG_RESPONSE_CONTENT_WIDTH         32
#define MP1_C2PMSG_RESPONSE_CONTENT_MASK          0xffffffffUL

/// MP1_C2PMSG_RESPONSE
typedef union {
  struct {
    ///<
    UINT32    CONTENT : 32;                                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} MP1_C2PMSG_RESPONSE_STRUCT;

// Address
#define MP1_C2PMSG_ARGUMENT_0_ADDRESS_HYEX          (UINT32)((NBIO0_SMU_APERTURE_ID_HYEX << 20) + 0x10598)
#define MP1_C2PMSG_ARGUMENT_0_ADDRESS_HYGX          (UINT32)((NBIO0_SMU_APERTURE_ID_HYGX << 20) + 0x10598)

// Type
#define MP1_C2PMSG_ARGUMENT_0_TYPE                 TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_0_CONTENT_OFFSET       0
#define MP1_C2PMSG_ARGUMENT_0_CONTENT_WIDTH        32
#define MP1_C2PMSG_ARGUMENT_0_CONTENT_MASK         0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_0
typedef union {
  struct {
    ///<
    UINT32    CONTENT : 32;                                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} MP1_C2PMSG_ARGUMENT_0_STRUCT;

// Address
#define MP1_C2PMSG_ARGUMENT_1_ADDRESS_HYEX           (UINT32)((NBIO0_SMU_APERTURE_ID_HYEX << 20) + 0x1059c)
#define MP1_C2PMSG_ARGUMENT_1_ADDRESS_HYGX           (UINT32)((NBIO0_SMU_APERTURE_ID_HYGX << 20) + 0x1059c)

// Type
#define MP1_C2PMSG_ARGUMENT_1_TYPE                  TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_1_CONTENT_OFFSET        0
#define MP1_C2PMSG_ARGUMENT_1_CONTENT_WIDTH         32
#define MP1_C2PMSG_ARGUMENT_1_CONTENT_MASK          0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_1
typedef union {
  struct {
    ///<
    UINT32    CONTENT : 32;                                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} MP1_C2PMSG_ARGUMENT_1_STRUCT;

// Address
#define MP1_C2PMSG_ARGUMENT_2_ADDRESS_HYEX           (UINT32)((NBIO0_SMU_APERTURE_ID_HYEX << 20) + 0x105a0)
#define MP1_C2PMSG_ARGUMENT_2_ADDRESS_HYGX           (UINT32)((NBIO0_SMU_APERTURE_ID_HYGX << 20) + 0x105a0)

// Type
#define MP1_C2PMSG_ARGUMENT_2_TYPE                  TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_2_CONTENT_OFFSET        0
#define MP1_C2PMSG_ARGUMENT_2_CONTENT_WIDTH         32
#define MP1_C2PMSG_ARGUMENT_2_CONTENT_MASK          0xffffffff

/// MP1_C2PMSG_ARGUMENT_2
typedef union {
  struct {
    ///<
    UINT32    CONTENT : 32;                                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} MP1_C2PMSG_ARGUMENT_2_STRUCT;

// Address
#define MP1_C2PMSG_ARGUMENT_3_ADDRESS_HYEX           (UINT32)((NBIO0_SMU_APERTURE_ID_HYEX << 20) + 0x105a4)
#define MP1_C2PMSG_ARGUMENT_3_ADDRESS_HYGX           (UINT32)((NBIO0_SMU_APERTURE_ID_HYGX << 20) + 0x105a4)

// Type
#define MP1_C2PMSG_ARGUMENT_3_TYPE                  TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_3_CONTENT_OFFSET        0
#define MP1_C2PMSG_ARGUMENT_3_CONTENT_WIDTH         32
#define MP1_C2PMSG_ARGUMENT_3_CONTENT_MASK          0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_3
typedef union {
  struct {
    ///<
    UINT32    CONTENT : 32;                                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} MP1_C2PMSG_ARGUMENT_3_STRUCT;

// Address
#define MP1_C2PMSG_ARGUMENT_4_ADDRESS_HYEX          (UINT32)((NBIO0_SMU_APERTURE_ID_HYEX << 20) + 0x105a8)
#define MP1_C2PMSG_ARGUMENT_4_ADDRESS_HYGX          (UINT32)((NBIO0_SMU_APERTURE_ID_HYGX << 20) + 0x105a8)

// Type
#define MP1_C2PMSG_ARGUMENT_4_TYPE                 TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_4_CONTENT_OFFSET       0
#define MP1_C2PMSG_ARGUMENT_4_CONTENT_WIDTH        32
#define MP1_C2PMSG_ARGUMENT_4_CONTENT_MASK         0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_4
typedef union {
  struct {
    ///<
    UINT32    CONTENT : 32;                                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} MP1_C2PMSG_ARGUMENT_4_STRUCT;

// Address
#define MP1_C2PMSG_ARGUMENT_5_ADDRESS_HYEX          (UINT32)((NBIO0_SMU_APERTURE_ID_HYEX << 20) + 0x105ac)
#define MP1_C2PMSG_ARGUMENT_5_ADDRESS_HYGX          (UINT32)((NBIO0_SMU_APERTURE_ID_HYGX << 20) + 0x105ac)

// Type
#define MP1_C2PMSG_ARGUMENT_5_TYPE                 TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_5_CONTENT_OFFSET       0
#define MP1_C2PMSG_ARGUMENT_5_CONTENT_WIDTH        32
#define MP1_C2PMSG_ARGUMENT_5_CONTENT_MASK         0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_5
typedef union {
  struct {
    ///<
    UINT32    CONTENT : 32;                                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} MP1_C2PMSG_ARGUMENT_5_STRUCT;

/// SMU hot plug controller flags

#define HP_DISABLE_SIDEBAND            0x01
#define HP_DISABLE_L1_WA               0x02
#define HP_DISABLE_BRIDGE_DIS          0x04
#define HP_DISABLE_IRQ_POLL            0x08
#define HP_DISABLE_IRQ_SET_BRIDGE_DIS  0x10

/// SMU  basic lib

VOID
NbioSmuServiceCommonInitArguments (
  IN OUT   UINT32                   *SmuArg
  );

UINT32
NbioSmuServiceRequest (
  IN       PCI_ADDR                 NbioPciAddress,
  IN       UINT32                   PhysicalDieId,
  IN       UINT32                   RequestId,
  IN OUT   UINT32                   *RequestArgument,
  IN       UINT32                   AccessFlags
  );

HGPI_STATUS
NbioSmuFirmwareTest (
  IN       GNB_HANDLE               *NbioHandle
  );

HGPI_STATUS
NbioHsioServiceRequest (
  IN       GNB_HANDLE               *NbioHandle,
  IN       UINT32                   PhysicalDieId,
  IN       UINT32                   RequestId,
  IN OUT   UINT32                   *RequestArgument,
  IN       UINT32                   AccessFlags
  );

VOID
SmuNotifyS3Entry (
  );

#endif
