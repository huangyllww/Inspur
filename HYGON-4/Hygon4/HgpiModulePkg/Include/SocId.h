/*****************************************************************************
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

#ifndef _SOC_ID_H_
#define _SOC_ID_H_

#define  HYGON_AX_CPU       0
#define  HYGON_BX_CPU       1
#define  HYGON_CX_CPU       2
#define  HYGON_DX_CPU       3
#define  HYGON_EX_CPU       4
#define  HYGON_FX_CPU       5
#define  HYGON_GX_CPU       6
#define  HYGON_HX_CPU       7

//Hygon Ex PkgType
#define  ST_SL2R2           0        ///< Server 2 Dharma CDD + Satori  
#define  ST_SL2             1        ///< Server 4 Dharma CDD + Satori
#define  ST_SL2R2S          2        ///< Server 2 Dharma CDD + Satori + Security

//Hygon Gx PkgType
#define  SP5_621            0         ///< Server 6 Shanghai CDD + 2 Emei IOD + 1 Dj IOD
#define  BGA_821            3         ///< Server 8 Shanghai CDD + 2 Emei IOD + 1 Dj IOD
#define  SP6_421            6         ///< Server 4 Shanghai CDD + 2 Emei IOD + 1 Dj IOD
#define  SP6_211            7         ///< Server 2 Shanghai CDD + 1 Emei IOD + 1 Dj IOD   Presilicon 1Shanghai + 1Emei +1Dj
#define  SP6_204            8         ///< Server 2 Shanghai CDD + 4 Dj IOD
#define  DM1_102            9         ///< Workstation 1 Shanghai CDD + 2 Dj IOD

#endif
