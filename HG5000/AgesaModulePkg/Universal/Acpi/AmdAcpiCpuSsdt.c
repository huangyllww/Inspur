/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate ACPI Processor SSDT.
 *
 * Contains code that generate ACPI Processor SSDT
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Universal
 * @e \$Revision$   @e \$Date$
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
#include "AGESA.h"
#include "Filecode.h"
#include "AmdAcpiDxe.h"
#include "AmdAcpiCpuSsdt.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdHeapLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/BaseLib.h"
#include "Library/AmdIdsHookLib.h"
#include "Protocol/AmdAcpiCpuSsdtServicesProtocol.h"
#include "Protocol/AmdCoreTopologyProtocol.h"
#include "PiDxe.h"

#define FILECODE UNIVERSAL_ACPI_AMDACPICPUSSDT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
STATIC ACPI_TABLE_HEADER  ROMDATA CpuSsdtHdrStruct =
{
  {'S','S','D','T'},
  0,
  1,
  0,
  {0},
  {0},
  1,
  {'H','Y','G','N'},
  1
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

UINT32
AmdAcpiCalSsdtSize (
  IN       AMD_PSTATE_SYS_INFO     *AmdPstateSysInfo,
  IN       AMD_CSTATE_INFO         *AmdCstateInfo,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );

UINT32
AmdAcpiCalCstObj (
  IN       AMD_CSTATE_INFO         *AmdCstateInfo,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );

UINT32
AmdAcpiCalPstObj (
  IN       AMD_PSTATE_SOCKET_INFO  *AmdPstateSocketInfo,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );

UINT32
AmdCreateCStateAcpiTables (
  IN       AMD_CSTATE_INFO         *AmdCstateInfo,
  IN OUT   VOID                   **SsdtPtr,
  IN       UINT8                    LocalApicId,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );

UINT32
AmdCreatePStateAcpiTables (
  IN       AMD_PSTATE_SOCKET_INFO  *AmdPstateSocketInfo,
  IN OUT   VOID                   **SsdtPtr,
  IN       AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL *CpuSsdtServices,
  IN       UINT8                    LocalApicId,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/**
 *---------------------------------------------------------------------------------------
 *
 *  GenerateSsdt
 *
 *  Description:
 *    This function will populate the SSDT with ACPI P-States and C-States Objects, whenever
 *    necessary
 *    This function should be called only from BSP
 *
 *  Parameters:
 *    @param[in]       AcpiTableProtocol          Pointer to gEfiAcpiTableProtocolGuid protocol
 *    @param[in]       StdHeader                  Header for library and services
 *
 *    @retval          AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdAcpiCpuSSdt (
  IN       EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8                  LocalApicId;
  UINT8                  SocketLoop;
  UINT8                  TotalSockets;
  UINT8                  CoreNumberHi;
  UINT8                  CoreNumberLo;
  UINT32                 CoreLoop;
  UINT32                 LogicalCoreNumberInThisSocket;
  UINT32                 CurrSize;
  UINT32                 SizeTemp;
  UINT32                 ScopeSize;
  UINT32                 CoreCount;
  UINTN                  TableSize;
  UINTN                  TableKey;
  ALLOCATE_HEAP_PARAMS   AllocateHeapParams;
  AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL *AmdAcpiCpuSsdtServices;
  AMD_PSTATE_SYS_INFO    *AmdPstateSysInfo;
  AMD_PSTATE_SOCKET_INFO *AmdPstateSocketInfo;
  AMD_CSTATE_INFO        *AmdCstateInfo;
  ACPI_TABLE_HEADER      *SsdtHeaderPtr;
  SCOPE                  *AmdCpuSsdtScopeStruct;
  UINT8                  *AmdCpuSsdtBodyStruct;
  EFI_STATUS             CalledStatus;

  IDS_HDT_CONSOLE (MAIN_FLOW, "  Start to create Processor SSDT\n");

  // Initialize data variables
  ScopeSize   = 0;
  CoreCount   = 0;
  LocalApicId = 0;

  // Get all the CPUs P-States information
  CalledStatus = gBS->LocateProtocol (&gAmdAcpiCpuSsdtServicesProtocolGuid, NULL, (VOID **)&AmdAcpiCpuSsdtServices);
  if (EFI_ERROR (CalledStatus)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  gAmdAcpiCpuSsdtServicesProtocolGuid not found\n");
    return EFI_PROTOCOL_ERROR;
  }

  AmdAcpiCpuSsdtServices->GetPstateInfo (AmdAcpiCpuSsdtServices, &AmdPstateSysInfo);
  AmdAcpiCpuSsdtServices->GetCstateInfo (AmdAcpiCpuSsdtServices, &AmdCstateInfo);

  // Allocate rough buffer for AcpiTable
  // Do not know the actual size.. pre-calculate it.
  AllocateHeapParams.RequestedBufferSize = AmdAcpiCalSsdtSize (AmdPstateSysInfo, AmdCstateInfo, StdHeader);
  AllocateHeapParams.BufferHandle        = AMD_PSTATE_ACPI_BUFFER_HANDLE;
  AllocateHeapParams.Persist             = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer (&AllocateHeapParams, StdHeader) != AGESA_SUCCESS) {
    return AGESA_ERROR;
  }
  SsdtHeaderPtr = (ACPI_TABLE_HEADER *) AllocateHeapParams.BufferPtr;

  // Copy SSDT header into allocated buffer
  LibAmdMemCopy (SsdtHeaderPtr, (VOID *) &CpuSsdtHdrStruct, (UINTN) (sizeof (ACPI_TABLE_HEADER)), StdHeader);

  // Update table OEM fields.
  ASSERT (AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiTableHeaderOemId), 100) <= 6);
  ASSERT (AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiCpuSsdtTableHeaderOemTableId), 100) <= 8);

  LibAmdMemCopy ((VOID *) SsdtHeaderPtr->OemId,
                 (VOID *) PcdGetPtr (PcdAmdAcpiTableHeaderOemId),
                 AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiTableHeaderOemId), 6),
                 StdHeader);
  LibAmdMemCopy ((VOID *) SsdtHeaderPtr->OemTableId,
                 (VOID *) PcdGetPtr (PcdAmdAcpiCpuSsdtTableHeaderOemTableId),
                 AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiCpuSsdtTableHeaderOemTableId), 8),
                 StdHeader);

  // Initialize ScopeStruct pointer
  AmdCpuSsdtScopeStruct = (SCOPE *) (SsdtHeaderPtr + 1);

  // Generate name scope and ACPI objects for every core in the system
  AmdPstateSocketInfo   = AmdPstateSysInfo->PStateSocketStruct;
  TotalSockets          = AmdPstateSysInfo->TotalSocketInSystem;
  for (SocketLoop = 0; SocketLoop < TotalSockets; SocketLoop++) {
    LogicalCoreNumberInThisSocket = AmdPstateSocketInfo->TotalLogicalCores;
    for (CoreLoop = 0; CoreLoop < LogicalCoreNumberInThisSocket; CoreLoop++) {
      CurrSize = 0;
      CoreCount++;
      // Set Name Scope for CPU0, 1, 2, ..... n
      // CPU0 to CPUn will name  as  C000 to Cnnn
      // -----------------------------------------
      AmdCpuSsdtScopeStruct->ScopeOpcode        = SCOPE_OPCODE;
      AmdCpuSsdtScopeStruct->ScopeValue1        = SCOPE_VALUE1;
      AmdCpuSsdtScopeStruct->ScopeValue2        = SCOPE_VALUE2;
      AmdCpuSsdtScopeStruct->ScopeNamePt1a__    = SCOPE_NAME__;
      if (PcdGetBool (PcdAmdAcpiCpuSsdtProcessorScopeInSb)) {
        AmdCpuSsdtScopeStruct->ScopeNamePt1a_P  = SCOPE_NAME_S;
        AmdCpuSsdtScopeStruct->ScopeNamePt1a_R  = SCOPE_NAME_B;
      } else {
        AmdCpuSsdtScopeStruct->ScopeNamePt1a_P  = SCOPE_NAME_P;
        AmdCpuSsdtScopeStruct->ScopeNamePt1a_R  = SCOPE_NAME_R;
      }
      AmdCpuSsdtScopeStruct->ScopeNamePt1b__    = SCOPE_NAME__;

      AmdCpuSsdtScopeStruct->ScopeNamePt2a_C    = PcdGet8 (PcdAmdAcpiCpuSsdtProcessorScopeName0);
      AmdCpuSsdtScopeStruct->ScopeNamePt2a_P    = PcdGet8 (PcdAmdAcpiCpuSsdtProcessorScopeName1);

      CoreNumberHi = ((CoreCount - 1) >> 4) & 0x0F;
      AmdCpuSsdtScopeStruct->ScopeNamePt2a_U    = (UINT8) (SCOPE_NAME_0 + CoreNumberHi);

      CoreNumberLo = (CoreCount - 1) & 0x0F;
      if (CoreNumberLo < 0xA) {
        AmdCpuSsdtScopeStruct->ScopeNamePt2a_0  = (UINT8) (SCOPE_NAME_0 + CoreNumberLo);
      } else {
        AmdCpuSsdtScopeStruct->ScopeNamePt2a_0  = (UINT8) (SCOPE_NAME_A + CoreNumberLo - 0xA);
      }

      // Increment and typecast the pointer
      AmdCpuSsdtBodyStruct = (UINT8 *) (AmdCpuSsdtScopeStruct + 1);

      // Get the Local Apic Id for each core
      LocalApicId = AmdPstateSocketInfo->LocalApicId[CoreLoop];

      // Create P-State ACPI Objects
      IDS_SKIP_HOOK (IDS_HOOK_CCX_SKIP_ACPI_PSTATE_OBJ, NULL, NULL) {
        if (AmdPstateSocketInfo->CreateAcpiTables) {
          CurrSize += (AmdCreatePStateAcpiTables (AmdPstateSocketInfo, (VOID *) &AmdCpuSsdtBodyStruct, AmdAcpiCpuSsdtServices, (UINT8) (CoreCount - 1), StdHeader));
        }
      }

      // Create C-State ACPI Objects
      if (AmdCstateInfo->IsCstateEnabled) {
        CurrSize += (AmdCreateCStateAcpiTables (AmdCstateInfo, (VOID *) &AmdCpuSsdtBodyStruct, (UINT8) (CoreCount - 1), StdHeader));
      }

      // Now update the SCOPE Length field
      CurrSize += (SCOPE_STRUCT_SIZE - 1);
      ScopeSize += CurrSize;

      SizeTemp = ((CurrSize << 4) & 0x0000FF00);
      SizeTemp |= ((CurrSize & 0x0000000F) | 0x00000040);
      AmdCpuSsdtScopeStruct->ScopeLength = (UINT16) SizeTemp;

      AmdCpuSsdtScopeStruct = (SCOPE *) AmdCpuSsdtBodyStruct;
    }
    AmdPstateSocketInfo = (AMD_PSTATE_SOCKET_INFO *) ((UINT8 *) AmdPstateSocketInfo + sizeof (AMD_PSTATE_SOCKET_INFO) + sizeof (S_PSTATE_VALUES) * AmdPstateSocketInfo->PStateMaxValue);
  }

  // Update SSDT header Checksum
  SsdtHeaderPtr->TableLength = (ScopeSize + CoreCount + sizeof (ACPI_TABLE_HEADER));
  ChecksumAcpiTable (SsdtHeaderPtr, StdHeader);

  // Publish SSDT
  TableSize = SsdtHeaderPtr->TableLength;
  TableKey = 0;
  AcpiTableProtocol->InstallAcpiTable (
                  AcpiTableProtocol,
                  SsdtHeaderPtr,
                  TableSize,
                  &TableKey
                  );

  // Deallocate heap
  HeapDeallocateBuffer (AMD_PSTATE_ACPI_BUFFER_HANDLE, StdHeader);

  IDS_HDT_CONSOLE (MAIN_FLOW, "  Processor SSDT is created\n");

  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

 /**
 *---------------------------------------------------------------------------------------
 *
 *  AmdAcpiCalSsdtSize
 *
 *  Description:
 *    This function will calculate the size of processor SSDT tables
 *
 *  Parameters:
 *    @param[in]     *AmdPstateSysInfo        Pstate system information
 *    @param[in]     *AmdCstateInfo           Cstate information
 *    @param[in]     *StdHeader               Stand header
 *
 *    @retval        UINT32
 *
 *---------------------------------------------------------------------------------------
 */
UINT32
AmdAcpiCalSsdtSize (
  IN       AMD_PSTATE_SYS_INFO     *AmdPstateSysInfo,
  IN       AMD_CSTATE_INFO         *AmdCstateInfo,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  UINT8                     SocketLoop;
  UINT32                    CoreLoop;
  UINT32                    ScopeSize;
  UINT32                    CstateAcpiObjSize;
  UINT32                    PstateAcpiObjSize;
  AMD_PSTATE_SOCKET_INFO   *AmdPstateSocketInfo;

  ScopeSize           = sizeof (ACPI_TABLE_HEADER);
  if (AmdCstateInfo->IsCstateEnabled) {
    CstateAcpiObjSize = AmdAcpiCalCstObj (AmdCstateInfo, StdHeader); // Size of Cstate Acpi objects are the same for every core
  } else {
    CstateAcpiObjSize = 0;
  }
  AmdPstateSocketInfo = AmdPstateSysInfo->PStateSocketStruct;

  for (SocketLoop = 0; SocketLoop < AmdPstateSysInfo->TotalSocketInSystem; SocketLoop++) {
    for (CoreLoop = 0; CoreLoop < AmdPstateSocketInfo->TotalLogicalCores; CoreLoop++) {
      ScopeSize += (SCOPE_STRUCT_SIZE - 1); // Scope size per core
      ScopeSize += CstateAcpiObjSize;       // C-State ACPI objects size per core

      // Add P-State ACPI Objects size per core
      if (AmdPstateSocketInfo->CreateAcpiTables) {
        PstateAcpiObjSize = AmdAcpiCalPstObj (AmdPstateSocketInfo, StdHeader);
        ScopeSize += PstateAcpiObjSize;
      }
    }
    ScopeSize += AmdPstateSocketInfo->TotalLogicalCores;
    AmdPstateSocketInfo = (AMD_PSTATE_SOCKET_INFO *) ((UINT8 *) AmdPstateSocketInfo + sizeof (AMD_PSTATE_SOCKET_INFO) + sizeof (S_PSTATE_VALUES) * AmdPstateSocketInfo->PStateMaxValue);
  }

  return ScopeSize;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Returns the size of Cstate ACPI objects
 *
 *  Parameters:
 *    @param[in]     *AmdCstateInfo           Cstate information
 *    @param[in]     *StdHeader               Stand header
 *
 *    @retval        CstObjSize               Size of CST Object
 *
 */
UINT32
AmdAcpiCalCstObj (
  IN       AMD_CSTATE_INFO         *AmdCstateInfo,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  UINT32   CStateAcpiObjSize;

  CStateAcpiObjSize = CST_HEADER_SIZE + CST_BODY_SIZE;

  // If CSD Object is generated, add the size of CSD Object to the total size of
  // CState ACPI Object size
  if (AmdCstateInfo->IsCsdGenerated) {
    CStateAcpiObjSize += CSD_HEADER_SIZE + CSD_BODY_SIZE;
  }

  if (PcdGetBool (PcdAmdAcpiCstC1) && AmdCstateInfo->IsMonitorMwaitSupported) {
    CStateAcpiObjSize += CST_BODY_SIZE;
  }

  return CStateAcpiObjSize;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Returns the size of Pstate ACPI objects
 *
 *  Parameters:
 *    @param[in]     *AmdPstateSocketInfo     Pstate information for this socket
 *    @param[in]     *StdHeader               Stand header
 *
 *    @retval        CstObjSize               Size of CST Object
 *
 */
UINT32
AmdAcpiCalPstObj (
  IN       AMD_PSTATE_SOCKET_INFO  *AmdPstateSocketInfo,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  UINT32 MaxPstateNumber;

  MaxPstateNumber = AmdPstateSocketInfo->PStateMaxValue + 1;
  return (PCT_STRUCT_SIZE +
          PSS_HEADER_STRUCT_SIZE +
          (MaxPstateNumber * PSS_BODY_STRUCT_SIZE) +
          XPSS_HEADER_STRUCT_SIZE +
          (MaxPstateNumber * XPSS_BODY_STRUCT_SIZE) +
          PSD_HEADER_STRUCT_SIZE +
          PSD_BODY_STRUCT_SIZE +
          PPC_HEADER_BODY_STRUCT_SIZE);
}

/**--------------------------------------------------------------------------------------
 *
 *  CreateCStateAcpiTables
 *
 *  Description:
 *     This is the common routine for creating ACPI C-State objects
 *
 *  Parameters:
 *    @param[in]     AmdCstateInfo        Buffer that contains C-State information
 *    @param[in,out] SsdtPtr              ACPI SSDT table pointer
 *    @param[in]     LocalApicId          Local Apic Id
 *    @param[in]     StdHeader            Handle to config for library and services
 *
 *    @retval        Size of ACPI C-States objects generated
 *
 *---------------------------------------------------------------------------------------
 **/
UINT32
AmdCreateCStateAcpiTables (
  IN       AMD_CSTATE_INFO         *AmdCstateInfo,
  IN OUT   VOID                   **SsdtPtr,
  IN       UINT8                    LocalApicId,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  UINT8                 NumberOfCstates;
  UINT32                ObjSize;
  BOOLEAN               DeclareC1;
  CST_HEADER_STRUCT     *CstHeaderPtr;
  CST_BODY_STRUCT       *CstBodyPtr;
  CSD_HEADER_STRUCT     *CsdHeaderPtr;
  CSD_BODY_STRUCT       *CsdBodyPtr;

  ObjSize = 0;

  if (PcdGetBool (PcdAmdAcpiCstC1) && AmdCstateInfo->IsMonitorMwaitSupported) {
    DeclareC1 = TRUE;
    NumberOfCstates = 2;
  } else {
    DeclareC1 = FALSE;
    NumberOfCstates = 1;
  }

  //====================
  // Set CST Table
  //====================

  // Set CST Header
  // --------------

  // Typecast the pointer
  CstHeaderPtr = (CST_HEADER_STRUCT *) *SsdtPtr;

  CstHeaderPtr->NameOpcode    = NAME_OPCODE;
  CstHeaderPtr->CstName_a__   = CST_NAME__;
  CstHeaderPtr->CstName_a_C   = CST_NAME_C;
  CstHeaderPtr->CstName_a_S   = CST_NAME_S;
  CstHeaderPtr->CstName_a_T   = CST_NAME_T;
  CstHeaderPtr->PkgOpcode     = PACKAGE_OPCODE;
  CstHeaderPtr->PkgLength     = (DeclareC1) ? CST_LENGTH_WITH_C1 : CST_LENGTH_NO_C1;
  CstHeaderPtr->PkgElements   = NumberOfCstates + 1;
  CstHeaderPtr->BytePrefix    = BYTE_PREFIX_OPCODE;
  CstHeaderPtr->Count         = NumberOfCstates;

  // Set CST Body
  // --------------

  // Typecast the pointer
  CstHeaderPtr++;
  CstBodyPtr = (CST_BODY_STRUCT *) CstHeaderPtr;

  if (DeclareC1) {

    CstBodyPtr->PkgOpcode       = PACKAGE_OPCODE;
    CstBodyPtr->PkgLength       = CST_PKG_LENGTH;
    CstBodyPtr->PkgElements     = CST_PKG_ELEMENTS;
    CstBodyPtr->BufferOpcode    = BUFFER_OPCODE;
    CstBodyPtr->BufferLength    = CST_SUBPKG_LENGTH;
    CstBodyPtr->BufferElements  = CST_SUBPKG_ELEMENTS;
    CstBodyPtr->BufferOpcode2   = BUFFER_OPCODE;
    CstBodyPtr->GdrOpcode       = GENERIC_REG_DESCRIPTION;
    CstBodyPtr->GdrLength       = CST_GDR_LENGTH;
    CstBodyPtr->AddrSpaceId     = GDR_ASI_FIXED_HW;
    CstBodyPtr->RegBitWidth     = 0x02;
    CstBodyPtr->RegBitOffset    = 0x02;
    CstBodyPtr->AddressSize     = GDR_ASZ_UNDEFINED;
    CstBodyPtr->RegisterAddr    = 0;
    CstBodyPtr->EndTag          = 0x0079;
    CstBodyPtr->BytePrefix      = BYTE_PREFIX_OPCODE;
    CstBodyPtr->Type            = CST_C1_TYPE;
    CstBodyPtr->WordPrefix      = WORD_PREFIX_OPCODE;
    CstBodyPtr->Latency         = 1;
    CstBodyPtr->DWordPrefix     = DWORD_PREFIX_OPCODE;
    CstBodyPtr->Power           = 0;

    CstBodyPtr++;
  }

  CstBodyPtr->PkgOpcode       = PACKAGE_OPCODE;
  CstBodyPtr->PkgLength       = CST_PKG_LENGTH;
  CstBodyPtr->PkgElements     = CST_PKG_ELEMENTS;
  CstBodyPtr->BufferOpcode    = BUFFER_OPCODE;
  CstBodyPtr->BufferLength    = CST_SUBPKG_LENGTH;
  CstBodyPtr->BufferElements  = CST_SUBPKG_ELEMENTS;
  CstBodyPtr->BufferOpcode2   = BUFFER_OPCODE;
  CstBodyPtr->GdrOpcode       = GENERIC_REG_DESCRIPTION;
  CstBodyPtr->GdrLength       = CST_GDR_LENGTH;
  CstBodyPtr->AddrSpaceId     = GDR_ASI_SYSTEM_IO;
  CstBodyPtr->RegBitWidth     = 0x08;
  CstBodyPtr->RegBitOffset    = 0x00;
  CstBodyPtr->AddressSize     = GDR_ASZ_BYTE_ACCESS;
  CstBodyPtr->RegisterAddr    = AmdCstateInfo->IoCstateAddr + 1;
  CstBodyPtr->EndTag          = 0x0079;
  CstBodyPtr->BytePrefix      = BYTE_PREFIX_OPCODE;
  CstBodyPtr->Type            = CST_C2_TYPE;
  CstBodyPtr->WordPrefix      = WORD_PREFIX_OPCODE;
  CstBodyPtr->Latency         = 400;
  CstBodyPtr->DWordPrefix     = DWORD_PREFIX_OPCODE;
  CstBodyPtr->Power           = 0;

  CstBodyPtr++;

  // Update the pointer
  *SsdtPtr = CstBodyPtr;

  //====================
  // Set CSD Table
  //====================

  if (AmdCstateInfo->IsCsdGenerated) {

    // Set CSD Header
    // --------------

    // Typecast the pointer
    CsdHeaderPtr = (CSD_HEADER_STRUCT *) *SsdtPtr;

    CsdHeaderPtr->NameOpcode  = NAME_OPCODE;
    CsdHeaderPtr->CsdName_a__ = CST_NAME__;
    CsdHeaderPtr->CsdName_a_C = CST_NAME_C;
    CsdHeaderPtr->CsdName_a_S = CST_NAME_S;
    CsdHeaderPtr->CsdName_a_D = CSD_NAME_D;

    // Set CSD Body
    // --------------

    // Typecast the pointer
    CsdHeaderPtr++;
    CsdBodyPtr = (CSD_BODY_STRUCT *) CsdHeaderPtr;

    CsdBodyPtr->PkgOpcode     = PACKAGE_OPCODE;
    CsdBodyPtr->PkgLength     = CSD_BODY_SIZE - 1;
    CsdBodyPtr->PkgElements   = 1;
    CsdBodyPtr->PkgOpcode2    = PACKAGE_OPCODE;
    CsdBodyPtr->PkgLength2    = CSD_BODY_SIZE - 4; // CSD_BODY_SIZE - Package() - Package Opcode
    CsdBodyPtr->PkgElements2  = 6;
    CsdBodyPtr->BytePrefix    = BYTE_PREFIX_OPCODE;
    CsdBodyPtr->NumEntries    = 6;
    CsdBodyPtr->BytePrefix2   = BYTE_PREFIX_OPCODE;
    CsdBodyPtr->Revision      = 0;
    CsdBodyPtr->DWordPrefix   = DWORD_PREFIX_OPCODE;
    CsdBodyPtr->Domain        = (LocalApicId & 0xFE) >> 1;
    CsdBodyPtr->DWordPrefix2  = DWORD_PREFIX_OPCODE;
    CsdBodyPtr->CoordType     = CSD_COORD_TYPE_HW_ALL;
    CsdBodyPtr->DWordPrefix3  = DWORD_PREFIX_OPCODE;
    CsdBodyPtr->NumProcessors = 0x2;
    CsdBodyPtr->DWordPrefix4  = DWORD_PREFIX_OPCODE;
    CsdBodyPtr->Index         = 0x0;

    CsdBodyPtr++;

    // Update the pointer
    *SsdtPtr = CsdBodyPtr;
  }

  ObjSize = AmdAcpiCalCstObj (AmdCstateInfo, StdHeader);
  return ObjSize;
}

/**--------------------------------------------------------------------------------------
 *
 *  CreatePStateAcpiTables
 *
 *  Description:
 *     This is the common routine for creating ACPI P-State objects
 *
 *  Parameters:
 *    @param[in]     AmdPstateSocketInfo  Buffer that contains P-State information
 *    @param[in,out] SsdtPtr              ACPI SSDT table pointer
 *    @param[in]     CpuSsdtServices      A pointer to AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL
 *    @param[in]     LocalApicId          Local Apic Id
 *    @param[in]     StdHeader            Handle to config for library and services
 *
 *    @retval          Size of generated ACPI P-States objects
 *
 *---------------------------------------------------------------------------------------
 **/
UINT32
AmdCreatePStateAcpiTables (
  IN       AMD_PSTATE_SOCKET_INFO  *AmdPstateSocketInfo,
  IN OUT   VOID                   **SsdtPtr,
  IN       AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL *CpuSsdtServices,
  IN       UINT8                    LocalApicId,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  UINT8                      PstateCapLevelSupport;
  UINT8                      PstateNum;
  UINT32                     SizeTemp;
  UINT32                     PstateCapInputMilliWatts;
  UINT32                     CurrSize;
  UINT32                     PstateCount;
  UINTN                      NumberOfComplexes;
  UINTN                      NumberOfCores;
  UINTN                      NumberOfThreads;
  BOOLEAN                    PstateCapEnable;
  BOOLEAN                    PstateCapLevelSupportDetermined;
  PCT_HEADER_BODY            *pPctAcpiTables;
  PSS_HEADER                 *pPssHeaderAcpiTables;
  PSS_BODY                   *pPssBodyAcpiTables;
  XPSS_HEADER                *pXpssHeaderAcpiTables;
  XPSS_BODY                  *pXpssBodyAcpiTables;
  PSD_HEADER                 *pPsdHeaderAcpiTables;
  PSD_BODY                   *pPsdBodyAcpiTables;
  PPC_HEADER_BODY            *pPpcAcpiTables;
  AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL *CoreTopology;
  EFI_STATUS                 CalledStatus;

  CurrSize = 0;
  PstateCount = 0;
  PstateCapEnable = FALSE;
  PstateCapLevelSupport = 0;
  PstateCapLevelSupportDetermined = TRUE;
  PstateCapInputMilliWatts = PcdGet32 (PcdAmdPowerCeiling);

  //Check Pstate Capability
  if (PstateCapInputMilliWatts != 0) {
    PstateCapEnable = TRUE;
    PstateCapLevelSupportDetermined = FALSE;
  }

  //====================
  // Set _PCT Table
  //====================

  // Typecast the pointer
  pPctAcpiTables = (PCT_HEADER_BODY *) *SsdtPtr;

  // Set _PCT Body
  //---------------
  if (PcdGetBool (PcdAmdAcpiCpuSsdtPct)) {
    pPctAcpiTables->NameOpcode                  = NAME_OPCODE;
    pPctAcpiTables->PctName_a__                 = PCT_NAME__;
    pPctAcpiTables->PctName_a_P                 = PCT_NAME_P;
    pPctAcpiTables->PctName_a_C                 = PCT_NAME_C;
    pPctAcpiTables->PctName_a_T                 = PCT_NAME_T;
    pPctAcpiTables->Value1                      = PCT_VALUE1;
    pPctAcpiTables->Value2                      = PCT_VALUE2;
    pPctAcpiTables->Value3                      = PCT_VALUE3;
    pPctAcpiTables->GenericRegDescription1      = GENERIC_REG_DESCRIPTION;
    pPctAcpiTables->Length1                     = PCT_LENGTH;
    pPctAcpiTables->AddressSpaceId1             = PCT_ADDRESS_SPACE_ID;
    pPctAcpiTables->RegisterBitWidth1           = PCT_REGISTER_BIT_WIDTH;
    pPctAcpiTables->RegisterBitOffset1          = PCT_REGISTER_BIT_OFFSET;
    pPctAcpiTables->Reserved1                   = PCT_RESERVED;
    pPctAcpiTables->ControlRegAddressLo         = PCT_CONTROL_REG_LO;
    pPctAcpiTables->ControlRegAddressHi         = PCT_CONTROL_REG_HI;
    pPctAcpiTables->Value4                      = PCT_VALUE4;
    pPctAcpiTables->Value5                      = PCT_VALUE5;
    pPctAcpiTables->GenericRegDescription2      = GENERIC_REG_DESCRIPTION;
    pPctAcpiTables->Length2                     = PCT_LENGTH;
    pPctAcpiTables->AddressSpaceId2             = PCT_ADDRESS_SPACE_ID;
    pPctAcpiTables->RegisterBitWidth2           = PCT_REGISTER_BIT_WIDTH;
    pPctAcpiTables->RegisterBitOffset2          = PCT_REGISTER_BIT_OFFSET;
    pPctAcpiTables->Reserved2                   = PCT_RESERVED;
    pPctAcpiTables->StatusRegAddressLo          = PCT_STATUS_REG_LO;
    pPctAcpiTables->StatusRegAddressHi          = PCT_STATUS_REG_HI;
    pPctAcpiTables->Value6                      = PCT_VALUE6;

    // Increment and then typecast the pointer
    pPctAcpiTables++;
    CurrSize += PCT_STRUCT_SIZE;

    *SsdtPtr = pPctAcpiTables;
  } // End of _PCT Table

  //====================
  // Set _PSS Table
  //====================

  // Typecast the pointer
  pPssHeaderAcpiTables = (PSS_HEADER *) pPctAcpiTables;
  pPssBodyAcpiTables   = (PSS_BODY *)   pPctAcpiTables;

  if (PcdGetBool (PcdAmdAcpiCpuSsdtPss)) {
    // Set _PSS Header
    //---------------

    // Note: Set pssLength and numOfItemsInPss later
    pPssHeaderAcpiTables->NameOpcode            = NAME_OPCODE;
    pPssHeaderAcpiTables->PssName_a__           = PSS_NAME__;
    pPssHeaderAcpiTables->PssName_a_P           = PSS_NAME_P;
    pPssHeaderAcpiTables->PssName_a_S           = PSS_NAME_S;
    pPssHeaderAcpiTables->PssName_b_S           = PSS_NAME_S;
    pPssHeaderAcpiTables->PkgOpcode             = PACKAGE_OPCODE;


    // Set _PSS Body
    //---------------

    // Typecast the pointer
    pPssBodyAcpiTables = (PSS_BODY *) (pPssHeaderAcpiTables + 1);

    PstateCount = 0;
    for (PstateNum = 0; PstateNum <= AmdPstateSocketInfo->PStateMaxValue; PstateNum++) {
      if (AmdPstateSocketInfo->PStateStruct[PstateNum].PStateEnable != 0) {
        pPssBodyAcpiTables->PkgOpcode           = PACKAGE_OPCODE;
        pPssBodyAcpiTables->PkgLength           = PSS_PKG_LENGTH;
        pPssBodyAcpiTables->NumOfElements       = PSS_NUM_OF_ELEMENTS;
        pPssBodyAcpiTables->DwordPrefixOpcode1  = DWORD_PREFIX_OPCODE;
        pPssBodyAcpiTables->Frequency           = AmdPstateSocketInfo->PStateStruct[PstateNum].CoreFreq;
        pPssBodyAcpiTables->DwordPrefixOpcode2  = DWORD_PREFIX_OPCODE;
        pPssBodyAcpiTables->Power               = AmdPstateSocketInfo->PStateStruct[PstateNum].Power;

        if (PstateCapEnable && (!PstateCapLevelSupportDetermined) && (PstateCapInputMilliWatts >= pPssBodyAcpiTables->Power)) {
          PstateCapLevelSupport = (UINT8) AmdPstateSocketInfo->PStateStruct[PstateNum].SwPstateNumber;
          PstateCapLevelSupportDetermined = TRUE;
        }

        pPssBodyAcpiTables->DwordPrefixOpcode3  = DWORD_PREFIX_OPCODE;
        pPssBodyAcpiTables->TransitionLatency   = AmdPstateSocketInfo->TransitionLatency;
        pPssBodyAcpiTables->DwordPrefixOpcode4  = DWORD_PREFIX_OPCODE;
        pPssBodyAcpiTables->BusMasterLatency    = AmdPstateSocketInfo->TransitionLatency;
        pPssBodyAcpiTables->DwordPrefixOpcode5  = DWORD_PREFIX_OPCODE;
        pPssBodyAcpiTables->Control             = AmdPstateSocketInfo->PStateStruct[PstateNum].SwPstateNumber;
        pPssBodyAcpiTables->DwordPrefixOpcode6  = DWORD_PREFIX_OPCODE;
        pPssBodyAcpiTables->Status              = AmdPstateSocketInfo->PStateStruct[PstateNum].SwPstateNumber;

        pPssBodyAcpiTables++;
        PstateCount++;
      }
    } // for (PstateNum = 0; PstateNum < MPPSTATE_MAXIMUM_STATES; PstateNum++)

    if (PstateCapEnable && (!PstateCapLevelSupportDetermined)) {
      PstateCapLevelSupport = AmdPstateSocketInfo->PStateMaxValue;
    }

    // Set _PSS Header again
    // Now Set pssLength and numOfItemsInPss
    SizeTemp = (PstateCount * PSS_BODY_STRUCT_SIZE) + 3;
    pPssHeaderAcpiTables->PssLength  = (SizeTemp << 4) & 0x0000FF00;
    pPssHeaderAcpiTables->PssLength |= (SizeTemp & 0x0000000F) | 0x00000040;
    pPssHeaderAcpiTables->NumOfItemsInPss = (UINT8) PstateCount;
    CurrSize += (PSS_HEADER_STRUCT_SIZE + (PstateCount * PSS_BODY_STRUCT_SIZE));

    *SsdtPtr = pPssBodyAcpiTables;
  } // End of _PSS Table

  //====================
  // Set XPSS Table
  //====================

  // Typecast the pointer
  pXpssHeaderAcpiTables = (XPSS_HEADER *) pPssBodyAcpiTables;
  pXpssBodyAcpiTables   = (XPSS_BODY *)   pPssBodyAcpiTables;

  if (PcdGetBool (PcdAmdAcpiCpuSsdtXpss)) {
    // Set XPSS Header
    //---------------

    // Note: Set the pssLength and numOfItemsInPss later
    pXpssHeaderAcpiTables->NameOpcode           = NAME_OPCODE;
    pXpssHeaderAcpiTables->XpssName_a_X         = PSS_NAME_X;
    pXpssHeaderAcpiTables->XpssName_a_P         = PSS_NAME_P;
    pXpssHeaderAcpiTables->XpssName_a_S         = PSS_NAME_S;
    pXpssHeaderAcpiTables->XpssName_b_S         = PSS_NAME_S;
    pXpssHeaderAcpiTables->PkgOpcode            = PACKAGE_OPCODE;

    // Set XPSS Body
    //---------------

    // Typecast the pointer
    pXpssBodyAcpiTables = (XPSS_BODY *) (pXpssHeaderAcpiTables + 1);

    for (PstateNum = 0; PstateNum <= AmdPstateSocketInfo->PStateMaxValue; PstateNum++) {
      if (AmdPstateSocketInfo->PStateStruct[PstateNum].PStateEnable != 0) {
        pXpssBodyAcpiTables->PkgOpcode          = PACKAGE_OPCODE;
        pXpssBodyAcpiTables->PkgLength          = XPSS_PKG_LENGTH;
        pXpssBodyAcpiTables->NumOfElements      = XPSS_NUM_OF_ELEMENTS;
        pXpssBodyAcpiTables->XpssValueTbd       = 04;
        pXpssBodyAcpiTables->DwordPrefixOpcode1 = DWORD_PREFIX_OPCODE;
        pXpssBodyAcpiTables->Frequency          = AmdPstateSocketInfo->PStateStruct[PstateNum].CoreFreq;
        pXpssBodyAcpiTables->DwordPrefixOpcode2 = DWORD_PREFIX_OPCODE;
        pXpssBodyAcpiTables->Power              = AmdPstateSocketInfo->PStateStruct[PstateNum].Power;
        pXpssBodyAcpiTables->DwordPrefixOpcode3 = DWORD_PREFIX_OPCODE;
        pXpssBodyAcpiTables->TransitionLatency  = AmdPstateSocketInfo->TransitionLatency;
        pXpssBodyAcpiTables->DwordPrefixOpcode4 = DWORD_PREFIX_OPCODE;
        pXpssBodyAcpiTables->BusMasterLatency   = AmdPstateSocketInfo->TransitionLatency;
        pXpssBodyAcpiTables->ControlBuffer      = ACPI_BUFFER;
        pXpssBodyAcpiTables->ControlLo          = AmdPstateSocketInfo->PStateStruct[PstateNum].SwPstateNumber;
        pXpssBodyAcpiTables->ControlHi          = 0;
        pXpssBodyAcpiTables->StatusBuffer       = ACPI_BUFFER;
        pXpssBodyAcpiTables->StatusLo           = AmdPstateSocketInfo->PStateStruct[PstateNum].SwPstateNumber;
        pXpssBodyAcpiTables->StatusHi           = 0;
        pXpssBodyAcpiTables->ControlMaskBuffer  = ACPI_BUFFER;
        pXpssBodyAcpiTables->ControlMaskLo      = 0;
        pXpssBodyAcpiTables->ControlMaskHi      = 0;
        pXpssBodyAcpiTables->StatusMaskBuffer   = ACPI_BUFFER;
        pXpssBodyAcpiTables->StatusMaskLo       = 0;
        pXpssBodyAcpiTables->StatusMaskHi       = 0;

        pXpssBodyAcpiTables++;
      }
    } // for (PstateNum = 0; PstateNum < MPPSTATE_MAXIMUM_STATES; PstateNum++)

    // Set XPSS Header again
    // Now set pssLength and numOfItemsInPss
    SizeTemp = (PstateCount * XPSS_BODY_STRUCT_SIZE) + 3;
    pXpssHeaderAcpiTables->XpssLength  = (SizeTemp << 4) & 0x0000FF00;
    pXpssHeaderAcpiTables->XpssLength |= (SizeTemp & 0x0000000F) | 0x00000040;
    pXpssHeaderAcpiTables->NumOfItemsInXpss = (UINT8) PstateCount;
    CurrSize += (XPSS_HEADER_STRUCT_SIZE + (PstateCount * XPSS_BODY_STRUCT_SIZE));

    *SsdtPtr = pXpssBodyAcpiTables;
  } // End of _XPSS Table

  //====================
  // Set _PSD Table
  //====================

  // Typecast the pointer
  pPsdHeaderAcpiTables = (PSD_HEADER *) pXpssBodyAcpiTables;
  pPsdBodyAcpiTables   = (PSD_BODY *)   pXpssBodyAcpiTables;

  CalledStatus = gBS->LocateProtocol (&gAmdCoreTopologyServicesProtocolGuid, NULL, &CoreTopology);
  ASSERT (!EFI_ERROR (CalledStatus));
  NumberOfThreads = 1;
  if (!EFI_ERROR (CalledStatus)) {
    CoreTopology->GetCoreTopologyOnDie (CoreTopology, AmdPstateSocketInfo->SocketNumber, 0, &NumberOfComplexes, &NumberOfCores, &NumberOfThreads);
  }
  if ((AmdPstateSocketInfo->TotalLogicalCores != 1) && (PcdGetBool (PcdAmdAcpiCpuSsdtPsd)) && ((AmdPstateSocketInfo->IsPsdDependent) || (NumberOfThreads > 1))) {
    // Set _PSD Header
    //----------------
    pPsdHeaderAcpiTables->NameOpcode            = NAME_OPCODE;
    pPsdHeaderAcpiTables->PkgOpcode             = PACKAGE_OPCODE;
    pPsdHeaderAcpiTables->PsdLength             = PSD_HEADER_LENGTH;
    pPsdHeaderAcpiTables->Value1                = PSD_VALUE1;
    pPsdHeaderAcpiTables->PsdName_a__           = PSD_NAME__;
    pPsdHeaderAcpiTables->PsdName_a_P           = PSD_NAME_P;
    pPsdHeaderAcpiTables->PsdName_a_S           = PSD_NAME_S;
    pPsdHeaderAcpiTables->PsdName_a_D           = PSD_NAME_D;

    CurrSize += PSD_HEADER_STRUCT_SIZE;

    // Set _PSD Body
    //--------------

    // Typecast the pointer
    pPsdBodyAcpiTables = (PSD_BODY *) (pPsdHeaderAcpiTables + 1);

    pPsdBodyAcpiTables->PkgOpcode               = PACKAGE_OPCODE;
    pPsdBodyAcpiTables->PkgLength               = PSD_PKG_LENGTH;
    pPsdBodyAcpiTables->NumOfEntries            = NUM_OF_ENTRIES;
    pPsdBodyAcpiTables->BytePrefixOpcode1       = BYTE_PREFIX_OPCODE;
    pPsdBodyAcpiTables->PsdNumOfEntries         = PSD_NUM_OF_ENTRIES;
    pPsdBodyAcpiTables->BytePrefixOpcode2       = BYTE_PREFIX_OPCODE;
    pPsdBodyAcpiTables->PsdRevision             = PSD_REVISION;
    pPsdBodyAcpiTables->DwordPrefixOpcode1      = DWORD_PREFIX_OPCODE;

    if (AmdPstateSocketInfo->IsPsdDependent) {
      pPsdBodyAcpiTables->DependencyDomain      = PSD_DEPENDENCY_DOMAIN;
      pPsdBodyAcpiTables->CoordinationType      = PSD_COORDINATION_TYPE_SW_ALL;
      pPsdBodyAcpiTables->NumOfProcessors       = AmdPstateSocketInfo->TotalLogicalCores;
    } else {
      // Get number of threads per core
      ASSERT (NumberOfThreads != 0);
      pPsdBodyAcpiTables->DependencyDomain      = (UINT32) (LocalApicId / NumberOfThreads); //CpuSsdtServices->GetPsdDomain (CpuSsdtServices, LocalApicId);
      pPsdBodyAcpiTables->CoordinationType      = PSD_COORDINATION_TYPE_HW_ALL;
      pPsdBodyAcpiTables->NumOfProcessors       = (UINT32) NumberOfThreads;
    }
    pPsdBodyAcpiTables->DwordPrefixOpcode2      = DWORD_PREFIX_OPCODE;
    pPsdBodyAcpiTables->DwordPrefixOpcode3      = DWORD_PREFIX_OPCODE;

    pPsdBodyAcpiTables++;
    *SsdtPtr = pPsdBodyAcpiTables;
    CurrSize += PSD_BODY_STRUCT_SIZE;
  } // End of _PSD Table

  //====================
  // Set _PPC Table
  //====================

  // Typecast the pointer
  pPpcAcpiTables = (PPC_HEADER_BODY *) pPsdBodyAcpiTables;

  if (PcdGetBool (PcdAmdAcpiCpuSsdtPpc)) {
    // Name (PPCV, value)
    pPpcAcpiTables->NameOpcode                  = NAME_OPCODE;
    pPpcAcpiTables->PpcName_a_P                 = PPC_NAME_P;
    pPpcAcpiTables->PpcName_b_P                 = PPC_NAME_P;
    pPpcAcpiTables->PpcName_a_C                 = PPC_NAME_C;
    pPpcAcpiTables->PpcName_a_V                 = PPC_NAME_V;
    pPpcAcpiTables->Value1                      = PPC_VALUE1;
    pPpcAcpiTables->DefaultPerfPresentCap       = PstateCapLevelSupport;
    // Method (_PPC) { return (PPCV) }
    pPpcAcpiTables->MethodOpcode                = METHOD_OPCODE;
    pPpcAcpiTables->PpcLength                   = PPC_METHOD_LENGTH;
    pPpcAcpiTables->PpcName_a__                 = PPC_NAME__;
    pPpcAcpiTables->PpcName_c_P                 = PPC_NAME_P;
    pPpcAcpiTables->PpcName_d_P                 = PPC_NAME_P;
    pPpcAcpiTables->PpcName_b_C                 = PPC_NAME_C;
    pPpcAcpiTables->MethodFlags                 = PPC_METHOD_FLAGS;
    pPpcAcpiTables->ReturnOpcode                = RETURN_OPCODE;
    pPpcAcpiTables->PpcName_e_P                 = PPC_NAME_P;
    pPpcAcpiTables->PpcName_f_P                 = PPC_NAME_P;
    pPpcAcpiTables->PpcName_c_C                 = PPC_NAME_C;
    pPpcAcpiTables->PpcName_b_V                 = PPC_NAME_V;

    CurrSize += PPC_HEADER_BODY_STRUCT_SIZE;
    // Increment and typecast the pointer
    pPpcAcpiTables++;
    *SsdtPtr =  pPpcAcpiTables;
  } // End of _PPC Table

  return CurrSize;
}

