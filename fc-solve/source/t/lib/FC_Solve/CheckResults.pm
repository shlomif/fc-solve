package FC_Solve::CheckResults;

use strict;
use warnings;

use parent 'Games::Solitaire::Verify::Base';

use YAML::XS qw(DumpFile LoadFile);
use FC_Solve::Paths qw( is_freecell_only is_without_flares );

use Carp ();

use Test::More;
use FC_Solve::ShaAndLen ();
use FC_Solve::GetOutput ();

__PACKAGE__->mk_acc_ref(
    [
        qw(
            digests_storage_fn
            digests_storage
            trim_stats
            )
    ]
);

sub _init
{
    my ( $self, $args ) = @_;
    $self->digests_storage_fn( $args->{data_filename} );
    $self->digests_storage( LoadFile( $self->digests_storage_fn() ) );
    $self->trim_stats( $args->{trim_stats} );
    return;
}

sub end
{
    my $self = shift;

    # Make sure we do it only once.
    if ( defined( $self->digests_storage_fn() ) )
    {
        DumpFile( $self->digests_storage_fn(), $self->digests_storage() );
        $self->digests_storage_fn( undef() );
    }
}

sub should_fill_in_id
{
    my ( $self, $id ) = @_;

    return (
        exists( $self->digests_storage->{digests}->{$id} )
        ? ( ( $ENV{'FCS_DIGESTS_REPLACE_IDS'} || "" ) =~ m{\Q,$id,\E} )
        : ( ( $ENV{'FCS_DIGESTS_FILL_IDS'} || "" ) =~ m{\Q,$id,\E} )
    );
}

# Short for a test to verify a solution.
sub vtest
{
    my ( $self, $args, $msg ) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    if ( exists( $args->{variant} ) and is_freecell_only() )
    {
        return ok( 1,
q#Test skipped because it's a non-Freecell variant on a Freecell-only build.#
        );
    }
    if ( $args->{with_flares} and is_without_flares() )
    {
        return ok( 1,
q#Test skipped because it uses flares, and we are running on a build without flares.#
        );
    }

    my $id = $args->{id};
    if ( !$id )
    {
        Carp::confess 'ID not specified';
    }
    my $output_file      = $args->{output_file};
    my $complete_command = $args->{complete_command};

    my $fc_solve_output;
    if ($complete_command)
    {
        open $fc_solve_output, "$complete_command |"
            or Carp::confess
            'Error! Could not open the complete command pipeline';
    }
    elsif ( !$output_file )
    {
        $fc_solve_output = FC_Solve::GetOutput->new($args)->open_cmd_line->{fh};
    }
    else
    {
        local $args->{theme} =
            [ '-o', $output_file, @{ $args->{theme} || [qw(-l gi)] } ];
        my @cmd = FC_Solve::GetOutput->new($args)->calc_cmd_line->{cmd_line};
        if ( system(@cmd) )
        {
            Carp::confess 'Error: could not execute the fc-solve pipeline.';
        }
        open $fc_solve_output, "<", $output_file
            or Carp::confess("Could not open file for reading - $!");
    }

    my $sha = FC_Solve::ShaAndLen->new();

    if ( $ENV{'FCS_DUMP_SOLS'} )
    {
        open my $out, '>', "$id.SOLUTION.txt"
            or die "Cannot open '$id.txt' for writing";

        local $/;
        print {$out} <$fc_solve_output>;

        close($out);

        return ok( 1, $msg );
    }
    $sha->add_processed_slurp(
        $fc_solve_output,
        sub {
            my $s = shift;
            if ( $self->trim_stats )
            {
                $s =~ s/^(This game is solveable\.\n).*/$1/ms;
            }
            $s =~ s/ +(\n)/$1/g;
            return $s;
        }
    );

    close($fc_solve_output);

    if ($output_file)
    {
        unlink($output_file);
    }

    if ( $self->should_fill_in_id($id) )
    {
        $self->digests_storage->{digests}->{$id} = {
            'len'       => $sha->len(),
            'hexdigest' => $sha->hexdigest(),
        };
    }
    my $info = $self->digests_storage->{digests}->{$id};

    my $test_verdict = ok(
        ( $sha->hexdigest() eq $info->{'hexdigest'} )
            && ( $sha->len() eq $info->{'len'} ),
        $msg
    );

    if ( !$test_verdict )
    {
        diag(<<"EOF");
Expected Digest: $info->{hexdigest}
Got Digest: @{[$sha->hexdigest()]}
Expected Len: $info->{len}
Got Len: @{[$sha->len()]}
EOF
    }

    close($fc_solve_output);

    return $test_verdict;
}

1;
