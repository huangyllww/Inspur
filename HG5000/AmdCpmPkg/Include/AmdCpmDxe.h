/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM structures and definitions
 *
 * Contains AMD CPM Platform Interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  Include
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */

#ifndef _AMD_CPM_DXE_H_
#define _AMD_CPM_DXE_H_

#include <AmdCpmBase.h>
#include CPM_PROTOCOL_DEFINITION (AmdCpmTableProtocol)
#include CPM_PROTOCOL_DEFINITION (AmdCpmNvDataProtocol)
#include CPM_PROTOCOL_DEFINITION (AmdBufferManagerProtocol)
#include CPM_PROTOCOL_DEFINITION (AmdCpmAllPciIoProtocolsInstalled)
#include CPM_GUID_DEFINITION (AmdCpmTableHob)

#include <PiDxe.h>
#include CPM_LIBRARY_DEFINITION (BaseLib)
#include CPM_LIBRARY_DEFINITION (BaseMemoryLib)
#include CPM_LIBRARY_DEFINITION (DebugLib)
#include CPM_LIBRARY_DEFINITION (HobLib)
#include CPM_LIBRARY_DEFINITION (PcdLib)
#include CPM_LIBRARY_DEFINITION (UefiLib)
#include CPM_LIBRARY_DEFINITION (UefiBootServicesTableLib)
#include CPM_INDUSTRYSTANDARD_DEFINITION (Acpi)
#include CPM_INDUSTRYSTANDARD_DEFINITION (Pci22)
#include CPM_INDUSTRYSTANDARD_DEFINITION (SmBus)
#include EFI_PROTOCOL_DEFINITION (CpmFirmwareVolume)
#include EFI_PROTOCOL_DEFINITION (SmbusHc)
#include EFI_PROTOCOL_DEFINITION (AcpiTable)
#include EFI_PROTOCOL_DEFINITION (PciIo)
#include EFI_GUID_DEFINITION (EventGroup)
#include EFI_GUID_DEFINITION (HobList)

typedef union _FCH_LCP_ROM_PROTECT_REG {
	UINT32		LpcRomProtect;
	struct {
		UINT32	Range           :   8;  //bit 0..7
		UINT32	RangeUnit       :   1;  //bit 8
		UINT32	ReadProtect	    :	1;	//bit 9
		UINT32	WriteProtect    :   1;  //bit 10
		UINT32	Reserved        :   1;  //bit 11
		UINT32	RomBase  	    :	20;	//bit 12-31
	}Field;
} FCH_LCP_ROM_PROTECT_REG;


extern EFI_GUID gFchSpiLockGuid; 

typedef struct _SPI_LOCK_PROTOCOL SPI_LOCK_PROTOCOL;

typedef EFI_STATUS(EFIAPI *FCH_SPI_LOCK)(
  IN  UINT32	LockBaseAddr,
  IN  UINT16	LockSize,
  IN  UINT8 	LockUnit,
  IN  UINT8     RangeSelecter
);

/// SPI LOCK PROTOCOL
struct _SPI_LOCK_PROTOCOL {
  UINTN                     Revision;                 ///< Protocol Revision
  FCH_SPI_LOCK              SpiLock;
};


#endif
