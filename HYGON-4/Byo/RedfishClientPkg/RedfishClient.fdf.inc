## @file
# Redfish Client FDF include file for [FV*] section of all Architectures.
#
# This file can be included to the [FV*] section(s) of a platform FDF file
# by using "!include RedfishClientPkg/RedfisClientLibs.fdf.inc" to specify the module instances
# to be built in the firmware volume.
#
# (C) Copyright 2021 Hewlett Packard Enterprise Development LP<BR>
#
#    SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
INF  RedfishPkg/RestJsonStructureDxe/RestJsonStructureDxe.inf
INF  RedfishClientPkg/Converter/Bios/v1_0_0/RedfishBios_V1_0_0_Dxe.inf
INF  RedfishClientPkg/Converter/AttributeRegistry/v1_0_0/RedfishAttributeRegistry_V1_0_0_Dxe.inf
INF  RedfishClientPkg/BiosJsonBmcDxe/BiosJsonBmcDxe.inf

INF  RuleOverride = JSON RedfishClientPkg/BiosJsonFile/BiosJson.inf
