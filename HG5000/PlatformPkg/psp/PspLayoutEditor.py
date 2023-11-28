import struct
import ConfigParser
import os
import stat
import xml.etree.ElementTree as ET
import argparse
import shutil

version = '0.2'
SUPPORT = '1'
UNSUPPORT = '0'
csv_data_tag = 'IMAGE_ENTRY'
csv_data_attrib = {'File': r"AgesaModulePkg\Firmwares\FW_VERSION\FW_SECURE_MODE\CsvData.bin", 'Type': "0x38"}
csv_firmware_tag = 'IMAGE_ENTRY'
csv_firmware_attrib = {'File': r'AgesaModulePkg\Firmwares\FW_VERSION\FW_SECURE_MODE\CsvFirmware.esbin', 'Type': '0x49'}
csv_data_path = ['DP', 'DNMcm_MP', 'CsvData.bin']
csv_firmware_path = ['DP', 'DNMcm_MP', 'CsvFirmware.esbin']
csv_type = ['0x38', '0x49']


def get_configurations(config_file):
    """
    get psp and bios layout from configuration file
    :param config_file: configuration file
    :return: psp layout info
    """
    config = ConfigParser.RawConfigParser()
    config.read(config_file)
    return config


def generate_fch_firmware(configurations, csv_support='0', multi_soc_support=False):
    """
    generate hygonfchfirmware.bin
    :param configurations:
    :param multi_soc_support:
    :return: None
    """
    # generate fch firmware
    OEM_SIG = 0x55AA55AA  # signature
    XHCI_FIRMWARE_BASE = 0xFF021000
    firmware_val = [0] * 8
    firmware_val[0] = OEM_SIG
    firmware_val[3] = XHCI_FIRMWARE_BASE  # default value
    if multi_soc_support:
        if csv_support == SUPPORT:
            combo_dir_base = int(configurations.get('multi_soc_csv', 'combo_dir_base'), base=16)
            soc0_bios_dir_level_1_base = int(configurations.get('multi_soc_csv', 'soc0_bios_dir_level_1_base'), base=16)
            soc1_bios_dir_level_1_base = int(configurations.get('multi_soc_csv', 'soc1_bios_dir_level_1_base'), base=16)
        elif csv_support == UNSUPPORT:
            combo_dir_base = int(configurations.get('multi_soc', 'combo_dir_base'), base=16)
            soc0_bios_dir_level_1_base = int(configurations.get('multi_soc', 'soc0_bios_dir_level_1_base'), base=16)
            soc1_bios_dir_level_1_base = int(configurations.get('multi_soc', 'soc1_bios_dir_level_1_base'), base=16)
        else:
            soc0_bios_dir_level_1_base = 0
            soc1_bios_dir_level_1_base = 0
            combo_dir_base = 0
        # fch_firmware_bin = configurations.get('file_path', 'fch_firmware_bin')
        # translate flash address to memory address
        soc0_bios_dir_level_1_base |= 0xFF000000
        soc1_bios_dir_level_1_base |= 0xFF000000
        combo_dir_base |= 0xFF000000
        firmware_val[5] = combo_dir_base
        firmware_val[6] = soc0_bios_dir_level_1_base
        firmware_val[7] = soc1_bios_dir_level_1_base
    else:
        soc0_bios_dir_level_1_base = int(configurations.get('uni_soc', 'soc0_bios_dir_level_1_base'), base=16)
        soc0_psp_dir_level_1_base = int(configurations.get('uni_soc', 'soc0_psp_dir_level_1_base'), base=16)
        soc0_bios_dir_level_1_base |= 0xFF000000
        soc0_psp_dir_level_1_base |= 0xFF000000
        firmware_val[5] = soc0_psp_dir_level_1_base
        firmware_val[6] = soc0_bios_dir_level_1_base

    fch_firmware_bin = configurations.get('file_path', 'fch_firmware_bin')
    print('fch firmware name: {}'.format(fch_firmware_bin))
    if os.path.exists(fch_firmware_bin):
        os.chmod(fch_firmware_bin, stat.S_IRWXU)
        os.remove(fch_firmware_bin)
    fd = os.open(fch_firmware_bin, os.O_RDWR | os.O_CREAT)
    print('generate fch binary:')
    for i in firmware_val:
        print(hex(i))
        os.write(fd, struct.pack('<I', i))
    os.close(fd)


def update_psp_data_template(configurations, multi_soc, soc_index, soc_generation, secure_mode, csv_support):
    """

    :param configurations: content of psplayout.cfg
    :param multi_soc: True: combo header supported,  False: combo header do not supported
    :param soc_index: 0 or 1
    :param soc_generation: 'DN' or 'DP'
    :param secure_mode: 0: engineer sample cpu,  1: security cpu
    :param csv_support:
    :return: None
    """
    secure_modes = [None, 'DNMcm_ES', 'DNMcm_MP']
    if secure_mode:
        secure_str = secure_modes[int(secure_mode)]
        # new_str = soc_generation + '\\' + secure_str

    if multi_soc:
        if csv_support == SUPPORT:
            config_sections = 'multi_soc_csv'
        else:
            config_sections = 'multi_soc'
    else:
        if csv_support == SUPPORT:
            config_sections = 'uni_soc_csv'
        else:
            config_sections = 'uni_soc'

    psp_level_1_str = 'soc{}_psp_dir_level_1_base'.format(soc_index)
    psp_level_1_size_str = 'soc{}_psp_dir_level_1_size'.format(soc_index)
    psp_level_2_str = 'soc{}_psp_dir_level_2_base'.format(soc_index)
    psp_level_2_size_str = 'soc{}_psp_dir_level_2_size'.format(soc_index)
    bios_level_1_str = 'soc{}_bios_dir_level_1_base'.format(soc_index)
    bios_level_1_size_str = 'soc{}_bios_dir_level_1_size'.format(soc_index)
    bios_level_2_str = 'soc{}_bios_dir_level_2_base'.format(soc_index)
    bios_level_2_size_str = 'soc{}_bios_dir_level_2_size'.format(soc_index)
    psp_data_template = 'psp_data_template_soc{}'.format(soc_index)
    psp_data_output_soc = 'psp_data_output_soc{}'.format(soc_index)

    soc_psp_dir_level_1_base = int(configurations.get(config_sections, psp_level_1_str), base=16)
    soc_psp_dir_level_1_size = int(configurations.get(config_sections, psp_level_1_size_str), base=16)
    soc_bios_dir_level_1_base = int(configurations.get(config_sections, bios_level_1_str), base=16)
    soc_bios_dir_level_1_size = int(configurations.get(config_sections, bios_level_1_size_str), base=16)
    soc_psp_dir_level_2_base = int(configurations.get(config_sections, psp_level_2_str), base=16)
    soc_psp_dir_level_2_size = int(configurations.get(config_sections, psp_level_2_size_str), base=16)
    soc_bios_dir_level_2_base = int(configurations.get(config_sections, bios_level_2_str), base=16)
    soc_bios_dir_level_2_size = int(configurations.get(config_sections, bios_level_2_size_str), base=16)

    psp_data_template_soc = configurations.get('file_path', psp_data_template)
    psp_data_output_soc = configurations.get('file_path', psp_data_output_soc)
    print("update :{}".format(psp_data_output_soc))
    if os.path.exists(psp_data_output_soc):
        os.chmod(psp_data_output_soc, stat.S_IRWXU)
        os.remove(psp_data_output_soc)
    shutil.copy(psp_data_template_soc, psp_data_output_soc)
    try:
        # update directory address in xml files
        os.chmod(psp_data_output_soc, stat.S_IRWXU)
        tree = ET.parse(psp_data_output_soc)
        root = tree.getroot()
        for psp_dir in root.iter('PSP_DIR'):
            # update psp directory level 1 and level 2 base address in xml file
            if psp_dir.attrib['Level'] == '1':
                psp_dir.set('Base', hex(soc_psp_dir_level_1_base).rstrip('L'))
                psp_dir.set('Size', hex(soc_psp_dir_level_1_size).rstrip('L'))

                for psp_level_2_point_entry in psp_dir.findall('POINT_ENTRY'):
                    if psp_level_2_point_entry.attrib['Type'] == "0x40":
                        psp_level_2_point_entry.set('Address', hex(soc_psp_dir_level_2_base).rstrip('L'))
                        pub_key_size = psp_level_2_point_entry.attrib['Size']

                for psp_level_2_point_entry in psp_dir.findall('POINT_ENTRY'):
                    if psp_level_2_point_entry.attrib['Type'] == "0x1":
                        psp_level_2_point_entry.set('Address', hex(soc_psp_dir_level_2_base +
                                                                   int(pub_key_size, base=16)).rstrip('L'))

                # check csv exists
                if secure_mode in ['1', '2']:
                    tmp = csv_data_attrib['File'].split('\\')
                    tmp[2] = soc_generation
                    tmp[3] = secure_modes[1] if secure_mode == '1' else secure_modes[2]
                    csv_data_attrib['File'] = '\\'.join(tmp)

                    tmp = csv_firmware_attrib['File'].split('\\')
                    tmp[2] = soc_generation
                    tmp[3] = secure_modes[1] if secure_mode == '1' else secure_modes[2]
                    csv_firmware_attrib['File'] = '\\'.join(tmp)

                csv_data_node = ET.Element(csv_data_tag, csv_data_attrib)
                csv_data_node.tail = '\n\t'
                csv_firmware_node = ET.Element(csv_firmware_tag, csv_firmware_attrib)
                csv_firmware_node.tail = '\n\t'

                # delete csv node first if it exists
                for image_node in psp_dir.findall('IMAGE_ENTRY'):
                    if image_node.attrib['Type'] in csv_type:
                        psp_dir.remove(image_node)

                # add csv entry
                if (csv_support == SUPPORT) and (soc_generation == 'DP'):
                    # update size because csv data and csv firmware are added to psp level 1 directory
                    # old_size = int(psp_dir.attrib['Size'], base=16)
                    # csv_data_size = os.path.getsize(os.sep.join(csv_data_path))
                    # csv_firmware_size = os.path.getsize(os.sep.join(csv_firmware_path))
                    # csv_total_size = csv_data_size + csv_firmware_size
                    # # print('total_size: {:X}'.format(csv_total_size))
                    # if (csv_total_size & 0xFFF) != 0:
                    #     csv_total_size = (csv_total_size & (~0xfff)) + 0x1000
                    # new_size = old_size + csv_total_size
                    # psp_dir.set('Size', hex(new_size).rstrip('L'))
                    psp_dir.insert(-1, csv_data_node)
                    psp_dir.insert(-1, csv_firmware_node)
                else:
                    pass

            elif psp_dir.attrib['Level'] == '2':
                psp_dir.set('Base', hex(soc_psp_dir_level_2_base).rstrip('L'))
                psp_dir.set('Size', hex(soc_psp_dir_level_2_size).rstrip('L'))
            else:
                pass

            if secure_mode in ['1', '2']:
                for image_node in psp_dir.findall('IMAGE_ENTRY'):
                    file_value = image_node.attrib['File']
                    tmp = file_value.split('\\')
                    tmp[2] = soc_generation
                    tmp[3] = secure_str
                    file_value = '\\'.join(tmp)
                    image_node.set('File', file_value)

        for bios_dir in root.iter('BIOS_DIR'):
            if bios_dir.attrib['Level'] == '1':
                bios_dir.set('Base', hex(soc_bios_dir_level_1_base).rstrip('L'))
                bios_dir.set('Size', hex(soc_bios_dir_level_1_size).rstrip('L'))
                for point_entry in bios_dir.findall('POINT_ENTRY'):
                    if point_entry.attrib['Type'] == '0x70':  # update bios level 2 directory
                        point_entry.set('Address', hex(soc_bios_dir_level_2_base).rstrip('L'))
            elif bios_dir.attrib['Level'] == '2':
                bios_dir.set('Base', hex(soc_bios_dir_level_2_base).rstrip('L'))
                bios_dir.set('Size', hex(soc_bios_dir_level_2_size).rstrip('L'))
            else:
                pass
            if secure_mode in ['1', '2']:
                for image_node in bios_dir.findall('IMAGE_ENTRY'):
                    if image_node.attrib['Type'] in ['0x60', '0x68']:
                        continue
                    file_value = image_node.attrib['File']
                    tmp = file_value.split('\\')
                    tmp[2] = soc_generation  # FW_VERSION index is 2
                    tmp[3] = secure_str      # FW_SECURE_MODE index is 3
                    file_value = '\\'.join(tmp)
                    image_node.set('File', file_value)
        # update to file
        tree.write(psp_data_output_soc)
    except:
        raise Exception('Failed to update psp data layout xml files')


def update_combo_template(configurations):
    """
    generate combo psp layout
    :param configurations: configurations: content of psplayout.cfg
    :return: None
    """
    combo_dir_base = int(configurations.get('multi_soc', 'combo_dir_base'), base=16)
    soc0_psp_dir_level_1_base = int(configurations.get('multi_soc', 'soc0_psp_dir_level_1_base'), base=16)
    soc1_psp_dir_level_1_base = int(configurations.get('multi_soc', 'soc1_psp_dir_level_1_base'), base=16)
    psp_data_template_combo = configurations.get('file_path', 'psp_data_template_combo')
    psp_data_output_combo = configurations.get('file_path', 'psp_data_output_combo')
    if os.path.exists(psp_data_output_combo):
        os.chmod(psp_data_output_combo, stat.S_IRWXU)
        os.remove(psp_data_output_combo)
    shutil.copy(psp_data_template_combo, psp_data_output_combo)
    print('update: {}'.format(psp_data_output_combo))
    try:
        # translate to memory address
        soc0_psp_dir_level_1_base |= 0xFF000000
        soc1_psp_dir_level_1_base |= 0xFF000000
        # update combo xml
        os.chmod(psp_data_output_combo, stat.S_IRWXU)
        tree = ET.parse(psp_data_output_combo)
        root = tree.getroot()
        combo_dir = root.find('COMBO_DIR')
        combo_dir.set('Base', hex(combo_dir_base).rstrip('L'))
        combo_entries = root.findall('COMBO_DIR/COMBO_ENTRY')
        combo_entries[0].set('Address', hex(soc0_psp_dir_level_1_base).rstrip('L'))
        combo_entries[1].set('Address', hex(soc1_psp_dir_level_1_base).rstrip('L'))
        tree.write(psp_data_output_combo)
    except:
        raise Exception('Failed to udpate combo xml template')


def update_firmware_and_layout(args):
    """
    generate fch firmware used for descriping psp layout
    :param args: input parameters
    :return: None
    """
    configurations = get_configurations(args.configuration)
    pap_layout_strategy = args.psplayout
    csv_support = args.csv
    secure_mode = args.securemode

    if pap_layout_strategy == '1':  # only dhyana
        generate_fch_firmware(configurations, csv_support, False)
        update_psp_data_template(configurations, False, 0, 'D1', secure_mode, UNSUPPORT)
    elif pap_layout_strategy == '2':   # only dhyana plus
        generate_fch_firmware(configurations, csv_support, False)
        update_psp_data_template(configurations, False, 0, 'DP', secure_mode, csv_support)

    elif pap_layout_strategy == '3':  # multi-soc: dhyana + dhyana plus
        generate_fch_firmware(configurations, csv_support, True)
        update_combo_template(configurations)  # update combo header xml
        # update soc0(dhyana) xml
        update_psp_data_template(configurations, True, 0, 'D1', secure_mode, csv_support)
        # update soc1(dhyana plus) xml
        update_psp_data_template(configurations, True, 1, 'DP', secure_mode, csv_support)
    else:
        pass


def parse_arg():
    """
    parse input parameters
    :return: arguments
    """
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--configuration', help='configuration file', type=str)
    parser.add_argument('-l', '--psplayout', help='psp layout stratergy, 1:DN, 2: DP, 3:(DN+DP)', type=str)
    parser.add_argument('-s', '--csv', help='csv function, 0:do not support, 1:support', type=str)
    parser.add_argument('-m', '--securemode', help='cpu security mode, 1: eng-sample cpu, 2:security cpu', type=str)
    parser.add_argument('--version', action='version', version='PspLayoutEditor.py v%s' % (version))
    args = parser.parse_args()
    return args


def main():
    args = parse_arg()
    update_firmware_and_layout(args)


if __name__ == '__main__':
    main()
