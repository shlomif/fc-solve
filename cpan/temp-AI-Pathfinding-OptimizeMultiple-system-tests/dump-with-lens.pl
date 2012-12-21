#!/usr/bin/perl

use strict;
use warnings;

use MyInput;
use PDL::IO::FastRaw;

my $input = MyInput->new(
    {
        start_board => 1,
        num_boards => 32_000,
    },
);

my $with_lens = $input->get_scans_lens_data();

writefraw($with_lens, "with_lens.pdl");
