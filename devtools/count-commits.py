#!/usr/bin/env python3

# run 'py <path>/count-commits.py' from Windows
# run '<path>/count-commits.py' from Linux

import subprocess
import sys
from pathlib import Path

invocation_dir = Path.cwd()

result = subprocess.run(
    ["git", "rev-list", "--count", "HEAD"],
    cwd=invocation_dir,
    capture_output=True,
    text=True,
)

if result.returncode != 0:
    print(result.stderr, end="", file=sys.stderr)
    sys.exit(result.returncode)

print(result.stdout, end="")

# ----------------------------------------------------------------------
# ----------------------------------------------------------------------
# ----------------------------------------------------------------------
