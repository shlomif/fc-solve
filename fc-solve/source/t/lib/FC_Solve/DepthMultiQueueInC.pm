package FC_Solve::DepthMultiQueueInC;

use strict;
use warnings;

use FC_Solve::InlineWrap (
    C => <<'EOF',
#define FCS_DBM_USE_OFFLOADING_QUEUE

#include "depth_multi_queue.h"

typedef struct
{
    fcs_depth_multi_queue q;
} QueueInC;

SV* _proto_new(int num_items_per_page, const char * offload_dir_path, int first_depth, int first_item_i) {
        SV*      obj_ref = newSViv(0);
        SV*      obj = newSVrv(obj_ref, "FC_Solve::DepthMultiQueueInC");

        QueueInC * s;
        New(42, s, 1, QueueInC);

        const_AUTO(first_item, (offloading_queue_item)first_item_i);
        fcs_depth_multi_queue__init(&(s->q), savepv(offload_dir_path), first_depth, &first_item);
        sv_setiv(obj, (IV)s);
        SvREADONLY_on(obj);
        return obj_ref;
}

static inline QueueInC * deref(SV * const obj) {
    return (QueueInC*)SvIV(SvRV(obj));
}

static inline fcs_depth_multi_queue * q(SV * const obj) {
    return &(deref(obj)->q);
}

void insert(SV* obj, int depth, int item_i) {
    const_AUTO(item, (offloading_queue_item)item_i);
    fcs_depth_multi_queue__insert( q(obj), depth, &item );
}

AV* extract_proto(SV* obj) {
    offloading_queue_item item;
    int ret_depth;
    AV * const ret = newAV();

    if (fcs_depth_multi_queue__extract(q(obj), &ret_depth, &item))
    {
        av_push(ret, newSViv(ret_depth));
        av_push(ret, newSViv((int)item));
    }

    return ret;
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
  fcs_depth_multi_queue__destroy(&(s->q));
  Safefree(s);
}

EOF
);

sub new
{
    my ( $class, $args ) = @_;

    return FC_Solve::DepthMultiQueueInC::_proto_new(
        $args->{num_items_per_page}, $args->{offload_dir_path},
        $args->{first_depth},        $args->{first_item},
    );
}

sub extract
{
    my $self = shift;

    return @{ $self->extract_proto() };

    return;
}

1;
