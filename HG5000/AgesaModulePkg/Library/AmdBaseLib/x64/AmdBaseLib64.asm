;/**
; * @file
; *
; * Agesa library 64bit
; *
; * Contains AMD AGESA Library
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  Lib
; * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
; */
;*****************************************************************************
;
;  
;  Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
;  AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
;  the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
;  the right to make the modified version available for use with HYGON's PRODUCT.
;*****************************************************************************

.code
;/*++

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Enter debugger on SimNow
; *
; *
; * @retval  Flag to indicate execution of Port 80 loop code.
; */
PUBLIC LibAmdSimNowEnterDebugger
LibAmdSimNowEnterDebugger PROC
    pushfq
    push rbx
    push rcx
    push rdx
    push rsi
    mov     si, 0                   ; Clear SI to indicate display message on port 80
    mov     rax, 0BACCD00Bh         ; Backdoor in SimNow
    mov     rbx, 2                  ; Select breakpoint feature
    cpuid

    mov     ax, si                  ; Return SI
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    popfq
    ret
LibAmdSimNowEnterDebugger ENDP


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Force breakpoint on HDT
; *
; *
; */
PUBLIC LibAmdHDTBreakPoint
LibAmdHDTBreakPoint PROC
    push rcx
    push rdi
    push rax
    push rdx
    mov rcx, 0C001100Ah             ;bit 0 = HDT redirect
    mov rdi, 09C5A203Ah             ;Password
    rdmsr
    and rax, 0ffffffffh
    or rax, 1

    wrmsr

    mov rax, 0B2h                  ;Marker = B2
    db 0F1h                        ;ICEBP

    pop rdx
    pop rax
    pop rdi
    pop rcx
    ret

LibAmdHDTBreakPoint ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  FPU init
; *
; *
; */
PUBLIC LibAmdFinit
LibAmdFinit  PROC
    db 9Bh, 5Dh, 0E3h ;finit
    ret
LibAmdFinit  ENDP

END
