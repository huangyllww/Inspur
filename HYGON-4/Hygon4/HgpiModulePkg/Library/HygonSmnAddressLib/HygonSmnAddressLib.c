/* $NoKeywords:$ */

/**
 * @file
 *
 * HygonSmnAddressLib.c
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: HGPI
 * @e sub-project: SOC
 *
 **/
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
  * ***************************************************************************
  *
 */
#include <Library/BaseLib.h>
#include <Base.h>
#include <Library/HygonSocBaseLib.h>
#include <GnbRegisters.h>
/**
 * Returns The ApertureId By Register Type And CPU Model
 *
 * @param[in]     CpuModel       CPU Model
 * @param[in]     RegType        Register Type
 *
 * @retval        Aperture ID
 */
UINT32
GetApertureId (UINT32 CpuModel, UINT32 RegType)
{
  UINT32 ApertureId;

  switch (RegType) {
    case IOHC_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_IOHC_APERTURE_ID_HYGX : NBIO0_IOHC_APERTURE_ID_HYEX);
      break;
    case L2IMU0_L2B_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX : NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX);
      break;
    case IOAPIC_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_IOAPIC_APERTURE_ID_HYGX : NBIO0_IOAPIC_APERTURE_ID_HYEX);
      break;
    case L1IMU_PCIE0_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_L1IMU_PCIE0_APERTURE_ID_HYGX : NBIO0_L1IMU_PCIE0_APERTURE_ID_HYEX);
      break;
    case L2IMU0_L2A_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_L2IMU0_L2A_APERTURE_ID_HYGX : NBIO0_L2IMU0_L2A_APERTURE_ID_HYEX);
      break;
    case L2IMU0_MMIO_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_L2IMU0_MMIO_APERTURE_ID_HYGX : NBIO0_L2IMU0_MMIO_APERTURE_ID_HYEX);
      break;
    case PCIE0_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_PCIE0_APERTURE_ID_HYGX : NBIO0_PCIE0_APERTURE_ID_HYEX);
      break;
    case PCIE1_REG_TYPE:
    case CXL_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_CXL_APERTURE_ID_HYGX : NBIO0_PCIE1_APERTURE_ID_HYEX);
      break;
    case PCIE2_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_PCIE2_APERTURE_ID_HYGX : NBIO0_PCIE2_APERTURE_ID_HYEX);
      break;
    case NBIF0_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_NBIF0_APERTURE_ID_HYGX : NBIO0_NBIF0_APERTURE_ID_HYEX);
      break;
    case NBIF1_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_NBIF1_APERTURE_ID_HYGX : NBIO0_NBIF1_APERTURE_ID_HYEX);
      break;
    case NBIF2_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_NBIF2_APERTURE_ID_HYGX : NBIO0_NBIF2_APERTURE_ID_HYEX);
      break;
    case NBIF3_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_NBIF3_APERTURE_ID_HYGX : UNKNOW_APERTURE_ID);
      break;
    case SYSHUB0_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_SYSHUB0_APERTURE_ID_HYGX : NBIO0_SYSHUB0_APERTURE_ID_HYEX);
      break;
    case SYSHUB1_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? UNKNOW_APERTURE_ID : NBIO0_SYSHUB1_APERTURE_ID_HYEX);
      break;
    case SYSHUB2_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_SYSHUB2_APERTURE_ID_HYGX : UNKNOW_APERTURE_ID);
      break;
    case SYSHUB3_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_SYSHUB3_APERTURE_ID_HYGX : UNKNOW_APERTURE_ID);
      break;
    case NTB_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_NTB_APERTURE_ID_HYGX : NBIO0_NTB_APERTURE_ID_HYEX);
      break;
    case SST_NBIO_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_SST_NBIO_APERTURE_ID_HYGX : NBIO0_SST_NBIO_APERTURE_ID_HYEX);
      break;
    case SST_FCH_REG_TYPE:
      ApertureId = (CpuModel == HYGON_GX_CPU ? NBIO0_SST_FCH_APERTURE_ID_HYGX : NBIO0_SST_FCH_APERTURE_ID_HYEX);
      break;
    default:
      ApertureId = UNKNOW_APERTURE_ID;
      break;
  }

  return ApertureId;
}

/**
 * Returns the NBIO SMN Address
 *
 * @param[in]     PhysicalDieId  Physical Die Id
 * @param[in]     RbId           Root Bridge ID(Nbio ID)
 * @param[in]     Address        Register address
 *
 * @retval        SMN Address
 */
UINT32
LibNbioSmnAddr2 (UINT8 PhysicalDieId, UINT8 RbId, UINT32 Address)
{
  UINT32 SmnAddress;

  SmnAddress = (UINT32)((GetHygonSocModel () == HYGON_GX_CPU) ? 
                         (UINT32)(IOD0_SMN_BASE_HYGX + (PhysicalDieId << 28)  + (RbId << 20) + Address) :
                         (UINT32)(Address + (RbId << 20)));
  
  return SmnAddress;
}

/**
 * Returns the NBIO SMN Address
 *
 * @param[in]     PhysicalDieId  Physical Die Id
 * @param[in]     RbId           Root Bridge ID(Nbio ID)
 * @param[in]     RegType        Register Type
 * @param[in]     RegOft         Register Offset
 *
 * @retval        SMN Address
 */
UINT32
LibNbioSmnAddr3 (UINT8 PhysicalDieId, UINT8 RbId, UINT32 RegType, UINT32 RegOft)
{
  UINT32 SmnAddress;
  UINT32 ApertureId;
  UINT32 CpuModel;
  UINT32 Step = 1;

  CpuModel   = GetHygonSocModel ();
  ApertureId = GetApertureId (CpuModel, RegType);
  if (CpuModel == HYGON_EX_CPU) {
    if ((ApertureId == L1IMU_PCIE0_REG_TYPE) ||
        (ApertureId == L1IMU_PCIE1_REG_TYPE) ||
        (ApertureId == L1IMU_NBIF0_REG_TYPE) ||
        (ApertureId == L1IMU_IOAGR_REG_TYPE)) {
          Step = 4;
        }
  }
  SmnAddress = (UINT32)((CpuModel == HYGON_GX_CPU) ? 
                         (UINT32)(IOD0_SMN_BASE_HYGX + (PhysicalDieId << 28)  + ((ApertureId + RbId * Step) << 20) + (RegOft & 0xFFFFF)) :
                         (UINT32)(((ApertureId + RbId) << 20) + RegOft));
  
  return SmnAddress;
}

/**
 * Returns the IOD SMN Address
 *
 * @param[in]     PhysicalDieId  Physical Die Id
 * @param[in]     Address        Register address
 *
 * @retval        SMN Address
 */
UINT32
LibIodSmnAddr (UINT8 PhysicalDieId, UINT32 Address)
{
  UINT32 SmnAddress;

  SmnAddress = (UINT32)((GetHygonSocModel () == HYGON_GX_CPU) ? 
                         (UINT32)(IOD0_SMN_BASE_HYGX + (PhysicalDieId << 28) + Address) :
                         (UINT32)Address);
  
  return SmnAddress;
}