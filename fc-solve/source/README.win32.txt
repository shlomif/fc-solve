This package contains the Windows 95/98/NT/2000/ME binaries of 
Freecell Solver 2.6.3. Read the file "README" for usage and other
information.

The vanilla Freecell Solver program (included here) is a text-mode
program. If you want a nice graphical interface for it, then 
download Freecell Pro:

http://www.rrhistorical.com/rrdata/Fcpro65/

This version of Freecell Solver supports up to 8 freecells, 12 
stacks and 11 initial cards per stack. If you'd like to increase 
those settings, you'll need to recompile it. It was compiled with a
recursion stack size of 32 megabytes. (don't worry if you don't have
so much memory in your computer - it will still run fine)

Freecell Solver's homepage is at the following URL:

http://vipe.technion.ac.il/~shlomif/freecell-solver/


Enjoy!

	Shlomi Fish

Notes to Windows Users:
-----------------------

The fc-solve.exe executable is a command-line program. If you
double-click its icon it will just wait for keyboard input. To use
it, you need to feed it with a board file or an input of such. Read
the file README for instructions on how to construct the board file.

However, many times the programs in the board_gen directory can do the
job for you. For instance, the following command line feeds fc-solve
with initial board No. 24 of Microsoft Freecell:

> .\board_gen\make-microsoft-freecell-board 24 | .\fc-solve

fc-solve accepts many options, and you may wish to consult the file
USAGE to learn more about it.

I am aware of several other implementations of Freecell for Windows 
using different Freecell solvers. Links to them can be
found in the links section of the Freecell Solver homepage.

