/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to access heap.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Porting.h>
#include <AMD.h>
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/GnbHeapLib.h>
#include <Library/PeiServicesLib.h>
#include <GnbGuidList.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/AmdHeapLib.h>

#define FILECODE LIBRARY_GNBHEAPPEILIB_GNBHEAPPEILIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------*/
/**
 * Allocates space for a new buffer in the heap
 *
 *
 * @param[in]      Handle            Buffer handle
 * @param[in]      Length            Buffer length
 * @param[in]      StdHeader         Standard configuration header
 *
 * @retval         NULL              Buffer allocation fail
 *
 */

VOID *
GnbAllocateHeapBuffer (
  IN      UINT32              Handle,
  IN      UINTN               Length,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS          Status;
  ALLOCATE_HEAP_PARAMS  AllocHeapParams;

  AllocHeapParams.RequestedBufferSize = (UINT32) Length;
  AllocHeapParams.BufferHandle = Handle;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  Status = HeapAllocateBuffer (&AllocHeapParams, StdHeader);
  if (Status != AGESA_SUCCESS) {
    return NULL;
  }
  return AllocHeapParams.BufferPtr;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Allocates space for a new buffer in the heap and clear it
 *
 *
 * @param[in]      Handle            Buffer handle
 * @param[in]      Length            Buffer length
 * @param[in]      StdHeader         Standard configuration header
 *
 * @retval         NULL              Buffer allocation fail
 *
 */

VOID *
GnbAllocateHeapBufferAndClear (
  IN      UINT32              Handle,
  IN      UINTN               Length,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  VOID  *Buffer;
  Buffer = GnbAllocateHeapBuffer (Handle, Length, StdHeader);
  if (Buffer != NULL) {
    LibAmdMemFill (Buffer, 0x00, Length, StdHeader);
  }
  return Buffer;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Locates a previously allocated buffer on the heap.
 *
 *
 * @param[in]      Handle            Buffer handle
 * @param[in]      StdHeader         Standard configuration header
 *
 * @retval         NULL              Buffer handle not found
 *
 */

VOID *
GnbLocateHeapBuffer (
  IN      UINT32              Handle,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS          Status;
  LOCATE_HEAP_PTR       LocHeapParams;
  LocHeapParams.BufferHandle = Handle;
  Status = HeapLocateBuffer (&LocHeapParams, StdHeader);
  if (Status != AGESA_SUCCESS) {
    return NULL;
  }
  return LocHeapParams.BufferPtr;
}

