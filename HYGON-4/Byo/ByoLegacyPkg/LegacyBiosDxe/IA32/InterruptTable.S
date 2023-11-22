## @file
#  Interrupt Redirection Template
#
# Copyright (c) 2006 - 2022, Byosoft Corporation.<BR>
#
# All rights reserved.This software and associated documentation (if any)
# is furnished under a license and may only be used or copied in
# accordance with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be reproduced,
# stored in a retrieval system, or transmitted in any form or by any
# means without the express written consent of Byosoft Corporation.
#
##

ASM_GLOBAL ASM_PFX(InterruptRedirectionTemplate)

#----------------------------------------------------------------------------
# Procedure:    InterruptRedirectionTemplate: Redirects interrupts 0x68-0x6F 
#
# Input:        None
#
# Output:       None
#
# Prototype:    VOID
#               InterruptRedirectionTemplate (  
#                                VOID
#                                );
#
# Saves:        None
#
# Modified:     None
#
# Description:  Contains the code that is copied into low memory (below 640K).
#               This code reflects interrupts 0x68-0x6f to interrupts 0x08-0x0f.
#               This template must be copied into low memory, and the IDT entries
#               0x68-0x6F must be point to the low memory copy of this code.  Each
#               entry is 4 bytes long, so IDT entries 0x68-0x6F can be easily 
#               computed.
#
#----------------------------------------------------------------------------
ASM_PFX(InterruptRedirectionTemplate):
  int     $0x8
  .byte   0xcf
  nop
  int     $0x9
  .byte   0xcf
  nop
  int     $0xa
  .byte   0xcf
  nop
  int     $0xb
  .byte   0xcf
  nop
  int     $0xc
  .byte   0xcf
  nop
  int     $0xd
  .byte   0xcf
  nop
  int     $0xe
  .byte   0xcf
  nop
  int     $0xf
  .byte   0xcf
  nop
