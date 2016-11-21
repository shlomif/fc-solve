package FreecellSolver::GenDealsCharts1;

use strict;
use warnings;
use IO::All qw/ io /;

sub format_num
{
    my $s = shift;
    my @d;
    while (length $s)
    {
        unshift @d, substr($s,-3);
        $s = substr($s,0,-3);
    }
    return join",",@d;
}

sub _calc_deal_nums
{
    return
    [
        sort { $a <=> $b }
        map { s#.*/##mrs =~ s#\.dump\.txt\z##mrs }
        glob("charts/fc-pro--4fc-intractable-deals--report/data/*.dump.txt")
    ];
}

sub gen_progress_charts
{
    foreach my $deal (@{_calc_deal_nums()})
    {
        my $data_id = "queue-items-$deal-data";
        my $chart_id = "queue-items-$deal-chart";
        print qq#<h4 id="queue-items-$deal">Deal @{[format_num($deal)]}</h4>\n#;
        print <<"EOF";
    <div class="demo-container">
        <div id="$chart_id" class="demo-placeholder"></div>
    </div>
    <script type="text/javascript">\$(function() { chart_data("#$data_id", "#$chart_id"); })</script>
    <br />
EOF
        print qq#<textarea id="$data_id" cols="40" rows="20" readonly="readonly" class="fcs_data">\n#;
        # print io->file("../dest/charts/fc-pro--4fc-intractable-deals--report/data/$deal" . ($deal eq '6825625742' ? ".filtered" : '') . ".tsv")->all;
        print io->file("../dest/charts/fc-pro--4fc-intractable-deals--report/data/$deal.filtered.tsv")->all;
        print qq#</textarea>\n<br />\n#;
    }
}

sub gen_summary_table
{
    print <<'EOF';
<table class="fcs_depth_dbm_deals" summary="Summary of the iterations for the deals">
<tr>
<th>Deal No.</th>
<th>Iterations Reached</th>
</tr>
EOF
    foreach my $deal (@{_calc_deal_nums()})
    {
        print "<tr><td>" . format_num($deal) . "</td><td>" . format_num(io->file("../dest/charts/fc-pro--4fc-intractable-deals--report/data/$deal.tsv")->tail(1) =~ s#\t.*##mrs) . "</td></tr>\n";
    }
    print <<'EOF';
</table>
EOF
}
1;
