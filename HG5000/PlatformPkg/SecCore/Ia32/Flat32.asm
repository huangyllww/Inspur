;
;      NOTICE: Copyright (c) 2006 - 2020 Byosoft Corporation. All rights reserved.
;              This program and associated documentation (if any) is furnished
;              under a license. Except as permitted by such license,no part of this
;              program or documentation may be reproduced, stored divulged or used
;              in a public system, or transmitted in any form or by any means
;              without the express written consent of Byosoft Corporation.
;
;
; Module Name:
;
;  Flat32.asm
;
; Abstract:
;
;  Sec code before "C"
;
;------------------------------------------------------------------------------
  INCLUDE AmdUefiStack.inc

.686p
.xmm
.model small, c

EXTRN   SecStartup:NEAR
EXTRN   PcdGet32 (PcdFlashFvRecoveryBase):DWORD
EXTRN   PcdGet32 (PcdFlashFvRecoverySize):DWORD
EXTRN   PcdGet32 (PcdFlashAreaBaseAddress):DWORD
EXTRN   PcdGet32 (PcdFlashAreaSize):DWORD
EXTRN   PcdGet32 (PcdTemporaryRamBase):DWORD
EXTRN   PcdGet32 (PcdTemporaryRamSize):DWORD
EXTRN   PcdGet16 (AcpiIoPortBaseAddress):WORD

HIGH_MEMORY_REGION_BASE	EQU   	1000000h
FW_CODE_AREA_START      EQU     HIGH_MEMORY_REGION_BASE
FW_CODE_AREA_SIZE       EQU     _PCD_VALUE_PcdFlashAreaSize
FW_CODE_AREA_SIZE_MASK  EQU     (NOT (FW_CODE_AREA_SIZE - 1))
MTRR_VAR_TOP_MASK_VALUE EQU     0FFFFh 

MSR_XAPIC_BASE      EQU   01Bh

PM_IO_INDEX         EQU 0CD6H
PM_IO_DATA          EQU 0CD7H

WriteProtect            EQU     5
WriteBack               EQU     6
ValidMask               EQU     1 SHL 11
MTRR_PHYS_BASE_7        EQU     020Eh
MTRR_VAR_TOP_MASK_VALUE EQU     0FFFFh        ; For Fam15 - 48 bits

READ_IO_PMU MACRO
    mov     al, ah
    mov     dx, PM_IO_INDEX
    out     dx, al
    out     0edh, al                ; I/O delay
    out     0edh, al                ; I/O delay
    mov     dx, PM_IO_DATA
    in      al, dx
    out     0edh, al                ; I/O delay
    out     0edh, al                ; I/O delay
ENDM

WRITE_IO_PMU MACRO
    xchg    al, ah  ;AL = Reg.
                    ;AH = Data
    mov     dx, PM_IO_INDEX
    out     dx, al
    out     0edh, al                ; I/O delay
    out     0edh, al                ; I/O delay
    xchg    al, ah
    mov     dx, PM_IO_DATA
    out     dx, al
    out     0edh, al                ; I/O delay
    out     0edh, al                ; I/O delay
ENDM


STATUS_CODE MACRO status
  mov  al,  status
  out  80h, al
ENDM

CALL_MMX macro   RoutineLabel
  local   ReturnAddress
  mov     esi, offset ReturnAddress
  movd    mm7, esi                      ; save ReturnAddress into MM7
  jmp     RoutineLabel
ReturnAddress:
endm

RET_MMX  macro
  movd    esi, mm7                      ; restore ESP from MM7
  jmp     esi
endm



_TEXT_REALMODE SEGMENT PARA PUBLIC USE16 'CODE'
  ASSUME  CS:_TEXT_REALMODE, DS:_TEXT_REALMODE

align 4
_ModuleEntryPoint PROC NEAR C PUBLIC

  cli

  movd    mm0, eax

  rdtsc
  movd    mm1, eax

  STATUS_CODE (02h)                     ; BSP_PROTECTED_MODE_START
  mov     esi,  OFFSET GdtDesc
  DB      66h
  lgdt    fword ptr cs:[si]
  mov     eax, cr0                      ; Get control register 0
  or      eax, 00000003h                ; Set PE bit (bit #0) & MP bit (bit #1)
  mov     cr0, eax                      ; Activate protected mode
  mov     eax, cr4                      ; Get control register 4
  or      eax, 00000600h                ; Set OSFXSR bit (bit #9) & OSXMMEXCPT bit (bit #10)
  mov     cr4, eax

  ;
  ; Now we're in Protected16
  ; Set up the selectors for protected mode entry
  ;
  mov     ax, SYS_DATA_SEL
  mov     ds, ax
  mov     es, ax
  mov     gs, ax
  mov     fs, ax
  mov     ss, ax

  ;
  ; Go to Protected32
  ;
  mov     esi, offset NemInitLinearAddress
  jmp     fword ptr cs:[si]

_ModuleEntryPoint ENDP
_TEXT_REALMODE      ENDS


; mm0 is used to save cpu BIST
; mm1 is used to save cpu tsc

_TEXT_PROTECTED_MODE SEGMENT PARA PUBLIC USE32 'CODE'
  ASSUME  CS:_TEXT_PROTECTED_MODE, DS:_TEXT_PROTECTED_MODE

align 16
ProtectedModeSECStart PROC NEAR PUBLIC

; we are in memory start from 0x1000000
; jmp to memory to run fast.

  mov     eax, $
  and eax, (HIGH_MEMORY_REGION_BASE + 0ffffffh)
  add     eax, 0fh
  jmp     eax

  mov     dx, 0cf8h
  mov     eax, 8000c184h			; (0, 18, 1, 84)
  out     dx, eax
  add     dx, 4
  mov     eax, 00FED880h
  out     dx, eax
  
  mov     dx, 0cf8h
  mov     eax, 8000c180h			; (0, 18, 1, 80)
  out     dx, eax
  add     dx, 4
  mov     eax, 00FED803h
  out     dx, eax


; AMD_ENABLE_UEFI_STACK2 will destory mm register, so here we need protect it.
  movd ebp, mm1
  movd ebx, mm0
  mov  ecx, FW_CODE_AREA_SIZE
  mov  edx, 0                                     ; zero for (4GB - size)
  AMD_ENABLE_UEFI_STACK2 STACK_AT_BOTTOM, _PCD_VALUE_PcdTemporaryRamSize, _PCD_VALUE_PcdTemporaryRamBase


  mov     ecx, MSR_XAPIC_BASE             ; Enable local APIC
  rdmsr
  bts     eax, 11                         ; Enable
  wrmsr
  
; (VRT_T2)
; This field specifies the time of VRT_Enable being low for the RTC battery monitor circuit in 4 ms increments.  
  mov ah, 059h                            ; PM_Reg 59: VRT_T2
  mov al, 10h                             ; 10h is the recommended value by AMD
  WRITE_IO_PMU 
  
  ;;
  ;; Enable following Io decoding -
  ;;    0x20, 0x21, 0xA0, 0xA1 (PIC);
  ;;    0x40, 0x41, 0x42, 0x43, 0x61 (8254 timer);
  ;;    0x70, 0x71, 0x72, 0x73 (RTC);
  ;;    0x92. 
  ;;
  mov     ah, 00h
  READ_IO_PMU
  or      al, 01h
  WRITE_IO_PMU  
  
  ;;
  ;; Disable BootTimer (watch dog)
  ;;
  mov ah, 47h
  READ_IO_PMU
  or al, 80h
  WRITE_IO_PMU  

  ;;
  ;; Enable LPC bridge
  ;;
  mov     ah, 0ECh
  READ_IO_PMU
  or      al, 001h
  WRITE_IO_PMU  
  
  STATUS_CODE (07h)
  jmp  CallPeiCoreEntryPoint

ProtectedModeSECStart ENDP


CallPeiCoreEntryPoint   PROC    NEAR    PRIVATE

  ; Switch to "C" code
  STATUS_CODE (0Ch)
  
  mov     esp, PcdGet32(PcdTemporaryRamBase)
  add     esp, PcdGet32(PcdTemporaryRamSize) 

;	pushd   027fh
;	fldcw   WORD PTR [esp]          ; Set FP Control Word according UEFI
;	add     esp, 4

  rdtsc
  push    eax         ; JmpSecCoreTsc
  
  movd    eax, mm1		; ResetTsc
  push    eax
  
  movd    eax, mm0    ; BIST
  push    eax

  mov     edi, PcdGet32(PcdFlashFvRecoveryBase)
  and     edi, (HIGH_MEMORY_REGION_BASE + 0ffffffh)
  push    edi
  ;push    PcdGet32(PcdFlashFvRecoveryBase)
  push    PcdGet32(PcdTemporaryRamBase)
  push    PcdGet32(PcdTemporaryRamSize)
  call    SecStartup
  
CallPeiCoreEntryPoint   ENDP


Iam_AP:
@@:
    cli
    hlt
    jmp @B
;============================ PSP RESUME ==================================
;typedef struct _PSP_SMM_RSM_MEM_INFO {
;  UINT32                  BspStackSize;           // BSP Stack Size for resume
;  UINT32                  ApStackSize;            // AP Stack Size for resume
;  EFI_PHYSICAL_ADDRESS    StackPtr;              // Point to the base of Stack
;  EFI_PHYSICAL_ADDRESS    RsmHeapPtr;            // Point to the base of Resume Heap
;  UINT32                  HeapSize;               // Reserved Heap Size
;  UINT32                  TempRegionSize;         // Reserved Temp Region Size
;  EFI_PHYSICAL_ADDRESS    TempRegionPtr;         // Point to the base of Temporary Region (used for store the dynamic value during SMM SEC Phase)
;} PSP_SMM_RSM_MEM_INFO;


SECCore_PspResEntry::
        mov     esi, eax                ; Save EAX - pointer to PSP_SMM_RSM_MEM_INFO
;Clear Long Mode Enable 
        mov     ecx, 0c0000080h         ; EFER MSR number. 
        rdmsr                           
        btr     eax, 8                  ; Set LME=0 
        wrmsr

;STATUS_CODE (11h)
        xor     edx, edx
;DM1-268, Setup MTRR Start >>>
; Use variable MTRRs to set ROM space to WP
        mov     eax, (_PCD_VALUE_PcdFlashAreaBaseAddress OR WriteProtect)
        mov     ecx, MTRR_PHYS_BASE_7
        wrmsr
        
; Write the size
        mov     eax, (FW_CODE_AREA_SIZE_MASK OR ValidMask)
        mov     edx, MTRR_VAR_TOP_MASK_VALUE
        inc     ecx                     ; MTRR_PHYS_MASK_7
        wrmsr
;DM1-268,Setup MTRR Ends <<<

        mov     ecx, MSR_XAPIC_BASE
        rdmsr
        
        and     eax, 0100h
        jnz     @f
        jmp     Iam_AP

ALIGN 4
        db      51h,52h,53h,54h
        dd      OFFSET GDT_BASE         ; GDT offset
        dw      0010h                   ; CODE selector
        dw      0018h                   ; DATA selector
        dd      OFFSET SECCore_PspResEntry ; RSM Entry Point
        dd      00000EDFh               ; RSM EDX signature
                
@@:
        call    SECCommonInit

; Call SEC C CORE
        mov     eax, PcdGet32(PcdFlashFvRecoveryBase)
        push    eax
        mov     eax, dword ptr[esi + 8]    ; STACK_BASE_ADDRESS
        push    eax
        mov     eax, dword ptr[esi]        ; STACK_TOTAL_SIZE
        push    eax
        call    SecStartup
;==========================================================================
;==========================================================================
SECCommonInit PROC NEAR PUBLIC
; SAVE ESP in MMX register
        movd    mm0, esp
        mov     ecx, MSR_XAPIC_BASE             ; Enable local APIC
        rdmsr
        bts     eax, 11 ; Enable the APIC
        wrmsr
        movd    esp, mm0

        ;---  Adding MMIO Base and Limit - Early setup needed for system boot with real Hw

        mov     dx, 0cf8h
        mov     eax, 8000c184h
        out     dx, eax
        add     dx, 4
        mov     eax, 00FED880h
        out     dx, eax
        mov     dx, 0cf8h
        mov     eax, 8000c180h
        out     dx, eax
        add     dx, 4
        mov     eax, 00FED803h
        out     dx, eax
        
        
        ; re-programm PM_BASE
        push    ebx
        mov     ebx, 0FED80362h         ; Pm1CtlBlock Base
        mov     ax, _PCD_VALUE_AcpiIoPortBaseAddress
        add     ax, 4
        mov     WORD PTR [ebx], ax
        pop     ebx
        
    ;PMx04, Enable ACPI MMIO range (FED8_0000h-FED8_1FFFh).
  mov     ah, 04h
  READ_IO_PMU
  or      al, 02h
  WRITE_IO_PMU  
  
; (VRT_T2)
; This field specifies the time of VRT_Enable being low for the RTC battery monitor circuit in 4 ms increments.  
  mov ah, 059h                            ; PM_Reg 59: VRT_T2
  mov al, 10h                             ; 10h is the recommended value by AMD
  WRITE_IO_PMU 

  ; PMx00, enable SMBUS decode, legacyIO Enable  
  ;;
  ;; Enable following Io decoding -
  ;;    0x20, 0x21, 0xA0, 0xA1 (PIC);
  ;;    0x40, 0x41, 0x42, 0x43, 0x61 (8254 timer);
  ;;    0x70, 0x71, 0x72, 0x73 (RTC);
  ;;    0x92. 
  ;;
  mov     ah, 00h
  READ_IO_PMU
  or      al, 11h
  WRITE_IO_PMU  
  
  ;;
  ;; Disable BootTimer (watch dog)
  ;;
  mov ah, 47h
  READ_IO_PMU
  or al, 80h
  WRITE_IO_PMU  

  ;;
  ;; Enable LPC bridge
  ;;
  mov     ah, 0ECh
  READ_IO_PMU
  or      al, 001h
  WRITE_IO_PMU  
  
  STATUS_CODE (07h)
  ret
SECCommonInit ENDP
        
align 10h
PUBLIC  BootGDTtable

;
; GDT[0]: 0x00: Null entry, never used.
;
NULL_SEL        EQU $ - GDT_BASE        ; Selector [0]
GDT_BASE:
BootGDTtable        DD  0
                    DD  0
;
; Linear data segment descriptor
;
LINEAR_SEL      EQU $ - GDT_BASE        ; Selector [0x8]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  092h                            ; present, ring 0, data, expand-up, writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0
;
; Linear code segment descriptor
;
LINEAR_CODE_SEL EQU $ - GDT_BASE        ; Selector [0x10]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  09Bh                            ; present, ring 0, data, expand-up, not-writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0
;
; System data segment descriptor
;
SYS_DATA_SEL    EQU $ - GDT_BASE        ; Selector [0x18]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  093h                            ; present, ring 0, data, expand-up, not-writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0

;
; System code segment descriptor
;
SYS_CODE_SEL    EQU $ - GDT_BASE        ; Selector [0x20]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  09Ah                            ; present, ring 0, data, expand-up, writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0
;
; Spare segment descriptor
;
SYS16_CODE_SEL  EQU $ - GDT_BASE        ; Selector [0x28]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0Eh                             ; Changed from F000 to E000.
    DB  09Bh                            ; present, ring 0, code, expand-up, writable
    DB  00h                             ; byte-granular, 16-bit
    DB  0
;
; Spare segment descriptor
;
SYS16_DATA_SEL  EQU $ - GDT_BASE        ; Selector [0x30]
    DW  0FFFFh                          ; limit 0xFFFF
    DW  0                               ; base 0
    DB  0
    DB  093h                            ; present, ring 0, data, expand-up, not-writable
    DB  00h                             ; byte-granular, 16-bit
    DB  0

;
; Spare segment descriptor
;
SPARE5_SEL      EQU $ - GDT_BASE        ; Selector [0x38]
    DW  0                               ; limit 0
    DW  0                               ; base 0
    DB  0
    DB  0                               ; present, ring 0, data, expand-up, writable
    DB  0                               ; page-granular, 32-bit
    DB  0
GDT_SIZE        EQU $ - BootGDTtable    ; Size, in bytes

GdtDesc:                                ; GDT descriptor
OffsetGDTDesc   EQU $ - _ModuleEntryPoint
    DW  GDT_SIZE - 1                    ; GDT limit
    DD  OFFSET BootGDTtable             ; GDT base address

NemInitLinearAddress   LABEL   FWORD
NemInitLinearOffset    LABEL   DWORD
    DD  OFFSET ProtectedModeSECStart    ; Offset of our 32 bit code
    DW  LINEAR_CODE_SEL

_TEXT_PROTECTED_MODE    ENDS
END

