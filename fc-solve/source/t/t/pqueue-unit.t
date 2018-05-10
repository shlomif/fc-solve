#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 12;
use Test::Differences qw/ eq_or_diff /;

package PQ;

use FC_Solve::InlineWrap (
    C => <<"EOF",
#include "pqueue.h"

typedef struct
{
    pri_queue pq;
} PqInC;

SV* _proto_new() {
        PqInC * s;
        SV*      obj_ref = newSViv(0);
        SV*      obj = newSVrv(obj_ref, "PQ");
        New(42, s, 1, PqInC);

        fc_solve_pq_init(&(s->pq));
        sv_setiv(obj, (IV)s);
        SvREADONLY_on(obj);
        return obj_ref;
}

static inline PqInC * deref(SV * const obj) {
    return (PqInC*)SvIV(SvRV(obj));
}

static inline pri_queue * q(SV * const obj) {
    return &(deref(obj)->pq);
}

int is_empty(SV * obj) {
    return fc_solve_is_pqueue_empty(q(obj));
}

void push(SV * obj, char * val, int rating) {
    SV * const sv = newSVpv(val, 0);

    fc_solve_pq_push(q(obj), (fcs_collectible_state *)sv, rating);
}

SV * pop(SV * obj) {
    fcs_collectible_state * val;

    fc_solve_pq_pop(q(obj), &val);

    if (!val)
    {
        return &PL_sv_undef;
    }
    else
    {
        return (SV *)val;
    }
}

EOF
);

sub new
{
    return PQ::_proto_new();
}

package main;

{
    my $pq = PQ->new;

    # TEST
    ok( scalar( $pq->is_empty() ), "PQ is empty upon init." );

    $pq->push( "Hello", 24 );

    # TEST
    ok( scalar( !$pq->is_empty() ), "PQ is not empty upon insertion." );

    # TEST
    is( scalar( $pq->pop() ), "Hello", "Received the right item after pop." );

    # TEST
    ok( scalar( $pq->is_empty() ), "PQ is empty again after popping." );

    # TEST
    ok( !defined( $pq->pop() ), "Item is null." );
}

{
    my $pq = PQ->new;

    # TEST
    ok( !defined( $pq->pop() ), "Popped item is null upon empty" );
}

{
    my $pq = PQ->new;

    $pq->push( "ShouldBeFirst",  1000 );
    $pq->push( "ShouldBeSecond", 1 );

    # TEST
    is( scalar( $pq->pop() ),
        "ShouldBeFirst", "First string out on in-order insert." );

    # TEST
    is( scalar( $pq->pop() ),
        "ShouldBeSecond", "Second string out on in-order insert." );

    # TEST
    ok( scalar( $pq->is_empty() ), "PQ is empty again after popping." );
}

{
    my $pq = PQ->new;

    $pq->push( "ShouldBeSecond", 1 );
    $pq->push( "ShouldBeFirst",  1000 );

    # TEST
    is( scalar( $pq->pop() ),
        "ShouldBeFirst", "First string out on reverse-order insert." );

    # TEST
    is( scalar( $pq->pop() ),
        "ShouldBeSecond", "Second string out on reverse-order insert." );

    # TEST
    ok( scalar( $pq->is_empty() ), "PQ is empty again after popping." );
}
