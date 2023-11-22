;/** @file
;
;Copyright (c) 2022, Byosoft Corporation.<BR>
;All rights reserved.This software and associated documentation (if any)
;is furnished under a license and may only be used or copied in
;accordance with the terms of the license. Except as permitted by such
;license, no part of this software or documentation may be reproduced,
;stored in a retrieval system, or transmitted in any form or by any
;means without the express written consent of Byosoft Corporation.
;
;**/

  .686
  .MODEL FLAT,C
  .CODE


UINT8    TYPEDEF    BYTE
UINT16   TYPEDEF    WORD
UINT32   TYPEDEF    DWORD
;PUBLIC  gSmmPort
;gSmmPort dw 0B2h

SMI_CALL PROC PUBLIC Func:UINT8, Address:UINT32
    
    push    ebx
    xor     eax,    eax
    mov     al,     Func
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
