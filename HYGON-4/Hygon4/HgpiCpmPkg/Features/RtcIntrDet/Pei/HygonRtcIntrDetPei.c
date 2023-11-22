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

  HygonRtcIntrDetPei.c
  Init RtcIntrDet interfaceRtcIntrDetPpi

Abstract:
--*/

#include "HygonRtcIntrDetPei.h"

/**
  HygonRtcIntrDetPeiInit is to Publish RtcIntrDet Ppi
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed 
**/
EFI_STATUS
EFIAPI
HygonRtcIntrDetPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  EFI_PEI_RTC_INTR_DET_PPI    *RtcIntrDetPpi;
  EFI_PEI_PPI_DESCRIPTOR      *PpiListRtcintrDet;
  
  DEBUG ((EFI_D_INFO, "HygonRtcIntrDetPeiInit Entry \n"));

  if(PcdGetBool (PcdHygonRtcIntrDetEnable) == FALSE)
  {
    DEBUG ((EFI_D_INFO, "Disable RtcIntrDet \n"));
    
    FchRwPmio (FCH_PMIOA_REG5C, AccessWidth8, ~(UINT32) BIT4, 0);

    return Status;
  }

  //If VDDBT_RTC_G has power down, clear ram24
  CheckRtcRam();

  // Read Ram24 and set PcdRtcRamData
  ReadRtcRam();

  //Create the PPIs
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_RTC_INTR_DET_PPI),
                             &RtcIntrDetPpi
                             );
  ASSERT_EFI_ERROR (Status);

  RtcIntrDetPpi->ReadIntrDetSts = ReadIntrDetSts;
  RtcIntrDetPpi->ClearIntrDetSts = ClearIntrDetSts;
  RtcIntrDetPpi->GetRtcRamData = GetRtcRamData;
  RtcIntrDetPpi->ClearRtcRamData = ClearRtcRamData;

  //Install the PPIs
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             &PpiListRtcintrDet
                             );
  ASSERT_EFI_ERROR (Status); 

  //Create the PPI descriptor
  PpiListRtcintrDet->Guid  = &gHygonRtcIntrDetPpiGuid;
  PpiListRtcintrDet->Ppi   = RtcIntrDetPpi;
  PpiListRtcintrDet->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);

  //Publish the PPI
  Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             PpiListRtcintrDet
                             );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "HygonRtcIntrDetPeiInit Exit \n"));

  return (Status);
}

/**
  Read Rtc Intrusion Detect Status
  @param  This                      Pointer to an EFI_PEI_RTC_INTR_DET_PPI structure.
  @param  IntrDetSts                Pointer to IntrDetSts.
  @retval EFI_SUCCESS	              Read Rtc Intrusion Detect Status completed.   
**/
EFI_STATUS
EFIAPI
ReadIntrDetSts (
  IN  EFI_PEI_RTC_INTR_DET_PPI     *This,
  OUT BOOLEAN                      *IntrDetSts
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
  @param  This                      Pointer to an EFI_PEI_RTC_INTR_DET_PPI structure.
  @retval EFI_SUCCESS	              Clear Rtc Intrusion Detect Status Successfully.   
**/
EFI_STATUS
EFIAPI
ClearIntrDetSts (
  IN  EFI_PEI_RTC_INTR_DET_PPI     *This
  )
{
  DEBUG ((EFI_D_INFO, "ClearIntrDetSts \n"));

  //Set IntrDetClr to clear IntrDetSts
  FchRwPmio (FCH_PMIOA_REG5C, AccessWidth8, ~(UINT32) BIT5, (UINT32) BIT5);

  return EFI_SUCCESS;
}

/**
  GetRtcRamData is to read Ram24
  @param  This                      Pointer to an EFI_PEI_RTC_INTR_DET_PPI structure.
  @param  RtcRamData                Pointer to RTCRAM_DATA.
  @retval EFI_SUCCESS	              Read Ram24 Successfully.   
**/
EFI_STATUS
EFIAPI
GetRtcRamData (
  IN  EFI_PEI_RTC_INTR_DET_PPI     *This,
  OUT RTCRAM_DATA                  *RtcRamData
  )
{

  DEBUG ((EFI_D_INFO, "GetRtcRamData \n"));
  
  //get IntrDet Ram data from PcdRtcRamData
  CopyMem (RtcRamData, PcdGetPtr (PcdRtcRamData), PcdGetSize (PcdRtcRamData));

  return EFI_SUCCESS;
}

/**
  ClearRtcRamData is to clear Ram24
  @param  This                      Pointer to an EFI_PEI_RTC_INTR_DET_PPI structure.
  @retval EFI_SUCCESS	              Clear Ram24 Successfully.   
**/
EFI_STATUS
EFIAPI
ClearRtcRamData (
  IN  EFI_PEI_RTC_INTR_DET_PPI     *This
  )
{
  DEBUG ((EFI_D_INFO, "ClearRtcRamData \n"));

  //Set IntrDetRegClr to clear RAM24
  FchRwPmio (FCH_PMIOA_REG5C, AccessWidth8, ~(UINT32) BIT7, (UINT32) BIT7);

  return EFI_SUCCESS;
}

/**
  CheckRtcRam is to check if VDDBT_RTC_G has power down
  @param  VOID                        VOID
  @retval EFI_SUCCESS	                Clear Ram24 Successfully.   
**/
EFI_STATUS
CheckRtcRam (
  VOID
  )
{
   
  //Check RAM24_BATTERY_FLAG
	if (IsRam24BatteryPowerDown()) {
    
    DEBUG ((EFI_D_INFO, "Clear IntrDetSts and RAM24\n"));
    
    //Set IntrDetClr to clear IntrDetSts and Set IntrDetRegClr to clear RAM24
    FchRwPmio (FCH_PMIOA_REG5C, AccessWidth8, ~(UINT32) (BIT5 + BIT7), (UINT32) (BIT5 + BIT7));
    
    //Set RAM24_BATTERY_FLAG to 0x55
    IoWrite8(FCH_IOMAP_REG72, PcdGet8(PcdRam24BatteryCheckAddress));
    IoWrite8(FCH_IOMAP_REG73, RAM24_BATTERY_FLAG);
  }

  return EFI_SUCCESS;
}

/**
  ReadRtcRam is to read Ram24
  @param[in,out]  VOID
  @retval     VOID
**/
VOID 
ReadRtcRam (
  VOID
  )
{
  RTCRAM_DATA        DefaultData = {{0,0,0,0,0,0}};
  RTCRAM_DATA        *RtcRamData = &DefaultData;
  UINTN              SizeofBuffer = 0;
  
  ReadRtcRam24((UINT8*)RtcRamData);
  
  DumpRtcRam24(RtcRamData);

  //Set PcdRtcRamData
  SizeofBuffer = sizeof(RTCRAM_DATA);
  PcdSetPtrS (PcdRtcRamData, &SizeofBuffer, (VOID*)RtcRamData);
}

/**
  IsRam24BatteryPowerDown is to check if VDDBT_RTC_G has power down by RAM24_BATTERY_FLAG
  @param[out]  VOID
  @retval      True           Ram24 Battery has PowerDown
               False          Ram24 Battery has not PowerDown
**/
BOOLEAN
IsRam24BatteryPowerDown(
  VOID
)
{
  //Check RAM24_BATTERY_FLAG
  IoWrite8(FCH_IOMAP_REG72, PcdGet8(PcdRam24BatteryCheckAddress));
  return (IoRead8(FCH_IOMAP_REG73) != RAM24_BATTERY_FLAG);
}
