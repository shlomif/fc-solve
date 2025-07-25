MACRO(MY_ADD_EXE EXE_FILE C_FILE)
    SET (EXE_FILE_DEST "${CMAKE_CURRENT_SOURCE_DIR}/${EXE_FILE}")
    SET (C_FILE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${C_FILE}")

    ADD_EXECUTABLE(
        ${EXE_FILE}
        ${C_FILE}
    )

    TARGET_LINK_LIBRARIES (${EXE_FILE} ${CMOCKA_LIBRARIES})

    SET_SOURCE_FILES_PROPERTIES (
        "${C_FILE}"
        PROPERTIES
            OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../card.c"
            GENERATED "1"
    )

    ADD_CUSTOM_COMMAND(
        OUTPUT "${C_FILE}"
        COMMAND "perl" ARGS "${CMAKE_CURRENT_SOURCE_DIR}/generate-card-tests.pl"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${C_FILE}.tt"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/generate-card-tests.pl"
            "${CMAKE_CURRENT_BINARY_DIR}/../include/freecell-solver/fcs_back_compat.h"
    )

ENDMACRO ()

MACRO(GEN_INDIVIDUAL_TESTS target_name perl_script valg_out module data_module)
    ADD_CUSTOM_COMMAND(
        OUTPUT "${valg_out}"
        COMMAND "perl" "${perl_script}"
        MAIN_DEPENDENCY "${perl_script}"
        DEPENDS
            "${module}"
            "${data_module}"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/FC_Solve/SplitTests.pm"
    )

    ADD_CUSTOM_TARGET(
        "${target_name}" ALL
        DEPENDS "${valg_out}"
    )
ENDMACRO ()

IF (FCS_WITH_TEST_SUITE)

    SET (MODULE "Task::FreecellSolver::Testing")
    if (NOT SKIP_PERL_TESTING_TASK_CHECK)
        EXECUTE_PROCESS (
            COMMAND "${PERL_EXECUTABLE}" "-M${MODULE}=" "-e" "exit(0)"
            RESULT_VARIABLE "RESULT"
        )
        IF (NOT RESULT EQUAL 0)
            MESSAGE(WARNING "You need the Perl 5/CPAN distribution ${MODULE} to run the test suite. Either skip the test suite using -DFCS_WITH_TEST_SUITE=OFF or see http://perl-begin.org/topics/cpan/ for installing from CPAN.")
        ENDIF ()
    ENDIF ()
    FOREACH (mod_ "cffi" "freecell_solver")
        ASSERT_PYTHON3_MODULE_PRESENCE("${mod_}" " It is needed by the test suite which was enabled.")
    ENDFOREACH()

    FIND_PACKAGE (PkgConfig REQUIRED)
    pkg_check_modules(CMOCKA REQUIRED cmocka)
    include_directories (${CMOCKA_INCLUDE_DIRS})
    link_directories (${CMOCKA_LIBRARY_DIRS})
    add_definitions (${CMOCKA_CFLAGS_OTHER})
    MY_FIND_GMP ("to run the test suite." "FCS_WITH_TEST_SUITE")

    INCLUDE_DIRECTORIES (BEFORE "${CMAKE_BINARY_DIR}")

    MY_ADD_EXE (
        "card-test-render.t.exe"
        "card-test-render.c"
    )

    MY_ADD_EXE (
        "card-test-parse.t.exe"
        "card-test-parse.c"
    )

    FILE (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/t")
    FILE (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/verify-cache")

    SET (EXE_FILE "bit-reader-and-writer-test.t.exe")

    ADD_EXECUTABLE(
        "${EXE_FILE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/bit-reader-and-writer-test.c"
    )

    TARGET_LINK_LIBRARIES (${EXE_FILE} ${CMOCKA_LIBRARIES})

    IF ("${DEBONDT_DELTA_STATES}")

        SET (EXE_FILE "delta-states-debondt-test.t.exe")

        ADD_EXECUTABLE(
            "${EXE_FILE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/debondt-delta-states-test.c"
        )

        TARGET_LINK_LIBRARIES (${EXE_FILE} ${CMOCKA_LIBRARIES}  ${LIBGMP_LIB})


        SET_SOURCE_FILES_PROPERTIES (
            "delta-states-test.c"
            PROPERTIES
                OBJECT_DEPENDS
                    "${CMAKE_CURRENT_SOURCE_DIR}/../delta_states_debondt.c"
                GENERATED "1"
        )

    ELSE ()

        SET (EXE_FILE "delta-states-test.t.exe")

        ADD_EXECUTABLE(
            "${EXE_FILE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/delta-states-test.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/../delta_states.c"
        )

        TARGET_LINK_LIBRARIES (${EXE_FILE} ${CMOCKA_LIBRARIES} ${LIBGMP_LIB})

        SET_SOURCE_FILES_PROPERTIES (
            "delta-states-test.c"
            PROPERTIES
                OBJECT_DEPENDS
                    "${CMAKE_CURRENT_SOURCE_DIR}/../delta_states.c"
                GENERATED "1"
        )
    ENDIF ()

    SET (EXE_FILE "dbm-kaztree-compare-records-test.t.exe")

    ADD_EXECUTABLE(
        "${EXE_FILE}"
        "dbm-kaztree-compare-records-test.c"
    )

    # This is a kind-of-temporary kludge to make
    # "dbm-kaztree-compare-records-test.c" build.
    ADD_DEFINITIONS("-DFCS_DBM_USE_LIBAVL=1"
        "-DFCS_LIBAVL_STORE_WHOLE_KEYS=1"
        "-I${CMAKE_CURRENT_SOURCE_DIR}/.."
        "-I${CMAKE_CURRENT_SOURCE_DIR}/../fcs-libavl/"
        )
    TARGET_LINK_LIBRARIES (${EXE_FILE} ${CMOCKA_LIBRARIES})

    GEN_INDIVIDUAL_TESTS(
        "generate_valgrind_tests"
        "${PROJECT_SOURCE_DIR}/scripts/gen-individual-valgrind-test-scripts.pl"
        "${CMAKE_CURRENT_BINARY_DIR}/t/valgrind--range_parallel_solve__11982_opt.t"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FC_Solve/Test/Valgrind.pm"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FC_Solve/Test/Valgrind/Data.pm"
    )
    GEN_INDIVIDUAL_TESTS(
        "generate_verify_tests"
        "${PROJECT_SOURCE_DIR}/scripts/gen-individual-verify-test-scripts.pl"
        "${CMAKE_CURRENT_BINARY_DIR}/t/verify--id24.t"

        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FC_Solve/Test/Verify.pm"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FC_Solve/Test/Verify/Data.pm"
    )
    GEN_INDIVIDUAL_TESTS(
        "generate_cmp_digest_tests"
        "${PROJECT_SOURCE_DIR}/scripts/gen-individual-compare-digests-test-scripts.pl"
        "${CMAKE_CURRENT_BINARY_DIR}/t/cmpdigest--id24.t"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FC_Solve/Test/CheckResults.pm"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FC_Solve/CheckResults/Data.pm"
    )
ENDIF ()

SET (test_tags )

MACRO(add_tag)
    LIST(APPEND test_tags ${ARGV})
ENDMACRO ()

IF (FCS_BREAK_BACKWARD_COMPAT_1)
    add_tag("break_backcompat")
ENDIF ()
IF (FCS_FREECELL_ONLY)
    add_tag("fc_only")
ENDIF ()
IF (DBM_APR)
    add_tag("dbm_apr")
ENDIF ()
IF (FCS_DISABLE_MULTI_FLARES)
    add_tag("no_flares")
ENDIF ()
IF ("${FCS_DISABLE_PATSOLVE}")
    add_tag("no_pats")
ENDIF ()
IF (FCS_ENABLE_RCS_STATES)
    add_tag("rcs_states")
ENDIF ()
IF ("${FCS_STATE_STORAGE}" STREQUAL "FCS_STATE_STORAGE_INTERNAL_HASH")
    add_tag("FCS_STATE_STORAGE_INTERNAL_HASH")
ENDIF ()
IF (SKIP_VALGRIND)
    add_tag("no_valg")
ENDIF ()
IF (NOT "${FCS_ENABLE_DBM_SOLVER}")
    add_tag("no_dbm")
ENDIF ()
IF (FCS_WITHOUT_EXPORTED_RESUME_SOLUTION)
    add_tag("FCS_WITHOUT_EXPORTED_RESUME_SOLUTION")
ENDIF ()
IF (FCS_WITHOUT_VISITED_ITER)
    add_tag("FCS_WITHOUT_VISITED_ITER")
ENDIF ()
IF ("${FCS_BREAK_BACKWARD_COMPAT_2}" OR "${FCS_DISABLE_ERROR_STRINGS}" OR "${FCS_DISABLE_MOVES_TRACKING}" OR "${FCS_DISABLE_MULTI_FLARES}" OR "${FCS_DISABLE_NUM_STORED_STATES}" OR "${FCS_DISABLE_STATE_VALIDITY_CHECK}" OR "${FCS_HARD_CODE_REPARENT_STATES_AS_FALSE}" OR "${FCS_HARD_CODE_STATE_DEPTH_FIELD}" OR "${FCS_UNSAFE}" OR "${FCS_USE_PRECOMPILED_CMD_LINE_THEME}" OR "${FCS_WITHOUT_ITER_HANDLER}" OR "${FCS_WITHOUT_MAX_NUM_STATES}" OR "${FCS_ZERO_FREECELLS_MODE}" OR (NOT "${FCS_WITH_TEST_SUITE}") OR (NOT ((NOT "${FCS_MAX_NUM_SCANS_BUCKETS}") OR ("${FCS_MAX_NUM_SCANS_BUCKETS}" GREATER_EQUAL "4"))) OR (NOT (IS_DEBUG OR FCS_WITH_TEST_SUITE)) OR ( "${FCS_BREAK_BACKWARD_COMPAT_1}" AND ( NOT ( "${FCS_CMD_LINE_ENABLE_INCREMENTAL_SOLVING}" STREQUAL "0" ) ) ) OR ("${FCS_FREECELL_ONLY}" AND (NOT ( "${FCS_HARD_CODED_NUM_FCS_FOR_FREECELL_ONLY}" STREQUAL "4") ) ) )
    add_tag("tests_may_fail")
ENDIF ()

FILE (WRITE "${CMAKE_CURRENT_BINARY_DIR}/TAGS.txt" "${test_tags}")
