@echo off


if /I "%1" == "" (
  goto :EOF
)
if /I "%2" == "" (
  goto :EOF
)

set OUTPUT_FILE=%2
set PSP_APCB_FILENAME=ProjectPkg\Apcb\Release\APCB_DN_D4.bin

set SPI_BLOCK_SIZE=0x1000
set FLASH_BLOCK_SIZE=0x1000

set PSP_DATA_OFFSET=%1
set PSP_DATA_L1_SIZE=0x180000
set PSP_DATA_L2_SIZE=0x100000
set BIOSDIR_DATA_L1_SIZE=0x32000
set BIOSDIR_DATA_L2_SIZE=0xBB000

set HIGH_MEMORY_APOB_BASE=0x4000000

set FV_BB_OFFSET=0xCF0000
set FV_BB_SIZE=0x310000

set HIGH_MEMORY_REGION_BASE=0x1000000
set /A HIGH_MEMORY_REGION_BB_BASE=%HIGH_MEMORY_REGION_BASE% + %FV_BB_OFFSET%


set /A BIOSDIR_DATA_L1_OFFSET=%PSP_DATA_OFFSET% + %PSP_DATA_L1_SIZE%
set /A PSPDIR_L2_OFFSET=%PSP_DATA_OFFSET% + %PSP_DATA_L1_SIZE% + %BIOSDIR_DATA_L1_SIZE%
set /A BIOSDIR_DATA_L2_OFFSET=%PSPDIR_L2_OFFSET% + %PSP_DATA_L2_SIZE%


set /A PSP_BOOT_LOADER_OFFSET=%PSPDIR_L2_OFFSET% + 0x400


set Firmwares_DIR_ZP=AgesaModulePkg\Firmwares\DP\DNMcm_MP
set PUB_KEY_FILENAME=%Firmwares_DIR_ZP%\HygonPubKeyDN.bin
set SMU_FIRMWARE_FILENAME=%Firmwares_DIR_ZP%\SmuFirmwareMcmDN.esbin
rem set ZEPPELIN_SECURE_DEBUG_FILENAME=%Firmwares_DIR_ZP%\ZeppelinSecureDebug.tkn
rem set PSP_ABL_SIGNED_KEY_FILENAME=%Firmwares_DIR_ZP%\ZeppelinOemAblFw.tkn
set PSP_SOFT_FUSE_CHAIN=0x01
set SMU_FIRMWARE2_FILENAME=%Firmwares_DIR_ZP%\SmuFirmware2McmDN.esbin
set DEBUG_UNLOCK_FILENAME=%Firmwares_DIR_ZP%\DebugUnlock_DN.esbin
set PSP_IKEK_FILENAME=%Firmwares_DIR_ZP%\PspIkekDN.bin
set SECURE_EMPTY_TOKEN_FILENAME=%Firmwares_DIR_ZP%\SecureEmptyToken.bin

set PSP_AGESA_BOOT_LOADER_0_FILENAME=%Firmwares_DIR_ZP%\AgesaBootloader0_prod_Mcm_DN.esbin
set PSP_AGESA_BOOT_LOADER_1_FILENAME=%Firmwares_DIR_ZP%\AgesaBootloader1_prod_Mcm_DN.esbin
set PSP_AGESA_BOOT_LOADER_2_FILENAME=%Firmwares_DIR_ZP%\AgesaBootloader2_prod_Mcm_DN.esbin
set PSP_AGESA_BOOT_LOADER_3_FILENAME=%Firmwares_DIR_ZP%\AgesaBootloader3_prod_Mcm_DN.esbin
set PSP_AGESA_BOOT_LOADER_4_FILENAME=%Firmwares_DIR_ZP%\AgesaBootloader4_prod_Mcm_DN.esbin
set PSP_AGESA_BOOT_LOADER_5_FILENAME=%Firmwares_DIR_ZP%\AgesaBootloader5_prod_Mcm_DN.esbin
set PSP_AGESA_BOOT_LOADER_6_FILENAME=%Firmwares_DIR_ZP%\AgesaBootloader6_prod_Mcm_DN.esbin

set PSP_APCB_SIZE=0x1D00

set PSP_APPB_ZP_1D_DDR4_UDIMM_IMEM_FILENAME=%Firmwares_DIR_ZP%\Appb_Dn_1D_Ddr4_Udimm_Imem_Mcm.esbin
set PSP_APPB_ZP_1D_DDR4_UDIMM_DMEM_FILENAME=%Firmwares_DIR_ZP%\Appb_Dn_1D_Ddr4_Udimm_Dmem_Mcm.esbin
set PSP_APPB_ZP_1D_DDR4_RDIMM_IMEM_FILENAME=%Firmwares_DIR_ZP%\Appb_Dn_1D_Ddr4_Rdimm_Imem_Mcm.esbin
set PSP_APPB_ZP_1D_DDR4_RDIMM_DMEM_FILENAME=%Firmwares_DIR_ZP%\Appb_Dn_1D_Ddr4_Rdimm_Dmem_Mcm.esbin
set PSP_APPB_ZP_1D_DDR4_LRDIMM_IMEM_FILENAME=%Firmwares_DIR_ZP%\Appb_Dn_1D_Ddr4_Lrdimm_Imem_Mcm.esbin
set PSP_APPB_ZP_1D_DDR4_LRDIMM_DMEM_FILENAME=%Firmwares_DIR_ZP%\Appb_Dn_1D_Ddr4_Lrdimm_Dmem_Mcm.esbin
set PSP_APPB_ZP_2D_DDR4_IMEM_FILENAME=%Firmwares_DIR_ZP%\Appb_Dn_2D_Ddr4_Imem_Mcm.esbin
set PSP_APPB_ZP_2D_DDR4_DMEM_FILENAME=%Firmwares_DIR_ZP%\Appb_Dn_2D_Ddr4_Dmem_Mcm.esbin
set PSP_APOB_FILENAME=%Firmwares_DIR_ZP%\APOB_NV_DN.bin
rem set UCODE_PATCH_ZP_B2_FILENAME=%Firmwares_DIR_ZP%\UcodePatch_cpuidHygonGenuine.bin
set CORE_MCE_EMPTY_DATA_FILENAME=%Firmwares_DIR_ZP%\CoreMceEmptyData.bin
set PSP_BOOT_LOADER_FILENAME=%Firmwares_DIR_ZP%\PspBootLoader_prod_DnSp3Sp4.esbin
set PSP_SECP_FILENAME=%Firmwares_DIR_ZP%\RsmuSecurityPolicy_DN.esbin
set UCODE_PATCH_HYGON_Ax_FILENAME=%Firmwares_DIR_ZP%\UcodePatch_DN_Ax.esbin
set UCODE_PATCH_HYGON_B0_FILENAME=%Firmwares_DIR_ZP%\UcodePatch_DN_B0.esbin
set UCODE_PATCH_HYGON_B1_FILENAME=%Firmwares_DIR_ZP%\UcodePatch_DN_B1.esbin

	echo ^<?xml version=^"1.0^" ?^>> %OUTPUT_FILE%
	echo ^<DIRS AddressMode=^"0^"^>>>%OUTPUT_FILE%
	echo   ^<PSP_DIR Level=^"1^" Base=^"%PSP_DATA_OFFSET%^" Size=^"%PSP_DATA_L1_SIZE%^" SpiBlockSize=^"%SPI_BLOCK_SIZE%^"^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x0^" File=^"%PUB_KEY_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<POINT_ENTRY Type=^"0x1^" Address=^"%PSP_BOOT_LOADER_OFFSET%^" Size=^"0x10000^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x8^" File=^"%SMU_FIRMWARE_FILENAME%^"/^>>>%OUTPUT_FILE%
	rem echo     ^<IMAGE_ENTRY Type=^"0x9^" File=^"%ZEPPELIN_SECURE_DEBUG_FILENAME%^"/^>>>%OUTPUT_FILE%
	rem echo     ^<IMAGE_ENTRY Type=^"0xA^" File=^"%PSP_ABL_SIGNED_KEY_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<VALUE_ENTRY Type=^"0xB^" Value=^"%PSP_SOFT_FUSE_CHAIN%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x12^" File=^"%SMU_FIRMWARE2_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x13^" File=^"%DEBUG_UNLOCK_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x21^" File=^"%PSP_IKEK_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x22^" File=^"%SECURE_EMPTY_TOKEN_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x24^" File=^"%PSP_SECP_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x30^" File=^"%PSP_AGESA_BOOT_LOADER_0_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x31^" File=^"%PSP_AGESA_BOOT_LOADER_1_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x32^" File=^"%PSP_AGESA_BOOT_LOADER_2_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x33^" File=^"%PSP_AGESA_BOOT_LOADER_3_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x34^" File=^"%PSP_AGESA_BOOT_LOADER_4_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x35^" File=^"%PSP_AGESA_BOOT_LOADER_5_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x36^" File=^"%PSP_AGESA_BOOT_LOADER_6_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x19000^" File=^"%UCODE_PATCH_HYGON_Ax_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x19001^" File=^"%UCODE_PATCH_HYGON_Ax_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x19002^" File=^"%UCODE_PATCH_HYGON_Ax_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x19010^" File=^"%UCODE_PATCH_HYGON_B0_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x19011^" File=^"%UCODE_PATCH_HYGON_B1_FILENAME%^"/^>>>%OUTPUT_FILE%
  
	echo     ^<POINT_ENTRY Type=^"0x40^" Address=^"%PSPDIR_L2_OFFSET%^" Size=^"0x400^"/^>>>%OUTPUT_FILE%
	echo   ^</PSP_DIR^>>>%OUTPUT_FILE%
	echo   ^<BIOS_DIR Level=^"1^" Base=^"%BIOSDIR_DATA_L1_OFFSET%^" Size=^"%BIOSDIR_DATA_L1_SIZE%^" SpiBlockSize=^"%FLASH_BLOCK_SIZE%^"^>>>%OUTPUT_FILE%
	rem echo     ^<IMAGE_ENTRY Type=^"0x5^" File=^"%BIOS_PSP_PUB_KEY_SIGNED_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x60^" Instance=^"0x00^" File=^"%PSP_APCB_FILENAME%^" Size=^"%PSP_APCB_SIZE%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x68^" Instance=^"0x00^" File=^"%PSP_APCB_FILENAME%^" Size=^"%PSP_APCB_SIZE%^"/^>>>%OUTPUT_FILE%
	echo     ^<POINT_ENTRY Type=^"0x61^" Address=^"0x0^" Size=^"0x0^" Destination=^"%HIGH_MEMORY_APOB_BASE%^"/^>>>%OUTPUT_FILE%
	echo     ^<POINT_ENTRY Type=^"0x62^" Address=^"%FV_BB_OFFSET%^" Destination=^"%HIGH_MEMORY_REGION_BB_BASE%^" Size=^"%FV_BB_SIZE%^"^>>>%OUTPUT_FILE%
	echo      ^<TypeAttrib Compressed=^"0x0^" Copy=^"0x1^" ReadOnly=^"0x0^" RegionType=^"0x0^" ResetImage=^"0x1^"/^>>>%OUTPUT_FILE%
	echo     ^</POINT_ENTRY^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x64^" Instance=^"0x01^" File=^"%PSP_APPB_ZP_1D_DDR4_UDIMM_IMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x65^" Instance=^"0x01^" File=^"%PSP_APPB_ZP_1D_DDR4_UDIMM_DMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x64^" Instance=^"0x02^" File=^"%PSP_APPB_ZP_1D_DDR4_RDIMM_IMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x65^" Instance=^"0x02^" File=^"%PSP_APPB_ZP_1D_DDR4_RDIMM_DMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x64^" Instance=^"0x03^" File=^"%PSP_APPB_ZP_1D_DDR4_LRDIMM_IMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x65^" Instance=^"0x03^" File=^"%PSP_APPB_ZP_1D_DDR4_LRDIMM_DMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x64^" Instance=^"0x04^" File=^"%PSP_APPB_ZP_2D_DDR4_IMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x65^" Instance=^"0x04^" File=^"%PSP_APPB_ZP_2D_DDR4_DMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<POINT_ENTRY Type=^"0x70^" Address=^"%BIOSDIR_DATA_L2_OFFSET%^" Size=^"0x400^"/^>>>%OUTPUT_FILE%
	echo   ^</BIOS_DIR^>>>%OUTPUT_FILE%
	echo   ^<PSP_DIR Level=^"2^" Base=^"%PSPDIR_L2_OFFSET%^" Size=^"%PSP_DATA_L2_SIZE%^" SpiBlockSize=^"%FLASH_BLOCK_SIZE%^"^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x1^" File=^"%PSP_BOOT_LOADER_FILENAME%^" Size=^"0x10000^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x8^" File=^"%SMU_FIRMWARE_FILENAME%^"/^>>>%OUTPUT_FILE%
	rem echo     ^<IMAGE_ENTRY Type=^"0x9^" File=^"%ZEPPELIN_SECURE_DEBUG_FILENAME%^"/^>>>%OUTPUT_FILE%
	rem echo     ^<IMAGE_ENTRY Type=^"0xA^" File=^"%PSP_ABL_SIGNED_KEY_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<VALUE_ENTRY Type=^"0xB^" Value=^"%PSP_SOFT_FUSE_CHAIN%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x13^" File=^"%DEBUG_UNLOCK_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x21^" File=^"%PSP_IKEK_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x22^" File=^"%SECURE_EMPTY_TOKEN_FILENAME%^"/^>>>%OUTPUT_FILE%
        echo     ^<IMAGE_ENTRY Type=^"0x24^" File=^"%PSP_SECP_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x30^" File=^"%PSP_AGESA_BOOT_LOADER_0_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x31^" File=^"%PSP_AGESA_BOOT_LOADER_1_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x32^" File=^"%PSP_AGESA_BOOT_LOADER_2_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x33^" File=^"%PSP_AGESA_BOOT_LOADER_3_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x34^" File=^"%PSP_AGESA_BOOT_LOADER_4_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x35^" File=^"%PSP_AGESA_BOOT_LOADER_5_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x36^" File=^"%PSP_AGESA_BOOT_LOADER_6_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo   ^</PSP_DIR^>>>%OUTPUT_FILE%
	echo   ^<BIOS_DIR Level=^"2^" Base=^"%BIOSDIR_DATA_L2_OFFSET%^" Size=^"%BIOSDIR_DATA_L2_SIZE%^" SpiBlockSize=^"%SPI_BLOCK_SIZE%^"^>>>%OUTPUT_FILE%
	rem echo     ^<IMAGE_ENTRY Type=^"0x5^" File=^"%BIOS_PSP_PUB_KEY_SIGNED_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x60^" Instance=^"0x00^" File=^"%PSP_APCB_FILENAME%^" Size=^"%PSP_APCB_SIZE%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x68^" Instance=^"0x00^" File=^"%PSP_APCB_FILENAME%^" Size=^"%PSP_APCB_SIZE%^"/^>>>%OUTPUT_FILE%
	echo     ^<POINT_ENTRY Type=^"0x61^" Address=^"0x0^" Size=^"0x0^" Destination=^"%HIGH_MEMORY_APOB_BASE%^"/^>>>%OUTPUT_FILE%
	echo     ^<POINT_ENTRY Type=^"0x62^" Address=^"%FV_BB_OFFSET%^" Destination=^"%HIGH_MEMORY_REGION_BB_BASE%^" Size=^"%FV_BB_SIZE%^"^>>>%OUTPUT_FILE%
	echo      ^<TypeAttrib Compressed=^"0x0^" Copy=^"0x1^" ReadOnly=^"0x0^" RegionType=^"0x0^" ResetImage=^"0x1^"/^>>>%OUTPUT_FILE%
	echo     ^</POINT_ENTRY^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x63^" File=^"%PSP_APOB_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x64^" Instance=^"0x01^" File=^"%PSP_APPB_ZP_1D_DDR4_UDIMM_IMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x65^" Instance=^"0x01^" File=^"%PSP_APPB_ZP_1D_DDR4_UDIMM_DMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x64^" Instance=^"0x02^" File=^"%PSP_APPB_ZP_1D_DDR4_RDIMM_IMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x65^" Instance=^"0x02^" File=^"%PSP_APPB_ZP_1D_DDR4_RDIMM_DMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x64^" Instance=^"0x03^" File=^"%PSP_APPB_ZP_1D_DDR4_LRDIMM_IMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x65^" Instance=^"0x03^" File=^"%PSP_APPB_ZP_1D_DDR4_LRDIMM_DMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x64^" Instance=^"0x04^" File=^"%PSP_APPB_ZP_2D_DDR4_IMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x65^" Instance=^"0x04^" File=^"%PSP_APPB_ZP_2D_DDR4_DMEM_FILENAME%^"/^>>>%OUTPUT_FILE%
	REM	echo     ^<IMAGE_ENTRY Type=^"0x66^" Instance=^"0x00^" File=^"%UCODE_PATCH_ZP_B2_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo     ^<IMAGE_ENTRY Type=^"0x67^" Instance=^"0x00^" File=^"%CORE_MCE_EMPTY_DATA_FILENAME%^"/^>>>%OUTPUT_FILE%
	echo   ^</BIOS_DIR^>>>%OUTPUT_FILE%
	echo ^</DIRS^>>> %OUTPUT_FILE%
