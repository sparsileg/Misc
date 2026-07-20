#!/usr/bin/env bash
#
# gh-project-status.sh — lists all items in a GitHub Project with their
# Status column (e.g. To Do / In Progress / Deferred / Done), sorted by
# status, and prints a formatted table to stdout. Excludes "Done" items
# by default.
#
# Usage:
#   ./gh-project-status.sh -p <PROJECT_NUMBER> -o <OWNER> [-l LIMIT] [-d]
#
# Flags:
#   -p  GitHub Project number (see: gh project list --owner <OWNER>)   [required]
#   -o  Project owner login, or @me for the current user               [required]
#   -l  Max items to fetch (default: 500)                              [optional]
#   -d  Include "Done" items (excluded by default)                     [optional]
#
# Examples:
#   ./gh-project-status.sh -p 7 -o sparsileg
#   ./gh-project-status.sh -p 7 -o sparsileg -l 1000
#   ./gh-project-status.sh -p 7 -o sparsileg -d          # include Done items too
#
# Requires: gh (with the "project" auth scope — run
# `gh auth refresh -s project` if you get a permissions error) and jq.

set -euo pipefail

usage() {
  cat >&2 <<EOF
Usage: $0 -p <PROJECT_NUMBER> -o <OWNER> [-l LIMIT] [-d]

  -p  GitHub Project number (see: gh project list --owner <OWNER>)   [required]
  -o  Project owner login, or @me for the current user               [required]
  -l  Max items to fetch (default: 500)                              [optional]
  -d  Include "Done" items (excluded by default)                     [optional]
EOF
  exit 1
}

PROJECT_NUMBER=""
OWNER=""
LIMIT=500
INCLUDE_DONE=0

while getopts ":p:o:l:d" opt; do
  case "$opt" in
    p) PROJECT_NUMBER="$OPTARG" ;;
    o) OWNER="$OPTARG" ;;
    l) LIMIT="$OPTARG" ;;
    d) INCLUDE_DONE=1 ;;
    \?) echo "Error: unknown option -$OPTARG" >&2; usage ;;
    :)  echo "Error: -$OPTARG requires an argument" >&2; usage ;;
  esac
done

if [[ -z "$PROJECT_NUMBER" || -z "$OWNER" ]]; then
  usage
fi

if ! command -v gh >/dev/null 2>&1; then
  echo "Error: gh (GitHub CLI) not found on PATH." >&2
  exit 1
fi

if ! command -v jq >/dev/null 2>&1; then
  echo "Error: jq not found on PATH." >&2
  exit 1
fi

# --format json + real jq (not gh's built-in -q) for full jq compatibility
# (sort_by, etc.) and to avoid depending on gh's -q feature set.
gh project item-list "$PROJECT_NUMBER" --owner "$OWNER" --format json --limit "$LIMIT" \
  | jq -r --argjson includeDone "$INCLUDE_DONE" '
      .items
      | map(select($includeDone == 1 or ((.status // "") | ascii_downcase) != "done"))
      | sort_by(.status // "zzz_no_status")[]
      | [
          (.content.number // "—" | tostring),
          (.status // "No Status"),
          .title
        ]
      | @tsv
    ' \
  | { printf 'NUMBER\tSTATUS\tTITLE\n'; cat; } \
  | column -t -s $'\t'


# ----------------------------------------------------------------------
# ----------------------------------------------------------------------
# ----------------------------------------------------------------------
