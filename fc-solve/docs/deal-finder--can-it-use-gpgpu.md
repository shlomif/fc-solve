# Can the MS Freecell deal finder utilise GPGPU / OpenCL / etc.?

With respect to this algorithm for dealing cards - http://rosettacode.org/wiki/Deal_cards_for_FreeCell
and http://fc-solve.shlomifish.org/faq.html#what_are_ms_deals , I have set up a reverse
lookup to find a deal index in the range based on the cards layout here:

https://github.com/shlomif/fc-solve/blob/master/fc-solve/source/board_gen/find_deal.c

(also see https://github.com/shlomif/fc-solve/blob/master/fc-solve/source/gen_ms_boards__rand.h )

Now, I can easily split the range scanning into segments and put each into different
CPU tasks for a gain in speed, but I wish to know if the same can be done using OpenCL /
etc. for an even greater gain in speed.

Can anyone enlighten me?
