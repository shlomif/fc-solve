package FreecellSolver::Site::News;

use strict;
use warnings;

use autodie;

use utf8;

use base 'HTML::Widgets::NavMenu::Object';
use base 'Class::Accessor';

use MyOldNews;
use DateTime;

__PACKAGE__->mk_accessors(qw(
    dir
    items
    num_on_front
    ));

my @old_news_items =
(
    map
    {
        +{
            body => $_->{'html'},
            date => DateTime->new(
                year => $_->{year},
                month => $_->{mon},
                day => $_->{day_of_month}
                ),
        }
    } @{MyOldNews::get_old_news()}
);

sub file_to_news_item
{
    my $self = shift;
    my $filename = shift;
    my $text = do {
        local $/;
        open my $fh, "<", $self->dir()."/".$filename;
        binmode $fh, ":utf8";
        <$fh>;
    };
    my $title;
    if ($text =~ s{\A<!-- TITLE=(.*?)-->\n}{})
    {
        $title = $1;
    }
    $text =~ s!<p>!<p class="newsitem">!g;
    $text =~ s!<ol>!<ol class="newsitem">!g;
    $text =~ s!<ul>!<ul class="newsitem">!g;
    $text =~ s#<div class="blogger-post-footer"><img.*?</div>##ms;
    $filename =~ /\A(\d{4})-(\d{2})-(\d{2})\.html\z/;
    my ($y, $m, $d) = ($1, $2, $3);
    return
        +{
            'date' => DateTime->new(year => $y, month => $m, day => $d),
            'body' => $text,
            'title' => $title,
        };
}

sub calc_rss_items
{
    my $self = shift;

    opendir my $dir, $self->dir();
    my @files = readdir($dir);
    closedir($dir);
    @files = (grep { /^\d{4}-\d{2}-\d{2}\.html$/ } @files);
    @files = sort { $a cmp $b } @files;
    return [
        map {
            $self->file_to_news_item($_)
        } @files
        ];
}

sub calc_items
{
    my $self = shift;
    return [@old_news_items, @{$self->calc_rss_items()}];
}

sub _init
{
    my $self = shift;

    $self->dir("../lib/feeds/fc-solve.blogspot/");
    $self->num_on_front(7);

    $self->items($self->calc_items());

    return 0;
}

sub get_item_html
{
    my $self = shift;
    my $item = shift;

    my $title = $item->{title};
    my $date = $item->{date};

    return "<h3 class=\"newsitem\" id=\""
        . $date->strftime("news-%Y-%m-%d")
        . "\">" . $date->strftime("%d-%b-%Y")
        . (defined($title) ? ": $title" : "")
        . "</h3>\n\n"
        . $item->{'body'}
        ;
}

sub render_items
{
    my $self = shift;
    my $items = shift;
    return join("\n\n", (map { $self->get_item_html($_) } @$items));
}

sub render_front_page
{
    my $self = shift;
    my @items = reverse(@{$self->items()});
    return $self->render_items([@items[0..($self->num_on_front()-1)]]);
}

sub render_old
{
    my $self = shift;
    my @items = @{$self->items()};
    return $self->render_items([reverse(@items[0 .. (@items - $self->num_on_front())])]);
}

1;

