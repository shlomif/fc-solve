package FC_Solve::Test::Valgrind;

use strict;
use warnings;

use Test::More ();
use Carp ();
use File::Spec ();
use File::Temp qw( tempdir );
use FC_Solve::Paths qw( bin_board bin_exe_raw samp_board samp_preset );

use Test::RunValgrind;

sub _expand_arg
{
    my $hash_ref = shift;

    my $type = $hash_ref->{type};

    if ($type eq 'tempdir')
    {
        return tempdir(CLEANUP => 1),
    }
    elsif ($type eq 'ENV')
    {
        return $ENV{ $hash_ref->{arg} };
    }
    elsif ($type eq 'bin_board')
    {
        return bin_board($hash_ref->{arg});
    }
    elsif ($type eq 'sample_board')
    {
        return samp_board($hash_ref->{arg});
    }
    elsif ($type eq 'sample_preset')
    {
        return samp_preset($hash_ref->{arg});
    }
    elsif ($type eq 'catfile')
    {
        my $prefix = exists($hash_ref->{prefix}) ? $hash_ref->{prefix} : '';
        return $prefix . File::Spec->catfile(
            map { (ref($_) eq 'HASH') ? _expand_arg($_) : $_ }
            @{$hash_ref->{args}}
        );
    }
    else
    {
        Carp::confess("Unknown arg type '$type'!");
    }
}


# Short for run.
sub r
{
    my ($args, $msg) = @_;

    if (!
        Test::RunValgrind->new({})->run(
            {
                log_fn => $args->{log_fn},
                blurb => $msg,
                prog => bin_exe_raw([$args->{prog}]),
                argv => [map { (ref($_) eq 'HASH') ? _expand_arg($_) : $_ } @{$args->{argv}}],
            }
        )
    )
    {
        die "Valgrind failed";
    }
}

1;

__END__

sub _run_test
{
    return r(@_);
}

sub run_id
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my (undef, $args) = @_;

    return _run_test(@$args{qw(id data)});
}

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

