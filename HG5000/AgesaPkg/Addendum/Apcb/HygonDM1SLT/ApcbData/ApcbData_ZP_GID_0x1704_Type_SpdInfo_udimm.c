
/* $NoKeywords:$ */
/**
 * @file
 *
 * @e \$Revision:$   @e \$Date:$
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 *
 ***************************************************************************/
//This file includes code originally published under the following license.


/** @file
  This module produces the EFI_PEI_S3_RESUME_PPI.
  This module works with StandAloneBootScriptExecutor to S3 resume to OS.
  This module will excute the boot script saved during last boot and after that,
  control is passed to OS waking up handler.

  Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available under the terms and conditions
  of the BSD License which accompanies this distribution.  The
  full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "MyPorting.h"
#include "ApcbV2.h"

#include <MiscMemDefines.h>
#include <APCB.h>
#include <ApcbCustomizedDefinitions.h>
#include <ApcbDefaults.h>

CHAR8 mDummyBuf;

APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER       ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_SPD_INFO, (sizeof(APCB_TYPE_HEADER) + sizeof (PSP_SPD_STRUCT)), 0, 0, 0 };  // SizeOfType will be fixed up by tool

///PSP_SPD_STRUCT is too big, redefine myPSP_SPD_STRUCT
typedef struct _myPSP_SPD_STRUCT {
	UINT32 TotalDimms;
  SPD_DEF_STRUCT  SpdData[4]; 
} myPSP_SPD_STRUCT;

myPSP_SPD_STRUCT memPspSpd = {
	4,                           //< TotalDimms - Total number of DIMMs in the system
	{
		TRUE,                   //< SpdValid - Indicates that the SPD is valid
		TRUE,                   //< DimmPresent - Indicates that the DIMM is present and Data is valid
		0,                      //< PageAddress - Indicates that the DIMM is present and Data is valid
		FALSE,	  				//< Indicates this DIMM is NVDIMM
		0,						//< DRAM Manufacture ID
		0xE0,                   //< Address - Indicates the SMBUS address of the DIMM
		{0, 0, 0, 0},           //< SMBUS Mux Info
		DDR4_TECHNOLOGY,        //< Technology - Indicates the type of technology supported
		UDIMM_PACKAGE,          //< Package - Package type  
		0,                      //< SocketNumber - Socket Number
        //1 for AM4, 0 for SP4, 3 for SP3
		2,                       //< ChannelNumber - Channel Number
		1,                       //< DimmNumber - DIMM Number
        0,	  				     //< Reserved
		///DDR4-1333-2G
        {
        
        //     0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f    
			 0x23, 0x11, 0x0C, 0x02, 0x85, 0x21, 0x00, 0x08, 0x00, 0x60, 0x00, 0x03, 0x01, 0x0B, 0x80, 0x00, //0
			 0x00, 0x00, 0x07, 0x0D, 0xF8, 0x0F, 0x00, 0x00, 0x6E, 0x6E, 0x6E, 0x11, 0x00, 0x6E, 0xF0, 0x0A, //1
			 0x20, 0x08, 0x00, 0x05, 0x00, 0xA8, 0x1B, 0x28, 0x28, 0x00, 0x78, 0x00, 0x14, 0x3C, 0x00, 0x00, //2
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x36, 0x16, 0x36, //3
			 0x16, 0x36, 0x16, 0x36, 0x16, 0x36, 0x16, 0x36, 0x16, 0x36, 0x16, 0x36, 0x16, 0x36, 0x00, 0x00, //4
			 0x35, 0xB3, 0x00, 0x85, 0xD9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //5
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //6
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9C, 0xB5, 0x00, 0x00, 0x00, 0x00, 0xE7, 0xD6, 0x36, 0x48, //7
			 0x11, 0x11, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //8
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //9
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //a
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //b
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //c
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //d
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //e
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4D, 0x57, //f
#if 0 //GR
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //10
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //11
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //12
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //13
			 0x80, 0xCE, 0x02, 0x17, 0x20, 0x35, 0xB3, 0x85, 0xD9, 0x4D, 0x33, 0x39, 0x31, 0x41, 0x31, 0x4B, //14
			 0x34, 0x33, 0x42, 0x42, 0x31, 0x2D, 0x43, 0x52, 0x43, 0x20, 0x20, 0x20, 0x20, 0x00, 0x80, 0xCE, //15
			 0x00, 0x53, 0x54, 0x5A, 0x48, 0x41, 0x31, 0x30, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, //16
			 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //17
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //18
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //19
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //1a
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //1b
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //1c
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //1d
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //1e
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //1f
#endif
		},
	},
};

APCB_TYPE_DATA_END_SIGNATURE();


int main (IN int argc, IN char * argv [ ])
{
  return 0;
}



