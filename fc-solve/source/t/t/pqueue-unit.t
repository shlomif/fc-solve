#!/usr/bin/perl

use strict;
use warnings;
use lib './t/lib';

use Test::More tests => 1;

use Test::Differences (qw( eq_or_diff ));

package PQ;

use FC_Solve::InlineWrap (
    C => <<"EOF",
#include "pqueue.h"

typedef struct
{
    PQUEUE pq;
} PqInC;

SV* _proto_new() {
        PqInC * s;
        SV*      obj_ref = newSViv(0);
        SV*      obj = newSVrv(obj_ref, "PQ");
        New(42, s, 1, PqInC);

        fc_solve_pq_init(&(s->pq), 500);
        sv_setiv(obj, (IV)s);
        SvREADONLY_on(obj);
        return obj_ref;
}

static GCC_INLINE PqInC * deref(SV * const obj) {
    return (PqInC*)SvIV(SvRV(obj));
}

static GCC_INLINE PQUEUE * q(SV * const obj) {
    return &(deref(obj)->pq);
}

int is_empty(SV * obj) {
    return fc_solve_is_pqueue_empty(q(obj));
}

EOF
    LIBS => "-L" . $ENV{FCS_PATH} . " ",
);

sub new
{
    return PQ::_proto_new();
}

package main;

{
    my $pq = PQ->new;

    # TEST
    ok (scalar($pq->is_empty()), "PQ is empty upon init.");
}
