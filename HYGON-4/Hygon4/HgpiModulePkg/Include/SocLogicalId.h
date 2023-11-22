/*
 ******************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 *
 ******************************************************************************
 */

#ifndef _SOC_LOGICAL_ID_H_
#define _SOC_LOGICAL_ID_H_
#pragma pack (push, 1)
#include <SocId.h>
/*
 *--------------------------------------------------------------
 *
 *                      M O D U L E S    U S E D
 *
 *---------------------------------------------------------------
 */

/*
 *--------------------------------------------------------------
 *
 *                      D E F I N I T I O N S  /  M A C R O S
 *
 *---------------------------------------------------------------
 */
/// SOC logical ID structure
typedef struct _SOC_LOGICAL_ID {
  IN OUT   UINT32    Family;          ///< Indicates logical ID Family
  IN OUT   UINT32    Revision;        ///< Indicates logical ID Revision
} SOC_LOGICAL_ID;

// UNKNOWN equates
// -------------------------------------
// This define should be equal to the total number of families
// in the cpuFamily enum.
#define MAX_CPU_FAMILIES   31
#define MAX_CPU_REVISIONS  15     // Max Cpu Revisions Per Family
// Family Unknown
#define HYGON_FAMILY_UNKNOWN    0x80000000ul
#define HYGON_REVISION_UNKNOWN  0x80000000ul

// SOC_LOGICAL_ID.Family equates
// -------------------------------------

// HYGON Family 18h DN equates
#define HYGON_FAMILY_18_DN  0x00010000ul
#define HYGON_FAMILY_DN     (HYGON_FAMILY_18_DN)
#define HYGON_FAMILY_18     (HYGON_FAMILY_DN)

// HYGON Family 01h SOC_LOGICAL_ID.Revision equates
// -------------------------------------
// Family 18h DN core steppings
#define HYGON_REV_F18_DN_A0  0x00000001ul
#define HYGON_REV_F18_DN_A1  0x00000002ul
#define HYGON_REV_F18_DN_A2  0x00000004ul

#define HYGON_REV_F18_DN_B0  0x00000008ul
#define HYGON_REV_F18_DN_B1  0x00000010ul
#define HYGON_REV_F18_DN_B2  0x00000020ul
#define HYGON_REV_F18_DN_C0  0x00000100ul
#define HYGON_REV_F18_DN_C1  0x00000200ul
#define HYGON_REV_F18_DN_C2  0x00000400ul
#define HYGON_REV_F18_DN_D0  0x00001000ul
#define HYGON_REV_F18_DN_D1  0x00002000ul
#define HYGON_REV_F18_DN_E0  0x00004000ul
#define HYGON_REV_F18_DN_E1  0x00008000ul
#define HYGON_REV_F18_DN_F0  0x00010000ul
#define HYGON_REV_F18_DN_G0  0x00020000ul
#define HYGON_REV_F18_DN_H0  0x00040000ul

#define HYGON_REV_F18_DN_Ax  (HYGON_REV_F18_DN_A0 | HYGON_REV_F18_DN_A1 | HYGON_REV_F18_DN_A2)
#define HYGON_REV_F18_DN_Bx  (HYGON_REV_F18_DN_B0 | HYGON_REV_F18_DN_B1 | HYGON_REV_F18_DN_B2)
#define HYGON_REV_F18_DN_Cx  (HYGON_REV_F18_DN_C0 | HYGON_REV_F18_DN_C1 | HYGON_REV_F18_DN_C2)
#define HYGON_REV_F18_DN_Dx  (HYGON_REV_F18_DN_D0 | HYGON_REV_F18_DN_D1)
#define HYGON_REV_F18_DN_Ex  (HYGON_REV_F18_DN_E0 | HYGON_REV_F18_DN_E1)
#define HYGON_REV_F18_DN_Fx  (HYGON_REV_F18_DN_F0)
#define HYGON_REV_F18_DN_Gx  (HYGON_REV_F18_DN_G0)
#define HYGON_REV_F18_DN_Hx  (HYGON_REV_F18_DN_H0)

#define HYGON_REV_F18_DN_ALL  (HYGON_REV_F18_DN_Ax | HYGON_REV_F18_DN_Bx | HYGON_REV_F18_DN_Cx \
                               | HYGON_REV_F18_DN_Dx | HYGON_REV_F18_DN_Ex | HYGON_REV_F18_DN_Fx \
                               | HYGON_REV_F18_DN_Gx | HYGON_REV_F18_DN_Hx)

// Family 18h DN derived steppings
#define HYGON_REV_F18_DN_LT_A1   (HYGON_REV_F18_DN_A0)
#define HYGON_REV_F18_DN_GTE_A1  (HYGON_REV_F18_DN_ALL & ~HYGON_REV_F18_DN_LT_A1)
#define HYGON_REV_F18_DN_LTE_A1  (HYGON_REV_F18_DN_A0 | HYGON_REV_F18_DN_A1)

#define HYGON_REV_F18_DN_LT_B0   (HYGON_REV_F18_DN_Ax)
#define HYGON_REV_F18_DN_GTE_B0  (HYGON_REV_F18_DN_ALL & ~HYGON_REV_F18_DN_Ax)
#define HYGON_REV_F18_DN_LTE_B0  (HYGON_REV_F18_DN_Ax | HYGON_REV_F18_DN_B0)

#define HYGON_REV_F18_DN_LT_C0   (HYGON_REV_F18_DN_Ax | HYGON_REV_F18_DN_Bx)
#define HYGON_REV_F18_DN_GTE_C0  (HYGON_REV_F18_DN_ALL & ~(HYGON_REV_F18_DN_Ax | HYGON_REV_F18_DN_Bx))
#define HYGON_REV_F18_DN_LTE_C0  (HYGON_REV_F18_DN_Ax | HYGON_REV_F18_DN_Bx | HYGON_REV_F18_DN_C0)

#define HYGON_REV_F18_DN_LT_D0   (HYGON_REV_F18_DN_Ax | HYGON_REV_F18_DN_Bx | HYGON_REV_F18_DN_Cx)
#define HYGON_REV_F18_DN_GTE_D0  (HYGON_REV_F18_DN_ALL & ~(HYGON_REV_F18_DN_Ax | HYGON_REV_F18_DN_Bx | HYGON_REV_F18_DN_Cx))
#define HYGON_REV_F18_DN_LTE_D0  (HYGON_REV_F18_DN_Ax | HYGON_REV_F18_DN_Bx | HYGON_REV_F18_DN_Cx | HYGON_REV_F18_DN_D0)

#define HYGON_REV_F18_DN_LT_E0   (HYGON_REV_F18_DN_Ax | HYGON_REV_F18_DN_Bx | HYGON_REV_F18_DN_Cx | HYGON_REV_F18_DN_Dx)
#define HYGON_REV_F18_DN_GTE_E0  (HYGON_REV_F18_DN_ALL & ~(HYGON_REV_F18_DN_Ax | HYGON_REV_F18_DN_Bx | HYGON_REV_F18_DN_Cx | HYGON_REV_F18_DN_Dx))
#define HYGON_REV_F18_DN_LTE_E0  (HYGON_REV_F18_DN_Ax | HYGON_REV_F18_DN_Bx | HYGON_REV_F18_DN_Cx | HYGON_REV_F18_DN_Dx | HYGON_REV_F18_DN_E0)

#define HYGON_REV_F18_DN_LT_F0   (HYGON_REV_F18_DN_Ax | HYGON_REV_F18_DN_Bx | HYGON_REV_F18_DN_Cx | HYGON_REV_F18_DN_Dx | HYGON_REV_F18_DN_Ex)
#define HYGON_REV_F18_DN_GTE_F0  (HYGON_REV_F18_DN_ALL & ~(HYGON_REV_F18_DN_Ax | HYGON_REV_F18_DN_Bx | HYGON_REV_F18_DN_Cx | HYGON_REV_F18_DN_Dx | HYGON_REV_F18_DN_Ex))
#define HYGON_REV_F18_DN_LTE_F0  (HYGON_REV_F18_DN_Ax | HYGON_REV_F18_DN_Bx | HYGON_REV_F18_DN_Cx | HYGON_REV_F18_DN_Dx | HYGON_REV_F18_DN_Ex | HYGON_REV_F18_DN_F0)

#pragma pack (pop)
#endif // _SOC_LOGICAL_ID_H_
