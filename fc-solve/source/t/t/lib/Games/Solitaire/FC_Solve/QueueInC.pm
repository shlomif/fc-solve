package Games::Solitaire::FC_Solve::QueueInC;

use strict;
use warnings;

use Config;

use Inline (
    C => <<'EOF',
#define FCS_DBM_USE_OFFLOADING_QUEUE

#include "offloading_queue.h"

typedef struct
{
    fcs_offloading_queue_t q;
} QueueInC;

SV* _proto_new(int num_items_per_page, const char * offload_dir_path) {
        QueueInC * s;
        SV*      obj_ref = newSViv(0);
        SV*      obj = newSVrv(obj_ref, "Games::Solitaire::FC_Solve::QueueInC");

        New(42, s, 1, QueueInC);

        fcs_offloading_queue__init(&(s->q), num_items_per_page, strdup(offload_dir_path), 0);
        sv_setiv(obj, (IV)s);
        SvREADONLY_on(obj);
        return obj_ref;
}

#define QUEUE_PTR() (&(((QueueInC*)SvIV(SvRV(obj)))->q))
#
void insert(SV* obj, int item_i) {
    fcs_offloading_queue_item_t item;

    item = (fcs_offloading_queue_item_t)item_i;
    fcs_offloading_queue__insert(
        QUEUE_PTR(), &item);

    return;
}

SV* extract(SV* obj) {
    fcs_bool_t is_valid;
    fcs_offloading_queue_item_t item;
    SV *ret;

    is_valid = fcs_offloading_queue__extract(QUEUE_PTR(), &item);

    if (is_valid)
    {
        ret = newSViv((int)item);
    }
    else
    {
        ret = &PL_sv_undef;
    }

    return ret;
}

long get_num_inserted(SV* obj) {
    return QUEUE_PTR()->num_inserted;
}

long get_num_items_in_queue(SV* obj) {
    return QUEUE_PTR()->num_items_in_queue;
}

long get_num_extracted(SV* obj) {
    return QUEUE_PTR()->num_extracted;
}

void DESTROY(SV* obj) {
  QueueInC * s = (QueueInC*)SvIV(SvRV(obj));
  free(s->q.offload_dir_path);
  fcs_offloading_queue__destroy(&s->q);
  Safefree(s);
}

EOF
    CLEAN_AFTER_BUILD => 0,
    INC => "-I" . $ENV{FCS_PATH},
    LIBS => "-L" . $ENV{FCS_PATH},
    CCFLAGS => "$Config{ccflags} -std=gnu99"
);

sub new
{
    my ($class, $args) = @_;

    return Games::Solitaire::FC_Solve::QueueInC::_proto_new($args->{num_items_per_page}, $args->{offload_dir_path});
}

1;

