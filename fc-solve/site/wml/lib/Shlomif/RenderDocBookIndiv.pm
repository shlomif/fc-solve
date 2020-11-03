package Shlomif::RenderDocBookIndiv;

use strict;
use warnings;

use Path::Tiny qw/ path /;

use XML::LibXML  ();
use XML::LibXSLT ();

my $xslt      = XML::LibXSLT->new;
my $style_doc = XML::LibXML->load_xml(
    location => "./bin/clean-up-docbook-xhtml-1.1.xslt",
    no_cdata => 1
);
my $stylesheet = $xslt->parse_stylesheet($style_doc);

sub render_docbook
{
    my ($args) = @_;

    # my $argv = $args->{argv}
    # App::Docmake->new( { argv => $argv, } )->run();
    my $dir = path( $args->{htmls_dir} );

    foreach my $fn ( $dir->children(qr/\.x?html\z/) )
    {
        my $source  = XML::LibXML->load_xml( location => $fn, );
        my $results = $stylesheet->transform($source);
        my $str_ref = $stylesheet->output_as_chars($results);
        $str_ref =~ s#<p>\s*?</p>##gms;
        $str_ref =~ s/[ \t]+$//gms;
        $fn->spew_utf8($str_ref);
    }

    return;
}

1;

__END__

=head1 COPYRIGHT & LICENSE

Copyright 2020 by Shlomi Fish

This program is distributed under the MIT / Expat License:
L<http://www.opensource.org/licenses/mit-license.php>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut
