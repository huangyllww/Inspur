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
import copy
import argparse
import sys
__copyright__   = 'Copyright (c) 2022, Byosoft Corporation. All rights reserved.'
__description__ = 'Check License and CopyRight header in files.\n'
str = 'without'
signal = 0
DEFAULT_EXT_LIST = ['.h', '.c', '.asm', '.asm16', '.S', '.inf', '.dec', '.dsc', '.uni', '.vfr', '.Vfr', '.idf', '.py']

def check_message(path):
        count = check_line(path)
        flag = 0
        flag1 = 0
        flag2 = 0
        flag3 = 0
        with open(path,encoding='utf-8',errors='ignore') as f:
            lines = [line for line in f][:count]
            for Test in lines:
                pattern1 = re.compile(r"Copyright[ ]*[(*c*)*[ ]*[0-9]{4}[ ]*-[ ]*[0-9]{4}[ ]*,*[ ]*Byosoft[ ]+[a-zA-Z]*|Copyright[ ]+[(]c[)][ ]+[0-9]{4}[ ]*,*[ ]*Byosoft[ ]+[a-zA-Z]*",re.I)
                pattern2 = re.compile(r"Copyright[ ]*[(*c*)*[ ]*[0-9]{4}[ ]*-[ ]*[0-9]{4}[ ]*,*[ ]*Intel[ ]+[a-zA-Z]*|Copyright[ ]+[(]c[)][ ]+[0-9]{4}[ ]*,*[ ]*Intel[ ]+[a-zA-Z]*",re.I)
                result1 = pattern1.findall(Test)
                result2 = pattern2.findall(Test)
                if str in Test:
                    Test1 = Test
                    flag = 1
                    continue
                if flag == 1:
                    Test = Test1 + Test
                    pattern3_1 = re.compile(r"without.*[\n]*.*Byosoft.*[\n]*.*Corporation",re.I)
                    pattern3_2 = re.compile(r"without.*[\n]*.*Byosoft.*[\n]*.*permission",re.I)
                    result3_1 = pattern3_1.findall(Test)
                    result3_2 = pattern3_2.findall(Test)
                    if result3_1 or result3_2:
                        flag3 = 1
                    flag = 0
                if result1:
                    flag1 = 1
                if result2:
                    flag2 = 1
        if flag3 == 0 or flag1 == 0 or flag2 == 1:
            print(path)
            return 1

def check_if_dir(file_path, ExtList):
    global signal 
    temp_list = os.listdir(file_path)
    for temp_list_each in temp_list:
        if os.path.isfile(file_path + '/' +temp_list_each):
            temp_path = file_path + '/' + temp_list_each
            if (file_name_suffix(temp_path, ExtList)):
                ExitCode = check_message(temp_path)
                if ExitCode == 1:
                    signal = 1
            else:
                continue
        else:
            check_if_dir(file_path + '/' + temp_list_each, ExtList)
    if signal == 1:
        return 1

def check_line(file_path):
    with open(file_path,encoding='utf-8-sig',errors='ignore') as f:
        line = f.readline()
        count = 0
        flag = 0
        while line:
            rtn_result1 = line.startswith("#")
            rtn_result1_1 = line.startswith("#ifndef")
            rtn_result1_2 = line.startswith("#define")
            rtn_result1_3 = line.startswith("#include")
            rtn_result1_4 = line.startswith("#langdef")
            rtn_result1_5 = line.startswith("#string")
            rtn_result1_6 = line.startswith("#language")
            rtn_result2 = line.startswith("/")
            rtn_result2_1 = line.startswith("/*")
            rtn_result2_2 = line.endswith("*/\n")
            rtn_result3 = line.startswith("\n")
            rtn_result4 = line.startswith(" ")
            rtn_result5 = line.startswith(";")
            if rtn_result1:
                if rtn_result1_1 or rtn_result1_2 or rtn_result1_3 or rtn_result1_4 or rtn_result1_5 or rtn_result1_6:
                    break
                else:
                    line = f.readline()
                    count = count +1
            elif rtn_result2:
                if rtn_result2_1:
                    flag = 1
                    line = f.readline()
                    count = count + 1
                else:
                    line = f.readline()
                    count = count + 1
            elif flag == 1:
                if rtn_result2_2:
                    flag = 0
                    line = f.readline()
                    count = count + 1
                else:
                    line = f.readline()
                    count = count + 1
            elif rtn_result3:
                line = f.readline()
                count = count +1
            elif rtn_result4:
                line = f.readline()
                count = count+1
            elif rtn_result5:
                line = f.readline()
                count = count+1
            else:
                break
    return count

def file_name_suffix(path,ExtList):
    flag = 0
    for ext in ExtList:
        if path.endswith(ext):
            flag = 1
    if flag == 1:
        return True
    else:
        return False

def main():
    parser = argparse.ArgumentParser(description=__description__ + __copyright__)
    parser.add_argument('-p','--Path', nargs='+',
                        help='the path for files to be checked.It could be directory or file path.')
    parser.add_argument('--append-extensions', dest='AppendExt', nargs='+',
                        help='append file extensions filter to default extensions. (Example: .c .h)')
    parser.add_argument('--override-extensions', dest='OverrideExt', nargs='+',
                        help='override file extensions filter on default extensions. (Example: .c .h)')
    parser.add_argument('-q', '--quiet', dest='Quiet', action='store_true',
                        help='reduce output messages')
    parser.add_argument('--debug', dest='Debug', type=int, metavar='[0-9]', choices=range(0, 10), default=0,
                        help='set debug level')
    args = parser.parse_args()
    DefaultExt = copy.copy(DEFAULT_EXT_LIST)

    if args.OverrideExt is not None:
        DefaultExt = args.OverrideExt
    if args.AppendExt is not None:
        DefaultExt = list(set(DefaultExt + args.AppendExt))

    for Path in args.Path:
        if not os.path.exists(Path):
            print("not exists path: {0}".format(Path))
            sys.exit(1)
        if os.path.isfile(Path):
            ExitCode = check_message(Path)
        elif os.path.isdir(Path):
            ExitCode = check_if_dir(Path,DefaultExt)
    if ExitCode == 1:
        print("error:Byo license in the listed files are incorrect or missing")
        return ExitCode
    else:
        return 0

if __name__ == "__main__":
    sys.exit(main())