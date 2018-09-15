# - Find Asciidoc
# this module looks for asciidoc
#
# ASCIIDOC_EXECUTABLE - the full path to asciidoc
# ASCIIDOC_FOUND - If false, don't attempt to use asciidoc.

# This file is taken from:
# http://grahamcox.co.uk/serendipity/index.php?/archives/36-CMake-code-to-support-asciidoc.html
FIND_PROGRAM(ASCIIDOC_EXECUTABLE
asciidoctor
)

MARK_AS_ADVANCED(
ASCIIDOC_EXECUTABLE
)

IF (NOT ASCIIDOC_EXECUTABLE)
    SET(ASCIIDOC_FOUND "NO")
ELSE ()
    SET(ASCIIDOC_FOUND "YES")
ENDIF ()


IF (NOT ASCIIDOC_FOUND AND Asciidoc_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find asciidoctor")
ENDIF ()
