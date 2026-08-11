#!/usr/bin/env python3
"""
css_audit.py — Cross-references CSS class/id selectors defined in Photyx's
stylesheets against their actual usage across the Svelte frontend and Rust
backend source.

Answers three questions:
  1. Which selectors are defined in CSS but never referenced anywhere?
  2. Which classes/ids are referenced in code but defined in no CSS file?
  3. Which class/id names are constructed dynamically at runtime and
     therefore can't be verified automatically?

This is a static, regex-based heuristic — not a full CSS/JS/Svelte parser.
Notes on known limitations:
  - Nested @keyframes percentage selectors (0%, 50%, from, to) are not
    classes/ids and are naturally skipped since they don't start with
    '.' or '#'.
  - Hex colors immediately after '#' (e.g. #aaaa33) are excluded from
    id-selector extraction via length/hex-digit filtering, but this is
    a heuristic, not a full CSS tokenizer.
  - Dynamic class construction (`class="foo {bar}"`, template literals,
    `class={someExpr}`) cannot be resolved to concrete class names —
    those cases are collected separately for manual review rather than
    silently guessed at or silently ignored.
"""

import argparse
import re
import sys
from pathlib import Path
from collections import defaultdict

HEX_COLOR_RE = re.compile(r'^[0-9a-fA-F]{3}$|^[0-9a-fA-F]{4}$|^[0-9a-fA-F]{6}$|^[0-9a-fA-F]{8}$')

CLASS_TOKEN_RE = re.compile(r'\.([a-zA-Z_-][a-zA-Z0-9_-]*)')
ID_TOKEN_RE    = re.compile(r'#([a-zA-Z_-][a-zA-Z0-9_-]*)')

# ── Source-side usage patterns ───────────────────────────────────────────

CLASS_ATTR_RE           = re.compile(r'\bclass\s*=\s*(["\'])(.*?)\1', re.DOTALL)
# class={expr}, no quotes — tolerates one level of nested {...} so a
# template-literal expression like {`tp-${field.key}`} captures in full
# rather than truncating at the inner `${field.key}`'s closing brace.
CLASS_ATTR_BARE_RE      = re.compile(r'\bclass\s*=\s*\{((?:[^{}]|\{[^{}]*\})*)\}')
CLASSNAME_ATTR_RE       = re.compile(r'\bclassName\s*=\s*(["\'])(.*?)\1', re.DOTALL)
CLASS_DIRECTIVE_RE      = re.compile(r'\bclass:([a-zA-Z_-][a-zA-Z0-9_-]*)')
CLASSLIST_RE            = re.compile(r'classList\.(?:add|remove|toggle)\(\s*(["\'])(.*?)\1(?:\s*,\s*(["\'])(.*?)\3)?')
ID_ATTR_STATIC_RE       = re.compile(r'\bid\s*=\s*(["\'])([a-zA-Z_-][a-zA-Z0-9_-]*)\1')
ID_ATTR_DYNAMIC_RE      = re.compile(r'\bid\s*=\s*\{((?:[^{}]|\{[^{}]*\})*)\}')
DYNAMIC_EXPR_IN_ATTR_RE = re.compile(r'\{[^}]*\}')

GET_BY_ID_RE    = re.compile(r'getElementById\(\s*(["\'])([a-zA-Z_-][a-zA-Z0-9_-]*)\1\s*\)')
GET_BY_CLASS_RE = re.compile(r'getElementsByClassName\(\s*(["\'])([a-zA-Z_-][a-zA-Z0-9_-]*)\1\s*\)')
QUERY_SEL_RE    = re.compile(r'\b(?:querySelector|querySelectorAll|closest|matches)\(\s*(["\'])(.*?)\1')
SET_ATTR_RE     = re.compile(r'setAttribute\(\s*(["\'])(class|id)\1\s*,\s*(["\'])(.*?)\3', re.DOTALL)

# Matches a template-literal / prefix-style dynamic class or id, e.g.
# `tp-${field.key}` — captures the literal leading segment as a "prefix"
# so a selector like `tp-profile-dropdown` defined in CSS can be matched
# against it even though the exact runtime value can't be resolved statically.
TEMPLATE_PREFIX_RE = re.compile(r'^([a-zA-Z_-][a-zA-Z0-9_-]*-)\$\{')


def _extract_literals_and_prefixes(expr, used_set, prefix_set):
    """Given the raw contents of a dynamic {expr} attribute or template
    literal, pull out any plain string-literal tokens (added directly to
    used_set) and any leading `name-${...}` prefixes (added to prefix_set
    for later prefix-matching against CSS-defined selectors)."""
    for lit_m in re.finditer(r'(["\'])([a-zA-Z_-][a-zA-Z0-9_-]*)\1', expr):
        used_set.add(lit_m.group(2))
    for tmpl_m in re.finditer(r'`([^`]*)`', expr):
        pm = TEMPLATE_PREFIX_RE.match(tmpl_m.group(1))
        if pm:
            prefix_set.add(pm.group(1))


def _add_tokens(raw, used_set):
    for tok in raw.split():
        tok = tok.strip()
        if tok:
            used_set.add(tok)


# ── CSS-side extraction ──────────────────────────────────────────────────

def strip_css_comments(text: str) -> str:
    return re.sub(r'/\*.*?\*/', '', text, flags=re.DOTALL)


def extract_css_selectors(css_text: str):
    """Return (classes, ids) sets, extracted only from selector positions
    (text before each '{'), not from property values, to avoid false
    positives from things like url() or hex colors."""
    text = strip_css_comments(css_text)
    classes = set()
    ids = set()

    # Walk brace-delimited blocks. This is a simple non-nesting scan;
    # nested @keyframes blocks still work fine here because keyframe
    # selectors (0%, 50%, from, to) never start with '.' or '#', so
    # even imperfect block boundaries don't introduce false positives.
    pos = 0
    while True:
        brace_idx = text.find('{', pos)
        if brace_idx == -1:
            break
        selector_text = text[pos:brace_idx]
        pos = brace_idx + 1

        for m in CLASS_TOKEN_RE.finditer(selector_text):
            classes.add(m.group(1))
        for m in ID_TOKEN_RE.finditer(selector_text):
            name = m.group(1)
            if not HEX_COLOR_RE.match(name):
                ids.add(name)

    return classes, ids


def load_css_definitions(css_dirs):
    """css_dirs: list of Path objects to scan for *.css files.
    Returns (class_origins, id_origins): dicts mapping selector name ->
    list of files it was found defined in, so the report can cite where
    each definition lives."""
    class_origins = defaultdict(list)
    id_origins = defaultdict(list)

    for d in css_dirs:
        if not d.is_dir():
            continue
        for f in sorted(d.rglob('*.css')):
            text = f.read_text(encoding='utf-8', errors='replace')
            classes, ids = extract_css_selectors(text)
            for c in classes:
                class_origins[c].append(str(f))
            for i in ids:
                id_origins[i].append(str(f))

    return class_origins, id_origins


# ── Source-side extraction ────────────────────────────────────────────────

def extract_source_usage(text, filepath, used_classes, used_ids,
                          class_prefixes, id_prefixes, dynamic_notes):
    # class="literal" / class="{dynamic}" mixed content
    for m in CLASS_ATTR_RE.finditer(text):
        raw = m.group(2)
        if DYNAMIC_EXPR_IN_ATTR_RE.search(raw):
            literal_part = DYNAMIC_EXPR_IN_ATTR_RE.sub(' ', raw)
            _add_tokens(literal_part, used_classes)
            line_no = text.count('\n', 0, m.start()) + 1
            dynamic_notes.append((filepath, line_no, 'class', raw.strip()))
        else:
            _add_tokens(raw, used_classes)

    # class={expr} — bare expression, no quotes. Can't resolve statically.
    for m in CLASS_ATTR_BARE_RE.finditer(text):
        expr = m.group(1)
        _extract_literals_and_prefixes(expr, used_classes, class_prefixes)
        line_no = text.count('\n', 0, m.start()) + 1
        dynamic_notes.append((filepath, line_no, 'class', expr.strip()))

    # className="literal" — component prop passthrough
    for m in CLASSNAME_ATTR_RE.finditer(text):
        _add_tokens(m.group(2), used_classes)

    # class:name Svelte directive (always a literal class name)
    for m in CLASS_DIRECTIVE_RE.finditer(text):
        used_classes.add(m.group(1))

    # classList.add/remove/toggle('name'[, 'name2'])
    for m in CLASSLIST_RE.finditer(text):
        used_classes.add(m.group(2))
        if m.group(4):
            used_classes.add(m.group(4))

    # id="literal"
    for m in ID_ATTR_STATIC_RE.finditer(text):
        used_ids.add(m.group(2))

    # id={expr} — dynamic, can't resolve statically
    for m in ID_ATTR_DYNAMIC_RE.finditer(text):
        expr = m.group(1)
        _extract_literals_and_prefixes(expr, used_ids, id_prefixes)
        line_no = text.count('\n', 0, m.start()) + 1
        dynamic_notes.append((filepath, line_no, 'id', expr.strip()))

    # getElementById / getElementsByClassName
    for m in GET_BY_ID_RE.finditer(text):
        used_ids.add(m.group(2))
    for m in GET_BY_CLASS_RE.finditer(text):
        used_classes.add(m.group(2))

    # querySelector / querySelectorAll / closest / matches — parse the
    # selector string for .class and #id tokens
    for m in QUERY_SEL_RE.finditer(text):
        sel = m.group(2)
        for cm in CLASS_TOKEN_RE.finditer(sel):
            used_classes.add(cm.group(1))
        for im in ID_TOKEN_RE.finditer(sel):
            name = im.group(1)
            if not HEX_COLOR_RE.match(name):
                used_ids.add(name)

    # setAttribute('class'|'id', '...')
    for m in SET_ATTR_RE.finditer(text):
        kind, value = m.group(2), m.group(4)
        if kind == 'class':
            _add_tokens(value, used_classes)
        else:
            used_ids.add(value.strip())

    # Bare string literals anywhere (e.g. `{ cls: 'status-ok' }`, a
    # levelClass() return value) — weaker evidence, kept separate so the
    # report can present it as "appears as a string literal" rather than
    # a confirmed structural usage.
    for m in re.finditer(r'(["\'])([a-zA-Z_-][a-zA-Z0-9_-]{2,})\1', text):
        pass  # collected at the classification stage against CSS-defined names


def scan_source_dirs(source_dirs, extensions):
    used_classes = set()
    used_ids = set()
    class_prefixes = set()
    id_prefixes = set()
    dynamic_notes = []  # (file, line, 'class'|'id', raw_snippet)
    all_text_blobs = []  # (file, text) — kept for the string-literal evidence pass

    for d in source_dirs:
        if not d.is_dir():
            continue
        for ext in extensions:
            for f in sorted(d.rglob(f'*{ext}')):
                text = f.read_text(encoding='utf-8', errors='replace')
                extract_source_usage(text, str(f), used_classes, used_ids,
                                      class_prefixes, id_prefixes, dynamic_notes)
                all_text_blobs.append((str(f), text))

    return used_classes, used_ids, class_prefixes, id_prefixes, dynamic_notes, all_text_blobs


# ── Reporting ──────────────────────────────────────────────────────────────

def fnames(files):
    return ', '.join(Path(f).name for f in sorted(set(files)))


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('project_root', type=Path, help='Path to the Photyx project root')
    args = parser.parse_args()

    root = args.project_root
    css_dirs = [
        root / 'static' / 'css',
        root / 'static' / 'themes',
    ]
    svelte_src_dirs = [root / 'src-svelte']
    tauri_src_dirs  = [root / 'src-tauri' / 'src']

    class_origins, id_origins = load_css_definitions(css_dirs)
    theme_dir = str(root / 'static' / 'themes')

    (used_classes, used_ids, class_prefixes, id_prefixes,
     dynamic_notes, svelte_blobs) = scan_source_dirs(svelte_src_dirs, ['.svelte', '.ts', '.js', '.html'])

    # Rust side: same literal-attribute patterns, in case HTML gets built
    # server-side (e.g. report export). Merge results into the same sets.
    (rust_classes, rust_ids, rust_class_prefixes, rust_id_prefixes,
     rust_dynamic, rust_blobs) = scan_source_dirs(tauri_src_dirs, ['.rs'])
    used_classes    |= rust_classes
    used_ids        |= rust_ids
    class_prefixes  |= rust_class_prefixes
    id_prefixes     |= rust_id_prefixes
    dynamic_notes   += rust_dynamic
    all_blobs = svelte_blobs + rust_blobs

    # ── Classify every CSS-defined class into one of four buckets ─────────
    truly_unused = []     # 1a
    prefix_matched = []   # 1b
    literal_matched = []  # 1c
    theme_only_list = []  # 1d

    for sel in sorted(class_origins):
        files = class_origins[sel]
        is_theme_only = all(theme_dir in f for f in files)

        if sel in used_classes:
            continue  # confirmed used — not reported

        prefix_hit = next((p for p in class_prefixes if sel.startswith(p)), None)
        if prefix_hit:
            prefix_matched.append((sel, prefix_hit, files))
            continue

        literal_hit = any(
            re.search(r'(["\'])' + re.escape(sel) + r'\1', text)
            for _, text in all_blobs
        )
        if literal_hit:
            literal_matched.append((sel, files))
            continue

        if is_theme_only:
            theme_only_list.append((sel, files))
            continue

        truly_unused.append((sel, files))

    # ── Section 1: dead / suspect CSS ──────────────────────────────────────
    print('=' * 78)
    print('1a. LIKELY UNUSED — no evidence of any kind found in source')
    print('=' * 78)
    if not truly_unused:
        print('  (none found)')
    else:
        for sel, files in truly_unused:
            print(f'  {sel:<42} defined in: {fnames(files)}')

    print()
    print('=' * 78)
    print('1b. PREFIX-MATCHED — may be constructed dynamically, e.g. `tp-${x}`')
    print('=' * 78)
    if not prefix_matched:
        print('  (none found)')
    else:
        for sel, prefix, files in prefix_matched:
            print(f'  {sel:<42} matches prefix `{prefix}…` — defined in: {fnames(files)}')

    print()
    print('=' * 78)
    print('1c. NAME APPEARS AS A STRING LITERAL IN SOURCE — verify manually')
    print('    (e.g. { cls: \'status-ok\' } or a function returning class names)')
    print('=' * 78)
    if not literal_matched:
        print('  (none found)')
    else:
        for sel, files in literal_matched:
            print(f'  {sel:<42} defined in: {fnames(files)}')

    print()
    print('=' * 78)
    print('1d. DEFINED ONLY IN THEME FILES — likely leftovers from another project')
    print('=' * 78)
    if not theme_only_list:
        print('  (none found)')
    else:
        for sel, files in theme_only_list:
            print(f'  {sel:<42} defined in: {fnames(files)}')

    # ── Section 2: referenced in code, defined nowhere ─────────────────────
    print()
    print('=' * 78)
    print('2. REFERENCED IN SOURCE BUT DEFINED IN NO CSS FILE')
    print('   (possible typo, or a reference to a removed/renamed style)')
    print('=' * 78)
    undefined_classes = sorted(used_classes - set(class_origins))
    undefined_ids     = sorted(used_ids - set(id_origins))
    if not undefined_classes and not undefined_ids:
        print('  (none found)')
    else:
        for c in undefined_classes:
            print(f'  .{c}')
        for i in undefined_ids:
            print(f'  #{i}')

    # ── Section 3: dynamic references needing manual review ────────────────
    print()
    print('=' * 78)
    print('3. DYNAMIC / UNRESOLVABLE REFERENCES — manual review')
    print('=' * 78)
    if not dynamic_notes:
        print('  (none found)')
    else:
        for filepath, line_no, kind, snippet in dynamic_notes:
            print(f'  {Path(filepath).name}:{line_no}  [{kind}]  {snippet}')

    # ── Summary ──────────────────────────────────────────────────────────
    print()
    print('=' * 78)
    print('SUMMARY')
    print('=' * 78)
    print(f'  Likely unused:              {len(truly_unused)}')
    print(f'  Possibly used (prefix):     {len(prefix_matched)}')
    print(f'  Possibly used (literal):    {len(literal_matched)}')
    print(f'  Theme-only (check origin):  {len(theme_only_list)}')
    print(f'  Referenced but undefined:   {len(undefined_classes) + len(undefined_ids)}')
    print(f'  Dynamic references to check: {len(dynamic_notes)}')


if __name__ == '__main__':
    main()
