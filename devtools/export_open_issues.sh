#!/bin/bash
# All open issues, as JSON with the fields you probably care about
#
# Usage: ./export_issues.sh <owner>/<repo>
# Example: ./export_issues.sh sparsileg/photyx

set -euo pipefail

if [ $# -ne 1 ]; then
  echo "Usage: $0 <owner>/<repo>" >&2
  exit 1
fi

REPO="$1"

gh issue list --repo "$REPO" --limit 500 \
  --json number,title,body,labels,state,createdAt \
  > issues.json

echo "Wrote $(jq length issues.json) issues to issues.json"#!/bin/bash
