CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_VERBOSE_MAKEFILE ON)

SET (_man_pod_targets )
RUN_POD2MAN(
    "_man_pod_targets"
    "freecell-solver-range-parallel-solve.pod"
    "freecell-solver-range-parallel-solve.6"
    "6"
    "Freecell Solver"
    "Freecell Solver ${VERSION}"
)

INSTALL_MAN ( "${CMAKE_CURRENT_BINARY_DIR}/freecell-solver-range-parallel-solve.6" 6)

ADD_CUSTOM_TARGET(
    "man_docs_pods" ALL
    DEPENDS ${_man_pod_targets}
)
