/* $NoKeywords:$ */

/**
 * @file
 *
 * Create outline and references for Processor Common Component mainpage documentation.
 *
 * Design guides, maintenance guides, and general documentation, are
 * collected using this file onto the documentation mainpage.
 * This file contains doxygen comment blocks, only.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Documentation
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

/**
 * @page commonmain Processor Common Component Documentation
 *
 * Additional documentation for the Common component consists of
 *
 * - Maintenance Guides:
 *   - @subpage hygonconfigparamname "Naming Guidelines for type HYGON_CONFIG_PARAMS"
 * - Design Guides:
 *   - add here >>>
 *
 */

/**
 * @page hygonconfigparamname Naming Guidelines for type HYGON_CONFIG_PARAMS
 * @par
 * These are the guidelines for naming objects of type HYGON_CONFIG_PARAMS and HYGON_CONFIG_PARAMS * in HGPI code.
 * <ul>
 *
 * <li>
 * Formal parameter names of type HYGON_CONFIG_PARAMS and HYGON_CONFIG_PARAMS * will always be named
 * StdHeader. This covers all function prototypes, function definitions, and method typedefs (a
 * typedef of a function prototype) in HGPI code. Examples:
 * @n @code
 * VOID
 *   LibHygonPciFindNextCap (
 *     IN OUT   PCI_ADDR *Address,
 *     IN       HYGON_CONFIG_PARAMS *StdHeader
 *     )
 *
 * typedef VOID F_DO_TABLE_ENTRY (
 *   IN       TABLE_ENTRY_DATA       *CurrentEntry,
 *   IN       PLATFORM_CONFIGURATION *PlatformConfig,
 *   IN       HYGON_CONFIG_PARAMS      *StdHeader
 *   );
 *
 * @endcode
 *
 * <li>
 * Structure members of type HYGON_CONFIG_PARAMS or HYGON_CONFIG_PARAMS * will always be named StdHeader. Examples:
 * @n @code
   /// Example of struct member naming.
 * typedef struct {
 *   IN OUT   HYGON_CONFIG_PARAMS StdHeader;             ///< Standard Header
 *   IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< platform operational characteristics.
 * } HYGON_CPU_RECOVERY_PARAMS;
 *
 * @endcode
 *
 * <li>
 * Routines which define local variables of type HYGON_CONFIG_PARAMS or HYGON_CONFIG_PARAMS * should
 * name the local variable as closely as practical to StdHeader, but otherwise freedom is allowed. Example:
 * @n @code
 * HYGON_CONFIG_PARAMS *NewStdHeader;
 * [...]
 * NewStdHeader = (HYGON_CONFIG_PARAMS *)AllocHeapParams.BufferPtr;
 * @endcode
 *
 * <li>
 * Arguments to routines with HYGON_CONFIG_PARAMS or HYGON_CONFIG_PARAMS * formal parameters are not
 * checked.  Freedom is allowed in order to conform to these guidelines in a practical, readable
 * way.  This includes typecast arguments.  Examples:
 * @n @code
 * Status = GetEventLog (&LogEvent, (HYGON_CONFIG_PARAMS *)Event);
 *
 * MemS3ExitSelfRefRegDA (NBPtr, &MemPtr->StdHeader);
 * @endcode
 *
 * </ul>
 *
 */
