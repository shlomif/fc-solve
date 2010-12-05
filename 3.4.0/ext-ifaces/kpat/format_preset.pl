#!/usr/bin/perl -w

sub arg_to_c
{
    my $arg = shift;
    $arg =~ s/\"/\\\"/;
    return "\"$arg\"";
}

my @args = (map { chomp($_); $_ } (<>));
my $length = 0;

my @c_args = map { &arg_to_c($_) } @args;

print "const char const * args[" . scalar(@c_args) . "] = \n";
print "{\n";
foreach my $arg (@c_args[0 .. ($#c_args-1)])
{
    $length += length($arg)+2;
    if ($length > 60)
    {
        print "\n";
        $length = 0;
    }
    print "$arg, ";
}
print $c_args[-1], "\n};\n";

