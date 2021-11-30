#!/usr/bin/perl

use strict;
use warnings;

use FC_Solve::Paths qw( FCS_STATE_STORAGE_INTERNAL_HASH is_rcs_states );

use Test::More;
use Test::Differences qw/ eq_or_diff /;

BEGIN
{
    if ( is_rcs_states() )
    {
        plan skip_all => "rcs_states";
    }
    elsif ( !FCS_STATE_STORAGE_INTERNAL_HASH() )
    {
        plan skip_all => "incompatible state storage";
    }
    else
    {
        plan tests => 125;
    }
}

package FcsHashTable;

use FC_Solve::InlineWrap (
    C => <<"EOF",
#include "fcs_hash__insert.h"
#if ((defined(INDIRECT_STACK_STATES) &&                                  \\
         (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH)) ||        \\
     (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH))

#include "wrap_xxhash.h"
#endif

#include "meta_alloc.c"

typedef struct
{
    meta_allocator meta_alloc;
    hash_table ht;
} HashTableInC;

SV* _proto_new() {
        HashTableInC * s;
        SV*      obj_ref = newSViv(0);
        SV*      obj = newSVrv(obj_ref, "FcsHashTable");
        New(42, s, 1, HashTableInC);

 fc_solve_meta_compact_allocator_init(
    &(s->meta_alloc));
        fc_solve_hash_init(&(s->meta_alloc), &(s->ht),
#ifdef FCS_INLINED_HASH_COMPARISON
    FCS_INLINED_HASH__STATES
#else
#ifdef FCS_WITH_CONTEXT_VARIABLE
        fc_solve_state_compare_with_context,

        NULL
#else
        fc_solve_state_compare
#endif
#endif
        );
        sv_setiv(obj, (IV)s);
        SvREADONLY_on(obj);
        return obj_ref;
}

static inline HashTableInC * deref(SV * const obj) {
    return (HashTableInC*)SvIV(SvRV(obj));
}

static inline hash_table * q(SV * const obj) {
    return &(deref(obj)->ht);
}

typedef long tok;

int insert(SV * obj, long token) {
fcs_kv_state *const new_state = SMALLOC1(new_state);
#define new_state_key (new_state->key)
new_state_key = SMALLOC1(new_state_key);

memset(new_state_key, '\\0', sizeof(*new_state_key));
*(tok*)new_state_key = token;
    #define EXISTS 1
    #define ADDED 0
#ifdef FCS_RCS_STATES
#define FCS_MY_STATE new_state->val, new_state->key
#else
#define FCS_MY_STATE FCS_STATE_kv_to_collectible(new_state)
#endif
    void * ret = fc_solve_hash_insert(q(obj), FCS_MY_STATE,
    DO_XXH(new_state_key, sizeof(*new_state_key)));
    if (ret)
    {
        fcs_kv_state existing_state;
        fcs_kv_state *const existing_state_raw = &existing_state;
        FCS_STATE_collectible_to_kv(existing_state_raw, ret);
        assert(
!memcmp(new_state_key, existing_state_raw->key, sizeof(*new_state_key)));
    }
    return (ret ? EXISTS : ADDED);
}

EOF
);

sub new
{
    return FcsHashTable::_proto_new();
}

package OneHash;

use Test::More;

sub new
{
    my $class = shift;

    my $self = bless {}, $class;

    $self->_init(@_);

    return $self;
}

sub _init
{
    my ( $self, $args ) = @_;

    $self->{'c'}    = FcsHashTable->new;
    $self->{'perl'} = +{};

    return;
}

sub insert
{
    my ( $self, $token ) = @_;

    my $r1 = $self->{'c'}->insert($token);
    my $r2 = exists( $self->{'perl'}->{$token} );

    local $Test::Builder::Level = $Test::Builder::Level + 1;
    my $ret = is( ( !$r1 ), ( !$r2 ), "same" );
    $self->{'perl'}->{$token} = 1;
    return $ret;
}

package MultiHash;

sub new
{
    my $class = shift;

    my $self = bless {}, $class;

    $self->_init(@_);

    return $self;
}

sub _init
{
    my ( $self, $args ) = @_;

    $self->{'h'} = +{ map { $_ => OneHash->new } @{ $args->{names} } };

    return;
}

sub insert
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;
    my ( $self, $name, $token ) = @_;
    if ( ref($name) )
    {
        ( $name, $token ) = @$name;
    }

    return $self->{'h'}->{$name}->insert($token);
}

1;

package main;

{
    my $ht = FcsHashTable->new;

    # TEST
    ok( scalar( not $ht->insert(24) ), 'insert new' );

    # TEST
    ok( scalar( $ht->insert(24) ), 'insert old exists' );

    # TEST
    ok( scalar( not $ht->insert(4) ), 'insert new' );

    # TEST
    ok( scalar( $ht->insert(4) ), 'insert old exists' );

    # TEST
    ok( scalar( $ht->insert(24) ), 'insert old exists' );
}

{
    my $h = MultiHash->new( { names => [qw/ one two /] } );

    srand(8);
    foreach my $token (
        ( map { [ 'one', $_ ] } 1 .. 5, reverse( 1 .. 5 ), ),
        ( map { [ 'two', $_ ] } 1 .. 5, reverse( 1 .. 5 ), ),
        ( map { [ 'one', int( rand(20) ), ] } 1 .. 100 ),
        )
    {
        # TEST*(10+10+100)
        $h->insert($token);
    }
}
