## @file  
# 
# Copyright (c) 2022, Byosoft Corporation.<BR>
# All rights reserved.This software and associated documentation (if any)
# is furnished under a license and may only be used or copied in
# accordance with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be reproduced,
# stored in a retrieval system, or transmitted in any form or by any
# means without the express written consent of Byosoft Corporation.
# 
##
#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import re
import argparse
import sys
__copyright__   = 'Copyright (c) 2022, Byosoft Corporation. All rights reserved.'
__description__ = 'Check for spelling problems in the file.\n'
flag = 0
prefixes = ['File', 'Message', 'Arguments', 'Specifier Count', 'Argument Count']
regex = re.compile(r'Unknown word')

def check_spell(log):
    global flag
    with open(log,encoding='utf-8',errors='ignore') as f:
        for line in f:
            result = regex.findall(line)
            if result:
                flag = 1
                print(line)
    if flag == 1:
        return 1
    
def check_debug(log):
    global flag
    with open(log,encoding='utf-8',errors='ignore') as f:
        for line in f:
            if any(line.lstrip().startswith(prefix) for prefix in prefixes):
                flag = 1
                print(line.strip())
    if flag == 1:
        return 2


def main():
    parser = argparse.ArgumentParser(description=__description__ + __copyright__)
    parser.add_argument('-p','--Path', nargs='+',
                        help='Files to check.')
    parser.add_argument('-q', '--quiet', dest='Quiet', action='store_true',
                        help='reduce output messages')
    parser.add_argument('--debug', dest='Debug', type=int, metavar='[0-9]', choices=range(0, 10), default=0,
                        help='set debug level')
    parser.add_argument("-D", "--define", action="append", dest="Macros", help='use to judge which function to run".')

    args = parser.parse_args()

    for File in args.Path:
        if not os.path.exists(File):
            print("not exists path: {0}".format(File))
            sys.exit(1)
        if os.path.isfile(File):
            if args.Macros is not None:
                if "Debug" in args.Macros:
                    ExitCode = check_debug(File)
            else:
                ExitCode = check_spell(File)
        elif os.path.isdir(File):
            print("The input file name cannot be a folder: {0}".format(File))
    if ExitCode == 1:
        print("error:There are spelling mistakes in the document")
        return ExitCode
    elif ExitCode == 2:
        print("error:There are debug macro errors in the document")
        return ExitCode
    else:
        return 0

if __name__ == "__main__":
    sys.exit(main())