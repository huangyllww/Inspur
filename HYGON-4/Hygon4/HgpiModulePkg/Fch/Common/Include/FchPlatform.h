/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH platform definition
 *
 *
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

#ifndef  _FCH_PLATFORM_H_
#define  _FCH_PLATFORM_H_

#define MAX_SOCKET_SUPPORT  8
#define MAX_IOD_SUPPORT     4
#define MAX_IOMS_SUPPORT    4

#define MAX_SATA_PORTS  8

// #include "Porting.h"
// #include "HYGON.h"
#include "HGPI.h"

#ifndef FCHOEM_ACPI_RESTORE_SWSMI
  #define FCHOEM_BEFORE_PCI_RESTORE_SWSMI  0xD3
  #define FCHOEM_AFTER_PCI_RESTORE_SWSMI   0xD4
  #define FCHOEM_ENABLE_ACPI_SWSMI         0xA0
  #define FCHOEM_DISABLE_ACPI_SWSMI        0xA1
  #define FCHOEM_START_TIMER_SMI           0xBC
  #define FCHOEM_STOP_TIMER_SMI            0xBD
#endif

#ifndef FCHOEM_SPI_UNLOCK_SWSMI
  #define FCHOEM_SPI_UNLOCK_SWSMI  0xAA
#endif
#ifndef FCHOEM_SPI_LOCK_SWSMI
  #define FCHOEM_SPI_LOCK_SWSMI  0xAB
#endif

#ifndef FCHOEM_ACPI_TABLE_RANGE_LOW
  #define FCHOEM_ACPI_TABLE_RANGE_LOW  0xE0000ul
#endif

#ifndef FCHOEM_ACPI_TABLE_RANGE_HIGH
  #define FCHOEM_ACPI_TABLE_RANGE_HIGH  0xFFFF0ul
#endif

#ifndef FCHOEM_ACPI_BYTE_CHECHSUM
  #define FCHOEM_ACPI_BYTE_CHECHSUM  0x100
#endif

#ifndef FCHOEM_IO_DELAY_PORT
  #define FCHOEM_IO_DELAY_PORT  0x80
#endif

#ifndef FCHOEM_OUTPUT_DEBUG_PORT
  #define FCHOEM_OUTPUT_DEBUG_PORT  0x80
#endif

#define FCH_PCIRST_BASE_IO        0xCF9
#define FCH_PCI_RESET_COMMAND06   0x06
#define FCH_PCI_RESET_COMMAND0E   0x0E
#define FCH_KBDRST_BASE_IO        0x64
#define FCH_KBC_RESET_COMMAND     0xFE
#define FCH_ROMSIG_BASE_IO        0x20000l
#define FCH_ROMSIG_SIGNATURE      0x55AA55AAul
#define FCH_MAX_TIMER             0xFFFFFFFFul
#define FCH_GEC_INTERNAL_REG      0x6804
#define FCH_HPET_REG_MASK         0xFFFFF800ul
#define FCH_FAKE_USB_BAR_ADDRESS  0x58830000ul

#define MAX_XHCI_CONTROLLERS                8   /*MAX 16NBIO 8XHCI */
#define MAX_XHCI_CONTROLLERS_PER_SATORI     2

#ifndef FCHOEM_ELAPSED_TIME_UNIT
  #define FCHOEM_ELAPSED_TIME_UNIT  28
#endif

#ifndef FCHOEM_ELAPSED_TIME_DIVIDER
  #define FCHOEM_ELAPSED_TIME_DIVIDER  100
#endif

#include "IdsLib.h"
#include "Filecode.h"
#include "HygonBaseLib.h"
#include "Fch.h"
#include "TSFch.h"
#include "Library/FchInterface.h"
#include "Library/AcpiLib.h"
#include "Library/FchDef.h"
#include "Library/FchAoacLib.h"
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseLib.h>
#include <Library/SerialPortLib.h>
#include <Library/FchBaseLib.h>
#include <Library/PcdLib.h>
#include <Library/SmnAccessLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <GnbRegisters.h>
#include <Library/HygonSmnAddressLib.h>
#include <Library/HygonSocBaseLib.h>

#endif // _FCH_PLATFORM_H_
