INCLUDE( "${CMAKE_CURRENT_SOURCE_DIR}/../Common.cmake" )

SUBDIRS( presets )

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/presetrc.proto.in
    ${CMAKE_CURRENT_SOURCE_DIR}/presetrc
)

INSTALL(
    FILES
        "presetrc"
    DESTINATION
        "${PKGDATADIR}"
)
