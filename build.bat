@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Optimization switches /O2 /Od

set CompilerFlags=-std:c++17 -Zi -MTd -nologo -fp:fast -EHa- -Od -Oi -W4 -FC  -wd4201 -wd4458 
set LinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib winmm.lib opengl32.lib gdiplus.lib Shlwapi.lib

IF NOT EXIST .\Build mkdir .\Build
del /S /Q .\Build
mkdir .\Build

pushd .\Build
cl  %CompilerFlags% ..\%1 -Fmtesting.map -Fetesting.exe /link %LinkerFlags%
popd

echo ----------------------
echo ** Complile Finished ** 