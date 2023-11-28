
#include <Library/DebugLib.h>
#include <NbioRegisterTypes.h>
#include <AMD.h>
#include <GnbDxio.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/AmdBaseLib.h>
#include <Protocol/AmdNbioBaseServicesProtocol.h>
#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Guid/GnbNbioBaseZPInfoHob.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <GnbRegistersZP.h>
#include <Library/NbioRegisterAccLib.h>
#include <SysMiscCfg.h>
#include <AmdRas.h>
#include <Library/PrintLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/HiiLib.h>
#include "SetupItemId.h"
#include <Guid/MdeModuleHii.h>
#include <Library/BaseLib.h>
#include <ByoPlatformSetupConfig.h>
#include <PlatformDefinition.h>
#include <Library/IoLib.h>



typedef struct {
  EFI_HII_HANDLE    HiiHandle;
  VOID              *StartOpCodeHandle;
} PCIE_STATUS_LIST_CALLBACK_CTX;

typedef struct {
  UINT16  VendorId;
  UINT16  DeviceId;
  UINT8   ClassCode[3];
  CHAR16  *ClassName;
} PCI_DEV_INFO;

CHAR16 *GetClassCodeName(UINT8 ClassCode[3]);

VOID GetSubDeviceInfo(GNB_HANDLE *NbioHandle, PCIe_ENGINE_CONFIG *Engine, PCI_DEV_INFO *Info)
{
  UINT32  Data32;
  UINT8   *p = (UINT8*)&Data32;
  UINTN   PciAddr;
  UINT32  PciId;
  UINT32  PciDevBase;

  
  PciDevBase = MAKE_SBDFO(0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, 0);
  NbioRegisterRead (NbioHandle, TYPE_PCI, PciDevBase+0x18, &Data32, 0);
  PciAddr = PCI_DEV_MMBASE(p[1], 0, 0);

  DEBUG((EFI_D_INFO, "BUS %x\n", p[1]));

  Info->ClassCode[0] = MmioRead8(PciAddr + 0x9);
  Info->ClassCode[1] = MmioRead8(PciAddr + 0xA);  
  Info->ClassCode[2] = MmioRead8(PciAddr + 0xB);
  if(Info->ClassCode[2] == 6 && Info->ClassCode[1] == 4 && Info->ClassCode[0] == 0 &&
     p[1] != p[2]){
    PciAddr = PCI_DEV_MMBASE(p[1]+1, 0, 0);
  }
  
  PciId = MmioRead32(PciAddr);
  Info->VendorId = (UINT16)PciId;
  Info->DeviceId = (UINT16)(PciId >> 16);
  Info->ClassCode[0] = MmioRead8(PciAddr + 0x9);
  Info->ClassCode[1] = MmioRead8(PciAddr + 0xA);  
  Info->ClassCode[2] = MmioRead8(PciAddr + 0xB);

  Info->ClassName = GetClassCodeName(Info->ClassCode);
}


VOID
STATIC
PcieStatusListCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  GNB_HANDLE    *NbioHandle;
  UINT32        PcieCapBase;
  UINT32        LinkStatus;
  UINT32        LinkCap;
  UINT32        LinkCap2;
  UINT32        DevCtrl;
  UINT32        LinkCtrl;
  UINTN         CurLinkSpeed;
  UINTN         CurLinkWidth;
  UINTN         SupportLinkSpeedVector;
  UINTN         MaxLinkSpeed;
  UINTN         MaxLinkWidth;
  CHAR8         *LinkSpeedStr;
  CHAR8         *MaxLinkSpeedStr;
  UINTN         MaxPayLoadSize;
  UINTN         MaxReadReqSize;
  UINT8         Aspm;
  CHAR8         *AspmStr[4] = {"Disabled", "L0S", "L1", "L0S+L1"};
  STATIC CHAR16 StrBuffer[256];
  EFI_STRING_ID StrId;
  PCIE_STATUS_LIST_CALLBACK_CTX  *Ctx;
  PCI_DEV_INFO                   PciInfo;


  DEBUG((EFI_D_INFO, "PcieStatusListCallback\n"));

  if (!Engine->Type.Port.PortData.PortPresent){
    return;
  }

  Ctx = (PCIE_STATUS_LIST_CALLBACK_CTX*)Buffer;
   
  NbioHandle = (GNB_HANDLE*)PcieConfigGetParentSilicon(Engine);

  if(!(Engine->InitStatus & INIT_STATUS_PCIE_TRAINING_SUCCESS)){
    UnicodeSPrint(
      StrBuffer,
      sizeof(StrBuffer), 
      L"  PCIE(%d-%d) Slot Empty", 
      Engine->EngineData.StartLane, Engine->EngineData.EndLane
      );
    goto DataReady;
  }

  PcieCapBase = MAKE_SBDFO(0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, 0x58); 
  NbioRegisterRead (NbioHandle, TYPE_PCI, PcieCapBase+0x10, &LinkStatus, 0);
  LinkStatus = LinkStatus >> 16;
  NbioRegisterRead (NbioHandle, TYPE_PCI, PcieCapBase+0x0C, &LinkCap, 0);
  NbioRegisterRead (NbioHandle, TYPE_PCI, PcieCapBase+0x2C, &LinkCap2, 0);
  NbioRegisterRead (NbioHandle, TYPE_PCI, PcieCapBase+0x08, &DevCtrl, 0);
  NbioRegisterRead (NbioHandle, TYPE_PCI, PcieCapBase+0x10, &LinkCtrl, 0);

  GetSubDeviceInfo(NbioHandle, Engine, &PciInfo);

// Supported Link Speeds Vector 
//   Bit 0  2.5 GT/s 
//   Bit 1  5.0 GT/s 
//   Bit 2  8.0 GT/s 
  CurLinkSpeed = LinkStatus & 0xF;
  CurLinkWidth = (LinkStatus >> 4) & 0x3F;
  SupportLinkSpeedVector = (LinkCap2 >> 1) & 0x7F;  
  MaxLinkSpeed = LinkCap & 0xF;
  MaxLinkWidth = (LinkCap >> 4) & 0x3F;
  MaxPayLoadSize = 128 << ((DevCtrl >> 5) & 7);
  MaxReadReqSize = 128 << ((DevCtrl >> 12) & 7);
  Aspm = (UINT8)(LinkCtrl & 0x3);

  LinkSpeedStr    = "?";
  MaxLinkSpeedStr = "?";

  if(CurLinkSpeed >= 1 && CurLinkSpeed <= 7){
    CurLinkSpeed = ((UINTN)1 << (CurLinkSpeed - 1)) & SupportLinkSpeedVector;
    switch(CurLinkSpeed){
      case 1:
        LinkSpeedStr = "2.5GT/s(Gen1)";
        break;
      case 2:
        LinkSpeedStr = "5GT/s(Gen2)";
        break;
      case 4:  
        LinkSpeedStr = "8GT/s(Gen3)";
        break; 
    }
  }
  if(MaxLinkSpeed >= 1 && MaxLinkSpeed <= 7){
    MaxLinkSpeed = ((UINTN)1 << (MaxLinkSpeed - 1)) & SupportLinkSpeedVector;
    switch(MaxLinkSpeed){
      case 1:
        MaxLinkSpeedStr = "Gen1";
        break;
      case 2:
        MaxLinkSpeedStr = "Gen2";
        break;
      case 4:  
        MaxLinkSpeedStr = "Gen3";
        break; 
    }
  }

  UnicodeSPrint(
    StrBuffer,
    sizeof(StrBuffer), 
    L"  PCIE(%d-%d) x%d %a  Dev VID:%04X DID:%04X Class:%s", 
    Engine->EngineData.StartLane, Engine->EngineData.EndLane,
    CurLinkWidth, 
    LinkSpeedStr,
    PciInfo.VendorId, PciInfo.DeviceId, PciInfo.ClassName
    );
    
DataReady:
    StrId = HiiSetString (
              Ctx->HiiHandle,
              0,
              StrBuffer,
              NULL
              );
    HiiCreateTextOpCode (
      Ctx->StartOpCodeHandle,
      StrId,
      STRING_TOKEN(STR_EMPTY),
      STRING_TOKEN(STR_EMPTY)
      );      
}



VOID UpdatePcieStatusList(EFI_HII_HANDLE HiiHandle)
{
  VOID                           *StartOpCodeHandle = NULL;
  VOID                           *EndOpCodeHandle   = NULL;
  EFI_IFR_GUID_LABEL             *StartLabel;
  EFI_IFR_GUID_LABEL             *EndLabel;
  EFI_STATUS                     Status;
  PCIe_PLATFORM_CONFIG           *Pcie;
  EFI_PEI_HOB_POINTERS           GuidHob;  
  PCIE_STATUS_LIST_CALLBACK_CTX  Ctx;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  GuidHob.Raw = GetFirstGuidHob(&gGnbPcieHobInfoGuid);
  if (GuidHob.Raw == NULL) {
    DEBUG((EFI_D_ERROR, "gGnbPcieHobInfoGuid not found\n"));
    return;
  }
  Pcie = (PCIe_PLATFORM_CONFIG*)GET_GUID_HOB_DATA(GuidHob); 

  StartOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (StartOpCodeHandle != NULL);
  EndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = ADV_PCIE_DYNAMIC_LABEL;

  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = ADV_PCIE_DYNAMIC_LABEL_END;

  Ctx.HiiHandle = HiiHandle;
  Ctx.StartOpCodeHandle = StartOpCodeHandle;

  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PcieStatusListCallback,
    &Ctx,
    Pcie
    );

  Status = HiiUpdateForm (
             HiiHandle,
             &gEfiFormsetGuidMain, // Formset GUID
             PCIE_FORM_ID,                   // Form ID
             StartOpCodeHandle,              // Label for where to insert opcodes
             EndOpCodeHandle                 // Replace data
             );
  DEBUG((EFI_D_INFO, "HiiUpdateForm:%r\n", Status));

  if(StartOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(StartOpCodeHandle);
  }
  if(EndOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(EndOpCodeHandle);
  }  
}
