/** @file

Copyright (c) 2006 - 2011, Byosoft Corporation.<BR> 
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced, 
stored in a retrieval system, or transmitted in any form or by any 
means without the express written consent of Byosoft Corporation.

File Name:
  tcm.asl

Abstract: 
  acpi asl file for tcm module.

Revision History:

Bug 3075 - Add TCM support.
TIME: 2011-11-14
$AUTHOR: Zhang Lin
$REVIEWERS: 
$SCOPE: SugarBay
$TECHNICAL: 
  1. Tcm module init version.
     Only support setup function.
$END--------------------------------------------------------------------

**/

DefinitionBlock (
  "Tcm.aml",
  "SSDT",
  2,
  "INTEL ",
  "TcmTable",
  0x1000
  )
{
  Scope (\_SB)
  {
    Device (TCM)
    {
      Method(_HID, 0){
        
        Store (TID, Local1)
        
        if(LEqual(Local1, 0x00011B4E)){
          return(EISAID("ZIC0101"))
        }
        if(LEqual(Local1, 0x02011B4E)){
          return(EISAID("ZIC0201"))
        }
        if(LEqual(Local1, 0x06011B4E)){
          return(EISAID("ZIC0601"))
        }  
        
        return(EISAID("XXX0000"))
      }

      Name(_STR, Unicode("TCM 1.2 Device"))
      Name(_CRS, ResourceTemplate(){
      Memory32Fixed(ReadWrite, FixedPcdGet64 (PcdTpmBaseAddress), 0x5000)
      })

      OperationRegion(TCMR, SystemMemory, FixedPcdGet64 (PcdTpmBaseAddress), 0x5000)
      Field(TCMR, AnyAcc, NoLock, Preserve){
        ACC0, 8,
        Offset(0xF00),
        TID, 32,
      }

      Method(_STA, 0){
      
        Store (TID, Local1)
        
        if(LEqual(Local1, 0x00011B4E)){
          return(0xF)
        }
        if(LEqual(Local1, 0x02011B4E)){
          return(0xF)
        }
        if(LEqual(Local1, 0x06011B4E)){
          return(0xF)
        }
        
        return(0)
      }
    }      
  }
}
