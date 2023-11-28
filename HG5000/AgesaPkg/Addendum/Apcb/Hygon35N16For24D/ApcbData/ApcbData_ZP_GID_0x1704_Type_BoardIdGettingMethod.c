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
 ******************************************************************************
 */

/*****************************************************************************
 *
 * Warning: BoardIdGettingMethod applies to multiple APCB only, please remove it
 * from ApcbCreate.bat if only 1 APCB instance is needed
 *
 *****************************************************************************
 */

#include "MyPorting.h"
#include "ApcbV2.h"

#include <MiscMemDefines.h>
#include <APCB.h>
#include <ApcbCustomizedDefinitions.h>
#include <ApcbDefaults.h>

CHAR8 mDummyBuf;

APCB_TYPE_DATA_START_SIGNATURE();
// From EEPROM
//APCB_TYPE_HEADER       ApcbTypeHeader = {APCB_GROUP_PSP, APCB_PSP_TYPE_BOARD_ID_GETTING_METHOD, (sizeof(APCB_TYPE_HEADER) + sizeof (PSP_GET_BOARD_ID_FROM_EEPROM_STRUCT)), 0, 0, 0 };  // SizeOfType will be fixed up by tool
//PSP_GET_BOARD_ID_FROM_EEPROM_STRUCT  ApcbGettingMethod =
//{
//  0,      // EEPROM
//  5,      // ZP I2C controller
//  0xa0    // SmbusAddr
//};

// From Smbus device
APCB_TYPE_HEADER       ApcbTypeHeader = {APCB_GROUP_PSP, APCB_PSP_TYPE_BOARD_ID_GETTING_METHOD, (sizeof(APCB_TYPE_HEADER) + sizeof (PSP_GET_BOARD_ID_FROM_SMBUS_STRUCT)), 0, 0, 0 };  // SizeOfType will be fixed up by tool
PSP_GET_BOARD_ID_FROM_SMBUS_STRUCT  BoardIdGettingMethod =
{
  BOARD_ID_METHOD_SMBUS,      // Smbus device method
  0,      // ZP I2C controller
  0x4e,   // Smbus address
  0,      // register index
  {       // Id_Apcb_Mapping
    {0x7, 0, 0},
    {0x7, 4, 1}
  }
};

// From FCH GPIO
//APCB_TYPE_HEADER       ApcbTypeHeader = {APCB_GROUP_PSP, APCB_PSP_TYPE_BOARD_ID_GETTING_METHOD, (sizeof(APCB_TYPE_HEADER) + sizeof (PSP_GET_BOARD_ID_FROM_GPIO_STRUCT)), 0, 0, 0 };  // SizeOfType will be fixed up by tool
//PSP_GET_BOARD_ID_FROM_GPIO_STRUCT  BoardIdGettingMethod =
//{
//  2,      // GPIO method
//  2,      // Gpio0
//  0,      // Gpio0IoMux
//  0,      // Gpio0BankCtl
//
//  2,      // Gpio1
//  0,      // Gpio1IoMux
//  0,      // Gpio1BankCtl
//
//  2,      // Gpio2
//  0,      // Gpio2IoMux
//  0,      // Gpio2BankCtl
//
//  0xff,   // Gpio3
//  0xff,   // Gpio3IoMux
//  0xff,   // Gpio3BankCtl
//  
//  {       // Id_Apcb_Mapping
//    {0x7, 0, 0},
//    {0x7, 1, 1}
//  }
//};

APCB_TYPE_DATA_END_SIGNATURE();

int main (IN int argc, IN char * argv [ ])
{
  return 0;
}



