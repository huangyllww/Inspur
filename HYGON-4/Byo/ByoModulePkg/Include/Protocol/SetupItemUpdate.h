/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __SETUP_ITEM_UPDATE_H__
#define __SETUP_ITEM_UPDATE_H__

#include <Guid/SetupItems.h>

// {AD49D841-F622-49D9-80B4-CD8250B96A8E}
#define SETUP_ITEM_UPDATE_PROTOCOL_GUID \
  {0xad49d841, 0xf622, 0x49d9, {0x80, 0xb4, 0xcd, 0x82, 0x50, 0xb9, 0x6a, 0x8e}}

typedef struct _SETUP_ITEM_UPDATE_PROTOCOL  SETUP_ITEM_UPDATE_PROTOCOL;

#define SETUP_UPDATE_TYPE_MIN_VALUE     0x01
#define SETUP_UPDATE_TYPE_MAX_VALUE     0x02
#define SETUP_UPDATE_TYPE_DEAULT_VALUE  0x03
#define SETUP_UPDATE_TYPE_ATTRIBUTE     0x04
#define SETUP_UPDATE_TYPE_OPTION_VALUE  0x05

///
/// Config Option can't be exposed if its attribute is invisible
///
#define SETUP_UPDATE_TYPE_ATTRIBUT_INVISIABLE 0x01
///
/// Config Option is read only. Its value can't be changed. 
///
#define SETUP_UPDATE_TYPE_ATTRIBUT_READONLY   0x02

/**
  Update the minimal value of the numeric option value.
  The option is for one variable storage decided by the input variable name and guid.
  Its value starts at offset to the variable storage.

  @param  VariableName       A Null-terminated string that is the name of the vendor's variable.
  @param  VendorGuid         A unique identifier for the vendor.
  @param  Offset             Option offset to the variable storage.
  @param  Value              The updated minimal value.

  @retval EFI_SUCCESS    Update the minimal value of option successfully.
  @retval EFI_NOT_FOUND  Don't find the matched option.
**/
typedef
EFI_STATUS
(EFIAPI *SETUP_ITEM_UPDATE_MIN_VALUE) (
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  );

/**
  Update the max value of the numeric option value.
  The option is for one variable storage decided by the input variable name and guid.
  Its value starts at offset to the variable storage.

  @param  VariableName       A Null-terminated string that is the name of the vendor's variable.
  @param  VendorGuid         A unique identifier for the vendor.
  @param  Offset             Option offset to the variable storage.
  @param  Value              The updated max value.

  @retval EFI_SUCCESS    Update the max value of option successfully.
  @retval EFI_NOT_FOUND  Don't find the matched option.
**/
typedef
EFI_STATUS
(EFIAPI *SETUP_ITEM_UPDATE_MAX_VALUE) (
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  );

/**
  Update the default value of the option value.
  The option is for one variable storage decided by the input variable name and guid.
  Its value starts at offset to the variable storage.

  @param  VariableName       A Null-terminated string that is the name of the vendor's variable.
  @param  VendorGuid         A unique identifier for the vendor.
  @param  Offset             Option offset to the variable storage.
  @param  Value              The updated default value.

  @retval EFI_SUCCESS    Update the default value of option successfully.
  @retval EFI_NOT_FOUND  Don't find the matched option.
**/
typedef
EFI_STATUS
(EFIAPI *SETUP_ITEM_UPDATE_DEFAULT_VALUE) (
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  );

/**
  Update the attribute (invisible or readonly) of the option value.
  The option is for one variable storage decided by the input variable name and guid.
  Its value starts at offset to the variable storage.

  @param  VariableName       A Null-terminated string that is the name of the vendor's variable.
  @param  VendorGuid         A unique identifier for the vendor.
  @param  Offset             Option offset to the variable storage.
  @param  Value              The updated value attribute.

  @retval EFI_SUCCESS    Update the option attribute successfully.
  @retval EFI_NOT_FOUND  Don't find the matched option.
**/
typedef
EFI_STATUS
(EFIAPI *SETUP_ITEM_UPDATE_ATTRIBUTE) (
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT16                       Attribute
  );

/**
  Update the option value of the oneof option.
  The option is for one variable storage decided by the input variable name and guid.
  Its value starts at offset to the variable storage.

  @param  VariableName       A Null-terminated string that is the name of the vendor's variable.
  @param  VendorGuid         A unique identifier for the vendor.
  @param  Offset             Option offset to the variable storage.
  @param  Count              Count of the oneof options will be used. 
  @param  OptionValueArray   Value of the oneof options will be used. There are count of values in this array.

  @retval EFI_SUCCESS    Update the minimal value of option successfully.
  @retval EFI_NOT_FOUND  Don't find the matched option.
**/
typedef
EFI_STATUS
(EFIAPI *SETUP_ITEM_UPDATE_OPTION_VALUE) (
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINTN                        Count,
  IN UINT64                       *OptionValueArray
  );

//
// New DXE driver will be added to install this protocol.
// This driver locates setup DB. The updated option must be in setup DB.
//
// On ready to boot event, for X86 platform, DXE driver will report all 
// updated option to SmiFlash module by trig smi, SmiFlash will base on 
// the updated option and update its internal OPTIONS_DATA.
//
// On ready to boot event, for non X86 platform, DXE driver will allocate
// reserved memory to store the updated option info, and set this memory 
// address and size to SMBIOS_BYOTOOL_TABLE MemoryAddress and MemorySize.
// ByoCfg tool needs to base on SMBIOS_BYOTOOL_TABLE to update option data.
//
struct _SETUP_ITEM_UPDATE_PROTOCOL {
  SETUP_ITEM_UPDATE_MIN_VALUE      UpdateMinValue;
  SETUP_ITEM_UPDATE_MAX_VALUE      UpdateMaxValue;
  SETUP_ITEM_UPDATE_DEFAULT_VALUE  UpdateDefaultValue;
  SETUP_ITEM_UPDATE_ATTRIBUTE      UpdateAttribute;
  SETUP_ITEM_UPDATE_OPTION_VALUE   UpdateOptionValue;
};

extern EFI_GUID gSetupItemUpdateProtocolGuid;
  
#endif