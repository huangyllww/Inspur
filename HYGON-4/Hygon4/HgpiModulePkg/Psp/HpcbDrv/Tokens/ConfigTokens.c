/* $NoKeywords:$ */

/**
 * @file
 *
 * HPCB DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HPCB
 *
 */
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
#include "HGPI.h"
#include <HPCB.h>
#include "HpcbCommon.h"
#include <Library/IdsLib.h>
#include <Filecode.h>

#define FILECODE  PSP_HPCB_TOKENS_CONFIG_TOKENS_FILECODE

//
// HPCB Config Token ID definitions
//
TOKEN_PAIR  ConfigTokenPair[] = {
  { HPCB_ID_CONFIG_CCX_MIN_CSV_ASID,               HPCB_TOKEN_CONFIG_CCX_MIN_CSV_ASID                     },
  { HPCB_ID_CONFIG_DF_HMI_ENCRYPT,                 HPCB_TOKEN_CONFIG_DF_HMI_ENCRYPT                       },
  { HPCB_ID_CONFIG_DF_XHMI_ENCRYPT,                HPCB_TOKEN_CONFIG_DF_XHMI_ENCRYPT                      },
  { HPCB_ID_CONFIG_DF_SAVE_RESTORE_MEM_ENCRYPT,    HPCB_TOKEN_CONFIG_DF_SAVE_RESTORE_MEM_ENCRYPT          },
  { HPCB_ID_CONFIG_DF_SYS_STORAGE_AT_TOP_OF_MEM,   HPCB_TOKEN_CONFIG_DF_SYS_STORAGE_AT_TOP_OF_MEM         },
  { HPCB_ID_CONFIG_DF_PROBE_FILTER_ENABLE,         HPCB_TOKEN_CONFIG_DF_PROBE_FILTER_ENABLE               },
  { HPCB_ID_CONFIG_DF_BOTTOMIO,                    HPCB_TOKEN_CONFIG_DF_BOTTOMIO                          },
  { HPCB_ID_CONFIG_DF_MEM_INTERLEAVING,            HPCB_TOKEN_CONFIG_DF_MEM_INTERLEAVING                  },
  { HPCB_ID_CONFIG_DF_DRAM_INTERLEAVE_SIZE,        HPCB_TOKEN_CONFIG_DF_DRAM_INTERLEAVE_SIZE              },
  { HPCB_ID_CONFIG_DF_ENABLE_CHAN_INTLV_HASH,      HPCB_TOKEN_CONFIG_DF_ENABLE_CHAN_INTLV_HASH            },
  { HPCB_ID_CONFIG_DF_PCI_MMIO_SIZE,               HPCB_TOKEN_CONFIG_DF_PCI_MMIO_SIZE                     },
  { HPCB_ID_CONFIG_DF_CAKE_CRC_THRESH_PERF_BOUNDS, HPCB_TOKEN_CONFIG_DF_CAKE_CRC_THRESH_PERF_BOUNDS       },
  { HPCB_ID_CONFIG_DF_MEM_CLEAR,                   HPCB_TOKEN_CONFIG_DF_MEM_CLEAR_DISABLE                 },
  { HPCB_ID_CONFIG_DF_PCI_BUS_ASSIGN_NUMBER,       HPCB_TOKEN_CONFIG_DF_PCI_BUS_ASSIGN_NUMBER             },
  { HPCB_ID_CONFIG_MEMHOLEREMAPPING,               HPCB_TOKEN_CONFIG_MEMHOLEREMAPPING                     },
  { HPCB_ID_CONFIG_LIMITMEMORYTOBELOW1TB,          HPCB_TOKEN_CONFIG_LIMITMEMORYTOBELOW1TB                },
  { HPCB_ID_CONFIG_USERTIMINGMODE,                 HPCB_TOKEN_CONFIG_USERTIMINGMODE                       },
  { HPCB_ID_CONFIG_MEMCLOCKVALUE,                  HPCB_TOKEN_CONFIG_MEMCLOCKVALUE                        },
  { HPCB_ID_CONFIG_ENABLECHIPSELECTINTLV,          HPCB_TOKEN_CONFIG_ENABLECHIPSELECTINTLV                },
  { HPCB_ID_CONFIG_ENABLEECCFEATURE,               HPCB_TOKEN_CONFIG_ENABLEECCFEATURE                     },
  { HPCB_ID_CONFIG_ENABLEPOWERDOWN,                HPCB_TOKEN_CONFIG_ENABLEPOWERDOWN                      },
  { HPCB_ID_CONFIG_ENABLEPARITY,                   HPCB_TOKEN_CONFIG_ENABLEPARITY                         },
  { HPCB_ID_CONFIG_ENABLEBANKSWIZZLE,              HPCB_TOKEN_CONFIG_ENABLEBANKSWIZZLE                    },
  { HPCB_ID_CONFIG_ENABLEMEMCLR,                   HPCB_TOKEN_CONFIG_ENABLEMEMCLR                         },
  { HPCB_ID_CONFIG_UMAMODE,                        HPCB_TOKEN_CONFIG_UMAMODE                              },
  { HPCB_ID_CONFIG_UMASIZE,                        HPCB_TOKEN_CONFIG_UMASIZE                              },
  { HPCB_ID_CONFIG_MEMRESTORECTL,                  HPCB_TOKEN_CONFIG_MEMRESTORECTL                        },
  { HPCB_ID_CONFIG_SAVEMEMCONTEXTCTL,              HPCB_TOKEN_CONFIG_SAVEMEMCONTEXTCTL                    },
  { HPCB_ID_CONFIG_ISCAPSULEMODE,                  HPCB_TOKEN_CONFIG_ISCAPSULEMODE                        },
  { HPCB_ID_CONFIG_FORCETRAINMODE,                 HPCB_TOKEN_CONFIG_FORCETRAINMODE                       },
  { HPCB_ID_CONFIG_DIMMTYPEUSEDINMIXEDCONFIG,      HPCB_TOKEN_CONFIG_DIMMTYPEUSEDINMIXEDCONFIG            },
  { HPCB_ID_CONFIG_AMPENABLE,                      HPCB_TOKEN_CONFIG_AMPENABLE                            },
  { HPCB_ID_CONFIG_DRAMDOUBLEREFRESHRATE,        HPCB_TOKEN_CONFIG_DRAMDOUBLEREFRESHRATE              },
  { HPCB_ID_CONFIG_PMUTRAINMODE,                   HPCB_TOKEN_CONFIG_PMUTRAINMODE                         },
  { HPCB_ID_CONFIG_ECCREDIRECTION,                 HPCB_TOKEN_CONFIG_ECCREDIRECTION                       },
  { HPCB_ID_CONFIG_SCRUBDRAMRATE,                  HPCB_TOKEN_CONFIG_SCRUBDRAMRATE                        },
  { HPCB_ID_CONFIG_SCRUBL2RATE,                    HPCB_TOKEN_CONFIG_SCRUBL2RATE                          },
  { HPCB_ID_CONFIG_SCRUBL3RATE,                    HPCB_TOKEN_CONFIG_SCRUBL3RATE                          },
  { HPCB_ID_CONFIG_SCRUBICRATE,                    HPCB_TOKEN_CONFIG_SCRUBICRATE                          },
  { HPCB_ID_CONFIG_SCRUBDCRATE,                    HPCB_TOKEN_CONFIG_SCRUBDCRATE                          },
  { HPCB_ID_CONFIG_ECCSYNCFLOOD,                   HPCB_TOKEN_CONFIG_ECCSYNCFLOOD                         },
  { HPCB_ID_CONFIG_ECCSYMBOLSIZE,                  HPCB_TOKEN_CONFIG_ECCSYMBOLSIZE                        },
  { HPCB_ID_CONFIG_DQSTRAININGCONTROL,             HPCB_TOKEN_CONFIG_DQSTRAININGCONTROL                   },
  { HPCB_ID_CONFIG_UMAABOVE4G,                     HPCB_TOKEN_CONFIG_UMAABOVE4G                           },
  { HPCB_ID_CONFIG_UMAALIGNMENT,                   HPCB_TOKEN_CONFIG_UMAALIGNMENT                         },
  { HPCB_ID_CONFIG_MEMORYALLCLOCKSON,              HPCB_TOKEN_CONFIG_MEMORYALLCLOCKSON                    },
  { HPCB_ID_CONFIG_MEMORYBUSFREQUENCYLIMIT,        HPCB_TOKEN_CONFIG_MEMORYBUSFREQUENCYLIMIT              },
  { HPCB_ID_CONFIG_POWERDOWNMODE,                  HPCB_TOKEN_CONFIG_POWERDOWNMODE                        },
  { HPCB_ID_CONFIG_IGNORESPDCHECKSUM,              HPCB_TOKEN_CONFIG_IGNORESPDCHECKSUM                    },
  { HPCB_ID_CONFIG_MEMORYMODEUNGANGED,             HPCB_TOKEN_CONFIG_MEMORYMODEUNGANGED                   },
  { HPCB_ID_CONFIG_MEMORYQUADRANKCAPABLE,          HPCB_TOKEN_CONFIG_MEMORYQUADRANKCAPABLE                },
  { HPCB_ID_CONFIG_MEMORYRDIMMCAPABLE,             HPCB_TOKEN_CONFIG_MEMORYRDIMMCAPABLE                   },
  { HPCB_ID_CONFIG_MEMORYLRDIMMCAPABLE,            HPCB_TOKEN_CONFIG_MEMORYLRDIMMCAPABLE                  },
  { HPCB_ID_CONFIG_MEMORYUDIMMCAPABLE,             HPCB_TOKEN_CONFIG_MEMORYUDIMMCAPABLE                   },
  { HPCB_ID_CONFIG_MEMORYSODIMMCAPABLE,            HPCB_TOKEN_CONFIG_MEMORYSODIMMCAPABLE                  },
  { HPCB_ID_CONFIG_DRAMDOUBLEREFRESHRATEEN,        HPCB_TOKEN_CONFIG_DRAMDOUBLEREFRESHRATEEN              },
  { HPCB_ID_CONFIG_DIMMTYPEDDDR5CAPABLE,           HPCB_TOKEN_CONFIG_DIMMTYPEDDDR5CAPABLE                 },
  { HPCB_ID_CONFIG_ENABLEZQRESET,                  HPCB_TOKEN_CONFIG_ENABLEZQRESET                        },
  { HPCB_ID_CONFIG_ENABLEBANKGROUPSWAP,            HPCB_TOKEN_CONFIG_ENABLEBANKGROUPSWAP                  },
  { HPCB_ID_CONFIG_ODTSCMDTHROTEN,                 HPCB_TOKEN_CONFIG_ODTSCMDTHROTEN                       },
  { HPCB_ID_CONFIG_SWCMDTHROTEN,                   HPCB_TOKEN_CONFIG_SWCMDTHROTEN                         },
  { HPCB_ID_CONFIG_FORCEPWRDOWNTHROTEN,            HPCB_TOKEN_CONFIG_FORCEPWRDOWNTHROTEN                  },
  { HPCB_ID_CONFIG_ODTSCMDTHROTCYC,                HPCB_TOKEN_CONFIG_ODTSCMDTHROTCYC                      },
  { HPCB_ID_CONFIG_SWCMDTHROTCYC,                  HPCB_TOKEN_CONFIG_SWCMDTHROTCYC                        },
  { HPCB_ID_CONFIG_DIMMSENSORCONF,                 HPCB_TOKEN_CONFIG_DIMMSENSORCONF                       },
  { HPCB_ID_CONFIG_DIMMSENSORUPPER,                HPCB_TOKEN_CONFIG_DIMMSENSORUPPER                      },
  { HPCB_ID_CONFIG_DIMMSENSORLOWER,                HPCB_TOKEN_CONFIG_DIMMSENSORLOWER                      },
  { HPCB_ID_CONFIG_DIMMSENSORCRITICAL,             HPCB_TOKEN_CONFIG_DIMMSENSORCRITICAL                   },
  { HPCB_ID_CONFIG_DIMMSENSORRESOLUTION,           HPCB_TOKEN_CONFIG_DIMMSENSORRESOLUTION                 },
  { HPCB_ID_CONFIG_AUTOREFFINEGRANMODE,            HPCB_TOKEN_CONFIG_AUTOREFFINEGRANMODE                  },
  { HPCB_ID_CONFIG_ENABLEMEMPSTATE,                HPCB_TOKEN_CONFIG_ENABLEMEMPSTATE                      },
  { HPCB_ID_CONFIG_SOLDERDOWNDRAM,                 HPCB_TOKEN_CONFIG_SOLDERDOWNDRAM                       },
  { HPCB_ID_CONFIG_DDRROUTEBALANCEDTEE,            HPCB_TOKEN_CONFIG_DDRROUTEBALANCEDTEE                  },
  { HPCB_ID_CONFIG_MEM_MBIST_TEST_ENABLE,          HPCB_TOKEN_CONFIG_MEM_MBIST_TEST_ENABLE                },
  // { HPCB_ID_CONFIG_MEM_MBIST_TESTMODE,          HPCB_TOKEN_CONFIG_MEM_MBIST_TESTMODE             },
  { HPCB_ID_CONFIG_MEM_MBIST_SUBTEST_TYPE,         HPCB_TOKEN_CONFIG_MEM_MBIST_SUBTEST_TYPE               },
  { HPCB_ID_CONFIG_MEM_MBIST_AGGRESOR_ON,          HPCB_TOKEN_CONFIG_MEM_MBIST_AGGRESOR_ON                },
  { HPCB_ID_CONFIG_MEM_MBIST_HALT_ON_ERROR,        HPCB_TOKEN_CONFIG_MEM_MBIST_HALT_ON_ERROR              },
  { HPCB_ID_CONFIG_MEM_CPU_VREF_RANGE,             HPCB_TOKEN_CONFIG_MEM_CPU_VREF_RANGE                   },
  { HPCB_ID_CONFIG_MEM_DRAM_VREF_RANGE,            HPCB_TOKEN_CONFIG_MEM_DRAM_VREF_RANGE                  },
  { HPCB_ID_CONFIG_MEM_TSME_ENABLE,                HPCB_TOKEN_CONFIG_MEM_TSME_ENABLE                      },
  { HPCB_ID_CONFIG_MEM_NVDIMM_POWER_SOURCE,        HPCB_TOKEN_CONFIG_MEM_NVDIMM_POWER_SOURCE              },
  { HPCB_ID_CONFIG_MEM_DATA_POISON,                HPCB_TOKEN_CONFIG_MEM_DATA_POISON                      },
  { HPCB_ID_CONFIG_MEM_DATA_SCRAMBLE,              HPCB_TOKEN_CONFIG_MEM_DATA_SCRAMBLE                    },
  { HPCB_ID_CONFIG_BMC_SOCKET_NUMBER,              HPCB_TOKEN_CONFIG_BMC_SOCKET_NUMBER                    },
  { HPCB_ID_CONFIG_BMC_START_LANE,                 HPCB_TOKEN_CONFIG_BMC_START_LANE                       },
  { HPCB_ID_CONFIG_BMC_END_LANE,                   HPCB_TOKEN_CONFIG_BMC_END_LANE                         },
  { HPCB_ID_CONFIG_BMC_DEVICE,                     HPCB_TOKEN_CONFIG_BMC_DEVICE                           },
  { HPCB_ID_CONFIG_BMC_FUNCTION,                   HPCB_TOKEN_CONFIG_BMC_FUNCTION                         },
  { HPCB_ID_CONFIG_FCH_CONSOLE_OUT_ENABLE,         HPCB_TOKEN_CONFIG_FCH_CONSOLE_OUT_ENABLE               },
  { HPCB_ID_CONFIG_FCH_CONSOLE_OUT_SERIAL_PORT,    HPCB_TOKEN_CONFIG_FCH_CONSOLE_OUT_SERIAL_PORT          },
};

UINT16
mTranslateConfigTokenId (
  IN       UINT16             CommonId
  )
{
  UINT16  i;

  for (i = 0; i < sizeof (ConfigTokenPair) / sizeof (TOKEN_PAIR); i++) {
    if (CommonId == ConfigTokenPair[i].HpcbCommonId) {
      return ConfigTokenPair[i].HpcbToken;
    }
  }

  ASSERT (FALSE);

  return 0xFFFF;
}
