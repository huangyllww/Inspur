
.data
EXTRN gSmmPort:WORD


.code

UINT8    TYPEDEF    BYTE
UINT16   TYPEDEF    WORD
UINT32   TYPEDEF    DWORD

;ULONG
;SMI_CALL (
;    UINT8                  Func,
;    UINT8                  SubFunc,
;    ADDRESS                Address
;    );
SMI_CALL PROC USES rbx

    xor     rax,    rax
    mov     al,     cl
    mov     ah,     dl
    mov     rbx,    r8
    mov     dx,     gSmmPort
    out     dx,     al
    nop
    nop
    nop
       
    ret
    
SMI_CALL  ENDP


END
