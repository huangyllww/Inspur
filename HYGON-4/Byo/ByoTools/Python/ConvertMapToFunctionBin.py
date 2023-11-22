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
import struct


def convert_map(input_file, output_file):
    """
    Convert map file to the binary data.
    Binary Data will be a group of MODULE_INFO.
    """

    pattern_begin = re.compile(r'^[a-zA-Z0-9]+\s+\(')
    pattern_offset = re.compile(r'\s+0x')
    num_list = []
    binary_data = b''

    # module struct.pack, return bytes.
    def module_do_pack(m_name, m_guid, f_sum):
        __one_module = b''
        module_name_len = len(m_name) + 1

        def pack_guid(guid_split):
            return struct.pack('=L2H8B',
                               int(guid_split[0], 16),
                               int(guid_split[1], 16),
                               int(guid_split[2], 16),
                               int(guid_split[3][-4:-2], 16),
                               int(guid_split[3][-2:], 16),
                               int(guid_split[4][-12:-10], 16),
                               int(guid_split[4][-10:-8], 16),
                               int(guid_split[4][-8:-6], 16),
                               int(guid_split[4][-6:-4], 16),
                               int(guid_split[4][-4:-2], 16),
                               int(guid_split[4][-2:], 16),
                               )

        try:
            __module_binary_data = struct.pack('={}s'.format(module_name_len), m_name.encode())
            # __guid_binary_data = pack_guid(m_guid.split('-'))
            __sum_binary_data = struct.pack('=L', f_sum)
        except Exception as e:
            print(e)
            sys.exit(2)
        else:
            # __one_module = __one_module + __module_binary_data + __guid_binary_data + __sum_binary_data
            __one_module = __one_module + __module_binary_data + __sum_binary_data
            return __one_module

    # function struct.pack, return bytes.
    def fun_do_pack(f_name, f_offset):

        if f_name:
            f_name_len = len(f_name) + 1

        return struct.pack('=L{}s'.format(f_name_len),
                           int(f_offset, 16),
                           f_name.encode()
                           )

    # open map file, for module and function, return all bytes for map file.
    try:
        with open(input_file, 'r') as f:
            data = f.readlines()

            # re Address append index in num_list List.
            for index, line in enumerate(data):
                if pattern_begin.match(line):
                    num_list.append(index)

            if not num_list:
                raise Exception('the input map file is invalid')
            
            # for num_list, do module and function.
            try:
                for index in num_list:
                    # do module
                    module_name = data[index].split(" ")[0]
                    module_address = data[index].split('BaseAddress=')[1].split(',')[0]
                    module_guid = data[index + 1].split(" ")[0].split('=')[1]

                    # do next index for num_list
                    # do function
                    if index == num_list[-1]:
                        next_index = len(data)
                    else:
                        next_index = num_list.index(index) + 1
                        next_index = num_list[next_index]

                    fun_sum = 0
                    fun_binary_data = b''

                    for i in range(index, next_index):
                        if pattern_offset.match(data[i]):
                            fun_offset = data[i].split()[0]
                            fun_offset = int(fun_offset, 16) - int(module_address, 16)
                            fun_offset = "{:#012x}".format(fun_offset)
                            fun_name = data[i].split()[1]

                            fun_sum = fun_sum + 1

                            # one module, add every function (fun_name, fun_offset) into bytes.
                            fun_binary_data = fun_binary_data + fun_do_pack(fun_name, fun_offset)

                    # add one module into bytes.
                    module_binary_data = module_do_pack(module_name, module_guid, fun_sum)
                    # every module and function, add bytes.
                    binary_data = binary_data + module_binary_data + fun_binary_data

                    continue
            except Exception as err:
                print(err)
                sys.exit(3)
            else:
                try:
                    with open(output_file, 'wb') as file:
                        file.write(binary_data)
                except IOError:
                    raise IOError('the output file {} can not be found'.format(output_file))
    except IOError:
        raise IOError('the input map {} can not be found'.format(input_file))
    except Exception as er:
        print(er)
        sys.exit(1)


def main():
    __usage = "python module_info.py -i INPUT_FILE -o OUTPUT_FILE"
    parser = argparse.ArgumentParser(description="Convert Map File To Binary Data File.", usage=__usage)
    parser.add_argument('-i', required=True, help='input a file')
    parser.add_argument('-o', required=True, help='output a file')
    args = parser.parse_args()

    abs_input_path = os.path.abspath(args.i)
    abs_output_path = os.path.abspath(args.o)

    try:
        convert_map(abs_input_path, abs_output_path)
    except Exception as e:
        print(e)
        sys.exit(5)
    else:
        print('Convert map file to {}'.format(abs_output_path))


if __name__ == '__main__':
    main()
