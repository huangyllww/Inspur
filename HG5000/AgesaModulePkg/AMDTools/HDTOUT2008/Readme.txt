------------------------------------------------------------------------
HDTOUT v2.2.5

Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.

HYGON is granting you permission to use this software (the Materials)
pursuant to the terms and conditions of your Software License Agreement
with HYGON.  This header does *NOT* give you permission to use the Materials
or any rights under HYGON's intellectual property.  Your use of any portion
of these Materials shall constitute your acceptance of those terms and
conditions.  If you do not agree to the terms and conditions of the Software
License Agreement, please do not use any portion of these Materials.

CONFIDENTIALITY:  The Materials and all other information, identified as
confidential and provided to you by HYGON shall be kept confidential in
accordance with the terms and conditions of the Software License Agreement.

LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
(INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
THE ABOVE LIMITATION MAY NOT APPLY TO YOU.

HYGON does not assume any responsibility for any errors which may appear in
the Materials or any other related information provided to you by HYGON, or
result from use of the Materials or any related information.

You agree that you will not reverse engineer or decompile the Materials.

NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
further information, software, technical information, know-how, or show-how
available to you.  Additionally, HYGON retains the right to modify the
Materials at any time, without notice, and is not obligated to provide such
modified Materials to you.

AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
the right to make the modified version available for use with HYGON's PRODUCT.
------------------------------------------------------------------------

Changes in 2.3.0
  - Reserve three filter flags for platform in case it needs to use HDTOUT to log platform data.

Changes in 2.2.5
  - Add TEST_POINT filter

Changes in 2.2.4
  - Improve bit field parsing to show field names during MEM_SETREG for KV.

Changes in 2.2.3
  - Add MEM_UNDEF_BF filter to show undefined memory bit field accesses.

Changes in 2.2.2
  - Fix crash on OR-B0

Changes in 2.2.0
  - Add LIB_PCI_RD and LIB_PCI_WR filter

Changes in 2.1.0
  - Fix script to work with HDT 8.0.336 due to new OR workaround with Exit PDM
  - Add check to avoid enabling-at-reset when BIOS has enabled on its own.
  - Remove HDT-on-at-reset when script die or interrupted.

Changes in 2.0.6
  - Fix enable-at-reset feature works with Wombat
  - Add $exit_dbg_after_reset control flag since some chipset requires exit_dbg after writing to CF9

Changes in 2.0.5
  - Add 'HDTOUT Spec.pdf'
  - Readme.txt will only contains release log
  - Due to the hardware dependency of the HDTOUT scripts, AGESA will only
    maintain hdtout2008.pl and will deprecate the other scripts. They will
    be moved into "Samples" directory for customers to reference while
    writing their own scripts. Below is the list of scripts that are deprecated:
      +  EnableHdtoutAtReset.pl
      +  FuncList.pm
      +  MemRdWr.pl
      +  setbp2008.pl
      +  StringBP.pm
  - No changes in scripts

Changes in 2.0.4
  - Fix enable-at-reset issue on SB880
  - Add Reset support: users can choose where to cold/warm reset by using HdtoutSelectionTable.
  - Fix STDIN bugs
  - Fix SPD dump bug

Changes in 2.0.2
  - Add "N" Enable without reset
  - Add Debug S3 resume tip to help

Changes in 2.0.1
  - Add support for parse ASSERT Message
    - Add wp comand to specify the working path, allow parse assert function
    get more information
  - Fix "Ctrl + C" doesn't work Properly in Simnow

Changes in 2.0.0
  - Update command line interface, use hdtout2008.pl -h for detail information
     - Configure output logfile
     - Configure possum id
     - Set print buffer size
     - Configure YAAP property
     - Configure SPD output
     - [new] Add filter control, user can set the filter of output at runtime

Changes in 1.4.0
  - Add option to show PCI register read/write (requires new BIOS)
  - Add option to read SPD data (requires new BIOS)
  - Move default output directory from C: to C:\HDTOUT to support Windows 7
  - Add YAAP support
  - Move control flags to top of hdtout2008.pl for easy modification
  - Add bsp_only flag so script does not need to scan through all the cores
  - Add usage
  - Fix bug when using hdtout string to break in


