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
 ******************************************************************************
 */
/*++
Module Name:

  HygonRtcIntrDetDxe.c
  Init RtcIntrDet interfaceRtcIntrDetProtocol

Abstract:
--*/

#include "HygonRtcIntrDetDxe.h"

extern  EFI_BOOT_SERVICES       *gBS;

/**
 * NotifySmuRtcIntrDetReady is to notify SMU BIOS SMI handler ready
 *
 * @param[in]         VOID
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
VOID
NotifySmuRtcIntrDetReady (
  VOID
  )
{
  PCI_ADDR                             PciAddr;
  UINT32                               SmuArg[6];
  EFI_STATUS                           Status;
  
  DEBUG ((EFI_D_INFO, "NotifySmuRtcIntrDetReady Entry \n"));
  
  PciAddr.AddressValue = 0;
  NbioSmuServiceCommonInitArguments (SmuArg);
  Status = NbioSmuServiceRequest(PciAddr, 0, SMC_MSG_SmiHandlerReady, SmuArg, 0);
  
  DEBUG ((EFI_D_INFO, "NotifySmuRtcIntrDetReady Exit \n"));
}

/**
 * Configurate FCAC die id to logical Iodie0 for every Socket
 * 
 *  @param[in] Event      Event whose notification function is being invoked.
 *  @param[in] Context    Pointer to the notification function's context.
 */

VOID
RtcIntrDetExitBootServices (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  DEBUG ((EFI_D_INFO, "RtcIntrDetExitBS \n"));

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }
  
  //Notify SMU RTC intrusion Detection Ready
  NotifySmuRtcIntrDetReady ();
}

/**
  HygonRtcIntrDetDxeInit is to Publish RtcIntrDet Protocol
  @param  ImageHandle                   EFI Image Handle for the DXE driver
  @param  SystemTable                   pointer to the EFI system table
  @retval EFI_SUCCESS                   Module initialized successfully
  @retval EFI_ERROR                     Initialization failed  
**/
EFI_STATUS
EFIAPI
HygonRtcIntrDetDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  EFI_HANDLE                        Handle;
  EFI_DXE_RTC_INTR_DET_PROTOCOL     *RtcIntrDetProtocol;
  EFI_EVENT                         ExitBootServicesEvent;

  DEBUG ((EFI_D_INFO, "HygonRtcIntrDetDxeInit Entry \n"));

  if(PcdGetBool (PcdHygonRtcIntrDetEnable) == FALSE)
  {
    DEBUG ((EFI_D_INFO, "RtcIntrDet Disabled \n"));
    
    return Status;
  }

  // Create the Protocol
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (EFI_DXE_RTC_INTR_DET_PROTOCOL), &RtcIntrDetProtocol);

  if (EFI_ERROR (Status)) {
    return Status;
  } else {
    //clear instances content
    gBS->SetMem (RtcIntrDetProtocol, sizeof (EFI_DXE_RTC_INTR_DET_PROTOCOL), 0);
  }

  RtcIntrDetProtocol->ReadIntrDetSts = ReadIntrDetSts;
  RtcIntrDetProtocol->ClearIntrDetSts = ClearIntrDetSts;
  RtcIntrDetProtocol->GetRtcRamData = GetRtcRamData;
  RtcIntrDetProtocol->ClearRtcRamData = ClearRtcRamData;
  
  Handle = NULL;

  // Publish RtcIntrDet service Protocol
  Status = gBS->InstallProtocolInterface (
                &Handle,
                &gHygonRtcIntrDetProtocolGuid,
                EFI_NATIVE_INTERFACE,
                RtcIntrDetProtocol
                );
  ASSERT_EFI_ERROR (Status); 

  Status = gBS->CreateEventEx (
                EVT_NOTIFY_SIGNAL,
                TPL_NOTIFY,
                RtcIntrDetExitBootServices,
                NULL,
                &gEfiEventExitBootServicesGuid,
                &ExitBootServicesEvent
                );  
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "HygonRtcIntrDetDxeInit Exit \n"));

  return (Status);
}

/**
  Read Rtc Intrusion Detect Status
  @param  This                          Pointer to an EFI_DXE_RTC_INTR_DET_PROTOCOL structure.
  @param  IntrDetSts                    Pointer to IntrDetSts.
  @retval EFI_SUCCESS	                  Read Rtc Intrusion Detect Status completed.   
**/
EFI_STATUS
EFIAPI
ReadIntrDetSts (
  IN  EFI_DXE_RTC_INTR_DET_PROTOCOL     *This,
  OUT BOOLEAN                           *IntrDetSts
  )
{
  PMIO_REG5C    Pmio_Reg5C;
  
  //Read Rtc Intrusion Detect Status
  FchReadPmio (FCH_PMIOA_REG5C, AccessWidth8, &Pmio_Reg5C.Value);
  *IntrDetSts = Pmio_Reg5C.Field.IntrDetSts;
  
  DEBUG ((EFI_D_INFO, "IntrDetSts %d \n", *IntrDetSts));

  return EFI_SUCCESS;
}

/**
  Clear Rtc Intrusion Detect Status
  @param  This                          Pointer to an EFI_DXE_RTC_INTR_DET_PROTOCOL structure.
  @retval EFI_SUCCESS	                  Clear Rtc Intrusion Detect Status Successfully.   
**/
EFI_STATUS
EFIAPI
ClearIntrDetSts (
  IN  EFI_DXE_RTC_INTR_DET_PROTOCOL     *This
  )
{
  DEBUG ((EFI_D_INFO, "ClearIntrDetSts \n"));

  //Set IntrDetClr to clear IntrDetSts
  FchRwPmio (FCH_PMIOA_REG5C, AccessWidth8, ~(UINT32) BIT5, (UINT32) BIT5);

  return EFI_SUCCESS;
}

/**
  GetRtcRamData is to read Ram24
  @param  This                          Pointer to an EFI_DXE_RTC_INTR_DET_PROTOCOL structure.
  @param  Byte                          Pointer to Ram24 Data.
  @retval EFI_SUCCESS	                  Read Ram24 Successfully.   
**/
EFI_STATUS
EFIAPI
GetRtcRamData (
  IN  EFI_DXE_RTC_INTR_DET_PROTOCOL     *This,
  OUT RTCRAM_DATA                        *RtcRamData
  )
{

  DEBUG ((EFI_D_INFO, "GetRtcRamData \n"));

  //get IntrDet Ram data from PcdRtcRamData
  gBS->CopyMem (
	        RtcRamData,
	        PcdGetPtr (PcdRtcRamData),
	        PcdGetSize (PcdRtcRamData)
	);

  return EFI_SUCCESS;
}

/**
  ClearRtcRamData is to clear Ram24
  @param  This                          Pointer to an EFI_DXE_RTC_INTR_DET_PROTOCOL structure.
  @retval EFI_SUCCESS	                  Clear Ram24 Successfully.   
**/
EFI_STATUS
EFIAPI
ClearRtcRamData (
  IN  EFI_DXE_RTC_INTR_DET_PROTOCOL     *This
  )
{
  DEBUG ((EFI_D_INFO, "ClearRtcRamData \n"));

  //Set IntrDetRegClr to clear RAM24
  FchRwPmio (FCH_PMIOA_REG5C, AccessWidth8, ~(UINT32) BIT7, (UINT32) BIT7);

  return EFI_SUCCESS;
}
