package FC_Solve::SplitCmdLine;

use strict;
use warnings;

use FC_Solve::Paths qw/ $IS_WIN bin_exe_raw /;
use IPC::Run qw/ run /;
use Socket qw(:crlf);

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

    my $output = '';
    my $pid = run [$SPLIT_EXE], \$input_string, \$output;

    $output = _normalize_lf($output);

    if ($IS_WIN)
    {
        require Test::More;
        Test::More::diag("Flutter output = [[[[\n$output\n]]]]");
    }

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
