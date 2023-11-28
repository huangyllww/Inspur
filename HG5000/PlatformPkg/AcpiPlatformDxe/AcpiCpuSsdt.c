
#include "AcpiPlatform.h"
#include <Guid/Acpi.h>

#pragma pack (push, 1)
//--------------------------------------------
//              AML code definition
//                   (Scope)
//---------------------------------------------
/// SCOPE
typedef struct _SCOPE {
  UINT8   ScopeOpcode;                  ///< Opcode
  UINT16  ScopeLength;                  ///< Scope Length
  UINT8   ScopeValue;                   ///< Value
  UINT8   ScopeNamePt1a__;              ///< Name Pointer
  UINT8   ScopeNamePt1a_P;              ///< Name Pointer
  UINT8   ScopeNamePt1a_R;              ///< Name Pointer
  UINT8   ScopeNamePt1b__;              ///< Name Pointer
} SCOPE;
//--------------------------------------------
//              AML code definition
//                   (AML_DEVICE)
//---------------------------------------------
// AML DEVICE
typedef struct _AML_PROCESSOR_DEVICE {
  UINT8   Opcode5b;                  ///< Opcode
  UINT8   Opcode82;                  ///< Opcode
  UINT8   Opcode19;                  ///< Opcode
  UINT8   DiviceName_C;              ///< Name Pointer
  UINT8   DiviceName_P;              ///< Name Pointer
  UINT8   DiviceName_U;              ///< Name Pointer
  UINT8   DiviceName_0;              ///< Name Pointer
  UINT8   Opcode08;                  ///< Opcode  
  UINT8   DiviceName2__;             ///< Opcode  
  UINT8   DiviceName2_H;             ///< Opcode  
  UINT8   DiviceName2_I;             ///< Opcode  
  UINT8   DiviceName2_D;             ///< Opcode  
  UINT8   Opcode0D;                  ///< Opcode  
  UINT8   DiviceName3_A;             ///< Opcode  
  UINT8   DiviceName3_C;             ///< Opcode  
  UINT8   DiviceName3_P;             ///< Opcode  
  UINT8   DiviceName3_I;             ///< Opcode  
  UINT8   DiviceName3a_0;            ///< Opcode  
  UINT8   DiviceName3b_0;            ///< Opcode  
  UINT8   DiviceName3c_0;            ///< Opcode  
  UINT8   DiviceName3_7;             ///< Opcode  
  UINT8   Opcode00;                  ///< Opcode    
  UINT8   Opcode408;                 ///< Opcode    
  UINT8   DiviceName4__;             ///< Name Pointer  
  UINT8   DiviceName4_U;             ///< Name Pointer  
  UINT8   DiviceName4_I;             ///< Name Pointer  
  UINT8   DiviceName4_D;             ///< Name Pointer  
  UINT8   Opcode0A;                  ///< Opcode    
  UINT8   DiviceName5_0;             ///< Name Pointer  
} AML_PROCESSOR_DEVICE;

typedef struct _AML_PROCESSOR_PR {
  UINT8   Opcode5b;                  ///< Opcode
  UINT8   Opcode83;                  ///< Opcode
  UINT8   Opcode0b;                  ///< Opcode
  UINT8   DiviceName_C;              ///< Name Pointer
  UINT8   DiviceName_P;              ///< Name Pointer
  UINT8   DiviceName_U;              ///< Name Pointer
  UINT8   DiviceName_0;              ///< Name Pointer
  UINT8   DiviceName5_0;             ///< Name Pointer  
  UINT8   ZERO[5];             ///< Name Pointer  
} AML_PROCESSOR_PR;

#pragma pack (pop)

STATIC AML_PROCESSOR_DEVICE  gAML_template = {  
//\_SB_        
// Device(C000) { Name(_HID, "ACPI0007")}
 0x5B, 0x82, 0x1B, 0x43, 0x30, 0x30, 0x30, 0x08, 0x5F, 0x48, 0x49, 0x44, 0x0D, 0x41, 0x43, 0x50, 0x49, 0x30, 0x30, 0x30, 0x37, 0x00, 0x08, 0x5F, 0x55, 0x49, 0x44, 0x0A, 0x00
};

STATIC AML_PROCESSOR_PR  gAPPR_template = {  
//\_PR_        
//Processor(C000,0x00,0x00000000,0x00) {}
 0x5B, 0x83, 0x0B, 0x43, 0x30, 0x30, 0x30, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
};

/**
   set CPU SSDT as 1st SSDT table
   XSDT RSDT just change order, no need update checksum
  @return     Unable to allocate required resources.

**/
VOID
SortSSDTTable(
        )
{
    UINTN                                       Index ;
    UINTN                                       Index1st ;
    UINTN                                       IndexLast ;
    EFI_STATUS                                  Status;
    EFI_ACPI_4_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp;
    EFI_ACPI_DESCRIPTION_HEADER                   *Xsdt;
    EFI_ACPI_DESCRIPTION_HEADER                   *Rsdt;
    UINT64                                        *XTableAddress;
    UINT32                                        *RTableAddress;
    UINTN                                         TableCount;
    EFI_ACPI_DESCRIPTION_HEADER                   *AcpiHdr;
    EFI_ACPI_DESCRIPTION_HEADER                   *SSDT1stHdr = NULL;
    EFI_ACPI_DESCRIPTION_HEADER                   *SSDTLastHdr = NULL;

    Status = EfiGetSystemConfigurationTable(&gEfiAcpiTableGuid, &Rsdp);
    ASSERT_EFI_ERROR (Status);

    //sort Xsdt
    Xsdt = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)Rsdp->XsdtAddress;
    DEBUG((EFI_D_INFO, "%a L%d Xsdt:%x\n", __FUNCTION__, __LINE__, Xsdt));
    if (Xsdt->Signature != EFI_ACPI_2_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
        return;
    }
    TableCount = (Xsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT64);
    XTableAddress = (UINT64 *)(Xsdt + 1);
    for (Index = 0; Index < TableCount; Index++) {
      AcpiHdr = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)XTableAddress[Index];
      if (AcpiHdr->Signature == EFI_ACPI_1_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
          DEBUG((EFI_D_INFO, "%a L%d AcpiHdr:%x\n", __FUNCTION__, __LINE__, AcpiHdr));
          if(SSDT1stHdr == NULL) {
              SSDT1stHdr = AcpiHdr;
              Index1st = Index;
          }
          SSDTLastHdr = AcpiHdr;
          IndexLast = Index;
      }
    }
    if(SSDTLastHdr != SSDT1stHdr) {
        XTableAddress[Index1st] = (UINT64)SSDTLastHdr;
        XTableAddress[IndexLast] = (UINT64)SSDT1stHdr;
    }

    //sort Rsdt
    SSDT1stHdr = NULL;
    SSDTLastHdr = NULL;
    Rsdt = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)Rsdp->RsdtAddress;
    DEBUG((EFI_D_INFO, "%a L%d Rsdt:%x\n", __FUNCTION__, __LINE__, Rsdt));
    if (Rsdt->Signature != EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
        return;
    }
    TableCount = (Rsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT32);
    RTableAddress = (UINT32 *)(Rsdt + 1);
    for (Index = 0; Index < TableCount; Index++) {
      AcpiHdr = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)RTableAddress[Index];
      if (AcpiHdr->Signature == EFI_ACPI_1_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
          DEBUG((EFI_D_INFO, "%a L%d AcpiHdr:%x\n", __FUNCTION__, __LINE__, AcpiHdr));
          if(SSDT1stHdr == NULL) {
              SSDT1stHdr = AcpiHdr;
              Index1st = Index;
          }
          SSDTLastHdr = AcpiHdr;
          IndexLast = Index;
      }
    }
    if(SSDTLastHdr != SSDT1stHdr) {
        RTableAddress[Index1st] = (UINT32)SSDTLastHdr;
        RTableAddress[IndexLast] = (UINT32)SSDT1stHdr;
    }
    
}

VOID
BuildAcpiCpuSsdtTable (
  EFI_ACPI_TABLE_PROTOCOL  *AcpiTable
	)
{
  CPU_APIC_ID_INFO                                      *CpuApicIdTable;
  UINTN                                                 CpuCount;
  UINTN                                                 CpuIndex;
  UINTN                                                 TableSize;
  EFI_ACPI_DESCRIPTION_HEADER                           *pCpuSsdt;
  SCOPE                                                 *pScope;
  AML_PROCESSOR_DEVICE                                  *pApd;
  AML_PROCESSOR_PR                                      *pAppr;
  UINT32                                                CurrSize;
  UINT32                                                SizeTemp;
  EFI_STATUS                                            Status;
  UINTN                                                 TableKey;

  GetCpuLocalApicInfo(&CpuApicIdTable, &CpuCount);
  if (PcdGetBool (PcdAmdAcpiCpuSsdtProcessorScopeInSb)) {
      TableSize = sizeof(EFI_ACPI_DESCRIPTION_HEADER)
                     + sizeof(SCOPE)
                     + sizeof(AML_PROCESSOR_DEVICE) * CpuCount;
  } else {
      TableSize = sizeof(EFI_ACPI_DESCRIPTION_HEADER)
                     + sizeof(SCOPE)
                     + sizeof(AML_PROCESSOR_PR) * CpuCount;
  }
  pCpuSsdt = AllocateZeroPool(TableSize);
  ASSERT(pCpuSsdt != NULL);

  pCpuSsdt->Signature = EFI_ACPI_1_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE;
  pCpuSsdt->Length    = (UINT32)TableSize;
  pCpuSsdt->Revision  = 2;
  CopyMem(pCpuSsdt->OemId, PcdGetPtr(PcdAcpiDefaultOemId), sizeof(pCpuSsdt->OemId));
  pCpuSsdt->OemTableId      = PcdGet64(PcdAcpiDefaultOemTableId);
  pCpuSsdt->OemRevision     = PcdGet32(PcdAcpiDefaultOemRevision);
  pCpuSsdt->CreatorId       = PcdGet32(PcdAcpiDefaultCreatorId);
  pCpuSsdt->CreatorRevision = PcdGet32(PcdAcpiDefaultCreatorId);
  
  pScope = (SCOPE*)(pCpuSsdt+1);
  pScope->ScopeOpcode = 0x10;
  
  CurrSize = (UINT32)TableSize - sizeof(EFI_ACPI_DESCRIPTION_HEADER) -1;
  SizeTemp = ((CurrSize << 4) & 0x0000FF00);
  SizeTemp |= ((CurrSize & 0x0000000F) | 0x00000040);
  pScope->ScopeLength = (UINT16) SizeTemp;
  
  pScope->ScopeValue  = '\\';
  pScope->ScopeNamePt1a__ = '_';
  if (PcdGetBool (PcdAmdAcpiCpuSsdtProcessorScopeInSb)) {
      pScope->ScopeNamePt1a_P = 'S';
      pScope->ScopeNamePt1a_R = 'B';
  } else {
      pScope->ScopeNamePt1a_P = 'P';
      pScope->ScopeNamePt1a_R = 'R';
  }
  pScope->ScopeNamePt1b__ = '_';

  if (PcdGetBool (PcdAmdAcpiCpuSsdtProcessorScopeInSb)) {
      pApd = (AML_PROCESSOR_DEVICE*)(pScope+1);
      for(CpuIndex=0; CpuIndex<CpuCount; CpuIndex++){
          CopyMem(pApd, &gAML_template, sizeof(AML_PROCESSOR_DEVICE));
          pApd->DiviceName_U = '0'+ (UINT8)(CpuIndex>>4);
    
          if ((CpuIndex&0x0F) > 0x09){
              pApd->DiviceName_0 = 'A'+ (UINT8)(CpuIndex&0x0F) -0x0A;
          }else{
              pApd->DiviceName_0 = '0'+ (UINT8)(CpuIndex&0x0F);
          }
          
          pApd->DiviceName5_0 = (UINT8)CpuIndex;
          pApd++;
      }
  } else {
      pAppr = (AML_PROCESSOR_PR*)(pScope+1);
      for(CpuIndex=0; CpuIndex<CpuCount; CpuIndex++){
          CopyMem(pAppr, &gAPPR_template, sizeof(AML_PROCESSOR_PR));
          pAppr->DiviceName_U = '0'+ (UINT8)(CpuIndex>>4);

          if ((CpuIndex&0x0F) > 0x09){
              pAppr->DiviceName_0 = 'A'+ (UINT8)(CpuIndex&0x0F) -0x0A;
          }else{
              pAppr->DiviceName_0 = '0'+ (UINT8)(CpuIndex&0x0F);
          }
          
          pAppr->DiviceName5_0 = (UINT8)CpuIndex;
          pAppr++;
      } 
  }
  //PlatformUpdateTables((EFI_ACPI_COMMON_HEADER*)pCpuSsdt);
  AcpiTableUpdateChksum(pCpuSsdt);
  Status = AcpiTable->InstallAcpiTable(
                        AcpiTable,
                        pCpuSsdt,
                        TableSize,
                        &TableKey
                        );
  DEBUG((EFI_D_INFO, "%a L%d %r\n", __FUNCTION__, __LINE__, Status));
  SortSSDTTable();
  FreePool(pCpuSsdt);
  FreePool(CpuApicIdTable);
}



