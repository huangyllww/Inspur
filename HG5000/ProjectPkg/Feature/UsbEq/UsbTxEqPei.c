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
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include "UsbTxEqPei.h"
#include "UsbReg.h"
#include <Library/IoLib.h>


 USB20_TX_EQ_INIT_TABLE USB20TxEqInitTable[]={
	//socket  | Die   |Link  |swing  | preemphasis
	//        |       |      |
	{0,        1,      0,     7,      3},//Socket 0 Die 0 Port 0 //J151
	{0,        1,      1,     7,      3},//Socket 0 Die 0 Port 1 //J151
	{0,        0,      2,     0xe,      3},//Socket 0 Die 1 Port 2 //J152
	{0,        0,      3,     0xe,      3},//Socket 0 Die 1 Port 3 //HUB
    //Table end	
	{0xFF,0,0,0,0}
 };
USB30_TX_EQ_INIT_TABLE USB30TxEqInitTable[]={
   //socket  | Die	 |Link	|vboost	| Peemph_mode 
   //		 |		 |		|       |
   {0,		  0,	  0,	 3, 	 0x2c },//Socket 0 Die 0 Port 4
   {0,		  0,	  1,	 3, 	 0x2c },//Socket 0 Die 0 Port 5   
   //{0,		  1,	  2,	 0x2C, 	 0xff },//Socket 0 Die 0 Port 6
   //{0,		  1,	  3,	 0x2C, 	 0xff },//Socket 0 Die 0 Port 7
   //Table end 
   {0xFF,0,0,0,0}
};

EFI_STATUS
CpmSetUsb20Eq(
	IN		  CPM_PEI_SERVICES			  **PeiServices,
	IN        UINT8           Socket,
    IN        UINT8           Die,
    IN        UINT8           Link,
    IN        UINT8           Swing,
    IN        UINT8           Preemphasis
)
{
	EFI_STATUS         Status;
	UINT32             SmnAddr;
	LANEPARACTL0	   LaneCtl;
	UINT16             Offset;
    AMD_CPM_TABLE_PPI  *CpmTablePpiPtr;
	UINT32            Debug1;

    Status = (*PeiServices)->LocatePpi (
                                      PeiServices,
                                      &gAmdCpmTablePpiGuid,
                                      0,
                                      NULL,
                                      (VOID**)&CpmTablePpiPtr
                                      );
    if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR,"CpmGetUSBEq:locate gAmdCpmTablePpiGuid Fail!!!\n"));
        return  Status;
    }    
    //calculate lan coff address by speed and port
    Offset = 4*Link;
    SmnAddr = USBLANCTRL_SMN_ADDRESS_BASE + (Offset << 8);
	DEBUG ((EFI_D_ERROR,"CpmGetUSBEq:Socket %x die %x SmnAddress:%x",Socket,Die,SmnAddr));
	DEBUG ((EFI_D_ERROR,"swing:%x Preemphasis %x\n",Swing,Preemphasis));
    LaneCtl.Data32 = CpmTablePpiPtr->CommonFunction.SmnRead32 (CpmTablePpiPtr,Socket,Die,SmnAddr,0);
	Debug1 = CpmTablePpiPtr->CommonFunction.SmnRead32 (CpmTablePpiPtr,Socket,Die,0x1240B550,0);
	DEBUG ((EFI_D_ERROR,"Read Reg Value: %x debug %x",LaneCtl.Data32,Debug1));
	LaneCtl.Reg.TXVREFTUNE= Swing;
	LaneCtl.Reg.TXPREEMPAMPTUNE= Preemphasis;
	DEBUG ((EFI_D_ERROR,"Write Reg Value: %x\n",LaneCtl.Data32));
	CpmTablePpiPtr->CommonFunction.SmnWrite32(CpmTablePpiPtr,Socket,Die,SmnAddr,0,LaneCtl.Data32);
	LaneCtl.Data32 = CpmTablePpiPtr->CommonFunction.SmnRead32 (CpmTablePpiPtr,Socket,Die,SmnAddr,0);
	DEBUG ((EFI_D_ERROR,"finish Reg Value: %x\n",LaneCtl.Data32));
    return EFI_SUCCESS;
}

EFI_STATUS
CpmSetUsb30Eq(
    IN        CPM_PEI_SERVICES            **PeiServices,
    IN        UINT8           Socket,
    IN        UINT8           Die,
    IN        UINT8           Link,
    IN        UINT8           Vboost,
    IN        UINT8           PeemphValue 
)
{
    EFI_STATUS         Status;
    UINT32             SmnAddr;
	PHYPARACTL2        PhyCtl2;
    TXOVRDDEVLO        TxOverDevLo;
    UINT16             Offset;

    //EFI_PEI_SERVICES   **PeiServices;
    AMD_CPM_TABLE_PPI    *CpmTablePpiPtr;

    //PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();

    Status = (*PeiServices)->LocatePpi (
                                      PeiServices,
                                      &gAmdCpmTablePpiGuid,
                                      0,
                                      NULL,
                                      (VOID**)&CpmTablePpiPtr
                                      );
    if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR,"CpmGetUSBEq:locate gAmdCpmTablePpiGuid Fail!!!\n"));
        return  Status;
    }    
    //calculate lan coff address by speed and port
    //set vboost
    if(Vboost != 0xff){
        Offset = 4*Link;
        SmnAddr = PHYPARACTL2_LINK0_SMN_ADDRESS + (Offset << 8);
        DEBUG ((EFI_D_ERROR,"CpmGetUSB30Eq:Socket %x die %x SmnAddress:%x",Socket,Die,SmnAddr));
        DEBUG ((EFI_D_ERROR,"vboost:%x\n",Vboost));
        PhyCtl2.Data32 = CpmTablePpiPtr->CommonFunction.SmnRead32 (CpmTablePpiPtr,Socket,Die,SmnAddr,0);
        DEBUG ((EFI_D_ERROR,"Read Reg Value: %x",PhyCtl2.Data32));
        PhyCtl2.Reg.tx_vboost_lvl= Vboost;
        DEBUG ((EFI_D_ERROR,"Write Reg Value: %x\n",PhyCtl2.Data32));
        CpmTablePpiPtr->CommonFunction.SmnWrite32(CpmTablePpiPtr,Socket,Die,SmnAddr,0,PhyCtl2.Data32);
    }
    if(PeemphValue != 0xff){
        Offset = 4*Link;
        SmnAddr = USBTX_OVRD_DRV_LO_LINK0_ADDRESS + (Offset << 8);
		TxOverDevLo.Data32 = CpmTablePpiPtr->CommonFunction.SmnRead32 (CpmTablePpiPtr,Socket,Die,SmnAddr,0);
		TxOverDevLo.Reg.AMPLITUDE = 0x7f;
		TxOverDevLo.Reg.EN = 1;
		TxOverDevLo.Reg.PREEMPH = PeemphValue;
		CpmTablePpiPtr->CommonFunction.SmnWrite32(CpmTablePpiPtr,Socket,Die,SmnAddr,0,TxOverDevLo.Data32);
	}
    return EFI_SUCCESS;
    
} 


#define PCI_DEV_MMBASE(Bus, Device, Function) \
    ( \
      (UINTN)PcdGet64(PcdPciExpressBaseAddress) + (UINTN) (Bus << 20) + (UINTN) (Device << 15) + (UINTN) \
        (Function << 12) \
    )
/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the USB Tx Eq driver
 *
 * This function to tune USB tx Eq
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
UsbTxEqPeiEntry (
    IN        CPM_PEI_FILE_HANDLE         FileHandle,
    IN        CPM_PEI_SERVICES            **PeiServices
)
{
    UINT8    i = 0;

	
	DEBUG ((EFI_D_ERROR,"Hygon Cpm USB Tx Eq Entry\n"));
    //setting USB EQ
  /*  while(USB20TxEqInitTable[i].Socket != 0xff)
	{
		CpmSetUsb20Eq(PeiServices,USB20TxEqInitTable[i].Socket,USB20TxEqInitTable[i].Die,USB20TxEqInitTable[i].Link,USB20TxEqInitTable[i].TxVREFTune,USB20TxEqInitTable[i].TxPreEmpampTune);
        i++;
	}
	i = 0;*/ 
    while(USB30TxEqInitTable[i].Socket != 0xff)
	{
		CpmSetUsb30Eq(PeiServices,USB30TxEqInitTable[i].Socket,USB30TxEqInitTable[i].Die,USB30TxEqInitTable[i].Link,USB30TxEqInitTable[i].vboost,USB30TxEqInitTable[i].PeemphValue);
        i++;
	}	

    DEBUG ((EFI_D_ERROR,"Hygon Cpm USB Tx Eq Exit\n"));
    return EFI_SUCCESS;
}
