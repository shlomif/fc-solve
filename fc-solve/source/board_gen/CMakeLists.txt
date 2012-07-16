INCLUDE( "${CMAKE_CURRENT_SOURCE_DIR}/../Common.cmake" )

ADD_EXECUTABLE (
    make-gnome-freecell-board
    make_gnome_freecell_board.c
)

ADD_EXECUTABLE(
    make-aisleriot-freecell-board
    make_aisleriot_freecell_board.c
)

ADD_EXECUTABLE(
    make-microsoft-freecell-board
    make_microsoft_freecell_board.c
)

ADD_EXECUTABLE(
    pi-make-microsoft-freecell-board
    pi_make_microsoft_freecell_board.c
)

SET (make_pysol "make_pysol_freecell_board.py")
SET (_dest "${CMAKE_CURRENT_BINARY_DIR}/${make_pysol}")
SET (_src  "${CMAKE_CURRENT_SOURCE_DIR}/${make_pysol}")

# Adapted from:
# http://stackoverflow.com/questions/697560/how-to-copy-directory-from-source-tree-to-binary-tree
ADD_CUSTOM_COMMAND(
    OUTPUT "${_dest}"
    DEPENDS "${_src}"
    COMMAND "${CMAKE_COMMAND}" "-E" "copy" "${_src}" "${_dest}"
)

ADD_CUSTOM_TARGET(
    "make_pysol_board"
    ALL DEPENDS "${_dest}"
)

SET (_make_py_dest "${_dest}")

RUN_POD2MAN(
    "${CMAKE_CURRENT_SOURCE_DIR}/fc-solve-board_gen.pod"
    "fc-solve-board_gen.6"
    "6"
    "board_gen"
    "board_gen ${VERSION}"
)

INSTALL_MAN("${CMAKE_CURRENT_BINARY_DIR}/fc-solve-board_gen.6" 6)

# Install the nroff symbolic links.
INSTALL_MAN ("make-aisleriot-freecell-board.6" 6)
INSTALL_MAN ("make-gnome-freecell-board.6" 6)
INSTALL_MAN ("make_pysol_freecell_board.py.6" 6)
INSTALL_MAN ("pi-make-microsoft-freecell-board.6" 6)

INSTALL(
    TARGETS
        "make-gnome-freecell-board"
        "pi-make-microsoft-freecell-board"
        "make-aisleriot-freecell-board"
        "make-microsoft-freecell-board"
    DESTINATION
        "bin"
)

INSTALL(
    PROGRAMS
        "${_make_py_dest}"
    DESTINATION
        "bin"
)

