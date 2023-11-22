/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef _BYO_LEGACY_H_
#define _BYO_LEGACY_H_

//
// Low stub lay out
//
#define FIRST_INSTANCE      1
#define NOT_FIRST_INSTANCE  0

#pragma pack(1)
//
// PnP Expansion Header
//
typedef struct {
  UINT32  PnpSignature;
  UINT8   Revision;
  UINT8   Length;
  UINT16  NextHeader;
  UINT8   Reserved1;
  UINT8   Checksum;
  UINT32  DeviceId;
  UINT16  MfgPointer;
  UINT16  ProductNamePointer;
  UINT8   Class;
  UINT8   SubClass;
  UINT8   Interface;
  UINT8   DeviceIndicators;
  UINT16  Bcv;
  UINT16  DisconnectVector;
  UINT16  Bev;
  UINT16  Reserved2;
  UINT16  StaticResourceVector;
} LEGACY_PNP_EXPANSION_HEADER;
#pragma pack()

#endif