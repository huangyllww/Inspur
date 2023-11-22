/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2022 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 * 
 ******************************************************************************
 */

#include <HygonCpmPei.h>
#include <Library/DebugLib.h>
#include <Ppi/EqTablePpi.h>
#include <Library/Ppi/HygonCpmTablePpi/HygonCpmTablePpi.h>
#include <Library/HygonSocBaseLib.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include "EqTable.h"
#include <TsFch.h>

static EFI_PEI_NOTIFY_DESCRIPTOR PeiNotifyList[] = {
  { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gHygonCpmTablePpiGuid, EqTableInitPei }
};

STATIC EFI_PEI_EQ_TABLE_PPI mEqTablePpi = {
  EQ_TABLE_PPI_REVISION,
  CoreGetCfgTable,
  CoreDumpEqCfgTable
};

STATIC EFI_PEI_PPI_DESCRIPTOR mEqTablePpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEqTablePpiGuid,
  &mEqTablePpi 
};

VOID
InternalPreprocessEqCfgTable (
  IN     CONST EFI_PEI_SERVICES      **PeiServices,
  IN OUT HYGON_CPM_EQ_CONFIG_TABLE   *EqCfgTable
)
{
  UINT32 i;
  EFI_STATUS Status;
  HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI  *FabricTopologyPpi;
  UINTN                                   NumberOfSockets;
  UINTN                                   TotalNumberOfIoDie;
  UINTN                                   TotalNumberRootBridges;
  UINT8                                   SataPortCount;
  UINT8                                   UsbPortCount;
  UINT32                                  CpuModel;
  
  CpuModel = GetHygonSocModel();
  
  if (CpuModel == HYGON_EX_CPU) {
    SataPortCount = TAISHAN_SATA_PORT_NUM;
    UsbPortCount  = USB_PORT_NUMBER_PER_NBIO_OF_SATORI;
  } else if (CpuModel == HYGON_GX_CPU) {
    SataPortCount = SATA_PORT_NUMBER_PER_DJ;
    UsbPortCount  = USB_PORT_NUMBER_PER_DJ;
  }
  
  

  Status = (*PeiServices)->LocatePpi (
                          (CPM_PEI_SERVICES**)PeiServices,
                          &gHygonFabricTopologyServicesPpiGuid,
                          0,
                          NULL,
                          (VOID**)&FabricTopologyPpi
                          );
  if (!EFI_ERROR(Status)) {
    FabricTopologyPpi->GetSystemInfo (&NumberOfSockets, &TotalNumberOfIoDie, &TotalNumberRootBridges);
    for (i=0; (EqCfgTable->EqCfgList[i].Flag&EQ_TABLE_END) != EQ_TABLE_END; i++) {
      if (EqCfgTable->EqCfgList[i].Flag & FLAG_THROW) {
        continue; 
      }
      if (EqCfgTable->EqCfgList[i].Socket >= NumberOfSockets) {
        EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
        DEBUG((EFI_D_ERROR, "Table Bad: Index=%x, Socket Num(%d) >= Total Socket Num(%d)!!!\n", i, EqCfgTable->EqCfgList[i].Socket, NumberOfSockets));
        continue;  
      }
      if (EqCfgTable->EqCfgList[i].Nbio >= (TotalNumberRootBridges / TotalNumberOfIoDie)) {
        EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
        DEBUG((EFI_D_ERROR, "Table Bad: Index=%x, Nbio Num(%d) >= Total Nbio Num(%d)!!!\n", i, EqCfgTable->EqCfgList[i].Nbio, (TotalNumberRootBridges / TotalNumberOfIoDie)));
        continue;  
      }

      if ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) == PCIE_EQ_TYPE) {
        //check FLAG_FORCE and FLAG_SPECIFIED
        if ((EqCfgTable->EqCfgList[i].Flag & (FLAG_SPECIFIED|FLAG_FORCE)) == 0) {
          EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
          DEBUG((EFI_D_ERROR, "Table Bad: Index=%x, Flag error!!!\n", i));
          continue;
        }

        if (((EqCfgTable->EqCfgList[i].Flag & FLAG_SPECIFIED) != 0) && ((EqCfgTable->EqCfgList[i].Flag&FLAG_FORCE) != 0)) {
          EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
          DEBUG((EFI_D_ERROR, "Table Bad: Index=%x, Flag error!!!\n", i));
          continue;
        }
      
        //check StartLane and EndtLane
        if (EqCfgTable->EqCfgList[i].StartLane > EqCfgTable->EqCfgList[i].EndLane) {
          EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
          DEBUG((EFI_D_ERROR, "Table Bad: Index=%x, StartLane~EndLane Error!!!\n", i));
          continue;
        }
      } else if ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) == USB_EQ_TYPE) {
        if (EqCfgTable->EqCfgList[i].UsbPort >= UsbPortCount) {
          EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
          DEBUG((EFI_D_ERROR, "Table Bad: Index=%x, UsbPort(%d) >= Max UsbPort(%d)!!!\n", i, EqCfgTable->EqCfgList[i].UsbPort, USB_PORT_NUMBER_PER_DJ));
          continue;
        }
      } else if ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) == SATA_EQ_TYPE) {
        if (EqCfgTable->EqCfgList[i].SataPort >= SataPortCount) {
          EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
          DEBUG((EFI_D_ERROR, "Table Bad: Index=%x, SataPort(%d) >= Max SataPort(%d)!!!\n", i, EqCfgTable->EqCfgList[i].UsbPort, SATA_PORT_NUMBER_PER_DJ));
          continue;
        }
      }
    }
  }
}

VOID
CoreDumpEqCfgTable (
    IN CONST EFI_PEI_SERVICES     **PeiServices,
    IN HYGON_CPM_EQ_CONFIG_TABLE  *EqCfgTable,
    IN EQ_CFG_TYPE                Type
)
{
  UINT32    i, j;

  if (!DebugPrintEnabled()) return;

  for (i=0; (EqCfgTable->EqCfgList[i].Flag & EQ_TABLE_END) != EQ_TABLE_END; i++) {
    if ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) != Type)
      continue;
    if ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) == PCIE_EQ_TYPE) {
      DEBUG((EFI_D_ERROR, "PCIE_EQ { Flag(0x%x), ", EqCfgTable->EqCfgList[i].Flag));
      DEBUG((EFI_D_ERROR, "Type(0x%x), ", EqCfgTable->EqCfgList[i].Type));
      DEBUG((EFI_D_ERROR, "Socket(0x%x), ", EqCfgTable->EqCfgList[i].Socket));
      DEBUG((EFI_D_ERROR, "VidDid(0x%x), ", EqCfgTable->EqCfgList[i].VidDid)); 
      DEBUG((EFI_D_ERROR, "SVidDid(0x%x), ", EqCfgTable->EqCfgList[i].SubSysVidDid)); 
      DEBUG((EFI_D_ERROR, "StartLane(0x%x), ", EqCfgTable->EqCfgList[i].StartLane)); 
      DEBUG((EFI_D_ERROR, "EndLane(0x%x), ", EqCfgTable->EqCfgList[i].EndLane)); 
      DEBUG((EFI_D_ERROR, "PerPort(0x%x), ", EqCfgTable->EqCfgList[i].PerPort));
      DEBUG((EFI_D_ERROR, "CfgValue { "));
      for (j=0; j < 16; j++) {
        DEBUG((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].CfgValue[j]));    
      }
      DEBUG((EFI_D_ERROR, "}}\n"));
      continue;
    }
    if ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) == USB_EQ_TYPE) {
      DEBUG((EFI_D_ERROR, "USB_EQ { Flag(0x%x), ", EqCfgTable->EqCfgList[i].Flag));
      DEBUG((EFI_D_ERROR, "Type(0x%x), ", EqCfgTable->EqCfgList[i].Type));
      DEBUG((EFI_D_ERROR, "Socket(0x%x), ", EqCfgTable->EqCfgList[i].Socket));
      DEBUG((EFI_D_ERROR, "PhysicalIodId(0x%x), ", EqCfgTable->EqCfgList[i].PhysicalIodId));  
      DEBUG((EFI_D_ERROR, "Nbio(0x%x), ", EqCfgTable->EqCfgList[i].Nbio));    
      DEBUG((EFI_D_ERROR, "UsbPort(0x%x), ", EqCfgTable->EqCfgList[i].UsbPort));
      DEBUG((EFI_D_ERROR, "CfgValue { ")); 
      DEBUG((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].UsbCfgValue[0]));
      DEBUG((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].UsbCfgValue[1])); 
      DEBUG((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].UsbCfgValue[2]));  
      DEBUG((EFI_D_ERROR, "}}\n"));
      continue;
    }
    if ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) == SATA_EQ_TYPE) {
      DEBUG((EFI_D_ERROR, "SATA_EQ { Flag(0x%x), ", EqCfgTable->EqCfgList[i].Flag));
      DEBUG((EFI_D_ERROR, "Type(0x%x), ", EqCfgTable->EqCfgList[i].Type));
      DEBUG((EFI_D_ERROR, "Socket(0x%x), ", EqCfgTable->EqCfgList[i].Socket));
      DEBUG((EFI_D_ERROR, "PhysicalIodId(0x%x), ", EqCfgTable->EqCfgList[i].PhysicalIodId));  
      DEBUG((EFI_D_ERROR, "Nbio(0x%x), ", EqCfgTable->EqCfgList[i].Nbio));    
      DEBUG((EFI_D_ERROR, "SataPort(0x%x), ", EqCfgTable->EqCfgList[i].UsbPort));
      DEBUG((EFI_D_ERROR, "CfgValue { ")); 
      DEBUG((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].SataCfgValue[0]));
      DEBUG((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].SataCfgValue[1])); 
      DEBUG((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].SataCfgValue[2]));  
      DEBUG((EFI_D_ERROR, "}}\n"));
      continue;
    }
  }
}

EFI_STATUS
CoreGetCfgTable (
    IN  CONST EFI_PEI_SERVICES     **PeiServices,
    OUT HYGON_CPM_EQ_CONFIG_TABLE  **EqCfgTable
)
{
  EFI_STATUS                  Status;
  UINT32                      i;
  HYGON_CPM_EQ_CONFIG_TABLE   *pEqCfgTable;
  HYGON_CPM_TABLE_PPI         *CpmTablePpiPtr;
  UINT32                      CpuModel;

  Status = (*PeiServices)->LocatePpi (
                            (CPM_PEI_SERVICES**)PeiServices,
                            &gHygonCpmTablePpiGuid,
                            0,
                            NULL,
                            (VOID**)&CpmTablePpiPtr
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  pEqCfgTable = NULL;
  CpuModel    = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    pEqCfgTable = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, HYEX_CPM_SIGNATURE_EQ_CONFIG);
  } else if (CpuModel == HYGON_GX_CPU) {
    pEqCfgTable = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, HYGX_CPM_SIGNATURE_EQ_CONFIG);
  }

  if (pEqCfgTable == NULL) {
    return EFI_NOT_FOUND;
  }

  for (i=0; i < MAX_EQ_CFG_ITEM; i++) {
    if ((pEqCfgTable->EqCfgList[i].Flag & EQ_TABLE_END) == EQ_TABLE_END) {
      *EqCfgTable = pEqCfgTable;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
EqTableInitPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  HYGON_CPM_EQ_CONFIG_TABLE  *EqCfgTable;

  Status = CoreGetCfgTable (PeiServices, &EqCfgTable);
  if (!EFI_ERROR(Status)) {
    InternalPreprocessEqCfgTable (PeiServices, EqCfgTable);
    CoreDumpEqCfgTable (PeiServices, EqCfgTable, PCIE_EQ_TYPE);
    CoreDumpEqCfgTable (PeiServices, EqCfgTable, USB_EQ_TYPE);
    CoreDumpEqCfgTable (PeiServices, EqCfgTable, SATA_EQ_TYPE);
  }
  
  // Install EqTable Ppi
  Status = (*PeiServices)->InstallPpi (
                           PeiServices,
                           &mEqTablePpiList
                           );

  return Status;
}

/**
 * Entry point of the EQ Table PEIM driver
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
EqTablePeiEntryPoint (
  IN        CPM_PEI_FILE_HANDLE   FileHandle,
  IN        CPM_PEI_SERVICES      **PeiServices
  )
{
  EFI_STATUS                      Status;

  Status = (*PeiServices)->NotifyPpi(PeiServices, PeiNotifyList);

  return EFI_SUCCESS;
}
