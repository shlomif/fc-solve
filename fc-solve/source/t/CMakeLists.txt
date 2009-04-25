INCLUDE( "${CMAKE_CURRENT_SOURCE_DIR}/../Common.cmake" )

FIND_LIBRARY(LIBTAP_LIB tap)

MACRO(MY_ADD_EXE EXE_FILE C_FILE)
    ADD_EXECUTABLE(
        ${EXE_FILE}
        ${C_FILE}
    )
    TARGET_LINK_LIBRARIES (${EXE_FILE} ${LIBTAP_LIB})
ENDMACRO(MY_ADD_EXE EXE_FILE C_FILE)

MY_ADD_EXE (
    "t/card-test-render.exe"
    "card-test-render.c"
)

MY_ADD_EXE (
    "t/card-test-parse.exe"
    "card-test-parse.c"
)

TARGET_LINK_LIBRARIES ("t/card-test-parse.exe""m" ${LIBREDBLACK_LIB} ${LIBJUDY_LIB})

SET_SOURCE_FILES_PROPERTIES (
    "card-test-parse.c"
    "card-test-render.c"
    PROPERTIES OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../card.c"
)

ADD_CUSTOM_COMMAND(
    OUTPUT "card-test-render.c"
    COMMAND "perl" ARGS "generate-card-tests.pl"
    MAIN_DEPENDENCY "card-test-render.c.tt"
    DEPENDS "generate-card-tests.pl"
)

ADD_CUSTOM_COMMAND(
    OUTPUT "card-test-parse.c"
    COMMAND "perl" ARGS "generate-card-tests.pl"
    MAIN_DEPENDENCY "card-test-parse.c.tt"
    DEPENDS "generate-card-tests.pl"
)

ADD_CUSTOM_TARGET(
    TARGET "test"
    COMMAND runprove ARGS "t/*.{exe,t}"
)

