This announcement was published on 1 April 2018 here:

* https://groups.yahoo.com/neo/groups/fc-solve-discuss/conversations/messages/1575
* https://www.mail-archive.com/linux-il@cs.huji.ac.il/msg66517.html

<hr />

Hi all!

We are happy to announce that we have decided to reimplement Freecell
Solver ( http://fc-solve.shlomifish.org/ ) in the Rust programming
language ( https://en.wikipedia.org/wiki/Rust_(programming_language) ).
Rust will provide the following advantages compared to the
venerable C language, in which Freecell Solver is currently written:

1. Improved performance.

2. Far superior compilation speed.

3. Better portability.

However, it also has some disadvantages, which we hope will not hinder
the conversion:

1. The Rust compiler is much more lax than GCC's C compiler.

2. Rust has much more undefined behaviours
( https://en.wikipedia.org/wiki/Undefined_behavior ) than C, and is generally
less safe.

3. The Rust licence is more restrictive than GCC's.

Nevertheless, we plan to proceed with this. We also have a future goal
of porting PySol FC ( http://pysolfc.sourceforge.net/ ) from Python to Rust
due to the superior dynamic nature of Rust and its succinctness (see:
http://www.paulgraham.com/power.html ).

Regards,

— Shlomi Fish, CTO, Freecell Solver Enterprises™
