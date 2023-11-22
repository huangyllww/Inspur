/* $NoKeywords:$ */

/**
 * @file
 *
 * Generate SMBIOS type 4
 *
 * Contains code that generate SMBIOS type 4
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
#include "Library/HygonBaseLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/MemoryAllocationLib.h"
#include <Library/PrintLib.h>
#include <Protocol/HygonNbioSmuServicesProtocol.h>
#include "Protocol/Smbios.h"
#include "Protocol/HygonSmbiosServicesProtocol.h"
#include "PiDxe.h"

#define FILECODE  UNIVERSAL_SMBIOS_HYGONSMBIOSTYPE4_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
CHAR8 ROMDATA  str_ProcManufacturer[] = "Unknown";
CHAR8 ROMDATA  str_ProcVersion[] = "Not Installed";

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
HygonAddSmbiosType4 (
  IN       UINTN                 Socket,
  IN       UINT16                CacheHandle[CpuLmaxCache],
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  UINTN                               StructureSize;
  UINTN                               StringSize;
  UINTN                               TotalSize;
  EFI_STATUS                          Status;
  EFI_STATUS                          CalledStatus;
  HYGON_SMBIOS_TABLE_TYPE4            *SmbiosTableType4;
  EFI_SMBIOS_STRING                   *StrPtr;
  HYGON_CCX_CORE_DMI_INFO             HygonCoreDmiInfo;
  HYGON_CCX_SMBIOS_SERVICES_PROTOCOL  *HygonSmbiosServices;
  HYGON_PROCESSOR_SN_PROTOCOL         *HygonProcessorSNProtocol;
  UINT32                              SerialNumber[6];
  CHAR8                               SerialNumberString[0x20];
  BOOLEAN                             SerialNumberExist;

  Status = EFI_SUCCESS;
  SerialNumberExist = FALSE;
  gBS->SetMem (SerialNumber, sizeof (SerialNumber), 0);
  gBS->SetMem (SerialNumberString, sizeof (SerialNumberString), 0);
  CalledStatus = gBS->LocateProtocol (&gHygonProcessorServicesProtocolGuid, NULL, (VOID **)&HygonProcessorSNProtocol);
  if (!EFI_ERROR (CalledStatus)) {
    //
    // This function will return all smu argument, but only 0&1 are serial number
    //
    CalledStatus = HygonProcessorSNProtocol->GetProcessorSN ((UINT8)Socket, SerialNumber);
    if (!EFI_ERROR (CalledStatus)) {
      SerialNumberExist = TRUE;
      AsciiSPrint (
        SerialNumberString,
        sizeof (SerialNumberString),
        "%x-%x",
        SerialNumber[1],
        SerialNumber[0]
        );
    }
  }

  // Calculate size of DMI type 4
  StructureSize = sizeof (HYGON_SMBIOS_TABLE_TYPE4);
  TotalSize     = StructureSize;
  switch (Socket) {
    case 0:
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket0));
      TotalSize += SerialNumberExist ? AsciiStrSize (SerialNumberString) : AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSerialNumberSocket0));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosAssetTagSocket0));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosPartNumberSocket0));
      break;
    case 1:
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket1));
      TotalSize += SerialNumberExist ? AsciiStrSize (SerialNumberString) : AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSerialNumberSocket1));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosAssetTagSocket1));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosPartNumberSocket1));
      break;
    case 2:
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket2));
      TotalSize += SerialNumberExist ? AsciiStrSize (SerialNumberString) : AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSerialNumberSocket2));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosAssetTagSocket2));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosPartNumberSocket2));
      break;
    case 3:
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket3));
      TotalSize += SerialNumberExist ? AsciiStrSize (SerialNumberString) : AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSerialNumberSocket3));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosAssetTagSocket3));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosPartNumberSocket3));
      break;
    default:
      // Only support up to 4 sockets
      ASSERT (FALSE);
  }

  TotalSize += sizeof (HygonCoreDmiInfo.ProcessorVersion);
  TotalSize += sizeof (HygonCoreDmiInfo.ProcessorManufacturer);
  ++TotalSize; // Additional null (00h), End of strings

  // Allocate zero pool
  SmbiosTableType4 = NULL;
  SmbiosTableType4 = AllocateZeroPool (TotalSize);
  if (SmbiosTableType4 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Generate DMI type 4 --- Start
  CalledStatus = gBS->LocateProtocol (&gHygonCcxSmbiosServicesProtocolGuid, NULL, (VOID **)&HygonSmbiosServices);
  if (EFI_ERROR (CalledStatus)) {
    return EFI_PROTOCOL_ERROR;
  }

  HygonSmbiosServices->GetCoreDmiInfo (HygonSmbiosServices, Socket, &HygonCoreDmiInfo);

  SmbiosTableType4->Hdr.Type             = HYGON_EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  SmbiosTableType4->Hdr.Length           = (UINT8) StructureSize;
  SmbiosTableType4->Hdr.Handle           = HYGON_SMBIOS_HANDLE_PI_RESERVED;

  SmbiosTableType4->Socket               = 1;
  SmbiosTableType4->ProcessorManufacture = 2;
  SmbiosTableType4->ProcessorVersion     = 3;
  SmbiosTableType4->SerialNumber         = 4;
  SmbiosTableType4->AssetTag             = 5;
  SmbiosTableType4->PartNumber           = 6;

  SmbiosTableType4->ProcessorType        = HygonCoreDmiInfo.ProcessorType;
  if (HygonCoreDmiInfo.ProcessorFamily < 0xFE) {
    SmbiosTableType4->ProcessorFamily    = (UINT8) HygonCoreDmiInfo.ProcessorFamily;
    SmbiosTableType4->ProcessorFamily2   = SmbiosTableType4->ProcessorFamily;
  } else {
    SmbiosTableType4->ProcessorFamily    = 0xFE;
    SmbiosTableType4->ProcessorFamily2   = HygonCoreDmiInfo.ProcessorFamily;
  }
  *((UINT32 *) &SmbiosTableType4->ProcessorId.Signature)    = HygonCoreDmiInfo.ProcessorID.EaxFamilyId;
  *((UINT32 *) &SmbiosTableType4->ProcessorId.FeatureFlags) = HygonCoreDmiInfo.ProcessorID.EdxFeatureId;
  *((UINT8  *) &SmbiosTableType4->Voltage)                  = HygonCoreDmiInfo.Voltage;
  SmbiosTableType4->ExternalClock        = HygonCoreDmiInfo.ExternalClock;
  SmbiosTableType4->MaxSpeed             = HygonCoreDmiInfo.MaxSpeed;
  SmbiosTableType4->CurrentSpeed         = HygonCoreDmiInfo.CurrentSpeed;
  SmbiosTableType4->Status               = HygonCoreDmiInfo.Status;
  SmbiosTableType4->ProcessorUpgrade     = HygonCoreDmiInfo.ProcessorUpgrade;
  SmbiosTableType4->L1CacheHandle        = CacheHandle[CpuL1Cache];
  SmbiosTableType4->L2CacheHandle        = CacheHandle[CpuL2Cache];
  SmbiosTableType4->L3CacheHandle        = CacheHandle[CpuL3Cache];
  if (HygonCoreDmiInfo.CoreCount >= 0xFF) {
    SmbiosTableType4->CoreCount          = 0xFF;
    SmbiosTableType4->CoreCount2         = HygonCoreDmiInfo.CoreCount;
  } else {
    SmbiosTableType4->CoreCount          = (UINT8) HygonCoreDmiInfo.CoreCount;
    SmbiosTableType4->CoreCount2         = HygonCoreDmiInfo.CoreCount;
  }

  if (HygonCoreDmiInfo.CoreEnabled >= 0xFF) {
    SmbiosTableType4->EnabledCoreCount   = 0xFF;
    SmbiosTableType4->EnabledCoreCount2  = HygonCoreDmiInfo.CoreEnabled;
  } else {
    SmbiosTableType4->EnabledCoreCount   = (UINT8) HygonCoreDmiInfo.CoreEnabled;
    SmbiosTableType4->EnabledCoreCount2  = HygonCoreDmiInfo.CoreEnabled;
  }

  if (HygonCoreDmiInfo.ThreadCount >= 0xFF) {
    SmbiosTableType4->ThreadCount        = 0xFF;
    SmbiosTableType4->ThreadCount2       = HygonCoreDmiInfo.ThreadCount;
  } else {
    SmbiosTableType4->ThreadCount        = (UINT8) HygonCoreDmiInfo.ThreadCount;
    SmbiosTableType4->ThreadCount2       = HygonCoreDmiInfo.ThreadCount;
  }

  SmbiosTableType4->ProcessorCharacteristics = HygonCoreDmiInfo.ProcessorCharacteristics;
  // Generate DMI type 4 --- End

  // Append all strings
  StrPtr     = (EFI_SMBIOS_STRING *)(((UINT8 *)SmbiosTableType4) + StructureSize);
  StringSize = TotalSize - StructureSize;
  switch (Socket) {
    case 0:
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket0)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 HygonCoreDmiInfo.ProcessorManufacturer
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 HygonCoreDmiInfo.ProcessorVersion
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 SerialNumberExist ? SerialNumberString : PcdGetPtr (PcdHygonSmbiosSerialNumberSocket0)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosAssetTagSocket0)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosPartNumberSocket0)
                 );
      break;
    case 1:
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket1)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 HygonCoreDmiInfo.ProcessorManufacturer
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 HygonCoreDmiInfo.ProcessorVersion
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 SerialNumberExist ? SerialNumberString : PcdGetPtr (PcdHygonSmbiosSerialNumberSocket1)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosAssetTagSocket1)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosPartNumberSocket1)
                 );
      break;
    case 2:
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket2)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 HygonCoreDmiInfo.ProcessorManufacturer
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 HygonCoreDmiInfo.ProcessorVersion
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 SerialNumberExist ? SerialNumberString : PcdGetPtr (PcdHygonSmbiosSerialNumberSocket2)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosAssetTagSocket2)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosPartNumberSocket2)
                 );
      break;
    case 3:
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket3)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 HygonCoreDmiInfo.ProcessorManufacturer
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 HygonCoreDmiInfo.ProcessorVersion
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 SerialNumberExist ? SerialNumberString : PcdGetPtr (PcdHygonSmbiosSerialNumberSocket3)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosAssetTagSocket3)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosPartNumberSocket3)
                 );
      break;
    default:
      // Only support up to 4 sockets
      ASSERT (FALSE);
  }

  // Add DMI type 4
  CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType4->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *)SmbiosTableType4);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  // Free pool
  FreePool (SmbiosTableType4);

  return Status;
}

EFI_STATUS
EFIAPI
HygonAddSmbiosType4ForNoProcInstalled (
  IN       UINTN                 Socket,
  IN       UINT16                CacheHandle[CpuLmaxCache],
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  UINTN                     StructureSize;
  UINTN                     StringSize;
  UINTN                     TotalSize;
  EFI_STATUS                Status;
  EFI_STATUS                CalledStatus;
  HYGON_SMBIOS_TABLE_TYPE4  *SmbiosTableType4;
  EFI_SMBIOS_STRING         *StrPtr;

  Status = EFI_SUCCESS;

  // Calculate size of DMI type 4
  StructureSize = sizeof (HYGON_SMBIOS_TABLE_TYPE4);
  TotalSize     = StructureSize;
  switch (Socket) {
    case 0:
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket0));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSerialNumberSocket0));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosAssetTagSocket0));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosPartNumberSocket0));
      break;
    case 1:
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket1));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSerialNumberSocket1));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosAssetTagSocket1));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosPartNumberSocket1));
      break;
    case 2:
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket2));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSerialNumberSocket2));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosAssetTagSocket2));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosPartNumberSocket2));
      break;
    case 3:
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket3));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosSerialNumberSocket3));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosAssetTagSocket3));
      TotalSize += AsciiStrSize (PcdGetPtr (PcdHygonSmbiosPartNumberSocket3));
      break;
    default:
      // Only support up to 4 sockets
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

  SmbiosTableType4->Hdr.Type             = HYGON_EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  SmbiosTableType4->Hdr.Length           = (UINT8) StructureSize;
  SmbiosTableType4->Hdr.Handle           = HYGON_SMBIOS_HANDLE_PI_RESERVED;

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
  StrPtr     = (EFI_SMBIOS_STRING *)(((UINT8 *)SmbiosTableType4) + StructureSize);
  StringSize = TotalSize - StructureSize;
  switch (Socket) {
    case 0:
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket0)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 str_ProcManufacturer
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 str_ProcVersion
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosSerialNumberSocket0)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosAssetTagSocket0)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosPartNumberSocket0)
                 );
      break;
    case 1:
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket1)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 str_ProcManufacturer
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 str_ProcVersion
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosSerialNumberSocket1)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosAssetTagSocket1)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosPartNumberSocket1)
                 );
      break;
    case 2:
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket2)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 str_ProcManufacturer
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 str_ProcVersion
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosSerialNumberSocket2)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosAssetTagSocket2)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosPartNumberSocket2)
                 );
      break;
    case 3:
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosSocketDesignationSocket3)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 str_ProcManufacturer
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 str_ProcVersion
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosSerialNumberSocket3)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosAssetTagSocket3)
                 );
      Status = LibHygonInsertSmbiosString (
                 &StrPtr,
                 &StringSize,
                 PcdGetPtr (PcdHygonSmbiosPartNumberSocket3)
                 );
      break;
    default:
      // Only support up to 4 sockets
      ASSERT (FALSE);
  }

  // Add DMI type 4
  CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType4->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *)SmbiosTableType4);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  // Free pool
  FreePool (SmbiosTableType4);

  return Status;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
