@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Optimization switches /O2
set CommonCompilerFlags=-MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -W4 -wd4201 -wd4100 -wd4189 -wd4505 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib


IF NOT EXIST .\build mkdir .\build

pushd .\build
cl %CommonCompilerFlags% ..\testing.cpp -Fmwin32_handmade.map /link %CommonLinkerFlags%
popd

echo ----------------------
echo ** Complile Succeed ** 