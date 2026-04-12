#!/bin/bash
# .jenkins/report-warnings.sh
#
# Generate a non-failing warning summary for Jenkins console output.

set -eu

if [ "$#" -gt 1 ]; then
    echo "Usage: $0 [artifacts_dir]" >&2
    exit 1
fi

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ARTIFACT_DIR="${1:-$REPO_ROOT/pages/artifacts}"

if [ ! -d "$ARTIFACT_DIR" ]; then
    echo "No warning artifact directory found: $ARTIFACT_DIR"
    exit 0
fi

mapfile -t WARN_FILES < <(find "$ARTIFACT_DIR" -maxdepth 1 -type f -name '*-warns.zip' | sort)

if [ "${#WARN_FILES[@]}" -eq 0 ]; then
    echo "No warning artifacts detected."
    exit 0
fi

echo "========================================"
echo "WARNING ARTIFACTS DETECTED"
echo "The build completed successfully, but warning artifacts were generated."
echo "Review the following files for details:"
for warn_file in "${WARN_FILES[@]}"; do
    echo " - $(basename "$warn_file")"
done
echo "Published HTML index (if configured): pages/index.html"
echo "========================================"
