package PrepareCommon;

use strict;
use warnings;

use Moose;

has 'depth_dbm' => (is => 'ro', isa => 'Bool', required => 1);

sub main_base
{
    return shift->depth_dbm ? "depth_dbm_solver" : "dbm_solver";
}

sub src_filenames
{
    my ($self) = @_;

    return
    [
        $self->main_base . '.c',
        'alloc_wrap.h',
        'bit_rw.h',
        'bool.h',
        'card.c',
        'count.h',
        'dbm_cache.h',
        'dbm_calc_derived.h',
        'dbm_calc_derived_iface.h',
        'dbm_kaztree.c',
        'dbm_lru_cache.h',
        'dbm_move_to_string.h',
        'dbm_procs.h',
        'dbm_procs_inner.h',
        'dbm_solver.h',
        'dbm_solver_head.h',
        'dbm_solver_key.h',
        'dbm_trace.h',
        'debondt_delta_states_iface.h',
        'delta_states.c',
        'delta_states.h',
        'delta_states_any.h',
        'delta_states_debondt.c',
        'delta_states_debondt.h',
        'delta_states_debondt_impl.h',
        'delta_states_iface.h',
        'depth_dbm_procs.h',
        'dll_thunk.h',
        'fcc_brfs_test.h','dbm_kaztree_compare.h',
        'fcs_dllexport.h',
        'fcs_enums.h',
        'fcs_err.h',
        'fcs_limit.h',
        'fcs_move.h',
        'game_type_limit.h',
        'generic_tree.h',
        'indirect_buffer.h',
        'inline.h',
        'internal_move_struct.h',
        'kaz_tree.h',
        'libavl/rb.c',
        'libavl/rb.h',
        'likely.h',
        'lock.h',
        'meta_alloc.c',
        'meta_alloc.h',
        'min_and_max.h',
        'offloading_queue.h',
        'p2u_rank.h',
        'portable_int32.h',
        'portable_time.h',
        'read_state.h',
        'render_state.h',
        'rinutils.h',
        'state.c',
        'state.h',
        'str_utils.h',
        'typeof_wrap.h',
        'unused.h',
        'var_base_int.h',
        'var_base_reader.h',
        'var_base_writer.h',
    ];
}

sub modules
{
    my $self = shift;

    return
    [
        sort { $a cmp $b }
        $self->main_base . '.o',
        'card.o',
        'dbm_kaztree.o',
        'libavl/rb.o',
        'meta_alloc.o',
        'state.o',
    ];
}
1;
