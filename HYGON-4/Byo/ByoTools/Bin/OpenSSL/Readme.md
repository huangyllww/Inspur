# Build the OpenSSL binaries from source code

> Build Version: 1.1.1j
> Build Date   : Apr. 7, 2021 (WW15.3)

## Build Environment
|   | Software           | Tier         | Version           | Required |
|---|--------------------|--------------|-------------------|----------|
| 1 | Windows 10         | Enterprise   | 1909 (18363.1443) |     v    |
| 2 | Visual Studio 2019 | Professional | 16.9.2            |     v    |
| 3 | Strawberry Perl    |              | 5.32.1.1 (64bit)  |     v    |
| 4 | NASM               |              | 2.14.02           |     v    |

## Pre-requirement
1. Install all needed software:
  - Visual Studio 2019
  - Strawberry Perl
  - NASM
2. Set the Strawberry Perl environment in %PATH%
3. Set the NASM environment variable in %PATH%

## Build Step
1. Download the specific version OpenSSL from [official website](https://www.openssl.org/source/).
2. Extract the compression file.
3. Launch the "x86 Native Tools Command Prompt for VS 2019" as administrator.
4. Goto the location that you extracted.
5. Configure for the target OS with 32-bit. Use the command ```perl Configure VC-WIN32```.
6. Clean up the output folder ```nmake clean``` to prevent uncertain error.
7. Build with the command ```nmake```.
8. After finishing build, there are ```libcrypto-1_1x64.dll``` and ```libssl-1_1-x64.dll```in the root of your folder.
9. Run the test cases to test the make target by ```nmake test```.
10. Install the application ```nmake install``` then you could find the needed files in C:\Program Files (x86)\OpenSSL\bin.
  - openssl.exe
  - libcrypto-1_1.dll
  - libssl-1_1.dll

## References
- [NASM](https://www.nasm.us/)
- [OpenSSL Wiki](https://wiki.openssl.org/index.php/Compilation_and_Installation)
- [Strawberry Perl](https://strawberryperl.com/)
