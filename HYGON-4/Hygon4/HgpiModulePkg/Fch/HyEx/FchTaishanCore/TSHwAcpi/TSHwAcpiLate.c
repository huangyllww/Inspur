/* $NoKeywords:$ */

/**
 * @file
 *
 * Config Fch HwAcpi controller
 *
 * Init HwAcpi Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: FCH
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "FchCommonCfg.h"
#include "Filecode.h"
#include "FchAoacLinkListData.h"
#define FILECODE  PROC_FCH_TAISHAN_TSHWACPI_TSHWACPILATE_FILECODE

#define HYGON_CPUID_APICID_LPC_BID  0x00000001ul    // Local APIC ID, Logical Processor Count, Brand ID

extern VOID
HpetInit                          (
  IN VOID  *FchDataPtr
  );

extern VOID
GcpuRelatedSetting              (
  IN VOID  *FchDataPtr
  );

extern VOID
StressResetModeLate               (
  IN VOID  *FchDataPtr
  );

VOID
FchInitLateHwAcpiP (
  IN  VOID     *FchDataPtr
  );

/**
 * FchInitLateHwAcpi - Prepare HwAcpi controller to boot to OS.
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateHwAcpi (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  HGPI_TESTPOINT (TpFchInitLateHwAcpi, NULL);

  GcpuRelatedSetting (LocalCfgPtr);

  // Mt C1E Enable
  MtC1eEnable (LocalCfgPtr);

  if (LocalCfgPtr->Gpp.SerialDebugBusEnable == TRUE ) {
    RwMem (ACPI_MMIO_BASE + SERIAL_DEBUG_BASE +  FCH_SDB_REG00, AccessWidth8, 0xFF, 0x05);
  }

  StressResetModeLate (LocalCfgPtr);
  SbSleepTrapControl (TRUE);

  FchInternalDeviceIrqInit (FchDataPtr);

  FchEventInitUsbGpe (FchDataPtr);

  FchI2cUartInitLate (FchDataPtr);
}

VOID
FchInitLateHwAcpiP (
  IN  VOID     *FchDataPtr
  )
{
  UINT16          FchSmiData;
  UINT8           SwSmiCmdAddress;
  FCH_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;

  FchSmiData = (FCH_SMI_DATA_POST_LATE << 8) | FCH_AOAC_SW_VALUE;
  SwSmiCmdAddress = ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A);
  LibHygonIoWrite (AccessWidth16, SwSmiCmdAddress, &FchSmiData, LocalCfgPtr->StdHeader);
}

/**
 * FchI2cUartInitLate - Config Fch AMBA I2C Uart init Late
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchI2cUartInitLate (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;
  UINTN                iUartLegacyChannel;
  UINTN                i;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;
  // UART Legacy IO Enable Support
  if ( LocalCfgPtr->FchRunTime.Al2AhbLegacyUartIoEnable ) {
    ACPIMMIO16 (FCH_AL2AHBx20_LEGACY_UART_IO_ENABLE) = LocalCfgPtr->FchRunTime.Al2AhbLegacyUartIoEnable;

    // 1. Write 0 to 0xFEDC9008[0], 0xFEDCA008[0], 0xFEDCE008[0] or 0xFEDCF008[0] depending on which UART controller will be used for Legacy UART
    // 2. Read 0xFEDC9008h[7:6], 0xFEDCA008[7:6], 0xFEDCE008[7:6] or 0xFEDCF008[7:6] to make sure these two bits are 2'b00
    // 3. If those two bits are 2'b11, make sure MMIO access is enabled and UART AOAC registers are not programmed on D3.
    // 4. Make sure IO Trap WA for UART frequency is removed.
    // 5. Set SMBus PCI_Config 0xFC[28] (UART 0), 0xFC[29], 0xFC[30] or 0xFC[31] to 1 depending on which UART controller willbe used as Legacy UART.
    for (i = 0; i < 4; i++) {
      if ( LocalCfgPtr->FchRunTime.Al2AhbLegacyUartIoEnable & (BIT0 << i)) {
        iUartLegacyChannel = (LocalCfgPtr->FchRunTime.Al2AhbLegacyUartIoEnable >> (8 + i * 2)) & 3;
        switch  (iUartLegacyChannel) {
          case 0:
            // ACPIMMIO16 (0xFEDC9008) &= 0xFF3E;
            PciAndThenOr32 ((SMBUS_BUS_DEV_FUN << 12) + 0xFC, ~(UINT32)(BIT28), (UINT32)(BIT28));
            break;
          case 1:
            // ACPIMMIO16 (0xFEDCA008) &= 0xFF3E;
            PciAndThenOr32 ((SMBUS_BUS_DEV_FUN << 12) + 0xFC, ~(UINT32)(BIT29), (UINT32)(BIT29));
            break;
          case 2:
            // ACPIMMIO16 (0xFEDCE008) &= 0xFF3E;
            PciAndThenOr32 ((SMBUS_BUS_DEV_FUN << 12) + 0xFC, ~(UINT32)(BIT30), (UINT32)(BIT30));
            break;
          case 3:
            // ACPIMMIO16 (0xFEDCF008) &= 0xFF3E;
            PciAndThenOr32 ((SMBUS_BUS_DEV_FUN << 12) + 0xFC, ~(UINT32)(BIT31), (UINT32)(BIT31));
            break;
        }
      }
    }
    SerialPortInitialize();
  }
}
