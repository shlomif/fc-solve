#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 6;

use lib './t/lib';

use Games::Solitaire::FC_Solve::ShaAndLen;

{
    my $string = "HelloWorld";

    my $sha = Games::Solitaire::FC_Solve::ShaAndLen->new();

    $sha->add($string);

    # TEST
    is ($sha->len(), 10, "Length is 10");

    # TEST
    is ($sha->hexdigest(),
        "872e4e50ce9990d8b041330c47c9ddd11bec6b503ae9386a99da8584e9bb12c4",
        "Hex digest is OK."
    );
}

{
    my $sha = Games::Solitaire::FC_Solve::ShaAndLen->new();

    $sha->add("Hello");
    $sha->add("World");

    # TEST
    is ($sha->len(), 10, "Length is 10");

    # TEST
    is ($sha->hexdigest(),
        "872e4e50ce9990d8b041330c47c9ddd11bec6b503ae9386a99da8584e9bb12c4",
        "Hex digest is OK."
    );
}

{
    my $sha = Games::Solitaire::FC_Solve::ShaAndLen->new();

    open my $in, "<", "./t/data/sample-solutions/fcs-freecell-24.txt";
    $sha->add_file($in);
    close($in);

    # TEST
    is ($sha->len(), 32280, "Length for add_file() is 32280");

    # TEST
    is ($sha->hexdigest(),
        "17abf343aac1dec42b924b968588f2fa4e07632aab6932f5d084a6c754d61956",
        "Hex digest is OK for add_file()"
    );
}

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2008 Shlomi Fish

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

