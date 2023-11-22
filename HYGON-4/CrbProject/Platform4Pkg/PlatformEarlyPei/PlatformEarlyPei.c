

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
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/TimerLib.h>
#include <Library/ByoHygonCommLib.h>
#include <Library/PlatformCommLib.h>
#include <Ppi/SecPerformance.h>


EFI_STATUS
EFIAPI
PlatIpmiPeiCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
EFIAPI
PlatPeiTpmDeviceSelectedCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );


STATIC
UINT64
CalculateTscFrequency (
  VOID
  )
{
  UINT64      StartTSC;
  UINT64      EndTSC;
  UINT32      TimerAddr;
  UINT32      Ticks;
  UINT64      TscFrequency;


  TimerAddr = PcdGet16(AcpiIoPortBaseAddress) + 8;  ///PMIO_Rx08[31:0] ACPI Timer
  Ticks     = IoRead32 (TimerAddr) + (3579);        // Set Ticks to 1ms in the future
  StartTSC  = AsmReadTsc();                         // Get base value for the TSC
  //
  // Wait until the ACPI timer has counted 1ms.
  // Timer wrap-arounds are handled correctly by this function.
  // When the current ACPI timer value is greater than 'Ticks', the while loop will exit.
  //
  while (((Ticks - IoRead32 (TimerAddr)) & BIT23) == 0) {
    CpuPause();
  }
  EndTSC = AsmReadTsc();    // TSC value 1ms later

  TscFrequency = MultU64x32 (
                   (EndTSC - StartTSC),    // Number of TSC counts in 1ms
                   1000                    // Number of ms in a second
                   );

  return TscFrequency;
}


STATIC
UINT64
GetTscFrequency (
  VOID
  )
{
  EFI_HOB_GUID_TYPE       *GuidHob;
  VOID                    *DataInHob;
  UINT64                  TscFrequency;


  GuidHob = GetFirstGuidHob(&gEfiTscFrequencyGuid);
  if (GuidHob != NULL) {
    DataInHob = GET_GUID_HOB_DATA(GuidHob);
    TscFrequency = *(UINT64*)DataInHob;
    return TscFrequency;
  }

  TscFrequency = CalculateTscFrequency();
  DEBUG((EFI_D_INFO, "TscFrequency:%ld\n", TscFrequency));

  BuildGuidDataHob (
    &gEfiTscFrequencyGuid,
    &TscFrequency,
    sizeof(UINT64)
    );

  return TscFrequency;
}


STATIC
UINT64
EFIAPI
TscGetTimeInNanoSecond (
  IN      UINT64                     Ticks
  )
{
  UINT64  Frequency;
  UINT64  NanoSeconds;
  UINT64  Remainder;
  INTN    Shift;

  Frequency = GetTscFrequency();

  //
  //          Ticks
  // Time = --------- x 1,000,000,000
  //        Frequency
  //
  NanoSeconds = MultU64x32 (DivU64x64Remainder (Ticks, Frequency, &Remainder), 1000000000u);

  //
  // Ensure (Remainder * 1,000,000,000) will not overflow 64-bit.
  // Since 2^29 < 1,000,000,000 = 0x3B9ACA00 < 2^30, Remainder should < 2^(64-30) = 2^34,
  // i.e. highest bit set in Remainder should <= 33.
  //
  Shift = MAX (0, HighBitSet64 (Remainder) - 33);
  Remainder = RShiftU64 (Remainder, (UINTN) Shift);
  Frequency = RShiftU64 (Frequency, (UINTN) Shift);
  NanoSeconds += DivU64x64Remainder (MultU64x32 (Remainder, 1000000000u), Frequency, NULL);

  return NanoSeconds;
}


STATIC
EFI_STATUS
EFIAPI
SecGetPerformance (
  IN CONST EFI_PEI_SERVICES          **PeiServices,
  IN       PEI_SEC_PERFORMANCE_PPI   *This,
  OUT      FIRMWARE_SEC_PERFORMANCE  *Performance
  )
{
  EFI_PEI_HOB_POINTERS   GuidHob;
  CAR_TOP_DATA           *CarTopData;

  GuidHob.Raw = GetFirstGuidHob(&gCarTopDataHobGuid);
  ASSERT(GuidHob.Raw != NULL);
  CarTopData  = (CAR_TOP_DATA*)(GuidHob.Guid+1);
  Performance->ResetEnd = TscGetTimeInNanoSecond(CarTopData->ResetTsc);

  return EFI_SUCCESS;
}


PEI_SEC_PERFORMANCE_PPI  gSecPerformancePpi = {
  SecGetPerformance
};

STATIC EFI_PEI_PPI_DESCRIPTOR  gPlatEarlyPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPeiSecPerformancePpiGuid,
    &gSecPerformancePpi
  }
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  gPlatEarlyPpiNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiTpmDeviceSelectedGuid,
    PlatPeiTpmDeviceSelectedCallBack
  },  
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gPeiIpmiTransportPpiGuid,
    PlatIpmiPeiCallBack
  },
};





EFI_STATUS
EFIAPI
EarlyPeiEntry (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS             Status = EFI_SUCCESS;
  CAR_TOP_DATA           *CarTopData;
  UINT8                  *TopofCar;
  UINT8                  PhyAddrBits;
  UINT16                 PmBase;
  PLATFORM_COMM_INFO     *Info;


  DEBUG((EFI_D_INFO, "EarlyPeiEntry\n"));

  CarTopData = BuildGuidHob(&gCarTopDataHobGuid, sizeof(CAR_TOP_DATA));
  ASSERT(CarTopData != NULL);
  TopofCar = (UINT8*)(UINTN)(PcdGet32(PcdTemporaryRamBase) + PcdGet32(PcdTemporaryRamSize));
  CopyMem(CarTopData, TopofCar - sizeof(CAR_TOP_DATA), sizeof(CAR_TOP_DATA));
  DEBUG((EFI_D_INFO, "CarTopData:%X %lX %lX\n", CarTopData->Bist, CarTopData->ResetTsc, CarTopData->JmpSecCoreTsc));
  DEBUG((EFI_D_INFO, "TSC:%lX\n", AsmReadTsc()));

  Info = (PLATFORM_COMM_INFO*)BuildGuidHob(&gEfiPlatformCommInfoGuid, sizeof(PLATFORM_COMM_INFO));
  ASSERT(Info != NULL);
  ZeroMem(Info, sizeof(PLATFORM_COMM_INFO));
  Info->Signature = PLAT_COMM_INFO_SIGN;
  Info->ThisSize  = sizeof(PLATFORM_COMM_INFO);
  Info->Csm       = 0xFF;
  Info->IgdSocket = 0xFF;
  Info->CpuModel  = LibGetCpuModel();
  Info->CpuPhySockets = PcdGet8(PcdHygonNumberOfPhysicalSocket);

  PmBase = PcdGet16(AcpiIoPortBaseAddress);
  LibHygonEarlyDecode(FixedPcdGet16(IpmiIoBase), PmBase);

  PhyAddrBits = LibGetCpuPhyAddrBits();
  BuildCpuHob(PhyAddrBits, 16);
  DEBUG((EFI_D_INFO, "PhyAddrBits:%d, CpuPhySockets:%d\n", PhyAddrBits, Info->CpuPhySockets));

  Status = PeiServicesNotifyPpi(gPlatEarlyPpiNotifyList);
  ASSERT_EFI_ERROR(Status);

// For Port 0x61, program timer 1 as refresh timer
  IoWrite8(0x43, 0x54);
  IoWrite8(0x41, 0x12);

  Info->AcpiWakeupSrc = LibGetAcpiWakeUpSrc(PmBase);

  Status = PeiServicesInstallPpi(gPlatEarlyPpiList);
  ASSERT_EFI_ERROR(Status);

  return Status;
}


