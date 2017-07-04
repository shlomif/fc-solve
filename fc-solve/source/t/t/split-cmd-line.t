#!/usr/bin/perl

use strict;
use warnings;

use FC_Solve::SplitCmdLine ();
use Test::More tests => 20;

sub check_split
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $input_string = shift;
    my $want_argv    = shift;
    my $msg          = shift;

    my $have_argv =
        FC_Solve::SplitCmdLine->split_cmd_line_string($input_string);

    is_deeply( $have_argv, $want_argv, $msg, );
}

# TEST
check_split( "one two three", [ "one", "two", "three", ], "Simple barewords", );

# TEST
check_split(
    "one two three lom-prom-tom KuTler abruptChange Come-Together",
    [
        "one",    "two",          "three", "lom-prom-tom",
        "KuTler", "abruptChange", "Come-Together",
    ],
    "Simple barewords No. 2",
);

# TEST
check_split(
    q{I read    "The Adventures of Tom Sawyer" and liked it.},
    [ "I", "read", "The Adventures of Tom Sawyer", "and", "liked", "it.", ],
    "Words in quotes + more than one space.",
);

# TEST
check_split(
    qq{I read \\"The       Adventure\\"},
    [ "I", "read", q{"The}, q{Adventure"}, ],
    "Escaped quotes",
);

# TEST
check_split(
    qq{I read \\"The       Adventure\\"\n},
    [ "I", "read", q{"The}, q{Adventure"}, ],
    "Escaped quotes with newline",
);

# TEST
check_split(
    qq{I read \\"The       Adventure\\"                \n},
    [ "I", "read", q{"The}, q{Adventure"}, ],
    "Escaped quotes line with trailing space",
);

# TEST
check_split(
    qq{I read \\"The       Adventure\\"      },
    [ "I", "read", q{"The}, q{Adventure"}, ],
    "Escaped quotes with trailing space.",
);

# TEST
check_split( qq{GNU\\\\Linux and Platro\\\\Day\n},
    [ "GNU\\Linux", "and", "Platro\\Day", ], "Backslash", );

# TEST
check_split(
    qq{Hi "Mostly \\"Done deal\\" Plurality" There},
    [ "Hi", "Mostly \"Done deal\" Plurality", "There", ],
    "Backslash-quotes inside quotes.",
);

# TEST
check_split(
    qq{Hi "Mostly \\"Done deal\\" w\\\\o Plurality" There},
    [ "Hi", "Mostly \"Done deal\" w\\o Plurality", "There", ],
    "Backslash-quotes and backslash inside quotes.",
);

# TEST
check_split(
    qq{Aloha\\ Audrey and Alan\n},
    [ "Aloha Audrey", "and", "Alan", ],
    "backslash-space.",
);

# TEST
check_split(
    qq{Aloha\\ Audrey and Alan},
    [ "Aloha Audrey", "and", "Alan", ],
    "backslash-space without newline",
);

# TEST
check_split(
    qq{One\\ Two" Three Four Five"Six\\ Seven & Eight\n},
    [ "One Two Three Four FiveSix Seven", "&", "Eight", ],
    "Mixed quotes and backslash space.",
);

# TEST
check_split(
    qq{       One\\ Two" Three Four Five"Six\\ Seven & Eight   },
    [ "One Two Three Four FiveSix Seven", "&", "Eight", ],
    "Mixed quotes and backslash space with leading and trailing space",
);

# TEST
check_split(
    <<'EOF',
# This is a comment.
OneWord word2 "Word 3"  \
    Word-No-4

EOF
    [ "OneWord", "word2", "Word 3", "Word-No-4", ],
    "Leading comment",
);

# TEST
check_split(
    <<'EOF',
# This is a comment.
OneWord word2 "Word 3"  \
   WORD_NO_444    # End of line comment.

EOF
    [ "OneWord", "word2", "Word 3", "WORD_NO_444", ],
    "Leading comment",
);

# TEST
check_split(
    <<'EOF',
First Line
Second Line
EOF
    [ "First", "Line", "Second", "Line", ],
    "Two lines",
);

# TEST
check_split(
    <<'EOF',
First Line   \
Second Line
EOF
    [ "First", "Line", "Second", "Line", ],
    "Two lines with trailing backslash",
);

# TEST
check_split(
    <<'EOF',
First Line
Second Line
"Third ""L-R"
EOF
    [ "First", "Line", "Second", "Line", "Third L-R", ],
    "Three lines.",
);

# TEST
check_split(
    <<'EOF',
First Line
Second Line                 # A comment
"Third ""L-R"
EOF
    [ "First", "Line", "Second", "Line", "Third L-R", ],
    "Three lines with a comment",
);

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2009 Shlomi Fish

=cut
