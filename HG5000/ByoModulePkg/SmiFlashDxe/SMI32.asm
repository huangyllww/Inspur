  .686
  .MODEL FLAT,C
  .CODE


UINT8    TYPEDEF    BYTE
UINT16   TYPEDEF    WORD
UINT32   TYPEDEF    DWORD
;PUBLIC  gSmmPort
;gSmmPort dw 0B2h

SMI_CALL PROC PUBLIC Func:UINT8, SubFunc:UINT8, Address:UINT32
    
    push    ebx
    xor     eax,    eax
    mov     al,     Func
    mov     ah,     SubFunc
    mov     ebx,    Address
    mov     dx,     gSmmPort
    out     dx,     al
    nop
    nop
    nop
       
    pop     ebx
    ret
    
SMI_CALL  ENDP

END
