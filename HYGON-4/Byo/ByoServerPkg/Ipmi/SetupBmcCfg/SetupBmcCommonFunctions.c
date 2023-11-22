/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SetupBmcCommonFunctions.c

Revision History:

**/

#include "SetupBmcCfg.h"
#include <Library/PcdLib.h>
//
// Length of temp string buffer to store value string.
//
#define CHARACTER_NUMBER_FOR_VALUE  30

EFI_STATUS
Ipv6FirstCheck (
  IN OUT CHAR16             *String
  );
  
#if 0
UINT8
IsRmmPresent (
  VOID
  )
/*++

Routine Description:

  Check for RMM presence 

Arguments:

  None

Returns:

  TRUE - If RMM present

--*/
{
  EFI_STATUS                  Status;
  UINT8                       Size;
  EFI_GET_RMM3_INFO_RESPONSE  GetRMM3InfoResponse;
  EFI_GET_RMM3_INFO           GetRMM3Info;

  GetRMM3Info.Flags             = 0x00;
  GetRMM3Info.ParameterSelector = 0x01;
  //
  // Paramater 1 (Intel RMM3 Info)
  //
  GetRMM3Info.SetSelector   = 0x00;
  GetRMM3Info.BlockSelector = 0x00;
  Size                      = sizeof (EFI_GET_RMM3_INFO_RESPONSE);

  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_GENERAL_APPLICATION,             // NetFunction
            EFI_PLATFORM_GET_ADDIN_SUPPORT_CONFIGURATION, // Command
            (UINT8 *) &GetRMM3Info,                       // *CommandData
            sizeof (EFI_GET_RMM3_INFO),                   // CommandDataSize
            (UINT8 *) (&GetRMM3InfoResponse),             // *ResponseData
            (UINT8 *) &Size                               // *ResponseDataSize              );
            );

  if ((Status == EFI_SUCCESS) && (GetRMM3InfoResponse.TempData2 & 0x02)) {
    return TRUE;
//
//  RMM4 present
//
  } else {
    return FALSE;
  }
}
#endif

UINT8
IsNicPresent (
  VOID
  )
/*++

Routine Description:

  Check for NIC presence 

Arguments:

  None

Returns:

  TRUE - If NIC present

--*/
{
    return TRUE;
}

UINTN
MemCmp (
  IN UINT8 *dest,
  IN UINT8 *src,
  IN UINTN size
  )
/*++
Routine Description:
  Compare Memory contents

Arguments:
  dest - Destination memory Address.
  src  - Source Memory Address.
  size - Length of Memory location to compare.
  
Returns:
  TRUE- If Buffers are same
  FALSE-If Buffers are not same
 
--*/
{
  UINTN i;

  for (i = 0; i < size; i++) {
    if (dest[i] != src[i]) {
      return FALSE;
    }
  }

  return TRUE;

}

void
MemCpy (
  IN void*dst,
  IN void*src,
  IN int sz
  )
/*++

Routine Description:

  Copy memory buffer

Arguments:

  dst - Destination memory location
  src - Source
  sz  - Size

Returns:
  
  None

--*/
{
  UINT8 *pdst;
  UINT8 *psrc;
  pdst  = (UINT8 *) dst;
  psrc  = (UINT8 *) src;

  for (; sz != 0; --sz) {
    *pdst++ = *psrc++;
  }
}

VOID
AsciiToUnicode (
  IN      CHAR8     *AsciiString,
  IN OUT  CHAR16    *UnicodeString
  )
/*++

Routine Description:

  Converts an ascii string to unicode string.

Arguments:

  AsciiString - ASCII String to Convert
  UnicodeString - Converted Unicode String 

Returns:
  UnicodeString - Converted Unicode String 
--*/
{
  UINT8 Index;

  for (Index = 0; AsciiString[Index] != 0; Index++) {
    UnicodeString[Index] = (CHAR16) AsciiString[Index];
  }

  return ;
}

CHAR8 *
Unicode2Ascii (
  OUT CHAR8          *AsciiStr,
  IN  CHAR16         *UnicodeStr
  )
/*++
  
  Routine Description:

    This function converts Unicode string to ASCII string.
  
  Arguments:

    AsciiStr      - Converted ASCII string
    UnicodeStr     - Unicode String to convert
 
  Returns: 

    Start of the ASCII ouput string.
    
--*/
{
  CHAR8 *Str;
  Str = AsciiStr;

  while (TRUE) {
    *(AsciiStr++) = (CHAR8) *UnicodeStr;
    if (*(UnicodeStr++) == '\0') {
      return Str;
    }
  }
}

STATIC
EFI_STATUS
EfiStringToValue (
  OUT UINT64        *Val,
  IN  CHAR16        *String,
  OUT UINT8         *EndIdx OPTIONAL
  )
/*++

Routine Description:
  Parses and converts Unicode string to decimal value.
  The returned value is 64-bit.
  The string is expected in decimal format,
  the string is parsed and format verified.
  This function is missing from the libs. One day it maybe
  replaced with a lib function when it'll become available.

Arguments:
  Val    - pointer to the variable to store the value to
  String - string that contains the value to parse and convert
  EndIdx - index on which the parsing stopped. It points to the
           first character that was not part of the returned Val.
           It's valid only if the function returns success.
           It's optional and it could be NULL.

Returns:
  EFI_SUCCESS           - if successful
  EFI_INVALID_PARAMETER - if String is in unexpected format

--*/
{
  UINT8   i;
  UINT64  TempVal;

  TempVal = 0;
  //
  // Iterate upto 20 digits, only so many could fit in the UINT64
  //
  for (i = 0; i <= 20; i++) {
    //
    // test if the next character is not a digit
    //
    if (!IS_DIGIT (String[i])) {
      //
      // If here, there is no more digits,
      // return with success if there was at least one to process
      //
      if (i == 0) {
        break;
      }

      *Val = TempVal;

      if (EndIdx != NULL) {
        *EndIdx = i;
      }

      return EFI_SUCCESS;
    }
    //
    // If here, there is a digit to process
    //
    TempVal = MultU64x32 (TempVal, 10);
    TempVal += String[i] - L"0"[0];
  }
  //
  // if here, there was some sort of format error
  //
  return EFI_INVALID_PARAMETER;
}

static CHAR16 mHexStr[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7',
                            L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' };

UINTN
EfiValueToHexStr (
  IN  OUT CHAR16  *Buffer, 
  IN  UINT64      Value, 
  IN  UINTN       Flags, 
  IN  UINTN       Width
  )
/*++

Routine Description:

  VSPrint worker function that prints a Value as a hex number in Buffer

Arguments:

  Buffer - Location to place ascii hex string of Value.

  Value  - Hex value to convert to a string in Buffer.

  Flags  - Flags to use in printing Hex string, see file header for details.

  Width  - Width of hex value.

Returns: 

  Number of characters printed.  

--*/
{
  CHAR16  TempBuffer[CHARACTER_NUMBER_FOR_VALUE];
  CHAR16  *TempStr;
  CHAR16  Prefix;
  CHAR16  *BufferPtr;
  UINTN   Count;
  UINTN   Index;

  TempStr   = TempBuffer;
  BufferPtr = Buffer;

  //
  // Count starts at one since we will null terminate. Each iteration of the
  // loop picks off one nibble. Oh yea TempStr ends up backwards
  //
  Count = 0;
  
  if (Width > CHARACTER_NUMBER_FOR_VALUE - 1) {
    Width = CHARACTER_NUMBER_FOR_VALUE - 1;
  }

  do {
    Index = ((UINTN)Value & 0xf);
    *(TempStr++) = mHexStr[Index];
    Value = RShiftU64 (Value, 4);
    Count++;
  } while (Value != 0);

  if (Flags & PREFIX_ZERO) {
    Prefix = '0';
  } else { 
    Prefix = ' ';
  }

  Index = Count;
  if (!(Flags & LEFT_JUSTIFY)) {
    for (; Index < Width; Index++) {
      *(TempStr++) = Prefix;
    }
  }

  //
  // Reverse temp string into Buffer.
  //
  if (Width > 0 && (UINTN) (TempStr - TempBuffer) > Width) {
    TempStr = TempBuffer + Width;
  }
  Index = 0;
  while (TempStr != TempBuffer) {
    *(BufferPtr++) = *(--TempStr);
    Index++;
  }
    
  *BufferPtr = 0;
  return Index;
}

BOOLEAN
IsHexDigit (
  OUT UINT8      *Digit,
  IN  CHAR16      Char
  )
/*++

  Routine Description:
    Determines if a Unicode character is a hexadecimal digit.
    The test is case insensitive.

  Arguments:
    Digit - Pointer to byte that receives the value of the hex character.
    Char  - Unicode character to test.

  Returns:
    TRUE  - If the character is a hexadecimal digit.
    FALSE - Otherwise.

--*/
{
  if ((Char >= L'0') && (Char <= L'9')) {
    *Digit = (UINT8) (Char - L'0');
    return TRUE;
  }

  if ((Char >= L'A') && (Char <= L'F')) {
    *Digit = (UINT8) (Char - L'A' + 0x0A);
    return TRUE;
  }

  if ((Char >= L'a') && (Char <= L'f')) {
    *Digit = (UINT8) (Char - L'a' + 0x0A);
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
HexStringToBuf (
  IN OUT UINT8                     *Buf,   
  IN OUT UINTN                    *Len,
  IN     CHAR16                    *Str,
  OUT    UINTN                     *ConvertedStrLen  OPTIONAL
  )
/*++

  Routine Description:
    Converts Unicode string to binary buffer.
    The conversion may be partial.
    The first character in the string that is not hex digit stops the conversion.
    At a minimum, any blob of data could be represented as a hex string.

  Arguments:
    Buf    - Pointer to buffer that receives the data.
    Len    - Length in bytes of the buffer to hold converted data.
                If routine return with EFI_SUCCESS, containing length of converted data.
                If routine return with EFI_BUFFER_TOO_SMALL, containing length of buffer desired.
    Str    - String to be converted from.
    ConvertedStrLen - Length of the Hex String consumed.

  Returns:
    EFI_SUCCESS: Routine Success.
    EFI_BUFFER_TOO_SMALL: The buffer is too small to hold converted data.
    EFI_

--*/
{
  UINTN       HexCnt;
  UINTN       Idx;
  UINTN       BufferLength;
  UINT8       Digit;
  UINT8       Byte;

  //
  // Find out how many hex characters the string has.
  //
  for (Idx = 0, HexCnt = 0; IsHexDigit (&Digit, Str[Idx]); Idx++, HexCnt++);

  if (HexCnt == 0) {
    *Len = 0;
    return EFI_SUCCESS;
  }
  //
  // Two Unicode characters make up 1 buffer byte. Round up.
  //
  BufferLength = (HexCnt + 1) / 2; 

  //
  // Test if  buffer is passed enough.
  //
  if (BufferLength > (*Len)) {
    *Len = BufferLength;
    return EFI_BUFFER_TOO_SMALL;
  }

  *Len = BufferLength;

  for (Idx = 0; Idx < HexCnt; Idx++) {

    IsHexDigit (&Digit, Str[HexCnt - 1 - Idx]);

    //
    // For odd characters, write the lower nibble for each buffer byte,
    // and for even characters, the upper nibble.
    //
    if ((Idx & 1) == 0) {
      Byte = Digit;
    } else {
      Byte = Buf[Idx / 2];
      Byte &= 0x0F;
      Byte = (UINT8)(Byte | (Digit << 4));
    }

    Buf[Idx / 2] = Byte;
  }

  if (ConvertedStrLen != NULL) {
    *ConvertedStrLen = HexCnt;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
StrStringToIp4Adr (
  OUT EFI_IPv4_ADDRESS   *Ip4Addr,
  IN  CHAR16             *String
  )
/*++

Routine Description:
  Parses and converts Unicode string to IP v4 address.
  The value will 64-bit.
  The string must be four decimal values 0-255 separated by dots.
  The string is parsed and format verified.

Arguments:
  Ip4Addr - pointer to the variable to store the value to
  String  - string that contains the value to parse and convert

Returns:
  EFI_SUCCESS           - if successful
  EFI_INVALID_PARAMETER - if String contains invalid IP v4 format

--*/
{
  EFI_STATUS        Status;

  EFI_IPv4_ADDRESS  RetVal;
  UINT64            TempVal;
  UINT8             Idx;
  UINT8             i;
  Idx = 0;
  TempVal = 0;
  //
  // Iterate the decimal values separated by dots
  //
  for (i = 0; i < 4; i++) {
    //
    // get the value of a decimal
    //
    Status = EfiStringToValue (&TempVal, String, &Idx);
    if ((EFI_ERROR (Status)) || (TempVal > 255)) {
      break;
    }

    RetVal.Addr[i] = (UINT8) TempVal;
    String += Idx;

    //
    // test if it is the last decimal value
    //
    if (i == 3) {
      if (String[0] != L'\0') {
        //
        // the string must end with string termination character
        //
        break;
      }
      *Ip4Addr = RetVal;
      return EFI_SUCCESS;
    }
    //
    // Test for presence of a dot, it is required between the values
    //
    if (String++[0] != L'.') {
      break;
    }
  }

  return EFI_INVALID_PARAMETER;
}
EFI_STATUS
IpAddtoStr (
  IN OUT CHAR16        *Buffer,
  IN     UINT8         *Ipadd
  )
/*++

Routine Description:

  Converts IP Address Buffer to String

Arguments:

  Buffer  - GC_TODO: add argument description
  Ipadd   - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{ 
  UnicodeSPrint (Buffer, 2 * 16, L"%d.%d.%d.%d", Ipadd[0], Ipadd[1], Ipadd[2], Ipadd[3]);
  return EFI_SUCCESS;
}

EFI_STATUS
Ipv6AddtoStr (
  IN OUT CHAR16        *Buffer,
  IN     UINT8         *Ipadd,
  IN     UINTN         Size
  )
/*++

Routine Description:

  Converts IPV6 Address Buffer to String

Arguments:

  Buffer  - GC_TODO: add argument description
  Ipadd   - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  UINTN Length;
  UINTN Ptr;
  UINTN BufferLen;

  Ptr     = 0;
  BufferLen = Size/sizeof(CHAR16);

 //
//In flag value is 0x20 to append character zero and 2 for specifying 2 digit value.
//
  Length  = EfiValueToHexStr (&Buffer[0], Ipadd[0], 0x20, 2);
  Ptr     = Ptr + Length;
 

 
  Length  = EfiValueToHexStr (&Buffer[Ptr], Ipadd[1], 0x20, 2);
  Ptr     = Ptr + Length;
  StrCpyS (&Buffer[Ptr], BufferLen-Ptr,  L":");
  Ptr++;
 
   Length  = EfiValueToHexStr (&Buffer[Ptr], Ipadd[2], 0x20, 2);
  Ptr = Ptr + Length;
  
  
  Length  = EfiValueToHexStr (&Buffer[Ptr], Ipadd[3], 0x20, 2);
  Ptr = Ptr + Length;
  StrCpyS (&Buffer[Ptr], BufferLen-Ptr, L":");
  Ptr++;

 
 Length  =  EfiValueToHexStr (&Buffer[Ptr], Ipadd[4], 0x20, 2);
  Ptr = Ptr + Length;
  

 
  Length  =  EfiValueToHexStr (&Buffer[Ptr], Ipadd[5], 0x20, 2);
  Ptr = Ptr + Length;
  StrCpyS (&Buffer[Ptr], BufferLen-Ptr, L":");
  Ptr++;
 
  Length  =  EfiValueToHexStr (&Buffer[Ptr], Ipadd[6], 0x20, 2);
  Ptr = Ptr + Length;
  
  
 
  Length  =   EfiValueToHexStr (&Buffer[Ptr], Ipadd[7], 0x20, 2);
  Ptr = Ptr + Length;
  StrCpyS (&Buffer[Ptr], BufferLen-Ptr, L":");
  Ptr++;
  
  Length  = EfiValueToHexStr (&Buffer[Ptr], Ipadd[8], 0x20, 2);
  Ptr     = Ptr + Length;
 

 
  Length  = EfiValueToHexStr (&Buffer[Ptr], Ipadd[9], 0x20, 2);
  Ptr     = Ptr + Length;
  StrCpyS (&Buffer[Ptr], BufferLen-Ptr, L":");
  Ptr++;
  
   Length  = EfiValueToHexStr (&Buffer[Ptr], Ipadd[10], 0x20, 2);
  Ptr = Ptr + Length;
  
  
  
  Length  = EfiValueToHexStr (&Buffer[Ptr], Ipadd[11], 0x20, 2);
  Ptr = Ptr + Length;
  StrCpyS (&Buffer[Ptr], BufferLen-Ptr, L":");
  Ptr++;
 
 Length  =  EfiValueToHexStr (&Buffer[Ptr], Ipadd[12], 0x20, 2);
  Ptr = Ptr + Length;
 

 
  Length  =  EfiValueToHexStr (&Buffer[Ptr], Ipadd[13], 0x20, 2);
  Ptr = Ptr + Length;
  StrCpyS (&Buffer[Ptr], BufferLen-Ptr, L":");
  Ptr++;
  
  Length  =  EfiValueToHexStr (&Buffer[Ptr], Ipadd[14], 0x20, 2);
  Ptr = Ptr + Length;
  

 
  Length  =   EfiValueToHexStr (&Buffer[Ptr], Ipadd[15], 0x20, 2);

  return EFI_SUCCESS;
}

VOID
EfiLibSafeFreePool (
  IN  VOID             *Buffer
  )
/*++

Routine Description:

  Free pool safely (without setting back Buffer to NULL).

Arguments:
  
  Buffer          - The allocated pool entry to free

Returns:

  Pointer of the buffer allocated.

--*/
{
  if (Buffer != NULL) {
    gBS->FreePool (Buffer);
  }
}

EFI_STATUS
StrStringToIp6Adr (
  OUT EFI_IPv6_ADDRESS   *Ip6Addr,
  IN  CHAR16             *String
  )
{
   CHAR16  *TempString;
   CHAR16   FormatString[4]={0,0,0,0};
   UINT8 i=0;
   UINT8 j=0;
   UINT8 k=0;
   UINT8 l=0;
   UINT8 ip[16]={0,0,0,0,0,0};
   EFI_IPv6_ADDRESS TempIp={{0,0,0,0,0,0}};
   UINTN  CLen,Len=16;
   UINTN  SrcLen=0;
   UINT8 Addrcount=0;
   UINT8 Maxcount=4;
   UINT8 ColonCount=0;
   UINT8 Tempcount=0;

   EFI_STATUS Status;
   //if there are '::' or 'xxx.xxx.xxx.xxx' or xxxx:xxxxxxx:xxxx...appear,check it first.    
   Status = Ipv6FirstCheck(String);

   if(Status == EFI_INVALID_PARAMETER){
    return EFI_INVALID_PARAMETER;
   }
   TempString = AllocateZeroPool(33 * sizeof(CHAR16));
   SrcLen = StrLen (String);
   for(i=0;i<SrcLen;i++){
     if(String[i] == L':') {
       ColonCount++;
     }
   }
   for(i=0;i<SrcLen;i=i+1){

       if( ((String[i]>=L"0"[0])&&(String[i]<=L"9"[0]))
      ||((String[i]>=L"A"[0])&&(String[i]<=L"F"[0]))
      ||((String[i]>=L"a"[0])&&(String[i]<=L"f"[0])) ){
          FormatString[Addrcount++] = String[i];
      continue;
       }else if((String[i]== L':')&&(Addrcount!= 0)){
         Tempcount=Maxcount-Addrcount;
         for(k=0;k<Tempcount;k++){
           if(31<j){
             break;
           }
           TempString[j]= L'0';
           j=j+1;
         }
      Tempcount=0;
      for(k=0;k<Addrcount;k++){
         if(31<j){
           break;
         }
         TempString[j++]= FormatString[k]; 
         }
      k=0;    
      Addrcount=0;
        
       }
    else if((String[i]== L':')&&(Addrcount == 0)){
      for (l=0;l< (8-ColonCount);l++) {
        for(k=0;k<4;k++){
         if(31<j){
           break;
         }
         TempString[j]= L'0';
         j=j+1;
         }     
      }
     }
    else{
      FreePool(TempString);
      return EFI_INVALID_PARAMETER;
    }
  }
  //
  //copying last part of IPV6 address
  //
  if (Addrcount !=0) {
         Tempcount=Maxcount-Addrcount;
         for(k=0;k<Tempcount;k++){
           if(31<j){
             break;
           }
           TempString[j]= L'0';
           j=j+1;
         }
          Tempcount=0;
     for(k=0;k<Addrcount;k++){
         if(31<j){
           break;
         }
         TempString[j++]= FormatString[k]; 
         }
      k=0;    
      Addrcount=0;
  }

  //
  //Calculate Length  of IPV6 address
  //
  Len = StrLen (TempString);
  //
  //Get Hex values from IP string
  //
  HexStringToBuf(&ip[0],&Len,TempString,&CLen);
   j=15;
   //
   //Traverse it back to get proper values.
   //
   for(i=0;i<=15;i=i+1){
   TempIp.Addr[i]=ip[j];
   --j;
    }
  *Ip6Addr= TempIp;
   FreePool(TempString);
   return EFI_SUCCESS;
 }

EFI_STATUS
Ipv6FirstCheck (
  IN OUT CHAR16             *String
  )
{
    CHAR8     String1[46];
    UINT8     SerColonCnt = 0;     //log the total number of "::" 
    UINT8     ColonCnt = 0;       //log the total number of ':'
    UINT8     DotCnt = 0;         //log the total number of '.'     
    UINTN     SrcLen = 0;     
    UINT8     i = 0;
    UINT8     j = 0;
    UINT8     LastColon = 0;      //the last suffix of ':'
    UINT8    Dot[3]={0,0,0};          //the suffix of '.'
    UINT16     Ipv4Adr[4];       
    CHAR8     TempS[10];       
    BOOLEAN     Flag = TRUE;

    SrcLen = StrLen(String);
/*
    Step 1. 
        if the number of serial colon >= 2,return invalid.    
        (eg. if input 2001:2002:::2003,return invalid)
        if the total number of "::" >= 2,return invalid.    
        (eg. if input 2001::2002:2003::2004,return invalid.)
*/
    for(i=0;i<SrcLen-2;i++){
        if(String[i] == L':' && String[i+1] == L':' && String[i+2] == L':'){
            return EFI_INVALID_PARAMETER;
        }
    }

    for(i=0;i<SrcLen-1;i++){
        if(String[i] == L':' && String[i+1] == L':'){
            SerColonCnt++;
        }
        if(SerColonCnt >= 2){
            return EFI_INVALID_PARAMETER;
        }
    }

/*
    Step 2. 
        check whether the number of '.' and ':' are valid
*/

    for(i=0;i<SrcLen;i++){
        if(String[i] == L':'){
            ColonCnt++;
        }
        if(String[i] == L'.'){
            DotCnt++;
        }
    }

    if((DotCnt != 0 && DotCnt < 3) || (DotCnt > 3)){
        return EFI_INVALID_PARAMETER;
    }

    if(ColonCnt > 7 || ColonCnt < 1){
        return EFI_INVALID_PARAMETER;
    }

    if(SerColonCnt == 0 && DotCnt == 0 && ColonCnt != 7){
        return EFI_INVALID_PARAMETER;
    }
    if(SerColonCnt == 0 && DotCnt == 3 && ColonCnt != 6){
        return EFI_INVALID_PARAMETER;
    }
    if(SerColonCnt == 1 && DotCnt == 3 && ColonCnt == 7){
        return EFI_INVALID_PARAMETER;
    }

/*
    Step 3. 
        if xxxx:xxxxxx:xxxx...appear,return invalid
        (eg.if input 2001:200002:2003::2004,return invalid)
*/
    for(i=0;i<SrcLen;i++){
        if(String[i] == L':'){
            if(String[i+1] == L':'){
                continue;
            }
            Flag = FALSE;
            for(j=i+1;j<=i+4;j++){
                if(String[j] == L'.' || String[j] == L':' || String[j] == L'\0' || String[j+1] == L':' || String[j+1] == L'\0'){
                    Flag = TRUE;
                    break;
                }
            }
            if(!Flag){
                return EFI_INVALID_PARAMETER;
            }
        }
    }

/*
    Step 4. 
        if input 2002:2003::10.11.12.13,convert it to IPv6 format.
        (eg.if input 2002:2003::10.11.12.13,convert it to2002:2003::0A0B:0C0D)
*/

    if(DotCnt == 3){
        for(i=0,j=0;i<SrcLen;i++){
            if(String[i] == L':'){
                LastColon = i;      
            }
            if(String[i] == L'.'){
                Dot[j] = i;     
                j++;
            }
        }

        if(Dot[0] < LastColon){
            return EFI_INVALID_PARAMETER;
        }

        for(i=LastColon+1;i<SrcLen;i++){
            if((String[i] >= L'0' && String[i] <= L'9') || String[i] == L'.'){
                continue;
            }else{
                return EFI_INVALID_PARAMETER;
            }
        }

        if((Dot[0] - LastColon < 2 || Dot[0] - LastColon > 4) ||
           (Dot[1] - Dot[0]    < 2 || Dot[1] - Dot[0]    > 4) ||
           (Dot[2] - Dot[1]    < 2 || Dot[2] - Dot[1]    > 4) ||
           (SrcLen - Dot[2]    < 2 || SrcLen - Dot[2]    > 4)){
            return EFI_INVALID_PARAMETER;  
        }

    Unicode2Ascii(String1,String);

    for(i=0;i<4;i++){
        Ipv4Adr[i] = 0;
    }        

        for(i=LastColon+1;i<Dot[0];i++){
            Ipv4Adr[0] = Ipv4Adr[0] * 10 + (String1[i] - 48);
        }
        for(i=Dot[0]+1;i<Dot[1];i++){
            Ipv4Adr[1] = Ipv4Adr[1] * 10 + (String1[i] - 48);
        }
        for(i=Dot[1]+1;i<Dot[2];i++){
            Ipv4Adr[2] = Ipv4Adr[2] * 10 + (String1[i] - 48);
        }
        for(i=Dot[2]+1;i<SrcLen;i++){
            Ipv4Adr[3] = Ipv4Adr[3] * 10 + (String1[i] - 48);
        }
        for(i=0;i<4;i++){
            if(Ipv4Adr[i] > 255){
                return EFI_INVALID_PARAMETER;  
            }
        }


        for(i=0;i<4;i++){
            if(i < 2){
                j = 2*i;
            }
            if(i >= 2){
                j = 2*i+1;
            }

            if(((Ipv4Adr[i] >> 4) & 0x0F) >= 0 &&
               ((Ipv4Adr[i] >> 4) & 0x0F) <= 9 ){
                TempS[j] = ((Ipv4Adr[i] >> 4) & 0x0F) + 48;
            }else{
                TempS[j] = ((Ipv4Adr[i] >> 4) & 0x0F) + 55;
            }

            if((Ipv4Adr[i] & 0x0F) >= 0 &&
               (Ipv4Adr[i] & 0x0F) <= 9 ){
                TempS[j+1] = (Ipv4Adr[i] & 0x0F) + 48;
            }else{
                TempS[j+1] = (Ipv4Adr[i] & 0x0F) + 55;
            }
        }
        TempS[4] = ':';
        TempS[9] = '\0';

        for(i=LastColon+1;i<=LastColon+10;i++){
            String1[i] = TempS[i-LastColon-1];
        }

    for (i = 0; String1[i] != '\0'; i++) {
            String[i] = (CHAR16) String1[i];
      }
    String[i] = L'\0';
    }
    return EFI_SUCCESS;
}

