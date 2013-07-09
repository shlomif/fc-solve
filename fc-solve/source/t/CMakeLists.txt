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
    )

ENDMACRO(MY_ADD_EXE EXE_FILE C_FILE)

IF (FCS_WITH_TEST_SUITE)

    FIND_LIBRARY(LIBTAP_LIB tap)
    IF (NOT LIBTAP_LIB)
        MESSAGE (FATAL_ERROR
"You must install libtap ( http://www.shlomifish.org/open-source/projects/libtap/ ) and its development package or headers to run the test suite.
If you're not interested in building it, run cmake with the -DFCS_WITH_TEST_SUITE= flag."
    )
    ENDIF (NOT LIBTAP_LIB)
    MY_FIND_GMP ("to run the test suite." "FCS_WITH_TEST_SUITE")

    INCLUDE_DIRECTORIES (BEFORE "${CMAKE_BINARY_DIR}")

    MY_ADD_EXE (
        "t/card-test-render.exe"
        "card-test-render.c"
    )

    MY_ADD_EXE (
        "t/card-test-parse.exe"
        "card-test-parse.c"
    )

    ADD_EXECUTABLE(
        "out-split-cmd-line.exe"
        "${CMAKE_CURRENT_SOURCE_DIR}/out-split-cmd-line.c"
    )

    SET_SOURCE_FILES_PROPERTIES (
        "out-split-cmd-line.c"
        PROPERTIES
            OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../split_cmd_line.c"
            GENERATED "1"
    )

    FILE (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/t")

    SET (EXE_FILE "t/bit-reader-and-writer-test.exe")

    ADD_EXECUTABLE(
        "${EXE_FILE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/bit-reader-and-writer-test.c"
    )

    TARGET_LINK_LIBRARIES (${EXE_FILE} ${LIBTAP_LIB})

    SET_SOURCE_FILES_PROPERTIES (
        "bit-reader-and-writer-test.c"
        PROPERTIES
            OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../bit_rw.h"
            GENERATED "1"
    )

    IF (DEBONDT_DELTA_STATES)

        SET (EXE_FILE "t/delta-states-debondt-test.exe")

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

    ELSE (DEBONDT_DELTA_STATES)

        SET (EXE_FILE "t/delta-states-test.exe")

        ADD_EXECUTABLE(
            "${EXE_FILE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/delta-states-test.c"
        )

        TARGET_LINK_LIBRARIES (${EXE_FILE} ${LIBTAP_LIB} ${LIBGMP_LIB})

        SET_SOURCE_FILES_PROPERTIES (
            "delta-states-test.c"
            PROPERTIES
                OBJECT_DEPENDS
                    "${CMAKE_CURRENT_SOURCE_DIR}/../delta_states.c"
                GENERATED "1"
        )
    ENDIF (DEBONDT_DELTA_STATES)

    SET (EXE_FILE "t/dbm-kaztree-compare-records-test.exe")

    ADD_EXECUTABLE(
        "${EXE_FILE}"
        "dbm-kaztree-compare-records-test.c"
    )

    # This is a kind-of-temporary kludge to make
    # "dbm-kaztree-compare-records-test.c" build.
    ADD_DEFINITIONS("-DFCS_DBM_USE_LIBAVL=1"
        "-I${CMAKE_CURRENT_SOURCE_DIR}/.."
        "-I${CMAKE_CURRENT_SOURCE_DIR}/../libavl/"
        )
    TARGET_LINK_LIBRARIES (${EXE_FILE} ${LIBTAP_LIB})

    SET (perl_script "${PROJECT_SOURCE_DIR}/scripts/generate-individual-valgrind-test-scripts.pl")
    SET (valg_out  "${CMAKE_CURRENT_BINARY_DIR}/t/valgrind--range_parallel_solve__11982_opt.t")

    ADD_CUSTOM_COMMAND(
        OUTPUT "${valg_out}"
        COMMAND "perl" "${perl_script}"
        MAIN_DEPENDENCY "${perl_script}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/t/lib/FC_Solve/Test/Valgrind.pm"
    )

    ADD_CUSTOM_TARGET(
        generate_valgrind_tests ALL
        DEPENDS "${valg_out}"
    )
ENDIF(FCS_WITH_TEST_SUITE)

