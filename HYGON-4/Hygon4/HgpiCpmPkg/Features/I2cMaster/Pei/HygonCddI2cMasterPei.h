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

#ifndef _HYGON_CDD_I2C_MASTER_PEI_H_
#define _HYGON_CDD_I2C_MASTER_PEI_H_

#define CDD_I2C_SMN_BASEADDRESS  0x5F000

/// I2cConfiguration
typedef struct _CDD_I2C_CONFIGURATION {
  UINT32    RxFifoDepth;         ///< RxFifoDepth
  UINT32    TxFifoDepth;         ///< TxFifoDepth
} CDD_I2C_CONFIGURATION;

/// CddI2cMaster PPI Private
typedef struct _CDD_I2CMASTER_PPI_PRIVATE {
  EFI_PEI_I2C_MASTER_PPI    I2cPpi;               ///< I2cPpi;
  EFI_PEI_SERVICES          **PeiServices;        ///< **PeiServices;
  UINT8                     SocketNumber;         ///< Access of socket number
  UINT8                     CddNumber;            ///< Access of CDD number
  UINT32                    I2cSmnAddress;        ///< I2cSmnAddress;
  UINT8                     *WriteData;           ///< *WriteData;
  UINT32                    WriteCount;           ///< WriteCount;
  UINT8                     *ReadData;            ///< *ReadData;
  UINT32                    ReadCount;            ///< ReadCount;
  UINT32                    TransferCount;        ///< TransferCount;
  CDD_I2C_CONFIGURATION     I2cConfiguration;     ///< I2cConfiguration;
} CDD_I2CMASTER_PPI_PRIVATE;

/// Private CddI2cMaster Data Block Structure
typedef struct _HYGON_PEI_CDD_I2C_MASTER_PRIVATE {
  CDD_I2CMASTER_PPI_PRIVATE    CddI2cMasterPpi;     ///< I2c0~5 Data Block
} HYGON_PEI_CDD_I2C_MASTER_PRIVATE;

EFI_STATUS
EFIAPI
CddI2cSetBusFrequency (
  IN EFI_PEI_I2C_MASTER_PPI   *This,
  IN UINTN                    *BusClockHertz
  );

EFI_STATUS
EFIAPI
CddI2cReset (
  IN CONST EFI_PEI_I2C_MASTER_PPI  *This
  );

EFI_STATUS
EFIAPI
CddI2cStartRequest (
  IN CONST EFI_PEI_I2C_MASTER_PPI     *This,
  IN UINTN                            SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET           *RequestPacket
  );

// *******************************************************
// HygonI2cMasterPei.c use only functions prototypes
// *******************************************************

EFI_STATUS
CddI2cInit (
  IN OUT CDD_I2CMASTER_PPI_PRIVATE    *Private
  );

EFI_STATUS
CddI2cWriteRead (
  IN OUT CDD_I2CMASTER_PPI_PRIVATE    *Private
  );

EFI_STATUS
CddI2cAccess (
  IN CDD_I2CMASTER_PPI_PRIVATE         *Private,
  IN UINTN                             SlaveAddress,
  IN OUT EFI_I2C_REQUEST_PACKET        *RequestPacket
  );

EFI_STATUS
CddI2cDwWaitI2cEnable (
  IN  CDD_I2CMASTER_PPI_PRIVATE         *Private,
  IN  UINT32   Base,
  IN  UINT32   I2cEnBit
  );

EFI_STATUS
HygonCddI2cMasterPeiInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

#endif // _HYGON_CDD_I2C_MASTER_PEI_H_
