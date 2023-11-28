/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 4
 *
 * Contains code that generate SMBIOS type 4
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
#include "Library/AmdBaseLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Protocol/Smbios.h"
#include "Protocol/AmdSmbiosServicesProtocol.h"
#include "PiDxe.h"

#define FILECODE UNIVERSAL_SMBIOS_AMDSMBIOSTYPE4_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
CHAR8 ROMDATA str_ProcManufacturer[] = "Unknown";
CHAR8 ROMDATA str_ProcVersion[] = "Not Installed";

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate SMBIOS type 4
 *
 *  Parameters:
 *    @param[in]       Socket             Socket number
 *    @param[in]       CacheHandle        Handle of L1 L2 & L3 cache
 *    @param[in]       Smbios             Pointer to EfiSmbiosProtocol
 *    @param[in]       StdHeader          Handle to config for library and services
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
AmdAddSmbiosType4 (
  IN       UINTN                 Socket,
  IN       UINT16                CacheHandle[CpuLmaxCache],
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINTN                             StructureSize;
  UINTN                             StringSize;
  UINTN                             TotalSize;
  EFI_STATUS                        Status;
  EFI_STATUS                        CalledStatus;
  AMD_SMBIOS_TABLE_TYPE4           *SmbiosTableType4;
  EFI_SMBIOS_STRING                *StrPtr;
  AMD_CCX_CORE_DMI_INFO             AmdCoreDmiInfo;
  AMD_CCX_SMBIOS_SERVICES_PROTOCOL *AmdSmbiosServices;
  CHAR8                            SerialNumberSocket0[]= "Not Specified";
  CHAR8                            AssetTagSocket0[]= "Not Specified";
  CHAR8                            PartNumberSocket0[]= "Not Specified";
  CHAR8                            SerialNumberSocket1[]= "Not Specified";
  CHAR8                            AssetTagSocket1[]= "Not Specified";
  CHAR8                            PartNumberSocket1[]= "Not Specified";

  Status = EFI_SUCCESS;

  // Calculate size of DMI type 4
  StructureSize = sizeof (AMD_SMBIOS_TABLE_TYPE4);
  TotalSize = StructureSize;
  switch (Socket) {
  case 0:
    TotalSize += AsciiStrSize (PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket0));
    TotalSize += sizeof(SerialNumberSocket0);
    TotalSize += sizeof(AssetTagSocket0);
    TotalSize += sizeof(PartNumberSocket0);
    break;
  case 1:
    TotalSize += AsciiStrSize (PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket1));
    TotalSize += sizeof(SerialNumberSocket1);
    TotalSize += sizeof(AssetTagSocket1);
    TotalSize += sizeof(PartNumberSocket1);

    break;
  default:
    // Only support up to 2 sockets
    ASSERT (FALSE);
  }
  TotalSize += sizeof (AmdCoreDmiInfo.ProcessorVersion);
  TotalSize += sizeof (AmdCoreDmiInfo.ProcessorManufacturer);
  ++TotalSize; // Additional null (00h), End of strings

  // Allocate zero pool
  SmbiosTableType4 = NULL;
  SmbiosTableType4 = AllocateZeroPool (TotalSize);
  if (SmbiosTableType4 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Generate DMI type 4 --- Start
  CalledStatus = gBS->LocateProtocol (&gAmdCcxSmbiosServicesProtocolGuid, NULL, (VOID **)&AmdSmbiosServices);
  if (EFI_ERROR (CalledStatus)) {
    return EFI_PROTOCOL_ERROR;
  }
  AmdSmbiosServices->GetCoreDmiInfo (AmdSmbiosServices, Socket, &AmdCoreDmiInfo);

  SmbiosTableType4->Hdr.Type             = AMD_EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  SmbiosTableType4->Hdr.Length           = (UINT8) StructureSize;
  SmbiosTableType4->Hdr.Handle           = AMD_SMBIOS_HANDLE_PI_RESERVED;

  SmbiosTableType4->Socket               = 1;
  SmbiosTableType4->ProcessorManufacture = 2;
  SmbiosTableType4->ProcessorVersion     = 3;
  SmbiosTableType4->SerialNumber         = 0;
  SmbiosTableType4->AssetTag             = 0;
  SmbiosTableType4->PartNumber           = 0;

  SmbiosTableType4->ProcessorType        = AmdCoreDmiInfo.ProcessorType;
  if (AmdCoreDmiInfo.ProcessorFamily < 0xFE) {
    SmbiosTableType4->ProcessorFamily    = (UINT8) AmdCoreDmiInfo.ProcessorFamily;
    SmbiosTableType4->ProcessorFamily2   = SmbiosTableType4->ProcessorFamily;
  } else {
    SmbiosTableType4->ProcessorFamily    = 0xFE;
    SmbiosTableType4->ProcessorFamily2   = AmdCoreDmiInfo.ProcessorFamily;
  }
  *((UINT32 *) &SmbiosTableType4->ProcessorId.Signature)    = AmdCoreDmiInfo.ProcessorID.EaxFamilyId;
  *((UINT32 *) &SmbiosTableType4->ProcessorId.FeatureFlags) = AmdCoreDmiInfo.ProcessorID.EdxFeatureId;
  *((UINT8  *) &SmbiosTableType4->Voltage)                  = AmdCoreDmiInfo.Voltage;
  SmbiosTableType4->ExternalClock        = AmdCoreDmiInfo.ExternalClock;
  SmbiosTableType4->MaxSpeed             = AmdCoreDmiInfo.MaxSpeed;
  SmbiosTableType4->CurrentSpeed         = AmdCoreDmiInfo.CurrentSpeed;
  SmbiosTableType4->Status               = AmdCoreDmiInfo.Status;
  SmbiosTableType4->ProcessorUpgrade     = AmdCoreDmiInfo.ProcessorUpgrade;
  SmbiosTableType4->L1CacheHandle        = CacheHandle[CpuL1Cache];
  SmbiosTableType4->L2CacheHandle        = CacheHandle[CpuL2Cache];
  SmbiosTableType4->L3CacheHandle        = CacheHandle[CpuL3Cache];
  if (AmdCoreDmiInfo.CoreCount >= 0xFF) {
    SmbiosTableType4->CoreCount          = 0xFF;
    SmbiosTableType4->CoreCount2         = AmdCoreDmiInfo.CoreCount;
  } else {
    SmbiosTableType4->CoreCount          = (UINT8) AmdCoreDmiInfo.CoreCount;
    SmbiosTableType4->CoreCount2         = AmdCoreDmiInfo.CoreCount;
  }
  if (AmdCoreDmiInfo.CoreEnabled >= 0xFF) {
    SmbiosTableType4->EnabledCoreCount   = 0xFF;
    SmbiosTableType4->EnabledCoreCount2  = AmdCoreDmiInfo.CoreEnabled;
  } else {
    SmbiosTableType4->EnabledCoreCount   = (UINT8) AmdCoreDmiInfo.CoreEnabled;
    SmbiosTableType4->EnabledCoreCount2  = AmdCoreDmiInfo.CoreEnabled;
  }
  if (AmdCoreDmiInfo.ThreadCount >= 0xFF) {
    SmbiosTableType4->ThreadCount        = 0xFF;
    SmbiosTableType4->ThreadCount2       = AmdCoreDmiInfo.ThreadCount;
  } else {
    SmbiosTableType4->ThreadCount        = (UINT8) AmdCoreDmiInfo.ThreadCount;
    SmbiosTableType4->ThreadCount2       = AmdCoreDmiInfo.ThreadCount;
  }
  SmbiosTableType4->ProcessorCharacteristics = AmdCoreDmiInfo.ProcessorCharacteristics;
  // Generate DMI type 4 --- End


  // Append all strings
  StrPtr = (EFI_SMBIOS_STRING*) (((UINT8 *) SmbiosTableType4) + StructureSize);
  StringSize = TotalSize - StructureSize;
  switch (Socket) {
  case 0:
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket0)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        AmdCoreDmiInfo.ProcessorManufacturer
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        AmdCoreDmiInfo.ProcessorVersion
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        SerialNumberSocket0
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        AssetTagSocket0
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        PartNumberSocket0
        );
    break;
  case 1:
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket1)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        AmdCoreDmiInfo.ProcessorManufacturer
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        AmdCoreDmiInfo.ProcessorVersion
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        SerialNumberSocket1
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        AssetTagSocket1
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        PartNumberSocket1
        );
    break;
  default:
    // Only support up to 2 sockets
    ASSERT (FALSE);
  }
  // Add DMI type 4
  CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType4->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType4);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  // Free pool
  FreePool (SmbiosTableType4);

  return Status;
}


EFI_STATUS
EFIAPI
AmdAddSmbiosType4ForNoProcInstalled (
  IN       UINTN                 Socket,
  IN       UINT16                CacheHandle[CpuLmaxCache],
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINTN                             StructureSize;
  UINTN                             StringSize;
  UINTN                             TotalSize;
  EFI_STATUS                        Status;
  EFI_STATUS                        CalledStatus;
  AMD_SMBIOS_TABLE_TYPE4           *SmbiosTableType4;
  EFI_SMBIOS_STRING                *StrPtr;

  Status = EFI_SUCCESS;

  // Calculate size of DMI type 4
  StructureSize = sizeof (AMD_SMBIOS_TABLE_TYPE4);
  TotalSize = StructureSize;
  switch (Socket) {
  case 0:
    TotalSize += AsciiStrSize (PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket0));
    TotalSize += AsciiStrSize (PcdGetPtr (PcdAmdSmbiosSerialNumberSocket0));
    TotalSize += AsciiStrSize (PcdGetPtr (PcdAmdSmbiosAssetTagSocket0));
    TotalSize += AsciiStrSize (PcdGetPtr (PcdAmdSmbiosPartNumberSocket0));
    break;
  case 1:
    TotalSize += AsciiStrSize (PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket1));
    TotalSize += AsciiStrSize (PcdGetPtr (PcdAmdSmbiosSerialNumberSocket1));
    TotalSize += AsciiStrSize (PcdGetPtr (PcdAmdSmbiosAssetTagSocket1));
    TotalSize += AsciiStrSize (PcdGetPtr (PcdAmdSmbiosPartNumberSocket1));
    break;
  default:
    // Only support up to 2 sockets
    ASSERT (FALSE);
  }
  TotalSize += sizeof (str_ProcVersion);
  TotalSize += sizeof (str_ProcManufacturer);
  ++TotalSize; // Additional null (00h), End of strings

  // Allocate zero pool
  SmbiosTableType4 = NULL;
  SmbiosTableType4 = AllocateZeroPool (TotalSize);
  if (SmbiosTableType4 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Generate DMI type 4 --- Start

  SmbiosTableType4->Hdr.Type             = AMD_EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  SmbiosTableType4->Hdr.Length           = (UINT8) StructureSize;
  SmbiosTableType4->Hdr.Handle           = AMD_SMBIOS_HANDLE_PI_RESERVED;

  SmbiosTableType4->Socket               = 1;
  SmbiosTableType4->ProcessorManufacture = 2;
  SmbiosTableType4->ProcessorVersion     = 3;
  SmbiosTableType4->SerialNumber         = 4;
  SmbiosTableType4->AssetTag             = 5;
  SmbiosTableType4->PartNumber           = 6;

  SmbiosTableType4->ProcessorType        = 0x2;
  SmbiosTableType4->ProcessorFamily      = 0x2;
  SmbiosTableType4->ProcessorFamily2     = 0x2;
  SmbiosTableType4->ProcessorUpgrade     = 0x2;
  SmbiosTableType4->L1CacheHandle        = 0xFFFF;
  SmbiosTableType4->L2CacheHandle        = 0xFFFF;
  SmbiosTableType4->L3CacheHandle        = 0xFFFF;
  SmbiosTableType4->ProcessorCharacteristics = 0x2;
  // Generate DMI type 4 --- End


  // Append all strings
  StrPtr = (EFI_SMBIOS_STRING*) (((UINT8 *) SmbiosTableType4) + StructureSize);
  StringSize = TotalSize - StructureSize;
  switch (Socket) {
  case 0:
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket0)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        str_ProcManufacturer
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        str_ProcVersion
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        PcdGetPtr (PcdAmdSmbiosSerialNumberSocket0)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        PcdGetPtr (PcdAmdSmbiosAssetTagSocket0)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        PcdGetPtr (PcdAmdSmbiosPartNumberSocket0)
        );
    break;
  case 1:
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket1)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        str_ProcManufacturer
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        str_ProcVersion
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        PcdGetPtr (PcdAmdSmbiosSerialNumberSocket1)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        PcdGetPtr (PcdAmdSmbiosAssetTagSocket1)
        );
    Status = LibAmdInsertSmbiosString (
        &StrPtr,
        &StringSize,
        PcdGetPtr (PcdAmdSmbiosPartNumberSocket1)
        );
    break;
  default:
    // Only support up to 2 sockets
    ASSERT (FALSE);
  }
  // Add DMI type 4
  CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType4->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType4);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  // Free pool
  FreePool (SmbiosTableType4);

  return Status;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

