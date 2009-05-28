#!/usr/bin/perl

use strict;
use warnings;

use MyInput;

use vars qw($scans_lens_data);

my @params = (1,32_000);
$scans_lens_data = MyInput::get_scans_lens_data(@params, 
    MyInput::get_selected_scan_list(@params));

while(1)
{
    sleep(1);
}
