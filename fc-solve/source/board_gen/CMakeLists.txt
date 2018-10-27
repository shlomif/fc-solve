ADD_EXECUTABLE(
    pi-make-microsoft-freecell-board
    pi_make_microsoft_freecell_board.c
)

MACRO(my_board_gen_prog myprog tgt)
    SET (_dest "${CMAKE_CURRENT_BINARY_DIR}/${myprog}")
    SET (_src  "${CMAKE_CURRENT_SOURCE_DIR}/${myprog}")

    # Adapted from:
    # http://stackoverflow.com/questions/697560/how-to-copy-directory-from-source-tree-to-binary-tree
    ADD_CUSTOM_COMMAND(
        OUTPUT "${_dest}"
        DEPENDS "${_src}"
        COMMAND "${CMAKE_COMMAND}" "-E" "copy" "${_src}" "${_dest}"
    )

    ADD_CUSTOM_TARGET(
        "${tgt}"
        ALL DEPENDS "${_dest}"
    )

    INSTALL(
        PROGRAMS
            "${_dest}"
        DESTINATION
            "bin"
    )
ENDMACRO()

my_board_gen_prog("fc_solve_find_index_s2ints.py" "fc_solve_find_index_s2ints")
my_board_gen_prog("find-freecell-deal-index.py" "find_index_prog")
my_board_gen_prog("gen-multiple-pysol-layouts" "gen_multi_layouts_prog")
my_board_gen_prog("make_pysol_freecell_board.py" "make_pysol_board")
my_board_gen_prog("transpose-freecell-board.py" "transpose_freecell_board_prog")
# Empty list
SET (_pod_targets )

RUN_POD2MAN(
    "_pod_targets"
    "fc-solve-board_gen.pod"
    "fc-solve-board_gen.6"
    "6"
    "Freecell Solver"
    "Freecell Solver ${VERSION}"
)

RUN_POD2MAN(
    "_pod_targets"
    "transpose-freecell-board.py.pod"
    "transpose-freecell-board.py.6"
    "6"
    "Freecell Solver"
    "Freecell Solver ${VERSION}"
)

RUN_POD2MAN(
    "_pod_targets"
    "gen-multiple-pysol-layouts.pod"
    "gen-multiple-pysol-layouts.6"
    "6"
    "Freecell Solver"
    "Freecell Solver ${VERSION}"
)


IF (UNIX)
    INSTALL_MAN("${CMAKE_CURRENT_BINARY_DIR}/fc-solve-board_gen.6" 6)
    INSTALL_MAN("${CMAKE_CURRENT_BINARY_DIR}/gen-multiple-pysol-layouts.6" 6)
    INSTALL_MAN("${CMAKE_CURRENT_BINARY_DIR}/transpose-freecell-board.py.6" 6)
    # INSTALL_MAN("fc-solve-board_gen.6" 6)

    # Install the nroff symbolic links.
    INSTALL_MAN ("make_pysol_freecell_board.py.6" 6)
    INSTALL_MAN ("pi-make-microsoft-freecell-board.6" 6)
ENDIF ()

INSTALL(
    TARGETS
        "pi-make-microsoft-freecell-board"
    DESTINATION
        "bin"
)

ADD_CUSTOM_TARGET(
    "docs_pods" ALL
    DEPENDS ${_pod_targets}
)

ADD_LIBRARY (fcs_find_deal
    SHARED
    find_deal.c
)
