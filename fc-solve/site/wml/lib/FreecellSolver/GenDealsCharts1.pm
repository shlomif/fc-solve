use strict;
use warnings;
use IO::All qw/ io /;

foreach my $deal (
    sort {$a <=> $b }
    map { s#.*/##mrs =~ s#\.dump\.txt\z##mrs }
    glob("charts/fc-pro--4fc-intractable-deals--report/data/*.dump.txt"))
{
    my $data_id = "queue-items-$deal-data";
    my $chart_id = "queue-items-$deal-chart";
    print qq#<h4 id="queue-items-$deal">Deal $deal</h4>\n#;
    print <<"EOF";
    <div class="demo-container">
        <div id="$chart_id" class="demo-placeholder">
    </div>
    <script type="text/javascript">\$(function() { chart_data("#$data_id", "#$chart_id"); })</script>
EOF
    print qq#<textarea id="$data_id">\n#;
    print io->file("../dest/charts/fc-pro--4fc-intractable-deals--report/data/$deal.tsv")->all;
    print qq#</textarea>\n#;
}

1;
