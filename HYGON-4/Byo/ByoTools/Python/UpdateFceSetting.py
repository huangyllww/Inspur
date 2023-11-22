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

# -*- coding: UTF-8 -*-
# description: Update FceRead.out based on input Cfg.log.

import sys
import os
import re
import argparse
import codecs

__reg_cfg_file = re.compile(r'.+\s*=\s*(.+);\s*(\(.*\))\s*(.+)')
__reg_fce_file = re.compile(r'Q\s+(.+)\s+//\s*(.+)')

def UpdateFceRead(cfg_file, fce_file):
    print('Read and Update file...')

    scr_data = None
    fce_file_dir = os.path.dirname(fce_file)
    tmp_fce_file = os.path.join(fce_file_dir, 'tmp-fce-read-file')

    # 检测文件编码
    try:
        with open(cfg_file, 'rb') as f:
            text_cfg = f.read()
            #
            # Detect UTF-16 Byte Order Mark at beginning of file.
            #
            bom = (text_cfg.startswith(codecs.BOM_UTF16_BE) or text_cfg.startswith(codecs.BOM_UTF16_LE))
            if bom:
                code_format_cfg_file = 'UTF-16'
            else:
                code_format_cfg_file = 'utf-8'


        with open(fce_file, 'rb') as fce:
            text_fce = fce.read()
            bom = (text_fce.startswith(codecs.BOM_UTF16_BE) or text_fce.startswith(codecs.BOM_UTF16_LE))
            if bom:
                code_format_fce_file = 'UTF-16'
            else:
                code_format_fce_file = 'utf-8'
    except Exception as e:
        print(e)

    # 查找修改
    try:
        with open(fce_file, 'r+', encoding=code_format_fce_file) as fce:
            scr_data = fce.readlines()

            with open(cfg_file, 'r', encoding=code_format_cfg_file) as cfg:
                for line in cfg.readlines():
                    cfg_reg_result = __reg_cfg_file.findall(line)

                    if cfg_reg_result:
                        for index,item in enumerate(scr_data):
                            fce_reg_result = __reg_fce_file.match(item)
                            if fce_reg_result:
                                if cfg_reg_result[0][2] == fce_reg_result.group(2):
                                    cfg_value = cfg_reg_result[0][0]
                                    fce_value = fce_reg_result.group(1).split(' ')[-1]
                                    if int(cfg_value, 16) != int(fce_value, 16):
                                        fce_string_length = len(fce_value)    # fce文件中，值的长度
                                        value = '{:X}'.format(int(cfg_value, 16))    # cfg 值，格式化
                                        value = value.zfill(fce_string_length)    # cfg 值，按照 fce 补齐长度

                                        # # 匹配替换指定值，并修改源数据
                                        reg_value = re.compile(r'(Q\s+.+\s+)(.+)(\s+//)(\s*.+)')
                                        r2 = re.sub(reg_value, r'\g<1>{}\g<3>\g<4>'.format(value), string=item)
                                        scr_data[index] = r2
    except Exception as err:
        print(err)
        sys.exit(2)

    with open(tmp_fce_file, 'w', encoding='utf-8') as fce:
        for line in scr_data:
            fce.writelines(line)

    os.remove(fce_file)
    os.rename(tmp_fce_file, fce_file)

def main():
    try:
        UpdateFceRead(abs_cfg_file_path, abs_fce_file_path)
    except Exception as e:
        print(e)
        sys.exit(1)
    else:
        print('Update done.')


if __name__ == '__main__':
    __usage = """
    python3 UpdateFceSetting.py -r CfgLogFile -u FceOutFile
    """
    parser = argparse.ArgumentParser(description="Update FceRead.out based on input Cfg.log.", usage=__usage)
    parser.add_argument('-r', '--read', required=True, dest='cfg_file', help='input version file')
    parser.add_argument('-u', '--update', required=True, dest='fce_file', help='input report file')
    args = parser.parse_args()

    cfg_file = args.cfg_file
    fce_file = args.fce_file

    abs_cfg_file_path = os.path.abspath(cfg_file)
    abs_fce_file_path = os.path.abspath(fce_file)

    main()
