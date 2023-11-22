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
#include <Library/PcdLib.h>
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
#include <Library/HygonHeapLib.h>
#include <Library/GnbHeapLib.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/HygonCxl11ServicesProtocol.h>
#include <Protocol/HygonCxl20ServicesProtocol.h>
#include <Protocol/HygonCxlManagerProtocol.h>
#include <Protocol/HygonNbioPcieServicesProtocol.h>

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define FILECODE        NBIO_CXL_HYGONCXLDXE_CXLCEDT_FILECODE

#define HYGON_CXL_RC_DEV                  3
#define HYGON_CXL_RC_FUN                  1

#define CEDT_TABLE_LENGTH                 8 * 1024
#define CEDT_CHBS_ENTRY_TYPE              0
#define CEDT_CHBS_ENTRY_LENGTH            32
#define CEDT_CHBS_CXL_VERSION_1P1         0x0000
#define CEDT_CHBS_CXL_VERSION_2P0         0x0001
#define CEDT_CHBS_CXL_VERSION_1P1_LENGTH  0x2000
#define CEDT_CHBS_CXL_VERSION_2P0_LENGTH  0x10000

#define CEDT_CFMWS_ENTRY_TYPE             1
#define CEDT_CFMWS_INTERLEAVE_ARITHMETIC  0
#define CEDT_CFMWS_ENTRY_LENGTH           36 

// Encoded NUmber of Interleave Ways (ENIW)
#define INTERLEAVE_1_WAY                  0
#define INTERLEAVE_2_WAY                  1
#define INTERLEAVE_4_WAY                  2
#define INTERLEAVE_8_WAY                  3

// Encoded Interleave Granularity (IG)
#define INTERLEAVE_GRANULARITY_256B       0
#define INTERLEAVE_GRANULARITY_512B       1
#define INTERLEAVE_GRANULARITY_1KB        2
#define INTERLEAVE_GRANULARITY_2KB        3
#define INTERLEAVE_GRANULARITY_4KB        4
#define INTERLEAVE_GRANULARITY_8KB        5
#define INTERLEAVE_GRANULARITY_16KB       6

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
#pragma pack (1)

typedef struct {
  UINT8   Sign[4];          ///< Signature
  UINT32  TableLength;      ///< Table Length
  UINT8   Revision;         ///< Revision
  UINT8   Checksum;         ///< Checksum
  UINT8   OemId[6];         ///< OEM ID
  UINT8   OemTableId[8];    ///< OEM Tabled ID
  UINT32  OemRev;           ///< OEM Revision
  UINT8   CreatorId[4];     ///< Creator ID
  UINT32  CreatorRev;       ///< Creator Revision
} CEDT_HEADER;              ///< CXL Early Discovery Table

#pragma pack ()

typedef struct {
  UINT8   Type;             ///< Inidcates CHBS (0)
  UINT8   Reserved;         ///< Reserved
  UINT16  RecordLength;     ///< Length of this record
  UINT32  Uid;              ///< CXL Host Bridge Unique ID
  UINT32  CxlVersion;       ///< CXL Spec version
  UINT32  Reserved2;        ///< Reserved
  UINT64  Base;             ///< Base address (CXL 1.1 DsRCRB, CXL 2.0 CHBCR)
  UINT64  Length;           ///< CXL 1.1 8KB, CXL 2.0 64KB
} CEDT_CHBS_ENTRY;          ///< CXL Host Bridge Structure

typedef struct {
  UINT8   Type;                           ///< Indicates CFMWS (1)
  UINT8   Reserved;                       ///< Reserved
  UINT16  RecordLength;                   ///< Length of this record
  UINT32  Reserved2;                      ///< Reserved
  UINT64  BaseHpa;                        ///< Base of this HPA range
  UINT64  WindowSize;                     ///< Total number of consecutive bytes of HPA
  UINT8   Eniw;                           ///< Encoded number of interleave ways (ENIW)
  UINT8   Ia;                             ///< Interleave arithmetic
  UINT16  Reserved3;                      ///< Reserved
  UINT32  Hbig;                           ///< Host bridge interleave granularity
  UINT16  Wr;                             ///< Window restrictions
  UINT16  QtgId;                          ///< QoS Throttling Group ID
  UINT32  IntlvTargetList[MAX_CXL_COUNT]; ///< Interleave Target List
} CEDT_CFMWS_ENTRY;                       ///< CXL Fixed Memory Window Structure

typedef struct {
  UINT16  RecordLength;
  UINT64  BaseHpa;
  UINT64  WindowSize;
  UINT8   Niw;
  UINT32  Hbig;
  UINT32  Itl[MAX_CXL_COUNT];
} CFMWS_INFO;
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

CEDT_HEADER CedtHeader = {
  {'C', 'E', 'D', 'T'},
  sizeof (CEDT_HEADER),
  1,
  0,
  {'H', 'Y', 'G', 'O', 'N', 0},
  {'H', 'G', 'N', 'C', 'E', 'D', 'T', 0},
  1,
  {'H','G','N',' '},
  1
};

UINT32  mSysCedtChbsUid[MAX_CXL_COUNT] = {0};
UINT8   mSysCedtChbsCount = 0;

/*----------------------------------------------------------------------------------------*/
/**
 * Create CHBS entry
 *
 * @param[in]  Cedt            CEDT header pointer
 *
 */
STATIC
BOOLEAN
CreateChbsEntry (
  IN  VOID    *Cedt
  )
{
  EFI_STATUS                            Status;
  HYGON_NBIO_CXL11_SERVICES_PROTOCOL    *Cxl11ServicesProtocol = NULL;
  HYGON_NBIO_CXL20_SERVICES_PROTOCOL    *Cxl20ServicesProtocol = NULL;
  UINT32                                Index;
  HYGON_CXL_PORT_INFO_STRUCT            Cxl11PortInfo[MAX_CXL_COUNT];
  HYGON_CXL20_PORT_INFO_STRUCT          Cxl20PortInfo[MAX_CXL_COUNT];
  CEDT_CHBS_ENTRY                       *ChbsEntry;
  UINT32                                ChbsUid;
  BOOLEAN                               ChbsEntryCreated = FALSE;
  PCI_ADDR                              PciAddr;
  PCIe_PLATFORM_CONFIG                  *Pcie;
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL   *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB         *PciePlatformConfigHobData;
  GNB_HANDLE                            *GnbHandle;

  IDS_HDT_CONSOLE (MAIN_FLOW, "CreateChbsEntry Entry\n");
  
  Status = gBS->LocateProtocol (
                   &gHygonNbioPcieServicesProtocolGuid,
                   NULL,
                   &PcieServicesProtocol
                   );
  if (EFI_ERROR (Status)) {
    return ChbsEntryCreated;
  }
  PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **) &PciePlatformConfigHobData);
  Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
  
  
  Status = gBS->LocateProtocol (
    &gHygonNbioCxl11ServicesProtocolGuid,
    NULL,
    (VOID **)&Cxl11ServicesProtocol
    );
  if (!EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Create CXL 1.1 CHBS \n");
    
    ChbsUid = 0x10; //CXL 1.1 UID 0x10~0x1F
    for (Index = 0; Index < Cxl11ServicesProtocol->Cxl11Count; Index++) {
      Status = Cxl11ServicesProtocol->Cxl11GetRootPortInfoByIndex (Cxl11ServicesProtocol, Index, &Cxl11PortInfo[Index]);
      if (!EFI_ERROR (Status)) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "  - Create Entry %d UID %d \n", Index, ChbsUid);
        mSysCedtChbsUid[mSysCedtChbsCount] = ChbsUid;
        mSysCedtChbsCount++;
        
        ChbsEntry = (CEDT_CHBS_ENTRY*) ((UINT8 *)Cedt + ((CEDT_HEADER *) Cedt)->TableLength);
        ChbsEntry->Type = CEDT_CHBS_ENTRY_TYPE;
        ChbsEntry->RecordLength = CEDT_CHBS_ENTRY_LENGTH;
        ChbsEntry->Uid = ChbsUid++;
        ChbsEntry->CxlVersion = CEDT_CHBS_CXL_VERSION_1P1;
        ChbsEntry->Base = (UINT64) Cxl11PortInfo[Index].DspRcrb;
        ChbsEntry->Length = CEDT_CHBS_CXL_VERSION_1P1_LENGTH;
        ((CEDT_HEADER *) Cedt)->TableLength = ((CEDT_HEADER *) Cedt)->TableLength + ChbsEntry->RecordLength;
        ChbsEntryCreated = TRUE;
      }
    }
  }

  Status = gBS->LocateProtocol (
    &gHygonNbioCxl20ServicesProtocolGuid,
    NULL,
    (VOID **)&Cxl20ServicesProtocol
    );
  if (!EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Create CXL 2.0 CHBS \n");
    
    Index = 0;
    ChbsUid = 0;  //CXL 2.0 UID 0x00~0x0F
    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      if (GnbHandle->CxlPresent) {
        PciAddr.AddressValue = 0;
        PciAddr.Address.Bus = GnbHandle->Address.Address.Bus;
        PciAddr.Address.Device = HYGON_CXL_RC_DEV;
        PciAddr.Address.Function = HYGON_CXL_RC_FUN;
        
        Status = Cxl20ServicesProtocol->Cxl20GetRootPortInfo (Cxl20ServicesProtocol, PciAddr, &Cxl20PortInfo[Index]);
        if (!EFI_ERROR (Status)) {
          IDS_HDT_CONSOLE (MAIN_FLOW, "  - Create Entry %d UID %d \n", Index, ChbsUid);
          
          mSysCedtChbsUid[mSysCedtChbsCount] = ChbsUid;
          mSysCedtChbsCount++;
          
          ChbsEntry = (CEDT_CHBS_ENTRY*) ((UINT8 *)Cedt + ((CEDT_HEADER *) Cedt)->TableLength);
          ChbsEntry->Type = CEDT_CHBS_ENTRY_TYPE;
          ChbsEntry->RecordLength = CEDT_CHBS_ENTRY_LENGTH;
          ChbsEntry->Uid = ChbsUid++;
          ChbsEntry->CxlVersion = CEDT_CHBS_CXL_VERSION_2P0;
          ChbsEntry->Base = Cxl20PortInfo[Index].ComponentRegBaseAddr;
          ChbsEntry->Length = CEDT_CHBS_CXL_VERSION_2P0_LENGTH;
          
          ((CEDT_HEADER *) Cedt)->TableLength = ((CEDT_HEADER *) Cedt)->TableLength + ChbsEntry->RecordLength;
          ChbsEntryCreated = TRUE;
          Index++;
        }
      }
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "CreateChbsEntry Exit\n");
  
  return ChbsEntryCreated;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Encode CFMWS NIW
 *
 * @param[in]  Niw             Number of Interleave Ways
 * @return     ENiw            Encoded Number of Interleave Ways
 *
 */
STATIC
UINT8
CfmwsEncodeNiw (
  IN  UINT8 Niw
)
{
  UINT8 Eniw;

  switch (Niw) {
    case 1: Eniw = INTERLEAVE_1_WAY; break;
    case 2: Eniw = INTERLEAVE_2_WAY; break;
    case 4: Eniw = INTERLEAVE_4_WAY; break;
    case 8: Eniw = INTERLEAVE_8_WAY; break;
    default: Eniw = INTERLEAVE_1_WAY; break;
  }

  return Eniw;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Decode CFMWS ENIW
 *
 * @param[in]  Eniw            Encoded Number of Interleave Ways
 * @return     Niw             Number of Interleave Ways
 *
 */
STATIC
UINT8
CfmwsDecodeNiw (
  IN  UINT8 Eniw
)
{
  UINT8 Niw;

  switch (Eniw) {
    case INTERLEAVE_1_WAY: Niw = 1; break;
    case INTERLEAVE_2_WAY: Niw = 2; break;
    case INTERLEAVE_4_WAY: Niw = 4; break;
    case INTERLEAVE_8_WAY: Niw = 8; break;
    default: Niw = 1; break;
  }

  return Niw;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Create CFMWS entry
 *
 * @param[in]  Cedt            CEDT header pointer
 *
 */
STATIC
VOID
CreateCfmwsEntry (
  IN   VOID *Cedt
  )
{
  CEDT_CFMWS_ENTRY               *CfmwsEntry;
  UINT8                          Index;
  UINT8                          Index2;
  CFMWS_INFO                     CfmwsInfoArray[MAX_CXL_COUNT];
  UINT8                          CfmwsEntriesCount;
  HYGON_CXL_MANAGER_PROTOCOL     *CxlMgrProtocol;
  FABRIC_CXL_USED_RESOURCE       CxlResourceSize;
  EFI_STATUS                     Status;
  UINT8                          IntlvTargetListIndex;
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "CreateCfmwsEntry Entry\n");
  
  CfmwsEntriesCount = 0;
  gBS->SetMem(&CfmwsInfoArray, (MAX_CXL_COUNT * sizeof(CFMWS_INFO)), 0);
  
  Status = gBS->LocateProtocol (
    &gHygonCxlManagerProtocolGuid,
    NULL,
    (VOID **)&CxlMgrProtocol
    );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  ERROR: Found gHygonCxlManagerProtocolGuid failed!\n");
    return;
  }
  
  Status = CxlMgrProtocol->FabricCxlGetUsedResource (CxlMgrProtocol, &CxlResourceSize);
  
  for (Index = 0; Index < CxlResourceSize.MemRegionCount; Index++) {
    CfmwsInfoArray[CfmwsEntriesCount].Niw = 1;            //Not interleave : 1 port;  Interleave: Target ports number
    CfmwsInfoArray[CfmwsEntriesCount].RecordLength = CEDT_CFMWS_ENTRY_LENGTH + (4 * CfmwsInfoArray[CfmwsEntriesCount].Niw);
    CfmwsInfoArray[CfmwsEntriesCount].BaseHpa = CxlResourceSize.MemRegion[Index].Base;
    CfmwsInfoArray[CfmwsEntriesCount].WindowSize = CxlResourceSize.MemRegion[Index].Size;
    CfmwsInfoArray[CfmwsEntriesCount].Hbig = 0;
    CfmwsInfoArray[CfmwsEntriesCount].Itl[0] = mSysCedtChbsUid[Index];     //CXL port _UID
    
    IDS_HDT_CONSOLE (MAIN_FLOW, "  CfmwsInfoArray[%d].Itl[0] = 0x%08X \n",  CfmwsEntriesCount, CfmwsInfoArray[CfmwsEntriesCount].Itl[0]);
    
    CfmwsEntriesCount++;
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "  Create CXL CFMWS Entry \n");
  for (Index = 0; Index < CfmwsEntriesCount; Index++) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  - Create Entry %d \n", Index);
    
    CfmwsEntry = (CEDT_CFMWS_ENTRY*) ((UINT8 *)Cedt + ((CEDT_HEADER *) Cedt)->TableLength);
    CfmwsEntry->Type = CEDT_CFMWS_ENTRY_TYPE;
    CfmwsEntry->RecordLength = CfmwsInfoArray[Index].RecordLength;
    CfmwsEntry->BaseHpa = CfmwsInfoArray[Index].BaseHpa;
    CfmwsEntry->WindowSize = CfmwsInfoArray[Index].WindowSize;
    CfmwsEntry->Eniw = CfmwsEncodeNiw (CfmwsInfoArray[Index].Niw);
    CfmwsEntry->Ia = CEDT_CFMWS_INTERLEAVE_ARITHMETIC;
    CfmwsEntry->Hbig = CfmwsInfoArray[Index].Hbig;
    CfmwsEntry->Wr = 0x16;  // Bitmap (Bit 0 = Type 2, Bit 1 = Type 3, Bit 2 = Volatile, Bit 3 = Persistent, Bit 4 = Fixed Device Config)
    CfmwsEntry->QtgId = 0; 
    
    IntlvTargetListIndex = 0;
    for (Index2 = 0; Index2 < CfmwsInfoArray[Index].Niw; Index2++) {
      CfmwsEntry->IntlvTargetList[Index2] = CfmwsInfoArray[Index].Itl[IntlvTargetListIndex];
      IDS_HDT_CONSOLE (MAIN_FLOW, "   CfmwsEntry->IntlvTargetList[%d] = 0x%08X \n",  Index2, CfmwsEntry->IntlvTargetList[Index2]);
      IntlvTargetListIndex++;
    }
    
    ((CEDT_HEADER *) Cedt)->TableLength = ((CEDT_HEADER *) Cedt)->TableLength + CfmwsEntry->RecordLength;
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "CreateCfmwsEntry Exit\n");
  return;
}

/**
 * Dump CEDT
 *
 * @param[in]     Cedt            Pointer to CEDT
 */
STATIC
VOID
CxlCedtDump (
  IN       VOID                 *Cedt
  )
{
  UINT8   *Block;
  UINT8   Index;
  UINT8   Niw;
  Block = (UINT8 *) Cedt + sizeof (CEDT_HEADER);
  IDS_HDT_CONSOLE (GNB_TRACE, "<----------  CEDT Table Start -----------> \n");
  IDS_HDT_CONSOLE (GNB_TRACE, "  Table Length         = 0x%08x\n", ((CEDT_HEADER *) Cedt)-> TableLength);
  IDS_HDT_CONSOLE (GNB_TRACE, "  Revision             = 0x%04x\n", ((CEDT_HEADER *) Cedt)-> Revision);
  IDS_HDT_CONSOLE (GNB_TRACE, "  Checksum             = 0x%02x\n", ((CEDT_HEADER *) Cedt)-> Checksum);
  while (Block < ((UINT8 *) Cedt + ((CEDT_HEADER *) Cedt)->TableLength)) {
    if (*Block == 0) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------CHBS Entry Start -------->\n");
      IDS_HDT_CONSOLE (GNB_TRACE, "  Type                 = 0x%02x\n", ((CEDT_CHBS_ENTRY *) Block)->Type);
      IDS_HDT_CONSOLE (GNB_TRACE, "  Record Length        = 0x%04x\n", ((CEDT_CHBS_ENTRY *) Block)->RecordLength);
      IDS_HDT_CONSOLE (GNB_TRACE, "  UID                  = 0x%08x\n", ((CEDT_CHBS_ENTRY *) Block)->Uid);
      IDS_HDT_CONSOLE (GNB_TRACE, "  CXL Version          = 0x%08x\n", ((CEDT_CHBS_ENTRY *) Block)->CxlVersion);
      IDS_HDT_CONSOLE (GNB_TRACE, "  Base                 = 0x%lx\n", ((CEDT_CHBS_ENTRY *) Block)->Base);
      IDS_HDT_CONSOLE (GNB_TRACE, "  Length               = 0x%lx\n", ((CEDT_CHBS_ENTRY *) Block)->Length);
      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------CHBS Entry End ---------->\n");
      Block = Block + ((CEDT_CHBS_ENTRY *) Block)->RecordLength;
      
    } else if (*Block == 1) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------CFMWS Entry Start -------->\n");
      IDS_HDT_CONSOLE (GNB_TRACE, "  Type                 = 0x%02x\n", ((CEDT_CFMWS_ENTRY *) Block)->Type);
      IDS_HDT_CONSOLE (GNB_TRACE, "  Record Length        = 0x%04x\n", ((CEDT_CFMWS_ENTRY *) Block)->RecordLength);
      IDS_HDT_CONSOLE (GNB_TRACE, "  Base HPA             = 0x%lx\n", ((CEDT_CFMWS_ENTRY *) Block)->BaseHpa);
      IDS_HDT_CONSOLE (GNB_TRACE, "  Window Size          = 0x%lx\n", ((CEDT_CFMWS_ENTRY *) Block)->WindowSize);
      IDS_HDT_CONSOLE (GNB_TRACE, "  ENIW                 = 0x%02x\n", ((CEDT_CFMWS_ENTRY *) Block)->Eniw);
      IDS_HDT_CONSOLE (GNB_TRACE, "  Intlv Arithmetic     = 0x%02x\n", ((CEDT_CFMWS_ENTRY *) Block)->Ia);
      IDS_HDT_CONSOLE (GNB_TRACE, "  HBIG                 = 0x%08x\n", ((CEDT_CFMWS_ENTRY *) Block)->Hbig);
      IDS_HDT_CONSOLE (GNB_TRACE, "  Window Restrictions  = 0x%04x\n", ((CEDT_CFMWS_ENTRY *) Block)->Wr);
      IDS_HDT_CONSOLE (GNB_TRACE, "  QTG ID               = 0x%04x\n", ((CEDT_CFMWS_ENTRY *) Block)->QtgId);
      Niw = CfmwsDecodeNiw (((CEDT_CFMWS_ENTRY *) Block)->Eniw);
      for (Index = 0; Index < Niw; Index++) {
        IDS_HDT_CONSOLE (GNB_TRACE, "  Intlv Target List[%d] = 0x%08x\n", Index, ((CEDT_CFMWS_ENTRY *) Block)->IntlvTargetList[Index]);
      }
      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------CFMWS Entry End ---------->\n");
      Block = Block + ((CEDT_CFMWS_ENTRY *) Block)->RecordLength;
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "<----------  CEDT Table Raw Data --------> \n");
  GnbLibDebugDumpBuffer (Cedt, ((CEDT_HEADER *) Cedt)->TableLength, 1, 16);
  IDS_HDT_CONSOLE (GNB_TRACE, "\n");
  IDS_HDT_CONSOLE (GNB_TRACE, "<----------  CEDT Table End -------------> \n");
}

/*---------------------------------------------------------------------------------------*/
/**
 * Calculate an ACPI style checksum
 *
 * Computes the checksum and stores the value to the checksum
 * field of the passed in ACPI table's header.
 *
 * @param[in]  Table             ACPI table to checksum
 *
 */
VOID
ChecksumAcpiTable (
  IN OUT   ACPI_TABLE_HEADER *Table
  )
{
  UINT8  *BuffTempPtr;
  UINT8  Checksum;
  UINT32 BufferOffset;

  Table->Checksum = 0;
  Checksum = 0;
  BuffTempPtr = (UINT8 *) Table;
  for (BufferOffset = 0; BufferOffset < Table->TableLength; BufferOffset++) {
    Checksum = Checksum - *(BuffTempPtr + BufferOffset);
  }

  Table->Checksum = Checksum;
}
/**
 *---------------------------------------------------------------------------------------
 *  CxlSetCedt
 *
 *  Description:
 *     Create CEDT ACPI table
 *  Parameters:
 *    
 *
 *---------------------------------------------------------------------------------------
 **/
void
CxlSetCedt (
  void
  )
{
  EFI_STATUS                          Status;
  HGPI_STATUS                         AgesaStatus;
  EFI_ACPI_TABLE_PROTOCOL             *AcpiTable;
  UINTN                               TableHandle;
  VOID                                *Cedt;
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "%a Entry\n", __FUNCTION__);
  
  AgesaStatus = HGPI_SUCCESS;
  Cedt = GnbAllocateHeapBuffer (
           HYGON_ACPI_CHBS_BUFFER_HANDLE,
           CEDT_TABLE_LENGTH,
           (HYGON_CONFIG_PARAMS *)NULL
           );
  ASSERT (Cedt != NULL);
  if (Cedt == NULL) {
    return; //AGESA_ERROR;
  }
  
  Cedt = AllocateZeroPool (CEDT_TABLE_LENGTH);
  CopyMem (Cedt, &CedtHeader, sizeof (CedtHeader));
 
  if(CreateChbsEntry (Cedt)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "mSysCedtChbsCount is %d \n", mSysCedtChbsCount);
    
    CreateCfmwsEntry (Cedt);
    ChecksumAcpiTable ((ACPI_TABLE_HEADER*) Cedt);
    CxlCedtDump (Cedt);
    
    //Publish CEDT table
    Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
    if (!EFI_ERROR (Status)) {
      Status = AcpiTable->InstallAcpiTable (AcpiTable, Cedt, ((CEDT_HEADER*) Cedt)->TableLength, &TableHandle);
    }
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "%a Exit\n", __FUNCTION__);
  return;
}
