#ifndef __HYGON_REPORT_RAS_UCE_H__
#define __HYGON_REPORT_RAS_UCE_H__


#pragma pack(1)

typedef struct {
  UINT64   CpuCoreMap[4];
  UINT32   PageCount;
  UINT32   MemPage[1];  
} HYGON_REPORT_RAS_UCE_INFO;

#pragma pack()

#define HYGON_REPORT_RAS_UCE_VARIABLE_NAME                  L"RasUce"

extern EFI_GUID gHygonReportRasUceVariableGuid;


typedef struct _HYGON_REPORT_UCE_PROTOCOL HYGON_REPORT_UCE_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *REPORT_MEM_UCE_ADDRESS)(
  IN HYGON_REPORT_UCE_PROTOCOL  *This,
  IN UINT64                     MemAddress
  );

typedef
BOOLEAN
(EFIAPI *IS_THIS_CORE_UCE)(
  IN HYGON_REPORT_UCE_PROTOCOL  *This,
  IN UINTN                      ApicId
  );

typedef
EFI_STATUS
(EFIAPI *REPORT_CORE_UCE)(
  IN HYGON_REPORT_UCE_PROTOCOL  *This,
  IN UINTN                      ApicId
  );

typedef
EFI_STATUS
(EFIAPI *GET_UCE_MEM_LIST)(
  IN HYGON_REPORT_UCE_PROTOCOL  *This,
  UINT32                        *MemPageCount,
  UINT32                        **MemPage
  );

typedef
EFI_STATUS
(EFIAPI *REPORT_UCE_END)(
  IN HYGON_REPORT_UCE_PROTOCOL  *This
  );

typedef
EFI_STATUS
(EFIAPI *GET_NEW_ERROR)(
  IN HYGON_REPORT_UCE_PROTOCOL      *This,
  OUT UINT32                        **MemPage,
  OUT UINT32                        *MemPageCount,
  OUT UINT64                        **ApicId,
  OUT UINTN                         *ApicIdSize
  );

struct _HYGON_REPORT_UCE_PROTOCOL {
  REPORT_MEM_UCE_ADDRESS  ReportMemUceAddr;
  REPORT_CORE_UCE         ReportCoreUce;
  IS_THIS_CORE_UCE        IsThisCoreUce;
  GET_UCE_MEM_LIST        GetUceMemList;
  REPORT_UCE_END          End;
  GET_NEW_ERROR           GetNewErr;
};

extern EFI_GUID gHygonReportRasUceProtocolGuid;

#endif

