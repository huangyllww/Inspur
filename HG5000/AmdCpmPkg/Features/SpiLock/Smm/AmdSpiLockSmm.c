/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM GPIO Initialization
 *
 * Contains CPM code to perform GPIO Init in SMM
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  GpioInit
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

#include <AmdCpmSmm.h>
#include <Library/AmdPspMboxLibV2.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SmmBase2.h>
#include <Library/PciLib.h>
#include <Protocol/PspMboxSmmBufferAddressProtocol.h>


EFI_STATUS
SmmSpiUnlock(VOID)
{
    EFI_STATUS   Status;
	BOOLEAN        *InSmmFlag =NULL;
	PSP_MBOX_SMMBUFFER_ADDRESS_PROTOCOL *PspMboxSmmBufferAddressProtocol;

	DEBUG((EFI_D_INFO,"SmmSpiUnlock Entry\n"));
	Status = gSmst->SmmLocateProtocol (&gPspMboxSmmBufferAddressProtocolGuid, NULL, &PspMboxSmmBufferAddressProtocol);
	InSmmFlag = PspMboxSmmBufferAddressProtocol->PspMboxSmmFlagAddr;
	if(InSmmFlag == NULL) return EFI_UNSUPPORTED;

	*InSmmFlag = TRUE;	
    Status = PspMboxBiosUnLockSpi();
	*InSmmFlag = FALSE;
    if (Status != EFI_SUCCESS){
        return EFI_DEVICE_ERROR;
    }
    DEBUG((EFI_D_INFO,"SmmSpiUnlock Exit\n"));
    return EFI_SUCCESS;
}

EFI_STATUS
SmmSpilock(VOID)
{
	PciAndThenOr32 ((LPC_BUS_DEV_FUN << 16) + LPC_REG50, ~(UINT32) BIT10, 1 << 10);
	PciAndThenOr32 ((LPC_BUS_DEV_FUN << 16) + LPC_REG54, ~(UINT32) BIT10, 1 << 10);
	PciAndThenOr32 ((LPC_BUS_DEV_FUN << 16) + LPC_REG58, ~(UINT32) BIT10, 1 << 10);
	PciAndThenOr32 ((LPC_BUS_DEV_FUN << 16) + LPC_REG5C, ~(UINT32) BIT10, 1 << 10);
    return EFI_SUCCESS;
}

EFI_STATUS
SmmSpiUnlockInitEntryPoint(
	IN		 EFI_HANDLE 		ImageHandle,
	IN		 EFI_SYSTEM_TABLE	*SystemTable

)
{
	FCH_SMM_SPI_LOCK_PROTOCOL  *FchSmmSpilockProtocol;
	EFI_HANDLE					FchSmmInitHandle;
	EFI_STATUS                  Status;

    if(!PcdGetBool(PcdFchSpiLockSupport)){
        return EFI_SUCCESS;
	}
    
    Status = gSmst->SmmAllocatePool (
          EfiRuntimeServicesData,
          sizeof(FCH_SMM_SPI_LOCK_PROTOCOL),
          &FchSmmSpilockProtocol
    );   
	if (EFI_ERROR (Status)) {
	  return (Status);
	}

    FchSmmSpilockProtocol->Revision     = 0x1;
    FchSmmSpilockProtocol->SmmSpiUnlock = SmmSpiUnlock;
	FchSmmSpilockProtocol->SmmSpilock = SmmSpilock;
    FchSmmInitHandle =  NULL;
    Status = gSmst->SmmInstallProtocolInterface (
                    &FchSmmInitHandle,
                    &gFchSmmlockProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    FchSmmSpilockProtocol
                    );
  if (EFI_ERROR (Status)) {
	return Status;
  }
  return EFI_SUCCESS;
}

