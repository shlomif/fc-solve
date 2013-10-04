Freecell Solver Readme File
===========================
Shlomi Fish <shlomif@cpan.org>
:Date: 2012-10-31
:Revision: $Id$

Introduction
------------

This is Freecell Solver version 3.20.x, a program that automatically
solves most layouts of Freecell, and similar Solitaire variants as
well as those of Simple Simon.

Freecell Solver is distributed under the MIT/X11 License
( http://en.wikipedia.org/wiki/MIT_License ), a free, permissive,
public-domain like, open-source license.

Note that the Freecell Solver source and Win32 binary distributions *do not*
provide a graphical user-interface (GUI) and are primarily meant to be used
by Solitaire researchers and software developers. If you're looking for a
suitable GUI based on Freecell Solver, see our links at:

http://fc-solve.shlomifish.org/links.html#front_ends

I hope you'll enjoy using Freecell Solver, and make the best of it.

-- Shlomi Fish ( http://www.shlomifish.org/ )

Building
--------

Read the file +INSTALL.txt+ for information on how to do that.

Usage
-----

The program is called "fc-solve". You invoke it like this:

    fc-solve board_file

board_file is the filename with a valid Freecell startup board. The file is
built as follows:

It has the 8 Freecell stacks.
Each stack contain its number of cards separated by a whitespace
and terminated with a newline character( it's important that the last stack
will also be terminated with a newline !). The cards in the line are ordered
from the bottom-most card in the left to the topmost card in the right.

A card string contains the rank of the card followed by its suit.
The card number is one of: +A,1,2,3,4,5,6,7,8,9,10,J,Q,K+. Alternatively,
+T+ can be used instead of +10+. The card suit is one of:  +H,S,D,C+ (standing
for Hearts, Spades, Diamonds and Clubs respectively).

Here is an example board: (PySol/Microsoft board No. 24)

-----------------------
4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD 10H 10C 10D 8D
7H JS KH 10S KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
-----------------------

And another one: (PySol board No. 198246790)

-----------------------
KD JH 5H 7D 9H KC 9D
3H JD 5D 8H QH 7H 2D
4D 3S QC 3C 6S QS KS
10C 9S 6D 9C QD 8S 10D
10S 8C 7S 10H 2C AS
8D AC AH 4H JC 4C
6H 7C 4S 5S 5C JS
AD KH 6C 2H 3D 2S
-----------------------

Starting from Freecell Solver 3.14.x, a stack can also start with a leading
colon (":"). This is to allow input from states as output by Freecell Solver
using the -p option.

You can specify the contents of the freecells by prefixing the line with
"FC:" or with "Freecells:". For example:

-----------------------
FC: 3H QC
-----------------------

will specify that the cards 3 of hearts and queen of clubs are present in
the freecells. To specify an empty freecell use a "-" as its designator.

If there's another "FC:" line, the previous line will be overriden.

You can specify the contents of the foundations by prefixing the line with
"Founds:" or with "Foundations:" and then using a format as follows:

-----------------------
Founds: H-5 C-A S-0 D-K
-----------------------

Hence, the suit ID followed by a dash followed by the card number in the
foundation. A suit that is not present will be assumed to be 0. Again, if
there's more than one line like that, then the previous lines will be
ignored and overridden.

The program will stop processing the input as soon as it read 8 lines of
standard stacks. Therefore, it is recommended that the foundations and
freecells lines will come at the beginning of the file.

The program will process the board and try to solve it. If it succeeds it
will output the states from the initial board to its final solution to the
standard output. If it fails, it will notify it.

For information about the various command-line switches that Freecell
Solver accepts, read the +USAGE.txt+ file in this directory.

To solve Simple Simon boards append +--game simple_simon+ right after
the "fc-solve" program name.

The board generation programs
-----------------------------

Several programs which can generate the initial boards of various Freecell
implementations can be found in the "board_gen/" sub-directory. Read the
+README.txt+ file there for details on how they can be compiled and used.

In any case, they can save you the time of inputting the board yourself.

Some complete examples for layouts
----------------------------------

A layout in the middle of the MS Freecell deal No. 109 solution:

-----------------------
Foundations: H-6 C-9 D-2 S-0
Freecells:  QS  3S  2S  KD
: 8H 3D
: KS QD JC
: AS 8D TD 7D JH TS 9D
: 7S 6D
: 5S
: KH QC JD TC 9H 8S 7H 6S 5D 4S
: KC QH JS TH 9S
: 4D
-----------------------

Similar, but with an empty Freecell:

-----------------------
Foundations: H-6 C-9 D-2 S-0
Freecells:  QS  3S  -  KD
: 8H 3D 2S
: KS QD JC
: AS 8D TD 7D JH TS 9D
: 7S 6D
: 5S
: KH QC JD TC 9H 8S 7H 6S 5D 4S
: KC QH JS TH 9S
: 4D
-----------------------

Likewise, only without leading colons where unnecessary:

-----------------------
Foundations: H-6 C-9 D-2 S-0
Freecells:  QS  3S  -  KD
8H 3D 2S
KS QD JC
AS 8D TD 7D JH TS 9D
7S 6D
5S
KH QC JD TC 9H 8S 7H 6S 5D 4S
KC QH JS TH 9S
4D
-----------------------

