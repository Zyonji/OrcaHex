@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

set WarningFlags=/W4 /wd4100 /wd4201 /wd4505 /wd4706 /WX
set OptionFlags=-DCOMPILE_WIN32=1
set LinkerFlags=/link /LIBPATH:%~dp0 /INCREMENTAL:NO /OPT:REF user32.lib Gdi32.lib opengl32.lib FreeImage.lib Comdlg32.lib

set BuildFlags=/FC /fp:fast /GL /GR- /Gw /nologo /MT /Oi
set DebugFlags=/Od /Zi
set OptimizedFlags=/O2

set CompilerFlags=%BuildFlags% %DebugFlags% %WarningFlags% %OptionFlags%
REM set CompilerFlags=%BuildFlags% %OptimizedFlags% %WarningFlags% %OptionFlags%

IF NOT EXIST %~dp0\..\build mkdir %~dp0\..\build
pushd %~dp0\..\build
cl %CompilerFlags% %~dp0\win32_orcahex.cpp %LinkerFlags%
popd