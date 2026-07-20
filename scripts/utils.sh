#!/usr/bin/env bash

# Clear/create the log file
executing_script_file_name="$(basename "$0")"
log_file="${BUILD_DIR}/${executing_script_file_name%%.*}.log"

# Define color variables
RED='\e[1;31m'
GREEN='\e[1;32m'
YELLOW='\e[1;33m'
WHITE='\e[1;37m'
NC='\e[0m' # No Color (Reset)

function logInfo {
    echo -e "[$(date +%T)] [${GREEN}info${NC}]:" $@
}

function logVerbose {
    echo -e "[$(date +%T)] [${WHITE}verbose{NC}]:" $@
}

function logWarn {
    echo -e "[$(date +%T)] [${YELLOW}warn{NC}]:" $@
}

function logError {
    echo -e "[$(date +%T)] [${RED}error{NC}]:" $@
}


# Redefine rm to create a safe rm command that helps minimize accidental deletion
# of protected directories when variables are not defined.
function rm() {
    # Define protected directories (Absolute paths, no trailing slashes)
    local protected=(
        "/"
        "/etc"
        "/var"
        "/usr"
        "/boot"
        "$HOME"
        "$HOME/Documents"
    )

    local target
    local safe=true

    # Loop through all arguments passed to rm
    for arg in "$@"; do
        # Skip flags (lines starting with -)
        [[ "$arg" =~ ^- ]] && continue

        # Resolve the full, absolute path of the argument
        if [[ -e "$arg" ]]; then
            target=$(realpath "$arg")
        else
            continue
        fi

        # Check if the target matches any protected path
        for protect in "${protected[@]}"; do
            if [[ "$target" == "$protect" ]]; then
                logError "safe-rm: Attempted to delete protected path: $target"
                logError "Operation aborted."
                safe=false
                break 2 # Break both loops
            fi
        done
    done

    # Run the real rm command if no protected paths were matched
    if [ "$safe" = true ]; then
        command rm "$@"
    else
        return 1
    fi
}

function time_step() {
    local step_name="$1"
    shift
    local command="$@"
    
    local start_time=$(date +%s)
    local start_time_friendly=$(date +%T)

    if [ ! -f "$log_file" ]; then
        mkdir -p "$(dirname "$log_file")"
        touch "$log_file"
    fi
    echo "$step_name..." >> "$log_file"
    
    if [ "$VERBOSE" = "true" ]; then
        # VERBOSE MODE: Print step name, stream stdout/stderr, then print final time
        logInfo "$step_name"
        
        # Pipe to console and file
        eval "$command" 2>&1 | tee -a "$log_file"
        local exit_code=${PIPESTATUS[0]} # Gets exit code of eval, not tee
        
        local end_time=$(date +%s)
        local elapsed=$((end_time - start_time))
        
        if [ $exit_code -eq 0 ]; then
            echo "✓ Completed in ${elapsed}s" | tee -a "$log_file"
        else
            echo "✗ Failed after ${elapsed}s (Exit code: $exit_code)"
        fi

        return $exit_code
    else        
        # Start the command in the background, redirecting output to log
        eval "$command" >> "$log_file" 2>&1 &
        local pid=$!
        
        # Timer loop running while the background process exists
        while kill -0 $pid 2>/dev/null; do
            local current_time=$(date +%s)
            local elapsed=$((current_time - start_time))
            # \r moves cursor to start of line, -n prevents a newline
            echo -ne "\r[$start_time_friendly] [${GREEN}info${NC}]: $step_name [${elapsed}s]... "
            sleep 1
        done
        
        # Wait for the process to finish to harvest its exit status
        wait $pid
        local exit_code=$?
        
        local end_time=$(date +%s)
        local elapsed=$((end_time - start_time))
        
        # Overwrite the final line with the result
        if [ $exit_code -eq 0 ]; then
            echo -e "\r[$start_time_friendly] [${GREEN}info${NC}]: $step_name [${elapsed}s]... ✓ Done!"
            echo "$step_name [${elapsed}s]... ✓ Done!" >> "$log_file"
        else
            echo -e "\r[$start_time_friendly] [${RED}error${NC}]: $step_name [${elapsed}s]... ✗ Failed! (Check $log_file)"
            echo "$step_name [${elapsed}s]... ✗ Failed!" >> "$log_file"
        fi
        
        return $exit_code
    fi
}

