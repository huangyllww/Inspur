
#ifndef __SETUP_GUI_CUSTOM_H__
#define __SETUP_GUI_CUSTOM_H__

#pragma pack(1)

//
// Type
//
#define BYO_IFR_TYPE_GRID_START                    0x01
#define BYO_IFR_TYPE_GRID_END                      0x02
#define BYO_IFR_TYPE_FONT_COLOUR                   0x03
#define BYO_IFR_TYPE_FONT_SIZE                     0x04
#define BYO_IFR_TYPE_HELP_SHOW                     0x05

//
// OpCode
//
#define BYO_IFR_OPCODE_SUBTITLE                    0x02
#define BYO_IFR_OPCODE_ACTION                      0x0C

//
// Attribute hide
//
// Notice:
// Action OpCode : default hide
//
#define BYO_IFR_HELP_SHOW                          0x00
#define BYO_IFR_HELP_HIDE                          0x01

//
// Attribute Align
//
#define BYO_IFR_ALIGN_AUTO                         0x00
#define BYO_IFR_ALIGN_LEFT                         0x01
         
//
// Attribute Colour
//
#define BYO_IFR_COLOUR_BLACK                       0x00
#define BYO_IFR_COLOUR_BLUE                        0x01
#define BYO_IFR_COLOUR_GREEN                       0x02
#define BYO_IFR_COLOUR_CYAN                        0x03
#define BYO_IFR_COLOUR_RED                         0x04
#define BYO_IFR_COLOUR_MAGENTA                     0x05
#define BYO_IFR_COLOUR_BROWN                       0x06
#define BYO_IFR_COLOUR_LIGHTGRAY                   0x07
#define BYO_IFR_COLOUR_BRIGHT                      0x08
#define BYO_IFR_COLOUR_DARKGRAY                    0x08
#define BYO_IFR_COLOUR_LIGHTBLUE                   0x09
#define BYO_IFR_COLOUR_LIGHTGREEN                  0x0A
#define BYO_IFR_COLOUR_LIGHTCYAN                   0x0B
#define BYO_IFR_COLOUR_LIGHTRED                    0x0C
#define BYO_IFR_COLOUR_LIGHTMAGENTA                0x0D
#define BYO_IFR_COLOUR_YELLOW                      0x0E
#define BYO_IFR_COLOUR_WHITE                       0x0F

typedef struct {
  UINT16              Type;
  UINT16              OpCode;
  UINT16              Attribute;
} BYO_GUI_IFR_GUID_DESCRIPTION;

#ifndef VFRCOMPILE
typedef struct {
  EFI_IFR_OP_HEADER            Header;
  EFI_GUID                     Guid;
  BYO_GUI_IFR_GUID_DESCRIPTION Data;
} BYO_GUI_IFR_GUID_DESCRIPTION_FULL;
#endif

#pragma pack()

#define BYO_GUI_IFR_DESCRIPTION_GUID \
  { \
    0x248b5348, 0x442c, 0x4a26, {0xa7, 0x8e, 0x60, 0x44, 0xfc, 0x4b, 0x64, 0xbc} \
  }

#endif
