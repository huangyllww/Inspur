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

#ifndef _HYGON_I2C_MASTER_PEI_H_
#define _HYGON_I2C_MASTER_PEI_H_

EFI_GUID  HygonI2cMasterID = \
{ 0x33df1f4a, 0xf8fe, 0x47e3, { 0x87, 0x53, 0x7a, 0x61, 0x75, 0x88, 0xa4, 0x2c } };

UINT32  I2cSdpAddressList[] = {
  0xFEDC2000,
  0xFEDC3000,
  0xFEDC4000,
  0xFEDC5000,
  0xFEDC6000,
  0xFEDCB000
};

/// I2cSdaHold
typedef struct _I2C_SDA_HOLD {
  BOOLEAN    OverrideEn;
  UINT32     SdaHold;
} I2C_SDA_HOLD;

I2C_SDA_HOLD  I2cSdaHoldTimeList[] = {
  FALSE, 0x1,     // I2C Bus 0
  FALSE, 0x1,     // I2C Bus 1
  FALSE, 0x48,    // I2C Bus 2
  FALSE, 0x1,     // I2C Bus 3
  FALSE, 0x1,     // I2C Bus 4
  FALSE, 0x48     // I2C Bus 5
};

EFI_STATUS
I2cSetSdaHold (
  IN UINT32               I2cSdpAddress
  );

/// I2cConfiguration
typedef struct _I2C_CONFIGURATION {
  UINT32    RxFifoDepth;         ///< RxFifoDepth
  UINT32    TxFifoDepth;         ///< TxFifoDepth
} I2C_CONFIGURATION;

#define I2C_CONTROLLER_COUNT  (sizeof (I2cSdpAddressList) / sizeof (UINT32))

/// I2cMaster PPI Private
typedef struct _I2CMASTER_PPI_PRIVATE {
  EFI_PEI_I2C_MASTER_PPI    I2cPpi;               ///< I2cPpi;
  EFI_PEI_SERVICES          **PeiServices;        ///< **PeiServices;
  UINT32                    I2cSdpAddress;        ///< I2cSdpAddress;
  UINT8                     *WriteData;           ///< *WriteData;
  UINT32                    WriteCount;           ///< WriteCount;
  UINT8                     *ReadData;            ///< *ReadData;
  UINT32                    ReadCount;            ///< ReadCount;
  UINT32                    TransferCount;        ///< TransferCount;
  I2C_CONFIGURATION         I2cConfiguration;     ///< I2cConfiguration;
} I2CMASTER_PPI_PRIVATE;

/// Private I2cMaster Data Block Structure
typedef struct _HYGON_PEI_I2C_MASTER_PRIVATE {
  I2CMASTER_PPI_PRIVATE    I2cMasterPpi[I2C_CONTROLLER_COUNT];     ///< I2c0~3 Data Block
} HYGON_PEI_I2C_MASTER_PRIVATE;

EFI_STATUS
EFIAPI
SetBusFrequency (
  IN EFI_PEI_I2C_MASTER_PPI   *This,
  IN UINTN                    *BusClockHertz
  );

EFI_STATUS
EFIAPI
Reset (
  IN CONST EFI_PEI_I2C_MASTER_PPI  *This
  );

EFI_STATUS
EFIAPI
StartRequest (
  IN CONST EFI_PEI_I2C_MASTER_PPI     *This,
  IN UINTN                            SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET           *RequestPacket
  );

// *******************************************************
// HygonI2cMasterPei.c use only functions prototypes
// *******************************************************

EFI_STATUS
I2cInit (
  IN OUT I2CMASTER_PPI_PRIVATE    *Private
  );

EFI_STATUS
I2cWriteRead (
  IN OUT I2CMASTER_PPI_PRIVATE    *Private
  );

EFI_STATUS
I2cAccess (
  IN I2CMASTER_PPI_PRIVATE         *Private,
  IN UINTN                         SlaveAddress,
  IN OUT EFI_I2C_REQUEST_PACKET    *RequestPacket
  );

EFI_STATUS
I2cDwWaitI2cEnable (
  IN  UINT32   Base,
  IN  UINT32   I2cEnBit
  );

#endif // _HYGON_I2C_MASTER_PEI_H_
