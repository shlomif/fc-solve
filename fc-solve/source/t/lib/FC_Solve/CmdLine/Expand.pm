package FC_Solve::CmdLine::Expand;

use strict;
use warnings;

use FC_Solve::SplitCmdLine ();
use Path::Tiny qw/ path /;

use MooX qw/ late /;

sub _build_argv
{
    my $self = shift;

    return $self->_process_cmd_line( $self->input_argv() );
}

has input_argv => ( is => 'ro', isa => 'ArrayRef[Str]', required => 1, );
has argv =>
    ( is => 'ro', isa => 'ArrayRef[Str]', lazy => 1, default => \&_build_argv );

sub _process_cmd_line
{
    my ( $self, $argv_input ) = @_;

    my @argv = @$argv_input;
    my @out;
    my $idx = 0;
    while ( $idx < @argv )
    {
        if ( $argv[$idx] eq '--read-from-file' )
        {
            $idx++;
            my $s        = $argv[$idx];
            my $num_skip = 0;

            if ( $s =~ s/\A(\d+),// )
            {
                $num_skip = $1;
            }
            my $filename = $s;

            if ( $filename !~ m#\A/# )
            {
                $filename = "$ENV{FCS_SRC_PATH}/Presets/presets/$filename";
            }

            my $text = path($filename)->slurp_utf8;
            my $argv_to_process =
                FC_Solve::SplitCmdLine->split_cmd_line_string($text);

            push @out,
                @{
                $self->_process_cmd_line(
                    [
                        @{$argv_to_process}[ $num_skip .. $#{$argv_to_process} ]
                    ]
                )
                };
        }
        elsif ( ( $argv[$idx] eq '-l' ) || ( $argv[$idx] eq '--load-config' ) )
        {
            $idx++;
            my $preset_name = $argv[$idx];
            open my $in, '<', "$ENV{FCS_PATH}/Presets/presetrc"
                or die "Cannot open presetrc file - $!";
            my $cmd_found;
        PRESETRC:
            while ( my $line = <$in> )
            {
                chomp($line);
                if ( $line =~ m#\Aname=\Q$preset_name\E\s*\z# )
                {
                    while ( my $cmd = <$in> )
                    {
                        chomp($cmd);
                        if ( $cmd =~ s/\Acommand=// )
                        {
                            $cmd_found = $cmd;
                            last PRESETRC;
                        }
                    }
                }
            }
            close($in);

            if ( defined($cmd_found) )
            {
                push @out,
                    @{
                    $self->_process_cmd_line(
                        FC_Solve::SplitCmdLine->split_cmd_line_string(
                            $cmd_found)
                    )
                    };
            }
            else
            {
                die "Cannot find preset $preset_name";
            }
        }
        else
        {
            push @out, $argv[$idx];
        }
    }
    continue
    {
        $idx++;
    }

    return \@out;
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
