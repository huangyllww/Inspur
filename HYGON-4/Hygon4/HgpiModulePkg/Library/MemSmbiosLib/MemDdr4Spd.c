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
#include "mtspd4.h"
#include "MemDmi.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include <HPCB.h>
#include "Filecode.h"

#define FILECODE  LIBRARY_MEMSMBIOSLIB_MEMDDR4SPD_FILECODE

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/

/**
 *  CheckNvHybridDimmDdr4
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
BOOLEAN
CheckNvHybridDimmDdr4 (
  IN        UINT8                       *SpdBuffer
  )
{
  ASSERT (SpdBuffer != NULL);

  if ((SpdBuffer[SPD_BASE_MODULE_TYPE] & MOD_TYPE_HYBRID) == 0x80 && (SpdBuffer[SPD_BASE_MODULE_TYPE] & MOD_TYPE_HYBRID_TYPE) == 0x10) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/* -----------------------------------------------------------------------------*/

/**
 *  GetDimmDdr4Type
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
UINT8
GetDimmDdr4Type (
  IN        UINT8                       *SpdBuffer
  )
{
  UINT16  SpdOffset;
  UINT8   MemTechology;
  UINT8   FunctionClass_9_8;
  UINT8   FunctionClass_7_5;

  ASSERT (SpdBuffer != NULL);

  MemTechology = DramType;
  if(CheckNvHybridDimmDdr4 (SpdBuffer) == FALSE) {
    return DramType; // DRAM
  }

  // NV hybrid DIMM
  if ((SpdBuffer[SPD_NVDIMM_HYBRID_MODULE_MEDIA_TYPES_LSB] & NAND_MSK) == 0) {
    ASSERT (FALSE);
  }

  if((SpdBuffer[SPD_NVDIMM_HYBRID_MODULE_MEDIA_TYPES_LSB] & SDRAM_MSK) == 0) {
    MemTechology = NvDimmFType;
  } else {
    //
    // NAND Flash + DRAM
    //
    for (SpdOffset = SPD_NVDIMM_FUNCTION_0_INTERFACE_DESCRIPTORS_LSB; SpdOffset <= SPD_NVDIMM_FUNCTION_7_INTERFACE_DESCRIPTORS_LSB; SpdOffset += 2) {
      if (0 == (SpdBuffer[SpdOffset + 1] & FUNCTION_INTERFACE_IMPLEMENTED_MSK)) {
        //
        // No more functions
        //
        break;
      }

      FunctionClass_9_8 = SpdBuffer[SpdOffset + 1]  & FUNCTION_CLASS_HIGH_MSK;
      FunctionClass_7_5 = SpdBuffer[SpdOffset] & FUNCTION_CLASS_LOW_MSK;
      if(((FunctionClass_9_8 << 3) + FunctionClass_7_5) == 3) {
        // Byte addressable, no energy backed
        MemTechology = NvDimmPType;
      } else {
        MemTechology = NvDimmFType;
      }
    }
  }

  return MemTechology;
}

/* -----------------------------------------------------------------------------*/

/**
 *  GetDimmDdr4OperatingModeCap
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
UINT16
GetDimmDdr4OperatingModeCap (
  IN        UINT8                       *SpdBuffer
  )
{
  UINT16                                    SpdByte;
  UINT8                                     FunctionClass_9_8;
  UINT8                                     FunctionClass_7_5;
  DMI_T17_MEMORY_OPERATING_MODE_CAPABILITY  Capability;

  ASSERT (SpdBuffer != NULL);

  Capability.MemOperatingModeCap = 0;

  if (CheckNvHybridDimmDdr4 (SpdBuffer)) {
    for (SpdByte = SPD_NVDIMM_FUNCTION_0_INTERFACE_DESCRIPTORS_LSB; SpdByte <= SPD_NVDIMM_FUNCTION_7_INTERFACE_DESCRIPTORS_LSB; SpdByte += 2) {
      //
      // Function Class Bit 9-8 defined as always 0 in Table 161 - Function Classess and Function Interfaces
      //
      if (0 == (SpdBuffer[SpdByte + 1] & FUNCTION_INTERFACE_IMPLEMENTED_MSK)) {
        //
        // No more functions
        //
        break;
      }

      FunctionClass_9_8 = (SpdBuffer[SpdByte + 1] & FUNCTION_CLASS_HIGH_MSK) >> FUNCTION_CLASS_HIGH_SHIFT;
      FunctionClass_7_5 = (SpdBuffer[SpdByte] & FUNCTION_CLASS_LOW_MSK) >> FUNCTION_CLASS_LOW_SHIFT;
      switch ((FunctionClass_9_8 << 3) + FunctionClass_7_5) {
        case 0:
          //
          // Undefined function
          //
          break;
        case 1:
          //
          // Byte addressable energy backed
          //
          Capability.RegField.ByteAccessiblePersistentMemory = 1;
          break;
        case 2:
          //
          // Block addressed
          //
          Capability.RegField.BlockAccessiblePersistentMemory = 1;
          break;
        case 3:
          //
          // Byte addressable, no energy backed
          //
          Capability.RegField.ByteAccessiblePersistentMemory = 1;
          break;
      }
    }

    Capability.RegField.VolatileMemory = 1;
  } else {
    //
    // DRAM
    //
    Capability.RegField.VolatileMemory = 1;
  }

  return Capability.MemOperatingModeCap;
}

/* -----------------------------------------------------------------------------*/

/**
 *  GetNvDimmDdr4FirmwareVersion
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
VOID
GetNvDimmDdr4FirmwareVersion (
  IN        UINT8                       *SpdBuffer,
  IN OUT    CHAR8                        *FirmwareVersion
  )
{
  UINT16  Slot0_Rev;
  UINT16  Slot1_Rev;
  UINT16  valid_Slot_Rev;
  UINT8   i;
  UINT8   data8;

  ASSERT (SpdBuffer != NULL);
  ASSERT (FirmwareVersion != NULL);

  // Function PlatformDimmSpdRead save NVDIMM-N SLOT0_FWREV0, SLOT0_FWREV1, SLOT1_FWREV0 & SLOT1_FWREV1 at SpdData[508 .. 511]
  if (CheckNvHybridDimmDdr4 (SpdBuffer)) {
    Slot0_Rev = (SpdBuffer[SPD_BYTE_OFFSET_509] << 8) | SpdBuffer[SPD_BYTE_OFFSET_508];
    Slot1_Rev = (SpdBuffer[SPD_BYTE_OFFSET_511] << 8) | SpdBuffer[SPD_BYTE_OFFSET_510];
    IDS_HDT_CONSOLE (MAIN_FLOW, "\tNVDIMM SLOT0_FWREV = %x, SLOT1_FWREV = %x\n", Slot0_Rev, Slot1_Rev);

    valid_Slot_Rev = Slot0_Rev;
    if (0 != Slot1_Rev) {
      valid_Slot_Rev = Slot1_Rev;
    }

    // Convert BCD to ASCII string
    for (i = 0; i < 4; i++) {
      data8 = (valid_Slot_Rev >> (4 * i)) & 0xf;
      if (data8 <= 9) {
        FirmwareVersion[3-i] = data8 + 0x30;
      } else {
        FirmwareVersion[3-i] = (data8 - 10) + 0x41;
      }
    }
  } else {
    FirmwareVersion[0] = '\0';
  }
}

/* -----------------------------------------------------------------------------*/

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
UINT16
GetDimmDdr4ModuleManufacturerId (
  IN        UINT8                       *SpdBuffer
  )
{
  ASSERT (SpdBuffer != NULL);

  return (SpdBuffer[SPD_MODULE_MANUFACTURER_ID_CODE_MSB] << 8) + SpdBuffer[SPD_MODULE_MANUFACTURER_ID_CODE_LSB];
}

/* -----------------------------------------------------------------------------*/

/**
 *  GetNvDimmDdr4ModuleProductId
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
UINT16
GetNvDimmDdr4ModuleProductId (
  IN        UINT8                       *SpdBuffer
  )
{
  ASSERT (SpdBuffer != NULL);

  if (CheckNvHybridDimmDdr4 (SpdBuffer)) {
    return (SpdBuffer[SPD_NVDIMM_MODULE_PRODUCT_ID_MSB] << 8) + SpdBuffer[SPD_NVDIMM_MODULE_PRODUCT_ID_LSB];
  } else {
    return 0;
  }
}

/* -----------------------------------------------------------------------------*/

/**
 *  GetNvDimmDdr4SubsystemControllerId
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
UINT16
GetNvDimmDdr4SubsystemControllerId (
  IN        UINT8                       *SpdBuffer
  )
{
  ASSERT (SpdBuffer != NULL);

  if (CheckNvHybridDimmDdr4 (SpdBuffer)) {
    return (SpdBuffer[SPD_NVDIMM_SUBSYSTEM_CONTROLLER_ID_MSB] << 8) + SpdBuffer[SPD_NVDIMM_SUBSYSTEM_CONTROLLER_ID_LSB];
  } else {
    return 0;
  }
}

/* -----------------------------------------------------------------------------*/

/**
 *  GetNvDimmDdr4SubsystemControllerManufacturerId
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
UINT16
GetNvDimmDdr4SubsystemControllerManufacturerId (
  IN        UINT8                       *SpdBuffer
  )
{
  ASSERT (SpdBuffer != NULL);

  if (CheckNvHybridDimmDdr4 (SpdBuffer)) {
    return (SpdBuffer[SPD_NVDIMM_SUBSYSTEM_CONTROLLER_MANUFACTURER_ID_MSB] << 8) + SpdBuffer[SPD_NVDIMM_SUBSYSTEM_CONTROLLER_MANUFACTURER_ID_LSB];
  } else {
    return 0;
  }
}

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

HGPI_STATUS
ParseDdr4SpdToInitT17 (
  IN UINT8                 *DimmSpd,
  IN OUT   TYPE17_DMI_INFO *T17
  )
{
  UINT16  i;
  UINT16  Capacity;
  UINT16  BusWidth;
  UINT16  EccWidth;
  UINT16  DeviceWidth;
  UINT8   Rank;
  UINT32  MemorySize;
  INT32   MTB_ps;
  INT32   FTB_ps;
  INT32   Value32;
  UINT8   VoltageMap;
  UINT8   DieCount = 0;

  IDS_HDT_CONSOLE (MAIN_FLOW, "ParseDdr4SpdToInitT17 Entry \n");

  // Set DIMM data width and total width
  BusWidth = 1 << ((DimmSpd[SPD_BUS_WIDTH] & BUSWIDTH_MASK) + 3);
  EccWidth = 8 * ((DimmSpd[SPD_BUSWIDTH_EXT] >> SPD_BUSWIDTH_EXT_SHIFT) & SPD_BUSWIDTH_EXT_MASK);
  T17->DataWidth  = BusWidth;
  T17->TotalWidth = BusWidth + EccWidth;

  IDS_HDT_CONSOLE (MAIN_FLOW, "DataWidth = %d, TotalWidth = %d \n", T17->DataWidth, T17->TotalWidth);

  // DDR4 DIMM memory size = DensitySize * DensityNumber * RankNumber
  Capacity = 0x100 << (DimmSpd[SPD_CAPACITY] & CAPACITY_MASK);      // Mb
  ASSERT (Capacity <= 0x8000);

  DeviceWidth = 4 << (DimmSpd[SPD_DEVICE_WIDTH] & DEVICE_WIDTH_MASK);
  ASSERT (DeviceWidth <= 32);

  Rank = 1 + (DimmSpd[SPD_RANKS] >> RANKS_SHIFT) & RANKS_MASK;
  ASSERT (Rank <= 4);

  if ((DimmSpd[SPD_PACKAGE_TYPE] & SIGNAL_LOADING_MASK) == SINGLE_LOAD_STACK_TYPE) {
    DieCount = ((DimmSpd[SPD_DIE_COUNT] >> SPD_DIE_COUNT_SHIFT) & SPD_DIE_COUNT_MASK) + 1;
    Rank     = Rank * DieCount;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "Rank = %d \n", Rank);

  MemorySize = (Capacity / 8) * (BusWidth / DeviceWidth) * Rank;    // MB
  IDS_HDT_CONSOLE (MAIN_FLOW, "MemorySize = %d MB \n", MemorySize);

  // Set DIMM memory size
  if (MemorySize < 0x7FFF) {
    T17->MemorySize = (UINT16)MemorySize;
    T17->ExtSize    = 0;
  } else {
    T17->MemorySize = 0x7FFF;
    T17->ExtSize    = MemorySize;
  }

  // Set type detail
  T17->TypeDetail.Synchronous = 1;

  switch (DimmSpd[SPD_BASE_MODULE_TYPE] & MODULE_TYPE_MASK) {
    case MOD_TYPE_RDIMM:
    case MOD_TYPE_LRDIMM:
      T17->TypeDetail.Registered = 1;
      T17->FormFactor = DimmFormFactorFormFactor;
      break;
    case MOD_TYPE_UDIMM:
      T17->TypeDetail.Unbuffered = 1;
      T17->FormFactor = DimmFormFactorFormFactor;
      break;
    case MOD_TYPE_SODIMM:
    case MOD_TYPE_72b_SO_RDIMM:
    case MOD_TYPE_72b_SO_UDIMM:
    case MOD_TYPE_16b_SODIMM:
    case MOD_TYPE_32b_SODIMM:
      T17->TypeDetail.Unbuffered = 1;
      T17->FormFactor = SodimmFormFactor;
      break;
    default:
      T17->TypeDetail.Unknown = 1;
      T17->FormFactor = UnknowFormFactor;
  }

  // tCKAVGAMIN = MTB * MTB Unit(0.125ns) + FTB * FTB Unit(0.001ns)
  MTB_ps  = (((DimmSpd[SPD_MEDIUM_TIMEBASE] >> MTB_SHIFT) & MTB_MSK) == 0) ? 125 : 0;
  FTB_ps  = ((DimmSpd[SPD_FINE_TIMEBASE] & FTB_MSK) == 0) ? 1 : 0;
  Value32 = (MTB_ps * DimmSpd[SPD_TCK]) + (FTB_ps * (INT8)DimmSpd[SPD_TCK_FTB]);

  // Set DIMM speed
  if ((Value32 <= 625)) {
    T17->Speed = 1600;                 // DDR4-3200
  } else if (Value32 <= 682) {
    T17->Speed = 1467;                 // DDR4-2933
  } else if (Value32 <= 750) {
    T17->Speed = 1333;                 // DDR4-2667
  } else if (Value32 <= 834) {
    T17->Speed = 1200;                 // DDR4-2400
  } else if (Value32 <= 938) {
    T17->Speed = 1067;                 // DDR4-2133
  } else if (Value32 <= 1071) {
    T17->Speed = 933;                  // DDR4-1866
  } else if (Value32 <= 1250) {
    T17->Speed = 800;                  // DDR4-1600
  } else {
    T17->Speed = 667;                  // DDR4-1333
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "Speed = %d \n", T17->Speed);

  // Set Manufacturer
  T17->ManufacturerIdCode = (DimmSpd[SPD_MODULE_MANUFACTURER_ID_CODE_MSB] << 8) | DimmSpd[SPD_MODULE_MANUFACTURER_ID_CODE_LSB];

  // Set Serial Number
  IntToString (T17->SerialNumber, &DimmSpd[SPD_MODULE_SERIAL_NUMBER], (sizeof (T17->SerialNumber) - 1) / 2);

  // Set Part Number
  for (i = 0; i < SPD_MODULE_PART_NUMBER_LEN; i++) {
    T17->PartNumber[i] = DimmSpd[i + SPD_MODULE_PART_NUMBER];
  }

  T17->PartNumber[i] = 0;

  // Set Rank Number
  T17->Attributes = Rank;

  // Set Voltage
  VoltageMap = DimmSpd[SPD_DRAM_VDD];
  T17->MinimumVoltage = CONVERT_ENCODED_TO_VDDIO_MILLIVOLTS ((HighBitSet32 (VoltageMap) & 0xFE), DDR4_TECHNOLOGY);
  T17->MaximumVoltage = CONVERT_ENCODED_TO_VDDIO_MILLIVOLTS ((LowBitSet32 (VoltageMap) & 0xFE), DDR4_TECHNOLOGY);

  // add for smbios 3.2
  T17->MemoryTechnology = GetDimmDdr4Type (DimmSpd);
  T17->MemoryOperatingModeCapability.MemOperatingModeCap = GetDimmDdr4OperatingModeCap (DimmSpd);
  GetNvDimmDdr4FirmwareVersion (DimmSpd, &T17->FirmwareVersion[0]);
  T17->ModuleManufacturerId = GetDimmDdr4ModuleManufacturerId (DimmSpd);
  T17->ModuleProductId = GetNvDimmDdr4ModuleProductId (DimmSpd);
  T17->MemorySubsystemControllerManufacturerId = GetNvDimmDdr4SubsystemControllerManufacturerId (DimmSpd);
  T17->MemorySubsystemControllerProductId = GetNvDimmDdr4SubsystemControllerId (DimmSpd);
  if (0x7FFF == T17->MemorySize) {
    MemorySize = T17->ExtSize;
  } else {
    MemorySize = T17->MemorySize;
  }

  if (CheckNvHybridDimmDdr4 (DimmSpd)) {
    T17->NonvolatileSize = 0xffffffffffffffff;
    T17->VolatileSize    = (UINT64)MemorySize * 0x100000;
    T17->CacheSize   = 0;
    T17->LogicalSize = 0;
    // add for smbios 3.3
    T17->ExtendedSpeed = 0;
    T17->ExtendedConfiguredMemorySpeed = 0;
  } else {
    T17->NonvolatileSize = 0;
    T17->VolatileSize    = (UINT64)MemorySize * 0x100000;
    T17->CacheSize   = 0;
    T17->LogicalSize = 0;
    // add for smbios 3.3
    T17->ExtendedSpeed = 0;
    T17->ExtendedConfiguredMemorySpeed = 0;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "ParseDdr4SpdToInitT17 Exit \n");

  return HGPI_SUCCESS;
}

HGPI_STATUS
ParseDdr4SpdToInitNvdimmSpdInfo (
  IN UINT8                    *DimmSpd,
  IN TYPE17_DMI_INFO          *T17,
  IN OUT  NVDIMM_SPD_INFO    **NvdimmSpdInfo
  )
{
  IDS_HDT_CONSOLE (MAIN_FLOW, "ParseDdr4SpdToInitNvdimmSpdInfo Entry \n");

  // Save NVDIMM SPD data to NvdimmSpdInfo
  if ((HYBRID_TYPE == ((DimmSpd[SPD_HYBRID] >> HYBRID_SHIFT) & HYBRID_MASK)) &&
      (NVDIMM_HYBRID == ((DimmSpd[SPD_HYBRID_MEDIA] >> HYBRID_MEDIA_SHIFT) & HYBRID_MEDIA_MASK))) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "NVDIMM present ! \n");

    (*NvdimmSpdInfo)->DataValid = 0x55aa;
    (*NvdimmSpdInfo)->Handle    = T17->Handle;
    (*NvdimmSpdInfo)->DeviceId  = ((UINT16)DimmSpd[193] << 8) | (UINT16)DimmSpd[192];
    (*NvdimmSpdInfo)->SubSysVendorId        = ((UINT16)DimmSpd[195] << 8) | (UINT16)DimmSpd[194];
    (*NvdimmSpdInfo)->SubSysDeviceId        = ((UINT16)DimmSpd[197] << 8) | (UINT16)DimmSpd[196];
    (*NvdimmSpdInfo)->SubSysRevisionId      = (UINT16)DimmSpd[198];
    (*NvdimmSpdInfo)->ManufacturingIdCode   = ((UINT16)DimmSpd[SPD_MODULE_MANUFACTURER_ID_CODE_MSB] << 8) | (UINT16)DimmSpd[SPD_MODULE_MANUFACTURER_ID_CODE_LSB];
    (*NvdimmSpdInfo)->ManufacturingLocation = (UINT16)DimmSpd[SPD_MODULE_MANUFACTURING_LOCATION];
    (*NvdimmSpdInfo)->ManufacturingDate     = ((UINT16)DimmSpd[SPD_MODULE_MANUFACTURING_WEEK] << 8) | (UINT16)DimmSpd[SPD_MODULE_MANUFACTURING_YEAR];
    (*NvdimmSpdInfo)->SerialNumber[0]       = DimmSpd[SPD_MODULE_SERIAL_NUMBER];
    (*NvdimmSpdInfo)->SerialNumber[1]       = DimmSpd[SPD_MODULE_SERIAL_NUMBER + 1];
    (*NvdimmSpdInfo)->SerialNumber[2]       = DimmSpd[SPD_MODULE_SERIAL_NUMBER + 2];
    (*NvdimmSpdInfo)->SerialNumber[3]       = DimmSpd[SPD_MODULE_SERIAL_NUMBER + 3];
    (*NvdimmSpdInfo)->RevisionId = (UINT16)DimmSpd[SPD_MODULE_REVISION_CODE];
    (*NvdimmSpdInfo)++;      // Move pointer to next available location
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "ParseDdr4SpdToInitNvdimmSpdInfo Exit \n");
  return HGPI_SUCCESS;
}
