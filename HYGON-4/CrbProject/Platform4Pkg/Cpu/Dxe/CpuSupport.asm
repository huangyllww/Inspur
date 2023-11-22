
ifndef EFIx64

.686p
.model  flat
.code

extern _gC1EStackSave:DWORD
extern _gC1ESupport:BYTE



EnableC1E       PROC    C
  ;Some CPUs may generate a GPF if C1E is not supported.
  ; This routine and the temp C1E GPF handler takes care of this.
  ; Enable Enhanced Halt State C1E. Note: Need setup question?

  ;This routine is not reentrant.
  pushad
  push   EnableC1Eerror      ;If gpf occurs, C1E gpf handler will give control to this.
  mov    _gC1EStackSave, esp    ;Save esp for handler.

  mov    ecx, 1a0h    ;IA32_MISC_ENABLE
  rdmsr
  or    eax, 1 SHL 25  ;enable C1E
  wrmsr
  rdmsr
  bt    eax, 25
  jc    @f        ;Cary set if enabled
  mov    _gC1ESupport, 0

@@:
  add    esp, 4      ;pop from stack.
EnableC1Eerror  equ $
  popad
  ret
EnableC1E ENDP



_TempGPInterruptHandler PROC
  mov _gC1ESupport, 0
  mov esp, _gC1EStackSave
  ret
_TempGPInterruptHandler ENDP


public _MachineCheckHandlerSize

MachineCheckHandler     PROC C

@@:
        cli
        hlt
        jmp     @b
        ret
MachineCheckHandler     ENDP
MachineCheckHandlerEnd equ $
_MachineCheckHandlerSize dd offset MachineCheckHandlerEnd - MachineCheckHandler

else
.code
extern gC1EStackSave:QWORD
extern gC1ESupport:BYTE



EnableC1E       PROC
  ;Some CPUs may generate a GPF if C1E is not supported.
  ; This routine and the temp C1E GPF handler takes care of this.
  ;Enable Enhanced Halt State C1E. Note: Need setup question?

  ;This routine is not reentrant.
;  pushad
;   markw this next instruction is generating an error.
;  push   qword ptr EnableC1Eerror      ;If gpf occurs, C1E gpf handler will give control to this.
    mov    gC1EStackSave, rsp    ;Save esp for handler.

  mov    ecx, 1a0h    ;IA32_MISC_ENABLE
  rdmsr
  or    eax, 1 SHL 25  ;enable C1E
  wrmsr
  rdmsr
  bt    eax, 25
  jc    @f        ;Cary set if enabled
  mov    gC1ESupport, 0

@@:
  add    rsp, 4      ;pop from stack.
EnableC1Eerror  equ $
;  popad
  ret
EnableC1E       ENDP



TempGPInterruptHandler  PROC
  mov gC1ESupport, 0
  mov rsp, gC1EStackSave
  ret
TempGPInterruptHandler  ENDP


public MachineCheckHandlerSize



MachineCheckHandler     PROC

@@:
        cli
        hlt
        jmp     @b
        ret
MachineCheckHandler     ENDP
MachineCheckHandlerEnd equ $
MachineCheckHandlerSize dd offset MachineCheckHandlerEnd - MachineCheckHandler



SaveC1EContext  PROC
        pop     rax     ; Get RIP
        push    rbx
        push    rcx
        push    rdx
        push    r8
        push    r9
        push    r10
        push    r11
        push    rax     ; Restore RIP
        ret
SaveC1EContext  ENDP



RestoreC1EContext       PROC
        pop     rax     ; Get rip
        pop     r11
        pop     r10
        pop     r9
        pop     r8
        pop     rdx
        pop     rcx
        pop     rbx
        push    rax     ; Restore rip
        ret
RestoreC1EContext       ENDP

AsmIret PROC
        iretq
AsmIret ENDP

endif

END


