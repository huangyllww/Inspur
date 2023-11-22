/* $NoKeywords:$ */

/**
 * @file
 *
 * Fabric Register Access library
 *
 * Contains interface to the fabric register access library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Lib
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
 **/

#ifndef _FABRIC_REGISTER_ACC_LIB_H_
#define _FABRIC_REGISTER_ACC_LIB_H_

#define FABRIC_REG_ACC_BC  (0xFF)

#define GROUPMODE_DIE      (0x0)

BOOLEAN
IsGroupMode (
  );

UINT32
FabricRegisterAccGetPciDeviceNumberOfDie (
  IN       UINTN   Socket,
  IN       UINTN   LogicalDie,
  IN       BOOLEAN GroupMode
  );

UINT32
FabricRegisterAccRead (
  IN       UINTN  Socket,
  IN       UINTN  LogicalDie,
  IN       UINTN  Function,
  IN       UINTN  Offset,
  IN       UINTN  Instance
  );

VOID
FabricRegisterAccWrite (
  IN       UINTN    Socket,
  IN       UINTN    LogicalDie,
  IN       UINTN    Function,
  IN       UINTN    Offset,
  IN       UINTN    Instance,
  IN       UINT32   Value,
  IN       BOOLEAN  LogForS3
  );

UINT32
FabricRegisterAccRMW (
  IN       UINTN    Socket,
  IN       UINTN    LogicalDie,
  IN       UINTN    Function,
  IN       UINTN    Offset,
  IN       UINTN    Instance,
  IN       UINT32   NandValue,
  IN       UINT32   OrValue,
  IN       BOOLEAN  LogForS3
  );

UINTN
GetCddPhysicalDieId (
  IN       UINTN      LogicalDieId
  );

UINTN
GetCddLogicalDieId (
  IN       UINTN      PhysicalDieId
  );
  
UINT32
CddFabricRegisterAccRead (
  IN       UINTN  Socket,
  IN       UINTN  Cdd,
  IN       UINTN  Function,
  IN       UINTN  Offset,
  IN       UINTN  Instance
  );

VOID
CddFabricRegisterAccWrite (
  IN       UINTN    Socket,
  IN       UINTN    Cdd,
  IN       UINTN    Function,
  IN       UINTN    Offset,
  IN       UINTN    Instance,
  IN       UINT32   Value,
  IN       BOOLEAN  LogForS3
  );

UINT32
CddFabricRegisterAccRMW (
  IN       UINTN    Socket,
  IN       UINTN    Cdd,
  IN       UINTN    Function,
  IN       UINTN    Offset,
  IN       UINTN    Instance,
  IN       UINT32   NandValue,
  IN       UINT32   OrValue,
  IN       BOOLEAN  LogForS3
  );

#endif // _FABRIC_REGISTER_ACC_LIB_H_
