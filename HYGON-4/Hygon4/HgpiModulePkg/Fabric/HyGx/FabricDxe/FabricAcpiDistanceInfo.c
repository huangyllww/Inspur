/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Fabric ACPI distance information for SLIT/CDIT.
 *
 * This funtion collect distance information for SLIT/CDIT.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Fabric
 *
 */
/*
 ****************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Porting.h"
#include "HYGON.h"
#include <Library/HygonBaseLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersST.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/HygonIdsHookLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include <Filecode.h>

#define FILECODE  FABRIC_HYGX_FABRICDXE_FABRICACPIDISTANCEINFO_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
#define DISTANCE_TO_SELF                       10
//CXL NUMA distance
#define CXL_TO_CXL_DISTANCE                    0xFF
#define CXL_LOCAL_DISTANCE                     19
#define CXL_REMOTE_DISTANCE                    29

// S0D4->S1D5 Do not change, Average Other Three Node Distance
UINT8 TwoSocketDistancePlanA[12][12] = {
             //S0D4,S0D5,S0D6,S0D7,S1D4,S1D5,S1D6,S1D7,S0D0CXL,S0D1CXL,S1D0CXL,S1D1CXL
/*S0D4*/      { 10, 17, 17, 17, 35, 28, 35, 35, 16, 19, 26, 29 },
/*S0D5*/      { 17, 10, 17, 17, 28, 35, 35, 35, 16, 19, 26, 29 },
/*S0D6*/      { 17, 17, 10, 17, 35, 35, 35, 28, 19, 16, 29, 26 },
/*S0D7*/      { 17, 17, 17, 10, 35, 35, 28, 35, 19, 16, 29, 26 },
/*S1D4*/      { 35, 28, 35, 35, 10, 17, 17, 17, 26, 29, 16, 19 },
/*S1D5*/      { 28, 35, 35, 35, 17, 10, 17, 17, 26, 29, 16, 19 },
/*S1D6*/      { 35, 35, 35, 28, 17, 17, 10, 17, 29, 26, 19, 16 },
/*S1D7*/      { 35, 35, 28, 35, 17, 17, 17, 10, 29, 26, 19, 16 },
/*S0D0CXL*/   { 16, 16, 19, 19, 26, 26, 29, 29, 0xFF,0xFF,0xFF,0xFF },
/*S0D1CXL*/   { 19, 19, 16, 16, 29, 29, 26, 26, 0xFF,0xFF,0xFF,0xFF },
/*S1D0CXL*/   { 26, 26, 29, 29, 16, 16, 19, 19, 0xFF,0xFF,0xFF,0xFF },
/*S1D1CXL*/   { 29, 29, 26, 26, 19, 19, 16, 16, 0xFF,0xFF,0xFF,0xFF }
};

// S0D4->S1D6 Do not change, Average Other Three Node Distance
UINT8 TwoSocketDistancePlanB[12][12] = {
             //S0D4,S0D5,S0D6,S0D7,S1D4,S1D5,S1D6,S1D7,S0D0CXL,S0D1CXL,S1D0CXL,S1D1CXL
/*S0D4*/      { 10, 17, 17, 17, 34, 34, 32, 34, 16, 19, 26, 29 },
/*S0D5*/      { 17, 10, 17, 17, 34, 34, 34, 32, 16, 19, 26, 29 },
/*S0D6*/      { 17, 17, 10, 17, 32, 34, 34, 34, 19, 16, 29, 26 },
/*S0D7*/      { 17, 17, 17, 10, 34, 32, 34, 34, 19, 16, 29, 26 },
/*S1D4*/      { 34, 34, 32, 34, 10, 17, 17, 17, 26, 29, 16, 19 },
/*S1D5*/      { 34, 34, 34, 32, 17, 10, 17, 17, 26, 29, 16, 19 },
/*S1D6*/      { 32, 34, 34, 34, 17, 17, 10, 17, 29, 26, 19, 16 },
/*S1D7*/      { 34, 32, 34, 34, 17, 17, 17, 10, 29, 26, 19, 16 },
/*S0D0CXL*/   { 16, 16, 19, 19, 26, 26, 29, 29, 0xFF,0xFF,0xFF,0xFF },
/*S0D1CXL*/   { 19, 19, 16, 16, 29, 29, 26, 26, 0xFF,0xFF,0xFF,0xFF },
/*S1D0CXL*/   { 26, 26, 29, 29, 16, 16, 19, 19, 0xFF,0xFF,0xFF,0xFF },
/*S1D1CXL*/   { 29, 29, 26, 26, 19, 19, 16, 16, 0xFF,0xFF,0xFF,0xFF }
};

// 4link for Single Side, Physical CDD ID
UINT8 TwoSocketDistanceFourLinkForSingleSide[8][8] = {
             //S0D4,S0D5,S0D8,S0D9,S1D4,S1D5,S1D8,S1D9
/*S0D4*/      { 10, 15, 20, 16, 44, 39, 34, 39 },
/*S0D5*/      { 15, 10, 16, 20, 39, 34, 29, 34 },
/*S0D8*/      { 20, 16, 10, 15, 34, 29, 34, 29 },
/*S0D9*/      { 16, 20, 15, 10, 39, 34, 39, 44 },
/*S1D4*/      { 44, 39, 34, 39, 10, 15, 20, 16 },
/*S1D5*/      { 39, 34, 29, 34, 15, 10, 16, 20 },
/*S1D8*/      { 34, 29, 34, 39, 20, 16, 10, 15 },
/*S1D9*/      { 39, 34, 39, 44, 16, 20, 15, 10 }
};

// All Distance is same between sockets fof 4ways
UINT8 FourSocketDistancePlanA[24][24] = {
             //S0D4,S0D5,S0D6,S0D7,S1D4,S1D5,S1D6,S1D7,S2D4,S2D5,S2D6,S2D7,S3D4,S3D5,S3D6,S3D7,S0D0CXL,S0D1CXL,S1D0CXL,S1D1CXL,S2D0CXL,S2D1CXL,S3D0CXL,S3D1CXL
/*S0D4*/      { 10, 17, 17, 17, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 16, 16, 41, 30, 30, 41, 30, 41 },
/*S0D5*/      { 17, 10, 17, 17, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 16, 16, 41, 30, 30, 41, 30, 41 },
/*S0D6*/      { 17, 17, 10, 17, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 16, 16, 41, 30, 30, 41, 30, 41 },
/*S0D7*/      { 17, 17, 17, 10, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 16, 16, 41, 30, 30, 41, 30, 41 },
/*S1D4*/      { 38, 38, 38, 38, 10, 17, 17, 17, 38, 38, 38, 38, 38, 38, 38, 38, 40, 30, 16, 16, 30, 41, 30, 41 },
/*S1D5*/      { 38, 38, 38, 38, 17, 10, 17, 17, 38, 38, 38, 38, 38, 38, 38, 38, 40, 30, 16, 16, 30, 41, 30, 41 },
/*S1D6*/      { 38, 38, 38, 38, 17, 17, 10, 17, 38, 38, 38, 38, 38, 38, 38, 38, 40, 30, 16, 16, 30, 41, 30, 41 },
/*S1D7*/      { 38, 38, 38, 38, 17, 17, 17, 10, 38, 38, 38, 38, 38, 38, 38, 38, 40, 30, 16, 16, 30, 41, 30, 41 },
/*S2D4*/      { 38, 38, 38, 38, 38, 38, 38, 38, 10, 17, 17, 17, 38, 38, 38, 38, 30, 41, 30, 41, 16, 16, 41, 30 },
/*S2D5*/      { 38, 38, 38, 38, 38, 38, 38, 38, 17, 10, 17, 17, 38, 38, 38, 38, 30, 41, 30, 41, 16, 16, 41, 30 },
/*S2D6*/      { 38, 38, 38, 38, 38, 38, 38, 38, 17, 17, 10, 17, 38, 38, 38, 38, 30, 41, 30, 41, 16, 16, 41, 30 },
/*S2D7*/      { 38, 38, 38, 38, 38, 38, 38, 38, 17, 17, 17, 10, 38, 38, 38, 38, 30, 41, 30, 41, 16, 16, 41, 30 },
/*S3D4*/      { 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 10, 17, 17, 17, 30, 41, 30, 41, 41, 30, 16, 16 },
/*S3D5*/      { 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 17, 10, 17, 17, 30, 41, 30, 41, 41, 30, 16, 16 },
/*S3D6*/      { 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 17, 17, 10, 17, 30, 41, 30, 41, 41, 30, 16, 16 },
/*S3D7*/      { 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 17, 17, 17, 10, 30, 41, 30, 41, 41, 30, 16, 16 },
/*S0D0CXL*/   { 16, 16, 16, 16, 41, 41, 41, 41, 30, 30, 30, 30, 30, 30, 30, 30, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
/*S0D1CXL*/   { 16, 16, 16, 16, 30, 30, 30, 30, 41, 41, 41, 41, 41, 41, 41, 41, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
/*S1D0CXL*/   { 41, 41, 41, 41, 16, 16, 16, 16, 30, 30, 30, 30, 30, 30, 30, 30, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
/*S1D1CXL*/   { 30, 30, 30, 30, 16, 16, 16, 16, 41, 41, 41, 41, 41, 41, 41, 41, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
/*S2D0CXL*/   { 30, 30, 30, 30, 30, 30, 30, 30, 16, 16, 16, 16, 41, 41, 41, 41, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
/*S2D1CXL*/   { 41, 41, 41, 41, 41, 41, 41, 41, 16, 16, 16, 16, 30, 30, 30, 30, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
/*S3D0CXL*/   { 30, 30, 30, 30, 30, 30, 30, 30, 41, 41, 41, 41, 16, 16, 16, 16, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
/*S3D1CXL*/   { 41, 41, 41, 41, 41, 41, 41, 41, 30, 30, 30, 30, 16, 16, 16, 16, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF }
};

// Choose a min Value as reference, Average Other Three Node Distance in a same socket
UINT8 FourSocketDistancePlanB[24][24] = {
             //S0D4,S0D5,S0D6,S0D7,S1D4,S1D5,S1D6,S1D7,S2D4,S2D5,S2D6,S2D7,S3D4,S3D5,S3D6,S3D7,S0D0CXL,S0D1CXL,S1D0CXL,S1D1CXL,S2D0CXL,S2D1CXL,S3D0CXL,S3D1CXL
/*S0D4*/      { 10, 17, 17, 17, 44, 44, 44, 38, 34, 39, 39, 39, 34, 39, 39, 39, 16, 16, 41, 30, 30, 41, 30, 41 },
/*S0D5*/      { 17, 10, 17, 17, 39, 39, 39, 32, 28, 34, 34, 34, 28, 34, 34, 34, 16, 16, 41, 30, 30, 41, 30, 41 },
/*S0D6*/      { 17, 17, 10, 17, 34, 34, 34, 28, 32, 39, 39, 39, 32, 39, 39, 39, 16, 16, 41, 30, 30, 41, 30, 41 },
/*S0D7*/      { 17, 17, 17, 10, 39, 39, 39, 34, 38, 44, 44, 44, 38, 44, 44, 44, 16, 16, 41, 30, 30, 41, 30, 41 },
/*S1D4*/      { 39, 39, 39, 32, 10, 17, 17, 17, 38, 38, 38, 38, 38, 38, 38, 38, 40, 30, 16, 16, 30, 41, 30, 41 },
/*S1D5*/      { 34, 34, 34, 28, 17, 10, 17, 17, 38, 38, 38, 38, 38, 38, 38, 38, 40, 30, 16, 16, 30, 41, 30, 41 },
/*S1D6*/      { 39, 39, 39, 34, 17, 17, 10, 17, 38, 38, 38, 38, 38, 38, 38, 38, 40, 30, 16, 16, 30, 41, 30, 41 },
/*S1D7*/      { 44, 44, 44, 39, 17, 17, 17, 10, 38, 38, 38, 38, 38, 38, 38, 38, 40, 30, 16, 16, 30, 41, 30, 41 },
/*S2D4*/      { 34, 39, 39, 39, 34, 39, 39, 39, 10, 17, 17, 17, 38, 38, 38, 38, 30, 41, 30, 41, 16, 16, 41, 30 },
/*S2D5*/      { 28, 34, 34, 34, 28, 34, 34, 34, 17, 10, 17, 17, 38, 38, 38, 38, 30, 41, 30, 41, 16, 16, 41, 30 },
/*S2D6*/      { 32, 39, 39, 39, 32, 39, 39, 39, 17, 17, 10, 17, 38, 38, 38, 38, 30, 41, 30, 41, 16, 16, 41, 30 },
/*S2D7*/      { 38, 44, 44, 44, 38, 44, 44, 44, 17, 17, 17, 10, 38, 38, 38, 38, 30, 41, 30, 41, 16, 16, 41, 30 },
/*S3D4*/      { 34, 39, 39, 39, 34, 39, 39, 39, 38, 38, 38, 38, 10, 17, 17, 17, 30, 41, 30, 41, 41, 30, 16, 16 },
/*S3D5*/      { 28, 34, 34, 34, 28, 34, 34, 34, 38, 38, 38, 38, 17, 10, 17, 17, 30, 41, 30, 41, 41, 30, 16, 16 },
/*S3D6*/      { 32, 39, 39, 39, 32, 39, 39, 39, 38, 38, 38, 38, 17, 17, 10, 17, 30, 41, 30, 41, 41, 30, 16, 16 },
/*S3D7*/      { 38, 44, 44, 44, 38, 44, 44, 44, 38, 38, 38, 38, 17, 17, 17, 10, 30, 41, 30, 41, 41, 30, 16, 16 },
/*S0D0CXL*/   { 16, 16, 16, 16, 41, 41, 41, 41, 30, 30, 30, 30, 30, 30, 30, 30, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
/*S0D1CXL*/   { 16, 16, 16, 16, 30, 30, 30, 30, 41, 41, 41, 41, 41, 41, 41, 41, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
/*S1D0CXL*/   { 41, 41, 41, 41, 16, 16, 16, 16, 30, 30, 30, 30, 30, 30, 30, 30, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
/*S1D1CXL*/   { 30, 30, 30, 30, 16, 16, 16, 16, 41, 41, 41, 41, 41, 41, 41, 41, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
/*S2D0CXL*/   { 30, 30, 30, 30, 30, 30, 30, 30, 16, 16, 16, 16, 41, 41, 41, 41, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
/*S2D1CXL*/   { 41, 41, 41, 41, 41, 41, 41, 41, 16, 16, 16, 16, 30, 30, 30, 30, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
/*S3D0CXL*/   { 30, 30, 30, 30, 30, 30, 30, 30, 41, 41, 41, 41, 16, 16, 16, 16, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
/*S3D1CXL*/   { 41, 41, 41, 41, 41, 41, 41, 41, 30, 30, 30, 30, 16, 16, 16, 16, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF }
};

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricGetDistanceInfo (
  IN OUT   UINT8                                  *Distance,
  IN       UINT32                                  DomainCount,
  IN       DOMAIN_INFO                            *DomainInfo,
  IN       UINT32                                  CxlDomainCount,
  IN       CXL_DOMAIN_INFO                        *CxlDomainInfo
  )
{
  UINT32                 i;
  UINT32                 j;
  UINT8                  CddNumPerSocket;
  UINT8                  SocketNum;
  UINT32                 TotalDomainCount;

  TotalDomainCount = DomainCount + CxlDomainCount;

  CddNumPerSocket = (UINT8)FabricTopologyGetNumberOfCddsOnSocket (0);
  SocketNum = (UINT8)FabricTopologyGetNumberOfSocketPresent ();

  //Dram NUMA node
  for (i = 0; i < TotalDomainCount; i++) {
    //ASSERT (DomainInfo[i].Type < MaxDomainType);
    for (j = 0; j < TotalDomainCount; j++) {
      if (i == j) {
        // Branch 1: distance to self
        *Distance = DISTANCE_TO_SELF;
        Distance++;
        continue;
      } 
      
      if ((i < DomainCount) && (j < DomainCount)) {
        //From DRAM NUMA node to DRAM NUMA node
        ASSERT (DomainInfo[i].Type != SocketIntlv);
        ASSERT (DomainInfo[j].Type != SocketIntlv);
        ASSERT (DomainInfo[j].Type < MaxDomainType);
        if ((DomainInfo[i].Type == DieIntlv) && (DomainInfo[j].Type == DieIntlv)) {
          // Branch 2: die interleaving is enabled on All sockets

          //todo

        } else if ((DomainInfo[i].Type == DieIntlv) || (DomainInfo[j].Type == DieIntlv)) {
          // Branch 3: 
          // 1.die interleaving is enabled on one of the two sockets 
          // 2.die interleaving is enabled on one of the four sockets
          // 3.die interleaving is enabled on two of the four sockets
          // 4.die interleaving is enabled on three of the four sockets
          
          //todo

        } else {
          // Branch 4: Die interleaving is disabled on both domains
          ASSERT (DomainInfo[i].Type == NoIntlv);
          ASSERT (DomainInfo[j].Type == NoIntlv);
          
          if (SocketNum == 1) {
            // 1 Socket, access from node to node is simple for one socket
            *Distance = TwoSocketDistanceFourLinkForSingleSide[i][j];
          } else if (SocketNum == 2 && CddNumPerSocket == 4) {
            // 2 Socket,4 Cdds per Socket;  Need Confirm if 4way but Only two cpu??? Check PcdPlatformSelect?
            /* 
              if 2 socket and 4 cdd per socket {
                // todo
              } else if 4way but only 2 socket {   // Need Confirm how to connect between the two CPU
                // todo
              } 
            */ 
            *Distance = TwoSocketDistanceFourLinkForSingleSide[i][j];
          } else if (SocketNum == 2 && CddNumPerSocket == 2) {
            // 2 Socket,2 Cdds per Socket
            *Distance = TwoSocketDistanceFourLinkForSingleSide[(i >= 2 ? i + 2 : i)][(j >= 2 ? j + 2 : j)];
          } else if (SocketNum == 4 && CddNumPerSocket == 4) {
            // 2 Socket,4 Cdds per Socket
            *Distance = FourSocketDistancePlanA[i][j];
          }
        }
      } else if ((i >= DomainCount) && (j >= DomainCount)) {
        //From CXL to CXL
        *Distance = CXL_TO_CXL_DISTANCE;
        
      } else if ((i < DomainCount) && (j >= DomainCount)) {
        //From DRAM to CXL
        if (((1 << DomainInfo[i].Intlv.None.Socket) & CxlDomainInfo[j - DomainCount].SocketMap) != 0) {
          *Distance = CXL_LOCAL_DISTANCE;
        } else {
          *Distance = CXL_REMOTE_DISTANCE;
        }
      } else if ((i >= DomainCount) && (j < DomainCount)) {
        //From CXL to DRAM
        if ((CxlDomainInfo[i - DomainCount].SocketMap & (1 << DomainInfo[j].Intlv.None.Socket)) != 0) {
          *Distance = CXL_LOCAL_DISTANCE;
        } else {
          *Distance = CXL_REMOTE_DISTANCE;
        }
      }

      Distance++;
    }
  }

  // todo Add CXL Node 
  // ?

  return EFI_SUCCESS;
}
