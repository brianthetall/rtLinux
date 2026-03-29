#!/usr/bin/env sh

set -o errexit
set -o nounset

#Check for not being root
#Check that doas is installed
#Check that the <user> is cool with doas

# Check 1: Ensure the caller is not root
if [ "$(id -u)" -eq 0 ]; then
    echo "Error: This script must NOT be run as root." >&2
    exit 1
fi

# Check 2: Ensure the <doas> program is installed and executable
if ! command -v doas >/dev/null 2>&1; then
    echo "Error: The 'doas' command is not installed or not in the PATH." >&2
    exit 1
fi

# Check 3: Check if the user is configured to use doas (specifically to run a command, e.g., 'id -u' as root)
# The '-n' flag runs doas in non-interactive mode, failing if a password is required.
# The 'id -u' command is an arbitrary command used to check permissions.
doas -n id -u >/dev/null 2>&1;
