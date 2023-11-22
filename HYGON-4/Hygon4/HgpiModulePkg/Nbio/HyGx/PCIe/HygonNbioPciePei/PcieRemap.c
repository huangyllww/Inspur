/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe port remapping functions.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: GNB
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
* ***************************************************************************
*
*/
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <HYGON.h>
#include <Gnb.h>
#include <PiPei.h>
#include <Filecode.h>
#include <GnbRegisters.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/TimerLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcieConfigLib.h>

#define FILECODE  NBIO_PCIE_HYGX_HYGONNBIOPCIESTPEI_PCIEREMAPST_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

#define DEVFUNC(d, f) ((((UINT8) d) << 3) | ((UINT8) f))

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                     L O C A L   D A T A   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

UINT8 ROMDATA DefaultPortDevMap [] = {
  DEVFUNC (1, 1),
  DEVFUNC (1, 2),
  DEVFUNC (1, 3),
  DEVFUNC (1, 4),
  DEVFUNC (1, 5),
  DEVFUNC (1, 6),
  DEVFUNC (1, 7),
  DEVFUNC (2, 1),
  DEVFUNC (3, 1),
  DEVFUNC (7, 1),
  DEVFUNC (8, 1),
  DEVFUNC (9, 1),
  DEVFUNC (5, 1),
  DEVFUNC (5, 2),
  DEVFUNC (5, 3),
  DEVFUNC (5, 4),
  DEVFUNC (5, 5),
  DEVFUNC (5, 6),
  DEVFUNC (5, 7),
  DEVFUNC (6, 1)
};

/*----------------------------------------------------------------------------------------*/

/**
 * Check if engine can be remapped to Device/function number requested by user
 * defined engine descriptor
 *
 *   Function only called if requested device/function does not much native device/function
 *
 * @param[in]  DevFunc             PCI Device(7:3) and Func(2:0)
 * @retval     TRUE                Descriptor can be mapped to engine
 * @retval     FALSE               Descriptor can NOT be mapped to engine
 */
BOOLEAN
PcieCheckPortPciDeviceMapping (
  IN      UINT8         DevFunc
  )
{
  UINT8  Index;

  for (Index = 0; Index < (sizeof (DefaultPortDevMap) / sizeof (DefaultPortDevMap[0])); Index++) {
    if (DefaultPortDevMap[Index] == DevFunc) {
      return TRUE;
    }
  }

  return FALSE;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Map engine to specific PCI device address
 *
 *
 *
 * @param[in]  Engine              Pointer to engine configuration
 * @param[in]  PortDevMap          Pointer to PortDevMap
 * @retval     HGPI_ERROR         Fail to map PCI device address
 * @retval     HGPI_SUCCESS       Successfully allocate PCI address
 */
HGPI_STATUS
PcieMapPortPciAddress (
  IN      PCIe_ENGINE_CONFIG     *Engine,
  IN      UINT8 *PortDevMap
  )
{
  HGPI_STATUS  Status;
  UINT8        DevFunc;
  UINT8        Index;

  Status = HGPI_SUCCESS;
  // IDS_HDT_CONSOLE (GNB_TRACE, "PcieMapPortPciAddress Enter\n");
  if (Engine->Type.Port.PortData.DeviceNumber == 0 && Engine->Type.Port.PortData.FunctionNumber == 0) {
    Engine->Type.Port.PortData.DeviceNumber   = Engine->Type.Port.NativeDevNumber;
    Engine->Type.Port.PortData.FunctionNumber = Engine->Type.Port.NativeFunNumber;
  }

  DevFunc = (Engine->Type.Port.PortData.DeviceNumber << 3) | Engine->Type.Port.PortData.FunctionNumber;
  if (PcieCheckPortPciDeviceMapping (DevFunc) != TRUE) {
    Status = HGPI_ERROR;
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "    Device %x, Function %x\n", Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber);
    for (Index = 0; Index < sizeof (DefaultPortDevMap); ++Index) {
      if (PortDevMap[Index] == DevFunc) {
        Status = HGPI_ERROR;
        break;
      }
    }
  }

  if (Status == HGPI_SUCCESS) {
    if (Engine->Type.Port.PortId <= 8) {
      // PCIE Core 0 port 0~7, core 1 port 0, Logical Bridge Number 0~8
      PortDevMap[Engine->Type.Port.PortId] = DevFunc;
    }

    if (Engine->Type.Port.PortId >= 16) {
      // PCIE Core 2 Port 0~7, Logical Bridge Number 12~19
      PortDevMap[Engine->Type.Port.PortId-4] = DevFunc;
    }

    IDS_HDT_CONSOLE (GNB_TRACE, "    PortDevMap DevFunc 0x%x, for PortId %d\n", DevFunc, Engine->Type.Port.PortId);
  }
  
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMapPortPciAddress Exit [0x%x]\n", Status);
  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Map engine to specific PCI device address
 *
 *
 * @param[in]  GnbHandle           Pointer to the Silicon Descriptor for this node
 * @param[in]  PortDevMap          Pointer to PortDevMap
 */
VOID
PcieSetPortPciAddressMap (
  IN      GNB_HANDLE    *GnbHandle,
  IN      UINT8         *PortDevMap
  )
{
  UINT8                        Index;
  UINT8                        DevFuncIndex;
  UINT8                        PortDevMapLocal[sizeof (DefaultPortDevMap)];
  NB_PROG_DEVICE_REMAP_STRUCT  DeviceRemap;

  LibHygonMemCopy ((VOID *)PortDevMapLocal, (VOID *)DefaultPortDevMap, sizeof (DefaultPortDevMap), (HYGON_CONFIG_PARAMS *)NULL);
  for (Index = 0; Index < sizeof (DefaultPortDevMap); ++Index) {
    if (PortDevMap[Index] != 0) {
      for (DevFuncIndex = 0; DevFuncIndex < sizeof (DefaultPortDevMap); ++DevFuncIndex) {
        if (PortDevMapLocal[DevFuncIndex] == PortDevMap[Index]) {
          PortDevMapLocal[DevFuncIndex] = 0;
          break;
        }
      }
    }
  }

  for (Index = 0; Index < sizeof (DefaultPortDevMap); ++Index) {
    if (PortDevMap[Index] == 0) {
      for (DevFuncIndex = 0; DevFuncIndex < sizeof (DefaultPortDevMap); ++DevFuncIndex) {
        if (PortDevMapLocal[DevFuncIndex] != 0) {
          PortDevMap[Index] = PortDevMapLocal[DevFuncIndex];
          PortDevMapLocal[DevFuncIndex] = 0;
          break;
        }
      }
    }

    if (Index < 18) {
      NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NB_PROG_DEVICE_REMAP_HYGX) + (Index << 2), &DeviceRemap.Value, 0);
    } else {
      NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NB_PROG_DEVICE_REMAP_N18_HYGX) + ((Index-18) << 2), &DeviceRemap.Value, 0);
    }

    DeviceRemap.Field.DevFnMap = PortDevMap[Index];

    if (Index < 18) {
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NB_PROG_DEVICE_REMAP_HYGX) + (Index << 2), &DeviceRemap.Value, 0);
    } else {
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NB_PROG_DEVICE_REMAP_N18_HYGX) + ((Index-18) << 2), &DeviceRemap.Value, 0);
    }
  }
}
