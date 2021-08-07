package Shlomif::Homepage::TocDiv;

use strict;
use warnings;
use utf8;

sub toc_div
{
    my %args = %{ shift() // {} };
    $args{head_tag} //= 'h2';
    $args{lang}     //= 'en';
    my $lang_attr =
        $args{lang} eq 'en'
        ? "lang=\"en\""
        : "lang=\"he\"";
    my $title =
        $args{lang} eq 'en'
        ? "Table of Contents"
        : "תוכן העניינים";

    my $head = "<$args{head_tag} id=\"toc\">$title</$args{head_tag}>";
    $head = '';
    my $details = "<summary>$title</summary>";
    my $c =
        $args{collapse}
        ? "<details id=\"toc\">$details<toc $lang_attr nohtag=\"1\"/></details>"
        : "$head<toc $lang_attr/>";
    return qq#<nav class="page_toc">$c</nav>#;
}

1;

__END__
