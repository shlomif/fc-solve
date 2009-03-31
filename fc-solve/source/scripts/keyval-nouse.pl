#!/usr/bin/perl

use strict;
use warnings;

use IO::All;

foreach my $file (glob("*.c"))
{
    my @lines = io($file)->getlines();

    my @new;

    while (defined(my $l = shift(@lines)))
    {
        # A function definition
        if ($l =~ m{\A\w})
        {
            my %ptr_states = ();
            my $first_iter = 1;
            while ($first_iter || ($l = shift(@lines) && ($l !~ m{\A\{})))
            {
                $first_iter = 0;
                if ($l =~ m{fcs_state_with_locations_t *\* *(\w+),})
                {
                    $ptr_state{$1} = 1;
                }
                else
                {

            }
        }
        else
        {
            push @new, $l;
        }
    }
}



=head1 COPYRIGHT AND LICENSE

Copyright (c) 2000 Shlomi Fish

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.



=cut

