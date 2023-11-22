/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_CUSTOMIZED_SORT_POLICY_H__
#define __BYO_CUSTOMIZED_SORT_POLICY_H__

#define BYO_CUSTOMIZED_SORT_POLICY_VAR_NAME     L"CustomizedSortPolicy"

extern EFI_GUID gByoGlobalVariableGuid;

#define BYO_SORT_POLICY_VAR_ATTR   (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS)

#define COUNT_OF_HDD_GROUP_TYPE  8
#define COUNT_OF_IP_GROUP_TYPE   4

//
// Provided sorting policies to assign OPTION_SORT_POLICY.CustomizedSortPolicy:
// * STACK_NET_OPTIONS_BY_SLOTNUMBER: sort PCI net options whose Description including L"Slot N" by slot number(N is
//                                    slot number), and put net options whose Description including L"Onboard" on the
//                                    first position.
// * INSERT_NEW_OPTION_BY_PRIORITY: enable inserting new net and hdd options by DevicePriority.For example, old options
//                                  are SATA2 HDD, SATA4 HDD, and now insert SATA3 HDD.When add
//                                  INSERT_NEW_OPTION_BY_PRIORITY to OPTION_SORT_POLICY.CustomizedSortPolicy, the order
//                                  of HDD options is SATA2 HDD, SATA3 HDD, SATA4 HDD.Otherwise, the order is SATA2 HDD,
//                                  SATA4 HDD, SATA3 HDD.
// * CUSTOMIZED_SORT_POLICY_DEFAULT: default sorting policy summarized in relevant byo wiki.
//
#define STACK_NET_OPTIONS_BY_SLOTNUMBER       0x00000001
#define INSERT_NEW_OPTION_BY_PRIORITY         0x00000002
#define CUSTOMIZED_SORT_POLICY_DEFAULT        0x80000000 // defined in gDefaultOptionSortOption

#define MASK_OF_PRIORITY 0x1000000
#define INVALID_PRIORITY 0xFFFFFFF

typedef enum {
  HDD_GROUP_TYPE_MIN = 1,
  HDD_SHORT_FORM_TYPE = HDD_GROUP_TYPE_MIN,
  NVME_SSD_TYPE,
  SATA_HDD_TYPE,
  ATAPI_HDD_TYPE,
  RAID_HDD_TYPE,
  SCSI_HDD_TYPE,
  SD_HDD_TYPE,
  MMC_HDD_TYPE,
  HDD_GROUP_TYPE_MAX = 0xF
} HDD_GROUP_TYPE;

typedef enum {
  IP_GROUP_TYPE_MIN = 0,
  PXE_IPV4_TYPE = IP_GROUP_TYPE_MIN,
  PXE_IPV6_TYPE,
  HTTP_IPV4_TYPE,
  HTTP_IPV6_TYPE,
  IP_GROUP_TYPE_MAX
} NET_IP_GROUP_TYPE;

typedef struct {
  UINT32       HddGroupOrder[COUNT_OF_HDD_GROUP_TYPE];
  UINT8        NetIpGroupType[COUNT_OF_IP_GROUP_TYPE];
  UINT32       CustomizedSortPolicy;
} OPTION_SORT_POLICY;


#endif // __BYO_CUSTOMIZED_SORT_POLICY_H__

