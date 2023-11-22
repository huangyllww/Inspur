## How To Run  
## Prerequisites  
* A server boot to shell

## Step
* Boot to internal shell
* Enter U disk file system
  `fs0: `
* Print help  
  `ByoFlash.efi -h`
* Flash BIOS image  
  `ByoFlash.efi -bfu Image.bin`
* All Flash BIOS image  
  `ByoFlash.efi -bfu Image.bin -all`
* Flash all BIOS image but not check biosid  
  `sudo ./ByoFlash -bfu Image.bin -all -no-id-check`
* Flash all BIOS image but not check ac(Only in X64 if supported)  
  `sudo ./ByoFlash -bfu Image.bin -all -no-ac-check`
* Dump BIOS image to Backup.bin  
  `ByoFlash.efi -bu Backup.bin`  
* Dump BIOS image to Backup.bin and then update BIOS image  
  `ByoFlash.efi -bu Backup.bin -bfu Image.bin -all`  
* Dump BIOS image to Backup.bin and then update BIOS image, reboot after update successful  
  `ByoFlash.efi -bu Backup.bin -bfu Image.bin -all -reboot` 
* Reserved NVM region
  `ByoFlash.efi -resvnvm Bios.bin`  
* Reserved smbios
  `ByoFlash.efi -resvsmbios Bios.bin` 
* Update logo  
  `ByoFlash.efi -logo logo.jpg`  
* Update ucode(Only in X64 if supported)  
  `ByoFlash.efi -ucode ucode.bin(need a specific ucode fv)`  
* Update OA3(Only in X64 if supported)  
  `ByoFlash.efi -oa3 Oa3.bin(need a specific oa3 file)`  
* reboot system
  `ByoFlash.efi -bfu Image.bin -reboot` 
* Lock all tools
  `ByoFlash.efi -lock`  
* Unlock all tools
  `ByoFlash.efi -unlock`    
* Check all tools lock status
  `ByoFlash.efi -locksts`    
* Reset password
  `ByoFlash.efi -rstpwd`   

## Notes
-  when Tool not work, must make sure the platform support it
-  This tool doesn't check the input file format, it just updates the input file into flash. The user needs to make sure the input file is correct.
-  when want to input special character like space,",!,and so on.  
-  You can use "" to contain them.To input ",can try use \".Shell mode can try use ^"

## ReleaseNote  
- V1.22  
  Fix full flash update break error  
  X86 will not update the same area  

- V1.21  
  Add logo format check  
  Change fvmain signature to ffff when update failed  
  Set cursor to invisible when update  


- V1.20  
  *Align print messages and change version print*  
  *Different tools not share same lock status*  
  *Add extra four single area update (secondbios der2 der1 pdr) if platform supported*

- V1.19  
  *Support more than one logo files to be updated if the platform supports.*  
  *Align Print messages*  

- V1.18  
  *Align the ways to get/set password and lock status with OS tool*  
  *Print Bios id when capsule update and normal update check bios id failed*  

- V1.17  
  *Add -cap option if platform supported*  

- V1.16  
  *Add -no-id-check to skip check bios id*  
  *Add check ac and -no-ac-check to avoid check ac* 

- V1.15  
  *Align and change help messages*  
  *Check bios version*  

- V1.14  
  *Update CopyRights*  
  *Arm version delete except ) when update*  
  *Will not reboot when update fail*  

- V1.13  
  *Update help messages*  
  *Reduce error messages*  

- V1.12  
  *Add input check to avoid update bios option together*  
  *Add "-pwd password" after -rstpwd*  

- V1.11  
  *Simplify help info*  
  *change report messages to follow byoflash*  
  
- V1.10
  *lock input before sign verify*
  *system will wait 5 seconds when reboot*
  *Report same messages when input error*
- V1.9
 *X86 add -me -irc -ec -gbe -nis option to update it only*
 *fix wrong print messages of 0% progress bar*
- V1.8
 *fix post error*
 *fix -h not print -logo*
- V1.7
 *修复OemSmi UEFI 中-chkel 参数使用了FixedPcd 导致不同平台编译出来的ByoFlash不能共用的问题.*
 *ByoFlash fix print warning message twice error*
- V1.6
 *Fix some errors*
 *correct print messages*
 *-bfu now reserve password*
 *Fix assert error when update*
- V1.5
 *Bioscheck and defaultupdate are separated*  
 *change -oa3 to -OA3*  
 *update copyright*
- V1.4  
 *Aarch64 and x86 are combined*  
 *get gSmmPort* 
 *Add AlderLake platform support*
 *Make sure resource is free when meet with error status* 
 *change input password logic to make backspace useful*
- V1.3  
 *Fix many errors* 
 *ByoFlash/Uefi更新BIOS不支持回刷保护* 
 *Insert event log when do BIOS update and success* 
 *-bfu bios.fd -bu Backup.bin backup first then write* 
 *Add -resvsmbios to reserve smbios space when update* 
 *Add -bfu -all to update all flash* 
 *Add -reboot to reboot system after update* 
- V1.2  
 *Fix many errors*  
