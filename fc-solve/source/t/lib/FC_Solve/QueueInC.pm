package FC_Solve::QueueInC;

use strict;
use warnings;

use FC_Solve::InlineWrap (
    C => <<'EOF',
#define FCS_DBM_USE_OFFLOADING_QUEUE

#include "offloading_queue.h"

typedef struct
{
    fcs_offloading_queue q;
} QueueInC;

SV* _proto_new(int num_items_per_page, const char * offload_dir_path, long queue_id) {
        QueueInC * s;

        New(42, s, 1, QueueInC);

        fcs_offloading_queue__init(&(s->q), savepv(offload_dir_path), queue_id);
        SV*      obj_ref = newSViv(0);
        SV*      obj = newSVrv(obj_ref, "FC_Solve::QueueInC");
        sv_setiv(obj, (IV)s);
        SvREADONLY_on(obj);
        return obj_ref;
}

static inline QueueInC * deref(SV * const obj) {
    return (QueueInC*)SvIV(SvRV(obj));
}

static inline fcs_offloading_queue * q(SV * const obj) {
    return &(deref(obj)->q);
}

void insert(SV* obj, int item_i) {
    offloading_queue_item item = (offloading_queue_item)item_i;
    fcs_offloading_queue__insert(q(obj), &item);
}

SV* extract(SV* obj) {
    offloading_queue_item item;

    return (fcs_offloading_queue__extract(q(obj), &item))
    ? newSViv((int)item)
    : &PL_sv_undef;
}

long get_num_inserted(SV* obj) {
    return q(obj)->stats.num_inserted;
}

long get_num_items_in_queue(SV* obj) {
    return q(obj)->stats.num_items_in_queue;
}

long get_num_extracted(SV* obj) {
    return q(obj)->stats.num_extracted;
}

void DESTROY(SV* obj) {
  QueueInC * s = deref(obj);
  Safefree(s->q.offload_dir_path);
  fcs_offloading_queue__destroy(&s->q);
  Safefree(s);
}

EOF
);

sub new
{
    my ( $class, $args ) = @_;

    return FC_Solve::QueueInC::_proto_new(
        $args->{num_items_per_page},
        $args->{offload_dir_path},
        ( $args->{queue_id} || 0 )
    );
}

1;
