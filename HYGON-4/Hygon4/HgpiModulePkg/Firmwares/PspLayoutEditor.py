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

import struct
import configparser as configparser
import os
import stat
import xml.etree.ElementTree as ET
import argparse
import shutil

version = '4.0'
BIT0 = (1 << 0)
BIT1 = (1 << 1)
BIT31 = (1 << 31)

SUPPORT = '1'
UNSUPPORT = '0'

SOC_GEN_ID_MAP = {
    'Ex': BIT31,
    'Gx': BIT0,
}

SOC_FW_MAP = {
    'Ex': 'SOC0',
    'Gx': 'SOC1',
}


class PspParam(object):
    psp_data_output = None
    psp_dir1_base = None
    psp_dir1_size = None
    bios_dir1_base = None
    bios_dir1_size = None
    soc_index = None
    fv_bb_addr = None
    fv_bb_dest = None
    fv_bb_size = None
    fv_main_base = None
    fv_main_size = None
    secure_mode = None


def parse_arg():
    """
    get input parameters
    :return: arguments
    """
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', help='configuration file', type=str, required=True)
    parser.add_argument('--securemode', help='es: eng-sample cpu, mp:security cpu', type=str, choices=['es', 'mp'],
                        required=True)
    parser.add_argument('--inxml', help='psp layout template xml file', type=str, required=True)
    parser.add_argument('--outxml', help='psp layout output xml file file', type=str, required=True)
    parser.add_argument('--outbin', help='output fchfirmware file', type=str, required=True)
    parser.add_argument('--soclist', help='soc list', type=str, required=True, nargs='+', choices=['Ex', 'Gx'])
    parser.add_argument('--version', action='version', version='psplayouteditor.py v%s' % (version))
    args = parser.parse_args()
    return args


def get_configurations(config_file):
    """
    get psp and bios layout from configuration file
    :param config_file: configuration file
    :return: psp layout info
    """
    config = configparser.ConfigParser()
    config.read(config_file)
    return config


def generate_fch_firmware(configurations, args):
    """
    create fch firmware binary. this binary is located at 0x20000 in bios binary, and 64-byte length
    Definitions as bellow:
    firmware_val[0] : OEM_SIG  0x55AA55AA
    firmware_val[3] : XHCI firmware base
    firmware_val[5] : PSP Directory pointer
    firmware_val[6] : BIOS directory pointer
    firmware_val[7] :
    firmware_val[8] :
    firmware_val[9] : compatible flag
    others          : not defined
    :param configurations: psp layout configuration file
    :param args:
    :return: None. This function generate an binary
    """
    SIGNATURE_OFFSET = 0
    XHCI_FW_BASE_OFFSET = 3
    PSP_DIRECTORY_OFFSET = 5
    BIOS_DIRECTORY_OFFSET = 6
    COMPATIBLE_FLAG_OFFSET = 9
    FCH_FIRMWARE_LENGTH = 18

    firmware_val = [0xFFFFFFFF] * FCH_FIRMWARE_LENGTH
    firmware_val[SIGNATURE_OFFSET] = 0x55AA55AA  # OEM_SIG is short for oem signature
    firmware_val[XHCI_FW_BASE_OFFSET] = 0xFF021000  # xhci firmware base
    firmware_val[COMPATIBLE_FLAG_OFFSET] = 0xFFFFFFFF

    bios_dir_base = int(configurations.get('layout_configurations', 'bios_dir_base'),
                        base=16) | 0xFF000000  # map to mmio address space
    psp_dir_base = int(configurations.get('layout_configurations', 'psp_dir_base'), base=16) | 0xFF000000  # map to mmio address space
    firmware_val[PSP_DIRECTORY_OFFSET] = psp_dir_base
    firmware_val[BIOS_DIRECTORY_OFFSET] = bios_dir_base

    # create compatile flag
    print(args.soclist)
    for soc in args.soclist:
        firmware_val[COMPATIBLE_FLAG_OFFSET] &= ~SOC_GEN_ID_MAP[soc]

    # generate fch firmware binary
    fch_firmware_bin = args.outbin
    print('fch firmware to be generated: {}'.format(fch_firmware_bin))
    if os.path.exists(fch_firmware_bin):
        os.chmod(fch_firmware_bin, stat.S_IRWXU)
        os.remove(fch_firmware_bin)

    fd = os.open(fch_firmware_bin, os.O_RDWR | os.O_CREAT)
    # fd = os.open(fch_firmware_bin, os.O_RDWR | os.O_CREAT)
    print('generate fch binary...')
    for i in range(FCH_FIRMWARE_LENGTH):
        print('offset {:02d}: |0x{:08x}|'.format((i * 4), firmware_val[i]))
        print(' ' * 11 + '-' * 12)
        os.write(fd, struct.pack('<I', firmware_val[i]))
    os.close(fd)
    print('generate fch binary done...')


def update_psp_data_template(configurations, args):
    """
    update base and size defined in psplayout.cfg
    :param configurations: configurations from psplayout.cfg
    :param args: command line parameters
    :return: None
    """
    psp0 = PspParam()
    config_sections = 'layout_configurations'

    psp_dir_base = int(configurations.get(config_sections, 'psp_dir_base'), base=16)
    psp_dir_size = int(configurations.get(config_sections, 'psp_dir_size'), base=16)
    bios_dir_base = int(configurations.get(config_sections, 'bios_dir_base'), base=16)
    bios_dir_size = int(configurations.get(config_sections, 'bios_dir_size'), base=16)
    fv_bb_addr = int(configurations.get(config_sections, 'fv_bb_addr'), base=16)
    fv_bb_dest = int(configurations.get(config_sections, 'fv_bb_dest'), base=16)
    fv_bb_size = int(configurations.get(config_sections, 'fv_bb_size'), base=16)
    fv_main_base = int(configurations.get(config_sections, 'fv_main_base'), base=16)
    fv_main_size = int(configurations.get(config_sections, 'fv_main_size'), base=16)

    if os.path.exists(args.outxml):
        os.chmod(args.outxml, stat.S_IRWXU)
        os.remove(args.outxml)
    shutil.copy(args.inxml, args.outxml)

    psp0.psp_data_output = args.outxml
    psp0.psp_dir1_base = psp_dir_base
    psp0.psp_dir1_size = psp_dir_size
    psp0.bios_dir1_base = bios_dir_base
    psp0.bios_dir1_size = bios_dir_size
    psp0.soc_index = SOC_FW_MAP[args.soclist[0]]  # all socs in soclist use same firmares
    psp0.fv_bb_size = fv_bb_size
    psp0.fv_bb_addr = fv_bb_addr
    psp0.fv_bb_dest = fv_bb_dest
    psp0.fv_main_base = fv_main_base
    psp0.fv_main_size = fv_main_size
    psp0.secure_mode = args.securemode
    update_xml(psp0)


def update_xml(psp):
    """
    :param psp:
    :return: None
    """
    bios_fv_bb_entry_type = '0x62'
    bios_fv_main_entry_type = '0x71'
    file_path = ['HgpiModulePkg', 'Firmwares', psp.soc_index, 'SOC_' + psp.secure_mode.upper()]

    # build fv_main entry node
    fv_main_attrib = dict()
    fv_main_attrib['Type'] = bios_fv_main_entry_type
    fv_main_attrib['Address'] = hex(psp.fv_main_base)
    fv_main_attrib['Size'] = hex(psp.fv_main_size)
    fv_main_node = ET.Element('POINT_ENTRY', fv_main_attrib)
    fv_main_node.tail = '\n' + ' ' * 4

    try:
        # update directory address in xml files
        os.chmod(psp.psp_data_output, stat.S_IRWXU)
        tree = ET.parse(psp.psp_data_output)
        root = tree.getroot()
        for psp_dir in root.iter('PSP_DIR'):
            # update psp directory level 1 base address in xml file
            if psp_dir.attrib['Level'] == '1':
                psp_dir.set('Base', hex(psp.psp_dir1_base).rstrip('L'))
                psp_dir.set('Size', hex(psp.psp_dir1_size).rstrip('L'))
            else:
                pass
            # update FW_VERSION and FW_SECURE_MODE
            for image_node in psp_dir.findall('IMAGE_ENTRY'):
                file_value = image_node.attrib['File']
                tmp = file_value.split('\\')
                tmp[2] = psp.soc_index
                tmp[3] = 'SOC_' + psp.secure_mode.upper()
                file_value = '\\'.join(tmp)
                image_node.set('File', file_value)
        for bios_dir in root.iter('BIOS_DIR'):
            if bios_dir.attrib['Level'] == '1':
                bios_dir.set('Base', hex(psp.bios_dir1_base).rstrip('L'))
                bios_dir.set('Size', hex(psp.bios_dir1_size).rstrip('L'))
            else:
                pass

            # update fv_bb
            for point_entry in bios_dir.findall('POINT_ENTRY'):
                if point_entry.attrib['Type'] == bios_fv_bb_entry_type:  # update bios level 2 directory
                    point_entry.set('Address', hex(psp.fv_bb_addr).rstrip('L'))
                    point_entry.set('Destination', hex(psp.fv_bb_dest).rstrip('L'))
                    point_entry.set('Size', hex(psp.fv_bb_size).rstrip('L'))
                    print('FV_BB:{}'.format(point_entry.attrib))

            for image_node in bios_dir.findall('IMAGE_ENTRY'):
                if image_node.attrib['Type'] in ['0x60', '0x68']:
                    continue
                file_value = image_node.attrib['File']
                tmp = file_value.split('\\')
                tmp[2] = psp.soc_index
                tmp[3] = 'SOC_' + psp.secure_mode.upper()
                file_value = '\\'.join(tmp)
                image_node.set('File', file_value)
        # update to file
        tree.write(psp.psp_data_output)

    except:
        raise Exception('Failed to update psp data layout xml files')


def update_firmware_and_layout(args):
    """
    :param args: from command line
    :return: None
    """
    configurations = get_configurations(args.config)
    generate_fch_firmware(configurations, args)
    update_psp_data_template(configurations, args)


def main():
    args = parse_arg()
    print("psplayouteditor version {}".format(version))
    print(vars(args))
    update_firmware_and_layout(args)


if __name__ == '__main__':
    main()
