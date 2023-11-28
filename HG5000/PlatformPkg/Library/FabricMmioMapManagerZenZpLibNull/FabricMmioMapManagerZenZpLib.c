/* $NoKeywords:$ */
/**
 * @file
 *
 * Base Fabric MMIO map manager Lib implementation for ZEN ZP
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright 2008 - 2016 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 ******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Porting.h"
#include "AMD.h"
#include "Filecode.h"
#include <FabricRegistersZP.h>
#include <Library/BaseLib.h>
#include "Library/AmdBaseLib.h"
#include "Library/AmdS3SaveLib.h"
#include "Library/AmdHeapLib.h"
#include "Library/FabricMmioMapManagerLib.h"
#include "Library/FabricRegisterAccLib.h"
#include <Library/BaseFabricTopologyLib.h>
#include <Library/PciLib.h>
#include "FabricMmioMapManagerZenZpLib.h"

#define FILECODE LIBRARY_FABRICMMIOMAPMANAGERZENZPLIB_FABRICMMIOMAPMANAGERZENZPLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

EFI_STATUS
FabricZenZpInitMmioForAllDie (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager
  );

VOID
FabricZenZpSetMmioReg (
  IN       UINT8  TotalSocket,
  IN       UINT8  DiePerSocket,
  IN       UINT8  MmioPairIndex,
  IN       UINT8  DstFabricIDSysOffset,
  IN       UINT64 BaseAddress,
  IN       UINT64 Length
  );

UINT64
FabricZenZpGetBaseAddress (
  IN       FABRIC_MMIO_REGION *MmioRegion,
  IN       UINT64              Length
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * BSC entry point for for adding MMIO map
 *
 * program MMIO base/limit registers
 *
 * @param[in, out]    BaseAddress        Starting address of the requested MMIO range.
 * @param[in, out]    Length             Length of the requested MMIO range.
 * @param[in]         Target             PCI bus number/Die number of the requestor.
 * @param[in, out]    Attributes         Attributes of the requested MMIO range indicating whether
 *                                       it is readable/writable/non-posted
 *
 * @retval            EFI_STATUS         EFI_OUT_OF_RESOURCES - The requested range could not be added because there are not
 *                                                              enough mapping resources.
 *                                       EFI_ABORTED          - One or more input parameters are invalid. For example, the
 *                                                              PciBusNumber does not correspond to any device in the system.
 *                                       EFI_SUCCESS          - Adding MMIO map succeeds
 */
EFI_STATUS
FabricAddMmioMap (
  IN OUT   UINT64 *BaseAddress,
  IN OUT   UINT64 *Length,
  IN       FABRIC_MMIO_TARGET     Target,
  IN OUT   FABRIC_MMIO_ATTRIBUTE *Attributes
  )
{
  UINT8                     Socket;
  UINT8                     Die;
  UINT8                     TempSocket;
  UINT8                     TempDie;
  UINT8                     DstFabricID;
  UINTN                     Register;
  UINT64                    SizeA;
  UINT64                    SizeB;
  CFG_ADDRESS_MAP_REGISTER  BusMap;
  ALLOCATE_HEAP_PARAMS      AllocateHeapParams;
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_MMIO_MANAGER      *FabricMmioManager;
  FABRIC_MMIO_REGION       *MmioRegion;
  FABRIC_MMIO_REGION       *Die02ndMmioRegion;
  EFI_STATUS                ReturnStatus;

  ReturnStatus = EFI_SUCCESS;

  IDS_HDT_CONSOLE (CPU_TRACE, " FabricAddMmioMap\n");
//Handles current AGESA callers of this function    
//a. Promontory CBS Pei  
//b. Promontory GPIOPei
//c. Promontory PEI
//d. PSP  
//Provides 12MB space on Die0 between 0xFE00_000 to 0xFEC0_0000 to be allocated.
//Current callers consume 3MB of space
//Need to add target checks since this is only expecting Die0 adds.
  
  
  // Find or create MMIO manager in heap
  LocateHeapParams.BufferHandle = AMD_MMIO_MANAGER;

  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricMmioManager = (FABRIC_MMIO_MANAGER *) LocateHeapParams.BufferPtr;
  } else {
    AllocateHeapParams.RequestedBufferSize = sizeof (FABRIC_MMIO_MANAGER);
    AllocateHeapParams.BufferHandle        = AMD_MMIO_MANAGER;
    AllocateHeapParams.Persist             = HEAP_SYSTEM_MEM;
    if (HeapAllocateBuffer (&AllocateHeapParams, NULL) != AGESA_SUCCESS) {
      return EFI_ABORTED;
    }
    FabricMmioManager = (FABRIC_MMIO_MANAGER *) AllocateHeapParams.BufferPtr;
    // initialize FabricMmioManager and pre-allocate MMIO for all DIEs
    FabricZenZpInitMmioForAllDie (FabricMmioManager);
  }

  // Check input parameters
  if (*Length == 0) {
    return EFI_SUCCESS;
  }
  if (*Length < ZEN_MMIO_MIN_SIZE) {
    *Length = ZEN_MMIO_MIN_SIZE;
  }

  // Find out DstFabricID
  DstFabricID = 0;
  if (Target.TgtType == MMIO_PCI_BUS) {
    for (Register = CFGADDRESSMAP0_REG; Register <= CFGADDRESSMAP7_REG; Register += 4) {
      BusMap.Value = FabricRegisterAccRead (0, 0, CFGADDRESSMAP_FUNC, Register, FABRIC_REG_ACC_BC);
      if ((BusMap.Field.RE == 1) && (BusMap.Field.WE == 1) && (BusMap.Field.BusNumLimit >= Target.PciBusNum) && (BusMap.Field.BusNumBase <= Target.PciBusNum)) {
        DstFabricID = (UINT8) BusMap.Field.DstFabricID;
        break;
      }
    }

    if (Register > CFGADDRESSMAP7_REG) {
      return EFI_ABORTED;
    }
  } else {
    DstFabricID = (UINT8) FabricTopologyGetDieSystemOffset (Target.SocketNum, Target.DieNum) | ZP_IOMS0_INSTANCE_ID;
  }

  Socket = (DstFabricID >> 7) & 1;
  Die    = (DstFabricID >> 5) & 3;

  if (Attributes->MmioType == MMIO_BELOW_4G) {
    //
    // Below 4G
    //
    MmioRegion = &FabricMmioManager->MmioRegionBelow4G[Socket][Die];
    // return EFI_ABORTED if some one wants to allocate MMIO on a DIE but it's disabled by PcdAmdAllocateMmioBelow4GOnSocketN
    if (FabricMmioManager->AllocateMmioOnThisDie[Socket][Die] == FALSE) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  ERROR: AGESA try to set MMIO for Socket %X Die %X, but seems MMIO allocation has been disabled for this DIE by PcdAmdAllocateMmioBelow4GOnSocketN\n", Socket, Die);
      return EFI_ABORTED;
    }
    if ((MmioRegion->Size - MmioRegion->UsedSizeFromBottom - MmioRegion->UsedSizeFromTop) >= (*Length)) {
      // Have enough space
      *BaseAddress = FabricZenZpGetBaseAddress (MmioRegion, *Length);
    } else {
      // Don't have enough space, update Length and return EFI_OUT_OF_RESOURCES

      // For DIE0, check if there's 2nd MMIO
      if (FabricMmioManager->Die0Has2ndMmioBelow4G && (Socket == 0) && (Die == 0)) {
        TempSocket = (FabricMmioManager->Die02ndMmioPairBelow4G >> 4) & 0xF;
        TempDie = FabricMmioManager->Die02ndMmioPairBelow4G & 0xF;
        Die02ndMmioRegion = &FabricMmioManager->MmioRegionBelow4G[TempSocket][TempDie];
        if ((Die02ndMmioRegion->Size - Die02ndMmioRegion->UsedSizeFromBottom - Die02ndMmioRegion->UsedSizeFromTop) >= (*Length)) {
          *BaseAddress = FabricZenZpGetBaseAddress (Die02ndMmioRegion, *Length);
        } else {
          SizeA = FabricMmioManager->MmioRegionBelow4G[0][0].Size - FabricMmioManager->MmioRegionBelow4G[0][0].UsedSizeFromBottom - FabricMmioManager->MmioRegionBelow4G[0][0].UsedSizeFromTop;
          SizeB = Die02ndMmioRegion->Size - Die02ndMmioRegion->UsedSizeFromBottom - Die02ndMmioRegion->UsedSizeFromTop;
          *Length = (SizeA > SizeB) ? SizeA : SizeB;
          *BaseAddress = 0;
          ReturnStatus = EFI_OUT_OF_RESOURCES;
        }
      } else {
        *Length = MmioRegion->Size - MmioRegion->UsedSizeFromBottom - MmioRegion->UsedSizeFromTop;
        *BaseAddress = 0;
        ReturnStatus = EFI_OUT_OF_RESOURCES;
      }
    }
  } else if (Attributes->MmioType == MMIO_ABOVE_4G) {
    //
    // Above 4G
    //
    MmioRegion = &FabricMmioManager->MmioRegionAbove4G[Socket][Die];
    if ((MmioRegion->Size - MmioRegion->UsedSizeFromBottom - MmioRegion->UsedSizeFromTop) >= (*Length)) {
      // Have enough space
      *BaseAddress = FabricZenZpGetBaseAddress (MmioRegion, *Length);
    } else {
      // Don't have enough space, update Length and return EFI_OUT_OF_RESOURCES

      // For DIE0, check if there's 2nd MMIO
      if (FabricMmioManager->Die0Has2ndMmioAbove4G && (Socket == 0) && (Die == 0)) {
        TempSocket = (FabricMmioManager->Die02ndMmioPairAbove4G >> 4) & 0xF;
        TempDie = FabricMmioManager->Die02ndMmioPairAbove4G & 0xF;
        Die02ndMmioRegion = &FabricMmioManager->MmioRegionAbove4G[TempSocket][TempDie];
        if ((Die02ndMmioRegion->Size - Die02ndMmioRegion->UsedSizeFromBottom - Die02ndMmioRegion->UsedSizeFromTop) >= (*Length)) {
          *BaseAddress = FabricZenZpGetBaseAddress (Die02ndMmioRegion, *Length);
        } else {
          SizeA = FabricMmioManager->MmioRegionAbove4G[0][0].Size - FabricMmioManager->MmioRegionAbove4G[0][0].UsedSizeFromBottom - FabricMmioManager->MmioRegionAbove4G[0][0].UsedSizeFromTop;
          SizeB = Die02ndMmioRegion->Size - Die02ndMmioRegion->UsedSizeFromBottom - Die02ndMmioRegion->UsedSizeFromTop;
          *Length = (SizeA > SizeB) ? SizeA : SizeB;
          *BaseAddress = 0;
          ReturnStatus = EFI_OUT_OF_RESOURCES;
        }
      } else {
        *Length = MmioRegion->Size - MmioRegion->UsedSizeFromBottom + MmioRegion->UsedSizeFromTop;
        *BaseAddress = 0;
        ReturnStatus = EFI_OUT_OF_RESOURCES;
      }
    }
  } else if (Attributes->MmioType == MMIO_AT_RESERVED_REGION) {
    // At fixed address, do nothing
    // For some devices which expect MMIO should be located at 0xFExx_0000 (PcdAmdBottomMmioReservedForDie0)
  }

  if (ReturnStatus == EFI_SUCCESS) {
    Attributes->NonPosted = 0;
    Attributes->ReadEnable = 1;
    Attributes->WriteEnable = 1;
    Attributes->CpuDis = 0;
    if (Attributes->MmioType != MMIO_AT_RESERVED_REGION) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Successfully Allocate MMIO at 0x%X0000 ~ 0x%XFFFF\n", (UINT32) (*BaseAddress >> 16), (UINT32) (((*BaseAddress + *Length) >> 16) - 1));
    }
  } else if (ReturnStatus == EFI_OUT_OF_RESOURCES) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  No enough space, the biggest MMIO size is 0x%X0000\n", (UINT32) (*Length >> 16));
  }

  return ReturnStatus;
}

EFI_STATUS
FabricZenZpInitMmioForAllDie (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager
  )
{
  UINT8  i;
  UINT8  j;
  UINT8  SocketNumber;
  UINT8  LastSocket;
  UINT8  LastDie;
  UINT8  DieNumberPerSocket;
  UINT32 DieMmioMask;
  UINT8  DieMmioNumber;
  UINT8  DstFabricIDSysOffset;
  UINT32 LocalPci;
  UINT64 MmioBaseAddr;
  UINT64 MmioSize;
  UINT64 MmioSize16MAlign;
  UINT64 MmioSizeRemain;
  UINT64 TOM2;
  UINT64 PciCfgSpace;

  SocketNumber = (UINT8) FabricTopologyGetNumberOfProcessorsPresent ();
  DieNumberPerSocket = (UINT8) FabricTopologyGetNumberOfDiesOnSocket (0);

  DieMmioNumber = 1;
  DieMmioMask   = (PcdGet8 (PcdAmdBitMapForMmioBelow4GOnSocket1) << 4) | (PcdGet8 (PcdAmdBitMapForMmioBelow4GOnSocket0) & 0xF);

  LastSocket = 0;
  LastDie = 0;

  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < DieNumberPerSocket; j++) {
      if ((i == 0) && (j == 0)) {
        FabricMmioManager->AllocateMmioOnThisDie[i][j] = TRUE;
        continue;
      }
      if (((DieMmioMask >> (i * DieNumberPerSocket + j)) & 1) == 1) {
        FabricMmioManager->AllocateMmioOnThisDie[i][j] = TRUE;
        LastSocket = i;
        LastDie = j;
        DieMmioNumber++;
      }
    }
  }

  // Below 4G:
  //    +---------------------------------+
  //    |  BIOS FW                        |
  //    |  Local APIC and etc.            |
  //    +---------------------------------+ +--+ 0xFEE0_0000
  //    |           Reserved for DIE0     | +--+ 0xFE00_0000 (PcdAmdBottomMmioReservedForDie0) - 0xFEE0_0000
  //    |  MMIO Above PCIe --- for DIE0   |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+ +--+ PcdPciExpressBaseAddress + Size (defined by MSR_C001_0058[5:2][BusRange])
  //    |  PCIe Configuration Space       |
  //    |                                 |
  //    +---------------------------------+ +--+ PcdPciExpressBaseAddress
  //    |                                 |
  //    | MMIO Above TOM - for other DIEs |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+ +--+ TOM
  //    |                                 |
  //    |  DRAM                           |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+ +--+ 0x0000_0000

  // MMIO for DIE 0
  PciCfgSpace = (AsmReadMsr64 (0xC0010058) >> 2) & 0xF;          // Get bus range from MSR_C001_0058[5:2][BusRange]
  PciCfgSpace = MultU64x64 (((UINT64) 1 << PciCfgSpace), (1024 * 1024));  // The size of configuration space is 1MB times the number of buses
//  MmioBaseAddr = PcdGet64 (PcdPciExpressBaseAddress) + PciCfgSpace;
//  ASSERT (MmioBaseAddr < 0xFEE00000);
//  MmioSize = PcdGet32 (PcdAmdBottomMmioReservedForDie0) - MmioBaseAddr;
  MmioBaseAddr = 0xFE000000;
  MmioSize = 0xC00000;
  
  FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, 0, 0, MmioBaseAddr, (0xFEE00000 - (UINT32) MmioBaseAddr));
  FabricMmioManager->MmioRegionBelow4G[0][0].Base = MmioBaseAddr;
  FabricMmioManager->MmioRegionBelow4G[0][0].Size = MmioSize;
  FabricMmioManager->MmioRegionBelow4G[0][0].UsedSizeFromBottom = 0;
  FabricMmioManager->MmioRegionBelow4G[0][0].UsedSizeFromTop = 0;

  // MMIO for other DIEs
  MmioBaseAddr = AsmReadMsr64 (0xC001001A);

  // if there're 3 or less DIEs need MMIO, then give DIE0 a 2nd MMIO region
  if (DieMmioNumber > 3) {
    MmioSize16MAlign = DivU64x32 ((PcdGet64 (PcdPciExpressBaseAddress) - MmioBaseAddr), (DieMmioNumber - 1)) & SIZE_16M_ALIGN;
    MmioSizeRemain = (PcdGet64 (PcdPciExpressBaseAddress) - MmioBaseAddr) - MultU64x64 ((DieMmioNumber - 2), MmioSize16MAlign);
  } else {
    MmioSize16MAlign = DivU64x32 ((PcdGet64 (PcdPciExpressBaseAddress) - MmioBaseAddr), DieMmioNumber) & SIZE_16M_ALIGN;
    MmioSizeRemain = (PcdGet64 (PcdPciExpressBaseAddress) - MmioBaseAddr) - MultU64x64 ((DieMmioNumber - 1), MmioSize16MAlign);
    // find out a MMIO pair which is not used
    for (i = 0; i < ZEN_MAX_SOCKET; i++) {
      for (j = 0; j < ZEN_MAX_DIE_PER_SOCKET; j++) {
        if (FabricMmioManager->AllocateMmioOnThisDie[i][j] == FALSE) {
          FabricMmioManager->Die02ndMmioPairBelow4G = (UINT8) ((i << 4) | j);
          FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * DieNumberPerSocket + j) * 2), 0, (PcdGet64 (PcdPciExpressBaseAddress) - MmioSizeRemain), MmioSizeRemain);
          FabricMmioManager->MmioRegionBelow4G[i][j].Base = PcdGet64 (PcdPciExpressBaseAddress) - MmioSizeRemain;
          FabricMmioManager->MmioRegionBelow4G[i][j].Size = MmioSizeRemain;
          FabricMmioManager->MmioRegionBelow4G[i][j].UsedSizeFromBottom = 0;
          FabricMmioManager->MmioRegionBelow4G[i][j].UsedSizeFromTop = 0;
          FabricMmioManager->Die0Has2ndMmioBelow4G = TRUE;
          MmioSizeRemain = MmioSize16MAlign;  // MmioSizeRemain has been assgined to DIE0, so MmioSizeRemain = MmioSize16MAlign
          break;
        }
      }
      if (FabricMmioManager->Die0Has2ndMmioBelow4G) {
        break;
      }
    }
  }

  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < DieNumberPerSocket; j++) {
      if ((i == 0) && (j == 0)) {
        continue;
      }
      if (FabricMmioManager->AllocateMmioOnThisDie[i][j] == TRUE) {
        if ((i == LastSocket) && (j == LastDie)) {
          MmioSize16MAlign = MmioSizeRemain;
        }
        DstFabricIDSysOffset = (UINT8) FabricTopologyGetDieSystemOffset (i, j);
        FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * DieNumberPerSocket + j) * 2), DstFabricIDSysOffset, MmioBaseAddr, MmioSize16MAlign);
        FabricMmioManager->MmioRegionBelow4G[i][j].Base = MmioBaseAddr;
        FabricMmioManager->MmioRegionBelow4G[i][j].Size = MmioSize16MAlign;
        FabricMmioManager->MmioRegionBelow4G[i][j].UsedSizeFromBottom = 0;
        FabricMmioManager->MmioRegionBelow4G[i][j].UsedSizeFromTop = 0;
        MmioBaseAddr += MmioSize16MAlign;
      }
    }
  }

  // Above 4G
  //    +---------------------------------+ +--+ 0xFFFD_0000_0000 (2^48 - 12G) or 0x7FD_0000_0000 (8T - 12G)
  //    |                                 |
  //    |   MMIO FOR DIE 7                |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+
  //    |                                 |
  //    |   ......                        |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+
  //    |                                 |
  //    |   MMIO FOR DIE 2                |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+
  //    |                                 |
  //    |   MMIO FOR DIE 1                |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+
  //    |                                 |
  //    |   MMIO FOR DIE 0                |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+ +--+ 0x100_0000_0000
  //    |                                 |
  //    |  Can't use this region          |
  //    |    0xFD_0000_0000 ~             |
  //    |    0x100_0000_0000              |
  //    |                                 |
  //    +---------------------------------+ +--+ 0xFD_0000_0000

  TOM2 = AsmReadMsr64 (0xC001001D);
  MmioBaseAddr = (TOM2 > 0x10000000000) ? TOM2 : 0x10000000000;
  LocalPci = 0;
  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < DieNumberPerSocket; j++) {
      LocalPci |= FabricRegisterAccRead (i, j, 0x0, 0x3FC, FABRIC_REG_ACC_BC);
    }
  }

  if ((LocalPci & BIT0) == BIT0) {
    MmioSize = 0x7FD00000000 - MmioBaseAddr;
  } else {
    MmioSize = 0xFFFD00000000 - MmioBaseAddr;
  }
  MmioSize16MAlign = DivU64x32 (MmioSize, (SocketNumber * DieNumberPerSocket)) & SIZE_16M_ALIGN;
  MmioSizeRemain = MmioSize - MultU64x64 ((SocketNumber * DieNumberPerSocket - 1), MmioSize16MAlign);

  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < DieNumberPerSocket; j++) {
      if ((i == (SocketNumber - 1)) && (j == (DieNumberPerSocket - 1))) {
        MmioSize16MAlign = MmioSizeRemain;
      }
      DstFabricIDSysOffset = (UINT8) FabricTopologyGetDieSystemOffset (i, j);
      if ((i == (SocketNumber - 1)) && (j == (DieNumberPerSocket - 1))) {
        // workaround to set last DIE's MMIO limit to 0xFFFF_FFFF_FFFF
        FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * DieNumberPerSocket + j) * 2 + 1), DstFabricIDSysOffset, MmioBaseAddr, (0x1000000000000 - MmioBaseAddr));
      } else {
        FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * DieNumberPerSocket + j) * 2 + 1), DstFabricIDSysOffset, MmioBaseAddr, MmioSize16MAlign);
      }
      FabricMmioManager->MmioRegionAbove4G[i][j].Base = MmioBaseAddr;
      FabricMmioManager->MmioRegionAbove4G[i][j].Size = MmioSize16MAlign;
      FabricMmioManager->MmioRegionAbove4G[i][j].UsedSizeFromBottom = 0;
      FabricMmioManager->MmioRegionAbove4G[i][j].UsedSizeFromTop = 0;
      MmioBaseAddr += MmioSize16MAlign;
    }
  }


  //    +---------------------------------+ +--+ 0xFD_0000_0000
  //    |                                 |
  //    |   MMIO FOR DIE 0                |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+ +--+ TOM2
  if ((TOM2 < 0xFD00000000) && ((SocketNumber * DieNumberPerSocket) < ZEN_MAX_SOCKET * ZEN_MAX_DIE_PER_SOCKET)) {
    if (DieNumberPerSocket == ZEN_MAX_DIE_PER_SOCKET) {
      i = 1;
      j = 0;
    } else {
      i = 0;
      j = DieNumberPerSocket;
    }
    FabricMmioManager->Die02ndMmioPairAbove4G = (UINT8) ((i << 4) | j);
    FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, (DieNumberPerSocket * 2 + 1), 0, TOM2, (0xFD00000000 - TOM2));
    FabricMmioManager->MmioRegionAbove4G[i][j].Base = TOM2;
    FabricMmioManager->MmioRegionAbove4G[i][j].Size = 0xFD00000000 - TOM2;
    FabricMmioManager->MmioRegionAbove4G[i][j].UsedSizeFromBottom = 0;
    FabricMmioManager->MmioRegionAbove4G[i][j].UsedSizeFromTop = 0;
    FabricMmioManager->Die0Has2ndMmioAbove4G = TRUE;
  }
  return EFI_SUCCESS;
}

VOID
FabricZenZpSetMmioReg (
  IN       UINT8  TotalSocket,
  IN       UINT8  DiePerSocket,
  IN       UINT8  MmioPairIndex,
  IN       UINT8  DstFabricIDSysOffset,
  IN       UINT64 BaseAddress,
  IN       UINT64 Length
  )
{
  //do not set any real regs
  /*  
  UINTN  i;
  UINTN  j;
  MMIO_ADDR_CTRL MmioAddrCtrlReg;

  MmioAddrCtrlReg.Value = FabricRegisterAccRead (0, 0, 0x0, (MMIO_ADDRESS_CONTROL_REG_0 + MmioPairIndex * 0x10), FABRIC_REG_ACC_BC);
  ASSERT ((MmioAddrCtrlReg.Field.RE != 1) && (MmioAddrCtrlReg.Field.WE != 1));
  for (i = 0; i < TotalSocket; i++) {
    for (j = 0; j < DiePerSocket; j++) {
      FabricRegisterAccWrite (i, j, 0x0, (MMIO_BASE_ADDRESS_REG_0  + MmioPairIndex * 0x10), FABRIC_REG_ACC_BC, (UINT32) (BaseAddress >> 16), TRUE);
      FabricRegisterAccWrite (i, j, 0x0, (MMIO_LIMIT_ADDRESS_REG_0 + MmioPairIndex * 0x10), FABRIC_REG_ACC_BC, (UINT32) ((BaseAddress + Length - 1) >> 16), TRUE);
      MmioAddrCtrlReg.Field.RE = 1;
      MmioAddrCtrlReg.Field.WE = 1;
      MmioAddrCtrlReg.Field.DstFabricID = (UINT8) (MmioAddrCtrlReg.Field.DstFabricID | DstFabricIDSysOffset);
      FabricRegisterAccWrite (i, j, 0x0, (MMIO_ADDRESS_CONTROL_REG_0 + MmioPairIndex * 0x10), FABRIC_REG_ACC_BC, MmioAddrCtrlReg.Value, TRUE);
    }
  }
  IDS_HDT_CONSOLE (CPU_TRACE, "  AGESA set MMIO pair #%X, 0x%X0000 ~ 0x%XFFFF DstFabricID: 0x%X\n", MmioPairIndex, (UINT32) (BaseAddress >> 16), (UINT32) ((BaseAddress + Length - 1) >> 16), MmioAddrCtrlReg.Field.DstFabricID);
  */
}

UINT64
FabricZenZpGetBaseAddress (
  IN       FABRIC_MMIO_REGION *MmioRegion,
  IN       UINT64              Length
  )
{
  UINT64 MmioBaseAddress;

  // if size >= 16M, then we allocate MMIO from bottom, since our MMIO space is 16M-aligned
  // else we allocate MMIO from top
  if (Length >= 0x1000000) {
    MmioBaseAddress = MmioRegion->Base + MmioRegion->UsedSizeFromBottom;
    MmioRegion->UsedSizeFromBottom += Length;
  } else {
    MmioBaseAddress = MmioRegion->Base + MmioRegion->Size - MmioRegion->UsedSizeFromTop - Length;
    MmioRegion->UsedSizeFromTop += Length;
  }

  return MmioBaseAddress;
}
