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
 ******************************************************************************
 */
#include "SnpDxe.h"
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "XgbeSgmiiExtPhy.h"
#include "SnpDxeXgbe.h"

UINT32
XgbeCreateMdioSca (
  UINT32 port,
  UINT32 reg
  )
{
  UINT32  mdio_sca, da;

  da = (reg & MII_ADDR_C45) ? reg >> 16 : 0;

  mdio_sca = 0;
  XGMAC_SET_BITS (mdio_sca, MAC_MDIOSCAR, RA, reg);
  XGMAC_SET_BITS (mdio_sca, MAC_MDIOSCAR, PA, port);
  XGMAC_SET_BITS (mdio_sca, MAC_MDIOSCAR, DA, da);

  return mdio_sca;
}

UINT32
XgbeReadExtMiiRegs (
  ENET_DXE_PRIVATE_DATA *pdata,
  UINT32 addr,
  UINT32 reg
  )
{
  UINT32  mdio_sca, mdio_sccd, RegVal;
  UINT32  delay_cnt;

  mdio_sca = XgbeCreateMdioSca (addr, reg);
  XGMAC_IOWRITE32 (&pdata->xgmac_pdata, MAC_MDIOSCAR, mdio_sca);
  gBS->Stall (1000);

  mdio_sccd = 0;
  /* set operating frequency of MDC to 2.5MHz */
  XGMAC_SET_BITS (mdio_sccd, MAC_MDIOSCCDR, CR, 0);
  XGMAC_SET_BITS (mdio_sccd, MAC_MDIOSCCDR, CMD, 3);
  XGMAC_SET_BITS (mdio_sccd, MAC_MDIOSCCDR, BUSY, 1);
  XGMAC_IOWRITE32 (&pdata->xgmac_pdata, MAC_MDIOSCCDR, mdio_sccd);
  gBS->Stall (1000);

  delay_cnt = 0;
  while (!(XGMAC_IOREAD32_BITS (&pdata->xgmac_pdata, MAC_MDIOISR, SNGLCOMPINT))) {
    RegVal = XGMAC_IOREAD32 (&pdata->xgmac_pdata, MAC_MDIOISR);
    DEBUG ((DEBUG_ERROR, "++MDIO Debug isr 0x214 val: 0x%x \n", RegVal));

    gBS->Stall (1000);   // delay 1ms * 100 (1 seconds)
    delay_cnt++;
    if (delay_cnt > 100) {
      DEBUG ((DEBUG_ERROR, "++MDIO read time out \n"));
      RegVal = 0xFFFF;
      return RegVal;
      // break;
    }
  }

  return XGMAC_IOREAD32_BITS (&pdata->xgmac_pdata, MAC_MDIOSCCDR, DATA);
}

EFI_STATUS
XgbeWriteExtMiiRegs (
  ENET_DXE_PRIVATE_DATA *pdata,
  UINT32 addr,
  UINT32 reg,
  UINT32 val
  )
{
  UINT32  mdio_sca, mdio_sccd, RegVal;
  UINT32  delay_cnt;

  // reinit_completion(&pdata->mdio_complete);

  mdio_sca = XgbeCreateMdioSca (addr, reg);
  XGMAC_IOWRITE32 (&pdata->xgmac_pdata, MAC_MDIOSCAR, mdio_sca);

  gBS->Stall (1000);

  mdio_sccd = 0;
  /* set operating frequency of MDC to 2.5MHz */
  XGMAC_SET_BITS (mdio_sccd, MAC_MDIOSCCDR, CR, 5);
  XGMAC_SET_BITS (mdio_sccd, MAC_MDIOSCCDR, DATA, val);
  XGMAC_SET_BITS (mdio_sccd, MAC_MDIOSCCDR, CMD, 1);
  XGMAC_SET_BITS (mdio_sccd, MAC_MDIOSCCDR, BUSY, 1);
  XGMAC_IOWRITE32 (&pdata->xgmac_pdata, MAC_MDIOSCCDR, mdio_sccd);

  gBS->Stall (1000);
  delay_cnt = 0;
  while (!(XGMAC_IOREAD32_BITS (&pdata->xgmac_pdata, MAC_MDIOISR, SNGLCOMPINT))) {
    RegVal = XGMAC_IOREAD32 (&pdata->xgmac_pdata, MAC_MDIOISR);
    DEBUG ((DEBUG_ERROR, "++MDIO Debug isr 0x214 val: 0x%x \n", RegVal));

    gBS->Stall (1000);        // delay 1ms * 100 (100 ms)
    delay_cnt++;
    if (delay_cnt > 10) {
      DEBUG ((DEBUG_ERROR, "++MDIO write time out \n"));
      return EFI_DEVICE_ERROR;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PhyStartAneg (
  IN ENET_DXE_PRIVATE_DATA *Private
  )
{
  XGBE_PHY_DATA  *phy_data;
  UINT32         RegVal;

  phy_data = Private->phy_data;
  // phy generic config restart an
  RegVal  = XgbeReadExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_BMCR);
  RegVal |= BMCR_ANENABLE | BMCR_ANRESTART | BMCR_FULLDPLX;
  RegVal &= ~BMCR_ISOLATE;
  DEBUG ((DEBUG_ERROR, "++MDIO write-write External PHY regs0  ----------->  0x%X \n", RegVal));
  XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_BMCR, RegVal);

  // Config MV88E15X PHY
  if (MV88E15X_PHY_ID1 == XgbeReadExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_PHYSID1)) {
    // config device
    DEBUG ((DEBUG_ERROR, "++MDIO MVREVLL External PHY found..\n"));
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_BMCR,            0x8000);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_PAGE_ADDR,  0x12);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_COPPER_CR2, 0x1);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_COPPER_CR2, 0x8001);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_PAGE_ADDR,  0x0);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_PAGE_ADDR,  0x3);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_COPPER_CR1, 0x30);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_PAGE_ADDR,  0x0);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_BMCR,            0x0);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_PAGE_ADDR,  0x0);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_PAGE_ADDR,  0x2);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_PAGE_ADDR,  0x4448);
    //config negotiation
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_PAGE_ADDR,  0x0);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_BMCR,            0x8000);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_ADVERTISE,       0x101);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_CTRL1000,        0x200);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_BMCR,            0x1200);
  
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_PAGE_ADDR,  0x0);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_PAGE_ADDR,  0x2);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_COPPER_CR1, 0x4448);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MV88E15X_PAGE_ADDR,  0x0);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_BMCR,            0x8000);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_CTRL1000,        0x200);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_BMCR,            0x1200);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_BMCR,            0x1340);
  }

  // Config CENTEC 4 PORT PHY
  if ((CENTEC_PHY_4C_ID1 == XgbeReadExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_PHYSID1)) &&
      (CENTEC_PHY_4C_ID2 == XgbeReadExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_PHYSID2))) {
    DEBUG ((DEBUG_ERROR, "++MDIO CENTEC 4 PORT External PHY found..\n"));
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, CENTEC_PHY_EXT_INDEX, 0x41);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, CENTEC_PHY_EXT_DATA,  0x33);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, CENTEC_PHY_EXT_INDEX, 0x42);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, CENTEC_PHY_EXT_DATA,  0x66);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, CENTEC_PHY_EXT_INDEX, 0x43);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, CENTEC_PHY_EXT_DATA,  0xaa);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, CENTEC_PHY_EXT_INDEX, 0x44);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, CENTEC_PHY_EXT_DATA,  0xd0d);

    if (2 == phy_data->mdio_addr) {
      XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, CENTEC_PHY_EXT_INDEX, 0x57);
      XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, CENTEC_PHY_EXT_DATA,  0x2929);
    }

    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, CENTEC_PHY_EXT_INDEX, 0x51);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, CENTEC_PHY_EXT_DATA,  0x439);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_BMCR, 0x9140);

    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_BMCR,     0x8000);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_BMCR,     0x0);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_BMCR,     0x1200);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_CTRL1000, 0x600);
    XgbeWriteExtMiiRegs (Private, (UINT32)phy_data->mdio_addr, MII_BMCR,     0x1200);
  }

  return EFI_SUCCESS;
}
