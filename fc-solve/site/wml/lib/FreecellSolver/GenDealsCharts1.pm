package FreecellSolver::GenDealsCharts1;

use strict;
use warnings;
use autodie;
use Path::Tiny qw/ path /;

sub format_num
{
    my $s = shift;
    my @d;
    while ( length $s )
    {
        unshift @d, substr( $s, -3 );
        $s = substr( $s, 0, -3 );
    }
    return join ",", @d;
}

sub _calc_deal_nums
{
    return [
        sort     { $a <=> $b }
            grep { /\A[0-9]+\z/ }
            map  { s#.*/##mrs =~ s#\.dump\.txt\z##mrs } glob(
            "src/charts/fc-pro--4fc-intractable-deals--report/data/*.dump.txt")
    ];
}

sub gen_progress_charts
{
    my ( $class, $args ) = @_;

    $args //= +{};
    my $try2 = $args->{try2} // 0;
    my $fh   = $args->{fh};

    my @funcs;
    my $_calc_deal_nums = _calc_deal_nums;
    while ( my ( $idx, $deal ) = each(@$_calc_deal_nums) )
    {
        my $data_id  = "queue-items-$deal-data";
        my $chart_id = "queue-items-$deal-chart";
        my $func_id  = "my_chart_$idx";
        push @funcs, $func_id;
        $fh->print(
            qq#<h4 id="queue-items-$deal">Deal @{[format_num($deal)]}</h4>\n#);
        $fh->print( <<"EOF");
    <div class="demo-container">
        <div id="$chart_id" class="demo-placeholder"></div>
    </div>
    <script>function $func_id(chart_data) { chart_data("#$data_id", "#$chart_id"); }</script>
    <br />
EOF
        $fh->print(
qq#<textarea id="$data_id" cols="40" rows="20" readonly="readonly" class="fcs_data">\n#
        );

# print io->file("../dest/charts/fc-pro--4fc-intractable-deals--report/data/$deal" . ($deal eq '6825625742' ? ".filtered" : '') . ".tsv")->all;
        $fh->print(
            path(
                "./dest/charts/fc-pro--4fc-intractable-deals--report/data/$deal"
                    . (
                    ( $try2 && ( $deal eq '6825625742' ) ) ? '--try2' : ''
                    )
                    . ".filtered.tsv"
            )->slurp_raw =~ s%(?:\A|\n)\K[^\t\n]+\t%%gmrs
        );
        $fh->print(qq#</textarea>\n<br />\n#);
    }
    $fh->print( <<"EOF");
<script>
function _fcs_chart_all(chart_data) {
    var funcs = [@{[join",",@funcs]}];
    var call_func;
    call_func = function(idx) {
        if (idx == funcs.length)
        {
            return;
        }
        window.setTimeout(function() {
            funcs[idx](chart_data);
            call_func(idx+1);
        }, 20);
        return;
    };
    call_func(0);
    return;
};
</script>
EOF
}

sub gen_summary_table
{
    my ( $class, $args ) = @_;

    $args //= +{};
    my $try2 = $args->{try2} // 0;
    my $fh   = $args->{fh};

    $fh->print( <<'EOF');
<table class="fcs_depth_dbm_deals">
<tr>
<th>Deal No.</th>
<th>Iterations Reached</th>
</tr>
EOF
    foreach my $deal ( @{ _calc_deal_nums() } )
    {
        $fh->print(
                  "<tr><td>"
                . format_num($deal)
                . "</td><td>"
                . format_num(
                (
                    path(
"./dest/charts/fc-pro--4fc-intractable-deals--report/data/$deal"
                            . (
                            ( $try2 && ( $deal eq '6825625742' ) )
                            ? '--try2'
                            : ''
                            )
                            . ".tsv"
                    )->lines( { count => -1 } )
                )[0] =~ s#\t.*##mrs
                )
                . "</td></tr>\n"
        );
    }
    $fh->print("</table>\n");
}
1;
