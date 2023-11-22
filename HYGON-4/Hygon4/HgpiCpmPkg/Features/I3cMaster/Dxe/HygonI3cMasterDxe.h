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
 ******************************************************************************
 */

#ifndef _HYGON_I3C_MASTER_DXE_H_
#define _HYGON_I3C_MASTER_DXE_H_

#define I3C_MASTER_DYNAMIC_ADDR  (0x08)

#define I2C_MODE  1
#define I3C_MODE  0

EFI_GUID  HygonI3cMasterID = \
{ 0x63b98eae, 0xeb16, 0x4561, { 0xba, 0x4b, 0xa9, 0x84, 0xf1, 0x12, 0x31, 0x77 } };

UINT32  I3cSdpAddressList[] = {
  0xFEDC6000,
  0xFEDCB000
};

/// I3cConfiguration
typedef struct _I3C_CONFIGURATION {
  UINT32    RxFifoDepth;         ///< RxFifoDepth
  UINT32    TxFifoDepth;         ///< TxFifoDepth
} I3C_CONFIGURATION;

#define I3C_CONTROLLER_COUNT  (sizeof (I3cSdpAddressList) / sizeof (UINT32))

/// I3cMaster PROTOCOL Private
typedef struct _I3CMASTER_PROTOCOL_PRIVATE {
  EFI_DXE_I3C_MASTER_PROTOCOL    I3cProtocol;             ///< I3cProtocol;
  EFI_HANDLE                     Handle;                  ///< Handle;
  UINT32                         I3cSdpAddress;           ///< I3cSdpAddress;
  UINT8                          *WriteData;              ///< *WriteData;
  UINT32                         WriteCount;              ///< WriteCount;
  UINT8                          *ReadData;               ///< *ReadData;
  UINT32                         ReadCount;               ///< ReadCount;
  UINT32                         TransferCount;           ///< TransferCount;
  I3C_CONFIGURATION              I3cConfiguration;        ///< I3cConfiguration;
  UINT32                         BusFrequency;            ///< BusFrequency;
  UINT8                          I3cI2cMode;              ///< Select I3c(0) or I2c(1) mode on I3c controller
} I3CMASTER_PROTOCOL_PRIVATE;

/// Private I3cMaster Data Block Structure
typedef struct _HYGON_DXE_I3C_MASTER_PRIVATE {
  I3CMASTER_PROTOCOL_PRIVATE    I3cMasterProtocol[I3C_CONTROLLER_COUNT];     ///< I3c0~1 Data Block
} HYGON_DXE_I3C_MASTER_PRIVATE;

EFI_STATUS
I3cInit (
  IN OUT   I3CMASTER_PROTOCOL_PRIVATE    *Private
  );

EFI_STATUS
InitCommonRegister (
  UINT32  Base
  );

EFI_STATUS
EFIAPI
SetBusFrequency (
  IN EFI_DXE_I3C_MASTER_PROTOCOL    *This,
  IN UINTN                          *BusClockHertz
  );

EFI_STATUS
EFIAPI
Reset (
  IN CONST EFI_DXE_I3C_MASTER_PROTOCOL  *This
  );

EFI_STATUS
EFIAPI
StartRequest (
  IN CONST EFI_DXE_I3C_MASTER_PROTOCOL      *This,
  IN UINTN                                  SlaveAddress,
  IN EFI_I3C_REQUEST_PACKET                 *RequestPacket
  );

EFI_STATUS
I3cAccess (
  IN I3CMASTER_PROTOCOL_PRIVATE         *Private,
  IN UINT32                             SlaveAddress,
  IN OUT EFI_I3C_REQUEST_PACKET         *RequestPacket
  );

EFI_STATUS
MasterRegistersInit (
  UINT32    Base,
  UINT32    StaticAddress,
  IN UINT8  I3cI2cMode
  );

EFI_STATUS
SendSETAASA (
  UINT32 Base,
  UINT32 StaticAddress
  );

UINT32
Addr_Xor_Neg (
  UINT32 Addr
  );

EFI_STATUS
I3cWaitRespStat (
  IN  UINT32   Base
  );

EFI_STATUS
I3cReadRespQueue (
  IN  UINT32   Base
  );

EFI_STATUS
I3cWaitMasterIdle (
  IN  UINT32   Base
  );

EFI_STATUS
I3cAbort (
  IN  UINT32   Base
  );

EFI_STATUS
I3cSoftRst (
  IN  UINT32   Base
  );

EFI_STATUS
I3cWriteRead (
  IN OUT   I3CMASTER_PROTOCOL_PRIVATE    *Private
  );

EFI_STATUS
I3cWaitRxData (
  IN  UINT32   Base
  );

EFI_STATUS
I3cWaitTxData (
  IN  UINT32   Base
  );

#endif
