@echo off

rem
rem Builds a static version of Qt for Wespal for Windows
rem
rem This will only ever support Mingw. This is by design.
rem

setlocal

set qtsrcdir=C:\Qt\6.8.0\Src
set mingwpath=C:\Qt\Tools\mingw1310_64\bin

set cmakepath=C:\Qt\Tools\CMake_64\bin
set ninjapath=C:\Qt\Tools\Ninja

set distdir=..\dist\windows-qt-6.8.0-static
set builddir=.\qt-build-windows-6.8.0-static

rem
rem Set up development environment
rem

set PATH=%ninjapath%;%cmakepath%;%mingwpath%;%PATH%

rem
rem Create and change into build directory
rem

rmdir /S /Q %builddir% 2> NUL
mkdir %builddir%
pushd %builddir%

rem
rem Configure Qt
rem

call %qtsrcdir%\configure -prefix %distdir% -release -optimize-size -static -static-runtime -submodules qtbase,qtimageformats,qt5compat -no-feature-network -no-feature-sql -no-feature-printer -no-feature-printsupport -no-feature-androiddeployqt -no-feature-qdbus -no-feature-concurrent -no-feature-testlib -no-feature-xml -no-feature-qmake -gui -widgets -no-opengl -no-gif -no-tiff -no-dbus -no-openssl -no-ssl -qt-zlib

if not %errorlevel% equ 0 goto configfail

rem
rem Build and install Qt
rem

cmake --build . --parallel
if not %errorlevel% equ 0 goto buildfail

cmake --install .
if not %errorlevel% equ 0 goto deployfail

goto cleanup

:configfail
echo *** ERROR: Configuring Qt failed
goto cleanup

:buildfail
echo *** ERROR: Building Qt failed
goto cleanup

:deployfail
echo *** ERROR: Deploying Qt failed
goto cleanup

:cleanup
popd
