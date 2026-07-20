#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source "${SCRIPT_DIR}/env.sh"
source "${SCRIPT_DIR}/utils.sh"

# Set default values
CLEAN=false
DEBUG=false
CONFIGURE_ONLY=false
VERBOSE=false

# Loop through all arguments
while [[ $# -gt 0 ]]; do
  case "$1" in
    --clean)
        CLEAN=true
        shift
        ;;
    -d|--debug)
        DEBUG=true
        shift
        ;;
    -c|--configure-only)
        CONFIGURE_ONLY=true
        shift
        ;;
    -v|--verbose)
        VERBOSE=true
        shift
        ;;
    -h|--help)
        echo "Usage: $0 [options]"
        echo "  -c, --clean            Enable cleaning mode"
        echo "  -c, --configure-only   Stop after configuring build"
        echo "  -d, --debug            Build in debug (default: release)"
        echo "  -h, --help             Show this help message"
        echo "  -v, --verbose          Enables verbose logging"
        exit 0
        ;;
    --) # End of all options (useful if you have files starting with -)
        shift
        break
        ;;
    -*) # Any other flag is an error
        echo "Error: Unknown option $1" >&2
        exit 1
        ;;
    *) # Positional arguments (not starting with -)
        POSITIONAL_ARGS+=("$1")
        shift # Move past the argument
        ;;
  esac
done

# Restore positional parameters if any were collected
set -- "${POSITIONAL_ARGS[@]}"

if [ "${CLEAN}" == "true" ]; then
    logInfo "Removing build dir"
    rm -rf "${BUILD_DIR}"
fi

time_step "Configuring cmake" cmake -B "${BUILD_DIR}" -S "${PROJECT_DIR}"


if [ "${CONFIGURE_ONLY}" == "false" ]; then
    NUM_CORES=$(nproc)

    cd "${BUILD_DIR}"
    case "$(uname -s)" in
        Linux*)
            CMAKE_CONFIGURATION=Release
            if [ "${DEBUG}" == "true" ]; then
                CMAKE_CONFIGURATION=Debug
            fi
            time_step "Building solution" cmake --build . --config ${CMAKE_CONFIGURATION} 
            ;;
        MINGW64_NT*)
            # Use MSBuild directly if available for MSVC builds. Faster build times
            MSBUILD_PATH=$("/c/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe" -latest -requires Microsoft.Component.MSBuild -find MSBuild/**/Bin/MSBuild.exe | xargs -0 cygpath )
            MSBUILD_VERBOSITY=minimal
            MSBUILD_CONFIGURATION=Release
            
            if [ "${DEBUG}" == "true" ]; then
                MSBUILD_CONFIGURATION=Debug
            fi

            time_step "Building solution" "\"${MSBUILD_PATH}\"" Chai.slnx \
                -mt \
                -m \
                -nr:False \
                -interactive:False \
                -nologo \
                -t:Build \
                -p:Configuration=${MSBUILD_CONFIGURATION} \
                -verbosity:${MSBUILD_VERBOSITY}
            ;;
        *)
    esac
fi

logInfo "Build complete."

exit 0
