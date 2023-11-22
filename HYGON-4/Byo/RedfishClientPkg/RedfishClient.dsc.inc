## @file
# Redfish Client DSC include file for Platform DSC
#
# This file includes all required information to enable Redfish features.
# It can be included to a platform DSC file by using "!include RedfishClientPkg/RedfishClient.dsc.inc".
#
# (C) Copyright 2021 Hewlett Packard Enterprise Development LP<BR>
#
#    SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  RedfishPkg/RestJsonStructureDxe/RestJsonStructureDxe.inf
  RedfishClientPkg/Converter/Bios/v1_0_0/RedfishBios_V1_0_0_Dxe.inf
  RedfishClientPkg/Converter/AttributeRegistry/v1_0_0/RedfishAttributeRegistry_V1_0_0_Dxe.inf
  RedfishClientPkg/BiosJsonBmcDxe/BiosJsonBmcDxe.inf {
    <LibraryClasses>
    FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  }
  RedfishClientPkg/BiosJsonConvertApp/BiosJsonConvertApp.inf

  RedfishClientPkg/BiosJsonFile/BiosJson.inf

