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
#define FILECODE  PROC_FCH_TAISHAN_TSHWACPI_TSHWACPIENV_FILECODE

extern VOID
ProgramEnvPFchAcpiMmio (
  IN VOID  *FchDataPtr
  );

extern VOID
ProgramFchEnvHwAcpiPciReg (
  IN VOID  *FchDataPtr
  );

extern VOID
ProgramSpecificFchInitEnvAcpiMmio (
  IN VOID  *FchDataPtr
  );

extern VOID
ProgramFchEnvSpreadSpectrum (
  IN VOID  *FchDataPtr
  );

extern VOID
ValidateFchVariant (
  IN VOID  *FchDataPtr
  );

VOID
FchInitSmiEnv (
  IN  VOID     *FchDataPtr
  );

/**
 * FchInitEnvHwAcpiP - Config HwAcpi controller preliminary
 *   (Special)
 *   Acpi S3 resume won't execute this procedure (POST only)
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvHwAcpiP (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  HGPI_TESTPOINT (TpFchInitEnvHwAcpiP, NULL);

  RecordFchConfigPtr ((UINT32)((UINTN)(LocalCfgPtr)));

  ProgramEnvPFchAcpiMmio (FchDataPtr);

  ProgramFchEnvSpreadSpectrum (FchDataPtr);
}

/**
 * FchInitEnvHwAcpi - Config HwAcpi controller before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvHwAcpi (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;

  HGPI_TESTPOINT (TpFchInitEnvHwAcpi, NULL);

  ProgramFchEnvHwAcpiPciReg (FchDataPtr);
  //
  // FCH Specific Function programming
  //
  ProgramSpecificFchInitEnvAcpiMmio (FchDataPtr);
  HpetInit (LocalCfgPtr);
  FchAl2ahbInit (LocalCfgPtr);
  FchI2cUartInit (LocalCfgPtr);
}

VOID
FchInitSmiEnv (
  IN  VOID     *FchDataPtr
  )
{
  UINT16          FchSmiData;
  UINT8           SwSmiCmdAddress;
  FCH_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;

  FchSmiData = (FCH_SMI_DATA_POST_ENV << 8) | FCH_AOAC_SW_VALUE;
  SwSmiCmdAddress = ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A);
  LibHygonIoWrite (AccessWidth16, SwSmiCmdAddress, &FchSmiData, LocalCfgPtr->StdHeader);
}
