/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 321534 $   @e \$Date: 2015-06-26 15:42:20 +0800 (Fri, 26 Jun 2015) $
 *
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */
#include <PiDxe.h>
#include <Guid/EventGroup.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/SmmControl2.h>
#include <Protocol/PspPlatformProtocol.h>
#include <Protocol/MpService.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciEnumerationComplete.h>
#include <Protocol/AmdPspCommonServiceProtocol.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspBaseLibV1.h>
#include <Library/AmdPspMboxLibV1.h>
#include <Library/AmdS3SaveLib.h>
#include <Library/PciLib.h>
#include <Library/CcxPspLib.h>
#include <Filecode.h>

#define FILECODE PSP_AMDPSPDXEV1_AMDPSPDXEV1_FILECODE

EFI_SMM_CONTROL2_PROTOCOL              *mSmmControl;
PSP_PLATFORM_PROTOCOL                 *mPspPlatfromProtocol;


///@todo move below assert definition to Ids header file
#define ASSERT_EFI_ERROR(x) ASSERT (!EFI_ERROR (x))

#define PSP_SMM_COMMUNICATION_BUFFER_SIZE (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data)  + sizeof (PSP_SMM_COMMUNICATION_BUFFER))

#define SOCKET_AM4   0x2

#define PT_PCIE_ID_A 0x43B91022ul
#define PT_PCIE_ID_B 0x43BA1022ul
#define PT_PCIE_ID_C 0x43BB1022ul
#define PT_PCIE_ID_D 0x43BC1022ul
#define HYGON_PT_ID_A 0x43B91D94ul
#define HYGON_PT_ID_B 0x43BA1D94ul
#define HYGON_PT_ID_C 0x43BB1D94ul
#define HYGON_PT_ID_D 0x43BC1D94ul

#define AMD_SMM_COMMUNICATION_BUFFER_SIZE (4 * 1024)

STATIC AMD_PSP_COMMON_SERVICE_PROTOCOL mPspCommonServiceProtocol = {
  GetFtpmControlAreaV1,
  SwitchPspMmioDecodeV1,
  CheckPspDevicePresentV1,
  CheckFtpmCapsV1,
  PSPEntryInfoV1,
  PspLibTimeOutV1
};

/*++

Routine Description:

  This function is invoked by EFI_EVENT_SIGNAL_LEGACY_BOOT.
  Before booting to legacy OS, inform SMM

Arguments:

  Event   - The triggered event.
  Context - Context for this event.

Returns:

  None

--*/
VOID
PspNotifySmmDrivers (
  VOID
  )
{

  UINT8 Cmd;
  EFI_STATUS Status;
  //Below two protocols haven't been added to driver's depex, assume it should be ready at RTB
  Status = gBS->LocateProtocol (&gEfiSmmControl2ProtocolGuid, NULL, &mSmmControl);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Locate SmmCtrl Protocol Fail\n");
    return;
  }

  //Locate Psp Platform Protocol to get customized information
  IDS_HDT_CONSOLE_PSP_TRACE ("Locate Psp Platform Protocol\n");
  Status = gBS->LocateProtocol (
                    &gPspPlatformProtocolGuid,
                    NULL,
                    &mPspPlatfromProtocol
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Locate Psp Platform Protocol Fail\n");
    return;
  }

  Cmd = mPspPlatfromProtocol->SwSmiCmdtoBuildContext;

  IDS_HDT_CONSOLE_PSP_TRACE ("PspNotifySmmDrivers Cmd[%x]\n", Cmd);
  mSmmControl->Trigger (mSmmControl, &Cmd, NULL, FALSE, 0);
}

VOID
PSPProgBar3Msr (
  IN  VOID                                     *Buffer
  )
{
  UINT32 Bar3Addr;
  UINT64 Tmp64;
  //Get Bar3 Addr
  Bar3Addr = PspLibPciReadPspConfig (PSP_PCI_BAR3_REG);
  Tmp64 = Bar3Addr & 0xFFF00000;

  UpdatePspAddr (Tmp64);
}

BOOLEAN
IsPT (
  IN UINT32  DevId
  )
{
  //Compare PT Ids
  if ((DevId  == PT_PCIE_ID_A) ||
      (DevId  == PT_PCIE_ID_B) ||
      (DevId  == PT_PCIE_ID_C) ||
      (DevId  == PT_PCIE_ID_D) ||
      (DevId  == HYGON_PT_ID_A) ||
      (DevId  == HYGON_PT_ID_B) ||
      (DevId  == HYGON_PT_ID_C) ||
      (DevId  == HYGON_PT_ID_D)) {
    return TRUE;
  }
  return FALSE;
}

VOID
PspDetectPT (
  VOID
  )
{
  PCI_ADDR    PciAddr;
  PCI_ADDR    DevPciAddr;
  UINT32      RegVal;
  UINT32      SecBus;
  UINT32      SubBus;
  BOOLEAN     Detected;

  Detected = FALSE;
  PciAddr.AddressValue = 0;

  DevPciAddr.AddressValue = 0;

  //Scan root port range Dev[3:2]Func[5:1]
  for (PciAddr.Address.Device = 0x2; PciAddr.Address.Device <= 0x3; PciAddr.Address.Device++) {
    for (PciAddr.Address.Function = 0x1; PciAddr.Address.Function <= 0x5; PciAddr.Address.Function++) {
      // Check RootPort
      RegVal = PciRead32 (PciAddr.AddressValue);
      if ((RegVal != 0xFFFFFFFF) && (RegVal != 0x0)) {
        // Read Bus Num
        PciAddr.Address.Register = 0x18;
        RegVal = PciRead32 (PciAddr.AddressValue);
        SecBus = (RegVal & 0xFF00) >> 8;
        SubBus = (RegVal & 0xFF0000) >> 16;
        // Detect the existence of EP, PT's PCIE Bridge is on Func 2
        DevPciAddr.Address.Bus = SecBus;
        DevPciAddr.Address.Function = 0x0;
        RegVal = PciRead32 (DevPciAddr.AddressValue);
        if (IsPT (RegVal) == TRUE) {
          Detected = TRUE;
          //Terminate the loop
          PciAddr.Address.Device = 0x3;
          PciAddr.Address.Function = 0x5;
        }
      }
    }
  }

  if (Detected) {
    PspMboxBiosPcieInfo (PCIE_INFO_DEVID_PROMONTORY, DevPciAddr);
  }
}

VOID
PspPciEnumerationCompleteCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;
  UINT32      PciReg48Data;
  UINT32      PciReg48DataMask;
  UINT32      Bar3Addr;
  VOID        *UnusedBufPtr;
  EFI_MP_SERVICES_PROTOCOL *MpServices;
  PCI_ADDR    PciAddress;
  VOID        *Interface;
  UINT32      PspBar3Size;
  CPUID_DATA  CpuId;
  UINT64      Tmp64;

  AGESA_TESTPOINT (TpPspDxeV1PciCallbackEntry, NULL);
  //
  // Try to locate it because EfiCreateProtocolNotifyEvent will trigger it once when registration.
  // Just return if it is not found.
  //
  Status = gBS->LocateProtocol (
                  &gEfiPciEnumerationCompleteProtocolGuid,
                  NULL,
                  &Interface
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }

  //Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  //Check AM4 socket platform,Send PCIE message to PSP if Promontory detected
  AsmCpuid (
      0x80000001,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  if (((CpuId.EBX_Reg >> 28) & 0xF) == SOCKET_AM4) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Detect Promontory\n");
    PspDetectPT ();
  }

  //Init Bar3 related
  //Ignore BAR3 programming, if MSR already programmed.
  Tmp64 = AsmReadMsr64 (PSP_MSR_PRIVATE_BLOCK_BAR);
  if (Tmp64) {
    return;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Locate MP Protocol\n");
  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
  ASSERT (!EFI_ERROR (Status));

  UnusedBufPtr = NULL;
  PSPProgBar3Msr (UnusedBufPtr);
  Status = MpServices->StartupAllAPs (
                        MpServices,                                    // EFI_MP_SERVICES_PROTOCOL *this
                        (EFI_AP_PROCEDURE)PSPProgBar3Msr,              // EFI_AP_PROCEDURE
                        TRUE,                                          // BOOLEAN SingleThreaded? FALSE=execute all functions at the same time on all CPUs
                        NULL,                                          // EFI_EVENT WaitEvent OPTIONAL
                        0,                                             // UINTN Timeout (Unsupported)
                        NULL,                                          // VOID *ProcArguments OPTIONAL
                        NULL                                           // Failed CPUList OPTIONAL (unsupported)
                        );

  IDS_HDT_CONSOLE_PSP_TRACE ("Locking Bar3\n");
  //Get Bar3 Addr
  Bar3Addr = PspLibPciReadPspConfig (PSP_PCI_BAR3_REG);

  //Get PSP BAR3 Size
  PspLibPciWritePspConfig (PSP_PCI_BAR3_REG, 0xFFFFFFFF);
  PspBar3Size = PspLibPciReadPspConfig (PSP_PCI_BAR3_REG);
  PspBar3Size = ~PspBar3Size + 1;

  //Restore BAR3 Address
  PspLibPciWritePspConfig (PSP_PCI_BAR3_REG, Bar3Addr);

  //Set Bar3 address to memory map.
  Status = gDS->SetMemorySpaceAttributes (Bar3Addr, PspBar3Size, EFI_MEMORY_RUNTIME);

  //Save to S3Script
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, PSP_PCI_DEV, PSP_PCI_FUNC, PSP_PCI_BAR3_REG);
  AmdS3SaveScriptPciWrite (AccessWidth32, PciAddress.AddressValue, &Bar3Addr);

  PciAddress.Address.Register = PSP_PCI_EXTRAPCIHDR_REG;
  PciReg48Data = BIT12 + BIT20;
  PciReg48DataMask = BIT12 + BIT20;
  AmdS3PciRMW (AccessWidth32, PciAddress.AddressValue, &PciReg48Data, &PciReg48DataMask, NULL);
  AGESA_TESTPOINT (TpPspDxeV1PciCallbackExit, NULL);
}

/*++

Routine Description:

  This function is invoked by EFI_EVENT_SIGNAL_LEGACY_BOOT.
  Before booting to legacy OS, inform PSP that BIOS is transitioning
  preparing for S3 usage.

Arguments:

  Event   - The triggered event.
  Context - Context for this event.

Returns:

  None

--*/
VOID
PspDxeReadyToBoot (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  AGESA_TESTPOINT (TpPspDxeV1RTBCallbackEntry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("PSP.ExitBootServicesEvent.Callback\n");

  //Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  PspNotifySmmDrivers ();

  if (PcdGetBool (PcdAmdS3SmmLockClearEnable)) {
    PspMboxBiosClearSmmLock ();
  }

  PspMboxBiosCmdExitBootServices ();
  AGESA_TESTPOINT (TpPspDxeV1RTBCallbackExit, NULL);
}

EFI_STATUS
EFIAPI
AmdPspDxeV1DriverEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_EVENT                   PspPciEnumerationCompleteEvent;
  EFI_EVENT                   ExitBootServicesEvent;
  EFI_STATUS                  Status;
  VOID                        *Registration;
  EFI_HANDLE                  Handle;
  UINT8                       *Buffer;

  AGESA_TESTPOINT (TpPspDxeV1Entry, NULL);
  Handle = NULL;
  //Install PSP Common service Protocol
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdPspCommonServiceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPspCommonServiceProtocol
                  );
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
  }

  //Allocate RT buffer for Smm communication before EndOfDxe Event to satisfie WSMT test
  Buffer = NULL;
  Buffer = AllocateRuntimePool (AMD_SMM_COMMUNICATION_BUFFER_SIZE);
  ASSERT (Buffer != NULL);
  //Save to PCD database
  if (Buffer != NULL) {
    PcdSet64 (PcdAmdSmmCommunicationAddress, (UINT64) (UINTN) Buffer);
  }

  //If PSP feature turn off, exit the driver
  if ((CheckPspDevicePresentV1 () == FALSE) ||
      (PcdGetBool (PcdAmdPspEnable) == FALSE)) {
    return EFI_SUCCESS;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.Drv.PspDxe Enter\n");


  IDS_HDT_CONSOLE_PSP_TRACE ("\tCreate PspInitBar3EventNotify\n");
  PspPciEnumerationCompleteEvent = EfiCreateProtocolNotifyEvent (
                      &gEfiPciEnumerationCompleteProtocolGuid,
                      TPL_NOTIFY,
                      PspPciEnumerationCompleteCallBack,
                      NULL,
                      &Registration
                      );
  ASSERT (PspPciEnumerationCompleteEvent != NULL);

  IDS_HDT_CONSOLE_PSP_TRACE ("\tCreate ExitBootServicesEvent\n");
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  PspDxeReadyToBoot,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &ExitBootServicesEvent
                  );

  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return (Status);
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("PspDxe Exit\n");
  AGESA_TESTPOINT (TpPspDxeV1Exit, NULL);
  return EFI_SUCCESS;
}
