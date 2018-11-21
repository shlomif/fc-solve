MACRO(MY_ADD_EXE EXE_FILE C_FILE)
    SET (EXE_FILE_DEST "${CMAKE_CURRENT_SOURCE_DIR}/${EXE_FILE}")
    SET (C_FILE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${C_FILE}")

    ADD_EXECUTABLE(
        ${EXE_FILE}
        ${C_FILE}
    )

    TARGET_LINK_LIBRARIES (${EXE_FILE} ${LIBTAP_LIB})

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
    EXECUTE_PROCESS (
        COMMAND "${PERL_EXECUTABLE}" "-M${MODULE}=" "-e" "exit(0)"
        RESULT_VARIABLE "RESULT"
    )
    IF (NOT RESULT EQUAL 0)
        MESSAGE(FATAL_ERROR "You need the Perl 5/CPAN distribution ${MODULE} to run the test suite. Either skip the test suite using -DFCS_WITH_TEST_SUITE=OFF or see http://perl-begin.org/topics/cpan/ for installing from CPAN.")
    ENDIF ()

    FIND_LIBRARY(LIBTAP_LIB tap)
    IF (NOT LIBTAP_LIB)
        MESSAGE (FATAL_ERROR
"You must install libtap ( http://www.shlomifish.org/open-source/projects/libtap/ ) and its development package or headers to run the test suite.
If you're not interested in building it, run cmake with the -DFCS_WITH_TEST_SUITE= flag."
    )
    ENDIF ()
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

    TARGET_LINK_LIBRARIES (${EXE_FILE} ${LIBTAP_LIB})

    SET_SOURCE_FILES_PROPERTIES (
        "bit-reader-and-writer-test.c"
        PROPERTIES
            OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../bit_rw.h"
    )

    IF ("${DEBONDT_DELTA_STATES}")

        SET (EXE_FILE "delta-states-debondt-test.t.exe")

        ADD_EXECUTABLE(
            "${EXE_FILE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/debondt-delta-states-test.c"
        )

        TARGET_LINK_LIBRARIES (${EXE_FILE} ${LIBTAP_LIB} ${LIBGMP_LIB})

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

        TARGET_LINK_LIBRARIES (${EXE_FILE} ${LIBTAP_LIB} ${LIBGMP_LIB})

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
        "-I${CMAKE_CURRENT_SOURCE_DIR}/.."
        "-I${CMAKE_CURRENT_SOURCE_DIR}/../fcs-libavl/"
        )
    TARGET_LINK_LIBRARIES (${EXE_FILE} ${LIBTAP_LIB})

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
IF (FCS_DISABLE_MULTI_FLARES)
    add_tag("no_flares")
ENDIF ()
IF ("${FCS_DISABLE_PATSOLVE}")
    add_tag("no_pats")
ENDIF ()
IF (SKIP_VALGRIND)
    add_tag("no_valg")
ENDIF ()
IF (NOT "${FCS_ENABLE_DBM_SOLVER}")
    add_tag("no_dbm")
ENDIF ()

FILE (WRITE "${CMAKE_CURRENT_BINARY_DIR}/TAGS.txt" "${test_tags}")
