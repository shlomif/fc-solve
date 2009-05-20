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

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

SET(CPACK_SOURCE_IGNORE_FILES
    "/_CPack_Packages/"
    "/CMakeFiles/"
    "/.deps/"
    "freecell-solver-[0-9]+\\\\.[0-9]+\\\\.[0-9]+(-Source|-Linux|)(/|\\\\.(sh|tar\\\\.(gz|bz2|Z|lzma|xz))$)"
    "\\\\.o$"
    "~$"
    "/board_gen/(pi-make-microsoft-freecell-board|make-microsoft-freecell-board|make-gnome-freecell-board|make-aisleriot-freecell-board)$"
    "/(fc-solve|freecell-solver-range-parallel-solve|freecell-solver-fc-pro-range-solve|freecell-solver-multi-thread-solve)$"
    "/lib(fcs|freecell-solver)\\\\.(a|la)$"
    "\\\\.so(\\\\.[0-9]+)*$"
    "/\\\\.svn/"
    "/t/t/.*\\\\.exe$"
    "/t/Presets"
    "/CMakeCache\\\\.txt$"
    "/Makefile$"
    "/CTestTestfile\\\\.cmake$"
    "/cmake_install\\\\.cmake$"
    "/CPackConfig\\\\.cmake$"
    "/CPackSourceConfig\\\\.cmake$"
    "/tags$"
    "/freecell-solver-config$"
    "/([0-9]+)\\\\.board$"
    "/config\\\\.h$"
    "/install_manifest\\\\.txt$"
    "/t/card-test-(parse|render)\\\\.c$"
    "/Testing/"
)

IF(WIN32 AND NOT UNIX)
    # There is a bug in NSI that does not handle full unix paths properly. Make
    # sure there is at least one set of four (4) backlasshes.
    SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\dondorf-king.bmp")
    SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\fc-solve.exe")
    SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\fc-solve.berlios.de")
    SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\fc-solve.berlios.de")
    SET(CPACK_NSIS_DISPLAY_NAME "Freecell Solver")
    SET(CPACK_NSIS_CONTACT "shlomif@iglu.org.il")
    SET(CPACK_NSIS_MODIFY_PATH ON)
    # Setting for NSIS :
    SET(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\fc-solve.ico")
    SET(CPACK_NSIS_MUI_UNIICON ${CPACK_NSIS_MUI_ICON})
    SET(CPACK_PACKAGE_ICON ${CPACK_NSIS_MUI_ICON})
    SET(CPACK_NSIS_MODIFY_PATH "ON")
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "fc-solve")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)

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
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


SET (STATES_TYPE "INDIRECT_STACK_STATES" CACHE STRING 
    "States Type ('INDIRECT_STACK_STATES', 'COMPACT_STATES' or 'DEBUG_STATES'). No need to usually change.")
SET (IA_STATE_PACKS_GROW_BY 32 CACHE STRING "Amount to Grow State Packs By")
SET (MAX_NUM_FREECELLS 8 CACHE STRING "Maximal Number of Freecells")
SET (MAX_NUM_STACKS 10 CACHE STRING "Maximal Number of Stacks")
SET (MAX_NUM_INITIAL_CARDS_IN_A_STACK 8 CACHE STRING
    "Maximal Number of Initial Cards in a Stack")
SET (MAX_NUM_DECKS 2 CACHE STRING "Maximal Number of Decks (1 or 2)")

SET (FCS_STATE_STORAGE "FCS_STATE_STORAGE_INTERNAL_HASH" CACHE STRING "The State Storage Type")
SET (FCS_STACK_STORAGE "FCS_STACK_STORAGE_INTERNAL_HASH" CACHE STRING "The Stack Storage Type")

SET (FCS_FREECELL_ONLY "" CACHE BOOL "Configure Freecell Solver to only be able to solve Freecell (not recommended)")
SET (FCS_DISABLE_SIMPLE_SIMON "" CACHE BOOL "Exclude being able to solve Simple Simon from the Binary (not recommended)")
SET (FCS_WITH_TEST_SUITE "" CACHE BOOL "Also build and run the test suite.")

SET (FCS_ENABLE_SECONDARY_HASH_VALUE "" 
    CACHE BOOL 
    "Enable the secondary hash value calculation and track-keeping. Seems to slow things down."
)

SET (BUILD_STATIC_LIBRARY "1" CACHE BOOL "Whether to build the static library (which takes more time)" )

SET (CPU_ARCH "" CACHE STRING "A specific CPU architecture to pass to gcc")
SET (OPTIMIZATION_OMIT_FRAME_POINTER "" CACHE STRING "Use -fomit-frame-pointer with gcc (not recommended for debugging)")

IF (NOT (STATES_TYPE MATCHES "^(INDIRECT_STACK_STATES|COMPACT_STATES|DEBUG_STATES)$"))
    MESSAGE(FATAL_ERROR "STATES_TYPE should be ('INDIRECT_STACK_STATES', 'COMPACT_STATES' or 'DEBUG_STATES')")
ENDIF(NOT (STATES_TYPE MATCHES "^(INDIRECT_STACK_STATES|COMPACT_STATES|DEBUG_STATES)$"))

SET (${STATES_TYPE} 1)

SET (FREECELL_SOLVER_LIB_MODULES alloc.c app_str.c caas.c card.c cl_chop.c cmd_line.c fcs_isa.c freecell.c intrface.c lib.c move.c pqueue.c preset.c rand.c scans.c simpsim.c state.c)

IF (FCS_FREECELL_ONLY)
    SET (FCS_DISABLE_SIMPLE_SIMON 1)
ENDIF (FCS_FREECELL_ONLY)

IF (FCS_DISABLE_SIMPLE_SIMON)
    LIST (REMOVE_ITEM FREECELL_SOLVER_LIB_MODULES simpsim.c)
ENDIF (FCS_DISABLE_SIMPLE_SIMON)

# Add the fcs_hash.c if (and only if) it is being used.
# 
IF ((${FCS_STATE_STORAGE} STREQUAL "FCS_STATE_STORAGE_INTERNAL_HASH") OR
    (${FCS_STACK_STORAGE} STREQUAL "FCS_STACK_STORAGE_INTERNAL_HASH"))

    LIST(APPEND FREECELL_SOLVER_LIB_MODULES fcs_hash.c)

ENDIF ((${FCS_STATE_STORAGE} STREQUAL "FCS_STATE_STORAGE_INTERNAL_HASH") OR
    (${FCS_STACK_STORAGE} STREQUAL "FCS_STACK_STORAGE_INTERNAL_HASH"))

# Add fcs_dm.c if (and only if) it is being used.
# 
IF (${FCS_STATE_STORAGE} STREQUAL "FCS_STATE_STORAGE_INDIRECT")
    LIST(APPEND FREECELL_SOLVER_LIB_MODULES fcs_dm.c)
ENDIF (${FCS_STATE_STORAGE} STREQUAL "FCS_STATE_STORAGE_INDIRECT")

# Search for libredblack if appropriate.
IF ((${FCS_STATE_STORAGE} STREQUAL "FCS_STATE_STORAGE_LIBREDBLACK_TREE") OR
    (${FCS_STACK_STORAGE} STREQUAL "FCS_STACK_STORAGE_LIBREDBLACK_TREE"))

    FIND_LIBRARY(LIBREDBLACK_LIB redblack)

ENDIF ((${FCS_STATE_STORAGE} STREQUAL "FCS_STATE_STORAGE_LIBREDBLACK_TREE") OR
    (${FCS_STACK_STORAGE} STREQUAL "FCS_STACK_STORAGE_LIBREDBLACK_TREE"))

# Search for Judy ( http://judy.sourceforge.net/ ) if appropriate.
IF ((${FCS_STATE_STORAGE} STREQUAL "FCS_STATE_STORAGE_JUDY") OR
    (${FCS_STACK_STORAGE} STREQUAL "FCS_STACK_STORAGE_JUDY"))

    FIND_LIBRARY(LIBJUDY_LIB Judy)

ENDIF ((${FCS_STATE_STORAGE} STREQUAL "FCS_STATE_STORAGE_JUDY") OR
       (${FCS_STACK_STORAGE} STREQUAL "FCS_STACK_STORAGE_JUDY"))

SET (MY_LINK_FLAGS "")
SET (MY_TO_PROFILE "")
IF (CMAKE_BUILD_TYPE STREQUAL profile)
    SET (MY_TO_PROFILE "1")
    SET (CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_DEBUG} -pg")
    SET (MY_LINK_FLAGS "-pg -static-libgcc")
    SET (BUILD_STATIC_LIBRARY "1")
    SET (MY_TARGET_LINK_LIBS "freecell-solver-static" "m_p" "c_p")
ELSE (CMAKE_BUILD_TYPE STREQUAL profile)
    SET (MY_TARGET_LINK_LIBS "freecell-solver")
ENDIF (CMAKE_BUILD_TYPE STREQUAL profile)

INCLUDE(CPack)

include(CheckFunctionExists)
INCLUDE(CheckCCompilerFlag)
INCLUDE(FindThreads)

ADD_SUBDIRECTORY ("board_gen")
ADD_SUBDIRECTORY ("man")
ADD_SUBDIRECTORY ("Presets")
ADD_SUBDIRECTORY ("t")

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(COMPILER_FLAGS_TO_CHECK "-Wall" "-Werror=implicit-function-declaration")

IF (CPU_ARCH)
    LIST(APPEND COMPILER_FLAGS_TO_CHECK "-march=${CPU_ARCH}")
ENDIF(CPU_ARCH)

IF (OPTIMIZATION_OMIT_FRAME_POINTER)
    LIST(APPEND COMPILER_FLAGS_TO_CHECK "-fomit-frame-pointer")
ENDIF(OPTIMIZATION_OMIT_FRAME_POINTER)

FOREACH (CFLAG_TO_CHECK ${COMPILER_FLAGS_TO_CHECK})
    CHECK_C_COMPILER_FLAG(${CFLAG_TO_CHECK} FLAG_EXISTS)
    IF (${FLAG_EXISTS})
        ADD_DEFINITIONS(${CFLAG_TO_CHECK})
    ENDIF (${FLAG_EXISTS})
ENDFOREACH(CFLAG_TO_CHECK)

# So it can find config.h
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

SET (FREECELL_SOLVER_LIBS)

ADD_LIBRARY (freecell-solver
    SHARED
    ${FREECELL_SOLVER_LIB_MODULES}
    )

LIST (APPEND FREECELL_SOLVER_LIBS "freecell-solver")
IF (${BUILD_STATIC_LIBRARY})
    ADD_LIBRARY (freecell-solver-static
        STATIC
        ${FREECELL_SOLVER_LIB_MODULES}
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
ENDIF (${BUILD_STATIC_LIBRARY})

SET_TARGET_PROPERTIES(freecell-solver
    PROPERTIES VERSION 0.5.0 SOVERSION 0
    )

TARGET_LINK_LIBRARIES (freecell-solver "m" ${LIBREDBLACK_LIB} ${LIBJUDY_LIB})

SET(FREECELL_SOLVER_EXECUTABLES )
MACRO(FCS_ADD_EXEC target)
    SET(modules ${ARGV})
    LIST(REMOVE_AT modules 0)
    ADD_EXECUTABLE(${target} ${modules})
    LIST(APPEND FREECELL_SOLVER_EXECUTABLES ${target})
    TARGET_LINK_LIBRARIES (${target} ${MY_TARGET_LINK_LIBS})
ENDMACRO(FCS_ADD_EXEC target)

FCS_ADD_EXEC(fc-solve main.c)
FCS_ADD_EXEC(freecell-solver-range-parallel-solve test_multi_parallel.c)
FCS_ADD_EXEC(freecell-solver-fc-pro-range-solve fc_pro_range_solver.c fc_pro_iface.c)

IF (CMAKE_USE_PTHREADS_INIT)
    FCS_ADD_EXEC(freecell-solver-multi-thread-solve threaded_range_solver.c)
    TARGET_LINK_LIBRARIES(freecell-solver-multi-thread-solve "pthread")
ENDIF (CMAKE_USE_PTHREADS_INIT)

IF (MY_LINK_FLAGS)
    SET_TARGET_PROPERTIES(
        ${FREECELL_SOLVER_LIBS} 
        ${FREECELL_SOLVER_EXECUTABLES}
        PROPERTIES LINK_FLAGS "${MY_LINK_FLAGS}"
    )
ENDIF (MY_LINK_FLAGS)

CHECK_FUNCTION_EXISTS(pow HAVE_POW)

SET(AUTOGENERATED_CONFIG_H "config.h was auto-generated from config.h.in . Do not modify directly")
CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
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
    ${CMAKE_CURRENT_SOURCE_DIR}/${CPACK_PACKAGE_NAME}-config.in
    ${CMAKE_CURRENT_BINARY_DIR}/${CPACK_PACKAGE_NAME}-config
    )

INSTALL(
    TARGETS "fc-solve" "freecell-solver-range-parallel-solve"
    DESTINATION "bin"
)

INSTALL(
    TARGETS ${FREECELL_SOLVER_LIBS}
    DESTINATION "lib"
)

IF (WIN32 AND NOT UNIX)
    # This is so on Windows, the .dll's will be installed in the bin/ 
    # directory as well where the Freecell Solver command-line utilities
    # will be able to find them.

    INSTALL(
        TARGETS "freecell-solver"
        DESTINATION "bin"
    )
ENDIF (WIN32 AND NOT UNIX)

INSTALL(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${CPACK_PACKAGE_NAME}-config"
    DESTINATION "bin"
)

RUN_POD2MAN(
    "${CMAKE_CURRENT_SOURCE_DIR}/fc-solve.pod"
    "fc-solve.6"
    "6"
    "Freecell Solver"
    "Freecell Solver ${VERSION}"
)

INSTALL_MAN ("${CMAKE_CURRENT_BINARY_DIR}/fc-solve.6" 6)

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

SET (BOARDS "")

MACRO(GEN_BOARD idx)
    SET(board "${idx}.board")
    ADD_CUSTOM_COMMAND(
        OUTPUT ${board}
        COMMAND "python"
        ARGS "board_gen/make_pysol_freecell_board.py" ${idx} ">" ${board}
    )
    SET(BOARDS ${BOARDS} ${board})
ENDMACRO(GEN_BOARD idx)

FOREACH(idx 24 1941)
    GEN_BOARD(${idx})
ENDFOREACH(idx)

ADD_CUSTOM_TARGET(
    boards
    DEPENDS ${BOARDS}
)

