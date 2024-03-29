@echo off

rem
rem Builds and deploys Wespal for Windows
rem

setlocal
set devenv=%1
set version=%2

if .%version%==. goto badversion

set appdir=Wespal-%version%
set appexe=wespal.exe
set archive=Wespal-%version%.zip

set devenvcmd=
if .%devenv%==.vs     goto vs
if .%devenv%==.mingw  goto mingw
if .%devenv%==.static goto static
goto baddevenv

:vs
rem Visual Studio 2022 host
set qtprefix=C:\Qt\6.6.3\msvc2019_64
set devenvcmd="C:\Program Files\Microsoft Visual Studio\2022\Community"\VC\Auxiliary\Build\vcvars64.bat
goto common

:static
:mingw
rem Mingw host
set qtprefix=C:\Qt\6.6.3\mingw_64
rem Static-linked version
if .%devenv%==.static set qtprefix=..\dist\windows-qt-6.6.3-static
set PATH=C:\Qt\Tools\mingw1120_64\bin;%PATH%
goto common

:common
set cmakepath=C:\Qt\Tools\CMake_64\bin
set ninjapath=C:\Qt\Tools\Ninja
set windeployqt=%qtprefix%\bin\windeployqt.exe

set distdir=.\dist\windows
set builddir=.\build-dist-windows

rem
rem Set up development environment
rem

if not .%devenvcmd%==. call %devenvcmd%
set PATH=%ninjapath%;%cmakepath%;%qtprefix%\bin;%PATH%

rem
rem Create and change into build directory
rem

rmdir /S /Q %builddir% 2> NUL
mkdir %builddir%
cd %builddir%

rem
rem Configure and build the app
rem

cmake -S .. -B . -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja

rem
rem Use windeployqt to deploy DLLs
rem

mkdir dist-prep
move %appexe% dist-prep
cd dist-prep
"%windeployqt%" --no-system-d3d-compiler --no-translations --no-opengl-sw --no-virtualkeyboard --no-quick-import --no-network --exclude-plugins qgif,qicns,qjpeg,qpdf,qsvg,qtga,qtiff,qsvgicon %appexe%

rem
rem Move deployed files into dist directory
rem

cd ..\..
mkdir %distdir% 2> NUL
move %builddir%\dist-prep %distdir%\%appdir%

rem
rem Create distribution archive
rem

powershell -Command "Compress-Archive -Force %distdir%\%appdir% %distdir%\%archive%"

rem
rem Cleanup and report
rem

rmdir /S /Q %builddir% 2> NUL
goto end

:end
echo Saved deployment files under %distdir%\:
echo   * %appdir%\
echo   * %archive%
goto :eof

:baddevenv
echo You need to specify a development environment in the command line:
goto usage

:badversion
echo You need to specify a version number in the command line:
goto usage

:usage
echo   dist-windows.cmd vs    (version number)
echo   dist-windows.cmd mingw (version number)
