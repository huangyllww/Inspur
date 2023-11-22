
#include "PlatformDxe.h"
#include <IndustryStandard/Pci.h>
#include <Protocol/SwSmiValuePolicyData.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/AtaPassThru.h>
#include <Protocol/PciIo.h>
#include <Protocol/LegacyOptionRomInfo.h>
#include <SetupVariable.h>
#include <LegacyBiosDxe/LegacyBiosInterface.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Library/SerialPortLib.h>
#include <Protocol/SerialIo.h>
#include <Library/ByoCommLib.h>
#include <Library/PciLib.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Protocol/GraphicsOutput.h>


#define ROM_MAX_ENTRIES               24
#define VGA_HANDLE_BUF_MAX_COUNT      0x20
#define IDE_HANDLE_BUF_MAX_COUNT      0x20
#define ISA_HANDLE_BUF_MAX_COUNT      0x20

#define EFI_AHCI_BAR_INDEX                                  5
#define VESA_BIOS_EXTENSIONS_RETURN_CONTROLLER_INFORMATION  0x4f00
#define VESA_BIOS_EXTENSIONS_VERSION_2_0                    0x0200
#define VESA_BIOS_EXTENSIONS_STATUS_SUCCESS                 0x004f
#define VESA_BIOS_EXTENSIONS_VESA_SIGNATURE                 SIGNATURE_32 ('V', 'E', 'S', 'A')
#define VESA_BIOS_EXTENSIONS_VBE2_SIGNATURE                 SIGNATURE_32 ('V', 'B', 'E', '2')

#pragma pack(1)
typedef struct {
  UINT32  VESASignature;      // 'VESA' 4 byte signature
  UINT16  VESAVersion;        // VBE version number
  UINT32  OEMStringPtr;       // Pointer to OEM string
  UINT32  Capabilities;       // Capabilities of video card
  UINT32  VideoModePtr;       // Pointer to an array of 16-bit supported modes values terminated by 0xFFFF
  UINT16  TotalMemory;        // Number of 64kb memory blocks
  UINT16  OemSoftwareRev;     // VBE implementation Software revision
  UINT32  OemVendorNamePtr;   // VbeFarPtr to Vendor Name String
  UINT32  OemProductNamePtr;  // VbeFarPtr to Product Name String
  UINT32  OemProductRevPtr;   // VbeFarPtr to Product Revision String
  UINT8   Reserved[222];      // Reserved for VBE implementation scratch area
  UINT8   OemData[256];       // Data area for OEM strings.  Pad to 512 byte block size
} VESA_BIOS_EXTENSIONS_INFORMATION_BLOCK;
#pragma pack()

typedef struct {
  UINT8   *Address;
  UINT8   *Data;
  UINT8   *RtData;
  UINT8   *RtDataAddr;
  UINT16  RtDataSize;
} SAVED_PCI_ROM;

typedef struct {
  EFI_PCI_IO_PROTOCOL   *PciIo;
  UINTN                 PortCount;
  UINT64                NativeFis[32];
  UINT64                NativeCmdList[32];
  UINT64                LegacyFis[32];
  UINT64                LegacyCmdList[32];
  UINT32                PciId;
} AHCI_FIS_CMDLIST_SAVE;

#define AHCI_ADDR_SAVE_MAX_HOST_COUNT          32

EFI_HANDLE  gVgaHandles[VGA_HANDLE_BUF_MAX_COUNT] = {NULL};
EFI_HANDLE  gIdeHandles[IDE_HANDLE_BUF_MAX_COUNT];
EFI_HANDLE  gIsaHandles[ISA_HANDLE_BUF_MAX_COUNT];

AHCI_FIS_CMDLIST_SAVE               *gAhciAddrSaveArray = NULL;
UINTN                               gAhciAddrSaveCurCount = 0;
UINTN                               mSavedCount = 0;
SAVED_PCI_ROM                       mSaveRomEntry[ROM_MAX_ENTRIES];
UINT16                              mBbsRomSegment;
LEGACY_PNP_EXPANSION_HEADER         *mBasePnpPtr;
UINT16                              mEbdaSegOrg;
UINT8                               mEbdaSizeOrg;
BOOLEAN                             mInt13VectorSaved = FALSE;
UINT32                              mInt13Vector = 0;
UINT16                              mVideoMode    = 0xFFFF;
UINT16                              mVideoModeSaved = FALSE;
BOOLEAN                             mNativeAhciDriver = FALSE;


UINT16 gTotalEntryNum = 0;
UINT16 gPirqTableSize = 0;


///MTN-20161010-S
VOID CollectPCIBusInfo (VOID)
{
  EFI_STATUS Status;
  EFI_HANDLE *pHandleBuffer;
  UINTN      NumberOfHandles;
  UINTN      i;
  EFI_PCI_IO_PROTOCOL *pPciIoProtocol;
  UINT8   PciData[4];
  UINTN  Seg, Bus, Dev, Fun;

  DEBUG((EFI_D_INFO,"[MTN-DBG]:CollectPCIBusInfo In \n"));

  //
  // detect all other buses via PCI bridges
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &pHandleBuffer
                  );
  if (EFI_ERROR (Status))
    return;

  for (i = 0; i < NumberOfHandles; i++) {
    Status = gBS->HandleProtocol (
                    pHandleBuffer[i],
                    &gEfiPciIoProtocolGuid,
                    (VOID**) &pPciIoProtocol
                    );
    if (EFI_ERROR (Status))
      continue;

    //
    // Read class code information at 0x8 offset in PCI header
    Status = pPciIoProtocol->Pci.Read (
                                   pPciIoProtocol,
                                   EfiPciIoWidthUint32,
                                   0x8,
                                   1,
                                   (VOID*) PciData
                                   );
    if (EFI_ERROR (Status))
      continue;

    //
    // not bridge device
    //
    if (PciData[3] != 0x6)
      continue;

    //
    // PCI host bridge we already know, so skip it or unknown bridge - skip it also
    //
    if ((PciData[2] == 0) || (PciData[2] > 6))
      continue;

    //
    // PCI to PCI bridge
    //
    if (PciData[2] == 4) {
      //
      // Read primary and secondary bus numbers
      //
      Status = pPciIoProtocol->Pci.Read (
                                     pPciIoProtocol,
                                     EfiPciIoWidthUint32,
                                     0x18,
                                     1,
                                     (VOID*) PciData
                                     );
      //

  //YKN_20160309 +S
  pPciIoProtocol->GetLocation(pPciIoProtocol, &Seg, &Bus, &Dev, &Fun);
  DEBUG((EFI_D_INFO,"[MTN-DBG]:Seg=0x%x Bus=0x%x Dev=0x%x Fun=0x%x \n",Seg,Bus,Dev,Fun));


  ////ASSERT(Index < PCI_BRIDGES_NUM);
    //YKN_20160309 +E
    }
}
}
///MTN-20161010-E



EFI_STATUS
EFIAPI
GetPlatformInfo (
  IN EFI_LEGACY_BIOS_PLATFORM_PROTOCOL            *This,
  IN EFI_GET_PLATFORM_INFO_MODE                   Mode,
  OUT VOID                                        **Table,
  OUT UINTN                                       *TableSize,
  OUT UINTN                                       *Location,
  OUT UINTN                                       *Alignment,
  IN  UINT16                                      LegacySegment,
  IN  UINT16                                      LegacyOffset
)
{
  EFI_STATUS    Status;
  DEBUG((EFI_D_INFO, "%a() M:%d\n", __FUNCTION__, Mode));

  switch (Mode) {
    case EfiGetPlatformBinarySystemRom:
      Status = GetSectionFromAnyFv (
                 PcdGetPtr(PcdCsm16File),
                 EFI_SECTION_RAW,
                 0,
                 Table,
                 TableSize
               );
      break;

    case EfiGetPlatformPciExpressBase:
      *Location = (UINTN)PcdGet64(PcdPciExpressBaseAddress);
      Status = EFI_SUCCESS;
      break;

    default:
      *Table     = NULL;
      *TableSize = 0;
      *Location  = 0;
      *Alignment = 0;
      Status = EFI_UNSUPPORTED;
      break;
  }

  DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
  return Status;
}


EFI_STATUS
FillHddInfo (
  IN  EFI_PCI_IO_PROTOCOL  *PciIo,
  OUT HDD_INFO             *HddInfo,
  OUT UINTN                *ChannelCount
  )
{
  EFI_STATUS  Status;
  PCI_TYPE00  PciConfigHeader;
  UINTN       Segment;
  UINTN       Bus;
  UINTN       Device;
  UINTN       Function;
  UINT16      BusMasterAddress;


  *ChannelCount = 0;

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        0,
                        sizeof(PciConfigHeader) / sizeof(UINT32),
                        &PciConfigHeader
                        );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  Status = PciIo->GetLocation (
                    PciIo,
                    &Segment,
                    &Bus,
                    &Device,
                    &Function
                    );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  HddInfo->Status  |= HDD_PRIMARY;
  HddInfo->Bus      = (UINT32)Bus;
  HddInfo->Device   = (UINT32)Device;
  HddInfo->Function = (UINT32)Function;

  if (PciConfigHeader.Hdr.ClassCode[0] & BIT0) {
    HddInfo->CommandBaseAddress = (UINT16)(PciConfigHeader.Device.Bar[0] & 0xfffc);
    HddInfo->ControlBaseAddress = (UINT16)((PciConfigHeader.Device.Bar[1] & 0xfffc)+2);
    HddInfo->HddIrq             = PciConfigHeader.Device.InterruptLine;
  } else {
    HddInfo->CommandBaseAddress = 0x1f0;
    HddInfo->ControlBaseAddress = 0x3f6;
    HddInfo->HddIrq             = 14;
  }
  HddInfo->BusMasterAddress = (UINT16)(PciConfigHeader.Device.Bar[4] & 0xfffc);
  BusMasterAddress = HddInfo->BusMasterAddress;
  (*ChannelCount)++;

  HddInfo++;
  HddInfo->Status  |= HDD_SECONDARY;
  HddInfo->Bus      = (UINT32)Bus;
  HddInfo->Device   = (UINT32)Device;
  HddInfo->Function = (UINT32)Function;
  HddInfo->BusMasterAddress = BusMasterAddress + 8;
  if (PciConfigHeader.Hdr.ClassCode[0] & BIT2) {
    HddInfo->CommandBaseAddress = (UINT16)(PciConfigHeader.Device.Bar[2] & 0xfffc);
    HddInfo->ControlBaseAddress = (UINT16)((PciConfigHeader.Device.Bar[3] & 0xfffc)+2);
    HddInfo->HddIrq             = PciConfigHeader.Device.InterruptLine;
  } else {
    HddInfo->CommandBaseAddress = 0x170;
    HddInfo->ControlBaseAddress = 0x376;
    HddInfo->HddIrq             = 15;
  }
  (*ChannelCount)++;

ProcExit:
  return Status;
}

EFI_STATUS
EFIAPI
GetPlatformHandle (
  IN EFI_LEGACY_BIOS_PLATFORM_PROTOCOL  *This,
  IN EFI_GET_PLATFORM_HANDLE_MODE       Mode,
  IN UINT16                             Type,
  OUT EFI_HANDLE                        **HandleBufferIn,
  OUT UINTN                             *HandleCountIn,
  OUT VOID                              **AdditionalData OPTIONAL
)
{
  EFI_STATUS                    Status;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer;
//EFI_HANDLE                    TmpHandle;
  UINTN                         Index;
  UINTN                         BufIdx;
  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  HDD_INFO                      *HddInfo;
  UINTN                         ChannelIndex;
  UINTN                         ChannelCount;
  UINT8                         ClassCode[3];
  BOOLEAN                       VgaExchg;


//DEBUG((EFI_D_INFO, "%a(%d) ", __FUNCTION__, Mode));
  if(Mode == EfiGetPlatformVgaHandle){
    gVgaHandles[0] = (EFI_HANDLE)(UINTN)PcdGet64(PcdFirstVideoHostHandle);
    if(gVgaHandles[0] == NULL){
      return EFI_NOT_FOUND;
    } else {
      *HandleBufferIn = gVgaHandles;
      *HandleCountIn = 1;
      return EFI_SUCCESS;
    }
  }

  HandleBuffer = NULL;
  BufIdx       = 0;
  ChannelIndex = 0;
  VgaExchg     = FALSE;

  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  &LegacyBios
                  );
  if (EFI_ERROR (Status)) {
    goto ProcExit;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    &PciIo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CLASSCODE_OFFSET, 3, &ClassCode[0]);
    if (EFI_ERROR (Status)) {
      continue;
    }

    switch (Mode) {
      case EfiGetPlatformVgaHandle:
        if(ClassCode[2] == PCI_CLASS_DISPLAY && ClassCode[1] == PCI_CLASS_DISPLAY_VGA){
          if(BufIdx < VGA_HANDLE_BUF_MAX_COUNT){
            gVgaHandles[BufIdx++] = HandleBuffer[Index];
            *HandleBufferIn = gVgaHandles;
          }
/*
          if(gSetupData->VideoDualVga && gSetupData->VideoPrimaryAdapter == 0){
            if(BufIdx > 1 && !VgaExchg){ //more than one VGA adapters
              TmpHandle = gVgaHandles[1];
              gVgaHandles[1] = gVgaHandles[0];
              gVgaHandles[0] = TmpHandle;
              VgaExchg = TRUE;
            }
          }
*/
        }
        break;

      case EfiGetPlatformIdeHandle:
        if(ClassCode[2] == PCI_CLASS_MASS_STORAGE && ClassCode[1] == PCI_CLASS_MASS_STORAGE_IDE){
          if(BufIdx < IDE_HANDLE_BUF_MAX_COUNT){
            gIdeHandles[BufIdx++] = HandleBuffer[Index];
            *HandleBufferIn = gIdeHandles;
            if (AdditionalData != NULL) {
              HddInfo = (HDD_INFO*)*AdditionalData;
              Status  = FillHddInfo(PciIo, &HddInfo[ChannelIndex], &ChannelCount);
              ChannelIndex += ChannelCount;
            }
          }
        }
        break;

      case EfiGetPlatformIsaBusHandle:
        if(ClassCode[2] == PCI_CLASS_BRIDGE && ClassCode[1] == PCI_CLASS_BRIDGE_ISA){
          if(BufIdx < ISA_HANDLE_BUF_MAX_COUNT){
            gIsaHandles[BufIdx++] = HandleBuffer[Index];
            *HandleBufferIn = gIsaHandles;
          }
        }
        break;

      default:
        Status = EFI_UNSUPPORTED;
        goto ProcExit;
    }
  }

  *HandleCountIn = BufIdx;
  if(BufIdx==0){
    Status = EFI_NOT_FOUND;
  }

ProcExit:
  if(HandleBuffer != NULL){
    gBS->FreePool(HandleBuffer);
  }
//DEBUG((EFI_D_INFO, "%r count:%d\n", Status, BufIdx));
  return Status;
}

#define SMM_PnP_BIOS_CALL   0x47

EFI_STATUS
EFIAPI
SmmInit (
  IN EFI_LEGACY_BIOS_PLATFORM_PROTOCOL   *This,
  IN  VOID                               *EfiToLegacy16BootTable
)
{
  EFI_STATUS                      Status;
  SMM_TABLE                       *SoftIntData;
  SMM_ENTRY                       *SmmEntry;
  UINTN                           TableSize;
  EFI_SWSMI_VALUE_POLICY_PROTOCOL *SwSmiPolicy;
  UINT8                           Index;
  UINT8                           NumSmmEntries;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateProtocol(&gSwSmiValuePolicyGuid, NULL, &SwSmiPolicy);
  if (EFI_ERROR(Status)){
    NumSmmEntries = 0;
  } else {
    NumSmmEntries = SwSmiPolicy->NumSmmEntries;
  }

  TableSize = 0x100;
  Status    = gBS->AllocatePool(EfiBootServicesData, TableSize, &SoftIntData);
  ASSERT_EFI_ERROR (Status);
  ASSERT((UINT64)(UINTN)SoftIntData < SIZE_4GB);
  ZeroMem(SoftIntData, TableSize);
  ((EFI_TO_COMPATIBILITY16_BOOT_TABLE*)EfiToLegacy16BootTable)->SmmTable = (UINT32)(UINTN)SoftIntData;

  SoftIntData->NumSmmEntries = NumSmmEntries;
  SmmEntry = &SoftIntData->SmmEntry;

  for(Index = 0; Index < NumSmmEntries; Index++) {
    SmmEntry->SmmAttributes.Type = SwSmiPolicy->SwSmiEntry[Index].Type;
    SmmEntry->SmmAttributes.PortGranularity = PORT_SIZE_16;
    SmmEntry->SmmAttributes.DataGranularity = DATA_SIZE_8;
    SmmEntry->SmmPort = _PCD_VALUE_PcdSwSmiCmdPort;
    SmmEntry->SmmData = SwSmiPolicy->SwSmiEntry[Index].Value;
    SmmEntry++;
  }

  SmmEntry->SmmAttributes.Type            = 0;
  SmmEntry->SmmAttributes.PortGranularity = PORT_SIZE_16;
  SmmEntry->SmmAttributes.DataGranularity = DATA_SIZE_8;
  SmmEntry->SmmFunction.Function          = DMI_PNP_50_57;
  SmmEntry->SmmPort                       = _PCD_VALUE_PcdSwSmiCmdPort;
  SmmEntry->SmmData                       = SMM_PnP_BIOS_CALL;
  SmmEntry ++;
  SoftIntData->NumSmmEntries ++;

  return EFI_SUCCESS;
}





BOOLEAN
CheckVbeSupport (
  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios
  )
{
  EFI_IA32_REGISTER_SET                   Regs;
  EFI_STATUS                              Status;
  EFI_PHYSICAL_ADDRESS                    AddressBelow1MB;
  static BOOLEAN                          HasExecute    = FALSE;
  static BOOLEAN                          mVbeSupport   = FALSE;
  VESA_BIOS_EXTENSIONS_INFORMATION_BLOCK  *VbeInformationBlock;

  if (HasExecute) {
    return mVbeSupport;
  }

  //
  // Test to see if the Video Adapter is compliant with VBE 3.0
  //
  AddressBelow1MB = 0x00100000 - 1;
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiBootServicesData,
                  EFI_SIZE_TO_PAGES (sizeof (VESA_BIOS_EXTENSIONS_INFORMATION_BLOCK)),
                  &AddressBelow1MB
                  );
  ASSERT_EFI_ERROR (Status);

  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = VESA_BIOS_EXTENSIONS_RETURN_CONTROLLER_INFORMATION;
  VbeInformationBlock = (VESA_BIOS_EXTENSIONS_INFORMATION_BLOCK *)(UINTN)AddressBelow1MB;
  VbeInformationBlock->VESASignature  = VESA_BIOS_EXTENSIONS_VBE2_SIGNATURE;
  Regs.X.ES = EFI_SEGMENT ((UINTN) VbeInformationBlock);
  Regs.X.DI = EFI_OFFSET ((UINTN) VbeInformationBlock);

  LegacyBios->Int86 (LegacyBios, 0x10, &Regs);

  HasExecute = TRUE;
  if ((Regs.X.AX != VESA_BIOS_EXTENSIONS_STATUS_SUCCESS) ||
      (VbeInformationBlock->VESASignature != VESA_BIOS_EXTENSIONS_VESA_SIGNATURE) ||
      (VbeInformationBlock->VESAVersion < VESA_BIOS_EXTENSIONS_VERSION_2_0)) {
    mVbeSupport = FALSE;
  } else {
    mVbeSupport = TRUE;
  }

  gBS->FreePages (AddressBelow1MB, EFI_SIZE_TO_PAGES (sizeof (VESA_BIOS_EXTENSIONS_INFORMATION_BLOCK)));
  return mVbeSupport;
}

VOID ClearScreen()
{
  EFI_STATUS                    Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop;

  Status = gBS->HandleProtocol(
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID**)&Gop
                  );
  if(!EFI_ERROR(Status)){
    Status = Gop->SetMode(Gop, Gop->Mode->Mode);
    DEBUG((EFI_D_INFO, "ClearScreen.Gop.SetMode:%r\n", Status));
  }
}


VOID
EFIAPI
PrepareToScanRomHookVideo (
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios
  )
{
  EFI_IA32_REGISTER_SET         Regs;

  if(mVideoModeSaved){
    return;
  }

  if (!CheckVbeSupport (LegacyBios)) {
    DEBUG((EFI_D_INFO, "CheckVbeSupport:No\n"));
    return;
  }

  ZeroMem (&Regs, sizeof(EFI_IA32_REGISTER_SET));
  Regs.X.AX = 0x4F03;
  LegacyBios->Int86(LegacyBios, 0x10, &Regs);

  mVideoMode = Regs.X.BX;
  mVideoModeSaved = TRUE;

}

VOID
EFIAPI
AfterRomInitHookVideo (
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios
  )
{
  EFI_IA32_REGISTER_SET         Regs;

  ZeroMem((VOID*)(UINTN)0xB8000, 80*25*2);

  if(mVideoModeSaved){
    ZeroMem(&Regs, sizeof(EFI_IA32_REGISTER_SET));
    Regs.X.AX = 0x4F02;
    Regs.X.BX = mVideoMode;
    LegacyBios->Int86(LegacyBios, 0x10, &Regs);
  }
}




EFI_STATUS
FindNextPnpExpansionHeader (
  IN BOOLEAN                           FirstHeader,
  IN OUT LEGACY_PNP_EXPANSION_HEADER   **PnpPtr

)
{
  UINTN                       TempData;
  LEGACY_PNP_EXPANSION_HEADER *LocalPnpPtr;
  LocalPnpPtr = *PnpPtr;
  if (FirstHeader == FIRST_INSTANCE) {
    mBasePnpPtr     = LocalPnpPtr;
    mBbsRomSegment  = (UINT16) ((UINTN) mBasePnpPtr >> 4);
    //
    // Offset 0x1a gives offset to PnP expansion header for the first
    // instance, there after the structure gives the offset to the next
    // structure
    //
    LocalPnpPtr = (LEGACY_PNP_EXPANSION_HEADER *) ((UINT8 *) LocalPnpPtr + 0x1a);
    TempData    = (*((UINT16 *) LocalPnpPtr));
  } else {
    TempData = (UINT16) LocalPnpPtr->NextHeader;
  }

  LocalPnpPtr = (LEGACY_PNP_EXPANSION_HEADER *) (((UINT8 *) mBasePnpPtr + TempData));

  //
  // Search for PnP table in Shadowed ROM
  //
  *PnpPtr = LocalPnpPtr;
  if (*(UINT32 *) LocalPnpPtr == SIGNATURE_32 ('$', 'P', 'n', 'P')) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

VOID
SaveOprom (
  UINT8   *Rom,
  UINT8   *RtDataAddr,
  UINT16  RtDataSize
)
{
  UINT16 Index;
  UINTN  RomSize = ((EFI_LEGACY_EXPANSION_ROM_HEADER*)Rom)->Size512 << 9;

  if (RomSize == 0) {
    DEBUG ((EFI_D_ERROR, "SaveOprom return abnormally!\n"));
    return;
  }
  //
  // If it has been saved, then just return
  //
  for (Index = 0; Index < mSavedCount; Index ++) {
    if (mSaveRomEntry[Index].Address == Rom){
      return;
    }
  }
  mSaveRomEntry[mSavedCount].Data = AllocateZeroPool(RomSize);
  CopyMem(mSaveRomEntry[mSavedCount].Data, Rom, RomSize);
  mSaveRomEntry[mSavedCount].Address = Rom;
  //
  // Save runtime data associated with this ROM
  //
  if (RtDataSize) {
    ASSERT(RtDataAddr); // if size is not zero, address must not be zero
    mSaveRomEntry[mSavedCount].RtDataAddr = RtDataAddr;
    mSaveRomEntry[mSavedCount].RtDataSize = RtDataSize;
    mSaveRomEntry[mSavedCount].RtData = AllocateZeroPool(RtDataSize);
    CopyMem(mSaveRomEntry[mSavedCount].RtData, RtDataAddr, RtDataSize);
  }

  mSavedCount++;  // Points to zero address/data now.

  return;
}

EFI_STATUS
CallBcvHookInt13 (
  IN  EFI_LEGACY_BIOS_PROTOCOL          *LegacyBios,
  IN  EFI_COMPATIBILITY16_TABLE         *Compatibility16Table,
  IN  EFI_LEGACY_EXPANSION_ROM_HEADER   *RomStart,
  UINT8                                 *RtData,
  UINT32                                RtDataSize
)
{
  VOID                            *RomEnd;
  EFI_LEGACY_EXPANSION_ROM_HEADER *PciPtr;
  LEGACY_PNP_EXPANSION_HEADER     *PnpPtr;
  BOOLEAN                         Instance;
  EFI_STATUS                      Status;
  EFI_IA32_REGISTER_SET           Regs;

  //
  // if it doesn't match signature, then we should not hook int13 either.
  //
  if (*(UINT16 *)(UINTN)RomStart != 0xAA55)
    return EFI_SUCCESS;

  ASSERT ((LegacyBios != NULL) && (Compatibility16Table != NULL));

  PnpPtr    = (LEGACY_PNP_EXPANSION_HEADER *) RomStart;
  PciPtr    = (EFI_LEGACY_EXPANSION_ROM_HEADER *) RomStart;

  RomEnd    = (VOID *) (PciPtr->Size512 * 512 + (UINTN) PciPtr);
  Instance  = FIRST_INSTANCE;
  //
  // Save the context before calling BCV in order to restore it during legacy boot
  //
  SaveOprom ((UINT8 *)RomStart, RtData, (UINT16)RtDataSize);
//if (PciPtr >= (EFI_LEGACY_EXPANSION_ROM_HEADER *) ((UINTN) 0xc8000)) {
  while (TRUE) {
    Status    = FindNextPnpExpansionHeader (Instance, &PnpPtr);
    Instance  = NOT_FIRST_INSTANCE;
    if (EFI_ERROR (Status)) {
      break;
    }

    //
    // Have seen case where PXE base code have PnP expansion ROM
    // header but no Bcv or Bev vectors.
    //
    if (PnpPtr->Bcv != 0) {
      ZeroMem (&Regs, sizeof(Regs));
      Regs.X.ES = Compatibility16Table->PnPInstallationCheckSegment;
      Regs.X.DI = Compatibility16Table->PnPInstallationCheckOffset;
      Status = LegacyBios->FarCall86 (
                             LegacyBios,
                             mBbsRomSegment,
                             PnpPtr->Bcv,
                             &Regs,
                             NULL,
                             0
                             );
      DEBUG((EFI_D_INFO, "[Bcv] int13h:%X\n", *(UINT32*)(UINTN)(0x13*4)));
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }

    if ((PnpPtr == (LEGACY_PNP_EXPANSION_HEADER *) PciPtr) || (PnpPtr > (LEGACY_PNP_EXPANSION_HEADER *) RomEnd)) {
      break;
    }
  }
//}

  return EFI_SUCCESS;
}

UINT8 *
FindCompatibility16Table (
  VOID
)
/*++
Routine Description:
  Find the EFI_COMPATIBILITY16_TABLE in F segment.

Returns:
  The found EFI_COMPATIBILITY16_TABLE or NULL if not found.
--*/
{
  UINT8  *Table;

  for (Table = (UINT8 *) (UINTN) 0xF0000;
       Table < (UINT8 *) (UINTN) 0x100000;
       Table = (UINT8 *) Table + 0x10
      ) {
    if (*(UINT32 *) Table == EFI_COMPATIBILITY16_TABLE_SIGNATURE) {
      return Table;
    }
  }

  return NULL;
}


VOID
UpdateEbdaMap(UINT32 AddrChange)
{
  UINTN         Index;

  DEBUG ((EFI_D_INFO, "UpdateEbdaMap!\n"));

  //
  // for every SAVED_PCI_ROM update ebdaAddr
  //
  for (Index = 0; Index < mSavedCount; Index++) {
    if (mSaveRomEntry[Index].RtDataAddr != NULL) {
      DEBUG ((EFI_D_INFO, "Original RtDataAddr = 0x%x\n", mSaveRomEntry[Index].RtDataAddr));
      mSaveRomEntry[Index].RtDataAddr -= AddrChange;
      DEBUG ((EFI_D_INFO, "New RtDataAddr = 0x%x\n", mSaveRomEntry[Index].RtDataAddr));
    }
  }
}



UINT32
EFIAPI
AhciReadReg (
  IN EFI_PCI_IO_PROTOCOL  *PciIo,
  IN  UINT32              Offset
)
{
  UINT32                  Data;

  ASSERT (PciIo != NULL);
  Data = 0;
  PciIo->Mem.Read (
    PciIo,
    EfiPciIoWidthUint32,
    EFI_AHCI_BAR_INDEX,
    (UINT64) Offset,
    1,
    &Data
  );
  return Data;
}

VOID
EFIAPI
AhciWriteReg (
  IN EFI_PCI_IO_PROTOCOL  *PciIo,
  IN UINT32               Offset,
  IN UINT32               Data
)
{
  ASSERT (PciIo != NULL);

  PciIo->Mem.Write (
    PciIo,
    EfiPciIoWidthUint32,
    EFI_AHCI_BAR_INDEX,
    (UINT64) Offset,
    1,
    &Data
  );
}


BOOLEAN OptionRomMessageNeedOrNot(BOOLEAN IsLegacyAhciRom)
{
  BOOLEAN  ShowScrMsg = TRUE;

  ShowScrMsg = PcdGet8(PcdDisplayOptionRomMessage);
  if(ShowScrMsg && IsLegacyAhciRom){
    ShowScrMsg = FALSE;
  }

  return ShowScrMsg;
}


VOID AgesaVgaEn(EFI_HANDLE DeviceHandle)
{
  EFI_STATUS                      Status;
  EFI_DEVICE_PATH_PROTOCOL        *Dp;
  ACPI_HID_DEVICE_PATH            *AcpiHidHp;
  PLATFORM_COMM_INFO              *Info;
  FABRIC_RESOURCE_MANAGER_PROTOCOL  *Frmp;
  FABRIC_TARGET                     Target;
  EFI_PCI_IO_PROTOCOL               *PciIo = NULL;
  UINTN                             Segment, Bus, Dev, Func;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  Status = gBS->HandleProtocol (
                  DeviceHandle,
                  &gEfiDevicePathProtocolGuid,
                  &Dp
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  if(Dp->Type == ACPI_DEVICE_PATH && Dp->SubType == ACPI_DP){
    AcpiHidHp = (ACPI_HID_DEVICE_PATH*)Dp;
    if(AcpiHidHp->HID != EISA_PNP_ID (0x0A03)){
      goto ProcExit;
    }

    Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();
    Info->VgaUid = (UINT8)AcpiHidHp->UID;

    Status = gBS->HandleProtocol (
                    DeviceHandle,
                    &gEfiPciIoProtocolGuid,
                    &PciIo
                    );
    if(EFI_ERROR(Status)){
     goto ProcExit;
    }
    PciIo->GetLocation(PciIo, &Segment, &Bus, &Dev, &Func);
    Target.TgtType = TARGET_PCI_BUS;
    Target.PciBusNum = (UINT8)Bus;
    Status = gBS->LocateProtocol (
                    &gHygonFabricResourceManagerServicesProtocolGuid,
                    NULL,
                    (VOID**)&Frmp
                    );
    ASSERT(!EFI_ERROR(Status));
    Status = Frmp->FabricEnableVgaMmio(Frmp, Target);
    DEBUG((EFI_D_INFO, "FabricEnableVgaMmio:%r\n", Status));
  }

ProcExit:
  return;
}



VOID SaveAhciMemBaseByPciIo(AHCI_FIS_CMDLIST_SAVE *AhciSave, EFI_PCI_IO_PROTOCOL *PciIo, UINT32 PciId)
{
  UINTN          Port = 0;
  UINT32         Offset;
  MY_DATA_64     Data64;

  AhciSave->PciIo = PciIo;
  AhciSave->PciId = PciId;
  AhciSave->PortCount = (AhciReadReg(PciIo, 0) & 0x1F) + 1;

  for (Port = 0; Port < AhciSave->PortCount; Port++) {
    Offset = 0x100 + (UINT32)Port * 0x80;         // base
    Data64.Uint32.Lower32 = AhciReadReg(PciIo, Offset+0x08);
    Data64.Uint32.Upper32 = AhciReadReg(PciIo, Offset+0x0C);
    AhciSave->NativeFis[Port] = Data64.Uint64;
    Data64.Uint32.Lower32 = AhciReadReg(PciIo, Offset+0x00);
    Data64.Uint32.Upper32 = AhciReadReg(PciIo, Offset+0x04);
    AhciSave->NativeCmdList[Port] = Data64.Uint64;
  }
}

VOID RestoreAhciMemBase(AHCI_FIS_CMDLIST_SAVE *AhciSave)
{
  UINTN               Port = 0;
  UINT32              Offset;
  MY_DATA_64          Data64;
  EFI_PCI_IO_PROTOCOL *PciIo = AhciSave->PciIo;

  for (Port = 0; Port < AhciSave->PortCount; Port++) {
    Offset = 0x100 + (UINT32)Port * 0x80;         // base
    Data64.Uint32.Lower32 = AhciReadReg(PciIo, Offset+0x08);
    Data64.Uint32.Upper32 = AhciReadReg(PciIo, Offset+0x0C);
    AhciSave->LegacyFis[Port] = Data64.Uint64;
    Data64.Uint32.Lower32 = AhciReadReg(PciIo, Offset+0x00);
    Data64.Uint32.Upper32 = AhciReadReg(PciIo, Offset+0x04);
    AhciSave->LegacyCmdList[Port] = Data64.Uint64;

    Data64.Uint64 = AhciSave->NativeFis[Port];
    AhciWriteReg(PciIo, Offset+0x08, Data64.Uint32.Lower32);
    AhciWriteReg(PciIo, Offset+0x0C, Data64.Uint32.Upper32);
    Data64.Uint64 = AhciSave->NativeCmdList[Port];
    AhciWriteReg(PciIo, Offset+0x00, Data64.Uint32.Lower32);
    AhciWriteReg(PciIo, Offset+0x04, Data64.Uint32.Upper32);
  }
}



BOOLEAN IsPciIoPresentInSaveTable(EFI_PCI_IO_PROTOCOL *PciIo)
{
  UINTN                 Index;
  AHCI_FIS_CMDLIST_SAVE *AhciSave;

  for (Index = 0; Index < gAhciAddrSaveCurCount; Index++) {
    AhciSave = &gAhciAddrSaveArray[Index];
    if(AhciSave->PciIo == PciIo){
      return TRUE;
    }
  }
  return FALSE;
}


VOID SaveAllAsmediaAhciMemBase()
{
  UINTN                 HandleCount;
  EFI_HANDLE            *HandleBuffer;
  EFI_STATUS            Status;
  UINTN                 Index;
  EFI_PCI_IO_PROTOCOL   *PciIo;
  UINT32                PciId;
  UINT8                 ClassCode[3];
  AHCI_FIS_CMDLIST_SAVE *AhciSave;


  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if(EFI_ERROR(Status) || HandleCount == 0){
    return;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    &PciIo
                    );
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0, 1, &PciId);
    if((UINT16)PciId != ASMEDIA_VENDOR_ID){
      continue;
    }

    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 9, 3, ClassCode);
    if(ClassCode[2] != 1 || ClassCode[1] != 6 || ClassCode[0] != 1){
      continue;
    }

    if(gAhciAddrSaveCurCount < AHCI_ADDR_SAVE_MAX_HOST_COUNT){
      AhciSave = &gAhciAddrSaveArray[gAhciAddrSaveCurCount];
      SaveAhciMemBaseByPciIo(AhciSave, PciIo, PciId);
      gAhciAddrSaveCurCount++;
    }
  }

  FreePool(HandleBuffer);
}


VOID RestoreAllAsmediaAhciMemBase()
{
  UINTN                 Index;
  AHCI_FIS_CMDLIST_SAVE *AhciSave;

  for (Index = 0; Index < gAhciAddrSaveCurCount; Index++) {
    AhciSave = &gAhciAddrSaveArray[Index];
    if((UINT16)AhciSave->PciId != ASMEDIA_VENDOR_ID){
      continue;
    }
    RestoreAhciMemBase(AhciSave);
  }
}



EFI_STATUS
EFIAPI
PlatformHooks (
  IN EFI_LEGACY_BIOS_PLATFORM_PROTOCOL              *This,
  IN EFI_GET_PLATFORM_HOOK_MODE                     Mode,
  IN UINT16                                         Type,
  IN EFI_HANDLE                                     DeviceHandle,          OPTIONAL
  IN OUT UINTN                                      *Shadowaddress,        OPTIONAL
  IN EFI_COMPATIBILITY16_TABLE                      *Compatibility16Table, OPTIONAL
  OUT VOID                                          **AdditionalData       OPTIONAL
)
{
  EFI_STATUS                      Status;
  EFI_PCI_IO_PROTOCOL             *PciIo = NULL;
  BOOLEAN                         IsLegacyRaidRom = FALSE;
  BOOLEAN                         IsLegacyLanRom = FALSE;
  BOOLEAN                         IsLegacyAhciRom = FALSE;
  BOOLEAN                         IsLegacySasRom = FALSE;
  BOOLEAN                         IsNvmeRom = FALSE;
  EFI_LEGACY_BIOS_PROTOCOL        *LegacyBios;
  EFI_IA32_REGISTER_SET           Regs;
  UINT16                          EbdaSegNew;
  UINT32                          RtDataSize;
  UINTN                           RuntimeAddress;
  UINT8                           *RtData = NULL;
  EFI_ATA_PASS_THRU_PROTOCOL      *AtaPassThru;
  UINT32                          PciId = 0xFFFFFFFF;    // wz161228.1 -
  UINT8                           ClassCode[3];
  AHCI_FIS_CMDLIST_SAVE           *AhciSave = NULL;
  EFI_HANDLE                      Handle;
  LEGACY_OPROM_INFO               *OpromInfo;
  UINTN                           Segment, Bus, Dev, Func;
  BOOLEAN                         ShowScrMsg;
  STATIC UINT8                    Vector[4] = {0x10, 0x1F, 0x43, 0x6D};
  STATIC UINT32                   VectorValue[4];
  EFI_INPUT_KEY                   Key;
  PLAT_HOST_INFO_PROTOCOL         *PlatHostInfo;
  STATIC BOOLEAN                  VgaEn = FALSE;
  UINTN                           i;
  PLATFORM_COMM_INFO              *PlatCommInfo;


  DEBUG((EFI_D_INFO, "%a(M:%d,A:0x%X)\n", __FUNCTION__, Mode, Shadowaddress==NULL?0:*Shadowaddress));

  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  &LegacyBios
                  );
  ASSERT_EFI_ERROR(Status);

  PlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  SetMem(ClassCode, sizeof(ClassCode), 0xFF);
  Segment = 0xFF;
  Bus     = 0xFF;
  Dev     = 0xFF;
  Func    = 0xFF;
  if (DeviceHandle != NULL) {
    Status = gBS->HandleProtocol (
                    DeviceHandle,
                    &gEfiPciIoProtocolGuid,
                    &PciIo
                    );
    if (!EFI_ERROR (Status)) {
      PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CLASSCODE_OFFSET, 3, ClassCode);
      PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, PCI_VENDOR_ID_OFFSET, 1, &PciId);
      DEBUG((EFI_D_INFO, "%08X (%02X,%02X,%02X)\n", PciId, ClassCode[2], ClassCode[1], ClassCode[0]));
      PciIo->GetLocation(PciIo, &Segment, &Bus, &Dev, &Func);
    }
  }

  if(!VgaEn && ClassCode[2] == PCI_BCC_DISPLAY){
    AgesaVgaEn(DeviceHandle);
    VgaEn = TRUE;
  }

  if(ClassCode[2] == PCI_BCC_NETWORK && !PlatCommInfo->LegacyPxeEn){
    return EFI_UNSUPPORTED;
  }

  if(ClassCode[2] == PCI_BCC_STORAGE && ClassCode[1] == PCI_SCC_AHCI){
    Status = gBS->HandleProtocol (
                    DeviceHandle,
                    &gEfiAtaPassThruProtocolGuid,
                    &AtaPassThru
                    );
    if(!EFI_ERROR (Status)){
      IsLegacyAhciRom = TRUE;
      ASSERT(gAhciAddrSaveCurCount < AHCI_ADDR_SAVE_MAX_HOST_COUNT);
      if(gAhciAddrSaveCurCount >= AHCI_ADDR_SAVE_MAX_HOST_COUNT){
        return EFI_UNSUPPORTED;
      }
      AhciSave = &gAhciAddrSaveArray[gAhciAddrSaveCurCount];
      DEBUG((EFI_D_INFO, "IsAhci I:%d\n", gAhciAddrSaveCurCount));

      //Patch for Asmedia OpRom
      if(PciId == 0x06221B21) {
        IsLegacyRaidRom = TRUE;
      }
    }
  } else if(ClassCode[2] == PCI_BCC_NETWORK && ClassCode[1] == 0 ){
    IsLegacyLanRom = TRUE;
  } else if(ClassCode[2] == PCI_BCC_STORAGE && ClassCode[1] == PCI_SCC_RAID ){
    IsLegacyRaidRom = TRUE;
  } else if(ClassCode[2] == 1 && ClassCode[1] == 8 && ClassCode[0] == 2){
    IsNvmeRom = TRUE;
  } else if(ClassCode[2] == 1 && ClassCode[1] == 7 && ClassCode[0] == 0){
    IsLegacySasRom = TRUE;
  }

  Status = EFI_SUCCESS;

  switch (Mode) {
    case EfiPlatformHookPrepareToScanRom:

      while(gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) == EFI_SUCCESS){};
      ZeroMem((VOID*)(UINTN)0x41E, 16);   // 40:1E-3D Bda1E_3D- key buffer -skip

      if (IsLegacyRaidRom || IsLegacyLanRom || IsLegacySasRom) {
          //RAID Card disable frb2 wdt
        Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &PlatHostInfo);
        if(!EFI_ERROR(Status)){
          PlatHostInfo->StopWdg();
        }
      }

      if (IsLegacyAhciRom) {
        if((UINT16)PciId == ASMEDIA_VENDOR_ID && !IsPciIoPresentInSaveTable(PciIo)){
          SaveAllAsmediaAhciMemBase(PciIo);
        } else {
          SaveAhciMemBaseByPciIo(AhciSave, PciIo, PciId);
        }
      }

      if(ClassCode[2] != PCI_BCC_DISPLAY){
        ShowScrMsg = OptionRomMessageNeedOrNot((IsLegacyAhciRom && ((UINT16)PciId) == HG_VENDOR_ID) || IsNvmeRom);
        if (ShowScrMsg) {
          PrepareToScanRomHookVideo(LegacyBios);
          ClearScreen();
          Regs.H.AH = 0x00;
          Regs.H.AL = 0x03;
          LegacyBios->Int86(LegacyBios, 0x10, &Regs);  // Set the 80x25 Text VGA Mode
        } else {
          for(i=0;i<ARRAY_SIZE(Vector);i++){
            VectorValue[i] = *(UINT32*)(UINTN)(Vector[i] * 4);
            *(UINT32*)(UINTN)(Vector[i] * 4) = 0xF000E020;
          }
        }
      }

      //
      // Show hide legacy option rom message
      //
      if (PcdGet8(PcdDisplayOptionRomMessage) == 0) {
        ShowHideOpromMsg ();
      }

      if (!mInt13VectorSaved) {
        mInt13Vector = *((UINT32*)(UINTN)(0x4c));
        mInt13VectorSaved = TRUE;
      }
      mEbdaSegOrg = *(UINT16*)(UINTN)0x40e;
      mEbdaSizeOrg = *(UINT8*)((UINTN)mEbdaSegOrg<<4);
      DEBUG ((EFI_D_INFO, "mEbdaSegOrg:0x%x, mEbdaSizeOrg:0x%x\n", mEbdaSegOrg, mEbdaSizeOrg));
      ASSERT(mEbdaSizeOrg);

      Handle = NULL;
      OpromInfo = AllocatePool(sizeof(LEGACY_OPROM_INFO));
      ASSERT(OpromInfo != NULL);
      OpromInfo->Bus  = (UINT8)Bus;
      OpromInfo->Dev  = (UINT8)Dev;
      OpromInfo->Func = (UINT8)Func;
      CopyMem(OpromInfo->ClassCode, ClassCode, sizeof(ClassCode));
      OpromInfo->OpromDataSize = (((UINT8*)(*Shadowaddress))[2])<<9;
      OpromInfo->OpromData = AllocatePool(OpromInfo->OpromDataSize);
      ASSERT(OpromInfo->OpromData!=NULL);
      CopyMem(OpromInfo->OpromData, (VOID*)(*Shadowaddress), OpromInfo->OpromDataSize);
      gBS->InstallProtocolInterface(
             &Handle,
             &gLegacyOptionRomInfoProtocolGuid,
             EFI_NATIVE_INTERFACE,
             OpromInfo
             );
      DEBUG ((EFI_D_INFO, "OpRomSize:%d\n", OpromInfo->OpromDataSize));
      break;

    case EfiPlatformHookShadowServiceRoms:
      break;

    case EfiPlatformHookAfterRomInit:

      if (IsLegacyRaidRom || IsLegacyLanRom || IsLegacySasRom) {
          //RAID Card Reset frb2 wdt
        Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &PlatHostInfo);
        if(!EFI_ERROR(Status)){
          PlatHostInfo->ResetFrb2Wdg();
        }
      }

      if(ClassCode[2] != PCI_BCC_DISPLAY){
        ShowScrMsg = OptionRomMessageNeedOrNot((IsLegacyAhciRom && ((UINT16)PciId) == HG_VENDOR_ID) || IsNvmeRom);
        if (ShowScrMsg) {
          AfterRomInitHookVideo(LegacyBios);
        } else {
          for(i=0;i<ARRAY_SIZE(Vector);i++){
            *(UINT32*)(UINTN)(Vector[i] * 4) = VectorValue[i];
          }
        }
      }
      //
      // Clear screen after option rom
      //
      if (ShowScrMsg) {
        gST->ConOut->ClearScreen (gST->ConOut);
      }

      if (IsLegacyAhciRom) {
        if((UINT16)PciId == ASMEDIA_VENDOR_ID){
          RestoreAllAsmediaAhciMemBase();
        } else {
          RestoreAhciMemBase(AhciSave);
          if(gAhciAddrSaveCurCount < AHCI_ADDR_SAVE_MAX_HOST_COUNT){
            gAhciAddrSaveCurCount++;
          }
        }
      }

      EbdaSegNew = *(UINT16*)(UINTN)0x40e;                    // Update EBDA map
      DEBUG ((EFI_D_INFO, "EbdaSegNew = 0x%x\n", EbdaSegNew));
      DEBUG ((EFI_D_INFO, "EbdaSegNew size = 0x%x\n", *(UINT8*)((UINTN)EbdaSegNew<<4)));
      ASSERT(EbdaSegNew <= mEbdaSegOrg);
      RtDataSize = (UINT32)(mEbdaSegOrg - EbdaSegNew) << 4;   // #of bytes taken by this OpROM
      DEBUG ((EFI_D_INFO, "RtDataSize = 0x%x\n", RtDataSize));

      if (RtDataSize) {   // EBDA was allocated and used by new OPROM, update the EBDA address
        UpdateEbdaMap(RtDataSize);
        RtData = (UINT8*)(((UINTN)EbdaSegNew<<4) + ((UINTN)mEbdaSizeOrg<<10));
        DEBUG ((EFI_D_INFO, "RtData = 0x%x\n", RtData));
      }
      RuntimeAddress = *Shadowaddress;

      //
      // Locate Compatibility16 Table if it is NULL when passed
      //
      if (Compatibility16Table == NULL) {
        Compatibility16Table = (EFI_COMPATIBILITY16_TABLE *) FindCompatibility16Table ();
        ASSERT (Compatibility16Table != NULL);
      }

      if(ClassCode[2] == PCI_BCC_STORAGE){
        CallBcvHookInt13 (
          LegacyBios,
          Compatibility16Table,
          (EFI_LEGACY_EXPANSION_ROM_HEADER *) RuntimeAddress,
          RtData,
          RtDataSize
          );
      }
      break;

    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}


#define IRQ_ROUTING_COUNT  0x200

#define LPC_BUS_NO         0
#define LPC_DEV_NO         0x14
#define LPC_FUNC_NO        4

#define LINK_NONE          0
#define PIRQA_LINK         1
#define PIRQB_LINK         2
#define PIRQC_LINK         3
#define PIRQD_LINK         4
#define PIRQE_LINK         5
#define PIRQF_LINK         6
#define PIRQG_LINK         7
#define PIRQH_LINK         8

#define PIRQ_MASK          0x0CE0
#define MASK_NONE          0

// HYGON Platform specific
#define PIRQ_NOT_SET(x)    ((x == 0) || (x == 0x1F))
#define MAX_IRQ_PRIORITY_ENTRIES    6
UINT8  IrqPriorityTable[] = {11, 10, 9, 7, 6, 5};
UINT8  IrqAllocatedCount[sizeof(IrqPriorityTable)] = {0};


#pragma pack(1)
typedef struct {
  EFI_LEGACY_PIRQ_TABLE_HEADER  Hdr;
  EFI_LEGACY_IRQ_ROUTING_ENTRY  Entry[IRQ_ROUTING_COUNT];
} PIR_DATA;
#pragma pack()

STATIC PIR_DATA gPirData = {
  {
    EFI_LEGACY_PIRQ_TABLE_SIGNATURE,
    0,
    1,
    sizeof(PIR_DATA),
    LPC_BUS_NO,
    (LPC_DEV_NO<<3) + LPC_FUNC_NO,
    0,                                     // PciOnlyIrq
    0x1002,                                // CompatibleVid
    0x439D,                                // CompatibleDid
    0,                                     // Miniport
    {0},                                   // Reserved
    0                                      // checksum
  },
/*
[00] (02,00,00) {05,0C30}{06,0C30}{07,0C30}{08,0C30} Slot:0
[01] (01,00,00) {01,0C30}{02,0C30}{03,0C30}{04,0C30} Slot:0
[02] (03,00,00) {00,0000}{05,0C30}{06,0C30}{00,0000} Slot:0
[03] (04,00,00) {02,0C30}{03,0C30}{00,0000}{00,0000} Slot:0
[04] (11,00,00) {00,0000}{05,0C30}{06,0C30}{00,0000} Slot:0
[05] (12,00,00) {02,0C30}{03,0C30}{00,0000}{00,0000} Slot:0
[06] (21,00,00) {00,0000}{05,0C30}{00,0000}{00,0000} Slot:0
[07] (22,00,00) {02,0C30}{00,0000}{00,0000}{00,0000} Slot:0

[08] (31,00,00) {07,0C30}{08,0C30}{05,0C30}{06,0C30} Slot:0
[09] (32,00,00) {00,0000}{05,0C30}{00,0000}{00,0000} Slot:0
[10] (33,00,00) {02,0C30}{00,0000}{00,0000}{00,0000} Slot:0
[11] (41,00,00) {00,0000}{05,0C30}{00,0000}{00,0000} Slot:0
[12] (42,00,00) {02,0C30}{03,0C30}{00,0000}{00,0000} Slot:0
[13] (51,00,00) {00,0000}{05,0C30}{00,0000}{00,0000} Slot:0
[14] (52,00,00) {02,0C30}{03,0C30}{00,0000}{00,0000} Slot:0
[15] (61,00,00) {00,0000}{05,0C30}{00,0000}{00,0000} Slot:0
[16] (62,00,00) {02,0C30}{00,0000}{00,0000}{00,0000} Slot:0
[17] (71,00,00) {00,0000}{05,0C30}{00,0000}{00,0000} Slot:0
[18] (72,00,00) {02,0C30}{00,0000}{00,0000}{00,0000} Slot:0
*/

  {
    {0x02, 0 << 3, {{PIRQE_LINK, PIRQ_MASK},{PIRQF_LINK, PIRQ_MASK},{PIRQG_LINK,PIRQ_MASK},{PIRQH_LINK,PIRQ_MASK}}, 0 ,0},
    {0x01, 0 << 3, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{PIRQC_LINK,PIRQ_MASK},{PIRQD_LINK,PIRQ_MASK}}, 0 ,0},
    {0x03, 0 << 3, {{LINK_NONE,  MASK_NONE},{PIRQE_LINK, PIRQ_MASK},{PIRQF_LINK,PIRQ_MASK},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x04, 0 << 3, {{PIRQB_LINK, PIRQ_MASK},{PIRQC_LINK, PIRQ_MASK},{LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x11, 0 << 3, {{LINK_NONE, MASK_NONE}, {PIRQE_LINK, PIRQ_MASK},{PIRQF_LINK,PIRQ_MASK},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x12, 0 << 3, {{PIRQB_LINK, PIRQ_MASK},{PIRQC_LINK, PIRQ_MASK},{LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x21, 0 << 3, {{LINK_NONE, MASK_NONE}, {PIRQE_LINK, PIRQ_MASK},{LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x22, 0 << 3, {{PIRQB_LINK, PIRQ_MASK},{LINK_NONE, MASK_NONE}, {LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x31, 0 << 3, {{PIRQG_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQE_LINK,PIRQ_MASK},{PIRQF_LINK,PIRQ_MASK}}, 0 ,0},
    {0x32, 0 << 3, {{LINK_NONE, MASK_NONE}, {PIRQE_LINK, PIRQ_MASK},{LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x33, 0 << 3, {{PIRQB_LINK, PIRQ_MASK},{LINK_NONE, MASK_NONE}, {LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x41, 0 << 3, {{LINK_NONE, MASK_NONE}, {PIRQE_LINK, PIRQ_MASK},{LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x42, 0 << 3, {{PIRQB_LINK, PIRQ_MASK},{PIRQC_LINK, PIRQ_MASK},{LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x51, 0 << 3, {{LINK_NONE, MASK_NONE}, {PIRQE_LINK, PIRQ_MASK},{LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x52, 0 << 3, {{PIRQB_LINK, PIRQ_MASK},{PIRQC_LINK, PIRQ_MASK},{LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x61, 0 << 3, {{LINK_NONE, MASK_NONE}, {PIRQE_LINK, PIRQ_MASK},{LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x62, 0 << 3, {{PIRQB_LINK, PIRQ_MASK},{LINK_NONE, MASK_NONE}, {LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x71, 0 << 3, {{LINK_NONE, MASK_NONE}, {PIRQE_LINK, PIRQ_MASK},{LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0},
    {0x72, 0 << 3, {{PIRQB_LINK, PIRQ_MASK},{LINK_NONE, MASK_NONE}, {LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0},
    //End
    {0xFF, 0 << 3, {{LINK_NONE , MASK_NONE},{LINK_NONE, MASK_NONE}, {LINK_NONE, MASK_NONE},{LINK_NONE, MASK_NONE}}, 0 ,0}
  }
};



UINT16
GetIrqRoutingEntry (
  VOID
  )
{
  UINT16 Index;
  UINT8 BusNumber;
  for (Index = 0; ; Index++) {
    BusNumber = gPirData.Entry[Index].Bus;
    if (BusNumber == 0xFF) {
      break;
    }
  }
  return Index;
}


EFI_STATUS
EFIAPI
GetRoutingTable (
  IN EFI_LEGACY_BIOS_PLATFORM_PROTOCOL            * This,
  OUT VOID                                        **RoutingTable,
  OUT UINTN                                       *RoutingTableEntries,
  OUT VOID                                        **LocalPirqTable, OPTIONAL
  OUT UINTN                                       *PirqTableSize, OPTIONAL
  OUT VOID                                        **LocalIrqPriorityTable, OPTIONAL
  OUT UINTN                                       *IrqPriorityTableEntries OPTIONAL
)
{
  UINT16                        PTableSize;
  UINT32                        Index;
  UINT8                         Bus;
  UINT8                         Device;
  UINT8                         Function;
  UINT8                         Checksum;
  UINT8                         *Ptr;
  EFI_STATUS                    Status;
  EFI_LEGACY_INTERRUPT_PROTOCOL *LegacyInterrupt;

  DEBUG((EFI_D_INFO, "GetRoutingTable\n"));
  Checksum = 0;
  if(LocalPirqTable!=NULL){
    PTableSize = sizeof (EFI_LEGACY_PIRQ_TABLE_HEADER) +
    sizeof (EFI_LEGACY_IRQ_ROUTING_ENTRY) * GetIrqRoutingEntry();

    Status = gBS->LocateProtocol (
                    &gEfiLegacyInterruptProtocolGuid,
                    NULL,
                    (VOID**)&LegacyInterrupt
                    );
    ASSERT_EFI_ERROR (Status);
    LegacyInterrupt->GetLocation (
                       LegacyInterrupt,
                       &Bus,
                       &Device,
                       &Function
                       );
    gPirData.Hdr.TableSize   = PTableSize;
    gPirData.Hdr.Bus         = Bus;
    gPirData.Hdr.DevFun      = (UINT8) ((Device << 3) + Function);
    Ptr = (UINT8 *) (&gPirData);
    //
    // Calculate checksum.
    //
    for (Index = 0; Index < PTableSize; Index++) {
      Checksum = (UINT8) (Checksum + (UINT8) *Ptr);
      Ptr += 1;
    }
    Checksum              = (UINT8) (0x00 - Checksum);
    gPirData.Hdr.Checksum = Checksum;

    *LocalPirqTable = &gPirData.Hdr;
    *PirqTableSize  = PTableSize;
  }

  *RoutingTable = &gPirData.Entry[0];
  *RoutingTableEntries = GetIrqRoutingEntry ();

  if (LocalIrqPriorityTable != NULL) {
    *LocalIrqPriorityTable    = IrqPriorityTable;
    *IrqPriorityTableEntries  = MAX_IRQ_PRIORITY_ENTRIES;
  }

  return EFI_SUCCESS;
}


/**
    This routine selects the IRQ from the list of allowed PCI
    interrupts, picks the best interrupt number according to
    interrupt priority table and programs the PCI interrupt
    router.

    @param        Int - zero based index in the list of router registers for INTA, INTB,...
    @param        Irq - new interrupt number
    @param        IrqMask - IRQ bit mask, bits are set for the interrupts that are not allowed

    @retval EFI_SUCCESS if interrupt is routed
    @retval EFI_ABORTED if routing register is already programmed

    @note  This routine can not be called externally. It is to be
              called after TranslatePirq returns the Rirq register is
              not programmed.

**/

EFI_STATUS
RoutePirq2Irq(
  IN  EFI_LEGACY_INTERRUPT_PROTOCOL    *LegacyInterrupt,
  IN  UINT8                            Int,
  IN  OUT  UINT8                       *Irq,
  IN  UINT16                           Mask
){
  UINT8                                NewIrq;
  UINT8                                Index;
  UINT8                                IrqIndex;
  UINT8                                IrqFound = FALSE;

  //
  // See if routing register is already programmed, return EFI_ABORTED if so.
  //
  NewIrq = *Irq;

  for (Index=0; Index<ARRAY_SIZE(IrqPriorityTable); Index++) {
    if (NewIrq == IrqPriorityTable[Index]) {
      DEBUG((DEBUG_ERROR, "The requested PIRQ[%d] is already programmed to IRQ%x. PCI Routing ABORTED.\n", Int, NewIrq));
      return EFI_ABORTED;
    }
  }

  //
  // Find the next available interrupt; IrqPriorityTable is zero-terminated array
  // used as priority list. Lower index in IrqPriorityTable indicates higher priority
  // interrupt.
  // Initially irq_priority_index is the index of zero in IrqPriorityTable; it will
  // be advanced to the beginning in the following for loop.
  //
  for (IrqIndex = 0; IrqIndex < sizeof(IrqPriorityTable); IrqIndex++) {
    DEBUG((DEBUG_ERROR,"IrqIndex %d....Allocated %d....\n", IrqIndex, IrqAllocatedCount[IrqIndex]));
    if (!((1 << IrqPriorityTable[IrqIndex]) & Mask)) {
      IrqFound = TRUE;    // Assume that IRQ was found
      for (Index = 0; Index < sizeof(IrqAllocatedCount); Index++) {
        // Check if the least allocated IRQ
        if (!((1 << IrqPriorityTable[Index]) & Mask)) {
          if (IrqAllocatedCount[IrqIndex] > IrqAllocatedCount[Index]) {
            IrqFound = FALSE;
            break;
          }
        }
      }
    }
    if (IrqFound) {
      break;
    }
  }
  if (!IrqFound){
    return EFI_NOT_FOUND;
  }

  IrqAllocatedCount[IrqIndex] += 1;
  NewIrq = IrqPriorityTable[IrqIndex];

  *Irq = NewIrq;
  return EFI_SUCCESS;
}


/**
  Translates the given PIRQ accounting for bridge.
  This function translates the given PIRQ back through all buses, if required,
  and returns the true PIRQ and associated IRQ.

  @param  This                  The protocol instance pointer.
  @param  PciBus                The PCI bus number for this device.
  @param  PciDevice             The PCI device number for this device.
  @param  PciFunction           The PCI function number for this device.
  @param  Pirq                  Input is PIRQ reported by device, and output is true PIRQ.
  @param  PciIrq                The IRQ already assigned to the PIRQ, or the IRQ to be
                                assigned to the PIRQ.

  @retval EFI_SUCCESS           The PIRQ was translated.

**/
EFI_STATUS
EFIAPI
TranslatePirq (
  IN        EFI_LEGACY_BIOS_PLATFORM_PROTOCOL           *This,
  IN        UINTN                                       PciBus,
  IN        UINTN                                       PciDevice,
  IN        UINTN                                       PciFunction,
  IN  OUT   UINT8                                       *Pirq,
      OUT   UINT8                                       *PciIrq
  )
{
  EFI_LEGACY_INTERRUPT_PROTOCOL      *LegacyInterrupt;
  EFI_STATUS                         Status;
  UINTN                              Index;
  UINT8                              LocalPirq;
  UINT8                              PirqData;
  UINT16                             IrqMask;
  UINT16                             IrqEntryCount;

  DEBUG((EFI_D_INFO, "TranslatePirq\n"));

  Status = gBS->LocateProtocol (
                  &gEfiLegacyInterruptProtocolGuid,
                  NULL,
                  (VOID**)&LegacyInterrupt
                  );
  ASSERT_EFI_ERROR (Status);
  LocalPirq = (UINT8) (*Pirq);

  IrqEntryCount = GetIrqRoutingEntry ();
  for (Index = 0; Index < IrqEntryCount; Index++) {
    if ((gPirData.Entry[Index].Bus == PciBus) &&
        (gPirData.Entry[Index].Device == PciDevice)) {
      LocalPirq = (UINT8) (gPirData.Entry[Index].PirqEntry[LocalPirq].Pirq & 0x0f);
      if (LocalPirq > 8) {
        LocalPirq -= 8;
      }else if(LocalPirq == 0){
        DEBUG((EFI_D_INFO, "LocalPirq == 0!!! ignore it!\n"));
        continue;
      }else{
        LocalPirq -= 1;
      }

      LegacyInterrupt->ReadPirq (LegacyInterrupt, LocalPirq, &PirqData);
      DEBUG((EFI_D_INFO, " ReadPirq  LocalPirq:%x PirqData:%x\n", LocalPirq,PirqData));

      if(PIRQ_NOT_SET(PirqData)){
        IrqMask = (UINT16)~PIRQ_MASK; // PIC-style mask (1 is for interrupts that are not allowed)
        Status = RoutePirq2Irq(LegacyInterrupt,LocalPirq, &PirqData, IrqMask);
        if(EFI_ERROR(Status)){
          return EFI_NOT_FOUND;
        }
      }

      *PciIrq = PirqData;  //pci irq, 5,6,7,10,11
      *Pirq   = LocalPirq; //pirq a,b,c,d,e,f,g,h

      DEBUG((EFI_D_INFO, " final  PciIrq = %x, Pirq = %x\n", *PciIrq,*Pirq ));
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
PrepareToBoot (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL           *This,
  IN  BBS_BBS_DEVICE_PATH                         *BbsDevicePath,
  IN  VOID                                        *BbsTable,
  IN  UINT32                                      LoadOptionsSize,
  IN  VOID                                        *LoadOptions,
  IN  EFI_TO_COMPATIBILITY16_BOOT_TABLE           *EfiToLegacy16BootTable
)
{
  DEVICE_PRODUCER_DATA_HEADER   *SioPtr;
  SAVED_PCI_ROM                 *Rom;
  UINTN                         Index;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  SioPtr                  = &EfiToLegacy16BootTable->SioData;
  SioPtr->Flags.A20Kybd   = 0;
  SioPtr->Flags.A20Port90 = 0;

// Restore ENV(ROM space and EBDA) for RAID boot
  for (Index = 0; Index < mSavedCount; Index++) {
    Rom = &mSaveRomEntry[Index];
    CopyMem (
      Rom->Address,
      Rom->Data,
      ((EFI_LEGACY_EXPANSION_ROM_HEADER*)Rom->Data)->Size512 * 0x200
    );
    if (Rom->RtDataAddr) {
      CopyMem (
        Rom->RtDataAddr,
        Rom->RtData,
        Rom->RtDataSize
      );
    }
  }

// Reset number of ATA drives to let CSM16 PrepareToBoot function connect the drives
  *(UINT8*)(UINTN)0x475 = 0;

  if(mInt13VectorSaved){
    *((UINT32*)(UINTN)0x4C) = mInt13Vector;
  }
  return EFI_SUCCESS;
}



#define CSM16_INFO_DEBUG_SUPPORT               0

#if CSM16_INFO_DEBUG_SUPPORT

#define    MaxBcvNum          20
#define    MaxOpRomNum        10

#pragma pack(1)

typedef struct {
    UINT32    Int13[MaxBcvNum];
    UINT8     DriverNumber[MaxBcvNum];
    UINT8     LogicNumber[MaxBcvNum];
    UINT8     BbsIndex[MaxBcvNum];
} BCV_BOOT_TABLE;  

#pragma pack()


VOID GetInfoBeforeLegacyBoot()
{
  EFI_STATUS                     Status;
  EFI_LEGACY_INTERRUPT_PROTOCOL  *LegacyInterrupt;
  UINT8                          NumberPirqs;
  UINT8                          PirqNumber;
  UINT8                          PirqData;
  UINTN                          Seg, Bus, Dev, Func;
	UINTN                          HandleCount;
	EFI_HANDLE 		                 *HandleBuffer = NULL;
	UINTN                          Index;
	EFI_PCI_IO_PROTOCOL            *PciIo;
  UINT8                          IntLine;
  UINT8                          IntPin;
  UINT32                         PciId;
  BCV_BOOT_TABLE                 *Bcv  = (BCV_BOOT_TABLE*)(UINTN)0xFDC50;
  UINT8                          *Ebda = (UINT8*)(((UINTN)*(UINT16*)(UINTN)0x40E) << 4);
  UINTN                          Count;
  UINT16                         *Data16;
  
  
  
  Status = gBS->LocateProtocol (
                  &gEfiLegacyInterruptProtocolGuid,
                  NULL,
                  (VOID**)&LegacyInterrupt
                  );
  if(!EFI_ERROR(Status)){
    LegacyInterrupt->GetNumberPirqs(LegacyInterrupt, &NumberPirqs);
    for(PirqNumber=0;PirqNumber<NumberPirqs;PirqNumber++){
      Status = LegacyInterrupt->ReadPirq(LegacyInterrupt, PirqNumber, &PirqData);
      if(PirqData != 0x1F){
        DEBUG((EFI_D_INFO, "irq %c -> %d\n", PirqNumber+'A', PirqData));
      }
    }
  }


	Status = gBS->LocateHandleBuffer(
	                ByProtocol, 
	                &gEfiPciIoProtocolGuid, 
	                NULL, 
	                &HandleCount, 
	                &HandleBuffer
	                );
	if(!EFI_ERROR(Status)){
  	for(Index=0; Index<HandleCount; Index++){
  		Status = gBS->HandleProtocol(
  		                HandleBuffer[Index],
  		                &gEfiPciIoProtocolGuid, 
                      (void**)&PciIo
  		                );
      PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0x3d, 1, &IntPin);
      if(IntPin == 0){
        continue;
      }
  		PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0, 1, &PciId);
  		PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0x3c, 1, &IntLine);
      PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);
      DEBUG((EFI_D_INFO, "(%02X,%02X,%02X) %08X Pin(%02X) -> Line(%02X)\n", Bus, Dev, Func, PciId, IntPin, IntLine));      
    }
    gBS->FreePool(HandleBuffer);
  }

  DEBUG((EFI_D_INFO, "[4D0] %04X\n", IoRead16(0x4D0)));


  DEBUG((EFI_D_INFO, "Csm16Dump\n"));  

  Count = Ebda[0x1F];
  if(Count > MaxBcvNum){
    Count = MaxBcvNum;
  }
  for(Index=0;Index<Count; Index++){
    DEBUG((EFI_D_INFO, "Bcv[%d] %x DN:%x LN:%x I:%x\n", Index, \
      Bcv->Int13[Index], Bcv->DriverNumber[Index], Bcv->LogicNumber[Index], Bcv->BbsIndex[Index]));
  }
  DEBUG((EFI_D_INFO, "Int18Vec:%x\n", *(UINT32*)&Ebda[0x10B]));
  DEBUG((EFI_D_INFO, "Int19Vec:%x\n", *(UINT32*)&Ebda[0x10F]));

  DEBUG((EFI_D_INFO, "NumberOfDrives:%d\n", *(UINT8*)(UINTN)0x475));
  Data16 = (UINT16*)&Ebda[0x12c];
  for(Index=0;Index<MaxOpRomNum; Index++){
    DEBUG((EFI_D_INFO, "OpRomSegment[%d]: %x\n", Index, *Data16));
    Data16++;
  }
}

#endif



VOID
LegacyBootEvent (
  IN EFI_EVENT           Event,
  IN VOID                *Context
)
{
  UINTN                  Port;
  UINT32                 Offset;
  MY_DATA_64             Data64;
  AHCI_FIS_CMDLIST_SAVE  *AhciSave;
  UINTN                  Index;


  DEBUG((EFI_D_INFO, "LegacyBootEvent Count:%d\n", gAhciAddrSaveCurCount));

  if (gAhciAddrSaveCurCount) {
    for(Index = 0; Index < gAhciAddrSaveCurCount; Index++){
      AhciSave = &gAhciAddrSaveArray[Index];
      for (Port = 0; Port < AhciSave->PortCount; Port++) {
        Offset = 0x100 + (UINT32)Port * 0x80;      // base
        Data64.Uint64 = AhciSave->LegacyFis[Port];
        AhciWriteReg(AhciSave->PciIo, Offset+0x08, Data64.Uint32.Lower32);
        AhciWriteReg(AhciSave->PciIo, Offset+0x0C, Data64.Uint32.Upper32);
        Data64.Uint64 = AhciSave->LegacyCmdList[Port];
        AhciWriteReg(AhciSave->PciIo, Offset+0x00, Data64.Uint32.Lower32);
        AhciWriteReg(AhciSave->PciIo, Offset+0x04, Data64.Uint32.Upper32);
      }
    }
  }

#if CSM16_INFO_DEBUG_SUPPORT
  GetInfoBeforeLegacyBoot();
#endif

  //
  // Enable Cursor when enter into Legacy boot
  //
  gST->ConOut->EnableCursor (gST->ConOut, TRUE);
}


VOID
LegacyBootExitEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  UINTN                  Port;
  UINT32                 Offset;
  MY_DATA_64             Data64;
  AHCI_FIS_CMDLIST_SAVE  *AhciSave;
  UINTN                  Index;

  DEBUG((EFI_D_INFO, "LegacyBootExitEvent AhciCount:%d\n", gAhciAddrSaveCurCount));

  if (gAhciAddrSaveCurCount) {
    for(Index = 0; Index < gAhciAddrSaveCurCount; Index++){
      AhciSave = &gAhciAddrSaveArray[Index];
      for (Port = 0; Port < AhciSave->PortCount; Port++) {
        Offset = 0x100 + (UINT32)Port * 0x80;      // base
        Data64.Uint64 = AhciSave->NativeFis[Port];
        AhciWriteReg(AhciSave->PciIo, Offset+0x08, Data64.Uint32.Lower32);
        AhciWriteReg(AhciSave->PciIo, Offset+0x0C, Data64.Uint32.Upper32);
        Data64.Uint64 = AhciSave->NativeCmdList[Port];
        AhciWriteReg(AhciSave->PciIo, Offset+0x00, Data64.Uint32.Lower32);
        AhciWriteReg(AhciSave->PciIo, Offset+0x04, Data64.Uint32.Upper32);
      }
    }
  }

}




STATIC EFI_LEGACY_BIOS_PLATFORM_PROTOCOL  gLegacyBiosPlatform = {
  GetPlatformInfo,
  GetPlatformHandle,
  SmmInit,
  PlatformHooks,
  GetRoutingTable,
  TranslatePirq,
  PrepareToBoot
};

VOID
SetSmmPort (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                Status;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  UINT16                    EBdaSeg;
  UINT16                    *SmmPortPtr;

  Status = gBS->LocateProtocol(
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID**)&LegacyBios
                  );
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  EBdaSeg = *(UINT16 *)(UINTN)0x40E;
  SmmPortPtr = (UINT16 *)(UINTN)(((UINT32)EBdaSeg << 4) + 0x1D0); // Offset 0x1D0
  *SmmPortPtr = _PCD_VALUE_PcdSwSmiCmdPort;
}

typedef struct  {
  UINT8                  Device;
  UINT8                  Function;
  UINT8                  LogicalId;
  EFI_LEGACY_PIRQ_ENTRY  IntMap[4];
}HYGON_LOGICAL_BRIDGE_NUMBER;

CONST HYGON_LOGICAL_BRIDGE_NUMBER HygonLogicalId[] = {
  {1, 1,  0, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{PIRQC_LINK,PIRQ_MASK},{PIRQD_LINK,PIRQ_MASK}}},
  {1, 2,  1, {{PIRQE_LINK, PIRQ_MASK},{PIRQF_LINK, PIRQ_MASK},{PIRQG_LINK,PIRQ_MASK},{PIRQH_LINK,PIRQ_MASK}}},
  {1, 3,  2, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{PIRQC_LINK,PIRQ_MASK},{PIRQD_LINK,PIRQ_MASK}}},
  {1, 4,  3, {{PIRQE_LINK, PIRQ_MASK},{PIRQF_LINK, PIRQ_MASK},{PIRQG_LINK,PIRQ_MASK},{PIRQH_LINK,PIRQ_MASK}}},
  {1, 5,  4, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{PIRQC_LINK,PIRQ_MASK},{PIRQD_LINK,PIRQ_MASK}}},
  {1, 6,  5, {{PIRQE_LINK, PIRQ_MASK},{PIRQF_LINK, PIRQ_MASK},{PIRQG_LINK,PIRQ_MASK},{PIRQH_LINK,PIRQ_MASK}}},
  {1, 7,  6, {{PIRQA_LINK, PIRQ_MASK},{PIRQB_LINK, PIRQ_MASK},{PIRQC_LINK,PIRQ_MASK},{PIRQD_LINK,PIRQ_MASK}}},

  {2, 1,  7, {{PIRQE_LINK, PIRQ_MASK},{PIRQF_LINK, PIRQ_MASK},{PIRQG_LINK,PIRQ_MASK},{PIRQH_LINK,PIRQ_MASK}}},

  {3, 1,  8, {{PIRQG_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQE_LINK,PIRQ_MASK},{PIRQF_LINK,PIRQ_MASK}}},
  {3, 2,  9, {{PIRQC_LINK, PIRQ_MASK},{PIRQD_LINK, PIRQ_MASK},{PIRQA_LINK,PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK}}},
  {3, 3, 10, {{PIRQG_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQE_LINK,PIRQ_MASK},{PIRQF_LINK,PIRQ_MASK}}},
  {3, 4, 11, {{PIRQC_LINK, PIRQ_MASK},{PIRQD_LINK, PIRQ_MASK},{PIRQA_LINK,PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK}}},
  {3, 5, 12, {{PIRQG_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQE_LINK,PIRQ_MASK},{PIRQF_LINK,PIRQ_MASK}}},
  {3, 6, 13, {{PIRQC_LINK, PIRQ_MASK},{PIRQD_LINK, PIRQ_MASK},{PIRQA_LINK,PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK}}},
  {3, 7, 14, {{PIRQG_LINK, PIRQ_MASK},{PIRQH_LINK, PIRQ_MASK},{PIRQE_LINK,PIRQ_MASK},{PIRQF_LINK,PIRQ_MASK}}},

  {4, 1, 15, {{PIRQC_LINK, PIRQ_MASK},{PIRQD_LINK, PIRQ_MASK},{PIRQA_LINK,PIRQ_MASK},{PIRQB_LINK,PIRQ_MASK}}},

  {7, 1, 16, {{PIRQH_LINK, PIRQ_MASK},{PIRQE_LINK, PIRQ_MASK},{PIRQF_LINK,PIRQ_MASK},{PIRQG_LINK,PIRQ_MASK}}},
  {8, 1, 17, {{PIRQB_LINK, PIRQ_MASK},{PIRQC_LINK, PIRQ_MASK},{PIRQD_LINK,PIRQ_MASK},{PIRQA_LINK,PIRQ_MASK}}},

  {5, 1, 18, {{PIRQF_LINK, PIRQ_MASK},{PIRQG_LINK, PIRQ_MASK},{PIRQH_LINK,PIRQ_MASK},{PIRQE_LINK,PIRQ_MASK}}},
  {5, 2, 19, {{PIRQB_LINK, PIRQ_MASK},{PIRQC_LINK, PIRQ_MASK},{PIRQD_LINK,PIRQ_MASK},{PIRQA_LINK,PIRQ_MASK}}},
};




STATIC EFI_DEVICE_PATH_PROTOCOL gEndDp = DP_END;

BOOLEAN 
GetPcieRootPortInfo (
  IN  EFI_DEVICE_PATH_PROTOCOL *DevDp, 
  OUT UINT8                    *pSlotNum,
  OUT UINT8                    *pLogicalBridgeNum
  )
{
  BOARDCARD_PATH1           Dp1;
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *Dp = (EFI_DEVICE_PATH_PROTOCOL*)&Dp1;
  EFI_HANDLE                RootHandle;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  UINT8                     CapabilityPtr;
  UINT8                     CapabilityID;
  UINT16                    CapabilityEntry;
  UINT32                    SlotCap;  
  UINTN                     Index;
  BOARDCARD_PATH1           *RpDp;
  UINT32                    PciId;


  if(GetDevicePathSize(DevDp) < sizeof(BOARDCARD_PATH1)){
    DEBUG((EFI_D_INFO, "dp too small.\n"));
    return FALSE;
  }

  CopyMem(&Dp1, DevDp, sizeof(BOARDCARD_PATH1));
  CopyMem(((UINT8*)&Dp1)+sizeof(BOARDCARD_PATH1)-4, &gEndDp, 4);

  if(DevicePathNodeLength(&Dp1.PciRootBridgeDevicePath) != sizeof(ACPI_HID_DEVICE_PATH) || 
     DevicePathNodeLength(&Dp1.PciBridge1Path) != sizeof(PCI_DEVICE_PATH)){
    DEBUG((EFI_D_INFO, "bad dp node size\n"));
    return FALSE;
  }

  Status = gBS->LocateDevicePath (
                 &gEfiPciIoProtocolGuid,
                 &Dp,
                 &RootHandle
                 );
  if (EFI_ERROR (Status) || !IsDevicePathEnd(Dp)) {
    DEBUG((EFI_D_INFO, "PciIo not found\n"));
    return FALSE;
  }

  *pSlotNum          = 0xFF;
  *pLogicalBridgeNum = 0xFF;

  Status = gBS->HandleProtocol(
                  RootHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID**)&PciIo
                  );
  PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, PCI_VENDOR_ID_OFFSET, 1, &PciId);
  if((UINT16)PciId != HG_VENDOR_ID){
    DEBUG((EFI_D_INFO, "Not Hygon\n"));
    return FALSE;
  }
  
  PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CAPBILITY_POINTER_OFFSET, 1, &CapabilityPtr);

  while ((CapabilityPtr >= 0x40) && ((CapabilityPtr & 0x03) == 0x00)) {
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, CapabilityPtr, 1, &CapabilityEntry);
    CapabilityID = (UINT8) CapabilityEntry;

    if ((UINT8)EFI_PCI_CAPABILITY_ID_PCIEXP == CapabilityID) {
      PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, CapabilityPtr + 0x14, 1, &SlotCap);
      *pSlotNum = (UINT8)((SlotCap & 0xfff80000) >> 19);
      break;
    }
    CapabilityPtr = (UINT8)(CapabilityEntry >> 8);
  }
  
  RpDp = (BOARDCARD_PATH1*)DevicePathFromHandle(RootHandle);
  for(Index=0;Index<ARRAY_SIZE(HygonLogicalId);Index++){
    if(HygonLogicalId[Index].Device == RpDp->PciBridge1Path.Device &&
       HygonLogicalId[Index].Function == RpDp->PciBridge1Path.Function){
      *pLogicalBridgeNum = HygonLogicalId[Index].LogicalId;
      break;
    }
  }

  if(*pSlotNum == 0xFF || *pLogicalBridgeNum == 0xFF){
    return FALSE;
  }

  return TRUE;
}



VOID
LegacyBiosConnectPciRootBridgeHook()
{
  EFI_STATUS                         Status;
  EFI_HANDLE                         *Handles;
  UINTN                              HandleCount;
  UINTN                              Index;
  EFI_PCI_IO_PROTOCOL                *PciIo;
  UINT8                              ClassCode[3];
  UINT32                             PciId;
  UINTN                              s, b, d, f;
  UINT16                             PirqCount;
  STATIC EFI_LEGACY_IRQ_ROUTING_ENTRY  NewEntry[IRQ_ROUTING_COUNT];
  UINTN                              NewEntryCount;
  EFI_DEVICE_PATH_PROTOCOL           *DevicePath;
  UINT8                              IntPin;
  UINT8                              SlotNum;
  UINT8                              LogicalBridgeNum;


  DEBUG((EFI_D_INFO, "LegacyBiosConnectPciRootBridgeHook\n"));

  Handles = NULL;
  NewEntry[0].Bus = 0xFF;
  NewEntryCount   = 0;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  for(Index=0;Index<HandleCount;Index++){
    Status = gBS->HandleProtocol(
                    Handles[Index],
                    &gEfiPciIoProtocolGuid,
                    &PciIo
                    );

    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, PCI_VENDOR_ID_OFFSET, 1, &PciId);
    if((UINT16)PciId == 0xFFFF){
      continue;
    }
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CLASSCODE_OFFSET, 3, ClassCode);
    if(ClassCode[2]== 0x6 && (UINT16)PciId == 0x1D94){
      continue;
    }

    if(PciId == 0x185A1D94 || PciId == 0x14851D94){
      continue;
    }

    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_INT_PIN_OFFSET, 1, &IntPin);
    PciIo->GetLocation(PciIo, &s, &b, &d, &f);

    DevicePath = DevicePathFromHandle(Handles[Index]);
    ShowDevicePathDxe(gBS, DevicePath);

    if(!GetPcieRootPortInfo(DevicePath, &SlotNum, &LogicalBridgeNum)){
      continue;
    }
    DEBUG((EFI_D_INFO, "(%x,%x,%x) slotid:%d, LogicalBridgeNum:%d\n", b, d, f, SlotNum, LogicalBridgeNum));

    if(NewEntryCount != 0 && NewEntry[NewEntryCount-1].Bus == b && (NewEntry[NewEntryCount-1].Device>>3) == d){
      continue;
    }

    DEBUG((EFI_D_INFO, "Found PCI Device(%x:%x:%x) : %x | INT pin:%x | SlotNum :%d @LogicalBridgeNum %d\n",b,d,f,PciId,IntPin,SlotNum,LogicalBridgeNum));
    NewEntry[NewEntryCount].Bus    = (UINT8)b;
    NewEntry[NewEntryCount].Device = (UINT8)(d<<3);
    NewEntry[NewEntryCount].Slot   = SlotNum;
    CopyMem(&NewEntry[NewEntryCount].PirqEntry[0], &HygonLogicalId[LogicalBridgeNum].IntMap[0], sizeof(EFI_LEGACY_PIRQ_ENTRY)*4);

    NewEntryCount++;
    DEBUG((EFI_D_INFO, "\n"));
  }

  DEBUG((EFI_D_INFO, "NewEntryCount = %d\n", NewEntryCount));

  CopyMem(&gPirData.Entry[0], &NewEntry[0], sizeof(EFI_LEGACY_IRQ_ROUTING_ENTRY) * NewEntryCount);
  ZeroMem(&gPirData.Entry[NewEntryCount], sizeof(EFI_LEGACY_IRQ_ROUTING_ENTRY));
  gPirData.Entry[NewEntryCount].Bus = 0xFF;

  DEBUG((EFI_D_INFO, "Merged Table!\n"));
  PirqCount = GetIrqRoutingEntry();
  for(Index=0;Index<PirqCount;Index++){
    DEBUG((EFI_D_INFO, "[%02d] (%02X,%02X) {%x,%x},{%x,%x},{%x,%x},{%x,%x} Slot:%x\n", \
              Index, \
              gPirData.Entry[Index].Bus, gPirData.Entry[Index].Device >> 3, \
              gPirData.Entry[Index].PirqEntry[0].Pirq, gPirData.Entry[Index].PirqEntry[0].IrqMask, \
              gPirData.Entry[Index].PirqEntry[1].Pirq, gPirData.Entry[Index].PirqEntry[1].IrqMask, \
              gPirData.Entry[Index].PirqEntry[2].Pirq, gPirData.Entry[Index].PirqEntry[2].IrqMask, \
              gPirData.Entry[Index].PirqEntry[3].Pirq, gPirData.Entry[Index].PirqEntry[3].IrqMask, \
              gPirData.Entry[Index].Slot));

  }
}


EFI_STATUS
LegacyBiosPlatformInstall (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;
  VOID        *Registration;


  gPirData.Hdr.CompatibleVid = MmioRead16(LPC_PCI_REG(PCI_VID_REG));
  gPirData.Hdr.CompatibleDid = MmioRead16(LPC_PCI_REG(PCI_DID_REG));
  gPirData.Hdr.Checksum = CalculateCheckSum8((UINT8*)&gPirData, sizeof(gPirData));
  gAhciAddrSaveArray = AllocateZeroPool(sizeof(AHCI_FIS_CMDLIST_SAVE) * AHCI_ADDR_SAVE_MAX_HOST_COUNT);
  ASSERT(gAhciAddrSaveArray!=NULL);

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiLegacyBiosPlatformProtocolGuid,
                  &gLegacyBiosPlatform,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);

  Status = EfiCreateEventLegacyBootEx (
             TPL_CALLBACK,
             LegacyBootEvent,
             (VOID *)&ImageHandle,
             &Event
             );
  EfiCreateProtocolNotifyEvent (
    &gEfiLegacyBiosProtocolGuid,
    TPL_CALLBACK,
    SetSmmPort,
    NULL,
    &Registration
    );

  gBS->CreateEventEx (
         EVT_NOTIFY_SIGNAL,
         TPL_NOTIFY,
         LegacyBootExitEvent,
         NULL,
         &gEfiEventLegacyBootExitGuid,
         &Event
         );

  return Status;
}
