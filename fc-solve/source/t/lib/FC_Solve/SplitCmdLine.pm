package FC_Solve::SplitCmdLine;

use strict;
use warnings;

use IPC::Open2 qw(open2);

my $split_exe = $ENV{'FCS_PATH'}."/t/out-split-cmd-line.exe";

sub split_cmd_line_string
{
    my ($class, $input_string) = @_;

    my ($child_out, $child_in);

    my $pid = open2($child_out , $child_in, $split_exe);

    print {$child_in} $input_string;
    close($child_in);

    my @have;
    while (my $line = <$child_out>)
    {
        chomp($line);
        if ($line !~ m{\A<<(.*)\z})
        {
            die "Invalid output from program.";
        }
        my $terminator = $1;

        my $item = "";
        my $found_terminator = 0;
        while ($line = <$child_out>)
        {
            if ($line eq "$terminator\n")
            {
                $found_terminator = 1;
                last;
            }
            $item .= $line;
        }
        if (!$found_terminator)
        {
            die "Could not find terminator '$terminator' in output.";
        }
        chomp($item);
        push @have, $item;
    }

    close ($child_out);

    return (\@have);
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
