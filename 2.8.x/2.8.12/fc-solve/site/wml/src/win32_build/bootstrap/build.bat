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

SET MSVC_PATH=C:\Program Files\Microsoft Visual Studio\VC98\Bin

call "%MSVC_PATH%\vcvars32"


IF EXIST add-cr.exe GOTO AFTER_ADD_CR
echo #include "stdio.h" > add_cr.c
echo int main(int argc, char * argv[]) >> add_cr.c
echo { >> add_cr.c
echo FILE * i, * o; char buf[1]; >> add_cr.c
echo i = fopen(argv[1], "rb"); >> add_cr.c
echo o = fopen(argv[2], "wb"); >> add_cr.c
echo while(fread(buf, 1,1,i)) { if (buf[0] == '\n') { fputs("\r\n",o); } else {fputc(buf[0],o);}} >> add_cr.c
echo fclose(i); fclose(o); >> add_cr.c
echo } >> add_cr.c
cl /Feadd-cr.exe add_cr.c

:AFTER_ADD_CR


REM Retrieve the archive
curl -o fcs.zip %SERVER_W32_BUILD_URL%dynamic/fcs.zip

mkdir src
cd src
..\unzip ..\fcs.zip

REM Set the MSVC building parameters


REM Build Everything
copy config.h.win32 config.h
copy prefix.h.win32 prefix.h
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
mkdir %ARC_DIR%\"To-Program-Files"
mkdir %ARC_DIR%\"To-Program-Files"\"Freecell Solver"
mkdir %ARC_DIR%\"To-Program-Files"\"Freecell Solver"\share
SET DATA_DIR=%ARC_DIR%\To-Program-Files\Freecell Solver\share\freecell-solver
mkdir %ARC_DIR%\To-Program-Files\"Freecell Solver"\share\freecell-solver
.\add-cr src\Presets\presetrc.win32 p
copy p %ARC_DIR%\To-Program-Files\"Freecell Solver"\share\freecell-solver\presetrc
mkdir %ARC_DIR%\To-Program-Files\"Freecell Solver"\share\freecell-solver\presets
copy src\Presets\presets\*.sh %ARC_DIR%\To-Program-Files\"Freecell Solver"\share\freecell-solver\presets

zip -r freecell-solver-%VERSION%.win32.bin.zip %ARC_DIR%
