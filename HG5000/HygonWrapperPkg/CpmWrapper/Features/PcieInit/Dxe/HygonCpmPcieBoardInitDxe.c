/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM PCIE Initialization
 *
 * Contains CPM code to perform PCIE initialization under DXE
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  PcieInit
 * @e \$Revision$   @e \$Date$
 *
 */
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

#include <AmdCpmDxe.h>
#include <Library/PcdLib.h>
#include <AmdBoardId.h>
#include <Token.h>
#include <GnbRegistersZP.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <protocol/PciIo.h>
#include <protocol/PciRootBridgeIo.h>
#include <IndustryStandard/PciExpress21.h>

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM PCIE Init DXE driver
 *
 * This function loads, overrides and installs Express Card SSDT table.
 *
 * @param[in]     ImageHandle    Pointer to the firmware file system header
 * @param[in]     SystemTable    Pointer to System table
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */

EFI_STATUS
EFIAPI
HygonCpmPcieBoardInitDxeEntryPoint (
  IN      EFI_HANDLE                      ImageHandle,
  IN      EFI_SYSTEM_TABLE                *SystemTable
  )
{
 EFI_STATUS 							 Status;
 PCIe_PLATFORM_CONFIG					 *Pcie;
 DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL	 *PcieServicesProtocol;
 GNB_PCIE_INFORMATION_DATA_HOB			 *PciePlatformConfigHobData;
 GNB_HANDLE 							 *NbioHandle;
 UINT32 								 Address;
 PCIe_ENGINE_CONFIG 					 *EngineList;
 PCIe_WRAPPER_CONFIG					 *Wrapper;
 UINT32	  HeaderType;
 PCI_REG_PCIE_SLOT_CAPABILITY  SlotCap;
 UINT8	   Dev;
 UINT8	   Fun;
 UINT32 				   HotPlugSlotNum =1 ;
 UINT32 				   SlotNum=0x3f;
 BOOLEAN    IsMutiFunction;
 
 DEBUG((DEBUG_INFO,"HygonCpmPcieBoardInitDxeEntryPoint Entry\n"));
 Wrapper = (PCIe_WRAPPER_CONFIG*) NULL;
 EngineList = (PCIe_ENGINE_CONFIG*) NULL;

 Status = gBS->LocateProtocol(
			&gAmdNbioPcieServicesProtocolGuid,
			NULL,
			&PcieServicesProtocol
		  );
 if(Status != EFI_SUCCESS){
   DEBUG((DEBUG_INFO,"LocalProtocol fail\n"));
 }
  
 if(Status == EFI_SUCCESS){
   Status = PcieServicesProtocol->PcieGetTopology(PcieServicesProtocol,(UINT32 **)&PciePlatformConfigHobData);
   if(Status != EFI_SUCCESS)
   {
	  return Status;
   }
   Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
   NbioHandle = NbioGetHandle(Pcie);
   while(NbioHandle != NULL){
	for(Dev = 0 ;Dev < 32; Dev++){
	  IsMutiFunction = FALSE;
	  for(Fun = 0; Fun < 7 ;Fun++){
		Address = MAKE_SBDFO(0,0,Dev,Fun,0xc); 
		NbioRegisterRead(NbioHandle,TYPE_PCI,Address,&HeaderType,0);
		if(Fun == 0) {
			if((HeaderType != 0xFFFFFFFF) && (HeaderType & BIT23))IsMutiFunction = TRUE;
		}
		if(Fun && IsMutiFunction == FALSE)break;
		if(HeaderType == 0xFFFFFFFF) continue;
		
		DEBUG((DEBUG_INFO,"HeaderType %x Dev %x Fun %x\n",HeaderType,Dev,Fun));
		if(HeaderType & BIT16){  //P2P bridge
		  Address = MAKE_SBDFO(0,0,Dev,Fun,0x6c); 
		  NbioRegisterRead(NbioHandle,TYPE_PCI,Address,&SlotCap,0);
		  if(SlotCap.Bits.HotPlugCapable == 1){
		  	  DEBUG((DEBUG_INFO,"hotplug en slot num %x\n",HotPlugSlotNum));
			  SlotCap.Bits.PhysicalSlotNumber = HotPlugSlotNum;
			  NbioRegisterWrite(NbioHandle,TYPE_PCI,Address,&SlotCap,0);
			  HotPlugSlotNum++;
		  }else{
		      DEBUG((DEBUG_INFO,"hotplug not en slot num %x\n",SlotNum));
			  SlotCap.Bits.PhysicalSlotNumber = SlotNum;
			  NbioRegisterWrite(NbioHandle,TYPE_PCI,Address,&SlotCap,0);
			  SlotNum++;
		  } 			  
		} 
	  } //for fun
	}//for Dev
	 NbioHandle = GnbGetNextHandle (NbioHandle);
   }
 }
return EFI_SUCCESS;  
}

