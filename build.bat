@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Optimization switches /O2 /Od
set CompilerFlags = -MTd -nologo -fp:fast -EHa- -Od -Oi -W4 -FC -Z7 -wd4201 -wd4458 
set LinkerFlags = -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib opengl32.lib gdiplus.lib Shlwapi.lib

IF NOT EXIST .\Build mkdir .\Build
del /S /Q .\Build
mkdir .\Build

pushd .\Build
cl -std:c++17 %CompilerFlags% ..\Example_Universe.cpp -Fmtesting.map -Fetesting.exe /link %LinkerFlags%
popd

echo ----------------------
echo ** Complile Finished ** 