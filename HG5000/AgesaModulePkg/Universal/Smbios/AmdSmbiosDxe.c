/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 4 7 16 17 18 19 20
 *
 * Contains code that generate SMBIOS type 4 7 16 17 18 19 20
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
#include "AMD.h"
#include "Filecode.h"
#include "Library/AmdBaseLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/IdsLib.h"
#include "Protocol/Smbios.h"
#include <MemDmi.h>
#include "Protocol/AmdSmbiosCompleteProtocol.h"
#include "Protocol/AmdSmbiosServicesProtocol.h"
#include "Protocol/FabricTopologyServices.h"
#include "AmdSmbiosDxe.h"
#include "PiDxe.h"
#include <Library/UefiLib.h>
#include <Library/ApcbLib.h>
#include <Library/ApcbZpLib.h>
#include "Library/MemoryAllocationLib.h"
#include "Library/MemChanXLat.h"

#define FILECODE UNIVERSAL_SMBIOS_AMDSMBIOSDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID *
FindPSOverrideEntry (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 EntryType,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID,
  IN       UINT8 DimmID
  );

UINT8
GetMaxDimmsPerChannel (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/**
 *---------------------------------------------------------------------------------------
 *
 *  Entry for AmdSmbiosDxe
 *  Generate SMBIOS type 4 7 16 17 18 19 20
 *
 *  Description:
 *    This function will populate SMBIOS with
 *      type  4: Processor Information
 *      type  7: Cache Information
 *      type 16: Physical Memory Array
 *      type 17: Memory Device
 *      type 18: Memory Error Information
 *      type 19: Memory Array Mapped Address
 *      type 20: Memory Device Mapped Address
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
AmdGenerateSmbios (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINT16                     CacheHandle[CpuLmaxCache];
  UINT8                      Socket;
  UINT8                      Channel;
  UINT8                      Dimm;
  UINTN                      NumberOfInstalledProcessors;
  UINTN                      NumberOfDie;
  EFI_SMBIOS_HANDLE          ArrayHandle;
  EFI_SMBIOS_HANDLE          ArrayMappedHandle[MAX_T19_REGION_SUPPORTED];
  EFI_SMBIOS_HANDLE          ArrayErrorHandle;
  EFI_SMBIOS_HANDLE          DeviceHandle;
  UINTN                      IgnoredRootBridges;
  EFI_STATUS                 Status;
  EFI_STATUS                 CalledStatus;
  EFI_SMBIOS_PROTOCOL       *Smbios;
  DMI_INFO                   MemDmiInfo;
  EFI_HANDLE                 Handle;
  AMD_CONFIG_PARAMS          StdHeader;
  DXE_AMD_SMBIOS_INIT_COMPLETE_PROTOCOL AmdSmbiosInitCompleteProtocol;
  AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;
  AMD_MEM_SMBIOS_SERVICES_PROTOCOL *AmdMemSmbiosServices;
  UINT8                      RegionIndex;
  UINT32                     TypeDataSize;
  UINT8                      *PlatformMemoryConfiguration;
  UINT8                      MaxDimmsPerChannel;
  UINTN                      DimmIndex = 0;           
  

  AGESA_TESTPOINT (TpUniversalSmbiosEntry, NULL);

  IDS_HDT_CONSOLE (MAIN_FLOW, "  AmdGenerateSmbios Entry\n");

  // Check PCD
  if (PcdGetBool (PcdAmdRemoveSmbios)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Smbios is disabled by user, exit\n");
    AGESA_TESTPOINT (TpUniversalSmbiosAbort, NULL);
    return EFI_UNSUPPORTED;
  }

  // Avoid re-entry by locating gAmdSmbiosDxeInitCompleteProtocolGuid.
  CalledStatus = gBS->LocateProtocol (&gAmdSmbiosDxeInitCompleteProtocolGuid, NULL, (VOID **)&AmdSmbiosInitCompleteProtocol);
  if (CalledStatus == EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "    re-entry, just return EFI_ALREADY_STARTED\n");
    AGESA_TESTPOINT (TpUniversalSmbiosAbort, NULL);
    return EFI_ALREADY_STARTED;
  }
  Status = EFI_SUCCESS;

  // Locate Smbios protocol
  gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
  IDS_HDT_CONSOLE (MAIN_FLOW, "  SMBIOS Version: %x.%x\n", Smbios->MajorVersion, Smbios->MinorVersion);

  CalledStatus = gBS->LocateProtocol (&gAmdMemSmbiosServicesProtocolGuid, NULL, (VOID **)&AmdMemSmbiosServices);
  if (EFI_ERROR (CalledStatus)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  AmdMemSmbiosServices not found.\n");
    AGESA_TESTPOINT (TpUniversalSmbiosAbort, NULL);
    return EFI_PROTOCOL_ERROR;
  }
  CalledStatus = AmdMemSmbiosServices->GetMemDmiInfo (AmdMemSmbiosServices, &MemDmiInfo);
  if (EFI_ERROR (CalledStatus)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Error invoking AmdMemSmbiosServices.\n");
    AGESA_TESTPOINT (TpUniversalSmbiosAbort, NULL);
    return EFI_PROTOCOL_ERROR;
  }

  // Locate FabricTopologyServicesProtocol
  gBS->LocateProtocol (&gAmdFabricTopologyServicesProtocolGuid, NULL, (VOID **) &FabricTopology);
  FabricTopology->GetSystemInfo (FabricTopology, &NumberOfInstalledProcessors, &NumberOfDie, &IgnoredRootBridges);

  for (Socket = 0; Socket < NumberOfInstalledProcessors; Socket++) {
    // Initialize CacheHandle with 0xFFFF
    LibAmdMemFill (CacheHandle, 0xFF, sizeof (UINT16) * CpuLmaxCache, &StdHeader);

    // Add type 7 first, to get handle
    CalledStatus = AmdAddSmbiosType7 (Socket, &CacheHandle, Smbios, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
    
    // Add type 4
    CalledStatus = AmdAddSmbiosType4 (Socket, CacheHandle, Smbios, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  // >>>>>>>>>>>>>>>>>
  TypeDataSize = 0;
  Status = ApcbGetType (APCB_GROUP_MEMORY , APCB_MEM_TYPE_PSO_DATA, 0, NULL, &TypeDataSize);
  if (TypeDataSize == 0) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get APCB_MEM_TYPE_PSO_DATA\n");
    return Status;
  }
  
  //PlatformMemoryConfiguration = 0;
  PlatformMemoryConfiguration = AllocateZeroPool (TypeDataSize);
  
  Status = ApcbGetType (APCB_GROUP_MEMORY , APCB_MEM_TYPE_PSO_DATA, 0, PlatformMemoryConfiguration, &TypeDataSize);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get the TypeDataStream for APCB_MEM_TYPE_PSO_DATA\n");
    return Status;
  }

  // <<<<<<<<<<<<<<<<
  
  //Modify if not exist 2 CPUs- 
  //- for (Socket = 0; Socket < PcdGet8 (PcdAmdNumberOfPhysicalSocket); Socket++) {
  for (Socket = 0; Socket < NumberOfInstalledProcessors; Socket++) { //modify +
      for (Channel = 0; Channel < MAX_CHANNELS_PER_SOCKET; Channel++) {
          MaxDimmsPerChannel = GetMaxDimmsPerChannel (PlatformMemoryConfiguration, Socket, Channel);

          for (Dimm = 0; Dimm < MaxDimmsPerChannel; Dimm++) {
// winddy + >>            
            IDS_HDT_CONSOLE (MAIN_FLOW, "(L%d)(%d,%d,%d) T:%d S:%d\n", \
              __LINE__, Socket, Channel, Dimm, MemDmiInfo.T17[Socket][Channel][Dimm].MemoryType, \
              MemDmiInfo.T17[Socket][Channel][Dimm].MemorySize);
// winddy + <<
			if(MemDmiInfo.T17[Socket][Channel][Dimm].MemorySize !=0){
    			CalledStatus = AmdAddSmbiosType18 (Smbios, &MemDmiInfo,Socket,Channel,Dimm,&ArrayErrorHandle, &StdHeader);
				Status = (CalledStatus > Status) ? CalledStatus : Status;
    			if (EFI_ERROR (CalledStatus)) {
      				return EFI_PROTOCOL_ERROR;
    			}
				CalledStatus = AmdAddSmbiosType16 (Smbios, &MemDmiInfo, ArrayErrorHandle, &ArrayHandle, &StdHeader);
  				Status = (CalledStatus > Status) ? CalledStatus : Status;
				  //[PRODUCT-374]
  				for (RegionIndex = 0; RegionIndex < MAX_T19_REGION_SUPPORTED; RegionIndex++) {
    			ArrayMappedHandle[RegionIndex] = 0;
    			CalledStatus = AmdAddSmbiosType19 (Smbios, &MemDmiInfo, ArrayHandle, &ArrayMappedHandle[RegionIndex], &StdHeader, RegionIndex);
  				}
  				Status = (CalledStatus > Status) ? CalledStatus : Status;
			}
			else
				ArrayErrorHandle = 0xFFFF;
            CalledStatus = AmdAddSmbiosType17 (Smbios, &MemDmiInfo, Socket, Channel, Dimm, ArrayHandle, ArrayErrorHandle, &DeviceHandle, &StdHeader);
            Status = (CalledStatus > Status) ? CalledStatus : Status;

            CalledStatus = AmdAddSmbiosType20 (DimmIndex, Smbios, &MemDmiInfo, Socket, Channel, Dimm, DeviceHandle, ArrayMappedHandle, &StdHeader);   // wz201008 - AR-001-BIOS-0004-0001 correct smbios type 20
            Status = (CalledStatus > Status) ? CalledStatus : Status;
            DimmIndex++;                                                                                                                              // wz201008 + AR-001-BIOS-0004-0001 correct smbios type 20
          }
    }
  }

  if (NumberOfInstalledProcessors < PcdGet8 (PcdAmdNumberOfPhysicalSocket)) {
    for (Socket = (UINT8) NumberOfInstalledProcessors; Socket < PcdGet8 (PcdAmdNumberOfPhysicalSocket); Socket++) {
      AmdAddSmbiosType4ForNoProcInstalled (Socket, CacheHandle, Smbios, &StdHeader);
    }
  }

  // Install Protocol gAmdSmbiosDxeInitCompleteProtocolGuid to avoid re-entry
  IDS_HDT_CONSOLE (MAIN_FLOW, "  Install Protocol gAmdSmbiosDxeInitCompleteProtocolGuid to avoid re-entry\n");
  AmdSmbiosInitCompleteProtocol.Revision = 311;
  Handle = NULL;
  gBS->InstallProtocolInterface (
                          &Handle,
                          &gAmdSmbiosDxeInitCompleteProtocolGuid,
                          EFI_NATIVE_INTERFACE,
                          &AmdSmbiosInitCompleteProtocol
                          );

  // Free pool
  FreePool (PlatformMemoryConfiguration);
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "  AmdGenerateSmbios Exit\n");

  AGESA_TESTPOINT (TpUniversalSmbiosExit, NULL);

  return Status;
}


/**
 *---------------------------------------------------------------------------------------
 *
 *  Check if kernel code support SMBIOS version x.x
 *
 *  Parameters:
 *    @param[in]       Smbios                       Pointer to EfiSmbiosProtocol
 *    @param[in]       MajorVersion                 Major version
 *    @param[in]       MinorVersion                 Minor version

 *    @retval          TRUE  - version x.x is supported
 *                     FALSE - version x.x is not supported
 *
 *---------------------------------------------------------------------------------------
 */
BOOLEAN
AmdSmbiosVersionCheck (
  IN       EFI_SMBIOS_PROTOCOL *Smbios,
  IN       UINT8                MajorVersion,
  IN       UINT8                MinorVersion
  )
{
	EFI_STATUS                    Status;
	SMBIOS_TABLE_ENTRY_POINT      *SmbiosTable;
	SMBIOS_TABLE_3_0_ENTRY_POINT  *Smbios30Table;
	  
// gEfiSmbiosProtocolGuid will return SMBIOS 2.x versions even though 3.x is supported. This is due to 1 protocol even though there are 2 table guids.
// return 3.x version if the table has been published
#if 0
  if ((Smbios->MajorVersion > MajorVersion) ||
      ((Smbios->MajorVersion == MajorVersion) && (Smbios->MinorVersion >= MinorVersion))) {
    return TRUE;
  } else {
    return FALSE;
  }
#endif
 IDS_HDT_CONSOLE (MAIN_FLOW, "  Supported SMBIOS Version: %d.%d\n", Smbios->MajorVersion, Smbios->MinorVersion);
  Status = EfiGetSystemConfigurationTable (&gEfiSmbios3TableGuid, (VOID **) &Smbios30Table);
    if (!(EFI_ERROR (Status) || Smbios30Table == NULL)) {
    	if ((Smbios30Table->MajorVersion > MajorVersion) ||
    	      ((Smbios30Table->MajorVersion == MajorVersion) && (Smbios30Table->MinorVersion >= MinorVersion))) {
    	    return TRUE;
    	  } else {
    	    return FALSE;
    	  }    	      
    } else {
      Status = EfiGetSystemConfigurationTable (&gEfiSmbiosTableGuid, (VOID **) &SmbiosTable);
      if (EFI_ERROR (Status) || SmbiosTable == NULL) {
        return FALSE;
      }
      if ((SmbiosTable->MajorVersion > MajorVersion) ||
          	      ((SmbiosTable->MajorVersion == MajorVersion) && (SmbiosTable->MinorVersion >= MinorVersion))) {
          	    return TRUE;
          	  } else {
          	    return FALSE;
          	  }    
    }
     
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID *
FindPSOverrideEntry (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 EntryType,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID,
  IN       UINT8 DimmID
  )
{
  UINT8 *Buffer;

  Buffer = PlatformMemoryConfiguration;

  while ((Buffer[0] != PSO_END) && (Buffer[0] != PSO_CPU_FAMILY_TO_OVERRIDE)) {
    if (Buffer[0] == EntryType) {
      if ((Buffer[2] & ((UINT8) 1 << SocketID)) != 0 ) {
        if ((Buffer[3] & ((UINT8) 1 << ChannelID)) != 0 ) {
          if ((Buffer[4] & ((UINT8) 1 << DimmID)) != 0 ) {
            return &Buffer[5];
          }
        }
      }
    }
    Buffer += Buffer[1] + 2;
  }

  return NULL;
}

UINT8
GetMaxDimmsPerChannel (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID
  )
{
  UINT8  *DimmsPerChPtr;
  UINT8  MaxDimmPerCH;

  DimmsPerChPtr = FindPSOverrideEntry (PlatformMemoryConfiguration, PSO_MAX_DIMMS, SocketID, ChannelID, 0);
  if (DimmsPerChPtr != NULL) {
    MaxDimmPerCH = *DimmsPerChPtr;
  } else {
    MaxDimmPerCH = MAX_DIMMS_PER_CHANNEL;
  }

  // Maximum number of dimms per channel cannot be larger than its default value.
  ASSERT (MaxDimmPerCH <= MAX_DIMMS_PER_CHANNEL);

  return MaxDimmPerCH;
}

