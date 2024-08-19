package FreecellSolver::Site::TTRender;

use strict;
use warnings;
use utf8;

use Encode      qw/ decode_utf8 /;
use URI::Escape qw( uri_escape );
use Moo;
use Path::Tiny                         qw/ path /;
use HTML::Widgets::NavMenu             ();
use HTML::Widgets::NavMenu::EscapeHtml qw( escape_html );
use MyNavData                          ();
use Template                           ();

has stdout => ( is => 'ro', required => 1 );

my $LATEMP_SERVER = "fc-solve";

my $base_path;

my $LONGBLANK = ( "<br/>" x 72 );

sub slurp
{
    return path(shift)->slurp_utf8;
}

sub retrieved_slurp
{
    return slurp( "lib/retrieved-html-parts/" . shift );
}

sub path_slurp
{
    return slurp( "lib/" . shift );
}

sub _htmlish
{
    my ($base) = @_;
    return sub {
        return path( 'lib/' . $base . '.htmlish' )->slurp_utf8();
    };
}

has vars => (
    is      => 'ro',
    default => sub {
        my $self = shift;
        return +{
            host            => $LATEMP_SERVER,
            msfreecell_note => _htmlish('msfreecell-note'),
            mytan           =>
qq#\\tan{\\left[\\arcsin{\\left(\\frac{1}{2 \\sin{36°}}\\right)}\\right]}#,
            d2url           => "http://divisiontwo.shlomifish.org/",
            old_news        => _htmlish('old-news'),
            print_nav_block => sub {
                my $args = shift;
                return _render_nav_block( $args->{name} );
            },
            longblank       => $LONGBLANK,
            main_email      => 'shlomif@shlomifish.org',
            toc_div         => \&Shlomif::Homepage::TocDiv::toc_div,
            retrieved_slurp => \&retrieved_slurp,
            path_slurp      => \&path_slurp,
            solver_bar_css  => sub {
                my $args = shift;

                my $arr = $args;
                return join( " , ", map { "#solver_bar button.$_" } @$arr );
            },
        };
    }
);

my %INDEX = ( map { $_ => 1 } 'index.html', 'index.xhtml' );

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

my @DESTs = (
    { production => 0, path => [ '.', "dest", ], },
    { production => 1, path => [ '.', "dest-prod", ], },
);

sub proc
{
    my ( $self, $input_tt2_page_path ) = @_;
    $::latemp_filename = $input_tt2_page_path;
    my @fn     = split m#/#, $input_tt2_page_path;
    my @fn_nav = @fn;
    my $tail   = \$fn_nav[-1];
    $$tail = '' if ( exists $INDEX{$$tail} );
    $base_path =
        ( '../' x ( scalar(@fn) - 1 ) );
    my $fn2 = ( join( '/', @fn_nav ) || '' );

    my $vars = $self->vars;
    $vars->{base_path} = $base_path;
    $vars->{fn_path}   = $input_tt2_page_path;
    $vars->{filename}  = $input_tt2_page_path;
    $vars->{raw_fn_path} =
        $input_tt2_page_path =~ s#(?:\A|/)\Kindex\.x?html\z##r;
    my $nav_bar = HTML::Widgets::NavMenu->new(
        'path_info'    => ( join( '/', @fn_nav ) || '/' ),
        'current_host' => $LATEMP_SERVER,
        MyNavData::get_params(),
        'ul_classes'     => [],
        'no_leading_dot' => 1,
    );
    my $rendered_results = $nav_bar->render();
    my $nav_links_obj    = $rendered_results->{nav_links_obj};
    my $nav_links        = $rendered_results->{nav_links};
    my $leading_path     = $rendered_results->{leading_path};

    $vars->{nav_bar} = $nav_bar;
    my $nav_html = $rendered_results->{html};
    $vars->{nav_menu_html} = join( '', @$nav_html );
    $vars->{share_link}    = escape_html(
        uri_escape(
            MyNavData::get_hosts()->{ $nav_bar->current_host() }->{'base_url'}
                . $nav_bar->path_info()
        )
    );
    $vars->{index_filename} = $fn2;

    my $load_javascript_srcs;

    $load_javascript_srcs = sub {
        my ($srcs) = @_;
        return join(
            "",
            map {
                my $js = $_;
                qq#<script src="${base_path}js/${js}"></script>#
            } ( ( ref($srcs) eq 'ARRAY' ) ? @$srcs : ($srcs) )
        );
    };

    my $pre_requirejs_javascript_tags = sub {
        return $load_javascript_srcs->(
            [
                "jquery.querystring.js", "jquery.phoenix.js",
                "lodash.custom.min.js"
            ]
        );
    };
    $vars->{load_javascript_srcs}          = $load_javascript_srcs;
    $vars->{pre_requirejs_javascript_tags} = $pre_requirejs_javascript_tags;
    $vars->{requirejs_conf}                = sub {
        return "requirejs.config({ baseUrl: '${base_path}js', });";
    };
    $vars->{enable_jquery_ui} =
        ( $input_tt2_page_path ne 'js-fc-solve/text/gui-tests.xhtml' );

    if ( $self->stdout )
    {
        binmode STDOUT, ':encoding(utf-8)';
        my $html = '';
        $template->process( "src/$input_tt2_page_path.tt2",
            $vars, \$html, binmode => ':utf8', )
            or die $template->error();

        print $html;
    }
    else
    {
        foreach my $rec (@DESTs)
        {
            my $d = $rec->{path};
            $vars->{production} = $rec->{production};
            my $html = '';
            $template->process( "src/$input_tt2_page_path.tt2",
                $vars, \$html, binmode => ':utf8', )
                or die $template->error();

            path( @$d, @fn, )->touchpath()->spew_utf8($html);
        }
    }
}

1;
