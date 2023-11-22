/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiTransportPpi.h

Abstract:
  Source file for the IPMI PEIM.

Revision History:

**/

#ifndef _IPMI_TRANSPORT_PPI_H_
#define _IPMI_TRANSPORT_PPI_H_

#include "EfiServerManagement.h"

typedef struct _PEI_IPMI_TRANSPORT_PPI PEI_IPMI_TRANSPORT_PPI;

#define PEI_IPMI_TRANSPORT_PPI_GUID \
  { \
    0x7bf5fecc, 0xc5b5, 0x4b25, 0x81, 0x1b, 0xb4, 0xb5, 0xb, 0x28, 0x79, 0xf7 \
  }  

//
// Common Defines
//
typedef UINT32  EFI_BMC_STATUS;

#define EFI_BMC_OK        0
#define EFI_BMC_SOFTFAIL  1
#define EFI_BMC_HARDFAIL  2
#define EFI_BMC_UPDATE_IN_PROGRESS  3
#define EFI_BMC_NOTREADY  4


//
//  IPMI Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI *PEI_IPMI_SEND_COMMAND) (
  IN PEI_IPMI_TRANSPORT_PPI            * This,
  IN UINT8                             NetFunction,
  IN UINT8                             Lun,
  IN UINT8                             Command,
  IN UINT8                             *CommandData,
  IN UINT8                             CommandDataSize,
  OUT UINT8                            *ResponseData,
  OUT UINT8                            *ResponseDataSize
  );

typedef
EFI_STATUS
(EFIAPI *PEI_IPMI_GET_CHANNEL_STATUS) (
  IN PEI_IPMI_TRANSPORT_PPI           * This,
  OUT EFI_BMC_STATUS                  * BmcStatus,
  OUT EFI_SM_COM_ADDRESS              * ComAddress
  );

//
// IPMI TRANSPORT PPI
//
struct _PEI_IPMI_TRANSPORT_PPI {
  UINT64                      Revision;
  PEI_IPMI_SEND_COMMAND       SendIpmiCommand;
  PEI_IPMI_GET_CHANNEL_STATUS GetBmcStatus;
  UINT8                       LastCompleteCode;
};

extern EFI_GUID gPeiIpmiTransportPpiGuid;

#endif
