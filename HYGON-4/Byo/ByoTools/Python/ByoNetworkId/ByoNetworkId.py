## @file  
# 
# Copyright (c) 2023, Byosoft Corporation.<BR>
# All rights reserved.This software and associated documentation (if any)
# is furnished under a license and may only be used or copied in
# accordance with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be reproduced,
# stored in a retrieval system, or transmitted in any form or by any
# means without the express written consent of Byosoft Corporation.
# 
##

# !/usr/bin/env python3
# -*- coding: utf-8 -*-

import re
import uuid
import pathlib
import argparse

__copyright__ = 'Copyright (c) 2023, Byosoft Corporation. All rights reserved.'
__description__ = 'Byo Network Identifier tool\n'

FILE = ""


#
# Accroding to file path load dhcpd.conf file
#
def load_cfg(file=FILE):
    with open(file, "r") as _f:
        return _f.read()


#
# Resolve dhcpd.conf information in segments for add or update NetworkId.
#
def str_split(pattern, strs):
    split_list = []
    split_tmp = []
    sub_node = 0
    sub_node_last = sub_node
    lines = strs.splitlines()
    for index, line in enumerate(lines):
        if re.search(f"{pattern}", line) and not line.lstrip().startswith("#"):  # first match line
            sub_node += 1
            split_list.append("".join(split_tmp))
        if sub_node != sub_node_last:  # new match line
            split_tmp = [f"{line}\n"]
        else:
            split_tmp.append(f"{line}\n")  # exists match node
        if index == len(lines) - 1:  # 0 ~ line-1
            split_list.append("".join(split_tmp))
        sub_node_last = sub_node
    return split_list


#
# Resolve dhcpd.conf with subnets
#
def subnet_split(cfg_str):
    return str_split("subnet .+ \{", cfg_str)


#
# Resolve dhcpd.conf with class in each subnet
#
def class_split(subnet_str):
    return str_split("class \".+\" \{", subnet_str)


#
# Get the uuid from user input
#
def get_user_uuid(Arg):
    len_uuid = hex(len(Arg.uuid))
    if len_uuid != hex(32):
        print("\033[31m The uuid length does not meet requirements!!\033[0m")
    else:
        uuid_str = f"\\x20{Arg.uuid}"
        print(f"Input UUID: {Arg.uuid}")
        print(f"NetworkId : {uuid_str}")
        return uuid_str


#
# Generate a NetworkId uuid
#
def get_networkid_uuid(Arg):
    if Arg.uuid:
        uuid_str = get_user_uuid(Arg)
        return uuid_str
    else:
        uuid_hex = uuid.uuid4().hex
        uuid_str = f"\\x20{uuid_hex}"
        print(f"Generate UUID: {uuid_hex}")
        print(f"NetworkId    : {uuid_str}")
        return uuid_str


#
# Update networkclient config in each subnet,
# If the NetworkId already exists in current local area networks server, update it; otherwise, add a new network NetworkId.
#
def update_networkclients(uuid_str, subnet_str):
    class_name = f'class "networkclients"'
    tmp_str = f"""{class_name} {{
        match if substring (option vendor-class-identifier, 0, 15) = "$DhcpNetworkID$";
        ##Report a OEM-Specific option type 0xAA with content GUID to identify this network
        option dhcp-option-overload 1;
        filename \"\\xAA{uuid_str}\\xFF\";
    }}"""
    if class_name in subnet_str:
        str_list = []
        for sp in re.split(f'({class_name} {{.*?}})', subnet_str, flags=re.S):
            if class_name not in sp:
                str_list.append(sp)
            else:
                str_list.append(tmp_str)
        str_new = "".join(str_list)
    else:
        str_list = []
        open_brace_count = 0
        close_brace_count = 0
        last_brace_index = 0
        for index, line in enumerate(subnet_str.splitlines()):
            #
            # Skip comment line
            #
            if line.lstrip().startswith("#"):
                str_list.append(f"{line}\n")
                continue

            line_without_sharp = line[:line.find("#")] if ("#" in line) else line
            open_brace_count += line_without_sharp.count("{")
            close_brace_count -= line_without_sharp.count("}")
            if open_brace_count and open_brace_count + close_brace_count == 0:
                str_list.append(line[:line.rfind("}")] + "\n")
                str_list.append(line[line.rfind("}"):] + "\n")
                last_brace_index = index + 1
                open_brace_count = 0
                continue
            str_list.append(f"{line}\n")

        if last_brace_index:
            str_list.insert(last_brace_index, f"    {tmp_str}\n")
        str_new = "".join(str_list)
    return str_new


#
# Traversal all the subnet text segments,for Update networkclient config in each existing subnet.
#
def update_dhcpd_conf_file(strs_old, Arg):
    sub_nets = subnet_split(strs_old)
    cfg_str = []
    uuid_str = get_networkid_uuid(Arg)
    for sp in sub_nets:
        cfg_str.append(update_networkclients(uuid_str, sp))
    return "".join(cfg_str)


def main():
    parser = argparse.ArgumentParser(description=__copyright__ + __description__)
    parser.add_argument("-u", "--uuid", action="store", nargs='?', type=str, help="Enter the 32-bit uuid")
    parser.add_argument("-f", "--file", action="store", nargs='?', type=str, help="Enter the path of config file")
    Arg = parser.parse_args()

    if Arg.file:
        FILE = Arg.file
    else:
        print("\033[31m Please input file path See the help information for details(ByoNetworkId.py -h)!!\033[0m")
        exit()

    try:
        with open(FILE, "r") as data:
            print("\nOpen the file succeed")
    except IOError as err:
        print("\033\n[31mFile path error!!\033[0m")
        exit()

    cfg_old = load_cfg(FILE)
    cfg_name = pathlib.Path(FILE).stem
    cfg_sufix = pathlib.Path(FILE).suffix

    # save dhcpd config file to dhcpd_backup.conf. before updating it.
    if not cfg_old:
        print(f"Invalid File: {FILE}")
        return
    with open(f"{cfg_name}_backup{cfg_sufix}", "w") as _backup:
        _backup.write(cfg_old)

    new_cfg = update_dhcpd_conf_file(cfg_old, Arg)
    if not new_cfg:
        print(f"Fail to generate new file: {FILE}")
        return
    with open(f"{FILE}", "w") as _new:
        _new.write(new_cfg)


if __name__ == '__main__':
    main()
