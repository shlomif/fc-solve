package MyNews;

use strict;
use warnings;

use POSIX;

use CGI;

use DateTime;
use XML::Feed;

use MyOldNews;

sub _render_news_item
{
    my $item = shift;

    my $issued = $item->{'issued'};
    my $html = $item->{'html'};
    my $title = $item->{'title'} || "";

    $title = $issued->strftime("%d-%b-%Y")
        . (length($title) ? ": " : "") 
        . $title;

    my $id = $issued->strftime("news-%Y-%m-%d");

    # Remove a leading 0 which is ugly.
    $title =~ s{\A0}{};

    my $id_esc = CGI::escapeHTML($id);
    my $title_esc = CGI::escapeHTML($title);

    return <<"EOF";
<h3 id="$id_esc">$title_esc</h3>
$html
EOF
}

sub _convert_from_old_news_item
{
    my $item = shift;

    my $y = $item->{'year'};
    my $m = $item->{'mon'};
    my $d = $item->{'day_of_month'};

    my $html = $item->{'html'};

    my $issued = DateTime->new(
        year => $y, month => $m, day => $d,
        hour => 12, minute => 0, second => 47,
    );

    return 
        {
            issued => $issued,
            html => $html,
            # id => $issued->strftime("news-%Y-%m-%d"),
        };
}

sub _filter_out_br_tags
{
    my $text = shift;
    $text =~ s{< *br */? *>}{}g;
    return $text;
}

sub _calc_blog_news
{
    my $feed = XML::Feed->parse(URI->new("http://fc-solve.blogspot.com/feeds/posts/default"))
        or die XML::Feed->errstr;

    my @results;
    for my $entry ($feed->entries) {
        push @results, 
            {
                issued => $entry->issued->clone(),
                html => _filter_out_br_tags($entry->content->body()),
                title => $entry->title(),
                # id => ($entry->issued->strftime("news-%Y-%m-%d-"),
            }
            ;
    }

    return [ sort 
        { DateTime->compare($a->{'issued'}, $b->{'issued'}) } 
        @results
    ];
}

my $num_new_items = 6;

my $results =
    [ 
        (map { _convert_from_old_news_item($_) } @{MyOldNews::get_old_news()}), 
        @{_calc_blog_news()}
    ];

sub _print_results
{
    my $start_idx = shift;
    my $end_idx = shift;

    binmode STDOUT, ":utf8";

    print map { _render_news_item($_) } 
        reverse(@{$results}[$start_idx .. $end_idx])
        ;

    return;
}

sub print_old
{
    return _print_results(0, ($#{$results} - $num_new_items));
}

sub print_new
{
    return _print_results((@{$results} - $num_new_items), $#{$results});
}

1;

