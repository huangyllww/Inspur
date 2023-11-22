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
 *
 ***************************************************************************/

#ifndef _HYGON_MBIST_PPI_H_
#define _HYGON_MBIST_PPI_H_

// ----------------------------------------------------
//
// HYGON MBIST PEI Initialization Complete PPI
//
// -------------------------------------------------------

typedef struct _MBIST_TEST_STATUS {
  UINT8    ResultValid[MAX_MBIST_SUBTEST][MAX_CHANNELS_PER_DIE];
  UINT8    ErrorStatus[MAX_MBIST_SUBTEST][MAX_CHANNELS_PER_DIE];
} MBIST_TEST_STATUS;

//
// MBIST DATA EYE
//
typedef struct _MBIST_MARGIN {
  UINT8    PositiveEdge;
  UINT8    NegativeEdge;
} MBIST_MARGIN;

typedef struct _MBIST_DATA_EYE_MARGIN {
  MBIST_MARGIN    ReadDqDelay;  ///< Smallest Positive and Negative Read Dq Delay margin
  MBIST_MARGIN    ReadVref;     ///< Smallest Positive and Negative Read Vref delay
  MBIST_MARGIN    WriteDqDelay; ///< Smallest Positive and Negative Write Dq Delay margin
  MBIST_MARGIN    WriteVref;    ///< Smallest Positive and Negative Write Vref delay
} MBIST_DATA_EYE_MARGIN;

typedef struct _MBIST_DATA_EYE_MARGIN_RECORD {
  BOOLEAN                  IsDataEyeValid;  ///< Is Data Eye Record Valid
  MBIST_DATA_EYE_MARGIN    DataEyeMargin;   ///< DataEyeRecord
} MBIST_DATA_EYE_MARGIN_RECORD;

typedef struct _MBIST_DATA_EYE {
  MBIST_DATA_EYE_MARGIN_RECORD    MbistDataEyeRecord[MAX_CHANNELS_PER_DIE][MAX_CHIPSELECT_PER_CHANNEL];
} MBIST_DATA_EYE;
//
// PPI function Prototype
//
typedef EFI_STATUS (EFIAPI *PEI_GET_MBIST_TEST_RESULTS)(
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINT32 *BufferSize,
  IN OUT MBIST_TEST_STATUS  *MbistTestResults
  );

typedef EFI_STATUS (EFIAPI *PEI_GET_MBIST_DATA_EYE)(
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINT32 *BufferSize,
  IN OUT MBIST_DATA_EYE  *MbistDataEye
  );

typedef struct _HYGON_MEMORY_MBIST_TEST_RESULTS_PPI {
  UINT32                        Revision;
  MBIST_TEST_STATUS             MbistTestStatus[MAX_SOCKETS_SUPPORTED][MAX_DIES_PER_SOCKET];
  MBIST_DATA_EYE                MbistDataEyeMargin[MAX_SOCKETS_SUPPORTED][MAX_DIES_PER_SOCKET];
  PEI_GET_MBIST_TEST_RESULTS    GetMbistTestResults;
  PEI_GET_MBIST_DATA_EYE        GetMbistDataEyeMargin;
} HYGON_MEMORY_MBIST_TEST_RESULTS_PPI;

// Current PPI revision
#define HYGON_MBIST_PPI_REVISION  0x03

extern EFI_GUID  gHygonMbistPeiPpiGuid;

#endif //_HYGON_MBIST_PPI_H_
