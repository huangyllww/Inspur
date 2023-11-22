;/**
; * @file
; *
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      HGPI
; * @e sub-project:  Ccx
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

.code

EXTRN BspMsrLocation:DWORD
EXTRN ApSyncLocation:DWORD
EXTRN AllowToLaunchNextThreadLocation:DWORD
EXTRN ApStackBasePtr:NEAR PTR QWORD
EXTRN RegSettingBeforeLaunchingNextThread:NEAR
EXTRN ApEntryPointInC:NEAR
EXTRN ApGdtDescriptor:WORD
EXTRN IsX2ApicMode:BYTE

ApAsmCode PROC

  mov ax, 30h
  mov ds, ax
  mov es, ax
  mov ss, ax

  ; Check if X2APIC is enabled
  mov al, IsX2ApicMode
  cmp al, 0
  jz  XAPIC_READ_APICID

X2APIC_READ_APICID:
  mov ecx, 01Bh
  rdmsr
  or  eax, 0800h ; Enable XAPIC by setting MSRx0000001B[11] = 1
  wrmsr
  or  eax, 0C00h ; Enable X2APIC by setting MSRx0000001B[11:10] = 2'b11
  wrmsr

  mov ecx, 0802h
  rdmsr
  mov ebx, eax

  jmp  END_READ_APICID

XAPIC_READ_APICID:
  mov ecx, 01Bh
  rdmsr
  or  eax, 0800h ; Enable XAPIC by setting MSRx0000001B[11] = 1
  wrmsr

  mov eax, 1
  cpuid
  shr ebx, 24

END_READ_APICID:
  
  ; Reset RSP
  ; Use 1-based APIC ID to index into the top of this AP's stack
  xor rax, rax
  mov eax, ebx
  mov ecx, 0400h
  mul ecx

  ; Make space for the first qword
  sub eax, 8

  mov rsi, ApStackBasePtr
  add rax, rsi
  mov rsp, rax

  ; Enable Fixed MTRR modification
  mov ecx, 0C0010010h
  rdmsr
  or  eax, 00080000h
  wrmsr

  ; Setup MSRs to BSP values
  mov esi, BspMsrLocation
MsrStart:
  mov ecx, [esi]
  cmp ecx, 0FFFFFFFFh
  jz MsrDone
  add esi, 4
  mov eax, [esi]
  add esi, 4
  mov edx, [esi]
  wrmsr
  add esi, 4
  jmp MsrStart

MsrDone:
  ; Disable Fixed MTRR modification and enable MTRRs
  mov ecx, 0C0010010h
  rdmsr
  and eax, 0FFF7FFFFh
  or  eax, 00140000h
  bt  eax, 21
  jnc Tom2Disabled
  bts eax, 22
Tom2Disabled:
  wrmsr

  ; Enable caching
  mov rax, cr0
  btr eax, 30
  btr eax, 29
  mov cr0, rax

  ; Call into C code before next thread is launched
  call RegSettingBeforeLaunchingNextThread

  ; Increment call count to allow to launch next thread
  mov esi, AllowToLaunchNextThreadLocation
  lock inc WORD PTR [esi]

  ; Call into C code
  call ApEntryPointInC

  ; Set up resident GDT
  lea rsi, ApGdtDescriptor
  lgdt FWORD PTR [rsi]
  lea rbx, NewGdtAddress
  lea rsi, NewGdtOffset
  mov [rsi], ebx
  lea rsi, NewGdtOffset
  jmp far ptr [rsi]
NewGdtAddress:

  ; Increment call count to indicate core is done running
  mov esi, ApSyncLocation
  lock inc WORD PTR [esi]

  ; Hlt
@@:
  cli
  hlt
  jmp @B

ApAsmCode ENDP

NewGdtOffset LABEL DWORD
  DD  ?
NewGdtSelector LABEL WORD
  DW  0038h

END
