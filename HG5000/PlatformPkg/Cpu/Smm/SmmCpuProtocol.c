
#include "CpuSmm.h"

extern  CPU_SMM_PRIVATE_DATA    gCpuSmmInt;
extern  CPU_SMM_I               *pCoreDataI;

typedef struct {
    UINT16 SaveStateOffset;
    UINT16 SaveStateWidth;
    BOOLEAN ReadOnly;
} AMD_X64_FORMAT;

AMD_X64_FORMAT gAMDX64Format[] = {
    //Treat special case - EFI_SMM_SAVE_STATE_REGISTER_GDTBASE = 4
    //Treat special case - EFI_SMM_SAVE_STATE_REGISTER_IDTBASE = 5
    //Treat special case - EFI_SMM_SAVE_STATE_REGISTER_LDTBASE = 6
    //Treat special case - EFI_SMM_SAVE_STATE_REGISTER_GDTLIMIT = 7
    //Treat special case - EFI_SMM_SAVE_STATE_REGISTER_IDTLIMIT = 8
    //Treat special case - EFI_SMM_SAVE_STATE_REGISTER_LDTLIMIT = 9
    //Treat special case - EFI_SMM_SAVE_STATE_REGISTER_LDTINFO = 10
    0xfe00, 2, TRUE, //EFI_SMM_SAVE_STATE_REGISTER_ES = 20
    0xfe10, 2, TRUE,//EFI_SMM_SAVE_STATE_REGISTER_CS = 21
    0xfe20, 2, TRUE,//EFI_SMM_SAVE_STATE_REGISTER_SS = 22
    0xfe30, 2, TRUE,//EFI_SMM_SAVE_STATE_REGISTER_DS = 23
    0xfe40, 2, TRUE,//EFI_SMM_SAVE_STATE_REGISTER_FS = 24
    0xfe50, 2, TRUE,//EFI_SMM_SAVE_STATE_REGISTER_GS = 25
    0xfe70, 2, TRUE, //EFI_SMM_SAVE_STATE_REGISTER_LDTR_SEL = 26
    0xfe90, 2, TRUE,   //EFI_SMM_SAVE_STATE_REGISTER_TR_SEL = 27
    0xff60, 8, TRUE,  //EFI_SMM_SAVE_STATE_REGISTER_DR7 = 28
    0xff68, 8, TRUE,   //EFI_SMM_SAVE_STATE_REGISTER_DR6 = 29
    0xffb8, 8, FALSE, //EFI_SMM_SAVE_STATE_REGISTER_R8 = 30
    0xffb0, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_R9 = 31
    0xffa8, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_R10 = 32
    0xffa0, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_R11 = 33
    0xff98, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_R12 = 34
    0xff90, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_R13 = 35
    0xff88, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_R14 = 36
    0xff80, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_R15 = 37
    0xfff8, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_RAX = 38
    0xffe0, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_RBX = 39
    0xfff0, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_RCX = 40
    0xffe8, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_RDX = 41
    0xffd8, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_RSP = 42
    0xffd0, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_RBP = 43
    0xffc8, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_RSI = 44
    0xffc0, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_RDI = 45
    0xff78, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_RIP = 46
    0, 0, TRUE, // 47
    0, 0, TRUE, // 48
    0, 0, TRUE, // 49
    0, 0, TRUE, // 50
    0xff70, 8, FALSE,//EFI_SMM_SAVE_STATE_REGISTER_RFLAGS = 51
    0xff58, 8, TRUE,//EFI_SMM_SAVE_STATE_REGISTER_CR0 = 52
    0xff50, 8, TRUE,//EFI_SMM_SAVE_STATE_REGISTER_CR3 = 53
    0xff48, 8, TRUE,//EFI_SMM_SAVE_STATE_REGISTER_CR4 = 54

    //Supported in XMM Save/restore function
    //EFI_SMM_SAVE_STATE_REGISTER_FCW = 256
    //EFI_SMM_SAVE_STATE_REGISTER_FSW = 257
    //EFI_SMM_SAVE_STATE_REGISTER_FTW = 258
    //EFI_SMM_SAVE_STATE_REGISTER_OPCODE = 259
    //EFI_SMM_SAVE_STATE_REGISTER_FP_EIP = 260
    //EFI_SMM_SAVE_STATE_REGISTER_FP_CS = 261
    //EFI_SMM_SAVE_STATE_REGISTER_DATAOFFSET = 262
    //EFI_SMM_SAVE_STATE_REGISTER_FP_DS = 263
    //EFI_SMM_SAVE_STATE_REGISTER_MM0 = 264
    //EFI_SMM_SAVE_STATE_REGISTER_MM1 = 265
    //EFI_SMM_SAVE_STATE_REGISTER_MM2 = 266
    //EFI_SMM_SAVE_STATE_REGISTER_MM3 = 267
    //EFI_SMM_SAVE_STATE_REGISTER_MM4 = 268
    //EFI_SMM_SAVE_STATE_REGISTER_MM5 = 269
    //EFI_SMM_SAVE_STATE_REGISTER_MM6 = 270
    //EFI_SMM_SAVE_STATE_REGISTER_MM7 = 271
    //EFI_SMM_SAVE_STATE_REGISTER_XMM0 = 272
    //EFI_SMM_SAVE_STATE_REGISTER_XMM1 = 273
    //EFI_SMM_SAVE_STATE_REGISTER_XMM2 = 274
    //EFI_SMM_SAVE_STATE_REGISTER_XMM3 = 275
    //EFI_SMM_SAVE_STATE_REGISTER_XMM4 = 276
    //EFI_SMM_SAVE_STATE_REGISTER_XMM5 = 277
    //EFI_SMM_SAVE_STATE_REGISTER_XMM6 = 278
    //EFI_SMM_SAVE_STATE_REGISTER_XMM7 = 279
    //EFI_SMM_SAVE_STATE_REGISTER_XMM8 = 280
    //EFI_SMM_SAVE_STATE_REGISTER_XMM9 = 281
    //EFI_SMM_SAVE_STATE_REGISTER_XMM10 = 282
    //EFI_SMM_SAVE_STATE_REGISTER_XMM11 = 283
    //EFI_SMM_SAVE_STATE_REGISTER_XMM12 = 284
    //EFI_SMM_SAVE_STATE_REGISTER_XMM13 = 285
    //EFI_SMM_SAVE_STATE_REGISTER_XMM14 = 286
    //EFI_SMM_SAVE_STATE_REGISTER_XMM15 = 287
    //
    // Pseudo-Registers
    //
    //EFI_SMM_SAVE_STATE_REGISTER_IO = 512
    //EFI_SMM_SAVE_STATE_REGISTER_LMA = 513
};




EFI_STATUS
EFIAPI
SmmReadSaveState (
  IN CONST EFI_SMM_CPU_PROTOCOL         *This,
  IN UINTN                              Width,
  IN EFI_SMM_SAVE_STATE_REGISTER        Register,
  IN UINTN                              CpuIndex,
  OUT VOID                              *Buffer
  )
{

    UINT8   *SmmBase;
    UINT32  Index = 0xffffffff;

    if(CpuIndex >= gCpuSmmInt.NumCpus)	return	EFI_INVALID_PARAMETER;
    if(Buffer == NULL)	return	EFI_INVALID_PARAMETER;

    SmmBase = (UINT8*)pCoreDataI[CpuIndex].SmmBase;



    if (Register >= 47 && Register <= 50) return EFI_INVALID_PARAMETER;
    if (Register >= 20 && Register <= 54) Index = Register - 20;

    if (Index != 0xffffffff) {
        if (Width > gAMDX64Format[Index].SaveStateWidth) return EFI_INVALID_PARAMETER;
        switch(Width) {
        case 1:
            *(UINT8*)Buffer = *(SmmBase + gAMDX64Format[Index].SaveStateOffset);
            return EFI_SUCCESS;
        case 2:
            *(UINT16*)Buffer = *(UINT16*)(SmmBase + gAMDX64Format[Index].SaveStateOffset);
            return EFI_SUCCESS;
        case 4:
            *(UINT32*)Buffer = *(UINT32*)(SmmBase + gAMDX64Format[Index].SaveStateOffset);
            return EFI_SUCCESS;
        case 8:
            *(UINT64*)Buffer = *(UINT64*)(SmmBase + gAMDX64Format[Index].SaveStateOffset);
            return EFI_SUCCESS;
        default:
            return	EFI_INVALID_PARAMETER;
        }
    }


    if (Register == EFI_SMM_SAVE_STATE_REGISTER_IO) {

        UINT32 IoMisc = *(UINT32*)(SmmBase + 0xfec0);

        if (!(IoMisc & BIT1)) return EFI_NOT_FOUND;    //if not valid return

        if(IoMisc & BIT4){
            ((EFI_SMM_SAVE_STATE_IO_INFO*)Buffer)->IoWidth = EFI_SMM_SAVE_STATE_IO_WIDTH_UINT8;
            ((EFI_SMM_SAVE_STATE_IO_INFO*)Buffer)->IoData = (*(UINT64*)(SmmBase + 0xfff8)) & 0xff;
        }else if (IoMisc & BIT5){
            ((EFI_SMM_SAVE_STATE_IO_INFO*)Buffer)->IoWidth = EFI_SMM_SAVE_STATE_IO_WIDTH_UINT16;
            ((EFI_SMM_SAVE_STATE_IO_INFO*)Buffer)->IoData = (*(UINT64*)(SmmBase + 0xfff8)) & 0xffff;
        }else{
            ((EFI_SMM_SAVE_STATE_IO_INFO*)Buffer)->IoWidth = EFI_SMM_SAVE_STATE_IO_WIDTH_UINT32;
            ((EFI_SMM_SAVE_STATE_IO_INFO*)Buffer)->IoData = (*(UINT64*)(SmmBase + 0xfff8)) & 0xffffffff;
        }
        ((EFI_SMM_SAVE_STATE_IO_INFO*)Buffer)->IoPort = (UINT16)(IoMisc >> 16);

        if(IoMisc & BIT0)
            ((EFI_SMM_SAVE_STATE_IO_INFO*)Buffer)->IoType = EFI_SMM_SAVE_STATE_IO_TYPE_INPUT;//In
        else
            ((EFI_SMM_SAVE_STATE_IO_INFO*)Buffer)->IoType = EFI_SMM_SAVE_STATE_IO_TYPE_OUTPUT;//Out

        if(IoMisc & BIT2){
            ((EFI_SMM_SAVE_STATE_IO_INFO*)Buffer)->IoType = EFI_SMM_SAVE_STATE_IO_TYPE_STRING;//String
            //IO Restart RSI has the source value.
            ((EFI_SMM_SAVE_STATE_IO_INFO*)Buffer)->IoData = *(UINT64*)(SmmBase + 0xfeb0); //Get RSI Value
        }
        if(IoMisc & BIT3)
            ((EFI_SMM_SAVE_STATE_IO_INFO*)Buffer)->IoType = EFI_SMM_SAVE_STATE_IO_TYPE_REP_PREFIX;//Repeat



        return EFI_SUCCESS;
    }
    return EFI_UNSUPPORTED; //Register not supported.
}


EFI_STATUS
EFIAPI
SmmWriteSaveState (
  IN CONST EFI_SMM_CPU_PROTOCOL         *This,
  IN UINTN                              Width, 
  IN EFI_SMM_SAVE_STATE_REGISTER        Register,
  IN UINTN                              CpuIndex,
  IN CONST VOID                         *Buffer
  )
{
    UINT32  Index = 0xffffffff;
    UINT8   *SmmBase;

    if(CpuIndex >= gCpuSmmInt.NumCpus)	return	EFI_INVALID_PARAMETER;
    if(Buffer == NULL)	return	EFI_INVALID_PARAMETER;
    
    if (Register >= 47 && Register <= 50) return EFI_INVALID_PARAMETER;
    if (Register >= 20 && Register <= 54) Index = Register - 20;
    
    if(gAMDX64Format[Index].ReadOnly) return EFI_UNSUPPORTED;
    
    SmmBase = (UINT8*)pCoreDataI[CpuIndex].SmmBase;

    if (Index != 0xffffffff) {
        if (Width > gAMDX64Format[Index].SaveStateWidth) return EFI_INVALID_PARAMETER;
        switch(Width) {
        case 1:
            *(SmmBase + gAMDX64Format[Index].SaveStateOffset) = *(UINT8*)Buffer;
            return EFI_SUCCESS;
        case 2:
            *(UINT16*)(SmmBase + gAMDX64Format[Index].SaveStateOffset) = *(UINT16*)Buffer;
            return EFI_SUCCESS;
        case 4:
            *(UINT32*)(SmmBase + gAMDX64Format[Index].SaveStateOffset) = *(UINT32*)Buffer;
            return EFI_SUCCESS;
        case 8:
            *(UINT64*)(SmmBase + gAMDX64Format[Index].SaveStateOffset) = *(UINT64*)Buffer;
            return EFI_SUCCESS;
        default:
            return	EFI_INVALID_PARAMETER;
        }
    }
    
    return  EFI_UNSUPPORTED; // Register not supported
}

