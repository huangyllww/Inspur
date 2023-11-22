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

#ifndef _HYGON_SOC_DXE_H_
#define _HYGON_SOC_DXE_H_

extern EFI_GUID  gHygonErrorLogDepexProtocolGuid;
extern EFI_GUID  gHygonFchTaishanDepexProtocolGuid;
extern EFI_GUID  gHygonFchTSSmbusDepexProtocolGuid;
extern EFI_GUID  gHygonFchTSSmmDispacherDepexProtocolGuid;
extern EFI_GUID  gHygonFchTSSmmControlDepexProtocolGuid;
extern EFI_GUID  gHygonFchTSMultiFchDepexProtocolGuid;
extern EFI_GUID  gHygonPspDxeDepexProtocolGuid;
extern EFI_GUID  gHygonPspP2CmboxDepexProtocolGuid;
extern EFI_GUID  gHygonPspSmmDepexProtocolGuid;
extern EFI_GUID  gHygonPspHstiDepexProtocolGuid;
extern EFI_GUID  gHygonMemDhm1DxeSatDepexProtocolGuid;
extern EFI_GUID  gHygonMemHygonDxeDepexProtocolGuid;
extern EFI_GUID  gHygonNbioBaseDxeDepexProtocolGuid;
extern EFI_GUID  gHygonNbioSmuDxeDepexProtocol;
extern EFI_GUID  gHygonNbioPcieDxeDepexProtocolGuid;
extern EFI_GUID  gHygonNbioIOMMUSTDxeDepexProtocolGuid;
extern EFI_GUID  gHygonNbioALIBSTDxeDepexProtocolGuid;
extern EFI_GUID  gHygonNbioCxlDxeDepexProtocolGuid;
extern EFI_GUID  gHygonCcxDharmaDepexProtocolGuid;
extern EFI_GUID  gHygonCcxDharmaDepexSmmProtocolGuid;
extern EFI_GUID  gHygonFabricStDepexProtocolGuid;
extern EFI_GUID  gHygonMemSmbiosDepexProtocolGuid;
extern EFI_GUID  gHygonHpcbServiceDepexProtocolGuid;

//
// Silicon Driver installation list
//

EFI_GUID  *HyExSiliconDriverProtocolList[] = {
  &gHygonErrorLogDepexProtocolGuid,
  &gHygonFchTaishanDepexProtocolGuid,
  &gHygonFchTSSmbusDepexProtocolGuid,
  &gHygonFchTSSmmDispacherDepexProtocolGuid,
  &gHygonFchTSSmmControlDepexProtocolGuid,
  &gHygonFchTSMultiFchDepexProtocolGuid,
  &gHygonPspDxeDepexProtocolGuid,
  &gHygonPspP2CmboxDepexProtocolGuid,
  &gHygonPspSmmDepexProtocolGuid,
  &gHygonPspHstiDepexProtocolGuid,
  &gHygonMemDhm1DxeSatDepexProtocolGuid,
  &gHygonMemHygonDxeDepexProtocolGuid,
  &gHygonNbioBaseDxeDepexProtocolGuid,
  &gHygonNbioSmuDxeDepexProtocol,
  &gHygonNbioPcieDxeDepexProtocolGuid,
  &gHygonNbioIOMMUSTDxeDepexProtocolGuid,
  &gHygonNbioALIBSTDxeDepexProtocolGuid,
  &gHygonCcxDharmaDepexProtocolGuid,
  &gHygonCcxDharmaDepexSmmProtocolGuid,
  &gHygonFabricStDepexProtocolGuid,
  &gHygonMemSmbiosDepexProtocolGuid,
  &gHygonHpcbServiceDepexProtocolGuid
};

UINT8  HyExSiliconDriverProtocolListNumber = (UINT8)(sizeof (HyExSiliconDriverProtocolList) / sizeof (HyExSiliconDriverProtocolList[0]));

EFI_GUID  *HyGxSiliconDriverProtocolList[] = {
  &gHygonErrorLogDepexProtocolGuid,
  &gHygonFchTaishanDepexProtocolGuid,
  &gHygonFchTSSmbusDepexProtocolGuid,
  &gHygonFchTSSmmDispacherDepexProtocolGuid,
  &gHygonFchTSSmmControlDepexProtocolGuid,
  &gHygonFchTSMultiFchDepexProtocolGuid,
  &gHygonPspDxeDepexProtocolGuid,
  &gHygonPspP2CmboxDepexProtocolGuid,
  &gHygonPspSmmDepexProtocolGuid,
  &gHygonPspHstiDepexProtocolGuid,
  &gHygonMemDhm1DxeSatDepexProtocolGuid,
  &gHygonMemHygonDxeDepexProtocolGuid,
  &gHygonNbioBaseDxeDepexProtocolGuid,
  &gHygonNbioSmuDxeDepexProtocol,
  &gHygonNbioPcieDxeDepexProtocolGuid,
  &gHygonNbioIOMMUSTDxeDepexProtocolGuid,
  &gHygonNbioALIBSTDxeDepexProtocolGuid,
  &gHygonNbioCxlDxeDepexProtocolGuid,
  &gHygonCcxDharmaDepexProtocolGuid,
  &gHygonCcxDharmaDepexSmmProtocolGuid,
  &gHygonFabricStDepexProtocolGuid,
  &gHygonMemSmbiosDepexProtocolGuid,
  &gHygonHpcbServiceDepexProtocolGuid
};

UINT8  HyGxSiliconDriverProtocolListNumber = (UINT8)(sizeof (HyGxSiliconDriverProtocolList) / sizeof (HyGxSiliconDriverProtocolList[0]));
//
// Functions Prototypes
//
EFI_STATUS
EFIAPI
HygonSocDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  );

#endif // _HYGON_SOC_SL2_ST_DXE_H_
