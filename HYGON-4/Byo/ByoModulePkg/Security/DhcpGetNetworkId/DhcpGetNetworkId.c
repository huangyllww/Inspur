/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  DhcpGetNetworkId.h

Abstract:
  DhcpGetNetworkId driver.

Revision History:

**/


#include "DhcpGetNetworkId.h"
#include "DhcpGetNetworldCfg.h"

EFI_DHCP4_PACKET               *gSeed;
DHCP_NETWORK_ID_PRIVATE_DATA   *gPrivate;
UINTN                          mNicNum;
EFI_HII_HANDLE                 mNetworkAuthHiiHandle;
UINT8                          *mLocalNetworkId   = NULL;
UINT8                          mLocalNetworkIdLen = 0x0;
//
//The Dhcp NetworkId get protocol
//
NETWORK_IDENTIFIER_PROTOCOL DhcpGetNetworkIdTemplate = {
  DhcpGetNetworkId
};

/**
  Create a template DHCPv4 packet as a seed.

  @param[out] Seed           Pointer to the seed packet.
  @param[in]  Udp4           Pointer to EFI_UDP4_PROTOCOL.

**/
VOID
CreateSeedDhcp4Packet (
  OUT EFI_DHCP4_PACKET           *Seed,
  IN  EFI_SIMPLE_NETWORK_MODE    *SnpMode
  )
{
  EFI_DHCP4_HEADER           *Header;

  Seed->Size            = sizeof (EFI_DHCP4_PACKET);
  Seed->Length          = sizeof (Seed->Dhcp4);
  Header                = &Seed->Dhcp4.Header;
  ZeroMem (Header, sizeof (EFI_DHCP4_HEADER));
  Header->OpCode        = DHCP4_OPCODE_REQUEST;
  Header->HwType        = SnpMode->IfType;
  Header->HwAddrLen     = (UINT8) SnpMode->HwAddressSize;
  CopyMem (Header->ClientHwAddr, &SnpMode->CurrentAddress, Header->HwAddrLen);

  Seed->Dhcp4.Magik     = DHCP_OPTION_MAGIC;
  Seed->Dhcp4.Option[0] = DHCP4_TAG_EOP;
}


/**
  Destroy the opened instances based on IPv4.

  @param[in]  This              Pointer to the EFI_DRIVER_BINDING_PROTOCOL.
  @param[in]  Private           Pointer to PXEBC_PRIVATE_DATA.

**/
VOID
DestroyIp4Child (
  IN DHCP_NETWORK_ID_PRIVATE_DATA       *Private
  )
{
  ASSERT(Private != NULL);

  if (Private->Dhcp4Child != NULL) {
    //
    // Close Ip4 for background ICMP error message and destroy the instance.
    //
    gBS->CloseProtocol (
           Private->Dhcp4Child,
           &gEfiDhcp4ProtocolGuid,
           Private->DriverBindingHandle,
           Private->Controller
           );

    NetLibDestroyServiceChild (
      Private->Controller,
      Private->DriverBindingHandle,
      &gEfiDhcp4ServiceBindingProtocolGuid,
      Private->Dhcp4Child
      );
  }

  //
  // Restore IP4 Policy if it is changed by DhcpNetworkIdDxe
  //
  if (Private->Ip4Config2 != NULL && Private->Policy != Ip4Config2PolicyMax) {
    Private->Ip4Config2->SetData (
                           Private->Ip4Config2,
                           Ip4Config2DataTypePolicy,
                           sizeof (EFI_IP4_CONFIG2_POLICY),
                           &Private->Policy
                           );
  }

  
  Private->Controller = NULL;
  Private->Dhcp4Child = NULL;

  Private->Ip4Config2 = NULL;
  Private->Dhcp4 = NULL;
  Private->Snp = NULL;

  ZeroMem(&Private->Dhcp4Mode, sizeof(EFI_DHCP4_MODE_DATA));
  Private->Policy = 0;

  ZeroMem(&Private->StationIp, sizeof(EFI_IP_ADDRESS));     // Client IP
  ZeroMem(&Private->SubnetMask, sizeof(EFI_IP_ADDRESS));    // Client IP Subnet Mask
  ZeroMem(&Private->GatewayIp, sizeof(EFI_IP_ADDRESS));
  ZeroMem(&Private->ServerIp, sizeof(EFI_IP_ADDRESS));      // DHCP Server IP

  ZeroMem(&Private->SeedPacket, sizeof(EFI_DHCP4_PACKET));
}



/**
  Create the opened instances based on IPv4.

  @param[in]  This              Pointer to EFI_DRIVER_BINDING_PROTOCOL.
  @param[in]  ControllerHandle  Handle of the child to destroy.
  @param[in]  Private Handle    Pointer to PXEBC_PRIVATE_DATA.

  @retval EFI_SUCCESS           The instances based on IPv4 were all created successfully.
  @retval Others                An unexpected error occurred.

**/
EFI_STATUS
CreateIp4Child (
  IN DHCP_NETWORK_ID_PRIVATE_DATA       *Private
  )
{
  EFI_STATUS                    Status;
  EFI_IP4_CONFIG2_POLICY        Policy;
  UINTN                         DataSize;
 
   //
  // 3. Get SNP protocol
  //
  Status = gBS->HandleProtocol (
                  Private->Controller, 
                  &gEfiSimpleNetworkProtocolGuid, 
                  (VOID **) &Private->Snp
                  );
  if (EFI_ERROR (Status)) {
    goto FUNC_EXIT;
  }

  //
  // If media are not connected to network; skip it 
  //  
  if (Private->Snp->Mode->MediaPresentSupported
     && !Private->Snp->Mode->MediaPresent) {
    Status = EFI_INVALID_PARAMETER;
    goto FUNC_EXIT;
  }

  //
  // 1. Force IP Policy to Static
  //
  //
  // DHCP4 service allows only one of its children to be configured in
  // the active state, If the DHCP4 D.O.R.A started by IP4 auto
  // configuration and has not been completed, the Dhcp4 state machine
  // will not be in the right state for the DHCPNetworkIdDXE to start a new round D.O.R.A.
  // to get NetworkId from DHCP server. So here we need to switch its policy to static.
  //
  Status = gBS->HandleProtocol (
                  Private->Controller, 
                  &gEfiIp4Config2ProtocolGuid, 
                  (VOID **) &Private->Ip4Config2
                  );
  if (EFI_ERROR (Status)) {
    goto FUNC_EXIT;
  }

  DataSize = sizeof (EFI_IP4_CONFIG2_POLICY);
  Status = Private->Ip4Config2->GetData (
                         Private->Ip4Config2,
                         Ip4Config2DataTypePolicy,
                         &DataSize,
                         &Policy
                         );
  if (Status == EFI_SUCCESS && Policy != Ip4Config2PolicyStatic) {
    Private->Policy = Policy;
    Policy = Ip4Config2PolicyStatic;
    Private->Ip4Config2->SetData (
                           Private->Ip4Config2,
                           Ip4Config2DataTypePolicy,
                           sizeof (EFI_IP4_CONFIG2_POLICY),
                           &Policy
                           );
  } else {
    Private->Policy = Ip4Config2PolicyMax;
  }

  //
  // 2. Create Dhcp4 child and open Dhcp4 protocol for PxeBc->Dhcp.
  //
  Status = NetLibCreateServiceChild (
             Private->Controller,
             Private->DriverBindingHandle,
             &gEfiDhcp4ServiceBindingProtocolGuid,
             &Private->Dhcp4Child
             );
  if (EFI_ERROR (Status)) {
    goto FUNC_EXIT;
  }

  Status = gBS->OpenProtocol (
                  Private->Dhcp4Child,
                  &gEfiDhcp4ProtocolGuid,
                  (VOID **) &Private->Dhcp4,
                  Private->DriverBindingHandle,
                  Private->Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    goto FUNC_EXIT;
  }

FUNC_EXIT:

  if (EFI_ERROR(Status)) {
    DestroyIp4Child(Private);
  }

  return Status;
}


VOID
EFIAPI
GetNetworkId (
  VOID
  )
{
  EFI_STATUS      Status;
  UINTN           Index;
  EFI_HANDLE      *NicHandleBuffer;

  DEBUG((DEBUG_INFO, "=====%a enter=====\n", __FUNCTION__));
  
  //
  // Find NIC device by locating each dhcp Service Binding Protocol 
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDhcp4ServiceBindingProtocolGuid,
                  NULL,
                  &mNicNum,
                  &NicHandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Fail to find Dhcp4ServiceBinding\n"));
    return;
  }
  
  gPrivate = AllocateZeroPool(sizeof(DHCP_NETWORK_ID_PRIVATE_DATA) * mNicNum);
  if (gPrivate == NULL) {
    return;
  }

  DEBUG((DEBUG_INFO, "Create Private data for NIC\n"));

  //
  // First Step, initialize information in this platform
  //
  for (Index = 0; Index < mNicNum; Index++) {
    //
    // Init Private data for each NIC
    //
    gPrivate[Index].Controller = NicHandleBuffer[Index];
    gPrivate[Index].DriverBindingHandle = gImageHandle;
  
    Status = CreateIp4Child(&gPrivate[Index]);

    DEBUG((DEBUG_INFO, "Create IP4 Child for NIC controller[%d] Status %x\n", Index, Status));
  }

  FreePool(NicHandleBuffer);

  DEBUG((DEBUG_INFO, "Send DHCPREQUEST from NIC NIC\n"));

  //
  // Next Step, try each NIC to send DHCPREQUEST to inquery for NetworkId 
  //
  for (Index = 0; Index < mNicNum; Index++) {

    if (gPrivate[Index].Controller != NULL) {

    
      //
      // Config DHCP Protocol
      //
      Status = Dhcp4Config (&gPrivate[Index]);
      if (EFI_ERROR(Status)){
        DEBUG((DEBUG_INFO, "DHCP4 D.O.R.A failed on controller[%d] Status %x\n", Index, Status));
        continue;
      }
      DEBUG((DEBUG_INFO, "DHCP4 D.O.R.A done on controller[%d]\n", Index));

      //
      // Create DCHP4 Seed Packet for NetworkId
      //
      CreateSeedDhcp4Packet(&gPrivate[Index].SeedPacket, gPrivate[Index].Snp->Mode);
  
      //
      // Send DHCPREQUEST for NetworkId & receive DHCPACK
      //
      Status = Dhcp4NetworkIdSendRecv (&gPrivate[Index]);
      DEBUG((DEBUG_INFO, "Dhcp4NetworkIdSendRecv done. Status %x\n", Status));

    }
  }


  //
  // Final step: clean up context For each NIC
  //
  for (Index = 0; Index < mNicNum; Index++) {
    if (gPrivate[Index].Controller != NULL) {
      DestroyIp4Child(&gPrivate[Index]);
    }
  }

  DEBUG((DEBUG_INFO, "=====%a exit=====\n", __FUNCTION__));

  return;
}

/**
 * @brief    Get Network Identifier for Generating Derived Key
 * 
 * @param   NetworkId[in ]               A pointer to the destination Server NetworkId got from dhcp service.
 * @param   NetworkIdLen[in]             A pointer to the destination Server NetworkId Length got from dhcp service.
 * @param   LocalNetworkId[out ]         A pointer to the destination LocalNetworkId.
 * @param   LocalNetworkId[out]          A pointer to the destination Local NetworkId Length.
 * @return  EFI_STATUS                   Get Local NetworkId Success
 */
EFI_STATUS
GetLocalNetworkId (
  IN UINT8     *ServerNetworkId,
  IN UINT8     *ServerNetworkIdLen,
  IN UINT8     *LocalNetworkId,
  IN UINT8     *LocalNetworkIdLen
)
{

  //
  //If Local NetworkId is already got and saved,Return the Local NetworkId that already exists.
  //
  if (mLocalNetworkIdLen != 0x0) {
    *LocalNetworkIdLen = mLocalNetworkIdLen;
    CopyMem(LocalNetworkId, mLocalNetworkId, *LocalNetworkIdLen);

    DEBUG((EFI_D_INFO, "Get the local NetworkID, NetworkIdLen:0x%x\n", mLocalNetworkIdLen));
    DumpMem8(mLocalNetworkId, mLocalNetworkIdLen);

    return EFI_SUCCESS;
  }

  if (ServerNetworkId == NULL || ServerNetworkIdLen == NULL) {
    DEBUG((EFI_D_INFO, "No valid NetworkId\n"));
    return EFI_INVALID_PARAMETER;
  }

  mLocalNetworkId = AllocateZeroPool(NETWORK_ID_MAX_SIZE);
  if (mLocalNetworkId == NULL) {
    DEBUG((DEBUG_INFO, "Allocate LocalNetworkId buffer failed!\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  mLocalNetworkIdLen = (UINT8)NETWORK_ID_MAX_SIZE;

  //
  //Save NetworkId which get from Dhcp Service to LocalNetworkId,It is always valid during this startup.
  //
  mLocalNetworkIdLen = *ServerNetworkIdLen;
  CopyMem(mLocalNetworkId, ServerNetworkId, mLocalNetworkIdLen);

  //
  //return the Local NetworkId for users.
  //
  *LocalNetworkIdLen = mLocalNetworkIdLen;
  CopyMem(LocalNetworkId, mLocalNetworkId, *LocalNetworkIdLen);

  DEBUG((EFI_D_INFO, "Get the local NetworkID, NetworkIdLen:0x%x\n", *LocalNetworkIdLen));
  DumpMem8(LocalNetworkId, *LocalNetworkIdLen);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
DhcpGetNetworkId(
  OUT UINT8     *NetworkId,
  IN OUT UINT8  *NetworkIdLen
)
{
  EFI_STATUS                     Status;
  DHCP_NETWORK_ID_PRIVATE_DATA   *Private;
  UINT8                          *ServerNetworkId = NULL;
  UINTN                          NicIndex, IdIndex;
  UINT8                          PxeRomPolicy;

  //
  //Get network OptionRom Launch Policy
  //If UEFI network optionrom is not loaded, then need to cache the local networkID
  //
  PxeRomPolicy = PcdGet8(PcdPxeOpRomLaunchPolicy);
  if (PxeRomPolicy != ROM_POLICY_UEFI_FIRST) {
    Status = GetLocalNetworkId(&ServerNetworkId[1], &ServerNetworkId[0], NetworkId, NetworkIdLen);
    if (Status == EFI_SUCCESS) {
      return Status;
    }
  }

  //
  // Always get DHCP NetworkId to ensure TOCTOU. 
  // Between 2 Password verifications, machine could be moved from one network to another network
  //
  //
  GetNetworkId();

  //
  // No Network Controller correctly initialized
  //
  if (gPrivate == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Current Policy: 
  //    Always return first-found NetworId. Search with all Network Controller
  //
  for (NicIndex = 0; NicIndex < mNicNum; NicIndex++) {
    Private = &gPrivate[NicIndex];
    for (IdIndex = 0; IdIndex < Private->NetworkIdNum; IdIndex ++) {
      if (Private->ServerNetworkId[IdIndex] != NULL) {
        ServerNetworkId = Private->ServerNetworkId[IdIndex];
        break;
      }
    }
  }

  if (ServerNetworkId == NULL) {
    Status = EFI_NOT_FOUND;
    goto _EXIT;
  }


  if (*NetworkIdLen < ServerNetworkId[0]) {
    *NetworkIdLen = ServerNetworkId[0];
    Status = EFI_BUFFER_TOO_SMALL;
    goto _EXIT;
  }

  if (PxeRomPolicy != ROM_POLICY_UEFI_FIRST) {
    Status = GetLocalNetworkId(&ServerNetworkId[1], &ServerNetworkId[0], NetworkId, NetworkIdLen);
  } else {
    *NetworkIdLen = ServerNetworkId[0];
    CopyMem(NetworkId, &ServerNetworkId[1] ,*NetworkIdLen);
    Status = EFI_SUCCESS;
  }

_EXIT:
  if (gPrivate != NULL) {
    FreePool(gPrivate);
    gPrivate = NULL;
    mNicNum  = 0x0;
  }

  return Status;
}

HII_VENDOR_DEVICE_PATH gHdpNetworkIdHiiVendorDevicePath = {
  {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        {
          (UINT8) (sizeof (HII_VENDOR_DEVICE_PATH_NODE)),
          (UINT8) ((sizeof (HII_VENDOR_DEVICE_PATH_NODE)) >> 8)
        }
      },
      NETWORK_SECURITY_AUTH_GUID,
    },
    0,
    (UINTN)&gHdpNetworkIdHiiVendorDevicePath
  },
  
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    { 
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }  
};

EFI_STATUS
EFIAPI
FormExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;
  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
FormRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;

  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
FormCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  CHAR16                          *TitleStr = NULL;
  CHAR16                          *ConfirmationStr = NULL;
  SELECTION_TYPE                  Choice;
  PASSWORD_NETWORK_AUTH_CONFIG    PwdNetworkAuthInfo;

  if (Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
    return EFI_SUCCESS;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGING) {
    return EFI_UNSUPPORTED;
  }

  if (QuestionId != PWD_NETWORK_AUTH_ENABLE) {
    return EFI_UNSUPPORTED;
  }

  HiiGetBrowserData (
    &gByoPasswordNetworkAuthConfigGuid, 
    PASSWORD_NETWORK_AUTH_VAR_NAME, 
    sizeof(PASSWORD_NETWORK_AUTH_CONFIG), 
    (UINT8 *) &PwdNetworkAuthInfo
    );

  if (Value->u8 == PwdNetworkAuthInfo.PwdNetworkAuthEnable) {
    return EFI_SUCCESS;
  }

  TitleStr        = HiiGetString(mNetworkAuthHiiHandle, STRING_TOKEN(STR_NETWORK_AUTH_PROMPT), NULL);
  ConfirmationStr = HiiGetString(mNetworkAuthHiiHandle, STRING_TOKEN(STR_PWD_NETWORK_AUTH_OPTION_CONFIRMATION), NULL);
  Choice = UiConfirmDialog(DIALOG_YESNO, TitleStr, NULL, TEXT_ALIGIN_CENTER, ConfirmationStr, NULL);
  if (Choice != SELECTION_YES) {
    Value->u8 = PwdNetworkAuthInfo.PwdNetworkAuthEnable;
  }

  FreePool(TitleStr);
  FreePool(ConfirmationStr );

  return EFI_SUCCESS;
}

/**
  @param[in]  Event     Event whose notification function is being invoked
  @param[in]  Context   Pointer to the notification function's context

**/
VOID
EFIAPI
DhcpOnReadyToBoot (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  ) 
{
  SETUP_ITEM_UPDATE_PROTOCOL               *UpdateProtocol;
  EFI_STATUS                               Status;
  UINTN                                    VariableSize;
  PASSWORD_NETWORK_AUTH_CONFIG             PwdNetworkAuthInfo;

  DEBUG ((DEBUG_INFO, "%a\n", __FUNCTION__));

  gBS->CloseEvent(Event);
  VariableSize = sizeof(PASSWORD_NETWORK_AUTH_CONFIG);
  Status = gRT->GetVariable (
                  PASSWORD_NETWORK_AUTH_VAR_NAME,
                  &gByoPasswordNetworkAuthConfigGuid,
                  NULL,
                  &VariableSize,
                  &PwdNetworkAuthInfo
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->LocateProtocol(&gSetupItemUpdateProtocolGuid, NULL, (VOID **)&UpdateProtocol);
    if (!EFI_ERROR(Status)) {
      UpdateProtocol->UpdateDefaultValue (PASSWORD_NETWORK_AUTH_VAR_NAME, &gByoPasswordNetworkAuthConfigGuid, 0, (UINT64)PwdNetworkAuthInfo.PwdNetworkAuthEnable);
    }
  }
}
EFI_STATUS
EFIAPI
DhcpGetNetworkIdEntryPoint (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                       Status;
  PASSWORD_NETWORK_AUTH_CONFIG     PwdNetworkAuthInfo;
  UINTN                            VariableSize;
  EFI_HANDLE                       DeviceHandle;
  static EFI_HII_CONFIG_ACCESS_PROTOCOL   ConfigAccess;
  EFI_EVENT                               DhcpReadyToBootEvent = NULL;

  DEBUG((DEBUG_INFO,"Enter %a \n", __FUNCTION__));
  //
  // Install Device Path Protocol to driver handle
  //
  ConfigAccess.ExtractConfig = FormExtractConfig;
  ConfigAccess.RouteConfig   = FormRouteConfig;
  ConfigAccess.Callback      = FormCallback;

  DeviceHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &DeviceHandle,
                  &gEfiDevicePathProtocolGuid, &gHdpNetworkIdHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid, &ConfigAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);

  //
  // Install Dhcp GetNetworkId configuration package to HII database
  //
  mNetworkAuthHiiHandle = HiiAddPackages (
                              &gByoPasswordNetworkAuthConfigGuid,
                              DeviceHandle,
                              STRING_ARRAY_NAME,
                              DhcpGetNetworkIdCfgBin,
                              NULL
                              );
  ASSERT (mNetworkAuthHiiHandle != NULL);

  //
  //Get Dhcp GetNetworkId configuration, if HddPassword Network Authentication status is Disable,
  //NetworkId Protocol should not be installed.
  //
  VariableSize = sizeof(PASSWORD_NETWORK_AUTH_CONFIG);
  Status = gRT->GetVariable (
                  PASSWORD_NETWORK_AUTH_VAR_NAME,
                  &gByoPasswordNetworkAuthConfigGuid,
                  NULL,
                  &VariableSize,
                  &PwdNetworkAuthInfo
                  );

  DEBUG((DEBUG_INFO,"%a Status:%r HdpNetworkAuthEnable:%x", __FUNCTION__, Status, PwdNetworkAuthInfo.PwdNetworkAuthEnable));

  if (Status == EFI_SUCCESS && (PwdNetworkAuthInfo.PwdNetworkAuthEnable == 0x1)) {
    //
    // Install NetworkId Protocol
    //
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &ImageHandle,
                    &gByoNetworkIdentifierProtocolGuid, 
                    &DhcpGetNetworkIdTemplate,
                    NULL
                    );
  }

  Status = gBS->CreateEventEx (
      EVT_NOTIFY_SIGNAL,
      TPL_NOTIFY,
      DhcpOnReadyToBoot,
      NULL,
      &gEfiEventReadyToBootGuid,
      &DhcpReadyToBootEvent
      );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR [DE]: Failed to register DhcpOnReadyToBoot.%r\n", Status));
  }

  return EFI_SUCCESS;
}
