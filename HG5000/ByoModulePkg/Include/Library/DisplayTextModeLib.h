#ifndef _DISPLAY_TEXTMODE_H_
#define _DISPLAY_TEXTMODE_H_

static UINT8 gEraseCountLine = 0;

VOID
Displaytextmodemessage (
  );

VOID
SwapWordArray (
  IN  UINT8   *Data,
  IN  UINTN   DataSize
  );

#endif
