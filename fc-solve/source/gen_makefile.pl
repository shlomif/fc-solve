#!/usr/bin/perl

use strict;

sub min
{
    my $ret = shift;
    foreach (@_)
    {
        if ($_ < $ret)
        {
            $ret = $_;
        }
    }

    return $ret;
}

my @objects=(
    qw(alloc app_str caas card cmd_line fcs_dm fcs_hash fcs_isa freecell), 
    qw(intrface lib lookup2 move pqueue preset rand scans simpsim state)
    );

my @targets = (
    {
        'exe' => "fc-solve",
        'objs' => [ qw(main) ],
    },
    {
        'exe' => "freecell-solver-range-parallel-solve",
        'objs' => [ qw(test_multi_parallel) ],
    },
);

my @headers=
    (
        qw(alloc app_str caas card config fcs_cl fcs fcs_dm fcs_enums),
        qw(fcs_hash fcs_isa fcs_move fcs_user inline jhjtypes), 
        qw(move ms_ca pqueue preset rand state test_arr tests)
    );

my @defines=(qw(WIN32));

my @debug_defines = (qw(DEBUG));

print "all: fc-solve.exe freecell-solver-range-parallel-solve.exe\n\n";

print "OFLAGS=" . "/Og2 " . join(" ", (map { "/D".$_ } @defines)) . "\n";
print "DFLAGS=\$(OFLAGS) " . join(" ", (map { "/D".$_ } @debug_defines)) . "\n";

print "INCLUDES=" . join(" ", (map { $_.".h" } @headers)). "\n";
print "CC=cl\n";
print "LIB32=link.exe\n";


print "\n\n";

print "OBJECTS = " . join(" ", (map { $_.".obj" } @objects)) . "\n";

print "\n\n";

foreach my $o (@objects, (map { @{$_->{'objs'}} } @targets))
{
    print "$o.obj: $o.c \$(INCLUDES)\n";
    print "\t\$(CC) /c /Fo$o.obj \$(OFLAGS) $o.c\n";
    print "\n";
}

print "\n\n###\n### Final Targets\n###\n\n\n";

foreach my $t (@targets)
{
    my $exe = $t->{'exe'};
    my @objs = @{$t->{'objs'}};

    #my $obj_line = "\$(OBJECTS) " . join(" ", (map { "$_.obj" } @objs));
    my $obj_line = "freecell-solver-static.lib " . join(" ", (map { "$_.obj" } @objs));

    print "$exe.exe: $obj_line\n";
    print "\t\$(CC) /Fe$exe.exe /F0x2000000 $obj_line\n";
    print "\n";
}

print "freecell-solver-static.lib: \$(OBJECTS)\n";
print "\t\$(LIB32) -lib \$(OBJECTS) /out:freecell-solver-static.lib\n";
print "\n";

print "freecell-solver.dll: \$(OBJECTS) freecell-solver.def\n";
print "\t\$(LIB32) kernel32.lib user32.lib gdi32.lib /dll /out:freecell-solver.dll /implib:freeecell-solver.lib /DEF:freecell-solver.def \$(OBJECTS) \n";
print "\n";

#print "fc-solve.exe: \$(OBJECTS)\n";
#print "\t\$(CC) /Fefc-solve.exe /F0x2000000 \$(OBJECTS)\n";
#print "\n";

print "clean:\n";
print "\tdel *.obj *.exe *.lib *.dll *.exp\n";

open I, "<Makefile.am";
open O, ">Makefile.am.new";
while (<I>)
{
    if (/^libfreecell_solver_la_SOURCES *=/)
    {
        print O "libfreecell_solver_la_SOURCES = " . join(" ", (map { "$_.c" } @objects)) . "\n";
    }
    elsif (/^#<<<HEADERS\.START/)
    {
        while(! /^#>>>HEADERS\.END/)
        {
            $_ = <I>;
        }
        print O "#<<<HEADERS.START\n";
        for my $i (0 .. ((int(scalar(@headers)/5)+((scalar(@headers)%5) > 0))-1))
        {
            print O "EXTRA_DIST += " . join(" ", map { "$_.h" } @headers[($i*5) .. min($i*5+4, $#headers)]) . "\n";
        }
        print O "#>>>HEADERS.END\n";
    }
    else
    {
        print O $_;
    }
}
close(O);
close(I);
rename("Makefile.am.new", "Makefile.am");

open I, "<Makefile.gnu";
open O, ">Makefile.gnu.new";
while(<I>)
{
    if (/^#<<<OBJECTS\.START/)
    {
        while(! /^#>>>OBJECTS\.END/)
        {
            $_ = <I>;
        }
        print O "#<<<OBJECTS.START\n";
        print O "OBJECTS = " . (" " x 20) . "\\\n";
        print O join("", (map { sprintf((" " x 10) . "%-20s\\\n", ($_.".o")) } @objects));
        print O "\n";
        print O "#>>>OBJECTS.END\n";
    }
    else
    {
        print O $_;
    }
}
close(O);
close(I);
rename("Makefile.gnu.new", "Makefile.gnu");

open I, "<Makefile.lite";
open O, ">Makefile.lite.new";
while(<I>)
{
    if (/^INCLUDES *=/)
    {
        print O "INCLUDES = " . join(" ", (map { "$_.h" } @headers)) . "\n";
    }
    elsif (/^#<<<OBJECTS\.START/)
    {
        while(! /^#>>>OBJECTS\.END/)
        {
            $_ = <I>;
        }
        my @ext_objects = (@objects, "main");
        print O "#<<<OBJECTS.START\n";
        print O join("\n\n", (map { "$_.o: $_.c \$(INCLUDES)\n\t\$(CC) -c \$(OFLAGS) -o \$@ \$<" } @ext_objects));
        print O "\n\nOBJECTS = " . join(" ", (map { "$_.o" } @ext_objects)) . "\n";
        print O "#>>>OBJECTS.END\n";
    }
    else
    {
        print O $_;    
    }
}
close(O);
close(I);
rename("Makefile.lite.new", "Makefile.lite");

