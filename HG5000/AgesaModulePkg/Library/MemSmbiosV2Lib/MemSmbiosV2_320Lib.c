/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdMemDmiConstruct.c
 *
 * Memory DMI table support.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 329870 $ @e \$Date: 2015-10-29 13:13:29 +0800 (Thu, 29 Oct 2015) $
 *
 **/
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
  * ***************************************************************************
  *
 */

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "Library/IdsLib.h"
#include "mtspd4.h"
#include "MemDmi.h"
#include "Filecode.h"


CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEMSMBIOSV2LIB_MEMSMBIOSV2_320LIB_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

// Hybrid Memory parameters are defined in bytes 192~253

/* -----------------------------------------------------------------------------*/
/**
 *  CheckNvHybridDimm
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
CheckNvHybridDimm (
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
UINT8
GetDimmType (
  IN        UINT8                       *SpdBuffer
  )
{
  UINT16                                SpdOffset;
  UINT8                                 MemTechology;
  UINT8                                 FunctionClass_9_8;
  UINT8                                 FunctionClass_7_5;

  ASSERT (SpdBuffer != NULL);

  if(CheckNvHybridDimm (SpdBuffer) == FALSE)
  	return DramType; //DRAM

  //NV hybrid DIMM
  if ((SpdBuffer[SPD_NVDIMM_HYBRID_MODULE_MEDIA_TYPES_LSB] & NAND_MSK) == 0){
     ASSERT (FALSE);
  }
  if((SpdBuffer[SPD_NVDIMM_HYBRID_MODULE_MEDIA_TYPES_LSB] & SDRAM_MSK) == 0){
     MemTechology = NvDimmFType;
  }else{
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
       if(((FunctionClass_9_8 << 3) + FunctionClass_7_5) == 3){ 
	   	  //Byte addressable, no energy backed
	   	  MemTechology = NvDimmPType; 
       }else{
          MemTechology = NvDimmFType;
	   }
    }
  } 
  return MemTechology;
}

/* -----------------------------------------------------------------------------*/
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
UINT16
GetDimmOperatingModeCap (
  IN        UINT8                       *SpdBuffer
  )
{
  UINT16                                        SpdByte;
  UINT8                                         FunctionClass_9_8;
  UINT8                                         FunctionClass_7_5;
  DMI_T17_MEMORY_OPERATING_MODE_CAPABILITY      Capability;

  ASSERT (SpdBuffer != NULL);

  Capability.MemOperatingModeCap = 0;

  if (CheckNvHybridDimm (SpdBuffer)) {
    for (SpdByte = SPD_NVDIMM_FUNCTION_0_INTERFACE_DESCRIPTORS_LSB; SpdByte <= SPD_NVDIMM_FUNCTION_7_INTERFACE_DESCRIPTORS_LSB; SpdByte += 2) {
      //
      // Function Class Bit 9-8 defined as always 0 in Table 161 - Function Classess and Function Interfaces
      //
      if (0 == (SpdBuffer[SpdByte + 1] & FUNCTION_INTERFACE_IMPLEMENTED_MSK )) {
        //
        // No more functions
        //
        break;
      }

      FunctionClass_9_8 = (SpdBuffer[SpdByte + 1] & FUNCTION_CLASS_HIGH_MSK) >> FUNCTION_CLASS_HIGH_SHIFT;
	  FunctionClass_7_5 = (SpdBuffer[SpdByte] & FUNCTION_CLASS_LOW_MSK ) >> FUNCTION_CLASS_LOW_SHIFT;
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
VOID
GetNvDimmFirmwareVersion (
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
  if (CheckNvHybridDimm (SpdBuffer)) {
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
GetDimmModuleManufacturerId (
  IN        UINT8                       *SpdBuffer
  )
{
  ASSERT (SpdBuffer != NULL);

  return (SpdBuffer[SPD_MANUFACTURER_ID_MSB] << 8) + SpdBuffer[SPD_MANUFACTURER_ID_LSB];
}

/* -----------------------------------------------------------------------------*/
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
UINT16
GetNvDimmModuleProductId (
  IN        UINT8                       *SpdBuffer
  )
{
  ASSERT (SpdBuffer != NULL);

  if (CheckNvHybridDimm (SpdBuffer)) {
    return (SpdBuffer[SPD_NVDIMM_MODULE_PRODUCT_ID_MSB] << 8) + SpdBuffer[SPD_NVDIMM_MODULE_PRODUCT_ID_LSB];
  } else {
    return 0;
  }
}

/* -----------------------------------------------------------------------------*/
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

UINT16
GetNvDimmSubsystemControllerId (
  IN        UINT8                       *SpdBuffer
  )
{
  ASSERT (SpdBuffer != NULL);

  if (CheckNvHybridDimm (SpdBuffer)) {
    return (SpdBuffer[SPD_NVDIMM_SUBSYSTEM_CONTROLLER_ID_MSB] << 8) + SpdBuffer[SPD_NVDIMM_SUBSYSTEM_CONTROLLER_ID_LSB];
  } else {
    return 0;
  }
}

/* -----------------------------------------------------------------------------*/
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
UINT16
GetNvDimmSubsystemControllerManufacturerId (
  IN        UINT8                       *SpdBuffer
  )
{
  ASSERT (SpdBuffer != NULL);

  if (CheckNvHybridDimm (SpdBuffer)) {
    return (SpdBuffer[SPD_NVDIMM_SUBSYSTEM_CONTROLLER_MANUFACTURER_ID_MSB] << 8) + SpdBuffer[SPD_NVDIMM_SUBSYSTEM_CONTROLLER_MANUFACTURER_ID_LSB];
  } else {
    return 0;
  }
}


