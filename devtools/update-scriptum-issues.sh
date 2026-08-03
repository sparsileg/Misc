#!/usr/bin/env bash
#
# update-scriptum-issues.sh
#
# Applies the regenerated issue markdown files to existing GitHub issues,
# one at a time, via `gh issue edit`. Overwrites title + body in place —
# does NOT delete/recreate, so issue numbers, URLs, and comment history
# are preserved.
#
# Usage:
#   ./update-scriptum-issues.sh /path/to/issues-folder
#
# Options:
#   --auto        Apply all updates without pausing for confirmation
#                  (default: pauses before each issue, y/n/s/q)
#   --dry-run     Show what would be sent, apply nothing
#   --repo OWNER/REPO   Explicit repo (default: relies on gh's repo
#                  auto-detection from the current git remote)
#
# Requires: gh CLI installed and authenticated (`gh auth status`),
# and to be run from inside the scriptum repo (or pass --repo).

set -euo pipefail

ISSUES_DIR="${1:-}"
AUTO=false
DRY_RUN=false
REPO_FLAG=()

shift || true
while [[ $# -gt 0 ]]; do
    case "$1" in
        --auto) AUTO=true ;;
        --dry-run) DRY_RUN=true ;;
        --repo)
            shift
            REPO_FLAG=(--repo "$1")
            ;;
        *)
            echo "Unknown option: $1" >&2
            exit 1
            ;;
    esac
    shift
done

if [[ -z "$ISSUES_DIR" || ! -d "$ISSUES_DIR" ]]; then
    echo "Usage: $0 /path/to/issues-folder [--auto] [--dry-run] [--repo OWNER/REPO]" >&2
    exit 1
fi

if ! command -v gh >/dev/null 2>&1; then
    echo "Error: gh CLI not found. Install from https://cli.github.com/" >&2
    exit 1
fi

if ! gh auth status >/dev/null 2>&1; then
    echo "Error: gh is not authenticated. Run 'gh auth login' first." >&2
    exit 1
fi

shopt -s nullglob
FILES=("$ISSUES_DIR"/issue-*.md)
shopt -u nullglob

if [[ ${#FILES[@]} -eq 0 ]]; then
    echo "No issue-*.md files found in $ISSUES_DIR" >&2
    exit 1
fi

# Sort numerically by the issue number embedded in the filename
IFS=$'\n' SORTED=($(printf '%s\n' "${FILES[@]}" | sort -t- -k2 -n))
unset IFS

echo "Found ${#SORTED[@]} issue file(s) in $ISSUES_DIR"
echo

APPLIED=0
SKIPPED=0

for FILE in "${SORTED[@]}"; do
    BASENAME=$(basename "$FILE")

    # Extract issue number from filename: issue-06-C2-....md -> 6
    NUM=$(echo "$BASENAME" | sed -E 's/^issue-0*([0-9]+)-.*/\1/')

    if [[ -z "$NUM" || ! "$NUM" =~ ^[0-9]+$ ]]; then
        echo "Skipping $BASENAME  could not parse issue number from filename."
        ((SKIPPED++)) || true
        continue
    fi

    # Title = first line, stripped of leading "# "
    TITLE=$(head -n 1 "$FILE" | sed -E 's/^#\s*//')

    # Body = everything after the first line, with leading blank line trimmed
    BODY_FILE=$(mktemp)
    tail -n +2 "$FILE" | sed '/./,$!d' > "$BODY_FILE"

    echo "-----------------------------------------------------------"
    echo "Issue #$NUM"
    echo "Title: $TITLE"
    echo "File:  $BASENAME"
    echo "-----------------------------------------------------------"

    if $DRY_RUN; then
        echo "[dry-run] Would run: gh issue edit $NUM --title \"$TITLE\" --body-file $BASENAME"
        rm -f "$BODY_FILE"
        continue
    fi

    if ! $AUTO; then
        read -rp "Apply this update? [y/N/s(kip)/q(uit)] " ANSWER
        case "$ANSWER" in
            y|Y) ;;
            s|S)
                echo "Skipped #$NUM."
                ((SKIPPED++)) || true
                rm -f "$BODY_FILE"
                continue
                ;;
            q|Q)
                echo "Quitting. Applied: $APPLIED, Skipped: $SKIPPED"
                rm -f "$BODY_FILE"
                exit 0
                ;;
            *)
                echo "Skipped #$NUM."
                ((SKIPPED++)) || true
                rm -f "$BODY_FILE"
                continue
                ;;
        esac
    fi

    if gh issue edit "$NUM" "${REPO_FLAG[@]}" --title "$TITLE" --body-file "$BODY_FILE"; then
        echo "Updated #$NUM."
        ((APPLIED++)) || true
    else
        echo "FAILED to update #$NUM  check the issue number exists and gh has write access." >&2
    fi

    rm -f "$BODY_FILE"
    echo
done

echo "-----------------------------------------------------------"
echo "Done. Applied: $APPLIED, Skipped: $SKIPPED"
