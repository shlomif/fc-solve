package FC_Solve::Paths;

use 5.014;
use strict;
use warnings;
use Socket qw(:crlf);
use String::ShellQuote qw/shell_quote/;

use parent 'Exporter';

our @EXPORT_OK =
    qw($FC_SOLVE_EXE $FC_SOLVE__RAW $FIND_DEAL_INDEX $GEN_MULTI $IS_WIN $MAKE_PYSOL bin_board bin_exe_raw bin_file data_file dll_file is_break is_freecell_only is_without_dbm is_without_flares is_without_patsolve is_without_valgrind normalize_lf samp_board samp_preset samp_sol);

use File::Spec ();

my $DATA_DIR    = File::Spec->catdir( $ENV{FCS_SRC_PATH}, qw(t data) );
my $BOARDS_DIR  = File::Spec->catdir( $DATA_DIR,          'sample-boards' );
my $SOLS_DIR    = File::Spec->catdir( $DATA_DIR,          'sample-solutions' );
my $PRESETS_DIR = File::Spec->catdir( $DATA_DIR,          'presets' );
our $IS_WIN = ( $^O eq "MSWin32" );

sub _correct_path
{
    my $p = shift;
    if ($IS_WIN)
    {
        $p =~ tr#/#\\#;
    }
    return $p;
}
my $EXE_SUF            = ( $IS_WIN ? '.exe' : '' );
my $FCS_PATH           = $ENV{FCS_PATH};
my $FC_SOLVE__RAW__RAW = "$FCS_PATH/fc-solve";
our $FC_SOLVE__RAW = _correct_path($FC_SOLVE__RAW__RAW) . $EXE_SUF;
our $FC_SOLVE_EXE  = _correct_path( shell_quote($FC_SOLVE__RAW__RAW) );
my $PY3 = ( $IS_WIN ? 'python3 ' : '' );

sub _board_gen
{
    return "${PY3}../board_gen/" . shift;

}
our $MAKE_PYSOL      = _board_gen('make_pysol_freecell_board.py');
our $FIND_DEAL_INDEX = _board_gen('find-freecell-deal-index.py');
our $GEN_MULTI       = _board_gen('gen-multiple-pysol-layouts.py');

sub _is_tag
{
    my ($tag) = @_;

    return ( ( $ENV{FCS_TEST_TAGS} // '' ) =~ /\b\Q$tag\E\b/ );
}
my $BREAK_TAG   = _is_tag('break_backcompat');
my $FC_ONLY     = _is_tag('fc_only');
my $NO_FLARES   = _is_tag('no_flares');
my $NO_PATSOLVE = _is_tag('no_pats');
my $NO_VALGRIND = _is_tag('no_valg');
my $NO_DBM      = _is_tag('no_dbm');

# A file in the output/binaries directory where fc-solve was compiled.
sub bin_file
{
    return File::Spec->catfile( $FCS_PATH, @{ shift @_ } );
}

sub dll_file
{
    my $fn = shift;
    return bin_file( [ "lib$fn." . ( $IS_WIN ? "dll" : "so" ) ] );
}

sub bin_exe_raw
{
    return _correct_path( bin_file(@_) ) . $EXE_SUF;
}

# A board file in the binary directory.
sub bin_board
{
    return File::Spec->catfile( $FCS_PATH, shift );
}

sub data_file
{
    return File::Spec->catfile( $DATA_DIR, @{ shift @_ } );
}

sub is_break
{
    return $BREAK_TAG;
}

sub is_freecell_only
{
    return $FC_ONLY;
}

sub is_without_dbm
{
    return $NO_DBM;
}

sub is_without_flares
{
    return $NO_FLARES;
}

sub is_without_patsolve
{
    return $NO_PATSOLVE;
}

sub is_without_valgrind
{
    return $NO_VALGRIND;
}

# Returns a board from the sample-boards directory.
sub samp_board
{
    return File::Spec->catfile( $BOARDS_DIR, shift );
}

sub samp_sol
{
    return File::Spec->catfile( $SOLS_DIR, shift );
}

sub samp_preset
{
    return File::Spec->catfile( $PRESETS_DIR, shift );
}

sub normalize_lf
{
    my ($s) = @_;
    $s =~ s#$CRLF#$LF#g;
    return $s;
}

1;
