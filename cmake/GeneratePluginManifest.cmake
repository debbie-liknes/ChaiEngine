# This script executes during the build step (POST_BUILD) of plugin targets
# See chai_common.cmake

string(TIMESTAMP CURRENT_BUILD_TIME "%Y-%m-%dT%H:%M:%SZ" UTC)

# Execute git command during the build phase
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY "${SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# Fallback if git fails (e.g., building outside a git repo)
if(NOT GIT_COMMIT_HASH)
    set(GIT_COMMIT_HASH "unknown")
endif()

file(SHA256 "${TARGET_FILE}" TARGET_CHECKSUM)

configure_file(
    "${INPUT_FILE}"
    "${OUTPUT_FILE}"
    @ONLY 
)
