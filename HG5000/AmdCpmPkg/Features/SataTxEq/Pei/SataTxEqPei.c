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
 
#include <AmdCpmPei.h>
#include <AmdCpmBase.h>
#include <Ppi/CpmSataTxEqPpi.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include "SataTxEqPei.h"

 SATA_TX_EQ_INIT_TABLE SataTxEqInitTable[]={
	//socket  | Die   |Port  |EQ GEN1        |EQ GEN2        |EQ GEN3       | 
	//        |       |      |pre main post  |pre main post  |pre main post |
	{0,        0,      4,    {0x0,0x20,0x12},{0x0,0x20,0x14},{0x0,0x1F,0x24}},//Socket 0 Die 0 Port 4
	{0,        0,      5,    {0x0,0x20,0x12},{0x0,0x20,0x14},{0x0,0x1F,0x24}},//Socket 0 Die 0 Port 5
	{0,        0,      6,    {0x0,0x20,0x12},{0x0,0x20,0x14},{0x0,0x1F,0x24}},//Socket 0 Die 0 Port 6
	{0,        0,      7,    {0x0,0x20,0x12},{0x0,0x20,0x14},{0x0,0x1F,0x24}},//Socket 0 Die 0 Port 7
    //Table end	
	{0xFF,0,0,{0,0,0},{0,0,0},{0,0,0}}
 };
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
)
{
	EFI_STATUS         Status;	
	UINT32             SmnOffset;
    PCS_LANE_COEFF1    LanCoeff1;
    PCS_LANE_COEFF2    LanCoeff2;
    PCS_LANE_COEFF3    LanCoeff3;
    EFI_PEI_SERVICES   **PeiServices;
    AMD_CPM_TABLE_PPI  *CpmTablePpiPtr;

    PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();

    Status = (*PeiServices)->LocatePpi (
                                      (CPM_PEI_SERVICES**)PeiServices,
                                      &gAmdCpmTablePpiGuid,
                                      0,
                                      NULL,
                                      (VOID**)&CpmTablePpiPtr
                                      );
    if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR,"CpmGetSataEq:locate gAmdCpmTablePpiGuid Fail!!!\n"));	
        return  Status;
    }
    //calculate lan coff address by speed and port
    SmnOffset =  ((Speed*4)<<4) + Port*4;
    DEBUG ((EFI_D_ERROR,"CpmGetSataEq:SmnAddress=%x\n",PCS_LANE_COEFF_SMN_BASE+SmnOffset));  
    switch(Speed)
    {
        //Gen 1 select
        case SATA_GEN1:
            LanCoeff1.Reg32 = CpmTablePpiPtr->CommonFunction.SmnRead32 (CpmTablePpiPtr,Socket,Die,PCS_LANE_COEFF_SMN_BASE,SmnOffset);
            EqData->PreCursor  = (UINT8)LanCoeff1.Bits.TxCoefficientPreCursor_gen1;
            EqData->MainCursor = (UINT8)LanCoeff1.Bits.TxCoefficientMainCursor_gen1;
            EqData->PostCursor = (UINT8)LanCoeff1.Bits.TxCoefficientPostCursor_gen1;
            break;
        //Gen 2 select
        case SATA_GEN2:
            LanCoeff2.Reg32 = CpmTablePpiPtr->CommonFunction.SmnRead32 (CpmTablePpiPtr,Socket,Die,PCS_LANE_COEFF_SMN_BASE,SmnOffset);
            EqData->PreCursor  = (UINT8)LanCoeff2.Bits.TxCoefficientPreCursor_gen2;
            EqData->MainCursor = (UINT8)LanCoeff2.Bits.TxCoefficientMainCursor_gen2;
            EqData->PostCursor = (UINT8)LanCoeff2.Bits.TxCoefficientPostCursor_gen2;
            break;
        //Gen 3 select
        case SATA_GEN3:
            LanCoeff3.Reg32 = CpmTablePpiPtr->CommonFunction.SmnRead32 (CpmTablePpiPtr,Socket,Die,PCS_LANE_COEFF_SMN_BASE,SmnOffset);
            EqData->PreCursor  = (UINT8)LanCoeff3.Bits.TxCoefficientPreCursor_gen3;
            EqData->MainCursor = (UINT8)LanCoeff3.Bits.TxCoefficientMainCursor_gen3;
            EqData->PostCursor = (UINT8)LanCoeff3.Bits.TxCoefficientPostCursor_gen3;
            break;
        default:
            break;
    }
    DEBUG ((EFI_D_ERROR,"CpmGetSataEq:Socket[%x] Die[%x] Port[%x] Speed[%x] - PreCursor[0x%x] MainCursor[0x%x] PostCursor[0x%x]\n",Socket,Die,Port,Speed,EqData->PreCursor,EqData->MainCursor,EqData->PostCursor));
    return EFI_SUCCESS;
}

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
)
{
	EFI_STATUS         Status;
	UINT32             SmnOffset;
    PCS_LANE_COEFF1    LanCoeff1;
    PCS_LANE_COEFF2    LanCoeff2;
    PCS_LANE_COEFF3    LanCoeff3;
    EFI_PEI_SERVICES   **PeiServices;
    AMD_CPM_TABLE_PPI  *CpmTablePpiPtr;

    PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();

    Status = (*PeiServices)->LocatePpi (
                                      (CPM_PEI_SERVICES**)PeiServices,
                                      &gAmdCpmTablePpiGuid,
                                      0,
                                      NULL,
                                      (VOID**)&CpmTablePpiPtr
                                      );
    if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR,"CpmGetSataEq:locate gAmdCpmTablePpiGuid Fail!!!\n"));
        return  Status;
    }    
    //calculate lan coff address by speed and port
    SmnOffset = ((Speed*4)<<4) + Port*4;
    DEBUG ((EFI_D_ERROR,"CpmGetSataEq:SmnAddress=%x\n",PCS_LANE_COEFF_SMN_BASE+SmnOffset));
    DEBUG ((EFI_D_ERROR,"CpmGetSataEq:Socket[%x] Die[%x] Port[%x] Speed[%x] - PreCursor[0x%x] MainCursor[0x%x] PostCursor[0x%x]\n",Socket,Die,Port,Speed,EqData->PreCursor,EqData->MainCursor,EqData->PostCursor));
    switch(Speed)
    {
        //Gen 1 select
        case SATA_GEN1:
            LanCoeff1.Reg32 = CpmTablePpiPtr->CommonFunction.SmnRead32 (CpmTablePpiPtr,Socket,Die,PCS_LANE_COEFF_SMN_BASE,SmnOffset);
            LanCoeff1.Bits.TxCoefficientPreCursor_gen1  = EqData->PreCursor;
            LanCoeff1.Bits.TxCoefficientMainCursor_gen1 = EqData->MainCursor;
            LanCoeff1.Bits.TxCoefficientPostCursor_gen1 = EqData->PostCursor;
			CpmTablePpiPtr->CommonFunction.SmnWrite32 (CpmTablePpiPtr,Socket,Die,PCS_LANE_COEFF_SMN_BASE,SmnOffset,LanCoeff1.Reg32);
            break;
        //Gen 2 select
        case SATA_GEN2:
            LanCoeff2.Reg32 = CpmTablePpiPtr->CommonFunction.SmnRead32 (CpmTablePpiPtr,Socket,Die,PCS_LANE_COEFF_SMN_BASE,SmnOffset);
            LanCoeff2.Bits.TxCoefficientPreCursor_gen2  = EqData->PreCursor;
            LanCoeff2.Bits.TxCoefficientMainCursor_gen2 = EqData->MainCursor;
            LanCoeff2.Bits.TxCoefficientPostCursor_gen2 = EqData->PostCursor;
			CpmTablePpiPtr->CommonFunction.SmnWrite32 (CpmTablePpiPtr,Socket,Die,PCS_LANE_COEFF_SMN_BASE,SmnOffset,LanCoeff2.Reg32);
            break;
        //Gen 3 select
        case SATA_GEN3:
            LanCoeff3.Reg32 = CpmTablePpiPtr->CommonFunction.SmnRead32 (CpmTablePpiPtr,Socket,Die,PCS_LANE_COEFF_SMN_BASE,SmnOffset);
            LanCoeff3.Bits.TxCoefficientPreCursor_gen3  = EqData->PreCursor;
            LanCoeff3.Bits.TxCoefficientMainCursor_gen3 = EqData->MainCursor;
            LanCoeff3.Bits.TxCoefficientPostCursor_gen3 = EqData->PostCursor;
			CpmTablePpiPtr->CommonFunction.SmnWrite32 (CpmTablePpiPtr,Socket,Die,PCS_LANE_COEFF_SMN_BASE,SmnOffset,LanCoeff3.Reg32);
            break;
        default:
            break;
    }
    
    return EFI_SUCCESS;
}
 /*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the Sata Tx Eq driver
 *
 * This function to tune sata tx Eq
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
SataTxEqPeiEntry (
    IN        CPM_PEI_FILE_HANDLE         FileHandle,
    IN        CPM_PEI_SERVICES            **PeiServices
)
{
    EFI_STATUS                      Status;
    UINT8    i;
    CPM_SATA_TX_EQ_PPI*              CpmSataTxEqPpi;
	EFI_PEI_PPI_DESCRIPTOR*          CpmSataTxEqPpiList;
	
	DEBUG ((EFI_D_ERROR,"Hygon Cpm Sata Tx Eq Entry\n"));
//1.install Sata Tx Eq ppi
    Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (CPM_SATA_TX_EQ_PPI),
                             &CpmSataTxEqPpi
                             );						
    ASSERT_EFI_ERROR (Status);
	//init ppi
	CpmSataTxEqPpi->Version   = CPM_SATA_TX_EQ_PPI_VERSION;
	CpmSataTxEqPpi->GetSataEq = CpmGetSataEq;
	CpmSataTxEqPpi->SetSataEq = CpmSetSataEq;
	
	Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             &CpmSataTxEqPpiList
                             );
    ASSERT_EFI_ERROR (Status);
    // Create the PPI descriptor
    CpmSataTxEqPpiList->Guid  = &gHygonSataTxEqPpiGuid;
    CpmSataTxEqPpiList->Ppi   = CpmSataTxEqPpi;
    CpmSataTxEqPpiList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
	//install ppi
	Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             CpmSataTxEqPpiList
                             );
	if(EFI_ERROR(Status)){
		return Status;
	}
    //2.init SATA EQ setting by SATA Tx Eq init table
    for(i=0;SataTxEqInitTable[i].Socket<2;i++)
	{
		CpmSataTxEqPpi->SetSataEq(CpmSataTxEqPpi,SataTxEqInitTable[i].Socket,SataTxEqInitTable[i].Die,SataTxEqInitTable[i].Port,SATA_GEN1,&SataTxEqInitTable[i].SataTxEqGen1);
		CpmSataTxEqPpi->SetSataEq(CpmSataTxEqPpi,SataTxEqInitTable[i].Socket,SataTxEqInitTable[i].Die,SataTxEqInitTable[i].Port,SATA_GEN2,&SataTxEqInitTable[i].SataTxEqGen2);
		CpmSataTxEqPpi->SetSataEq(CpmSataTxEqPpi,SataTxEqInitTable[i].Socket,SataTxEqInitTable[i].Die,SataTxEqInitTable[i].Port,SATA_GEN3,&SataTxEqInitTable[i].SataTxEqGen3);
	}
    DEBUG ((EFI_D_ERROR,"Hygon Cpm Sata Tx Eq Exit\n"));
    return EFI_SUCCESS;
}
