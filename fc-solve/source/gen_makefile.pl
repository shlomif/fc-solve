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
    qw(alloc app_str caas card cl_chop cmd_line fcs_dm fcs_hash fcs_isa), 
    qw(freecell intrface lib lookup2 move pqueue preset rand), 
    qw(scans simpsim state)
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

my @headers_proto=
    (
        qw(alloc app_str caas card cl_chop), { 'name' => "config", 'gen' => 1}, 
            qw(ds_order fcs_cl fcs fcs_dm fcs_enums),
        qw(fcs_hash fcs_isa fcs_move fcs_user inline jhjtypes lookup2), 
        qw(move ms_ca), {'name' => "prefix", 'gen' => 1}, 
            qw(pqueue preset rand state test_arr tests)
    );

my @headers = (map { ref($_) eq "HASH" ? $_->{'name'} : $_ } @headers_proto);

my @defines=(qw(WIN32));

my @debug_defines = (qw(DEBUG));

open O, ">Makefile.win32";

print O "all: fc-solve.exe freecell-solver-range-parallel-solve.exe\r\n\r\n";

print O "OFLAGS=" . "/Og2 " . join(" ", (map { "/D".$_ } @defines)) . "\r\n";
print O "DFLAGS=\$(OFLAGS) " . join(" ", (map { "/D".$_ } @debug_defines)) . "\r\n";

print O "INCLUDES=" . join(" ", (map { $_.".h" } @headers)). "\r\n";
print O "CC=cl\r\n";
print O "LIB32=link.exe\r\n";


print O "\r\n\r\n";

print O "OBJECTS = " . join(" ", (map { $_.".obj" } @objects)) . "\r\n";

print O "\r\n\r\n";

foreach my $o (@objects, (map { @{$_->{'objs'}} } @targets))
{
    print O "$o.obj: $o.c \$(INCLUDES)\r\n";
    print O "\t\$(CC) /c /Fo$o.obj \$(OFLAGS) $o.c\r\n";
    print O "\r\n";
}

print O "\r\n\r\n###\r\n### Final Targets\r\n###\r\n\r\n\r\n";

foreach my $t (@targets)
{
    my $exe = $t->{'exe'};
    my @objs = @{$t->{'objs'}};

    #my $obj_line = "\$(OBJECTS) " . join(" ", (map { "$_.obj" } @objs));
    my $obj_line = "freecell-solver-static.lib " . join(" ", (map { "$_.obj" } @objs));

    print O "$exe.exe: $obj_line\r\n";
    print O "\t\$(CC) /Fe$exe.exe /F0x2000000 $obj_line\r\n";
    print O "\r\n";
}

print O "freecell-solver-static.lib: \$(OBJECTS)\r\n";
print O "\t\$(LIB32) -lib \$(OBJECTS) /out:freecell-solver-static.lib\r\n";
print O "\r\n";

print O "freecell-solver.dll: \$(OBJECTS) freecell-solver.def\r\n";
print O "\t\$(LIB32) kernel32.lib user32.lib gdi32.lib /dll /out:freecell-solver.dll /implib:freeecell-solver.lib /DEF:freecell-solver.def \$(OBJECTS) \r\n";
print O "\r\n";

#print O "fc-solve.exe: \$(OBJECTS)\r\n";
#print O "\t\$(CC) /Fefc-solve.exe /F0x2000000 \$(OBJECTS)\r\n";
#print O "\r\n";

print O "clean:\r\n";
print O "\tdel *.obj *.exe *.lib *.dll *.exp\r\n";

close(O);

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
        my @headers_no_gen = map { (ref($_) eq "HASH") ? $_->{'name'} : $_ } (grep { (ref($_) eq "HASH") ? (! $_->{'gen'}) : 1 } @headers_proto);
        for my $i (0 .. ((int(scalar(@headers_no_gen)/5)+((scalar(@headers_no_gen)%5) > 0))-1))
        {
            print O "EXTRA_DIST += " . join(" ", map { "$_.h" } @headers_no_gen[($i*5) .. min($i*5+4, $#headers_no_gen)]) . "\n";
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

