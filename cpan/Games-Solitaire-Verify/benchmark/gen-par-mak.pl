#!/usr/bin/perl

use strict;
use warnings;

use IO::All;
use Template;

my $MIN = 1;
my $MAX = 32_000;
my $STEP = 100;

my $ARGS = +{
    min => $MIN,
    max => ($MAX / $STEP),
    step => $STEP,
    res => sub {
        return "Results/" . shift . ".res";
    },
};

my $template = Template->new(
    {
            EVAL_PERL    => 1,               # evaluate Perl code blocks
    }
);

my $TEXT = <<'EOF';
all:[% FOREACH i = [min .. max] %] [% res(i) %][% END %]

[% FOREACH i = [min .. max] %]
[% res(i) %]:
[% "\t" %]F=[% min + (i - 1) * step %] L=[% min + i * step - 1 %] perl benchmark-no-backticks.pl -- -l as -ni -l fg > [% res(i) %]
[% END %]
EOF

$template->process(\$TEXT,
    $ARGS,
    'par2.mak',
) or die $template->error;

my $NINJA_TEXT = <<'EOF';
rule b
  command = F=$f L=$l perl benchmark-no-backticks.pl -- -l as -ni -l fg > $out

[% FOREACH i = [min .. max] %]
build [% res(i) %]: b
  f=[% min + (i - 1) * step %]
  l=[% min + i * step - 1 %]
[% END %]
EOF

$template->process(\$NINJA_TEXT,
    $ARGS,
    'build.ninja',
) or die $template->error;
