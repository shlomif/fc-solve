#!/usr/bin/perl

use strict;
use warnings;

use Template;

sub card_num_normalize
{
    my $arg = shift;

    if (ref($arg) eq "")
    {
        return +{ map { $_ => $arg } (qw(t non_t)) };
    }
    else
    {
        return $arg
    }
}

my @suits = (qw(H C D S));
my @card_nums =  ("A", (2 .. 9),
    {
        't' => "T",
        'non_t' => "10",
    },
    , "J", "Q", "K");

@card_nums = (map { card_num_normalize($_) } @card_nums);

my $template = Template->new();

sub indexify
{
    my $offset = shift;
    my $array = shift;

    return
    [
        map
        { +{ 'idx' => ($offset+$_), 'value' => $array->[$_] } }
        (0 .. $#$array)
    ];
}

my $args =
{
    'suits' => indexify(0, \@suits),
    'card_nums' => indexify(1, \@card_nums),
};

$template->process(
    "card-test-render.c.tt",
    $args,
    "card-test-render.c",
) || die $template->error();

$template->process(
    "card-test-parse.c.tt",
    $args,
    "card-test-parse.c",
) || die $template->error();

=begin Discard

open my $out, ">", "t/test.bash";
print {$out} <<"EOF";
#!/bin/bash
exec ./binary-tests/card-test-render
EOF
close($out);

=end Discard

=cut




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

