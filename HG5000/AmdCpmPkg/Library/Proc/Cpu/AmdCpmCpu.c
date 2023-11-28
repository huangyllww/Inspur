/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM Library for Common CPU/APU Function
 *
 * Contains interface to the AMD CPM library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  Library
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

#include <AmdCpmBase.h>
#include <OptionCpuInstall.h>
#include CPM_PPI_DEFINITION (AmdCpmTablePpi)

CPU_REVISION_ITEM *CpuRevisionTable[] = {
  OPTION_CPM_CPU_REVISION_ID_ON
  OPTION_CPM_CPU_REVISION_ID_TN
  OPTION_CPM_CPU_REVISION_ID_KV
  OPTION_CPM_CPU_REVISION_ID_KB
  OPTION_CPM_CPU_REVISION_ID_ML
  OPTION_CPM_CPU_REVISION_ID_CZ
  OPTION_CPM_CPU_REVISION_ID_NL
  OPTION_CPM_CPU_REVISION_ID_AM
  OPTION_CPM_CPU_REVISION_ID_ST
  OPTION_CPM_CPU_REVISION_ID_BR
  OPTION_CPM_CPU_REVISION_ID_ZP
  OPTION_CPM_CPU_REVISION_ID_HYGON_ZP //PS-473
  NULL
};

PCIE_BRIDGE_NAME *PcieBridgeName[] = {
  OPTION_CPM_PCIE_BRIDGE_NAME_ON
  OPTION_CPM_PCIE_BRIDGE_NAME_TN
  OPTION_CPM_PCIE_BRIDGE_NAME_KV
  OPTION_CPM_PCIE_BRIDGE_NAME_KB
  OPTION_CPM_PCIE_BRIDGE_NAME_ML
  OPTION_CPM_PCIE_BRIDGE_NAME_CZ
  OPTION_CPM_PCIE_BRIDGE_NAME_NL
  OPTION_CPM_PCIE_BRIDGE_NAME_AM
  OPTION_CPM_PCIE_BRIDGE_NAME_ST
  OPTION_CPM_PCIE_BRIDGE_NAME_BR
  OPTION_CPM_PCIE_BRIDGE_NAME_ZP
  OPTION_CPM_PCIE_BRIDGE_NAME_HYGON_ZP //PS-473
  NULL
};

UINT8 AmdCpmSsdtSupport = AMD_CPM_SSDT_SUPPORT;


/*---------------------------------------------------------------------------------------*/
/**
 * Get SBI (Sideband Interface) Address in PCI MMIO Base
 *
 *
 * @param[in] This    The pointer of AMD CPM Table Ppi or Protocol
 *
 * @retval    SBI Address
 *
 */
UINT32
CpmGetSbTsiAddr (
  IN       VOID                        *This
  )
{
  AMD_CPM_MAIN_TABLE  *MainTablePtr;
  MainTablePtr = ((AMD_CPM_TABLE_PPI*)This)->MainTablePtr;
  return (MainTablePtr->PcieMemIoBaseAddr + 0x000C31E8);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Check whether thermal function is enabled in current CPU or APU
 *
 *
 * @param[in] This    The pointer of AMD CPM Table Ppi or Protocol
 *
 * @retval    TRUE    Both of ThermtpEn and HtcCapable are enabled
 *            FALSE   Either of ThermtpEn or HtcCapable is not enabled
 *
 */
BOOLEAN
CpmIsThermalSupport (
  IN       VOID                        *This
  )
{
  AMD_CPM_TABLE_PPI   *CpmTablePpiPtr = This;
  UINT32              ThermtpEn;
  UINT32              HtcCapable;

  ThermtpEn   = CpmTablePpiPtr->CommonFunction.PciRead32 (CpmTablePpiPtr, 0, 0x18, 3, 0xE4) & BIT5;      // D18F3xE4 Thermtrip Status [5]: ThermtpEn
  HtcCapable  = CpmTablePpiPtr->CommonFunction.PciRead32 (CpmTablePpiPtr, 0, 0x18, 3, 0xE8) & BIT10;     // D18F3xE8 Northbridge Capabilities [10]: HtcCapable

  return (BOOLEAN) ((ThermtpEn != 0) && (HtcCapable != 0));
}

/*---------------------------------------------------------------------------------------*/
/**
 * Check whether thermal function is enabled in current CPU or APU
 *
 *
 * @param[in] This    The pointer of AMD CPM Table Ppi or Protocol
 *
 * @retval    The CPU Revision Item for current platform
 *
 */
CPU_REVISION_ITEM*
CpmGetCpuRevisionItem (
  IN       VOID           *This
  )
{
  AMD_CPM_COMMON_FUNCTION *CommonFunctionPtr;
  CPU_REVISION_ITEM       **CpuRevisionPtr;
  CPU_REVISION_ITEM       *CpuRevisionItemPtr;
  CPU_REVISION_ITEM       *Item;
  UINT32                  Value;
  UINT32                  ApuSupportMask;

  ApuSupportMask = PcdGet32 (ApuSupportMask);
  CommonFunctionPtr = &((AMD_CPM_TABLE_PPI*)This)->CommonFunction;
  CpuRevisionPtr = &CpuRevisionTable[0];
  CommonFunctionPtr->CpuidRead(This, &Value, NULL);            // AMD_CPUID_FMF
  Item = 0;
  while (*CpuRevisionPtr && !Item) {
    CpuRevisionItemPtr = *CpuRevisionPtr;
    while (CpuRevisionItemPtr->Mask) {
      if (!(ApuSupportMask & (1 << CpuRevisionItemPtr->CpuRevision))) {
        break;
      }
      if ((CpuRevisionItemPtr->Mask & Value) == CpuRevisionItemPtr->Value) {
        Item = CpuRevisionItemPtr;
        break;
      }
      CpuRevisionItemPtr ++;
    }
    CpuRevisionPtr ++;
  }
  return Item;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Get Current PCIe Bridge Name Table
 *
 *
 * @param[in] This    The pointer of AMD CPM Table Ppi or Protocol
 *
 * @retval    The PCIe Bridge Name Table
 *
 */
PCIE_BRIDGE_NAME*
CpmGetPcieBridgeNameTable (
  IN       VOID           *This
  )
{
  PCIE_BRIDGE_NAME **PcieNameTablePtr;
  PCIE_BRIDGE_NAME *PcieNameItemPtr;
  CPU_REVISION_ITEM *ItemPtr;
  UINT8 PcieBridgeType;
  PcieBridgeType = 0;
  ItemPtr = CpmGetCpuRevisionItem (This);
  if (ItemPtr) {
    PcieBridgeType = ItemPtr->PcieRevision;
  }

  PcieNameTablePtr = &PcieBridgeName[0];
  PcieNameItemPtr = 0;
  while (*PcieNameTablePtr) {
    if ((*PcieNameTablePtr)->PcieRevision == PcieBridgeType) {
      PcieNameItemPtr = *PcieNameTablePtr;
      break;
    }
    PcieNameTablePtr ++;
  }
  return PcieNameItemPtr;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Get PCIe Bridge Name in ASL code
 *
 *
 * @param[in] This      The pointer of AMD CPM Table Ppi or Protocol
 * @param[in] Device    The PCI Device Number of PCIe Bridge
 * @param[in] Function  The PCI Function Number of PCIe Bridge
 *
 * @retval    The PCIe Bridge Name in ASL code
 *
 */
UINT32
CpmGetPcieAslName (
  IN       VOID                       *This,
  IN       UINT8                      Device,
  IN       UINT8                      Function
  )
{
  PCIE_BRIDGE_NAME  *BridgeNamePtr;
  UINT32  Name;

  Name = 0;
  BridgeNamePtr = CpmGetPcieBridgeNameTable (This);
  while (BridgeNamePtr->Device != 0) {
    if (BridgeNamePtr->Device == Device && BridgeNamePtr->Function == Function) {
      switch (BridgeNamePtr->NameId) {
      case 0x10:
        Name = PcdGet32 (PcieGfx0AslName);
        break;
      case 0x11:
        Name = PcdGet32 (PcieGfx1AslName);
        break;
      case 0x12:
        Name = PcdGet32 (PcieGfx2AslName);
        break;
      case 0x13:
        Name = PcdGet32 (PcieGfx3AslName);
        break;
      case 0x14:
        Name = PcdGet32 (PcieGfx4AslName);
        break;
      case 0x20:
        Name = PcdGet32 (PcieGpp0AslName);
        break;
      case 0x21:
        Name = PcdGet32 (PcieGpp1AslName);
        break;
      case 0x22:
        Name = PcdGet32 (PcieGpp2AslName);
        break;
      case 0x23:
        Name = PcdGet32 (PcieGpp3AslName);
        break;
      case 0x24:
        Name = PcdGet32 (PcieGpp4AslName);
        break;
      }
      if (!Name) {
        Name = BridgeNamePtr->Name;
      }
      break;
    }
    BridgeNamePtr ++;
  }
  return Name;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Get CPU Revision Id
 *
 *
 * @param[in] This      The pointer of AMD CPM Table Ppi or Protocol
 *
 * @retval    The CPU Revision Id
 *
 */
UINT8
CpmGetCpuRevisionId (
  IN       VOID                       *This
  )
{
  CPU_REVISION_ITEM *ItemPtr;
  UINT8             Revision;

  Revision = 0xFF;
  ItemPtr = CpmGetCpuRevisionItem (This);
  if (ItemPtr) {
    Revision = ItemPtr->CpuRevision;
  }

  return Revision;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Is the device for UMI link
 *
 *
 * @param[in] This      The pointer of AMD CPM Table Ppi or Protocol
 * @param[in] Device    The PCI Device Number of PCIe Bridge
 * @param[in] Function  The PCI Function Number of PCIe Bridge
 *
 * @retval    TRUE    Is the device for UMI link
 *            FALSE   Is not the device for UMI link
 *
 */
BOOLEAN
CpmIsUmi (
  IN       VOID                       *This,
  IN       UINT8                      Device,
  IN       UINT8                      Function
  )
{
  BOOLEAN Value;
  UINT8   CpuRevision;
  Value = FALSE;
  CpuRevision = CpmGetCpuRevisionId (This);
  switch (CpuRevision) {
  case CPM_CPU_REVISION_ID_CZ:
  case CPM_CPU_REVISION_ID_ST:
  case CPM_CPU_REVISION_ID_BR:
    if (Device == 2 && Function == 1) {
      Value = TRUE;
    }
    if (Device == 4 && Function == 1) {
      Value = TRUE;
    }
    break;
  case CPM_CPU_REVISION_ID_ZP:
  case CPM_CPU_REVISION_ID_HYGON_ZP://PS-473
    break;
  default:
    if (Device == 4 && Function == 1) {
      Value = TRUE;
    }
    if (Device == 8 && Function == 0) {
      Value = TRUE;
    }
  }
  return Value;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Register CPM Common CPU Function
 *
 *
 * @param[in] This      The pointer of AMD CPM Table Ppi or Protocol
 *
 */
VOID
EFIAPI
CpmRegisterCpu (
  IN       VOID                        *This
  )
{
  AMD_CPM_COMMON_FUNCTION *CommonFunctionPtr;
  AMD_CPM_MAIN_TABLE      *MainTablePtr;
  UINT64                  Value;
  MainTablePtr = ((AMD_CPM_TABLE_PPI*)This)->MainTablePtr;
  CommonFunctionPtr = &((AMD_CPM_TABLE_PPI*)This)->CommonFunction;
  if (!MainTablePtr->PcieMemIoBaseAddr) {
    Value = CommonFunctionPtr->MsrRead (0xC0010058);
    MainTablePtr->PcieMemIoBaseAddr = (UINT32) (Value & 0xFFF00000);
  }
  CommonFunctionPtr->GetCpuRevisionId = CpmGetCpuRevisionId;
  CommonFunctionPtr->IsUmi            = CpmIsUmi;
  CommonFunctionPtr->GetSbTsiAddr     = CpmGetSbTsiAddr;
  CommonFunctionPtr->IsThermalSupport = CpmIsThermalSupport;
  CommonFunctionPtr->GetPcieAslName   = CpmGetPcieAslName;
  return;
}
