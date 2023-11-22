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

#define FILECODE  FABRIC_HYEX_FABRICDXE_FABRICACPIDISTANCEINFO_FILECODE

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
#define DISTANCE_TO_SELF  10
#define XHMI_PENALTY      7
#define HMI_PENALTY       5

#define SLIT_DEGREE_AUTO_LOCAL_DISTANCE   0x10
#define SLIT_DEGREE_AUTO_REMOTE_DISTANCE  0x20

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

EFI_STATUS
EFIAPI
FabricGetDistanceInfo (
  IN OUT   UINT8                                  *Distance,
  IN       UINT32                                  DomainCount,
  IN       DOMAIN_INFO                            *DomainInfo
  )
{
  UINT32                 i;
  UINT32                 j;

  for (i = 0; i < DomainCount; i++) {
    ASSERT (DomainInfo[i].Type < MaxDomainType);
    for (j = 0; j < DomainCount; j++) {
      if (i == j) {
        // Case 1: distance to self
        *Distance = DISTANCE_TO_SELF;
      } else {
        ASSERT (DomainInfo[i].Type != SocketIntlv);
        ASSERT (DomainInfo[j].Type != SocketIntlv);
        ASSERT (DomainInfo[j].Type < MaxDomainType);
        if ((DomainInfo[i].Type == DieIntlv) && (DomainInfo[j].Type == DieIntlv)) {
          // Case 2: die interleaving is enabled on both sockets
          // The normalized penalty is 1 xHMI hop as local is slower due to 3/4 chance of local hop
          *Distance = DISTANCE_TO_SELF + XHMI_PENALTY;

        } else if ((DomainInfo[i].Type == DieIntlv) || (DomainInfo[j].Type == DieIntlv)) {
          // Case 3: die interleaving is enabled on one of the two sockets
          // 1 xHMI + 2 * HMI
          *Distance = DISTANCE_TO_SELF + 2 * HMI_PENALTY + XHMI_PENALTY;

        } else {
          // Die interleaving is disabled on both domains
          ASSERT (DomainInfo[i].Type == NoIntlv);
          ASSERT (DomainInfo[j].Type == NoIntlv);
          if (DomainInfo[i].Intlv.None.Socket == DomainInfo[j].Intlv.None.Socket) {
            // Case 4: Domains are on the same socket
            *Distance = DISTANCE_TO_SELF + HMI_PENALTY;
          } else {
            if (((INT8)(i-j) == 4) || ((INT8)(i-j) == -4)) {
              *Distance = 25;
            } else {
              *Distance = DISTANCE_TO_SELF + 2 * HMI_PENALTY + XHMI_PENALTY;
			}
          }
        }
      }

      Distance++;
    }
  }

  return EFI_SUCCESS;
}

