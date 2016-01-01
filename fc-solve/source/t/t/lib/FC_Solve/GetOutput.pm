package FC_Solve::GetOutput;

use strict;
use warnings;

use parent 'Games::Solitaire::Verify::Base';

use Carp qw/confess/;
use String::ShellQuote qw/shell_quote/;

__PACKAGE__->mk_acc_ref([qw(
        board
        deal
        is_custom
        msdeals
        pysolfc_deals
        theme
        variant
        variant_s
        )]);

sub _init
{
    my ($self, $args) = @_;

    my $board = $self->board($args->{board});
    my $deal = $self->deal($args->{deal});

    $self->theme($args->{theme} || ["-l", "gi"]);

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


    my $variant = $self->variant($args->{variant} || "freecell");
    $self->variant_s(
        $self->is_custom(scalar ($variant eq "custom"))
        ? "" : "-g $variant"
    );

    $self->msdeals($args->{msdeals});
    $self->pysolfc_deals($args->{pysolfc_deals});

    return;
}

sub board_gen_prefix
{
    my ($self) = @_;

    my $deal = $self->deal;

    return
    ($self->board ? "" :
        (
            ($self->msdeals ?
                "pi-make-microsoft-freecell-board -t $deal | " :
                ("make_pysol_freecell_board.py -t" .
                    ($self->pysolfc_deals ? " -F " : "") . " $deal @{[$self->variant]} | ")
            )
        )
    );
}

sub fc_solve_params_suffix
{
    my ($self) = @_;

    my $board = $self->board;

    return $self->variant_s . ' ' . shell_quote(@{$self->theme})
        . " -p -t -sam " . ($board ? shell_quote($board) : "");
}

my $FC_SOLVE_EXE = shell_quote($ENV{'FCS_PATH'} . "/fc-solve");

sub calc_cmd_line
{
    my ($self) = @_;

    my $cmd_line =
    (
        $self->board_gen_prefix() .
        " $FC_SOLVE_EXE " . $self->fc_solve_params_suffix()
    );

    return
    +{
        cmd_line => $cmd_line,
    };
}

sub open_cmd_line
{
    my ($self) = @_;

    my $cmd_line_args = $self->calc_cmd_line;

    open my $fc_solve_output, "$cmd_line_args->{cmd_line} |"
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
