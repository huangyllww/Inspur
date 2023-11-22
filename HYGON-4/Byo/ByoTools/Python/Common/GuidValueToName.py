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
#
#According to guid.xref, get gNameArray and gGuidArray.
#

import argparse
import os

def generate_guid_map(guid_xref_file):
    with open(guid_xref_file, 'r') as xref_file:
        map_content = "#include <uefi.h>\n"
        map_content += "CHAR8 *gNameArray[] = {\n"

        for line in xref_file:
            parts = line.strip().split()
            if len(parts) >= 2:
                guid_name = parts[1].split("\\")[-1]
                map_content += '    "%s",\n' % (guid_name)

        map_content += "};\n"

        xref_file.seek(0)
        map_content += "CHAR8 *gGuidArray[] = {\n"
        for line in xref_file:
            parts = line.strip().split()
            if len(parts) >= 2:
                guid_value = parts[0]
                map_content += '    "%s",\n' % (guid_value)

        map_content += "};\n"
        map_content += "UINTN gTableNumber = ARRAY_SIZE (gGuidArray);\n"

        return map_content

#
# Generate guidmap.c and define gNameArray and gGuidArray.
#
def initialize_guid_map( guid_map_file):
    with open(guid_map_file, 'w') as map_file:
        map_file.write("#include <uefi.h>\n")
        map_file.write('CHAR8 *gNameArray[] = {""};\n')
        map_file.write('CHAR8 *gGuidArray[] = {""}; \n')
        map_file.write("UINTN gTableNumber = ARRAY_SIZE (gGuidArray);\n")

def main():
    parser = argparse.ArgumentParser(description="Generate GuidMap.c.")
    parser.add_argument('-i', required=True, help='Enter the BuildOutput, which is the Path to folder containing the FV files')
    parser.add_argument('-o', required=True, help='Output file Path containing the Byo files')
    args = parser.parse_args()

    guid_xref_file = os.path.join(os.path.abspath(args.i), 'FV' , 'Guid.xref')
    guid_map_file = os.path.join(os.path.abspath(args.o), 'Byo','ByoModulePkg', 'Library', 'ByoGuidValueToNameLib', 'GuidMap.c')

    if not os.path.exists(guid_xref_file):
        initialize_guid_map( guid_map_file)
    else:
        map_content = generate_guid_map(guid_xref_file)
        if os.path.exists(guid_map_file):
            with open(guid_map_file, 'r') as map_file:
              existing_lines = map_file.readlines()
              if not all(line == existing_line for line, existing_line in zip(map_content.splitlines(True), existing_lines)):
                with open(guid_map_file, 'w') as map_file:
                  map_file.write(map_content)
        else:
            initialize_guid_map( guid_map_file)


if __name__ == '__main__':
    main()