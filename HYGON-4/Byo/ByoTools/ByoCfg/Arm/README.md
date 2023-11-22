# How To Run  
## Prerequisites  
* A 64-bit Linux PC
* Administrator permission

## Step
* Add execute attribute  
  `chmod +x ByoCfg `  
* Print help  
  `sudo ./ByoCfg -h `  
* Print Version  
  `sudo ./ByoCfg -v `  
* Read setup Variable value  
  `sudo ./ByoCfg -r VariableName`  
* Set setup Variable value  
  `sudo ./ByoCfg -w VariableName:Value `  
* Read multiple Variables value  
  `sudo ./ByoCfg -r VariableName1 VariableName2 VariableName3`  
* Set multiple Variables value  
  `sudo ./ByoCfg -w VariableName1:Value1  VariableName2:Value2  VariableName3:Value3`  
* Dump all supported variable on current platform  
  `sudo ./ByoCfg -dump`  
* Read variables value in config file and get current value to write output file  
  `sudo ./ByoCfg -rf config.txt -o out_cfg.txt`  
* Write variables value in config file
  `sudo ./ByoCfg -wf config.txt`  
* Set all setup variable to default  
  `sudo ./ByoCfg -setdefault`  
* Lock all flash region  
  `sudo ./ByoCfg -lock`  
* Unlock all flash region  
  `sudo ./ByoCfg -unlock`  

## config file format  
* CurrentValue must one of the Options value in parenthesis  
VariableName = CurrentValue; (Option1 Option2 Option3 ...)  
VariableName2 = CurrentValue2; (Option1 Option2 Option3 ...)  


 
## Notes
-  Must execute tool with administrator permission
-  when Tool not work, must make sure the platform support it
-  when want to input special character like space,",!,and so on.  
-  You can use '' to contain them.To input ',can try use \'.

## ReleaseNotes  
- V1.25  
  Change byocfg value print to capital letter  
  Add check admin and user password whether same when set password if platform needs  

- V1.24  
  Check password length,complexity,whether repeat when set password  
  Set password time when set password  
  Support check password when PcdSystemPasswordHashIteration set as ture  
  Password hash type add sha512 and sha384 support  

- V1.23  
  Change message from reset password to remove password  
  Set EnteredType as admin when password reset  
  Print error message to error screen when -dump  

- V1.22  
  Add Password read and write  
  Not show suppressif if platform supported  
  Different tools not share same lock status  
  Change to use gcc4.9  

- V1.21  
  Fix -wf write uefi and legacy boot group order error  
  Will report error when input file is empty  

- V1.20  
  Skip invalid one of option string

- V1.19  
  Fix the issue that the option value is always got as zeros  
  Fix -wf write only one boot option error  
  Add GroupOrder write check  

- V1.18  
  Check bios version  
  Solve lack legacy Bootorder error  
  Password add support for sm3 hash type  
  Add printf messages when write the option value  

- V1.17  
  Update CopyRights  
  Report some messages when -rf and -wf fail or success  
  Delete bootorder which not contain mBmAutoCreateBootOptionGuid  
  Add support for setup f9 the same as setdefault  

- V1.16  
  ByoCfg add orderedlist support  

- V1.15  
  ByoCfg report error when -w uefigrouporder and legacygroupOrder input not optional  

- V1.14  
  Byocfg fix -w LegacyBootGroupOrder error  
  ByoCfg not show bootorder whose attributes contain LOAD_OPTION_HIDDEN  

- V1.13  
  Upgrade IsFactoryMode() function  
  Change -w and -wf print messages when WriteUefiGroupOrder and WriteLegacyGroupOrder  
  Change %x -> %X  
  Change report messages when input error to follow byoflash

- V1.12  
  Byocfg check input variable value;It should be dec or hex start with 0x and optional  
  -r and -w now print full messages like -dump  
  byocfg fix -rf and -wf line length too short error  

- V1.11  
  Support dump one of option strings  
  Boot000a -> Boot000A  
  change -rf logic  

- V1.10  
  Update FmcNorFlashDmaComplete function  
  fix -setdefault error

- V1.9  
  Update for support new platform  

- V1.8  
  ByoCfg -wf fix when UefiBootGroupOrder ends with ' ;'  

- V1.7  
  Correct and alignment print message  

- V1.6  
  linux Arm/ByoCfg update crc32 when modify variable value  
  Linux arm byocfg return 1 when section not exist  

- V1.5  
  Update linux Arm/ByoCfg to modify bootorder  
  Update for dump setup item name string  

- V1.4  
  change struct SYSTEM_PASSWORD to follow changes in bios  
  Add Modify boot order in shell version byocfg and Fix Hygon#585 Byocfg -rf -wf error  
  Update Byocfg to set admin_password and poweron_password  
  Update Byocfg group read/write  

- V1.3  
  Fix when set password can't update variable issue  
  Fix -rf and -wf option issue  
- V1.2  
  Update for check BIOS related tool version  
  Support HashType from Password  
  Add -setdefault option  
  Fix lock error and returncode error  
  Add ByoCfg checkpassword and lock/unlock  

- V1.1  
  First release  
