CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_VERBOSE_MAKEFILE ON)

RUN_POD2MAN(
    "_pod_targets"
    "freecell-solver-range-parallel-solve"
    "freecell-solver-range-parallel-solve"
    "6"
    "Freecell Solver"
    "Freecell Solver ${VERSION}"
)

INSTALL_MAN ( "${CMAKE_CURRENT_BINARY_DIR}/freecell-solver-range-parallel-solve.6" 6)
