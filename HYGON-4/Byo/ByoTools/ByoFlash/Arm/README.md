# How To Run  
## Prerequisites  
* A 64-bit ARM platform PC
* Administrator permission

## Step
* Add execute attribute  
  `chmod +x ByoFlash `
* Print help  
  `sudo ./ByoFlash -h `
* Print Version  
  `sudo ./ByoFlash -v `
* Flash BIOS image with BIOS info defined block 
  `sudo ./ByoFlash -bfu Image.bin`
* Flash all BIOS image  
  `sudo ./ByoFlash -bfu Image.bin -all`
* Flash all BIOS image but not check biosid  
  `sudo ./ByoFlash -bfu Image.bin -all -no-id-check`
* Dump BIOS image to Backup.bin  
  `sudo ./ByoFlash -bu Backup.bin `  
* Dump BIOS image to Backup.bin and then update BIOS image  
  `sudo ./ByoFlash -bu Backup.bin -bfu Image.bin -all`  
* Dump BIOS image to Backup.bin and then update BIOS image, reboot after update successful  
  `sudo ./ByoFlash -bu Backup.bin -bfu Image.bin -all -reboot`  
* Reserved NVM region
  `sudo ./ByoFlash -resvnvm Bios.bin`  
* Reserved SMBIOS region
  `sudo ./ByoFlash -resvsmbios Bios.bin`  
* Update logo  
  `sudo ./ByoFlash -logo logo.jpg`  
* Lock all flash region  
  `sudo ./ByoFlash -lock`  
* UnLock all flash region
  `sudo ./ByoFlash -unlock`  
* Query flash lock status  
  `sudo ./ByoFlash -locksts`  
* Reset password  
  `sudo ./ByoFlash -rstpwd`  
 
## Notes
-  Must execute tool with administrator permission
-  Please don't press ctrl+alt+delete or change any media device
-  when Tool not work, must make sure the platform support it
-  This tool doesn't check the input file format, it just updates the input file into flash. The user needs to make sure the input file is correct.
-  when want to input special character like space,",!,and so on.  
-  You can use '' to contain them.To input ',can try use \'.

## ReleaseNote  
- V1.27  
  Fix check public key issue  
  
- V1.26  
  check lock status first then check password

- V1.25  
  Support check password when PcdSystemPasswordHashIteration set as ture  
  Password hash type add sha512 and sha384 support  

- V1.24  
  Add logo format check  
  Change reboot to cold reboot  
  Change fvmain signature to ffff when update failed  

- V1.23  
  Fix update error on arm platforms,Change to use gcc4.9  
  resvsmbios and resvnvm to update all except the chosen area  
  Align print messages and change version print  
  Different tools not share same lock status  

- V1.22  
  Support more than one logo files to be updated if the platform supports.  
  Align Print messages  

- V1.21  
  Will print bios version when check biosid failed  

- V1.20  
  Password add support for sm3 hash type  

- V1.19  
  Add -no-id-check to skip check bios id  
  Align and change help messages  

- V1.18  
  Update CopyRights  
  Will not reboot when lock  
  
- V1.17  
  bu will not print extra error messages  
  fix -rstpwd message error  

- V1.16  
  Add input check to avoid update bios option together  
  Add "-pwd password" after -rstpwd  

- V1.15  
  Upgrade IsFactoryMode() function  
  Simplify help info  
  change report messages to follow byoflash  

- V1.14  
  system will wait 5 seconds when reboot  
  Report same messages when input error  

- V1.13  
  check return status of updateflash  

- V1.12  
  Fix print backup image issue  

- V1.11  
  Support new platform  

- V1.10  
  Fix memory segmentation fault issue  
  pdate check BIOS ID logic  

- V1.9  
  Fix regression issue  
  Correct print help message  

- V1.8  
  Modify for verify sign data process  

- V1.7   
  Update help message  

- V1.6  
  解决更新bios输入密码错误时，段错误问题  
  change struct SYSTEM_PASSWORD to follow changes in bios  
  fix check password error  
  Fix -resvsmbios not work issue  
  Update argument for more options, support -all -resvnvm -resvsmbios  
  Add reboot option for reboot system when updated BIOS  

- V1.5  
  Support first backup and then update BIOS image  
  Update full image not save smbios region  
  Take lock ahead for flash update  

- V1.4  
  Add x64 lib and remove unused aarch64 lib  
  Update for check BIOS related tool version  
  Support HashType from Password  
  修正 BIOS更新，在系统重启后，系统事件日志中没有相应信息  
  
- V1.3  
  Disable input interrupt when update flash  
  Fix Logo update issue  
  Fix update Logo not print success status issue  
  Fix Reset Password issue  
- V1.2  
  * Add loc/unlock and check password  
  * Move openssl  
  * Fix Logo update issue  
  * Fix parameter illegal issue  
- V1.1  
  * Add -logo option for logo update  
  * Add -fvmain option for fvmain update  
  * Add -fvsec option for fvsec update  
- V1.0  
  * First version  