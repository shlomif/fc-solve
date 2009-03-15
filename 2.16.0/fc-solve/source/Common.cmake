include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(FindPerl)
IF (NOT PERL_FOUND)
    MESSAGE ( FATAL_ERROR "perl must be installed")
ENDIF(NOT PERL_FOUND)

# Taken from http://www.cmake.org/pipermail/cmake/2007-March/013060.html
MACRO(REPLACE_FUNCTIONS sources)
  FOREACH(name ${ARGN})
    STRING(TOUPPER have_${name} SYMBOL_NAME)
    CHECK_FUNCTION_EXISTS(${name} ${SYMBOL_NAME})
    IF(NOT ${SYMBOL_NAME})
      SET(${sources} ${${sources}} ${name}.c)
    ENDIF(NOT ${SYMBOL_NAME})
  ENDFOREACH(name)
ENDMACRO(REPLACE_FUNCTIONS)

MACRO(CHECK_MULTI_INCLUDE_FILES)
  FOREACH(name ${ARGN})
    STRING(TOUPPER have_${name} SYMBOL_NAME)
    STRING(REGEX REPLACE "\\." "_" SYMBOL_NAME ${SYMBOL_NAME})
    STRING(REGEX REPLACE "/" "_" SYMBOL_NAME ${SYMBOL_NAME})
    CHECK_INCLUDE_FILE(${name} ${SYMBOL_NAME})
  ENDFOREACH(name)
ENDMACRO(CHECK_MULTI_INCLUDE_FILES)

MACRO(CHECK_MULTI_FUNCTIONS_EXISTS)
  FOREACH(name ${ARGN})
    STRING(TOUPPER have_${name} SYMBOL_NAME)
    CHECK_FUNCTION_EXISTS(${name} ${SYMBOL_NAME})
  ENDFOREACH(name)
ENDMACRO(CHECK_MULTI_FUNCTIONS_EXISTS)

MACRO(PREPROCESS_PATH_PERL SOURCE DEST)
    SET(PATH_PERL ${PERL_EXECUTABLE})
    ADD_CUSTOM_COMMAND(
        OUTPUT ${DEST}
        COMMAND ${PATH_PERL} 
        ARGS "-e" 
        "open I, qq{<\$ARGV[0]}; open O, qq{>\$ARGV[1]}; while(<I>){s{\\@PATH_PERL\\@}{\$ARGV[2]}g;print O \$_;} close(I); close(O);"
        ${SOURCE}
        ${DEST}
        ${PATH_PERL}
        COMMAND chmod ARGS "a+x" ${DEST}
        DEPENDS ${SOURCE}
        VERBATIM
    )
    # The custom command needs to be assigned to a target.
    ADD_CUSTOM_TARGET(
        process_perl ALL
        DEPENDS ${DEST}
    )
ENDMACRO(PREPROCESS_PATH_PERL)

MACRO(RUN_POD2MAN SOURCE DEST SECTION CENTER RELEASE)
    SET(PATH_PERL ${PERL_EXECUTABLE})
    ADD_CUSTOM_COMMAND(
        OUTPUT ${DEST}
        COMMAND ${PATH_PERL} 
        ARGS "-e" 
        "my (\$src, \$dest, \$sect, \$center, \$release) = @ARGV; my \$pod = qq{Hoola.pod}; use File::Copy; copy(\$src, \$pod); system(qq{pod2man --section=\$sect --center=\"\$center\" --release=\"\$release\" \$pod > \$dest}); unlink(\$pod)"
        ${SOURCE}
        ${DEST}
        ${SECTION}
        "${CENTER}"
        "${RELEASE}"
        MAIN_DEPENDENCY ${SOURCE}
        VERBATIM
    )
    ADD_CUSTOM_TARGET(
        POD_${DEST} ALL
        DEPENDS ${DEST}
    )
ENDMACRO(RUN_POD2MAN)

MACRO(CHOMP VAR)
    STRING(REGEX REPLACE "[\r\n]+$" "" ${VAR} "${${VAR}}")
ENDMACRO(CHOMP)

MACRO(INSTALL_MAN SOURCE SECTION)
    INSTALL(
        FILES
            ${SOURCE}
        DESTINATION
            "share/man/man${SECTION}"
   )
ENDMACRO(INSTALL_MAN)


# Configure paths.
SET (DATADIR "${CMAKE_INSTALL_PREFIX}/share"
    CACHE PATH "The data dir"
)

SET (PKGDATADIR "${DATADIR}/freecell-solver")
