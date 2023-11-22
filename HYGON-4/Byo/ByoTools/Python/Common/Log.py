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
import logging

class BuildLog(object):

    def __init__(self, LogFile=''):
        # Define a Handler which writes INFO messages or higher to the sys.stderr
        self.log = logging.getLogger()
        Console = logging.StreamHandler()
        Console.setLevel(logging.INFO)
        self.log.addHandler(Console)

        if LogFile:
            FileHandler = logging.FileHandler(LogFile, 'w')
            FileHandler.setLevel(logging.INFO)
            self.log.addHandler(FileHandler)
        self.log.setLevel(logging.INFO)

def main():
    build_log = BuildLog("test.log")
    build_log.log.info("log successfully, totally!")

if __name__ == '__main__':
    main()

