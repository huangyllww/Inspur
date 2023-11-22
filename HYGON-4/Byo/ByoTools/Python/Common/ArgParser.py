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
from GlobalVariable import *
import argparse

gParamCheck = []
class SingleCheckCallback(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        if option_string not in gParamCheck:
            setattr(namespace, self.dest, values)
            gParamCheck.append(option_string)
        else:
            parser.error("Option %s only allows one instance in command line!" % option_string)

def ArgParser():
    Arg = argparse.ArgumentParser(
        prog=Prog,
        description=Description + Copyright,
        conflict_handler='resolve'
    )
    Arg.add_argument("-a", "--arch", action="append", type=str,
                     choices=['IA32', 'X64', 'EBC', 'ARM', 'AARCH64', 'RISCV64'], dest="TargetArch",
                     help="ARCHS is one of list: IA32, X64, ARM, AARCH64, RISCV64 or EBC, which overrides target.txt's TARGET_ARCH definition. To specify more archs, please repeat this option.")
    Arg.add_argument("-t", "--tagname", action="append", dest="ToolChain",
            help="Using the Tool Chain Tagname to build the platform, overriding target.txt's TOOL_CHAIN_TAG definition.")
    Arg.add_argument("-p", "--platform", action=SingleCheckCallback, type=str, dest="PlatformFile",
                      help="Build the platform specified by the DSC file name argument, overriding target.txt's ACTIVE_PLATFORM definition.")
    Arg.add_argument("-m", "--module", action=SingleCheckCallback, type=str, dest="ModuleFile",
                      help="Build the module specified by the INF file name argument.")
    Arg.add_argument("-D", "--define", action="append", dest="Macros", help="Macro: \"Name [= Value]\".")
    Arg.add_argument("-b", "--buildtarget", action="append", dest="BuildTarget",
                      help="Using the TARGET to build the platform, overriding target.txt's TARGET definition.")
    Arg.add_argument("-j", "--log", action="store", dest="LogFile", help="Put log in specified file as well as on console.")
    Arg.add_argument("-y", "--report-file", action="store", dest="ReportFile", help="Create/overwrite the report to the specified filename.")
    Arg.add_argument("Clean", action="store", nargs="?", default=False, help="Clean all the generated files")
    return Arg.parse_args()