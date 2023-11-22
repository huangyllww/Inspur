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
  * ***************************************************************************
  *
 */

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#include <Library/BaseLib.h>
#include "Library/HygonBaseLib.h"
#include "mtspd5.h"
#include "MemDmi.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include <HPCB.h>
#include "Filecode.h"
#include <HPOB.h>

#define FILECODE  LIBRARY_MEMSMBIOSLIB_MEMDDR5SPD_FILECODE

VOID
InitSmbios32Type17 (
  IN      BOOLEAN                       DimmPresent,
  IN      UINT8                         *SpdData,
  IN OUT  TYPE17_DMI_INFO               *T17
  );

/*----------------------------------------------------------------------------
 *                           DEFINES AND MACRO
 *
 *----------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */
typedef struct _MEM_TCK_ENTRY {
  UINT16  Memclk;
  UINT16  TckPs;
} MEM_TCK_ENTRY;

CONST MEM_TCK_ENTRY MemFreqToTckTable[] = {
  {1000,1000},
  {1050, 952},
  {1067, 938},
  {1200, 833},
  {1333, 750},
  {1367, 732},
  {1400, 714},
  {1433, 698},
  {1467, 682},
  {1500, 667},
  {1533, 652},
  {1567, 638},
  {1600, 625},
  {1633, 612},
  {1667, 599},
  {1700, 588},
  {1733, 577},
  {1767, 565},
  {1800, 555},
  {1833, 545},
  {1867, 535},
  {1900, 526},
  {1933, 517},
  {1967, 508},
  {2000, 500},
  {2033, 491},
  {2067, 483},
  {2100, 476},
  {2133, 468},
  {2167, 461},
  {2200, 454},
  {2233, 447},
  {2267, 441},
  {2300, 434},
  {2333, 428},
  {2367, 422},
  {2400, 416},
  {2433, 411},
  {2467, 405},
  {2500, 400},
  {2550, 392},
  {2600, 384},
  {2650, 377},
  {2700, 370},
  {2750, 363},
  {2800, 357},
  {2850, 350},
  {2900, 344},
  {2950, 338},
  {3000, 333},
  {3050, 327},
  {3100, 322},
  {3150, 317},
  {3200, 312},
  {3250, 307},
  {3300, 303},
  {3350, 298},
  {3400, 294},
  {3450, 289},
  {3500, 285},
  {3550, 281},
  {3600, 277},
  {3650, 273},
  {3700, 270},
  {3750, 266},
  {3800, 263},
  {3850, 259},
  {3900, 256},
  {3950, 253},
  {4000, 250},
  {4050, 246},
  {4100, 243},
  {4150, 240},
  {4200, 238},
  {4250, 235},
  {4300, 232},
  {4350, 229},
  {4400, 227}
};

/**
 *
 *  IntToString
 *
 *  Description:
 *    Translate UINT array to CHAR array.
 *
 *  Parameters:
 *    @param[in, out]    *String       Pointer to CHAR array
 *    @param[in]         *Integer      Pointer to UINT array
 *    @param[in]         SizeInByte    The size of UINT array
 *
 *  Processing:
 *
 */
VOID
STATIC
IntToString (
  IN OUT   CHAR8 *String,
  IN       UINT8 *Integer,
  IN       UINT8 SizeInByte
  )
{
  UINT8  Index;

  for (Index = 0; Index < SizeInByte; Index++) {
    *(String + Index * 2)     = (*(Integer + Index) >> 4) & 0x0F;
    *(String + Index * 2 + 1) = *(Integer + Index) & 0x0F;
  }

  for (Index = 0; Index < (SizeInByte * 2); Index++) {
    if (*(String + Index) >= 0x0A) {
      *(String + Index) += 0x37;
    } else {
      *(String + Index) += 0x30;
    }
  }

  *(String + SizeInByte * 2) = 0x0;
}


UINT8
MemSpdDecodeDiesPerPackage (
  UINT8	SpdDies
)
{
  UINT8 SpdDiePerPkgTable[] = SPD_DIE_PER_PKG_TABLE;
  UINT8 Dies;

if (SpdDies >= sizeof(SpdDiePerPkgTable)) {
  Dies = 0;
} else {
  Dies = SpdDiePerPkgTable[SpdDies];
}
  return Dies;
}

UINT8
MemSpdDecodeDensity (
 UINT8  SpdDensity
)
{
  UINT8 SpdDensityTable[] = SPD_DENSITY_TABLE;
  UINT8 Density;

  if (SpdDensity >= sizeof(SpdDensityTable)) {
    Density = 0;
  } else {
    Density = SpdDensityTable[SpdDensity];
  }
  return Density;
}

HGPI_STATUS
ParseDdr5SpdToInitT17 (
  IN UINT8                 *DimmSpd,
  IN OUT   TYPE17_DMI_INFO *T17,
  IN HPOB_MEM_DMI_PHYSICAL_DIMM  *PhysicalDimm
  )
{
  UINT16  i;
  UINT8   Rank;
  UINT32  MemorySize;
  SPD_BASE_CONFIG_0_S  *BaseConfig0;
  SPD_ANNEX_COMMON_S   *ModuleParms;
  SPD_MANUFACTURING_INFO_S *MfgInfo;
  BOOLEAN               Asymetric;
  UINT8                 NumRanks;
  UINT8 DiePerPkg;
  UINT8 ChannelsPerDimm;
  UINT8 				IoWidth;
  UINT16				BusWidth;
  UINT8                 SpdCapacity;
  UINT16                TckPs;
  UINT32				  FreqTableIndex;

  IDS_HDT_CONSOLE (MAIN_FLOW, "ParseDdr5SpdToInitT17 Entry \n");

  /*Memory SIZE calculate
  memory size = 
  number of channel per dimm--------------spd 235
  primary bus width per channel ----------spd 235
  number of package ranks per channel ----spd 234
  SDRAM I/O width-------------------------spd 6
  die per package--------------------- ---spd 4
  SDRAM density per die-------------------spd 4
  */
  BaseConfig0 = (SPD_BASE_CONFIG_0_S*) &(DimmSpd[SpdBlock_BaseConfig_0 * SPD_BLOCK_LEN]);
  ModuleParms = (SPD_ANNEX_COMMON_S*) &(DimmSpd[SpdBlock_ModuleParms_0 * SPD_BLOCK_LEN]);
  MfgInfo     = (SPD_MANUFACTURING_INFO_S*) &(DimmSpd[SpdBlock_MfgInfo0 * SPD_BLOCK_LEN]);
  MemorySize = 0;
  Asymetric = (ModuleParms->ModuleOrg.Field.RankMix == RankMixAsymmetrical) ? TRUE : FALSE;
  
  NumRanks = SPD_PACKAGE_RANKS_DECODE(ModuleParms->ModuleOrg.Field.RanksPerChannel);
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "DDR5 SPD Asymetric = %d NumRanks = %d  \n", Asymetric,NumRanks);
  for (Rank = 0; Rank < MIN(NumRanks, 2); Rank ++ ) {
    if (Asymetric && (Rank & 0x01) ) {
      IoWidth = SPD_DECODE_IO_WIDTH(BaseConfig0->SecondIoWidth.Field.IoWidth);
      DiePerPkg = MemSpdDecodeDiesPerPackage(BaseConfig0->SecondDensity.Field.DiePerPkg);
    } else {
      IoWidth = SPD_DECODE_IO_WIDTH(BaseConfig0->FirstIoWidth.Field.IoWidth);
      DiePerPkg = MemSpdDecodeDiesPerPackage(BaseConfig0->FirstDensity.Field.DiePerPkg);
    }
    BusWidth = SPD_CHANNEL_BUS_WIDTH_DECODE(ModuleParms->ChBusWidth.Field.Width);
    IDS_HDT_CONSOLE (MAIN_FLOW, "DDR5 SPD RandIndex = %d  IoWidth = %d DiePerPkg = %d BusWidth = %d  \n", Rank,IoWidth,DiePerPkg,BusWidth);
    T17->DataWidth = BusWidth * 2;  // RDIMM data width = 2 x data width per sub-channel
    T17->TotalWidth = T17->DataWidth + (SPD_CHANNEL_BUS_WIDTH_EXT_DECODE (ModuleParms->ChBusWidth.Field.WidthExt) * 2);
  
    IDS_HDT_CONSOLE (MAIN_FLOW, "T17->DataWidth = %d, T17->TotalWidth = %d \n", T17->DataWidth, T17->TotalWidth);
    SpdCapacity = MemSpdDecodeDensity(BaseConfig0->FirstDensity.Field.Density);
    ChannelsPerDimm = SPD_CHANNELS_PER_DIMM_DECODE(ModuleParms->ChBusWidth.Field.NumChannels);
    MemorySize += ChannelsPerDimm * BusWidth / IoWidth * DiePerPkg * SpdCapacity / 8; // MemorySize in GB
    
    IDS_HDT_CONSOLE (MAIN_FLOW, "DDR5 SPD RandIndex = %d  MemorySize = %d SpdCapacity = %d ChannelsPerDimm = %d  \n", Rank,MemorySize,SpdCapacity,ChannelsPerDimm);
    
  }
  
  MemorySize = MemorySize << 10; // MemorySize in MB
  IDS_HDT_CONSOLE (MAIN_FLOW, "MemorySize = %d MB \n", MemorySize);
  
  if (MemorySize < 0x7FFF) {
    T17->MemorySize = (UINT16)MemorySize;
    T17->ExtSize = 0;
  } else {
    T17->MemorySize = 0x7FFF;
    T17->ExtSize = MemorySize;
  }
  
  T17->TypeDetail.Synchronous = 1;
  
  switch (BaseConfig0->KeyByte2.Field.BaseModuleType) {
    case SPD_BASEMODULE_RDIMM:
      T17->TypeDetail.Registered = 1;
      T17->FormFactor = DimmFormFactorFormFactor;
      break;
    case SPD_BASEMODULE_LRDIMM:
      T17->TypeDetail.Registered = 1;
      T17->FormFactor = DimmFormFactorFormFactor;
      break;
    case SPD_BASEMODULE_UDIMM:
      T17->TypeDetail.Unbuffered = 1;
      T17->FormFactor = DimmFormFactorFormFactor;
      break;
    case SPD_BASEMODULE_SODIMM:
    case SPD_BASEMODULE_SORDIMM:
      T17->TypeDetail.Unbuffered = 1;
      T17->FormFactor = SodimmFormFactor;
      break;
    case SPD_BASEMODULE_SOLDEREDDOWN:
      T17->TypeDetail.Unbuffered = 1;
      T17->FormFactor = OtherFormFactor;
      break;
    default:
      T17->TypeDetail.Unknown = 1;
      T17->FormFactor = UnknowFormFactor;
  }
  
  switch (BaseConfig0->KeyByte1.Field.ModuleType) {
    case ModTypeLpddr4Sram:
    case ModTypeLpddr4xSdram:
      T17->MemoryType = LpDdr4MemType;
      T17->MinimumVoltage = 600;
      T17->MaximumVoltage = 600;
      break;
    case ModTypeLpddr5Sdram:
      T17->MemoryType = LpDdr5MemType;
      T17->MinimumVoltage = 1100;
      T17->MaximumVoltage = 1100;
      break;
    case ModTypeDdr4Sdram:
    case ModTypeDdr4ESdram:
      T17->MemoryType = Ddr4MemType;
      T17->MinimumVoltage = 1200;
      T17->MaximumVoltage = 1200;
      break;
    default:
      T17->MemoryType = Ddr5MemType;
      T17->MinimumVoltage = 1100;
      T17->MaximumVoltage = 1100;
      break;
  }
  TckPs = BaseConfig0->TckAvgMin.Value;
  for (FreqTableIndex = 0; FreqTableIndex < sizeof(MemFreqToTckTable) / sizeof(MemFreqToTckTable[0]); FreqTableIndex ++) {
    if (MemFreqToTckTable[FreqTableIndex].TckPs <= TckPs) {
      T17->Speed = MemFreqToTckTable[FreqTableIndex].Memclk;
      break;
    }
  }
  
  T17->ManufacturerIdCode = MfgInfo->ModuleMfgId.Value;
  IDS_HDT_CONSOLE (MAIN_FLOW, "T17->ManufacturerIdCode 0x%lx ,MfgInfo->ModuleMfgId.Value 0x%x \n", T17->ManufacturerIdCode,MfgInfo->ModuleMfgId.Value);
  IntToString (T17->SerialNumber, MfgInfo->ModuleSerialNumber.String, SPD_MODULE_SERIAL_NUMBER_LEN);
  IDS_HDT_CONSOLE (MAIN_FLOW, "T17->SerialNumber %s  \n", T17->SerialNumber);
  for (i = 0; i < sizeof (T17->PartNumber) - 1; i ++) {
    T17->PartNumber[i] = MfgInfo->ModulePartNumber.String[i];
  }
  T17->PartNumber[i] = 0;
  IDS_HDT_CONSOLE (MAIN_FLOW, "T17->PartNumber %s  \n", T17->PartNumber);
  T17->Attributes = NumRanks;
  T17->DevWidth   = IoWidth;                // byo230906 +
  T17->ConfigSpeed = PhysicalDimm->ConfigSpeed;
  T17->ConfiguredVoltage = PhysicalDimm->ConfigVoltage;
  IDS_HDT_CONSOLE (MAIN_FLOW, "PhysicalDimm->ConfigSpeed 0x%X,T17->ConfiguredVoltage 0x%X  \n", T17->ConfigSpeed,T17->ConfiguredVoltage);
  InitSmbios32Type17 (PhysicalDimm->DimmPresent, DimmSpd, T17);
  IDS_HDT_CONSOLE (MAIN_FLOW, "ParseDdr5SpdToInitT17 Exit \n");
  
  return HGPI_SUCCESS;
}

HGPI_STATUS
ParseDdr5SpdToInitNvdimmSpdInfo (
  IN UINT8                    *DimmSpd,
  IN TYPE17_DMI_INFO          *T17,
  IN OUT  NVDIMM_SPD_INFO    **nvdimmSpdInfo
  )
{
  IDS_HDT_CONSOLE (MAIN_FLOW, "ParseDdr5SpdToInitNvdimmSpdInfo Entry \n");

  // Save NVDIMM SPD data to nvdimmSpdInfo
  if ((HYBRID_TYPE == ((DimmSpd[SPD_HYBRID] >> HYBRID_SHIFT) & HYBRID_MASK)) &&
      ((NVDIMM_N_HYBRID == ((DimmSpd[SPD_HYBRID_MEDIA] >> HYBRID_MEDIA_SHIFT) & HYBRID_MEDIA_MASK)) ||
       (NVDIMM_P_HYBRID == ((DimmSpd[SPD_HYBRID_MEDIA] >> HYBRID_MEDIA_SHIFT) & HYBRID_MEDIA_MASK)))) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "NVDIMM present ! \n");

    (*nvdimmSpdInfo)->DataValid = 0x55aa;
    (*nvdimmSpdInfo)->Handle    = T17->Handle;
    (*nvdimmSpdInfo)->DeviceId  = 0;
    (*nvdimmSpdInfo)->SubSysVendorId      = 0;
    (*nvdimmSpdInfo)->SubSysDeviceId      = 0;
    (*nvdimmSpdInfo)->SubSysRevisionId    = 0;
    (*nvdimmSpdInfo)->ManufacturingIdCode =
      ((UINT16)DimmSpd[SPD_MODULE_MANUFACTURER_ID_CODE_2nd_BYTE] <<
       8) | (UINT16)DimmSpd[SPD_MODULE_MANUFACTURER_ID_CODE_1ST_BYTE];
    (*nvdimmSpdInfo)->ManufacturingLocation = (UINT16)DimmSpd[SPD_MODULE_MANUFACTURING_LOCATION];
    (*nvdimmSpdInfo)->ManufacturingDate     =
      ((UINT16)DimmSpd[SPD_MODULE_MANUFACTURING_WEEK] << 8) | (UINT16)DimmSpd[SPD_MODULE_MANUFACTURING_YEAR];
    (*nvdimmSpdInfo)->SerialNumber[0] = DimmSpd[SPD_MODULE_SERIAL_NUMBER];
    (*nvdimmSpdInfo)->SerialNumber[1] = DimmSpd[SPD_MODULE_SERIAL_NUMBER + 1];
    (*nvdimmSpdInfo)->SerialNumber[2] = DimmSpd[SPD_MODULE_SERIAL_NUMBER + 2];
    (*nvdimmSpdInfo)->SerialNumber[3] = DimmSpd[SPD_MODULE_SERIAL_NUMBER + 3];
    (*nvdimmSpdInfo)->RevisionId = (UINT16)DimmSpd[SPD_MODULE_REVISION_CODE];
    (*nvdimmSpdInfo)++;      // Move pointer to next available location
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "ParseDdr5SpdToInitNvdimmSpdInfo Exit \n");
  return HGPI_SUCCESS;
}

/**
 *  IsNvHybridDimm
 *
 *  Description:
 *     Check if this is a NV hybrid DIMM
 *
 *  Parameters:
 *    @param[in]       *SpdData        - Pointer to the SPD data array
 *
 *    @retval          BOOLEAN
 *
 */
STATIC
BOOLEAN
IsNvHybridDimm (
  IN        UINT8                       *SpdData
  )
{
  SPD_BASE_CONFIG_0_S*  BaseConfig0;
  ASSERT (SpdData != NULL);

  BaseConfig0 = (SPD_BASE_CONFIG_0_S*) &(SpdData[SpdBlock_BaseConfig_0 * SPD_BLOCK_LEN]);
  return (BaseConfig0->KeyByte2.Field.Hybrid == SpdHybrid);
}

/**
 *  GetNvDimmSubsystemControllerId
 *
 *  Description:
 *     Get the ID of NVDIMM Subsystem Controller
 *
 *  Parameters:
 *    @param[in]       *SpdData        - Pointer to the SPD data array
 *
 *    @retval          UINT16          - The Identifier of NVDIMM Subsystem Controller
 *
 */
STATIC
UINT16
GetNvDimmSubsystemControllerId (
  IN        UINT8                       *SpdData
  )
{
  ASSERT (SpdData != NULL);

  // TODO NVDIMM Subsystem Controller ID
  return 0;
}


/**
 *  GetNvDimmSubsystemControllerManufacturerId
 *
 *  Description:
 *     Get the manufacturer ID of NVDIMM Subsystem Controller
 *
 *  Parameters:
 *    @param[in]       *SpdData        - Pointer to the SPD data array
 *
 *    @retval          UINT16          - The Manufacturer Identifier of NVDIMM Subsystem Controller
 *
 */
STATIC
UINT16
GetNvDimmSubsystemControllerManufacturerId (
  IN        UINT8                       *SpdData
  )
{
  SPD_NVDIMM_N_ANNEX_S   *ModuleParms;

  ASSERT (SpdData != NULL);
  ModuleParms = (SPD_NVDIMM_N_ANNEX_S *)&(SpdData[SpdBlock_ModuleParms_0 * SPD_BLOCK_LEN]);
  if (IsNvHybridDimm (SpdData)) {
    return ModuleParms->DbMfgId.Value;
  } else {
    return 0;
  }
}

/**
 *  GetNvDimmModuleProductId
 *
 *  Description:
 *     Get the module product ID of NVDIMM
 *
 *  Parameters:
 *    @param[in]       *SpdData        - Pointer to the SPD data array
 *
 *    @retval          UINT16          - The Module Product Identifier
 *
 */
STATIC
UINT16
GetNvDimmModuleProductId (
  IN        UINT8                       *SpdData
  )
{
  ASSERT (SpdData != NULL);

  // TODO NVDIMM Module Product ID
  return 0;
}

/**
 *  GetDimmModuleManufacturerId
 *
 *  Description:
 *     Get the module manufacturer ID
 *
 *  Parameters:
 *    @param[in]       *SpdData        - Pointer to the SPD data array
 *
 *    @retval          UINT16          - The Manufacturer Identifier
 *
 */
STATIC
UINT16
GetDimmModuleManufacturerId (
  IN        UINT8                       *SpdData
  )
{
  SPD_MANUFACTURING_INFO_S *MfgInfo;

  ASSERT (SpdData != NULL);
  MfgInfo     = (SPD_MANUFACTURING_INFO_S *)&(SpdData[SpdBlock_MfgInfo0 * SPD_BLOCK_LEN]);
  return MfgInfo->ModuleMfgId.Value;
}




/**
 *  GetDimmOperatingModeCap
 *
 *  Description:
 *     Get the operating mode capability of NVDIMM
 *
 *  Parameters:
 *    @param[in]       *SpdData        - Pointer to the SPD data array
 *
 *    @retval          UINT8
 *
 */
STATIC
UINT16
GetDimmOperatingModeCap (
  IN UINT8   *SpdData
  )
{
  DMI_T17_MEMORY_OPERATING_MODE_CAPABILITY  Capability;

  ASSERT (SpdData != NULL);

  Capability.MemOperatingModeCap = 0;

  if (IsNvHybridDimm (SpdData)) {
    // TODO supported operating modes for NVDIMM
  } else {
    //
    // DRAM
    //
    Capability.RegField.VolatileMemory = 1;
  }                     

  return Capability.MemOperatingModeCap;
}

/**
 *  GetDimmType
 *
 *  Description:
 *     Get the type of NVDIMM
 *
 *  Parameters:
 *    @param[in]       *SpdData        - Pointer to the SPD data array
 *
 *    @retval          UINT8
 *
 */
STATIC
UINT8
GetDimmType (
  IN UINT8   *SpdData
  )
{
  SPD_BASE_CONFIG_0_S*  BaseConfig0;

  ASSERT (SpdData != NULL);
  BaseConfig0 = (SPD_BASE_CONFIG_0_S*) &(SpdData[SpdBlock_BaseConfig_0 * SPD_BLOCK_LEN]);
  if (IsNvHybridDimm (SpdData)) {
    switch (BaseConfig0->KeyByte2.Field.HybridMedia) {
      case SpdHybridMediaNvdimmN:
        return NvDimmNType;
      case SpdHybridMediaNvdimmP:
        return NvDimmPType;
    }
  }
  return DramType;
}


/* -----------------------------------------------------------------------------*/
/**
 *  GetNvDimmFirmwareVersion
 *
 *  Description:
 *     Get the Firmware Version of NVDIMM Subsystem Controller
 *
 *  Parameters:
 *    @param[in]       *SpdData               - Pointer to the SPD data array
 *    @param[in, out]  *FirmwareVersion       - Firmware version
 *
 *    @retval
 *
 */
STATIC
VOID
GetNvDimmFirmwareVersion (
  IN        UINT8                       *SpdData,
  IN OUT    CHAR8                       *FirmwareVersion
  )
{
  ASSERT (SpdData != NULL);
  ASSERT (FirmwareVersion != NULL);

  // TODO NVDIMM Subsystem Controller FW Version
  FirmwareVersion[0] = '\0';
}

/* -----------------------------------------------------------------------------*/
/**
 *  InitSmbios32Type17
 *
 *  Description:
 *     Initialize the SMBIOS 3.2 fields of Type 17
 *
 *  Parameters:
 *    @param[in]       *DimmPresent         - DIMM Present
 *    @param[in]       *SpdData             - Pointer to the SPD data array
 *    @param[in, out]  *T17                 - Pointer to TYPE17_DMI_INFO
 *
 *    @retval          UINT8
 *
 */
VOID
InitSmbios32Type17 (
  IN      BOOLEAN                       DimmPresent,
  IN      UINT8                         *SpdData,
  IN OUT  TYPE17_DMI_INFO               *T17
  )
{
  UINT32         MemorySize;

  ASSERT (SpdData != NULL);
  ASSERT (T17 != NULL);

  if (DimmPresent) {
    T17->MemoryTechnology                           = GetDimmType (SpdData);
    T17->MemoryOperatingModeCapability.MemOperatingModeCap     = GetDimmOperatingModeCap (SpdData);
    GetNvDimmFirmwareVersion (SpdData, &T17->FirmwareVersion[0]);
    T17->ModuleManufacturerId                       = GetDimmModuleManufacturerId (SpdData);
    T17->ModuleProductId                            = GetNvDimmModuleProductId (SpdData);
    T17->MemorySubsystemControllerManufacturerId    = GetNvDimmSubsystemControllerManufacturerId (SpdData);
    T17->MemorySubsystemControllerProductId         = GetNvDimmSubsystemControllerId (SpdData);
    if (0x7FFF == T17->MemorySize) {
      MemorySize = T17->ExtSize;
    } else {
      MemorySize = T17->MemorySize;
    }
    if (IsNvHybridDimm (SpdData)) {
      T17->NonvolatileSize                          = MultU64x32 (MemorySize, 0x100000); // Convert Mega size to Byte size
      T17->VolatileSize                             = 0;
      T17->CacheSize                                = 0;
      T17->LogicalSize                              = 0;
    } else {
      T17->NonvolatileSize                          = 0;
      T17->VolatileSize                             = MultU64x32 (MemorySize, 0x100000); // Convert Mega size to Byte size
      T17->CacheSize                                = 0;
      T17->LogicalSize                              = 0;
    }
  } else {
    T17->MemoryTechnology                           = UnknownType;
    T17->MemoryOperatingModeCapability.RegField.Unknown = 1;
    T17->FirmwareVersion[0]                         = '\0';
    T17->ModuleManufacturerId                       = 0;
    T17->ModuleProductId                            = 0;
    T17->MemorySubsystemControllerManufacturerId    = 0;
    T17->MemorySubsystemControllerProductId         = 0;
    T17->NonvolatileSize                            = 0;
    T17->VolatileSize                               = 0;
    T17->CacheSize                                  = 0;
    T17->LogicalSize                                = 0;
  }
}
