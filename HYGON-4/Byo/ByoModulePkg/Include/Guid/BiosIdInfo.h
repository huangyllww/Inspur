/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BIOS_ID_INFO_H__
#define __BIOS_ID_INFO_H__


#pragma pack(1)

typedef struct {
  CHAR16  BoardId[7];               // "TNAPCRB"
  CHAR16  BoardRev;                 // "1"
  CHAR16  Dot1;                     // "."
  CHAR16  OemId[3];                 // "86C"
  CHAR16  Dot2;                     // "."
  CHAR16  BuildType;                // "D"
  CHAR16  VersionMajor[4];          // "0008"
  CHAR16  Dot4;                     // "."
  CHAR16  TimeStamp[10];            // "YYMMDDHHMM"
  CHAR16  NullTerminator;           // 0x0000
} BIOS_ID_STRING;


#define BIOS_ID_SIGN_DATA  {'$', 'I', 'B', 'I', 'O', 'S', 'I', '$'}

//
// A signature precedes the BIOS ID string in the FV to enable search by external tools.
//
typedef struct {
  UINT8           Signature[8];     // "$IBIOSI$"
  BIOS_ID_STRING  BiosIdString;     // "TNAPCRB1.86C.D0008.1106141018"
} BIOS_ID_IMAGE;

#pragma pack()

#define BIOS_ID_INFO_SIGN     SIGNATURE_32('B', 'I', 'I', 'F')

typedef struct {
  UINT32 Signature;
  CHAR8  BiosVer[20];
  CHAR8  BiosDate[11];
  CHAR8  BoardId[16];     // setup may be required for a special name, so give a enough buffer.
  UINT8  VerMajor;
  UINT8  VerMinor;
  UINT32 Feature;
  UINT32 Ssid;
  CHAR16 RecoveryFilePath[32];
  CHAR16 RecoveryFileName[12+1];
  CHAR16 RecoverySignFileName[12+1];
  BIOS_ID_IMAGE  BiosId;
} BIOS_ID_INFO;

// sometimes, board id in bios id may not be same as the board name printed on the board,
// so here give a convert table.
typedef struct {
  CHAR16 BoardId[7+1];
  CHAR16 ExtId[3+1];
  CHAR8  BoardName[16+1];
} BIOS_ID_BOARD_ID;

#define EFI_BIOS_ID_GUID \
{ 0xC3E36D09, 0x8294, 0x4b97, 0xA8, 0x57, 0xD5, 0x28, 0x8F, 0xE3, 0x3E, 0x28 }


extern EFI_GUID gEfiBiosIdGuid;

extern EFI_GUID gBiosIdHobGuid;

#endif
