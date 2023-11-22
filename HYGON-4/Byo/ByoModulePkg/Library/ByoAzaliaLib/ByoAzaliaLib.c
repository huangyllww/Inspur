/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <VerbTableHdr.h>

typedef union {
  struct {
    UINT32  Lower32;
    UINT32  Upper32;
  } Uint32;
  UINT64    Uint64;
} MY_DATA_64;


//-------------------------------- Azalia --------------------------------------
#define REG_HDA_GCAP            0x00      // 2
#define REG_HDA_VMIN            0x02      // 1
#define REG_HDA_VMAJ            0x03      // 1
#define REG_HDA_OUTPAY          0x04      // 2
#define REG_HDA_INPAY           0x06      // 2
#define REG_HDA_GCTL            0x08      // 4
#define   REG_HDA_GCTL_RESET      BIT0
#define REG_HDA_WAKEEN          0x0C      // 2
#define REG_HDA_STATESTS        0x0E      // 2
#define REG_HDA_GSTS            0x10      // 2
#define REG_HDA_IC              0x60      // 4
#define REG_HDA_IR              0x64      // 4
#define REG_HDA_IRS             0x68      // 4
#define   REG_HDA_IRS_BUSY        BIT0
#define   REG_HDA_IRS_VALID       BIT1
#define REG_HDA_OSD0CTL         0x100     // 3
#define REG_HDA_OSD0CBL         0x108     // 4
#define REG_HDA_OSD0LVI         0x10C     // 2
#define REG_HDA_OSD0FIFOS       0x110     // 2
#define REG_HDA_OSD0FMT         0x112     // 2
#define REG_HDA_OSD0BDPL        0x118     // 4
#define REG_HDA_OSD0BDPU        0x11C     // 4

#define ROOT_NODE_ID		                0x00

#define VERB_GET_PARAMETER  		        0xF00
#define PARAMETER_ID_VENDOR_ID          0x00

#define CREATE_VERB12_FORMAT(CAd, NodeID, VerbID, PayLoad) \
  (UINT32)((((CAd)&0x0F)<<28) | (((NodeID)&0xFF)<<20) | (((VerbID)&0xFFF)<<8) | ((PayLoad)&0xFF))

#define AZALIA_MAX_SID_NUMBER     2
#define AZALIA_MAX_SID_MASK       ((1 << AZALIA_MAX_SID_NUMBER) - 1)
#define AZALIA_MAX_LOOP_TIME      10
#define AZALIA_WAIT_PERIOD        100
#define AZALIA_RESET_WAIT_TIME    300

  
STATIC 
EFI_STATUS 
AzaliaSendCommand (
  UINTN   HdaBar, 
  UINT32  Verb, 
  UINT32  *Res  OPTIONAL
  )
{
  UINTN       TimeOut;
  UINTN       Address;
  EFI_STATUS  Status;

//DEBUG((EFI_D_INFO, "Verb:%08X\n", Verb));  
  Status  = EFI_SUCCESS;  
  TimeOut = AZALIA_MAX_LOOP_TIME;
  Address = HdaBar + REG_HDA_IRS;
  while(TimeOut--){
    if(!(MmioRead32(Address)&REG_HDA_IRS_BUSY)){
      break;
    }
    MicroSecondDelay(AZALIA_WAIT_PERIOD);
  }
  if(MmioRead32(Address)&REG_HDA_IRS_BUSY){
    Status = EFI_DEVICE_ERROR;
    goto ProcExit;
  }
        
  MmioWrite32(Address, MmioRead32(Address)|REG_HDA_IRS_VALID);  // clear
  MmioWrite32(HdaBar+REG_HDA_IC, Verb);
  MmioWrite32(Address, MmioRead32(Address)|REG_HDA_IRS_BUSY);   // start
  
  TimeOut = AZALIA_MAX_LOOP_TIME;
  while(TimeOut--){
    if(MmioRead32(Address)&REG_HDA_IRS_VALID){
      break;
    }
    MicroSecondDelay(AZALIA_WAIT_PERIOD);
  }
  if(!(MmioRead32(Address)&REG_HDA_IRS_VALID)){
    Status = EFI_DEVICE_ERROR;
    goto ProcExit;    
  }
  
  if(Res!=NULL){
    *Res = MmioRead32(HdaBar+REG_HDA_IR);
  }

ProcExit:
  return Status;    
}


STATIC EFI_STATUS AzaliaReset(UINTN HdaBar)
{
  UINTN       TimeOut;
  UINTN       Address;
  EFI_STATUS  Status;
  
  Status = EFI_SUCCESS;
  
// 1. clear STATESTS
  Address = HdaBar + REG_HDA_STATESTS;
  MmioOr8(Address, AZALIA_MAX_SID_MASK);

// 2. reset controller
  Address = HdaBar + REG_HDA_GCTL;
  MmioWrite32(Address, MmioRead32(Address) & ~REG_HDA_GCTL_RESET);
	TimeOut = AZALIA_MAX_LOOP_TIME;
	while((MmioRead32(Address)&REG_HDA_GCTL_RESET) && TimeOut--){
		MicroSecondDelay(AZALIA_WAIT_PERIOD);
	}
  if(MmioRead32(Address)&REG_HDA_GCTL_RESET){
    Status = EFI_DEVICE_ERROR;
    goto ProcExit;    
  }

  MicroSecondDelay(AZALIA_RESET_WAIT_TIME);  
  
// 3. Bring controller out of reset
  Address = HdaBar + REG_HDA_GCTL;
  MmioWrite32(Address, MmioRead32(Address) | REG_HDA_GCTL_RESET);
	TimeOut = AZALIA_MAX_LOOP_TIME;
	while((!(MmioRead32(Address)&REG_HDA_GCTL_RESET)) && TimeOut--){
		MicroSecondDelay(AZALIA_WAIT_PERIOD);
	}
  if(!(MmioRead32(Address)&REG_HDA_GCTL_RESET)){
    Status = EFI_DEVICE_ERROR;
    goto ProcExit;    
  }
  
ProcExit:
  return Status;
}



STATIC UINTN GetHdaBar(UINTN HostPcieAddr)
{
  MY_DATA_64  Data64;
  UINTN       HdaBar;

  Data64.Uint64 = 0;
  Data64.Uint32.Lower32 = MmioRead32(HostPcieAddr + 0x10);

  if((Data64.Uint32.Lower32 & (BIT1 + BIT2)) == BIT2){         // 64bit Address.
    Data64.Uint32.Upper32 = MmioRead32(HostPcieAddr + 0x14);
  }
  HdaBar = (UINTN)(Data64.Uint64 & ~(BIT0 + BIT1 + BIT2 + BIT3));
  ASSERT(HdaBar!=0);
  ASSERT(HdaBar < SIZE_4GB);
  
  return HdaBar;
}


// Only handle the first found codec.
EFI_STATUS 
AzaliaLoadVerbTable (
  IN UINTN           HostPcieAddr,
  IN VOID            *VerbTable, 
  IN UINTN           VerbTableSize
  )
{
	EFI_STATUS		      Status;
  UINTN               HdaBar;
  UINT16              CodecMask;  
  UINT8               CAd;
  UINT32              Verb;
  UINT32              RetVal;
  UINTN               TableIndex;
  UINTN               TableCount;  
  UINTN               VerbIndex;
  UINTN               VerbCount;
  UINT32              *VerbData;
  UINTN               LoopTime;
  OEM_VERB_TABLE      *OemVerbTable;
  UINTN               VerbRealCount;
  
  
  ASSERT(VerbTable!=NULL && VerbTableSize!=0);

  if(MmioRead16(HostPcieAddr)==0xFFFF){
    DEBUG((EFI_D_INFO, "Azalia Not Present\n"));
    Status = EFI_SUCCESS;
    goto ProcExit;		
  }		

  HdaBar = GetHdaBar(HostPcieAddr);
  DEBUG((EFI_D_INFO, "HdaBar:%X\n", HdaBar));  
  Status = AzaliaReset(HdaBar);
  ASSERT_EFI_ERROR(Status);
  
  CodecMask = 0;
  for (LoopTime = 0; LoopTime < AZALIA_MAX_LOOP_TIME; LoopTime++) {
    CodecMask = MmioRead16(HdaBar + REG_HDA_STATESTS) & AZALIA_MAX_SID_MASK;
    if(CodecMask!=0){
      break;
    } 
    MicroSecondDelay(AZALIA_WAIT_PERIOD);
  }  
  if(!CodecMask){
    DEBUG((EFI_D_ERROR, "No Codec Detect!\n"));
    goto ProcExit;
  }

  OemVerbTable = (OEM_VERB_TABLE*)VerbTable;
  TableCount = VerbTableSize/sizeof(OEM_VERB_TABLE);  
  for(CAd=0;CAd<AZALIA_MAX_SID_NUMBER;CAd++){
    if(!((UINT16)(1<<CAd) & CodecMask)){
      continue;
    }
    Verb   = CREATE_VERB12_FORMAT(CAd, ROOT_NODE_ID, VERB_GET_PARAMETER, PARAMETER_ID_VENDOR_ID);
    Status = AzaliaSendCommand(HdaBar, Verb, &RetVal);
    ASSERT_EFI_ERROR(Status);
    DEBUG((EFI_D_INFO, "SDI[%d] %08X\n", CAd, RetVal));
    
    for(TableIndex=0;TableIndex<TableCount;TableIndex++){
      if(RetVal != OemVerbTable[TableIndex].Hdr.Vdid){
        continue;
      }
      VerbCount = OemVerbTable[TableIndex].VerbDataSize/sizeof(UINT32);
      VerbData  = OemVerbTable[TableIndex].VerbData;
      VerbRealCount = 0;
      for(VerbIndex=0;VerbIndex<VerbCount;VerbIndex++){
        if(VerbData[VerbIndex] == 0 || VerbData[VerbIndex] == 0xFFFFFFFF){
          continue;
        }
        Verb   = (VerbData[VerbIndex] & 0x0FFFFFFF) | (CAd<<28);
        Status = AzaliaSendCommand(HdaBar, Verb, &RetVal);
        ASSERT_EFI_ERROR(Status);
        VerbRealCount++;
      }
      DEBUG((EFI_D_INFO, "VerbRealCount:%d\n", VerbRealCount));
      break;
    }
    if(TableIndex>=TableCount){
      DEBUG((EFI_D_ERROR, "VerbTable Not Found!\n"));
    }
    break;
  }
  
ProcExit:
  return Status;  
}