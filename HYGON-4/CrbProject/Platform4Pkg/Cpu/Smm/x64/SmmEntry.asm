

EXTERNDEF       gSmmEntrySize:WORD
EXTERNDEF       SmmEntryStart:BYTE
EXTERNDEF       gSmmStackOffs:WORD
EXTERNDEF       gProt32JmpOffs:WORD
EXTERNDEF       gLModeJmpOffs:WORD
EXTERNDEF        gPageTableOffs:WORD
EXTERNDEF       gSmmIdtBaseOffs:WORD
EXTERNDEF       SmmHandler:PROC

IDT_LIMIT           EQU 16 * 32         ; only exception vectors - 0..31

; offset relative to SMM BASE
SMM_BASE_SAVE_OFFS  EQU 0FF00h
GdtBaseOffset       EQU OFFSET GDT_BASE - SmmEntryStart + 8000h
GdtDescrOffset      EQU GdtDescriptor - SmmEntryStart + 8000h
Prot32Offs          EQU prot32 - SmmEntryStart + 8000h
LongModeOffs        EQU longMode - SmmEntryStart + 8000h


.code
SmmEntryStart   LABEL   BYTE


SmmEntry PROC

    ; Adjust GDT descriptor
    db      0bbh                        ; mov bx, imm16
    dw      SMM_BASE_SAVE_OFFS
    db      66h, 2eh, 8bh, 07h          ; mov eax,cs:[bx]
    db      66h, 8bh, 0f8h              ; mov edi, eax - save copy of SMM_BASE
    db      0bbh                        ; mov bx, imm16
    dw      GdtDescrOffset + 2          ; GDT Base Addr
    db      66h, 05h                    ; add eax, imm32
    dd      GdtBaseOffset
    db      66h, 2eh, 89h, 07h          ; mov cs:[bx], eax

    ; Load GDT
    db      66h, 2eh, 0fh,1,16h         ; lgdt cs:[imm16]
    dw      GdtDescrOffset

    ; copy SMM BASE to ecx
    db      66h
    mov     ecx, eax

    ; Enable Prot mode
    mov     rax, cr0
    or      al, 1                       ;Set PE bit
    mov     cr0, rax                    ;Turn on Protected Mode


    db      66h, 0eah                   ; jmp far CS:offs32
    dd      ?                           ; offset ; referenced by C code using gProt32JmpOffs offs
    dw      10h                         ; CS selector
prot32::

    ; Set base address of PML4
    db    0b8h          ; mov eax, imm32
gPageTable  dd  ?
    mov    cr3, rax

    mov     rax, cr4
  or      ax, 0220h      ;Enable PAE and OSFXSR
  mov     cr4, rax

    ; Set EFER.LME
    mov     ecx, 0c0000080h
  rdmsr
  bts     eax, 8
  wrmsr

  ;Enable paging
  mov     rax, cr0
  bts      eax, 31
  mov     cr0, rax                ;Now in long mode compatiblity.
  jmp      @f
@@:

    ; Set Data selectors
    mov     ax, 08h                     ; Data selector
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     fs, ax
    mov     gs, ax


    db  0eah
    dd  ?                               ; ; referenced by C code using gLModeJmpOffs offs
    dw  38h                             ; 64 bit Code selector

longMode::

    ; Init Stack
    db  48h, 0bch                       ; mov rsp, imm64
SmmStack   dq  ?


    ; Load IDT !!!!!!!!!!!!!!!!!!
    lea     rax, IDT_DESCR
    lidt    fword ptr [rax]

    ; save FX regs
    ; SmmStack is initialized 4K aligned, assuming we are not used stack yet,
    ; otherwise 16-Byte alignment required
    sub     rsp, 200h
    db  48h                             ; force 64-bit
    fxsave  [rsp]

    ; call SMI handler
    sub     rsp, 30h
    mov     rax, SmmHandler
    call    rax
    add     rsp, 30h

    ; restore FX regs
    db  48h                             ; force 64-bit
    fxrstor [rsp]

    rsm
    jmp $
SmmEntry ENDP

align 16
GDT_BASE:
SEL_00  equ $ - GDT_BASE                ;NULL Selector 0
        dd 0, 0

SEL_08  equ  $ - GDT_BASE                ; Selector 8, Data
        dd 0000ffffh
        dd 00cf9300h

SEL_10  equ $ - GDT_BASE                ; Selector 10h, Code
        dd 0000ffffh
        dd 00cf9b00h

SEL_18  equ $ - GDT_BASE                ; Selector 18h, not used
        dd 0, 0

SEL_20  equ $ - GDT_BASE                ; Selector 20h, not used
        dd 0, 0

SEL_28  equ $ - GDT_BASE                ; Selector 28h, not used
        dd 0, 0

SEL_30  equ $ - GDT_BASE                ; Selector 30h, not used
        dd 0, 0

SEL_38  equ $ - GDT_BASE              ; Selector 38h, Code x64
        dd 0000FFFFh
        dd 00af9b00h

TSS_SEL equ $ - GDT_BASE                ; Selector 40h, TSS
        dw 0067h
TSS_SEL_FIXUP:
        dw 0000h
        dd 00008900h

L_DATA_SEL equ  $ - GDT_BASE
        dd 0000ffffh
        dd 008f9300h

L_CODE_SEL equ  $ - GDT_BASE
        dw 0ffffh
L_CODE_SEL_FIXUP:
        dd 09a000000h                   ; Fix up by adding SMM BASE + 8000h
        dw 0

GDT_SIZE equ    $ - GDT_BASE        ; Size of Descriptor Table

GdtDescriptor:
        dw GDT_SIZE - 1                 ; GDT limit
GDT_DESC_FIXUP:
        dd GdtBaseOffset

align   16
IDT_DESCR:
        dw  IDT_LIMIT
SmmIdtBase      dq  ?

; offset relative to SmmEntryStart
gSmmStackOffs   dw  SmmStack - SmmEntryStart
gProt32JmpOffs  dw  prot32 - SmmEntryStart - 6
gLModeJmpOffs   dw  longMode - SmmEntryStart - 6
gPageTableOffs  dw  gPageTable - SmmEntryStart
gSmmIdtBaseOffs dw  SmmIdtBase - SmmEntryStart

gSmmEntrySize   dw  $ - SmmEntryStart
END
