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

RUN_POD2MAN(
    "fc-solve-board_gen.pod"
    "fc-solve-board_gen.6"
    "6"
    "board_gen"
    "board_gen ${VERSION}"
)

INSTALL_MAN("fc-solve-board_gen.6" 6)

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
        "make_pysol_freecell_board.py"
    DESTINATION
        "bin"
)

