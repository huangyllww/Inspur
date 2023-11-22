
#include <Uefi.h>
#include <CpuBoard.h>


extern   UINT32 CpuReadPCI32(IN UINT32);



/**
    Read the PCI Register

    @param Address (UINT32) ( (((UINT32)bus) << 24) + (((UINT32)dev) << 19) + (((UINT32)func) << 16) + ((UINT32)reg) )

    @retval UINT32 return value from the PCI Register.

**/
UINT32 CpuReadPCI32(
    IN UINT32 Address
)
{
    UINT32 Value32;
    UINT32 AddrCf8 = (1 << 31) + (Address & 0x0FFFF00) + (Address & 0xFC);

    IoWrite32(0xCF8, AddrCf8);
    Value32 = IoRead32(0xCFC);
    return Value32;
}

/**
    Board specfic CPU changes. This function will only execute
    on the BSP.

    @param VOID

    @retval EFI_STATUS return the EFI status

**/

EFI_STATUS BoardCpuInit(VOID)
{
    EFI_STATUS  Status = EFI_SUCCESS;

    return Status;
}


