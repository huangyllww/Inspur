## @file
# EFI/PI Reference Module Package for All Architectures
#
# Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
# All rights reserved.This software and associated documentation (if any)
# is furnished under a license and may only be used or copied in
# accordance with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be reproduced,
# stored in a retrieval system, or transmitted in any form or by any
# means without the express written consent of Byosoft Corporation.
#
#	File Name:
#  	ByoHgpiModulePkg.inc.dsc
#
#	Abstract:
#  	ByoSoft PlaformPkg Module Package include File
#
##


[LibraryClasses.common]
  ByoSharedSmmDataLib|ByoHgpiModulePkg/Library/ByoSharedSmmDataLib/ByoSharedSmmDataLib.inf
  ByoHygonFabricLib|ByoHgpiModulePkg/Library/ByoHygonFabricLib/ByoHygonFabricLib.inf
  ByoHygonCommLib|ByoHgpiModulePkg/Library/ByoHygonCommLib/ByoHygonCommLib.inf
  ByoHygonGpioLib|ByoHgpiModulePkg/Library/ByoHygonGpioLib/ByoHygonGpioLib.inf
  
  

[LibraryClasses.X64.DXE_SMM_DRIVER]
  HygonPspFlashAccLib|ByoHgpiModulePkg/Library/HygonPspFlashAccLibSmm/HygonPspFlashAccLibSmm.inf
  CbsSyncMmLib|HgpiCbsPkg/Library/CbsSyncMmLib.inf

