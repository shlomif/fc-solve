REM         Let's clean the files of the previous run
del /q fcs.zip
del /q freecell-solver-*.zip
rmdir /s /q src
rmdir /s /q freecell-solver-*


REM Assign the base URL of the site
SET SERVER_URL=http://vipe.technion.ac.il/~shlomif/freecell-solver/

REM Assign the relative place where all the files required for
REM Building the Win32 package can be found
SET SERVER_W32_BUILD_URL=%SERVER_URL%win32_build/

REM REM REM
REM 
REM     Cancelled
REM 
REM REM REM
REM Retrieve the Version Number from the Web
REM curl -o ver.txt %SERVER_W32_BUILD_URL%dynamic/ver.txt


REM           Fetch zip.exe and unzip.exe 
REM           If they were not already fetched.
IF NOT EXIST zip.exe curl -o zip.exe %SERVER_W32_BUILD_URL%static/zip.exe
IF NOT EXIST unzip.exe curl -o unzip.exe %SERVER_W32_BUILD_URL%static/unzip.exe

REM Retrieve the archive
curl -o fcs.zip %SERVER_W32_BUILD_URL%dynamic/fcs.zip

mkdir src
cd src
..\unzip ..\fcs.zip

REM Set the MSVC building parameters

SET MSVC_PATH=C:\Program Files\Microsoft Visual Studio\VC98\Bin

call "%MSVC_PATH%\vcvars32"

REM Build Everything
copy config.h.win32 config.h
nmake /f Makefile.win32
cd board_gen
nmake /f Makefile.win32
cd ..\..\

REM Put the version number inside the variable VERSION
FOR /F " tokens=1 delims=, " %%i in (.\src\ver.txt) do set VERSION=%%i

SET ARC_DIR=freecell-solver-%VERSION%-bin
mkdir %ARC_DIR%
FOR %%i in (ChangeLog fc-solve.exe freecell-solver-range-parallel-solve.exe README README.win32.txt TODO USAGE) do copy src\%%i %ARC_DIR%\
mkdir %ARC_DIR%\board_gen
FOR %%i in (COPYING make_pysol_freecell_board.py make-aisleriot-freecell-board.exe make-gnome-freecell-board.exe make-microsoft-freecell-board.exe README) do copy src\board_gen\%%i %ARC_DIR%\board_gen

zip -r freecell-solver-%VERSION%.win32.bin.zip %ARC_DIR%
