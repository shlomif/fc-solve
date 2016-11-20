use strict;
use warnings;
use IO::All qw/ io /;

foreach my $deal (map { s#.*/##mrs =~ s#\.dump\.txt\z##mrs } glob("charts/fc-pro--4fc-intractable-deals--report/data/*.dump.txt"))
{
    print qq#<h4 id="queue-items-$deal">Deal $deal</h4>\n#;
    print qq#<textarea id="queue-items-$deal-data">\n#;
    print io->file("../dest/charts/fc-pro--4fc-intractable-deals--report/data/$deal.tsv")->all;
    print qq#</textarea>\n#;
}

1;
