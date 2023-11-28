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

#ifndef _AMD_I2C_MASTER_DXE_H_
#define _AMD_I2C_MASTER_DXE_H_

UINT32 I2cSdpAddressList[] = {
//  0xFEDC2000,
  0xFEDC3000,
//  0xFEDC4000,
//  0xFEDC5000,
//  0xFEDC6000,
//  0xFEDCB000
};

/// I2cConfiguration struct
typedef struct _I2C_CONFIGURATION {
  UINT32 RxFifoDepth;       // RxFifoDepth
  UINT32 TxFifoDepth;       // TxFifoDepth
} I2C_CONFIGURATION;

#define I2C_CONTROLLER_COUNT  (sizeof (I2cSdpAddressList) / sizeof (UINT32))

/// I2cMaster Private struct
typedef struct _I2CMASTER_PRIVATE {
  EFI_I2C_MASTER_PROTOCOL   I2cController;                  ///< I2cController;
  EFI_HANDLE                Handle;                         ///< Handle;
  UINT32                    I2cSdpAddress;                  ///< I2cSdpAddress;
  UINT8                     *WriteData;                     ///< *WriteData;
  UINT32                    WriteCount;                     ///< WriteCount;
  UINT8                     *ReadData;                      ///< *ReadData;
  UINT32                    ReadCount;                      ///< ReadCount;
  UINT32                    TransferCount;                  ///< TransferCount;
  I2C_CONFIGURATION         I2cConfiguration;               ///< I2cConfiguration;
} I2CMASTER_PRIVATE;

/// Private I2cMaster Data Block Structure
typedef struct _AMD_I2C_MASTER_PRIVATE {
  I2CMASTER_PRIVATE   I2cMaster[I2C_CONTROLLER_COUNT];      ///< I2c0~5 Data Block
} AMD_I2C_MASTER_PRIVATE;

EFI_STATUS
EFIAPI
SetBusFrequency (
  IN CONST EFI_I2C_MASTER_PROTOCOL   *This,
  IN UINTN                    *BusClockHertz
  );

EFI_STATUS
EFIAPI
Reset (
  IN CONST EFI_I2C_MASTER_PROTOCOL *This
  );

EFI_STATUS
EFIAPI
StartRequest (
  IN CONST EFI_I2C_MASTER_PROTOCOL    *This,
  IN UINTN                            SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET           *RequestPacket,
  IN EFI_EVENT                        Event      OPTIONAL,
  OUT  EFI_STATUS                     *I2cStatus OPTIONAL
  );


//*******************************************************
//      AmdI2cMasterPei.c use only functions prototypes
//*******************************************************

EFI_STATUS
I2cInit (
  IN OUT I2CMASTER_PRIVATE    *Private
  );

EFI_STATUS
I2cWriteRead (
  IN OUT I2CMASTER_PRIVATE    *Private
  );

EFI_STATUS
I2cAccess (
  IN I2CMASTER_PRIVATE             *Private,
  IN UINTN                         SlaveAddress,
  IN OUT EFI_I2C_REQUEST_PACKET    *RequestPacket
  );

EFI_STATUS
I2cDwWaitI2cEnable (
  IN  UINT32   Base,
  IN  UINT32   I2cEnBit
  );

//
// Functions Prototypes
//
EFI_STATUS
EFIAPI
AmdI2cMasterDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  );

#endif // _AMD_I2C_MASTER_DXE_H_
