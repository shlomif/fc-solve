package MyOldNews;

use strict;
use warnings;

my @old_news = 
(
          {
            'html' => '
<p>
Freecell Solver version 0.4 was released. This version contains three major
code optimizations, so it now runs much faster. It can now also start
solving from a non-initial board.
</p>

',
            'mon' => 6,
            'day_of_month' => 6,
            'year' => 2000
          },
          {
            'html' => '
<p>
Freecell Solver version 0.6 was released. This version features a more 
modular code as well as the following usability features:
</p>

<ol>
<li>An extra movement that allows Freecell Solver to solve 
some boards which it could not solve previously.
</li>
<li>The order of the stacks and freecells is preserved 
throughout the solutions.
</li>
<li>Limit to a certain number of iterations (so it will stop before it consumes 
too much memory)</li>
<li>Specify the order of the movements that will be tested. Usually, a test 
order can be found that will solve a given board really fast.</li>
</ol>

',
            'mon' => 7,
            'day_of_month' => 28,
            'year' => 2000
          },
          {
            'html' => '
<p>
Freecell Solver version 0.8.0 was released. New features in this version
include:
</p>

<ol>
<li>Some bug-fixes. (users of previous versions should upgrade)</li>

<li>Support for a variable number of freecells, stacks, and cards per stack.
</li>

<li>Board generators for Microsoft Freecell, Freecell Pro and PySol.
</li>

<li>An option to use the balanced binary tree implementations of 
<a href="http://www.msu.edu/user/pfaffben/avl/">libavl</a>
,
<a href="http://www.gtk.org/">glib</a>
, 
or 
<a href="http://libredblack.sourceforge.net/">libredblack</a>
. Using them makes Freecell Solver about 33% faster.
</li>

<li>Support for using "T" instead of "10" in board input/output.
</li>

<li>Improved Documentation.
</li>

</ol>

',
            'mon' => 8,
            'day_of_month' => 28,
            'year' => 2000
          },
          {
            'html' => '
<p>
Freecell Solver version 0.10.0 was released. This version features support 
for several Solitaire variants besides Freecell, such as Forecell, Seahaven
Towers and Eight Off. Aside from that, it offers a more verbose solution, as
well as several bug-fixes.
</p>


',
            'mon' => 10,
            'day_of_month' => 9,
            'year' => 2000
          },
          {
            'html' => '
<p>Freecell Solver version 1.0.0 was released. This version can solve more
game types, and can be compiled so it will be less memory intensive which is
especially good for such games as Die Schlange or Der Katzenschwantz.</p>

<p>Now there is also a library interface for use with games, which supports
suspending a solution process and resuming it from its last position. And
naturally, there is a bug fix or two, so you should upgrade. :-)</p>

<p>But the most important news is that it is no longer version 0.x.y but
rather version 1.x.y. Hoorah for the new initial digit!</p>



',
            'mon' => 11,
            'day_of_month' => 19,
            'year' => 2000
          },
          {
            'html' => '
<p>
Version 1.2.0 of Freecell Solver is now available for download. This version
has several improved moves so it can solve more boards. It also sports a
more robust command-line argument handling so you can expect much less 
segfaults if you use it improperly.
</p>

<p>
The code has also changed quite a bit, so it may be of interest to
developers who are interested to look under its hood.
</p>

',
            'mon' => 12,
            'day_of_month' => 21,
            'year' => 2000
          },
          {
            'html' => '
<p>
I have setup a <a href="http://fc-solve.berlios.de/">mirror</a> for Freecell
Solver at <a href="http://developer.berlios.de/">BerliOS</a>. It will
contain much of the same contents as this one, but will not be updated as
frequently.
</p>

<p>
The motivation for setting up the mirror was because I noticed that the
connectivity at the Technion was not failsafe, and I want a backup site.
</p>

',
            'mon' => 1,
            'day_of_month' => 26,
            'year' => 2001
          },
          {
            'html' => '
<p>
The long-awaited Freecell Solver 1.4.0 is now available. Changes from the 
previous version include some fixes for bugs and memory leaks (mostly
library-related), as well as an improved Soft-DFS scan, plus A* and BFS
scans. (which are experimental and do not always work properly).
</p>

<p>
A choice between all of those scans is available at run-time.
</p>


',
            'mon' => 2,
            'day_of_month' => 7,
            'year' => 2001
          },
          {
            'html' => '
<p>
I have set up a mailing-list for Freecell Solver. For the mailing-list 
homepage and for subscription information consult the following page:
</p>

<p>
<a href="http://groups.yahoo.com/group/fc-solve-discuss/">
http://groups.yahoo.com/group/fc-solve-discuss/
</a>
</p>


<p>
If possible, send your questions or commentary there. If not
you can still E-mail me (Shlomi Fish).
</p>


',
            'mon' => 2,
            'day_of_month' => 12,
            'year' => 2001
          },
          {
            'html' => '
<p>
Freecell Solver version 1.6.0 has been released. It supports a solution path
optimization routine which enables it to generate solutions that contain a 
smaller number of moves. Furthermore, there are some optimizations, so it 
should be at least a little faster.
</p>

<p>
Otherwise, a lot of comments were added to the code, to make it easier 
for interested developers to understand it.
</p>

',
            'mon' => 4,
            'day_of_month' => 11,
            'year' => 2001
          },
          {
            'html' => '
<p>
Freecell Solver version 1.8.0 was released into the world. Its main
highlight is its new building process based on GNU Autotools. Now, one
can build a shared library, a static library and an executable across
many UNIX variants out of the box.
</p>

<p>
I also adapted the GNOME AisleRiot board generation program to generate
the boards of the new Solitaire variants that were added to it and are 
supported by Freecell Solver.
</p>

',
            'mon' => 8,
            'day_of_month' => 31,
            'year' => 2001
          },
          {
            'html' => '
<p>
Freecell Solver version 1.10.0 was released. This version can now solve
deals of Simple Simon, as well as solving all the Freecell-like variants
that it could before.
</p>

',
            'mon' => 10,
            'day_of_month' => 2,
            'year' => 2001
          },
          {
            'html' => '
<p>
Freecell Solver 2.0.0 was released. This version features a lot of new 
enhancements and additions:
</p>

<ol>
<li>Added some presets for Beleaguered Castle, Citadel and Streets and
Alleys.</li>
<li>Re-factoring of the scans code to make it simpler.</li>
<li>Added many functions to the programmer\'s library.</li>
<li>fc-solve now uses it, so it is fully independant of the library it is
linked against.</li>
<li>Added a randomized DFS scan (with a user-defined seed).</li>
<li>Win32 Makefile can now generate a working DLL</li>
<li>Most importantly : a new initial digit! All hail the "2"!!</li>
</ol>

',
            'mon' => 12,
            'day_of_month' => 19,
            'year' => 2001
          },
          {
            'html' => '
<p>
Freecell Solver 2.2.0 came into the world today. This release features
several improvements and fixes to the library\'s internals - it now resides
solely in the "freecell_solver_" namespace, has a more robust state output, 
and has a simplified and more modular scans\' code. Aside from that, the 
maintainance of the package became more integrated and now it is
possible to build an RPM by typing executing rpm -ta on the archive.
</p>

',
            'mon' => 2,
            'day_of_month' => 18,
            'year' => 2002
          },
          {
            'html' => '
<p>
Freecell Solver 2.4.0 was released. The highlight of this release is the 
ability for several scans to operate on the same state collection. This is
done using a mechanism called "soft threads", which is switched in user-land 
and does not require system multi-threading.
</p>


',
            'mon' => 3,
            'day_of_month' => 29,
            'year' => 2002
          },
          {
            'html' => '
<p>
Freecell Solver 2.6.0 has been spotted in the wild. This version features 
atomic moves, which are slower than the traditional ones, but guarantee 
a true verdict. It also has a <tt>--prelude</tt> switch that allows running
a static order of quotas at start for the participating soft threads, and
makes constructing faster solving presets easier.
</p>

<p>
Several other flags were added, and the code has also been greatly optimized 
and cleaned up, which makes Freecell Solver much faster than it was before.
</p>

',
            'mon' => 7,
            'day_of_month' => 12,
            'year' => 2002
          },
          {
            'html' => '
<p>
Freecell Solver 2.8.0 has been released. This version features a better
documentation and help screens, a preset system, and an option to read 
paramters from files. Plus, it is possible to run one instance of the 
solver after the other in case the other one has returned a negative 
verdict.
</p>
<p>
All in all: a lot of added usability!
</p>

',
            'mon' => 9,
            'day_of_month' => 28,
            'year' => 2002
          },
          {
            'html' => '
<p>
There is now <a href="./pysol/">PySol Integration with Freecell Solver</a>
available here. This enables receiving hints from Freecell Solver and seeing
demos of it while within PySol.
</p>

',
            'mon' => 10,
            'day_of_month' => 4,
            'year' => 2003
          },
          {
            'html' => '
<p>
The <a href="download.html">Downloads</a> are now working again. Plus,
several new pages were added or updated in the site:
</p>

<ol>
<li>
<a href="$(ROOT)/current-status.html">The Current Status Page</a> answers
the question "Is Freecell Solver dead?".
</li>
<li>
Several new items were added to <a href="$(ROOT)/to-do.html">the "To
Do List" page</a>. It now also features a table of contents.
</li>
<li>
There is now <a href="$(ROOT)/getting-credit.html">a page 
about Giving or Getting Academic Credit for working on Freecell
Solver</a>.
</li>
</ol>

',
            'mon' => 12,
            'day_of_month' => 15,
            'year' => 2005
          },
          {
            'html' => '
<p>
<a href="http://en.wikipedia.org/wiki/Perl_6">Perl 6</a> will be ready by
Christmas, but of an unknown year. On the other hand, Freecell Solver 2.12.0,
is already ready for Christmas 2008, and was released today. The highlights
are a new configuration and build system based on <a 
href="http://www.cmake.org/">CMake</a>, which results in faster configurations 
and builds, and a much reduced archive size; a new suite of automated tests;
an optimisation to the command line processing and a new <tt>--version</tt>
flag.
</p>

<p>
Happy holidays and hope you enjoy this release.
</p>

',
            'mon' => 12,
            'day_of_month' => 10,
            'year' => 2008
          },
          {
            'html' => '
<p>
Freecell Solver 2.14.0 was released today. This release features a major
speed boost, and an important bug fix: when "--sequence-move unlimited"
had been specified in previous versions, it sometimes did not take effect
due to other parameters, and as a result the sequence move was still limited.
(Thanks to larrysan for reporting this problem).
</p>

<p>
This release comes relatively shortly after the previous release, and without
any developer versions. This is in an effort to make more frequent stable
releases with notable features.
</p>

',
            'mon' => 1,
            'day_of_month' => 25,
            'year' => 2009
          },
          {
            'html' => '
<p>
Freecell Solver 2.16.0 is now available. Culminating many person-years
of effort (OK - some person-days of effort), it is the best release todate.
</p>

<p>
The highlights of this release are a bug fix to the iteration numbers in
the "-s -i" display, and an internal refactoring of the
ptr_state_with_locations_t struct into a separate ptr_state_t (the key) and
a ptr_state_extra_info_t (the value). The latter should not affect the external
behaviour or API in any way.
</p>

<p>
This release (like previous ones) is available from 
<a href="download.html">the download page</a>. Next on our agenda is getting
a binary package for Microsoft Windows out-of-the-door, and more code cleanups,
optimisations and enhancements.
</p>

',
            'mon' => 3,
            'day_of_month' => 15,
            'year' => 2009
          },
);

sub get_old_news
{
    return \@old_news;
}

1;
