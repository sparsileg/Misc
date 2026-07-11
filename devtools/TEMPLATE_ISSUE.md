# Short, specific issue title describing the problem or change

Size: S
Labels: bug, area-tag

## Problem

Symptom: One or two sentences describing what the user actually observes —
the visible failure, not the cause. Written so someone with no source
knowledge understands what's wrong before any code is mentioned.

Traced the full path: `entry_point_function()` (`path/to/file.ext` ~line N)
→ `next_call()` (`other_file.ext`) → wherever the root cause lives. If there
are multiple compounding causes, number them:

1. **Short bolded label for cause one.** Explanation with specific function
   names, file paths, and line numbers where known. Quote the relevant
   behavior or comment from source if it clarifies the mismatch between
   intended and actual behavior.

2. **Short bolded label for cause two.** Same level of specificity. If this
   cause references a documented contract (a spec section, a code comment
   stating an invariant), quote or cite it directly so the gap is provable,
   not asserted.

Optional closing paragraph: related findings noticed while tracing this
issue that are lower priority, duplicate logic spotted nearby, or minor
inconsistencies worth a one-line mention but not their own issue.

## Approach

State whether this needs discussion before coding, or is mechanical enough
to proceed directly. For anything with more than one reasonable fix, lay out
the options and give a recommendation rather than silently picking one:

- **Option / fix area one:** what changes, where, and why this is the
  recommended approach (or why it's presented as a choice for the reader to
  make).
- **Option / fix area two:** same treatment. Flag any open question that
  needs an explicit decision before code is written.
- Note any sequencing dependency on other issues (must land after/before
  issue NN because they touch the same code or the same semantic decision).

## Constraints (methodology)

- Fresh uploads of the specific files this issue touches before any change
  — file paths listed explicitly, not "the usual files."
- One change at a time if the issue has multiple separable deltas; state
  the order.
- Any testing, verification, or regression requirements specific to this
  issue (e.g. "verify against the benchmark session," "test on both
  platforms," "add unit tests alongside the fix").
- Anything else project-specific that constrains how the fix should be
  delivered (discussion-first, BEFORE/AFTER blocks, full-file replacement
  threshold, etc.) — restate only what's relevant to this issue, not the
  full standing methodology.

## Acceptance criteria

- Concrete, checkable statement of the fixed behavior — not "works
  correctly" but the specific input/output or before/after state.
- One line per distinct guarantee the fix must provide.
- Include a regression guarantee where relevant ("existing X behavior is
  unchanged").
