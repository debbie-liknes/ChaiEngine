# - Find fmod
# Find the fmod includes and library
#
# Uses FMOD_HOME=</path/to/FMOD SoundSystem/FMOD Studio API>
# to locate the following
#
#  FMOD_INCLUDE_DIR - Where to find fmod includes
#  FMOD_LIBRARY     - List of libraries when using fmod
#  FMOD_FOUND       - True if fmod was found
if ("$ENV{FMOD_HOME}" STREQUAL "")
    message(FATAL_ERROR "FMOD_HOME not set! Set FMOD_HOME=</path/to/FMOD SoundSystem/FMOD Studio API> to locate FMOD libraries.")
endif ()

find_path(FMOD_INCLUDE_DIR "fmod.h"
        PATHS
        "$ENV{FMOD_HOME}/api/core/inc"
        DOC "fmod - Headers"
)

set(FMOD_NAMES fmod fmod_vc)

find_library(FMOD_LIBRARY NAMES ${FMOD_NAMES}
        PATHS
        "$ENV{FMOD_HOME}/api/core/lib/x64"
        DOC "fmod - Library"
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(FMOD DEFAULT_MSG FMOD_LIBRARY FMOD_INCLUDE_DIR)
