#ifndef __BYO_HYGON_GPIO_LIB_H__
#define __BYO_HYGON_GPIO_LIB_H__


#define BYO_GPIO_DEF_PIN(socket, die, gpio) \
  ((socket << 13) + (die << 11) + gpio)

#define BYO_GPIO_DEF_SET(function, output, pullup) \
  ((function <= 3 ? 0x800 : 0) + (function << 8) + \
    (output < 3 ? (output < 2 ? output : 0x02) : 0x08) + \
    ((pullup == 0 ? 0x40 : 0) + (pullup == 1 ? 0x20 : 0) + 0x80))

VOID
LibCpmSetGpio (
  IN       UINT16   Pin,
  IN       UINT16   Value
  );

UINT8
LibCpmGetGpio (
  IN       UINT16   Pin
  );

#endif