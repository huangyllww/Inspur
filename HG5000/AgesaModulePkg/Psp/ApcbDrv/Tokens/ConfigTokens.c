/* $NoKeywords:$ */
/**
 * @file
 *
 * APCB DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  APCB
 * @e \$Revision$   @e \$Date$
 *
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
 ******************************************************************************
 */
#include "AGESA.h"
#include "Firmwares/ZP/APCB.h"
#include "ApcbCommon.h"
#include <Library/IdsLib.h>
#include <Filecode.h>

#define FILECODE PSP_APCB_TOKENS_CONFIG_TOKENS_FILECODE

//
// APCB Config Token ID definitions
//
TOKEN_PAIR ConfigTokenPair[] = {
  { APCB_ID_CONFIG_CCX_MIN_SEV_ASID,                APCB_TOKEN_CONFIG_CCX_MIN_SEV_ASID                   },
  { APCB_ID_CONFIG_DF_GMI_ENCRYPT,                  APCB_TOKEN_CONFIG_DF_GMI_ENCRYPT                     },
  { APCB_ID_CONFIG_DF_XGMI_ENCRYPT,                 APCB_TOKEN_CONFIG_DF_XGMI_ENCRYPT                    },
  { APCB_ID_CONFIG_DF_SAVE_RESTORE_MEM_ENCRYPT,     APCB_TOKEN_CONFIG_DF_SAVE_RESTORE_MEM_ENCRYPT        },
  { APCB_ID_CONFIG_DF_SYS_STORAGE_AT_TOP_OF_MEM,    APCB_TOKEN_CONFIG_DF_SYS_STORAGE_AT_TOP_OF_MEM       },
  { APCB_ID_CONFIG_DF_PROBE_FILTER_ENABLE,          APCB_TOKEN_CONFIG_DF_PROBE_FILTER_ENABLE             },
  { APCB_ID_CONFIG_DF_BOTTOMIO,                     APCB_TOKEN_CONFIG_DF_BOTTOMIO                        },
  { APCB_ID_CONFIG_DF_MEM_INTERLEAVING,             APCB_TOKEN_CONFIG_DF_MEM_INTERLEAVING                },
  { APCB_ID_CONFIG_DF_DRAM_INTERLEAVE_SIZE,         APCB_TOKEN_CONFIG_DF_DRAM_INTERLEAVE_SIZE            },
  { APCB_ID_CONFIG_DF_ENABLE_CHAN_INTLV_HASH,       APCB_TOKEN_CONFIG_DF_ENABLE_CHAN_INTLV_HASH          },
  { APCB_ID_CONFIG_DF_PCI_MMIO_SIZE,                APCB_TOKEN_CONFIG_DF_PCI_MMIO_SIZE                   },
  { APCB_ID_CONFIG_DF_CAKE_CRC_THRESH_PERF_BOUNDS,  APCB_TOKEN_CONFIG_DF_CAKE_CRC_THRESH_PERF_BOUNDS     },
  { APCB_ID_CONFIG_DF_MEM_CLEAR,                    APCB_TOKEN_CONFIG_DF_MEM_CLEAR                       },
  { APCB_ID_CONFIG_MEMHOLEREMAPPING,                APCB_TOKEN_CONFIG_MEMHOLEREMAPPING                   },
  { APCB_ID_CONFIG_LIMITMEMORYTOBELOW1TB,           APCB_TOKEN_CONFIG_LIMITMEMORYTOBELOW1TB              },
  { APCB_ID_CONFIG_USERTIMINGMODE,                  APCB_TOKEN_CONFIG_USERTIMINGMODE                     },
  { APCB_ID_CONFIG_MEMCLOCKVALUE,                   APCB_TOKEN_CONFIG_MEMCLOCKVALUE                      },
  { APCB_ID_CONFIG_ENABLECHIPSELECTINTLV,           APCB_TOKEN_CONFIG_ENABLECHIPSELECTINTLV              },
  { APCB_ID_CONFIG_ENABLEECCFEATURE,                APCB_TOKEN_CONFIG_ENABLEECCFEATURE                   },
  { APCB_ID_CONFIG_ENABLEPOWERDOWN,                 APCB_TOKEN_CONFIG_ENABLEPOWERDOWN                    },
  { APCB_ID_CONFIG_ENABLEPARITY,                    APCB_TOKEN_CONFIG_ENABLEPARITY                       },
  { APCB_ID_CONFIG_ENABLEBANKSWIZZLE,               APCB_TOKEN_CONFIG_ENABLEBANKSWIZZLE                  },
  { APCB_ID_CONFIG_ENABLEMEMCLR,                    APCB_TOKEN_CONFIG_ENABLEMEMCLR                       },
  { APCB_ID_CONFIG_UMAMODE,                         APCB_TOKEN_CONFIG_UMAMODE                            },
  { APCB_ID_CONFIG_UMASIZE,                         APCB_TOKEN_CONFIG_UMASIZE                            },
  { APCB_ID_CONFIG_MEMRESTORECTL,                   APCB_TOKEN_CONFIG_MEMRESTORECTL                      },
  { APCB_ID_CONFIG_SAVEMEMCONTEXTCTL,               APCB_TOKEN_CONFIG_SAVEMEMCONTEXTCTL                  },
  { APCB_ID_CONFIG_ISCAPSULEMODE,                   APCB_TOKEN_CONFIG_ISCAPSULEMODE                      },
  { APCB_ID_CONFIG_FORCETRAINMODE,                  APCB_TOKEN_CONFIG_FORCETRAINMODE                     },
  { APCB_ID_CONFIG_DIMMTYPEUSEDINMIXEDCONFIG,       APCB_TOKEN_CONFIG_DIMMTYPEUSEDINMIXEDCONFIG          },
  { APCB_ID_CONFIG_AMPENABLE,                       APCB_TOKEN_CONFIG_AMPENABLE                          },
  { APCB_ID_CONFIG_DRAMDOUBLEREFRESHRATE,           APCB_TOKEN_CONFIG_DRAMDOUBLEREFRESHRATE              },
  { APCB_ID_CONFIG_PMUTRAINMODE,                    APCB_TOKEN_CONFIG_PMUTRAINMODE                       },
  { APCB_ID_CONFIG_ECCREDIRECTION,                  APCB_TOKEN_CONFIG_ECCREDIRECTION                     },
  { APCB_ID_CONFIG_SCRUBDRAMRATE,                   APCB_TOKEN_CONFIG_SCRUBDRAMRATE                      },
  { APCB_ID_CONFIG_SCRUBL2RATE,                     APCB_TOKEN_CONFIG_SCRUBL2RATE                        },
  { APCB_ID_CONFIG_SCRUBL3RATE,                     APCB_TOKEN_CONFIG_SCRUBL3RATE                        },
  { APCB_ID_CONFIG_SCRUBICRATE,                     APCB_TOKEN_CONFIG_SCRUBICRATE                        },
  { APCB_ID_CONFIG_SCRUBDCRATE,                     APCB_TOKEN_CONFIG_SCRUBDCRATE                        },
  { APCB_ID_CONFIG_ECCSYNCFLOOD,                    APCB_TOKEN_CONFIG_ECCSYNCFLOOD                       },
  { APCB_ID_CONFIG_ECCSYMBOLSIZE,                   APCB_TOKEN_CONFIG_ECCSYMBOLSIZE                      },
  { APCB_ID_CONFIG_DQSTRAININGCONTROL,              APCB_TOKEN_CONFIG_DQSTRAININGCONTROL                 },
  { APCB_ID_CONFIG_UMAABOVE4G,                      APCB_TOKEN_CONFIG_UMAABOVE4G                         },
  { APCB_ID_CONFIG_UMAALIGNMENT,                    APCB_TOKEN_CONFIG_UMAALIGNMENT                       },
  { APCB_ID_CONFIG_MEMORYALLCLOCKSON,               APCB_TOKEN_CONFIG_MEMORYALLCLOCKSON                  },
  { APCB_ID_CONFIG_MEMORYBUSFREQUENCYLIMIT,         APCB_TOKEN_CONFIG_MEMORYBUSFREQUENCYLIMIT            },
  { APCB_ID_CONFIG_POWERDOWNMODE,                   APCB_TOKEN_CONFIG_POWERDOWNMODE                      },
  { APCB_ID_CONFIG_IGNORESPDCHECKSUM,               APCB_TOKEN_CONFIG_IGNORESPDCHECKSUM                  },
  { APCB_ID_CONFIG_MEMORYMODEUNGANGED,              APCB_TOKEN_CONFIG_MEMORYMODEUNGANGED                 },
  { APCB_ID_CONFIG_MEMORYQUADRANKCAPABLE,           APCB_TOKEN_CONFIG_MEMORYQUADRANKCAPABLE              },
  { APCB_ID_CONFIG_MEMORYRDIMMCAPABLE,              APCB_TOKEN_CONFIG_MEMORYRDIMMCAPABLE                 },
  { APCB_ID_CONFIG_MEMORYLRDIMMCAPABLE,             APCB_TOKEN_CONFIG_MEMORYLRDIMMCAPABLE                },
  { APCB_ID_CONFIG_MEMORYUDIMMCAPABLE,              APCB_TOKEN_CONFIG_MEMORYUDIMMCAPABLE                 },
  { APCB_ID_CONFIG_MEMORYSODIMMCAPABLE,             APCB_TOKEN_CONFIG_MEMORYSODIMMCAPABLE                },
  { APCB_ID_CONFIG_DRAMDOUBLEREFRESHRATEEN,         APCB_TOKEN_CONFIG_DRAMDOUBLEREFRESHRATEEN            },
  { APCB_ID_CONFIG_DIMMTYPEDDDR4CAPABLE,            APCB_TOKEN_CONFIG_DIMMTYPEDDDR4CAPABLE               },
  { APCB_ID_CONFIG_DIMMTYPEDDDR3CAPABLE,            APCB_TOKEN_CONFIG_DIMMTYPEDDDR3CAPABLE               },
  { APCB_ID_CONFIG_DIMMTYPELPDDDR3CAPABLE,          APCB_TOKEN_CONFIG_DIMMTYPELPDDDR3CAPABLE             },
  { APCB_ID_CONFIG_ENABLEZQRESET,                   APCB_TOKEN_CONFIG_ENABLEZQRESET                      },
  { APCB_ID_CONFIG_ENABLEBANKGROUPSWAP,             APCB_TOKEN_CONFIG_ENABLEBANKGROUPSWAP                },
  { APCB_ID_CONFIG_ODTSCMDTHROTEN,                  APCB_TOKEN_CONFIG_ODTSCMDTHROTEN                     },
  { APCB_ID_CONFIG_SWCMDTHROTEN,                    APCB_TOKEN_CONFIG_SWCMDTHROTEN                       },
  { APCB_ID_CONFIG_FORCEPWRDOWNTHROTEN,             APCB_TOKEN_CONFIG_FORCEPWRDOWNTHROTEN                },
  { APCB_ID_CONFIG_ODTSCMDTHROTCYC,                 APCB_TOKEN_CONFIG_ODTSCMDTHROTCYC                    },
  { APCB_ID_CONFIG_SWCMDTHROTCYC,                   APCB_TOKEN_CONFIG_SWCMDTHROTCYC                      },
  { APCB_ID_CONFIG_DIMMSENSORCONF,                  APCB_TOKEN_CONFIG_DIMMSENSORCONF                     },
  { APCB_ID_CONFIG_DIMMSENSORUPPER,                 APCB_TOKEN_CONFIG_DIMMSENSORUPPER                    },
  { APCB_ID_CONFIG_DIMMSENSORLOWER,                 APCB_TOKEN_CONFIG_DIMMSENSORLOWER                    },
  { APCB_ID_CONFIG_DIMMSENSORCRITICAL,              APCB_TOKEN_CONFIG_DIMMSENSORCRITICAL                 },
  { APCB_ID_CONFIG_DIMMSENSORRESOLUTION,            APCB_TOKEN_CONFIG_DIMMSENSORRESOLUTION               },
  { APCB_ID_CONFIG_AUTOREFFINEGRANMODE,             APCB_TOKEN_CONFIG_AUTOREFFINEGRANMODE                },
  { APCB_ID_CONFIG_ENABLEMEMPSTATE,                 APCB_TOKEN_CONFIG_ENABLEMEMPSTATE                    },
  { APCB_ID_CONFIG_SOLDERDOWNDRAM,                  APCB_TOKEN_CONFIG_SOLDERDOWNDRAM                     },
  { APCB_ID_CONFIG_DDRROUTEBALANCEDTEE,             APCB_TOKEN_CONFIG_DDRROUTEBALANCEDTEE                },
  { APCB_ID_CONFIG_MEM_MBIST_TEST_ENABLE,           APCB_TOKEN_CONFIG_MEM_MBIST_TEST_ENABLE              },
  //{ APCB_ID_CONFIG_MEM_MBIST_TESTMODE,          APCB_TOKEN_CONFIG_MEM_MBIST_TESTMODE             }, 
  { APCB_ID_CONFIG_MEM_MBIST_SUBTEST_TYPE,          APCB_TOKEN_CONFIG_MEM_MBIST_SUBTEST_TYPE             },
  { APCB_ID_CONFIG_MEM_MBIST_AGGRESOR_ON,           APCB_TOKEN_CONFIG_MEM_MBIST_AGGRESOR_ON              },
  { APCB_ID_CONFIG_MEM_MBIST_HALT_ON_ERROR,         APCB_TOKEN_CONFIG_MEM_MBIST_HALT_ON_ERROR            },
  { APCB_ID_CONFIG_MEM_CPU_VREF_RANGE,              APCB_TOKEN_CONFIG_MEM_CPU_VREF_RANGE                 },
  { APCB_ID_CONFIG_MEM_DRAM_VREF_RANGE,             APCB_TOKEN_CONFIG_MEM_DRAM_VREF_RANGE                },
  { APCB_ID_CONFIG_MEM_TSME_ENABLE,                 APCB_TOKEN_CONFIG_MEM_TSME_ENABLE                    },
  { APCB_ID_CONFIG_MEM_NVDIMM_POWER_SOURCE,         APCB_TOKEN_CONFIG_MEM_NVDIMM_POWER_SOURCE            },
  { APCB_ID_CONFIG_MEM_DATA_POISON,                 APCB_TOKEN_CONFIG_MEM_DATA_POISON                    },
  { APCB_ID_CONFIG_MEM_DATA_SCRAMBLE,               APCB_TOKEN_CONFIG_MEM_DATA_SCRAMBLE                  },
  { APCB_ID_CONFIG_BMC_SOCKET_NUMBER,               APCB_TOKEN_CONFIG_BMC_SOCKET_NUMBER                  },
  { APCB_ID_CONFIG_BMC_START_LANE,                  APCB_TOKEN_CONFIG_BMC_START_LANE                     },
  { APCB_ID_CONFIG_BMC_END_LANE,                    APCB_TOKEN_CONFIG_BMC_END_LANE                       },
  { APCB_ID_CONFIG_BMC_DEVICE,                      APCB_TOKEN_CONFIG_BMC_DEVICE                         },
  { APCB_ID_CONFIG_BMC_FUNCTION,                    APCB_TOKEN_CONFIG_BMC_FUNCTION                       },
  { APCB_ID_CONFIG_FCH_CONSOLE_OUT_ENABLE,          APCB_TOKEN_CONFIG_FCH_CONSOLE_OUT_ENABLE             },
  { APCB_ID_CONFIG_FCH_CONSOLE_OUT_SERIAL_PORT,     APCB_TOKEN_CONFIG_FCH_CONSOLE_OUT_SERIAL_PORT        },
};

UINT16
mTranslateConfigTokenId (
  IN       UINT16             CommonId
  )
{
  UINT16 i;

  for (i = 0; i < sizeof (ConfigTokenPair) / sizeof (TOKEN_PAIR); i++) {
    if (CommonId == ConfigTokenPair[i].ApcbCommonId) {
      return ConfigTokenPair[i].ApcbToken;
    }
  }

  ASSERT (FALSE);

  return 0xFFFF;
}
