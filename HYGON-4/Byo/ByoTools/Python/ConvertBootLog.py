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

import sys
import os
import re
import argparse


def guid_map(guid_xref_file):
    """
        Parser Guid.xref file.
    """

    guid_xref_dict = {}

    try:
        with open(guid_xref_file, 'r') as f:
            data = f.readlines()

            for line in data:
                if line.strip():
                    if len(line.strip().split()) < 2:
                        raise Exception('input Guid.xref file is invalid!')

            for line in data:
                if line.strip():
                    line_list = line.strip().split()
                    guid_xref_dict[line_list[0].lower()] = line_list[1]
    except Exception as er:
        print(er)
        sys.exit(1)
    else:
        return guid_xref_dict


def convert_log(guid_xref_file, boot_log):
    """
        pattern "InstallProtocolInterface: ***-" from log file, and replace it.
    """

    guid_dict = guid_map(guid_xref_file)
    pattern = re.compile(r"(([a-zA-Z0-9]+-){4}[a-zA-Z0-9]+).*")
    new_file_data = ""

    try:
        with open(boot_log, 'r', encoding="utf-8") as f:
            while True:
                data = f.readline()
                if not data:
                    break
                else:
                    result = pattern.findall(data)
                    if result:
                        for guid_string in result:
                            if guid_string[0].lower() in guid_dict.keys():
                                data = data.replace(guid_string[0], guid_dict[guid_string[0].lower()])
                new_file_data += data
    except Exception as e:
        print(e)

    try:
        with open(boot_log, 'w', encoding="utf-8") as f:
            f.write(new_file_data)
    except Exception as e:
        print(e)


def main():
    __usage = "python ConvertBootLog.py -g Guid.xref -l Assert.log"
    parser = argparse.ArgumentParser(description="Convert GUID value to GuidCName in boot log.", usage=__usage)
    parser.add_argument('-g', required=True, help='input guid file')
    parser.add_argument('-l', required=True, help='input log file')
    args = parser.parse_args()

    abs_guid_path = os.path.abspath(args.g)
    abs_log_path = os.path.abspath(args.l)

    try:
        convert_log(abs_guid_path, abs_log_path)
    except Exception as e:
        print(e)
        sys.exit(5)
    else:
        print('Convert GUID value to GuidCName, file is: {}'.format(abs_log_path))


if __name__ == '__main__':
    main()
