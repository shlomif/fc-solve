package FreecellSolver::Site::Rss;

use strict;
use warnings;

use autodie;

use base 'HTML::Widgets::NavMenu::Object';
use base 'Class::Accessor';

use XML::Feed;
use CGI;
use Encode;

__PACKAGE__->mk_accessors(qw(
    feed
));

sub _init
{
    my $self = shift;
    my $feed = XML::Feed->parse(
        URI->new(
            "http://fc-solve.blogspot.com/feeds/posts/default"
        )
    ) or die XML::Feed->errstr;
    $self->feed($feed);

    return 0;
}

sub _calc_entry_body
{
    my ($self, $entry) = @_;
    my $body = $entry->content()->body();

    my $title = decode('UTF-8', $entry->title());

    $body =~ s{<a name="cut[^"]*"></a>}{}g;
    $body =~ s{<table border="1">}{<table class="downloads">}g;
    # Remove trailing space.
    $body =~ s{[ \t]+$}{}gms;

    return "<!-- TITLE=" . CGI::escapeHTML($title) . "-->\n" . $body;
}

sub run
{
    my $self = shift;
    foreach my $entry ($self->feed()->entries())
    {
        my $text = $self->_calc_entry_body($entry) . "\n\n" . "<p><a href=\"" . CGI::escapeHTML($entry->link()) . "\">See comments and comment on this.</a></p>\n";
        my $issued = $entry->issued();
        my $filename = "lib/feeds/fc-solve.blogspot/" . $issued->ymd() . ".html";
        open my $out_fh, ">", $filename;
        binmode $out_fh, ":utf8";
        print {$out_fh} $text;
        close ($out_fh);
    }
}

1;

