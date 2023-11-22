

#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/Spi.h>




typedef struct {
  UINTN             Signature;
  EFI_HANDLE        Handle;
  EFI_SPI_PROTOCOL  SpiProtocol;
  SPI_INIT_TABLE    SpiInitTable;
  UINT32            FlashId;
  UINT32            SizeFix;
} SPI_INSTANCE;

extern SPI_INSTANCE  gSpiInstance;

VOID
SpiVirtualddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

