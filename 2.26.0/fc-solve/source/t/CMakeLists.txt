INCLUDE( "${CMAKE_CURRENT_SOURCE_DIR}/../Common.cmake" )

MESSAGE ("CMAKE_CUR = ${CMAKE_CURRENT_SOURCE_DIR}")

MACRO(MY_ADD_EXE EXE_FILE C_FILE)
    SET (EXE_FILE_DEST "${CMAKE_CURRENT_SOURCE_DIR}/${EXE_FILE}")
    SET (C_FILE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${C_FILE}")

    ADD_EXECUTABLE(
        ${EXE_FILE}
        ${C_FILE_SRC}
    )

    TARGET_LINK_LIBRARIES (${EXE_FILE} ${LIBTAP_LIB})

    SET_SOURCE_FILES_PROPERTIES (
        "${C_FILE_SRC}"
        PROPERTIES OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../card.c"
    )

    ADD_CUSTOM_COMMAND(
        OUTPUT "${C_FILE_SRC}"
        COMMAND "perl" ARGS "${CMAKE_CURRENT_SOURCE_DIR}/generate-card-tests.pl"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/card-test-render.c.tt"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/generate-card-tests.pl"
    )

ENDMACRO(MY_ADD_EXE EXE_FILE C_FILE)

IF ("${FCS_WITH_TEST_SUITE}")

    FIND_LIBRARY(LIBTAP_LIB tap)

    MY_ADD_EXE (
        "t/card-test-render.exe"
        "card-test-render.c"
    )

    MY_ADD_EXE (
        "t/card-test-parse.exe"
        "card-test-parse.c"
    )

    # SET_SOURCE_FILES_PROPERTIES (
    #    "card-test-parse.c"
    #    "card-test-render.c"
    #    PROPERTIES OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../card.c"
    #)

    ADD_CUSTOM_TARGET(
        "test"
        "cd" ".." "&&" "perl" "run-tests.pl"
    )

ENDIF("${FCS_WITH_TEST_SUITE}")

