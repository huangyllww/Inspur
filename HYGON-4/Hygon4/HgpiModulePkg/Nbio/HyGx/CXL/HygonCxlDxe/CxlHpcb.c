/* $NoKeywords:$ */
/**
 * @file
 *
 * HygonCxlDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonCxlDxe
 * @e \$Revision: 312065 $   @e \$Date: 2022-11-08 16:17:05 -0600 (Nov, 8 Tue 2022) $
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
#include <PiDxe.h>
#include <Filecode.h>
#include <HygonCxlDxe.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/HygonNbioPcieServicesProtocol.h>
#include <GnbHsio.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Guid/GnbCxlInfoHob.h>
#include <GnbRegisters.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/UefiLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/GnbLib.h>
#include <Library/HygonPspHpobLib.h>
#include <Library/HobLib.h>
#include <Library/GnbCxlLib.h>
#include <Library/HpcbLib.h>
#include <Library/HygonPspBaseLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/FabricTopologyServices.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/PciNew.h>                 // byo230928 +
#include <IndustryStandard/Cxl.h>
#include <IndustryStandard/Cxl20.h>                  // byo230928 +
#include <Protocol/PciIo.h>
#include <Protocol/HygonCxlManagerProtocol.h>
#include <Protocol/HygonCxl11ServicesProtocol.h>
#include <Protocol/HygonCxl20ServicesProtocol.h>
#include <Addendum/Hpcb/HyGx/Inc/HpcbDfCxlConfig.h>
/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define FILECODE        NBIO_CXL_HYGONCXLDXE_CXLHPCB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
DumpCxlHpcbInfo (
  IN UINT8    *TypeDataStream
) ;

EFI_STATUS
FindCxlHpcbInfo ();

/**
 *---------------------------------------------------------------------------------------
 *  CxlHpcbGetMemPoolSize
 *
 *  Description:
 *     Read CXL memory range registers to get CXL memory size
 *  Parameters:
 *    @param[in]     *PciIo      PCI IO pointer
 *                   CxlMemSize
 *                   CxlMemType
 *---------------------------------------------------------------------------------------
 **/
void CxlHpcbGetMemPoolSize (
  IN  PCI_ADDR              PciAddr,
  IN  EFI_PCI_IO_PROTOCOL   *PciIo,
  OUT UINT64                *CxlMemSize,
  OUT UINT8                 *CxlMemType
  )
{
  UINT16                                         FlexBusDeviceCapPtr;
  EFI_STATUS                                     Status;
  UINT16                                         HdmIndex;
  UINT16                                         MemPoolCount;
  CXL_DVSEC_FLEX_BUS_DEVICE_CAPABILITY           FlexBusDevCap;
  CXL_DVSEC_FLEX_BUS_DEVICE_RANGE1_SIZE_LOW      RangeSizeLow;
  CXL_DVSEC_FLEX_BUS_DEVICE_RANGE1_SIZE_HIGH     RangeSizeHigh;

  *CxlMemSize = 0;
  *CxlMemType = CxlConventionalMemory;

  FlexBusDeviceCapPtr = FindPciCxlDvsecCapability (PciAddr.AddressValue, FLEX_BUS_DEVICE_DVSEC_ID, NULL);
  if (FlexBusDeviceCapPtr == 0) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "    Not found Flex Bus Device capability \n");
    return;
  }

  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, FlexBusDeviceCapPtr + CXL_FLEX_BUS_CAP_OFFSET, 1, (VOID *)&FlexBusDevCap.Uint16);
  
  if (FlexBusDevCap.Bits.MemCapable == 0) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "    CXL memory link failure \n");
    return;
  }

  //computes the size of the HDM range
  MemPoolCount = FlexBusDevCap.Bits.HdmCount;
  IDS_HDT_CONSOLE (MAIN_FLOW, "    HDM count %d \n", MemPoolCount);
  
  for (HdmIndex = 0; HdmIndex < MemPoolCount; HdmIndex++) {
    Status = PciIo->Pci.Read (
      PciIo,
      EfiPciIoWidthUint32,
      (FlexBusDeviceCapPtr + CXL_FLEX_BUS_RANGE1_SIZE_LOW_OFFSET + HdmIndex * 0x10),
      1,
      (VOID *)&RangeSizeLow.Uint32
      );

    IDS_HDT_CONSOLE (MAIN_FLOW, "    Range %d SizeLow = 0x%X \n", HdmIndex, RangeSizeLow.Uint32);
    
    Status = PciIo->Pci.Read (
      PciIo,
      EfiPciIoWidthUint32,
      (FlexBusDeviceCapPtr + CXL_FLEX_BUS_RANGE1_SIZE_HIGH_OFFSET + HdmIndex * 0x10),
      1,
      (VOID *)&RangeSizeHigh.Uint32
      );
    IDS_HDT_CONSOLE (MAIN_FLOW, "    Range %d SizeHigh = 0x%X \n", HdmIndex, RangeSizeHigh.Uint32);
    
    if (RangeSizeLow.Bits.MemoryInfoValid == 1) {
      *CxlMemSize += LShiftU64 ((UINT64)(RangeSizeHigh.Uint32), 32) + (UINT64)(RangeSizeLow.Uint32 & 0xF0000000);
    }
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "    Cxl memory size 0x%lx \n", *CxlMemSize);
}
 /**
  *---------------------------------------------------------------------------------------
  *  CxlUpdateHpcb
  *
  *  Description:
  *     Check and update CXL HPCB info
  *  Parameters:
  *    
  *
  *---------------------------------------------------------------------------------------
  **/
void
CxlUpdateHpcb (
  void
  )
{
  EFI_STATUS                           Status;
  UINTN                                Index, Index2;
  UINTN                                HandleCount = 0;
  EFI_HANDLE                           *HandleBuffer = NULL;
  EFI_PCI_IO_PROTOCOL                  *PciIo = NULL;
  UINT8                                ClassCode[3];
  PCI_ADDR                             PciAddr; 
  UINTN                                Segment, Bus, Device, Function;
  UINT8                                CxlDeviceCount = 0;
  UINT32                               VidDid;
  CXL_REGION_DESCRIPTOR                CxlRequestRegion[MAX_CXL_REGIONS];
  HYGON_CXL_MANAGER_PROTOCOL           *CxlMgrProtocol;
  HYGON_NBIO_CXL11_SERVICES_PROTOCOL   *Cxl11ServicesProtocol = NULL;
  HYGON_NBIO_CXL20_SERVICES_PROTOCOL   *Cxl20ServicesProtocol = NULL;
  UINT8                                SocketId;
  UINT8                                PhysicalDieId;
  UINT8                                LogicalDieId;
  UINT8                                RbIndex;
  FABRIC_CXL_AVAIL_RESOURCE            CxlAvailResourceSize;
  BOOLEAN                              NeedUpdateCxlHPCB;
  UINT64                               HpcbPtr;
  UINT32                               HpcbEntrySize;
  UINT64                               HpcbEntryDest;
  UINT8                                *NewHpcbPtr;
  TYPE_ATTRIB                          TypeAttrib;
  BOOLEAN                              Succeed;
  BOOLEAN                              FoundFlag;

  IDS_HDT_CONSOLE (MAIN_FLOW, "%a Entry\n", __FUNCTION__);

  gBS->SetMem (&CxlRequestRegion[0], sizeof (CxlRequestRegion), 0);
 
  Status = gBS->LocateProtocol (&gHygonCxlManagerProtocolGuid, NULL, (VOID **)&CxlMgrProtocol);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Found gHygonCxlManagerProtocolGuid failed!\n");
    return;
  } else {
    Status = CxlMgrProtocol->FabricCxlGetAvailableResource (CxlMgrProtocol, &CxlAvailResourceSize);
  }

  Status = gBS->LocateProtocol (&gHygonNbioCxl11ServicesProtocolGuid, NULL, (VOID **)&Cxl11ServicesProtocol);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Found gHygonNbioCxl11ServicesProtocolGuid failed!\n");
    Cxl11ServicesProtocol = NULL;
  }

  Status = gBS->LocateProtocol (&gHygonNbioCxl20ServicesProtocolGuid, NULL, (VOID **)&Cxl20ServicesProtocol);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Found gHygonNbioCxl20ServicesProtocolGuid failed!\n");
    Cxl20ServicesProtocol = NULL;
  }

  // Scan CXL devices
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiPciIoProtocolGuid, NULL, &HandleCount, &HandleBuffer);
  if(EFI_ERROR(Status)) {
    return;
  }

  for(Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
    if(EFI_ERROR(Status)) {
      continue;
    }
    
    PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
    PciAddr.AddressValue = 0;
    PciAddr.Address.Segment = (UINT32) Segment;
    PciAddr.Address.Bus = (UINT32) Bus;
    PciAddr.Address.Device = (UINT32) Device;
    PciAddr.Address.Function = (UINT32) Function;

    Status = PciIo->Pci.Read ( PciIo,
                             EfiPciIoWidthUint32,
                             0x00,
                             1,
                             &VidDid
                             );

    Status = PciIo->Pci.Read ( PciIo,
                             EfiPciIoWidthUint8,
                             0x09,
                             3,
                             &ClassCode[0]
                             );
    //IDS_HDT_CONSOLE (MAIN_FLOW, "  Casscode %02x %02X %02X \n", ClassCode[0], ClassCode[1], ClassCode[2]);

    if ((ClassCode[2] == PCI_CLASS_MEMORY_CONTROLLER) && (ClassCode[1] == PCI_CLASS_MEMORY_CXL)) {
      if (ClassCode[0] == PCI_IF_CXL_MEM_DEVICE) {
        //Add CXL 1.1 info
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Found CXL 1.1 device [%02X|%02X|%02X] VidDid 0x%08X \n", Bus, Device, Function, VidDid);
        if (Cxl11ServicesProtocol != NULL) {
          Cxl11ServicesProtocol->GetCxl11PortRBLocation (
            Cxl11ServicesProtocol, 
            (UINT8) PciAddr.Address.Bus, 
            &SocketId, 
            &PhysicalDieId, 
            &LogicalDieId, 
            &RbIndex);
        }
      } else {
        //Add CXL 2.0 info
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Found CXL 2.0 device [%02X|%02X|%02X] VidDid 0x%08X \n", Bus, Device, Function, VidDid);
        if (Cxl20ServicesProtocol != NULL) {
          Cxl20ServicesProtocol->GetCXL20PortRBLocation (
            Cxl20ServicesProtocol, 
            (UINT8) PciAddr.Address.Bus, 
            &SocketId, 
            &LogicalDieId, 
            &RbIndex);
        }
      }

      CxlRequestRegion[CxlDeviceCount].Valid = TRUE;
      CxlRequestRegion[CxlDeviceCount].SocketId = SocketId;
      CxlRequestRegion[CxlDeviceCount].LogicalDieId = LogicalDieId;
      CxlRequestRegion[CxlDeviceCount].NbioMap = 1 << RbIndex;
      CxlRequestRegion[CxlDeviceCount].Alignment = CXL_256MB_ALIGN;
      CxlHpcbGetMemPoolSize (PciAddr, PciIo, &CxlRequestRegion[CxlDeviceCount].MemSize, &CxlRequestRegion[CxlDeviceCount].MemType);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Cxl Request Socket %d LogicalDie %d Nbio %d memory size 0x%lx type %d \n", SocketId, LogicalDieId, RbIndex, 
                                    CxlRequestRegion[CxlDeviceCount].MemSize, 
                                    CxlRequestRegion[CxlDeviceCount].MemType);
      CxlDeviceCount++;
    }
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "Cxl Device count = %d \n", CxlDeviceCount);

  NeedUpdateCxlHPCB = FALSE;
  if (CxlDeviceCount == 0) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Not found CXL device, %a Exit\n", __FUNCTION__);
    //return;
  }

  for (Index = 0; Index < CxlDeviceCount; Index++) {
    FoundFlag = FALSE;
    if (CxlAvailResourceSize.MemRegionCount == 0) {
      NeedUpdateCxlHPCB = TRUE;
      IDS_HDT_CONSOLE (MAIN_FLOW, "  CxlAvailResourceSize.MemRegionCount is 0, Need update CXL HPCB \n");
      break;
    }

    for (Index2 = 0; Index2 < CxlAvailResourceSize.MemRegionCount; Index2++) {
      if (((CxlAvailResourceSize.MemRegion[Index2].SocketMap & (1 << CxlRequestRegion[Index].SocketId)) != 0) &&
           ((CxlAvailResourceSize.MemRegion[Index2].DieMap & (1 << CxlRequestRegion[Index].LogicalDieId)) != 0) &&
           ((CxlAvailResourceSize.MemRegion[Index2].NbioMap == CxlRequestRegion[Index].NbioMap))) {
        //Found avail CXL memory resource
        FoundFlag = TRUE;
        if (CxlAvailResourceSize.MemRegion[Index2].Size >= CxlRequestRegion[Index].MemSize) {
          IDS_HDT_CONSOLE (MAIN_FLOW, "  Cxl mem avail size 0x%lx is meet request \n", CxlAvailResourceSize.MemRegion[Index2].Size);
        } else {
          NeedUpdateCxlHPCB = TRUE;
          IDS_HDT_CONSOLE (MAIN_FLOW, "  Need update CXL HPCB \n");
        }
        break;    
      }
    }

    if ((FoundFlag == FALSE) && (NeedUpdateCxlHPCB == FALSE)) {
      //Not found mem resource in CxlAvailResource for this device
      NeedUpdateCxlHPCB = TRUE;
      break;
    }
  }

  if (NeedUpdateCxlHPCB) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Dump CxlRequestRegion : \n");
    DumpCxlHpcbInfo ((UINT8 *)(UINTN)CxlRequestRegion);

    IDS_HDT_CONSOLE (MAIN_FLOW, "  Before Replace Cxl HPCB \n");
    Status = FindCxlHpcbInfo ();
    if(EFI_ERROR(Status)) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Not found CXL HPCB \n");
      return;
    }

    Succeed = BIOSEntryInfo (BIOS_HPCB_INFO, INSTANCE_IGNORED, &TypeAttrib, &HpcbPtr, &HpcbEntrySize, &HpcbEntryDest);
    if (Succeed == FALSE) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "Fail to get HPCB size \n");
      return;
    }

    IDS_HDT_CONSOLE (MAIN_FLOW, "  HpcbEntrySize 0x%X, Replace Cxl HPCB \n", HpcbEntrySize);
    NewHpcbPtr = NULL;
    NewHpcbPtr = AllocateZeroPool (HpcbEntrySize);
    ASSERT (NewHpcbPtr != NULL);
    Status = HpcbReplaceType (HPCB_GROUP_DF, HPCB_DF_TYPE_CXL_CONFIG, 0, 
                                 (UINT8 *)CxlRequestRegion, 
                                 sizeof (CxlRequestRegion), 
                                 NewHpcbPtr);
    if (Status == EFI_SUCCESS) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "  HygonPspWriteBackHpcbShadowCopy \n");
      HygonPspWriteBackHpcbShadowCopy ();

      IDS_HDT_CONSOLE (MAIN_FLOW, "  After Replace Cxl HPCB \n");
      Status = FindCxlHpcbInfo ();

      IDS_HDT_CONSOLE (MAIN_FLOW, "  Reset system \n");
      gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "%a Exit\n", __FUNCTION__);
}

 /**
  *---------------------------------------------------------------------------------------
  *  DumpCxlHpcbInfo
  *
  *  Description:
  *     Dump CXL HPCB info
  *  Parameters:
  *    
  *
  *---------------------------------------------------------------------------------------
  **/
VOID
DumpCxlHpcbInfo (
  IN UINT8    *TypeDataStream
) 
{
  UINT8                                Index;
  CXL_REGION_DESCRIPTOR                *HpcbCxlRegionPtr;

  HpcbCxlRegionPtr = (CXL_REGION_DESCRIPTOR *)TypeDataStream;
  for (Index = 0; Index < MAX_CXL_REGIONS; Index++) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "HPCB CXL Region %d Valid %d, socket %d LogicalDieId %d, NbioMap 0x%X, Mem size 0x%lx, Alignment 0x%lX\n",
      Index,
      HpcbCxlRegionPtr->Valid, 
      HpcbCxlRegionPtr->SocketId, 
      HpcbCxlRegionPtr->LogicalDieId,
      HpcbCxlRegionPtr->NbioMap,
      HpcbCxlRegionPtr->MemSize,
      HpcbCxlRegionPtr->Alignment);

    HpcbCxlRegionPtr++;
  }
}

 /**
  *---------------------------------------------------------------------------------------
  *  FindCxlHpcbInfo
  *
  *  Description:
  *     Found CXL HPCB info
  *  Parameters:
  *    
  *
  *---------------------------------------------------------------------------------------
  **/
EFI_STATUS
FindCxlHpcbInfo () 
{
  EFI_STATUS                           Status;
  UINT8                                *TypeDataStream;
  UINT32                               TypeDataSize;

  TypeDataSize = 0;
  Status = HpcbGetType (HPCB_GROUP_DF, HPCB_DF_TYPE_CXL_CONFIG, 0, NULL, &TypeDataSize);
  if (TypeDataSize == 0) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Fail to get the HPCB_DF_TYPE_CXL_CONFIG Data Size\n");
    return EFI_NOT_FOUND;
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "HPCB_DF_TYPE_CXL_CONFIG TypeDataSize 0x%X\n", TypeDataSize);

  TypeDataStream = NULL;
  TypeDataStream = AllocateZeroPool (TypeDataSize);
  ASSERT (TypeDataStream != NULL);
  Status = HpcbGetType (HPCB_GROUP_DF, HPCB_DF_TYPE_CXL_CONFIG, 0, TypeDataStream, &TypeDataSize);
  if(EFI_ERROR(Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Fail to get the HPCB_DF_TYPE_CXL_CONFIG data\n");
    return Status;
  }

  DumpCxlHpcbInfo (TypeDataStream);

  return EFI_SUCCESS;
}