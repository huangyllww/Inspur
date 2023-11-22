#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
## @file
#
# Copyright (c) 2020, Byosoft Corporation.<BR>
# All rights reserved.This software and associated documentation (if any)
# is furnished under a license and may only be used or copied in
# accordance with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be reproduced,
# stored in a retrieval system, or transmitted in any form or by any
# means without the express written consent of Byosoft Corporation.
#

##
# Import Modules
#

Prog = 'PlatformBuild.py'
Copyright = 'Copyright (c) 2020, Byosoft Corporation. All rights reserved.'
Description = 'Platform build.\n'

COMMAND_SUCCESS = 0
FILE_NOT_FOUND  = 1
COMMAND_FAILURE = 2
UNKNOWN_ERROR   = 255

gErrorMessage = {
    FILE_NOT_FOUND          :   "File/directory not found in workspace",
    COMMAND_FAILURE         :   "Failed to execute command",
    UNKNOWN_ERROR           :   "Unknown error"
}