# Project Development Guide

*Read before starting a new project. Reread during development and debugging.
Purpose: prevent the class of bugs and rework that comes from designing as
you go rather than deciding up front.*

---

## 1. Write a short architecture doc before any code

Even rough. Cover: data model, single source of truth, storage boundaries,
what's in scope for v1 vs. later. Cheap to write now; expensive to
reconstruct after months of undirected code.

- **One source of truth per piece of state.** If a second storage layer or
  cache is introduced, it either replaces the first or has an explicit,
  written reason to coexist temporarily (e.g. "dual-write until cutover in
  Issue X") — never an open-ended "just in case."

## 2. Ask "what bounds this?" at design time, not after it breaks

For anything that grows, compounds, or repeats — intervals, retries,
caches, lists, recursive/multiplicative math — decide the ceiling before
writing the function. "What stops this from growing forever" is a
design-time question. Finding out via a `NaN` in production is a debugging
session that shouldn't have to happen.

## 3. When a new mechanism overlaps an old one, remove the old one

Don't let two systems solve the same problem side by side "temporarily."
If a replacement is landing, the old path gets removed or deprecated in
the same change — not left running until someone notices.

## 4. Every issue follows one structure

- Brief, descriptive title (let the tracker assign numbers)
- **Labels** — category tags (ux, enhancement, data-integrity, etc.)
- **Files** — exact files expected to change
- **Description** — lead with *why this matters* in plain terms, then the
  mechanism/root cause
- **Proposed Fix** — specific enough to implement from; include code
  snippets where useful. If it proposes removing or renaming anything,
  specify the grep/audit to confirm no other call sites first
- **Validation Tests** — concrete and runnable. For tests involving
  persisted state, specify the *setup* steps as explicitly as the
  assertion — not just the expected outcome

## 5. Never guess at file or call-site state

Re-view a file before editing it, especially one touched more than once
this session. Grep for other callers before removing or renaming anything
that isn't obviously local. Cheap to check, expensive to guess wrong.

## 6. Validate before closing

An issue isn't done when the code is written — it's done when its own
validation tests have actually been run and reported back, not assumed.

## 7. Full code audit during Beta

Before calling a project stable, do a dedicated audit pass across the
whole codebase — not issue-by-issue, but looking for the class of thing
that only shows up in aggregate (dead code, inconsistent patterns,
missing bounds, duplicated logic). Where possible, have this done by:
- a newer/better model generation than the one used to build it, or
- a higher-functioning model of the *same* generation, if a newer
  generation isn't available.

The point is a second, less-familiar set of eyes — the same reason human
teams don't let an author review their own PR.

---

*None of this requires more skill than last time — it requires deciding
these things before they become bugs instead of after.*
