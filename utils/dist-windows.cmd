@echo off

rem
rem Builds and deploys Wespal for Windows
rem

setlocal
set devenv=%1
set version=%2

if .%version%==. goto badversion

set qtversion=6.9.3

set appdir=Wespal-%version%
set appexe=wespal.exe
set archive=Wespal-%version%-windows-x64.zip

set devenvcmd=
if .%devenv%==.vs     goto vs
if .%devenv%==.mingw  goto mingw
if .%devenv%==.static goto static
goto baddevenv

:vs
rem Visual Studio 2022 host
set qtprefix=C:\Qt\%qtversion%\msvc2019_64
set devenvcmd="C:\Program Files\Microsoft Visual Studio\2022\Community"\VC\Auxiliary\Build\vcvars64.bat
goto common

:static
:mingw
rem Mingw host
set qtprefix=C:\Qt\%qtversion%\mingw_64
rem Static-linked version
if .%devenv%==.static set qtprefix=..\dist\windows-qt-%qtversion%-static
set PATH=C:\Qt\Tools\mingw1310_64\bin;%PATH%
goto common

:common
set cmakepath=C:\Qt\Tools\CMake_64\bin
set ninjapath=C:\Qt\Tools\Ninja
set windeployqt=%qtprefix%\bin\windeployqt.exe

set distdir=U:\wespal
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
pushd %builddir%

rem
rem Configure and build the app
rem

cmake -S .. -B . -G Ninja -DCMAKE_BUILD_TYPE=Release -DENABLE_BUILTIN_IMAGE_PLUGINS=ON -DQUAZIP_USE_QT_ZLIB=ON
ninja

rem
rem Use windeployqt to deploy DLLs
rem

mkdir %appdir%
move %appexe% %appdir%
pushd %appdir%
"%windeployqt%" --no-system-d3d-compiler --no-translations --no-opengl-sw --no-virtualkeyboard --no-quick-import --no-network --exclude-plugins qgif,qicns,qjpeg,qpdf,qsvg,qtga,qtiff,qsvgicon %appexe%

rem
rem Create distribution archive
rem

popd
powershell -Command "Compress-Archive -Force %appdir% %distdir%\%archive%"

rem
rem Cleanup and report
rem

popd
rmdir /S /Q %builddir% 2> NUL
goto end

:end
echo Saved deployment files under %distdir%\:
echo   * %archive%
goto :eof

:baddevenv
echo You need to specify a development environment in the command line:
goto usage

:badversion
echo You need to specify a version number in the command line:
goto usage

:usage
echo   dist-windows.cmd vs     (version number)
echo   dist-windows.cmd mingw  (version number)
echo   dist-windows.cmd static (version number)
