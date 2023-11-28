#ifndef __BYO_EARLY_SIMPLE_TEXT_OUTPUT_PPI_H__
#define __BYO_EARLY_SIMPLE_TEXT_OUTPUT_PPI_H__

#include <Base.h>

typedef struct _BYO_EARLY_SIMPLE_TEXT_OUTPUT BYO_EARLY_SIMPLE_TEXT_OUTPUT;

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_STRING)(
  IN  UINT16                          Column,
  IN  UINT16                          Row,
  IN  CHAR8                           *String
  );

/**
  Sets the output device(s) to a specified mode.

  @param  This       The protocol instance pointer.
  @param  ModeNumber The mode to set.
  OPT_MODE_STRUCT OptModeTable[] = {
    {"mode3+" ,       720,  400,  1},	
    {"640x480x8" ,    640,  480,  8},
    {"640x480x16",    640,  480, 16},
    {"640x480x32",    640,  480, 32},
    {"800x600x8" ,    800,  600,  8},
    {"800x600x16",    800,  600, 16},
    {"800x600x32",    800,  600, 32},
    {"1024x768x8",   1024,  768,  8},
    {"1024x768x16",  1024,  768, 16},
    {"1024x768x32",  1024,  768, 32}, // default mode
    {"1280x1024x8",  1280, 1024,  8},
    {"1280x1024x16", 1280, 1024, 16},
    {"1280x1024x32", 1280, 1024, 32},
    {"1600x1200x8",  1600, 1200,  8},
    {"1600x1200x16", 1600, 1200, 16},
    {"1600x1200x32", 1600, 1200, 32},
    {"end",1600,1200,  32},
  };
  
  @retval EFI_SUCCESS      The requested text mode was set.
  @retval EFI_DEVICE_ERROR The device had an error and could not complete the request.
  @retval EFI_UNSUPPORTED  The mode number was not valid.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_MODE)(
  IN UINT32                               ModeNumber
  );

/**
  Sets the background and foreground colors for the OutputString ()
  @param  This      The protocol instance pointer.
  @param  Attribute The attribute to set. Bits 0..3 are the foreground color, and
                    bits 4..6 are the background color. All other bits are undefined
                    and must be zero. The valid Attributes are defined in this file.

  @retval EFI_SUCCESS       The attribute was set.
  @retval EFI_DEVICE_ERROR  The device had an error and could not complete the request.
  @retval EFI_UNSUPPORTED   The attribute requested is not defined.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_ATTRIBUTE)(
  IN UINT32      Attribute
  );

/**
  @par Data Structure Description:
  Mode Structure pointed to by Simple Text Out protocol.
**/
typedef struct {
  UINT32   ModeNumber;
  UINT32   Horizontal;
  UINT32   Vertical;
} EFI_SIMPLE_TEXT_OUTPUT_MODE;


struct _BYO_EARLY_SIMPLE_TEXT_OUTPUT{
  EFI_TEXT_STRING               OutputString;
  EFI_TEXT_SET_ATTRIBUTE        SetAttribute;
  EFI_TEXT_SET_MODE             SetMode;
  EFI_SIMPLE_TEXT_OUTPUT_MODE   Mode;
};

extern EFI_GUID gByoEarlySimpleTextOutPpiGuid;


#endif
