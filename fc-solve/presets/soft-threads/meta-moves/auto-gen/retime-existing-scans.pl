#!/usr/bin/perl

use strict;
use warnings;

use FC_Solve::TimePresets;

my $input_obj = FC_Solve::TimePresets->new;

my $params = $input_obj->calc_params_from_environment;

SCANS_LOOP:
foreach my $scan_rec ( @{ $input_obj->selected_scans() } )
{
    my $id = $scan_rec->id();
    my @cmd_line = split( / /, $scan_rec->cmd_line() );
    print "Now doing Scan No. $id - '@cmd_line'\n";

    # Scan was already done.
    if ( -e "data/$id.fcs.moves.txt" )
    {
        next SCANS_LOOP;
    }
    $input_obj->time_scan( { %$params, id => $id, argv => \@cmd_line, } );
}
