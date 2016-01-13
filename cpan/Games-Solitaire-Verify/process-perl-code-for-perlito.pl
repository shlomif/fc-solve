#!/usr/bin/perl

use strict;
use warnings;

sub _gen_accessors
{
    my ($words_str) = @_;

    return join("\n\n", map { my $acc = $_ ; <<"EOF" } $words_str =~ /(\w+)/g);
sub $acc
{
    my \$self = shift;
    if (\@_)
    {
        \$self->{'$acc'} = shift;
    }
    return \$self->{'$acc'};
}
EOF
}

sub _gen_use_function_call
{
    my ($mod, $f) = @_;

    if ($mod eq 'List::Util' and $f eq 'first')
    {
        return "sub $f(&@) { my \$cb = shift; return &${mod}::$f(\$cb , \@_); }";
    }

    return "sub $f { return ${mod}::$f(\@_) }";
}

sub _gen_use_call
{
    my ($mod, $words_str) = @_;

    return "use $mod;\n\n" . join("\n\n", map { my $f = $_; _gen_use_function_call($mod, $f); } $words_str =~ /(\w+)/g);
}

my $text = do
{
    local $/;
    <ARGV>;
};

$text =~ s#^use parent '([^']+)';#use $1 (); use vars qw(\@ISA); \@ISA = (qw($1));#gms;

$text =~ s#^use Games::Solitaire::Verify::Exception;##gms;
$text =~ s#([\w:]+)->throw\s*\((.*?)\)\s*;#die +(bless { $2 }, '$1')#gms;
$text =~ s#^__PACKAGE__->mk_acc_ref\(\s*\[\s*qw\(([\s\w]+)\)\s*\]\s*\)\s*;#_gen_accessors($1)#egms;

$text =~ s#^use\s+([\w:]+)\s+qw\(([\s\w]+)\)\s*;\s*$#_gen_use_call($1, $2);#egms;

$text =~ s#open (\$\w+), ["']<["'], (\$\w+)#$1 = MainOpen::my_open($2)#gms;
$text =~ s#readline *\(([^\n]+)\) *;#MainOpen::readline($1);#gms;

$text =~ s#my \$err;\n\s*if\s*\(!\s*\$\@\).*?\n([ \t]+return\s*;)#if (\$\@) { die \$\@; }\n\n$1#gms;

$text =~ s#^(\s*)chomp\((\$[A-Za-z0-9]+)\);\s*$#$1$2 =~ s{\\n\\z}{};#gms;

$text =~ s#\\A#^#g;
$text =~ s#\\z#\$#g;

$text =~ s#(first|firstidx)(\s*)\{([^\}]*)\}(.*?);#$1(${2}sub {$3},$4);#gms;
print $text;
