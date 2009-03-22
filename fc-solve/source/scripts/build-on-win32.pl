#!/usr/bin/perl

use strict;
use warnings;

use Carp;

sub run
{
	my $cmd = shift;
	if (system(@$cmd))
	{
		confess ("[@{$cmd}] failed! $! $?");		
	}
}

run(
	[
	qq{C:\\Program Files\\CMake 2.6\\bin\\CMake},
	"-G", "MinGW Makefiles",
	@ARGV,
	"."
	]
);

run(
	[
	"C:\\strawberry\\c\\bin\\mingw32-make",
	]
);

run(
	[
	"C:\\strawberry\\c\\bin\\mingw32-make",
	"package",
	]
);

