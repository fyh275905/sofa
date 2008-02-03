@echo off
REM This batch permit to create all dsp project or vcproj project for
REM Visual C++ 2003 or 2005
REM use : project [VC7 / VC8 / clean]
REM default visual project depends on the environment variable QMAKESPEC
set QMAKEPATH=%CD%\tools\qt4win
set QTDIR=%CD%\tools\qt4win
set PATH=%QMAKEPATH%\bin;%PATH%
if "%1" == "VC7" goto vc7
if "%1" == "VC8" goto vc8
if "%1" == "clean" goto clean
:console
@echo Making Makefiles
qmake -recursive
@goto end
:vc7
set QMAKESPEC=win32-msvc.net
goto common
:vc8
set QMAKESPEC=win32-msvc2005
:common
@echo on
@if "%QMAKESPEC%"=="win32-msvc.net" @echo Making Visual project 7
@if "%QMAKESPEC%"=="win32-msvc2005" @echo Making Visual project 8
@REM cd src
@REM for /R %%i in (*.pro) do qmake "%%i"
@REM cd ..
qmake -tp vc -recursive -o Sofa QT_INSTALL_PREFIX="%QTDIR%"
@if "%ERRORLEVEL%"=="0" goto end
echo ERROR %ERRORLEVEL%
pause
@goto end
:clean
@echo cleaning all VC Projects
for /R %%i in (*.ncb, *.suo, Makefile, *.idb, *.pdb, *.plg, *.opt) do del "%%i"
cd src
for /R %%i in (*.dsp, *.vcproj, *.vcproj.*) do del "%%i"
cd ..
:end
