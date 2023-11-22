## How To Run  
## Prerequisites  
* A server boot to shell

## Step
* Boot to internal shell
* Enter U disk file system
  `fs0: `
* Print help  
  `ByoCfg.efi -h `  
* Print Version  
  `ByoCfg.efi -v `  
* Read setup Variable value  
  `ByoCfg.efi -r VariableName`  
* Set setup Variable value  
  `ByoCfg.efi -w VariableName:Value `  
* Read multiple Variables value  
  `ByoCfg.efi -r VariableName1 VariableName2 VariableName3`  
* Set multiple Variables value  
  `ByoCfg.efi -w VariableName1:Value1  VariableName2:Value2  VariableName3:Value3`  
* Dump all supported variable on current platform  
  `ByoCfg.efi -dump`  
* Read variables value in config file and get current value to write output file  
  `ByoCfg.efi -rf config.txt -o out_cfg.txt`  
* Write variables value in config file
  `ByoCfg.efi -wf config.txt`  
* Set all Variable to default
  `ByoCfg.efi -setdefault`  
* Lock all tools
  `ByoCfg.efi -lock`  
* Unlock all tools
  `ByoCfg.efi -unlock`  

## config file format  
* CurrentValue must one of the Options value in parenthesis  
VariableName = CurrentValue; (Option1 Option2 Option3 ...)  
VariableName2 = CurrentValue2; (Option1 Option2 Option3 ...)  


## Notes
-  Must execute tool with administrator permission
-  when Tool not work, must make sure the platform support it
-  when want to input special character like space,",!,and so on.  
-  You can use "" to contain them.To input ",can try use \".Shell mode can try use ^"

## ReleaseNote  
- V1.25  
  *Fix SystemPasswordLib get PCD value issue  
  
- V1.24  
  *Fix password become unknown when ctrl c at set password*  
  *Add check admin and user password whether same when set password if platform needs*  
  *Arm byocfg add check password length,complexity,whether repeat and report*  

- V1.23  
  *Change message from reset password to remove password*  
  *Set EnteredType as admin when password reset*  
  *Check password length,complexity,whether repeat and report*  
  *Print error message to error screen when -dump*  

- V1.22  
  *Add Password read and write*  
  *Not show suppressif if platform supported*  
  *Different tools not share same lock status*  

- V1.21  
  *Fix -wf write uefi and legacy boot group order error*  
  *Will report error when input file is empty*  

- V1.20  
  *Align the ways to get/set password and lock status with OS tool*  

- V1.19  
  *Skip invalid one of option string*  

- V1.18  
  *Fix -wf write only one boot option error*  
  *Add GroupOrder write check*  

- V1.17  
  *Check bios version*  
  *Solve lack legacy Bootorder error*  
  *Add printf messages when write the option value*  

- V1.16  
  *Update CopyRights*  
  *Report some messages when -rf and -wf fail or success*  
  *Delete bootorder which not contain mBmAutoCreateBootOptionGuid*  
  *Add support for setup f9 the same as setdefault*  

- V1.15  
  *ByoCfg add orderedlist support*  
  *ByoCfg use smi ways to get setup db*

- V1.14  
  *ByoCfg report error when -w uefigrouporder and legacygroupOrder input not optional*  

- V1.13  
  *Byocfg fix -w LegacyBootGroupOrder error*  
  *ByoCfg not show bootorder whose attributes contain LOAD_OPTION_HIDDEN*  

- V1.12
  *Change -w and -wf print messages when WriteUefiGroupOrder and WriteLegacyGroupOrder *  
  *Change report messages when input error to follow byoflash*  
- V1.11
  *Byocfg check input variable value;It should be dec or hex start with 0x and optional*  
  *-r and -w now print full messages like -dump*
- V1.10
  *ByoCfg report error when input error*  
  *Support dump one of option strings*
- V1.9
  *fix -setdefault error*
- V1.8
  *fix -setdefault error*
- V1.7
 *-setdefault/-wf no longer print details*  
 *print processed failed when input error*  
 *Fix some errors*
- V1.6
 *Fix some errors*  
 *Add modify language* 
 *-setdefault reset bootorder and language*   
- V1.5 
 *-dump print variable namestring* 
 *update copyright*   
- V1.4 
 *Fix some errors*  
 *Add modify admin_password and poweron_password*
 *Add AlderLake platform support*
 *change input password logic to make backspace useful*
- V1.3 
 *Fix some errors*  
 *Add Read and write BootOrder*  
- V1.2 
 *Fix many errors*  

