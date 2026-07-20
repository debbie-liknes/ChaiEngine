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
    -v|--verbose)
        VERBOSE=true
        shift
        ;;
    -d|--debug)
        DEBUG=true
        shift
        ;;
    -h|--help)
        echo "Usage: $0 [options]"
        echo "  -d, --debug            Test in debug (default: release)"
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

CTEST_CONFIGURATION=Release
if [ "${DEBUG}" == "true" ]; then
    CTEST_CONFIGURATION=Debug
fi

cd "${BUILD_DIR}"
time_step "Running tests..." ctest --output-on-failure -C ${CTEST_CONFIGURATION}

exit 0
