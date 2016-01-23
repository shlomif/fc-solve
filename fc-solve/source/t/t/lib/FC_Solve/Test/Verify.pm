package FC_Solve::Test::Verify;

use strict;
use warnings;

sub _run_test
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ($args, $msg) = @_;

    require FC_Solve::GetOutput;
    my $cmd_line = FC_Solve::GetOutput->new($args);
    my $fc_solve_output = $cmd_line->open_cmd_line->{fh};

    require Games::Solitaire::Verify::Solution;

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $fc_solve_output,
            variant => $cmd_line->variant,
            ($cmd_line->is_custom ? (variant_params => $args->{variant_params}) : ()),
        },
    );

    my $verdict = $solution->verify();
    my $test_verdict = Test::More::ok (!$verdict, $msg);

    if (!$test_verdict)
    {
        require Data::Dumper;
        Test::More::diag("Verdict == " . Data::Dumper::Dumper($verdict));
    }

    close($fc_solve_output);

    return $test_verdict;
}

sub run_id
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ($s, $args) = @_;

    return _run_test(@{ $args->{data} }{qw(args msg)});
}

1;

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2009 Shlomi Fish

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

