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

.data
EXTRN gSmmPort:WORD


.code

UINT8    TYPEDEF    BYTE
UINT16   TYPEDEF    WORD
UINT32   TYPEDEF    DWORD

;ULONG
;SMI_CALL (
;    UINT8                  Func,
;    ADDRESS                Address
;    );
SMI_CALL PROC USES rbx

    xor     rax,    rax
    mov     al,     cl
    mov     rbx,    RDX
    mov     dx,     gSmmPort
    out     dx,     al
    nop
    nop
    nop
       
    ret
    
SMI_CALL  ENDP


END
