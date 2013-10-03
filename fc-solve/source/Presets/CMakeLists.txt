SUBDIRS( presets )

SET (suf "/presets/")
SET (PRESETS_DATA_DIR "${PKGDATADIR}${suf}")
IF (WIN32 AND NOT UNIX)
    # We need to put an actual $ followed by { and EXE_DIRNAME so we
    # escape it.
    SET (PRESETS_DATA_DIR "\${EXE_DIRNAME}/../${RELATIVE_PKGDATADIR}${suf}")
ENDIF (WIN32 AND NOT UNIX)

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/presetrc.proto.in
    ${CMAKE_CURRENT_BINARY_DIR}/presetrc
)

INSTALL(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/presetrc"
    DESTINATION
        "${RELATIVE_PKGDATADIR}"
)
