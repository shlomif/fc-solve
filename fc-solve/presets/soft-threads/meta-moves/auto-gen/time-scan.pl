#!/usr/bin/perl

use strict;
use warnings;

use List::MoreUtils qw(firstidx);
use FC_Solve::TimePresets;

my $opt = ( ( $ARGV[0] eq "--gen-bat" ) ? shift : "" );

my @scan_args = @ARGV;
my $scan      = join( " ", @scan_args );

sub get_scan_index_by_its_cmd_line
{
    my ( $input_obj, $cmd_line ) = @_;

    my $sel_scans = $input_obj->selected_scans;

    return ( firstidx { $_->cmd_line() eq $cmd_line } @$sel_scans );
}

my $input_obj = FC_Solve::TimePresets->new;

if ( ( my $index = get_scan_index_by_its_cmd_line( $input_obj, $scan ) ) >= 0 )
{
    die "The scan already exists with the ID "
        . $input_obj->selected_scans->[$index]->id() . "\n";
}

my $id = $input_obj->get_next_id();
{
    open my $out_fh, ">>", "scans.txt";
    print {$out_fh} "$id\t$scan\n";
    close($out_fh);
}

my %params = (
    %{ FC_Solve::TimePresets->calc_params_from_environment() },
    id   => $id,
    argv => [@scan_args],
);

if ( $opt eq "--gen-bat" )
{
    open my $out, ">", "run_scan_$id.bat"
        or die "Could not open run_scan_$id.bat for writing";

    print {$out} join( " ", @{ $input_obj->_get_scan_cmd_line( \%params ) } ),
        "\n\r";
    close($out);
}
else
{
    $input_obj->time_scan( \%params );
}
