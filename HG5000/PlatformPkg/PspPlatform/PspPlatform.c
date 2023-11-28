
#include <Protocol/PspPlatformProtocol.h>
#include <token.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>


EFI_HANDLE              mPspPlatformHandle = NULL;

#define RESUME_BSP_STACK_SIZE _PCD_VALUE_PcdTemporaryRamSize
#define RESUME_AP_STACK_SIZE  (1024 * 16)


PSP_PLATFORM_PROTOCOL mPspPlatformProtocol = {
  FixedPcdGetBool(PcdAcpiS3Enable),                    //Support CPU Context Resume
  PSP_NOTIFY_SMM_SW_SMI,  //SW SMI Command for Build Context
  RESUME_BSP_STACK_SIZE,  //BSP Stack Size
  RESUME_AP_STACK_SIZE,   //AP Stack Size
  NULL                    //Address of PEI Info Structure
};



/**
    PSP Platform driver entry point

               
    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS return EFI status

**/
EFI_STATUS
PspPlatformEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINT64               RsmInfoLocation = 0;
  EFI_STATUS           Status;
  UINT32 volatile      *pRom;
  UINT64               SecLabel;

/*
org     (0FFFFFFF0h - THIS_BINARY_BASE) 
nop             +0
nop             +1
DB      0e9h    +2
DW      -3      +3

---XXXX000000
*/
  SecLabel = *(UINT64*)(UINTN)0x0FFFFFFF0;
  SecLabel = (SecLabel >> 24) & 0x0000FFF0;
  SecLabel = SecLabel | 0xFFFF0000;

  pRom = (UINT32*)(UINTN)SecLabel;

  while(pRom < (UINT32*)0xFFFFFF00){
      if(*pRom == 0x54535251){
          RsmInfoLocation = (UINT32)(UINTN)(pRom + 1);
          break;
      }
      pRom++;
  }

  DEBUG((EFI_D_INFO, "PSPplatform RsmInfoLocation = %x\n", RsmInfoLocation));
  ASSERT(RsmInfoLocation);

  mPspPlatformProtocol.RsmHandOffInfo = (RSM_HANDOFF_INFO*)(UINTN)RsmInfoLocation;

  Status = gBS->InstallProtocolInterface (
                  &mPspPlatformHandle,
                  &gPspPlatformProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPspPlatformProtocol
                  );
  return Status;
}



