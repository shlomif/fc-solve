CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

INCLUDE ("${CMAKE_SOURCE_DIR}/cmake/shlomif_common_bootstrap.cmake")
SHLOMIF_COMMON_SETUP("${private_mod_path}")

CMAKE_POLICY(SET CMP0054 NEW)

SET (PKGDATADIR_SUBDIR "freecell-solver")
SET (RELATIVE_PKGDATADIR "${RELATIVE_DATADIR}/${PKGDATADIR_SUBDIR}")
SET (PKGDATADIR "${DATADIR}/${PKGDATADIR_SUBDIR}")

# Introduces VERSION , CPACK_PACKAGE_VERSION_MAJOR,
# CPACK_PACKAGE_VERSION_MAJOR, and CPACK_PACKAGE_VERSION_PATCH
READ_VERSION_FROM_VER_TXT()

# This is the equivalent to perform a "make dist"/"make distdir" etc.
SET(CPACK_PACKAGE_NAME "freecell-solver")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Freecell Solver")
SET(CPACK_PACKAGE_VENDOR "Shlomi Fish")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.asciidoc")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.asciidoc")

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

SET (base_with_ver "freecell-solver-[0-9]+\\\\.[0-9]+\\\\.[0-9]+")

SET(CPACK_SOURCE_IGNORE_FILES
    "/.deps/"
    "~$"
    "\\\\.pyc$"
    "/tags$"
    "fcs-freecell-24\\\\.txt$"
    "^${PROJECT_SOURCE_DIR}/ids\\\\.txt$"
    "^${PROJECT_SOURCE_DIR}/scripts/ids-whitelist(\\\\.txt)?"
    "/(build|BUILD|B)/"
    "/Presets/testing-presets"
    "^${PROJECT_SOURCE_DIR}/patsolve/(\\\\.(git)|appveyor|\\\\.travis|CI-testing)"
    "^${PROJECT_SOURCE_DIR}/patsolve/patsolve/(OLD|Rejects|cmake|ga|patsolve-specific|t/)"
    "^${PROJECT_SOURCE_DIR}/patsolve/patsolve/(CMakeLists\\\\.txt|COPYING|Manifest|Parameters|check_layout|config\\\\.h\\\\.in|convert_[a-z]+|fc-solve\\\\.ico|dondorf-king\\\\.bmp|threaded_main\\\\.c|patmain\\\\.c|run-tests\\\\.pl|read_layout\\\\.h|print_layout\\\\.h)$"
    "^${PROJECT_SOURCE_DIR}/xxHash-wrapper/(\\\\.git)"
)

SET(_win32_static_lib_flags )
IF(WIN32 AND NOT UNIX)
    # There is a bug in NSI that does not handle full unix paths properly. Make
    # sure there is at least one set of four (4) backlasshes.
    SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\dondorf-king.bmp")
    SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\fc-solve.exe")
    SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\fc-solve.shlomifish.org")
    SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\fc-solve.shlomifish.org")
    SET(CPACK_NSIS_DISPLAY_NAME "Freecell Solver")
    SET(CPACK_NSIS_CONTACT "shlomif@shlomifish.org")
    SET(CPACK_NSIS_MODIFY_PATH ON)
    # Setting for NSIS :
    SET(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\fc-solve.ico")
    SET(CPACK_NSIS_MUI_UNIICON ${CPACK_NSIS_MUI_ICON})
    SET(CPACK_PACKAGE_ICON ${CPACK_NSIS_MUI_ICON})
    SET(CPACK_NSIS_MODIFY_PATH "ON")
    IF (${CMAKE_COMPILER_IS_GNUCC})
        IF (CMAKE_BUILD_TYPE STREQUAL release)
            SET (_win32_static_lib_flags "-static-libgcc" "-static-libstdc++")
        ENDIF ()
    ENDIF ()
ELSE()
  SET(CPACK_STRIP_FILES "fc-solve")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF()

    # See:
    # https://stackoverflow.com/questions/4702732/the-program-cant-start-because-libgcc-s-dw2-1-dll-is-missing#4703059
SET(CPACK_PACKAGE_EXECUTABLES
    "fc-solve" "Single-Game Solver"
)

### This is to set the RPATH correctly, so when installed under a prefix
### the executables will find the libraries.
###
### See:
###
### http://www.cmake.org/Wiki/CMake_RPATH_handling
###
### (Taken from that wiki page)

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

SET (STATES_TYPE "INDIRECT_STACK_STATES" CACHE STRING
    "States Type ('INDIRECT_STACK_STATES', or 'COMPACT_STATES'). No need to usually change.")
option (FCS_ENABLE_RCS_STATES "Whether to use RCS-like states (requires a STATES_TYPE of COMPACT_STATES")
option (FCS_ENABLE_DBM_SOLVER "Whether to build the DBM solver")
SET (FCS_DBM_BACKEND "kaztree" CACHE STRING "Type of DBM backend.")
SET (FCS_DBM_FREECELLS_NUM 2 CACHE STRING "Number of Freecells for the DBM* solvers")
SET (FCS_DBM_TREE_BACKEND "libavl2" CACHE STRING "Type of DBM tree backend.")
SET (FCS_IA_PACK_SIZE 64 CACHE STRING "Size of a single pack in kilo-bytes.")
SET (FCS_MAX_RANK 13 CACHE STRING "Maximal rank - should be 13")
SET (MAX_NUM_FREECELLS 8 CACHE STRING "Maximal Number of Freecells")
SET (MAX_NUM_STACKS 10 CACHE STRING "Maximal Number of Stacks")
SET (MAX_NUM_INITIAL_CARDS_IN_A_STACK 8 CACHE STRING
    "Maximal Number of Initial Cards in a Stack")
SET (MAX_NUM_DECKS 2 CACHE STRING "Maximal Number of Decks (1 or 2)")
SET (MAX_NUM_BENCHMARK_THREADS 4 CACHE STRING "Maximal Number of Threads to Use When Benchmarking")
SET (FCS_STATE_STORAGE "FCS_STATE_STORAGE_INTERNAL_HASH" CACHE STRING "The State Storage Type")
SET (FCS_STACK_STORAGE "FCS_STACK_STORAGE_INTERNAL_HASH" CACHE STRING "The Stack Storage Type")
SET (FCS_RCS_CACHE_STORAGE "FCS_RCS_CACHE_STORAGE_KAZ_TREE" CACHE STRING "The LRU Cache Type of for FCS_RCS_STATES.")
SET (FCS_WHICH_COLUMNS_GOOGLE_HASH "FCS_WHICH_COLUMNS_GOOGLE_HASH__SPARSE" CACHE STRING "The Columns' Google Hash Type")
SET (FCS_WHICH_STATES_GOOGLE_HASH "FCS_WHICH_STATES_GOOGLE_HASH__SPARSE" CACHE STRING "The States/Positions' Google Hash Type")
option (FCS_FREECELL_ONLY "Configure Freecell Solver to only be able to solve Freecell (not recommended)")
option (FCS_DISABLE_SIMPLE_SIMON "Exclude being able to solve Simple Simon from the Binary (not recommended)")
option (FCS_WITH_TEST_SUITE "Also build and run the test suite." ON)
option (FCS_LINK_TO_STATIC "Link to the static library.")
SET (FCS_HARD_CODED_NUM_FCS_FOR_FREECELL_ONLY "4" CACHE STRING "The hard-coded number of freecells (4, 2, etc.). Usually ignored")
option (FCS_ENABLE_SECONDARY_HASH_VALUE "Enable the secondary hash value calculation and track-keeping. Seems to slow things down.")
option (FCS_WITH_CONTEXT_VARIABLE "Enable the context (extra void *) as passed to the comparison functions, etc." ON)
option (FCS_INLINED_HASH_COMPARISON "Use more compact tokens for moves internally (makes things slower)" ON)
option (FCS_AVOID_TCMALLOC "Avoid linking against Google's tcmalloc")
option (FCS_BUILD_DOCS "Whether to build the documentation or not." ON)
option (BUILD_STATIC_LIBRARY "Whether to build the static library (which takes more time)" ON)
SET (CPU_ARCH "" CACHE STRING "A specific CPU architecture to pass to gcc")
SET (OPTIMIZATION_OMIT_FRAME_POINTER "" CACHE STRING "Use -fomit-frame-pointer with gcc (not recommended for debugging)")
SET (FCS_STATE_STORAGE_LIBAVL2_TREE_TYPE "avl" CACHE STRING "The libavl2 tree type used for the state storage. Ignored if not set to libavl2")
SET (FCS_STACK_STORAGE_LIBAVL2_TREE_TYPE "avl" CACHE STRING "The libavl2 tree type used for the stack storage. Ignored if not set to libavl2")
SET (LIBAVL2_SOURCE_DIR "/usr/src/avl-2.0.3" CACHE STRING "The location of the libavl2 source tree (from which files are copied)")
SET (LEVELDB_SOURCE_DIR "/home/shlomif/Download/unpack/prog/leveldb/leveldb" CACHE STRING "The location of the LevelDB sources.")
option (FCS_WITHOUT_VISITED_ITER "Disable the visited_iter counter in each state (somewhat useful for debugging, otherwise not needed.)")
option (FCS_WITHOUT_DEPTH_FIELD "Disable the depth field in each state (not absolutely necessary.)")
option (FCS_WITHOUT_LOCS_FIELDS "Does not do anything (kept for backwards-compatibility)")
option (FCS_UNSAFE "Disable safety checks. Enable at your own risk!")
option (FCS_TRACE_MEM "Enable memory tracing in fc-solve.")
SET (FCS_MAX_NUM_SCANS_BUCKETS "" CACHE STRING "The number of scan_visited buckets in fc-solve (safe to ignore).")
option (FCS_DISABLE_PATSOLVE "Disable the patsolve code")
SET (FCS_MAX_IDENT_LEN "10" CACHE STRING "Maximal length of identifiers")
option (FCS_SINGLE_HARD_THREAD "Enable single-hard-thread-per-instance (an optimization)")
option (FCS_DISABLE_DEBONDT_DELTA_STATES "Disable the DEBONDT delta states")
option (FCS_BREAK_BACKWARD_COMPAT_1 "Break some backward compatibility in the library and input/output")
option (FCS_BREAK_BACKWARD_COMPAT_2 "Break some backward compatibility in the generated solutions while improving performance")
option (FCS_WITHOUT_FC_PRO_MOVES_COUNT "Don't include the code and functionality of FC-Pro moves count.")
option (FCS_WITHOUT_TRIM_MAX_STORED_STATES "Don't include the trim-max-stored-states-functionality (should make things faster).")
option (FCS_WITHOUT_ITER_HANDLER "Don't include the iteration handler functionality (should make things faster).")
option (FCS_WITHOUT_MAX_NUM_STATES "Don't include the iterations limit functionality (should make things faster).")
option (FCS_WITHOUT_CMD_LINE_HELP "Don't include the cmd line help (should make things faster).")
option (FCS_DISABLE_MULTI_FLARES "Disable the multi-flaring functionality. May make things faster but break compatibilty. Enable at your own risk.")
option (FCS_DISABLE_MULTI_NEXT_INSTS "Disable the --next-instance/-ni functionality. May make things faster but break compatibilty. Enable at your own risk.")
option (FCS_DISABLE_MOVES_TRACKING "Disable the moves-tracking functionality. May make things faster but break a lot of compatibilty. Enable at your own risk.")
option (FCS_DISABLE_NUM_STORED_STATES "Disable the num-stored-states functionality. May make things faster but breaks compatibilty. Enable at your own risk.")
option (FCS_DISABLE_ERROR_STRINGS "Disables passing/setting the error strings - breaks a lot of compatibility - enable at your own risk.")
option (FCS_DISABLE_STATE_VALIDITY_CHECK "Disable the state validity check. May make things faster but breaks some compatibilty. Enable at your own risk.")
option (FCS_HARD_CODE_SCANS_SYNERGY_AS_TRUE "Make scan synergy as always true. Reduces functionality but may make things faster.")
option (FCS_HARD_CODE_REPARENT_STATES_AS_FALSE "Make reparent states as always false. Reduces functionality but may make things faster.")
option (FCS_HARD_CODE_CALC_REAL_DEPTH_AS_FALSE "Make calc-real-depth as always false. Reduces functionality but may make things faster.")
option (FCS_WITHOUT_EXPORTED_RESUME_SOLUTION "Don't export resume_solution. Breaks compat but may make things faseer. Enable at your own risk.")
option (FCS_ENABLE_PRUNE__R_TF__UNCOND "Hard code -sp as 'r:tf' (may be useful for speedup but breaks functionality.)")
option (FCS_USE_PRECOMPILED_CMD_LINE_THEME "Hard code a command line theme as generated by the compiler (useful for speedup but breaks functionality.)")
option (FCS_AVOID_INT128 "Avoid using GCC's __int128 extension (used for debugging)")
option (FCS_RANGE_SOLVERS_PRINT_SOLVED "Print solved boards.")


IF (NOT (STATES_TYPE MATCHES "^(INDIRECT_STACK_STATES|COMPACT_STATES)$"))
    MESSAGE(FATAL_ERROR "STATES_TYPE should be ('INDIRECT_STACK_STATES', or 'COMPACT_STATES')")
ENDIF()

SET (${STATES_TYPE} 1)

IF (FCS_ENABLE_RCS_STATES)
    SET (FCS_DISABLE_MOVES_TRACKING 0)
    SET (FCS_RCS_STATES 1)
ENDIF ()

# Clone the patsolve repository with the appropriate branch.
SET (pats_dir "patsolve")

IF (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${pats_dir}")
    EXECUTE_PROCESS(COMMAND "git" "clone" "https://github.com/shlomif/patsolve.git" "${pats_dir}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
ENDIF ()

SET (_gen_c_files )
MACRO(add_gen_c)
    LIST(APPEND _gen_c_files ${ARGV})
ENDMACRO ()

SET (MOVE_GEN_MOD "${CMAKE_CURRENT_SOURCE_DIR}/t/lib/FC_Solve/MoveFuncs.pm")
SET (presets_gen "${CMAKE_CURRENT_SOURCE_DIR}/scripts/gen-presets.pl")
SET (presets_gen_output "${CMAKE_CURRENT_BINARY_DIR}/fcs_presets_generated.h")
SET (presets_gen_args "${presets_gen}" "--mode=c" "--output" "${presets_gen_output}")
add_gen_c("${presets_gen_output}")
# We do this so there will be no need to run "make" first.
# See: http://github.com/kripken/emscripten/issues/766#issuecomment-11771150
ADD_CUSTOM_COMMAND(
    OUTPUT "${presets_gen_output}"
    COMMAND "${PERL_EXECUTABLE}"
    ARGS ${presets_gen_args}
    DEPENDS "${presets_gen}" "${MOVE_GEN_MOD}"
)
SET (ENV{FCS_MAX_RANK} "${FCS_MAX_RANK}")
SET (rate_state_output "${CMAKE_CURRENT_BINARY_DIR}/rate_state.c" "${CMAKE_CURRENT_BINARY_DIR}/rate_state.h")
# We do this so there will be no need to run "make" first.
# See: http://github.com/kripken/emscripten/issues/766#issuecomment-11771150
SET (is_parent_gen "${CMAKE_CURRENT_SOURCE_DIR}/scripts/gen-c-lookup-files.pl")
SET (is_parent_args "${is_parent_gen}")
SET (is_parent_output
    "${CMAKE_CURRENT_BINARY_DIR}/debondt__card_pos.c" "${CMAKE_CURRENT_BINARY_DIR}/debondt__card_pos.h"
    "${CMAKE_CURRENT_BINARY_DIR}/debondt__state_pos.c" "${CMAKE_CURRENT_BINARY_DIR}/debondt__state_pos.h"
    "${CMAKE_CURRENT_BINARY_DIR}/is_king.c" "${CMAKE_CURRENT_BINARY_DIR}/is_king.h"
    "${CMAKE_CURRENT_BINARY_DIR}/is_parent.c" "${CMAKE_CURRENT_BINARY_DIR}/is_parent.h"
    "${CMAKE_CURRENT_BINARY_DIR}/pos_by_rank__freecell.c" "${CMAKE_CURRENT_BINARY_DIR}/pos_by_rank__freecell.h"
    ${rate_state_output}
    )
add_gen_c(${is_parent_output})
ADD_CUSTOM_COMMAND(
    OUTPUT ${is_parent_output}
    COMMAND "${PERL_EXECUTABLE}"
    ARGS ${is_parent_args}
    DEPENDS "${is_parent_gen}"
)
# Execute it right away to avoid missing files/includes
EXECUTE_PROCESS(COMMAND "${PERL_EXECUTABLE}" ${is_parent_args})

SET (patsolve_dir "${pats_dir}/patsolve")
SET (p_dir "${patsolve_dir}")

SET (patsolve_modules
    "param.c"
    "${p_dir}/pat.c"
    "${p_dir}/patsolve.c"
    "${p_dir}/tree.c"
    )

FIND_PROGRAM(_GPERF "gperf")

IF ("${_GPERF}" STREQUAL "_GPERF-NOTFOUND")
    MESSAGE(FATAL_ERROR "gperf must be in the path - see https://www.gnu.org/software/gperf/ .")
ENDIF ()

SET (CL_INC_H "cmd_line_inc.h")
SET (BIN_CL_INC_H "${CMAKE_CURRENT_BINARY_DIR}/${CL_INC_H}")
SET (CL_INC_SCRIPT "scripts/gen-cmd-line-radix-tree.pl")
SET (cl_enum_h "${CMAKE_CURRENT_BINARY_DIR}/cmd_line_enum.h")

add_gen_c("${BIN_CL_INC_H}" "${cl_enum_h}")
ADD_CUSTOM_COMMAND(
    OUTPUT
        "${BIN_CL_INC_H}"
        "${cl_enum_h}"
    COMMAND "${PERL_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/${CL_INC_SCRIPT}"
    DEPENDS "${CL_INC_SCRIPT}"
)

SET (MOVE_C move_funcs_maps.c)
SET (MOVE_H move_funcs_maps.h)
SET (MOVE_GEN "${CMAKE_CURRENT_SOURCE_DIR}/scripts/gen-move-funcs.pl")

add_gen_c("${CMAKE_CURRENT_BINARY_DIR}/${MOVE_C}"
        "${CMAKE_CURRENT_BINARY_DIR}/${MOVE_H}")

ADD_CUSTOM_COMMAND(
    OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/${MOVE_C}"
        "${CMAKE_CURRENT_BINARY_DIR}/${MOVE_H}"
    COMMAND "${PERL_EXECUTABLE}" "${MOVE_GEN}"
    DEPENDS "${MOVE_GEN}" "${MOVE_GEN_MOD}"
)

ADD_CUSTOM_TARGET(
    generate_c_files
    DEPENDS ${_gen_c_files}
)

SET (FREECELL_SOLVER_LIB_HEADERS
    is_king.h
    "${BIN_CL_INC_H}"
    "${presets_gen_output}"
)

SET (FREECELL_SOLVER_LIB_MODULES
    check_and_add_state.c
    freecell.c
    instance.c
    is_king.c
    is_parent.c
    lib.c
    meta_alloc.c
    move.c
    move_funcs_order.c
    pos_by_rank__freecell.c
    rate_state.c
    scans.c
    state.c
    "${MOVE_C}"
)
MACRO(add_lib_mods)
    LIST (APPEND FREECELL_SOLVER_LIB_MODULES ${ARGV})
ENDMACRO()

IF (NOT "${FCS_DISABLE_MOVES_TRACKING}")
    add_lib_mods(card.c)
ENDIF ()
IF (NOT "${FCS_DISABLE_PATSOLVE}")
    SET (pp_dir "${CMAKE_CURRENT_SOURCE_DIR}/${p_dir}")
    EXECUTE_PROCESS(COMMAND "python3" "${pp_dir}/param.py" "${pp_dir}/param.dat")
    add_lib_mods(${patsolve_modules})
ENDIF ()
IF (NOT "${FCS_USE_PRECOMPILED_CMD_LINE_THEME}")
    add_lib_mods(cmd_line.c split_cmd_line.c)
ENDIF ()

IF (FCS_FREECELL_ONLY)
    SET (FCS_DISABLE_SIMPLE_SIMON 1)
ELSE ()
    add_lib_mods("preset.c")
ENDIF ()

IF ("${FCS_DISABLE_MULTI_FLARES}")
    SET (FCS_WITHOUT_FC_PRO_MOVES_COUNT 1)
ENDIF ()

IF (NOT "${FCS_WITHOUT_FC_PRO_MOVES_COUNT}")
    add_lib_mods("fc_pro_iface.c")
ENDIF ()

IF (NOT "${FCS_DISABLE_SIMPLE_SIMON}")
    add_lib_mods("simpsim.c")
ENDIF ()

INCLUDE(CheckSymbolExists)

SET (CMAKE_REQUIRED_DEFINITIONS "-D_GNU_SOURCE=1")
ADD_DEFINITIONS ("-D_GNU_SOURCE=1")
# ADD_DEFINITIONS ("-DXXH_INLINE_ALL=1")
CHECK_SYMBOL_EXISTS("vasprintf" "stdio.h" HAVE_VASPRINTF)

IF (NOT "${HAVE_VASPRINTF}")
    INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/asprintf-1.0")
    add_lib_mods("asprintf-1.0/vasprintf-c99.c")
ENDIF ()

IF (NOT ("${STATES_TYPE}" STREQUAL "INDIRECT_STACK_STATES"))
    # FCS_STACK_STORAGE in this case is irrelevant and should be ignored.
    SET (FCS_STACK_STORAGE "FCS_STACK_STORAGE_NULL")
ENDIF ()

# Add the kaz_tree.c module if (and only if) it is being used.
#
IF ("${FCS_STATE_STORAGE}" STREQUAL "FCS_STATE_STORAGE_KAZ_TREE" OR
    (FCS_RCS_STATES AND ("${FCS_RCS_CACHE_STORAGE}" STREQUAL "FCS_RCS_CACHE_STORAGE_KAZ_TREE")))

    add_lib_mods("kaz_tree.c")

ENDIF ()

SET (SKIP_VALGRIND )
# Add the google_hash.cpp if (and only if) it is being used.
#
IF (("${FCS_STATE_STORAGE}" STREQUAL "FCS_STATE_STORAGE_GOOGLE_DENSE_HASH") OR
    ("${FCS_STACK_STORAGE}" STREQUAL "FCS_STACK_STORAGE_GOOGLE_DENSE_HASH"))

    add_lib_mods("google_hash.cpp")

    SET (SKIP_VALGRIND 1)
ENDIF ()

# Search for libredblack if appropriate.
IF (("${FCS_STATE_STORAGE}" STREQUAL "FCS_STATE_STORAGE_LIBREDBLACK_TREE") OR
    ("${FCS_STACK_STORAGE}" STREQUAL "FCS_STACK_STORAGE_LIBREDBLACK_TREE"))

    FIND_LIBRARY(LIBREDBLACK_LIB redblack)

ENDIF ()

# Search for glib if appropriate.
IF (("${FCS_STATE_STORAGE}" STREQUAL "FCS_STATE_STORAGE_GLIB_TREE") OR
    ("${FCS_STATE_STORAGE}" STREQUAL "FCS_STATE_STORAGE_GLIB_HASH") OR
    ("${FCS_STACK_STORAGE}" STREQUAL "FCS_STACK_STORAGE_GLIB_TREE") OR
    ("${FCS_STACK_STORAGE}" STREQUAL "FCS_STACK_STORAGE_GLIB_HASH"))

    FIND_PACKAGE (PkgConfig REQUIRED)
    pkg_check_modules (GLIB REQUIRED glib-2.0)

    include_directories (${GLIB_INCLUDE_DIRS})
    link_directories (${GLIB_LIBRARY_DIRS})
    add_definitions (${GLIB_CFLAGS_OTHER})

ENDIF ()

SET (WITH_JUDY )

# Search for Judy ( http://judy.sourceforge.net/ ) if appropriate.
IF (("${FCS_STATE_STORAGE}" STREQUAL "FCS_STATE_STORAGE_JUDY") OR
    ("${FCS_STACK_STORAGE}" STREQUAL "FCS_STACK_STORAGE_JUDY") OR
    (FCS_RCS_STATES AND ("${FCS_RCS_CACHE_STORAGE}" STREQUAL "FCS_RCS_CACHE_STORAGE_JUDY")
    )
)

    FIND_LIBRARY(LIBJUDY_LIB Judy)

    SET (WITH_JUDY 1)
ENDIF ()

SET (LIBAVL2_COPIED_MODULES_DIR "${CMAKE_CURRENT_BINARY_DIR}/libavl2")

# Initialize to the empty list - it may or may not be populated.
SET (LIBAVL2_TREE_TYPES )
MACRO(write_tree TREE_TYPE PREFIXO fn)
    LIST(APPEND LIBAVL2_TREE_TYPES "${TREE_TYPE}")
    LIST(REMOVE_DUPLICATES LIBAVL2_TREE_TYPES)
    FILE (MAKE_DIRECTORY "${LIBAVL2_COPIED_MODULES_DIR}")
    FILE (WRITE "${LIBAVL2_COPIED_MODULES_DIR}/${fn}"
"#pragma once
#include \"${TREE_TYPE}.h\"

typedef struct ${TREE_TYPE}_table ${PREFIXO}_table;
#define ${PREFIXO}_insert(a,b) ${TREE_TYPE}_insert((a),(b))
#define ${PREFIXO}_create(a,b,c) ${TREE_TYPE}_create((a),(b),(c))
#define ${PREFIXO}_destroy(a,b) ${TREE_TYPE}_destroy((a),(b))
")
ENDMACRO()

IF ("${FCS_STATE_STORAGE}" STREQUAL "FCS_STATE_STORAGE_LIBAVL2_TREE")
    write_tree("${FCS_STATE_STORAGE_LIBAVL2_TREE_TYPE}" "fcs_libavl2_states_tree" "fcs_libavl2_state_storage.h")
ENDIF ()

IF ("${FCS_STACK_STORAGE}" STREQUAL "FCS_STACK_STORAGE_LIBAVL2_TREE")
    write_tree("${FCS_STACK_STORAGE_LIBAVL2_TREE_TYPE}" "fcs_libavl2_stacks_tree" "fcs_libavl2_stack_storage.h")
ENDIF ()

IF (LIBAVL2_TREE_TYPES)
    INCLUDE_DIRECTORIES(BEFORE "${LIBAVL2_COPIED_MODULES_DIR}")
    FOREACH (TREE_TYPE ${LIBAVL2_TREE_TYPES})
        SET (C_FN "${TREE_TYPE}.c")
        FOREACH (fn "${C_FN}" "${TREE_TYPE}.h")
            FILE (COPY "${LIBAVL2_SOURCE_DIR}/${fn}"
                DESTINATION "${LIBAVL2_COPIED_MODULES_DIR}/")
        ENDFOREACH()
        add_lib_mods("${LIBAVL2_COPIED_MODULES_DIR}/${C_FN}")
    ENDFOREACH ()
ENDIF ()

SET (MY_LINK_FLAGS )
SET (MY_TO_PROFILE "")
IF (CMAKE_BUILD_TYPE STREQUAL profile)
    SET (MY_TO_PROFILE "1")
    SET (CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_DEBUG} -pg")
    LIST (APPEND MY_LINK_FLAGS "-pg" "-static-libgcc")
    SET (BUILD_STATIC_LIBRARY "1")
    SET (MY_TARGET_LINK_LIBS "freecell-solver-static" "m_p" "c_p")
ELSE ()
    IF (FCS_LINK_TO_STATIC)
        SET (BUILD_STATIC_LIBRARY "1")
        SET (MY_TARGET_LINK_LIBS "freecell-solver-static")
    ELSE ()
        SET (MY_TARGET_LINK_LIBS "freecell-solver")
    ENDIF()
ENDIF ()

INCLUDE(CPack)

include(CheckFunctionExists)
INCLUDE(CheckTypeSize)
INCLUDE(FindThreads)


SHLOMIF_ADD_COMMON_C_FLAGS()

IF (IS_DEBUG OR FCS_WITH_TEST_SUITE)
    add_flags("-DFCS_COMPILE_DEBUG_FUNCTIONS=1")
ENDIF ()

SHLOMIF_FINALIZE_FLAGS()

IF ("$ENV{FCS_CLANG}")
    ADD_DEFINITIONS("-Weverything -Wno-language-extension-token -Wno-format-nonliteral -Wno-padded -Wno-gnu-statement-expression -Wno-unused-parameter -Wno-vla -Wno-extra-semi -Wno-cast-align -Wno-used-but-marked-unused -Wno-cast-qual -Wno-conversion -Wno-sign-compare -Wno-overlength-strings -Wno-comma -Wno-disabled-macro-expansion")
ENDIF ()

IF ("$ENV{FCS_GCC}")
    ADD_DEFINITIONS("-W -Wabi=11 -Waddress -Waggressive-loop-optimizations -Wall -Wattributes -Wbad-function-cast -Wbool-compare -Wbool-operation -Wbuiltin-declaration-mismatch -Wbuiltin-macro-redefined -Wcast-align -Wchar-subscripts -Wchkp -Wclobbered -Wcomment -Wcomments -Wcoverage-mismatch -Wcpp -Wdangling-else -Wdate-time -Wdeprecated -Wdeprecated-declarations -Wdesignated-init -Wdisabled-optimization -Wdiscarded-array-qualifiers -Wdiscarded-qualifiers -Wdiv-by-zero -Wdouble-promotion -Wduplicated-branches -Wduplicated-cond -Wduplicate-decl-specifier -Wempty-body -Wendif-labels -Wenum-compare -Wexpansion-to-defined -Wextra -Wformat-contains-nul -Wformat-extra-args -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat-y2k -Wformat-zero-length -Wframe-address -Wfree-nonheap-object -Whsa -Wignored-attributes -Wignored-qualifiers -Wimplicit -Wimplicit-function-declaration -Wimplicit-int -Wincompatible-pointer-types -Winit-self -Winline -Wint-conversion -Wint-in-bool-context -Wint-to-pointer-cast -Winvalid-memory-model -Winvalid-pch -Wjump-misses-init -Wlogical-not-parentheses -Wlogical-op -Wmain -Wmaybe-uninitialized -Wmemset-elt-size -Wmemset-transposed-args -Wmisleading-indentation -Wmissing-braces -Wmissing-declarations -Wmissing-field-initializers -Wmissing-include-dirs -Wmissing-parameter-type -Wmissing-prototypes -Wmultichar -Wnarrowing -Wnested-externs -Wnonnull -Wnonnull-compare -Wnull-dereference -Wodr -Wold-style-declaration -Wold-style-definition -Wopenmp-simd -Woverflow -Woverlength-strings -Woverride-init -Wpacked -Wpacked-bitfield-compat -Wparentheses -Wpointer-arith -Wpointer-compare -Wpointer-sign -Wpointer-to-int-cast -Wpragmas -Wpsabi -Wrestrict -Wreturn-local-addr -Wreturn-type -Wscalar-storage-order -Wsequence-point -Wshadow -Wshift-count-negative -Wshift-count-overflow -Wshift-negative-value -Wsizeof-array-argument -Wsizeof-pointer-memaccess -Wstack-protector -Wstrict-aliasing -Wstrict-prototypes  -Wsuggest-attribute=format -Wsuggest-attribute=noreturn -Wsuggest-attribute=pure -Wsuggest-final-methods -Wsuggest-final-types -Wswitch -Wswitch-bool -Wswitch-default -Wswitch-unreachable -Wsync-nand -Wtautological-compare -Wtrampolines -Wtrigraphs -Wtype-limits -Wuninitialized -Wunknown-pragmas -Wunsafe-loop-optimizations -Wunused -Wunused-but-set-parameter -Wunused-but-set-variable -Wunused-function -Wunused-label -Wunused-local-typedefs -Wunused-macros -Wunused-parameter -Wunused-result -Wunused-value -Wunused-variable -Wvarargs -Wvariadic-macros -Wvector-operation-performance -Wvla -Wvolatile-register-var -Wwrite-strings -Walloc-size-larger-than=9223372036854775807 -Warray-bounds=2 -Wformat-overflow=2 -Wformat-truncation=2 -Wnormalized=nfc -Wshift-overflow=2  -Wunused-const-variable=2 -Wstrict-overflow=1  -Wno-switch-default -Wno-vla -Wno-inline  -Wno-jump-misses-init -Wno-unused-result -Wno-unsafe-loop-optimizations")
    # -Wimplicit-fallthrough=5
    # -Wno-vla-larger-than
    # -Wsuggest-attribute=const
    # -Wstringop-overflow=2

    IF (NOT IS_DEBUG)
        ADD_DEFINITIONS("-D_FORTIFY_SOURCE=2")
    ENDIF ()
ENDIF ()

IF (FCS_TRACE_MEM)
    ADD_DEFINITIONS("-DFCS_TRACE_MEM=1")
ENDIF ()

IF (FCS_MAX_NUM_SCANS_BUCKETS)
    ADD_DEFINITIONS("-DFCS_MAX_NUM_SCANS_BUCKETS=${FCS_MAX_NUM_SCANS_BUCKETS}")
ENDIF ()

# So it can find the includes when building fcs-libavl/rb.h.
INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/include" ${CMAKE_CURRENT_SOURCE_DIR})

# So it can find config.h
INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_BINARY_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}")

SET (FREECELL_SOLVER_LIBS)

ADD_LIBRARY (freecell-solver
    SHARED
    ${FREECELL_SOLVER_LIB_MODULES} ${FREECELL_SOLVER_LIB_HEADERS}
    )

LIST (APPEND FREECELL_SOLVER_LIBS "freecell-solver")
IF (BUILD_STATIC_LIBRARY)
    ADD_LIBRARY (freecell-solver-static
        STATIC
        ${FREECELL_SOLVER_LIB_MODULES} ${FREECELL_SOLVER_LIB_HEADERS}
    )
    LIST (APPEND FREECELL_SOLVER_LIBS "freecell-solver-static")

    SET_TARGET_PROPERTIES(
        freecell-solver-static
        PROPERTIES OUTPUT_NAME "freecell-solver"
    )
    SET_TARGET_PROPERTIES(
        ${FREECELL_SOLVER_LIBS}
        PROPERTIES CLEAN_DIRECT_OUTPUT 1
    )
ENDIF ()

ADD_LIBRARY (fcs_gen_ms_freecell_boards
    SHARED
    gen_boards_lib.c
    )

SET_TARGET_PROPERTIES(freecell-solver
    PROPERTIES VERSION 0.6.0 SOVERSION 0
    )

FIND_LIBM()
SET(LIBTCMALLOC_LIB_LIST)

# Avoid linking to tcmalloc if the test suite is enabled due to:
# https://github.com/gperftools/gperftools/issues/758
IF ((NOT (FCS_AVOID_TCMALLOC)) AND (NOT (FCS_WITH_TEST_SUITE)) AND ( NOT ((CMAKE_BUILD_TYPE STREQUAL "debug") OR (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") ) ))
    # Optionally link against Google's TCMalloc if it's available:
    # http://goog-perftools.sourceforge.net/
    # This gives better performance for the threaded programs.
    FIND_LIBRARY(LIBTCMALLOC_LIB tcmalloc)

    IF(LIBTCMALLOC_LIB STREQUAL "LIBTCMALLOC_LIB-NOTFOUND")
        # Do nothing.
    ELSE()
        SET(LIBTCMALLOC_LIB_LIST ${LIBTCMALLOC_LIB})
    ENDIF()
ENDIF ()

SET(FREECELL_SOLVER_EXECUTABLES )
MACRO(FCS_ADD_EXEC_NO_INSTALL target)
    SET(modules ${ARGV})
    LIST(REMOVE_AT modules 0)
    ADD_EXECUTABLE(${target} ${modules})
    LIST(APPEND FREECELL_SOLVER_EXECUTABLES ${target})
    TARGET_LINK_LIBRARIES (${target} ${MY_TARGET_LINK_LIBS})
ENDMACRO()

MACRO(FCS_ADD_EXEC target)
    FCS_ADD_EXEC_NO_INSTALL(${ARGV})
    INSTALL (TARGETS ${target} DESTINATION "bin")
ENDMACRO()


FCS_ADD_EXEC(fc-solve main.c)
FCS_ADD_EXEC(freecell-solver-range-parallel-solve serial_range_solver.c)
SET (_mods fc_pro_range_solver.c fc_pro_iface_aux.c)
IF ("${FCS_WITHOUT_FC_PRO_MOVES_COUNT}")
    LIST(APPEND _mods fc_pro_iface.c)
ENDIF ()
FCS_ADD_EXEC(freecell-solver-fc-pro-range-solve ${_mods})
FCS_ADD_EXEC_NO_INSTALL(fc-solve-pruner pruner-main.c)
FCS_ADD_EXEC_NO_INSTALL(fc-solve-multi multi_fc_solve_main.c)
FCS_ADD_EXEC_NO_INSTALL(summary-fc-solve summarizing_solver.c)

SET (DBM_FCC_COMMON card.c is_king.c is_king.h is_parent.c meta_alloc.c state.c)

IF (NOT FCS_AVOID_INT128)
    CHECK_TYPE_SIZE("__int128" INT128 LANGUAGE C)
ENDIF()
IF (HAVE_INT128)
    SET(FCS_USE_INT128_FOR_VAR_BASE 1)
ENDIF()

MACRO (MY_FIND_GMP reason_why my_option)
    IF (HAVE_INT128)
        SET(LIBGMP_LIB )
    ELSE()
        FIND_LIBRARY(LIBGMP_LIB gmp)

        IF (NOT LIBGMP_LIB)
            MESSAGE (FATAL_ERROR
    "You must install libgmp ( http://gmplib.org/ ) and its development package ${reason_why}.
    If you're not interested in building it, run cmake with the -D${my_option}= flag."
            )
        ENDIF ()
    ENDIF ()
ENDMACRO ()

# We also need to put the CMAKE_POLICY invocation here or else it still warns.
CMAKE_POLICY(SET CMP0054 NEW)

SET(unbuilt_err_fn "${CMAKE_SOURCE_DIR}/scripts/err-on-unbuilt-executable.pl")
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E touch "${unbuilt_err_fn}")
MACRO(gen_error_exe myprog)
    ADD_COPY_TARGET("${myprog}__target" "${unbuilt_err_fn}" "${CMAKE_CURRENT_BINARY_DIR}/${myprog}")
ENDMACRO()

IF (NOT "${FCS_USE_PRECOMPILED_CMD_LINE_THEME}")
    FCS_ADD_EXEC_NO_INSTALL(measure-depth-dep-tests-order-perf measure_depth_dep_tests_order_performance.c)
ELSE ()
    gen_error_exe( measure-depth-dep-tests-order-perf )
ENDIF ()
SET (DEBONDT_DELTA_STATES )
IF ("${FCS_ENABLE_DBM_SOLVER}")

    MY_FIND_GMP ("to build the DBM solver." "FCS_ENABLE_DBM_SOLVER")
    IF (NOT "${FCS_DISABLE_DEBONDT_DELTA_STATES}")
        ADD_DEFINITIONS("-DFCS_DEBONDT_DELTA_STATES=1")
        SET (DEBONDT_DELTA_STATES 1)
    ENDIF ()

    SET (DBM_LIBS "pthread")
    # LINK_DIRECTORIES("/home/shlomif/progs/C/pthreads/rwlock/fcfs-rwlock/pthreads")
    # INCLUDE_DIRECTORIES("/home/shlomif/progs/C/pthreads/rwlock/fcfs-rwlock/pthreads")

    IF (FCS_DBM_USE_RWLOCK)
        LIST(APPEND DBM_LIBS "pthread_rwlock_fcfs")
        ADD_DEFINITIONS("-DFCS_DBM_USE_RWLOCK=1")
    ENDIF ()

    SET (DBM_DEFINITIONS )

    IF (FCS_DBM_TREE_BACKEND STREQUAL "libavl2")
        SET (BIN_TREE_MODULE "fcs-libavl/rb.c")
        ADD_DEFINITIONS("-DFCS_DBM_USE_LIBAVL=1")
        LIST (APPEND DBM_DEFINITIONS "FCS_LIBAVL_STORE_WHOLE_KEYS=1"
            "FCS_DBM_RECORD_POINTER_REPR=1")
    ELSE ()
        SET (BIN_TREE_MODULE "kaz_tree.c")
    ENDIF ()

    IF (FCS_DBM_BACKEND STREQUAL "bdb")
        SET (_dbm_mod "dbm_bdb.c")
        LIST (INSERT DBM_LIBS 0 "db-4")
    ELSEIF (FCS_DBM_BACKEND STREQUAL "kaztree")
        SET (_dbm_mod "dbm_kaztree.c")
        # ADD_DEFINITIONS("-DFCS_DBM_CACHE_ONLY=1")
        ADD_DEFINITIONS("-DFCS_DBM_WITHOUT_CACHES=1")
    ELSE ()
        SET (_dbm_mod "dbm_leveldb.cpp")
        INCLUDE_DIRECTORIES(BEFORE "${LEVELDB_SOURCE_DIR}/include")
        LINK_DIRECTORIES("${LEVELDB_SOURCE_DIR}")
        LIST (INSERT DBM_LIBS 0 "leveldb")
    ENDIF ()
    SET (dbm_solvers)
    MACRO (DBM_EXE exe c_file)
        ADD_EXECUTABLE("${exe}" "${c_file}" ${DBM_FCC_COMMON} ${_dbm_mod} ${BIN_TREE_MODULE})
        LIST(APPEND dbm_solvers "${exe}")
    ENDMACRO()

    DBM_EXE(dbm_fc_solver dbm_solver.c)
    DBM_EXE(depth_dbm_fc_solver depth_dbm_solver.c)
    DBM_EXE(split_fcc_fc_solver split_fcc_solver.c)

    IF ((NOT ("${STATES_TYPE}" STREQUAL "INDIRECT_STACK_STATES")) AND "${WITH_JUDY}")
        DBM_EXE(pseudo_dfs_fc_solver pseudo_dfs_atomic_moves_solver.c)
        TARGET_LINK_LIBRARIES("pseudo_dfs_fc_solver" ${LIBJUDY_LIB})
    ELSE ()
        gen_error_exe( pseudo_dfs_fc_solver )
    ENDIF ()

    INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/fcs-libavl")

    FOREACH (TGT ${dbm_solvers})
        TARGET_LINK_LIBRARIES("${TGT}" ${DBM_LIBS} ${LIBTCMALLOC_LIB_LIST} ${LIBGMP_LIB})
        SET_TARGET_PROPERTIES("${TGT}"
            PROPERTIES COMPILE_DEFINITIONS "${DBM_DEFINITIONS}"
        )
    ENDFOREACH ()
ELSE ()
    gen_error_exe( depth_dbm_fc_solver )
    gen_error_exe( dbm_fc_solver )
    gen_error_exe( split_fcc_fc_solver )
    gen_error_exe( pseudo_dfs_fc_solver )
ENDIF ()

IF (CMAKE_USE_PTHREADS_INIT)
    FCS_ADD_EXEC(freecell-solver-multi-thread-solve threaded_range_solver.c)
    TARGET_LINK_LIBRARIES(freecell-solver-multi-thread-solve "pthread")
ENDIF ()

IF (UNIX)
    FCS_ADD_EXEC_NO_INSTALL(
        freecell-solver-fork-solve "forking_range_solver.c"
    )
ENDIF ()

SET (TARGETS "freecell-solver")
IF (FCS_LINK_TO_STATIC)
    LIST (APPEND TARGETS ${FREECELL_SOLVER_EXECUTABLES})
ENDIF()

ADD_DEFINITIONS(${_win32_static_lib_flags})
FOREACH (TGT ${TARGETS})
    TARGET_LINK_LIBRARIES (${TGT}
        ${MATH_LIB_LIST} ${LIBTCMALLOC_LIB_LIST} ${LIBREDBLACK_LIB} ${LIBJUDY_LIB} ${GLIB_LIBRARIES}
        ${_win32_static_lib_flags}
    )
ENDFOREACH ()


SET (MY_EXES_LINK_FLAGS ${MY_LINK_FLAGS} ${MY_EXE_FLAGS})
SET (MY_LIBS_LINK_FLAGS ${MY_LINK_FLAGS})
IF (MY_LIBS_LINK_FLAGS)
    STRING (REPLACE ";" " "  MY_LIBS_LINK_FLAGS_STRING "${MY_LIBS_LINK_FLAGS}")
    SET_TARGET_PROPERTIES(
        ${FREECELL_SOLVER_LIBS}
        PROPERTIES LINK_FLAGS "${MY_LIBS_LINK_FLAGS_STRING}"
    )
ENDIF ()

IF (MY_EXES_LINK_FLAGS)
    STRING (REPLACE ";" " "  MY_EXES_LINK_FLAGS_STRING "${MY_EXES_LINK_FLAGS}")
    SET_TARGET_PROPERTIES(
        ${FREECELL_SOLVER_EXECUTABLES}
        PROPERTIES LINK_FLAGS "${MY_EXES_LINK_FLAGS_STRING}"
    )
ENDIF ()

CHECK_FUNCTION_EXISTS(pow HAVE_POW)
CHECK_FUNCTION_EXISTS(strncasecmp HAVE_STRNCASECMP)

SET(AUTOGENERATED_CONFIG_H "config.h was auto-generated from config.h.in . Do not modify directly")
SET(AUTOGENERATED_BACK_COMPAT_H "fcs_back_compat.h was auto-generated from fcs_back_compat.h.in . Do not modify directly")

INCLUDE (CheckTypeSize)
CHECK_TYPE_SIZE("int" INT_SIZE_IN_BYTES)
CHECK_TYPE_SIZE("void*" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)

MATH(EXPR INT_SIZE_IN_BITS "8 * ${INT_SIZE_IN_BYTES}")

SET(TEMP_SIZE 1)
SET(FCS_INT_BIT_SIZE_LOG2 0)

WHILE ( NOT ("${TEMP_SIZE}" STREQUAL "${INT_SIZE_IN_BITS}") )

    MATH(EXPR TEMP2 "${TEMP_SIZE} << 1")
    SET(TEMP_SIZE "${TEMP2}")

    MATH(EXPR TEMP2 "${FCS_INT_BIT_SIZE_LOG2} + 1")
    SET(FCS_INT_BIT_SIZE_LOG2 "${TEMP2}")

ENDWHILE ()

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    )

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/fcs_back_compat.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/freecell-solver/fcs_back_compat.h
    )

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/prefix.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/prefix.h
    )

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/${CPACK_PACKAGE_NAME}.spec.in
    ${CMAKE_CURRENT_SOURCE_DIR}/${CPACK_PACKAGE_NAME}.spec
    )

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib${CPACK_PACKAGE_NAME}.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/lib${CPACK_PACKAGE_NAME}.pc
    @ONLY
    )

INSTALL(
    TARGETS ${FREECELL_SOLVER_LIBS}
    LIBRARY DESTINATION "lib${LIB_SUFFIX}"
    ARCHIVE DESTINATION "lib${LIB_SUFFIX}"
)

IF (WIN32 AND NOT UNIX)
    # This is so on Windows, the .dll's will be installed in the bin/
    # directory as well where the Freecell Solver command-line utilities
    # will be able to find them.

    INSTALL(
        TARGETS "freecell-solver"
        DESTINATION "bin"
    )
ENDIF ()

SET (fc_solve_manpage "${CMAKE_CURRENT_SOURCE_DIR}/fc-solve.6")

IF (UNIX AND FCS_BUILD_DOCS)
    INSTALL_MAN ("${fc_solve_manpage}" 6)
ENDIF ()

INSTALL(
    FILES
        ${docs_to_install}
        "README.win32.txt"
    DESTINATION
        "share/doc/freecell-solver/"
)

SET (dest "include/freecell-solver")
INSTALL(
    FILES
        "include/freecell-solver/fcs_cl.h"
        "include/freecell-solver/fcs_dllexport.h"
        "include/freecell-solver/fcs_enums.h"
        "include/freecell-solver/fcs_limit.h"
        "include/freecell-solver/fcs_move.h"
        "include/freecell-solver/fcs_user.h"
        "${p_dir}/include/freecell-solver/fcs_pats_xy_param.h"
    DESTINATION
        "${dest}"
)

INSTALL(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/include/freecell-solver/fcs_back_compat.h"
    DESTINATION
        "${dest}"
)

INSTALL(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/lib${CPACK_PACKAGE_NAME}.pc"
    DESTINATION
        "lib${LIB_SUFFIX}/pkgconfig"
)

# Rebuild config.h if ver.txt has changed.
ADD_CUSTOM_COMMAND(
    OUTPUT "config.h.in"
    DEPENDS "ver.txt"
    COMMAND "touch"
    ARGS "config.h.in"
)

SET (BOARDS "")
MACRO(add_boards)
    LIST(APPEND BOARDS ${ARGV})
ENDMACRO ()

MACRO(GEN_BOARD idx)
    SET(board "${idx}.board")
    ADD_CUSTOM_COMMAND(
        OUTPUT ${board}
        COMMAND "python3"
        ARGS "${CMAKE_CURRENT_SOURCE_DIR}/board_gen/make_pysol_freecell_board.py"
        "--ms" "-t" "${idx}" ">" "${board}"
    )
    add_boards("${board}")
ENDMACRO()

MACRO(GEN_BOARD_NO_NEWLINE idx)
    SET(orig_board "${idx}.board")
    SET(board "${idx}.no-newline.board")
    ADD_CUSTOM_COMMAND(
        OUTPUT "${board}"
        DEPENDS "${orig_board}"
        COMMAND "${PERL_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/perl-remove-trailing-newlines.pl" -o "${board}" "${orig_board}"
    )
    add_boards("${board}")
ENDMACRO()

FIND_PROGRAM(_PYTHON3 "python3")
SET (found_python3 )
IF (NOT "${_PYTHON3}" STREQUAL "_PYTHON3-NOTFOUND")
    SET (found_python3 "1")
    EXECUTE_PROCESS(COMMAND "python3" "-c" "import random2; import sys; sys.exit(0);" RESULT_VARIABLE _random2)
    IF ("${_random2}" STREQUAL "0")
        SET (found_python3_random2 "1")
    ELSE()
        SET (found_python3_random2)
        MESSAGE (WARNING "Could not find the python3's random2 module - please install it.")
    ENDIF ()
    EXECUTE_PROCESS(COMMAND "python3" "-c" "import six; import sys; sys.exit(0);" RESULT_VARIABLE _six)
    IF (NOT ("${_six}" STREQUAL "0"))
        MESSAGE (FATAL_ERROR "Could not find the python3's six module - please install it.")
    ENDIF ()
ENDIF ()

IF (found_python3_random2)
    FOREACH(idx 24 981 1941 1107600547)
        GEN_BOARD("${idx}")
    ENDFOREACH(idx)
    GEN_BOARD_NO_NEWLINE(24)


    SET(board "empty.board")
    ADD_CUSTOM_COMMAND(
        OUTPUT "${board}"
        COMMAND "${CMAKE_COMMAND}"
        ARGS "-E" "touch" "${board}"
    )
    add_boards("${board}")

    ADD_CUSTOM_TARGET(
        boards
        DEPENDS ${BOARDS}
    )
ENDIF ()

SET (_readme "${CMAKE_CURRENT_SOURCE_DIR}/README.asciidoc")
SET (_usage  "${CMAKE_CURRENT_SOURCE_DIR}/USAGE.asciidoc")
SET (_fc_solve_txt "fc-solve.txt")

ADD_CUSTOM_COMMAND(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${_fc_solve_txt}"
    COMMAND "perl"
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/gen-man-page.pl"
        "--readme" "${_readme}"
        "--usage" "${_usage}"
        "--output" "${CMAKE_CURRENT_SOURCE_DIR}/${_fc_solve_txt}"
    DEPENDS "${_readme}" "${_usage}"
)

ADD_CUSTOM_TARGET(
    asciidoc_man_pages_base ALL
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${_fc_solve_txt}"
)

IF (FCS_BUILD_DOCS)
    # Asciidoc is optional so should not be "REQUIRED" because we include
    # these files in the source distribution.
    FIND_PACKAGE(Asciidoc)
ENDIF ()

SET (_docfiles "AUTHORS.asciidoc" "COPYING.asciidoc" "HACKING.asciidoc"
    "INSTALL.asciidoc" "NEWS.asciidoc" "README.asciidoc" "TODO.asciidoc"
    "USAGE.asciidoc" "${_fc_solve_txt}"
)

SET (docs_to_install)
FOREACH(_file ${_docfiles})
    GET_FILENAME_COMPONENT(_file_we ${_file} NAME_WE)
    SET(_full_file "${CMAKE_CURRENT_BINARY_DIR}/${_file_we}")
    LIST(APPEND docs_to_install ${_full_file})
ENDFOREACH()

MACRO(TRAIL_SPACE_COPY_TARGET TARGET_NAME SOURCE DEST)
    ADD_CUSTOM_COMMAND(
        OUTPUT "${DEST}"
        COMMAND ${PERL_EXECUTABLE}
        ARGS "-lpe" "s/[ \\t]+$//ms" "<" "${SOURCE}" ">" "${DEST}"
        DEPENDS "${SOURCE}"
        VERBATIM
    )
    # The custom command needs to be assigned to a target.
    ADD_CUSTOM_TARGET(
        "${TARGET_NAME}" ALL
        DEPENDS "${DEST}"
    )
ENDMACRO()
IF(ASCIIDOC_FOUND)

    SET (_pdf_targets)
    FOREACH(_file ${_docfiles})
        GET_FILENAME_COMPONENT(_file_we ${_file} NAME_WE)
        SET(_in "${_file_we}")
        SET(_html_out "${CMAKE_CURRENT_SOURCE_DIR}/${_file_we}.html")
        SET(_full_file "${CMAKE_CURRENT_SOURCE_DIR}/${_file}")
        SET(_docbook_out "${CMAKE_CURRENT_SOURCE_DIR}/${_file_we}.xml")
        # MESSAGE(STATUS "_in == <<${_in}>> _html_out == <<${_html_out}>>")

        # -a toc
        SET (htmls )
        IF (NOT "${_file}" STREQUAL "${_fc_solve_txt}")
            SET (_flags )
            IF ("${_file}" STREQUAL "NEWS.asciidoc")
                LIST(APPEND _flags "--attribute=idprefix=news__")
            ENDIF ()
            ADD_CUSTOM_COMMAND(
                OUTPUT "${_html_out}"
                COMMAND ${ASCIIDOC_EXECUTABLE}
                    "-a" "linkcss"
                    "--backend" "xhtml5"
                    ${_flags}
                    -o ${_html_out} "${_full_file}"
                DEPENDS "${_full_file}"
                COMMENT "Asciidoc ${_in}"
                )
            LIST (APPEND htmls "${_html_out}")
        ENDIF ()


        SET (TO_DOCBOOK_EXTRA_FLAGS )
        IF ("${_file}" STREQUAL "${_fc_solve_txt}")
            LIST (APPEND TO_DOCBOOK_EXTRA_FLAGS "-d" "manpage")
        ENDIF ()

        ADD_CUSTOM_COMMAND(
            OUTPUT "${_docbook_out}"
            COMMAND ${ASCIIDOC_EXECUTABLE}
                --backend=docbook ${TO_DOCBOOK_EXTRA_FLAGS} -o ${_docbook_out} "${_full_file}"
            DEPENDS "${_full_file}"
            COMMENT "Asciidoc->DocBook ${_in}"
            )

        SET (_pdf "${_file_we}.pdf")

        SET (target "${_file_we}_build")

        ADD_CUSTOM_COMMAND(
            OUTPUT "${_pdf}"
            COMMAND docmake -v --make -o ${_pdf} pdf ${_docbook_out}
            DEPENDS ${_docbook_out}
            COMMENT "docmake to PDF"
        )

        SET (_file_we_full "${CMAKE_CURRENT_BINARY_DIR}/${_file_we}")
        ADD_CUSTOM_COMMAND(
            OUTPUT "${_file_we_full}"
            COMMAND cp -f "${_full_file}" "${_file_we_full}"
            DEPENDS "${_full_file}"
            COMMENT "copy AsciiDoc to non-\".txt\" file"
        )

        ADD_CUSTOM_TARGET(${target} ALL echo -n
            DEPENDS ${htmls} "${_docbook_out}" "${_file_we}"
        )

        ADD_CUSTOM_TARGET(${_in}_pdf echo -n
            DEPENDS "${_pdf}"
        )

        LIST(APPEND _pdf_targets ${_in}_pdf)

    ENDFOREACH()

    # MESSAGE ("PDF_TARGETS == ${_pdf_targets}")

    SET (_master_pdf_target "pdfs")
    ADD_CUSTOM_TARGET("${_master_pdf_target}" echo -n)
    ADD_DEPENDENCIES("${_master_pdf_target}" ${_pdf_targets})

    SET (_out "${CMAKE_CURRENT_SOURCE_DIR}/fc-solve.xml")
    SET (_manpage "${fc_solve_manpage}")

    ADD_CUSTOM_COMMAND(
        OUTPUT "${_manpage}"
        COMMAND "xsltproc" "--output" "${_manpage}"
            "--nonet" "/etc/asciidoc/docbook-xsl/manpage.xsl"
             "${_out}"
        DEPENDS "${_out}"
        COMMENT "Build man page ${_out}"
    )

    ADD_CUSTOM_TARGET("fc_solve_man" ALL echo -n
        DEPENDS "${_manpage}"
    )
ENDIF ()

IF (FCS_WITH_TEST_SUITE)

    ENABLE_TESTING()

    ADD_TEST(
        NAME perl_run_tests
        COMMAND "perl" "${CMAKE_CURRENT_SOURCE_DIR}/run-tests.pl"
    )

    ADD_CUSTOM_TARGET(
        "check"
        "perl" "${CMAKE_CURRENT_SOURCE_DIR}/run-tests.pl"
    )

ENDIF ()

# For Makefile.gnu in pgo.bash (see below)
FILE (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.deps")

ADD_CUSTOM_TARGET(
    "pgo_proto"
    COMMAND "bash" "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/pgo.bash" "gcc" "total"
    DEPENDS generate_c_files
            "${CMAKE_CURRENT_BINARY_DIR}/config.h"
            "${CMAKE_CURRENT_BINARY_DIR}/prefix.h"
)

# This does not work properly because the silencing happens earlier in the
# line, but I'm still keeping it.

ADD_CUSTOM_TARGET(
    "pgo"
    COMMAND "env" "VERBOSE=1" "make" "VERBOSE=1" "pgo_proto"
)

FILE (COPY "${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake_pgo_wrapper/pgo.sh"
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE WORLD_READ GROUP_READ
                    OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
)

ADD_CUSTOM_TARGET(
    "bench" bash "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/time-threads-num.bash" -s 2 "${MAX_NUM_BENCHMARK_THREADS}"
    COMMAND perl "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/time-fcs.pl" "DUMPS-*/*"
)


IF ("${FCS_ENABLE_DBM_SOLVER}" AND "${IS_DEBUG}" AND (NOT "${FCS_ENABLE_RCS_STATES}"))
    # The delta-states testing library
    ADD_LIBRARY(fcs_delta_states_test
        SHARED
        card.c state.c delta_states.c ${FREECELL_SOLVER_LIB_HEADERS}
    )
    # The DeBondt delta-states testing library
    ADD_LIBRARY(fcs_debondt_delta_states_test
        SHARED
        card.c state.c delta_states_debondt.c ${FREECELL_SOLVER_LIB_HEADERS}
    )
    TARGET_LINK_LIBRARIES(fcs_debondt_delta_states_test ${LIBGMP_LIB})
    ADD_LIBRARY(fcs_dbm_calc_derived_test
        SHARED
        ${DBM_FCC_COMMON}
        dbm_calc_derived_test.c
        )
    TARGET_LINK_LIBRARIES(fcs_dbm_calc_derived_test ${LIBGMP_LIB})
    ADD_LIBRARY(fcs_fcc_brfs_test
        SHARED
        ${DBM_FCC_COMMON} fcc_brfs_test.c ${BIN_TREE_MODULE}
        )
    TARGET_LINK_LIBRARIES(fcs_fcc_brfs_test ${LIBGMP_LIB})
ENDIF ()

INCLUDE_DIRECTORIES(AFTER "${CMAKE_CURRENT_SOURCE_DIR}/${patsolve_dir}/include" "${CMAKE_CURRENT_SOURCE_DIR}/${patsolve_dir}")
INCLUDE("${private_mod_path}/xxhash_wrapper_bootstrap.cmake")

ADD_SUBDIRECTORY ("t")
ADD_SUBDIRECTORY ("board_gen")
ADD_SUBDIRECTORY ("man")
ADD_SUBDIRECTORY ("Presets")
