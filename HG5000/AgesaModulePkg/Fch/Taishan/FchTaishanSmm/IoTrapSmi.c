/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH SMM Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH SMM Driver
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
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
#include "FchSmm.h"
#define FILECODE UEFI_SMM_FCHSMM_IOTRAPSMI_FILECODE
UINT8       bLastFlag;
UINT16       wCurrentValue;

//#define DO_IO_TRAP_TEST     TRUE

/*++

Routine Description:
  SMI handler to output PC99 when CF9 reset

Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering
  DispatchContext - Pointer to the FCH_SMM_IO_TRAP_REGISTER_CONTEXT

Returns:

  None.

--*/
EFI_STATUS
EFIAPI
AmdIoTrapSmiCallback (
  IN       EFI_HANDLE   DispatchHandle,
  IN       CONST VOID   *Context        OPTIONAL,
  IN OUT   VOID         *CommBuffer     OPTIONAL,
  IN OUT   UINTN        *CommBufferSize OPTIONAL
  )
{
  EFI_STATUS  Status;
  UINT8       bData;

  Status = 0;
  bData = 0x99;

  //output PC99
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, 0x80, &bData);

  //do USB reset
  bData = 0x87;
  LibFchSmmPmioWrite8 (&gSmst->SmmIo, FCH_PMIOA_REGF3, &bData);
  bData = 0x00;
  LibFchSmmPmioWrite8 (&gSmst->SmmIo, FCH_PMIOA_REGF3, &bData);

  //enable CF9 reset
  LibFchSmmPmioRead8 (&gSmst->SmmIo, FCH_PMIOA_REGD2, &bData);
  bData &= ~BIT6;
  LibFchSmmPmioWrite8 (&gSmst->SmmIo, FCH_PMIOA_REGD2, &bData);

  //reset system by CF9
  LibFchSmmIoRead (&gSmst->SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGCF9, &bData);
  bData |= BIT2;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGCF9, &bData);
// Shadow CF9 LibFchSmmPmioWrite8 (&gSmst->SmmIo, FCH_PMIOA_REGC5, &bData);

  return Status;
}

/*++

Routine Description:
  SMI handler to change divider 26 times

Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering
  DispatchContext - Pointer to the FCH_SMM_IO_TRAP_REGISTER_CONTEXT

Returns:

  None.

--*/
EFI_STATUS
EFIAPI
AmdLegacyUartTrapSmiCallback (
  IN       EFI_HANDLE   DispatchHandle,
  IN       CONST VOID   *Context        OPTIONAL,
  IN OUT   VOID         *CommBuffer     OPTIONAL,
  IN OUT   UINTN        *CommBufferSize OPTIONAL
  )
{
  EFI_STATUS  Status;
  UINT8       bData;
  UINT8       Value8;
  UINT16      Value16;
  UINT16      IoAddress;
  UINT16      Divisor_Latch_Low_Byte;
  UINT16      Divisor_Latch_High_Byte;
  UINT16      Line_Control_Register;
  UINT16                                      wData;
  FCH_SMM_IO_TRAP_REGISTER_CONTEXT           *IoTrapRegisterContext;

  Status = 0;
  IoTrapRegisterContext = (FCH_SMM_IO_TRAP_REGISTER_CONTEXT *) Context;
  IoAddress = IoTrapRegisterContext->Address & 0xfff8;
  Divisor_Latch_Low_Byte = IoAddress + 0;
  Divisor_Latch_High_Byte = IoAddress + 1;
  Line_Control_Register = IoAddress + 3;
  bData = 0xff & (IoAddress >> 4);
  //output PC"EB"
  LibFchSmmIoRead (&gSmst->SmmIo, SMM_IO_UINT8, Line_Control_Register, &bData);

  //
  if (bData & BIT7) {
    bLastFlag = BIT7;
    LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT16, 0x80, &Line_Control_Register);
  }  else {
    //Fix Divisor here
    if (bLastFlag) {
      bData |= BIT7;
      LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, Line_Control_Register, &bData);
      LibFchSmmIoRead (&gSmst->SmmIo, SMM_IO_UINT8, Divisor_Latch_High_Byte, &Value8);
      Value16 = Value8 << 8;
      LibFchSmmIoRead (&gSmst->SmmIo, SMM_IO_UINT8, Divisor_Latch_Low_Byte, &Value8);
      Value16 += Value8;
      if (wCurrentValue != Value16) {
          for (wData = 0; wData < 0xffff; wData ++) {  //display previous data
            LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT16, 0x80, &Value16);
          }
      Value16 *= 26;
          wCurrentValue = Value16;
          LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT16, 0x80, &Value16); //display current data
      Value8 = Value16 & 0xff;
          LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, Divisor_Latch_Low_Byte, &Value8);
      Value8 = (Value16 >> 8) & 0xff;
          LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, Divisor_Latch_High_Byte, &Value8);
      }
      bData &= ~ BIT7;
      LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, Line_Control_Register, &bData);
      bLastFlag = 0;
    }
  }
  ACPIMMIO32 (ACPI_MMIO_BASE + 0x290) = 1 << (20 + IoTrapRegisterContext->Length);
  return Status;
}


EFI_STATUS
FchSmmRegisterIoTrapSmi (
  VOID
  )
{
  EFI_STATUS                                 Status;
  FCH_SMM_IO_TRAP_DISPATCH2_PROTOCOL         *AmdIoTrapDispatch;
  FCH_SMM_IO_TRAP_REGISTER_CONTEXT           IoTrapRegisterContext;
  EFI_HANDLE                                 IoTrapHandle;
//#ifdef DO_IO_TRAP_TEST
  UINT8                                      bData;
//#endif

  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmIoTrapDispatch2ProtocolGuid,
                  NULL,
                  &AmdIoTrapDispatch
                  );
  ASSERT_EFI_ERROR (Status);

#ifdef DO_IO_TRAP_TEST
  ////
  ////Sample: CF9 trap
  ////

  ////disable CF9 first
  LibFchSmmPmioRead8 (&gSmst->SmmIo, FCH_PMIOA_REGD2, &bData);
  bData |= BIT6;
  LibFchSmmPmioWrite8 (&gSmst->SmmIo, FCH_PMIOA_REGD2, &bData);

  ////enbale IO trap for 0xCF9 port
  IoTrapRegisterContext.Address = FCH_IOMAP_REGCF9;
  IoTrapRegisterContext.Length  = 1;
  IoTrapRegisterContext.Type    = WriteTrap;

  Status = AmdIoTrapDispatch->Register (
                                AmdIoTrapDispatch,
                                AmdIoTrapSmiCallback,
                                &IoTrapRegisterContext,
                                &IoTrapHandle
                                );
#endif

if (!(FchCheckZPB2 ())) {
LibFchSmmPciRead (&gSmst->SmmIo, SMM_IO_UINT8,0xA00008, &bData);
if (bData == 0x59) {  //Summit 
  bLastFlag = 0;
  wCurrentValue = 0;
  if ((ACPIMMIO8 (0xFEDC0020) & BIT0 )) {
  IoTrapRegisterContext.Address = 0x2EB;
      IoTrapRegisterContext.Length  = 0;
      IoTrapRegisterContext.Type    = WriteTrap;
      bLastFlag = 0;
      Status = AmdIoTrapDispatch->Register (
                                    AmdIoTrapDispatch,
                                    AmdLegacyUartTrapSmiCallback,
                                    &IoTrapRegisterContext,
                                    &IoTrapHandle
                                    );
  }
  if ((ACPIMMIO8 (0xFEDC0020) & BIT1 )) {
      IoTrapRegisterContext.Address = 0x2FB;
  IoTrapRegisterContext.Length  = 1;
  IoTrapRegisterContext.Type    = WriteTrap;
  bLastFlag = 0;
      Status = AmdIoTrapDispatch->Register (
                                    AmdIoTrapDispatch,
                                    AmdLegacyUartTrapSmiCallback,
                                    &IoTrapRegisterContext,
                                    &IoTrapHandle
                                    );
  }
  if ((ACPIMMIO8 (0xFEDC0020) & BIT2 )) {
      IoTrapRegisterContext.Address = 0x3EB;
      IoTrapRegisterContext.Length  = 2;
      IoTrapRegisterContext.Type    = WriteTrap;
      bLastFlag = 0;
  Status = AmdIoTrapDispatch->Register (
                                AmdIoTrapDispatch,
                                AmdLegacyUartTrapSmiCallback,
                                &IoTrapRegisterContext,
                                &IoTrapHandle
                                );
  }
  if ((ACPIMMIO8 (0xFEDC0020) & BIT3 )) {
      IoTrapRegisterContext.Address = 0x3FB;
      IoTrapRegisterContext.Length  = 3;
      IoTrapRegisterContext.Type    = WriteTrap;
      bLastFlag = 0;
      Status = AmdIoTrapDispatch->Register (
                                    AmdIoTrapDispatch,
                                    AmdLegacyUartTrapSmiCallback,
                                    &IoTrapRegisterContext,
                                    &IoTrapHandle
                                    );
  }
}
}
  return Status;
}


