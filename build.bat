@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Optimization switches /O2 /Od
set CommonCompilerFlags=-MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4458 -wd4706 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib opengl32.lib gdiplus.lib

IF NOT EXIST .\build mkdir .\build
del /S /Q .\build
mkdir .\build

pushd .\build
cl %CommonCompilerFlags% ..\Example_GFX2D.cpp -Fmtesting.map -Fetesting.exe /link %CommonLinkerFlags%
popd

echo ----------------------
echo ** Complile Succeed ** 