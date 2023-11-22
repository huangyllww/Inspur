/* $NoKeywords:$ */

/**
 * @file
 *
 * Generate SMBIOS type 17
 *
 * Contains code that generate SMBIOS type 17
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Universal
 *
 */
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
 *
 ******************************************************************************
 */
#include "HygonSmbios.h"
#include "HYGON.h"
#include "Porting.h"
#include "Filecode.h"
#include "Library/IdsLib.h"
#include "Library/BaseLib.h"
#include <Library/HygonBaseLib.h>
#include <Library/SmnAccessLib.h>
#include "Library/MemChanXLat.h"
#include <MemDmi.h>
#include "Library/UefiBootServicesTableLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Protocol/Smbios.h"
#include "Protocol/HygonSmbiosServicesProtocol.h"
#include "Protocol/FabricTopologyServices.h"
#include "HygonSmbiosDxe.h"
#include "PiDxe.h"
#include <CddRegistersDm.h>

#include <Library/PciLib.h>
#include <Library/HygonSocBaseLib.h>

#define FILECODE  UNIVERSAL_SMBIOS_HYGONSMBIOSTYPE17_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define MANUFACTURER_STRING_LENGTH  65                             // byo230906 -

typedef struct {
  UINT16          DimmVendorWord;
  CONST CHAR8    *DimmVendorString;
} MANUFACTURER_STRING_PAIR;

MANUFACTURER_STRING_PAIR ManufacturerStrPairs[] = {
  // ManufacturerIdCode, ManufacturerString
  {0x2C00, "Micron Technology"},
  {0xAD00, "SK Hynix"},
  {0xC100, "Infineon (Siemens)"},
  {0xCE00, "Samsung"},
  {0x4F00, "Transcend Information"},
  {0x9801, "Kingston"},
  {0xFE02, "Elpida"},
  {0x0B03, "Nanya Technology"},
  {0x2503, "Kingmax Semiconductor"},
  {0x8303, "Buffalo (Formerly Melco)"},
  {0xCB04, "A-DATA Technology"},
  {0xC106, "ASint Technology"},
  {0x4304, "Ramaxel"},
  {0x1A08, "JHICC"},
  {0x830C,  "UniIC"},
  {0x910A,  "CXMT"},
  {0xAB0C,  "Biwin"},
  {0x7F75, "Welldisk" }
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
ConvertManufacturer (
  IN     UINT8                                  MemoryType,
  IN     UINT64                                 ManufacturerIdCode,
  OUT    UINT8                                  *Manufacturer
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
  UINT16  DoubleSpeed = 0;

  if(Speed != 0) {
    if (Speed >= 1600 && Speed <= 3200) {
	    DoubleSpeed = Speed * 2;           // DDR-3200/3600/4000/4400/4800/5200
    } else {
      DoubleSpeed = 0;
    }
  }

  return DoubleSpeed;
}

BOOLEAN
IsMemTrained (
  IN      UINT8                     CurSocket,
  IN      UINT8                     Channel
  )
{
  UINT32                                   Address;
  UINT32                                   IsTrained;
  UINTN                                    NumberOfInstalledProcessors;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  HOST_TO_HPCB_CHANNEL_XLAT                *HpcbToBoardChanXLatTab;
  UINT8                                    Index = 0;
  UINT8                                    HostChannel;
  BOOLEAN                                  ValidChannel = FALSE;
  UINTN                                    CddNumberPerSocket;
  UINTN                                    CddsPresent;
  UINTN                                    CddIndex = 0;
  UINT32                                   CpuModel;
  UINT8                                    ChannelNumPerCdd;

  IDS_HDT_CONSOLE_PSP_TRACE ("IsMemTrained socket %d Channel %d\n", CurSocket, Channel);

  CpuModel = GetHygonSocModel();
  ChannelNumPerCdd = (CpuModel == HYGON_EX_CPU) ? MAX_CHANNELS_PER_CDD_HYEX : MAX_CHANNELS_PER_CDD_HYGX;
  if (CpuModel == HYGON_GX_CPU) {
    // convert continuous channel ID to actual channel ID
    Channel = (Channel >= 4) ? (Channel + 4) : Channel;
  }
  // Locate FabricTopologyServicesProtocol
  gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **)&FabricTopology);
  FabricTopology->GetSystemInfo (FabricTopology, &NumberOfInstalledProcessors, NULL, NULL);
  FabricTopology->GetCddInfo (FabricTopology, CurSocket, &CddNumberPerSocket, &CddsPresent);

  // Get HpcbToBoard channel mapping table
  HpcbToBoardChanXLatTab = (HOST_TO_HPCB_CHANNEL_XLAT *)PcdGetPtr (PcdHpcbToBoardChanXLatTab);

  // Calculate which Cdd the channel is on
  while (HpcbToBoardChanXLatTab[Index].TranslatedChannelId != 0xFF) {
    IDS_HDT_CONSOLE_PSP_TRACE ("HpcbToBoardChanXLatTab[%d] TranslatedChannelId %d \n", Index, HpcbToBoardChanXLatTab[Index].TranslatedChannelId);
    if (Channel == HpcbToBoardChanXLatTab[Index].TranslatedChannelId) {
      HostChannel = Channel;
      CddIndex = Channel / ChannelNumPerCdd;
      Address = (UINT32)UMC_SPACE(CddIndex, (HostChannel % ChannelNumPerCdd), UMC0_CH_REG_BASE | UMC_SDP_CTRL);
      IDS_HDT_CONSOLE_PSP_TRACE ("IsMemTrained Channel:%d  HostChannel: %d, CddIndex:%d  Address:0x%x\n", Channel, HostChannel,CddIndex,Address);
      ValidChannel = TRUE;
      break;
    }

    Index++;
    ValidChannel = FALSE;
  }

  if (!ValidChannel) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Error: Current Channel %d is invalid! \n", Channel);
    return FALSE;
  }

  // *** Get Channel training status start ***//
  if (IS_CDD_PRESENT (CddIndex, CddsPresent)) {
    SmnRegisterReadBySocket (CurSocket, Address, AccessWidth32, &IsTrained);
    IsTrained = !!(IsTrained & SDP_CTRL_SDPINIT);                                            // byo230906 -
    IDS_HDT_CONSOLE_PSP_TRACE ("IsMemTrained(%d,%d):%d\n", CurSocket, Channel, IsTrained);   // byo230906 +
    return (BOOLEAN)IsTrained;                                                               // byo230906 -
  } else {
    return FALSE;
  }

  // *** Get Channel training status end ***//
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
HygonAddSmbiosType17 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       UINT8                 Socket,
  IN       UINT8                 Channel,
  IN       UINT8                 Dimm,
  IN       EFI_SMBIOS_HANDLE     MemoryArrayHandle,
  IN       EFI_SMBIOS_HANDLE     MemoryErrorInfoHandle,
  OUT      EFI_SMBIOS_HANDLE    *MemoryDeviceHandle,
  IN       HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  UINTN                          StructureSize;
  UINTN                          StringSize;
  UINTN                          TotalSize;
  EFI_STATUS                     Status;
  EFI_STATUS                     CalledStatus;
  EFI_SMBIOS_HANDLE              DeviceErrorHandle;
  HYGON_SMBIOS_TABLE_TYPE17     *SmbiosTableType17;
  EFI_SMBIOS_STRING             *StrPtr;
  CHAR8                          Manufacturer[MANUFACTURER_STRING_LENGTH];
  CHAR8                          DevWidthStr[4];                   // byo230906 +
  BOOLEAN                        IsTrained;                        // byo230906 +
  UINT8                          DevWidth;                         // byo230906 +
  DMI_T17_TYPE_DETAIL            EmptyTypeDetail = { 0 };

  IDS_HDT_CONSOLE_PSP_TRACE ("HygonAddSmbiosType17 socket %d Channel %d dimm %d \n", Socket, Channel, Dimm);

  Status = EFI_SUCCESS;

  // Generate DMI type 17 --- Start

  if (MemDmiInfo->T17[Socket][Channel][Dimm].MemoryType != 0) {
    // Calculate size of DMI type 17
    ConvertManufacturer (
      MemDmiInfo->T17[Socket][Channel][Dimm].MemoryType,
      MemDmiInfo->T17[Socket][Channel][Dimm].ManufacturerIdCode,
      Manufacturer
      );
    StructureSize = sizeof (HYGON_SMBIOS_TABLE_TYPE17);
    TotalSize     = StructureSize + sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].DeviceLocator);
    TotalSize    += sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].BankLocator);
    TotalSize    += sizeof (Manufacturer);
    TotalSize    += sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].SerialNumber);
    TotalSize    += sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].PartNumber);
    // add smbios 3.2 support
    if (HygonSmbiosVersionCheck (Smbios, 3, 2)) {
      TotalSize += sizeof (MemDmiInfo->T17[Socket][Channel][Dimm].FirmwareVersion);
    }
    TotalSize += sizeof(DevWidthStr);                              // byo230906 +
    ++TotalSize; // Additional null (00h), End of strings

    // Allocate zero pool
    SmbiosTableType17 = NULL;
    SmbiosTableType17 = AllocateZeroPool (TotalSize);
    if (SmbiosTableType17 == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    IsTrained = IsMemTrained (Socket, Channel);                    // byo230906 +
    CalledStatus = HygonAddSmbiosType18 (Smbios, &DeviceErrorHandle, StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
    if (EFI_ERROR (CalledStatus)) {
      return EFI_PROTOCOL_ERROR;
    }

    SmbiosTableType17->Hdr.Handle                        = HYGON_SMBIOS_HANDLE_PI_RESERVED;
    SmbiosTableType17->Hdr.Type                          = HYGON_EFI_SMBIOS_TYPE_MEMORY_DEVICE;
    SmbiosTableType17->Hdr.Length                        = (UINT8) StructureSize;
    SmbiosTableType17->MemoryArrayHandle                 = MemoryArrayHandle;
    SmbiosTableType17->MemoryErrorInformationHandle      = DeviceErrorHandle;
    SmbiosTableType17->DeviceSet                         = MemDmiInfo->T17[Socket][Channel][Dimm].DeviceSet;
    SmbiosTableType17->DeviceLocator                     = 1;
    SmbiosTableType17->BankLocator                       = 2;
    SmbiosTableType17->Manufacturer                      = 3;
    SmbiosTableType17->SerialNumber                      = 4;
    SmbiosTableType17->AssetTag                          = 0x00;
    SmbiosTableType17->PartNumber                        = 5;
    
    
    StrPtr     = (EFI_SMBIOS_STRING*) (((UINT8 *) SmbiosTableType17) + StructureSize);
    StringSize = TotalSize - StructureSize;
    Status     = LibHygonInsertSmbiosString (
                   &StrPtr,
                   &StringSize,
                   MemDmiInfo->T17[Socket][Channel][Dimm].DeviceLocator
                   );
    Status = LibHygonInsertSmbiosString (
               &StrPtr,
               &StringSize,
               MemDmiInfo->T17[Socket][Channel][Dimm].BankLocator
               );

    if(!IsTrained && (MemDmiInfo->T17[Socket][Channel][Dimm].MemorySize > 0)) {    // byo230906 -
      // only change if not trained AND DIMM inserted
      SmbiosTableType17->TotalWidth                        = 0xFFFF;
      SmbiosTableType17->DataWidth                         = 0xFFFF;
      SmbiosTableType17->Size                              = 0;
      SmbiosTableType17->FormFactor                        = UnknowFormFactor;
      SmbiosTableType17->MemoryType                        = UnknownMemType;

      MemDmiInfo->T17[Socket][Channel][Dimm].TypeDetail    = EmptyTypeDetail;
      MemDmiInfo->T17[Socket][Channel][Dimm].TypeDetail.Unknown = 1;
      LibHygonMemCopy (&SmbiosTableType17->TypeDetail, &MemDmiInfo->T17[Socket][Channel][Dimm].TypeDetail, sizeof (HYGON_MEMORY_DEVICE_TYPE_DETAIL), StdHeader);

      SmbiosTableType17->Speed                             = 0;
      SmbiosTableType17->Attributes                        = 0;
      SmbiosTableType17->ExtendedSize                      = 0;
      SmbiosTableType17->ConfiguredMemoryClockSpeed        = 0;
      SmbiosTableType17->MinimumVoltage                    = 0;
      SmbiosTableType17->MaximumVoltage                    = 0;
      SmbiosTableType17->ConfiguredVoltage                 = 0;

      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 Manufacturer // Leave this string for installed DIMM but not trained
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 NULL       // MemDmiInfo->T17[Socket][Channel][Dimm].SerialNumber
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 NULL // MemDmiInfo->T17[Socket][Channel][Dimm].PartNumber
                 );
    } else {
      SmbiosTableType17->TotalWidth                        = MemDmiInfo->T17[Socket][Channel][Dimm].TotalWidth;
      SmbiosTableType17->DataWidth                         = MemDmiInfo->T17[Socket][Channel][Dimm].DataWidth;
      SmbiosTableType17->Size                              = MemDmiInfo->T17[Socket][Channel][Dimm].MemorySize;
      SmbiosTableType17->FormFactor                        = MemDmiInfo->T17[Socket][Channel][Dimm].FormFactor;
      SmbiosTableType17->MemoryType                        = MemDmiInfo->T17[Socket][Channel][Dimm].MemoryType;

      LibHygonMemCopy (&SmbiosTableType17->TypeDetail, &MemDmiInfo->T17[Socket][Channel][Dimm].TypeDetail, sizeof (HYGON_MEMORY_DEVICE_TYPE_DETAIL), StdHeader);

      SmbiosTableType17->Speed                             = MemDmiInfo->T17[Socket][Channel][Dimm].Speed;
      SmbiosTableType17->Attributes                        = MemDmiInfo->T17[Socket][Channel][Dimm].Attributes;
      SmbiosTableType17->ExtendedSize                      = MemDmiInfo->T17[Socket][Channel][Dimm].ExtSize;
      SmbiosTableType17->ConfiguredMemoryClockSpeed        = MemDmiInfo->T17[Socket][Channel][Dimm].ConfigSpeed;
      if (HygonSmbiosVersionCheck (Smbios, 3, 1)) {
        SmbiosTableType17->Speed                           = DoubleMemSpeed (SmbiosTableType17->Speed);
        SmbiosTableType17->ConfiguredMemoryClockSpeed      = DoubleMemSpeed (SmbiosTableType17->ConfiguredMemoryClockSpeed);
      }
      SmbiosTableType17->MinimumVoltage                    = MemDmiInfo->T17[Socket][Channel][Dimm].MinimumVoltage;
      SmbiosTableType17->MaximumVoltage                    = MemDmiInfo->T17[Socket][Channel][Dimm].MaximumVoltage;
      SmbiosTableType17->ConfiguredVoltage                 = MemDmiInfo->T17[Socket][Channel][Dimm].ConfiguredVoltage;


      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 Manufacturer
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 MemDmiInfo->T17[Socket][Channel][Dimm].SerialNumber
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 MemDmiInfo->T17[Socket][Channel][Dimm].PartNumber
                 );
    }

    // Add for smbios 3.2
    if (HygonSmbiosVersionCheck (Smbios, 3, 2)) {
      SmbiosTableType17->MemoryTechnology                           = MemDmiInfo->T17[Socket][Channel][Dimm].MemoryTechnology;
      SmbiosTableType17->MemoryOperatingModeCapability              = MemDmiInfo->T17[Socket][Channel][Dimm].MemoryOperatingModeCapability.MemOperatingModeCap;
      SmbiosTableType17->FirmwareVersion                            = 6;
      CalledStatus = LibHygonInsertSmbiosString (
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

    // Add for smbios 3.3
    if (HygonSmbiosVersionCheck (Smbios, 3, 3)) {
      SmbiosTableType17->ExtendedSpeed                              = MemDmiInfo->T17[Socket][Channel][Dimm].ExtendedSpeed;
      SmbiosTableType17->ExtendedConfiguredMemorySpeed              = MemDmiInfo->T17[Socket][Channel][Dimm].ExtendedConfiguredMemorySpeed;
    }

// byo230906 + >>
    if(MemDmiInfo->T17[Socket][Channel][Dimm].MemorySize){
      if(IsTrained){
        DevWidth = MemDmiInfo->T17[Socket][Channel][Dimm].DevWidth;
        DevWidth = DevWidth%100;
        DevWidthStr[0] = 'W';
        DevWidthStr[1] = (DevWidth / 10) + '0';
        DevWidthStr[2] = (DevWidth % 10) + '0';   
        DevWidthStr[3] = 0;
        Status = LibHygonInsertSmbiosString (
                  &StrPtr,
                  &StringSize,
                  DevWidthStr
                  );
      } else {
        DevWidthStr[0] = 'M';
        DevWidthStr[1] = 'N';
        DevWidthStr[2] = 'T';   
        DevWidthStr[3] = 0;
        Status = LibHygonInsertSmbiosString (
                  &StrPtr,
                  &StringSize,
                  DevWidthStr
                  );
      }
    }
// byo230906 + <<	
	
    // Add DMI type 17
    CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType17->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *)SmbiosTableType17);
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

// byo230906 + >>
CHAR8 * 
Jep106GetManufacturerString (
  UINT8  Bank,
  UINT8  Id
);
// byo230906 + <<

/**
 Convert the manufactureId into an ASCII string.

 @param[in]     MemoryType                      The type of memory used in this device.
 @param[in]     ManufacturerIdCode              Manufacturer ID code.
 @param[out]    Manufacturer                    Return macufacturer.

 @retval        VOID
*/
VOID
STATIC
ConvertManufacturer (
  IN       UINT8                                  MemoryType,
  IN       UINT64                                 ManufacturerIdCode,
  OUT      UINT8                                  *Manufacturer
  )
{
// byo230906 + >>
  BOOLEAN                ManufacturerFound = FALSE;
  UINT8                  ManuIDBank;  // Manufacturer ID code bank
  UINT8                  ManuID;      // Manufacturer ID code value
  CHAR8                  *pManuIDStr = "";

  switch (MemoryType) {
    case HygonMemoryTypeDdr3:
    case HygonMemoryTypeDdr4:
    case HygonMemoryTypeLpddr3:
    case HygonMemoryTypeLpddr4:
    case HygonMemoryTypeDdr5:
    case HygonMemoryTypeLpddr5:
      ManuIDBank = (UINT8)(ManufacturerIdCode & 0x7F);                 // Bit 7 is odd parity bit.
      ManuID     = (UINT8)RShiftU64 (ManufacturerIdCode, 8) & 0x7F;    // Bit 7 is odd parity bit.
      pManuIDStr = Jep106GetManufacturerString(ManuIDBank, ManuID);
      if(pManuIDStr[0] != '<'){
        ManufacturerFound = TRUE;
      }
      break;
  }
  if (!ManufacturerFound) {
    AsciiStrCpyS (Manufacturer, MANUFACTURER_STRING_LENGTH, "Unknown");
  } else {
    AsciiStrCpyS (Manufacturer, MANUFACTURER_STRING_LENGTH, pManuIDStr);
  }
// byo230906 + <<

#if 0                         // byo230906 +
  UINT16   i;
  UINT8    LowId;
  UINT8    HighId;
  UINT16   DimmVendorWord;
  BOOLEAN  ManufacturerFound;

  LowId = 0;
  HighId = 0;
  ManufacturerFound = FALSE;

  switch (MemoryType) {
    case HygonMemoryTypeDdr3:
    case HygonMemoryTypeDdr4:
    case HygonMemoryTypeLpddr3:
    case HygonMemoryTypeLpddr4:
    case HygonMemoryTypeDdr5:
    case HygonMemoryTypeLpddr5:
      if (ManufacturerIdCode != 0) {
        LowId  = (UINT8)(ManufacturerIdCode & 0x7F);
        HighId = (UINT8)RShiftU64 (ManufacturerIdCode, 8);

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
#endif                        // byo230906 +
}
