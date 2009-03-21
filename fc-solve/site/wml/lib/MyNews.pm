package MyNews;

use strict;
use warnings;

use MyOldNews;
use POSIX;
use DateTime;

sub _render_old_news_item
{
    my $item = shift;

    my $y = $item->{'year'};
    my $m = $item->{'mon'};
    my $d = $item->{'day_of_month'};

    my $html = $item->{'html'};

    my ($h3_id, $title) = DateTime->new(
        year => $y, month => $m, day => $d,
        hour => 12, minute => 0, second => 47,
    )->strftime("news-%Y-%m-%d", "%d-%b-%Y");

    # Remove a leading 0 which is ugly.
    $title =~ s{\A0}{};

    return <<"EOF";
<h3 id="$h3_id">$title</h3>
$html
EOF
}

my $num_new_items = 6;

sub print_old
{
    my $results = MyOldNews::get_old_news();
    print map { _render_old_news_item($_) } 
        reverse(@{$results}[0 .. ($#{$results} - $num_new_items)])
        ;
}

sub print_new
{
    my $results = MyOldNews::get_old_news();
    print map { _render_old_news_item($_) } 
        reverse(@{$results}[(@{$results} - $num_new_items) .. $#{$results}])
        ;

}
1;

