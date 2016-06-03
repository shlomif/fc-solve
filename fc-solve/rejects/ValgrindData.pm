package ValgrindData;

use strict;
use warnings;

sub foo
{
    return +{
    'board_gen__aisleriot__t_only' =>
    {
        prog => "board_gen/make-aisleriot-freecell-board",
        argv => [qw(-t)
        ],
        msg => qq{Board generation should not crash with only -t flag (aisleriot)},
    },
    'board_gen__gnome__t_only' =>
    {
        prog => "board_gen/make-gnome-freecell-board",
        argv => [qw(-t)
        ],
        msg => qq{Board generation should not crash with only -t flag (gnome)},
    },
};
}

1;
