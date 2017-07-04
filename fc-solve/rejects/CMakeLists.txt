SET(CPACK_SOURCE_IGNORE_FILES
    "/_CPack_Packages/"
    "/CMakeFiles/"
    "^${PROJECT_SOURCE_DIR}/${base_with_ver}(-Source|-Linux)?/"
    "${base_with_ver}.tar\\\\.(gz|bz2|Z|lzma|xz)$"
    "\\\\.o$"
    "/board_gen/(pi-make-microsoft-freecell-board|make-microsoft-freecell-board|make-gnome-freecell-board|make-aisleriot-freecell-board)$"
    "/(dbm_fc_solver|fcc_fc_solver|fc-solve|fc-solve-pruner|freecell-solver-range-parallel-solve|freecell-solver-fc-pro-range-solve|freecell-solver-multi-thread-solve|freecell-solver-fork-solve|measure-depth-dep-tests-order-perf)$"
    "/lib(fcs|freecell-solver)\\\\.(a|la)$"
    "\\\\.so(\\\\.[0-9]+)*$"
    "/\\\\.svn/"
    "\\\\.(pdf|fo)$"
    "/t/t/.*\\\\.exe$"
    "/t/Presets"
    "/CMakeCache\\\\.txt$"
    "/Makefile$"
    "/CTestTestfile\\\\.cmake$"
    "/cmake_install\\\\.cmake$"
    "/CPackConfig\\\\.cmake$"
    "/CPackSourceConfig\\\\.cmake$"
    "/freecell-solver-config$"
    "/libfreecell-solver\\\\.pc$"
    "/([0-9]+)\\\\.board$"
    "/config\\\\.h$"
    "/install_manifest\\\\.txt$"
    "/t/card-test-(parse|render)\\\\.c$"
    "/Testing/"
    "/_Inline/"
)
# Inspired from /usr/share/autoconf/autoconf/c.m4
FOREACH(KEYWORD "inline" "__inline__" "__inline")
   IF(NOT DEFINED HAVE_C_INLINE)
     TRY_COMPILE(C_HAS_${KEYWORD} "${CMAKE_CURRENT_BINARY_DIR}"
       "${CMAKE_CURRENT_SOURCE_DIR}/test_inline.c"
       COMPILE_DEFINITIONS "-Dinline=${KEYWORD}")
     IF(C_HAS_${KEYWORD})
       SET(HAVE_C_INLINE TRUE)
       SET(FCS_INLINE_KEYWORD "${KEYWORD}")
     ENDIF()
   ENDIF()
ENDFOREACH()

# Cancelled out.
IF ()
    RUN_POD2MAN(
        "${CMAKE_CURRENT_SOURCE_DIR}/fc-solve.pod"
        "fc-solve.6"
        "6"
        "Freecell Solver"
        "Freecell Solver ${VERSION}"
    )
ENDIF ()

ADD_CUSTOM_TARGET(
    "dist"
    COMMAND make package_source
    COMMAND
    "perl" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/normalize-cmake-source-tarballs-time-stamps.pl"
    "--version" "${VERSION}"
    "--package-base" "${CPACK_PACKAGE_NAME}"
    "--source-dir" "${CMAKE_CURRENT_SOURCE_DIR}"
    "--binary-dir" "${CMAKE_CURRENT_BINARY_DIR}"
)

