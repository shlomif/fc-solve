package HTML::Latemp::AddToc;

use strict;
use warnings;

use Moo;
use HTML::Toc          ();
use HTML::TocGenerator ();

my $TOC = qr#<toc */ *>#;

sub add_toc
{
    my ( $self, $html_ref ) = @_;
    if ( $$html_ref =~ $TOC )
    {
        my $toc = HTML::Toc->new();
        $toc->setOptions(
            {
                0
                ? (
                    templateAnchorName => sub {
                        return $_[-1] =~ /id="([^"]+)/
                            ? $1
                            : ( die "no id found" );
                    }
                    )
                : ( doLinkToId => 1 ),
                tokenToToc => [
                    map {
                        +{
                            tokenBegin => "<h" . ( $_ + 1 ) . " id=\\S+>",
                            level      => $_,
                        }
                    } ( 1 .. 5 )
                ],
            }
        );
        my $tocgen = HTML::TocGenerator->new();
        $tocgen->generate( $toc, $$html_ref, {} );
        my $text = $toc->format();
        $text =~ s%\A\s*<!-.*?->\s*%<h2 id="toc">Table of Contents</h2>%ms
            or die "foo";
        $text      =~ s%(</a>)\s*(<ul>)%$1<br/>$2%gms;
        $text      =~ s%<!-.*?->\s*%%gms;
        $$html_ref =~ s#$TOC#$text#g;
    }

    return;
}

1;
