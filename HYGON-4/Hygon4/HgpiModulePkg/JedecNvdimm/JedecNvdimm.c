/* $NoKeywords:$ */

/**
 * @file
 *
 * JedecNvdimm.c
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  JedecNvdimm
 *
 */
/*
 *****************************************************************************
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

/** @file JedecNvdimm.c
    JedecNvdimm Driver Implementation

**/

// ---------------------------------------------------------------------------

#include "JedecNvdimm.h"
#include "JedecNvdimmDsm.h"
#include "IndustryStandard/Acpi.h"
#include "Protocol/AcpiSystemDescriptionTable.h"
#include <Library/FchDxeLib.h>
#include <Library/HygonSocBaseLib.h>

// ---------------------------------------------------------------------------

DSM_ACPI_SMM_DATA  *pDsmAcpiSmmData;
EFI_GUID           gJedecNvdimmSmmProtocolGuid = JEDEC_NVDIMM_SMM_PROTOCOL_GUID;

JEDEC_NVDIMM_SMM_PROTOCOL  JedecNvdimmSmmProtocol = {
  ReadMemoryErrorCounters,
  SetMemoryErrorCounters,
  IncrementMemErrorCounters,
  PerformNvdimmPurge
};

/**
 * gNvdimmBitMap
 *
+---------+-----------------------------------------------+-----------------------------------------------+
| Socket  |                      S1                       |                      S0                       |
+---------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
| Channel |  C7 |  C6 |  C5 |  C4 |  C3 |  C2 |  C1 |  C0 |  C7 |  C6 |  C5 |  C4 |  C3 |  C2 |  C1 |  C0 |
+---------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
| Dimm    |D1|D0|D1|D0|D1|D0|D1|D0|D1|D0|D1|D0|D1|D0|D1|D0|D1|D0|D1|D0|D1|D0|D1|D0|D1|D0|D1|D0|D1|D0|D1|D0|
+---------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
| Bit     |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|
+---------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
**/

UINT32  gNvdimmBitMap;

// ======================================================
// ACPI Tables header structure  for IA32
// ======================================================
typedef struct _ACPI_HDR {
  UINT32    Signature;
  UINT32    Length;
  UINT8     Revision;
  UINT8     Checksum;
  UINT8     OemId[6];
  UINT8     OemTblId[8];
  UINT32    OemRev;
  UINT32    CreatorId;
  UINT32    CreatorRev;
} ACPI_HDR;

/*---------------------------------------------------------------------------------------*/
/*
 * Calculate an ACPI Table checksum
 *
 * Computes the checksum and stores the value to the checksum
 * field of the passed in ACPI table's header.
 *
 * @param[in]  Table             ACPI table to checksum
 *
 */
VOID
ChecksumAcpiTable (
  IN OUT   ACPI_HDR *Table
  )
{
  UINT8  *BuffTempPtr;
  UINT8  Checksum;
  UINT32 BufferOffset;

  Table->Checksum = 0;
  Checksum = 0;
  BuffTempPtr = (UINT8 *) Table;
  for (BufferOffset = 0; BufferOffset < Table->Length; BufferOffset++) {
    Checksum = Checksum - *(BuffTempPtr + BufferOffset);
  }

  Table->Checksum = Checksum;
  DEBUG ((DEBUG_INFO, "ACPI Table CheckSum Update, Value is 0x%X\n", Table->Checksum));
}

/**
  Initialize timeout for a specified amount of time in uS.

   @param Timeout - Pointer to structure, data modified by function.
   @param Time - Timeout value in uS.
   @param Status - Status returned by function.

   @return EFI_STATUS
   @retval  EFI_SUCCESS - Always return success.
*/
EFI_STATUS
StartTimeout (
  IN SMM_TIMEOUT *Timeout,
  IN UINTN        Time
  )
{
  UINT64  TicksNeeded;
  UINT64  EndValue;
  UINT16  AcpiBaseAddr;

  //
  // To get FCH ACPI Base Address.
  //
  LibFchGetAcpiPmBase (&AcpiBaseAddr);

  // There are 3.58 ticks per uS.
  TicksNeeded = (Time * 358) / 100;

  // Read ACPI Timer
  Timeout->OldTimerValue = IoRead32 (AcpiBaseAddr + 8);
  EndValue = TicksNeeded + Timeout->OldTimerValue;

  // Calculate Overflow and EndValue from FullEndValue,
  // based on number of bits in ACPI Timer
  Timeout->OverFlow = (UINT32)(EndValue >> NUM_BITS_IN_ACPI_TIMER);
  Timeout->EndValue = ((UINT32)EndValue) & (UINT32)(((UINT64)1 << NUM_BITS_IN_ACPI_TIMER) - 1);

  return EFI_SUCCESS;
}

/**
  Checks if timeout has expired.

   @param Timeout - Pointer to structure, data modified by function.
   @param Status - Status returned by function.

   @return EFI_STATUS
   @retval  EFI_SUCCESS - Timer not expired.
   @retval  EFI_TIMEOUT - Timer expired.
*/
EFI_STATUS
HasItTimedOut (
  IN SMM_TIMEOUT *Timeout
  )
{
  UINTN   TimerValue;
  UINT16  AcpiBaseAddr;

  //
  // To get FCH ACPI Base Address.
  //
  LibFchGetAcpiPmBase (&AcpiBaseAddr);

  // Read ACPI Timer
  TimerValue = IoRead32 (AcpiBaseAddr + 8);

  if (Timeout->OverFlow > 0) {
    // See if the current timer value is less than the previous value.
    // If it is, then the timer had wrapped around.
    if (TimerValue < Timeout->OldTimerValue) {
      --Timeout->OverFlow;
    }

    // Update OldTimerValue
    Timeout->OldTimerValue = TimerValue;
    return EFI_SUCCESS;
  }

  // See if the current timer value is less than the previous value.
  // If it is, then we are done.
  if (TimerValue < Timeout->OldTimerValue) {
    return EFI_TIMEOUT;
  }

  // If we passed the EndValue, we are done.
  if (TimerValue >= Timeout->EndValue) {
    return EFI_TIMEOUT;
  }

  // Update OldTimerValue
  Timeout->OldTimerValue = TimerValue;

  return EFI_SUCCESS;
}

/**
    This routine delays for specified number of micro seconds

    @param Usec      Amount of delay (count in 1 microsec)

    @retval VOID

**/
VOID
FixedDelay (
  IN UINTN  Usec
  )
{
  UINTN   Counter, i;
  UINT32  Data32, PrevData;
  UINT16  AcpiBaseAddr;

  //
  // To get FCH ACPI Base Address.
  //
  LibFchGetAcpiPmBase (&AcpiBaseAddr);

  // There are 3.58 ticks per Usec
  Counter = (Usec * 358) / 100;

  //
  // Call WaitForTick for Counter + 1 ticks to try to guarantee Counter tick
  // periods, thus attempting to ensure Microseconds of stall time.
  //
  if (Counter != 0) {
    PrevData = IoRead32 (AcpiBaseAddr + 8);
    for (i = 0; i < Counter; ) {
      Data32 = IoRead32 (AcpiBaseAddr + 8);
      if (Data32 < PrevData) {
        // Reset if there is a overlap
        PrevData = Data32;
        continue;
      }

      i += (Data32 - PrevData);
      PrevData = Data32;
    }
  }

  return;
}

/**
  Reads the DRAM_ECC_ERROR_COUNT (0x80) and DRAM_THRESHOLD_ECC_COUNT (0x81)
  NVDIMM registers

  @param  This                  JEDEC_NVDIMM_SMM_PROTOCOL instance
  @param  Socket                Socket # of NVDIMM
  @param  Channel               Channel # of NVDIMM
  @param  Dimm                  Dimm # of NVDIMM
  @param  DramEccErrorCount     Number of uncorrectable ECC errors detected
  @param  DramThresholdEccCount Number of correctable ECC threshold exceeded events

  @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
ReadMemoryErrorCounters (
  IN CONST JEDEC_NVDIMM_SMM_PROTOCOL  *This,
  IN UINT8                            Socket,
  IN UINT8                            Channel,
  IN UINT8                            Dimm,
  OUT UINT8                           *DramEccErrorCount,
  OUT UINT8                           *DramThresholdEccCount
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT8       Data;

  Status = ReadNvdimm (Socket, Channel, Dimm, 2, PAGE2_DRAM_ECC_ERROR_COUNT, &Data, FALSE);
  *DramEccErrorCount = Data;
  Status = ReadNvdimm (Socket, Channel, Dimm, 2, PAGE2_DRAM_THRESHOLD_ECC_COUNT, &Data, FALSE);
  *DramThresholdEccCount = Data;

  return Status;
}

/**
  Sets the DRAM_ECC_ERROR_COUNT (0x80) and DRAM_THRESHOLD_ECC_COUNT (0x81)
  NVDIMM registers

  @param  This                  JEDEC_NVDIMM_SMM_PROTOCOL instance
  @param  Socket                Socket # of NVDIMM
  @param  Channel               Channel # of NVDIMM
  @param  Dimm                  Dimm # of NVDIMM
  @param  DramEccErrorCount     Number of uncorrectable ECC errors
  @param  DramThresholdEccCount Number of correctable ECC threshold exceeded events

  @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
SetMemoryErrorCounters (
  IN CONST JEDEC_NVDIMM_SMM_PROTOCOL  *This,
  IN UINT8                            Socket,
  IN UINT8                            Channel,
  IN UINT8                            Dimm,
  IN UINT8                            DramEccErrorCount,
  IN UINT8                            DramThresholdEccCount
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  Status = WriteNvdimm (Socket, Channel, Dimm, 2, PAGE2_DRAM_ECC_ERROR_COUNT, &DramEccErrorCount, FALSE);
  Status = WriteNvdimm (Socket, Channel, Dimm, 2, PAGE2_DRAM_THRESHOLD_ECC_COUNT, &DramThresholdEccCount, FALSE);

  return Status;
}

/**
  Increments the DRAM_ECC_ERROR_COUNT (0x80) and DRAM_THRESHOLD_ECC_COUNT (0x81)
  NVDIMM registers

  @param  This                      JEDEC_NVDIMM_SMM_PROTOCOL instance
  @param  Socket                    Socket # of NVDIMM
  @param  Channel                   Channel # of NVDIMM
  @param  Dimm                      Dimm # of NVDIMM
  @param  IncDramEccErrorCount      Increment the number of uncorrectable ECC errors detected
  @param  IncDramThresholdEccCount  Increment the number of correctable ECC threshold exceeded events

  @retval EFI_SUCCESS or EFI_UNSUPPORTED

**/
EFI_STATUS
EFIAPI
IncrementMemErrorCounters (
  IN CONST JEDEC_NVDIMM_SMM_PROTOCOL  *This,
  IN UINT8                            Socket,
  IN UINT8                            Channel,
  IN UINT8                            Dimm,
  IN BOOLEAN                          IncDramEccErrorCount,
  IN BOOLEAN                          IncDramThresholdEccCount
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT8       Data;

  if (IncDramEccErrorCount) {
    Status = ReadNvdimm (Socket, Channel, Dimm, 2, PAGE2_DRAM_ECC_ERROR_COUNT, &Data, FALSE);
    if (Data < 255) {
      Data  += 1;
      Status = WriteNvdimm (Socket, Channel, Dimm, 2, PAGE2_DRAM_ECC_ERROR_COUNT, &Data, FALSE);
      DEBUG ((EFI_D_ERROR, "IncrementMemErrorCounters: New DramEccErrorCount = %d\n", Data));
    } else {
      DEBUG ((EFI_D_ERROR, "IncrementMemErrorCounters ERROR: DramEccErrorCount at max 255\n"));
      return EFI_UNSUPPORTED;
    }
  }

  if (IncDramThresholdEccCount) {
    Status = ReadNvdimm (Socket, Channel, Dimm, 2, PAGE2_DRAM_THRESHOLD_ECC_COUNT, &Data, FALSE);
    if (Data < 255) {
      Data  += 1;
      Status = WriteNvdimm (Socket, Channel, Dimm, 2, PAGE2_DRAM_THRESHOLD_ECC_COUNT, &Data, FALSE);
      DEBUG ((EFI_D_ERROR, "IncrementMemErrorCounters: New DramThresholdEccCount = %d\n", Data));
    } else {
      DEBUG ((EFI_D_ERROR, "IncrementMemErrorCounters ERROR: DramThresholdEccCount at max 255\n"));
      return EFI_UNSUPPORTED;
    }
  }

  return Status;
}

/**
  Executes the JEDEC Restore Factory Defaults function

  @param  This                  JEDEC_NVDIMM_SMM_PROTOCOL instance
  @param  Socket                Socket # of NVDIMM
  @param  Channel               Channel # of NVDIMM
  @param  Dimm                  Dimm # of NVDIMM

  @retval EFI_SUCCESS or EFI_TIMEOUT or EFI_DEVICE_ERROR

**/
EFI_STATUS
EFIAPI
PerformNvdimmPurge (
  IN CONST JEDEC_NVDIMM_SMM_PROTOCOL  *This,
  IN UINT8                            Socket,
  IN UINT8                            Channel,
  IN UINT8                            Dimm
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32      DsmStatus;
  UINT8       Data;

  DsmStatus = (UINT32)ResetToFactoryDefaults (Socket, Channel, Dimm);
  Status    = ReadNvdimm (Socket, Channel, Dimm, 0, PAGE0_FACTORY_DEFAULT_STATUS0, &Data, FALSE);

  if (DsmStatus == ((1 << 16) | DSM_FUNCTION_ERROR_CODE)) {
    DEBUG ((EFI_D_ERROR, "PerformNvdimmPurge ERROR: Factory Default operation timed out\n"));
    return EFI_TIMEOUT;
  } else if (DsmStatus == DSM_FUNCTION_ERROR_CODE) {
    DEBUG ((EFI_D_ERROR, "PerformNvdimmPurge ERROR: Unable to start Factory Default operation\n"));
    return EFI_DEVICE_ERROR;
  } else if (Data & BIT1) {
    DEBUG ((EFI_D_ERROR, "PerformNvdimmPurge ERROR: Factory Default operation failed\n"));
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((EFI_D_ERROR, "PerformNvdimmPurge SUCCESS!\n"));
  return Status;
}

/**
    JedecNvdimm Driver Entry Point

    @param    ImageHandle
    @param    *SystemTable

    @retval   EFI_SUCCESS or EFI_NOT_FOUND

**/
EFI_STATUS
EFIAPI
JedecNvdimmEntryPoint (
  IN  EFI_HANDLE              ImageHandle,
  IN  EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS              Status;
  EFI_PHYSICAL_ADDRESS    DsmAcpiSmmDataAddress;
  EFI_ACPI_SDT_PROTOCOL   *AcpiSdtProtocol;
  UINTN                   AcpiTblIndex;
  EFI_ACPI_SDT_HEADER     *AcpiTbl = NULL;
  EFI_ACPI_TABLE_VERSION  Version;
  UINTN                   AcpiHandle;
  EFI_PHYSICAL_ADDRESS    Dsdt;
  EFI_PHYSICAL_ADDRESS    XDsdt;
  EFI_PHYSICAL_ADDRESS    DsdtAddr = 0;
  UINT32                  DsdtSize;
  UINT8                   *CurrentPtr;
  UINT32                  *CurrentSig;
  UINT16                  TotalNvdimm = 0;
  EFI_HANDLE              Handle = NULL;
  UINT32                  HygonCpuModel;

  DEBUG ((EFI_D_INFO, "JedecNvdimm Entry\n"));                                                // byo230914 -

  HygonCpuModel = GetHygonSocModel();

  // Initialize NVDIMM bitmap
  gNvdimmBitMap = 0;

  //
  // Initial Smbus variables
  //
  Status = JedecNvdimmSmbusInit ();
  ASSERT (!EFI_ERROR (Status));

  //
  // Allocate ACPI to SMM data buffer
  //
  DsmAcpiSmmDataAddress = 0xFFFFFFFF;
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiACPIMemoryNVS,
                  EFI_SIZE_TO_PAGES (sizeof (DSM_ACPI_SMM_DATA)),
                  &DsmAcpiSmmDataAddress
                  );
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to allocate ACPI NVS memory!\n"));
    return Status;
  }

  pDsmAcpiSmmData = (DSM_ACPI_SMM_DATA *)DsmAcpiSmmDataAddress;
  gBS->SetMem (pDsmAcpiSmmData, sizeof (DSM_ACPI_SMM_DATA), 0);

  //
  // Update ACPI buffer address to point to pDsmAcpiSmmData
  //
  Dsdt  = 0;
  XDsdt = 0;

  Status = gBS->LocateProtocol (&gEfiAcpiSdtProtocolGuid, NULL, &AcpiSdtProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "LocateProtocol EFI_ACPI_SDT_PROTOCOL_GUID failure, return status %x\n", Status));
    return EFI_NOT_AVAILABLE_YET;
  }

  for (AcpiTblIndex = 0; ; AcpiTblIndex++) {
    Status = AcpiSdtProtocol->GetAcpiTable (AcpiTblIndex, &AcpiTbl, &Version, &AcpiHandle);

    if (AcpiTbl->Signature == EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
      // ('F', 'A', 'C', 'P')
      if (Version == EFI_ACPI_TABLE_VERSION_1_0B) {
        Dsdt = (EFI_PHYSICAL_ADDRESS)(((EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE *)AcpiTbl)->Dsdt);
        DEBUG ((EFI_D_INFO, "DSDT pointer = %08lX\n", Dsdt));                                 // byo230914 -
      }

      if ((Version & EFI_ACPI_TABLE_VERSION_X) != 0) {
        Dsdt  = (EFI_PHYSICAL_ADDRESS)(((EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE *)AcpiTbl)->Dsdt);
        XDsdt = (EFI_PHYSICAL_ADDRESS)(((EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE *)AcpiTbl)->XDsdt);
        DEBUG ((EFI_D_INFO, "DSDT pointer = %08lX, X_DSDT pointer = %08lX\n", Dsdt, XDsdt));  // byo230914 -
      }
    }

    // gBS->FreePool((VOID *) AcpiTbl);    // should confirm free or not

    if (Dsdt || XDsdt) {
      break;
    }
  }

  if ((Dsdt != 0) && (XDsdt == 0)) {
    DsdtAddr = Dsdt;
    Status   = EFI_SUCCESS;
  } else if ((Dsdt == 0) && (XDsdt != 0)) {
    DsdtAddr = XDsdt;
    Status   = EFI_SUCCESS;
  } else if((Dsdt != 0) && (XDsdt != 0)) {
    DsdtAddr = XDsdt;
    Status   = EFI_SUCCESS;
  } else {
    DEBUG ((EFI_D_ERROR, "Both DSDT & X_DSDT pointer are not NULL, invalid\n"));
    ASSERT (FALSE);
  }

  DsdtSize   = ((ACPI_HDR *)DsdtAddr)->Length;
  CurrentPtr = (UINT8 *)DsdtAddr;

  // Search for ('N', 'V', 'D', 'M')
  Status = EFI_NOT_FOUND;
  while ((UINTN)CurrentPtr < ((UINTN)DsdtAddr + (UINTN)DsdtSize)) {
    CurrentSig = (UINT32 *)CurrentPtr;
    if (*CurrentSig == SIGNATURE_32 ('N', 'V', 'D', 'M')) {
      *(UINT32 *)CurrentPtr = (UINT32)(UINTN)pDsmAcpiSmmData;
      DEBUG ((DEBUG_INFO, "DSDT(NVDM) CheckSum Update\n"));      // byo230831 +
      ChecksumAcpiTable ((ACPI_HDR *) DsdtAddr);                 // byo230831 +    
      Status = EFI_SUCCESS;
      break;
    }

    CurrentPtr++;
  }
//  DEBUG ((DEBUG_INFO, "DSDT(NVDM) CheckSum Update\n"));        // byo230831 -
//  ChecksumAcpiTable ((ACPI_HDR *) DsdtAddr);                   // byo230831 -
  ASSERT (!EFI_ERROR (Status));
// byo230823 + >>  
  if(EFI_ERROR (Status)){
    return Status;
  }
// byo230823 + <<  

  //
  // Create NFIT tables
  //
  if (HygonCpuModel == HYGON_EX_CPU) {
    Status = GenerateJedecNvdimmAcpiHyEx (&TotalNvdimm);
  } else {
    Status = GenerateJedecNvdimmAcpiHyGx (&TotalNvdimm);
  }
  pDsmAcpiSmmData->NvdimmBitMap = gNvdimmBitMap;
  if (EFI_ERROR (Status) || (TotalNvdimm == 0)) {
    DEBUG ((EFI_D_ERROR, "No NVDIMMs found, JedecNvdimm Exit\n"));
    return EFI_SUCCESS;
  }

  //
  // Install DSM SMI handler
  //
  Status = InstallDsmSmiHandler ();
  ASSERT (!EFI_ERROR (Status));

  //
  // Set NVDIMM event notifications
  //
  Status = SetEventNotifications (TotalNvdimm);
  ASSERT (!EFI_ERROR (Status));

  //
  // Install JEDEC NVDIMM SMM Protocol
  //
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gJedecNvdimmSmmProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &JedecNvdimmSmmProtocol
                    );
  ASSERT (!EFI_ERROR (Status));

  DEBUG ((EFI_D_INFO, "JedecNvdimm Exit\n"));                                                 // byo230914 -

  return Status;
}
