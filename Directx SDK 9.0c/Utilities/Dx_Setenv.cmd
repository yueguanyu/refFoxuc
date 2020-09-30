if "%cpu%" == "AMD64" Goto X64_ENV
if "%cpu%" == "i386" Goto  I386_ENV
Goto Exit


:X64_ENV
Set Lib=%DXSDK_DIR%Lib\X64;%Lib%
Set Include=%DXSDK_DIR%Include;%Include%
Set Path=%DXSDK_DIR%Utilities\X64;%Path%
echo Dx9 X64 Enviroment is Now enabled.
Goto Exit
:I386_ENV
Set Lib=%DXSDK_DIR%Lib;%Lib%
Set Include=%DXSDK_DIR%Include;%Include%
Set Path=%DXSDK_DIR%Utilities;%Path%
echo Dx9 x86 Enviroment is Now enabled.
Goto Exit


:Exit