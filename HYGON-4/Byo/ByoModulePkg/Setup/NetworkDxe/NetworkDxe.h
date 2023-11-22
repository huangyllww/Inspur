/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  NetworkDxe.h

Abstract:

Revision History:

**/

#ifndef __NETWORK_H__
#define __NETWORK_H__


#include <Guid/PxeControlVariable.h>


#define SETUP_NETWORK_CONFIGURATION_FORMSET_GUID \
  { 0x195c1197, 0xe667, 0x0e0e, { 0x29, 0xea, 0x82, 0x5a, 0xd2, 0xd2, 0x28, 0xe9 } }


#define NETWORD_FORM_ID             0x1000
#define NETWORK_FORMSET_CLASS_ID    0x1001

extern EFI_GUID gByoSetupNetworkConfigurationFormsetGuid;

#endif
