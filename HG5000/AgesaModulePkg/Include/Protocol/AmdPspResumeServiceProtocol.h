/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA PSP Resume Service Protocol Initialization
 *
 * Contains code to initialize GUID for AMD_PSP_RESUME_SERVICE_PROTOCOL_GUID
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision$   @e \$Date$
 *
 */
/****************************************************************************
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

#ifndef _AMD_PSP_RESUME_SERVICE_PROTOCOL_H_
#define _AMD_PSP_RESUME_SERVICE_PROTOCOL_H_

/// PSP resume callback priority level
#define PSP_RESUME_CALLBACK_LOWEST_LEVEL  0x1
#define PSP_RESUME_CALLBACK_LOW_LEVEL     0x40
#define PSP_RESUME_CALLBACK_MEDIUM_LEVEL  0x80
#define PSP_RESUME_CALLBACK_HIGH_LEVEL    0xC0
#define PSP_RESUME_CALLBACK_HIGHEST_LEVEL 0xFF

/// Define enum PSP_RESUME_CALLBACK_FLAG
typedef enum {
  PspResumeCallBackFlagBspOnly           = 0x01, ///< Call Back function will only be executed on BSP
  PspResumeCallBackFlagCorePrimaryOnly   = 0x02, ///< Call Back function will only be executed on CorePrimary
  PspResumeCallBackFlagCcxPrimaryOnly    = 0x03, ///< Call Back function will only be executed on CcxPrimary
  PspResumeCallBackFlagDiePrimaryOnly    = 0x04, ///< Call Back function will only be executed on DiePrimary
  PspResumeCallBackFlagSocketPrimaryOnly = 0x05, ///< Call Back function will only be executed on SocketPrimary
  PspResumeCallBackFlagAllCores          = 0x06, ///< Call Back function will be executed on AllCores
} PSP_RESUME_CALLBACK_FLAG;

/// Define enum RESUME_TYPE
typedef enum {
  ResumeFromConnectedStandby  = 0x01, ///< When resume from CS
  ResumeFromS3                = 0x02, ///< When resuming from S3
  InvalidResumeValue          = 0xFF  ///< Invalid Resume Value
} RESUME_TYPE;

typedef struct _PSP_RESUME_SERVICE_PROTOCOL PSP_RESUME_SERVICE_PROTOCOL;

typedef EFI_STATUS (EFIAPI *PSP_RESUME_CALLBACK) (
  IN  RESUME_TYPE   ResumeType,
  IN  VOID          *Context
  );

typedef EFI_STATUS (EFIAPI *PSP_RESUME_REGISTER) (
  IN       PSP_RESUME_SERVICE_PROTOCOL        *This,
  IN       PSP_RESUME_CALLBACK                CallBackFunction,
  IN OUT   VOID                               *Context,
  IN       UINTN                              CallbackPriority,
  IN       PSP_RESUME_CALLBACK_FLAG           Flag,
  OUT      EFI_HANDLE                         *DispatchHandle
);

typedef EFI_STATUS (EFIAPI *PSP_RESUME_UNREGISTER) (
  IN       PSP_RESUME_SERVICE_PROTOCOL        *This,
  IN       EFI_HANDLE                         DispatchHandle
);

/// Structure for PSP_RESUME_SERVICE_PROTOCOL protocol
typedef struct  _PSP_RESUME_SERVICE_PROTOCOL {
  PSP_RESUME_REGISTER    Register;        ///< Register fn
  PSP_RESUME_UNREGISTER  UnRegister;      ///< Unregister fn
} PSP_RESUME_SERVICE_PROTOCOL ;

extern EFI_GUID gAmdPspResumeServiceProtocolGuid;
#endif //_AMD_PSP_RESUME_SERVICE_PROTOCOL_H_
