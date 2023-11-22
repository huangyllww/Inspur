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

PUBLIC  gSmmPort
gSmmPort dw 0x82F

;UINT32
;SMI_CALL(
;    UINT8    SmiValue,
;    UINT32   Address
;);

SMI_CALL PROC PUBLIC Func:BYTE, Address:DWORD
    push    ebx
    mov     ebx,    Address
    mov     al,     Func  
    mov     dx,     gSmmPort
    out     dx,     al
    pop     ebx
    nop
    nop
    nop
       
    ret
    
SMI_CALL  ENDP

END