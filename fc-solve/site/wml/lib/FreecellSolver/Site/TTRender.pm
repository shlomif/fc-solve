package FreecellSolver::Site::TTRender;

use strict;
use warnings;
use utf8;

use Encode qw/ decode_utf8 /;
use URI::Escape qw( uri_escape );
use Moo;
use Path::Tiny qw/ path /;
use HTML::Acronyms         ();
use HTML::Latemp::AddToc   ();
use HTML::Widgets::NavMenu ();
use HTML::Widgets::NavMenu::EscapeHtml qw( escape_html );
use Module::Format::AsHTML    ();
use MyNavData                 ();
use MyNavLinks                ();
use Shlomif::Homepage::TocDiv ();
use Template                  ();
use YAML::XS                  ();

has printable => ( is => 'ro', required => 1 );
has stdout    => ( is => 'ro', required => 1 );

my $LATEMP_SERVER = "fc-solve";
my $toc           = HTML::Latemp::AddToc->new;

my $DEFAULT_TOC_DIV = Shlomif::Homepage::TocDiv::toc_div();
my $cpan            = Module::Format::AsHTML->new;

my $base_path;

my $ACRONYMS_FN = "lib/acronyms/list1.yaml";
my $latemp_acroman =
    HTML::Acronyms->new( dict => scalar( YAML::XS::LoadFile($ACRONYMS_FN) ) );
my $shlomif_cpan = $cpan->homepage( +{ who => 'shlomif' } );
my $LONGBLANK    = ( "<br/>" x 72 );

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

sub _shlomif_include_colorized_file
{
    my $args = shift;

    return decode_utf8(
        VimIface::get_syntax_highlighted_html_from_file(
            +{ 'filename' => $args->{fn}, }
        )
    );
}

has vars => (
    is      => 'ro',
    default => sub {
        my $self = shift;
        return +{
            %{
                YAML::XS::LoadFile("lib/static_constant_params.yaml")
                    ->{static_constant_params}
            },
            ( $self->printable ? ( PRINTABLE => 1 ) : () ),
            common_keywords => (
                      "Freecell, Freecell Solver, solvers, "
                    . "AI, artificial intelligence, solitaire, Simple Simon, "
                    . "Baker's Game, Seahaven Towers, Shlomi Fish, games"
            ),

            cpan  => $cpan,
            host  => $LATEMP_SERVER,
            mytan =>
qq#\\tan{\\left[\\arcsin{\\left(\\frac{1}{2 \\sin{36°}}\\right)}\\right]}#,
            d2url           => "http://divisiontwo.shlomifish.org/",
            print_nav_block => sub {
                my $args = shift;
                return _render_nav_block( $args->{name} );
            },
            longblank  => $LONGBLANK,
            main_email => 'shlomif@shlomifish.org',
            my_acronym => sub {
                my $args = shift;

                return $latemp_acroman->abbr( { key => $args->{key}, } )
                    ->{html};
            },
            shlomif_cpan    => $shlomif_cpan,
            default_toc     => $DEFAULT_TOC_DIV,
            toc_div         => \&Shlomif::Homepage::TocDiv::toc_div,
            retrieved_slurp => \&retrieved_slurp,
            path_slurp      => \&path_slurp,
            shlomif_include_colorized_file => \&_shlomif_include_colorized_file,
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

my @DESTs = ( [ '.', "dest", ], [ '.', 'dest-prod', ], );

sub _render_leading_path_component
{
    my $component  = shift;
    my $title      = $component->title;
    my $title_attr = defined($title) ? qq# title="$title"# : "";
    return
          "<a href=\""
        . escape_html( $component->direct_url )
        . "\"$title_attr>"
        . $component->label() . "</a>";
}

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
    my $fn2 = join( '/', @fn_nav ) || '/';

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
    my $rendered_results   = $nav_bar->render();
    my $nav_links_obj      = $rendered_results->{nav_links_obj};
    my $nav_links          = $rendered_results->{nav_links};
    my $leading_path       = $rendered_results->{leading_path};
    my $nav_links_renderer = MyNavLinks->new(
        'nav_links'     => $nav_links,
        'nav_links_obj' => $nav_links_obj,
        'root'          => $base_path,
    );

    my $nav_links_html = '';

LINKS:
    foreach my $key ( sort { $a cmp $b } keys(%$nav_links_obj) )
    {
        if ( ( $key eq 'top' ) or ( $key eq 'up' ) )
        {
            next LINKS;
        }
        my $val        = $nav_links_obj->{$key};
        my $url        = escape_html( $val->direct_url() );
        my $title      = $val->title();
        my $title_attr = defined($title) ? " title=\"$title\"" : "";
        $nav_links_html .= "<link rel=\"$key\" href=\"$url\"$title_attr />\n";
    }
    my $leading_path_string = join( " → ",
        ( map { _render_leading_path_component($_) } @$leading_path ) );
    my $get_nav_links = sub {
        my $with_accesskey = shift;
        my $ret            = '';
        my @params;
        if ( $with_accesskey ne "" )
        {
            push @params, ( 'with_accesskey' => $with_accesskey );
        }
        $ret .= $nav_links_renderer->get_total_html(@params);
        $ret =~ s#"((?:\.\./)*)/#"$1#g;
        return $ret;
    };
    $vars->{latemp_get_html_body_nav_links} = $get_nav_links->(1);
    $vars->{latemp_get_html_body_nav_links__no_accesskey} =
        $get_nav_links->('');
    $vars->{leading_path_string} = $leading_path_string;
    $vars->{nav_bar}             = $nav_bar;
    $vars->{nav_links}           = $nav_links_html;
    my $nav_html = $rendered_results->{html};
    $vars->{nav_menu_html} = join( '', @$nav_html );
    $vars->{share_link}    = escape_html(
        uri_escape(
            MyNavData::get_hosts()->{ $nav_bar->current_host() }->{'base_url'}
                . $nav_bar->path_info()
        )
    );
    $vars->{index_filename}     = $fn2;
    $vars->{doxygen_url}        = $base_path . "michael_mann/";
    $vars->{arch_doc_url}       = $base_path . "arch_doc/";
    $vars->{use_online_wrapper} = (
        ( $input_tt2_page_path !~ m#\A js-fc-solve/#msx )
        ? <<"EOF"
<div class="use_online_wrapper"><a class="solve_online_button"
id="try_online_link" href="${base_path}js-fc-solve/text/"><span
class="try_main">Use</span><br/>
<span class="try_main">Online</span><br/>
<br/>
<span class="try_note">Firefox, Chrome, Opera, or IE10+</span></a></div>
EOF
        : ''
    );

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

    $vars->{load_javascript_srcs} = $load_javascript_srcs;

    my $html = '';
    $template->process( "src/$input_tt2_page_path.tt2",
        $vars, \$html, binmode => ':utf8', )
        or die $template->error();

    $toc->add_toc( \$html );
    if ( $self->stdout )
    {
        binmode STDOUT, ':encoding(utf-8)';
        print $html;
    }
    else
    {
        foreach my $d (@DESTs)
        {
            path( @$d, @fn, )->touchpath()->spew_utf8($html);
        }
    }
}

1;
