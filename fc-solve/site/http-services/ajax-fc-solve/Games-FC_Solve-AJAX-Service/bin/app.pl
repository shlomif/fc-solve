#!/usr/bin/env perl

use FindBin;
use lib "$FindBin::Bin/../lib";

use Games::FC_Solve::AJAX::Service ();
Games::FC_Solve::AJAX::Service->dance;
