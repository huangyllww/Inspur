# ****************************************************************************
# *
# *
# * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
# *
# * HYGON is granting you permission to use this software (the Materials)
# * pursuant to the terms and conditions of your Software License Agreement
# * with HYGON.  This header does *NOT* give you permission to use the Materials
# * or any rights under HYGON's intellectual property.  Your use of any portion
# * of these Materials shall constitute your acceptance of those terms and
# * conditions.  If you do not agree to the terms and conditions of the Software
# * License Agreement, please do not use any portion of these Materials.
# *
# * CONFIDENTIALITY:  The Materials and all other information, identified as
# * confidential and provided to you by HYGON shall be kept confidential in
# * accordance with the terms and conditions of the Software License Agreement.
# *
# * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
# * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
# * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
# * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
# * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
# * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
# * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
# * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
# * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
# * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
# * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
# * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
# * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
# *
# * HYGON does not assume any responsibility for any errors which may appear in
# * the Materials or any other related information provided to you by HYGON, or
# * result from use of the Materials or any related information.
# *
# * You agree that you will not reverse engineer or decompile the Materials.
# *
# * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
# * further information, software, technical information, know-how, or show-how
# * available to you.  Additionally, HYGON retains the right to modify the
# * Materials at any time, without notice, and is not obligated to provide such
# * modified Materials to you..
# ******************************************************************************
# python 3.9

import os
import stat
import argparse

version = '1.0'


def parse_arg():
    """
    get input parameters
    :return: arguments
    """
    parser = argparse.ArgumentParser()
    parser.add_argument('--version', action='version', version='hygonfwpatcher.py v%s' % (version))

    parser.add_argument('--bios', help='16MB BIOS binary file', type=str, required=True)
    parser.add_argument('--fchfirmware', help='fchfirmware binary', type=str, required=True)
    args = parser.parse_args()
    return args


def patch_fchfirmware(bios_file, fchfirmware):
    """
    patch 16MB bios binary with fchfirmware.bin
    :param bios_file: 16MB bios binary
    :param fchfirmware: fchfirmware.bin
    :return: None
    """
    fch_firmware_offset = 0x20000

    firmware_data = open(fchfirmware, 'rb').read()
    fd_bios = open(bios_file, 'rb')
    fd_merged_1 = fd_bios.read(fch_firmware_offset)   # read 0 ~ 0x20000 -1
    fd_bios.seek(fch_firmware_offset + os.path.getsize(fchfirmware))  # skipp 0x20000 + sizeof(fchfirmware)
    fd_merged_2 = fd_bios.read()  # read 0x20000 + sizeof(fchfirmware) ~ sizeof(bios_file) - 1
    fd_bios.close()
    
    try:
        with open(bios_file, 'wb') as fd_merged:
            
            fd_merged.write(fd_merged_1)
            fd_merged.write(firmware_data)
            fd_merged.write(fd_merged_2)
            fd_merged.close()
            print('new bios: {} , size: 0x{:x}'.format(bios_file, os.path.getsize(bios_file)))
    except:
        pass


def main():
    args = parse_arg()
    print(vars(args))
    patch_fchfirmware(args.bios, args.fchfirmware)


if __name__ == '__main__':
    main()
