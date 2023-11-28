/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Platform Protocol
 *
 * Contains definitions for AMD_PSP_PLATFORM_PROTOCOL_GUID
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  UEFI
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ****************************************************************************
 * AMD Generic Encapsulated Software Architecture
 *
 * Description: AmdAgesaDxeProtocol.h - DXE Protocol and GUID Declaration.
 *
 ******************************************************************************
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

#ifndef _PSP_PLATFORM_PROTOCOL_H_
#define _PSP_PLATFORM_PROTOCOL_H_

// !!! NOTE Please don't change the GUID defined below
// {CCF14A29-37E0-48ad-9005-1F89622FB798}
#define AMD_PSP_PLATFORM_PROTOCOL_GUID \
  { 0xccf14a29, 0x37e0, 0x48ad, { 0x90, 0x5, 0x1f, 0x89, 0x62, 0x2f, 0xb7, 0x98 } }

/// Resume Handoff Structure
typedef struct {
  UINT32  GdtOffset;                              // GDT table offset for RSM
  UINT16  CodeSelector;                           // CODE Segment Selector
  UINT16  DataSelector;                           // DATA Segment Selector
  UINT32  RsmEntryPoint;                          // IP Address after executing rsm command
  UINT32  EdxResumeSignature;                     // Value keep in EDX after executing rsm command
} RSM_HANDOFF_INFO;


/// PSP Platform Protocol, provide PSP platform customized information
typedef struct _PSP_PLATFORM_PROTOCOL {
  BOOLEAN                 CpuContextResumeEnable; // TRUE:Enable CPU Context Resume, FALSE:Disable CPU Context Resume
  UINT8                   SwSmiCmdtoBuildContext; // SW SMI number for build Cpu Context
  UINT32                  BspStackSize;           // BSP Stack Size for resume
  UINT32                  ApStackSize;            // AP Stack Size for resume
  RSM_HANDOFF_INFO        *RsmHandOffInfo;        // Resume Handoff structure include GDTTable Offset, CS&DS Selector, Entrypoint for Resume
                                                  // Also a signature kept in EDX for identification
} PSP_PLATFORM_PROTOCOL;

extern EFI_GUID gPspPlatformProtocolGuid;

#endif //_PSP_PLATFORM_PROTOCOL_H_
