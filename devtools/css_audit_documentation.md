# css_audit.py — CSS Selector Cross-Reference Tool

## Purpose

`css_audit.py` cross-references every CSS class and id selector defined in
Photyx's stylesheets against their actual usage in the source code. It answers
three questions:

1. Which selectors are defined in CSS but never referenced anywhere? (dead CSS)
2. Which classes/ids are referenced in code but defined in no CSS file?
   (typos, or references to removed styles)
3. Which class/id names are constructed dynamically at runtime and therefore
   can't be verified automatically? (manual review list)

It is a **static, regex-based heuristic** — not a full CSS/JS/Svelte parser.
It is designed to *sort evidence into confidence buckets* rather than give a
single yes/no answer, so that the "likely unused" list is trustworthy and
everything uncertain is surfaced for human judgment instead of silently
guessed at.

## Usage

```bash
python3 css_audit.py /path/to/Photyx
```

The single argument is the project root. No dependencies beyond the Python 3
standard library. Output goes to stdout; redirect to a file if desired:

```bash
python3 css_audit.py ~/projects/Photyx > css_audit_report.txt
```

## What It Scans

| Role | Locations | File types |
|---|---|---|
| CSS definitions | `static/css/`, `static/themes/` | `*.css` |
| Source usage | `src-svelte/` (recursive) | `.svelte`, `.ts`, `.js`, `.html` |
| Source usage | `src-tauri/src/` (recursive) | `.rs` (in case Rust emits HTML, e.g. report export) |

### Selector extraction (CSS side)

Class and id names are extracted only from **selector positions** — the text
preceding each `{` block — never from property values. This avoids false
positives from hex colors (`#aaaa33`) and `url()` values. Hex-color-shaped
tokens after `#` are additionally filtered by a length/hex-digit check.
`@keyframes` frame selectors (`0%`, `from`, `to`) are naturally skipped since
they don't begin with `.` or `#`.

### Usage detection (source side)

The following patterns count as a reference:

| Pattern | Example | Notes |
|---|---|---|
| Quoted class attribute | `class="kw-btn kw-btn-write"` | Also matches inside TS template strings that build HTML |
| `className` prop | `className="tp-profile-dropdown"` | Component prop passthrough (e.g. to `Dropdown`) |
| Mixed literal + dynamic | `class="active {isOpen ? 'open' : ''}"` | Literal tokens are extracted; the dynamic portion is logged for manual review |
| Bare dynamic expression | `class={someExpr}` or `class={`tp-${field.key}`}` | Any plain string-literal tokens inside are captured directly; template-literal prefixes like `tp-${...}` are captured as a *prefix* and matched against CSS-defined selectors that start with it |
| `class:name` Svelte directive | `class:active={isActive}` | The class name itself is always a literal, regardless of the condition |
| `classList.add/remove/toggle(...)` | `classList.add('kw-highlight')` | Both arguments checked (`toggle` can take a second class name) |
| `id="literal"` | `id="ag-root"` | — |
| `id={expr}` | `id={dynamicId}` | Dynamic; logged for manual review, same prefix/literal extraction as class |
| `getElementById(...)` / `getElementsByClassName(...)` | `document.getElementById('sr-root')` | — |
| `querySelector` / `querySelectorAll` / `closest` / `matches` | `el.closest('.status-error')` | The selector string is parsed for `.class` and `#id` tokens |
| `setAttribute('class'\|'id', ...)` | `el.setAttribute('class', 'lv-warn')` | — |

Bare string literals elsewhere in the source (e.g. `{ cls: 'status-ok' }`, a
`levelClass()` return value) are **not** treated as confirmed usage on their
own — they're checked separately as weaker evidence (see bucket 1c below),
since a string literal could be unrelated text that happens to match a CSS
class name.

## Report Format

Output is organized into three numbered sections plus a summary.

### Section 1 — Defined in CSS, not confirmed used

Split into four confidence buckets, from strongest to weakest evidence of
being genuinely dead:

- **1a. Likely unused** — no evidence of any kind found in source. This is
  the trustworthy "safe to investigate deleting" list.
- **1b. Prefix-matched** — the selector's name matches a dynamically
  constructed prefix seen in source (e.g. a selector `tp-header` matching a
  `` `tp-${field.key}` `` template literal found somewhere). May be
  constructed at runtime; needs a human to confirm.
- **1c. String-literal evidence** — the selector's name appears as an
  ordinary quoted string literal somewhere in the source, outside of any
  recognized class/id attribute pattern (e.g. inside an object literal or a
  function's return value). Weaker evidence than a real usage site, but
  strong enough to disqualify it from the "likely unused" bucket.
- **1d. Theme-only** — defined only inside `static/themes/*.css`, with zero
  references anywhere in the app source. Frequently indicates leftover
  styling from a different project/template that was never adopted.

### Section 2 — Referenced but not defined

Classes and ids used somewhere in source but not defined in any scanned CSS
file. Usually a typo, or a reference to a style that was renamed or removed
without updating every call site.

### Section 3 — Dynamic / unresolvable references

Every place a class or id is constructed dynamically in a way the script
can't fully resolve to a literal name — logged as `file:line [kind] snippet`
for manual review. This is not a defect list; it's a "here's what the script
couldn't verify one way or the other" list.

### Summary

Counts for each bucket above, printed at the end for a quick read without
scrolling back through the full report.

## Known Limitations

- This is a **regex-based heuristic**, not a real parser. It handles one
  level of nested `{...}` inside a dynamic attribute (so template literals
  like `` class={`tp-${field.key}`} `` capture correctly), but does not
  handle arbitrarily deep nesting.
- Prefix matching and string-literal matching are both **weaker evidence**
  than a confirmed structural usage site — treat buckets 1b/1c/1d and
  Section 3 as a review list, not a verdict.
- A selector that's genuinely unused but happens to share a name with an
  unrelated string literal elsewhere in the codebase will be misclassified
  into 1c instead of 1a. This is a deliberate false-negative bias — the tool
  is built to avoid ever telling you something is dead when it might not be.
- Re-run against a **fresh, non-stale checkout** before acting on results —
  the report reflects exactly the tree it was pointed at.
