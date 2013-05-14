CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_VERBOSE_MAKEFILE ON)

RUN_POD2MAN(
    "${CMAKE_CURRENT_SOURCE_DIR}/freecell-solver-range-parallel-solve.pod"
    "freecell-solver-range-parallel-solve.6"
    "6"
    "Freecell Solver"
    "Freecell Solver ${VERSION}"
)

INSTALL_MAN ( "${CMAKE_CURRENT_BINARY_DIR}/freecell-solver-range-parallel-solve.6" 6)
