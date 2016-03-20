#!/usr/bin/perl

use strict;
use warnings;

use lib "./lib";

use FreecellSolver::Site::Rss;

my $feed = FreecellSolver::Site::Rss->new();
$feed->run();

# Touch the files so they'll be recompiled.
utime(undef, undef, "src/index.html.wml", "src/old-news.html.wml");
