CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_VERBOSE_MAKEFILE ON)

INCLUDE( "${CMAKE_CURRENT_SOURCE_DIR}/../Common.cmake" )

RUN_POD2MAN(
    "freecell-solver-range-parallel-solve.pod"
    "freecell-solver-range-parallel-solve.6"
    "6"
    "Freecell Solver"
    "Freecell Solver ${VERSION}"
)

INSTALL_MAN ( "freecell-solver-range-parallel-solve.6" 6)
