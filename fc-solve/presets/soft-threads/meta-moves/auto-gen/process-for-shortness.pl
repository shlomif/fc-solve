#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long;

use List::Util qw(min);

use Shlomif::FCS::CalcMetaScan;

use Text::Table;

use MyInput;

my $with_len = 0;

GetOptions(
    "l!" => \$with_len,
);

my ($scan_id, $board_idx) = @ARGV;

my @guessed_quotas = ((350) x 300);

my @final_quotas;

my $start_board = 1;
my $num_boards = 32_000;

my $input_obj = MyInput->new(
    {
        start_board => $start_board,
        num_boards => $num_boards,
    }
);

my $scan_index = 0;

my $data = $input_obj->get_scans_lens_data();

my @results;
foreach my $scan (@{$input_obj->selected_scans()})
{
    my $scan_id = $scan->id();
    my $cmd_line = $scan->cmd_line();

    my $vec = $data->slice(":,$scan_index,0");
    $vec = $vec->where($vec > 0);
    
    my $sorted = $vec->flat()->qsort(); 

    # The step should be the 90% percentile, but not higher than
    # 10,000 iterations.
    my $quota = min($sorted->at(int(($sorted->dims())[0] * 0.9)), 10_000);

    push @results, { cmd_line => $cmd_line, id => $scan_id, quota => $quota };
}
continue
{
    $scan_index++;
}


# Flair is our temporary name for these independently evaluated instances
# print join(" \\\n--next-flair ", map { "--flair-id $_->{id} --flair-quota $_->{quota} $_->{cmd_line}" } @results);
if (0) 
{
print 
(
    join(" \\\n-nf ", map { "--flare-name $_->{id} $_->{cmd_line} -opt" } @results),
    " \\\n --flares-plan '",
    join(",", map { "Run:$_->{quota}\@$_->{id}" } @results),
    "'\n",
);
}

{
    my $x = $data->slice(":,:,1")->clump(1..2);

    $x = ($x >= 0) * $x + ($x < 0) * PDL->ones($x->dims()) * 100_000;

    my $z = $x->xchg(0,1);
    
    my $s = $z->qsort();

    #my $histograms =
    #    $x->xchg(0,1)->qsorti()->xchg(0,1)->histogram(1,0,scalar(@results));

    # print $histograms;

    my $sep = \'|';
    my $major_sep = \'||';
    my $tb = Text::Table->new($sep,  "Place", $major_sep, (map { $_->id(), $sep,} @{$input_obj->selected_scans()}));

    my $num_scans = @{$input_obj->selected_scans()};

    my $get_place = sub {
        my ($idx) = @_;
        return ($s->slice("$idx,:")->clump(0,1)->dummy(0, $num_scans) == $z);
    };

    foreach my $idx (0 .. $num_scans - 1)
    {
        # $tb->load([$idx+1, $histograms->slice(":,$idx")->list()]);
        $tb->load([$idx+1,
                (($idx == 0)
                    ? $get_place->($idx)
                    : ( $get_place->($idx) & (~ $get_place->($idx-1)) ) 
                )->xchg(0,1)->sumover()->list()
            ]
        );
    }

    my $rule = $tb->rule('-', '+');

    print $rule, $tb->title, $rule, 
        map { $_, $rule, } $tb->body();

=begin Foo
    print map { $results[$_]->{id} . ": " . $histogram[$_] . "\n" }
        reverse sort { $histogram[$a] <=> $histogram[$b] }
        (0 .. $#histogram)
        ;

=cut

}

