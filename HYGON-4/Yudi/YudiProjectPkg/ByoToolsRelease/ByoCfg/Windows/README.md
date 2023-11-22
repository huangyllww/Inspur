# How To Run  
## Prerequisites  
* A 64-bit Windows PC
* Administrator permission

## Step
* Open cmd with administrator
* Print help  
  `ByoCfg.exe -h `  
* Print Version  
  `ByoCfg.exe -v `  
* Read setup Variable value  
  `ByoCfg.exe -r VariableName`  
* Set setup Variable value  
  `ByoCfg.exe -w VariableName:Value `  
* Read multiple Variables value  
  `ByoCfg.exe -r VariableName1 VariableName2 VariableName3`  
* Set multiple Variables value  
  `ByoCfg.exe -w VariableName1:Value1  VariableName2:Value2  VariableName3:Value3`  
* Dump all supported variable on current platform  
  `ByoCfg.exe -dump`  
* Read variables value in config file and get current value to write output file  
  `ByoCfg.exe -rf config.txt -o out_cfg.txt`  
* Write variables value in config file
  `ByoCfg.exe -wf config.txt`  
* Set all setup variable to default  
  `ByoCfg.exe -setdefault`  

## config file format  
* CurrentValue must one of the Options value in parenthesis  
VariableName = CurrentValue; (Option1 Option2 Option3 ...)  
VariableName2 = CurrentValue2; (Option1 Option2 Option3 ...)  


 
## Notes
-  Must execute tool with administrator permission
-  when Tool not work, must make sure the platform support it
-  when want to input special character like space,",!,and so on.  
-  You can use "" to contain them.To input ",can try use \".Shell mode can try use ^"

## ReleaseNotes  
- V1.23  
  Add Password read and write  
  Not show suppressif if platform supported  
  Different tools not share same lock status  

- V1.22  
  Fix -wf write uefi and legacy boot group order error  
  Will report error when input file is empty  

- V1.21  
  Skip invalid one of option string  

- V1.20  
  Fix -dump and -rf error because of malloc not enough  
  Fix -wf write only one boot option error  
  Add GroupOrder write check  
  Hide OS created Windows Boot Manager boot option  

- V1.19  
  Check bios version  
  Solve lack legacy Bootorder error
  Fix setdefault -rf -wf error because of malloc space not enough  
  Add printf messages when write the option value  

- V1.18  
  Update CopyRights  
  Report some messages when -rf and -wf fail or success  
  Delete bootorder which not contain mBmAutoCreateBootOptionGuid  
  Add support for setup f9 the same as setdefault  

- V1.17  
  ByoCfg add orderedlist support  

- V1.16  
  ByoCfg report error when -w uefigrouporder and legacygrouprder input not optional  
  Remove BootXXXX from -dump result

- V1.15  
  Byocfg fix -w LegacyBootGroupOrder error  
  ByoCfg not show bootorder whose attributes contain LOAD_OPTION_HIDDEN  
  Byocfg now can use on powershell

- V1.14  
  Change -w and -wf print messages when WriteUefiGroupOrder and WriteLegacyGroupOrder  
  Change %x -> %X  
  Change report messages when input error to follow byoflash  

- V1.13  
  Byocfg gets smbios not smbios3 address  
  Byocfg check input variable value;It should be dec or hex start with 0x and optional  
  -r and -w now print full messages like -dump  
  byocfg fix -rf and -wf line length too short error  

- V1.12  
  Support dump one of option strings  
  change -rf logic and fix its error  

- V1.11  
  fix -setdefault error  

- V1.10  
  fix -setdefault error

- V1.9  
  Linux and windows byocfg Add bootorder read and write  

- V1.8  
  Correct and alignment print message  
 
- V1.7  
  Update Copyright information  

- V1.6  
  Update for dump setup item name string  

- V1.5  
  Update version  

- V1.4  
  Update for check BIOS related tool version  
  Update Windows vcxproj for release build  

- V1.3  
  Add Win 10 X64 driver to resource  
  Add Password check  
- V1.2  
  *Support setdefault parameter  
- V1.1  
  *Check administrator password before modification*  
  *Fix regression issue by 8a9c4c66*  
  *Fix the line of input config file is too long issue*  
