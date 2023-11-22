/*++

Copyright (c) 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name: SelEventInfo.c

Abstract:
  BMC System log.

Revision History:

--*/
#include <SetupBmcCfg.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/IpmiNetFnStorage.h>
#include <IpmiSensorEventData.h>
#include <Ipmi/IpmiNetFnStorageDefinitions.h>
#include "SelEventLog.h"


#define IsLeap(y)   (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))
#define SECSPERMIN  (60)
#define SECSPERHOUR (60 * 60)
#define SECSPERDAY  (24 * SECSPERHOUR)

CHAR16 ByoSoftSensorInfo[100];

/**

    GC_TODO: add routine description

    @param StdSelData     - GC_TODO: add arg description

    @retval STRING        - GC_TODO: add retval description

**/
CHAR16 *
GetByosoftIpmiSensorOffset (
  IN SEL_RECORD          *SelEntry
  )
{
  UINTN                  Size;
  UINT16                 *PromptSting;
  BOOLEAN                IsUpdated = FALSE;

  Size = sizeof (ByoSoftSensorInfo);
  PromptSting = &(ByoSoftSensorInfo[0]);
  SetMem (ByoSoftSensorInfo, Size, 0);

  if ((BYO_RAS_GID == SelEntry->SelData.StdSelData.GeneratorId) || (BYO_RAS_MCA_GID == SelEntry->SelData.StdSelData.GeneratorId)) {
    if (SENSOR_RAS_MEM_ECC_ERROR == SelEntry->SelData.StdSelData.SensorNumber) {
      if (0xA0 == SelEntry->SelData.StdSelData.EvData1) {
        UnicodeSPrint (PromptSting, Size, L"Socket[%d] Channel[%d] Dimm[%d] Rank[%d] correctable Error",
                      (SelEntry->SelData.StdSelData.EvData2 >> 5),
                      (SelEntry->SelData.StdSelData.EvData2 >> 2) & 0x7,
                      (SelEntry->SelData.StdSelData.EvData2 &0x3),
                      SelEntry->SelData.StdSelData.EvData3
                      );
        IsUpdated = TRUE;
      } else if (0xA1 == SelEntry->SelData.StdSelData.EvData1) {
        UnicodeSPrint (PromptSting, Size, L"Socket[%d] Channel[%d] Dimm[%d] Rank[%d] MultiBit Error",
                      (SelEntry->SelData.StdSelData.EvData2 >> 5),
                      (SelEntry->SelData.StdSelData.EvData2 >> 2) & 0x7,
                      (SelEntry->SelData.StdSelData.EvData2 &0x3),
                      SelEntry->SelData.StdSelData.EvData3
                      );
        IsUpdated = TRUE;
      } else if (0xA2 == SelEntry->SelData.StdSelData.EvData1) {
        UnicodeSPrint (PromptSting, Size, L"Socket[%d] Channel[%d] Dimm[%d] Rank[%d] Parity Error",
                      (SelEntry->SelData.StdSelData.EvData2 >> 5),
                      (SelEntry->SelData.StdSelData.EvData2 >> 2) & 0x7,
                      (SelEntry->SelData.StdSelData.EvData2 &0x3),
                      SelEntry->SelData.StdSelData.EvData3
                      );
        IsUpdated = TRUE;
      } else if (0xA3 == SelEntry->SelData.StdSelData.EvData1) {
        UnicodeSPrint (PromptSting, Size, L"Socket[%d] Channel[%d] Dimm[%d] Rank[%d] Scrub Uncorrectable Error",
                      (SelEntry->SelData.StdSelData.EvData2 >> 5),
                      (SelEntry->SelData.StdSelData.EvData2 >> 2) & 0x7,
                      (SelEntry->SelData.StdSelData.EvData2 &0x3),
                      SelEntry->SelData.StdSelData.EvData3
                      );
        IsUpdated = TRUE;
      }
    } else if (SENSOR_RAS_MEM_MISC_ERROR == SelEntry->SelData.StdSelData.SensorNumber) {
      UnicodeSPrint (PromptSting, Size, L"Socket[%d] Channel[%d] Dimm[%d] Rank[%d]  Error",
                    (SelEntry->SelData.StdSelData.EvData2 >> 5),
                    (SelEntry->SelData.StdSelData.EvData2 >> 2) & 0x7,
                    (SelEntry->SelData.StdSelData.EvData2 &0x3),
                    SelEntry->SelData.StdSelData.EvData3
                    );
      IsUpdated = TRUE;
    } else if (SENSOR_RAS_MEM_MIRROR_ERROR == SelEntry->SelData.StdSelData.SensorNumber) {
      UnicodeSPrint (PromptSting, Size, L"Socket[%d] Channel[%d] Dimm[%d] Rank[%d] redundancy degraded",
                    (SelEntry->SelData.StdSelData.EvData2 >> 5),
                    (SelEntry->SelData.StdSelData.EvData2 >> 2) & 0x7,
                    (SelEntry->SelData.StdSelData.EvData2 &0x3),
                    SelEntry->SelData.StdSelData.EvData3
                    );
      IsUpdated = TRUE;
    } else if (SENSOR_RAS_PCI_LEGACY_ERROR == SelEntry->SelData.StdSelData.SensorNumber) {
       if (0xA4 == SelEntry->SelData.StdSelData.EvData1){
         UnicodeSPrint (PromptSting, Size, L"Pci Bus[0x%x] Dev[%d] Fun[%d] PERR",
                       (SelEntry->SelData.StdSelData.EvData2),
                       (SelEntry->SelData.StdSelData.EvData3 >> 3),
                       (SelEntry->SelData.StdSelData.EvData3) & 0x7
                       );
         IsUpdated = TRUE;
       } else if (0xA5 == SelEntry->SelData.StdSelData.EvData1) {
         UnicodeSPrint (PromptSting, Size, L"Pci Bus[0x%x] Dev[%d] Fun[%d] SERR",
                       (SelEntry->SelData.StdSelData.EvData2),
                       (SelEntry->SelData.StdSelData.EvData3 >> 3),
                       (SelEntry->SelData.StdSelData.EvData3) & 0x7
                       );
         IsUpdated = TRUE;
       }
    } else if (SENSOR_RAS_PCIBRIDGE_LEGACY_ERROR == SelEntry->SelData.StdSelData.SensorNumber) {
       if (0xA4 == SelEntry->SelData.StdSelData.EvData1){
         UnicodeSPrint (PromptSting, Size, L"Pci Bus[0x%x] Dev[%d] Fun[%d] PERR",
                       (SelEntry->SelData.StdSelData.EvData2),
                       (SelEntry->SelData.StdSelData.EvData3 >> 3),
                       (SelEntry->SelData.StdSelData.EvData3) & 0x7
                       );
         IsUpdated = TRUE;
       } else if (0xA5 == SelEntry->SelData.StdSelData.EvData1) {
         UnicodeSPrint (PromptSting, Size, L"Pci Bus[0x%x] Dev[%d] Fun[%d] SERR",
                       (SelEntry->SelData.StdSelData.EvData2),
                       (SelEntry->SelData.StdSelData.EvData3 >> 3),
                       (SelEntry->SelData.StdSelData.EvData3) & 0x7
                       );
         IsUpdated = TRUE;
       }
    } else if (SENSOR_RAS_PCIE_AER_CORRE_ERROR == SelEntry->SelData.StdSelData.SensorNumber) {
      UnicodeSPrint (PromptSting, Size, L"Pci Bus[0x%x] Dev[%d] Fun[%d] AER Correctable Error",
                    (SelEntry->SelData.StdSelData.EvData2),
                    (SelEntry->SelData.StdSelData.EvData3 >> 3),
                    (SelEntry->SelData.StdSelData.EvData3) & 0x7
                    );
      IsUpdated = TRUE;

      switch (SelEntry->SelData.StdSelData.EvData1) {
      case 0xA0:
        StrCatS (PromptSting, Size/sizeof(CHAR16), L"\nReceiver Error Status");
      break;
      case 0xA1:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nBad TPL Status");
      break;
      case 0xA2:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nBad DLLP Status");
      break;
      case 0xA3:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nREPLAY_NUM Rollover Status");
      break;
      case 0xA4:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nReplay Timer Timeout Status");
      break;
      case 0xA5:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nAdvisory Non-Fatal Error Status");
      break;
      case 0xA6:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nCorrected Internal Error Status");
      break;
      case 0xA7:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nHeader Log Overflow Status");
      break;
      default:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nUndefined");
      break;
      }
    } else if (SENSOR_RAS_PCIE_AER_UNCORRE_ERROR == SelEntry->SelData.StdSelData.SensorNumber) {
      UnicodeSPrint (PromptSting, Size, L"Pci Bus[0x%x] Dev[%d] Fun[%d] AER Uncorrectable Error",
                    (SelEntry->SelData.StdSelData.EvData2),
                    (SelEntry->SelData.StdSelData.EvData3 >> 3),
                    (SelEntry->SelData.StdSelData.EvData3) & 0x7
                    );
      IsUpdated = TRUE;
      switch (SelEntry->SelData.StdSelData.EvData1) {
      case 0xA0:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nData Link Protocol Error Status");
      break;
      case 0xA1:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nSurprise Down Error Status");
      break;
      case 0xA2:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nPoisoned TLP Status");
      break;
      case 0xA3:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nFlow Control Protocol Error Status");
      break;
      case 0xA4:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nCompletion Timeout Status");
      break;
      case 0xA5:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nCompleter Abort Status");
      break;
      case 0xA6:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nUnexpected Completion Status");
      break;
      case 0xA7:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nReceiver Overflow Status");
      break;
      case 0xA8:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nMalformed TLP Status");
      break;
      case 0xA9:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nECRC Error Status");
      break;
      case 0xAA:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nUnsupported Request Error Status");
      break;
      case 0xAB:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nACS Violation Status");
      break;
      case 0xAC:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nUncorrectable Internal Error Status");
      break;
      case 0xAD:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nMC Blocked TLP Status");
      break;
      case 0xAE:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nAtomicOp Egress Blocked Status");
      break;
      case 0xAF:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nTLP Prefix Blocked Error Status");
      default:
        StrCatS (PromptSting, Size/sizeof(CHAR16),  L"\nUndefined");
      break;
      }
    } else if (SENSOR_RAS_PCIE_ERROR == SelEntry->SelData.StdSelData.SensorNumber) {
       UnicodeSPrint (PromptSting, Size, L"Pcie Bus[0x%x] Dev[%d] Fun[%d] Error",
                     (SelEntry->SelData.StdSelData.EvData2),
                     (SelEntry->SelData.StdSelData.EvData3 >> 3),
                     (SelEntry->SelData.StdSelData.EvData3) & 0x7
                     );
       IsUpdated = TRUE;
    }
  }

  if (IsUpdated) {
    return PromptSting;
  } else {
    return NULL;
  }
}

//
//  The arrays give the cumulative number of days up to the first of the
//  month number used as the index (1 -> 12) for regular and leap years.
//  The value at index 13 is for the whole year.
//
UINTN CumulativeDays[2][14] = {
  {
    0,
    0,
    31,
    31 + 28,
    31 + 28 + 31,
    31 + 28 + 31 + 30,
    31 + 28 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31
  },
  {
    0,
    0,
    31,
    31 + 29,
    31 + 29 + 31,
    31 + 29 + 31 + 30,
    31 + 29 + 31 + 30 + 31,
    31 + 29 + 31 + 30 + 31 + 30,
    31 + 29 + 31 + 30 + 31 + 30 + 31,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31 
  }
};

VOID
ConvertTimeToYMDHMS (
  IN UINTN         Time,
  IN OUT UINT8     *Second,
  IN OUT UINT8     *Minute,
  IN OUT UINT8     *Hour,
  IN OUT UINT8     *Day,
  IN OUT UINT8     *Month,
  IN OUT UINT16    *YearK
  )
{
  UINTN      DayNo;
  UINTN      DayRemainder;
  UINTN      Year;
  UINTN      YearNo;
  UINTN      TotalDays;
  UINT16     MonthNo;
  
  DayNo        = Time / SECSPERDAY;
  DayRemainder = Time % SECSPERDAY;
  
  *Second  = (UINT8) (DayRemainder % SECSPERMIN);
  *Minute  = (UINT8) ((DayRemainder % SECSPERHOUR) / SECSPERMIN);
  *Hour = (UINT8) (DayRemainder / SECSPERHOUR);

  for (Year = 1970, YearNo = 0; DayNo > 0; Year++) {
    TotalDays = IsLeap (Year) ? 366 : 365;
    if (DayNo >= TotalDays) {
      DayNo = DayNo - TotalDays;
      YearNo++;
    } else {
      break;
    }
  }

  *YearK = (UINT16) (YearNo + 1970);

  for (MonthNo = 12; MonthNo > 1; MonthNo--) {
    if (DayNo >= CumulativeDays[IsLeap(Year)][MonthNo]) {
      DayNo = DayNo - CumulativeDays[IsLeap(Year)][MonthNo];
      break;
    }
  }

  *Month  = (UINT8) MonthNo;    //BYOSOFT_OVERRIDE
  *Day = (UINT8) DayNo + 1;
}
/**

    GC_TODO: add routine description

    @param StdSelData     - GC_TODO: add arg description

    @retval *Ptr          - GC_TODO: add arg description
**/
CHAR16 *
FindType6FEventOffset (
  IN  STD_SEL_RECORD_DATA   *StdSelData
  )
{
  UINTN         i;
  BOOLEAN       IsFound;

  i = 0;
  IsFound = FALSE;
  while ((0 != Type6FOffset[i].SensorType) && (NULL != Type6FOffset[i].EventDesc)) {
    if ((StdSelData->SensorType == Type6FOffset[i].SensorType) &&
        ((StdSelData->EvData1 & 0x0f) == Type6FOffset[i].Offset)) {

         //
         //Assume we found first, then continue to check event data2 and data3.
         //
         IsFound = TRUE;
         //
         //If required to check Event Data2, check Data2.
         //
         if (0xff != Type6FOffset[i].EvData2) {
            if (StdSelData->EvData2  != Type6FOffset[i].EvData2) {
              IsFound = FALSE;
            }
         }
         //
         //If required to check Event Data3, check Data3.
         //
         if (0xff != Type6FOffset[i].EvData3) {
            if (StdSelData->EvData3  != Type6FOffset[i].EvData3) {
              IsFound = FALSE;
            }
         }
         if (IsFound) {
          return Type6FOffset[i].EventDesc;
         }
    }
    i++;
  }
  return NULL;
}
/**

    GC_TODO: add routine description

    @param StdSelData     - GC_TODO: add arg description
    @param PromptSting     - GC_TODO: add arg description
    @param HelpSting     - GC_TODO: add arg description

    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description
    @retval EFI_NOT_FOUND         - GC_TODO: add retval description
    @retval Status                - GC_TODO: add retval description

**/
EFI_STATUS
ConvertStdLogInfo (
  IN SEL_RECORD          *SelEntry,
  IN OUT CHAR16          **PromptSting,
  IN OUT CHAR16          **HelpSting
  )
{
  UINT8     Second;
  UINT8     Minute;
  UINT8     Hour;
  UINT8     Day;
  UINT8     Month;
  UINT16    Year;
  CHAR16    *pOffset;
  CHAR16    Owner[] = L"IPMBc01a48";
  CHAR16    *pSensor = L"Undefined";

  if ((NULL == SelEntry) ||
     (NULL == PromptSting) ||
     (NULL == HelpSting)) {
     return EFI_INVALID_PARAMETER;
  }

  *PromptSting = AllocateZeroPool (0x200);
  if (NULL == *PromptSting) {
    return EFI_OUT_OF_RESOURCES;
  }

  ConvertTimeToYMDHMS (SelEntry->SelData.StdSelData.TimeStamp, &Second, &Minute, &Hour, &Day, &Month, &Year);

  SetMem (Owner, sizeof (Owner), 0);
  if (SelEntry->SelData.StdSelData.GeneratorId&0x01)  {
     if ((SelEntry->SelData.StdSelData.GeneratorId >> 1) < 0x10) {
       CopyMem (Owner, L"BIOS", sizeof (L"BIOS"));
     } else if ((SelEntry->SelData.StdSelData.GeneratorId >> 1) < 0x20) {
       CopyMem (Owner, L"SMI", sizeof (L"SMI"));
     } else if ((SelEntry->SelData.StdSelData.GeneratorId >> 1) < 0x30) {
       CopyMem (Owner, L"SMS", sizeof (L"SMS"));
     } else if ((SelEntry->SelData.StdSelData.GeneratorId >> 1) < 0x40) {
       CopyMem (Owner, L"OEM", sizeof (L"OEM"));
     } else if ((SelEntry->SelData.StdSelData.GeneratorId >> 1)< 0x48) {
       CopyMem (Owner, L"Remote", sizeof (L"Remote"));
     }
  } else {
    //
    //T.O.D.O we need know IPMB owner name from SDR.
    // Currently, just show "Channel number and IPMB address"
    //
    if (0x10 == ((SelEntry->SelData.StdSelData.GeneratorId >> 1) & 0x7f)) {
      CopyMem (Owner, L"BMC", sizeof (L"BMC"));
    } else {
      UnicodeSPrint (Owner, sizeof (Owner), L"IPMBc%da%2x", ((SelEntry->SelData.StdSelData.GeneratorId >> 12) & 0x3),((SelEntry->SelData.StdSelData.GeneratorId >> 1) & 0x7f));
    }
  }
  if (SelEntry->SelData.StdSelData.SensorType < 45) {
    pSensor = SensorType[SelEntry->SelData.StdSelData.SensorType];
  }
  //
  //PromptSting format
  //2017/6/10 18:16:00 Bmc s#13 Offset#5 Fan assert
  //
  UnicodeSPrint (*PromptSting, 0x200, L"%d/%02d/%02d %02d:%02d:%02d %s S#%d O#%d %s %s",
                 Year,Month,Day,Hour,Minute,Second,
                 Owner,
                 SelEntry->SelData.StdSelData.SensorNumber,
                 (SelEntry->SelData.StdSelData.EvData1&0x0f),
                 pSensor,
                 (SelEntry->SelData.StdSelData.EventDir?L"Deassert":L"Assert")
                );

  *HelpSting = AllocateZeroPool (0x400);
  if (NULL == *HelpSting) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  //Check whether this is Byosoft definitions first.
  //
  pOffset = GetByosoftIpmiSensorOffset (SelEntry);
  if (NULL == pOffset) {
    pOffset = L"Undefined";
    //
    //If we can not find ByosoftIpmiSensor Info, then get from IPMI definitions.
    //
    if (0x01 == SelEntry->SelData.StdSelData.EventType) {
      if ((SelEntry->SelData.StdSelData.EvData1&0x0f) <= 0x0B) {
        pOffset = Type1Offset[SelEntry->SelData.StdSelData.EvData1&0x0f];
      }
    } else if ((SelEntry->SelData.StdSelData.EventType >= 0x02) && (SelEntry->SelData.StdSelData.EventType <= 0x0c)) {
     if ((0x02 == SelEntry->SelData.StdSelData.EventType) && ((SelEntry->SelData.StdSelData.EvData1&0x0f) <= 0x02)) {
       pOffset = Type2Offset[SelEntry->SelData.StdSelData.EvData1&0x0f];
     }
     if ((0x03 == SelEntry->SelData.StdSelData.EventType) && ((SelEntry->SelData.StdSelData.EvData1&0x0f) <= 0x01)) {
       pOffset = Type3Offset[SelEntry->SelData.StdSelData.EvData1&0x0f];
     }
     if ((0x04 == SelEntry->SelData.StdSelData.EventType) && ((SelEntry->SelData.StdSelData.EvData1&0x0f) <= 0x01)) {
       pOffset = Type4Offset[SelEntry->SelData.StdSelData.EvData1&0x0f];
     }
     if ((0x05 == SelEntry->SelData.StdSelData.EventType) && ((SelEntry->SelData.StdSelData.EvData1&0x0f) <= 0x01)) {
       pOffset = Type5Offset[SelEntry->SelData.StdSelData.EvData1&0x0f];
     }
     if ((0x06 == SelEntry->SelData.StdSelData.EventType) && ((SelEntry->SelData.StdSelData.EvData1&0x0f) <= 0x01)) {
       pOffset = Type6Offset[SelEntry->SelData.StdSelData.EvData1&0x0f];
     }
     if ((0x07 == SelEntry->SelData.StdSelData.EventType) && ((SelEntry->SelData.StdSelData.EvData1&0x0f) <= 0x08)) {
       pOffset = Type7Offset[SelEntry->SelData.StdSelData.EvData1&0x0f];
     }
     if ((0x08 == SelEntry->SelData.StdSelData.EventType) && ((SelEntry->SelData.StdSelData.EvData1&0x0f) <= 0x01)) {
       pOffset = Type8Offset[SelEntry->SelData.StdSelData.EvData1&0x0f];
     }
     if ((0x09 == SelEntry->SelData.StdSelData.EventType) && ((SelEntry->SelData.StdSelData.EvData1&0x0f) <= 0x01)) {
       pOffset = Type9Offset[SelEntry->SelData.StdSelData.EvData1&0x0f];
     }
     if ((0x0a == SelEntry->SelData.StdSelData.EventType) && ((SelEntry->SelData.StdSelData.EvData1&0x0f) <= 0x08)) {
       pOffset = TypeAOffset[SelEntry->SelData.StdSelData.EvData1&0x0f];
     }
     if ((0x0b == SelEntry->SelData.StdSelData.EventType) && ((SelEntry->SelData.StdSelData.EvData1&0x0f) <= 0x07)) {
       pOffset = TypeBOffset[SelEntry->SelData.StdSelData.EvData1&0x0f];
     }
     if ((0x0c == SelEntry->SelData.StdSelData.EventType) && ((SelEntry->SelData.StdSelData.EvData1&0x0f) <= 0x03)) {
       pOffset = TypeCOffset[SelEntry->SelData.StdSelData.EvData1&0x0f];
     }
    } else if (0x6f == SelEntry->SelData.StdSelData.EventType) {
      pOffset = FindType6FEventOffset (&(SelEntry->SelData.StdSelData));
      if (NULL == pOffset) {
        pOffset = L"Undefined";
      }
    } else {
        pOffset = L"Oem defined event offset";
    }
  }
  //
  //HelpSting format
  //Event Details:
  //lower Critical going low
  //Generator ID:  0x..
  //Sensor Number:..
  //Senor Type:  ..
  //Event Type:  ..
  //Event Dir: ..
  //Event  Data1:   0x..
  //Event  Data2:   0x..
  //Event  Data3:   0x..
  //
  //***SensorName: T.O.D.O, need parse SDR.
  //
  UnicodeSPrint (*HelpSting, 0x200, L"Event Details:%s\nGenerator ID:0x%x\nSensor Number:%d\nSensor Type:0x%x\nEvent Type:0x%x\nEvent Dir:%d\nEvent Data1:0x%x\nEvent Data2:0x%x\nEvent Data3:0x%x\n",
                 pOffset,
                 SelEntry->SelData.StdSelData.GeneratorId,
                 SelEntry->SelData.StdSelData.SensorNumber,
                 SelEntry->SelData.StdSelData.SensorType,
                 SelEntry->SelData.StdSelData.EventType,
                 SelEntry->SelData.StdSelData.EventDir,
                 SelEntry->SelData.StdSelData.EvData1,
                 SelEntry->SelData.StdSelData.EvData2,
                 SelEntry->SelData.StdSelData.EvData3
                );
  SEL_VIEWER_DEBUG((EFI_D_INFO, "%s\n%s\n", *PromptSting,*HelpSting));
  return EFI_SUCCESS;
}

/**

    GC_TODO: add routine description

    @param StdSelData     - GC_TODO: add arg description
    @param PromptSting     - GC_TODO: add arg description
    @param HelpSting     - GC_TODO: add arg description

    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description
    @retval EFI_NOT_FOUND         - GC_TODO: add retval description
    @retval Status                - GC_TODO: add retval description

**/
EFI_STATUS
ConvertOemLogInfo (
  IN SEL_RECORD          *SelEntry,
  IN OUT CHAR16          **PromptSting,
  IN OUT CHAR16          **HelpSting
  )
{
  UINT8     Second;
  UINT8     Minute;
  UINT8     Hour;
  UINT8     Day;
  UINT8     Month;
  UINT16    Year;

  if ((NULL == SelEntry) ||
     (NULL == PromptSting) ||
     (NULL == HelpSting)) {
     return EFI_INVALID_PARAMETER;
  }

  *PromptSting = AllocateZeroPool (0x200);
  if (NULL == *PromptSting) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (SelEntry->RecordType < 0xe0){
    ConvertTimeToYMDHMS (SelEntry->SelData.OemSelData.TimeStamp, &Second, &Minute, &Hour, &Day, &Month, &Year);
    //
    //PromptSting format
    //2017/6/10 18:16:00 Oem(Manufacture Id 0xxxxx) Event
    //
    UnicodeSPrint (*PromptSting, 0x200, L"%d/%02d/%02d %02d:%02d:%02d Oem(MID%x%x%x) Event",
                   Year,Month,Day,Hour,Minute,Second,
                   SelEntry->SelData.OemSelData.ManufactureId[2],
                   SelEntry->SelData.OemSelData.ManufactureId[1],
                   SelEntry->SelData.OemSelData.ManufactureId[0]
                  );
  } else if (SelEntry->RecordType == BYO_OEM_SEL_DEF) {
    //
    //PromptSting format
    //Oem(Manufacture ID unknown) Event
    //
    // UnicodeSPrint (*PromptSting, 0x65, L"MCA Bank[%d].address[0x]", );
    //
      switch (SelEntry->SelData.Msr64SelData.BankReg) {
      case 1:
        UnicodeSPrint (*PromptSting, 0x200, L"ApicId[%d] MCA Bank[%d].Status[0x%08lx]", SelEntry->SelData.Msr64SelData.ApicId, SelEntry->SelData.Msr64SelData.BankNum, SelEntry->SelData.Msr64SelData.MsrData);
        break;
      case 2:
        UnicodeSPrint (*PromptSting, 0x200, L"ApicId[%d] MCA Bank[%d].Address[0x%08lx]", SelEntry->SelData.Msr64SelData.ApicId, SelEntry->SelData.Msr64SelData.BankNum, SelEntry->SelData.Msr64SelData.MsrData);
        break;
      case 3:
       UnicodeSPrint (*PromptSting, 0x200, L"ApicId[%d] MCA Bank[%d].Misc[0x%08lx]", SelEntry->SelData.Msr64SelData.ApicId, SelEntry->SelData.Msr64SelData.BankNum, SelEntry->SelData.Msr64SelData.MsrData);
        break;
      }
  } else {
    //
    //PromptSting format
    //Oem(Manufacture ID unknown) Event
    //
    UnicodeSPrint (*PromptSting, 0x200, L"Oem SEL log without timestamp");
  }

  *HelpSting = AllocateZeroPool (0x400);
  if (NULL == *HelpSting) {
    return EFI_OUT_OF_RESOURCES;
  }
  if (SelEntry->RecordType < 0xe0){
    //
    //HelpSting format
    //Event Details:
    //Event  Data1:   0x..
    //Event  Data2:   0x..
    //Event  Data3:   0x..
    //Event  Data4:   0x..
    //Event  Data5:   0x..
    //Event  Data6:   0x..
    //
    UnicodeSPrint (*HelpSting, 0x400, L"Event Details:\nRecord Type:0x%x\nEvent Data1:0x%x\nEvent Data2:0x%x\nEvent Data3:0x%x\nEvent Data4:0x%x\nEvent Data5:0x%x\nEvent Data6:0x%x",
                   SelEntry->RecordType,
                   SelEntry->SelData.OemSelData.OemData[0],
                   SelEntry->SelData.OemSelData.OemData[1],
                   SelEntry->SelData.OemSelData.OemData[2],
                   SelEntry->SelData.OemSelData.OemData[3],
                   SelEntry->SelData.OemSelData.OemData[4],
                   SelEntry->SelData.OemSelData.OemData[5]
                  );
  } else if (SelEntry->RecordType == BYO_OEM_SEL_DEF) {
    UnicodeSPrint (*HelpSting, 0x400, L"Event Details:\nApicId:0x%x\nBank Number:%d\nData:0x%08lx\n",
                 SelEntry->SelData.Msr64SelData.ApicId,
                 SelEntry->SelData.Msr64SelData.BankNum,
                 SelEntry->SelData.Msr64SelData.MsrData
                );
  } else {
    //
    //HelpSting format
    //Event Details:
    //Event  Data1:   0x..
    //Event  Data2:   0x..
    //Event  Data3:   0x..
    //Event  Data4:   0x..
    //Event  Data5:   0x..
    //Event  Data6:   0x..
    //Event  Data7:   0x..
    //Event  Data8:   0x..
    //Event  Data9:   0x..
    //Event  Data10:  0x..
    //Event  Data11:  0x..
    //Event  Data12:  0x..
    //Event  Data13:  0x..
    //
    UnicodeSPrint (*HelpSting, 0x400, L"Event Details:\nRecord Type:0x%x\nEvent Data1:0x%x\nEvent Data2:0x%x\nEvent Data3:0x%x\nEvent Data4:0x%x\nEvent Data5:0x%x\nEvent Data6:0x%x\nEvent Data7:0x%x\nEvent Data8:0x%x\nEvent Data9:0x%x\nEvent Data10:0x%x\nEvent Data11:0x%x\nEvent Data12:0x%x\nEvent Data13:0x%x",
                   SelEntry->RecordType,
                   SelEntry->SelData.OemSelData.OemData[0],
                   SelEntry->SelData.OemSelData.OemData[1],
                   SelEntry->SelData.OemSelData.OemData[2],
                   SelEntry->SelData.OemSelData.OemData[3],
                   SelEntry->SelData.OemSelData.OemData[4],
                   SelEntry->SelData.OemSelData.OemData[5],
                   SelEntry->SelData.OemSelData.OemData[6],
                   SelEntry->SelData.OemSelData.OemData[7],
                   SelEntry->SelData.OemSelData.OemData[8],
                   SelEntry->SelData.OemSelData.OemData[9],
                   SelEntry->SelData.OemSelData.OemData[10],
                   SelEntry->SelData.OemSelData.OemData[11],
                   SelEntry->SelData.OemSelData.OemData[12],
                   SelEntry->SelData.OemSelData.OemData[13]
                  );
  }

  SEL_VIEWER_DEBUG((EFI_D_INFO, "%s\n%s\n", *PromptSting,*HelpSting));
  return EFI_SUCCESS;
}
/**

    GC_TODO: add routine description

    @param StdSelData     - GC_TODO: add arg description
    @param PromptSting     - GC_TODO: add arg description
    @param HelpSting     - GC_TODO: add arg description

    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description
    @retval EFI_NOT_FOUND         - GC_TODO: add retval description
    @retval Status                - GC_TODO: add retval description

**/
EFI_STATUS
ConvertSensorEventLogInfo (
  IN SEL_RECORD          *SelEntry,
  IN OUT CHAR16          **PromptSting,
  IN OUT CHAR16          **HelpSting
  )
{
  if ((NULL == SelEntry) ||
     (NULL == PromptSting) ||
     (NULL == HelpSting)) {
     return EFI_INVALID_PARAMETER;
  }
  //DEBUG ((EFI_D_ERROR, "ConvertSensorEventLogInfo, RecordId :0x%x, RecordType :0x%x.\n", SelEntry->RecordId, SelEntry->RecordType));

  if (SelEntry->RecordType < 0xc0) {
    ConvertStdLogInfo (SelEntry, PromptSting, HelpSting);
  } else {
    ConvertOemLogInfo (SelEntry, PromptSting, HelpSting);
  }

  return EFI_SUCCESS;
}
