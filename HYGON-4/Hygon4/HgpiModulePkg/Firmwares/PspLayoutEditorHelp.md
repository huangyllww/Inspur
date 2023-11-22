**PspLayoutEditor Help**

This script gets psp layout settings from psplayout.cfg and update psp layout xml files.
It  python2  python3

The settings in psplayout.cfg should be set manualy.

**Usage**

usage: PspLayoutEditor.py --securemode {es,mp} -c CONFIG --xml TemplateXML --outxml OUTPUTXML --outbin OUTPUTBIN [-h] [--version]

optional arguments:
  -h, --help            show this help message and exit
  --securemode {es,mp}  es: eng-sample cpu, mp:security cpu
  -c CONFIG, --config CONFIG
                        psp layout configuration file
  --inxml XML           psp layout template xml file
  --outxml OUTPUTXML    output xml file
  --outbin OUTPUTBIN    output fchfirmware file
  --soclist Ex Gx       soc list
  --version             show program's version number and exit


**Configurations**

[layout_configurations]
psp_dir_base=0x73000      # psp directory base address
psp_dir_size=0x320000     # psp directory size
bios_dir_base=0x393000    # bios directory base address
bios_dir_size=0x4d000     # bios directory size
fv_bb_addr=0xe00000       # fv_bb address
fv_bb_dest=0x31e00000     # fv_bb destination address
fv_bb_size=0x200000       # fv_bb size
fv_main_base=0x3E0000     # fv_main base address
fv_main_size=0xA20000     # fv_main size


**Notes:**

psplayout.cfg should be checked carefully, definitions in it can not overlap with each other, or bios will not work









