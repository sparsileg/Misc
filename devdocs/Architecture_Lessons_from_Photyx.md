# Architecture Lessons — Patterns for Scriptable Desktop Applications

*A project-agnostic pattern reference distilled from building a native-backend
(Rust) + web-frontend (JS/TS) desktop application with a full macro language,
parallel compute, and a plugin-based command surface. Written to be handed to a
future development partner — human or AI — at the start of a new project, before
architecture is settled.*

*The recurring theme: **decide the shape once, generate everything else from it.**
Most of the pain in this class of application comes from the same fact expressed
in four places and drifting in three of them.*

---

## Index

1. Make Every Function a Scriptable Command
2. Plugin/Trait Architecture with a Single Registry
3. One Command Definition, Every Entry Point
4. Declarative Side-Effect Signaling (Action Tokens)
5. Shared Mutable State — The Single-Lock Model and Its Ceiling
6. Parallelism with a User-Adjustable Thread Count
7. Bounded Residency and Background Prefetch
8. Measurement Bases and Validation Gates
9. Process Discipline That Paid For Itself

---

## 1. Make Every Function a Scriptable Command

**The single highest-leverage decision in the whole project.**

If the application does something, a script should be able to make it do that
thing. Not a curated "automation subset" — everything. Load, filter, compute,
export, navigate, configure. The moment a capability exists only behind a button,
it becomes untestable in batch, unreproducible by the user, and undemonstrable in
a bug report.

### Why it compounds

- **Reproducibility.** A user reporting a bug can send a ten-line script instead
  of a description of the clicks they made. This alone justifies the design.
- **Regression testing for free.** Scripts *are* integration tests. A validation
  suite becomes a directory of scripts and expected outputs.
- **The UI gets a customer.** When the menu, the toolbar, and the quick-launch
  bar all execute scripts rather than calling functions directly, any bug in the
  script path is found immediately by ordinary use, not by the three people who
  use the console.
- **User-built workflows.** Users assemble sequences the developer never
  imagined, and never file a feature request for them.

### Language design notes

Line-oriented is enough, and it is the right amount of ambition. `CommandName
arg=value arg="quoted value"`, one command per line. Resist the pull toward a
real programming language; you want a *command language* with just enough
structure to be useful:

- **Named arguments only, case-insensitive names.** Positional arguments seem
  concise and become unreadable at four parameters and unmaintainable when one is
  inserted in the middle.
- **Variables with a sigil** (`$name`) and a braced form (`${name}`) for names
  containing characters outside the bare-identifier set. This is not
  hypothetical — the moment a variable name can be derived from external data
  (a file header key, a column name, an environment variable), hyphens and dots
  appear, and `$a-b` parses as subtraction unless the braced form exists. Design
  it in on day one; retrofitting it means auditing every script in the wild.
- **Flow control: conditionals and two loop forms.** A numeric range and an
  iterator over some domain-natural collection (files matching a pattern, rows,
  records). Nesting allowed. That is genuinely sufficient for the large majority
  of real user scripts.
- **Commands that produce a value write it into a well-known variable**, and this
  is documented as part of the command. The next line can consume it. This is far
  simpler than expression-level return values and covers nearly every real use.
  Be *consistent*: if one command stores its output path in a conventional
  variable, all of them must, and the convention gets a name in the docs.
- **An assertion command.** Scripts that halt loudly on a violated precondition
  are worth more than scripts that produce garbage.
- **A trace mode.** Echo each command with resolved arguments before execution.
  Debugging a macro without this is guesswork.
- **A comment character**, and decide up front whether inline (end-of-line)
  comments are supported. Either answer is fine; an undocumented answer is not.

### The trap

**Command syntax documentation drifts from implementation.** Every command needs
a name, an argument list, help text, and (if there is autocompletion) an argument
hint. If those live in four separate places maintained by discipline, they will
diverge — not "might," *will*, and the divergence is silent because nothing fails
when help text is stale.

**Fix it structurally.** Either derive help and hints from a single command
definition (see §3), or add a test that fails the build when a registered command
has no help entry and no hint entry. The test is a half-hour of work and it is
the difference between documentation that is trustworthy and documentation that
is decorative.

---

## 2. Plugin/Trait Architecture with a Single Registry

Define one interface — a trait, an abstract base, whatever the language offers —
that every operation implements. Register instances into one registry. Dispatch
by name through the registry. Never hand-write a dispatch `switch`.

```
trait Command {
    fn name(&self) -> &str;
    fn execute(&self, ctx: &mut Context, args: &Args) -> Result<Output>;
}
```

### What this buys

- **The registration site becomes a manifest.** One file, readable top to bottom,
  that lists everything the application can do. This is a genuinely useful
  artifact for onboarding and for auditing.
- **Adding a capability touches one place**, not a dispatcher plus an enum plus a
  frontend list plus a docs table.
- **Cross-cutting concerns get one implementation site.** Logging, timing,
  progress reporting, permission checks, and error wrapping happen in the
  dispatcher, once, for every command — rather than being copy-pasted into each
  and omitted from three.
- **An extension path exists without being built.** The same trait boundary that
  serves built-in native operations is the boundary a scripted, WASM, or
  dynamically-loaded plugin would implement later. Design the boundary now; ship
  only native implementations. The option costs nothing to preserve and is
  expensive to add retroactively.

### Conventions worth fixing early

- **Fully-qualified registration names** (`domain::action`, not bare `action`).
  Name collisions arrive later than you expect and are annoying to resolve once
  scripts in the wild depend on the short names.
- **Cleanup is the plugin's own responsibility, on every return path including
  errors.** If a command sets progress state, opens a handle, or takes a lock, it
  clears/releases it before returning — including on the error path. A command
  that returns early and leaves a stale progress indicator or a held resource is
  a bug that manifests three commands later in an unrelated place. Consider
  enforcing this with a guard/RAII wrapper rather than convention.
- **One file per command, grouped in a directory by domain.** Trivially
  navigable, minimal merge conflicts, and the file list is itself documentation.

---

## 3. One Command Definition, Every Entry Point

**This is the one to get right the first time. Getting it wrong is not fatal, but
it is a persistent, low-grade tax that touches every feature added afterward.**

A mature application accumulates entry points: a menu bar, a toolbar, a
right-click context menu, a console, a script runner, a pinnable quick-launch bar,
keyboard shortcuts. Each one needs to invoke the same underlying operations.

The naive path — each entry point wires itself to backend functions in its own
way — produces four subtly different call paths per capability. They diverge in
argument defaults, in error handling, in whether output reaches the console, in
whether the view refreshes afterward. Every new capability must be wired four
times, and bugs get fixed in the two paths the developer happened to test.

### The fix: one definition, generated surfaces

Define each command **once**, as data, with everything any entry point could need:

```
{
  name:            "ExportReport",
  args:            [ { name: "path", required: false, type: Path } ],
  help:            "Exports current results to a file.",
  hint:            "path=<path>",
  category:        "Export",
  script_only:     false,   // meaningful in a saved script?
  console_only:    false,   // interactive-session only?
  menu:            { path: ["File", "Export Report…"], accel: "Ctrl+E" },
  needs_selection: true,     // enable/disable state for UI surfaces
}
```

Then **every entry point is generated from that definition** rather than
hand-wired against it:

- The menu bar builds its items from entries carrying menu metadata.
- The console's autocomplete and help both read the same table.
- The quick-launch bar stores *script text* and executes it through the ordinary
  script runner.
- Enable/disable state across all surfaces derives from the same predicate.

### The key insight

**Make every entry point produce a script and run it through the one script
runner.** A menu item is not "call function X" — it is "execute the one-line
script `X arg=value`." A quick-launch button is a stored script. A toolbar button
is a stored script.

The payoff is disproportionate:

- One execution path to debug, instrument, and log.
- Console output, progress reporting, and error surfacing work identically
  everywhere, automatically.
- The user can see exactly what a menu item did, and copy it into their own
  script.
- Adding an entry point for an existing command becomes a data change.

The performance cost of parsing a one-line script is irrelevant next to any real
operation.

### Corollaries

- **Some commands are meaningfully interactive-only** (clear the display buffer,
  show version, open help). Mark them in the definition rather than special-casing
  them in the runner.
- **A single in-flight-job guard belongs at the runner**, not per-entry-point.
  Rejecting a concurrent invocation must produce a *visible error*, not a silently
  ignorable return flag — a flag that callers may forget to check will be
  forgotten by some caller, and the resulting "nothing happened when I clicked"
  bug is unpleasant to track down.
- **Persist user-pinned launcher entries in real storage**, keyed by position,
  and let them be reordered and removed. They are user data.

---

## 4. Declarative Side-Effect Signaling (Action Tokens)

When a backend operation needs the frontend to do something afterward — refresh a
view, open a modal, redraw an overlay, invalidate a cache — **never infer that
from the command's name in frontend code.**

Name-matching (`if (command === "Analyze") refreshResults()`) is fragile in the
worst way: it is invisible to the backend developer who renames the command, it is
scattered across every entry point, and it fails silently.

### The pattern

The backend returns explicit action tokens as *data*:

```json
{ "message": "Analysis complete", "client_actions": ["refresh_results", "open_results_view"] }
```

The frontend has exactly **one** table mapping token to effect, and **every** entry
point dispatches through it:

```js
for (const action of response.client_actions ?? []) {
  ACTIONS[action]?.();
}
```

### Why this earns its keep

- **The backend declares intent; the frontend owns mechanism.** Neither needs to
  know the other's internals.
- **Renaming a command breaks nothing.**
- **Consistent behavior across entry points is structural**, not something each
  new entry point has to remember to replicate.
- **The token list is a small, greppable, documentable contract** — a table in the
  architecture doc with three columns: token, emitted by, frontend effect.

### Extend it to two levels

For a script of many lines, both the individual result and the aggregate job
result should carry tokens. Per-line tokens let a mid-script command trigger an
immediate effect; the job-level aggregate lets the frontend do a single settling
pass at the end rather than thrashing a view once per line.

### Keep tokens coarse

`refresh_results` is a good token. `set_results_table_column_3_sort_descending` is
the frontend's business leaking backward. If a token needs parameters, it is
probably two tokens or a data field.

---

## 5. Shared Mutable State — The Single-Lock Model and Its Ceiling

### The model

All session state in one struct, behind one mutex, passed to every operation.
Every command receives `&mut Context` and can read or write anything.

**This is the correct starting point**, and it should be defended against premature
sophistication:

- Impossible to deadlock against yourself with one lock.
- Impossible to observe a torn or inconsistent view of state.
- No cache-coherence reasoning, no "which lock protects this field" archaeology.
- Every operation gets the full state without threading a dozen parameters.

Fine-grained locking bought before it is needed costs correctness and buys
nothing.

### The ceiling — know it before you hit it

One lock means **one operation at a time, application-wide.** A long-running
operation holding the lock blocks every other operation that needs it. In a UI
application this presents as a freeze.

Two distinct problems, often conflated:

**Problem A: the UI thread blocks.** If the platform dispatches some calls on the
main/UI thread, a call that waits on the lock parks the event loop and the entire
interface freezes — rendering, input, everything.

*Fix:* every call that touches shared state must be dispatched to a worker
thread/runtime rather than the UI thread. Then a waiting call blocks a worker; the
interface stays live. **Make this a stated invariant covering every call added
later**, not a fix applied to the calls that happened to be slow during
development. The failure mode is a *new* call added a year later that quietly
reintroduces the freeze, and the person who fixed it originally has forgotten why
the rule exists.

**Problem B: calls still serialize.** Even dispatched to workers, a query issued
during a long operation does not resolve until that operation finishes, and then
resolves with post-operation state. This is usually acceptable — often it is even
*correct* — but it must be understood and documented, because it produces
"delayed" or "stale" behavior that looks like a bug.

### When to escalate, and how

Escalate only on a measured problem, not on principle. In rough order of cost:

1. **Shorten the critical section.** Extract owned data under the lock, release,
   compute, re-acquire to write back. Often eliminates the problem outright.
2. **Split read from write.** A reader-writer lock, if the access pattern is
   genuinely read-dominated.
3. **Split the state.** Separate locks for genuinely independent substructures —
   at the cost of lock-ordering discipline and the class of bug that comes with
   it.
4. **Message-passing / actor model.** State owned by one task; everything else
   sends messages. Correct at scale, a substantial rewrite.

### A rule worth writing down

**Nothing outside the designated path may depend on cached/resident state being
present.** If a bounded cache exists (see §6), any consumer that is not part of
the path that owns that cache must source its own data independently. A consumer
that "usually finds the data resident" works in testing and fails on a large
workload — the worst possible failure profile.

### Language-specific note

In Rust, extract owned data *before* entering a parallel section. A `&mut`
borrow cannot cross into parallel closures, and discovering this after writing the
parallel body means restructuring it. Plan the extract-compute-writeback shape
from the start.

---

## 6. Parallelism with a User-Adjustable Thread Count

Work-stealing data parallelism (Rayon in Rust, equivalents elsewhere) over an
independent-per-item workload is nearly free performance. Two disciplines make it
sustainable.

### Make the thread count a real, persisted user preference

Default to something reasonable — `cores − 1` is a good default, leaving headroom
for the UI and the OS — but **expose it, persist it, and let it be set to 1.**

Reasons this matters more than it looks:

- **Debugging.** Setting threads to 1 is the fastest way to determine whether a
  bug is a race. This alone justifies the setting.
- **Shared machines.** A user running other work does not want every core
  consumed.
- **Thermal and power limits.** Laptops throttle; users know their machines.
- **Pathological hardware.** Very high core counts sometimes perform *worse* due
  to memory bandwidth saturation or per-thread buffer memory. The user can find
  their own optimum faster than you can predict it.

Mirror the setting into the runtime state at startup and on every change, and let
it be the single source of truth for anything derived from it.

### Bound memory, not just concurrency

**The critical, easily-missed point: peak memory scales with the number of items
in flight, and each in-flight item may be far larger than the input file.**

A decoded, converted, expanded working buffer can be an order of magnitude larger
than the compressed source. Sixteen threads processing 100 MB working buffers is
1.6 GB of transient peak, and the operation that ran fine on the developer's
dataset fails on the user's.

**Deliberately decouple buffer/prefetch depth from thread count.** They answer
different questions: thread count asks "how much compute parallelism," buffer
depth asks "how much memory am I willing to hold." Tying them together means a
user raising thread count for speed silently raises peak memory and gets an
out-of-memory failure they cannot diagnose. Use separate named constants, and
document that the decoupling is deliberate so a future contributor does not
"simplify" it.

### Other notes

- **Aggregation of results must be handled explicitly** — either a reduction, or a
  sequential accumulation step outside the parallel section. Online/streaming
  algorithms (e.g. Welford for mean and variance) let you avoid holding all
  results simultaneously and are worth knowing.
- **Beware non-thread-safe C libraries** reached through bindings. If a dependency
  is not thread-safe, all access to it must funnel through a single point — one
  dedicated thread, or a serializing lock. Discover this by reading the library's
  documentation, not by debugging an intermittent crash.
- **Progress reporting from parallel work** needs an atomic counter, not a
  per-thread callback. Poll it from the frontend on a fixed cadence (500 ms is
  imperceptible and cheap) rather than pushing an event per item.

---

## 7. Bounded Residency and Background Prefetch

Two related patterns for applications processing datasets larger than memory.

### Bounded residency

**Never let working-set size scale with dataset size.** Split "everything about an
item" into cheap metadata (always resident, small, in a registry keyed by
identity) and expensive payload (resident only for a bounded set).

Then define exactly one sanctioned accessor for the payload — a function that
returns it, loading from source on a miss, evicting the least-recently-used entry
beyond capacity. The eviction policy is unremarkable; **the discipline of a single
accessor is the whole pattern.** Multiple accessors mutating the same residency
structure independently produce eviction thrash and heisenbugs.

Rules to write down:

- **One accessor for the resident path; document who is allowed to call it.**
- **Any consumer outside that path loads its own data independently** and never
  assumes residency (see §5).
- **Metadata already loaded is authoritative.** When re-reading an item's payload,
  do not silently overwrite the metadata read earlier — an item changed on disk
  mid-session should not mutate session state behind the user's back.
- **Capacity is a named constant with a stated rationale**, not a literal.

### Background prefetch

Once processing is a loop of *read item → compute on item*, a single background
reader thread working ahead through a caller-supplied ordered list of requests
overlaps the next read with the current compute. Results arrive over a bounded
channel — bounded is the important word, since it is the backpressure that keeps
memory in check.

Notes from experience:

- **A caller-supplied ordered request list, with a per-request kind/variant** for
  the conversion needed, lets one reader serve several consumers with different
  needs. Much better than a reader per consumer.
- **Doing the conversion on the reader thread**, not the consumer thread, is where
  most of the win comes from — the expensive decode/transform overlaps too, not
  just the I/O.
- **A closed channel means "closed," not "all requests fulfilled."** A consumer
  that requires every item (because its output is a function of the complete set)
  must reconcile the count it received against the count it requested. Trusting
  end-of-stream alone produces silently incomplete results — the worst class of
  bug, because nothing errors.
- **Keep the synchronous single-item primitive** the reader is built on. Rare
  one-off consumers should call it directly rather than spinning up a reader for
  one item.
- **Prefetch depth is a memory decision** (see §6). Different consumers may
  warrant different depths — a consumer processing in batches of N wants a deeper
  queue than one processing strictly sequentially.

### Platform note: allocator behavior

On some platforms, an allocator's dynamic tuning can decide that large
allocations should come from the general heap rather than being individually
mapped, at which point freed memory cannot return to the OS while smaller
allocations pin the heap top. The symptom is multi-gigabyte resident memory that
persists after everything has been "cleared," and it looks exactly like a leak
while being nothing of the kind.

If the application routinely allocates and frees large buffers, investigate the
allocator's threshold behavior early. Pinning a threshold so that every
sufficiently large allocation is individually mapped costs one line at startup and
eliminates the entire class of report. More generally: **memory that looks leaked
may be an allocator policy artifact.** Check that before auditing code for leaks.

---

## 8. Measurement Bases and Validation Gates

*Applies to any application producing numbers that users make decisions from —
analysis, metrics, scoring, classification, simulation.*

### Thresholds are calibrated against a measurement basis

If the application computes a value and compares it to a threshold, that threshold
was calibrated against a specific way of computing that value: a specific
preprocessing chain, a specific input representation, a specific algorithm.

**Change any part of the chain and the threshold is unvalidated** — not
necessarily wrong, but no longer known to be right. Refactors, performance
optimizations, and "make these two paths consistent" changes are all fully capable
of altering a measurement basis while every test still passes, because the tests
check that the code runs, not that the numbers still mean what they meant.

### The gate

**Any change to a computation feeding a threshold requires explicit
re-validation before the change is considered complete.** Not a TODO. Not a note
in the issue. A blocking item. If re-validation cannot happen immediately, the
affected thresholds must be marked provisional *in user-facing documentation*,
because the user is making decisions with them.

### Validate against ground truth *before* building a replacement

**The most expensive process error of the project, stated plainly.**

The sequence that costs the most is: suspect the current implementation is wrong →
build a replacement → measure both against ground truth → discover the original
was very good and the replacement worse on nearly every axis → revert.

The correct sequence: **measure the existing implementation against ground truth
first.** It is a fraction of the effort, and it produces one of three outcomes:

- The current implementation is fine and the suspicion was wrong — the work stops
  there, at a small fraction of the cost.
- The current implementation is bad — now you have a quantitative baseline and can
  tell whether a replacement is actually better.
- The current implementation is good on most inputs and bad on one class of input
  — the most likely and most useful outcome, and it reframes the problem from
  "replace the algorithm" to "handle this input class," which is a much smaller
  and better-targeted job.

**A single failing case is not a calibration set.** Tuning to one input risks
every input that currently behaves. Get a second and third failing case before
generalizing from one, and hold the passing cases as regression baselines.

### Corollaries

- **Retain unused-but-correct code that took real effort to get right**, marked
  as unused, with a note on why it is kept. Deleting a validated implementation
  because nothing currently calls it means rebuilding and re-validating it later.
- **Reusing a component tuned for one purpose in another requires re-examining its
  tuning.** Acceptance gates, tolerances, and thresholds calibrated for a
  low-recall use case will silently destroy a high-recall one. The interface being
  compatible says nothing about the calibration being appropriate.
- **A stable multiplicative offset against ground truth is a different problem
  from a rank correlation failure**, and is usually much easier to fix. Separate
  the two before diagnosing.

---

## 9. Process Discipline That Paid For Itself

Practices that measurably reduced defect and rework rates. Framed for working with
an AI development partner, though most apply to any collaboration.

### Confirm root cause before proposing a fix

State the mechanism — the specific function, the specific line, the specific
reason — before writing any code. A fix proposed from a plausible-sounding theory
is a guess with syntax highlighting. When the mechanism cannot be stated, the
correct next step is diagnosis, not a candidate patch.

### Never assume file contents

Re-read a file before editing it, especially one already modified in the same
session. Grep for callers before changing or removing anything non-local. When
working with an AI partner, this is not optional: **a model asked to modify code
it has not seen in the current context will produce something structurally
plausible and specifically wrong** — plausible enough to survive review, wrong
enough to break the build or, worse, to compile and misbehave.

The complementary discipline: **grep the entire repository for call sites before
changing a shared signature.** A sweep done from memory of the architecture misses
the consumers that were added since. This has a habit of surfacing as a broken
build at the worst moment.

### One change at a time

Deliver and verify one discrete change before starting the next. Build/typecheck
after each. When ten changes land together and the build breaks, the bisection
cost exceeds everything the batching saved.

### Bugs found mid-fix get their own issue

Never fold an unrelated discovery silently into the current change. It corrupts
the record of what was changed and why, defeats bisection, and produces commits
whose message describes a fraction of their content.

### Investigation-only work prohibits fixes

When the goal is to understand something, the deliverable is understanding. A
"while I was in there" fix inside an investigation contaminates the measurement
the investigation exists to produce.

### An issue is not done when the code is written

It is done when its own validation has been run and the result reported. "Should
work" is not a result. This is the single most common place where quality is
silently lost.

### Documentation lands in the same change as the code

Not the next commit. The same one. Documentation deferred to a follow-up is
documentation that does not get written, and the drift is invisible — nothing
fails when a reference document describes last month's behavior.

### Keep one authoritative reference document

A single source of truth describing what the system does *today* — not a
changelog, not a history. Everything else points at it. When it and the code
disagree, that is a defect in one of them, and the disagreement is worth
resolving immediately rather than accumulating.

This document is also what makes an AI partner effective across sessions. A model
starting fresh with an accurate architecture reference is dramatically more useful
than one reconstructing the design from code fragments. **Treat the reference
document as infrastructure for collaboration, not as an afterthought for users.**

### Trim scope aggressively

Distinguish bugs from design decisions. Close "nice to have" items that do not
materially affect users, and say so plainly. A backlog of forty open items where
thirty are aspirational is less useful than ten real ones, because nobody trusts
the list.

### Audit in aggregate before declaring stability

Issue-by-issue work cannot find the class of problem that only appears in
aggregate: dead code, inconsistent patterns, missing bounds, duplicated logic,
drift between parallel implementations. Before calling a project stable, do one
dedicated pass across the whole codebase looking specifically for those.

Where possible, have that pass done by a different reviewer than the author —
a newer or stronger model generation, or a different partner entirely. The
rationale is the same reason human teams do not let an author review their own
PR: familiarity with the code is precisely what makes the flaws invisible.

### Know when to revert

When a replacement is measurably worse than what it replaces, revert it — fully,
at version-control level, keeping nothing. The sunk cost is already sunk, and
partially-kept work from a rejected approach is the source of the strangest bugs.

The knowledge gained is the deliverable. Write it down — what was tried, what was
measured, precisely why it failed, and what has therefore been ruled out — and
that record is worth considerably more than the code was. The next attempt starts
from a much better place, and does not spend its first week rediscovering a dead
end.
