INCLUDE( "${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake" )

# This is the equivalent to perform a "make dist"/"make distdir" etc.
SET(CPACK_PACKAGE_NAME "freecell-solver")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Freecell Solver")
SET(CPACK_PACKAGE_VENDOR "Shlomi Fish")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")


# Process and extract the version number.
FILE( READ "ver.txt" VERSION)

CHOMP (VERSION)

STRING (REGEX MATCHALL "([0-9]+)" VERSION_DIGITS "${VERSION}")

LIST(GET VERSION_DIGITS 0 CPACK_PACKAGE_VERSION_MAJOR)
LIST(GET VERSION_DIGITS 1 CPACK_PACKAGE_VERSION_MINOR)
LIST(GET VERSION_DIGITS 2 CPACK_PACKAGE_VERSION_PATCH)

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")

SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

SET(CPACK_SOURCE_IGNORE_FILES
    "/_CPack_Packages/"
    "/CMakeFiles/"
    "freecell-solver-[0-9]+\\\\.[0-9]+\\\\.[0-9]+(-Source|-Linux|)(/|\\\\.(sh|tar\\\\.(gz|bz2|Z|lzma))$)"
    "\\\\.o$"
    "~$"
    "/board_gen/(pi-make-microsoft-freecell-board|make-microsoft-freecell-board|make-gnome-freecell-board|make-aisleriot-freecell-board)$"
    "/fc-solve$"
    "\\\\.so(\\\\.[0-9]+)*$"
    "/.svn/"
    "/t/t/.*\\\\.exe$"
    "/CMakeCache.txt$"
    "/Makefile$"
    "/CTestTestfile.cmake$"
    "/CPackConfig.cmake$"
    "/tags$"
)


IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\fc-solve.berlios.de")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\fc-solve.berlios.de")
  SET(CPACK_NSIS_CONTACT "shlomif@iglu.org.il")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "fc-solve")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)

SET(CPACK_PACKAGE_EXECUTABLES 
    "fc-solve" "Single-Game Solver"
)

INCLUDE(CPack)

include(CheckFunctionExists)

ADD_SUBDIRECTORY ("board_gen")
ADD_SUBDIRECTORY ("man")
ADD_SUBDIRECTORY ("Presets")

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

ADD_LIBRARY (freecell-solver
    SHARED
    alloc.c app_str.c caas.c card.c cl_chop.c cmd_line.c fcs_dm.c fcs_hash.c fcs_isa.c freecell.c intrface.c lib.c lookup2.c move.c pqueue.c preset.c rand.c scans.c simpsim.c state.c
    )

SET_TARGET_PROPERTIES(freecell-solver
    PROPERTIES VERSION 0.5.0 SOVERSION 0
    )

TARGET_LINK_LIBRARIES (freecell-solver m)

ADD_EXECUTABLE (fc-solve
    main.c
)

ADD_EXECUTABLE (freecell-solver-range-parallel-solve
    test_multi_parallel.c
)

CHECK_FUNCTION_EXISTS(pow HAVE_POW)

TARGET_LINK_LIBRARIES (fc-solve freecell-solver)
TARGET_LINK_LIBRARIES (freecell-solver-range-parallel-solve freecell-solver)

SET (INDIRECT_STACK_STATES 1)
SET (IA_STATE_PACKS_GROW_BY 32 CACHE STRING "Amount to Grow State Packs By")
SET (MAX_NUM_FREECELLS 8 CACHE STRING "Maximal Number of Freecells")
SET (MAX_NUM_STACKS 10 CACHE STRING "Maximal Number of Stacks")
SET (MAX_NUM_INITIAL_CARDS_IN_A_STACK 8 CACHE STRING 
    "Maximal Number of Initial Cards in a Stack")
SET (MAX_NUM_DECKS 2 CACHE STRING "Maximal Number of Decks (1 or 2)")

SET (FCS_STATE_STORAGE "FCS_STATE_STORAGE_INTERNAL_HASH")
SET (FCS_STACK_STORAGE "FCS_STACK_STORAGE_INTERNAL_HASH")

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h
    )

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/prefix.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/prefix.h
    )

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/${CPACK_PACKAGE_NAME}.spec.in
    ${CMAKE_CURRENT_SOURCE_DIR}/${CPACK_PACKAGE_NAME}.spec
    )

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/${CPACK_PACKAGE_NAME}-config.in
    ${CMAKE_CURRENT_SOURCE_DIR}/${CPACK_PACKAGE_NAME}-config
    )

INSTALL(
    TARGETS "fc-solve" "freecell-solver-range-parallel-solve"
    DESTINATION "bin"
)

INSTALL(
    TARGETS "freecell-solver"
    DESTINATION "lib"
)

INSTALL(
    FILES "freecell-solver-config"
    DESTINATION "bin"
)

RUN_POD2MAN(
    "fc-solve.pod"
    "fc-solve.6"
    "6"
    "Freecell Solver"
    "Freecell Solver ${VERSION}"
)

INSTALL_MAN ("fc-solve.6" 6)

INSTALL(
    FILES
        "AUTHORS"
        "INSTALL"
        "README"
        "README.win32.txt"
        "TODO"
        "USAGE"
    DESTINATION
        "share/doc/freecell-solver/"
)

INSTALL(
    FILES
        "fcs_cl.h"
        "fcs_enums.h"
        "fcs_move.h"
        "fcs_user.h"
    DESTINATION
        "include/freecell-solver"
)
        
# Rebuild config.h if ver.txt has changed.
ADD_CUSTOM_COMMAND(
    OUTPUT "config.h.in"
    DEPENDS "ver.txt"
    COMMAND "touch" 
    ARGS "config.h.in"
    )

ENABLE_TESTING()

ADD_TEST(
    tap-tests
    perl run-tests.pl
)
