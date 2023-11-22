/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

EFI_STATUS
SendElogEventMca (
  IN  UINT8        *ErrorRecord,
  IN  UINT8        BankIndex,
  IN  DIMM_INFO    *DimmInfo
);
  
EFI_STATUS
SendElogEventNbio (
  IN  UINT8        *ErrorRecord
  );

EFI_STATUS
SendElogEventPcie (
  IN  UINT8        *ErrorRecord,
  IN  UINT32       PcieUncorrStatus,
  IN  UINT32       PcieCorrStatus
  );

EFI_STATUS
SendElogEventMemTest (
  IN  UINT8        Socket,
  IN  UINT8        Channel,
  IN  UINT8        Dimm
  );

EFI_STATUS
SendElogEventMemTraining (
  IN  UINT8        Socket,
  IN  UINT8        Channel,
  IN  UINT8        Dimm
  );

