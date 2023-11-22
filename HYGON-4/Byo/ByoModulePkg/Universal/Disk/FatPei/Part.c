/** @file
  Routines supporting partition discovery and
  logical device reading

Copyright (c) 2006 - 2022, Byosoft Corporation. All rights reserved.<BR>

This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.


**/

#include "FatLitePeim.h"

/**
  This function finds Eltorito partitions. Main algorithm
  is ported from DXE partition driver.

  @param[in]  PrivateData       The global memory map
  @param[in]  ParentBlockDevNo  The parent block device

  @retval TRUE              New partitions are detected and logical block devices
                            are added to block device array
  @retval FALSE             No new partitions are added

**/
BOOLEAN
FatFindEltoritoPartitions (
  IN  PEI_FAT_PRIVATE_DATA *PrivateData,
  IN  UINTN                ParentBlockDevNo
  );

/**
  This function finds Mbr partitions. Main algorithm
  is ported from DXE partition driver.

  @param[in]  PrivateData       The global memory map
  @param[in]  ParentBlockDevNo  The parent block device

  @retval TRUE              New partitions are detected and logical block devices
                            are added to block device array
  @retval FALSE             No new partitions are added

**/
BOOLEAN
FatFindMbrPartitions (
  IN  PEI_FAT_PRIVATE_DATA *PrivateData,
  IN  UINTN                ParentBlockDevNo
  );

/**
  This function is used for finding GPT partition on block device.
  As follow UEFI spec we should check protective MBR first and then
  try to check both primary/backup GPT structures.

  @param[in]  PrivateData       The global memory map
  @param[in]  ParentBlockDevNo  The parent block device

  @retval TRUE              New partitions are detected and logical block devices
                            are added to block device array
  @retval FALSE             No new partitions are added

**/
BOOLEAN
FatFindGptPartitions (
  IN  PEI_FAT_PRIVATE_DATA *PrivateData,
  IN  UINTN                ParentBlockDevNo
  );

/**
  This function finds partitions (logical devices) in physical block devices.

  @param  PrivateData       Global memory map for accessing global variables.

**/
VOID
FatFindPartitions (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData
  )
{
  BOOLEAN Found;
  UINTN   Index;

  do {
    Found = FALSE;

    for (Index = 0; Index < PrivateData->BlockDeviceCount; Index++) {
      if (!PrivateData->BlockDevice[Index].PartitionChecked) {
        if (FatFindGptPartitions (PrivateData, Index)) {
          Found = TRUE;
          continue;
        }

        if (FatFindMbrPartitions (PrivateData, Index)) {
          Found = TRUE;
          continue;
        }

//        if (FatFindEltoritoPartitions (PrivateData, Index)) {
//          Found = TRUE;
//          continue;
//        }
      }
    }
  } while (Found && PrivateData->BlockDeviceCount <= PEI_FAT_MAX_BLOCK_DEVICE);
}

