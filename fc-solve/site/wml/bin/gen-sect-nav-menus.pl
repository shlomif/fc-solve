#!/usr/bin/perl

use Carp::Always;

use strict;
use warnings;

use lib './lib';

use File::Basename qw/ dirname /;
use Path::Tiny qw/ path /;
use List::MoreUtils qw/ natatime /;

use NavDataRender;
use MyNavData;
use URI::Escape qw(uri_escape);
use HTML::Widgets::NavMenu::EscapeHtml qw(escape_html);
use Parallel::ForkManager;

sub get_page_path
{
    my $filename = "<get-var filename />";
    $filename =~ s{index\.html$}{};

    return $filename;
}

sub get_root
{
    my $url = shift;

    $url =~ s#\A/##;

    my $ret = ( ( "../" x ( $url =~ y#/#/# ) ) =~ s#/\z##r );

    return ( ( $ret eq '' ) ? '.' : $ret );
}

use Shlomif::Out qw/write_on_change/;

sub _out
{
    my ( $path, $text_cb ) = @_;

    my $text_ref = $text_cb->();

    my $text = ( $$text_ref
            . ( ( $$text_ref !~ /\n\z/ && $$text_ref =~ /\S/ ) ? "\n" : '' ) );

    write_on_change( scalar( path($path) ), \$text, );

    return;
}

# At least temporarily disable Parallel::ForkManager because it causes
# the main process to exit before all the work is done.

my $WITH_PM = 1;

my $pm;

if ($WITH_PM)
{
    $pm = Parallel::ForkManager->new(4);
}

foreach my $host (qw(fc-solve))
{
    my $hostp = "lib/cache/combined/$host";

    my $it = natatime 8,
        (
        ( $host eq 'fc-solve' )
        ? @ARGV
        : (qw(index.html lecture/index.html))
        );

URLS:
    while ( my @urls = $it->() )
    {
        if ($WITH_PM)
        {
            my $pid = $pm->start;

            if ($pid)
            {
                next URLS;
            }
        }

        foreach my $proto_url (@urls)
        {
            my $url = $proto_url =~ s#(\A|/)index\.html\z#$1#r;

            my $filename = "/$url";

            # urlpath.
            my $urlp = "$hostp/" . ( $url =~ s#(\A|/)$#${1}index.html#r ) . '/';
            path($urlp)->mkpath;

            print "start filename=$filename\n";

            my $nav_bar = HTML::Widgets::NavMenu::JQueryTreeView->new(
                'path_info'    => $filename,
                'current_host' => $host,
                MyNavData::get_params(),
                'ul_classes' => [ "navbarmain", ("navbarnested") x 10 ],
                'no_leading_dot' => 1,
            );

            my $rendered_results = $nav_bar->render();

            my $leading_path = $rendered_results->{leading_path};

            my $nav_results = NavDataRender->nav_data_render(
                {
                    filename => $url,
                    host     => $host,
                    ROOT     => get_root($url),
                }
            );

            my $shlomif_nav_links_renderer = $nav_results->{nav_links_renderer};

            my $nav_links_obj = $rendered_results->{nav_links_obj};

            my $out = sub {
                my ( $id, $cb ) = @_;

                return _out( $urlp . $id, $cb );
            };

            $out->(
                'main_nav_menu_html',
                sub {
                    return \( join( "\n", @{ $rendered_results->{html} } ) );
                }
            );
            $out->(
                'html_head_nav_links',
                sub {
                    return \(
                        NavDataRender->get_html_head_nav_links(
                            { nav_links_obj => $nav_links_obj }
                        )
                    );
                }
            );

            $out->(
                'page_url',
                sub {
                    return \(
                        escape_html(
                            uri_escape(
                                MyNavData::get_hosts()->{$host}->{base_url}
                                    . $url
                            )
                        )
                    );
                }
            );

            foreach my $with_accesskey ( '', 1 )
            {
                $out->(
                    "shlomif_nav_links_renderer-with_accesskey=$with_accesskey",
                    sub {
                        my @params;
                        if ( $with_accesskey ne "" )
                        {
                            push @params,
                                ( 'with_accesskey' => $with_accesskey );
                        }
                        return \(
                            join(
                                '',
                                $shlomif_nav_links_renderer->get_total_html(
                                    @params)
                            )
                        );
                    },
                );
            }

            if ( $filename eq '/site-map/' )
            {
                $out->(
                    'shlomif_main_expanded_nav_bar',
                    sub {
                        return \(
                            join(
                                '',
                                map { "$_\n" } @{
                                    $nav_results
                                        ->{shlomif_main_expanded_nav_bar}
                                        ->gen_site_map()
                                }
                            )
                        );
                    },
                );
            }
            print "filename=$filename\n";
        }

        if ($WITH_PM)
        {
            $pm->finish;    # Terminates the child process
        }
    }
}

if ($WITH_PM)
{
    $pm->wait_all_children;
}
