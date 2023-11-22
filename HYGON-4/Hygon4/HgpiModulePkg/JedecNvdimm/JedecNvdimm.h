/* $NoKeywords:$ */

/**
 * @file
 *
 * JedecNvdimm.h
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  JedecNvdimm
 *
 */
/*
 ******************************************************************************
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

/** @file JedecNvdimm.h
    Header file for JedecNvdimm Driver

**/

#ifndef _JEDEC_NVDIMM_H_
#define _JEDEC_NVDIMM_H_

// ---------------------------------------------------------------------------
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/IoLib.h>

#define EFI_ACPI_TABLE_VERSION_X  (EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0 | EFI_ACPI_TABLE_VERSION_4_0)

#define INPUT_DATA_SIZE   0x100     // TODO: largest size of input?
#define OUTPUT_DATA_SIZE  0x100     // TODO: largest size of output?

#define JEDEC_NVDIMM_SMM_PROTOCOL_GUID \
  { 0x7b7a8cad, 0xab14, 0x45cf, { 0x8e, 0xeb, 0xe2, 0x30, 0xb2, 0xa, 0x12, 0x68 } }

#pragma pack (1)

typedef struct _DSM_ACPI_SMM_DATA {
  UINT32    FunctionIndex;                  // Function Index 0 - 31
  UINT32    InputDataSize;                  // Size of input data
  UINT8     InputData[INPUT_DATA_SIZE];     // Input data (bytes)
  UINT32    OutputDataSize;                 // Size of output data
  UINT8     OutputData[OUTPUT_DATA_SIZE];   // Output data (bytes)
  UINT16    NfitDeviceHandle;               // NFIT Device Handle
  UINT32    NvdimmBitMap;                   // NVDIMM Bitmap
} DSM_ACPI_SMM_DATA;

/**
    Structure to keep SMM timeout data.
 */
typedef struct {
  UINTN    OverFlow;
  UINTN    EndValue;
  UINTN    OldTimerValue;
} SMM_TIMEOUT;

/**
    Defines ACPI timer width in bits
 */
#define NUM_BITS_IN_ACPI_TIMER  32

typedef struct _JEDEC_NVDIMM_SMM_PROTOCOL JEDEC_NVDIMM_SMM_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *READ_MEM_ERROR_COUNTERS)(
  IN CONST JEDEC_NVDIMM_SMM_PROTOCOL  *This,
  IN UINT8                            Socket,
  IN UINT8                            Channel,
  IN UINT8                            Dimm,
  OUT UINT8                           *DramEccErrorCount,
  OUT UINT8                           *DramThresholdEccCount
  );

typedef
EFI_STATUS
(EFIAPI *SET_MEM_ERROR_COUNTERS)(
  IN CONST JEDEC_NVDIMM_SMM_PROTOCOL  *This,
  IN UINT8                            Socket,
  IN UINT8                            Channel,
  IN UINT8                            Dimm,
  IN UINT8                            DramEccErrorCount,
  IN UINT8                            DramThresholdEccCount
  );

typedef
EFI_STATUS
(EFIAPI *INCREMENT_MEM_ERROR_COUNTERS)(
  IN CONST JEDEC_NVDIMM_SMM_PROTOCOL  *This,
  IN UINT8                            Socket,
  IN UINT8                            Channel,
  IN UINT8                            Dimm,
  IN BOOLEAN                          IncDramEccErrorCount,
  IN BOOLEAN                          IncDramThresholdEccCount
  );

typedef
EFI_STATUS
(EFIAPI *PERFORM_NVDIMM_PURGE)(
  IN CONST JEDEC_NVDIMM_SMM_PROTOCOL  *This,
  IN UINT8                            Socket,
  IN UINT8                            Channel,
  IN UINT8                            Dimm
  );

struct _JEDEC_NVDIMM_SMM_PROTOCOL {
  READ_MEM_ERROR_COUNTERS         ReadMemoryErrorCounters;
  SET_MEM_ERROR_COUNTERS          SetMemoryErrorCounters;
  INCREMENT_MEM_ERROR_COUNTERS    IncrementMemErrorCounters;
  PERFORM_NVDIMM_PURGE            PerformNvdimmPurge;
};

#pragma pack ()

EFI_STATUS
EFIAPI
ReadMemoryErrorCounters (
  IN CONST JEDEC_NVDIMM_SMM_PROTOCOL  *This,
  IN UINT8                            Socket,
  IN UINT8                            Channel,
  IN UINT8                            Dimm,
  OUT UINT8                           *DramEccErrorCount,
  OUT UINT8                           *DramThresholdEccCount
  );

EFI_STATUS
EFIAPI
SetMemoryErrorCounters (
  IN CONST JEDEC_NVDIMM_SMM_PROTOCOL  *This,
  IN UINT8                            Socket,
  IN UINT8                            Channel,
  IN UINT8                            Dimm,
  IN UINT8                            DramEccErrorCount,
  IN UINT8                            DramThresholdEccCount
  );

EFI_STATUS
EFIAPI
IncrementMemErrorCounters (
  IN CONST JEDEC_NVDIMM_SMM_PROTOCOL  *This,
  IN UINT8                            Socket,
  IN UINT8                            Channel,
  IN UINT8                            Dimm,
  IN BOOLEAN                          IncDramEccErrorCount,
  IN BOOLEAN                          IncDramThresholdEccCount
  );

EFI_STATUS
EFIAPI
PerformNvdimmPurge (
  IN CONST JEDEC_NVDIMM_SMM_PROTOCOL  *This,
  IN UINT8                            Socket,
  IN UINT8                            Channel,
  IN UINT8                            Dimm
  );

/**
  Initialize timeout for a specified amount of time in uS.
   @param Timeout - Pointer to structure, data modified by function
   @param Time - Timeout value in uS.
   @param Status - Status returned by function.
   @return EFI_STATUS
   @retval EFI_SUCCESS - Always return success.
*/
EFI_STATUS
StartTimeout (
  IN SMM_TIMEOUT *Timeout,
  IN UINTN        Time
  );

/**
  Checks if timeout has expired.
  @param Timeout - Pointer to structure, data modified by function
  @param Status - Status returned by function.
  @return EFI_STATUS
  @retval EFI_SUCCESS - Timer not expired.
  @retval EFI_TIMEOUT - Time expired.
**/
EFI_STATUS
HasItTimedOut (
  IN SMM_TIMEOUT  *Timeout
  );

/**
  This routine delays for specified number of micro seconds.
  @param Usec      Amount of delay (count in 1 microsec)
  @retval VOID
**/
VOID
FixedDelay (
  IN UINTN  Usec
  );

/**
  Generate the NFIT ACPI tables.
  @param TotalNvdimm
  @retval EFI_SUCCESS or EFI_DEVICE_ERROR.
**/
EFI_STATUS
GenerateJedecNvdimmAcpiHyEx (
  OUT UINT16  *TotalNvdimm
  );

/**
  Generate the NFIT ACPI tables.
  @param TotalNvdimm
  @retval EFI_SUCCESS or EFI_DEVICE_ERROR.
**/
EFI_STATUS
GenerateJedecNvdimmAcpiHyGx (
  OUT UINT16  *TotalNvdimm
  );

/**
  Set NVDIMM event notifications
  @param TotalNvdimm
  @retval Status
**/
EFI_STATUS
SetEventNotifications (
  IN UINT16  TotalNvdimm
  );

/**
  InstallDsmSmiHandler installs the SMI handler for DSM exectuion from OS
  @retval EFI_SUCCESS
**/
EFI_STATUS
InstallDsmSmiHandler (
  VOID
  );

/**
  Reset to Factory Default (Function Index 21)
  @param Socket
  @param Channel
  @param Dimm
  @retval DsmStatus
**/
UINT32
ResetToFactoryDefaults (
  IN UINT8 Socket,
  IN UINT8 Channel,
  IN UINT8 Dimm
  );

/**
  Initialize SMBUS access routines
  @retval EFI_SUCCESS
**/
EFI_STATUS
JedecNvdimmSmbusInit (
  VOID
  );

/**
  Read byte from NVDIMM
  @param Socket
  @param Channel
  @param Dimm
  @param Page
  @param Register
  @param *Data
  @param IsSpd
  @retval EFI_SUCCESS
**/
EFI_STATUS
ReadNvdimm (
  UINT8    Socket,
  UINT8    Channel,
  UINT8    Dimm,
  UINT8    Page,
  UINT8    Register,
  UINT8    *Data,
  BOOLEAN  IsSpd
  );

/**
  Write byte to NVDIMM
  @param Socket
  @param Channel
  @param Dimm
  @param Page
  @param Register
  @param *Data
  @param IsSpd
  @retval EFI_SUCCESS
**/
EFI_STATUS
WriteNvdimm (
  UINT8    Socket,
  UINT8    Channel,
  UINT8    Dimm,
  UINT8    Page,
  UINT8    Register,
  UINT8    *Data,
  BOOLEAN  IsSpd
  );

#endif // JEDEC_NVDIMM_H
