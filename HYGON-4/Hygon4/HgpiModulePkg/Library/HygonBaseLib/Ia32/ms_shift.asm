;/**
; * @file
; *
; * Hgpi library 32bit
; *
; * Contains HYGON HGPI Library
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      HGPI
; * @e sub-project:  Lib
; */
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

.586p
.model  flat
ASSUME FS:NOTHING
.code
;/*++
;
;Routine Description:
;
;  Shifts a UINT64 to the right.
;
;Arguments:
;
;  EDX:EAX - UINT64 value to be shifted
;  CL -      Shift count
;
;Returns:
;
;  EDX:EAX - shifted value
;
;--*/
_allshr  PROC NEAR C PUBLIC
_aullshr  PROC NEAR C PUBLIC
        .if (cl < 64)
            .if (cl >= 32)
                sub    cl, 32
                mov    eax, edx
                xor    edx, edx
            .endif
            shrd   eax, edx, cl
            shr    edx, cl
        .else
            xor    eax, eax
            xor    edx, edx
        .endif
        ret
_aullshr ENDP
_allshr ENDP

;/*++
;
;Routine Description:
;
;  Shifts a UINT64 to the left.
;
;Arguments:
;
;  EDX:EAX - UINT64 value to be shifted
;  CL -      Shift count
;
;Returns:
;
;  EDX:EAX - shifted value
;
;--*/
_allshl  PROC NEAR C PUBLIC USES CX
        .if (cl < 64)
            .if (cl >= 32)
                sub    cl, 32
                mov    edx, eax
                xor    eax, eax
            .endif
            shld   edx, eax, cl
            shl    eax, cl
        .else
            xor    eax, eax
            xor    edx, edx
        .endif
        ret
_allshl ENDP

END
