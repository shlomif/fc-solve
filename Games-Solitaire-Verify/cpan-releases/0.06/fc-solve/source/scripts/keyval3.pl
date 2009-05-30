#!/usr/bin/perl

use strict;
use warnings;

sub process
{
    my ($ident, $field) = @_;

    if ($field eq "s")
    {
        return $ident . "_key";
    }
    else
    {
        return $ident . "_val->" . $field;
    }
}

while (<>)
{
    chomp;
    # An fcs_state_t function parameter
    if (m{\A\s+fcs_state_t\s*\*\s*(\w+)_key\s*,\s*\z})
    {
        my $ident = $1;
        print "$_\n";
        PARAM_LOOP:
        while(<>)
        {
            chomp;
            if (m{\A\{})
            {
                print "$_\n";
                FUNC_LOOP:
                while (<>)
                {
                    chomp;
                    if (m{\A\}})
                    {
                        print "$_\n";
                        last FUNC_LOOP;
                    }
                    else
                    {
                        s[$ident->(\w+)][process($ident, $1)]ge;
                        print "$_\n";
                    }
                }
                last PARAM_LOOP;
            }
            else
            {
                print "$_\n";
            }
        }
    }
    else
    {
        print "$_\n";
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

