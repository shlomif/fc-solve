package FreecellSolver::Site::TTRender;

use strict;
use warnings;
use utf8;

use Moo;
use Path::Tiny qw/ path /;
use Template   ();

my $LATEMP_SERVER = "fc-solve";

my $template = Template->new(
    {
        COMPILE_DIR  => ( $ENV{TMPDIR} // "/tmp" ) . "/shlomif-hp-tt2-cache",
        COMPILE_EXT  => ".ttc",
        INCLUDE_PATH => [ ".", "./lib", ],
        PRE_PROCESS  => ["lib/blocks.tt2"],
        POST_CHOMP   => 1,
        RELATIVE     => 1,
        ENCODING     => 'utf8',
    }
);

sub proc
{
    my ( $self, $input_tt2_page_path ) = @_;
    $::latemp_filename = $input_tt2_page_path;
    my @fn = split m#/#, $input_tt2_page_path;

    my $vars = { host => $LATEMP_SERVER, };
    $vars->{load_javascript_srcs} = sub { return ''; };
    $vars->{enable_jquery_ui} =
        ( $input_tt2_page_path ne 'js-fc-solve/text/gui-tests.xhtml' );

    $vars->{production} = 1;
    my $html = '';
    $template->process( "src/$input_tt2_page_path.tt2",
        $vars, \$html, binmode => ':utf8', )
        or die $template->error();

    path( ".", "dest-prod/", @fn, )->touchpath()->spew_utf8($html);
}

1;
