/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 17
 *
 * Contains code that generate SMBIOS type 17
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Universal
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
#include "AmdSmbios.h"
#include "AMD.h"
#include "Porting.h"
#include "Filecode.h"
#include "Library/IdsLib.h"
#include "Library/BaseLib.h"
#include <Library/AmdBaseLib.h>
#include "Library/MemChanXLat.h"
#include <MemDmi.h>
#include "Library/UefiBootServicesTableLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Protocol/Smbios.h"
#include "Protocol/AmdSmbiosServicesProtocol.h"
#include "Protocol/FabricTopologyServices.h"
#include "AmdSmbiosDxe.h"
#include "PiDxe.h"
#include <Token.h>
#include "JEP106.h"
#include <Ipmi/IpmiNetFnStorageDefinitions.h>
#include <Library\IpmiBaseLib.h>
#include <IndustryStandard\IpmiNetFnStorage.h>



#include <Library/PciLib.h>
#define DIE_PER_SCKT 4
#define NB_SMN_INDEX_EXT_3_BIOS  0x00C0
#define NB_SMN_INDEX_3_BIOS  0x00C4
#define NB_SMN_DATA_3_BIOS   0x00C8
#define FILECODE UNIVERSAL_SMBIOS_AMDSMBIOSTYPE17_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
 #define MANUFACTURER_STRING_LENGTH             0x30

typedef struct {
  UINT16 DimmVendorWord;
  CONST CHAR8 *DimmVendorString;
} MANUFACTURER_STRING_PAIR;
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
ConvertManufacturer (
  IN     UINT8                                  MemoryType,
  IN     UINT64                                 ManufacturerIdCode,
     OUT UINT8                                  *Manufacturer
  );

/* -----------------------------------------------------------------------------*/
/**
 *  DoubleMemSpeed
 *
 *  Description:
 *     Reports MT/s instead MHz for memory speed in compliance with SMBIOS 3.1.1
 *
 *  Parameters:
 *    @param[in]        Speed - Memory Speed
*
 *    @retval          DoubleSpeed
 *
 */
UINT16
STATIC
DoubleMemSpeed (
  IN       UINT16 Speed
  )
{

  UINT16 DoubleSpeed = 0;

  if(Speed !=0) {
    if (Speed == 1600) {
      DoubleSpeed = 3200;              // DDR4-3200
    } else if (Speed == 1467) {
      DoubleSpeed = 2933;              // DDR4-2933
    } else if (Speed == 1333) {
      DoubleSpeed = 2666;              // DDR4-2666
    } else if (Speed == 1200) {
      DoubleSpeed = 2400;              // DDR4-2400
    } else if (Speed == 1067) {
      DoubleSpeed = 2133;              // DDR4-2133
    } else if (Speed == 933) {
      DoubleSpeed = 1866;              // DDR4-1866
    } else if (Speed == 800) {
      DoubleSpeed = 1600;              // DDR4-1600
    } else if (Speed == 667) {
      DoubleSpeed = 1333;              // DDR4-1333
    } else {
      DoubleSpeed = 0;
    }
  }
  return DoubleSpeed;
}

VOID
NbSmnRead (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN       UINT32          *Value
  )
{
    UINT32 read_value=0;
    
    PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, DieNum);
    PciWrite32(NB_SMN_INDEX_3_BIOS, Address);
    read_value=PciRead32(NB_SMN_DATA_3_BIOS);
    *Value=read_value;
    
    //clear in case other functions don't pay attention to this reg
    PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, 0);
}

BOOLEAN
IsMemTrained (
  IN      UINT8                     CurSocket,
  IN      UINT8                     Channel
  )

  {
    UINT32                              DieNum;
    UINT32                              Address;
    UINT32                              IsTrained;
    UINT32                              DieCount;
    UINTN                               NumberOfInstalledProcessors;
    UINTN                               NumberOfDie;
    UINT32                              DieNumberOfPerSocket;
    UINTN                               IgnoredRootBridges;
    AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;
    HOST_TO_APCB_CHANNEL_XLAT           *ApcbToBoardChanXLatTab;
    UINT8                               Index = 0;
    UINT8                               HostChannel;
    BOOLEAN                             ValidChannel = FALSE;
        
    // Locate FabricTopologyServicesProtocol
    gBS->LocateProtocol (&gAmdFabricTopologyServicesProtocolGuid, NULL, (VOID **) &FabricTopology);
    FabricTopology->GetSystemInfo (FabricTopology, &NumberOfInstalledProcessors, &NumberOfDie, &IgnoredRootBridges);
    
    DieNumberOfPerSocket = (UINT32)(NumberOfDie / NumberOfInstalledProcessors);
    
    // Get ApcbToBoard channel mapping table
    ApcbToBoardChanXLatTab = (HOST_TO_APCB_CHANNEL_XLAT*)PcdGetPtr(PcdApcbToBoardChanXLatTab);
    
    // Calculate which Die the channel is on
    while (ApcbToBoardChanXLatTab[Index].TranslatedChannelId != 0xFF) {
        if (Channel == ApcbToBoardChanXLatTab[Index].TranslatedChannelId) {
            HostChannel = Index;
            DieNum = CurSocket * DieNumberOfPerSocket + Index / MAX_CHANNELS_PER_DIE;
            Address = ((HostChannel % 2) == 0) ? 0x50104 : 0x150104;
            ValidChannel = TRUE;
            break;
        }
        Index++;
        ValidChannel = FALSE;
    }
    
    if (!ValidChannel) {
        IDS_HDT_CONSOLE_PSP_TRACE ("Error Channel %d\n", Channel);
        ASSERT(FALSE);
    }
    
    //*** Get Channel training status start ***//
    NbSmnRead (0, 0x1C604, &DieCount);
    DieCount = DieCount & 0xF; 
    if(DieCount >= DieNum){
        NbSmnRead(DieNum, Address, &IsTrained);
        IsTrained = IsTrained & 0x80000000;
        return IsTrained>>31;
    }else
       return 0;
    //*** Get Channel training status end ***//   
        
  }

EFI_STATUS
EfiSmGetTimeStamp (
  OUT UINT32 *NumOfSeconds
  );


VOID
DimmNoTrainingToBmc(UINT8 Socket,UINT8 Channel,UINT8 Dimm)
{
    EFI_STATUS					          Status = EFI_SUCCESS;
    IPMI_SEL_EVENT_RECORD_DATA	          *SelRecord=NULL;
	UINT32	                              TimeStamp = 0;
    UINT16                                MyGeneratorId;
    UINT8                                 MySensorNumber;
    UINT8                                 ResponseBuff[16]={0};
    UINT8                                 ResponseSize;
	UINT8                                 ResponseData[10]={0};
    UINT8                                 ResponseDataSize = 4;
	UINTN                                 DataSize;
		
	EfiSmGetTimeStamp (&TimeStamp);
		

    EfiInitializeIpmiBase ();

    DataSize = sizeof(IPMI_SEL_EVENT_RECORD_DATA);
    ResponseSize = sizeof (ResponseBuff);
    Status = EfiSendCommandToBMC (
					  0x6,
					  0x1,
					  (UINT8 *)NULL,
					  0,
					  (UINT8 *)&ResponseBuff,
					  (UINT8 *)&ResponseSize
					  );

     if(ResponseBuff[2]>3)//AMI BMC
     {
        MyGeneratorId = 0x21;
        MySensorNumber= 0xF9;
     }else{
        MyGeneratorId = 1;
        MySensorNumber = 0xA2;
     }
					
	SelRecord->TimeStamp = TimeStamp;
	SelRecord->RecordType = IPMI_SEL_SYSTEM_RECORD;
	SelRecord->GeneratorId = MyGeneratorId;//(UINT16)EFI_GENERATOR_ID(SMI_HANDLER_SOFTWARE_ID);
	SelRecord->EvMRevision = IPMI_EVM_REVISION;
	SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
					
	SelRecord->SensorType = 0x0C; // Memory
	SelRecord->SensorNumber = MySensorNumber;//ByoBMC define;
					//common data
	SelRecord->OEMEvData1 = ((EFI_OEM_SPECIFIC_DATA << 6) | (EFI_OEM_SPECIFIC_DATA << 4));
	SelRecord->OEMEvData2 = 0xff;
					
	SelRecord->OEMEvData3 = (Socket << 5)//CPU Socket Number
							|(Channel<<2)//Channel Number
							|(Dimm);//DIMM Number
	SelRecord->OEMEvData1 |= 0x01;

	IDS_HDT_CONSOLE(MAIN_FLOW,"Send Sel Recorder is:%x %x %x %x %x %x %x %x %x %x %x\n",SelRecord->RecordId,SelRecord->RecordType,SelRecord->TimeStamp,SelRecord->GeneratorId,SelRecord->EvMRevision,SelRecord->SensorType,SelRecord->SensorNumber,SelRecord->EventDirType,SelRecord->OEMEvData1,SelRecord->OEMEvData2,SelRecord->OEMEvData3);


  
  IDS_HDT_CONSOLE(MAIN_FLOW, "SendErrorLogDataToBmc \n");

  Status = EfiSendCommandToBMC (
            0x0A,
            0x44,
            (UINT8 *) SelRecord,
            (UINT8) DataSize,
            (UINT8 *) &ResponseData,
            (UINT8 *) &ResponseDataSize
            );

}



/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate SMBIOS type 17
 *
 *  Parameters:
 *    @param[in]       Smbios                       Pointer to EfiSmbiosProtocol
 *    @param[in]       MemDmiInfo                   Pointer to Memory DMI information
 *    @param[in]       Socket                       Socket number
 *    @param[in]       Channel                      Channel number
 *    @param[in]       Dimm                         Dimm number
 *    @param[in]       MemoryArrayHandle            Handle of the array where the device is mapped to
 *    @param[in]       MemoryErrorInfoHandle        Handle of the device where error is detected
 *    @param[out]      MemoryDeviceHandle           Handle of the current device
 *    @param[in]       StdHeader                    Handle to config for library and services
 *
 *    @retval          EFI_SUCCESS                  The Type 17 entry is added successfully.
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
AmdAddSmbiosType17 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       UINT8                 Socket,
  IN       UINT8                 Channel,
  IN       UINT8                 Dimm,
  IN       EFI_SMBIOS_HANDLE     MemoryArrayHandle,
  IN       EFI_SMBIOS_HANDLE     MemoryErrorInfoHandle,
     OUT   EFI_SMBIOS_HANDLE    *MemoryDeviceHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINTN                             StructureSize;
  UINTN                             StringSize;
  UINTN                             TotalSize;
  EFI_STATUS                        Status;
  EFI_STATUS                        CalledStatus;
  AMD_SMBIOS_TABLE_TYPE17          *SmbiosTableType17;
  EFI_SMBIOS_STRING                *StrPtr;
  CHAR8                             Manufacturer[MANUFACTURER_STRING_LENGTH];
  CHAR8                             DevWidthStr[4];                   // wz200505 +
  BOOLEAN                           MemPresent;                       // wz200505 +
  UINT8                             DevWidth;                         // wz200505 +
  DMI_T17_TYPE_DETAIL               EmptyTypeDetail = {0};
  BOOLEAN                           IsDmi3_2 = FALSE;                 // winddy +
  UINT8                             DimmErrorValue;                   // XCL 20201214 +

// ASSUME smbios 3.1

  Status = EFI_SUCCESS;

// winddy + >>  
  if (AmdSmbiosVersionCheck (Smbios, 3, 2)) {
    IsDmi3_2 = TRUE;
  }
// winddy + <<

  IDS_HDT_CONSOLE(MAIN_FLOW,"Socket:%d Channel:%d Dimm:%d\n",Socket,Channel,Dimm);

  // Generate DMI type 17 --- Start

  if (MemDmiInfo->T17[Socket][Channel][Dimm].MemoryType != 0) {
  // Calculate size of DMI type 17
    ConvertManufacturer (MemDmiInfo->T17[Socket][Channel][Dimm].MemoryType,
                         MemDmiInfo->T17[Socket][Channel][Dimm].ManufacturerIdCode,
                         Manufacturer);

// winddy - >>    
    if(IsDmi3_2){
      StructureSize = sizeof (AMD_SMBIOS_TABLE_TYPE17);
    } else {
      StructureSize = sizeof (AMD_SMBIOS_TABLE_TYPE17_31);
    }
// winddy - <<    
	
    TotalSize = StructureSize + sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].DeviceLocator);
    TotalSize += sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].BankLocator);
    TotalSize += sizeof (Manufacturer);
    TotalSize += sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].SerialNumber);
    TotalSize += sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].PartNumber);
    TotalSize += sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].AssetTag);
//add smbios 3.2 support 
    if (IsDmi3_2) {                     // winddy -
      TotalSize += sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].FirmwareVersion);
    }
    TotalSize += sizeof(DevWidthStr);          // wz200505 + 
    ++TotalSize; // Additional null (00h), End of strings

    // Allocate zero pool
    SmbiosTableType17 = NULL;
    SmbiosTableType17 = AllocateZeroPool (TotalSize);
    if (SmbiosTableType17 == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
	

    SmbiosTableType17->Hdr.Handle                        = AMD_SMBIOS_HANDLE_PI_RESERVED;
    SmbiosTableType17->Hdr.Type                          = AMD_EFI_SMBIOS_TYPE_MEMORY_DEVICE;
    SmbiosTableType17->Hdr.Length                        = (UINT8) StructureSize;
    SmbiosTableType17->MemoryArrayHandle                 = MemoryArrayHandle;
    SmbiosTableType17->MemoryErrorInformationHandle      = MemoryErrorInfoHandle;
    SmbiosTableType17->DeviceSet                         = MemDmiInfo->T17[Socket][Channel][Dimm].DeviceSet;
    SmbiosTableType17->DeviceLocator                     = 1;
    SmbiosTableType17->BankLocator                       = 2;
    SmbiosTableType17->Manufacturer                      = 3;
    SmbiosTableType17->SerialNumber                      = 4;
    SmbiosTableType17->AssetTag                          = 5;
    SmbiosTableType17->PartNumber                        = 6;
    
    
    StrPtr = (EFI_SMBIOS_STRING*) (((UINT8 *) SmbiosTableType17) + StructureSize);
    StringSize = TotalSize - StructureSize;
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        MemDmiInfo->T17[Socket][Channel][Dimm].DeviceLocator
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        MemDmiInfo->T17[Socket][Channel][Dimm].BankLocator
        );

// only change if not trained AND DIMM inserted    
   if(MemDmiInfo->T17[Socket][Channel][Dimm].MemorySize && !IsMemTrained(Socket, Channel)){  // winddy -
      IDS_HDT_CONSOLE (MAIN_FLOW, "No Trained\n");                                           // winddy +

      DimmErrorValue = (1<<7) | (Socket<<6) | (Channel<<2)| Dimm;
      DimmNoTrainingToBmc(Socket,Channel,Dimm);            // XCL 20201214 +
      PcdSet8(PcdDimmError, DimmErrorValue);               // XCL 20201214 +

	  IDS_HDT_CONSOLE (MAIN_FLOW, "DimmErrorValue is %x\n",DimmErrorValue); 
      SmbiosTableType17->TotalWidth                        = 0xFFFF;
      SmbiosTableType17->DataWidth                         = 0xFFFF;
      SmbiosTableType17->Size                              = 0;
      SmbiosTableType17->FormFactor                        = UnknowFormFactor;
      SmbiosTableType17->MemoryType                        = UnknownMemType;

      MemDmiInfo->T17[Socket][Channel][Dimm].NonvolatileSize = 0;
      MemDmiInfo->T17[Socket][Channel][Dimm].VolatileSize    = 0;
      MemDmiInfo->T17[Socket][Channel][Dimm].CacheSize       = 0;
      MemDmiInfo->T17[Socket][Channel][Dimm].LogicalSize     = 0;
      MemDmiInfo->T17[Socket][Channel][Dimm].TypeDetail    = EmptyTypeDetail;
      MemDmiInfo->T17[Socket][Channel][Dimm].TypeDetail.Unknown = 1;
      LibAmdMemCopy (&SmbiosTableType17->TypeDetail, &MemDmiInfo->T17[Socket][Channel][Dimm].TypeDetail, sizeof (AMD_MEMORY_DEVICE_TYPE_DETAIL), StdHeader);

      SmbiosTableType17->TypeDetail.Reserved = 1;

      SmbiosTableType17->Speed                             = 0;
      SmbiosTableType17->Attributes                        = 0;
      SmbiosTableType17->ExtendedSize                      = 0;
      SmbiosTableType17->ConfiguredMemoryClockSpeed        = 0;
      SmbiosTableType17->MinimumVoltage                    = 0;
      SmbiosTableType17->MaximumVoltage                    = 0;
      SmbiosTableType17->ConfiguredVoltage                 = 0;

    
      Status = LibAmdInsertSmbiosString (
          &StrPtr,
          &StringSize,
          Manufacturer  //Leave this string for installed DIMM but not trained
          );   
       
      Status = LibAmdInsertSmbiosString (
          &StrPtr,
          &StringSize,
          "Unknown"              // MemDmiInfo->T17[Socket][Channel][Dimm].SerialNumber
          );
      Status = LibAmdInsertSmbiosString (
          &StrPtr,
          &StringSize,
          "Unknown"   //MemDmiInfo->T17[Socket][Channel][Dimm].AssetTag
          );
      Status = LibAmdInsertSmbiosString (
          &StrPtr,
          &StringSize,
          "Unknown"   //MemDmiInfo->T17[Socket][Channel][Dimm].PartNumber
          );
      MemPresent = FALSE;                         // wz200505 +     
   } else {
     
          SmbiosTableType17->TotalWidth                        = MemDmiInfo->T17[Socket][Channel][Dimm].TotalWidth;
          SmbiosTableType17->DataWidth                         = MemDmiInfo->T17[Socket][Channel][Dimm].DataWidth;
          SmbiosTableType17->Size                              = MemDmiInfo->T17[Socket][Channel][Dimm].MemorySize;
          SmbiosTableType17->FormFactor                        = MemDmiInfo->T17[Socket][Channel][Dimm].FormFactor;
          SmbiosTableType17->MemoryType                        = MemDmiInfo->T17[Socket][Channel][Dimm].MemoryType;

          LibAmdMemCopy (&SmbiosTableType17->TypeDetail, &MemDmiInfo->T17[Socket][Channel][Dimm].TypeDetail, sizeof (AMD_MEMORY_DEVICE_TYPE_DETAIL), StdHeader);

          SmbiosTableType17->Speed                             = MemDmiInfo->T17[Socket][Channel][Dimm].Speed;
          SmbiosTableType17->Attributes                        = MemDmiInfo->T17[Socket][Channel][Dimm].Attributes;
          SmbiosTableType17->ExtendedSize                      = MemDmiInfo->T17[Socket][Channel][Dimm].ExtSize;
          SmbiosTableType17->ConfiguredMemoryClockSpeed        = MemDmiInfo->T17[Socket][Channel][Dimm].ConfigSpeed;
// winddy - >>
          SmbiosTableType17->Speed                             = DoubleMemSpeed (SmbiosTableType17->Speed);
          SmbiosTableType17->ConfiguredMemoryClockSpeed        = DoubleMemSpeed (SmbiosTableType17->ConfiguredMemoryClockSpeed);
// winddy - <<
          SmbiosTableType17->MinimumVoltage                    = MemDmiInfo->T17[Socket][Channel][Dimm].MinimumVoltage;
          SmbiosTableType17->MaximumVoltage                    = MemDmiInfo->T17[Socket][Channel][Dimm].MaximumVoltage;
          SmbiosTableType17->ConfiguredVoltage                 = MemDmiInfo->T17[Socket][Channel][Dimm].ConfiguredVoltage;


          Status = LibAmdInsertSmbiosString (
              &StrPtr,
              &StringSize,
              Manufacturer
              );
		  MemDmiInfo->T17[Socket][Channel][Dimm].SerialNumber[11]=0;
          Status = LibAmdInsertSmbiosString (
              &StrPtr,
              &StringSize,
              MemDmiInfo->T17[Socket][Channel][Dimm].SerialNumber
              );
          Status = LibAmdInsertSmbiosString (
              &StrPtr,
              &StringSize,
              MemDmiInfo->T17[Socket][Channel][Dimm].AssetTag
              ); 
          Status = LibAmdInsertSmbiosString (
              &StrPtr,
              &StringSize,
              MemDmiInfo->T17[Socket][Channel][Dimm].PartNumber
              );  
          MemPresent = TRUE;                         // wz200505 +            

   }
    //Add for smbios 3.2
    if (IsDmi3_2) {                      // winddy -
      SmbiosTableType17->MemoryTechnology                           = MemDmiInfo->T17[Socket][Channel][Dimm].MemoryTechnology;
      SmbiosTableType17->MemoryOperatingModeCapability              = MemDmiInfo->T17[Socket][Channel][Dimm].MemoryOperatingModeCapability.MemOperatingModeCap;
      SmbiosTableType17->FirmwareVersion                            = 6;
      CalledStatus = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        MemDmiInfo->T17[Socket][Channel][Dimm].FirmwareVersion
        );
      Status = (CalledStatus > Status) ? CalledStatus : Status;
      SmbiosTableType17->ModuleManufacturerId                       = MemDmiInfo->T17[Socket][Channel][Dimm].ModuleManufacturerId;
      SmbiosTableType17->ModuleProductId                            = MemDmiInfo->T17[Socket][Channel][Dimm].ModuleProductId;
      SmbiosTableType17->MemorySubsystemControllerManufacturerId    = MemDmiInfo->T17[Socket][Channel][Dimm].MemorySubsystemControllerManufacturerId;
      SmbiosTableType17->MemorySubsystemControllerProductId         = MemDmiInfo->T17[Socket][Channel][Dimm].MemorySubsystemControllerProductId;
      SmbiosTableType17->NonvolatileSize                            = MemDmiInfo->T17[Socket][Channel][Dimm].NonvolatileSize;
      SmbiosTableType17->VolatileSize                               = MemDmiInfo->T17[Socket][Channel][Dimm].VolatileSize;
      SmbiosTableType17->CacheSize                                  = MemDmiInfo->T17[Socket][Channel][Dimm].CacheSize;
      SmbiosTableType17->LogicalSize                                = MemDmiInfo->T17[Socket][Channel][Dimm].LogicalSize;
    }

// wz200505 + >>
    if(MemPresent){
      DevWidth = MemDmiInfo->T17[Socket][Channel][Dimm].DevWidth;
      if(DevWidth!=0)
      {  
        PcdSet8(PcdUnilcRank, DevWidth);
      }
      IDS_HDT_CONSOLE (MAIN_FLOW, "DevWidth is %d\n",DevWidth);
      DevWidth = DevWidth%100;
      DevWidthStr[0] = 'W';
      DevWidthStr[1] = (DevWidth / 10) + '0';
      DevWidthStr[2] = (DevWidth % 10) + '0';   
      DevWidthStr[3] = 0;
      Status = LibAmdInsertSmbiosString (
          &StrPtr,
          &StringSize,
          DevWidthStr
          );
      IDS_HDT_CONSOLE (MAIN_FLOW, "DevWidthStr is %a\n",DevWidthStr);
    }
// wz200505 + <<     
    
    // Add DMI type 17
    CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType17->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType17);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
    *MemoryDeviceHandle = SmbiosTableType17->Hdr.Handle;

    // Free pool
    FreePool (SmbiosTableType17);

    return Status;
  } else {
    return EFI_SUCCESS;
  }
}



/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 Convert the manufactureId into an ASCII string.

 @param[in]     MemoryType                      The type of memory used in this device.
 @param[in]     ManufacturerIdCode              Manufacturer ID code.
 @param[out]    Manufacturer                    Return macufacturer.

 @retval        VOID
*/

#if 0
VOID
STATIC
ConvertManufacturer (
  IN       UINT8                                  MemoryType,
  IN       UINT64                                 ManufacturerIdCode,
     OUT   UINT8                                  *Manufacturer
  )
{
  UINT16                                        i;
  UINT8                                         LowId;
  UINT8                                         HighId;
  UINT16                                        DimmVendorWord;
  BOOLEAN                                       ManufacturerFound;

  MANUFACTURER_STRING_PAIR ManufacturerStrPairs[] = {
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
    {0,  NULL},
  };

  LowId = 0;
  HighId = 0;
  ManufacturerFound = FALSE;

  // Initialize the DIMM Vendor Words and Strings
  ManufacturerStrPairs[0].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord0);
  ManufacturerStrPairs[1].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord1);
  ManufacturerStrPairs[2].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord2);
  ManufacturerStrPairs[3].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord3);
  ManufacturerStrPairs[4].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord4);
  ManufacturerStrPairs[5].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord5);
  ManufacturerStrPairs[6].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord6);
  ManufacturerStrPairs[7].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord7);
  ManufacturerStrPairs[8].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord8);
  ManufacturerStrPairs[9].DimmVendorWord  = PcdGet16 (PcdAmdSmbiosDimmVendorWord9);
  ManufacturerStrPairs[10].DimmVendorWord = PcdGet16 (PcdAmdSmbiosDimmVendorWord10);
  ManufacturerStrPairs[11].DimmVendorWord = PcdGet16 (PcdAmdSmbiosDimmVendorWord11);
  ManufacturerStrPairs[12].DimmVendorWord = PcdGet16 (PcdAmdSmbiosDimmVendorWord12);
  ManufacturerStrPairs[13].DimmVendorWord = PcdGet16 (PcdAmdSmbiosDimmVendorWord13);
  ManufacturerStrPairs[14].DimmVendorWord = PcdGet16 (PcdAmdSmbiosDimmVendorWord14);
  ManufacturerStrPairs[15].DimmVendorWord = PcdGet16 (PcdAmdSmbiosDimmVendorWord15);

  ManufacturerStrPairs[0].DimmVendorString  = PcdGetPtr (PcdAmdSmbiosDimmVendorStr0);
  ManufacturerStrPairs[1].DimmVendorString  = PcdGetPtr (PcdAmdSmbiosDimmVendorStr1);
  ManufacturerStrPairs[2].DimmVendorString  = PcdGetPtr (PcdAmdSmbiosDimmVendorStr2);
  ManufacturerStrPairs[3].DimmVendorString  = PcdGetPtr (PcdAmdSmbiosDimmVendorStr3);
  ManufacturerStrPairs[4].DimmVendorString  = PcdGetPtr (PcdAmdSmbiosDimmVendorStr4);
  ManufacturerStrPairs[5].DimmVendorString  = PcdGetPtr (PcdAmdSmbiosDimmVendorStr5);
  ManufacturerStrPairs[6].DimmVendorString  = PcdGetPtr (PcdAmdSmbiosDimmVendorStr6);
  ManufacturerStrPairs[7].DimmVendorString  = PcdGetPtr (PcdAmdSmbiosDimmVendorStr7);
  ManufacturerStrPairs[8].DimmVendorString  = PcdGetPtr (PcdAmdSmbiosDimmVendorStr8);
  ManufacturerStrPairs[9].DimmVendorString  = PcdGetPtr (PcdAmdSmbiosDimmVendorStr9);
  ManufacturerStrPairs[10].DimmVendorString = PcdGetPtr (PcdAmdSmbiosDimmVendorStr10);
  ManufacturerStrPairs[11].DimmVendorString = PcdGetPtr (PcdAmdSmbiosDimmVendorStr11);
  ManufacturerStrPairs[12].DimmVendorString = PcdGetPtr (PcdAmdSmbiosDimmVendorStr12);
  ManufacturerStrPairs[13].DimmVendorString = PcdGetPtr (PcdAmdSmbiosDimmVendorStr13);
  ManufacturerStrPairs[14].DimmVendorString = PcdGetPtr (PcdAmdSmbiosDimmVendorStr14);
  ManufacturerStrPairs[15].DimmVendorString = PcdGetPtr (PcdAmdSmbiosDimmVendorStr15);

  switch (MemoryType) {
  case AmdMemoryTypeDdr3:
  case AmdMemoryTypeDdr4:
  case AmdMemoryTypeLpddr3:
  case AmdMemoryTypeLpddr4:
    if (ManufacturerIdCode != 0) {
      LowId = (UINT8) (ManufacturerIdCode & 0x7F);
      HighId = (UINT8) RShiftU64 (ManufacturerIdCode, 8);

      for (i = 0; i < (sizeof (ManufacturerStrPairs) / sizeof (MANUFACTURER_STRING_PAIR)); ++i) {
        DimmVendorWord = ManufacturerStrPairs[i].DimmVendorWord;
        if ((DimmVendorWord & 0xFF) == LowId &&
            ((DimmVendorWord >> 8) & 0xFF) == HighId) {
          AsciiStrCpyS (Manufacturer, MANUFACTURER_STRING_LENGTH, ManufacturerStrPairs[i].DimmVendorString);
          ManufacturerFound = TRUE;
          break;
        }
      }
    }
    break;
  }
  if (!ManufacturerFound) {
    AsciiStrCpyS (Manufacturer, MANUFACTURER_STRING_LENGTH, "Unknown");
  }
}

#endif



VOID
STATIC
ConvertManufacturer (
  IN       UINT8                                  MemoryType,
  IN       UINT64                                 ManufacturerIdCode,
     OUT   UINT8                                  *Manufacturer
  )
{
  BOOLEAN                ManufacturerFound = FALSE;
  UINT8                  ManuIDBank;  // Manufacturer ID code bank
  UINT8                  ManuID;      // Manufacturer ID code value
  CHAR8                  *pManuIDStr;

  switch (MemoryType) {
    case AmdMemoryTypeDdr4:
    case AmdMemoryTypeLpddr4:
      ManuIDBank = (UINT8)(ManufacturerIdCode & 0x7F);                 // Bit 7 is odd parity bit.
      ManuID     = (UINT8)RShiftU64 (ManufacturerIdCode, 8) & 0x7F;    // Bit 7 is odd parity bit.
      pManuIDStr = Jep106GetManufacturerString(ManuIDBank, ManuID);
      AsciiStrCpyS (Manufacturer, MANUFACTURER_STRING_LENGTH, pManuIDStr);
      ManufacturerFound = TRUE;
      break;
  }
  if (!ManufacturerFound) {
    AsciiStrCpyS (Manufacturer, MANUFACTURER_STRING_LENGTH, "Unknown");
  }
}

