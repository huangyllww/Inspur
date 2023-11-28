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
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Ppi/Pca9555aPpi.h>
#include <Ppi/Stall.h>

#define PCA9555A_INPUT0      0
#define PCA9555A_INPUT1      1
#define PCA9555A_OUTPUT0     2
#define PCA9555A_OUTPUT1     3
#define PCA9555A_CONFIG0     6
#define PCA9555A_CONFIG1     7

#define PCA9555A_PRESENT       BIT0
#define PCA9555A_PWRFAULT      BIT1
#define PCA9555A_ATTENTION     BIT2
#define PCA9555A_EMILS         BIT3
#define PCA9555A_POWER_EN      BIT4
#define PCA9555A_ATTENTION_LED BIT5
#define PCA9555A_POWER_LED     BIT6
#define PCA9555A_EMIL          BIT7

EFI_STATUS PeiPatchPac9555PowerenableCallback(
        IN EFI_PEI_SERVICES             **PeiServices,
        IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
        IN VOID                         *InvokePpi
);
//-----------------------------------------------------------------------------

static EFI_PEI_NOTIFY_DESCRIPTOR PeiNotifyList9555A[] = {
        { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
                &gPca9555aPpiGuid, PeiPatchPac9555PowerenableCallback }
};

EFI_STATUS PeiPatchPac9555PowerenableCallback(
        IN EFI_PEI_SERVICES             **PeiServices,
        IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
        IN VOID                         *InvokePpi
)
{
    EFI_STATUS               Status;
    EFI_PEI_PCA9555A_PPI     *Pca9555APpi;
    UINT16                   Data;
    UINTN                    SlaveAddress;
    UINT8                    Command;
    EFI_BOOT_MODE            BootMode;

    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    if(BootMode == BOOT_ON_S3_RESUME) {
        return Status;
    }

    Status = (*PeiServices)->LocatePpi (
            PeiServices,
            &gPca9555aPpiGuid,
            0,
            NULL,
            &Pca9555APpi
    );
    if(EFI_ERROR(Status)) {
        return Status;
    }

    SlaveAddress = 0x23;
    Data = 0;
    
    //bit0~3 = input, bit4~7 = output
    Data |= (PCA9555A_PRESENT | PCA9555A_PWRFAULT | PCA9555A_ATTENTION | PCA9555A_EMILS); 
    Command = PCA9555A_CONFIG0;
    Status = Pca9555APpi->Set(PeiServices, 0, SlaveAddress, Command, Data);
    if(EFI_ERROR(Status)) return Status;
    
    Command = PCA9555A_INPUT0;
    Status = Pca9555APpi->Get(PeiServices, 0, SlaveAddress, Command, &Data);
    DEBUG((DEBUG_ERROR, "Get PCA9555A Slave:%x Register:%d Data:%02x Status:%r\n", SlaveAddress, Command, Data, Status));
    if(EFI_ERROR(Status)) return Status;

    //if 9555 PCIE slot not empty, then set PCIE power_enable to low
    if(((Data & PCA9555A_PRESENT)==0) && ((Data & PCA9555A_POWER_EN)==PCA9555A_POWER_EN) ||
       (PcdGetBool(PcdCfgPcieHotplugSupport) == FALSE))
    {
        Data &= (~BIT4);
        Command = PCA9555A_OUTPUT0;
        Status = Pca9555APpi->Set(PeiServices, 0, SlaveAddress, Command, Data);
        DEBUG((DEBUG_ERROR, "Set PCA9555A Slave:%x Register:%d Data:%02x Status:%r\n", SlaveAddress, Command, Data, Status));
    }
    return Status;
}

EFI_STATUS
Pca9555AWorkaroundInit(
        IN       EFI_PEI_FILE_HANDLE   FileHandle,
        IN CONST EFI_PEI_SERVICES     **PeiServices)
{
    EFI_STATUS    Status;

    Status = (*PeiServices)->NotifyPpi(PeiServices, PeiNotifyList9555A);

    return Status;
}
