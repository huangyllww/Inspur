# How To Run
## Prerequisites
### Arm
* A 64-bit ARM platform PC
* Administrator permission
### Linux
* A 64-bit Linux PC
* Administrator permission
### Windows
* A 64-bit Windows PC
* Administrator permission

## PreStep
### Linux or Arm
*  ` sudo su ` or add ` sudo ` before each command
*  ` chmod +x ByoDp `
*  Use command ` ./ByoDp ` instead of ` ByoDp ` when run
### Windows
* Open cmd with administrator

## Step
* Print help  
  ` ByoDp -? `
* Displays additional information  
  ` ByoDp -v `
* Prevents display of individual measurements for cumulative items  
  ` ByoDp -x `
* Displays summary information only  
  ` ByoDp -s ` or ` ByoDp.exe -S `
* Displays all measurements in a list  
  ` ByoDp -A `
* Displays all measurements in raw format  
  ` ByoDp -R `
* Sets display threshold to VALUE microseconds, default is 1,000(us)  
  ` ByoDp -t VALUE `
* Limits display to COUNT lines in All and Raw modes, default is 50  
  ` ByoDp -n COUNT `
* Displays progress identifier  
  ` ByoDp -i `
* Display pre-defined and custom cumulative data  
  ` ByoDp -c TOKEN `  
  Pre-defined cumulative token are:  
    1. LoadImage
    2. StartImage
    3. DB:Start
    4. DB:Support
    5. DB:Stop

## Notes
-  Must execute tool with administrator permission
-  when Tool not work, must make sure the platform support it 
-  when want to input special character like space,",!,and so on.  
-  You can use "" to contain them.To input ",can try use \".Shell mode can try use ^"
-  Mouse click not supported

## ReleaseNotes
- V1.2  
  Fix Linux GetMemData  
- V1.1  
  Fix Major Phases Total Time in Legacy OS  
  Fix general measurements number  
  Return error and exit when get too many arguments  
  Fix windows GetMemData  
  Prevent print PEIM in ProcessHandles  
- V1.0  
  *First version*
