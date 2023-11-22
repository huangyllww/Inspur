

#include <PiPei.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/ByoCommLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Ppi/ByoEarlyGopInfoPpi.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ByoHygonCommLib.h>


typedef struct {
  BYO_EARLY_GOP_INFO      Info;
  EFI_PEI_PPI_DESCRIPTOR  Desc;
} BYO_EARLY_GOP_INFO_PPI_CTX;


STATIC
EFI_STATUS
AllocateMmio32 (
  IN  UINT8       Bus,
  IN  UINT32      Length,
  IN  UINT32      Align,
  OUT UINTN       *pMmio
  )
{
  FABRIC_MMIO_ATTRIBUTE                 MmioAttr;
  FABRIC_TARGET                         Target;
  UINT64                                BaseAddress;
  UINT64                                AddrLen = Length;
  EFI_STATUS                            Status;

  MmioAttr.MmioType = MMIO_BELOW_4G;
  Target.TgtType    = TARGET_PCI_BUS;
  Target.PciBusNum  = Bus;
  Status = FabricAllocateMmio (
             &BaseAddress, 
             &AddrLen, 
             Align, 
             Target, 
             &MmioAttr
             );
  if(!EFI_ERROR(Status)){
    *pMmio = (UINT32)BaseAddress;
  } else {
    DEBUG((EFI_D_INFO, "AllocateMmio32:%r\n", Status));
  }
  return Status;
}



STATIC
EFI_STATUS
AllocateIo16 (
  IN  UINT8       Bus,
  IN  UINT16      Length,
  OUT UINT16      *pIo
  )
{
  FABRIC_TARGET                         Target;
  UINT32                                BaseAddress;
  UINT32                                AddrLen = Length;
  EFI_STATUS                            Status;

  Target.TgtType    = TARGET_PCI_BUS;
  Target.PciBusNum  = Bus;
  Status = FabricAllocateIo(&BaseAddress, &AddrLen, Target);
  if(!EFI_ERROR(Status)){
    *pIo = (UINT16)BaseAddress;
  } else {
    DEBUG((EFI_D_INFO, "AllocateIo16:%r\n", Status));
  }
  return Status;
}


VOID
EFIAPI
PeiStall (
  IN  UINTN  Microseconds
  )
{
  MicroSecondDelay(Microseconds);
}


#define ASPEED_VGA_P2P_BUS          1
#define ASPEED_VGA_P2P_DEV          0
#define ASPEED_VGA_P2P_FUN          0

#define ASPEED_VGA_CONTROLLER_BUS   2
#define ASPEED_VGA_CONTROLLER_DEV   0
#define ASPEED_VGA_CONTROLLER_FUN   0

#define ASPEED_VGA_PCIID            0x20001A03


EFI_STATUS
EFIAPI
PlatEarlyGopPciInitEntry (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS       Status = EFI_SUCCESS;
  UINTN            Bar0;
  UINTN            Bar1;
  UINTN            Base;
  UINT16           MmioStart;
  UINT16           MmioEnd;
  UINT32           MmioRange;
  UINT16              IoBase;
  UINT16              IoRange;
  BYO_EARLY_GOP_INFO_PPI_CTX  *EarlyGopInfoCtx;
  BYO_EARLY_GOP_INFO  *Info;
  UINT8               *LogoData;
  UINTN               LogoDataSize;
  EFI_BOOT_MODE       BootMode;
  UINT32              PciId;
  UINT8               RpBus;
  UINT8               RpDev;
  UINT8               RpFunc;
  UINT16              RpBDF;
  

  DEBUG((EFI_D_INFO, "PlatEarlyGopPciInitEntry\n"));

  BootMode = GetBootModeHob();
  if(BootMode == BOOT_IN_RECOVERY_MODE || BootMode == BOOT_ON_S3_RESUME){
    return EFI_UNSUPPORTED;
  }

  RpBDF = PcdGet16(PcdIgdRpBDF);
  if(RpBDF == 0xFFFF){
    return EFI_UNSUPPORTED;
  } 
  
  RpBus = (UINT8)(RpBDF >> 8);
  RpDev = (UINT8)(RpBDF >> 3) & 0x1F;
  RpFunc = (UINT8)(RpBDF & 0x7);
  DEBUG((EFI_D_INFO, "IGD_RP(%x,%x,%x)\n", RpBus, RpDev, RpFunc));    
  if(RpBus != 0){
    DEBUG((EFI_D_INFO, "Now only support bmc GFX on die 0\n"));    
    return EFI_UNSUPPORTED;
  } 
  
  LogoData = (UINT8*)(UINTN)PcdGet32(PcdLogoDataAddress);
  LogoDataSize = PcdGet32(PcdLogoDataSize);
  DEBUG((EFI_D_INFO, "Logo(%x,%x)\n", LogoData, LogoDataSize));
  if(LogoData == NULL || LogoDataSize == 0){
    return EFI_NOT_FOUND;
  }

  Status = AllocateMmio32(RpBus, 0x1100000, 0xFFFFFF, &Bar0);
  if(EFI_ERROR(Status)){
    return Status;
  }  
  Status = AllocateIo16(0, 0x1000, &IoBase);
  if(EFI_ERROR(Status)){
    return Status;
  }
    
  Bar1 = Bar0 + 0x1000000;
  MmioStart = (UINT16)(Bar0 >> 20) << 4;
  MmioEnd = (UINT16)((Bar0 + 0x1100000 - 1) >> 20) << 4;
  MmioRange = MmioStart | (MmioEnd << 16);

  IoRange = ((IoBase >> 12) << 4) | ((IoBase >> 12) << 12);
  
  DEBUG((EFI_D_INFO, "(L%d) %X %X %X %X %X %X\n", __LINE__, Bar0, Bar1, MmioStart, MmioEnd, MmioRange, IoRange));
  DEBUG((EFI_D_INFO, "(%d,%d,%d)\n", RpBus, RpDev, RpFunc));

  Base = PCI_DEV_MMBASE(RpBus, RpDev, RpFunc);
  PciId = MmioRead32(Base);
  if((UINT16)PciId == 0xFFFF){
    return EFI_NOT_FOUND;
  }
  DEBUG((EFI_D_INFO, "(L%d) Base:%X\n", __LINE__, Base));
  DEBUG((EFI_D_INFO, "(L%d) %X\n", __LINE__, PciId));   
  MmioWrite32(Base+0x18, ASPEED_VGA_CONTROLLER_BUS << 16 | ASPEED_VGA_P2P_BUS << 8 );
  MmioWrite16(Base+0x1C, IoRange);
  MmioWrite32(Base+0x20, MmioRange);

  Base = PCI_DEV_MMBASE(ASPEED_VGA_P2P_BUS, ASPEED_VGA_P2P_DEV, ASPEED_VGA_P2P_FUN);
  PciId = MmioRead32(Base);
  if((UINT16)PciId == 0xFFFF){
    return EFI_NOT_FOUND;
  }  
  DEBUG((EFI_D_INFO, "(L%d) Base:%X\n", __LINE__, Base));
  DEBUG((EFI_D_INFO, "(L%d) %X\n", __LINE__, PciId));    
  MmioWrite32(Base+0x18, ASPEED_VGA_CONTROLLER_BUS << 16 | ASPEED_VGA_CONTROLLER_BUS << 8 | ASPEED_VGA_P2P_BUS);
  MmioWrite16(Base+0x1C, IoRange);
  MmioWrite32(Base+0x20, MmioRange);

  Base = PCI_DEV_MMBASE(ASPEED_VGA_CONTROLLER_BUS, ASPEED_VGA_CONTROLLER_DEV, ASPEED_VGA_CONTROLLER_FUN);
  PciId = MmioRead32(Base);
  if(PciId != ASPEED_VGA_PCIID){
    DEBUG((EFI_D_INFO, "Not 2500VGA:%X\n", PciId));
    return EFI_NOT_FOUND;
  }    
  DEBUG((EFI_D_INFO, "(L%d) Base:%X\n", __LINE__, Base));
  DEBUG((EFI_D_INFO, "(L%d) %X\n", __LINE__, PciId));    
  MmioWrite32(Base+0x10, Bar0);
  MmioWrite32(Base+0x14, Bar1);
  MmioWrite32(Base+0x18, IoBase);

  MmioWrite8(PCI_DEV_MMBASE(ASPEED_VGA_CONTROLLER_BUS, ASPEED_VGA_CONTROLLER_DEV, ASPEED_VGA_CONTROLLER_FUN)+4, 3);
  MmioWrite8(PCI_DEV_MMBASE(ASPEED_VGA_P2P_BUS, ASPEED_VGA_P2P_DEV, ASPEED_VGA_P2P_FUN)+4, 7);
  MmioWrite8(PCI_DEV_MMBASE(RpBus, RpDev, RpFunc)+4, 7);

  //DumpPci(0x10, 1, 4);
  //DumpPci(0x11, 0, 0);
  //DumpPci(0x12, 0, 0);
  //
  //DumpIo(IoBase, 256);
  //DumpMem32((VOID*)Bar0, 256);
  //DumpMem32((VOID*)Bar1, 256);  
  //
  //Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();
  //DumpRootBridgeResource(Info);

  EarlyGopInfoCtx = (BYO_EARLY_GOP_INFO_PPI_CTX*)AllocateZeroPool(sizeof(BYO_EARLY_GOP_INFO_PPI_CTX));
  ASSERT_EFI_ERROR(EarlyGopInfoCtx != NULL);
  Info = &EarlyGopInfoCtx->Info;
  Info->Version = EARLY_GOP_INFO_VERSION1;
  Info->Bus     = ASPEED_VGA_CONTROLLER_BUS;
  Info->Dev     = ASPEED_VGA_CONTROLLER_DEV;  
  Info->Fun     = ASPEED_VGA_CONTROLLER_FUN;
  Info->LogoDataAddr = (UINTN)LogoData;
  Info->LogoDataSize = (UINTN)LogoDataSize;
  Info->Stall = PeiStall;
  Info->PcieConfigBase       = (UINTN)PcdGet64(PcdPciExpressBaseAddress);
  Info->PciIoTranslation     = 0;
  Info->PciMmio32Translation = 0;

  EarlyGopInfoCtx->Desc.Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  EarlyGopInfoCtx->Desc.Guid = &gByoEarlyGopInfoPpiGuid;
  EarlyGopInfoCtx->Desc.Ppi  = (VOID*)Info;
  Status = PeiServicesInstallPpi(&EarlyGopInfoCtx->Desc);
  ASSERT_EFI_ERROR(Status);

  return Status;  
}  


