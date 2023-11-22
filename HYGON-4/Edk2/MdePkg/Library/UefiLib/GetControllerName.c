/** @file
  Mde UEFI library API implementation.
  Get Controller Name

  Copyright (c) 2007 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UefiLibInternal.h"

#define RELATIONSHIP_TYPE_DRIVER_BINDING_HANDLE  BIT2
#define RELATIONSHIP_TYPE_DEVICE_DRIVER          BIT3
#define RELATIONSHIP_TYPE_PARENT_HANDLE          BIT9

/**
  Gets all the related EFI_HANDLEs based on the single EFI_HANDLE and the mask
  supplied.

  This function will scan all EFI_HANDLES in the UEFI environment's handle database
  and return all the ones with the specified relationship (Mask) to the specified
  controller handle.

  If both ControllerHandle are NULL, then ASSERT.
  If MatchingHandleCount is NULL, then ASSERT.

  If MatchingHandleBuffer is not NULL upon a sucessful return the memory must be
  caller freed.

  @param[in] ControllerHandle       Handle to a device with Device Path protocol on it.
  @param[in] Mask                   Mask of what relationship(s) is desired.
  @param[in] MatchingHandleCount    Poitner to UINTN specifying number of HANDLES in
                                    MatchingHandleBuffer.
  @param[out] MatchingHandleBuffer  On a sucessful return a buffer of MatchingHandleCount
                                    EFI_HANDLEs and a terminating NULL EFI_HANDLE.

  @retval EFI_SUCCESS               The operation was sucessful and any related handles
                                    are in MatchingHandleBuffer;
  @retval EFI_NOT_FOUND             No matching handles were found.
  @retval EFI_INVALID_PARAMETER     A parameter was invalid or out of range.
**/
EFI_STATUS
EFIAPI
ScanHandleDatabaseByRelationship (
  IN CONST EFI_HANDLE  ControllerHandle,
  IN CONST UINTN       Mask,
  IN       UINTN       *MatchingHandleCount,
  OUT      EFI_HANDLE  **MatchingHandleBuffer
  )
{
  EFI_STATUS                           Status;
  UINTN                                HandleIndex;
  EFI_GUID                             **ProtocolGuidArray;
  UINTN                                ArrayCount;
  UINTN                                ProtocolIndex;
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY  *OpenInfo;
  UINTN                                OpenInfoCount;
  UINTN                                OpenInfoIndex;
  UINTN                                ChildIndex;
  UINTN                                HandleCount;
  EFI_HANDLE                           *HandleBuffer;
  UINTN                                *HandleType;

  ASSERT (ControllerHandle    != NULL);
  ASSERT (MatchingHandleCount != NULL);
  ASSERT (MatchingHandleBuffer    != NULL);

  HandleCount  = 0;
  HandleBuffer = NULL;
  HandleType   = NULL;

  //
  // Retrieve the list of all handles from the handle database
  //
  Status = gBS->LocateHandleBuffer (
                  AllHandles,
                  NULL,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return (Status);
  }

  HandleType = AllocateZeroPool (HandleCount * sizeof (UINTN));
  if (HandleType == NULL) {
    FreePool (HandleBuffer);
    return EFI_OUT_OF_RESOURCES;
  }

  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    //
    // Retrieve the list of all the protocols on each handle
    //
    Status = gBS->ProtocolsPerHandle (
                    HandleBuffer[HandleIndex],
                    &ProtocolGuidArray,
                    &ArrayCount
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    for (ProtocolIndex = 0; ProtocolIndex < ArrayCount; ProtocolIndex++) {
      //
      // Set the bit describing what this handle has
      //
      if (CompareGuid (ProtocolGuidArray[ProtocolIndex], &gEfiDriverBindingProtocolGuid)) {
        HandleType[HandleIndex] |= (UINTN)RELATIONSHIP_TYPE_DRIVER_BINDING_HANDLE;
      }

      Status = gBS->OpenProtocolInformation (
                      HandleBuffer[HandleIndex],
                      ProtocolGuidArray[ProtocolIndex],
                      &OpenInfo,
                      &OpenInfoCount
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }

      if (ControllerHandle != NULL) {
        if (ControllerHandle == HandleBuffer[HandleIndex]) {
          for (OpenInfoIndex = 0; OpenInfoIndex < OpenInfoCount; OpenInfoIndex++) {
            if (  ((OpenInfo[OpenInfoIndex].Attributes & EFI_OPEN_PROTOCOL_BY_DRIVER) != 0)
               || ((OpenInfo[OpenInfoIndex].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) != 0))
            {
              for (ChildIndex = 0; ChildIndex < HandleCount; ChildIndex++) {
                if (OpenInfo[OpenInfoIndex].AgentHandle == HandleBuffer[ChildIndex]) {
                  HandleType[ChildIndex] |= (UINTN)(RELATIONSHIP_TYPE_DEVICE_DRIVER);
                }
              }
            }
          }
        } else {
          for (OpenInfoIndex = 0; OpenInfoIndex < OpenInfoCount; OpenInfoIndex++) {
            if ((OpenInfo[OpenInfoIndex].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) != 0) {
              if (OpenInfo[OpenInfoIndex].ControllerHandle == ControllerHandle) {
                HandleType[HandleIndex] |= (UINTN)(RELATIONSHIP_TYPE_PARENT_HANDLE);
              }
            }
          }
        }
      }

      FreePool (OpenInfo);
    }

    FreePool (ProtocolGuidArray);
  }

  *MatchingHandleCount = 0;
  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    if ((HandleType[HandleIndex] & Mask) == Mask) {
      (*MatchingHandleCount)++;
    }
  }

  if (*MatchingHandleCount != 0) {
    Status                = EFI_SUCCESS;
    *MatchingHandleBuffer = AllocateZeroPool ((*MatchingHandleCount +1) * sizeof (EFI_HANDLE));
    *MatchingHandleCount  = 0;
    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
      if ((HandleType[HandleIndex] & Mask) == Mask) {
        (*MatchingHandleBuffer)[(*MatchingHandleCount)++] = HandleBuffer[HandleIndex];
      }
    }

    (*MatchingHandleBuffer)[*MatchingHandleCount] = NULL;
  } else {
    Status = EFI_NOT_FOUND;
  }

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  if (HandleType != NULL) {
    FreePool (HandleType);
  }

  return Status;
}

/**
  Gets the name of the device specified by the device handle.

  If EFI_DEVICE_NAME_USE_COMPONENT_NAME is set, then the function will return the
  device's name using the EFI_COMPONENT_NAME2_PROTOCOL, if present on
  DeviceHandle.

  @param DeviceHandle           The handle of the device.

  @param RetName                On return, points to the callee-allocated NULL-
                                terminated name of the device. If no device name
                                could be found, points to NULL. The name must be
                                freed by the caller...

  @retval EFI_SUCCESS           Get the name successfully.
  @retval EFI_NOT_FOUND         Fail to get the device name.
  @retval EFI_INVALID_PARAMETER DeviceHandle was NULL
**/
EFI_STATUS
EFIAPI
EfiGetControllerName (
  EFI_HANDLE  DeviceHandle,
  CHAR16      **RetName
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleList;
  UINTN                         HandleCount;
  UINTN                         LoopVar;
  CHAR16                        *DeviceNameToReturn;
  UINTN                         ParentControllerCount;
  EFI_HANDLE                    *ParentControllerBuffer;
  UINTN                         ParentDriverCount;
  EFI_HANDLE                    *ParentDriverBuffer;
  EFI_COMPONENT_NAME2_PROTOCOL  *CompName2;
  CHAR8                         *Language;

  HandleList             = NULL;
  DeviceNameToReturn     = NULL;
  ParentControllerBuffer = NULL;
  ParentDriverBuffer     = NULL;
  CompName2              = NULL;

  //
  // Go through those handles until we get one that passes for GetComponentName
  //
  Status = ScanHandleDatabaseByRelationship (
             DeviceHandle,
             RELATIONSHIP_TYPE_DRIVER_BINDING_HANDLE|RELATIONSHIP_TYPE_DEVICE_DRIVER,
             &HandleCount,
             &HandleList
             );

  if (EFI_ERROR (Status)) {
    goto CheckParent;
  }
  for (LoopVar = 0; LoopVar < HandleCount; LoopVar++) {
    Status = gBS->HandleProtocol (
                    HandleList[LoopVar],
                    &gEfiComponentName2ProtocolGuid,
                    (VOID **)&CompName2
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Language = GetBestLanguage (CompName2->SupportedLanguages, 0, "en", "en-US");
    Status   = CompName2->GetControllerName (
                            CompName2,
                            DeviceHandle,
                            NULL,
                            Language,
                            &DeviceNameToReturn
                            );
    if (Language != NULL) {
      FreePool (Language);
    }

    if (!EFI_ERROR (Status) && (DeviceNameToReturn != NULL)) {
      goto Exit;
      break;
    }
  }

CheckParent:
  //
  // Now check the parent controller using this as the child.
  //
  Status = ScanHandleDatabaseByRelationship (
    DeviceHandle,
    RELATIONSHIP_TYPE_PARENT_HANDLE,
    &ParentControllerCount,
    &ParentControllerBuffer
    );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }
  for (LoopVar = 0; LoopVar < ParentControllerCount; LoopVar++) {
    Status = ScanHandleDatabaseByRelationship (
      ParentControllerBuffer[LoopVar],
      RELATIONSHIP_TYPE_DRIVER_BINDING_HANDLE|RELATIONSHIP_TYPE_DEVICE_DRIVER,
      &ParentDriverCount,
      &ParentDriverBuffer
      );
    if (EFI_ERROR (Status)) {
      continue;
    }
    for (HandleCount = 0; HandleCount < ParentDriverCount; HandleCount++) {
      Status = gBS->HandleProtocol (
                      ParentDriverBuffer[HandleCount],
                      &gEfiComponentName2ProtocolGuid,
                      (VOID **)&CompName2
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }

      Language = GetBestLanguage (CompName2->SupportedLanguages, 0, "en", "en-US");
      Status   = CompName2->GetControllerName (
                              CompName2,
                              ParentControllerBuffer[LoopVar],
                              DeviceHandle,
                              Language,
                              &DeviceNameToReturn
                              );
      if (Language != NULL) {
        FreePool (Language);
      }

      if (!EFI_ERROR (Status) && (DeviceNameToReturn != NULL)) {
        goto Exit;
        break;
      }
    }

    if (ParentDriverBuffer != NULL) {
      FreePool (ParentDriverBuffer);
      ParentDriverBuffer = NULL;
    }
  }

Exit:
  if (HandleList != NULL) {
    FreePool (HandleList);
  }

  if (ParentDriverBuffer != NULL) {
    FreePool (ParentDriverBuffer);
  }

  if (ParentControllerBuffer != NULL) {
    FreePool (ParentControllerBuffer);
  }

  if (DeviceNameToReturn != NULL) {
    *RetName = AllocateCopyPool (StrSize (DeviceNameToReturn), DeviceNameToReturn);
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}
