package FC_Solve::DepthMultiQueueInC;

use strict;
use warnings;

use Config;

use FC_Solve::InlineWrap (
    C => <<'EOF',
#define FCS_DBM_USE_OFFLOADING_QUEUE

#include "depth_multi_queue.h"

typedef struct
{
    fcs_depth_multi_queue_t q;
} QueueInC;

SV* _proto_new(int num_items_per_page, const char * offload_dir_path, int first_depth, int first_item_i) {
        QueueInC * s;
        SV*      obj_ref = newSViv(0);
        SV*      obj = newSVrv(obj_ref, "FC_Solve::DepthMultiQueueInC");
        fcs_offloading_queue_item_t first_item;

        first_item = (fcs_offloading_queue_item_t)first_item_i;

        New(42, s, 1, QueueInC);

        fcs_depth_multi_queue__init(&(s->q), strdup(offload_dir_path), first_depth, &first_item);
        sv_setiv(obj, (IV)s);
        SvREADONLY_on(obj);
        return obj_ref;
}

static GCC_INLINE QueueInC * deref(SV * const obj) {
    return (QueueInC*)SvIV(SvRV(obj));
}

static GCC_INLINE fcs_depth_multi_queue_t * q(SV * const obj) {
    return &(deref(obj)->q);
}

void insert(SV* obj, int depth, int item_i) {
    fcs_offloading_queue_item_t item = (fcs_offloading_queue_item_t)item_i;

    fcs_depth_multi_queue__insert( q(obj), depth, &item );

    return;
}

AV* extract_proto(SV* obj) {
    fcs_offloading_queue_item_t item;
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
    return q(obj)->num_inserted;
}

long get_num_items_in_queue(SV* obj) {
    return q(obj)->num_items_in_queue;
}

long get_num_extracted(SV* obj) {
    return q(obj)->num_extracted;
}

void DESTROY(SV* obj) {
  QueueInC * s = deref(obj);
  free(s->q.offload_dir_path);
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

