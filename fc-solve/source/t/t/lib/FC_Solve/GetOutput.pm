package FC_Solve::GetOutput;

use strict;
use warnings 'FATAL';

use Carp qw/confess/;
use String::ShellQuote qw/shell_quote/;

sub board_gen_prefix
{
    my ($self, $args) = @_;

    my $deal = $args->{deal};

    return
    ($args->{board} ? "" :
        (
            ($args->{msdeals} ?
                "pi-make-microsoft-freecell-board -t $deal | " :
                ("make_pysol_freecell_board.py -t" .
                    ($args->{pysolfc_deals} ? " -F " : "") . " $deal $args->{variant} | ")
            )
        )
    );
}

sub fc_solve_params_suffix
{
    my ($self, $args) = @_;

    my $board = $args->{board};

    return "$args->{variant_s} " . shell_quote(@{$args->{theme}})
        . " -p -t -sam " .  ($board ? shell_quote($board) : "");
}

sub compile_args
{
    my ($self, $args) = @_;

    my $board = $args->{board};
    my $deal = $args->{deal};

    $args->{theme} ||= ["-l", "gi"];

    if (! defined($board))
    {
        if (!defined($deal))
        {
            confess "Neither Deal nor board are specified";
        }
        if ($deal !~ m{\A[1-9][0-9]*\z})
        {
            confess "Invalid deal $deal";
        }
    }


    my $variant = ($args->{variant}  ||= "freecell");
    $args->{variant_s} =
    (
        ($args->{is_custom} = ($variant eq "custom"))
        ? "" : "-g $variant"
    );

    return;
}

sub _calc_cmd_line
{
    my $self = shift;

    my $args = shift;

    $self->compile_args($args);

    my $fc_solve_exe = shell_quote($ENV{'FCS_PATH'} . "/fc-solve");

    my $cmd_line =
    (
        $self->board_gen_prefix($args) .
        "$fc_solve_exe " . $self->fc_solve_params_suffix($args) . " |"
    );

    return
    +{
        %$args,
        cmd_line => $cmd_line,
    };
}

sub open_cmd_line
{
    my ($self, $args) = @_;

    my $cmd_line_args = $self->_calc_cmd_line($args);

    open my $fc_solve_output, "$cmd_line_args->{cmd_line}"
        or Carp::confess "Error! Could not open the fc-solve pipeline";

    $cmd_line_args->{fh} = $fc_solve_output;

    return $cmd_line_args;
}
1;

=head1 COPYRIGHT & LICENSE

Copyright 2015 by Shlomi Fish

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
