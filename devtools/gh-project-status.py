#!/usr/bin/env python3
"""
gh-project-status.py — lists all items in a GitHub Project with their
Status column (e.g. To Do / In Progress / Deferred / Done), sorted by
status, and prints a formatted table to stdout. Excludes "Done" items
by default. Optionally prints each surviving item's body text and/or
comment thread below the table.

Cross-platform replacement for gh-project-status.sh — works identically
via `python`/`python3` on Linux, Windows, and macOS. Only external
dependency is the `gh` CLI itself (jq is no longer required; this script
does its own JSON parsing and table formatting).

Usage:
    python gh-project-status.py -p <PROJECT_NUMBER> -o <OWNER> [-l LIMIT] [-d] [-b] [-c]

Flags:
    -p  GitHub Project number (see: gh project list --owner <OWNER>)   [required]
    -o  Project owner login, or @me for the current user               [required]
    -l  Max items to fetch (default: 500)                              [optional]
    -d  Include "Done" items (excluded by default)                     [optional]
    -b  Include each item's body text below the table                  [optional]
    -c  Include each item's comments below the table                   [optional]

Examples:
    python gh-project-status.py -p 7 -o sparsileg
    python gh-project-status.py -p 7 -o sparsileg -l 1000
    python gh-project-status.py -p 7 -o sparsileg -d        # include Done items too
    python gh-project-status.py -p 7 -o sparsileg -b        # include body text
    python gh-project-status.py -p 7 -o sparsileg -b -c     # include body + comments

Requires: gh (with the "project" auth scope — run
`gh auth refresh -s project` if you get a permissions error) on PATH.

Note on -c: body text comes free with the project item-list payload, but
comments do not — fetching them costs one additional `gh issue view` call
per item that survives the status filter (not per item in the raw -l
fetch). A failed fetch for one item is logged as a warning and does not
abort the rest of the run.
"""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
import textwrap
from dataclasses import dataclass


def die(message: str, code: int = 1) -> None:
    print(f"Error: {message}", file=sys.stderr)
    sys.exit(code)


def warn(message: str) -> None:
    print(f"Warning: {message}", file=sys.stderr)


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
    parser.add_argument(
        "-b", "--body", dest="include_body", action="store_true",
        help="Include each item's body text below the table",
    )
    parser.add_argument(
        "-c", "--comments", dest="include_comments", action="store_true",
        help="Include each item's comments below the table (one extra gh call per item)",
    )
    return parser.parse_args()


@dataclass
class Row:
    number_str: str
    status: str
    title: str
    body: str
    repository: str | None
    content_type: str


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


def build_rows(items: list[dict], include_done: bool) -> list[Row]:
    rows = []
    for item in items:
        status = item.get("status") or "No Status"
        if not include_done and status.strip().lower() == "done":
            continue
        content = item.get("content", {})
        number = content.get("number")
        number_str = str(number) if number is not None else " "
        title = item.get("title", "")
        rows.append(Row(
            number_str=number_str,
            status=status,
            title=title,
            body=content.get("body") or "",
            repository=content.get("repository"),
            content_type=content.get("type", ""),
        ))

    # Sort by status; items with no status sort last, matching the
    # original script's `zzz_no_status` fallback sort key.
    def sort_key(row: Row) -> str:
        return "zzz_no_status" if row.status == "No Status" else row.status

    rows.sort(key=sort_key)
    return rows


def print_table(rows: list[Row]) -> None:
    header = ("NUMBER", "STATUS", "TITLE")
    all_rows = [header] + [(r.number_str, r.status, r.title) for r in rows]

    col_widths = [
        max(len(row[i]) for row in all_rows)
        for i in range(3)
    ]

    for row in all_rows:
        line = "  ".join(
            cell.ljust(col_widths[i]) for i, cell in enumerate(row)
        )
        print(line.rstrip())


def fetch_comments(number_str: str, repository: str) -> list[dict] | None:
    cmd = [
        "gh", "issue", "view", number_str,
        "--repo", repository,
        "--json", "comments",
    ]
    try:
        result = subprocess.run(
            cmd, capture_output=True, text=True, check=True,
        )
    except subprocess.CalledProcessError as e:
        warn(f"could not fetch comments for #{number_str}: {e.stderr.strip()}")
        return None

    try:
        data = json.loads(result.stdout)
    except json.JSONDecodeError as e:
        warn(f"could not parse comments for #{number_str}: {e}")
        return None

    return data.get("comments", [])


def print_details(rows: list[Row], include_body: bool, include_comments: bool) -> None:
    separator = "=" * 60
    for row in rows:
        print(separator)
        print(f"#{row.number_str.strip()}  {row.status}  {row.title}")

        if include_body:
            print("  Body:")
            body = row.body.strip()
            print(textwrap.indent(body, "    ") if body else "    (no body)")

        if include_comments:
            if row.content_type != "Issue" or not row.repository or not row.number_str.strip():
                print("  Comments: (unavailable — not a linked issue)")
                continue

            comments = fetch_comments(row.number_str.strip(), row.repository)
            if comments is None:
                print("  Comments: (failed to fetch — see warning above)")
            elif not comments:
                print("  Comments: (none)")
            else:
                print(f"  Comments ({len(comments)}):")
                for c in comments:
                    author = c.get("author", {}).get("login", "unknown")
                    created = c.get("createdAt", "")
                    body = (c.get("body") or "").strip()
                    print(f"    [{author}, {created}]")
                    print(textwrap.indent(body, "      "))


def main() -> None:
    args = parse_args()
    items = fetch_items(args.project_number, args.owner, args.limit)
    rows = build_rows(items, args.include_done)
    print_table(rows)

    if args.include_body or args.include_comments:
        print_details(rows, args.include_body, args.include_comments)


if __name__ == "__main__":
   main()

### ----------------------------------------------------------------------
### ----------------------------------------------------------------------
### ----------------------------------------------------------------------
