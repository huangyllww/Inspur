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
#include <CxlCdat.h>
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
#include <Library/GnbPciLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/TimerLib.h>
#include <Protocol/FabricTopologyServices.h>

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define FILECODE        NBIO_CXL_HYGONCXLDXE_CXLCDAT_FILECODE

#define     DVSEC_CAP_DOE               0x2E

#define     DOE_DISCOVERY_PROTOCOL      0x00
#define     DOE_DISCOVERY_VID           0x01
#define     DOE_CMA_PROTOCOL            0x01
#define     DOE_CDAT                    0x02

#define     DOE_CONTROL_OFFSET          0x08
#define        DOE_GO_BIT               BIT31
#define        DOE_ABORT_BIT            BIT0
#define     DOE_STATUS_OFFSET           0x0C
#define        DATA_OBJECT_READY_BIT    BIT31
#define        DOE_ERROR_BIT            BIT2
#define        DOE_BUSY_BIT             BIT0
#define     DOE_MAILBOX_WRITE_OFFSET    0x10
#define     DOE_MAILBOX_READ_OFFSET     0x14

#define     CXL_COMPLIANCE_DOE_MAILBOX  0x00
#define     CXL_CDAT_DOE_MAILBOX        0x02
#define     CXL_VENDOR_ID               0x1E98
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
typedef struct {
  UINT8                     ProtocolType;
  UINT16                    DoeDvsecPtr;
  UINT32                    CxlAddress;
} DOE_INFO;

typedef union {
  struct {
    UINT32  Length:18;
    UINT32  Reserved:14;
  } Field;
  UINT32 Value;
} DOE_DATA_OBJECT_HEADER_2;

typedef union {
  struct {
    UINT32  VendorId:16;
    UINT32  DoeType:8;
    UINT32  Reserved:8;
  } Field;
  UINT32 Value;
} DOE_DATA_OBJECT_HEADER_1;

typedef union {
  struct {
    UINT32 Index:8;
    UINT32 Reserved:24;
  } Field;
  UINT32 Value;
} DOE_DISCOVERY_CONTENTS;

typedef union {
  struct {
    UINT32 RequestCode:8;
    UINT32 TableType:8;
    UINT32 EntryHandle:16;
  } Field;
  UINT32 Value;
} DOE_READ_CONTENTS;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Read DOE capability register
 *
 * @param[in]   DoeInfo    The CXL endpoint info used in the exchange
 * @param[in]   RegOffset  The DOE register offset
 *
 * @return      RegValue
 */
UINT32 
ReadDoeReg (
  IN  DOE_INFO    *DoeInfo,
  IN  UINT16      RegOffset
) 
{
  UINT32 Value = 0;
  
  GnbLibPciRead (
    DoeInfo->CxlAddress | (DoeInfo->DoeDvsecPtr + RegOffset),
    AccessWidth32,
    &Value,
    NULL
  );

  return Value;
}

/**
 * Write DOE capability register
 *
 * @param[in]   DoeInfo    The CXL endpoint info used in the exchange
 * @param[in]   RegOffset  The DOE register offset
 *              Value      The pointer to write value
 *
 * @return      VOID
 */
VOID 
WriteDoeReg (
  IN  DOE_INFO    *DoeInfo,
  IN  UINT16      RegOffset,
  IN  UINT32      *Value
) 
{ 
  GnbLibPciWrite (
    DoeInfo->CxlAddress | (DoeInfo->DoeDvsecPtr + RegOffset),
    AccessWidth32,
    Value,
    NULL
  );
}

/*----------------------------------------------------------------------------------------*/
/**
 * DOE Operation
 *
 * @param[in]   DoeInfo    The CXL endpoint info used in the exchange
 * @param[in]   DoeReq     The Data Object request to exchange
 * @param[in]   DoeResp    The Data Object response
 *
 * @return                 TRUE if successful, FALSE if unsuccessful
 */
STATIC
BOOLEAN
CxlDoe (
  IN  DOE_INFO    *DoeInfo,
  IN  UINT32      *DoeReq,
  IN  UINT32      *DoeResp
)
{
  UINT32    DoeStatus;
  UINTN     Index;
  UINT32    DoeValue;
  UINT16    ReadyRetry = 0;
  BOOLEAN   ResponseReady = FALSE;
  UINT32    ResponseLength;

  DoeStatus = ReadDoeReg (DoeInfo, DOE_STATUS_OFFSET);
  IDS_HDT_CONSOLE (GNB_TRACE, "(CxlDoe) DOE Status: 0x%x \n", DoeStatus);

  if ((DoeStatus & DOE_ERROR_BIT) != 0) {
    IDS_HDT_CONSOLE (GNB_TRACE, "DOE Error, abort it!\n");

    DoeValue = DOE_ABORT_BIT;
    WriteDoeReg(DoeInfo, DOE_CONTROL_OFFSET, &DoeValue);

    DoeStatus = ReadDoeReg (DoeInfo, DOE_STATUS_OFFSET);
  }

  if ((DoeStatus & DOE_BUSY_BIT) == 0) {
    // Write to mailbox one dword at a time
    for (Index = 0; Index < 3; Index++) {
      DoeValue = *DoeReq;
      //IDS_HDT_CONSOLE (GNB_TRACE, "(CxlDoe) Writing 0x%08x to mailbox\n", DoeValue);
      WriteDoeReg(DoeInfo, DOE_MAILBOX_WRITE_OFFSET, &DoeValue);
      DoeReq++;
    }

    //IDS_HDT_CONSOLE (GNB_TRACE, "(CxlDoe) Write GO bit\n");
    DoeValue = DOE_GO_BIT;
    WriteDoeReg(DoeInfo, DOE_CONTROL_OFFSET, &DoeValue);

    // Look for Data Object Ready bit
    do {
      ReadyRetry++;
      DoeStatus = ReadDoeReg (DoeInfo, DOE_STATUS_OFFSET);
      if (DoeStatus & DATA_OBJECT_READY_BIT) {
        ResponseReady = TRUE;
        break;
      } else if (DoeStatus & DOE_ERROR_BIT) {
        IDS_HDT_CONSOLE (GNB_TRACE, "DOE Error, abort it!\n");
        // Set Abort bit in Control register
        DoeValue = DOE_ABORT_BIT;
        WriteDoeReg(DoeInfo, DOE_CONTROL_OFFSET, &DoeValue);
        break;
      }
      MicroSecondDelay (1000);    // 1ms
    } while (ReadyRetry < 1000);  //The ready or error bit should be set within 1 second

    if (ResponseReady) {
      //IDS_HDT_CONSOLE (GNB_TRACE, "(CxlDoe) Mailbox response ready\n");
      DoeValue = 0;
      DoeResp[0] = ReadDoeReg (DoeInfo, DOE_MAILBOX_READ_OFFSET);
      WriteDoeReg(DoeInfo, DOE_MAILBOX_READ_OFFSET, &DoeValue);
      //IDS_HDT_CONSOLE (GNB_TRACE, "(CxlDoe) DOE response [0] = 0x%08x \n", DoeResp[0]);

      DoeResp[1] = ReadDoeReg (DoeInfo, DOE_MAILBOX_READ_OFFSET);
      WriteDoeReg(DoeInfo, DOE_MAILBOX_READ_OFFSET, &DoeValue);
      //IDS_HDT_CONSOLE (GNB_TRACE, "(CxlDoe) DOE response [1] = 0x%08x \n", DoeResp[1]);

      // Get the response length (17:0 in DWORD 2)
      ResponseLength = (0x0003FFFF & DoeResp[1]);
      //IDS_HDT_CONSOLE (GNB_TRACE, "(CxlDoe) DOE response length = 0x%08x \n", ResponseLength));

      for (Index = 2; Index < ResponseLength; Index++) {
        DoeResp[Index] = ReadDoeReg (DoeInfo, DOE_MAILBOX_READ_OFFSET);
        WriteDoeReg(DoeInfo, DOE_MAILBOX_READ_OFFSET, &DoeValue);
        //IDS_HDT_CONSOLE (GNB_TRACE, "(CxlDoe) DOE response [%d] = 0x%08x \n", Index, DoeResp[Index]);
      }
    } else {
      IDS_HDT_CONSOLE (GNB_TRACE, "DOE Response ready timed out!\n");
      return FALSE;
    }

  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "DOE Mailbox is busy!\n");
    return FALSE;
  }

  return TRUE;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Performs a DOE Discovery
 *
 * @param[in]   DoeInfo    CXL Endpoint Info used in the DOE
 *
 * @return                 TRUE if successful, FALSE if unsuccessful
 */
STATIC
BOOLEAN
CxlDoeDiscovery (
  IN  DOE_INFO    *DoeInfo
) {
  DOE_DATA_OBJECT_HEADER_1 Header1;
  DOE_DATA_OBJECT_HEADER_2 Header2;
  DOE_DISCOVERY_CONTENTS   DoeDiscoveryContents;
  UINT32                   DoeReq[3];
  UINT32                   DoeDiscoveryResp[4];
  UINT8                    DiscoveryIndex = 0;
  BOOLEAN                  CxlCdatDoe = FALSE;

  do {
    Header1.Field.VendorId = DOE_DISCOVERY_VID;
    Header1.Field.DoeType = DOE_DISCOVERY_PROTOCOL;
    Header1.Field.Reserved = 0;
    Header2.Field.Length = sizeof(DoeReq) / 4;
    Header2.Field.Reserved = 0;
    DoeDiscoveryContents.Field.Index = DiscoveryIndex;  //this gets incremented by the response
    DoeDiscoveryContents.Field.Reserved = 0;
    DoeReq[0] = Header1.Value;
    DoeReq[1] = Header2.Value;
    DoeReq[2] = DoeDiscoveryContents.Value;

    //IDS_HDT_CONSOLE (GNB_TRACE, "(CxlDoe) DoeDiscoveryReq 0: 0x%08x \n", DoeReq[0]);
    //IDS_HDT_CONSOLE (GNB_TRACE, "(CxlDoe) DoeDiscoveryReq 1: 0x%08x \n", DoeReq[1]);
    //IDS_HDT_CONSOLE (GNB_TRACE, "(CxlDoe) DoeDiscoveryReq 2: 0x%08x \n", DoeReq[2]);

    // Write to the mailbox and get a response
    if(CxlDoe (DoeInfo, DoeReq, DoeDiscoveryResp)) {
      //IDS_HDT_CONSOLE (GNB_TRACE, "(CxlDoe) DOE Discovery Response: 0x%x \n", DoeDiscoveryResp[2]);
      // Check for Vendor ID = 1E98h and Data Object Protocol = 2h (CDAT)
      if((DoeDiscoveryResp[2] & 0x00021E98) == 0x00021E98) {
        IDS_HDT_CONSOLE (GNB_TRACE, "Found CDAT Mailbox\n");
        //DoeInfo->IndexLocation = DiscoveryIndex;
        DoeInfo->ProtocolType = CXL_CDAT_DOE_MAILBOX;
        CxlCdatDoe = TRUE;
        break;
      } else {
        DiscoveryIndex = (UINT8) ((0xFF000000 & DoeDiscoveryResp[2]) >> 24);
        //IDS_HDT_CONSOLE (GNB_TRACE, "(CxlDoe) DOE Discovery Next Index: 0x%02x \n", DiscoveryIndex);
      }
    }
  } while (DiscoveryIndex != 0);

  return CxlCdatDoe;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Performs CXL DOE Discovery and reads the CDAT
 *
 * @param[in]   CxlAddress        CXL Address
 * @param[in]   DoeCapPtr         CXL DOE extended capability address
 * @param[out]  CDAT_TABLE        Pointer to CDAT of a CXL device
 *
 */
EFI_STATUS
CxlReadCdat (
  IN      UINT32      CxlAddress,
  IN      UINT16      DoeCapPtr,
  OUT     CDAT_TABLE  *CdatTable
  )
{
  DOE_INFO                  DoeInfo;
  DOE_DATA_OBJECT_HEADER_1  Header1;
  DOE_DATA_OBJECT_HEADER_2  Header2;
  DOE_READ_CONTENTS         DoeReadEntryContents;
  UINT32                    DoeReq[3];
  UINT32                    DoeReadEntryResp[32];
  UINT32                    EntryHandle = 0;
  UINT8                     EntryType;
  UINT8                     *TableEnd;
  DSMAS_CDAT                *CdatDsmas;
  DSLBIS_CDAT               *CdatDslbis;
  DSMSCIS_CDAT              *CdatDsmscis;
  DSIS_CDAT                 *CdatDsis;
  DSEMTS_CDAT               *CdatDsemts;
  UINTN                     CdatStructSize;

  IDS_HDT_CONSOLE (MAIN_FLOW, "%a Entry\n", __FUNCTION__);

  if (CdatTable == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DoeInfo.CxlAddress = CxlAddress;
  DoeInfo.DoeDvsecPtr = DoeCapPtr;

  CdatStructSize = (MAX_DSMAS_RECORDS_PER_CXL_DEVICE * sizeof (DSMAS_CDAT)) +
                   (MAX_DSLBIS_RECORDS_PER_CXL_DEVICE * sizeof (DSLBIS_CDAT)) +
                   (MAX_DSMSCIS_RECORDS_PER_CXL_DEVICE * sizeof (DSMSCIS_CDAT)) +
                   (MAX_DSIS_RECORDS_PER_CXL_DEVICE * sizeof (DSIS_CDAT)) +
                   (MAX_DSEMTS_RECORDS_PER_CXL_DEVICE * sizeof (DSEMTS_CDAT));

  CdatTable->Entries = AllocateZeroPool (CdatStructSize);
  if (CdatTable->Entries == NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "%a ERROR: Cannot allocate memory for CDAT for CXL device at address 0x%x\n", __FUNCTION__, CxlAddress);
    return EFI_OUT_OF_RESOURCES;
  }
  TableEnd = (UINT8*) CdatTable->Entries;

  // If there is a CDAT mailbox, gather the CDAT structure
  if (CxlDoeDiscovery (&DoeInfo)) {
    do {
      ZeroMem (&DoeReadEntryResp, sizeof (DoeReadEntryResp));
      Header1.Field.VendorId = CXL_VENDOR_ID;
      Header1.Field.DoeType = DOE_CDAT;
      Header1.Field.Reserved = 0;
      Header2.Field.Length = 0x3;
      Header2.Field.Reserved = 0;
      DoeReadEntryContents.Field.RequestCode = 0;
      DoeReadEntryContents.Field.TableType = 0;
      DoeReadEntryContents.Field.EntryHandle = EntryHandle;
      DoeReq[0] = Header1.Value;
      DoeReq[1] = Header2.Value;
      DoeReq[2] = DoeReadEntryContents.Value;

      // write to the mailbox and get a response
      if (CxlDoe (&DoeInfo, DoeReq, DoeReadEntryResp)) {
        if (EntryHandle == 0) {
          CdatTable->Header.Length = DoeReadEntryResp[3];
          CdatTable->Header.Revision = (UINT8) (0x000000FF & DoeReadEntryResp[4]);
          CdatTable->Header.Checksum = (UINT8) ((0x0000FF00 & DoeReadEntryResp[4]) >> 8);
          CdatTable->Header.Sequence = DoeReadEntryResp[5];
          IDS_HDT_CONSOLE (GNB_TRACE, "CDAT Length = 0x%08x, Revision = 0x%02x, Checksum = 0x%02x, Sequence = 0x%08x \n",
            CdatTable->Header.Length,
            CdatTable->Header.Revision,
            CdatTable->Header.Checksum,
            CdatTable->Header.Sequence
            );
        } else {
          EntryType = (UINT8) (0x000000FF & DoeReadEntryResp[3]);
          //IDS_HDT_CONSOLE (GNB_TRACE, "Entry Type: 0x%02x \n", EntryType);

          switch (EntryType) {
            case CdatTypeDsmas:
              CdatDsmas = (DSMAS_CDAT*) TableEnd;
              TableEnd += sizeof (DSMAS_CDAT);
              // Boundary check
              if ((UINTN)(TableEnd - (UINT8*)CdatTable->Entries) > CdatStructSize) {
                IDS_HDT_CONSOLE (GNB_TRACE, "%a ERROR: Allocation size is smaller than actual CDAT size.  Free mem and return error.\n", __FUNCTION__);
                IDS_HDT_CONSOLE (GNB_TRACE, "\tAllocated size = %d, Actual Size at %d\n", CdatStructSize, (UINTN)(TableEnd - (UINT8*)CdatTable->Entries));
                ZeroMem (&CdatTable->Header, sizeof (CDAT_HEADER));
                FreePool (CdatTable->Entries);
                return EFI_OUT_OF_RESOURCES;
              }

              CdatDsmas->Type = (UINT8) EntryType;
              CdatDsmas->Length = (UINT16) ((0xFFFF0000 & DoeReadEntryResp[3]) >> 16);
              CdatDsmas->Handle = (UINT8) (0x000000FF & DoeReadEntryResp[4]);
              CdatDsmas->Flags = (UINT8) ((0x0000FF00 & DoeReadEntryResp[4]) >> 8);
              CdatDsmas->DpaBase = LShiftU64((UINT64) DoeReadEntryResp[6], 32) | DoeReadEntryResp[5];
              CdatDsmas->DpaLength = LShiftU64((UINT64) DoeReadEntryResp[8], 32) | DoeReadEntryResp[7];

              IDS_HDT_CONSOLE (GNB_TRACE, "CDAT DSMAS Handle = 0x%02x, Flags = 0x%02x, DPA Base = 0x%lx, DPA Length = 0x%lx \n",
                CdatDsmas->Handle,
                CdatDsmas->Flags,
                CdatDsmas->DpaBase,
                CdatDsmas->DpaLength
                );
              break;
            case CdatTypeDslbis:
              CdatDslbis = (DSLBIS_CDAT*) TableEnd;
              TableEnd += sizeof (DSLBIS_CDAT);
              // Boundary check
              if ((UINTN)(TableEnd - (UINT8*)CdatTable->Entries) > CdatStructSize) {
                IDS_HDT_CONSOLE (GNB_TRACE, "%a ERROR: Allocation size is smaller than actual CDAT size.  Free mem and return error.\n", __FUNCTION__);
                IDS_HDT_CONSOLE (GNB_TRACE, "\tAllocated size = %d, Actual Size at %d\n", CdatStructSize, (UINTN)(TableEnd - (UINT8*)CdatTable->Entries));
                ZeroMem (&CdatTable->Header, sizeof (CDAT_HEADER));
                FreePool (CdatTable->Entries);
                return EFI_OUT_OF_RESOURCES;
              }

              CdatDslbis->Type = (UINT8) EntryType;
              CdatDslbis->Length = (UINT16) ((0xFFFF0000 & DoeReadEntryResp[3]) >> 16);
              CdatDslbis->Handle = (UINT8) (0x000000FF & DoeReadEntryResp[4]);
              CdatDslbis->Flags = (UINT8) ((0x0000FF00 & DoeReadEntryResp[4]) >> 8);
              CdatDslbis->DataType = (UINT8) ((0x00FF0000 & DoeReadEntryResp[4]) >> 16);
              CdatDslbis->EntryBaseUnit = LShiftU64((UINT64) DoeReadEntryResp[6], 32) | DoeReadEntryResp[5];
              CdatDslbis->Entry[0] = (0x0000FFFF & DoeReadEntryResp[7]);
              CdatDslbis->Entry[1] = ((0xFFFF0000 & DoeReadEntryResp[7]) >> 16);
              CdatDslbis->Entry[2] = (0x0000FFFF & DoeReadEntryResp[8]);

              IDS_HDT_CONSOLE (GNB_TRACE, "CDAT DSLBIS Handle = 0x%02x, Flags = 0x%02x, Data Type = 0x%02x, Base Unit = 0x%x \n",
                CdatDslbis->Handle,
                CdatDslbis->Flags,
                CdatDslbis->DataType,
                CdatDslbis->EntryBaseUnit
                );
              IDS_HDT_CONSOLE (GNB_TRACE, "\tDSLBIS Entry 0 = 0x%x \n", CdatDslbis->Entry[0]);
              IDS_HDT_CONSOLE (GNB_TRACE, "\tDSLBIS Entry 1 = 0x%x \n", CdatDslbis->Entry[1]);
              IDS_HDT_CONSOLE (GNB_TRACE, "\tDSLBIS Entry 2 = 0x%x \n", CdatDslbis->Entry[2]);
              break;

            case CdatTypeDsmscis:
              CdatDsmscis = (DSMSCIS_CDAT*) TableEnd;
              TableEnd += sizeof (DSMSCIS_CDAT);
              // Boundary check
              if ((UINTN)(TableEnd - (UINT8*)CdatTable->Entries) > CdatStructSize) {
                IDS_HDT_CONSOLE (GNB_TRACE, "%a ERROR: Allocation size is smaller than actual CDAT size.  Free mem and return error.\n", __FUNCTION__);
                IDS_HDT_CONSOLE (GNB_TRACE, "\tAllocated size = %d, Actual Size at %d\n", CdatStructSize, (UINTN)(TableEnd - (UINT8*)CdatTable->Entries));
                ZeroMem (&CdatTable->Header, sizeof (CDAT_HEADER));
                FreePool (CdatTable->Entries);
                return EFI_OUT_OF_RESOURCES;
              }

              CdatDsmscis->Type = (UINT8) EntryType;
              CdatDsmscis->Length = (UINT16) ((0xFFFF0000 & DoeReadEntryResp[3]) >> 16);
              CdatDsmscis->Handle = (UINT8) (0x000000FF & DoeReadEntryResp[4]);
              CdatDsmscis->MemSideCacheSize = LShiftU64((UINT64) DoeReadEntryResp[6], 32) | DoeReadEntryResp[5];
              CdatDsmscis->CacheAttributes = DoeReadEntryResp[7];

              IDS_HDT_CONSOLE (GNB_TRACE, "CDAT DSMSCIS Handle = 0x%02x, Mem Side Cache Size = 0x%x, Cache Attributes = 0x%08x",
                CdatDsmscis->Handle,
                CdatDsmscis->MemSideCacheSize,
                CdatDsmscis->CacheAttributes
                );
              break;

            case CdatTypeDsis:
              CdatDsis = (DSIS_CDAT*) TableEnd;
              TableEnd += sizeof (DSIS_CDAT);
              // Boundary check
              if ((UINTN)(TableEnd - (UINT8*)CdatTable->Entries) > CdatStructSize) {
                IDS_HDT_CONSOLE (GNB_TRACE, "%a ERROR: Allocation size is smaller than actual CDAT size.  Free mem and return error.\n", __FUNCTION__);
                IDS_HDT_CONSOLE (GNB_TRACE, "\tAllocated size = %d, Actual Size at %d\n", CdatStructSize, (UINTN)(TableEnd - (UINT8*)CdatTable->Entries));
                ZeroMem (&CdatTable->Header, sizeof (CDAT_HEADER));
                FreePool (CdatTable->Entries);
                return EFI_OUT_OF_RESOURCES;
              }

              CdatDsis->Type = (UINT8) EntryType;
              CdatDsis->Length = (UINT16) ((0xFFFF0000 & DoeReadEntryResp[3]) >> 16);
              CdatDsis->Flags = (UINT8) (0x000000FF & DoeReadEntryResp[4]);
              CdatDsis->Handle = (UINT8) ((0x0000FF00 & DoeReadEntryResp[4]) >> 8);
              IDS_HDT_CONSOLE (GNB_TRACE, "CDAT DSIS Handle = 0x%02x, Flags = 0x%02x", CdatDsis->Handle, CdatDsis->Flags);
              break;

            case CdatTypeDsemts:
              CdatDsemts = (DSEMTS_CDAT*) TableEnd;
              TableEnd += sizeof (DSEMTS_CDAT);
              // Boundary check
              if ((UINTN)(TableEnd - (UINT8*)CdatTable->Entries) > CdatStructSize) {
                IDS_HDT_CONSOLE (GNB_TRACE, "%a ERROR: Allocation size is smaller than actual CDAT size.  Free mem and return error.\n", __FUNCTION__);
                IDS_HDT_CONSOLE (GNB_TRACE, "\tAllocated size = %d, Actual Size at %d\n", CdatStructSize, (UINTN)(TableEnd - (UINT8*)CdatTable->Entries));
                ZeroMem (&CdatTable->Header, sizeof (CDAT_HEADER));
                FreePool (CdatTable->Entries);
                return EFI_OUT_OF_RESOURCES;
              }

              CdatDsemts->Type = (UINT8) EntryType;
              CdatDsemts->Length = (UINT16) ((0xFFFF0000 & DoeReadEntryResp[3]) >> 16);
              CdatDsemts->Handle = (UINT8) (0x000000FF & DoeReadEntryResp[4]);
              CdatDsemts->EfiTypeAndAttributes = (UINT8) ((0x0000FF00 & DoeReadEntryResp[4]) >> 8);
              CdatDsemts->DpaOffset = LShiftU64((UINT64) DoeReadEntryResp[6], 32) | DoeReadEntryResp[5];
              CdatDsemts->DpaLength = LShiftU64((UINT64) DoeReadEntryResp[8], 32) | DoeReadEntryResp[7];
              IDS_HDT_CONSOLE (GNB_TRACE, "CDAT DSEMTS Handle = 0x%02x, EFI Info = 0x%02x, DPA Offset = 0x%lx, DPA Length = 0x%lx \n",
                CdatDsemts->Handle,
                CdatDsemts->EfiTypeAndAttributes,
                CdatDsemts->DpaOffset,
                CdatDsemts->DpaLength
                );
              break;

            case CdatTypeSslbis:
              // SSLBIS not currently supported in CXL 1.1
            default:
              break;
          }
        }
        // Get next entry handle
        EntryHandle = ((0xFFFF0000 & DoeReadEntryResp[2]) >> 16);
        //IDS_HDT_CONSOLE (GNB_TRACE, "Next Entry Handle: 0x%08x \n", EntryHandle);
      }
    } while (EntryHandle != 0xFFFF);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "%a Exit\n", __FUNCTION__);
  return EFI_SUCCESS;
}

/**
 * Gets the pointer to the CDAT for a CXL device. Caller needs to free contents of CdatTable.
 *
 * @param[in]    Address                           Address of CXL device.
 * @param[out]   CdatTable                         Pointer to CDAT for a CXL device
*/
EFI_STATUS
EFIAPI
CxlGetCdat (
  IN   HYGON_NBIO_CXL20_SERVICES_PROTOCOL   *This,
  IN   UINT32                                Address,
  OUT  CDAT_TABLE                           *CdatTable
)
{
  EFI_STATUS                          Status;
  UINT16                              PcieCapPtr;

  if (CdatTable == NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "%a ERROR: Invalid parameter passed through\n", __FUNCTION__);
    return EFI_INVALID_PARAMETER;
  }

  if (!PcdGetBool (PcdCxlCdatSupport)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "%a - CDAT Unsupported.\n", __FUNCTION__);
    return EFI_UNSUPPORTED;
  }

  PcieCapPtr = GnbLibFindPcieExtendedCapability (
                 Address,
                 DVSEC_CAP_DOE,
                 NULL
                 );

  if (PcieCapPtr != 0) {
    Status = CxlReadCdat (
               Address,
               PcieCapPtr,
               CdatTable
               );
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "%a CXL device does not support DOE extended capability\n", __FUNCTION__);
    return EFI_UNSUPPORTED;
  }

  return Status;
}

/**
 * Reads a CDAT for a CXL device for Entries of CDAT struct type passed through
 *
 * @param[in]   CdatTable                         Pointer to CDAT of a CXL device
 * @param[in]   CdatType                          CDAT Struct Type to return
 * @param[out]  CdatStruct                        Pointer to struct entries in CDAT
 *
 */
EFI_STATUS
EFIAPI
CxlParseCdat (
  IN      HYGON_NBIO_CXL20_SERVICES_PROTOCOL  *This,
  IN      CDAT_TABLE                           CdatTable,
  IN      UINT8                                CdatType,
  OUT     VOID                                *CdatStruct
  )
{
  UINT8          *TableEnd;
  UINT8          *Temp;
  UINT8          EntryType;
  UINT8          RecordCount;
  UINT8          MaxRecordsOfType;
  UINT16         Length;
  UINTN          Index;
  UINTN          MaxCdatEntries;

  if (CdatStruct == NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "%a ERROR: Invalid parameter passed through\n", __FUNCTION__);
    return EFI_INVALID_PARAMETER;
  }

  if ((CdatTable.Header.Length == 0) || (CdatTable.Entries == NULL)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "%a ERROR: No CDAT data for CXL device\n", __FUNCTION__);
    return EFI_INVALID_PARAMETER;
  }

  if (!PcdGetBool (PcdCxlCdatSupport)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "%a - CDAT Unsupported.\n", __FUNCTION__);
    return EFI_UNSUPPORTED;
  }

  // SSLBIS not currently supported in CXL 1.1
  switch (CdatType) {
    case CdatTypeDsmas:
      MaxRecordsOfType = MAX_DSMAS_RECORDS_PER_CXL_DEVICE;
      break;
    case CdatTypeDslbis:
      MaxRecordsOfType = MAX_DSLBIS_RECORDS_PER_CXL_DEVICE;
      break;
    case CdatTypeDsmscis:
      MaxRecordsOfType = MAX_DSMSCIS_RECORDS_PER_CXL_DEVICE;
      break;
    case CdatTypeDsis:
      MaxRecordsOfType = MAX_DSIS_RECORDS_PER_CXL_DEVICE;
      break;
    case CdatTypeDsemts:
      MaxRecordsOfType = MAX_DSEMTS_RECORDS_PER_CXL_DEVICE;
      break;
    default:
      IDS_HDT_CONSOLE (GNB_TRACE, "%a ERROR: Unsupported CDAT type passed through\n", __FUNCTION__);
      return EFI_UNSUPPORTED;
  }

  RecordCount = 0;
  TableEnd = (UINT8*) CdatTable.Entries;
  MaxCdatEntries = MAX_DSMAS_RECORDS_PER_CXL_DEVICE +
                   MAX_DSLBIS_RECORDS_PER_CXL_DEVICE +
                   MAX_DSMSCIS_RECORDS_PER_CXL_DEVICE +
                   MAX_DSIS_RECORDS_PER_CXL_DEVICE +
                   MAX_DSEMTS_RECORDS_PER_CXL_DEVICE;

  for (Index = 0; (Index < MaxCdatEntries) && (RecordCount < MaxRecordsOfType); Index++) {
    EntryType = TableEnd[0];
    Length = (UINT16) ((TableEnd[3] << 8) | TableEnd[2]);
    if (Length == 0) {
      break;
    }

    Temp = (UINT8*) CdatStruct;
    switch (EntryType) {
      case CdatTypeDsmas:
        if (CdatType == CdatTypeDsmas) {
          Temp += RecordCount * sizeof (DSMAS_CDAT);
          CopyMem ((VOID*)Temp, (VOID*)TableEnd, sizeof (DSMAS_CDAT));
          RecordCount++;
        }
        TableEnd += sizeof (DSMAS_CDAT);
        break;
      case CdatTypeDslbis:
        if (CdatType == CdatTypeDslbis) {
          Temp += RecordCount * sizeof (DSLBIS_CDAT);
          CopyMem ((VOID*)Temp, (VOID*)TableEnd, sizeof (DSLBIS_CDAT));
          RecordCount++;
        }
        TableEnd += sizeof (DSLBIS_CDAT);
        break;
      case CdatTypeDsmscis:
        if (CdatType == CdatTypeDsmscis) {
          Temp += RecordCount * sizeof (DSMSCIS_CDAT);
          CopyMem ((VOID*)Temp, (VOID*)TableEnd, sizeof (DSMSCIS_CDAT));
          RecordCount++;
        }
        TableEnd += sizeof (DSMSCIS_CDAT);
        break;
      case CdatTypeDsis:
        if (CdatType == CdatTypeDsis) {
          Temp += RecordCount * sizeof (DSIS_CDAT);
          CopyMem ((VOID*)Temp, (VOID*)TableEnd, sizeof (DSIS_CDAT));
          RecordCount++;
        }
        TableEnd += sizeof (DSIS_CDAT);
        break;
      case CdatTypeDsemts:
        if (CdatType == CdatTypeDsemts) {
          Temp += RecordCount * sizeof (DSEMTS_CDAT);
          CopyMem ((VOID*)Temp, (VOID*)TableEnd, sizeof (DSEMTS_CDAT));
          RecordCount++;
        }
        TableEnd += sizeof (DSEMTS_CDAT);
        break;
      default:
        IDS_HDT_CONSOLE (GNB_TRACE, "%a ERROR: Unsupported CDAT entry found in CDAT table\n", __FUNCTION__);
        return EFI_NOT_FOUND;
    }
  }

  if (RecordCount == 0) {
    IDS_HDT_CONSOLE (GNB_TRACE, "%a: CDAT entry type not found in CDAT table\n", __FUNCTION__);
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}