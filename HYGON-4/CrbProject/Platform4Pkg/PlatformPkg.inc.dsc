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
#  	PlatformPkg.inc.dsc
#
#	Abstract:
#  	ByoSoft PlaformPkg Module Package include File
#
##

[LibraryClasses.common]
  DebugPrintErrorLevelLib|$(PLATFORM_PACKAGE)/Library/DebugPrintErrorLevelLibCmos/DebugPrintErrorLevelLibCmos.inf
  PlatformBootManagerLib|$(PLATFORM_PACKAGE)/Library/PlatformBootManagerLib/PlatformBootManagerLib.inf
  ResetSystemLib|$(PLATFORM_PACKAGE)/Library/ResetSystemLib/ResetSystemLib.inf  
  TimerLib|$(PLATFORM_PACKAGE)/Library/CpuTimerLib/TscTimerLib.inf
  SmmLib|$(PLATFORM_PACKAGE)/Library/SmmLib/SmmLib.inf
  BeepLib|$(PLATFORM_PACKAGE)/Library/BeepLib/BeepLib.inf
  PlatformCommLib|$(PLATFORM_PACKAGE)/Library/PlatformCommLib/PlatformCommLib.inf
  RasElogEventLib|$(PLATFORM_PACKAGE)/Library/RasElogEventLib/RasElogEventLib.inf
  TimestampLib|$(PLATFORM_PACKAGE)/Library/TimestampLib/Timestamp.inf



[LibraryClasses.X64.DXE_SMM_DRIVER]
  PlatformSyncMmLib|$(PLATFORM_PACKAGE)/Library/PlatformSyncMmLib/PlatformSyncMmLib.inf
  