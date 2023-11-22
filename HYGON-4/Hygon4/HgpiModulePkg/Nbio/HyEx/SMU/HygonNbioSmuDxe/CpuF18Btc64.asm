;/**
; * @file
; *
; * HYGON Family_17 Satori boot time calibration code
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      HGPI
; * @e sub-project:
; */
;*****************************************************************************
;
;  
;  Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
;  
;  HYGON is granting you permission to use this software (the Materials)
;  pursuant to the terms and conditions of your Software License Agreement
;  with HYGON.  This header does *NOT* give you permission to use the Materials
;  or any rights under HYGON's intellectual property.  Your use of any portion
;  of these Materials shall constitute your acceptance of those terms and
;  conditions.  If you do not agree to the terms and conditions of the Software
;  License Agreement, please do not use any portion of these Materials.
;  
;  CONFIDENTIALITY:  The Materials and all other information, identified as
;  confidential and provided to you by HYGON shall be kept confidential in
;  accordance with the terms and conditions of the Software License Agreement.
;  
;  LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
;  PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
;  WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
;  MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
;  OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
;  IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
;  (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
;  INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
;  GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
;  RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
;  THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
;  EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
;  THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
;  
;  HYGON does not assume any responsibility for any errors which may appear in
;  the Materials or any other related information provided to you by HYGON, or
;  result from use of the Materials or any related information.
;  
;  You agree that you will not reverse engineer or decompile the Materials.
;  
;  NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
;  further information, software, technical information, know-how, or show-how
;  available to you.  Additionally, HYGON retains the right to modify the
;  Materials at any time, without notice, and is not obligated to provide such
;  modified Materials to you.
;  
;  
;*****************************************************************************
 text  SEGMENT

XSET_BV MACRO
        db 00fh, 001h, 0d1h
ENDM

XGET_BV MACRO
        db 00fh, 001h, 0d0h
ENDM


;======================================================================
; ExecuteF18Btc:  Wrapper code for boot time calibration workloads
;
;   In:
;       rdx - buffer address for workloads
;       rcx - test to run
;
;   Out:
;       None
;
;   Destroyed:
;       None
;
;======================================================================
ExecuteHalt PROC PUBLIC
cli
hlt
jmp ExecuteHalt
ExecuteHalt ENDP


;======================================================================
; ExecuteF18Btc:  Wrapper code for boot time calibration workloads
;
;   In:
;       rdx - buffer address for workloads
;       rcx - test to run
;
;   Out:
;       None
;
;   Destroyed:
;       None
;
;======================================================================
ExecuteF18Btc PROC PUBLIC
  push  rax
  push  rbx
  push  rcx
  push  rdx
  push  rdi
  push  rsi
  push  rbp
  push  r8
  push  r9
  push  r10
  push  r11
  push  r12
  push  r13
  push  r14
  push  r15
  pushfq

;  add   rdx, 63
;  and   rdx, 0FFFFFFFFFFFFFFC0h
  push  rcx
  push  rdx
  mov   rax, 1
  cpuid
  bt    rcx, 26
  jc    SetupAVX
  pop   rdx
  pop   rcx
  jmp   NoAVX

SetupAVX:
  mov   r10, cr4
  mov   r11, r10
  bts   r10, 18
  mov   cr4, r10
  xor   rcx, rcx
  XGET_BV
  mov   r12, rax
  mov   r13, rdx
  mov   rax, 0dh
  cpuid
  xor   rdx, rdx
  xor   rcx, rcx
  XSET_BV
  pop   rdx
  pop   rcx
  push  r11
  push  r12
  push  r13
  call  CpuF18Btc
  pop   rdx
  pop   rax
  xor   rcx, rcx
  XSET_BV
  pop   r11
  mov   cr4, r11

NoAVX:
  popfq
  pop   r15
  pop   r14
  pop   r13
  pop   r12
  pop   r11
  pop   r10
  pop   r9
  pop   r8
  pop   rbp
  pop   rsi
  pop   rdi
  pop   rdx
  pop   rcx
  pop   rbx
  pop   rax
  ret
ExecuteF18Btc ENDP

;======================================================================
; CpuF18Btc: The workloads.
;
;   In:
;       rdx - buffer address for workloads
;       rcx - test to run
;
;   Out:
;       None
;
;   Destroyed:
;       Many
;
;======================================================================
CpuF18Btc PROC PUBLIC
include CpuF18Btc64.inc
CpuF18Btc ENDP

END

