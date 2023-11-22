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
# -*- coding: UTF-8 -*-

import sys
import os
import re
import argparse
import threading
import time
import shutil
import stat


def git_clone(ver_file):
    """
    Read version.txt file.
    Git clone project repo and checkout commitID.
    """

    global flug, lock, project_name, backup, projects, abs_output_path
    __module_info = []
    
    lock.acquire()
    
    flug = 1
    
    if flug == 1:
        print('[INFO]: git clone and checkout commitID ...')
        
        # project_name branch commitID in value of __module_info list.
        # if download version.txt form gitlab, read file use encoding='utf-16'.
        try:
            with open(ver_file, 'r', encoding='utf-16') as f:
                data = f.readlines()
                
                for line in data:
                    line = line.split()
                    __module_info.append(line)
        except Exception as err:
            print(err)
        
        projects = __module_info

        cwdpath = os.getcwd()

        # make code path, work into this code path. clone and checkout.
        # use edkrepo tool clone.
        try:
            clone_work_path = os.path.join(abs_output_path, project_name)
            edkrepo_cmd = 'edkrepo clone ' + project_name + ' ' + project_name
            if not os.path.exists(abs_output_path):
                os.makedirs(abs_output_path)
                os.chdir(abs_output_path)
                if os.path.exists(clone_work_path):
                    rm_do(clone_work_path)
                try:
                    os.system(edkrepo_cmd)
                except Exception as e:
                    print(e)
            else:
                os.chdir(abs_output_path)
                if os.path.exists(clone_work_path):
                    rm_do(clone_work_path)
                try:
                    os.system(edkrepo_cmd)
                except Exception as e:
                    print(e)
        except Exception as e:
            print(e)
        else:
            os.chdir(clone_work_path)
        
        for module in __module_info:
            for project in project_xml:
                if module[0].lower() == project['name'].lower():
                    git_checkout = 'git checkout ' + module[2]

                    if os.path.exists(os.path.join(clone_work_path, module[0])):
                        os.chdir(os.path.join(clone_work_path, module[0]))
                        os.system(git_checkout)
                        print('[INFO]: git checkout {} to {}'.format(project['url'], module[2]))
                    else:
                        print('[ERROR]: repo err, {} not exists!'.format(module[0]))
                        
        os.chdir(clone_work_path)

    print('[INFO]: work path is {}'.format(clone_work_path, project_name))
    print('[INFO]: git clone and checkout commitID was done!')

    flug = 0    
    lock.release()


def source_code(rep_file):
    """
    Read report.txt file, Clean source code.
    """

    global flug, lock, project_name, backup, projects, abs_output_path
    pattern_begin = re.compile(r'Module INF Path:\s+ByoModulePkg')
    pattern_lib = re.compile(r'.*\\Byo\\ByoModulePkg\\.*inf$')
    result_list = []
    backup = project_name + 'backup'
    
    lock.acquire()
    
    flug = 2
    
    if flug ==2:
        print('[INFO]: clean working ...')
        
        # pattern byomodulepkg inf file
        try:
            with open(rep_file, 'r') as f:
                while True:
                    data = f.readline()
                    if not data:
                        break
                    else:
                        # split and return right path, and append list
                        if pattern_begin.match(data):
                            result_list.append(data.split('ByoModulePkg')[1].strip())
                        
                        if pattern_lib.match(data):
                            result_list.append(data.split('ByoModulePkg')[1].strip())
        except Exception as err:
            print(err)
        
        cwdpath = os.getcwd()
        
        # cut string, append left string for '\' to list_dirs
        list_dirs = []
        for i in result_list:
            list_dirs.append(i.rpartition('\\')[0])

        # for list_dirs, and copy dirs into tmp backup dir
        if os.path.exists(backup):
            rm_do(backup)
        for i in list(set(list_dirs)):
            shutil.copytree(os.path.join(cwdpath, 'Byo', 'ByoModulePkg', *i.split('\\')),
            os.path.join(cwdpath, backup, *i.split('\\')))

        shutil.copytree(os.path.join(cwdpath, 'Byo', 'ByoModulePkg', 'Include'), os.path.join(cwdpath, backup, 'Include'))
        
        # copy files into tmp backup dir
        files_list = os.listdir(os.path.join(cwdpath, 'Byo', 'ByoModulePkg'))
        for file in files_list:
            if os.path.isfile(os.path.join(cwdpath, 'Byo', 'ByoModulePkg', file)):
                shutil.copyfile(os.path.join(cwdpath, 'Byo', 'ByoModulePkg', file), os.path.join(cwdpath, backup, file))
        
        # clean files and dirs
        if os.path.exists(os.path.join(cwdpath, 'Byo', 'ByoModulePkg')):
            rm_do(os.path.join(cwdpath, 'Byo', 'ByoModulePkg'))

        for p in projects:
            rm_do(os.path.join(cwdpath, p[0], '.git'))

        shutil.copytree(os.path.join(cwdpath, backup), os.path.join(cwdpath, 'Byo', 'ByoModulePkg'))
        rm_do(os.path.join(cwdpath, 'repo'))
        rm_do(os.path.join(cwdpath, backup))

    print('[INFO]: clean work was done!')
    print('[INFO]: code output path is {}'.format(cwdpath))

    flug = 0
    lock.release()

# rm tree method define
def rm_do(path):
    if os.path.exists(path):
        shutil.rmtree(path, onerror=readonly_handler)
        
def readonly_handler(func, path, execinfo):
    os.chmod(path, stat.S_IWRITE)
    func(path)


def main():
    def do(ver_file):
        try:
            source_code(ver_file)
        except Exception as e:
            print(e)

    t1 = threading.Thread(target=git_clone, args=(abs_version_path,))
    t2 = threading.Thread(target=do, args=(abs_report_path,))
    
    t1.start()
    t2.start()
    
    t1.join()
    t2.join()


if __name__ == '__main__':
    __usage = "python source_code.py -v VERSION_FILE -r REPORT_FILE -m MANIFEST -o OUTPUT_PATH"
    parser = argparse.ArgumentParser(description="Input Version.txt and Report.txt, Output source code.", usage=__usage)
    parser.add_argument('-v', required=True, help='input version file')
    parser.add_argument('-r', required=True, help='input report file')
    parser.add_argument('-m', required=True, help='manifest name')
    parser.add_argument('-o', required=True, help='output file path')
    args = parser.parse_args()

    abs_version_path = os.path.abspath(args.v)
    abs_report_path = os.path.abspath(args.r)
    abs_output_path = os.path.abspath(args.o)


    flug = 0
    lock = threading.Lock()
    
    #project_name = 'TiandyK800'
    project_name = args.m
    backup = ''
    projects = []
    project_xml = [{'name': 'Edk2', 'url': 'http://221.228.236.229:28090/byocore/edk2.git'}, 
        {'name': 'Edk2Platforms', 'url': 'http://221.228.236.229:28090/byocore/edk2platforms.git'},
        {'name': 'Intel', 'url': 'http://221.228.236.229:28090/byocore/intel.git'},
        {'name': 'Byo', 'url': 'http://221.228.236.229:28090/byocore/byo.git'},
        {'name': 'Tiandy', 'url': 'http://221.228.236.229:28090/iotgproject/tiandyk800.git'},
    ]
    

    main()
