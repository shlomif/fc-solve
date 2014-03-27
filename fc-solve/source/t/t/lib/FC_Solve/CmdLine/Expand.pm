package FC_Solve::CmdLine::Expand;

use strict;
use warnings;

use FC_Solve::SplitCmdLine;

use MooX qw( late );

sub _build_argv
{
    my $self = shift;

    return $self->_process_cmd_line($self->input_argv());
}

has input_argv => (is => 'ro', isa => 'ArrayRef[Str]', required => 1, );
has argv => (is => 'ro', isa => 'ArrayRef[Str]', lazy => 1, default => \&_build_argv);

sub _slurp
{
    my $filename = shift;

    open my $in, '<', $filename
        or die "Cannot open '$filename' for slurping - $!";

    local $/;
    my $contents = <$in>;

    close($in);

    return $contents;
}

sub _process_cmd_line
{
    my ($self, $argv_input) = @_;

    my @argv = @$argv_input;
    my @out;
    my $idx = 0;
    while ($idx < @argv)
    {
        if ($argv[$idx] eq '--read-from-file')
        {
            $idx++;
            my $s = $argv[$idx];
            my $num_skip = 0;

            if ($s =~ s/\A(\d+),//)
            {
                $num_skip = $1;
            }
            my $filename = $s;

            if ($filename !~ m#\A/#)
            {
                $filename = "$ENV{FCS_SRC_PATH}/Presets/presets/$filename";
            }

            my $text = _slurp($filename);
            my $argv_to_process = FC_Solve::SplitCmdLine->split_cmd_line_string(
                $text
            );

            push @out, @{
                $self->_process_cmd_line(
                    [ @{$argv_to_process}[$num_skip .. $#{$argv_to_process}] ]
                )
            };
        }
        elsif (($argv[$idx] eq '-l') || ($argv[$idx] eq '--load-config'))
        {
            $idx++;
            my $preset_name = $argv[$idx];
            open my $in, '<', "$ENV{FCS_PATH}/Presets/presetrc"
                or die "Cannot open presetrc file - $!";
            my $cmd_found;
            PRESETRC:
            while (my $line = <$in>)
            {
                chomp($line);
                if ($line =~ m#\Aname=\Q$preset_name\E\s*\z#)
                {
                    while (my $cmd = <$in>)
                    {
                        chomp($cmd);
                        if ($cmd =~ s/\Acommand=//)
                        {
                            $cmd_found = $cmd;
                            last PRESETRC;
                        }
                    }
                }
            }
            close($in);

            if (defined($cmd_found))
            {
                push @out,
                @{
                    $self->_process_cmd_line(
                        FC_Solve::SplitCmdLine->split_cmd_line_string(
                            $cmd_found
                        )
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

=head1 COPYRIGHT & LICENSE

Copyright 2013 by Shlomi Fish

This program is distributed under the MIT (X11) License:
L<http://www.opensource.org/licenses/mit-license.php>

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
