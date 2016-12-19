package FC_Solve::SplitCmdLine;

use strict;
use warnings;
use autodie;

use Path::Tiny qw/ path /;
use FC_Solve::Paths qw/ bin_exe_raw /;
use Socket qw(:crlf);
use File::Temp qw/ tempdir /;

sub _normalize_lf
{
    my ($s) = @_;
    $s =~ s#$CRLF#$LF#g;
    return $s;
}

my $SPLIT_EXE = bin_exe_raw( [qw#t out-split-cmd-line.exe#] );

sub split_cmd_line_string
{
    my ( $class, $input_string ) = @_;

    my $dir = tempdir( CLEANUP => 1 );
    my $input_fn = "$dir/in.txt";
    open my $i, '>', $input_fn;
    print {$i} $input_string;
    close($i);

    my $output_fn = "$dir/out.txt";
    system( $SPLIT_EXE, '-i', $input_fn, '-o', $output_fn );

    my $output = path($output_fn)->slurp_utf8;
    $output = _normalize_lf($output);

    open my $child_out, '<', \$output;
    my @have;
    while ( my $line = <$child_out> )
    {
        chomp($line);
        if ( $line !~ m{\A<<(.*)\z} )
        {
            die "Invalid output from program.";
        }
        my $terminator = $1;

        my $item             = "";
        my $found_terminator = 0;
        while ( $line = <$child_out> )
        {
            if ( $line eq "$terminator\n" )
            {
                $found_terminator = 1;
                last;
            }
            $item .= $line;
        }
        if ( !$found_terminator )
        {
            die "Could not find terminator '$terminator' in output.";
        }
        chomp($item);
        push @have, $item;
    }

    close($child_out);

    return ( \@have );
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2013 Shlomi Fish

=cut
