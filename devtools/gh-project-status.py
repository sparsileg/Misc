#!/usr/bin/env python3
"""
gh-project-status.py — lists all items in a GitHub Project with their
Status column (e.g. To Do / In Progress / Deferred / Done), sorted by
status, and prints a formatted table to stdout. Excludes "Done" items
by default.

Cross-platform replacement for gh-project-status.sh — works identically
via `python`/`python3` on Linux, Windows, and macOS. Only external
dependency is the `gh` CLI itself (jq is no longer required; this script
does its own JSON parsing and table formatting).

Usage:
    python gh-project-status.py -p <PROJECT_NUMBER> -o <OWNER> [-l LIMIT] [-d]

Flags:
    -p  GitHub Project number (see: gh project list --owner <OWNER>)   [required]
    -o  Project owner login, or @me for the current user               [required]
    -l  Max items to fetch (default: 500)                              [optional]
    -d  Include "Done" items (excluded by default)                     [optional]

Examples:
    python gh-project-status.py -p 7 -o sparsileg
    python gh-project-status.py -p 7 -o sparsileg -l 1000
    python gh-project-status.py -p 7 -o sparsileg -d   # include Done items too

Requires: gh (with the "project" auth scope — run
`gh auth refresh -s project` if you get a permissions error) on PATH.
"""

import argparse
import json
import shutil
import subprocess
import sys


def die(message: str, code: int = 1) -> None:
    print(f"Error: {message}", file=sys.stderr)
    sys.exit(code)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="List GitHub Project items with their Status column.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "-p", dest="project_number", required=True,
        help="GitHub Project number (see: gh project list --owner <OWNER>)",
    )
    parser.add_argument(
        "-o", dest="owner", required=True,
        help="Project owner login, or @me for the current user",
    )
    parser.add_argument(
        "-l", dest="limit", type=int, default=500,
        help="Max items to fetch (default: 500)",
    )
    parser.add_argument(
        "-d", dest="include_done", action="store_true",
        help='Include "Done" items (excluded by default)',
    )
    return parser.parse_args()


def fetch_items(project_number: str, owner: str, limit: int) -> list[dict]:
    if shutil.which("gh") is None:
        die('gh (GitHub CLI) not found on PATH.')

    cmd = [
        "gh", "project", "item-list", project_number,
        "--owner", owner,
        "--format", "json",
        "--limit", str(limit),
    ]
    try:
        result = subprocess.run(
            cmd, capture_output=True, text=True, check=True,
        )
    except subprocess.CalledProcessError as e:
        die(f"gh command failed:\n{e.stderr.strip()}")

    try:
        data = json.loads(result.stdout)
    except json.JSONDecodeError as e:
        die(f"Failed to parse gh output as JSON: {e}")

    return data.get("items", [])


def build_rows(items: list[dict], include_done: bool) -> list[tuple[str, str, str]]:
    rows = []
    for item in items:
        status = item.get("status") or "No Status"
        if not include_done and status.strip().lower() == "done":
            continue
        number = item.get("content", {}).get("number")
        number_str = str(number) if number is not None else " "
        title = item.get("title", "")
        rows.append((number_str, status, title))

    # Sort by status; items with no status sort last, matching the
    # original script's `zzz_no_status` fallback sort key.
    def sort_key(row: tuple[str, str, str]) -> str:
        status = row[1]
        return "zzz_no_status" if status == "No Status" else status

    rows.sort(key=sort_key)
    return rows


def print_table(rows: list[tuple[str, str, str]]) -> None:
    header = ("NUMBER", "STATUS", "TITLE")
    all_rows = [header] + rows

    col_widths = [
        max(len(row[i]) for row in all_rows)
        for i in range(3)
    ]

    for row in all_rows:
        line = "  ".join(
            cell.ljust(col_widths[i]) for i, cell in enumerate(row)
        )
        print(line.rstrip())


def main() -> None:
    args = parse_args()
    items = fetch_items(args.project_number, args.owner, args.limit)
    rows = build_rows(items, args.include_done)
    print_table(rows)


if __name__ == "__main__":
   main()
