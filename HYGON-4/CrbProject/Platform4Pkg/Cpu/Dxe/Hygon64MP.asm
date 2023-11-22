
.code

TKN_USE_AP_HLT  = 1
TKN_APIC_BASE  =  0fee00000h
TKN_APIC_ID_REGISTER  =  020h
TKN_MP_ZERO_DATA_ADDRESS  =  00h
TKN_APIC_ICR_LOW_REGISTER  =  0300h
TKN_MP_JUMP_FUNCTION_ADDRESS  =  01000h
TKN_APIC_SPURIOUS_VECTOR_REGISTER  =  0f0h
TKN_APIC_LVT_LINT0_REGISTER  =  0350h
TKN_APIC_LVT_LINT1_REGISTER  =  0360h
TKN_APIC_ICR_HIGH_REGISTER  =  0310h

public InterruptHandlerStart
public InterruptHandlerSize
public InterruptHandlerTblFixup

EFI_FX_SAVE_STATE_X64 STRUCT
    Fcw         dw ?
    Fsw         dw ?
    Ftw         dw ?
    Opcode      dw ?
    RegRip      dq ?
    DataOffset  dq ?
    Rsv1        db 8  dup (?)
    St0Mm0      db 10 dup (?)
    Rsv2        db 6  dup (?)
    St1Mm1      db 10 dup (?)
    Rsv3        db 6  dup (?)
    St2Mm2      db 10 dup (?)
    Rsv4        db 6  dup (?)
    St3Mm3      db 10 dup (?)
    Rsv5        db 6  dup (?)
    St4Mm4      db 10 dup (?)
    Rsv6        db 6  dup (?)
    St5Mm5      db 10 dup (?)
    Rsv7        db 6  dup (?)
    St6Mm6      db 10 dup (?)
    Rsv8        db 6  dup (?)
    St7Mm7      db 10 dup (?)
    Rsv9        db 6  dup (?)
    RegXmm0     db 16 dup (?)
    RegXmm1     db 16 dup (?)
    RegXmm2     db 16 dup (?)
    RegXmm3     db 16 dup (?)
    RegXmm4     db 16 dup (?)
    RegXmm5     db 16 dup (?)
    RegXmm6     db 16 dup (?)
    RegXmm7     db 16 dup (?)
    RegXmm8     db 16 dup (?)
    RegXmm9     db 16 dup (?)
    RegXmm10    db 16 dup (?)
    RegXmm11    db 16 dup (?)
    RegXmm12    db 16 dup (?)
    RegXmm13    db 16 dup (?)
    RegXmm14    db 16 dup (?)
    RegXmm15    db 16 dup (?)
    Rsv10       db 96 dup (?)
EFI_FX_SAVE_STATE_X64 ENDS


EFI_SYSTEM_CONTEXT_X64 STRUCT
    ExceptionData dq ?
    FxSaveState EFI_FX_SAVE_STATE_X64 <>
    RegDr0      dq ?
    RegDr1      dq ?
    RegDr2      dq ?
    RegDr3      dq ?
    RegDr6      dq ?
    RegDr7      dq ?
    RegCr0      dq ?
    RegCr1      dq ?  ;Reserved
    RegCr2      dq ?
    RegCr3      dq ?
    RegCr4      dq ?
    RegCr8      dq ?
    RegRflags   dq ?
    RegLdtr     dq ?
    RegTr       dq ?
    RegGdtr     dq ?, ?
    RegIdtr     dq ?, ?
    RegRip      dq ?
    RegGs       dq ?
    RegFs       dq ?
    RegEs       dq ?
    RegDs       dq ?
    RegCs       dq ?
    RegSs       dq ?
    RegRdi      dq ?
    RegRsi      dq ?
    RegRbp      dq ?
    RegRsp      dq ?
    RegRbx      dq ?
    RegRdx      dq ?
    RegRcx      dq ?
    RegRax      dq ?
    RegR8       dq ?
    RegR9       dq ?
    RegR10      dq ?
    RegR11      dq ?
    RegR12      dq ?
    RegR13      dq ?
    RegR14      dq ?
    RegR15      dq ?
EFI_SYSTEM_CONTEXT_X64 ENDS

INT_DATA_NO_EX STRUCT
    Retoffs     dq ?
    RetSeg      dq ?
    Flags       dq ?
    RegRsp      dq ?
    RegSs       dq ?
INT_DATA_NO_EX ENDS

INT_DATA_EX STRUCT
    ErrorCode   dq ?
    Retoffs     dq ?
    RetSeg      dq ?
    Flags       dq ?
    RegRsp      dq ?
    RegSs       dq ?
INT_DATA_EX ENDS

INT_DATA UNION
    NoEx    INT_DATA_NO_EX <>
    Ex      INT_DATA_EX <>
INT_DATA ENDS

INT_HDL_ENTRY_STACK STRUCT
    RegRdi      dq ?
    Handler     dq ?
    RegRax      dq ?
    Exception   dq ?
    IntData INT_DATA <>
INT_HDL_ENTRY_STACK ENDS


align 16
CpuSupportInterruptHandlerStart equ $



CommonEntry:
        cli
        push    rbx
        mov     rbx, [rsp + 10h]
        ;mov rax, InterruptPtrTable
            db 48h, 0B8h
InterruptPtrTableAddrFixup EQU $
            dq  0
        lea     rax, [rax + rbx * 8]
        cmp     qword ptr [rax], 00h    ; Zero if no int handler installed.
        pop     rbx
        je      NoIntHandler

        push    qword ptr [rax]         ;push handler
        push    rdi                     ;Must manually update edi since it is being destoryed.
        mov     rdi, rsp

        ;rsp is aligned for fxsave below.
        and     rsp, 0fffffff0h         ;Align to 16-byte boundary.

        ;The registers are stored on stack to match EFI_SYSTEM_CONTEXT_X64
        push    r15
        push    r14
        push    r13
        push    r12
        push    r11
        push    r10
        push    r9
        push    r8
        push    rax
        push    rcx
        push    rdx
        push    rbx
        push    rsp
        push    rbp
        push    rsi
        push    rdi
        xor     eax, eax
        mov     ax, ss
        push    rax
        mov     ax, cs
        push    rax
        mov     ax, ds
        push    rax
        mov     ax, es
        push    rax
        mov     ax, fs
        push    rax
        mov     ax, gs
        push    rax

        sub     esp, 64 ;RIP, idtr, gdtr, etc. updated later
        mov     rax, cr8
        push    rax
        mov     rax, cr4
        push    rax
        mov     rax, cr3
        push    rax
        mov     rax, cr2
        push    rax
        push    0       ;reserved
        mov     rax, cr0
        push    rax
        mov     rax, dr7
        push    rax
        mov     rax, dr6
        push    rax
        mov     rax, dr3
        push    rax
        mov     rax, dr2
        push    rax
        mov     rax, dr1
        push    rax
        mov     rax, dr0
        push    rax
        sub     rsp, size EFI_FX_SAVE_STATE_X64
        fxsave  [rsp]
        push    rax ;Exception data added later. Dummy push.

        mov     rbp, rsp    ;rbp = EFI_SYSTEM_CONTEXT_X64

        xor     eax, eax
        str     ax
        mov     (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegTr, rax

        ;Clear upper bytes
        xor     eax, eax
        mov     (EFI_SYSTEM_CONTEXT_X64 ptr [rbp + 8]).RegGdtr, rax
        sgdt    (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegGdtr
        mov     rbx, (EFI_SYSTEM_CONTEXT_X64 ptr [rbp + 2]).RegGdtr
        mov     rax, [rbx + 8 * rax]
        mov     (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegLdtr, rax

        ;Clear upper bytes
        xor     eax, eax
        mov     (EFI_SYSTEM_CONTEXT_X64 ptr [rbp + 8]).RegIdtr, rax
        sidt    (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegIdtr

        mov     rax, (INT_HDL_ENTRY_STACK ptr [rdi]).RegRax
        mov     (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegRax, rax

        mov     rax, (INT_HDL_ENTRY_STACK ptr [edi]).RegRdi
        mov     (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegRdi, rax

        ;Some Exceptions have a exception code. Others don't.
        ;INT_HDL_ENTRY_STACK is the structure on stack above EFI_SYSTEM_CONTEXT_X64.
        ;This contains the original stack, rip, exception code, and other saved registers.
        mov     rax, (INT_HDL_ENTRY_STACK ptr [rdi]).Exception
        ;8, 10 - 14, 17
        cmp     al, 7
        jle     noerrcode
        cmp     al, 9
        je      noerrcode
        ;int 15 not defined
        cmp     al, 16
        je      noerrcode
        cmp     al, 18
        jae     noerrcode
;errcode:
        mov rax, (INT_HDL_ENTRY_STACK ptr [rdi]).IntData.Ex.ErrorCode
        mov (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).ExceptionData, rax

        mov rax, (INT_HDL_ENTRY_STACK ptr [rdi]).IntData.Ex.Flags
        mov (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegRflags, rax

        mov rax, (INT_HDL_ENTRY_STACK ptr [rdi]).IntData.Ex.Retoffs
        mov (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegRip, rax
        mov rax, (INT_HDL_ENTRY_STACK ptr [rdi]).IntData.Ex.RetSeg
        mov (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegCs, rax

        mov   rax, (INT_HDL_ENTRY_STACK ptr [rdi]).IntData.Ex.RegSs
        mov   (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegSs, rax
        mov   rax, (INT_HDL_ENTRY_STACK ptr [rdi]).IntData.Ex.RegRsp
        mov   (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegRsp, rax

        jmp     gotointerrupt

noerrcode:
        xor eax, eax
        mov (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).ExceptionData, rax

        mov rax, (INT_HDL_ENTRY_STACK ptr [rdi]).IntData.NoEx.Flags
        mov (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegRflags, rax

        mov rax, (INT_HDL_ENTRY_STACK ptr [rdi]).IntData.NoEx.Retoffs
        mov (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegRip, rax
        mov rax, (INT_HDL_ENTRY_STACK ptr [rdi]).IntData.NoEx.RetSeg
        mov (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegCs, rax

        mov   rax, (INT_HDL_ENTRY_STACK ptr [rdi]).IntData.NoEx.RegSs
        mov   (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegSs, rax
        mov   rax, (INT_HDL_ENTRY_STACK ptr [rdi]).IntData.NoEx.RegRsp
        mov   (EFI_SYSTEM_CONTEXT_X64 ptr [rbp]).RegRsp, rax

gotointerrupt:

        cld

        mov     rdx, rbp                ;currently esp = ebp
        mov     rbp, rsp
        and     rsp, 0FFFFFFF0h         ;guarantee 16-byte stack alignment


        ;Call the interrupt handler which has this C function.
        ;VOID (*EFI_CPU_INTERRUPT_HANDLER) (
        ;     IN EFI_EXCEPTION_TYPE   InterruptType,
        ;     IN EFI_SYSTEM_CONTEXT   SystemContext)
        mov     rcx, (INT_HDL_ENTRY_STACK ptr [edi]).Exception
        mov     rax, (INT_HDL_ENTRY_STACK ptr [edi]).Handler

        sub     rsp, 32
        call    rax

        mov     rsp, rbp

        fxrstor (EFI_SYSTEM_CONTEXT_X64 ptr [rsp]).FxSaveState

        ;Remove data to on stack to discard.
        add     rsp, size EFI_SYSTEM_CONTEXT_X64 - 16 * 8

        ;Restore the original registers.
        add     rsp, 8  ;Skip rdi. This the original. We need the current rdi..
        pop     rsi
        pop     rbp
        add     rsp, 8  ;Skip rsp. This is the original. We the current rsp.
        pop     rbx
        pop     rdx
        pop     rcx
        pop     rax
        pop     r8
        pop     r9
        pop     r10
        pop     r11
        pop     r12
        pop     r13
        pop     r14
        pop     r15

        mov     rsp, rdi
        pop     rdi
        add     rsp, 8      ;remove handler saved on stack.
NoIntHandler:
        pop     rax
        add     rsp, 8      ;remove exception
        iretq               ;flags are restored on iretd
align 16

InterruptHandlerEnd equ $
InterruptHandlerStart    dq CpuSupportInterruptHandlerStart
InterruptHandlerSize     dq InterruptHandlerEnd - CpuSupportInterruptHandlerStart
InterruptHandlerTblFixup dd InterruptPtrTableAddrFixup - CpuSupportInterruptHandlerStart

DATA_SEL EQU 8
CODE_SEL EQU 10h


public JmpToMpStart
public JmpAddress
public JmpToMpStartSize

CPU_INFO_INIT STRUCT
    Id        dd ?
    CpuNum    dd ?
CPU_INFO_INIT ENDS

CPU_CONTROL STRUCT 8
    Function  dq ?    ;If not zero, CPU thread is executing this function.
    Context   dq ?    ;Cpu context.
    Stack     dq ?    ;Initial stack address of AP during init. The actual stack pointer can change.
    Id        dd ?    ;Apic Id
    Halted    db ?    ;Signal to halt CPU.
    Disabled  db ?
CPU_CONTROL  ENDS

ZERO_DATA   STRUCT
    Gdt             dq  ?
    GdtPadding      dd  ?       ; GDT64 takes 10 bytes
    NumAps          dd  ?
    ApGlobalData    dq  ?       ; make ApGlobalData 64 bit wide
    PageTable       dd  ?
    TOM             dd  ?
    EnteredHoldLoop dd  ?
    EightByteAlign  dd  ?
ZERO_DATA  ENDS

OFFS_NUMAPS EQU SIZEOF ZERO_DATA.Gdt + SIZEOF ZERO_DATA.GdtPadding
OFFS_PTABLE EQU OFFS_NUMAPS + SIZEOF ZERO_DATA.NumAps + SIZEOF ZERO_DATA.ApGlobalData

AP_GLOBAL_DATA_STRUCT  STRUCT
    CpuControl    dq ?    ;Pointer to CPU_CONTROL structures.
    HaltLoopEntry dq ?    ;If runtime halt loop, jump here to halt.
    ReleaseCpu    dq ?    ;Release CPU
    Idt           dd ?    ;Interrupt table pointer
    CpuSync       dd ?    ;Count CPU for sync during init.
    BSP           dd ?    ;BSP number. Initially 0.
    NumCpu        dd ?    ;Number of CPUs including BSP.
    RunningAps    dd ?    ;AP thread executing count.
    HaltedCpus    dd ?    ;Count of all halted CPUs.
AP_GLOBAL_DATA_STRUCT  ENDS


MpStart proc
        mov   ax, DATA_SEL
        mov   ds, ax
        mov   es, ax
        mov   ss, ax
        mov   fs, ax
        mov   gs, ax

        cld

; msr(1b).[10] - x2
; msr(802).[31:0] -> apic id
        mov   ecx, 1bh
        rdmsr
        test  ax, 400h
        jnz   short IsX2Apic
        
        mov   eax, TKN_APIC_BASE + TKN_APIC_ID_REGISTER
        mov   edi, [eax]
        shr   edi, 24
        jmp   short GetApid
        
IsX2Apic:
        mov   ecx, 802h
        rdmsr
        mov   edi, eax

GetApid:
        xor   rbx, rbx
        xor   rdx, rdx
        mov   rbx, (ZERO_DATA ptr [rdx]).ApGlobalData

IF TKN_USE_AP_HLT
        mov   rcx, 0
        mov   ecx, (AP_GLOBAL_DATA_STRUCT ptr [rbx]).Idt
        lidt  fword ptr [rcx]
ENDIF
    ;Hold CPUS until ready.
        xor   rsi, rsi
;       lock inc DWORD ptr ds:[24+TKN_MP_ZERO_DATA_ADDRESS]
        lock inc DWORD ptr (ZERO_DATA ptr [rdx]).EnteredHoldLoop

HOLD_CPU:
        pause
        xchg  rsi, (AP_GLOBAL_DATA_STRUCT ptr [rbx]).ReleaseCpu
        or    rsi, rsi        ;ecx = CPU_INFO_INIT
        jz    HOLD_CPU

        mov   (CPU_INFO_INIT ptr [rsi]).Id, edi

        lock  dec (AP_GLOBAL_DATA_STRUCT ptr [rbx]).CpuSync

HOLD_CPU2:
        pause
        mov   ecx, (CPU_INFO_INIT ptr [rsi]).CpuNum
        cmp   ecx, 0ffffffffh
        je    HOLD_CPU2

    ;Let BSP know CPU number was received.
        lock  dec (AP_GLOBAL_DATA_STRUCT ptr [rbx]).CpuSync

    ;Note here: cl = CPU number.
        xor   rsi, rsi
        mov   esi, ecx      ;ecx = esi = CPU Count
        mov   rax, sizeof CPU_CONTROL
        mul   esi           ;eax = sizeof CPU_CONTROL * CPU Number, edx = 0
        mov   rsi, (AP_GLOBAL_DATA_STRUCT ptr [rbx]).CpuControl
        add   rsi, rax      ;esi = address of CpuControl for CPU.
        mov   rsp, (CPU_CONTROL ptr [rsi]).Stack
        mov   (CPU_CONTROL ptr [rsi]).Id, edi

MP_COMMON_ENTRY::
        mov   rax, cr4
        or    ax, 1 SHL 9 + 1 SHL 10
        mov   cr4, rax      ;In CR4, set OSFXSR bit 9 and OSXMMEXCPT bit 10

;---Holding loop for CPU threads---
HOLD_CPU3:
        pause
        cmp   (CPU_CONTROL ptr [rsi]).Halted, 0
        jne   Halt_Cpu

    ;Number of Running CPUs set before CPU is released here.
        cmp   (CPU_CONTROL ptr [rsi]).Function, 0
IF NOT TKN_USE_AP_HLT
        je    HOLD_CPU3
ELSE
        jne   Funcexec
        hlt
        jmp   HOLD_CPU3

Funcexec:
ENDIF
        push  rbx
        push  rsi
        push  rcx
        push  rdx
        sub   rsp, 32

        mov   rdx, (CPU_CONTROL ptr [rsi]).Context
        call  (CPU_CONTROL ptr [rsi]).Function

        add   rsp, 32
        pop   rdx
        pop   rcx
        pop   rsi
        pop   rbx

        cmp   (CPU_CONTROL ptr [rsi]).Function, -1          ;Check if BSP change
        jne   @f
    ;--Switch BSP here--
        xchg  (AP_GLOBAL_DATA_STRUCT ptr [rbx]).BSP, ecx    ;ecx = new AP, set new BSP
    ;Get address of CpuControl for CPU.
        mov   rsi, rcx
        mov   rax, sizeof CPU_CONTROL
        mul   rsi           ;rax = sizeof CPU_CONTROL * CPU Number, rdx = 0
        mov   rsi, (AP_GLOBAL_DATA_STRUCT ptr [rbx]).CpuControl
        add   rsi, rax      ;rsi = address of CpuControl for CPU.
@@:
        mov   (CPU_CONTROL ptr [rsi]).Function, 0

    ;Sync CPUs
        lock  dec (AP_GLOBAL_DATA_STRUCT ptr [rbx]).RunningAps  ;Num of Cpus in wait loop.

        jmp   HOLD_CPU3
;---Cpus that are halted come here.---
Halt_Cpu:
        lock  inc (AP_GLOBAL_DATA_STRUCT ptr [rbx]).HaltedCpus

    ;If Halt loop in ram, jump to it.
        cmp   (AP_GLOBAL_DATA_STRUCT ptr [rbx]).HaltLoopEntry, 0
        je    @f
        jmp   (AP_GLOBAL_DATA_STRUCT ptr [rbx]).HaltLoopEntry
@@:
        cli
        hlt
        jmp   short @b
MpStart endp

MpRestart proc
        mov   ax, DATA_SEL
        mov   ds, ax
        mov   es, ax
        mov   ss, ax
        mov   fs, ax
        mov   gs, ax

        cld
        xor   rbx, rbx

        xor   rdx, rdx
        lock  inc DWORD ptr (ZERO_DATA ptr [rdx]).NumAps
;       mov   ebx, ds:[12+TKN_MP_ZERO_DATA_ADDRESS]       ;Get AP_GLOBAL_DATA_STRUCT
;Agd64  mov   ebx, (ZERO_DATA ptr [rdx]).ApGlobalData
        mov   rbx, (ZERO_DATA ptr [rdx]).ApGlobalData
    ; retrive the IDT table, the AP's IDTR will be erased after waking up by SIPI
    ; the IDT table pointer is saved in StartAllAps of CpuMp.c
        mov   rcx, 0
        mov   ecx, (AP_GLOBAL_DATA_STRUCT ptr [rbx]).Idt
        lidt  fword ptr [rcx]

        mov   rcx, (AP_GLOBAL_DATA_STRUCT ptr [rbx]).ReleaseCpu
    ;Note here: cl = CPU number.
        mov   esi, ecx          ;ecx = esi = CPU Count
        mov   eax, sizeof CPU_CONTROL
        mul   esi               ;eax = sizeof CPU_CONTROL * CPU Number, edx = 0
        mov   rsi, (AP_GLOBAL_DATA_STRUCT ptr [rbx]).CpuControl
        add   esi, eax          ;esi = address of CpuControl for CPU.
        mov   rsp, (CPU_CONTROL ptr [rsi]).Stack

        mov   (CPU_CONTROL ptr [rsi]).Function, 0
        cmp   (CPU_CONTROL ptr [rsi]).Halted, 0
        jz    @f
        mov   (CPU_CONTROL ptr [rsi]).Halted, 0
        dec   (AP_GLOBAL_DATA_STRUCT ptr [rbx]).HaltedCpus
@@:
        lock  inc DWORD ptr (ZERO_DATA ptr [rdx]).EnteredHoldLoop
        jmp   MP_COMMON_ENTRY
MpRestart endp



JmpToMpStart proc
        ;***NOTE***
        ;***NOTE***
        ;***NOTE***
        ;---16 bit in 32 bit assembler--
        ;---Some 16-bit and 32-bit assembly is the same, others are not.---
        ;---Need to use some machine code.---

        ;---------------------------------------------------------------
        ; (*ZERO_DATA)->NumAps++ (Count APs)
        ;---------------------------------------------------------------
        ;lock inc byte ptr [(ZERO_DATA).NumAps + TKN_MP_ZERO_DATA_ADDRESS]  ;Count CPU
        db 0f0h, 66h, 0ffh, 6
        dw OFFS_NUMAPS + TKN_MP_ZERO_DATA_ADDRESS

        ;---------------------------------------------------------------
        ;Switch to protected mode
        ;---------------------------------------------------------------
        ;lgdt fword ptr [ ]
        db  66h,0fh,1,16h
        dw  SIZEOF ZERO_DATA

        mov   rax, cr0        ;===>actualy mov eax, cr0
        or    al, 1           ;Set PE bit
        mov   cr0, rax        ;===>actualy mov cr0, eax

        ;---------------------------------------------------------------
        ; In 16 bit protected mode, hack retf to initialize CS by
        ; first pushing the new CS and then pushing/modifying
        ; the EIP so that retf returns @ In_prot_mode.
        ;---------------------------------------------------------------
        ;;jmp $
        ;db 0ebh, 0feh

        ;In 16 bit protected mode
        ;jmp  10h:In_prot_mode
        db    66h, 0EAh
        dw    ((In_prot_mode - JmpToMpStart) + TKN_MP_JUMP_FUNCTION_ADDRESS)
        dw    00h
        dw    CODE_SEL
In_prot_mode:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        ;mov eax, [16+TKN_MP_ZERO_DATA_ADDRESS]  ;PageTable
        db    8bh, 5
        dd    OFFS_PTABLE

        mov   cr3, rax        ;Set CR3 to first page directory pointer table

        mov   rax, cr4        ;actually eax
        or    al, 020h        ;Enable PAE
        mov   cr4, rax

    ;Enable long mode in msr register. Doesn't actually enter long mode yet.
        mov   ecx, 0c0000080h
        rdmsr
        bts   eax, 8
        wrmsr

    ;Enable paging
        mov   rax, cr0        ;actually eax
        bts   eax, 31
        mov   cr0, rax        ;Now in long mode compatibility.
        jmp   @f

@@:
        db    0EAh
        dd    ((In_long_mode - JmpToMpStart) + TKN_MP_JUMP_FUNCTION_ADDRESS)
        dw    38h                 ; 64 bit code selector
In_long_mode:
        mov   eax, TKN_MP_ZERO_DATA_ADDRESS
        lgdt  fword ptr[eax]
        db    48h
        lea   eax, JmpAddress
        jmp   qword ptr[rax]

JmpAddress  dd  ?
JAddr1      dd  0

JmpToMpStart endp

JmpToMpStartEnd   equ $
JmpToMpStartSize  dq offset JmpToMpStartEnd - JmpToMpStart



WaitUntilZero8 Proc
@@:
  mov al, [rcx]
  or  al, al
  pause
  jnz short @b
  ret
WaitUntilZero8 endp

WaitUntilZero32 Proc
@@:
  mov eax, [rcx]
  or  eax, eax
  pause
  jnz short @b
  ret
WaitUntilZero32 endp

CPULib_LockByteInc proc
  lock inc byte ptr [rcx]
  ret
CPULib_LockByteInc endp

CPULib_LockDwordInc proc
  lock inc dword ptr [rcx]
  ret
CPULib_LockDwordInc endp

WaitForSemaphore Proc
  mov  al, 1
@@:
  xchg al, [rcx]
  or  al, al
  pause
  jnz  @b
  ret
WaitForSemaphore endp

END


