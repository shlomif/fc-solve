package FreecellSolver::Site::Rss;

use strict;
use warnings;
use autodie;
use parent 'HTML::Widgets::NavMenu::Object';

use XML::Feed ();
use CGI       ();
use Encode qw/ decode /;

sub feed
{
    my $self = shift;

    if (@_)
    {
        $self->{feed} = shift;
    }

    return $self->{feed};
}

sub _init
{
    my $self = shift;
    my $feed =
        XML::Feed->parse(
        URI->new("http://fc-solve.blogspot.com/feeds/posts/default") )
        or die XML::Feed->errstr;
    $self->feed($feed);

    return;
}

sub _calc_entry_body
{
    my ( $self, $entry ) = @_;
    my $body = $entry->content()->body();

    my $title = decode( 'UTF-8', $entry->title() );

    $body =~ s{<a name="cut[^"]*"></a>}{}g;
    $body =~ s{<table border="1">}{<table class="downloads">}g;

    # Remove trailing space.
    $body =~ s{[ \t]+$}{}gms;

    return "<!-- TITLE=" . CGI::escapeHTML($title) . "-->\n" . $body;
}

sub run
{
    my $self = shift;
    foreach my $entry ( $self->feed()->entries() )
    {
        my $text =
              $self->_calc_entry_body($entry) . "\n\n"
            . "<p><a href=\""
            . CGI::escapeHTML( $entry->link() )
            . "\">See comments and comment on this.</a></p>\n";
        my $issued = $entry->issued();
        my $filename =
            "lib/feeds/fc-solve.blogspot/" . $issued->ymd() . ".html";
        open my $out_fh, ">:encoding(UTF-8)", $filename;
        print {$out_fh} $text;
        close($out_fh);
    }
    return;
}

1;
