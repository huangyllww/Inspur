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
#ifndef __SATA_TX_EQ_H__
#define __SATA_TX_EQ_H__
#include <Base.h>

 typedef struct{
	UINT8    Socket;
    UINT8    Die;
    UINT8    Port;
    EQ_DATA  SataTxEqGen1;
    EQ_DATA  SataTxEqGen2;
    EQ_DATA  SataTxEqGen3;	
 }SATA_TX_EQ_INIT_TABLE;
 
 /*----------------------------------------------------------------------------------------*/
/**
 * This function to get current sata eq 
 *
 * @param[in]     this           this
 * @param[in]     Socket         socket number
 * @param[in]     Die            Die number
 * @param[in]     Port           Port number
 * @param[in]     Speed          Speed number
 * @param[out]    EqData         Eq param
 *
 * @retval        EFI_SUCCESS    Get successfully
 */ 

EFI_STATUS
CpmGetSataEq(
    CPM_SATA_TX_EQ_PPI* this,
	UINT8           Socket,
    UINT8           Die,
    UINT8           Port,
    SATA_SPEED      Speed,
    EQ_DATA*        EqData
);

 /*----------------------------------------------------------------------------------------*/
/**
 * This function to set sata eq 
 *
 * @param[in]     this           this
 * @param[in]     Socket         socket number
 * @param[in]     Die            Die number
 * @param[in]     Port           Port number
 * @param[in]     Speed          Speed number
 * @param[out]    EqData         Eq param
 *
 * @retval        EFI_SUCCESS    Get successfully
 */ 
EFI_STATUS
CpmSetSataEq(
    CPM_SATA_TX_EQ_PPI* this,
	UINT8           Socket,
    UINT8           Die,
    UINT8           Port,
    SATA_SPEED      Speed,
    EQ_DATA*        EqData
);

//======================================================================================
//    Pcs Reg defines
//======================================================================================
#define PCS_LANE_COEFF_SMN_BASE 0x1240b500

typedef union{
    struct {
    UINT32    TxCoefficientPreCursor_gen1:6;//Read-Write.reset 0
    UINT32    Reserved_6_7:2;
    UINT32    TxCoefficientMainCursor_gen1:6;//read-write.reset 0x14
    UINT32    Reserved_14_15:2;
    UINT32    TxCoefficientPostCursor_gen1:6;//read-write.reset 0x12
    UINT32    Reserved_22_23:2;
    UINT32    rxX_eq_ctle_boost_gen1:5;//read-write.reset 0x06
    UINT32    rxX_eq_ctle_pole_gen1:3;//read-write.reset 0x05
    }Bits;
    UINT32 Reg32;
}PCS_LANE_COEFF1;

typedef union{
    struct {
    UINT32    TxCoefficientPreCursor_gen2:6;//Read-Write.reset 0
    UINT32    Reserved_6_7:2;
    UINT32    TxCoefficientMainCursor_gen2:6;//read-write.reset 0x17
    UINT32    Reserved_14_15:2;
    UINT32    TxCoefficientPostCursor_gen2:6;//read-write.reset 0x14
    UINT32    Reserved_22_23:2;
    UINT32    rxX_eq_ctle_boost_gen2:5;//read-write.reset 0x06
    UINT32    rxX_eq_ctle_pole_gen2:3;//read-write.reset 0x05
    }Bits;
    UINT32 Reg32;
}PCS_LANE_COEFF2;

typedef union{
    struct {
    UINT32    TxCoefficientPreCursor_gen3:6;//Read-Write.reset 0
    UINT32    Reserved_6_7:2;
    UINT32    TxCoefficientMainCursor_gen3:6;//read-write.reset 0x1d
    UINT32    Reserved_14_15:2;
    UINT32    TxCoefficientPostCursor_gen3:6;//read-write.reset 0x2c
    UINT32    Reserved_22_23:2;
    UINT32    rxX_eq_ctle_boost_gen3:5;//read-write.reset 0x10
    UINT32    rxX_eq_ctle_pole_gen3:3;//read-write.reset 0x06
    }Bits;
    UINT32 Reg32;
}PCS_LANE_COEFF3;
#endif
