# Clone the patsolve repository with the appropriate branch.
SET (xxhash_wrapper_dir_base "xxHash-wrapper")
SET (xxhash_wrapper_dir "${CMAKE_CURRENT_SOURCE_DIR}/xxHash-wrapper")

IF (NOT EXISTS "${xxhash_wrapper_dir}")
    EXECUTE_PROCESS(COMMAND "git" "clone" "https://github.com/shlomif/xxHash-wrapper.git" "${xxhash_wrapper_dir_base}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
ENDIF ()

INCLUDE("${xxhash_wrapper_dir}/xxhash_wrapper.cmake")
