package Shlomif::Spelling::FindFiles;

use strict;
use warnings;

use MooX qw/late/;
use List::MoreUtils qw/any/;

use HTML::Spelling::Site::Finder ();

my @prunes = (
    qr#\Adest/js/jquery-ui/#,
    qr#\Adest/js/yui-unpack/#,

    qr#\A dest/js-fc-solve/text/gui-tests\.xhtml \z#msx,
    qr#\Adest/mail-lists/#,

    # qr#\A dest/book--freecell-solver--evolution-of-a-c-program/#msx,
);

sub list_htmls
{
    my ($self) = @_;

    return HTML::Spelling::Site::Finder->new(
        {
            root_dir => 'dest',
            prune_cb => sub {
                my ($path) = @_;
                return any { $path =~ $_ } @prunes;
            },
        }
    )->list_all_htmls;
}

1;
