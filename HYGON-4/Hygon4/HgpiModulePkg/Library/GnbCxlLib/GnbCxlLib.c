/* $NoKeywords:$ */
/**
 * @file
 *
 * CXL Base lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: CXL
 * @e \$Revision: 309090 $   @e \$Date: 2022-08-04 16:28:05 -0800 (Thurs, 04 Aug 2022) $
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/GnbPciLib.h>
#include <Library/GnbMemAccLib.h>
#include <Library/GnbCxlLib.h>
#include <Library/BaseLib.h>

#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_CXLBASELIB_CXLBASELIB_FILECODE



/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/*
 * Find CXL DVSEC capability pointer in PCI CFG register space
 *
 *
 *
 * @param[in] Address               PCI address (as described in PCI_ADDR)
 * @param[in] ExtendedCapabilityId  CXL DVSEC capability ID
 * @param[in] StdHeader             Standard configuration header
 * @retval                          Register address of CXL DVSEC capability pointer
 *
 */
UINT16
FindPciCxlDvsecCapability (
    IN      UINT32              PciAddress,
    IN      UINT16              CxlDvsecId,
    IN      HYGON_CONFIG_PARAMS   *StdHeader
)
{
  UINT16  CapabilityPtr;
  UINT32  ExtendedCapabilityIdBlock;
  UINT16  TempDvsecVendorId;
  UINT16  TempDvsecId;

  IDS_HDT_CONSOLE (GNB_TRACE, "Found CXL DVSEC CAP ID %d : \n", CxlDvsecId);

  if (GnbLibPciIsPcieDevice (PciAddress, StdHeader)) {
    CapabilityPtr = 0x100;
    GnbLibPciRead (PciAddress | CapabilityPtr , AccessWidth32 , &ExtendedCapabilityIdBlock, StdHeader);
    if ((ExtendedCapabilityIdBlock != 0) && ((UINT16)ExtendedCapabilityIdBlock != 0xffff)) {
      do {
        GnbLibPciRead (PciAddress | CapabilityPtr , AccessWidth32 , &ExtendedCapabilityIdBlock, StdHeader);
        //IDS_HDT_CONSOLE (GNB_TRACE, "   - Capability at 0x%x with type 0x%x\n", CapabilityPtr, (UINT16)ExtendedCapabilityIdBlock);

        if ((UINT16)ExtendedCapabilityIdBlock == DVSEC_CAP_ID) {
          GnbLibPciRead (PciAddress | (CapabilityPtr + 0x04) , AccessWidth16 , &TempDvsecVendorId, StdHeader);           
          if ((TempDvsecVendorId == CXL20_DVSEC_CAP_VID) || (TempDvsecVendorId == INTEL_DVSEC_CAP_VID)) {
            GnbLibPciRead (PciAddress | (CapabilityPtr + 0x08) , AccessWidth16 , &TempDvsecId, StdHeader);
            if (TempDvsecId == CxlDvsecId) {
              IDS_HDT_CONSOLE (GNB_TRACE, "  DVSEC Capability at 0x%x with type 0x%x\n", CapabilityPtr, (UINT16)ExtendedCapabilityIdBlock);
              return  CapabilityPtr;
            }
          }
        }
        CapabilityPtr = (UINT16) ((ExtendedCapabilityIdBlock >> 20) & 0xfff);
      } while (CapabilityPtr !=  0);
    }
  }
  return  0;
}

 /*----------------------------------------------------------------------------------------*/
 /*
  * Find RCRB PCI capability pointer
  *
  *
  *
  * @param[in] RCRB Address          RCRB address
  * @param[in] ExtendedCapabilityId  Extended PCIe capability ID
  * @retval                          Register address of CXL DVSEC capability pointer
  *
  */
UINT8
FindRcrbPciCapability (
    IN      UINT64              RcrbAddress,
    IN      UINT16              CapabilityId,
    IN      HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8     CapabilityPtr;
  UINT8     CurrentCapabilityId;
  
  CapabilityPtr = 0x34;
  while ((CapabilityPtr != 0) && (CapabilityPtr != 0xff)) {
    GnbLibMemRead (RcrbAddress | CapabilityPtr , AccessWidth8 , &CapabilityPtr, StdHeader);
    if (CapabilityPtr != 0) {
      GnbLibMemRead (RcrbAddress | CapabilityPtr , AccessWidth8 , &CurrentCapabilityId, StdHeader);
      if (CurrentCapabilityId == CapabilityId) {
        IDS_HDT_CONSOLE (GNB_TRACE, "   - Capability at 0x%x with type 0x%x\n", CapabilityPtr, CurrentCapabilityId);
        return CapabilityPtr;
      }
      CapabilityPtr++;
    }
  }
  return  0;
}

 /*----------------------------------------------------------------------------------------*/
 /*
  * Find RCRB extended PCI capability pointer
  *
  *
  *
  * @param[in] RCRB Address          RCRB address
  * @param[in] ExtendedCapabilityId  Extended PCIe capability ID
  * @retval                          Register address of CXL DVSEC capability pointer
  *
  */
UINT16
FindRcrbExtendedCapability (
  IN      UINT64              RcrbAddress,
  IN      UINT16              ExtendedCapabilityId,
  IN      HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  UINT16                            CapabilityPtr;
  PCIE_EXTENDED_CAPABILITY_HEADER   ExtendedCapabilityIdBlock;

  CapabilityPtr = 0;
  GnbLibMemRead (RcrbAddress | CapabilityPtr , AccessWidth32 , &ExtendedCapabilityIdBlock.Value, StdHeader);
  
  //Check first DSP RCRB PCIe extended capability header
  if ((ExtendedCapabilityIdBlock.Field.PcieExtCapNextCapOffet != 0) && ((UINT16)ExtendedCapabilityIdBlock.Field.PcieExtCapId == 0)) {
    do {
      CapabilityPtr = (UINT16)ExtendedCapabilityIdBlock.Field.PcieExtCapNextCapOffet;
      GnbLibMemRead (RcrbAddress | CapabilityPtr , AccessWidth32 , &ExtendedCapabilityIdBlock.Value, StdHeader);
      if (ExtendedCapabilityIdBlock.Field.PcieExtCapId == (UINT32)ExtendedCapabilityId) {
        IDS_HDT_CONSOLE (GNB_TRACE, "   - Capability at 0x%x with type 0x%x\n", CapabilityPtr, ExtendedCapabilityIdBlock.Field.PcieExtCapId);
        return  CapabilityPtr;
      }
    } while ((CapabilityPtr != 0) && (CapabilityPtr != 0xfff));
  }
  return 0;
}

 
/*----------------------------------------------------------------------------------------*/
/*
 * Find CXL DVSEC capability pointer in RCRB space
 *
 *
 *
 * @param[in] Address               RCRB MMIO Address
 * @param[in] ExtendedCapabilityId  CXL DVSEC capability ID
 * @param[in] StdHeader             Standard configuration header
 * @retval                          Register address of CXL DVSEC capability pointer
 *
 */
UINT16
FindRcrbCxlDvsecCapability (
    IN      UINT64              RcrbAddress,
    IN      UINT16              CxlDvsecId,
    IN      HYGON_CONFIG_PARAMS   *StdHeader
)
{
  UINT16  CapabilityPtr;
  UINT32  ExtendedCapabilityIdBlock;
  UINT16  TempDvsecVendorId;
  UINT16  TempDvsecId;

  CapabilityPtr = 0x100;
  GnbLibMemRead (RcrbAddress | CapabilityPtr , AccessWidth32 , &ExtendedCapabilityIdBlock, StdHeader);
  if ((ExtendedCapabilityIdBlock != 0) && ((UINT16)ExtendedCapabilityIdBlock != 0xffff)) {
    do {
      GnbLibMemRead (RcrbAddress | CapabilityPtr , AccessWidth32 , &ExtendedCapabilityIdBlock, StdHeader);
      IDS_HDT_CONSOLE (GNB_TRACE, "  CXL 1.1 DVSEC CAP at 0x%x with type 0x%x\n", CapabilityPtr, (UINT16)ExtendedCapabilityIdBlock);

      if ((UINT16)ExtendedCapabilityIdBlock == DVSEC_CAP_ID) {
        GnbLibMemRead (RcrbAddress | (CapabilityPtr + 0x04) , AccessWidth16 , &TempDvsecVendorId, StdHeader);           
        if ((TempDvsecVendorId == CXL20_DVSEC_CAP_VID) || (TempDvsecVendorId == INTEL_DVSEC_CAP_VID)) {
          GnbLibMemRead (RcrbAddress | (CapabilityPtr + 0x08) , AccessWidth16 , &TempDvsecId, StdHeader);
          if (TempDvsecId == CxlDvsecId) {
            return  CapabilityPtr;
          }
        }
      }
      CapabilityPtr = (UINT16) ((ExtendedCapabilityIdBlock >> 20) & 0xfff);
    } while (CapabilityPtr !=  0);
  }
  return  0;
}
  
 /**
  * Check is CXL memory device
 @param[in] CxlDev  CXL device PCI address
 
 @retval    TRUE    Is CXL device
            FALSE   Not CXL device
 **/
BOOLEAN IsCxlDevice(
     IN     PCI_ADDR               CxlDev
)
{
   UINT32    DeviceId;
   UINT8     BaseClassCode;
   UINT8     SubClassCode;
   UINT8     ProgInterface;

   GnbLibPciRead (CxlDev.AddressValue, AccessWidth32, &DeviceId, NULL);  
   IDS_HDT_CONSOLE (MAIN_FLOW, "[B%X|D%X|F%X] VIDDID: 0x%08X \n", 
       CxlDev.Address.Bus, 
       CxlDev.Address.Device, 
       CxlDev.Address.Function, 
       DeviceId);

   if (DeviceId != 0xFFFFFFFF) {
     GnbLibPciRead (CxlDev.AddressValue | BASE_CLASSCODE_OFFSET, AccessWidth8, &BaseClassCode, NULL);  
     GnbLibPciRead (CxlDev.AddressValue | SUB_CLASSCODE_OFFSET, AccessWidth8, &SubClassCode, NULL); 
     GnbLibPciRead (CxlDev.AddressValue | PROG_INTERFACE_OFFSET, AccessWidth8, &ProgInterface, NULL); 

     IDS_HDT_CONSOLE (MAIN_FLOW, "BaseClassCode %d \n", BaseClassCode);    
     IDS_HDT_CONSOLE (MAIN_FLOW, "SubClassCode %d \n", SubClassCode);    
     IDS_HDT_CONSOLE (MAIN_FLOW, "ProgInterface %d \n", ProgInterface);    
     if ((BaseClassCode == 0x05) && (SubClassCode == 0x02)) {
       IDS_HDT_CONSOLE (MAIN_FLOW, "is CXL device \n");
       return TRUE;
     }
   }

   IDS_HDT_CONSOLE (MAIN_FLOW, "is not CXL device \n");    
   return FALSE;
}
 
 /**
  * Find CXL 2.0 USP, DSP or device register block BAR offset
 @param[in] PciAddress    CXL 2.0 USP/DSP/Device PCI address
 @param[in] BlockType     Register Block Type

 @retval
  **/
VOID
FindCxl20RegBlock (
       IN     PCI_ADDR                           PciAddress,
       IN     CXL_REGISTER_BLOCK_TYPE            BlockType,
       OUT    CXL_REGISTER_BLOCK_INFO           *BlockInfo
)
{
  UINT16            DeviceLocatorCapPtr;
  UINT32            RegisterBlockOffsetLow;
  UINT16            BlockIndex;
  PCI_ADDR          LocalPciAddress;
  UINT32            Value;
  
  DeviceLocatorCapPtr = FindPciCxlDvsecCapability (PciAddress.AddressValue, CXL_2_0_REGISTER_LOCATOR_DVSEC_ID, NULL);
  if (DeviceLocatorCapPtr != 0) {
    for (BlockIndex = 0; BlockIndex < 3; BlockIndex++) {
      GnbLibPciRead ( PciAddress.AddressValue | (DeviceLocatorCapPtr + REGISTER_BLOCK1_LOW_OFFSET + BlockIndex * 8), 
               AccessWidth32, 
               &RegisterBlockOffsetLow, 
               NULL);
      //IDS_HDT_CONSOLE (MAIN_FLOW, "   DVSEC 8 RegisterBlockOffsetLow 0x%X \n", RegisterBlockOffsetLow); 
      if (((RegisterBlockOffsetLow >> 8) & 0xFF) == (UINT32)BlockType) {
        BlockInfo->BlockOffset = (UINT64)(RegisterBlockOffsetLow & 0xFFFF0000);
        BlockInfo->BarNo = RegisterBlockOffsetLow & 0x07;
        BlockInfo->BarOffset = 0x10 +  BlockInfo->BarNo * 0x04;
        
        LocalPciAddress = PciAddress;
        LocalPciAddress.Address.Register = BlockInfo->BarOffset;
        GnbLibPciRead (LocalPciAddress.AddressValue, AccessWidth32, &Value, NULL); 
        BlockInfo->BarAddress = (UINT64)(Value & 0xFFFFFFF0);
        if ((Value & 0x7) == 0x04) {
          GnbLibPciRead (LocalPciAddress.AddressValue + 4, AccessWidth32, &Value, NULL); 
          BlockInfo->BarAddress |= LShiftU64 ((UINT64)Value, 32);
        }
        IDS_HDT_CONSOLE (MAIN_FLOW, "Find register block type %d : \n", BlockType);
        IDS_HDT_CONSOLE (MAIN_FLOW, "   BlockOffset 0x%X \n", BlockInfo->BlockOffset);
        IDS_HDT_CONSOLE (MAIN_FLOW, "   BarNo %d \n", BlockInfo->BarNo);
        IDS_HDT_CONSOLE (MAIN_FLOW, "   BarOffset 0x%X \n", BlockInfo->BarOffset);
        IDS_HDT_CONSOLE (MAIN_FLOW, "   BarAddress 0x%lX \n", BlockInfo->BarAddress);
        return;
      }
    }
  }

  BlockInfo->BlockOffset = CXL_INVALID_DATA;
  BlockInfo->BarNo = CXL_INVALID_DATA;
  BlockInfo->BarOffset = CXL_INVALID_DATA;
  BlockInfo->BarAddress = CXL_INVALID_DATA;
  return;
}

/**
 * Enumerate CXL cache and memory registers capability
@param[in] CmRegBaseAddress    CXL.cache and CXL.memory registers base address

@retval NULL
 **/
VOID
EnumCxlCacheMemCapability (
    IN     UINT64     CmRegBaseAddress
)
{
  UINT32                           Offset;
  UINT32                           CapIndex;
  CXLLIB_CAPABILITY_HEADER         CxlCmHeader;
  CXLLIB_COMMON_CAPABILITY_HEADER  CxlCmCapHeader;
        
  CxlCmHeader.Uint32 = CXL_REG32 (CmRegBaseAddress, CXLLIB_CAPABILITY_HEADER_OFFSET);
  if (CxlCmHeader.Bits.CxlCapabilityId == CXLLIB_CM_CAP_HDR_CAP_ID) {
    for (CapIndex = 0; CapIndex < CxlCmHeader.Bits.ArraySize; CapIndex++) {
      CxlCmCapHeader.Uint32 = CXL_REG32 (CmRegBaseAddress, (CapIndex + 1) * 4);
      
      Offset = CxlCmCapHeader.Bits.CxlCapabilityPointer;
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Find CXL CM CAP ID %d, ", CxlCmCapHeader.Bits.CxlCapabilityId);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  First DW data is 0x%08X \n", CXL_REG32 (CmRegBaseAddress, Offset));
    }
  }
  
  return;
}

/**
 * Find CXL cache and memory registers capability offset
@param[in] CmRegBaseAddress    CXL.cache and CXL.memory registers base address
@param[in] CmCapId             CXL.cache and CXL.memory capability ID to find

@retval The offset of the CXL capability relative to CmRegBaseAddress 
 **/
UINT32
FindCxlCacheMemCapability (
    IN     UINT64   CmRegBaseAddress,
    IN     UINT16   CmCapId
)
{
  UINT32                        Offset;
  UINT32                        CapIndex;
  CXLLIB_CAPABILITY_HEADER         CxlCmHeader;
  CXLLIB_COMMON_CAPABILITY_HEADER  CxlCmCapHeader;
  
  Offset = CXL_INVALID_DATA;
      
  CxlCmHeader.Uint32 = CXL_REG32 (CmRegBaseAddress, CXLLIB_CAPABILITY_HEADER_OFFSET);
  if (CxlCmHeader.Bits.CxlCapabilityId == CXLLIB_CM_CAP_HDR_CAP_ID) {
    for (CapIndex = 0; CapIndex < CxlCmHeader.Bits.ArraySize; CapIndex++) {
      CxlCmCapHeader.Uint32 = CXL_REG32 (CmRegBaseAddress, (CapIndex + 1) * 4);
      if (CxlCmCapHeader.Bits.CxlCapabilityId == CmCapId) {
        Offset = CxlCmCapHeader.Bits.CxlCapabilityPointer;
        break;
      }
    }
  }
  
  return Offset;
}

/*----------------------------------------------------------------------------------------*/
/**
 * DumpCxlCmComponentRegisters
 *
 *
 *
 *
 * @param[in]       CxlCmRegBaseAddr       CXL cache and memory component register base address
 *
 */
VOID
DumpCxlCmComponentRegisters (
    IN     UINT64                            CxlCmRegBaseAddr
) 
{
  UINT32        Index;
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "  <------------------DumpCxlCmComponentRegisters---------------> ");

  for (Index = 0; Index < 0x40; Index += 4) {
    if (Index % 16 == 0) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "\n  Offset 0x%04X : ", Index);
    }
    IDS_HDT_CONSOLE (MAIN_FLOW, "%08X ", CXL_REG32 (CxlCmRegBaseAddr, Index));
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "\n");
  
  EnumCxlCacheMemCapability (CxlCmRegBaseAddr);

  IDS_HDT_CONSOLE (MAIN_FLOW, "  <------------------------------------------------------------> \n");
}
 /*----------------------------------------------------------------------------------------*/
 /**
  * Calculate size of 32-bit BAR
  *
  *
  *
  *
  * @param[in]       EndpointBar       Pointer to engine config descriptor
  *
  */
 UINT32
 GetMmio32BarSize (
   IN  UINT32     EndpointBar
   )
 {
   UINT32        OriginalValue;
   UINT32        Value;

   //Save org value
   GnbLibPciRead (EndpointBar, AccessWidth32, &OriginalValue, NULL);
   
   Value = 0xFFFFFFFF;
   GnbLibPciWrite (EndpointBar, AccessS3SaveWidth32, &Value, NULL);
   GnbLibPciRead (EndpointBar, AccessWidth32, &Value, NULL);
   Value &= 0xFFFFFFF0;
   if (Value != 0xFFFFFFF0) {
     Value = (UINT32)(~Value) + 1;
   } else {
     Value = 0;
   }
   
   //Restore org value
   GnbLibPciWrite (EndpointBar, AccessS3SaveWidth32, &OriginalValue, NULL);
   
   return Value;
 }

 /*----------------------------------------------------------------------------------------*/
 /**
  * Calculate size of 64-bit BAR
  *
  *
  *
  *
  * @param[in]       EndpointBar       Pointer to engine config descriptor
  *
  */
UINT64
GetMmio64BarSize (
   IN  UINT32     EndpointBar
  )
{
   UINT32        NextBar;
   UINT32        OrgValue;
   UINT32        OrgValue2;
   UINT32        Value;
   UINT64        Value64;

   NextBar = EndpointBar + 4;
   
   //Save org value
   GnbLibPciRead (EndpointBar, AccessWidth32, &OrgValue, NULL);
   GnbLibPciRead (NextBar, AccessWidth32, &OrgValue2, NULL);

   Value = 0xFFFFFFFF;
   GnbLibPciWrite (EndpointBar, AccessWidth32, &Value, NULL);
   GnbLibPciWrite (NextBar, AccessWidth32, &Value, NULL);
   
   GnbLibPciRead (EndpointBar, AccessWidth32, &Value, NULL);
   if ((Value & 0xFFFFFFF0) == 0) {
     // BAR size is greater than 32-bits.. only check high bar
     GnbLibPciRead (NextBar, AccessWidth32, &Value, NULL);
     Value = (~Value) + 1;
     Value64 = LShiftU64 (((UINT64) Value), 32);
   } else {
     GnbLibPciRead (EndpointBar, AccessWidth32, &Value, NULL);
     Value64 = 0xFFFFFFFF00000000 + (Value & 0xFFFFFFF0);
     Value64 = (UINT64)(~Value64) + 1;
   }
   
   //Restore org value
   GnbLibPciWrite (NextBar, AccessWidth32, &OrgValue2, NULL);
   GnbLibPciWrite (EndpointBar, AccessWidth32, &OrgValue, NULL);
   return Value64;
}
