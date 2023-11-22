/* $NoKeywords:$ */
/**
 * @file
 *
 * HYGON Sata TX EQ
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  SataTxEq
 *
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2022 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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

#ifndef __SATA_TX_EQ_H__
#define __SATA_TX_EQ_H__
#include <Base.h>
#include <HygonCpmPei.h>
#include <HygonCpmBase.h>

typedef struct {
  UINT8    Pre;
  UINT8    Main;
  UINT8    Post;	
} SATA_TX_CURSOR;
 
typedef enum{
  SATA_GEN1=1, 
  SATA_GEN2,
  SATA_GEN3
}SATA_SPEED;

typedef struct {
  BOOLEAN        InitGen1Cursor;
  SATA_TX_CURSOR SataGen1Cursor;
  BOOLEAN        InitGen2Cursor;
  SATA_TX_CURSOR SataGen2Cursor;
  BOOLEAN        InitGen3Cursor;
  SATA_TX_CURSOR SataGen3Cursor;
} SATA_EQ_INIT_DATE;

 /*----------------------------------------------------------------------------------------*/
/**
 * This function to set sata eq 
 *
 * @param[in]     CpmTablePpi    CPM Table
 * @param[in]     Socket         Socket number
 * @param[in]     Nbio           Nbio number
 * @param[in]     Port           Port number
 * @param[in]     Speed          Speed number
 * @param[out]    EqData         Eq param
 *
 */ 
VOID
CpmSetSataEq (
  HYGON_CPM_TABLE_PPI  *CpmTablePpi,
  UINT8                Socket,
  UINT8                Nbio,
  UINT8                Port,
  SATA_SPEED           Speed,
  SATA_TX_CURSOR       *EqData
  );

//======================================================================================
//    Pcs Reg defines
//======================================================================================
#define PCS_LANE_COEFF_SMN_BASE_HYEX 0x1800b500
#define PCS_LANE_COEFF_SMN_BASE_HYGX 0xDD0b500

typedef union {
    struct {
    UINT32    TxCoefficientPreCursorGen1:6;//Read-Write.reset 0
    UINT32    Reserved6To7:2;
    UINT32    TxCoefficientMainCursorGen1:6;//read-write.reset 0x14
    UINT32    Reserved14To15:2;
    UINT32    TxCoefficientPostCursorGen1:6;//read-write.reset 0x12
    UINT32    Reserved22To23:2;
    UINT32    RxEqCtleBoostGen1:5;//read-write.reset 0x06
    UINT32    RxEqCtlePoleGen1:3;//read-write.reset 0x05
    } Bits;
    UINT32 Reg32;
} PCS_LANE_COEFF1;

typedef union {
    struct {
    UINT32    TxCoefficientPreCursorGen2:6;//Read-Write.reset 0
    UINT32    Reserved6To7:2;
    UINT32    TxCoefficientMainCursorGen2:6;//read-write.reset 0x17
    UINT32    Reserved14To15:2;
    UINT32    TxCoefficientPostCursorGen2:6;//read-write.reset 0x14
    UINT32    Reserved22To23:2;
    UINT32    RxEqCtleBoostGen2:5;//read-write.reset 0x06
    UINT32    RxEqCtlePoleGen2:3;//read-write.reset 0x05
    } Bits;
    UINT32 Reg32;
} PCS_LANE_COEFF2;

typedef union{
    struct {
    UINT32    TxCoefficientPreCursorGen3:6;//Read-Write.reset 0
    UINT32    Reserved6To7:2;
    UINT32    TxCoefficientMainCursorGen3:6;//read-write.reset 0x1d
    UINT32    Reserved14To15:2;
    UINT32    TxCoefficientPostCursorGen3:6;//read-write.reset 0x2c
    UINT32    Reserved22To23:2;
    UINT32    RxEqCtleBoostGen3:5;//read-write.reset 0x10
    UINT32    RxEqCtlePoleGen3:3;//read-write.reset 0x06
    } Bits;
    UINT32 Reg32;
} PCS_LANE_COEFF3;

#endif
