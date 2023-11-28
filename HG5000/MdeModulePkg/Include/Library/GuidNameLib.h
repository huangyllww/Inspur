#ifndef __GUID_NAME_LIB_H__
#define __GUID_NAME_LIB_H__

typedef
VOID
(EFIAPI *FUNC_SHOW_GUID_NAME)(
  IN CHAR8         *Str,
  IN EFI_GUID      *Guid,
  IN VOID          *Interface
  );

extern FUNC_SHOW_GUID_NAME gFuncShowGuidName;  
  
#endif
