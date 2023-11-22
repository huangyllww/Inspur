;*****************************************************************************
; HYGON Generic Encapsulated Software Architecture
;
; $Workfile:: cpstackhooks.inc
; $Revision$    $Date$
;
; Description: CPSTACKHOOKS.INC - HGPI stack setup family hooks
;
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
    .LIST
    .mmx

STACK_SIZE_1M           EQU 100000h     ; 1MB
STACK_SIZE_256K         EQU 40000h      ; 256KB
STACK_SIZE_192K         EQU 30000h      ; 192KB
STACK_SIZE_128K         EQU 20000h      ; 128KB
STACK_SIZE_64K          EQU 10000h      ; 64KB
STACK_SIZE_32K          EQU 8000h       ; 32KB
STACK_SIZE_16K          EQU 4000h       ; 16KB
STACK_SIZE_4K           EQU 1000h       ; 4KB

CORE0_STACK_SIZE        EQU STACK_SIZE_16K   ; 16KB for primary cores
CORE1_STACK_SIZE        EQU STACK_SIZE_4K    ; 4KB for each AP cores

BSP_STACK_BASE_ADDR     EQU 30000h      ; Base address for core 0 stack
BSP_STACK_BASE_ADDR_4M  EQU 400000h     ; Base address at 4MB
CORE0_STACK_BASE_ADDR   EQU 80000h      ; Base address for primary cores stack
;CORE1_STACK_BASE_ADDR = BSP_STACK_BASE_ADDR + BSP_STACK_SIZE

BSP_CACHE_TYPE_POSITION EQU 3

;============================================================================
;
; Define a  macro that allow the exclusion of processor families from
; the cache-as-ram code. This will reduce the size of the assembled file.
;
;============================================================================

;---------------------------------------------------
;
; HYGON_ENABLE_STACK_FAMILY_HOOK Macro - Stackless
;
;   Set any family specific controls needed to enable the use of
;   cache as general storage before main memory is available.
;
; Inputs:
;       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;---------------------------------------------------
HYGON_ENABLE_STACK_FAMILY_HOOK MACRO

    HYGON_ENABLE_STACK_FAMILY_HOOK_F18

ENDM

;----------------------------------------------
;
; HYGON_DISABLE_STACK_FAMILY_HOOK Macro - Stackless
;
;   Return any family specific controls to their 'standard'
;   settings for using cache with main memory.
;
; Inputs:
;       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;----------------------------------------------
HYGON_DISABLE_STACK_FAMILY_HOOK MACRO

    HYGON_DISABLE_STACK_FAMILY_HOOK_F18

ENDM

;---------------------------------------------------
;
; GET_NODE_ID_CORE_ID Macro - Stackless
;
;   Read family specific values to determine the node and core
;   numbers for the core executing this code.
;
; Inputs:
;     none
; Outputs:
;     SI[7:0] = Core# (0..N, relative to node)
;     SI[15:8]= Node# (0..N)
;     SI[23:16]= reserved
;     SI[24]=   flag: 1=Family Unrecognized
;     SI[25]=   flag: 1=Interface re-entry call
;     SI[26]=   flag: 1=Core is primary of compute unit
;     SI[31:27]= reserved, =0
;
; Destroyed:
;       eax, ebx, ecx, edx, esi
;---------------------------------------------------
GET_NODE_ID_CORE_ID MACRO

    mov     si, -1
    GET_NODE_ID_CORE_ID_F18

    ;
    ; Check for unrecognized Family
    ;
    .if (si == -1)                      ; Has family (node/core) been discovered?
        mov     esi, ( (1 SHL FLAG_UNKNOWN_FAMILY)+(1 SHL FLAG_IS_PRIMARY) ) ; No, Set error code, Only let BSP continue
        mov     ecx, APIC_BASE_ADDRESS  ; MSR:0000_001B
        _RDMSR
        bt      eax, APIC_BSC           ;   Is this the BSC?
        .if (!carry?)
            ; No, this is an AP
            hlt                         ;       Kill APs
        .endif
    .endif
ENDM

;;***************************************************************************
;;                      Family 18h MACROS
;;***************************************************************************
;---------------------------------------------------
;
; HYGON_ENABLE_STACK_FAMILY_HOOK_F18 Macro - Stackless
;
;   Set any family specific controls needed to enable the use of
;   cache as general storage before main memory is available.
;
; Inputs:
;       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
;   * No INVD or WBINVD, no exceptions, page faults or interrupts
;---------------------------------------------------
HYGON_ENABLE_STACK_FAMILY_HOOK_F18 MACRO
    local   fam18_enable_stack_hook_exit

    HYGON_CPUID   CPUID_MODEL
    mov     ebx, eax                    ; Save revision info to EBX
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 08h                     ; Is this family 18h?
    jnz     fam18_enable_stack_hook_exit ; DnF18 if no

    ; Set TOP_MEM (C001_001A) for non-shared cores to 16M. This will be increased at heap init.
    ;  - not strictly needed since the FixedMTRRs take presedence.
    mov     ecx, TOP_MEM                ; MSR:C001_001A
    _RDMSR
    test    eax, eax
    .if (zero?)
        dec     eax
        _WRMSR
    .endif

fam18_enable_stack_hook_exit:
ENDM


;----------------------------------------------
;
; HYGON_DISABLE_STACK_FAMILY_HOOK_F18 Macro - Stackless
;
;   Return any family specific controls to their 'standard'
;   settings for using cache with main memory.
;
; Inputs:
;       ESI - [31:24] flags; [15:8]= Node#; [7:0]= core#
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
;---------------------------------------------------
HYGON_DISABLE_STACK_FAMILY_HOOK_F18 MACRO
    local   fam18_disable_stack_hook_exit

    HYGON_CPUID   CPUID_MODEL
    mov     ebx, eax                    ; Save revision info to EBX
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 08h                     ; Is this family 18h?
    jnz     fam18_disable_stack_hook_exit ; DnF18 if no

fam18_disable_stack_hook_exit:
ENDM

;---------------------------------------------------
;
; GET_NODE_ID_CORE_ID_F18 Macro - Stackless
;
;   Read family specific values to determine the node and core
;   numbers for the core executing this code.
;
; Inputs:
;     none
; Outputs:
;     ESI = core#, node# & flags (see GET_NODE_ID_CORE_ID macro above)
;     MM5 = 32b pointer to family info structure
; Destroyed:
;       eax, ebx, ecx, edx, esi, mm5
;---------------------------------------------------
GET_NODE_ID_CORE_ID_F18 MACRO
    local   node_core_f18_start
    local   node_core_f18_exit
    local   end_of_f18h_data

    IFNDEF FAM18H_INFO_STRUCT
        jmp     end_of_f18h_data
        ; Family 18h Info Structure:          L2Size,  #SharedCores, AllocMem, AllocExe, SzAddrBus, pad
        FAM18H_INFO_STRUCT  CPU_FAMILY_INFO {  2048,         2,          0,       0,         48,     0    }
end_of_f18h_data:
    ENDIF

    cmp     si, -1                      ; Has node/core already been discovered?
    jnz     node_core_f18_exit          ; DnF18 if yes
;PS381 modify -s
    HYGON_CPUID   CPUID_MODEL
;    shr     eax, 20                    ; AH = cpu extended family
   	and 	eax, 0FF00F00h
    cmp     eax, 00900F00h              ; Is this Hygon family 18h?
    jz     node_core_f18_start          ; DnF18 if yes
    jnz     node_core_f18_exit          ; DnF18 if no
node_core_f18_start:
;PS381 modify -e

    shr     al, 4                       ; AL = cpu extended model
    shr     ebx, 16                     ; BH = LocalApicId
    mov     bl, al                      ; BL = cpu extended model

    LoadTableAddress(FAM18H_INFO_STRUCT)
    movd    mm5, eax                    ; load pointer to Family Info Struc

    xor     esi, esi                    ; Assume BSC, clear local flags
    mov     ecx, APIC_BASE_ADDRESS      ; MSR:0000_001B
    _RDMSR
    bt      eax, APIC_BSC               ; Is this the BSC?
    .if (!carry?)
        shr     bx, 4
        shr     bl, 4
        mov     si, bx                  ;   SI = [15:8]= Node# = 0; [7:0]= core#
    .endif                              ; end

    ;
    ;   determine if this core shares MTRRs
    ;

    HYGON_CPUID   HYGON_CPUID_EXT_APIC
    .if (bh != 0)
        bt     si, 0
        .if (!carry?)
            bts     esi, FLAG_IS_PRIMARY    ;   Set shared flag into return value
        .endif
    .else
        bts     esi, FLAG_IS_PRIMARY    ;   Set shared flag into return value
    .endif

    bts    esi, FLAG_DRAM_AVAILABLE
node_core_f18_exit:
ENDM
