#!/usr/bin/perl -w

use strict;
use Data::Dumper;

my $other_dir = shift;

my (@scans);

sub get_dir
{
    my $source = shift;
    
    return ($source eq "this") ? "./" : "$other_dir/";
}

sub get_fn
{
    my $source = shift;
    my $id = shift;
    return get_dir($source)."/data/" . $id . ".data.bin";
}

sub file_len
{
    my $filename = shift;

    my @data = stat($filename);

    return @data ? $data[7] : 0;
}

foreach my $source ('this', 'other')
{
    my $dir = get_dir($source);
    
    open I, "<$dir/scans.txt";
    while (my $line = <I>)
    {
        chomp($line);
        my ($id, $cmd_line) = split(/\t/, $line);
        push @scans, { 'src' => $source, 'id' => $id, 'cmd' => $cmd_line};
    }
    close(I);
}

{
    my $d = Data::Dumper->new([ \@scans], [ "\$scans"]);
    print $d->Dump();
}

foreach my $scan (grep { ($_->{'src'} eq "other") && (-e get_fn($_->{'src'}, $_->{'id'})) } @scans)
{
    my @src_scan = (grep { ($_->{'src'} eq "this") && ($_->{'cmd'} eq $scan->{'cmd'}) } @scans);
    if ((&file_len(&get_fn("other", $scan->{'id'})) == (3+32000)*4))
    {
        if (scalar(@src_scan) == 0)
        {
            print "I need to add:\n";
            my $d = Data::Dumper->new([ $scan], [ "\$scan"]);
            print $d->Dump();

            open I, "<next-id.txt";
            my $new_id = <I>;
            close(I);
            open O, ">next-id.txt";
            print O ($new_id+1);
            close(O);

            open O, ">>scans.txt";
            print O "$new_id\t" . $scan->{'cmd'} . "\n";
            close(O);

            system("cp", get_fn("other", $scan->{'id'}), get_fn("this", $new_id));
        }
        elsif (&file_len(&get_fn("this", $src_scan[0]->{'id'})) < (3+32000)*4)
        {
            print "I need to put the file of:\n";
            my $d = Data::Dumper->new([ $scan], [ "\$scan"]);
            print $d->Dump();

            system("cp", get_fn("other", $scan->{'id'}), get_fn("this", $src_scan[0]->{'id'}));
            
        }
    }
    
}



