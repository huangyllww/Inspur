#ifndef __BYO_PLATFORM_LANGUAGE_PROTOCOL_H__
#define __BYO_PLATFORM_LANGUAGE_PROTOCOL_H__


typedef struct {
  CHAR8    PlatformLang[32];
  BOOLEAN  LangChanged;
} BYO_PLATFORM_LANGUAGE_PROTOCOL;

extern EFI_GUID gByoPlatformLanguageProtocolGuid;

#endif
